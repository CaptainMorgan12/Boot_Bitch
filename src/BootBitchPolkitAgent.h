// Round 17 (0.2.32 — the decisive authorization round): the IN-APP
// PolkitQt1 authentication agent.  The app registers itself as the session's
// authentication agent for the brief window of its own pkexec conversation,
// so the app OWNS the password dialog and receives the REAL per-attempt
// outcome from polkitd — instead of the desktop agent (the Sparky
// lxqt-policykit-agent pops its own "Authorization failed for some reason"
// box on every failed attempt, which stacked with the app's popup and never
// re-opened the password prompt).
//
// The conversation contract:
//  - initiateAuthentication #1 (no failure detail): show the app's password
//    dialog (the §4 ui-guide styling, the LUKS locked-icon pattern).
//  - a REPEATED initiateAuthentication carrying the failure detail
//    (details["polkit.message"], the wrong password): the host shows the ONE
//    honest popup and this agent re-opens the password dialog ONCE — a
//    second failure stays quiet (the chain-wide latch semantics).
//  - the user dismisses the dialog: the request is answered as cancelled and
//    the host resolves QUIETLY (no popup — the round-16 original design).
//  - a FOREIGN action id arriving while registered is answered with a
//    failing reply and logged (the registration window is the modal busy
//    window; the desktop agent owns everything else).
//
// The UI-test build compiles WITHOUT the PolkitQt1 stack: the host contract
// (BootBitchAgentHost) exists in both builds and the seam drives the same
// outcome callbacks (see ensurePrivilegedSession's BOOT_BITCH_UI_TEST
// branch).
#ifndef BOOT_BITCH_POLKIT_AGENT_H
#define BOOT_BITCH_POLKIT_AGENT_H

#include <QObject>
#include <QString>

class QDialog;

// The agent→host outcome contract (the MainWindow integration point).  Kept
// out of the PolkitQt1 guard so the UI-test seam implements the same
// outcomes without the real polkit stack.
class BootBitchAgentHost
{
public:
    virtual ~BootBitchAgentHost() = default;
    // Builds the conversation's password dialog (the §4 ui-guide styling,
    // the locked-icon pattern from the LUKS dialog): the message line, the
    // identity line, the hidden password field and the accept/reject row,
    // named for the agent's wiring (polkitPasswordDialog /
    // polkitPasswordEdit / polkitPasswordAcceptButton /
    // polkitPasswordRejectButton).  The agent owns + closes it.
    virtual QDialog *polkitAgentCreatePasswordDialog(const QString &message,
                                                     const QString &identityName) = 0;
    // The user dismissed the in-app password dialog (a deliberate cancel):
    // the conversation resolves QUIETLY — no popup, no re-prompt.
    virtual void polkitAgentDismissed() = 0;
    // polkitd re-invoked the conversation with the failure detail (the wrong
    // password): show the ONE honest popup; this agent re-opens its dialog
    // ONCE (a second failure stays quiet — the existing latch semantics).
    virtual void polkitAgentWrongPassword() = 0;
    // A foreign action id arrived while the agent was registered: the request
    // was answered with a failing reply; the host logs the occurrence.
    virtual void polkitAgentForeignAction(const QString &actionId) = 0;
    // The agent received the polkitd conversation (the round-17b decisive
    // instrumentation — the host logs the session-log line with the action id
    // and whether this is the failure-detail retry).
    virtual void polkitAgentConversationReceived(const QString &actionId,
                                                 bool retry) = 0;
};

#ifdef BOOT_BITCH_HAVE_POLKIT_AGENT

#include "polkitqt1-agent-listener.h"

namespace PolkitQt1
{
class Details;
class Identity;
namespace Agent
{
class AsyncResult;
class Session;
}
}
class QLineEdit;

class BootBitchPolkitAgent final : public PolkitQt1::Agent::Listener
{
    Q_OBJECT
public:
    BootBitchPolkitAgent(BootBitchAgentHost *host, const QString &expectedActionId);
    ~BootBitchPolkitAgent() override;

    // Registers this agent with polkitd for the APP'S OWN unix-process scope
    // (round 17b — the decisive fix): polkit 126 keeps exactly ONE agent per
    // scope (first-wins) and the desktop agent already owns the SESSION
    // scope, so a session-scope registration is refused ("An authentication
    // agent already exists for the given subject") and the conversation
    // never reaches the in-app agent.  The unix-process scope is empty and
    // the polkitd agent lookup prefers the exact-subject agent over the
    // session agent — the pkexec check's subject IS the app's own process —
    // so the app owns exactly its own conversations while the desktop agent
    // keeps everything else.  Returns an empty string on success, else the
    // captured failure detail (the caller logs it + degrades to the
    // external-agent conversation).
    QString registerForAppProcess();
    // Closes any pending conversation quietly and unregisters the agent.
    void unregisterAgent();

    // PolkitQt1::Agent::Listener:
    void initiateAuthentication(const QString &actionId, const QString &message,
                                const QString &iconName, const PolkitQt1::Details &details,
                                const QString &cookie,
                                const PolkitQt1::Identity::List &identities,
                                PolkitQt1::Agent::AsyncResult *result) override;
    bool initiateAuthenticationFinish() override;
    void cancelAuthentication() override;

private Q_SLOTS:
    void onSessionRequest(const QString &request, bool echo);
    void onSessionCompleted(bool gainedAuthorization);
    void onDialogAccepted();
    void onDialogRejected();

private:
    void closeDialogQuietly();
    void failConversation(const QString &errorText);

    BootBitchAgentHost *m_host = nullptr;
    QString m_expectedActionId;
    PolkitQt1::Agent::AsyncResult *m_result = nullptr;
    PolkitQt1::Agent::Session *m_session = nullptr;
    QDialog *m_dialog = nullptr;
    QLineEdit *m_passwordEdit = nullptr;
    QString m_actionMessage;
    QString m_cookie;
    PolkitQt1::Identity m_identity;
    bool m_retriedOnce = false;
    bool m_registered = false;
    // Round 17e (the submit-then-wait contract): true while the password
    // response is pending with polkitd — the accept button and the Return
    // key must not submit twice (no double setResponse), and the dialog
    // stays open until the agent closes it.
    bool m_responsePending = false;
    // Round 17c (the teardown hardening): the reject button's clicked() and
    // the dialog's rejected() both fire onDialogRejected, and polkitd's
    // cancelAuthentication can race it — the dismissal is reported to the
    // host exactly ONCE per conversation.
    bool m_dismissalReported = false;
};

#endif // BOOT_BITCH_HAVE_POLKIT_AGENT

#endif // BOOT_BITCH_POLKIT_AGENT_H
