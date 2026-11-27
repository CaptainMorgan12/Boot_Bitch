// Round 17 (0.2.32): the in-app PolkitQt1 authentication agent — the
// implementation.  See BootBitchPolkitAgent.h for the conversation contract.
#ifdef BOOT_BITCH_HAVE_POLKIT_AGENT

#include "BootBitchPolkitAgent.h"

#include "polkitqt1-agent-session.h"
#include "polkitqt1-details.h"
#include "polkitqt1-identity.h"
#include "polkitqt1-subject.h"

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDialog>
#include <QLineEdit>
#include <QPointer>
#include <QPushButton>
#include <QTimer>

#include <unistd.h>

namespace {

// polkitd's generic execution action — the fallback action id when the
// helper runs from a source tree without an installed policy file.  The
// app's own conversation may carry either this or the annotated action.
const QString kGenericExecActionId = QStringLiteral("org.freedesktop.policykit.exec");

// The captured polkit-qt warning text of the registration attempt (the DBus
// error polkitd returned — e.g. the refused registration), for the decisive
// round-17b instrumentation.
QString g_capturedAgentWarning;

void agentWarningCaptureHandler(QtMsgType type, const QMessageLogContext &,
                                const QString &message)
{
    if (type != QtWarningMsg && type != QtCriticalMsg) {
        return;
    }
    if (!g_capturedAgentWarning.isEmpty()) {
        g_capturedAgentWarning += QStringLiteral("; ");
    }
    g_capturedAgentWarning += message;
}

// The display name of one identity: the unix-user name without the
// "unix-user:" prefix, or the serialized form for anything else.
QString identityDisplayName(const PolkitQt1::Identity &identity)
{
    const QString serialized = identity.toString();
    const QString prefix = QStringLiteral("unix-user:");
    if (serialized.startsWith(prefix)) {
        return serialized.mid(prefix.size());
    }
    return serialized;
}

} // namespace

BootBitchPolkitAgent::BootBitchPolkitAgent(BootBitchAgentHost *host,
                                           const QString &expectedActionId)
    : m_host(host)
    , m_expectedActionId(expectedActionId)
{
}

BootBitchPolkitAgent::~BootBitchPolkitAgent()
{
    closeDialogQuietly();
    // The listener's destruction unregisters the agent object from the bus
    // (the Listener owns the PolkitAgentListener).
    if (m_result) {
        m_result->setError(QStringLiteral("Boot Bitch agent went away"));
        m_result->setCompleted();
        m_result = nullptr;
    }
}

QString BootBitchPolkitAgent::registerForAppProcess()
{
    if (m_registered) {
        return QString();
    }
    if (!QDBusConnection::systemBus().isConnected()) {
        return QStringLiteral("no system bus connection");
    }
    const QString objectPath = QStringLiteral("/org/bootbitch/PolkitAgent%1")
        .arg(QCoreApplication::applicationPid());
    // Round 17b (the decisive fix + instrumentation): register for the app's
    // OWN unix-process scope.  polkit 126 keeps exactly ONE agent per scope
    // (the first registration wins; a second registration for the same scope
    // is refused with "An authentication agent already exists for the given
    // subject"), and the desktop agent already owns the SESSION scope — the
    // unix-process scope is empty, and polkitd's agent lookup prefers the
    // exact-subject agent over the session agent (the pkexec check's subject
    // IS the app's process), so the app owns exactly its own conversations.
    // Capture the polkit-qt warning (the DBus error text) for the failure
    // detail the caller logs.
    g_capturedAgentWarning.clear();
    const QtMessageHandler previous = qInstallMessageHandler(agentWarningCaptureHandler);
    const bool registered = registerListener(
        PolkitQt1::UnixProcessSubject(static_cast<qint64>(getpid())), objectPath);
    qInstallMessageHandler(previous);
    if (!registered) {
        const QString detail = g_capturedAgentWarning.trimmed();
        return detail.isEmpty() ? QStringLiteral("registration refused") : detail;
    }
    m_registered = true;
    return QString();
}

void BootBitchPolkitAgent::unregisterAgent()
{
    // The listener is unregistered when this object is destroyed (the caller
    // deleteLater()s it right after the conversation resolves, so the
    // destruction — and the unregistration — happens at the end of the
    // event-loop turn, AFTER the pending polkit/glib callbacks have drained).
    // Here we only close any still-pending conversation quietly and mark the
    // agent unregistered so a stray late re-invocation is answered with a
    // failing reply instead of reopening the dialog for a dead conversation.
    m_registered = false;
    closeDialogQuietly();
    m_session = nullptr;
}

void BootBitchPolkitAgent::initiateAuthentication(
    const QString &actionId, const QString &message, const QString &iconName,
    const PolkitQt1::Details &details, const QString &cookie,
    const PolkitQt1::Identity::List &identities, PolkitQt1::Agent::AsyncResult *result)
{
    Q_UNUSED(iconName);
    if (!result) {
        return;
    }
    // Round 17c (the teardown hardening): a stray late re-invocation after
    // the app already tore the conversation down (the unregistration window,
    // the pending deleteLater) must never reopen the password dialog for a
    // dead conversation — answer it with a failing reply.
    if (!m_registered) {
        result->setError(QStringLiteral("Boot Bitch agent already unregistered"));
        result->setCompleted();
        return;
    }
    // A FOREIGN action id arriving while the app's conversation owns the
    // registration window: answer with a failing reply and report it.  The
    // desktop agent owns everything outside this busy window (the
    // multi-agent latest-wins note: our registration is the most recent, so
    // concurrent requests still reach us — failing them cleanly hands the
    // user back to the desktop agent for that action).
    if (actionId != m_expectedActionId && actionId != kGenericExecActionId) {
        result->setError(QStringLiteral("Boot Bitch is not handling this action"));
        result->setCompleted();
        if (m_host) {
            m_host->polkitAgentForeignAction(actionId);
        }
        return;
    }

    // A repeated invocation with the failure detail = the wrong password
    // (polkitd re-invokes the conversation with the same cookie and the
    // authentication error in polkit.message).
    const QString failureMessage = details.lookup(QStringLiteral("polkit.message"));
    if (m_host) {
        m_host->polkitAgentConversationReceived(actionId, !failureMessage.isEmpty());
    }
    if (!failureMessage.isEmpty()) {
        if (m_host) {
            m_host->polkitAgentWrongPassword();
        }
        if (m_retriedOnce) {
            // The second failure stays quiet: fail the conversation without
            // re-opening the dialog (the pkexec failure site then resolves
            // through the chain-wide latch — no second popup, no re-prompt).
            result->setError(failureMessage);
            result->setCompleted();
            return;
        }
        m_retriedOnce = true;
        // Fall through: re-open the password dialog ONCE for the fresh
        // attempt below.
    }

    m_actionMessage = message;
    m_cookie = cookie;
    m_result = result;
    m_identity = identities.isEmpty() ? PolkitQt1::Identity() : identities.first();
    m_dismissalReported = false;

    m_session = new PolkitQt1::Agent::Session(m_identity, m_cookie, m_result, this);
    connect(m_session, &PolkitQt1::Agent::Session::request, this,
            &BootBitchPolkitAgent::onSessionRequest);
    connect(m_session, &PolkitQt1::Agent::Session::completed, this,
            &BootBitchPolkitAgent::onSessionCompleted);
    m_session->initiate();

    // The password dialog: one per conversation, built by the host with the
    // ui-guide styling (the locked-icon pattern).
    m_dialog = m_host ? m_host->polkitAgentCreatePasswordDialog(
                            m_actionMessage, identityDisplayName(m_identity))
                      : nullptr;
    if (m_dialog) {
        m_dialog->setAttribute(Qt::WA_DeleteOnClose, false);
        m_passwordEdit = m_dialog->findChild<QLineEdit *>(
            QStringLiteral("polkitPasswordEdit"));
        m_responsePending = false;
        // Round 17e (the submit-then-wait contract): BOTH the accept button
        // and the Return key invoke the AGENT's submission — never
        // QDialog::accept() (which used to close the dialog without
        // submitting, leaving the polkit conversation stuck at REQUEST with
        // the pkexec child running).  The dialog stays open while the
        // response is pending and closes only through the agent's paths.
        if (QPushButton *accept = m_dialog->findChild<QPushButton *>(
                QStringLiteral("polkitPasswordAcceptButton"))) {
            connect(accept, &QPushButton::clicked, this,
                    &BootBitchPolkitAgent::onDialogAccepted);
        }
        if (m_passwordEdit) {
            connect(m_passwordEdit, &QLineEdit::returnPressed, this,
                    &BootBitchPolkitAgent::onDialogAccepted);
        }
        if (QPushButton *reject = m_dialog->findChild<QPushButton *>(
                QStringLiteral("polkitPasswordRejectButton"))) {
            connect(reject, &QPushButton::clicked, this,
                    &BootBitchPolkitAgent::onDialogRejected);
        }
        connect(m_dialog, &QDialog::rejected, this,
                &BootBitchPolkitAgent::onDialogRejected);
        m_dialog->show();
        m_dialog->raise();
        m_dialog->activateWindow();
    }
}

bool BootBitchPolkitAgent::initiateAuthenticationFinish()
{
    // The authentication request finished (polkitd's teardown notification).
    // Nothing to answer — the Session's completed() signal carries the
    // outcome.
    return true;
}

void BootBitchPolkitAgent::cancelAuthentication()
{
    // polkitd cancelled the request (the pkexec process went away or the
    // conversation timed out): close quietly and mark the conversation
    // dismissed — the failure site resolves the quiet outcome.
    failConversation(QStringLiteral("Cancelled by user"));
    if (m_host && !m_dismissalReported) {
        m_dismissalReported = true;
        m_host->polkitAgentDismissed();
    }
}

void BootBitchPolkitAgent::onSessionRequest(const QString &request, bool echo)
{
    Q_UNUSED(request);
    Q_UNUSED(echo);
    // The PAM "password:" prompt arrived.  The dialog is already visible;
    // focus the password field so the user can answer immediately.
    if (m_passwordEdit) {
        m_passwordEdit->setFocus(Qt::OtherFocusReason);
    }
}

void BootBitchPolkitAgent::onSessionCompleted(bool gainedAuthorization)
{
    if (!m_session) {
        return;
    }
    const bool responseWasSubmitted = m_responsePending;
    m_responsePending = false;
    m_session->deleteLater();
    m_session = nullptr;
    closeDialogQuietly();
    if (!m_result) {
        return;
    }
    if (gainedAuthorization) {
        // The conversation succeeded: pkexec proceeds and the existing
        // SESSION_READY flow takes over.  Do not clear the completion — the
        // result object is owned by the polkit stack.
        m_result->setCompleted();
    } else {
        // Round 17f (0.2.32 — the positive wrong-password evidence): the
        // completed(false) AFTER a submitted response IS the wrong password
        // (the dismissal is the separate onDialogRejected/cancelAuthentication
        // path).  Mark the host so the failure site classifies the attempt as
        // the WrongPassword class (the ONE popup + the ONE automatic
        // re-prompt) — polkitd never re-invokes the conversation with the
        // failure detail on this rig, so the re-invocation branch never
        // fires.  A completed(false) WITHOUT a submitted response (a
        // polkitd-initiated failure) stays plain.
        if (responseWasSubmitted && m_host) {
            m_host->polkitAgentWrongPassword();
        }
        // The password was not accepted: complete with the error (the
        // conversation ends and the pkexec failure site resolves the
        // outcome through the marker set above).
        m_result->setError(QStringLiteral("Authentication failure"));
        m_result->setCompleted();
    }
    m_result = nullptr;
}

void BootBitchPolkitAgent::onDialogAccepted()
{
    // Round 17e (the submit-then-wait contract): submit the password to the
    // polkit session and WAIT — the dialog stays open (the guard prevents a
    // double submission from a second click or the Return key while the
    // response is pending), the field clears, and the accept button disables
    // until the conversation resolves.  The dialog closes only through the
    // agent's paths (the completion / the re-invocation / the reject).
    if (!m_session || !m_passwordEdit || m_responsePending) {
        return;
    }
    const QString password = m_passwordEdit->text();
    m_passwordEdit->clear();
    m_responsePending = true;
    if (QPushButton *accept = m_dialog
            ? m_dialog->findChild<QPushButton *>(
                  QStringLiteral("polkitPasswordAcceptButton"))
            : nullptr) {
        accept->setEnabled(false);
    }
    m_session->setResponse(password);
}

void BootBitchPolkitAgent::onDialogRejected()
{
    // The deliberate dismissal: answer the request as cancelled and let the
    // host resolve QUIETLY (no popup — the round-16 original design).  The
    // double-fire (the reject button's clicked() + the dialog's rejected())
    // reports the dismissal exactly once.
    failConversation(QStringLiteral("Cancelled by user"));
    if (m_host && !m_dismissalReported) {
        m_dismissalReported = true;
        m_host->polkitAgentDismissed();
    }
}

void BootBitchPolkitAgent::closeDialogQuietly()
{
    if (m_dialog) {
        m_dialog->hide();
        m_dialog->deleteLater();
        m_dialog = nullptr;
    }
    m_passwordEdit = nullptr;
}

void BootBitchPolkitAgent::failConversation(const QString &errorText)
{
    // Round 17d (the gdb-pinned crash): Session::cancel() emits the Qt
    // completed() signal SYNCHRONOUSLY — the polkit-qt 0.200.0
    // Session::Private::completed runs Q_EMIT inside the C "completed"
    // callback, which polkit_agent_session_cancel() fires — so
    // onSessionCompleted re-enters THIS frame and nulled m_session, and the
    // second m_session->deleteLater() re-read the nulled member: a null-this
    // QObject::deleteLater dereferencing the d-pointer at offset 8 (the
    // deterministic crash at libQt6Core[1d76db]).  The teardown is now
    // single-path and re-entrancy-proof: the members are captured and NULLED
    // BEFORE the cancel, the nested completed handler early-returns on the
    // nulls, the session and the dialog are each deleteLater()d exactly once
    // from the locals, and the result is completed exactly once here (with
    // the cancelled error — never the double setError/setCompleted, whose
    // second setError would hit the freed GSimpleAsyncResult).
    PolkitQt1::Agent::Session *session = m_session;
    PolkitQt1::Agent::AsyncResult *result = m_result;
    QDialog *dialog = m_dialog;
    m_session = nullptr;
    m_result = nullptr;
    m_dialog = nullptr;
    m_passwordEdit = nullptr;
    if (session) {
        session->cancel();
        session->deleteLater();
    }
    if (dialog) {
        dialog->hide();
        dialog->deleteLater();
    }
    if (result) {
        result->setError(errorText);
        result->setCompleted();
    }
}

#endif // BOOT_BITCH_HAVE_POLKIT_AGENT
