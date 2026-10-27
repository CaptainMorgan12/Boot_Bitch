// LegacyMainWindow implementation. Qt3. See LegacyMainWindow.h.

#include "LegacyMainWindow.h"
#include "DeviceInventory.h"
#include "HelperRunner.h"

#include <qapplication.h>
#include <qcheckbox.h>
#include <qclipboard.h>
#include <qcombobox.h>
#include <qdatetime.h>
#include <qdialog.h>
#include <qdir.h>
#include <qevent.h>
#include <qfiledialog.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qglobal.h>
#include <qgrid.h>
#include <qgroupbox.h>
#include <qimage.h>
#include <qinputdialog.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qobjectlist.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qscrollbar.h>
#include <qscrollview.h>
#include <qsettings.h>
#include <qsplitter.h>
#include <qstatusbar.h>
#include <qtabwidget.h>
#include <qtable.h>
#include <qtextedit.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qtoolbutton.h>
#include <qtooltip.h>
#include <qsignalmapper.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <string>

#ifndef LEGACY_VERSION
#define LEGACY_VERSION "unknown"
#endif

// A QScrollView whose minimumSizeHint() reports a one-row floor instead of the
// inflated minimum AutoOneFit produces. AutoOneFit's minimumSizeHint() stays
// large even after setMinimumHeight, which makes the parent vertical splitter
// lock (the plan frame's minimum exceeds its maximum). The splitter queries
// minimumSizeHint() for the pane floor, so overriding it here lets the handle
// drag the plan frame down to its one-row minimum. See etch-lessons-learned.md
// §6.
class PlanScrollView : public QScrollView
{
public:
    PlanScrollView(QWidget *parent, const char *name = 0)
        : QScrollView(parent, name), m_minimumHeight(0)
    {
    }

    void setMinimumContentHeight(int height)
    {
        m_minimumHeight = height;
    }

    QSize minimumSizeHint() const
    {
        if (m_minimumHeight <= 0) {
            return QScrollView::minimumSizeHint();
        }
        return QSize(QScrollView::minimumSizeHint().width(), m_minimumHeight);
    }

private:
    int m_minimumHeight;
};

// A QTable whose minimumSizeHint() reports a small floor (the header plus a
// couple of rows) instead of its full 18-row content height. The table is
// scrollable, so it can shrink far below its content; the inflated
// minimumSizeHint() would otherwise keep the Capabilities frame tall and lock
// the Settings vertical splitter, so the Full Repair plan frame could never
// expand past its default. Same pattern as PlanScrollView (see
// etch-lessons-learned.md §6).
class CapabilityTable : public QTable
{
public:
    CapabilityTable(int numRows, int numCols, QWidget *parent,
                    const char *name = 0)
        : QTable(numRows, numCols, parent, name), m_minimumHeight(0)
    {
    }

    void setMinimumContentHeight(int height)
    {
        m_minimumHeight = height;
    }

    QSize minimumSizeHint() const
    {
        if (m_minimumHeight <= 0) {
            return QTable::minimumSizeHint();
        }
        return QSize(QTable::minimumSizeHint().width(), m_minimumHeight);
    }

private:
    int m_minimumHeight;
};

namespace legacy {

// The modal repair-result dialog. Its Close button stays disabled and Escape /
// the window-close are refused until the owner calls setCloseAllowed(true), so
// the dialog cannot be silently dismissed while its helper request is still
// running and strand the busy indicator or the completion feedback (modern
// RepairProgressDialog parity: the popup stays with a disabled Close button
// until the request completes). Qt 3.3.7's QDialog::accept()/reject() are
// non-virtual slots, so the two widget-level dismissal paths (the Escape key
// and the window-close) are gated here in keyPressEvent()/closeEvent() instead.
class RepairResultDialog : public QDialog
{
public:
    RepairResultDialog(QWidget *parent, const char *name)
        : QDialog(parent, name, true), m_closeAllowed(false)
    {
    }

    void setCloseAllowed(bool allowed)
    {
        m_closeAllowed = allowed;
    }

protected:
    void closeEvent(QCloseEvent *event)
    {
        if (m_closeAllowed) {
            QDialog::closeEvent(event);
        } else {
            event->ignore();
        }
    }

    void keyPressEvent(QKeyEvent *event)
    {
        // Swallow Escape until the request completes; every other key (and
        // Escape once Close is allowed) falls through to the base handling.
        if (!m_closeAllowed && event->key() == Qt::Key_Escape) {
            event->accept();
            return;
        }
        QDialog::keyPressEvent(event);
    }

private:
    bool m_closeAllowed;
};

// Round 31 (Debian Legacy #9): the in-field greyed placeholder — the Qt 3.3.7
// stand-in for the modern setPlaceholderText().  QLineEdit and QTextEdit have
// no placeholder concept on Qt 3.3.7, so the hint is painted INSIDE the widget
// itself in the palette's disabled-text colour whenever the widget has no
// content; the moment real content lands (typed text or streamed results) the
// hint disappears on its own, exactly like the modern placeholder.  Used by
// the Diagnostics results pane, the Chroot/Host Shell output pane and the
// Shell command field.
static QString placeholderElided(const QString &text, const QFontMetrics &fm,
                                 int width)
{
    if (text.isEmpty() || width <= 0 || fm.width(text) <= width) {
        return text;
    }
    QString clipped = text;
    const QString ellipsis = QString::fromLatin1("...");
    while (!clipped.isEmpty()
           && fm.width(clipped) + fm.width(ellipsis) > width) {
        clipped.truncate(clipped.length() - 1);
    }
    return clipped + ellipsis;
}

// Item #3 (modern parity): the Shell tab command field is single-line, so
// Up/Down are otherwise unused. This QLineEdit subclass forwards them to the
// owning window's session-only command-history navigation and lets every other
// key fall through to QLineEdit (Qt 3.3.7 has no QLineEdit::returnPressed for
// Up/Down and no event-filter convenience, so a keyPressEvent subclass is the
// reliable path). Round 31: it also carries the in-field greyed placeholder
// (see placeholderElided above) — the round-29 hint label above the command
// row is gone.
// Round 72 (ITEM 1, the rig-verified trap): Qt 3.3.7's
// QLineEdit::keyPressEvent emits returnPressed() for Return/Enter but does
// NOT consume the event, so it propagates to the parent dialog — whose key
// handling clicks the FOCUSED button.  In the repair result popup the
// answer commit disables the input, which hands the focus to the armed
// Cancel button, so the propagated Return clicked Cancel and cancelled the
// run right after the answer was committed (the round-72 probe evidence:
// ">> n" followed by "Stopping the running operation..." with exit -1).
// The prompt inputs therefore consume Return/Enter themselves (the answer
// still commits through the returnPressed signal) so the dialog never sees
// the key.  Used by BOTH prompt surfaces (the repair result dialog's input
// and the shell tab's inline input).
class PromptAnswerLineEdit : public QLineEdit
{
public:
    PromptAnswerLineEdit(QWidget *parent, const char *name = 0)
        : QLineEdit(parent, name)
    {
    }

protected:
    void keyPressEvent(QKeyEvent *event)
    {
        if (event->key() == Qt::Key_Return
            || event->key() == Qt::Key_Enter) {
            QLineEdit::keyPressEvent(event);
            event->accept();
            return;
        }
        QLineEdit::keyPressEvent(event);
    }

    void keyReleaseEvent(QKeyEvent *event)
    {
        if (event->key() == Qt::Key_Return
            || event->key() == Qt::Key_Enter) {
            event->accept();
            return;
        }
        QLineEdit::keyReleaseEvent(event);
    }
};

class ShellCommandLineEdit : public QLineEdit
{
public:
    ShellCommandLineEdit(LegacyMainWindow *owner, QWidget *parent,
                         const char *name = 0)
        : QLineEdit(parent, name), m_owner(owner)
    {
    }

    void setPlaceholderText(const QString &text)
    {
        m_placeholder = text;
        update();
    }

    QString placeholderText() const
    {
        return m_placeholder;
    }

protected:
    void keyPressEvent(QKeyEvent *event)
    {
        if (event->key() == Qt::Key_Up) {
            m_owner->stepShellHistory(+1);
            event->accept();
            return;
        }
        if (event->key() == Qt::Key_Down) {
            m_owner->stepShellHistory(-1);
            event->accept();
            return;
        }
        QLineEdit::keyPressEvent(event);
    }

    void paintEvent(QPaintEvent *event)
    {
        QLineEdit::paintEvent(event);
        if (!text().isEmpty() || m_placeholder.isEmpty()) {
            return;
        }
        QPainter painter(this);
        QRect textRect = contentsRect();
        textRect.setLeft(textRect.left() + 2);
        textRect.setRight(textRect.right() - 2);
        // The palette's disabled Text colour is the lighter-grey placeholder
        // tone, independent of the widget's own enabled state.
        painter.setPen(palette().disabled().color(QColorGroup::Text));
        painter.setFont(font());
        const QFontMetrics metrics(font());
        const int baseline = textRect.top()
            + (textRect.height() - metrics.height()) / 2 + metrics.ascent();
        painter.drawText(textRect.left(), baseline,
                         placeholderElided(m_placeholder, metrics,
                                           textRect.width()));
    }

private:
    LegacyMainWindow *m_owner;
    QString m_placeholder;
};

// Round 31 (Debian Legacy #9): the QTextEdit placeholder twin (items 1 and 2).
// QTextEdit paints through QScrollView::drawContents on Qt 3.3.7, so the
// greyed hint is drawn after the document whenever the pane is empty; content
// arriving (results, streamed shell output) hides it without any bookkeeping.
class PlaceholderTextEdit : public QTextEdit
{
public:
    PlaceholderTextEdit(QWidget *parent, const char *name = 0)
        : QTextEdit(parent, name)
    {
    }

    void setPlaceholderText(const QString &text)
    {
        m_placeholder = text;
        viewport()->update();
    }

    QString placeholderText() const
    {
        return m_placeholder;
    }

protected:
    void drawContents(QPainter *p, int clipx, int clipy, int clipw, int cliph)
    {
        QTextEdit::drawContents(p, clipx, clipy, clipw, cliph);
        if (m_placeholder.isEmpty() || !text().isEmpty()) {
            return;
        }
        // Round 38 (Etch #6): the hint is anchored to the TOP of the content,
        // never to the top of the clip.  QScrollView calls drawContents once
        // per exposed region with the region's content-relative clip; the
        // round-31 code drew the hint at (clipx, clipy), so a partial repaint
        // of a LOWER region (for example the rect a closing popup uncovered)
        // drew the hint AGAIN at the clip's top while the earlier paint's
        // hint pixels above stayed visible - the empty pane showed the hint
        // twice.  Drawing only when the clip covers the content origin keeps
        // exactly one instance.
        if (clipy > 0 || clipx > 0) {
            return;
        }
        const QFontMetrics metrics(font());
        // The palette's disabled Text colour is the lighter-grey placeholder
        // tone, independent of the widget's own enabled state.
        p->setPen(palette().disabled().color(QColorGroup::Text));
        p->setFont(font());
        p->drawText(2, metrics.ascent() + 2,
                    placeholderElided(m_placeholder, metrics,
                                      clipw > 4 ? clipw - 4 : 0));
    }

private:
    QString m_placeholder;
};

// Item 3 (modern parity): Qt 3.3.7 QListView draws every column on a single
// line, so the Systems details Value column elided long helper text
// ("Helper-confirmed by the last read-only diagnostics.", "PROTECTED -
// running system; read-only details only"). This QListViewItem subclass
// raises the row height to the value's word-wrapped extent and paints the
// Value column with Qt::WordBreak so long values wrap within the pane
// instead of clipping (the modern Qt6 Selected drive details pane wraps its
// value labels the same way). The Field column and every inherited
// QListViewItem behavior stay untouched.
class WrappingDetailItem : public QListViewItem
{
public:
    WrappingDetailItem(QListView *parent, QListViewItem *after,
                       const QString &field, const QString &value)
        : QListViewItem(parent, after, field, value)
    {
    }

    void setup()
    {
        QListViewItem::setup();
        QListView *view = listView();
        if (!view || view->columns() < 2 || text(1).isEmpty()) {
            return;
        }
        const int margin = view->itemMargin();
        const int columnWidth = view->columnWidth(1) - 2 * margin;
        if (columnWidth <= 0) {
            return;
        }
        const QFontMetrics metrics(view->font());
        // Qt 3.3.7 has no QFontMetrics::boundingRect(QRect, int, QString)
        // (that overload is Qt 4); the equivalent is the 11-arg form
        // (x, y, w, h, flags, text, len...). A large height keeps the text
        // from being truncated, so the returned rect's height is the
        // word-wrapped extent (WordBreak wraps to the given width).
        const QRect wrapped = metrics.boundingRect(
            0, 0, columnWidth, 1000000,
            Qt::AlignLeft | Qt::AlignTop | Qt::WordBreak, text(1));
        const int wanted = wrapped.height() + 2 * margin;
        if (wanted > height()) {
            setHeight(wanted);
        }
    }

    void paintCell(QPainter *p, const QColorGroup &cg, int column,
                   int width, int alignment)
    {
        if (column != 1) {
            QListViewItem::paintCell(p, cg, column, width, alignment);
            return;
        }
        QListView *view = listView();
        const int margin = view ? view->itemMargin() : 2;
        if (isSelected()) {
            p->fillRect(QRect(0, 0, width, height()), cg.highlight());
            p->setPen(cg.highlightedText());
        } else {
            p->setPen(cg.text());
        }
        p->drawText(QRect(margin, 0, width - 2 * margin, height()),
                    Qt::AlignLeft | Qt::AlignTop | Qt::WordBreak, text(1));
    }
};

// Cycle 9 loop 2: --smoke-test settings isolation (set by main() before the
// window is constructed). The flag lives in the legacy namespace so the
// accessors resolve identically everywhere.
static bool g_smokeSettingsIsolation = false;

void legacySetSmokeSettingsIsolation(bool enabled)
{
    g_smokeSettingsIsolation = enabled;
}

bool legacySmokeSettingsIsolation()
{
    return g_smokeSettingsIsolation;
}

// Cycle 16: an initial log directory applied BEFORE the window is
// constructed. The constructor appends its first readiness lines before
// main() can call setLogDirectory(), and a root run whose HOME is preserved
// (sudo env) would otherwise drop those lines into the invoking user's
// ~/.boot-repair-legacy/logs tree as root-owned files the user cannot
// delete. main() publishes the parsed --log-dir here first, mirroring the
// pre-construction smoke settings isolation.
static QString g_initialLogDirectory = QString::null;

void legacySetInitialLogDirectory(const QString &dir)
{
    g_initialLogDirectory = dir;
}

QString legacyInitialLogDirectory()
{
    return g_initialLogDirectory;
}

namespace {

// Forward declaration: maps the helper's capability-unavailable reason key
// (`reason:<key>[|param:...]`) to the modern kReasonSpecs wording, byte-identical.
// Defined later in this file (next to the `reason:<key>` change-status table),
// but called earlier by toolRunReady()/planStageState().
QString localizedCapabilityReason(const QString &raw);

// Layout floors applied by the widget guards and re-checked by the 1024x768
// smoke layout assertion. They are deliberately larger than the raw font
// metrics so KDE's group-box title indent and button/list frame insets cannot
// clip a label on a real desktop.
const int kGroupTitlePadding = 34;
const int kButtonTextPadding = 40;
const int kListHeaderPadding = 26;
const int kLastColumnGutter = 8;
const int kSectionTitlePadding = 10;
// The section-title left-edge probe tolerance: a title's mapped x must sit at
// its container's content-left plus at most the frame and layout margins
// (never centered inside the row).
const int kSectionTitleLeftTolerance = 18;

// Hidden-input dialogs (administrator authorization, LUKS passphrase) are
// width-constrained and word-wrapped: the long explanatory text used to size
// the modal to its single unwrapped line and produced an unusably wide window.
const int kHiddenInputMaximumWidth = 440;
// Round 34 (Etch #4): the delete-log confirmation body width. The round-31/32
// widening chain (440 -> 880 -> ~920 px) made the dialog almost as wide as the
// application; this fixed 320-px text column pins the dialog at 360 px (~35%
// of the 1024-px-wide application window) — the full wrapped text still fits,
// without the over-wide box.
const int kDeleteLogConfirmTextWidth = 320;
// Round 21 (uniform "i" popup sizing): every tab's "i" popup renders at this
// ONE fixed width (and one fixed height derived from the Repair page's "i"
// text — see showHelpPopup()); shorter texts share the frame, longer ones
// scroll inside it.
const int kHelpPopupWidth = 480;

// Round 54 (E4a): the modern session-log retention report.  The legacy GUI
// does not prune its session logs (modern-only behavior), but the string is
// registered for translation so the 18 legacy catalogs carry it and the
// Known-notes backlog is closed; a future legacy retention pass can reuse it.
static const char *const kSessionLogRetentionParity = QT_TRANSLATE_NOOP(
    "LegacyMainWindow",
    "Session log retention removed %1 older file(s); at most 20 files / 10 MB are kept.");
static const char *const kSessionLogRetentionTitleParity =
    QT_TRANSLATE_NOOP("LegacyMainWindow", "Session log retention");

// B5: the guarded `config-write` verb transports the edited content through a
// private mode-600 content file (--content-file <path> --content-owner <uid>,
// never argv), so the editor's cap is the helper's 1 MiB file bound (the old
// 64 KiB cap only guarded Etch's 128 KiB argv limit, which no longer applies).
const int kConfigEditMaximumBytes = 1048576;

// The global header icon is kept compact so the eight tabs still fit at the
// 1024x768 layout contract size.
const int kHeaderIconSize = 32;
// Round 70 (E2): the running-system shield badge renders at the same size the
// modern host card uses (30 px artwork in a 34 px box); the legacy card shows
// the extracted 32x32 atlas PNG 1:1.
const int kShieldBadgeSize = 32;

// The global busy indicator (modern parity) sits in a reserved row below the
// guarded-repair badge and animates by cycling 0..3 trailing dots with a
// plain QTimer while a helper command runs (no threads; Qt 3.3.7 has no
// animated-widget or style-dependent busy rendering to lean on).
const int kBusyAnimationIntervalMs = 400;
// The busy indicator's right edge must be flush with the badge's right edge
// (both are right-aligned in zero-margin rows); layout rounding keeps this
// small tolerance from failing the smoke geometry gate.
const int kBusyBadgeEdgeTolerance = 3;

// The individual repair tools, in the modern Repair page order. `stage` is the
// legacy helper stage; an empty stage marks a display-only row (the helper
// capability exists but this frontend exposes no action for it, fail closed).
// Every stage maps to the capability key of MainWindow::repairToolKeyForStage;
// the title/button text mirror the modern individual-tool wording.
struct ToolSpec {
    const char *key;
    const char *stage;
    const char *title;
    const char *button;
    const char *description;
    const char *planStage;      // "" when the tool is not a Full Repair stage
    const char *planBase;       // base plan status for non-plan tools
    bool write;
    // Host-scope stages other than the package stages run behind the helper's
    // host-maintenance feature gate (`Legacy feature host-maintenance:`).
    bool hostMaintenance;
    // Host-only stages (the legacy display-manager repair) stay disabled for
    // the offline target scope with an exact reason.
    bool hostOnly;
    const char *confirm;
    // Round 32 (Etch #4): the concise per-function button hint, matching the
    // modern updateRepairToolDetails buttonTooltip chain (the modern wording;
    // the dkms/grub rows use the legacy's static Etch-applicable variants).
    const char *tooltip;
};

const ToolSpec toolSpecs[] = {
    { "validate", "validate",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Validate environment"),
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Validate"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Check the selected system's mounts, filesystem metadata, boot files, "
      "mapper consistency and dependency readiness before any repair action. "
      "This is an independent safety preflight rather than an optional Full "
      "Repair stage."),
      "", QT_TRANSLATE_NOOP("LegacyMainWindow", "Always preflight"), false, false, false, 0,
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Run the read-only safety preflight.") },
    { "filesystem", "fs-inspect",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "File system repair"),
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Check File Systems"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Run the read-only file system check on the selected system's root and "
      "/boot filesystems, then repair only the devices that report errors. "
      "The Etch check tools (e2fsck for ext2/ext3) run only after the helper "
      "releases its read-only mounts; a still-mounted filesystem is never "
      "repaired offline and unsupported filesystems are reported rather than "
      "guessed about."),
      "filesystem", "", false, false, false, 0,
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Check and repair the detected file systems.") },
    { "dpkg", "dpkg-configure",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Complete package configuration"),
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Complete Configuration"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Complete interrupted dpkg package configuration in the selected repair "
      "system. This is the same stage controlled by Settings -> Full Repair "
      "plan -> Complete interrupted package configuration, but it can also be "
      "run independently here."),
      "dpkg-configure", "", true, false, false,
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Run the guarded dpkg-configure repair? The helper keeps its package-lock and runtime preflights."),
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Complete interrupted package configuration.") },
    { "fixbroken", "fix-broken",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Repair broken dependencies"),
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Repair Dependencies"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Repair package dependencies in the selected repair system after the "
      "mandatory safety preflight. This maps directly to Settings -> Repair "
      "broken package dependencies."),
      "fix-broken", "", true, false, false,
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Run the guarded fix-broken repair? The helper keeps its simulation-first preflight and runtime guards."),
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Repair broken package dependencies.") },
    { "aptupdate", "apt-update",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Refresh package metadata"),
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Refresh Metadata"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Refresh APT metadata in the selected repair system without upgrading "
      "installed packages. This maps directly to Settings -> Refresh package "
      "metadata."),
      "apt-update", "", true, false, false,
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Refresh package metadata for the selected scope? The helper requires a reachable, trusted APT source."),
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Refresh package metadata.") },
    { "upgrade", "apt-upgrade",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Upgrade installed packages"),
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Simulate and Upgrade"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Simulate the APT transaction first, inspect proposed removals, then "
      "apply a safe upgrade. This maps directly to Settings -> Upgrade "
      "installed packages."),
      "apt-upgrade", "", true, false, false,
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Run the guarded apt-upgrade transaction? The helper keeps its simulation-first and source guards."),
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Simulate and apply a safe package upgrade.") },
    { "kernels", "kernels",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Kernel repair"),
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Repair kernels"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Review the installed kernel inventory and choose one concrete kernel repair action: remove the oldest/superseded or newest kernel, remove orphaned headers/components/build support, install missing headers, reinstall an incomplete kernel, or install the repository-latest kernel. "
      "Nothing is removed or modified until you pick an action and confirm it. "
      "The running kernel is always protected, and the remaining kernels' initramfs images and the bootloader configuration are regenerated after a removal."),
      "", QT_TRANSLATE_NOOP("LegacyMainWindow", "Interactive tool - run individually"), true, false, false,
      0,
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Choose a kernel repair action from the evidence.") },
    { "dkms", "",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "DKMS"),
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Rebuild DKMS"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Rebuild out-of-tree kernel modules for kernels installed in the "
      "selected system. The helper refuses this action when DKMS is not "
      "installed; this legacy frontend exposes no DKMS action."),
      "", "", false, false, false, 0,
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Rebuild DKMS kernel modules.") },
    { "initramfs", "initramfs",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Initramfs"),
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Rebuild Initramfs"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Rebuild initramfs images for the selected repair system only after "
      "mapper and crypttab consistency checks pass. The helper backs up each "
      "image before the apply. On Etch the stage runs through the guarded "
      "plain-chroot fallback (no unshare required)."),
      "initramfs", "", true, true, false,
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Rebuild the initramfs for the selected scope? The helper keeps its mapper/crypttab and backup preflights."),
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Rebuild initramfs images.") },
    { "display", "display-manager",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Graphical login / display manager"),
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Restore Graphical Login"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Restore the legacy SysV display manager configured for the selected "
      "system: the /etc/X11/default-display-manager entry and the missing "
      "runlevel S-symlink, with a backup and rollback, never starting the "
      "GUI. When the boot expects a disk-unlock prompt, the console routing "
      "is also repaired: console=tty0 is appended last to the bootloader "
      "command line (menu.lst on this legacy GRUB; the serial console stays) "
      "so the prompt remains on the visible screen instead of going black. "
      "Available for the running host and a committed repair target."),
      "display-manager", "", true, true, false,
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Restore the graphical login configuration for the selected system? The helper backs up /etc/X11/default-display-manager and the runlevel symlink state, restores the configured entry and the missing S-symlink, rolls back on any failure, and never starts the display manager."),
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Restore the graphical login manager.") },
    { "gpu", "gpu",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "GPU driver repair"),
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Repair GPU driver"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Walk an evidence-guided interactive session to repair the kernel GPU driver (nvidia/nouveau/amdgpu/radeon/mesa), applying only the small guarded step you choose with its own rollback. "
      "Two decision points guide the session: kernel-vs-driver (replace the newest kernel the driver cannot build against, or remove the older kernel the driver is pinned to) and wrong-driver (purge the incorrect driver, install the correct one, and rebuild it against every installed kernel). "
      "A separate one-boot nomodeset fallback reaches a console on a broken driver."),
      "", QT_TRANSLATE_NOOP("LegacyMainWindow", "Interactive tool - run individually"), true, false, false,
      0,
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Choose a GPU driver repair action from the evidence.") },
    { "efi", "",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "EFI / UKI bootloader"),
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Repair EFI / UKI"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Repair the selected system's EFI / UKI boot path. This legacy frontend "
      "exposes no EFI action; the Etch target is a BIOS/GRUB-legacy system."),
      "", "", false, false, false, 0,
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Repair the EFI / UKI boot path.") },
    { "grub", "grub",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "GRUB configuration"),
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Regenerate GRUB"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Regenerate the selected repair system's GRUB menu/configuration after "
      "the mandatory safety preflight. The helper backs up menu.lst, preserves "
      "every existing boot entry and rolls back on any failure."),
      "grub", "", true, true, false,
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Regenerate the GRUB configuration? The helper backs up the target menu/configuration, preserves every existing boot entry and rolls back on any failure."),
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Regenerate the GRUB configuration.") },
    { "extlinux", "",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "extlinux configuration"),
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Regenerate extlinux"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Regenerate the selected system's extlinux bootloader configuration. "
      "This legacy frontend exposes no extlinux action."),
      "", "", false, false, false, 0,
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Regenerate the extlinux configuration.") },
    { "bootstack", "boot-stack",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Boot stack reconciliation"),
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Reconcile Boot Stack"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Reconcile the selected repair system's boot stack in one guarded "
      "legacy pass: mapper/crypttab validation, initramfs rebuild and "
      "GRUB-legacy configuration regeneration, with the component backups "
      "and preflights unchanged. This is the Etch equivalent of the modern "
      "boot-stack reconciliation and stays out of the Full Repair plan."),
      "", QT_TRANSLATE_NOOP("LegacyMainWindow", "Full Repair plan: Manual recovery tool"), true, true, false,
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Run the guarded boot-stack reconciliation? The helper runs the mapper/crypttab validation, the initramfs rebuild and the GRUB-legacy regeneration in one pass with every component preflight and backup."),
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Reconcile the boot stack.") }
};
const int toolSpecCount = sizeof(toolSpecs) / sizeof(toolSpecs[0]);

// B7-4: the change-status keys the helper emits, mapped to the friendly stage
// titles used by the repair summary block.
struct RepairStageTitle {
    const char *key;
    const char *title;
};
const RepairStageTitle repairStageTitles[] = {
    { "validate", QT_TRANSLATE_NOOP("LegacyMainWindow", "Validate") },
    { "filesystem", QT_TRANSLATE_NOOP("LegacyMainWindow", "File system repair") },
    { "dpkg", QT_TRANSLATE_NOOP("LegacyMainWindow", "Complete interrupted package configuration") },
    { "fixbroken", QT_TRANSLATE_NOOP("LegacyMainWindow", "Repair broken package dependencies") },
    { "aptupdate", QT_TRANSLATE_NOOP("LegacyMainWindow", "Refresh package metadata") },
    { "upgrade", QT_TRANSLATE_NOOP("LegacyMainWindow", "Upgrade installed packages") },
    { "dkms", QT_TRANSLATE_NOOP("LegacyMainWindow", "DKMS") },
    { "display", QT_TRANSLATE_NOOP("LegacyMainWindow", "Graphical login / display manager") },
    { "initramfs", QT_TRANSLATE_NOOP("LegacyMainWindow", "Initramfs") },
    { "efi", QT_TRANSLATE_NOOP("LegacyMainWindow", "EFI / UKI bootloader") },
    { "grub", QT_TRANSLATE_NOOP("LegacyMainWindow", "GRUB configuration") },
    { "extlinux", QT_TRANSLATE_NOOP("LegacyMainWindow", "extlinux configuration") },
    { "bootstack", QT_TRANSLATE_NOOP("LegacyMainWindow", "Boot stack reconciliation") },
    { "host-default", QT_TRANSLATE_NOOP("LegacyMainWindow", "Make Default") },
    { "install-host-package", QT_TRANSLATE_NOOP("LegacyMainWindow", "Install missing support package") }
};
const int repairStageTitleCount = sizeof(repairStageTitles) / sizeof(repairStageTitles[0]);

QString repairStageDisplayTitle(const QString &key)
{
    for (int i = 0; i < repairStageTitleCount; ++i) {
        if (key == QString::fromLatin1(repairStageTitles[i].key)) {
            return qApp->translate("LegacyMainWindow",
                                           repairStageTitles[i].title);
        }
    }
    return key;
}

// The Full Repair plan stages supported by the legacy helper, in the order the
// plan runs them (the helper's stage_rank order). The Settings checkboxes and
// the persisted state use the modern-compatible labels and QSettings keys.
// Plan-list parity fix: the row set is the modern Settings plan's capability
// key set (filesystem, dpkg, fixbroken, aptupdate, upgrade, dkms, display,
// initramfs, efi, grub, extlinux); a row is never hidden. Availability follows
// the modern semantics: available = checkable with the saved/default
// preference, unavailable = shown, disabled and unchecked (the reason names
// the exact capability/feature probe outcome). The filesystem row maps to the
// inspect-first `filesystem` plan stage: the frontend runs the read-only
// fs-inspect and the per-device fs-repair verbs itself (split out of the
// `repair` command by runFullRepair), so the row is checkable like any other
// available stage.
struct PlanSpec {
    const char *stage;
    const char *capability;
    const char *title;      // plan list / confirmation title
    const char *checkLabel; // Settings -> Full Repair plan checkbox label
    const char *settingsKey;
    bool defaultChecked;
    // Host-scope stages other than the package stages run behind the helper's
    // host-maintenance feature gate (`Legacy feature host-maintenance:`). The
    // individual-tool run gate (toolRunReady()) still enforces this; the plan
    // checkbox presentation (planStageState()) does not, so these stages stay
    // checkable settings in Host Maintenance regardless of evidence warmth.
    bool hostMaintenance;
    // Host-only stages (the legacy display-manager repair) stay disabled for
    // the offline target scope with an exact reason, so the host-scope
    // selection is never checkable (or overwritable) outside Host Maintenance.
    bool hostOnly;
    // Non-empty when the frontend exposes no plan action for the capability:
    // the row is shown disabled+unchecked with this exact reason even when
    // the helper reports the capability as available (fail closed).
    const char *frontendGap;
};
const PlanSpec planSpecs[] = {
    { "filesystem", "filesystem",
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Repair file system errors (read-only check first)"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Repair file system errors (read-only check first)"),
      "repair/filesystem", true, false, false, "" },
    { "dpkg-configure", "dpkg",
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Complete interrupted package configuration"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Complete interrupted package configuration"),
      "repair/dpkgConfigure", true, false, false, "" },
    { "fix-broken", "fixbroken",
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Repair broken package dependencies"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Repair broken package dependencies"),
      "repair/fixBroken", true, false, false, "" },
    { "apt-update", "aptupdate",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Refresh package metadata"),
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Refresh package metadata"),
      "repair/refreshMetadata", true, false, false, "" },
    { "apt-upgrade", "upgrade",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Upgrade installed packages"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Upgrade installed packages (adaptive APT simulation)"),
      "repair/upgradePackages", false, false, false, "" },
    { "dkms", "dkms",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Rebuild DKMS modules"),
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Rebuild DKMS modules"),
      "repair/dkms", true, true, false, "" },
    { "display-manager", "display",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Restore graphical login manager"),
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Restore graphical login manager"),
      "repair/displayManager", false, true, false, "" },
    { "initramfs", "initramfs",
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Rebuild initramfs after mapper/crypttab validation"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Rebuild initramfs after mapper/crypttab validation"),
      "repair/initramfs", true, true, false, "" },
    { "efi", "efi",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Repair EFI / UKI boot path"),
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Repair EFI / UKI boot path"),
      "repair/efiBootloader", false, true, false, "" },
    { "grub", "grub",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Update GRUB configuration"),
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Update GRUB configuration"),
      "repair/grub", true, true, false, "" },
    { "extlinux", "extlinux",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Update extlinux configuration"),
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Update extlinux configuration"),
      "repair/extlinux", true, true, false, "" }
};
const int planSpecCount = sizeof(planSpecs) / sizeof(planSpecs[0]);

// Index of a legacy stage in planSpecs, or -1.
int planIndexForStage(const char *stage)
{
    if (!stage || !*stage) {
        return -1;
    }
    for (int i = 0; i < planSpecCount; ++i) {
        if (std::strcmp(planSpecs[i].stage, stage) == 0) {
            return i;
        }
    }
    return -1;
}

// Index of a plan capability key in planSpecs, or -1 (the row set must stay
// 1:1 with the modern plan key set; the smoke test and the contract use this
// to pin the parity).
int planIndexForCapability(const char *capability)
{
    if (!capability || !*capability) {
        return -1;
    }
    for (int i = 0; i < planSpecCount; ++i) {
        if (std::strcmp(planSpecs[i].capability, capability) == 0) {
            return i;
        }
    }
    return -1;
}

// Modern Full Repair readiness strings.
const char *const kPlanReadinessDefault =
    QT_TRANSLATE_NOOP("LegacyMainWindow",
    "Run All diagnostics for the selected target or running host before "
    "starting Full Repair. The report is read-only evidence used to choose and "
    "confirm repair stages.");
const char *const kPlanReadinessReady =
    QT_TRANSLATE_NOOP("LegacyMainWindow",
    "Ready: required cached read-only diagnostics are available for the "
    "selected stages. Review them in Diagnostics or Logs before confirming.");

// Read-only diagnostic descriptions for the per-check list; the key list
// itself comes from legacy::diagnosticKeys() so the parser test and the GUI
// cannot drift. The list column carries the stable helper key (the evidence
// name used in the command and the log) while the Selected diagnostic pane
// shows the modern friendly title and description. The tooltip is the short
// per-diagnostic action phrase the Run Diagnostic button shows when that
// check is selected (round 32, Etch #4 — modern per-action button-tooltip
// parity, the legacy wording).
struct DiagnosticSpec {
    const char *key;
    const char *title;
    const char *description;
    const char *tooltip;
};
const DiagnosticSpec diagnosticSpecs[] = {
    { "environment",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Environment validation"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Summarizes the selected system, protection state, mounted identity and inspection readiness."),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "read-only environment and protection-state inspection") },
    { "backend",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Distribution and boot backend profile"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Identifies the distribution family, package manager, initramfs generator, bootloader and current guarded repair capability."),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "read-only distribution and boot backend profiling") },
    { "package",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Package state"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Summarizes pending configuration, broken dependencies and missing package files for every detected package manager without changing the selected system."),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "read-only package state inspection") },
    { "boot",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Boot diagnostics"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Shows boot mounts and /boot contents plus storage evidence without changing the selected system."),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "read-only boot mounts and /boot contents inspection") },
    { "boot-evidence",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Boot evidence and selection history"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Correlates the detected boot chain, bootloader selection, kernel/initramfs, snapshots, EFI and unlock evidence, including whether one or more LUKS prompts are expected."),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "read-only boot chain and unlock evidence correlation") },
    { "kernel",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Kernel / initramfs"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Reviews kernel files and verifies matching initramfs images through a read-only inspection."),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "read-only kernel and initramfs inspection") },
    { "grub",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "GRUB configuration"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Reviews the GRUB configuration without changing boot files."),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "read-only GRUB configuration review") },
    { "uki",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "EFI / UKI boot state"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Inspects EFI/UKI evidence; unavailable on this legacy BIOS frontend with the helper's probe reason."),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "read-only EFI/UKI evidence inspection") },
    { "display",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Graphical login / display manager"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Reviews the configured display manager and recent boot evidence without starting the GUI."),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "read-only display manager review") },
    { "gpu",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Graphics / GPU driver stack"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Reviews the kernel GPU driver: detected hardware, the loaded and "
      "blacklisted modules, installed driver packages, the expected driver, "
      "per-kernel DKMS build status, how far this boot reached and actionable "
      "journal driver errors."),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "read-only GPU driver stack review") },
    { "errors",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Boot errors"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Reads recent error-priority entries from the running host or selected repair system when available."),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "read-only boot error journal read") },
    { "usage",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Disk usage"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Summarizes filesystem capacity and free space for the running host or read-only repair target."),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "read-only disk usage summary") },
    { "filesystem",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "File systems"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Runs the read-only file system check for the selected system's root, /boot and other filesystems."),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "read-only file system check") },
    { "fstab",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "fstab"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Displays the running host or selected repair system's fstab; repair-system inspection is mounted read-only."),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "read-only fstab display") },
    { "btrfs",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Btrfs status"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Shows Btrfs filesystem and subvolume information when the target uses Btrfs."),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "read-only Btrfs status inspection") },
    { "mapper",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Mapper status"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Shows selected mapper ancestry, device-mapper state and cryptsetup status when available."),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "read-only device-mapper state review") },
    { "luks",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "LUKS / crypttab"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Shows LUKS/mapped ancestry plus crypttab and fstab mapper references."),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "read-only LUKS and crypttab review") },
    { "report",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Full diagnostic report"),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "Combines all read-only diagnostics for the selected scope (same as Run All)."),
      QT_TRANSLATE_NOOP("LegacyMainWindow",
      "combined read-only diagnostic report") }
};
const int diagnosticSpecCount = sizeof(diagnosticSpecs) / sizeof(diagnosticSpecs[0]);

// The Logs filter combo, 1:1 with the modern order: All entries / Diagnostics
// / Repairs / the three workflow filters / the 18 diagnostic sections. The
// combo cannot carry per-item data in Qt3, so the index semantics live in
// this single table. `kind` is the LogEntry::Kind filter (or Application for
// an entry that matches nothing), `stage` is the helper stage a Repair entry
// must carry for the workflow filters ("" = no stage check), `diagKey` is the
// diagnostic key a Diagnostic entry must carry for the section filters ("" =
// no key check). The File copy workflow has no lines in this frontend (the
// file-copy feature is greyed) but the entry stays for the 1:1 combo.
struct LogFilterSpec {
    const char *title;
    int kind;            // LogEntry::Kind or Application when unused
    const char *stage;   // single stage match for Repair workflows
    const char *diagKey; // diagnostic-key match for section filters
};
const LogFilterSpec logFilterSpecs[] = {
    { QT_TRANSLATE_NOOP("LegacyMainWindow", "All entries"), LogEntry::Application, "", "" },
    { QT_TRANSLATE_NOOP("LegacyMainWindow", "Diagnostics"), LogEntry::Diagnostic, "", "" },
    { QT_TRANSLATE_NOOP("LegacyMainWindow", "Repairs"), LogEntry::Repair, "", "" },
    { QT_TRANSLATE_NOOP("LegacyMainWindow", "File system repair"), LogEntry::Repair, "fs-inspect", "" },
    { QT_TRANSLATE_NOOP("LegacyMainWindow", "Package repair"), LogEntry::Repair, "package", "" },
    { QT_TRANSLATE_NOOP("LegacyMainWindow", "File copy"), LogEntry::Application, "", "" }
};
const int logFilterSpecCount = sizeof(logFilterSpecs) / sizeof(logFilterSpecs[0]);

// True when the space-separated stage list contains the legacy package stages
// mapped to the "Package repair" workflow (fix-broken, dpkg-configure,
// apt-update, apt-upgrade).
bool stagesContainPackageStage(const QString &stages)
{
    if (stages.find(QString::fromLatin1("fix-broken")) >= 0
        || stages.find(QString::fromLatin1("dpkg-configure")) >= 0
        || stages.find(QString::fromLatin1("apt-update")) >= 0
        || stages.find(QString::fromLatin1("apt-upgrade")) >= 0) {
        return true;
    }
    return false;
}

// Resolves a command through a read-only PATH search (plus the standard system
// directories for launchers with a reduced PATH); the probe never executes
// anything, mirroring CapabilityChecker::findExecutablePortable.
QString findExecutablePath(const QString &command)
{
    if (command.isEmpty()) {
        return QString::null;
    }
    const QString pathValue = QString::fromLocal8Bit(::getenv("PATH"));
    const QStringList dirs = QStringList::split(QChar(':'), pathValue, false);
    static const char *const standardDirs[] = {
        "/usr/local/sbin", "/usr/local/bin", "/usr/sbin", "/usr/bin",
        "/sbin", "/bin"
    };
    for (QStringList::ConstIterator it = dirs.begin(); it != dirs.end(); ++it) {
        if ((*it).isEmpty()) {
            continue;
        }
        const QString candidate = *it + QString::fromLatin1("/") + command;
        if (QFile::exists(candidate) && ::access(candidate.latin1(), X_OK) == 0) {
            return candidate;
        }
    }
    for (int i = 0; i < 6; ++i) {
        const QString candidate = QString::fromLatin1(standardDirs[i])
            + QString::fromLatin1("/") + command;
        if (QFile::exists(candidate) && ::access(candidate.latin1(), X_OK) == 0) {
            return candidate;
        }
    }
    return QString::null;
}

// One /etc/os-release value, read-only; "" when the file is missing or the
// key is absent (the Settings host-capabilities summary probes the running
// host exactly like CapabilityChecker::distributionLabel).
QString readOsReleaseValue(const QString &key)
{
    QFile file(QString::fromLatin1("/etc/os-release"));
    if (!file.open(IO_ReadOnly)) {
        return QString::null;
    }
    QTextStream stream(&file);
    QString line;
    while (!stream.atEnd()) {
        line = stream.readLine().stripWhiteSpace();
        if (line.isEmpty() || line[0] == QChar('#')) {
            continue;
        }
        const int eq = line.find(QChar('='));
        if (eq <= 0 || line.left(eq) != key) {
            continue;
        }
        QString value = line.mid(eq + 1).stripWhiteSpace();
        if (value.length() >= 2 && value[0] == QChar('"')
            && value[value.length() - 1] == QChar('"')) {
            value = value.mid(1, value.length() - 2);
        }
        file.close();
        return value;
    }
    file.close();
    return QString::null;
}

// Human-readable package-manager family for the running host, mirroring
// CapabilityChecker::packageManagerLabel with the same fallbacks.
QString packageManagerLabelForHost()
{
    const QString id = readOsReleaseValue(QString::fromLatin1("ID")).lower();
    const QString like = readOsReleaseValue(QString::fromLatin1("ID_LIKE")).lower();
    // Debian Etch predates /etc/os-release (that file arrived later), so the
    // ID read is empty on Etch; the /etc/debian_version (or /etc/debian_release)
    // presence is the Debian-family fallback that still maps to APT / dpkg.
    const bool debianReleaseFile =
        QFileInfo(QString::fromLatin1("/etc/debian_version")).exists()
        || QFileInfo(QString::fromLatin1("/etc/debian_release")).exists();
    const bool debianLike = id == QString::fromLatin1("debian")
        || id == QString::fromLatin1("ubuntu")
        || id == QString::fromLatin1("tuxedo")
        || id == QString::fromLatin1("linuxmint")
        || id == QString::fromLatin1("pop")
        || id == QString::fromLatin1("elementary")
        || id == QString::fromLatin1("zorin")
        || like.find(QString::fromLatin1("debian")) >= 0
        || like.find(QString::fromLatin1("ubuntu")) >= 0
        || (id.isEmpty() && debianReleaseFile);
    if (debianLike) {
        return QString::fromLatin1("APT / dpkg");
    }
    const bool fedoraLike = id == QString::fromLatin1("fedora")
        || id == QString::fromLatin1("rhel")
        || id == QString::fromLatin1("rocky")
        || id == QString::fromLatin1("almalinux")
        || like.find(QString::fromLatin1("fedora")) >= 0
        || like.find(QString::fromLatin1("rhel")) >= 0;
    if (fedoraLike) {
        return QString::fromLatin1("DNF / RPM");
    }
    const bool archLike = id == QString::fromLatin1("arch")
        || id == QString::fromLatin1("manjaro")
        || id == QString::fromLatin1("endeavouros")
        || id == QString::fromLatin1("garuda")
        || id == QString::fromLatin1("artix")
        || like.find(QString::fromLatin1("arch")) >= 0;
    if (archLike) {
        return QString::fromLatin1("pacman");
    }
    if (id == QString::fromLatin1("alpine")
        || like.find(QString::fromLatin1("alpine")) >= 0) {
        return QString::fromLatin1("apk / OpenRC");
    }
    if (id.startsWith(QString::fromLatin1("opensuse"))
        || id == QString::fromLatin1("suse") || id == QString::fromLatin1("sles")
        || like.find(QString::fromLatin1("suse")) >= 0) {
        return QString::fromLatin1("zypper / RPM");
    }
    return QString::fromLatin1("Unknown / unsupported automatic mapping");
}

// True when `package` has an installable candidate on the running host
// (apt/dpkg): a read-only `apt-cache policy <package>` probe (Etch's apt 0.6)
// that reports a non-empty Candidate version. A missing apt-cache, a
// "Candidate: (none)" answer or an "Unable to locate package" result fails
// closed; nothing is ever installed here.
bool hostPackageInstallable(const QString &package)
{
    if (package.isEmpty()) {
        return false;
    }
    if (findExecutablePath(QString::fromLatin1("apt-cache")).isEmpty()) {
        return false;
    }
    const QString command =
        QString::fromLatin1("apt-cache policy \"%1\" 2>/dev/null").arg(package);
    FILE *pipe = ::popen(command.local8Bit().data(), "r");
    if (!pipe) {
        return false;
    }
    bool installable = false;
    char buffer[512];
    while (::fgets(buffer, sizeof(buffer), pipe)) {
        const QString line = QString::fromLocal8Bit(buffer).stripWhiteSpace();
        if (!line.startsWith(QString::fromLatin1("Candidate:"))) {
            continue;
        }
        const QString value = line.mid(QString::fromLatin1("Candidate:").length())
                                  .stripWhiteSpace();
        // A concrete candidate version means the package is installable from
        // the configured apt sources; "(none)" or an empty value means not.
        installable = !value.isEmpty()
            && value != QString::fromLatin1("(none)");
        break;
    }
    ::pclose(pipe);
    return installable;
}

// True when installing `package` from the Etch APT sources could actually
// deliver `command` on this frontend.  `hostPackageInstallable` only proves the
// PACKAGE NAME is in the repo; it cannot prove the package version it would
// install contains the command.  On Etch the `unshare` row is the concrete
// trap: `util-linux` is present (2.12r) but `unshare` entered util-linux only
// in 2.17, so installing the suggested package can never provide the missing
// command.  The ported helper covers that absence with the guarded
// plain-chroot fallback (legacy/overlay.sh), so the row must never offer
// Install — mirroring the modern frontend, which only offers an install when a
// precise `missing-*` reason proves the package actually resolves the missing
// prerequisite.  Fail closed: unknown command/package pairs are refused here
// only when they are a known cannot-provide case; everything else stays
// governed by the apt-cache candidate probe.
bool legacyPackageProvidesCommand(const QString &package, const QString &command)
{
    Q_UNUSED(package);
    if (command == QString::fromLatin1("unshare")) {
        return false;
    }
    return true;
}

// The installable package for a target-scope missing capability row, derived
// from the cached state's machine-readable `reason:` key exactly like the
// modern CapabilityChecker::capabilitiesFromRepairEvidence. Only the module-
// build (dkms) row maps a `missing-*` build-system reason to a concrete
// package (dkms on Debian-family/Arch, akmods on Fedora/RHEL, akms on
// Alpine); a `*-no-build-tree` reason means the build system is present but
// the matching kernel headers build tree is missing, so no one-click install
// is offered (the headers package is version-specific). Every other row has
// no installable package and fails closed (empty). Actual installability
// against the TARGET's own apt sources is enforced by the helper's guarded
// install-target-package verb (simulation-first, fail-closed) — never a host
// apt-cache probe (hostPackageInstallable below stays host-only).
QString legacyTargetInstallablePackage(const QString &key, const std::string &state)
{
    if (key != QString::fromLatin1("dkms")) {
        return QString::null;
    }
    const QString s = QString::fromLatin1(state.c_str());
    if (!s.startsWith(QString::fromLatin1("unavailable|"))) {
        return QString::null;
    }
    const QString detail = s.mid(QString::fromLatin1("unavailable|").length())
                               .stripWhiteSpace();
    if (!detail.startsWith(QString::fromLatin1("reason:"))) {
        return QString::null;
    }
    const QString reasonKey = detail.mid(QString::fromLatin1("reason:").length());
    const int pipe = reasonKey.find(QString::fromLatin1("|"));
    const QString keyOnly = pipe >= 0 ? reasonKey.left(pipe) : reasonKey;
    if (keyOnly == QString::fromLatin1("missing-akmods")) {
        return QString::fromLatin1("akmods");
    }
    if (keyOnly == QString::fromLatin1("missing-dkms-alpine")) {
        return QString::fromLatin1("akms");
    }
    if (keyOnly == QString::fromLatin1("missing-dkms")
        || keyOnly == QString::fromLatin1("missing-dkms-arch")) {
        return QString::fromLatin1("dkms");
    }
    return QString::null;
}

// Round 48 (user request): the display row's target-scope installable package,
// mirroring the modern CapabilityChecker display branch. The reason must be a
// missing-display-manager reason AND the helper's proven side-channel
// (`Repair capability evidence display-installable: <pkg>|available`) must
// name the package — the suggestion is derived from the DETECTED family/
// backends, never a hardcoded per-distro list. Fail closed otherwise.
QString legacyTargetDisplayInstallablePackage(const QString &key,
                                              const std::string &state,
                                              const std::string &displaySideChannel)
{
    if (key != QString::fromLatin1("display")) {
        return QString::null;
    }
    const QString s = QString::fromLatin1(state.c_str());
    if (!s.startsWith(QString::fromLatin1("unavailable|"))) {
        return QString::null;
    }
    const QString detail = s.mid(QString::fromLatin1("unavailable|").length())
                               .stripWhiteSpace();
    if (!detail.startsWith(QString::fromLatin1("reason:"))) {
        return QString::null;
    }
    const QString reasonKey = detail.mid(QString::fromLatin1("reason:").length());
    const int pipe = reasonKey.find(QString::fromLatin1("|"));
    const QString keyOnly = pipe >= 0 ? reasonKey.left(pipe) : reasonKey;
    if (keyOnly != QString::fromLatin1("no-display-manager")
        && keyOnly != QString::fromLatin1("no-display-manager-unit")
        && keyOnly != QString::fromLatin1("no-openrc-display-manager")) {
        return QString::null;
    }
    const QString sideChannel =
        QString::fromLatin1(displaySideChannel.c_str()).stripWhiteSpace();
    const QString suffix = QString::fromLatin1("|available");
    if (sideChannel.length() <= suffix.length()
        || !sideChannel.endsWith(suffix)) {
        return QString::null;
    }
    const QString package =
        sideChannel.left(sideChannel.length() - suffix.length()).stripWhiteSpace();
    return package.isEmpty() ? QString::null : package;
}

// One read-only host-capability probe row. The consolidated table mirrors the
// modern CapabilityChecker feature list (the 15 repair-tool capabilities plus
// LVM and software RAID) into one row per feature, probing a representative
// command through the read-only PATH search, and keeps the two Etch-specific
// rows (LUKS support and the unshare fallback) the legacy frontend still
// needs. The suggested package is the Etch-era Debian package that provides
// that command; EFI/UKI and snapshot-rollback are omitted because Etch is a
// BIOS/GRUB-legacy system with no btrfs/snapper (see refreshCapabilities()).
struct CapabilitySpec {
    const char *feature;
    const char *command;
    const char *scope;
    const char *package;
    const char *note;
};
const CapabilitySpec capabilitySpecs[] = {
    { QT_TRANSLATE_NOOP("LegacyMainWindow", "Validate environment"), "mount",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Host"), "util-linux",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Read-only preflight of mounts, filesystem metadata, boot files and mapper consistency before any repair") },
    { QT_TRANSLATE_NOOP("LegacyMainWindow", "File system repair"), "fsck",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Target"), "e2fsprogs",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Read-only check of the selected system's root and /boot filesystems, then repair only the devices that report errors (e2fsck for ext2/ext3)") },
    { QT_TRANSLATE_NOOP("LegacyMainWindow", "Complete package configuration"), "dpkg",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Target"), "dpkg",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Completes interrupted dpkg package configuration in the selected repair system") },
    { QT_TRANSLATE_NOOP("LegacyMainWindow", "Repair broken dependencies"), "apt-get",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Target"), "apt",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Repairs broken package dependencies after the simulation-first safety preflight") },
    { QT_TRANSLATE_NOOP("LegacyMainWindow", "Refresh package metadata"), "apt-get",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Target"), "apt",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Refreshes APT metadata without upgrading installed packages") },
    { QT_TRANSLATE_NOOP("LegacyMainWindow", "Upgrade installed packages"), "apt-get",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Target"), "apt",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Simulates the APT transaction, then applies a safe upgrade") },
    { QT_TRANSLATE_NOOP("LegacyMainWindow", "DKMS rebuild"), "dkms",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Target"), "dkms",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Rebuilds out-of-tree kernel modules; required only when the target uses DKMS modules") },
    { QT_TRANSLATE_NOOP("LegacyMainWindow", "Graphical login / display manager"), "update-rc.d",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Host"), "sysv-rc",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Restores /etc/X11/default-display-manager and the missing SysV runlevel S-symlink without starting the GUI") },
    { QT_TRANSLATE_NOOP("LegacyMainWindow", "GPU driver repair"), "lspci",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Host"), "pciutils",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Read-only capability on this frontend; the GPU-driver repair runs through the helper") },
    { QT_TRANSLATE_NOOP("LegacyMainWindow", "Initramfs rebuild"), "update-initramfs",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Target"), "initramfs-tools",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Debian-family initramfs helper; on Etch the rebuild runs through the guarded plain-chroot fallback") },
    { QT_TRANSLATE_NOOP("LegacyMainWindow", "GRUB configuration"), "update-grub",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Target"), "grub",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Regenerates the Etch GRUB-legacy menu.lst configuration (backed up, every entry preserved)") },
    { QT_TRANSLATE_NOOP("LegacyMainWindow", "extlinux configuration"), "extlinux",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Target"), "syslinux",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Alternative bootloader; Etch boots through GRUB-legacy, so extlinux is not part of the default boot path") },
    { QT_TRANSLATE_NOOP("LegacyMainWindow", "Boot stack reconciliation"), "chroot",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Target"), "coreutils",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "One guarded pass (mapper/crypttab validation, initramfs rebuild, GRUB-legacy regeneration) through the plain-chroot fallback") },
    { QT_TRANSLATE_NOOP("LegacyMainWindow", "Kernel repair"), "modprobe",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Target"), "module-init-tools",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Read-only capability on this frontend; kernel removal/cleanup/install run through the helper's interactive menu") },
    { QT_TRANSLATE_NOOP("LegacyMainWindow", "LVM inspection"), "lvs",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Host"), "lvm2",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Optional LVM storage-stack support") },
    { QT_TRANSLATE_NOOP("LegacyMainWindow", "Software RAID"), "mdadm",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Host"), "mdadm",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Optional Linux MD RAID support") },
    { QT_TRANSLATE_NOOP("LegacyMainWindow", "LUKS support"), "cryptsetup",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Host"), "cryptsetup",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Required to unlock encrypted targets") },
    // Legacy-specific row: Etch has no unshare, so the ported helper falls
    // back to a guarded plain chroot.
    { QT_TRANSLATE_NOOP("LegacyMainWindow", "Process namespace isolation"), "unshare",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "Host+Target"), "util-linux",
      QT_TRANSLATE_NOOP("LegacyMainWindow", "The ported helper falls back to a guarded plain chroot when unshare is absent") }
};
const int capabilitySpecCount = sizeof(capabilitySpecs) / sizeof(capabilitySpecs[0]);

// The 7th (Action) column shows the per-row install affordance as TEXT (Qt3
// QTable cannot host a per-row cell widget, so there is no clickable per-row
// button): "Install" for a missing, installable row and the "-" placeholder
// otherwise. Selecting that row enables the Install Missing Support... button
// below, which runs the guarded install-host-package helper verb. ASCII-only:
// Etch's fonts garble the em dash, so a hyphen stands in for the modern "no
// action" placeholder.
const char *const kCapabilityActionCell = "-";

// Etch-era target configuration files offered by the Diagnostics tab's
// "Edit Target File" control. The keys match the legacy helper's guarded
// `config-read`/`config-write` keys (legacy/overlay.sh); per-target
// availability comes from the helper's read-only `Legacy config <key>:`
// diagnostics probe, so absent files are omitted with the helper's reason.
struct ConfigSpec {
    const char *key;
    const char *path;
};
const ConfigSpec configSpecs[] = {
    { "fstab", "/etc/fstab" },
    { "inittab", "/etc/inittab" },
    { "menu-lst", "/boot/grub/menu.lst" },
    { "crypttab", "/etc/crypttab" },
    { "modules", "/etc/modules" },
    { "interfaces", "/etc/network/interfaces" },
    { "sources-list", "/etc/apt/sources.list" },
    { "apt-conf", "/etc/apt/apt.conf" }
};
const int configSpecCount = sizeof(configSpecs) / sizeof(configSpecs[0]);

// Maps a combo entry (a display path) back to its helper key ("" when the
// path is not one of the Etch configuration files).
QString configKeyForPath(const QString &path)
{
    for (int i = 0; i < configSpecCount; ++i) {
        if (path == QString::fromLatin1(configSpecs[i].path)) {
            return QString::fromLatin1(configSpecs[i].key);
        }
    }
    return QString::null;
}

// Qt3 has no QLabel::setWordWrap (that API is Qt4); `Qt::WordBreak` alignment
// is the Qt3 idiom that makes a QLabel wrap its text, and the explicit
// maximum width keeps the dialog from sizing itself to one unwrapped line.
void enableLabelWordWrap(QLabel *label)
{
    if (label) {
        label->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    }
}

// Shared modal-dialog button labels. QApplication::translate is a non-static
// member in Qt3, so these free helpers translate through the qApp instance at
// runtime; the QT_TRANSLATE_NOOP markers make lupdate extract the labels into
// the LegacyMainWindow context (they have no tr() call site of their own).
static const char *const kDialogButtonLabels[] = {
    QT_TRANSLATE_NOOP("LegacyMainWindow", "Cancel"),
    QT_TRANSLATE_NOOP("LegacyMainWindow", "OK"),
    QT_TRANSLATE_NOOP("LegacyMainWindow", "Yes"),
    QT_TRANSLATE_NOOP("LegacyMainWindow", "No")
};

// Modal hidden-input prompt with a width-constrained, word-wrapped label.
// Qt3's QInputDialog sizes itself to the unwrapped text, which made the long
// administrator-authorization wording produce an unusably wide dialog.
QString promptHiddenPassword(QWidget *parent, const QString &title,
                             const QString &text, bool *ok)
{
    QDialog dialog(parent, "legacy-hidden-input", true);
    dialog.setCaption(title);
    QVBoxLayout *layout = new QVBoxLayout(&dialog, 10, 8);
    QLabel *label = new QLabel(text, &dialog);
    enableLabelWordWrap(label);
    label->setMaximumWidth(kHiddenInputMaximumWidth);
    layout->addWidget(label);

    QLineEdit *edit = new QLineEdit(&dialog);
    edit->setEchoMode(QLineEdit::Password);
    edit->setText(QString::null);
    layout->addWidget(edit);

    QHBoxLayout *buttons = new QHBoxLayout(layout);
    buttons->setSpacing(6);
    buttons->addStretch();
    QPushButton *cancel = new QPushButton(
        qApp->translate("LegacyMainWindow", "Cancel"), &dialog);
    QPushButton *accept = new QPushButton(
        qApp->translate("LegacyMainWindow", "OK"), &dialog);
    accept->setDefault(true);
    // Round 72 (modern parity, TUXEDO button order): the positive action
    // sits LEFT of Cancel with the positive default (the modern
    // addUniformDialogButtons [accept][Cancel] convention).
    buttons->addWidget(accept);
    buttons->addWidget(cancel);
    QObject::connect(cancel, SIGNAL(clicked()), &dialog, SLOT(reject()));
    QObject::connect(accept, SIGNAL(clicked()), &dialog, SLOT(accept()));
    QObject::connect(edit, SIGNAL(returnPressed()), &dialog, SLOT(accept()));
    dialog.setMinimumWidth(360);
    dialog.setMaximumWidth(kHiddenInputMaximumWidth + 40);

    if (dialog.exec() != QDialog::Accepted) {
        if (ok) {
            *ok = false;
        }
        return QString::null;
    }
    if (ok) {
        *ok = true;
    }
    return edit->text();
}

// Cycle 10: shared wrapped confirmation dialog. Qt3's QMessageBox sizes
// itself to the unwrapped text, so long repair confirmations stretched past
// the window; this stack dialog wraps the wording (Qt::WordBreak) inside a
// bounded width and keeps the Yes/No (or Yes/Cancel) question semantics.
// Cycle 17: the optional yesLabel/noLabel overrides let a call site mirror a
// specific modern confirmation's exact button text (for example the Full
// Repair plan's "Run Repair"/"Cancel" pair) without losing the wrapping.
// Round 31 (Debian Legacy #9): the optional maxTextWidth lets a call site
// that needs a wider dialog (the delete-log confirmation, whose long
// timestamped file name must fit on one line) double the default cap without
// touching the other call sites.
// Round 32 (Etch #2): the round-31 cap-only widening was a no-op on Qt 3.3.7.
// The label's sizeHint() (sizeForWidth(-1)) wraps at ~80 x-widths and then
// shrinks to 40/20 x-widths for short texts WITHOUT ever consulting
// setMaximumWidth, so the natural dialog size stays ~340-400 px no matter how
// high the caps are raised. A widened dialog must be sized EXPLICITLY: pin
// the dialog to the requested width and compute the body height for the real
// wrap width (the showHelpPopup pattern), so both the full one-line body and
// the window caption are guaranteed visible.
// Round 34 (Etch #4): the delete-log dialog is now sized to the sane fixed
// kDeleteLogConfirmTextWidth column (the round-32 pinning path run at a
// SMALLER width than the default cap, not a larger one): any caller-specified
// maxTextWidth that differs from the default cap takes the explicit-pinning
// branch, so the delete-log confirmation renders at ~35% of the application
// width instead of ~90%.
bool confirmWrapped(QWidget *parent, const QString &title, const QString &text,
                    bool cancelInsteadOfNo = false,
                    const QString &yesLabel = QString::null,
                    const QString &noLabel = QString::null,
                    int maxTextWidth = kHiddenInputMaximumWidth,
                    bool noIsDefault = false)
{
    QDialog dialog(parent, "legacy-confirm", true);
    dialog.setCaption(title);
    QVBoxLayout *layout = new QVBoxLayout(&dialog, 10, 8);
    QLabel *label = new QLabel(text, &dialog);
    enableLabelWordWrap(label);
    label->setMaximumWidth(maxTextWidth);
    layout->addWidget(label);
    QHBoxLayout *buttons = new QHBoxLayout(layout);
    buttons->setSpacing(6);
    buttons->addStretch();
    QPushButton *noButton = new QPushButton(
        !noLabel.isEmpty()
            ? noLabel
            : (cancelInsteadOfNo
                ? qApp->translate("LegacyMainWindow", "Cancel")
                : qApp->translate("LegacyMainWindow", "No")),
        &dialog);
    QPushButton *yesButton = new QPushButton(
        !yesLabel.isEmpty()
            ? yesLabel
            : qApp->translate("LegacyMainWindow", "Yes"),
        &dialog);
    // Round 72 (modern parity, TUXEDO button order): the POSITIVE action
    // sits LEFT of No/Cancel, exactly like the modern runConfirmDialog
    // [action][Cancel] row.  The default follows the modern dialog family:
    // a confirmation defaults to the SAFE button (Cancel/No — the modern
    // runConfirmDialog sets autoDefault(false) on the action and makes
    // Cancel the default + focus), so every call site now passes
    // noIsDefault=true; the input-dialog family (the modern
    // addUniformDialogButtons) keeps the positive default and lives in its
    // own builders (promptHiddenPassword, the config editor, the pickers).
    if (noIsDefault) {
        noButton->setDefault(true);
        yesButton->setDefault(false);
        noButton->setFocus();
        // The safe default owns the focus; re-assert on a later
        // event-loop turn (the round-27 trap: the dialog's show-time focus
        // pass can revert a pre-show setFocus() — the 0-timer can fire
        // before the posted show-focus events, so the re-assert waits until
        // the show pass has settled).
        QTimer::singleShot(100, noButton, SLOT(setFocus()));
    } else {
        yesButton->setDefault(true);
    }
    buttons->addWidget(yesButton);
    buttons->addWidget(noButton);
    QObject::connect(noButton, SIGNAL(clicked()), &dialog, SLOT(reject()));
    QObject::connect(yesButton, SIGNAL(clicked()), &dialog, SLOT(accept()));
    dialog.setMinimumWidth(360);
    if (maxTextWidth != kHiddenInputMaximumWidth) {
        // The explicitly-sized call: size the dialog to the requested width
        // (the round-32 note above). The body height is the text wrapped at
        // the requested width plus the dialog chrome (margins + spacing +
        // button row).
        QFontMetrics metrics(dialog.fontMetrics());
        const QRect wrapped = metrics.boundingRect(
            0, 0, maxTextWidth, 1000000,
            Qt::WordBreak | Qt::AlignLeft | Qt::AlignTop, label->text());
        const int bodyHeight = QMAX(wrapped.height(), metrics.lineSpacing());
        const int chromeHeight = 2 * layout->margin() + layout->spacing()
            + buttons->sizeHint().height();
        dialog.setFixedSize(maxTextWidth + 40, bodyHeight + chromeHeight);
    } else {
        dialog.setMaximumWidth(kHiddenInputMaximumWidth + 40);
    }
    return dialog.exec() == QDialog::Accepted;
}

QString fromStd(const std::string &text)
{
    return QString::fromLatin1(text.c_str());
}

std::string toStd(const QString &text)
{
    return std::string(text.latin1());
}

// Creates every missing path component with `mode` (mkdir); an existing
// component must be a directory or the whole call fails. The leaf itself is
// never chmod'ed here: callers that need a re-tightened leaf do it
// explicitly and only for the GUI's own private paths.
bool ensureDirectory(const QString &path, mode_t mode)
{
    if (path.isEmpty()) {
        return false;
    }
    QString current;
    const QStringList parts = QStringList::split(QChar('/'), path, false);
    if (!path.startsWith(QChar('/'))) {
        current = QString::fromLatin1(".");
    }
    for (QStringList::ConstIterator it = parts.begin(); it != parts.end(); ++it) {
        if ((*it).isEmpty()) {
            continue;
        }
        current += QString::fromLatin1("/") + *it;
        if (::mkdir(current.latin1(), mode) != 0) {
            struct stat info;
            if (::stat(current.latin1(), &info) != 0 || !S_ISDIR(info.st_mode)) {
                return false;
            }
        }
    }
    return true;
}

// True when `path` is the GUI's own default log tree
// (~/.boot-repair-legacy/logs or below it). Only that private tree is ever
// chmod'ed by the GUI: a user-supplied --log-dir may be a shared path and
// keeps the permissions its owner chose.
bool isDefaultLogTree(const QString &path)
{
    const QString homeLogs = QDir::homeDirPath()
        + QString::fromLatin1("/.boot-repair-legacy/logs");
    return path == homeLogs
        || path.startsWith(homeLogs + QString::fromLatin1("/"));
}

// The pending secret-file paths for the termination handlers, kept in plain
// char buffers (never QString/QByteArray) so a handler only touches
// async-signal-safe primitives: unlink() and _exit(). The secret-file writer
// registers the created file (unlock keyfile or config-write content file),
// discardUnlockKeyfile()/discardConfigContentFile() clear it.
static char gUnlockKeyfilePath[PATH_MAX];
static char gConfigContentFilePath[PATH_MAX];

extern "C" void legacyKeyfileTerminationHandler(int signalNumber)
{
    if (gUnlockKeyfilePath[0] != '\0') {
        ::unlink(gUnlockKeyfilePath);
    }
    if (gConfigContentFilePath[0] != '\0') {
        ::unlink(gConfigContentFilePath);
    }
    ::_exit(128 + signalNumber);
}

// Installs SIGTERM/SIGINT/SIGHUP handlers once, so a terminated GUI never
// leaves a secret file behind: the handler unlinks the pending unlock
// keyfile and the pending config-write content file (best-effort) and exits
// with the conventional 128+signum status.
void installKeyfileTerminationHandlers()
{
    struct sigaction action;
    ::memset(&action, 0, sizeof(action));
    action.sa_handler = legacyKeyfileTerminationHandler;
    ::sigemptyset(&action.sa_mask);
    const int handledSignals[] = { SIGTERM, SIGINT, SIGHUP };
    for (std::size_t i = 0;
         i < sizeof(handledSignals) / sizeof(handledSignals[0]); ++i) {
        ::sigaction(handledSignals[i], &action, 0);
    }
}

QString mapValue(const QMap<QString, QString> &map, const QString &key)
{
    const QMap<QString, QString>::const_iterator it = map.find(key);
    return it == map.end() ? QString::null : it.data();
}

QString detailOrDash(const QString &text)
{
    return text.stripWhiteSpace().isEmpty() ? QString::fromLatin1("-") : text;
}

QString diagnosticDescription(const QString &key)
{
    for (int i = 0; i < diagnosticSpecCount; ++i) {
        if (key == QString::fromLatin1(diagnosticSpecs[i].key)) {
            return qApp->translate("LegacyMainWindow",
                                           diagnosticSpecs[i].description);
        }
    }
    return qApp->translate("LegacyMainWindow",
                                   "read-only helper diagnostic");
}

// Round 32 (Etch #4): the short per-diagnostic action phrase for the Run
// Diagnostic button tooltip; an unknown key falls back to the generic hint.
QString diagnosticTooltip(const QString &key)
{
    for (int i = 0; i < diagnosticSpecCount; ++i) {
        if (key == QString::fromLatin1(diagnosticSpecs[i].key)) {
            return qApp->translate("LegacyMainWindow",
                                           diagnosticSpecs[i].tooltip);
        }
    }
    return qApp->translate("LegacyMainWindow",
                                   "read-only helper diagnostic");
}

QString diagnosticTitle(const QString &key)
{
    for (int i = 0; i < diagnosticSpecCount; ++i) {
        if (key == QString::fromLatin1(diagnosticSpecs[i].key)) {
            return qApp->translate("LegacyMainWindow",
                                           diagnosticSpecs[i].title);
        }
    }
    return key;
}

// The helper frames every diagnostic section and its title with this exact
// divider. Section extraction relies on the framing to keep the captured
// header and metadata intact (ported from MainWindow::isDiagnosticSectionDivider).
bool isDiagnosticSectionDivider(const QString &line)
{
    return line == QString::fromLatin1("========================================");
}

// Line index where the embedded section whose "Diagnostic:" header is at
// diagnosticLine begins. Helper output frames the title between two dividers
// directly above the header, while the GUI framing places the opening divider
// directly above the header instead (ported from
// MainWindow::embeddedSectionStartLine).
int embeddedSectionStartLine(const QStringList &lines, int diagnosticLine)
{
    if (diagnosticLine >= 3
        && isDiagnosticSectionDivider(lines[diagnosticLine - 1])
        && !lines[diagnosticLine - 2].isEmpty()
        && !isDiagnosticSectionDivider(lines[diagnosticLine - 2])
        && isDiagnosticSectionDivider(lines[diagnosticLine - 3])) {
        return diagnosticLine - 3;
    }
    if (diagnosticLine >= 1
        && isDiagnosticSectionDivider(lines[diagnosticLine - 1])) {
        return diagnosticLine - 1;
    }
    return diagnosticLine;
}

// Extracts every embedded diagnostic section from a combined report, keyed by
// the section's `Diagnostic: <key>` header (case-folded). Each section keeps
// its header, metadata and framing exactly as captured, ending where the next
// section's frame begins (ported from
// MainWindow::extractEmbeddedDiagnosticSections).
QMap<QString, QString> extractDiagnosticSections(const QString &entry)
{
    QMap<QString, QString> sections;
    const QString headerPrefix = QString::fromLatin1("Diagnostic: ");
    const QStringList lines = QStringList::split(QChar('\n'), entry, true);
    const int lineCount = static_cast<int>(lines.count());
    std::vector<int> headers;
    for (int index = 0; index < lineCount; ++index) {
        if (lines[index].startsWith(headerPrefix)) {
            headers.push_back(index);
        }
    }
    const int headerCount = static_cast<int>(headers.size());
    for (int position = 0; position < headerCount; ++position) {
        const int header = headers[position];
        const QString key = lines[header].mid(headerPrefix.length()).stripWhiteSpace();
        const int start = embeddedSectionStartLine(lines, header);
        int end = position + 1 < headerCount
            ? embeddedSectionStartLine(lines, headers[position + 1])
            : lineCount;
        while (end > start && lines[end - 1].stripWhiteSpace().isEmpty()) {
            --end;
        }
        if (end > start) {
            QStringList body;
            for (int i = start; i < end; ++i) {
                body.append(lines[i]);
            }
            sections.insert(key.lower(), body.join(QString::fromLatin1("\n")));
        }
    }
    return sections;
}

// Qt 3.3.7 has no QByteArray::toBase64, so the repair-run baseline is
// serialized with a local encoder over the section's UTF-8 bytes, matching
// MainWindow's `QString::fromLatin1(section.toUtf8().toBase64())`. The helper
// decodes with `base64 -d` and parses the original text back.
QString legacyBase64Encode(const QString &text)
{
    static const char kTable[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    const QByteArray bytes = text.utf8();
    const char *data = bytes.data();
    const unsigned int length = static_cast<unsigned int>(bytes.size());
    QCString out;
    for (unsigned int i = 0; i < length; i += 3) {
        const unsigned int a = static_cast<unsigned char>(data[i]);
        const unsigned int b = (i + 1 < length)
            ? static_cast<unsigned char>(data[i + 1]) : 0u;
        const unsigned int c = (i + 2 < length)
            ? static_cast<unsigned char>(data[i + 2]) : 0u;
        out += kTable[(a >> 2) & 0x3Fu];
        out += kTable[((a << 4) | (b >> 4)) & 0x3Fu];
        out += (i + 1 < length) ? kTable[((b << 2) | (c >> 6)) & 0x3Fu] : '=';
        out += (i + 2 < length) ? kTable[c & 0x3Fu] : '=';
    }
    return QString::fromLatin1(out.data(), static_cast<int>(out.size()));
}

// Qt 3.3.7 has no QByteArray::fromBase64, so the PROMPT menu payload is
// decoded with a local decoder (the inverse of legacyBase64Encode).  The menu
// text is only ever displayed, never interpreted, so a malformed payload fails
// closed to an empty result rather than surfacing partial text.
QByteArray legacyBase64Decode(const QString &encoded)
{
    static const char kTable[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int lookup[256];
    for (int i = 0; i < 256; ++i) {
        lookup[i] = -1;
    }
    for (int i = 0; i < 64; ++i) {
        lookup[static_cast<unsigned char>(kTable[i])] = i;
    }
    const QCString text = encoded.latin1();
    // Qt 3's QByteArray/QCString::size() counts the terminating NUL, so the
    // loop bound must exclude it: an unpadded payload (length % 4 == 0, no
    // '=' terminator) would otherwise iterate one byte too far and fail
    // closed on the NUL, returning an empty result for a valid payload.
    int len = static_cast<int>(text.size());
    if (len > 0 && text[len - 1] == '\0') {
        --len;
    }
    QCString out;
    int buffer = 0;
    int bits = 0;
    for (int i = 0; i < len; ++i) {
        const unsigned char c = static_cast<unsigned char>(text[i]);
        if (c == '=') {
            break;  // padding terminates the quad
        }
        if (c == '\n' || c == '\r' || c == ' ' || c == '\t') {
            continue;  // tolerate stray transport whitespace
        }
        const int value = lookup[c];
        if (value < 0) {
            return QByteArray();  // invalid character: fail closed
        }
        buffer = (buffer << 6) | value;
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            out += static_cast<char>((buffer >> bits) & 0xFF);
        }
    }
    return out;
}

} // namespace (closed here: the round-30 member definitions follow at namespace scope)

// Forward declaration: defined below (the round-27 merged-report section
// removal, consumed by the round-30 mapped invalidation).
QString removeEmbeddedDiagnosticSection(const QString &bundle, const QString &key);

// Ported from MainWindow::requiredDiagnosticSectionsForTool: the cached
// diagnostic sections a repair consumes (the repair-run gate and the baseline
// serialization both use this list). "report" means the combined report.
// Round 30 (modern parity): the requires column now comes from the shared
// data file (data/repair-diagnostic-map.tsv); an unknown key and the plan
// rule rows keep the conservative {report} default, and a row without a
// baseline ("-") has no required sections.
QStringList LegacyMainWindow::requiredDiagnosticSectionsForTool(
    const QString &toolKey) const
{
    const std::vector<std::string> required =
        mapRequiredSections(m_repairDiagnosticMap, toStd(toolKey));
    QStringList result;
    for (std::size_t i = 0; i < required.size(); ++i) {
        result.append(fromStd(required[i]));
    }
    return result;
}

// ===== Round 30 (modern parity): the shared repair->diagnostic map ==========

namespace {

QStringList mapSectionsToQStringList(const std::vector<std::string> &sections)
{
    QStringList result;
    for (std::size_t i = 0; i < sections.size(); ++i) {
        result.append(QString::fromLatin1(sections[i].c_str()));
    }
    return result;
}

void appendUniqueSections(QValueList<QString> *target, const QValueList<QString> &more)
{
    for (QValueList<QString>::ConstIterator it = more.begin(); it != more.end(); ++it) {
        if (!target->contains(*it)) {
            target->append(*it);
        }
    }
}

bool startsWithToken(const std::string &text, const char *prefix)
{
    const std::size_t length = std::strlen(prefix);
    return text.size() >= length && text.compare(0, length, prefix) == 0;
}

bool repairChangeStatusIsSkipped(const std::string &status)
{
    return status == "skipped" || startsWithToken(status, "skipped|");
}

// Ported from MainWindow::invalidatingRepairToolKeys: the requested stages
// without a proven unchanged/skipped status plus every reported status key
// that is not proven unchanged/skipped (fail safe: a missing status counts
// as a change, and a later proven-unchanged line never overrides an earlier
// change).
QStringList invalidatingRepairToolKeys(const ParsedTranscript &parsed,
                                       const QStringList &requestedStages)
{
    QMap<QString, QString> statuses;
    for (std::size_t i = 0; i < parsed.changeStatuses.size(); ++i) {
        const QString key =
            fromStd(repairToolKeyForStage(parsed.changeStatuses[i].first));
        const QString state = fromStd(parsed.changeStatuses[i].second);
        if (statuses.contains(key)
            && !changeStatusIsUnchanged(toStd(statuses[key]))
            && changeStatusIsUnchanged(toStd(state))) {
            continue;
        }
        statuses.insert(key, state);
    }
    QStringList keys;
    for (QStringList::ConstIterator it = requestedStages.begin();
         it != requestedStages.end(); ++it) {
        const QString toolKey =
            fromStd(repairToolKeyForStage(toStd(*it)));
        const QMap<QString, QString>::const_iterator found =
            statuses.find(toolKey);
        if (found != statuses.end()
            && (changeStatusIsUnchanged(toStd(found.data()))
                || repairChangeStatusIsSkipped(toStd(found.data())))) {
            continue;
        }
        if (!keys.contains(toolKey)) {
            keys.append(toolKey);
        }
    }
    for (QMap<QString, QString>::ConstIterator it = statuses.begin();
         it != statuses.end(); ++it) {
        if (changeStatusIsUnchanged(toStd(it.data()))
            || repairChangeStatusIsSkipped(toStd(it.data()))
            || keys.contains(it.key())) {
            continue;
        }
        keys.append(it.key());
    }
    return keys;
}

// Ported from MainWindow::repairTranscriptHasChangedStatus: any
// `Repair change status <key>: changed|...` line before the abort.
bool repairTranscriptHasChangedStatus(const ParsedTranscript &parsed)
{
    for (std::size_t i = 0; i < parsed.changeStatuses.size(); ++i) {
        const std::string &state = parsed.changeStatuses[i].second;
        if (state == "changed" || startsWithToken(state, "changed|")) {
            return true;
        }
    }
    return false;
}

// The last change-status state for one key ("" when absent).
std::string changeStatusOf(const ParsedTranscript &parsed, const std::string &key)
{
    std::string state;
    for (std::size_t i = 0; i < parsed.changeStatuses.size(); ++i) {
        if (parsed.changeStatuses[i].first == key) {
            state = parsed.changeStatuses[i].second;
        }
    }
    return state;
}

// The helper's `Shell command class: <class>` line ("" when absent or
// malformed - the modern regex requires a lowercase-letter/hyphen class).
std::string shellCommandClassOf(const QString &transcript)
{
    const QStringList lines = QStringList::split(QChar('\n'), transcript, false);
    const QString prefix = QString::fromLatin1("Shell command class: ");
    for (QStringList::ConstIterator it = lines.begin(); it != lines.end(); ++it) {
        const QString line = (*it).stripWhiteSpace();
        if (!line.startsWith(prefix)) {
            continue;
        }
        const QString candidate = line.mid(prefix.length()).stripWhiteSpace();
        if (candidate.isEmpty()) {
            continue;
        }
        bool valid = true;
        for (int i = 0; i < static_cast<int>(candidate.length()); ++i) {
            const QChar c = candidate[i];
            if (!((c >= QChar('a') && c <= QChar('z')) || c == QChar('-'))) {
                valid = false;
                break;
            }
        }
        if (valid) {
            return toStd(candidate);
        }
    }
    return std::string();
}

// True when at least one stage's map row carries a non-empty affected column:
// a genuine (non-cancelled) failure of a MODIFYING action forces the complete
// set, while a read-only row (validate, fs-inspect) invalidates nothing. An
// empty stage set or an unknown stage fails safe (modifying).
bool stagesAreModifying(const RepairDiagnosticMap &map, const QStringList &stages)
{
    if (stages.isEmpty()) {
        return true;
    }
    for (QStringList::ConstIterator it = stages.begin(); it != stages.end(); ++it) {
        const RepairDiagnosticMapRow *row =
            map.find(repairToolKeyForStage(toStd(*it)));
        if (!row || row->affectedSections != "-") {
            return true;
        }
    }
    return false;
}

} // namespace

bool LegacyMainWindow::applyCommandTranscriptMapped(const ParsedTranscript &parsed,
                                                    bool commandOk, bool cancelled,
                                                    bool shell)
{
    QValueList<QString> affected;
    bool full = false;
    if (shell) {
        // Rounds 16/18/19/20 (modern parity): a proven no-op invalidates
        // nothing; a cancelled shell with no change before the abort
        // invalidates nothing; otherwise the class row decides
        // (missing/unclassifiable -> the conservative complete set).
        const std::string shellState = changeStatusOf(parsed, "shell");
        if (!shellState.empty() && changeStatusIsUnchanged(shellState)) {
            return false;
        }
        if (cancelled && !repairTranscriptHasChangedStatus(parsed)) {
            return false;
        }
        const std::vector<std::string> mapped =
            mapShellClassAffectedSections(m_repairDiagnosticMap,
                                          shellCommandClassOf(m_transcript));
        if (mapSectionsAreComplete(mapped)) {
            full = true;
        } else if (mapped.empty()) {
            return false; // the documented no-op class: nothing
        } else {
            affected = mapSectionsToQStringList(mapped);
        }
    } else if (m_pendingVerb == QString::fromLatin1("copy")) {
        // File copy: the helper's `filecopy` verdict is the single source of
        // truth; a proven-unchanged copy keeps the cache, anything else
        // (including a failure) keeps the fail-safe complete set.
        const std::string copyState = changeStatusOf(parsed, "filecopy");
        if (!copyState.empty() && changeStatusIsUnchanged(copyState)) {
            return false;
        }
        full = true;
    } else if (m_pendingVerb == QString::fromLatin1("copy-preview")
               || m_pendingVerb == QString::fromLatin1("browse-target")) {
        return false; // read-only
    } else if (!commandOk && !cancelled) {
        // A genuine failure of a modifying action may have completed an
        // earlier stage before stopping: the complete set (fail safe). A
        // read-only action (the validate/fs-inspect verbs, a read-only map
        // row, unlock/config/browse traffic) invalidates nothing — modern
        // only invalidates the modifying repair/copy verbs on failure.
        const QString verb = m_pendingVerb;
        const bool modifyingVerb = verb == QString::fromLatin1("repair")
            || verb == QString::fromLatin1("host-repair")
            || verb == QString::fromLatin1("fs-repair")
            || verb == QString::fromLatin1("host-fs-repair");
        full = modifyingVerb
            && stagesAreModifying(m_repairDiagnosticMap, m_activeRepairStages);
        if (!full) {
            return false;
        }
    } else if (cancelled) {
        if (!repairTranscriptHasChangedStatus(parsed)) {
            return false; // nothing changed before the abort: the cache stays valid
        }
        if (m_activeRepairStages.isEmpty()) {
            full = true; // an unclassifiable cancelled request: fail safe
        } else {
            for (QStringList::ConstIterator it = m_activeRepairStages.begin();
                 it != m_activeRepairStages.end(); ++it) {
                const std::vector<std::string> mapped =
                    mapCancelledRepairSections(m_repairDiagnosticMap,
                                               repairToolKeyForStage(toStd(*it)));
                if (mapSectionsAreComplete(mapped)) {
                    full = true;
                    break;
                }
                appendUniqueSections(&affected, mapSectionsToQStringList(mapped));
            }
        }
        if (!full && affected.isEmpty()) {
            return false;
        }
    } else {
        const QStringList keys =
            invalidatingRepairToolKeys(parsed, m_activeRepairStages);
        if (keys.isEmpty()) {
            return false; // every stage proven unchanged/skipped
        }
        for (QStringList::ConstIterator it = keys.begin(); it != keys.end(); ++it) {
            const std::vector<std::string> mapped =
                mapAffectedSectionsForTool(m_repairDiagnosticMap, toStd(*it));
            if (mapSectionsAreComplete(mapped)) {
                full = true;
                break;
            }
            appendUniqueSections(&affected, mapSectionsToQStringList(mapped));
        }
        if (!full && affected.isEmpty()) {
            return false;
        }
    }
    if (full) {
        return applyDiagnosticInvalidation(true, QValueList<QString>());
    }
    return applyDiagnosticInvalidation(false, affected);
}

bool LegacyMainWindow::applyDiagnosticInvalidation(bool full,
                                                   const QValueList<QString> &sections)
{
    if (full) {
        // The complete set: the capability model and every cached section are
        // invalidated (modern invalidateAllActiveScopeDiagnostics parity).
        m_model.reset();
        m_diagnosticSections.clear();
        m_fullDiagnosticReport = QString::null;
        m_lastDiagnosticKey = QString::null;
        m_lastDiagnosticIdentity = QString::null;
        refreshDiagnosticResultsView();
        refreshCapabilities();
        return true;
    }
    bool removedAny = false;
    bool clearsCapabilities = false;
    for (QValueList<QString>::ConstIterator it = sections.begin();
         it != sections.end(); ++it) {
        const QString section = (*it).lower();
        if (section == QString::fromLatin1("capabilities")) {
            clearsCapabilities = true;
            continue;
        }
        m_diagnosticSections.remove(section);
        m_fullDiagnosticReport = removeEmbeddedDiagnosticSection(
            m_fullDiagnosticReport, section);
        removedAny = true;
        if (m_lastDiagnosticKey.lower() == section) {
            m_lastDiagnosticKey = QString::null;
            m_lastDiagnosticIdentity = QString::null;
        }
    }
    if (clearsCapabilities) {
        // The capability lines are evidence of their own (the dedicated
        // capability cache entry in modern terms): clear them so every tool
        // gate fails closed with "run diagnostics" until a fresh run lands.
        m_model.clearRepairCapabilities();
        removedAny = true;
    }
    if (!removedAny) {
        return false;
    }
    refreshDiagnosticResultsView();
    refreshCapabilities();
    return true;
}

void LegacyMainWindow::accumulateRepairPlanInvalidation(const ParsedTranscript &parsed,
                                                        bool commandOk, bool cancelled)
{
    QValueList<QString> affected;
    bool full = false;
    if (!commandOk && !cancelled) {
        // A genuine stage failure may have left more than its mapped sections
        // in an unknown state: the complete set; a failed read-only
        // pre-stage invalidates nothing.
        full = stagesAreModifying(m_repairDiagnosticMap, m_activeRepairStages);
        if (!full) {
            return;
        }
    } else if (cancelled) {
        if (!repairTranscriptHasChangedStatus(parsed)) {
            return; // nothing changed before the abort
        }
        if (m_activeRepairStages.isEmpty()) {
            full = true;
        } else {
            for (QStringList::ConstIterator it = m_activeRepairStages.begin();
                 it != m_activeRepairStages.end(); ++it) {
                const std::vector<std::string> mapped =
                    mapCancelledRepairSections(m_repairDiagnosticMap,
                                               repairToolKeyForStage(toStd(*it)));
                if (mapSectionsAreComplete(mapped)) {
                    full = true;
                    break;
                }
                appendUniqueSections(&affected, mapSectionsToQStringList(mapped));
            }
        }
        if (!full && affected.isEmpty()) {
            return;
        }
    } else {
        const QStringList keys =
            invalidatingRepairToolKeys(parsed, m_activeRepairStages);
        if (keys.isEmpty()) {
            return; // every stage proven unchanged/skipped
        }
        for (QStringList::ConstIterator it = keys.begin(); it != keys.end(); ++it) {
            const std::vector<std::string> mapped =
                mapAffectedSectionsForTool(m_repairDiagnosticMap, toStd(*it));
            if (mapSectionsAreComplete(mapped)) {
                full = true;
                break;
            }
            appendUniqueSections(&affected, mapSectionsToQStringList(mapped));
        }
        if (!full && affected.isEmpty()) {
            return;
        }
    }
    if (full) {
        m_repairPlanRequiresFullInvalidation = true;
        m_repairPlanInvalidatedSections.clear();
    } else {
        appendUniqueSections(&m_repairPlanInvalidatedSections, affected);
    }
    m_repairPlanModified = true;
}

// Ported from MainWindow::repairToolSerializesBaselineSection: the sections
// with a helper-side `--evidence` consumer. The legacy helper's
// store_evidence_baseline accepts exactly kernel, gpu, display, uki, grub,
// boot-evidence and report; the individual repair path serializes the gpu
// baseline as `--evidence gpu` (the helper routes it to GPU_BASELINE_B64, the
// same store the modern runGpuRepairSession feeds via --gpu-baseline) and
// report has no consumer, so only this set is serialized, exactly like modern.
bool repairToolSerializesBaselineSection(const QString &section)
{
    return section == QString::fromLatin1("kernel")
        || section == QString::fromLatin1("gpu")
        || section == QString::fromLatin1("display")
        || section == QString::fromLatin1("uki")
        || section == QString::fromLatin1("grub")
        || section == QString::fromLatin1("boot-evidence");
}

// Ported from MainWindow::splitDiagnosticCapabilityPreamble and extended for
// the legacy helper's richer shared preamble: `diagnostic_repair_capabilities`
// emits the modern `Repair capability probes (read-only...` block plus the
// legacy `Legacy feature gating (read-only):` and (target scope only)
// `Legacy configuration files (read-only probe):` blocks. A single-key
// `diagnose <key>` run includes this whole preamble (REPORT_SHARED_PREAMBLE=0),
// so it is stripped here so the section shows only its own content.
void splitDiagnosticCapabilityPreamble(const QString &captured,
                                       QString *body, QString *preamble)
{
    if (body) {
        *body = captured;
    }
    if (preamble) {
        *preamble = QString::null;
    }
    const QString marker = QString::fromLatin1("Repair capability probes (read-only");
    if (!captured.contains(marker)) {
        return;
    }

    const QStringList lines = QStringList::split(QChar('\n'), captured, true);
    const int lineCount = static_cast<int>(lines.count());
    int start = -1;
    for (int index = 0; index < lineCount; ++index) {
        if (lines[index].stripWhiteSpace().startsWith(marker)) {
            start = index;
            break;
        }
    }
    if (start < 0) {
        return;
    }

    // The preamble is the contiguous run of `Repair ` / `Host ` /
    // `Snapshot rollback` / `Storage ...` lines plus the legacy
    // `Legacy feature ...` and `Legacy config ...` blocks, ending at the first
    // blank or unrelated line (the section's own body).
    int end = start;
    while (end < lineCount) {
        const QString line = lines[end].stripWhiteSpace();
        if (line.isEmpty()
            || (!line.startsWith(QString::fromLatin1("Repair "))
                && !line.startsWith(QString::fromLatin1("Host "))
                && !line.startsWith(QString::fromLatin1("Snapshot rollback"))
                && !line.startsWith(QString::fromLatin1("Storage lvm inspection"))
                && !line.startsWith(QString::fromLatin1("Storage software raid"))
                && !line.startsWith(QString::fromLatin1("Legacy feature "))
                && !line.startsWith(QString::fromLatin1("Legacy config "))
                && !line.startsWith(QString::fromLatin1("Legacy configuration files")))) {
            break;
        }
        ++end;
    }

    if (preamble) {
        QStringList block;
        for (int index = start; index < end; ++index) {
            block.append(lines[index]);
        }
        *preamble = block.join(QString::fromLatin1("\n")).stripWhiteSpace();
    }

    if (body) {
        QStringList result;
        for (int index = 0; index < lineCount; ++index) {
            if (index >= start && index < end) {
                continue;  // the shared preamble block
            }
            if (index == end && lines[index].stripWhiteSpace().isEmpty()) {
                continue;  // the blank separator that framed the preamble
            }
            result.append(lines[index]);
        }
        *body = result.join(QString::fromLatin1("\n"));
    }
}

// Ported from MainWindow::mergeEmbeddedDiagnosticSection: replaces one embedded
// section of the combined report with a freshly captured section, matched by
// its `Diagnostic: <key>` header, so an individual re-run keeps the Full report
// usable instead of dropping the bundle. Returns the bundle unchanged when the
// key is not embedded (or the bundle is empty).
QString mergeEmbeddedDiagnosticSection(const QString &bundle, const QString &key,
                                       const QString &section)
{
    if (bundle.stripWhiteSpace().isEmpty() || section.stripWhiteSpace().isEmpty()) {
        return bundle;
    }
    QStringList lines = QStringList::split(QChar('\n'), bundle, true);
    const QString headerPrefix = QString::fromLatin1("Diagnostic: ");
    std::vector<int> headers;
    for (int index = 0; index < static_cast<int>(lines.count()); ++index) {
        if (lines[index].startsWith(headerPrefix)) {
            headers.push_back(index);
        }
    }
    int position = -1;
    for (std::size_t index = 0; index < headers.size(); ++index) {
        if (lines[headers[index]].mid(headerPrefix.length()).stripWhiteSpace().lower()
                == key.lower()) {
            position = static_cast<int>(index);
            break;
        }
    }
    if (position < 0) {
        return bundle;
    }
    const int start = embeddedSectionStartLine(lines, headers[position]);
    const int end = position + 1 < static_cast<int>(headers.size())
        ? embeddedSectionStartLine(lines, headers[position + 1])
        : static_cast<int>(lines.count());
    const QStringList replacement =
        QStringList::split(QChar('\n'), section.stripWhiteSpace(), true);
    QStringList merged;
    for (int index = 0; index < start; ++index) {
        merged.append(lines[index]);
    }
    for (int index = 0; index < static_cast<int>(replacement.count()); ++index) {
        merged.append(replacement[index]);
    }
    // Keep the blank separator that framed the replaced section so the next
    // section's divider stays visually separated.
    if (end < static_cast<int>(lines.count()) && !merged.isEmpty()
        && !merged.last().isEmpty()) {
        merged.append(QString::null);
    }
    for (int index = end; index < static_cast<int>(lines.count()); ++index) {
        merged.append(lines[index]);
    }
    return merged.join(QString::fromLatin1("\n"));
}

// Round 27 (Debian Legacy #7): the inverse of mergeEmbeddedDiagnosticSection
// — removes one embedded section of the combined report, matched by its
// `Diagnostic: <key>` header, at the moment the section is invalidated (an
// individual re-run start or a cancel initiation).  Returns the bundle
// unchanged when the key is not embedded (or the bundle is empty).
QString removeEmbeddedDiagnosticSection(const QString &bundle, const QString &key)
{
    if (bundle.stripWhiteSpace().isEmpty()) {
        return bundle;
    }
    QStringList lines = QStringList::split(QChar('\n'), bundle, true);
    const QString headerPrefix = QString::fromLatin1("Diagnostic: ");
    std::vector<int> headers;
    for (int index = 0; index < static_cast<int>(lines.count()); ++index) {
        if (lines[index].startsWith(headerPrefix)) {
            headers.push_back(index);
        }
    }
    int position = -1;
    for (std::size_t index = 0; index < headers.size(); ++index) {
        if (lines[headers[index]].mid(headerPrefix.length()).stripWhiteSpace().lower()
                == key.lower()) {
            position = static_cast<int>(index);
            break;
        }
    }
    if (position < 0) {
        return bundle;
    }
    const int start = embeddedSectionStartLine(lines, headers[position]);
    const int end = position + 1 < static_cast<int>(headers.size())
        ? embeddedSectionStartLine(lines, headers[position + 1])
        : static_cast<int>(lines.count());
    QStringList result;
    for (int index = 0; index < start; ++index) {
        result.append(lines[index]);
    }
    // Keep the blank separator that framed the removed section so the next
    // section's divider stays visually separated.
    if (end < static_cast<int>(lines.count()) && !result.isEmpty()
        && !result.last().stripWhiteSpace().isEmpty()) {
        result.append(QString::null);
    }
    for (int index = end; index < static_cast<int>(lines.count()); ++index) {
        result.append(lines[index]);
    }
    return result.join(QString::fromLatin1("\n"));
}

// The filesystem name shown in the inventory/details: the mounted filesystem
// first, then the read-only udev probe (so an unmounted ext3 or a locked
// crypto_LUKS component is still visible without opening the device).
// The filesystem label for one non-disk inventory row: the mounted/swapped
// filesystem first, then the read-only udev probe (so an unmounted ext3 or a
// locked crypto_LUKS component is still visible without opening the device),
// then the swap marker, and "unknown" as the last resort. The label is never
// empty: every Filesystem cell in the device list and the details panel
// renders something.
QString displayFsType(const DeviceRow &row)
{
    if (!row.fstype.empty()) {
        return fromStd(row.fstype);
    }
    if (!row.probedFstype.empty()) {
        return fromStd(row.probedFstype);
    }
    if (row.mountpoint == "[swap]") {
        return QString::fromLatin1("[swap]");
    }
    return QString::fromLatin1("unknown");
}

// Loads the packaged PNG icon with a graceful fallback: the installed hicolor
// path first, then the source tree next to the binary, then no icon at all.
QPixmap legacyHeaderPixmap()
{
    QStringList candidates;
    candidates.append(QString::fromLatin1(
        "/usr/share/icons/hicolor/32x32/apps/boot-repair-legacy.png"));
    candidates.append(QString::fromLatin1(
        "/usr/share/icons/hicolor/48x48/apps/boot-repair-legacy.png"));
    candidates.append(QString::fromLatin1(
        "/usr/share/icons/hicolor/22x22/apps/boot-repair-legacy.png"));
    candidates.append(QString::fromLatin1(
        "/usr/share/pixmaps/boot-repair-legacy.png"));
    if (qApp) {
        const QString appDir = qApp->applicationDirPath();
        candidates.append(appDir
            + QString::fromLatin1("/../share/icons/hicolor/32x32/apps/boot-repair-legacy.png"));
        candidates.append(appDir
            + QString::fromLatin1("/../share/icons/hicolor/48x48/apps/boot-repair-legacy.png"));
        candidates.append(appDir
            + QString::fromLatin1("/data/boot-repair-legacy-32x32.png"));
    }
    for (QStringList::ConstIterator it = candidates.begin(); it != candidates.end(); ++it) {
        if (!QFile::exists(*it)) {
            continue;
        }
        QImage image(*it);
        if (image.isNull()) {
            continue;
        }
        if (image.width() != kHeaderIconSize || image.height() != kHeaderIconSize) {
            image = image.smoothScale(kHeaderIconSize, kHeaderIconSize,
                                      QImage::ScaleMin);
        }
        QPixmap pixmap(image);
        if (!pixmap.isNull()) {
            return pixmap;
        }
    }
    return QPixmap();
}

// Round 70 (E2): loads the packaged running-system shield badge — the COLORED
// semantic status asset extracted from the shared modern icon atlas
// (resources/icons/atlas/security.svg, the green #2e9d50 shield with a
// checkmark, rendered to the shipped PNG by
// legacy/gui/data/make-shield-icon.py) — with the same graceful fallback
// chain as legacyHeaderPixmap().  A null pixmap lets the host card fall back
// to the green "[OK]" text.
QPixmap legacyShieldPixmap()
{
    QStringList candidates;
    candidates.append(QString::fromLatin1(
        "/usr/share/icons/hicolor/32x32/apps/boot-repair-legacy-shield.png"));
    if (qApp) {
        const QString appDir = qApp->applicationDirPath();
        candidates.append(appDir
            + QString::fromLatin1("/../share/icons/hicolor/32x32/apps/boot-repair-legacy-shield.png"));
        candidates.append(appDir
            + QString::fromLatin1("/data/boot-repair-legacy-shield-32x32.png"));
    }
    for (QStringList::ConstIterator it = candidates.begin(); it != candidates.end(); ++it) {
        if (!QFile::exists(*it)) {
            continue;
        }
        QImage image(*it);
        if (image.isNull()) {
            continue;
        }
        if (image.width() != kShieldBadgeSize || image.height() != kShieldBadgeSize) {
            image = image.smoothScale(kShieldBadgeSize, kShieldBadgeSize,
                                      QImage::ScaleMin);
        }
        QPixmap pixmap(image);
        if (!pixmap.isNull()) {
            return pixmap;
        }
    }
    return QPixmap();
}

// True when a failed command transcript actually shows a sudo session problem
// (expired timestamp, no tty for a prompt, rejected password). A mere
// "sudo:" substring in unrelated target log output must not drop the cached
// authorization.
bool transcriptSuggestsAuthFailure(const QString &transcript)
{
    static const char *const markers[] = {
        "a password is required",
        "no tty present",
        "a terminal is required",
        "incorrect password",
        "Sorry, try again",
        "authentication failure",
        "must have a tty",
        "not allowed to execute",
        "is not in the sudoers"
    };
    for (int i = 0; i < static_cast<int>(sizeof(markers) / sizeof(markers[0])); ++i) {
        if (transcript.find(QString::fromLatin1(markers[i])) >= 0) {
            return true;
        }
    }
    return false;
}

// Finds a menu item by its visible text (accelerator ampersands and the
// accelerator shortcut after a tab are ignored; Qt3's text(id) returns
// '&Refresh Devices\tF5'). Qt3 auto-generates NEGATIVE menu ids, so the sign
// cannot indicate validity; the boolean return is the only validity signal.
// Returns the id in *id when requested. Used by the smoke control
// verification.
bool menuItemId(QPopupMenu *menu, const QString &text, int *id)
{
    if (id) {
        *id = -1;
    }
    if (!menu) {
        return false;
    }
    for (uint i = 0; i < menu->count(); ++i) {
        const int itemId = menu->idAt(static_cast<int>(i));
        QString label = menu->text(itemId);
        const int tab = label.find(QChar('\t'));
        if (tab >= 0) {
            label = label.left(tab);
        }
        label.remove(QChar('&'));
        if (label == text) {
            if (id) {
                *id = itemId;
            }
            return true;
        }
    }
    return false;
}

// True when `child` is `ancestor` or a descendant of it (Qt3 QWidget has no
// isAncestorOf).
bool isInsideWidget(const QWidget *child, const QWidget *ancestor)
{
    for (const QWidget *widget = child; widget; widget = widget->parentWidget()) {
        if (widget == ancestor) {
            return true;
        }
    }
    return false;
}

// The list view must be wide enough for every declared column, otherwise the
// Qt3 header clips the last column. `problems` receives one line per issue.
bool listColumnsFit(QListView *list, const QString &name, QString *problems)
{
    if (!list || !list->viewport()) {
        return true;
    }
    int total = 0;
    for (int column = 0; column < list->columns(); ++column) {
        total += list->columnWidth(column);
    }
    if (list->viewport()->width() < total) {
        if (problems) {
            problems->append(QString::fromLatin1("%1 columns need %2px, viewport is %3px")
                                 .arg(name).arg(total).arg(list->viewport()->width()));
        }
        return false;
    }
    return true;
}

// The last column must stretch to the right edge (Qt3 `LastColumn` resize
// mode), leaving no empty gutter after the widest declared column.
bool listLastColumnFills(QListView *list, const QString &name, QString *problems)
{
    if (!list || !list->viewport() || list->columns() == 0) {
        return true;
    }
    int total = 0;
    for (int column = 0; column < list->columns(); ++column) {
        total += list->columnWidth(column);
    }
    const int gutter = list->viewport()->width() - total;
    if (gutter > kLastColumnGutter) {
        if (problems) {
            problems->append(QString::fromLatin1(
                "%1 leaves a %2px gutter after the last column (viewport %3px, columns %4px)")
                .arg(name).arg(gutter).arg(list->viewport()->width()).arg(total));
        }
        return false;
    }
    return true;
}

bool comboTextFits(QComboBox *combo, const QString &name, QString *problems)
{
    if (!combo) {
        return true;
    }
    QFontMetrics metrics(combo->font());
    const int needed = metrics.width(combo->currentText()) + 46;
    if (combo->width() < needed) {
        if (problems) {
            problems->append(QString::fromLatin1("%1 text needs %2px, width is %3px")
                                 .arg(name).arg(needed).arg(combo->width()));
        }
        return false;
    }
    return true;
}

// True when `rootPath` plausibly belongs to `diskName`: a partition on that
// disk, or a mapper whose resolved backing chain leads to one. A mapper with
// an unresolved backing device stays compatible (best effort), exactly like
// the inventory's own resolution.
bool rootBelongsToDisk(const QMap<QString, DeviceRow> &rows,
                       const QString &rootPath, const QString &diskName)
{
    const QMap<QString, DeviceRow>::const_iterator it = rows.find(rootPath);
    if (it == rows.end()) {
        return false;
    }
    const DeviceRow &row = it.data();
    if (row.mapper) {
        if (row.parent.empty()) {
            return true;
        }
        if (row.parent == diskName) {
            return true;
        }
        const QMap<QString, DeviceRow>::const_iterator parent =
            rows.find(QString::fromLatin1("/dev/") + fromStd(row.parent));
        return parent != rows.end() && !parent.data().disk
            && parent.data().parent == diskName;
    }
    return !row.disk && row.parent == diskName;
}

// Qt 3.3.7's DEFAULT QSettings is GLOBAL scope (QSettingsPrivate's ctor sets
// globalScope=TRUE), NOT User scope: as root it writes qInstallPathSysconf()/
// <group>rc (/etc/qt3/<group>rc on this rig) while readGroup() merges every
// search path with the LAST path winning, so a stale ~/.qt/repairrc masked
// fresh root writes.  setPath(..., QSettings::User) is therefore REQUIRED to
// force User scope (see configureLegacySettings).  setPath ALSO inserts a
// search path BEFORE the last element (insertSearchPath): User scope builds
// $HOME/.<domain>/<product>.  An EMPTY domain collapses that to $HOME ITSELF
// (qsettings.cpp: homeDirPath() + "/." + domain.mid(0, lastDot) + "/" +
// product), so User-scope sync() - which writes searchPaths[1] - landed every
// save at $HOME/<group>rc (rig-verified on the Etch VM) while
// readGroup() merged [$HOME, ~/.qt] with ~/.qt winning, so the stale
// ~/.qt/devicesrc masked the fresh $HOME writes and the Device discovery
// checkboxes reverted to their defaults after every restart.  The domain "qt"
// below makes the inserted path exactly $HOME/.qt, so sync() writes
// ~/.qt/<group>rc and readGroup() reads [~/.qt, ~/.qt] - one location, no
// cross-location masking, for root and the desktop user alike (sudo preserves HOME).
// NOTE: a one-time cleanup is needed on already-deployed rigs: the
// empty-domain build left stray $HOME/<group>rc and $HOME/.<group>rc.lock
// files.  They are never read once $HOME stops being a search path; remove
// them once by hand.  This code deliberately does NOT delete files
// automatically.

// The Full Repair plan group's file (heading "repair" -> ~/.qt/repairrc).
QString legacySettingsRepairRcPath()
{
    return QDir::homeDirPath() + QString::fromLatin1("/.qt/repairrc");
}

// Force USER scope on a QSettings so root writes ~/.qt/<group>rc instead of the
// system /etc/qt3/<group>rc (sudo preserves HOME, so root and
// the desktop user share ~/.qt/).  The domain is NOT decoration: setPath(User) inserts
// $HOME/.<domain>/<product> as searchPaths[1], and User-scope sync() writes
// THERE.  An earlier empty domain collapsed that path to $HOME itself and
// every save landed at $HOME/<group>rc (the Device discovery revert).  The
// domain "qt" makes the inserted path exactly $HOME/.qt - the canonical
// per-user location - so both the write and the read resolve
// ~/.qt/<group>rc deterministically.
void configureLegacySettings(QSettings &settings)
{
    settings.setPath(QString::fromLatin1("qt"), QString::fromLatin1(""),
                     QSettings::User);
}

// The persisted settings files (~/.qt/<group>rc) that exist but cannot be read
// by this user.  A file that cannot be read makes every key in it read as
// absent, so the in-code defaults would silently stand in for the saved
// selection; the constructor reports these files in the session log.
QStringList unreadableSettingsFiles()
{
    QStringList files;
    const QString qtDirectory = QDir::homeDirPath() + QString::fromLatin1("/.qt");
    static const char *const settingsFiles[] = {
        "devicesrc", "logsrc", "diagnosticsrc", "repairrc"
    };
    for (int i = 0;
         i < static_cast<int>(sizeof(settingsFiles) / sizeof(settingsFiles[0])); ++i) {
        const QString path = qtDirectory + QString::fromLatin1("/")
            + QString::fromLatin1(settingsFiles[i]);
        const QFileInfo info(path);
        if (info.exists() && !info.isReadable()) {
            files.append(path);
        }
    }
    return files;
}

void LegacyMainWindow::loadRepairDiagnosticMap()
{
    const QString appDir = qApp->applicationDirPath();
    QStringList candidates;
    // The staged package copy (the same canonical data file the modern GUI
    // embeds), then a copy beside the executable for source-tree/dev runs.
    candidates << appDir
        + QString::fromLatin1("/../share/boot-repair-legacy/repair-diagnostic-map.tsv");
    candidates << appDir + QString::fromLatin1("/repair-diagnostic-map.tsv");
    for (QStringList::ConstIterator it = candidates.begin();
         it != candidates.end(); ++it) {
        std::string error;
        if (m_repairDiagnosticMap.load(toStd(*it), &error)) {
            return;
        }
    }
    qWarning("repair-diagnostic-map.tsv: the shared repair->diagnostic map is "
             "unavailable; every lookup fails safe to the complete set");
}

LegacyMainWindow::LegacyMainWindow(QWidget *parent, const char *name)
    : QMainWindow(parent, name),
      m_toolSortColumn(-1),
      m_toolSortAscending(true),
      m_helpSignalMapper(0),
      m_tabs(0),
      m_fileMenu(0),
      m_viewMenu(0),
      m_helpMenu(0),
      m_wrapLogsMenuValid(false),
      m_wrapLogsMenuId(-1),
      m_logFilterCombo(0),
      m_configCombo(0),
      m_deviceList(0),
      m_detailList(0),
      m_diagnosticList(0),
      m_toolList(0),
      m_sessionLogList(0),
      m_rawView(0),
      m_logView(0),
      m_unlockStatusView(0),
      m_logSearchEdit(0),
      m_shellCommandEdit(0),
      m_shellOutput(0),
      m_logWrapCheck(0),
      m_showNonLinuxCheck(0),
      m_showRemovableCheck(0),
      m_showEncryptedCheck(0),
      m_autoRefreshCheck(0),
      m_priorLogBanner(0),
      m_headerTitle(0),
      m_headerSubtitle(0),
      m_headerBadge(0),
      m_busyLabel(0),
      m_busyTimer(0),
      m_busyFrame(0),
      m_systemsHeading(0),
      m_targetsHeading(0),
      m_repairHeading(0),
      m_repairScopeLabel(0),
      m_diagHeading(0),
      m_scopeLabel(0),
      m_diagTitle(0),
      m_diagDescription(0),
      m_diagAvailability(0),
      m_chrootHeading(0),
      m_chrootScopeLabel(0),
      m_fileCopyHeading(0),
      m_fileCopyScopeLabel(0),
      m_fileCopySourceTitle(0),
      m_fileCopyDestinationTitle(0),
      m_fileCopyOptionsTitle(0),
      m_logsHeading(0),
      m_settingsHeading(0),
      m_planCountLabel(0),
      m_planReadinessLabel(0),
      m_planScroll(0),
      m_planGroup(0),
      m_toolsGroup(0),
      m_planGridContentHeight(0),
      m_planHugPending(false),
      m_planHugContentHeight(0),
      m_repairPage(0),
      m_toolTitle(0),
      m_toolDescription(0),
      m_toolPlanStatus(0),
      m_resultStatus(0),
      m_targetSummary(0),
      m_configLabel(0),
      m_configReasonLabel(0),
      m_chrootReasonLabel(0),
      m_authStatusLabel(0),
      m_settingsElevationLabel(0),
      m_capDistributionLabel(0),
      m_capPackageManagerLabel(0),
      m_capAuthLabel(0),
      m_capabilityTable(0),
      m_capabilityScopeSelector(0),
      m_scanButton(0),
      m_diagnosticsButton(0),
      m_runDiagnosticButton(0),
      m_copyResultsButton(0),
      m_saveResultsButton(0),
      m_configButton(0),
      m_unlockButton(0),
      m_setTargetButton(0),
      m_hostSystemLabel(0),
      m_hostStorageLabel(0),
      m_hostProtectedBadge(0),
      m_hostDetailsButton(0),
      m_detailsPaneTitle(0),
      m_inspectingHostDetails(false),
      m_hostCard(0),
      m_hostMaintenanceButton(0),
      m_authorizeButton(0),
      m_configurePlanButton(0),
      m_runFullRepairButton(0),
      m_toolRunButton(0),
      m_resultCloseButton(0),
      m_resultCancelButton(0),
      m_diagnosticsCancelButton(0),
      m_shellCancelButton(0),
      m_shellRunButton(0),
      m_shellClearButton(0),
      m_newSessionLogButton(0),
      m_addNoteButton(0),
      m_deleteSessionLogButton(0),
      m_refreshCapabilitiesButton(0),
      m_installSupportButton(0),
      m_hostDefaultButton(0),
      m_fileCopyAddFilesButton(0),
      m_fileCopyAddFolderButton(0),
      m_fileCopyRemoveButton(0),
      m_fileCopyClearButton(0),
      m_fileCopyPreviewButton(0),
      m_fileCopyRunButton(0),
      m_fileCopyDirectionCombo(0),
      m_fileCopyOwnershipCombo(0),
      m_fileCopySourceList(0),
      m_fileCopyDestinationEdit(0),
      m_fileCopyBrowseButton(0),
      m_fileCopyBrowseSourceButton(0),
      m_systemsSplitter(0),
      m_repairVerticalSplitter(0),
      m_repairToolsSplitter(0),
      m_chrootGroup(0),
      m_fileCopySourceGroup(0),
      m_fileCopyDestinationGroup(0),
      m_fileCopyOptionsGroup(0),
      m_chrootTab(0),
      m_settingsContent(0),
      m_settingsPlanGroup(0),
      m_settingsCapabilityGroup(0),
      m_settingsSafetyGroup(0),
      m_settingsSplitterHandle(0),
      m_settingsDragActive(false),
      m_settingsDragBaseY(0),
      m_settingsDragBaseUpper(0),
      m_repairContent(0),
      m_resultDialog(0),
      m_resultView(0),
      m_promptInput(0),
      m_promptSend(0),
      m_promptHint(0),
      m_promptActive(false),
      m_interactiveRequestCounter(0),
      m_shellPromptHint(0),
      m_shellPromptInput(0),
      m_shellPromptSend(0),
      m_chrootCommandHeading(0),
      m_settingsLoaded(false),
      m_runner(new HelperRunner(this)),
      m_fileCopyBrowseMode(0),
      m_fileCopyBrowseEntryDialog(0),
      m_fileCopyBrowseEntryEdit(0),
      m_fileCopyBrowseEntryBrowse(false),
      m_fileCopyBrowsePickerDialog(0),
      m_fileCopyBrowsePickerList(0),
      m_fileCopyBrowsePickerHint(0),
      m_fileCopyBrowsePickerUp(0),
      m_fileCopyBrowsePickerAwaiting(false),
      m_fileCopyBrowsePickerLate(false),
      m_logEntryKind(LogEntry::Application),
      m_updatingSelection(false),
      m_running(false),
      m_resultCancelRequested(false),
      m_diagnosticCancelInvalidated(false),
      m_filesystemRepairFlowActive(false),
      m_lastCommandOk(false),
      m_repairPlanActive(false),
      m_repairPlanCancelled(false),
      m_repairPlanFullRepair(false),
      m_repairPlanHost(false),
      m_repairPlanRepairIndex(-1),
      m_repairPlanAnyRepairCompleted(false),
      m_repairPlanInlineQuestion(false),
      m_repairPlanPhase(RepairPlanIdle),
      m_pendingDiagnostic(false),
      m_pendingQuiet(false),
      m_pendingUnlock(false),
      m_pendingConfig(false),
      m_pendingShell(false),
      m_pendingBrowse(false),
      m_unlockRetry(false),
      m_targetCommitted(false),
      m_hostMaintenance(false),
      m_viewingPriorLog(false),
      m_showNonLinux(true),
      m_showRemovable(true),
      m_showEncrypted(true),
      m_logWrapEnabled(true),
      m_autoRefreshEnabled(true),
      m_pendingConfigWrite(false),
      m_scopeSwitchPending(false),
      m_commitPending(false),
      m_smokeMode(false),
      m_smokeStep(0),
      m_smokeDiagnoseOk(false),
      m_smokeValidateOk(false),
      m_shellScopeHost(false),
      m_shellHistoryIndex(-1)
{
    setCaption(tr("Boot Bitch Legacy (Etch / KDE 3.5 era)"));
    // Round 30: load the shared repair->diagnostic invalidation map (the same
    // data/repair-diagnostic-map.tsv the modern GUI embeds; the package stages
    // a copy at /usr/share/boot-repair-legacy/). A missing/malformed file
    // logs a warning and every lookup fails safe to the complete set.
    loadRepairDiagnosticMap();
    // B3/B5: a SIGTERM/SIGINT/SIGHUP must never leave a secret file behind
    // (unlock keyfile or config-write content file). The handlers unlink the
    // pending files (if any) and exit 128+signum.
    installKeyfileTerminationHandlers();
    // Every section title/group title/button/list column must stay fully
    // visible at the 1024x768 contract size; the layouts only need a modest
    // floor below it.
    setMinimumSize(820, 600);

    // Global header, mirroring the modern Qt6 window: packaged icon, the
    // 1.65x bold "Boot Bitch" title, the subtle recovery subtitle and the
    // right-aligned guarded-repair badge. The icon falls back gracefully when
    // the PNG is not installed (source-tree or headless runs).
    QWidget *central = new QWidget(this);
    // The global header spans the full window width: the guarded-repair badge
    // must sit flush against the window's right edge (modern parity), which a
    // QBoxLayout margin cannot express per side. The main layout carries no
    // margin; the header row gets an explicit left inset and the tab pages
    // keep their own inner margins.
    QVBoxLayout *mainLayout = new QVBoxLayout(central, 0, 6);

    QHBoxLayout *headerLayout = new QHBoxLayout(mainLayout);
    headerLayout->setSpacing(8);
    headerLayout->addSpacing(8);
    QLabel *iconLabel = new QLabel(central);
    const QPixmap headerPixmap = legacyHeaderPixmap();
    if (!headerPixmap.isNull()) {
        iconLabel->setPixmap(headerPixmap);
        iconLabel->setFixedSize(kHeaderIconSize, kHeaderIconSize);
        iconLabel->setAlignment(Qt::AlignCenter);
        // The packaged PNG is the real Boot Bitch artwork (generated from the
        // modern master by legacy/gui/data/make-icons.py); the window/app icon
        // uses it too, falling back gracefully when the PNG is not installed.
        setIcon(headerPixmap);
    } else {
        iconLabel->hide();
    }
    headerLayout->addWidget(iconLabel);

    QVBoxLayout *titleColumn = new QVBoxLayout();
    titleColumn->setSpacing(1);
    m_headerTitle = new QLabel(QString::fromLatin1("Boot Bitch"), central);
    QFont titleFont = m_headerTitle->font();
    titleFont.setPointSizeFloat(titleFont.pointSizeFloat() * 1.65);
    titleFont.setBold(true);
    m_headerTitle->setFont(titleFont);
    titleColumn->addWidget(m_headerTitle);
    m_headerSubtitle = new QLabel(
        tr("Linux recovery and boot-repair utility"), central);
    titleColumn->addWidget(m_headerSubtitle);
    headerLayout->addLayout(titleColumn, 1);

    // The badge text is built from a clean "GUARDED REPAIR" tr() token plus
    // the bullet and version (the override row covers the translated token),
    // so the badge reads correctly in every locale without trailing-space
    // pitfalls in the catalog source string.
    m_headerBadge = new QLabel(
        tr("GUARDED REPAIR") + QString::fromLatin1("  ") + QChar(0x2022)
            + QString::fromLatin1("  ") + QString::fromLatin1(LEGACY_VERSION),
        central);
    QFont badgeFont = m_headerBadge->font();
    badgeFont.setBold(true);
    m_headerBadge->setFont(badgeFont);
    m_headerBadge->setFrameShape(QFrame::NoFrame);
    m_headerBadge->setMargin(5);
    m_headerBadge->setAlignment(Qt::AlignCenter);
    // Cycle 14 loop: the frameless badge keeps its needed width (the frame
    // removal shaved 2px and the smoke's clip gate caught it).
    m_headerBadge->setMinimumWidth(
        QFontMetrics(m_headerBadge->font()).width(m_headerBadge->text()) + 12);
    QToolTip::add(m_headerBadge, tr(
        "Ordinary repairs require an explicitly selected non-host target. The "
        "protected running host has a separate deliberate maintenance mode with "
        "the same guarded repair stages and requires privilege authorization."));
    headerLayout->addWidget(m_headerBadge, 0, Qt::AlignTop);

    // Global busy indicator (modern parity): a reserved row BELOW the header
    // that shows the modern working text right-aligned under the guarded-
    // repair badge only while a helper command runs. The row keeps a fixed
    // height (the label never changes height), so showing or clearing the
    // text never moves the header widgets or the tab pages; the right-aligned
    // label grows toward the left when the text appears. A plain QTimer
    // cycles 0..3 trailing dots behind the text (no threads; Qt 3.3.7 needs
    // no animated-widget support for this).
    QHBoxLayout *busyRow = new QHBoxLayout();
    busyRow->setSpacing(0);
    busyRow->addStretch(1);
    m_busyLabel = new QLabel(QString::null, central);
    QFont busyFont = m_busyLabel->font();
    busyFont.setBold(true);
    m_busyLabel->setFont(busyFont);
    m_busyLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_busyLabel->setFixedHeight(
        QFontMetrics(m_busyLabel->font()).height() + 4);
    busyRow->addWidget(m_busyLabel, 0, Qt::AlignRight);
    mainLayout->addLayout(busyRow);

    m_busyTimer = new QTimer(this);
    m_busyFrame = 0;
    connect(m_busyTimer, SIGNAL(timeout()), this, SLOT(advanceBusyAnimation()));

    m_tabs = new QTabWidget(central);
    mainLayout->addWidget(m_tabs, 1);
    setCentralWidget(central);
    m_tabs->addTab(buildTargetsTab(), tr("Systems"));
    m_tabs->addTab(buildDiagnosticsTab(), tr("Diagnostics"));
    m_repairPage = buildActionsTab();
    m_tabs->addTab(m_repairPage, tr("Repair"));
    m_chrootTab = buildChrootShellTab();
    m_tabs->addTab(m_chrootTab, tr("Chroot Shell"));
    m_tabs->addTab(buildFileCopyTab(), tr("File Copy"));
    m_tabs->addTab(buildLogTab(), tr("Logs"));
    m_tabs->addTab(buildSettingsTab(), tr("Settings"));
    // Round 21 (splitter hug): a deferred plan hug (a Settings stage toggle
    // while the Repair page was hidden) re-applies once the Repair page
    // becomes current and its layouts activated for real — Qt3 does not
    // re-layout a hidden tab otherwise.
    connect(m_tabs, SIGNAL(currentChanged(QWidget *)),
            this, SLOT(repairTabShown()));
    // Modern parity: there is no About tab; Help -> About Boot Bitch opens the
    // dialog instead (showAbout()).

    buildMenus();
    // Load the persisted options before the first inventory scan so the
    // device-list filters are applied to the initial population.
    loadLegacySettings();

    connect(m_runner, SIGNAL(outputLine(const QString &)),
            this, SLOT(helperLine(const QString &)));
    connect(m_runner, SIGNAL(finished(bool, int)),
            this, SLOT(helperFinished(bool, int)));

    // Cycle 16: honor the pre-construction --log-dir so the constructor's own
    // readiness lines (written below) never land in the default $HOME tree.
    if (legacyInitialLogDirectory().isEmpty()) {
        m_logDirectory = QDir::homeDirPath() + QString::fromLatin1("/.boot-repair-legacy/logs");
    } else {
        m_logDirectory = legacyInitialLogDirectory();
    }
    scanDevices();
    autoDetectHostTarget();
    updateElevationLabel();
    updateHostCard();
    updateDriveDetails();
    updateUnlockStatus();
    updateDiagnosticDetails();
    refreshCapabilities();
    refreshSessionLogList();
    appendLog(tr(
        "Boot Bitch legacy GUI ready. Every privileged command runs through the "
        "ported helper; actions stay disabled until a repair target is "
        "committed (or Host Maintenance is active) and diagnostics report their "
        "capability line."));
    if (!legacySmokeSettingsIsolation()) {
        const QStringList unreadable = unreadableSettingsFiles();
        for (QStringList::ConstIterator it = unreadable.begin();
             it != unreadable.end(); ++it) {
            appendLog(tr(
                "WARNING: the saved GUI settings file %1 cannot be read; its "
                "options read as their defaults, not as the saved values.").arg(*it));
        }
        // Record where Qt 3.3.7 will read/write the Full Repair preference
        // (the per-user ~/.qt location, forced User scope via setPath), and
        // emit the load-side read-location/value line recorded above.
        appendLog(tr(
            "Settings location: Full Repair preferences resolve to %1 "
            "(Qt 3.3.7 per-user location; User scope forced via setPath).")
            .arg(legacySettingsRepairRcPath()));
        if (!m_settingsLoadDiagnostic.isEmpty()) {
            appendLog(m_settingsLoadDiagnostic);
        }
    }
    updateStatus();
    updateActionStates();
    resize(1024, 768);
}

void LegacyMainWindow::closeEvent(QCloseEvent *event)
{
    // Belt-and-braces persistence flush: every toggle already saves on change;
    // this final write covers any path that mutated state without re-entering
    // a handler (for example a programmatic scope change).
    saveLegacySettings();
    // The Cancel button is gone (modern parity); closing the window is the
    // only remaining path that stops a running helper command.
    if (m_runner && m_runner->isRunning()) {
        appendLog(tr(
            "Window closed while a helper command was running; cancelling it."));
        m_runner->cancel();
    }
    QMainWindow::closeEvent(event);
}

void LegacyMainWindow::setHelperPath(const QString &path)
{
    m_runner->setHelperPath(path);
    updateElevationLabel();
    updateStatus();
}

void LegacyMainWindow::setElevationOverride(const QString &override)
{
    m_runner->setElevationOverride(override);
    updateElevationLabel();
}

void LegacyMainWindow::setNoElevate(bool noElevate)
{
    m_runner->setNoElevate(noElevate);
    updateElevationLabel();
}

void LegacyMainWindow::setLogDirectory(const QString &path)
{
    // Cycle 16: when a pre-construction --log-dir already routed the
    // constructor's readiness lines into `path`, the session file is already
    // open there; only a genuinely different directory reopens it (otherwise
    // one session would split into two files).
    if (path != m_logDirectory) {
        m_logDirectory = path;
        m_logPath = QString::null;
    }
    refreshSessionLogList();
}

void LegacyMainWindow::setTarget(const QString &disk, const QString &root)
{
    setSelection(disk, QString::null, root);
    // Item #4 (preview/commit separation): the programmatic selection must also
    // record the preview row, so updateActionStates()/updateUnlockStatus()
    // (which act on previewDisk() while Host Maintenance is active) see the
    // same drive the caller just selected. Without this, the smoke's host-scope
    // checks (unlock status pane and the locked-LUKS Select Target tooltip)
    // read a stale empty preview instead of the selected drive.
    m_previewDisk = disk.stripWhiteSpace();
    m_previewComponent = QString::null;
    selectInventoryRow(disk);
    updateStatus();
    updateActionStates();
    updateDriveDetails();
    updateUnlockStatus();
}

// Programmatic selection used by setTarget() and the smoke test. `rootOverride`
// is remembered for `disk` and wins over the inventory auto-resolution until
// the user selects a different row.
void LegacyMainWindow::setSelection(const QString &disk, const QString &component,
                                    const QString &rootOverride)
{
    m_updatingSelection = true;
    m_selectedDisk = disk.stripWhiteSpace();
    m_selectedComponent = component.stripWhiteSpace();
    m_rootOverrideDisk = rootOverride.isEmpty() ? QString::null : m_selectedDisk;
    m_rootOverrideRoot = rootOverride.stripWhiteSpace();
    m_updatingSelection = false;
}

// Selects the inventory row for `disk` (and then the component row when one
// is set) without re-entering the selection handler.
void LegacyMainWindow::selectInventoryRow(const QString &disk)
{
    if (!m_deviceList) {
        return;
    }
    m_updatingSelection = true;
    QListViewItem *match = 0;
    for (QListViewItem *item = m_deviceList->firstChild(); item;
         item = item->nextSibling()) {
        if (item->text(0) == disk) {
            match = item;
            break;
        }
    }
    if (match) {
        m_deviceList->setSelected(match, true);
        m_deviceList->setCurrentItem(match);
    }
    m_updatingSelection = false;
}

QPushButton *LegacyMainWindow::makeButton(const QString &text, QWidget *parent)
{
    QPushButton *button = new QPushButton(text, parent);
    QFontMetrics metrics(button->font());
    button->setMinimumWidth(QMAX(button->minimumWidth(),
                                 metrics.width(text) + kButtonTextPadding));
    button->setMinimumHeight(QMAX(button->minimumHeight(), metrics.height() + 14));
    m_buttons.push_back(button);
    return button;
}

void LegacyMainWindow::updateButtonText(QPushButton *button, const QString &text)
{
    if (!button) {
        return;
    }
    button->setText(text);
    QFontMetrics metrics(button->font());
    button->setMinimumWidth(QMAX(40, metrics.width(text) + kButtonTextPadding));
}

// Modern sectionTitle() parity: 1.2x point size, bold. Qt3 spells the font
// size accessors pointSizeFloat()/setPointSizeFloat() (Qt4's pointSizeF()/
// setPointSizeF() do not exist here). The font-metric floor and the page
// containment check are asserted by the 1024x768 smoke layout gate.
// The label expands horizontally and keeps AlignLeft, so its text always
// starts at the same left edge as the frame content below it; without the
// Expanding policy some Qt3/KDE style combinations center the narrow label
// inside the row, which the smoke's left-edge probe rejects.
QLabel *LegacyMainWindow::makeSectionTitle(const QString &text, QWidget *parent)
{
    QLabel *label = new QLabel(text, parent);
    QFont font = label->font();
    font.setPointSizeFloat(font.pointSizeFloat() * 1.2);
    font.setBold(true);
    label->setFont(font);
    QFontMetrics metrics(label->font());
    label->setMinimumWidth(metrics.width(text) + kSectionTitlePadding);
    label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    m_sectionTitles.push_back(label);
    return label;
}

// Cycle 14: the (i) help link beside each page title (modern
// contextHelpButton parity): a frameless hyperlink-style label with the
// page's informational text, shown in a width-constrained read-only dialog
// on click. No tooltip-only behavior remains.
int g_helpCounter = 0;

QWidget *LegacyMainWindow::makeHelpButton(QWidget *parent, const QString &title,
                                          const QString &text)
{
    // Cycle 14 loop: Qt 3.3.7's QLabel has no linkActivated signal at
    // runtime, so the (i) affordance is a flat auto-raised QToolButton that
    // opens the width-constrained info dialog through a QSignalMapper.
    ++g_helpCounter;
    const QString href = QString::fromLatin1("help:%1").arg(g_helpCounter);
    m_helpTitles.insert(href, title);
    m_helpTexts.insert(href, text);
    QToolButton *button = new QToolButton(parent);
    button->setText(QString::fromLatin1("i"));
    button->setAutoRaise(true);
    button->setFixedSize(18, 18);
    if (!m_helpSignalMapper) {
        m_helpSignalMapper = new QSignalMapper(this);
        connect(m_helpSignalMapper, SIGNAL(mapped(const QString &)),
                this, SLOT(showHelpPopup(const QString &)));
    }
    connect(button, SIGNAL(clicked()), m_helpSignalMapper, SLOT(map()));
    m_helpSignalMapper->setMapping(button, href);
    QToolTip::add(button, tr("About %1").arg(title));
    return button;
}

void LegacyMainWindow::showHelpPopup(const QString &href)
{
    const QString title = mapValue(m_helpTitles, href);
    const QString text = mapValue(m_helpTexts, href);
    if (text.isEmpty()) {
        return;
    }
    // Round 21 (uniform "i" popup sizing): every tab's "i" popup renders at
    // ONE width and ONE height — the Repair page's popup is the reference
    // (its text is the longest).  The height is the Repair text wrapped at
    // the fixed text-column width plus the dialog chrome (layout margins +
    // the Close row), measured with the dialog's own font metrics so it
    // tracks the system font; every other tab's text renders inside the same
    // box — shorter texts sit in the shared frame, longer ones scroll (the
    // body lives in a scroll view).  No popup sizes itself per content
    // anymore, so the "i" dialogs look identical across all tabs.
    const int wrapWidth = kHelpPopupWidth - 40;  // the dialog's 20 px side margins

    // The reference: the Repair page's "i" text (title-based lookup; the
    // fallback keeps a sensible box when the register is unexpectedly empty).
    QString referenceText;
    for (QMap<QString, QString>::Iterator it = m_helpTitles.begin();
         it != m_helpTitles.end(); ++it) {
        if (it.data() == tr("Repair")) {
            referenceText = mapValue(m_helpTexts, it.key());
            break;
        }
    }
    if (referenceText.isEmpty()) {
        referenceText = text;
    }

    QDialog dialog(this, "legacy-info", true);
    dialog.setCaption(title.isEmpty()
                          ? tr("Information") : title);
    QVBoxLayout *layout = new QVBoxLayout(&dialog, 10, 8);

    QFontMetrics metrics(dialog.fontMetrics());
    const QRect referenceRect = metrics.boundingRect(
        0, 0, wrapWidth, 1000000,
        Qt::WordBreak | Qt::AlignLeft | Qt::AlignTop, referenceText);
    // One spare line of slack keeps the reference popup itself scrollbar-free
    // when the real wrap (which can be a few px narrower once a scrollbar
    // appears) lands a hair taller than the metric estimate.
    const int referenceBodyHeight = QMAX(referenceRect.height(),
                                         metrics.lineSpacing())
        + metrics.lineSpacing();

    // The body is a scroll view: every popup gets the SAME fixed frame, and a
    // text taller than the reference scrolls inside it.
    QScrollView *scroll = new QScrollView(&dialog);
    scroll->setResizePolicy(QScrollView::AutoOneFit);
    scroll->setVScrollBarMode(QScrollView::Auto);
    scroll->setHScrollBarMode(QScrollView::AlwaysOff);
    scroll->setFrameShape(QFrame::NoFrame);
    QWidget *bodyHost = new QWidget(scroll->viewport());
    scroll->addChild(bodyHost);
    QVBoxLayout *bodyLayout = new QVBoxLayout(bodyHost, 0, 0);
    QLabel *label = new QLabel(text, bodyHost);
    enableLabelWordWrap(label);
    label->setMaximumWidth(wrapWidth);
    bodyLayout->addWidget(label);
    layout->addWidget(scroll, 1);

    QHBoxLayout *buttons = new QHBoxLayout(layout);
    buttons->addStretch();
    QPushButton *close = new QPushButton(tr("Close"), &dialog);
    close->setDefault(true);
    buttons->addWidget(close);
    QObject::connect(close, SIGNAL(clicked()), &dialog, SLOT(accept()));
    const int referenceHeight = 10 + referenceBodyHeight + 8
                              + close->sizeHint().height() + 10;
    dialog.setFixedSize(kHelpPopupWidth, referenceHeight);
    dialog.exec();
}

// Qt3 list columns are fixed-width by default, so a header wider than its
// declared column clips. Raise every declared width to the header's font
// metric plus padding; `QListView::LastColumn` then stretches the last column
// to the viewport's right edge.
void LegacyMainWindow::addListViewColumn(QListView *list, const QString &title,
                                         int width)
{
    if (!list) {
        return;
    }
    QFontMetrics metrics(list->font());
    list->addColumn(title, QMAX(width, metrics.width(title) + kListHeaderPadding));
}

void LegacyMainWindow::registerGroupBox(QGroupBox *box)
{
    if (!box) {
        return;
    }
    QFontMetrics metrics(box->font());
    const int titleWidth = metrics.width(box->title())
        + 2 * box->frameWidth() + kGroupTitlePadding;
    box->setMinimumWidth(QMAX(box->minimumWidth(), titleWidth));
    m_groupBoxes.push_back(box);
}

void LegacyMainWindow::buildMenus()
{
    // Modern menu structure (MainWindow::buildMenuBar): File (Refresh
    // Devices, Lock Administrator Session, Quit), View (tab shortcuts,
    // Auto-size Device Columns, checkable Wrap Log Lines) and Help (Using
    // Boot Bitch, About Boot Bitch). The Logs save/clear actions live on the
    // Logs tab only.
    m_fileMenu = new QPopupMenu(this);
    menuBar()->insertItem(tr("&File"), m_fileMenu);
    m_fileMenu->insertItem(tr("&Refresh Devices"), this,
                           SLOT(scanDevices()), Key_F5);
    m_fileMenu->insertItem(tr("Lock Administrator Session"),
                           this, SLOT(lockAdministratorSession()));
    m_fileMenu->insertSeparator();
    m_fileMenu->insertItem(tr("Quit"), qApp, SLOT(quit()),
                           CTRL + Key_Q);

    m_viewMenu = new QPopupMenu(this);
    menuBar()->insertItem(tr("&View"), m_viewMenu);
    m_viewMenu->insertItem(tr("&Systems"), this, SLOT(showSystemsTab()));
    m_viewMenu->insertItem(tr("&Diagnostics"), this, SLOT(showDiagnosticsTab()));
    m_viewMenu->insertItem(tr("&Logs"), this, SLOT(showLogsTab()));
    m_viewMenu->insertItem(tr("&Settings"), this, SLOT(showSettingsTab()));
    m_viewMenu->insertSeparator();
    m_viewMenu->insertItem(tr("Auto-size Device Columns"),
                           this, SLOT(autoSizeDeviceColumns()));
    m_wrapLogsMenuId = m_viewMenu->insertItem(tr("Wrap Log Lines"),
                                              this, SLOT(toggleLogWrapFromMenu()));
    // Qt3 auto-generates negative ids; remember that the item exists instead of
    // testing the id's sign.
    m_wrapLogsMenuValid = true;
    m_viewMenu->setItemChecked(m_wrapLogsMenuId, m_logWrapEnabled);

    m_helpMenu = new QPopupMenu(this);
    menuBar()->insertItem(tr("&Help"), m_helpMenu);
    m_helpMenu->insertItem(tr("&Using Boot Bitch"), this,
                           SLOT(showUsageHelp()));
    m_helpMenu->insertSeparator();
    m_helpMenu->insertItem(tr("&About Boot Bitch"), this,
                           SLOT(showAbout()));
}

void LegacyMainWindow::showSystemsTab()
{
    if (m_tabs) {
        m_tabs->setCurrentPage(0);
    }
}

void LegacyMainWindow::showDiagnosticsTab()
{
    if (m_tabs) {
        m_tabs->setCurrentPage(1);
    }
}

void LegacyMainWindow::showLogsTab()
{
    if (m_tabs) {
        m_tabs->setCurrentPage(5);
    }
}

void LegacyMainWindow::showSettingsTab()
{
    if (m_tabs) {
        m_tabs->setCurrentPage(6);
    }
}

// Every visible device-tree item (top-level disks then their children), in
// traversal order.  The device list is now a tree (B7-2): the flat firstChild
// walk only ever sees the disks.
std::vector<QListViewItem *> LegacyMainWindow::deviceTreeItems() const
{
    std::vector<QListViewItem *> items;
    if (!m_deviceList) {
        return items;
    }
    for (QListViewItem *item = m_deviceList->firstChild(); item;
         item = item->nextSibling()) {
        items.push_back(item);
        for (QListViewItem *child = item->firstChild(); child;
             child = child->nextSibling()) {
            items.push_back(child);
        }
    }
    return items;
}

// Modern Auto-size Device Columns: size every column to the widest header or
// cell text plus the header-safe padding floor. The QListView LastColumn
// resize mode keeps stretching the final column to the viewport edge.
void LegacyMainWindow::autoSizeDeviceColumns()
{
    if (!m_deviceList) {
        return;
    }
    const QFontMetrics metrics(m_deviceList->font());
    const std::vector<QListViewItem *> items = deviceTreeItems();
    for (int column = 0; column < m_deviceList->columns(); ++column) {
        int width = metrics.width(m_deviceList->columnText(column)) + kListHeaderPadding;
        for (std::size_t i = 0; i < items.size(); ++i) {
            width = QMAX(width,
                         metrics.width(items[i]->text(column)) + kListHeaderPadding);
        }
        m_deviceList->setColumnWidth(column, width);
    }
    // Round 45 (round-24 out-of-scope finding): a long mapper name (the
    // unlocked `luks-<uuid>` device) used to widen the Device column past the
    // viewport, and the Qt3 header then clipped the last column (the smoke's
    // column-fit check failed with the unlocked hdb attached). Cap the Device
    // column at the width the other auto-sized columns leave in the viewport,
    // so the long name is clipped inside its own cell and the list never
    // overflows — the remaining columns stay fully visible.
    if (m_deviceList->columns() > 1 && m_deviceList->viewport()) {
        int others = 0;
        for (int column = 1; column < m_deviceList->columns(); ++column) {
            others += m_deviceList->columnWidth(column);
        }
        const int deviceCap = m_deviceList->viewport()->width() - others;
        if (deviceCap > 0 && m_deviceList->columnWidth(0) > deviceCap) {
            m_deviceList->setColumnWidth(0, deviceCap);
        }
    }
    // Qt 3.3.7 only re-stretches the LastColumn-mode final column from the
    // header's own resize event; a programmatic setColumnWidth() leaves the
    // viewport gutter behind (the Etch smoke's 382px-gutter regression). Fill
    // the viewport explicitly: the final column spans from the previous
    // column's end to the viewport's right edge, never below its auto-sized
    // width and never below the 20px Qt3 minimum.
    if (m_deviceList->columns() > 0 && m_deviceList->viewport()) {
        const int lastColumn = m_deviceList->columns() - 1;
        int others = 0;
        for (int column = 0; column < lastColumn; ++column) {
            others += m_deviceList->columnWidth(column);
        }
        const int fill = m_deviceList->viewport()->width() - others;
        const int lastWidth = QMAX(QMAX(m_deviceList->columnWidth(lastColumn), 20),
                                   fill);
        m_deviceList->setColumnWidth(lastColumn, lastWidth);
    }
    statusBar()->message(tr(
        "Device columns auto-sized. Drag headers to fine-tune widths."), 3500);
}

// The View menu item is the only wrap control (item 1 removed the Settings
// checkbox); this slot derives the new value from the cached flag (Qt3
// QPopupMenu does not guarantee an automatic check toggle) and toggleLogWrap()
// updates the menu check mark without re-entering its signal.
void LegacyMainWindow::toggleLogWrapFromMenu()
{
    toggleLogWrap(!m_logWrapEnabled);
}

// Modern File -> Lock Administrator Session: drop the cached elevation
// decision and best-effort clear the sudo timestamp without ever prompting or
// blocking, then refresh the inventory/status so the next privileged action
// asks for the password again.
void LegacyMainWindow::lockAdministratorSession()
{
    if (m_running) {
        statusBar()->message(tr(
            "A helper command is running; wait for it to finish before locking "
            "the session."), 4000);
        return;
    }
    const bool cleared = m_runner->clearSudoTimestamp();
    m_runner->resetElevation();
    appendLog(tr(
        "Administrator authorization session locked by the user; the cached "
        "session was dropped and the next privileged action requests the "
        "password again. Any LUKS mapping opened earlier remains open for this "
        "recovery session (the legacy helper exposes no close verb)."));
    if (!cleared) {
        appendLog(tr(
            "Note: the cached sudo timestamp could not be cleared (sudo is not "
            "installed or is not the elevation method); the cached decision is "
            "still reset."));
    }
    updateElevationLabel();
    updateActionStates();
    scanDevices();
    statusBar()->message(tr(
        "Administrator session locked; the next privileged action will request "
        "authorization."), 6000);
}

// Modern Help -> Using Boot Bitch, adapted to the legacy frontend. The dialog
// is width-constrained and word-wrapped (Qt3 has no QLabel::setWordWrap).
void LegacyMainWindow::showUsageHelp()
{
    QDialog dialog(this, "legacy-usage-help", true);
    dialog.setCaption(tr("Using Boot Bitch"));
    QVBoxLayout *layout = new QVBoxLayout(&dialog, 10, 8);
    QLabel *text = new QLabel(tr(
        "Boot Bitch must run from a different booted Linux environment than the "
        "system being repaired. Use a Linux live medium or another Linux "
        "installation on a different physical drive.<br><br>"
        "The running host is protected from ordinary repair-target selection, "
        "but it can be explicitly selected through <b>Host Maintenance</b> for "
        "guarded native diagnostics and supported maintenance stages.<br><br>"
        "Diagnostics follow the Systems page: the committed repair drive while "
        "Host Maintenance is off, or the protected running host while it is "
        "active.<br><br>"
        "The first privileged action requests administrator authorization once "
        "for this Boot Bitch window; <b>File - Lock Administrator Session</b> "
        "ends that helper session immediately. Every repair keeps the helper's "
        "own runtime preflights."),
        &dialog);
    text->setTextFormat(Qt::RichText);
    enableLabelWordWrap(text);
    text->setMaximumWidth(560);
    layout->addWidget(text);
    QHBoxLayout *buttons = new QHBoxLayout(layout);
    buttons->addStretch();
    QPushButton *close = new QPushButton(tr("Close"), &dialog);
    close->setDefault(true);
    buttons->addWidget(close);
    QObject::connect(close, SIGNAL(clicked()), &dialog, SLOT(accept()));
    dialog.setMinimumWidth(360);
    dialog.exec();
}

QWidget *LegacyMainWindow::buildTargetsTab()
{
    QWidget *page = new QWidget(m_tabs);
    QVBoxLayout *pageLayout = new QVBoxLayout(page, 8, 6);

    // Modern Systems page heading: title left, Refresh Devices right.
    QHBoxLayout *headingRow = new QHBoxLayout(pageLayout);
    headingRow->setSpacing(6);
    m_systemsHeading = makeSectionTitle(tr("Systems"), page);
    headingRow->addWidget(m_systemsHeading);
    // Cycle 15: a Preferred policy keeps the title at its text width so
    // the (i) button sits immediately after it on every page.
    m_systemsHeading->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
    headingRow->addWidget(makeHelpButton(page, tr("Systems"), tr(
        "Select a physical drive; Boot Bitch resolves the most likely Linux "
        "system volume automatically. The running host stays protected from "
        "ordinary target repairs, with a separate explicit host-maintenance "
        "path for its own system. The Details button shows the protected "
        "host's facts in the details pane; selecting any drive row restores "
        "the per-drive pane.")));
    headingRow->addStretch();
    m_scanButton = makeButton(tr("Refresh Devices"), page);
    QToolTip::add(m_scanButton, tr(
        "Re-read the read-only kernel inventory (/proc/partitions, /proc/mounts, "
        "/proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* and the udev "
        "metadata database). No block device is opened and nothing is written."));
    connect(m_scanButton, SIGNAL(clicked()), this, SLOT(scanDevices()));
    headingRow->addWidget(m_scanButton, 0, Qt::AlignTop);

    // Modern protected-host card (buildSystemsPage parity): the running host
    // is shown in a permanently-protected card above the candidate list, with
    // the green-check indicator, the probe-based identity/storage line, the
    // PROTECTED badge and the Details / Host Maintenance / Make Default
    // actions on its right. The values are the legacy-available fields
    // (helper facts + the read-only inventory) and stay honest when unknown.
    m_hostCard = new QGroupBox(page);
    QHBoxLayout *hostCardLayout = new QHBoxLayout(m_hostCard, 10, 6);
    // Cycle 10: ASCII-only indicator - Etch's fonts garble the modern check
    // glyph.  Round 70 (E2): the green "[OK]" text is replaced by the modern
    // running-system badge — the COLORED shield-checkmark extracted from the
    // shared icon atlas (resources/icons/atlas/security.svg, rendered to the
    // shipped PNG by legacy/gui/data/make-shield-icon.py) — the same green
    // look the modern Systems tab shows.  The green "[OK]" text stays as the
    // graceful fallback when the packaged PNG is missing (a dev build without
    // the installed asset).
    QLabel *hostCheck = new QLabel(m_hostCard);
    const QPixmap shieldPixmap = legacyShieldPixmap();
    if (!shieldPixmap.isNull()) {
        hostCheck->setPixmap(shieldPixmap);
        hostCheck->setFixedSize(kShieldBadgeSize, kShieldBadgeSize);
        hostCheck->setAlignment(Qt::AlignCenter);
        hostCheck->setSizePolicy(
            QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    } else {
        hostCheck->setText(tr("[OK]"));
        QFont checkFont = hostCheck->font();
        checkFont.setBold(true);
        checkFont.setPointSizeFloat(checkFont.pointSizeFloat() * 1.4);
        hostCheck->setFont(checkFont);
        QPalette checkPalette = hostCheck->palette();
        checkPalette.setColor(QColorGroup::Foreground, QColor(0x2e, 0xa0, 0x43));
        hostCheck->setPalette(checkPalette);
    }
    QToolTip::add(hostCheck, tr(
        "The running system was detected and stays protected from ordinary "
        "target repairs."));
    hostCardLayout->addWidget(hostCheck, 0, Qt::AlignTop);
    // Parentless sub-layouts owned by addLayout() (a layout constructed with
    // a parent layout is added automatically at construction; an explicit
    // addLayout() would parent it twice).
    QVBoxLayout *hostText = new QVBoxLayout();
    hostText->setSpacing(2);
    m_hostSystemLabel = new QLabel(
        tr("Detecting running system..."), m_hostCard);
    QFont hostFont = m_hostSystemLabel->font();
    hostFont.setBold(true);
    m_hostSystemLabel->setFont(hostFont);
    m_hostSystemLabel->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    m_hostSystemLabel->setMinimumWidth(0);
    hostText->addWidget(m_hostSystemLabel);
    m_hostStorageLabel = new QLabel(
        tr("Detecting protected storage..."), m_hostCard);
    m_hostStorageLabel->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    m_hostStorageLabel->setMinimumWidth(0);
    hostText->addWidget(m_hostStorageLabel);
    hostCardLayout->addLayout(hostText, 1);
    // The actions column is a ctor-parented sub-layout (auto-added at
    // construction, no explicit addLayout) like the other card sub-rows.
    QVBoxLayout *hostActions = new QVBoxLayout(hostCardLayout);
    hostActions->setSpacing(6);
    QHBoxLayout *hostBadgeRow = new QHBoxLayout(hostActions);
    m_hostProtectedBadge = new QLabel(tr("PROTECTED"), m_hostCard);
    QFont protectedFont = m_hostProtectedBadge->font();
    protectedFont.setBold(true);
    m_hostProtectedBadge->setFont(protectedFont);
    m_hostProtectedBadge->setFrameShape(QFrame::NoFrame);
    m_hostProtectedBadge->setMargin(5);
    QToolTip::add(m_hostProtectedBadge, tr(
        "The running host remains protected from ordinary repair-target "
        "operations."));
    hostBadgeRow->addWidget(m_hostProtectedBadge, 0, Qt::AlignLeft | Qt::AlignTop);
    QHBoxLayout *hostButtonsRow = new QHBoxLayout(hostActions);
    hostButtonsRow->setSpacing(6);
    m_hostDetailsButton = makeButton(tr("Details"), m_hostCard);
    m_hostDetailsButton->setEnabled(false);
    QToolTip::add(m_hostDetailsButton, tr(
        "Show read-only details for the protected running host."));
    connect(m_hostDetailsButton, SIGNAL(clicked()), this, SLOT(showHostDetails()));
    hostButtonsRow->addWidget(m_hostDetailsButton, 0, Qt::AlignVCenter);
    m_hostMaintenanceButton = makeButton(tr("Host Maintenance"), m_hostCard);
    m_hostMaintenanceButton->setEnabled(false);
    connect(m_hostMaintenanceButton, SIGNAL(clicked()), this, SLOT(toggleHostMaintenance()));
    hostButtonsRow->addWidget(m_hostMaintenanceButton, 0, Qt::AlignVCenter);
    // Modern Make Default parity (B7-7): beside Host Maintenance, gated by
    // Host Maintenance + the cached host-default probe + the session.
    m_hostDefaultButton = makeButton(tr("Make Default"), m_hostCard);
    m_hostDefaultButton->setEnabled(false);
    QToolTip::add(m_hostDefaultButton, tr(
        "Make the canonical installed kernel entry the default GRUB-legacy "
        "boot entry on the running host (menu.lst default directive with a "
        "backup and rollback). Requires Host Maintenance and the cached "
        "host-default probe."));
    connect(m_hostDefaultButton, SIGNAL(clicked()), this, SLOT(makeDefault()));
    hostButtonsRow->addWidget(m_hostDefaultButton, 0, Qt::AlignVCenter);
    registerGroupBox(m_hostCard);
    pageLayout->addWidget(m_hostCard);

    // "Available repair targets" section title above the drive list (modern
    // candidateRow). The list itself is the read-only kernel inventory.
    QHBoxLayout *targetsRow = new QHBoxLayout(pageLayout);
    targetsRow->setSpacing(6);
    m_targetsHeading = makeSectionTitle(
        tr("Available repair targets"), page);
    targetsRow->addWidget(m_targetsHeading);
    targetsRow->addStretch();
    QLabel *sortHint = new QLabel(tr("Most likely first"), page);
    QToolTip::add(sortHint, tr(
        "Drives are listed by the read-only inventory. Select a row to inspect "
        "it; Select Target commits the selected non-host drive with its "
        "auto-resolved Linux root component."));
    targetsRow->addWidget(sortHint);

    QSplitter *splitter = new QSplitter(Qt::Horizontal, page);
    splitter->setChildrenCollapsible(false);
    m_systemsSplitter = splitter;

    // ---- Left column: inventory, action row, Unlock status ------------------
    QWidget *left = new QWidget(splitter);
    QVBoxLayout *leftLayout = new QVBoxLayout(left, 0, 6);

    m_deviceList = new QListView(left);
    // Modern tree parity (B7-2): whole disks are top-level items with their
    // partitions/mappers as indented children (expand/collapse via the Qt3
    // tree features); Device, Size, Type, Filesystem columns only (Mount and
    // Note are dropped; the Filesystem column keeps the never-empty labels
    // with the disk aggregate).
    addListViewColumn(m_deviceList, tr("Device"), 100);
    addListViewColumn(m_deviceList, tr("Size"), 60);
    addListViewColumn(m_deviceList, tr("Type"), 80);
    addListViewColumn(m_deviceList, tr("Filesystem"), 75);
    m_deviceList->setAllColumnsShowFocus(true);
    m_deviceList->setShowSortIndicator(true);
    m_deviceList->setMultiSelection(false);
    m_deviceList->setResizeMode(QListView::LastColumn);
    m_deviceList->setMinimumHeight(70);
    m_deviceList->setRootIsDecorated(true);
    m_deviceList->setTreeStepSize(20);
    connect(m_deviceList, SIGNAL(selectionChanged()), this, SLOT(deviceSelectionChanged()));
    leftLayout->addWidget(m_deviceList, 1);

    // Single action row (modern parity): the repair-target actions (Select
    // Target / Unlock) sit left-aligned, followed by the deferred-authorization
    // affordance (status label + Authorize, both hidden unless authorization is
    // deferred) and the right-aligned committed-target summary on the SAME
    // line. Host Maintenance and Make Default live on the protected-host card
    // above. The status label keeps its Ignored horizontal policy so the row
    // minimum is the buttons only, and the Unlock status frame below reclaims
    // the vertical space the old two-row split wasted.
    QHBoxLayout *actionRow = new QHBoxLayout(leftLayout);
    actionRow->setSpacing(6);
    m_setTargetButton = makeButton(tr("Select Target"), left);
    m_setTargetButton->setEnabled(false);
    connect(m_setTargetButton, SIGNAL(clicked()), this, SLOT(setRepairTarget()));
    actionRow->addWidget(m_setTargetButton);

    m_unlockButton = makeButton(tr("Unlock"), left);
    m_unlockButton->setEnabled(false);
    connect(m_unlockButton, SIGNAL(clicked()), this, SLOT(runUnlock()));
    actionRow->addWidget(m_unlockButton);

    m_authStatusLabel = new QLabel(left);
    m_authStatusLabel->setTextFormat(Qt::PlainText);
    m_authStatusLabel->setAlignment(Qt::WordBreak | Qt::AlignLeft | Qt::AlignVCenter);
    // The label may shrink to zero width (its text wraps within the space the
    // row leaves it); the row's minimum stays the buttons only.
    m_authStatusLabel->setSizePolicy(
        QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred, 1, 0, TRUE));
    actionRow->addWidget(m_authStatusLabel, 1);

    m_authorizeButton = makeButton(tr("Authorize"), left);
    m_authorizeButton->setEnabled(false);
    QToolTip::add(m_authorizeButton, tr(
        "Establish the privileged helper session for the current scope now."));
    connect(m_authorizeButton, SIGNAL(clicked()), this, SLOT(authorizeNow()));
    actionRow->addWidget(m_authorizeButton, 0, Qt::AlignVCenter);

    // The committed-target summary keeps the row's right edge even when the
    // Authorize affordance is hidden (the stretch lives before the label,
    // independent of the deferred state).
    actionRow->addStretch(1);
    m_targetSummary = new QLabel(tr("Committed target: none"), left);
    m_targetSummary->setTextFormat(Qt::PlainText);
    m_targetSummary->setAlignment(Qt::WordBreak | Qt::AlignRight | Qt::AlignVCenter);
    actionRow->addWidget(m_targetSummary, 0, Qt::AlignRight | Qt::AlignVCenter);

    // Unlock status sits directly below the button row, exactly like modern.
    // The frame is titleless and the visible title is a sectionTitle label:
    // Qt3's KDE/Etch style clips QGroupBox titles at the top.
    QGroupBox *unlock = new QGroupBox(left);
    QToolTip::add(unlock, tr(
        "Unlock state for the selected drive; the LUKS passphrase is never logged."));
    QVBoxLayout *unlockLayout = new QVBoxLayout(unlock, 8, 4);
    unlockLayout->addWidget(makeSectionTitle(tr("Unlock status"), unlock));
    m_unlockStatusView = new QTextEdit(unlock);
    m_unlockStatusView->setReadOnly(true);
    m_unlockStatusView->setTextFormat(Qt::PlainText);
    m_unlockStatusView->setWordWrap(QTextEdit::WidgetWidth);
    m_unlockStatusView->setMinimumHeight(96);
    unlockLayout->addWidget(m_unlockStatusView, 1);
    leftLayout->addWidget(unlock, 1);

    // ---- Right column: selected drive details --------------------------------
    QWidget *right = new QWidget(splitter);
    // The details pane keeps an explicit floor: Field (120px) + Value (220px)
    // plus the frame/layout margins must stay fully visible at 1024x768, and
    // the "Selected drive details" section title must never clip. The left
    // pane absorbs any extra width (stretch factor below).
    right->setMinimumWidth(372);
    QVBoxLayout *rightLayout = new QVBoxLayout(right, 0, 6);

    QGroupBox *details = new QGroupBox(right);
    QToolTip::add(details, tr(
        "Read-only inventory plus helper-confirmed facts; mirrors the modern "
        "Qt6 Selected drive details panel."));
    QVBoxLayout *detailsLayout = new QVBoxLayout(details, 8, 4);
    m_detailsPaneTitle = makeSectionTitle(
        tr("Selected drive details"), details);
    detailsLayout->addWidget(m_detailsPaneTitle);
    m_detailList = new QListView(details);
    addListViewColumn(m_detailList, tr("Field"), 120);
    addListViewColumn(m_detailList, tr("Value"), 220);
    m_detailList->setAllColumnsShowFocus(true);
    m_detailList->setResizeMode(QListView::LastColumn);
    // The documented details field order (Drive, Detected target, Model/label,
    // ...) must not be alphabetized by Qt3's default first-column sorting.
    m_detailList->setSorting(-1);
    m_detailList->setMinimumHeight(120);
    m_detailList->setMinimumWidth(340);
    detailsLayout->addWidget(m_detailList, 1);
    rightLayout->addWidget(details, 1);

    registerGroupBox(unlock);
    registerGroupBox(details);

    QValueList<int> sizes;
    sizes.append(600);
    sizes.append(372);
    splitter->setSizes(sizes);
    // The left inventory pane absorbs the splitter's extra width; the details
    // pane keeps its 372px floor (its explicit minimum width is never broken
    // by QSplitter, so the Field/Value columns and the section title stay
    // fully visible on Etch's default sizing path). The single-row action area
    // caps the left pane minimum near the button width (the status labels
    // shrink to zero), so the splitter's own minimum (left + 372 + handle)
    // stays inside the 1004px page bound at 1024x768 and the splitter can
    // never grow past the window.
    left->setMinimumWidth(400);
    splitter->setResizeMode(left, QSplitter::Stretch);
    splitter->setResizeMode(right, QSplitter::KeepSize);
    pageLayout->addWidget(splitter, 1);
    return page;
}

QWidget *LegacyMainWindow::buildDiagnosticsTab()
{
    // Modern parity: heading row (title, scope label, Run All), the
    // target-configuration row (committed target only), then a horizontal
    // splitter with the diagnostic checks list on the left and the Selected
    // diagnostic pane (title, description, availability, Results + Run
    // Diagnostic, results text, Copy/Save Results) on the right.
    QWidget *page = new QWidget(m_tabs);
    QVBoxLayout *layout = new QVBoxLayout(page, 8, 6);

    QHBoxLayout *headingRow = new QHBoxLayout(layout);
    headingRow->setSpacing(6);
    m_diagHeading = makeSectionTitle(tr("Diagnostics"), page);
    headingRow->addWidget(m_diagHeading);
    // Cycle 15: a Preferred policy keeps the title at its text width so
    // the (i) button sits immediately after it on every page.
    m_diagHeading->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
    headingRow->addWidget(makeHelpButton(page, tr("Diagnostics"), tr(
        "Run All runs every available read-only diagnostic for the current "
        "scope; selecting a check runs it alone. Diagnostics are read-only "
        "and are the only evidence source for the gated repair actions.")));
    headingRow->addStretch();
    m_scopeLabel = new QLabel(tr("Target: none selected"), page);
    m_scopeLabel->setTextFormat(Qt::PlainText);
    m_scopeLabel->setAlignment(Qt::WordBreak | Qt::AlignRight | Qt::AlignVCenter);
    QToolTip::add(m_scopeLabel, tr(
        "Diagnostics follow the committed repair target, or the protected "
        "running host while Host Maintenance is active."));
    headingRow->addWidget(m_scopeLabel, 0, Qt::AlignVCenter);
    m_diagnosticsButton = makeButton(tr("Run All"), page);
    QToolTip::add(m_diagnosticsButton, tr(
        "Run All - run every available read-only diagnostic for the current scope."));
    connect(m_diagnosticsButton, SIGNAL(clicked()), this, SLOT(runDiagnostics()));
    headingRow->addWidget(m_diagnosticsButton, 0, Qt::AlignVCenter);
    // Round 21 (modern cancel-surface parity): Cancel for a running read-only
    // diagnostics run (Run All or a single diagnostic).  Armed only while a
    // diagnostic owns the helper (updateActionStates()), disabled before the
    // run starts and after it completes/aborts; the click drives the
    // documented legacy cancel path (cancel-file token + the bounded
    // sudo-wrapper kill) exactly like the repair popup's Cancel.
    m_diagnosticsCancelButton = makeButton(tr("Cancel"), page);
    m_diagnosticsCancelButton->setEnabled(false);
    QToolTip::add(m_diagnosticsCancelButton, tr(
        "Stop the running diagnostic."));
    connect(m_diagnosticsCancelButton, SIGNAL(clicked()), this,
            SLOT(cancelDiagnosticOperation()));
    headingRow->addWidget(m_diagnosticsCancelButton, 0, Qt::AlignVCenter);

    QHBoxLayout *configRow = new QHBoxLayout(layout);
    configRow->setSpacing(6);
    m_configLabel = new QLabel(tr("Target configuration:"), page);
    configRow->addWidget(m_configLabel);
    m_configCombo = new QComboBox(page);
    m_configCombo->setMinimumWidth(220);
    QToolTip::add(m_configCombo, tr(
        "Etch-era target configuration files; availability is probed read-only "
        "by the helper's diagnostics."));
    configRow->addWidget(m_configCombo, 1);
    m_configButton = makeButton(tr("Edit Target File"), page);
    connect(m_configButton, SIGNAL(clicked()), this, SLOT(editTargetConfigFile()));
    configRow->addWidget(m_configButton);

    m_configReasonLabel = new QLabel(page);
    m_configReasonLabel->setTextFormat(Qt::PlainText);
    m_configReasonLabel->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    layout->addWidget(m_configReasonLabel);

    QSplitter *splitter = new QSplitter(Qt::Horizontal, page);
    splitter->setChildrenCollapsible(false);

    QGroupBox *checks = new QGroupBox(splitter);
    QToolTip::add(checks, tr(
        "Runs one read-only diagnostic for the selected scope through the "
        "helper (`diagnose <key>` / `host-diagnose <key>`); Run All is the "
        "combined report."));
    QVBoxLayout *checksLayout = new QVBoxLayout(checks, 8, 4);
    checksLayout->addWidget(makeSectionTitle(tr("Diagnostic checks"), checks));
    m_diagnosticList = new QListView(checks);
    addListViewColumn(m_diagnosticList, tr("Check"), 130);
    m_diagnosticList->setAllColumnsShowFocus(true);
    m_diagnosticList->setResizeMode(QListView::LastColumn);
    m_diagnosticList->setShowToolTips(true);
    // Qt3 sorts a QListView by the first column by default; disable it so the
    // list keeps the legacy::diagnosticKeys() order (and the smoke's first-
    // entry assertion stays meaningful).
    m_diagnosticList->setSorting(-1);
    m_diagnosticList->setMinimumHeight(150);
    connect(m_diagnosticList, SIGNAL(selectionChanged()),
            this, SLOT(diagnosticSelectionChanged()));
    const std::vector<std::string> diagKeys = legacy::diagnosticKeys();
    m_diagKeyByTitle.clear();
    // Qt3 prepends a plain insertion when sorting is disabled, so every item is
    // chained after the previous one (the after-form constructor) to keep the
    // legacy::diagnosticKeys() order.
    QListViewItem *lastDiagnostic = 0;
    for (std::size_t i = 0; i < diagKeys.size(); ++i) {
        // Modern parity: the list shows the friendly diagnostic title; the
        // stable helper key stays internal (commands and the log use it).
        const QString key = fromStd(diagKeys[i]);
        const QString title = diagnosticTitle(key);
        m_diagKeyByTitle.insert(title, key);
        lastDiagnostic = new QListViewItem(m_diagnosticList, lastDiagnostic, title);
    }
    if (m_diagnosticList->firstChild()) {
        m_diagnosticList->setSelected(m_diagnosticList->firstChild(), true);
        m_diagnosticList->setCurrentItem(m_diagnosticList->firstChild());
    }
    checksLayout->addWidget(m_diagnosticList, 1);

    QGroupBox *detail = new QGroupBox(splitter);
    QVBoxLayout *detailLayout = new QVBoxLayout(detail, 8, 6);
    detailLayout->addWidget(makeSectionTitle(tr("Selected diagnostic"), detail));
    m_diagTitle = makeSectionTitle(tr("Select a diagnostic"), detail);
    detailLayout->addWidget(m_diagTitle);
    m_diagDescription = new QLabel(
        tr("Choose a diagnostic from the list."), detail);
    m_diagDescription->setTextFormat(Qt::PlainText);
    m_diagDescription->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    detailLayout->addWidget(m_diagDescription);
    m_diagAvailability = new QLabel(tr("Ready"), detail);
    m_diagAvailability->setTextFormat(Qt::PlainText);
    QFont availabilityFont = m_diagAvailability->font();
    availabilityFont.setBold(true);
    m_diagAvailability->setFont(availabilityFont);
    detailLayout->addWidget(m_diagAvailability);

    QHBoxLayout *resultsHeader = new QHBoxLayout(detailLayout);
    resultsHeader->setSpacing(6);
    QLabel *resultsTitle = makeSectionTitle(tr("Results"), detail);
    resultsHeader->addWidget(resultsTitle);
    resultsHeader->addStretch();
    m_runDiagnosticButton = makeButton(tr("Run Diagnostic"), detail);
    QToolTip::add(m_runDiagnosticButton, tr(
        "Run Diagnostic - run the selected read-only diagnostic for the current scope."));
    connect(m_runDiagnosticButton, SIGNAL(clicked()), this, SLOT(runSelectedDiagnostic()));
    resultsHeader->addWidget(m_runDiagnosticButton, 0, Qt::AlignVCenter);

    // Round 31 (Debian Legacy #9, modern placeholder parity): the empty-state
    // hint ("Diagnostic results appear here.") is the in-field greyed
    // placeholder painted INSIDE the results pane itself (the
    // PlaceholderTextEdit pattern) — not a label below the Results header.  It
    // shows only while the pane has no content, disappears the moment a result
    // appears, and re-appears when a re-run/cancelled diagnostic clears the
    // pane (the pane repaints on every setText/clear; no explicit sync).
    m_rawView = new PlaceholderTextEdit(detail);
    m_rawView->setReadOnly(true);
    // Item 2: Qt 3.3.7's Qt::LogText mode disables word wrap entirely, so the
    // Selected diagnostic pane uses Qt::PlainText with a fixed-pitch font and
    // widget-width wrap: the result reflows as the window or the splitter
    // shrinks instead of clipping long diagnostic lines horizontally.
    m_rawView->setTextFormat(Qt::PlainText);
    QFont rawMono(QString::fromLatin1("monospace"));
    rawMono.setStyleHint(QFont::TypeWriter);
    m_rawView->setFont(rawMono);
    m_rawView->setWordWrap(QTextEdit::WidgetWidth);
    m_rawView->setMinimumHeight(80);
    static_cast<PlaceholderTextEdit *>(m_rawView)->setPlaceholderText(
        tr("Diagnostic results appear here."));
    detailLayout->addWidget(m_rawView, 1);

    QHBoxLayout *resultButtons = new QHBoxLayout(detailLayout);
    resultButtons->setSpacing(6);
    resultButtons->addStretch();
    m_copyResultsButton = makeButton(tr("Copy Results"), detail);
    connect(m_copyResultsButton, SIGNAL(clicked()), this, SLOT(copyResults()));
    resultButtons->addWidget(m_copyResultsButton);
    m_saveResultsButton = makeButton(tr("Save Results..."), detail);
    connect(m_saveResultsButton, SIGNAL(clicked()), this, SLOT(saveResults()));
    resultButtons->addWidget(m_saveResultsButton);

    registerGroupBox(checks);
    registerGroupBox(detail);

    // Cycle 9: the Selected diagnostic pane keeps usable floors when the
    // window shrinks - the results view needs its minimum height, the pane a
    // minimum width that fits the Copy/Save Results row, and the checks list
    // keeps its size while the detail pane absorbs the rest, so the results
    // buttons row never gets clipped.
    detail->setMinimumWidth(360);
    m_rawView->setMinimumWidth(200);
    splitter->setResizeMode(checks, QSplitter::KeepSize);
    splitter->setResizeMode(detail, QSplitter::Stretch);

    QValueList<int> sizes;
    sizes.append(300);
    sizes.append(700);
    splitter->setSizes(sizes);
    layout->addWidget(splitter, 1);
    return page;
}

QWidget *LegacyMainWindow::buildActionsTab()
{
    // Modern Repair page parity: heading + scope label, the plan paragraph,
    // the "Full Repair plan" section, then a horizontal splitter with the
    // "Individual repair tools" list (Tool | Full Repair) on the left and the
    // "Selected tool" pane (title, run button, description, plan status) on the
    // right, then the legacy-only privilege-elevation frame.
    QWidget *page = new QWidget(m_tabs);
    QVBoxLayout *pageLayout = new QVBoxLayout(page, 8, 6);

    // The page is scrollable (Qt3 QScrollView, like the Settings tab) because
    // the plan + tools + elevation sections exceed 1024x768.
    QScrollView *scroll = new QScrollView(page);
    scroll->setResizePolicy(QScrollView::AutoOneFit);
    scroll->setVScrollBarMode(QScrollView::Auto);
    scroll->setHScrollBarMode(QScrollView::AlwaysOff);
    scroll->setFrameShape(QFrame::NoFrame);
    pageLayout->addWidget(scroll, 1);

    QWidget *content = new QWidget(scroll->viewport());
    scroll->addChild(content);
    m_repairContent = content;
    QVBoxLayout *layout = new QVBoxLayout(content, 6, 6);

    QHBoxLayout *headingRow = new QHBoxLayout(layout);
    headingRow->setSpacing(6);
    m_repairHeading = makeSectionTitle(tr("Repair"), content);
    headingRow->addWidget(m_repairHeading);
    // Cycle 15: a Preferred policy keeps the title at its text width so
    // the (i) button sits immediately after it on every page.
    m_repairHeading->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
    // Round 21: the inline plan paragraph is gone — its content moved into
    // this "i" popup with the modern wording, so the Full Repair plan frame
    // moves up by the freed space.
    headingRow->addWidget(makeHelpButton(content, tr("Repair"), tr(
        "Run one repair tool or use the Full Repair plan against the "
        "selected repair drive. Choose Host Maintenance on Systems to run "
        "the same supported, guarded stages against the protected Running "
        "Host. Choose Full Repair stages in Settings. Enabled stages run in "
        "the order shown. Each configurable stage also appears below as an "
        "individual tool; the Full Repair column mirrors its current "
        "Settings state. The boot tools (GRUB configuration, boot-stack "
        "reconciliation and Make Default) are independent: run them in any "
        "order, and a later action re-verifies what an earlier one changed "
        "and reports its own result. The active scope is shown beside "
        "Repair: selected repair drive or Running Host maintenance. Every "
        "action stays disabled until the cached capability lines say "
        "available and the helper keeps its runtime preflights. Write "
        "actions ask for confirmation and then run the helper's own runtime "
        "preflights; the GUI never weakens them. A repair that is not "
        "proven 'unchanged' invalidates the cached diagnostics and disables "
        "the gated actions until diagnostics run again.")));
    headingRow->addStretch();
    m_repairScopeLabel = new QLabel(tr("Target: none selected"), content);
    m_repairScopeLabel->setTextFormat(Qt::PlainText);
    m_repairScopeLabel->setAlignment(Qt::WordBreak | Qt::AlignRight | Qt::AlignVCenter);
    headingRow->addWidget(m_repairScopeLabel, 0, Qt::AlignVCenter);

    // Modern Repair page: the Full Repair plan frame and the individual-tools
    // splitter live in one draggable vertical splitter, so the user can
    // resize the top and bottom panes. The page keeps its Qt3 QScrollView.
    QSplitter *vertical = new QSplitter(Qt::Vertical, content);
    vertical->setChildrenCollapsible(false);
    m_repairVerticalSplitter = vertical;

    // ---- Full Repair plan section -------------------------------------------
    QGroupBox *planBox = new QGroupBox(vertical);
    m_planGroup = planBox;
    QVBoxLayout *planLayout = new QVBoxLayout(planBox, 6, 4);
    planLayout->addWidget(makeSectionTitle(tr("Full Repair plan"), planBox));

    QHBoxLayout *planHeader = new QHBoxLayout(planLayout);
    planHeader->setSpacing(6);
    m_planCountLabel = new QLabel(tr("No stages selected"), planBox);
    QFont countFont = m_planCountLabel->font();
    countFont.setBold(true);
    m_planCountLabel->setFont(countFont);
    planHeader->addWidget(m_planCountLabel);
    planHeader->addStretch();
    // Cycle 15: the plan actions live on their own right-aligned row so
    // Configure Plan / Run Full Repair never clip at the minimum width
    // (the stretch absorbs the leftover instead of pushing the buttons out).
    QHBoxLayout *planButtons = new QHBoxLayout(planLayout);
    planButtons->setSpacing(6);
    planButtons->addStretch(1);
    m_configurePlanButton = makeButton(tr("Configure Plan"), planBox);
    QToolTip::add(m_configurePlanButton, tr(
        "Open Settings to choose which Full Repair stages are part of the plan."));
    connect(m_configurePlanButton, SIGNAL(clicked()), this, SLOT(configurePlan()));
    planButtons->addWidget(m_configurePlanButton);
    m_runFullRepairButton = makeButton(tr("Run Full Repair"), planBox);
    m_runFullRepairButton->setEnabled(false);
    QToolTip::add(m_runFullRepairButton, tr(
        "Select a repair drive, or choose Host Maintenance on the protected "
        "running-host card."));
    connect(m_runFullRepairButton, SIGNAL(clicked()), this, SLOT(runFullRepair()));
    planButtons->addWidget(m_runFullRepairButton);

    m_planReadinessLabel = new QLabel(tr(kPlanReadinessDefault), planBox);
    m_planReadinessLabel->setTextFormat(Qt::PlainText);
    m_planReadinessLabel->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    planLayout->addWidget(m_planReadinessLabel);

    // Item 1 (two-column plan): the numbered stage titles flow across ONE
    // two-column grid of read-only QLabels (ceil(planSpecCount/2) rows x 2
    // columns, matching the Settings plan's two-column QGridLayout), left
    // column first so the plan order reads top-to-bottom down the left column
    // then the right. Qt3 has no responsive-column layout, so the columns are
    // a fixed-row split; updatePlanView() fills the labels in a BALANCED split
    // (ceil(N/2) left / floor(N/2) right) and hides the unused ones, keeping
    // one section header with no per-column titles.
    //
    // The stage-label grid lives inside a QScrollView so the Full Repair plan
    // scrolls its stages when the vertical splitter is dragged up to hide
    // them: the plan count, readiness hint and Configure Plan / Run Full
    // Repair buttons stay outside the scroll view and remain visible.
    PlanScrollView *planScroll = new PlanScrollView(planBox);
    m_planScroll = planScroll;
    // AutoOneFit (not AutoOne): the policy must stretch the single child to
    // the viewport WIDTH as the view is resized, otherwise the child keeps its
    // content width and a grey dead-space gutter is left to the right of the
    // two columns. Qt3's AutoOne only keeps the contents at the child's own
    // size; AutoOneFit resizes the child to at least the viewport (see
    // QScrollViewData::viewportResized), so the setColStretch(1,1) columns
    // below fill the available width with no right gutter.
    planScroll->setResizePolicy(QScrollView::AutoOneFit);
    planScroll->setVScrollBarMode(QScrollView::AlwaysOff);
    planScroll->setHScrollBarMode(QScrollView::AlwaysOff);
    planScroll->setFrameShape(QFrame::NoFrame);
    QWidget *planGridHost = new QWidget(planScroll->viewport());
    planScroll->addChild(planGridHost);
    const int planGridRows = (planSpecCount + 1) / 2;
    QGridLayout *planGrid = new QGridLayout(planGridHost, planGridRows, 2, 4, 4);
    planGrid->setColStretch(0, 1);
    planGrid->setColStretch(1, 1);
    m_planStageLabels.clear();
    for (int i = 0; i < planSpecCount; ++i) {
        QLabel *stageLabel = new QLabel(planGridHost);
        stageLabel->setTextFormat(Qt::PlainText);
        stageLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        stageLabel->hide();
        planGrid->addWidget(stageLabel, i % planGridRows, i / planGridRows);
        m_planStageLabels.append(stageLabel);
    }
    planLayout->addWidget(planScroll, 0);
    // A trailing stretch absorbs any extra frame height at the bottom: the
    // header rows and the capped stage grid keep their fixed heights (the
    // spacing never moves when the splitter is dragged - no vertical
    // stretching of the rows), and the slack collects below the last row.
    planLayout->addStretch(1);

    // ---- Individual tools list + Selected tool pane -------------------------
    QSplitter *splitter = new QSplitter(Qt::Horizontal, vertical);
    m_repairToolsSplitter = splitter;
    splitter->setChildrenCollapsible(false);
    // Drag floor: the tools pane is never squeezed below about a quarter of
    // the default window height (the plan frame's own layout minimum is the
    // matching floor on the other side), so the handle always has room and
    // neither pane collapses to a sliver.
    splitter->setMinimumHeight(192);

    QGroupBox *tools = new QGroupBox(splitter);
    m_toolsGroup = tools;
    QVBoxLayout *toolsLayout = new QVBoxLayout(tools, 6, 4);
    toolsLayout->addWidget(makeSectionTitle(tr("Individual repair tools"), tools));
    m_toolList = new QListView(tools);
    addListViewColumn(m_toolList, tr("Tool"), 200);
    addListViewColumn(m_toolList, tr("Full Repair"), 200);
    m_toolList->setAllColumnsShowFocus(true);
    m_toolList->setResizeMode(QListView::LastColumn);
    // Curated workflow order (Qt3's default first-column sorting would reorder
    // it); the after-form insertion keeps that order on the Etch Qt3 style.
    m_toolList->setSorting(-1);
    // Cycle 14 loop: Qt3's setSorting(true) re-sorts immediately (breaking
    // the curated order), so header-click sorting is manual: the curated
    // insertion order stays the default and clicking a header column sorts
    // asc/desc with the Selected tool pane refreshed afterwards.
    if (m_toolList->header()) {
        connect(m_toolList->header(), SIGNAL(clicked(int)),
                this, SLOT(toolListHeaderClicked(int)));
    }
    m_toolList->setMinimumHeight(220);
    connect(m_toolList, SIGNAL(selectionChanged()), this, SLOT(toolSelectionChanged()));
    QListViewItem *lastTool = 0;
    for (int i = 0; i < toolSpecCount; ++i) {
        // The rows stay selectable so the Selected tool pane can explain why a
        // display-only tool cannot run; the run button and the Full Repair
        // column carry the disabled state and the exact reason.
        lastTool = new QListViewItem(m_toolList, lastTool,
                                     tr(toolSpecs[i].title),
                                     tr("not reported"));
    }
    if (m_toolList->firstChild()) {
        m_toolList->setSelected(m_toolList->firstChild(), true);
        m_toolList->setCurrentItem(m_toolList->firstChild());
    }
    toolsLayout->addWidget(m_toolList, 1);

    QGroupBox *detail = new QGroupBox(splitter);
    // The Selected tool pane floor keeps its header row usable at 1024x768;
    // long tool titles then wrap (Qt::WordBreak below) instead of forcing the
    // pane wider or clipping like the Qt3 QGroupBox titles did.
    detail->setMinimumWidth(300);
    QVBoxLayout *detailLayout = new QVBoxLayout(detail, 6, 6);
    detailLayout->addWidget(makeSectionTitle(tr("Selected tool"), detail));
    QHBoxLayout *detailHeader = new QHBoxLayout(detailLayout);
    detailHeader->setSpacing(6);
    m_toolTitle = makeSectionTitle(tr("Select a repair tool"), detail);
    // Qt3 idiom for a dynamic title that must never clip: Qt::WordBreak lets
    // the label break onto a second line when the pane is too narrow for a
    // long tool name (QLabel has no setWordWrap in Qt 3.3.7).
    m_toolTitle->setAlignment(Qt::WordBreak | Qt::AlignLeft | Qt::AlignVCenter);
    // Cycle 15 loop + A12-10: the title is LEFT-aligned at its natural width
    // (may shrink to zero so the run button never clips at the minimum window
    // width) and the run button is pinned RIGHT-aligned on the SAME row — the
    // stretch between them absorbs the middle instead of the title expanding
    // up to the old 380 px claim, so every individual tool shows
    // "<title> ... <Run button>" with the button on the row's right edge.
    m_toolTitle->setMinimumWidth(0);
    m_toolTitle->setMaximumWidth(380);
    detailHeader->addWidget(m_toolTitle, 0, Qt::AlignVCenter);
    detailHeader->addStretch();
    m_toolRunButton = makeButton(tr("Run Tool"), detail);
    m_toolRunButton->setEnabled(false);
    QToolTip::add(m_toolRunButton, tr(
        "Select a repair drive, or choose Host Maintenance on the protected "
        "running-host card."));
    connect(m_toolRunButton, SIGNAL(clicked()), this, SLOT(runSelectedTool()));
    detailHeader->addWidget(m_toolRunButton, 0, Qt::AlignVCenter);
    m_toolDescription = new QLabel(
        tr("Select a tool to review its repair action."), detail);
    m_toolDescription->setTextFormat(Qt::PlainText);
    m_toolDescription->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    detailLayout->addWidget(m_toolDescription);
    m_toolPlanStatus = new QLabel(detail);
    m_toolPlanStatus->setTextFormat(Qt::PlainText);
    m_toolPlanStatus->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    QFont planStatusFont = m_toolPlanStatus->font();
    planStatusFont.setBold(true);
    m_toolPlanStatus->setFont(planStatusFont);
    detailLayout->addWidget(m_toolPlanStatus);
    detailLayout->addStretch();

    QValueList<int> splitterSizes;
    // Modern parity (item 6): ~50/50 panes (450/550, matching the modern
    // repair splitter's 9/11 stretch) instead of the lopsided KeepSize/Stretch
    // default, so the tools list and the Selected tool pane split evenly.
    splitterSizes.append(450);
    splitterSizes.append(550);
    splitter->setSizes(splitterSizes);

    layout->addWidget(vertical, 1);

    // Cycle 13: no legacy-only elevation section (modern has none); the
    // elevation state stays visible in Settings (privileged-authorization
    // support) and the status bar, and the deferred Authorize affordance
    // lives on the Systems page. The internal elevation re-resolution used
    // by the Authorize flow is unchanged.

    // Round 21: the bottom gate-hint label ("Run diagnostics for this scope
    // to unlock the gated actions...") is gone — the modern Repair page has
    // no such text: the gating reasons live in the plan readiness line (plan
    // box) and the Run Full Repair / Run Tool tooltips, so the freed row
    // lets the tools/Selected tool boxes expand down to fill the space
    // (the trailing stretch below).
    layout->addStretch();

    registerGroupBox(planBox);
    registerGroupBox(tools);
    registerGroupBox(detail);
    return page;
}

QWidget *LegacyMainWindow::buildChrootShellTab()
{
    QWidget *page = new QWidget(m_tabs);
    QVBoxLayout *layout = new QVBoxLayout(page, 10, 8);

    // Modern parity: the heading and tab label switch between "Chroot shell"
    // (offline target) and "Host shell" (Host Maintenance) in
    // updateChrootShellMode(); the scope label mirrors the modern page.
    QHBoxLayout *headingRow = new QHBoxLayout(layout);
    headingRow->setSpacing(6);
    m_chrootHeading = makeSectionTitle(tr("Chroot shell"), page);
    headingRow->addWidget(m_chrootHeading);
    // Cycle 15: a Preferred policy keeps the title at its text width so
    // the (i) button sits immediately after it on every page.
    m_chrootHeading->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
    // Round 21: the page's static "Run a command ... as root ..." notice is
    // gone — its content moved into this "i" popup, aligned with the TUXEDO
    // modern "i" wording (both scopes in one text).
    headingRow->addWidget(makeHelpButton(page, tr("Chroot shell"), tr(
        "Run a command on the running host as root (Host shell) or inside "
        "the selected repair system as root (Chroot shell); sudo is not "
        "needed in either. Host-shell commands are executed directly on the "
        "active system; chroot commands are executed one at a time in a "
        "fresh chroot and cannot answer interactive prompts, so "
        "non-interactive flags such as apt-get -y upgrade remain recommended "
        "for unattended runs. Output is kept in this window and in the "
        "application log. The helper's read-only probes gate the command "
        "field; while the shell is unavailable the page shows the exact "
        "reason below the field.")));
    headingRow->addStretch();
    m_chrootScopeLabel = new QLabel(tr("Target: none selected"), page);
    m_chrootScopeLabel->setTextFormat(Qt::PlainText);
    m_chrootScopeLabel->setAlignment(Qt::WordBreak | Qt::AlignRight | Qt::AlignVCenter);
    headingRow->addWidget(m_chrootScopeLabel, 0, Qt::AlignVCenter);

    m_chrootReasonLabel = new QLabel(page);
    m_chrootReasonLabel->setTextFormat(Qt::PlainText);
    m_chrootReasonLabel->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    // Round 21: the label no longer carries the static scope notice (that
    // text moved into the "i" popup above).  It now shows ONLY the
    // availability reason while the shell is greyed out (no target committed,
    // no authorization, the helper's read-only probe refusal, ...) and hides
    // when the shell is ready — the freed row moves the command frame up.
    m_chrootReasonLabel->hide();
    layout->addWidget(m_chrootReasonLabel);

    m_chrootGroup = new QGroupBox(page);
    QVBoxLayout *shellLayout = new QVBoxLayout(m_chrootGroup, 8, 6);
    m_chrootCommandHeading = makeSectionTitle(tr("Command"), m_chrootGroup);
    shellLayout->addWidget(m_chrootCommandHeading);

    // Round 31 (Debian Legacy #9, modern parity): the round-21 hint label
    // above the command row is gone — the command field shows the hint as a
    // lighter-grey in-field placeholder INSIDE the "Command:" input line
    // (ShellCommandLineEdit::setPlaceholderText, painted whenever the field is
    // empty; typing hides it).  The hint keeps the Etch-era apt wording (the
    // audit decision: on Etch the shell hint's specific form is always apt).
    QHBoxLayout *commandRow = new QHBoxLayout(shellLayout);
    commandRow->setSpacing(6);
    commandRow->addWidget(new QLabel(tr("Command:"), m_chrootGroup));
    m_shellCommandEdit = new ShellCommandLineEdit(this, m_chrootGroup);
    m_shellCommandEdit->setEnabled(false);
    m_shellCommandEdit->setText(QString::null);
    m_shellCommandEdit->setPlaceholderText(
        tr("Command to run, for example: apt-get update, apt-get upgrade"));
    QToolTip::add(m_shellCommandEdit, tr(
        "One reviewed command string, passed to the helper as a single "
        "argument (no shell interpolation by the GUI)."));
    commandRow->addWidget(m_shellCommandEdit, 1);
    // Enter in the command field runs the command through the same path as the
    // Run button (modern returnPressed parity); the Run button stays the
    // default action of the row.
    connect(m_shellCommandEdit, SIGNAL(returnPressed()), this,
            SLOT(runChrootShell()));
    m_shellRunButton = makeButton(tr("Run Command"), m_chrootGroup);
    m_shellRunButton->setEnabled(false);
    m_shellRunButton->setDefault(true);
    connect(m_shellRunButton, SIGNAL(clicked()), this, SLOT(runChrootShell()));
    commandRow->addWidget(m_shellRunButton);
    // Round 21 (modern cancel-surface parity): Cancel for a running shell
    // command (there is no result popup for the shell - the transcript
    // streams inline into this tab).  Armed only while a shell command owns
    // the helper (updateActionStates()), disabled before the run starts and
    // after it completes/aborts; the click drives the documented legacy
    // cancel path exactly like the repair popup's Cancel.
    m_shellCancelButton = makeButton(tr("Cancel"), m_chrootGroup);
    m_shellCancelButton->setEnabled(false);
    QToolTip::add(m_shellCancelButton, tr(
        "Stop the running shell command."));
    connect(m_shellCancelButton, SIGNAL(clicked()), this,
            SLOT(cancelShellOperation()));
    commandRow->addWidget(m_shellCancelButton);
    m_shellClearButton = makeButton(tr("Clear Output"), m_chrootGroup);
    m_shellClearButton->setEnabled(false);
    connect(m_shellClearButton, SIGNAL(clicked()), this, SLOT(clearChrootOutput()));
    commandRow->addWidget(m_shellClearButton);

    // Round 31 (Debian Legacy #9, item 2): the round-21 "The helper exposes
    // `shell`/`host-shell`..." static text is gone.  The pane shows the exact
    // modern empty-state placeholder ("Command output will appear here.") as
    // an in-field greyed hint — TEMPORARY: it disappears on its own the moment
    // the first command output streams into the window (and re-appears after
    // Clear Output empties the pane, exactly like the modern placeholder).
    m_shellOutput = new PlaceholderTextEdit(m_chrootGroup);
    m_shellOutput->setReadOnly(true);
    m_shellOutput->setTextFormat(Qt::LogText);
    m_shellOutput->setMinimumHeight(120);
    static_cast<PlaceholderTextEdit *>(m_shellOutput)->setPlaceholderText(
        tr("Command output will appear here."));
    shellLayout->addWidget(m_shellOutput, 1);

    // Prompt-flow contract (L1/L2): the shell run's interactive question is
    // answered INLINE in this tab, not a separate popup.  A persistent hint
    // label + input line + Send button sit directly under the output pane,
    // hidden/disabled until a shell PROMPT record arrives.  The hint is the
    // free-text shell hint (not the numbered repair-menu hint); the answer
    // travels over the same mode-600 menu-answer file as the repair menus.
    m_shellPromptHint = new QLabel(tr("Type the answer (Enter = Send)"),
                                   m_chrootGroup);
    m_shellPromptHint->hide();
    shellLayout->addWidget(m_shellPromptHint);
    QHBoxLayout *shellPromptRow = new QHBoxLayout(shellLayout);
    shellPromptRow->setSpacing(6);
    m_shellPromptInput = new PromptAnswerLineEdit(m_chrootGroup);
    m_shellPromptInput->setEnabled(false);
    m_shellPromptSend = new QPushButton(tr("Send"), m_chrootGroup);
    m_shellPromptSend->setEnabled(false);
    shellPromptRow->addWidget(m_shellPromptInput, 1);
    shellPromptRow->addWidget(m_shellPromptSend);
    QObject::connect(m_shellPromptInput, SIGNAL(returnPressed()), this,
                     SLOT(sendPromptAnswer()));
    QObject::connect(m_shellPromptSend, SIGNAL(clicked()), this,
                     SLOT(sendPromptAnswer()));

    registerGroupBox(m_chrootGroup);
    layout->addWidget(m_chrootGroup, 1);
    return page;
}

QWidget *LegacyMainWindow::buildFileCopyTab()
{
    QWidget *page = new QWidget(m_tabs);
    QVBoxLayout *layout = new QVBoxLayout(page, 8, 6);

    // Modern heading row (buildFileCopyPage parity): the File copy section
    // title + (i), then the live scope label, then the two primary actions
    // share the page-title row; both actions stay disabled until the
    // direction and scope are ready.
    QHBoxLayout *headingRow = new QHBoxLayout(layout);
    headingRow->setSpacing(6);
    m_fileCopyHeading = makeSectionTitle(tr("File copy"), page);
    headingRow->addWidget(m_fileCopyHeading);
    // Cycle 15: a Preferred policy keeps the title at its text width so
    // the (i) button sits immediately after it on every page.
    m_fileCopyHeading->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
    headingRow->addWidget(makeHelpButton(page, tr("File copy"), tr(
        "Copy and verify files in either direction through the guarded "
        "helper (cp -a plus ownership restoration and a per-file "
        "byte-compare). The helper's file-copy probe gates the controls and "
        "keeps the direction and path containment checks.")));
    headingRow->addStretch();
    m_fileCopyScopeLabel = new QLabel(tr("Target: none selected"), page);
    m_fileCopyScopeLabel->setTextFormat(Qt::PlainText);
    m_fileCopyScopeLabel->setAlignment(Qt::WordBreak | Qt::AlignRight | Qt::AlignVCenter);
    headingRow->addWidget(m_fileCopyScopeLabel, 1);
    m_fileCopyPreviewButton = makeButton(tr("Preview Changes"), page);
    m_fileCopyPreviewButton->setEnabled(false);
    QToolTip::add(m_fileCopyPreviewButton, tr(
        "Run a copy dry-run through the guarded helper. No files are changed."));
    connect(m_fileCopyPreviewButton, SIGNAL(clicked()), this, SLOT(fileCopyPreview()));
    headingRow->addWidget(m_fileCopyPreviewButton, 0, Qt::AlignVCenter);
    m_fileCopyRunButton = makeButton(tr("Copy and Verify"), page);
    m_fileCopyRunButton->setEnabled(false);
    QToolTip::add(m_fileCopyRunButton, tr(
        "Copy staged items and verify the result. Existing destination names "
        "are overwritten when source content differs; unrelated destination "
        "files are never deleted."));
    connect(m_fileCopyRunButton, SIGNAL(clicked()), this, SLOT(fileCopyRun()));
    headingRow->addWidget(m_fileCopyRunButton, 0, Qt::AlignVCenter);

    // Direction row (modern parity: Host -> Repair / Repair -> Host). The
    // sub-layout is constructed WITHOUT a parent and added via addLayout(),
    // matching the existing-tab pattern: a layout constructed with a parent
    // layout is added automatically at construction, so an explicit
    // addLayout() would parent it twice ("QLayout::addChildLayout: layout
    // already has a parent") and double-free it at shutdown.
    QHBoxLayout *directionRow = new QHBoxLayout();
    directionRow->setSpacing(6);
    directionRow->addWidget(new QLabel(tr("Direction:"), page));
    m_fileCopyDirectionCombo = new QComboBox(page);
    m_fileCopyDirectionCombo->insertItem(QString::fromUtf8("Host \xE2\x86\x92 Repair"));
    m_fileCopyDirectionCombo->insertItem(QString::fromUtf8("Repair \xE2\x86\x92 Host"));
    m_fileCopyDirectionCombo->setEnabled(false);
    QToolTip::add(m_fileCopyDirectionCombo, tr(
        "Choose which system supplies the source files and which system "
        "receives them."));
    connect(m_fileCopyDirectionCombo, SIGNAL(activated(int)), this, SLOT(fileCopyDirectionChanged()));
    directionRow->addWidget(m_fileCopyDirectionCombo);
    directionRow->addStretch();
    layout->addLayout(directionRow);

    // Cycle 14: the vertical splitter owns its panes from construction, so
    // the three groups are always visible inside them (the cycle-13 wrap
    // created the panes after the groups and the reparenting hid them on
    // Etch).
    QSplitter *fileCopySplitter = new QSplitter(Qt::Vertical, page);
    fileCopySplitter->setChildrenCollapsible(false);
    QWidget *sourcePane = new QWidget(fileCopySplitter);
    QVBoxLayout *sourcePaneLayout = new QVBoxLayout(sourcePane, 0, 0);
    QWidget *lowerPane = new QWidget(fileCopySplitter);
    QVBoxLayout *lowerPaneLayout = new QVBoxLayout(lowerPane, 0, 6);

    // Group 1: staged sources with the Add/Remove/Clear button row. The
    // title adapts per direction (modern updateFileCopyDirection).
    m_fileCopySourceGroup = new QGroupBox(sourcePane);
    QVBoxLayout *sourceGroupLayout = new QVBoxLayout(m_fileCopySourceGroup, 8, 4);
    m_fileCopySourceTitle = makeSectionTitle(
        tr("1. Select source files or folders from this host"),
        m_fileCopySourceGroup);
    // The group-1 title is the longest direction-adapted title: make it
    // wrap-capable (WordBreak + no minimum width) so a narrow details-less
    // pane can never clip it; the smoke's clipping gates exempt wrapping
    // section titles from the single-line width check.
    m_fileCopySourceTitle->setAlignment(Qt::WordBreak | Qt::AlignLeft | Qt::AlignVCenter);
    m_fileCopySourceTitle->setMinimumWidth(0);
    sourceGroupLayout->addWidget(m_fileCopySourceTitle);
    m_fileCopySourceList = new QListView(m_fileCopySourceGroup);
    addListViewColumn(m_fileCopySourceList, tr("Source"), 150);
    m_fileCopySourceList->setAllColumnsShowFocus(true);
    m_fileCopySourceList->setResizeMode(QListView::LastColumn);
    m_fileCopySourceList->setSorting(-1);
    m_fileCopySourceList->setEnabled(false);
    m_fileCopySourceList->setMinimumHeight(80);
    QToolTip::add(m_fileCopySourceList, tr(
        "Files and folders staged for the verified copy. The legacy backend "
        "copies with cp -a and restores ownership with chown --reference; "
        "every regular file is byte-compared after the copy."));
    sourceGroupLayout->addWidget(m_fileCopySourceList, 1);
    QHBoxLayout *sourceButtons = new QHBoxLayout(sourceGroupLayout);
    sourceButtons->setSpacing(6);
    m_fileCopyAddFilesButton = makeButton(tr("Add Files..."), m_fileCopySourceGroup);
    m_fileCopyAddFilesButton->setEnabled(false);
    connect(m_fileCopyAddFilesButton, SIGNAL(clicked()), this, SLOT(fileCopyAddFiles()));
    sourceButtons->addWidget(m_fileCopyAddFilesButton);
    m_fileCopyAddFolderButton = makeButton(tr("Add Folder..."), m_fileCopySourceGroup);
    m_fileCopyAddFolderButton->setEnabled(false);
    connect(m_fileCopyAddFolderButton, SIGNAL(clicked()), this, SLOT(fileCopyAddFolder()));
    sourceButtons->addWidget(m_fileCopyAddFolderButton);
    // Repair -> Host only (modern buildFileCopyPage parity): the repaired-system
    // source side gets the same read-only browse-target folder picker as the
    // destination side, staging the chosen in-chroot folder as a source. Hidden
    // in Host -> Repair, where the Add buttons already open host-native dialogs.
    m_fileCopyBrowseSourceButton = makeButton(tr("Browse..."), m_fileCopySourceGroup);
    m_fileCopyBrowseSourceButton->setEnabled(false);
    m_fileCopyBrowseSourceButton->hide();
    connect(m_fileCopyBrowseSourceButton, SIGNAL(clicked()), this, SLOT(fileCopyBrowseSource()));
    sourceButtons->addWidget(m_fileCopyBrowseSourceButton);
    m_fileCopyRemoveButton = makeButton(tr("Remove"), m_fileCopySourceGroup);
    m_fileCopyRemoveButton->setEnabled(false);
    connect(m_fileCopyRemoveButton, SIGNAL(clicked()), this, SLOT(fileCopyRemoveSelected()));
    sourceButtons->addWidget(m_fileCopyRemoveButton);
    sourceButtons->addStretch();
    m_fileCopyClearButton = makeButton(tr("Clear"), m_fileCopySourceGroup);
    m_fileCopyClearButton->setEnabled(false);
    QToolTip::add(m_fileCopyClearButton, tr(
        "Clear the staged source list (nothing is copied or deleted)."));
    connect(m_fileCopyClearButton, SIGNAL(clicked()), this, SLOT(fileCopyClearStaging()));
    sourceButtons->addWidget(m_fileCopyClearButton);
    registerGroupBox(m_fileCopySourceGroup);
    sourcePaneLayout->addWidget(m_fileCopySourceGroup);

    // Group 2: destination with the Browse Target Folders... action.
    m_fileCopyDestinationGroup = new QGroupBox(lowerPane);
    QVBoxLayout *destinationLayout = new QVBoxLayout(m_fileCopyDestinationGroup, 8, 4);
    m_fileCopyDestinationTitle = makeSectionTitle(
        tr("2. Choose destination in repaired system"),
        m_fileCopyDestinationGroup);
    destinationLayout->addWidget(m_fileCopyDestinationTitle);
    QHBoxLayout *destinationRow = new QHBoxLayout(destinationLayout);
    destinationRow->setSpacing(6);
    m_fileCopyDestinationEdit = new QLineEdit(m_fileCopyDestinationGroup);
    m_fileCopyDestinationEdit->setEnabled(false);
    m_fileCopyDestinationEdit->setText(tr(
        "unavailable: see the helper's Legacy feature file-copy: probe reason above"));
    QToolTip::add(m_fileCopyDestinationEdit, tr(
        "An absolute path inside the selected repair system (Host to Repair) "
        "or on the running host (Repair to Host)."));
    destinationRow->addWidget(m_fileCopyDestinationEdit, 1);
    m_fileCopyBrowseButton = makeButton(tr("Browse Target Folders..."), m_fileCopyDestinationGroup);
    m_fileCopyBrowseButton->setEnabled(false);
    QToolTip::add(m_fileCopyBrowseButton, tr(
        "Browse the selected repair system through the helper's temporary "
        "read-only mounts and choose an absolute destination path. No target "
        "files are changed while browsing."));
    connect(m_fileCopyBrowseButton, SIGNAL(clicked()), this, SLOT(fileCopyBrowse()));
    destinationRow->addWidget(m_fileCopyBrowseButton);
    registerGroupBox(m_fileCopyDestinationGroup);
    lowerPaneLayout->addWidget(m_fileCopyDestinationGroup);

    // Group 3: ownership and copy policy (the legacy equivalent set).
    m_fileCopyOptionsGroup = new QGroupBox(lowerPane);
    QVBoxLayout *optionsLayout = new QVBoxLayout(m_fileCopyOptionsGroup, 8, 4);
    m_fileCopyOptionsTitle = makeSectionTitle(
        tr("3. Ownership and copy policy"),
        m_fileCopyOptionsGroup);
    optionsLayout->addWidget(m_fileCopyOptionsTitle);
    QHBoxLayout *ownershipRow = new QHBoxLayout(optionsLayout);
    ownershipRow->setSpacing(6);
    ownershipRow->addWidget(new QLabel(tr("Ownership:"), m_fileCopyOptionsGroup));
    m_fileCopyOwnershipCombo = new QComboBox(m_fileCopyOptionsGroup);
    m_fileCopyOwnershipCombo->insertItem(tr(
        "Smart destination ownership (recommended)"));
    m_fileCopyOwnershipCombo->insertItem(tr(
        "Preserve source numeric UID/GID"));
    m_fileCopyOwnershipCombo->setEnabled(false);
    QToolTip::add(m_fileCopyOwnershipCombo, tr(
        "Smart mode validates UID/GID identity mapping across the two systems "
        "and falls back to the destination-directory owner when the same "
        "numeric ID means a different account (the legacy backend implements "
        "it with chown --reference)."));
    ownershipRow->addWidget(m_fileCopyOwnershipCombo, 1);
    registerGroupBox(m_fileCopyOptionsGroup);
    lowerPaneLayout->addWidget(m_fileCopyOptionsGroup);
    // Round 67 (File Copy top-alignment): Qt3's qGeomCalc splits the lower
    // pane's surplus equally between the two group frames when no item has a
    // stretch factor (QBoxLayout "distributed according to sizePolicy" =
    // 50/50 for two Preferred boxes) — each box then grew ~half the surplus,
    // its fixed-height section title pinned at the top and its content row
    // centered in the leftover cell: a large gap between the title and its
    // box, and section 3 pushed down.  The trailing stretch makes the groups
    // pin at their sizeHint (they are non-expanding with stretch 0 while a
    // stretch item exists), so the titles sit tight under the previous
    // section and the boxes hug their content; the surplus parks below
    // section 3 inside the fixed-height lower pane, exactly like the modern
    // page's trailing placeholder.
    lowerPaneLayout->addStretch(1);

    sourcePane->setMinimumHeight(120);
    lowerPane->setMinimumHeight(140);
    QValueList<int> fileCopySizes;
    fileCopySizes.append(190);
    fileCopySizes.append(260);
    fileCopySplitter->setSizes(fileCopySizes);
    layout->addWidget(fileCopySplitter, 1);

    return page;
}

QWidget *LegacyMainWindow::buildLogTab()
{
    QWidget *page = new QWidget(m_tabs);
    QVBoxLayout *layout = new QVBoxLayout(page, 8, 4);

    QHBoxLayout *logsHeadingRow = new QHBoxLayout(layout);
    logsHeadingRow->setSpacing(6);
    // Cycle 13: the page heading is "Application log" (modern parity); the
    // tab keeps the "Logs" label.
    m_logsHeading = makeSectionTitle(tr("Application log"), page);
    logsHeadingRow->addWidget(m_logsHeading);
    // Cycle 15: a Preferred policy keeps the title at its text width so
    // the (i) button sits immediately after it on every page.
    m_logsHeading->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
    logsHeadingRow->addWidget(makeHelpButton(page, tr("Application log"), tr(
        "The complete session register; Save As... writes every entry even "
        "while a filter hides lines. If a writable system share mount exists "
        "at /host, Save As... starts there; otherwise the log directory is "
        "the fallback. Prior session files are listed read-only.")));
    logsHeadingRow->addStretch();

    QSplitter *splitter = new QSplitter(Qt::Horizontal, page);
    splitter->setChildrenCollapsible(false);

    QGroupBox *sessions = new QGroupBox(splitter);
    // Cycle 14: the session pane keeps a floor so Add Note and Refresh are
    // never cut off at the default window size.
    sessions->setMinimumWidth(240);
    QVBoxLayout *sessionLayout = new QVBoxLayout(sessions, 8, 4);
    sessionLayout->addWidget(makeSectionTitle(tr("Session logs"), sessions));
    m_sessionLogList = new QListView(sessions);
    addListViewColumn(m_sessionLogList, tr("Session"), 150);
    m_sessionLogList->setAllColumnsShowFocus(true);
    m_sessionLogList->setResizeMode(QListView::LastColumn);
    m_sessionLogList->setMinimumHeight(80);
    QToolTip::add(m_sessionLogList, tr(
        "The first entry is the live session; earlier files in the log "
        "directory are listed read-only below it."));
    connect(m_sessionLogList, SIGNAL(selectionChanged(QListViewItem *)),
            this, SLOT(sessionLogSelectionChanged(QListViewItem *)));
    sessionLayout->addWidget(m_sessionLogList, 1);

    // Modern session controls (two compact rows so the narrow frame fits):
    // New Session Log, Add Note, Delete (prior files only) and Refresh.
    QGridLayout *sessionButtons = new QGridLayout(sessionLayout, 2, 2, 4);
    sessionButtons->setMargin(4);
    m_newSessionLogButton = makeButton(tr("New Session Log"), sessions);
    QToolTip::add(m_newSessionLogButton, tr(
        "Close the active session file; it becomes a prior session and the next "
        "log entry starts a new file."));
    connect(m_newSessionLogButton, SIGNAL(clicked()), this, SLOT(startNewSessionLog()));
    sessionButtons->addWidget(m_newSessionLogButton, 0, 0);
    m_addNoteButton = makeButton(tr("Add Note"), sessions);
    QToolTip::add(m_addNoteButton, tr(
        "Append a NOTE entry to the live session register."));
    connect(m_addNoteButton, SIGNAL(clicked()), this, SLOT(addSessionNote()));
    sessionButtons->addWidget(m_addNoteButton, 0, 1);
    m_deleteSessionLogButton = makeButton(tr("Delete"), sessions);
    m_deleteSessionLogButton->setEnabled(false);
    QToolTip::add(m_deleteSessionLogButton, tr(
        "Delete the selected prior session file (the live session is never "
        "deleted)."));
    connect(m_deleteSessionLogButton, SIGNAL(clicked()), this, SLOT(deleteSelectedSessionLog()));
    sessionButtons->addWidget(m_deleteSessionLogButton, 1, 0);
    QPushButton *refresh = makeButton(tr("Refresh"), sessions);
    connect(refresh, SIGNAL(clicked()), this, SLOT(refreshSessionLogs()));
    sessionButtons->addWidget(refresh, 1, 1);

    QGroupBox *applicationLog = new QGroupBox(splitter);
    // Cycle 14: the log pane keeps a readability floor; the session pane
    // takes the leftover.
    applicationLog->setMinimumWidth(480);
    QVBoxLayout *logLayout = new QVBoxLayout(applicationLog, 8, 4);

    // Modern top row: the "Application log" section title on the left and the
    // Save As... / Clear Register actions on the right, above the
    // search/filter row (the page heading "Logs" stays above the splitter).
    QHBoxLayout *logHeader = new QHBoxLayout(logLayout);
    logHeader->setSpacing(4);
    logHeader->addWidget(makeSectionTitle(tr("Application log"), applicationLog));
    logHeader->addStretch();
    QPushButton *save = makeButton(tr("Save As..."), applicationLog);
    QToolTip::add(save, tr(
        "Save the complete session log (all entries, not just the current filter)."));
    connect(save, SIGNAL(clicked()), this, SLOT(saveLog()));
    logHeader->addWidget(save);
    QPushButton *clear = makeButton(tr("Clear Register"), applicationLog);
    QToolTip::add(clear, tr(
        "Clear the live register and view; prior session files are never "
        "modified."));
    connect(clear, SIGNAL(clicked()), this, SLOT(clearLog()));
    logHeader->addWidget(clear);

    QHBoxLayout *filterRow = new QHBoxLayout(logLayout);
    filterRow->setSpacing(4);
    filterRow->addWidget(new QLabel(tr("Search log:"), applicationLog));
    m_logSearchEdit = new QLineEdit(applicationLog);
    m_logSearchEdit->setText(QString::null);
    QToolTip::add(m_logSearchEdit, tr(
        "Type any characters to show matching log entries (case-insensitive). "
        "Save As always writes every entry."));
    connect(m_logSearchEdit, SIGNAL(textChanged(const QString &)), this, SLOT(logSearchChanged()));
    filterRow->addWidget(m_logSearchEdit, 1);
    filterRow->addWidget(new QLabel(tr("Filter:"), applicationLog));
    m_logFilterCombo = new QComboBox(applicationLog);
    // 1:1 with the modern combo: All entries / Diagnostics / Repairs / the
    // three workflows / the 18 diagnostic section titles. Qt3's QComboBox
    // cannot carry per-item data, so the index semantics live in
    // logFilterSpecs plus the diagnosticSpecs order below.
    for (int i = 0; i < logFilterSpecCount; ++i) {
        m_logFilterCombo->insertItem(tr(logFilterSpecs[i].title));
    }
    for (int i = 0; i < diagnosticSpecCount; ++i) {
        m_logFilterCombo->insertItem(tr(diagnosticSpecs[i].title));
    }
    QToolTip::add(m_logFilterCombo, tr(
        "Filter the visible log by entry kind. Selecting a diagnostic section "
        "shows the lines captured for that section; a workflow filter such as "
        "File system repair or Package repair shows its mapped repair lines "
        "(File copy has no lines in this frontend). Save As always writes "
        "every entry."));
    connect(m_logFilterCombo, SIGNAL(activated(int)), this, SLOT(logFilterChanged()));
    filterRow->addWidget(m_logFilterCombo);

    // Modern prior-log banner: shown only while a prior session file is
    // selected (read-only).
    m_priorLogBanner = new QLabel(applicationLog);
    m_priorLogBanner->setFrameShape(QFrame::StyledPanel);
    m_priorLogBanner->setMargin(4);
    m_priorLogBanner->setAlignment(Qt::WordBreak | Qt::AlignLeft | Qt::AlignVCenter);
    m_priorLogBanner->hide();
    logLayout->addWidget(m_priorLogBanner);

    m_logView = new QTextEdit(applicationLog);
    m_logView->setReadOnly(true);
    // Item 1: Qt 3.3.7's Qt::LogText mode disables word wrap entirely, so the
    // Logs view uses Qt::PlainText with a fixed-pitch font (the LogText
    // appearance) so the widget-width wrap below actually takes effect; long
    // lines wrap down instead of clipping horizontally when the window is
    // narrowed. The wrap preference still persists through /logs/wrapLines.
    m_logView->setTextFormat(Qt::PlainText);
    QFont logMono(QString::fromLatin1("monospace"));
    logMono.setStyleHint(QFont::TypeWriter);
    m_logView->setFont(logMono);
    m_logView->setWordWrap(m_logWrapEnabled ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);
    // Cycle 14: long lines wrap at the widget width with no horizontal
    // scrollbar while wrapping is on (the default); turning wrap off
    // re-enables the scrollbar.
    m_logView->setHScrollBarMode(m_logWrapEnabled ? QScrollView::AlwaysOff
                                                 : QScrollView::Auto);
    m_logView->setMinimumHeight(80);
    logLayout->addWidget(m_logView, 1);

    registerGroupBox(sessions);
    registerGroupBox(applicationLog);
    QValueList<int> sizes;
    sizes.append(220);
    sizes.append(760);
    splitter->setSizes(sizes);
    layout->addWidget(splitter, 1);
    return page;
}

QWidget *LegacyMainWindow::buildSettingsTab()
{
    QWidget *page = new QWidget(m_tabs);
    QVBoxLayout *pageLayout = new QVBoxLayout(page, 8, 6);

    QHBoxLayout *settingsHeadingRow = new QHBoxLayout(pageLayout);
    settingsHeadingRow->setSpacing(6);
    m_settingsHeading = makeSectionTitle(tr("Settings"), page);
    settingsHeadingRow->addWidget(m_settingsHeading);
    // Cycle 15: a Preferred policy keeps the title at its text width so
    // the (i) button sits immediately after it on every page.
    m_settingsHeading->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
    settingsHeadingRow->addWidget(makeHelpButton(page, tr("Settings"), tr(
        "Settings are stored per user under ~/.qt/, one file per settings "
        "group (devicesrc, logsrc, diagnosticsrc, repairrc), and are saved "
        "immediately on every change and on close. The same per-user location "
        "is used whether the GUI runs as the logged-in user or as root (sudo "
        "preserves the home directory).")));
    settingsHeadingRow->addStretch();

    // Modern Settings is a scrollable page. Qt3 has no QScrollArea, so the
    // Qt3 QScrollView hosts the content; AutoOneFit keeps the content at the
    // viewport width and only scrolls vertically when the groups exceed the
    // 1024x768 contract height.
    QScrollView *scroll = new QScrollView(page);
    scroll->setResizePolicy(QScrollView::AutoOneFit);
    scroll->setVScrollBarMode(QScrollView::Auto);
    scroll->setHScrollBarMode(QScrollView::AlwaysOff);
    scroll->setFrameShape(QFrame::NoFrame);
    pageLayout->addWidget(scroll, 1);

    QWidget *content = new QWidget(scroll->viewport());
    scroll->addChild(content);
    m_settingsContent = content;
    QVBoxLayout *layout = new QVBoxLayout(content, 6, 6);

    // Modern Settings parity: the lightweight preferences row + Full Repair
    // plan live in an UPPER pane and the capabilities + mandatory safety
    // groups in a LOWER pane, separated by a draggable vertical QSplitter
    // (non-collapsible children, a grabbable handle). The outer QScrollView
    // stays so the whole page still scrolls vertically when the window is
    // short. The splitter owns its panes from construction (the cycle-14
    // rule: create the panes before the groups so they are never hidden by
    // a later reparent on Etch).
    QSplitter *settingsSplitter = new QSplitter(Qt::Vertical, content);
    settingsSplitter->setChildrenCollapsible(false);
    QWidget *upperPane = new QWidget(settingsSplitter);
    QVBoxLayout *upperLayout = new QVBoxLayout(upperPane, 0, 6);
    QWidget *lowerPane = new QWidget(settingsSplitter);
    QVBoxLayout *lowerLayout = new QVBoxLayout(lowerPane, 0, 6);

    // Modern parity: device discovery + diagnostics share one side-by-side
    // preferences row, then the Full Repair plan, then the capabilities
    // section and finally the mandatory safety list, in the modern order.
    // Every frame is titleless with a sectionTitle label inside: Qt3's
    // KDE/Etch style clips QGroupBox titles at the top.
    QGroupBox *discovery = new QGroupBox(upperPane);
    QVBoxLayout *discoveryLayout = new QVBoxLayout(discovery, 6, 3);
    discoveryLayout->addWidget(makeSectionTitle(tr("Device discovery"), discovery));
    m_showNonLinuxCheck = new QCheckBox(
        tr("Show devices without an identified Linux installation"), discovery);
    m_showRemovableCheck = new QCheckBox(
        tr("Show removable and USB storage"), discovery);
    m_showEncryptedCheck = new QCheckBox(
        tr("Show encrypted devices before unlocking"), discovery);
    QToolTip::add(m_showNonLinuxCheck, tr(
        "When off, drives without a visible Linux filesystem are hidden unless "
        "they still contain an encrypted device and encrypted devices are shown."));
    QToolTip::add(m_showRemovableCheck, tr(
        "When off, removable and USB drives are hidden from the repair-target list."));
    QToolTip::add(m_showEncryptedCheck, tr(
        "When off, drives with an encrypted device are hidden until the volume "
        "is unlocked."));
    discoveryLayout->addWidget(m_showNonLinuxCheck);
    discoveryLayout->addWidget(m_showRemovableCheck);
    discoveryLayout->addWidget(m_showEncryptedCheck);
    connect(m_showNonLinuxCheck, SIGNAL(toggled(bool)), this, SLOT(deviceFilterChanged(bool)));
    connect(m_showRemovableCheck, SIGNAL(toggled(bool)), this, SLOT(deviceFilterChanged(bool)));
    connect(m_showEncryptedCheck, SIGNAL(toggled(bool)), this, SLOT(deviceFilterChanged(bool)));
    // No trailing stretch: the box is pinned at its content height (below),
    // so there is no extra height to absorb - the stretch only rendered the
    // pin's extra height as padding below the lowest checkbox.

    QGroupBox *diagnostics = new QGroupBox(upperPane);
    QVBoxLayout *diagnosticsLayout = new QVBoxLayout(diagnostics, 6, 3);
    diagnosticsLayout->addWidget(makeSectionTitle(tr("Diagnostics"), diagnostics));
    m_autoRefreshCheck = new QCheckBox(
        tr("Automatically regenerate read-only diagnostics after repairs or target changes"),
        diagnostics);
    QToolTip::add(m_autoRefreshCheck, tr(
        "Regenerates the cached read-only diagnostics for the current scope "
        "after an operation that invalidates them (LUKS unlock, target "
        "configuration edit). It runs only inside an already authorized "
        "administrator session and never opens an authorization prompt by "
        "itself."));
    connect(m_autoRefreshCheck, SIGNAL(toggled(bool)), this, SLOT(autoRefreshToggled(bool)));
    diagnosticsLayout->addWidget(m_autoRefreshCheck);
    // Empty-space absorber below the checkbox (the same mechanism the Full
    // Repair plan box and the discovery box use): it takes all extra height
    // the box is given, so the title and the checkbox stay pinned to the
    // top-left with fixed gaps and the extra pools as empty space below the
    // "Automatically regenerate..." checkbox - without it Qt3 spreads the
    // extra evenly between the title and the checkbox.
    diagnosticsLayout->addStretch(1);

    // Modern parity: Device discovery and Diagnostics share one side-by-side
    // row (each stretch 1) so the heavier Full Repair plan and the capability
    // table below gain vertical room. The sub-layout is parentless and owned
    // by addLayout() (see the file-copy direction-row guard).
    QHBoxLayout *preferencesRow = new QHBoxLayout();
    preferencesRow->setSpacing(6);
    preferencesRow->addWidget(discovery, 1);
    preferencesRow->addWidget(diagnostics, 1);
    upperLayout->addLayout(preferencesRow);
    // Pin both preferences boxes 10px below the old 108px row height: the
    // 108px pin was one line (20px) taller than the boxes' content, and the
    // removed trailing stretches rendered that extra as padding below the
    // lowest checkbox; 98px leaves half of it. Nothing above the plan box
    // stretches or distributes on a splitter drag - the pane's growth and
    // squeeze land entirely on the plan box, exactly like the repair tab's
    // plan frame.
    discovery->setMinimumHeight(98);
    discovery->setMaximumHeight(98);
    diagnostics->setMinimumHeight(98);
    diagnostics->setMaximumHeight(98);

    // Modern Settings -> Full Repair plan. The checkboxes drive the Repair
    // plan section, the tools "Full Repair" column and Run Full Repair.
    // Modern parity: the 11 stages flow into two columns (6 left, 5 right) in
    // the planSpecs order instead of one-per-row.
    QGroupBox *planGroup = new QGroupBox(upperPane);
    QVBoxLayout *planGroupLayout = new QVBoxLayout(planGroup, 6, 3);
    planGroupLayout->addWidget(makeSectionTitle(tr("Full Repair plan"), planGroup));
    QGridLayout *planGrid = new QGridLayout(planGroupLayout, 6, 2, 4);
    planGrid->setColStretch(0, 1);
    planGrid->setColStretch(1, 1);
    m_planChecks.clear();
    for (int i = 0; i < planSpecCount; ++i) {
        QCheckBox *check = new QCheckBox(tr(planSpecs[i].checkLabel), planGroup);
        check->setChecked(planSpecs[i].defaultChecked);
        // Fixed vertical policy (like the section title): the checkbox rows
        // never grow or shrink, so the two-column list stays pinned to the
        // top of the box with fixed gaps between the frame, the title and
        // the rows - the spacing never changes, and any extra height the
        // box gains on a drag-down shows as empty space below the list,
        // exactly like the repair tab's Full Repair plan box.
        check->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,
                                          QSizePolicy::Fixed));
        QToolTip::add(check, tr(
            "Include the %1 stage in the Full Repair plan. The stage runs in "
            "the plan order shown on the Repair tab.").arg(tr(planSpecs[i].title)));
        connect(check, SIGNAL(toggled(bool)), this, SLOT(planCheckboxChanged()));
        planGrid->addWidget(check, i % 6, i / 6);
        m_planChecks.push_back(check);
    }
    // Pin the title and the two-column list to the top of the box: this
    // empty-space absorber takes ALL extra height the box gains on a
    // drag-down, below the list. It is not a visual stretch - without it
    // Qt3 spreads the extra height as wider gaps between the frame, the
    // title, the list and the rows (everything slides down and the spacing
    // adjusts, the behavior that was just observed). With it, the title and
    // the rows stay at their fixed gaps and the extra shows as empty space
    // below the list - the same mechanism the Device Discovery box has
    // always used, and what keeps the repair tab's plan box content pinned.
    planGroupLayout->addStretch(1);
    // Make the Settings plan box behave exactly like the repair tab's plan
    // frame on a drag: it shrinks toward 75px when the handle is dragged up
    // and grows by about three checkbox rows past its ~188px default when
    // dragged down (max 250 - the earlier 404 was wrong; the box only needs
    // ~3 rows of headroom). No stretch factor: the discovery/diagnostics row
    // above is fully pinned (see below), so this box is the pane's only
    // growable item and receives the pane's pixels directly.
    planGroup->setMinimumHeight(75);
    planGroup->setMaximumHeight(250);
    upperLayout->addWidget(planGroup);

    // Modern parity (item 1): the Settings page no longer exposes the log-wrap
    // toggle (wrap is on by default in the modern backend); the View menu's
    // "Wrap Log Lines" item remains the only wrap control. The m_logWrapCheck
    // member stays declared (always null) so the wrap state, the View-menu
    // sync and the settings persistence keep their existing code paths.

    // Modern Capabilities and dependencies: the section title, the three
    // summary labels (Distribution / Package manager family / KAuth build
    // support), the 7-column probe table (the trailing Action column shows an
    // "Install" text cell for missing+installable rows) and the Refresh
    // Capabilities / Install Missing Support... actions. Every host probe is a
    // read-only PATH search (never executed); the Repair-target scope re-reads
    // only cached evidence.
    QGroupBox *capability = new QGroupBox(lowerPane);
    QVBoxLayout *capabilityLayout = new QVBoxLayout(capability, 6, 3);
    capabilityLayout->addWidget(
        makeSectionTitle(tr("Capabilities and dependencies"), capability));
    QGridLayout *identityGrid = new QGridLayout(capabilityLayout, 3, 2, 4);
    identityGrid->setColStretch(1, 1);
    m_capDistributionLabel = new QLabel(capability);
    m_capPackageManagerLabel = new QLabel(capability);
    m_capAuthLabel = new QLabel(capability);
    QLabel *capValues[] = {
        m_capDistributionLabel, m_capPackageManagerLabel, m_capAuthLabel
    };
    for (int i = 0; i < 3; ++i) {
        capValues[i]->setTextFormat(Qt::PlainText);
        capValues[i]->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    }
    // Modern parity: the Distribution row also carries the right-aligned
    // scope selector (Repair target / Host), mirroring the modern
    // distributionField HBox. It only re-reads cached capability evidence
    // into the table below (never runs a new helper probe).
    QHBoxLayout *distributionField = new QHBoxLayout();
    distributionField->setSpacing(6);
    distributionField->addWidget(m_capDistributionLabel, 1);
    QLabel *scopeCaption = new QLabel(tr("Show capabilities for:"), capability);
    scopeCaption->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    distributionField->addWidget(scopeCaption);
    m_capabilityScopeSelector = new QComboBox(capability);
    m_capabilityScopeSelector->insertItem(tr("Repair target"));
    m_capabilityScopeSelector->insertItem(tr("Host"));
    QToolTip::add(m_capabilityScopeSelector, tr(
        "Choose which cached scope's capabilities the table shows."));
    // The legacy capability table's native data source is the read-only host
    // PATH probe, so Host is the default scope (matching the pre-parity table
    // and the smoke test's 18-row assertion); Repair target re-reads the
    // cached target diagnostics' Repair tool lines.
    m_capabilityScopeSelector->setCurrentItem(1);
    connect(m_capabilityScopeSelector, SIGNAL(activated(int)),
            this, SLOT(refreshCapabilities()));
    distributionField->addWidget(m_capabilityScopeSelector);
    identityGrid->addWidget(new QLabel(tr("Distribution:"), capability), 0, 0);
    identityGrid->addLayout(distributionField, 0, 1);
    identityGrid->addWidget(new QLabel(tr("Package manager family:"), capability), 1, 0);
    identityGrid->addWidget(m_capPackageManagerLabel, 1, 1);
    identityGrid->addWidget(new QLabel(QString::fromLatin1("KAuth build support:"), capability), 2, 0);
    identityGrid->addWidget(m_capAuthLabel, 2, 1);

    // Qt3 has no QTableWidget; the QTable fills the same 7-column
    // Feature|Command|Scope|Status|Suggested package|Notes|Action contract.
    // The Notes column (5) stretches to the frame edge; the trailing Action
    // column stays content-sized. The table is read-only and the rows are
    // populated by refreshCapabilities().
    m_capabilityTable = new CapabilityTable(0, 7, capability);
    m_capabilityTable->setLeftMargin(0);
    // Full-row selection (modern parity): clicking any cell highlights the
    // whole row; the selected missing+installable capability enables the
    // Install Missing Support... button.  The first row is selected by
    // refreshCapabilities() after every repopulation.
    m_capabilityTable->setSelectionMode(QTable::SingleRow);
    m_capabilityTable->setReadOnly(true);
    // The table is scrollable, so it can shrink well below its full 18-row
    // content: a low minimum-content floor lets the Capabilities frame shrink
    // when the Settings splitter is dragged down, giving the Full Repair plan
    // frame room to grow past its ~188px default.
    m_capabilityTable->setMinimumContentHeight(100);
    // Cycle 13: header-click sorting on the seven columns; refreshCapabilities
    // still restores the first-row selection after every repopulation.
    m_capabilityTable->setSorting(true);
    // Modern capability-table header parity: the seven column titles are
    // translatable (tr()) so the parity extractor sees the same surface as
    // MainWindow's Boot capabilities table.
    const QString capabilityHeaders[] = {
        tr("Feature"), tr("Command"), tr("Scope"), tr("Status"),
        tr("Suggested package"), tr("Notes"), tr("Action")
    };
    for (int column = 0; column < 7; ++column) {
        m_capabilityTable->horizontalHeader()->setLabel(
            column, capabilityHeaders[column]);
    }
    m_capabilityTable->setColumnWidth(0, 150);
    m_capabilityTable->setColumnWidth(1, 150);
    m_capabilityTable->setColumnWidth(2, 90);
    m_capabilityTable->setColumnWidth(3, 100);
    m_capabilityTable->setColumnWidth(4, 150);
    // The Action column is a read-only TEXT cell (Qt3 QTable cannot host a
    // per-row cell widget, so there is no clickable per-row Install button):
    // refreshCapabilities() writes "Install" for a missing+installable row and
    // "-" otherwise; selecting such a row enables the Install Missing
    // Support... button below, which runs the guarded install-host-package
    // helper verb.
    m_capabilityTable->setColumnWidth(6, 70);
    m_capabilityTable->setColumnStretchable(5, true);
    // Item 2: the row selection drives the Install button's enabled state, so
    // the selected row's suggested package can be installed on the running host.
    connect(m_capabilityTable, SIGNAL(currentChanged(int, int)),
            this, SLOT(capabilitySelectionChanged(int, int)));
    capabilityLayout->addWidget(m_capabilityTable);

    QHBoxLayout *capabilityButtons = new QHBoxLayout(capabilityLayout);
    capabilityButtons->setSpacing(4);
    m_refreshCapabilitiesButton = makeButton(
        tr("Refresh Capabilities"), capability);
    QToolTip::add(m_refreshCapabilitiesButton, tr(
        "Re-run the read-only host capability probes (a PATH search, nothing "
        "is executed) and refresh the distribution summary."));
    connect(m_refreshCapabilitiesButton, SIGNAL(clicked()), this, SLOT(refreshCapabilities()));
    capabilityButtons->addWidget(m_refreshCapabilitiesButton);
    capabilityButtons->addStretch();
    m_installSupportButton = makeButton(
        tr("Install Missing Support..."), capability);
    m_installSupportButton->setEnabled(false);
    QToolTip::add(m_installSupportButton, tr(
        "Automatic installation will require explicit package mapping and "
        "privilege authorization; installs the selected missing capability's "
        "suggested package on the current scope (running host or committed "
        "repair target) through the guarded helper."));
    connect(m_installSupportButton, SIGNAL(clicked()),
            this, SLOT(installSelectedCapabilityPackage()));
    capabilityButtons->addWidget(m_installSupportButton);
    // The Settings splitter's handle drags up but not down because the lower
    // pane is pinned at its own minimumSizeHint (smoke evidence: the pane's
    // floor equals its current height, 444px). The pane's layout computes
    // that floor from the groups' minimumSizeHint()s (capabilities 239px +
    // safety 110px + spacing), and an explicit minimumSize() REPLACES the
    // computed floor for a layout item - verified on the upper pane, whose
    // floor is discMin 108 + the plan group's explicit minimum 75 + spacing,
    // = 189px exactly. A low explicit minimum on the capabilities group
    // therefore lowers the pane's floor and lets the handle drag down: the
    // capability table is scrollable (its own floor is the 100px
    // CapabilityTable minimumSizeHint), so the Capabilities box shrinks
    // gracefully and the Full Repair plan box grows by the same amount.
    // Default heights are unchanged - a minimum only bounds how far a drag
    // may shrink a box (same technique as the plan group's explicit 75px).
    capability->setMinimumHeight(150);
    // No stretch factor: the safety box below is Fixed at 110px, so this
    // box is the lower pane's only growable item and absorbs the pane's
    // growth and squeeze directly (its scrollable table shrinks gracefully).
    lowerLayout->addWidget(capability);

    // Mandatory safety controls (modern parity): informational and
    // de-prioritized below the capabilities table, in the same visual order as
    // the modern lower pane (table, refresh, safety).
    QGroupBox *safety = new QGroupBox(lowerPane);
    QVBoxLayout *safetyLayout = new QVBoxLayout(safety, 6, 3);
    safetyLayout->addWidget(makeSectionTitle(tr("Mandatory safety controls"), safety));
    // Modern parity (item 3): the five safety controls flow 3 left / 2 right in
    // a QGridLayout ((i % 3, i / 3)), mirroring the Full Repair plan's two-column
    // layout; the five items keep their existing order.
    QGridLayout *safetyGrid = new QGridLayout(safetyLayout, 3, 2, 4);
    safetyGrid->setColStretch(0, 1);
    safetyGrid->setColStretch(1, 1);
    const QString safetyItems[] = {
        QString::fromLatin1("Protect every physical device backing the running host root"),
        tr("Require explicit confirmation before package installation or repair actions"),
        tr("Require mapper/crypttab consistency before initramfs rebuild"),
        tr("Never log LUKS passphrases or authentication secrets"),
        QString::fromLatin1("Keep every helper runtime preflight active")
    };
    for (int i = 0; i < 5; ++i) {
        QCheckBox *check = new QCheckBox(safetyItems[i], safety);
        check->setChecked(true);
        check->setEnabled(false);
        safetyGrid->addWidget(check, i % 3, i / 3);
    }
    // The mandatory safety box stays FIXED at its natural 110px height (its
    // sizeHint equals its floor, so the pin changes no default): the lower
    // pane's growth and shrink must adjust only the capabilities box.
    safety->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,
                                       QSizePolicy::Fixed));
    lowerLayout->addWidget(safety);

    // Item 3: the legacy-only "Application configuration" group is removed
    // (modern parity has no such section; the version is available via
    // --print-config / Help -> About).
    // Pane bounds - the repair-tab pattern (explicit min+max on the
    // splitter's direct children are the only bounds the drag honors):
    //   upper pane: row 98 + spacing 6 + plan box [75..250] = [179..354]
    //   lower pane: capabilities floor 150 + spacing 6 + safety 110 = 266
    // The lower pane's minimum keeps the mandatory safety box (110px) fully
    // inside the pane on a full-down drag: without it the pane can be
    // squeezed below its content, the safety box overflows past the
    // scrollable content's bottom edge and no scrollbar position reveals it
    // (the clipped-mandatory-items symptom). The upper pane's maximum stops
    // the drag exactly at the plan box's 250px maximum, with no empty gap.
    upperPane->setMinimumHeight(179);
    upperPane->setMaximumHeight(354);
    lowerPane->setMinimumHeight(266);
    // Guarantee the total scrollable height can always contain the
    // mandatory safety box: the splitter's own minimum is the full-down
    // drag's worst case (upper pane max 354 + handle ~6 + lower pane floor
    // 266 = 626, plus a little slack), so the content can never shrink
    // past the point where the safety box would fall below the scrollable
    // bottom edge (the hidden-mandatory-section symptom on a full-down
    // drag - the total settings height must always include the mandatory
    // box's pixels).
    settingsSplitter->setMinimumHeight(630);
    // The vertical splitter keeps a sensible default split; the user can
    // drag the handle afterwards. The upper share dropped from 280 to 250
    // with the row's 108->98 shrink: the plan box's default drops a further
    // ~30px (1.5 lines, ~178->~148) - the box was 1.5-2 lines taller than
    // its two-column list; now the last plan item ("Rebuild DKMS modules")
    // sits at the bottom of the box with no extra line of empty space.
    QValueList<int> settingsSizes;
    settingsSizes.append(250);
    settingsSizes.append(530);
    settingsSplitter->setSizes(settingsSizes);
    // Intercept the splitter's handle itself: Qt 3.3.7's own opaque drag
    // path decays the panes' sum by the handle size on EVERY incremental
    // mouse move (logged in the user's real drag: 815 -> 773 over six
    // moves, the mandatory box sliding 92px below the splitter's bottom
    // edge where it is clipped and unscrollable). The handle's mouse
    // events are captured by the same filter and the drag is driven
    // through setSizes() - the distribution path that provably preserves
    // the panes' sum and keeps every box inside the content.
    {
        const QObjectList *splitterKids = settingsSplitter->children();
        if (splitterKids) {
            QObjectList splitterKidList(*splitterKids);
            for (uint ci = 0; ci < splitterKidList.count(); ++ci) {
                QObject *ko = splitterKidList.at(ci);
                if (ko && ko->isWidgetType()) {
                    QWidget *kid = static_cast<QWidget *>(ko);
                    if (!kid->isHidden() && kid != upperPane
                        && kid != lowerPane) {
                        m_settingsSplitterHandle = kid;
                    }
                }
            }
        }
        if (m_settingsSplitterHandle) {
            m_settingsSplitterHandle->installEventFilter(this);
        }
    }
    layout->addWidget(settingsSplitter, 0);
    layout->addStretch();

    registerGroupBox(discovery);
    registerGroupBox(planGroup);
    registerGroupBox(diagnostics);
    registerGroupBox(capability);
    registerGroupBox(safety);
    // Retain the Settings section frames the drag controller's event filter
    // walks (the pane and splitter parents, the capabilities and mandatory
    // boxes it resizes; see etch-agent-guide.md §6).
    m_settingsPlanGroup = planGroup;
    m_settingsCapabilityGroup = capability;
    m_settingsSafetyGroup = safety;
    return page;
}

bool LegacyMainWindow::eventFilter(QObject *watched, QEvent *event)
{
    // Settings-splitter drag control: Qt 3.3.7's own opaque drag path
    // (doMove/setGeo) decays the panes' sum by the handle size on every
    // incremental mouse move - logged in the user's real drag: the sum
    // fell 815 -> 773 over six moves while the mandatory box slid 92px
    // below the splitter's bottom edge, where it is clipped and no
    // scrollbar position can reveal it. The handle's mouse events are
    // intercepted here (the default path never runs) and the drag is
    // driven through setSizes() instead: the distribution path that
    // provably preserves the panes' sum (the smoke's extreme/jump tests)
    // and keeps every box inside the content. The new upper-pane size is
    // clamped to the pane's own explicit bounds [179..354] (= row 98 +
    // spacing 6 + plan box [75..250]).
    if (event && watched == m_settingsSplitterHandle) {
        QWidget *upperPane = m_settingsPlanGroup
            ? m_settingsPlanGroup->parentWidget() : 0;
        QSplitter *splitter = m_settingsCapabilityGroup
            ? static_cast<QSplitter *>(
                m_settingsCapabilityGroup->parentWidget()->parentWidget())
            : 0;
        if (upperPane && splitter) {
            if (event->type() == QEvent::MouseButtonPress) {
                QMouseEvent *press = static_cast<QMouseEvent *>(event);
                if (press->button() == Qt::LeftButton) {
                    m_settingsDragActive = true;
                    m_settingsDragBaseY = splitter->mapFromGlobal(
                        press->globalPos()).y();
                    m_settingsDragBaseUpper = upperPane->height();
                }
                return true;
            }
            if (event->type() == QEvent::MouseMove) {
                if (m_settingsDragActive) {
                    QMouseEvent *move = static_cast<QMouseEvent *>(event);
                    const int deltaY = splitter->mapFromGlobal(
                        move->globalPos()).y() - m_settingsDragBaseY;
                    int newUpper = m_settingsDragBaseUpper + deltaY;
                    if (newUpper < upperPane->minimumHeight()) {
                        newUpper = upperPane->minimumHeight();
                    }
                    if (newUpper > upperPane->maximumHeight()) {
                        newUpper = upperPane->maximumHeight();
                    }
                    QValueList<int> dragSizes;
                    dragSizes.append(newUpper);
                    dragSizes.append(splitter->height() - newUpper);
                    splitter->setSizes(dragSizes);
                    // Shrink the capabilities frame to the pane's new
                    // height immediately and pin the mandatory section to
                    // the pane's bottom. The logged real drag proved Qt3's
                    // posted layout events lag the drag - the frame rode
                    // at its old height while the pane shrank (lower 550
                    // -> 458), pushing the mandatory section below the
                    // pane's bottom edge where the pane clips it -
                    // invisible at any scroll position. The pane's own
                    // layout performs exactly this squeeze when its posted
                    // event eventually catches up; doing it synchronously
                    // here keeps the mandatory section visible at every
                    // drag step (the frame shrinks by the drag amount,
                    // the mandatory box stays inside).
                    if (m_settingsCapabilityGroup
                        && m_settingsSafetyGroup
                        && m_settingsCapabilityGroup->parentWidget()) {
                        const int paneHeight
                            = m_settingsCapabilityGroup->parentWidget()
                                ->height();
                        const int safetyHeight
                            = m_settingsSafetyGroup->height();
                        const int layoutSpacing = 6;
                        const int capTarget = paneHeight - safetyHeight
                            - layoutSpacing;
                        if (capTarget > 0
                            && m_settingsCapabilityGroup->height()
                                != capTarget) {
                            m_settingsCapabilityGroup->resize(
                                m_settingsCapabilityGroup->width(),
                                capTarget);
                        }
                        m_settingsSafetyGroup->move(
                            m_settingsSafetyGroup->x(),
                            m_settingsCapabilityGroup->y()
                                + m_settingsCapabilityGroup->height()
                                + layoutSpacing);
                    }
                }
                return true;
            }
            if (event->type() == QEvent::MouseButtonRelease) {
                m_settingsDragActive = false;
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void LegacyMainWindow::scanDevices()
{
    // The member slot shadows the namespace-level scanDevices(), so the
    // inventory function is called with its explicit namespace qualification.
    // The complete inventory is cached in m_rows/m_inventory; the visible list
    // is a filtered projection so a hidden row (device-discovery filter) can
    // never invalidate the selected/committed target state.
    // Cycle 14: the session unlock state survives a rescan while the
    // unlocked mapping still exists (leaving Host Maintenance must not lose
    // the already-unlocked target); it clears only when the mapping is gone.
    const QString priorUnlockedDisk = m_unlockedDisk;
    const QString priorUnlockedRoot = m_unlockedRoot;
    const QString priorUnlockedMapper = m_unlockedMapper;
    const QString priorUnlockedUuid = m_unlockedRootUuid;
    m_unlockedDisk = QString::null;
    m_unlockedRoot = QString::null;
    m_unlockedMapper = QString::null;
    m_unlockedRootUuid = QString::null;
    m_inventory = legacy::scanDevices();
    m_rows.clear();
    for (std::size_t i = 0; i < m_inventory.size(); ++i) {
        m_rows.insert(fromStd(m_inventory[i].path), m_inventory[i]);
    }
    if (!priorUnlockedDisk.isEmpty() && !priorUnlockedRoot.isEmpty()
        && QFileInfo(priorUnlockedRoot).exists()) {
        m_unlockedDisk = priorUnlockedDisk;
        m_unlockedRoot = priorUnlockedRoot;
        m_unlockedMapper = priorUnlockedMapper;
        m_unlockedRootUuid = priorUnlockedUuid;
        injectUnlockedMapperRows(m_unlockedDisk, m_unlockedMapper,
                                 m_unlockedRoot, QString::null,
                                 m_unlockedRootUuid);
    }
    rebuildDeviceList();
    updateHostCard();
    updateStatus();
    updateActionStates();
    updateDriveDetails();
    updateUnlockStatus();
}

// Rebuilds the visible device tree from the cached inventory, applying the
// Settings device-discovery filters. Whole disks are the top-level items with
// their partitions/mappers as indented children (modern tree parity); the
// selected disk is re-selected when it is still visible, and a hidden drive
// hides its children while a visible drive with a hidden child shows the
// drive only. The selection/commit state stays intact when a row is hidden.
void LegacyMainWindow::rebuildDeviceList()
{
    if (!m_deviceList) {
        return;
    }
    const QString previousDisk = previewDisk();
    m_deviceList->clear();

    // Pass 1: top-level disk items (inventory order, expanded).
    QMap<QString, QListViewItem *> diskItems;
    for (std::size_t i = 0; i < m_inventory.size(); ++i) {
        const DeviceRow &row = m_inventory[i];
        if (!row.disk || !deviceRowVisible(row)) {
            continue;
        }
        // Cycle 15: the protected running host never appears in the
        // candidate tree (the host card above shows it); Host Maintenance
        // behavior is unchanged.
        if (!runningHostDisk().isEmpty()
            && fromStd(row.path) == runningHostDisk()) {
            continue;
        }
        const QString fsLabel = diskFilesystemSummary(row);
        const QString type = row.optical ? QString::fromLatin1("optical")
                                         : QString::fromLatin1("disk");
        QListViewItem *item = new QListViewItem(m_deviceList,
                                                fromStd(row.path),
                                                fromStd(row.size), type,
                                                fsLabel);
        // Cycle 9: the tree starts collapsed (disks rolled up; expand with +).
        diskItems.insert(fromStd(row.path), item);
    }

    // Pass 2: partitions and mappers as children of their owning disk. A
    // non-disk row whose owning disk is not in the tree (floppy stubs like
    // /dev/fd0, empty-device rows like /dev/hdc, helper-reported orphans) is
    // not a repair target and is omitted from the tree; its inspection data
    // stays in the cached inventory for the details pane.
    for (std::size_t i = 0; i < m_inventory.size(); ++i) {
        const DeviceRow &row = m_inventory[i];
        if (row.disk || !deviceRowVisible(row)) {
            continue;
        }
        QString type;
        if (row.mapper) {
            type = QString::fromLatin1("mapper");
        } else {
            type = QString::fromLatin1("partition");
        }
        const QString fsLabel = displayFsType(row);
        const QString owner = owningDiskFor(row);
        QMap<QString, QListViewItem *>::const_iterator parent =
            diskItems.find(owner);
        if (parent != diskItems.end()) {
            new QListViewItem(parent.data(), fromStd(row.path),
                              fromStd(row.size), type, fsLabel);
        }
        // else: top-level = row.disk only; the orphan stays out of the tree.
    }
    if (!previousDisk.isEmpty()) {
        selectInventoryRow(previousDisk);
    }
}

// Modern devicePassesTopLevelFilters() applied to the flat legacy list: the
// filters act on the owning drive, so a hidden drive hides its partitions and
// mappers too. A row whose owning drive cannot be resolved stays visible
// (fail open for inspection only; gating is unaffected).
bool LegacyMainWindow::deviceRowVisible(const DeviceRow &row) const
{
    if (!m_showNonLinuxCheck || !m_showRemovableCheck || !m_showEncryptedCheck) {
        return true;
    }
    const QString diskPath = row.disk ? fromStd(row.path) : owningDiskFor(row);
    if (diskPath.isEmpty()) {
        return true;
    }
    const QMap<QString, DeviceRow>::const_iterator diskIt = m_rows.find(diskPath);
    if (diskIt == m_rows.end()) {
        return true;
    }
    const DeviceRow &disk = diskIt.data();
    const bool removableOrUsb = disk.transport == std::string("removable")
        || disk.transport == std::string("USB");
    if (!m_showRemovableCheck->isChecked() && removableOrUsb) {
        return false;
    }
    const bool encryptedTree = diskHasEncryptedRow(diskPath);
    if (!m_showEncryptedCheck->isChecked() && encryptedTree) {
        return false;
    }
    const bool linuxCandidate = diskHasLinuxCandidate(diskPath);
    if (!m_showNonLinuxCheck->isChecked() && !linuxCandidate
        && !(encryptedTree && m_showEncryptedCheck->isChecked())) {
        return false;
    }
    return true;
}

bool LegacyMainWindow::rowBelongsToDisk(const DeviceRow &row, const QString &diskPath) const
{
    if (row.disk) {
        return fromStd(row.path) == diskPath;
    }
    if (row.mapper) {
        return owningDiskFor(row) == diskPath;
    }
    const QMap<QString, DeviceRow>::const_iterator diskIt = m_rows.find(diskPath);
    if (diskIt == m_rows.end()) {
        return false;
    }
    return row.parent == diskIt.data().name;
}

bool LegacyMainWindow::diskHasEncryptedRow(const QString &diskPath) const
{    for (QMap<QString, DeviceRow>::const_iterator it = m_rows.begin();
         it != m_rows.end(); ++it) {
        if (it.data().encrypted && rowBelongsToDisk(it.data(), diskPath)) {
            return true;
        }
    }
    return false;
}

bool LegacyMainWindow::diskHasLinuxCandidate(const QString &diskPath) const
{
    for (QMap<QString, DeviceRow>::const_iterator it = m_rows.begin();
         it != m_rows.end(); ++it) {
        const DeviceRow &row = it.data();
        if (!rowBelongsToDisk(row, diskPath)) {
            continue;
        }
        if (legacy::isLinuxFileSystemName(row.fstype)
            || legacy::isLinuxFileSystemName(row.probedFstype)) {
            return true;
        }
    }
    return false;
}

// Filesystem label for a whole-disk row: aggregate the disk's children so the
// row is informative but never blank - the primary Linux filesystem of the
// children plus " + LUKS" when an encrypted child exists (e.g. "ext3 + LUKS",
// "ext3", "unknown" when nothing is known).
QString LegacyMainWindow::diskFilesystemSummary(const DeviceRow &row) const
{
    const QString diskPath = fromStd(row.path);
    QString linuxFs;
    bool hasLuks = false;
    for (QMap<QString, DeviceRow>::const_iterator it = m_rows.begin();
         it != m_rows.end(); ++it) {
        const DeviceRow &child = it.data();
        if (!rowBelongsToDisk(child, diskPath)) {
            continue;
        }
        if (child.encrypted) {
            hasLuks = true;
        }
        if (linuxFs.isEmpty()) {
            if (legacy::isLinuxFileSystemName(child.fstype)) {
                linuxFs = fromStd(child.fstype);
            } else if (legacy::isLinuxFileSystemName(child.probedFstype)) {
                linuxFs = fromStd(child.probedFstype);
            }
        }
    }
    if (linuxFs.isEmpty()) {
        return hasLuks ? QString::fromLatin1("LUKS")
                       : QString::fromLatin1("unknown");
    }
    if (hasLuks) {
        return linuxFs + QString::fromLatin1(" + LUKS");
    }
    return linuxFs;
}

// Modern parity: selecting a drive row inspects the drive and resolves its
// best Linux root automatically; selecting a partition/mapper row inspects
// that component while keeping its owning drive as the target candidate.
void LegacyMainWindow::deviceSelectionChanged()
{
    if (m_updatingSelection) {
        return;
    }
    // Cycle 13: any drive-row selection restores the normal per-drive pane.
    m_inspectingHostDetails = false;
    QListViewItem *item = m_deviceList->currentItem();
    if (!item) {
        return;
    }
    const QString path = item->text(0);
    QMap<QString, DeviceRow>::const_iterator it = m_rows.find(path);
    if (it == m_rows.end()) {
        return;
    }
    const DeviceRow &row = it.data();
    // Item #4: highlighting a row records the PREVIEW row only. In ordinary
    // repair-target mode the preview and the selection are the same, so the
    // selection follows; while Host Maintenance is active the active scope is
    // the protected running host and MUST NOT be changed by a highlight, so
    // only the preview is recorded (the scope stays Host Maintenance until an
    // explicit Select Target commit).
    if (row.disk) {
        m_previewDisk = path;
        m_previewComponent = QString::null;
        if (!hostScope()) {
            setSelection(path, QString::null, QString::null);
        }
    } else {
        const QString disk = owningDiskFor(row);
        m_previewDisk = disk;
        m_previewComponent = path;
        if (!hostScope()) {
            setSelection(disk, path, QString::null);
        }
    }
    updateStatus();
    updateActionStates();
    updateDriveDetails();
    updateUnlockStatus();
}

void LegacyMainWindow::setRepairTarget()
{
    // Commit-path cancel parity (item #7 adjacent, modern setPreviewTarget()):
    // committing a repair target is a scope change away from the currently
    // diagnosed scope. A read-only diagnostic in flight is cancelled (the
    // helper's cancel token + direct kill) and the commit is deferred until the
    // cancelled run unwinds in helperFinished(), so the new scope's diagnostic
    // can start immediately afterwards. A mutating repair is never cancelled
    // here (the Select Target control stays disabled while one runs).
    if (m_running && m_pendingDiagnostic) {
        m_commitPending = true;
        m_scopeSwitchPending = false;
        appendLog(tr(
            "A read-only diagnostic is running; cancelling it to commit the "
            "repair target. Mutating repairs are never cancelled."));
        m_runner->cancel();
        return;
    }
    const QString disk = previewDisk();
    // Captured before the scope may flip below: the highlighted component the
    // commit should carry forward (in Host Maintenance this is the preview
    // child, not the stale running-host selection).
    const QString commitComponent = previewComponent();
    if (disk.isEmpty()) {
        QMessageBox::warning(this, tr("Boot Bitch Legacy"),
                             tr("Select a physical drive in the Available repair targets list first."),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    if (!runningHostDisk().isEmpty() && disk == runningHostDisk()) {
        QMessageBox::warning(this, tr("Protected system"),
                             tr(
                                 "The running system cannot be selected as a repair "
                                 "target. Use Host Maintenance for the protected "
                                 "running host or choose another disk."),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    // Cycle 15 (modern commit parity): a blank or non-Linux data disk
    // commits its physical path (the resolved root falls back to the disk
    // itself) so it can be inspected; diagnostics against it fail closed
    // with the helper's exact reason. A locked LUKS container without a
    // visible Linux filesystem stays refused.
    if (!autoResolvedLuks(disk).isEmpty()
        && autoResolvedRoot(disk).isEmpty()) {
        QMessageBox::information(
            this, tr("Unlock or select a Linux system first"),
            tr(
                "This encrypted drive has no visible Linux filesystem yet. "
                "Use Unlock, refresh devices, and select the target after its "
                "Linux root is detected."),
            QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    QString root = autoResolvedRoot(previewDisk());
    if (root.isEmpty()) {
        root = disk;
    }
    // Never resolve the protected running host as a target, even through an
    // inherited component path.
    const QString hostRoot = runningHostRoot();
    if (!hostRoot.isEmpty() && root == hostRoot) {
        QMessageBox::warning(this, tr("Protected system"),
                             tr(
                                 "The selected root component (%1) belongs to the "
                                 "running system and cannot be committed as a "
                                 "repair target. Use Host Maintenance for the "
                                 "protected running host.").arg(root),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    // Committing a repair target is an authorization point: the modal
    // hidden-input dialog is requested here once and the cached session is
    // reused by every later command (never on Run All).
    if (!ensureAdministratorSession(tr("repair target commit"))) {
        return;
    }
    if (m_hostMaintenance) {
        m_hostMaintenance = false;
        if (m_hostMaintenanceButton) {
            updateButtonText(m_hostMaintenanceButton, tr("Host Maintenance"));
        }
        appendLog(tr(
            "Host Maintenance left; the committed repair target is now the active scope."));
    }
    m_targetCommitted = true;
    m_committedDisk = disk;
    m_committedRoot = root;
    // Item #4: committing switches the active scope to the committed disk. In
    // ordinary mode the selection already equals the preview; entering from
    // Host Maintenance the selection still names the running host, so update it
    // here so the scope (selectedDisk/selectedRoot/identity) follows the commit.
    m_selectedDisk = disk;
    m_selectedComponent = commitComponent;
    // Item #8: committing a repair target changes the scope identity, so the
    // cached host evidence and the diagnostic display cache are cleared before
    // the new scope's diagnostics run.
    invalidateScopeDiagnostics();
    // Item #3: the Settings capability selector follows the active scope
    // (Repair target now that a target is committed).
    syncCapabilityScopeSelector();
    appendLog(tr("Repair target committed: %1 + %2. Diagnostics and gated repairs now target this scope.")
                  .arg(m_committedDisk).arg(m_committedRoot));
    QString statusText = tr("Repair drive selected: %1")
                             .arg(m_committedDisk);
    if (m_committedRoot != m_committedDisk) {
        statusText += tr("; best detected system component: %1")
                          .arg(m_committedRoot);
    }
    statusText += tr(". No mount or repair action was performed.");
    statusBar()->message(statusText, 4000);
    updateStatus();
    updateActionStates();
    updateDriveDetails();
    updateUnlockStatus();
    // Gap #21: committing a target changes the scope identity, so the cached
    // diagnostics no longer describe the selected scope. With auto-refresh on
    // and the just-authorized session active, schedule one quiet Run All for
    // the new scope (never opens an authorization prompt by itself; the
    // skip/pending reasons are logged like the unlock/config-write paths).
    maybeAutoRefreshDiagnostics(QString::fromLatin1(
        "committing the repair target changed the diagnostics scope"));
}

// B7-7: Make Default for a GRUB-legacy BIOS host - the helper verifies
// /boot/grub/menu.lst, sets `default <N>` to the canonical installed kernel
// entry with a backup and rollback, and never writes a boot sector.
void LegacyMainWindow::makeDefault()
{
    if (m_running) {
        return;
    }
    if (!hostScope()) {
        QMessageBox::warning(this, tr("Make Default unavailable"),
                             tr(
                                 "Make Default is a running-host action on this "
                                 "frontend; enter Host Maintenance first."),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    std::string featureReason;
    if (!m_model.legacyFeatureAvailable("host-default", toStd(identity()),
                                        &featureReason)) {
        QMessageBox::warning(this, tr("Make Default unavailable"),
                             fromStd(featureReason),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    if (!administratorSessionActive()) {
        QMessageBox::warning(this, tr("Administrator authorization required"),
                             tr(
                                 "No administrator session is active; press "
                                 "Authorize first."),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    const bool answer = confirmWrapped(
        this, tr("Make Default"),
        tr(
            "Make the canonical installed kernel entry the default GRUB-legacy "
            "boot entry on the running host?\n\n"
            "The helper verifies /boot/grub/menu.lst, sets the `default <N>` "
            "directive to the canonical entry, backs the menu up first and "
            "restores it on any failure."),
        /*cancelInsteadOfNo=*/false, QString::null, QString::null,
        kHiddenInputMaximumWidth, /*noIsDefault=*/true);
    if (!answer) {
        return;
    }
    QStringList args;
    args << QString::fromLatin1("host-default")
         << selectedDisk() << selectedRoot();
    const bool started = startCommand(args, false,
                                      tr("Make Default"),
                                      false, false, false, false,
                                      QString::null,
                                      QString::fromLatin1("host-default"));
    if (started && !m_smokeMode) {
        showRepairResultDialog(tr("Make Default"));
    }
}

void LegacyMainWindow::toggleHostMaintenance()
{
    // Modern cancel-and-switch parity (item #7): selecting Host Maintenance
    // (or leaving it) while a read-only diagnostic is in flight interrupts
    // that diagnostic (the helper's cancel token + direct kill) and defers
    // the actual scope switch until the cancelled run unwinds in
    // helperFinished(), so the new scope's diagnostic can start immediately
    // afterwards. A mutating repair is never cancelled here: the Host
    // Maintenance control stays disabled while one runs (updateActionStates),
    // so only a read-only diagnostic can reach this point with m_running set.
    if (m_running && m_pendingDiagnostic) {
        m_scopeSwitchPending = true;
        m_commitPending = false;
        appendLog(tr(
            "A read-only diagnostic is running; cancelling it to switch scope. "
            "Mutating repairs are never cancelled."));
        m_runner->cancel();
        return;
    }

    if (m_hostMaintenance) {
        m_hostMaintenance = false;
        if (m_hostMaintenanceButton) {
            updateButtonText(m_hostMaintenanceButton, tr("Host Maintenance"));
        }
        appendLog(tr(
            "Running-host maintenance deselected; commit an offline repair target to run diagnostics."));
        // Item #8: leaving the running-host scope clears the host evidence and
        // the diagnostic display cache so a later target-scope run never shows
        // it.
        invalidateScopeDiagnostics();
        // Item #3: the Settings capability selector follows the active scope.
        syncCapabilityScopeSelector();
        updateStatus();
        updateActionStates();
        updateDriveDetails();
        updateUnlockStatus();
        // Modern exitHostMaintenanceMode() parity: leaving Host Maintenance
        // changes the scope identity, so the cached diagnostics no longer
        // describe the selected scope. With auto-refresh on and the session
        // active, schedule one quiet Run All for the restored scope (never
        // opens an authorization prompt by itself; the skip/pending reasons
        // are logged). No target is committed yet, so this is recorded as
        // pending until a target is committed.
        maybeAutoRefreshDiagnostics(QString::fromLatin1(
            "leaving Host Maintenance changed the diagnostics scope"));
        return;
    }

    // Host Maintenance always targets the detected running host, whatever the
    // current selection is; entering it is the authorization point for the
    // running-host session.
    std::string root;
    std::string disk;
    if (!detectRunningHostTarget(&root, &disk)) {
        QMessageBox::warning(this, tr("Host Maintenance unavailable"),
                             tr(
                                 "The running host target could not be detected; "
                                 "diagnostics need a committed repair target or a "
                                 "detected running host."),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    m_hostDetectedDisk = fromStd(disk);
    m_hostDetectedRoot = fromStd(root);
    setSelection(m_hostDetectedDisk, QString::null, m_hostDetectedRoot);
    selectInventoryRow(m_hostDetectedDisk);
    // Modern parity (item #6): the running-host scope is committed BEFORE the
    // session request, exactly like the deferred-authorization flow in
    // MainWindow::selectHostForMaintenance(). A cancelled password prompt
    // therefore leaves the scope entered and surfaces the Authorize affordance
    // (updateAuthorizationAffordance, via updateActionStates below) instead of
    // silently dropping the scope switch.
    m_targetCommitted = false;
    m_hostMaintenance = true;
    if (m_hostMaintenanceButton) {
        updateButtonText(m_hostMaintenanceButton, tr("Exit Host Maintenance"));
    }
    // Item #8: entering the running-host scope clears the target evidence and
    // the diagnostic display cache so a host-scope run never shows it.
    invalidateScopeDiagnostics();
    // Item #3: the Settings capability selector follows the active scope.
    syncCapabilityScopeSelector();
    if (!ensureAdministratorSession(tr("Host Maintenance"))) {
        appendLog(tr(
            "Administrator authorization deferred; press Authorize on the "
            "Systems tab to establish the privileged session."));
    }
    appendLog(tr(
        "Running-host maintenance activated; diagnostics and gated repairs now "
        "target the protected running host (%1 + %2).")
        .arg(selectedDisk()).arg(selectedRoot()));
    updateStatus();
    updateActionStates();
    updateDriveDetails();
    updateUnlockStatus();
    // Gap #21: entering Host Maintenance changes the scope identity, so the
    // cached diagnostics no longer describe the selected scope. With
    // auto-refresh on and the just-authorized session active, schedule one
    // quiet Run All for the running host (never opens an authorization prompt
    // by itself; the skip/pending reasons are logged).
    maybeAutoRefreshDiagnostics(QString::fromLatin1(
        "entering Host Maintenance changed the diagnostics scope"));
}

// Best Linux root component for a drive, resolved from the read-only
// inventory plus the helper-confirmed facts. Order mirrors the modern page:
// a dm/mapper device on the disk with a Linux filesystem first, then a
// partition with a Linux filesystem, then the disk itself. An explicit
// override (main.cpp --host-root or the smoke test) and the helper-confirmed
// component win over the inventory resolution.
QString LegacyMainWindow::autoResolvedRoot(const QString &disk) const
{
    if (disk.isEmpty()) {
        return QString::null;
    }
    if (m_rootOverrideDisk == disk && !m_rootOverrideRoot.isEmpty()
        && m_rootOverrideRoot.startsWith(QString::fromLatin1("/dev/"))) {
        return m_rootOverrideRoot;
    }
    if (m_helperDisk == disk && !m_helperComponent.isEmpty()
        && m_helperComponent.startsWith(QString::fromLatin1("/dev/"))) {
        return m_helperComponent;
    }
    // Cycle 11: after this session's successful unlock, the helper-confirmed
    // root component resolves even though the read-only inventory cannot see
    // the mapped LV's filesystem (the B7-1 locked-LUKS gate below would
    // otherwise keep Select Target disabled).
    if (m_unlockedDisk == disk && !m_unlockedRoot.isEmpty()
        && m_unlockedRoot.startsWith(QString::fromLatin1("/dev/"))) {
        return m_unlockedRoot;
    }
    const QString diskName = disk.startsWith(QString::fromLatin1("/dev/"))
        ? disk.mid(5) : disk;

    // Modern parity (B7-1): when the drive has a locked LUKS component (no
    // unlocked mapper child yet), the target root is not resolvable - do NOT
    // fall back to a plain partition candidate such as the unencrypted /boot,
    // which would commit an unusable component.  Select Target stays disabled
    // with the unlock-first reason until the unlock exposes the mapped Linux
    // root (after which autoResolvedLuks reports no locked candidate and the
    // mapper branch below resolves it).
    if (!autoResolvedLuks(disk).isEmpty()) {
        return QString::null;
    }

    QString partitionCandidate;
    for (QMap<QString, DeviceRow>::const_iterator it = m_rows.begin();
         it != m_rows.end(); ++it) {
        const DeviceRow &row = it.data();
        const bool linuxFs = legacy::isLinuxFileSystemName(row.fstype)
            || legacy::isLinuxFileSystemName(row.probedFstype);
        if (!linuxFs) {
            continue;
        }
        if (row.mapper && rootBelongsToDisk(m_rows, it.key(), diskName)) {
            return it.key();
        }
        if (!row.disk && !row.mapper && row.parent == diskName
            && partitionCandidate.isEmpty()) {
            partitionCandidate = it.key();
        }
    }
    if (!partitionCandidate.isEmpty()) {
        return partitionCandidate;
    }
    // Whole-disk filesystem: the disk itself is a candidate only when its
    // own filesystem is Linux root-capable - a vfat share disk or a
    // blank/data disk must never become a committable target (Select Target
    // then stays disabled with the no-Linux-filesystem reason).
    const QMap<QString, DeviceRow>::const_iterator diskRow = m_rows.find(disk);
    if (diskRow != m_rows.end()) {
        const DeviceRow &own = diskRow.data();
        if (legacy::isLinuxFileSystemName(own.fstype)
            || legacy::isLinuxFileSystemName(own.probedFstype)) {
            return disk;
        }
    }
    return QString::null;
}

// The helper's `UNLOCKED_ROOT` probe can mis-attribute the root on the Etch
// split-LV rig: its VG scoping falls back when the host LVM2 lacks the
// `lv_path` report field (LVM2 2.02.07), and the unscoped scan can then name a
// running-host LV that merely shares the `*root*` suffix (debian1-root versus
// debian-root). The GUI therefore re-resolves the root from the freshly
// rescanned inventory whenever the reported component does not actually belong
// to the unlocked disk, so a mis-attributed running-host root can never become
// the committed target. The reported root is still trusted when it belongs to
// the disk (best effort: a mapper with an unresolved backing chain stays
// compatible, exactly like the inventory's own resolution).
QString LegacyMainWindow::resolvedUnlockedRoot(const QString &disk,
                                               const QString &reportedRoot) const
{
    if (disk.isEmpty()) {
        return QString::null;
    }
    const QString diskName = disk.startsWith(QString::fromLatin1("/dev/"))
        ? disk.mid(5) : disk;
    if (!reportedRoot.isEmpty()
        && reportedRoot.startsWith(QString::fromLatin1("/dev/"))
        && rootBelongsToDisk(m_rows, reportedRoot, diskName)) {
        return reportedRoot;
    }
    QString fallback;
    for (QMap<QString, DeviceRow>::const_iterator it = m_rows.begin();
         it != m_rows.end(); ++it) {
        const DeviceRow &row = it.data();
        if (!row.mapper || !rootBelongsToDisk(m_rows, it.key(), diskName)) {
            continue;
        }
        // Prefer the LV the target names as its root; the split-LV layout has
        // exactly one *root* LV per volume group.
        if (fromStd(row.name).find(QString::fromLatin1("root"), 0, false) >= 0) {
            return it.key();
        }
        if (fallback.isEmpty()) {
            fallback = it.key();
        }
    }
    return fallback;
}

// First locked LUKS component visible on `disk` (a partition or whole-device
// crypto_LUKS row that has no unlocked mapper child yet), or "" when none is
// known. Detection uses the world-readable udev metadata; it fails closed
// (no candidate) when the probe data is unavailable.
QString LegacyMainWindow::autoResolvedLuks(const QString &disk) const
{
    if (disk.isEmpty()) {
        return QString::null;
    }
    const QString diskName = disk.startsWith(QString::fromLatin1("/dev/"))
        ? disk.mid(5) : disk;
    for (QMap<QString, DeviceRow>::const_iterator it = m_rows.begin();
         it != m_rows.end(); ++it) {
        const DeviceRow &row = it.data();
        if (!row.encrypted) {
            continue;
        }
        const bool onDisk = row.disk ? (row.name == diskName)
                                     : (row.parent == diskName);
        if (!onDisk) {
            continue;
        }
        // An encrypted container with a visible Linux mapper child is already
        // unlocked; Boot Bitch reuses that mapping and does not reopen it.
        bool unlockedChild = false;
        for (QMap<QString, DeviceRow>::const_iterator child = m_rows.begin();
             child != m_rows.end(); ++child) {
            const DeviceRow &candidate = child.data();
            if (candidate.mapper && candidate.parent == row.name
                && (legacy::isLinuxFileSystemName(candidate.fstype)
                    || legacy::isLinuxFileSystemName(candidate.probedFstype))) {
                unlockedChild = true;
                break;
            }
        }
        if (!unlockedChild) {
            return it.key();
        }
    }
    return QString::null;
}

// The component the Unlock action should open: the inspected row when it is
// itself a locked encrypted container (modern inspectedNode preference),
// otherwise the first locked LUKS component on the drive.
QString LegacyMainWindow::unlockCandidateFor(const QString &disk) const
{
    const QString inspected = previewComponent();
    if (!inspected.isEmpty()) {
        const QMap<QString, DeviceRow>::const_iterator row = m_rows.find(inspected);
        if (row != m_rows.end() && row.data().encrypted) {
            bool unlockedChild = false;
            for (QMap<QString, DeviceRow>::const_iterator child = m_rows.begin();
                 child != m_rows.end(); ++child) {
                const DeviceRow &candidate = child.data();
                if (candidate.mapper && candidate.parent == row.data().name
                    && (legacy::isLinuxFileSystemName(candidate.fstype)
                        || legacy::isLinuxFileSystemName(candidate.probedFstype))) {
                    unlockedChild = true;
                    break;
                }
            }
            if (!unlockedChild) {
                return inspected;
            }
        }
    }
    return autoResolvedLuks(disk);
}

// The physical disk that owns an inspected partition/mapper row (walking the
// resolved dm chain for mappers).
QString LegacyMainWindow::owningDiskFor(const DeviceRow &row) const
{
    if (row.disk) {
        return QString::fromLatin1("/dev/") + fromStd(row.name);
    }
    QString parent = fromStd(row.parent);
    for (int depth = 0; depth < 8 && !parent.isEmpty(); ++depth) {
        const QMap<QString, DeviceRow>::const_iterator candidate =
            m_rows.find(QString::fromLatin1("/dev/") + parent);
        if (candidate == m_rows.end()) {
            break;
        }
        if (candidate.data().disk) {
            return candidate.key();
        }
        parent = fromStd(candidate.data().parent);
    }
    return QString::null;
}

void LegacyMainWindow::autoDetectHostTarget()
{
    std::string root;
    std::string disk;
    if (detectRunningHostTarget(&root, &disk)) {
        m_hostDetectedDisk = fromStd(disk);
        m_hostDetectedRoot = fromStd(root);
    }
    if (selectionComplete()) {
        return;
    }
    if (!root.empty() && !disk.empty()) {
        setTarget(fromStd(disk), fromStd(root));
    }
}

QString LegacyMainWindow::identity() const
{
    // Modern currentTargetDiagnosticCacheIdentity() parity: diagnostics are
    // scoped to the explicitly selected physical drive, never to the transient
    // root-component spelling. After an unlock or topology refresh the
    // preferred root may legitimately change spelling (for example
    // /dev/mapper/name versus /dev/dm-N); keying the cached evidence by that
    // alias would discard valid diagnostics the moment the fact view refreshes
    // the resolved component, leaving every Settings -> Full Repair checkbox
    // stuck on "unavailable" until a later manual interaction re-evaluates a
    // now-stable identity. The host/target scope prefix still keeps the two
    // scopes apart.
    return QString(hostScope() ? QString::fromLatin1("host") : QString::fromLatin1("target"))
        + QString::fromLatin1("|") + selectedDisk();
}

bool LegacyMainWindow::hostScope() const
{
    return m_hostMaintenance;
}

QString LegacyMainWindow::selectedDisk() const
{
    return m_selectedDisk.stripWhiteSpace();
}

QString LegacyMainWindow::selectedComponent() const
{
    return m_selectedComponent.stripWhiteSpace();
}

// Item #4: the highlighted (preview) Systems tree row that Select Target,
// Unlock and the details panes act on. While Host Maintenance is active the
// preview is tracked separately from the active-scope selection, so merely
// highlighting a row never changes the scope or disables host-scope actions.
QString LegacyMainWindow::previewDisk() const
{
    return hostScope() ? m_previewDisk.stripWhiteSpace() : selectedDisk();
}

QString LegacyMainWindow::previewComponent() const
{
    return hostScope() ? m_previewComponent.stripWhiteSpace()
                       : selectedComponent();
}

QString LegacyMainWindow::selectedRoot() const
{
    return autoResolvedRoot(selectedDisk());
}

bool LegacyMainWindow::selectionComplete() const
{
    return !selectedDisk().isEmpty() && !selectedRoot().isEmpty();
}

QString LegacyMainWindow::runningHostDisk() const
{
    if (!m_hostDetectedDisk.isEmpty()) {
        return m_hostDetectedDisk;
    }
    std::string root;
    std::string disk;
    if (detectRunningHostTarget(&root, &disk)) {
        return fromStd(disk);
    }
    return QString::null;
}

QString LegacyMainWindow::runningHostRoot() const
{
    if (!m_hostDetectedRoot.isEmpty()) {
        return m_hostDetectedRoot;
    }
    std::string root;
    std::string disk;
    if (detectRunningHostTarget(&root, &disk)) {
        return fromStd(root);
    }
    return QString::null;
}

bool LegacyMainWindow::targetCommitted() const
{
    return m_targetCommitted
        && m_committedDisk == selectedDisk()
        && m_committedRoot == selectedRoot();
}

bool LegacyMainWindow::hostMaintenanceActive() const
{
    return hostScope() && m_hostMaintenance;
}

bool LegacyMainWindow::diagnosticsScopeReady() const
{
    return hostMaintenanceActive() || targetCommitted();
}

QString LegacyMainWindow::scopeFeatureKey() const
{
    return QString::fromLatin1(hostScope() ? "host-shell" : "shell");
}

QString LegacyMainWindow::scopeReadyReason() const
{
    if (hostMaintenanceActive()) {
        return tr(
            "Host Maintenance is active; diagnostics and gated repairs target "
            "the protected running host.");
    }
    if (selectedDisk().isEmpty()) {
        return tr(
            "Select a physical drive in the Available repair targets list on the "
            "Systems tab first, or use Host Maintenance for the protected "
            "running host.");
    }
    // Round 16 (c): nothing committed yet — the start-up host auto-detection
    // may have recorded the protected running host as the selected disk
    // without any user action.  That state must show the neutral
    // "no repair target is committed" reason (modern parity: nothing shows),
    // never the stale "The selected drive (...) is the protected running
    // host..." warning pulled from the auto-selected drive.
    if (!m_targetCommitted) {
        return tr(
            "No repair target is committed. Choose Select Target on the Systems "
            "tab (or Host Maintenance for the protected running host) first.");
    }
    if (!runningHostDisk().isEmpty() && selectedDisk() == runningHostDisk()) {
        return tr(
            "The selected drive (%1) is the protected running host. Choose Host "
            "Maintenance on the Systems tab to run read-only host diagnostics "
            "and guarded host repairs; ordinary target repairs stay disabled.")
            .arg(selectedDisk());
    }
    return tr(
        "The selection changed after the target was committed. Choose Select "
        "Target again.");
}

void LegacyMainWindow::runDiagnostics()
{
    runDiagnosticsInternal(false);
}

// `quiet` is used by the automatic regeneration: it never opens a dialog and
// never prompts for authorization, it only logs the skip reason.
void LegacyMainWindow::runDiagnosticsInternal(bool quiet)
{
    if (!diagnosticsScopeReady()) {
        if (quiet) {
            appendLog(tr(
                "Automatic read-only diagnostics regeneration skipped: %1")
                .arg(scopeReadyReason()));
        } else {
            QMessageBox::information(this, tr("Diagnostics scope required"),
                                     scopeReadyReason(),
                                     QMessageBox::Ok, QMessageBox::NoButton);
        }
        return;
    }
    if (!selectionComplete()) {
        if (quiet) {
            appendLog(tr(
                "Automatic read-only diagnostics regeneration skipped: the "
                "running host target could not be resolved."));
        } else {
            QMessageBox::warning(
                this, tr("Diagnostics scope unresolved"),
                tr(
                    "The running host target could not be resolved; use Refresh "
                    "Devices and commit a repair target or re-enter Host "
                    "Maintenance."),
                QMessageBox::Ok, QMessageBox::NoButton);
        }
        return;
    }
    const bool host = hostScope();
    QStringList args;
    args << (host ? QString::fromLatin1("host-diagnose") : QString::fromLatin1("diagnose"))
         << selectedDisk() << selectedRoot() << QString::fromLatin1("all");
    // The combined run has no fixed section: the stream's `Diagnostic: <key>`
    // markers tag each section's lines.
    startCommand(args, true,
                 host ? QString::fromLatin1("host-diagnose all")
                      : QString::fromLatin1("diagnose all"),
                 false, false, false, quiet, QString::null, QString::null);
}

void LegacyMainWindow::runSelectedDiagnostic()
{
    if (m_running) {
        return;
    }
    if (!diagnosticsScopeReady()) {
        QMessageBox::information(this, tr("Diagnostics scope required"),
                                 scopeReadyReason(),
                                 QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    if (!selectionComplete()) {
        QMessageBox::warning(
            this, tr("Diagnostics scope unresolved"),
            tr(
                "The running host target could not be resolved; use Refresh "
                "Devices and commit a repair target or re-enter Host "
                "Maintenance."),
            QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    QListViewItem *item = m_diagnosticList ? m_diagnosticList->currentItem() : 0;
    if (!item) {
        QMessageBox::information(this, tr("Diagnostic check required"),
                                 tr(
                                     "Select a diagnostic check in the list first."),
                                 QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    const QString key = mapValue(m_diagKeyByTitle,
                                 item->text(0).stripWhiteSpace());
    if (key.isEmpty()) {
        return;
    }
    const bool host = hostScope();
    QStringList args;
    args << (host ? QString::fromLatin1("host-diagnose") : QString::fromLatin1("diagnose"))
         << selectedDisk() << selectedRoot() << key;
    startCommand(args, true,
                 (host ? QString::fromLatin1("host-diagnose ") : QString::fromLatin1("diagnose "))
                     + key,
                 false, false, false, false, key, QString::null);
}

void LegacyMainWindow::diagnosticSelectionChanged()
{
    updateDiagnosticDetails();
    refreshDiagnosticResultsView();
    updateActionStates();
}

// Modern parity: the target configuration row is target-only. Host
// Maintenance never shows it and never enables target-file editing, and the
// helper refuses to read or write a running-host configuration path.
void LegacyMainWindow::editTargetConfigFile()
{
    if (m_running) {
        return;
    }
    if (!targetCommitted() || hostMaintenanceActive()) {
        QMessageBox::information(
            this, tr("Repair target required"),
            targetCommitted() ? scopeReadyReason()
                              : tr(
                                    "Target file editing needs a committed repair "
                                    "target. Running-host maintenance has no "
                                    "target-file editing; commit an offline target "
                                    "first."),
            QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    const QString path = m_configCombo ? m_configCombo->currentText().stripWhiteSpace()
                                       : QString::null;
    const QString key = configKeyForPath(path);
    if (key.isEmpty()) {
        QMessageBox::information(this, tr("Configuration file required"),
                                 tr(
                                     "Select a target configuration file first."),
                                 QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    m_pendingConfigKey = key;
    m_pendingConfigPath = path;
    QStringList args;
    args << QString::fromLatin1("config-read") << selectedDisk() << selectedRoot() << key;
    startCommand(args, false, QString::fromLatin1("config-read %1").arg(path), false, true);
}

// Opens the modal editor with the helper's read-only `config-read` content.
// Saving writes through the guarded `config-write` verb and invalidates the
// cached diagnostics, exactly like the modern Edit Target File dialog.
void LegacyMainWindow::openConfigEditor(const QString &content, const QString &key,
                                        const QString &path)
{
    if (key.isEmpty()) {
        return;
    }
    QDialog dialog(this, "legacy-config-editor", true);
    dialog.setCaption(tr("Edit target %1").arg(path));
    QVBoxLayout *layout = new QVBoxLayout(&dialog, 10, 8);
    QLabel *info = new QLabel(
        tr(
            "Edit this target file through the guarded administrator helper. A "
            "successful save invalidates cached diagnostics; rerun diagnostics "
            "before repair. Generated files such as /boot/grub/menu.lst may be "
            "replaced by the next bootloader update."),
        &dialog);
    enableLabelWordWrap(info);
    info->setMaximumWidth(640);
    layout->addWidget(info);

    QTextEdit *editor = new QTextEdit(&dialog);
    editor->setTextFormat(Qt::PlainText);
    editor->setText(content);
    editor->setWordWrap(QTextEdit::NoWrap);
    QFont mono(QString::fromLatin1("monospace"));
    mono.setStyleHint(QFont::TypeWriter);
    editor->setFont(mono);
    layout->addWidget(editor, 1);

    QHBoxLayout *buttons = new QHBoxLayout(layout);
    buttons->setSpacing(6);
    buttons->addStretch();
    QPushButton *cancel = new QPushButton(tr("Cancel"), &dialog);
    QPushButton *save = new QPushButton(tr("Save Target File"), &dialog);
    save->setDefault(true);
    // Round 72 (modern parity, TUXEDO button order): the positive action
    // sits LEFT of Cancel, and the Save button is the default AND owns the
    // focus so Enter saves (the modern addUniformDialogButtons
    // [accept][Cancel] convention with the accept default).  The focus is
    // re-asserted after the dialog's show-time focus pass has settled (the
    // round-27 trap — a 0-timer can fire before the posted show-focus
    // events).
    buttons->addWidget(save);
    buttons->addWidget(cancel);
    save->setFocus();
    QTimer::singleShot(100, save, SLOT(setFocus()));
    QObject::connect(cancel, SIGNAL(clicked()), &dialog, SLOT(reject()));
    QObject::connect(save, SIGNAL(clicked()), &dialog, SLOT(accept()));
    dialog.resize(760, 520);

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }
    const QString edited = editor->text();
    if (edited == content) {
        statusBar()->message(tr("No changes to %1.").arg(path), 3000);
        return;
    }
    // B5: the write transport is a private content file, never an argv
    // element, so the edited text can never surface in /proc/<pid>/cmdline,
    // helper logs or the session log. NUL stays rejected (the file transport
    // would carry it, so it is refused here explicitly), and the size cap is
    // the helper's 1 MiB file bound (raised from the old argv bound).
    if (edited.contains(QChar(0x0000))) {
        QMessageBox::warning(
            this, tr("Configuration write refused"),
            tr(
                "The edited content contains NUL bytes; the guarded write "
                "refuses it."),
            QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    const QByteArray contentBytes = edited.local8Bit();
    if (static_cast<int>(contentBytes.size()) > kConfigEditMaximumBytes) {
        QMessageBox::warning(
            this, tr("File too large"),
            tr(
                "The edited file is larger than 1 MiB. The guarded write "
                "refuses it; edit the file from a console instead."),
            QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    const bool answer = confirmWrapped(
        this, tr("Write target configuration"),
        tr(
            "Write the edited contents to %1? This modifies the repair target "
            "and invalidates cached diagnostics.").arg(path),
        /*cancelInsteadOfNo=*/true, QString::null, QString::null,
        kHiddenInputMaximumWidth, /*noIsDefault=*/true);
    if (!answer) {
        return;
    }

    // The edited content travels through the private mode-600 content file
    // (the helper reads it, proves the ownership and never deletes it; the
    // GUI unlinks it on every completion path, including the signal
    // handlers).
    QString contentFilePath;
    if (!writeConfigContentFile(contentBytes, &contentFilePath)) {
        return;
    }
    m_configContentFilePath = contentFilePath;
    m_pendingConfigKey = key;
    m_pendingConfigPath = path;
    m_pendingConfigWrite = true;
    QStringList args;
    args << QString::fromLatin1("config-write") << selectedDisk() << selectedRoot()
         << key
         << QString::fromLatin1("--content-file") << contentFilePath
         << QString::fromLatin1("--content-owner")
         << QString::number(static_cast<unsigned long>(::getuid()));
    if (!startCommand(args, false, QString::fromLatin1("config-write %1").arg(path),
                      false, true)) {
        // startCommand failed before the runner started (no authorization
        // session, smoke elevation, ...); the secret file must not outlive
        // the refused attempt.
        discardConfigContentFile();
    }
}

void LegacyMainWindow::copyResults()
{
    if (!m_rawView) {
        return;
    }
    const QString text = m_rawView->text();
    if (text.stripWhiteSpace().isEmpty()) {
        statusBar()->message(tr("No diagnostic results to copy yet."), 3000);
        return;
    }
    QApplication::clipboard()->setText(text);
    statusBar()->message(tr("Diagnostic results copied to the clipboard."), 3000);
}

// Modern parity: Save Results... writes the current read-only results pane to
// a user-chosen file (no helper interaction, no secret can be present).
void LegacyMainWindow::saveResults()
{
    if (!m_rawView) {
        return;
    }
    const QString text = m_rawView->text();
    if (text.stripWhiteSpace().isEmpty()) {
        statusBar()->message(tr("No diagnostic results to save yet."), 3000);
        return;
    }
    const QString path = QFileDialog::getSaveFileName(
        QString::null,
        tr("Text files (*.txt);;All files (*)"),
        this, "save-results", tr("Save diagnostic results"));
    if (path.isEmpty()) {
        return;
    }
    QFile file(path);
    if (!file.open(IO_WriteOnly)) {
        QMessageBox::warning(this, tr("Boot Bitch Legacy"),
                             tr("Could not write %1.").arg(path),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    QTextStream stream(&file);
    stream << text;
    if (!text.endsWith(QString::fromLatin1("\n"))) {
        stream << "\n";
    }
    file.close();
    statusBar()->message(tr("Diagnostic results saved to %1").arg(path), 4000);
}

QDialog *LegacyMainWindow::buildUnlockDialog(const QString &luks,
                                            QLineEdit **editOut)
{
    QDialog *dialog = new QDialog(this, "legacy-unlock-passphrase", true);
    dialog->setCaption(tr("Unlock LUKS repair target"));
    QVBoxLayout *layout = new QVBoxLayout(dialog, 10, 8);

    QHBoxLayout *targetRow = new QHBoxLayout(layout);
    targetRow->setSpacing(4);
    QLabel *targetCaption = new QLabel(tr("LUKS volume:"), dialog);
    targetRow->addWidget(targetCaption, 0, Qt::AlignVCenter);
    QLabel *targetPath = new QLabel(luks, dialog);
    QFont pathFont = targetPath->font();
    pathFont.setBold(true);
    targetPath->setFont(pathFont);
    targetRow->addWidget(targetPath, 1, Qt::AlignVCenter);

    QLabel *instruction = new QLabel(
        tr("Enter the passphrase to unlock this volume."), dialog);
    enableLabelWordWrap(instruction);
    instruction->setMaximumWidth(kHiddenInputMaximumWidth);
    layout->addWidget(instruction);

    QLineEdit *edit = new QLineEdit(dialog);
    edit->setEchoMode(QLineEdit::Password);
    edit->setText(QString::null);
    layout->addWidget(edit);

    QLabel *note = new QLabel(tr(
        "Administrator authorization is already active. The passphrase is sent "
        "only to cryptsetup over the privileged helper pipe and is never "
        "logged or placed on a command line."), dialog);
    enableLabelWordWrap(note);
    note->setMaximumWidth(kHiddenInputMaximumWidth);
    QFont noteFont = note->font();
    noteFont.setPointSizeFloat(noteFont.pointSizeFloat() * 0.92f);
    note->setFont(noteFont);
    layout->addWidget(note);

    QHBoxLayout *buttons = new QHBoxLayout(layout);
    buttons->setSpacing(6);
    buttons->addStretch();
    QPushButton *cancel = new QPushButton(tr("Cancel"), dialog);
    QPushButton *unlockButton = new QPushButton(tr("Unlock"), dialog);
    unlockButton->setDefault(true);
    // Round 72 (modern parity, TUXEDO button order): Unlock sits LEFT of
    // Cancel with the Unlock default (the modern passphrase dialog's
    // addUniformDialogButtons [Unlock][Cancel] convention).
    buttons->addWidget(unlockButton);
    buttons->addWidget(cancel);
    QObject::connect(cancel, SIGNAL(clicked()), dialog, SLOT(reject()));
    QObject::connect(unlockButton, SIGNAL(clicked()), dialog, SLOT(accept()));
    QObject::connect(edit, SIGNAL(returnPressed()), dialog, SLOT(accept()));
    // Widen the popup substantially over the earlier 420px minimum. Qt3 sizes
    // a fresh modal dialog to its (narrow) content sizeHint and does NOT honor
    // setMinimumWidth as a lower bound on the displayed width (the smoke
    // measurement reads minimumWidth() back as 0, while setMaximumWidth
    // persists). So pin the width explicitly with resize(); the label wrapping
    // width stays on kHiddenInputMaximumWidth.
    dialog->setMaximumWidth(800);
    dialog->resize(420, dialog->sizeHint().height());

    if (editOut) {
        *editOut = edit;
    }
    return dialog;
}

void LegacyMainWindow::runUnlock()
{
    if (m_running) {
        return;
    }
    // Cycle 13 loop: Host Maintenance is a scope, not a lock - unlocking is
    // refused only when the selected drive IS the protected running host.
    if (!runningHostDisk().isEmpty()
        && previewDisk() == runningHostDisk()) {
        QMessageBox::warning(this, tr("Unlock not available"),
                             tr(
                                 "The protected running host cannot be unlocked. "
                                 "Select an offline repair target to unlock."),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    if (previewDisk().isEmpty()) {
        QMessageBox::warning(this, tr("Boot Bitch Legacy"),
                             tr("Select a physical drive in the Available repair targets list first."),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    // Auto-resolve the locked LUKS component from the read-only inventory; the
    // helper's own cryptsetup preflights verify it again before opening it.
    const QString luks = unlockCandidateFor(previewDisk());
    if (luks.isEmpty()) {
        QMessageBox::warning(this, tr("Unlock not available"),
                             tr(
                                 "No locked LUKS component is currently visible on "
                                 "this selected drive."),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }

    // Modern parity: the user already pressed Unlock for the selected drive,
    // so proceed straight to the cryptsetup passphrase prompt without an extra
    // confirmation dialog. The explanation now lives as the note inside the
    // passphrase dialog ("sent only to cryptsetup ... never logged"); the
    // already-granted administrator authorization carries forward through
    // ensureAdministratorSession below (no second prompt).

    // Establish the elevation session before collecting the disk passphrase,
    // so the two secrets are never held in memory at the same time. The cached
    // session from the target commit is reused; only an expired session
    // prompts here.
    if (!ensureAdministratorSession(tr("LUKS unlock"))) {
        return;
    }

    // Modern parity: the unlock passphrase dialog is structured like the
    // modern one instead of the generic single-label hidden-input prompt — a
    // "LUKS volume: <path>" line (bold path), the "Enter the passphrase to
    // unlock this volume." instruction, the hidden passphrase entry and the
    // smaller "Administrator authorization is already active..." note. The
    // passphrase delivery (a mode-600 --key-file argument, never stdin/argv)
    // is unchanged.
    QLineEdit *edit = 0;
    QDialog *dialog = buildUnlockDialog(luks, &edit);
    const int result = dialog->exec();
    QString passphrase = (result == QDialog::Accepted && edit)
        ? edit->text() : QString::null;
    delete dialog;
    if (result != QDialog::Accepted) {
        return;
    }
    if (passphrase.isEmpty()) {
        QMessageBox::warning(this, tr("Passphrase required"),
                             tr(
                                 "An empty passphrase was not submitted. Enter "
                                 "the LUKS passphrase or choose Cancel."),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }

    QByteArray secret = passphrase.local8Bit();
    passphrase.fill(QChar(0));
    passphrase = QString::null;

    // Qt 3.3.7's QProcess cannot deliver stdin reliably, so the passphrase
    // travels through a mode-600 keyfile argument (O_EXCL, in the GUI's log
    // directory, never in argv) that the helper reads, verifies and never
    // deletes - the GUI unlinks it on every completion path. The buffer is
    // wiped right after the write.
    QString keyfilePath;
    if (!writeUnlockKeyfile(secret, &keyfilePath)) {
        secret.fill('\0');
        secret = QByteArray();
        return;
    }
    secret.fill('\0');
    secret = QByteArray();

    m_unlockRetry = false;
    m_unlockDevice = luks;
    m_unlockDisk = previewDisk();
    QStringList args;
    // Cycle 10: the GUI passes its own uid with the keyfile so the elevated
    // helper can accept the file under gksu/gksudo (no SUDO_UID) too.
    args << QString::fromLatin1("unlock") << previewDisk() << luks
         << QString::fromLatin1("--key-file") << keyfilePath
         << QString::fromLatin1("--key-owner")
         << QString::number(static_cast<unsigned long>(::getuid()));
    m_unlockKeyfilePath = keyfilePath;
    startCommand(args, false, QString::fromLatin1("unlock %1").arg(luks), true);
}

// F05 pattern (UI-1): open the directory at `path` through a symlink-refusing
// fd, verify it is a real directory owned by the invoking user, and
// re-tighten it to 0700 through fchmod. mkdir(0700) is attempted first (EEXIST
// is fine); a symlink — even one pointing at a directory — fails the
// O_NOFOLLOW open, so a hostile home can never make the GUI chmod a foreign
// directory. A root-run GUI pointed at a non-root-owned directory refuses,
// which is intended: the private dirs belong to the invoking user.
static bool ensurePrivateDirectory0700(const QString &path)
{
    if (path.isEmpty()) {
        return false;
    }
    if (::mkdir(path.local8Bit().data(), 0700) != 0 && errno != EEXIST) {
        return false;
    }
    const int fd = ::open(path.local8Bit().data(),
                          O_RDONLY | O_DIRECTORY | O_NOFOLLOW);
    if (fd < 0) {
        return false;
    }
    struct stat info;
    const bool ok = ::fstat(fd, &info) == 0
        && S_ISDIR(info.st_mode)
        && info.st_uid == ::geteuid()
        && ::fchmod(fd, 0700) == 0;
    ::close(fd);
    return ok;
}

// A non-predictable name component for the menu-answer file (UI-1): 8 bytes
// from /dev/urandom, hex-encoded, with a time/pid/counter fallback for an
// environment without /dev/urandom. The O_EXCL creation still fails closed on
// any collision, so the fallback never weakens the reservation.
static QString randomAnswerFileComponent()
{
    const int fd = ::open("/dev/urandom", O_RDONLY);
    if (fd >= 0) {
        unsigned char bytes[8];
        std::size_t got = 0;
        while (got < sizeof(bytes)) {
            const ssize_t n = ::read(fd, bytes + got, sizeof(bytes) - got);
            if (n <= 0) {
                break;
            }
            got += static_cast<std::size_t>(n);
        }
        ::close(fd);
        if (got == sizeof(bytes)) {
            static const char hex[] = "0123456789abcdef";
            char out[2 * sizeof(bytes) + 1];
            for (std::size_t i = 0; i < sizeof(bytes); ++i) {
                out[i * 2] = hex[bytes[i] >> 4];
                out[i * 2 + 1] = hex[bytes[i] & 0x0f];
            }
            out[2 * sizeof(bytes)] = '\0';
            return QString::fromLatin1(out, 2 * static_cast<int>(sizeof(bytes)));
        }
    }
    static unsigned long fallbackCounter = 0;
    return QString::number(static_cast<unsigned long>(
               QDateTime::currentDateTime().toTime_t()))
        + QString::fromLatin1("-")
        + QString::number(static_cast<unsigned long>(::getpid()))
        + QString::fromLatin1("-")
        + QString::number(++fallbackCounter);
}

// Shared secret-file writer (B5, generalized from the unlock keyfile path):
// write `data` to a mode-600, O_EXCL-created file named .<prefix><pid> in the
// dedicated 0700 `.keys` subdirectory of the GUI's log directory (never argv,
// never the session log). The subdirectory is created 0700 and re-tightened
// on every attempt, so a shared --log-dir cannot expose the file to other
// users. The created path is registered in the caller's termination-handler
// buffer (registeredPath) immediately, so a SIGTERM/SIGINT/SIGHUP always
// unlinks it before the GUI exits, even mid-write. Fails closed: an existing
// file, an unwritable directory or a partial write leaves nothing behind.
bool LegacyMainWindow::writeSecretFile(const QByteArray &data, const char *prefix,
                                       char *registeredPath,
                                       std::size_t registeredPathSize,
                                       QString *path)
{
    if (path) {
        *path = QString::null;
    }
    ensureLogDirectory();
    const QString keysDirectory = m_logDirectory + QString::fromLatin1("/.keys");
    if (!ensurePrivateDirectory0700(keysDirectory)) {
        return false;
    }
    QString candidate = keysDirectory + QString::fromLatin1("/.")
        + QString::fromLatin1(prefix)
        + QString::number(static_cast<unsigned long>(::getpid()));
    const QByteArray candidateBytes = candidate.local8Bit();
    const int fd = ::open(candidateBytes.data(),
                          O_WRONLY | O_CREAT | O_EXCL, 0600);
    if (fd < 0) {
        return false;
    }
    // Register the created file with the termination handlers immediately
    // so a SIGTERM/SIGINT/SIGHUP always unlinks it before the GUI exits,
    // even mid-write.
    if (registeredPath && registeredPathSize > 0) {
        ::strncpy(registeredPath, candidateBytes.data(), registeredPathSize - 1);
        registeredPath[registeredPathSize - 1] = '\0';
    }
    const unsigned char *bytes =
        reinterpret_cast<const unsigned char *>(data.data());
    std::size_t remaining = static_cast<std::size_t>(data.size());
    while (remaining > 0) {
        const ssize_t written = ::write(fd, bytes, remaining);
        if (written <= 0) {
            ::close(fd);
            ::unlink(candidateBytes.data());
            if (registeredPath && registeredPathSize > 0) {
                registeredPath[0] = '\0';
            }
            return false;
        }
        bytes += written;
        remaining -= static_cast<std::size_t>(written);
    }
    ::close(fd);
    if (path) {
        *path = candidate;
    }
    return true;
}

// The unlock passphrase secret file: the dedicated .unlock-key-<pid> name in
// the same private .keys directory. Every failure path leaves nothing behind
// and the unlock is not started.
bool LegacyMainWindow::writeUnlockKeyfile(const QByteArray &secret, QString *path)
{
    if (writeSecretFile(secret, "unlock-key-",
                        gUnlockKeyfilePath, sizeof(gUnlockKeyfilePath), path)) {
        return true;
    }
    QMessageBox::warning(
        this, tr("Unlock keyfile unavailable"),
        tr(
            "The LUKS passphrase could not be written to a private keyfile in "
            "%1; the unlock was not started.")
            .arg(m_logDirectory),
        QMessageBox::Ok, QMessageBox::NoButton);
    return false;
}

// The config-write content file: the dedicated .config-content-<pid> name in
// the same private .keys directory. The helper never deletes the file; the
// GUI unlinks it on every completion path.
bool LegacyMainWindow::writeConfigContentFile(const QByteArray &content, QString *path)
{
    if (writeSecretFile(content, "config-content-",
                        gConfigContentFilePath, sizeof(gConfigContentFilePath),
                        path)) {
        return true;
    }
    QMessageBox::warning(
        this, tr("Configuration write unavailable"),
        tr(
            "The edited content could not be written to a private temporary "
            "file in %1; the write was not started.")
            .arg(m_logDirectory),
        QMessageBox::Ok, QMessageBox::NoButton);
    return false;
}

// Delete the unlock keyfile (best-effort) and clear the tracked path. Called
// from every completion and failure path, so the passphrase file never
// outlives the attempt.
void LegacyMainWindow::discardUnlockKeyfile()
{
    if (!m_unlockKeyfilePath.isEmpty()) {
        ::unlink(m_unlockKeyfilePath.local8Bit().data());
        m_unlockKeyfilePath = QString::null;
    }
    gUnlockKeyfilePath[0] = '\0';
}

// Delete the pending config-write content file (best-effort) and clear the
// tracked path. Called from every completion and failure path (the helper
// never deletes the caller's file), so the edited content file never
// outlives the attempt; the unlink is idempotent.
void LegacyMainWindow::discardConfigContentFile()
{
    if (!m_configContentFilePath.isEmpty()) {
        ::unlink(m_configContentFilePath.local8Bit().data());
        m_configContentFilePath = QString::null;
    }
    gConfigContentFilePath[0] = '\0';
}

// Reserve the mode-600 menu-answer file for the running interactive repair and
// return its path.  The helper polls this file (BOOT_REPAIR_LEGACY_MENU_ANSWER_FILE)
// for each PROMPT record and truncates it after every read, so the two-level
// kernel menu can reuse the one path; the GUI writes the raw answer (plus a
// newline) on Send.  Created empty in the GUI's private 0700 `.keys` directory
// (the unlock keyfile pattern) so the helper's poll loop has a stable path from
// the first prompt.
bool LegacyMainWindow::ensureMenuAnswerFile(QString *path)
{
    if (path) {
        *path = QString::null;
    }
    ensureLogDirectory();
    const QString keysDirectory = m_logDirectory + QString::fromLatin1("/.keys");
    if (!ensurePrivateDirectory0700(keysDirectory)) {
        return false;
    }
    const QString candidate = keysDirectory + QString::fromLatin1("/.menu-answer-")
        + randomAnswerFileComponent();
    const int fd = ::open(candidate.local8Bit().data(),
                          O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW, 0600);
    if (fd < 0) {
        return false;
    }
    ::close(fd);
    if (path) {
        *path = candidate;
    }
    return true;
}

// Writes the RAW answer plus a single newline to the menu-answer file (no
// `ANSWER\t<id>\t<base64>` framing, no base64).  An empty answer (cancel)
// writes just the newline, which makes the helper fail closed.  The record is
// a single small buffer; a short write unlinks the file so a partial record
// can never be mistaken for a complete answer.
bool LegacyMainWindow::writeMenuAnswerRecord(const QString &answer)
{
    if (m_promptAnswerFile.isEmpty()) {
        return false;
    }
    const QString record = answer + QString::fromLatin1("\n");
    const QCString bytes = record.latin1();
    // O_NOFOLLOW refuses a replaced symlink; the fstat confirms the opened
    // file is still the caller-owned regular file the GUI reserved (the
    // truncate-after-read reuse contract stays: the helper truncates, never
    // deletes, so the same path is rewritten for every answer).
    const int fd = ::open(m_promptAnswerFile.local8Bit().data(),
                          O_WRONLY | O_TRUNC | O_NOFOLLOW, 0600);
    if (fd < 0) {
        return false;
    }
    struct stat info;
    if (::fstat(fd, &info) != 0 || !S_ISREG(info.st_mode)
        || info.st_uid != ::geteuid()) {
        ::close(fd);
        return false;
    }
    const char *data = bytes.data();
    std::size_t remaining = static_cast<std::size_t>(bytes.size());
    bool ok = true;
    while (remaining > 0) {
        const ssize_t written = ::write(fd, data, remaining);
        if (written <= 0) {
            ok = false;
            break;
        }
        data += written;
        remaining -= static_cast<std::size_t>(written);
    }
    ::close(fd);
    if (!ok) {
        ::unlink(m_promptAnswerFile.local8Bit().data());
    }
    return ok;
}

// Delete the menu-answer file (best-effort) and clear the tracked path.  The
// helper truncates but never deletes the file; this unlink runs on every
// completion path so the answer channel never outlives the command.
void LegacyMainWindow::discardMenuAnswerFile()
{
    if (!m_promptAnswerFile.isEmpty()) {
        ::unlink(m_promptAnswerFile.local8Bit().data());
        m_promptAnswerFile = QString::null;
    }
}


void LegacyMainWindow::logFilterChanged()
{
    refreshLogView();
}

void LegacyMainWindow::logSearchChanged()
{
    refreshLogView();
}

void LegacyMainWindow::sessionLogSelectionChanged(QListViewItem *item)
{
    if (!m_sessionLogList) {
        return;
    }
    if (!item) {
        item = m_sessionLogList->selectedItem();
    }
    if (!item) {
        item = m_sessionLogList->currentItem();
    }
    if (!item) {
        return;
    }
    if (item == m_sessionLogList->firstChild()) {
        m_viewingPriorLog = false;
        m_priorLogPath = QString::null;
        m_priorLogLines.clear();
        if (m_priorLogBanner) {
            m_priorLogBanner->hide();
        }
        if (m_deleteSessionLogButton) {
            m_deleteSessionLogButton->setEnabled(false);
        }
        refreshLogView();
        return;
    }
    const QString path = m_logDirectory + QString::fromLatin1("/") + item->text(0);
    QFile file(path);
    if (!file.open(IO_ReadOnly)) {
        // Round 50 (Etch log deletion): a prior log the GUI cannot read (for
        // example a root-owned mode-600 file from an elevated smoke session)
        // must still be SELECTABLE for deletion — the delete goes through the
        // privileged helper, which removes the file regardless of its owning
        // user.  The prior-log state is recorded (with an empty line buffer,
        // so the view falls back to the read-error line) and the Delete button
        // enables; the old early return left the button disabled and made the
        // file undeletable from the GUI.
        m_logView->append(tr("ERROR: cannot read the session log %1").arg(path));
        m_priorLogPath = path;
        m_priorLogLines.clear();
        m_viewingPriorLog = true;
        if (m_priorLogBanner) {
            m_priorLogBanner->setText(tr(
                "Viewing a prior session log (read-only): %1").arg(item->text(0)));
            m_priorLogBanner->show();
        }
        if (m_deleteSessionLogButton) {
            m_deleteSessionLogButton->setEnabled(true);
        }
        refreshLogView();
        return;
    }
    const QByteArray fileData = file.readAll();
    file.close();
    const QString text = QString::fromLocal8Bit(fileData.data(), fileData.size());
    m_priorLogLines = QStringList::split(QChar('\n'), text, false);
    while (!m_priorLogLines.isEmpty() && m_priorLogLines.last().isEmpty()) {
        m_priorLogLines.pop_back();
    }
    m_priorLogPath = path;
    m_viewingPriorLog = true;
    if (m_priorLogBanner) {
        m_priorLogBanner->setText(tr(
            "Viewing a prior session log (read-only): %1").arg(item->text(0)));
        m_priorLogBanner->show();
    }
    if (m_deleteSessionLogButton) {
        m_deleteSessionLogButton->setEnabled(true);
    }
    refreshLogView();
}

void LegacyMainWindow::refreshSessionLogs()
{
    refreshSessionLogList();
    statusBar()->message(tr("Session log list refreshed."), 3000);
}

void LegacyMainWindow::toggleLogWrap(bool enabled)
{
    m_logWrapEnabled = enabled;
    if (m_logView) {
        m_logView->setWordWrap(enabled ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);
        m_logView->setHScrollBarMode(enabled ? QScrollView::AlwaysOff
                                             : QScrollView::Auto);
    }
    if (m_resultView) {
        m_resultView->setWordWrap(enabled ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);
    }
    // Keep the View menu item in sync without re-entering its signal. The
    // Settings checkbox is gone (item 1); the null-guarded m_logWrapCheck
    // block remains only so the member/state plumbing stays intact.
    if (m_logWrapCheck && m_logWrapCheck->isChecked() != enabled) {
        m_logWrapCheck->blockSignals(true);
        m_logWrapCheck->setChecked(enabled);
        m_logWrapCheck->blockSignals(false);
    }
    if (m_viewMenu && m_wrapLogsMenuValid
        && m_viewMenu->isItemChecked(m_wrapLogsMenuId) != enabled) {
        m_viewMenu->setItemChecked(m_wrapLogsMenuId, enabled);
    }
    saveLegacySettings();
}

// Modern New Session Log: close the active session file. It becomes a prior
// session in the list; the next log entry creates a new timestamped file (the
// legacy logger has no separate scope-identification step).
void LegacyMainWindow::startNewSessionLog()
{
    const QString closed = m_logPath;
    m_logPath = QString::null;
    m_logEntries.clear();
    m_viewingPriorLog = false;
    m_priorLogPath = QString::null;
    m_priorLogLines.clear();
    if (m_priorLogBanner) {
        m_priorLogBanner->hide();
    }
    appendLog(tr(
        "Started a new session log; earlier files remain available in the "
        "session list."));
    if (!closed.isEmpty()) {
        appendLog(tr("Closed session log: %1").arg(closed));
    }
    refreshSessionLogList();
    if (m_sessionLogList && m_sessionLogList->firstChild()) {
        m_sessionLogList->setSelected(m_sessionLogList->firstChild(), true);
        m_sessionLogList->setCurrentItem(m_sessionLogList->firstChild());
    }
    refreshLogView();
}

// Modern Add Note: a free-text NOTE entry in the live register.  Round 72:
// the Qt3 QInputDialog (platform-ordered OK/Cancel) is replaced by a custom
// dialog mirroring the modern round-66 Add Note popup's control surface —
// the "Add Note"/"Cancel" button pair in the TUXEDO order (positive LEFT,
// Cancel RIGHT) with Add Note as the default AND the focused button, so
// Enter saves; Enter in the one-line field saves through returnPressed too.
// The modern multi-line memo field was evaluated and NOT mirrored (the
// round-72 evaluation: a Qt3 multi-line editor consumes Enter as a newline,
// which would break the Enter-saves flow this dialog guarantees, and the
// memo placeholder would add an 18-catalog translation key).
void LegacyMainWindow::addSessionNote()
{
    // Round 73 (A2 mirror): when the Logs list highlights a PRIOR session log,
    // the note is appended into THAT log file (the modern behavior); otherwise
    // it goes to the live register as before.  The dialog names the target
    // log, and an unwritable target fails closed (the note is never silently
    // redirected into the live register).
    const bool targetIsPrior = m_viewingPriorLog && !m_priorLogPath.isEmpty();
    const QString priorFileName =
        targetIsPrior ? QFileInfo(m_priorLogPath).fileName() : QString::null;
    QDialog dialog(this, "legacy-add-note", true);
    dialog.setCaption(tr("Add Note"));
    QVBoxLayout *layout = new QVBoxLayout(&dialog, 10, 8);
    QLabel *label = new QLabel(tr("Note:"), &dialog);
    layout->addWidget(label);
    if (targetIsPrior) {
        QLabel *targetLine = new QLabel(
            tr("This note will be appended to the selected session log %1.")
                .arg(priorFileName),
            &dialog);
        // Round 74 (modern parity): the target line word-wraps inside the
        // pinned 640-px width (the modern configureWrappedDialogBody shape;
        // Qt3 has no QLabel::setWordWrap, so this is the round-28 wrap
        // idiom).
        enableLabelWordWrap(targetLine);
        layout->addWidget(targetLine);
    }
    // Round 74 (modern parity, the user's ask): the note input is a
    // MULTI-LINE memo (the modern round-66 QPlainTextEdit shape).  Enter
    // inserts a newline INSIDE the memo; the Add Note button stays the
    // default + focused (the round-72 order/default), so a dialog-level
    // Enter still commits while the button has the focus.  The greyed
    // in-field placeholder is the round-31 PlaceholderTextEdit stand-in for
    // the modern setPlaceholderText().
    PlaceholderTextEdit *memo = new PlaceholderTextEdit(&dialog, "addNoteMemo");
    memo->setPlaceholderText(tr("Enter the note text"));
    memo->setMinimumHeight(96);
    memo->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    layout->addWidget(memo, 1);
    QHBoxLayout *buttons = new QHBoxLayout(layout);
    buttons->setSpacing(6);
    buttons->addStretch();
    QPushButton *cancel = new QPushButton(tr("Cancel"), &dialog);
    QPushButton *addNote = new QPushButton(tr("Add Note"), &dialog);
    addNote->setDefault(true);
    buttons->addWidget(addNote);
    buttons->addWidget(cancel);
    // The Add Note button owns the focus too (Enter saves); the focus is
    // re-asserted after the dialog's show-time focus pass has settled (the
    // round-27 trap — a 0-timer can fire before the posted show-focus
    // events).
    addNote->setFocus();
    QTimer::singleShot(100, addNote, SLOT(setFocus()));
    QObject::connect(cancel, SIGNAL(clicked()), &dialog, SLOT(reject()));
    QObject::connect(addNote, SIGNAL(clicked()), &dialog, SLOT(accept()));
    // Round 74 (modern parity): the popup is PINNED at the modern 640-px
    // width (the round-66/73-A5 dialog).  Qt 3.3.7 trap (rig-measured): a
    // top-level dialog's default layout resize mode (SetMinimumSize)
    // re-derives the widget minimum from the layout's ~220-px content hint
    // on every activation, silently clobbering an explicit minimum and
    // rendering the popup at its narrow one-line width (the round-72
    // behaviour, measured 224/257 px on the rig); FreeResize stops the
    // layout from overriding the widget's explicit bounds.  The minimum
    // HEIGHT is taken from the layout hint so the popup cannot be shrunk
    // below its content, and the memo's Expanding policy + stretch keep
    // the dialog vertically resizable exactly like the modern one.
    layout->setResizeMode(QLayout::FreeResize);
    dialog.setMinimumWidth(640);
    dialog.setMaximumWidth(640);
    const QSize addNoteHint = dialog.sizeHint();
    dialog.setMinimumHeight(addNoteHint.height());
    dialog.resize(640, addNoteHint.height());
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }
    const QString note = memo->text();
    const QString trimmed = note.stripWhiteSpace();
    if (trimmed.isEmpty()) {
        return;
    }
    if (!targetIsPrior) {
        appendLog(tr("NOTE: %1").arg(trimmed));
        return;
    }

    // Round 73 (A2 mirror): the same containment guards as
    // deleteSelectedSessionLog() (canonical directory + plain regular file,
    // never a symlink) decide whether the selected path may be appended to.
    const QString canonicalDirectory = QDir(m_logDirectory).canonicalPath();
    if (canonicalDirectory.isEmpty()) {
        QMessageBox::warning(this, tr("Add Note"),
                             tr("The selected log entry is not a valid session log; the note was not written."),
                             QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
        return;
    }
    const QString canonicalPath = canonicalDirectory + QString::fromLatin1("/") + priorFileName;
    QFileInfo fileInfo(canonicalPath);
    if (!fileInfo.exists() || !fileInfo.isFile() || fileInfo.isSymLink()) {
        QMessageBox::warning(this, tr("Add Note"),
                             tr("The selected log entry is not a valid session log; the note was not written."),
                             QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
        return;
    }
    QFile file(canonicalPath);
    if (!file.open(IO_WriteOnly | IO_Append)) {
        // Fail closed: a root-owned log (an elevated run's file) cannot be
        // appended to by the unprivileged GUI; the note is not written and the
        // reason is shown (modern parity — the note is never silently moved
        // into the live register).
        QMessageBox::warning(this, tr("Add Note"),
                             tr("The selected session log %1 is not writable; the note was not added.")
                                 .arg(priorFileName),
                             QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
        return;
    }
    // The same on-disk entry shape the live register writes
    // (formattedLogEntry: "---- APPLICATION ----" header + timestamped line).
    const QString timestamp =
        QDateTime::currentDateTime().toString(QString::fromLatin1("yyyy-MM-dd hh:mm:ss"));
    const QString entry = QString::fromLatin1("---- APPLICATION ----\n[%1] [INFO] %2")
        .arg(timestamp).arg(tr("NOTE: %1").arg(trimmed));
    // Qt 3.3.7 trap (round 75): QString::local8Bit() returns a QCString whose
    // QMemArray size() includes the terminating NUL; writing size() bytes
    // embeds the NUL in the log file, and the GUI's own
    // QString::fromLocal8Bit decode stops at the first NUL — the appended
    // note then never appears in ANY read of the file (the round-75
    // "the note vanished" divergence).  Write the entry's character bytes
    // without the terminator, exactly like the modern toUtf8() write.
    const QByteArray bytes = entry.local8Bit();
    Q_ULONG byteCount = bytes.size();
    if (byteCount > 0 && bytes.at(byteCount - 1) == '\0') {
        --byteCount;
    }
    file.writeBlock(bytes.data(), byteCount);
    file.writeBlock("\n", 1);
    file.close();
    // The register line: an audit entry in the live register naming where the
    // note went, then a re-read of the prior log so the note appears in the
    // view immediately.
    appendLog(tr("NOTE appended to prior session log %1: %2")
                  .arg(priorFileName, trimmed));
    sessionLogSelectionChanged(0);
}

// Modern Delete: only a selected prior session file inside the configured log
// directory, never the live file, after a confirmation. The file-name and
// canonical-path guards mirror the modern deleteSelectedSessionLog().
void LegacyMainWindow::deleteSelectedSessionLog()
{
    if (!m_sessionLogList) {
        return;
    }
    QListViewItem *item = m_sessionLogList->currentItem();
    if (!item || item == m_sessionLogList->firstChild()) {
        return;
    }
    const QString fileName = item->text(0);
    if (fileName.isEmpty() || !fileName.endsWith(QString::fromLatin1(".log"))) {
        return;
    }
    const QString path = m_logDirectory + QString::fromLatin1("/") + fileName;
    if (m_logDirectory.isEmpty() || !path.startsWith(m_logDirectory)) {
        return;
    }
    if (!m_logPath.isEmpty() && path == m_logPath) {
        return;
    }
    // Canonical containment: never follow a symlink out of the log directory.
    // Qt3 QFileInfo has no canonicalFilePath(), so canonicalize the directory
    // and rebuild the candidate path from the plain file name.
    const QString canonicalDirectory = QDir(m_logDirectory).canonicalPath();
    if (canonicalDirectory.isEmpty()) {
        return;
    }
    const QString canonicalPath = canonicalDirectory + QString::fromLatin1("/") + fileName;
    QFileInfo fileInfo(canonicalPath);
    if (!fileInfo.exists() || !fileInfo.isFile() || fileInfo.isSymLink()) {
        return;
    }
    // Record the file identity before the confirmation, then re-check with
    // lstat() immediately before the unlink: only a regular file with the
    // same device/inode may be removed, so a path swapped for a symlink or
    // another file while the confirmation dialog is open is refused.
    struct stat earlier;
    if (::lstat(canonicalPath.local8Bit().data(), &earlier) != 0
        || !S_ISREG(earlier.st_mode)) {
        return;
    }
    // Round 31 (Debian Legacy #9, item 4) widened the confirmation to double
    // the standard cap so the full timestamped log file name fit on one line;
    // round 34 (Etch #4) replaces the over-wide ~920-px dialog with a sane
    // fixed column (kDeleteLogConfirmTextWidth): the dialog pins at 360 px
    // (~35% of the application width) and the full wording wraps inside it.
    const bool answer = confirmWrapped(
        this, tr("Delete session log"),
        tr("Delete %1 permanently? This cannot be undone.")
            .arg(fileName),
        true, QString::null, QString::null,
        kDeleteLogConfirmTextWidth, /*noIsDefault=*/true);
    if (!answer) {
        return;
    }
    struct stat current;
    if (::lstat(canonicalPath.local8Bit().data(), &current) != 0
        || !S_ISREG(current.st_mode)
        || current.st_dev != earlier.st_dev
        || current.st_ino != earlier.st_ino) {
        QMessageBox::warning(
            this, tr("Delete session log"),
            tr(
                "%1 changed while the confirmation was open; the delete was "
                "refused.").arg(fileName),
            QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    // Round 50 (Etch log deletion): the delete goes through the authenticated
    // privileged helper instead of a direct unlink, so a stored log created by
    // a different user (a root smoke run, an elevated session) is removable.
    // The confirmation-time identity guards above stay; the helper re-verifies
    // containment and lstat identity in its own channel.  helperFinished
    // consumes the stable `Session log delete:` marker, refreshes the session
    // list and reports failures.  startCommand fails closed when no
    // administrator session is active (with the Authorize remedy) — the
    // delete is privileged by design, exactly like every other helper verb.
    // Round 75 (mid-run log deletion): while a diagnostic/repair owns the
    // single helper channel (m_running) the delete is no longer refused or
    // silently dropped — this housekeeping verb is independent of the target
    // session state (no target mount, no per-disk lock), so it runs
    // CONCURRENTLY through a one-shot helper invocation
    // (HelperRunner::runOneShotCaptured) that never touches the running
    // command.  Genuine failures still report the reason.
    QStringList args;
    args << QString::fromLatin1("session-log-delete") << selectedDisk() << selectedRoot()
         << m_logDirectory << fileName;
    if (m_running) {
        QByteArray captured;
        int exitCode = -1;
        QString errorDescription;
        const bool ran = m_runner->runOneShotCaptured(
            args, &captured, &exitCode, &errorDescription);
        const QString transcript = QString::fromLocal8Bit(
            captured.data(), captured.size());
        const bool markerOk = transcript.find(
                QString::fromLatin1("Session log delete: ") + fileName
                + QString::fromLatin1("|deleted")) >= 0
            || transcript.find(
                QString::fromLatin1("Session log delete: ") + fileName
                + QString::fromLatin1("|missing")) >= 0;
        if (!ran || !markerOk) {
            QString reason = errorDescription;
            const QString stripped = transcript.stripWhiteSpace();
            if (!stripped.isEmpty()) {
                if (!reason.isEmpty()) {
                    reason += QString::fromLatin1("\n");
                }
                reason += stripped;
            }
            // Bound the reason so the dialog stays concise.
            if (reason.length() > 300) {
                reason = reason.right(300);
            }
            QMessageBox::warning(
                this, tr("Delete session log"),
                reason.isEmpty()
                    ? tr("Unable to delete %1.").arg(fileName)
                    : tr("Unable to delete %1.\n\n%2").arg(fileName).arg(reason),
                QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
            return;
        }
        // The same completion as the helperFinished delete branch: the audit
        // line, the prior-log view reset when the deleted file was being
        // viewed, and the list refresh (no dangling records).
        appendLog(tr("Deleted prior session log %1.").arg(fileName));
        if (m_viewingPriorLog && m_priorLogPath == path) {
            m_viewingPriorLog = false;
            m_priorLogPath = QString::null;
            m_priorLogLines.clear();
            if (m_priorLogBanner) {
                m_priorLogBanner->hide();
            }
        }
        refreshSessionLogList();
        refreshLogView();
        return;
    }
    m_pendingSessionLogDeleteName = fileName;
    m_pendingSessionLogDeletePath = path;
    if (!startCommand(args, false, tr("Delete session log"),
                      false, false, false, false, QString::null, QString::null,
                      false, false)) {
        // Busy or authorization refused: startCommand already reported the
        // reason; drop the pending state and leave the file in place.
        m_pendingSessionLogDeleteName = QString::null;
        m_pendingSessionLogDeletePath = QString::null;
    }
}

// ---- persisted options (modern-compatible QSettings key names) -------------

void LegacyMainWindow::loadLegacySettings()
{
    if (legacySmokeSettingsIsolation()) {
        // --smoke-test is deterministic: persisted overrides are ignored and
        // the in-code defaults apply, so a user's ~/.qt/devicesrc|logsrc|
        // diagnosticsrc|repairrc overrides can never flip a smoke assertion,
        // and the smoke never reads or writes those files.
        applyLegacySettingsDefaults();
        m_settingsLoaded = true;
        return;
    }
    QSettings settings;
    configureLegacySettings(settings);
    // Qt 3.3.7 stores each settings group in its own per-user file named after
    // the group, so the keys below live in ~/.qt/devicesrc, ~/.qt/logsrc,
    // ~/.qt/diagnosticsrc and ~/.qt/repairrc.  configureLegacySettings forces
    // User scope (setPath(..., QSettings::User)); without it the DEFAULT is
    // Global scope, so a root run would write the system path /etc/qt3/ and a
    // stale ~/.qt/repairrc would mask the fresh values on the merge read.
    // dpkg -r / dpkg -i reinstall never touches these files.
    m_showNonLinux = settings.readBoolEntry(
        QString::fromLatin1("/devices/showNonLinux"), true);
    m_showRemovable = settings.readBoolEntry(
        QString::fromLatin1("/devices/showRemovable"), true);
    m_showEncrypted = settings.readBoolEntry(
        QString::fromLatin1("/devices/showEncrypted"), true);
    m_logWrapEnabled = settings.readBoolEntry(
        QString::fromLatin1("/logs/wrapLines"), true);
    m_autoRefreshEnabled = settings.readBoolEntry(
        QString::fromLatin1("/diagnostics/autoRefreshStale"), true);

    // The device-discovery filters re-apply from their register (the plan
    // path's updatePlanChecks() mirror); the auto-refresh and log-wrap widgets
    // are set directly below.
    updateDeviceFilterChecks();
    QCheckBox *checks[] = { m_autoRefreshCheck, m_logWrapCheck };
    const bool values[] = { m_autoRefreshEnabled, m_logWrapEnabled };
    for (int i = 0; i < 2; ++i) {
        if (!checks[i]) {
            continue;
        }
        checks[i]->blockSignals(true);
        checks[i]->setChecked(values[i]);
        checks[i]->blockSignals(false);
    }
    if (m_logView) {
        m_logView->setWordWrap(m_logWrapEnabled ? QTextEdit::WidgetWidth
                                                : QTextEdit::NoWrap);
    }
    if (m_viewMenu && m_wrapLogsMenuValid) {
        m_viewMenu->setItemChecked(m_wrapLogsMenuId, m_logWrapEnabled);
    }
    // Full Repair plan checkboxes (modern-compatible key names). The saved
    // preference is kept separately from the widget state so the availability
    // pass (updatePlanChecks) can force unavailable rows off without losing
    // the user's choice. The preference is a plain per-stage user setting: it
    // never depends on diagnostic-cache warmth, so a stage stays checkable
    // before its capability line arrives.
    m_planPreferences.assign(planSpecCount, false);
    for (int i = 0; i < planSpecCount; ++i) {
        // Every stage's register entry comes from its persisted key (or the
        // default when the key is absent), whether or not its checkbox exists.
        const bool checked = settings.readBoolEntry(
            QString::fromLatin1(planSpecs[i].settingsKey),
            planSpecs[i].defaultChecked);
        m_planPreferences[i] = checked;
        if (i < static_cast<int>(m_planChecks.size()) && m_planChecks[i]) {
            m_planChecks[i]->blockSignals(true);
            m_planChecks[i]->setChecked(checked);
            m_planChecks[i]->blockSignals(false);
        }
    }
    // The register now holds the persisted selection (or the default of each
    // absent key): presentation and saves may use it from here on.
    m_settingsLoaded = true;
    // Bug B diagnostic: record the resolved read location and the value loaded
    // for the repair/grub key, so a post-deploy manual test can confirm the
    // toggle round-trip.  Emitted by the constructor once the session log
    // directory exists (appendLog here would bypass a pre-set --log-dir).
    {
        const int grubIdx = planIndexForCapability("grub");
        const QString grubValue = (grubIdx >= 0
                                   && grubIdx < static_cast<int>(m_planPreferences.size()))
            ? (m_planPreferences[grubIdx]
                   ? QString::fromLatin1("true") : QString::fromLatin1("false"))
            : QString::fromLatin1("unknown");
        m_settingsLoadDiagnostic = tr(
            "Settings load: repairrc read from %1; repair/grub = %2; "
            "devices/showNonLinux = %3; diagnostics/autoRefreshStale = %4")
            .arg(legacySettingsRepairRcPath()).arg(grubValue)
            .arg(m_showNonLinux ? QString::fromLatin1("true")
                                : QString::fromLatin1("false"))
            .arg(m_autoRefreshEnabled ? QString::fromLatin1("true")
                                      : QString::fromLatin1("false"));
    }
}

void LegacyMainWindow::saveLegacySettings()
{
    if (legacySmokeSettingsIsolation()) {
        // The smoke never writes the user's settings files.
        return;
    }
    // Fail closed: nothing is written before loadLegacySettings() has filled the
    // preference register, and the register is never refilled with defaults
    // here (that refill would persist the in-code defaults over every saved key,
    // including the Full Repair selection).
    if (!m_settingsLoaded
        || m_planPreferences.size() != static_cast<std::size_t>(planSpecCount)) {
        appendLog(tr(
            "Settings save: skipped (settings not loaded: m_settingsLoaded=%1, "
            "preference count=%2 of %3); the plan selection was not written.")
            .arg(m_settingsLoaded ? QString::fromLatin1("true")
                                  : QString::fromLatin1("false"))
            .arg(static_cast<int>(m_planPreferences.size()))
            .arg(planSpecCount));
        return;
    }
    QSettings settings;
    configureLegacySettings(settings);
    // Same per-group layout as loadLegacySettings: configureLegacySettings
    // forces User scope so writes go to ~/.qt/ (devicesrc, logsrc,
    // diagnosticsrc, repairrc), never the system path.
    settings.writeEntry(QString::fromLatin1("/devices/showNonLinux"), m_showNonLinux);
    settings.writeEntry(QString::fromLatin1("/devices/showRemovable"), m_showRemovable);
    settings.writeEntry(QString::fromLatin1("/devices/showEncrypted"), m_showEncrypted);
    settings.writeEntry(QString::fromLatin1("/logs/wrapLines"), m_logWrapEnabled);
    settings.writeEntry(QString::fromLatin1("/diagnostics/autoRefreshStale"),
                        m_autoRefreshEnabled);
    // The persisted plan preference is written from the preference register
    // (never from the widget state, which the availability pass may have
    // forced off for unavailable rows).
    for (int i = 0; i < planSpecCount; ++i) {
        settings.writeEntry(QString::fromLatin1(planSpecs[i].settingsKey),
                            m_planPreferences[i]);
    }
    // Flush the buffered writes to the canonical settings directory now
    // (Qt 3.3.7 QSettings::sync(), the modern m_settings->sync() parity).
    // QSettings::writeEntry only updates the in-memory map and relies on the
    // QSettings destructor to flush; an explicit sync() makes the write
    // durable the moment the toggle or save happens, so a freshly-checked
    // plan stage survives a later
    // abnormal exit. It also runs BEFORE the chmod pass below, so the
    // per-group files already exist when they are tightened to 0600.
    const bool syncOk = settings.sync();
    // Bug B diagnostic: report the resolved write path and the sync result on
    // EVERY save (success and failure), so a silent non-persist is visible.
    appendLog(tr(
        "Settings save: repairrc written to %1 (sync %2).")
        .arg(legacySettingsRepairRcPath())
        .arg(syncOk ? QString::fromLatin1("ok") : QString::fromLatin1("failed")));
    // The four per-group files Qt 3.3.7 wrote under ~/.qt carry the GUI's
    // persisted state; keep them private (0600) like the session logs. A
    // file that was never written (its group stayed untouched) is missing and
    // its chmod failure is ignored.
    const QString qtDirectory = QDir::homeDirPath() + QString::fromLatin1("/.qt");
    static const char *const settingsFiles[] = {
        "devicesrc", "logsrc", "diagnosticsrc", "repairrc"
    };
    for (int i = 0;
         i < static_cast<int>(sizeof(settingsFiles) / sizeof(settingsFiles[0])); ++i) {
        const QString settingsPath = qtDirectory + QString::fromLatin1("/")
            + QString::fromLatin1(settingsFiles[i]);
        ::chmod(settingsPath.local8Bit().data(), 0600);
    }
}

// The deterministic default settings (smoke isolation): the same values
// loadLegacySettings uses as read defaults, applied without touching QSettings.
void LegacyMainWindow::applyLegacySettingsDefaults()
{
    m_logWrapEnabled = true;
    m_autoRefreshEnabled = true;
    QCheckBox *checks[] = { m_showNonLinuxCheck, m_showRemovableCheck,
                            m_showEncryptedCheck, m_autoRefreshCheck,
                            m_logWrapCheck };
    const bool values[] = { true, true, true, true, true };
    for (int i = 0; i < 5; ++i) {
        if (!checks[i]) {
            continue;
        }
        checks[i]->blockSignals(true);
        checks[i]->setChecked(values[i]);
        checks[i]->blockSignals(false);
    }
    if (m_logView) {
        m_logView->setWordWrap(m_logWrapEnabled ? QTextEdit::WidgetWidth
                                                : QTextEdit::NoWrap);
    }
    if (m_viewMenu && m_wrapLogsMenuValid) {
        m_viewMenu->setItemChecked(m_wrapLogsMenuId, m_logWrapEnabled);
    }
    m_planPreferences.assign(planSpecCount, false);
    for (int i = 0; i < planSpecCount; ++i) {
        m_planPreferences[i] = planSpecs[i].defaultChecked;
    }
    for (int i = 0; i < planSpecCount
                    && i < static_cast<int>(m_planChecks.size()); ++i) {
        if (!m_planChecks[i]) {
            continue;
        }
        m_planChecks[i]->blockSignals(true);
        m_planChecks[i]->setChecked(planSpecs[i].defaultChecked);
        m_planChecks[i]->blockSignals(false);
    }
}

void LegacyMainWindow::deviceFilterChanged(bool checked)
{
    // Mirror autoRefreshToggled(bool): set the ONE toggled member from the
    // signal parameter (sender() identifies which checkbox), never re-read all
    // three widgets, so the user's change is recorded from the authoritative
    // signal value.
    const QObject *toggled = sender();
    if (toggled == m_showNonLinuxCheck) {
        m_showNonLinux = checked;
    } else if (toggled == m_showRemovableCheck) {
        m_showRemovable = checked;
    } else if (toggled == m_showEncryptedCheck) {
        m_showEncrypted = checked;
    }
    saveLegacySettings();
    rebuildDeviceList();
    updateActionStates();
}

void LegacyMainWindow::autoRefreshToggled(bool enabled)
{
    m_autoRefreshEnabled = enabled;
    saveLegacySettings();
    updateActionStates();
}

// Modern Capabilities and dependencies: read-only labels fed by the host's
// /etc/os-release and package-manager facts, plus the KAuth build support
// line (always "Not compiled" on Etch, which has no KAuth).
// Modern refreshCapabilities(): repopulate the summary labels and the
// 7-column capability table. Host scope = the existing read-only host PATH
// probe; Repair-target scope = the cached target diagnostics' `Repair tool`
// lines (the dropdown only re-reads cached evidence, never a new helper run).
void LegacyMainWindow::refreshCapabilities()
{
    if (m_capDistributionLabel) {
        const QString pretty = readOsReleaseValue(QString::fromLatin1("PRETTY_NAME"));
        m_capDistributionLabel->setText(pretty.isEmpty()
            ? tr("Unknown Linux distribution") : pretty);
    }
    if (m_capPackageManagerLabel) {
        m_capPackageManagerLabel->setText(packageManagerLabelForHost());
    }
    if (m_capAuthLabel) {
        m_capAuthLabel->setText(tr(
            "Not compiled - optional integration unavailable"));
    }
    if (!m_capabilityTable) {
        return;
    }
    // Scope selector (modern parity): Host shows the existing read-only host
    // PATH probe; Repair target re-reads the cached target diagnostics'
    // `Repair tool` lines. The legacy table's native data source is the host
    // probe, so Host (item 1) is the default scope.
    const bool targetScope = m_capabilityScopeSelector
        && m_capabilityScopeSelector->currentItem() == 0;
    if (!targetScope) {
        // Host scope: the 18 consolidated capabilitySpecs rows (the modern
        // feature list plus the two Etch-specific rows), probed read-only from
        // the running host's PATH plus the standard system directories.
        m_capabilityTable->setNumRows(capabilitySpecCount);
        for (int row = 0; row < capabilitySpecCount; ++row) {
            const CapabilitySpec &spec = capabilitySpecs[row];
            const QString command = QString::fromLatin1(spec.command);
            const bool available = !findExecutablePath(command).isEmpty();
            QString status;
            if (available) {
                status = tr("Available");
            } else if (command == QString::fromLatin1("unshare")) {
                // The legacy-specific row: Etch has no unshare, so the ported
                // helper falls back to a guarded plain chroot.
                status = tr("Missing on this frontend");
            } else {
                status = tr("Missing");
            }
            const QString package = QString::fromLatin1(spec.package);
            // Item 2: the Action cell offers "Install" only for a row that is
            // missing AND has a package with an apt install candidate on THIS
            // Etch system AND whose suggested package can actually deliver the
            // command (read-only probes, never an install). The Install Missing
            // Support... button below installs the SELECTED row's package
            // through the guarded helper verb.
            const bool installable = !available && !package.isEmpty()
                && hostPackageInstallable(package)
                && legacyPackageProvidesCommand(package, command);
            m_capabilityTable->setText(row, 0, tr(spec.feature));
            m_capabilityTable->setText(row, 1, command);
            m_capabilityTable->setText(row, 2, tr(spec.scope));
            m_capabilityTable->setText(row, 3, status);
            m_capabilityTable->setText(row, 4, package);
            m_capabilityTable->setText(row, 5, tr(spec.note));
            m_capabilityTable->setText(row, 6,
                installable ? tr("Install")
                            : QString::fromLatin1(kCapabilityActionCell));
        }
        appendLog(tr(
            "Host capability scan refreshed (read-only PATH probes; nothing was "
            "executed). Missing optional tools disable only the related features."));
    } else {
        // Repair-target scope: reflect the cached target diagnostics' `Repair
        // tool` lines. Fail closed: with no cached target evidence the table
        // shows a single "run diagnostics first" placeholder row, never a
        // fabricated availability.
        const QString targetIdentity = QString::fromLatin1("target|")
            + selectedDisk();
        if (!m_model.hasDiagnostics(toStd(targetIdentity))) {
            m_capabilityTable->setNumRows(1);
            m_capabilityTable->setText(0, 0, tr("No capability evidence cached"));
            m_capabilityTable->setText(0, 1, QString::null);
            m_capabilityTable->setText(0, 2, tr("Repair target"));
            m_capabilityTable->setText(0, 3, QString::null);
            m_capabilityTable->setText(0, 4, QString::null);
            m_capabilityTable->setText(0, 5, tr(
                "Run read-only target diagnostics to populate this table."));
            m_capabilityTable->setText(0, 6,
                QString::fromLatin1(kCapabilityActionCell));
            // Round 32 (Etch #3): no log line here (modern parity). The
            // placeholder row is a table state, not a log event; the earlier
            // "evidence is not cached; run target diagnostics first" line
            // stayed in the log after diagnostics ran and read as a stale
            // capability verdict.
        } else {
            // The 15 Repair tool keys map 1:1 to the individual-tool specs
            // (same order), so the friendly title comes from toolSpecs while
            // the state/reason come from the cached target evidence.
            // Item 2 (target-scope install): a missing capability row whose
            // reason names an installable prerequisite (the module-build
            // build-system package) shows "Install" in the Action cell and its
            // package in the Suggested package column, routed to the helper's
            // install-target-package verb. Fail closed: the affordance only
            // appears while a repair target is committed AND the helper proved
            // (read-only, against the TARGET's own repositories) that the
            // suggested package is present - `Repair capability evidence
            // dkms-installable: available`. No repo evidence -> no Install.
            const bool committed = targetCommitted();
            const bool dkmsRepoAvailable = m_model.evidence(
                toStd(QString::fromLatin1("dkms-installable"))) == "available";
            // Round 48 (user request): the display row's Install suggestion
            // reads the helper's proven side-channel (the package + the
            // |available proof) — the modern CapabilityChecker parity.
            const std::string displaySideChannel = m_model.evidence(
                toStd(QString::fromLatin1("display-installable")));
            m_capabilityTable->setNumRows(toolSpecCount);
            for (int i = 0; i < toolSpecCount; ++i) {
                const QString key = QString::fromLatin1(toolSpecs[i].key);
                const std::string state = m_model.state(toStd(key));
                std::string reason;
                const bool available = capabilityIsAvailable(state, &reason);
                QString package = legacyTargetInstallablePackage(key, state);
                bool repoProven = dkmsRepoAvailable;
                if (key == QString::fromLatin1("display")) {
                    package = legacyTargetDisplayInstallablePackage(
                        key, state, displaySideChannel);
                    repoProven = !package.isEmpty();
                }
                const bool installable = !available && !package.isEmpty()
                    && committed && repoProven;
                m_capabilityTable->setText(i, 0, tr(toolSpecs[i].title));
                m_capabilityTable->setText(i, 1, key);
                m_capabilityTable->setText(i, 2, tr("Repair target"));
                m_capabilityTable->setText(i, 3,
                    available ? tr("Available") : tr("Missing"));
                m_capabilityTable->setText(i, 4, package);
                m_capabilityTable->setText(i, 5,
                    available ? QString::null : fromStd(reason));
                m_capabilityTable->setText(i, 6,
                    installable ? tr("Install")
                                : QString::fromLatin1(kCapabilityActionCell));
            }
            appendLog(tr(
                "Repair-target capability evidence refreshed from the cached "
                "target diagnostics."));
        }
    }
    // Modern parity: the first row starts selected and clicking any cell
    // selects its whole row (QTable::SingleRow above). The Install Missing
    // Support... button's enabled state follows the selected row's Action cell
    // ("Install" for a missing+installable row).
    if (m_capabilityTable->numRows() > 0) {
        m_capabilityTable->setCurrentCell(0, 0);
        m_capabilityTable->selectRow(0);
    }
    updateInstallSupportButton();
}

// Item 2: the Install Missing Support... button is enabled exactly when the
// selected capability row is missing, installable and has a package (its
// Action cell shows "Install"). Fail closed: any other row keeps it disabled.
void LegacyMainWindow::updateInstallSupportButton()
{
    if (!m_installSupportButton || !m_capabilityTable) {
        return;
    }
    bool installableRow = false;
    const int row = m_capabilityTable->currentRow();
    if (row >= 0 && row < m_capabilityTable->numRows()) {
        installableRow = m_capabilityTable->text(row, 6) == tr("Install");
    }
    m_installSupportButton->setEnabled(installableRow && !m_running);
}

// QTable currentChanged: the Install button follows the newly selected row.
void LegacyMainWindow::capabilitySelectionChanged(int, int)
{
    updateInstallSupportButton();
}

// Installs the SELECTED capability row's suggested package through the helper's
// guarded install verb (simulation-first, fail-closed; never a bare apt-get).
// Host scope runs install-host-package; the Repair-target scope runs
// install-target-package into the committed target (fail-closed: no committed
// target -> no install affordance, so this never runs for a target that is not
// committed). Mirrors the modern installMissingCapability branches.
void LegacyMainWindow::installSelectedCapabilityPackage()
{
    if (m_running || !m_capabilityTable) {
        return;
    }
    const int row = m_capabilityTable->currentRow();
    if (row < 0 || row >= m_capabilityTable->numRows()) {
        return;
    }
    const QString package = m_capabilityTable->text(row, 4).stripWhiteSpace();
    // The Action cell is the single source of truth for installability
    // ("Install" only for a missing, installable row).
    if (package.isEmpty() || m_capabilityTable->text(row, 6) != tr("Install")) {
        return;
    }
    if (!administratorSessionActive()) {
        QMessageBox::warning(this, tr("Administrator authorization required"),
                             tr(
                                 "No administrator session is active; press "
                                 "Authorize first."),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    // Item 2 (target-scope install): the scope selector (Repair target == 0,
    // Host == 1) routes the same selected-row install to the matching helper
    // verb. The target scope is fail-closed: without a committed target the
    // Action cell never offered Install, and this double-check refuses anyway.
    const bool targetScope = m_capabilityScopeSelector
        && m_capabilityScopeSelector->currentItem() == 0;
    QString disk;
    QString root;
    if (targetScope) {
        if (!targetCommitted()) {
            QMessageBox::warning(this, tr("Repair target required"),
                                 scopeReadyReason(),
                                 QMessageBox::Ok, QMessageBox::NoButton);
            return;
        }
        disk = selectedDisk();
        root = selectedRoot();
    } else {
        disk = runningHostDisk().isEmpty() ? selectedDisk() : runningHostDisk();
        root = runningHostRoot().isEmpty() ? selectedRoot() : runningHostRoot();
    }
    if (disk.isEmpty() || root.isEmpty()) {
        QMessageBox::warning(this, tr("Install unavailable"),
                             tr(
                                 "The selected scope's disk and root component "
                                 "could not be resolved."),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    const bool answer = confirmWrapped(
        this, tr("Install Missing Support"),
        targetScope
            ? tr(
                  "Install the package '%1' into the selected repair target "
                  "through the guarded package backend? This modifies the "
                  "repair target and invalidates cached diagnostics.").arg(package)
            : tr(
                  "Install the package '%1' on the running host through the "
                  "guarded package backend? This modifies the running host and "
                  "invalidates cached running-host diagnostics.").arg(package),
        /*cancelInsteadOfNo=*/false, QString::null, QString::null,
        kHiddenInputMaximumWidth, /*noIsDefault=*/true);
    if (!answer) {
        return;
    }
    const QString verb = targetScope
        ? QString::fromLatin1("install-target-package")
        : QString::fromLatin1("install-host-package");
    QStringList args;
    args << verb << disk << root << package;
    const bool started = startCommand(args, false,
                                      tr("Install %1").arg(package),
                                      false, false, false, false,
                                      QString::null, verb);
    if (started && !m_smokeMode) {
        showRepairResultDialog(tr("Install %1").arg(package));
    }
}

// Cycle 14 loop: the tools list is sortable, so the selected tool resolves
// by the current row's TITLE (titles are unique), never by row position.
int LegacyMainWindow::toolIndexForTitle(const QString &title) const
{
    for (int i = 0; i < toolSpecCount; ++i) {
        if (tr(toolSpecs[i].title) == title) {
            return i;
        }
    }
    return -1;
}

int LegacyMainWindow::selectedToolIndex() const
{
    if (!m_toolList) {
        return -1;
    }
    QListViewItem *item = m_toolList->currentItem();
    if (!item) {
        return -1;
    }
    return toolIndexForTitle(item->text(0));
}

void LegacyMainWindow::toolSelectionChanged()
{
    updateToolDetails();
    updateActionStates();
}

// Cycle 14 loop: manual header-click sorting for the individual tools list.
// Qt3 cannot keep the curated order as the default while sorting is enabled,
// so the items are detached, reordered by the clicked column (toggling
// asc/desc), and re-inserted; the current selection and the Selected tool
// pane are then restored.
void LegacyMainWindow::toolListHeaderClicked(int column)
{
    if (!m_toolList || !m_toolList->header()
        || column < 0 || column >= m_toolList->columns()) {
        return;
    }
    const bool ascending = m_toolSortColumn == column
        ? !m_toolSortAscending : true;
    // Deterministic on Qt3: rebuild the rows in the sorted order instead of
    // detaching/re-inserting live items. Column 0 sorts by the tool title;
    // column 1 sorts by a static per-tool key (the plan stage or the plan
    // base), never by the dynamic availability text.
    std::vector<int> order;
    for (int i = 0; i < toolSpecCount; ++i) {
        order.push_back(i);
    }
    for (std::size_t i = 1; i < order.size(); ++i) {
        const int key = order[i];
        QString keyText = column == 0
            ? tr(toolSpecs[key].title)
            : (toolSpecs[key].planStage && *toolSpecs[key].planStage
                ? QString::fromLatin1(toolSpecs[key].planStage)
                : tr(toolSpecs[key].planBase));
        std::size_t j = i;
        while (j > 0) {
            const int prev = order[j - 1];
            const QString prevText = column == 0
                ? tr(toolSpecs[prev].title)
                : (toolSpecs[prev].planStage && *toolSpecs[prev].planStage
                    ? QString::fromLatin1(toolSpecs[prev].planStage)
                    : tr(toolSpecs[prev].planBase));
            const bool before = ascending
                ? prevText > keyText : prevText < keyText;
            if (!before) {
                break;
            }
            order[j] = order[j - 1];
            --j;
        }
        order[j] = key;
    }
    m_toolList->clear();
    QListViewItem *lastTool = 0;
    for (std::size_t i = 0; i < order.size(); ++i) {
        const ToolSpec &spec = toolSpecs[order[i]];
        lastTool = new QListViewItem(m_toolList, lastTool,
                                     tr(spec.title),
                                     tr("not reported"));
    }
    m_toolSortColumn = column;
    m_toolSortAscending = ascending;
    if (m_toolList->firstChild()) {
        m_toolList->setSelected(m_toolList->firstChild(), true);
        m_toolList->setCurrentItem(m_toolList->firstChild());
    }
    updateToolDetails();
}

// Round 52 (user request): the proven display-manager package the CURRENT
// scope's capability evidence offers for install — the display row's
// missing-DM state plus the helper's `Repair capability evidence
// display-installable: <pkg>|available` side-channel (the same derivation the
// capabilities table's Install cell uses). Fail closed: no matching state or
// side-channel -> empty.
QString LegacyMainWindow::displayInstallablePackage() const
{
    const std::string state = m_model.state(
        toStd(QString::fromLatin1("display")));
    const std::string sideChannel = m_model.evidence(
        toStd(QString::fromLatin1("display-installable")));
    return legacyTargetDisplayInstallablePackage(
        QString::fromLatin1("display"), state, sideChannel);
}

// Round 52 (user request): the display tool's unavailable info derived from
// the capability evidence — the localized capability reason plus, when the
// helper proved a display-manager package installable for the current scope,
// a one-line pointer at the Capabilities table's Install action. Never a
// fixed host-scope literal: the scope hint is a separate, evidence-gated
// state (see toolRunReady()).
QString LegacyMainWindow::displayToolUnavailableReason(
    const std::string &capabilityReason) const
{
    QString text = localizedCapabilityReason(fromStd(capabilityReason));
    const QString package = displayInstallablePackage();
    if (!package.isEmpty()) {
        text += QString::fromLatin1("\n") + tr(
            "Missing display manager: install the %1 package from the "
            "Capabilities table.").arg(package);
    }
    return text;
}

// Missing required diagnostic sections for a repair tool (empty when every
// required section is cached in m_diagnosticSections). The fail-closed repair
// gate consumes this list, mirroring MainWindow::missingRequiredDiagnosticTitles.
QStringList LegacyMainWindow::missingRequiredDiagnosticSections(
    const QString &toolKey) const
{
    const QStringList required = requiredDiagnosticSectionsForTool(toolKey);
    QStringList missing;
    for (QStringList::ConstIterator it = required.begin(); it != required.end(); ++it) {
        if (mapValue(m_diagnosticSections, *it).stripWhiteSpace().isEmpty()) {
            missing.append(*it);
        }
    }
    return missing;
}

// True when the selected tool may run now; `reason` always receives the exact
// explanation. Fail closed: scope, session and the cached capability line
// (plus the host-maintenance probe for the host initramfs/grub stages) must
// all agree. Display-only tools never run.
bool LegacyMainWindow::toolRunReady(int toolIndex, QString *reason) const
{
    if (toolIndex < 0 || toolIndex >= toolSpecCount) {
        if (reason) {
            *reason = tr("No repair tool is selected.");
        }
        return false;
    }
    const ToolSpec &spec = toolSpecs[toolIndex];
    if (m_running) {
        if (reason) {
            *reason = tr("A helper command is already running.");
        }
        return false;
    }
    // Round 55 (smoke-caught): the round-54 hostOnly removal also removed
    // the display tool's EARLY capability check (it used to live inside the
    // hostOnly branch), so the no-scope/no-evidence pane fell back to the
    // generic scope remedy and broke the round-52 contract (the offline
    // display info must be capability-derived).  Restore the
    // capability-first ordering for the display tool: absent capability
    // evidence names the missing evidence (the round-52 offline reason),
    // present evidence falls through to the ordinary scope gates (the
    // round-54 target-scope repair).
    if (QString::fromLatin1(spec.key) == QString::fromLatin1("display")) {
        std::string displayCapabilityReason;
        if (!m_model.isAvailable(spec.key, toStd(identity()),
                                 &displayCapabilityReason)) {
            if (reason) {
                *reason = displayToolUnavailableReason(
                    displayCapabilityReason);
            }
            return false;
        }
    }
    if (spec.hostOnly && !hostScope()) {
        // Round 52 (user request): capability-derived info instead of a fixed
        // host-scope literal. A missing display manager surfaces the localized
        // capability reason plus the proven Install action (the round-48
        // target-scope install); a present display manager surfaces the scope
        // hint.  Round 54: the display repair is no longer host-only, so this
        // branch now serves only a future genuinely host-only stage.
        std::string scopeCapabilityReason;
        if (!m_model.isAvailable(spec.key, toStd(identity()),
                                 &scopeCapabilityReason)) {
            if (reason) {
                *reason = displayToolUnavailableReason(scopeCapabilityReason);
            }
        } else if (reason) {
            *reason = tr(
                "Restore Graphical Login is a host-scope stage on this legacy "
                "frontend; enter Host Maintenance to run it.");
        }
        return false;
    }
    if (!diagnosticsScopeReady()) {
        if (reason) {
            *reason = scopeReadyReason();
        }
        return false;
    }
    if (!selectionComplete()) {
        if (reason) {
            *reason = tr(
                "The selected scope has no resolved root component; use "
                "Refresh Devices and commit the repair target again.");
        }
        return false;
    }
    if (!spec.stage || !*spec.stage) {
        // Display-only tool: prefer the helper's cached capability reason; when
        // the helper reports the capability as available, name the frontend
        // omission instead (still fail closed, never runnable).
        std::string capabilityReason;
        if (!m_model.isAvailable(spec.key, toStd(identity()), &capabilityReason)) {
            if (reason) {
                *reason = localizedCapabilityReason(fromStd(capabilityReason));
            }
        } else if (reason) {
            *reason = tr(
                "This legacy frontend exposes no %1 action; the helper reports "
                "the capability as available.").arg(tr(spec.title));
        }
        return false;
    }
    std::string capabilityReason;
    if (!m_model.isAvailable(spec.key, toStd(identity()), &capabilityReason)) {
        if (reason) {
            // Round 52 (user request): the display tool's unavailable info is
            // capability-derived in the host scope too — the localized reason
            // plus the proven Install action when the helper proved a
            // display-manager package installable (never the reason alone).
            *reason = QString::fromLatin1(spec.key)
                    == QString::fromLatin1("display")
                ? displayToolUnavailableReason(capabilityReason)
                : localizedCapabilityReason(fromStd(capabilityReason));
        }
        return false;
    }
    if (spec.hostMaintenance && hostScope()) {
        std::string featureReason;
        if (!m_model.legacyFeatureAvailable("host-maintenance", toStd(identity()),
                                            &featureReason)) {
            if (reason) {
                *reason = fromStd(featureReason);
            }
            return false;
        }
    }
    // Single source of truth: every required diagnostic section must be cached
    // before the tool may run. Missing/stale evidence fails closed with the
    // same "run diagnostics first" remedy the modern GUI shows.
    const QStringList missing = missingRequiredDiagnosticSections(
        QString::fromLatin1(spec.key));
    if (!missing.isEmpty()) {
        QStringList titles;
        for (QStringList::ConstIterator it = missing.begin(); it != missing.end(); ++it) {
            titles.append(diagnosticTitle(*it));
        }
        if (reason) {
            *reason = tr("Run the %1 diagnostic for the selected scope first "
                         "(the cached evidence is missing).")
                          .arg(titles.join(QString::fromLatin1(", ")));
        }
        return false;
    }
    if (reason) {
        *reason = tr(
            "Run this guarded repair action using the cached read-only "
            "diagnostic evidence. A confirmation is shown first.");
    }
    return true;
}

// Ported from MainWindow::kernelsRemovalPreview(): parses the cached `kernel`
// diagnostic section into the concise kernel-inventory summary modern shows in
// the kernels tool detail pane (running/newest/repository-latest kernel,
// superseded kernels available for removal, and the orphaned/incomplete/
// missing-headers classes). Fail-closed: an empty/absent cached section returns
// an empty string and the detail pane keeps its static description only.
// gcc 4.1 (Etch) enforces the C++98 rule that a local type cannot be a
// template argument, so KernelRemoval lives at namespace scope rather than
// inside kernelsRemovalPreview(). (Qt3 QValueList<KernelRemoval> needs this.)
struct KernelRemoval {
    QString version;
    QStringList packages;
    unsigned long long sizeBytes;
    KernelRemoval() : sizeBytes(0) {}
};

// Parses the helper's read-only `File system check <device>: <fstype>
// uuid=<uuid> mount=<mount> tool=<tool> result=<result>` lines (plus the
// per-device `File system check detail ...` line) into records. Fail closed: a
// malformed or unrecognized line is skipped, never fabricated; the summary
// line is ignored.
QValueList<LegacyFsCheckResult> parseFilesystemCheckOutput(const QString &output)
{
    QValueList<LegacyFsCheckResult> results;
    QMap<QString, int> indexByDevice;
    const QStringList lines = QStringList::split(QChar('\n'), output, true);
    for (QStringList::ConstIterator it = lines.begin(); it != lines.end(); ++it) {
        const QString line = (*it).stripWhiteSpace();
        if (line.startsWith(QString::fromLatin1("File system check detail "))) {
            const QString rest = line.mid(24);
            const int colon = rest.find(QString::fromLatin1(": tool="));
            if (colon < 0) {
                continue;
            }
            const QString device = rest.left(colon);
            const int outPos = rest.find(QString::fromLatin1("output="));
            if (outPos < 0) {
                continue;
            }
            const QMap<QString, int>::const_iterator found =
                indexByDevice.find(device);
            if (found != indexByDevice.end()) {
                results[found.data()].detail =
                    rest.mid(outPos + 7).stripWhiteSpace();
            }
            continue;
        }
        if (line.startsWith(QString::fromLatin1("File system check summary:"))) {
            continue;
        }
        if (!line.startsWith(QString::fromLatin1("File system check "))) {
            continue;
        }
        const QString rest = line.mid(18);
        const int colon = rest.find(QString::fromLatin1(": "));
        if (colon < 0) {
            continue;
        }
        const QString device = rest.left(colon);
        const QStringList fields =
            QStringList::split(QChar(' '), rest.mid(colon + 2), true);
        if (fields.count() < 5) {
            continue;
        }
        LegacyFsCheckResult r;
        r.device = device;
        r.fstype = fields[0];
        r.uuid = fields[1].startsWith(QString::fromLatin1("uuid="))
            ? fields[1].mid(5) : QString::null;
        r.mount = fields[2].startsWith(QString::fromLatin1("mount="))
            ? fields[2].mid(6) : QString::null;
        r.tool = fields[3].startsWith(QString::fromLatin1("tool="))
            ? fields[3].mid(5) : QString::null;
        r.result = fields[4].startsWith(QString::fromLatin1("result="))
            ? fields[4].mid(7) : QString::null;
        indexByDevice.insert(r.device, results.count());
        results.append(r);
    }
    return results;
}

// The preferred (safest complete) repair mode is listed first (modern
// MainWindow::filesystemRepairModes parity, mirroring the helper's
// filesystem_mode_field matrix). A mounted non-btrfs filesystem offers only
// the read-only check: its offline repair tool must never run against a
// mounted filesystem.
QStringList filesystemRepairModes(const QString &fstype, bool mounted)
{
    const QString fs = fstype.lower();
    if (fs == QString::fromLatin1("btrfs")) {
        return mounted
            ? QStringList() << QString::fromLatin1("scrub")
                            << QString::fromLatin1("check")
            : QStringList() << QString::fromLatin1("repair")
                            << QString::fromLatin1("rescue")
                            << QString::fromLatin1("scrub")
                            << QString::fromLatin1("check");
    }
    if (fs == QString::fromLatin1("zfs")) {
        return QStringList() << QString::fromLatin1("scrub")
                             << QString::fromLatin1("check");
    }
    if (fs == QString::fromLatin1("f2fs")) {
        return mounted
            ? QStringList()
            : QStringList() << QString::fromLatin1("repair");
    }
    if (mounted) {
        return QStringList() << QString::fromLatin1("check");
    }
    return QStringList() << QString::fromLatin1("repair")
                         << QString::fromLatin1("check");
}

QString LegacyMainWindow::kernelsRemovalPreview() const
{
    const QString section = mapValue(m_diagnosticSections,
                                     QString::fromLatin1("kernel"));
    if (section.stripWhiteSpace().isEmpty()) {
        return QString::null;
    }

    QValueList<KernelRemoval> removable;
    QString running;
    QString latest;
    QString repoLatest;
    QString repoLatestStatus;
    QStringList missingHeaders;
    QStringList orphanedHeaders;
    QStringList orphanedKernels;
    QStringList orphanedBuildSupport;
    QStringList incompleteKernels;

    const QString removablePrefix = QString::fromLatin1("Kernel removable:");
    const QString runningPrefix = QString::fromLatin1("Kernel running:");
    const QString latestPrefix = QString::fromLatin1("Kernel latest:");
    const QString repoLatestPrefix = QString::fromLatin1("Kernel repository latest:");
    const QString repoLatestStatusPrefix =
        QString::fromLatin1("Kernel repository latest status:");
    const QString missingPrefix = QString::fromLatin1("Kernel missing headers:");
    const QString orphanedPrefix = QString::fromLatin1("Kernel orphaned headers:");
    const QString orphanedKernelPrefix = QString::fromLatin1("Kernel orphaned kernel:");
    const QString orphanedBuildSupportPrefix =
        QString::fromLatin1("Kernel orphaned build support:");
    const QString incompletePrefix = QString::fromLatin1("Kernel incomplete:");
    const QStringList lines = QStringList::split(QChar('\n'), section, true);
    for (QStringList::ConstIterator it = lines.begin(); it != lines.end(); ++it) {
        const QString line = (*it).stripWhiteSpace();
        if (line.startsWith(removablePrefix)) {
            const QString rest = line.mid(removablePrefix.length())
                                     .stripWhiteSpace();
            if (rest == QString::fromLatin1("none")) {
                continue;
            }
            KernelRemoval entry;
            const int spacePos = rest.find(QChar(' '));
            entry.version = spacePos >= 0 ? rest.left(spacePos) : rest;
            const int sizePos = rest.find(QString::fromLatin1("size="));
            if (sizePos >= 0) {
                bool ok = false;
                const QString sizeRest = rest.mid(sizePos + 5);
                const int sizeSpace = sizeRest.find(QChar(' '));
                const QString sizeText = sizeSpace >= 0
                    ? sizeRest.left(sizeSpace) : sizeRest;
                const unsigned long long bytes = sizeText.toULongLong(&ok);
                if (ok && bytes > 0) {
                    entry.sizeBytes = bytes;
                }
            }
            const int pkgsPos = rest.find(QString::fromLatin1("pkgs="));
            if (pkgsPos >= 0) {
                const QString pkgs = rest.mid(pkgsPos + 5).stripWhiteSpace();
                if (!pkgs.isEmpty()) {
                    entry.packages = QStringList::split(QChar(','), pkgs, false);
                }
            }
            removable.append(entry);
        } else if (line.startsWith(missingPrefix)) {
            const QString version = line.mid(missingPrefix.length())
                                        .stripWhiteSpace();
            if (!version.isEmpty()) {
                missingHeaders.append(version);
            }
        } else if (line.startsWith(orphanedPrefix)) {
            const QString version = line.mid(orphanedPrefix.length())
                                        .stripWhiteSpace();
            if (!version.isEmpty()) {
                orphanedHeaders.append(version);
            }
        } else if (line.startsWith(orphanedKernelPrefix)) {
            const QString version = line.mid(orphanedKernelPrefix.length())
                                        .stripWhiteSpace();
            if (!version.isEmpty()) {
                orphanedKernels.append(version);
            }
        } else if (line.startsWith(orphanedBuildSupportPrefix)) {
            const QString rest = line.mid(orphanedBuildSupportPrefix.length())
                                     .stripWhiteSpace();
            if (!rest.isEmpty()) {
                orphanedBuildSupport.append(rest);
            }
        } else if (line.startsWith(incompletePrefix)) {
            const QString version = line.mid(incompletePrefix.length())
                                        .stripWhiteSpace();
            if (!version.isEmpty()) {
                incompleteKernels.append(version);
            }
        } else if (line.startsWith(repoLatestPrefix)) {
            const QString value = line.mid(repoLatestPrefix.length())
                                      .stripWhiteSpace();
            if (!value.isEmpty()
                && value != QString::fromLatin1("unavailable")) {
                repoLatest = value;
            }
        } else if (line.startsWith(repoLatestStatusPrefix)) {
            const QString value = line.mid(repoLatestStatusPrefix.length())
                                      .stripWhiteSpace();
            if (!value.isEmpty()) {
                repoLatestStatus = value;
            }
        } else if (line.startsWith(runningPrefix)) {
            running = line.mid(runningPrefix.length()).stripWhiteSpace();
        } else if (line.startsWith(latestPrefix)) {
            latest = line.mid(latestPrefix.length()).stripWhiteSpace();
        }
    }

    QStringList out;
    // Current kernel state (running / installed-latest / repository-latest).
    // A repair target has no running kernel ("none" or empty), so no running
    // line is emitted for it; a real host kernel is always shown.
    if (!running.isEmpty() && running != QString::fromLatin1("none")) {
        out << tr("Running kernel: %1.").arg(running);
    }
    if (!latest.isEmpty()) {
        out << tr("Newest installed kernel: %1.").arg(latest);
    }
    if (!repoLatest.isEmpty() && repoLatest != latest) {
        const QString notInstallable = QString::fromLatin1("not installable ");
        if (repoLatestStatus.startsWith(notInstallable)) {
            out << tr("Repository latest kernel: %1 (newer than the installed newest; %2).")
                       .arg(repoLatest,
                            repoLatestStatus.mid(notInstallable.length())
                                .stripWhiteSpace());
        } else {
            out << tr("Repository latest kernel: %1 (newer than the installed newest).")
                       .arg(repoLatest);
        }
    }
    if (removable.isEmpty()) {
        out << tr("No superseded (older) kernels are installed.");
    } else {
        out << tr("Superseded kernel(s) available for removal: %1.")
                   .arg(removable.count());
        int number = 1;
        for (QValueList<KernelRemoval>::ConstIterator it = removable.begin();
             it != removable.end(); ++it) {
            for (QStringList::ConstIterator pkg = (*it).packages.begin();
                 pkg != (*it).packages.end(); ++pkg) {
                out << QString::fromLatin1("%1. %2").arg(number++).arg(*pkg);
            }
        }
        unsigned long long totalBytes = 0;
        for (QValueList<KernelRemoval>::ConstIterator it = removable.begin();
             it != removable.end(); ++it) {
            totalBytes += (*it).sizeBytes;
        }
        if (totalBytes > 0) {
            out << QString::null;
            out << tr("Approximately %1 MB would be restored.")
                       .arg(static_cast<double>(totalBytes) / (1024.0 * 1024.0),
                            0, 'f', 1);
        }
    }
    if (!missingHeaders.isEmpty()) {
        out << tr("Missing headers (latest kernel has no matching headers): %1.")
                   .arg(missingHeaders.join(QString::fromLatin1(", ")));
    }
    if (!orphanedHeaders.isEmpty()) {
        out << tr("Orphaned headers (no matching kernel): %1.")
                   .arg(orphanedHeaders.join(QString::fromLatin1(", ")));
    }
    if (!orphanedKernels.isEmpty()) {
        out << tr("Orphaned kernel components (no owning package): %1.")
                   .arg(orphanedKernels.join(QString::fromLatin1(", ")));
    }
    if (!orphanedBuildSupport.isEmpty()) {
        out << tr("Orphaned kernel build support (kernel gone): %1.")
                   .arg(orphanedBuildSupport.join(QString::fromLatin1("; ")));
    }
    if (!incompleteKernels.isEmpty()) {
        out << tr("Incomplete kernels (installed package without its on-disk files): %1.")
                   .arg(incompleteKernels.join(QString::fromLatin1(", ")));
    }
    return out.join(QString::fromLatin1("\n"));
}

// Selected tool pane: modern title, per-tool button text, description and
// plan status. The "Unavailable: <reason>" suffix is appended exactly like the
// modern page; the tools list's "Full Repair" column mirrors the Settings plan
// state and the cached capability line.
void LegacyMainWindow::updateToolDetails()
{
    if (!m_toolTitle || !m_toolDescription || !m_toolPlanStatus || !m_toolRunButton) {
        return;
    }

    // 1. Full Repair column for every tool row.
    if (m_toolList) {
        for (QListViewItem *row = m_toolList->firstChild(); row;
             row = row->nextSibling()) {
            // The row's tool resolves by its title, so a sorted list still
            // mirrors the correct spec (the curated order is not assumed).
            const int rowIndex = toolIndexForTitle(row->text(0));
            if (rowIndex < 0) {
                continue;
            }
            const ToolSpec &spec = toolSpecs[rowIndex];
            QString status;
            if (spec.planStage && *spec.planStage) {
                const int planIndex = planIndexForStage(spec.planStage);
                status = (planIndex >= 0 && planStageSelected(planIndex))
                    ? tr("Enabled in Settings")
                    : tr("Disabled in Settings - enable it to include this stage");
            } else {
                status = tr(spec.planBase);
            }
            QString reason;
            if (!toolRunReady(rowIndex, &reason)) {
                // Round 52: the display tool's capability-derived reason may
                // carry a second line (the Install guidance); the list cell is
                // single-line, so flatten the line break there. The Selected
                // tool pane keeps the full two-line text.
                status = tr("Unavailable: %1")
                             .arg(reason)
                             .replace(QChar('\n'), QChar(' '));
            }
            row->setText(1, status);
        }
    }

    // 2. Selected tool pane.
    const int index = selectedToolIndex();
    if (index < 0) {
        m_toolTitle->setText(tr("Select a repair tool"));
        m_toolDescription->setText(tr("Select a tool to review its repair action."));
        m_toolPlanStatus->setText(QString::null);
        updateButtonText(m_toolRunButton, tr("Run Tool"));
        m_toolRunButton->setEnabled(false);
        return;
    }
    const ToolSpec &spec = toolSpecs[index];
    m_toolTitle->setText(tr(spec.title));
    m_toolDescription->setText(tr(spec.description));
    updateButtonText(m_toolRunButton, tr(spec.button));

    QString planStatus;
    if (spec.planStage && *spec.planStage) {
        const int planIndex = planIndexForStage(spec.planStage);
        planStatus = (planIndex >= 0 && planStageSelected(planIndex))
            ? tr("Enabled in Settings")
            : tr("Disabled in Settings - enable it to include this stage");
    } else {
        planStatus = tr(spec.planBase);
    }
    // P2 (modern kernelsRemovalPreview parity): when the kernels tool is
    // selected and the cached `kernel` diagnostic section is present, append
    // the parsed kernel-inventory summary (superseded/orphaned/incomplete/
    // missing-headers) to the plan-status pane. Fail-closed: no cached
    // evidence leaves the static description untouched.
    if (QString::fromLatin1(spec.key) == QString::fromLatin1("kernels")) {
        const QString preview = kernelsRemovalPreview();
        if (!preview.isEmpty()) {
            planStatus += QString::fromLatin1("\n\n") + preview;
        }
    }
    QString reason;
    const bool ready = toolRunReady(index, &reason);
    if (!ready) {
        // Display-only tools have an empty plan text: the unavailable reason
        // becomes the whole status, with no cosmetic leading blank line.
        if (planStatus.isEmpty()) {
            planStatus = tr("Unavailable: %1").arg(reason);
        } else {
            planStatus += tr("\nUnavailable: %1").arg(reason);
        }
    }
    m_toolPlanStatus->setText(planStatus);
    m_toolRunButton->setEnabled(ready);
    // Round 32 (Etch #4): the enabled hint is the concise per-function
    // tooltip from the spec (the modern buttonTooltip chain wording); an
    // unavailable tool shows its gating reason instead.
    QToolTip::add(m_toolRunButton, ready ? tr(spec.tooltip) : reason);
}

void LegacyMainWindow::runSelectedTool()
{
    const int index = selectedToolIndex();
    if (index < 0) {
        return;
    }
    const ToolSpec &spec = toolSpecs[index];
    QString reason;
    if (!toolRunReady(index, &reason)) {
        QMessageBox::information(this, tr("Repair tool unavailable"),
                                 reason, QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    // The file system repair tool runs the inspect-then-repair flow inside the
    // single repair result popup (the read-only check, the inline per-device
    // decision and each confirmed repair through the helper's fs-repair verb).
    // It carries its own per-device confirmation, so it never reaches the
    // generic single-stage command path below.
    if (QString::fromLatin1(spec.key) == QString::fromLatin1("filesystem")) {
        runFilesystemRepairFlow();
        return;
    }
    if (spec.write && spec.confirm) {
        const bool answer = confirmWrapped(
            this, tr("Confirm repair"),
            tr(spec.confirm),
            /*cancelInsteadOfNo=*/false, QString::null, QString::null,
            kHiddenInputMaximumWidth, /*noIsDefault=*/true);
        if (!answer) {
            return;
        }
    }
    const bool host = hostScope();
    const QString stage = QString::fromLatin1(spec.stage);
    QStringList args;
    if (stage == QString::fromLatin1("validate")) {
        args << (host ? QString::fromLatin1("host-validate") : QString::fromLatin1("validate"));
    } else if (stage == QString::fromLatin1("fs-inspect")) {
        args << (host ? QString::fromLatin1("host-fs-inspect") : QString::fromLatin1("fs-inspect"));
    } else {
        args << (host ? QString::fromLatin1("host-repair") : QString::fromLatin1("repair"));
    }
    args << selectedDisk() << selectedRoot();
    if (stage != QString::fromLatin1("validate")
        && stage != QString::fromLatin1("fs-inspect")) {
        args << stage;
    }
    // Serialize the cached baseline sections the helper consumes so this
    // repair reuses the cached diagnostic evidence instead of re-collecting
    // it (modern parity): initramfs and the interactive kernels menu consume
    // `kernel`, the GPU repair consumes `gpu` + `kernel`, the display repair
    // consumes `display`, the GRUB repair consumes `grub`, and the boot-stack
    // reconciliation consumes boot-evidence/kernel/uki/grub. Sections without
    // a helper-side consumer are gated by toolRunReady above only and stay
    // verified fresh by the helper's own preflights. The gate above guarantees
    // every required section is cached, so nothing is silently omitted.
    const QString toolKey = QString::fromLatin1(spec.key);
    const QStringList required = requiredDiagnosticSectionsForTool(toolKey);
    for (QStringList::ConstIterator it = required.begin(); it != required.end(); ++it) {
        if (!repairToolSerializesBaselineSection(*it)) {
            continue;
        }
        const QString cached = mapValue(m_diagnosticSections, *it);
        if (!cached.stripWhiteSpace().isEmpty()) {
            args << QString::fromLatin1("--evidence") << *it
                 << legacyBase64Encode(cached);
        }
    }
    // The GPU and kernel repairs drive the helper's interactive menus over the
    // PROMPT/ANSWER channel; startCommand reserves the mode-600 menu-answer
    // file and hands the helper the interactive-session environment for these
    // two tools only.
    const bool interactive = toolKey == QString::fromLatin1("gpu")
        || toolKey == QString::fromLatin1("kernels");
    const bool started = startCommand(args, false, tr(spec.title),
                                       false, false, false, false, QString::null,
                                       stage, false, interactive);
    if (started && !m_smokeMode) {
        showRepairResultDialog(tr(spec.title));
    }
}

// The standalone "Check File Systems" flow (modern runFilesystemRepairFlow
// parity): the read-only fs-inspect and the per-device repair all run inside
// the single repair result popup, with the per-device decision surfaced inline
// through the popup's prompt input line (no separate modal). Only a device the
// check reported with `issues` is ever offered for repair, and the helper
// repeats its scope/mount/tool preflights on every repair (never an offline
// fsck against a still-mounted filesystem). Returns true when at least one
// repair completed successfully.
bool LegacyMainWindow::runFilesystemRepairFlow()
{
    if (m_running || m_filesystemRepairFlowActive || m_repairPlanActive) {
        return false;
    }
    // Fail closed: the cached capability line must name the filesystem tool as
    // available before any fs-inspect/fs-repair command runs.
    std::string capReason;
    if (!m_model.isAvailable("filesystem", toStd(identity()), &capReason)) {
        QMessageBox::warning(this, tr("File system repair unavailable"),
                             localizedCapabilityReason(fromStd(capReason)));
        return false;
    }
    if (!beginRepairPlan(/*fullRepair=*/false, QStringList(),
                         /*runFilesystem=*/true)) {
        return false;
    }
    if (!m_smokeMode) {
        showRepairResultDialog(tr("File system repair"));
    }
    return m_repairPlanAnyRepairCompleted;
}

// Captures the scope and starts the first command of the single-popup repair
// plan (the read-only fs-inspect, or the remaining Full Repair stages directly
// when the plan has no file system stage). The remaining steps (inline
// decision, per-device repair, remaining Full Repair stages) are driven by
// advanceRepairPlan().
bool LegacyMainWindow::beginRepairPlan(bool fullRepair,
                                       const QStringList &remainingStages,
                                       bool runFilesystem)
{
    if (m_running || m_filesystemRepairFlowActive || m_repairPlanActive) {
        return false;
    }
    const bool host = hostScope();
    m_repairPlanActive = true;
    m_repairPlanFullRepair = fullRepair;
    m_repairPlanHost = host;
    m_repairPlanDisk = selectedDisk();
    m_repairPlanRoot = selectedRoot();
    m_repairPlanScopeLabel = host ? tr("running host")
                                  : tr("selected repair system");
    m_repairPlanRemainingStages = remainingStages;
    m_repairPlanCandidates.clear();
    m_repairPlanCandidateModes.clear();
    m_repairPlanChosenModes.clear();
    m_repairPlanRepairIndex = -1;
    m_repairPlanAnyRepairCompleted = false;
    m_repairPlanInlineQuestion = false;
    m_filesystemRepairFlowActive = runFilesystem;
    // Round 30 (modern accumulatePlanInvalidation parity): the plan starts
    // with an empty accumulated invalidation; the per-command accumulation in
    // helperFinished fills it and finishRepairPlan applies it once.
    m_repairPlanInvalidatedSections.clear();
    m_repairPlanRequiresFullInvalidation = false;
    m_repairPlanModified = false;

    if (!runFilesystem) {
        // No file system pre-stage: run the remaining stages directly.
        if (!startRemainingRepairStages()) {
            m_repairPlanPhase = RepairPlanDone;
            m_repairPlanActive = false;
            m_filesystemRepairFlowActive = false;
            return false;
        }
        return true;
    }

    m_repairPlanPhase = RepairPlanFsInspect;
    QStringList args;
    args << (host ? QString::fromLatin1("host-fs-inspect")
                  : QString::fromLatin1("fs-inspect"))
         << m_repairPlanDisk << m_repairPlanRoot;
    const bool started = startCommand(
        args, false,
        fullRepair ? tr("Check File Systems (Full Repair pre-stage)")
                   : tr("Check File Systems (manual)"),
        false, false, false, false, QString::null,
        QString::fromLatin1("fs-inspect"));
    if (!started) {
        // startCommand already emitted finished(false,-1), which scheduled
        // advanceRepairPlan(); clearing the flag makes that a no-op and the
        // caller reports the failure.
        m_filesystemRepairFlowActive = false;
        m_repairPlanActive = false;
        m_repairPlanPhase = RepairPlanDone;
        return false;
    }
    return true;
}

// The single-popup plan step driver, re-entered after every completed plan
// command and after the inline per-device decision. It never opens a new
// window: every step streams into the one repair result popup.
void LegacyMainWindow::advanceRepairPlan()
{
    if (!m_repairPlanActive || m_repairPlanPhase == RepairPlanDone) {
        return;
    }
    switch (m_repairPlanPhase) {
    case RepairPlanFsInspect: {
        // Step 2: parse the read-only per-device results and decide the repair
        // candidates (only `issues` devices with a safe mode qualify).
        const bool inspectOk = m_lastCommandOk;
        const QString inspectOutput = m_transcript;
        if (!inspectOk) {
            appendLog(tr(
                "The read-only file system check failed; review the helper "
                "output in Logs."));
            m_repairPlanPhase = RepairPlanDone;
            finishRepairPlan();
            return;
        }
        const QValueList<LegacyFsCheckResult> results =
            parseFilesystemCheckOutput(inspectOutput);
        QValueList<LegacyFsCheckResult> repairCandidates;
        QValueList<QStringList> candidateModes;
        int issueCount = 0;
        int skippedCount = 0;
        int unavailableCount = 0;
        int noSafeModeCount = 0;
        for (QValueList<LegacyFsCheckResult>::ConstIterator it = results.begin();
             it != results.end(); ++it) {
            const LegacyFsCheckResult &result = *it;
            if (result.result == QString::fromLatin1("skipped")) {
                ++skippedCount;
                continue;
            }
            if (result.result == QString::fromLatin1("unsupported")
                || result.result == QString::fromLatin1("tool-missing")) {
                ++unavailableCount;
                continue;
            }
            if (result.result != QString::fromLatin1("issues")) {
                continue;
            }
            ++issueCount;
            const bool mounted = !result.mount.isEmpty()
                && result.mount != QString::fromLatin1("unmounted");
            const QStringList modes =
                filesystemRepairModes(result.fstype, mounted);
            bool hasRepairMode = false;
            for (QStringList::ConstIterator m = modes.begin();
                 m != modes.end(); ++m) {
                if (*m != QString::fromLatin1("check")) {
                    hasRepairMode = true;
                    break;
                }
            }
            if (!hasRepairMode) {
                ++noSafeModeCount;
                continue;
            }
            repairCandidates.append(result);
            candidateModes.append(modes);
        }

        if (issueCount == 0) {
            QStringList limitations;
            if (skippedCount > 0) {
                limitations.append(tr(
                    "%1 mounted filesystem(s) were skipped because their check "
                    "tools are offline-only").arg(skippedCount));
            }
            if (unavailableCount > 0) {
                limitations.append(tr(
                    "%1 filesystem(s) are unsupported or have no installed "
                    "check tool").arg(unavailableCount));
            }
            const QString message = limitations.isEmpty()
                ? tr("No file system errors were detected on the %1's root, "
                     "/boot or /home filesystems. No repair was run.")
                      .arg(m_repairPlanScopeLabel)
                : tr("No repairable file system errors were detected on the "
                     "%1's root, /boot or /home filesystems, but %2. No "
                     "repair was run.").arg(m_repairPlanScopeLabel)
                         .arg(limitations.join(QString::fromLatin1("; ")));
            appendLog(message);
            if (m_resultView) {
                m_resultView->append(message);
            }
            m_repairPlanPhase = RepairPlanDone;
            continueOrFinishRepairPlan();
            return;
        }

        if (repairCandidates.isEmpty()) {
            const QString message = tr(
                "The read-only check found issues on %1 mounted filesystem(s) "
                "that have no safe repair mode from this environment. Unmount "
                "the filesystem and check again, or repair it from a live "
                "system that is not using it.").arg(noSafeModeCount);
            appendLog(message);
            if (m_resultView) {
                m_resultView->append(message);
            }
            // Issues exist but no safe repair is available: fail closed and do
            // not proceed into the remaining plan stages.
            m_repairPlanPhase = RepairPlanDone;
            finishRepairPlan();
            return;
        }

        m_repairPlanCandidates = repairCandidates;
        m_repairPlanCandidateModes = candidateModes;
        poseRepairPlanInlineDecision();
        return;
    }
    case RepairPlanFsRepair: {
        // A per-device repair finished; record its outcome and start the next
        // selected device (or move on to the remaining plan stages).
        if (m_lastCommandOk) {
            m_repairPlanAnyRepairCompleted = true;
        }
        if (startNextRepairPlanDevice()) {
            return;
        }
        // Every selected device has been attempted. If none completed, fail
        // closed: the remaining plan stages never run after a file system
        // repair that failed on every selected device.
        if (!m_repairPlanAnyRepairCompleted) {
            m_repairPlanPhase = RepairPlanDone;
            finishRepairPlan();
            return;
        }
        continueOrFinishRepairPlan();
        return;
    }
    case RepairPlanRemaining: {
        m_repairPlanPhase = RepairPlanDone;
        finishRepairPlan();
        return;
    }
    default:
        return;
    }
}

// Presents the numbered per-device repair decision through the popup's prompt
// input line (the same surface the helper's interactive menus use). The user
// types the 1-based device numbers to repair, or 0 to skip.
void LegacyMainWindow::poseRepairPlanInlineDecision()
{
    m_repairPlanPhase = RepairPlanFsDecision;
    m_repairPlanInlineQuestion = true;

    QStringList lines;
    lines << tr("File system errors were detected. Choose the devices to "
                "repair (the safest mode is used for each):");
    const int count = static_cast<int>(m_repairPlanCandidates.size());
    for (int i = 0; i < count; ++i) {
        const LegacyFsCheckResult &candidate = m_repairPlanCandidates[i];
        const QStringList modes =
            i < static_cast<int>(m_repairPlanCandidateModes.size())
                ? m_repairPlanCandidateModes[i] : QStringList();
        const QString mode = modes.isEmpty()
            ? QString::fromLatin1("repair") : modes.first();
        lines << QString::fromLatin1("  %1. %2 (%3) - %4")
                     .arg(i + 1)
                     .arg(candidate.device)
                     .arg(candidate.fstype)
                     .arg(mode);
        if (!candidate.detail.isEmpty()) {
            lines << QString::fromLatin1("     %1").arg(candidate.detail);
        }
    }
    lines << QString::fromLatin1("  0. %1")
                 .arg(tr("Skip file system repair and continue"));
    lines << QString::null;
    lines << tr("Enter the device numbers to repair (space- or comma-separated, "
                "for example \"1 3\"), or 0 to skip.");
    lines << tr("Offline repair modes refuse mounted filesystems; the running "
                "host root is never repaired offline.");

    const QString text = lines.join(QString::fromLatin1("\n"));
    appendLog(text);
    if (m_resultView) {
        m_resultView->append(text);
    }
    if (m_promptHint) {
        m_promptHint->setText(tr("Numbers of the devices to repair (0 = skip)"));
        m_promptHint->show();
    }
    if (m_promptInput) {
        m_promptInput->setEnabled(true);
        m_promptInput->clear();
        m_promptInput->setFocus();
    }
    if (m_promptSend) {
        m_promptSend->setEnabled(true);
    }
}

// Applies the typed inline answer (a numbered device list). Declining (0 or an
// empty answer) stops a Full Repair plan (the remaining stages never run after
// a declined file system repair) and just ends a standalone flow.
void LegacyMainWindow::processRepairPlanInlineAnswer(const QString &answer)
{
    m_repairPlanInlineQuestion = false;

    const int count = static_cast<int>(m_repairPlanCandidates.size());
    m_repairPlanChosenModes.clear();
    for (int i = 0; i < count; ++i) {
        m_repairPlanChosenModes.append(QString::null);
    }
    // Parse decimal numbers out of the free text (space/comma separated).
    bool any = false;
    const int len = static_cast<int>(answer.length());
    int idx = 0;
    while (idx < len) {
        if (answer[idx].isDigit()) {
            int value = 0;
            while (idx < len && answer[idx].isDigit()) {
                value = value * 10 + answer[idx].digitValue();
                ++idx;
            }
            if (value >= 1 && value <= count) {
                const QStringList modes =
                    value - 1 < static_cast<int>(m_repairPlanCandidateModes.size())
                        ? m_repairPlanCandidateModes[value - 1] : QStringList();
                m_repairPlanChosenModes[value - 1] =
                    modes.isEmpty() ? QString::fromLatin1("repair")
                                    : modes.first();
                any = true;
            }
            continue;
        }
        ++idx;
    }

    if (!any) {
        appendLog(tr(
            "File system repair cancelled before any repair was run."));
        if (m_resultView) {
            m_resultView->append(tr(
                "No devices were selected; the file system repair stage was "
                "skipped."));
        }
        m_repairPlanPhase = RepairPlanDone;
        finishRepairPlan();
        return;
    }

    m_repairPlanRepairIndex = -1;
    m_repairPlanAnyRepairCompleted = false;
    if (!startNextRepairPlanDevice()) {
        // Cannot happen once at least one device is chosen, but fail closed.
        m_repairPlanPhase = RepairPlanDone;
        finishRepairPlan();
    }
}

// Starts the next chosen per-device fs-repair command inside the single popup.
// Returns false when no chosen device remains (the caller then moves on to the
// remaining plan stages or finalizes the plan).
bool LegacyMainWindow::startNextRepairPlanDevice()
{
    const int count = static_cast<int>(m_repairPlanCandidates.size());
    int index = m_repairPlanRepairIndex + 1;
    while (index < count && m_repairPlanChosenModes[index].isEmpty()) {
        ++index;
    }
    if (index >= count) {
        m_repairPlanRepairIndex = -1;
        return false;
    }
    m_repairPlanRepairIndex = index;
    m_repairPlanPhase = RepairPlanFsRepair;
    const LegacyFsCheckResult &candidate = m_repairPlanCandidates[index];
    const QString mode = m_repairPlanChosenModes[index];
    QStringList args;
    args << (m_repairPlanHost ? QString::fromLatin1("host-fs-repair")
                              : QString::fromLatin1("fs-repair"))
         << m_repairPlanDisk << m_repairPlanRoot << candidate.device << mode;
    // A start failure emits finished(false,-1) synchronously, which schedules
    // advanceRepairPlan(); the driver then records the failure and moves on.
    startCommand(args, false, tr("File system repair (%1)").arg(mode),
                 false, false, false, false, QString::null,
                 QString::fromLatin1("filesystem"));
    return true;
}

// Continues to the remaining Full Repair stages (one helper request) or
// finalizes the plan when there is nothing left to run.
void LegacyMainWindow::continueOrFinishRepairPlan()
{
    if (startRemainingRepairStages()) {
        return;
    }
    m_repairPlanPhase = RepairPlanDone;
    finishRepairPlan();
}

// Starts the remaining Full Repair stages as one helper request inside the
// single popup. Returns false when there are no remaining stages (the plan
// ends here) or the command could not start (the caller finalizes the plan).
bool LegacyMainWindow::startRemainingRepairStages()
{
    if (!m_repairPlanFullRepair || m_repairPlanRemainingStages.isEmpty()) {
        return false;
    }
    m_repairPlanPhase = RepairPlanRemaining;
    QStringList args;
    args << (m_repairPlanHost ? QString::fromLatin1("host-repair")
                              : QString::fromLatin1("repair"))
         << m_repairPlanDisk << m_repairPlanRoot;
    args += m_repairPlanRemainingStages;
    // Serialize the boot-stack component baselines (modern runFullRepair
    // parity); the helper re-verifies the per-stage preflights regardless.
    if (m_repairPlanRemainingStages.contains(QString::fromLatin1("boot-stack"))) {
        const QStringList required =
            requiredDiagnosticSectionsForTool(QString::fromLatin1("bootstack"));
        for (QStringList::ConstIterator it = required.begin();
             it != required.end(); ++it) {
            const QString cached = mapValue(m_diagnosticSections, *it);
            if (!cached.stripWhiteSpace().isEmpty()) {
                args << QString::fromLatin1("--evidence") << *it
                     << legacyBase64Encode(cached);
            }
        }
    }
    // Round 54 (E2, modern parity): the plan command runs with the
    // interactive session armed so the helper's per-package-stage
    // "CONFIRM PACKAGE CHANGE" prompts surface through the result popup's
    // prompt line (a declined answer skips the stage and the plan continues,
    // exactly like the modern in-run confirmations).
    return startCommand(args, false, tr("Full Repair"),
                        false, false, false, false, QString::null,
                        m_repairPlanRemainingStages.join(QString::fromLatin1(" ")),
                        false, /*interactive=*/true);
}

// Finalizes the single-popup plan on every path (success, failure, decline):
// enables the popup's Close, performs the one invalidation + regeneration for
// a Full Repair plan (or the stale-based refresh for a standalone flow), and
// clears the plan state. Mirrors helperFinished's Full Repair path.
void LegacyMainWindow::finishRepairPlan()
{
    const bool fullRepair = m_repairPlanFullRepair;
    // Round 16 (ITEM 4e): an aborted plan reports the abort status (modern
    // parity), never "plan finished".
    const bool planCancelled = m_repairPlanCancelled;
    m_repairPlanPhase = RepairPlanDone;
    m_repairPlanActive = false;
    m_repairPlanInlineQuestion = false;
    m_repairPlanCancelled = false;
    m_filesystemRepairFlowActive = false;

    // The popup's Close stays disabled until this point.
    if (m_resultStatus && m_resultCloseButton) {
        if (planCancelled) {
            m_resultStatus->setText(fullRepair
                ? tr("Full Repair plan stopped at your request. Review the "
                     "output above, then close this window.")
                : tr("File system repair stopped at your request. Review the "
                     "output above, then close this window."));
        } else {
            m_resultStatus->setText(fullRepair
                ? tr("Full Repair plan finished. Review the output above, then "
                     "close this window.")
                : tr("File system repair finished. Review the output above, then "
                     "close this window."));
        }
        m_resultCloseButton->setEnabled(true);
        if (m_resultDialog) {
            m_resultDialog->setCloseAllowed(true);
        }
    }

    // Round 30 (modern parity): the plan applies its accumulated mapped union
    // ONCE here — a genuine stage failure or a fail-safe row forces the
    // complete set, a targeted set removes exactly the mapped sections (and
    // the capability model only when the set names the capability
    // pseudo-entry), and a plan that proved no change invalidates nothing.
    // The full-Full-Repair reset keeps the historical wording.
    if (m_repairPlanRequiresFullInvalidation) {
        m_model.reset();
        m_diagnosticSections.clear();
        m_fullDiagnosticReport = QString::null;
        m_lastDiagnosticKey = QString::null;
        m_lastDiagnosticIdentity = QString::null;
        refreshDiagnosticResultsView();
        refreshCapabilities();
        appendLog(planCancelled
            ? (fullRepair
                ? QString::fromLatin1(
                      "Full Repair was stopped at your request; cached diagnostics "
                      "were invalidated for the current scope.")
                : QString::fromLatin1(
                      "File system repair was stopped at your request; cached "
                      "diagnostics were invalidated for the current scope."))
            : (fullRepair
                ? QString::fromLatin1(
                      "Full Repair completed; cached diagnostics were invalidated "
                      "for the current scope.")
                : QString::fromLatin1(
                      "File system repair completed; cached diagnostics were "
                      "invalidated for the current scope.")));
        maybeAutoRefreshDiagnostics(fullRepair
            ? QString::fromLatin1("Full Repair")
            : QString::fromLatin1("file system repair"));
    } else if (!m_repairPlanInvalidatedSections.isEmpty()) {
        QStringList names;
        bool clearsCapabilities = false;
        for (QValueList<QString>::ConstIterator it =
                 m_repairPlanInvalidatedSections.begin();
             it != m_repairPlanInvalidatedSections.end(); ++it) {
            if (*it == QString::fromLatin1("capabilities")) {
                clearsCapabilities = true;
                continue;
            }
            m_diagnosticSections.remove((*it).lower());
            m_fullDiagnosticReport = removeEmbeddedDiagnosticSection(
                m_fullDiagnosticReport, *it);
            names.append(*it);
        }
        if (clearsCapabilities) {
            m_model.clearRepairCapabilities();
        }
        refreshDiagnosticResultsView();
        refreshCapabilities();
        appendLog(tr(
            "The repair plan modified the selected system; the mapped cached "
            "diagnostic sections were invalidated: %1.")
            .arg(names.join(QString::fromLatin1(", "))));
        maybeAutoRefreshDiagnostics(fullRepair
            ? QString::fromLatin1("Full Repair")
            : QString::fromLatin1("file system repair"));
    } else if (m_repairPlanModified) {
        appendLog(tr(
            "The repair plan made no changes; cached diagnostics remain "
            "valid."));
    }

    m_repairPlanRemainingStages.clear();
    m_repairPlanCandidates.clear();
    m_repairPlanCandidateModes.clear();
    m_repairPlanChosenModes.clear();
    m_repairPlanRepairIndex = -1;
    m_repairPlanDisk = QString::null;
    m_repairPlanRoot = QString::null;
    m_repairPlanScopeLabel = QString::null;
    m_repairPlanInvalidatedSections.clear();
    m_repairPlanRequiresFullInvalidation = false;
    m_repairPlanModified = false;
}

void LegacyMainWindow::configurePlan()
{
    if (m_tabs) {
        m_tabs->setCurrentPage(6);
    }
}

void LegacyMainWindow::planCheckboxChanged()
{
    // Nothing is recorded before loadLegacySettings() has filled the register;
    // the register is never refilled with defaults from this handler.
    if (!m_settingsLoaded
        || m_planPreferences.size() != static_cast<std::size_t>(planSpecCount)) {
        return;
    }
    // Modern preference semantics: record the toggled checkbox directly
    // (sender()) so the user's explicit change is never dropped, then refresh
    // the other ENABLED rows.  Rows forced off by the availability pass stay
    // untouched so they never clobber the saved preference.
    const QObject *toggled = sender();
    for (int i = 0; i < planSpecCount
                    && i < static_cast<int>(m_planChecks.size()); ++i) {
        if (!m_planChecks[i]) {
            continue;
        }
        if (m_planChecks[i] == toggled || m_planChecks[i]->isEnabled()) {
            m_planPreferences[i] = m_planChecks[i]->isChecked();
        }
    }
    saveLegacySettings();
    updateActionStates();
}

// Modern availability semantics for the Settings plan checkboxes
// Re-applies the device-discovery filter checkboxes from their register
// (m_showNonLinux/m_showRemovable/m_showEncrypted), the plan path's
// updatePlanChecks() mirror.  blockSignals keeps the re-apply from re-recording
// a toggle; the register is the single source of truth for the filters.
void LegacyMainWindow::updateDeviceFilterChecks()
{
    if (m_showNonLinuxCheck) {
        m_showNonLinuxCheck->blockSignals(true);
        m_showNonLinuxCheck->setChecked(m_showNonLinux);
        m_showNonLinuxCheck->blockSignals(false);
    }
    if (m_showRemovableCheck) {
        m_showRemovableCheck->blockSignals(true);
        m_showRemovableCheck->setChecked(m_showRemovable);
        m_showRemovableCheck->blockSignals(false);
    }
    if (m_showEncryptedCheck) {
        m_showEncryptedCheck->blockSignals(true);
        m_showEncryptedCheck->setChecked(m_showEncrypted);
        m_showEncryptedCheck->blockSignals(false);
    }
}

// (MainWindow::updateRepairScopeControls() parity): every row is shown. The
// plan checkboxes are plain user settings: an available stage is checkable
// with its saved preference (never clobbered by evidence warmth), while an
// explicitly unavailable stage (capability `unavailable|<reason>`, a frontend
// gap, or a host-only stage outside Host Maintenance) is disabled AND
// unchecked with the exact reason. The pass blocks signals so the presentation
// never re-records the preference; it runs from updateActionStates() whenever
// the scope or the cached diagnostics change.
void LegacyMainWindow::updatePlanChecks()
{
    // The presentation reads the register only after loadLegacySettings() has
    // filled it; it never refills the register with defaults.
    if (!m_settingsLoaded
        || m_planPreferences.size() != static_cast<std::size_t>(planSpecCount)) {
        return;
    }
    for (int i = 0; i < planSpecCount
                    && i < static_cast<int>(m_planChecks.size()); ++i) {
        if (!m_planChecks[i]) {
            continue;
        }
        QString reason;
        const PlanStageState state = planStageState(i, &reason);
        bool enabled;
        bool checked;
        QString tip;
        if (state == PlanStageAvailable) {
            enabled = true;
            checked = m_planPreferences[i];
            // Round 52: a host-only stage checked at target scope is recorded
            // but runs only under Host Maintenance; the tip names that instead
            // of promising a target-scope run.
            tip = (planSpecs[i].hostOnly && !hostScope())
                ? tr(
                      "Include the %1 stage in the Full Repair plan. On this "
                      "legacy frontend the stage runs under Host Maintenance "
                      "only.")
                      .arg(tr(planSpecs[i].title))
                : tr(
                      "Include the %1 stage in the Full Repair plan. The stage runs "
                      "in the plan order shown on the Repair tab.")
                      .arg(tr(planSpecs[i].title));
        } else {
            // PlanStageUnavailable is the only disabled state: the stage stays
            // shown, disabled and unchecked with the exact reason. The saved
            // preference register is left untouched, so the selection is never
            // silently unchecked by the presentation pass.
            enabled = false;
            checked = false;
            tip = tr("Unavailable: %1").arg(reason);
        }
        m_planChecks[i]->blockSignals(true);
        if (enabled) {
            m_planChecks[i]->setEnabled(true);
            m_planChecks[i]->setChecked(checked);
        } else {
            m_planChecks[i]->setChecked(false);
            m_planChecks[i]->setEnabled(false);
        }
        m_planChecks[i]->blockSignals(false);
        if (i >= static_cast<int>(m_planCheckTips.size())) {
            m_planCheckTips.resize(i + 1);
        }
        if (m_planCheckTips[i] != tip) {
            QToolTip::add(m_planChecks[i], tip);
            m_planCheckTips[i] = tip;
        }
    }
}

bool LegacyMainWindow::planStageSelected(int planIndex) const
{
    if (planIndex < 0 || planIndex >= static_cast<int>(m_planChecks.size())) {
        return false;
    }
    return m_planChecks[planIndex] && m_planChecks[planIndex]->isChecked();
}


// Fail-closed availability for one plan stage: a frontend gap, or a cached
// `unavailable|<reason>` capability line (including a stale cache), and —
// round 52, kept OUT of the checkbox presentation — a host-only stage outside
// Host Maintenance. The hostOnly gate lives here (the RUN side), not in
// planStageState() (the PRESENTATION side), so the Settings checkbox records
// the user's preference at target scope while the target-scope plan still
// excludes the host-scope stage with the exact scope hint (a checked-but-
// excluded stage is visible in the plan list's exclusion reason, and the
// helper never receives a stage it would refuse). Absent evidence keeps the
// stage available to the plan presentation (checkable), while the run gate
// still fails closed on the missing diagnostics.
bool LegacyMainWindow::planStageAvailable(int planIndex, QString *reason) const
{
    if (planStageState(planIndex, reason) == PlanStageUnavailable) {
        return false;
    }
    // Round 52/54 (user requests): the run-side scope gate for host-only
    // stages.  Round 54 enabled the legacy display-manager repair for the
    // target scope (hostOnly=false), so the gate no longer fires for it; it
    // remains for any future genuinely host-only stage with the exact remedy.
    if (planIndex >= 0 && planIndex < planSpecCount
        && planSpecs[planIndex].hostOnly && !hostScope()) {
        if (reason) {
            *reason = tr(
                "Restore Graphical Login is a host-scope stage on this legacy "
                "frontend; enter Host Maintenance to run it.");
        }
        return false;
    }
    return true;
}

// Presentation state for one plan stage. The plan checkboxes are plain user
// settings that stay checkable unless the stage is EXPLICITLY unavailable: a
// frontend gap, or a cached `unavailable|<reason>` capability line in the
// current scope (including a stale cache). A capability line that is merely
// absent (no completed diagnostics yet) keeps the stage checkable with its
// saved preference, so the presentation never depends on diagnostic-cache
// warmth. Round 52 (user request): a host-only stage (the legacy
// display-manager repair) is NO LONGER greyed out by the presentation for the
// offline target scope — the checkbox records the user's preference there too
// (the "Disabled in Settings - enable it to include this stage" note must
// only point at Settings when Settings actually allows enabling). The
// fail-closed run gate (toolRunReady()/planStageAvailable()/planRunReady()
// and the helper runtime preflights) still enforces the host-scope evidence:
// planStageAvailable() keeps its own hostOnly gate so a target-scope plan
// never launches a repair the helper refuses.
LegacyMainWindow::PlanStageState LegacyMainWindow::planStageState(
    int planIndex, QString *reason) const
{
    if (planIndex < 0 || planIndex >= planSpecCount) {
        if (reason) {
            *reason = tr("Unknown Full Repair stage.");
        }
        return PlanStageUnavailable;
    }
    const PlanSpec &spec = planSpecs[planIndex];
    // Frontend gap (fail closed): the capability may be available in the
    // helper, but this legacy frontend exposes no plan action for it, so the
    // row stays shown, disabled and unchecked with the exact reason.
    if (spec.frontendGap && *spec.frontendGap) {
        if (reason) {
            *reason = tr(spec.frontendGap);
        }
        return PlanStageUnavailable;
    }
    // Capability explicitly unavailable in the current scope (the exact helper
    // reason, including a stale cache). This is the soft gate: it disables the
    // checkbox and surfaces the reason, but never silently unchecks a saved
    // selection.
    std::string capabilityReason;
    const CapabilityModel::CapabilityState state =
        m_model.capabilityState(spec.capability, toStd(identity()),
                                &capabilityReason);
    if (state == CapabilityModel::CapabilityUnavailable) {
        if (reason) {
            *reason = localizedCapabilityReason(fromStd(capabilityReason));
        }
        return PlanStageUnavailable;
    }
    // Capability available, or its line simply not cached yet: a plain
    // checkable user setting.
    if (reason) {
        *reason = QString::null;
    }
    return PlanStageAvailable;
}

// The selected plan stages in execution order, filtered to the ones whose
// capability is available (the modern plan list excludes unavailable stages).
QStringList LegacyMainWindow::selectedPlanStages() const
{
    QStringList stages;
    for (int i = 0; i < planSpecCount; ++i) {
        if (!planStageSelected(i)) {
            continue;
        }
        if (!planStageAvailable(i, 0)) {
            continue;
        }
        stages.append(QString::fromLatin1(planSpecs[i].stage));
    }
    return stages;
}

QStringList LegacyMainWindow::selectedPlanTitles() const
{
    QStringList titles;
    for (int i = 0; i < planSpecCount; ++i) {
        if (!planStageSelected(i)) {
            continue;
        }
        if (!planStageAvailable(i, 0)) {
            continue;
        }
        titles.append(tr(planSpecs[i].title));
    }
    return titles;
}

bool LegacyMainWindow::planRunReady(QString *reason) const
{
    if (m_running) {
        if (reason) {
            *reason = tr("A helper command is already running.");
        }
        return false;
    }
    if (!diagnosticsScopeReady()) {
        if (reason) {
            *reason = scopeReadyReason();
        }
        return false;
    }
    if (!selectionComplete()) {
        if (reason) {
            *reason = tr(
                "The selected scope has no resolved root component; use "
                "Refresh Devices and commit the repair target again.");
        }
        return false;
    }
    if (selectedPlanStages().isEmpty()) {
        if (reason) {
            *reason = tr(
                "No Full Repair stages are selected or available; use "
                "Configure Plan to choose the stages.");
        }
        return false;
    }
    if (!m_model.hasDiagnostics(toStd(identity())) || m_model.diagnosticsStale()) {
        if (reason) {
            *reason = tr(kPlanReadinessDefault);
        }
        return false;
    }
    // Single source of truth: every selected+available stage's required
    // diagnostic sections must be cached before the plan may run (modern
    // runFullRepair's per-stage repairEvidenceReadyForTool gate).
    for (int i = 0; i < planSpecCount; ++i) {
        if (!planStageSelected(i) || !planStageAvailable(i, 0)) {
            continue;
        }
        const QStringList missing = missingRequiredDiagnosticSections(
            QString::fromLatin1(planSpecs[i].capability));
        if (!missing.isEmpty()) {
            QStringList titles;
            for (QStringList::ConstIterator it = missing.begin(); it != missing.end(); ++it) {
                titles.append(diagnosticTitle(*it));
            }
            if (reason) {
                *reason = tr("Run the %1 diagnostic for the selected scope first "
                             "(the cached evidence is missing).")
                              .arg(titles.join(QString::fromLatin1(", ")));
            }
            return false;
        }
    }
    if (!administratorSessionActive()) {
        if (reason) {
            *reason = tr(
                "Administrator authorization is required; press Authorize on "
                "the Systems or Repair tab to establish the session.");
        }
        return false;
    }
    if (reason) {
        *reason = tr(
            "Runs the selected stages using the cached read-only diagnostic "
            "evidence after privilege confirmation.");
    }
    return true;
}

// Plan section: the numbered list of selected+available stages, the count
// label and the readiness text, exactly like the modern Full Repair plan.
// Round 16 (a): with NO committed scope (no host maintenance, no committed
// target) the plan shows NO selected stages — the modern plan list is empty
// in that state, never the default-checked stage set.  Round 16 (b): the
// splitter sizes are re-applied only when the plan CONTENT actually changed
// (the stage set, their wrapped heights or the chrome); a plain re-entry
// (selecting an individual repair item, a tab switch) leaves the splitter
// exactly where the user left it.
void LegacyMainWindow::updatePlanView()
{
    if (m_planStageLabels.isEmpty() || !m_planCountLabel
        || !m_planReadinessLabel || !m_runFullRepairButton) {
        return;
    }
    for (int i = 0; i < static_cast<int>(m_planStageLabels.size()); ++i) {
        m_planStageLabels[i]->setText(QString::null);
        m_planStageLabels[i]->hide();
    }
    int availableCount = 0;
    QString excludedReason;
    QStringList titles;
    // Round 16 (a): the plan enumerates stages only for a READY scope (Host
    // Maintenance active or a committed target).  On first open the start-up
    // host auto-detection records the protected running host as the selected
    // disk WITHOUT any user action, so selectionComplete() alone is not "the
    // user selected something"; diagnosticsScopeReady() is.  Nothing ready ->
    // NO selected stages (modern parity).
    const bool scopeComplete = diagnosticsScopeReady();
    if (scopeComplete) {
        for (int i = 0; i < planSpecCount; ++i) {
            if (!planStageSelected(i)) {
                continue;
            }
            QString reason;
            if (!planStageAvailable(i, &reason)) {
                if (excludedReason.isEmpty()) {
                    excludedReason = reason;
                }
                continue;
            }
            ++availableCount;
            titles.append(QString::fromLatin1("%1. %2").arg(availableCount)
                              .arg(tr(planSpecs[i].title)));
        }
    } else {
        // Round 16 (a): nothing is selected — the plan list stays empty and
        // the readiness names the scope reason (modern parity).
        excludedReason = scopeReadyReason();
    }
    if (availableCount == 0) {
        m_planCountLabel->setText(tr("No stages selected"));
        m_planStageLabels[0]->setText(tr(
            "No Full Repair stages selected - use Configure "
            "Plan or Settings."));
        m_planStageLabels[0]->show();
    } else {
        // Balanced two-column fill (modern parity): the titles flow down the
        // left column first, then the right, preserving plan order, with the
        // columns split as evenly as possible (leftCount = ceil(N/2),
        // rightCount = floor(N/2), so 8 -> 4/4, 9 -> 5/4, 4 -> 2/2). The grid
        // is a fixed two-column grid of ceil(planSpecCount/2) rows, so the
        // right column's labels begin at label index ceil(planSpecCount/2).
        const int leftCount = (availableCount + 1) / 2;
        const int rightColumnStart = (planSpecCount + 1) / 2;
        int index = 0;
        for (QStringList::ConstIterator it = titles.begin();
             it != titles.end(); ++it, ++index) {
            const int slot = index < leftCount
                ? index
                : rightColumnStart + (index - leftCount);
            m_planStageLabels[slot]->setText(*it);
            m_planStageLabels[slot]->show();
        }
        m_planCountLabel->setText(availableCount == 1
            ? tr("1 stage selected")
            : tr("%1 stages selected").arg(availableCount));
    }

    QString readiness;
    if (!scopeComplete) {
        // Round 16 (a): nothing selected — the plan names the scope reason
        // (modern parity), not a "no stages selected" hint.
        readiness = scopeReadyReason();
    } else if (availableCount == 0) {
        readiness = excludedReason.isEmpty()
            ? tr(
                  "No repair stages are selected. Use Configure Plan to "
                  "choose the stages Full Repair will run.")
            : tr("No selected stages are available. %1").arg(excludedReason);
    } else if (!diagnosticsScopeReady()) {
        readiness = scopeReadyReason();
    } else if (!selectionComplete()) {
        readiness = tr(
            "The selected scope has no resolved root component; use Refresh "
            "Devices and commit the repair target again.");
    } else if (!m_model.hasDiagnostics(toStd(identity())) || m_model.diagnosticsStale()) {
        readiness = tr(kPlanReadinessDefault);
    } else if (!administratorSessionActive()) {
        readiness = tr(
            "Administrator authorization is required; press Authorize on the "
            "Systems or Repair tab to establish the session.");
    } else {
        readiness = tr(kPlanReadinessReady);
    }
    m_planReadinessLabel->setText(readiness);

    QString runReason;
    const bool canRun = planRunReady(&runReason);
    m_runFullRepairButton->setEnabled(canRun);
    QToolTip::add(m_runFullRepairButton, runReason);
    // Cap the plan scroll view to the grid's real content height, computed
    // from the visible labels' own size hints.  Measured on the rig: the
    // AutoOneFit scroll view's inflated size hint (~676px) made the vertical
    // splitter give the plan frame 700 of the ~645px Repair page (the tools
    // pane was squeezed to 257px and clipped) and stretched the four stage
    // rows to 92px each against 17px size hints (the dead white space between
    // the rows and after the last one).  With the cap the plan frame sizes to
    // its content, the stage rows keep their natural height, the readiness
    // hint gets its full size hint back, and the tools pane reclaims the
    // space; a two-row floor keeps the view shrinkable so the vertical
    // scrollbar still appears when the splitter is dragged up.
    if (m_planScroll) {
        int rowHint = 0;
        for (int i = 0; i < static_cast<int>(m_planStageLabels.size()); ++i) {
            QLabel *stageLabel = m_planStageLabels[i];
            if (!stageLabel || stageLabel->isHidden()) {
                continue;
            }
            rowHint = QMAX(rowHint, stageLabel->sizeHint().height());
        }
        if (rowHint > 0) {
            // The balanced two-column fill's taller column holds
            // ceil(availableCount/2) rows.  Round 21 (empty-plan comfort,
            // modern parity) kept TWO rows — the info label plus one spare
            // row of padding below it.  Round 23 (Debian Legacy #3): measured
            // on the rig the default splitter handle still sits on top of the
            // last line at two rows; the empty plan now keeps THREE rows —
            // the info label plus ~2 spare lines of padding below it — so the
            // default splitter clears the text and the box shows no
            // scrollbar, exactly like the modern empty plan's content +
            // comfortable floor.
            const int rows = availableCount == 0
                ? 3 : ((availableCount + 1) / 2);
            const int gridSpacing = 4;
            const int padding = 8;
            const int content = rows * rowHint
                + (rows - 1) * gridSpacing + padding;
            m_planGridContentHeight = content;
            m_planScroll->setMaximumHeight(content);
            m_planScroll->setMinimumContentHeight(
                rowHint + gridSpacing + padding);
            m_planScroll->setMinimumHeight(
                rowHint + gridSpacing + padding);
            // Round 21 (splitter hug + static, the Etch-measured model): the
            // plan frame hugs its content — the splitter's upper pane gets
            // EXACTLY the frame height and the lower tools pane takes the
            // remainder (the old equal-sizes call scaled both panes 50/50,
            // which is why the plan frame never hugged the last stage row).
            // The size application stays content-driven ONLY: a re-entry
            // that changed nothing (selecting an individual repair item, a
            // tab switch, a full-repair run/cancel/popup close) never moves
            // the splitter; a real content change (stage set, wrap heights
            // or chrome) re-applies the hug.  The hug defers to the
            // tab-visibility hook while the Repair page is not the current
            // one (the Settings toggle case): Qt3 does not re-layout a
            // hidden tab, so a hug applied there would size from stale
            // geometry — the hook re-applies with the real layout.
            if (m_planGroup) {
                const int chrome = m_planGroup->sizeHint().height()
                    - m_planScroll->sizeHint().height();
                const QString signature = QString::fromLatin1("%1|%2|%3|%4")
                    .arg(availableCount)
                    .arg(rowHint)
                    .arg(chrome)
                    .arg(titles.join(QString::fromLatin1("\x1f")));
                if (m_planViewContentSignature == signature) {
                    return;
                }
                m_planViewContentSignature = signature;
                m_planHugContentHeight = content;
                m_planHugPending = true;
                applyPlanSplitterHug();
            }
        }
    }
}

// Round 21: applies the plan pane's content hug through the sum-preserving
// setSizes() path (upper pane = the frame's tight height, the lower tools
// pane = the remainder).  Only runs while a hug is pending (the content
// signature changed in updatePlanView()) and only when the splitter has real
// geometry — while the Repair page is hidden the call is deferred and the
// tab-visibility hook re-invokes it after the page lays out for real.  The
// frame height is the group box's sizeHint minus the inflated scroll hint
// plus the grid's tight content plus one line of bottom padding, so the
// frame's bottom line hugs the last stage row instead of leaving dead rows
// below it.  The group's MINIMUM is the same full height: the splitter can
// shrink the pane down to the hugged frame (every stage row, the count
// header and the readiness label fully visible) but never below it — no row
// can ever be cut by the handle.
void LegacyMainWindow::applyPlanSplitterHug()
{
    if (!m_planHugPending || !m_planGroup || !m_planScroll
        || !m_repairVerticalSplitter) {
        return;
    }
    // The Repair page must be the current tab: a hug applied while the page
    // is hidden sizes from stale geometry (the Settings stage-toggle case —
    // Qt3 does not re-layout a hidden tab), so it defers to the
    // tab-visibility hook (repairTabShown()), which re-invokes this pass
    // after the page laid out for real.
    if (!m_tabs || m_tabs->currentPage() != m_repairPage) {
        return;
    }
    // Activate the ancestor layouts synchronously before measuring: the
    // readiness label's text may have changed just before this pass (a stage
    // toggle, a diagnostics completion), and its REAL wrapped height is only
    // correct after the layout computes it — activate() is a synchronous
    // layout computation (no event re-entrancy), the same sequence the smoke
    // measurement uses.
    for (QWidget *ancestor = m_planGroup; ancestor;
         ancestor = ancestor->parentWidget()) {
        if (ancestor->layout()) {
            ancestor->layout()->activate();
        }
    }
    const int total = m_repairVerticalSplitter->height();
    if (total <= 0) {
        // No real geometry yet (the page was never laid out): keep the hug
        // pending; the Repair-tab visibility hook applies it after layout.
        return;
    }
    // Round 25 (Debian Legacy #4): measure the chrome from REAL laid-out
    // geometry, never from the sizeHint difference.  The group's sizeHint is
    // the bare VBox hint (QGroupBox::sizeHint with a layout returns
    // QFrame::sizeHint — the layout hint WITHOUT the group title/frame
    // overhead) and it carries the word-wrapped readiness label at its
    // ~80-char probe-width hint height, while the real layout resolves the
    // label at the pane's actual width; the round-24
    // `chrome + grid content + one line` path therefore under-measured small
    // grids (3 rows hugged 2, 2 rows overlapped the list entirely, while
    // 4+ rows happened to fit).
    // The scroll view's laid-out top offset inside the plan group IS
    // everything above the grid for real: the group title/frame, the section
    // title, the count + Configure/Run rows and the readiness label at its
    // REAL wrapped height.  It is stable while the frame is short: the rows
    // above the scroll never shrink (the trailing stretch absorbs the slack
    // first), so the same value is read before and after the hug — no
    // bounce, no convergence loop.  The bottom chrome is the layout's bottom
    // margin plus the group's bottom frame line below the layout area.
    // frameHeight = real top chrome + the grid's tight content + real bottom
    // chrome — the pane minimum is then the FULL content + label for every
    // count (1..6 rows hug exactly, no dead rows; the empty plan keeps its
    // label + 2 spare rows).
    const int scrollTopY = m_planScroll->mapTo(m_planGroup, QPoint(0, 0)).y();
    if (scrollTopY <= 0) {
        // No real layout yet (the page was never laid out): keep the hug
        // pending; the Repair-tab visibility hook applies it after layout.
        return;
    }
    const int layoutMargin = m_planGroup->layout()
        && m_planGroup->layout()->margin() > 0
        ? m_planGroup->layout()->margin() : 0;
    const int bottomChrome = layoutMargin + m_planGroup->frameWidth() + 1;
    const int frameHeight = scrollTopY + m_planHugContentHeight + bottomChrome;
    // Cap the frame so it does not over-allocate from the scroll view's
    // inflated sizeHint, but keep the cap generous (2x) so the handle can be
    // dragged down.  The MINIMUM is the full content height (Debian Legacy
    // #3): the drag can still shrink the pane down to the hugged frame —
    // which shows every stage row plus the label — but never below it, so
    // 5 stages keep all 5 labels visible, host maintenance keeps all 4 and
    // the empty plan keeps the info label + 2 comfortable rows (the
    // round-23 default).
    m_planGroup->setMaximumHeight(2 * frameHeight);
    m_planGroup->setMinimumHeight(frameHeight);
    // Stack minimum (the modern model): the splitter keeps enough total
    // height for the hugged plan pane plus the tools pane's own floor, so
    // the hug never squeezes the lower pane below its floor — a short window
    // scrolls the page's QScrollView instead.
    const int lowerFloor = m_repairToolsSplitter
        ? m_repairToolsSplitter->minimumSizeHint().height() : 0;
    const int stackMinimum = frameHeight
        + m_repairVerticalSplitter->handleWidth() + lowerFloor;
    if (m_repairVerticalSplitter->minimumHeight() < stackMinimum) {
        m_repairVerticalSplitter->setMinimumHeight(stackMinimum);
    }
    QValueList<int> sizes;
    sizes.append(frameHeight);
    sizes.append(QMAX(total - frameHeight, lowerFloor));
    m_repairVerticalSplitter->setSizes(sizes);
    m_planHugPending = false;
}

// Round 21: the Repair-tab visibility hook.  A deferred plan hug (a Settings
// stage toggle while the Repair page was hidden) re-applies here, after the
// page became current and its ancestor layouts activated for real — the
// Etch measurement sequence (switch + flush + activate + flush), because
// Qt3 does not re-layout a hidden tab from processEvents() alone.  With no
// pending hug the hook does nothing, so ordinary tab switches never move
// the splitter.
void LegacyMainWindow::repairTabShown()
{
    if (!m_planHugPending || !m_tabs
        || m_tabs->currentPage() != m_repairPage) {
        return;
    }
    qApp->sendPostedEvents();
    qApp->processEvents();
    for (QWidget *ancestor = m_planGroup; ancestor;
         ancestor = ancestor->parentWidget()) {
        if (ancestor->layout()) {
            ancestor->layout()->activate();
        }
    }
    qApp->processEvents();
    applyPlanSplitterHug();
}

void LegacyMainWindow::runFullRepair()
{
    const QStringList stages = selectedPlanStages();
    if (stages.isEmpty()) {
        QMessageBox::information(
            this, tr("Full Repair"),
            tr(
                "No Full Repair stages are selected or available; use "
                "Configure Plan to choose the stages."),
            QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    QString reason;
    if (!planRunReady(&reason)) {
        QMessageBox::warning(this, tr("Full Repair unavailable"),
                             reason, QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    const bool host = hostScope();
    // Modern parity (MainWindow::runFullRepair / confirmRepairAction): the
    // plan's confirmation popup mirrors the modern title, description and
    // button text exactly.  The operation list is the modern per-stage wording
    // (the same strings the modern Full Repair plan lists), not the legacy
    // Settings row titles, so the confirmation reads identically on Etch.
    const QString selectedSystemLabel = host
        ? tr("running host") : tr("selected repair system");
    QStringList operations;
    for (QStringList::ConstIterator it = stages.begin(); it != stages.end(); ++it) {
        const QString stage = *it;
        if (stage == QString::fromLatin1("filesystem")) {
            operations.append(tr(
                "Run the read-only file system check, then repair the selected "
                "devices"));
        } else if (stage == QString::fromLatin1("dpkg-configure")) {
            operations.append(tr("Complete interrupted package configuration"));
        } else if (stage == QString::fromLatin1("fix-broken")) {
            operations.append(tr("Repair broken APT dependencies"));
        } else if (stage == QString::fromLatin1("apt-update")) {
            operations.append(tr("Refresh APT package metadata"));
        } else if (stage == QString::fromLatin1("apt-upgrade")) {
            operations.append(tr(
                "Simulate APT first, then choose a safe "
                "upgrade/full-upgrade/dist-upgrade transaction"));
        } else if (stage == QString::fromLatin1("dkms")) {
            operations.append(tr("Rebuild DKMS modules"));
        } else if (stage == QString::fromLatin1("display-manager")) {
            operations.append(tr(
                "Restore the detected display manager and graphical.target "
                "without starting the GUI inside chroot"));
        } else if (stage == QString::fromLatin1("initramfs")) {
            operations.append(tr("Rebuild all initramfs images"));
        } else if (stage == QString::fromLatin1("efi")) {
            operations.append(tr(
                "Repair the %1 EFI / UKI boot path using its validated ESP")
                                  .arg(selectedSystemLabel));
        } else if (stage == QString::fromLatin1("grub")) {
            operations.append(tr("Regenerate the %1's GRUB configuration")
                                  .arg(selectedSystemLabel));
        } else if (stage == QString::fromLatin1("extlinux")) {
            operations.append(tr("Regenerate the %1's extlinux configuration")
                                  .arg(selectedSystemLabel));
        }
    }
    operations.prepend(tr(
        "Read-only diagnostics completed; review the full evidence in Logs "
        "before confirming repair."));

    QString text;
    if (host) {
        text = tr("Confirm changes to the running host")
            + QString::fromLatin1("\n\n")
            + tr(
                "Host disk: %1\nLinux root: %2\n\nThe following guarded changes "
                "will run natively on the active system:\n- %3\n\n"
                "The helper independently re-checks that the selected disk backs "
                "/, /boot and /boot/efi, and applies the same package, mapper, "
                "EFI/NVRAM and GRUB preservation safeguards used for repair targets.")
                  .arg(selectedDisk())
                  .arg(selectedRoot())
                  .arg(operations.join(QString::fromLatin1("\n- ")));
    } else {
        text = tr("Confirm changes to the selected repair system")
            + QString::fromLatin1("\n\n")
            + tr(
                "Target disk: %1\nLinux root: %2\n\nThe following target-system "
                "changes will run:\n- %3\n\n"
                "The running host is independently re-checked and refused by the "
                "privileged helper.")
                  .arg(selectedDisk())
                  .arg(selectedRoot())
                  .arg(operations.join(QString::fromLatin1("\n- ")));
    }
    const bool answer = confirmWrapped(
        this, tr("Run Full Repair"), text,
        /*cancelInsteadOfNo=*/true, tr("Run Repair"), tr("Cancel"),
        kHiddenInputMaximumWidth,
        /*noIsDefault=*/true);
    if (!answer) {
        return;
    }
    // File system repair is inspect-first: the read-only fs-inspect run and the
    // per-device decision happen before the remaining stages execute. The stage
    // list sent to the helper excludes `filesystem` because fs-repair is a
    // separate helper verb with its own device/mode arguments. The complete
    // plan - the inspect, the inline per-device decision, each per-device
    // repair and the remaining stages - runs inside ONE repair result popup, so
    // no window changes mid-plan. Declining the file system repair stops the
    // plan before the remaining stages (the driver fails closed).
    QStringList helperStages = stages;
    // Qt3 QStringList has no removeAll() (Qt 4+); QValueList::remove(const T&)
    // drops every occurrence but returns void, so probe with contains() first.
    const bool hasFilesystem = helperStages.contains(QString::fromLatin1("filesystem"));
    if (hasFilesystem) {
        helperStages.remove(QString::fromLatin1("filesystem"));
    }
    if (!beginRepairPlan(/*fullRepair=*/true, helperStages,
                         /*runFilesystem=*/hasFilesystem)) {
        // beginRepairPlan already reset the plan state; nothing ran, so no
        // end-of-plan invalidation is needed.
        return;
    }
    if (!m_smokeMode) {
        showRepairResultDialog(tr("Full Repair"));
    }
}

// Modern-style repair result popup: tool title, bold status line, the streamed
// helper transcript and a Close button that enables on finish. Never opened in
// --smoke-test; diagnostics keep their in-tab Results behavior.
void LegacyMainWindow::showRepairResultDialog(const QString &title)
{
    if (m_smokeMode || m_resultDialog) {
        return;
    }
    RepairResultDialog *dialog = new RepairResultDialog(this, "legacy-repair-result");
    dialog->setCaption(title);
    dialog->resize(760, 460);
    QVBoxLayout *layout = new QVBoxLayout(dialog, 10, 8);
    m_resultStatus = new QLabel(
        tr(
            "Running %1 through the privileged helper... The Logs tab keeps "
            "the complete transcript.").arg(title),
        dialog);
    QFont statusFont = m_resultStatus->font();
    statusFont.setBold(true);
    m_resultStatus->setFont(statusFont);
    m_resultStatus->setAlignment(Qt::WordBreak | Qt::AlignLeft);
    layout->addWidget(m_resultStatus);
    m_resultView = new QTextEdit(dialog);
    m_resultView->setReadOnly(true);
    // Qt 3.3.7's Qt::LogText mode disables word wrap entirely, so the popup
    // uses Qt::PlainText (the transcript is short-lived, not the Logs view)
    // and follows the View menu's "Wrap Log Lines" toggle (item 1 removed the
    // Settings checkbox) like the Logs view; the monospace font stays either
    // way.
    m_resultView->setTextFormat(Qt::PlainText);
    m_resultView->setWordWrap(m_logWrapEnabled
                                  ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);
    QFont mono(QString::fromLatin1("monospace"));
    mono.setStyleHint(QFont::TypeWriter);
    m_resultView->setFont(mono);
    m_resultView->setText(m_transcript);
    layout->addWidget(m_resultView, 1);
    // Interactive repair prompt surface (modern parity): a persistent input
    // line + Send button, enabled and focused only while a PROMPT record is
    // waiting.  Qt 3.3.7 QLineEdit has no placeholder, so the hint is a small
    // label above the input row; it is hidden until the first prompt.
    m_promptHint = new QLabel(tr("Number of the action (0 = Abort)"), dialog);
    m_promptHint->hide();
    layout->addWidget(m_promptHint);
    QHBoxLayout *promptRow = new QHBoxLayout(layout);
    m_promptInput = new PromptAnswerLineEdit(dialog);
    m_promptInput->setEnabled(false);
    m_promptSend = new QPushButton(tr("Send"), dialog);
    m_promptSend->setEnabled(false);
    promptRow->addWidget(m_promptInput, 1);
    promptRow->addWidget(m_promptSend);
    QObject::connect(m_promptInput, SIGNAL(returnPressed()), this,
                     SLOT(sendPromptAnswer()));
    QObject::connect(m_promptSend, SIGNAL(clicked()), this,
                     SLOT(sendPromptAnswer()));
    QHBoxLayout *buttons = new QHBoxLayout(layout);
    buttons->addStretch();
    // A12-09: the Cancel button (modern RepairProgressDialog parity) —
    // armed only while the helper command runs, disabled once nothing is
    // cancellable; the popup's own Close button (below) dismisses it after
    // completion/abort.
    m_resultCancelButton = new QPushButton(tr("Cancel"), dialog);
    m_resultCancelButton->setEnabled(false);
    buttons->addWidget(m_resultCancelButton);
    QObject::connect(m_resultCancelButton, SIGNAL(clicked()), this,
                     SLOT(cancelResultOperation()));
    m_resultCloseButton = new QPushButton(tr("Close"), dialog);
    m_resultCloseButton->setDefault(true);
    m_resultCloseButton->setEnabled(false);
    buttons->addWidget(m_resultCloseButton);
    QObject::connect(m_resultCloseButton, SIGNAL(clicked()), dialog, SLOT(accept()));
    m_resultDialog = dialog;
    // The dialog is shown while the command already runs: arm Cancel now.
    updateResultCancelState();
    dialog->exec();
    // exec() returns only on Close (accept), which stays disabled until the
    // helper request completes and helperFinished calls setCloseAllowed(true)
    // (Escape and the window-close are refused before then). The command has
    // therefore always finished by the time exec() returns; clear the
    // streaming targets before deleting the dialog.
    m_resultDialog = 0;
    m_resultStatus = 0;
    m_resultView = 0;
    m_promptHint = 0;
    m_promptInput = 0;
    m_promptSend = 0;
    m_resultCloseButton = 0;
    m_resultCancelButton = 0;
    delete dialog;
}

// The helper's `shell`/`host-shell` verbs run one reviewed command string as
// root (inside a fresh target chroot, or directly on the running host). The
// GUI passes the string as a single argument (no shell interpolation), gates
// the controls on the scope's cached `Legacy feature` probe plus the cached
// administrator session, and never weakens the helper's runtime preflights.
// A12-09: arms/disarms the repair result popup's Cancel button from the run
// state (modern round-12 parity: armed only while a cancellable request is in
// flight; disabled at all other times).
void LegacyMainWindow::updateResultCancelState()
{
    if (!m_resultCancelButton) {
        return;
    }
    m_resultCancelButton->setEnabled(m_running && !m_resultCancelRequested);
}

// A12-09: the repair result popup's Cancel handler.  Cancels the running
// helper command through the documented legacy cancel path (the mode-600
// cancel-file token observed at the helper's stage boundaries / shell ticks,
// then the bounded sudo-wrapper kill), disables the button (one-shot) and
// shows the transient stopping status; helperFinished() reports the
// stopped-at-your-request status and enables Close.
void LegacyMainWindow::cancelResultOperation()
{
    if (!m_running || m_resultCancelRequested) {
        return;
    }
    m_resultCancelRequested = true;
    updateResultCancelState();
    updateActionStates();
    if (m_resultStatus) {
        m_resultStatus->setText(tr(
            "Stopping the running operation... The Logs tab keeps the "
            "complete transcript."));
    }
    appendLog(tr(
        "Stopping the running operation... The Logs tab keeps the complete "
        "transcript."));
    m_runner->cancel();
}

// Round 21 (modern cancel-surface parity): the Diagnostics page's Cancel.
// Same one-shot cancel path as the repair popup — the read-only diagnostic
// result is never cached afterwards (helperFinished applies a cancelled run
// fail-closed) and the completion line reports "stopped at your request".
void LegacyMainWindow::cancelDiagnosticOperation()
{
    if (!m_running || !m_pendingDiagnostic || m_resultCancelRequested) {
        return;
    }
    m_resultCancelRequested = true;
    updateResultCancelState();
    updateActionStates();
    // Round 27 (Debian Legacy #7, modern round-10 ITEM 3 parity): the doomed
    // section is invalidated AT CANCEL INITIATION (idempotent with the
    // run-start invalidation in startCommand), so a tab switch during the
    // cancel window can never re-retrieve the section being cancelled.
    if (!m_pendingDiagnosticKey.isEmpty()) {
        m_diagnosticSections.remove(m_pendingDiagnosticKey.lower());
        m_fullDiagnosticReport = removeEmbeddedDiagnosticSection(
            m_fullDiagnosticReport, m_pendingDiagnosticKey);
        refreshDiagnosticResultsView();
    } else {
        // Round 28 (Debian Legacy #3, modern matrix): cancelling the combined
        // `all`/report run invalidates the FULL register AT CANCEL INITIATION
        // too (modern invalidateAllActiveScopeDiagnostics at initiation), so
        // a tab switch during the cancel window shows every section empty
        // from the start — never the pre-cancel cached content.  The
        // completion path re-runs the clear idempotently and resets the
        // capability model.
        m_diagnosticSections.clear();
        m_fullDiagnosticReport = QString::null;
        refreshDiagnosticResultsView();
    }
    // Round 29 (Debian Legacy #8, the cancel race): track the cancel-
    // initiation invalidation so helperFinished() never re-shows the doomed
    // run's transcript in the results pane after the cancel. The pane was
    // emptied here and must STAY empty through the completion; the raw
    // transcript remains visible in the Logs tab either way.
    m_diagnosticCancelInvalidated = true;
    appendLog(tr(
        "Stopping the running diagnostic... The Logs tab keeps the complete "
        "transcript."));
    m_runner->cancel();
}

// Round 21 (modern cancel-surface parity): the Chroot/Host Shell page's
// Cancel.  Same one-shot cancel path; helperFinished() closes the transcript
// with its exit marker and reports "stopped at your request".
void LegacyMainWindow::cancelShellOperation()
{
    if (!m_running || !m_pendingShell || m_resultCancelRequested) {
        return;
    }
    m_resultCancelRequested = true;
    updateResultCancelState();
    updateActionStates();
    appendLog(tr(
        "Stopping the running shell command... The Logs tab keeps the "
        "complete transcript."));
    m_runner->cancel();
}

void LegacyMainWindow::runChrootShell()
{
    if (m_running) {
        return;
    }
    if (!diagnosticsScopeReady()) {
        QMessageBox::information(this, tr("Shell scope required"),
                                 scopeReadyReason(),
                                 QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    if (!administratorSessionActive()) {
        QMessageBox::warning(
            this, tr("Administrator authorization required"),
            tr(
                "No administrator session is active.\n\n"
                "Press Authorize on the Systems or Repair tab to establish the "
                "session, or enter Host Maintenance / commit a repair target on "
                "the Systems tab; the chroot shell then reuses the cached "
                "authorization."),
            QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    const QString command = m_shellCommandEdit
        ? m_shellCommandEdit->text() : QString::null;
    if (command.stripWhiteSpace().isEmpty()) {
        QMessageBox::information(this, tr("Shell command required"),
                                 tr(
                                     "Enter the command to run first."),
                                 QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    const bool host = hostScope();
    // The modern GUI runs a running-host shell command directly after the
    // scope/authorization checks above; no per-command confirmation popup is
    // shown here (parity with the target-scope chroot shell, which has none).
    QStringList args;
    const QString shellVerb = host ? QString::fromLatin1("host-shell")
                                   : QString::fromLatin1("shell");
    args << shellVerb << selectedDisk() << selectedRoot() << command;
    // Each shell command's transcript is APPENDED to the shared pane (modern
    // parity), never clearing the previous run. A blank line plus a rule
    // separates successive commands so the per-command "Running %1..." /
    // "[exit N]" markers stay visually distinct; the Clear Output button is
    // the only way to empty the pane.
    if (m_shellOutput) {
        if (!m_shellOutput->text().stripWhiteSpace().isEmpty()) {
            appendShellOutput(QString::null);
            appendShellOutput(QString::fromLatin1(
                "----------------------------------------"));
        }
        appendShellOutput(tr("Running %1...").arg(shellVerb));
    }
    // Item #3 (modern parity): record the dispatched command into the
    // session-only history (skipping consecutive duplicates, capped at 500),
    // reset the navigation state, and clear the field once so it is empty while
    // the command runs. A completion-time clear would wipe text the user typed
    // during the run, so there is none; the user can recall the command with Up.
    if (m_shellHistory.isEmpty() || m_shellHistory.last() != command) {
        m_shellHistory.append(command);
        if ((int)m_shellHistory.size() > 500) {
            m_shellHistory.pop_front();
        }
    }
    m_shellHistoryIndex = -1;
    m_shellHistoryDraft = QString::null;
    if (m_shellCommandEdit) {
        m_shellCommandEdit->clear();
    }
    // The shell verbs drive the helper's generic interactive PTY pump over the
    // same PROMPT/ANSWER channel as the kernel/GPU menus.  Set interactive=true
    // so startCommand reserves the menu-answer file and HelperRunner hands the
    // helper BOOT_REPAIR_SESSION_PROTOCOL=1 and the menu-answer path explicitly
    // (also through an elevated run), and the Shell tab's own inline input line
    // can answer a question the command asks instead of the helper running it
    // on stdin /dev/null.  The transcript streams live into m_shellOutput
    // (helperLine) and any question is answered inline in this tab - no separate
    // showRepairResultDialog popup (prompt-flow contract L1/L2).
    startCommand(args, false, shellVerb,
                 false, false, true, false,
                 QString::null, QString::null,
                 false, true);
}

void LegacyMainWindow::clearChrootOutput()
{
    if (m_shellOutput) {
        m_shellOutput->setText(QString::null);
    }
    updateActionStates();
}

// Item #3 (modern parity): session-only Up/Down command-history navigation for
// the Shell tab command field. +1 steps Up (toward older entries), -1 steps
// Down (toward newer entries, then back to the saved draft). The history is
// in-memory only and is never persisted to QSettings.
void LegacyMainWindow::stepShellHistory(int direction)
{
    if (!m_shellCommandEdit) {
        return;
    }
    if (direction > 0) {
        // Up: toward older entries.
        if (m_shellHistory.isEmpty()) {
            return;
        }
        if (m_shellHistoryIndex == -1) {
            m_shellHistoryDraft = m_shellCommandEdit->text();
            m_shellHistoryIndex = (int)m_shellHistory.size() - 1;
        } else if (m_shellHistoryIndex > 0) {
            --m_shellHistoryIndex;
        } else {
            return; // Already at the oldest entry.
        }
        m_shellCommandEdit->setText(m_shellHistory[m_shellHistoryIndex]);
    } else {
        // Down: toward newer entries, then back to the saved draft.
        if (m_shellHistoryIndex == -1) {
            return;
        }
        if (m_shellHistoryIndex + 1 < (int)m_shellHistory.size()) {
            ++m_shellHistoryIndex;
            m_shellCommandEdit->setText(m_shellHistory[m_shellHistoryIndex]);
        } else {
            m_shellHistoryIndex = -1;
            m_shellCommandEdit->setText(m_shellHistoryDraft);
            m_shellHistoryDraft = QString::null;
        }
    }
}

bool LegacyMainWindow::administratorSessionActive() const
{
    // True when no modal password is pending: root/direct execution, an
    // authenticated `sudo` session (including Etch's timestamp-validated plain
    // sudo), `sudo -n`, or an elevation method that prompts on its own
    // (gksu/gksudo).
    return !m_runner->elevationNeedsPassword(0);
}

// The cached decision plus a live, non-blocking timestamp probe. A plain-sudo
// timestamp can expire between commands; probing here (stdin closed, so an
// expired sudo gets EOF instead of blocking on a password read) lets the GUI
// fail closed with the Authorize remedy instead of hanging.
bool LegacyMainWindow::sessionStillCurrent()
{
    if (!administratorSessionActive()) {
        return false;
    }
    if (m_runner->sessionIsCurrent()) {
        return true;
    }
    m_runner->resetElevation();
    appendLog(tr(
        "The cached administrator authorization expired or was refused; press "
        "Authorize (Systems or Repair tab) to re-establish the session."));
    updateElevationLabel();
    updateActionStates();
    return false;
}

void LegacyMainWindow::authorizeNow()
{
    if (m_running) {
        return;
    }
    if (!diagnosticsScopeReady()) {
        QMessageBox::information(this, tr("Authorization scope required"),
                                 scopeReadyReason(),
                                 QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    if (ensureAdministratorSession(tr("administrator authorization"))) {
        appendLog(tr(
            "Administrator session authorized from the Authorize control."));
        // Modern handlePrivilegedSessionEstablished() parity: establishing the
        // session releases the deferred diagnostics regeneration the Authorize
        // affordance promised ("diagnostics will regenerate after you
        // authorize"). It never opens its own authorization prompt.
        maybeAutoRefreshDiagnostics(QString::fromLatin1(
            "administrator authorization became active"));
    }
    updateActionStates();
}

// Authorization is requested exactly once per scope: on entering Host
// Maintenance or committing a repair target. Every privileged command reuses
// the cached session afterwards, so Run All never opens the password dialog.
// The hidden-input modal is kept and the password is fed to `sudo -S -v` over
// a pipe only; it is never logged or placed on a command line.
bool LegacyMainWindow::ensureAdministratorSession(const QString &context)
{
    QString description;
    if (!m_runner->elevationNeedsPassword(&description)) {
        return true;
    }
    if (m_smokeMode) {
        // The smoke test must never block on a modal dialog: the Etch
        // validation runs as root or after `sudo -S -v` has cached the
        // timestamp, exactly like the launcher's sudo elevation.
        appendLog(tr(
            "ERROR: elevation requires an interactive password; the smoke test "
            "runs as root or after sudo authentication."));
        return false;
    }
    const char *userEnv = ::getenv("USER");
    const QString user = userEnv ? QString::fromLocal8Bit(userEnv) : QString::null;
    bool ok = false;
    QString password = promptHiddenPassword(
        this, tr("Administrator authorization"),
        tr(
            "Administrator authorization is required for %1.\n\n"
            "Enter the password for %2 (sudo). It is used only for this sudo "
            "authentication, is sent over a pipe and is never logged or placed "
            "on a command line. The authorization is cached for this session "
            "and reused by diagnostics and repairs.")
            .arg(context)
            .arg(user.isEmpty() ? tr("your account") : user),
        &ok);
    if (!ok) {
        appendLog(tr(
            "Administrator authorization was cancelled; the command was not started."));
        return false;
    }
    if (password.isEmpty()) {
        QMessageBox::warning(this, tr("Password required"),
                             tr(
                                 "An empty password was not submitted. Enter the "
                                 "sudo password or choose Cancel."),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return false;
    }
    QByteArray secret = password.local8Bit();
    password.fill(QChar(0));
    password = QString::null;
    QString error;
    const bool authenticated = m_runner->authenticateElevation(secret, &error);
    secret.fill('\0');
    secret = QByteArray();
    if (!authenticated) {
        appendLog(tr("Administrator authorization failed: %1").arg(error));
        QMessageBox::warning(this, tr("Administrator authorization failed"),
                             tr(
                                 "sudo did not accept the password: %1\n\n"
                                 "The command was not started.").arg(error),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return false;
    }
    appendLog(tr(
        "Administrator authorization established with sudo for this session (%1).")
        .arg(context));
    updateElevationLabel();
    return true;
}

bool LegacyMainWindow::startCommand(const QStringList &args,
                                     bool diagnostic, const QString &label,
                                     bool unlock, bool config, bool shell,
                                     bool quiet, const QString &logSection,
                                     const QString &logStages, bool browse,
                                     bool interactive)
{
    if (m_running) {
        return false;
    }
    // The previous command's exit status must never leak into the next one
    // (the file system repair flow reads m_lastCommandOk after each command).
    m_lastCommandOk = false;
    // Every log entry is tagged at capture time; outside a running command
    // the register is plain application text.
    m_logEntryKind = LogEntry::Application;
    m_logEntryDiagnostic = QString::null;
    m_logEntryStages = QString::null;
    m_activeRepairStages.clear();
    if (m_smokeMode && m_runner->elevationNeedsPassword(0)) {
        // A modal password prompt would hang the headless smoke: fail with the
        // exact remedy instead. The unlock keyfile and the pending config
        // content file are discarded here too so no secret file ever outlives
        // the refused attempt.
        discardUnlockKeyfile();
        discardConfigContentFile();
        m_smokeMode = false;
        m_pendingConfigWrite = false;
        emit smokeFinished(false, tr(
            "elevation requires an interactive sudo password; run the smoke as "
            "root or after `sudo -S -v` with --elevate 'sudo -n'"));
        return false;
    }
    if (!administratorSessionActive()) {
        // Authorization is established on Host Maintenance entry, target
        // commit or the explicit Authorize control, never here: fail closed
        // with the exact remedy instead of opening a password prompt on Run All.
        appendLog(tr(
            "ERROR: administrator authorization is required for %1; press "
            "Authorize (Systems or Repair tab), enter Host Maintenance or commit "
            "a repair target to authorize this session.")
            .arg(label));
        if (!m_smokeMode && !quiet) {
            QMessageBox::warning(
                this, tr("Administrator authorization required"),
                tr(
                    "No administrator session is active for %1.\n\n"
                    "Press Authorize on the Systems or Repair tab to establish "
                    "the session now, or enter Host Maintenance / commit a repair "
                    "target on the Systems tab; diagnostics and repairs then "
                    "reuse the cached authorization.").arg(label),
                QMessageBox::Ok, QMessageBox::NoButton);
        }
        // Never leave a collected secret behind when authorization is missing.
        m_runner->setInputData(QByteArray());
        discardUnlockKeyfile();
        discardConfigContentFile();
        m_pendingConfigWrite = false;
        m_pendingConfigKey = QString::null;
        m_pendingConfigPath = QString::null;
        return false;
    }
    // A cached plain-sudo timestamp can expire between commands. Probe it
    // without blocking and fail closed with the Authorize remedy when it did;
    // otherwise the GUI would either prompt mid-command or hang on sudo's
    // password read.
    if (!sessionStillCurrent()) {
        appendLog(tr(
            "ERROR: the administrator session for %1 is no longer valid; the "
            "command was not started.").arg(label));
        if (!m_smokeMode && !quiet) {
            QMessageBox::warning(
                this, tr("Administrator authorization expired"),
                tr(
                    "The cached administrator authorization for %1 expired or "
                    "was refused.\n\nPress Authorize on the Systems or Repair "
                    "tab to re-establish the session, then run the command "
                    "again. No command was started.").arg(label),
                QMessageBox::Ok, QMessageBox::NoButton);
        }
        m_runner->setInputData(QByteArray());
        discardUnlockKeyfile();
        discardConfigContentFile();
        m_pendingConfigWrite = false;
        m_pendingConfigKey = QString::null;
        m_pendingConfigPath = QString::null;
        return false;
    }
    m_transcript = QString::null;
    m_pendingDiagnostic = diagnostic;
    m_pendingQuiet = quiet;
    // Round 30: the helper verb of this command (args.first()), so the mapped
    // invalidation path can tell the modifying copy verb from the read-only
    // copy-preview/browse-target verbs (the modern isFileCopy rule).
    m_pendingVerb = args.isEmpty() ? QString::null : args.first();
    // The single-key diagnostic key (empty for the combined `all` run) drives
    // the busy indicator's modern scope-aware "Running Host/Target diagnostic:
    // <title>" text.
    m_pendingDiagnosticKey = diagnostic ? logSection : QString::null;
    m_pendingUnlock = unlock;
    m_pendingConfig = config;
    m_pendingShell = shell;
    m_pendingBrowse = browse;
    m_pendingIdentity = identity();
    m_pendingLabel = label;
    // Tag the command's own header lines and its streamed output: diagnostic
    // runs are Diagnostic entries (the section key follows the stream's
    // `Diagnostic: <key>` markers; a single-key run seeds the requested key),
    // repair runs are Repair entries carrying the helper stage (or the Full
    // Repair stage set), config/shell/unlock traffic stays Application.
    if (diagnostic) {
        m_logEntryKind = LogEntry::Diagnostic;
        m_logEntryDiagnostic = logSection;
    } else if (!unlock && !config && !shell && !browse) {
        m_logEntryKind = LogEntry::Repair;
        m_logEntryStages = logStages;
        m_activeRepairStages = QStringList::split(QString::fromLatin1(" "), logStages, false);
    } else {
        m_activeRepairStages.clear();
    }
    // Item 4 (hide-until-complete): a diagnostic run clears the results pane
    // up front so the previous run's evidence never shows while the new run is
    // in progress; helperFinished() re-populates it once the run completes.
    // Round 31 (Debian Legacy #9): the cleared pane paints the in-field
    // greyed placeholder automatically (modern placeholder parity).
    if (diagnostic && m_rawView) {
        m_rawView->clear();
    }
    // Round 27 (Debian Legacy #7, modern round-12 at-initiation parity): an
    // individual diagnostic re-run invalidates ITS cached section at run
    // start (the per-section cache entry is removed and the stale block is
    // dropped from the merged report), so switching away and back mid-run
    // shows the section EMPTY until the fresh result lands — never the old
    // cache.  Round 45: the combined `all`/report re-run clears the FULL
    // register at run start (the same at-initiation rule); repairs invalidate
    // through the round-30 shared map at completion
    // (applyCommandTranscriptMapped), never here.
    if (diagnostic && !logSection.isEmpty()) {
        m_diagnosticSections.remove(logSection.lower());
        m_fullDiagnosticReport = removeEmbeddedDiagnosticSection(
            m_fullDiagnosticReport, logSection);
        refreshDiagnosticResultsView();
    } else if (diagnostic) {
        // Round 45 (modern parity; round-28 out-of-scope finding #1): the
        // combined `all`/report RE-RUN also clears the full register AT RUN
        // START — the same clear the round-28 cancel-initiation path applies
        // (m_diagnosticSections.clear() + the dropped merged report) — so the
        // old per-section content never stays visible mid-run. The completion
        // path (helperFinished, the empty-key branch) re-parses the fresh
        // transcript and re-appends every section.
        m_diagnosticSections.clear();
        m_fullDiagnosticReport = QString::null;
        refreshDiagnosticResultsView();
    }
    m_running = true;
    // A12-09: a fresh command starts with a clean cancel state; the popup's
    // Cancel button (when shown) arms for the run. Round 29 (Debian Legacy
    // #8): the diagnostic-cancel tracking flag resets with it — a re-run
    // after a cancelled diagnostic must land its fresh result normally.
    m_resultCancelRequested = false;
    m_diagnosticCancelInvalidated = false;
    updateResultCancelState();
    updateActionStates();
    updateBusyIndicator();

    // Interactive repair session setup (kernels/gpu menus): reserve the
    // mode-600 menu-answer file, mint a per-request id and hand the helper the
    // interactive-session environment.  Fails closed: an unreservable answer
    // file aborts the command instead of launching a menu the GUI cannot
    // answer.  Qt 3.3.7 QProcess cannot deliver stdin, so the raw answer
    // travels through this file (the unlock --key-file pattern).
    m_promptActive = false;
    m_promptRequestId = QString::null;
    if (interactive) {
        QString answerFile;
        if (!ensureMenuAnswerFile(&answerFile)) {
            appendLog(tr(
                "ERROR: the interactive menu-answer file could not be created "
                "in %1; the command was not started.").arg(m_logDirectory));
            m_running = false;
            updateActionStates();
            updateBusyIndicator();
            return false;
        }
        m_promptAnswerFile = answerFile;
        m_promptRequestId = QString::fromLatin1("legacy-")
            + QString::number(static_cast<unsigned long>(::getpid()))
            + QString::fromLatin1("-")
            + QString::number(++m_interactiveRequestCounter);
        QStringList interactiveEnv;
        interactiveEnv << QString::fromLatin1("BOOT_REPAIR_SESSION_PROTOCOL=1");
        interactiveEnv << (QString::fromLatin1("BOOT_REPAIR_SESSION_REQUEST=")
                           + m_promptRequestId);
        interactiveEnv << (QString::fromLatin1(
                               "BOOT_REPAIR_LEGACY_MENU_ANSWER_FILE=")
                           + answerFile);
        m_runner->setExtraEnvironment(interactiveEnv);
    } else {
        m_runner->setExtraEnvironment(QStringList());
    }

    // A10-05: the cancel token for this command.  The file lives in the
    // GUI's own private directory (0700) and is removed before the run - a
    // leftover from an interrupted session must never cancel the new command
    // immediately - and after it finishes.  HelperRunner passes it as
    // --cancel-file and cancel() touches it before the direct kill.
    ensureDirectory(QDir::homeDirPath()
                        + QString::fromLatin1("/.boot-repair-legacy"),
                    0700);
    m_cancelFilePath = QDir::homeDirPath()
        + QString::fromLatin1("/.boot-repair-legacy/cancel-request");
    ::unlink(m_cancelFilePath.local8Bit().data());
    m_runner->setCancelFile(m_cancelFilePath);

    QString elevation;
    m_runner->resolveElevation(&elevation);
    m_lastHelperDescription = elevation;
    updateElevationLabel();
    appendLog(QString::null);
    appendLog(QString::fromLatin1("=== %1 ===").arg(label));
    appendLog(QString::fromLatin1("helper: %1 (%2)").arg(m_runner->helperPath()).arg(elevation));
    // The command transcript never carries secrets. The guarded config-write
    // verb transports the edited content either through a private file
    // (--content-file <path>, B5) or - for the deprecated argv fallback - as
    // one argv element (the last one); the unlock carries its passphrase in
    // the --key-file argument: all are redacted before the line is logged.
    QStringList loggableArgs(args);
    if (loggableArgs.count() > 4
        && loggableArgs[0] == QString::fromLatin1("config-write")) {
        if (loggableArgs.count() > 5
            && loggableArgs[4] == QString::fromLatin1("--content-file")) {
            loggableArgs[5] =
                QString::fromLatin1("<config-write content path redacted>");
        } else {
            loggableArgs[loggableArgs.count() - 1] =
                QString::fromLatin1("<config-write content redacted>");
        }
    }
    if (loggableArgs.count() > 1) {
        for (int i = 0; i + 1 < static_cast<int>(loggableArgs.count()); ++i) {
            if (loggableArgs[i] == QString::fromLatin1("--key-file")) {
                loggableArgs[i + 1] =
                    QString::fromLatin1("<keyfile path redacted>");
            }
        }
    }
    appendLog(QString::fromLatin1("command: %1 %2")
                  .arg(m_runner->helperPath())
                  .arg(loggableArgs.join(QString::fromLatin1(" "))));
    if (!m_runner->run(args)) {
        // HelperRunner emitted finished(false, -1) synchronously.
        return false;
    }
    updateStatus();
    return true;
}

void LegacyMainWindow::helperLine(const QString &line)
{
    // Interactive repair menu records (legacy parity): while an interactive
    // session is active (m_promptAnswerFile set) the helper emits a
    // `PROMPT\t<id>\t<base64-menu>` record when the kernel/gpu menu waits for
    // an answer (no PUMP token).  The menu text is base64-decoded and shown
    // verbatim in the result pane (never interpreted or executed); the raw
    // records stay out of the transcript and the Logs register.  A mismatched
    // request id or a malformed record is dropped silently (fail closed).
    if (!m_promptAnswerFile.isEmpty()) {
        if (line.startsWith(QString::fromLatin1("PROMPT\t"))) {
            const QStringList fields =
                QStringList::split(QChar('\t'), line, true);
            if (fields.count() == 3 && fields[1] == m_promptRequestId) {
                const QByteArray decoded = legacyBase64Decode(fields[2]);
                const QString promptText = QString::fromUtf8(
                    decoded.data(), static_cast<int>(decoded.size()));
                // Prompt-flow contract (L1/L2 + D5): the prompt surface is the
                // Shell tab's inline pane while a shell command runs, the
                // repair result dialog's pane otherwise.  m_pendingShell picks
                // the live surface and the free-text vs numbered hint.
                const bool shell = m_pendingShell;
                const bool firstPrompt = !m_promptActive;
                QLineEdit *input = shell ? m_shellPromptInput : m_promptInput;
                QPushButton *send = shell ? m_shellPromptSend : m_promptSend;
                QLabel *hint = shell ? m_shellPromptHint : m_promptHint;
                if (!shell) {
                    // The repair menu text exists only in the PROMPT payload
                    // (it is never streamed), so it is appended to the
                    // transcript, the Logs register and the result pane.
                    // Round 70 (E1): the prompt append FORCES the bottom so
                    // the prompt line is always visible on arrival.
                    m_transcript += promptText;
                    m_transcript += QString::fromLatin1("\n");
                    appendLog(promptText);
                    appendResultOutput(promptText, /*forceBottom=*/true);
                } else {
                    // For a shell PROMPT the payload is the trailing output,
                    // which already streamed live into m_shellOutput
                    // (shell_pump_cleaned_byte echoes every byte to stdout); it
                    // is display-only and must never be re-appended, or the
                    // trailing question line would duplicate in the
                    // transcript.  Round 70 (E1): the arriving question line
                    // forces the pane to the bottom so it is always visible.
                    if (m_shellOutput) {
                        m_shellOutput->scrollToBottom();
                    }
                }
                m_promptActive = true;
                if (shell && m_tabs) {
                    // Round 27 (Debian Legacy #9, modern A8-02 parity): a
                    // shell question surfaces its tab so the armed inline
                    // prompt input is visible and can take keyboard focus the
                    // moment the PROMPT is shown.
                    m_tabs->setCurrentPage(3);
                }
                if (input) {
                    input->setEnabled(true);
                    // A re-emitted PROMPT re-arms the line without clearing a
                    // partially typed answer; a fresh prompt clears + focuses.
                    if (firstPrompt) {
                        input->clear();
                    }
                }
                if (send) {
                    send->setEnabled(true);
                }
                if (firstPrompt) {
                    // Round 72 (modern parity, supersedes the round-70 E4
                    // Send-focus): when the prompt arrives the activate focus
                    // moves to the INPUT line on BOTH surfaces (the shell
                    // inline pane and the full-repair terminal), so keyboard
                    // y/n POPULATES the field and returnPressed commits the
                    // answer through the Send path — the modern GUI's prompt
                    // behavior (src/MainWindow.cpp promptInput->setFocus()).
                    // A Qt 3.3.7 focus request delivered synchronously while
                    // the widgets are being enabled (and, for a shell, its tab
                    // raised) can be reverted by the focus handling that
                    // follows in the same event batch; re-assert the focus on
                    // the next event-loop turn so the widget really owns it
                    // (round 27, Debian Legacy #9).
                    if (input) {
                        input->setFocus();
                        QTimer::singleShot(0, input, SLOT(setFocus()));
                    }
                }
                if (hint) {
                    // The hint is dynamic by run kind: a numbered hint for the
                    // kernel/gpu menus, a free-text hint for the shell verbs
                    // (y/n or a password typed into the input line) and for
                    // the per-package-stage "CONFIRM PACKAGE CHANGE" y/n
                    // prompts (round 54, modern parity).
                    const bool confirmPrompt = !shell
                        && promptText.startsWith(QString::fromLatin1(
                            "CONFIRM PACKAGE CHANGE"));
                    hint->setText(shell || confirmPrompt
                        ? tr("Type the answer (Enter = Send)")
                        : tr("Number of the action (0 = Abort)"));
                    hint->show();
                }
            }
            return;
        }
    }
    m_transcript += line;
    m_transcript += QString::fromLatin1("\n");
    // Diagnostic runs: the stream's `Diagnostic: <key>` markers drive the
    // section tagging of the lines that follow (the marker itself carries the
    // new key, so a section filter shows its header too). Unknown keys leave
    // the current section untouched (fail closed).
    if (m_pendingDiagnostic
        && line.startsWith(QString::fromLatin1("Diagnostic: "))) {
        const QString key = line.mid(12).stripWhiteSpace();
        for (int i = 0; i < diagnosticSpecCount; ++i) {
            if (key == QString::fromLatin1(diagnosticSpecs[i].key)) {
                m_logEntryDiagnostic = key;
                break;
            }
        }
    }
    // Item 4 (hide-until-complete): the diagnostic transcript is buffered
    // while a run is in progress instead of being appended to the register
    // live, so partial per-section results never appear mid-run; the complete
    // run is flushed by helperFinished(). The tag is captured here (kind +
    // current section key) so the deferred flush keeps the per-line filter.
    if (m_pendingDiagnostic) {
        LogEntry entry;
        entry.text = line.isNull() ? QString::fromLatin1("") : line;
        entry.kind = m_logEntryKind;
        entry.diagnostic = m_logEntryDiagnostic;
        entry.stages = m_logEntryStages;
        // Round 36 (Etch #5): the arrival stamp is captured when the line
        // lands, even though the entry is only revealed at run completion —
        // the deferred flush must not stamp the whole buffered block with one
        // shared flush-time moment.
        entry.stamp = QDateTime::currentDateTime();
        m_bufferedDiagnosticEntries.push_back(entry);
    } else {
        appendLog(line);
    }
    // Stream into the modern-style repair result popup only while the popup's
    // own command is running. The popup is opened for repair/file-copy
    // commands, never for diagnostics, so a quiet diagnostic regeneration that
    // fires while a repair popup is still open must feed the primary session
    // register only (modern parity: the repair popup shows the repair log, the
    // primary log receives the regenerated diagnostic transcript). The wrap
    // mode is re-applied defensively so a toggle that flips through any path
    // mid-dialog is honored (cheap; the widget keeps its state).
    if (m_resultView && !m_pendingDiagnostic) {
        m_resultView->setWordWrap(m_logWrapEnabled
                                      ? QTextEdit::WidgetWidth
                                      : QTextEdit::NoWrap);
        appendResultOutput(line, /*forceBottom=*/false);
    }
    // Prompt-flow contract (L1/L2): the shell transcript streams live into the
    // Shell tab's own output pane (m_shellOutput) as the helper emits it, so a
    // question appears at its natural position in the transcript and can be
    // answered inline through the tab's prompt input line.  Round 27 (Debian
    // Legacy #8): the append goes through appendShellOutput() so the pane pegs
    // to the newest line (stick-to-bottom unless the user scrolled up).
    if (m_pendingShell && m_shellOutput) {
        appendShellOutput(line);
    }
}

// Emits the RAW answer for the waiting interactive repair menu.  The typed
// answer is written verbatim (plus a single newline) to the mode-600
// menu-answer file the helper polls (no ANSWER\t<id>\t<base64> framing, no
// base64); an empty input (cancel) writes just the newline so the helper fails
// closed.  The input surface is disabled until the next PROMPT record.
void LegacyMainWindow::sendPromptAnswer()
{
    // Single-popup repair plan: the inline per-device decision is a GUI-driven
    // question (not a helper PROMPT record), so the answer stays inside the
    // frontend and advances the plan instead of writing the helper's
    // menu-answer file.
    if (m_repairPlanInlineQuestion) {
        const QString answer = m_promptInput ? m_promptInput->text()
                                             : QString::null;
        if (m_promptInput) {
            m_promptInput->setEnabled(false);
            m_promptInput->clear();
        }
        if (m_promptSend) {
            m_promptSend->setEnabled(false);
        }
        if (m_promptHint) {
            m_promptHint->hide();
        }
        const QString echo = answer.isEmpty()
            ? QString::fromLatin1(">> [prompt cancelled]")
            : QString::fromLatin1(">> %1").arg(answer);
        appendLog(echo);
        // Round 70 (E1): the inline decision echo forces the bottom too (the
        // scroll hits the bottom after every send).
        appendResultOutput(echo, /*forceBottom=*/true);
        processRepairPlanInlineAnswer(answer);
        return;
    }
    if (!m_promptActive) {
        return;
    }
    // The live prompt surface follows the running kind: the Shell tab's inline
    // input line while a shell command runs, the repair result dialog's input
    // line otherwise.
    const bool shell = m_pendingShell;
    QLineEdit *input = shell ? m_shellPromptInput : m_promptInput;
    QPushButton *send = shell ? m_shellPromptSend : m_promptSend;
    QLabel *hint = shell ? m_shellPromptHint : m_promptHint;
    const QString answer = input ? input->text() : QString::null;
    if (!m_promptAnswerFile.isEmpty()) {
        writeMenuAnswerRecord(answer);
    }
    m_promptActive = false;
    if (input) {
        input->setEnabled(false);
        input->clear();
    }
    if (send) {
        send->setEnabled(false);
    }
    if (hint) {
        hint->hide();
    }
    // Echo the answer (or the cancel note) into the active pane, mirroring the
    // modern ">> <answer>" / ">> [prompt cancelled]" lines.  Round 27 (Debian
    // Legacy #8): the shell pane append goes through appendShellOutput() so
    // the echo is visible without manual scrolling.  Round 70 (E1): the
    // result-pane echo goes through appendResultOutput(forceBottom) so the
    // sent answer is always visible — the scroll hits the bottom after every
    // send.
    const QString echo = answer.isEmpty()
        ? QString::fromLatin1(">> [prompt cancelled]")
        : QString::fromLatin1(">> %1").arg(answer);
    appendLog(echo);
    if (shell) {
        appendShellOutput(echo);
    } else {
        appendResultOutput(echo, /*forceBottom=*/true);
    }
}

void LegacyMainWindow::helperFinished(bool ok, int exitCode)
{
    if (m_pendingLabel.isEmpty() && !m_running) {
        return;
    }
    // Record the exit status for the file system repair flow (which runs the
    // inspect and each per-device repair as separate commands).
    m_lastCommandOk = ok;
    // Item 4 (hide-until-complete): flush the buffered diagnostic transcript
    // before the completion line so the full run appears in the register in
    // order, then the "--- ... finished ---" line closes it (modern parity:
    // the diagnostic register is revealed only once the run completes).
    flushBufferedDiagnosticEntries();
    // A10-05: the finished command's cancel token is removed so the next run
    // starts clean (startCommand unlinks it again defensively).
    if (!m_cancelFilePath.isEmpty()) {
        ::unlink(m_cancelFilePath.local8Bit().data());
    }
    // Round 16 (ITEM 4e, modern parity): a run the user cancelled (through
    // the popup button or the window-close path, the latter visible only as
    // the helper's `Repair aborted:` evidence line) reports the ABORT status —
    // never a generic failure line.
    const bool aborted = m_resultCancelRequested
        || m_transcript.find(QString::fromLatin1("Repair aborted:")) >= 0;
    // Round 27 (Debian Legacy #10): the run's diagnostic key captured BEFORE
    // the kind reset below — the round-41 per-key user-run latch consumes it
    // (the Cycle-9 "Re-run Diagnostic" button state), and the reset (added
    // for the completion line's kind tagging) ran before the capture, so the
    // button could never reach the Re-run wording on the rig.
    const QString completedDiagnosticKey = m_logEntryDiagnostic;
    appendLog(QString::fromLatin1("--- %1 %2 (exit %3) ---")
                  .arg(m_pendingLabel)
                  .arg(aborted
                           ? QString::fromLatin1("stopped at your request")
                           : (ok ? QString::fromLatin1("finished")
                                 : QString::fromLatin1("failed")))
                  .arg(exitCode));
    // The completion line belongs to the command's kind; everything appended
    // afterwards (invalidation notes, unlock follow-ups, dialogs) is plain
    // application text again.
    m_logEntryKind = LogEntry::Application;
    m_logEntryDiagnostic = QString::null;
    m_logEntryStages = QString::null;

    // Modern-style result popup: final status line and Close enablement. While
    // the single-popup repair plan owns the session, Close stays disabled and
    // the status names the in-progress plan (finishRepairPlan() enables Close
    // and writes the final status at the very end).
    // A12-09: a command the user cancelled through the popup's Cancel button
    // reports the stopped-at-your-request wording (modern parity), never a
    // generic success/error line; the run is never reported successful.
    // Round 16 (ITEM 4e): the window-close cancel (no popup button) is
    // recognized through the helper's `Repair aborted:` evidence line.
    const bool resultCancelRequested = m_resultCancelRequested;
    const bool cancelled = resultCancelRequested
        || m_transcript.find(QString::fromLatin1("Repair aborted:")) >= 0;
    if (m_resultStatus && m_resultCloseButton) {
        if (m_repairPlanActive) {
            m_resultStatus->setText(cancelled
                ? tr(
                      "Repair stage stopped at your request. The plan is "
                      "finishing; review the streamed output below.")
                : (ok
                    ? tr(
                          "Repair stage completed successfully. The plan is "
                          "continuing; review the streamed output below.")
                    : tr(
                          "Repair stage stopped with an error. The plan is "
                          "finishing; review the streamed output below.")));
        } else {
            m_resultStatus->setText(cancelled
                ? tr(
                      "The privileged operation was stopped at your request. "
                      "Review the output before closing.")
                : (ok
                    ? tr(
                          "Privileged operation completed successfully. "
                          "Administrator authorization remains active for this "
                          "session.")
                    : tr(
                          "Privileged operation stopped with an error. "
                          "Administrator authorization remains active; review the "
                          "output before closing.")));
            m_resultCloseButton->setEnabled(true);
            if (m_resultDialog) {
                m_resultDialog->setCloseAllowed(true);
            }
        }
    }
    // A12-09: the finished command is no longer cancellable — the popup's
    // Cancel disarms (the next plan command re-arms it in startCommand).
    // Round 16 (ITEM 4e): remember an aborted plan command so
    // finishRepairPlan() writes the abort status (never "plan finished").
    if (m_repairPlanActive && cancelled) {
        m_repairPlanCancelled = true;
    }
    m_resultCancelRequested = false;
    updateResultCancelState();

    // A cached sudo authorization can expire or be refused between commands:
    // drop the cached decision so the explicit Authorize control re-establishes
    // it. Only a transcript that actually shows a sudo session problem resets
    // it (a mere "sudo:" substring in unrelated target log output must not).
    if (!ok && transcriptSuggestsAuthFailure(m_transcript)) {
        m_runner->resetElevation();
        appendLog(tr(
            "The sudo authorization expired or was refused; press Authorize "
            "(Systems or Repair tab) to re-establish the session."));
        updateElevationLabel();
    }

    const std::string transcript = toStd(m_transcript);
    const ParsedTranscript parsed = parseTranscript(transcript);
    const bool wasConfig = m_pendingConfig;
    const bool wasConfigWrite = m_pendingConfigWrite;
    const QString configKey = m_pendingConfigKey;
    const QString configPath = m_pendingConfigPath;
    // Round 50: capture the pending session-log delete state before the
    // per-command resets below (the delete completion runs after the run
    // state is reset, like the config follow-ups).
    const bool wasSessionLogDelete = !m_pendingSessionLogDeleteName.isEmpty();
    const QString sessionLogDeleteName = m_pendingSessionLogDeleteName;
    const QString sessionLogDeletePath = m_pendingSessionLogDeletePath;
    const bool wasShell = m_pendingShell;
    const bool wasBrowse = m_pendingBrowse;
    // Prompt-flow contract (L1/L2): close each shell transcript with its exit
    // marker so successive commands stay delimited in the shared pane (modern
    // "[exit N]" parity; the pane appends and never clears). The transcript
    // already streamed live into m_shellOutput through helperLine().
    if (wasShell && m_shellOutput) {
        appendShellOutput(QString::fromLatin1("[exit %1]").arg(exitCode));
    }
    const bool wasFullRepair = m_pendingLabel == tr("Full Repair");
    const QString finishedLabel = m_pendingLabel;
    QString configReadContent;
    // Round 30: whether the mapped invalidation below removed any cached
    // evidence (drives the per-command auto-refresh scheduling for individual
    // repairs; plan commands accumulate instead).
    bool mappedInvalidated = false;
    if (m_pendingDiagnostic) {
        // Round 27 (Debian Legacy #6): a cancelled diagnostic applies
        // fail-closed even when the helper exited 0 after its own clean
        // cancel path (the round-23/27 helper stops at the next probe
        // boundary and emits the stable msg:diagnostic-cancelled marker
        // with a normal exit): the partial evidence is never cached and the
        // raw transcript is shown instead.
        const bool diagOk = ok && !aborted
            && m_transcript.find(QString::fromLatin1(
                   "msg:diagnostic-cancelled")) < 0;
        const bool individualRun = !m_pendingDiagnosticKey.isEmpty();
        // Round 28 (Debian Legacy #3, modern per-section matrix): a failed or
        // cancelled INDIVIDUAL diagnostic no longer resets the capability
        // model — its invalidation is scoped to the doomed section only, and
        // the capability evidence survives exactly like the modern
        // markDiagnosticSectionsStale({key}) (which clears the capability
        // cache only when it is named).  The combined `all`/report failure
        // keeps the fail-closed whole-model reset.
        if (diagOk || !individualRun) {
            m_model.applyDiagnosticTranscript(toStd(m_pendingIdentity),
                                              transcript, diagOk);
        }
        if (!diagOk) {
            if (individualRun) {
                // Round 28 (Debian Legacy #3, modern matrix): invalidate ONLY
                // the doomed section (idempotent with the at-initiation
                // removals in startCommand/cancelDiagnosticOperation); every
                // other cached section and the capability model survive.
                // Show the raw transcript so the stop/failure is visible.
                m_diagnosticSections.remove(m_pendingDiagnosticKey.lower());
                m_fullDiagnosticReport = removeEmbeddedDiagnosticSection(
                    m_fullDiagnosticReport, m_pendingDiagnosticKey);
            } else {
                // Fail closed: a failed or cancelled combined `all`/report run
                // must not leave stale per-section results behind (the whole
                // register is cleared; the capability model was reset above).
                // Show the raw transcript so the failure is visible.
                m_diagnosticSections.clear();
                m_fullDiagnosticReport = QString::null;
            }
            // Round 29 (Debian Legacy #8, the cancel race): a run cancelled
            // through the Diagnostics page's Cancel button was invalidated AT
            // CANCEL INITIATION and the results pane was emptied there — the
            // completion must NOT re-show the doomed transcript afterwards
            // (that re-append re-populated the visible pane after the cancel
            // and made it look like the cancel did not go through). Refresh
            // the pane from the (invalidated) cache instead, so it stays
            // empty through the completion; the raw transcript remains in the
            // Logs tab. A genuine non-cancelled failure keeps the raw
            // transcript in the pane so the failure stays visible.
            if (m_diagnosticCancelInvalidated) {
                refreshDiagnosticResultsView();
            } else {
                m_rawView->setText(m_transcript);
            }
        } else if (m_pendingDiagnosticKey.isEmpty()) {
            // Combined `diagnose all` run: parse the per-section results and
            // show the SELECTED check's own section instead of the full
            // combined transcript (modern parity). The `report` aggregate
            // keeps the full combined report.
            m_fullDiagnosticReport = m_transcript;
            m_diagnosticSections = extractDiagnosticSections(m_transcript);
            refreshDiagnosticResultsView();
        } else {
            // Individual `diagnose <key>` run: strip the shared capability
            // preamble (the helper always includes it for a single-key run,
            // REPORT_SHARED_PREAMBLE=0) so the section shows only its own
            // content, then merge the fresh section back into the per-section
            // cache and the combined report (modern mergeEmbeddedDiagnosticSection
            // parity) so selecting away and back re-shows the fresh section
            // rather than the older combined one.
            QString sectionBody = m_transcript;
            QString ignoredPreamble;
            splitDiagnosticCapabilityPreamble(m_transcript, &sectionBody, &ignoredPreamble);
            m_rawView->setText(sectionBody);
            const QString key = m_pendingDiagnosticKey;
            if (!key.isEmpty()) {
                m_diagnosticSections.insert(key.lower(), sectionBody);
                m_fullDiagnosticReport = mergeEmbeddedDiagnosticSection(
                    m_fullDiagnosticReport, key, sectionBody);
                // Round 27 (Debian Legacy #7): the run-start invalidation
                // removed the doomed section's block from the merged report,
                // so the merge above found no embedded key (it returns the
                // bundle unchanged); append the fresh block so the combined
                // report stays complete after the re-run.
                if (m_fullDiagnosticReport.find(
                        QString::fromLatin1("Diagnostic: ") + key) < 0) {
                    if (!m_fullDiagnosticReport.stripWhiteSpace().isEmpty()) {
                        m_fullDiagnosticReport += QString::fromLatin1("\n");
                    }
                    m_fullDiagnosticReport += sectionBody;
                }
            }
        }
        if (diagOk) {
            // Cycle 9: remember the run's key+scope so the Results button can
            // offer "Re-run Diagnostic" while this cached result matches the
            // selected check (a scope change, an invalidation or a missing
            // cache flips it back to "Run Diagnostic").
            m_lastDiagnosticKey = completedDiagnosticKey;
            m_lastDiagnosticIdentity = m_pendingIdentity;
            // Round 41: latch the per-key USER-RUN state (modern parity) — an
            // INDIVIDUAL diagnostic the user ran successfully flips its Run
            // button to "Re-run Diagnostic" for the whole session. A combined
            // Run All (empty key) never flips individual buttons, and a
            // cancelled or failed run (diagOk false) reaches no latch — the
            // run produced no evidence. The latch needs no emptiness guard
            // beyond diagOk: a successful helper run that emitted no body at
            // all cannot happen through the real dispatch.
            if (!completedDiagnosticKey.isEmpty()) {
                m_userRunDiagnosticKeys.insert(completedDiagnosticKey.lower(),
                                               true);
            }
            updateFactView(parsed);
            mergeHelperDevices(parsed);
        }
    } else {
        // Round 30 (modern parity): the shared repair->diagnostic map drives
        // the invalidation instead of the round-21 whole-model staleness. A
        // plan command accumulates into the plan's single post-plan
        // invalidation; every other command applies its mapped set here.
        if (m_repairPlanActive && !wasShell && !wasConfig && !m_pendingUnlock
            && !wasBrowse) {
            accumulateRepairPlanInvalidation(parsed, ok, cancelled);
        } else {
            mappedInvalidated =
                applyCommandTranscriptMapped(parsed, ok, cancelled, wasShell);
        }
        reportChangeStatuses(parsed);
        // B7-4: the structured repair summary block after every repair
        // command (individual tool or Full Repair); unlock/config/shell and
        // the read-only browse listing stay out of the summary, and the
        // round-50 session-log delete carries no repair statuses either.
        if (!wasConfig && !m_pendingUnlock && !wasShell && !wasBrowse
            && !wasSessionLogDelete) {
            appendRepairSummaryBlock(parsed, ok, cancelled);
        }
        if (wasConfig && !wasConfigWrite && ok) {
            // The helper's `config-read` output carries a small read-only
            // preamble; the editor receives the file content only.
            configReadContent = m_transcript;
            const QString marker = tr("Inspection is read-only.\n\n");
            const int markerPos = configReadContent.find(marker);
            if (markerPos >= 0) {
                configReadContent = configReadContent.mid(markerPos + marker.length());
            }
        }
        if (wasConfig && wasConfigWrite && ok) {
            // A target configuration write can change mount, initramfs and
            // boot behavior: the complete cached target diagnostics are
            // invalidated, exactly like the modern Edit Target File dialog.
            m_model.reset();
            m_diagnosticSections.clear();
            m_fullDiagnosticReport = QString::null;
            appendLog(QString::fromLatin1("STALE DIAGNOSTICS: ")
                + tr("target configuration edited: %1. "
                     "Regenerate diagnostics before the next repair.")
                      .arg(configPath));
        }
        // Prompt-flow contract (L1/L2): the shell transcript already streamed
        // live into m_shellOutput (helperLine), including the inline question
        // and the ">> answer" echo, so the pane is NOT overwritten here with
        // the transcript (which would drop the echo).  The helper keeps its
        // own containment/timeout preflights.
    }

    const bool wasSmoke = m_smokeMode;
    const bool wasDiagnostic = m_pendingDiagnostic;
    const bool wasUnlock = m_pendingUnlock;
    const QString unlockDevice = m_unlockDevice;
    const QString unlockDisk = m_unlockDisk;
    m_running = false;
    // Round 16 (d): with the run over (completed OR cancelled) nothing is
    // cancellable anymore — the popup's Cancel must disarm HERE, after
    // m_running flips.  The earlier updateResultCancelState() call ran while
    // m_running was still true and left the button armed (the completion
    // never re-armed a subsequent pass).
    updateResultCancelState();
    m_pendingDiagnostic = false;
    m_pendingQuiet = false;
    m_pendingDiagnosticKey = QString::null;
    m_pendingUnlock = false;
    m_pendingConfig = false;
    m_pendingConfigWrite = false;
    m_pendingConfigKey = QString::null;
    m_pendingConfigPath = QString::null;
    m_pendingShell = false;
    m_pendingBrowse = false;
    m_pendingSessionLogDeleteName = QString::null;
    m_pendingSessionLogDeletePath = QString::null;
    m_pendingLabel = QString::null;
    // Round 29 (Debian Legacy #8): the diagnostic-cancel tracking flag is
    // consumed by the completion branch above (or was never set for this
    // command); reset it with the other per-command state so a later command
    // is never misread as cancelled.
    m_diagnosticCancelInvalidated = false;
    // The unlock keyfile and the config-write content file are deleted on
    // every completion path (the helper has already read the files by the
    // time the command returns and never deletes them; these unlinks are
    // best-effort and idempotent).
    discardUnlockKeyfile();
    discardConfigContentFile();
    // Interactive repair cleanup: a still-pending prompt is answered with an
    // empty raw answer (a lone newline) so the helper fails closed, the prompt
    // surface is disabled and the menu-answer file + interactive environment
    // are released for the next command.
    if (m_promptActive) {
        m_promptActive = false;
        // Disable BOTH prompt surfaces (kind-agnostic): the repair result
        // dialog's and the Shell tab's inline input line, whichever was armed.
        if (m_promptInput) {
            m_promptInput->setEnabled(false);
            m_promptInput->clear();
        }
        if (m_promptSend) {
            m_promptSend->setEnabled(false);
        }
        if (m_promptHint) {
            m_promptHint->hide();
        }
        if (m_shellPromptInput) {
            m_shellPromptInput->setEnabled(false);
            m_shellPromptInput->clear();
        }
        if (m_shellPromptSend) {
            m_shellPromptSend->setEnabled(false);
        }
        if (m_shellPromptHint) {
            m_shellPromptHint->hide();
        }
        if (!m_promptAnswerFile.isEmpty()) {
            writeMenuAnswerRecord(QString::null);
        }
    }
    discardMenuAnswerFile();
    m_promptRequestId = QString::null;
    m_runner->setExtraEnvironment(QStringList());
    updateActionStates();
    updateBusyIndicator();
    updateStatus();
    updateDriveDetails();

    // Item 5: after a diagnostic run finishes, re-read the freshly cached
    // capability evidence into the Settings table for the current scope, so
    // the table populates immediately (no manual host/target toggle needed).
    // Fail closed: an empty cache still shows the "No capability evidence
    // cached" placeholder, never a fabricated availability.
    if (wasDiagnostic) {
        refreshCapabilities();
    }

    // Item 2 (install refresh): after a successful install-host-package /
    // install-target-package helper run, re-read the freshly changed host/
    // target capability evidence into the Settings table so the installed row
    // flips from Missing to Available immediately (modern
    // MainWindow::installMissingCapability parity, which calls
    // refreshCapabilities() after every install). A failed install leaves the
    // row Missing, so only the success path refreshes.
    if (ok && !m_activeRepairStages.isEmpty()
        && (m_activeRepairStages.contains(QString::fromLatin1("install-host-package"))
            || m_activeRepairStages.contains(QString::fromLatin1("install-target-package")))) {
        refreshCapabilities();
    }

    // Config read/write follow-ups run after the run state is reset, so the
    // editor can start the guarded write without hitting the m_running guard.
    if (wasConfig && !wasConfigWrite) {
        if (ok) {
            openConfigEditor(configReadContent, configKey, configPath);
        } else {
            std::string error = unlockErrorLine(transcript);
            if (error.empty()) {
                error = toStd(m_transcript).substr(0, 400);
            }
            appendLog(tr("Configuration read failed: %1")
                          .arg(fromStd(error)));
            QMessageBox::warning(this, tr("Configuration unavailable"),
                                 tr(
                                     "The helper could not read %1:\n\n%2")
                                     .arg(configPath).arg(fromStd(error)),
                                 QMessageBox::Ok, QMessageBox::NoButton);
        }
    } else if (wasConfig && wasConfigWrite && !ok) {
        std::string error = unlockErrorLine(transcript);
        if (error.empty()) {
            error = toStd(m_transcript).substr(0, 400);
        }
        QMessageBox::warning(this, tr("Configuration write failed"),
                             QString::fromLatin1("%1").arg(fromStd(error)),
                             QMessageBox::Ok, QMessageBox::NoButton);
    }

    // Round 50: the helper-routed session-log delete completion.  The stable
    // `Session log delete: <name>|deleted|missing` marker is the success
    // evidence; any refusal (or a failed helper run) reports the failure and
    // keeps the file.  The list refresh removes the deleted file from the
    // Session logs pane (no dangling records), and the prior-log view state
    // resets when the deleted file was the one being viewed.
    if (wasSessionLogDelete) {
        const bool markerOk = m_transcript.find(
                QString::fromLatin1("Session log delete: ") + sessionLogDeleteName
                + QString::fromLatin1("|deleted")) >= 0
            || m_transcript.find(
                QString::fromLatin1("Session log delete: ") + sessionLogDeleteName
                + QString::fromLatin1("|missing")) >= 0;
        if (markerOk) {
            appendLog(tr("Deleted prior session log %1.").arg(sessionLogDeleteName));
            if (m_viewingPriorLog && m_priorLogPath == sessionLogDeletePath) {
                m_viewingPriorLog = false;
                m_priorLogPath = QString::null;
                m_priorLogLines.clear();
                if (m_priorLogBanner) {
                    m_priorLogBanner->hide();
                }
            }
        } else {
            QMessageBox::warning(this, tr("Delete session log"),
                                 tr("Unable to delete %1.").arg(sessionLogDeleteName),
                                 QMessageBox::Ok, QMessageBox::NoButton);
        }
        refreshSessionLogList();
        refreshLogView();
    }

    if (wasUnlock) {
        handleUnlockFinished(ok, transcript, unlockDevice, unlockDisk);
    }

    if (wasConfig && wasConfigWrite && ok) {
        maybeAutoRefreshDiagnostics(QString::fromLatin1(
            "the target configuration was edited"));
    }

    // The read-only browse listing drives the repair-system folder picker.
    if (wasBrowse) {
        handleFileCopyBrowseResult(transcript, ok);
    }

    // Modern finishFullRepairPlan parity: once after the last plan stage
    // (success OR aborted), the scope's cached diagnostics are invalidated
    // and one quiet Run All is scheduled when the auto-refresh setting, the
    // administrator session and the idle state allow it; it never prompts.
    // Round 30: the plan applies its accumulated mapped union in
    // finishRepairPlan(); individual repairs schedule the refresh only when
    // the map actually invalidated evidence (the old whole-model staleness
    // condition is gone).
    if (!wasDiagnostic && !wasConfig && !wasUnlock && !wasShell && !wasBrowse
        && !m_repairPlanActive) {
        if (wasFullRepair) {
            m_model.reset();
            m_diagnosticSections.clear();
            m_fullDiagnosticReport = QString::null;
            appendLog(tr(
                "Full Repair completed; cached diagnostics were invalidated "
                "for the current scope."));
            maybeAutoRefreshDiagnostics(QString::fromLatin1("Full Repair"));
        } else if (mappedInvalidated && !m_filesystemRepairFlowActive) {
            // The mapped invalidation above already removed the affected
            // sections (or the complete set); schedule the one regeneration.
            maybeAutoRefreshDiagnostics(finishedLabel);
        }
    }

    // The single-popup repair plan driver advances after every completed plan
    // command (deferred so a new command never starts inside the finished
    // signal's call stack). advanceRepairPlan() is a no-op outside a plan.
    if (m_repairPlanActive) {
        QTimer::singleShot(0, this, SLOT(advanceRepairPlan()));
    }

    // Item #7 cancel-and-switch: complete a Host Maintenance scope switch
    // that was deferred while a read-only diagnostic was being cancelled. The
    // cancelled diagnostic has already cleared the model and display cache;
    // re-running the toggle now performs the switch (and, on the enter path,
    // schedules the new scope's automatic diagnostic regeneration).
    if (m_scopeSwitchPending) {
        m_scopeSwitchPending = false;
        toggleHostMaintenance();
        return;
    }

    // Item #7 adjacent (commit-path cancel): complete a repair-target commit
    // that was deferred while a read-only diagnostic was being cancelled. The
    // cancelled diagnostic has already cleared the model and display cache;
    // re-running setRepairTarget() now performs the commit (and schedules the
    // new scope's automatic diagnostic regeneration).
    if (m_commitPending) {
        m_commitPending = false;
        setRepairTarget();
        return;
    }

    if (wasSmoke) {
        if (!ok) {
            m_smokeMode = false;
            emit smokeFinished(false, QString::fromLatin1("helper command failed (exit %1)").arg(exitCode));
            return;
        }
        if (wasDiagnostic) {
            int reported = 0;
            const std::vector<std::string> keys = capabilityKeys();
            for (std::size_t i = 0; i < keys.size(); ++i) {
                if (!m_model.state(keys[i]).empty()) {
                    ++reported;
                }
            }
            m_smokeDiagnoseOk = (reported == static_cast<int>(keys.size()));
        } else {
            m_smokeValidateOk = ok;
        }
        ++m_smokeStep;
        runSmokeStep();
    }
}

void LegacyMainWindow::handleUnlockFinished(bool ok, const std::string &transcript,
                                            const QString &device,
                                            const QString &disk)
{
    const std::string mapper = unlockMapper(transcript);
    const std::string unlockedRoot = unlockRoot(transcript);
    const std::string unlockedFstype = unlockRootFstype(transcript);
    const std::string unlockedUuid = unlockRootUuid(transcript);
    const bool authFailed = unlockAuthFailed(transcript);
    const QString targetDisk = disk.isEmpty() ? selectedDisk() : disk;
    QString status;
    if (ok && !mapper.empty()) {
        status = tr(
            "State: unlocked\nComponent: %1\nMapper: %2\n"
            "Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)\n"
            "Result: mapping opened for this recovery session.")
            .arg(device).arg(fromStd(mapper));
        // Item #1 (modern unlockSelectedTarget() parity): unlocking is a
        // pre-commit step and must NOT change the active scope or trigger a
        // diagnostics refresh against the unlocked target. Only when the
        // unlocked drive IS the committed repair target does the topology
        // change invalidate that target's cached evidence; otherwise the scope
        // (for example Host Maintenance) and its cached diagnostics stay
        // intact until the user explicitly commits the target with Select
        // Target. Diagnostics are never auto-refreshed here - they regenerate
        // at Select Target, mirroring the modern "unlocking must not run
        // diagnostics" rule.
        const bool unlockedCommittedTarget =
            m_targetCommitted && !m_committedDisk.isEmpty()
            && m_committedDisk == targetDisk;
        if (unlockedCommittedTarget) {
            m_model.reset();
            m_diagnosticSections.clear();
            m_fullDiagnosticReport = QString::null;
            appendLog(tr(
                "LUKS volume unlocked; the committed target topology changed, "
                "so cached diagnostics were invalidated. Run diagnostics again."));
        }
        scanDevices();
        // Cycle 11: record the helper-confirmed root AFTER the rescan (which
        // clears the previous session state) so Select Target can resolve
        // the mapped LV the read-only inventory cannot see. The helper's
        // UNLOCKED_ROOT is validated against the freshly-rescanned inventory:
        // when the probe mis-attributes the root (the Etch VG-scoping fallback
        // can name a running-host LV), the root is re-resolved from the
        // unlocked disk so the running host's root is never offered here.
        const QString reportedRoot = fromStd(unlockedRoot);
        const QString resolvedRoot = resolvedUnlockedRoot(targetDisk, reportedRoot);
        QString resolvedUuid = fromStd(unlockedUuid);
        if (!resolvedRoot.isEmpty() && resolvedRoot != reportedRoot) {
            appendLog(tr(
                "NOTE: the helper reported the unlocked root as %1, which does "
                "not belong to the unlocked disk; re-resolved to %2.")
                .arg(reportedRoot).arg(resolvedRoot));
            // The helper's root UUID belongs to the mis-attributed LV and is
            // dropped rather than shown for the corrected root.
            resolvedUuid = QString::null;
        }
        m_unlockedDisk = targetDisk;
        m_unlockedRoot = resolvedRoot;
        m_unlockedMapper = fromStd(mapper);
        m_unlockedRootUuid = resolvedUuid;
        if (m_unlockedRoot.isEmpty()) {
            m_unlockedDisk = QString::null;
        }
        if (!unlockedFstype.empty()) {
            appendLog(tr(
                "Unlocked root candidate: %1 (fstype %2).")
                .arg(m_unlockedRoot).arg(fromStd(unlockedFstype)));
        }
        // Cycle 12: show the freshly activated mapper and root under the
        // selected drive so the tree and the inventory-backed resolution
        // both see them.
        injectUnlockedMapperRows(targetDisk, m_unlockedMapper,
                                 m_unlockedRoot, fromStd(unlockedFstype),
                                 m_unlockedRootUuid);
        updateDriveDetails();
        updateActionStates();
    } else if (authFailed) {
        status = tr(
            "State: locked\nComponent: %1\nMethod: helper unlock (cryptsetup)\n"
            "Error: the passphrase was not accepted; retry offered.")
            .arg(device);
        appendLog(tr(
            "LUKS passphrase was not accepted for %1; offering retry.")
            .arg(device));
    } else {
        std::string error = unlockErrorLine(transcript);
        if (error.empty()) {
            error = "unlock failed (exit 1)";
        }
        status = tr(
            "State: locked\nComponent: %1\nMethod: helper unlock (cryptsetup)\n"
            "Error: %2")
            .arg(device).arg(fromStd(error));
        appendLog(status);
    }
    m_unlockStatusCache.insert(targetDisk, status);
    updateUnlockStatus();

    if (authFailed) {
        // Round 72 (modern parity): the retry decision is the modern
        // "Passphrase not accepted" confirmation — "Try Again" LEFT of
        // Cancel with Cancel as the default + focus (the modern
        // runConfirmDialog convention); the Qt3 QMessageBox Retry/Cancel
        // pair is platform-ordered and is gone.
        const bool answer = confirmWrapped(
            this, tr("Passphrase not accepted"),
            tr(
                "The LUKS passphrase was not accepted.\n\nTry again?"),
            /*cancelInsteadOfNo=*/true, tr("Try Again"), tr("Cancel"),
            kHiddenInputMaximumWidth, /*noIsDefault=*/true);
        if (answer) {
            m_unlockRetry = true;
            QTimer::singleShot(0, this, SLOT(runUnlock()));
        }
    }
}

void LegacyMainWindow::reportChangeStatuses(const ParsedTranscript &parsed)
{
    for (std::size_t i = 0; i < parsed.changeStatuses.size(); ++i) {
        const std::string &key = parsed.changeStatuses[i].first;
        const std::string &state = parsed.changeStatuses[i].second;
        if (!changeStatusIsUnchanged(state)) {
            // Round 30: the invalidation is the shared map's per-tool section
            // set now (the whole-model staleness is gone); keep the note
            // accurate.
            appendLog(tr("NOTE: repair status for %1 is not proven unchanged; the mapped cached diagnostic sections are invalidated.")
                          .arg(fromStd(key)));
        }
    }
}

namespace {

// The helper's own failure lines carry a [HH:MM:SS] log prefix; strip it when
// present and return the message of an ERROR line ("" for any other line).
QString errorMessageOfLine(const QString &line)
{
    QString text = line.stripWhiteSpace();
    // The legacy helper prefixes log() lines with [HH:MM:SS] (ten characters
    // plus the separating space).
    if (text.length() > 11 && text[0] == QChar('[') && text[9] == QChar(']')
        && text[10] == QChar(' ')) {
        text = text.mid(11).stripWhiteSpace();
    }
    if (text.startsWith(QString::fromLatin1("ERROR:"))) {
        return text.mid(6).stripWhiteSpace();
    }
    return QString::null;
}

// The repair stage the helper names in its final `ERROR: stage 'X' failed:`
// line, mapped to its tool key ("" when the transcript names none). Decorated
// labels ("grub (EFI follow-up)") belong to their base stage, exactly like
// MainWindow::repairFailureStageKey().
QString repairFailureStageKey(const QString &transcript)
{
    const QStringList lines = QStringList::split(QChar('\n'), transcript, false);
    QString stage;
    const QString prefix = QString::fromLatin1("stage '");
    const QString suffix = QString::fromLatin1("' failed:");
    for (QStringList::ConstIterator it = lines.begin(); it != lines.end(); ++it) {
        const QString message = errorMessageOfLine(*it);
        if (message.isEmpty()) {
            continue;
        }
        const int begin = message.find(prefix);
        if (begin < 0) {
            continue;
        }
        const int end = message.find(suffix, begin + prefix.length());
        if (end < 0) {
            continue;
        }
        stage = message.mid(begin + prefix.length(),
                            end - begin - prefix.length()).stripWhiteSpace();
    }
    if (stage.isEmpty()) {
        return QString::null;
    }
    const int decoration = stage.find(QString::fromLatin1(" ("));
    if (decoration > 0) {
        stage = stage.left(decoration);
    }
    return fromStd(repairToolKeyForStage(toStd(stage)));
}

// The last ERROR message of the transcript (the helper may retry internally
// and only the final failure line describes where the plan stopped), like
// MainWindow::shortRepairFailureReason().
QString shortRepairFailureReason(const QString &transcript)
{
    const QStringList lines = QStringList::split(QChar('\n'), transcript, false);
    QString last;
    for (QStringList::ConstIterator it = lines.begin(); it != lines.end(); ++it) {
        const QString message = errorMessageOfLine(*it);
        if (!message.isEmpty()) {
            last = message;
        }
    }
    return last;
}

// The failure reason the helper reported for one named stage (the last
// `ERROR: stage '<tool-key stage>' failed: <reason>` line for that stage,
// reason only), so an unrelated ERROR line elsewhere in the transcript can
// never be misattributed to this stage - exactly like
// MainWindow::repairStageFailureReason(). Empty when the transcript names no
// failure for the stage.
QString repairStageFailureReason(const QString &transcript, const QString &toolKey)
{
    const QStringList lines = QStringList::split(QChar('\n'), transcript, false);
    const QString prefix = QString::fromLatin1("stage '");
    const QString suffix = QString::fromLatin1("' failed:");
    QString reason;
    for (QStringList::ConstIterator it = lines.begin(); it != lines.end(); ++it) {
        const QString message = errorMessageOfLine(*it);
        if (message.isEmpty()) {
            continue;
        }
        const int begin = message.find(prefix);
        if (begin < 0) {
            continue;
        }
        const int end = message.find(suffix, begin + prefix.length());
        if (end < 0) {
            continue;
        }
        QString stage = message.mid(begin + prefix.length(),
                                    end - begin - prefix.length()).stripWhiteSpace();
        const int decoration = stage.find(QString::fromLatin1(" ("));
        if (decoration > 0) {
            stage = stage.left(decoration);
        }
        if (fromStd(repairToolKeyForStage(toStd(stage))) == toolKey) {
            reason = message.mid(end + suffix.length()).stripWhiteSpace();
        }
    }
    return reason;
}

// Fix: maps the helper's stable `reason:<key>[|param:<value>...]` token to
// human text for the legacy repair summary.  The helper's reason() prefixes
// the machine key with `reason:` (for example `reason:validation-read-only`),
// so the summary previously rendered the raw token verbatim.  This table maps
// the reason keys the legacy helper can emit on the Etch-reachable repair
// stages (validate, dpkg, fixbroken, aptupdate, upgrade, display, initramfs,
// grub, bootstack, filesystem) plus the package-manager-timeout failure
// reason, each to the modern kReasonSpecs wording lowercased and without the
// trailing period to match the summary's fragment style (and the smoke
// assertion `[-] Validate - validation is read-only`).  Unknown keys and
// legacy English prose pass through verbatim: display-only, never a gate.
struct LegacyReasonSpec {
    const char *key;
    const char *text;
};

const LegacyReasonSpec legacyReasonSpecs[] = {
    { "validation-read-only", QT_TRANSLATE_NOOP("LegacyMainWindow", "validation is read-only") },
    { "dpkg-nothing-pending", QT_TRANSLATE_NOOP("LegacyMainWindow", "dpkg reported no packages pending configuration") },
    { "fixbroken-simulated-no-changes", QT_TRANSLATE_NOOP("LegacyMainWindow", "the simulated fix-broken transaction proposed no package changes") },
    { "apt-lists-identical", QT_TRANSLATE_NOOP("LegacyMainWindow", "APT package lists are byte-identical and no repository index was fetched") },
    { "upgrade-simulated-no-changes", QT_TRANSLATE_NOOP("LegacyMainWindow", "the simulated upgrade transaction proposed no package changes") },
    { "display-already-correct", QT_TRANSLATE_NOOP("LegacyMainWindow", "default.target and display-manager.service were already correct") },
    { "display-already-correct-boot-tty", QT_TRANSLATE_NOOP("LegacyMainWindow", "the display manager configuration is already correct, but the last boot stopped at a text console (the display manager did not start); the failure is likely elsewhere (driver or service crash)") },
    { "openrc-runlevel-enabled", QT_TRANSLATE_NOOP("LegacyMainWindow", "the OpenRC default runlevel already enabled %1") },
    { "initramfs-identical", QT_TRANSLATE_NOOP("LegacyMainWindow", "rebuilt initramfs images are byte-identical") },
    { "grub-cfg-identical", QT_TRANSLATE_NOOP("LegacyMainWindow", "grub.cfg is byte-identical") },
    { "grub-cfg-grubenv-identical", QT_TRANSLATE_NOOP("LegacyMainWindow", "grub.cfg and grubenv are byte-identical") },
    { "fedora-grub-boot-identical", QT_TRANSLATE_NOOP("LegacyMainWindow", "MBR, BIOS boot partition and i386-pc modules are byte-identical") },
    { "bootstack-grub2-identical", QT_TRANSLATE_NOOP("LegacyMainWindow", "initramfs and GRUB2 artifacts are byte-identical") },
    { "bootstack-efi-identical", QT_TRANSLATE_NOOP("LegacyMainWindow", "initramfs, EFI/UKI and GRUB artifacts are byte-identical") },
    { "pacman-no-changes", QT_TRANSLATE_NOOP("LegacyMainWindow", "the pacman transaction reported no packages to install, upgrade or remove") },
    { "apk-no-changes", QT_TRANSLATE_NOOP("LegacyMainWindow", "apk simulated no package changes and the package state is byte-identical") },
    { "dnf5-no-changes", QT_TRANSLATE_NOOP("LegacyMainWindow", "dnf5 simulated no package changes and the rpm database is byte-identical") },
    { "dnf5-fixbroken-clean", QT_TRANSLATE_NOOP("LegacyMainWindow", "no missing package files were detected; dnf5 check is reported as evidence") },
    { "dnf5-metadata-identical", QT_TRANSLATE_NOOP("LegacyMainWindow", "the dnf5 repository metadata cache is byte-identical") },
    { "filesystem-check-clean", QT_TRANSLATE_NOOP("LegacyMainWindow", "the read-only check reported no errors") },
    { "package-manager-timeout", QT_TRANSLATE_NOOP("LegacyMainWindow", "the package manager did not respond within the safety limit while %1 and was stopped; no package change was applied") }
};
const int legacyReasonSpecCount = sizeof(legacyReasonSpecs) / sizeof(legacyReasonSpecs[0]);

// Resolves a `reason:<key>[|param:<value>...]` token to its human text and
// substitutes the positional `%1`..`%n` placeholders.  Non-`reason:` text and
// unknown keys pass through verbatim (fail open on display only, never on
// gating).
QString localizedReason(const QString &raw)
{
    const QString prefix = QString::fromLatin1("reason:");
    if (!raw.startsWith(prefix)) {
        return raw;
    }
    const QStringList tokens = QStringList::split(QChar('|'), raw, false);
    const QString key = tokens.first().mid(prefix.length());
    QStringList params;
    QStringList::ConstIterator it = tokens.begin();
    ++it;
    for (; it != tokens.end(); ++it) {
        if ((*it).startsWith(QString::fromLatin1("param:"))) {
            params.append((*it).mid(6));
        }
    }
    for (int i = 0; i < legacyReasonSpecCount; ++i) {
        if (key != QString::fromLatin1(legacyReasonSpecs[i].key)) {
            continue;
        }
        QString text = qApp->translate("LegacyMainWindow", legacyReasonSpecs[i].text);
        for (int p = 0; p < static_cast<int>(params.size()) && p < 4; ++p) {
            text.replace(QString::fromLatin1("%") + QString::number(p + 1), params[p]);
        }
        return text;
    }
    return raw;
}

// Maps the helper's capability-unavailable reason keys (the
// `Repair tool <key>: unavailable|reason:<key>[|param:...]` detail) to the
// modern kReasonSpecs wording, byte-identical, so each repair tool's
// "Unavailable:" reason reads 1:1 with the modern GUI.  Unlike the
// `legacyReasonSpecs` change-status fragments above (lowercased, no trailing
// period, for the `[-] <stage> - <reason>` summary), this table carries the
// full modern sentence for every reason key the legacy helper can emit for the
// 15 capability keys (validate, filesystem, dpkg, fixbroken, aptupdate,
// upgrade, dkms, display, gpu, initramfs, efi, grub, extlinux, bootstack,
// kernels).  Non-`reason:` text and unknown keys still pass through verbatim
// (fail open on display only, never on gating).
struct LegacyCapabilityReasonSpec {
    const char *key;
    const char *text;
};

const LegacyCapabilityReasonSpec legacyCapabilityReasonSpecs[] = {
    { "akmods-no-build-tree", QT_TRANSLATE_NOOP("LegacyMainWindow", "akmods preflight found no build tree for installed kernel %1; install the matching kernel-devel packages and retry.") },
    { "akms-headers-newer", QT_TRANSLATE_NOOP("LegacyMainWindow", "akms preflight found no build tree for kernel %1: the installed %2 headers are newer (version %3). Reboot into the kernel matching the installed headers, or install headers matching %1 if they are still available.") },
    { "akms-no-build-tree", QT_TRANSLATE_NOOP("LegacyMainWindow", "akms preflight found no build tree for installed kernel %1; install the matching %2 headers package (or boot a kernel that matches the installed headers) and retry.") },
    { "alpine-bootstack-extlinux-disabled", QT_TRANSLATE_NOOP("LegacyMainWindow", "Alpine uses OpenRC, mkinitfs and syslinux/extlinux; boot-stack reconciliation is not enabled (run the initramfs and extlinux stages separately).") },
    { "alpine-bootstack-grub-disabled", QT_TRANSLATE_NOOP("LegacyMainWindow", "Alpine boot-stack reconciliation is not enabled (run the initramfs and GRUB stages separately).") },
    { "alpine-dkms-no-build-tree", QT_TRANSLATE_NOOP("LegacyMainWindow", "Alpine DKMS preflight found no build tree for installed kernel %1; install the matching headers and retry.") },
    { "alpine-nvidia-unsupported", QT_TRANSLATE_NOOP("LegacyMainWindow", "Alpine's main repository has no proprietary NVIDIA DKMS driver; the GPU driver swap is not available on this target.") },
    { "alpine-syslinux-efi-unsupported", QT_TRANSLATE_NOOP("LegacyMainWindow", "Alpine syslinux-EFI boot detected (EFI/syslinux/syslinux.efi); guarded repair is not implemented.") },
    { "aptupdate-not-on-alpine", QT_TRANSLATE_NOOP("LegacyMainWindow", "Standalone APK metadata refresh is not available on Alpine; use Upgrade installed packages for one guarded apk transaction.") },
    { "aptupdate-not-on-arch", QT_TRANSLATE_NOOP("LegacyMainWindow", "Standalone APT metadata refresh is not available on Arch; use Upgrade installed packages for one full pacman transaction.") },
    { "arch-bootstack-needs-efi", QT_TRANSLATE_NOOP("LegacyMainWindow", "Arch boot-stack reconciliation requires available initramfs, GRUB and EFI repair prerequisites.") },
    { "arch-dkms-no-build-tree", QT_TRANSLATE_NOOP("LegacyMainWindow", "Arch DKMS preflight found no build tree for installed kernel %1; install the matching headers and retry.") },
    { "booster-unsupported", QT_TRANSLATE_NOOP("LegacyMainWindow", "The initramfs backend is booster, which the current repair implementation does not handle.") },
    { "bootstack-needs-initramfs-grub", QT_TRANSLATE_NOOP("LegacyMainWindow", "Requires available initramfs and GRUB repair prerequisites.") },
    { "dkms-no-build-tree", QT_TRANSLATE_NOOP("LegacyMainWindow", "DKMS preflight found no build tree for installed kernel %1; install the matching kernel-devel packages and retry.") },
    { "dnf4-unsupported", QT_TRANSLATE_NOOP("LegacyMainWindow", "dnf4 is not supported by the guarded rpm backend.") },
    { "dpkg-not-on-alpine", QT_TRANSLATE_NOOP("LegacyMainWindow", "Alpine uses apk; dpkg configuration is not available on Alpine.") },
    { "dpkg-not-on-arch", QT_TRANSLATE_NOOP("LegacyMainWindow", "dpkg configuration is not available on Arch; use the Arch package transaction stages instead.") },
    { "dpkg-not-on-fedora", QT_TRANSLATE_NOOP("LegacyMainWindow", "Fedora uses rpm/dnf; dpkg configuration is not available.") },
    { "efi-stub-needs-efibootmgr", QT_TRANSLATE_NOOP("LegacyMainWindow", "EFI-stub entry repair requires efibootmgr in the recovery host.") },
    { "efi-stub-needs-uefi", QT_TRANSLATE_NOOP("LegacyMainWindow", "EFI-stub boot requires UEFI firmware; the recovery host booted in legacy BIOS mode.") },
    { "gpu-driver-unknown", QT_TRANSLATE_NOOP("LegacyMainWindow", "No expected GPU driver could be derived from hardware, package recommendations or the OEM flavour.") },
    { "gpu-hybrid-ambiguous", QT_TRANSLATE_NOOP("LegacyMainWindow", "Multiple GPU devices were detected with no explicit choice; the GPU driver swap is refused rather than guessing the primary GPU.") },
    { "grub-bios-no-efi", QT_TRANSLATE_NOOP("LegacyMainWindow", "GRUB detected on legacy BIOS; no EFI boot path is available.") },
    { "incomplete-dpkg", QT_TRANSLATE_NOOP("LegacyMainWindow", "The target dpkg database or executable is incomplete.") },
    { "invalid-grubenv", QT_TRANSLATE_NOOP("LegacyMainWindow", "The grubenv file is missing or not a valid GRUB environment block.") },
    { "legacy-bios-no-efi", QT_TRANSLATE_NOOP("LegacyMainWindow", "legacy BIOS target; no EFI boot path is available.") },
    { "missing-akmods", QT_TRANSLATE_NOOP("LegacyMainWindow", "akmods is not installed in the target system.") },
    { "missing-apk", QT_TRANSLATE_NOOP("LegacyMainWindow", "apk is not installed in the target.") },
    { "missing-apk-installed-db", QT_TRANSLATE_NOOP("LegacyMainWindow", "The target apk installed database is missing.") },
    { "missing-apk-world", QT_TRANSLATE_NOOP("LegacyMainWindow", "The target apk world file is missing.") },
    { "missing-apt-get", QT_TRANSLATE_NOOP("LegacyMainWindow", "apt-get is not installed in the target.") },
    { "missing-dkms-alpine", QT_TRANSLATE_NOOP("LegacyMainWindow", "DKMS is not installed in the Alpine target system.") },
    { "missing-dkms-arch", QT_TRANSLATE_NOOP("LegacyMainWindow", "DKMS is not installed in the Arch target system.") },
    { "missing-dkms", QT_TRANSLATE_NOOP("LegacyMainWindow", "DKMS is not installed in the target.") },
    { "missing-dnf5", QT_TRANSLATE_NOOP("LegacyMainWindow", "dnf5 is not installed in the target system.") },
    { "missing-dpkg", QT_TRANSLATE_NOOP("LegacyMainWindow", "dpkg is not installed in the target.") },
    { "missing-dracut-dir", QT_TRANSLATE_NOOP("LegacyMainWindow", "The dracut generator directory is missing from the target.") },
    { "missing-dracut", QT_TRANSLATE_NOOP("LegacyMainWindow", "dracut is not installed in the target system.") },
    { "missing-graphical-target", QT_TRANSLATE_NOOP("LegacyMainWindow", "graphical.target is missing from the target.") },
    { "missing-grub2-install", QT_TRANSLATE_NOOP("LegacyMainWindow", "grub2-install is not installed in the target.") },
    { "missing-grub2-mkconfig", QT_TRANSLATE_NOOP("LegacyMainWindow", "grub2-mkconfig is not installed in the target.") },
    { "missing-grub-config", QT_TRANSLATE_NOOP("LegacyMainWindow", "%1 is missing.") },
    { "missing-grub-config-tooling", QT_TRANSLATE_NOOP("LegacyMainWindow", "Requires GRUB configuration tooling.") },
    { "missing-grub-efi-alpine", QT_TRANSLATE_NOOP("LegacyMainWindow", "grub-efi is not installed in the Alpine target.") },
    { "missing-grub-efi-modules", QT_TRANSLATE_NOOP("LegacyMainWindow", "The x86_64-efi GRUB module directory is missing from the Alpine target.") },
    { "missing-grub-generator", QT_TRANSLATE_NOOP("LegacyMainWindow", "Neither grub-mkconfig nor update-grub is installed in the target system.") },
    { "missing-grub-install-alpine", QT_TRANSLATE_NOOP("LegacyMainWindow", "grub-install is not installed in the Alpine target.") },
    { "missing-grub-install", QT_TRANSLATE_NOOP("LegacyMainWindow", "grub-install missing.") },
    { "missing-grub-package-alpine", QT_TRANSLATE_NOOP("LegacyMainWindow", "The grub package is not installed in the Alpine target.") },
    { "missing-grub-zfs-module", QT_TRANSLATE_NOOP("LegacyMainWindow", "The GRUB ZFS module (zfs.mod) is not installed in the target.") },
    { "missing-initramfs-tools", QT_TRANSLATE_NOOP("LegacyMainWindow", "update-initramfs/mkinitramfs are not installed in the target.") },
    { "missing-lsinitrd", QT_TRANSLATE_NOOP("LegacyMainWindow", "lsinitrd is not installed in the target system; dracut image verification is unavailable.") },
    { "missing-mkinitcpio", QT_TRANSLATE_NOOP("LegacyMainWindow", "mkinitcpio is not installed in the target system.") },
    { "missing-mkinitfs-conf", QT_TRANSLATE_NOOP("LegacyMainWindow", "The target has no mkinitfs configuration.") },
    { "missing-mkinitfs", QT_TRANSLATE_NOOP("LegacyMainWindow", "mkinitfs is not installed in the target system.") },
    { "missing-pacman-conf", QT_TRANSLATE_NOOP("LegacyMainWindow", "The target has no /etc/pacman.conf; refusing a package transaction.") },
    { "missing-pacman-db", QT_TRANSLATE_NOOP("LegacyMainWindow", "The target pacman database directory is missing.") },
    { "missing-pacman", QT_TRANSLATE_NOOP("LegacyMainWindow", "pacman is not installed in the target.") },
    { "missing-rpm-database", QT_TRANSLATE_NOOP("LegacyMainWindow", "The target RPM database is missing.") },
    { "missing-rpm", QT_TRANSLATE_NOOP("LegacyMainWindow", "rpm is not installed in the target system.") },
    { "missing-syslinux-package", QT_TRANSLATE_NOOP("LegacyMainWindow", "The syslinux package is not installed according to the detected package manager.") },
    { "missing-tuxedo-uki-builder", QT_TRANSLATE_NOOP("LegacyMainWindow", "TUXEDO UKI builder create_boot_uki_base.sh is not installed in the target.") },
    { "missing-update-extlinux", QT_TRANSLATE_NOOP("LegacyMainWindow", "update-extlinux is not installed in the target.") },
    { "missing-zfs-initramfs-hook", QT_TRANSLATE_NOOP("LegacyMainWindow", "The zfs-initramfs hook is not installed in the target (no /usr/share/initramfs-tools/hooks/zfs).") },
    { "no-alpine-efi-path", QT_TRANSLATE_NOOP("LegacyMainWindow", "No EFI boot path was detected for the Alpine target (bootloader backend %1).") },
    { "no-alpine-kernels", QT_TRANSLATE_NOOP("LegacyMainWindow", "No installed Alpine kernels were found under target /boot.") },
    { "no-apk-repositories", QT_TRANSLATE_NOOP("LegacyMainWindow", "The target has no configured apk repositories.") },
    { "no-apt-sources", QT_TRANSLATE_NOOP("LegacyMainWindow", "The target has no configured APT sources to refresh.") },
    { "no-arch-esp", QT_TRANSLATE_NOOP("LegacyMainWindow", "No EFI System Partition was identified for the selected Arch target.") },
    { "no-boot-filesystem", QT_TRANSLATE_NOOP("LegacyMainWindow", "No /boot (and no resolved EFI System Partition) is visible for the selected scope.") },
    { "no-display-manager", QT_TRANSLATE_NOOP("LegacyMainWindow", "No supported display manager was detected in the target.") },
    { "no-display-manager-unit", QT_TRANSLATE_NOOP("LegacyMainWindow", "No supported display manager unit is installed in the target.") },
    { "no-dnf-repositories", QT_TRANSLATE_NOOP("LegacyMainWindow", "The target has no enabled dnf repositories.") },
    { "no-dnf-repositories-to-refresh", QT_TRANSLATE_NOOP("LegacyMainWindow", "The target has no enabled dnf repositories to refresh.") },
    { "no-dracut-kernels", QT_TRANSLATE_NOOP("LegacyMainWindow", "No installed dracut kernels were found under target /boot.") },
    { "no-efi-stub-initramfs", QT_TRANSLATE_NOOP("LegacyMainWindow", "No EFI-stub initramfs image (initramfs-*) is present at the EFI System Partition root.") },
    { "no-efi-stub-kernel", QT_TRANSLATE_NOOP("LegacyMainWindow", "No EFI-stub kernel image (vmlinuz-*) is present at the EFI System Partition root.") },
    { "no-esp-candidate", QT_TRANSLATE_NOOP("LegacyMainWindow", "No EFI System Partition candidate on the selected disk.") },
    { "no-esp-derivable", QT_TRANSLATE_NOOP("LegacyMainWindow", "No EFI System Partition is present or derivable on the selected disk.") },
    { "no-extlinux-config", QT_TRANSLATE_NOOP("LegacyMainWindow", "No extlinux/syslinux configuration was detected in the target.") },
    { "no-filesystem-check-tool", QT_TRANSLATE_NOOP("LegacyMainWindow", "No supported file system check tool is installed in the recovery environment.") },
    { "no-gpu-hardware", QT_TRANSLATE_NOOP("LegacyMainWindow", "No class-03 GPU device and no target journal/module evidence were found; there is no GPU driver stack to repair.") },
    { "no-initramfs-backend", QT_TRANSLATE_NOOP("LegacyMainWindow", "No supported initramfs backend (mkinitfs, mkinitcpio, dracut or initramfs-tools) was detected.") },
    { "no-kernel-inventory", QT_TRANSLATE_NOOP("LegacyMainWindow", "No installed versioned kernel packages were found; the newest kernel cannot be identified.") },
    { "no-mkinitcpio-kernels", QT_TRANSLATE_NOOP("LegacyMainWindow", "No installed kernel module directories were found for mkinitcpio.") },
    { "no-openrc-display-manager", QT_TRANSLATE_NOOP("LegacyMainWindow", "No supported OpenRC display manager service is installed in the target.") },
    { "no-package-backend", QT_TRANSLATE_NOOP("LegacyMainWindow", "No guarded package-manager backend that can both answer installed-state and install packages was detected (detected: %1).") },
    { "no-package-manager-for-syslinux", QT_TRANSLATE_NOOP("LegacyMainWindow", "No detected package manager can verify the syslinux package.") },
    { "no-package-manager", QT_TRANSLATE_NOOP("LegacyMainWindow", "No guarded package-manager backend was detected (detected: %1).") },
    { "no-resolvable-scope-filesystems", QT_TRANSLATE_NOOP("LegacyMainWindow", "The selected scope's root, /boot, ESP and /home filesystems could not be resolved.") },
    { "no-service-manager", QT_TRANSLATE_NOOP("LegacyMainWindow", "No supported service manager (systemd or OpenRC) was detected in the target.") },
    { "no-supported-filesystem-type", QT_TRANSLATE_NOOP("LegacyMainWindow", "The selected scope has no supported file system type for a read-only check.") },
    { "not-grub-bootloader", QT_TRANSLATE_NOOP("LegacyMainWindow", "The detected bootloader is %1; GRUB is not the selected bootloader.") },
    { "nvidia-dkms-requires-network", QT_TRANSLATE_NOOP("LegacyMainWindow", "The proprietary NVIDIA driver needs network access to fetch the package set; this offline target has no usable APT sources.") },
    { "running-kernel-not-identified", QT_TRANSLATE_NOOP("LegacyMainWindow", "The running kernel release could not be determined (uname -r is empty).") },
    { "syslinux-bios-no-efi", QT_TRANSLATE_NOOP("LegacyMainWindow", "syslinux/extlinux (BIOS) boot detected; no EFI boot path is available.") },
    { "sysvinit-display-unsupported", QT_TRANSLATE_NOOP("LegacyMainWindow", "A sysvinit display-manager script was detected; offline repair is not implemented for sysvinit.") },
    { "unimplemented-backend-stage", QT_TRANSLATE_NOOP("LegacyMainWindow", "Backend %1 has no guarded implementation for stage %2.") },
    { "unresolvable-zfs-root", QT_TRANSLATE_NOOP("LegacyMainWindow", "The running host ZFS root pool is not resolvable (zpool is not installed or the pool is not imported).") },
    { "virtual-gpu-no-discrete-driver", QT_TRANSLATE_NOOP("LegacyMainWindow", "The detected GPU is a virtual/para-virtual display device (Virtio, QEMU/Bochs, Cirrus, VirtualBox, VMware, QXL or Hyper-V) with no discrete driver to swap or rebuild; the GPU driver swap is not available.") },
};
const int legacyCapabilityReasonSpecCount =
    sizeof(legacyCapabilityReasonSpecs) / sizeof(legacyCapabilityReasonSpecs[0]);

QString localizedCapabilityReason(const QString &raw)
{
    const QString prefix = QString::fromLatin1("reason:");
    if (!raw.startsWith(prefix)) {
        return raw;
    }
    const QStringList tokens = QStringList::split(QChar('|'), raw, false);
    const QString key = tokens.first().mid(prefix.length());
    QStringList params;
    QStringList::ConstIterator it = tokens.begin();
    ++it;
    for (; it != tokens.end(); ++it) {
        if ((*it).startsWith(QString::fromLatin1("param:"))) {
            params.append((*it).mid(6));
        }
    }
    for (int i = 0; i < legacyCapabilityReasonSpecCount; ++i) {
        if (key != QString::fromLatin1(legacyCapabilityReasonSpecs[i].key)) {
            continue;
        }
        QString text = qApp->translate("LegacyMainWindow",
                                       legacyCapabilityReasonSpecs[i].text);
        for (int p = 0; p < static_cast<int>(params.size()) && p < 4; ++p) {
            text.replace(QString::fromLatin1("%") + QString::number(p + 1),
                         params[p]);
        }
        return text;
    }
    return raw;
}

} // namespace

// B7-4: the structured repair summary block, appended after every repair
// command (individual tool or Full Repair) and built exclusively from the
// parsed `Repair change status` lines.  A requested stage with no parsed
// status counts as failed unless the helper reported a clean read-only
// outcome (nothing is fabricated).  The block is bracketed by
// `==== REPAIR ====` lines, tagged with the Repair kind so the Repairs
// log filter shows it, and uses ASCII markers ([OK]/[FAIL]/[-]) because
// Etch's fonts garble the modern check/cross/bullet glyphs.
void LegacyMainWindow::appendRepairSummaryBlock(const ParsedTranscript &parsed,
                                                bool commandOk, bool cancelled)
{
    // Per-stage outcomes from the transcript, in first-seen order.
    QStringList seenStages;
    QMap<QString, QString> outcomes;
    for (std::size_t i = 0; i < parsed.changeStatuses.size(); ++i) {
        const QString key = fromStd(parsed.changeStatuses[i].first);
        const QString state = fromStd(parsed.changeStatuses[i].second);
        if (!outcomes.contains(key)) {
            seenStages.append(key);
        }
        outcomes.insert(key, state);
    }
    QStringList expected = m_activeRepairStages;
    if (expected.isEmpty()) {
        expected = seenStages;
    }

    // The read-only file system check counts as a clean outcome even without
    // a change-status line: `File system check summary: ... issues=0`.
    const bool cleanFsInspect = m_transcript.find(QString::fromLatin1("File system check summary:")) >= 0
        && m_transcript.find(QString::fromLatin1("issues=0")) >= 0;

    // Host-repair summary fix: a stage's result comes from its own
    // `Repair change status` line, never from the overall command exit code.
    // `unchanged|<reason>` is a proven no-op, `changed` is a stage that
    // completed (the helper only reports it after the stage succeeded), and
    // only the stage the helper names in its final `ERROR: stage 'X' failed:`
    // line owns the failure; requested stages after it never ran. This
    // mirrors MainWindow's Full Repair stage categorization.
    // Round 16 (ITEM 4e, modern parity): a CANCELLED run reports the abort
    // status, not a per-stage failure — completed stages keep their statuses,
    // the aborted and later stages are "not run (stopped at your request)",
    // and no stage is attributed a failure.
    const QString failedStageKey = repairFailureStageKey(m_transcript);
    QString failureReason = localizedReason(shortRepairFailureReason(m_transcript));
    if (failureReason.isEmpty()) {
        failureReason = QString::fromLatin1("the privileged helper reported a failure");
    }

    int successful = 0;
    int failed = 0;
    int unchanged = 0;
    int notRun = 0;
    bool failureAttributed = false;
    QStringList handledKeys;
    QStringList lines;
    for (int i = 0; i < static_cast<int>(expected.size()); ++i) {
        const QString stage = expected[i];
        // Cycle 11 fix: the plan-stage keys (dpkg-configure, fix-broken,
        // apt-update, apt-upgrade) must map to the transcript's tool keys
        // (dpkg, fixbroken, aptupdate, upgrade) before matching the
        // `Repair change status <key>:` lines, so the four package stages
        // render their real outcomes instead of "not reported".
        const QString toolKey = fromStd(repairToolKeyForStage(toStd(stage)));
        const QString title = repairStageDisplayTitle(toolKey);
        handledKeys.append(toolKey);
        const QMap<QString, QString>::const_iterator found =
            outcomes.find(toolKey);
        QString detail;
        if (found != outcomes.end()) {
            const QString state = found.data();
            if (changeStatusIsUnchanged(toStd(state))) {
                ++unchanged;
                const QString reason = state.startsWith(QString::fromLatin1("unchanged|"))
                    ? localizedReason(state.mid(10)) : QString::fromLatin1("no repair needed");
                // Cycle 10: ASCII markers only - Etch's fonts garble the
                // modern check/cross/bullet glyphs in the Qt3 log view.
                detail = QString::fromLatin1("[-] %1 - %2")
                             .arg(title).arg(reason);
            } else {
                // A reported change is a completed stage even when a later
                // stage (or the command's final bookkeeping) failed.
                ++successful;
                detail = QString::fromLatin1("[OK] %1 - changed")
                             .arg(title);
                const int separator = state.find(QChar('|'));
                if (separator > 0) {
                    detail += QString::fromLatin1(" - %1")
                                  .arg(localizedReason(state.mid(separator + 1).stripWhiteSpace()));
                }
            }
        } else if (cancelled) {
            // Round 16 (ITEM 4e): the aborted (and later) stages are not
            // failures of their own.
            ++notRun;
            detail = tr(
                "[-] %1 - not run (the plan was stopped at your request)")
                .arg(title);
        } else if (!commandOk && !failureAttributed
                   && (failedStageKey.isEmpty() || toolKey == failedStageKey)) {
            // The named failing stage (or, when the helper failed before
            // naming one, the first stage without a completed status) owns
            // the failure reason; it fails the overall summary. The
            // stage-specific reason line wins so an unrelated ERROR line
            // earlier in the transcript can never be misattributed.
            ++failed;
            failureAttributed = true;
            QString stageReason = localizedReason(repairStageFailureReason(m_transcript, toolKey));
            if (stageReason.isEmpty()) {
                stageReason = failureReason;
            }
            detail = QString::fromLatin1("[FAIL] %1 - %2")
                         .arg(title).arg(stageReason);
        } else if (!commandOk) {
            // A requested stage after the failure never ran; it is not a
            // failure of its own.
            ++notRun;
            detail = tr(
                "[-] %1 - not run (the plan stopped before reaching this stage)")
                .arg(title);
        } else if ((stage == QString::fromLatin1("filesystem")
                    || stage == QString::fromLatin1("fs-inspect"))
                   && cleanFsInspect) {
            ++unchanged;
            detail = tr(
                "[-] %1 - no file system errors found - no changes")
                .arg(title);
        } else {
            ++failed;
            detail = tr(
                "[FAIL] %1 - not reported").arg(title);
        }
        lines.append(QString::fromLatin1("  ") + detail);
    }
    // A failure can belong to a stage the plan did not request (for example
    // the GRUB follow-up of an EFI repair); keep it visible in the summary.
    if (!commandOk && !cancelled && !failureAttributed && !failedStageKey.isEmpty()
        && !handledKeys.contains(failedStageKey)) {
        ++failed;
        failureAttributed = true;
        QString stageReason = localizedReason(repairStageFailureReason(m_transcript, failedStageKey));
        if (stageReason.isEmpty()) {
            stageReason = failureReason;
        }
        lines.append(QString::fromLatin1("  [FAIL] %1 - %2")
                         .arg(repairStageDisplayTitle(failedStageKey))
                         .arg(stageReason));
    }

    QString tail;
    if (cancelled) {
        // Round 16 (ITEM 4e, modern parity): the abort status, never a
        // per-stage failure — completed stages keep their statuses above.
        tail = tr(
            "the plan was stopped at your request; no stage failure is "
            "reported (review the helper output in Logs).");
    } else if (failed > 0) {
        tail = tr(
            "%1 stage(s) failed; review the helper output in Logs.").arg(failed);
    } else if (notRun > 0 && successful == 0) {
        tail = tr(
            "the plan stopped before any stage completed.");
    } else if (successful == 0) {
        tail = tr(
            "no repair was needed; cached diagnostics remain valid.");
    } else {
        tail = tr(
            "repair completed; cached diagnostics were invalidated and must be "
            "regenerated.");
    }
    QString headline;
    if (cancelled) {
        headline = tr(
            "%1 stopped at your request: [OK] %2 successful "
            "| [-] %3 no repair needed - %4")
            .arg(m_pendingLabel == tr("Full Repair")
                     ? tr("Full Repair")
                     : m_pendingLabel)
            .arg(successful).arg(unchanged).arg(tail);
    } else {
        headline = tr(
            "%1 results: [OK] %2 successful | [FAIL] %3 failed "
            "| [-] %4 no repair needed - %5")
            .arg(m_pendingLabel == tr("Full Repair")
                     ? tr("Full Repair")
                     : m_pendingLabel)
            .arg(successful).arg(failed).arg(unchanged).arg(tail);
    }
    if (notRun > 0) {
        headline += QString::fromLatin1(" | [-] %1 not run").arg(notRun);
    }

    // The block is Repair-tagged so the Repairs log filter shows it.
    const int savedKind = m_logEntryKind;
    const QString savedStages = m_logEntryStages;
    m_logEntryKind = LogEntry::Repair;
    m_logEntryStages = m_activeRepairStages.join(QString::fromLatin1(" "));
    appendLog(QString::null);
    // Cycle 10: ASCII brackets, consistent with the ASCII stage markers.
    appendLog(QString::fromLatin1("==== REPAIR ===="));
    appendLog(headline);
    for (int i = 0; i < static_cast<int>(lines.size()); ++i) {
        appendLog(lines[i]);
    }
    appendLog(QString::fromLatin1("==== REPAIR ===="));
    appendLog(QString::null);
    m_logEntryKind = savedKind;
    m_logEntryStages = savedStages;
}

// Modern scheduleEvidenceRefresh() semantics, reduced to the legacy session
// model: after an operation that invalidated the cached diagnostics, schedule
// one Run All for the current scope only when the setting is on, the scope is
// ready, no command is running and the administrator session is already
// active. It never opens an authorization prompt by itself; a missing session
// leaves the manual stale hint in place.
void LegacyMainWindow::maybeAutoRefreshDiagnostics(const QString &reason)
{
    if (!m_autoRefreshEnabled) {
        return;
    }
    if (m_smokeMode || m_running) {
        return;
    }
    if (!diagnosticsScopeReady()) {
        appendLog(tr(
            "Automatic read-only diagnostics regeneration is pending until a "
            "scope is ready (%1).").arg(reason));
        return;
    }
    if (!administratorSessionActive()) {
        appendLog(tr(
            "Automatic read-only diagnostics regeneration is pending until "
            "administrator authorization is active (%1); it never opens an "
            "authorization prompt by itself.").arg(reason));
        return;
    }
    appendLog(tr(
        "Automatic read-only diagnostics regeneration scheduled after %1.")
        .arg(reason));
    QTimer::singleShot(0, this, SLOT(runScheduledAutoRefresh()));
}

void LegacyMainWindow::runScheduledAutoRefresh()
{
    if (!m_autoRefreshEnabled || m_running || !diagnosticsScopeReady()) {
        return;
    }
    if (!administratorSessionActive()) {
        return;
    }
    runDiagnosticsInternal(true);
}

void LegacyMainWindow::runSmokeStep()
{
    if (!m_smokeMode) {
        return;
    }
    if (m_smokeStep == 0) {
        if (!selectionComplete()) {
            autoDetectHostTarget();
        }
        if (!selectionComplete()) {
            m_smokeMode = false;
            emit smokeFinished(false, QString::fromLatin1("no running-host target could be detected"));
            return;
        }
        m_hostMaintenance = true;
        if (m_hostMaintenanceButton) {
            updateButtonText(m_hostMaintenanceButton, tr("Exit Host Maintenance"));
        }
        QStringList args;
        args << QString::fromLatin1("host-diagnose") << selectedDisk() << selectedRoot()
             << QString::fromLatin1("all");
        startCommand(args, true,
                     QString::fromLatin1("smoke host-diagnose all"),
                     false, false, false, false, QString::null, QString::null);
    } else if (m_smokeStep == 1) {
        QStringList args;
        args << QString::fromLatin1("host-validate") << selectedDisk() << selectedRoot();
        startCommand(args, false,
                     QString::fromLatin1("smoke host-validate"),
                     false, false, false, false, QString::null,
                     QString::fromLatin1("validate"));
    } else if (m_smokeStep == 2) {
        // Item #8 adjacent: verifySmokeControls() exercises the real commit
        // path via a synthetic setRepairTarget(), which now (correctly)
        // invalidates the scope's cached diagnostics. Capture the step-0
        // capability count BEFORE that synthetic commit runs, so the recount
        // reflects the parsed host-diagnose evidence rather than the expected
        // post-reset empty model.
        int reported = 0;
        const std::vector<std::string> keys = capabilityKeys();
        for (std::size_t i = 0; i < keys.size(); ++i) {
            if (!m_model.state(keys[i]).empty()) {
                ++reported;
            }
        }
        QString problems;
        const bool controlsOk = verifySmokeControls(&problems);
        int layoutChecked = 0;
        // Round 67: the File Copy top-alignment measurements are reported
        // inline in the geometry line below (verifyLayout appends them).
        QString fileCopyGeometry;
        const bool layoutOk = verifyLayout(&problems, &layoutChecked,
                                           &fileCopyGeometry);
        // Frame-height + content-height measurement (documented in
        // etch-agent-guide.md §6): report the actual displayed frame heights
        // (QWidget::height()) and the tight content heights (the sum of the
        // content widgets plus layout padding/spacing, with the plan scroll
        // view's inflated sizeHint replaced by its real grid content). The
        // reliable read is QWidget::height() after the Repair page is laid out
        // for real (Qt3 does not re-layout a hidden tab from processEvents()
        // alone), so switch to the page, flush and activate the ancestor
        // layouts first.
        QString geometryReport;
        if (m_planGroup && m_toolsGroup && m_planScroll) {
            m_tabs->setCurrentPage(2);  // Repair page
            qApp->sendPostedEvents();
            qApp->processEvents();
            for (QWidget *ancestor = m_planGroup; ancestor;
                 ancestor = ancestor->parentWidget()) {
                if (ancestor->layout()) {
                    ancestor->layout()->activate();
                }
            }
            for (QWidget *ancestor = m_toolsGroup; ancestor;
                 ancestor = ancestor->parentWidget()) {
                if (ancestor->layout()) {
                    ancestor->layout()->activate();
                }
            }
            qApp->processEvents();
            // Tight content height of the plan frame (round 25, Debian
            // Legacy #4): the REAL chrome — the plan scroll view's laid-out
            // top offset inside the group (title/frame, section title,
            // count + buttons, the readiness label at its REAL wrapped
            // height) — plus the grid's real content plus the real bottom
            // chrome (layout bottom margin + group bottom frame).  This is
            // exactly the hugged frame height, so frame == content for a
            // content-hugging frame; the sizeHint difference is NOT used
            // (QGroupBox::sizeHint is the bare VBox hint — no group
            // title/frame overhead — with the word-wrapped label at its
            // probe-width hint height).
            const int planScrollTopY = m_planScroll
                ->mapTo(m_planGroup, QPoint(0, 0)).y();
            const int planLayoutMargin = m_planGroup->layout()
                && m_planGroup->layout()->margin() > 0
                ? m_planGroup->layout()->margin() : 0;
            const int planBottomChrome = planLayoutMargin
                + m_planGroup->frameWidth() + 1;
            const int planContent = planScrollTopY + m_planGridContentHeight
                + planBottomChrome;
            // The tools frame has no scroll view, so its sizeHint is already
            // the tight content (title + list + padding).
            const int toolsContent = m_toolsGroup->sizeHint().height();
            geometryReport += QString::fromLatin1(
                "; frame height (Full Repair plan): %1 px; "
                "frame height (Individual repair tools): %2 px; "
                "content height (Full Repair plan): %3 px; "
                "content height (Individual repair tools): %4 px; "
                "DEBUG gridCalc=%5 readActual=%6 readHint=%7 chrome=%8 "
                "scrollTopY=%9")
                .arg(m_planGroup->height())
                .arg(m_toolsGroup->height())
                .arg(planContent)
                .arg(toolsContent)
                .arg(m_planGridContentHeight)
                .arg(m_planReadinessLabel
                     ? m_planReadinessLabel->height() : -1)
                .arg(m_planReadinessLabel
                     ? m_planReadinessLabel->sizeHint().height() : -1)
                .arg(m_planGroup->sizeHint().height()
                     - m_planScroll->sizeHint().height())
                .arg(planScrollTopY);
        }
        // Round 67: the File Copy top-alignment numbers (verifyLayout
        // measured them on the real laid-out page; see the contract there).
        if (!fileCopyGeometry.isEmpty()) {
            geometryReport += fileCopyGeometry;
        }
        // A pass without checked widgets would be vacuous: every tab has
        // titles, buttons, lists and combos, so the floor stays well above 0.
        if (layoutChecked < 20) {
            problems.append(QString::fromLatin1(
                "layout verification checked only %1 widgets").arg(layoutChecked));
        }
        const bool ok = m_smokeDiagnoseOk && m_smokeValidateOk
            && reported == static_cast<int>(keys.size()) && controlsOk
            && layoutOk && layoutChecked >= 20;
        if (!problems.isEmpty()) {
            appendLog(QString::fromLatin1("smoke layout/control problems:\n%1").arg(problems));
        }
        const QString summary = QString::fromLatin1(
            "host-diagnose: %1 capability lines parsed; host-validate: %2; "
            "controls: %3; layout: %4 (%5 widgets checked)")
            .arg(reported)
            .arg(m_smokeValidateOk ? QString::fromLatin1("ok") : QString::fromLatin1("incomplete"))
            .arg(controlsOk ? QString::fromLatin1("ok") : QString::fromLatin1("FAIL"))
            .arg(layoutOk ? QString::fromLatin1("ok") : QString::fromLatin1("FAIL"))
            .arg(layoutChecked)
            + geometryReport;
        m_smokeMode = false;
        emit smokeFinished(ok, summary);
    }
}

void LegacyMainWindow::startSmokeTest()
{
    m_smokeMode = true;
    m_smokeStep = 0;
    m_smokeDiagnoseOk = false;
    m_smokeValidateOk = false;
    // The layout contract is asserted at 1024x768: resize before the first
    // event-loop turn so the verification step sees the final geometry.
    resize(1024, 768);
    QTimer::singleShot(0, this, SLOT(runSmokeStep()));
}

bool LegacyMainWindow::verifySmokeControls(QString *problems)
{
    bool ok = true;
    // Round 21 (modern parity): the Chroot/Host Shell reason label shows ONLY
    // the availability reason while the shell is greyed out and hides when
    // the shell is ready.  Checked HERE, before the synthetic commit below:
    // at this point the smoke still holds the step-0 cached host diagnostics
    // (the commit resets the model afterwards), so the shell is available and
    // the label must be hidden — the ready state.
    if (m_chrootReasonLabel && !m_chrootReasonLabel->isHidden()) {
        problems->append(QString::fromLatin1(
            "Chroot Shell reason label must be hidden while the shell is available"));
        ok = false;
    }
    // Round 31 (Debian Legacy #9): the in-field greyed placeholders — the
    // round-21 hint labels are gone.  Items 1-3: the empty-state hint moved
    // INSIDE the diagnostics results pane, the shell output pane shows the
    // exact modern "Command output will appear here." placeholder (temporary —
    // it yields to the first streamed output), and the shell command hint is
    // the placeholder INSIDE the "Command:" input line.  The smoke asserts
    // the wiring; the paint itself follows the PlaceholderTextEdit /
    // ShellCommandLineEdit pattern.
    if (!m_rawView) {
        problems->append(QString::fromLatin1("diagnostics results pane missing"));
        ok = false;
    } else if (static_cast<PlaceholderTextEdit *>(m_rawView)->placeholderText()
               != tr("Diagnostic results appear here.")) {
        problems->append(QString::fromLatin1(
            "diagnostics results pane lost the in-pane empty-state placeholder"));
        ok = false;
    }
    if (!m_shellOutput) {
        problems->append(QString::fromLatin1("shell output pane missing"));
        ok = false;
    } else if (static_cast<PlaceholderTextEdit *>(m_shellOutput)->placeholderText()
               != tr("Command output will appear here.")) {
        problems->append(QString::fromLatin1(
            "shell output pane lost the modern empty-state placeholder"));
        ok = false;
    }
    if (!m_shellCommandEdit) {
        problems->append(QString::fromLatin1("shell command field missing"));
        ok = false;
    } else if (m_shellCommandEdit->placeholderText()
               != tr("Command to run, for example: apt-get update, apt-get upgrade")) {
        problems->append(QString::fromLatin1(
            "shell command field lost the in-field command placeholder"));
        ok = false;
    }
    if (!m_tabs || m_tabs->count() != 7) {
        problems->append(QString::fromLatin1("tab set missing (expected 7 tabs)"));
        ok = false;
    } else {
        // The Chroot Shell tab label switches to "Host Shell" in host mode
        // (modern updateChrootShellMode); the smoke runs with Host Maintenance
        // active, so expect the host label here. Modern parity: there is no
        // About tab (Help -> About Boot Bitch is the dialog).
        const char *expectedTabs[] = {
            "Systems", "Diagnostics", "Repair", "Host Shell", "File Copy",
            "Logs", "Settings"
        };
        for (int i = 0; i < 7; ++i) {
            if (m_tabs->label(i) != QString::fromLatin1(expectedTabs[i])) {
                problems->append(QString::fromLatin1("tab %1 is '%2', expected '%3'")
                                     .arg(i).arg(m_tabs->label(i))
                                     .arg(QString::fromLatin1(expectedTabs[i])));
                ok = false;
            }
        }
    }
    // Global header (modern parity): title, subtitle and guarded-repair badge.
    if (!m_headerTitle || m_headerTitle->text() != QString::fromLatin1("Boot Bitch")) {
        problems->append(QString::fromLatin1("global header title missing"));
        ok = false;
    }
    if (!m_headerSubtitle
        || m_headerSubtitle->text() != tr("Linux recovery and boot-repair utility")) {
        problems->append(QString::fromLatin1("global header subtitle missing"));
        ok = false;
    }
    if (!m_headerBadge
        || !m_headerBadge->text().contains(QString::fromLatin1("GUARDED REPAIR"))
        || !m_headerBadge->text().contains(QString::fromLatin1(LEGACY_VERSION))) {
        problems->append(QString::fromLatin1("guarded-repair version badge missing"));
        ok = false;
    }
    // Global busy indicator (modern parity): a reserved row below the
    // guarded-repair badge that shows the modern working text only while a
    // helper command runs; the smoke runs between commands, so the label
    // must be empty now and the row must keep its reserved height.
    if (!m_busyLabel) {
        problems->append(QString::fromLatin1("busy indicator missing"));
        ok = false;
    } else {
        if (!m_busyTimer) {
            problems->append(QString::fromLatin1("busy animation timer missing"));
            ok = false;
        }
        if (!m_busyLabel->text().isEmpty()) {
            problems->append(QString::fromLatin1(
                "busy indicator shows '%1' while no helper command runs")
                .arg(m_busyLabel->text()));
            ok = false;
        }
        // Modern parity: the indicator sits BELOW the guarded-repair badge
        // and its right edge is flush with the badge's right edge.
        if (m_headerBadge) {
            const QRect badgeRect(m_headerBadge->mapTo(this, QPoint(0, 0)),
                                  m_headerBadge->size());
            const QRect busyRect(m_busyLabel->mapTo(this, QPoint(0, 0)),
                                 m_busyLabel->size());
            if (busyRect.top() < badgeRect.bottom()) {
                problems->append(QString::fromLatin1(
                    "busy indicator is not below the guarded-repair badge"));
                ok = false;
            }
            if (QABS(busyRect.right() - badgeRect.right())
                > kBusyBadgeEdgeTolerance) {
                problems->append(QString::fromLatin1(
                    "busy indicator is not right-aligned with the guarded-repair badge"));
                ok = false;
            }
        }
        // Show the widest animated frame; the reserved height must not
        // change and the slot must stay inside the window.
        const int reservedHeight = m_busyLabel->height();
        m_busyLabel->setText(QString::fromLatin1("Running all diagnostics..."));
        qApp->processEvents();
        if (m_busyLabel->height() != reservedHeight) {
            problems->append(QString::fromLatin1(
                "busy indicator row lost its reserved height when shown"));
            ok = false;
        }
        const QRect shownRect(m_busyLabel->mapTo(this, QPoint(0, 0)),
                              m_busyLabel->size());
        if (!rect().contains(shownRect)) {
            problems->append(QString::fromLatin1(
                "busy indicator is clipped by the window when shown"));
            ok = false;
        }
        m_busyLabel->setText(QString::null);
        qApp->processEvents();
        if (m_busyLabel->height() != reservedHeight) {
            problems->append(QString::fromLatin1(
                "busy indicator row lost its reserved height when cleared"));
            ok = false;
        }
    }
    // Section titles: the page headings plus the sectionTitle label inside
    // every titleless group frame (Qt3 clips QGroupBox titles).
    if (m_sectionTitles.size() < 24) {
        problems->append(QString::fromLatin1("section titles missing (found %1)")
                             .arg(static_cast<int>(m_sectionTitles.size())));
        ok = false;
    }
    if (!m_detailList || m_detailList->childCount() < 10) {
        problems->append(QString::fromLatin1("selected-drive details panel missing or incomplete"));
        ok = false;
    } else {
        // The ten modern details rows, in the modern order; unknown values
        // render as "-", never blank.
        const char *expectedDetailFields[] = {
            "Drive:", "Detected target:", "Model / label:", "Status:",
            "Size:", "Connection:", "Filesystem:", "UUID:", "Mounts:",
            "Protection:"
        };
        QListViewItem *detailRow = m_detailList->firstChild();
        for (int i = 0; i < 10 && detailRow;
             ++i, detailRow = detailRow->nextSibling()) {
            if (detailRow->text(0) != QString::fromLatin1(expectedDetailFields[i])) {
                problems->append(QString::fromLatin1(
                    "details panel row %1 is '%2', expected '%3'")
                    .arg(i + 1).arg(detailRow->text(0))
                    .arg(QString::fromLatin1(expectedDetailFields[i])));
                ok = false;
            }
            if (detailRow->text(1).isEmpty()) {
                problems->append(QString::fromLatin1(
                    "details value for '%1' is blank (must render '-')")
                    .arg(detailRow->text(0)));
                ok = false;
            }
        }
    }
    if (!m_unlockStatusView || !m_unlockStatusView->text().contains(QString::fromLatin1("State:"))) {
        problems->append(QString::fromLatin1("unlock status panel missing or not showing unlock state"));
        ok = false;
    }
    if (!m_logSearchEdit) {
        problems->append(QString::fromLatin1("log search control missing"));
        ok = false;
    }
    if (!m_sessionLogList || m_sessionLogList->childCount() < 1) {
        problems->append(QString::fromLatin1("session log list missing or empty"));
        ok = false;
    }
    // Item 1: the Settings wrap checkbox is intentionally gone (modern parity:
    // wrap is on by default and the View menu is the only wrap control), so
    // there is no checkbox-presence assertion here anymore.
    if (!m_unlockButton || !m_setTargetButton || !m_hostMaintenanceButton) {
        problems->append(QString::fromLatin1("target/unlock/host-maintenance controls missing"));
        ok = false;
    }
    if (!m_authorizeButton || !m_authStatusLabel) {
        problems->append(QString::fromLatin1("deferred-authorization controls missing"));
        ok = false;
    }
    if (!m_logFilterCombo
        || m_logFilterCombo->count() != logFilterSpecCount + diagnosticSpecCount) {
        problems->append(QString::fromLatin1(
            "log filter missing or not 1:1 with modern (all/diagnostics/repairs/"
            "3 workflows/16 sections)"));
        ok = false;
    }
    // Repair parity: the Full Repair plan section and the tools list/Selected
    // tool pane replace the old button grid and the removed unsupported list.
    // (Round 21: the inline plan paragraph is gone — its content moved into
    // the Repair "i" popup — so the check below names the remaining controls.)
    if (!m_planCountLabel || !m_planReadinessLabel
        || m_planStageLabels.isEmpty() || !m_configurePlanButton
        || !m_runFullRepairButton) {
        problems->append(QString::fromLatin1("Full Repair plan controls missing"));
        ok = false;
    }
    if (m_configurePlanButton
        && m_configurePlanButton->text() != tr("Configure Plan")) {
        problems->append(QString::fromLatin1("Configure Plan text changed: '%1'")
                             .arg(m_configurePlanButton->text()));
        ok = false;
    }
    if (m_runFullRepairButton
        && m_runFullRepairButton->text() != tr("Run Full Repair")) {
        problems->append(QString::fromLatin1("Run Full Repair text changed: '%1'")
                             .arg(m_runFullRepairButton->text()));
        ok = false;
    }
    if (!m_repairVerticalSplitter
        || m_repairVerticalSplitter->orientation() != Qt::Vertical) {
        problems->append(QString::fromLatin1(
            "Repair vertical splitter missing or not vertical"));
        ok = false;
    }
    if (m_repairVerticalSplitter
        && m_repairVerticalSplitter->orientation() == Qt::Vertical) {
        // The modern Repair page keeps a draggable divider between the Full
        // Repair plan and the individual-tools panes; both panes must keep a
        // usable floor at 1024x768. Qt3's QSplitter exposes the panes through
        // sizes() (there is no count()/widget()).
        const QValueList<int> paneSizes = m_repairVerticalSplitter->sizes();
        if (paneSizes.count() != 2 || paneSizes[0] < 100 || paneSizes[1] < 200) {
            problems->append(QString::fromLatin1(
                "Repair vertical splitter panes are missing or too small "
                "(%1 panes: top=%2px, bottom=%3px)")
                .arg(paneSizes.count())
                .arg(paneSizes.count() > 0 ? paneSizes[0] : -1)
                .arg(paneSizes.count() > 1 ? paneSizes[1] : -1));
            ok = false;
        }
    }
    // B7 loop fix: the Systems horizontal splitter must keep the details pane
    // at its 372px floor (Field/Value columns 340px + margins) with the left
    // inventory pane absorbing the rest at 1024x768.
    if (m_systemsSplitter
        && m_systemsSplitter->orientation() == Qt::Horizontal) {
        const QValueList<int> paneSizes = m_systemsSplitter->sizes();
        if (paneSizes.count() != 2 || paneSizes[0] < 400 || paneSizes[1] < 360) {
            problems->append(QString::fromLatin1(
                "Systems splitter panes are missing or too small "
                "(%1 panes: left=%2px, details=%3px; the details pane needs "
                "its 372px floor)")
                .arg(paneSizes.count())
                .arg(paneSizes.count() > 0 ? paneSizes[0] : -1)
                .arg(paneSizes.count() > 1 ? paneSizes[1] : -1));
            ok = false;
        }
    }
    if (!m_toolList || m_toolList->childCount() != toolSpecCount) {
        problems->append(QString::fromLatin1(
            "individual repair tools list missing or incomplete (count=%1)")
            .arg(m_toolList ? m_toolList->childCount() : -1));
        ok = false;
    }
    if (m_toolList && m_toolList->columns() != 2) {
        problems->append(QString::fromLatin1(
            "tools list must carry the Tool and Full Repair columns"));
        ok = false;
    }
    if (m_toolList && m_toolList->firstChild()
        && m_toolList->firstChild()->text(0) != QString::fromLatin1("Validate environment")) {
        problems->append(QString::fromLatin1(
            "tools list does not start with Validate environment (first='%1')")
            .arg(m_toolList->firstChild()->text(0)));
        ok = false;
    }
    if (!m_toolTitle || !m_toolDescription || !m_toolPlanStatus || !m_toolRunButton) {
        problems->append(QString::fromLatin1("Selected tool pane missing"));
        ok = false;
    }
    // B7-7: the bootstack tool is the legacy one-pass stage, gated like the
    // other write tools (capability + host-maintenance on host scope) and
    // kept out of the Full Repair plan.
    {
        int bootstackRow = -1;
        int rowIndex = 0;
        for (QListViewItem *row = m_toolList->firstChild(); row;
             row = row->nextSibling(), ++rowIndex) {
            if (row->text(0) == QString::fromLatin1("Boot stack reconciliation")) {
                bootstackRow = rowIndex;
                break;
            }
        }
        if (bootstackRow < 0) {
            problems->append(QString::fromLatin1("tool list rows missing (bootstack)"));
            ok = false;
        } else if (m_model.isAvailable("bootstack", toStd(identity()), 0)) {
            QListViewItem *bootItem = m_toolList->firstChild();
            for (int step = 0; bootItem && step < bootstackRow; ++step) {
                bootItem = bootItem->nextSibling();
            }
            m_toolList->setSelected(bootItem, true);
            m_toolList->setCurrentItem(bootItem);
            updateToolDetails();
            if (m_toolRunButton->text() != QString::fromLatin1("Reconcile Boot Stack")) {
                problems->append(QString::fromLatin1(
                    "bootstack tool button is '%1', expected Reconcile Boot Stack")
                    .arg(m_toolRunButton->text()));
                ok = false;
            }
            if (m_toolPlanStatus->text().find(
                    QString::fromLatin1("Full Repair plan: Manual recovery tool")) < 0) {
                problems->append(QString::fromLatin1(
                    "bootstack tool lost its plan status"));
                ok = false;
            }
            if (m_toolList->firstChild()) {
                m_toolList->setSelected(m_toolList->firstChild(), true);
                m_toolList->setCurrentItem(m_toolList->firstChild());
                updateToolDetails();
            }
        }
    }
    // Cycle 8: the File Copy tab matches modern's buildFileCopyPage - heading
    // row (title | scope label | Preview Changes | Copy and Verify), the
    // direction row, and the three titled groups (sources / destination with
    // Browse Target Folders... / ownership and copy policy).
    if (!m_fileCopyDirectionCombo || !m_fileCopySourceList
        || !m_fileCopyDestinationEdit || !m_fileCopyAddFilesButton
        || !m_fileCopyAddFolderButton || !m_fileCopyRemoveButton
        || !m_fileCopyClearButton || !m_fileCopyPreviewButton
        || !m_fileCopyRunButton || !m_fileCopyScopeLabel
        || !m_fileCopyBrowseButton || !m_fileCopyOwnershipCombo
        || !m_fileCopySourceGroup || !m_fileCopyDestinationGroup
        || !m_fileCopyOptionsGroup) {
        problems->append(QString::fromLatin1("File Copy controls missing"));
        ok = false;
    } else {
        if (m_fileCopyDirectionCombo->count() != 2) {
            problems->append(QString::fromLatin1(
                "File Copy direction combo must carry Host -> Repair / Repair -> Host"));
            ok = false;
        }
        if (m_fileCopyOwnershipCombo->count() != 2
            || m_fileCopyOwnershipCombo->text(0) != QString::fromLatin1(
                "Smart destination ownership (recommended)")
            || m_fileCopyOwnershipCombo->text(1) != QString::fromLatin1(
                "Preserve source numeric UID/GID")) {
            problems->append(QString::fromLatin1(
                "File Copy ownership combo lost the legacy ownership set"));
            ok = false;
        }
        const char *const expectedTitles[] = {
            "1. Select source files or folders from this host",
            "2. Choose destination in repaired system",
            "3. Ownership and copy policy"
        };
        for (std::size_t t = 0; t < sizeof(expectedTitles) / sizeof(expectedTitles[0]); ++t) {
            bool found = false;
            for (std::size_t i = 0; i < m_sectionTitles.size(); ++i) {
                if (m_sectionTitles[i]
                    && m_sectionTitles[i]->text() == QString::fromLatin1(expectedTitles[t])) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                problems->append(QString::fromLatin1(
                    "File Copy group title missing: '%1'")
                    .arg(QString::fromLatin1(expectedTitles[t])));
                ok = false;
            }
        }
        if (m_fileCopyScopeLabel->text() != tr("Target: none selected")
            && m_fileCopyScopeLabel->text().find(QString::fromLatin1("Host maintenance:")) < 0
            && m_fileCopyScopeLabel->text().find(QString::fromLatin1("Target:")) < 0) {
            problems->append(QString::fromLatin1(
                "File Copy scope label lost the modern scope text"));
            ok = false;
        }
        // Browse Target Folders... reads the repair tree through the helper
        // and needs a committed target; in the smoke the scope is the
        // running host with no committed repair target, so it stays disabled
        // in the default Host -> Repair direction.
        if (m_fileCopyBrowseButton->isEnabled()) {
            problems->append(QString::fromLatin1(
                "Browse Target Folders... must stay disabled without a "
                "committed repair target"));
            ok = false;
        }
    }
    // B7-7: Make Default beside Host Maintenance, gated by Host Maintenance +
    // the cached host-default probe + the session.
    if (!m_hostDefaultButton) {
        problems->append(QString::fromLatin1("Make Default button missing"));
        ok = false;
    } else if (m_hostDefaultButton->text() != tr("Make Default")) {
        problems->append(QString::fromLatin1("Make Default button text changed"));
        ok = false;
    }
    // Cycle 9: the protected-host card carries the green-check identity line,
    // the storage line, the PROTECTED badge and the Details action; the host
    // is known in the smoke, so Details must be enabled.
    if (!m_hostCard || !m_hostSystemLabel || !m_hostStorageLabel
        || !m_hostProtectedBadge || !m_hostDetailsButton) {
        problems->append(QString::fromLatin1("protected-host card controls missing"));
        ok = false;
    } else {
        if (m_hostSystemLabel->text().isEmpty()
            || m_hostSystemLabel->text() == tr("Detecting running system...")) {
            problems->append(QString::fromLatin1(
                "protected-host identity line was never populated"));
            ok = false;
        }
        if (m_hostStorageLabel->text().isEmpty()
            || m_hostStorageLabel->text() == tr("Detecting protected storage...")) {
            problems->append(QString::fromLatin1(
                "protected-host storage line was never populated"));
            ok = false;
        }
        if (m_hostProtectedBadge->text() != tr("PROTECTED")) {
            problems->append(QString::fromLatin1("PROTECTED badge text changed"));
            ok = false;
        }
        if (m_hostDetailsButton->text() != tr("Details")) {
            problems->append(QString::fromLatin1("Details button text changed"));
            ok = false;
        }
        if (!m_hostDetailsButton->isEnabled()) {
            problems->append(QString::fromLatin1(
                "Details must be enabled while the running host is known"));
            ok = false;
        }
    }
    // Cycle 9: the device tree starts collapsed (disks rolled up).
    if (m_deviceList) {
        for (QListViewItem *diskItem = m_deviceList->firstChild(); diskItem;
             diskItem = diskItem->nextSibling()) {
            if (diskItem->isOpen()) {
                problems->append(QString::fromLatin1(
                    "device tree row '%1' must start collapsed")
                    .arg(diskItem->text(0)));
                ok = false;
            }
        }
    }
    // Cycle 9: the Diagnostics run button offers Run/Re-run Diagnostic; the
    // scope label is the two-line modern form (Host maintenance: over the
    // disk path in the smoke's host scope).
    if (m_runDiagnosticButton) {
        const QString runText = m_runDiagnosticButton->text();
        if (runText != tr("Run Diagnostic")
            && runText != tr("Re-run Diagnostic")) {
            problems->append(QString::fromLatin1(
                "diagnostic run button lost the Run/Re-run text ('%1')")
                .arg(runText));
            ok = false;
        }
    }
    if (m_scopeLabel
        && m_scopeLabel->text().find(QString::fromLatin1("Host maintenance:\n")) < 0) {
        problems->append(QString::fromLatin1(
            "scope label lost the two-line Host maintenance form"));
        ok = false;
    }
    // Cycle 14 loop: manual header-click sorting keeps the curated order by
    // default; clicking the Tool column sorts asc then desc, and the
    // Selected tool pane follows. Afterwards the curated order is restored
    // (detach + re-insert per toolSpecs) so later checks stay valid.
    if (m_toolList && m_toolList->header() && m_toolList->childCount() > 1
        && m_toolList->firstChild()) {
        // Cycle 14 loop: the sort case is deterministic - the pane-follow
        // invariant compares the pane against the CURRENT first row's spec
        // (looked up by the row's text, never a hard-coded expectation), and
        // the restore rebuilds the curated order from toolSpecs.
        const QString firstBefore = m_toolList->firstChild()->text(0);
        toolListHeaderClicked(0);
        if (!m_toolList->firstChild()) {
            problems->append(QString::fromLatin1(
                "tools list became empty after the first header click"));
            ok = false;
        } else {
            if (m_toolList->firstChild()->text(0) == firstBefore) {
                problems->append(QString::fromLatin1(
                    "tools list did not reorder on the first header click"));
                ok = false;
            }
            // Pane-follow invariant: title/button must match the current
            // first row's spec (the row's text IS the spec title).
            int firstSpec = -1;
            const QString firstRowText = m_toolList->firstChild()->text(0);
            for (int i = 0; i < toolSpecCount; ++i) {
                if (QString::fromLatin1(toolSpecs[i].title) == firstRowText) {
                    firstSpec = i;
                    break;
                }
            }
            if (firstSpec < 0) {
                problems->append(QString::fromLatin1(
                    "sorted first row has no tool spec"));
                ok = false;
            } else {
                if (m_toolTitle && m_toolTitle->text()
                    != QString::fromLatin1(toolSpecs[firstSpec].title)) {
                    problems->append(QString::fromLatin1(
                        "Selected tool pane did not follow the sorted selection"));
                    ok = false;
                }
                if (m_toolRunButton && m_toolRunButton->text()
                    != QString::fromLatin1(toolSpecs[firstSpec].button)) {
                    problems->append(QString::fromLatin1(
                        "Selected tool button did not follow the sorted selection"));
                    ok = false;
                }
            }
            const QString firstAfterFirstClick = m_toolList->firstChild()->text(0);
            toolListHeaderClicked(0);   // descending (toggle)
            if (!m_toolList->firstChild()) {
                problems->append(QString::fromLatin1(
                    "tools list became empty after the toggle click"));
                ok = false;
            } else {
                if (m_toolList->firstChild()->text(0) == firstAfterFirstClick) {
                    problems->append(QString::fromLatin1(
                        "tools list did not toggle on the second header click"));
                    ok = false;
                }
                const QString toggledRowText = m_toolList->firstChild()->text(0);
                int toggledSpec = -1;
                for (int i = 0; i < toolSpecCount; ++i) {
                    if (QString::fromLatin1(toolSpecs[i].title) == toggledRowText) {
                        toggledSpec = i;
                        break;
                    }
                }
                if (toggledSpec >= 0 && m_toolTitle && m_toolTitle->text()
                    != QString::fromLatin1(toolSpecs[toggledSpec].title)) {
                    problems->append(QString::fromLatin1(
                        "Selected tool pane did not follow the toggled selection"));
                    ok = false;
                }
            }
            // Restore the curated order by rebuilding the rows.
            m_toolList->clear();
            QListViewItem *lastTool = 0;
            for (int i = 0; i < toolSpecCount; ++i) {
                lastTool = new QListViewItem(m_toolList, lastTool,
                                             QString::fromLatin1(toolSpecs[i].title),
                                             tr("not reported"));
            }
            m_toolSortColumn = -1;
            m_toolSortAscending = true;
            if (m_toolList->firstChild()) {
                m_toolList->setSelected(m_toolList->firstChild(), true);
                m_toolList->setCurrentItem(m_toolList->firstChild());
                updateToolDetails();
                if (m_toolList->firstChild()->text(0)
                    != QString::fromLatin1("Validate environment")) {
                    problems->append(QString::fromLatin1(
                        "tools list did not restore the curated order"));
                    ok = false;
                }
                if (m_toolTitle && m_toolTitle->text()
                    != QString::fromLatin1(toolSpecs[0].title)) {
                    problems->append(QString::fromLatin1(
                        "Selected tool pane did not return to the first curated tool"));
                    ok = false;
                }
                if (m_toolRunButton && m_toolRunButton->text()
                    != QString::fromLatin1(toolSpecs[0].button)) {
                    problems->append(QString::fromLatin1(
                        "Selected tool button did not return to the first curated tool"));
                    ok = false;
                }
            } else {
                problems->append(QString::fromLatin1(
                    "tools list became empty after the restore"));
                ok = false;
            }
        }
    }
    // Cycle 11: the Unlock button is in its default state in the smoke (no
    // unlock ran), and the Authorize affordance is hidden while the
    // administrator session is active (modern deferred-only parity).
    if (m_unlockButton
        && m_unlockButton->text() != tr("Unlock")) {
        problems->append(QString::fromLatin1(
            "Unlock button is not in its default state ('%1')")
            .arg(m_unlockButton->text()));
        ok = false;
    }
    if (m_authStatusLabel && !m_authStatusLabel->isHidden()) {
        problems->append(QString::fromLatin1(
            "authorization status label must be hidden while the session is active"));
        ok = false;
    }
    if (m_authorizeButton && !m_authorizeButton->isHidden()) {
        problems->append(QString::fromLatin1(
            "Authorize button must be hidden while the session is active"));
        ok = false;
    }
    // Cycle 12: the unlock result's mapper rows are injected into the
    // inventory owned by the selected drive (the read-only scan cannot see
    // them), so they show indented under the drive with the helper-probed
    // fstype and the inventory-backed resolution can find them.
    {
        const QString fakeDisk = QString::fromLatin1("/dev/bootrepair-smoke-inject");
        DeviceRow dk;
        dk.name = "bootrepair-smoke-inject";
        dk.path = toStd(fakeDisk);
        dk.disk = true;
        m_rows.insert(fakeDisk, dk);
        m_inventory.push_back(dk);
        injectUnlockedMapperRows(fakeDisk,
                                 QString::fromLatin1("/dev/mapper/luks-smoke-inject"),
                                 QString::fromLatin1("/dev/mapper/smoke-inject-root"),
                                 QString::fromLatin1("ext3"),
                                 QString::fromLatin1("smoke-uuid"));
        QListViewItem *injectedDisk = 0;
        for (QListViewItem *item = m_deviceList->firstChild(); item;
             item = item->nextSibling()) {
            if (item->text(0) == fakeDisk) {
                injectedDisk = item;
                break;
            }
        }
        if (!injectedDisk || injectedDisk->childCount() != 2) {
            problems->append(QString::fromLatin1(
                "unlocked mapper rows were not added under the selected drive"));
            ok = false;
        } else {
            for (QListViewItem *child = injectedDisk->firstChild(); child;
                 child = child->nextSibling()) {
                if (child->text(0) == QString::fromLatin1(
                        "/dev/mapper/smoke-inject-root")
                    && child->text(3) != QString::fromLatin1("ext3")) {
                    problems->append(QString::fromLatin1(
                        "unlocked root row lost the helper-probed fstype ('%1')")
                        .arg(child->text(3)));
                    ok = false;
                }
            }
            const QMap<QString, DeviceRow>::const_iterator rootRow =
                m_rows.find(QString::fromLatin1("/dev/mapper/smoke-inject-root"));
            if (rootRow == m_rows.end()
                || fromStd(rootRow.data().uuid) != QString::fromLatin1("smoke-uuid")) {
                problems->append(QString::fromLatin1(
                    "unlocked root row lost the helper-probed UUID"));
                ok = false;
            }
        }
        m_rows.remove(fakeDisk);
        for (std::vector<DeviceRow>::iterator it = m_inventory.begin();
             it != m_inventory.end(); ++it) {
            if (fromStd(it->path) == fakeDisk) {
                m_inventory.erase(it);
                break;
            }
        }
        m_rows.remove(QString::fromLatin1("/dev/mapper/luks-smoke-inject"));
        m_rows.remove(QString::fromLatin1("/dev/mapper/smoke-inject-root"));
        rebuildDeviceList();
    }
    // Cycle 9: the guarded-repair badge reaches the window's right edge.
    if (m_headerBadge) {
        const QRect badgeRect(m_headerBadge->mapTo(this, QPoint(0, 0)),
                              m_headerBadge->size());
        if (badgeRect.right() < width() - 2) {
            problems->append(QString::fromLatin1(
                "GUARDED REPAIR badge does not reach the window edge "
                "(right=%1, window=%2)")
                .arg(badgeRect.right()).arg(width()));
            ok = false;
        }
    }
    // B7-3: the unlock status pane keeps the modern field order.
    if (m_unlockStatusView) {
        const QString statusText = m_unlockStatusView->text();
        if (statusText.contains(QString::fromLatin1("State:"))
            && statusText.contains(QString::fromLatin1("Component:"))
            && statusText.contains(QString::fromLatin1("Mapper:"))
            && statusText.contains(QString::fromLatin1("Method:"))) {
            // Modern field order State -> Component -> Mapper -> Method.
            const int statePos = statusText.find(QString::fromLatin1("State:"));
            const int componentPos = statusText.find(QString::fromLatin1("Component:"));
            const int mapperPos = statusText.find(QString::fromLatin1("Mapper:"));
            const int methodPos = statusText.find(QString::fromLatin1("Method:"));
            if (!(statePos >= 0 && statePos < componentPos
                  && componentPos < mapperPos && mapperPos < methodPos)) {
                problems->append(QString::fromLatin1(
                    "unlock status fields are not in the modern order"));
                ok = false;
            }
        }
    }
    if (m_planChecks.size() != static_cast<std::size_t>(planSpecCount)) {
        problems->append(QString::fromLatin1(
            "Settings Full Repair plan checkboxes missing (found %1)")
            .arg(static_cast<int>(m_planChecks.size())));
        ok = false;
    } else {
        // Modern availability semantics: an available stage is checkable
        // (with its saved/default preference), an unavailable stage stays
        // SHOWN but disabled AND unchecked (the row set is never hidden).
        // Round 52: checkbox enablement follows the PRESENTATION state
        // (planStageState) — the run-side hostOnly gate
        // (planStageAvailable) no longer greys the checkbox out, only the
        // plan exclusion.
        for (int i = 0; i < planSpecCount; ++i) {
            if (!m_planChecks[i]) {
                problems->append(QString::fromLatin1(
                    "Full Repair plan checkbox missing: %1")
                    .arg(QString::fromLatin1(planSpecs[i].checkLabel)));
                ok = false;
                continue;
            }
            QString reason;
            const bool available =
                planStageState(i, &reason) == PlanStageAvailable;
            if (m_planChecks[i]->isEnabled() != available) {
                problems->append(QString::fromLatin1(
                    "Full Repair plan checkbox enablement does not match its "
                    "availability: %1 (enabled=%2 available=%3)")
                    .arg(QString::fromLatin1(planSpecs[i].checkLabel))
                    .arg(m_planChecks[i]->isEnabled()
                             ? QString::fromLatin1("yes")
                             : QString::fromLatin1("no"))
                    .arg(available ? QString::fromLatin1("yes")
                                   : QString::fromLatin1("no")));
                ok = false;
            }
            if (!available && m_planChecks[i]->isChecked()) {
                problems->append(QString::fromLatin1(
                    "unavailable Full Repair plan checkbox stays checked: %1")
                    .arg(QString::fromLatin1(planSpecs[i].checkLabel)));
                ok = false;
            }
        }
        // Host-scope smoke parity: the legacy SysV display-manager capability
        // is available on this Etch host, so its plan row must be present,
        // checkable and off by default (the modern per-key default).
        const int displayPlan = planIndexForCapability("display");
        if (displayPlan < 0 || !m_planChecks[displayPlan]
            || !m_planChecks[displayPlan]->isEnabled()
            || m_planChecks[displayPlan]->isChecked()) {
            problems->append(QString::fromLatin1(
                "display-manager plan row is not a checkable off-by-default "
                "row on the host scope"));
            ok = false;
        }
        // The file system repair row is now wired (the inspect-first flow): on
        // the Etch host the filesystem capability is available, so its plan row
        // must be present, checkable and on by default (the modern per-key
        // default for the filesystem stage).
        const int filesystemPlan = planIndexForCapability("filesystem");
        if (filesystemPlan < 0 || !m_planChecks[filesystemPlan]
            || !m_planChecks[filesystemPlan]->isEnabled()
            || !m_planChecks[filesystemPlan]->isChecked()) {
            problems->append(QString::fromLatin1(
                "file system repair plan row is not a checkable on-by-default "
                "row on the host scope"));
            ok = false;
        }
    }
    // Plan/tools behavior: selecting a legacy-runnable tool fills the Selected
    // tool pane, a display-only tool disables the run button with the exact
    // reason, and a Settings plan checkbox toggles the tool's Full Repair
    // column.
    if (m_toolList && m_planChecks.size() == static_cast<std::size_t>(planSpecCount)
        && m_toolTitle && m_toolRunButton && m_toolPlanStatus) {
        int dpkgRow = -1;
        int dkmsRow = -1;
        int rowIndex = 0;
        for (QListViewItem *row = m_toolList->firstChild(); row;
             row = row->nextSibling(), ++rowIndex) {
            if (row->text(0) == QString::fromLatin1("Complete package configuration")) {
                dpkgRow = rowIndex;
            } else if (row->text(0) == QString::fromLatin1("DKMS")) {
                dkmsRow = rowIndex;
            }
        }
        if (dpkgRow < 0 || dkmsRow < 0) {
            problems->append(QString::fromLatin1("tool list rows missing (dpkg/dkms)"));
            ok = false;
        } else {
            QListViewItem *dpkgItem = m_toolList->firstChild();
            for (int step = 0; dpkgItem && step < dpkgRow; ++step) {
                dpkgItem = dpkgItem->nextSibling();
            }
            m_toolList->setSelected(dpkgItem, true);
            m_toolList->setCurrentItem(dpkgItem);
            updateToolDetails();
            if (m_toolTitle->text() != QString::fromLatin1("Complete package configuration")
                || m_toolRunButton->text() != QString::fromLatin1("Complete Configuration")) {
                problems->append(QString::fromLatin1(
                    "Selected tool pane does not follow the selected tool (title='%1', button='%2')")
                    .arg(m_toolTitle->text()).arg(m_toolRunButton->text()));
                ok = false;
            }
            // The Full Repair column mirrors the Settings checkbox when the
            // stage's capability is available (otherwise the exact
            // "Unavailable: ..." reason correctly overrides it).
            const int dpkgPlan = planIndexForStage("dpkg-configure");
            if (dpkgPlan >= 0 && planStageAvailable(dpkgPlan, 0)) {
                // The column-mirror check is state-independent: it pins the
                // checkbox OFF then ON (restoring the prior state afterwards),
                // so a persisted non-default plan override can never invert
                // the expected texts.
                const bool original = m_planChecks[dpkgPlan]->isChecked();
                m_planChecks[dpkgPlan]->setChecked(false);
                const QString offText = dpkgItem->text(1);
                m_planChecks[dpkgPlan]->setChecked(true);
                const QString onText = dpkgItem->text(1);
                m_planChecks[dpkgPlan]->setChecked(original);
                if (offText.find(QString::fromLatin1("Disabled in Settings")) < 0
                    || onText.find(tr("Enabled in Settings")) < 0) {
                    problems->append(QString::fromLatin1(
                        "Full Repair column does not mirror the Settings checkbox"));
                    ok = false;
                }
            }
            // A display-only tool is disabled with the exact reason.
            QListViewItem *dkmsItem = m_toolList->firstChild();
            for (int step = 0; dkmsItem && step < dkmsRow; ++step) {
                dkmsItem = dkmsItem->nextSibling();
            }
            m_toolList->setSelected(dkmsItem, true);
            m_toolList->setCurrentItem(dkmsItem);
            updateToolDetails();
            if (m_toolRunButton->isEnabled()) {
                problems->append(QString::fromLatin1(
                    "display-only DKMS tool has an enabled run button"));
                ok = false;
            }
            if (m_toolPlanStatus->text().find(tr("Unavailable")) < 0) {
                problems->append(QString::fromLatin1(
                    "display-only DKMS tool does not show its unavailable reason"));
                ok = false;
            }
            // The DKMS plan status must carry the reason directly, without a
            // cosmetic leading blank line (its plan text is empty).
            if (m_toolPlanStatus->text().startsWith(QString::fromLatin1("\n"))) {
                problems->append(QString::fromLatin1(
                    "display-only tool plan status starts with a blank line"));
                ok = false;
            }
            // The display tool is the legacy host-scope stage: it runs only
            // with the display capability line available, Host Maintenance
            // active and the host-maintenance feature available; the offline
            // scope keeps it disabled with the host-scope reason.
            int displayRow = -1;
            rowIndex = 0;
            for (QListViewItem *row = m_toolList->firstChild(); row;
                 row = row->nextSibling(), ++rowIndex) {
                if (row->text(0) == QString::fromLatin1("Graphical login / display manager")) {
                    displayRow = rowIndex;
                    break;
                }
            }
            if (displayRow < 0) {
                problems->append(QString::fromLatin1("tool list rows missing (display)"));
                ok = false;
            } else {
                QListViewItem *displayItem = m_toolList->firstChild();
                for (int step = 0; displayItem && step < displayRow; ++step) {
                    displayItem = displayItem->nextSibling();
                }
                m_toolList->setSelected(displayItem, true);
                m_toolList->setCurrentItem(displayItem);
                updateToolDetails();
                if (m_toolRunButton->text() != QString::fromLatin1("Restore Graphical Login")) {
                    problems->append(QString::fromLatin1(
                        "display tool button is '%1', expected Restore Graphical Login")
                        .arg(m_toolRunButton->text()));
                    ok = false;
                }
                std::string displayReason;
                const bool displayCapable = m_model.isAvailable(
                    "display", toStd(identity()), &displayReason);
                std::string maintenanceReason;
                const bool maintenanceAvailable = m_model.legacyFeatureAvailable(
                    "host-maintenance", toStd(identity()), &maintenanceReason);
                const bool displayExpected = displayCapable && maintenanceAvailable
                    && m_hostMaintenance;
                if (m_toolRunButton->isEnabled() != displayExpected) {
                    problems->append(QString::fromLatin1(
                        "display tool runnable=%1 but capability=%2 host-maintenance=%3")
                        .arg(m_toolRunButton->isEnabled()
                                 ? QString::fromLatin1("yes") : QString::fromLatin1("no"))
                        .arg(displayCapable
                                 ? QString::fromLatin1("available") : QString::fromLatin1("unavailable"))
                        .arg(maintenanceAvailable
                                 ? QString::fromLatin1("available") : QString::fromLatin1("unavailable")));
                    ok = false;
                }
                // Round 52 (user request): the offline display tool's info is
                // CAPABILITY-DERIVED. The smoke holds host-scope evidence
                // only, so the offline identity has no matching capability
                // line: the reason names the missing evidence (fail closed) —
                // never the hardcoded host-scope literal; the scope hint
                // itself requires present-DM capability evidence in the
                // target scope (rig-verified by the round-52 probe).
                const bool savedMaintenance = m_hostMaintenance;
                m_hostMaintenance = false;
                updateActionStates();
                updateToolDetails();
                if (m_toolRunButton->isEnabled()) {
                    problems->append(QString::fromLatin1(
                        "display tool enabled for the offline target scope"));
                    ok = false;
                }
                if (m_toolPlanStatus->text().find(tr(
                        "Run diagnostics for the selected scope first.")) < 0) {
                    problems->append(QString::fromLatin1(
                        "display tool offline reason is not capability-derived "
                        "(missing-evidence state)"));
                    ok = false;
                }
                if (m_toolPlanStatus->text().find(QString::fromLatin1(
                        "host-scope stage")) >= 0) {
                    problems->append(QString::fromLatin1(
                        "display tool offline info still carries the hardcoded "
                        "host-scope literal without present-DM evidence"));
                    ok = false;
                }
                m_hostMaintenance = savedMaintenance;
                updateActionStates();
                updateToolDetails();
            }
            // Every tool title plus the placeholder must fit the Selected tool
            // pane at 1024x768.  The assertions are deterministic: the title
            // wraps by construction (Qt::WordBreak + an Expanding horizontal
            // policy), the pane keeps its 300px floor, and after each title
            // is set the label must actually fill the header row (width >=
            // pane - Run button - tolerance), which is what lets long titles
            // wrap inside the full row width in the real (visible-page)
            // rendering.  A wrap-height threshold is deliberately not
            // asserted: Qt3's WordBreak sizeHint/height behavior is
            // style/font-dependent and unreliable headless.
            if (!(m_toolTitle->alignment() & Qt::WordBreak)) {
                problems->append(QString::fromLatin1(
                    "Selected tool title does not wrap (Qt::WordBreak)"));
                ok = false;
            }
            if (m_toolTitle->sizePolicy().horData() != QSizePolicy::Expanding) {
                problems->append(QString::fromLatin1(
                    "Selected tool title lacks the Expanding horizontal policy"));
                ok = false;
            }
            QStringList titleTexts;
            titleTexts.append(tr("Select a repair tool"));
            for (int t = 0; t < toolSpecCount; ++t) {
                titleTexts.append(QString::fromLatin1(toolSpecs[t].title));
            }
            QWidget *titlePane = m_toolTitle->parentWidget();
            // The wrap measurements need the Repair page laid out for real:
            // while the page sits on a hidden tab the Etch Qt3 does not
            // reliably re-run its layouts from processEvents() alone, so the
            // WordBreak row never re-asks its heightForWidth and stays one
            // line at a stale width.  Switch to the page, flush the queued
            // events and explicitly activate every layout in the label's
            // ancestor chain (bottom-up, so the page's top-level layout runs
            // last and re-queries the nested boxes' heightForWidth).
            const int pageBeforeToolTitles = m_tabs->currentPageIndex();
            m_tabs->setCurrentPage(2);
            qApp->sendPostedEvents();
            qApp->processEvents();
            for (QWidget *ancestor = m_toolTitle->parentWidget(); ancestor;
                 ancestor = ancestor->parentWidget()) {
                if (ancestor->layout()) {
                    ancestor->layout()->activate();
                }
            }
            qApp->processEvents();
            if (titlePane->width() < 300) {
                // The pane floor (setMinimumWidth(300)) proves the page did
                // not lay out; report the exact geometry instead of letting
                // the per-title checks fire with a stale-size false positive.
                problems->append(QString::fromLatin1(
                    "Selected tool pane did not lay out (%1px wide, needs the "
                    "300px floor); page=%2 window=%3x%4")
                    .arg(titlePane->width())
                    .arg(m_tabs->currentPageIndex())
                    .arg(width()).arg(height()));
                ok = false;
            }
            for (QStringList::ConstIterator it = titleTexts.begin();
                 it != titleTexts.end(); ++it) {
                m_toolTitle->setText(*it);
                // Two passes deliver the layout invalidation; the ancestor
                // activation then forces the header row to recompute for the
                // new text even on a freshly shown page.
                qApp->processEvents();
                qApp->processEvents();
                for (QWidget *ancestor = m_toolTitle->parentWidget(); ancestor;
                     ancestor = ancestor->parentWidget()) {
                    if (ancestor->layout()) {
                        ancestor->layout()->activate();
                    }
                }
                qApp->processEvents();
                const QRect mapped(
                    m_toolTitle->mapTo(titlePane, QPoint(0, 0)),
                    m_toolTitle->size());
                if (!titlePane->rect().contains(mapped)) {
                    problems->append(QString::fromLatin1(
                        "tool title '%1' overflows the Selected tool pane")
                        .arg(*it));
                    ok = false;
                }
                if (m_toolTitle->width() < 40) {
                    problems->append(QString::fromLatin1(
                        "tool title '%1' has no usable width").arg(*it));
                    ok = false;
                }
                // A12-10: the title is LEFT-aligned at its natural width and
                // the Run button is pinned RIGHT-aligned on the SAME row (the
                // stretch between them absorbs the middle) — the old
                // Expanding-fill contract is gone.  Assert the new geometry:
                // the title stays left (its left edge at the pane's left
                // margin), the button's right edge reaches the pane's right
                // edge (within the layout margins), and neither overlaps the
                // other.
                const int margin = 16;
                if (mapped.left() > margin) {
                    problems->append(QString::fromLatin1(
                        "tool title '%1' is not left-aligned (left edge %2px)")
                        .arg(*it).arg(mapped.left()));
                    ok = false;
                }
                const QRect buttonMapped(
                    m_toolRunButton->mapTo(titlePane, QPoint(0, 0)),
                    m_toolRunButton->size());
                if (titlePane->width() - (buttonMapped.right() + 1) > margin + 8) {
                    problems->append(QString::fromLatin1(
                        "tool run button is not right-aligned for '%1' "
                        "(right gap %2px)")
                        .arg(*it)
                        .arg(titlePane->width() - (buttonMapped.right() + 1)));
                    ok = false;
                }
                if (mapped.right() + 1 >= buttonMapped.left()) {
                    problems->append(QString::fromLatin1(
                        "tool title '%1' overlaps the run button").arg(*it));
                    ok = false;
                }
            }
            // Restore the previous tab and the first tool selection.
            m_tabs->setCurrentPage(pageBeforeToolTitles);
            qApp->processEvents();
            if (m_toolList->firstChild()) {
                m_toolList->setSelected(m_toolList->firstChild(), true);
                m_toolList->setCurrentItem(m_toolList->firstChild());
                updateToolDetails();
            }
        }
    }
    if (!m_diagnosticList
        || m_diagnosticList->childCount() != static_cast<int>(legacy::diagnosticKeys().size())) {
        problems->append(QString::fromLatin1(
            "per-diagnostic check list missing or incomplete (count=%1)")
            .arg(m_diagnosticList ? m_diagnosticList->childCount() : -1));
        ok = false;
    }
    if (m_diagnosticList && m_diagnosticList->columns() != 1) {
        problems->append(QString::fromLatin1(
            "diagnostic check list must carry the single Check column"));
        ok = false;
    }
    if (!m_configCombo || !m_configButton || !m_configLabel || !m_configReasonLabel) {
        problems->append(QString::fromLatin1("target configuration controls missing"));
        ok = false;
    }
    if (!m_runDiagnosticButton || !m_copyResultsButton || !m_saveResultsButton) {
        problems->append(QString::fromLatin1("diagnostic run/copy/save controls missing"));
        ok = false;
    }
    if (!m_scopeLabel || !m_diagTitle || !m_diagDescription || !m_diagAvailability) {
        problems->append(QString::fromLatin1("selected-diagnostic pane missing"));
        ok = false;
    }
    if (m_runDiagnosticButton
        && m_runDiagnosticButton->text() != tr("Run Diagnostic")) {
        problems->append(QString::fromLatin1("Run Diagnostic button text changed: '%1'")
                             .arg(m_runDiagnosticButton->text()));
        ok = false;
    }
    if (m_diagnosticsButton
        && m_diagnosticsButton->text() != tr("Run All")) {
        problems->append(QString::fromLatin1("Run All button text changed: '%1'")
                             .arg(m_diagnosticsButton->text()));
        ok = false;
    }
    if (m_setTargetButton
        && m_setTargetButton->text() != tr("Select Target")) {
        problems->append(QString::fromLatin1("Select Target button text changed: '%1'")
                             .arg(m_setTargetButton->text()));
        ok = false;
    }
    if (m_scanButton
        && m_scanButton->text() != tr("Refresh Devices")) {
        problems->append(QString::fromLatin1("Refresh Devices button text changed: '%1'")
                             .arg(m_scanButton->text()));
        ok = false;
    }

    // B2 menus: File (Refresh Devices, Lock Administrator Session, Quit),
    // View (tab shortcuts, Auto-size Device Columns, checkable Wrap Log Lines)
    // and Help (Using Boot Bitch, About Boot Bitch). Qt3's auto-generated menu
    // ids are NEGATIVE, so validity is tracked separately.
    if (!m_fileMenu || !m_viewMenu || !m_helpMenu || !m_wrapLogsMenuValid) {
        problems->append(QString::fromLatin1("modern menu structure missing"));
        ok = false;
    } else {
        struct MenuItemCheck {
            QPopupMenu *menu;
            const char *label;
        };
        const MenuItemCheck menuChecks[] = {
            { m_fileMenu, "Refresh Devices" },
            { m_fileMenu, "Lock Administrator Session" },
            { m_fileMenu, "Quit" },
            { m_viewMenu, "Systems" },
            { m_viewMenu, "Diagnostics" },
            { m_viewMenu, "Logs" },
            { m_viewMenu, "Settings" },
            { m_viewMenu, "Auto-size Device Columns" },
            { m_viewMenu, "Wrap Log Lines" },
            { m_helpMenu, "Using Boot Bitch" },
            { m_helpMenu, "About Boot Bitch" }
        };
        for (int i = 0; i < static_cast<int>(sizeof(menuChecks) / sizeof(menuChecks[0])); ++i) {
            if (!menuItemId(menuChecks[i].menu,
                            QString::fromLatin1(menuChecks[i].label), 0)) {
                problems->append(QString::fromLatin1("menu item missing: %1")
                                     .arg(QString::fromLatin1(menuChecks[i].label)));
                ok = false;
            }
        }
        if (m_viewMenu->isItemChecked(m_wrapLogsMenuId) != m_logWrapEnabled) {
            problems->append(QString::fromLatin1("Wrap Log Lines menu check is out of sync"));
            ok = false;
        }
    }
    // View tab shortcuts must switch the pages.
    if (m_tabs) {
        const int originalPage = m_tabs->currentPageIndex();
        showLogsTab();
        if (m_tabs->currentPageIndex() != 5) {
            problems->append(QString::fromLatin1("View > Logs did not select the Logs tab"));
            ok = false;
        }
        showSettingsTab();
        if (m_tabs->currentPageIndex() != 6) {
            problems->append(QString::fromLatin1("View > Settings did not select the Settings tab"));
            ok = false;
        }
        showSystemsTab();
        if (m_tabs->currentPageIndex() != 0) {
            problems->append(QString::fromLatin1("View > Systems did not select the Systems tab"));
            ok = false;
        }
        showDiagnosticsTab();
        if (m_tabs->currentPageIndex() != 1) {
            problems->append(QString::fromLatin1("View > Diagnostics did not select the Diagnostics tab"));
            ok = false;
        }
        m_tabs->setCurrentPage(originalPage);
    }
    // Wrap Log Lines (View menu): item 1 removed the Settings checkbox (wrap
    // is on by default), so the smoke verifies the menu toggle round-trips and
    // persists through saveLegacySettings().
    if (m_viewMenu && m_wrapLogsMenuValid) {
        const bool originalWrap = m_logWrapEnabled;
        // View menu item -> state (this also persists through saveLegacySettings).
        toggleLogWrapFromMenu();
        if (m_logWrapEnabled == originalWrap) {
            problems->append(QString::fromLatin1(
                "Wrap Log Lines menu toggle did not change the wrap state"));
            ok = false;
        }
        if (m_viewMenu->isItemChecked(m_wrapLogsMenuId) != m_logWrapEnabled) {
            problems->append(QString::fromLatin1(
                "Wrap Log Lines menu check is out of sync after the menu toggle"));
            ok = false;
        }
        // Toggle again to restore the original state.
        toggleLogWrapFromMenu();
        if (m_logWrapEnabled != originalWrap) {
            problems->append(QString::fromLatin1(
                "Wrap Log Lines menu toggle did not restore the state"));
            ok = false;
        }
        // Persistence: only asserted when the environment can write settings
        // (a read-only home must not fail a correct GUI). Under --smoke-test
        // isolation the user's files are never read or written, so the
        // round-trip probe is skipped and the deterministic defaults stand.
        if (!legacySmokeSettingsIsolation()) {
            QSettings probe;
            const bool settingsWritable = probe.writeEntry(
                QString::fromLatin1("/logs/wrapLines"), m_logWrapEnabled);
            if (settingsWritable) {
                QSettings readBack;
                if (readBack.readBoolEntry(QString::fromLatin1("/logs/wrapLines"),
                                           !m_logWrapEnabled) != m_logWrapEnabled) {
                    problems->append(QString::fromLatin1(
                        "Wrap Log Lines state was not persisted through QSettings"));
                    ok = false;
                }
            }
        }
    }
    // Cycle 9 loop 3: the smoke must run under settings isolation (set by
    // main() before the window was constructed); otherwise a persisted
    // settings file (e.g. the canonical repairrc) could flip the assertions
    // above.
    if (!legacySmokeSettingsIsolation()) {
        problems->append(QString::fromLatin1(
            "smoke ran without the settings isolation"));
        ok = false;
    }
    // Auto-size Device Columns must keep every column visible and the last
    // column stretched (verifyLayout re-checks both afterwards).
    autoSizeDeviceColumns();

    // Logs parity controls: New Session Log / Add Note / Delete / Refresh and
    // the prior-log banner (hidden while the live register is shown).
    if (!m_newSessionLogButton || !m_addNoteButton || !m_deleteSessionLogButton) {
        problems->append(QString::fromLatin1("session log management controls missing"));
        ok = false;
    }
    if (!m_priorLogBanner || m_priorLogBanner->isVisible()) {
        problems->append(QString::fromLatin1("prior-log banner missing or shown for the live register"));
        ok = false;
    }

    // Settings parity: functional device-discovery filters, functional
    // diagnostics auto-refresh and the read-only host capabilities group.
    if (!m_showNonLinuxCheck || !m_showRemovableCheck || !m_showEncryptedCheck
        || !m_autoRefreshCheck) {
        problems->append(QString::fromLatin1("Settings filter/auto-refresh controls missing"));
        ok = false;
    } else if (!m_showNonLinuxCheck->isEnabled() || !m_showRemovableCheck->isEnabled()
               || !m_showEncryptedCheck->isEnabled() || !m_autoRefreshCheck->isEnabled()) {
        problems->append(QString::fromLatin1("Settings filter/auto-refresh controls are disabled"));
        ok = false;
    }
    if (!m_capDistributionLabel || !m_capPackageManagerLabel || !m_capAuthLabel
        || !m_capabilityTable || !m_refreshCapabilitiesButton
        || !m_installSupportButton) {
        problems->append(QString::fromLatin1("host capabilities group missing"));
        ok = false;
    }
    // Modern 1:1 capability table: seven columns (the trailing Action column
    // is a read-only placeholder), the 16 modern-mirrored probe rows plus the
    // two legacy-specific rows (LUKS support and process-namespace isolation),
    // and the disabled Install Missing Support... button with the modern
    // tooltip.
    if (m_capabilityTable) {
        if (m_capabilityTable->numCols() != 7
            || m_capabilityTable->numRows() != capabilitySpecCount) {
            problems->append(QString::fromLatin1(
                "host capability table is %1x%2, expected 18x7")
                .arg(m_capabilityTable->numRows())
                .arg(m_capabilityTable->numCols()));
            ok = false;
        } else {
            const char *expectedHeaders[] = {
                "Feature", "Command", "Scope", "Status",
                "Suggested package", "Notes", "Action"
            };
            for (int column = 0; column < 7; ++column) {
                if (m_capabilityTable->horizontalHeader()->label(column)
                    != QString::fromLatin1(expectedHeaders[column])) {
                    problems->append(QString::fromLatin1(
                        "capability table header %1 changed").arg(column + 1));
                    ok = false;
                }
            }
            bool unshareRow = false;
            for (int row = 0; row < capabilitySpecCount; ++row) {
                if (m_capabilityTable->text(row, 0)
                    == QString::fromLatin1("Process namespace isolation")) {
                    unshareRow = true;
                    if (m_capabilityTable->text(row, 1)
                        != QString::fromLatin1("unshare")) {
                        problems->append(QString::fromLatin1(
                            "unshare capability row lost its command"));
                        ok = false;
                    }
                }
            }
            if (!unshareRow) {
                problems->append(QString::fromLatin1(
                    "legacy unshare capability row missing"));
                ok = false;
            }
        }
        // Full-row selection (modern parity): clicking any cell selects its
        // row, and the first row starts selected after every refresh.
        if (m_capabilityTable->selectionMode() != QTable::SingleRow) {
            problems->append(QString::fromLatin1(
                "capability table is not in QTable::SingleRow selection mode"));
            ok = false;
        }
        refreshCapabilities();
        if (m_capabilityTable->numRows() > 0
            && !m_capabilityTable->isRowSelected(0)) {
            problems->append(QString::fromLatin1(
                "capability table does not select its first row after refresh"));
            ok = false;
        }
        if (m_capabilityTable->numRows() > 3) {
            m_capabilityTable->setCurrentCell(2, 3);
            qApp->processEvents();
            if (!m_capabilityTable->isRowSelected(2)) {
                problems->append(QString::fromLatin1(
                    "clicking a capability cell did not select its row"));
                ok = false;
            }
            m_capabilityTable->setCurrentCell(0, 0);
            m_capabilityTable->selectRow(0);
        }
    }
    if (m_capAuthLabel
        && m_capAuthLabel->text().find(QString::fromLatin1("Not compiled")) < 0) {
        problems->append(QString::fromLatin1(
            "KAuth build support label does not name the unavailable integration"));
        ok = false;
    }
    if (m_installSupportButton
        && m_installSupportButton->text() != tr("Install Missing Support...")) {
        problems->append(QString::fromLatin1("Install Missing Support... text changed: '%1'")
                             .arg(m_installSupportButton->text()));
        ok = false;
    }
    // Item 2: the button is enabled exactly when the selected row's Action cell
    // reads "Install" (a missing, installable row); any other row keeps it
    // disabled. Fail closed either way.
    if (m_installSupportButton && m_capabilityTable
        && m_capabilityTable->numRows() > 0) {
        const int selectedRow = m_capabilityTable->currentRow();
        const bool cellOffersInstall = selectedRow >= 0
            && m_capabilityTable->text(selectedRow, 6) == tr("Install");
        if (m_installSupportButton->isEnabled() != cellOffersInstall) {
            problems->append(QString::fromLatin1(
                "Install Missing Support... enablement does not match the selected row's Action cell"));
            ok = false;
        }
    }
    if (m_installSupportButton
        && QToolTip::textFor(m_installSupportButton).find(
               QString::fromLatin1("Automatic installation will require")) < 0) {
        problems->append(QString::fromLatin1(
            "Install Missing Support... lost the modern tooltip"));
        ok = false;
    }

    // Diagnostics list parity: the friendly titles are shown, the stable keys
    // stay internal.
    if (m_diagnosticList && m_diagnosticList->firstChild()
        && m_diagnosticList->firstChild()->text(0) != QString::fromLatin1("Environment validation")) {
        problems->append(QString::fromLatin1(
            "diagnostic list does not show the friendly titles (first='%1')")
            .arg(m_diagnosticList->firstChild()->text(0)));
        ok = false;
    }

    // Read-only udev metadata probe: a locked LUKS container must be visible in
    // the inventory with its probed type (the GUI never opens the device).
    if (m_deviceList) {
        for (QMap<QString, DeviceRow>::const_iterator it = m_rows.begin();
             it != m_rows.end(); ++it) {
            const DeviceRow &row = it.data();
            if (!row.encrypted || !row.fstype.empty()) {
                continue;
            }
            const std::vector<QListViewItem *> treeItems = deviceTreeItems();
            for (std::size_t ti = 0; ti < treeItems.size(); ++ti) {
                QListViewItem *item = treeItems[ti];
                if (item->text(0) != it.key()) {
                    continue;
                }
                if (item->text(3) != QString::fromLatin1("crypto_LUKS")) {
                    problems->append(QString::fromLatin1(
                        "locked LUKS device %1 is not shown with its probed type (row: '%2')")
                        .arg(it.key()).arg(item->text(3)));
                    ok = false;
                }
                break;
            }
        }
    }

    // Filesystem column: every visible device row carries a label (disk rows
    // aggregate their children, component rows fall back through the
    // mounted/udev/[swap]/unknown chain) - the cell is never empty.
    if (m_deviceList) {
        const std::vector<QListViewItem *> treeItems = deviceTreeItems();
        for (std::size_t i = 0; i < treeItems.size(); ++i) {
            if (treeItems[i]->text(3).isEmpty()) {
                problems->append(QString::fromLatin1(
                    "device row %1 has an empty Filesystem cell")
                    .arg(treeItems[i]->text(0)));
                ok = false;
            }
        }
        // Cycle 15: the protected running host must NOT appear in the
        // candidate tree (the host card above shows it); only offline drives
        // are repair candidates.
        const QString hostDisk = runningHostDisk();
        if (!hostDisk.isEmpty()) {
            for (QListViewItem *item = m_deviceList->firstChild(); item;
                 item = item->nextSibling()) {
                if (item->text(0) == hostDisk) {
                    problems->append(QString::fromLatin1(
                        "the protected running host %1 must not appear in the "
                        "candidate tree").arg(hostDisk));
                    ok = false;
                }
            }
        }
    }

    // Device-discovery filters: unchecking "Show encrypted devices before
    // unlocking" hides the encrypted drive's rows (and never loses the
    // selected/committed target state); restoring shows them again.
    if (m_showEncryptedCheck && m_showEncryptedCheck->isChecked() && m_deviceList) {
        const QString selectedBefore = selectedDisk();
        const int rowsBefore = m_deviceList->childCount();
        m_showEncryptedCheck->setChecked(false);
        const int rowsFiltered = m_deviceList->childCount();
        if (rowsFiltered > rowsBefore) {
            problems->append(QString::fromLatin1(
                "device-discovery filter added rows instead of hiding them"));
            ok = false;
        }
        if (!selectedBefore.isEmpty() && selectedDisk() != selectedBefore) {
            problems->append(QString::fromLatin1(
                "hiding the selected drive's row lost the selected target state"));
            ok = false;
        }
        m_showEncryptedCheck->setChecked(true);
        if (m_deviceList->childCount() != rowsBefore) {
            problems->append(QString::fromLatin1(
                "restoring the device-discovery filter did not restore the rows"));
            ok = false;
        }
    }

    // B7-2: the device list is a tree - top-level items are disks only and
    // their children are the disk's partitions/mappers; exactly four columns.
    // Orphan non-disk rows (/dev/fd0 floppy stubs, /dev/hdc empty devices,
    // helper-reported pass-through rows with no resolvable owner) must never
    // appear top-level. Rows the inventory does not know are exempt here
    // (helper-reported devices merged straight into the list before a
    // rebuild); the builder itself drops every orphan.
    if (m_deviceList && m_deviceList->columns() != 4) {
        problems->append(QString::fromLatin1(
            "device tree must carry exactly Device/Size/Type/Filesystem columns"));
        ok = false;
    }
    if (m_deviceList) {
        for (QListViewItem *disk = m_deviceList->firstChild(); disk;
             disk = disk->nextSibling()) {
            const QMap<QString, DeviceRow>::const_iterator row =
                m_rows.find(disk->text(0));
            if (row == m_rows.end()) {
                // Helper-reported pass-through rows merged before a rebuild.
                continue;
            }
            if (!row.data().disk) {
                problems->append(QString::fromLatin1(
                    "device tree top-level row '%1' is not a disk")
                    .arg(disk->text(0)));
                ok = false;
                continue;
            }
            const QString diskPath = disk->text(0);
            for (QListViewItem *child = disk->firstChild(); child;
                 child = child->nextSibling()) {
                const QMap<QString, DeviceRow>::const_iterator childRow =
                    m_rows.find(child->text(0));
                if (childRow == m_rows.end()) {
                    continue;
                }
                if (childRow.data().disk
                    || owningDiskFor(childRow.data()) != diskPath) {
                    problems->append(QString::fromLatin1(
                        "device tree child '%1' does not belong to its disk '%2'")
                        .arg(child->text(0)).arg(diskPath));
                    ok = false;
                }
            }
        }
    }

    // B7-1: a drive with a locked LUKS component and an unencrypted Linux
    // partition must NOT resolve that partition (unlock-first; Select Target
    // disabled with the modern reason); after the mapped Linux root appears,
    // the mapper resolves and the locked candidate disappears.
    {
        const QString fakeDisk = QString::fromLatin1("/dev/bootrepair-smoke-luks");
        const QString fakePart = QString::fromLatin1("/dev/bootrepair-smoke-luks1");
        const QString fakeLuks = QString::fromLatin1("/dev/bootrepair-smoke-luks5");
        const QString fakeRoot = QString::fromLatin1("/dev/mapper/bootrepair-smoke-root");
        DeviceRow dk; dk.name = "bootrepair-smoke-luks"; dk.path = toStd(fakeDisk); dk.disk = true;
        DeviceRow part; part.name = "bootrepair-smoke-luks1"; part.path = toStd(fakePart);
        part.parent = "bootrepair-smoke-luks"; part.probedFstype = "ext3";
        DeviceRow luks; luks.name = "bootrepair-smoke-luks5"; luks.path = toStd(fakeLuks);
        luks.parent = "bootrepair-smoke-luks"; luks.encrypted = true;
        luks.probedFstype = "crypto_LUKS";
        const QString savedDisk = selectedDisk();
        const QString savedRoot = selectedRoot();
        m_rows.insert(fakeDisk, dk);
        m_rows.insert(fakePart, part);
        m_rows.insert(fakeLuks, luks);
        setTarget(fakeDisk, QString::null);
        updateActionStates();
        if (!autoResolvedRoot(fakeDisk).isEmpty()) {
            problems->append(QString::fromLatin1(
                "locked-LUKS drive resolved a plain partition root (unlock-first regression)"));
            ok = false;
        }
        if (m_setTargetButton && m_setTargetButton->isEnabled()) {
            problems->append(QString::fromLatin1(
                "Select Target enabled for a locked-LUKS drive"));
            ok = false;
        }
        if (m_setTargetButton
            && QToolTip::textFor(m_setTargetButton).find(
                   QString::fromLatin1("Unlock the encrypted volume first")) < 0) {
            problems->append(QString::fromLatin1(
                "Select Target unlock-first reason missing for a locked-LUKS drive"));
            ok = false;
        }
        DeviceRow mapped; mapped.name = "bootrepair-smoke-root"; mapped.path = toStd(fakeRoot);
        mapped.mapper = true; mapped.parent = "bootrepair-smoke-luks5";
        mapped.fstype = "ext3";
        m_rows.insert(fakeRoot, mapped);
        if (autoResolvedRoot(fakeDisk) != fakeRoot) {
            problems->append(QString::fromLatin1(
                "unlocked mapper root did not resolve after the LUKS child appeared"));
            ok = false;
        }
        if (!autoResolvedLuks(fakeDisk).isEmpty()) {
            problems->append(QString::fromLatin1(
                "autoResolvedLuks still reports a locked candidate after unlock"));
            ok = false;
        }
        m_rows.remove(fakeDisk);
        m_rows.remove(fakePart);
        m_rows.remove(fakeLuks);
        m_rows.remove(fakeRoot);
        setTarget(savedDisk, savedRoot);
        updateActionStates();
    }

    // Cycle 13 loop: a root-less non-LUKS drive (the vfat share disk) must
    // show the no-Linux-filesystem reason - never the unlock-first wording.
    {
        const QString savedDisk2 = selectedDisk();
        const QString savedRoot2 = selectedRoot();
        const QString fakeVfat = QString::fromLatin1("/dev/bootrepair-smoke-vfat");
        DeviceRow vfat;
        vfat.name = "bootrepair-smoke-vfat";
        vfat.path = toStd(fakeVfat);
        vfat.disk = true;
        vfat.probedFstype = "vfat";
        m_rows.insert(fakeVfat, vfat);
        m_inventory.push_back(vfat);
        const bool savedHostMaintenance = m_hostMaintenance;
        m_hostMaintenance = false;
        if (m_hostMaintenanceButton) {
            updateButtonText(m_hostMaintenanceButton,
                             tr("Host Maintenance"));
        }
        setTarget(fakeVfat, QString::null);
        updateActionStates();
        if (m_setTargetButton && !m_setTargetButton->isEnabled()) {
            problems->append(QString::fromLatin1(
                "Select Target disabled for a selectable root-less vfat drive"));
            ok = false;
        }
        if (m_setTargetButton
            && QToolTip::textFor(m_setTargetButton).find(
                   tr("Commit this physical drive as the repair target.")) < 0) {
            problems->append(QString::fromLatin1(
                "Select Target lost the commit tooltip for a vfat drive"));
            ok = false;
        }
        m_rows.remove(fakeVfat);
        for (std::vector<DeviceRow>::iterator it = m_inventory.begin();
             it != m_inventory.end(); ++it) {
            if (fromStd(it->path) == fakeVfat) {
                m_inventory.erase(it);
                break;
            }
        }
        setTarget(savedDisk2, savedRoot2);
        m_hostMaintenance = savedHostMaintenance;
        updateActionStates();
    }

    // Fail closed for the protected running host: Unlock and Select Target must
    // stay disabled and their tooltips must name Host Maintenance as the path.
    const QString hostDisk = runningHostDisk();
    m_hostMaintenance = false;
    if (m_hostMaintenanceButton) {
        updateButtonText(m_hostMaintenanceButton, tr("Host Maintenance"));
    }
    if (!hostDisk.isEmpty()) {
        setTarget(hostDisk, selectedRoot());
    }
    updateActionStates();
    if (m_unlockButton && m_unlockButton->isEnabled()) {
        problems->append(QString::fromLatin1("unlock button enabled for the running host scope"));
        ok = false;
    }
    if (!hostDisk.isEmpty() && m_unlockButton
        && QToolTip::textFor(m_unlockButton).find(tr("Host Maintenance")) < 0) {
        problems->append(QString::fromLatin1(
            "unlock tooltip for the protected host does not name Host Maintenance"));
        ok = false;
    }
    if (!hostDisk.isEmpty() && m_setTargetButton
        && QToolTip::textFor(m_setTargetButton).find(tr("Host Maintenance")) < 0) {
        problems->append(QString::fromLatin1(
            "Select Target tooltip for the protected host does not name Host Maintenance"));
        ok = false;
    }
    if (!hostDisk.isEmpty() && m_setTargetButton && m_setTargetButton->isEnabled()) {
        problems->append(QString::fromLatin1(
            "Select Target enabled for the protected running-host disk"));
        ok = false;
    }
    // The protected host must be shown as protected in the details pane.
    if (!hostDisk.isEmpty() && m_detailList) {
        bool protectedShown = false;
        for (QListViewItem *item = m_detailList->firstChild(); item;
             item = item->nextSibling()) {
            if (item->text(0) == tr("Protection:")
                && item->text(1).contains(tr("PROTECTED"))) {
                protectedShown = true;
                break;
            }
        }
        if (!protectedShown) {
            problems->append(QString::fromLatin1(
                "protected running host is not shown as PROTECTED in the details pane"));
            ok = false;
        }
    }

    // Modern gating: Run All needs a committed repair target or active Host
    // Maintenance, never just an auto-filled selection.
    if (m_diagnosticsButton->isEnabled()) {
        problems->append(QString::fromLatin1("Run All enabled without a committed target or Host Maintenance"));
        ok = false;
    }
    if (m_configButton && m_configButton->isEnabled()) {
        problems->append(QString::fromLatin1("configuration viewer enabled for the running host scope"));
        ok = false;
    }
    if (m_scopeLabel
        && m_scopeLabel->text() != tr("Target: none selected")) {
        problems->append(QString::fromLatin1(
            "scope label is '%1', expected 'Target: none selected'")
            .arg(m_scopeLabel->text()));
        ok = false;
    }

    // Target eligibility without a scope selector: any selectable
    // non-running-host disk can be committed, the protected running-host disk
    // stays excluded.
    const QString previousDisk = selectedDisk();
    const QString previousRoot = selectedRoot();
    setTarget(QString::fromLatin1("/dev/bootrepair-smoke-target"),
              QString::fromLatin1("/dev/bootrepair-smoke-target1"));
    updateActionStates();
    if (m_setTargetButton && !m_setTargetButton->isEnabled()) {
        problems->append(QString::fromLatin1(
            "Select Target disabled for a selectable non-host disk"));
        ok = false;
    }
    if (m_unlockButton && m_unlockButton->isEnabled()) {
        problems->append(QString::fromLatin1(
            "Unlock enabled for a disk without a locked LUKS component"));
        ok = false;
    }
    if (administratorSessionActive()) {
        // Exercise the real commit path (the smoke runs as root, with
        // --no-elevate or with an authenticated `sudo -n`).
        setRepairTarget();
        if (!targetCommitted() || !m_diagnosticsButton->isEnabled()) {
            problems->append(QString::fromLatin1(
                "committing a selectable non-host disk did not enable diagnostics"));
            ok = false;
        }
        if (m_scopeLabel
            && !m_scopeLabel->text().contains(QString::fromLatin1(
                "Target:\n/dev/bootrepair-smoke-target + /dev/bootrepair-smoke-target1"))) {
            problems->append(QString::fromLatin1(
                "committed scope label is '%1'").arg(m_scopeLabel->text()));
            ok = false;
        }
        // Item #1 (modern parity): the committed drive DISABLES Select Target
        // (its row is already the committed target) but keeps the committed
        // tooltip; a different selectable drive stays selectable.
        updateActionStates();
        if (m_setTargetButton && m_setTargetButton->isEnabled()) {
            problems->append(QString::fromLatin1(
                "Select Target enabled for the already-committed drive"));
            ok = false;
        }
        if (m_setTargetButton
            && QToolTip::textFor(m_setTargetButton).find(
                   QString::fromLatin1("Committed repair target")) < 0) {
            problems->append(QString::fromLatin1(
                "Select Target lost the committed tooltip"));
            ok = false;
        }
        setTarget(QString::fromLatin1("/dev/bootrepair-smoke-other"),
                  QString::fromLatin1("/dev/bootrepair-smoke-other1"));
        updateActionStates();
        if (m_setTargetButton && !m_setTargetButton->isEnabled()) {
            problems->append(QString::fromLatin1(
                "Select Target disabled for a different selectable drive"));
            ok = false;
        }
        m_targetCommitted = false;
        m_committedDisk = QString::null;
        m_committedRoot = QString::null;
    }
    if (!previousDisk.isEmpty()) {
        setTarget(previousDisk, previousRoot);
    }
    updateActionStates();

    m_hostMaintenance = true;
    updateActionStates();
    if (!m_diagnosticsButton->isEnabled()) {
        problems->append(QString::fromLatin1("Run All disabled while Host Maintenance is active"));
        ok = false;
    }
    if (!m_runDiagnosticButton->isEnabled()) {
        problems->append(QString::fromLatin1("Run Diagnostic disabled while Host Maintenance is active"));
        ok = false;
    }
    if (m_scopeLabel
        && !m_scopeLabel->text().startsWith(QString::fromLatin1("Host maintenance:\n"))) {
        problems->append(QString::fromLatin1(
            "host scope label is '%1'").arg(m_scopeLabel->text()));
        ok = false;
    }
    // Modern parity: the Chroot Shell heading/tab/button switch to host mode.
    if (m_chrootHeading && m_chrootHeading->text() != tr("Host shell")) {
        problems->append(QString::fromLatin1("Chroot Shell heading did not switch to Host shell"));
        ok = false;
    }
    if (m_shellRunButton && m_shellRunButton->text() != tr("Run on Host")) {
        problems->append(QString::fromLatin1("shell run button did not switch to Run on Host"));
        ok = false;
    }
    if (m_tabs && m_tabs->label(3) != tr("Host Shell")) {
        problems->append(QString::fromLatin1("Chroot Shell tab label did not switch to Host Shell"));
        ok = false;
    }
    // Modern parity: the target configuration row (and Edit Target File) is
    // never shown or enabled for Host Maintenance.
    if (m_configCombo && !m_configCombo->isHidden()) {
        problems->append(QString::fromLatin1(
            "target configuration row shown for Host Maintenance"));
        ok = false;
    }
    if (m_configButton && m_configButton->isEnabled()) {
        problems->append(QString::fromLatin1(
            "Edit Target File enabled for Host Maintenance"));
        ok = false;
    }
    m_hostMaintenance = false;
    m_targetCommitted = true;
    m_committedDisk = selectedDisk();
    m_committedRoot = selectedRoot();
    updateActionStates();
    if (!m_diagnosticsButton->isEnabled()) {
        problems->append(QString::fromLatin1("Run All disabled for a committed repair target"));
        ok = false;
    }
    if (m_configCombo && m_configCombo->isHidden()) {
        problems->append(QString::fromLatin1(
            "target configuration row hidden for a committed repair target"));
        ok = false;
    }
    if (m_configCombo && m_configCombo->count() != configSpecCount) {
        problems->append(QString::fromLatin1(
            "target configuration list incomplete for an unprobed target (count=%1)")
            .arg(m_configCombo->count()));
        ok = false;
    }
    if (m_configButton && !m_configButton->isEnabled()) {
        problems->append(QString::fromLatin1("Edit Target File disabled for a committed repair target"));
        ok = false;
    }
    if (m_configCombo && !m_configCombo->isHidden()
        && !comboTextFits(m_configCombo, QString::fromLatin1("configuration combo"), problems)) {
        ok = false;
    }
    if (m_scopeLabel
        && !m_scopeLabel->text().contains(QString::fromLatin1("Target:\n"))) {
        problems->append(QString::fromLatin1(
            "committed scope label is '%1'").arg(m_scopeLabel->text()));
        ok = false;
    }
    m_targetCommitted = false;
    updateActionStates();
    if (m_diagnosticsButton->isEnabled()) {
        problems->append(QString::fromLatin1("Run All enabled for an uncommitted target"));
        ok = false;
    }
    if (m_configCombo && !m_configCombo->isHidden()) {
        problems->append(QString::fromLatin1(
            "target configuration row shown for an uncommitted target"));
        ok = false;
    }
    if (m_configButton && m_configButton->isEnabled()) {
        problems->append(QString::fromLatin1(
            "Edit Target File enabled for an uncommitted target"));
        ok = false;
    }
    // Restore the running-host scope the smoke's helper commands used.
    m_hostMaintenance = true;
    if (m_hostMaintenanceButton) {
        updateButtonText(m_hostMaintenanceButton, tr("Exit Host Maintenance"));
    }
    updateActionStates();

    // The helper commands already ran, so no modal authorization may be
    // pending: the smoke requires root, --no-elevate or `sudo -n`.
    if (!administratorSessionActive()) {
        problems->append(QString::fromLatin1(
            "smoke session is not authorized although helper commands ran"));
        ok = false;
    }
    // Cycle 11 deferred-only parity: in the authorized (root) smoke state the
    // Authorize affordance must be HIDDEN (its text may be empty by design);
    // it appears only in the deferred state (a ready scope without an active
    // session). The smoke runs as root with the session permanently active,
    // so the deferred/shown state cannot be exercised here.
    if (m_authStatusLabel && !m_authStatusLabel->isHidden()) {
        problems->append(QString::fromLatin1(
            "authorization status label must be hidden while the session is active"));
        ok = false;
    }
    if (m_authorizeButton && !m_authorizeButton->isHidden()) {
        problems->append(QString::fromLatin1(
            "Authorize button must be hidden while the session is active"));
        ok = false;
    }

    // Probe-based legacy feature gating (fail closed): the chroot/file-copy
    // probe reasons and the host-maintenance-gated tools must follow the
    // cached `Legacy feature` lines, never hardcoded text.
    // Chroot Shell gating: the controls must be exactly as enabled as the
    // scope's cached feature probe plus the committed scope and the cached
    // administrator session allow (fail closed).
    {
        const QString shellFeature = scopeFeatureKey();
        std::string shellFeatureReason;
        const bool shellFeatureAvailable = m_model.legacyFeatureAvailable(
            toStd(shellFeature), toStd(identity()), &shellFeatureReason);
        const bool shellExpected = shellFeatureAvailable
            && diagnosticsScopeReady() && administratorSessionActive();
        if (m_shellRunButton && m_shellRunButton->isEnabled() != shellExpected) {
            problems->append(QString::fromLatin1(
                "chroot shell enabled=%1 but feature=%2 scope-ready=%3 authorized=%4")
                .arg(m_shellRunButton->isEnabled()
                         ? QString::fromLatin1("yes") : QString::fromLatin1("no"))
                .arg(shellFeatureAvailable
                         ? QString::fromLatin1("available") : QString::fromLatin1("unavailable"))
                .arg(diagnosticsScopeReady()
                         ? QString::fromLatin1("yes") : QString::fromLatin1("no"))
                .arg(administratorSessionActive()
                         ? QString::fromLatin1("yes") : QString::fromLatin1("no")));
            ok = false;
        }
        if (m_shellCommandEdit
            && m_shellCommandEdit->isEnabled() != shellExpected) {
            problems->append(QString::fromLatin1(
                "chroot shell command field does not follow the shell gating"));
            ok = false;
        }
    }
    // Host-maintenance feature gate: the host initramfs/grub tools must be
    // exactly as runnable as the cached `Legacy feature host-maintenance:` line
    // allows (both also need their capability line).
    std::string maintenanceReason;
    const bool maintenanceAvailable = m_model.legacyFeatureAvailable(
        "host-maintenance", toStd(identity()), &maintenanceReason);
    static const char *const gatedKeys[] = { "initramfs", "grub" };
    for (int i = 0; i < 2; ++i) {
        int toolIndex = -1;
        for (int t = 0; t < toolSpecCount; ++t) {
            if (QString::fromLatin1(toolSpecs[t].key) == QString::fromLatin1(gatedKeys[i])) {
                toolIndex = t;
                break;
            }
        }
        if (toolIndex < 0) {
            problems->append(QString::fromLatin1("host-maintenance-gated tool missing: %1")
                                 .arg(QString::fromLatin1(gatedKeys[i])));
            ok = false;
            continue;
        }
        QString runReason;
        const bool runnable = toolRunReady(toolIndex, &runReason);
        std::string capabilityReason;
        const bool capabilityAvailable = m_model.isAvailable(
            gatedKeys[i], toStd(identity()), &capabilityReason);
        const bool expected = capabilityAvailable && maintenanceAvailable;
        if (runnable != expected) {
            problems->append(QString::fromLatin1(
                "host %1 tool runnable=%2 but capability=%3 host-maintenance=%4")
                .arg(QString::fromLatin1(gatedKeys[i]))
                .arg(runnable ? QString::fromLatin1("yes") : QString::fromLatin1("no"))
                .arg(capabilityAvailable ? QString::fromLatin1("available") : QString::fromLatin1("unavailable"))
                .arg(maintenanceAvailable ? QString::fromLatin1("available") : QString::fromLatin1("unavailable")));
            ok = false;
        }
    }

    // Target configuration probe: after the running-host diagnostics the
    // target identity still has no cached `Legacy config` lines, so the row
    // lists every Etch key and the reason label asks for diagnostics; a
    // committed target with a cached unavailable line must omit that file and
    // show the helper's exact reason instead.
    if (m_configCombo && m_configReasonLabel) {
        static const char *const expectedKeys[] = {
            "fstab", "inittab", "menu-lst", "crypttab", "modules", "interfaces",
            "sources-list", "apt-conf"
        };
        const std::vector<std::string> keys = legacy::configFileKeys();
        bool keysMatch = keys.size() == 8;
        for (std::size_t i = 0; keysMatch && i < keys.size(); ++i) {
            keysMatch = keys[i] == expectedKeys[i];
        }
        if (!keysMatch) {
            problems->append(QString::fromLatin1(
                "Etch configuration key list changed (expected 8 fixed keys)"));
            ok = false;
        }
        m_hostMaintenance = false;
        m_targetCommitted = true;
        m_committedDisk = selectedDisk();
        m_committedRoot = selectedRoot();
        updateActionStates();
        if (m_configCombo->count() != static_cast<int>(keys.size())) {
            problems->append(QString::fromLatin1(
                "unprobed target configuration list is incomplete (count=%1)")
                .arg(m_configCombo->count()));
            ok = false;
        }
        if (m_configReasonLabel->text().find(QString::fromLatin1("Run diagnostics")) < 0) {
            problems->append(QString::fromLatin1(
                "unprobed target configuration reason does not ask for diagnostics"));
            ok = false;
        }
        m_targetCommitted = false;
        m_hostMaintenance = true;
        updateActionStates();
    }

    // Log filter (modern 1:1): All entries / Diagnostics / Repairs / the three
    // workflows / the 18 diagnostic sections, plus the capture-time kind
    // tagging: the smoke's real host-diagnose lines are Diagnostic entries
    // tagged per `Diagnostic: <key>` marker and the host-validate lines are
    // Repair entries with the validate stage.
    if (m_logFilterCombo && m_logView) {
        m_viewingPriorLog = false;
        if (m_logFilterCombo->text(0) != QString::fromLatin1("All entries")
            || m_logFilterCombo->text(1) != tr("Diagnostics")
            || m_logFilterCombo->text(2) != QString::fromLatin1("Repairs")
            || m_logFilterCombo->text(3) != QString::fromLatin1("File system repair")
            || m_logFilterCombo->text(4) != QString::fromLatin1("Package repair")
            || m_logFilterCombo->text(5) != tr("File copy")
            || m_logFilterCombo->text(6) != QString::fromLatin1(diagnosticSpecs[0].title)
            || m_logFilterCombo->text(logFilterSpecCount + diagnosticSpecCount - 1)
                   != QString::fromLatin1(diagnosticSpecs[diagnosticSpecCount - 1].title)) {
            problems->append(QString::fromLatin1(
                "log filter combo is not 1:1 with modern order"));
            ok = false;
        }
        // Synthetic tagged entries: the workflow and section mappings are
        // deterministic even when the helper transcript varies.
        LogEntry fsEntry;
        fsEntry.text = QString::fromLatin1("SMOKE-KIND fs-inspect synthetic line");
        fsEntry.kind = LogEntry::Repair;
        fsEntry.stages = QString::fromLatin1("fs-inspect");
        LogEntry pkgEntry;
        pkgEntry.text = QString::fromLatin1("SMOKE-KIND fix-broken synthetic line");
        pkgEntry.kind = LogEntry::Repair;
        pkgEntry.stages = QString::fromLatin1("fix-broken");
        LogEntry diagEntry;
        diagEntry.text = QString::fromLatin1("SMOKE-KIND environment synthetic line");
        diagEntry.kind = LogEntry::Diagnostic;
        diagEntry.diagnostic = QString::fromLatin1("environment");
        m_logEntries.push_back(fsEntry);
        m_logEntries.push_back(pkgEntry);
        m_logEntries.push_back(diagEntry);

        struct FilterExpectation {
            int index;              // combo row
            const char *mustShow;   // substring that must appear
            const char *mustHide;   // substring that must not appear
        };
        const FilterExpectation expectations[] = {
            { 1, "SMOKE-KIND environment synthetic line", "SMOKE-KIND fs-inspect synthetic line" },
            { 2, "SMOKE-KIND fs-inspect synthetic line", "SMOKE-KIND environment synthetic line" },
            { 3, "SMOKE-KIND fs-inspect synthetic line", "SMOKE-KIND fix-broken synthetic line" },
            { 4, "SMOKE-KIND fix-broken synthetic line", "SMOKE-KIND fs-inspect synthetic line" },
            { 5, "", "SMOKE-KIND" },
            { 6, "SMOKE-KIND environment synthetic line", "SMOKE-KIND fix-broken synthetic line" }
        };
        for (int i = 0; i < 6; ++i) {
            m_logFilterCombo->setCurrentItem(expectations[i].index);
            refreshLogView();
            const QString view = m_logView->text();
            if (expectations[i].mustShow[0] != '\0'
                && view.find(QString::fromLatin1(expectations[i].mustShow)) < 0) {
                problems->append(QString::fromLatin1(
                    "log filter row %1 did not show its entries").arg(expectations[i].index));
                ok = false;
            }
            if (expectations[i].mustHide[0] != '\0'
                && view.find(QString::fromLatin1(expectations[i].mustHide)) >= 0) {
                problems->append(QString::fromLatin1(
                    "log filter row %1 leaked entries").arg(expectations[i].index));
                ok = false;
            }
        }
        // The real stream markers tagged the smoke diagnostics per section:
        // pick the first section that really appears in the register and
        // verify its filter shows the helper's marker line while the
        // repair/validate lines stay hidden.
        int sectionRow = -1;
        QString sectionMarker;
        for (int d = 0; d < diagnosticSpecCount; ++d) {
            const QString key = QString::fromLatin1(diagnosticSpecs[d].key);
            bool seen = false;
            for (std::size_t e = 0; e < m_logEntries.size() && !seen; ++e) {
                if (m_logEntries[e].kind == LogEntry::Diagnostic
                    && m_logEntries[e].diagnostic == key) {
                    seen = true;
                }
            }
            if (seen) {
                sectionRow = logFilterSpecCount + d;
                sectionMarker = QString::fromLatin1("Diagnostic: %1").arg(key);
                break;
            }
        }
        if (sectionRow < 0) {
            problems->append(QString::fromLatin1(
                "smoke diagnostics did not tag any diagnostic section"));
            ok = false;
        } else {
            m_logFilterCombo->setCurrentItem(sectionRow);
            refreshLogView();
            if (m_logView->text().find(sectionMarker) < 0) {
                problems->append(QString::fromLatin1(
                    "diagnostic section filter did not show the tagged section"));
                ok = false;
            }
            if (m_logView->text().find(QString::fromLatin1("=== smoke host-validate ===")) >= 0) {
                problems->append(QString::fromLatin1(
                    "diagnostic section filter leaked the repair lines"));
                ok = false;
            }
        }
        // The Repairs filter shows the smoke validate run; the Package repair
        // workflow does not (validate maps to no workflow).
        m_logFilterCombo->setCurrentItem(2);
        refreshLogView();
        if (m_logView->text().find(QString::fromLatin1("=== smoke host-validate ===")) < 0) {
            problems->append(QString::fromLatin1(
                "Repairs filter did not show the repair entries"));
            ok = false;
        }
        m_logFilterCombo->setCurrentItem(4);
        refreshLogView();
        if (m_logView->text().find(QString::fromLatin1("=== smoke host-validate ===")) >= 0) {
            problems->append(QString::fromLatin1(
                "Package repair workflow leaked the validate stage"));
            ok = false;
        }
        // Remove the synthetic entries so they never reach a saved log.
        m_logEntries.pop_back();
        m_logEntries.pop_back();
        m_logEntries.pop_back();
        m_logFilterCombo->setCurrentItem(0);
        refreshLogView();
        if (!m_logView->text().contains(QString::fromLatin1("Diagnostic: "))
            || !m_logView->text().contains(QString::fromLatin1("=== smoke host-validate ==="))) {
            problems->append(QString::fromLatin1(
                "All entries did not restore the complete register"));
            ok = false;
        }
    }

    // Log search: matching entries only, restoring every entry when cleared.
    if (m_logSearchEdit && m_logView) {
        m_logSearchEdit->setText(QString::fromLatin1("SMOKE-SEARCH"));
        appendLog(QString::fromLatin1("SMOKE-SEARCH searchable line"));
        if (!m_logView->text().contains(QString::fromLatin1("SMOKE-SEARCH searchable line"))) {
            problems->append(QString::fromLatin1("log search did not show a matching line"));
            ok = false;
        }
        m_logSearchEdit->setText(QString::fromLatin1("NO-SUCH-SMOKE-TEXT"));
        if (m_logView->text().contains(QString::fromLatin1("SMOKE-SEARCH searchable line"))) {
            problems->append(QString::fromLatin1("log search did not hide non-matching lines"));
            ok = false;
        }
        m_logSearchEdit->setText(QString::null);
        if (!m_logView->text().contains(QString::fromLatin1("SMOKE-SEARCH searchable line"))) {
            problems->append(QString::fromLatin1("log search did not restore every entry when cleared"));
            ok = false;
        }
    }

    // Round 34 (Etch #4): Etch-safe separators + one header per section.
    // The separator must be the ASCII dash form "---- CATEGORY ----" (the
    // box-drawing U+2500 run garbles on the Etch Qt3 fonts/encoding) and the
    // header frequency must be one per section block, never one per line.
    if (m_logView) {
        const QString logText = m_logView->text();
        if (logText.find(QString::fromLatin1("---- DIAGNOSTIC ----")) < 0
            || logText.find(QString::fromLatin1("---- REPAIR ----")) < 0
            || logText.find(QString::fromLatin1("---- APPLICATION ----")) < 0) {
            problems->append(QString::fromLatin1(
                "log view lost the ASCII section headers"));
            ok = false;
        }
        if (logText.find(QString::fromLatin1("\xE2\x94\x80")) >= 0) {
            problems->append(QString::fromLatin1(
                "log view still carries the box-drawing separator"));
            ok = false;
        }
        int diagHeaders = 0;
        int pos = 0;
        while ((pos = logText.find(QString::fromLatin1("---- DIAGNOSTIC ----"), pos)) >= 0) {
            ++diagHeaders;
            pos += 18;
        }
        // Expected headers: count the section-identity transitions across the
        // whole non-blank register (kind + diagnostic key + stage set, the
        // same identity formattedLogEntry groups by) and count the transitions
        // that open a Diagnostic block.
        int diagLines = 0;
        int diagBlocks = 0;
        QString lastIdentity;
        for (std::size_t e = 0; e < m_logEntries.size(); ++e) {
            const LogEntry &entry = m_logEntries[e];
            if (entry.text.isNull() || entry.text.isEmpty()) {
                continue;
            }
            const QString identity = QString::fromLatin1("%1\x1f%2\x1f%3")
                .arg(static_cast<int>(entry.kind))
                .arg(entry.diagnostic)
                .arg(entry.stages);
            if (identity != lastIdentity) {
                if (entry.kind == LogEntry::Diagnostic) {
                    ++diagBlocks;
                }
                lastIdentity = identity;
            }
            if (entry.kind == LogEntry::Diagnostic) {
                ++diagLines;
            }
        }
        if (diagLines > 0 && diagHeaders != diagBlocks) {
            problems->append(QString::fromLatin1(
                "log headers are not one per diagnostic section block "
                "(%1 headers, %2 blocks)")
                .arg(diagHeaders).arg(diagBlocks));
            ok = false;
        }
        if (diagLines > 0 && diagHeaders >= diagLines) {
            problems->append(QString::fromLatin1(
                "log emits a DIAGNOSTIC header per line (no section grouping)"));
            ok = false;
        }
    }

    // B7-4: the host-validate run produced the Repair-tagged summary block
    // with the unchanged Validate line and the no-repair-needed headline.
    {
        bool sawBracket = false;
        bool sawValidateLine = false;
        bool sawHeadline = false;
        for (std::size_t e = 0; e < m_logEntries.size(); ++e) {
            const LogEntry &entry = m_logEntries[e];
            if (entry.text.find(QString::fromLatin1("==== REPAIR ====")) >= 0) {
                sawBracket = true;
                if (entry.kind != LogEntry::Repair) {
                    problems->append(QString::fromLatin1(
                        "repair summary block is not Repair-tagged"));
                    ok = false;
                }
            }
            if (entry.text.find(QString::fromLatin1("[-] Validate - validation is read-only")) >= 0
                && entry.kind == LogEntry::Repair) {
                sawValidateLine = true;
            }
            if (entry.text.find(QString::fromLatin1("no repair was needed; cached diagnostics remain valid.")) >= 0) {
                sawHeadline = true;
            }
        }
        if (!sawBracket) {
            problems->append(QString::fromLatin1(
                "repair summary block missing after the validate run"));
            ok = false;
        }
        if (!sawValidateLine) {
            problems->append(QString::fromLatin1(
                "repair summary block lost the Validate stage line"));
            ok = false;
        }
        if (!sawHeadline) {
            problems->append(QString::fromLatin1(
                "repair summary headline lost the no-repair-needed tail"));
            ok = false;
        }
    }

    // Cycle 11 fix: a Full Repair summary must map the plan-stage keys to the
    // transcript's tool keys, so the four package stages render their real
    // outcomes instead of "not reported".
    {
        const QString fullRepairTranscript = QString::fromLatin1(
            "=== Full Repair ===\n"
            "Repair change status dpkg: changed\n"
            "Repair change status fixbroken: changed\n"
            "Repair change status aptupdate: unchanged|APT package lists are byte-identical and no repository index was fetched\n"
            "Repair change status upgrade: changed\n"
            "Repair change status initramfs: unchanged|already up to date\n"
            "Repair change status grub: changed\n");
        const ParsedTranscript fullParsed = parseTranscript(
            toStd(fullRepairTranscript));
        const QStringList savedStages = m_activeRepairStages;
        const QString savedPendingLabel = m_pendingLabel;
        const std::size_t savedEntries = m_logEntries.size();
        m_activeRepairStages = QStringList::split(QString::fromLatin1(" "),
            QString::fromLatin1("dpkg-configure fix-broken apt-update apt-upgrade initramfs grub"),
            false);
        m_pendingLabel = tr("Full Repair");
        appendRepairSummaryBlock(fullParsed, true);
        QString blockText;
        for (std::size_t e = savedEntries; e < m_logEntries.size(); ++e) {
            blockText += m_logEntries[e].text;
            blockText += QString::fromLatin1("\n");
        }
        const char *const expectedLines[] = {
            "[OK] Complete interrupted package configuration - changed",
            "[OK] Repair broken package dependencies - changed",
            "[-] Refresh package metadata - APT package lists are byte-identical and no repository index was fetched",
            "[OK] Upgrade installed packages - changed",
            "[-] Initramfs - already up to date",
            "[OK] GRUB configuration - changed"
        };
        for (std::size_t e = 0; e < sizeof(expectedLines) / sizeof(expectedLines[0]); ++e) {
            if (blockText.find(QString::fromLatin1(expectedLines[e])) < 0) {
                problems->append(QString::fromLatin1(
                    "Full Repair summary lost the %1 stage outcome")
                    .arg(QString::fromLatin1(expectedLines[e])));
                ok = false;
            }
        }
        if (blockText.find(tr("not reported")) >= 0) {
            problems->append(QString::fromLatin1(
                "Full Repair summary rendered 'not reported' for a real stage"));
            ok = false;
        }
        m_activeRepairStages = savedStages;
        m_pendingLabel = savedPendingLabel;
    }

    // Host-repair summary fix: a failed plan attributes the failure to the
    // stage the helper names (`ERROR: stage 'grub' failed:`) and keeps every
    // completed stage's own result (unchanged = no repair needed, changed =
    // successful) even though the overall command failed.
    {
        const QString failedTranscript = QString::fromLatin1(
            "Repair change status dpkg: unchanged|dpkg reported no packages pending configuration\n"
            "Repair change status fixbroken: unchanged|simulated fix-broken transaction proposed no package changes\n"
            "Repair change status aptupdate: changed\n"
            "Repair change status upgrade: unchanged|simulated upgrade transaction proposed no package changes\n"
            "Repair change status initramfs: changed\n"
            "[00:34:51] ERROR: stage 'grub' failed: GRUB legacy regeneration was rolled back because it removed an existing boot entry.\n");
        const ParsedTranscript failedParsed = parseTranscript(
            toStd(failedTranscript));
        const QStringList savedStages = m_activeRepairStages;
        const QString savedPendingLabel = m_pendingLabel;
        const QString savedTranscript = m_transcript;
        const std::size_t savedEntries = m_logEntries.size();
        m_activeRepairStages = QStringList::split(QString::fromLatin1(" "),
            QString::fromLatin1("dpkg-configure fix-broken apt-update apt-upgrade initramfs grub"),
            false);
        m_pendingLabel = tr("Full Repair");
        m_transcript = failedTranscript;
        appendRepairSummaryBlock(failedParsed, false);
        QString failedBlockText;
        for (std::size_t e = savedEntries; e < m_logEntries.size(); ++e) {
            failedBlockText += m_logEntries[e].text;
            failedBlockText += QString::fromLatin1("\n");
        }
        const char *const failedExpectedLines[] = {
            "[OK] 2 successful | [FAIL] 1 failed | [-] 3 no repair needed",
            "[-] Complete interrupted package configuration - dpkg reported no packages pending configuration",
            "[-] Repair broken package dependencies - simulated fix-broken transaction proposed no package changes",
            "[OK] Refresh package metadata - changed",
            "[-] Upgrade installed packages - simulated upgrade transaction proposed no package changes",
            "[OK] Initramfs - changed",
            "[FAIL] GRUB configuration - GRUB legacy regeneration was rolled back because it removed an existing boot entry."
        };
        for (std::size_t e = 0;
             e < sizeof(failedExpectedLines) / sizeof(failedExpectedLines[0]); ++e) {
            if (failedBlockText.find(QString::fromLatin1(failedExpectedLines[e])) < 0) {
                problems->append(QString::fromLatin1(
                    "failed Full Repair summary lost the expected outcome: %1")
                    .arg(QString::fromLatin1(failedExpectedLines[e])));
                ok = false;
            }
        }
        if (failedBlockText.find(tr("not reported")) >= 0) {
            problems->append(QString::fromLatin1(
                "failed Full Repair summary rendered 'not reported' for a "
                "stage whose status was reported"));
            ok = false;
        }
        m_activeRepairStages = savedStages;
        m_pendingLabel = savedPendingLabel;
        m_transcript = savedTranscript;
    }

    // Session list: the live session must be listed and selectable.
    ensureLogFile();
    refreshSessionLogList();
    if (!m_sessionLogList || m_sessionLogList->childCount() < 1
        || !m_sessionLogList->firstChild()
        || m_sessionLogList->firstChild()->text(0) != QString::fromLatin1("Current session")) {
        problems->append(QString::fromLatin1(
            "session log list does not lead with the live session (count=%1 first='%2')")
            .arg(m_sessionLogList ? m_sessionLogList->childCount() : -1)
            .arg(m_sessionLogList && m_sessionLogList->firstChild()
                     ? m_sessionLogList->firstChild()->text(0)
                     : QString::fromLatin1("<none>")));
        ok = false;
    }
    return ok;
}

bool LegacyMainWindow::verifyLayout(QString *problems, int *checked,
                                    QString *geometryOut)
{
    bool ok = true;
    int checkedCount = 0;
    if (width() < 1024 || height() < 768) {
        problems->append(QString::fromLatin1(
            "window is %1x%2, smaller than the 1024x768 layout contract")
            .arg(width()).arg(height()));
        ok = false;
    }

    const int originalPage = m_tabs->currentPageIndex();
    for (int page = 0; page < m_tabs->count(); ++page) {
        m_tabs->setCurrentPage(page);
        qApp->processEvents();
        QWidget *pageWidget = m_tabs->page(page);

        for (std::size_t i = 0; i < m_groupBoxes.size(); ++i) {
            QGroupBox *box = m_groupBoxes[i];
            if (!box->isVisibleTo(m_tabs)) {
                continue;
            }
            ++checkedCount;
            QFontMetrics metrics(box->font());
            const int needed = metrics.width(box->title())
                + 2 * box->frameWidth() + kGroupTitlePadding;
            if (box->width() < needed) {
                problems->append(QString::fromLatin1(
                    "group title clipped in '%1' (%2px needed, %3px available)")
                    .arg(box->title()).arg(needed).arg(box->width()));
                ok = false;
            }
            QWidget *bound = layoutBoundFor(box, pageWidget);
            const QRect boundRect = bound->rect();
            const QRect mapped(box->mapTo(bound, QPoint(0, 0)), box->size());
            if (!boundRect.contains(mapped)) {
                problems->append(QString::fromLatin1(
                    "group box '%1' is clipped by the page (at %2,%3 %4x%5, bound %6x%7)")
                    .arg(box->title())
                    .arg(mapped.x()).arg(mapped.y())
                    .arg(mapped.width()).arg(mapped.height())
                    .arg(boundRect.width()).arg(boundRect.height()));
                ok = false;
            }
            // Regression guard for the Qt3 KDE/Etch group-title clipping: a
            // non-empty QGroupBox title is drawn on the upper frame line with
            // its top half above the frame, so any future titled frame needs
            // at least half a line of clearance inside its bound. The legacy
            // frames use a sectionTitle label instead and have empty titles.
            if (!box->title().isEmpty()) {
                const int clearance = metrics.height() / 2 + 2;
                if (mapped.y() < clearance) {
                    problems->append(QString::fromLatin1(
                        "group title '%1' has no top clearance (%2px, needs %3px)")
                        .arg(box->title()).arg(mapped.y()).arg(clearance));
                    ok = false;
                }
            }
        }

        for (std::size_t i = 0; i < m_buttons.size(); ++i) {
            QPushButton *button = m_buttons[i];
            if (!button->isVisibleTo(m_tabs)) {
                continue;
            }
            ++checkedCount;
            QFontMetrics metrics(button->font());
            const int needed = metrics.width(button->text()) + 20;
            if (button->width() < needed) {
                problems->append(QString::fromLatin1(
                    "button text clipped in '%1' (%2px needed, %3px available)")
                    .arg(button->text()).arg(needed).arg(button->width()));
                ok = false;
            }
            QWidget *bound = layoutBoundFor(button, pageWidget);
            const QRect boundRect = bound->rect();
            const QRect mapped(button->mapTo(bound, QPoint(0, 0)), button->size());
            if (!boundRect.contains(mapped)) {
                problems->append(QString::fromLatin1(
                    "button '%1' is clipped by the page (at %2,%3 %4x%5, bound %6x%7)")
                    .arg(button->text())
                    .arg(mapped.x()).arg(mapped.y())
                    .arg(mapped.width()).arg(mapped.height())
                    .arg(boundRect.width()).arg(boundRect.height()));
                ok = false;
            }
        }

        if (m_deviceList->isVisibleTo(m_tabs)) {
            ++checkedCount;
            if (!listColumnsFit(m_deviceList, QString::fromLatin1("device list"), problems)) {
                ok = false;
            }
            if (!listLastColumnFills(m_deviceList, QString::fromLatin1("device list"), problems)) {
                ok = false;
            }
        }
        if (m_detailList->isVisibleTo(m_tabs)) {
            ++checkedCount;
            if (!listColumnsFit(m_detailList, QString::fromLatin1("details list"), problems)) {
                ok = false;
            }
            if (!listLastColumnFills(m_detailList, QString::fromLatin1("details list"), problems)) {
                ok = false;
            }
        }
        if (m_diagnosticList && m_diagnosticList->isVisibleTo(m_tabs)) {
            ++checkedCount;
            if (!listColumnsFit(m_diagnosticList, QString::fromLatin1("diagnostic list"), problems)) {
                ok = false;
            }
            if (!listLastColumnFills(m_diagnosticList, QString::fromLatin1("diagnostic list"), problems)) {
                ok = false;
            }
        }
        if (m_toolList && m_toolList->isVisibleTo(m_tabs)) {
            ++checkedCount;
            if (!listColumnsFit(m_toolList, QString::fromLatin1("tools list"), problems)) {
                ok = false;
            }
            if (!listLastColumnFills(m_toolList, QString::fromLatin1("tools list"), problems)) {
                ok = false;
            }
        }
        if (m_sessionLogList->isVisibleTo(m_tabs)) {
            ++checkedCount;
            if (!listColumnsFit(m_sessionLogList, QString::fromLatin1("session log list"), problems)) {
                ok = false;
            }
            if (!listLastColumnFills(m_sessionLogList, QString::fromLatin1("session log list"), problems)) {
                ok = false;
            }
        }
        if (m_logSearchEdit && m_logSearchEdit->isVisibleTo(m_tabs)) {
            ++checkedCount;
            QFontMetrics metrics(m_logSearchEdit->font());
            if (m_logSearchEdit->width() < metrics.width(m_logSearchEdit->text()) + 24) {
                problems->append(QString::fromLatin1("log search field is too narrow"));
                ok = false;
            }
        }
        if (m_configCombo && m_configCombo->isVisibleTo(m_tabs)) {
            ++checkedCount;
            if (!comboTextFits(m_configCombo, QString::fromLatin1("configuration combo"), problems)) {
                ok = false;
            }
        }
        if (m_capabilityTable && m_capabilityTable->isVisibleTo(m_tabs)) {
            ++checkedCount;
            QWidget *bound = layoutBoundFor(m_capabilityTable, pageWidget);
            const QRect boundRect = bound->rect();
            const QRect mapped(m_capabilityTable->mapTo(bound, QPoint(0, 0)),
                               m_capabilityTable->size());
            if (!boundRect.contains(mapped)) {
                problems->append(QString::fromLatin1(
                    "capability table is clipped by the page (bound %1x%2)")
                    .arg(boundRect.width()).arg(boundRect.height()));
                ok = false;
            }
            if (m_capabilityTable->width() < 420) {
                problems->append(QString::fromLatin1(
                    "capability table is too narrow (%1px)").arg(m_capabilityTable->width()));
                ok = false;
            }
        }
        if (m_repairVerticalSplitter && m_repairVerticalSplitter->isVisibleTo(m_tabs)) {
            ++checkedCount;
            QWidget *bound = layoutBoundFor(m_repairVerticalSplitter, pageWidget);
            const QRect boundRect = bound->rect();
            const QRect mapped(m_repairVerticalSplitter->mapTo(bound, QPoint(0, 0)),
                               m_repairVerticalSplitter->size());
            if (!boundRect.contains(mapped)) {
                problems->append(QString::fromLatin1(
                    "Repair vertical splitter is clipped by the page"));
                ok = false;
            }
        }

        // Section titles on THIS page (modern sectionTitle parity): every
        // tab's titles are checked, not only the active one, so a clip on an
        // unvisited page cannot pass the smoke. Wrapping titles
        // (Qt::WordBreak) are exempt from the single-line width check: they
        // break onto a second line instead of clipping, and the containment
        // check below still applies.
        for (std::size_t i = 0; i < m_sectionTitles.size(); ++i) {
            QLabel *title = m_sectionTitles[i];
            if (!title) {
                continue;
            }
            QWidget *titlePage = title->parentWidget();
            while (titlePage && titlePage != m_tabs
                   && m_tabs->indexOf(titlePage) < 0) {
                titlePage = titlePage->parentWidget();
            }
            if (titlePage != pageWidget) {
                continue;
            }
            ++checkedCount;
            QFontMetrics metrics(title->font());
            const bool wraps = (title->alignment() & Qt::WordBreak) != 0;
            if (!wraps && title->width() < metrics.width(title->text()) + 8) {
                problems->append(QString::fromLatin1(
                    "section title clipped: '%1' (%2px needed, %3px available)")
                    .arg(title->text()).arg(metrics.width(title->text()) + 8)
                    .arg(title->width()));
                ok = false;
            }
            // Left-edge probe: the title's text must start at its container's
            // content-left (within the frame/layout margin), never centered
            // inside the row (the Repair-page centering regression). Qt3's
            // QWidget has no contentsRect(); the container is a QFrame
            // (QGroupBox) or a plain page.
            QWidget *container = title->parentWidget();
            if (container) {
                int contentLeft = 0;
                if (container->inherits("QFrame")) {
                    contentLeft = static_cast<QFrame *>(container)
                                      ->contentsRect().left();
                }
                const QPoint origin = title->mapTo(container, QPoint(0, 0));
                if (origin.x() < contentLeft - 2
                    || origin.x() > contentLeft + kSectionTitleLeftTolerance) {
                    problems->append(QString::fromLatin1(
                        "section title '%1' is not left-aligned with its content "
                        "(x=%2, content-left=%3)")
                        .arg(title->text()).arg(origin.x()).arg(contentLeft));
                    ok = false;
                }
            }
            QWidget *bound = layoutBoundFor(title, pageWidget);
            const QRect boundRect = bound->rect();
            const QRect mapped(title->mapTo(bound, QPoint(0, 0)), title->size());
            if (!boundRect.contains(mapped)) {
                problems->append(QString::fromLatin1(
                    "section title '%1' is clipped by its page").arg(title->text()));
                ok = false;
            }
        }
    }
    m_tabs->setCurrentPage(originalPage);
    // Round 67 (File Copy top-alignment layout contract): section 2
    // ("Choose destination in repaired system") and section 3 ("Ownership
    // and copy policy") must sit tight at the top of the lower pane with
    // their content rows tight under their titles. Qt3's qGeomCalc split
    // the pane's surplus equally between the two group frames (no stretch
    // factor -> the "distributed according to sizePolicy" 50/50 path), so
    // each box grew ~half the surplus, its title pinned at the top and its
    // content row centered in the leftover cell — a large gap between the
    // title and its box plus a pushed-down section 3. The trailing
    // lowerPaneLayout stretch pins both boxes at their sizeHint. Measured
    // on the REAL laid-out page (switch to the tab + activate the ancestor
    // layouts — the hidden-tab trap), never from sizeHint differences.
    if (m_fileCopyDestinationGroup && m_fileCopyOptionsGroup
        && m_fileCopyDestinationTitle && m_fileCopyOptionsTitle
        && m_fileCopyDestinationEdit && m_fileCopyOwnershipCombo) {
        m_tabs->setCurrentPage(4);
        qApp->sendPostedEvents();
        qApp->processEvents();
        for (QWidget *ancestor = m_fileCopyDestinationGroup; ancestor;
             ancestor = ancestor->parentWidget()) {
            if (ancestor->layout()) {
                ancestor->layout()->activate();
            }
        }
        for (QWidget *ancestor = m_fileCopyOptionsGroup; ancestor;
             ancestor = ancestor->parentWidget()) {
            if (ancestor->layout()) {
                ancestor->layout()->activate();
            }
        }
        qApp->sendPostedEvents();
        qApp->processEvents();
        QWidget *lowerPane = m_fileCopyDestinationGroup->parentWidget();
        const int destTopInPane = lowerPane
            ? m_fileCopyDestinationGroup->mapTo(lowerPane, QPoint(0, 0)).y()
            : -1;
        const int destBoxHeight = m_fileCopyDestinationGroup->height();
        const int destTitleTop = m_fileCopyDestinationTitle
            ->mapTo(m_fileCopyDestinationGroup, QPoint(0, 0)).y();
        const int destTitleBottom = destTitleTop
            + m_fileCopyDestinationTitle->height();
        const int destEditTop = m_fileCopyDestinationEdit
            ->mapTo(m_fileCopyDestinationGroup, QPoint(0, 0)).y();
        const int optionsTopInPane = lowerPane
            ? m_fileCopyOptionsGroup->mapTo(lowerPane, QPoint(0, 0)).y()
            : -1;
        const int optionsBoxHeight = m_fileCopyOptionsGroup->height();
        const int optionsTitleTop = m_fileCopyOptionsTitle
            ->mapTo(m_fileCopyOptionsGroup, QPoint(0, 0)).y();
        const int optionsTitleBottom = optionsTitleTop
            + m_fileCopyOptionsTitle->height();
        const int ownershipTop = m_fileCopyOwnershipCombo
            ->mapTo(m_fileCopyOptionsGroup, QPoint(0, 0)).y();
        const int destTitleGap = destTitleTop;
        const int destContentGap = destEditTop - destTitleBottom;
        const int optionsTitleGap = optionsTitleTop;
        const int optionsContentGap = ownershipTop - optionsTitleBottom;
        const int sectionGap = optionsTopInPane
            - (destTopInPane + destBoxHeight);
        if (geometryOut) {
            *geometryOut += QString::fromLatin1(
                "; fileCopy destBoxTop=%1 destBoxH=%2 destTitleGap=%3 "
                "destContentGap=%4 section2to3Gap=%5 optionsTitleGap=%6 "
                "optionsContentGap=%7 optionsBoxH=%8")
                .arg(destTopInPane).arg(destBoxHeight)
                .arg(destTitleGap).arg(destContentGap).arg(sectionGap)
                .arg(optionsTitleGap).arg(optionsContentGap)
                .arg(optionsBoxHeight);
        }
        if (destTopInPane != 0) {
            problems->append(QString::fromLatin1(
                "File Copy section 2 must sit at the top of the lower pane "
                "(top=%1)").arg(destTopInPane));
            ok = false;
        }
        if (destTitleGap >= 16) {
            problems->append(QString::fromLatin1(
                "File Copy section 2 title floats below its box top (gap=%1)")
                .arg(destTitleGap));
            ok = false;
        }
        if (destContentGap >= 16) {
            problems->append(QString::fromLatin1(
                "File Copy section 2 content floats below its title (gap=%1)")
                .arg(destContentGap));
            ok = false;
        }
        if (sectionGap > 12) {
            problems->append(QString::fromLatin1(
                "File Copy section 3 is not tight under section 2 (gap=%1)")
                .arg(sectionGap));
            ok = false;
        }
        if (optionsTitleGap >= 16) {
            problems->append(QString::fromLatin1(
                "File Copy section 3 title floats below its box top (gap=%1)")
                .arg(optionsTitleGap));
            ok = false;
        }
        if (optionsContentGap >= 16) {
            problems->append(QString::fromLatin1(
                "File Copy section 3 content floats below its title (gap=%1)")
                .arg(optionsContentGap));
            ok = false;
        }
        if (destBoxHeight >= 110 || optionsBoxHeight >= 110) {
            problems->append(QString::fromLatin1(
                "File Copy section boxes do not hug their content "
                "(section 2=%1px, section 3=%2px)")
                .arg(destBoxHeight).arg(optionsBoxHeight));
            ok = false;
        }
        m_tabs->setCurrentPage(originalPage);
        qApp->sendPostedEvents();
        qApp->processEvents();
    }
    // Global header: the title, subtitle and badge must fit the 1024x768
    // contract without clipping.
    if (m_headerTitle && m_headerTitle->isVisibleTo(this)) {
        ++checkedCount;
        QFontMetrics metrics(m_headerTitle->font());
        if (m_headerTitle->width() < metrics.width(m_headerTitle->text()) + 4) {
            problems->append(QString::fromLatin1("global header title is clipped"));
            ok = false;
        }
    }
    if (m_headerBadge && m_headerBadge->isVisibleTo(this)) {
        ++checkedCount;
        QFontMetrics metrics(m_headerBadge->font());
        if (m_headerBadge->width() < metrics.width(m_headerBadge->text()) + 12) {
            problems->append(QString::fromLatin1(
                "guarded-repair badge is clipped (%1px needed, %2px available)")
                .arg(metrics.width(m_headerBadge->text()) + 12)
                .arg(m_headerBadge->width()));
            ok = false;
        }
    }
    // The reserved busy-indicator slot must sit below the guarded-repair
    // badge (modern parity), flush with the badge's right edge, inside the
    // window at the 1024x768 contract size, and its widest animated frame
    // must fit the window width. The geometry is verified with the widest
    // frame shown (Qt 3.3.7's QRect::contains() rejects a null rect, so an
    // empty idle label can never be checked directly).
    if (m_busyLabel && m_busyLabel->isVisibleTo(this)) {
        ++checkedCount;
        const QString savedText = m_busyLabel->text();
        m_busyLabel->setText(QString::fromLatin1("Running all diagnostics..."));
        qApp->processEvents();
        const QRect mapped(m_busyLabel->mapTo(this, QPoint(0, 0)),
                           m_busyLabel->size());
        if (!rect().contains(mapped)) {
            problems->append(QString::fromLatin1(
                "busy indicator is clipped by the window"));
            ok = false;
        }
        if (m_headerBadge && m_headerBadge->isVisibleTo(this)) {
            const QRect badgeRect(m_headerBadge->mapTo(this, QPoint(0, 0)),
                                  m_headerBadge->size());
            if (mapped.top() < badgeRect.bottom()) {
                problems->append(QString::fromLatin1(
                    "busy indicator is not below the guarded-repair badge"));
                ok = false;
            }
            if (QABS(mapped.right() - badgeRect.right())
                > kBusyBadgeEdgeTolerance) {
                problems->append(QString::fromLatin1(
                    "busy indicator is not right-aligned with the guarded-repair badge"));
                ok = false;
            }
        }
        QFontMetrics metrics(m_busyLabel->font());
        if (metrics.width(QString::fromLatin1("Running all diagnostics...")) + 12
            > width()) {
            problems->append(QString::fromLatin1(
                "busy indicator's widest frame cannot fit the window width"));
            ok = false;
        }
        m_busyLabel->setText(savedText);
        qApp->processEvents();
    }
    if (checked) {
        *checked = checkedCount;
    }
    return ok;
}

// Rebuilds the target configuration row from the helper's read-only
// `Legacy config <key>:` probe. The row is only visible for a committed
// repair target (never Host Maintenance); files the probe reports absent are
// omitted from the combo and their exact reason is shown below it, while an
// unprobed identity lists every Etch key so the guarded read can decide.
void LegacyMainWindow::updateConfigView()
{
    const bool show = targetCommitted() && !hostMaintenanceActive();
    // Qt3 QWidget has no setVisible(bool); show()/hide() are the Qt3 API.
    if (m_configLabel) {
        if (show) { m_configLabel->show(); } else { m_configLabel->hide(); }
    }
    if (m_configCombo) {
        if (show) { m_configCombo->show(); } else { m_configCombo->hide(); }
    }
    if (m_configButton) {
        if (show) { m_configButton->show(); } else { m_configButton->hide(); }
    }
    if (m_configReasonLabel) {
        if (show) { m_configReasonLabel->show(); } else { m_configReasonLabel->hide(); }
    }
    if (!show || !m_configCombo) {
        return;
    }

    const QString previousPath = m_configCombo->currentText().stripWhiteSpace();
    const bool probed = m_model.hasDiagnostics(toStd(identity()))
        && !m_model.diagnosticsStale();
    QStringList absent;
    m_configCombo->clear();
    for (int i = 0; i < configSpecCount; ++i) {
        const ConfigSpec &spec = configSpecs[i];
        const std::string state = probed
            ? m_model.configFileState(spec.key) : std::string();
        if (state == "available") {
            m_configCombo->insertItem(QString::fromLatin1(spec.path));
        } else if (state.size() >= 12 && state.compare(0, 12, "unavailable|") == 0) {
            absent.append(QString::fromLatin1("%1 - %2")
                              .arg(QString::fromLatin1(spec.path))
                              .arg(fromStd(state.substr(12))));
        } else {
            // Not probed yet (or an unrecognised state): list the file and let
            // the helper's guarded read decide (fail closed at the helper).
            m_configCombo->insertItem(QString::fromLatin1(spec.path));
        }
    }
    if (!previousPath.isEmpty()) {
        for (int i = 0; i < m_configCombo->count(); ++i) {
            if (m_configCombo->text(i) == previousPath) {
                m_configCombo->setCurrentItem(i);
                break;
            }
        }
    }
    if (m_configReasonLabel) {
        if (m_configCombo->count() == 0) {
            m_configReasonLabel->setText(
                tr(
                    "The read-only probe found no editable target configuration "
                    "file in this target. %1")
                    .arg(absent.isEmpty() ? QString::fromLatin1("")
                                          : absent.join(QString::fromLatin1("; "))));
        } else if (!absent.isEmpty()) {
            m_configReasonLabel->setText(
                tr("Not present in the selected target (omitted): %1")
                    .arg(absent.join(QString::fromLatin1("; "))));
        } else if (!probed) {
            m_configReasonLabel->setText(tr(
                "Run diagnostics to probe which target configuration files "
                "exist; the helper's read-only probe decides the list."));
        } else {
            m_configReasonLabel->setText(tr(
                "Probed read-only by the helper; a saved edit invalidates the "
                "cached diagnostics."));
        }
    }
}

// Fill the state/reason pair for a legacy feature row. Fail closed: when no
// diagnostics for the current identity are cached, or the helper emitted no
// `Legacy feature <feature>:` line (or an unrecognised state), the state reads
// "not reported"/"unrecognised" and the reason is the model's fail-closed
// explanation.
void LegacyMainWindow::legacyFeatureDisplay(const char *feature, QString *state,
                                            QString *reason) const
{
    const QString id = identity();
    std::string modelReason;
    const bool available =
        m_model.legacyFeatureAvailable(feature, toStd(id), &modelReason);
    const bool haveDiagnostics =
        m_model.hasDiagnostics(toStd(id)) && !m_model.diagnosticsStale();
    const std::string raw =
        haveDiagnostics ? m_model.legacyFeatureState(feature) : std::string();
    QString stateText;
    if (available) {
        stateText = QString::fromLatin1("available");
    } else if (raw.empty()) {
        stateText = QString::fromLatin1("not reported");
    } else if (raw.size() >= 12 && raw.compare(0, 12, "unavailable|") == 0) {
        stateText = QString::fromLatin1("unavailable");
    } else {
        stateText = QString::fromLatin1("unrecognised");
    }
    if (state) {
        *state = stateText;
    }
    if (reason) {
        *reason = fromStd(modelReason);
    }
}

void LegacyMainWindow::updateFeatureTab(QLabel *label, const char *feature)
{
    if (!label) {
        return;
    }
    QString state;
    QString reason;
    legacyFeatureDisplay(feature, &state, &reason);
    QString text;
    if (state == QString::fromLatin1("available")) {
        text = QString::fromLatin1(
                   "Legacy feature %1: available. The helper command exists, but "
                   "this legacy frontend keeps the workflow greyed and never "
                   "weakens the helper's runtime preflights.")
                   .arg(QString::fromLatin1(feature));
    } else {
        text = QString::fromLatin1("Legacy feature %1: %2\n%3")
                   .arg(QString::fromLatin1(feature))
                   .arg(state == QString::fromLatin1("not reported")
                            ? QString::fromLatin1("no probe line cached")
                            : state)
                   .arg(reason);
    }
    label->setText(text);
}

// Modern updateChrootShellMode() parity: the heading, tab label, notice,
// command button and warning follow the active scope (Host Shell while Host
// Maintenance is active, Chroot Shell for a committed offline target).
void LegacyMainWindow::updateChrootShellMode()
{
    const bool host = hostScope();
    // Item #4: the shell transcript + command history are per-scope. When the
    // scope flips, park the outgoing scope's transcript and restore the
    // incoming scope's so the Host Shell and Chroot Shell never share output.
    // Session-only (modern parity): the buffers are in-memory, not persisted.
    if (host != m_shellScopeHost && m_shellOutput) {
        QString &outgoing = m_shellScopeHost ? m_shellTranscriptHost
                                             : m_shellTranscriptTarget;
        outgoing = m_shellOutput->text();
        m_shellScopeHost = host;
        m_shellOutput->setText(host ? m_shellTranscriptHost
                                    : m_shellTranscriptTarget);
    }
    if (m_chrootHeading) {
        m_chrootHeading->setText(host
            ? tr("Host shell")
            : tr("Chroot shell"));
    }
    if (m_chrootScopeLabel) {
        m_chrootScopeLabel->setText(host
            ? tr("Host maintenance: %1")
                  .arg(selectedDisk().isEmpty() ? tr("unresolved")
                                                : selectedDisk())
            : (targetCommitted()
                ? tr("Target: %1 + %2")
                      .arg(m_committedDisk).arg(m_committedRoot)
                : tr("Target: none selected")));
    }
    if (m_shellRunButton) {
        updateButtonText(m_shellRunButton, host
            ? tr("Run on Host")
            : tr("Run Command"));
    }
    if (m_chrootCommandHeading) {
        m_chrootCommandHeading->setText(host
            ? tr("Host command")
            : tr("Command"));
    }
    if (m_tabs && m_chrootTab) {
        m_tabs->setTabLabel(m_chrootTab,
                            host ? tr("Host Shell")
                                 : tr("Chroot Shell"));
        m_tabs->setTabToolTip(m_chrootTab, host
            ? tr("Execute a command on the running host as root.")
            : tr("Execute a command inside the selected repair system as root."));
    }
}

// Round 27 (Debian Legacy #8): stick-to-bottom append for the shell
// transcript.  Qt 3.3.7's QTextEdit::append moves the cursor but does NOT
// scroll the view, so the newest output stayed invisible unless the user
// scrolled manually.  This appends and then re-positions to the newest line
// UNLESS the user had scrolled up (the vertical scrollbar is no longer at its
// maximum): new output is always visible by default while a manual scroll-up
// for reading is never snapped back.
void LegacyMainWindow::appendShellOutput(const QString &text)
{
    if (!m_shellOutput) {
        return;
    }
    QScrollBar *bar = m_shellOutput->verticalScrollBar();
    // "Newest line visible" check: the bottom of the visible page reaches the
    // scroll maximum (one line-step of tolerance).  A naive
    // `maxValue() - value() <= lineStep()` misfires on short documents where
    // maxValue() - lineStep() goes negative (every position reads as "at the
    // bottom") and would snap a real scroll-up back down; the pageStep form
    // is true exactly when the newest line is already on screen, including
    // the degenerate case of a document that fits entirely (scrollToBottom
    // is then a no-op).
    const bool atBottom = !bar
        || bar->value() + bar->pageStep()
            >= bar->maxValue() - bar->lineStep();
    m_shellOutput->append(text);
    if (atBottom) {
        m_shellOutput->scrollToBottom();
    }
}

// Round 70 (E1): the repair result dialog pane twin of appendShellOutput().
// Streaming transcript lines stick to the bottom unless the user scrolled up
// (the same pageStep atBottom check); the prompt-arrival and post-send
// updates force the bottom so the prompt line and the ">> answer" echo are
// always visible (the user's rule: the scroll hits the bottom before, on and
// after every prompt, and after the input is sent).
void LegacyMainWindow::appendResultOutput(const QString &text, bool forceBottom)
{
    if (!m_resultView) {
        return;
    }
    QScrollBar *bar = m_resultView->verticalScrollBar();
    const bool atBottom = forceBottom || !bar
        || bar->value() + bar->pageStep()
            >= bar->maxValue() - bar->lineStep();
    m_resultView->append(text);
    if (atBottom) {
        m_resultView->scrollToBottom();
    }
}

// Chroot Shell follows the active scope's own helper probe (`host-shell` for
// Host Maintenance, `shell` for a committed repair target) plus the cached
// administrator session. When any prerequisite is missing every control stays
// greyed with the exact reason; the helper keeps its runtime preflights.
void LegacyMainWindow::updateChrootShellState()
{
    if (!m_chrootGroup || !m_chrootReasonLabel) {
        return;
    }
    updateChrootShellMode();
    const bool host = hostScope();
    const char *feature = host ? "host-shell" : "shell";
    QString state;
    QString probeReason;
    legacyFeatureDisplay(feature, &state, &probeReason);

    QString reason;
    bool enabled = false;
    if (m_running) {
        reason = tr("A helper command is already running.");
    } else if (!diagnosticsScopeReady()) {
        reason = scopeReadyReason();
    } else if (!administratorSessionActive()) {
        reason = tr(
            "Administrator authorization is required; press Authorize on the "
            "Systems or Repair tab (or re-enter Host Maintenance / re-commit the "
            "repair target) to authorize this session.");
    } else {
        std::string featureReason;
        if (!m_model.legacyFeatureAvailable(feature, toStd(identity()),
                                            &featureReason)) {
            reason = fromStd(featureReason);
        } else {
            enabled = true;
            reason = host
                ? tr(
                      "Run one reviewed command as root on the running host "
                      "through the helper's guarded host-shell verb.")
                : tr(
                      "Run one reviewed command as root inside the target "
                      "chroot through the helper's guarded shell verb.");
        }
    }

    // Round 21 (modern parity): the label under the heading shows ONLY the
    // availability reason while the shell is greyed out — the exact gate (a
    // running command, no target committed, no administrator session, or the
    // helper's read-only probe refusal) — and hides when the shell is ready,
    // so the freed row moves the command frame up.  The reason stays in the
    // Run button tooltip too.
    if (enabled) {
        m_chrootReasonLabel->hide();
    } else {
        m_chrootReasonLabel->setText(reason);
        m_chrootReasonLabel->show();
    }

    if (m_shellCommandEdit) {
        m_shellCommandEdit->setEnabled(enabled);
    }
    if (m_shellRunButton) {
        m_shellRunButton->setEnabled(enabled);
        QToolTip::add(m_shellRunButton, reason);
    }
    if (m_shellClearButton) {
        m_shellClearButton->setEnabled(enabled
            || (m_shellOutput && !m_shellOutput->text().isEmpty()));
    }
    // Round 25 (Debian Legacy #4): the transcript stays ACTIVE and
    // SCROLLABLE while a command runs — a disabled QTextEdit swallows the
    // wheel/scrollbar input, so the user could never scroll back to read a
    // question the running command printed.  Only the command INPUT is
    // disabled during a run (the gating above already handles the command
    // field and the Run button); the pane is read-only, so enabling it
    // changes nothing else.  Appending still auto-scrolls to the latest
    // output (QTextEdit::append moves the cursor to the end) and manual
    // scrolling works at any time, mid-run included.
    if (m_shellOutput) {
        m_shellOutput->setEnabled(true);
    }
}

void LegacyMainWindow::updateLegacyFeatureView()
{
    updateChrootShellState();
    updateFileCopyTab();
}

// The File Copy direction key the helper's copy/copy-preview verbs expect.
QString LegacyMainWindow::fileCopyDirection() const
{
    if (m_fileCopyDirectionCombo && m_fileCopyDirectionCombo->currentItem() == 1) {
        return QString::fromLatin1("repair-to-host");
    }
    return QString::fromLatin1("host-to-repair");
}

// File Copy gating: the cached `Legacy feature file-copy:` probe, a ready
// scope, the administrator session and no running command must all agree.
bool LegacyMainWindow::fileCopyReady(QString *reason) const
{
    if (m_running) {
        if (reason) {
            *reason = tr("A helper command is already running.");
        }
        return false;
    }
    std::string featureReason;
    if (!m_model.legacyFeatureAvailable("file-copy", toStd(identity()),
                                        &featureReason)) {
        if (reason) {
            *reason = fromStd(featureReason);
        }
        return false;
    }
    if (!diagnosticsScopeReady()) {
        if (reason) {
            *reason = scopeReadyReason();
        }
        return false;
    }
    if (!administratorSessionActive()) {
        if (reason) {
            *reason = tr(
                "No administrator session is active; press Authorize or "
                "re-enter Host Maintenance / commit a repair target.");
        }
        return false;
    }
    return true;
}

void LegacyMainWindow::updateFileCopyTab()
{
    if (!m_fileCopySourceGroup) {
        return;
    }
    QString reason;
    const bool ready = fileCopyReady(&reason);
    const bool hasSources = !m_fileCopySources.isEmpty();
    const bool anySources = hasSources && ready;
    const bool repairToHost = m_fileCopyDirectionCombo
        && m_fileCopyDirectionCombo->currentItem() == 1;
    const bool targetCommitted = !m_committedDisk.isEmpty();

    // Direction-driven labels (modern updateFileCopyDirection). The heading
    // stays the static "File copy" title - the direction arrow lives only in
    // the Direction combo - so the page-title row can never crowd it out.
    // The group-1 title uses the modern wording and is wrap-capable (see
    // buildFileCopyTab), so a narrow pane can never clip it.
    if (m_fileCopySourceTitle) {
        m_fileCopySourceTitle->setText(repairToHost
            ? tr("1. Select source files or folders from the repaired system")
            : tr("1. Select source files or folders from this host"));
    }
    if (m_fileCopyDestinationTitle) {
        m_fileCopyDestinationTitle->setText(repairToHost
            ? tr("2. Choose destination on this host")
            : tr("2. Choose destination in repaired system"));
    }
    if (m_fileCopyAddFilesButton) {
        updateButtonText(m_fileCopyAddFilesButton, repairToHost
            ? tr("Add File Path...")
            : tr("Add Files..."));
        m_fileCopyAddFilesButton->setEnabled(ready);
    }
    if (m_fileCopyAddFolderButton) {
        updateButtonText(m_fileCopyAddFolderButton, repairToHost
            ? tr("Add Folder Path...")
            : tr("Add Folder..."));
        m_fileCopyAddFolderButton->setEnabled(ready);
    }
    if (m_fileCopyBrowseSourceButton) {
        // Repair -> Host only: browsing the repaired-system source tree needs a
        // committed repair target (the helper mounts that target read-only per
        // browse request); Host -> Repair hides it because the Add buttons
        // already open host-native dialogs.
        if (repairToHost) {
            m_fileCopyBrowseSourceButton->show();
        } else {
            m_fileCopyBrowseSourceButton->hide();
        }
        const bool sourceBrowseEnabled = ready && repairToHost && targetCommitted;
        m_fileCopyBrowseSourceButton->setEnabled(sourceBrowseEnabled);
        const QString sourceBrowseTip = !sourceBrowseEnabled
            ? (!ready
                   ? reason
                   : tr(
                         "Select the repair drive in Systems before choosing a "
                         "source folder inside it."))
            : tr(
                  "Browse the repaired system's folders read-only and stage "
                  "the chosen folder.");
        if (m_fileCopyBrowseSourceTip != sourceBrowseTip) {
            m_fileCopyBrowseSourceTip = sourceBrowseTip;
            QToolTip::add(m_fileCopyBrowseSourceButton, sourceBrowseTip);
        }
    }
    if (m_fileCopyBrowseButton) {
        updateButtonText(m_fileCopyBrowseButton, repairToHost
            ? tr("Browse...")
            : tr("Browse Target Folders..."));
        // Host to Repair browsing reads the selected repair tree through the
        // helper and needs a committed repair target (Host Maintenance alone
        // provides no repair tree); Repair to Host browses the running host
        // directly and needs no target.
        const bool browseEnabled = ready && (repairToHost || targetCommitted);
        m_fileCopyBrowseButton->setEnabled(browseEnabled);
        const QString browseTip = !browseEnabled
            ? (!ready
                   ? reason
                   : tr(
                         "Select the repair drive in Systems before choosing a "
                         "destination inside it (Host Maintenance does not "
                         "provide a repair tree to browse)."))
            : (repairToHost
                   ? tr(
                         "Choose a host destination folder directly.")
                   : tr(
                         "Browse the selected repair system through the helper's "
                         "temporary read-only mounts and choose an absolute "
                         "destination path. No target files are changed while "
                         "browsing."));
        if (m_fileCopyBrowseTip != browseTip) {
            m_fileCopyBrowseTip = browseTip;
            QToolTip::add(m_fileCopyBrowseButton, browseTip);
        }
    }

    if (m_fileCopyDirectionCombo) {
        m_fileCopyDirectionCombo->setEnabled(ready);
    }
    if (m_fileCopyOwnershipCombo) {
        m_fileCopyOwnershipCombo->setEnabled(ready);
    }
    if (m_fileCopyDestinationEdit) {
        m_fileCopyDestinationEdit->setEnabled(ready);
        if (ready && m_fileCopyDestinationEdit->text()
                        == tr("unavailable: see the helper's Legacy feature file-copy: probe reason above")) {
            m_fileCopyDestinationEdit->setText(QString::null);
        }
    }
    if (m_fileCopySourceList) {
        m_fileCopySourceList->setEnabled(ready);
    }
    if (m_fileCopyRemoveButton) {
        m_fileCopyRemoveButton->setEnabled(anySources);
    }
    if (m_fileCopyClearButton) {
        m_fileCopyClearButton->setEnabled(anySources);
    }
    // The primary actions share the page-title row with the scope label and
    // light up as soon as the direction and scope are ready (modern parity);
    // the run path itself re-validates the staged sources and destination.
    if (m_fileCopyPreviewButton) {
        m_fileCopyPreviewButton->setEnabled(ready);
        const QString previewTip = ready
            ? tr(
                  "Run a copy dry-run through the guarded helper. No files are changed.")
            : reason;
        if (m_fileCopyPreviewTip != previewTip) {
            m_fileCopyPreviewTip = previewTip;
            QToolTip::add(m_fileCopyPreviewButton, previewTip);
        }
    }
    if (m_fileCopyRunButton) {
        m_fileCopyRunButton->setEnabled(ready);
        const QString runTip = ready
            ? tr(
                  "Copy the staged files and folders with cp -a, restore "
                  "ownership with chown --reference and byte-compare every "
                  "regular file afterwards.")
            : reason;
        if (m_fileCopyRunTip != runTip) {
            m_fileCopyRunTip = runTip;
            QToolTip::add(m_fileCopyRunButton, runTip);
        }
    }
}

void LegacyMainWindow::fileCopyDirectionChanged()
{
    // Direction changes clear the staged sources and destination to avoid
    // mixing source/destination namespaces (modern parity).
    m_fileCopySources.clear();
    if (m_fileCopySourceList) {
        m_fileCopySourceList->clear();
    }
    if (m_fileCopyDestinationEdit) {
        m_fileCopyDestinationEdit->clear();
    }
    const bool repairToHost = m_fileCopyDirectionCombo
        && m_fileCopyDirectionCombo->currentItem() == 1;
    appendLog(tr(
        "File Copy direction changed to %1; staged paths were cleared to "
        "avoid mixing source/destination namespaces.")
        .arg(repairToHost
                 ? QString::fromUtf8("Repair \xE2\x86\x92 Host")
                 : QString::fromUtf8("Host \xE2\x86\x92 Repair")));
    updateFileCopyTab();
}

// Browse Target Folders... (modern browseFileCopyDestination): Repair -> Host
// picks a host folder through QFileDialog; Host -> Repair walks the selected
// repair tree one directory view at a time through the helper's read-only
// browse-target verb (BROWSE_ENTRY records, base64-encoded names).
void LegacyMainWindow::fileCopyBrowse()
{
    if (m_running) {
        return;
    }
    QString reason;
    if (!fileCopyReady(&reason)) {
        QMessageBox::information(this, tr("File Copy unavailable"),
                                 reason, QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    const bool repairToHost = m_fileCopyDirectionCombo
        && m_fileCopyDirectionCombo->currentItem() == 1;
    if (repairToHost) {
        const QString path = QFileDialog::getExistingDirectory(
            QString::null, this, "file-copy-host-dest",
            tr("Choose host destination folder"));
        if (path.isEmpty()) {
            return;
        }
        if (m_fileCopyDestinationEdit) {
            m_fileCopyDestinationEdit->setText(path);
        }
        appendLog(tr(
            "Staged Repair \xE2\x86\x92 Host destination: %1. No copy occurred.")
            .arg(path));
        updateFileCopyTab();
        return;
    }
    if (m_committedDisk.isEmpty() || m_committedRoot.isEmpty()) {
        QMessageBox::information(
            this, tr("Select a repair target"),
            tr(
                "Select the repair drive in Systems before choosing a "
                "destination inside it."),
            QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    // Round 57 (modern two-level workflow): level 1 is the destination-entry
    // dialog (folder entry field + Browse repair folders... + OK/Cancel); its
    // Browse action hands the entered path to the level-2
    // read-only folder picker, whose Choose This Folder returns the chosen
    // path into a fresh entry dialog for the OK confirmation.  Round 68 (I1):
    // the explicit Clear button was removed from this dialog (the entry is
    // cleared with backspace/Ctrl+A — the modern in-field atlas clear action
    // made the redundant button obsolete).
    fileCopyBrowseEntry(QString::null);
}

// Level 1 of the Host -> Repair destination workflow (modern
// repairDestinationDialog parity, adapted to the Qt3 request-driven browse):
// the destination-entry dialog with the folder entry field, the Browse
// repair folders... action and OK/Cancel.  OK validates and stages the
// entered absolute in-chroot path.  Browse repair folders... starts the
// level-2 picker at the entered path (the picker's Choose This Folder calls
// fileCopyBrowseEntry() again with the chosen path pre-filled).  The two
// levels run sequentially (the legacy GUI drives the helper asynchronously,
// so the picker cannot stack above a still-open entry dialog) — the control
// surface is the modern one.
void LegacyMainWindow::fileCopyBrowseEntry(const QString &initial)
{
    QString current = initial.isNull()
        ? (m_fileCopyDestinationEdit
           && !m_fileCopyDestinationEdit->text().stripWhiteSpace().isEmpty()
           ? m_fileCopyDestinationEdit->text().stripWhiteSpace()
           : QString::fromLatin1("/home"))
        : initial;
    while (true) {
        QDialog dialog(this, "legacy-file-copy-destination", true);
        dialog.setCaption(tr("Select repair-system destination"));
        m_fileCopyBrowseEntryDialog = &dialog;
        m_fileCopyBrowseEntryEdit = 0;
        m_fileCopyBrowseEntryBrowse = false;
        QVBoxLayout *layout = new QVBoxLayout(&dialog, 10, 8);
        QLabel *heading = new QLabel(tr(
            "Choose a destination folder inside the repaired system"), &dialog);
        enableLabelWordWrap(heading);
        heading->setMaximumWidth(520);
        layout->addWidget(heading);
        QLabel *description = new QLabel(tr(
            "Browse folders from the selected repaired system through "
            "temporary read-only mounts. Nothing is written while browsing, "
            "and the guarded copy validates the chosen path again before any "
            "write."), &dialog);
        enableLabelWordWrap(description);
        description->setMaximumWidth(520);
        layout->addWidget(description);
        QHBoxLayout *pathRow = new QHBoxLayout(layout);
        pathRow->setSpacing(6);
        m_fileCopyBrowseEntryEdit = new QLineEdit(&dialog);
        m_fileCopyBrowseEntryEdit->setText(current);
        QToolTip::add(m_fileCopyBrowseEntryEdit, tr(
            "Absolute path inside repaired system (for example /home/user/Recovered)"));
        pathRow->addWidget(m_fileCopyBrowseEntryEdit, 1);
        QPushButton *browse = makeButton(tr("Browse repair folders..."), &dialog);
        QToolTip::add(browse, tr(
            "Open a read-only folder view; the target is unmounted after each listing."));
        // Qt 3.3.7 QDialog::accept()/reject()/done() are all protected: the
        // closing slot is reached through the metaobject (the signal->slot
        // connection), while the member slot only latches the Browse intent.
        QObject::connect(browse, SIGNAL(clicked()), this,
                         SLOT(fileCopyBrowseEntryBrowse()));
        QObject::connect(browse, SIGNAL(clicked()), &dialog, SLOT(accept()));
        pathRow->addWidget(browse);
        QHBoxLayout *buttons = new QHBoxLayout(layout);
        buttons->setSpacing(6);
        buttons->addStretch();
        QPushButton *cancel = makeButton(tr("Cancel"), &dialog);
        QPushButton *ok = makeButton(tr("OK"), &dialog);
        ok->setDefault(true);
        // Round 72 (modern parity, TUXEDO button order): OK sits LEFT of
        // Cancel with the OK default (the modern level-1 entry dialog's
        // addUniformDialogButtons [OK][Cancel] convention).
        buttons->addWidget(ok);
        buttons->addWidget(cancel);
        QObject::connect(cancel, SIGNAL(clicked()), &dialog, SLOT(reject()));
        QObject::connect(ok, SIGNAL(clicked()), &dialog, SLOT(accept()));
        dialog.resize(560, 250);
        const int result = dialog.exec();
        const QString entered = m_fileCopyBrowseEntryEdit
            ? m_fileCopyBrowseEntryEdit->text().stripWhiteSpace()
            : QString::null;
        const bool browseRequested = m_fileCopyBrowseEntryBrowse;
        m_fileCopyBrowseEntryDialog = 0;
        m_fileCopyBrowseEntryEdit = 0;
        m_fileCopyBrowseEntryBrowse = false;
        if (result != QDialog::Accepted) {
            return; // Cancel.
        }
        if (browseRequested) {
            // Level 2: the read-only folder picker at the entered path (the
            // picker's Choose This Folder re-opens this entry dialog).  Round
            // 60: the picker opens IMMEDIATELY (visible, greyed while the
            // first listing is in flight) — the entry dialog hands over with
            // no blank background window in between.
            QString start = QDir::cleanDirPath(entered);
            if (!start.startsWith(QString::fromLatin1("/"))
                || start == QString::fromLatin1("/..")
                || start.startsWith(QString::fromLatin1("/../"))) {
                start = QString::fromLatin1("/home");
            }
            m_fileCopyBrowseMode = 0;
            m_fileCopyBrowsePath = start;
            openFileCopyBrowsePicker();
            return;
        }
        const QString cleaned = QDir::cleanDirPath(entered);
        if (cleaned.startsWith(QString::fromLatin1("/"))
            && cleaned != QString::fromLatin1("/")
            && cleaned != QString::fromLatin1("/..")
            && !cleaned.startsWith(QString::fromLatin1("/../"))) {
            if (m_fileCopyDestinationEdit) {
                m_fileCopyDestinationEdit->setText(cleaned);
            }
            appendLog(tr(
                "Staged Host \xE2\x86\x92 Repair destination: %1. No copy "
                "occurred.").arg(cleaned));
            updateFileCopyTab();
            return;
        }
        QMessageBox::warning(
            this, tr("Invalid repair path"),
            tr(
                "Use an absolute path inside the repaired system. "
                "Parent-directory escape paths are not accepted."),
            QMessageBox::Ok, QMessageBox::NoButton);
        current = entered;
        // Re-open the entry dialog with the entered text (modern parity: the
        // exec loop re-prompts until a valid path or Cancel).
    }
}

// The Browse repair folders... button of the level-1 entry dialog: latch the
// request and close the dialog through the member pointer (valid only while
// fileCopyBrowseEntry() runs its exec() loop).
void LegacyMainWindow::fileCopyBrowseEntryBrowse()
{
    // Only the Browse intent latch: the dialog closes through its own
    // accept() slot (the Qt3 metaobject connection); accept()/done() are
    // protected members and cannot be called from here.
    m_fileCopyBrowseEntryBrowse = true;
}

// Browse... on the repaired-system SOURCE side (Repair -> Host, modern
// browseFileCopySource parity): the same read-only browse-target picker as the
// destination browse, but the chosen folder is staged as a source instead of
// filling the destination edit. Needs a committed repair target (the helper
// mounts that target read-only per request).
void LegacyMainWindow::fileCopyBrowseSource()
{
    if (m_running) {
        return;
    }
    QString reason;
    if (!fileCopyReady(&reason)) {
        QMessageBox::information(this, tr("File Copy unavailable"),
                                 reason, QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    if (m_committedDisk.isEmpty() || m_committedRoot.isEmpty()) {
        QMessageBox::information(
            this, tr("Select a repair target"),
            tr(
                "Select the repair drive in Systems before choosing a "
                "source folder inside it."),
            QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    m_fileCopyBrowseMode = 1;
    m_fileCopyBrowsePath = QString::fromLatin1("/");
    // Round 60: the picker opens immediately and issues the first listing
    // through its own exec loop (no separate pre-flight request).
    openFileCopyBrowsePicker();
}

// Qt 3.3.7 has no QByteArray::fromBase64, so the legacy helper emits the
// browse names raw with only %, TAB, CR and LF percent-encoded
// (% -> %25, TAB -> %09, CR -> %0D, LF -> %0A); decode inline.  The chain
// reverses the helper's encode order so a name whose text literally contains
// "%0A" / "%0D" / "%09" / "%25" round-trips instead of double-decoding.
static QString legacyPercentDecode(const QString &encoded)
{
    QString out = encoded;
    out.replace(QString::fromLatin1("%0A"), QString::fromLatin1("\n"));
    out.replace(QString::fromLatin1("%0D"), QString::fromLatin1("\r"));
    out.replace(QString::fromLatin1("%09"), QString::fromLatin1("\t"));
    out.replace(QString::fromLatin1("%25"), QString::fromLatin1("%"));
    return out;
}

// Short interruptible sleep (40 ms) between the browse pump's processEvents
// passes, so the pump never busy-spins (nanosleep retries the remainder on
// EINTR — the HelperRunner boundedPollSleep shape).
static void browsePumpSleep()
{
    struct timespec delay;
    delay.tv_sec = 0;
    delay.tv_nsec = 40 * 1000 * 1000; // 40 ms
    while (::nanosleep(&delay, &delay) != 0 && errno == EINTR) {
        // retry the remainder on EINTR
    }
}

// The browse-target transcript drives the folder picker: immediate
// subdirectories arrive as BROWSE_ENTRY records (the legacy port emits the
// raw percent-encoded name). The user can step into a folder, move up, or
// choose the current folder as the destination.
static QStringList legacyBrowseEntriesFromTranscript(const std::string &transcript)
{
    QStringList entries;
    const QStringList lines = QStringList::split(QString::fromLatin1("\n"),
                                                fromStd(transcript), false);
    for (QStringList::ConstIterator it = lines.begin(); it != lines.end(); ++it) {
        if (!(*it).startsWith(QString::fromLatin1("BROWSE_ENTRY\t"))) {
            continue;
        }
        entries.append(legacyPercentDecode((*it).mid(13)));
    }
    entries.sort();
    return entries;
}

void LegacyMainWindow::handleFileCopyBrowseResult(const std::string &transcript,
                                                  bool ok)
{
    // Round 59/60 (picker persistence): the picker dialog is created ONCE by
    // openFileCopyBrowsePicker() and every browse-target response — the first
    // listing and each navigation (Up / Open Selected / Refresh) — updates
    // that SAME visible dialog in place (re-enable, refresh the list, path
    // label and Up state), never a rebuilt second dialog.  Round 60: the
    // request is pumped to completion inside the open picker
    // (legacyFileCopyBrowsePump), so this handler always runs while the
    // dialog pointer is live; a failed listing re-enables the picker with
    // the previous listing so the user can retry or cancel.
    if (m_fileCopyBrowsePickerDialog && m_fileCopyBrowsePickerAwaiting) {
        m_fileCopyBrowsePickerAwaiting = false;
        m_fileCopyBrowsePickerDialog->setEnabled(true);
        if (!ok) {
            QMessageBox::warning(
                this, tr("Browse Target Folders"),
                tr(
                    "The helper could not list the repair-system folder:\n\n%1")
                    .arg(transcript.size() > 400
                             ? QString::fromLatin1(transcript.substr(0, 400).c_str())
                             : QString::fromLatin1(transcript.c_str())),
                QMessageBox::Ok, QMessageBox::NoButton);
            return;
        }
        if (m_fileCopyBrowsePickerList) {
            m_fileCopyBrowsePickerList->clear();
            const QStringList entries =
                legacyBrowseEntriesFromTranscript(transcript);
            for (QStringList::ConstIterator it = entries.begin();
                 it != entries.end(); ++it) {
                new QListViewItem(m_fileCopyBrowsePickerList, *it);
            }
        }
        if (m_fileCopyBrowsePickerHint) {
            m_fileCopyBrowsePickerHint->setText(
                tr("Current repaired-system folder: %1")
                    .arg(m_fileCopyBrowsePath));
        }
        if (m_fileCopyBrowsePickerUp) {
            m_fileCopyBrowsePickerUp->setEnabled(
                m_fileCopyBrowsePath != QString::fromLatin1("/"));
        }
        return;
    }
    // The late completion of a browse request the pump timed out and
    // cancelled: the failure was already reported and the picker re-enabled;
    // drop it quietly (and never apply a stale response to a newer request).
    if (m_fileCopyBrowsePickerDialog && m_fileCopyBrowsePickerLate) {
        m_fileCopyBrowsePickerLate = false;
        return;
    }
    if (!ok) {
        // A listing failed while the picker was NOT awaiting (impossible in
        // the round-60 flow: every request completes inside the open picker
        // through legacyFileCopyBrowsePump).  Fail quiet and stay closed —
        // never rebuild a new dialog from a stale response.
        return;
    }
    // No picker is open and no request was in flight: a stale completion
    // must never rebuild a NEW dialog instance (round 60 — this was the
    // round-59 failure mode: the response landed after the user closed the
    // picker and the old code created a second picker, so the refreshed list
    // appeared in the WRONG window).  The browse surface stays closed.
}

// Round 60: builds the level-2 picker ONCE and keeps it up for the whole
// browse session.  The first read-only browse-target listing is issued
// through a zero timer so it runs INSIDE the picker's exec loop (the picker
// is already visible, greyed while in flight) — the level-1 entry dialog
// hands over directly with no blank background window in between.
// Stack-allocated picker (the config-editor pattern): every child is
// parented to the stack dialog, so the whole browse-picker lifecycle
// stays inside this frame and nothing is heap-allocated without a parent.
// (Qt3's static QInputDialog::getItem heap-allocates and deletes its
// dialog around the modal exec; the stack dialog avoids that pattern.)
void LegacyMainWindow::openFileCopyBrowsePicker()
{
    QDialog picker(this, "legacy-file-copy-browse", true);
    const bool sourceMode = m_fileCopyBrowseMode == 1;
    picker.setCaption(sourceMode
                          ? tr("Select repair-system source folder")
                          : tr("Select repair-system destination"));
    QVBoxLayout *pickerLayout = new QVBoxLayout(&picker, 10, 8);
    QLabel *pickerHint = new QLabel(
        tr("Current repaired-system folder: %1").arg(m_fileCopyBrowsePath),
        &picker);
    enableLabelWordWrap(pickerHint);
    pickerHint->setMaximumWidth(520);
    pickerLayout->addWidget(pickerHint);
    QListView *pickerList = new QListView(&picker);
    pickerList->addColumn(tr("Folder"));
    pickerList->setSorting(-1);
    pickerList->setAllColumnsShowFocus(true);
    pickerLayout->addWidget(pickerList, 1);
    // Round 57 (modern runRepairFolderBrowser parity): the navigation row
    // carries Up / Open Selected / Refresh (replacing the old
    // "(parent folder)" pseudo-entries and the single Open button), and the
    // confirmation row carries Choose This Folder / Cancel.  Round 59: each
    // navigation action issues the next read-only browse-target request
    // through the member slot and KEEPS the picker open (disabled/busy while
    // the request is in flight — the completion updates the list in place,
    // see handleFileCopyBrowseResult); Choose This Folder accepts with the
    // current folder (the destination mode then re-opens the level-1 entry
    // dialog for the OK confirmation — the modern two-level workflow).
    QHBoxLayout *navigation = new QHBoxLayout(pickerLayout);
    navigation->setSpacing(6);
    QPushButton *upButton = makeButton(tr("Up"), &picker);
    QPushButton *openButton = makeButton(tr("Open Selected"), &picker);
    QPushButton *refreshButton = makeButton(tr("Refresh"), &picker);
    upButton->setEnabled(m_fileCopyBrowsePath != QString::fromLatin1("/"));
    QToolTip::add(upButton, tr("Show the parent folder in the repaired system."));
    QToolTip::add(openButton, tr("Open the selected repaired-system folder."));
    QToolTip::add(refreshButton, tr(
        "Reload this repaired-system folder through a fresh read-only mount."));
    navigation->addWidget(upButton);
    navigation->addWidget(openButton);
    navigation->addWidget(refreshButton);
    navigation->addStretch();
    QLabel *pickerStatus = new QLabel(
        tr("Read-only target view; no files are modified."), &picker);
    enableLabelWordWrap(pickerStatus);
    pickerStatus->setMaximumWidth(520);
    pickerLayout->addWidget(pickerStatus);
    QHBoxLayout *pickerButtons = new QHBoxLayout(pickerLayout);
    pickerButtons->setSpacing(6);
    pickerButtons->addStretch();
    QPushButton *pickerCancel = makeButton(tr("Cancel"), &picker);
    QPushButton *pickerChoose = makeButton(tr("Choose This Folder"), &picker);
    pickerChoose->setDefault(true);
    // Round 72 (modern parity, TUXEDO button order): Choose This Folder sits
    // LEFT of Cancel with the Choose default (the modern folder browser's
    // addUniformDialogButtons [Choose This Folder][Cancel] convention).
    pickerButtons->addWidget(pickerChoose);
    pickerButtons->addWidget(pickerCancel);
    QObject::connect(pickerCancel, SIGNAL(clicked()), &picker, SLOT(reject()));
    QObject::connect(pickerChoose, SIGNAL(clicked()), &picker, SLOT(accept()));
    // Qt 3.3.7 QDialog::accept()/reject()/done() are all protected.  Only the
    // confirmation row reaches the closing slots; each navigation action is
    // wired to the member slot alone (the picker stays open — round 59) and
    // the member slot latches the busy state in legacyFileCopyBrowseIssue().
    QObject::connect(upButton, SIGNAL(clicked()), this,
                     SLOT(fileCopyBrowsePickerUp()));
    QObject::connect(openButton, SIGNAL(clicked()), this,
                     SLOT(fileCopyBrowsePickerOpen()));
    QObject::connect(refreshButton, SIGNAL(clicked()), this,
                     SLOT(fileCopyBrowsePickerRefresh()));
    QObject::connect(pickerList, SIGNAL(doubleClicked(QListViewItem *)), this,
                     SLOT(fileCopyBrowsePickerOpen()));
    m_fileCopyBrowsePickerDialog = &picker;
    m_fileCopyBrowsePickerList = pickerList;
    m_fileCopyBrowsePickerHint = pickerHint;
    m_fileCopyBrowsePickerUp = upButton;
    m_fileCopyBrowsePickerAwaiting = false;
    m_fileCopyBrowsePickerLate = false;
    picker.resize(520, 380);
    // The FIRST listing is issued through a zero timer so it runs inside the
    // picker's own exec loop (a modal Qt3 exec loop delivers QTimer events;
    // it does not deliver the QProcess completion — round 60, see
    // legacyFileCopyBrowsePump).  The picker shows immediately and greys out
    // while the listing is in flight — no blank window between the levels.
    QTimer::singleShot(0, this, SLOT(fileCopyBrowsePickerRefresh()));
    const int pickerResult = picker.exec();
    m_fileCopyBrowsePickerDialog = 0;
    m_fileCopyBrowsePickerList = 0;
    m_fileCopyBrowsePickerHint = 0;
    m_fileCopyBrowsePickerUp = 0;
    m_fileCopyBrowsePickerAwaiting = false;
    m_fileCopyBrowsePickerLate = false;
    if (pickerResult != QDialog::Accepted) {
        // Cancel (a navigation action never closes the picker; its
        // completion updates the open dialog in place — rounds 59/60).
        return;
    }
    // Choose This Folder with the current folder.
    if (sourceMode) {
        // Stage the chosen in-chroot folder as a source (deduplicated),
        // exactly like the modern browseFileCopySource staging path.
        if (!m_fileCopySources.contains(m_fileCopyBrowsePath)) {
            m_fileCopySources.append(m_fileCopyBrowsePath);
            if (m_fileCopySourceList) {
                new QListViewItem(m_fileCopySourceList, m_fileCopyBrowsePath);
            }
        }
        appendLog(tr(
            "Added repaired-system folder path to the Repair \xE2\x86\x92 "
            "Host staging list: %1. No copy occurred.")
            .arg(m_fileCopyBrowsePath));
        updateFileCopyTab();
        return;
    }
    // Destination mode: return the chosen folder to the level-1 entry dialog
    // (pre-filled) so OK confirms it — the modern two-level workflow.
    fileCopyBrowseEntry(m_fileCopyBrowsePath);
}

// Issues the next read-only browse-target helper request for a navigation
// action (Up / Open Selected / Refresh) or the picker's first listing.
// Round 59 (picker persistence): the picker STAYS OPEN — the member slot
// latches the in-flight state (awaiting + disabled/busy dialog) before the
// request starts; handleFileCopyBrowseResult() re-enables and updates the
// dialog in place when the listing lands.  Round 60: after a successful
// start the completion is pumped to delivery while the picker stays visible
// (legacyFileCopyBrowsePump) — Qt 3.3.7 does not deliver the QProcess
// completion through the picker's modal exec loop on the rig, so the
// round-59 passive wait left the picker disabled until the user closed it
// and the response then rebuilt a NEW dialog (the wrong window).  The
// dialog's accept()/reject()/done() are protected members and cannot be
// called from here (Qt 3.3.7); only the confirmation row closes it.
void LegacyMainWindow::legacyFileCopyBrowseIssue(const QString &path)
{
    if (m_fileCopyBrowsePickerDialog) {
        m_fileCopyBrowsePickerAwaiting = true;
        m_fileCopyBrowsePickerDialog->setEnabled(false);
    }
    QStringList args;
    args << QString::fromLatin1("browse-target")
         << m_committedDisk << m_committedRoot << path;
    const bool started = startCommand(args, false,
                 m_fileCopyBrowseMode == 1
                     ? tr("File Copy - Browse Source Folders")
                     : tr("File Copy - Browse Target Folders"),
                 false, false, false, false, QString::null, QString::null,
                 true);
    // A refused start (expired authorization, smoke-mode elevation fail)
    // never completes: un-latch so the picker does not stay disabled.
    if (!started && m_fileCopyBrowsePickerDialog) {
        m_fileCopyBrowsePickerAwaiting = false;
        m_fileCopyBrowsePickerDialog->setEnabled(true);
    }
    if (!started) {
        return;
    }
    legacyFileCopyBrowsePump();
}

// Bounded request-to-response pump for an in-flight browse-target request
// (round 60).  The picker's modal exec loop delivers QTimer events (the
// busy-indicator animation proves that) but on the rig it does NOT deliver
// the QProcess completion, so the round-59 passive wait could never update
// the visible picker.  This pump re-enters the event loop directly and
// additionally polls QProcess::isRunning(), whose Qt 3.3.7 implementation
// synchronously reaps the exited child and, through the sigchld pipe,
// triggers the processExited delivery even when the loop has not activated
// the notifier.  The in-place handler resolves the awaiting state on every
// path (success, failure, refusal and this timeout), so the disabled picker
// can never stay greyed.
void LegacyMainWindow::legacyFileCopyBrowsePump()
{
    QTime deadline = QTime::currentTime().addSecs(60);
    while (m_fileCopyBrowsePickerDialog && m_fileCopyBrowsePickerAwaiting
           && QTime::currentTime() < deadline) {
        qApp->processEvents();
        if (!m_fileCopyBrowsePickerDialog || !m_fileCopyBrowsePickerAwaiting) {
            return;
        }
        // Two event-loop-independent delivery paths (round 60):
        // (1) QProcess::isRunning() synchronously reaps the exited child and,
        // through its sigchld pipe, triggers the processExited emission even
        // when the event loop has not activated the notifier;
        // (2) HelperRunner::pollFinished() reports the reaped-but-unreported
        // child directly (the pipe byte could be missing entirely).
        if (m_runner && !m_runner->isRunning()) {
            m_runner->pollFinished();
        }
        if (!m_fileCopyBrowsePickerDialog || !m_fileCopyBrowsePickerAwaiting) {
            return;
        }
        browsePumpSleep();
    }
    if (m_fileCopyBrowsePickerDialog && m_fileCopyBrowsePickerAwaiting) {
        // Fail closed: cancel the wedged request and resolve the picker to
        // its previous listing with the failure warning.  The late
        // completion of the cancelled request is swallowed quietly (the
        // m_fileCopyBrowsePickerLate latch in handleFileCopyBrowseResult).
        m_fileCopyBrowsePickerLate = true;
        m_runner->cancel();
        handleFileCopyBrowseResult(toStd(m_transcript), false);
    }
}

void LegacyMainWindow::fileCopyBrowsePickerUp()
{
    if (m_fileCopyBrowsePath != QString::fromLatin1("/")) {
        const int slash = m_fileCopyBrowsePath.findRev('/');
        m_fileCopyBrowsePath = slash > 0
            ? m_fileCopyBrowsePath.left(slash) : QString::fromLatin1("/");
    }
    legacyFileCopyBrowseIssue(m_fileCopyBrowsePath);
}

void LegacyMainWindow::fileCopyBrowsePickerOpen()
{
    QString picked;
    if (m_fileCopyBrowsePickerList
        && m_fileCopyBrowsePickerList->currentItem()) {
        picked = m_fileCopyBrowsePickerList->currentItem()->text(0);
    }
    if (picked.isEmpty()) {
        return;
    }
    m_fileCopyBrowsePath = m_fileCopyBrowsePath == QString::fromLatin1("/")
        ? QString::fromLatin1("/") + picked
        : m_fileCopyBrowsePath + QString::fromLatin1("/") + picked;
    legacyFileCopyBrowseIssue(m_fileCopyBrowsePath);
}

void LegacyMainWindow::fileCopyBrowsePickerRefresh()
{
    legacyFileCopyBrowseIssue(m_fileCopyBrowsePath);
}

void LegacyMainWindow::fileCopyAddFiles()
{
    const QStringList files = QFileDialog::getOpenFileNames(
        QString::null, QString::null, this, "add-files",
        tr("Add files to copy"));
    if (files.isEmpty()) {
        return;
    }
    for (QStringList::ConstIterator it = files.begin(); it != files.end(); ++it) {
        if (!m_fileCopySources.contains(*it)) {
            m_fileCopySources.append(*it);
            new QListViewItem(m_fileCopySourceList, *it);
        }
    }
    updateFileCopyTab();
}

void LegacyMainWindow::fileCopyAddFolder()
{
    const QString folder = QFileDialog::getExistingDirectory(
        QString::null, this, "add-folder",
        tr("Add folder to copy"));
    if (folder.isEmpty() || m_fileCopySources.contains(folder)) {
        return;
    }
    m_fileCopySources.append(folder);
    new QListViewItem(m_fileCopySourceList, folder);
    updateFileCopyTab();
}

void LegacyMainWindow::fileCopyRemoveSelected()
{
    QListViewItem *item = m_fileCopySourceList
        ? m_fileCopySourceList->currentItem() : 0;
    if (!item) {
        return;
    }
    m_fileCopySources.remove(item->text(0));
    delete item;
    updateFileCopyTab();
}

void LegacyMainWindow::fileCopyClearStaging()
{
    m_fileCopySources.clear();
    if (m_fileCopySourceList) {
        m_fileCopySourceList->clear();
    }
    updateFileCopyTab();
}

// Starts copy-preview or copy through the legacy helper (the helper keeps the
// direction/path containment checks, the sensitive-destination refusal and
// the per-file cmp verification).
bool LegacyMainWindow::startFileCopyCommand(bool realCopy)
{
    QString reason;
    if (!fileCopyReady(&reason)) {
        QMessageBox::information(this, tr("File Copy unavailable"),
                                 reason, QMessageBox::Ok, QMessageBox::NoButton);
        return false;
    }
    const QString destination = m_fileCopyDestinationEdit
        ? m_fileCopyDestinationEdit->text().stripWhiteSpace() : QString::null;
    if (m_fileCopySources.isEmpty() || destination.isEmpty()) {
        QMessageBox::information(
            this, tr("File Copy"),
            tr(
                "Stage at least one source and name a destination path first."),
            QMessageBox::Ok, QMessageBox::NoButton);
        return false;
    }
    if (realCopy) {
        const bool answer = confirmWrapped(
            this, tr("Copy and Verify"),
            tr(
                "Copy the %1 staged item(s) to %2?\n\nThe helper keeps its "
                "direction and path containment checks; a sensitive repair-system "
                "destination is refused unless the helper approves it, and every "
                "regular file is byte-compared after the copy.")
                .arg(static_cast<int>(m_fileCopySources.size()))
                .arg(destination),
            /*cancelInsteadOfNo=*/false, QString::null, QString::null,
            kHiddenInputMaximumWidth, /*noIsDefault=*/true);
        if (!answer) {
            return false;
        }
    }
    const QString ownership = m_fileCopyOwnershipCombo
        && m_fileCopyOwnershipCombo->currentItem() == 1
            ? QString::fromLatin1("preserve")
            : QString::fromLatin1("smart");
    QStringList args;
    args << (realCopy ? QString::fromLatin1("copy")
                      : QString::fromLatin1("copy-preview"))
         << selectedDisk() << selectedRoot()
         << fileCopyDirection()
         << ownership
         << QString::fromLatin1("normal")
         << destination;
    for (QStringList::ConstIterator it = m_fileCopySources.begin();
         it != m_fileCopySources.end(); ++it) {
        args << *it;
    }
    const QString label = realCopy ? tr("File Copy - Copy and Verify")
                                   : tr("File Copy - Preview Changes");
    const bool started = startCommand(args, false, label, false, false, false,
                                      false, QString::null,
                                      QString::fromLatin1("file-copy"));
    if (started && !m_smokeMode) {
        showRepairResultDialog(label);
    }
    return started;
}

void LegacyMainWindow::fileCopyPreview()
{
    startFileCopyCommand(false);
}

void LegacyMainWindow::fileCopyRun()
{
    startFileCopyCommand(true);
}

void LegacyMainWindow::updateFactView(const ParsedTranscript &parsed)
{
    m_factMap.clear();
    for (std::size_t i = 0; i < parsed.targetFacts.size(); ++i) {
        const std::string &line = parsed.targetFacts[i];
        const std::string::size_type colon = line.find(": ");
        if (colon == std::string::npos) {
            continue;
        }
        m_factMap.insert(fromStd(line.substr(0, colon + 1)),
                         fromStd(line.substr(colon + 2)));
    }
    m_helperDisk = mapValue(m_factMap, QString::fromLatin1("Physical drive:"));
    m_helperComponent = mapValue(m_factMap, QString::fromLatin1("Detected component:"));
    if (m_helperComponent.isEmpty()) {
        m_helperComponent = mapValue(m_factMap, QString::fromLatin1("Running host root mount source:"));
    }
    if (m_helperComponent.isEmpty()) {
        m_helperComponent = mapValue(m_factMap, QString::fromLatin1("Target root mount:"));
    }
    if (m_helperComponent.isEmpty()) {
        m_helperComponent = mapValue(m_factMap, QString::fromLatin1("Root mount source:"));
    }
    updateHostCard();
    updateDriveDetails();
    updateUnlockStatus();
}

void LegacyMainWindow::updateDriveDetails()
{
    if (!m_detailList) {
        return;
    }
    m_detailList->clear();
    const QString disk = m_inspectingHostDetails
        ? runningHostDisk() : previewDisk();
    if (m_detailsPaneTitle) {
        m_detailsPaneTitle->setText(m_inspectingHostDetails
            ? tr("Protected running host - details")
            : tr("Selected drive details"));
    }
    // Qt3 prepends a plain insertion when sorting is disabled; chain the rows
    // with the after-form constructor to keep the documented field order
    // (Drive, Detected target, Model/label, ...).
    const std::vector<std::pair<QString, QString> > rows = driveDetailsRows(disk);
    QListViewItem *lastDetail = 0;
    for (std::size_t i = 0; i < rows.size(); ++i) {
        lastDetail = new WrappingDetailItem(m_detailList, lastDetail,
                                            rows[i].first, rows[i].second);
    }
}

// The Field/Value details rows for a drive (the same rows the Systems details
// pane and the protected-host Details dialog show). Shared by both so the two
// views cannot drift apart.
std::vector<std::pair<QString, QString> > LegacyMainWindow::driveDetailsRows(
    const QString &disk) const
{
    std::vector<std::pair<QString, QString> > rows;
    QMap<QString, DeviceRow>::const_iterator row = m_rows.find(disk);
    const DeviceRow *selected = row == m_rows.end() ? 0 : &row.data();
    const bool helperConfirmed = !disk.isEmpty() && m_helperDisk == disk;

    // Modern parity: the inspected row (a child component when one is
    // selected, otherwise the drive) supplies the details; the Drive field
    // always names the top-level drive. For the protected-host pane the
    // resolved root component supplies UUID and Mounts.
    QString inspectedPath = previewComponent();
    if (inspectedPath.isEmpty() && m_inspectingHostDetails) {
        inspectedPath = autoResolvedRoot(disk);
    }
    if (inspectedPath.isEmpty()) {
        inspectedPath = disk;
    }
    QMap<QString, DeviceRow>::const_iterator inspectedRow = m_rows.find(inspectedPath);
    const DeviceRow *inspected = inspectedRow == m_rows.end() ? selected : &inspectedRow.data();
    const bool inspectingChild = !previewComponent().isEmpty()
        && previewComponent() != disk;

    QString model;
    QString size;
    QString transport;
    QString fstype;
    QString uuid;
    QString mounts;
    if (inspected) {
        // Model/label: the inspected row first, with the owning disk's
        // model/label as the fallback for mapper/partition children (the
        // inventory keeps the model only on disk rows).
        QString childModel = fromStd(inspected->model);
        QString childLabel = fromStd(inspected->label);
        if ((childModel.isEmpty() || childLabel.isEmpty()) && selected) {
            if (childModel.isEmpty()) {
                childModel = fromStd(selected->model);
            }
            if (childLabel.isEmpty()) {
                childLabel = fromStd(selected->label);
            }
        }
        if (childModel.isEmpty()) {
            model = childLabel;
        } else if (childLabel.isEmpty()) {
            model = childModel;
        } else {
            model = childModel + QString::fromLatin1(" (")
                + childLabel + QString::fromLatin1(")");
        }
        size = fromStd(inspected->size);
        transport = fromStd(inspected->transport);
        fstype = inspected->disk ? diskFilesystemSummary(*inspected)
                                 : displayFsType(*inspected);
        uuid = fromStd(inspected->uuid);
        // Cycle 15 disk-level UUID roll-up for the drive row itself:
        // 1) the unlocked root's UUID (UNLOCKED_ROOT_UUID), 2) the locked
        // LUKS container's by-uuid identity, 3) the resolved Linux root
        // component's UUID. A boot partition's UUID is never used at the
        // drive level.
        if (uuid.isEmpty() && inspected->disk) {
            if (!m_unlockedRootUuid.isEmpty() && disk == m_unlockedDisk) {
                uuid = m_unlockedRootUuid;
            } else {
                const QString luks = autoResolvedLuks(disk);
                if (!luks.isEmpty()) {
                    const QMap<QString, DeviceRow>::const_iterator luksRow =
                        m_rows.find(luks);
                    if (luksRow != m_rows.end()) {
                        uuid = fromStd(luksRow.data().uuid);
                    }
                }
                if (uuid.isEmpty()) {
                    const QString rootComponent = autoResolvedRoot(disk);
                    const QMap<QString, DeviceRow>::const_iterator rootRow =
                        m_rows.find(rootComponent);
                    if (rootRow != m_rows.end()) {
                        uuid = fromStd(rootRow.data().uuid);
                    }
                }
            }
        }
        // Helper-confirmed root UUID fallback: the helper names the root
        // component and the inventory carries its by-uuid identity.
        if (uuid.isEmpty() && helperConfirmed && !m_helperComponent.isEmpty()) {
            const QMap<QString, DeviceRow>::const_iterator helperRow =
                m_rows.find(m_helperComponent);
            if (helperRow != m_rows.end()) {
                uuid = fromStd(helperRow.data().uuid);
            }
        }
        // Mounts: the joined distinct targets (primary first); the single
        // mountpoint covers rows without a joined list. The freshly unlocked
        // root is never auto-mounted on selection (mounting happens during
        // diagnostics read-only and repairs read-write).
        mounts = fromStd(inspected->mountpoints);
        if (mounts.isEmpty()) {
            mounts = fromStd(inspected->mountpoint);
        }
        if (mounts.isEmpty() && inspectedPath == m_unlockedRoot
            && !m_unlockedRoot.isEmpty()) {
            mounts = tr("not mounted (offline target)");
        }
    }
    if ((fstype.isEmpty() || fstype == QString::fromLatin1("unknown"))
        && helperConfirmed) {
        fstype = mapValue(m_factMap, QString::fromLatin1("Filesystem:"));
    }

    QString component = previewComponent();
    if (component.isEmpty()) {
        component = autoResolvedRoot(disk);
    }
    if (component.isEmpty() && helperConfirmed) {
        component = m_helperComponent;
    }
    QString status;
    if (disk.isEmpty()) {
        status = tr("Select a drive to see its details.");
    } else if (inspectingChild) {
        status = tr("Inspecting the selected component.");
    } else if (helperConfirmed) {
        status = tr("Helper-confirmed by the last read-only diagnostics.");
    } else {
        status = tr("Read-only inventory only; run diagnostics to confirm.");
    }
    QString protection;
    if (disk.isEmpty()) {
        protection = QString::fromLatin1("-");
    } else if (!runningHostDisk().isEmpty() && disk == runningHostDisk()) {
        protection = tr("PROTECTED - running system; read-only details only");
    } else if (selected && selected->optical) {
        protection = tr("Live / installer media - not selectable");
    } else if (hostScope()) {
        protection = tr("PROTECTED - running host scope");
    } else if (!autoResolvedLuks(disk).isEmpty() && component.isEmpty()) {
        protection = tr("Unlock required before selection");
    } else {
        protection = tr("Eligible repair candidate");
    }

    rows.push_back(std::make_pair(tr("Drive:"),
                                  detailOrDash(disk)));
    rows.push_back(std::make_pair(tr("Detected target:"),
                                  component.isEmpty()
                                      ? tr("Pending inspection")
                                      : component));
    rows.push_back(std::make_pair(tr("Model / label:"),
                                  detailOrDash(model)));
    rows.push_back(std::make_pair(tr("Status:"), status));
    rows.push_back(std::make_pair(tr("Size:"),
                                  detailOrDash(size)));
    rows.push_back(std::make_pair(tr("Connection:"),
                                  detailOrDash(transport)));
    rows.push_back(std::make_pair(tr("Filesystem:"),
                                  detailOrDash(fstype)));
    rows.push_back(std::make_pair(tr("UUID:"),
                                  detailOrDash(uuid)));
    rows.push_back(std::make_pair(tr("Mounts:"),
                                  detailOrDash(mounts)));
    rows.push_back(std::make_pair(tr("Protection:"),
                                  protection));
    return rows;
}

// Cycle 9 protected-host card: the probe-based identity line (helper OS fact)
// and the legacy-available storage line (model/label, device path, size,
// transport, critical mounts from the read-only inventory). Every value stays
// honest when unknown.
void LegacyMainWindow::updateHostCard()
{
    if (!m_hostSystemLabel || !m_hostStorageLabel) {
        return;
    }
    const QString disk = runningHostDisk();
    if (disk.isEmpty()) {
        m_hostSystemLabel->setText(tr(
            "Running system protection unresolved"));
        m_hostStorageLabel->setText(tr(
            "No protected physical backing disk was identified"));
        return;
    }
    // Round 70 (E3, modern parity): resolve the running system's name the way
    // the modern GUI does — the running host's own /etc/os-release
    // PRETTY_NAME (CapabilityChecker::distributionLabel's mechanism), then the
    // helper's resolved fact lines ("System:" carries the ported
    // TARGET_PRETTY, os-release-aware with the legacy-root evidence fallback
    // on Etch where /etc/os-release does not exist), then the historical
    // "OS:"/"Distribution:" keys, and the static fallback only when nothing
    // resolved (the modern "Current running Linux system" parity string).
    QString os = readOsReleaseValue(QString::fromLatin1("PRETTY_NAME"));
    if (os.isEmpty()) {
        os = mapValue(m_factMap, QString::fromLatin1("System:"));
    }
    if (os.isEmpty()) {
        os = mapValue(m_factMap, QString::fromLatin1("OS:"));
    }
    if (os.isEmpty()) {
        os = mapValue(m_factMap, QString::fromLatin1("Distribution:"));
    }
    if (os.isEmpty()) {
        os = tr("Current running Linux system");
    }
    m_hostSystemLabel->setText(os);

    QStringList parts;
    const QMap<QString, DeviceRow>::const_iterator it = m_rows.find(disk);
    QString mounts;
    if (it != m_rows.end()) {
        const DeviceRow &hostRow = it.data();
        QString model = fromStd(hostRow.model);
        if (model.isEmpty()) {
            model = fromStd(hostRow.label);
        }
        if (!model.isEmpty()) {
            parts.append(model);
        }
        parts.append(disk);
        if (!fromStd(hostRow.size).isEmpty()) {
            parts.append(fromStd(hostRow.size));
        }
        if (!fromStd(hostRow.transport).isEmpty()) {
            parts.append(fromStd(hostRow.transport).upper());
        }
        mounts = fromStd(hostRow.mountpoints);
        if (mounts.isEmpty()) {
            mounts = fromStd(hostRow.mountpoint);
        }
    }
    QString summary = parts.join(QString::fromLatin1("  |  "));
    if (!mounts.isEmpty()) {
        if (!summary.isEmpty()) {
            summary += QString::fromLatin1("  |  ");
        }
        summary += tr("Critical mounts: %1").arg(mounts);
    }
    m_hostStorageLabel->setText(summary);
    QToolTip::add(m_hostStorageLabel, tr(
        "Read-only protected-running-system facts: the helper OS fact plus "
        "the inventory model, device path, size, transport and critical "
        "mounts. Nothing here is probed destructively."));
}

// Cycle 9 Details button: the running-host facts in a read-only dialog,
// reusing exactly the rows the Systems details pane shows.
// Cycle 13: the Details button (kept on the host card, since the running
// host is never selectable as a drive row) populates the right Selected
// drive details pane with the running-host facts - the same rows the
// popup used. No dialog: selecting any drive row restores the normal
// per-drive pane.
void LegacyMainWindow::showHostDetails()
{
    m_inspectingHostDetails = true;
    updateDriveDetails();
}

void LegacyMainWindow::updateUnlockStatus()
{
    if (!m_unlockStatusView) {
        return;
    }
    const QString disk = previewDisk();
    if (disk.isEmpty()) {
        m_unlockStatusView->setText(tr("Select a drive to see unlock status."));
        return;
    }
    const QString component = unlockCandidateFor(disk);
    if (hostScope() || (!runningHostDisk().isEmpty() && disk == runningHostDisk())) {
        m_unlockStatusView->setText(tr(
            "State: protected\n"
            "Component: %1\n"
            "Mapper: (none)\n"
            "Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)\n"
            "The protected running host cannot be unlocked or modified; unlock "
            "is available for an offline repair target only. Use Host "
            "Maintenance for the protected running host.")
            .arg(component.isEmpty() ? tr("(none detected)")
                                     : component));
        return;
    }
    const QMap<QString, QString>::const_iterator cached = m_unlockStatusCache.find(disk);
    if (cached != m_unlockStatusCache.end() && !cached.data().stripWhiteSpace().isEmpty()) {
        m_unlockStatusView->setText(cached.data());
        return;
    }
    const QString mapper = visibleMapperForDisk(disk);
    const bool helperUnlocked = !m_helperComponent.isEmpty()
        && m_helperComponent.startsWith(QString::fromLatin1("/dev/mapper/"))
        && !m_helperDisk.isEmpty() && m_helperDisk == disk;
    if (!component.isEmpty()) {
        m_unlockStatusView->setText(tr(
            "State: locked\n"
            "Component: %1\n"
            "Mapper: (none)\n"
            "Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)\n"
            "A locked LUKS container is visible on this drive; press Unlock to "
            "open it for this recovery session.")
            .arg(component));
    } else if (!mapper.isEmpty()) {
        const QString shownComponent = previewComponent().isEmpty()
            ? (m_helperComponent.isEmpty() ? tr("(visible mapper)")
                                           : m_helperComponent)
            : previewComponent();
        m_unlockStatusView->setText(tr(
            "State: unlocked\n"
            "Component: %1\n"
            "Mapper: %2\n"
            "Method: already open before this session (visible mapper; Boot "
            "Bitch will reuse it and will not close it).")
            .arg(shownComponent)
            .arg(mapper));
    } else if (helperUnlocked) {
        m_unlockStatusView->setText(tr(
            "State: unlocked\n"
            "Component: %1\n"
            "Mapper: %2\n"
            "Method: helper-confirmed mapping from the last read-only diagnostics.")
            .arg(m_helperComponent.startsWith(QString::fromLatin1("/dev/mapper/"))
                     ? tr("(mapper)")
                     : m_helperComponent)
            .arg(m_helperComponent));
    } else {
        m_unlockStatusView->setText(tr(
            "State: locked or no encrypted component detected\n"
            "Component: (none detected)\n"
            "Mapper: (none)\n"
            "Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)\n"
            "No locked LUKS component and no unlock operation were recorded for "
            "this drive in the current session."));
    }
}

QString LegacyMainWindow::visibleMapperForDisk(const QString &disk) const
{
    if (disk.isEmpty()) {
        return QString::null;
    }
    const QString kernel = disk.startsWith(QString::fromLatin1("/dev/"))
        ? disk.mid(5) : disk;
    for (QMap<QString, DeviceRow>::const_iterator it = m_rows.begin();
         it != m_rows.end(); ++it) {
        const DeviceRow &row = it.data();
        if (!row.mapper || row.parent.empty()) {
            continue;
        }
        const QString parent = fromStd(row.parent);
        if (parent == kernel) {
            return it.key();
        }
        if (parent.startsWith(kernel) && parent.length() > kernel.length()
            && parent[kernel.length()] >= QChar('0')
            && parent[kernel.length()] <= QChar('9')) {
            return it.key();
        }
    }
    return QString::null;
}

// Cycle 12: after a successful unlock, the freshly activated LVs are added to
// the cached inventory as mapper rows owned by the selected drive (the
// read-only inventory scan cannot see the dm nodes' chain, so the tree would
// otherwise stay empty below the drive). The fstype comes from the helper's
// UNLOCKED_ROOT_FSTYPE probe line, never from a block read here.
void LegacyMainWindow::injectUnlockedMapperRows(const QString &disk,
                                                const QString &mapper,
                                                const QString &root,
                                                const QString &fstype,
                                                const QString &uuid)
{
    if (disk.isEmpty()) {
        return;
    }
    const QString diskName = disk.startsWith(QString::fromLatin1("/dev/"))
        ? disk.mid(5) : disk;
    const QStringList paths = (QStringList() << mapper << root);
    bool added = false;
    for (QStringList::ConstIterator it = paths.begin(); it != paths.end(); ++it) {
        const QString path = *it;
        if (path.isEmpty() || !path.startsWith(QString::fromLatin1("/dev/"))) {
            continue;
        }
        if (path == root) {
            // The freshly-rescanned inventory may already carry the root row
            // while its udev record lags behind the just-activated LVs (the
            // filesystem type is still unknown). Backfill the helper-confirmed
            // fstype/uuid onto the existing row instead of leaving it empty,
            // so the inventory-backed resolution and the details pane both see
            // a Linux root and the container is no longer treated as locked.
            QMap<QString, DeviceRow>::iterator existing = m_rows.find(path);
            if (existing != m_rows.end()) {
                if (!fstype.isEmpty()) {
                    existing.data().probedFstype = toStd(fstype);
                }
                if (!uuid.isEmpty()) {
                    existing.data().uuid = toStd(uuid);
                }
                for (std::size_t i = 0; i < m_inventory.size(); ++i) {
                    if (m_inventory[i].path == toStd(path)) {
                        if (!fstype.isEmpty()) {
                            m_inventory[i].probedFstype = toStd(fstype);
                        }
                        if (!uuid.isEmpty()) {
                            m_inventory[i].uuid = toStd(uuid);
                        }
                        break;
                    }
                }
                continue;
            }
        } else if (m_rows.contains(path)) {
            continue;
        }
        DeviceRow row;
        row.name = toStd(path);
        row.path = toStd(path);
        row.mapper = true;
        // The tree's owning-disk resolution walks the parent chain by name,
        // so the row is parented to the selected drive for display.
        row.parent = toStd(diskName);
        if (path == root) {
            if (!fstype.isEmpty()) {
                row.probedFstype = toStd(fstype);
            } else {
                row.probedFstype = "unknown";
            }
            if (!uuid.isEmpty()) {
                row.uuid = toStd(uuid);
            }
        } else {
            row.probedFstype = "unknown";
        }
        m_rows.insert(path, row);
        m_inventory.push_back(row);
        added = true;
    }
    if (added) {
        rebuildDeviceList();
    }
}

void LegacyMainWindow::mergeHelperDevices(const ParsedTranscript &parsed)
{
    bool added = false;
    for (std::size_t i = 0; i < parsed.devicePaths.size(); ++i) {
        const QString path = fromStd(parsed.devicePaths[i]);
        if (m_rows.contains(path)) {
            continue;
        }
        DeviceRow row;
        row.name = toStd(path);
        row.path = toStd(path);
        row.mapper = path.startsWith(QString::fromLatin1("/dev/mapper/"));
        m_rows.insert(path, row);
        // Keep the helper-reported row in the cached inventory too, so a
        // device-discovery refilter cannot drop it from the details path.
        m_inventory.push_back(row);
        added = true;
    }
    // The tree builder is the single source of truth: helper-reported rows
    // enter the tree only as children of a top-level disk (or mappers whose
    // owning disk resolves); orphan pass-through rows stay out of the tree
    // while their inspection data remains in the inventory.
    if (added) {
        rebuildDeviceList();
    }
}

void LegacyMainWindow::updateElevationLabel()
{
    QString description;
    const bool ok = m_runner->resolveElevation(&description);
    m_lastHelperDescription = description;
    if (m_settingsElevationLabel) {
        m_settingsElevationLabel->setText(ok
            ? description
            : tr("unavailable - %1").arg(description));
    }
}

// Modern scope label parity: the Diagnostics/Repair/Shell pages show the
// committed target, the protected running host while Host Maintenance is
// active, or "Target: none selected".
void LegacyMainWindow::updateScopeLabel()
{
    // Cycle 9: the scope label renders like modern's wrapped scope label -
    // "Host maintenance:" / "Target:" on the first line and the disk (or
    // disk + root) on the second.
    QString text;
    if (hostMaintenanceActive()) {
        text = tr("Host maintenance:\n%1")
                   .arg(selectedDisk().isEmpty()
                            ? tr("unresolved")
                            : selectedDisk());
    } else if (targetCommitted()) {
        text = tr("Target:\n%1 + %2")
                   .arg(m_committedDisk).arg(m_committedRoot);
    } else {
        text = tr("Target: none selected");
    }
    if (m_scopeLabel) {
        m_scopeLabel->setText(text);
    }
    if (m_repairScopeLabel) {
        m_repairScopeLabel->setText(text);
    }
    if (m_chrootScopeLabel) {
        m_chrootScopeLabel->setText(text);
    }
    if (m_fileCopyScopeLabel) {
        m_fileCopyScopeLabel->setText(text);
    }
}

// Modern updateTargetLabels() parity (item #3): the Settings "Show
// capabilities for:" selector follows the active scope - Host Maintenance
// selects "Host", otherwise the committed repair target selects
// "Repair target". The selector only chooses which cached scope the table
// reads; it never creates a scope model of its own. Qt3's setCurrentItem()
// never emits activated(), so the sync never re-reads the cache by itself:
// refreshCapabilities() runs explicitly below only when the selection
// actually changed (so the frequent updateActionStates() passes do not spam
// the session log).
void LegacyMainWindow::syncCapabilityScopeSelector()
{
    if (!m_capabilityScopeSelector) {
        return;
    }
    const int targetIndex = 0;   // "Repair target"
    const int hostIndex = 1;     // "Host"
    const int desired = hostScope() ? hostIndex : targetIndex;
    if (m_capabilityScopeSelector->currentItem() != desired) {
        m_capabilityScopeSelector->setCurrentItem(desired);
        refreshCapabilities();
    }
}

// Modern invalidateAllActiveScopeDiagnostics() parity (item #8): the legacy
// frontend keeps ONE cached capability/evidence model (identity-keyed, not
// two per-scope maps like the modern host/target caches), so a scope switch
// must clear it together with the diagnostic display cache. This is what
// stops a target-scope run from initially showing the previous host-scope
// transcript (and vice-versa): the results pane, the per-section cache, the
// re-run marker and the capability/evidence model are all dropped so the new
// scope starts from "no evidence" until its own diagnostics complete.
void LegacyMainWindow::invalidateScopeDiagnostics()
{
    m_model.reset();
    m_diagnosticSections.clear();
    m_fullDiagnosticReport = QString::null;
    m_lastDiagnosticKey = QString::null;
    m_lastDiagnosticIdentity = QString::null;
    // Item #8 adjacent: the helper-confirmed fact map (and its derived disk/
    // component fields) describe the previous scope's evidence; clearing them
    // with the diagnostics cache stops the Systems "Selected drive details"
    // pane from briefly showing the prior scope's facts.
    m_factMap.clear();
    m_helperDisk = QString::null;
    m_helperComponent = QString::null;
    refreshDiagnosticResultsView();
}

// Deferred-authorization affordance (modern m_authorizationStatusLabel /
// Authorize): the status text and the button state on the Systems and Repair
// pages. The modal hidden-input dialog is opened by authorizeNow().
void LegacyMainWindow::updateAuthorizationAffordance()
{
    // Cycle 11 (modern parity): the Authorize button and status label are
    // only visible in the deferred-authorization state - a scope is ready
    // AND the administrator session is not active. With no scope, or once
    // the session is active, both are hidden (the modern frontend shows no
    // idle authorization row either).
    const bool ready = diagnosticsScopeReady();
    const bool active = administratorSessionActive();
    const bool deferred = ready && !active;

    QString text;
    bool canAuthorize = false;
    if (deferred) {
        // Modern deferred wording (item #6): the status label and tooltip
        // mirror MainWindow::updateAuthorizationAffordance()'s deferred
        // (no-failure) branch, and the Authorize button keeps the modern
        // tooltip.
        text = tr(
            "Administrator authorization deferred - diagnostics will regenerate "
            "after you authorize.");
        canAuthorize = true;
    }
    if (m_authStatusLabel) {
        m_authStatusLabel->setText(text);
        m_authStatusLabel->setHidden(!deferred);
        if (deferred) {
            QToolTip::add(m_authStatusLabel, tr(
                "Authorization deferred; press Authorize."));
        }
    }
    if (m_authorizeButton) {
        m_authorizeButton->setHidden(!deferred);
        m_authorizeButton->setEnabled(canAuthorize);
        if (canAuthorize) {
            QToolTip::add(m_authorizeButton, tr(
                "Establish the privileged helper session for the current scope now."));
        }
    }
}

// Selected-diagnostic pane: modern title/description/availability text plus
// the list tooltip. The list shows the friendly title; the stable helper key
// is mapped back internally for the command and the description.
// Round 32 (Etch #4): the selected check's stable helper key (title->key
// map lookup), shared by updateDiagnosticDetails and the per-diagnostic Run
// button tooltip in updateActionStates.
QString LegacyMainWindow::selectedDiagnosticKey() const
{
    QListViewItem *item = m_diagnosticList ? m_diagnosticList->currentItem() : 0;
    if (!item) {
        return QString::null;
    }
    return mapValue(m_diagKeyByTitle, item->text(0).stripWhiteSpace());
}

void LegacyMainWindow::updateDiagnosticDetails()
{
    if (!m_diagTitle || !m_diagDescription || !m_diagAvailability) {
        return;
    }
    QListViewItem *item = m_diagnosticList ? m_diagnosticList->currentItem() : 0;
    if (!item) {
        m_diagTitle->setText(tr("Select a diagnostic"));
        m_diagDescription->setText(tr("Choose a diagnostic from the list."));
        m_diagAvailability->setText(tr("Unavailable"));
        return;
    }
    const QString key = mapValue(m_diagKeyByTitle,
                                 item->text(0).stripWhiteSpace());
    const QString description = diagnosticDescription(key);
    m_diagTitle->setText(key.isEmpty() ? item->text(0) : diagnosticTitle(key));    m_diagDescription->setText(description);
    QToolTip::add(m_diagnosticList, description);
    if (m_running) {
        m_diagAvailability->setText(tr("Running..."));
    } else if (!diagnosticsScopeReady()) {
        m_diagAvailability->setText(tr("Scope required"));
    } else {
        m_diagAvailability->setText(tr("Ready"));
    }
    // Round 41 Run/Re-run wording (modern parity, the round-40 user ruling):
    // the wording is driven by the per-key USER-RUN latch — "Run Diagnostic"
    // until the user has run this diagnostic, "Re-run Diagnostic" afterwards
    // for the whole session. Cached evidence alone (a Run All auto-refresh, a
    // repair-triggered regeneration) never flips the button, and a cancelled
    // or failed run produces no evidence and never flips. The pre-round-39
    // last-key driver (m_lastDiagnosticKey) only ever offered "Re-run" for
    // the last-run key, and the round-39 cache-driven wording flipped on any
    // cached evidence including the auto-refresh — both superseded.
    if (m_runDiagnosticButton) {
        m_runDiagnosticButton->setText(m_userRunDiagnosticKeys.contains(key.lower())
            ? tr("Re-run Diagnostic")
            : tr("Run Diagnostic"));
    }
}

// Results pane population: after a combined `diagnose all` run the per-section
// cache is parsed and the SELECTED check's own section (title + body) is
// shown, matching the modern GUI. The `report` aggregate shows the full
// combined report; an individual run shows its raw helper output directly
// (set by helperFinished), so a selection change re-populates from the
// per-section cache only.
void LegacyMainWindow::refreshDiagnosticResultsView()
{
    if (!m_rawView) {
        return;
    }
    QListViewItem *item = m_diagnosticList ? m_diagnosticList->currentItem() : 0;
    if (!item) {
        m_rawView->setText(QString::null);
        return;
    }
    const QString key = mapValue(m_diagKeyByTitle,
                                 item->text(0).stripWhiteSpace());
    if (key == QString::fromLatin1("report")) {
        m_rawView->setText(m_fullDiagnosticReport);
        return;
    }
    m_rawView->setText(mapValue(m_diagnosticSections, key));
}

void LegacyMainWindow::updateActionStates()
{
    const bool complete = selectionComplete();
    const bool idle = !m_running;
    const bool ready = diagnosticsScopeReady();
    const QString id = identity();

    // The individual tools list and the Full Repair plan follow the same
    // fail-closed capability/feature probes; updateToolDetails() and
    // updatePlanView() recompute their buttons and reasons, and the Settings
    // plan checkboxes re-apply the modern availability semantics.
    updateToolDetails();
    updatePlanView();
    updatePlanChecks();
    updateDeviceFilterChecks();

    // Round 21 (modern cancel-surface parity): every Cancel affordance arms
    // only while its request kind owns the helper — the Diagnostics page's
    // Cancel while a read-only diagnostic runs, the Shell page's Cancel while
    // a shell command runs, the repair popup's Cancel via
    // updateResultCancelState() for popup-backed runs.  A cancel click (or
    // the window-close path) sets m_resultCancelRequested, which disarms all
    // three one-shot; completion/abort disarms them here because the request
    // no longer owns the gate.
    if (m_diagnosticsCancelButton) {
        m_diagnosticsCancelButton->setEnabled(
            m_running && m_pendingDiagnostic && !m_resultCancelRequested);
    }
    if (m_shellCancelButton) {
        m_shellCancelButton->setEnabled(
            m_running && m_pendingShell && !m_resultCancelRequested);
    }

    const bool diagnosticsEnabled = complete && idle && ready;
    if (m_diagnosticsButton) {
        m_diagnosticsButton->setEnabled(diagnosticsEnabled);
        QToolTip::add(m_diagnosticsButton,
                      diagnosticsEnabled
                          ? tr("Run All - run every available read-only diagnostic for the current scope; this unlocks the gated actions.")
                          : (ready ? tr("Select a target and wait for any running command first.")
                                   : scopeReadyReason()));
    }
    if (m_runDiagnosticButton) {
        const bool hasCheck = m_diagnosticList && m_diagnosticList->currentItem() != 0;
        m_runDiagnosticButton->setEnabled(diagnosticsEnabled && hasCheck);
        // Round 32 (Etch #4): the enabled hint names the SELECTED diagnostic's
        // action (modern per-action button-tooltip parity, the legacy wording)
        // instead of the generic "run the selected diagnostic" text.
        const QString actionHint = !hasCheck || !diagnosticsEnabled
            ? QString::null
            : diagnosticTooltip(selectedDiagnosticKey());
        QToolTip::add(m_runDiagnosticButton, !hasCheck
            ? tr("Select a diagnostic check in the list first.")
            : (diagnosticsEnabled
                ? (actionHint.isEmpty()
                    ? tr("Run Diagnostic - run the selected read-only diagnostic for the current scope.")
                    : tr("Run Diagnostic - %1").arg(actionHint))
                : (ready ? tr("Select a target and wait for any running command first.")
                         : scopeReadyReason())));
    }
    if (m_refreshCapabilitiesButton) {
        // Modern Refresh Capabilities: run the current scope's diagnostics and
        // rebuild the read-only backend profile.
        m_refreshCapabilitiesButton->setEnabled(diagnosticsEnabled);
        QToolTip::add(m_refreshCapabilitiesButton, diagnosticsEnabled
            ? tr("Run All for the current scope and refresh the read-only backend profile.")
            : (ready ? tr("Select a target and wait for any running command first.")
                     : scopeReadyReason()));
    }
    if (m_configButton) {
        // Modern parity: Edit Target File is target-only (never Host
        // Maintenance) and needs a probed, selectable file in the combo.
        const bool configShown = targetCommitted() && !hostMaintenanceActive();
        const bool configEnabled = configShown && idle
            && m_configCombo && m_configCombo->count() > 0;
        m_configButton->setEnabled(configEnabled);
        QToolTip::add(m_configButton, configEnabled
            ? tr(
                  "Read or edit the selected target configuration file through "
                  "the guarded helper; a saved edit invalidates cached diagnostics.")
            : (hostMaintenanceActive()
                ? tr(
                      "Host Maintenance has no target-file editing; commit an "
                      "offline repair target first.")
                : (!targetCommitted()
                    ? tr("Commit an offline repair target first.")
                    : (!idle
                        ? tr("A helper command is already running.")
                        : tr(
                              "No target configuration file is available for this "
                              "target; run diagnostics to probe the list.")))));
    }
    if (m_scanButton) {
        m_scanButton->setEnabled(idle);
    }

    // Unlock mirrors the modern GUI: it is offered only for an offline target
    // (never the protected running host, whether or not Host Maintenance is
    // active) and only when a locked LUKS component is visible on the selected
    // drive; the helper's own cryptsetup preflights verify it at runtime.
    if (m_unlockButton) {
        const bool onRunningHost = !runningHostDisk().isEmpty()
            && previewDisk() == runningHostDisk();
        // Cycle 11 Already-Unlocked state (modern parity): after this
        // session's successful unlock of the selected drive the button reads
        // "Already Unlocked" and is disabled; it returns to the normal
        // gating on another selection or a rescan.
        const bool alreadyUnlocked = !previewDisk().isEmpty()
            && m_unlockedDisk == previewDisk()
            && !m_unlockedRoot.isEmpty();
        if (alreadyUnlocked && idle) {
            updateButtonText(m_unlockButton,
                             tr("Already Unlocked"));
            m_unlockButton->setEnabled(false);
            QToolTip::add(m_unlockButton, tr(
                "An unlocked Linux filesystem is already visible on this "
                "drive. Boot Bitch will reuse the existing mapper and will "
                "not close or reopen a mapping created by this recovery "
                "session. Mounting happens during diagnostics (read-only) "
                "and repairs (read-write); data filesystems are never "
                "auto-mounted on selection."));
        } else {
            updateButtonText(m_unlockButton, tr("Unlock"));
            const QString luks = onRunningHost ? QString::null
                                               : unlockCandidateFor(previewDisk());
            // Cycle 13: Host Maintenance is a scope, not a lock on other
            // drives - a selected offline drive stays unlockable while it is
            // active.
            const bool unlockEnabled = idle && !onRunningHost
                && !previewDisk().isEmpty() && !luks.isEmpty();
            m_unlockButton->setEnabled(unlockEnabled);
            if (onRunningHost) {
                QToolTip::add(m_unlockButton, tr(
                    "The protected running host cannot be unlocked; use Host "
                    "Maintenance for the protected running host."));
            } else if (hostScope()) {
                QToolTip::add(m_unlockButton, tr(
                    "Host Maintenance is the current scope, but the selected "
                    "offline drive can still be unlocked."));
            } else if (previewDisk().isEmpty()) {
                QToolTip::add(m_unlockButton, tr(
                    "Select a physical drive in the Available repair targets list first."));
            } else if (!idle) {
                QToolTip::add(m_unlockButton, tr(
                    "A helper command is already running."));
            } else if (luks.isEmpty()) {
                QToolTip::add(m_unlockButton, tr(
                    "No locked LUKS component is currently visible on this selected "
                    "drive."));
            } else {
                QToolTip::add(m_unlockButton, tr(
                    "Unlock %1 using cryptsetup through the privileged helper. The "
                    "passphrase travels through a private keyfile and is never "
                    "placed in command arguments or logs.").arg(luks));
            }
        }
    }
    if (m_setTargetButton) {
        const bool onRunningHost = !runningHostDisk().isEmpty()
            && previewDisk() == runningHostDisk();
        QMap<QString, DeviceRow>::const_iterator selectedRow =
            m_rows.find(previewDisk());
        const bool optical = selectedRow != m_rows.end()
            && selectedRow.data().optical;
        const QString root = autoResolvedRoot(previewDisk());
        // Cycle 15 (modern diskIsSelectableRepairTarget parity): Select
        // Target is enabled for every selectable non-host, non-optical drive
        // - blank and non-Linux data disks included. The only drive-level
        // refusal besides the protected host and optical media is a locked
        // LUKS container without a visible Linux filesystem. The already-
        // committed drive is disabled (Item #1: its row is already the
        // committed target; every other row stays selectable), with the
        // committed tooltip.
        const bool lockedLuksNoLinux = !autoResolvedLuks(previewDisk()).isEmpty()
            && root.isEmpty();
        // Item #4: "already committed" only applies in ordinary repair-target
        // mode; while Host Maintenance is active the scope is the running host,
        // so the highlighted row always shows the commit (leave Host
        // Maintenance) wording rather than a stale committed tooltip.
        const bool alreadyCommitted = !hostScope() && !m_committedDisk.isEmpty()
            && previewDisk() == m_committedDisk;
        // Commit-path cancel parity (item #7 adjacent): Select Target stays
        // enabled during a read-only diagnostic run - committing cancels that
        // diagnostic and switches scope (setRepairTarget). It only waits (stays
        // disabled) while a mutating repair is running, which is never
        // cancelled.
        const bool busyMutating = m_running && !m_pendingDiagnostic;
        const bool canCommit = !busyMutating && !previewDisk().isEmpty()
            && !onRunningHost && !optical && !lockedLuksNoLinux
            && !alreadyCommitted;
        m_setTargetButton->setEnabled(canCommit);
        if (previewDisk().isEmpty()) {
            QToolTip::add(m_setTargetButton, tr(
                "Select a physical drive in the Available repair targets list first."));
        } else if (onRunningHost) {
            QToolTip::add(m_setTargetButton, tr(
                "The protected running system cannot be selected as a repair "
                "target; use Host Maintenance for the protected running host."));
        } else if (optical) {
            QToolTip::add(m_setTargetButton, tr(
                "Live / installer media is read-only boot media and cannot be "
                "selected as a repair target."));
        } else if (lockedLuksNoLinux) {
            QToolTip::add(m_setTargetButton, tr(
                "Unlock the encrypted volume first; Select Target becomes "
                "available after a Linux filesystem is detected."));
        } else if (alreadyCommitted) {
            QToolTip::add(m_setTargetButton, tr(
                "Committed repair target. Repair, Diagnostics and File Copy "
                "target this physical drive until another drive is explicitly "
                "selected with Select Target."));
        } else if (busyMutating) {
            QToolTip::add(m_setTargetButton, tr(
                "A guarded repair is running; wait for it to finish before "
                "committing a repair target."));
        } else if (hostScope()) {
            QToolTip::add(m_setTargetButton, tr(
                "Commit %1 as the repair target; this leaves Host Maintenance "
                "and switches the scope to the selected drive.")
                .arg(previewDisk()));
        } else {
            QToolTip::add(m_setTargetButton, tr(
                "Commit this physical drive as the repair target."));
        }
    }
    if (m_hostDefaultButton) {
        const bool hostReady = idle && hostScope() && !runningHostDisk().isEmpty();
        std::string defaultReason;
        const bool defaultAvailable = m_model.legacyFeatureAvailable(
            "host-default", toStd(identity()), &defaultReason);
        const bool canDefault = hostReady && defaultAvailable
            && administratorSessionActive();
        m_hostDefaultButton->setEnabled(canDefault);
        if (!canDefault && hostReady) {
            QToolTip::add(m_hostDefaultButton, fromStd(defaultReason));
        }
    }
    if (m_hostDetailsButton) {
        const bool hostKnown = !runningHostDisk().isEmpty();
        m_hostDetailsButton->setEnabled(hostKnown);
        QToolTip::add(m_hostDetailsButton, hostKnown
            ? tr(
                  "Show read-only details for the protected running host.")
            : tr(
                  "The running host target could not be detected."));
    }
    if (m_hostMaintenanceButton) {
        // Modern parity (item #7): Host Maintenance stays enabled during a
        // read-only diagnostic run - selecting it cancels the diagnostic and
        // switches scope (toggleHostMaintenance). It only waits (stays
        // disabled) while a mutating repair is running, which is never
        // cancelled.
        const bool hostKnown = !runningHostDisk().isEmpty();
        const bool busyMutating = m_running && !m_pendingDiagnostic;
        const bool canMaintain = hostKnown && !busyMutating;
        m_hostMaintenanceButton->setEnabled(canMaintain);
        if (m_hostMaintenance) {
            QToolTip::add(m_hostMaintenanceButton,
                          tr("Exit Host Maintenance - return to ordinary repair-target mode."));
        } else if (busyMutating) {
            QToolTip::add(m_hostMaintenanceButton, tr(
                "A guarded repair is running; wait for it to finish before switching scope."));
        } else if (hostKnown) {
            QToolTip::add(m_hostMaintenanceButton,
                          tr("Host Maintenance - run guarded repair stages against the running system."));
        } else {
            QToolTip::add(m_hostMaintenanceButton, tr(
                "The running host target could not be detected; diagnostics need a repair target."));
        }
    }
    if (m_targetSummary) {
        if (hostMaintenanceActive()) {
            m_targetSummary->setText(tr("Host maintenance: %1")
                                         .arg(selectedDisk().isEmpty()
                                                  ? tr("unresolved")
                                                  : selectedDisk()));
        } else if (targetCommitted()) {
            m_targetSummary->setText(tr("Committed target: %1 + %2")
                                         .arg(m_committedDisk).arg(m_committedRoot));
        } else if (m_targetCommitted && !hostScope()) {
            m_targetSummary->setText(tr("Committed target: none (selection changed)"));
        } else {
            m_targetSummary->setText(tr("Committed target: none"));
        }
    }

    // Round 21: the bottom gate-hint label is removed (modern parity — the
    // modern Repair page shows no such text below the tools box; the gating
    // reasons live in the plan readiness line and the Run Full Repair / Run
    // Tool tooltips), so the tools/Selected tool boxes expand down.

    updateScopeLabel();
    updateAuthorizationAffordance();
    updateDiagnosticDetails();
    updateConfigView();
    updateLegacyFeatureView();
}

// The containment bound for a widget: the scroll content widget when the
// widget lives inside a scrollable page (Settings/Repair use a Qt3
// QScrollView), otherwise the tab page itself.
QWidget *LegacyMainWindow::layoutBoundFor(QWidget *widget, QWidget *pageWidget) const
{
    if (m_settingsContent && isInsideWidget(widget, m_settingsContent)) {
        return m_settingsContent;
    }
    if (m_repairContent && isInsideWidget(widget, m_repairContent)) {
        return m_repairContent;
    }
    return pageWidget;
}

void LegacyMainWindow::updateBusyIndicator()
{
    if (!m_busyLabel) {
        return;
    }
    // Modern parity: the reserved row below the guarded-repair badge shows
    // the same working text the modern Qt6 GUI's global busy label uses for
    // the equivalent operation. While a helper command runs a plain QTimer
    // cycles 0..3 trailing dots behind the base text; the row keeps its
    // fixed height either way, so showing/clearing never moves the header or
    // the tab pages.
    if (!m_running) {
        if (m_busyTimer) {
            m_busyTimer->stop();
        }
        m_busyFrame = 0;
        m_busyBaseText = QString::null;
        m_busyLabel->setText(QString::null);
        return;
    }
    m_busyBaseText = busyIndicatorText();
    m_busyFrame = 0;
    m_busyLabel->setText(m_busyBaseText);
    if (m_busyTimer) {
        m_busyTimer->start(kBusyAnimationIntervalMs);
    }
}

void LegacyMainWindow::advanceBusyAnimation()
{
    if (!m_busyLabel) {
        return;
    }
    // The lightweight ellipsis cycle: the base working text plus (frame % 4)
    // dots. Plain ASCII dots only - Etch's fonts garble non-ASCII glyphs and
    // Qt 3.3.7 has no animated-widget busy rendering.
    QString text = m_busyBaseText;
    for (int i = 0; i < (m_busyFrame % 4); ++i) {
        text += QString::fromLatin1(".");
    }
    m_busyLabel->setText(text);
    ++m_busyFrame;
}

QString LegacyMainWindow::busyIndicatorText() const
{
    // The exact strings the modern Qt6 GUI's busy status label shows for the
    // equivalent operation, so the two frontends read identically while they
    // work. Diagnostics runs are scope-aware: the label names the running host
    // versus the repair target (modern "Running %1 diagnostic: %2" /
    // "Running all %1 diagnostics" / "Regenerating %1 diagnostics
    // automatically"), while install/repair/unlock keep their modern
    // scope-independent labels.
    if (m_pendingDiagnostic) {
        const QString scopeWord = hostScope() ? tr("Host") : tr("Target");
        if (m_pendingQuiet) {
            return hostScope()
                ? tr("Regenerating Host diagnostics automatically")
                : tr("Regenerating Target diagnostics automatically");
        }
        if (!m_pendingDiagnosticKey.isEmpty()) {
            return tr("Running %1 diagnostic: %2")
                .arg(scopeWord)
                .arg(diagnosticTitle(m_pendingDiagnosticKey));
        }
        return hostScope()
            ? tr("Running all Host diagnostics")
            : tr("Running all Target diagnostics");
    }
    if (m_pendingUnlock) {
        return tr("Unlocking %1").arg(m_unlockDevice);
    }
    // Repair tools, Full Repair, Make Default and File Copy already carry the
    // modern tool/operation titles as their pending label.
    return m_pendingLabel;
}

void LegacyMainWindow::updateStatus()
{
    // Cycle 15: the scope/readiness message is transient (modern parity) and
    // clears after a few seconds; nothing permanent stays in the bar.
    const QString scope = hostScope()
        ? tr("running host")
        : tr("offline target");
    const QString readiness = hostMaintenanceActive()
        ? tr("host maintenance active")
        : (targetCommitted()
            ? tr("repair target committed")
            : tr("no committed scope"));
    statusBar()->message(QString::fromLatin1("%1 | %2").arg(scope).arg(readiness),
                         6000);
}

// Round 32 (Etch #3): modern log-format parity. Every displayed/saved entry
// renders in the modern shape — a "---- CATEGORY ----" header plus a
// timestamped, level-tagged line — while the stored LogEntry.text stays the
// RAW message (the diagnostics pane and the smoke assertions consume raw
// texts, so the formatting is applied only at the display/file sinks). Blank
// separator lines stay blank.
// Round 34 (Etch #4), two fixes:
// 1. Etch-safe separator. The round-32 box-drawing run (U+2500) did not
//    survive the Etch Qt3 font/encoding ("garbled ... APPLICATION ...
//    garbled"), so the separator is now the ASCII dash form
//    "---- CATEGORY ----" — modern parity in structure, Etch-safe glyphs.
// 2. One header per section, not per line. The modern GUI appends one
//    complete section per log entry (the whole section body is a single
//    append), so its per-entry header appears once per diagnostic section and
//    once per repair block; the legacy streams the helper transcript line by
//    line, so the emitter groups the lines: a header is prepended only when
//    the (kind, diagnostic key, stage set) section identity differs from the
//    previously formatted entry. `sectionState` carries the caller's grouping
//    state (null = every entry starts a new section); the live register/file
//    sequence shares one state, and each rebuilt view or saved file starts
//    its own.
// Round 36 (Etch #5): the rendered timestamp is the entry's CAPTURED arrival
// stamp (LogEntry::stamp), never the format-time clock — a view rebuild, a
// buffered-diagnostic flush or a Save As re-formats the whole register, and a
// format-time clock would re-stamp every entry with one shared moment (the
// user saw 16:50:44 on every line of a multi-entry block). A null stamp
// (synthetic smoke entries) falls back to the format time.
QString logEntryIdentity(const LogEntry &entry)
{
    return QString::fromLatin1("%1\x1f%2\x1f%3")
        .arg(static_cast<int>(entry.kind))
        .arg(entry.diagnostic)
        .arg(entry.stages);
}

QString formattedLogEntry(const LogEntry &entry, QString *sectionState)
{
    if (entry.text.isNull() || entry.text.isEmpty()) {
        return QString::fromLatin1("");
    }
    const QString category = entry.kind == LogEntry::Diagnostic
        ? QString::fromLatin1("DIAGNOSTIC")
        : (entry.kind == LogEntry::Repair
            ? QString::fromLatin1("REPAIR")
            : QString::fromLatin1("APPLICATION"));
    const QDateTime stamp = entry.stamp.isNull()
        ? QDateTime::currentDateTime()
        : entry.stamp;
    const QString timestamp = stamp
        .toString(QString::fromLatin1("yyyy-MM-dd hh:mm:ss"));
    const QString body = QString::fromLatin1("[%1] [INFO] %2")
        .arg(timestamp).arg(entry.text);
    const QString identity = logEntryIdentity(entry);
    if (sectionState && *sectionState == identity) {
        return body;
    }
    if (sectionState) {
        *sectionState = identity;
    }
    return QString::fromLatin1("---- %1 ----\n%2").arg(category).arg(body);
}

void LegacyMainWindow::appendLog(const QString &line)
{
    const QString text = line.isNull() ? QString::fromLatin1("") : line;
    // Tagged at capture time so the Logs filter combo can select entries by
    // kind, section key and repair stage. Round 36 (Etch #5): the per-entry
    // ARRIVAL stamp is captured here too (modern appendLog() parity), so the
    // display/file sinks can render it no matter when they re-format the
    // register.
    LogEntry entry;
    entry.text = text;
    entry.kind = m_logEntryKind;
    entry.diagnostic = m_logEntryDiagnostic;
    entry.stages = m_logEntryStages;
    entry.stamp = QDateTime::currentDateTime();
    m_logEntries.push_back(entry);
    ensureLogFile();
    const QString formatted = formattedLogEntry(entry, &m_logSectionState);
    if (!m_logPath.isEmpty()) {
        appendToLogFile(formatted);
    }
    if (!m_viewingPriorLog && logLinePassesFilter(entry)) {
        prependLogEntryToView(entry);
    }
}

// Round 36 (Etch #5): renders one entry into the live Logs view, newest at
// the top. The view sequence runs newest-first (the reverse of the register),
// so the grouping state is the separate m_logViewSectionState (the newest
// rendered entry); the string is prepended through insertParagraph() at the
// top — QTextEdit::append() would grow the view downwards, leaving the newest
// entry at the bottom like the round-35 behavior this fixes.
void LegacyMainWindow::prependLogEntryToView(const LogEntry &entry)
{
    if (!m_logView) {
        return;
    }
    const QString formatted = formattedLogEntry(entry, &m_logViewSectionState);
    m_logView->insertParagraph(formatted, 0);
}

// Flushes the diagnostic stream buffered during a run into the register and
// the session file, preserving each line's captured kind/section tag (modern
// hide-until-complete parity: the buffered transcript is revealed only once
// the run completes).
void LegacyMainWindow::flushBufferedDiagnosticEntries()
{
    if (m_bufferedDiagnosticEntries.empty()) {
        return;
    }
    ensureLogFile();
    for (std::size_t i = 0; i < m_bufferedDiagnosticEntries.size(); ++i) {
        const LogEntry &entry = m_bufferedDiagnosticEntries[i];
        m_logEntries.push_back(entry);
        const QString formatted = formattedLogEntry(entry, &m_logSectionState);
        if (!m_logPath.isEmpty()) {
            appendToLogFile(formatted);
        }
        // Round 36 (Etch #5): the revealed block joins the view at the top
        // (newest first), like every live append.
        if (!m_viewingPriorLog && logLinePassesFilter(entry)) {
            prependLogEntryToView(entry);
        }
    }
    m_bufferedDiagnosticEntries.clear();
}

// Ensures the session-log directory. The GUI's own default tree
// (~/.boot-repair-legacy/logs) is private: components are created 0700 and
// the leaf is re-tightened to 0700 (a pre-existing directory from an earlier
// run with looser permissions, or one weakened by umask, is fixed here). A
// user-supplied --log-dir is never chmod'ed: it may be a shared path and
// keeps the permissions its owner chose.
void LegacyMainWindow::ensureLogDirectory()
{
    if (m_logDirectory.isEmpty()) {
        m_logDirectory = QDir::homeDirPath()
            + QString::fromLatin1("/.boot-repair-legacy/logs");
    }
    if (isDefaultLogTree(m_logDirectory)) {
        ensureDirectory(m_logDirectory, 0700);
        // F05 pattern (UI-1): re-tighten the leaf through a symlink-refusing
        // fd instead of chmod(path) (which would follow a symlink).
        ensurePrivateDirectory0700(m_logDirectory);
    } else {
        ensureDirectory(m_logDirectory, 0755);
    }
}

void LegacyMainWindow::ensureLogFile()
{
    if (!m_logPath.isEmpty()) {
        return;
    }
    ensureLogDirectory();
    m_logPath = m_logDirectory + QString::fromLatin1("/")
        + QDateTime::currentDateTime().toString(QString::fromLatin1("yyyyMMdd-hhmmss"))
        + QString::fromLatin1(".log");
    refreshSessionLogList();
}

void LegacyMainWindow::appendToLogFile(const QString &line)
{
    ensureLogFile();
    if (m_logPath.isEmpty()) {
        return;
    }
    // Append-only open with O_NOFOLLOW + mode 0600: the session log is the
    // GUI's own file, a replaced symlink must never redirect the write, and
    // fchmod re-tightens the file to 0600 regardless of the process umask
    // (no symlink following, no umask dependence). Written through the
    // fdopen'ed FILE stream in local8Bit, the same encoding the previous
    // QTextStream path produced.
    const QByteArray pathBytes = m_logPath.local8Bit();
    const int fd = ::open(pathBytes.data(),
                          O_WRONLY | O_CREAT | O_APPEND | O_NOFOLLOW, 0600);
    if (fd < 0) {
        return;
    }
    ::fchmod(fd, 0600);
    FILE *file = ::fdopen(fd, "a");
    if (!file) {
        ::close(fd);
        return;
    }
    const QByteArray lineBytes = line.local8Bit();
    // Round 75 (same Qt 3.3.7 trap as the prior-log note append):
    // QString::local8Bit() returns a QCString whose size() includes the
    // terminating NUL; writing size() bytes embeds the NUL between the line
    // and its newline, and every later QString::fromLocal8Bit decode of the
    // file stops at the first NUL (prior-log views truncated to the first
    // line, grep/save output corrupted).  Write the line's own bytes only.
    std::size_t lineByteCount = static_cast<std::size_t>(lineBytes.size());
    if (lineByteCount > 0 && lineBytes.at(lineByteCount - 1) == '\0') {
        --lineByteCount;
    }
    if (lineByteCount > 0) {
        std::fwrite(lineBytes.data(), 1, lineByteCount, file);
    }
    std::fputc('\n', file);
    std::fclose(file);
}

// The combo index -> filter semantics live in logFilterSpecs plus the 16
// diagnostic sections appended after them; the smoke addresses the rows by
// their known order (indexes 0-5 and 6..21 for the sections).
bool LegacyMainWindow::logLinePassesFilter(const LogEntry &entry) const
{
    if (m_logFilterCombo && m_logFilterCombo->currentItem() > 0) {
        const int index = m_logFilterCombo->currentItem();
        if (index == 5) {
            // The File copy workflow has no lines in this frontend (the
            // feature is greyed); the combo entry stays for the 1:1 order.
            return false;
        }
        if (index < logFilterSpecCount) {
            const LogFilterSpec &spec = logFilterSpecs[index];
            if (spec.kind != LogEntry::Application && entry.kind != spec.kind) {
                return false;
            }
            if (spec.kind == LogEntry::Repair && spec.stage[0] != '\0'
                && !entry.stages.stripWhiteSpace().isEmpty()) {
                // Round 41 (Etch #1/#2): a prior-session repair line has no
                // stage tags (the saved file carries none), so the workflow
                // rows accept it as a kind-only match — the same looseness
                // the modern GUI applies to unclassifiable prior-log content.
                // The live register's repair entries always carry stages, so
                // the exact stage match below is unchanged for them.
                if (QString::fromLatin1(spec.stage) == QString::fromLatin1("package")) {
                    if (!stagesContainPackageStage(entry.stages)) {
                        return false;
                    }
                } else if (entry.stages.find(QString::fromLatin1(spec.stage)) < 0) {
                    return false;
                }
            }
        } else {
            // Diagnostic section filters: only Diagnostic entries carrying
            // that section's key.
            const int diagIndex = index - logFilterSpecCount;
            if (diagIndex >= diagnosticSpecCount) {
                return false;
            }
            if (entry.kind != LogEntry::Diagnostic
                || entry.diagnostic != QString::fromLatin1(diagnosticSpecs[diagIndex].key)) {
                return false;
            }
        }
    }
    if (m_logSearchEdit) {
        const QString search = m_logSearchEdit->text();
        if (!search.isEmpty() && entry.text.find(search, 0, false) < 0) {
            return false;
        }
    }
    return true;
}

void LegacyMainWindow::refreshLogView()
{
    if (!m_logView) {
        return;
    }
    m_logView->setText(QString::null);
    if (m_viewingPriorLog) {
        // Round 41 (Etch #1/#2, the round-40 filter regression): the saved
        // session file carries the modern shape — "---- CATEGORY ----"
        // section headers and the helper's "Diagnostic: <key>" markers — so
        // the dropdown filters a prior log the SAME way it filters the live
        // register (modern parity: the modern GUI classifies prior-session
        // entries from their text too). The pre-round-34/41 pass ran every
        // line through logLinePassesFilter as an UNTAGGED Application entry,
        // so only "All entries" ever showed anything and every other category
        // did nothing — the reported regression. Each line is now tagged from
        // the file's own headers/markers as the scan proceeds; a repair
        // line's stages are unknown (the file carries no stage tags), which
        // the workflow rows accept as a kind-only match.
        QString kindWord;    // APPLICATION | DIAGNOSTIC | REPAIR (from the header)
        QString diagKey;     // the current "Diagnostic: <key>" marker
        for (QStringList::ConstIterator it = m_priorLogLines.begin();
             it != m_priorLogLines.end(); ++it) {
            const QString line = *it;
            if (line.startsWith(QString::fromLatin1("---- "))
                && line.endsWith(QString::fromLatin1(" ----"))) {
                const QString word = line.mid(5, line.length() - 10);
                kindWord = (word == QString::fromLatin1("DIAGNOSTIC"))
                    ? QString::fromLatin1("DIAGNOSTIC")
                    : ((word == QString::fromLatin1("REPAIR"))
                        ? QString::fromLatin1("REPAIR")
                        : QString::fromLatin1("APPLICATION"));
                diagKey = QString::null;  // a new section starts
            } else if (line.startsWith(QString::fromLatin1("Diagnostic: "))) {
                const QString key = line.mid(12).stripWhiteSpace();
                for (int i = 0; i < diagnosticSpecCount; ++i) {
                    if (key == QString::fromLatin1(diagnosticSpecs[i].key)) {
                        diagKey = key;
                        break;
                    }
                }
            }
            LogEntry entry;
            entry.text = line;
            entry.kind = (kindWord == QString::fromLatin1("DIAGNOSTIC"))
                ? LogEntry::Diagnostic
                : ((kindWord == QString::fromLatin1("REPAIR"))
                    ? LogEntry::Repair
                    : LogEntry::Application);
            entry.diagnostic = diagKey;
            if (logLinePassesFilter(entry)) {
                m_logView->append(line);
            }
        }
        // Round 36 (Etch #5): a rebuild lands on the top of the document
        // (modern setPlainText() parity), never on the appended tail.
        m_logView->setContentsPos(0, 0);
        return;
    }
    // Round 36 (Etch #5): the live register renders NEWEST-FIRST, exactly like
    // the modern register (appendLog prepends and every rebuild joins it
    // top-down). The reverse pass visits the newest entry first and finishes
    // at the oldest, so the top of the view is the newest entry and the
    // default scroll position is the top — switching the filter/dropdown no
    // longer scrolls all the way down to an old entry. The rebuilt view owns
    // its local section state; the persisted m_logViewSectionState is
    // re-derived from the newest VISIBLE entry so the live prepend path keeps
    // grouping correctly after the rebuild.
    QString viewSectionState;
    QString newestVisibleIdentity;
    bool haveNewest = false;
    for (std::size_t i = m_logEntries.size(); i-- > 0;) {
        if (logLinePassesFilter(m_logEntries[i])) {
            // Round 32 (Etch #3): the live register renders the modern
            // formatted-entry shape (header + timestamped level line); prior
            // session files above are plain text from the file itself.
            // Round 34 (Etch #4): the rebuild starts its own section state,
            // so every filtered section opens with its header.
            if (!haveNewest) {
                newestVisibleIdentity = logEntryIdentity(m_logEntries[i]);
                haveNewest = true;
            }
            m_logView->append(formattedLogEntry(
                m_logEntries[i], &viewSectionState));
        }
    }
    m_logViewSectionState = haveNewest ? newestVisibleIdentity
                                       : QString::null;
    m_logView->setContentsPos(0, 0);
}

QStringList LegacyMainWindow::sessionLogFiles() const
{
    QStringList paths;
    if (m_logDirectory.isEmpty()) {
        return paths;
    }
    QDir dir(m_logDirectory);
    const QStringList names = dir.entryList(QString::fromLatin1("*.log"),
                                            QDir::Files,
                                            QDir::Name | QDir::Reversed);
    for (QStringList::ConstIterator it = names.begin(); it != names.end(); ++it) {
        paths.append(m_logDirectory + QString::fromLatin1("/") + *it);
    }
    return paths;
}

void LegacyMainWindow::refreshSessionLogList()
{
    if (!m_sessionLogList) {
        return;
    }
    const QString wanted = m_viewingPriorLog ? m_priorLogPath : QString::fromLatin1("current");
    m_sessionLogList->clear();
    // Qt3's QListViewItem has no tooltip support; the file names are the
    // selectable labels and the full paths stay in the Settings tab. Sorting is
    // explicitly disabled and every item is chained after the previous one so
    // the live session is always the first entry.
    m_sessionLogList->setSorting(-1);
    QListViewItem *last = new QListViewItem(
        m_sessionLogList, tr("Current session"));
    const QStringList files = sessionLogFiles();
    for (QStringList::ConstIterator it = files.begin(); it != files.end(); ++it) {
        if (!m_logPath.isEmpty() && *it == m_logPath) {
            continue;
        }
        last = new QListViewItem(m_sessionLogList, last, QFileInfo(*it).fileName());
    }
    // Restore the selection so a refresh never changes the displayed log.
    bool selectedPrior = false;
    for (QListViewItem *item = m_sessionLogList->firstChild(); item;
         item = item->nextSibling()) {
        const bool matches = wanted == QString::fromLatin1("current")
            ? item == m_sessionLogList->firstChild()
            : item->text(0) == QFileInfo(wanted).fileName();
        if (matches) {
            m_sessionLogList->setSelected(item, true);
            m_sessionLogList->setCurrentItem(item);
            selectedPrior = item != m_sessionLogList->firstChild();
            break;
        }
    }
    if (!selectedPrior && m_sessionLogList->firstChild()) {
        m_sessionLogList->setSelected(m_sessionLogList->firstChild(), true);
        m_sessionLogList->setCurrentItem(m_sessionLogList->firstChild());
    }
    if (m_deleteSessionLogButton) {
        m_deleteSessionLogButton->setEnabled(selectedPrior);
    }
}

void LegacyMainWindow::saveLog()
{
    // The Save As... dialog starts in /host when a writable system share
    // mount exists there (the etch-share vfat disk), so saved logs land on
    // the share consistently; otherwise the log directory is the fallback.
    // The host fetches the files offline: save to /host, shut the rig down
    // (both Etch domains - the peer rule), then mirror the share host-side;
    // the file appears under the host's shared dir. The dialog itself keeps
    // the ability to choose any directory the user can write.
    QString startDir = m_logDirectory;
    const QFileInfo hostShare(QString::fromLatin1("/host"));
    if (hostShare.exists() && hostShare.isDir() && hostShare.isWritable()) {
        startDir = QString::fromLatin1("/host");
    }
    const QString path = QFileDialog::getSaveFileName(
        startDir, tr("Log files (*.log);;All files (*)"),
        this, "save-log", tr("Save log as"));
    if (path.isEmpty()) {
        return;
    }
    QFile file(path);
    if (!file.open(IO_WriteOnly)) {
        QMessageBox::warning(this, tr("Boot Bitch Legacy"),
                             tr("Could not write %1.").arg(path),
                             QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    QTextStream stream(&file);
    // The complete live register is saved even while a filter hides entries,
    // in the modern formatted-entry shape (matching the session file). The
    // save starts its own section state so the file opens with a header.
    QString text;
    QString saveSectionState;
    for (std::size_t i = 0; i < m_logEntries.size(); ++i) {
        if (i > 0) {
            text += QString::fromLatin1("\n");
        }
        text += formattedLogEntry(m_logEntries[i], &saveSectionState);
    }
    stream << text;
    if (!text.isEmpty()) {
        stream << "\n";
    }
    file.close();
}

void LegacyMainWindow::clearLog()
{
    // Modern clearActionRegister() parity (round 58): clearing the register
    // also clears the cached diagnostic evidence that gates every repair tool,
    // so when cached diagnostics are warm the clear is confirmed first — the
    // modern popup's wording with Clear/Cancel buttons (Cancel is the default,
    // mirroring the modern dialog). A refused confirmation aborts the clear
    // and leaves the register, the cache and the repair-tool gates untouched.
    const bool hasCachedDiagnostics = m_model.hasDiagnostics(toStd(identity()))
        || !m_diagnosticSections.isEmpty() || !m_fullDiagnosticReport.isEmpty();
    if (hasCachedDiagnostics
        && !confirmWrapped(
            this, tr("Clear Register"),
            tr("Clear the action register and cached diagnostics?")
                + QString::fromLatin1("\n\n")
                + tr("Cached diagnostics will also be cleared, and repair tools will be "
                     "disabled until diagnostics are regenerated."),
            /*cancelInsteadOfNo=*/true, tr("Clear"), tr("Cancel"),
            kHiddenInputMaximumWidth, /*noIsDefault=*/true)) {
        return;
    }
    // Clear Register: the live register and view only; prior session files are
    // never modified.
    m_logEntries.clear();
    m_logSectionState = QString::null;
    m_logViewSectionState = QString::null;
    m_viewingPriorLog = false;
    m_priorLogPath = QString::null;
    m_priorLogLines.clear();
    if (m_priorLogBanner) {
        m_priorLogBanner->hide();
    }
    if (m_deleteSessionLogButton) {
        m_deleteSessionLogButton->setEnabled(false);
    }
    // Modern clearActionRegister() parity: clearing the register also clears
    // the cached diagnostic evidence that gates every repair tool, so the
    // visible register and the invisible single source of truth cannot drift
    // apart. Reset the capability model and the per-section display cache, then
    // re-render the diagnostics list and the capability table so no stale
    // evidence lingers after the clear.
    m_model.reset();
    m_diagnosticSections.clear();
    m_fullDiagnosticReport = QString::null;
    m_lastDiagnosticKey = QString::null;
    m_lastDiagnosticIdentity = QString::null;
    refreshDiagnosticResultsView();
    updateDiagnosticDetails();
    refreshCapabilities();
    updateActionStates();
    refreshSessionLogList();
    refreshLogView();
}

void LegacyMainWindow::showAbout()
{
    // Modern Help -> About Boot Bitch (MainWindow::showAboutDialog), adapted
    // truthfully for the legacy frontend. The heading and the developer line
    // mirror the modern dialog; the body states exactly what this Qt 3.3.x
    // frontend and the ported guarded helper provide on Debian Etch-era
    // systems, and which modern-only features stay greyed.
    QMessageBox::about(
        this, tr("About Boot Bitch"),
        tr(
            "<h3>Boot Bitch %1</h3>"
            "<p><b>Developer:</b> CaptainMorgan12</p>"
            "<p>This GUI is the package's entry point: a Qt 3.3.x frontend with "
            "the ported guarded helper for Debian Etch-era systems.</p>"
            "<p><b>Guarded repair mode:</b> read-only diagnostics can inspect "
            "either the protected Running Host or an explicitly selected repair "
            "drive. The guarded Debian/APT package stages (interrupted "
            "configuration, broken dependencies, metadata refresh, upgrade), "
            "GRUB-legacy configuration regeneration, LUKS target unlock and "
            "guarded target-file editing run through the ported helper after "
            "confirmation; Host Maintenance enables the same supported stages "
            "natively on the active system after repeating the host identity "
            "and boot-mount checks.</p>"
            "<p>The modern-only features - verified File Copy, Btrfs snapshot "
            "rollback, EFI/UKI and extlinux repair, boot-stack reconciliation "
            "and Make Default - are greyed with the helper's own probe reasons "
            "on this frontend; Arch/Alpine/Fedora package backends stay "
            "diagnostics-only here.</p>"
            "<p>The first privileged action authorizes one cached administrator "
            "session per scope through a hidden-input modal (the Qt GUI remains "
            "unprivileged). It can be ended at any time from File - Lock "
            "Administrator Session.</p>").arg(QString::fromLatin1(LEGACY_VERSION)));
}

} // namespace legacy
