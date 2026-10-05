// Part of the boot-repair-ui-tests read-only regression suite: Boot capabilities
// table (cache-sourced population, the module-build backend mapping, the scope
// selector and the install affordance) and the repair detail pane's
// missing-tool guidance.
#include "MainWindowUiTest.h"
#include "UiTestHelpers.h"

#include <QFontMetrics>
#include <QTranslator>

using namespace UiTestDetail;

namespace {

// A translator that renders the five capability header titles as deliberately
// wider German strings. Installed by capabilityHeaderColumnsFitTranslatedTitles
// so the column-floor regression (measuring the hardcoded English title instead
// of the translated one) is exercised even in the English test environment.
class WidenedHeaderTranslator final : public QTranslator
{
public:
    QString translate(const char *context, const char *sourceText,
                      const char * /*disambiguation*/, int /*n*/) const override
    {
        if (qstrcmp(context, "MainWindow") != 0) {
            return QString();
        }
        const QString source = QString::fromUtf8(sourceText);
        if (source == QLatin1String("Feature")) {
            return QStringLiteral("Merkmal");
        }
        if (source == QLatin1String("Command")) {
            return QStringLiteral("Kommando");
        }
        if (source == QLatin1String("Scope")) {
            return QStringLiteral("Anwendungsbereich");
        }
        if (source == QLatin1String("Status")) {
            return QStringLiteral("Statuslage");
        }
        if (source == QLatin1String("Suggested package")) {
            return QStringLiteral("Vorgeschlagenes Paket");
        }
        return QString();
    }
};

// Installs a translator for the duration of a test function; the destructor
// removes it even when a QVERIFY/QCOMPARE macro returns early.
class ScopedTranslator
{
public:
    explicit ScopedTranslator(QTranslator *translator)
        : m_translator(translator)
    {
        QCoreApplication::installTranslator(translator);
    }
    ~ScopedTranslator()
    {
        QCoreApplication::removeTranslator(m_translator);
    }

private:
    QTranslator *m_translator;
};

// A complete 15-key repair-target capability preamble with matching evidence
// lines, plus the backend section that names the module-build system.
QString repairCapabilitiesPreamble(const QString &dkmsState, const QString &dkmsEvidence)
{
    return QStringLiteral(
        "Repair tool validate: available\n"
        "Repair tool filesystem: available\n"
        "Repair tool dpkg: available\n"
        "Repair tool fixbroken: available\n"
        "Repair tool aptupdate: available\n"
        "Repair tool upgrade: available\n"
        "Repair tool dkms: %1\n"
        "Repair tool display: available\n"
        "Repair tool gpu: available\n"
        "Repair tool initramfs: available\n"
        "Repair tool efi: available\n"
        "Repair tool grub: available\n"
        "Repair tool extlinux: unavailable|reason:missing-update-extlinux\n"
        "Repair tool bootstack: available\n"
        "Repair tool kernels: available\n"
        "Repair capability evidence validate: read-only preflight always available\n"
        "Repair capability evidence filesystem: e2fsck present\n"
        "Repair capability evidence dpkg: /usr/bin/dpkg and /var/lib/dpkg/status present\n"
        "Repair capability evidence fixbroken: apt-get present\n"
        "Repair capability evidence aptupdate: apt-get present; APT sources present\n"
        "Repair capability evidence upgrade: apt-get present\n"
        "Repair capability evidence dkms: %2\n"
        "Repair capability evidence display: graphical.target present\n"
        "Repair capability evidence gpu: nvidia kernel module loaded\n"
        "Repair capability evidence initramfs: dracut executable present\n"
        "Repair capability evidence efi: grub-install present; ESP mount candidate: /boot/efi\n"
        "Repair capability evidence grub: grub-install present\n"
        "Repair capability evidence extlinux: update-extlinux is not installed in the target\n"
        "Repair capability evidence bootstack: dracut and GRUB2 present\n"
        "Repair capability evidence kernels: kernel inventory present\n")
        .arg(dkmsState, dkmsEvidence);
}

QString backendSection(const QString &moduleBuild)
{
    return QStringLiteral(
        "Target backend profile (read-only):\n"
        "Distribution ID: fedora\n"
        "Distribution family: fedora\n"
        "Package manager backend: rpm\n"
        "Service manager: systemd\n"
        "Display manager backend: systemd\n"
        "Initramfs backend: dracut\n"
        "Bootloader backend: grub\n"
        "Module build system: %1\n")
        .arg(moduleBuild);
}

// Stable running-host prerequisite lines, one per host tool.  The lsblk entry is
// deliberately marked missing with a fake package so a test can prove the table
// reads the cache instead of re-probing the real filesystem.
QString hostCapabilitiesSection()
{
    return QStringLiteral(
        "Host capability probes (read-only, recovery host):\n"
        "Host capability: lsblk package=testutil status=missing scope=host note=Required for block-device inventory\n"
        "Host capability: blkid package=util-linux status=available scope=host note=Used to identify filesystem metadata\n"
        "Host capability: findmnt package=util-linux status=available scope=host note=Used to understand active mounts\n"
        "Host capability: cryptsetup package=cryptsetup status=available scope=host note=Required to unlock encrypted targets\n"
        "Host capability: btrfs package=btrfs-progs status=available scope=host note=Required for Btrfs inspection and snapshot rollback\n"
        "Host capability: rsync package=rsync status=available scope=host note=Required for verified Host-to-Repair and Repair-to-Host transfer\n"
        "Host capability: chroot package=coreutils status=available scope=host note=Required for target-side repair commands\n"
        "Host capability: systemctl package=systemd status=available scope=host note=Used to restore graphical.target and the configured display manager without starting the target GUI\n"
        "Host capability: efibootmgr package=efibootmgr status=available scope=host note=Used to preserve target EFI BootOrder during TUXEDO UKI rebuilds when efivars are available\n"
        "Host capability: objcopy package=binutils status=available scope=host note=Used to verify the kernel embedded in a rebuilt unified kernel image\n"
        "Host capability: lvs package=lvm2 status=available scope=host note=Optional LVM storage-stack support\n"
        "Host capability: mdadm package=mdadm status=available scope=host note=Optional Linux MD RAID support\n");
}

int rowForCommand(MainWindow &window, const QString &command)
{
    for (int row = 0; row < window.m_capabilityTable->rowCount(); ++row) {
        QTableWidgetItem *item = window.m_capabilityTable->item(row, 1);
        if (item && item->text() == command) {
            return row;
        }
    }
    return -1;
}

int rowForFeature(MainWindow &window, const QString &feature)
{
    for (int row = 0; row < window.m_capabilityTable->rowCount(); ++row) {
        QTableWidgetItem *item = window.m_capabilityTable->item(row, 0);
        if (item && item->text() == feature) {
            return row;
        }
    }
    return -1;
}

QString cellText(MainWindow &window, int row, int column)
{
    QTableWidgetItem *item = window.m_capabilityTable->item(row, column);
    return item ? item->text() : QString();
}

} // namespace

// (a) The repair-target table parses the cached capability preamble: one row per
// `Repair tool <key>` line with availability from the cache, and the module-build
// row's Command/Suggested package come from the target's `Module build system:`
// line (never the host distro).  The parse is pure cache consumption.
void MainWindowUiTest::capabilityRepairEvidenceParsesFromCache()
{
    const QList<Capability> capabilities = CapabilityChecker::capabilitiesFromRepairEvidence(
        repairCapabilitiesPreamble(QStringLiteral("available"),
                                   QStringLiteral("akmods executable present; kernel 6.12.1 build tree present")),
        backendSection(QStringLiteral("akmods")));

    // The 15 repair-tool keys all parse into rows.
    QCOMPARE(capabilities.size(), 15);

    const Capability *dkms = nullptr;
    const Capability *grub = nullptr;
    const Capability *extlinux = nullptr;
    for (const Capability &capability : capabilities) {
        if (capability.feature.contains(QStringLiteral("DKMS"))) {
            dkms = &capability;
        } else if (capability.feature == QCoreApplication::translate("CapabilityChecker", "GRUB configuration")) {
            grub = &capability;
        } else if (capability.feature == QCoreApplication::translate("CapabilityChecker", "extlinux configuration")) {
            extlinux = &capability;
        }
    }
    QVERIFY(dkms);
    QVERIFY(grub);
    QVERIFY(extlinux);

    // Availability is read verbatim from the cache, not probed.
    QVERIFY(dkms->available);
    QVERIFY(grub->available);
    QVERIFY(!extlinux->available);

    // The module-build row names the target backend (akmods) and its package,
    // independent of any host distribution.
    QCOMPARE(dkms->command, QStringLiteral("akmods"));
    QCOMPARE(dkms->packageName, QStringLiteral("akmods"));
    QVERIFY(dkms->installable);
    QVERIFY(dkms->targetScope);

    // A different backend token maps to its own package.
    const QList<Capability> akmsCapabilities = CapabilityChecker::capabilitiesFromRepairEvidence(
        repairCapabilitiesPreamble(QStringLiteral("available"), QStringLiteral("akms executable present")),
        backendSection(QStringLiteral("akms")));
    for (const Capability &capability : akmsCapabilities) {
        if (capability.feature.contains(QStringLiteral("DKMS"))) {
            QCOMPARE(capability.command, QStringLiteral("akms"));
            QCOMPARE(capability.packageName, QStringLiteral("akms"));
        }
    }
}

// (b) The running-host table parses the cached `host-capabilities` section into
// one row per `Host capability:` line, with the package/status/note read from
// the line (never re-probed).
void MainWindowUiTest::capabilityHostEvidenceParsesFromCache()
{
    const QList<Capability> capabilities =
        CapabilityChecker::capabilitiesFromHostEvidence(hostCapabilitiesSection());
    QCOMPARE(capabilities.size(), 12);

    const Capability *lsblk = nullptr;
    for (const Capability &capability : capabilities) {
        if (capability.command == QStringLiteral("lsblk")) {
            lsblk = &capability;
        }
    }
    QVERIFY(lsblk);
    // The injected evidence says "missing" with package "testutil"; the parser
    // must reflect that verbatim (it must not re-probe the real /bin/lsblk).
    QVERIFY(!lsblk->available);
    QCOMPARE(lsblk->packageName, QStringLiteral("testutil"));
    QVERIFY(!lsblk->targetScope);
    QCOMPARE(lsblk->note, QStringLiteral("Required for block-device inventory"));
}

// (c) The Boot capabilities table populates from the cached evidence even when
// the filesystem probe inputs are poisoned.  A poison os-release proves the
// module-build row's package comes from the cached backend line, and a fake
// host-capabilities "missing lsblk" proves the host table never re-probes the
// real filesystem.
void MainWindowUiTest::capabilityTablePopulatesFromCacheWithoutProbe()
{
    QTemporaryDir osReleaseDir;
    QVERIFY(osReleaseDir.isValid());
    QFile fake(osReleaseDir.filePath(QStringLiteral("os-release")));
    QVERIFY(fake.open(QIODevice::WriteOnly | QIODevice::Text));
    fake.write(QStringLiteral("ID=arch\nPRETTY_NAME=\"Poisoned arch\"\n").toUtf8());
    fake.close();
    const QByteArray previous = qgetenv("BOOT_REPAIR_OS_RELEASE");
    qputenv("BOOT_REPAIR_OS_RELEASE", fake.fileName().toUtf8());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window);

    // Target scope: inject the capability preamble plus a backend section that
    // names akmods. The host os-release is poisoned to "arch", so only the
    // cache can supply the akmods package.
    window.m_targetDiagnosticCache.insert(
        QStringLiteral("capabilities"),
        repairCapabilitiesPreamble(QStringLiteral("available"),
                                   QStringLiteral("akmods executable present; kernel 6.12.1 build tree present")));
    window.m_targetDiagnosticCache.insert(QStringLiteral("backend"), backendSection(QStringLiteral("akmods")));

    // Host scope: inject a host-capabilities section that reports lsblk missing
    // with a fake package even though the real host has /bin/lsblk.
    window.m_hostDiagnosticCache.insert(QStringLiteral("host-capabilities"), hostCapabilitiesSection());

    window.m_capabilityScopeSelector->setCurrentIndex(0); // Repair target
    window.refreshCapabilities();

    const int dkmsRow = rowForCommand(window, QStringLiteral("akmods"));
    QVERIFY2(dkmsRow >= 0, "the module-build row must be present in the target table");
    QCOMPARE(cellText(window, dkmsRow, 4), QStringLiteral("akmods"));
    QCOMPARE(cellText(window, dkmsRow, 3), QStringLiteral("Available"));

    // Host scope: the poisoned lsblk stays "Missing" because the table reads the
    // cache, never the filesystem.
    window.m_capabilityScopeSelector->setCurrentIndex(1); // Host
    window.refreshCapabilities();

    const int lsblkRow = rowForCommand(window, QStringLiteral("lsblk"));
    QVERIFY2(lsblkRow >= 0, "the lsblk row must be present in the host table");
    QCOMPARE(cellText(window, lsblkRow, 3), QStringLiteral("Missing"));
    QCOMPARE(cellText(window, lsblkRow, 4), QStringLiteral("testutil"));

    if (previous.isNull()) {
        qunsetenv("BOOT_REPAIR_OS_RELEASE");
    } else {
        qputenv("BOOT_REPAIR_OS_RELEASE", previous);
    }
}

// (d) The scope selector switches the table between the two cached scopes
// without changing the app's scope model.
void MainWindowUiTest::capabilityScopeSelectorSwitchesCaches()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window);

    window.m_targetDiagnosticCache.insert(
        QStringLiteral("capabilities"),
        repairCapabilitiesPreamble(QStringLiteral("available"), QStringLiteral("dkms executable present")));
    window.m_targetDiagnosticCache.insert(QStringLiteral("backend"), backendSection(QStringLiteral("dkms")));
    window.m_hostDiagnosticCache.insert(QStringLiteral("host-capabilities"), hostCapabilitiesSection());

    // Default scope (target mode) shows the repair-target rows.
    QCOMPARE(window.m_capabilityScopeSelector->currentData().toString(), QStringLiteral("target"));
    window.refreshCapabilities();
    QVERIFY2(rowForCommand(window, QStringLiteral("dkms")) >= 0, "target scope shows the dkms row");
    QCOMPARE(rowForCommand(window, QStringLiteral("lsblk")), -1);

    // Switching to Host shows the host rows; the app scope is unchanged.
    window.m_capabilityScopeSelector->setCurrentIndex(1);
    QVERIFY2(rowForCommand(window, QStringLiteral("lsblk")) >= 0, "host scope shows the lsblk row");
    QCOMPARE(rowForCommand(window, QStringLiteral("dkms")), -1);
    QVERIFY(!window.m_hostMaintenanceMode);

    // Switching back restores the target rows.
    window.m_capabilityScopeSelector->setCurrentIndex(0);
    QVERIFY2(rowForCommand(window, QStringLiteral("dkms")) >= 0, "target scope shows the dkms row again");
}

// (e) The Boot capabilities group box must carry a scope-neutral title: the
// table now shows both the repair-target and the running-host scopes, so the
// title no longer names only the host.
void MainWindowUiTest::capabilityGroupBoxTitleIsScopeNeutral()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    QGroupBox *capabilityBox = nullptr;
    for (QGroupBox *box : window.findChildren<QGroupBox *>()) {
        if (box->isAncestorOf(window.m_capabilityTable)) {
            capabilityBox = box;
            break;
        }
    }
    QVERIFY2(capabilityBox, "the Boot capabilities table must live in a group box");
    QCOMPARE(capabilityBox->title(), QStringLiteral("Capabilities and dependencies"));
    QVERIFY2(!capabilityBox->title().contains(QStringLiteral("Host")),
             "the group-box title must be scope-neutral (no Host-only wording)");
}

// (f) refreshCapabilities() must run at the end of a diagnostic run and after
// Clear Register clears the caches, so the table populates from fresh evidence
// and reverts to the placeholder (never stale rows) once the cache is gone.
void MainWindowUiTest::capabilityRefreshAfterDiagnosticsAndClear()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window);

    // refreshCapabilities() records a capability-scan status entry; drop the
    // constructor's entry so its reappearance proves the post-run call.
    // statusEntryIdentity("capability-scan") → "status\x1fcapability-scan" (0x1F
    // unit separator). Build it explicitly so the \x escape cannot swallow the
    // following hex letter 'c'/'a'.
    const QString capabilityScanIdentity =
        QStringLiteral("status") + QChar(0x1f) + QStringLiteral("capability-scan");
    window.m_statusLogEntries.remove(capabilityScanIdentity);

    // (f1) Completing a diagnostic run must refresh the table.
    window.runDiagnosticSections({QStringLiteral("grub")});
    QVERIFY2(window.m_statusLogEntries.contains(capabilityScanIdentity),
             "refreshCapabilities must run after runDiagnosticSections() completes");

    // (f2) Clear Register fully clears the caches, then refreshes the table so
    // the placeholder replaces the previously populated rows.
    window.m_targetDiagnosticCache.insert(
        QStringLiteral("capabilities"),
        repairCapabilitiesPreamble(QStringLiteral("available"),
                                   QStringLiteral("dkms executable present")));
    window.m_targetDiagnosticCache.insert(QStringLiteral("backend"), backendSection(QStringLiteral("dkms")));
    window.refreshCapabilities();
    QVERIFY2(rowForCommand(window, QStringLiteral("dkms")) >= 0,
             "warm target evidence must show rows before the clear");

    QPushButton *clearRegister = buttonWithText(window, QStringLiteral("Clear Register"));
    QVERIFY(clearRegister);
    acceptNextMessageBoxByText(&window, QStringLiteral("Clear"));
    clearRegister->click();

    QVERIFY2(window.m_statusLogEntries.contains(capabilityScanIdentity),
             "refreshCapabilities must run after clearActionRegister() clears the caches");
    QCOMPARE(window.m_capabilityTable->rowCount(), 1);
    QCOMPARE(cellText(window, 0, 0), QStringLiteral("No capability evidence cached"));
}

// (g) When a repair tool is unavailable because a required module-build system
// is missing, the detail pane surfaces a one-liner naming the missing tool, the
// suggested package and the Boot capabilities table. The Run action stays
// disabled. The Alpine variant names akms, never dkms.
void MainWindowUiTest::repairDetailPaneShowsMissingToolGuidance()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window);

    // Debian target: missing-dkms names the dkms package.
    QString evidence = capabilityEvidence(false, true);
    evidence.replace(QStringLiteral("Repair tool dkms: available"),
                     QStringLiteral("Repair tool dkms: unavailable|reason:missing-dkms"));
    cacheRepairEvidence(window, evidence);

    QTreeWidgetItem *dkms = repairItem(window, QStringLiteral("dkms"));
    QVERIFY(dkms);
    window.m_repairToolTree->setCurrentItem(dkms);
    window.updateRepairToolDetails();

    QString status = window.m_repairToolPlanStatus->text();
    QVERIFY2(status.contains(QStringLiteral("Missing module-build system")), qPrintable(status));
    QVERIFY2(status.contains(QStringLiteral("dkms")), qPrintable(status));
    QVERIFY2(status.contains(QStringLiteral("Boot capabilities table")), qPrintable(status));
    QVERIFY2(!window.m_repairToolButton->isEnabled(),
             "the Run action must stay disabled while the build system is missing");

    // Alpine target: missing-dkms-alpine names the akms package.
    evidence = capabilityEvidence(false, true);
    evidence.replace(QStringLiteral("Distribution family: debian"),
                     QStringLiteral("Distribution family: alpine"));
    evidence.replace(QStringLiteral("Repair tool dkms: available"),
                     QStringLiteral("Repair tool dkms: unavailable|reason:missing-dkms-alpine"));
    cacheRepairEvidence(window, evidence);
    window.m_repairToolTree->setCurrentItem(dkms);
    window.updateRepairToolDetails();

    status = window.m_repairToolPlanStatus->text();
    QVERIFY2(status.contains(QStringLiteral("Missing module-build system")), qPrintable(status));
    QVERIFY2(status.contains(QStringLiteral("akms")), qPrintable(status));
    QVERIFY2(status.contains(QStringLiteral("Boot capabilities table")), qPrintable(status));
    QVERIFY2(!window.m_repairToolButton->isEnabled(),
             "the Run action must stay disabled while the build system is missing");
}

// (h) Non-available capability evidence must surface as "Missing" rows in both
// scopes rather than being dropped: an `unavailable|reason:missing-update-extlinux`
// repair-tool line stays in the repair-target table, and a
// `Host capability: mdadm ... status=missing` line stays in the host table.
void MainWindowUiTest::capabilityUnavailableRowsRenderAsMissing()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window);

    // Target scope: extlinux is unavailable with a machine-readable reason.
    window.m_targetDiagnosticCache.insert(
        QStringLiteral("capabilities"),
        repairCapabilitiesPreamble(QStringLiteral("available"), QStringLiteral("dkms executable present")));
    window.m_targetDiagnosticCache.insert(QStringLiteral("backend"), backendSection(QStringLiteral("dkms")));

    // Host scope: mdadm is reported missing (the real host may have it, but the
    // cache is the single source of truth).
    QString hostSection = hostCapabilitiesSection();
    hostSection.replace(
        QStringLiteral("Host capability: mdadm package=mdadm status=available scope=host note=Optional Linux MD RAID support"),
        QStringLiteral("Host capability: mdadm package=mdadm status=missing scope=host note=Optional Linux MD RAID support"));
    window.m_hostDiagnosticCache.insert(QStringLiteral("host-capabilities"), hostSection);

    window.m_capabilityScopeSelector->setCurrentIndex(0); // Repair target
    window.refreshCapabilities();

    const QString extlinuxFeature = QCoreApplication::translate("CapabilityChecker", "extlinux configuration");
    const int extlinuxRow = rowForFeature(window, extlinuxFeature);
    QVERIFY2(extlinuxRow >= 0, "the unavailable extlinux row must be present in the target table");
    QCOMPARE(cellText(window, extlinuxRow, 3), QStringLiteral("Missing"));
    QCOMPARE(cellText(window, extlinuxRow, 5), QStringLiteral("reason:missing-update-extlinux"));

    window.m_capabilityScopeSelector->setCurrentIndex(1); // Host
    window.refreshCapabilities();

    const int mdadmRow = rowForCommand(window, QStringLiteral("mdadm"));
    QVERIFY2(mdadmRow >= 0, "the missing mdadm row must be present in the host table");
    QCOMPARE(cellText(window, mdadmRow, 3), QStringLiteral("Missing"));
}

// (i) Each non-stretch column's default width is floored on its translated
// header title, not the hardcoded English string. A widened translator is
// installed before the window is constructed so the header titles render wider
// than English (German "Vorgeschlagenes Paket" for "Suggested package",
// "Merkmal" for "Feature"); the column must be wide enough to fit the title.
void MainWindowUiTest::capabilityHeaderColumnsFitTranslatedTitles()
{
    WidenedHeaderTranslator translator;
    ScopedTranslator installed(&translator);

    // Sanity: the translator actually widens the titles beyond English.
    QVERIFY(QCoreApplication::translate("MainWindow", "Suggested package")
            != QLatin1String("Suggested package"));
    QVERIFY(QCoreApplication::translate("MainWindow", "Feature")
            != QLatin1String("Feature"));

    MainWindow window;
    window.show();
    QTest::qWait(50);

    const QFontMetrics metrics(window.m_capabilityTable->font());
    const QHeaderView *header = window.m_capabilityTable->horizontalHeader();
    QVERIFY(header);

    const struct {
        int column;
        const char *sourceText;
    } expectations[] = {
        {0, "Feature"},
        {1, "Command"},
        {2, "Scope"},
        {3, "Status"},
        {4, "Suggested package"},
    };

    for (const auto &expectation : expectations) {
        const QString title = QCoreApplication::translate("MainWindow", expectation.sourceText);
        // The column floor reserves 24 px of padding (sort indicator + margins)
        // on top of the title, so "fits" means the section is at least that wide.
        // This catches the regression where the floor is measured on the
        // hardcoded English string (e.g. German "Vorgeschlagenes Paket" + 24 >
        // the old 150 px floor, so the last letters clipped).
        const int titleWidth = metrics.horizontalAdvance(title) + 24;
        QVERIFY2(header->sectionSize(expectation.column) >= titleWidth,
                 qPrintable(QStringLiteral("column %1 must fit its translated title \"%2\"")
                                .arg(expectation.column)
                                .arg(title)));
    }

    // The Command column also keeps its representative content cell as a floor.
    const int commandContentWidth =
        metrics.horizontalAdvance(QStringLiteral("update-grub2 --version")) + 24;
    QVERIFY2(header->sectionSize(1) >= commandContentWidth,
             "the Command column must keep its content example as a floor");
}

// (j) The Distribution value is an ElidedLabel (elides right, zero-width
// minimum hint) so a long pretty name collapses to "TUXEDO OS…" at the minimum
// window width instead of overflowing the row, while the package-manager and
// KAuth labels keep their wrapping QLabel form.
void MainWindowUiTest::capabilityDistributionLabelIsElidedLabel()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);

    QVERIFY(window.m_distributionLabel);
    QVERIFY(window.m_packageManagerLabel);
    QVERIFY(window.m_authBuildLabel);

    // ElidedLabel reports a 0-width minimum hint; a wrapping QLabel does not.
    QCOMPARE(window.m_distributionLabel->minimumSizeHint().width(), 0);
    // The full text stays available in text() (elision is paint-only).
    QVERIFY(!window.m_distributionLabel->text().isEmpty());

    // The other identity labels keep the word-wrap/size-policy configuration.
    QVERIFY(window.m_packageManagerLabel->wordWrap());
    QVERIFY(window.m_authBuildLabel->wordWrap());
    QVERIFY(!window.m_distributionLabel->wordWrap());

    // The scope selector stays present on the same Distribution row (right-aligned
    // via the distribution label's stretch factor).
    QVERIFY(window.m_capabilityScopeSelector);
    QCOMPARE(window.m_distributionLabel->parentWidget(),
             window.m_capabilityScopeSelector->parentWidget());
}
