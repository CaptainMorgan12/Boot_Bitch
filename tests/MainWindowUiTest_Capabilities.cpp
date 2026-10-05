// Part of the boot-repair-ui-tests read-only regression suite: Boot capabilities
// table (cache-sourced population, the module-build backend mapping, the scope
// selector and the install affordance) and the repair detail pane's
// missing-tool guidance.
#include "MainWindowUiTest.h"
#include "UiTestHelpers.h"

using namespace UiTestDetail;

namespace {

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

// (e) When a repair tool is unavailable because a required module-build system
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
