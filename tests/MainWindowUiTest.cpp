#define private public
#include "MainWindow.h"
#undef private

#include <QApplication>
#include <QAbstractButton>
#include <QClipboard>
#include <QCheckBox>
#include <QColor>
#include <QComboBox>
#include <QDateTime>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDir>
#include <QFile>
#include <QGuiApplication>
#include <QInputDialog>
#include <QLayout>
#include <QFileDialog>
#include <QGroupBox>
#include <QHeaderView>
#include <QListWidget>
#include <QLineEdit>
#include <QLabel>
#include <QMessageBox>
#include <QMetaType>
#include <QPalette>
#include <QPlainTextEdit>
#include <QProcess>
#include <QProgressBar>
#include <QPushButton>
#include <QRegularExpression>
#include <QScrollArea>
#include <QScrollBar>
#include <QSettings>
#include <QSplitter>
#include <QStatusBar>
#include <QStyleHints>
#include <QTabWidget>
#include <QTabBar>
#include <QTableWidget>
#include <QTemporaryDir>
#include <QTextBlock>
#include <QTextDocument>
#include <QTextFragment>
#include <QTextStream>
#include <QTimer>
#include <QTreeWidget>
#include <QToolButton>
#include <QtTest>

#ifdef BOOT_REPAIR_HAVE_DBUS
#include <QDBusVariant>
#endif

#include <algorithm>

namespace {
// Restores an environment variable on scope exit. A failing QVERIFY returns
// early from the test function, so the fixture must be undone by a destructor
// instead of a trailing statement.
class ScopedEnvironmentVariable
{
public:
    ScopedEnvironmentVariable(const char *name, const QByteArray &value)
        : m_name(name)
        , m_hadValue(qEnvironmentVariableIsSet(name))
        , m_original(qgetenv(name))
    {
        qputenv(m_name.constData(), value);
    }

    ~ScopedEnvironmentVariable()
    {
        if (m_hadValue) {
            qputenv(m_name.constData(), m_original);
        } else {
            qunsetenv(m_name.constData());
        }
    }

private:
    QByteArray m_name;
    bool m_hadValue = false;
    QByteArray m_original;
};

QPushButton *buttonWithText(MainWindow &window, const QString &text)
{
    for (QPushButton *button : window.findChildren<QPushButton *>()) {
        if (button->text() == text) {
            return button;
        }
    }
    return nullptr;
}

// Page section titles are created inline by the page builders (only the File
// Copy title is a member). Find the direct child label carrying the exact
// title text so the header-row geometry can be asserted without new members.
QLabel *directChildLabelWithText(QWidget *parent, const QString &text)
{
    if (!parent) {
        return nullptr;
    }
    const QList<QLabel *> labels =
        parent->findChildren<QLabel *>(QString(), Qt::FindDirectChildrenOnly);
    for (QLabel *label : labels) {
        if (label->text() == text) {
            return label;
        }
    }
    return nullptr;
}

// Synthetic unprivileged-scan fixture mirroring an Alpine BIOS target:
// /dev/test-vdb with a 300 MiB ext4 /boot partition, a swap partition and a
// 15.9 GiB ext4 root. The fake lsblk deliberately reports every FSTYPE as
// null (Alpine's musl util-linux build cannot probe block devices as an
// unprivileged user and is not linked against libudev), while the synthetic
// /run/udev/data entries carry the real ID_FS_* evidence. The scanner
// udev-fallback test and the Systems UI regression tests share this fixture so
// they all exercise the same device layout.
struct UnprivilegedVdbFixture {
    QString lsblkPath;
    QString udevDir;
    QString diskPath = QStringLiteral("/dev/test-vdb");
    QString bootPath = QStringLiteral("/dev/test-vdb1");
    QString swapPath = QStringLiteral("/dev/test-vdb2");
    QString rootPath = QStringLiteral("/dev/test-vdb3");
    QString bootUuid = QStringLiteral("11111111-2222-3333-4444-555555555555");
    QString rootUuid = QStringLiteral("66666666-7777-8888-9999-000000000000");
    QString rootPartUuid = QStringLiteral("11111111-22");
};

// Writes the fake lsblk and the udev database below rootDir. Returns an empty
// string on success; otherwise the path of the first step that failed.
QString writeUnprivilegedVdbFixture(const QString &rootDir, UnprivilegedVdbFixture *fixture)
{
    if (!fixture) {
        return QStringLiteral("missing fixture output");
    }

    fixture->lsblkPath = QDir(rootDir).filePath(QStringLiteral("lsblk"));
    QFile lsblk(fixture->lsblkPath);
    if (!lsblk.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return fixture->lsblkPath;
    }
    lsblk.write(R"LSBLK(#!/bin/sh
cat <<'JSON'
{
   "blockdevices": [
      {
         "name": "vdb",
         "kname": "vdb",
         "path": "/dev/test-vdb",
         "maj:min": "253:16",
         "type": "disk",
         "size": 21474836480,
         "fstype": null,
         "mountpoints": [],
         "tran": "virtio",
         "vendor": "0x1af4",
         "children": [
            {
               "name": "vdb1",
               "kname": "vdb1",
               "path": "/dev/test-vdb1",
               "maj:min": "253:17",
               "type": "part",
               "size": 314572800,
               "fstype": null,
               "mountpoints": []
            },
            {
               "name": "vdb2",
               "kname": "vdb2",
               "path": "/dev/test-vdb2",
               "maj:min": "253:18",
               "type": "part",
               "size": 4134535168,
               "fstype": null,
               "mountpoints": []
            },
            {
               "name": "vdb3",
               "kname": "vdb3",
               "path": "/dev/test-vdb3",
               "maj:min": "253:19",
               "type": "part",
               "size": 17024679936,
               "fstype": null,
               "mountpoints": []
            }
         ]
      }
   ]
}
JSON
)LSBLK");
    lsblk.close();
    if (!QFile::setPermissions(fixture->lsblkPath,
            QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner
            | QFileDevice::ReadGroup | QFileDevice::ExeGroup
            | QFileDevice::ReadOther | QFileDevice::ExeOther)) {
        return fixture->lsblkPath;
    }

    fixture->udevDir = QDir(rootDir).filePath(QStringLiteral("udev-data"));
    if (!QDir().mkpath(fixture->udevDir)) {
        return fixture->udevDir;
    }
    const auto writeUdevEntry = [fixture](const QString &deviceNumber, const QString &type,
                                          const QString &uuid, const QString &partUuid) {
        QFile entry(fixture->udevDir + QStringLiteral("/b") + deviceNumber);
        if (!entry.open(QIODevice::WriteOnly | QIODevice::Text)) {
            return false;
        }
        QTextStream stream(&entry);
        stream << "E:ID_FS_TYPE=" << type << '\n';
        stream << "E:ID_FS_USAGE=filesystem\n";
        if (!uuid.isEmpty()) {
            stream << "E:ID_FS_UUID=" << uuid << '\n';
        }
        if (!partUuid.isEmpty()) {
            stream << "E:ID_PART_ENTRY_UUID=" << partUuid << '\n';
        }
        return true;
    };
    if (!writeUdevEntry(QStringLiteral("253:17"), QStringLiteral("ext4"), fixture->bootUuid, QString())) {
        return fixture->udevDir;
    }
    if (!writeUdevEntry(QStringLiteral("253:18"), QStringLiteral("swap"), QString(), QString())) {
        return fixture->udevDir;
    }
    if (!writeUdevEntry(QStringLiteral("253:19"), QStringLiteral("ext4"),
                        fixture->rootUuid, fixture->rootPartUuid)) {
        return fixture->udevDir;
    }
    return QString();
}

// Finds the real top-level row a device scan rendered for diskPath.
QTreeWidgetItem *deviceTreeTopLevelItem(MainWindow &window, const QString &diskPath)
{
    for (int row = 0; row < window.m_deviceTree->topLevelItemCount(); ++row) {
        QTreeWidgetItem *item = window.m_deviceTree->topLevelItem(row);
        if (item && item->data(0, Qt::UserRole).toString() == diskPath) {
            return item;
        }
    }
    return nullptr;
}

// Finds a rendered partition/mapper row by device path. The device tree sorts
// partitions within their drive, so tests must not rely on child order.
QTreeWidgetItem *deviceTreeChildItem(QTreeWidgetItem *parent, const QString &devicePath)
{
    if (!parent) {
        return nullptr;
    }
    for (int row = 0; row < parent->childCount(); ++row) {
        QTreeWidgetItem *item = parent->child(row);
        if (item && item->data(0, Qt::UserRole).toString() == devicePath) {
            return item;
        }
    }
    return nullptr;
}

// Clicks a header section like a user. With sorting enabled the header updates
// its sort indicator and the view applies the new order.
QPoint headerSectionCenter(QHeaderView *header, int section)
{
    return QPoint(header->sectionViewportPosition(section) + header->sectionSize(section) / 2,
                  header->height() / 2);
}

// Selects a rendered device row exactly like a user click and lets the
// selection handler update the details panel.
void selectDeviceTreeItem(MainWindow &window, QTreeWidgetItem *item)
{
    window.m_deviceTree->clearSelection();
    window.m_deviceTree->setCurrentItem(item);
    item->setSelected(true);
    QCoreApplication::processEvents();
}

// Reusable alignment check for a stack of action buttons that share one
// visual column: every member must have the same width and height and share
// its left and right edges when mapped into a common ancestor. The same
// helper is intended for any other button stack in the application.
bool buttonsShareColumn(QWidget *ancestor, const QList<QPushButton *> &buttons,
                        QString *failure = nullptr)
{
    if (!ancestor || buttons.isEmpty() || !buttons.first()) {
        if (failure) {
            *failure = QStringLiteral("no buttons to compare");
        }
        return false;
    }
    const QRect reference(buttons.first()->mapTo(ancestor, QPoint(0, 0)),
                          buttons.first()->size());
    for (QPushButton *button : buttons) {
        if (!button) {
            if (failure) {
                *failure = QStringLiteral("null button in stack");
            }
            return false;
        }
        const QRect rect(button->mapTo(ancestor, QPoint(0, 0)), button->size());
        if (rect.width() != reference.width() || rect.height() != reference.height()
            || rect.left() != reference.left() || rect.right() != reference.right()) {
            if (failure) {
                *failure = QStringLiteral("%1 (%2x%3 at x=%4) is not aligned with %5 (%6x%7 at x=%8)")
                    .arg(button->text())
                    .arg(rect.width()).arg(rect.height()).arg(rect.left())
                    .arg(buttons.first()->text())
                    .arg(reference.width()).arg(reference.height()).arg(reference.left());
            }
            return false;
        }
    }
    return true;
}

// Width available to a button label, matching the reserved icon extent and
// padding used by ElidedPushButton::paintEvent().
int buttonTextWidth(const QPushButton *button)
{
    int available = button->contentsRect().width();
    if (!button->icon().isNull()) {
        const QSize extent = button->iconSize().isValid()
            ? button->iconSize()
            : QSize(button->style()->pixelMetric(QStyle::PM_ButtonIconSize),
                    button->style()->pixelMetric(QStyle::PM_ButtonIconSize));
        available -= extent.width() + 8;
    }
    return qMax(0, available - 8);
}

// The bounded size hint a button needs for the label it actually paints
// (elided). The full content sizeHint may legitimately exceed the allocation
// at the minimum window width; the bounded hint must fit.
QSize boundedButtonSizeHint(const QPushButton *button)
{
    const QFontMetrics metrics(button->font());
    const QString elided = metrics.elidedText(button->text(), Qt::ElideRight,
                                              buttonTextWidth(button));
    int width = metrics.horizontalAdvance(elided) + 8;
    if (!button->icon().isNull()) {
        const QSize extent = button->iconSize().isValid()
            ? button->iconSize()
            : QSize(button->style()->pixelMetric(QStyle::PM_ButtonIconSize),
                    button->style()->pixelMetric(QStyle::PM_ButtonIconSize));
        width += extent.width() + 8;
    }
    return QSize(width, button->sizeHint().height());
}

// The bounded-text convention for runtime-toggling buttons: the painted label
// is elided when it cannot fit, the elided text stays inside the button, the
// bounded size request never exceeds the allocation, and the button remains
// inside its parent layout.
bool buttonTextIsBounded(QPushButton *button, QString *failure = nullptr)
{
    if (!button) {
        if (failure) {
            *failure = QStringLiteral("missing button");
        }
        return false;
    }
    const QString fullText = button->text();
    const QFontMetrics metrics(button->font());
    const int available = buttonTextWidth(button);
    const QString elided = metrics.elidedText(fullText, Qt::ElideRight, available);
    if (elided != fullText && metrics.horizontalAdvance(elided) > available) {
        if (failure) {
            *failure = QStringLiteral("%1 elided text overflows the button").arg(fullText);
        }
        return false;
    }
    if (boundedButtonSizeHint(button).width() > button->width()) {
        if (failure) {
            *failure = QStringLiteral("%1 bounded size hint exceeds the actual width").arg(fullText);
        }
        return false;
    }
    if (button->minimumSizeHint().width() > button->width()) {
        if (failure) {
            *failure = QStringLiteral("%1 minimum size request exceeds the actual width").arg(fullText);
        }
        return false;
    }
    QWidget *parent = button->parentWidget();
    if (!parent) {
        if (failure) {
            *failure = QStringLiteral("%1 has no parent").arg(fullText);
        }
        return false;
    }
    const QRect rect(button->mapTo(parent, QPoint(0, 0)), button->size());
    if (!parent->contentsRect().contains(rect)) {
        if (failure) {
            *failure = QStringLiteral("%1 escapes its parent layout").arg(fullText);
        }
        return false;
    }
    return true;
}

void prepareRepairScope(MainWindow &window, bool host = false)
{
    DeviceNode component;
    component.path = host ? QStringLiteral("/dev/test-host-root") : QStringLiteral("/dev/test-root");
    component.type = QStringLiteral("part");
    component.fileSystem = QStringLiteral("ext4");
    component.linuxCapableFileSystem = true;
    component.installedLinux = true;
    component.protectedDevice = host;
    DeviceNode disk;
    disk.path = host ? QStringLiteral("/dev/test-host") : QStringLiteral("/dev/test-system");
    disk.type = QStringLiteral("disk");
    disk.protectedDevice = host;
    disk.children.append(component);
    window.m_deviceIndex.insert(disk.path, disk);
    window.m_deviceIndex.insert(component.path, component);
    window.m_hostMaintenanceMode = host;
    if (host) {
        window.m_hostPrimaryPath = disk.path;
        window.m_hostPrimaryComponentPath = component.path;
    } else {
        window.m_previewTargetPath = disk.path;
        window.m_previewTargetComponentPath = component.path;
        window.m_targetDiagnosticCacheIdentity = window.currentTargetDiagnosticCacheIdentity();
    }
}

// Enters the explicit running-host maintenance scope the way the real UI does.
// The Diagnostics scope is derived from the Systems state, so setting Host
// Maintenance active is what switches diagnostics to Running Host. Tests that
// exercise host diagnostics must establish this state first; the production
// transitions are exercised separately through the real
// selectHostForMaintenance()/exitHostMaintenanceMode() handlers.
void enterHostDiagnosticScope(MainWindow &window)
{
    window.m_hostMaintenanceMode = true;
    window.updateTargetLabels();
}

// Host Maintenance scope with a Btrfs running-host root, ready for the
// running-host snapshot flow tests.
void prepareHostSnapshotScope(MainWindow &window)
{
    prepareRepairScope(window, true);
    DeviceNode component = window.m_deviceIndex.value(window.m_hostPrimaryComponentPath);
    component.fileSystem = QStringLiteral("btrfs");
    window.m_deviceIndex.insert(component.path, component);
    window.m_hostMaintenanceMode = true;
    window.updateTargetLabels();
}

QTreeWidgetItem *repairItem(MainWindow &window, const QString &key)
{
    for (int row = 0; row < window.m_repairToolTree->topLevelItemCount(); ++row) {
        auto *item = window.m_repairToolTree->topLevelItem(row);
        if (item->data(0, Qt::UserRole).toString() == key) {
            return item;
        }
    }
    return nullptr;
}

// Add and select a top-level drive row so the scope-confirmation handlers can
// read it exactly like a user selection in the device tree.
QTreeWidgetItem *selectTopLevelDisk(MainWindow &window, const QString &diskPath)
{
    window.m_deviceTree->clearSelection();
    auto *item = new QTreeWidgetItem(window.m_deviceTree);
    item->setData(0, Qt::UserRole, diskPath);
    window.m_deviceTree->setCurrentItem(item);
    item->setSelected(true);
    return item;
}

QString capabilityEvidence(bool arch, bool dkms)
{
    return QStringLiteral("Distribution family: %1\n"
                          "Package manager backend: %2\n"
                          "Package manager backends: %2\n"
                          "Service manager: systemd\n"
                          "Initramfs backend: %3\n"
                          "Bootloader backend: grub\n"
                          "Repair tool validate: available\n"
                          "Repair tool dpkg: %4\n"
                          "Repair tool fixbroken: available\n"
                          "Repair tool aptupdate: %5\n"
                          "Repair tool upgrade: available\n"
                          "Repair tool dkms: %6\n"
                          "Repair tool display: unavailable|No display manager installed\n"
                          "Repair tool initramfs: available\n"
                          "Repair tool efi: available\n"
                          "Repair tool grub: available\n"
                          "Repair tool bootstack: available\n")
        .arg(arch ? QStringLiteral("arch") : QStringLiteral("debian"),
             arch ? QStringLiteral("pacman") : QStringLiteral("apt/dpkg"),
             arch ? QStringLiteral("mkinitcpio") : QStringLiteral("initramfs-tools"),
             arch ? QStringLiteral("unavailable|Arch has no dpkg database") : QStringLiteral("available"),
             arch ? QStringLiteral("unavailable|Arch refuses metadata-only transactions") : QStringLiteral("available"),
             dkms ? QStringLiteral("available") : QStringLiteral("unavailable|DKMS is not installed"));
}

// The same capability evidence with the display-manager tool available so the
// scoped-invalidation tests can dispatch the display repair.
QString capabilityEvidenceWithDisplay(bool arch, bool dkms)
{
    QString evidence = capabilityEvidence(arch, dkms);
    evidence.replace(QStringLiteral("Repair tool display: unavailable|No display manager installed"),
                     QStringLiteral("Repair tool display: available"));
    return evidence;
}

// Alpine capability evidence in the shape the helper emits for an apk/OpenRC/
// mkinitfs/extlinux target: the guarded package, display, initramfs, GRUB and
// extlinux stages carry their own available/unavailable line, while dpkg and
// the standalone metadata refresh stay unavailable by policy. dkms, grub and
// extlinux availability follow their installed-state evidence exactly like
// every other tool.
QString capabilityEvidenceAlpine(bool dkms, bool grub, bool extlinux)
{
    return QStringLiteral("Distribution family: alpine\n"
                          "Package manager backend: apk\n"
                          "Package manager backends: apk\n"
                          "Service manager: OpenRC\n"
                          "Display manager backend: OpenRC\n"
                          "Initramfs backend: mkinitfs\n"
                          "Bootloader backend: syslinux/extlinux\n"
                          "Repair tool validate: available\n"
                          "Repair tool filesystem: available\n"
                          "Repair tool dpkg: unavailable|Alpine uses apk; no dpkg configuration step\n"
                          "Repair tool fixbroken: available\n"
                          "Repair tool aptupdate: unavailable|Alpine has no standalone metadata refresh; apk upgrade refreshes the index\n"
                          "Repair tool upgrade: available\n"
                          "Repair tool dkms: %1\n"
                          "Repair tool display: available\n"
                          "Repair tool initramfs: available\n"
                          "Repair tool efi: unavailable|syslinux/extlinux (BIOS) boot detected; no EFI boot path is available\n"
                          "Repair tool grub: %2\n"
                          "Repair tool extlinux: %3\n"
                          "Repair tool bootstack: available\n")
        .arg(dkms ? QStringLiteral("available") : QStringLiteral("unavailable|DKMS is not installed"),
             grub ? QStringLiteral("available") : QStringLiteral("unavailable|grub-mkconfig is not installed"),
             extlinux ? QStringLiteral("available") : QStringLiteral("unavailable|update-extlinux is not installed"));
}

// Alpine UEFI GRUB capability evidence: the same apk/OpenRC/mkinitfs backend
// set with a GRUB UEFI bootloader. The helper reports efi and grub from the
// detected GRUB EFI prerequisites; both follow their capability lines exactly.
QString capabilityEvidenceAlpineUefi(bool efi, bool grub)
{
    return QStringLiteral("Distribution family: alpine\n"
                          "Package manager backend: apk\n"
                          "Package manager backends: apk\n"
                          "Service manager: OpenRC\n"
                          "Display manager backend: OpenRC\n"
                          "Initramfs backend: mkinitfs\n"
                          "Bootloader backend: grub\n"
                          "Repair tool validate: available\n"
                          "Repair tool filesystem: available\n"
                          "Repair tool dpkg: unavailable|Alpine uses apk; no dpkg configuration step\n"
                          "Repair tool fixbroken: available\n"
                          "Repair tool aptupdate: unavailable|Alpine has no standalone metadata refresh; apk upgrade refreshes the index\n"
                          "Repair tool upgrade: available\n"
                          "Repair tool dkms: available\n"
                          "Repair tool display: available\n"
                          "Repair tool initramfs: available\n"
                          "Repair tool efi: %1\n"
                          "Repair tool grub: %2\n"
                          "Repair tool extlinux: available\n"
                          "Repair tool bootstack: available\n")
        .arg(efi ? QStringLiteral("available")
                 : QStringLiteral("unavailable|no EFI System Partition candidate on the selected disk"),
             grub ? QStringLiteral("available")
                  : QStringLiteral("unavailable|Neither grub-mkconfig nor update-grub is installed in the target system"));
}

// Fedora/RPM capability evidence in the shape the helper emits for a
// dnf/dracut/GRUB2/GDM target: the guarded rpm backend reports dnf-backed
// package stages, dracut initramfs, Fedora GRUB2 tooling/paths and the
// systemd GDM path, while dpkg and the BIOS EFI stage stay unavailable with
// probe-based reasons.  The package, initramfs, GRUB and display lines follow
// their own availability flags exactly like every other backend.
QString capabilityEvidenceFedora(bool packages, bool dracut, bool grub2, bool display)
{
    return QStringLiteral(
               "Distribution family: fedora\n"
               "Package manager backend: rpm\n"
               "Package manager backends: rpm\n"
               "Service manager: systemd\n"
               "Display manager backend: systemd\n"
               "Initramfs backend: dracut\n"
               "Initramfs backends: dracut\n"
               "Bootloader backend: grub\n"
               "Logging backend: journald\n"
               "Repair capability evidence display: graphical.target present; display manager unit gdm.service\n"
               "Repair capability evidence grub: grub2-mkconfig present; /boot/grub2/grub.cfg present; grubenv present\n"
               "Repair capability evidence initramfs: initramfs backend: dracut; dracut executable and /usr/lib/dracut present; kernels: 6.19.10-300.fc44.x86_64\n"
               "Repair capability evidence fixbroken: rpm executable present; /usr/lib/sysimage/rpm present; dnf5 present; 3 enabled repositories\n"
               "Repair tool validate: available\n"
               "Repair tool filesystem: available\n"
               "Repair tool dpkg: unavailable|Fedora uses rpm/dnf; dpkg configuration is not available\n"
               "Repair tool fixbroken: %1\n"
               "Repair tool aptupdate: %2\n"
               "Repair tool upgrade: %3\n"
               "Repair tool dkms: unavailable|the rpm DKMS header correction (kernel-devel/akmods) is not implemented\n"
               "Repair tool display: %4\n"
               "Repair tool initramfs: %5\n"
               "Repair tool efi: unavailable|legacy BIOS target; no EFI boot path is available\n"
               "Repair tool grub: %6\n"
               "Repair tool extlinux: unavailable|syslinux/extlinux is not installed in the target system\n"
               "Repair tool bootstack: %7\n")
        .arg(packages ? QStringLiteral("available")
                      : QStringLiteral("unavailable|rpm and dnf5 are not installed in the target system"),
             packages ? QStringLiteral("available")
                      : QStringLiteral("unavailable|dnf5 is not installed in the target system"),
             packages ? QStringLiteral("available")
                      : QStringLiteral("unavailable|dnf5 is not installed in the target system"),
             display ? QStringLiteral("available")
                     : QStringLiteral("unavailable|No display manager was found"),
             dracut ? QStringLiteral("available")
                    : QStringLiteral("unavailable|dracut is not installed in the target system"),
             grub2 ? QStringLiteral("available")
                   : QStringLiteral("unavailable|grub2-mkconfig is not installed in the target"),
             dracut && grub2
                 ? QStringLiteral("available")
                 : QStringLiteral("unavailable|dracut and GRUB2 are required for boot-stack reconciliation"));
}

void cacheRepairEvidence(MainWindow &window, const QString &evidence)
{
    // Injected evidence is fresh by definition: clear the scoped invalidation
    // markers so a test that re-caches does not keep a previous repair's
    // stale bookkeeping.
    window.m_targetDiagnosticsStaleSections.clear();
    window.m_hostDiagnosticsStaleSections.clear();
    auto &cache = window.m_hostMaintenanceMode ? window.m_hostDiagnosticCache : window.m_targetDiagnosticCache;
    cache.clear();
    for (const QString &key : {QStringLiteral("report"), QStringLiteral("environment"),
                              QStringLiteral("kernel"), QStringLiteral("grub"), QStringLiteral("uki"),
                              QStringLiteral("display"), QStringLiteral("boot")}) {
        cache.insert(key, QStringLiteral("Diagnostic: %1\n%2").arg(key, evidence));
    }
}

// Configure a target-scope window for a file-system-only Full Repair plan with
// the filesystem capability available and every other stage unchecked.
void prepareFilesystemPlan(MainWindow &window)
{
    prepareRepairScope(window);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);
    QString evidence = capabilityEvidence(false, true);
    evidence += QStringLiteral("Repair tool filesystem: available\n");
    cacheRepairEvidence(window, evidence);

    for (QCheckBox *toggle : {window.m_fullRepairDpkg, window.m_fullRepairBrokenPackages,
                              window.m_fullRepairAptUpdate, window.m_fullRepairUpgrade,
                              window.m_fullRepairDkms, window.m_fullRepairDisplayManager,
                              window.m_fullRepairInitramfs, window.m_fullRepairEfi,
                              window.m_fullRepairGrub}) {
        if (toggle) {
            toggle->setChecked(false);
        }
    }
    window.m_fullRepairFilesystem->setChecked(true);
    window.updateFullRepairSummary();
}

// Prepare a target-scope window whose cached diagnostics were invalidated but
// not yet regenerated, with the automatic refresh setting enabled.
void prepareStaleTargetScope(MainWindow &window)
{
    prepareRepairScope(window);
    window.m_targetDiagnosticCache.clear();
    window.m_targetDiagnosticTimes.clear();
    window.m_targetDiagnosticCacheIdentity = window.currentTargetDiagnosticCacheIdentity();
    window.m_targetDiagnosticsNeedRegeneration = true;
    window.m_autoRefreshDiagnostics->setChecked(true);
    // These tests spin the event loop with a committed target. Keep the
    // constructor's deferred Btrfs snapshot preload from starting a real
    // privileged helper session; the automatic refresh is exercised through
    // the UI-test diagnostics backend instead.
    window.m_snapshotPreloadScheduled = true;
}

int diagnosticRunCount(MainWindow &window)
{
    return static_cast<int>(window.m_actionLogEntries.join(QLatin1Char('\n'))
                                .count(QStringLiteral("Starting all available read-only diagnostics")));
}

// Application-log refreshes are coalesced through a single-shot timer, so
// tests that assert on the rendered view must flush the pending refresh.
void flushLogRefresh(MainWindow &window)
{
    if (window.m_logRefreshTimer) {
        window.m_logRefreshTimer->stop();
    }
    window.refreshLogView();
}

// Applies a dark application palette for the duration of a test, matching the
// white-on-dark appearance the real KDE app uses, so the glyph colors are
// verified in the theme the user actually sees.
class ScopedDarkPalette
{
public:
    ScopedDarkPalette()
        : m_original(QApplication::palette())
    {
        QPalette palette = m_original;
        palette.setColor(QPalette::Base, QColor(30, 30, 30));
        palette.setColor(QPalette::Window, QColor(30, 30, 30));
        palette.setColor(QPalette::Text, Qt::white);
        palette.setColor(QPalette::WindowText, Qt::white);
        palette.setColor(QPalette::Button, QColor(45, 45, 45));
        palette.setColor(QPalette::ButtonText, Qt::white);
        QApplication::setPalette(palette);
    }
    ~ScopedDarkPalette()
    {
        QApplication::setPalette(m_original);
    }

private:
    QPalette m_original;
};

// Applies a deterministic desktop color scheme through the production test
// seam (the same code path the platform's colorSchemeChanged signal uses) and
// restores the previous application palette on scope exit.
class ScopedColorSchemeOverride
{
public:
    explicit ScopedColorSchemeOverride(Qt::ColorScheme scheme)
        : m_originalPalette(QApplication::palette())
    {
        MainWindow::setColorSchemeOverrideForTests(scheme);
    }

    ~ScopedColorSchemeOverride()
    {
        MainWindow::setColorSchemeOverrideForTests(Qt::ColorScheme::Unknown);
        QApplication::setPalette(m_originalPalette);
    }

private:
    QPalette m_originalPalette;
};

// Foreground color a rendered repair-result glyph carries in the log document.
QColor repairGlyphColor(MainWindow &window, const QString &glyph)
{
    const QTextDocument *document = window.m_logView->document();
    for (QTextBlock block = document->begin(); block.isValid(); block = block.next()) {
        if (!block.text().contains(QStringLiteral("Repair result:"))) {
            continue;
        }
        for (QTextBlock::iterator it = block.begin(); !it.atEnd(); ++it) {
            const QTextFragment fragment = it.fragment();
            if (fragment.text().contains(glyph)) {
                return fragment.charFormat().foreground().color();
            }
        }
    }
    return QColor();
}

// Pixel-level verification of the repair-result glyph colors. The log viewport
// is grabbed with QWidget::grab and every summary/stage glyph's painted
// rectangle is scanned for a pixel of the expected class. This proves the
// rendered output, not just the character formats: a glyph that is painted in
// the normal text color contributes nothing to the returned counts.
QHash<QString, int> coloredGlyphCounts(MainWindow &window)
{
    QHash<QString, int> counts;
    const QImage image = window.m_logView->viewport()->grab().toImage();
    const QStringList glyphs = {
        QStringLiteral("✓"), QStringLiteral("✗"), QStringLiteral("▪")
    };
    const auto matches = [](const QString &glyph, const QColor &pixel) {
        // Text antialiasing blends the glyph with the background, so the
        // target channel only has to dominate the other two.
        if (glyph == QStringLiteral("✓")) {
            return pixel.green() > pixel.red() + 20 && pixel.green() > pixel.blue() + 20;
        }
        if (glyph == QStringLiteral("✗")) {
            return pixel.red() > pixel.green() + 20 && pixel.red() > pixel.blue() + 20;
        }
        return pixel.blue() > pixel.red() + 20 && pixel.blue() > pixel.green() + 20;
    };
    QTextDocument *document = window.m_logView->document();
    for (QTextBlock block = document->begin(); block.isValid(); block = block.next()) {
        if (!block.text().contains(QStringLiteral("Repair result:"))
            && !block.text().contains(QStringLiteral("Full Repair results:"))
            && !block.text().startsWith(QStringLiteral("  ✓"))
            && !block.text().startsWith(QStringLiteral("  ✗"))
            && !block.text().startsWith(QStringLiteral("  ▪"))) {
            continue;
        }
        for (QTextBlock::iterator it = block.begin(); !it.atEnd(); ++it) {
            const QTextFragment fragment = it.fragment();
            for (const QString &glyph : glyphs) {
                const int glyphIndex = fragment.text().indexOf(glyph);
                if (glyphIndex < 0) {
                    continue;
                }
                // cursorRect() reports the caret, not the selection, so the
                // glyph's painted rectangle is the union of the carets on both
                // sides of the glyph.
                const int glyphStart = fragment.position() + glyphIndex;
                QTextCursor startCursor(document);
                startCursor.setPosition(glyphStart);
                QTextCursor endCursor(document);
                endCursor.setPosition(glyphStart + glyph.size());
                const QRect rect = window.m_logView->cursorRect(startCursor)
                                       .united(window.m_logView->cursorRect(endCursor));
                const QRect scan = rect.adjusted(-1, -1, 1, 1).intersected(image.rect());
                bool found = false;
                for (int y = scan.top(); y <= scan.bottom() && !found; ++y) {
                    for (int x = scan.left(); x <= scan.right() && !found; ++x) {
                        if (matches(glyph, image.pixelColor(x, y))) {
                            found = true;
                        }
                    }
                }
                if (found) {
                    counts[glyph] += 1;
                }
            }
        }
    }
    return counts;
}

int diagnosticRow(MainWindow &window, const QString &key)
{
    for (int row = 0; row < window.m_diagnosticList->count(); ++row) {
        if (window.m_diagnosticList->item(row)->data(Qt::UserRole).toString() == key) {
            return row;
        }
    }
    return -1;
}

// Decodes the ARG records of one captured privileged-session request. The
// capture is written by startFakePrivilegedSession().
QStringList capturedHelperArguments(const QString &capturePath)
{
    QFile file(capturePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QStringList();
    }
    const QStringList lines = QString::fromUtf8(file.readAll())
        .split(QLatin1Char('\n'), Qt::SkipEmptyParts);
    QStringList arguments;
    for (const QString &line : lines) {
        const QStringList fields = line.split(QLatin1Char('\t'));
        if (fields.size() >= 3 && fields.at(0) == QStringLiteral("ARG")) {
            arguments.append(QString::fromUtf8(QByteArray::fromBase64(fields.at(2).toLatin1())));
        }
    }
    return arguments;
}

// Decodes every privileged-session request captured by a persistent fake
// session, in request order. Each inner list holds that request's arguments,
// so a retry can be told apart from the original attempt.
QList<QStringList> capturedHelperRequests(const QString &capturePath)
{
    QList<QStringList> requests;
    QFile file(capturePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return requests;
    }
    QStringList current;
    const QStringList lines = QString::fromUtf8(file.readAll())
        .split(QLatin1Char('\n'), Qt::SkipEmptyParts);
    for (const QString &line : lines) {
        const QStringList fields = line.split(QLatin1Char('\t'));
        if (fields.size() >= 3 && fields.at(0) == QStringLiteral("ARG")) {
            current.append(QString::fromUtf8(QByteArray::fromBase64(fields.at(2).toLatin1())));
        } else if (!fields.isEmpty() && fields.at(0) == QStringLiteral("END")) {
            requests.append(current);
            current.clear();
        }
    }
    if (!current.isEmpty()) {
        requests.append(current);
    }
    return requests;
}

int capturedHostRequestCount(const QString &capturePath, const QString &command,
                             const QString &action = QString())
{
    int count = 0;
    for (const QStringList &request : capturedHelperRequests(capturePath)) {
        if (request.isEmpty() || request.value(0) != command) {
            continue;
        }
        if (!action.isEmpty() && request.value(3) != action) {
            continue;
        }
        ++count;
    }
    return count;
}

// Starts a fake privileged-session process for the BOOT_REPAIR_UI_TEST build.
// It records the six protocol records of one request (BEGIN, four ARG, END)
// into capturePath, then emits a canned response:
//   success   - one OUT line followed by DONE with exit code 0
//   aptfail   - a failed-fetch OUT line followed by DONE with exit code 100
//   truncated - one OUT line and an exit with no DONE record at all
//   prompt    - a delayed interactive-prompt abort with the helper's
//               non-interactive hint, followed by DONE with exit code 1
//   aptupgrade - the helper's distribution-disabled apt-upgrade mapping line
//               and a successful full-upgrade retry, followed by DONE 0
// The process is parented to the window so MainWindow owns its lifetime.
QProcess *startFakePrivilegedSession(MainWindow &window, const QString &capturePath,
                                     const QString &mode)
{
    auto *session = new QProcess(&window);
    const QString script = QStringLiteral(
        "capture=\"%1\"\n"
        "head -n 6 > \"$capture\"\n"
        "id=$(head -n 1 \"$capture\" | cut -f2)\n"
        "case \"%2\" in\n"
        "  success) printf 'OUT\\t%s\\tmock host command output\\n' \"$id\"; printf 'DONE\\t%s\\t0\\n' \"$id\"; exit 0 ;;\n"
        "  aptfail) printf 'OUT\\t%s\\tTemporary failure resolving archive.example\\n' \"$id\"; printf 'DONE\\t%s\\t100\\n' \"$id\"; exit 100 ;;\n"
        "  truncated) printf 'OUT\\t%s\\tpartial helper output\\n' \"$id\"; exit 7 ;;\n"
        "  prompt) sleep 0.3; printf 'OUT\\t%s\\tIs this ok [y/N]: \\n' \"$id\"; "
        "printf 'OUT\\t%s\\tBoot Bitch: the command asked an interactive question; re-run it with the non-interactive flag, for example dnf update -y, apt-get -y upgrade or pacman --noconfirm -Syu.\\n' \"$id\"; "
        "printf 'DONE\\t%s\\t1\\n' \"$id\"; exit 1 ;;\n"
        "  aptupgrade) printf 'OUT\\t%s\\t[12:00:00] apt upgrade is disabled by this distribution; running '\\''apt full-upgrade'\\'' instead\\n' \"$id\"; "
        "printf 'OUT\\t%s\\tmock full-upgrade completed\\n' \"$id\"; printf 'DONE\\t%s\\t0\\n' \"$id\"; exit 0 ;;\n"
        "esac\n"
        "exit 9\n")
        .arg(capturePath, mode);
    session->start(QStringLiteral("/bin/sh"), {QStringLiteral("-c"), script});
    if (!session->waitForStarted(5000)) {
        delete session;
        return nullptr;
    }
    window.m_privilegedSession = session;
    window.m_privilegedSessionReady = true;
    return session;
}

// Persistent fake privileged session for the shell release-info-change retry.
// The first request receives the exact apt release-info-change error class and
// exit code 100; every later request succeeds. All requests are recorded so a
// test can assert the retry command and that a non-apt command never retries.
QProcess *startReleaseInfoChangeFakePrivilegedSession(MainWindow &window, const QString &capturePath)
{
    auto *session = new QProcess(&window);
    QString script = QStringLiteral(R"SCRIPT(
capture="$1"
counter=0
while IFS= read -r line; do
  tag="${line%%$'\t'*}"
  [ "$tag" = "BEGIN" ] || continue
  rest="${line#*$'\t'}"
  id="${rest%%$'\t'*}"
  rest="${rest#*$'\t'}"
  count="${rest%%$'\t'*}"
  printf '%s\n' "$line" >> "$capture"
  i=0
  while [ "$i" -lt "$count" ]; do
    IFS= read -r argline
    printf '%s\n' "$argline" >> "$capture"
    i=$((i+1))
  done
  IFS= read -r endline
  printf '%s\n' "$endline" >> "$capture"
  counter=$((counter+1))
  if [ "$counter" -eq 1 ]; then
    repo="https://txos.tuxedocomputers.com/debian-cache testing InRelease"
    printf "OUT\t%s\tE: Repository '%s' changed its 'Origin' value from 'TUXEDO Computers' to 'TUXEDO'\n" "$id" "$repo"
    printf "OUT\t%s\tE: Repository '%s' changed its 'Label' value from 'tuxedoos-testing' to 'tuxedoos-cache-testing'\n" "$id" "$repo"
    printf 'DONE\t%s\t100\n' "$id"
  else
    printf 'OUT\t%s\tmock apt update completed after accepting the metadata change\n' "$id"
    printf 'DONE\t%s\t0\n' "$id"
  fi
done
)SCRIPT");
    session->start(QStringLiteral("/bin/bash"),
                   {QStringLiteral("-c"), script, QStringLiteral("fake-release-info-session"), capturePath});
    if (!session->waitForStarted(5000)) {
        delete session;
        return nullptr;
    }
    window.m_privilegedSession = session;
    window.m_privilegedSessionReady = true;
    return session;
}

// Fake privileged-session process for the file system repair flow. It reads
// every BEGIN/ARG/END request, records the raw protocol lines like the helper
// broker does, and answers fs-inspect with one issue device and fs-repair with
// a successful result so the inspect -> confirm -> repair path can run
// headless. The process keeps serving requests until the window is destroyed.
QProcess *startFilesystemFakePrivilegedSession(MainWindow &window, const QString &capturePath,
                                               const QString &fstype = QStringLiteral("ext4"),
                                               bool issue = true,
                                               const QString &repairStatus = QString(),
                                               const QString &inspectResult = QString(),
                                               const QString &mount = QStringLiteral("unmounted"))
{
    auto *session = new QProcess(&window);
    QString script = QStringLiteral(R"SCRIPT(
capture="$1"
while IFS= read -r line; do
  tag="${line%%$'\t'*}"
  [ "$tag" = "BEGIN" ] || continue
  rest="${line#*$'\t'}"
  id="${rest%%$'\t'*}"
  rest="${rest#*$'\t'}"
  count="${rest%%$'\t'*}"
  printf '%s\n' "$line" >> "$capture"
  args=""
  i=0
  while [ "$i" -lt "$count" ]; do
    IFS= read -r argline
    printf '%s\n' "$argline" >> "$capture"
    payload="${argline##*$'\t'}"
    args="$args $(printf '%s' "$payload" | base64 -d)"
    i=$((i+1))
  done
  IFS= read -r endline
  printf '%s\n' "$endline" >> "$capture"
  case "$args" in
    *fs-inspect*)
      printf 'OUT\t%s\tFile system check /dev/test-root: @FSTYPE@ uuid=11111111-2222-3333-4444-555555555555 mount=@MOUNT@ tool=@TOOL@ result=@RESULT@\n' "$id"
      printf 'OUT\t%s\tFile system check detail /dev/test-root: tool=@TOOL@ output=@DETAIL@\n' "$id"
      printf 'OUT\t%s\tFile system check /dev/test-efi: vfat uuid=ABCD-1234 mount=unmounted tool=fsck.fat result=clean\n' "$id"
      printf 'DONE\t%s\t0\n' "$id"
      ;;
    *fs-repair*)
      printf 'OUT\t%s\tFile system repair /dev/test-root: @FSTYPE@ uuid=11111111-2222-3333-4444-555555555555 mount=unmounted tool=@TOOL@ mode=repair result=clean\n' "$id"
      @REPAIR_STATUS_LINE@
      printf 'DONE\t%s\t0\n' "$id"
      ;;
    *)
      printf 'DONE\t%s\t0\n' "$id"
      ;;
  esac
done
)SCRIPT");
    const QString tool = fstype.compare(QStringLiteral("btrfs"), Qt::CaseInsensitive) == 0
        ? QStringLiteral("btrfs")
        : QStringLiteral("e2fsck");
    script.replace(QStringLiteral("@FSTYPE@"), fstype);
    script.replace(QStringLiteral("@TOOL@"), tool);
    script.replace(QStringLiteral("@RESULT@"), inspectResult.isEmpty()
        ? (issue ? QStringLiteral("issues") : QStringLiteral("skipped"))
        : inspectResult);
    script.replace(QStringLiteral("@MOUNT@"), mount);
    script.replace(QStringLiteral("@DETAIL@"), issue
        ? QStringLiteral("Filesystem is not clean; errors found and the superblock needs attention")
        : QStringLiteral("not run: /dev/test-root is mounted; this check tool is offline-only"));
    script.replace(QStringLiteral("@REPAIR_STATUS_LINE@"), repairStatus.isEmpty()
        ? QString()
        : QStringLiteral("      printf 'OUT\\t%s\\tRepair change status filesystem: %1\\n' \"$id\"\n")
              .arg(repairStatus));
    session->start(QStringLiteral("/bin/bash"),
                   {QStringLiteral("-c"), script, QStringLiteral("fake-fs-session"), capturePath});
    if (!session->waitForStarted(5000)) {
        delete session;
        return nullptr;
    }
    window.m_privilegedSession = session;
    window.m_privilegedSessionReady = true;
    return session;
}

// Persistent fake privileged session for multi-device file system repairs. The
// read-only inspection reports issues on two unmounted ext4 devices; every
// fs-repair request is answered for the device named in the request. The
// inspection response is delayed by inspectDelay (seconds, "0" for none) so a
// test can act while the flow is still in flight, and a non-zero repairExit
// makes the first repair request fail and end the session like a real helper
// failure. All requests are recorded so a blocked second attempt can be
// distinguished from an accepted one.
QProcess *startMultiDeviceFilesystemFakePrivilegedSession(MainWindow &window,
                                                          const QString &capturePath,
                                                          const QString &inspectDelay = QStringLiteral("0"),
                                                          int repairExit = 0)
{
    auto *session = new QProcess(&window);
    QString script = QStringLiteral(R"SCRIPT(
capture="$1"
inspect_delay="$2"
repair_exit="$3"
while IFS= read -r line; do
  tag="${line%%$'\t'*}"
  [ "$tag" = "BEGIN" ] || continue
  rest="${line#*$'\t'}"
  id="${rest%%$'\t'*}"
  rest="${rest#*$'\t'}"
  count="${rest%%$'\t'*}"
  printf '%s\n' "$line" >> "$capture"
  args=""
  i=0
  while [ "$i" -lt "$count" ]; do
    IFS= read -r argline
    printf '%s\n' "$argline" >> "$capture"
    payload="${argline##*$'\t'}"
    args="$args $(printf '%s' "$payload" | base64 -d)"
    i=$((i+1))
  done
  IFS= read -r endline
  printf '%s\n' "$endline" >> "$capture"
  case "$args" in
    *fs-inspect*)
      [ "$inspect_delay" = "0" ] || sleep "$inspect_delay"
      printf 'OUT\t%s\tFile system check /dev/test-root: ext4 uuid=11111111-2222-3333-4444-555555555555 mount=unmounted tool=e2fsck result=issues\n' "$id"
      printf 'OUT\t%s\tFile system check detail /dev/test-root: tool=e2fsck output=Filesystem is not clean; errors found\n' "$id"
      printf 'OUT\t%s\tFile system check /dev/test-data: ext4 uuid=66666666-7777-8888-9999-000000000000 mount=unmounted tool=e2fsck result=issues\n' "$id"
      printf 'OUT\t%s\tFile system check detail /dev/test-data: tool=e2fsck output=Filesystem is not clean; errors found\n' "$id"
      printf 'DONE\t%s\t0\n' "$id"
      ;;
    *fs-repair*)
      case "$args" in
        */dev/test-data*) dev=/dev/test-data ;;
        *) dev=/dev/test-root ;;
      esac
      printf 'OUT\t%s\tFile system repair %s: ext4 uuid=11111111-2222-3333-4444-555555555555 mount=unmounted tool=e2fsck mode=repair result=clean\n' "$id" "$dev"
      printf 'OUT\t%s\tRepair change status filesystem: changed\n' "$id"
      printf 'DONE\t%s\t%s\n' "$id" "$repair_exit"
      [ "$repair_exit" = "0" ] || exit "$repair_exit"
      ;;
    *)
      printf 'DONE\t%s\t0\n' "$id"
      ;;
  esac
done
)SCRIPT");
    session->start(QStringLiteral("/bin/bash"),
                   {QStringLiteral("-c"), script, QStringLiteral("fake-multi-fs-session"),
                    capturePath, inspectDelay, QString::number(repairExit)});
    if (!session->waitForStarted(5000)) {
        delete session;
        return nullptr;
    }
    window.m_privilegedSession = session;
    window.m_privilegedSessionReady = true;
    return session;
}

// Persistent fake privileged session for the rapid scope-switching and
// safety-timeout regressions. It records every request like the other fakes
// and answers a snapshot inventory request only after delaySeconds, so a test
// can switch the active scope (or wedge the helper) while the read-only
// inventory request still owns the privileged gate.
QProcess *startSlowSnapshotFakePrivilegedSession(MainWindow &window, const QString &capturePath,
                                                 const QString &delaySeconds)
{
    auto *session = new QProcess(&window);
    QString script = QStringLiteral(R"SCRIPT(
capture="$1"
delay="$2"
while IFS= read -r line; do
  tag="${line%%$'\t'*}"
  [ "$tag" = "BEGIN" ] || continue
  rest="${line#*$'\t'}"
  id="${rest%%$'\t'*}"
  rest="${rest#*$'\t'}"
  count="${rest%%$'\t'*}"
  printf '%s\n' "$line" >> "$capture"
  args=""
  i=0
  while [ "$i" -lt "$count" ]; do
    IFS= read -r argline
    printf '%s\n' "$argline" >> "$capture"
    payload="${argline##*$'\t'}"
    args="$args $(printf '%s' "$payload" | base64 -d)"
    i=$((i+1))
  done
  IFS= read -r endline
  printf '%s\n' "$endline" >> "$capture"
  case "$args" in
    *snapshots*)
      sleep "$delay"
      printf 'OUT\t%s\tSNAPSHOT\t1\tMQ==\tMjAyNi0wOS0yMCAxMjowMDowMA==\tc2luZ2xl\tdGVzdCBzbmFwc2hvdA==\tTGludXggcm9vdCBzbmFwc2hvdA==\tQC8uc25hcHNob3RzLzEvc25hcHNob3Q=\n' "$id"
      printf 'DONE\t%s\t0\n' "$id"
      ;;
    *)
      printf 'DONE\t%s\t0\n' "$id"
      ;;
  esac
done
)SCRIPT");
    session->start(QStringLiteral("/bin/bash"),
                   {QStringLiteral("-c"), script, QStringLiteral("fake-slow-snapshot-session"),
                    capturePath, delaySeconds});
    if (!session->waitForStarted(5000)) {
        delete session;
        return nullptr;
    }
    window.m_privilegedSession = session;
    window.m_privilegedSessionReady = true;
    return session;
}

// Persistent fake privileged session for the running-host snapshot flow. It
// records every request like the other fakes and answers host-snapshots
// list/inspect/plan/rollback and host-reboot with the helper's stable evidence.
// rollbackMode "fail" answers the rollback with a failed auto-restore;
// rebootMode "fail" answers host-reboot with a refusal.
QProcess *startHostSnapshotFakePrivilegedSession(MainWindow &window, const QString &capturePath,
                                                 const QString &rollbackMode = QStringLiteral("success"),
                                                 const QString &rebootMode = QStringLiteral("success"))
{
    auto *session = new QProcess(&window);
    QString script = QStringLiteral(R"SCRIPT(
capture="$1"
rollback_mode="$2"
reboot_mode="$3"
while IFS= read -r line; do
  tag="${line%%$'\t'*}"
  [ "$tag" = "BEGIN" ] || continue
  rest="${line#*$'\t'}"
  id="${rest%%$'\t'*}"
  rest="${rest#*$'\t'}"
  count="${rest%%$'\t'*}"
  printf '%s\n' "$line" >> "$capture"
  args=""
  i=0
  while [ "$i" -lt "$count" ]; do
    IFS= read -r argline
    printf '%s\n' "$argline" >> "$capture"
    payload="${argline##*$'\t'}"
    args="$args $(printf '%s' "$payload" | base64 -d)"
    i=$((i+1))
  done
  IFS= read -r endline
  printf '%s\n' "$endline" >> "$capture"
  case "$args" in
    *host-snapshots*list*)
      printf 'OUT\t%s\tHost snapshot inventory: 1 root snapshot(s)\n' "$id"
      printf 'OUT\t%s\tSNAPSHOT\t7\t@CREATED@\t@TYPE@\t@DESC@\t@STATUS@\t@REL@\n' "$id"
      printf 'DONE\t%s\t0\n' "$id"
      ;;
    *host-snapshots*inspect*)
      printf 'OUT\t%s\tInspection is read-only. No snapshot, subvolume, boot file or package state was modified.\n' "$id"
      printf 'DONE\t%s\t0\n' "$id"
      ;;
    *snapshots*list*)
      printf 'OUT\t%s\tSNAPSHOT\t9\t@CREATED@\t@TYPE@\t@DESC@\t@STATUS@\t@REL@\n' "$id"
      printf 'DONE\t%s\t0\n' "$id"
      ;;
    *host-snapshots*plan*)
      printf 'OUT\t%s\tRUNNING-HOST BTRFS ROLLBACK PLAN\n' "$id"
      printf 'OUT\t%s\tPLAN_OK=1\n' "$id"
      printf 'DONE\t%s\t0\n' "$id"
      ;;
    *host-snapshots*rollback*)
      if [ "$rollback_mode" = "fail" ]; then
        printf 'OUT\t%s\tHOST ROLLBACK RECOVERY: restoring the preserved pre-rollback @.\n' "$id"
        printf 'OUT\t%s\tHOST_ROLLBACK_RECOVERY=ok\n' "$id"
        printf 'DONE\t%s\t1\n' "$id"
      else
        printf 'OUT\t%s\tHOST_ROLLBACK_BACKUP_ROOT=@rollback-before-20260920-120000\n' "$id"
        printf 'OUT\t%s\tHOST_ROLLBACK_REBOOT_REQUIRED=1\n' "$id"
        printf 'OUT\t%s\tHOST_ROLLBACK_RESULT=SUCCESS\n' "$id"
        printf 'OUT\t%s\tRepair change status snapshots: changed\n' "$id"
        printf 'DONE\t%s\t0\n' "$id"
      fi
      ;;
    *host-reboot*)
      if [ "$reboot_mode" = "fail" ]; then
        printf 'OUT\t%s\tERROR: systemd refused the running-host reboot request (inhibitor or policy).\n' "$id"
        printf 'DONE\t%s\t1\n' "$id"
      else
        printf 'OUT\t%s\tHOST_REBOOT_SCHEDULED=1\n' "$id"
        printf 'DONE\t%s\t0\n' "$id"
      fi
      ;;
    *)
      printf 'DONE\t%s\t0\n' "$id"
      ;;
  esac
done
)SCRIPT");
    auto encode = [](const QString &text) {
        return QString::fromLatin1(text.toUtf8().toBase64());
    };
    script.replace(QStringLiteral("@CREATED@"), encode(QStringLiteral("2026-09-20 12:00:00")));
    script.replace(QStringLiteral("@TYPE@"), encode(QStringLiteral("single")));
    script.replace(QStringLiteral("@DESC@"), encode(QStringLiteral("host test snapshot")));
    script.replace(QStringLiteral("@STATUS@"), encode(QStringLiteral("Linux root snapshot; ro=true")));
    script.replace(QStringLiteral("@REL@"), encode(QStringLiteral("@.snapshots/7/snapshot")));
    session->start(QStringLiteral("/bin/bash"),
                   {QStringLiteral("-c"), script, QStringLiteral("fake-host-snapshot-session"),
                    capturePath, rollbackMode, rebootMode});
    if (!session->waitForStarted(5000)) {
        delete session;
        return nullptr;
    }
    window.m_privilegedSession = session;
    window.m_privilegedSessionReady = true;
    return session;
}

// Builds one eligible Btrfs repair-target disk (and its root component) in the
// synthetic device index used by the scope-switching regressions.
void installSyntheticBtrfsTarget(MainWindow &window, const QString &diskPath, const QString &rootPath)
{
    DeviceNode root;
    root.path = rootPath;
    root.type = QStringLiteral("part");
    root.fileSystem = QStringLiteral("btrfs");
    root.linuxCapableFileSystem = true;
    root.installedLinux = true;
    root.sizeBytes = 16ll * 1024 * 1024 * 1024;
    DeviceNode disk;
    disk.path = diskPath;
    disk.type = QStringLiteral("disk");
    disk.sizeBytes = 20ll * 1024 * 1024 * 1024;
    disk.children.append(root);
    window.m_deviceIndex.insert(disk.path, disk);
    window.m_deviceIndex.insert(root.path, root);
}

// Persistent fake privileged session for repair-section tests. It serves every
// BEGIN/ARG/END request until the window is destroyed and answers each one with
// a counter-tagged OUT line, so a re-run's replacement can be told apart from
// the previous run's captured output. With constantOutput the same text is
// returned every time, which forces scope identity (not output text) to keep
// two blocks apart.
QProcess *startRepairFakePrivilegedSession(MainWindow &window, const QString &capturePath,
                                           bool constantOutput = false)
{
    auto *session = new QProcess(&window);
    QString script = QStringLiteral(R"SCRIPT(
capture="$1"
counter=0
while IFS= read -r line; do
  tag="${line%%$'\t'*}"
  [ "$tag" = "BEGIN" ] || continue
  rest="${line#*$'\t'}"
  id="${rest%%$'\t'*}"
  rest="${rest#*$'\t'}"
  count="${rest%%$'\t'*}"
  printf '%s\n' "$line" >> "$capture"
  i=0
  while [ "$i" -lt "$count" ]; do
    IFS= read -r argline
    printf '%s\n' "$argline" >> "$capture"
    i=$((i+1))
  done
  IFS= read -r endline
  printf '%s\n' "$endline" >> "$capture"
  counter=$((counter+1))
  if [ "%2" = "constant" ]; then
    printf 'OUT\t%s\tCONST_REPAIR_OUTPUT\n' "$id"
  else
    printf 'OUT\t%s\tREPAIR_RUN_%s captured output\n' "$id" "$counter"
  fi
  printf 'DONE\t%s\t0\n' "$id"
done
)SCRIPT");
    script.replace(QStringLiteral("%2"), constantOutput ? QStringLiteral("constant")
                                                       : QStringLiteral("counter"));
    session->start(QStringLiteral("/bin/bash"),
                   {QStringLiteral("-c"), script, QStringLiteral("fake-repair-session"), capturePath});
    if (!session->waitForStarted(5000)) {
        delete session;
        return nullptr;
    }
    window.m_privilegedSession = session;
    window.m_privilegedSessionReady = true;
    return session;
}

// Persistent fake privileged session that answers every request with the
// helper's stable evidence-driven invalidation line. Each plan entry is
// "<helper-stage>=<state>"; a stage missing from the plan gets no status line
// (which the GUI must treat as changed/fail safe). Stage names are mapped to
// capability keys exactly like the helper does. A non-zero exitCode makes the
// session report DONE with that code and stop, like a failed helper run.
QProcess *startChangeStatusFakePrivilegedSession(MainWindow &window, const QString &capturePath,
                                                 const QStringList &plan, int exitCode = 0,
                                                 const QString &failedStage = QString(),
                                                 const QString &failureDetail = QString())
{
    auto *session = new QProcess(&window);
    QString script = QStringLiteral(R"SCRIPT(
capture="@CAPTURE@"
plan="@PLAN@"
failed_stage="@FAILED_STAGE@"
failure_detail="@FAILURE_DETAIL@"
while IFS= read -r line; do
  tag="${line%%$'\t'*}"
  [ "$tag" = "BEGIN" ] || continue
  rest="${line#*$'\t'}"
  id="${rest%%$'\t'*}"
  rest="${rest#*$'\t'}"
  count="${rest%%$'\t'*}"
  printf '%s\n' "$line" >> "$capture"
  args=""
  i=0
  while [ "$i" -lt "$count" ]; do
    IFS= read -r argline
    printf '%s\n' "$argline" >> "$capture"
    payload="${argline##*$'\t'}"
    args="$args $(printf '%s' "$payload" | base64 -d)"
    i=$((i+1))
  done
  IFS= read -r endline
  printf '%s\n' "$endline" >> "$capture"
  printf 'OUT\t%s\tREPAIR_OUTPUT captured\n' "$id"
  while IFS= read -r entry; do
    [ -n "$entry" ] || continue
    stage="${entry%%=*}"
    state="${entry#*=}"
    case " $args " in
      *" $stage "*) ;;
      *) continue ;;
    esac
    key="$stage"
    case "$stage" in
      dpkg-configure) key=dpkg ;;
      fix-broken) key=fixbroken ;;
      apt-update) key=aptupdate ;;
      apt-upgrade) key=upgrade ;;
      display-manager) key=display ;;
      boot-stack) key=bootstack ;;
      fs-repair|host-fs-repair) key=filesystem ;;
    esac
    printf 'OUT\t%s\tRepair change status %s: %s\n' "$id" "$key" "$state"
  done <<< "$plan"
  if [ -n "$failed_stage" ]; then
    printf 'OUT\t%s\t[00:00:00] ERROR: stage %s failed: %s\n' "$id" "'$failed_stage'" "$failure_detail"
  fi
  printf 'DONE\t%s\t@EXIT@\n' "$id"
  [ "@EXIT@" = "0" ] || exit @EXIT@
done
)SCRIPT");
    script.replace(QStringLiteral("@CAPTURE@"), capturePath);
    // One entry per line so a proven-unchanged reason may contain spaces.
    script.replace(QStringLiteral("@PLAN@"), plan.join(QLatin1Char('\n')));
    script.replace(QStringLiteral("@EXIT@"), QString::number(exitCode));
    script.replace(QStringLiteral("@FAILED_STAGE@"), failedStage);
    script.replace(QStringLiteral("@FAILURE_DETAIL@"), failureDetail);
    session->start(QStringLiteral("/bin/bash"),
                   {QStringLiteral("-c"), script, QStringLiteral("fake-change-status-session")});
    if (!session->waitForStarted(5000)) {
        delete session;
        return nullptr;
    }
    window.m_privilegedSession = session;
    window.m_privilegedSessionReady = true;
    return session;
}

// The privileged repair path shows a modal progress dialog whose Close button
// only becomes usable once the helper's DONE record has been consumed. The
// offscreen tests therefore dismiss the dialog as soon as it reports that the
// request completed, so a repair cycle can run without user interaction.
void closeRepairProgressDialogWhenDone(QObject *context)
{
    QTimer *poll = new QTimer(context);
    poll->setInterval(5);
    QObject::connect(poll, &QTimer::timeout, context, [poll] {
        for (QWidget *top : QApplication::topLevelWidgets()) {
            auto *dialog = qobject_cast<QDialog *>(top);
            if (!dialog || !dialog->isVisible()) {
                continue;
            }
            auto *buttons = dialog->findChild<QDialogButtonBox *>();
            if (!buttons) {
                continue;
            }
            QPushButton *close = buttons->button(QDialogButtonBox::Close);
            if (!close || !close->isEnabled()) {
                continue;
            }
            poll->stop();
            dialog->reject();
            poll->deleteLater();
            return;
        }
    });
    poll->start();
}

// Counts framed diagnostic sections in the live register. The "Title:" line
// distinguishes a real section from the per-key text embedded inside a Run
// All report.
int diagnosticSectionCount(MainWindow &window, const QString &key)
{
    const QString marker = QStringLiteral("\nDiagnostic: %1\nTitle: ").arg(key);
    int count = 0;
    for (const QString &entry : window.m_actionLogEntries) {
        if (entry.contains(marker)) {
            ++count;
        }
    }
    return count;
}

int diagnosticSectionCountInText(const QString &text, const QString &key)
{
    const QString marker = QStringLiteral("\nDiagnostic: %1\nTitle: ").arg(key);
    int count = 0;
    int position = text.indexOf(marker);
    while (position >= 0) {
        ++count;
        position = text.indexOf(marker, position + 1);
    }
    return count;
}

// Counts captured repair output blocks in the live register by their framing
// line. A replaced block contributes one; a re-run that appended a second copy
// would contribute two.
int repairOutputCount(MainWindow &window, const QString &title)
{
    const QString marker = QStringLiteral("Repair output\nDiagnostic: %1\n").arg(title);
    int count = 0;
    for (const QString &entry : window.m_actionLogEntries) {
        if (entry.contains(marker)) {
            ++count;
        }
    }
    return count;
}

// Counts live-register entries containing the given status text.
int liveEntryCount(MainWindow &window, const QString &message)
{
    int count = 0;
    for (const QString &entry : window.m_actionLogEntries) {
        if (entry.contains(message)) {
            ++count;
        }
    }
    return count;
}

// Returns the live-register entry containing the given status text, or an
// empty string when it is absent.
QString liveEntryText(MainWindow &window, const QString &message)
{
    for (const QString &entry : window.m_actionLogEntries) {
        if (entry.contains(message)) {
            return entry;
        }
    }
    return QString();
}

// One per-key section exactly as the helper embeds it in a combined Run All
// report: a title frame, the "Diagnostic:" header with its metadata, then the
// captured body.
QString embeddedReportSection(const QString &key, const QString &title,
                              const QString &scope, const QString &body)
{
    return QStringLiteral(
        "========================================\n"
        "%1\n"
        "========================================\n"
        "Diagnostic: %2\n"
        "Scope: %3\n"
        "Time: 2026-01-01T00:00:00+00:00\n"
        "\n"
        "%4")
        .arg(title, key, scope, body);
}

// A combined Run All report as appendDiagnosticLog() records it: one framed
// "Diagnostic: report" entry whose body embeds the captured sections.
QString combinedReportEntry(const QString &scope, const QStringList &sections)
{
    return QStringLiteral(
        "========================================\n"
        "Diagnostic: report\n"
        "Title: Full diagnostic report\n"
        "Scope: %1\n"
        "What was actually run: boot-repair-helper diagnose /dev/test /dev/test-root all\n"
        "Status: completed\n"
        "========================================\n"
        "%2")
        .arg(scope, sections.join(QLatin1Char('\n')));
}

// The scheduler's coalescing delay is 1200 ms; tests that observe a real
// automatic run shorten it through MainWindow::m_evidenceRefreshDelayMs and
// wait long enough that a second (unwanted) run would have fired.
constexpr int kEvidenceRefreshTestDelayMs = 50;
constexpr int kNoSecondRunWaitMs = 250;

// Route every test window's session files into a throwaway directory so real
// user logs under AppDataLocation are never touched.
class ScopedSessionLogDir
{
public:
    ScopedSessionLogDir()
        : m_previous(qgetenv("BOOT_REPAIR_LOG_DIR"))
    {
        qputenv("BOOT_REPAIR_LOG_DIR", m_dir.path().toUtf8());
    }
    ~ScopedSessionLogDir()
    {
        qputenv("BOOT_REPAIR_LOG_DIR", m_previous);
    }

    bool isValid() const
    {
        return m_dir.isValid();
    }

    QString path() const
    {
        return m_dir.path();
    }

private:
    QTemporaryDir m_dir;
    QByteArray m_previous;
};

QStringList sessionLogFiles(const QString &directory)
{
    const QDir dir(directory);
    QStringList paths;
    for (const QString &name : dir.entryList({QStringLiteral("session-*.log")}, QDir::Files, QDir::Name)) {
        paths.append(dir.absoluteFilePath(name));
    }
    return paths;
}

QString readSessionFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }
    return QString::fromUtf8(file.readAll());
}

void writeSessionLog(const QString &path, const QString &scopeLine, const QString &marker)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        return;
    }
    QTextStream stream(&file);
    stream << "──────── SCOPE ────────\n"
           << scopeLine << "\n"
           << "──────── APPLICATION ────────\n"
           << "[2020-01-01 00:00:01] [INFO] " << marker << "\n";
}

void acceptNextMessageBox(QObject *context, QMessageBox::StandardButton button)
{
    QTimer *poll = new QTimer(context);
    poll->setInterval(5);
    QObject::connect(poll, &QTimer::timeout, context, [poll, button] {
        for (QWidget *top : QApplication::topLevelWidgets()) {
            auto *box = qobject_cast<QMessageBox *>(top);
            if (!box) {
                continue;
            }
            poll->stop();
            if (QAbstractButton *target = box->button(button)) {
                target->click();
            } else {
                box->done(button);
            }
            poll->deleteLater();
            return;
        }
    });
    poll->start();
}

// Dismisses a sequence of modal message boxes with the same standard button.
// Each visible box is clicked once; the visibility check keeps the first click
// from being counted twice while the box is still closing.
void acceptNextMessageBoxes(QObject *context, int count, QMessageBox::StandardButton button)
{
    QTimer *poll = new QTimer(context);
    poll->setInterval(5);
    QObject::connect(poll, &QTimer::timeout, context, [poll, count, button]() mutable {
        for (QWidget *top : QApplication::topLevelWidgets()) {
            auto *box = qobject_cast<QMessageBox *>(top);
            if (!box || !box->isVisible()) {
                continue;
            }
            if (QAbstractButton *target = box->button(button)) {
                target->click();
            } else {
                box->done(button);
            }
            if (--count <= 0) {
                poll->stop();
                poll->deleteLater();
            }
            return;
        }
    });
    poll->start();
}

// Records the next visible modal message box and dismisses it with the given
// standard button. The test owns the instance and declares it after the
// window, so the poller is stopped before the window is destroyed. Values are
// copied before the click and stay readable after the synchronous operation
// returns; a test that expects no prompt calls stop() so no poller leaks into
// the next operation.
class NextMessageBoxCapture
{
public:
    NextMessageBoxCapture(QObject *context, QMessageBox::StandardButton button)
        : m_button(button)
    {
        m_timer = new QTimer(context);
        m_timer->setInterval(5);
        QObject::connect(m_timer, &QTimer::timeout, context, [this] {
            for (QWidget *top : QApplication::topLevelWidgets()) {
                auto *box = qobject_cast<QMessageBox *>(top);
                if (!box || !box->isVisible()) {
                    continue;
                }
                appeared = true;
                title = box->windowTitle();
                text = box->text();
                informativeText = box->informativeText();
                m_timer->stop();
                if (QAbstractButton *target = box->button(m_button)) {
                    target->click();
                } else {
                    box->done(m_button);
                }
                return;
            }
        });
        m_timer->start();
    }

    ~NextMessageBoxCapture()
    {
        stop();
    }

    NextMessageBoxCapture(const NextMessageBoxCapture &) = delete;
    NextMessageBoxCapture &operator=(const NextMessageBoxCapture &) = delete;

    void stop()
    {
        if (m_timer) {
            m_timer->stop();
        }
    }

    bool appeared = false;
    QString title;
    QString text;
    QString informativeText;

private:
    QMessageBox::StandardButton m_button;
    QTimer *m_timer = nullptr;
};

// One synchronous failed repair cycle shows two different message boxes: the
// confirmation (Yes) before the privileged request and the failure notice (Ok)
// afterwards. This poll clicks the first visible Yes, keeps running, and stops
// after the first visible Ok.
void acceptConfirmationThenFailure(QObject *context)
{
    QTimer *poll = new QTimer(context);
    poll->setInterval(5);
    QObject::connect(poll, &QTimer::timeout, context, [poll] {
        for (QWidget *top : QApplication::topLevelWidgets()) {
            auto *box = qobject_cast<QMessageBox *>(top);
            if (!box || !box->isVisible()) {
                continue;
            }
            if (QAbstractButton *yes = box->button(QMessageBox::Yes)) {
                yes->click();
                return;
            }
            if (QAbstractButton *ok = box->button(QMessageBox::Ok)) {
                ok->click();
                poll->stop();
                poll->deleteLater();
                return;
            }
            box->accept();
            return;
        }
    });
    poll->start();
}

void acceptNextInputDialog(QObject *context, const QString &text)
{
    QTimer *poll = new QTimer(context);
    poll->setInterval(5);
    QObject::connect(poll, &QTimer::timeout, context, [poll, text] {
        for (QWidget *top : QApplication::topLevelWidgets()) {
            auto *dialog = qobject_cast<QInputDialog *>(top);
            if (!dialog) {
                continue;
            }
            poll->stop();
            dialog->setTextValue(text);
            dialog->accept();
            poll->deleteLater();
            return;
        }
    });
    poll->start();
}

// Clicks the named button of the next visible QMessageBox that carries it.
// Used for the custom Reboot Now / Later buttons of the host rollback dialogs.
void clickMessageBoxButtonWhenVisible(QObject *context, const QString &buttonText)
{
    QTimer *poll = new QTimer(context);
    poll->setInterval(5);
    QObject::connect(poll, &QTimer::timeout, context, [poll, buttonText] {
        for (QWidget *top : QApplication::topLevelWidgets()) {
            auto *box = qobject_cast<QMessageBox *>(top);
            if (!box || !box->isVisible()) {
                continue;
            }
            for (QAbstractButton *button : box->buttons()) {
                if (button->text() == buttonText) {
                    poll->stop();
                    button->click();
                    poll->deleteLater();
                    return;
                }
            }
        }
    });
    poll->start();
}
}

class MainWindowUiTest final : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void tabsAndReadOnlyControls();
    void chrootShellControlsAreGuardedAndReadOnly();
    void hostDiagnosticAndActionRegister();
    void diagnosticOutputIsMirroredToMainLog();
    void narrowRepairAndFileCopyScrollAreasStayStable();
    void fullRepairButtonsDoNotOverlapAtMinimumWidth();
    void fullRepairPlanListUsesSplitterAdjustableHeight();
    void repairSplitterDefaultsFavorToolListWidth();
    void applicationDialogsFollowConsistentLayout();
    void snapshotRowsFitSingleLineContent();
    void snapshotsSectionUsesFramedContainer();
    void unlockStatusPanelIsReadOnlyAndScoped();
    void individualDiagnosticEnablesMatchingRepair();
    void cachedDiagnosticLabelIsCompactAndRerunSharesResultsHeader();
    void individualDiagnosticCachePersistsAcrossSelection();
    void filesystemRepairToolRowIsGatedByCapabilityEvidence();
    void fullRepairFilesystemStageIsFirstWhenEnabled();
    void fullRepairFilesystemCheckboxExistsAndPersists();
    void filesystemCheckOutputParsesStableEvidence();
    void filesystemRepairDialogRowsTrackWrappedSummary();
    void filesystemRepairFlowInspectsConfirmsAndRepairs();
    void filesystemRepairFlowUsesHostCommandsInHostMode();
    void fullRepairSkippedFilesystemPreStageIsNotCountedAsClean();
    void btrfsRepairRequiresExtraDangerConfirmation();
    void fullRepairUsesFreshEvidenceForSelectedStage();
    void fullRepairPlanSchedulesSingleEvidenceRegeneration();
    void manualRepairAfterModificationStillReportsStale();
    void bootStackReusesEfiRepairFromSameSession();
    void displayManagerToolUsesGenericLanguage();
    void repairInvalidationExplainsDiagnosticRerun();
    void logSearchFiltersApplicationLog();
    void logSectionSearchShowsWholeDiagnosticSection();
    void logFilterDropdownFiltersDiagnosticsRepairsAndSections();
    void logFilterTracksHostDkmsRepairCycleAndDiagnosticsRegeneration();
    void logSectionFilterShowsRelatedRepairsAndDiagnosticSection();
    void logSectionFilterWithoutMatchesShowsNotice();
    void logSectionFilterSearchMatchesOnlyFilteredSubset();
    void logSectionFilterExtractsEmbeddedReportSectionsAndShowsNotice();
    void logSectionFilterExtractsEmbeddedSectionsFromPriorSession();
    void logWorkflowFilesystemFilterShowsCheckRepairAndEvidence();
    void logWorkflowFilesystemFilterShowsFileCopyOperations();
    void logWorkflowPackageFilterShowsPackageStagesAndKeepsFilesystemSeparate();
    void logWorkflowPackageFilterExtractsCombinedReportSections();
    void logSearchFileTermKeepsEmbeddedSectionsCoherent();
    void logViewShowsNewestEntriesFirst();
    void rapidLogAppendsCoalesceIntoOneRefresh();
    void diagnosticRerunReplacesSectionOnly();
    void everyDiagnosticSectionReplacesOnRerun();
    void diagnosticRerunKeepsNewestSectionContent();
    void repairRerunReplacesSectionEntries();
    void repairRerunKeepsToolsAndScopesSeparate();
    void recurringStatusEntriesReplaceAcrossRepairCycles();
    void recurringStatusEntriesStaySeparatePerScopeAndDisk();
    void runAllReplacesPriorReportAndPerKeySections();
    void incrementalLogViewMatchesFullRebuild();
    void nonLinuxTargetIsRejectedSafely();
    void selectTargetStaysEnabledForEligibleHostMaintenanceCandidate();
    void unprivilegedScanUdevEvidenceEnablesSelectTarget();
    void tableFilesystemColumnShowsUdevEnrichedFilesystem();
    void preferredRepairNodeResolvesVdbRootAndCommittedTarget();
    void targetDiagnosticFallbackUpdatesCommittedComponent();
    void hostMaintenanceSelectionAndReselectionRestoreRepairScope();
    void rapidScopeSwitchingDuringSnapshotPreloadDoesNotDeadlock();
    void privilegedRequestSafetyTimeoutAbortsWedgedHelper();
    void nonBtrfsTargetSnapshotInventoryIsInformational();
    void hostMaintenanceSnapshotInventoryUsesHostScope();
    void hostSnapshotRollbackRequiresConsequencesConfirmation();
    void hostSnapshotRollbackSuccessShowsRebootRequiredAndBusyClears();
    void hostSnapshotRollbackRebootNowRequiresSecondConfirmation();
    void hostSnapshotRebootBannerPersistsAcrossScopeAndClearsOnBootChange();
    void hostSnapshotControlsFailClosedWithoutCapabilityEvidence();
    void hostSnapshotRollbackNeverRebootsAutomatically();
    void guardedWriteActionsStayDisabledWithoutTarget();
    void exportDialogsCanBeCancelledReadOnly();
    void actionRegisterPersistsAcrossWindows();
    void freshLaunchDoesNotAdoptNewestPriorSession();
    void sessionLogDefersUntilScopeIdentifiedThenAppends();
    void sessionLogScopeMarkersTrackTarget();
    void priorSessionLogsAreListedAndReadOnly();
    void activeSessionFileCannotBeDeleted();
    void sessionLogHeaderShowsGenerationTimestamp();
    void priorSessionLogLabelFallsBackToFileTime();
    void clearTruncatesOnlyActiveSessionLog();
    void clearWithoutSessionFileClearsMemoryOnly();
    void addNoteAppendsTimestampedEntry();
    void sessionLogRetentionPrunesOldFilesAndKeepsActive();
    void scopeSwitchingRejectsStaleTargetEvidence();
    void missingCapabilityEvidenceFailsClosed();
    void archIndividualDpkgAndAptUpdateStayDisabled();
    void archDkmsAvailableGatesDkmsTool();
    void debianDpkgToolAllowedAndArchRefused();
    void alpineDetectionLabelsAndCapabilityGating();
    void alpineExtlinuxToolRowIsGatedByCapabilityEvidence();
    void alpineFullRepairPlanIncludesExtlinuxStage();
    void alpineUefiEfiGrubRepairIsOfferedAndLabelled();
    void fedoraDetectionLabelsAndCapabilityGating();
    void fedoraUpgradeStageIncludedWhenEnabledAndGateRefreshesImmediately();
    void themeFollowsDesktopColorScheme();
    void themeDetectsDarkSchemeAfterStartup();
    void portalColorSchemeParsing();
    void filesystemDiagnosticUsesFsInspectAndKeepsFstabPure();
    void hostDiagnosticsRequireHostMaintenanceScope();
    void snapshotPreloadIsDeduplicatedAcrossScopeTransitions();
    void helperPathResolutionPrefersInstalledUnlessOverridden();
    void stageLabelsTrackBackendFamilyWithoutLeaking();
    void mixedPackageManagerLabelsStayGeneric();
    void disabledSettingsDoNotLeakIntoFullRepairAndPreservePreferences();
    void autoRefreshSettingDefaultsOnAndPersists();
    void autoRefreshDecisionRespectsSettingScopeAndEvidence();
    void autoRefreshStaysIdleWithoutSessionOrSetting();
    void autoRefreshSessionEstablishedRunsOnceAndCoalescesBursts();
    void autoRefreshCoalescesPlanAndManualRuns();
    void diagnosticSectionMappingTable();
    void scopedRepairRegeneratesOnlyMappedSections();
    void packageStageRepairRegeneratesCompleteSet();
    void fullRepairPlanRegeneratesOnlyChangedSectionsOnce();
    void repairUnchangedKeepsCachedEvidence();
    void repairChangeStatusInvalidatesMappedSectionsOrFailsSafe();
    void repairFailedResultSummaryAndInvalidation();
    void repairResultSummarySymbolsAreColored();
    void repairResultSummaryGlyphsRenderColored();
    void repairResultSummaryColorsSurviveFilterAndAppend();
    void repairResultSummaryColorsSurviveSectionReplacementAndPriorLog();
    void repairPlanSummaryVocabularyIsActionAccurate();
    void repairResultSummaryShowsHeldBackPackages();
    void fullRepairFilesystemPreStageIsOrderedBeforePlanSummary();
    void fullRepairCleanFilesystemPreStageIsLogOnlyAndDoesNotClaimUnchanged();
    void fullRepairIssuesFilesystemPreStageShowsSelectionDialog();
    void fullRepairNoSafeRepairModeFilesystemPreStageShowsDialog();
    void fullRepairRepairFailureShowsDialogAndClearsBusy();
    void filesystemRepairFlowBlocksSecondRepairAndKeepsBusy();
    void filesystemRepairCancelClearsBusyAndFlowGuard();
    void manualFilesystemCheckIsDistinguishableFromPlanPreStage();
    void fullRepairPlanSkipsRegenerationWhenAllStagesUnchanged();
    void fullRepairPlanFailureAttributesOnlyFailingStage();
    void repairTargetSelectionRegeneratesCompleteSet();
    void hostMaintenanceRequestsPrivilegedSessionOnce();
    void concurrentAuthorizationRequestsCoalesce();
    void authorizationFailureSurfacesPolkitAgentHint();
    void repairTargetConfirmationRequestsPrivilegedSessionOnce();
    void activePrivilegedSessionIsNotRequestedAgain();
    void authorizationCancelKeepsScopeWithoutReprompting();
    void authorizationPathCreatesNoVisibleTopLevelWindow();
    void pendingAutoRefreshRunsAfterScopeAuthorization();
    void logKindsSeparateStartupAndDiagnosticLines();
    void busyIndicatorTracksOverlappingOperations();
    void busyIndicatorUsesCustomStripedIndicator();
    void busyIndicatorPaintsClassicStripedBar();
    void busyIndicatorCoversDiagnosticsAndFailedOperations();
    void busyIndicatorDoesNotShiftLayout();
    void logsTabReorientsWithWindowWidth();
    void fileCopyButtonsDoNotOverlapAtMinimumWidth();
    void hostDriveSummaryWrapsAtMinimumWidth();
    void hostShellModeLabelsAndEnablement();
    void hostShellDispatchesAndHandlesFailureModes();
    void shellAptUpgradePolicyRetryLineIsRendered();
    void privilegedRefusalNamesNextStep();
    void targetShellDispatchStillUsesShell();
    void chrootShellInteractivePromptHintAndBusyCoverage();
    void shellReadinessGateIsSharedByButtonAndReturnPressed();
    void chrootShellReleaseInfoChangeRetryOnAccept();
    void chrootShellReleaseInfoChangeRetryDeclinedKeepsFailure();
    void hostShellReleaseInfoChangeRetryOnlyForAptFamily();
    void aptReleaseInfoChangeCommandShapes();
    void systemsTabActionButtonsShareColumn();
    void stateTogglingButtonsElideAtMinimumWidth();
    void sortableDeviceTreeSortsDrivesAndPartitions();
    void deviceTreeSortIndicatorSurvivesLegacyHeaderState();
    void sortableSnapshotTableUsesNumericAndDateKeys();
    void sortableRepairToolTreeKeepsWorkflowDefaultAndSorts();
    void sortableCapabilityTableDefaultsToFeature();
    void repairToolActionButtonStaysPinnedToPanelHeader();
    void snapshotActionsStayAboveInventory();
    void fileCopyHeaderActionsShareTitleRow();
    void diagnosticsHeaderRunAllFollowsScopeLabel();
    void scopeLabelHeadersStayOnTitleRow();
    void responsivePageHeadersWrapActionsAtMinimumWidth();
    void scopeLabelWrapsToTwoLinesBesideActions();
};

void MainWindowUiTest::initTestCase()
{
    QCoreApplication::setOrganizationName(QStringLiteral("BootRepair"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("bootrepair.org"));
    QCoreApplication::setApplicationName(QStringLiteral("BootRepairUiTests"));
    QCoreApplication::setApplicationVersion(QStringLiteral(BOOT_REPAIR_VERSION));

    QSettings settings(QStringLiteral("BootRepair"), QStringLiteral("BootRepair"));
    settings.clear();
    settings.sync();
}

void MainWindowUiTest::tabsAndReadOnlyControls()
{
    MainWindow window;
    window.show();
    QTest::qWait(100);

    QVERIFY(window.m_tabs);
    QCOMPARE(window.m_tabs->count(), 8);
    QVERIFY(!window.m_tabs->tabBar()->usesScrollButtons());
    QVERIFY(window.m_tabScrollLeftButton);
    QVERIFY(window.m_tabScrollRightButton);
    QCOMPARE(window.m_tabScrollLeftButton->arrowType(), Qt::LeftArrow);
    QCOMPARE(window.m_tabScrollRightButton->arrowType(), Qt::RightArrow);
    window.m_tabScrollRightButton->click();
    QCOMPARE(window.m_tabs->currentIndex(), 1);
    window.m_tabScrollLeftButton->click();
    QCOMPARE(window.m_tabs->currentIndex(), 0);
    QCOMPARE(window.m_tabs->tabText(0), QStringLiteral("Systems"));
    QCOMPARE(window.m_tabs->tabText(1), QStringLiteral("Diagnostics"));
    QCOMPARE(window.m_tabs->tabText(2), QStringLiteral("Repair"));
    QCOMPARE(window.m_tabs->tabText(3), QStringLiteral("Snapshots"));
    QCOMPARE(window.m_tabs->tabText(4), QStringLiteral("Chroot Shell"));
    QCOMPARE(window.m_tabs->tabText(5), QStringLiteral("File Copy"));
    QCOMPARE(window.m_tabs->tabText(6), QStringLiteral("Logs"));
    QCOMPARE(window.m_tabs->tabText(7), QStringLiteral("Settings"));
    QCOMPARE(window.m_tabs->currentIndex(), 0);
    window.resize(480, 500);
    QTest::qWait(50);
    QWidget *tabControls = window.m_tabs->cornerWidget(Qt::TopLeftCorner);
    QVERIFY(tabControls);
    QVERIFY(tabControls->geometry().right() <= window.m_tabs->tabBar()->geometry().left());
    for (QToolButton *native : window.m_tabs->tabBar()->findChildren<QToolButton *>()) {
        if (native->objectName().startsWith(QStringLiteral("Scroll"))) {
            QVERIFY(!native->isVisible());
        }
    }
    QVERIFY(window.m_chrootShellCommandEdit);
    QVERIFY(window.m_chrootShellOutput);
    QVERIFY(window.m_chrootShellRunButton);
    QVERIFY(window.m_repairVerticalSplitter);
    QCOMPARE(window.m_repairVerticalSplitter->orientation(), Qt::Vertical);
    QCOMPARE(window.m_repairVerticalSplitter->count(), 2);
    QVERIFY(!window.m_repairVerticalSplitter->childrenCollapsible());
    QVERIFY(window.m_repairSplitter);
    QCOMPARE(window.m_repairVerticalSplitter->widget(1), window.m_repairSplitter);
    QVERIFY(window.m_repairToolTree);
    QCOMPARE(window.m_repairToolTree->verticalScrollBarPolicy(), Qt::ScrollBarAlwaysOn);
    QPushButton *configurePlan = buttonWithText(window, QStringLiteral("Configure Plan…"));
    QVERIFY(configurePlan);
    QVERIFY2(configurePlan->sizeHint().width() < 300,
             "Full Repair plan controls should retain standard content-sized widths");
    QVERIFY(window.m_deviceTree);
    QCOMPARE(window.m_deviceTree->horizontalScrollBarPolicy(), Qt::ScrollBarAsNeeded);

    for (int index = 0; index < window.m_tabs->count(); ++index) {
        window.m_tabs->setCurrentIndex(index);
        QCOMPARE(window.m_tabs->currentIndex(), index);
    }

    window.m_tabs->setCurrentIndex(6);
    QPushButton *clearRegister = buttonWithText(window, QStringLiteral("Clear Register"));
    QVERIFY(clearRegister);
    QVERIFY(clearRegister->isEnabled());
    QVERIFY(window.m_logView);
    window.appendLog(QStringLiteral("UI_TEST_BEFORE_CLEAR"));
    clearRegister->click();
    QVERIFY(window.m_logView->toPlainText().contains(QStringLiteral("Action register cleared")));
}

void MainWindowUiTest::chrootShellControlsAreGuardedAndReadOnly()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    QVERIFY(window.m_chrootShellCommandEdit);
    QVERIFY(window.m_chrootShellOutput);
    QVERIFY(window.m_chrootShellRunButton);
    QVERIFY(window.m_chrootShellOutput->isReadOnly());
    window.m_previewTargetPath.clear();
    window.m_previewTargetComponentPath.clear();
    window.updateTargetLabels();
    QVERIFY(!window.m_chrootShellRunButton->isEnabled());
    QCOMPARE(window.m_chrootShellTargetLabel->text(), QStringLiteral("Target: none selected"));

    window.m_chrootShellCommandEdit->setText(QStringLiteral("update-grub"));
    QVERIFY(window.m_chrootShellCommandEdit->text().contains(QStringLiteral("update-grub")));
}

void MainWindowUiTest::hostDiagnosticAndActionRegister()
{
    MainWindow window;
    window.show();
    QTest::qWait(100);

    QVERIFY(window.m_diagnosticScopeLabel);
    QVERIFY(window.m_diagnosticList);
    QVERIFY(window.m_diagnosticResults);
    prepareRepairScope(window, true);
    enterHostDiagnosticScope(window);
    QCOMPARE(window.m_diagnosticScopeLabel->text(),
             QStringLiteral("Host maintenance: /dev/test-host"));
    QVERIFY(window.m_diagnosticList->count() > 0);
    bool hasBootEvidence = false;
    for (int row = 0; row < window.m_diagnosticList->count(); ++row) {
        if (window.m_diagnosticList->item(row)->text().contains(QStringLiteral("Boot evidence"))) {
            hasBootEvidence = true;
            break;
        }
    }
    QVERIFY(hasBootEvidence);
    window.m_diagnosticList->setCurrentRow(0);
    window.runSelectedDiagnostic();

    QVERIFY(!window.m_diagnosticResults->toPlainText().isEmpty());
    QVERIFY(window.m_actionLogEntries.join(QLatin1Char('\n')).contains(QStringLiteral("Host diagnostic")));

    QPushButton *copy = buttonWithText(window, QStringLiteral("Copy Results"));
    QVERIFY(copy);
    QVERIFY(copy->isEnabled());
    window.copyDiagnosticResults();
    QVERIFY(!QApplication::clipboard()->text().isEmpty());
    QVERIFY(window.m_actionLogEntries.join(QLatin1Char('\n')).contains(QStringLiteral("copied to the clipboard")));

    // Run All stays entirely in the running host scope and exercises every
    // registered read-only diagnostic implementation in one pass.
    window.runAllDiagnostics();
    QVERIFY(window.m_diagnosticResults->toPlainText().contains(QStringLiteral("========================================")));
    QVERIFY(window.m_actionLogEntries.join(QLatin1Char('\n')).contains(QStringLiteral("Starting all available read-only diagnostics")));
}

void MainWindowUiTest::diagnosticOutputIsMirroredToMainLog()
{
    MainWindow window;
    window.show();
    QTest::qWait(100);

    QVERIFY(window.m_diagnosticScopeLabel);
    QVERIFY(window.m_diagnosticList);
    QVERIFY(window.m_diagnosticResults);
    QVERIFY(window.m_logView);
    enterHostDiagnosticScope(window);

    // Select a deterministic individual diagnostic so the test checks that
    // its complete captured output is copied to the application log, rather
    // than only the short "diagnostic run" status entry.
    int environmentRow = -1;
    for (int row = 0; row < window.m_diagnosticList->count(); ++row) {
        if (window.m_diagnosticList->item(row)->data(Qt::UserRole).toString() == QStringLiteral("environment")) {
            environmentRow = row;
            break;
        }
    }
    QVERIFY(environmentRow >= 0);
    window.m_diagnosticList->setCurrentRow(environmentRow);
    window.runSelectedDiagnostic();
    flushLogRefresh(window);

    const QString result = window.m_diagnosticResults->toPlainText().trimmed();
    QVERIFY(!result.isEmpty());
    const QString log = window.m_logView->toPlainText();
    QVERIFY(log.contains(QStringLiteral("========================================")));
    QVERIFY(log.contains(QStringLiteral("Diagnostic: environment")));
    QVERIFY(log.contains(QStringLiteral("Scope: Running Host")));
    QVERIFY(log.contains(QStringLiteral("What was actually run")));
    QVERIFY(log.contains(result));

    // Run All should append a report block to the same persistent log, while
    // retaining the individual section markers used for later inspection.
    const int logLengthBeforeAll = log.size();
    window.runAllDiagnostics();
    flushLogRefresh(window);
    const QString reportLog = window.m_logView->toPlainText();
    QVERIFY(reportLog.size() > logLengthBeforeAll);
    QVERIFY(reportLog.contains(QStringLiteral("Diagnostic: environment")));
    QVERIFY(reportLog.contains(QStringLiteral("Diagnostic: report")));

    // Re-running one diagnostic invalidates the prior aggregate report so
    // repair actions cannot consume mixed-generation evidence.
    window.m_diagnosticList->setCurrentRow(environmentRow);
    window.runSelectedDiagnostic();
    flushLogRefresh(window);
    QVERIFY(!window.m_hostDiagnosticCache.contains(QStringLiteral("report")));
}

void MainWindowUiTest::narrowRepairAndFileCopyScrollAreasStayStable()
{
    MainWindow window;
    window.resize(560, 380);
    window.show();
    QTest::qWait(100);

    const QList<QPair<QString, int>> pages = {
        {QStringLiteral("repairPageScroll"), 2},
        {QStringLiteral("fileCopyPageScroll"), 5}
    };
    for (const auto &page : pages) {
        window.m_tabs->setCurrentIndex(page.second);
        QCoreApplication::processEvents();
        auto *scroll = window.findChild<QScrollArea *>(page.first);
        QVERIFY2(scroll, qPrintable(page.first));
        QVERIFY(scroll->widget());
        QCOMPARE(scroll->horizontalScrollBarPolicy(), Qt::ScrollBarAlwaysOff);
        QCOMPARE(scroll->alignment(), Qt::AlignLeft | Qt::AlignTop);
        QCOMPARE(scroll->horizontalScrollBar()->maximum(), 0);

        const QRect settled = scroll->widget()->geometry();
        QVERIFY(settled.width() > 0);
        QVERIFY(settled.width() <= scroll->viewport()->width());
        const int settledX = settled.x();
        QTest::qWait(50);
        QCOMPARE(scroll->widget()->geometry(), settled);

        // Repeated tab/layout changes at the narrow width must not move the
        // page content into a hidden horizontal-scroll offset.
        window.m_tabs->setCurrentIndex(0);
        window.m_tabs->setCurrentIndex(page.second);
        QCoreApplication::processEvents();
        QCOMPARE(scroll->horizontalScrollBar()->maximum(), 0);
        QCOMPARE(scroll->widget()->geometry().x(), settledX);
    }
}

void MainWindowUiTest::fullRepairButtonsDoNotOverlapAtMinimumWidth()
{
    MainWindow window;
    window.resize(480, 500);
    window.show();
    QTest::qWait(100);
    window.m_tabs->setCurrentIndex(2); // Repair
    QCoreApplication::processEvents();

    // Enable both controls with a fresh capability-evidence cache so the row
    // is exercised in its busiest state at the minimum supported width.
    prepareRepairScope(window);
    window.m_snapshotPreloadScheduled = true;
    cacheRepairEvidence(window, capabilityEvidence(false, true));
    window.updateFullRepairSummary();
    QCoreApplication::processEvents();
    QTest::qWait(50);

    QPushButton *configurePlan = buttonWithText(window, QStringLiteral("Configure Plan…"));
    QVERIFY(configurePlan);
    QVERIFY(window.m_runFullRepairButton);
    QVERIFY2(configurePlan->isVisible(), "Configure Plan must stay visible at minimum width");
    QVERIFY2(window.m_runFullRepairButton->isVisible(), "Run Full Repair must stay visible at minimum width");
    QVERIFY2(configurePlan->isEnabled(), "Configure Plan must stay enabled at minimum width");
    QVERIFY2(window.m_runFullRepairButton->isEnabled(), "Run Full Repair must stay enabled at minimum width");
    // Both controls must be allowed to shrink below their content width so the
    // row can fit instead of being over-constrained into overlapping.
    QVERIFY2(configurePlan->minimumWidth() < configurePlan->sizeHint().width(),
             "Configure Plan must be shrinkable");
    QVERIFY2(window.m_runFullRepairButton->minimumWidth() < window.m_runFullRepairButton->sizeHint().width(),
             "Run Full Repair must be shrinkable");

    QWidget *commonParent = configurePlan->parentWidget();
    QVERIFY(commonParent);
    const QRect configureRect(configurePlan->mapTo(commonParent, QPoint(0, 0)), configurePlan->size());
    const QRect runRect(window.m_runFullRepairButton->mapTo(commonParent, QPoint(0, 0)),
                        window.m_runFullRepairButton->size());
    QVERIFY2(!configureRect.intersects(runRect),
             "Configure Plan and Run Full Repair must not overlap at minimum width");
    QVERIFY2(configureRect.right() < runRect.left(),
             "the Full Repair buttons must remain separated at minimum width");
}

void MainWindowUiTest::fullRepairPlanListUsesSplitterAdjustableHeight()
{
    MainWindow window;
    // A taller window leaves the tools pane its minimum while the splitter
    // still has room to grow the plan list well past its default height.
    window.resize(1200, 1000);
    window.show();
    QTest::qWait(100);
    window.m_tabs->setCurrentIndex(2); // Repair
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window);

    // Make every configured stage available so the plan list can reach its
    // scroll cap; the shared helper marks the display manager unavailable.
    QString evidence = capabilityEvidence(false, true);
    evidence.replace(QStringLiteral("Repair tool display: unavailable|No display manager installed"),
                     QStringLiteral("Repair tool display: available"));
    evidence += QStringLiteral("Repair tool filesystem: available\n");
    cacheRepairEvidence(window, evidence);

    QVERIFY(window.m_fullRepairStageList);
    QVERIFY(window.m_fullRepairPlanBox);
    QGroupBox *toolsBox = qobject_cast<QGroupBox *>(window.m_repairSplitter->widget(0));
    QVERIFY2(toolsBox, "the Individual repair tools section must be the first tools pane");

    const QList<QCheckBox *> toggles = {
        window.m_fullRepairFilesystem,
        window.m_fullRepairDpkg, window.m_fullRepairBrokenPackages, window.m_fullRepairAptUpdate,
        window.m_fullRepairUpgrade, window.m_fullRepairDkms, window.m_fullRepairDisplayManager,
        window.m_fullRepairInitramfs, window.m_fullRepairEfi, window.m_fullRepairGrub
    };
    for (QCheckBox *toggle : toggles) {
        QVERIFY(toggle);
        toggle->setChecked(false);
    }
    window.updateFullRepairSummary();
    QCoreApplication::processEvents();
    QTest::qWait(20);

    // Expected list height for the visible rows using the same delegate row
    // hints as the sizing rule; the plan frame must not add empty space below.
    auto expectedPlanHeight = [&window](int rows) {
        const int visibleRows = qBound(2, rows, 8);
        int rowHeight = 0;
        for (int row = 0; row < window.m_fullRepairStageList->count(); ++row) {
            rowHeight = qMax(rowHeight, window.m_fullRepairStageList->sizeHintForRow(row));
        }
        return visibleRows * rowHeight + 2 * window.m_fullRepairStageList->frameWidth();
    };
    auto planFrameGap = [&window, toolsBox]() {
        const QPoint frameBottom(window.m_fullRepairPlanBox->mapTo(
            window.m_repairVerticalSplitter,
            QPoint(0, window.m_fullRepairPlanBox->height())));
        const QPoint toolsTop(toolsBox->mapTo(window.m_repairVerticalSplitter, QPoint(0, 0)));
        return toolsTop.y() - frameBottom.y();
    };
    auto planInternalGap = [&window]() {
        const QPoint listBottom(window.m_fullRepairStageList->mapTo(
            window.m_fullRepairPlanBox,
            QPoint(0, window.m_fullRepairStageList->height())));
        return window.m_fullRepairPlanBox->height() - listBottom.y();
    };

    // Two stages: the list starts at its minimum (a few rows), not a forced
    // content height, and the tools section follows directly.
    window.m_fullRepairDpkg->setChecked(true);
    window.m_fullRepairBrokenPackages->setChecked(true);
    window.updateFullRepairSummary();
    QCoreApplication::processEvents();
    QTest::qWait(20);

    QCOMPARE(window.m_fullRepairStageList->count(), 2);
    const int minimumHeight = expectedPlanHeight(3);
    QVERIFY2(window.m_fullRepairStageList->height() <= minimumHeight + 2,
             qPrintable(QStringLiteral("two stages: list height %1 exceeds the minimum %2")
                            .arg(window.m_fullRepairStageList->height()).arg(minimumHeight)));
    QVERIFY2(planFrameGap() >= 0 && planFrameGap() < 16,
             qPrintable(QStringLiteral("two stages: gap below plan frame is %1").arg(planFrameGap())));
    QVERIFY2(planInternalGap() < 24,
             qPrintable(QStringLiteral("two stages: empty space inside plan frame is %1").arg(planInternalGap())));

    // Nine stages: the list keeps its starting height and scrolls instead of
    // being forced to the content height.
    for (QCheckBox *toggle : toggles) {
        toggle->setChecked(true);
    }
    window.updateFullRepairSummary();
    QCoreApplication::processEvents();
    QTest::qWait(20);

    QCOMPARE(window.m_fullRepairStageList->count(), 10);
    QVERIFY2(window.m_fullRepairStageList->height() <= minimumHeight + 2,
             "the plan list must keep its starting height instead of forcing the content height");
    QVERIFY2(window.m_fullRepairStageList->verticalScrollBar()->maximum() > 0,
             "the plan list must scroll while it keeps its starting height");
    QVERIFY2(planFrameGap() >= 0 && planFrameGap() < 16,
             qPrintable(QStringLiteral("nine stages: gap below plan frame is %1").arg(planFrameGap())));

    // The splitter is fully adjustable: growing it shows more stage rows, and
    // shrinking it returns the list to its minimum.
    window.m_repairVerticalSplitter->setSizes({minimumHeight + 300, 120});
    QCoreApplication::processEvents();
    QTest::qWait(20);
    QVERIFY2(window.m_fullRepairStageList->height() > minimumHeight + 100,
             qPrintable(QStringLiteral("the splitter must be able to grow the plan list "
                                       "(list %1, minimum %2, plan box %3, splitter %4/%5, splitter height %6)")
                            .arg(window.m_fullRepairStageList->height())
                            .arg(minimumHeight)
                            .arg(window.m_fullRepairPlanBox->height())
                            .arg(window.m_repairVerticalSplitter->sizes().value(0))
                            .arg(window.m_repairVerticalSplitter->sizes().value(1))
                            .arg(window.m_repairVerticalSplitter->height())));
    window.m_repairVerticalSplitter->setSizes({1, 100000});
    QCoreApplication::processEvents();
    QTest::qWait(20);
    QVERIFY2(window.m_fullRepairStageList->height() <= minimumHeight + 2,
             "the splitter must be able to shrink the plan list back to its minimum");

    // Deselecting stages leaves the splitter-owned height in place.
    for (QCheckBox *toggle : toggles) {
        toggle->setChecked(false);
    }
    window.m_fullRepairGrub->setChecked(true);
    window.updateFullRepairSummary();
    QCoreApplication::processEvents();
    QTest::qWait(20);

    QCOMPARE(window.m_fullRepairStageList->count(), 1);
    QVERIFY2(window.m_fullRepairStageList->height() <= minimumHeight + 2,
             "the plan list must stay at the splitter-owned height");
    QVERIFY2(window.m_fullRepairStageList->verticalScrollBar()->maximum() == 0,
             "a single-stage plan must not scroll");
}

void MainWindowUiTest::repairSplitterDefaultsFavorToolListWidth()
{
    // Exercise the built-in defaults, not a state saved by another test.
    QSettings settings(QStringLiteral("BootRepair"), QStringLiteral("BootRepair"));
    settings.remove(QStringLiteral("repair/splitterStateV3"));
    settings.sync();

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_tabs->setCurrentIndex(2); // Repair
    QCoreApplication::processEvents();
    QTest::qWait(50);

    QVERIFY(window.m_repairSplitter);
    QCOMPARE(window.m_repairSplitter->orientation(), Qt::Horizontal);
    QVERIFY2(!window.m_repairSplitter->childrenCollapsible(),
             "both repair panes must stay user-adjustable and non-collapsible");
    const QList<int> sizes = window.m_repairSplitter->sizes();
    QCOMPARE(sizes.size(), 2);
    QVERIFY2(sizes.at(0) > sizes.at(1),
             "the Individual repair tools pane must receive the larger default share");
    const int total = sizes.at(0) + sizes.at(1);
    QVERIFY(total > 0);
    const int toolsShare = sizes.at(0) * 100 / total;
    QVERIFY2(toolsShare >= 55 && toolsShare <= 70,
             qPrintable(QStringLiteral("tools pane share is %1% (sizes %2/%3)")
                            .arg(toolsShare).arg(sizes.at(0)).arg(sizes.at(1))));

    // The longest complete tool name must fit the Tool column at the default
    // window size instead of being elided.
    QVERIFY(window.m_repairToolTree);
    const QFontMetrics metrics(window.m_repairToolTree->font());
    QString longest;
    int longestWidth = 0;
    for (int row = 0; row < window.m_repairToolTree->topLevelItemCount(); ++row) {
        const QString title = window.m_repairToolTree->topLevelItem(row)->text(0);
        const int width = metrics.horizontalAdvance(title);
        if (width > longestWidth) {
            longestWidth = width;
            longest = title;
        }
    }
    QVERIFY2(!longest.isEmpty(), "the repair tool tree must list the individual tools");
    const int columnWidth = window.m_repairToolTree->columnWidth(0);
    QVERIFY2(columnWidth >= longestWidth + window.m_repairToolTree->indentation(),
             qPrintable(QStringLiteral("Tool column width %1 cannot show '%2' (%3 px + %4 px indentation)")
                            .arg(columnWidth).arg(longest).arg(longestWidth)
                            .arg(window.m_repairToolTree->indentation())));

    // Both panes remain user-adjustable: an explicit drag target is honored.
    window.m_repairSplitter->setSizes({300, 700});
    QCoreApplication::processEvents();
    const QList<int> adjusted = window.m_repairSplitter->sizes();
    QCOMPARE(adjusted.size(), 2);
    QVERIFY2(adjusted.at(1) > adjusted.at(0), "the user must be able to widen the description pane");
    QVERIFY(adjusted.at(0) > 0);
    QVERIFY(adjusted.at(1) > 0);
}

void MainWindowUiTest::applicationDialogsFollowConsistentLayout()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    QVERIFY(window.m_fileCopyDirectionCombo);
    QVERIFY(window.m_destinationEdit);

    // The repaired target is intentionally virtual/unmounted, so the custom
    // destination dialog is the deterministic app-owned modal we can exercise
    // without authorizing a privileged operation.  Capture its layout before
    // rejecting it and verify the shared KDE-style spacing contract.
    window.m_previewTargetPath = QStringLiteral("/dev/test-data-disk");
    window.m_fileCopyDirectionCombo->setCurrentIndex(0);
    QMargins margins;
    int spacing = -1;
    bool hasButtons = false;
    QTimer poll;
    poll.setInterval(5);
    connect(&poll, &QTimer::timeout, &window, [&] {
        for (QWidget *widget : QApplication::topLevelWidgets()) {
            auto *dialog = qobject_cast<QDialog *>(widget);
            if (!dialog || dialog->objectName() != QStringLiteral("repairDestinationDialog")) {
                continue;
            }
            if (auto *layout = dialog->layout()) {
                margins = layout->contentsMargins();
                spacing = layout->spacing();
            }
            hasButtons = dialog->findChild<QDialogButtonBox *>() != nullptr;
            poll.stop();
            dialog->reject();
            return;
        }
    });
    poll.start();
    window.browseFileCopyDestination();
    QVERIFY(hasButtons);
    QCOMPARE(margins.left(), 20);
    QCOMPARE(margins.right(), 20);
    QCOMPARE(margins.top(), 16);
    QCOMPARE(margins.bottom(), 16);
    QCOMPARE(spacing, 10);

    // The clear affordance supplied by QLineEdit must remain vertically
    // centered when the shared button normalization runs.  This guards the
    // small-window regression where the icon appeared pinned to the bottom.
    QVERIFY(window.m_logSearchEdit);
    window.m_tabs->setCurrentIndex(6);
    window.m_logSearchEdit->setText(QStringLiteral("theme"));
    QCoreApplication::processEvents();
    const auto clearButtons = window.m_logSearchEdit->findChildren<QToolButton *>();
    QVERIFY(!clearButtons.isEmpty());
    const QToolButton *clearButton = clearButtons.constFirst();
    QVERIFY(clearButton->isVisible());
    const int centerDelta = std::abs(clearButton->geometry().center().y()
                                     - window.m_logSearchEdit->rect().center().y());
    QVERIFY2(centerDelta <= window.m_logSearchEdit->height() / 4,
             "QLineEdit clear button is not vertically centered");
}

void MainWindowUiTest::snapshotRowsFitSingleLineContent()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    QVERIFY(window.m_snapshotTable);
    QVERIFY(window.m_snapshotSplitter);
    QCOMPARE(window.m_snapshotSplitter->orientation(), Qt::Vertical);
    QCOMPARE(window.m_snapshotSplitter->count(), 2);
    QVERIFY(!window.m_snapshotSplitter->childrenCollapsible());
    QCOMPARE(window.m_snapshotSplitter->widget(0), window.m_snapshotTable);
    QCOMPARE(window.m_snapshotSplitter->widget(1), window.m_snapshotDetails);
    QCOMPARE(window.m_snapshotTable->verticalScrollBarPolicy(), Qt::ScrollBarAlwaysOn);
    window.m_tabs->setCurrentIndex(3);
    window.resize(1400, 900);
    QCoreApplication::processEvents();

    // Populate representative one-line rows after the table is visible. This
    // guards against asynchronous snapshot loading measuring rows while the
    // page is hidden and retaining a tall wrapped height.
    window.m_snapshotTable->setColumnWidth(0, 90);
    window.m_snapshotTable->setColumnWidth(1, 220);
    window.m_snapshotTable->setColumnWidth(2, 90);
    window.m_snapshotTable->setColumnWidth(3, 240);
    window.m_snapshotTable->setColumnWidth(4, 280);
    window.m_snapshotTable->setRowCount(1);
    window.m_snapshotTable->setItem(0, 0, new QTableWidgetItem(QStringLiteral("14")));
    window.m_snapshotTable->setItem(0, 1, new QTableWidgetItem(QStringLiteral("2026-09-10")));
    window.m_snapshotTable->setItem(0, 2, new QTableWidgetItem(QStringLiteral("pre")));
    window.m_snapshotTable->setItem(0, 3, new QTableWidgetItem(QStringLiteral("apt")));
    window.m_snapshotTable->setItem(0, 4, new QTableWidgetItem(QStringLiteral("Linux root")));
    window.resizeSnapshotRows();

    const int expected = QFontMetrics(window.m_snapshotTable->font()).lineSpacing() + 10;
    QVERIFY2(window.m_snapshotTable->rowHeight(0) <= expected + 2,
             "single-line snapshot content should use a compact row height");
}

void MainWindowUiTest::snapshotsSectionUsesFramedContainer()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    // The Snapshots tab content must use the same framed QGroupBox container
    // as the Diagnostics, Repair and Settings sections.
    QVERIFY(window.m_snapshotSplitter);
    auto *snapshotBox = qobject_cast<QGroupBox *>(window.m_snapshotSplitter->parentWidget());
    QVERIFY2(snapshotBox, "the snapshot inventory must be wrapped in the shared framed container");
    QVERIFY2(!snapshotBox->title().isEmpty(), "the framed snapshot container needs a visible title");
    QVERIFY2(snapshotBox->isAncestorOf(window.m_snapshotDetails),
             "the snapshot details pane must live inside the framed container");
    QVERIFY2(snapshotBox->isAncestorOf(window.m_snapshotLoadButton),
             "the snapshot actions must live inside the framed container");
    QVERIFY(window.m_snapshotSplitter->count() == 2);
    QCOMPARE(window.m_snapshotSplitter->widget(0), window.m_snapshotTable);
    QCOMPARE(window.m_snapshotSplitter->widget(1), window.m_snapshotDetails);
}

void MainWindowUiTest::unlockStatusPanelIsReadOnlyAndScoped()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    QVERIFY(window.m_unlockStatusView);
    QVERIFY(window.m_unlockStatusBox);
    // The status frame expands into the left pane's remaining space so its
    // bottom edge stays aligned with the selected-drive details frame.
    QCOMPARE(window.m_unlockStatusBox->sizePolicy().verticalPolicy(), QSizePolicy::Expanding);
    QCOMPARE(window.m_unlockStatusView->sizePolicy().verticalPolicy(), QSizePolicy::Expanding);
    QVERIFY(window.m_selectedDriveDetailsBox);
    if (window.m_systemSplitter->orientation() == Qt::Horizontal) {
        QCoreApplication::processEvents();
        const int unlockBottom = window.m_unlockStatusBox->mapTo(
            window.m_systemSplitter, QPoint(0, window.m_unlockStatusBox->height())).y();
        const int detailsBottom = window.m_selectedDriveDetailsBox->mapTo(
            window.m_systemSplitter, QPoint(0, window.m_selectedDriveDetailsBox->height())).y();
        QVERIFY2(qAbs(unlockBottom - detailsBottom) <= 2,
                 "unlock status and selected-drive details frames should share a bottom edge");
    }
    QVERIFY(window.m_unlockStatusView->isReadOnly());
    QVERIFY(!window.m_unlockStatusView->toPlainText().isEmpty());

    // Session cleanup must discard cached unlock evidence so a subsequent
    // target selection cannot display a mapper that is no longer owned.
    window.m_unlockStatusCache.insert(QStringLiteral("/dev/test-disk"), QStringLiteral("UNLOCKED=/dev/mapper/test"));
    window.closePrivilegedSession();
    QVERIFY(window.m_unlockStatusCache.isEmpty());
    QVERIFY(window.m_unlockStatusView->toPlainText().contains(QStringLiteral("No unlock operation")));
}

void MainWindowUiTest::individualDiagnosticEnablesMatchingRepair()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    DeviceNode component;
    component.path = QStringLiteral("/dev/test-root");
    component.type = QStringLiteral("part");
    component.fileSystem = QStringLiteral("ext4");
    component.linuxCapableFileSystem = true;
    component.installedLinux = true;
    DeviceNode disk;
    disk.path = QStringLiteral("/dev/test-system");
    disk.type = QStringLiteral("disk");
    disk.children.append(component);
    window.m_deviceIndex.insert(disk.path, disk);
    window.m_deviceIndex.insert(component.path, component);
    window.m_previewTargetPath = disk.path;
    window.m_previewTargetComponentPath = component.path;
    window.m_targetDiagnosticCacheIdentity = disk.path;
    window.m_targetDiagnosticCache.insert(QStringLiteral("grub"), QStringLiteral("Diagnostic: grub\nRepair tool grub: available\nPASS"));

    QVERIFY(window.m_repairToolTree);
    for (int row = 0; row < window.m_repairToolTree->topLevelItemCount(); ++row) {
        auto *item = window.m_repairToolTree->topLevelItem(row);
        if (item->data(0, Qt::UserRole).toString() == QStringLiteral("grub")) {
            window.m_repairToolTree->setCurrentItem(item);
            break;
        }
    }
    window.updateRepairToolDetails();
    QVERIFY(window.m_repairToolButton);
    QVERIFY2(window.m_repairToolButton->isEnabled(),
             "A fresh matching target diagnostic must enable its individual repair action");
}

void MainWindowUiTest::cachedDiagnosticLabelIsCompactAndRerunSharesResultsHeader()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_tabs->setCurrentIndex(1); // Diagnostics

    prepareRepairScope(window);
    cacheRepairEvidence(window, capabilityEvidence(false, true));
    const QDateTime capturedAt = QDateTime::fromString(QStringLiteral("2026-09-19 20:32:29"),
                                                       QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    QVERIFY(capturedAt.isValid());
    window.m_targetDiagnosticTimes.insert(QStringLiteral("grub"), capturedAt);

    int grubRow = -1;
    for (int row = 0; row < window.m_diagnosticList->count(); ++row) {
        if (window.m_diagnosticList->item(row)->data(Qt::UserRole).toString() == QStringLiteral("grub")) {
            grubRow = row;
            break;
        }
    }
    QVERIFY(grubRow >= 0);
    window.m_diagnosticList->setCurrentRow(grubRow);
    window.updateDiagnosticDetails();

    // The visible label is only the capture timestamp; the scope and
    // freshness explanation survives in the hover tooltip.
    QCOMPARE(window.m_diagnosticAvailability->text(), QStringLiteral("Cached: 2026-09-19 20:32:29 result"));
    QVERIFY2(window.m_diagnosticAvailability->toolTip().contains(
                 QStringLiteral("read-only selected repair-system result")),
             "the compact label must keep the scope explanation in its tooltip");
    QVERIFY2(window.m_diagnosticAvailability->toolTip().contains(
                 QStringLiteral("use Re-run only for fresh data")),
             "the compact label must keep the freshness guidance in its tooltip");
    QCOMPARE(window.m_runDiagnosticButton->text(), QStringLiteral("Re-run Diagnostic"));
    QVERIFY(window.m_runDiagnosticButton->isEnabled());
    QVERIFY(!window.m_diagnosticResults->toPlainText().isEmpty());

    // The run/re-run action shares the Results heading row: same vertical
    // center as the heading, to its right and flush with the results pane.
    QVERIFY(window.m_diagnosticResultsTitle);
    const auto headingCenter = [&window] {
        return window.m_diagnosticResultsTitle->mapTo(
            &window, window.m_diagnosticResultsTitle->rect().center());
    };
    const auto buttonCenter = [&window] {
        return window.m_runDiagnosticButton->mapTo(
            &window, window.m_runDiagnosticButton->rect().center());
    };
    const int buttonRight = window.m_runDiagnosticButton->mapTo(
        &window, QPoint(window.m_runDiagnosticButton->width(), 0)).x();
    const int resultsRight = window.m_diagnosticResults->mapTo(
        &window, QPoint(window.m_diagnosticResults->width(), 0)).x();
    const int buttonBottom = window.m_runDiagnosticButton->mapTo(
        &window, QPoint(0, window.m_runDiagnosticButton->height())).y();
    const int resultsTop = window.m_diagnosticResults->mapTo(&window, QPoint(0, 0)).y();

    QVERIFY2(qAbs(headingCenter().y() - buttonCenter().y()) <= 2,
             qPrintable(QStringLiteral("Results heading center y=%1, run button center y=%2")
                 .arg(headingCenter().y()).arg(buttonCenter().y())));
    QVERIFY2(buttonCenter().x() > headingCenter().x(),
             "the run/re-run button must sit to the right of the Results heading");
    QVERIFY2(qAbs(buttonRight - resultsRight) <= 2,
             qPrintable(QStringLiteral("run button right=%1, results pane right=%2")
                 .arg(buttonRight).arg(resultsRight)));
    QVERIFY2(buttonBottom <= resultsTop,
             "the run/re-run button must stay above the results text area");

    // The standalone run row is gone: the heading row follows the availability
    // line by just the layout spacing, so the action cannot float between the
    // cached line and the results box again.
    const int availabilityBottom = window.m_diagnosticAvailability->mapTo(
        &window, QPoint(0, window.m_diagnosticAvailability->height())).y();
    const int headingTop = window.m_diagnosticResultsTitle->mapTo(&window, QPoint(0, 0)).y();
    QVERIFY2(headingTop - availabilityBottom <= 16,
             qPrintable(QStringLiteral("availability line bottom=%1, Results heading top=%2")
                 .arg(availabilityBottom).arg(headingTop)));
    QVERIFY2(window.m_diagnosticResults->height()
                 > window.m_runDiagnosticButton->height() + window.m_diagnosticResultsTitle->height(),
             "the results text area must keep the space freed by the removed run row");

    // The same row geometry must survive the smallest supported window: no
    // overlap with the results pane and a usable results height.
    window.resize(480, 500);
    QCoreApplication::processEvents();
    QTest::qWait(50);
    const QRect smallHeading = window.m_diagnosticResultsTitle->geometry();
    const QRect smallButton = window.m_runDiagnosticButton->geometry();
    const QRect smallResults = window.m_diagnosticResults->geometry();
    QVERIFY2(smallHeading.top() <= smallButton.bottom()
                 && smallButton.top() <= smallHeading.bottom(),
             qPrintable(QStringLiteral("minimum size: heading rect %1,%2 %3x%4, button rect %5,%6 %7x%8")
                 .arg(smallHeading.x()).arg(smallHeading.y())
                 .arg(smallHeading.width()).arg(smallHeading.height())
                 .arg(smallButton.x()).arg(smallButton.y())
                 .arg(smallButton.width()).arg(smallButton.height())));
    QVERIFY2(smallButton.left() > smallHeading.left(),
             "the run/re-run button must stay right of the Results heading at minimum size");
    QVERIFY2(smallButton.bottom() <= smallResults.top(),
             qPrintable(QStringLiteral("minimum size: run button bottom=%1, results pane top=%2")
                 .arg(smallButton.bottom()).arg(smallResults.top())));
    QVERIFY2(window.m_diagnosticResultsTitle->parentWidget()
                 && window.m_diagnosticResultsTitle->parentWidget()->rect().contains(smallButton),
             "the run/re-run button must stay inside the selected-diagnostic frame at minimum size");
    QVERIFY2(window.m_diagnosticResults->height() > 0,
             "the results text area must keep a visible height at minimum size");
}

// Every individual diagnostic keeps its own cached result and timestamp for
// the active scope: selecting another diagnostic and returning must show the
// previous output again without re-running anything. Only an explicit scope
// change, repair invalidation or a fresh run of that section replaces it.
void MainWindowUiTest::individualDiagnosticCachePersistsAcrossSelection()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);
    window.m_tabs->setCurrentIndex(1); // Diagnostics

    prepareRepairScope(window);

    const int fstabRow = diagnosticRow(window, QStringLiteral("fstab"));
    const int usageRow = diagnosticRow(window, QStringLiteral("usage"));
    QVERIFY2(fstabRow >= 0, "the fstab diagnostic must exist");
    QVERIFY2(usageRow >= 0, "the Disk usage diagnostic must exist");

    // Run A (fstab) through the UI-test diagnostics backend.
    window.m_diagnosticList->setCurrentRow(fstabRow);
    window.runSelectedDiagnostic();
    const QString fstabResult = window.m_targetDiagnosticCache.value(QStringLiteral("fstab"));
    QVERIFY2(!fstabResult.isEmpty(), "running fstab must cache its result");
    const QDateTime fstabCapturedAt = window.m_targetDiagnosticTimes.value(QStringLiteral("fstab"));
    QVERIFY2(fstabCapturedAt.isValid(), "running fstab must record its capture timestamp");
    QCOMPARE(window.m_diagnosticResults->toPlainText(), fstabResult);

    // Select and run B (Disk usage): the cached fstab evidence and timestamp
    // must survive both the selection and another diagnostic's run.
    window.m_diagnosticList->setCurrentRow(usageRow);
    window.runSelectedDiagnostic();
    QVERIFY2(!window.m_targetDiagnosticCache.value(QStringLiteral("usage")).isEmpty(),
             "running Disk usage must cache its own result");
    QVERIFY2(window.m_targetDiagnosticCache.contains(QStringLiteral("fstab")),
             "running another diagnostic must not clear fstab's cached result");
    QCOMPARE(window.m_targetDiagnosticTimes.value(QStringLiteral("fstab")), fstabCapturedAt);

    // Select A again: its cached result and timestamp are still shown.
    window.m_diagnosticList->setCurrentRow(fstabRow);
    QCOMPARE(window.m_diagnosticResults->toPlainText(), fstabResult);
    QCOMPARE(window.m_targetDiagnosticTimes.value(QStringLiteral("fstab")), fstabCapturedAt);
    QVERIFY2(window.m_diagnosticAvailability->text().contains(QStringLiteral("Cached")),
             qPrintable(window.m_diagnosticAvailability->text()));
    QCOMPARE(window.m_runDiagnosticButton->text(), QStringLiteral("Re-run Diagnostic"));
    QVERIFY(window.m_runDiagnosticButton->isEnabled());
    QVERIFY(window.m_copyDiagnosticButton->isEnabled());
}

void MainWindowUiTest::filesystemRepairToolRowIsGatedByCapabilityEvidence()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;

    prepareRepairScope(window);
    QVERIFY(window.m_repairToolTree);
    QTreeWidgetItem *filesystemItem = repairItem(window, QStringLiteral("filesystem"));
    QVERIFY2(filesystemItem, "the File system repair tool row must exist");
    QCOMPARE(filesystemItem->text(0), QStringLiteral("File system repair"));

    // Unavailable evidence disables the row and surfaces the helper's reason.
    QString evidence = capabilityEvidence(false, true);
    evidence += QStringLiteral("Repair tool filesystem: unavailable|No supported file system check tool is installed in the recovery environment\n");
    cacheRepairEvidence(window, evidence);
    window.updateFullRepairSummary();
    filesystemItem = repairItem(window, QStringLiteral("filesystem"));
    QVERIFY(filesystemItem);
    QVERIFY2(!window.repairToolAvailable(QStringLiteral("filesystem")),
             "an unavailable filesystem capability must fail closed");
    QVERIFY(filesystemItem->text(1).contains(QStringLiteral("No supported file system check tool")));
    QVERIFY(window.m_fullRepairFilesystem);
    QVERIFY2(!window.m_fullRepairFilesystem->isEnabled(),
             "the Settings stage must be disabled with the unavailable capability");
    QVERIFY2(window.m_fullRepairFilesystem->toolTip().contains(
                 QStringLiteral("No supported file system check tool")),
             "the disabled Settings stage must surface the helper's reason");
    QVERIFY2(!window.selectedRepairStages().contains(QStringLiteral("filesystem")),
             "an unavailable filesystem stage must never enter the execution plan");

    // Available evidence enables the row.
    evidence = capabilityEvidence(false, true);
    evidence += QStringLiteral("Repair tool filesystem: available\n");
    cacheRepairEvidence(window, evidence);
    window.updateFullRepairSummary();
    QVERIFY2(window.repairToolAvailable(QStringLiteral("filesystem")),
             "available filesystem capability evidence must enable the tool");
    QVERIFY(window.m_fullRepairFilesystem->isEnabled());

    window.m_repairToolTree->setCurrentItem(repairItem(window, QStringLiteral("filesystem")));
    window.updateRepairToolDetails();
    QVERIFY(window.m_repairToolTitle);
    QVERIFY(window.m_repairToolTitle->text().contains(QStringLiteral("File system repair")));
    QVERIFY(window.m_repairToolButton);
    QVERIFY2(window.m_repairToolButton->isEnabled(),
             "the Check File Systems action must be enabled for an available filesystem capability");
    QVERIFY(window.m_repairToolDescription->text().contains(QStringLiteral("read-only")));
    QVERIFY(window.m_repairToolDescription->text().contains(QStringLiteral("mounted filesystem")));
}

void MainWindowUiTest::fullRepairFilesystemStageIsFirstWhenEnabled()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;

    prepareRepairScope(window);
    QString evidence = capabilityEvidence(false, true);
    evidence += QStringLiteral("Repair tool filesystem: available\n");
    cacheRepairEvidence(window, evidence);

    for (QCheckBox *toggle : {window.m_fullRepairDpkg, window.m_fullRepairBrokenPackages,
                              window.m_fullRepairAptUpdate, window.m_fullRepairUpgrade,
                              window.m_fullRepairDkms, window.m_fullRepairDisplayManager,
                              window.m_fullRepairInitramfs, window.m_fullRepairEfi,
                              window.m_fullRepairGrub}) {
        QVERIFY(toggle);
        toggle->setChecked(false);
    }
    QVERIFY(window.m_fullRepairFilesystem);
    window.m_fullRepairFilesystem->setChecked(true);
    window.updateFullRepairSummary();

    QVERIFY(window.m_fullRepairStageList);
    QCOMPARE(window.m_fullRepairStageList->count(), 1);
    QVERIFY2(window.m_fullRepairStageList->item(0)->text().contains(QStringLiteral("Repair file system errors")),
             "the file system stage must be the first Full Repair stage when enabled");
    QVERIFY2(window.m_fullRepairStageList->item(0)->text().startsWith(QStringLiteral("1. ")),
             "the file system stage must be ordered first in the execution plan");
    QCOMPARE(window.selectedRepairStages(), QStringList{QStringLiteral("filesystem")});

    // The execution list Full Repair sends to the helper must also start with
    // the file system stage; otherwise the inspect/confirm/repair flow is
    // skipped even though the plan lists it.
    window.m_fullRepairGrub->setChecked(true);
    window.updateFullRepairSummary();
    QCOMPARE(window.selectedRepairStages(),
             QStringList({QStringLiteral("filesystem"), QStringLiteral("grub")}));
    window.m_fullRepairGrub->setChecked(false);

    // Disabling the stage removes it from the plan.
    window.m_fullRepairFilesystem->setChecked(false);
    window.updateFullRepairSummary();
    QCOMPARE(window.m_fullRepairStageList->count(), 1);
    QVERIFY2(!window.m_fullRepairStageList->item(0)->text().contains(QStringLiteral("Repair file system errors")),
             "a disabled file system stage must not appear in the plan");
    QVERIFY(!window.selectedRepairStages().contains(QStringLiteral("filesystem")));
}

void MainWindowUiTest::fullRepairFilesystemCheckboxExistsAndPersists()
{
    {
        MainWindow window;
        window.show();
        QTest::qWait(50);
        QVERIFY(window.m_fullRepairFilesystem);
        QVERIFY2(window.m_fullRepairFilesystem->text().contains(QStringLiteral("Repair file system errors")),
                 "the Settings checkbox must be labelled for file system repair");
        QVERIFY2(window.m_fullRepairFilesystem->isChecked(),
                 "file system repair must default to enabled like the other safe stages");
        window.m_fullRepairFilesystem->setChecked(false);
        window.close();
        QTest::qWait(20);
    }

    {
        MainWindow window;
        window.show();
        QTest::qWait(50);
        QVERIFY(window.m_fullRepairFilesystem);
        QVERIFY2(!window.m_fullRepairFilesystem->isChecked(),
                 "the disabled file system repair preference must persist across windows");
        window.m_fullRepairFilesystem->setChecked(true);
        window.close();
        QTest::qWait(20);
    }
}

void MainWindowUiTest::filesystemCheckOutputParsesStableEvidence()
{
    const QString output = QStringLiteral(
        "File system check /dev/sda2: ext4 uuid=1111 mount=/ tool=e2fsck result=clean\n"
        "File system check detail /dev/sda2: tool=e2fsck output=clean\n"
        "File system check /dev/sda1: vfat uuid=ABCD mount=/boot/efi tool=fsck.fat result=issues\n"
        "File system check detail /dev/sda1: tool=fsck.fat output=Filesystem is not clean; errors found\n"
        "File system check /dev/sdb1: zzzfs uuid=none mount=unmounted tool=none result=unsupported\n"
        "File system check summary: devices=3 clean=1 issues=1 unsupported=1 tool-missing=0 skipped=0\n");
    const QList<FilesystemCheckResult> results = MainWindow::parseFilesystemCheckOutput(output);
    QCOMPARE(results.size(), 3);
    QCOMPARE(results.at(0).device, QStringLiteral("/dev/sda2"));
    QCOMPARE(results.at(0).fstype, QStringLiteral("ext4"));
    QCOMPARE(results.at(0).result, QStringLiteral("clean"));
    QCOMPARE(results.at(0).detail, QStringLiteral("clean"));
    QVERIFY(!results.at(0).needsRepair());
    QVERIFY(results.at(1).needsRepair());
    QCOMPARE(results.at(1).mount, QStringLiteral("/boot/efi"));
    QCOMPARE(results.at(1).detail, QStringLiteral("Filesystem is not clean; errors found"));
    QCOMPARE(results.at(2).result, QStringLiteral("unsupported"));
    QVERIFY(!results.at(2).needsRepair());

    // Repair outcomes and skipped inspections must never be re-offered as
    // issues; only result=issues is actionable.
    for (const QString &result : {QStringLiteral("repaired"), QStringLiteral("skipped"),
                                  QStringLiteral("tool-missing"), QStringLiteral("unreliable")}) {
        FilesystemCheckResult check;
        check.result = result;
        QVERIFY2(!check.needsRepair(), qPrintable(result));
    }
    FilesystemCheckResult issue;
    issue.result = QStringLiteral("issues");
    QVERIFY(issue.needsRepair());

    // A repair evidence line is not an inspection line and must not create a
    // repair candidate even when it carries result=issues.
    const QList<FilesystemCheckResult> repairLines = MainWindow::parseFilesystemCheckOutput(
        QStringLiteral("File system repair /dev/sda2: ext4 uuid=1111 mount=unmounted tool=e2fsck mode=repair result=issues\n"));
    QVERIFY2(repairLines.isEmpty(), "repair evidence must not be parsed as an inspection result");

    // Mode selection: the preferred repair mode is first and unsupported
    // filesystems still offer only the generic check/repair pair that the
    // helper validates.  Mounted filesystems never offer an offline repair:
    // only btrfs/zfs have an online mode, and f2fs is repair-only because it
    // has no read-only check mode.
    const QStringList extModes = MainWindow::filesystemRepairModes(QStringLiteral("ext4"), false);
    QCOMPARE(extModes.value(0), QStringLiteral("repair"));
    QVERIFY(extModes.contains(QStringLiteral("check")));
    const QStringList extMountedModes = MainWindow::filesystemRepairModes(QStringLiteral("ext4"), true);
    QCOMPARE(extMountedModes, QStringList{QStringLiteral("check")});
    const QStringList btrfsModes = MainWindow::filesystemRepairModes(QStringLiteral("btrfs"), false);
    QCOMPARE(btrfsModes.value(0), QStringLiteral("repair"));
    QVERIFY(btrfsModes.contains(QStringLiteral("rescue")));
    QVERIFY(btrfsModes.contains(QStringLiteral("scrub")));
    const QStringList btrfsMountedModes = MainWindow::filesystemRepairModes(QStringLiteral("btrfs"), true);
    QCOMPARE(btrfsMountedModes.value(0), QStringLiteral("scrub"));
    QVERIFY(!btrfsMountedModes.contains(QStringLiteral("repair")));
    const QStringList zfsModes = MainWindow::filesystemRepairModes(QStringLiteral("zfs"), true);
    QCOMPARE(zfsModes.value(0), QStringLiteral("scrub"));
    const QStringList f2fsModes = MainWindow::filesystemRepairModes(QStringLiteral("f2fs"), false);
    QCOMPARE(f2fsModes, QStringList{QStringLiteral("repair")});
    QVERIFY2(MainWindow::filesystemRepairModes(QStringLiteral("f2fs"), true).isEmpty(),
             "a mounted f2fs has no safe mode");
    const QStringList xfsModes = MainWindow::filesystemRepairModes(QStringLiteral("xfs"), false);
    QCOMPARE(xfsModes, QStringList({QStringLiteral("repair"), QStringLiteral("check")}));
}

void MainWindowUiTest::filesystemRepairDialogRowsTrackWrappedSummary()
{
    const QString longSummary = QStringLiteral(
        "e2fsck: Filesystem is not clean; errors found. The superblock reports 12345 "
        "inodes with zero dtime, 6789 blocks in use by deleted inodes and orphaned "
        "inode 4242. Additional detail: inode 98765 has an invalid extent tree and "
        "the directory entry for /var/lib/something points to an unallocated inode.");

    QList<FilesystemCheckResult> results;
    FilesystemCheckResult longIssue;
    longIssue.device = QStringLiteral("/dev/sda1");
    longIssue.fstype = QStringLiteral("vfat");
    longIssue.uuid = QStringLiteral("ABCD-1234");
    longIssue.mount = QStringLiteral("/boot/efi");
    longIssue.tool = QStringLiteral("fsck.fat");
    longIssue.result = QStringLiteral("issues");
    longIssue.detail = longSummary;
    results.append(longIssue);
    FilesystemCheckResult shortIssue;
    shortIssue.device = QStringLiteral("/dev/sda2");
    shortIssue.fstype = QStringLiteral("ext4");
    shortIssue.uuid = QStringLiteral("11111111-2222-3333-4444-555555555555");
    shortIssue.mount = QStringLiteral("unmounted");
    shortIssue.tool = QStringLiteral("e2fsck");
    shortIssue.result = QStringLiteral("issues");
    shortIssue.detail = QStringLiteral("short issue");
    results.append(shortIssue);

    const QList<QStringList> modes = {
        MainWindow::filesystemRepairModes(QStringLiteral("vfat"), true),
        MainWindow::filesystemRepairModes(QStringLiteral("ext4"), false)
    };

    FilesystemRepairDialog dialog(QStringLiteral("selected repair system"), results, modes);
    dialog.resize(900, 520);
    dialog.show();
    QCoreApplication::processEvents();
    QTest::qWait(50);

    QTableWidget *table = dialog.findChild<QTableWidget *>(QStringLiteral("filesystemRepairTable"));
    QVERIFY(table);
    QCOMPARE(table->rowCount(), 2);
    QCOMPARE(table->columnCount(), 5);
    QCOMPARE(table->verticalHeader()->sectionResizeMode(0), QHeaderView::ResizeToContents);
    QVERIFY(table->wordWrap());
    QVERIFY(table->alternatingRowColors());

    const QFontMetrics metrics(table->font());
    constexpr int summaryColumn = 4;
    const auto wrappedSummaryHeight = [&metrics, table, longSummary] {
        const int available = qMax(24, table->columnWidth(summaryColumn) - 8);
        return metrics.boundingRect(QRect(0, 0, available, 10000),
                                    Qt::TextWordWrap, longSummary).height();
    };

    const int wideRowHeight = table->rowHeight(0);
    QVERIFY2(wideRowHeight >= metrics.lineSpacing(),
             "the issue row must keep at least one line of height");
    QVERIFY2(wideRowHeight <= wrappedSummaryHeight() + 20,
             qPrintable(QStringLiteral("row height %1 exceeds the wrapped summary height %2")
                            .arg(wideRowHeight).arg(wrappedSummaryHeight())));
    QVERIFY2(wideRowHeight < 8 * metrics.lineSpacing(),
             qPrintable(QStringLiteral("the issue row is oversized at %1 px").arg(wideRowHeight)));
    QVERIFY2(table->rowHeight(1) < wideRowHeight,
             "a short summary must keep a shorter row than a long wrapped one");

    QTableWidgetItem *summaryItem = table->item(0, summaryColumn);
    QVERIFY(summaryItem);
    QCOMPARE(summaryItem->text(), longSummary);
    QVERIFY(summaryItem->toolTip().contains(QStringLiteral("Filesystem is not clean")));
    QCOMPARE(table->item(0, 2)->text(), QStringLiteral("vfat"));

    auto *mountedCombo = qobject_cast<QComboBox *>(table->cellWidget(0, 3));
    QVERIFY(mountedCombo);
    QCOMPARE(mountedCombo->count(), 1);
    QCOMPARE(mountedCombo->currentText(), QStringLiteral("check"));
    QCOMPARE(mountedCombo->sizePolicy().verticalPolicy(), QSizePolicy::Fixed);
    QVERIFY2(mountedCombo->height() <= wideRowHeight,
             "the mode combo must not stretch the row");
    auto *offlineCombo = qobject_cast<QComboBox *>(table->cellWidget(1, 3));
    QVERIFY(offlineCombo);
    QCOMPARE(offlineCombo->count(), 2);
    QCOMPARE(offlineCombo->currentText(), QStringLiteral("repair"));

    const QList<FilesystemRepairDialog::Selection> initialSelections = dialog.selections();
    QCOMPARE(initialSelections.size(), 2);
    QCOMPARE(initialSelections.at(0).device, QStringLiteral("/dev/sda1"));
    QCOMPARE(initialSelections.at(0).mode, QStringLiteral("check"));
    QCOMPARE(initialSelections.at(1).mode, QStringLiteral("repair"));

    dialog.resize(dialog.minimumWidth(), dialog.minimumHeight());
    QCoreApplication::processEvents();
    QTest::qWait(50);
    const int narrowRowHeight = table->rowHeight(0);
    QVERIFY2(narrowRowHeight >= wideRowHeight,
             "a narrower summary column must not shrink the wrapped row");
    QVERIFY2(narrowRowHeight <= wrappedSummaryHeight() + 20,
             qPrintable(QStringLiteral("narrow row height %1 exceeds the wrapped summary height %2")
                            .arg(narrowRowHeight).arg(wrappedSummaryHeight())));
    QDialogButtonBox *buttons = dialog.findChild<QDialogButtonBox *>();
    QVERIFY(buttons);
    QVERIFY2(buttons->isVisible(), "the dialog buttons must stay visible when narrow");

    QPushButton *repairButton = nullptr;
    for (QPushButton *button : dialog.findChildren<QPushButton *>()) {
        if (button->text() == QStringLiteral("Run Selected Repairs")) {
            repairButton = button;
            break;
        }
    }
    QVERIFY(repairButton);
    QVERIFY(repairButton->isEnabled());
    table->item(0, 0)->setCheckState(Qt::Unchecked);
    QCoreApplication::processEvents();
    QVERIFY2(!mountedCombo->isEnabled(), "an unchecked device must disable its mode combo");
    const QList<FilesystemRepairDialog::Selection> remaining = dialog.selections();
    QCOMPARE(remaining.size(), 1);
    QCOMPARE(remaining.first().device, QStringLiteral("/dev/sda2"));
    QCOMPARE(remaining.first().mode, QStringLiteral("repair"));
    table->item(1, 0)->setCheckState(Qt::Unchecked);
    QCoreApplication::processEvents();
    QVERIFY2(!repairButton->isEnabled(),
             "unchecking every device must disable the repair action");
    QVERIFY(dialog.selections().isEmpty());
}

void MainWindowUiTest::filesystemRepairFlowInspectsConfirmsAndRepairs()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareFilesystemPlan(window);

    // Run the stage through Full Repair itself so the plan entry, the
    // inspect/confirm/repair flow and the helper commands are exercised as one
    // user action.
    QCOMPARE(window.selectedRepairStages(), QStringList{QStringLiteral("filesystem")});

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("fs-requests.log"));
    QProcess *session = startFilesystemFakePrivilegedSession(window, capturePath);
    QVERIFY2(session, "the scripted privileged session must start");
    QVERIFY(window.m_privilegedSessionReady);

    bool sawDialog = false;
    int dialogRowHeight = 0;
    int dialogRowCount = 0;
    QString dialogDevice;
    QString dialogSummary;
    QStringList dialogModes;
    QTimer poll;
    poll.setInterval(5);
    QObject::connect(&poll, &QTimer::timeout, &window, [&] {
        for (QWidget *top : QApplication::topLevelWidgets()) {
            auto *dialog = qobject_cast<QDialog *>(top);
            if (!dialog || dialog->windowTitle() != QStringLiteral("Repair file system errors")) {
                continue;
            }
            auto *table = dialog->findChild<QTableWidget *>(QStringLiteral("filesystemRepairTable"));
            if (!table || table->rowCount() < 1) {
                continue;
            }
            sawDialog = true;
            dialogRowHeight = table->rowHeight(0);
            dialogRowCount = table->rowCount();
            dialogDevice = table->item(0, 1) ? table->item(0, 1)->text() : QString();
            dialogSummary = table->item(0, 4) ? table->item(0, 4)->text() : QString();
            if (auto *combo = qobject_cast<QComboBox *>(table->cellWidget(0, 3))) {
                for (int i = 0; i < combo->count(); ++i) {
                    dialogModes << combo->itemText(i);
                }
            }
            poll.stop();
            for (QPushButton *button : dialog->findChildren<QPushButton *>()) {
                if (button->text() == QStringLiteral("Run Selected Repairs")) {
                    button->click();
                    return;
                }
            }
            dialog->accept();
            return;
        }
    });
    poll.start();

    QTimer safety;
    safety.setSingleShot(true);
    safety.setInterval(5000);
    QObject::connect(&safety, &QTimer::timeout, &window, [&] {
        for (QWidget *top : QApplication::topLevelWidgets()) {
            auto *dialog = qobject_cast<QDialog *>(top);
            if (dialog && dialog->windowTitle() == QStringLiteral("Repair file system errors")) {
                dialog->reject();
            }
        }
    });
    safety.start();

    acceptNextMessageBoxes(&window, 2, QMessageBox::Yes);

    window.runFullRepair();
    QVERIFY2(sawDialog, "the file system repair confirmation dialog must be shown");
    QCOMPARE(dialogRowCount, 1);
    QCOMPARE(dialogDevice, QStringLiteral("/dev/test-root"));
    QVERIFY2(dialogSummary.contains(QStringLiteral("Filesystem is not clean")),
             "the dialog must carry the helper's issue summary");
    QCOMPARE(dialogModes, QStringList({QStringLiteral("repair"), QStringLiteral("check")}));
    QVERIFY2(dialogRowHeight > 0, "the issue row must be measured");
    QVERIFY2(dialogRowHeight < 200, "the issue row must stay content-sized");

    const QStringList requests = capturedHelperArguments(capturePath);
    QVERIFY2(requests.contains(QStringLiteral("fs-inspect")),
             "the flow must run the read-only inspection first");
    QVERIFY2(requests.contains(QStringLiteral("fs-repair")),
             "the flow must run the confirmed repair");
    QVERIFY2(requests.contains(QStringLiteral("/dev/test-root")),
             "the inspection and repair must name the selected device");

    const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QVERIFY(log.contains(QStringLiteral("──────── REPAIR ────────")));
    QVERIFY(log.contains(QStringLiteral("Starting read-only file system check")));
    QVERIFY(log.contains(QStringLiteral("File system check output")));
    QVERIFY(log.contains(QStringLiteral("File system repair output for /dev/test-root (repair)")));
}

void MainWindowUiTest::filesystemRepairFlowUsesHostCommandsInHostMode()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());
    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareRepairScope(window, /*host=*/true);
    window.m_snapshotPreloadScheduled = true;
    QString evidence = capabilityEvidence(false, true);
    evidence += QStringLiteral("Repair tool filesystem: available\n");
    cacheRepairEvidence(window, evidence);

    for (QCheckBox *toggle : {window.m_fullRepairDpkg, window.m_fullRepairBrokenPackages,
                              window.m_fullRepairAptUpdate, window.m_fullRepairUpgrade,
                              window.m_fullRepairDkms, window.m_fullRepairDisplayManager,
                              window.m_fullRepairInitramfs, window.m_fullRepairEfi,
                              window.m_fullRepairGrub}) {
        QVERIFY(toggle);
        toggle->setChecked(false);
    }
    QVERIFY(window.m_fullRepairFilesystem);
    window.m_fullRepairFilesystem->setChecked(true);
    window.updateFullRepairSummary();
    QCOMPARE(window.selectedRepairStages(), QStringList{QStringLiteral("filesystem")});

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("host-fs-requests.log"));
    QProcess *session = startFilesystemFakePrivilegedSession(window, capturePath);
    QVERIFY2(session, "the scripted privileged session must start");
    QVERIFY(window.m_privilegedSessionReady);

    QTimer poll;
    poll.setInterval(5);
    QObject::connect(&poll, &QTimer::timeout, &window, [&] {
        for (QWidget *top : QApplication::topLevelWidgets()) {
            auto *dialog = qobject_cast<QDialog *>(top);
            if (!dialog || !dialog->isVisible()
                || dialog->windowTitle() != QStringLiteral("Repair file system errors")) {
                continue;
            }
            auto *table = dialog->findChild<QTableWidget *>(QStringLiteral("filesystemRepairTable"));
            if (!table || table->rowCount() < 1) {
                continue;
            }
            poll.stop();
            for (QPushButton *button : dialog->findChildren<QPushButton *>()) {
                if (button->text() == QStringLiteral("Run Selected Repairs")) {
                    button->click();
                    return;
                }
            }
            dialog->accept();
        }
    });
    poll.start();

    acceptNextMessageBoxes(&window, 2, QMessageBox::Yes);

    window.runFullRepair();

    const QStringList requests = capturedHelperArguments(capturePath);
    QVERIFY2(requests.contains(QStringLiteral("host-fs-inspect")),
             "host maintenance must inspect through host-fs-inspect");
    QVERIFY2(requests.contains(QStringLiteral("host-fs-repair")),
             "host maintenance must repair through host-fs-repair");
    QVERIFY2(!requests.contains(QStringLiteral("fs-inspect")),
             "host maintenance must not send the target inspection command");
    QVERIFY2(!requests.contains(QStringLiteral("fs-repair")),
             "host maintenance must not send the target repair command");
}

void MainWindowUiTest::fullRepairSkippedFilesystemPreStageIsNotCountedAsClean()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());
    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareFilesystemPlan(window);
    QCOMPARE(window.selectedRepairStages(), QStringList{QStringLiteral("filesystem")});

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("skipped-fs-requests.log"));
    QProcess *session = startFilesystemFakePrivilegedSession(window, capturePath,
                                                             QStringLiteral("ext4"), /*issue=*/false);
    QVERIFY2(session, "the scripted privileged session must start");
    QVERIFY(window.m_privilegedSessionReady);

    QStringList messageTitles;
    QTimer poll;
    poll.setInterval(5);
    QObject::connect(&poll, &QTimer::timeout, &window, [&] {
        for (QWidget *top : QApplication::topLevelWidgets()) {
            auto *box = qobject_cast<QMessageBox *>(top);
            if (!box || !box->isVisible()) {
                continue;
            }
            messageTitles.append(box->windowTitle());
            if (QAbstractButton *yes = box->button(QMessageBox::Yes)) {
                yes->click();
            } else {
                box->accept();
            }
            return;
        }
    });
    poll.start();

    window.runFullRepair();
    poll.stop();

    QVERIFY2(!messageTitles.contains(QStringLiteral("File system repair")),
             "a skipped inspection must be log-only during a plan, not a modal notice");
    const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QVERIFY2(log.contains(QStringLiteral("No repairable file system errors were detected")),
             "the skipped inspection result must still be recorded in the plan log");
    QVERIFY2(log.contains(QStringLiteral("skipped")),
             "a fully skipped inspection must not be reported as a clean filesystem");
    const QStringList requests = capturedHelperArguments(capturePath);
    QVERIFY2(requests.contains(QStringLiteral("fs-inspect")),
             "the read-only inspection must still have run");
    QVERIFY2(!requests.contains(QStringLiteral("fs-repair")),
             "a skipped inspection must not offer or run a repair");

    // --- merged from fullRepairSkippedFilesystemPreStageIsNotCountedAsNoRepairNeeded() ---
    {
        ScopedSessionLogDir logDir;
        QVERIFY(logDir.isValid());

        MainWindow window;
        window.show();
        QTest::qWait(50);
        prepareFilesystemPlan(window);
        QVERIFY(window.m_fullRepairDpkg);
        window.m_fullRepairDpkg->setChecked(true);
        window.updateFullRepairSummary();
        QCOMPARE(window.selectedRepairStages(),
                 QStringList({QStringLiteral("filesystem"), QStringLiteral("dpkg-configure")}));

        QTemporaryDir requestDir;
        QVERIFY(requestDir.isValid());
        const QString capturePath = requestDir.filePath(QStringLiteral("plan-fs-skipped-requests.log"));
        QProcess *session = startFilesystemFakePrivilegedSession(window, capturePath,
                                                                 QStringLiteral("ext4"), /*issue=*/false);
        QVERIFY2(session, "the scripted privileged session must start");
        QVERIFY(window.m_privilegedSessionReady);

        closeRepairProgressDialogWhenDone(&window);
        // The skipped pre-stage is log-only inside the plan: only the plan
        // confirmation remains modal.
        acceptNextMessageBoxes(&window, 1, QMessageBox::Yes);

        window.runFullRepair();

        QVERIFY2(!window.m_fullRepairPlanInProgress, "the plan must finish");
        const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
        QVERIFY2(log.contains(QStringLiteral(
                     "Full Repair results: ✓ 1 successful · ✗ 0 failed · ▪ 0 no repair needed · "
                     "▪ 1 not checked — see the filesystem stage line — changes were applied.")),
                 "the aggregate must count the skipped filesystem in its own category "
                 "and reference the stage line that lists the skipped device");
        QVERIFY2(!log.contains(QStringLiteral("▪ 1 no repair needed")),
                 "a skipped filesystem must never be counted as no-repair-needed");
        QVERIFY2(log.contains(QStringLiteral(
                     "  ▪ filesystem — not all filesystems were checked — "
                     "1 skipped (mounted, offline-only check): /dev/test-root ext4")),
                 "the skipped filesystem and its device must be auditable in the summary");
        QVERIFY2(log.contains(QStringLiteral("  ✓ dpkg — package configuration completed — changes were applied")),
                 "the remaining plan stages must still run and be auditable");
    }
}


void MainWindowUiTest::btrfsRepairRequiresExtraDangerConfirmation()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());
    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareFilesystemPlan(window);
    QCOMPARE(window.selectedRepairStages(), QStringList{QStringLiteral("filesystem")});

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("btrfs-fs-requests.log"));
    QProcess *session = startFilesystemFakePrivilegedSession(window, capturePath, QStringLiteral("btrfs"));
    QVERIFY2(session, "the scripted privileged session must start");
    QVERIFY(window.m_privilegedSessionReady);

    bool sawDanger = false;
    QString dangerText;
    QTimer poll;
    poll.setInterval(5);
    QObject::connect(&poll, &QTimer::timeout, &window, [&] {
        for (QWidget *top : QApplication::topLevelWidgets()) {
            if (auto *box = qobject_cast<QMessageBox *>(top)) {
                if (!box->isVisible()) {
                    continue;
                }
                if (box->windowTitle() == QStringLiteral("Dangerous Btrfs repair")) {
                    sawDanger = true;
                    dangerText = box->text() + QLatin1Char('\n') + box->informativeText();
                    poll.stop();
                    box->reject();
                    return;
                }
                if (box->windowTitle() == QStringLiteral("Run Full Repair")
                    || box->windowTitle() == QStringLiteral("Repair file system errors")) {
                    if (QAbstractButton *yes = box->button(QMessageBox::Yes)) {
                        yes->click();
                    }
                    return;
                }
                continue;
            }
            auto *dialog = qobject_cast<QDialog *>(top);
            if (!dialog || !dialog->isVisible()
                || dialog->windowTitle() != QStringLiteral("Repair file system errors")) {
                continue;
            }
            auto *table = dialog->findChild<QTableWidget *>(QStringLiteral("filesystemRepairTable"));
            if (!table || table->rowCount() < 1) {
                continue;
            }
            for (QPushButton *button : dialog->findChildren<QPushButton *>()) {
                if (button->text() == QStringLiteral("Run Selected Repairs")) {
                    button->click();
                    return;
                }
            }
        }
    });
    poll.start();

    QTimer safety;
    safety.setSingleShot(true);
    safety.setInterval(5000);
    QObject::connect(&safety, &QTimer::timeout, &window, [&] {
        for (QWidget *top : QApplication::topLevelWidgets()) {
            if (auto *box = qobject_cast<QMessageBox *>(top)) {
                if (box->isVisible()) {
                    box->reject();
                }
                continue;
            }
            auto *dialog = qobject_cast<QDialog *>(top);
            if (dialog && dialog->windowTitle() == QStringLiteral("Repair file system errors")) {
                dialog->reject();
            }
        }
    });
    safety.start();

    window.runFullRepair();

    QVERIFY2(sawDanger, "btrfs check --repair must require the extra danger confirmation");
    QVERIFY2(dangerText.contains(QStringLiteral("last-resort")),
             "the danger confirmation must name the operation");
    QVERIFY2(dangerText.contains(QStringLiteral("Back up")),
             "the danger confirmation must require a backup");

    const QStringList requests = capturedHelperArguments(capturePath);
    QVERIFY2(requests.contains(QStringLiteral("fs-inspect")),
             "the inspection must still have run");
    QVERIFY2(!requests.contains(QStringLiteral("fs-repair")),
             "cancelling the danger confirmation must not run btrfs check --repair");
    const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QVERIFY2(log.contains(QStringLiteral("cancelled at the extra danger confirmation")),
             "the cancellation must be recorded in the repair log");
}

void MainWindowUiTest::fullRepairUsesFreshEvidenceForSelectedStage()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    DeviceNode component;
    component.path = QStringLiteral("/dev/test-root");
    component.type = QStringLiteral("part");
    component.fileSystem = QStringLiteral("ext4");
    component.linuxCapableFileSystem = true;
    component.installedLinux = true;
    DeviceNode disk;
    disk.path = QStringLiteral("/dev/test-system");
    disk.type = QStringLiteral("disk");
    disk.children.append(component);
    window.m_deviceIndex.insert(disk.path, disk);
    window.m_deviceIndex.insert(component.path, component);
    window.m_previewTargetPath = disk.path;
    window.m_previewTargetComponentPath = component.path;
    window.m_targetDiagnosticCacheIdentity = disk.path;
    window.m_targetDiagnosticCache.insert(QStringLiteral("display"), QStringLiteral("Diagnostic: display\nRepair tool display: available\nPASS"));

    QVERIFY(window.m_fullRepairDisplayManager);
    window.m_fullRepairDpkg->setChecked(false);
    window.m_fullRepairBrokenPackages->setChecked(false);
    window.m_fullRepairAptUpdate->setChecked(false);
    window.m_fullRepairUpgrade->setChecked(false);
    window.m_fullRepairDkms->setChecked(false);
    window.m_fullRepairDisplayManager->setChecked(true);
    window.m_fullRepairInitramfs->setChecked(false);
    window.m_fullRepairEfi->setChecked(false);
    window.m_fullRepairGrub->setChecked(false);
    window.updateFullRepairSummary();

    QVERIFY2(window.m_runFullRepairButton->isEnabled(),
             "Full Repair should use fresh evidence for the selected display-manager stage");
}

void MainWindowUiTest::fullRepairPlanSchedulesSingleEvidenceRegeneration()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    // finishFullRepairPlan() rescans devices before scheduling the single
    // end-of-plan regeneration. Point the UI-test scanner at a fake lsblk that
    // still exposes the committed target so the target-scope readiness gate
    // holds after the refresh, exactly like a production rescan of the real
    // repair drive.
    QTemporaryDir scanDir;
    QVERIFY(scanDir.isValid());
    const QString lsblkPath = scanDir.filePath(QStringLiteral("lsblk"));
    {
        QFile lsblk(lsblkPath);
        QVERIFY(lsblk.open(QIODevice::WriteOnly | QIODevice::Text));
        lsblk.write(R"LSBLK(#!/bin/sh
cat <<'JSON'
{
   "blockdevices": [
      {
         "name": "test-system",
         "kname": "test-system",
         "path": "/dev/test-system",
         "maj:min": "253:0",
         "type": "disk",
         "size": 21474836480,
         "fstype": null,
         "mountpoints": [],
         "children": [
            {
               "name": "test-root",
               "kname": "test-root",
               "path": "/dev/test-root",
               "maj:min": "253:1",
               "type": "part",
               "size": 17024679936,
               "fstype": "ext4",
               "mountpoints": []
            }
         ]
      }
   ]
}
JSON
)LSBLK");
        lsblk.close();
        QVERIFY(QFile::setPermissions(lsblkPath,
                QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner
                | QFileDevice::ReadGroup | QFileDevice::ExeGroup
                | QFileDevice::ReadOther | QFileDevice::ExeOther));
    }
    ScopedEnvironmentVariable lsblkOverride("BOOT_REPAIR_LSBLK", lsblkPath.toLocal8Bit());
    prepareRepairScope(window);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(true);
    QString evidence = capabilityEvidence(false, true);
    evidence += QStringLiteral("Repair tool filesystem: available\n");
    cacheRepairEvidence(window, evidence);

    for (QCheckBox *toggle : {window.m_fullRepairDpkg, window.m_fullRepairBrokenPackages,
                              window.m_fullRepairAptUpdate, window.m_fullRepairUpgrade,
                              window.m_fullRepairDkms, window.m_fullRepairDisplayManager,
                              window.m_fullRepairInitramfs, window.m_fullRepairEfi,
                              window.m_fullRepairGrub}) {
        QVERIFY(toggle);
        toggle->setChecked(false);
    }
    QVERIFY(window.m_fullRepairFilesystem);
    window.m_fullRepairFilesystem->setChecked(true);
    window.m_fullRepairDpkg->setChecked(true);
    window.updateFullRepairSummary();
    QCOMPARE(window.selectedRepairStages(),
             QStringList({QStringLiteral("filesystem"), QStringLiteral("dpkg-configure")}));

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("full-plan-requests.log"));
    QVERIFY2(startFilesystemFakePrivilegedSession(window, capturePath),
             "the scripted privileged session must start");
    closeRepairProgressDialogWhenDone(&window);

    // The file system pre-stage is inspect-first: confirm the inspected
    // device selection through its dialog, then the plan confirmation and the
    // file system confirmation message boxes.
    QTimer poll;
    poll.setInterval(5);
    QObject::connect(&poll, &QTimer::timeout, &window, [&] {
        for (QWidget *top : QApplication::topLevelWidgets()) {
            auto *dialog = qobject_cast<QDialog *>(top);
            if (!dialog || !dialog->isVisible()
                || dialog->windowTitle() != QStringLiteral("Repair file system errors")) {
                continue;
            }
            auto *table = dialog->findChild<QTableWidget *>(QStringLiteral("filesystemRepairTable"));
            if (!table || table->rowCount() < 1) {
                continue;
            }
            poll.stop();
            for (QPushButton *button : dialog->findChildren<QPushButton *>()) {
                if (button->text() == QStringLiteral("Run Selected Repairs")) {
                    button->click();
                    return;
                }
            }
            dialog->accept();
        }
    });
    poll.start();
    acceptNextMessageBoxes(&window, 2, QMessageBox::Yes);

    window.runFullRepair();

    QVERIFY2(!window.m_fullRepairPlanInProgress,
             "the plan guard must be released after the last stage");
    QVERIFY2(window.m_fullRepairPlanModified,
             "the plan must remember that a modifying stage ran");
    QVERIFY2(window.m_targetDiagnosticsNeedRegeneration,
             "the completed plan must invalidate the approved evidence");

    // One stale notice and one coalesced regeneration for the whole plan; the
    // file system pre-stage must not schedule its own intermediate run.
    const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QCOMPARE(log.count(QStringLiteral("STALE DIAGNOSTICS")), 1);
    QCOMPARE(log.count(QStringLiteral(
                 "Automatic read-only diagnostics regeneration scheduled after Full Repair plan completed.")), 1);
    QCOMPARE(log.count(QStringLiteral("Automatically regenerating read-only diagnostics")), 0);
    // The register is newest-first, so the final stale notice must precede the
    // Full Repair stage block, which must precede the file system pre-stage.
    const int fsRepairIndex = log.indexOf(QStringLiteral("File system repair output for /dev/test-root"));
    const int fullRepairIndex = log.indexOf(QStringLiteral("Starting privileged action: Full Repair"));
    const int staleIndex = log.indexOf(QStringLiteral("STALE DIAGNOSTICS"));
    QVERIFY2(fsRepairIndex >= 0, "the file system pre-stage must be logged");
    QVERIFY2(fullRepairIndex >= 0 && fullRepairIndex < fsRepairIndex,
             "the remaining stages must run after the file system pre-stage");
    QVERIFY2(staleIndex >= 0 && staleIndex < fullRepairIndex,
             "the single stale notice must come after the final stage, not between stages");

    const QStringList requests = capturedHelperArguments(capturePath);
    QVERIFY2(requests.contains(QStringLiteral("fs-inspect")),
             "the plan must inspect the file systems first");
    QVERIFY2(requests.contains(QStringLiteral("fs-repair")),
             "the plan must run the confirmed file system repair");
    QVERIFY2(requests.contains(QStringLiteral("dpkg-configure")),
             "the plan must still run its remaining stages after the file system repair");
    // The aggregate covers every plan stage, including the file system
    // pre-stage whose detailed output lives in its own section. Both stages
    // completed without a change-status line here, which stays fail-safe
    // successful exactly like the invalidation decision.
    QVERIFY2(log.contains(QStringLiteral(
                 "Full Repair results: ✓ 2 successful · ✗ 0 failed · ▪ 0 no repair needed")),
             "the plan aggregate must include the file system pre-stage");
    QVERIFY2(log.contains(QStringLiteral(
                 "  ✓ filesystem — file system repaired — changes were applied — 1 file system(s) with issues")),
             "the file system pre-stage must be auditable in the plan summary");
    QVERIFY2(log.contains(QStringLiteral("  ✓ dpkg — package configuration completed — changes were applied")),
             "the dpkg stage must be auditable in the plan summary");
}

void MainWindowUiTest::manualRepairAfterModificationStillReportsStale()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareRepairScope(window, true);
    window.m_snapshotPreloadScheduled = true;
    // Keep the automatic regeneration out of the way so the between-run stale
    // gate is observed directly instead of being repaired by a refresh.
    window.m_autoRefreshDiagnostics->setChecked(false);
    cacheRepairEvidence(window, capabilityEvidence(false, true));

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("manual-stale-requests.log"));
    QVERIFY2(startRepairFakePrivilegedSession(window, capturePath),
             "the scripted privileged session must start");
    closeRepairProgressDialogWhenDone(&window);

    window.runRepairHelper(QStringLiteral("Rebuild DKMS"),
                           {QStringLiteral("repair"), QString(), QString(), QStringLiteral("dkms")});
    const QString firstLog = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QCOMPARE(firstLog.count(QStringLiteral("STALE DIAGNOSTICS")), 1);
    QVERIFY2(window.m_hostDiagnosticCache.value(QStringLiteral("kernel")).trimmed().isEmpty(),
             "the DKMS mapping must invalidate the kernel section");
    QVERIFY2(window.m_hostDiagnosticsStaleSections.contains(QStringLiteral("kernel")),
             "the DKMS mapping must mark the kernel section stale");

    // A second manual tool run still sees stale evidence: the Full Repair
    // optimization must never weaken the between-run stale gate. The stale
    // notice is a single replaceable status, so the second run updates the
    // existing entry instead of appending a copy.
    prepareRepairScope(window, true);
    closeRepairProgressDialogWhenDone(&window);
    window.runRepairHelper(QStringLiteral("Regenerate GRUB configuration"),
                           {QStringLiteral("repair"), QString(), QString(), QStringLiteral("grub")});
    const QString secondLog = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QCOMPARE(secondLog.count(QStringLiteral("STALE DIAGNOSTICS")), 1);
    QVERIFY2(window.m_hostDiagnosticCache.value(QStringLiteral("grub")).trimmed().isEmpty(),
             "the second manual run must still invalidate its mapped cached section");
    QVERIFY2(window.currentScopeEvidenceStale(),
             "the between-run stale gate must stay in effect for the invalidated sections");
}

void MainWindowUiTest::bootStackReusesEfiRepairFromSameSession()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareRepairScope(window);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);
    const QString evidence = capabilityEvidence(false, true);
    cacheRepairEvidence(window, evidence);

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("bootstack-reuse-requests.log"));
    QVERIFY2(startRepairFakePrivilegedSession(window, capturePath),
             "the scripted privileged session must start");
    closeRepairProgressDialogWhenDone(&window);

    // The EFI / UKI stage repairs the boot path and records the reuse claim.
    window.runRepairHelper(QStringLiteral("Repair EFI / UKI bootloader"),
                           {QStringLiteral("repair"), QString(), QString(), QStringLiteral("efi")});
    QVERIFY2(window.m_efiBootloaderRepaired,
             "a successful EFI/UKI stage must enable the reuse claim");
    QVERIFY(window.efiRepairReuseAvailable());

    // Reconcile boot stack is offered with the explicit hint and its label
    // states the relationship before the confirmation.
    prepareRepairScope(window);
    cacheRepairEvidence(window, evidence);
    QTreeWidgetItem *bootstackItem = repairItem(window, QStringLiteral("bootstack"));
    QVERIFY(bootstackItem);
    window.m_repairToolTree->setCurrentItem(bootstackItem);
    window.updateRepairToolDetails();
    QVERIFY2(window.m_repairToolPlanStatus->text().contains(QStringLiteral("Reuse:")),
             "the boot-stack label must state that the EFI/UKI repair is reused");
    QVERIFY2(window.m_repairToolDescription->text().contains(QStringLiteral("reuses it")),
             "the boot-stack description must document the reuse relationship");

    closeRepairProgressDialogWhenDone(&window);
    acceptNextMessageBox(&window, QMessageBox::Yes);
    window.runSelectedRepairTool();

    QStringList requests = capturedHelperArguments(capturePath);
    QVERIFY2(requests.contains(QStringLiteral("boot-stack")),
             "the boot-stack stage must be dispatched");
    QCOMPARE(requests.count(QStringLiteral("--post-efi")), 1);

    // Any other modifying action invalidates the claim, and a later
    // boot-stack run performs the complete reconciliation again.
    prepareRepairScope(window);
    cacheRepairEvidence(window, evidence);
    closeRepairProgressDialogWhenDone(&window);
    window.runRepairHelper(QStringLiteral("Rebuild DKMS"),
                           {QStringLiteral("repair"), QString(), QString(), QStringLiteral("dkms")});
    QVERIFY2(!window.m_efiBootloaderRepaired,
             "a non-boot modifying stage must drop the reuse claim");
    QVERIFY(!window.efiRepairReuseAvailable());

    prepareRepairScope(window);
    cacheRepairEvidence(window, evidence);
    bootstackItem = repairItem(window, QStringLiteral("bootstack"));
    QVERIFY(bootstackItem);
    window.m_repairToolTree->setCurrentItem(bootstackItem);
    window.updateRepairToolDetails();
    QVERIFY2(!window.m_repairToolPlanStatus->text().contains(QStringLiteral("Reuse:")),
             "the reuse label must disappear once the claim is invalidated");
    closeRepairProgressDialogWhenDone(&window);
    acceptNextMessageBox(&window, QMessageBox::Yes);
    window.runSelectedRepairTool();
    requests = capturedHelperArguments(capturePath);
    QCOMPARE(requests.count(QStringLiteral("--post-efi")), 1);
}

void MainWindowUiTest::displayManagerToolUsesGenericLanguage()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    QVERIFY(window.m_repairToolTree);
    QTreeWidgetItem *displayItem = nullptr;
    for (int row = 0; row < window.m_repairToolTree->topLevelItemCount(); ++row) {
        auto *item = window.m_repairToolTree->topLevelItem(row);
        if (item->data(0, Qt::UserRole).toString() == QStringLiteral("display")) {
            displayItem = item;
            break;
        }
    }
    QVERIFY(displayItem);
    QVERIFY(displayItem->text(0).contains(QStringLiteral("display manager"), Qt::CaseInsensitive));
    QVERIFY(!displayItem->text(0).contains(QStringLiteral("SDDM"), Qt::CaseInsensitive));

    window.m_repairToolTree->setCurrentItem(displayItem);
    window.updateRepairToolDetails();
    QVERIFY(window.m_repairToolTitle);
    QVERIFY(window.m_repairToolDescription);
    QVERIFY(window.m_repairToolTitle->text().contains(QStringLiteral("display manager"), Qt::CaseInsensitive));
    const QString description = window.m_repairToolDescription->text();
    QVERIFY(description.contains(QStringLiteral("SDDM")));
    QVERIFY(description.contains(QStringLiteral("GDM3")));
    QVERIFY(description.contains(QStringLiteral("LightDM")));
    QVERIFY(description.contains(QStringLiteral("boot evidence"), Qt::CaseInsensitive));
}

void MainWindowUiTest::repairInvalidationExplainsDiagnosticRerun()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    DeviceNode component;
    component.path = QStringLiteral("/dev/test-root");
    component.type = QStringLiteral("part");
    component.fileSystem = QStringLiteral("ext4");
    component.linuxCapableFileSystem = true;
    component.installedLinux = true;
    DeviceNode disk;
    disk.path = QStringLiteral("/dev/test-system");
    disk.type = QStringLiteral("disk");
    disk.children.append(component);
    window.m_deviceIndex.insert(disk.path, disk);
    window.m_deviceIndex.insert(component.path, component);
    window.m_previewTargetPath = disk.path;
    window.m_previewTargetComponentPath = component.path;
    window.m_targetDiagnosticCacheIdentity = disk.path;
    window.m_targetDiagnosticsNeedRegeneration = true;

    window.updateFullRepairSummary();
    QVERIFY(window.m_runFullRepairButton);
    QVERIFY(!window.m_runFullRepairButton->isEnabled());
    QVERIFY(window.m_fullRepairReadinessLabel);
    QVERIFY(window.m_fullRepairReadinessLabel->text().contains(
        QStringLiteral("after the last diagnostic or repair action")));
    QVERIFY(window.m_fullRepairReadinessLabel->text().contains(
        QStringLiteral("regenerate diagnostics")));
}

void MainWindowUiTest::logSearchFiltersApplicationLog()
{
    MainWindow window;
    window.show();
    QTest::qWait(100);

    QVERIFY(window.m_logView);
    QVERIFY(window.m_logSearchEdit);

    const QString matching = QStringLiteral("UI_TEST_FuzzyDiagnosticMarker");
    const QString unrelated = QStringLiteral("UI_TEST_UnrelatedMarker");
    window.appendLog(matching);
    window.appendLog(unrelated);
    flushLogRefresh(window);
    QVERIFY(window.m_logView->toPlainText().contains(matching));
    QVERIFY(window.m_logView->toPlainText().contains(unrelated));

    // The characters f-z-y occur in order in “Fuzzy”, so this exercises the
    // forgiving subsequence matcher rather than an exact substring search.
    window.m_logSearchEdit->setText(QStringLiteral("fzy diagnostic"));
    const QString filtered = window.m_logView->toPlainText();
    QVERIFY(filtered.contains(matching));
    QVERIFY(!filtered.contains(unrelated));

    window.m_logSearchEdit->clear();
    const QString restored = window.m_logView->toPlainText();
    QVERIFY(restored.contains(matching));
    QVERIFY(restored.contains(unrelated));
}

void MainWindowUiTest::logSectionSearchShowsWholeDiagnosticSection()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window, true);
    enterHostDiagnosticScope(window);

    const int errorsRow = diagnosticRow(window, QStringLiteral("errors"));
    const int environmentRow = diagnosticRow(window, QStringLiteral("environment"));
    QVERIFY(errorsRow >= 0);
    QVERIFY(environmentRow >= 0);
    window.m_diagnosticList->setCurrentRow(errorsRow);
    window.runSelectedDiagnostic();
    window.m_diagnosticList->setCurrentRow(environmentRow);
    window.runSelectedDiagnostic();
    window.appendLog(QStringLiteral("UI_TEST_UNRELATED_SECTION_MARKER"));
    flushLogRefresh(window);
    QVERIFY(window.m_logView->toPlainText().contains(QStringLiteral("Diagnostic: environment")));

    window.m_logSearchEdit->setText(QStringLiteral("@errors"));
    const QString filtered = window.m_logView->toPlainText();
    QVERIFY2(filtered.contains(QStringLiteral("Diagnostic: errors")),
             "@errors must select the boot-errors section");
    QVERIFY2(filtered.contains(QStringLiteral("Title: Boot errors")),
             "the complete section framing must be shown");
    QVERIFY2(filtered.contains(QStringLiteral("journalctl")),
             "the complete section body must be shown, not only matching lines");
    QVERIFY2(!filtered.contains(QStringLiteral("Diagnostic: environment")),
             "unrelated diagnostic sections must be hidden");
    QVERIFY2(!filtered.contains(QStringLiteral("UI_TEST_UNRELATED_SECTION_MARKER")),
             "unrelated entries must be hidden");

    // Other terms keep the current fuzzy behavior and combine as AND with the
    // selected section.
    window.m_logSearchEdit->setText(QStringLiteral("@errors journalctl"));
    QVERIFY(window.m_logView->toPlainText().contains(QStringLiteral("Diagnostic: errors")));
    window.m_logSearchEdit->setText(QStringLiteral("@errors no-such-marker"));
    QVERIFY(!window.m_logView->toPlainText().contains(QStringLiteral("Diagnostic: errors")));
    window.m_logSearchEdit->clear();
    QVERIFY(window.m_logView->toPlainText().contains(QStringLiteral("UI_TEST_UNRELATED_SECTION_MARKER")));
}

void MainWindowUiTest::logFilterDropdownFiltersDiagnosticsRepairsAndSections()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window, true);
    enterHostDiagnosticScope(window);

    const int errorsRow = diagnosticRow(window, QStringLiteral("errors"));
    const int environmentRow = diagnosticRow(window, QStringLiteral("environment"));
    QVERIFY(errorsRow >= 0);
    QVERIFY(environmentRow >= 0);
    window.m_diagnosticList->setCurrentRow(errorsRow);
    window.runSelectedDiagnostic();
    window.m_diagnosticList->setCurrentRow(environmentRow);
    window.runSelectedDiagnostic();
    const QString repairMarker = QStringLiteral("UI_TEST_REPAIR_FILTER_MARKER");
    window.appendLog(QStringLiteral("Repair output\n%1").arg(repairMarker),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    flushLogRefresh(window);

    QVERIFY(window.m_logFilterCombo);
    const int allIndex = window.m_logFilterCombo->findData(QStringLiteral("all"));
    const int diagnosticsIndex = window.m_logFilterCombo->findData(QStringLiteral("diagnostics"));
    const int repairsIndex = window.m_logFilterCombo->findData(QStringLiteral("repairs"));
    const int errorsIndex = window.m_logFilterCombo->findData(QStringLiteral("@errors"));
    QVERIFY(allIndex >= 0);
    QVERIFY(diagnosticsIndex >= 0);
    QVERIFY(repairsIndex >= 0);
    QVERIFY(errorsIndex >= 0);

    window.m_logFilterCombo->setCurrentIndex(diagnosticsIndex);
    QString visible = window.m_logView->toPlainText();
    QVERIFY(visible.contains(QStringLiteral("Diagnostic: errors")));
    QVERIFY(visible.contains(QStringLiteral("Diagnostic: environment")));
    QVERIFY(!visible.contains(repairMarker));

    window.m_logFilterCombo->setCurrentIndex(repairsIndex);
    visible = window.m_logView->toPlainText();
    QVERIFY(visible.contains(repairMarker));
    QVERIFY(!visible.contains(QStringLiteral("Diagnostic: errors")));
    QVERIFY(!visible.contains(QStringLiteral("Diagnostic: environment")));

    // Selecting a diagnostic section by name shows that complete section.
    window.m_logFilterCombo->setCurrentIndex(errorsIndex);
    visible = window.m_logView->toPlainText();
    QVERIFY(visible.contains(QStringLiteral("Diagnostic: errors")));
    QVERIFY(visible.contains(QStringLiteral("journalctl")));
    QVERIFY(!visible.contains(QStringLiteral("Diagnostic: environment")));
    QVERIFY(!visible.contains(repairMarker));

    // Search keeps working together with the dropdown filter.
    window.m_logSearchEdit->setText(QStringLiteral("environment"));
    visible = window.m_logView->toPlainText();
    QVERIFY(!visible.contains(QStringLiteral("Diagnostic: errors")));
    window.m_logSearchEdit->clear();

    window.m_logFilterCombo->setCurrentIndex(allIndex);
    visible = window.m_logView->toPlainText();
    QVERIFY(visible.contains(repairMarker));
    QVERIFY(visible.contains(QStringLiteral("Diagnostic: errors")));
}

void MainWindowUiTest::logFilterTracksHostDkmsRepairCycleAndDiagnosticsRegeneration()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window, true);
    enterHostDiagnosticScope(window);
    // This test regenerates diagnostics explicitly; the automatic refresh is
    // exercised elsewhere and would otherwise run concurrently.
    window.m_autoRefreshDiagnostics->setChecked(false);

    QTemporaryDir captureDir;
    QVERIFY(captureDir.isValid());
    const QString capturePath = captureDir.path() + QStringLiteral("/request.txt");
    QVERIFY(startFakePrivilegedSession(window, capturePath, QStringLiteral("success")));

    // A host DKMS repair exactly as the Repair page dispatches it.
    closeRepairProgressDialogWhenDone(&window);
    window.runRepairHelper(QStringLiteral("Rebuild DKMS"),
                           {QStringLiteral("repair"), QString(), QString(), QStringLiteral("dkms")});
    const QStringList helperArguments = capturedHelperArguments(capturePath);
    QCOMPARE(helperArguments.value(0), QStringLiteral("host-repair"));
    QCOMPARE(helperArguments.value(1), QStringLiteral("/dev/test-host"));
    QCOMPARE(helperArguments.value(2), QStringLiteral("/dev/test-host-root"));
    QCOMPARE(helperArguments.value(3), QStringLiteral("dkms"));

    // The repair's device refresh replaced the synthetic host identity; restore
    // it before the explicit diagnostics regeneration.
    prepareRepairScope(window, true);
    window.runAllDiagnostics();
    flushLogRefresh(window);
    QCOMPARE(diagnosticSectionCount(window, QStringLiteral("report")), 1);

    const int allIndex = window.m_logFilterCombo->findData(QStringLiteral("all"));
    const int diagnosticsIndex = window.m_logFilterCombo->findData(QStringLiteral("diagnostics"));
    const int repairsIndex = window.m_logFilterCombo->findData(QStringLiteral("repairs"));
    QVERIFY(allIndex >= 0);
    QVERIFY(diagnosticsIndex >= 0);
    QVERIFY(repairsIndex >= 0);

    const QString repairStart = QStringLiteral("Starting privileged action: Rebuild DKMS");
    const QString repairCompletion = QStringLiteral("Rebuild DKMS finished with exit code 0");
    const QString repairOutput = QStringLiteral("Repair output\nDiagnostic: Rebuild DKMS");
    const QString repairStale = QStringLiteral("selected system was modified by repair action");
    const QString diagnosticStart = QStringLiteral("Starting all available read-only diagnostics");
    const QString reportMarker = QStringLiteral("Diagnostic: report");

    // Repairs shows every entry of the repair cycle and no diagnostic section.
    window.m_logFilterCombo->setCurrentIndex(repairsIndex);
    QString visible = window.m_logView->toPlainText();
    QVERIFY2(visible.contains(repairStart), "the privileged action start must be a repair entry");
    QVERIFY2(visible.contains(repairCompletion), "the completion must be a repair entry");
    QVERIFY2(visible.contains(repairOutput), "the stage output must be a repair entry");
    QVERIFY(visible.contains(repairStale));
    QVERIFY(!visible.contains(diagnosticStart));
    QVERIFY(!visible.contains(reportMarker));

    // Diagnostics shows the regenerated section and no repair entry.
    window.m_logFilterCombo->setCurrentIndex(diagnosticsIndex);
    visible = window.m_logView->toPlainText();
    QVERIFY(visible.contains(diagnosticStart));
    QVERIFY(visible.contains(reportMarker));
    QVERIFY(!visible.contains(repairStart));
    QVERIFY(!visible.contains(repairCompletion));
    QVERIFY(!visible.contains(repairOutput));

    // A search term is applied inside the selected filter only.
    window.m_logFilterCombo->setCurrentIndex(repairsIndex);
    window.m_logSearchEdit->setText(QStringLiteral("dkms"));
    visible = window.m_logView->toPlainText();
    QVERIFY(visible.contains(repairStart));
    QVERIFY(visible.contains(repairCompletion));
    QVERIFY(visible.contains(repairOutput));
    QVERIFY(!visible.contains(repairStale));
    QVERIFY(!visible.contains(diagnosticStart));
    QVERIFY(!visible.contains(reportMarker));
    window.m_logSearchEdit->clear();

    // The regeneration replaced the report in place; there is exactly one.
    window.m_logFilterCombo->setCurrentIndex(allIndex);
    visible = window.m_logView->toPlainText();
    QCOMPARE(diagnosticSectionCount(window, QStringLiteral("report")), 1);
    QCOMPARE(diagnosticSectionCountInText(visible, QStringLiteral("report")), 1);

    // Prior sessions persist the kind in the recorded category header, so the
    // same filter-then-search order holds for the read-only prior view.
    const QString priorPath = logDir.path() + QStringLiteral("/session-20260101-000000.log");
    {
        QFile file(priorPath);
        QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate));
        QTextStream stream(&file);
        stream << "──────── REPAIR ────────\n"
               << "[2026-01-01 00:00:00] [INFO] Starting privileged action: Rebuild DKMS on /dev/vdb (/dev/vdb2).\n"
               << "──────── DIAGNOSTIC ────────\n"
               << "[2026-01-01 00:00:01] [INFO] Starting all available read-only diagnostics (Target scope).\n"
               << "──────── REPAIR ────────\n"
               << "[2026-01-01 00:00:02] [INFO] Repair output\n"
               << "Diagnostic: Rebuild DKMS\n"
               << "dkms modules rebuilt\n";
    }
    window.displaySessionLog(priorPath);
    QVERIFY(window.m_viewingPriorLog);

    window.m_logFilterCombo->setCurrentIndex(repairsIndex);
    visible = window.m_logView->toPlainText();
    QVERIFY(visible.contains(repairStart));
    QVERIFY(visible.contains(repairOutput));
    QVERIFY(!visible.contains(diagnosticStart));

    window.m_logSearchEdit->setText(QStringLiteral("dkms"));
    visible = window.m_logView->toPlainText();
    QVERIFY(visible.contains(repairStart));
    QVERIFY(visible.contains(repairOutput));
    QVERIFY(!visible.contains(diagnosticStart));

    window.m_logSearchEdit->clear();
    window.m_logFilterCombo->setCurrentIndex(diagnosticsIndex);
    visible = window.m_logView->toPlainText();
    QVERIFY(visible.contains(diagnosticStart));
    QVERIFY(!visible.contains(repairStart));
    QVERIFY(!visible.contains(repairOutput));
}

void MainWindowUiTest::logSectionFilterShowsRelatedRepairsAndDiagnosticSection()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window, true);
    enterHostDiagnosticScope(window);

    const QString kernelSection = QStringLiteral(
        "========================================\n"
        "Diagnostic: kernel\n"
        "Title: Kernel / initramfs\n"
        "Scope: Running Host\n"
        "What was actually run: Boot Bitch host diagnostic implementation (read-only): kernel\n"
        "Status: completed\n"
        "========================================\n"
        "KERNEL_EVIDENCE_MARKER vmlinuz and initramfs image inspected");
    window.appendLog(kernelSection, QStringLiteral("INFO"), MainWindow::LogEntryKind::Diagnostic);
    window.appendLog(QStringLiteral("Starting privileged action: Rebuild initramfs on /dev/test-host (/dev/test-host-root)."),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral("Repair output\nDiagnostic: Rebuild initramfs\nINITRAMFS_REPAIR_MARKER images rebuilt"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral("Rebuild initramfs finished with exit code 0 (success)."),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral("Repair output\nDiagnostic: Rebuild DKMS\nDKMS_REPAIR_MARKER modules rebuilt"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral("Repair output\nDiagnostic: Refresh package metadata\nAPT_REPAIR_MARKER metadata refreshed"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral("Repair output\nDiagnostic: Regenerate GRUB configuration\nGRUB_REPAIR_MARKER regenerated"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral("Repair output\nDiagnostic: Repair EFI / UKI bootloader\nEFI_REPAIR_MARKER reinstalled"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    flushLogRefresh(window);

    const int kernelIndex = window.m_logFilterCombo->findData(QStringLiteral("@kernel"));
    QVERIFY(kernelIndex >= 0);
    window.m_logFilterCombo->setCurrentIndex(kernelIndex);
    const QString visible = window.m_logView->toPlainText();
    QVERIFY2(visible.contains(QStringLiteral("Diagnostic: kernel")),
             "the kernel diagnostic section must be shown");
    QVERIFY2(visible.contains(QStringLiteral("Title: Kernel / initramfs")),
             "the complete section framing must be shown");
    QVERIFY2(visible.contains(QStringLiteral("KERNEL_EVIDENCE_MARKER")),
             "the complete section body must be shown");
    QVERIFY2(visible.contains(QStringLiteral("INITRAMFS_REPAIR_MARKER")),
             "initramfs repair entries belong to the Kernel / initramfs filter");
    QVERIFY2(visible.contains(QStringLiteral("Rebuild initramfs finished with exit code 0")),
             "the complete initramfs repair cycle must be shown");
    QVERIFY2(visible.contains(QStringLiteral("DKMS_REPAIR_MARKER")),
             "dkms repair entries belong to the Kernel / initramfs filter");
    QVERIFY2(!visible.contains(QStringLiteral("APT_REPAIR_MARKER")),
             "unrelated package repairs must stay hidden");
    QVERIFY2(!visible.contains(QStringLiteral("GRUB_REPAIR_MARKER")),
             "unrelated grub repairs must stay hidden");
    QVERIFY2(!visible.contains(QStringLiteral("(no matching entries for this filter)")),
             "the Kernel / initramfs filter must not fall back to the empty notice");

    const int grubIndex = window.m_logFilterCombo->findData(QStringLiteral("@grub"));
    QVERIFY(grubIndex >= 0);
    window.m_logFilterCombo->setCurrentIndex(grubIndex);
    const QString grubVisible = window.m_logView->toPlainText();
    QVERIFY(grubVisible.contains(QStringLiteral("GRUB_REPAIR_MARKER")));
    QVERIFY(!grubVisible.contains(QStringLiteral("INITRAMFS_REPAIR_MARKER")));
    QVERIFY(!grubVisible.contains(QStringLiteral("DKMS_REPAIR_MARKER")));

    // EFI / UKI pairs with the efi repair workflow.
    const int ukiIndex = window.m_logFilterCombo->findData(QStringLiteral("@uki"));
    QVERIFY(ukiIndex >= 0);
    window.m_logFilterCombo->setCurrentIndex(ukiIndex);
    const QString ukiVisible = window.m_logView->toPlainText();
    QVERIFY(ukiVisible.contains(QStringLiteral("EFI_REPAIR_MARKER")));
    QVERIFY(!ukiVisible.contains(QStringLiteral("GRUB_REPAIR_MARKER")));
    QVERIFY(!ukiVisible.contains(QStringLiteral("INITRAMFS_REPAIR_MARKER")));

    // Environment pairs with the package-stage repair workflows.
    const int environmentIndex = window.m_logFilterCombo->findData(QStringLiteral("@environment"));
    QVERIFY(environmentIndex >= 0);
    window.m_logFilterCombo->setCurrentIndex(environmentIndex);
    const QString environmentVisible = window.m_logView->toPlainText();
    QVERIFY(environmentVisible.contains(QStringLiteral("APT_REPAIR_MARKER")));
    QVERIFY(!environmentVisible.contains(QStringLiteral("EFI_REPAIR_MARKER")));
    QVERIFY(!environmentVisible.contains(QStringLiteral("DKMS_REPAIR_MARKER")));

    // Prior sessions are filtered and expanded identically.
    const QString priorPath = logDir.path() + QStringLiteral("/session-20260101-000000.log");
    {
        QFile file(priorPath);
        QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate));
        QTextStream stream(&file);
        stream << "──────── DIAGNOSTIC ────────\n"
               << kernelSection << "\n"
               << "──────── REPAIR ────────\n"
               << "[2026-01-01 00:00:02] [INFO] Repair output\n"
               << "Diagnostic: Rebuild initramfs\n"
               << "PRIOR_INITRAMFS_REPAIR_MARKER rebuilt\n";
    }
    window.displaySessionLog(priorPath);
    QVERIFY(window.m_viewingPriorLog);
    window.m_logFilterCombo->setCurrentIndex(kernelIndex);
    const QString priorVisible = window.m_logView->toPlainText();
    QVERIFY2(priorVisible.contains(QStringLiteral("KERNEL_EVIDENCE_MARKER")),
             "prior sessions must show the kernel diagnostic section");
    QVERIFY2(priorVisible.contains(QStringLiteral("PRIOR_INITRAMFS_REPAIR_MARKER")),
             "prior sessions must show the related repair entries");
    QVERIFY2(!priorVisible.contains(QStringLiteral("GRUB_REPAIR_MARKER")),
             "prior sessions must keep unrelated repairs hidden");
}

void MainWindowUiTest::logSectionFilterWithoutMatchesShowsNotice()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window, true);
    enterHostDiagnosticScope(window);

    window.appendLog(QStringLiteral("Repair output\nDiagnostic: Rebuild initramfs\nNOTICE_TEST_REPAIR_MARKER"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    flushLogRefresh(window);

    // The Btrfs status section has no cached section and no related repairs.
    const int btrfsIndex = window.m_logFilterCombo->findData(QStringLiteral("@btrfs"));
    QVERIFY(btrfsIndex >= 0);
    window.m_logFilterCombo->setCurrentIndex(btrfsIndex);
    QString visible = window.m_logView->toPlainText();
    QVERIFY2(visible.contains(QStringLiteral("(no matching entries for this filter)")),
             "a section filter without matches must show the notice instead of an empty view");
    QVERIFY(!visible.contains(QStringLiteral("NOTICE_TEST_REPAIR_MARKER")));

    // The notice also covers a search inside the filter that matches nothing.
    window.m_logSearchEdit->setText(QStringLiteral("no-such-marker"));
    visible = window.m_logView->toPlainText();
    QVERIFY2(visible.contains(QStringLiteral("(no matching entries for this filter)")),
             "a search without matches must show the notice instead of an empty view");

    // Clearing both returns to the complete view.
    window.m_logSearchEdit->clear();
    window.m_logFilterCombo->setCurrentIndex(window.m_logFilterCombo->findData(QStringLiteral("all")));
    visible = window.m_logView->toPlainText();
    QVERIFY(visible.contains(QStringLiteral("NOTICE_TEST_REPAIR_MARKER")));
}

void MainWindowUiTest::logSectionFilterSearchMatchesOnlyFilteredSubset()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window, true);
    enterHostDiagnosticScope(window);

    const QString kernelSection = QStringLiteral(
        "========================================\n"
        "Diagnostic: kernel\n"
        "Title: Kernel / initramfs\n"
        "Scope: Running Host\n"
        "Status: completed\n"
        "========================================\n"
        "KERNEL_SECTION_HEADER_LINE\n"
        "SUBSETTOKEN in the kernel evidence\n"
        "KERNEL_SECTION_TAIL_LINE");
    window.appendLog(kernelSection, QStringLiteral("INFO"), MainWindow::LogEntryKind::Diagnostic);
    window.appendLog(QStringLiteral("Repair output\nDiagnostic: Rebuild initramfs\nSUBSETTOKEN in the initramfs repair"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral("Repair output\nDiagnostic: Rebuild DKMS\nDKMS_WITHOUT_TOKEN modules rebuilt"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral("Repair output\nDiagnostic: Refresh package metadata\nSUBSETTOKEN in the package repair"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    flushLogRefresh(window);

    const int kernelIndex = window.m_logFilterCombo->findData(QStringLiteral("@kernel"));
    QVERIFY(kernelIndex >= 0);
    window.m_logFilterCombo->setCurrentIndex(kernelIndex);
    window.m_logSearchEdit->setText(QStringLiteral("SUBSETTOKEN"));
    const QString visible = window.m_logView->toPlainText();
    QVERIFY2(visible.contains(QStringLiteral("Diagnostic: kernel")),
             "the matching kernel section must be shown whole");
    QVERIFY2(visible.contains(QStringLiteral("KERNEL_SECTION_TAIL_LINE")),
             "the complete section must be shown, not only matching lines");
    QVERIFY2(visible.contains(QStringLiteral("SUBSETTOKEN in the initramfs repair")),
             "matching repair entries inside the filter must be shown");
    QVERIFY2(!visible.contains(QStringLiteral("DKMS_WITHOUT_TOKEN")),
             "repair entries that do not match the search must stay hidden");
    QVERIFY2(!visible.contains(QStringLiteral("SUBSETTOKEN in the package repair")),
             "matching entries outside the selected section must stay hidden");
}

void MainWindowUiTest::logSectionFilterExtractsEmbeddedReportSectionsAndShowsNotice()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window, true);
    enterHostDiagnosticScope(window);

    // One Run All report entry embedding the per-key sections, exactly as the
    // privileged helper captures them inside the combined report.
    window.appendLog(combinedReportEntry(QStringLiteral("Running Host"),
        {embeddedReportSection(QStringLiteral("environment"), QStringLiteral("Environment validation"),
                               QStringLiteral("Running Host"), QStringLiteral("ENVONLYTOKEN inspection ready")),
         embeddedReportSection(QStringLiteral("grub"), QStringLiteral("GRUB configuration"),
                               QStringLiteral("Running Host"), QStringLiteral("GRUBONLYTOKEN timeout=5 default entry")),
         embeddedReportSection(QStringLiteral("kernel"), QStringLiteral("Kernel / initramfs"),
                               QStringLiteral("Running Host"), QStringLiteral("KERNELONLYTOKEN vmlinuz present"))}),
        QStringLiteral("INFO"), MainWindow::LogEntryKind::Diagnostic);
    window.appendLog(QStringLiteral("Repair output\nDiagnostic: Regenerate GRUB configuration\nGRUBREPAIRTOKEN regenerated"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral("Repair output\nDiagnostic: Rebuild initramfs\nINITRAMFSREPAIRTOKEN images rebuilt"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    flushLogRefresh(window);

    // The visible dropdown entry extracts the embedded grub section from the
    // combined report instead of falling back to the empty notice.
    const int grubIndex = window.m_logFilterCombo->findText(QStringLiteral("GRUB configuration"));
    QVERIFY(grubIndex >= 0);
    window.m_logFilterCombo->setCurrentIndex(grubIndex);
    QString visible = window.m_logView->toPlainText();
    QVERIFY2(visible.contains(QStringLiteral("Diagnostic: grub")),
             "the embedded grub section header must be shown");
    QVERIFY2(visible.contains(QStringLiteral("GRUB configuration")),
             "the embedded grub section title must be shown");
    QVERIFY2(visible.contains(QStringLiteral("GRUBONLYTOKEN")),
             "the complete embedded grub section body must be shown");
    QVERIFY2(visible.contains(QStringLiteral("GRUBREPAIRTOKEN")),
             "related grub repair entries must be included");
    QVERIFY2(!visible.contains(QStringLiteral("ENVONLYTOKEN")),
             "unrelated embedded sections must stay hidden");
    QVERIFY2(!visible.contains(QStringLiteral("KERNELONLYTOKEN")),
             "unrelated embedded sections must stay hidden");
    QVERIFY2(!visible.contains(QStringLiteral("INITRAMFSREPAIRTOKEN")),
             "unrelated repairs must stay hidden");
    QVERIFY2(!visible.contains(QStringLiteral("(no matching entries for this filter)")),
             "the GRUB configuration filter must not fall back to the empty notice");

    // The @grub search term behaves exactly like the dropdown section entry.
    window.m_logFilterCombo->setCurrentIndex(window.m_logFilterCombo->findData(QStringLiteral("all")));
    window.m_logSearchEdit->setText(QStringLiteral("@grub"));
    visible = window.m_logView->toPlainText();
    QVERIFY2(visible.contains(QStringLiteral("Diagnostic: grub")),
             "@grub must select the embedded grub section");
    QVERIFY2(visible.contains(QStringLiteral("GRUBONLYTOKEN")),
             "@grub must show the complete embedded grub section");
    QVERIFY2(visible.contains(QStringLiteral("GRUBREPAIRTOKEN")),
             "@grub must include the related grub repair entries");
    QVERIFY2(!visible.contains(QStringLiteral("KERNELONLYTOKEN")),
             "@grub must hide unrelated embedded sections");

    // Additional free-text terms combine with the extracted section as AND.
    window.m_logSearchEdit->setText(QStringLiteral("@grub GRUBONLYTOKEN"));
    QVERIFY(window.m_logView->toPlainText().contains(QStringLiteral("Diagnostic: grub")));
    window.m_logSearchEdit->setText(QStringLiteral("@grub KERNELONLYTOKEN"));
    visible = window.m_logView->toPlainText();
    QVERIFY2(!visible.contains(QStringLiteral("Diagnostic: grub")),
             "a term that only matches another section must hide the grub section");
    QVERIFY2(visible.contains(QStringLiteral("(no matching entries for this filter)")),
             "an unmatched search inside the section filter must show the notice");
    window.m_logSearchEdit->clear();

    // --- merged from logSectionFilterWithoutEmbeddedSectionShowsNotice() ---
    {
        MainWindow window;
        window.show();
        QTest::qWait(50);
        window.m_snapshotPreloadScheduled = true;
        prepareRepairScope(window, true);
        enterHostDiagnosticScope(window);

        // A combined report that simply never captured the grub section.
        window.appendLog(combinedReportEntry(QStringLiteral("Running Host"),
            {embeddedReportSection(QStringLiteral("environment"), QStringLiteral("Environment validation"),
                                   QStringLiteral("Running Host"), QStringLiteral("ENVONLYTOKEN inspection ready")),
             embeddedReportSection(QStringLiteral("kernel"), QStringLiteral("Kernel / initramfs"),
                                   QStringLiteral("Running Host"), QStringLiteral("KERNELONLYTOKEN vmlinuz present"))}),
            QStringLiteral("INFO"), MainWindow::LogEntryKind::Diagnostic);
        flushLogRefresh(window);

        const int grubIndex = window.m_logFilterCombo->findText(QStringLiteral("GRUB configuration"));
        QVERIFY(grubIndex >= 0);
        window.m_logFilterCombo->setCurrentIndex(grubIndex);
        const QString visible = window.m_logView->toPlainText();
        QVERIFY2(visible.contains(QStringLiteral("(no matching entries for this filter)")),
                 "a report without a grub section must show the notice");
        QVERIFY2(!visible.contains(QStringLiteral("Diagnostic: grub")),
                 "no grub section exists to show");
        QVERIFY2(!visible.contains(QStringLiteral("ENVONLYTOKEN")),
                 "unrelated embedded sections must stay hidden");
        QVERIFY2(!visible.contains(QStringLiteral("KERNELONLYTOKEN")),
                 "unrelated embedded sections must stay hidden");
    }
}

void MainWindowUiTest::logSectionFilterExtractsEmbeddedSectionsFromPriorSession()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window, true);
    enterHostDiagnosticScope(window);

    const QString priorPath = logDir.path() + QStringLiteral("/session-20260101-000000.log");
    {
        QFile file(priorPath);
        QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate));
        QTextStream stream(&file);
        stream << "──────── DIAGNOSTIC ────────\n"
               << "[2026-01-01 00:00:01] [INFO] "
               << combinedReportEntry(QStringLiteral("Repair Target"),
                    {embeddedReportSection(QStringLiteral("environment"), QStringLiteral("Environment validation"),
                                           QStringLiteral("Repair Target"), QStringLiteral("PRIORENVTOKEN ready")),
                     embeddedReportSection(QStringLiteral("grub"), QStringLiteral("GRUB configuration"),
                                           QStringLiteral("Repair Target"), QStringLiteral("PRIORGRUBTOKEN timeout=5"))})
               << "\n"
               << "──────── REPAIR ────────\n"
               << "[2026-01-01 00:00:02] [INFO] Repair output\n"
               << "Diagnostic: Regenerate GRUB configuration\n"
               << "PRIORGRUBREPAIRTOKEN regenerated\n";
    }
    window.displaySessionLog(priorPath);
    QVERIFY(window.m_viewingPriorLog);

    const int grubIndex = window.m_logFilterCombo->findText(QStringLiteral("GRUB configuration"));
    QVERIFY(grubIndex >= 0);
    window.m_logFilterCombo->setCurrentIndex(grubIndex);
    const QString visible = window.m_logView->toPlainText();
    QVERIFY2(visible.contains(QStringLiteral("Diagnostic: grub")),
             "prior sessions must extract the embedded grub section");
    QVERIFY2(visible.contains(QStringLiteral("PRIORGRUBTOKEN")),
             "prior sessions must show the complete embedded grub section");
    QVERIFY2(visible.contains(QStringLiteral("PRIORGRUBREPAIRTOKEN")),
             "prior sessions must include the related grub repair entries");
    QVERIFY2(!visible.contains(QStringLiteral("PRIORENVTOKEN")),
             "prior sessions must hide unrelated embedded sections");
    QVERIFY2(!visible.contains(QStringLiteral("(no matching entries for this filter)")),
             "the prior-session grub filter must not fall back to the notice");
}

// The "File system repair" workflow filter shows the file system check/repair
// entries together with the complete storage evidence sections (dedicated or
// extracted from a combined report), while unrelated sections and repairs stay
// hidden and the existing section filters keep their behavior.
void MainWindowUiTest::logWorkflowFilesystemFilterShowsCheckRepairAndEvidence()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window, true);
    enterHostDiagnosticScope(window);

    const QString capabilityPreamble = QStringLiteral(
        "Repair capability probes (read-only, selected target):\n"
        "Repair tool filesystem: available\n"
        "Repair capability evidence (read-only):\n"
        "Repair capability evidence filesystem: e2fsck, fsck.fat\n");
    window.appendLog(combinedReportEntry(QStringLiteral("Running Host"),
        {capabilityPreamble
             + embeddedReportSection(QStringLiteral("environment"), QStringLiteral("Environment validation"),
                                     QStringLiteral("Running Host"), QStringLiteral("FS_ENV_MARKER Filesystem: btrfs")),
         embeddedReportSection(QStringLiteral("usage"), QStringLiteral("Disk usage"),
                               QStringLiteral("Running Host"), QStringLiteral("FS_USAGE_MARKER Filesystem usage:")),
         embeddedReportSection(QStringLiteral("btrfs"), QStringLiteral("Btrfs status"),
                               QStringLiteral("Running Host"), QStringLiteral("FS_BTRFS_MARKER Btrfs filesystem:")),
         embeddedReportSection(QStringLiteral("fstab"), QStringLiteral("fstab"),
                               QStringLiteral("Running Host"), QStringLiteral("FS_FSTAB_MARKER uuid=1111 / ext4")),
         embeddedReportSection(QStringLiteral("kernel"), QStringLiteral("Kernel / initramfs"),
                               QStringLiteral("Running Host"), QStringLiteral("FS_KERNEL_MARKER Target kernel files:"))}),
        QStringLiteral("INFO"), MainWindow::LogEntryKind::Diagnostic);
    window.appendLog(QStringLiteral("Repair output\nDiagnostic: Check File Systems\nFS_CHECK_REPAIR_MARKER e2fsck clean"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral("Repair output\nDiagnostic: File system repair (repair)\nFS_REPAIR_MARKER repaired"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral("Repair output\nDiagnostic: Rebuild initramfs\nFS_UNRELATED_REPAIR_MARKER rebuilt"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    flushLogRefresh(window);

    const int filesystemIndex = window.m_logFilterCombo->findData(QStringLiteral("topic:filesystem"));
    QVERIFY(filesystemIndex >= 0);
    QCOMPARE(window.m_logFilterCombo->itemText(filesystemIndex), QStringLiteral("File system repair"));
    window.m_logFilterCombo->setCurrentIndex(filesystemIndex);
    const QString visible = window.m_logView->toPlainText();
    QVERIFY2(visible.contains(QStringLiteral("Diagnostic: environment")),
             "the environment storage evidence must be shown whole");
    QVERIFY2(visible.contains(QStringLiteral("FS_ENV_MARKER")),
             "the complete environment section body must be shown");
    QVERIFY2(visible.contains(QStringLiteral("Diagnostic: usage")),
             "the usage evidence must be shown whole");
    QVERIFY2(visible.contains(QStringLiteral("FS_USAGE_MARKER")),
             "the complete usage section body must be shown");
    QVERIFY2(visible.contains(QStringLiteral("Diagnostic: btrfs")),
             "the btrfs evidence must be shown whole");
    QVERIFY2(visible.contains(QStringLiteral("FS_BTRFS_MARKER")),
             "the complete btrfs section body must be shown");
    QVERIFY2(visible.contains(QStringLiteral("Diagnostic: fstab")),
             "the fstab evidence must be shown whole");
    QVERIFY2(visible.contains(QStringLiteral("FS_FSTAB_MARKER")),
             "the complete fstab section body must be shown");
    QVERIFY2(visible.contains(QStringLiteral("FS_CHECK_REPAIR_MARKER")),
             "the file system check entry must be included");
    QVERIFY2(visible.contains(QStringLiteral("FS_REPAIR_MARKER")),
             "the file system repair entry must be included");
    QVERIFY2(!visible.contains(QStringLiteral("FS_KERNEL_MARKER")),
             "unrelated embedded sections must stay hidden");
    QVERIFY2(!visible.contains(QStringLiteral("FS_UNRELATED_REPAIR_MARKER")),
             "unrelated repair entries must stay hidden");
    QVERIFY2(!visible.contains(QStringLiteral("(no matching entries for this filter)")),
             "the workflow filter must not fall back to the empty notice");

    // The existing section filters keep their behavior: Kernel / initramfs
    // still shows the kernel section and its initramfs repair, not the file
    // system workflow entries.
    window.m_logFilterCombo->setCurrentIndex(
        window.m_logFilterCombo->findData(QStringLiteral("@kernel")));
    const QString kernelVisible = window.m_logView->toPlainText();
    QVERIFY2(kernelVisible.contains(QStringLiteral("FS_KERNEL_MARKER")),
             "the kernel section filter must still show its embedded section");
    QVERIFY2(kernelVisible.contains(QStringLiteral("FS_UNRELATED_REPAIR_MARKER")),
             "the kernel section filter must still show its initramfs repair");
    QVERIFY2(!kernelVisible.contains(QStringLiteral("FS_REPAIR_MARKER")),
             "the kernel section filter must hide file system repairs");
    window.m_logFilterCombo->setCurrentIndex(
        window.m_logFilterCombo->findData(QStringLiteral("all")));
}

// Host→target and target→host file-copy operations are tagged FileCopy
// entries. They must be visible under the "File system repair" workflow
// filter (the copy is part of repairing the system's filesystems) and under a
// dedicated "File copy" filter, while unrelated section filters keep hiding
// them and the file system check/repair entries keep their own filter.
void MainWindowUiTest::logWorkflowFilesystemFilterShowsFileCopyOperations()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window, true);
    enterHostDiagnosticScope(window);

    window.appendLog(QStringLiteral("Repair output\nDiagnostic: Check File Systems\nFS_COPY_UNRELATED_CHECK e2fsck clean"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral("Repair output\nDiagnostic: File Copy — Host → Repair\nFS_COPY_HOST_TO_TARGET copied"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::FileCopy);
    window.appendLog(QStringLiteral("Repair output\nDiagnostic: File Copy — Repair → Host\nFS_COPY_TARGET_TO_HOST copied"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::FileCopy);
    window.appendLog(QStringLiteral("Staged Host → Repair destination: /mnt/recovery. No copy occurred."),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::FileCopy);
    window.appendLog(QStringLiteral("Repair output\nDiagnostic: Rebuild initramfs\nFS_COPY_UNRELATED_INITRAMFS rebuilt"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    flushLogRefresh(window);

    // The dedicated File copy filter exists and shows both copy directions
    // plus the staging notice, without leaking check/repair or kernel entries.
    const int fileCopyIndex = window.m_logFilterCombo->findData(QStringLiteral("topic:filecopy"));
    QVERIFY(fileCopyIndex >= 0);
    QCOMPARE(window.m_logFilterCombo->itemText(fileCopyIndex), QStringLiteral("File copy"));
    window.m_logFilterCombo->setCurrentIndex(fileCopyIndex);
    QString visible = window.m_logView->toPlainText();
    QVERIFY2(visible.contains(QStringLiteral("FS_COPY_HOST_TO_TARGET")),
             "the dedicated filter must show the host-to-target copy");
    QVERIFY2(visible.contains(QStringLiteral("FS_COPY_TARGET_TO_HOST")),
             "the dedicated filter must show the target-to-host copy");
    QVERIFY2(visible.contains(QStringLiteral("Staged Host → Repair destination")),
             "the dedicated filter must show the staging notice");
    QVERIFY2(!visible.contains(QStringLiteral("FS_COPY_UNRELATED_CHECK")),
             "the dedicated filter must hide file system check entries");
    QVERIFY2(!visible.contains(QStringLiteral("FS_COPY_UNRELATED_INITRAMFS")),
             "the dedicated filter must hide unrelated repair entries");

    // The File system repair workflow includes the copy operations and keeps
    // its check/repair entries.
    const int filesystemIndex = window.m_logFilterCombo->findData(QStringLiteral("topic:filesystem"));
    QVERIFY(filesystemIndex >= 0);
    window.m_logFilterCombo->setCurrentIndex(filesystemIndex);
    visible = window.m_logView->toPlainText();
    QVERIFY2(visible.contains(QStringLiteral("FS_COPY_HOST_TO_TARGET")),
             "File system repair must show the host-to-target copy");
    QVERIFY2(visible.contains(QStringLiteral("FS_COPY_TARGET_TO_HOST")),
             "File system repair must show the target-to-host copy");
    QVERIFY2(visible.contains(QStringLiteral("FS_COPY_UNRELATED_CHECK")),
             "File system repair must keep the file system check entry");
    QVERIFY2(!visible.contains(QStringLiteral("FS_COPY_UNRELATED_INITRAMFS")),
             "File system repair must keep hiding unrelated repair entries");

    // An unrelated section filter still hides the copy entries.
    window.m_logFilterCombo->setCurrentIndex(
        window.m_logFilterCombo->findData(QStringLiteral("@kernel")));
    const QString kernelVisible = window.m_logView->toPlainText();
    QVERIFY2(!kernelVisible.contains(QStringLiteral("FS_COPY_HOST_TO_TARGET")),
             "the kernel section filter must hide host-to-target copies");
    QVERIFY2(!kernelVisible.contains(QStringLiteral("FS_COPY_TARGET_TO_HOST")),
             "the kernel section filter must hide target-to-host copies");
    QVERIFY2(kernelVisible.contains(QStringLiteral("FS_COPY_UNRELATED_INITRAMFS")),
             "the kernel section filter must keep its initramfs repair");
    window.m_logFilterCombo->setCurrentIndex(
        window.m_logFilterCombo->findData(QStringLiteral("all")));
}

// The "Package repair" workflow filter shows the package-stage entries
// (dependency repair, interrupted package configuration, metadata refresh,
// upgrades and DKMS) plus the helper's package-backend BEGIN/SKIP and
// held-back/skipped feedback lines, and it sits directly after "File system
// repair" in the dropdown. The file system check/repair workflow keeps
// showing only its own entries, so package stages never leak into it.
void MainWindowUiTest::logWorkflowPackageFilterShowsPackageStagesAndKeepsFilesystemSeparate()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window, true);
    enterHostDiagnosticScope(window);

    window.appendLog(QStringLiteral("Repair output\nDiagnostic: Check File Systems\nPKG_FS_CHECK_MARKER e2fsck clean"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral("Repair output\nDiagnostic: File system repair (repair)\nPKG_FS_REPAIR_MARKER repaired"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral("Repair output\nDiagnostic: Repair broken dependencies\nPKG_FIXBROKEN_MARKER dependencies repaired"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral("Repair output\nDiagnostic: Complete package configuration\nPKG_DPKG_MARKER configuration completed"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral("Repair output\nDiagnostic: Refresh package metadata\nPKG_APTUPDATE_MARKER metadata refreshed"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral("Repair output\nDiagnostic: Upgrade installed packages\n"
                                    "BEGIN: package backend apt/dpkg (apt-upgrade)\n"
                                    "Package kept back: firmware-mediatek\n"
                                    "PKG_UPGRADE_MARKER upgraded"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral("Repair output\nDiagnostic: DKMS\nPKG_DKMS_MARKER modules rebuilt"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    // A combined Full Repair entry carries the package backends' BEGIN/SKIP
    // and feedback lines for a stage whose name matches no stage topic
    // (dpkg-configure); only the packagefeedback topic can capture it.
    window.appendLog(QStringLiteral("Repair output\nDiagnostic: Full Repair\n"
                                    "BEGIN: package backend apt/dpkg (dpkg-configure)\n"
                                    "SKIP: package backend rpm is not runnable for stage 'dpkg-configure': dnf5 is not installed\n"
                                    "Package manager feedback: apt/dpkg: 1 package kept back: firmware-mediatek\n"
                                    "PKG_FULLREPAIR_MARKER"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral("Repair output\nDiagnostic: Regenerate GRUB configuration\nPKG_GRUB_MARKER regenerated"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    flushLogRefresh(window);

    const int filesystemIndex = window.m_logFilterCombo->findData(QStringLiteral("topic:filesystem"));
    const int packageIndex = window.m_logFilterCombo->findData(QStringLiteral("topic:package"));
    QVERIFY(filesystemIndex >= 0);
    QVERIFY(packageIndex >= 0);
    QCOMPARE(window.m_logFilterCombo->itemText(packageIndex), QStringLiteral("Package repair"));
    QCOMPARE(packageIndex, filesystemIndex + 1);

    window.m_logFilterCombo->setCurrentIndex(packageIndex);
    const QString packageVisible = window.m_logView->toPlainText();
    QVERIFY2(packageVisible.contains(QStringLiteral("PKG_FIXBROKEN_MARKER")),
             "the dependency repair entry must be shown");
    QVERIFY2(packageVisible.contains(QStringLiteral("PKG_DPKG_MARKER")),
             "the interrupted package configuration entry must be shown");
    QVERIFY2(packageVisible.contains(QStringLiteral("PKG_APTUPDATE_MARKER")),
             "the metadata refresh entry must be shown");
    QVERIFY2(packageVisible.contains(QStringLiteral("PKG_UPGRADE_MARKER")),
             "the package upgrade entry must be shown");
    QVERIFY2(packageVisible.contains(QStringLiteral("Package kept back: firmware-mediatek")),
             "the held-back feedback line must be shown");
    QVERIFY2(packageVisible.contains(QStringLiteral("PKG_DKMS_MARKER")),
             "the DKMS rebuild entry must be shown");
    QVERIFY2(packageVisible.contains(QStringLiteral("PKG_FULLREPAIR_MARKER")),
             "the combined Full Repair entry must be shown");
    QVERIFY2(packageVisible.contains(QStringLiteral("BEGIN: package backend apt/dpkg (dpkg-configure)")),
             "the package-backend BEGIN line must be shown");
    QVERIFY2(packageVisible.contains(QStringLiteral("SKIP: package backend rpm is not runnable for stage 'dpkg-configure'")),
             "the package-backend SKIP line must be shown");
    QVERIFY2(packageVisible.contains(QStringLiteral("Package manager feedback: apt/dpkg: 1 package kept back")),
             "the package-manager feedback line must be shown");
    QVERIFY2(!packageVisible.contains(QStringLiteral("PKG_FS_CHECK_MARKER")),
             "the file system check must stay out of the package workflow");
    QVERIFY2(!packageVisible.contains(QStringLiteral("PKG_FS_REPAIR_MARKER")),
             "the file system repair must stay out of the package workflow");
    QVERIFY2(!packageVisible.contains(QStringLiteral("PKG_GRUB_MARKER")),
             "unrelated GRUB repairs must stay hidden");
    QVERIFY2(!packageVisible.contains(QStringLiteral("(no matching entries for this filter)")),
             "the package workflow filter must not fall back to the empty notice");

    window.m_logFilterCombo->setCurrentIndex(filesystemIndex);
    const QString filesystemVisible = window.m_logView->toPlainText();
    QVERIFY2(filesystemVisible.contains(QStringLiteral("PKG_FS_CHECK_MARKER")),
             "File system repair must keep the file system check entry");
    QVERIFY2(filesystemVisible.contains(QStringLiteral("PKG_FS_REPAIR_MARKER")),
             "File system repair must keep the file system repair entry");
    for (const QString &marker : {QStringLiteral("PKG_FIXBROKEN_MARKER"), QStringLiteral("PKG_DPKG_MARKER"),
                                  QStringLiteral("PKG_APTUPDATE_MARKER"), QStringLiteral("PKG_UPGRADE_MARKER"),
                                  QStringLiteral("PKG_DKMS_MARKER"), QStringLiteral("PKG_FULLREPAIR_MARKER"),
                                  QStringLiteral("PKG_GRUB_MARKER")}) {
        QVERIFY2(!filesystemVisible.contains(marker),
                 qPrintable(QStringLiteral("File system repair must hide %1").arg(marker)));
    }
    window.m_logFilterCombo->setCurrentIndex(
        window.m_logFilterCombo->findData(QStringLiteral("all")));
}

// Selecting the "Package repair" workflow filter extracts the environment and
// backend sections embedded in a combined Run All report (the evidence that
// names the detected package managers) whole, keeps unrelated embedded
// sections hidden, and includes package repair entries while hiding file
// system repairs.
void MainWindowUiTest::logWorkflowPackageFilterExtractsCombinedReportSections()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window, true);
    enterHostDiagnosticScope(window);

    const QString capabilityPreamble = QStringLiteral(
        "Repair capability probes (read-only, selected target):\n"
        "Repair tool fixbroken: available\n"
        "Repair tool aptupdate: available\n"
        "Repair capability evidence (read-only):\n"
        "Repair capability evidence fixbroken: apt/dpkg present\n");
    window.appendLog(combinedReportEntry(QStringLiteral("Running Host"),
        {capabilityPreamble
             + embeddedReportSection(QStringLiteral("environment"), QStringLiteral("Environment validation"),
                                     QStringLiteral("Running Host"),
                                     QStringLiteral("PKG_ENV_MARKER Package manager: apt/dpkg")),
         embeddedReportSection(QStringLiteral("backend"), QStringLiteral("Distribution and boot backend profile"),
                               QStringLiteral("Running Host"),
                               QStringLiteral("PKG_BACKEND_MARKER Package manager backends: apt/dpkg")),
         embeddedReportSection(QStringLiteral("usage"), QStringLiteral("Disk usage"),
                               QStringLiteral("Running Host"), QStringLiteral("PKG_USAGE_MARKER filesystem usage")),
         embeddedReportSection(QStringLiteral("kernel"), QStringLiteral("Kernel / initramfs"),
                               QStringLiteral("Running Host"), QStringLiteral("PKG_KERNEL_MARKER vmlinuz present"))}),
        QStringLiteral("INFO"), MainWindow::LogEntryKind::Diagnostic);
    window.appendLog(QStringLiteral("Repair output\nDiagnostic: Upgrade installed packages\nPKG_COMBINED_UPGRADE_MARKER upgraded"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral("Repair output\nDiagnostic: Check File Systems\nPKG_COMBINED_FS_MARKER e2fsck clean"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    flushLogRefresh(window);

    const int packageIndex = window.m_logFilterCombo->findData(QStringLiteral("topic:package"));
    QVERIFY(packageIndex >= 0);
    window.m_logFilterCombo->setCurrentIndex(packageIndex);
    const QString visible = window.m_logView->toPlainText();
    QVERIFY2(visible.contains(QStringLiteral("Diagnostic: environment")),
             "the embedded environment section must be selected");
    QVERIFY2(visible.contains(QStringLiteral("PKG_ENV_MARKER")),
             "the complete environment section body must be shown");
    QVERIFY2(visible.contains(QStringLiteral("Diagnostic: backend")),
             "the embedded backend section must be selected");
    QVERIFY2(visible.contains(QStringLiteral("PKG_BACKEND_MARKER")),
             "the complete backend section body must be shown");
    QVERIFY2(visible.contains(QStringLiteral("PKG_COMBINED_UPGRADE_MARKER")),
             "the package repair entry must be included");
    QVERIFY2(!visible.contains(QStringLiteral("PKG_USAGE_MARKER")),
             "unrelated embedded sections must stay hidden");
    QVERIFY2(!visible.contains(QStringLiteral("PKG_KERNEL_MARKER")),
             "unrelated embedded sections must stay hidden");
    QVERIFY2(!visible.contains(QStringLiteral("PKG_COMBINED_FS_MARKER")),
             "file system repairs must stay hidden");
    QVERIFY2(!visible.contains(QStringLiteral("(no matching entries for this filter)")),
             "the package workflow filter must not fall back to the empty notice");
}

// A fuzzy search for a term shared by many sections ("file") must not flatten
// the report into single lines without context: every matched embedded section
// keeps its title frame and metadata above its matching lines, and the shared
// capability preamble emitted once at the top is still matched. The @section
// extraction keeps working on the same report.
void MainWindowUiTest::logSearchFileTermKeepsEmbeddedSectionsCoherent()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window, true);
    enterHostDiagnosticScope(window);

    const QString capabilityPreamble = QStringLiteral(
        "Repair capability probes (read-only, selected target):\n"
        "Repair tool filesystem: available\n"
        "Repair capability evidence (read-only):\n"
        "Repair capability evidence filesystem: e2fsck\n");
    window.appendLog(combinedReportEntry(QStringLiteral("Running Host"),
        {capabilityPreamble
             + embeddedReportSection(QStringLiteral("environment"), QStringLiteral("Environment validation"),
                                     QStringLiteral("Running Host"), QStringLiteral("ENV_FILE_LINE Filesystem: btrfs")),
         embeddedReportSection(QStringLiteral("usage"), QStringLiteral("Disk usage"),
                               QStringLiteral("Running Host"), QStringLiteral("USAGE_FILE_LINE Filesystem usage:")),
         embeddedReportSection(QStringLiteral("kernel"), QStringLiteral("Kernel / initramfs"),
                               QStringLiteral("Running Host"), QStringLiteral("KERNEL_UNMATCHED_LINE vmlinuz present"))}),
        QStringLiteral("INFO"), MainWindow::LogEntryKind::Diagnostic);
    flushLogRefresh(window);

    window.m_logSearchEdit->setText(QStringLiteral("file"));
    const QString visible = window.m_logView->toPlainText();
    QVERIFY2(visible.contains(QStringLiteral("Repair tool filesystem: available")),
             "the shared capability preamble match must stay visible");
    QVERIFY2(visible.contains(QStringLiteral("Diagnostic: environment")),
             "the matched environment section must keep its header");
    QVERIFY2(visible.contains(QStringLiteral("Environment validation")),
             "the matched environment section must keep its title frame");
    QVERIFY2(visible.contains(QStringLiteral("ENV_FILE_LINE")),
             "the matching environment line must be shown");
    QVERIFY2(visible.contains(QStringLiteral("Diagnostic: usage")),
             "the matched usage section must keep its header");
    QVERIFY2(visible.contains(QStringLiteral("USAGE_FILE_LINE")),
             "the matching usage line must be shown");
    QVERIFY2(!visible.contains(QStringLiteral("Diagnostic: kernel")),
             "a section without a match must stay hidden");
    QVERIFY2(!visible.contains(QStringLiteral("KERNEL_UNMATCHED_LINE")),
             "an unmatched section body must stay hidden");
    QVERIFY2(visible.indexOf(QStringLiteral("Diagnostic: environment"))
                 < visible.indexOf(QStringLiteral("ENV_FILE_LINE")),
             "the environment match must appear under its own section header");
    QVERIFY2(visible.indexOf(QStringLiteral("ENV_FILE_LINE"))
                 < visible.indexOf(QStringLiteral("Diagnostic: usage")),
             "the environment match must not drift into the next section");
    QVERIFY2(visible.indexOf(QStringLiteral("Diagnostic: usage"))
                 < visible.indexOf(QStringLiteral("USAGE_FILE_LINE")),
             "the usage match must appear under its own section header");

    // The same report still yields whole embedded sections for @section
    // extraction after the capability preamble was emitted once at the top.
    window.m_logSearchEdit->clear();
    window.m_logFilterCombo->setCurrentIndex(
        window.m_logFilterCombo->findData(QStringLiteral("@kernel")));
    const QString kernelVisible = window.m_logView->toPlainText();
    QVERIFY2(kernelVisible.contains(QStringLiteral("Diagnostic: kernel")),
             "the embedded kernel section must be extracted whole");
    QVERIFY2(kernelVisible.contains(QStringLiteral("Kernel / initramfs")),
             "the extracted kernel section must keep its title frame");
    QVERIFY2(kernelVisible.contains(QStringLiteral("KERNEL_UNMATCHED_LINE")),
             "the extracted kernel section must keep its complete body");
    QVERIFY2(!kernelVisible.contains(QStringLiteral("ENV_FILE_LINE")),
             "unrelated embedded sections must stay hidden");
    window.m_logFilterCombo->setCurrentIndex(
        window.m_logFilterCombo->findData(QStringLiteral("all")));
}

void MainWindowUiTest::logViewShowsNewestEntriesFirst()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    const QString older = QStringLiteral("UI_TEST_LOG_OLDER_ENTRY");
    const QString newer = QStringLiteral("UI_TEST_LOG_NEWER_ENTRY");
    window.appendLog(older);
    window.appendLog(newer);
    flushLogRefresh(window);

    const QString visible = window.m_logView->toPlainText();
    QVERIFY(visible.contains(older));
    QVERIFY(visible.contains(newer));
    QVERIFY(visible.indexOf(newer) < visible.indexOf(older));
    QVERIFY(window.m_logView->verticalScrollBar()->value() <= 2);

    // When target evidence is invalidated (for example after editing
    // /etc/default/grub), the complete affected diagnostic entry is marked,
    // not only the one-line stale warning.
    window.m_targetDiagnosticsNeedRegeneration = true;
    window.appendLog(QStringLiteral("Diagnostic: grub\nScope: Repair Target\nGRUB configuration evidence"));
    flushLogRefresh(window);
    QVERIFY2(window.m_logView->extraSelections().size() >= 3,
             "stale target diagnostic sections should be visually marked as a whole");
}

void MainWindowUiTest::rapidLogAppendsCoalesceIntoOneRefresh()
{
    MainWindow window;
    window.show();
    QTest::qWait(100);
    flushLogRefresh(window);
    const quint64 baseline = window.m_logRefreshCount;

    for (int index = 0; index < 25; ++index) {
        window.appendLog(QStringLiteral("UI_TEST_BURST_%1").arg(index));
    }
    QCOMPARE(window.m_logRefreshCount, baseline);

    QTRY_COMPARE_WITH_TIMEOUT(window.m_logRefreshCount, baseline + 1, 1000);
    const QString visible = window.m_logView->toPlainText();
    QVERIFY(visible.contains(QStringLiteral("UI_TEST_BURST_24")));
    QVERIFY(visible.contains(QStringLiteral("UI_TEST_BURST_0")));
    QVERIFY(visible.indexOf(QStringLiteral("UI_TEST_BURST_24"))
            < visible.indexOf(QStringLiteral("UI_TEST_BURST_0")));

    // The coalesced refresh must not leave a second one queued behind it.
    QTest::qWait(200);
    QCOMPARE(window.m_logRefreshCount, baseline + 1);
}

void MainWindowUiTest::diagnosticRerunReplacesSectionOnly()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    const QString repairMarker = QStringLiteral("UI_TEST_REPAIR_OUTPUT_KEEP");
    window.appendLog(QStringLiteral("Repair output\n%1").arg(repairMarker));

    enterHostDiagnosticScope(window);
    const int environmentRow = diagnosticRow(window, QStringLiteral("environment"));
    QVERIFY(environmentRow >= 0);
    window.m_diagnosticList->setCurrentRow(environmentRow);

    window.runSelectedDiagnostic();
    flushLogRefresh(window);
    QCOMPARE(diagnosticSectionCount(window, QStringLiteral("environment")), 1);

    window.runSelectedDiagnostic();
    flushLogRefresh(window);
    QCOMPARE(diagnosticSectionCount(window, QStringLiteral("environment")), 1);

    // Re-running one diagnostic must never disturb repair output or any other
    // non-diagnostic entry.
    const QString joined = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QCOMPARE(joined.count(repairMarker), 1);
    QCOMPARE(diagnosticSectionCount(window, QStringLiteral("report")), 0);
    const QString visible = window.m_logView->toPlainText();
    QCOMPARE(diagnosticSectionCountInText(visible, QStringLiteral("environment")), 1);
    QCOMPARE(diagnosticSectionCountInText(visible, QStringLiteral("report")), 0);
    QVERIFY(visible.contains(repairMarker));
    QVERIFY(visible.indexOf(QStringLiteral("Diagnostic: environment")) < visible.indexOf(repairMarker));
}

void MainWindowUiTest::everyDiagnosticSectionReplacesOnRerun()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    // Every listed diagnostic section keeps exactly one block per key when it
    // is run again: the newer capture replaces the older one.
    QStringList keys;
    for (int row = 0; row < window.m_diagnosticList->count(); ++row) {
        const QString key = window.m_diagnosticList->item(row)->data(Qt::UserRole).toString();
        if (key == QStringLiteral("report")) {
            continue;
        }
        keys.append(key);
        const QString title = window.m_diagnosticList->item(row)->text();
        window.appendDiagnosticLog(key, title, QStringLiteral("Running Host"),
                                   QStringLiteral("OLD_%1_BODY").arg(key), true);
        window.appendDiagnosticLog(key, title, QStringLiteral("Running Host"),
                                   QStringLiteral("NEW_%1_BODY").arg(key), true);
    }
    QVERIFY(keys.contains(QStringLiteral("environment")));
    QVERIFY(keys.contains(QStringLiteral("boot-evidence")));
    QVERIFY(keys.contains(QStringLiteral("luks")));
    flushLogRefresh(window);

    const QString joined = window.m_actionLogEntries.join(QLatin1Char('\n'));
    for (const QString &key : keys) {
        QCOMPARE(diagnosticSectionCount(window, key), 1);
        QVERIFY2(joined.contains(QStringLiteral("NEW_%1_BODY").arg(key)), qPrintable(key));
        QVERIFY2(!joined.contains(QStringLiteral("OLD_%1_BODY").arg(key)), qPrintable(key));
    }

    // A fresh combined report supersedes every per-key section it regenerates,
    // and running it again leaves exactly one report carrying the newer body.
    window.appendDiagnosticLog(QStringLiteral("report"), QStringLiteral("Full diagnostic report"),
                               QStringLiteral("Running Host"), QStringLiteral("OLD_REPORT_BODY"), true);
    QCOMPARE(diagnosticSectionCount(window, QStringLiteral("report")), 1);
    for (const QString &key : keys) {
        QCOMPARE(diagnosticSectionCount(window, key), 0);
    }
    window.appendDiagnosticLog(QStringLiteral("report"), QStringLiteral("Full diagnostic report"),
                               QStringLiteral("Running Host"), QStringLiteral("NEW_REPORT_BODY"), true);
    const QString reportJoined = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QCOMPARE(diagnosticSectionCount(window, QStringLiteral("report")), 1);
    QVERIFY(reportJoined.contains(QStringLiteral("NEW_REPORT_BODY")));
    QVERIFY(!reportJoined.contains(QStringLiteral("OLD_REPORT_BODY")));
}

void MainWindowUiTest::diagnosticRerunKeepsNewestSectionContent()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    window.appendDiagnosticLog(QStringLiteral("grub"), QStringLiteral("GRUB configuration"),
                               QStringLiteral("Running Host"),
                               QStringLiteral("OLD_GRUB_DIAGNOSTIC_BODY timeout=9"), true);
    window.appendDiagnosticLog(QStringLiteral("grub"), QStringLiteral("GRUB configuration"),
                               QStringLiteral("Running Host"),
                               QStringLiteral("NEW_GRUB_DIAGNOSTIC_BODY timeout=5"), true);
    flushLogRefresh(window);

    QCOMPARE(diagnosticSectionCount(window, QStringLiteral("grub")), 1);
    const QString joined = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QVERIFY(joined.contains(QStringLiteral("NEW_GRUB_DIAGNOSTIC_BODY")));
    QVERIFY(!joined.contains(QStringLiteral("OLD_GRUB_DIAGNOSTIC_BODY")));

    // The @section search still selects the updated block exactly once.
    window.m_logSearchEdit->setText(QStringLiteral("@grub"));
    const QString visible = window.m_logView->toPlainText();
    QCOMPARE(diagnosticSectionCountInText(visible, QStringLiteral("grub")), 1);
    QVERIFY(visible.contains(QStringLiteral("NEW_GRUB_DIAGNOSTIC_BODY")));
    QVERIFY(!visible.contains(QStringLiteral("OLD_GRUB_DIAGNOSTIC_BODY")));
    window.m_logSearchEdit->clear();
}

void MainWindowUiTest::repairRerunReplacesSectionEntries()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());
    // A fresh live register: never adopt a session file another test created
    // in this process.
    MainWindow::s_activeSessionPath.clear();

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("repair-requests.log"));
    QProcess *session = startRepairFakePrivilegedSession(window, capturePath);
    QVERIFY2(session, "the scripted repair session must start");

    for (int run = 1; run <= 2; ++run) {
        // A successful repair refreshes devices, which drops the synthetic
        // host identity; restore it before the next run.
        prepareRepairScope(window, true);
        closeRepairProgressDialogWhenDone(&window);
        window.runRepairHelper(QStringLiteral("Rebuild DKMS"),
                               {QStringLiteral("repair"), QString(), QString(), QStringLiteral("dkms")},
                               MainWindow::LogEntryKind::Repair, QStringLiteral("dkms"));
        QCoreApplication::processEvents();
        flushLogRefresh(window);
        QCOMPARE(repairOutputCount(window, QStringLiteral("Rebuild DKMS")), 1);
    }

    // Exactly one complete repair cycle remains, carrying the newer output:
    // the start notice, the privileged completion line and the captured block
    // were all replaced together.
    const QString joined = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QCOMPARE(joined.count(QStringLiteral("Starting privileged action: Rebuild DKMS")), 1);
    QCOMPARE(joined.count(QStringLiteral("Rebuild DKMS finished with exit code 0")), 1);
    QCOMPARE(repairOutputCount(window, QStringLiteral("Rebuild DKMS")), 1);
    QVERIFY(joined.contains(QStringLiteral("REPAIR_RUN_2 captured output")));
    QVERIFY(!joined.contains(QStringLiteral("REPAIR_RUN_1 captured output")));

    // The rendered view shows the updated block exactly once.
    QString visible = window.m_logView->toPlainText();
    QCOMPARE(visible.count(QStringLiteral("Repair output\nDiagnostic: Rebuild DKMS")), 1);
    QVERIFY(visible.contains(QStringLiteral("REPAIR_RUN_2 captured output")));
    QVERIFY(!visible.contains(QStringLiteral("REPAIR_RUN_1 captured output")));

    // The Repairs dropdown and the related @kernel section filter still find
    // the updated block and only the updated block.
    const int repairsIndex = window.m_logFilterCombo->findData(QStringLiteral("repairs"));
    QVERIFY(repairsIndex >= 0);
    window.m_logFilterCombo->setCurrentIndex(repairsIndex);
    visible = window.m_logView->toPlainText();
    QCOMPARE(visible.count(QStringLiteral("Repair output\nDiagnostic: Rebuild DKMS")), 1);
    QVERIFY(visible.contains(QStringLiteral("REPAIR_RUN_2 captured output")));
    QVERIFY(!visible.contains(QStringLiteral("REPAIR_RUN_1 captured output")));

    const int kernelIndex = window.m_logFilterCombo->findData(QStringLiteral("@kernel"));
    QVERIFY(kernelIndex >= 0);
    window.m_logFilterCombo->setCurrentIndex(kernelIndex);
    visible = window.m_logView->toPlainText();
    QVERIFY(visible.contains(QStringLiteral("REPAIR_RUN_2 captured output")));
    QVERIFY(!visible.contains(QStringLiteral("REPAIR_RUN_1 captured output")));
    window.m_logFilterCombo->setCurrentIndex(window.m_logFilterCombo->findData(QStringLiteral("all")));
}

void MainWindowUiTest::repairRerunKeepsToolsAndScopesSeparate()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());
    MainWindow::s_activeSessionPath.clear();

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("separate-repairs.log"));
    // Constant output makes scope identity, not output text, the only thing
    // that can keep the host and target blocks apart.
    QProcess *session = startRepairFakePrivilegedSession(window, capturePath, /*constantOutput=*/true);
    QVERIFY2(session, "the scripted repair session must start");

    // The same tool twice on the running host replaces its own block.
    for (int run = 0; run < 2; ++run) {
        prepareRepairScope(window, true);
        closeRepairProgressDialogWhenDone(&window);
        window.runRepairHelper(QStringLiteral("Rebuild DKMS"),
                               {QStringLiteral("repair"), QString(), QString(), QStringLiteral("dkms")},
                               MainWindow::LogEntryKind::Repair, QStringLiteral("dkms"));
        QCoreApplication::processEvents();
    }
    flushLogRefresh(window);
    QCOMPARE(repairOutputCount(window, QStringLiteral("Rebuild DKMS")), 1);

    // The same tool on a different scope (target vs running host) stays
    // separate.
    prepareRepairScope(window, false);
    closeRepairProgressDialogWhenDone(&window);
    window.runRepairHelper(QStringLiteral("Rebuild DKMS"),
                           {QStringLiteral("repair"), QString(), QString(), QStringLiteral("dkms")},
                           MainWindow::LogEntryKind::Repair, QStringLiteral("dkms"));
    QCoreApplication::processEvents();
    flushLogRefresh(window);
    QCOMPARE(repairOutputCount(window, QStringLiteral("Rebuild DKMS")), 2);

    // Re-running the host block must not consume the identical target block:
    // the target cycle stays a complete, contiguous block.
    prepareRepairScope(window, true);
    closeRepairProgressDialogWhenDone(&window);
    window.runRepairHelper(QStringLiteral("Rebuild DKMS"),
                           {QStringLiteral("repair"), QString(), QString(), QStringLiteral("dkms")},
                           MainWindow::LogEntryKind::Repair, QStringLiteral("dkms"));
    QCoreApplication::processEvents();
    flushLogRefresh(window);
    QCOMPARE(repairOutputCount(window, QStringLiteral("Rebuild DKMS")), 2);
    const QString joined = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QCOMPARE(joined.count(QStringLiteral("Starting privileged action: Rebuild DKMS on /dev/test-host")), 1);
    QCOMPARE(joined.count(QStringLiteral("Starting privileged action: Rebuild DKMS on /dev/test-system")), 1);

    int targetStartIndex = -1;
    for (int index = 0; index < window.m_actionLogEntries.size(); ++index) {
        if (window.m_actionLogEntries.at(index).contains(
                QStringLiteral("Starting privileged action: Rebuild DKMS on /dev/test-system"))) {
            targetStartIndex = index;
            break;
        }
    }
    QVERIFY2(targetStartIndex >= 2, "the target block must still be in the register");
    QVERIFY2(window.m_actionLogEntries.at(targetStartIndex - 1).endsWith(
                 QStringLiteral("Rebuild DKMS finished with exit code 0 (success).")),
             "the target completion line must stay with its block");
    QVERIFY2(window.m_actionLogEntries.at(targetStartIndex - 2).endsWith(
                 QStringLiteral("Repair output\nDiagnostic: Rebuild DKMS\nCONST_REPAIR_OUTPUT")),
             "the target captured output must stay with its block");

    // Different tools stay separate blocks. GRUB and initramfs are the two
    // stages that used to accumulate repeated sections.
    prepareRepairScope(window, true);
    closeRepairProgressDialogWhenDone(&window);
    window.runRepairHelper(QStringLiteral("Rebuild initramfs"),
                           {QStringLiteral("repair"), QString(), QString(), QStringLiteral("initramfs")},
                           MainWindow::LogEntryKind::Repair, QStringLiteral("initramfs"));
    QCoreApplication::processEvents();
    prepareRepairScope(window, true);
    closeRepairProgressDialogWhenDone(&window);
    window.runRepairHelper(QStringLiteral("Regenerate GRUB configuration"),
                           {QStringLiteral("repair"), QString(), QString(), QStringLiteral("grub")},
                           MainWindow::LogEntryKind::Repair, QStringLiteral("grub"));
    QCoreApplication::processEvents();
    flushLogRefresh(window);
    QCOMPARE(repairOutputCount(window, QStringLiteral("Rebuild DKMS")), 2);
    QCOMPARE(repairOutputCount(window, QStringLiteral("Rebuild initramfs")), 1);
    QCOMPARE(repairOutputCount(window, QStringLiteral("Regenerate GRUB configuration")), 1);

    // The @grub section filter still finds the related GRUB repair block.
    const int grubIndex = window.m_logFilterCombo->findData(QStringLiteral("@grub"));
    QVERIFY(grubIndex >= 0);
    window.m_logFilterCombo->setCurrentIndex(grubIndex);
    const QString grubVisible = window.m_logView->toPlainText();
    QVERIFY(grubVisible.contains(QStringLiteral("Repair output\nDiagnostic: Regenerate GRUB configuration")));
    QVERIFY(!grubVisible.contains(QStringLiteral("Repair output\nDiagnostic: Rebuild DKMS")));
    window.m_logFilterCombo->setCurrentIndex(window.m_logFilterCombo->findData(QStringLiteral("all")));
}

void MainWindowUiTest::recurringStatusEntriesReplaceAcrossRepairCycles()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());
    // A fresh live register: never adopt a session file another test created
    // in this process.
    MainWindow::s_activeSessionPath.clear();

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareStaleTargetScope(window);
    // Identify the synthetic target scope so this run owns a session file; the
    // file must keep the append-only history the live register dedupes.
    window.updateSessionScope();
    QVERIFY(!MainWindow::s_activeSessionPath.isEmpty());

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("status-cycles.log"));
    QProcess *session = startRepairFakePrivilegedSession(window, capturePath);
    QVERIFY2(session, "the scripted repair session must start");

    // One authorization session so the established status has an entry the
    // second authorization can replace.
    window.m_privilegedSessionReady = false;
    window.m_uiTestPrivilegedSessionGranted = true;
    window.m_uiTestPrivilegedSessionDelayMs = 0;
    QVERIFY(window.ensurePrivilegedSession());
    QVERIFY(window.m_privilegedSessionReady);

    // Establish a complete report before the cycles so both DKMS repairs are
    // scoped to the kernel section and the recurring scoped statuses are what
    // the two cycles replace.
    window.runAllDiagnostics();
    QVERIFY(!window.m_targetDiagnosticsNeedRegeneration);

    const auto runCycle = [&] {
        // A successful repair refreshes devices, which drops the synthetic
        // target identity. Restore it before the run and before scheduling the
        // automatic regeneration a successful modifying repair requests.
        prepareRepairScope(window);
        closeRepairProgressDialogWhenDone(&window);
        window.runRepairHelper(QStringLiteral("Rebuild DKMS"),
                               {QStringLiteral("repair"), window.m_previewTargetPath,
                                window.m_previewTargetComponentPath, QStringLiteral("dkms")},
                               MainWindow::LogEntryKind::Repair, QStringLiteral("dkms"));
        QCoreApplication::processEvents();
        prepareRepairScope(window);
        window.scheduleEvidenceRefresh(QStringLiteral("modifying repair completed"));
        QVERIFY(window.m_evidenceRefreshTimer);
        QTRY_VERIFY_WITH_TIMEOUT(!window.diagnosticsInvalidationPending(), 15000);
        flushLogRefresh(window);
    };

    runCycle();

    // Every recurring status introduced by a repair/regeneration cycle. Each
    // must exist exactly once after two cycles, carrying the newer content.
    // A DKMS repair maps to the kernel section only, so the scoped
    // regeneration runs that one diagnostic instead of the full report.
    const QStringList statuses = {
        QStringLiteral("lsblk scan completed successfully."),
        QStringLiteral("Discovered "),
        QStringLiteral("Launch detection: "),
        QStringLiteral("Host capability scan refreshed. Missing optional tools disable only the related future feature."),
        QStringLiteral("STALE DIAGNOSTICS: selected system was modified by repair action. Stale cached diagnostic sections: kernel. They will be regenerated automatically before the next repair."),
        QStringLiteral("Automatic read-only diagnostics regeneration scheduled after modifying repair completed."),
        QStringLiteral("Automatically regenerating read-only diagnostics (modifying repair completed)."),
        QStringLiteral("Starting read-only target diagnostic 'kernel' on /dev/test-system (/dev/test-root)."),
        QStringLiteral("Read-only target diagnostic finished with exit code 0 (success)."),
        QStringLiteral("Read-only target diagnostic 'kernel' completed."),
        QStringLiteral("Automatic read-only diagnostics regeneration completed."),
        QStringLiteral("Administrator authorization session established. The GUI remains unprivileged.")
    };

    QHash<QString, QString> firstEntries;
    for (const QString &status : statuses) {
        const QString entry = liveEntryText(window, status);
        QVERIFY2(!entry.isEmpty(), qPrintable(status));
        QCOMPARE(liveEntryCount(window, status), 1);
        firstEntries.insert(status, entry);
    }

    // Force the second cycle into a later second so replacement (rather than a
    // same-second rewrite of identical text) is observable.
    QTest::qWait(1100);

    // A second authorization session and a capability rescan update their
    // status entries in place as well.
    window.m_privilegedSessionReady = false;
    QVERIFY(window.ensurePrivilegedSession());
    window.refreshCapabilities();

    runCycle();

    for (const QString &status : statuses) {
        QCOMPARE(liveEntryCount(window, status), 1);
        const QString entry = liveEntryText(window, status);
        QVERIFY2(!entry.isEmpty(), qPrintable(status));
        QVERIFY2(entry != firstEntries.value(status),
                 qPrintable(QStringLiteral("%1 kept the stale entry").arg(status)));
    }

    // The on-disk session file stays append-only: it keeps every occurrence
    // the live register has already collapsed to one.
    QVERIFY2(!MainWindow::s_activeSessionPath.isEmpty(),
             "the run must have an active session file");
    const QString sessionText = readSessionFile(MainWindow::s_activeSessionPath);
    QVERIFY(!sessionText.isEmpty());
    QVERIFY2(sessionText.count(QStringLiteral("lsblk scan completed successfully.")) >= 2,
             "the session file must keep the historical scan entries");
    QVERIFY2(sessionText.count(QStringLiteral(
                  "STALE DIAGNOSTICS: selected system was modified by repair action")) >= 2,
             "the session file must keep the historical stale notices");

    // Repair output blocks and the diagnostic report section still replace
    // rather than accumulate, independently of the status lines.
    const QString joined = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QCOMPARE(repairOutputCount(window, QStringLiteral("Rebuild DKMS")), 1);
    QVERIFY(joined.contains(QStringLiteral("REPAIR_RUN_2 captured output")));
    QVERIFY(!joined.contains(QStringLiteral("REPAIR_RUN_1 captured output")));
    QCOMPARE(diagnosticSectionCount(window, QStringLiteral("report")), 1);

    // The rendered view shows every status exactly once as well, and the
    // incrementally edited document matches a clean full rebuild.
    QString visible = window.m_logView->toPlainText();
    for (const QString &status : statuses) {
        QCOMPARE(visible.count(status), 1);
    }
    const QString incrementalText = visible;
    window.m_logViewRendered = false;
    window.refreshLogView();
    QCOMPARE(window.m_logView->toPlainText(), incrementalText);

    // Filters and search still find the updated entries and only those.
    const int allIndex = window.m_logFilterCombo->findData(QStringLiteral("all"));
    const int diagnosticsIndex = window.m_logFilterCombo->findData(QStringLiteral("diagnostics"));
    const int repairsIndex = window.m_logFilterCombo->findData(QStringLiteral("repairs"));
    QVERIFY(allIndex >= 0);
    QVERIFY(diagnosticsIndex >= 0);
    QVERIFY(repairsIndex >= 0);

    window.m_logFilterCombo->setCurrentIndex(diagnosticsIndex);
    window.m_logSearchEdit->setText(QStringLiteral("automatic"));
    visible = window.m_logView->toPlainText();
    QCOMPARE(visible.count(QStringLiteral("Automatically regenerating read-only diagnostics (modifying repair completed).")), 1);
    QCOMPARE(visible.count(QStringLiteral("Automatic read-only diagnostics regeneration scheduled after modifying repair completed.")), 1);
    QCOMPARE(visible.count(QStringLiteral("Automatic read-only diagnostics regeneration completed.")), 1);
    QVERIFY(!visible.contains(QStringLiteral("REPAIR_RUN_2 captured output")));
    window.m_logSearchEdit->clear();

    window.m_logFilterCombo->setCurrentIndex(repairsIndex);
    window.m_logSearchEdit->setText(QStringLiteral("dkms"));
    visible = window.m_logView->toPlainText();
    QCOMPARE(visible.count(QStringLiteral("Repair output\nDiagnostic: Rebuild DKMS")), 1);
    QVERIFY(visible.contains(QStringLiteral("REPAIR_RUN_2 captured output")));
    QVERIFY(!visible.contains(QStringLiteral("REPAIR_RUN_1 captured output")));
    window.m_logSearchEdit->clear();
    window.m_logFilterCombo->setCurrentIndex(allIndex);
}

void MainWindowUiTest::recurringStatusEntriesStaySeparatePerScopeAndDisk()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());
    MainWindow::s_activeSessionPath.clear();

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    QProcess *session = startRepairFakePrivilegedSession(
        window, requestDir.filePath(QStringLiteral("separate-status.log")));
    QVERIFY2(session, "the scripted repair session must start");

    const auto runRepair = [&](const QString &targetDisk, const QString &targetRoot, bool host) {
        prepareRepairScope(window, host);
        window.m_hostMaintenanceMode = host;
        if (!host) {
            window.m_previewTargetPath = targetDisk;
            window.m_previewTargetComponentPath = targetRoot;
            DeviceNode disk;
            disk.path = targetDisk;
            disk.type = QStringLiteral("disk");
            DeviceNode component;
            component.path = targetRoot;
            component.type = QStringLiteral("part");
            component.fileSystem = QStringLiteral("ext4");
            component.linuxCapableFileSystem = true;
            component.installedLinux = true;
            disk.children.append(component);
            window.m_deviceIndex.insert(disk.path, disk);
            window.m_deviceIndex.insert(component.path, component);
        }
        closeRepairProgressDialogWhenDone(&window);
        window.runRepairHelper(QStringLiteral("Rebuild DKMS"),
                               {QStringLiteral("repair"),
                                host ? QString() : targetDisk,
                                host ? QString() : targetRoot,
                                QStringLiteral("dkms")},
                               MainWindow::LogEntryKind::Repair, QStringLiteral("dkms"));
        QCoreApplication::processEvents();
        flushLogRefresh(window);
    };

    runRepair(QStringLiteral("/dev/test-system"), QStringLiteral("/dev/test-root"), false);
    runRepair(QStringLiteral("/dev/test-system2"), QStringLiteral("/dev/test-root2"), false);
    runRepair(QString(), QString(), true);

    const QString joined = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QCOMPARE(joined.count(QStringLiteral("Starting privileged action: Rebuild DKMS on /dev/test-system (")), 1);
    QCOMPARE(joined.count(QStringLiteral("Starting privileged action: Rebuild DKMS on /dev/test-system2 (")), 1);
    QCOMPARE(joined.count(QStringLiteral("Starting privileged action: Rebuild DKMS on /dev/test-host (")), 1);
    // The stale status text is scope-agnostic, so it is one replaceable entry:
    // the newest invalidation wins and repeated cycles never stack copies.
    QCOMPARE(joined.count(QStringLiteral("STALE DIAGNOSTICS: selected system was modified by repair action")), 1);

    // Re-running the first target keeps the same single stale entry and does
    // not disturb the second target's or the host's repair blocks.
    runRepair(QStringLiteral("/dev/test-system"), QStringLiteral("/dev/test-root"), false);
    const QString reRun = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QCOMPARE(reRun.count(QStringLiteral("STALE DIAGNOSTICS: selected system was modified by repair action")), 1);
    QCOMPARE(reRun.count(QStringLiteral("Starting privileged action: Rebuild DKMS on /dev/test-system (")), 1);
    QCOMPARE(reRun.count(QStringLiteral("Starting privileged action: Rebuild DKMS on /dev/test-system2 (")), 1);
    QCOMPARE(reRun.count(QStringLiteral("Starting privileged action: Rebuild DKMS on /dev/test-host (")), 1);

    // Manual Run All for two target disks and the host keeps the diagnostic
    // start entries separate per disk and per scope.
    prepareRepairScope(window);
    window.runAllDiagnostics();
    prepareRepairScope(window);
    window.m_previewTargetPath = QStringLiteral("/dev/test-system2");
    window.m_previewTargetComponentPath = QStringLiteral("/dev/test-root2");
    window.runAllDiagnostics();
    prepareRepairScope(window, true);
    window.m_hostMaintenanceMode = true;
    window.runAllDiagnostics();
    flushLogRefresh(window);

    const QString diagnostics = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QCOMPARE(diagnostics.count(QStringLiteral("Starting read-only target diagnostic 'all' on /dev/test-system (/dev/test-root).")), 1);
    QCOMPARE(diagnostics.count(QStringLiteral("Starting read-only target diagnostic 'all' on /dev/test-system2 (/dev/test-root2).")), 1);
    QCOMPARE(diagnostics.count(QStringLiteral("Starting read-only running-host diagnostic 'all' on /dev/test-host (/dev/test-host-root).")), 1);
    // The completion and request lines carry no disk in their text, so their
    // identity carries no disk either: the newest target run updates the one
    // completion entry instead of stacking a per-disk copy.
    QCOMPARE(diagnostics.count(QStringLiteral("Read-only target diagnostic 'all' completed.")), 1);
    QCOMPARE(diagnostics.count(QStringLiteral("Read-only running-host diagnostic 'all' completed.")), 1);
    QCOMPARE(diagnostics.count(QStringLiteral("Run All target diagnostics finished with exit code 0 (success).")), 1);
    // The per-scope run-all framing is keyed by the scope word in its text.
    QCOMPARE(diagnostics.count(QStringLiteral("Starting all available read-only diagnostics (Target scope).")), 1);
    QCOMPARE(diagnostics.count(QStringLiteral("Starting all available read-only diagnostics (Host scope).")), 1);
    QCOMPARE(diagnostics.count(QStringLiteral("All available read-only diagnostic summaries generated and cached by diagnostic.")), 1);
}

void MainWindowUiTest::runAllReplacesPriorReportAndPerKeySections()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    enterHostDiagnosticScope(window);
    const int environmentRow = diagnosticRow(window, QStringLiteral("environment"));
    QVERIFY(environmentRow >= 0);
    window.m_diagnosticList->setCurrentRow(environmentRow);
    window.runSelectedDiagnostic();
    flushLogRefresh(window);
    QCOMPARE(diagnosticSectionCount(window, QStringLiteral("environment")), 1);

    // A fresh Run All report supersedes the per-key sections it regenerates.
    window.runAllDiagnostics();
    flushLogRefresh(window);
    QCOMPARE(diagnosticSectionCount(window, QStringLiteral("report")), 1);
    QCOMPARE(diagnosticSectionCount(window, QStringLiteral("environment")), 0);
    const QString firstReport = window.m_logView->toPlainText();
    QCOMPARE(diagnosticSectionCountInText(firstReport, QStringLiteral("report")), 1);
    QCOMPARE(diagnosticSectionCountInText(firstReport, QStringLiteral("environment")), 0);

    window.runAllDiagnostics();
    flushLogRefresh(window);
    QCOMPARE(diagnosticSectionCount(window, QStringLiteral("report")), 1);
    QCOMPARE(diagnosticSectionCount(window, QStringLiteral("environment")), 0);
    const QString secondReport = window.m_logView->toPlainText();
    QCOMPARE(diagnosticSectionCountInText(secondReport, QStringLiteral("report")), 1);
    QCOMPARE(diagnosticSectionCountInText(secondReport, QStringLiteral("environment")), 0);

    // A manual individual diagnostic after Run All replaces only its own
    // section and leaves the aggregate report in place.
    window.m_diagnosticList->setCurrentRow(environmentRow);
    window.runSelectedDiagnostic();
    flushLogRefresh(window);
    QCOMPARE(diagnosticSectionCount(window, QStringLiteral("environment")), 1);
    QCOMPARE(diagnosticSectionCount(window, QStringLiteral("report")), 1);
    const QString finalVisible = window.m_logView->toPlainText();
    QCOMPARE(diagnosticSectionCountInText(finalVisible, QStringLiteral("environment")), 1);
    QCOMPARE(diagnosticSectionCountInText(finalVisible, QStringLiteral("report")), 1);
    QVERIFY(finalVisible.indexOf(QStringLiteral("Diagnostic: environment"))
            < finalVisible.indexOf(QStringLiteral("Diagnostic: report")));
}

void MainWindowUiTest::incrementalLogViewMatchesFullRebuild()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    enterHostDiagnosticScope(window);
    const int environmentRow = diagnosticRow(window, QStringLiteral("environment"));
    const int grubRow = diagnosticRow(window, QStringLiteral("grub"));
    QVERIFY(environmentRow >= 0);
    QVERIFY(grubRow >= 0);

    // Interleave appends, individual replacements and full-report
    // replacements so the incremental document edits see every transition.
    for (int round = 0; round < 3; ++round) {
        window.appendLog(QStringLiteral("Repair output\nUI_TEST_INCREMENTAL_%1").arg(round));
        window.m_diagnosticList->setCurrentRow(environmentRow);
        window.runSelectedDiagnostic();
        flushLogRefresh(window);
        window.m_diagnosticList->setCurrentRow(grubRow);
        window.runSelectedDiagnostic();
        flushLogRefresh(window);
        window.runAllDiagnostics();
        flushLogRefresh(window);
    }

    const QString incrementalText = window.m_logView->toPlainText();
    QCOMPARE(diagnosticSectionCountInText(incrementalText, QStringLiteral("environment")), 0);
    QCOMPARE(diagnosticSectionCountInText(incrementalText, QStringLiteral("grub")), 0);
    QCOMPARE(diagnosticSectionCountInText(incrementalText, QStringLiteral("report")), 1);

    // Force the full-rebuild fallback and confirm the incrementally edited
    // document is identical to a clean rebuild.
    window.m_logViewRendered = false;
    window.refreshLogView();
    QCOMPARE(window.m_logView->toPlainText(), incrementalText);
}

void MainWindowUiTest::nonLinuxTargetIsRejectedSafely()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    DeviceNode disk;
    disk.path = QStringLiteral("/dev/test-data-disk");
    disk.type = QStringLiteral("disk");
    disk.model = QStringLiteral("Test data disk");
    window.m_deviceIndex.insert(disk.path, disk);

    // A disk with no Linux filesystem and no encrypted Linux candidate must
    // never expose Select Target; this also protects the click path from
    // stale/default-constructed device records during udev refreshes.
    window.showDeviceDetails(disk, true, &disk);
    QVERIFY(window.m_setTargetButton);
    QVERIFY(!window.m_setTargetButton->isEnabled());
    QVERIFY(window.m_setTargetButton->toolTip().contains(QStringLiteral("No Linux filesystem")));

    // A locked encrypted data volume must remain unlock-only until a Linux
    // root is visible.  It must never be committed as an unusable target.
    DeviceNode encrypted;
    encrypted.path = QStringLiteral("/dev/test-data-disk2p1");
    encrypted.type = QStringLiteral("crypt");
    encrypted.fileSystem = QStringLiteral("crypto_LUKS");
    encrypted.encrypted = true;
    DeviceNode encryptedDisk;
    encryptedDisk.path = QStringLiteral("/dev/test-data-disk2");
    encryptedDisk.type = QStringLiteral("disk");
    encryptedDisk.children.append(encrypted);
    window.m_deviceIndex.insert(encryptedDisk.path, encryptedDisk);
    window.m_deviceIndex.insert(encrypted.path, encrypted);
    window.showDeviceDetails(encryptedDisk, true, &encryptedDisk);
    QVERIFY(!window.m_setTargetButton->isEnabled());
    QVERIFY(window.m_setTargetButton->toolTip().contains(QStringLiteral("Unlock")));
    window.m_previewTargetPath.clear();
    window.m_previewTargetComponentPath.clear();
}

void MainWindowUiTest::selectTargetStaysEnabledForEligibleHostMaintenanceCandidate()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;

    // Protected running host selected for explicit maintenance (host mode
    // state: /dev/vda protected, Host Maintenance active).
    prepareRepairScope(window, true);
    QVERIFY(window.m_hostMaintenanceMode);

    // Eligible repair candidate on another physical drive with a visible Linux
    // root (host mode state: /dev/vdb3 ext4).
    DeviceNode component;
    component.path = QStringLiteral("/dev/test-target-root");
    component.type = QStringLiteral("part");
    component.fileSystem = QStringLiteral("ext4");
    component.linuxCapableFileSystem = true;
    DeviceNode disk;
    disk.path = QStringLiteral("/dev/test-target");
    disk.type = QStringLiteral("disk");
    disk.children.append(component);
    window.m_deviceIndex.insert(disk.path, disk);
    window.m_deviceIndex.insert(component.path, component);

    selectTopLevelDisk(window, disk.path);
    QVERIFY2(window.m_setTargetButton->isEnabled(),
             qPrintable(QStringLiteral("Select Target must be enabled for an eligible candidate "
                                       "while host maintenance is active (tooltip: %1)")
                            .arg(window.m_setTargetButton->toolTip())));
}

void MainWindowUiTest::unprivilegedScanUdevEvidenceEnablesSelectTarget()
{
    // Reproduces the Alpine BIOS failure: the GUI runs as an unprivileged
    // user, Alpine's util-linux lsblk is built without libudev and cannot
    // probe block devices, so it reports every FSTYPE as empty even though
    // udev already identified ext4/swap on the target drive. The scanner must
    // fall back to the world-readable udev database and classify the drive as
    // an eligible Linux candidate.
    QTemporaryDir fakeRoot;
    QVERIFY(fakeRoot.isValid());
    UnprivilegedVdbFixture fixture;
    const QString fixtureError = writeUnprivilegedVdbFixture(fakeRoot.path(), &fixture);
    QVERIFY2(fixtureError.isEmpty(), qPrintable(fixtureError));

    ScopedEnvironmentVariable lsblkOverride("BOOT_REPAIR_LSBLK", fixture.lsblkPath.toLocal8Bit());
    ScopedEnvironmentVariable udevOverride("BOOT_REPAIR_UDEV_DATA_DIR", fixture.udevDir.toLocal8Bit());

    SystemScanner scanner;
    QString error;
    const QList<DeviceNode> devices = scanner.scan(&error);
    QVERIFY2(error.isEmpty(), qPrintable(error));
    QCOMPARE(devices.size(), 1);
    const DeviceNode disk = devices.first();
    QCOMPARE(disk.path, fixture.diskPath);
    QCOMPARE(disk.children.size(), 3);
    QCOMPARE(disk.children.at(0).fileSystem, QStringLiteral("ext4"));
    QCOMPARE(disk.children.at(0).uuid, fixture.bootUuid);
    QCOMPARE(disk.children.at(1).fileSystem, QStringLiteral("swap"));
    QCOMPARE(disk.children.at(2).fileSystem, QStringLiteral("ext4"));
    QCOMPARE(disk.children.at(2).uuid, fixture.rootUuid);
    QCOMPARE(disk.children.at(2).partUuid, fixture.rootPartUuid);
    QVERIFY2(disk.children.at(2).linuxCapableFileSystem,
             "the udev evidence must classify the ext4 partition as Linux-capable");

    // Now drive the real Systems selection while host maintenance is active:
    // the eligible target drive must enable Select Target.
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window, true);
    window.m_deviceIndex.insert(disk.path, disk);
    for (const DeviceNode &child : disk.children) {
        window.m_deviceIndex.insert(child.path, child);
    }
    selectTopLevelDisk(window, disk.path);
    QVERIFY2(window.m_setTargetButton->isEnabled(),
             qPrintable(QStringLiteral("Select Target must be enabled for the eligible target "
                                       "(tooltip: %1)").arg(window.m_setTargetButton->toolTip())));
}

void MainWindowUiTest::tableFilesystemColumnShowsUdevEnrichedFilesystem()
{
    // Alpine BIOS systems: the unprivileged scan learns ext4/swap only from
    // the world-readable udev database, but the top-level /dev/vdb candidate
    // row must show the detected filesystem exactly like the details panel
    // instead of staying blank while the details name ext4.
    QTemporaryDir fakeRoot;
    QVERIFY(fakeRoot.isValid());
    UnprivilegedVdbFixture fixture;
    const QString fixtureError = writeUnprivilegedVdbFixture(fakeRoot.path(), &fixture);
    QVERIFY2(fixtureError.isEmpty(), qPrintable(fixtureError));
    ScopedEnvironmentVariable lsblkOverride("BOOT_REPAIR_LSBLK", fixture.lsblkPath.toLocal8Bit());
    ScopedEnvironmentVariable udevOverride("BOOT_REPAIR_UDEV_DATA_DIR", fixture.udevDir.toLocal8Bit());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;

    QTreeWidgetItem *diskItem = deviceTreeTopLevelItem(window, fixture.diskPath);
    QVERIFY2(diskItem, "the scanned /dev/test-vdb drive row must be rendered");
    QCOMPARE(diskItem->childCount(), 3);
    // Partitions sort within their drive, so resolve them by device path.
    QTreeWidgetItem *bootItem = deviceTreeChildItem(diskItem, fixture.bootPath);
    QTreeWidgetItem *swapItem = deviceTreeChildItem(diskItem, fixture.swapPath);
    QTreeWidgetItem *rootItem = deviceTreeChildItem(diskItem, fixture.rootPath);
    QVERIFY2(bootItem && swapItem && rootItem, "all three partition rows must be rendered");
    QCOMPARE(bootItem->text(4), QStringLiteral("ext4"));
    QCOMPARE(swapItem->text(4), QStringLiteral("swap"));
    QCOMPARE(rootItem->text(4), QStringLiteral("ext4"));
    QVERIFY2(diskItem->text(4) != QStringLiteral("—"),
             qPrintable(QStringLiteral("the /dev/test-vdb filesystem cell stayed blank; "
                                       "details report %1")
                            .arg(window.m_detailFilesystem->text())));
    QCOMPARE(diskItem->text(4), QStringLiteral("ext4"));

    // The rendered cell must agree with the details panel for the same row.
    selectDeviceTreeItem(window, diskItem);
    QCOMPARE(window.m_detailFilesystem->text(), diskItem->text(4));
    QCOMPARE(window.m_detailFilesystem->text(), QStringLiteral("ext4"));

    // A Refresh Devices rebuild keeps the enriched value.
    window.refreshDevices();
    diskItem = deviceTreeTopLevelItem(window, fixture.diskPath);
    QVERIFY(diskItem);
    QCOMPARE(diskItem->text(4), QStringLiteral("ext4"));

    // Inspecting a partition and returning to the drive keeps the drive cell
    // and its details consistent.
    selectDeviceTreeItem(window, deviceTreeChildItem(diskItem, fixture.bootPath));
    QCOMPARE(window.m_detailFilesystem->text(), QStringLiteral("ext4"));
    diskItem = deviceTreeTopLevelItem(window, fixture.diskPath);
    QVERIFY(diskItem);
    selectDeviceTreeItem(window, diskItem);
    QCOMPARE(diskItem->text(4), QStringLiteral("ext4"));
    QCOMPARE(window.m_detailFilesystem->text(), QStringLiteral("ext4"));
}

void MainWindowUiTest::preferredRepairNodeResolvesVdbRootAndCommittedTarget()
{
    // 300 MiB ext4 /boot + swap + 15.9 GiB ext4 root: the unprivileged
    // preferred component must be the largest Linux-capable partition
    // (/dev/test-vdb3), never the first ext4 match (/dev/test-vdb1) and never
    // the swap partition. The committed-target line must name that component,
    // not only the physical disk.
    QTemporaryDir fakeRoot;
    QVERIFY(fakeRoot.isValid());
    UnprivilegedVdbFixture fixture;
    const QString fixtureError = writeUnprivilegedVdbFixture(fakeRoot.path(), &fixture);
    QVERIFY2(fixtureError.isEmpty(), qPrintable(fixtureError));
    // Committing a target opens a session log; keep it inside this test's
    // throwaway directory so the process-wide active-session state cannot leak
    // into the later session-log tests.
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());
    ScopedEnvironmentVariable lsblkOverride("BOOT_REPAIR_LSBLK", fixture.lsblkPath.toLocal8Bit());
    ScopedEnvironmentVariable udevOverride("BOOT_REPAIR_UDEV_DATA_DIR", fixture.udevDir.toLocal8Bit());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_uiTestPrivilegedSessionGranted = false;

    QTreeWidgetItem *diskItem = deviceTreeTopLevelItem(window, fixture.diskPath);
    QVERIFY(diskItem);
    selectDeviceTreeItem(window, diskItem);
    QVERIFY2(window.m_setTargetButton->isEnabled(),
             qPrintable(QStringLiteral("Select Target must be enabled for the eligible target "
                                       "(tooltip: %1)").arg(window.m_setTargetButton->toolTip())));
    QCOMPARE(window.m_detailResolvedTarget->text(), fixture.rootPath);
    QCOMPARE(window.m_detailFilesystem->text(), QStringLiteral("ext4"));
    QCOMPARE(window.m_detailUuid->text(), fixture.rootUuid);

    window.m_setTargetButton->click();
    QCOMPARE(window.m_previewTargetPath, fixture.diskPath);
    QCOMPARE(window.m_previewTargetComponentPath, fixture.rootPath);
    const QString committed = window.m_systemTargetLabel->text();
    QVERIFY2(committed.contains(fixture.rootPath),
             qPrintable(QStringLiteral("the committed-target line must name the resolved "
                                       "component: %1").arg(committed)));
    QVERIFY2(!committed.contains(QStringLiteral("vdb1")),
             qPrintable(QStringLiteral("the committed-target line must not name the /boot "
                                       "partition: %1").arg(committed)));
    QVERIFY2(committed.contains(QStringLiteral("0x1af4")),
             qPrintable(QStringLiteral("the committed-target line must keep the drive model: %1")
                            .arg(committed)));
}

void MainWindowUiTest::hostMaintenanceSelectionAndReselectionRestoreRepairScope()
{
    // The reported flow: commit /dev/vdb, enter Host Maintenance, then select
    // the same row again and press Select Target. Re-selection must restore an
    // enabled Select Target, the resolved /dev/vdb3 details and the committed
    // target line instead of leaving stale host-maintenance state behind.
    QTemporaryDir fakeRoot;
    QVERIFY(fakeRoot.isValid());
    UnprivilegedVdbFixture fixture;
    const QString fixtureError = writeUnprivilegedVdbFixture(fakeRoot.path(), &fixture);
    QVERIFY2(fixtureError.isEmpty(), qPrintable(fixtureError));
    // Both the first commit and host maintenance open a session log; keep it
    // inside this test's throwaway directory so the process-wide active-session
    // state cannot leak into the later session-log tests.
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());
    ScopedEnvironmentVariable lsblkOverride("BOOT_REPAIR_LSBLK", fixture.lsblkPath.toLocal8Bit());
    ScopedEnvironmentVariable udevOverride("BOOT_REPAIR_UDEV_DATA_DIR", fixture.udevDir.toLocal8Bit());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_uiTestPrivilegedSessionGranted = false;

    // First commit exactly like the user: select /dev/test-vdb, press Select
    // Target (the privileged prompt is deferred by the UI-test seam).
    QTreeWidgetItem *diskItem = deviceTreeTopLevelItem(window, fixture.diskPath);
    QVERIFY(diskItem);
    selectDeviceTreeItem(window, diskItem);
    QVERIFY(window.m_setTargetButton->isEnabled());
    window.m_setTargetButton->click();
    QCOMPARE(window.m_previewTargetPath, fixture.diskPath);
    QCOMPARE(window.m_previewTargetComponentPath, fixture.rootPath);
    QVERIFY(window.m_systemTargetLabel->text().contains(fixture.rootPath));
    QVERIFY(!window.m_hostMaintenanceMode);

    // Enter Host Maintenance through the real handler. The protected running
    // host tree is injected because the test host has no real protected disk.
    prepareRepairScope(window, true);
    window.m_hostMaintenanceMode = false;
    window.updateHostSystemSummary({window.m_deviceIndex.value(QStringLiteral("/dev/test-host"))});
    window.selectHostForMaintenance();
    QVERIFY(window.m_hostMaintenanceMode);
    QVERIFY(window.m_previewTargetPath.isEmpty());
    QVERIFY(window.m_previewTargetComponentPath.isEmpty());
    QVERIFY(window.m_systemTargetLabel->text().contains(QStringLiteral("Host maintenance")));

    // Re-selecting the same physical row must re-enable Select Target, show
    // the resolved root details again and re-commit the same component.
    diskItem = deviceTreeTopLevelItem(window, fixture.diskPath);
    QVERIFY(diskItem);
    QCOMPARE(diskItem->text(4), QStringLiteral("ext4"));
    selectDeviceTreeItem(window, diskItem);
    QVERIFY2(window.m_setTargetButton->isEnabled(),
             qPrintable(QStringLiteral("Select Target must be enabled after host maintenance "
                                       "(tooltip: %1)").arg(window.m_setTargetButton->toolTip())));
    QCOMPARE(window.m_detailResolvedTarget->text(), fixture.rootPath);
    QCOMPARE(window.m_detailFilesystem->text(), QStringLiteral("ext4"));

    window.m_setTargetButton->click();
    QVERIFY2(!window.m_hostMaintenanceMode,
             "committing a repair target must end host-maintenance mode");
    QCOMPARE(window.m_previewTargetPath, fixture.diskPath);
    QCOMPARE(window.m_previewTargetComponentPath, fixture.rootPath);
    QVERIFY(window.m_systemTargetLabel->text().contains(fixture.rootPath));
    QVERIFY(!window.m_systemTargetLabel->text().contains(QStringLiteral("vdb1")));
    QCOMPARE(window.m_detailResolvedTarget->text(), fixture.rootPath);
    QCOMPARE(window.m_detailFilesystem->text(), QStringLiteral("ext4"));
    QVERIFY2(window.m_setTargetButton->isEnabled(),
             "Select Target must stay enabled for the committed eligible target");
    diskItem = deviceTreeTopLevelItem(window, fixture.diskPath);
    QVERIFY(diskItem);
    QVERIFY2(diskItem->text(1).contains(QStringLiteral("SELECTED TARGET")),
             qPrintable(QStringLiteral("the re-committed row must stay visibly marked: %1")
                            .arg(diskItem->text(1))));

    // --- merged from hostMaintenanceTargetSelectionRestoresRepairScope() ---
    {
        // Never adopt the previous phase's session file.
        MainWindow::s_activeSessionPath.clear();

        // The reported flow: Host Maintenance is active and the user selects an
        // eligible repair drive and presses Select Target. The selection must
        // leave host maintenance through the shared deselect path and commit the
        // target in one gesture, with the host card, diagnostics scope, session
        // scope and plan all back on the ordinary repair-target mode.
        QTemporaryDir fakeRoot;
        QVERIFY(fakeRoot.isValid());
        UnprivilegedVdbFixture fixture;
        const QString fixtureError = writeUnprivilegedVdbFixture(fakeRoot.path(), &fixture);
        QVERIFY2(fixtureError.isEmpty(), qPrintable(fixtureError));
        ScopedSessionLogDir logDir;
        QVERIFY(logDir.isValid());
        ScopedEnvironmentVariable lsblkOverride("BOOT_REPAIR_LSBLK", fixture.lsblkPath.toLocal8Bit());
        ScopedEnvironmentVariable udevOverride("BOOT_REPAIR_UDEV_DATA_DIR", fixture.udevDir.toLocal8Bit());

        MainWindow window;
        window.show();
        QTest::qWait(50);
        window.m_snapshotPreloadScheduled = true;
        window.m_uiTestPrivilegedSessionGranted = false;

        // Enter Host Maintenance through the real handler (the protected running
        // host tree is injected because the test host has no real protected disk).
        prepareRepairScope(window, true);
        window.m_hostMaintenanceMode = false;
        window.updateHostSystemSummary({window.m_deviceIndex.value(QStringLiteral("/dev/test-host"))});
        window.selectHostForMaintenance();
        QVERIFY(window.m_hostMaintenanceMode);
        QCOMPARE(window.m_hostMaintenanceButton->text(), QStringLiteral("Exit Host Maintenance"));
        QCOMPARE(window.m_diagnosticScopeLabel->text(),
                 QStringLiteral("Host maintenance: /dev/test-host"));
        QCOMPARE(window.m_sessionScopeLabel, QStringLiteral("Running Host"));

        // Select the eligible /dev/vdb target row while host maintenance is
        // active. Select Target must be enabled and the details must resolve the
        // real root component.
        QTreeWidgetItem *diskItem = deviceTreeTopLevelItem(window, fixture.diskPath);
        QVERIFY(diskItem);
        selectDeviceTreeItem(window, diskItem);
        QVERIFY2(window.m_setTargetButton->isEnabled(),
                 qPrintable(QStringLiteral("Select Target must be enabled while host maintenance "
                                           "is active (tooltip: %1)")
                                .arg(window.m_setTargetButton->toolTip())));
        QCOMPARE(window.m_detailResolvedTarget->text(), fixture.rootPath);
        QCOMPARE(window.m_detailFilesystem->text(), QStringLiteral("ext4"));

        window.m_setTargetButton->click();

        // One gesture: host maintenance is cleanly off and the target is committed.
        QVERIFY2(!window.m_hostMaintenanceMode,
                 "Select Target must automatically deselect host maintenance");
        QCOMPARE(window.m_hostMaintenanceButton->text(), QStringLiteral("Host Maintenance"));
        QVERIFY2(window.m_hostMaintenanceButton->toolTip().contains(
                     QStringLiteral("select the running host for deliberate guarded maintenance")),
                 qPrintable(QStringLiteral("the host card tooltip must return to its maintenance "
                                           "meaning: %1").arg(window.m_hostMaintenanceButton->toolTip())));
        QCOMPARE(window.m_previewTargetPath, fixture.diskPath);
        QCOMPARE(window.m_previewTargetComponentPath, fixture.rootPath);
        QCOMPARE(window.m_diagnosticScopeLabel->text(),
                 QStringLiteral("Target: %1").arg(fixture.diskPath));
        QCOMPARE(window.m_sessionScopeLabel, QStringLiteral("Repair Target"));
        QVERIFY(window.m_systemTargetLabel->text().contains(fixture.rootPath));
        QVERIFY2(!window.m_systemTargetLabel->text().contains(QStringLiteral("Host maintenance")),
                 qPrintable(QStringLiteral("the committed-target line must leave host maintenance: %1")
                                .arg(window.m_systemTargetLabel->text())));

        const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
        QVERIFY2(log.contains(QStringLiteral(
                     "Running-host maintenance deselected; ordinary repair-target mode restored.")),
                 "the shared host-maintenance deselection log line must be recorded");
        QVERIFY2(log.contains(QStringLiteral("best detected system component: %1").arg(fixture.rootPath)),
                 qPrintable(QStringLiteral("the selection log must name the resolved component: %1")
                                .arg(log)));
    }
}

// Regression: rapidly switching between Host Maintenance and repair targets
// while a privileged request is in flight used to leave the UI permanently
// busy ("Waiting for the running operation"). The re-entrant snapshot preload
// waited in a nested event loop for the active request's busy flag, but the
// active request could not clear the flag until that nested handler returned:
// a mutual wait. The gate must now refuse or defer every re-entrant request
// and always release the busy state.
void MainWindowUiTest::rapidScopeSwitchingDuringSnapshotPreloadDoesNotDeadlock()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_autoRefreshDiagnostics->setChecked(false);

    // Synthetic protected running host plus two eligible Btrfs repair targets.
    prepareRepairScope(window, true);
    window.m_hostMaintenanceMode = false;
    window.updateHostSystemSummary({window.m_deviceIndex.value(QStringLiteral("/dev/test-host"))});
    installSyntheticBtrfsTarget(window, QStringLiteral("/dev/test-target-a"),
                                QStringLiteral("/dev/test-target-a1"));
    installSyntheticBtrfsTarget(window, QStringLiteral("/dev/test-target-b"),
                                QStringLiteral("/dev/test-target-b1"));

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("rapid-scope-snapshot.log"));
    QVERIFY2(startSlowSnapshotFakePrivilegedSession(window, capturePath, QStringLiteral("0.8")),
             "the scripted privileged session must start");
    QVERIFY(window.m_privilegedSessionReady);

    // Commit target A. Its preload dispatches on the next event-loop turn and
    // then owns the gate for the scripted delay.
    selectTopLevelDisk(window, QStringLiteral("/dev/test-target-a"));
    window.setPreviewTarget();
    QCOMPARE(window.m_previewTargetPath, QStringLiteral("/dev/test-target-a"));

    // Rapid host/target transitions while the request is in flight. The
    // transitions run from a timer, so they are dispatched inside the active
    // request's event loop exactly like user clicks are; the preloads they
    // schedule used to wait in a nested event loop and deadlock.
    bool transitionsDone = false;
    bool gateHeldDuringTransition = false;
    bool snapshotBusyDuringTransition = false;
    QTimer transitions;
    transitions.setSingleShot(true);
    transitions.setInterval(100);
    QObject::connect(&transitions, &QTimer::timeout, &window, [&] {
        gateHeldDuringTransition = window.m_privilegedOperationActive;
        snapshotBusyDuringTransition = window.m_activeBusyOperations.contains(
            QStringLiteral("Loading Btrfs snapshots"));
        window.selectHostForMaintenance();
        selectTopLevelDisk(window, QStringLiteral("/dev/test-target-b"));
        window.setPreviewTarget();
        window.selectHostForMaintenance();
        selectTopLevelDisk(window, QStringLiteral("/dev/test-target-a"));
        window.setPreviewTarget();
        transitionsDone = true;
    });
    transitions.start();

    // Fail fast instead of the 5-minute QtTest watchdog if the gate deadlocks:
    // timers keep firing inside a nested wait loop, so this aborts the test
    // process even when the test's own event loop can no longer return.
    QTimer deadlockGuard;
    deadlockGuard.setSingleShot(true);
    deadlockGuard.setInterval(15000);
    QObject::connect(&deadlockGuard, &QTimer::timeout, [] {
        qFatal("rapid scope switching deadlocked the privileged request gate");
    });
    deadlockGuard.start();

    QTRY_VERIFY_WITH_TIMEOUT(transitionsDone && !window.m_privilegedOperationActive
                                 && window.m_activeBusyOperations.isEmpty(),
                             8000);
    deadlockGuard.stop();

    QVERIFY2(gateHeldDuringTransition,
             "the scope transitions must arrive while the snapshot request owns the gate");
    QVERIFY2(snapshotBusyDuringTransition,
             "the snapshot request must be the busy operation during the transitions");
    QVERIFY2(window.m_busyIndicator->isHidden(),
             "the busy indicator must clear after the request completes");
    QVERIFY2(!window.m_hostMaintenanceMode,
             "the final transition must restore the repair-target scope");
    QCOMPARE(window.m_previewTargetPath, QStringLiteral("/dev/test-target-a"));

    const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QVERIFY2(!log.contains(QStringLiteral("is waiting for the running operation to finish")),
             "a re-entrant request must never queue on the busy gate");
    QVERIFY2(log.contains(QStringLiteral("is deferred until the running privileged operation finishes")),
             qPrintable(QStringLiteral("the deferred snapshot preload must be recorded: %1").arg(log)));

    // Rapid switching must not accumulate snapshot loads: the in-flight load
    // already matches the final target, so the deferred retry is satisfied
    // without a second request.
    const QList<QStringList> requests = capturedHelperRequests(capturePath);
    int snapshotRequests = 0;
    for (const QStringList &request : requests) {
        if (!request.isEmpty() && request.first() == QStringLiteral("snapshots")) {
            ++snapshotRequests;
        }
    }
    QCOMPARE(snapshotRequests, 1);
    QVERIFY2(window.m_snapshotTable->rowCount() == 1,
             "the in-flight inventory must populate the final target's table");
}

// The bounded safety net: a helper that never answers a request must not keep
// the UI busy forever. The watchdog aborts the wait with a clear log line,
// closes the unresponsive session and releases the gate.
void MainWindowUiTest::privilegedRequestSafetyTimeoutAbortsWedgedHelper()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);
    window.m_privilegedRequestTimeoutMs = 300;

    // Commit a synthetic Btrfs target directly so no preload is scheduled;
    // this test calls loadSnapshots() itself.
    installSyntheticBtrfsTarget(window, QStringLiteral("/dev/test-target-a"),
                                QStringLiteral("/dev/test-target-a1"));
    window.m_previewTargetPath = QStringLiteral("/dev/test-target-a");
    window.m_previewTargetComponentPath = QStringLiteral("/dev/test-target-a1");
    window.m_targetDiagnosticCacheIdentity = window.currentTargetDiagnosticCacheIdentity();
    window.updateTargetLabels();

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("timeout-snapshot.log"));
    QVERIFY2(startSlowSnapshotFakePrivilegedSession(window, capturePath, QStringLiteral("30")),
             "the scripted privileged session must start");
    QVERIFY(window.m_privilegedSessionReady);

    window.loadSnapshots();

    QVERIFY2(!window.m_privilegedOperationActive,
             "the safety timeout must release the privileged gate");
    QVERIFY(window.m_activeBusyOperations.isEmpty());
    QVERIFY2(window.m_busyIndicator->isHidden(),
             "the busy indicator must clear after the timeout");
    QVERIFY2(!window.m_privilegedSession
                 || window.m_privilegedSession->state() == QProcess::NotRunning,
             "the timed-out helper session must be closed");
    const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QVERIFY2(log.contains(QStringLiteral("safety limit")),
             qPrintable(QStringLiteral("the timeout must be recorded: %1").arg(log)));
    QVERIFY2(window.m_snapshotDetails->toPlainText().contains(QStringLiteral("safety limit")),
             "the captured output must explain the timeout");
}

void MainWindowUiTest::targetDiagnosticFallbackUpdatesCommittedComponent()
{
    // The privileged helper confirms the installed root by mounting it. When
    // the committed component lacks /etc/os-release and the helper resolves a
    // different Linux-capable partition on the same disk, the committed
    // target must adopt the resolved component so the log, the committed
    // target line and every later request stay consistent. An ordinary
    // mapper/dm-N spelling change must never rewrite the committed component.
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;

    DeviceNode boot;
    boot.path = QStringLiteral("/dev/test-vdb1");
    boot.type = QStringLiteral("part");
    boot.fileSystem = QStringLiteral("ext4");
    boot.linuxCapableFileSystem = true;
    DeviceNode root;
    root.path = QStringLiteral("/dev/test-vdb3");
    root.type = QStringLiteral("part");
    root.fileSystem = QStringLiteral("ext4");
    root.linuxCapableFileSystem = true;
    DeviceNode disk;
    disk.path = QStringLiteral("/dev/test-vdb");
    disk.type = QStringLiteral("disk");
    disk.children.append(boot);
    disk.children.append(root);
    window.m_deviceIndex.insert(disk.path, disk);
    window.m_deviceIndex.insert(boot.path, boot);
    window.m_deviceIndex.insert(root.path, root);
    window.m_previewTargetPath = disk.path;
    window.m_previewTargetComponentPath = boot.path;
    window.updateTargetLabels();

    const QString helperOutput = QStringLiteral(
        "[12:00:01] Protected host check: PASS\n"
        "[12:00:01] Target disk: /dev/test-vdb\n"
        "[12:00:01] Root component: /dev/test-vdb1 (ext4)\n"
        "[12:00:02] Root component fallback: selected component /dev/test-vdb1 (ext4) lacks /etc/os-release; resolved /dev/test-vdb3 (ext4) from /dev/test-vdb.\n"
        "[12:00:02] Detected target OS: Alpine Linux v3.24\n");
    window.appendDiagnosticLog(QStringLiteral("environment"),
                               QStringLiteral("Environment validation"),
                               QStringLiteral("Repair Target"), helperOutput, true);

    QCOMPARE(window.m_previewTargetComponentPath, QStringLiteral("/dev/test-vdb3"));
    QVERIFY2(window.m_systemTargetLabel->text().contains(QStringLiteral("/dev/test-vdb3")),
             qPrintable(QStringLiteral("the committed target must adopt the resolved component: %1")
                            .arg(window.m_systemTargetLabel->text())));
    const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QVERIFY2(log.contains(QStringLiteral(
                 "Target root component confirmed by privileged evidence: /dev/test-vdb3 (was /dev/test-vdb1)")),
             qPrintable(QStringLiteral("the confirmation must be visible in the log: %1").arg(log)));

    // A normal spelling change (mapper alias versus dm-N) is not a fallback
    // and must leave the committed component untouched.
    window.m_previewTargetComponentPath = boot.path;
    window.appendDiagnosticLog(QStringLiteral("boot"), QStringLiteral("Boot diagnostics"),
                               QStringLiteral("Repair Target"),
                               QStringLiteral("[12:00:03] Root component: /dev/dm-0 (ext4)\n"), true);
    QCOMPARE(window.m_previewTargetComponentPath, boot.path);
}

void MainWindowUiTest::nonBtrfsTargetSnapshotInventoryIsInformational()
{
    // A non-Btrfs repair target has no Btrfs snapshot inventory. Selecting it
    // must not produce a failed snapshot state or an ERROR-styled log entry:
    // the page states the applicability and the load/inspect/rollback
    // controls stay disabled.
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = false;

    prepareRepairScope(window); // /dev/test-system -> /dev/test-root (ext4)
    QVERIFY(window.m_previewTargetPath == QStringLiteral("/dev/test-system"));
    QCOMPARE(window.m_deviceIndex.value(window.m_previewTargetComponentPath).fileSystem,
             QStringLiteral("ext4"));
    window.updateTargetLabels();

    // The automatic preload must resolve the applicability without any
    // privileged request.
    window.scheduleSnapshotPreload();
    QTest::qWait(50);
    const QString details = window.m_snapshotDetails->toPlainText();
    QVERIFY2(details.contains(QStringLiteral("not applicable")),
             qPrintable(QStringLiteral("the Snapshots page must explain the non-Btrfs target: %1")
                            .arg(details)));
    QVERIFY2(details.contains(QStringLiteral("ext4")),
             qPrintable(QStringLiteral("the Snapshots page must name the detected filesystem: %1")
                            .arg(details)));
    QVERIFY2(!window.m_snapshotLoadButton->isEnabled(),
             "the Load Btrfs snapshots control must stay disabled for a non-Btrfs target");
    QVERIFY(!window.m_snapshotInspectButton->isEnabled());
    QVERIFY(!window.m_snapshotRollbackButton->isEnabled());

    // Explicitly running the inventory follows the same informational path.
    window.loadSnapshots();
    const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QVERIFY2(log.contains(QStringLiteral("Btrfs snapshot inventory is not applicable")),
             qPrintable(QStringLiteral("the log must record the informational outcome: %1").arg(log)));
    QVERIFY2(!log.contains(QStringLiteral("Btrfs snapshot inventory failed.")),
             "a non-Btrfs target must never produce the snapshot failure log line");
    QVERIFY2(!log.contains(QStringLiteral("Load Btrfs snapshots finished with exit code 1")),
             "a non-Btrfs target must never produce an error-styled snapshot completion");
    QVERIFY(!window.m_snapshotTable->rowCount());
}

void MainWindowUiTest::guardedWriteActionsStayDisabledWithoutTarget()
{
    MainWindow window;
    window.show();
    QTest::qWait(100);

    // No target is selected in the test environment. The write-capable repair,
    // copy, snapshot inspection, and rollback controls must remain guarded.
    QVERIFY(window.m_runFullRepairButton);
    QVERIFY(!window.m_runFullRepairButton->isEnabled());
    QVERIFY(window.m_repairToolButton);
    QVERIFY(!window.m_repairToolButton->isEnabled());
    QVERIFY(window.m_fileCopyPreviewButton);
    QVERIFY(!window.m_fileCopyPreviewButton->isEnabled());
    QVERIFY(window.m_fileCopyRunButton);
    QVERIFY(!window.m_fileCopyRunButton->isEnabled());
    QVERIFY(window.m_fileCopyDirectionCombo);
    window.m_fileCopyDirectionCombo->setCurrentIndex(1);
    window.m_fileCopyDirectionCombo->setCurrentIndex(0);
    QVERIFY(window.m_saveDiagnosticButton);
    QVERIFY(!window.m_saveDiagnosticButton->isEnabled());
    QVERIFY(window.m_snapshotLoadButton);
    QVERIFY(!window.m_snapshotLoadButton->isEnabled());
    QVERIFY(window.m_snapshotInspectButton);
    QVERIFY(!window.m_snapshotInspectButton->isEnabled());
    QVERIFY(window.m_snapshotRollbackButton);
    QVERIFY(!window.m_snapshotRollbackButton->isEnabled());

    // Changing File Copy direction clears staged paths and records the user
    // action without invoking a helper or modifying a file.
    QVERIFY(window.m_fileCopyDirectionCombo);
    QVERIFY(window.m_sourceList);
    QVERIFY(window.m_destinationEdit);
    window.m_fileCopyDirectionCombo->setCurrentIndex(1);
    QCOMPARE(window.m_sourceList->count(), 0);
    QVERIFY(window.m_destinationEdit->text().isEmpty());
    window.m_fileCopyDirectionCombo->setCurrentIndex(0);
    QVERIFY(window.m_actionLogEntries.join(QLatin1Char('\n')).contains(QStringLiteral("File Copy direction changed")));

    // Clicking guarded controls without a committed target must be a no-op;
    // no privileged request or write confirmation is allowed to appear.
    window.m_fileCopyPreviewButton->click();
    window.m_fileCopyRunButton->click();
    window.m_runFullRepairButton->click();
    window.m_repairToolButton->click();
    window.m_snapshotLoadButton->click();
    window.m_snapshotRollbackButton->click();
    QVERIFY(!window.m_actionLogEntries.join(QLatin1Char('\n')).contains(QStringLiteral("Privileged action started:")));
}

void MainWindowUiTest::exportDialogsCanBeCancelledReadOnly()
{
    MainWindow window;
    window.show();
    QTest::qWait(100);
    enterHostDiagnosticScope(window);
    window.m_diagnosticList->setCurrentRow(0);
    window.runSelectedDiagnostic();
    QVERIFY(window.m_diagnosticResults && !window.m_diagnosticResults->toPlainText().isEmpty());

    // Saving an export writes only a user-selected report file. Exercise both
    // modal export actions and cancel their dialogs so this test never writes
    // outside its temporary settings directory.
    auto cancelNextFileDialog = [] {
        QTimer *poll = new QTimer(qApp);
        poll->setInterval(10);
        QObject::connect(poll, &QTimer::timeout, poll, [poll] {
            for (QWidget *top : QApplication::topLevelWidgets()) {
                if (auto *dialog = qobject_cast<QFileDialog *>(top)) {
                    poll->stop();
                    dialog->reject();
                    poll->deleteLater();
                    return;
                }
            }
        });
        poll->start();
    };

    cancelNextFileDialog();
    window.saveDiagnosticResults();
    cancelNextFileDialog();
    window.saveLogAs();
    QVERIFY(window.m_actionLogEntries.join(QLatin1Char('\n')).contains(QStringLiteral("Host diagnostic")));
}

void MainWindowUiTest::actionRegisterPersistsAcrossWindows()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());
    const QString marker = QStringLiteral("UI_TEST_REGISTER_MARKER");
    QString activePath;
    {
        MainWindow window;
        // The session file only starts once a scope is identified.
        prepareRepairScope(window, true);
        window.updateSessionScope();
        window.appendLog(marker, QStringLiteral("TEST"));
        QVERIFY(window.m_actionLogEntries.join(QLatin1Char('\n')).contains(marker));
        QCOMPARE(sessionLogFiles(logDir.path()).size(), 1);
        activePath = window.m_sessionLogPath;
        QVERIFY(!activePath.isEmpty());
        QCOMPARE(MainWindow::s_activeSessionPath, activePath);
        QVERIFY(readSessionFile(activePath).contains(marker));
        window.m_tabs->setCurrentIndex(5);
    }

    // A second window in the same process continues this run's active session
    // instead of starting another file or adopting an older one.
    MainWindow reopened;
    QCOMPARE(reopened.m_tabs->currentIndex(), 0);
    QCOMPARE(reopened.m_sessionLogPath, activePath);
    QCOMPARE(MainWindow::s_activeSessionPath, activePath);
    QVERIFY(reopened.m_actionLogEntries.join(QLatin1Char('\n')).contains(marker));
    flushLogRefresh(reopened);
    QVERIFY(reopened.m_logView->toPlainText().contains(marker));
    // The active file is the first entry and is never listed as a prior one.
    QCOMPARE(reopened.m_sessionLogList->count(), 1);
    QVERIFY(reopened.m_sessionLogList->item(0)->data(Qt::UserRole).toString().isEmpty());
    QVERIFY(!reopened.m_sessionLogList->item(0)->text().contains(QStringLiteral("not started")));
    QCOMPARE(sessionLogFiles(logDir.path()).size(), 1);
    // Construction in the same process must not disturb the run's active file:
    // the restored scope survives and no "No scope" marker is appended.
    QCOMPARE(reopened.m_sessionScopeLabel, QStringLiteral("Running Host"));
    const QString activeContent = readSessionFile(activePath);
    QCOMPARE(activeContent.count(QStringLiteral("[SCOPE]")), 1);
    QVERIFY(!activeContent.contains(QStringLiteral("No scope")));
}

void MainWindowUiTest::freshLaunchDoesNotAdoptNewestPriorSession()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());
    const QString priorPath = logDir.path() + QStringLiteral("/session-20200101-000000.log");
    writeSessionLog(priorPath,
                    QStringLiteral("[2020-01-01 00:00:00] [SCOPE] Running Host | Disk: /dev/prior | Root: /dev/prior1 | OS: debian"),
                    QStringLiteral("UI_TEST_PRIOR_LAUNCH_MARKER"));
    const QString priorContent = readSessionFile(priorPath);

    // Simulate a fresh launch: this process owns no active session yet.
    MainWindow::s_activeSessionPath.clear();
    MainWindow window;
    window.show();
    QTest::qWait(50);

    // The previous run's file is never loaded as the current session.
    QVERIFY(window.m_sessionLogPath.isEmpty());
    QVERIFY(!window.m_sessionLogFile);
    QVERIFY2(!window.m_actionLogEntries.join(QLatin1Char('\n')).contains(QStringLiteral("UI_TEST_PRIOR_LAUNCH_MARKER")),
             "a fresh launch must not load the newest prior session into the live log");
    QCOMPARE(window.m_sessionLogList->count(), 2);
    QVERIFY(window.m_sessionLogList->item(0)->data(Qt::UserRole).toString().isEmpty());
    QVERIFY2(window.m_sessionLogList->item(0)->text().contains(QStringLiteral("not started")),
             "the live entry must state that no session file exists yet");
    QVERIFY2(!window.m_sessionLogList->item(0)->text().contains(QStringLiteral("Running Host")),
             "the previous run's session must never be labeled as current");
    QCOMPARE(window.m_sessionLogList->item(1)->data(Qt::UserRole).toString(), priorPath);
    QVERIFY(window.m_sessionLogList->item(1)->text().contains(QStringLiteral("Running Host")));

    // The prior file stays selectable read-only.
    window.m_tabs->setCurrentIndex(6);
    QCoreApplication::processEvents();
    window.m_sessionLogList->setCurrentRow(1);
    QCoreApplication::processEvents();
    QVERIFY(window.m_priorLogBanner->isVisible());
    QVERIFY(window.m_logView->toPlainText().contains(QStringLiteral("UI_TEST_PRIOR_LAUNCH_MARKER")));
    QVERIFY(window.m_deleteSessionLogButton->isEnabled());

    // Identifying a scope creates this run's file with a new timestamp.
    prepareRepairScope(window, true);
    window.updateSessionScope();
    QVERIFY(!window.m_sessionLogPath.isEmpty());
    QVERIFY(window.m_sessionLogPath != priorPath);
    QCOMPARE(MainWindow::s_activeSessionPath, window.m_sessionLogPath);
    QVERIFY(sessionLogFiles(logDir.path()).contains(window.m_sessionLogPath));
    QCOMPARE(sessionLogFiles(logDir.path()).size(), 2);

    // The live entry becomes the current session; the old file stays prior
    // and byte-for-byte unchanged.
    window.m_sessionLogList->setCurrentRow(0);
    QCoreApplication::processEvents();
    QVERIFY(!window.m_priorLogBanner->isVisible());
    QCOMPARE(window.m_sessionLogList->count(), 2);
    QVERIFY(window.m_sessionLogList->item(0)->data(Qt::UserRole).toString().isEmpty());
    QVERIFY(window.m_sessionLogList->item(0)->text().startsWith(QStringLiteral("Current session")));
    QVERIFY(!window.m_sessionLogList->item(0)->text().contains(QStringLiteral("not started")));
    QCOMPARE(window.m_sessionLogList->item(1)->data(Qt::UserRole).toString(), priorPath);
    QVERIFY2(readSessionFile(priorPath) == priorContent,
             "the previous run's session file must never be modified by a fresh launch");
    QVERIFY(readSessionFile(window.m_sessionLogPath).contains(QStringLiteral("Running Host")));
    QVERIFY(!readSessionFile(window.m_sessionLogPath).contains(QStringLiteral("UI_TEST_PRIOR_LAUNCH_MARKER")));
}

void MainWindowUiTest::sessionLogDefersUntilScopeIdentifiedThenAppends()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());
    MainWindow window;
    window.show();
    QTest::qWait(50);

    const QString marker = QStringLiteral("UI_TEST_DEFERRED_MARKER");
    window.appendLog(marker);
    QVERIFY(window.m_actionLogEntries.join(QLatin1Char('\n')).contains(marker));
    QVERIFY2(sessionLogFiles(logDir.path()).isEmpty(),
             "no session file may exist before a scope is identified");

    prepareRepairScope(window, true);
    window.updateSessionScope();
    const QStringList files = sessionLogFiles(logDir.path());
    QCOMPARE(files.size(), 1);
    const QString content = readSessionFile(files.first());
    QVERIFY(content.contains(QStringLiteral("[SCOPE]")));
    QVERIFY(content.contains(QStringLiteral("Running Host")));
    QVERIFY(content.contains(QStringLiteral("/dev/test-host")));
    QVERIFY2(content.contains(marker), "buffered entries must be flushed into the new session file");
    QVERIFY(content.indexOf(QStringLiteral("[SCOPE]")) < content.indexOf(marker));

    // --- merged from sessionLogAppendsAfterCreation() ---
    {
        // Never adopt the previous phase's session file.
        MainWindow::s_activeSessionPath.clear();
        ScopedSessionLogDir logDir;
        QVERIFY(logDir.isValid());
        MainWindow window;
        window.show();
        QTest::qWait(50);

        prepareRepairScope(window, true);
        window.updateSessionScope();
        const QStringList files = sessionLogFiles(logDir.path());
        QCOMPARE(files.size(), 1);

        const QString marker = QStringLiteral("UI_TEST_APPEND_AFTER_CREATION");
        window.appendLog(marker);
        QVERIFY(readSessionFile(files.first()).contains(marker));
        QVERIFY(window.m_actionLogEntries.join(QLatin1Char('\n')).contains(marker));
    }
}

void MainWindowUiTest::sessionLogScopeMarkersTrackTarget()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());
    MainWindow window;
    window.show();
    QTest::qWait(50);

    prepareRepairScope(window);
    cacheRepairEvidence(window, capabilityEvidence(false, false));
    window.updateSessionScope();
    const QStringList files = sessionLogFiles(logDir.path());
    QCOMPARE(files.size(), 1);
    QString content = readSessionFile(files.first());
    QVERIFY(content.contains(QStringLiteral("[SCOPE]")));
    QVERIFY(content.contains(QStringLiteral("Repair Target")));
    QVERIFY(content.contains(QStringLiteral("/dev/test-system")));
    QVERIFY(content.contains(QStringLiteral("/dev/test-root")));
    QVERIFY(content.contains(QStringLiteral("OS: debian")));
    QCOMPARE(content.count(QStringLiteral("[SCOPE]")), 1);

    // Switching to another drive appends a second marker so both environments
    // stay delineated in one session file.
    DeviceNode component;
    component.path = QStringLiteral("/dev/other-root");
    component.type = QStringLiteral("part");
    component.fileSystem = QStringLiteral("ext4");
    component.linuxCapableFileSystem = true;
    component.installedLinux = true;
    DeviceNode disk;
    disk.path = QStringLiteral("/dev/other-system");
    disk.type = QStringLiteral("disk");
    disk.children.append(component);
    window.m_deviceIndex.insert(disk.path, disk);
    window.m_deviceIndex.insert(component.path, component);
    window.m_previewTargetPath = disk.path;
    window.m_previewTargetComponentPath = component.path;
    window.m_targetDiagnosticCacheIdentity = window.currentTargetDiagnosticCacheIdentity();
    window.updateSessionScope();
    content = readSessionFile(files.first());
    QCOMPARE(content.count(QStringLiteral("[SCOPE]")), 2);
    QVERIFY(content.contains(QStringLiteral("/dev/other-system")));
    QVERIFY(content.contains(QStringLiteral("/dev/other-root")));
}

void MainWindowUiTest::priorSessionLogsAreListedAndReadOnly()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());
    const QString olderPath = logDir.path() + QStringLiteral("/session-20200101-000000.log");
    const QString newerPath = logDir.path() + QStringLiteral("/session-20210101-000000.log");
    writeSessionLog(olderPath,
                    QStringLiteral("[2020-01-01 00:00:00] [SCOPE] Repair Target | Disk: /dev/older | Root: /dev/older1 | OS: debian"),
                    QStringLiteral("PRIOR_OLDER_MARKER"));
    writeSessionLog(newerPath,
                    QStringLiteral("[2021-01-01 00:00:00] [SCOPE] Running Host | Disk: /dev/newer | Root: /dev/newer1 | OS: arch"),
                    QStringLiteral("PRIOR_NEWER_MARKER"));

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_tabs->setCurrentIndex(6); // Logs; widgets must be visible to assert banners
    QCoreApplication::processEvents();
    QVERIFY(window.m_sessionLogList);
    QCOMPARE(window.m_sessionLogList->count(), 3);
    QVERIFY(window.m_sessionLogList->item(0)->data(Qt::UserRole).toString().isEmpty());
    QCOMPARE(window.m_sessionLogList->item(1)->data(Qt::UserRole).toString(), newerPath);
    QCOMPARE(window.m_sessionLogList->item(2)->data(Qt::UserRole).toString(), olderPath);
    QVERIFY(window.m_sessionLogList->item(1)->text().contains(QStringLiteral("2021-01-01 00:00:00")));
    QVERIFY(window.m_sessionLogList->item(1)->text().contains(QStringLiteral("Running Host")));
    QVERIFY(window.m_sessionLogList->item(1)->text().contains(QStringLiteral("/dev/newer")));

    window.m_sessionLogList->setCurrentRow(1);
    QCoreApplication::processEvents();
    QVERIFY(window.m_priorLogBanner->isVisible());
    QVERIFY(window.m_priorLogBanner->text().contains(QStringLiteral("session-20210101-000000.log")));
    QVERIFY(window.m_priorLogBanner->text().contains(QStringLiteral("2021-01-01 00:00:00")));
    QVERIFY(window.m_priorLogBanner->text().contains(QStringLiteral("read only")));
    QVERIFY(window.m_logView->toPlainText().contains(QStringLiteral("PRIOR_NEWER_MARKER")));
    QVERIFY(!window.m_logView->toPlainText().contains(QStringLiteral("PRIOR_OLDER_MARKER")));
    QVERIFY(window.m_deleteSessionLogButton->isEnabled());

    // The search box keeps working for whichever log is displayed.
    window.m_logSearchEdit->setText(QStringLiteral("prior newer"));
    QVERIFY(window.m_logView->toPlainText().contains(QStringLiteral("PRIOR_NEWER_MARKER")));
    window.m_logSearchEdit->clear();

    window.m_sessionLogList->setCurrentRow(0);
    QCoreApplication::processEvents();
    QVERIFY(!window.m_priorLogBanner->isVisible());
    QVERIFY(!window.m_deleteSessionLogButton->isEnabled());
}

void MainWindowUiTest::activeSessionFileCannotBeDeleted()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());
    MainWindow window;
    window.show();
    QTest::qWait(50);

    prepareRepairScope(window, true);
    window.updateSessionScope();
    QCOMPARE(sessionLogFiles(logDir.path()).size(), 1);
    // The active file is represented by the live entry only.
    QCOMPARE(window.m_sessionLogList->count(), 1);
    QVERIFY(window.m_sessionLogList->currentItem());
    QVERIFY(window.m_sessionLogList->currentItem()->data(Qt::UserRole).toString().isEmpty());
    QVERIFY(!window.m_deleteSessionLogButton->isEnabled());
}

void MainWindowUiTest::sessionLogHeaderShowsGenerationTimestamp()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());
    MainWindow window;
    window.show();
    QTest::qWait(50);

    prepareRepairScope(window, true);
    window.updateSessionScope();
    const QStringList files = sessionLogFiles(logDir.path());
    QCOMPARE(files.size(), 1);
    const QString fileName = QFileInfo(files.first()).fileName();
    const QString content = readSessionFile(files.first());
    const QRegularExpression header(
        QStringLiteral("──────── APPLICATION ────────\n"
                       "\\[\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\] \\[INFO\\] Session log %1 generated "
                       "\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}")
            .arg(QRegularExpression::escape(fileName)));
    QVERIFY2(header.match(content).hasMatch(),
             "the session file must open with a visible APPLICATION generation header");
    QVERIFY(header.match(window.m_actionLogEntries.join(QLatin1Char('\n'))).hasMatch());
    QVERIFY(header.match(window.m_logView->toPlainText()).hasMatch());
    QVERIFY(content.indexOf(QStringLiteral("[SCOPE]")) < content.indexOf(QStringLiteral("generated")));
}

void MainWindowUiTest::priorSessionLogLabelFallsBackToFileTime()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());
    const QString path = logDir.path() + QStringLiteral("/session-not-a-timestamp.log");
    writeSessionLog(path,
                    QStringLiteral("[2020-01-01 00:00:00] [SCOPE] Running Host | Disk: /dev/mtime | Root: /dev/mtime1 | OS: debian"),
                    QStringLiteral("MTIME_MARKER"));
    const QDateTime modified(QDate(2019, 5, 4), QTime(3, 2, 1));
    QFile stampFile(path);
    QVERIFY(stampFile.open(QIODevice::ReadWrite));
    QVERIFY(stampFile.setFileTime(modified, QFileDevice::FileModificationTime));
    stampFile.close();

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_tabs->setCurrentIndex(6);
    QCoreApplication::processEvents();

    int row = -1;
    for (int index = 0; index < window.m_sessionLogList->count(); ++index) {
        if (window.m_sessionLogList->item(index)->data(Qt::UserRole).toString() == path) {
            row = index;
            break;
        }
    }
    QVERIFY(row >= 0);
    QVERIFY(window.m_sessionLogList->item(row)->text().contains(QStringLiteral("2019-05-04 03:02:01")));
    QVERIFY(window.m_sessionLogList->item(row)->text().contains(QStringLiteral("Running Host")));

    window.m_sessionLogList->setCurrentRow(row);
    QCoreApplication::processEvents();
    QVERIFY(window.m_priorLogBanner->isVisible());
    QVERIFY(window.m_priorLogBanner->text().contains(QStringLiteral("2019-05-04 03:02:01")));
}

void MainWindowUiTest::clearTruncatesOnlyActiveSessionLog()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());
    const QString priorPath = logDir.path() + QStringLiteral("/session-20200101-000000.log");
    writeSessionLog(priorPath,
                    QStringLiteral("[2020-01-01 00:00:00] [SCOPE] Repair Target | Disk: /dev/older | Root: /dev/older1 | OS: debian"),
                    QStringLiteral("PRIOR_CLEAR_MARKER"));

    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareRepairScope(window, true);
    window.updateSessionScope();

    QCOMPARE(sessionLogFiles(logDir.path()).size(), 2);
    const QString activePath = window.m_sessionLogPath;
    QVERIFY(!activePath.isEmpty());
    QVERIFY(activePath != priorPath);
    const QString activeMarker = QStringLiteral("UI_TEST_CLEAR_ACTIVE_MARKER");
    window.appendLog(activeMarker);
    QVERIFY(readSessionFile(activePath).contains(activeMarker));

    // Cancelling the confirmation must leave the active session untouched.
    acceptNextMessageBox(&window, QMessageBox::Cancel);
    window.clearCurrentSessionLog();
    QVERIFY(readSessionFile(activePath).contains(activeMarker));

    acceptNextMessageBox(&window, QMessageBox::Yes);
    window.clearCurrentSessionLog();

    const QString activeContent = readSessionFile(activePath);
    QVERIFY2(QRegularExpression(QStringLiteral("Session log cleared by user at \\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}"))
                 .match(activeContent).hasMatch(),
             "the truncated file must contain the cleared-by-user timestamp entry");
    QVERIFY(!activeContent.contains(activeMarker));
    QVERIFY(!activeContent.contains(QStringLiteral("[SCOPE]")));
    QVERIFY(!window.m_actionLogEntries.join(QLatin1Char('\n')).contains(activeMarker));
    QVERIFY(window.m_actionLogEntries.join(QLatin1Char('\n')).contains(QStringLiteral("Session log cleared by user at")));
    QVERIFY(window.m_logView->toPlainText().contains(QStringLiteral("Session log cleared by user at")));

    QVERIFY2(readSessionFile(priorPath).contains(QStringLiteral("PRIOR_CLEAR_MARKER")),
             "clearing the active session must never touch prior session files");
}

void MainWindowUiTest::clearWithoutSessionFileClearsMemoryOnly()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());
    MainWindow window;
    window.show();
    QTest::qWait(50);

    const QString marker = QStringLiteral("UI_TEST_NO_FILE_CLEAR_MARKER");
    window.appendLog(marker);
    QVERIFY(sessionLogFiles(logDir.path()).isEmpty());

    acceptNextMessageBox(&window, QMessageBox::Yes);
    window.clearCurrentSessionLog();

    QVERIFY(!window.m_actionLogEntries.join(QLatin1Char('\n')).contains(marker));
    QVERIFY(window.m_actionLogEntries.join(QLatin1Char('\n')).contains(QStringLiteral("no session file exists yet")));
    QVERIFY(window.m_logView->toPlainText().contains(QStringLiteral("no session file exists yet")));
    QVERIFY2(sessionLogFiles(logDir.path()).isEmpty(),
             "clear must not create a session file before a scope exists");
}

void MainWindowUiTest::addNoteAppendsTimestampedEntry()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());
    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareRepairScope(window, true);
    window.updateSessionScope();
    const QString activePath = window.m_sessionLogPath;
    QVERIFY(!activePath.isEmpty());

    acceptNextInputDialog(&window, QStringLiteral("UI_TEST_NOTE_TEXT"));
    window.addSessionNote();
    const QString noteEntry = QStringLiteral("NOTE: UI_TEST_NOTE_TEXT");
    QVERIFY(window.m_actionLogEntries.join(QLatin1Char('\n')).contains(noteEntry));
    flushLogRefresh(window);
    QVERIFY(window.m_logView->toPlainText().contains(noteEntry));
    QVERIFY(readSessionFile(activePath).contains(noteEntry));
    QVERIFY(window.m_actionLogEntries.first().contains(QStringLiteral("──────── APPLICATION ────────")));

    const int before = window.m_actionLogEntries.size();
    acceptNextInputDialog(&window, QString());
    window.addSessionNote();
    QCOMPARE(window.m_actionLogEntries.size(), before);
}

void MainWindowUiTest::sessionLogRetentionPrunesOldFilesAndKeepsActive()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());
    for (int index = 0; index < 25; ++index) {
        const QString name = QStringLiteral("session-202001%1-%2.log")
                                 .arg(index + 1, 2, 10, QLatin1Char('0'))
                                 .arg(index + 1, 6, 10, QLatin1Char('0'));
        writeSessionLog(logDir.path() + QLatin1Char('/') + name,
                        QStringLiteral("[2020-01-01 00:00:00] [SCOPE] Repair Target | Disk: /dev/old | Root: /dev/old1 | OS: debian"),
                        QStringLiteral("RETENTION_MARKER"));
    }
    QCOMPARE(sessionLogFiles(logDir.path()).size(), 25);

    MainWindow window;
    window.show();
    QTest::qWait(50);
    const QStringList remaining = sessionLogFiles(logDir.path());
    QCOMPARE(remaining.size(), 20);
    QVERIFY2(!remaining.contains(logDir.path() + QStringLiteral("/session-20200101-000001.log")),
             "oldest session files are pruned first");
    QVERIFY(window.m_actionLogEntries.join(QLatin1Char('\n')).contains(QStringLiteral("retention")));

    // --- merged from retentionKeepsActiveSessionFile() ---
    {
        ScopedSessionLogDir logDir;
        QVERIFY(logDir.isValid());
        for (int index = 0; index < 25; ++index) {
            const QString name = QStringLiteral("session-202001%1-%2.log")
                                     .arg(index + 1, 2, 10, QLatin1Char('0'))
                                     .arg(index + 1, 6, 10, QLatin1Char('0'));
            writeSessionLog(logDir.path() + QLatin1Char('/') + name,
                            QStringLiteral("[2020-01-01 00:00:00] [SCOPE] Repair Target | Disk: /dev/old | Root: /dev/old1 | OS: debian"),
                            QStringLiteral("RETENTION_MARKER"));
        }
        const QStringList before = sessionLogFiles(logDir.path());
        QCOMPARE(before.size(), 25);
        // The newest file is this run's active session, as if it had been created
        // earlier in the process. Retention must never prune it.
        const QString activePath = before.last();
        MainWindow::s_activeSessionPath = activePath;

        MainWindow window;
        window.show();
        QTest::qWait(50);
        QCOMPARE(window.m_sessionLogPath, activePath);
        const QStringList remaining = sessionLogFiles(logDir.path());
        QCOMPARE(remaining.size(), 21);
        QVERIFY2(remaining.contains(activePath), "retention must never prune the active session file");
        QVERIFY2(!remaining.contains(logDir.path() + QStringLiteral("/session-20200101-000001.log")),
                 "prior session files are still pruned oldest-first around the active file");
    }
}

void MainWindowUiTest::scopeSwitchingRejectsStaleTargetEvidence()
{
    MainWindow window;
    prepareRepairScope(window);
    cacheRepairEvidence(window, capabilityEvidence(true, false));
    prepareRepairScope(window, true);
    cacheRepairEvidence(window, capabilityEvidence(false, true));
    window.m_fullRepairDkms->setChecked(true);
    auto *item = repairItem(window, QStringLiteral("dkms"));
    QVERIFY(item);
    window.m_repairToolTree->setCurrentItem(item);
    window.updateFullRepairSummary();
    QVERIFY(window.m_fullRepairDkms->isEnabled());
    QVERIFY(window.m_repairToolButton->isEnabled());
    QVERIFY(window.selectedRepairStages().contains(QStringLiteral("dkms")));

    window.m_hostMaintenanceMode = false;
    window.updateFullRepairSummary();
    QVERIFY(!window.m_fullRepairDkms->isEnabled());
    QVERIFY(window.m_fullRepairDkms->isChecked());
    QVERIFY(!window.m_repairToolButton->isEnabled());
    QVERIFY(!window.selectedRepairStages().contains(QStringLiteral("dkms")));
    QVERIFY(item->text(1).contains(QStringLiteral("DKMS is not installed")));

    window.m_hostMaintenanceMode = true;
    window.updateFullRepairSummary();
    QVERIFY(window.m_repairToolButton->isEnabled());
    QVERIFY(window.selectedRepairStages().contains(QStringLiteral("dkms")));

    window.m_hostMaintenanceMode = false;
    cacheRepairEvidence(window, capabilityEvidence(false, true));
    window.m_previewTargetPath = QStringLiteral("/dev/another-target");
    QString reason;
    QVERIFY(!window.repairToolAvailable(QStringLiteral("dkms"), &reason));
    QVERIFY(reason.contains(QStringLiteral("different target")));
    QVERIFY(window.selectedRepairStages().isEmpty());
    window.updateFullRepairSummary();
    QVERIFY(!window.m_fullRepairDkms->isEnabled());
    QVERIFY(!window.m_repairToolButton->isEnabled());
    QVERIFY(!window.m_runFullRepairButton->isEnabled());
}

void MainWindowUiTest::missingCapabilityEvidenceFailsClosed()
{
    MainWindow window;
    for (bool host : {false, true}) {
        prepareRepairScope(window, host);
        cacheRepairEvidence(window, QStringLiteral("PASS\nDistribution family: debian\n"));
        window.m_fullRepairDpkg->setChecked(true);
        window.updateFullRepairSummary();
        QVERIFY(!window.m_fullRepairDpkg->isEnabled());
        QVERIFY(window.selectedRepairStages().isEmpty());
        QVERIFY(!window.m_runFullRepairButton->isEnabled());
        for (int row = 0; row < window.m_repairToolTree->topLevelItemCount(); ++row) {
            auto *item = window.m_repairToolTree->topLevelItem(row);
            const QString key = item->data(0, Qt::UserRole).toString();
            QString reason;
            QVERIFY(!window.repairToolAvailable(key, &reason));
            QVERIFY(!reason.isEmpty());
            QVERIFY(!window.repairEvidenceReadyForTool(key));
            window.m_repairToolTree->setCurrentItem(item);
            window.updateRepairToolDetails();
            QVERIFY(!window.m_repairToolButton->isEnabled());
            QVERIFY(item->text(1).startsWith(QStringLiteral("Unavailable:")));
        }
        cacheRepairEvidence(window, QStringLiteral("Repair tool unknown: available\nRepair tool dpkg: maybe\n"));
        QVERIFY(!window.repairToolAvailable(QStringLiteral("unknown")));
        QVERIFY(!window.repairToolAvailable(QStringLiteral("dpkg")));
        cacheRepairEvidence(window, QStringLiteral("Repair tool dpkg: available\nRepair tool dpkg: unavailable|Conflict\n"));
        QString reason;
        QVERIFY(!window.repairToolAvailable(QStringLiteral("dpkg"), &reason));
        QCOMPARE(reason, QStringLiteral("Conflict"));
    }
}

void MainWindowUiTest::archIndividualDpkgAndAptUpdateStayDisabled()
{
    MainWindow window;
    for (bool host : {false, true}) {
        prepareRepairScope(window, host);
        cacheRepairEvidence(window, capabilityEvidence(true, false));
        window.m_fullRepairDpkg->setChecked(true);
        window.m_fullRepairAptUpdate->setChecked(true);
        window.m_fullRepairBrokenPackages->setChecked(true);
        window.updateFullRepairSummary();
        QVERIFY(!window.m_fullRepairDpkg->isEnabled());
        QVERIFY(!window.m_fullRepairAptUpdate->isEnabled());
        QVERIFY(!window.selectedRepairStages().contains(QStringLiteral("dpkg-configure")));
        QVERIFY(!window.selectedRepairStages().contains(QStringLiteral("apt-update")));
        QVERIFY(window.selectedRepairStages().contains(QStringLiteral("fix-broken")));
        QCOMPARE(window.m_fullRepairStageList->count(), window.selectedRepairStages().size());
        for (const QString &key : {QStringLiteral("dpkg"), QStringLiteral("aptupdate")}) {
            auto *item = repairItem(window, key);
            QVERIFY(item);
            window.m_repairToolTree->setCurrentItem(item);
            window.updateRepairToolDetails();
            QVERIFY(!window.m_repairToolButton->isEnabled());
            QVERIFY(window.m_repairToolButton->toolTip().contains(QStringLiteral("Arch")));
            QVERIFY(window.m_repairToolPlanStatus->text().contains(QStringLiteral("Unavailable:")));
            QVERIFY(!window.repairEvidenceReadyForTool(key));
        }
    }
}

void MainWindowUiTest::archDkmsAvailableGatesDkmsTool()
{
    MainWindow window;
    for (bool host : {false, true}) {
        prepareRepairScope(window, host);
        window.m_fullRepairDkms->setChecked(true);
        auto *item = repairItem(window, QStringLiteral("dkms"));
        QVERIFY(item);
        window.m_repairToolTree->setCurrentItem(item);
        for (bool installed : {false, true, false}) {
            cacheRepairEvidence(window, capabilityEvidence(true, installed));
            window.updateFullRepairSummary();
            QCOMPARE(window.repairToolAvailable(QStringLiteral("dkms")), installed);
            QCOMPARE(window.repairEvidenceReadyForTool(QStringLiteral("dkms")), installed);
            QCOMPARE(window.m_fullRepairDkms->isEnabled(), installed);
            QCOMPARE(window.m_repairToolButton->isEnabled(), installed);
            QCOMPARE(window.selectedRepairStages().contains(QStringLiteral("dkms")), installed);
            QVERIFY(window.m_fullRepairDkms->isChecked());
            if (!installed) {
                QVERIFY(item->text(1).contains(QStringLiteral("DKMS is not installed")));
                QVERIFY(window.m_fullRepairDkms->toolTip().contains(QStringLiteral("DKMS is not installed")));
            }
        }
    }
}

void MainWindowUiTest::debianDpkgToolAllowedAndArchRefused()
{
    MainWindow window;
    for (bool host : {false, true}) {
        prepareRepairScope(window, host);
        window.m_fullRepairDpkg->setChecked(true);
        auto *item = repairItem(window, QStringLiteral("dpkg"));
        QVERIFY(item);
        window.m_repairToolTree->setCurrentItem(item);
        for (bool arch : {false, true, false}) {
            cacheRepairEvidence(window, capabilityEvidence(arch, false));
            window.updateFullRepairSummary();
            QCOMPARE(window.repairToolAvailable(QStringLiteral("dpkg")), !arch);
            QCOMPARE(window.repairEvidenceReadyForTool(QStringLiteral("dpkg")), !arch);
            QCOMPARE(window.m_fullRepairDpkg->isEnabled(), !arch);
            QCOMPARE(window.m_repairToolButton->isEnabled(), !arch);
            QCOMPARE(window.selectedRepairStages().contains(QStringLiteral("dpkg-configure")), !arch);
        }
    }
}

// Alpine hosts must be labelled accurately (apk / OpenRC with apk package
// hints) and stay diagnostics-only: cached Alpine capability evidence enables
// validate/filesystem but fails every guarded repair closed with the
// Alpine-specific reason.
void MainWindowUiTest::alpineDetectionLabelsAndCapabilityGating()
{
    QTemporaryDir releaseDir;
    QVERIFY(releaseDir.isValid());
    const QString releasePath = releaseDir.path() + QStringLiteral("/os-release");
    {
        QFile file(releasePath);
        QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
        QTextStream stream(&file);
        stream << "ID=alpine\n"
               << "ID_LIKE=\"alpine\"\n"
               << "PRETTY_NAME=\"Alpine Linux v3.20\"\n";
    }

    {
        ScopedEnvironmentVariable alpineRelease("BOOT_REPAIR_OS_RELEASE", releasePath.toLocal8Bit());
        MainWindow window;
        QCOMPARE(window.m_distributionLabel->text(), QStringLiteral("Alpine Linux v3.20"));
        QCOMPARE(window.m_packageManagerLabel->text(), QStringLiteral("apk / OpenRC"));

        QHash<QString, QString> suggestedPackages;
        for (int row = 0; row < window.m_capabilityTable->rowCount(); ++row) {
            QTableWidgetItem *commandItem = window.m_capabilityTable->item(row, 1);
            QTableWidgetItem *packageItem = window.m_capabilityTable->item(row, 4);
            QVERIFY(commandItem);
            QVERIFY(packageItem);
            suggestedPackages.insert(commandItem->text(), packageItem->text());
        }
        QCOMPARE(suggestedPackages.value(QStringLiteral("grub-mkconfig")), QStringLiteral("grub"));
        QCOMPARE(suggestedPackages.value(QStringLiteral("mkinitcpio")), QStringLiteral("mkinitfs"));
        QCOMPARE(suggestedPackages.value(QStringLiteral("pacman")), QStringLiteral("Not available on Alpine"));
        QCOMPARE(suggestedPackages.value(QStringLiteral("dkms")), QStringLiteral("dkms"));
        QVERIFY(!suggestedPackages.values().contains(QStringLiteral("Unknown")));
    }

    // Alpine is no longer diagnostics-only: the helper reports guarded apk,
    // OpenRC display, mkinitfs initramfs and extlinux stages as available and
    // keeps dpkg, the standalone metadata refresh and EFI/UKI unavailable.
    // Every available stage must carry the Alpine backend wording.
    MainWindow window;
    for (bool host : {false, true}) {
        prepareRepairScope(window, host);
        cacheRepairEvidence(window, capabilityEvidenceAlpine(true, true, true));
        window.updateFullRepairSummary();
        QVERIFY(window.repairToolAvailable(QStringLiteral("validate")));
        QVERIFY(window.repairToolAvailable(QStringLiteral("filesystem")));

        for (const QString &key : {QStringLiteral("fixbroken"), QStringLiteral("upgrade"),
                                   QStringLiteral("dkms"), QStringLiteral("display"),
                                   QStringLiteral("initramfs"), QStringLiteral("grub"),
                                   QStringLiteral("extlinux"), QStringLiteral("bootstack")}) {
            QVERIFY2(window.repairToolAvailable(key), qPrintable(key));
        }
        for (const QString &key : {QStringLiteral("dpkg"), QStringLiteral("aptupdate")}) {
            QString reason;
            QVERIFY2(!window.repairToolAvailable(key, &reason), qPrintable(key));
            QVERIFY2(reason.contains(QStringLiteral("Alpine")), qPrintable(reason));
        }
        // The Alpine BIOS/extlinux refusal names the missing EFI boot path
        // instead of claiming the EFI backend is unimplemented.
        {
            QString reason;
            QVERIFY2(!window.repairToolAvailable(QStringLiteral("efi"), &reason), "efi");
            QVERIFY2(reason.contains(QStringLiteral("syslinux/extlinux (BIOS) boot detected")),
                     qPrintable(reason));
        }

        // dkms and grub follow their installed-state capability lines on
        // Alpine exactly like the other families.
        cacheRepairEvidence(window, capabilityEvidenceAlpine(false, false, true));
        window.updateFullRepairSummary();
        QVERIFY(!window.repairToolAvailable(QStringLiteral("dkms")));
        QVERIFY(!window.repairToolAvailable(QStringLiteral("grub")));
        QVERIFY(!window.m_fullRepairDkms->isEnabled());
        QVERIFY(!window.m_fullRepairGrub->isEnabled());
        QVERIFY(window.repairToolAvailable(QStringLiteral("extlinux")));
        cacheRepairEvidence(window, capabilityEvidenceAlpine(true, true, true));
        window.updateFullRepairSummary();
        QVERIFY(window.repairToolAvailable(QStringLiteral("dkms")));
        QVERIFY(window.repairToolAvailable(QStringLiteral("grub")));

        QCOMPARE(window.m_fullRepairBrokenPackages->text(),
                 QStringLiteral("Repair Alpine packages (apk fix)"));
        QCOMPARE(window.m_fullRepairUpgrade->text(),
                 QStringLiteral("Upgrade Alpine packages (apk upgrade)"));
        QCOMPARE(window.m_fullRepairDisplayManager->text(),
                 QStringLiteral("Restore detected graphical login manager (OpenRC runlevel)"));
        QCOMPARE(window.m_fullRepairInitramfs->text(),
                 QStringLiteral("Rebuild initramfs (mkinitfs)"));
        QCOMPARE(window.m_fullRepairExtlinux->text(),
                 QStringLiteral("Regenerate extlinux configuration"));
        QVERIFY(!window.m_fullRepairDpkg->isEnabled());
        QVERIFY(!window.m_fullRepairAptUpdate->isEnabled());
        QVERIFY(window.m_fullRepairDpkg->toolTip().contains(QStringLiteral("Alpine")));
        QVERIFY(window.m_fullRepairAptUpdate->toolTip().contains(QStringLiteral("Alpine")));

        auto *fixbrokenItem = repairItem(window, QStringLiteral("fixbroken"));
        QVERIFY(fixbrokenItem);
        QVERIFY(fixbrokenItem->text(1).contains(QStringLiteral("Enabled in Settings")));

        auto *initramfsItem = repairItem(window, QStringLiteral("initramfs"));
        QVERIFY(initramfsItem);
        window.m_repairToolTree->setCurrentItem(initramfsItem);
        window.updateRepairToolDetails();
        QVERIFY(window.m_repairToolButton->isEnabled());
        QVERIFY(window.m_repairToolDescription->text().contains(QStringLiteral("mkinitfs")));

        window.m_fullRepairFilesystem->setChecked(false);
        window.m_fullRepairDpkg->setChecked(true);
        window.m_fullRepairBrokenPackages->setChecked(true);
        window.m_fullRepairDkms->setChecked(true);
        window.updateFullRepairSummary();
        QVERIFY(!window.m_fullRepairDpkg->isEnabled());
        QVERIFY(window.m_fullRepairBrokenPackages->isEnabled());
        QVERIFY(window.m_fullRepairDkms->isEnabled());
        QVERIFY(!window.selectedRepairStages().contains(QStringLiteral("dpkg-configure")));
        QVERIFY(window.selectedRepairStages().contains(QStringLiteral("fix-broken")));
        QVERIFY(window.selectedRepairStages().contains(QStringLiteral("dkms")));
    }
}

// The extlinux key is the Alpine/BIOS sibling of GRUB: it must exist as a tool
// row ordered next to GRUB, carry the extlinux details panel, and stay gated
// on the helper's `Repair tool extlinux` capability line (fail closed).
void MainWindowUiTest::alpineExtlinuxToolRowIsGatedByCapabilityEvidence()
{
    MainWindow window;
    for (bool host : {false, true}) {
        prepareRepairScope(window, host);
        auto *extlinuxItem = repairItem(window, QStringLiteral("extlinux"));
        QVERIFY2(extlinuxItem, "the extlinux repair tool row must exist");
        QCOMPARE(extlinuxItem->text(0), QStringLiteral("extlinux configuration"));
        QVERIFY2(!extlinuxItem->icon(0).isNull(), "the extlinux row must carry the shared boot icon");
        QVERIFY(window.m_fullRepairExtlinux);

        // The row is GRUB's sibling: it follows GRUB and precedes the
        // boot-stack recovery tool.
        auto *grubItem = repairItem(window, QStringLiteral("grub"));
        auto *bootstackItem = repairItem(window, QStringLiteral("bootstack"));
        QVERIFY(grubItem && bootstackItem);
        const int grubRow = window.m_repairToolTree->indexOfTopLevelItem(grubItem);
        const int extlinuxRow = window.m_repairToolTree->indexOfTopLevelItem(extlinuxItem);
        const int bootstackRow = window.m_repairToolTree->indexOfTopLevelItem(bootstackItem);
        QCOMPARE(extlinuxRow, grubRow + 1);
        QCOMPARE(bootstackRow, extlinuxRow + 1);

        window.m_repairToolTree->setCurrentItem(extlinuxItem);
        for (bool available : {false, true, false}) {
            cacheRepairEvidence(window, capabilityEvidenceAlpine(true, true, available));
            window.updateFullRepairSummary();

            QCOMPARE(window.repairToolAvailable(QStringLiteral("extlinux")), available);
            QCOMPARE(window.repairEvidenceReadyForTool(QStringLiteral("extlinux")), available);
            QCOMPARE(window.m_fullRepairExtlinux->isEnabled(), available);
            QCOMPARE(window.m_repairToolButton->isEnabled(), available);
            QCOMPARE(window.selectedRepairStages().contains(QStringLiteral("extlinux")), available);
            QVERIFY2(window.m_fullRepairExtlinux->isChecked(),
                     "an unavailable stage must keep the user's Settings selection");

            if (available) {
                QCOMPARE(extlinuxItem->text(1), QStringLiteral("Enabled in Settings"));
                QCOMPARE(window.m_repairToolTitle->text(), QStringLiteral("extlinux configuration"));
                QCOMPARE(window.m_repairToolButton->text(), QStringLiteral("Regenerate extlinux"));
                QVERIFY(window.m_repairToolDescription->text().contains(QStringLiteral("update-extlinux")));
                QVERIFY(window.m_repairToolDescription->text().contains(QStringLiteral("entry-preserving")));
                QCOMPARE(window.m_repairToolPlanStatus->text(),
                         QStringLiteral("Full Repair plan: Enabled in Settings"));
            } else {
                QVERIFY2(extlinuxItem->text(1).contains(QStringLiteral("update-extlinux is not installed")),
                         qPrintable(extlinuxItem->text(1)));
                QVERIFY(window.m_fullRepairExtlinux->toolTip().contains(
                    QStringLiteral("update-extlinux is not installed")));
                QVERIFY(window.m_repairToolPlanStatus->text().contains(QStringLiteral("Unavailable:")));
            }
        }
    }
}

// Enabling the extlinux stage must add it to the Full Repair execution plan
// directly after GRUB, and an unavailable capability line must remove it again
// without unchecking the user's Settings selection.
void MainWindowUiTest::alpineFullRepairPlanIncludesExtlinuxStage()
{
    MainWindow window;
    prepareRepairScope(window);
    window.m_snapshotPreloadScheduled = true;
    for (QCheckBox *toggle : {window.m_fullRepairFilesystem, window.m_fullRepairDpkg,
                              window.m_fullRepairBrokenPackages, window.m_fullRepairAptUpdate,
                              window.m_fullRepairUpgrade, window.m_fullRepairDkms,
                              window.m_fullRepairDisplayManager, window.m_fullRepairInitramfs,
                              window.m_fullRepairEfi, window.m_fullRepairGrub,
                              window.m_fullRepairExtlinux}) {
        QVERIFY(toggle);
        toggle->setChecked(false);
    }
    window.m_fullRepairGrub->setChecked(true);
    window.m_fullRepairExtlinux->setChecked(true);

    cacheRepairEvidence(window, capabilityEvidenceAlpine(false, true, true));
    window.updateFullRepairSummary();
    QCOMPARE(window.selectedRepairStages(),
             QStringList({QStringLiteral("grub"), QStringLiteral("extlinux")}));
    QCOMPARE(window.m_fullRepairStageList->count(), 2);
    QVERIFY(window.m_fullRepairStageList->item(0)->text().contains(QStringLiteral("GRUB")));
    QVERIFY2(window.m_fullRepairStageList->item(1)->text().contains(QStringLiteral("extlinux configuration")),
             qPrintable(window.m_fullRepairStageList->item(1)->text()));
    QVERIFY2(window.m_runFullRepairButton->isEnabled(),
             "the extlinux stage must be runnable with cached Alpine evidence");

    // An unavailable extlinux capability fails closed: the stage leaves the
    // plan while the Settings selection is preserved.
    cacheRepairEvidence(window, capabilityEvidenceAlpine(false, true, false));
    window.updateFullRepairSummary();
    QCOMPARE(window.selectedRepairStages(), QStringList{QStringLiteral("grub")});
    QCOMPARE(window.m_fullRepairStageList->count(), 1);
    QVERIFY(!window.m_fullRepairStageList->item(0)->text().contains(QStringLiteral("extlinux")));
    QVERIFY(window.m_fullRepairExtlinux->isChecked());
    QVERIFY(!window.m_fullRepairExtlinux->isEnabled());
}

// Alpine UEFI GRUB targets must offer the EFI and GRUB stages from the cached
// capability lines, label them with the Alpine GRUB EFI wording, describe the
// guarded --no-nvram / EFI-stub behavior and include both stages in the Full
// Repair plan; an unavailable EFI line fails closed.
void MainWindowUiTest::alpineUefiEfiGrubRepairIsOfferedAndLabelled()
{
    MainWindow window;
    for (bool host : {false, true}) {
        prepareRepairScope(window, host);
        cacheRepairEvidence(window, capabilityEvidenceAlpineUefi(true, true));
        window.updateFullRepairSummary();

        QVERIFY(window.repairToolAvailable(QStringLiteral("efi")));
        QVERIFY(window.repairToolAvailable(QStringLiteral("grub")));
        QVERIFY(window.m_fullRepairEfi->isEnabled());
        QVERIFY(window.m_fullRepairGrub->isEnabled());
        QCOMPARE(window.m_fullRepairEfi->text(), QStringLiteral("Reinstall Alpine GRUB EFI loader"));
        QCOMPARE(window.m_fullRepairGrub->text(), QStringLiteral("Regenerate Alpine GRUB configuration"));

        auto *efiItem = repairItem(window, QStringLiteral("efi"));
        QVERIFY(efiItem);
        window.m_repairToolTree->setCurrentItem(efiItem);
        window.updateRepairToolDetails();
        QVERIFY(window.m_repairToolDescription->text().contains(QStringLiteral("--no-nvram")));
        QVERIFY(window.m_repairToolDescription->text().contains(QStringLiteral("Alpine")));
        QVERIFY(window.m_repairToolDescription->text().contains(QStringLiteral("EFI-stub")));

        auto *grubItem = repairItem(window, QStringLiteral("grub"));
        QVERIFY(grubItem);
        window.m_repairToolTree->setCurrentItem(grubItem);
        window.updateRepairToolDetails();
        QVERIFY(window.m_repairToolDescription->text().contains(QStringLiteral("Alpine")));
        QVERIFY(window.m_repairToolDescription->text().contains(QStringLiteral("grub-mkconfig")));

        // The plan names the Alpine GRUB EFI stages and keeps EFI before GRUB.
        window.m_fullRepairEfi->setChecked(true);
        window.m_fullRepairGrub->setChecked(true);
        window.updateFullRepairSummary();
        const QStringList stages = window.selectedRepairStages();
        QVERIFY2(stages.contains(QStringLiteral("efi")), qPrintable(stages.join(QLatin1Char(','))));
        QVERIFY2(stages.contains(QStringLiteral("grub")), qPrintable(stages.join(QLatin1Char(','))));
        bool sawEfi = false;
        bool sawGrub = false;
        for (int row = 0; row < window.m_fullRepairStageList->count(); ++row) {
            const QString text = window.m_fullRepairStageList->item(row)->text();
            if (text.contains(QStringLiteral("Reinstall Alpine GRUB EFI loader"))) {
                sawEfi = true;
                QVERIFY2(!sawGrub, "the EFI stage must precede GRUB in the plan");
            }
            if (text.contains(QStringLiteral("Regenerate Alpine GRUB configuration"))) {
                sawGrub = true;
            }
        }
        QVERIFY2(sawEfi, "the EFI stage must be in the Full Repair plan");
        QVERIFY2(sawGrub, "the GRUB stage must be in the Full Repair plan");

        // An unavailable EFI capability removes the stage from the plan while
        // preserving the Settings selection, exactly like every other key.
        cacheRepairEvidence(window, capabilityEvidenceAlpineUefi(false, true));
        window.updateFullRepairSummary();
        QVERIFY(!window.repairToolAvailable(QStringLiteral("efi")));
        QVERIFY(!window.m_fullRepairEfi->isEnabled());
        QVERIFY(window.m_fullRepairEfi->isChecked());
        QVERIFY(!window.selectedRepairStages().contains(QStringLiteral("efi")));
        QVERIFY(window.m_fullRepairGrub->isEnabled());
        QVERIFY(window.selectedRepairStages().contains(QStringLiteral("grub")));
        QCOMPARE(window.m_fullRepairGrub->text(), QStringLiteral("Regenerate Alpine GRUB configuration"));
    }
}

// Fedora/RPM targets must be labelled and gated from the detected backends
// (rpm/dnf5, dracut, GRUB2, GDM) exactly like every other family: labels come
// from the backend profile lines, availability comes only from the
// `Repair tool <key>` capability lines, and the plan stages keep the
// upgrade(40) -> initramfs(70) -> grub(90) order.
void MainWindowUiTest::fedoraDetectionLabelsAndCapabilityGating()
{
    MainWindow window;
    // The Fedora GDM configuration path is offered next to the Debian GDM3
    // path through the guarded target configuration action.
    bool sawGdmConfig = false;
    for (int index = 0; index < window.m_targetConfigCombo->count(); ++index) {
        if (window.m_targetConfigCombo->itemText(index) == QStringLiteral("/etc/gdm/custom.conf")) {
            sawGdmConfig = true;
            QCOMPARE(window.m_targetConfigCombo->itemData(index).toString(), QStringLiteral("gdm"));
        }
    }
    QVERIFY2(sawGdmConfig, "the Fedora GDM configuration path must be offered");
    for (bool host : {false, true}) {
        prepareRepairScope(window, host);
        cacheRepairEvidence(window, capabilityEvidenceFedora(true, true, true, true));
        window.updateFullRepairSummary();

        // Every Fedora backend stage is available from its own capability
        // line; dpkg, the BIOS EFI path and extlinux stay unavailable with
        // their probe reasons.
        for (const QString &key : {QStringLiteral("validate"), QStringLiteral("filesystem"),
                                   QStringLiteral("fixbroken"), QStringLiteral("aptupdate"),
                                   QStringLiteral("upgrade"), QStringLiteral("display"),
                                   QStringLiteral("initramfs"), QStringLiteral("grub"),
                                   QStringLiteral("bootstack")}) {
            QVERIFY2(window.repairToolAvailable(key), qPrintable(key));
        }
        for (const QString &key : {QStringLiteral("dpkg"), QStringLiteral("efi"),
                                   QStringLiteral("extlinux")}) {
            QString reason;
            QVERIFY2(!window.repairToolAvailable(key, &reason), qPrintable(key));
            QVERIFY2(!reason.isEmpty(), qPrintable(key));
        }
        {
            QString reason;
            QVERIFY(!window.repairToolAvailable(QStringLiteral("efi"), &reason));
            QVERIFY2(reason.contains(QStringLiteral("BIOS")), qPrintable(reason));
        }

        QCOMPARE(window.m_fullRepairBrokenPackages->text(),
                 QStringLiteral("Repair Fedora packages (dnf)"));
        QCOMPARE(window.m_fullRepairAptUpdate->text(),
                 QStringLiteral("Refresh package metadata (dnf makecache)"));
        QCOMPARE(window.m_fullRepairUpgrade->text(),
                 QStringLiteral("Upgrade Fedora packages (dnf upgrade)"));
        QCOMPARE(window.m_fullRepairInitramfs->text(),
                 QStringLiteral("Rebuild initramfs (dracut)"));
        QCOMPARE(window.m_fullRepairGrub->text(),
                 QStringLiteral("Update GRUB2 configuration"));
        QCOMPARE(window.m_fullRepairEfi->text(),
                 QStringLiteral("Reinstall GRUB2 bootloader"));
        QCOMPARE(window.m_fullRepairDisplayManager->text(),
                 QStringLiteral("Restore detected graphical login manager (GDM)"));

        // The plan names the Fedora backends and keeps the fixed stage order.
        for (QCheckBox *toggle : {window.m_fullRepairFilesystem, window.m_fullRepairDpkg,
                                  window.m_fullRepairBrokenPackages, window.m_fullRepairAptUpdate,
                                  window.m_fullRepairUpgrade, window.m_fullRepairDkms,
                                  window.m_fullRepairDisplayManager, window.m_fullRepairInitramfs,
                                  window.m_fullRepairEfi, window.m_fullRepairGrub,
                                  window.m_fullRepairExtlinux}) {
            QVERIFY(toggle);
            toggle->setChecked(false);
        }
        window.m_fullRepairUpgrade->setChecked(true);
        window.m_fullRepairInitramfs->setChecked(true);
        window.m_fullRepairGrub->setChecked(true);
        window.updateFullRepairSummary();
        QCOMPARE(window.selectedRepairStages(),
                 QStringList({QStringLiteral("apt-upgrade"), QStringLiteral("initramfs"),
                              QStringLiteral("grub")}));
        QCOMPARE(window.m_fullRepairStageList->count(), 3);
        QVERIFY(window.m_fullRepairStageList->item(0)->text().contains(
            QStringLiteral("Upgrade installed packages")));
        QVERIFY2(window.m_fullRepairStageList->item(1)->text().contains(
                     QStringLiteral("Rebuild initramfs (dracut)")),
                 qPrintable(window.m_fullRepairStageList->item(1)->text()));
        QVERIFY2(window.m_fullRepairStageList->item(2)->text().contains(
                     QStringLiteral("Update GRUB2 configuration")),
                 qPrintable(window.m_fullRepairStageList->item(2)->text()));
        QVERIFY2(window.m_runFullRepairButton->isEnabled(),
                 "the Fedora stages must be runnable with cached evidence");

        // Tool details follow the same detected backends.
        const QList<QPair<QString, QString>> descriptions = {
            {QStringLiteral("fixbroken"), QStringLiteral("dnf reinstall")},
            {QStringLiteral("aptupdate"), QStringLiteral("dnf makecache")},
            {QStringLiteral("upgrade"), QStringLiteral("dnf upgrade")},
            {QStringLiteral("initramfs"), QStringLiteral("dracut")},
            {QStringLiteral("initramfs"), QStringLiteral("lsinitrd")},
            {QStringLiteral("bootstack"), QStringLiteral("dracut")},
            {QStringLiteral("bootstack"), QStringLiteral("GRUB2")}
        };
        for (const auto &check : descriptions) {
            auto *item = repairItem(window, check.first);
            QVERIFY2(item, qPrintable(check.first));
            window.m_repairToolTree->setCurrentItem(item);
            window.updateRepairToolDetails();
            QVERIFY2(window.m_repairToolDescription->text().contains(check.second),
                     qPrintable(check.first + QLatin1Char(':') + check.second));
        }
        {
            auto *item = repairItem(window, QStringLiteral("grub"));
            QVERIFY(item);
            window.m_repairToolTree->setCurrentItem(item);
            window.updateRepairToolDetails();
            QCOMPARE(window.m_repairToolTitle->text(), QStringLiteral("GRUB2 configuration"));
            QCOMPARE(window.m_repairToolButton->text(), QStringLiteral("Regenerate GRUB2"));
            QVERIFY(window.m_repairToolDescription->text().contains(
                QStringLiteral("grub2-mkconfig")));
            QVERIFY(window.m_repairToolDescription->text().contains(
                QStringLiteral("/boot/grub2/grub.cfg")));
            QVERIFY(window.m_repairToolDescription->text().contains(
                QStringLiteral("Reinstall GRUB2 bootloader")));
            QVERIFY(window.m_repairToolDescription->text().contains(
                QStringLiteral("bios_grub")));
        }
        {
            auto *item = repairItem(window, QStringLiteral("display"));
            QVERIFY(item);
            window.m_repairToolTree->setCurrentItem(item);
            window.updateRepairToolDetails();
            QVERIFY(window.m_repairToolDescription->text().contains(QStringLiteral("GDM")));
            QVERIFY(window.m_repairToolDescription->text().contains(
                QStringLiteral("/etc/gdm/custom.conf")));
        }

        // Unavailable capability lines fail closed while the backend labels
        // stay derived from the profile evidence, not from availability.
        cacheRepairEvidence(window, capabilityEvidenceFedora(false, false, false, false));
        window.updateFullRepairSummary();
        for (const QString &key : {QStringLiteral("fixbroken"), QStringLiteral("aptupdate"),
                                   QStringLiteral("upgrade"), QStringLiteral("display"),
                                   QStringLiteral("initramfs"), QStringLiteral("grub"),
                                   QStringLiteral("bootstack")}) {
            QVERIFY2(!window.repairToolAvailable(key), qPrintable(key));
        }
        QVERIFY(!window.m_fullRepairBrokenPackages->isEnabled());
        QVERIFY(!window.m_fullRepairInitramfs->isEnabled());
        QVERIFY(!window.m_fullRepairGrub->isEnabled());
        QVERIFY2(window.m_fullRepairInitramfs->isChecked(),
                 "an unavailable stage must keep the user's Settings selection");
        QVERIFY2(window.m_fullRepairGrub->isChecked(),
                 "an unavailable stage must keep the user's Settings selection");
        QVERIFY(window.selectedRepairStages().isEmpty());
        QCOMPARE(window.m_fullRepairBrokenPackages->text(),
                 QStringLiteral("Repair Fedora packages (dnf)"));
        QCOMPARE(window.m_fullRepairInitramfs->text(),
                 QStringLiteral("Rebuild initramfs (dracut)"));
        QCOMPARE(window.m_fullRepairGrub->text(),
                 QStringLiteral("Update GRUB2 configuration"));
    }
}

// The Fedora upgrade stage must appear in the plan and run when its capability
// line is available and the setting is on, including when the evidence arrived
// through an individual diagnostic rather than Run All. The stage is off by
// default, so the Repair page must say how to include it.
void MainWindowUiTest::fedoraUpgradeStageIncludedWhenEnabledAndGateRefreshesImmediately()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);
    prepareRepairScope(window);

    // Fresh settings: upgrade is unchecked and gated off until capability
    // evidence exists.
    QVERIFY(!window.m_fullRepairUpgrade->isChecked());
    QVERIFY(!window.m_fullRepairUpgrade->isEnabled());

    // Fedora capability evidence (upgrade available) arrives through one
    // individual diagnostic; the repair gates must refresh immediately.
    window.m_uiTestDiagnosticResults.insert(QStringLiteral("environment"), QStringLiteral(
        "Distribution family: fedora\n"
        "Package manager backend: rpm\n"
        "Package manager backends: rpm\n"
        "Service manager: systemd\n"
        "Display manager backend: systemd\n"
        "Initramfs backend: dracut\n"
        "Bootloader backend: grub\n"
        "Repair capability probes (read-only, selected target):\n"
        "Repair tool validate: available\n"
        "Repair tool filesystem: available\n"
        "Repair tool dpkg: unavailable|Fedora uses rpm/dnf; dpkg configuration is not available\n"
        "Repair tool fixbroken: available\n"
        "Repair tool aptupdate: available\n"
        "Repair tool upgrade: available\n"
        "Repair tool dkms: unavailable|the rpm DKMS header correction is not implemented\n"
        "Repair tool display: available\n"
        "Repair tool initramfs: available\n"
        "Repair tool efi: unavailable|legacy BIOS target; no EFI boot path is available\n"
        "Repair tool grub: available\n"
        "Repair tool extlinux: unavailable|syslinux/extlinux is not installed in the target system\n"
        "Repair tool bootstack: available\n"
        "Repair capability evidence (read-only):\n"
        "Repair capability evidence upgrade: dnf5 present\n\n"
        "Fedora profile: dnf/dracut/GRUB2.\n"));
    int environmentRow = -1;
    for (int row = 0; row < window.m_diagnosticList->count(); ++row) {
        if (window.m_diagnosticList->item(row)->data(Qt::UserRole).toString() == QStringLiteral("environment")) {
            environmentRow = row;
            break;
        }
    }
    QVERIFY(environmentRow >= 0);
    window.m_diagnosticList->setCurrentRow(environmentRow);
    window.runSelectedDiagnostic();

    // No second Run All and no summary refresh was needed: the individual
    // diagnostic refreshed the capability gates itself.
    QVERIFY2(window.repairToolAvailable(QStringLiteral("upgrade")),
             "the Fedora upgrade capability must gate open from the individual diagnostic");
    QVERIFY2(window.m_fullRepairUpgrade->isEnabled(),
             "the Settings upgrade stage must be enabled without another Run All");
    QCOMPARE(window.m_fullRepairUpgrade->text(),
             QStringLiteral("Upgrade Fedora packages (dnf upgrade)"));
    // Available but unchecked: the Repair page says how to include it instead
    // of only "Disabled in Settings".
    auto *upgradeItem = repairItem(window, QStringLiteral("upgrade"));
    QVERIFY(upgradeItem);
    QVERIFY2(upgradeItem->text(1).contains(QStringLiteral("enable it to include")),
             qPrintable(upgradeItem->text(1)));

    // Setting on: the stage is in the plan and runnable. Every other stage is
    // unchecked so the plan contains exactly the upgrade stage.
    for (QCheckBox *toggle : {window.m_fullRepairFilesystem, window.m_fullRepairDpkg,
                              window.m_fullRepairBrokenPackages, window.m_fullRepairAptUpdate,
                              window.m_fullRepairUpgrade, window.m_fullRepairDkms,
                              window.m_fullRepairDisplayManager, window.m_fullRepairInitramfs,
                              window.m_fullRepairEfi, window.m_fullRepairGrub,
                              window.m_fullRepairExtlinux}) {
        toggle->setChecked(false);
    }
    window.m_fullRepairUpgrade->setChecked(true);
    window.updateFullRepairSummary();
    QCOMPARE(window.selectedRepairStages(), QStringList({QStringLiteral("apt-upgrade")}));
    QCOMPARE(window.m_fullRepairStageList->count(), 1);
    QVERIFY2(window.m_fullRepairStageList->item(0)->text().contains(
                 QStringLiteral("Upgrade installed packages")),
             qPrintable(window.m_fullRepairStageList->item(0)->text()));
    QVERIFY(window.m_runFullRepairButton->isEnabled());
    QVERIFY2(upgradeItem->text(1).contains(QStringLiteral("Enabled in Settings")),
             qPrintable(upgradeItem->text(1)));

    // A stale widget enablement can never drop a checked, capability-available
    // stage from the executable plan.
    window.m_fullRepairUpgrade->setEnabled(false);
    QCOMPARE(window.selectedRepairStages(), QStringList({QStringLiteral("apt-upgrade")}));
    window.updateFullRepairSummary();
    QVERIFY(window.m_fullRepairUpgrade->isEnabled());
}

// The application must follow the desktop color scheme in both directions:
// the palette, log text/selection, table alternate rows, disabled states and
// the custom repair-result colors all adapt, and switching the scheme at
// runtime goes through colorSchemeChanged.
void MainWindowUiTest::themeFollowsDesktopColorScheme()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    ScopedColorSchemeOverride scheme(Qt::ColorScheme::Dark);
    // Emitting the platform signal is exactly what happens when the desktop
    // switches appearance at runtime.
    QGuiApplication::styleHints()->colorSchemeChanged(Qt::ColorScheme::Dark);
    QCoreApplication::processEvents();

    const QPalette darkPalette = window.palette();
    QVERIFY2(darkPalette.color(QPalette::Window).lightness() < 128,
             "a dark scheme must produce a dark window color");
    QVERIFY(darkPalette.color(QPalette::Base).lightness() < 128);
    QVERIFY(darkPalette.color(QPalette::AlternateBase).lightness() < 128);
    QVERIFY(darkPalette.color(QPalette::Button).lightness() < 128);
    QVERIFY(darkPalette.color(QPalette::Text).lightness() > 128);
    QVERIFY(darkPalette.color(QPalette::WindowText).lightness() > 128);
    QVERIFY(darkPalette.color(QPalette::ButtonText).lightness() > 128);
    QVERIFY(darkPalette.color(QPalette::HighlightedText).lightness() > 128);
    QVERIFY2(darkPalette.color(QPalette::Disabled, QPalette::Text)
                 != darkPalette.color(QPalette::Text),
             "disabled states must keep their own readable text color");
    QVERIFY2(window.m_logView->palette().color(QPalette::Base).lightness() < 128,
             "the log view must use the dark base color");
    QVERIFY2(window.m_logView->palette().color(QPalette::Text).lightness() > 128,
             "the log text must stay readable on dark");
    QVERIFY(window.m_capabilityTable->alternatingRowColors());
    QVERIFY2(window.m_capabilityTable->palette().color(QPalette::AlternateBase).lightness() < 128,
             "table alternate rows must use the dark alternate color");

    window.appendLog(QStringLiteral("Repair result: ✓ dark scheme"), QStringLiteral("INFO"),
                     MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral("Repair result: ✗ dark scheme"), QStringLiteral("ERROR"),
                     MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral("Repair result: ▪ dark scheme"), QStringLiteral("INFO"),
                     MainWindow::LogEntryKind::Repair);
    flushLogRefresh(window);
    const QColor darkSuccess = repairGlyphColor(window, QStringLiteral("✓"));
    const QColor darkFailed = repairGlyphColor(window, QStringLiteral("✗"));
    const QColor darkNoRepair = repairGlyphColor(window, QStringLiteral("▪"));
    QVERIFY(darkSuccess.isValid() && darkFailed.isValid() && darkNoRepair.isValid());
    QVERIFY2(darkSuccess.lightness() > 128, "the success glyph must be a light green on dark");
    QVERIFY2(darkFailed.lightness() > 128, "the failure glyph must be a light red on dark");
    QVERIFY2(darkNoRepair.lightness() > 128, "the no-repair glyph must be a light blue on dark");
    QVERIFY2(window.m_actionLogEntries.join(QLatin1Char('\n')).contains(
                 QStringLiteral("Desktop color scheme: dark (source: test override")),
             "the applied scheme and its source must be logged");

    // Switch the desktop scheme at runtime: the palette and the custom colors
    // must follow through the same signal.
    MainWindow::setColorSchemeOverrideForTests(Qt::ColorScheme::Light);
    QGuiApplication::styleHints()->colorSchemeChanged(Qt::ColorScheme::Light);
    QCoreApplication::processEvents();

    const QPalette lightPalette = window.palette();
    QVERIFY2(lightPalette.color(QPalette::Window).lightness() > 128,
             "a light scheme must produce a light window color");
    QVERIFY(lightPalette.color(QPalette::Base).lightness() > 128);
    QVERIFY(lightPalette.color(QPalette::AlternateBase).lightness() > 128);
    QVERIFY(lightPalette.color(QPalette::Text).lightness() < 128);
    QVERIFY(lightPalette.color(QPalette::HighlightedText).lightness() > 128);
    QVERIFY(lightPalette.color(QPalette::Disabled, QPalette::Text)
                != lightPalette.color(QPalette::Text));
    QVERIFY(window.m_logView->palette().color(QPalette::Base).lightness() > 128);
    QVERIFY(window.m_capabilityTable->palette().color(QPalette::AlternateBase).lightness() > 128);

    flushLogRefresh(window);
    const QColor lightSuccess = repairGlyphColor(window, QStringLiteral("✓"));
    const QColor lightFailed = repairGlyphColor(window, QStringLiteral("✗"));
    const QColor lightNoRepair = repairGlyphColor(window, QStringLiteral("▪"));
    QVERIFY2(lightSuccess.lightness() < 128, "the success glyph must be a dark green on light");
    QVERIFY2(lightFailed.lightness() < 128, "the failure glyph must be a dark red on light");
    QVERIFY2(lightNoRepair.lightness() < 128, "the no-repair glyph must be a dark blue on light");
    QVERIFY2(window.m_actionLogEntries.join(QLatin1Char('\n')).contains(
                 QStringLiteral("Desktop color scheme: light (source: test override")),
             "a runtime switch must be logged with its source");
    QVERIFY(darkSuccess != lightSuccess);
    QVERIFY(darkFailed != lightFailed);
    QVERIFY(darkNoRepair != lightNoRepair);
    QVERIFY(darkSuccess.lightness() > lightSuccess.lightness());
    QVERIFY(darkFailed.lightness() > lightFailed.lightness());
    QVERIFY(darkNoRepair.lightness() > lightNoRepair.lightness());
}

// Reproduces the Fedora/GNOME startup race: the desktop was already dark before
// the launch, the platform theme publishes the scheme only after the first
// paint, and no colorSchemeChanged is emitted because nothing changed. The
// deferred re-check must pick the late value up without any user toggle.
void MainWindowUiTest::themeDetectsDarkSchemeAfterStartup()
{
    // Deterministic initial state: the window starts light and the desktop
    // value only becomes visible after the event loop has started.
    ScopedColorSchemeOverride scheme(Qt::ColorScheme::Light);
    MainWindow window;
    window.show();
    QVERIFY2(window.palette().color(QPalette::Window).lightness() > 128,
             "the window must start light before the desktop scheme resolves");
    QVERIFY2(window.m_logView->palette().color(QPalette::Base).lightness() > 128,
             "the log view must start light before the desktop scheme resolves");

    window.appendLog(QStringLiteral("Repair result: ✓ startup scheme"), QStringLiteral("INFO"),
                     MainWindow::LogEntryKind::Repair);
    flushLogRefresh(window);
    const QColor lightSuccess = repairGlyphColor(window, QStringLiteral("✓"));
    QVERIFY(lightSuccess.isValid() && lightSuccess.lightness() < 128);

    // The scheme becomes dark shortly after startup, without any
    // colorSchemeChanged emission (the desktop was already dark).
    MainWindow::setColorSchemeOverrideForTests(Qt::ColorScheme::Dark);
    QTRY_VERIFY_WITH_TIMEOUT(
        window.palette().color(QPalette::Window).lightness() < 128, 3000);

    QVERIFY(window.palette().color(QPalette::Base).lightness() < 128);
    QVERIFY(window.palette().color(QPalette::AlternateBase).lightness() < 128);
    QVERIFY(window.palette().color(QPalette::Text).lightness() > 128);
    QVERIFY(window.palette().color(QPalette::WindowText).lightness() > 128);
    QVERIFY2(window.m_logView->palette().color(QPalette::Base).lightness() < 128,
             "the log view must adopt the late dark scheme");
    QVERIFY2(window.m_capabilityTable->palette().color(QPalette::AlternateBase).lightness() < 128,
             "table alternate rows must adopt the late dark scheme");

    // The deferred re-apply must also re-render palette-derived custom colors,
    // not just switch the palette.
    const QColor darkSuccess = repairGlyphColor(window, QStringLiteral("✓"));
    QVERIFY(darkSuccess.isValid());
    QVERIFY2(darkSuccess.lightness() > 128,
             "the success glyph must be re-rendered light on the late dark scheme");
    QVERIFY(darkSuccess != lightSuccess);
}

// The portal fallback must understand GNOME's Settings portal values: the
// GNOME namespace returns the strings "prefer-dark"/"prefer-light", the
// cross-desktop appearance key returns 0/1/2, and GNOME wraps the payload in
// an extra variant layer that defeated the old single-unwrap parser.
void MainWindowUiTest::portalColorSchemeParsing()
{
    QVERIFY2(MainWindow::colorSchemeFromPortalValue(QVariant(QStringLiteral("prefer-dark")))
                 == Qt::ColorScheme::Dark,
             "GNOME's prefer-dark string must resolve to dark");
    QVERIFY(MainWindow::colorSchemeFromPortalValue(QVariant(QStringLiteral("dark")))
            == Qt::ColorScheme::Dark);
    QVERIFY(MainWindow::colorSchemeFromPortalValue(QVariant(QStringLiteral("prefer-light")))
            == Qt::ColorScheme::Light);
    QVERIFY(MainWindow::colorSchemeFromPortalValue(QVariant(QStringLiteral("default")))
            == Qt::ColorScheme::Unknown);
    QVERIFY(MainWindow::colorSchemeFromPortalValue(QVariant(uint(1))) == Qt::ColorScheme::Dark);
    QVERIFY(MainWindow::colorSchemeFromPortalValue(QVariant(uint(2))) == Qt::ColorScheme::Light);
    QVERIFY(MainWindow::colorSchemeFromPortalValue(QVariant(uint(0))) == Qt::ColorScheme::Unknown);
    QVERIFY(MainWindow::colorSchemeFromPortalValue(QVariant(QStringLiteral("")))
            == Qt::ColorScheme::Unknown);
#ifdef BOOT_REPAIR_HAVE_DBUS
    // Exactly what `gdbus call ... Read org.gnome.desktop.interface
    // color-scheme` returns on GNOME 50: a variant inside a variant.
    const QDBusVariant darkInner(QStringLiteral("prefer-dark"));
    QVERIFY(MainWindow::colorSchemeFromPortalValue(QVariant::fromValue(QDBusVariant(darkInner)))
            == Qt::ColorScheme::Dark);
    const QDBusVariant lightInner(QStringLiteral("prefer-light"));
    QVERIFY(MainWindow::colorSchemeFromPortalValue(QVariant::fromValue(QDBusVariant(lightInner)))
            == Qt::ColorScheme::Light);
#endif
}

// The Diagnostics list gains a dedicated "File systems" entry backed by the
// read-only fs-inspect helper command, and an individual diagnostic never
// shows the shared capability preamble inside its own result: fstab stays
// purely fstab while the gating lines remain cached for repairToolAvailable().
void MainWindowUiTest::filesystemDiagnosticUsesFsInspectAndKeepsFstabPure()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);
    prepareRepairScope(window);

    int filesystemRow = -1;
    int usageRow = -1;
    int fstabRow = -1;
    for (int row = 0; row < window.m_diagnosticList->count(); ++row) {
        const QString key = window.m_diagnosticList->item(row)->data(Qt::UserRole).toString();
        if (key == QStringLiteral("filesystem")) filesystemRow = row;
        if (key == QStringLiteral("usage")) usageRow = row;
        if (key == QStringLiteral("fstab")) fstabRow = row;
    }
    QVERIFY2(filesystemRow >= 0, "the Diagnostics list must offer a dedicated File systems entry");
    QVERIFY(usageRow >= 0);
    QVERIFY(fstabRow >= 0);
    QCOMPARE(window.m_diagnosticList->item(filesystemRow)->text(), QStringLiteral("File systems"));
    QVERIFY2(usageRow < filesystemRow && filesystemRow < fstabRow,
             "File systems belongs with the storage diagnostics, before fstab");

    // Running it issues the existing fs-inspect helper command (not diagnose).
    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("fs-diagnostic.log"));
    QVERIFY(startFilesystemFakePrivilegedSession(window, capturePath));
    window.m_diagnosticList->setCurrentRow(filesystemRow);
    window.runSelectedDiagnostic();

    const QList<QStringList> requests = capturedHelperRequests(capturePath);
    QVERIFY2(!requests.isEmpty(), "the read-only fs-inspect request must be recorded");
    QVERIFY2(requests.first().contains(QStringLiteral("fs-inspect")),
             qPrintable(requests.first().join(QLatin1Char(' '))));
    QVERIFY2(!requests.first().contains(QStringLiteral("diagnose")),
             "the File systems diagnostic must use fs-inspect, not diagnose");
    const QString cachedFilesystem = window.m_targetDiagnosticCache.value(QStringLiteral("filesystem"));
    QVERIFY2(cachedFilesystem.contains(QStringLiteral("File system check")),
             qPrintable(cachedFilesystem));
    QVERIFY2(!cachedFilesystem.contains(QStringLiteral("Repair capability probes")),
             "the capability preamble must not leak into the File systems result");
    QVERIFY(!window.m_diagnosticResults->toPlainText().contains(QStringLiteral("Repair capability probes")));

    // An individual diagnostic that carries the helper's capability preamble
    // keeps the result pure and the gating lines cached separately.
    window.m_uiTestDiagnosticResults.insert(QStringLiteral("fstab"), QStringLiteral(
        "========================================\n"
        "FSTAB\n"
        "========================================\n"
        "Diagnostic: fstab\n"
        "Scope: Repair Target\n"
        "Time: 2026-09-20T00:00:00\n\n"
        "Repair capability probes (read-only, selected target):\n"
        "Repair tool validate: available\n"
        "Repair tool upgrade: available\n"
        "Repair capability evidence (read-only):\n"
        "Repair capability evidence upgrade: dnf5 present\n\n"
        "UUID=11111111-2222-3333-4444-555555555555 / ext4 defaults 0 1\n"));
    window.m_diagnosticList->setCurrentRow(fstabRow);
    window.runSelectedDiagnostic();

    const QString cachedFstab = window.m_targetDiagnosticCache.value(QStringLiteral("fstab"));
    QVERIFY2(cachedFstab.contains(QStringLiteral("UUID=11111111-2222-3333-4444-555555555555 / ext4")),
             qPrintable(cachedFstab));
    QVERIFY2(!cachedFstab.contains(QStringLiteral("Repair tool")), qPrintable(cachedFstab));
    QVERIFY2(!cachedFstab.contains(QStringLiteral("Repair capability")), qPrintable(cachedFstab));
    QVERIFY2(!window.m_diagnosticResults->toPlainText().contains(QStringLiteral("Repair tool upgrade")),
             "the fstab result pane must stay purely fstab");
    const QString capabilities = window.m_targetDiagnosticCache.value(QStringLiteral("capabilities"));
    QVERIFY2(capabilities.contains(QStringLiteral("Repair tool upgrade: available")),
             qPrintable(capabilities));
    QVERIFY2(window.repairToolAvailable(QStringLiteral("upgrade")),
             "the separated capability evidence must keep gating repairToolAvailable()");

    // Re-running the preamble-less File systems diagnostic must not drop the
    // preserved capability evidence and disable every repair gate.
    window.m_diagnosticList->setCurrentRow(filesystemRow);
    window.runSelectedDiagnostic();
    QVERIFY2(window.repairToolAvailable(QStringLiteral("upgrade")),
             "a preamble-less diagnostic must preserve the cached capability evidence");

    // The same generic File systems entry is present in the Running Host scope
    // and dispatches host-fs-inspect (never the target fs-inspect command).
    // The list itself is backend-family independent, so the entry is offered
    // for Debian/Arch/Alpine/Fedora alike.
    prepareRepairScope(window, true);
    enterHostDiagnosticScope(window);
    QCOMPARE(window.m_diagnosticScopeLabel->text(),
             QStringLiteral("Host maintenance: /dev/test-host"));
    QVERIFY2(filesystemRow >= 0, "the File systems entry must exist in the Running Host scope");
    QCOMPARE(window.m_diagnosticList->item(filesystemRow)->text(), QStringLiteral("File systems"));
    window.m_diagnosticList->setCurrentRow(filesystemRow);
    window.runSelectedDiagnostic();

    const QList<QStringList> hostRequests = capturedHelperRequests(capturePath);
    QVERIFY2(hostRequests.size() > requests.size(),
             "the host-scope File systems run must issue a helper request");
    const QStringList hostRequest = hostRequests.last();
    QVERIFY2(hostRequest.contains(QStringLiteral("host-fs-inspect")),
             qPrintable(hostRequest.join(QLatin1Char(' '))));
    QVERIFY2(!hostRequest.contains(QStringLiteral("fs-inspect")),
             "host scope must not dispatch the target fs-inspect command");
    QVERIFY2(!hostRequest.contains(QStringLiteral("diagnose")),
             "the File systems diagnostic must use host-fs-inspect, not diagnose");
    const QString cachedHostFilesystem = window.m_hostDiagnosticCache.value(QStringLiteral("filesystem"));
    QVERIFY2(cachedHostFilesystem.contains(QStringLiteral("File system check")),
             qPrintable(cachedHostFilesystem));
    QVERIFY2(window.m_diagnosticResults->toPlainText().contains(QStringLiteral("File system check")),
             "the host File systems result must be rendered");

    // The Diagnostics list is backend-family independent: the File systems
    // entry stays present for every detected family (Debian/Arch/Alpine/Fedora)
    // and only its capability evidence changes the repair-stage gate.
    for (const QString &familyEvidence : {capabilityEvidence(false, true),
                                          capabilityEvidence(true, true),
                                          capabilityEvidenceAlpine(true, true, true),
                                          capabilityEvidenceFedora(true, true, true, true)}) {
        cacheRepairEvidence(window, familyEvidence);
        const int familyFilesystemRow = diagnosticRow(window, QStringLiteral("filesystem"));
        QVERIFY2(familyFilesystemRow >= 0,
                 "the File systems entry must be offered for every backend family");
        QCOMPARE(window.m_diagnosticList->item(familyFilesystemRow)->text(),
                 QStringLiteral("File systems"));
    }
}

// The Diagnostics scope is derived from the Systems page and can never be
// chosen independently: Host Maintenance active means Running Host, otherwise
// the committed repair target. The standard top-right scope line follows every
// Systems transition; a Repair Target run without a committed target raises
// the existing "No repair target selected" popup; cached running-host results
// stay hidden while maintenance is off.
void MainWindowUiTest::hostDiagnosticsRequireHostMaintenanceScope()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);

    // Default state: the derived scope is Repair Target with no committed
    // target, so the standard scope line names no target.
    QVERIFY(window.m_diagnosticScopeLabel);
    QCOMPARE(window.m_diagnosticScopeLabel->text(), QStringLiteral("Target: none selected"));
    QVERIFY2(window.m_targetConfigCombo && window.m_targetConfigCombo->isHidden(),
             "target configuration controls are only meaningful for a committed repair target");
    QVERIFY2(!window.m_runDiagnosticButton->isEnabled(),
             "the individual run must not be offered without a target or host maintenance");

    // A no-target Run All keeps the existing popup and dispatches nothing.
    {
        NextMessageBoxCapture capture(&window, QMessageBox::Ok);
        window.runAllDiagnostics();
        QVERIFY2(capture.appeared, "Run All without a target must raise the no-target popup");
        QCOMPARE(capture.title, QStringLiteral("No repair target selected"));
    }
    const QString noTargetLog = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QVERIFY2(!noTargetLog.contains(QStringLiteral("Starting all available read-only diagnostics")),
             "a no-target Run All must not dispatch a diagnostic request");

    // Cached running-host evidence is never rendered while maintenance is off,
    // even though the cache keeps it for the next maintenance session.
    window.m_hostDiagnosticCache.insert(QStringLiteral("environment"),
                                        QStringLiteral("HOST_CACHE_MARKER must stay hidden"));
    window.updateDiagnosticDetails();
    QVERIFY2(!window.m_diagnosticResults->toPlainText().contains(QStringLiteral("HOST_CACHE_MARKER")),
             "cached host diagnostics must not be displayed outside Host Maintenance");
    QVERIFY2(!window.m_diagnosticAvailability->text().contains(QStringLiteral("running-host")),
             qPrintable(window.m_diagnosticAvailability->text()));
    QVERIFY2(window.m_hostDiagnosticCache.value(QStringLiteral("environment"))
                 .contains(QStringLiteral("HOST_CACHE_MARKER")),
             "hiding host results must not clear the cached host evidence");

    // Entering through the real Systems handler switches the standard scope
    // line to the protected running host.
    prepareRepairScope(window, true);
    window.m_hostMaintenanceMode = false;
    window.updateHostSystemSummary({window.m_deviceIndex.value(QStringLiteral("/dev/test-host"))});
    window.updateTargetLabels();
    QCOMPARE(window.m_diagnosticScopeLabel->text(), QStringLiteral("Target: none selected"));
    window.selectHostForMaintenance();
    QVERIFY2(window.m_hostMaintenanceMode,
             "Host Maintenance must be entered through the real handler");
    QCOMPARE(window.m_diagnosticScopeLabel->text(),
             QStringLiteral("Host maintenance: /dev/test-host"));
    QVERIFY2(window.m_targetConfigCombo->isHidden(),
             "target configuration controls are never meaningful in host maintenance");

    // Host scope now works: the File systems diagnostic dispatches the host
    // command and its result is cached and rendered.
    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("host-gate-fs.log"));
    QVERIFY(startFilesystemFakePrivilegedSession(window, capturePath));
    const int filesystemRow = diagnosticRow(window, QStringLiteral("filesystem"));
    QVERIFY(filesystemRow >= 0);
    window.m_diagnosticList->setCurrentRow(filesystemRow);
    window.runSelectedDiagnostic();
    const QList<QStringList> requests = capturedHelperRequests(capturePath);
    QVERIFY2(!requests.isEmpty(), "the host File systems diagnostic must issue a helper request");
    QVERIFY2(requests.first().contains(QStringLiteral("host-fs-inspect")),
             qPrintable(requests.first().join(QLatin1Char(' '))));
    QVERIFY(window.m_diagnosticResults->toPlainText().contains(QStringLiteral("File system check")));

    // Exiting through the real handler returns to Repair Target, hides the
    // host results and keeps the cache for the next maintenance session.
    window.exitHostMaintenanceMode();
    QVERIFY(!window.m_hostMaintenanceMode);
    QCOMPARE(window.m_diagnosticScopeLabel->text(), QStringLiteral("Target: none selected"));
    QVERIFY2(!window.m_diagnosticResults->toPlainText().contains(QStringLiteral("File system check")),
             "cached host results must not remain shown after leaving maintenance");
    QVERIFY2(window.m_hostDiagnosticCache.value(QStringLiteral("filesystem"))
                 .contains(QStringLiteral("File system check")),
             "leaving maintenance must keep the cached host evidence for the next session");

    // Committing a repair target moves the derived scope back to Repair Target
    // with the target disk named in the standard scope line.
    prepareRepairScope(window);
    window.updateTargetLabels();
    QVERIFY(!window.m_hostMaintenanceMode);
    QCOMPARE(window.m_diagnosticScopeLabel->text(),
             QStringLiteral("Target: /dev/test-system"));
    QVERIFY2(!window.m_targetConfigCombo->isHidden(),
             "target configuration controls must appear for a committed repair target");

    // A target Run All dispatches target diagnostics and never touches the
    // cached running-host evidence.
    window.runAllDiagnostics();
    const QString targetLog = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QVERIFY2(targetLog.contains(QStringLiteral("Starting all available read-only diagnostics (Target scope)")),
             "a committed target Run All must dispatch the target scope");
    QVERIFY2(window.m_hostDiagnosticCache.value(QStringLiteral("filesystem"))
                 .contains(QStringLiteral("File system check")),
             "a target run must not clear the cached host evidence");
    QVERIFY2(!window.m_diagnosticResults->toPlainText().contains(QStringLiteral("HOST_CACHE_MARKER")),
             "target results must never render host cache markers");
}

// A selection or scope change gets exactly one Btrfs snapshot inventory
// request; re-scheduling the same scope (including a results refresh, a
// deferred retry and the host-maintenance round trip) never repeats the
// identical "Loading Btrfs snapshots" request.
void MainWindowUiTest::snapshotPreloadIsDeduplicatedAcrossScopeTransitions()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_autoRefreshDiagnostics->setChecked(false);

    // Synthetic protected running host plus two eligible Btrfs repair targets.
    prepareRepairScope(window, true);
    window.m_hostMaintenanceMode = false;
    window.updateHostSystemSummary({window.m_deviceIndex.value(QStringLiteral("/dev/test-host"))});
    installSyntheticBtrfsTarget(window, QStringLiteral("/dev/test-target-a"),
                                QStringLiteral("/dev/test-target-a1"));
    installSyntheticBtrfsTarget(window, QStringLiteral("/dev/test-target-b"),
                                QStringLiteral("/dev/test-target-b1"));

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("dedupe-snapshot.log"));
    QVERIFY2(startSlowSnapshotFakePrivilegedSession(window, capturePath, QStringLiteral("0.2")),
             "the scripted privileged session must start");
    QVERIFY(window.m_privilegedSessionReady);

    const auto snapshotRequestCount = [&capturePath] {
        int count = 0;
        for (const QStringList &request : capturedHelperRequests(capturePath)) {
            if (!request.isEmpty() && request.first() == QStringLiteral("snapshots")) {
                ++count;
            }
        }
        return count;
    };

    // First selection: one automatic preload.
    selectTopLevelDisk(window, QStringLiteral("/dev/test-target-a"));
    window.setPreviewTarget();
    QCOMPARE(window.m_previewTargetPath, QStringLiteral("/dev/test-target-a"));
    QTRY_VERIFY_WITH_TIMEOUT(window.m_snapshotTable->rowCount() == 1
                                 && !window.m_privilegedOperationActive, 8000);
    QCOMPARE(snapshotRequestCount(), 1);

    // Re-scheduling and a results refresh within the same scope generation
    // must not issue a second identical request.
    window.scheduleSnapshotPreload();
    window.scheduleSnapshotPreload();
    QTest::qWait(150);
    QCOMPARE(snapshotRequestCount(), 1);
    window.clearSnapshotResults();
    window.scheduleSnapshotPreload();
    QTest::qWait(150);
    QCOMPARE(snapshotRequestCount(), 1);

    // Host-maintenance round trip back to the same target is a scope change:
    // exactly one new load, and no duplicate afterwards.
    window.selectHostForMaintenance();
    QTest::qWait(50);
    selectTopLevelDisk(window, QStringLiteral("/dev/test-target-a"));
    window.setPreviewTarget();
    QTRY_VERIFY_WITH_TIMEOUT(window.m_snapshotTable->rowCount() == 1
                                 && !window.m_privilegedOperationActive, 8000);
    QCOMPARE(snapshotRequestCount(), 2);
    window.scheduleSnapshotPreload();
    QTest::qWait(150);
    QCOMPARE(snapshotRequestCount(), 2);

    // A manual Load still refreshes once; the automatic preload never
    // duplicates it.
    window.loadSnapshots();
    QCOMPARE(snapshotRequestCount(), 3);
    window.scheduleSnapshotPreload();
    QTest::qWait(150);
    QCOMPARE(snapshotRequestCount(), 3);
}

// Installed packages must resolve /usr/libexec/boot-repair/boot-repair-helper
// instead of a developer source-tree helper; only an explicit user override
// may win over the installed path, and a source/build run keeps its own helper.
void MainWindowUiTest::helperPathResolutionPrefersInstalledUnlessOverridden()
{
    // Installed layout (/usr/bin): the installed libexec helper is first; the
    // dev/source helper, if present at all, is only a last-resort fallback.
    const QList<MainWindow::HelperPathCandidate> installed =
        MainWindow::repairHelperCandidates(QStringLiteral("/usr/bin"), QByteArray());
    QVERIFY(!installed.isEmpty());
    QCOMPARE(installed.first().path,
             QStringLiteral("/usr/libexec/boot-repair/boot-repair-helper"));
    QVERIFY2(installed.first().reason.contains(QStringLiteral("installed")),
             qPrintable(installed.first().reason));
    int installedSourceIndex = -1;
    for (int index = 0; index < installed.size(); ++index) {
        if (installed.at(index).path.contains(QStringLiteral("scripts/boot-repair-helper.sh"))) {
            installedSourceIndex = index;
            break;
        }
    }
    if (installedSourceIndex >= 0) {
        QVERIFY2(installedSourceIndex > 0,
                 "a source-tree helper must never precede the installed helper");
        QVERIFY2(installed.at(installedSourceIndex).reason.contains(QStringLiteral("missing")),
                 qPrintable(installed.at(installedSourceIndex).reason));
    }

    // Build/portable layout: the helper beside or above the executable wins
    // over any stale installed helper.
    const QList<MainWindow::HelperPathCandidate> portable =
        MainWindow::repairHelperCandidates(QStringLiteral("/home/user/build-release"), QByteArray());
    int portableSourceIndex = -1;
    int portableInstalledIndex = -1;
    for (int index = 0; index < portable.size(); ++index) {
        if (portable.at(index).path.contains(QStringLiteral("scripts/boot-repair-helper.sh"))
            && portableSourceIndex < 0) {
            portableSourceIndex = index;
        }
        if (portable.at(index).path == QStringLiteral("/usr/libexec/boot-repair/boot-repair-helper")
            && portableInstalledIndex < 0) {
            portableInstalledIndex = index;
        }
    }
    QVERIFY2(portableSourceIndex >= 0, "a build run must offer its source-tree helper");
    QVERIFY2(portableInstalledIndex >= 0, "the installed helper stays a fallback");
    QVERIFY2(portableSourceIndex < portableInstalledIndex,
             "a build run must prefer its source-tree helper over a stale installed one");

    // An explicit override always wins over every candidate.
    const QList<MainWindow::HelperPathCandidate> overridden = MainWindow::repairHelperCandidates(
        QStringLiteral("/usr/bin"), QByteArray("/opt/custom/boot-repair-helper"));
    QVERIFY(!overridden.isEmpty());
    QCOMPARE(overridden.first().path, QStringLiteral("/opt/custom/boot-repair-helper"));
    QVERIFY2(overridden.first().reason.contains(QStringLiteral("BOOT_REPAIR_HELPER_PATH")),
             qPrintable(overridden.first().reason));

    // End to end: a real override file resolves through the same path, and the
    // window's resolution honors the environment override.
    QTemporaryDir helperDir;
    QVERIFY(helperDir.isValid());
    const QString overridePath = helperDir.filePath(QStringLiteral("boot-repair-helper"));
    QFile overrideFile(overridePath);
    QVERIFY(overrideFile.open(QIODevice::WriteOnly | QIODevice::Text));
    overrideFile.write("#!/bin/sh\nexit 0\n");
    overrideFile.close();
    QVERIFY(QFile::setPermissions(overridePath,
            QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner));
    const QString canonicalOverride = QFileInfo(overridePath).canonicalFilePath();
    QString reason;
    QCOMPARE(MainWindow::resolveRepairHelperPath(QStringLiteral("/usr/bin"),
                                                 overridePath.toUtf8(), &reason),
             canonicalOverride);
    QVERIFY2(reason.contains(QStringLiteral("override")), qPrintable(reason));

    MainWindow window;
    {
        ScopedEnvironmentVariable overrideEnv("BOOT_REPAIR_HELPER_PATH", overridePath.toUtf8());
        QString windowReason;
        QCOMPARE(window.repairHelperPath(&windowReason), canonicalOverride);
        QVERIFY2(windowReason.contains(QStringLiteral("override")), qPrintable(windowReason));
    }
    QVERIFY2(!window.repairHelperPath().isEmpty(),
             "a build run must still resolve its source-tree helper");
}

// The family-specific stage labels are applied per backend and reset for
// Debian/Arch, so Alpine wording can never leak into another family's plan.
void MainWindowUiTest::stageLabelsTrackBackendFamilyWithoutLeaking()
{
    MainWindow window;
    for (bool host : {false, true}) {
        prepareRepairScope(window, host);

        cacheRepairEvidence(window, capabilityEvidenceAlpine(true, true, true));
        window.updateFullRepairSummary();
        QCOMPARE(window.m_fullRepairBrokenPackages->text(),
                 QStringLiteral("Repair Alpine packages (apk fix)"));
        QCOMPARE(window.m_fullRepairUpgrade->text(),
                 QStringLiteral("Upgrade Alpine packages (apk upgrade)"));
        QCOMPARE(window.m_fullRepairDisplayManager->text(),
                 QStringLiteral("Restore detected graphical login manager (OpenRC runlevel)"));
        QCOMPARE(window.m_fullRepairInitramfs->text(),
                 QStringLiteral("Rebuild initramfs (mkinitfs)"));
        QCOMPARE(window.m_fullRepairExtlinux->text(),
                 QStringLiteral("Regenerate extlinux configuration"));
        // The Alpine BIOS/extlinux fixture keeps the generic EFI/GRUB labels.
        QCOMPARE(window.m_fullRepairEfi->text(),
                 QStringLiteral("Repair EFI / UKI boot path (explicit target ESP repair)"));
        QCOMPARE(window.m_fullRepairGrub->text(), QStringLiteral("Update GRUB configuration"));

        // An Alpine UEFI GRUB target gets the Alpine-specific EFI/GRUB labels.
        cacheRepairEvidence(window, capabilityEvidenceAlpineUefi(true, true));
        window.updateFullRepairSummary();
        QCOMPARE(window.m_fullRepairEfi->text(), QStringLiteral("Reinstall Alpine GRUB EFI loader"));
        QCOMPARE(window.m_fullRepairGrub->text(), QStringLiteral("Regenerate Alpine GRUB configuration"));

        // A Fedora/RPM target gets the dnf/dracut/GRUB2/GDM wording; the
        // Debian reset below proves none of it leaks into another family.
        cacheRepairEvidence(window, capabilityEvidenceFedora(true, true, true, true));
        window.updateFullRepairSummary();
        QCOMPARE(window.m_fullRepairBrokenPackages->text(),
                 QStringLiteral("Repair Fedora packages (dnf)"));
        QCOMPARE(window.m_fullRepairAptUpdate->text(),
                 QStringLiteral("Refresh package metadata (dnf makecache)"));
        QCOMPARE(window.m_fullRepairUpgrade->text(),
                 QStringLiteral("Upgrade Fedora packages (dnf upgrade)"));
        QCOMPARE(window.m_fullRepairInitramfs->text(),
                 QStringLiteral("Rebuild initramfs (dracut)"));
        QCOMPARE(window.m_fullRepairGrub->text(),
                 QStringLiteral("Update GRUB2 configuration"));
        QCOMPARE(window.m_fullRepairEfi->text(),
                 QStringLiteral("Reinstall GRUB2 bootloader"));
        QCOMPARE(window.m_fullRepairDisplayManager->text(),
                 QStringLiteral("Restore detected graphical login manager (GDM)"));

        cacheRepairEvidence(window, capabilityEvidence(false, true));
        window.updateFullRepairSummary();
        QCOMPARE(window.m_fullRepairBrokenPackages->text(),
                 QStringLiteral("Repair broken package dependencies"));
        QCOMPARE(window.m_fullRepairAptUpdate->text(),
                 QStringLiteral("Refresh package metadata"));
        QCOMPARE(window.m_fullRepairEfi->text(),
                 QStringLiteral("Repair EFI / UKI boot path (explicit target ESP repair)"));
        QCOMPARE(window.m_fullRepairGrub->text(), QStringLiteral("Update GRUB configuration"));
        QCOMPARE(window.m_fullRepairUpgrade->text(),
                 QStringLiteral("Upgrade installed packages (adaptive APT simulation)"));
        QCOMPARE(window.m_fullRepairDisplayManager->text(),
                 QStringLiteral("Restore detected graphical login manager and graphical.target"));
        QCOMPARE(window.m_fullRepairInitramfs->text(),
                 QStringLiteral("Rebuild initramfs after mapper/crypttab validation"));
        QCOMPARE(window.m_fullRepairExtlinux->text(),
                 QStringLiteral("Update extlinux configuration"));
        QVERIFY(!window.repairToolAvailable(QStringLiteral("extlinux")));

        cacheRepairEvidence(window, capabilityEvidence(true, false));
        window.updateFullRepairSummary();
        QCOMPARE(window.m_fullRepairBrokenPackages->text(),
                 QStringLiteral("Repair Arch package dependencies (full pacman transaction)"));
        QCOMPARE(window.m_fullRepairUpgrade->text(),
                 QStringLiteral("Upgrade Arch packages (full pacman transaction)"));
        QCOMPARE(window.m_fullRepairDisplayManager->text(),
                 QStringLiteral("Restore detected graphical login manager and graphical.target"));
        QCOMPARE(window.m_fullRepairInitramfs->text(),
                 QStringLiteral("Rebuild initramfs (mkinitcpio)"));
        QCOMPARE(window.m_fullRepairExtlinux->text(),
                 QStringLiteral("Update extlinux configuration"));
        QVERIFY(!window.repairToolAvailable(QStringLiteral("extlinux")));
    }
}

// A target with several detected package managers runs every backend, so the
// Settings labels stay generic; the service-manager, initramfs and bootloader
// labels still follow their own detected backends.
void MainWindowUiTest::mixedPackageManagerLabelsStayGeneric()
{
    const QString mixedEvidence = QStringLiteral(
        "Distribution family: debian\n"
        "Package manager backend: apt/dpkg\n"
        "Package manager backends: apt/dpkg, apk\n"
        "Service manager: OpenRC\n"
        "Initramfs backend: mkinitfs\n"
        "Bootloader backend: syslinux/extlinux\n"
        "Repair tool validate: available\n"
        "Repair tool fixbroken: available\n"
        "Repair tool upgrade: available\n"
        "Repair tool display: available\n"
        "Repair tool initramfs: available\n"
        "Repair tool extlinux: available\n");

    // A mixed rpm + apt/dpkg target: both package backends run, so the package
    // labels stay generic while the initramfs, bootloader and display labels
    // still follow their own detected backends.
    const QString mixedRpmEvidence = QStringLiteral(
        "Distribution family: fedora\n"
        "Package manager backend: rpm\n"
        "Package manager backends: rpm, apt/dpkg\n"
        "Service manager: systemd\n"
        "Display manager backend: systemd\n"
        "Initramfs backend: dracut\n"
        "Bootloader backend: grub\n"
        "Repair capability evidence display: graphical.target present; display manager unit gdm.service\n"
        "Repair capability evidence grub: grub2-mkconfig present; /boot/grub2/grub.cfg present\n"
        "Repair tool validate: available\n"
        "Repair tool fixbroken: available\n"
        "Repair tool aptupdate: available\n"
        "Repair tool upgrade: available\n"
        "Repair tool display: available\n"
        "Repair tool initramfs: available\n"
        "Repair tool grub: available\n");

    MainWindow window;
    for (bool host : {false, true}) {
        prepareRepairScope(window, host);
        cacheRepairEvidence(window, mixedEvidence);
        window.updateFullRepairSummary();
        QCOMPARE(window.m_fullRepairBrokenPackages->text(),
                 QStringLiteral("Repair broken package dependencies (all detected package managers)"));
        QCOMPARE(window.m_fullRepairUpgrade->text(),
                 QStringLiteral("Upgrade installed packages (all detected package managers)"));
        QCOMPARE(window.m_fullRepairDisplayManager->text(),
                 QStringLiteral("Restore detected graphical login manager (OpenRC runlevel)"));
        QCOMPARE(window.m_fullRepairInitramfs->text(),
                 QStringLiteral("Rebuild initramfs (mkinitfs)"));
        QCOMPARE(window.m_fullRepairExtlinux->text(),
                 QStringLiteral("Regenerate extlinux configuration"));

        cacheRepairEvidence(window, mixedRpmEvidence);
        window.updateFullRepairSummary();
        QCOMPARE(window.m_fullRepairBrokenPackages->text(),
                 QStringLiteral("Repair broken package dependencies (all detected package managers)"));
        QCOMPARE(window.m_fullRepairAptUpdate->text(),
                 QStringLiteral("Refresh package metadata"));
        QCOMPARE(window.m_fullRepairUpgrade->text(),
                 QStringLiteral("Upgrade installed packages (all detected package managers)"));
        QCOMPARE(window.m_fullRepairInitramfs->text(),
                 QStringLiteral("Rebuild initramfs (dracut)"));
        QCOMPARE(window.m_fullRepairGrub->text(),
                 QStringLiteral("Update GRUB2 configuration"));
        QCOMPARE(window.m_fullRepairDisplayManager->text(),
                 QStringLiteral("Restore detected graphical login manager (GDM)"));
    }
}

void MainWindowUiTest::disabledSettingsDoNotLeakIntoFullRepairAndPreservePreferences()
{
    QTemporaryDir configDir;
    QVERIFY(configDir.isValid());
    const QByteArray savedConfig = qgetenv("XDG_CONFIG_HOME");
    qputenv("XDG_CONFIG_HOME", configDir.path().toUtf8());
    {
        QSettings persist(QStringLiteral("BootRepair"), QStringLiteral("BootRepair"));
        persist.setValue(QStringLiteral("repair/dkms"), true);
        persist.setValue(QStringLiteral("repair/dpkgConfigure"), true);
        persist.sync();
    }
    {
        MainWindow window;
        prepareRepairScope(window);
        cacheRepairEvidence(window, capabilityEvidence(true, false));
        window.m_fullRepairDkms->setChecked(true);
        window.m_fullRepairDpkg->setChecked(true);
        window.updateFullRepairSummary();
        QVERIFY(window.m_fullRepairDkms->isChecked());
        QVERIFY(!window.m_fullRepairDkms->isEnabled());
        QVERIFY(window.m_fullRepairDpkg->isChecked());
        QVERIFY(!window.m_fullRepairDpkg->isEnabled());
        QVERIFY(!window.selectedRepairStages().contains(QStringLiteral("dkms")));
        QVERIFY(!window.selectedRepairStages().contains(QStringLiteral("dpkg-configure")));
        window.m_fullRepairGrub->setChecked(true);
        for (QCheckBox *check : {window.m_fullRepairBrokenPackages, window.m_fullRepairUpgrade,
                                 window.m_fullRepairInitramfs, window.m_fullRepairEfi}) {
            check->setChecked(false);
        }
        window.updateFullRepairSummary();
        QVERIFY(window.m_runFullRepairButton->isEnabled());
        QCOMPARE(window.selectedRepairStages(), QStringList{QStringLiteral("grub")});
        window.m_fullRepairGrub->setChecked(false);
        QVERIFY(!window.m_runFullRepairButton->isEnabled());
        window.m_fullRepairExtlinux->setChecked(false);
        window.saveSettings();
        QCOMPARE(QSettings(QStringLiteral("BootRepair"), QStringLiteral("BootRepair"))
                     .value(QStringLiteral("repair/dkms")).toBool(), true);
        QCOMPARE(QSettings(QStringLiteral("BootRepair"), QStringLiteral("BootRepair"))
                     .value(QStringLiteral("repair/dpkgConfigure")).toBool(), true);
        QCOMPARE(QSettings(QStringLiteral("BootRepair"), QStringLiteral("BootRepair"))
                     .value(QStringLiteral("repair/extlinux")).toBool(), false);
    }
    {
        MainWindow window;
        window.loadSettings();
        QVERIFY(window.m_fullRepairDkms->isChecked());
        QVERIFY(window.m_fullRepairDpkg->isChecked());
        QVERIFY(!window.m_fullRepairExtlinux->isChecked());
        QVERIFY(!window.m_fullRepairDkms->isEnabled());
        QVERIFY(!window.m_fullRepairDpkg->isEnabled());
        for (QCheckBox *check : {window.m_fullRepairBrokenPackages, window.m_fullRepairUpgrade,
                                 window.m_fullRepairInitramfs, window.m_fullRepairEfi,
                                 window.m_fullRepairGrub}) {
            check->setChecked(false);
        }
        window.updateFullRepairSummary();
        QVERIFY(window.selectedRepairStages().isEmpty());
        QVERIFY(!window.m_runFullRepairButton->isEnabled());
    }
    qputenv("XDG_CONFIG_HOME", savedConfig);
}

void MainWindowUiTest::autoRefreshSettingDefaultsOnAndPersists()
{
    MainWindow window;
    QVERIFY(window.m_autoRefreshDiagnostics);
    QVERIFY(window.m_autoRefreshDiagnostics->isChecked());
    QVERIFY(window.m_autoRefreshDiagnostics->text().contains(
        QStringLiteral("Automatically regenerate read-only diagnostics")));

    window.m_autoRefreshDiagnostics->setChecked(false);
    window.saveSettings();
    QCOMPARE(QSettings(QStringLiteral("BootRepair"), QStringLiteral("BootRepair"))
                 .value(QStringLiteral("diagnostics/autoRefreshStale")).toBool(), false);

    window.m_autoRefreshDiagnostics->setChecked(true);
    window.saveSettings();
    QCOMPARE(QSettings(QStringLiteral("BootRepair"), QStringLiteral("BootRepair"))
                 .value(QStringLiteral("diagnostics/autoRefreshStale")).toBool(), true);
}

void MainWindowUiTest::autoRefreshDecisionRespectsSettingScopeAndEvidence()
{
    MainWindow window;
    prepareStaleTargetScope(window);

    QString reason;
    QVERIFY(window.shouldScheduleEvidenceRefresh(&reason));
    QVERIFY(!reason.isEmpty());

    window.m_autoRefreshDiagnostics->setChecked(false);
    QVERIFY(!window.shouldScheduleEvidenceRefresh(&reason));
    window.m_autoRefreshDiagnostics->setChecked(true);
    QVERIFY(window.shouldScheduleEvidenceRefresh(&reason));

    cacheRepairEvidence(window, capabilityEvidence(false, true));
    window.m_targetDiagnosticsNeedRegeneration = false;
    QVERIFY(!window.shouldScheduleEvidenceRefresh(&reason));

    window.m_targetDiagnosticsNeedRegeneration = true;
    QVERIFY(window.shouldScheduleEvidenceRefresh(&reason));

    window.m_previewTargetPath.clear();
    QVERIFY(!window.shouldScheduleEvidenceRefresh(&reason));

    prepareStaleTargetScope(window);
    window.m_privilegedOperationActive = true;
    QVERIFY(!window.shouldScheduleEvidenceRefresh(&reason));
    window.m_privilegedOperationActive = false;
    QVERIFY(window.shouldScheduleEvidenceRefresh(&reason));
}

void MainWindowUiTest::autoRefreshStaysIdleWithoutSessionOrSetting()
{
    MainWindow window;
    prepareStaleTargetScope(window);
    window.m_privilegedSessionReady = true;
    window.scheduleEvidenceRefresh(QStringLiteral("UI test invalidation"));
    QVERIFY(window.m_evidenceRefreshTimer);
    QVERIFY(window.m_evidenceRefreshTimer->isActive());

    // Disabling the setting cancels the coalescing timer before it fires.
    window.m_autoRefreshDiagnostics->setChecked(false);
    QVERIFY(!window.m_evidenceRefreshPending);
    QVERIFY(!window.m_evidenceRefreshTimer->isActive());

    QTest::qWait(kNoSecondRunWaitMs);
    QCOMPARE(diagnosticRunCount(window), 0);
    QVERIFY(window.m_targetDiagnosticsNeedRegeneration);

    // --- merged from autoRefreshWithoutSessionStaysPending() ---
    {
        MainWindow window;
        prepareStaleTargetScope(window);
        QVERIFY(!window.m_privilegedSessionReady);

        window.scheduleEvidenceRefresh(QStringLiteral("UI test invalidation"));
        QVERIFY(window.m_evidenceRefreshPending);
        QVERIFY(!window.m_evidenceRefreshTimer || !window.m_evidenceRefreshTimer->isActive());

        QTest::qWait(kNoSecondRunWaitMs);
        QCOMPARE(diagnosticRunCount(window), 0);
        QVERIFY(window.m_targetDiagnosticsNeedRegeneration);
        QVERIFY(!window.m_targetDiagnosticCache.contains(QStringLiteral("report")));
        QVERIFY(window.m_actionLogEntries.join(QLatin1Char('\n')).contains(
            QStringLiteral("pending until administrator authorization")));
    }
}

void MainWindowUiTest::autoRefreshSessionEstablishedRunsOnceAndCoalescesBursts()
{
    MainWindow window;
    prepareStaleTargetScope(window);
    window.m_evidenceRefreshDelayMs = kEvidenceRefreshTestDelayMs;
    window.scheduleEvidenceRefresh(QStringLiteral("UI test invalidation"));
    QVERIFY(window.m_evidenceRefreshPending);

    window.m_privilegedSessionReady = true;
    window.handlePrivilegedSessionEstablished();
    QVERIFY(!window.m_evidenceRefreshPending);
    QVERIFY(window.m_evidenceRefreshTimer);
    QVERIFY(window.m_evidenceRefreshTimer->isActive());

    QTRY_VERIFY(diagnosticRunCount(window) >= 1);
    QTest::qWait(kNoSecondRunWaitMs);
    QCOMPARE(diagnosticRunCount(window), 1);
    QVERIFY(!window.m_targetDiagnosticsNeedRegeneration);
    QVERIFY(window.m_targetDiagnosticCache.contains(QStringLiteral("report")));
    QVERIFY(window.m_actionLogEntries.join(QLatin1Char('\n')).contains(
        QStringLiteral("Automatic read-only diagnostics regeneration completed")));
    QVERIFY(window.statusBar()->currentMessage().contains(
        QStringLiteral("Read-only diagnostics regenerated automatically")));

    // --- merged from autoRefreshCoalescesRapidInvalidations() ---
    {
        MainWindow window;
        prepareStaleTargetScope(window);
        window.m_privilegedSessionReady = true;
        window.m_evidenceRefreshDelayMs = kEvidenceRefreshTestDelayMs;

        window.scheduleEvidenceRefresh(QStringLiteral("first invalidation"));
        QTest::qWait(10);
        window.scheduleEvidenceRefresh(QStringLiteral("second invalidation"));
        QVERIFY(window.m_evidenceRefreshTimer);
        QVERIFY(window.m_evidenceRefreshTimer->isActive());

        QTRY_VERIFY(diagnosticRunCount(window) >= 1);
        QTest::qWait(kNoSecondRunWaitMs);
        QCOMPARE(diagnosticRunCount(window), 1);
    }
}

void MainWindowUiTest::autoRefreshCoalescesPlanAndManualRuns()
{
    MainWindow window;
    prepareStaleTargetScope(window);
    window.m_privilegedSessionReady = true;
    window.m_evidenceRefreshDelayMs = kEvidenceRefreshTestDelayMs;

    // While the Full Repair request owns the helper, per-stage invalidations
    // must not schedule anything.
    window.m_privilegedOperationActive = true;
    for (int stage = 0; stage < 3; ++stage) {
        window.scheduleEvidenceRefresh(QStringLiteral("Full Repair stage %1").arg(stage));
    }
    QVERIFY(!window.m_evidenceRefreshPending);
    QVERIFY(!window.m_evidenceRefreshTimer || !window.m_evidenceRefreshTimer->isActive());

    // The single completion hook after the whole operation schedules one run.
    window.m_privilegedOperationActive = false;
    window.scheduleEvidenceRefresh(QStringLiteral("Full Repair completed"));
    QTRY_VERIFY(diagnosticRunCount(window) >= 1);
    QTest::qWait(kNoSecondRunWaitMs);
    QCOMPARE(diagnosticRunCount(window), 1);
    QVERIFY(!window.m_targetDiagnosticsNeedRegeneration);

    // --- merged from autoRefreshManualRunAllDoesNotDoubleRun() ---
    {
        MainWindow window;
        prepareStaleTargetScope(window);
        window.m_privilegedSessionReady = true;
        window.m_evidenceRefreshDelayMs = kEvidenceRefreshTestDelayMs;
        window.scheduleEvidenceRefresh(QStringLiteral("UI test invalidation"));
        QVERIFY(window.m_evidenceRefreshTimer);
        QVERIFY(window.m_evidenceRefreshTimer->isActive());

        // Manual Run All before the coalescing timer fires regenerates once and
        // leaves the scheduled automatic run a no-op.
        window.runAllDiagnostics();
        QVERIFY(!window.m_diagnosticsRunInProgress);
        QCOMPARE(diagnosticRunCount(window), 1);
        QVERIFY(!window.m_targetDiagnosticsNeedRegeneration);

        QTest::qWait(kNoSecondRunWaitMs);
        QCOMPARE(diagnosticRunCount(window), 1);
    }
}

// The single invalidation mapping is the contract every repair action uses;
// this table mirrors the documented MainWindow.cpp table.
void MainWindowUiTest::diagnosticSectionMappingTable()
{
    MainWindow window;
    QCOMPARE(window.diagnosticSectionsForRepair(QStringLiteral("validate")), QStringList());
    QCOMPARE(window.diagnosticSectionsForRepair(QStringLiteral("display")),
             QStringList({QStringLiteral("display")}));
    QCOMPARE(window.diagnosticSectionsForRepair(QStringLiteral("display-manager")),
             QStringList({QStringLiteral("display")}));
    QCOMPARE(window.diagnosticSectionsForRepair(QStringLiteral("grub")),
             QStringList({QStringLiteral("grub")}));
    QCOMPARE(window.diagnosticSectionsForRepair(QStringLiteral("extlinux")),
             QStringList({QStringLiteral("boot"), QStringLiteral("boot-evidence")}));
    QCOMPARE(window.diagnosticSectionsForRepair(QStringLiteral("initramfs")),
             QStringList({QStringLiteral("kernel")}));
    QCOMPARE(window.diagnosticSectionsForRepair(QStringLiteral("dkms")),
             QStringList({QStringLiteral("kernel")}));
    QCOMPARE(window.diagnosticSectionsForRepair(QStringLiteral("efi")),
             QStringList({QStringLiteral("uki"), QStringLiteral("grub"), QStringLiteral("boot-evidence")}));
    QCOMPARE(window.diagnosticSectionsForRepair(QStringLiteral("filesystem")),
             QStringList({QStringLiteral("environment"), QStringLiteral("usage"),
                          QStringLiteral("btrfs"), QStringLiteral("filesystem")}));
    for (const QString &broad : {QStringLiteral("dpkg"), QStringLiteral("dpkg-configure"),
                                 QStringLiteral("fixbroken"), QStringLiteral("fix-broken"),
                                 QStringLiteral("aptupdate"), QStringLiteral("apt-update"),
                                 QStringLiteral("upgrade"), QStringLiteral("apt-upgrade"),
                                 QStringLiteral("bootstack"), QStringLiteral("boot-stack"),
                                 QStringLiteral("host-default"), QStringLiteral("full-repair"),
                                 QStringLiteral("shell"), QStringLiteral("unknown")}) {
        QCOMPARE(window.diagnosticSectionsForRepair(broad), QStringList({QStringLiteral("all")}));
    }
}

// A narrow repair tool maps to exactly one diagnostic section: after a
// display-manager repair only the display diagnostic is regenerated and the
// complete report is never re-run.
void MainWindowUiTest::scopedRepairRegeneratesOnlyMappedSections()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareRepairScope(window);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(true);
    cacheRepairEvidence(window, capabilityEvidenceWithDisplay(false, true));

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("scoped-display-requests.log"));
    QVERIFY2(startRepairFakePrivilegedSession(window, capturePath),
             "the scripted privileged session must start");
    closeRepairProgressDialogWhenDone(&window);
    acceptNextMessageBox(&window, QMessageBox::Yes);

    QTreeWidgetItem *item = repairItem(window, QStringLiteral("display"));
    QVERIFY(item);
    window.m_repairToolTree->setCurrentItem(item);
    window.runSelectedRepairTool();

    QVERIFY2(capturedHelperArguments(capturePath).contains(QStringLiteral("display-manager")),
             "the display-manager repair must be dispatched");

    // The display-manager mapping invalidates exactly the display section.
    QVERIFY2(window.m_targetDiagnosticsStaleSections.contains(QStringLiteral("display")),
             "the display repair must mark the display section stale");
    QCOMPARE(window.m_targetDiagnosticsStaleSections.size(), 1);
    QVERIFY2(!window.m_targetDiagnosticsNeedRegeneration,
             "a narrow repair must not invalidate the complete target set");
    QVERIFY2(window.m_targetDiagnosticCache.value(QStringLiteral("display")).trimmed().isEmpty(),
             "the stale display section must be dropped from the cache");
    QVERIFY2(!window.m_targetDiagnosticCache.value(QStringLiteral("grub")).trimmed().isEmpty(),
             "unrelated cached sections must remain valid");

    // A successful repair refreshes devices, which drops the synthetic target;
    // restore it and let the coalesced regeneration run.
    prepareRepairScope(window);
    window.scheduleEvidenceRefresh(QStringLiteral("UI test scoped refresh"));
    QTRY_VERIFY_WITH_TIMEOUT(!window.diagnosticsInvalidationPending(), 15000);

    QCOMPARE(diagnosticRunCount(window), 0);
    QVERIFY2(!window.m_targetDiagnosticCache.value(QStringLiteral("display")).trimmed().isEmpty(),
             "the display section must be regenerated");
    QVERIFY2(!window.m_targetDiagnosticCache.value(QStringLiteral("grub")).trimmed().isEmpty(),
             "unrelated cached sections must stay valid after the scoped refresh");
    const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QVERIFY2(log.contains(QStringLiteral("Starting read-only target diagnostic 'display'")),
             "the scoped refresh must run the display diagnostic");
    QVERIFY2(log.contains(QStringLiteral("Read-only target diagnostic 'display' completed.")),
             "the display diagnostic must complete");
    QVERIFY2(!log.contains(QStringLiteral("Starting all available read-only diagnostics")),
             "a scoped refresh must never run the complete report");
}

// A package transaction is broad: it regenerates the complete diagnostic set
// with one Run All, exactly like before the scoping change.
void MainWindowUiTest::packageStageRepairRegeneratesCompleteSet()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareRepairScope(window);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(true);
    cacheRepairEvidence(window, capabilityEvidence(false, true));

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("package-requests.log"));
    QVERIFY2(startRepairFakePrivilegedSession(window, capturePath),
             "the scripted privileged session must start");
    closeRepairProgressDialogWhenDone(&window);
    acceptNextMessageBox(&window, QMessageBox::Yes);

    QTreeWidgetItem *item = repairItem(window, QStringLiteral("dpkg"));
    QVERIFY(item);
    window.m_repairToolTree->setCurrentItem(item);
    window.runSelectedRepairTool();

    QVERIFY2(capturedHelperArguments(capturePath).contains(QStringLiteral("dpkg-configure")),
             "the package stage must be dispatched");
    QVERIFY2(window.m_targetDiagnosticsNeedRegeneration,
             "a package transaction must invalidate the complete target set");
    QVERIFY2(window.m_targetDiagnosticCache.value(QStringLiteral("report")).trimmed().isEmpty(),
             "the complete invalidation must drop the cached report");

    prepareRepairScope(window);
    window.scheduleEvidenceRefresh(QStringLiteral("UI test package refresh"));
    QTRY_VERIFY_WITH_TIMEOUT(!window.diagnosticsInvalidationPending(), 15000);

    QCOMPARE(diagnosticRunCount(window), 1);
    QVERIFY2(!window.m_targetDiagnosticCache.value(QStringLiteral("report")).trimmed().isEmpty(),
             "the complete set must be regenerated as one report");
    QVERIFY2(window.m_actionLogEntries.join(QLatin1Char('\n')).contains(
                 QStringLiteral("Starting all available read-only diagnostics (Target scope).")),
             "the package stage refresh must run the complete report");
}

// A Run Full Repair plan with two narrow stages accumulates their section
// union and regenerates it exactly once at the end; a mixed plan regenerates
// only the changed stage's mapped section.
void MainWindowUiTest::fullRepairPlanRegeneratesOnlyChangedSectionsOnce()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareRepairScope(window);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(true);
    cacheRepairEvidence(window, capabilityEvidenceWithDisplay(false, true));

    for (QCheckBox *toggle : {window.m_fullRepairFilesystem, window.m_fullRepairDpkg,
                              window.m_fullRepairBrokenPackages, window.m_fullRepairAptUpdate,
                              window.m_fullRepairUpgrade, window.m_fullRepairDkms,
                              window.m_fullRepairDisplayManager, window.m_fullRepairInitramfs,
                              window.m_fullRepairEfi, window.m_fullRepairGrub}) {
        QVERIFY(toggle);
        toggle->setChecked(false);
    }
    window.m_fullRepairDisplayManager->setChecked(true);
    window.m_fullRepairGrub->setChecked(true);
    window.updateFullRepairSummary();
    QCOMPARE(window.selectedRepairStages(),
             QStringList({QStringLiteral("display-manager"), QStringLiteral("grub")}));

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("plan-union-requests.log"));
    QVERIFY2(startRepairFakePrivilegedSession(window, capturePath),
             "the scripted privileged session must start");
    closeRepairProgressDialogWhenDone(&window);
    acceptNextMessageBox(&window, QMessageBox::Yes);

    window.runFullRepair();

    QVERIFY2(!window.m_fullRepairPlanInProgress,
             "the plan guard must be released after the last stage");
    QVERIFY2(window.m_fullRepairPlanModified,
             "the plan must remember that modifying stages ran");
    QVERIFY2(!window.m_targetDiagnosticsNeedRegeneration,
             "the plan's narrow stages must not invalidate the complete set");
    QVERIFY2(window.m_targetDiagnosticsStaleSections.contains(QStringLiteral("display")),
             "the plan union must contain the display section");
    QVERIFY2(window.m_targetDiagnosticsStaleSections.contains(QStringLiteral("grub")),
             "the plan union must contain the GRUB section");
    QCOMPARE(window.m_targetDiagnosticsStaleSections.size(), 2);
    QCOMPARE(window.m_actionLogEntries.join(QLatin1Char('\n')).count(QStringLiteral("STALE DIAGNOSTICS")), 1);
    QCOMPARE(diagnosticRunCount(window), 0);

    // The plan stages ran; the union regeneration has not run yet.
    const QStringList requests = capturedHelperArguments(capturePath);
    QVERIFY2(requests.contains(QStringLiteral("display-manager")) && requests.contains(QStringLiteral("grub")),
             "both plan stages must be dispatched in the single Full Repair request");

    prepareRepairScope(window);
    window.scheduleEvidenceRefresh(QStringLiteral("UI test plan union refresh"));
    QTRY_VERIFY_WITH_TIMEOUT(!window.diagnosticsInvalidationPending(), 15000);

    QCOMPARE(diagnosticRunCount(window), 0);
    const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QCOMPARE(log.count(QStringLiteral("Target diagnostic run: Graphical login / display manager")), 1);
    QCOMPARE(log.count(QStringLiteral("Target diagnostic run: GRUB configuration")), 1);
    QVERIFY2(!window.m_targetDiagnosticCache.value(QStringLiteral("display")).trimmed().isEmpty(),
             "the union refresh must regenerate display");
    QVERIFY2(!window.m_targetDiagnosticCache.value(QStringLiteral("grub")).trimmed().isEmpty(),
             "the union refresh must regenerate GRUB");

    // --- merged from fullRepairPlanRegeneratesOnlyChangedStages() ---
    {
        ScopedSessionLogDir logDir;
        QVERIFY(logDir.isValid());

        MainWindow window;
        window.show();
        QTest::qWait(50);
        prepareRepairScope(window);
        window.m_snapshotPreloadScheduled = true;
        window.m_autoRefreshDiagnostics->setChecked(true);
        cacheRepairEvidence(window, capabilityEvidenceWithDisplay(false, true));

        for (QCheckBox *toggle : {window.m_fullRepairFilesystem, window.m_fullRepairDpkg,
                                  window.m_fullRepairBrokenPackages, window.m_fullRepairAptUpdate,
                                  window.m_fullRepairUpgrade, window.m_fullRepairDkms,
                                  window.m_fullRepairDisplayManager, window.m_fullRepairInitramfs,
                                  window.m_fullRepairEfi, window.m_fullRepairGrub}) {
            QVERIFY(toggle);
            toggle->setChecked(false);
        }
        window.m_fullRepairDisplayManager->setChecked(true);
        window.m_fullRepairGrub->setChecked(true);
        window.updateFullRepairSummary();
        QCOMPARE(window.selectedRepairStages(),
                 QStringList({QStringLiteral("display-manager"), QStringLiteral("grub")}));

        QTemporaryDir requestDir;
        QVERIFY(requestDir.isValid());
        const QString capturePath = requestDir.filePath(QStringLiteral("plan-mixed-requests.log"));
        QVERIFY2(startChangeStatusFakePrivilegedSession(window, capturePath,
                                                        {QStringLiteral("display-manager=unchanged"),
                                                         QStringLiteral("grub=changed")}),
                 "the scripted privileged session must start");
        closeRepairProgressDialogWhenDone(&window);
        acceptNextMessageBox(&window, QMessageBox::Yes);

        window.runFullRepair();

        QVERIFY2(!window.m_fullRepairPlanInProgress,
                 "the plan guard must be released after the last stage");
        QVERIFY2(window.m_fullRepairPlanModified,
                 "a mixed plan must remember the changed stage");
        QVERIFY2(window.m_targetDiagnosticsStaleSections.contains(QStringLiteral("grub")),
                 "the changed GRUB stage must mark the grub section stale");
        QVERIFY2(!window.m_targetDiagnosticsStaleSections.contains(QStringLiteral("display")),
                 "the unchanged display stage must not mark its section stale");
        QCOMPARE(window.m_targetDiagnosticsStaleSections.size(), 1);
        QVERIFY2(!window.m_targetDiagnosticsNeedRegeneration,
                 "a mixed narrow plan must not invalidate the complete set");
        QVERIFY2(!window.m_targetDiagnosticCache.value(QStringLiteral("display")).trimmed().isEmpty(),
                 "the unchanged stage's cached evidence must stay valid");
        QVERIFY2(window.m_targetDiagnosticCache.value(QStringLiteral("grub")).trimmed().isEmpty(),
                 "the changed stage's cached section must be dropped");
        const QString planLog = window.m_actionLogEntries.join(QLatin1Char('\n'));
        QCOMPARE(planLog.count(QStringLiteral("STALE DIAGNOSTICS")), 1);
        QVERIFY2(planLog.contains(QStringLiteral(
                     "Full Repair results: ✓ 1 successful · ✗ 0 failed · ▪ 1 no repair needed")),
                 "the mixed plan aggregate must count each stage category");
        QVERIFY2(planLog.contains(QStringLiteral("  ✓ grub — GRUB configuration regenerated — changes were applied")),
                 "the changed stage must have an auditable summary line");
        QVERIFY2(planLog.contains(QStringLiteral("  ▪ display — display manager already correct — no changes")),
                 "the unchanged stage must have an auditable summary line");
        // The aggregate is the newest entry of the plan's log section, so it
        // renders above the captured Full Repair output.
        flushLogRefresh(window);
        const QString planVisible = window.m_logView->toPlainText();
        const int aggregateIndex = planVisible.indexOf(QStringLiteral("Full Repair results:"));
        const int planOutputIndex = planVisible.indexOf(
            QStringLiteral("Repair output\nDiagnostic: Full Repair"));
        QVERIFY2(aggregateIndex >= 0 && planOutputIndex >= 0 && aggregateIndex < planOutputIndex,
                 "the plan aggregate must render above the captured Full Repair output");

        // The plan's coalesced regeneration runs only the changed stage's section.
        prepareRepairScope(window);
        window.scheduleEvidenceRefresh(QStringLiteral("UI test mixed plan refresh"));
        QTRY_VERIFY_WITH_TIMEOUT(!window.diagnosticsInvalidationPending(), 15000);

        const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
        QCOMPARE(log.count(QStringLiteral("Target diagnostic run: GRUB configuration")), 1);
        QCOMPARE(log.count(QStringLiteral("Target diagnostic run: Graphical login / display manager")), 0);
        QVERIFY2(!window.m_targetDiagnosticCache.value(QStringLiteral("grub")).trimmed().isEmpty(),
                 "the changed stage's section must be regenerated");
        QVERIFY2(!window.m_targetDiagnosticCache.value(QStringLiteral("display")).trimmed().isEmpty(),
                 "the unchanged stage's cached evidence must survive the regeneration");
    }
}


// A repair whose helper status proves "unchanged" keeps every cached section
// valid, schedules no regeneration and leaves every other repair tool enabled;
// only its own repair log entry is added.
void MainWindowUiTest::repairUnchangedKeepsCachedEvidence()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareRepairScope(window);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(true);
    cacheRepairEvidence(window, capabilityEvidenceWithDisplay(false, true));

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("unchanged-requests.log"));
    QVERIFY2(startChangeStatusFakePrivilegedSession(window, capturePath,
                                                    {QStringLiteral("display-manager=unchanged")}),
             "the scripted privileged session must start");
    closeRepairProgressDialogWhenDone(&window);
    acceptNextMessageBox(&window, QMessageBox::Yes);

    QTreeWidgetItem *item = repairItem(window, QStringLiteral("display"));
    QVERIFY(item);
    window.m_repairToolTree->setCurrentItem(item);
    window.runSelectedRepairTool();

    QVERIFY2(capturedHelperArguments(capturePath).contains(QStringLiteral("display-manager")),
             "the display-manager repair must be dispatched");
    QVERIFY2(window.m_targetDiagnosticsStaleSections.isEmpty(),
             "an unchanged repair must not mark any section stale");
    QVERIFY2(!window.m_targetDiagnosticsNeedRegeneration,
             "an unchanged repair must not invalidate the complete set");
    QVERIFY2(!window.m_targetDiagnosticCache.value(QStringLiteral("display")).trimmed().isEmpty(),
             "the cached display evidence must stay valid");
    QVERIFY2(!window.m_targetDiagnosticCache.value(QStringLiteral("grub")).trimmed().isEmpty(),
             "unrelated cached sections must stay valid");

    const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QVERIFY2(log.contains(QStringLiteral("No system changes were detected; cached diagnostics remain valid.")),
             "the unchanged repair must log the no-change notice");
    QVERIFY2(log.contains(QStringLiteral("Repair result: ▪ display manager already correct — no changes")),
             "the unchanged repair must report the no-repair-needed category with the display vocabulary");
    QVERIFY2(!log.contains(QStringLiteral("Repair result: ✓")),
             "an unchanged repair must not be categorized as successful");
    QVERIFY2(!log.contains(QStringLiteral("Repair result: ✗")),
             "an unchanged repair must not be categorized as failed");
    QVERIFY2(!log.contains(QStringLiteral("STALE DIAGNOSTICS")),
             "an unchanged repair must not emit a stale notice");
    QVERIFY2(!window.m_evidenceRefreshTimer || !window.m_evidenceRefreshTimer->isActive(),
             "an unchanged repair must not schedule a regeneration");
    QVERIFY2(log.contains(QStringLiteral("Starting privileged action: Restore detected graphical login manager")),
             "the unchanged repair must still add its repair log entry");
    QVERIFY2(log.contains(QStringLiteral("REPAIR_OUTPUT captured")),
             "the unchanged repair must still capture the helper output");

    QString reason;
    QVERIFY2(window.repairToolAvailable(QStringLiteral("display"), &reason),
             qPrintable(QStringLiteral("the unchanged tool must stay enabled: %1").arg(reason)));
    QVERIFY2(window.repairToolAvailable(QStringLiteral("grub"), &reason),
             qPrintable(QStringLiteral("other repair tools must stay enabled: %1").arg(reason)));
    QCOMPARE(diagnosticRunCount(window), 0);
}

// A repair whose helper status reports "changed" invalidates exactly the
// mapped sections, like the pre-existing scoped invalidation; a transcript
// without the stable status line keeps the fail-safe invalidation.
void MainWindowUiTest::repairChangeStatusInvalidatesMappedSectionsOrFailsSafe()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareRepairScope(window);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);
    cacheRepairEvidence(window, capabilityEvidenceWithDisplay(false, true));

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("changed-requests.log"));
    QVERIFY2(startChangeStatusFakePrivilegedSession(window, capturePath,
                                                    {QStringLiteral("display-manager=changed")}),
             "the scripted privileged session must start");
    closeRepairProgressDialogWhenDone(&window);
    acceptNextMessageBox(&window, QMessageBox::Yes);

    QTreeWidgetItem *item = repairItem(window, QStringLiteral("display"));
    QVERIFY(item);
    window.m_repairToolTree->setCurrentItem(item);
    window.runSelectedRepairTool();

    QVERIFY2(window.m_targetDiagnosticsStaleSections.contains(QStringLiteral("display")),
             "a changed repair must mark its mapped display section stale");
    QCOMPARE(window.m_targetDiagnosticsStaleSections.size(), 1);
    QVERIFY2(!window.m_targetDiagnosticsNeedRegeneration,
             "a narrow changed repair must not invalidate the complete set");
    QVERIFY2(window.m_targetDiagnosticCache.value(QStringLiteral("display")).trimmed().isEmpty(),
             "the stale display section must be dropped from the cache");
    QVERIFY2(!window.m_targetDiagnosticCache.value(QStringLiteral("grub")).trimmed().isEmpty(),
             "unrelated cached sections must remain valid");
    const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QVERIFY2(log.contains(QStringLiteral("STALE DIAGNOSTICS")),
             "a changed repair must emit the stale notice");
    QVERIFY2(log.contains(QStringLiteral("Repair result: ✓ display manager repaired — changes were applied")),
             "a changed repair must report the success category");
    QVERIFY2(!log.contains(QStringLiteral("No system changes were detected")),
             "a changed repair must not claim that nothing changed");
    // The summary is the newest entry of the replaceable repair section, so it
    // renders above the captured helper output.
    flushLogRefresh(window);
    const QString visible = window.m_logView->toPlainText();
    const int summaryIndex = visible.indexOf(QStringLiteral("Repair result: ✓ display manager repaired"));
    const int outputIndex = visible.indexOf(
        QStringLiteral("Repair output\nDiagnostic: Restore detected graphical login manager"));
    QVERIFY2(summaryIndex >= 0 && outputIndex >= 0 && summaryIndex < outputIndex,
             "the repair summary must render above the captured output");

    // --- merged from missingRepairChangeStatusInvalidates() ---
    {
        ScopedSessionLogDir logDir;
        QVERIFY(logDir.isValid());

        MainWindow window;
        window.show();
        QTest::qWait(50);
        prepareRepairScope(window);
        window.m_snapshotPreloadScheduled = true;
        window.m_autoRefreshDiagnostics->setChecked(false);
        cacheRepairEvidence(window, capabilityEvidenceWithDisplay(false, true));

        QTemporaryDir requestDir;
        QVERIFY(requestDir.isValid());
        const QString capturePath = requestDir.filePath(QStringLiteral("missing-status-requests.log"));
        QVERIFY2(startChangeStatusFakePrivilegedSession(window, capturePath, QStringList()),
                 "the scripted privileged session must start");
        closeRepairProgressDialogWhenDone(&window);
        acceptNextMessageBox(&window, QMessageBox::Yes);

        QTreeWidgetItem *item = repairItem(window, QStringLiteral("display"));
        QVERIFY(item);
        window.m_repairToolTree->setCurrentItem(item);
        window.runSelectedRepairTool();

        QVERIFY2(window.m_targetDiagnosticsStaleSections.contains(QStringLiteral("display")),
                 "a missing change status must fail safe and invalidate the mapped section");
        QVERIFY2(window.m_targetDiagnosticCache.value(QStringLiteral("display")).trimmed().isEmpty(),
                 "the fail-safe invalidation must drop the cached section");
        const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
        QVERIFY2(!log.contains(QStringLiteral("No system changes were detected")),
                 "a missing status must never be reported as unchanged");
        QVERIFY2(log.contains(QStringLiteral("Repair result: ✓ display manager repaired — changes were applied")),
                 "a missing status keeps the fail-safe success category, matching the invalidation decision");
    }
}

// A failed modifying repair reports the failed category with the helper's
// short reason and keeps the fail-safe complete invalidation. A change-status
// line in a failed transcript must never turn the failure into a success.
void MainWindowUiTest::repairFailedResultSummaryAndInvalidation()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareRepairScope(window);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);
    cacheRepairEvidence(window, capabilityEvidenceWithDisplay(false, true));

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("failed-repair-requests.log"));
    QVERIFY2(startChangeStatusFakePrivilegedSession(window, capturePath,
                                                    {QStringLiteral("display-manager=changed")},
                                                    /*exitCode=*/1),
             "the scripted privileged session must start");
    closeRepairProgressDialogWhenDone(&window);
    acceptConfirmationThenFailure(&window);

    QTreeWidgetItem *item = repairItem(window, QStringLiteral("display"));
    QVERIFY(item);
    window.m_repairToolTree->setCurrentItem(item);
    window.runSelectedRepairTool();

    const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QVERIFY2(log.contains(QStringLiteral("Repair result: ✗ display manager repair failed — the privileged helper reported a failure")),
             "a non-zero exit must report the failed category and its short reason");
    QVERIFY2(!log.contains(QStringLiteral("Repair result: ✓")),
             "a non-zero exit must never be categorized as successful");
    QVERIFY2(!log.contains(QStringLiteral("Repair result: ▪")),
             "a non-zero exit must never be categorized as no-repair-needed");
    QVERIFY2(window.m_targetDiagnosticsNeedRegeneration,
             "a failed modifying repair must invalidate the complete target set (fail safe)");
    QVERIFY2(window.m_targetDiagnosticCache.value(QStringLiteral("display")).trimmed().isEmpty(),
             "the failed repair must drop the mapped cached section");
    QVERIFY2(log.contains(QStringLiteral("STALE DIAGNOSTICS")),
             "a failed repair must emit the stale notice");
}

// The three repair-result symbols render in color while the document stays
// plain text. Both the full-rebuild and the incremental-prepend paths must
// apply the character formats, and the plain-text content must be unchanged.
void MainWindowUiTest::repairResultSummarySymbolsAreColored()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    window.appendLog(QStringLiteral("Repair result: ✓ repair successful — changes were applied"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    flushLogRefresh(window);
    window.appendLog(QStringLiteral("Repair result: ✗ repair failed — helper exit 1"),
                     QStringLiteral("ERROR"), MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral("Repair result: ▪ no repair needed — no changes were detected"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral(
                         "Full Repair results: ✓ 1 successful · ✗ 1 failed · ▪ 1 no repair needed\n"
                         "  ✓ grub — repair successful — changes were applied\n"
                         "  ✗ dkms — repair failed — helper exit 1\n"
                         "  ▪ display — no repair needed — no changes were detected"),
                     QStringLiteral("ERROR"), MainWindow::LogEntryKind::Repair);
    flushLogRefresh(window);

    const QHash<QString, QColor> expected = {
        {QStringLiteral("✓"), QColor(0x2e, 0xa0, 0x43)},
        {QStringLiteral("✗"), QColor(0xc0, 0x39, 0x2b)},
        {QStringLiteral("▪"), QColor(0x1f, 0x6f, 0xd6)}
    };
    QHash<QString, int> coloredCounts;
    const QTextDocument *document = window.m_logView->document();
    for (QTextBlock block = document->begin(); block.isValid(); block = block.next()) {
        if (!block.text().contains(QStringLiteral("Repair result:"))
            && !block.text().contains(QStringLiteral("Full Repair results:"))
            && !block.text().startsWith(QStringLiteral("  ✓"))
            && !block.text().startsWith(QStringLiteral("  ✗"))
            && !block.text().startsWith(QStringLiteral("  ▪"))) {
            continue;
        }
        for (QTextBlock::iterator it = block.begin(); !it.atEnd(); ++it) {
            const QTextFragment fragment = it.fragment();
            for (auto itExpected = expected.constBegin(); itExpected != expected.constEnd(); ++itExpected) {
                if (!fragment.text().contains(itExpected.key())) {
                    continue;
                }
                QCOMPARE(fragment.charFormat().foreground().color(), itExpected.value());
                coloredCounts[itExpected.key()] += 1;
            }
        }
    }
    QCOMPARE(coloredCounts.value(QStringLiteral("✓")), 3);
    QCOMPARE(coloredCounts.value(QStringLiteral("✗")), 3);
    QCOMPARE(coloredCounts.value(QStringLiteral("▪")), 3);

    // The document stays plain text: search, copy and Save As see the symbols
    // with no markup, and the session file remains plain as well.
    const QString visible = window.m_logView->toPlainText();
    QVERIFY(visible.contains(QStringLiteral("Repair result: ✓ repair successful — changes were applied")));
    QVERIFY(visible.contains(QStringLiteral("Full Repair results: ✓ 1 successful · ✗ 1 failed · ▪ 1 no repair needed")));
    QVERIFY(!visible.contains(QStringLiteral("<span")));
}

// The rendered repair-result glyphs must be visibly colored. The test grabs the
// log viewport and checks the painted pixels, not only the document formats, so
// a glyph that falls back to the normal text color fails even though its
// QTextCharFormat may be correct.
void MainWindowUiTest::repairResultSummaryGlyphsRenderColored()
{
    ScopedDarkPalette darkPalette;
    MainWindow window;
    window.show();
    window.resize(1000, 800);
    QTest::qWait(50);
    window.m_tabs->setCurrentIndex(6);
    QTest::qWait(50);

    window.appendLog(QStringLiteral("Repair result: ✓ repair successful — changes were applied"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral("Repair result: ✗ repair failed — helper exit 1"),
                     QStringLiteral("ERROR"), MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral("Repair result: ▪ no repair needed — no changes were detected"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    window.appendLog(QStringLiteral(
                         "Full Repair results: ✓ 1 successful · ✗ 1 failed · ▪ 1 no repair needed\n"
                         "  ✓ grub — repair successful — changes were applied\n"
                         "  ✗ dkms — repair failed — helper exit 1\n"
                         "  ▪ display — no repair needed — no changes were detected"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    flushLogRefresh(window);
    QTest::qWait(50);

    const QHash<QString, int> colored = coloredGlyphCounts(window);
    QCOMPARE(colored.value(QStringLiteral("✓")), 3);
    QCOMPARE(colored.value(QStringLiteral("✗")), 3);
    QCOMPARE(colored.value(QStringLiteral("▪")), 3);

    // The document stays plain text: search, copy and Save As see the symbols
    // with no markup.
    const QString visible = window.m_logView->toPlainText();
    QVERIFY(visible.contains(QStringLiteral(
        "Full Repair results: ✓ 1 successful · ✗ 1 failed · ▪ 1 no repair needed")));
    QVERIFY(!visible.contains(QStringLiteral("<span")));
}

// A filter rebuild rewrites the whole document and an incremental append
// inserts at the top; both must keep the glyph pixels colored.
void MainWindowUiTest::repairResultSummaryColorsSurviveFilterAndAppend()
{
    MainWindow window;
    window.show();
    window.resize(1000, 800);
    QTest::qWait(50);
    window.m_tabs->setCurrentIndex(6);
    QTest::qWait(50);

    window.appendLog(QStringLiteral(
                         "Full Repair results: ✓ 1 successful · ✗ 1 failed · ▪ 1 no repair needed\n"
                         "  ✓ grub — repair successful — changes were applied\n"
                         "  ✗ dkms — repair failed — helper exit 1\n"
                         "  ▪ display — no repair needed — no changes were detected"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    flushLogRefresh(window);
    QTest::qWait(50);
    QHash<QString, int> colored = coloredGlyphCounts(window);
    QCOMPARE(colored.value(QStringLiteral("✓")), 2);
    QCOMPARE(colored.value(QStringLiteral("✗")), 2);
    QCOMPARE(colored.value(QStringLiteral("▪")), 2);

    // Filter rebuild: the rendered document is rewritten from the filtered
    // entry list.
    window.m_logFilterCombo->setCurrentIndex(2);
    QTest::qWait(50);
    QVERIFY(window.m_logView->toPlainText().contains(QStringLiteral("Full Repair results:")));
    colored = coloredGlyphCounts(window);
    QCOMPARE(colored.value(QStringLiteral("✓")), 2);
    QCOMPARE(colored.value(QStringLiteral("✗")), 2);
    QCOMPARE(colored.value(QStringLiteral("▪")), 2);

    // Incremental append after the filter is cleared: the new entry is
    // prepended without a full rebuild.
    window.m_logFilterCombo->setCurrentIndex(0);
    QTest::qWait(50);
    window.appendLog(QStringLiteral("Repair result: ✓ repair successful — second pass"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    flushLogRefresh(window);
    QTest::qWait(50);
    colored = coloredGlyphCounts(window);
    QCOMPARE(colored.value(QStringLiteral("✓")), 3);
    QCOMPARE(colored.value(QStringLiteral("✗")), 2);
    QCOMPARE(colored.value(QStringLiteral("▪")), 2);
}

// A repair section replaced by a newer run and a prior session log populated
// into the view are separate rendering paths; both must keep the glyph pixels
// colored and must drop the replaced block's colors with the block itself.
void MainWindowUiTest::repairResultSummaryColorsSurviveSectionReplacementAndPriorLog()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    window.resize(1000, 800);
    QTest::qWait(50);
    window.m_tabs->setCurrentIndex(6);
    QTest::qWait(50);

    window.beginRepairLogSection(QStringLiteral("color-test-section"));
    window.appendLog(QStringLiteral(
                         "Full Repair results: ✓ 1 successful · ✗ 0 failed · ▪ 1 no repair needed\n"
                         "  ✓ grub — repair successful — changes were applied\n"
                         "  ▪ display — no repair needed — no changes were detected"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    window.finishRepairLogSection(QStringLiteral("color-test-section"));
    flushLogRefresh(window);
    QTest::qWait(50);
    QHash<QString, int> colored = coloredGlyphCounts(window);
    QCOMPARE(colored.value(QStringLiteral("✓")), 2);
    QCOMPARE(colored.value(QStringLiteral("✗")), 1);
    QCOMPARE(colored.value(QStringLiteral("▪")), 2);

    // Replace the section with a newer run: the previous block is removed
    // together with its colors and the replacement block is colored.
    window.beginRepairLogSection(QStringLiteral("color-test-section"));
    window.appendLog(QStringLiteral(
                         "Full Repair results: ✓ 0 successful · ✗ 1 failed · ▪ 0 no repair needed\n"
                         "  ✗ grub — repair failed — helper exit 1"),
                     QStringLiteral("ERROR"), MainWindow::LogEntryKind::Repair);
    window.finishRepairLogSection(QStringLiteral("color-test-section"));
    flushLogRefresh(window);
    QTest::qWait(50);
    colored = coloredGlyphCounts(window);
    QCOMPARE(colored.value(QStringLiteral("✓")), 1);
    QCOMPARE(colored.value(QStringLiteral("✗")), 2);
    QCOMPARE(colored.value(QStringLiteral("▪")), 1);

    // A prior session log rendered into the same view must be colored too.
    const QString path = logDir.path() + QStringLiteral("/session-20260101-000000.log");
    QFile file(path);
    QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
    QTextStream(&file) << QStringLiteral(
        "──────── REPAIR ────────\n[2026-01-01 00:00:00] [INFO] "
        "Full Repair results: ✓ 1 successful · ✗ 0 failed · ▪ 1 no repair needed\n"
        "  ✓ grub — repair successful — changes were applied\n"
        "  ▪ display — no repair needed — no changes were detected\n");
    file.close();
    window.displaySessionLog(path);
    QTest::qWait(50);
    colored = coloredGlyphCounts(window);
    QCOMPARE(colored.value(QStringLiteral("✓")), 2);
    QCOMPARE(colored.value(QStringLiteral("✗")), 1);
    QCOMPARE(colored.value(QStringLiteral("▪")), 2);
}

// Every tool has its own action-accurate phrase: a metadata refresh is not a
// repair, an initramfs regeneration is a rebuild, and a skipped filesystem is
// counted as not checked instead of no-repair-needed. The same vocabulary
// table drives the single-action summary and the plan stage lines.
void MainWindowUiTest::repairPlanSummaryVocabularyIsActionAccurate()
{
    using Category = MainWindow::RepairResultCategory;
    const auto stage = [](const QString &toolKey, Category category,
                          const QString &reason = QString(), const QString &detail = QString()) {
        MainWindow::RepairStageResult result;
        result.toolKey = toolKey;
        result.category = category;
        result.reason = reason;
        result.detail = detail;
        return result;
    };

    // Changed and unchanged phrases per tool.
    QCOMPARE(MainWindow::repairResultStageLine(stage(QStringLiteral("aptupdate"), Category::Success)),
             QStringLiteral("  ✓ aptupdate — package metadata refreshed — changes were applied"));
    QCOMPARE(MainWindow::repairResultStageLine(stage(QStringLiteral("aptupdate"), Category::NoRepairNeeded)),
             QStringLiteral("  ▪ aptupdate — package metadata already current — no changes"));
    QCOMPARE(MainWindow::repairResultStageLine(stage(QStringLiteral("initramfs"), Category::Success)),
             QStringLiteral("  ✓ initramfs — initramfs rebuilt — images regenerated"));
    QCOMPARE(MainWindow::repairResultStageLine(stage(QStringLiteral("initramfs"), Category::NoRepairNeeded)),
             QStringLiteral("  ▪ initramfs — initramfs already current — no changes"));
    QCOMPARE(MainWindow::repairResultStageLine(stage(QStringLiteral("upgrade"), Category::Success)),
             QStringLiteral("  ✓ upgrade — packages upgraded — changes were applied"));
    QCOMPARE(MainWindow::repairResultStageLine(stage(QStringLiteral("upgrade"), Category::NoRepairNeeded)),
             QStringLiteral("  ▪ upgrade — no packages to upgrade"));
    // Held-back/skipped package-manager feedback is part of the summary
    // wording for a changed stage as well as a proven-unchanged one.
    QCOMPARE(MainWindow::repairResultStageLine(stage(
                 QStringLiteral("upgrade"), Category::Success, QString(),
                 QStringLiteral("1 package kept back: firmware-mediatek"))),
             QStringLiteral("  ✓ upgrade — packages upgraded — changes were applied — "
                            "1 package kept back: firmware-mediatek"));
    QCOMPARE(MainWindow::repairResultStageLine(stage(
                 QStringLiteral("upgrade"), Category::NoRepairNeeded, QString(),
                 QStringLiteral("1 package kept back: firmware-mediatek"))),
             QStringLiteral("  ▪ upgrade — no packages to upgrade — "
                            "1 package kept back: firmware-mediatek"));
    QCOMPARE(MainWindow::repairResultStageLine(stage(QStringLiteral("fixbroken"), Category::Success)),
             QStringLiteral("  ✓ fixbroken — broken dependencies repaired — changes were applied"));
    QCOMPARE(MainWindow::repairResultStageLine(stage(QStringLiteral("fixbroken"), Category::NoRepairNeeded)),
             QStringLiteral("  ▪ fixbroken — no broken dependencies"));
    QCOMPARE(MainWindow::repairResultStageLine(stage(QStringLiteral("dpkg"), Category::Success)),
             QStringLiteral("  ✓ dpkg — package configuration completed — changes were applied"));
    QCOMPARE(MainWindow::repairResultStageLine(stage(QStringLiteral("dpkg"), Category::NoRepairNeeded)),
             QStringLiteral("  ▪ dpkg — nothing to configure"));
    QCOMPARE(MainWindow::repairResultStageLine(stage(QStringLiteral("display"), Category::Success)),
             QStringLiteral("  ✓ display — display manager repaired — changes were applied"));
    QCOMPARE(MainWindow::repairResultStageLine(stage(QStringLiteral("display"), Category::NoRepairNeeded)),
             QStringLiteral("  ▪ display — display manager already correct — no changes"));
    QCOMPARE(MainWindow::repairResultStageLine(stage(QStringLiteral("efi"), Category::Success)),
             QStringLiteral("  ✓ efi — EFI boot path repaired — changes were applied"));
    QCOMPARE(MainWindow::repairResultStageLine(stage(QStringLiteral("efi"), Category::NoRepairNeeded)),
             QStringLiteral("  ▪ efi — EFI boot path already correct — no changes"));
    QCOMPARE(MainWindow::repairResultStageLine(stage(QStringLiteral("grub"), Category::Success)),
             QStringLiteral("  ✓ grub — GRUB configuration regenerated — changes were applied"));
    QCOMPARE(MainWindow::repairResultStageLine(stage(QStringLiteral("grub"), Category::NoRepairNeeded)),
             QStringLiteral("  ▪ grub — GRUB configuration already current — no changes"));
    QCOMPARE(MainWindow::repairResultStageLine(stage(QStringLiteral("extlinux"), Category::Success)),
             QStringLiteral("  ✓ extlinux — extlinux configuration regenerated — changes were applied"));
    QCOMPARE(MainWindow::repairResultStageLine(stage(QStringLiteral("extlinux"), Category::NoRepairNeeded)),
             QStringLiteral("  ▪ extlinux — extlinux configuration already current — no changes"));
    QCOMPARE(MainWindow::repairResultStageLine(stage(QStringLiteral("dkms"), Category::Success)),
             QStringLiteral("  ✓ dkms — DKMS modules rebuilt — changes were applied"));
    QCOMPARE(MainWindow::repairResultStageLine(stage(QStringLiteral("dkms"), Category::NoRepairNeeded)),
             QStringLiteral("  ▪ dkms — DKMS modules already current — no changes"));

    // The filesystem stage distinguishes clean, skipped and unavailable
    // evidence; skipped filesystems are never folded into no-repair-needed.
    QCOMPARE(MainWindow::repairResultStageLine(stage(QStringLiteral("filesystem"), Category::NoRepairNeeded,
                                                    QString(), QStringLiteral("2 checked clean"))),
             QStringLiteral("  ▪ filesystem — no file system errors found — no changes — 2 checked clean"));
    QCOMPARE(MainWindow::repairResultStageLine(stage(
                 QStringLiteral("filesystem"), Category::Skipped, QString(),
                 QStringLiteral("1 skipped (mounted, offline-only check): /dev/dm-4 btrfs"))),
             QStringLiteral("  ▪ filesystem — not all filesystems were checked — "
                            "1 skipped (mounted, offline-only check): /dev/dm-4 btrfs"));
    QCOMPARE(MainWindow::repairResultStageLine(stage(
                 QStringLiteral("filesystem"), Category::Skipped, QString(),
                 QStringLiteral("1 not checked (unsupported or no installed check tool): /dev/sda1 xfs"))),
             QStringLiteral("  ▪ filesystem — not all filesystems were checked — "
                            "1 not checked (unsupported or no installed check tool): /dev/sda1 xfs"));

    // Helper-proven unchanged reasons and failure reasons stay auditable.
    QCOMPARE(MainWindow::repairResultStageLine(stage(QStringLiteral("grub"), Category::NoRepairNeeded,
                                                     QString(),
                                                     QStringLiteral("grub.cfg is byte-identical"))),
             QStringLiteral("  ▪ grub — GRUB configuration already current — no changes — grub.cfg is byte-identical"));
    QCOMPARE(MainWindow::repairResultStageLine(stage(QStringLiteral("aptupdate"), Category::Failed,
                                                     QStringLiteral("helper exit 1"))),
             QStringLiteral("  ✗ aptupdate — package metadata refresh failed — helper exit 1"));

    // A stage the plan never reached is reported as not run, never as failed
    // and never as a check that ran and skipped.
    QCOMPARE(MainWindow::repairResultStageLine(stage(QStringLiteral("grub"), Category::NotRun)),
             QStringLiteral("  ▪ grub — GRUB regeneration did not run"));
    QCOMPARE(MainWindow::repairResultStageLine(stage(QStringLiteral("efi"), Category::NotRun)),
             QStringLiteral("  ▪ efi — EFI boot path repair did not run"));
    QCOMPARE(MainWindow::repairResultSummary(Category::NotRun, QStringLiteral("efi")),
             QStringLiteral("Repair result: ▪ EFI boot path repair did not run"));

    // The aggregate counts the skipped stage in its own category.
    const QList<MainWindow::RepairStageResult> stages = {
        stage(QStringLiteral("filesystem"), Category::Skipped,
              QString(), QStringLiteral("1 skipped (mounted, offline-only check): /dev/dm-4 btrfs")),
        stage(QStringLiteral("dpkg"), Category::NoRepairNeeded),
        stage(QStringLiteral("fixbroken"), Category::NoRepairNeeded),
        stage(QStringLiteral("aptupdate"), Category::Success),
        stage(QStringLiteral("upgrade"), Category::NoRepairNeeded),
        stage(QStringLiteral("dkms"), Category::Success),
        stage(QStringLiteral("display"), Category::NoRepairNeeded),
        stage(QStringLiteral("initramfs"), Category::Success),
        stage(QStringLiteral("efi"), Category::NoRepairNeeded),
        stage(QStringLiteral("grub"), Category::NoRepairNeeded)
    };
    const QString summary = MainWindow::fullRepairPlanSummary(stages);
    const QStringList lines = summary.split(QLatin1Char('\n'));
    QCOMPARE(lines.first(),
             QStringLiteral("Full Repair results: ✓ 3 successful · ✗ 0 failed · "
                            "▪ 6 no repair needed · ▪ 1 not checked — see the filesystem stage line — changes were applied."));
    QVERIFY2(!summary.contains(QStringLiteral("▪ 7 no repair needed")),
             "a skipped filesystem must not be counted as no-repair-needed");
    QCOMPARE(lines.size(), stages.size() + 1);
    // The aggregate must reference the skipped stage by name and the stage
    // line must carry the affected device, so the two read together.
    QVERIFY2(lines.first().contains(QStringLiteral("see the filesystem stage line")),
             "the aggregate must point at the skipped filesystem stage");
    QVERIFY2(lines.at(1).contains(QStringLiteral("1 skipped (mounted, offline-only check): /dev/dm-4 btrfs")),
             "the referenced stage line must list the skipped device");

    // A plan without skipped stages keeps the established aggregate shape and
    // suffix.
    QCOMPARE(MainWindow::fullRepairPlanSummary({stage(QStringLiteral("grub"), Category::Success)})
                 .split(QLatin1Char('\n')).first(),
             QStringLiteral("Full Repair results: ✓ 1 successful · ✗ 0 failed · "
                            "▪ 0 no repair needed — changes were applied."));

    // A failed plan keeps each completed stage's own result: only the stage
    // the helper named is failed and stages the plan never reached are
    // counted, and named, as not run.
    const QList<MainWindow::RepairStageResult> failedStages = {
        stage(QStringLiteral("fixbroken"), Category::Success),
        stage(QStringLiteral("upgrade"), Category::Success),
        stage(QStringLiteral("display"), Category::NoRepairNeeded),
        stage(QStringLiteral("initramfs"), Category::NoRepairNeeded),
        stage(QStringLiteral("efi"), Category::Failed,
              QStringLiteral("Unable to restore the reconciled EFI BootOrder safely")),
        stage(QStringLiteral("grub"), Category::NotRun)
    };
    const QString failedSummary = MainWindow::fullRepairPlanSummary(failedStages);
    const QStringList failedLines = failedSummary.split(QLatin1Char('\n'));
    QCOMPARE(failedLines.first(),
             QStringLiteral("Full Repair results: ✓ 2 successful · ✗ 1 failed · "
                            "▪ 2 no repair needed · ▪ 1 not run — see the grub stage line — "
                            "review the failed stages in Logs."));
    QVERIFY2(!failedSummary.contains(QStringLiteral("✗ grub")),
             "a stage the plan never reached must not be reported as failed");
    QVERIFY2(failedSummary.contains(QStringLiteral("  ▪ grub — GRUB regeneration did not run")),
             "the not-run stage must say it did not run");
    QVERIFY2(failedSummary.contains(
                 QStringLiteral("  ✗ efi — EFI boot path repair failed — "
                                "Unable to restore the reconciled EFI BootOrder safely")),
             "only the named failing stage carries the failure reason");

    // The real failed run's shape: a skipped file-system pre-stage, two
    // changed stages, two unchanged stages, the failed EFI stage and the
    // never-reached GRUB stage. Each category is counted and named separately.
    const QList<MainWindow::RepairStageResult> realFailedRun = {
        stage(QStringLiteral("filesystem"), Category::Skipped,
              QString(), QStringLiteral("1 not checked (unsupported or no installed check tool): /dev/vdb1 vfat")),
        stage(QStringLiteral("fixbroken"), Category::Success),
        stage(QStringLiteral("upgrade"), Category::Success),
        stage(QStringLiteral("display"), Category::NoRepairNeeded),
        stage(QStringLiteral("initramfs"), Category::NoRepairNeeded),
        stage(QStringLiteral("efi"), Category::Failed,
              QStringLiteral("Unable to restore the reconciled EFI BootOrder safely after conventional GRUB EFI install.")),
        stage(QStringLiteral("grub"), Category::NotRun)
    };
    QCOMPARE(MainWindow::fullRepairPlanSummary(realFailedRun).split(QLatin1Char('\n')).first(),
             QStringLiteral("Full Repair results: ✓ 2 successful · ✗ 1 failed · "
                            "▪ 2 no repair needed · ▪ 1 not checked — see the filesystem stage line · "
                            "▪ 1 not run — see the grub stage line — review the failed stages in Logs."));

    // Single-action summaries use the same vocabulary table and carry the
    // same held-back/skipped feedback detail.
    QCOMPARE(MainWindow::repairResultSummary(Category::Success, QStringLiteral("upgrade"), QString(),
                                             QStringLiteral("1 package kept back: firmware-mediatek")),
             QStringLiteral("Repair result: ✓ packages upgraded — changes were applied — "
                            "1 package kept back: firmware-mediatek"));
    QCOMPARE(MainWindow::repairResultSummary(Category::Success, QStringLiteral("aptupdate")),
             QStringLiteral("Repair result: ✓ package metadata refreshed — changes were applied"));
    QCOMPARE(MainWindow::repairResultSummary(Category::NoRepairNeeded, QStringLiteral("initramfs")),
             QStringLiteral("Repair result: ▪ initramfs already current — no changes"));
    QCOMPARE(MainWindow::repairResultSummary(Category::Failed, QStringLiteral("fixbroken"),
                                             QStringLiteral("helper exit 1")),
             QStringLiteral("Repair result: ✗ broken dependency repair failed — helper exit 1"));
    QCOMPARE(MainWindow::repairResultSummary(Category::Skipped, QStringLiteral("filesystem")),
             QStringLiteral("Repair result: ▪ not all filesystems were checked"));
    QCOMPARE(MainWindow::repairResultSummary(Category::NoRepairNeeded),
             QStringLiteral("Repair result: ▪ no repair needed — no changes were detected"));
}

// A package upgrade whose helper transcript appends held-back/skipped feedback
// to its change status must show the count and names in the result summary.
// The helper reports `changed|1 package kept back: <pkg>` when other packages
// were upgraded; the GUI must not hide that feedback behind the bare changed
// category.
void MainWindowUiTest::repairResultSummaryShowsHeldBackPackages()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareRepairScope(window);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(true);
    cacheRepairEvidence(window, capabilityEvidence(false, true));

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("upgrade-feedback-requests.log"));
    QVERIFY2(startChangeStatusFakePrivilegedSession(
                 window, capturePath,
                 {QStringLiteral("apt-upgrade=changed|1 package kept back: firmware-mediatek")}),
             "the scripted privileged session must start");
    closeRepairProgressDialogWhenDone(&window);
    acceptNextMessageBox(&window, QMessageBox::Yes);

    QTreeWidgetItem *item = repairItem(window, QStringLiteral("upgrade"));
    QVERIFY(item);
    window.m_repairToolTree->setCurrentItem(item);
    window.runSelectedRepairTool();

    QVERIFY2(capturedHelperArguments(capturePath).contains(QStringLiteral("apt-upgrade")),
             "the upgrade repair must be dispatched");
    const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QVERIFY2(log.contains(QStringLiteral(
                 "Repair result: ✓ packages upgraded — changes were applied — "
                 "1 package kept back: firmware-mediatek")),
             "the changed upgrade result summary must name the held-back package");
    QVERIFY2(!log.contains(QStringLiteral("Repair result: ✗")),
             "held-back packages must never be reported as a failure");
}

// A Full Repair run must read chronologically: the file system pre-stage runs
// and is logged before the helper plan, the aggregate summary is the newest
// entry of the plan section and includes the pre-stage, and the post-plan
// notice follows the plan. The register is newest-first, so the summary must
// render above its repair output block, which renders above the plan start,
// which renders above the pre-stage.
void MainWindowUiTest::fullRepairFilesystemPreStageIsOrderedBeforePlanSummary()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareFilesystemPlan(window);
    QVERIFY(window.m_fullRepairDpkg);
    window.m_fullRepairDpkg->setChecked(true);
    window.updateFullRepairSummary();
    QCOMPARE(window.selectedRepairStages(),
             QStringList({QStringLiteral("filesystem"), QStringLiteral("dpkg-configure")}));

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("plan-fs-order-requests.log"));
    QProcess *session = startFilesystemFakePrivilegedSession(window, capturePath,
                                                             QStringLiteral("ext4"), true,
                                                             QStringLiteral("changed"));
    QVERIFY2(session, "the scripted privileged session must start");
    QVERIFY(window.m_privilegedSessionReady);

    // Drive the custom file system repair dialog; the plan confirmation and
    // the per-device repair confirmation are standard message boxes.
    QTimer poll;
    poll.setInterval(5);
    QObject::connect(&poll, &QTimer::timeout, &window, [] {
        for (QWidget *top : QApplication::topLevelWidgets()) {
            auto *dialog = qobject_cast<QDialog *>(top);
            if (!dialog || !dialog->isVisible()
                || dialog->windowTitle() != QStringLiteral("Repair file system errors")) {
                continue;
            }
            auto *table = dialog->findChild<QTableWidget *>(QStringLiteral("filesystemRepairTable"));
            if (!table || table->rowCount() < 1) {
                continue;
            }
            for (QPushButton *button : dialog->findChildren<QPushButton *>()) {
                if (button->text() == QStringLiteral("Run Selected Repairs")) {
                    button->click();
                    return;
                }
            }
            dialog->accept();
            return;
        }
    });
    poll.start();
    closeRepairProgressDialogWhenDone(&window);
    acceptNextMessageBoxes(&window, 2, QMessageBox::Yes);

    window.runFullRepair();
    poll.stop();

    QVERIFY2(!window.m_fullRepairPlanInProgress, "the plan must finish");
    QVERIFY2(window.m_fullRepairPlanModified, "the changed plan must invalidate");
    QCOMPARE(diagnosticRunCount(window), 0);

    // The helper request order proves the pre-stage runs before the plan.
    const QList<QStringList> requests = capturedHelperRequests(capturePath);
    QCOMPARE(requests.size(), 3);
    QVERIFY2(requests.at(0).contains(QStringLiteral("fs-inspect")),
             "the pre-stage inspection must run first");
    QVERIFY2(requests.at(1).contains(QStringLiteral("fs-repair")),
             "the confirmed repair must run second");
    QVERIFY2(requests.at(2).contains(QStringLiteral("dpkg-configure")),
             "the helper plan must run after the pre-stage");

    const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
    const int summaryIndex = log.indexOf(QStringLiteral("Full Repair results:"));
    const int planOutputIndex = log.indexOf(QStringLiteral("Repair output\nDiagnostic: Full Repair"));
    const int planStartIndex = log.indexOf(QStringLiteral("Starting privileged action: Full Repair"));
    const int fsOutputIndex = log.indexOf(QStringLiteral("File system check output"));
    const int fsStartIndex = log.indexOf(
        QStringLiteral("Starting read-only file system check (Full Repair pre-stage)"));
    QVERIFY2(summaryIndex >= 0 && planOutputIndex >= 0 && planStartIndex >= 0
                 && fsOutputIndex >= 0 && fsStartIndex >= 0,
             "the plan summary, plan block and pre-stage block must all be logged");
    QVERIFY2(summaryIndex < planOutputIndex && planOutputIndex < planStartIndex,
             "the plan summary must be newest, above its repair output and plan start");
    QVERIFY2(planStartIndex < fsOutputIndex && fsOutputIndex < fsStartIndex,
             "the file system pre-stage must be older than the helper plan");
    QVERIFY2(log.contains(QStringLiteral(
                 "Full Repair results: ✓ 2 successful · ✗ 0 failed · ▪ 0 no repair needed")),
             "the aggregate must include the file system pre-stage");
    QVERIFY2(log.contains(QStringLiteral(
                 "  ✓ filesystem — file system repaired — changes were applied — 1 file system(s) with issues")),
             "the pre-stage must be auditable in the summary");
    QVERIFY2(log.contains(QStringLiteral("  ✓ dpkg — package configuration completed — changes were applied")),
             "the plan stage must be auditable in the summary");
    QVERIFY2(!log.contains(QStringLiteral("No system changes were detected; cached diagnostics remain valid.")),
             "a changing plan must not leave an intermediate unchanged notice");
    QVERIFY2(log.contains(QStringLiteral("STALE DIAGNOSTICS")),
             "the post-plan notice must follow the aggregate");

    // The same order is visible in the rendered newest-first view.
    flushLogRefresh(window);
    const QString visible = window.m_logView->toPlainText();
    const int visibleSummary = visible.indexOf(QStringLiteral("Full Repair results:"));
    const int visiblePlanOutput = visible.indexOf(QStringLiteral("Repair output\nDiagnostic: Full Repair"));
    const int visibleFsStart = visible.indexOf(
        QStringLiteral("Starting read-only file system check (Full Repair pre-stage)"));
    QVERIFY2(visibleSummary >= 0 && visiblePlanOutput >= 0 && visibleFsStart >= 0
                 && visibleSummary < visiblePlanOutput && visiblePlanOutput < visibleFsStart,
             "the rendered view must show the summary above the plan output above the pre-stage");

    // A post-repair diagnostic entry is newer than the plan, so it must render
    // above the aggregate summary while the summary stays above its output.
    window.appendDiagnosticLog(QStringLiteral("grub"), QStringLiteral("GRUB configuration"),
                               QStringLiteral("Repair Target"),
                               QStringLiteral("Post-repair evidence."), true);
    flushLogRefresh(window);
    const QString withDiagnostics = window.m_logView->toPlainText();
    const int diagnosticIndex = withDiagnostics.indexOf(QStringLiteral("Diagnostic: grub"));
    const int summaryAfterDiagnostics = withDiagnostics.indexOf(QStringLiteral("Full Repair results:"));
    const int outputAfterDiagnostics = withDiagnostics.indexOf(
        QStringLiteral("Repair output\nDiagnostic: Full Repair"));
    QVERIFY2(diagnosticIndex >= 0 && summaryAfterDiagnostics >= 0 && outputAfterDiagnostics >= 0
                 && diagnosticIndex < summaryAfterDiagnostics
                 && summaryAfterDiagnostics < outputAfterDiagnostics,
             "the post-repair diagnostics must render above the summary, which stays "
             "directly above its repair output");
}

// A clean file system pre-stage inside a Full Repair plan must not append the
// standalone "no system changes" status: later stages can still change the
// system, and the plan's single end-of-plan notice owns that register entry.
// The clean pre-stage is log-only inside the plan: the plan summary already
// reports the result, so no modal information dialog may interrupt it.
void MainWindowUiTest::fullRepairCleanFilesystemPreStageIsLogOnlyAndDoesNotClaimUnchanged()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareFilesystemPlan(window);
    QVERIFY(window.m_fullRepairDpkg);
    window.m_fullRepairDpkg->setChecked(true);
    window.updateFullRepairSummary();
    QCOMPARE(window.selectedRepairStages(),
             QStringList({QStringLiteral("filesystem"), QStringLiteral("dpkg-configure")}));

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("plan-fs-clean-requests.log"));
    QProcess *session = startFilesystemFakePrivilegedSession(window, capturePath,
                                                             QStringLiteral("ext4"), false,
                                                             QString(),
                                                             QStringLiteral("clean"));
    QVERIFY2(session, "the scripted privileged session must start");
    QVERIFY(window.m_privilegedSessionReady);

    closeRepairProgressDialogWhenDone(&window);
    // The clean pre-stage is log-only inside the plan: only the plan
    // confirmation remains modal.
    acceptNextMessageBoxes(&window, 1, QMessageBox::Yes);

    window.runFullRepair();

    QVERIFY2(!window.m_fullRepairPlanInProgress, "the plan must finish");
    QVERIFY2(window.m_fullRepairPlanModified, "the plan stage changed the system");
    const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QVERIFY2(!log.contains(QStringLiteral("No system changes were detected; cached diagnostics remain valid.")),
             "a clean pre-stage must not claim the plan left the system unchanged");
    QVERIFY2(log.contains(QStringLiteral("STALE DIAGNOSTICS")),
             "the changing plan must emit the single post-plan stale notice");
    QVERIFY2(log.contains(QStringLiteral(
                 "Full Repair results: ✓ 1 successful · ✗ 0 failed · ▪ 1 no repair needed")),
             "the aggregate must count the clean pre-stage as no-repair-needed");
    QVERIFY2(log.contains(QStringLiteral(
                 "  ▪ filesystem — no file system errors found — no changes — 2 checked clean")),
             "the clean pre-stage must be auditable in the summary");
    QVERIFY2(log.contains(QStringLiteral("  ✓ dpkg — package configuration completed — changes were applied")),
             "the changing stage must be auditable in the summary");

    // --- merged from fullRepairCleanFilesystemPreStageIsLogOnly() ---
    {
        // Never adopt the previous phase's session file.
        MainWindow::s_activeSessionPath.clear();
        ScopedSessionLogDir logDir;
        QVERIFY(logDir.isValid());

        MainWindow window;
        window.show();
        QTest::qWait(50);
        prepareFilesystemPlan(window);
        QVERIFY(window.m_fullRepairDpkg);
        window.m_fullRepairDpkg->setChecked(true);
        window.updateFullRepairSummary();
        QCOMPARE(window.selectedRepairStages(),
                 QStringList({QStringLiteral("filesystem"), QStringLiteral("dpkg-configure")}));

        QTemporaryDir requestDir;
        QVERIFY(requestDir.isValid());
        const QString capturePath = requestDir.filePath(QStringLiteral("plan-fs-clean-log-only.log"));
        QProcess *session = startFilesystemFakePrivilegedSession(window, capturePath,
                                                                 QStringLiteral("ext4"), false,
                                                                 QString(),
                                                                 QStringLiteral("clean"));
        QVERIFY2(session, "the scripted privileged session must start");
        QVERIFY(window.m_privilegedSessionReady);

        closeRepairProgressDialogWhenDone(&window);

        QStringList messageTitles;
        QTimer poll;
        poll.setInterval(5);
        QObject::connect(&poll, &QTimer::timeout, &window, [&] {
            for (QWidget *top : QApplication::topLevelWidgets()) {
                auto *box = qobject_cast<QMessageBox *>(top);
                if (!box || !box->isVisible()) {
                    continue;
                }
                messageTitles.append(box->windowTitle());
                if (QAbstractButton *yes = box->button(QMessageBox::Yes)) {
                    yes->click();
                } else {
                    box->accept();
                }
                return;
            }
        });
        poll.start();

        window.runFullRepair();
        poll.stop();

        QCOMPARE(messageTitles, QStringList({QStringLiteral("Run Full Repair")}));
        QVERIFY2(!messageTitles.contains(QStringLiteral("File system repair")),
                 "the clean pre-stage must not open the information dialog during a plan");
        const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
        QVERIFY2(log.contains(QStringLiteral(
                     "No file system errors were detected on the selected repair system's root, "
                     "/boot, ESP or /home filesystems. No repair was run.")),
                 "the clean pre-stage result must be recorded in the plan log");
        QVERIFY2(log.contains(QStringLiteral(
                     "Full Repair results: ✓ 1 successful · ✗ 0 failed · ▪ 1 no repair needed")),
                 "the plan summary must still report the clean pre-stage");
    }
}

// Popup policy inside a plan: a clean (or fully skipped) inspection stays
// log-only, but an inspection that found repairable issues must present the
// device/mode selection dialog. The busy indicator stays active across the
// inspection, the selection dialog and the confirmed repairs.
void MainWindowUiTest::fullRepairIssuesFilesystemPreStageShowsSelectionDialog()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareFilesystemPlan(window);
    QCOMPARE(window.selectedRepairStages(), QStringList{QStringLiteral("filesystem")});

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("plan-fs-issues-dialog.log"));
    QVERIFY2(startFilesystemFakePrivilegedSession(window, capturePath),
             "the scripted privileged session must start");
    QVERIFY(window.m_privilegedSessionReady);

    bool sawSelectionDialog = false;
    bool busyDuringSelection = false;
    QString busyLabelDuringSelection;
    QStringList messageTitles;
    QTimer poll;
    poll.setInterval(5);
    QObject::connect(&poll, &QTimer::timeout, &window, [&] {
        for (QWidget *top : QApplication::topLevelWidgets()) {
            if (auto *box = qobject_cast<QMessageBox *>(top)) {
                if (!box->isVisible()) {
                    continue;
                }
                messageTitles.append(box->windowTitle());
                if (QAbstractButton *yes = box->button(QMessageBox::Yes)) {
                    yes->click();
                } else {
                    box->accept();
                }
                return;
            }
            auto *dialog = qobject_cast<QDialog *>(top);
            if (!dialog || !dialog->isVisible()
                || dialog->windowTitle() != QStringLiteral("Repair file system errors")) {
                continue;
            }
            auto *table = dialog->findChild<QTableWidget *>(QStringLiteral("filesystemRepairTable"));
            if (!table || table->rowCount() < 1) {
                continue;
            }
            sawSelectionDialog = true;
            busyDuringSelection = !window.m_busyIndicator->isHidden();
            busyLabelDuringSelection = window.m_busyStatusLabel->text();
            for (QPushButton *button : dialog->findChildren<QPushButton *>()) {
                if (button->text() == QStringLiteral("Run Selected Repairs")) {
                    button->click();
                    return;
                }
            }
            dialog->accept();
            return;
        }
    });
    poll.start();

    QTimer safety;
    safety.setSingleShot(true);
    safety.setInterval(5000);
    QObject::connect(&safety, &QTimer::timeout, &window, [&] {
        for (QWidget *top : QApplication::topLevelWidgets()) {
            if (auto *dialog = qobject_cast<QDialog *>(top)) {
                if (dialog->windowTitle() == QStringLiteral("Repair file system errors")) {
                    dialog->reject();
                }
            }
        }
    });
    safety.start();

    window.runFullRepair();
    poll.stop();

    QVERIFY2(sawSelectionDialog,
             "a plan whose inspection found issues must show the device/mode selection dialog");
    QVERIFY2(!messageTitles.contains(QStringLiteral("File system repair")),
             "the issues path must not be replaced by the log-only information dialog");
    QVERIFY2(busyDuringSelection,
             "the busy indicator must stay active between the inspection and the repair");
    QCOMPARE(busyLabelDuringSelection,
             QStringLiteral("Checking and repairing file systems (Full Repair)"));
    QVERIFY2(!window.m_filesystemRepairFlowActive, "the flow guard must be released after the plan");
    QVERIFY2(window.m_busyIndicator->isHidden(), "the busy indicator must hide after the flow");
    QVERIFY(window.m_activeBusyOperations.isEmpty());

    const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QVERIFY2(log.contains(QStringLiteral(
                 "Full Repair results: ✓ 1 successful · ✗ 0 failed · ▪ 0 no repair needed")),
             "the repaired filesystem stage must be counted successful");
}

// Popup policy inside a plan: issues that have no safe repair mode must not be
// log-only. The user must see a dialog that the stage cannot proceed, and the
// plan must report the failed stage without ever dispatching fs-repair.
void MainWindowUiTest::fullRepairNoSafeRepairModeFilesystemPreStageShowsDialog()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareFilesystemPlan(window);

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("plan-fs-no-safe-mode.log"));
    // A mounted ext4 filesystem with issues has only the read-only check mode
    // available: the offline repair tool must not run against a mounted
    // filesystem, so the stage cannot proceed.
    QVERIFY2(startFilesystemFakePrivilegedSession(window, capturePath, QStringLiteral("ext4"),
                                                  /*issue=*/true, QString(), QString(),
                                                  QStringLiteral("/")),
             "the scripted privileged session must start");
    QVERIFY(window.m_privilegedSessionReady);

    bool sawNotice = false;
    QString noticeText;
    QStringList messageTitles;
    QTimer poll;
    poll.setInterval(5);
    QObject::connect(&poll, &QTimer::timeout, &window, [&] {
        for (QWidget *top : QApplication::topLevelWidgets()) {
            auto *box = qobject_cast<QMessageBox *>(top);
            if (!box || !box->isVisible()) {
                continue;
            }
            messageTitles.append(box->windowTitle());
            if (box->windowTitle() == QStringLiteral("File system repair")) {
                sawNotice = true;
                noticeText = box->text();
                box->accept();
                return;
            }
            if (QAbstractButton *yes = box->button(QMessageBox::Yes)) {
                yes->click();
            } else {
                box->accept();
            }
            return;
        }
    });
    poll.start();

    window.runFullRepair();
    poll.stop();

    QVERIFY2(sawNotice,
             "a plan whose issues have no safe repair mode must show the information dialog");
    QVERIFY2(noticeText.contains(QStringLiteral("no safe repair mode")),
             "the notice must explain that the stage cannot proceed");
    QVERIFY2(!window.m_filesystemRepairFlowActive, "the flow guard must be released");
    QVERIFY2(window.m_busyIndicator->isHidden(), "the busy indicator must hide after the failure");
    QVERIFY(window.m_activeBusyOperations.isEmpty());

    const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QVERIFY2(log.contains(QStringLiteral(
                 "Full Repair results: ✓ 0 successful · ✗ 1 failed · ▪ 0 no repair needed")),
             "the unrepairable stage must be counted as failed in the aggregate");
    QVERIFY2(log.contains(QStringLiteral(
                 "file system issues were found but no safe repair mode is available")),
             "the aggregate stage line must carry the failure reason");
    QVERIFY2(!capturedHelperArguments(capturePath).contains(QStringLiteral("fs-repair")),
             "no repair may run when no safe repair mode exists");
}

// Popup policy inside a plan: a real repair failure must be visible instead of
// log-only, and the busy indicator and flow guard must still be released.
void MainWindowUiTest::fullRepairRepairFailureShowsDialogAndClearsBusy()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareFilesystemPlan(window);

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("plan-fs-repair-failure.log"));
    QVERIFY2(startMultiDeviceFilesystemFakePrivilegedSession(window, capturePath,
                                                             QStringLiteral("0"), /*repairExit=*/1),
             "the scripted privileged session must start");
    QVERIFY(window.m_privilegedSessionReady);

    bool sawFailureDialog = false;
    QString failureText;
    QTimer poll;
    poll.setInterval(5);
    QObject::connect(&poll, &QTimer::timeout, &window, [&] {
        for (QWidget *top : QApplication::topLevelWidgets()) {
            auto *box = qobject_cast<QMessageBox *>(top);
            if (!box || !box->isVisible()) {
                continue;
            }
            if (box->windowTitle() == QStringLiteral("File system repair failed")) {
                sawFailureDialog = true;
                failureText = box->text();
                box->accept();
                return;
            }
            if (QAbstractButton *yes = box->button(QMessageBox::Yes)) {
                yes->click();
            } else {
                box->accept();
            }
            return;
        }
        for (QWidget *top : QApplication::topLevelWidgets()) {
            auto *dialog = qobject_cast<QDialog *>(top);
            if (!dialog || !dialog->isVisible()
                || dialog->windowTitle() != QStringLiteral("Repair file system errors")) {
                continue;
            }
            auto *table = dialog->findChild<QTableWidget *>(QStringLiteral("filesystemRepairTable"));
            if (!table || table->rowCount() < 1) {
                continue;
            }
            for (QPushButton *button : dialog->findChildren<QPushButton *>()) {
                if (button->text() == QStringLiteral("Run Selected Repairs")) {
                    button->click();
                    return;
                }
            }
            dialog->accept();
            return;
        }
    });
    poll.start();

    QTimer safety;
    safety.setSingleShot(true);
    safety.setInterval(5000);
    QObject::connect(&safety, &QTimer::timeout, &window, [&] {
        for (QWidget *top : QApplication::topLevelWidgets()) {
            if (auto *box = qobject_cast<QMessageBox *>(top)) {
                if (box->isVisible()) {
                    box->accept();
                }
                continue;
            }
            auto *dialog = qobject_cast<QDialog *>(top);
            if (dialog && dialog->windowTitle() == QStringLiteral("Repair file system errors")) {
                dialog->reject();
            }
        }
    });
    safety.start();

    window.runFullRepair();
    poll.stop();

    QVERIFY2(sawFailureDialog,
             "a real repair failure during a plan must show the failure dialog");
    QVERIFY2(failureText.contains(QStringLiteral("Review the captured helper output")),
             "the failure dialog must point at the captured helper output");
    QVERIFY2(!window.m_filesystemRepairFlowActive, "the flow guard must be released after failure");
    QVERIFY2(window.m_busyIndicator->isHidden(), "the busy indicator must hide after failure");
    QVERIFY(window.m_activeBusyOperations.isEmpty());

    const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QVERIFY2(log.contains(QStringLiteral(
                 "Full Repair results: ✓ 0 successful · ✗ 1 failed · ▪ 0 no repair needed")),
             "the failed repair must be counted as failed in the aggregate");
    QVERIFY2(!log.contains(QStringLiteral("is waiting for the running operation to finish")),
             "repairs must never be queued behind the file system flow");
}

// A second repair attempt while a multi-device file system repair is in flight
// must be refused with a clear message, not queued behind the running flow.
// The busy indicator must cover the complete flow, including the gap between
// the read-only check and the confirmed repairs.
void MainWindowUiTest::filesystemRepairFlowBlocksSecondRepairAndKeepsBusy()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareRepairScope(window);
    window.m_snapshotPreloadScheduled = true;
    QString evidence = capabilityEvidence(false, true);
    evidence += QStringLiteral("Repair tool filesystem: available\n");
    cacheRepairEvidence(window, evidence);

    QTreeWidgetItem *filesystemItem = repairItem(window, QStringLiteral("filesystem"));
    QVERIFY(filesystemItem);
    window.m_repairToolTree->setCurrentItem(filesystemItem);

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("fs-inflight-block.log"));
    // Delay the inspection so the second attempt is guaranteed to arrive while
    // the flow is in flight.
    QVERIFY2(startMultiDeviceFilesystemFakePrivilegedSession(window, capturePath,
                                                             QStringLiteral("0.4")),
             "the scripted privileged session must start");
    QVERIFY(window.m_privilegedSessionReady);

    bool sawFlowActiveAtSecondAttempt = false;
    bool sawRefusal = false;
    QString refusalText;
    bool busyDuringSelection = false;
    QString busyLabelDuringSelection;
    QTimer poll;
    poll.setInterval(5);
    QObject::connect(&poll, &QTimer::timeout, &window, [&] {
        for (QWidget *top : QApplication::topLevelWidgets()) {
            auto *box = qobject_cast<QMessageBox *>(top);
            if (!box || !box->isVisible()) {
                continue;
            }
            if (box->windowTitle() == QStringLiteral("File system repair in progress")) {
                sawRefusal = true;
                refusalText = box->text();
                box->accept();
                return;
            }
            if (QAbstractButton *yes = box->button(QMessageBox::Yes)) {
                yes->click();
            } else {
                box->accept();
            }
            return;
        }
        for (QWidget *top : QApplication::topLevelWidgets()) {
            auto *dialog = qobject_cast<QDialog *>(top);
            if (!dialog || !dialog->isVisible()
                || dialog->windowTitle() != QStringLiteral("Repair file system errors")) {
                continue;
            }
            auto *table = dialog->findChild<QTableWidget *>(QStringLiteral("filesystemRepairTable"));
            if (!table || table->rowCount() < 2) {
                continue;
            }
            busyDuringSelection = !window.m_busyIndicator->isHidden();
            busyLabelDuringSelection = window.m_busyStatusLabel->text();
            for (QPushButton *button : dialog->findChildren<QPushButton *>()) {
                if (button->text() == QStringLiteral("Run Selected Repairs")) {
                    button->click();
                    return;
                }
            }
            dialog->accept();
            return;
        }
    });
    poll.start();

    QTimer secondAttempt;
    secondAttempt.setSingleShot(true);
    secondAttempt.setInterval(100);
    QObject::connect(&secondAttempt, &QTimer::timeout, &window, [&] {
        sawFlowActiveAtSecondAttempt = window.m_filesystemRepairFlowActive;
        window.runSelectedRepairTool();
    });
    secondAttempt.start();

    window.runSelectedRepairTool();
    poll.stop();

    QVERIFY2(sawFlowActiveAtSecondAttempt,
             "the second attempt must arrive while the flow owns the repair session");
    QVERIFY2(sawRefusal, "the second repair attempt must be refused with a clear message");
    QVERIFY2(refusalText.contains(QStringLiteral("already running")),
             "the refusal must say that a file system repair is already running");
    QVERIFY2(refusalText.contains(QStringLiteral("not queued")),
             "the refusal must state that the request was not queued");
    QVERIFY2(busyDuringSelection,
             "the busy indicator must cover the gap between inspection and repairs");
    QCOMPARE(busyLabelDuringSelection, QStringLiteral("Checking and repairing file systems"));
    QVERIFY2(!window.m_filesystemRepairFlowActive, "the flow guard must be released at the end");
    QVERIFY2(window.m_busyIndicator->isHidden(), "the busy indicator must hide after the last device");
    QVERIFY(window.m_activeBusyOperations.isEmpty());

    const QList<QStringList> requests = capturedHelperRequests(capturePath);
    QCOMPARE(requests.size(), 3);
    QVERIFY2(requests.at(0).contains(QStringLiteral("fs-inspect")),
             "the flow must start with the read-only inspection");
    QVERIFY2(requests.at(1).contains(QStringLiteral("fs-repair"))
                 && requests.at(1).contains(QStringLiteral("/dev/test-root")),
             "the first selected device must be repaired");
    QVERIFY2(requests.at(2).contains(QStringLiteral("fs-repair"))
                 && requests.at(2).contains(QStringLiteral("/dev/test-data")),
             "the second selected device must be repaired after the first");
    const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QVERIFY2(log.contains(QStringLiteral("Run Tool refused")),
             "the refusal must be recorded in the repair log");
    QVERIFY2(!log.contains(QStringLiteral("is waiting for the running operation to finish")),
             "the refused attempt must not be queued behind the running flow");
}

// Cancelling the device/mode selection must still clear the busy indicator and
// the flow guard, with no repair request dispatched.
void MainWindowUiTest::filesystemRepairCancelClearsBusyAndFlowGuard()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareRepairScope(window);
    window.m_snapshotPreloadScheduled = true;
    QString evidence = capabilityEvidence(false, true);
    evidence += QStringLiteral("Repair tool filesystem: available\n");
    cacheRepairEvidence(window, evidence);

    QTreeWidgetItem *filesystemItem = repairItem(window, QStringLiteral("filesystem"));
    QVERIFY(filesystemItem);
    window.m_repairToolTree->setCurrentItem(filesystemItem);

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("fs-cancel-busy.log"));
    QVERIFY2(startFilesystemFakePrivilegedSession(window, capturePath),
             "the scripted privileged session must start");
    QVERIFY(window.m_privilegedSessionReady);

    bool busyDuringSelection = false;
    QTimer poll;
    poll.setInterval(5);
    QObject::connect(&poll, &QTimer::timeout, &window, [&] {
        for (QWidget *top : QApplication::topLevelWidgets()) {
            auto *dialog = qobject_cast<QDialog *>(top);
            if (!dialog || !dialog->isVisible()
                || dialog->windowTitle() != QStringLiteral("Repair file system errors")) {
                continue;
            }
            auto *table = dialog->findChild<QTableWidget *>(QStringLiteral("filesystemRepairTable"));
            if (!table || table->rowCount() < 1) {
                continue;
            }
            busyDuringSelection = !window.m_busyIndicator->isHidden();
            poll.stop();
            dialog->reject();
            return;
        }
    });
    poll.start();

    QTimer safety;
    safety.setSingleShot(true);
    safety.setInterval(5000);
    QObject::connect(&safety, &QTimer::timeout, &window, [&] {
        for (QWidget *top : QApplication::topLevelWidgets()) {
            if (auto *dialog = qobject_cast<QDialog *>(top)) {
                if (dialog->windowTitle() == QStringLiteral("Repair file system errors")) {
                    dialog->reject();
                }
            }
        }
    });
    safety.start();

    window.runSelectedRepairTool();
    poll.stop();

    QVERIFY2(busyDuringSelection, "the busy indicator must be active while the selection dialog is up");
    QVERIFY2(!window.m_filesystemRepairFlowActive, "a cancel must release the flow guard");
    QVERIFY2(window.m_busyIndicator->isHidden(), "a cancel must clear the busy indicator");
    QVERIFY(window.m_activeBusyOperations.isEmpty());
    QVERIFY2(!capturedHelperArguments(capturePath).contains(QStringLiteral("fs-repair")),
             "a cancelled selection must not dispatch any repair");
    const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QVERIFY2(log.contains(QStringLiteral("cancelled before any repair was run")),
             "the cancellation must be recorded in the repair log");
}

// A standalone Check File Systems run must be clearly distinguishable from the
// Full Repair pre-stage and must not overwrite the plan's pre-stage block in
// the live register.
void MainWindowUiTest::manualFilesystemCheckIsDistinguishableFromPlanPreStage()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareFilesystemPlan(window);
    QCOMPARE(window.selectedRepairStages(), QStringList{QStringLiteral("filesystem")});

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("manual-fs-requests.log"));
    QProcess *session = startFilesystemFakePrivilegedSession(window, capturePath,
                                                             QStringLiteral("ext4"), false);
    QVERIFY2(session, "the scripted privileged session must start");
    QVERIFY(window.m_privilegedSessionReady);

    closeRepairProgressDialogWhenDone(&window);
    // The skipped plan pre-stage is log-only: only the plan confirmation
    // remains modal. The standalone run below still shows its own notice.
    acceptNextMessageBoxes(&window, 1, QMessageBox::Yes);
    window.runFullRepair();
    QVERIFY2(!window.m_fullRepairPlanInProgress, "the plan must finish");

    // A later standalone check replaces only its own section. It is a manual
    // action, so it keeps the information dialog the plan suppresses.
    NextMessageBoxCapture standaloneNotice(&window, QMessageBox::Ok);
    QVERIFY2(window.runFilesystemRepairFlow(/*partOfFullRepair=*/false),
             "the standalone check must complete");
    QVERIFY2(standaloneNotice.appeared,
             "a standalone manual check must keep its information dialog");
    QVERIFY2(standaloneNotice.text.contains(QStringLiteral("No repairable file system errors were detected")),
             "the standalone dialog must report the skipped inspection result");

    const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QCOMPARE(log.count(QStringLiteral("File system check output")), 2);
    QVERIFY2(log.contains(QStringLiteral(
                 "Starting read-only file system check (Full Repair pre-stage)")),
             "the plan pre-stage block must survive the standalone run");
    QVERIFY2(log.contains(QStringLiteral(
                 "Starting manual read-only file system check (standalone, not part of a Full Repair plan)")),
             "the standalone check must name itself as manual");
    QVERIFY2(log.contains(QStringLiteral("Check File Systems (Full Repair pre-stage) finished")),
             "the plan completion line must stay distinguishable");
    QVERIFY2(log.contains(QStringLiteral("Check File Systems (manual) finished")),
             "the manual completion line must stay distinguishable");

    // Newest-first: the manual check, then the plan summary, then the plan's
    // own pre-stage output.
    const int manualStart = log.indexOf(QStringLiteral("Starting manual read-only file system check"));
    const int planSummary = log.indexOf(QStringLiteral("Full Repair results:"));
    const int planPreStage = log.indexOf(
        QStringLiteral("Starting read-only file system check (Full Repair pre-stage)"));
    QVERIFY2(manualStart >= 0 && planSummary >= 0 && planPreStage >= 0,
             "all three blocks must be present in the register");
    QVERIFY2(manualStart < planSummary && planSummary < planPreStage,
             "the manual check must read as newer than the plan, whose summary must "
             "stay above its own pre-stage");
}
// A Full Repair plan whose every modifying stage reported unchanged performs no
// end-of-plan invalidation and no regeneration at all.
void MainWindowUiTest::fullRepairPlanSkipsRegenerationWhenAllStagesUnchanged()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareRepairScope(window);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(true);
    cacheRepairEvidence(window, capabilityEvidenceWithDisplay(false, true));

    for (QCheckBox *toggle : {window.m_fullRepairFilesystem, window.m_fullRepairDpkg,
                              window.m_fullRepairBrokenPackages, window.m_fullRepairAptUpdate,
                              window.m_fullRepairUpgrade, window.m_fullRepairDkms,
                              window.m_fullRepairDisplayManager, window.m_fullRepairInitramfs,
                              window.m_fullRepairEfi, window.m_fullRepairGrub}) {
        QVERIFY(toggle);
        toggle->setChecked(false);
    }
    window.m_fullRepairDisplayManager->setChecked(true);
    window.m_fullRepairGrub->setChecked(true);
    window.updateFullRepairSummary();
    QCOMPARE(window.selectedRepairStages(),
             QStringList({QStringLiteral("display-manager"), QStringLiteral("grub")}));

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("plan-unchanged-requests.log"));
    QVERIFY2(startChangeStatusFakePrivilegedSession(window, capturePath,
                                                    {QStringLiteral("display-manager=unchanged|default.target and display-manager.service were already correct"),
                                                     QStringLiteral("grub=unchanged|grub.cfg is byte-identical")}),
             "the scripted privileged session must start");
    closeRepairProgressDialogWhenDone(&window);
    acceptNextMessageBox(&window, QMessageBox::Yes);

    window.runFullRepair();

    QVERIFY2(!window.m_fullRepairPlanInProgress,
             "the plan guard must be released after the last stage");
    QVERIFY2(!window.m_fullRepairPlanModified,
             "an all-unchanged plan must not remember an invalidation");
    QVERIFY2(window.m_targetDiagnosticsStaleSections.isEmpty(),
             "an all-unchanged plan must not mark any section stale");
    QVERIFY2(!window.m_targetDiagnosticsNeedRegeneration,
             "an all-unchanged plan must not invalidate the complete set");
    QVERIFY2(!window.m_targetDiagnosticCache.value(QStringLiteral("display")).trimmed().isEmpty()
                 && !window.m_targetDiagnosticCache.value(QStringLiteral("grub")).trimmed().isEmpty(),
             "an all-unchanged plan must keep the cached evidence");
    QVERIFY2(!window.m_evidenceRefreshTimer || !window.m_evidenceRefreshTimer->isActive(),
             "an all-unchanged plan must skip the end-of-plan regeneration");
    const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QCOMPARE(log.count(QStringLiteral("STALE DIAGNOSTICS")), 0);
    QVERIFY2(log.contains(QStringLiteral("No system changes were detected; cached diagnostics remain valid.")),
             "an all-unchanged plan must log the no-change notice");
    QVERIFY2(log.contains(QStringLiteral(
                 "Full Repair results: ✓ 0 successful · ✗ 0 failed · ▪ 2 no repair needed")),
             "an all-unchanged plan must report the all-no-repair aggregate");
    QVERIFY2(log.contains(QStringLiteral("no repair was needed; cached diagnostics remain valid")),
             "the all-no-repair aggregate must say that no repair was needed");
    QVERIFY2(log.contains(QStringLiteral(
                 "  ▪ display — display manager already correct — no changes — "
                 "default.target and display-manager.service were already correct")),
             "the plan summary must list the display stage with the helper's unchanged reason");
    QVERIFY2(log.contains(QStringLiteral(
                 "  ▪ grub — GRUB configuration already current — no changes — grub.cfg is byte-identical")),
             "the plan summary must list the grub stage with the helper's unchanged reason");
    QCOMPARE(diagnosticRunCount(window), 0);

    QString reason;
    QVERIFY2(window.repairToolAvailable(QStringLiteral("display"), &reason),
             qPrintable(QStringLiteral("an all-unchanged plan must keep the display tool enabled: %1").arg(reason)));
    QVERIFY2(window.repairToolAvailable(QStringLiteral("grub"), &reason),
             qPrintable(QStringLiteral("an all-unchanged plan must keep the grub tool enabled: %1").arg(reason)));

    const QStringList requests = capturedHelperArguments(capturePath);
    QVERIFY2(requests.contains(QStringLiteral("display-manager")) && requests.contains(QStringLiteral("grub")),
             "both plan stages must be dispatched in the single Full Repair request");
}
// A failed Full Repair plan must attribute the failure to the stage the helper
// named. Stages that completed before it keep their own changed/unchanged
// result and stages the plan never reached are reported as not run instead of
// inheriting the failure reason.
void MainWindowUiTest::fullRepairPlanFailureAttributesOnlyFailingStage()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareRepairScope(window);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);
    cacheRepairEvidence(window, capabilityEvidenceWithDisplay(false, true));

    for (QCheckBox *toggle : {window.m_fullRepairFilesystem, window.m_fullRepairDpkg,
                              window.m_fullRepairBrokenPackages, window.m_fullRepairAptUpdate,
                              window.m_fullRepairUpgrade, window.m_fullRepairDkms,
                              window.m_fullRepairDisplayManager, window.m_fullRepairInitramfs,
                              window.m_fullRepairEfi, window.m_fullRepairGrub}) {
        QVERIFY(toggle);
        toggle->setChecked(false);
    }
    for (QCheckBox *toggle : {window.m_fullRepairBrokenPackages, window.m_fullRepairUpgrade,
                              window.m_fullRepairDisplayManager, window.m_fullRepairInitramfs,
                              window.m_fullRepairEfi, window.m_fullRepairGrub}) {
        toggle->setChecked(true);
    }
    window.updateFullRepairSummary();
    QCOMPARE(window.selectedRepairStages(),
             QStringList({QStringLiteral("fix-broken"), QStringLiteral("apt-upgrade"),
                          QStringLiteral("display-manager"), QStringLiteral("initramfs"),
                          QStringLiteral("efi"), QStringLiteral("grub")}));

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("plan-failure-requests.log"));
    QVERIFY2(startChangeStatusFakePrivilegedSession(
                 window, capturePath,
                 {QStringLiteral("fix-broken=changed"),
                  QStringLiteral("apt-upgrade=changed"),
                  QStringLiteral("display-manager=unchanged|default.target and display-manager.service were already correct"),
                  QStringLiteral("initramfs=unchanged|rebuilt initramfs images are byte-identical")},
                 /*exitCode=*/1, QStringLiteral("efi"),
                 QStringLiteral("Unable to restore the reconciled EFI BootOrder safely")),
             "the scripted privileged session must start");
    closeRepairProgressDialogWhenDone(&window);
    acceptConfirmationThenFailure(&window);

    window.runFullRepair();

    QVERIFY2(!window.m_fullRepairPlanInProgress,
             "the plan guard must be released after the failure");
    const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QVERIFY2(log.contains(QStringLiteral(
                 "Full Repair results: ✓ 2 successful · ✗ 1 failed · ▪ 2 no repair needed · "
                 "▪ 1 not run — see the grub stage line — review the failed stages in Logs.")),
             "the aggregate must count every stage's own result and separate not-run stages");
    QVERIFY2(log.contains(QStringLiteral(
                 "  ✓ fixbroken — broken dependencies repaired — changes were applied")),
             "a stage that completed before the failure keeps its changed result");
    QVERIFY2(log.contains(QStringLiteral(
                 "  ✓ upgrade — packages upgraded — changes were applied")),
             "a stage that completed before the failure keeps its changed result");
    QVERIFY2(log.contains(QStringLiteral(
                 "  ▪ display — display manager already correct — no changes — "
                 "default.target and display-manager.service were already correct")),
             "a stage that completed before the failure keeps its unchanged result");
    QVERIFY2(log.contains(QStringLiteral(
                 "  ▪ initramfs — initramfs already current — no changes — "
                 "rebuilt initramfs images are byte-identical")),
             "a stage that completed before the failure keeps its unchanged result");
    QVERIFY2(log.contains(QStringLiteral("  ✗ efi — EFI boot path repair failed —")),
             "only the stage the helper named is failed");
    QVERIFY2(log.contains(QStringLiteral("Unable to restore the reconciled EFI BootOrder safely")),
             "the failing stage must carry the helper's reason");
    QVERIFY2(log.contains(QStringLiteral("  ▪ grub — GRUB regeneration did not run")),
             "a stage the plan never reached must be reported as not run");
    QVERIFY2(!log.contains(QStringLiteral("  ✗ grub")),
             "a stage the plan never reached must never inherit the failure");
    QVERIFY2(!log.contains(QStringLiteral("  ✗ fixbroken")),
             "a completed stage must never be relabeled as failed");
}

// Selecting a repair drive is a scope change: the complete diagnostic set is
// invalidated and regenerated, not just one section.
void MainWindowUiTest::repairTargetSelectionRegeneratesCompleteSet()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    prepareRepairScope(window);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(true);
    window.m_privilegedSessionReady = true;
    cacheRepairEvidence(window, capabilityEvidence(false, true));

    DeviceNode component;
    component.path = QStringLiteral("/dev/test-root2");
    component.type = QStringLiteral("part");
    component.fileSystem = QStringLiteral("ext4");
    component.linuxCapableFileSystem = true;
    component.installedLinux = true;
    DeviceNode disk;
    disk.path = QStringLiteral("/dev/test-system2");
    disk.type = QStringLiteral("disk");
    disk.children.append(component);
    window.m_deviceIndex.insert(disk.path, disk);
    window.m_deviceIndex.insert(component.path, component);

    selectTopLevelDisk(window, disk.path);
    window.setPreviewTarget();
    QCOMPARE(window.m_previewTargetPath, QStringLiteral("/dev/test-system2"));
    QVERIFY2(window.m_targetDiagnosticsNeedRegeneration,
             "selecting a repair drive must invalidate the complete target set");
    QVERIFY2(window.m_targetDiagnosticCache.value(QStringLiteral("report")).trimmed().isEmpty(),
             "selecting a repair drive must drop the previous target's cached report");

    QTRY_VERIFY_WITH_TIMEOUT(!window.diagnosticsInvalidationPending(), 15000);
    QCOMPARE(diagnosticRunCount(window), 1);
    QVERIFY2(!window.m_targetDiagnosticCache.value(QStringLiteral("report")).trimmed().isEmpty(),
             "selecting a repair drive must regenerate the complete set");
    QVERIFY2(window.m_actionLogEntries.join(QLatin1Char('\n')).contains(
                 QStringLiteral("Starting all available read-only diagnostics (Target scope).")),
             "the target selection refresh must run the complete report");
}

void MainWindowUiTest::hostMaintenanceRequestsPrivilegedSessionOnce()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    prepareRepairScope(window, true);
    window.m_hostMaintenanceMode = false;
    window.m_uiTestPrivilegedSessionGranted = false;
    window.m_privilegedSessionRequestCount = 0;

    window.selectHostForMaintenance();
    QVERIFY(window.m_hostMaintenanceMode);
    QTRY_COMPARE(window.m_privilegedSessionRequestCount, quint64(1));
    QVERIFY(!window.m_privilegedSessionReady);

    // Re-entering the same host scope must not request authorization again
    // automatically, including after a cancellation.
    window.selectHostForMaintenance();
    QVERIFY(!window.m_hostMaintenanceMode);
    window.selectHostForMaintenance();
    QVERIFY(window.m_hostMaintenanceMode);
    QTest::qWait(kNoSecondRunWaitMs);
    QCOMPARE(window.m_privilegedSessionRequestCount, quint64(1));
}

void MainWindowUiTest::concurrentAuthorizationRequestsCoalesce()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    prepareStaleTargetScope(window);
    window.m_uiTestPrivilegedSessionGranted = true;
    // Keep the authorization request in flight long enough for a second
    // caller to arrive while the first prompt is still pending.
    window.m_uiTestPrivilegedSessionDelayMs = 100;
    window.m_privilegedSessionRequestCount = 0;

    // The pending automatic evidence refresh is waiting for authorization
    // while a concurrent caller reaches ensurePrivilegedSession() before the
    // first request completes.
    window.scheduleEvidenceRefresh(QStringLiteral("UI test concurrent authorization"));
    QVERIFY(window.m_evidenceRefreshPending);

    QString secondError;
    bool secondResult = false;
    QTimer secondCaller;
    secondCaller.setSingleShot(true);
    secondCaller.setInterval(20);
    QObject::connect(&secondCaller, &QTimer::timeout, &window, [&window, &secondError, &secondResult] {
        secondResult = window.ensurePrivilegedSession(&secondError);
    });
    secondCaller.start();

    QString firstError;
    QVERIFY2(window.ensurePrivilegedSession(&firstError),
             qPrintable(QStringLiteral("first authorization failed: %1").arg(firstError)));
    QVERIFY2(secondResult,
             qPrintable(QStringLiteral("coalesced authorization failed: %1").arg(secondError)));
    QCOMPARE(window.m_privilegedSessionRequestCount, quint64(1));

    // The single established session also releases the pending refresh.
    QTRY_VERIFY(!window.m_evidenceRefreshPending);

    // A cancelled request publishes its failure outcome so coalescing callers
    // cannot hang waiting for a prompt that will never become usable.
    window.m_privilegedSessionReady = false;
    window.m_privilegedSessionRequestCount = 0;
    window.m_uiTestPrivilegedSessionGranted = false;
    window.m_uiTestPrivilegedSessionDelayMs = 80;
    bool secondCancelledResult = true;
    QTimer secondCancelledCaller;
    secondCancelledCaller.setSingleShot(true);
    secondCancelledCaller.setInterval(20);
    QObject::connect(&secondCancelledCaller, &QTimer::timeout, &window,
                     [&window, &secondCancelledResult] {
        secondCancelledResult = window.ensurePrivilegedSession();
    });
    secondCancelledCaller.start();
    QString cancelledError;
    QVERIFY2(!window.ensurePrivilegedSession(&cancelledError),
             "a cancelled authorization request must report failure");
    QVERIFY2(!secondCancelledResult,
             "a coalesced caller must observe the cancelled request's failure");
    QCOMPARE(window.m_privilegedSessionRequestCount, quint64(1));
    window.m_uiTestPrivilegedSessionDelayMs = 0;
}

void MainWindowUiTest::authorizationFailureSurfacesPolkitAgentHint()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    // Reuse the session-process test seam: a finished process whose merged
    // output carries the pkexec textual-agent failure. The UI-test
    // authorization branch must surface the actionable Alpine hint through the
    // same composition the production pkexec path uses.
    auto *session = new QProcess(&window);
    session->setProcessChannelMode(QProcess::MergedChannels);
    session->start(QStringLiteral("/bin/sh"),
                   {QStringLiteral("-c"), QStringLiteral("cat >&2; exit 126")});
    QVERIFY2(session->waitForStarted(5000), "the failing session process must start");
    session->write(QByteArrayLiteral(
        "Error creating textual authentication agent: Error opening current "
        "controlling terminal for the process (`/dev/tty'): No such device or address\n"));
    session->closeWriteChannel();
    QVERIFY2(session->waitForFinished(5000), "the failing session process must finish");
    window.m_privilegedSession = session;
    window.m_privilegedSessionReady = false;
    window.m_uiTestPrivilegedSessionGranted = false;

    QString error;
    QVERIFY2(!window.ensurePrivilegedSession(&error),
             "an agent-failure session must report authorization failure");
    // The stable prefix stays first so existing callers and messages remain
    // compatible, followed by the real pkexec detail and the actionable fix.
    QVERIFY2(error.startsWith(QStringLiteral("Administrator authorization session was not established.")),
             qPrintable(QStringLiteral("the stable failure prefix must stay: %1").arg(error)));
    QVERIFY2(error.contains(QStringLiteral("textual authentication agent")),
             qPrintable(QStringLiteral("the pkexec detail must be surfaced: %1").arg(error)));
    QVERIFY2(error.contains(QStringLiteral("No working Polkit authentication agent was found")),
             qPrintable(QStringLiteral("the actionable agent hint must be surfaced: %1").arg(error)));
    QVERIFY2(error.contains(QStringLiteral("apk add polkit-elogind xfce-polkit")),
             qPrintable(QStringLiteral("the hint must name the Alpine fix: %1").arg(error)));
    // The complete pkexec detail stays in the session log even though the
    // dialog text stays concise.
    QVERIFY2(window.m_actionLogEntries.join(QLatin1Char('\n')).contains(
                 QStringLiteral("Authorization session: Error creating textual authentication agent")),
             "the session log must keep the full pkexec detail");

    // A plain cancellation with no pkexec detail keeps the exact generic
    // message and gains no misleading agent hint.
    window.m_privilegedSession = nullptr;
    session->deleteLater();
    QString cancelledError;
    QVERIFY(!window.ensurePrivilegedSession(&cancelledError));
    QCOMPARE(cancelledError, QStringLiteral("Administrator authorization session was not established."));
}

void MainWindowUiTest::repairTargetConfirmationRequestsPrivilegedSessionOnce()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    prepareRepairScope(window);
    window.m_previewTargetPath.clear();
    window.m_previewTargetComponentPath.clear();
    window.m_uiTestPrivilegedSessionGranted = false;
    window.m_privilegedSessionRequestCount = 0;
    // Keep the committed target from starting the deferred Btrfs snapshot
    // preload; the session-request behavior is what this test exercises.
    window.m_snapshotPreloadScheduled = true;

    selectTopLevelDisk(window, QStringLiteral("/dev/test-system"));
    window.setPreviewTarget();
    QCOMPARE(window.m_previewTargetPath, QStringLiteral("/dev/test-system"));
    QCOMPARE(window.m_previewTargetComponentPath, QStringLiteral("/dev/test-root"));
    QTRY_COMPARE(window.m_privilegedSessionRequestCount, quint64(1));

    // Confirming the same committed target again does not re-prompt.
    window.setPreviewTarget();
    QTest::qWait(kNoSecondRunWaitMs);
    QCOMPARE(window.m_privilegedSessionRequestCount, quint64(1));

    // A still-locked LUKS drive is rejected before any authorization request.
    DeviceNode locked;
    locked.path = QStringLiteral("/dev/test-luksp1");
    locked.type = QStringLiteral("crypt");
    locked.fileSystem = QStringLiteral("crypto_LUKS");
    locked.encrypted = true;
    DeviceNode lockedDisk;
    lockedDisk.path = QStringLiteral("/dev/test-luks");
    lockedDisk.type = QStringLiteral("disk");
    lockedDisk.children.append(locked);
    window.m_deviceIndex.insert(lockedDisk.path, lockedDisk);
    window.m_deviceIndex.insert(locked.path, locked);
    selectTopLevelDisk(window, lockedDisk.path);
    acceptNextMessageBox(&window, QMessageBox::Ok);
    window.setPreviewTarget();
    QTest::qWait(200);
    QCOMPARE(window.m_privilegedSessionRequestCount, quint64(1));

    // Once the decrypted Linux root is visible and selected, the mapped target
    // confirmation requests the session.
    DeviceNode mapped;
    mapped.path = QStringLiteral("/dev/mapper/test-root");
    mapped.type = QStringLiteral("part");
    mapped.fileSystem = QStringLiteral("ext4");
    mapped.linuxCapableFileSystem = true;
    mapped.installedLinux = true;
    DeviceNode crypt;
    crypt.path = QStringLiteral("/dev/test-luksp1");
    crypt.type = QStringLiteral("crypt");
    crypt.fileSystem = QStringLiteral("crypto_LUKS");
    crypt.encrypted = true;
    crypt.children.append(mapped);
    DeviceNode mappedDisk;
    mappedDisk.path = QStringLiteral("/dev/test-luks");
    mappedDisk.type = QStringLiteral("disk");
    mappedDisk.children.append(crypt);
    window.m_deviceIndex.insert(mappedDisk.path, mappedDisk);
    window.m_deviceIndex.insert(crypt.path, crypt);
    window.m_deviceIndex.insert(mapped.path, mapped);
    selectTopLevelDisk(window, mappedDisk.path);
    window.setPreviewTarget();
    QCOMPARE(window.m_previewTargetComponentPath, mapped.path);
    QTRY_COMPARE(window.m_privilegedSessionRequestCount, quint64(2));
}

void MainWindowUiTest::activePrivilegedSessionIsNotRequestedAgain()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    prepareRepairScope(window);
    window.m_previewTargetPath.clear();
    window.m_previewTargetComponentPath.clear();
    window.m_privilegedSessionReady = true;
    window.m_uiTestPrivilegedSessionGranted = true;
    window.m_privilegedSessionRequestCount = 0;
    window.m_snapshotPreloadScheduled = true;

    selectTopLevelDisk(window, QStringLiteral("/dev/test-system"));
    window.setPreviewTarget();
    QCOMPARE(window.m_previewTargetPath, QStringLiteral("/dev/test-system"));
    QTest::qWait(200);
    QCOMPARE(window.m_privilegedSessionRequestCount, quint64(0));

    prepareRepairScope(window, true);
    window.m_hostMaintenanceMode = false;
    window.selectHostForMaintenance();
    QVERIFY(window.m_hostMaintenanceMode);
    QTest::qWait(200);
    QCOMPARE(window.m_privilegedSessionRequestCount, quint64(0));
    QVERIFY(window.m_authorizationStatusLabel->isHidden());
    QVERIFY(window.m_authorizeNowButton->isHidden());
}

void MainWindowUiTest::authorizationCancelKeepsScopeWithoutReprompting()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    prepareRepairScope(window);
    window.m_previewTargetPath.clear();
    window.m_previewTargetComponentPath.clear();
    window.m_uiTestPrivilegedSessionGranted = false;
    window.m_privilegedSessionRequestCount = 0;
    window.m_snapshotPreloadScheduled = true;

    selectTopLevelDisk(window, QStringLiteral("/dev/test-system"));
    window.setPreviewTarget();
    QTRY_COMPARE(window.m_privilegedSessionRequestCount, quint64(1));

    // The cancelled authorization never drops or blocks the committed scope.
    QVERIFY(!window.m_privilegedSessionReady);
    QCOMPARE(window.m_previewTargetPath, QStringLiteral("/dev/test-system"));
    QCOMPARE(window.m_previewTargetComponentPath, QStringLiteral("/dev/test-root"));
    QCOMPARE(window.m_authorizationDeferredScope, QStringLiteral("target:/dev/test-system"));
    QVERIFY(window.m_authorizationStatusLabel);
    QVERIFY(window.m_authorizeNowButton);
    QVERIFY2(!window.m_authorizationStatusLabel->isHidden(),
             "the deferred authorization must stay visible on the scope controls");
    QVERIFY2(!window.m_authorizeNowButton->isHidden(),
             "a non-modal Authorize affordance must be offered after cancellation");
    QVERIFY(window.m_authorizationStatusLabel->text().contains(QStringLiteral("deferred"), Qt::CaseInsensitive));
    QVERIFY(window.m_actionLogEntries.join(QLatin1Char('\n')).contains(
        QStringLiteral("Administrator authorization was deferred; the next privileged action will request it again.")));

    // No automatic re-prompt loop for the same scope.
    QTest::qWait(kNoSecondRunWaitMs);
    QCOMPARE(window.m_privilegedSessionRequestCount, quint64(1));

    // The explicit Authorize button may request again at any time.
    window.m_authorizeNowButton->click();
    QCOMPARE(window.m_privilegedSessionRequestCount, quint64(2));
    QVERIFY(!window.m_privilegedSessionReady);

    // A later successful authorization clears the deferred state.
    window.m_uiTestPrivilegedSessionGranted = true;
    window.m_authorizeNowButton->click();
    QCOMPARE(window.m_privilegedSessionRequestCount, quint64(3));
    QVERIFY(window.m_privilegedSessionReady);
    QVERIFY(window.m_authorizationDeferredScope.isEmpty());
    QVERIFY(window.m_authorizationStatusLabel->isHidden());
    QVERIFY(window.m_authorizeNowButton->isHidden());
}

void MainWindowUiTest::authorizationPathCreatesNoVisibleTopLevelWindow()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    const auto visibleTopLevels = [] {
        QSet<QWidget *> widgets;
        for (QWidget *top : QApplication::topLevelWidgets()) {
            if (top->isVisible()) {
                widgets.insert(top);
            }
        }
        return widgets;
    };
    const QSet<QWidget *> before = visibleTopLevels();

    // A granted authorization must not open any application-side window: the
    // pkexec/Polkit prompt is the only authorization UI.
    window.m_uiTestPrivilegedSessionGranted = true;
    window.m_uiTestPrivilegedSessionDelayMs = 60;
    window.authorizePrivilegedSessionNow();
    QVERIFY2(visibleTopLevels() == before,
             "a granted authorization opened a visible top-level window");
    QVERIFY(window.m_privilegedSessionReady);

    // A cancelled authorization must stay windowless as well.
    window.m_privilegedSessionReady = false;
    window.m_uiTestPrivilegedSessionGranted = false;
    window.authorizePrivilegedSessionNow();
    QVERIFY2(visibleTopLevels() == before,
             "a cancelled authorization opened a visible top-level window");
    QVERIFY(!window.m_privilegedSessionReady);

    // The deferred automatic request path stays windowless while it waits and
    // after the outcome is published.
    window.m_authorizationScopeRequested.clear();
    window.m_authorizationDeferredScope.clear();
    window.m_uiTestPrivilegedSessionGranted = true;
    window.requestPrivilegedSessionForScope(QStringLiteral("target:/dev/test-system"));
    QCoreApplication::processEvents();
    QVERIFY2(visibleTopLevels() == before,
             "a pending authorization request opened a visible top-level window");
    QTRY_VERIFY(window.m_privilegedSessionReady);
    QVERIFY2(visibleTopLevels() == before,
             "an established authorization opened a visible top-level window");
    window.m_uiTestPrivilegedSessionDelayMs = 0;
}

void MainWindowUiTest::pendingAutoRefreshRunsAfterScopeAuthorization()
{
    MainWindow window;
    prepareStaleTargetScope(window);
    window.m_evidenceRefreshDelayMs = kEvidenceRefreshTestDelayMs;
    window.m_uiTestPrivilegedSessionGranted = true;
    window.m_privilegedSessionRequestCount = 0;

    window.scheduleEvidenceRefresh(QStringLiteral("UI test scope confirmation"));
    QVERIFY(window.m_evidenceRefreshPending);

    window.requestPrivilegedSessionForScope(QStringLiteral("target:/dev/test-system"));
    QTRY_VERIFY(window.m_privilegedSessionReady);
    QCOMPARE(window.m_privilegedSessionRequestCount, quint64(1));
    QVERIFY(!window.m_evidenceRefreshPending);

    QTRY_VERIFY(diagnosticRunCount(window) >= 1);
    QTest::qWait(kNoSecondRunWaitMs);
    QCOMPARE(diagnosticRunCount(window), 1);
    QVERIFY(!window.m_targetDiagnosticsNeedRegeneration);
    QVERIFY(window.m_targetDiagnosticCache.contains(QStringLiteral("report")));
    QVERIFY(window.m_actionLogEntries.join(QLatin1Char('\n')).contains(
        QStringLiteral("Automatic read-only diagnostics regeneration completed")));
}

void MainWindowUiTest::logKindsSeparateStartupAndDiagnosticLines()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    const auto latestIsCategory = [&window](const QString &category) {
        return window.m_actionLogEntries.first().contains(
            QStringLiteral("──────── %1 ────────").arg(category));
    };

    // Generic application messages default to the Application kind.
    window.appendLog(QStringLiteral("Boot Bitch 9.9.9 started in guarded repair mode."));
    QVERIFY(latestIsCategory(QStringLiteral("APPLICATION")));

    window.appendLog(QStringLiteral(
        "Launch detection: 3 device(s) scanned; protected running host /dev/vda; repair target /dev/vdb; privileged helper available; pkexec available."));
    QVERIFY(latestIsCategory(QStringLiteral("APPLICATION")));

    window.appendLog(QStringLiteral("Session log session-20260101-000000.log generated 2026-01-01 00:00:00"));
    QVERIFY(latestIsCategory(QStringLiteral("APPLICATION")));

    // Every other kind is chosen by the caller, never inferred from the text:
    // diagnostic text may contain repair keywords and repair text may contain
    // diagnostic keywords without changing the recorded kind.
    window.appendLog(QStringLiteral("Diagnostic: grub\nRepair tool grub: available\nPASS"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Diagnostic);
    QVERIFY(latestIsCategory(QStringLiteral("DIAGNOSTIC")));

    window.appendLog(QStringLiteral("Repair output\nupdate-grub completed successfully."),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Repair);
    QVERIFY(latestIsCategory(QStringLiteral("REPAIR")));

    window.appendLog(QStringLiteral("Unlock /dev/vdb2 through LUKS"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Unlock);
    QVERIFY(latestIsCategory(QStringLiteral("UNLOCK")));

    window.appendLog(QStringLiteral("Loading Btrfs snapshots read-only."),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::Snapshot);
    QVERIFY(latestIsCategory(QStringLiteral("SNAPSHOTS")));

    window.appendLog(QStringLiteral("Starting verified File Copy (Host → Repair)."),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::FileCopy);
    QVERIFY(latestIsCategory(QStringLiteral("FILE COPY")));

    window.appendLog(QStringLiteral("Chroot shell command completed: update-grub"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::ChrootShell);
    QVERIFY(latestIsCategory(QStringLiteral("CHROOT SHELL")));

    window.appendLog(QStringLiteral("Host shell command completed: apt update"),
                     QStringLiteral("INFO"), MainWindow::LogEntryKind::HostShell);
    QVERIFY(latestIsCategory(QStringLiteral("HOST SHELL")));
}

void MainWindowUiTest::busyIndicatorTracksOverlappingOperations()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    QVERIFY(window.m_busyIndicator);
    QVERIFY(window.m_busyProgress);
    QVERIFY(window.m_busyStatusLabel);
    QVERIFY2(qobject_cast<BusyIndicatorWidget *>(window.m_busyProgress),
             "the busy indicator must be the custom-painted BusyIndicatorWidget");
    QVERIFY2(!qobject_cast<QProgressBar *>(window.m_busyProgress),
             "the platform-styled QProgressBar must not be used");
    QVERIFY2(!window.m_busyProgress->isAnimating(),
             "the custom striped bar must not animate while idle");
    QVERIFY2(window.m_busyIndicator->isHidden(), "the busy indicator must be hidden while idle");

    window.beginBusyOperation(QStringLiteral("Running diagnostic: Environment validation"));
    QVERIFY(!window.m_busyIndicator->isHidden());
    QCOMPARE(window.m_busyStatusLabel->text(),
             QStringLiteral("Running diagnostic: Environment validation"));

    window.beginBusyOperation(QStringLiteral("Running all diagnostics"));
    QVERIFY(!window.m_busyIndicator->isHidden());
    QCOMPARE(window.m_busyStatusLabel->text(), QStringLiteral("Running all diagnostics"));

    window.endBusyOperation(QStringLiteral("Running all diagnostics"));
    QVERIFY2(!window.m_busyIndicator->isHidden(),
             "finishing one of two overlapping operations must keep the indicator visible");
    QCOMPARE(window.m_busyStatusLabel->text(),
             QStringLiteral("Running diagnostic: Environment validation"));

    window.endBusyOperation(QStringLiteral("Running diagnostic: Environment validation"));
    QVERIFY2(window.m_busyIndicator->isHidden(),
             "finishing the last operation must hide the indicator");
    QVERIFY(window.m_activeBusyOperations.isEmpty());
}

// The busy indicator must be the same custom-painted striped bar on every
// distribution and theme: the platform QProgressBar indeterminate style (a
// static solid bar on Fedora/Adwaita, a moving chunk on Breeze/Fusion) is not
// used. The animation starts with the busy state and stops when the last
// operation ends; the painted shape itself is pinned by the pixel-level
// busyIndicatorPaintsClassicStripedBar test.
void MainWindowUiTest::busyIndicatorUsesCustomStripedIndicator()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    QCOMPARE(QString::fromLatin1(window.m_busyProgress->metaObject()->className()),
             QStringLiteral("BusyIndicatorWidget"));
    QVERIFY2(!window.m_busyProgress->isAnimating(),
             "the custom striped bar must be idle while no operation is running");
    const qreal idlePhase = window.m_busyProgress->animationPhase();

    window.beginBusyOperation(QStringLiteral("Running chroot shell command"));
    QVERIFY(!window.m_busyIndicator->isHidden());
    QVERIFY2(window.m_busyProgress->isAnimating(),
             "the custom striped bar must animate while an operation is running");
    QTest::qWait(120);
    QVERIFY2(window.m_busyProgress->animationPhase() != idlePhase,
             "the stripe phase must advance while the busy state is active");

    window.endBusyOperation(QStringLiteral("Running chroot shell command"));
    QVERIFY(window.m_busyIndicator->isHidden());
    QVERIFY2(!window.m_busyProgress->isAnimating(),
             "the custom striped bar must stop animating when the operation ends");

    // Overlapping operations keep the striped bar running until the last one.
    window.beginBusyOperation(QStringLiteral("Running all diagnostics"));
    window.beginBusyOperation(QStringLiteral("Running chroot shell command"));
    window.endBusyOperation(QStringLiteral("Running all diagnostics"));
    QVERIFY(window.m_busyProgress->isAnimating());
    window.endBusyOperation(QStringLiteral("Running chroot shell command"));
    QVERIFY(!window.m_busyProgress->isAnimating());
}

// Pixel-level verification of the restored busy look: a classic striped
// indeterminate bar (recessed groove, repeating diagonal stripes) instead of
// the former dashed-segment marquee. A standalone widget with a pinned palette
// is grabbed so the rendered output itself is inspected, not just the widget
// class; the pattern is derived only from the palette and the phase, so the
// same frame is expected on Alpine, Arch, Fedora and Debian/TUXEDO.
void MainWindowUiTest::busyIndicatorPaintsClassicStripedBar()
{
    BusyIndicatorWidget indicator;
    indicator.resize(120, 8);
    indicator.setAnimating(false);

    QPalette palette = indicator.palette();
    palette.setColor(QPalette::Highlight, QColor(0, 0, 255));
    palette.setColor(QPalette::Base, QColor(255, 255, 255));
    palette.setColor(QPalette::Mid, QColor(128, 128, 128));
    palette.setColor(QPalette::WindowText, QColor(0, 0, 0));
    indicator.setPalette(palette);

    const QImage image = indicator.grab().toImage();
    QVERIFY(!image.isNull());
    QVERIFY(image.width() >= 100);
    QVERIFY(image.height() >= 8);

    const auto isStripe = [](const QColor &pixel) {
        return pixel.blue() > pixel.red() + 20 && pixel.blue() > pixel.green() + 20;
    };
    const auto stripeRuns = [&image, &isStripe](int y) {
        int runs = 0;
        bool inStripe = false;
        for (int x = 0; x < image.width(); ++x) {
            const bool stripe = isStripe(image.pixelColor(x, y));
            if (stripe && !inStripe) {
                ++runs;
            }
            inStripe = stripe;
        }
        return runs;
    };
    const auto firstStripeX = [&image, &isStripe](int y) {
        // Start past the left edge so the comparison sees a complete stripe
        // instead of one clipped by the groove outline.
        for (int x = image.width() / 8; x < image.width(); ++x) {
            if (isStripe(image.pixelColor(x, y))) {
                return x;
            }
        }
        return -1;
    };

    // The classic bar repeats stripes across the track instead of a single
    // moving chunk, and the stripes cover the full bar height.
    const int topY = 1;
    const int bottomY = image.height() - 2;
    QVERIFY2(stripeRuns(topY) >= 4, "the indeterminate bar must paint repeating stripes");
    QVERIFY2(stripeRuns(bottomY) >= 4, "the stripes must cover the full bar height");

    // The stripes are diagonal: the first complete stripe starts at a
    // different x near the top and the bottom of the bar (dashed vertical
    // segments would align).
    const int topX = firstStripeX(topY);
    const int bottomX = firstStripeX(bottomY);
    QVERIFY(topX >= 0);
    QVERIFY(bottomX >= 0);
    QVERIFY2(qAbs(topX - bottomX) >= 2,
             "the stripes must be diagonal, not vertical or dashed");
}

void MainWindowUiTest::busyIndicatorCoversDiagnosticsAndFailedOperations()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    QSignalSpy spy(&window, &MainWindow::busyStateChanged);
    QVERIFY(spy.isValid());

    // A real diagnostics run must drive the indicator through one complete
    // visible -> hidden cycle carrying the diagnostic label.
    enterHostDiagnosticScope(window);
    int environmentRow = -1;
    for (int row = 0; row < window.m_diagnosticList->count(); ++row) {
        if (window.m_diagnosticList->item(row)->data(Qt::UserRole).toString() == QStringLiteral("environment")) {
            environmentRow = row;
            break;
        }
    }
    QVERIFY(environmentRow >= 0);
    window.m_diagnosticList->setCurrentRow(environmentRow);
    window.runSelectedDiagnostic();

    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.at(0).at(0).toBool(), true);
    QVERIFY(spy.at(0).at(1).toString().contains(QStringLiteral("diagnostic"), Qt::CaseInsensitive));
    QCOMPARE(spy.at(1).at(0).toBool(), false);
    QVERIFY(window.m_busyIndicator->isHidden());

    // A failed privileged operation must still release the indicator.
    spy.clear();
    window.m_evidenceRefreshInProgress = true; // Suppress the modal authorization warning.
    window.m_uiTestPrivilegedSessionGranted = false;
    window.m_privilegedSessionReady = false;
    bool succeeded = true;
    window.runPrivilegedRequest(QStringLiteral("Failed test operation"),
                                {QStringLiteral("diagnose")},
                                QByteArray(), &succeeded, false);
    QVERIFY(!succeeded);
    QCOMPARE(spy.count(), 4);
    QCOMPARE(spy.at(0).at(0).toBool(), true);
    QCOMPARE(spy.at(3).at(0).toBool(), false);
    QVERIFY(window.m_busyIndicator->isHidden());
    QVERIFY(window.m_activeBusyOperations.isEmpty());
}

void MainWindowUiTest::busyIndicatorDoesNotShiftLayout()
{
    MainWindow window;
    window.show();
    QTest::qWait(100);
    flushLogRefresh(window);

    const QSize settledWindowSize = window.size();
    const QRect settledTabs = window.m_tabs->geometry();
    const QRect settledLogView = window.m_logView->geometry();

    // A deliberately long label exercises elision without changing the
    // reserved slot's geometry.
    const QString label = QStringLiteral(
        "Running diagnostic: Environment validation with an intentionally long operation description");
    window.beginBusyOperation(label);
    QCoreApplication::processEvents();
    QVERIFY(!window.m_busyIndicator->isHidden());
    QVERIFY(window.m_busyStatusLabel->text() == label);
    QCOMPARE(window.size(), settledWindowSize);
    QCOMPARE(window.m_tabs->geometry(), settledTabs);
    QCOMPARE(window.m_logView->geometry(), settledLogView);

    window.endBusyOperation(label);
    QCoreApplication::processEvents();
    QVERIFY(window.m_busyIndicator->isHidden());
    QCOMPARE(window.size(), settledWindowSize);
    QCOMPARE(window.m_tabs->geometry(), settledTabs);
    QCOMPARE(window.m_logView->geometry(), settledLogView);
}

void MainWindowUiTest::logsTabReorientsWithWindowWidth()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());
    const QString priorPath = logDir.path() + QStringLiteral("/session-20200101-000000.log");
    writeSessionLog(priorPath,
                    QStringLiteral("[2020-01-01 00:00:00] [SCOPE] Running Host | Disk: /dev/narrow | Root: /dev/narrow1 | OS: debian"),
                    QStringLiteral("NARROW_PRIOR_MARKER"));

    MainWindow window;
    window.resize(480, 500);
    window.show();
    QTest::qWait(100);
    window.m_tabs->setCurrentIndex(6);
    QCoreApplication::processEvents();
    QTest::qWait(50);

    QVERIFY(window.m_sessionLogSplitter);
    QVERIFY(window.m_sessionLogPanel);
    QVERIFY(window.m_sessionLogViewer);
    QVERIFY(!window.m_sessionLogSplitter->childrenCollapsible());
    QCOMPARE(window.m_sessionLogSplitter->count(), 2);
    QCOMPARE(window.m_sessionLogSplitter->orientation(), Qt::Vertical);
    QCOMPARE(window.m_sessionLogSplitter->widget(0), window.m_sessionLogViewer);
    QCOMPARE(window.m_sessionLogSplitter->widget(1), window.m_sessionLogPanel);
    QVERIFY(window.m_sessionLogViewer->isVisible());
    QVERIFY(window.m_sessionLogPanel->isVisible());

    const QRect viewerRect(window.m_sessionLogViewer->mapTo(window.m_sessionLogSplitter, QPoint(0, 0)),
                           window.m_sessionLogViewer->size());
    const QRect panelRect(window.m_sessionLogPanel->mapTo(window.m_sessionLogSplitter, QPoint(0, 0)),
                          window.m_sessionLogPanel->size());
    QVERIFY2(viewerRect.bottom() <= panelRect.top(),
             "the log view must sit above the session selection frame when narrow");
    QVERIFY2(!viewerRect.intersects(panelRect), "the Logs frames must not overlap");
    // The selection frame starts compact and must not squeeze the log view.
    QVERIFY(window.m_logView->width() >= 300);
    QVERIFY(window.m_sessionLogList->minimumWidth() <= 160);

    const QList<QPushButton *> sessionButtons = window.m_sessionLogPanel->findChildren<QPushButton *>();
    QCOMPARE(sessionButtons.size(), 5);
    for (int i = 0; i < sessionButtons.size(); ++i) {
        for (int j = i + 1; j < sessionButtons.size(); ++j) {
            const QRect first(sessionButtons.at(i)->mapTo(window.m_sessionLogPanel, QPoint(0, 0)),
                              sessionButtons.at(i)->size());
            const QRect second(sessionButtons.at(j)->mapTo(window.m_sessionLogPanel, QPoint(0, 0)),
                               sessionButtons.at(j)->size());
            QVERIFY2(!first.intersects(second),
                     qPrintable(QStringLiteral("session buttons %1 and %2 overlap")
                                    .arg(sessionButtons.at(i)->text(), sessionButtons.at(j)->text())));
        }
    }

    // Session selection, prior-log banner and search keep working while the
    // splitter is vertical.
    int priorRow = -1;
    for (int row = 0; row < window.m_sessionLogList->count(); ++row) {
        if (window.m_sessionLogList->item(row)->data(Qt::UserRole).toString() == priorPath) {
            priorRow = row;
            break;
        }
    }
    QVERIFY(priorRow >= 0);
    window.m_sessionLogList->setCurrentRow(priorRow);
    QCoreApplication::processEvents();
    QVERIFY(window.m_priorLogBanner->isVisible());
    QVERIFY(window.m_priorLogBanner->text().contains(QStringLiteral("read only")));
    QVERIFY(window.m_logView->toPlainText().contains(QStringLiteral("NARROW_PRIOR_MARKER")));
    window.m_logSearchEdit->setText(QStringLiteral("narrow prior"));
    QVERIFY(window.m_logView->toPlainText().contains(QStringLiteral("NARROW_PRIOR_MARKER")));
    window.m_logSearchEdit->clear();

    // Wide windows restore the side-by-side layout with the selection frame
    // on the left and the log view on the right.
    window.resize(1400, 900);
    QCoreApplication::processEvents();
    QTest::qWait(50);
    QCOMPARE(window.m_sessionLogSplitter->orientation(), Qt::Horizontal);
    QCOMPARE(window.m_sessionLogSplitter->widget(0), window.m_sessionLogPanel);
    QCOMPARE(window.m_sessionLogSplitter->widget(1), window.m_sessionLogViewer);
    const QRect widePanelRect(window.m_sessionLogPanel->mapTo(window.m_sessionLogSplitter, QPoint(0, 0)),
                              window.m_sessionLogPanel->size());
    const QRect wideViewerRect(window.m_sessionLogViewer->mapTo(window.m_sessionLogSplitter, QPoint(0, 0)),
                               window.m_sessionLogViewer->size());
    QVERIFY2(widePanelRect.right() <= wideViewerRect.left(),
             "the session selection frame must stay left of the log view when wide");
    QVERIFY2(!widePanelRect.intersects(wideViewerRect), "the Logs frames must not overlap");
    QVERIFY(window.m_sessionLogSplitter->sizes().value(0)
            < window.m_sessionLogSplitter->sizes().value(1));

    // The prior selection and banner survive the orientation change.
    QCoreApplication::processEvents();
    QVERIFY(window.m_priorLogBanner->isVisible());
    window.m_sessionLogList->setCurrentRow(0);
    QCoreApplication::processEvents();
    QVERIFY(!window.m_priorLogBanner->isVisible());
}

void MainWindowUiTest::fileCopyButtonsDoNotOverlapAtMinimumWidth()
{
    MainWindow window;
    window.resize(480, 500);
    window.show();
    QTest::qWait(100);
    window.m_tabs->setCurrentIndex(5);
    QCoreApplication::processEvents();
    QTest::qWait(50);

    QVERIFY(window.m_fileCopySourceBox);
    const QList<QPushButton *> sourceButtons = window.m_fileCopySourceBox->findChildren<QPushButton *>();
    QCOMPARE(sourceButtons.size(), 4);
    for (QPushButton *button : sourceButtons) {
        // The staging row may shrink below its content width instead of
        // colliding with the neighbouring buttons.
        QVERIFY2(button->minimumWidth() < button->sizeHint().width(),
                 qPrintable(QStringLiteral("%1 should be allowed to shrink").arg(button->text())));
        const QRect rect(button->mapTo(window.m_fileCopySourceBox, QPoint(0, 0)), button->size());
        QVERIFY2(window.m_fileCopySourceBox->contentsRect().contains(rect),
                 qPrintable(QStringLiteral("%1 is clipped by the source frame").arg(button->text())));
    }
    for (int i = 0; i < sourceButtons.size(); ++i) {
        for (int j = i + 1; j < sourceButtons.size(); ++j) {
            const QRect first(sourceButtons.at(i)->mapTo(window.m_fileCopySourceBox, QPoint(0, 0)),
                              sourceButtons.at(i)->size());
            const QRect second(sourceButtons.at(j)->mapTo(window.m_fileCopySourceBox, QPoint(0, 0)),
                               sourceButtons.at(j)->size());
            QVERIFY2(!first.intersects(second),
                     qPrintable(QStringLiteral("File Copy buttons %1 and %2 overlap")
                                    .arg(sourceButtons.at(i)->text(), sourceButtons.at(j)->text())));
        }
    }

    QVERIFY(window.m_fileCopyPreviewButton);
    QVERIFY(window.m_fileCopyRunButton);
    QVERIFY2(!window.m_fileCopyPreviewButton->geometry().intersects(window.m_fileCopyRunButton->geometry()),
             "preview and copy buttons must not overlap");
}

void MainWindowUiTest::hostDriveSummaryWrapsAtMinimumWidth()
{
    MainWindow window;
    window.resize(480, 500);
    window.show();
    QTest::qWait(100);

    QVERIFY(window.m_hostStorageLabel);
    QVERIFY(window.m_hostMountsLabel);
    QVERIFY(window.m_hostCardLayout);
    // The protected drive card labels share the same summary pattern; both
    // must wrap instead of clipping their text.
    QVERIFY(window.m_hostStorageLabel->wordWrap());
    QVERIFY(window.m_hostMountsLabel->wordWrap());
    // Narrow windows stack the running-system identity above its actions so
    // the summary keeps the full card width instead of wrapping word by word
    // beside the buttons.
    QCOMPARE(window.m_hostCardLayout->direction(), QBoxLayout::TopToBottom);

    const QString summary = QStringLiteral(
        "Example NVMe 1TB  •  /dev/nvme0n1  •  931 GiB  •  NVME  •  Critical mounts: /, /boot/efi");
    window.m_hostStorageLabel->setText(summary);
    QCoreApplication::processEvents();
    QTest::qWait(50);

    const QFontMetrics metrics(window.m_hostStorageLabel->font());
    QVERIFY2(window.m_hostStorageLabel->height()
                 >= window.m_hostStorageLabel->heightForWidth(window.m_hostStorageLabel->width()),
             "the wrapped drive summary must not be clipped vertically");

    window.resize(1400, 900);
    QCoreApplication::processEvents();
    QTest::qWait(50);
    QCOMPARE(window.m_hostCardLayout->direction(), QBoxLayout::LeftToRight);
    window.m_hostStorageLabel->setText(summary);
    QCoreApplication::processEvents();
    QVERIFY2(window.m_hostStorageLabel->height() <= metrics.lineSpacing() * 2,
             "the drive summary should stay on one line when the window is wide");
}

void MainWindowUiTest::hostShellModeLabelsAndEnablement()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    // A prior test may have persisted compact window geometry; the full tab
    // label is only used at the wide responsive breakpoint.
    window.resize(1180, 760);
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window, true);
    window.updateTargetLabels();

    QVERIFY(window.m_chrootShellHeading);
    QVERIFY(window.m_chrootShellNotice);
    QVERIFY(window.m_chrootShellWarning);
    QVERIFY2(window.m_chrootShellRunButton->isEnabled(),
             "a resolved running host must enable the host shell");
    QCOMPARE(window.m_tabs->tabText(4), QStringLiteral("Host Shell"));
    QCOMPARE(window.m_chrootShellHeading->text(), QStringLiteral("Host shell"));
    QVERIFY(window.m_chrootShellNotice->text().contains(QStringLiteral("running host")));
    QVERIFY(window.m_chrootShellRunButton->toolTip().contains(QStringLiteral("running host")));
    QVERIFY(window.m_chrootShellCommandEdit->placeholderText().contains(QStringLiteral("running host")));
    QVERIFY(window.m_chrootShellCommandEdit->accessibleName().contains(QStringLiteral("Host shell")));
    QVERIFY(window.m_chrootShellWarning->text().contains(QStringLiteral("running host")));
    QVERIFY(window.m_chrootShellTargetLabel->text().contains(QStringLiteral("Running Host")));
    QVERIFY(window.m_chrootShellTargetLabel->text().contains(QStringLiteral("/dev/test-host")));

    // Committing an ordinary repair target restores the chroot labels.
    window.m_hostMaintenanceMode = false;
    prepareRepairScope(window);
    window.updateTargetLabels();
    QCOMPARE(window.m_tabs->tabText(4), QStringLiteral("Chroot Shell"));
    QCOMPARE(window.m_chrootShellHeading->text(), QStringLiteral("Chroot shell"));
    QVERIFY(window.m_chrootShellNotice->text().contains(QStringLiteral("repair system")));
    // The chroot shell cannot answer prompts: the UI must say so and point at
    // the non-interactive flag instead of letting a command hang.
    QVERIFY(window.m_chrootShellNotice->text().contains(QStringLiteral("interactive prompts")));
    QVERIFY(window.m_chrootShellNotice->text().contains(QStringLiteral("dnf update -y")));
    QVERIFY(window.m_chrootShellRunButton->toolTip().contains(QStringLiteral("repair system")));
    QVERIFY(window.m_chrootShellCommandEdit->placeholderText().contains(QStringLiteral("update-grub")));
    QVERIFY(window.m_chrootShellCommandEdit->accessibleName().contains(QStringLiteral("Chroot shell")));
    QVERIFY2(window.m_chrootShellRunButton->isEnabled(),
             "a resolved repair target must enable the chroot shell");
    QCOMPARE(window.m_chrootShellTargetLabel->text(), QStringLiteral("Target: /dev/test-system"));
}

void MainWindowUiTest::hostShellDispatchesAndHandlesFailureModes()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window, true);
    cacheRepairEvidence(window, capabilityEvidence(false, true));
    window.updateTargetLabels();
    QVERIFY(!window.m_hostDiagnosticCache.isEmpty());

    QTemporaryDir captureDir;
    QVERIFY(captureDir.isValid());
    const QString capturePath = captureDir.path() + QStringLiteral("/request.txt");
    QVERIFY(startFakePrivilegedSession(window, capturePath, QStringLiteral("success")));

    window.m_chrootShellCommandEdit->setText(QStringLiteral("apt update"));
    window.runChrootShellCommand();

    QCOMPARE(capturedHelperArguments(capturePath),
             QStringList({QStringLiteral("host-shell"), QStringLiteral("/dev/test-host"),
                          QStringLiteral("/dev/test-host-root"), QStringLiteral("apt update")}));
    const QString shellOutput = window.m_chrootShellOutput->toPlainText();
    QVERIFY(shellOutput.contains(QStringLiteral("mock host command output")));
    QVERIFY(shellOutput.contains(QStringLiteral("[exit 0]")));
    QVERIFY(window.m_actionLogEntries.join(QLatin1Char('\n')).contains(QStringLiteral("Host shell command completed")));

    // A modifying host command invalidates cached host diagnostics and
    // schedules the existing automatic evidence refresh.
    QVERIFY(window.m_hostDiagnosticCache.isEmpty());
    QVERIFY(window.m_evidenceRefreshTimer);
    QVERIFY(window.m_evidenceRefreshTimer->isActive());

    // --- merged from hostShellAptUpdateWithoutRepositoryDataKeepsHostEvidence() ---
    {
        MainWindow window;
        window.show();
        QTest::qWait(50);
        window.m_snapshotPreloadScheduled = true;
        prepareRepairScope(window, true);
        cacheRepairEvidence(window, capabilityEvidence(false, true));
        window.updateTargetLabels();

        QTemporaryDir captureDir;
        QVERIFY(captureDir.isValid());
        const QString capturePath = captureDir.path() + QStringLiteral("/request.txt");
        QVERIFY(startFakePrivilegedSession(window, capturePath, QStringLiteral("aptfail")));

        window.m_chrootShellCommandEdit->setText(QStringLiteral("apt update"));
        window.runChrootShellCommand();

        QCOMPARE(capturedHelperArguments(capturePath).value(0), QStringLiteral("host-shell"));
        QVERIFY2(!window.m_hostDiagnosticCache.isEmpty(),
                 "an apt update that received no repository data must keep cached host diagnostics");
        QVERIFY(window.m_actionLogEntries.join(QLatin1Char('\n')).contains(QStringLiteral("no repository data")));
    }

    // --- merged from hostShellTruncatedResponseFailsSafely() ---
    {
        MainWindow window;
        window.show();
        QTest::qWait(50);
        window.m_snapshotPreloadScheduled = true;
        prepareRepairScope(window, true);
        cacheRepairEvidence(window, capabilityEvidence(false, true));
        window.updateTargetLabels();

        QTemporaryDir captureDir;
        QVERIFY(captureDir.isValid());
        const QString capturePath = captureDir.path() + QStringLiteral("/request.txt");
        QVERIFY(startFakePrivilegedSession(window, capturePath, QStringLiteral("truncated")));

        window.m_chrootShellCommandEdit->setText(QStringLiteral("apt update"));
        acceptNextMessageBox(&window, QMessageBox::Ok);
        // A helper response that ends without a DONE record must fail safely:
        // no crash, a clear error in the shell pane and the application log.
        window.runChrootShellCommand();

        QCOMPARE(capturedHelperArguments(capturePath).value(0), QStringLiteral("host-shell"));
        const QString shellOutput = window.m_chrootShellOutput->toPlainText();
        QVERIFY(shellOutput.contains(QStringLiteral("partial helper output")));
        QVERIFY(shellOutput.contains(QStringLiteral("protocol DONE record")));
        QVERIFY(shellOutput.contains(QStringLiteral("[command failed]")));
        QVERIFY(window.m_actionLogEntries.join(QLatin1Char('\n')).contains(QStringLiteral("protocol DONE record")));
        // A truncated response is not a completed command: host evidence is stale.
        QVERIFY(window.m_hostDiagnosticCache.isEmpty());
    }
}

// The helper maps a distribution-disabled plain apt upgrade to a full-upgrade
// retry and emits a stable mapping line. That line must be visible in the
// Host/Chroot shell pane and in the application log so the user can see why
// the requested transaction ran as full-upgrade.
void MainWindowUiTest::shellAptUpgradePolicyRetryLineIsRendered()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window, true);
    cacheRepairEvidence(window, capabilityEvidence(false, true));
    window.updateTargetLabels();

    QTemporaryDir captureDir;
    QVERIFY(captureDir.isValid());
    const QString capturePath = captureDir.path() + QStringLiteral("/request.txt");
    QVERIFY(startFakePrivilegedSession(window, capturePath, QStringLiteral("aptupgrade")));

    window.m_chrootShellCommandEdit->setText(QStringLiteral("apt upgrade"));
    window.runChrootShellCommand();

    QCOMPARE(capturedHelperArguments(capturePath).value(0), QStringLiteral("host-shell"));
    const QString mappingLine = QStringLiteral(
        "apt upgrade is disabled by this distribution; running 'apt full-upgrade' instead");
    const QString shellOutput = window.m_chrootShellOutput->toPlainText();
    QVERIFY2(shellOutput.contains(mappingLine), qPrintable(shellOutput));
    QVERIFY(shellOutput.contains(QStringLiteral("mock full-upgrade completed")));
    QVERIFY(shellOutput.contains(QStringLiteral("[exit 0]")));
    const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QVERIFY2(log.contains(mappingLine), qPrintable(log));
}

// A same-scope privileged request that arrives while another operation owns
// the helper is refused, never queued, and the refusal must name the concrete
// next step instead of leaving the user with a dead end.
void MainWindowUiTest::privilegedRefusalNamesNextStep()
{
    MainWindow window;
    window.m_privilegedOperationActive = true;
    window.m_privilegedOperationScopeKey = QStringLiteral("/dev/test-system");
    window.m_privilegedOperationTitle = QStringLiteral("Run running-host diagnostics");

    bool succeeded = true;
    const QString response = window.runPrivilegedRequest(
        QStringLiteral("Host shell"),
        {QStringLiteral("host-shell"), QStringLiteral("/dev/test-system"),
         QStringLiteral("/dev/test-root"), QStringLiteral("apt update")},
        QByteArray(), &succeeded, false, MainWindow::LogEntryKind::HostShell);
    QVERIFY(!succeeded);
    QVERIFY2(response.contains(QStringLiteral("is still running and was not queued")), qPrintable(response));
    QVERIFY2(response.contains(QStringLiteral("Wait for it to finish and retry, or cancel it.")), qPrintable(response));
    const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QVERIFY2(log.contains(QStringLiteral("Wait for it to finish and retry, or cancel it.")), qPrintable(log));
}

void MainWindowUiTest::targetShellDispatchStillUsesShell()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.resize(1180, 760);
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window);
    window.updateTargetLabels();
    QVERIFY(window.m_chrootShellRunButton->isEnabled());
    QCOMPARE(window.m_tabs->tabText(4), QStringLiteral("Chroot Shell"));

    QTemporaryDir captureDir;
    QVERIFY(captureDir.isValid());
    const QString capturePath = captureDir.path() + QStringLiteral("/request.txt");
    QVERIFY(startFakePrivilegedSession(window, capturePath, QStringLiteral("success")));

    window.m_chrootShellCommandEdit->setText(QStringLiteral("update-grub"));
    window.runChrootShellCommand();

    QCOMPARE(capturedHelperArguments(capturePath),
             QStringList({QStringLiteral("shell"), QStringLiteral("/dev/test-system"),
                          QStringLiteral("/dev/test-root"), QStringLiteral("update-grub")}));
    QVERIFY(window.m_chrootShellOutput->toPlainText().contains(QStringLiteral("[exit 0]")));
}

// A chroot-shell command that aborts on an interactive prompt (for example
// "dnf update" ending in "Is this ok [y/N]:") must surface the helper's
// actionable non-interactive hint and the failure message in both the shell
// pane and the application log. The global busy indicator must stay active for
// the complete chroot-shell request and stop when it returns.
void MainWindowUiTest::chrootShellInteractivePromptHintAndBusyCoverage()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window);
    window.updateTargetLabels();
    QVERIFY(window.m_chrootShellRunButton->isEnabled());

    QTemporaryDir captureDir;
    QVERIFY(captureDir.isValid());
    const QString capturePath = captureDir.path() + QStringLiteral("/request.txt");
    QVERIFY(startFakePrivilegedSession(window, capturePath, QStringLiteral("prompt")));

    bool busyDuringRequest = false;
    QString busyLabelDuringRequest;
    QTimer poll;
    poll.setInterval(5);
    QObject::connect(&poll, &QTimer::timeout, &window, [&] {
        if (!window.m_busyIndicator->isHidden()) {
            busyDuringRequest = true;
            busyLabelDuringRequest = window.m_busyStatusLabel->text();
        }
    });
    poll.start();

    window.m_chrootShellCommandEdit->setText(QStringLiteral("dnf update"));
    window.runChrootShellCommand();
    poll.stop();

    const QString shellOutput = window.m_chrootShellOutput->toPlainText();
    QVERIFY2(shellOutput.contains(QStringLiteral("Is this ok [y/N]")),
             "the interactive prompt evidence must stay visible in the shell pane");
    QVERIFY2(shellOutput.contains(QStringLiteral("non-interactive flag")),
             "the actionable non-interactive hint must reach the shell pane");
    QVERIFY2(shellOutput.contains(QStringLiteral("dnf update -y")),
             "the hint must name the concrete non-interactive command form");
    QVERIFY2(shellOutput.contains(QStringLiteral("[command failed]")),
             "the aborted command must be reported as failed");

    QVERIFY2(busyDuringRequest,
             "the busy indicator must be active for the whole chroot-shell request");
    QVERIFY2(busyLabelDuringRequest.contains(QStringLiteral("shell"), Qt::CaseInsensitive),
             "the busy label must identify the running shell request");
    QVERIFY2(window.m_busyIndicator->isHidden(),
             "the busy indicator must hide after the chroot-shell request");
    QVERIFY(!window.m_busyProgress->isAnimating());

    const QString log = window.m_actionLogEntries.join(QLatin1Char('\n'));
    QVERIFY2(log.contains(QStringLiteral("non-interactive flag")),
             "the actionable hint must be recorded in the application log");
    QVERIFY2(log.contains(QStringLiteral("Chroot shell command failed: dnf update")),
             "the failure message must be recorded in the application log");
}

void MainWindowUiTest::shellReadinessGateIsSharedByButtonAndReturnPressed()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;

    // Host maintenance selected but the running-host identity is unresolved:
    // the shell must stay unavailable for both the button and Enter.
    window.m_hostMaintenanceMode = true;
    window.m_hostPrimaryPath.clear();
    window.m_hostPrimaryComponentPath.clear();
    window.m_previewTargetPath.clear();
    window.m_previewTargetComponentPath.clear();
    window.updateTargetLabels();
    QVERIFY(!window.m_chrootShellRunButton->isEnabled());

    QTemporaryDir captureDir;
    QVERIFY(captureDir.isValid());
    const QString capturePath = captureDir.path() + QStringLiteral("/request.txt");
    QVERIFY(startFakePrivilegedSession(window, capturePath, QStringLiteral("success")));

    window.m_chrootShellCommandEdit->setText(QStringLiteral("apt update"));
    acceptNextMessageBox(&window, QMessageBox::Ok);
    QTest::keyClick(window.m_chrootShellCommandEdit, Qt::Key_Return);
    QTest::qWait(50);
    QVERIFY2(QFileInfo(capturePath).size() == 0,
             "returnPressed must honor the same readiness gate as the disabled Run button");
    QVERIFY(!window.m_chrootShellOutput->toPlainText().contains(QStringLiteral("[running")));

    // A resolved host enables the button, and the same command dispatches.
    prepareRepairScope(window, true);
    window.updateTargetLabels();
    QVERIFY(window.m_chrootShellRunButton->isEnabled());
    window.m_chrootShellCommandEdit->setText(QStringLiteral("apt update"));
    window.m_chrootShellRunButton->click();
    QVERIFY(QFileInfo(capturePath).size() > 0);
    QCOMPARE(capturedHelperArguments(capturePath).value(0), QStringLiteral("host-shell"));
}

void MainWindowUiTest::chrootShellReleaseInfoChangeRetryOnAccept()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window);
    window.updateTargetLabels();

    QTemporaryDir captureDir;
    QVERIFY(captureDir.isValid());
    const QString capturePath = captureDir.path() + QStringLiteral("/request.txt");
    QVERIFY(startReleaseInfoChangeFakePrivilegedSession(window, capturePath));

    const int logEntriesBefore = window.m_actionLogEntries.size();
    window.m_chrootShellCommandEdit->setText(QStringLiteral("apt-get update"));
    NextMessageBoxCapture prompt(&window, QMessageBox::Yes);
    window.runChrootShellCommand();

    // The prompt is required and names the affected repository, the accepted
    // option and the fact that acceptance is confined to this retry.
    QVERIFY2(prompt.appeared, "a release-info change on an apt command must prompt before retrying");
    QVERIFY(prompt.text.contains(QStringLiteral("txos.tuxedocomputers.com")));
    QVERIFY(prompt.informativeText.contains(QStringLiteral("Origin changed from 'TUXEDO Computers' to 'TUXEDO'")));
    QVERIFY(prompt.informativeText.contains(QStringLiteral("Acquire::AllowReleaseInfoChange=true")));
    QVERIFY(prompt.informativeText.contains(QStringLiteral("this retry only")));

    // The original command and the retried command are separate helper
    // requests, and the option is inserted directly after the executable.
    const QList<QStringList> requests = capturedHelperRequests(capturePath);
    QCOMPARE(requests.size(), 2);
    QCOMPARE(requests.at(0).value(3), QStringLiteral("apt-get update"));
    QCOMPARE(requests.at(1).value(3),
             QStringLiteral("apt-get -o Acquire::AllowReleaseInfoChange=true update"));

    // The complete transcript keeps the first failure and shows the retry
    // result; the final result is reported as success.
    const QString shellOutput = window.m_chrootShellOutput->toPlainText();
    QVERIFY(shellOutput.contains(QStringLiteral("[command failed]")));
    QVERIFY(shellOutput.contains(QStringLiteral("[exit 0]")));
    QVERIFY(shellOutput.indexOf(QStringLiteral("[command failed]")) < shellOutput.indexOf(QStringLiteral("[exit 0]")));
    QVERIFY(shellOutput.contains(QStringLiteral("mock apt update completed after accepting the metadata change")));

    const QString log = window.m_actionLogEntries
        .mid(0, window.m_actionLogEntries.size() - logEntriesBefore).join(QLatin1Char('\n'));
    QVERIFY(log.contains(QStringLiteral("Chroot shell command failed: apt-get update")));
    QVERIFY(log.contains(QStringLiteral("Chroot shell finished with exit code 100 (error).")));
    QVERIFY(log.contains(QStringLiteral("retrying once with Acquire::AllowReleaseInfoChange=true")));
    QVERIFY(log.contains(QStringLiteral(
        "Chroot shell command completed: apt-get -o Acquire::AllowReleaseInfoChange=true update")));
    QVERIFY(log.contains(QStringLiteral("Chroot shell finished with exit code 0 (success).")));
}

void MainWindowUiTest::chrootShellReleaseInfoChangeRetryDeclinedKeepsFailure()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window);
    window.updateTargetLabels();

    QTemporaryDir captureDir;
    QVERIFY(captureDir.isValid());
    const QString capturePath = captureDir.path() + QStringLiteral("/request.txt");
    QVERIFY(startReleaseInfoChangeFakePrivilegedSession(window, capturePath));

    const int logEntriesBefore = window.m_actionLogEntries.size();
    window.m_chrootShellCommandEdit->setText(QStringLiteral("apt update"));
    NextMessageBoxCapture prompt(&window, QMessageBox::No);
    window.runChrootShellCommand();

    QVERIFY(prompt.appeared);
    // Declining keeps the original failure: exactly one request, no option and
    // no success marker anywhere.
    const QList<QStringList> requests = capturedHelperRequests(capturePath);
    QCOMPARE(requests.size(), 1);
    QCOMPARE(requests.at(0).value(3), QStringLiteral("apt update"));
    const QString shellOutput = window.m_chrootShellOutput->toPlainText();
    QVERIFY(shellOutput.contains(QStringLiteral("[command failed]")));
    QVERIFY(!shellOutput.contains(QStringLiteral("[exit 0]")));
    QVERIFY(!shellOutput.contains(QStringLiteral("Acquire::AllowReleaseInfoChange=true")));

    const QString log = window.m_actionLogEntries
        .mid(0, window.m_actionLogEntries.size() - logEntriesBefore).join(QLatin1Char('\n'));
    QVERIFY(log.contains(QStringLiteral("Chroot shell command failed: apt update")));
    QVERIFY(log.contains(QStringLiteral("was not accepted; the original failure stands")));
    QVERIFY(!log.contains(QStringLiteral("retrying once with Acquire::AllowReleaseInfoChange=true")));
}

void MainWindowUiTest::hostShellReleaseInfoChangeRetryOnlyForAptFamily()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window, true);
    window.updateTargetLabels();

    QTemporaryDir captureDir;
    QVERIFY(captureDir.isValid());
    const QString capturePath = captureDir.path() + QStringLiteral("/request.txt");
    QVERIFY(startReleaseInfoChangeFakePrivilegedSession(window, capturePath));

    // The fake transcript contains release-info-change errors, but update-grub
    // is not an apt-family command: no prompt and no retry may occur.
    window.m_chrootShellCommandEdit->setText(QStringLiteral("update-grub"));
    NextMessageBoxCapture prompt(&window, QMessageBox::No);
    window.runChrootShellCommand();
    prompt.stop();

    QVERIFY2(!prompt.appeared, "a non-apt command must never be offered the release-info-change retry");
    const QList<QStringList> requests = capturedHelperRequests(capturePath);
    QCOMPARE(requests.size(), 1);
    QCOMPARE(requests.at(0).value(0), QStringLiteral("host-shell"));
    QCOMPARE(requests.at(0).value(3), QStringLiteral("update-grub"));
    QVERIFY(window.m_chrootShellOutput->toPlainText().contains(QStringLiteral("[command failed]")));
    QVERIFY(!window.m_chrootShellOutput->toPlainText().contains(QStringLiteral("[exit 0]")));
}

void MainWindowUiTest::aptReleaseInfoChangeCommandShapes()
{
    // Detection is case-insensitive and covers all five release-info fields.
    QVERIFY(MainWindow::outputHasAptReleaseInfoChange(
        QStringLiteral("E: Repository 'r' changed its 'Origin' value from 'a' to 'b'")));
    QVERIFY(MainWindow::outputHasAptReleaseInfoChange(
        QStringLiteral("error: repository 'r' CHANGED ITS 'version' VALUE from '1' to '2'")));
    QVERIFY(MainWindow::outputHasAptReleaseInfoChange(
        QStringLiteral("Repository 'r' changed its 'Suite' value from 'a' to 'b'")));
    QVERIFY(MainWindow::outputHasAptReleaseInfoChange(
        QStringLiteral("Repository 'r' changed its 'Codename' value from 'a' to 'b'")));
    QVERIFY(MainWindow::outputHasAptReleaseInfoChange(
        QStringLiteral("Repository 'r' changed its 'Label' value from 'a' to 'b'")));
    QVERIFY(!MainWindow::outputHasAptReleaseInfoChange(QStringLiteral("Repository 'r' changed its 'Component' value")));
    QVERIFY(!MainWindow::outputHasAptReleaseInfoChange(QStringLiteral("Repository 'r' changed its Origin value")));

    QCOMPARE(MainWindow::aptReleaseInfoChangedRepositories(
                 QStringLiteral("E: Repository 'https://repo/one' changed its 'Origin' value from 'a' to 'b'\n"
                                "E: Repository 'https://repo/two' changed its 'Label' value from 'c' to 'd'\n"
                                "E: Repository 'https://repo/one' changed its 'Label' value from 'c' to 'd'")),
             QStringList({QStringLiteral("https://repo/one"), QStringLiteral("https://repo/two")}));
    QCOMPARE(MainWindow::aptReleaseInfoChangeDetails(
                 QStringLiteral("E: Repository 'https://repo/one' changed its 'Origin' value from 'TUXEDO Computers' to 'TUXEDO'")),
             QStringLiteral("• https://repo/one: Origin changed from 'TUXEDO Computers' to 'TUXEDO'"));

    // The option is inserted directly after the apt executable, before the
    // subcommand; the rest of the command is byte-for-byte unchanged.
    QCOMPARE(MainWindow::aptReleaseInfoChangeRetryCommand(QStringLiteral("apt update")),
             QStringLiteral("apt -o Acquire::AllowReleaseInfoChange=true update"));
    QCOMPARE(MainWindow::aptReleaseInfoChangeRetryCommand(QStringLiteral("apt-get -q update")),
             QStringLiteral("apt-get -o Acquire::AllowReleaseInfoChange=true -q update"));
    QCOMPARE(MainWindow::aptReleaseInfoChangeRetryCommand(QStringLiteral("aptitude safe-upgrade")),
             QStringLiteral("aptitude -o Acquire::AllowReleaseInfoChange=true safe-upgrade"));
    QCOMPARE(MainWindow::aptReleaseInfoChangeRetryCommand(QStringLiteral("/usr/bin/apt-get update")),
             QStringLiteral("/usr/bin/apt-get -o Acquire::AllowReleaseInfoChange=true update"));
    QCOMPARE(MainWindow::aptReleaseInfoChangeRetryCommand(QStringLiteral("apt update --option 'a b'")),
             QStringLiteral("apt -o Acquire::AllowReleaseInfoChange=true update --option 'a b'"));
    // Only the first apt invocation of a compound command receives the option:
    // a metadata refresh is retried, a following package change is not.
    QCOMPARE(MainWindow::aptReleaseInfoChangeRetryCommand(QStringLiteral("apt-get update && apt-get upgrade")),
             QStringLiteral("apt-get -o Acquire::AllowReleaseInfoChange=true update && apt-get upgrade"));

    // Leading sudo/env/nice prefixes and one shell-wrapper level are handled
    // conservatively.
    QCOMPARE(MainWindow::aptReleaseInfoChangeRetryCommand(QStringLiteral("sudo apt-get update")),
             QStringLiteral("sudo apt-get -o Acquire::AllowReleaseInfoChange=true update"));
    QCOMPARE(MainWindow::aptReleaseInfoChangeRetryCommand(QStringLiteral("sudo -n apt update")),
             QStringLiteral("sudo -n apt -o Acquire::AllowReleaseInfoChange=true update"));
    QCOMPARE(MainWindow::aptReleaseInfoChangeRetryCommand(QStringLiteral("env DEBIAN_FRONTEND=noninteractive apt-get update")),
             QStringLiteral("env DEBIAN_FRONTEND=noninteractive apt-get -o Acquire::AllowReleaseInfoChange=true update"));
    QCOMPARE(MainWindow::aptReleaseInfoChangeRetryCommand(QStringLiteral("nice -n 10 apt update")),
             QStringLiteral("nice -n 10 apt -o Acquire::AllowReleaseInfoChange=true update"));
    QCOMPARE(MainWindow::aptReleaseInfoChangeRetryCommand(QStringLiteral("sh -c 'apt update'")),
             QStringLiteral("sh -c 'apt -o Acquire::AllowReleaseInfoChange=true update'"));
    QCOMPARE(MainWindow::aptReleaseInfoChangeRetryCommand(QStringLiteral("sudo bash -lc \"apt-get update\"")),
             QStringLiteral("sudo bash -lc \"apt-get -o Acquire::AllowReleaseInfoChange=true update\""));

    // Shapes intentionally not rewritten: the original failure stays visible
    // instead of an unsafe or surprising rewrite.
    QVERIFY(MainWindow::aptReleaseInfoChangeRetryCommand(QStringLiteral("update-grub")).isEmpty());
    QVERIFY(MainWindow::aptReleaseInfoChangeRetryCommand(QStringLiteral("echo apt update")).isEmpty());
    QVERIFY(MainWindow::aptReleaseInfoChangeRetryCommand(QStringLiteral("command apt update")).isEmpty());
    QVERIFY(MainWindow::aptReleaseInfoChangeRetryCommand(QStringLiteral("sudo -u root apt update")).isEmpty());
    QVERIFY(MainWindow::aptReleaseInfoChangeRetryCommand(QStringLiteral("sh -c \"apt update")).isEmpty());
    QVERIFY(MainWindow::aptReleaseInfoChangeRetryCommand(QStringLiteral("env -S 'apt update'")).isEmpty());
}

void MainWindowUiTest::systemsTabActionButtonsShareColumn()
{
    MainWindow window;
    window.resize(1180, 760);
    window.show();
    QTest::qWait(100);
    QVERIFY(window.m_tabs);
    window.m_tabs->setCurrentIndex(0);
    QCoreApplication::processEvents();
    QTest::qWait(50);

    QVERIFY(window.m_refreshDevicesButton);
    QVERIFY(window.m_hostDefaultButton);
    QWidget *systemsPage = window.m_tabs->currentWidget();
    QVERIFY(systemsPage);

    // Refresh Devices and Make Default are the Systems tab's stacked action
    // column (both right-aligned on the page's action edge). The column
    // convention is checked at the normal window width and again at the
    // minimum supported width, where the same shared width and edges must
    // hold instead of the buttons drifting apart.
    const QList<QPushButton *> stack = {window.m_refreshDevicesButton, window.m_hostDefaultButton};
    QString failure;
    QVERIFY2(buttonsShareColumn(systemsPage, stack, &failure),
             qPrintable(QStringLiteral("normal width: %1").arg(failure)));

    window.resize(480, 500);
    QCoreApplication::processEvents();
    QTest::qWait(100);
    // At the minimum width the host card stacks its identity above the
    // actions and the actions wrap onto a second row. Every host action must
    // stay fully inside the page instead of spilling off the card edge.
    QVERIFY(window.m_hostCardLayout);
    QCOMPARE(window.m_hostCardLayout->direction(), QBoxLayout::TopToBottom);
    const QList<QPushButton *> hostActions = {
        window.m_hostDetailsButton, window.m_hostMaintenanceButton, window.m_hostDefaultButton
    };
    for (QPushButton *button : hostActions) {
        QVERIFY(button);
        const QPoint topLeft(button->mapTo(systemsPage, QPoint(0, 0)));
        QVERIFY2(topLeft.x() >= 0 && topLeft.x() + button->width() <= systemsPage->width(),
                 qPrintable(QStringLiteral("button '%1' is not fully visible at the minimum width "
                                           "(x %2, width %3, page %4)")
                                .arg(button->text())
                                .arg(topLeft.x())
                                .arg(button->width())
                                .arg(systemsPage->width())));
    }
    // Both wrapped rows stay anchored to the card's trailing edge instead of
    // sitting at fixed positions.
    QWidget *actionsHost = window.m_hostActionsLayout->parentWidget();
    QVERIFY(actionsHost);
    const int actionsRight = actionsHost->mapTo(systemsPage, QPoint(actionsHost->width(), 0)).x();
    const QList<QPushButton *> trailingButtons = {
        window.m_hostDetailsButton, window.m_hostDefaultButton
    };
    for (QPushButton *button : trailingButtons) {
        const QPoint topLeft(button->mapTo(systemsPage, QPoint(0, 0)));
        QVERIFY2(qAbs(topLeft.x() + button->width() - actionsRight) <= 4,
                 qPrintable(QStringLiteral("button '%1' is not right-anchored at the minimum width "
                                           "(right %2, anchor %3)")
                                .arg(button->text())
                                .arg(topLeft.x() + button->width())
                                .arg(actionsRight)));
    }
    QVERIFY2(window.m_refreshDevicesButton->isVisible(),
             "Refresh Devices must stay visible at the minimum width");
}

void MainWindowUiTest::stateTogglingButtonsElideAtMinimumWidth()
{
    MainWindow window;
    window.resize(480, 500);
    window.show();
    QTest::qWait(100);
    window.m_snapshotPreloadScheduled = true;

    QVERIFY(window.m_hostMaintenanceButton);
    QVERIFY(window.m_unlockTargetButton);

    // Resolve a protected running host so the maintenance action is enabled,
    // then enter maintenance through the real handler so the longer runtime
    // label and its tooltip are applied exactly as in the application. At the
    // minimum supported width the label cannot fit and must be elided instead
    // of spilling over the icon and the button edge.
    prepareRepairScope(window, true);
    window.m_hostMaintenanceMode = false;
    window.m_uiTestPrivilegedSessionGranted = false;
    window.updateHostSystemSummary({window.m_deviceIndex.value(QStringLiteral("/dev/test-host"))});
    QVERIFY2(window.m_hostMaintenanceButton->isEnabled(),
             "the host maintenance action must stay enabled");
    window.selectHostForMaintenance();
    QVERIFY(window.m_hostMaintenanceMode);
    QCOMPARE(window.m_hostMaintenanceButton->text(), QStringLiteral("Exit Host Maintenance"));
    QVERIFY2(window.m_hostMaintenanceButton->isEnabled(),
             "the host maintenance action must stay enabled while active");
    QCoreApplication::processEvents();
    QTest::qWait(50);

    const QString fullText = window.m_hostMaintenanceButton->text();
    const QFontMetrics metrics(window.m_hostMaintenanceButton->font());
    const QString elided = metrics.elidedText(fullText, Qt::ElideRight,
                                              buttonTextWidth(window.m_hostMaintenanceButton));
    QVERIFY2(elided != fullText,
             "Exit Host Maintenance must be elided at the minimum window width");
    // The full content sizeHint is wider than the standardized column width;
    // the bounded (elided) size hint is the one that must fit, so the label
    // and its icon stay inside the button instead of spilling.
    QVERIFY2(window.m_hostMaintenanceButton->sizeHint().width()
                 > window.m_hostMaintenanceButton->width(),
             "the elision is required because the full label is wider than the button");
    QVERIFY2(boundedButtonSizeHint(window.m_hostMaintenanceButton).width()
                 <= window.m_hostMaintenanceButton->width(),
             "the bounded label size hint must fit the allocated width");
    QVERIFY2(window.m_hostMaintenanceButton->toolTip().contains(fullText),
             "the complete label must remain available in the tooltip");
    QString failure;
    QVERIFY2(buttonTextIsBounded(window.m_hostMaintenanceButton, &failure),
             qPrintable(QStringLiteral("Exit Host Maintenance: %1").arg(failure)));

    // The other runtime-toggling Systems action follows the same convention.
    window.m_unlockTargetButton->setText(QStringLiteral("Already Unlocked"));
    QCoreApplication::processEvents();
    QTest::qWait(50);
    QVERIFY2(buttonTextIsBounded(window.m_unlockTargetButton, &failure),
             qPrintable(QStringLiteral("Already Unlocked: %1").arg(failure)));
}

// Every table/tree sorts on a header click. The targets tree is the special
// case: top-level drives and the partitions inside each drive are ordered by
// the clicked column while the drive/partition identity (selection and
// expansion state) is preserved.
void MainWindowUiTest::sortableDeviceTreeSortsDrivesAndPartitions()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;

    const auto makeDisk = [](const QString &path, const QString &model, quint64 size) {
        DeviceNode disk;
        disk.path = path;
        disk.name = path.section(QLatin1Char('/'), -1);
        disk.type = QStringLiteral("disk");
        disk.model = model;
        disk.sizeBytes = size;
        disk.transport = QStringLiteral("sata");
        return disk;
    };
    const auto makePart = [](const QString &path, const QString &fileSystem, quint64 size,
                             bool installedLinux) {
        DeviceNode part;
        part.path = path;
        part.name = path.section(QLatin1Char('/'), -1);
        part.type = QStringLiteral("part");
        part.fileSystem = fileSystem;
        part.linuxCapableFileSystem = fileSystem != QStringLiteral("vfat");
        part.installedLinux = installedLinux;
        part.sizeBytes = size;
        return part;
    };

    DeviceNode big = makeDisk(QStringLiteral("/dev/test-big"), QStringLiteral("Big Disk"), 2147483648ULL);
    DeviceNode bigEsp = makePart(QStringLiteral("/dev/test-big1"), QStringLiteral("vfat"), 536870912ULL, false);
    DeviceNode bigRoot = makePart(QStringLiteral("/dev/test-big2"), QStringLiteral("btrfs"), 1610612736ULL, true);
    big.children = {bigEsp, bigRoot};
    DeviceNode mid = makeDisk(QStringLiteral("/dev/test-mid"), QStringLiteral("Mid Disk"), 1073741824ULL);
    DeviceNode midRoot = makePart(QStringLiteral("/dev/test-mid1"), QStringLiteral("ext4"), 1000000000ULL, false);
    mid.children = {midRoot};
    DeviceNode small = makeDisk(QStringLiteral("/dev/test-small"), QStringLiteral("Small Disk"), 104857600ULL);
    DeviceNode smallRoot = makePart(QStringLiteral("/dev/test-small1"), QStringLiteral("ext4"), 104857600ULL, false);
    small.children = {smallRoot};

    window.populateDeviceTree({big, mid, small});
    QCoreApplication::processEvents();

    QTreeWidget *tree = window.m_deviceTree;
    QVERIFY(tree);
    QCOMPARE(tree->topLevelItemCount(), 3);
    auto topPath = [tree](int row) {
        return tree->topLevelItem(row)->data(0, Qt::UserRole).toString();
    };
    // Default: repair-likelihood rank ascending, so the installed-Linux drive
    // is first and the two Linux-capable drives follow by name.
    QCOMPARE(tree->header()->sortIndicatorSection(), 1);
    QCOMPARE(tree->header()->sortIndicatorOrder(), Qt::AscendingOrder);
    QCOMPARE(topPath(0), big.path);
    QCOMPARE(topPath(1), mid.path);
    QCOMPARE(topPath(2), small.path);

    QTreeWidgetItem *bigItem = deviceTreeTopLevelItem(window, big.path);
    QVERIFY(bigItem);
    // Keep the drive expanded and one partition selected across the sorts.
    bigItem->setExpanded(true);
    QTreeWidgetItem *bigRootItem = deviceTreeChildItem(bigItem, bigRoot.path);
    QVERIFY(bigRootItem);
    selectDeviceTreeItem(window, bigRootItem);

    // Size ascending must be numeric, not the formatted "1.0 GiB" text:
    // 100 MiB < 1 GiB < 2 GiB.
    QTest::mouseClick(tree->header()->viewport(), Qt::LeftButton, Qt::NoModifier,
                      headerSectionCenter(tree->header(), 3));
    QCoreApplication::processEvents();
    QCOMPARE(tree->header()->sortIndicatorSection(), 3);
    QCOMPARE(tree->header()->sortIndicatorOrder(), Qt::AscendingOrder);
    QCOMPARE(topPath(0), small.path);
    QCOMPARE(topPath(1), mid.path);
    QCOMPARE(topPath(2), big.path);
    bigItem = deviceTreeTopLevelItem(window, big.path);
    QVERIFY(bigItem);
    QVERIFY2(bigItem->isExpanded(), "the expanded drive must stay expanded across a sort");
    QCOMPARE(bigItem->child(0)->data(0, Qt::UserRole).toString(), bigEsp.path);
    QCOMPARE(bigItem->child(1)->data(0, Qt::UserRole).toString(), bigRoot.path);
    QVERIFY2(bigRootItem->isSelected(), "the selected partition must stay selected across a sort");
    QVERIFY2(tree->selectedItems().contains(bigRootItem),
             "selection must follow the partition item, not its old row");

    // The second click reverses the order, drives and partitions alike.
    QTest::mouseClick(tree->header()->viewport(), Qt::LeftButton, Qt::NoModifier,
                      headerSectionCenter(tree->header(), 3));
    QCoreApplication::processEvents();
    QCOMPARE(tree->header()->sortIndicatorOrder(), Qt::DescendingOrder);
    QCOMPARE(topPath(0), big.path);
    QCOMPARE(topPath(1), mid.path);
    QCOMPARE(topPath(2), small.path);
    bigItem = deviceTreeTopLevelItem(window, big.path);
    QVERIFY(bigItem);
    QCOMPARE(bigItem->child(0)->data(0, Qt::UserRole).toString(), bigRoot.path);
    QCOMPARE(bigItem->child(1)->data(0, Qt::UserRole).toString(), bigEsp.path);

    // Status returns to the most-likely-first ranking; the tree exposes a
    // visible indicator for every sort it applies.
    QTest::mouseClick(tree->header()->viewport(), Qt::LeftButton, Qt::NoModifier,
                      headerSectionCenter(tree->header(), 1));
    QCoreApplication::processEvents();
    QCOMPARE(tree->header()->sortIndicatorSection(), 1);
    QCOMPARE(tree->header()->sortIndicatorOrder(), Qt::AscendingOrder);
    QCOMPARE(topPath(0), big.path);
    QVERIFY2(tree->header()->isSortIndicatorShown(), "the active sort indicator must be visible");
}

void MainWindowUiTest::deviceTreeSortIndicatorSurvivesLegacyHeaderState()
{
    // A header state saved before header sorting existed carries
    // sortIndicatorShown=false and non-clickable sections. Restoring it must
    // not hide the indicator or disable header clicks: the device tree always
    // starts at the documented Status-ascending sort with a visible indicator.
    QSettings settings(QStringLiteral("BootRepair"), QStringLiteral("BootRepair"));
    settings.remove(QStringLiteral("systems/headerStateV3"));
    settings.sync();
    {
        QTreeWidget legacy;
        legacy.setColumnCount(6);
        legacy.header()->setSectionsClickable(false);
        legacy.header()->setSortIndicatorShown(false);
        settings.setValue(QStringLiteral("systems/headerStateV3"), legacy.header()->saveState());
        settings.sync();
    }

    MainWindow window;
    window.show();
    QTest::qWait(50);
    QVERIFY(window.m_deviceTree);
    QVERIFY2(window.m_deviceTree->header()->sectionsClickable(),
             "header clicks must stay enabled after a legacy header state restore");
    QVERIFY2(window.m_deviceTree->header()->isSortIndicatorShown(),
             "the sort indicator must stay visible after a legacy header state restore");
    QCOMPARE(window.m_deviceTree->header()->sortIndicatorSection(), 1);
    QCOMPARE(window.m_deviceTree->header()->sortIndicatorOrder(), Qt::AscendingOrder);

    settings.remove(QStringLiteral("systems/headerStateV3"));
    settings.sync();
}

void MainWindowUiTest::sortableSnapshotTableUsesNumericAndDateKeys()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    QList<SnapshotInventoryRow> rows;
    rows.append({QStringLiteral("10"), QStringLiteral("2024-01-01 23:30:00 -0500"),
                 QStringLiteral("pre"), QStringLiteral("Alpha"),
                 QStringLiteral("Linux root snapshot"), QStringLiteral("@/.snapshots/10/snapshot")});
    rows.append({QStringLiteral("9"), QStringLiteral("2024-01-02 00:30:00 +0000"),
                 QStringLiteral("pre"), QStringLiteral("Bravo"),
                 QStringLiteral("Linux root snapshot"), QStringLiteral("@/.snapshots/9/snapshot")});
    rows.append({QStringLiteral("100"), QStringLiteral("2024-01-03 08:00:00 +0000"),
                 QStringLiteral("single"), QStringLiteral("Charlie"),
                 QStringLiteral("Incomplete/non-root snapshot"), QStringLiteral("@/.snapshots/100/snapshot")});
    window.populateSnapshotTable(rows);
    QCoreApplication::processEvents();

    QTableWidget *table = window.m_snapshotTable;
    QVERIFY(table);
    QCOMPARE(table->rowCount(), 3);
    auto idAt = [table](int row) {
        QTableWidgetItem *item = table->item(row, 0);
        return item ? item->text() : QString();
    };
    QCOMPARE(table->horizontalHeader()->sortIndicatorSection(), 1);
    QCOMPARE(table->horizontalHeader()->sortIndicatorOrder(), Qt::DescendingOrder);
    // Default: newest first. The -0500 snapshot was created at
    // 2024-01-02 04:30 UTC, so it must precede the +0000 00:30 UTC one even
    // though its display text sorts earlier.
    QCOMPARE(idAt(0), QStringLiteral("100"));
    QCOMPARE(idAt(1), QStringLiteral("10"));
    QCOMPARE(idAt(2), QStringLiteral("9"));

    // The numeric and date sort keys are stored as real values, not strings.
    QVERIFY2(table->item(0, 0)->data(BootRepairSortKeyRole).metaType() == QMetaType::fromType<qlonglong>(),
             "snapshot numbers must carry a numeric sort key");
    QVERIFY2(table->item(0, 1)->data(BootRepairSortKeyRole).metaType() == QMetaType::fromType<QDateTime>(),
             "snapshot creation times must carry a date sort key");

    // Snapshot numbers sort 9 < 10 < 100 instead of "10" < "100" < "9".
    QTest::mouseClick(table->horizontalHeader()->viewport(), Qt::LeftButton, Qt::NoModifier,
                      headerSectionCenter(table->horizontalHeader(), 0));
    QCoreApplication::processEvents();
    QCOMPARE(table->horizontalHeader()->sortIndicatorSection(), 0);
    QCOMPARE(table->horizontalHeader()->sortIndicatorOrder(), Qt::AscendingOrder);
    QCOMPARE(idAt(0), QStringLiteral("9"));
    QCOMPARE(idAt(1), QStringLiteral("10"));
    QCOMPARE(idAt(2), QStringLiteral("100"));

    QTest::mouseClick(table->horizontalHeader()->viewport(), Qt::LeftButton, Qt::NoModifier,
                      headerSectionCenter(table->horizontalHeader(), 0));
    QCoreApplication::processEvents();
    QCOMPARE(table->horizontalHeader()->sortIndicatorOrder(), Qt::DescendingOrder);
    QCOMPARE(idAt(0), QStringLiteral("100"));
    QCOMPARE(idAt(1), QStringLiteral("10"));
    QCOMPARE(idAt(2), QStringLiteral("9"));

    // Created ascending uses the parsed timestamps: 00:30 UTC, then 04:30 UTC,
    // then the next day.
    QTest::mouseClick(table->horizontalHeader()->viewport(), Qt::LeftButton, Qt::NoModifier,
                      headerSectionCenter(table->horizontalHeader(), 1));
    QCoreApplication::processEvents();
    QCOMPARE(table->horizontalHeader()->sortIndicatorSection(), 1);
    QCOMPARE(table->horizontalHeader()->sortIndicatorOrder(), Qt::AscendingOrder);
    QCOMPARE(idAt(0), QStringLiteral("9"));
    QCOMPARE(idAt(1), QStringLiteral("10"));
    QCOMPARE(idAt(2), QStringLiteral("100"));

    QTest::mouseClick(table->horizontalHeader()->viewport(), Qt::LeftButton, Qt::NoModifier,
                      headerSectionCenter(table->horizontalHeader(), 1));
    QCoreApplication::processEvents();
    QCOMPARE(table->horizontalHeader()->sortIndicatorOrder(), Qt::DescendingOrder);
    QCOMPARE(idAt(0), QStringLiteral("100"));
    QCOMPARE(idAt(1), QStringLiteral("10"));
    QCOMPARE(idAt(2), QStringLiteral("9"));
}

void MainWindowUiTest::sortableRepairToolTreeKeepsWorkflowDefaultAndSorts()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    QTreeWidget *tree = window.m_repairToolTree;
    QVERIFY(tree);
    QCOMPARE(tree->topLevelItemCount(), 13);

    auto toolKeys = [tree] {
        QStringList keys;
        for (int row = 0; row < tree->topLevelItemCount(); ++row) {
            keys << tree->topLevelItem(row)->data(0, Qt::UserRole).toString();
        }
        return keys;
    };
    // The curated Full Repair workflow order is the default and no indicator
    // is shown until the user picks a column.
    QCOMPARE(tree->header()->sortIndicatorSection(), -1);
    QCOMPARE(toolKeys().first(), QStringLiteral("validate"));
    QCOMPARE(toolKeys().last(), QStringLiteral("bootstack"));
    const QStringList workflow = toolKeys();

    QTest::mouseClick(tree->header()->viewport(), Qt::LeftButton, Qt::NoModifier,
                      headerSectionCenter(tree->header(), 0));
    QCoreApplication::processEvents();
    QCOMPARE(tree->header()->sortIndicatorSection(), 0);
    QCOMPARE(tree->header()->sortIndicatorOrder(), Qt::AscendingOrder);
    const QStringList ascending = toolKeys();
    // The click reorders the list into the platform's tool-name collation; the
    // exact collation differs between glibc and musl, so assert the structural
    // contract instead of one locale's ordering.
    QVERIFY2(ascending != workflow, "clicking the Tool header must reorder the list");
    QStringList workflowSorted = workflow;
    QStringList ascendingSorted = ascending;
    std::sort(workflowSorted.begin(), workflowSorted.end());
    std::sort(ascendingSorted.begin(), ascendingSorted.end());
    QCOMPARE(ascendingSorted, workflowSorted);

    QTest::mouseClick(tree->header()->viewport(), Qt::LeftButton, Qt::NoModifier,
                      headerSectionCenter(tree->header(), 0));
    QCoreApplication::processEvents();
    QCOMPARE(tree->header()->sortIndicatorOrder(), Qt::DescendingOrder);
    QStringList reversed = ascending;
    std::reverse(reversed.begin(), reversed.end());
    QCOMPARE(toolKeys(), reversed);
}

void MainWindowUiTest::sortableCapabilityTableDefaultsToFeature()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    QTableWidget *table = window.m_capabilityTable;
    QVERIFY(table);
    const int count = table->rowCount();
    QVERIFY(count > 1);
    QCOMPARE(table->horizontalHeader()->sortIndicatorSection(), 0);
    QCOMPARE(table->horizontalHeader()->sortIndicatorOrder(), Qt::AscendingOrder);

    // Population must never leave a cell empty, even though sorting is active
    // for the next refresh.
    for (int row = 0; row < count; ++row) {
        for (int column = 0; column < table->columnCount(); ++column) {
            QVERIFY2(table->item(row, column),
                     qPrintable(QStringLiteral("capability cell %1/%2 is empty").arg(row).arg(column)));
        }
    }
    auto featureAt = [table](int row) {
        QTableWidgetItem *item = table->item(row, 0);
        return item ? item->text() : QString();
    };
    const QString ascendingFirst = featureAt(0);
    const QString ascendingLast = featureAt(count - 1);

    QTest::mouseClick(table->horizontalHeader()->viewport(), Qt::LeftButton, Qt::NoModifier,
                      headerSectionCenter(table->horizontalHeader(), 0));
    QCoreApplication::processEvents();
    QCOMPARE(table->horizontalHeader()->sortIndicatorOrder(), Qt::DescendingOrder);
    QCOMPARE(featureAt(0), ascendingLast);
    QCOMPARE(featureAt(count - 1), ascendingFirst);

    QTest::mouseClick(table->horizontalHeader()->viewport(), Qt::LeftButton, Qt::NoModifier,
                      headerSectionCenter(table->horizontalHeader(), 0));
    QCoreApplication::processEvents();
    QCOMPARE(table->horizontalHeader()->sortIndicatorOrder(), Qt::AscendingOrder);
    QCOMPARE(featureAt(0), ascendingFirst);
}

// The Selected tool primary action is pinned to the panel header: shrinking
// the window and scrolling the page to the panel must never hide it.
void MainWindowUiTest::repairToolActionButtonStaysPinnedToPanelHeader()
{
    MainWindow window;
    window.resize(640, 480);
    window.show();
    QTest::qWait(50);
    window.m_tabs->setCurrentIndex(2); // Repair
    prepareRepairScope(window);
    window.m_snapshotPreloadScheduled = true;
    cacheRepairEvidence(window, capabilityEvidence(false, true));
    window.m_repairToolTree->setCurrentItem(repairItem(window, QStringLiteral("validate")));
    window.updateRepairToolDetails();
    QCoreApplication::processEvents();

    QVERIFY(window.m_repairToolButton);
    QVERIFY2(window.m_repairToolButton->isEnabled(),
             "Validate must be enabled with fresh capability evidence");
    QVERIFY(window.m_repairSplitter);
    auto *detailBox = qobject_cast<QGroupBox *>(window.m_repairSplitter->widget(1));
    QVERIFY(detailBox);
    auto *scroll = window.findChild<QScrollArea *>(QStringLiteral("repairPageScroll"));
    QVERIFY(scroll);
    QVERIFY(scroll->viewport());

    // The action sits in the header row, above the description body.
    const int buttonTop = window.m_repairToolButton->mapTo(detailBox, QPoint(0, 0)).y();
    const int descriptionTop = window.m_repairToolDescription->mapTo(detailBox, QPoint(0, 0)).y();
    QVERIFY2(buttonTop < descriptionTop,
             "the action button must sit above the tool description in the panel header");

    // Force the panel taller than the short viewport so its body cannot fit;
    // the pinned header action must stay in view while the page scrolls.
    const int viewportHeight = scroll->viewport()->height();
    detailBox->setMinimumHeight(viewportHeight + 140);
    QCoreApplication::processEvents();
    QTest::qWait(30);

    const int panelTop = detailBox->mapTo(scroll->widget(), QPoint(0, 0)).y();
    scroll->verticalScrollBar()->setValue(panelTop);
    QCoreApplication::processEvents();
    QTest::qWait(20);

    QVERIFY2(window.m_repairToolButton->isVisible(), "the action button must stay visible");
    QVERIFY2(window.m_repairToolButton->isEnabled(), "the action button must stay enabled");
    const QRect buttonRect(window.m_repairToolButton->mapTo(scroll->viewport(), QPoint(0, 0)),
                           window.m_repairToolButton->size());
    QVERIFY2(scroll->viewport()->rect().intersects(buttonRect),
             qPrintable(QStringLiteral("the action button at %1,%2 must stay inside the %3x%4 viewport")
                            .arg(buttonRect.x()).arg(buttonRect.y())
                            .arg(scroll->viewport()->width()).arg(scroll->viewport()->height())));
    const int panelBottom = detailBox->mapTo(scroll->viewport(), QPoint(0, detailBox->height())).y();
    QVERIFY2(panelBottom > scroll->viewport()->height(),
             "the panel must extend below the viewport so the header pinning is meaningful");
    QVERIFY2(buttonTop * 2 < detailBox->height(),
             "the action button must stay in the upper half of the panel, not at its bottom");
}

// The snapshot inventory actions follow the same header pattern instead of
// sitting at the bottom of the scrollable page.
void MainWindowUiTest::snapshotActionsStayAboveInventory()
{
    MainWindow window;
    window.resize(640, 480);
    window.show();
    QTest::qWait(50);
    window.m_tabs->setCurrentIndex(3); // Snapshots

    prepareRepairScope(window);
    DeviceNode component = window.m_deviceIndex.value(window.m_previewTargetComponentPath);
    component.fileSystem = QStringLiteral("btrfs");
    component.linuxCapableFileSystem = true;
    component.installedLinux = true;
    window.m_deviceIndex.insert(component.path, component);
    window.m_snapshotPreloadScheduled = true;
    window.updateSnapshotControls();
    QCoreApplication::processEvents();
    QVERIFY2(window.m_snapshotLoadButton->isEnabled(),
             qPrintable(QStringLiteral("Load Snapshots must be enabled for a Btrfs target (tooltip: %1)")
                            .arg(window.m_snapshotLoadButton->toolTip())));

    auto *snapshotBox = qobject_cast<QGroupBox *>(window.m_snapshotSplitter->parentWidget());
    QVERIFY(snapshotBox);
    const int actionsTop = window.m_snapshotLoadButton->mapTo(snapshotBox, QPoint(0, 0)).y();
    const int tableTop = window.m_snapshotTable->mapTo(snapshotBox, QPoint(0, 0)).y();
    QVERIFY2(actionsTop < tableTop,
             "snapshot actions must sit above the inventory table inside the framed panel");
    QVERIFY2(!window.m_snapshotTable->isAncestorOf(window.m_snapshotLoadButton),
             "the actions must not live inside the scrollable inventory");

    // Shrinking the window keeps the header actions visible and enabled.
    window.setMinimumSize(0, 0);
    window.resize(520, 360);
    QCoreApplication::processEvents();
    QTest::qWait(30);
    QVERIFY(window.m_snapshotLoadButton->isVisible());
    QVERIFY(window.m_snapshotLoadButton->isEnabled());
    const int shrunkActionsTop = window.m_snapshotLoadButton->mapTo(snapshotBox, QPoint(0, 0)).y();
    const int shrunkTableTop = window.m_snapshotTable->mapTo(snapshotBox, QPoint(0, 0)).y();
    QVERIFY2(shrunkActionsTop < shrunkTableTop,
             "the actions must stay above the inventory after the window shrinks");
}

// The File Copy primary actions close the page-title row with the scope
// label: title left, scope label, then Preview Changes and Copy and Verify
// ending at the content edge, all vertically centered with the title while the
// window is wide enough. The minimum-width block below asserts the responsive
// fallback (a right-aligned second row) that keeps the title complete.
void MainWindowUiTest::fileCopyHeaderActionsShareTitleRow()
{
    MainWindow window;
    window.resize(1000, 700);
    window.show();
    QTest::qWait(50);
    window.m_tabs->setCurrentIndex(5); // File Copy
    QCoreApplication::processEvents();
    QTest::qWait(30);

    QVERIFY(window.m_fileCopyHeading);
    QVERIFY(window.m_fileCopyTargetLabel);
    QVERIFY(window.m_fileCopyPreviewButton);
    QVERIFY(window.m_fileCopyRunButton);
    // The guarded initial state is unchanged by the header move.
    QVERIFY(!window.m_fileCopyPreviewButton->isEnabled());
    QVERIFY(!window.m_fileCopyRunButton->isEnabled());

    // The actions now live in the page scroll content together with the title,
    // instead of floating in a separate row above it.
    auto *scroll = window.findChild<QScrollArea *>(QStringLiteral("fileCopyPageScroll"));
    QVERIFY(scroll);
    QVERIFY2(scroll->isAncestorOf(window.m_fileCopyPreviewButton),
             "Preview Changes must share the page scroll content with the title");
    QVERIFY2(scroll->isAncestorOf(window.m_fileCopyRunButton),
             "Copy and Verify must share the page scroll content with the title");

    QWidget *content = window.m_fileCopyHeading->parentWidget();
    QVERIFY(content);
    QCOMPARE(window.m_fileCopyTargetLabel->parentWidget(), content);
    QCOMPARE(window.m_fileCopyPreviewButton->parentWidget(), content);
    QCOMPARE(window.m_fileCopyRunButton->parentWidget(), content);

    const auto centerIn = [content](QWidget *widget) {
        return widget->mapTo(content, widget->rect().center());
    };
    const QPoint headingCenter = centerIn(window.m_fileCopyHeading);
    const QPoint scopeCenter = centerIn(window.m_fileCopyTargetLabel);
    const QPoint previewCenter = centerIn(window.m_fileCopyPreviewButton);
    const QPoint runCenter = centerIn(window.m_fileCopyRunButton);
    QVERIFY2(qAbs(headingCenter.y() - scopeCenter.y()) <= 2,
             qPrintable(QStringLiteral("title center y=%1, scope label center y=%2")
                            .arg(headingCenter.y()).arg(scopeCenter.y())));
    QVERIFY2(qAbs(headingCenter.y() - previewCenter.y()) <= 2,
             qPrintable(QStringLiteral("title center y=%1, Preview Changes center y=%2")
                            .arg(headingCenter.y()).arg(previewCenter.y())));
    QVERIFY2(qAbs(headingCenter.y() - runCenter.y()) <= 2,
             qPrintable(QStringLiteral("title center y=%1, Copy and Verify center y=%2")
                            .arg(headingCenter.y()).arg(runCenter.y())));

    // The buttons are on the title row itself, not floating above it.
    const QRect headingRect(window.m_fileCopyHeading->mapTo(content, QPoint(0, 0)),
                            window.m_fileCopyHeading->size());
    const QRect previewRect(window.m_fileCopyPreviewButton->mapTo(content, QPoint(0, 0)),
                            window.m_fileCopyPreviewButton->size());
    const QRect runRect(window.m_fileCopyRunButton->mapTo(content, QPoint(0, 0)),
                        window.m_fileCopyRunButton->size());
    QVERIFY2(previewRect.top() >= headingRect.top() && previewRect.bottom() <= headingRect.bottom(),
             qPrintable(QStringLiteral("Preview Changes %1,%2 %3x%4 is not inside the title row %5,%6 %7x%8")
                            .arg(previewRect.x()).arg(previewRect.y())
                            .arg(previewRect.width()).arg(previewRect.height())
                            .arg(headingRect.x()).arg(headingRect.y())
                            .arg(headingRect.width()).arg(headingRect.height())));
    QVERIFY2(runRect.top() >= headingRect.top() && runRect.bottom() <= headingRect.bottom(),
             qPrintable(QStringLiteral("Copy and Verify %1,%2 %3x%4 is not inside the title row %5,%6 %7x%8")
                            .arg(runRect.x()).arg(runRect.y())
                            .arg(runRect.width()).arg(runRect.height())
                            .arg(headingRect.x()).arg(headingRect.y())
                            .arg(headingRect.width()).arg(headingRect.height())));

    // Left-to-right order: title → scope label → Preview Changes → Copy and Verify.
    QVERIFY2(headingRect.left() < scopeCenter.x(),
             "the scope label must sit right of the title");
    QVERIFY2(scopeCenter.x() < previewCenter.x(),
             "Preview Changes must sit right of the scope label");
    QVERIFY2(previewCenter.x() < runCenter.x(),
             "Copy and Verify must sit right of Preview Changes");
    QVERIFY2(previewRect.right() < runRect.left(),
             "Preview Changes and Copy and Verify must not overlap");

    // The row closes at the content pane edge: the last action's right edge
    // matches the full-width source frame in the same content layout.
    QVERIFY(window.m_fileCopySourceBox);
    const int contentRight = window.m_fileCopySourceBox->mapTo(
        content, QPoint(window.m_fileCopySourceBox->width(), 0)).x();
    const int runRight = window.m_fileCopyRunButton->mapTo(
        content, QPoint(window.m_fileCopyRunButton->width(), 0)).x();
    QVERIFY2(qAbs(runRight - contentRight) <= 2,
             qPrintable(QStringLiteral("Copy and Verify right=%1, content pane right=%2")
                            .arg(runRight).arg(contentRight)));

    // The minimum supported window no longer squeezes the title row: the
    // actions move to a right-aligned second row while the title stays
    // complete and the scope label keeps the first row.
    window.setMinimumSize(0, 0);
    window.resize(480, 500);
    QCoreApplication::processEvents();
    QTest::qWait(30);
    QVERIFY(window.m_fileCopyPreviewButton->isVisible());
    QVERIFY(window.m_fileCopyRunButton->isVisible());
    const QRect smallHeading(window.m_fileCopyHeading->mapTo(content, QPoint(0, 0)),
                             window.m_fileCopyHeading->size());
    const QRect smallScope(window.m_fileCopyTargetLabel->mapTo(content, QPoint(0, 0)),
                           window.m_fileCopyTargetLabel->size());
    const QRect smallPreview(window.m_fileCopyPreviewButton->mapTo(content, QPoint(0, 0)),
                             window.m_fileCopyPreviewButton->size());
    const QRect smallRun(window.m_fileCopyRunButton->mapTo(content, QPoint(0, 0)),
                         window.m_fileCopyRunButton->size());
    QVERIFY2(!smallPreview.intersects(smallRun),
             "the File Copy actions must not overlap at minimum width");
    QVERIFY2(smallPreview.left() < smallRun.left(),
             "the File Copy action order must survive the minimum width");
    QVERIFY2(smallPreview.top() > smallHeading.bottom(),
             "Preview Changes must move to a second row at minimum width");
    QVERIFY2(smallRun.top() > smallHeading.bottom(),
             "Copy and Verify must move to a second row at minimum width");
    QVERIFY2(!smallPreview.intersects(smallScope) && !smallRun.intersects(smallScope),
             "the wrapped File Copy actions must not overlap the scope label");
    const int smallContentRight = window.m_fileCopySourceBox->mapTo(
        content, QPoint(window.m_fileCopySourceBox->width(), 0)).x();
    const int smallRunRight = window.m_fileCopyRunButton->mapTo(
        content, QPoint(window.m_fileCopyRunButton->width(), 0)).x();
    QVERIFY2(qAbs(smallRunRight - smallContentRight) <= 2,
             qPrintable(QStringLiteral("minimum width: Copy and Verify right=%1, content pane right=%2")
                            .arg(smallRunRight).arg(smallContentRight)));
}

// The Diagnostics title row closes with the scope label followed by Run All:
// title left, scope label, then Run All at the content edge, all vertically
// centered with the Diagnostics title.
void MainWindowUiTest::diagnosticsHeaderRunAllFollowsScopeLabel()
{
    MainWindow window;
    window.resize(1000, 700);
    window.show();
    QTest::qWait(50);
    window.m_tabs->setCurrentIndex(1); // Diagnostics
    QCoreApplication::processEvents();
    QTest::qWait(30);

    QVERIFY(window.m_diagnosticScopeLabel);
    QVERIFY(window.m_runAllDiagnosticsButton);
    QCOMPARE(window.m_diagnosticScopeLabel->text(), QStringLiteral("Target: none selected"));

    QWidget *page = window.m_diagnosticScopeLabel->parentWidget();
    QVERIFY(page);
    QLabel *title = directChildLabelWithText(page, QStringLiteral("Diagnostics"));
    QVERIFY2(title, "the Diagnostics section title must exist");
    QCOMPARE(window.m_runAllDiagnosticsButton->parentWidget(), page);
    QCOMPARE(title->parentWidget(), page);

    const auto centerIn = [page](QWidget *widget) {
        return widget->mapTo(page, widget->rect().center());
    };
    const QPoint titleCenter = centerIn(title);
    const QPoint scopeCenter = centerIn(window.m_diagnosticScopeLabel);
    const QPoint runCenter = centerIn(window.m_runAllDiagnosticsButton);
    QVERIFY2(qAbs(titleCenter.y() - scopeCenter.y()) <= 2,
             qPrintable(QStringLiteral("title center y=%1, scope label center y=%2")
                            .arg(titleCenter.y()).arg(scopeCenter.y())));
    QVERIFY2(qAbs(titleCenter.y() - runCenter.y()) <= 2,
             qPrintable(QStringLiteral("title center y=%1, Run All center y=%2")
                            .arg(titleCenter.y()).arg(runCenter.y())));

    const QRect titleRect(title->mapTo(page, QPoint(0, 0)), title->size());
    const QRect runRect(window.m_runAllDiagnosticsButton->mapTo(page, QPoint(0, 0)),
                        window.m_runAllDiagnosticsButton->size());
    QVERIFY2(runRect.top() >= titleRect.top() && runRect.bottom() <= titleRect.bottom(),
             qPrintable(QStringLiteral("Run All %1,%2 %3x%4 is not inside the title row %5,%6 %7x%8")
                            .arg(runRect.x()).arg(runRect.y())
                            .arg(runRect.width()).arg(runRect.height())
                            .arg(titleRect.x()).arg(titleRect.y())
                            .arg(titleRect.width()).arg(titleRect.height())));

    // Left-to-right order: title → scope label → Run All.
    QVERIFY2(titleRect.left() < scopeCenter.x(),
             "the scope label must sit right of the Diagnostics title");
    QVERIFY2(scopeCenter.x() < runCenter.x(),
             "Run All must sit right of the scope label");

    // Run All closes the row at the content pane edge, matching the diagnostic
    // splitter that spans the same page layout.
    QVERIFY(window.m_diagnosticSplitter);
    const int contentRight = window.m_diagnosticSplitter->mapTo(
        page, QPoint(window.m_diagnosticSplitter->width(), 0)).x();
    const int runRight = window.m_runAllDiagnosticsButton->mapTo(
        page, QPoint(window.m_runAllDiagnosticsButton->width(), 0)).x();
    QVERIFY2(qAbs(runRight - contentRight) <= 2,
             qPrintable(QStringLiteral("Run All right=%1, content pane right=%2")
                            .arg(runRight).arg(contentRight)));

    // The minimum supported window keeps the order and centering.
    window.setMinimumSize(0, 0);
    window.resize(480, 500);
    QCoreApplication::processEvents();
    QTest::qWait(30);
    QVERIFY(window.m_runAllDiagnosticsButton->isVisible());
    QVERIFY2(qAbs(centerIn(title).y() - centerIn(window.m_runAllDiagnosticsButton).y()) <= 2,
             "Run All must stay centered on the Diagnostics title row at minimum width");
    QVERIFY2(centerIn(window.m_diagnosticScopeLabel).x()
                 < centerIn(window.m_runAllDiagnosticsButton).x(),
             "Run All must stay right of the scope label at minimum width");
    const int smallContentRight = window.m_diagnosticSplitter->mapTo(
        page, QPoint(window.m_diagnosticSplitter->width(), 0)).x();
    const int smallRunRight = window.m_runAllDiagnosticsButton->mapTo(
        page, QPoint(window.m_runAllDiagnosticsButton->width(), 0)).x();
    QVERIFY2(qAbs(smallRunRight - smallContentRight) <= 2,
             qPrintable(QStringLiteral("minimum width: Run All right=%1, content pane right=%2")
                            .arg(smallRunRight).arg(smallContentRight)));
}

// The Repair and Snapshots headers already follow the same pattern: the scope
// label shares the title row and ends at the content pane edge. This locks the
// consistency the File Copy and Diagnostics headers were aligned to.
void MainWindowUiTest::scopeLabelHeadersStayOnTitleRow()
{
    MainWindow window;
    window.resize(1000, 700);
    window.show();
    QTest::qWait(50);

    struct HeaderSpec {
        int tabIndex;
        QLabel *scopeLabel;
        QString title;
    };
    const QList<HeaderSpec> specs = {
        {2, window.m_repairTargetLabel, QStringLiteral("Repair")},
        {3, window.m_snapshotTargetLabel, QStringLiteral("Btrfs snapshots")},
    };
    for (const HeaderSpec &spec : specs) {
        window.m_tabs->setCurrentIndex(spec.tabIndex);
        QCoreApplication::processEvents();
        QTest::qWait(30);

        QVERIFY2(spec.scopeLabel, qPrintable(spec.title));
        QWidget *content = spec.scopeLabel->parentWidget();
        QVERIFY2(content, qPrintable(spec.title));
        QLabel *title = directChildLabelWithText(content, spec.title);
        QVERIFY2(title, qPrintable(QStringLiteral("missing %1 title").arg(spec.title)));
        QCOMPARE(title->parentWidget(), content);

        const QPoint titleCenter = title->mapTo(content, title->rect().center());
        const QPoint scopeCenter = spec.scopeLabel->mapTo(content, spec.scopeLabel->rect().center());
        QVERIFY2(qAbs(titleCenter.y() - scopeCenter.y()) <= 2,
                 qPrintable(QStringLiteral("%1: title center y=%2, scope label center y=%3")
                                .arg(spec.title).arg(titleCenter.y()).arg(scopeCenter.y())));
        QVERIFY2(titleCenter.x() < scopeCenter.x(),
                 qPrintable(QStringLiteral("%1: the scope label must sit right of the title")
                                .arg(spec.title)));

        // The full-width content pane in the same layout gives the right edge.
        QWidget *contentPane = spec.tabIndex == 2
            ? static_cast<QWidget *>(window.m_repairVerticalSplitter)
            : static_cast<QWidget *>(window.m_snapshotSplitter->parentWidget());
        QVERIFY2(contentPane, qPrintable(spec.title));
        const int scopeRight = spec.scopeLabel->mapTo(
            content, QPoint(spec.scopeLabel->width(), 0)).x();
        const int paneRight = contentPane->mapTo(
            content, QPoint(contentPane->width(), 0)).x();
        QVERIFY2(qAbs(scopeRight - paneRight) <= 2,
                 qPrintable(QStringLiteral("%1: scope label right=%2, content pane right=%3")
                                .arg(spec.title).arg(scopeRight).arg(paneRight)));
    }
}

// The File Copy and Diagnostics headers reflow instead of squeezing the title
// row: the title and help keep the first row, the wrapping scope label and the
// primary actions share the action row (scope left of the buttons, buttons
// right-aligned) while it has room, and the scope plus actions move together to
// a right-aligned second row when the window narrows. The title is never
// truncated and the scope label wraps to its two-line form instead of eliding.
void MainWindowUiTest::responsivePageHeadersWrapActionsAtMinimumWidth()
{
    MainWindow window;
    window.show();
    QTest::qWait(100);
    window.setMinimumSize(0, 0);

    const auto titleFullyShown = [](QLabel *title) {
        return title->fontMetrics().horizontalAdvance(title->text()) <= title->width();
    };
    const auto rectIn = [](QWidget *parent, QWidget *widget) {
        return QRect(widget->mapTo(parent, QPoint(0, 0)), widget->size());
    };
    const auto rightEdgeIn = [](QWidget *parent, QWidget *widget) {
        return widget->mapTo(parent, QPoint(widget->width(), 0)).x();
    };

    // --- File Copy: Host → Repair title with Preview Changes / Copy and Verify ---
    window.m_tabs->setCurrentIndex(5);
    QCoreApplication::processEvents();
    QTest::qWait(30);

    QVERIFY(window.m_fileCopyHeading);
    QVERIFY(window.m_fileCopyTargetLabel);
    QVERIFY(window.m_fileCopyPreviewButton);
    QVERIFY(window.m_fileCopyRunButton);
    QVERIFY(window.m_fileCopySourceBox);
    QWidget *fileCopyContent = window.m_fileCopyHeading->parentWidget();
    QVERIFY(fileCopyContent);
    const QList<QPushButton *> fileCopyActions = {
        window.m_fileCopyPreviewButton, window.m_fileCopyRunButton};

    // Wide: title complete, actions right-aligned on the first row, no overlap
    // with the scope label.
    window.resize(1000, 700);
    QCoreApplication::processEvents();
    QTest::qWait(50);
    QVERIFY2(titleFullyShown(window.m_fileCopyHeading),
             "the File Copy title must be fully shown at a wide width");
    const QRect wideFileCopyTitle = rectIn(fileCopyContent, window.m_fileCopyHeading);
    const QRect wideFileCopyScope = rectIn(fileCopyContent, window.m_fileCopyTargetLabel);
    for (QPushButton *action : fileCopyActions) {
        const QRect rect = rectIn(fileCopyContent, action);
        QVERIFY2(qAbs(rect.center().y() - wideFileCopyTitle.center().y()) <= 2,
                 qPrintable(QStringLiteral("%1 must share the wide title row").arg(action->text())));
        QVERIFY2(!rect.intersects(wideFileCopyScope),
                 qPrintable(QStringLiteral("%1 must not overlap the scope label").arg(action->text())));
        QVERIFY2(rect.left() > wideFileCopyScope.right(),
                 qPrintable(QStringLiteral("%1 must sit right of the scope label").arg(action->text())));
    }
    QVERIFY2(fileCopyActions.at(0)->mapTo(fileCopyContent, QPoint(0, 0)).x()
                 < fileCopyActions.at(1)->mapTo(fileCopyContent, QPoint(0, 0)).x(),
             "the wide File Copy action order must stay Preview Changes then Copy and Verify");
    const int wideFileCopyRight = window.m_fileCopySourceBox->mapTo(
        fileCopyContent, QPoint(window.m_fileCopySourceBox->width(), 0)).x();
    QVERIFY2(qAbs(rightEdgeIn(fileCopyContent, fileCopyActions.constLast()) - wideFileCopyRight) <= 2,
             "the wide File Copy actions must stay right-aligned to the content edge");

    // Narrow: title complete, actions on a right-aligned second row, no overlap
    // with the title or the scope label.
    window.resize(480, 500);
    QCoreApplication::processEvents();
    QTest::qWait(50);
    QVERIFY2(titleFullyShown(window.m_fileCopyHeading),
             "the File Copy title must stay fully shown at the minimum width");
    const QRect narrowFileCopyTitle = rectIn(fileCopyContent, window.m_fileCopyHeading);
    const QRect narrowFileCopyScope = rectIn(fileCopyContent, window.m_fileCopyTargetLabel);
    QVERIFY2(!narrowFileCopyTitle.intersects(narrowFileCopyScope),
             "the File Copy title and scope label must not overlap at minimum width");
    for (QPushButton *action : fileCopyActions) {
        const QRect rect = rectIn(fileCopyContent, action);
        QVERIFY2(rect.top() > narrowFileCopyTitle.bottom(),
                 qPrintable(QStringLiteral("%1 must move to a second row at minimum width").arg(action->text())));
        QVERIFY2(!rect.intersects(narrowFileCopyTitle),
                 qPrintable(QStringLiteral("%1 must not overlap the title").arg(action->text())));
        QVERIFY2(!rect.intersects(narrowFileCopyScope),
                 qPrintable(QStringLiteral("%1 must not overlap the scope label").arg(action->text())));
    }
    QVERIFY2(qAbs(rectIn(fileCopyContent, fileCopyActions.at(0)).center().y()
                  - rectIn(fileCopyContent, fileCopyActions.at(1)).center().y()) <= 2,
             "both wrapped File Copy actions must share the second row");
    const int narrowFileCopyRight = window.m_fileCopySourceBox->mapTo(
        fileCopyContent, QPoint(window.m_fileCopySourceBox->width(), 0)).x();
    QVERIFY2(qAbs(rightEdgeIn(fileCopyContent, fileCopyActions.constLast()) - narrowFileCopyRight) <= 2,
             "the wrapped File Copy actions must stay right-aligned to the content edge");

    // --- Diagnostics: the same pattern with Run All and the shared scope label ---
    window.m_tabs->setCurrentIndex(1);
    QCoreApplication::processEvents();
    QTest::qWait(30);
    QWidget *diagnosticsPage = window.m_diagnosticScopeLabel->parentWidget();
    QVERIFY(diagnosticsPage);
    QLabel *diagnosticsTitle = directChildLabelWithText(diagnosticsPage, QStringLiteral("Diagnostics"));
    QVERIFY(diagnosticsTitle);
    QVERIFY(window.m_runAllDiagnosticsButton);
    QVERIFY(window.m_diagnosticSplitter);

    // A long running-host scope is the crowded case: the scope must wrap in
    // full (never elide away) and stay on the same row as Run All.
    window.m_diagnosticScopeLabel->setText(QStringLiteral(
        "Host maintenance: /dev/disk/by-id/nvme-eui.11111111222233334444555566667777"));
    QCoreApplication::processEvents();

    window.resize(1000, 700);
    QCoreApplication::processEvents();
    QTest::qWait(50);
    QVERIFY2(titleFullyShown(diagnosticsTitle),
             "the Diagnostics title must be fully shown at a wide width");
    const QRect wideDiagnosticsTitle = rectIn(diagnosticsPage, diagnosticsTitle);
    const QRect wideDiagnosticsScope = rectIn(diagnosticsPage, window.m_diagnosticScopeLabel);
    const QRect wideRunAll = rectIn(diagnosticsPage, window.m_runAllDiagnosticsButton);
    QVERIFY2(qAbs(wideRunAll.center().y() - wideDiagnosticsTitle.center().y()) <= 2,
             "Run All must share the wide Diagnostics title row");
    QVERIFY2(!wideRunAll.intersects(wideDiagnosticsScope),
             "Run All must not overlap the Diagnostics scope label");
    QVERIFY2(wideRunAll.left() > wideDiagnosticsScope.right(),
             "Run All must sit right of the Diagnostics scope label");
    const int wideDiagnosticsRight = window.m_diagnosticSplitter->mapTo(
        diagnosticsPage, QPoint(window.m_diagnosticSplitter->width(), 0)).x();
    QVERIFY2(qAbs(rightEdgeIn(diagnosticsPage, window.m_runAllDiagnosticsButton) - wideDiagnosticsRight) <= 2,
             "Run All must stay right-aligned to the Diagnostics content edge");

    window.resize(480, 500);
    QCoreApplication::processEvents();
    QTest::qWait(50);
    QVERIFY2(titleFullyShown(diagnosticsTitle),
             "the Diagnostics title must stay fully shown at the minimum width");
    QVERIFY2(window.m_diagnosticScopeLabel->wordWrap(),
             "the Diagnostics scope label must wrap instead of eliding");
    const QRect narrowDiagnosticsTitle = rectIn(diagnosticsPage, diagnosticsTitle);
    const QRect narrowDiagnosticsScope = rectIn(diagnosticsPage, window.m_diagnosticScopeLabel);
    const QRect narrowRunAll = rectIn(diagnosticsPage, window.m_runAllDiagnosticsButton);
    QVERIFY2(!narrowDiagnosticsTitle.intersects(narrowDiagnosticsScope),
             "the Diagnostics title and scope label must not overlap at minimum width");
    QVERIFY2(narrowDiagnosticsScope.width() >= window.m_diagnosticScopeLabel->sizeHint().width(),
             qPrintable(QStringLiteral("the long Diagnostics scope must keep its wrapped width: %1 < %2")
                            .arg(narrowDiagnosticsScope.width())
                            .arg(window.m_diagnosticScopeLabel->sizeHint().width())));
    QVERIFY2(narrowDiagnosticsScope.height()
                 >= window.m_diagnosticScopeLabel->heightForWidth(narrowDiagnosticsScope.width()),
             "the Diagnostics scope must show every wrapped line");
    QVERIFY2(qAbs(narrowRunAll.center().y() - narrowDiagnosticsScope.center().y()) <= 2,
             "Run All must share the scope row at minimum width");
    QVERIFY2(!narrowRunAll.intersects(narrowDiagnosticsTitle),
             "Run All must not overlap the Diagnostics title");
    QVERIFY2(!narrowRunAll.intersects(narrowDiagnosticsScope),
             "Run All must not overlap the Diagnostics scope label");
    const int narrowDiagnosticsRight = window.m_diagnosticSplitter->mapTo(
        diagnosticsPage, QPoint(window.m_diagnosticSplitter->width(), 0)).x();
    QVERIFY2(qAbs(rightEdgeIn(diagnosticsPage, window.m_runAllDiagnosticsButton) - narrowDiagnosticsRight) <= 2,
             "Run All must stay right-aligned to the Diagnostics content edge at minimum width");
}

// Every tab that shows the scope label uses the same wrapping label, and the
// File Copy header keeps it visible beside its actions: "Host maintenance:
// /dev/test-host" wraps into the two-line form, stays on the same row as
// Preview Changes and Copy and Verify at normal and minimum widths, and never
// elides the value while that two-line form fits. At the minimum width the
// actions wrap below the title and the scope label wraps with them instead of
// staying behind on the title row squeezed to nothing.
void MainWindowUiTest::scopeLabelWrapsToTwoLinesBesideActions()
{
    MainWindow window;
    window.resize(1000, 700);
    window.show();
    QTest::qWait(50);
    window.setMinimumSize(0, 0);

    prepareRepairScope(window, /*host=*/true);
    window.updateTargetLabels();
    window.m_tabs->setCurrentIndex(5); // File Copy
    QCoreApplication::processEvents();
    QTest::qWait(30);

    QVERIFY(window.m_fileCopyHeading);
    QVERIFY(window.m_fileCopyTargetLabel);
    QVERIFY(window.m_fileCopyPreviewButton);
    QVERIFY(window.m_fileCopyRunButton);
    QCOMPARE(window.m_fileCopyTargetLabel->text(),
             QStringLiteral("Host maintenance: /dev/test-host"));

    // Consistency: every tab that shows the scope label wraps it the same way
    // instead of eliding the value away.
    const QList<QLabel *> scopeLabels = {
        window.m_fileCopyTargetLabel, window.m_snapshotTargetLabel,
        window.m_repairTargetLabel, window.m_diagnosticScopeLabel,
        window.m_chrootShellTargetLabel};
    for (QLabel *label : scopeLabels) {
        QVERIFY2(label, "every tab scope label must exist");
        QVERIFY2(label->wordWrap(),
                 qPrintable(QStringLiteral("scope label must wrap: %1").arg(label->text())));
        QCOMPARE(label->alignment(), Qt::Alignment(Qt::AlignRight | Qt::AlignVCenter));
        QCOMPARE(label->sizePolicy().horizontalPolicy(), QSizePolicy::Preferred);
    }

    const QFontMetrics metrics(window.m_fileCopyTargetLabel->font());
    const QString firstLine = QStringLiteral("Host maintenance:");
    const QString secondLine = QStringLiteral("/dev/test-host");
    const int twoLineWidth = qMax(metrics.horizontalAdvance(firstLine),
                                  metrics.horizontalAdvance(secondLine));

    QWidget *content = window.m_fileCopyHeading->parentWidget();
    QVERIFY(content);
    const auto rectIn = [content](QWidget *widget) {
        return QRect(widget->mapTo(content, QPoint(0, 0)), widget->size());
    };

    struct WidthCase {
        int width;
        bool actionsStacked;
        QString description;
    };
    const QList<WidthCase> cases = {
        {1000, false, QStringLiteral("normal width")},
        {480, true, QStringLiteral("minimum width")},
    };
    for (const WidthCase &testCase : cases) {
        window.resize(testCase.width, 700);
        QCoreApplication::processEvents();
        QTest::qWait(50);

        const QRect heading = rectIn(window.m_fileCopyHeading);
        const QRect scope = rectIn(window.m_fileCopyTargetLabel);
        const QRect preview = rectIn(window.m_fileCopyPreviewButton);
        const QRect run = rectIn(window.m_fileCopyRunButton);
        const int scopeWrappedHeight =
            window.m_fileCopyTargetLabel->heightForWidth(scope.width());

        // The two-line form fits in full: both wrapped lines have their
        // complete width and the allocated height covers every line, so the
        // scope value is visible instead of elided.
        QVERIFY2(scope.width() >= twoLineWidth,
                 qPrintable(QStringLiteral("%1: scope width %2 cannot fit the two-line form %3")
                                .arg(testCase.description).arg(scope.width()).arg(twoLineWidth)));
        QVERIFY2(scopeWrappedHeight >= 2 * metrics.lineSpacing(),
                 qPrintable(QStringLiteral("%1: scope must wrap onto two lines").arg(testCase.description)));
        QVERIFY2(scope.height() >= scopeWrappedHeight,
                 qPrintable(QStringLiteral("%1: scope must not clip a wrapped line").arg(testCase.description)));

        // The scope label and both actions share one row, scope to the left.
        QVERIFY2(qAbs(scope.center().y() - preview.center().y()) <= 2,
                 qPrintable(QStringLiteral("%1: scope and Preview Changes must share a row")
                                .arg(testCase.description)));
        QVERIFY2(qAbs(scope.center().y() - run.center().y()) <= 2,
                 qPrintable(QStringLiteral("%1: scope and Copy and Verify must share a row")
                                .arg(testCase.description)));
        QVERIFY2(scope.right() < preview.left(),
                 qPrintable(QStringLiteral("%1: scope must sit left of Preview Changes")
                                .arg(testCase.description)));
        QVERIFY2(!scope.intersects(preview) && !scope.intersects(run),
                 qPrintable(QStringLiteral("%1: scope must not overlap the actions")
                                .arg(testCase.description)));
        QVERIFY2(!preview.intersects(run) && preview.left() < run.left(),
                 qPrintable(QStringLiteral("%1: the action order must survive")
                                .arg(testCase.description)));

        if (testCase.actionsStacked) {
            // At the minimum width the actions wrap below the title, and the
            // scope stays with them (never left behind and squeezed).
            QVERIFY2(preview.top() > heading.bottom() && run.top() > heading.bottom(),
                     qPrintable(QStringLiteral("%1: the actions must wrap below the title")
                                    .arg(testCase.description)));
        } else {
            QVERIFY2(qAbs(heading.center().y() - preview.center().y()) <= 2,
                     qPrintable(QStringLiteral("%1: the actions must stay on the title row")
                                    .arg(testCase.description)));
        }

        QVERIFY(window.m_fileCopySourceBox);
        const int contentRight = window.m_fileCopySourceBox->mapTo(
            content, QPoint(window.m_fileCopySourceBox->width(), 0)).x();
        QVERIFY2(qAbs(run.right() - contentRight) <= 2,
                 qPrintable(QStringLiteral("%1: the actions must stay right-aligned to the content edge")
                                .arg(testCase.description)));
    }
}

// Running-host snapshot inventory must use the host scope: exactly one
// host-snapshots list request, the target inventory is not reused, and leaving
// Host Maintenance restores the ordinary target preload.
void MainWindowUiTest::hostMaintenanceSnapshotInventoryUsesHostScope()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_autoRefreshDiagnostics->setChecked(false);

    prepareHostSnapshotScope(window);
    window.clearHostRebootRequired();
    installSyntheticBtrfsTarget(window, QStringLiteral("/dev/test-target-a"),
                                QStringLiteral("/dev/test-target-a1"));
    window.m_previewTargetPath = QStringLiteral("/dev/test-target-a");
    window.m_previewTargetComponentPath = QStringLiteral("/dev/test-target-a1");
    window.m_targetDiagnosticCacheIdentity = window.currentTargetDiagnosticCacheIdentity();

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("host-snapshot-scope.log"));
    QVERIFY2(startHostSnapshotFakePrivilegedSession(window, capturePath),
             "the scripted host snapshot session must start");

    window.updateSnapshotControls();
    window.scheduleSnapshotPreload();
    QTRY_VERIFY_WITH_TIMEOUT(window.m_snapshotTable->rowCount() == 1, 5000);
    QVERIFY(window.m_snapshotTable->item(0, 4)->text().startsWith(QStringLiteral("Linux root snapshot")));
    QCOMPARE(capturedHostRequestCount(capturePath, QStringLiteral("host-snapshots"),
                                      QStringLiteral("list")), 1);
    QCOMPARE(capturedHostRequestCount(capturePath, QStringLiteral("snapshots"),
                                      QStringLiteral("list")), 0);

    // Leaving Host Maintenance restores the target preload.
    window.exitHostMaintenanceMode();
    QTRY_VERIFY_WITH_TIMEOUT(window.m_snapshotTable->rowCount() == 1, 5000);
    QCOMPARE(capturedHostRequestCount(capturePath, QStringLiteral("snapshots"),
                                      QStringLiteral("list")), 1);
    QCOMPARE(capturedHostRequestCount(capturePath, QStringLiteral("host-snapshots"),
                                      QStringLiteral("list")), 1);
}

// The consequences dialog names the running host, the reboot requirement, the
// automatic current-state preservation, the /home exclusion and the data-loss
// caveat; Cancel and a wrong typed confirmation produce no rollback request.
void MainWindowUiTest::hostSnapshotRollbackRequiresConsequencesConfirmation()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_autoRefreshDiagnostics->setChecked(false);
    prepareHostSnapshotScope(window);
    window.clearHostRebootRequired();
    QString evidence = capabilityEvidence(true, true);
    evidence += QStringLiteral("Host snapshot rollback: available\nHost reboot: available\n");
    cacheRepairEvidence(window, evidence);

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("host-snapshot-confirm.log"));
    QVERIFY2(startHostSnapshotFakePrivilegedSession(window, capturePath),
             "the scripted host snapshot session must start");

    window.updateSnapshotControls();
    window.loadSnapshots();
    QCOMPARE(window.m_snapshotTable->rowCount(), 1);
    window.m_snapshotTable->setCurrentCell(0, 0);
    window.updateSnapshotControls();
    QVERIFY2(window.m_snapshotRollbackButton->isEnabled(),
             qPrintable(QStringLiteral("host rollback must be enabled with available evidence (tooltip: %1)")
                            .arg(window.m_snapshotRollbackButton->toolTip())));

    // Cancel at the consequences dialog: no rollback request.
    {
        NextMessageBoxCapture capture(&window, QMessageBox::Cancel);
        closeRepairProgressDialogWhenDone(&window);
        window.m_snapshotRollbackButton->click();
        QVERIFY(capture.appeared);
        QVERIFY(capture.informativeText.contains(QStringLiteral("next reboot")));
        QVERIFY(capture.informativeText.contains(QStringLiteral("Snapper")));
        QVERIFY(capture.informativeText.contains(QStringLiteral("cannot guarantee")));
        QVERIFY(capture.informativeText.contains(QStringLiteral("/home")));
        QVERIFY(capture.informativeText.contains(QStringLiteral("/dev/test-host")));
    }
    QCOMPARE(capturedHostRequestCount(capturePath, QStringLiteral("host-snapshots"),
                                      QStringLiteral("rollback")), 0);

    // Wrong typed text: still no rollback request.
    {
        acceptNextMessageBox(&window, QMessageBox::Yes);
        acceptNextInputDialog(&window, QStringLiteral("ROLLBACKX"));
        closeRepairProgressDialogWhenDone(&window);
        window.m_snapshotRollbackButton->click();
    }
    QCOMPARE(capturedHostRequestCount(capturePath, QStringLiteral("host-snapshots"),
                                      QStringLiteral("rollback")), 0);

    // Correct typed text issues exactly one rollback request; dismiss the
    // success dialog with Later.
    {
        NextMessageBoxCapture consequences(&window, QMessageBox::Yes);
        acceptNextInputDialog(&window, QStringLiteral("ROLLBACK"));
        clickMessageBoxButtonWhenVisible(&window, QStringLiteral("Later"));
        // Two progress dialogs: the read-only plan and the rollback request.
        closeRepairProgressDialogWhenDone(&window);
        closeRepairProgressDialogWhenDone(&window);
        window.m_snapshotRollbackButton->click();
        QVERIFY(consequences.appeared);
    }
    QCOMPARE(capturedHostRequestCount(capturePath, QStringLiteral("host-snapshots"),
                                      QStringLiteral("rollback")), 1);
}

// A successful host rollback clears the busy state, clears the stale
// inventory, persists the reboot-required state and shows the persistent
// banner with both actions.
void MainWindowUiTest::hostSnapshotRollbackSuccessShowsRebootRequiredAndBusyClears()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_autoRefreshDiagnostics->setChecked(false);
    prepareHostSnapshotScope(window);
    window.clearHostRebootRequired();
    QString evidence = capabilityEvidence(true, true);
    evidence += QStringLiteral("Host snapshot rollback: available\nHost reboot: available\n");
    cacheRepairEvidence(window, evidence);

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("host-snapshot-success.log"));
    QVERIFY2(startHostSnapshotFakePrivilegedSession(window, capturePath),
             "the scripted host snapshot session must start");

    window.updateSnapshotControls();
    window.loadSnapshots();
    QCOMPARE(window.m_snapshotTable->rowCount(), 1);
    window.m_snapshotTable->setCurrentCell(0, 0);
    window.updateSnapshotControls();

    NextMessageBoxCapture consequences(&window, QMessageBox::Yes);
    acceptNextInputDialog(&window, QStringLiteral("ROLLBACK"));
    clickMessageBoxButtonWhenVisible(&window, QStringLiteral("Later"));
    // Two progress dialogs: the read-only plan and the rollback request.
    closeRepairProgressDialogWhenDone(&window);
    closeRepairProgressDialogWhenDone(&window);
    window.m_snapshotRollbackButton->click();

    QVERIFY(consequences.appeared);
    QVERIFY(window.m_hostRebootRequired);
    QCOMPARE(window.m_hostRebootSnapshotId, QStringLiteral("7"));
    QVERIFY(!window.m_hostRebootRequiredAt.isEmpty());
    QVERIFY(!window.m_hostRebootBootId.isEmpty());
    QVERIFY(!window.m_hostRebootBanner->isHidden());
    QVERIFY(window.m_hostRebootBannerLabel->text().contains(QStringLiteral("Reboot required")));
    QVERIFY(window.m_hostRebootBannerLabel->text().contains(QStringLiteral("snapshot 7")));
    QVERIFY(window.m_activeBusyOperations.isEmpty());
    QVERIFY(window.m_busyIndicator->isHidden());
    QCOMPARE(window.m_snapshotTable->rowCount(), 0);
    // The staged rollback invalidated the cached running-host evidence.
    QVERIFY(window.m_hostDiagnosticCache.value(QStringLiteral("capabilities")).isEmpty());
}

// Reboot Now always requires a second explicit confirmation; Cancel sends no
// host-reboot request and the accepted path sends exactly one.
void MainWindowUiTest::hostSnapshotRollbackRebootNowRequiresSecondConfirmation()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);
    prepareHostSnapshotScope(window);
    window.clearHostRebootRequired();

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("host-snapshot-reboot.log"));
    QVERIFY2(startHostSnapshotFakePrivilegedSession(window, capturePath),
             "the scripted host snapshot session must start");

    window.setHostRebootRequired(QStringLiteral("7"));
    QVERIFY(!window.m_hostRebootBanner->isHidden());
    QVERIFY(window.m_hostRebootNowButton->isEnabled());

    // Cancel at the second confirmation: no host-reboot request.
    {
        NextMessageBoxCapture capture(&window, QMessageBox::Cancel);
        window.m_hostRebootNowButton->click();
        QVERIFY(capture.appeared);
        QVERIFY(capture.title.contains(QStringLiteral("Reboot")));
    }
    QCOMPARE(capturedHostRequestCount(capturePath, QStringLiteral("host-reboot")), 0);
    QVERIFY(window.m_hostRebootRequired);

    // Accept: exactly one host-reboot request.
    {
        NextMessageBoxCapture capture(&window, QMessageBox::Yes);
        closeRepairProgressDialogWhenDone(&window);
        window.m_hostRebootNowButton->click();
        QVERIFY(capture.appeared);
    }
    QCOMPARE(capturedHostRequestCount(capturePath, QStringLiteral("host-reboot")), 1);
}

// The persisted reminder hides when Host Maintenance is left but returns on
// re-entry, and a new kernel boot id clears it after a real reboot.
void MainWindowUiTest::hostSnapshotRebootBannerPersistsAcrossScopeAndClearsOnBootChange()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());
    ScopedEnvironmentVariable bootIdOverride("BOOT_REPAIR_BOOT_ID", "boot-a");

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareHostSnapshotScope(window);
    window.clearHostRebootRequired();

    window.setHostRebootRequired(QStringLiteral("7"));
    QCOMPARE(window.m_hostRebootBootId, QStringLiteral("boot-a"));
    QVERIFY(!window.m_hostRebootBanner->isHidden());

    // Leaving hides the banner but keeps the flag; re-entering shows it again.
    window.exitHostMaintenanceMode();
    QVERIFY(window.m_hostRebootBanner->isHidden());
    QVERIFY(window.m_hostRebootRequired);
    window.m_hostMaintenanceMode = true;
    window.updateHostRebootBanner();
    QVERIFY(!window.m_hostRebootBanner->isHidden());

    // A different kernel boot id means the staged rollback took effect.
    qputenv("BOOT_REPAIR_BOOT_ID", "boot-b");
    window.reconcileHostRebootRequired();
    QVERIFY(!window.m_hostRebootRequired);
    QVERIFY(window.m_hostRebootSnapshotId.isEmpty());
    QVERIFY(window.m_hostRebootBanner->isHidden());
}

// The host rollback control fails closed without capability evidence and uses
// the helper's exact unavailable reason when one is cached.
void MainWindowUiTest::hostSnapshotControlsFailClosedWithoutCapabilityEvidence()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);
    prepareHostSnapshotScope(window);
    window.clearHostRebootRequired();

    SnapshotInventoryRow row;
    row.id = QStringLiteral("7");
    row.created = QStringLiteral("2026-09-20 12:00:00");
    row.type = QStringLiteral("single");
    row.description = QStringLiteral("host test snapshot");
    row.status = QStringLiteral("Linux root snapshot; ro=true");
    row.relativePath = QStringLiteral("@.snapshots/7/snapshot");

    window.updateSnapshotControls();
    window.m_snapshotResultIdentity = window.snapshotScopeIdentity();
    window.populateSnapshotTable({row});
    window.m_snapshotTable->setCurrentCell(0, 0);

    // No cached evidence: disabled with the run-diagnostics reason.
    window.updateSnapshotControls();
    QVERIFY(!window.m_snapshotRollbackButton->isEnabled());
    QVERIFY2(window.m_snapshotRollbackButton->toolTip().contains(QStringLiteral("Run running-host diagnostics")),
             qPrintable(window.m_snapshotRollbackButton->toolTip()));

    // An exact unavailable reason is surfaced verbatim.
    QString evidence = capabilityEvidence(true, true);
    evidence += QStringLiteral("Host snapshot rollback: unavailable|snapper is not installed\n");
    cacheRepairEvidence(window, evidence);
    window.updateSnapshotControls();
    QVERIFY(!window.m_snapshotRollbackButton->isEnabled());
    QCOMPARE(window.m_snapshotRollbackButton->toolTip(), QStringLiteral("snapper is not installed"));

    // Available evidence enables the rollback for a valid root row.
    evidence = capabilityEvidence(true, true);
    evidence += QStringLiteral("Host snapshot rollback: available\n");
    cacheRepairEvidence(window, evidence);
    window.updateSnapshotControls();
    QVERIFY2(window.m_snapshotRollbackButton->isEnabled(),
             qPrintable(window.m_snapshotRollbackButton->toolTip()));
}

// A successful rollback never reboots by itself, and a failed Reboot Now keeps
// the banner and surfaces the helper's refusal reason.
void MainWindowUiTest::hostSnapshotRollbackNeverRebootsAutomatically()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_autoRefreshDiagnostics->setChecked(false);
    prepareHostSnapshotScope(window);
    window.clearHostRebootRequired();
    QString evidence = capabilityEvidence(true, true);
    evidence += QStringLiteral("Host snapshot rollback: available\nHost reboot: available\n");
    cacheRepairEvidence(window, evidence);

    QTemporaryDir requestDir;
    QVERIFY(requestDir.isValid());
    const QString capturePath = requestDir.filePath(QStringLiteral("host-snapshot-noreboot.log"));
    QVERIFY2(startHostSnapshotFakePrivilegedSession(window, capturePath, QStringLiteral("success"),
                                                    QStringLiteral("fail")),
             "the scripted host snapshot session must start");

    window.updateSnapshotControls();
    window.loadSnapshots();
    QCOMPARE(window.m_snapshotTable->rowCount(), 1);
    window.m_snapshotTable->setCurrentCell(0, 0);
    window.updateSnapshotControls();

    NextMessageBoxCapture consequences(&window, QMessageBox::Yes);
    acceptNextInputDialog(&window, QStringLiteral("ROLLBACK"));
    clickMessageBoxButtonWhenVisible(&window, QStringLiteral("Later"));
    // Two progress dialogs: the read-only plan and the rollback request.
    closeRepairProgressDialogWhenDone(&window);
    closeRepairProgressDialogWhenDone(&window);
    window.m_snapshotRollbackButton->click();
    QVERIFY(consequences.appeared);
    QVERIFY(window.m_hostRebootRequired);

    // No automatic reboot after a successful rollback.
    QTest::qWait(200);
    QCOMPARE(capturedHostRequestCount(capturePath, QStringLiteral("host-reboot")), 0);

    // A refused Reboot Now keeps the banner and reports the helper reason.
    bool rebootConfirmed = false;
    bool failureSeen = false;
    QString failureText;
    QTimer poll;
    poll.setInterval(5);
    QObject::connect(&poll, &QTimer::timeout, &window, [&] {
        for (QWidget *top : QApplication::topLevelWidgets()) {
            auto *box = qobject_cast<QMessageBox *>(top);
            if (!box || !box->isVisible()) {
                continue;
            }
            if (!rebootConfirmed) {
                if (QAbstractButton *yes = box->button(QMessageBox::Yes)) {
                    rebootConfirmed = true;
                    yes->click();
                    return;
                }
            } else if (QAbstractButton *ok = box->button(QMessageBox::Ok)) {
                failureSeen = true;
                failureText = box->text();
                ok->click();
                poll.stop();
                poll.deleteLater();
                return;
            }
        }
    });
    poll.start();
    closeRepairProgressDialogWhenDone(&window);
    closeRepairProgressDialogWhenDone(&window);
    window.m_hostRebootNowButton->click();

    QTRY_VERIFY_WITH_TIMEOUT(rebootConfirmed && failureSeen, 5000);
    QVERIFY(failureText.contains(QStringLiteral("systemd refused")));
    QVERIFY(window.m_hostRebootRequired);
    QVERIFY(!window.m_hostRebootBanner->isHidden());
    QCOMPARE(capturedHostRequestCount(capturePath, QStringLiteral("host-reboot")), 1);
}

int main(int argc, char **argv)
{
    QTemporaryDir config;
    QTemporaryDir sessionLogs;
    if (!config.isValid() || !sessionLogs.isValid()) {
        return 2;
    }
    qputenv("XDG_CONFIG_HOME", config.path().toUtf8());
    qputenv("BOOT_REPAIR_LOG_DIR", sessionLogs.path().toUtf8());
    qputenv("QT_QPA_PLATFORM", "offscreen");
    QApplication application(argc, argv);
    MainWindowUiTest test;
    return QTest::qExec(&test, argc, argv);
}

#include "MainWindowUiTest.moc"
