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

const Capability *snapshotCapabilityOf(const QList<Capability> &capabilities)
{
    for (const Capability &capability : capabilities) {
        if (capability.feature == QCoreApplication::translate("CapabilityChecker", "Snapshot rollback")) {
            return &capability;
        }
    }
    return nullptr;
}

// The module-build (dkms) row is identified by its Command column, which always
// carries the detected build-system token (dkms|akmods|akms|none). It cannot be
// matched on the Feature label anymore, because that label now reflects the
// detected build system ("DKMS rebuild" / "AKMS rebuild" / "AKMODS rebuild").
const Capability *moduleBuildCapability(const QList<Capability> &capabilities)
{
    for (const Capability &capability : capabilities) {
        const QString command = capability.command;
        if (command == QStringLiteral("dkms")
            || command == QStringLiteral("akmods")
            || command == QStringLiteral("akms")
            || command == QStringLiteral("none")) {
            return &capability;
        }
    }
    return nullptr;
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

    const Capability *dkms = moduleBuildCapability(capabilities);
    const Capability *grub = nullptr;
    const Capability *extlinux = nullptr;
    for (const Capability &capability : capabilities) {
        if (capability.feature == QCoreApplication::translate("CapabilityChecker", "GRUB configuration")) {
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

    // The module-build row names the target backend (akmods) but, when the tool
    // is available, has no suggested package and is not installable (nothing is
    // missing to install). Its Feature label reflects the akmods backend.
    QCOMPARE(dkms->command, QStringLiteral("akmods"));
    QCOMPARE(dkms->feature, QCoreApplication::translate("CapabilityChecker", "AKMODS rebuild"));
    QVERIFY(dkms->packageName.isEmpty());
    QVERIFY(!dkms->installable);
    QVERIFY(dkms->targetScope);

    // A different backend token maps to its own command and its own Feature
    // label; an available row again carries no installable package.
    const QList<Capability> akmsCapabilities = CapabilityChecker::capabilitiesFromRepairEvidence(
        repairCapabilitiesPreamble(QStringLiteral("available"), QStringLiteral("akms executable present")),
        backendSection(QStringLiteral("akms")));
    const Capability *akms = moduleBuildCapability(akmsCapabilities);
    QVERIFY(akms);
    QCOMPARE(akms->command, QStringLiteral("akms"));
    QCOMPARE(akms->feature, QCoreApplication::translate("CapabilityChecker", "AKMS rebuild"));
    QVERIFY(akms->packageName.isEmpty());

    // A missing build system names the build-system package (derived from the
    // family, since no binary was detected) and is installable ONLY when the
    // helper proved the package is present in the TARGET's own repositories
    // (the read-only `dkms-installable` side-channel). Its Feature label falls
    // back to "DKMS rebuild".
    const QList<Capability> missing = CapabilityChecker::capabilitiesFromRepairEvidence(
        repairCapabilitiesPreamble(QStringLiteral("unavailable|reason:missing-akmods"),
                                   QStringLiteral("akmods not detected"))
            + QStringLiteral("Repair capability evidence dkms-installable: available\n"),
        backendSection(QStringLiteral("none")));
    const Capability *missingDkms = moduleBuildCapability(missing);
    QVERIFY(missingDkms);
    QCOMPARE(missingDkms->command, QStringLiteral("none"));
    QCOMPARE(missingDkms->feature, QCoreApplication::translate("CapabilityChecker", "DKMS rebuild"));
    QCOMPARE(missingDkms->packageName, QStringLiteral("akmods"));
    QVERIFY(missingDkms->installable);

    // Item #4: without the helper's target-repo availability evidence the
    // missing build-system row fails closed - no suggested package and no
    // Install affordance (an install would otherwise fail "couldn't find
    // package" when the target repo lacks the package).
    const QList<Capability> noRepoEvidence = CapabilityChecker::capabilitiesFromRepairEvidence(
        repairCapabilitiesPreamble(QStringLiteral("unavailable|reason:missing-akmods"),
                                   QStringLiteral("akmods not detected")),
        backendSection(QStringLiteral("none")));
    const Capability *noRepoDkms = moduleBuildCapability(noRepoEvidence);
    QVERIFY(noRepoDkms);
    QVERIFY(noRepoDkms->packageName.isEmpty());
    QVERIFY(!noRepoDkms->installable);

    // A *-no-build-tree reason means the build system is present but the kernel
    // headers build tree is missing: no installable package is offered (the
    // headers package is surfaced as guidance instead, never a one-click
    // "Install akmods" for the already-installed build system).
    const QList<Capability> noBuildTree = CapabilityChecker::capabilitiesFromRepairEvidence(
        repairCapabilitiesPreamble(QStringLiteral("unavailable|reason:akmods-no-build-tree|param:6.20.1-300.fc44.x86_64"),
                                   QStringLiteral("akmods executable present; kernel 6.20.1 build tree missing")),
        backendSection(QStringLiteral("akmods")));
    const Capability *noBuildTreeDkms = moduleBuildCapability(noBuildTree);
    QVERIFY(noBuildTreeDkms);
    QCOMPARE(noBuildTreeDkms->command, QStringLiteral("akmods"));
    QCOMPARE(noBuildTreeDkms->feature, QCoreApplication::translate("CapabilityChecker", "AKMODS rebuild"));
    QVERIFY(noBuildTreeDkms->packageName.isEmpty());
    QVERIFY(!noBuildTreeDkms->installable);
}

// (b) The running-host scope derives from the SAME `Repair tool <key>` evidence
// as the repair target: both scopes render the same 15 repair-tool rows,
// differing only in the scope label ("Host") and targetScope=false.
void MainWindowUiTest::capabilityHostEvidenceParsesFromCache()
{
    const QString preamble = repairCapabilitiesPreamble(
        QStringLiteral("available"),
        QStringLiteral("akmods executable present; kernel 6.12.1 build tree present"));
    const QString backend = backendSection(QStringLiteral("akmods"));

    const QList<Capability> target = CapabilityChecker::capabilitiesFromRepairEvidence(preamble, backend);
    const QList<Capability> host = CapabilityChecker::capabilitiesFromRepairEvidence(
        preamble, backend, QCoreApplication::translate("CapabilityChecker", "Host"), /*targetScope=*/false);

    // Both scopes render the same 15 repair-tool rows.
    QCOMPARE(target.size(), 15);
    QCOMPARE(host.size(), 15);

    for (int row = 0; row < target.size(); ++row) {
        const Capability &t = target.at(row);
        const Capability &h = host.at(row);
        // Identical feature/command/availability/suggested-package/notes.
        QCOMPARE(h.feature, t.feature);
        QCOMPARE(h.command, t.command);
        QCOMPARE(h.available, t.available);
        QCOMPARE(h.packageName, t.packageName);
        QCOMPARE(h.note, t.note);
        QCOMPARE(h.installable, t.installable);
        // The only differences: the scope label and the targetScope flag.
        QCOMPARE(h.scope, QCoreApplication::translate("CapabilityChecker", "Host"));
        QVERIFY(!h.targetScope);
        QCOMPARE(t.scope, QCoreApplication::translate("CapabilityChecker", "Repair target"));
        QVERIFY(t.targetScope);
    }
}

// (b2) The scope-neutral storage-stack markers (`Storage lvm inspection:` and
// `Storage software raid:`) parse from the cached preamble into two extra rows
// for BOTH scopes. A missing tool names its providing package (lvm2/mdadm) and
// is installable; an available row has nothing to install. The rows carry the
// caller's scope label and targetScope routing (Repair target vs. Host).
void MainWindowUiTest::capabilityStorageStackRowsParseFromCache()
{
    const QString backend = backendSection(QStringLiteral("dkms"));

    // Missing tooling: both markers name their package and are installable.
    const QString missingPreamble = repairCapabilitiesPreamble(
        QStringLiteral("available"), QStringLiteral("dkms executable present"))
        + QStringLiteral(
            "Storage lvm inspection: unavailable|reason:missing-lvm2\n"
            "Storage lvm inspection evidence: lvs\n"
            "Storage software raid: unavailable|reason:missing-mdadm\n"
            "Storage software raid evidence: mdadm\n");
    const QList<Capability> missing = CapabilityChecker::capabilitiesFromRepairEvidence(
        missingPreamble, backend, QCoreApplication::translate("CapabilityChecker", "Host"),
        /*targetScope=*/false);

    const Capability *lvm = nullptr;
    const Capability *raid = nullptr;
    for (const Capability &capability : missing) {
        if (capability.feature == QCoreApplication::translate("CapabilityChecker", "LVM inspection")) {
            lvm = &capability;
        } else if (capability.feature == QCoreApplication::translate("CapabilityChecker", "Software RAID")) {
            raid = &capability;
        }
    }
    QVERIFY(lvm);
    QVERIFY(raid);
    QCOMPARE(missing.size(), 17); // 15 repair tools + lvm + software raid
    QVERIFY(!lvm->available);
    QCOMPARE(lvm->command, QStringLiteral("lvs"));
    QCOMPARE(lvm->packageName, QStringLiteral("lvm2"));
    QVERIFY(lvm->installable);
    QCOMPARE(lvm->scope, QCoreApplication::translate("CapabilityChecker", "Host"));
    QVERIFY(!lvm->targetScope);
    QVERIFY(!raid->available);
    QCOMPARE(raid->command, QStringLiteral("mdadm"));
    QCOMPARE(raid->packageName, QStringLiteral("mdadm"));
    QVERIFY(raid->installable);
    QVERIFY(!raid->targetScope);

    // The same missing preamble in repair-target scope surfaces the two rows
    // with the target scope label and targetScope=true routing.
    const QList<Capability> missingTarget = CapabilityChecker::capabilitiesFromRepairEvidence(
        missingPreamble, backend);
    const Capability *targetLvm = nullptr;
    const Capability *targetRaid = nullptr;
    for (const Capability &capability : missingTarget) {
        if (capability.feature == QCoreApplication::translate("CapabilityChecker", "LVM inspection")) {
            targetLvm = &capability;
        } else if (capability.feature == QCoreApplication::translate("CapabilityChecker", "Software RAID")) {
            targetRaid = &capability;
        }
    }
    QVERIFY(targetLvm);
    QVERIFY(targetRaid);
    QCOMPARE(missingTarget.size(), 17);
    QVERIFY(!targetLvm->available);
    QCOMPARE(targetLvm->command, QStringLiteral("lvs"));
    QCOMPARE(targetLvm->packageName, QStringLiteral("lvm2"));
    QVERIFY(targetLvm->installable);
    QCOMPARE(targetLvm->scope, QCoreApplication::translate("CapabilityChecker", "Repair target"));
    QVERIFY(targetLvm->targetScope);
    QVERIFY(!targetRaid->available);
    QCOMPARE(targetRaid->command, QStringLiteral("mdadm"));
    QCOMPARE(targetRaid->packageName, QStringLiteral("mdadm"));
    QVERIFY(targetRaid->installable);
    QVERIFY(targetRaid->targetScope);

    // Available tooling: no suggested package, not installable.
    const QString availablePreamble = repairCapabilitiesPreamble(
        QStringLiteral("available"), QStringLiteral("dkms executable present"))
        + QStringLiteral(
            "Storage lvm inspection: available\n"
            "Storage lvm inspection evidence: lvs present\n"
            "Storage software raid: available\n"
            "Storage software raid evidence: mdadm present\n");
    const QList<Capability> available = CapabilityChecker::capabilitiesFromRepairEvidence(
        availablePreamble, backend, QCoreApplication::translate("CapabilityChecker", "Host"),
        /*targetScope=*/false);
    const Capability *availableLvm = nullptr;
    const Capability *availableRaid = nullptr;
    for (const Capability &capability : available) {
        if (capability.feature == QCoreApplication::translate("CapabilityChecker", "LVM inspection")) {
            availableLvm = &capability;
        } else if (capability.feature == QCoreApplication::translate("CapabilityChecker", "Software RAID")) {
            availableRaid = &capability;
        }
    }
    QVERIFY(availableLvm);
    QVERIFY(availableRaid);
    QVERIFY(availableLvm->available);
    QCOMPARE(availableLvm->command, QStringLiteral("lvs present"));
    QVERIFY(availableLvm->packageName.isEmpty());
    QVERIFY(!availableLvm->installable);
    QVERIFY(availableRaid->available);
    QCOMPARE(availableRaid->command, QStringLiteral("mdadm present"));
    QVERIFY(availableRaid->packageName.isEmpty());
    QVERIFY(!availableRaid->installable);

    // The same available preamble in repair-target scope surfaces the two
    // available rows with the target scope label/routing.
    const QList<Capability> availableTarget = CapabilityChecker::capabilitiesFromRepairEvidence(
        availablePreamble, backend);
    const Capability *availableTargetLvm = nullptr;
    const Capability *availableTargetRaid = nullptr;
    for (const Capability &capability : availableTarget) {
        if (capability.feature == QCoreApplication::translate("CapabilityChecker", "LVM inspection")) {
            availableTargetLvm = &capability;
        } else if (capability.feature == QCoreApplication::translate("CapabilityChecker", "Software RAID")) {
            availableTargetRaid = &capability;
        }
    }
    QVERIFY(availableTargetLvm);
    QVERIFY(availableTargetRaid);
    QVERIFY(availableTargetLvm->available);
    QCOMPARE(availableTargetLvm->command, QStringLiteral("lvs present"));
    QCOMPARE(availableTargetLvm->scope, QCoreApplication::translate("CapabilityChecker", "Repair target"));
    QVERIFY(availableTargetLvm->targetScope);
    QVERIFY(availableTargetRaid->available);
    QCOMPARE(availableTargetRaid->command, QStringLiteral("mdadm present"));
    QVERIFY(availableTargetRaid->targetScope);

    // A preamble without any storage markers (e.g. an older cache) keeps
    // exactly the 15 tool rows in both scopes.
    const QList<Capability> target = CapabilityChecker::capabilitiesFromRepairEvidence(
        repairCapabilitiesPreamble(QStringLiteral("available"),
                                   QStringLiteral("dkms executable present")),
        backend);
    QCOMPARE(target.size(), 15);
}

// (c) The Boot capabilities table populates from the cached evidence even when
// the filesystem probe inputs are poisoned.  A poison os-release proves the
// module-build row's command comes from the cached backend line, and the host
// scope rendering the same cached rows proves the table never re-probes the
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

    // Both scopes inject the same cached capability preamble plus a backend
    // section that names akmods. The host os-release is poisoned to "arch", so
    // only the cache can supply the akmods backend token.
    const QString preamble = repairCapabilitiesPreamble(
        QStringLiteral("available"),
        QStringLiteral("akmods executable present; kernel 6.12.1 build tree present"));
    const QString backend = backendSection(QStringLiteral("akmods"));
    window.m_targetDiagnosticCache.insert(QStringLiteral("capabilities"), preamble);
    window.m_targetDiagnosticCache.insert(QStringLiteral("backend"), backend);
    window.m_hostDiagnosticCache.insert(QStringLiteral("capabilities"), preamble);
    window.m_hostDiagnosticCache.insert(QStringLiteral("backend"), backend);

    window.m_capabilityScopeSelector->setCurrentIndex(0); // Repair target
    window.refreshCapabilities();

    const int dkmsRow = rowForCommand(window, QStringLiteral("akmods"));
    QVERIFY2(dkmsRow >= 0, "the module-build row must be present in the target table");
    QCOMPARE(cellText(window, dkmsRow, 4), QString()); // available -> no suggested package
    QCOMPARE(cellText(window, dkmsRow, 3), QStringLiteral("Available"));

    // Host scope: the same cached akmods row is rendered from the cache (never
    // the poisoned "arch" host distro) with the Host scope label.
    window.m_capabilityScopeSelector->setCurrentIndex(1); // Host
    window.refreshCapabilities();

    const int hostDkmsRow = rowForCommand(window, QStringLiteral("akmods"));
    QVERIFY2(hostDkmsRow >= 0, "the module-build row must be present in the host table");
    QCOMPARE(cellText(window, hostDkmsRow, 3), QStringLiteral("Available"));
    QCOMPARE(cellText(window, hostDkmsRow, 2), QCoreApplication::translate("CapabilityChecker", "Host"));

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

    const QString preamble = repairCapabilitiesPreamble(
        QStringLiteral("available"), QStringLiteral("dkms executable present"));
    const QString backend = backendSection(QStringLiteral("dkms"));
    window.m_targetDiagnosticCache.insert(QStringLiteral("capabilities"), preamble);
    window.m_targetDiagnosticCache.insert(QStringLiteral("backend"), backend);
    window.m_hostDiagnosticCache.insert(QStringLiteral("capabilities"), preamble);
    window.m_hostDiagnosticCache.insert(QStringLiteral("backend"), backend);

    // Default scope (target mode) shows the repair-target rows.
    QCOMPARE(window.m_capabilityScopeSelector->currentData().toString(), QStringLiteral("target"));
    window.refreshCapabilities();
    QVERIFY2(rowForCommand(window, QStringLiteral("dkms")) >= 0, "target scope shows the dkms row");
    QCOMPARE(cellText(window, rowForCommand(window, QStringLiteral("dkms")), 2),
             QCoreApplication::translate("CapabilityChecker", "Repair target"));

    // Switching to Host shows the same repair-tool rows, relabelled Host; the
    // app scope is unchanged.
    window.m_capabilityScopeSelector->setCurrentIndex(1);
    QVERIFY2(rowForCommand(window, QStringLiteral("dkms")) >= 0, "host scope shows the dkms row");
    QCOMPARE(cellText(window, rowForCommand(window, QStringLiteral("dkms")), 2),
             QCoreApplication::translate("CapabilityChecker", "Host"));
    QVERIFY(!window.m_hostMaintenanceMode);

    // Switching back restores the target rows.
    window.m_capabilityScopeSelector->setCurrentIndex(0);
    QVERIFY2(rowForCommand(window, QStringLiteral("dkms")) >= 0, "target scope shows the dkms row again");
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

    // Fedora target: akmods-no-build-tree means the build system is present but
    // the kernel-devel build tree is missing; the guidance names kernel-devel.
    evidence = capabilityEvidence(false, true);
    evidence.replace(QStringLiteral("Distribution family: debian"),
                     QStringLiteral("Distribution family: fedora"));
    evidence.replace(QStringLiteral("Repair tool dkms: available"),
                     QStringLiteral("Repair tool dkms: unavailable|reason:akmods-no-build-tree|param:6.20.1-300.fc44.x86_64"));
    cacheRepairEvidence(window, evidence);
    window.m_repairToolTree->setCurrentItem(dkms);
    window.updateRepairToolDetails();

    status = window.m_repairToolPlanStatus->text();
    QVERIFY2(status.contains(QStringLiteral("Missing kernel build tree")), qPrintable(status));
    QVERIFY2(status.contains(QStringLiteral("kernel-devel")), qPrintable(status));
    QVERIFY2(!window.m_repairToolButton->isEnabled(),
             "the Run action must stay disabled while the kernel-devel build tree is missing");

    // Alpine target: akms-no-build-tree means the akms build system is present
    // but the version-specific dev package (linux-lts-dev) is missing. The
    // localized reason already names the exact package and the "boot a kernel
    // that matches the installed headers" guidance; no misleading linux-headers
    // install hint is added (nothing is installable for this reason).
    evidence = capabilityEvidence(false, true);
    evidence.replace(QStringLiteral("Distribution family: debian"),
                     QStringLiteral("Distribution family: alpine"));
    evidence.replace(QStringLiteral("Repair tool dkms: available"),
                     QStringLiteral("Repair tool dkms: unavailable|reason:akms-no-build-tree|param:6.18.54-0-lts|param:linux-lts-dev"));
    cacheRepairEvidence(window, evidence);
    window.m_repairToolTree->setCurrentItem(dkms);
    window.updateRepairToolDetails();

    status = window.m_repairToolPlanStatus->text();
    QVERIFY2(status.contains(QStringLiteral("linux-lts-dev")), qPrintable(status));
    QVERIFY2(status.contains(QStringLiteral("boot a kernel that matches the installed headers")), qPrintable(status));
    QVERIFY2(!status.contains(QStringLiteral("linux-headers")),
             qPrintable(status));
    QVERIFY2(!window.m_repairToolButton->isEnabled(),
             "the Run action must stay disabled while the akms build tree is missing");
}

// (h) Non-available capability evidence must surface as "Missing" rows in both
// scopes rather than being dropped: an `unavailable|reason:missing-update-extlinux`
// repair-tool line stays in the repair-target table and renders identically in
// the host scope (same single-truth evidence).
void MainWindowUiTest::capabilityUnavailableRowsRenderAsMissing()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window);

    // Both scopes inject the same preamble: extlinux is unavailable with a
    // machine-readable reason.
    const QString preamble = repairCapabilitiesPreamble(
        QStringLiteral("available"), QStringLiteral("dkms executable present"));
    const QString backend = backendSection(QStringLiteral("dkms"));
    window.m_targetDiagnosticCache.insert(QStringLiteral("capabilities"), preamble);
    window.m_targetDiagnosticCache.insert(QStringLiteral("backend"), backend);
    window.m_hostDiagnosticCache.insert(QStringLiteral("capabilities"), preamble);
    window.m_hostDiagnosticCache.insert(QStringLiteral("backend"), backend);

    window.m_capabilityScopeSelector->setCurrentIndex(0); // Repair target
    window.refreshCapabilities();

    const QString extlinuxFeature = QCoreApplication::translate("CapabilityChecker", "extlinux configuration");
    const int extlinuxRow = rowForFeature(window, extlinuxFeature);
    QVERIFY2(extlinuxRow >= 0, "the unavailable extlinux row must be present in the target table");
    QCOMPARE(cellText(window, extlinuxRow, 3), QStringLiteral("Missing"));
    QCOMPARE(cellText(window, extlinuxRow, 5), QStringLiteral("reason:missing-update-extlinux"));

    window.m_capabilityScopeSelector->setCurrentIndex(1); // Host
    window.refreshCapabilities();

    const int hostExtlinuxRow = rowForFeature(window, extlinuxFeature);
    QVERIFY2(hostExtlinuxRow >= 0, "the unavailable extlinux row must be present in the host table");
    QCOMPARE(cellText(window, hostExtlinuxRow, 3), QStringLiteral("Missing"));
    QCOMPARE(cellText(window, hostExtlinuxRow, 5), QStringLiteral("reason:missing-update-extlinux"));
    QCOMPARE(cellText(window, hostExtlinuxRow, 2), QCoreApplication::translate("CapabilityChecker", "Host"));
}

// The Settings page gains a vertical-splitting (horizontal) QSplitter between
// the upper sections (device-discovery/diagnostics preferences + the Full
// Repair plan) and the Boot capabilities region, so the two regions can be
// dragged to independent sizes. The splitter mirrors the other tabs' pattern:
// non-collapsible children, a grabbable handle, an explicit default size (the
// upper pane keeps its natural content height, so the pre-splitter default
// heights are restored), and relative pane minimums (~25%) so neither pane can
// be dragged out of view. The Refresh Capabilities button sits between the
// capabilities table and the mandatory safety controls. The Command/evidence
// column is also widened (roughly 70% from its previous 160 px floor) so long
// evidence text wraps less.
void MainWindowUiTest::settingsSplitterSeparatesPlanFromCapabilitiesAndWidensCommandColumn()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_tabs->setCurrentIndex(window.m_tabs->indexOf(window.m_settingsPage));
    QTest::qWait(50);
    QApplication::processEvents();

    // The splitter exists, splits horizontally, and holds exactly two panes.
    QVERIFY(window.m_settingsSplitter);
    QCOMPARE(window.m_settingsSplitter->orientation(), Qt::Vertical);
    QCOMPARE(window.m_settingsSplitter->count(), 2);

    QWidget *upper = window.m_settingsSplitter->widget(0);
    QWidget *lower = window.m_settingsSplitter->widget(1);
    QVERIFY(upper);
    QVERIFY(lower);

    // The Full Repair plan selections live in the upper pane; the capabilities
    // table lives in the lower pane, so the splitter sits between them.
    QVERIFY(upper->isAncestorOf(window.m_fullRepairFilesystem));
    QVERIFY(lower->isAncestorOf(window.m_capabilityTable));
    QVERIFY(!upper->isAncestorOf(window.m_capabilityTable));
    QVERIFY(!lower->isAncestorOf(window.m_fullRepairFilesystem));

    // The splitter is draggable but neither pane can be shrunk out of view:
    // each pane keeps at least ~25% of the current splitter height.
    const int splitterHeight = window.m_settingsSplitter->height();
    QVERIFY2(splitterHeight > 0, "the settings splitter must have a positive height");
    QVERIFY2(upper->minimumHeight() >= splitterHeight / 4,
             qPrintable(QStringLiteral("the upper pane must keep at least ~25% of the "
                                       "splitter height (min %1 vs splitter %2)")
                            .arg(upper->minimumHeight()).arg(splitterHeight)));
    QVERIFY2(lower->minimumHeight() >= splitterHeight / 4,
             qPrintable(QStringLiteral("the lower pane must keep at least ~25% of the "
                                       "splitter height (min %1 vs splitter %2)")
                            .arg(lower->minimumHeight()).arg(splitterHeight)));

    // Default heights restored: the upper pane keeps its natural content height
    // (preferences row + two-column Full Repair plan) instead of being crushed
    // to the plan's one-row minimum size hint.
    const int naturalUpper = upper->heightForWidth(upper->width());
    QVERIFY2(naturalUpper > 0, "the upper pane must have a measurable natural height");
    QVERIFY2(upper->height() >= naturalUpper - 2,
             qPrintable(QStringLiteral("the upper pane must keep its natural content height "
                                       "(got %1 vs natural %2)")
                            .arg(upper->height()).arg(naturalUpper)));

    // The Refresh Capabilities button sits below the capabilities table and
    // above the mandatory safety controls, outside the capabilities group box.
    QPushButton *refresh = window.findChild<QPushButton *>(QStringLiteral("settingsRefreshCapabilities"));
    QVERIFY2(refresh, "the Refresh Capabilities button must be present");
    QGroupBox *capabilityBox = nullptr;
    QGroupBox *safetyBox = nullptr;
    const auto groups = window.findChildren<QGroupBox *>();
    for (QGroupBox *group : groups) {
        if (group->title() == QStringLiteral("Capabilities and dependencies")) {
            capabilityBox = group;
        } else if (group->title() == QStringLiteral("Mandatory safety controls")) {
            safetyBox = group;
        }
    }
    QVERIFY2(capabilityBox, "the capabilities group box must exist");
    QVERIFY2(safetyBox, "the mandatory safety controls group box must exist");
    QVERIFY(!capabilityBox->isAncestorOf(refresh));
    const int capabilityTop = capabilityBox->mapTo(&window, QPoint(0, 0)).y();
    const int refreshTop = refresh->mapTo(&window, QPoint(0, 0)).y();
    const int safetyTop = safetyBox->mapTo(&window, QPoint(0, 0)).y();
    QVERIFY2(capabilityTop < refreshTop && refreshTop < safetyTop,
             qPrintable(QStringLiteral("the refresh button must sit between the capabilities "
                                       "table and the safety controls (tops %1 < %2 < %3)")
                            .arg(capabilityTop).arg(refreshTop).arg(safetyTop)));

    // The Command/evidence column keeps its widened default: at least 50%
    // wider than the previous 160 px floor (the actual floor is ~270 px).
    const int commandWidth = window.m_capabilityTable->horizontalHeader()->sectionSize(1);
    QVERIFY2(commandWidth >= 240,
             qPrintable(QStringLiteral("the Command column must stay widened (got %1 px)")
                            .arg(commandWidth)));

    // The device-discovery filters keep tight, uniform, content-height rows:
    // handing the group box extra vertical space (as a splitter drag or a
    // window resize does) must never stretch the three checkboxes apart into
    // growing inter-row gaps. The gaps and each row's height stay fixed.
    QGroupBox *deviceBox = nullptr;
    QGroupBox *diagnosticsBox = nullptr;
    for (QGroupBox *group : groups) {
        if (group->title() == QStringLiteral("Device discovery")) {
            deviceBox = group;
        } else if (group->title() == QStringLiteral("Diagnostics")) {
            diagnosticsBox = group;
        }
    }
    QVERIFY2(deviceBox, "the device discovery group box must exist");
    QVERIFY2(diagnosticsBox, "the diagnostics group box must exist");

    // The preferences row (Device discovery + Diagnostics side-by-side) must
    // render at its content height — three checkboxes plus the group title and
    // frame (~3.5 lines) — not grow to ~9 lines because the splitter handed the
    // upper pane extra vertical slack. The earlier regression stretched the
    // device-discovery box to ~3x its content height.
    const int deviceContentHeight = deviceBox->sizeHint().height();
    QVERIFY2(deviceContentHeight > 0, "the device discovery box must have a content height");
    QVERIFY2(deviceBox->height() <= deviceContentHeight + 2
                 && deviceBox->height() >= deviceContentHeight - 2,
             qPrintable(QStringLiteral("the device discovery group box must stay content-sized "
                                       "(got %1 vs content %2)")
                            .arg(deviceBox->height()).arg(deviceContentHeight)));

    // At first open the capabilities table actually renders at its fixed ~16-row
    // default height (the lower capabilities pane keeps the reclaimed space
    // from the shrunken top section), instead of the table being squeezed below
    // its 16-row minimum.
    const int capabilityOneLineHeight =
        window.m_capabilityTable->fontMetrics().lineSpacing() + 10;
    QVERIFY2(window.m_capabilityTable->height() >= 16 * capabilityOneLineHeight,
             qPrintable(QStringLiteral("the capabilities table must render at least 16 rows "
                                       "tall at first open (got %1 vs %2)")
                            .arg(window.m_capabilityTable->height())
                            .arg(16 * capabilityOneLineHeight)));

    {
        const auto gap = [](QCheckBox *a, QCheckBox *b) {
            return b->y() - (a->y() + a->height());
        };
        const QList<QCheckBox *> dev = {
            window.m_showNonLinux, window.m_showRemovable, window.m_showEncrypted
        };
        const QList<int> baselineGaps = {
            gap(dev.at(0), dev.at(1)), gap(dev.at(1), dev.at(2))
        };
        const QList<int> baselineHeights = {
            dev.at(0)->height(), dev.at(1)->height(), dev.at(2)->height()
        };

        // Grow the group box the way a taller splitter/window would: the three
        // rows must keep their own content height (no stretching) and the gaps
        // must stay uniform and unchanged.
        deviceBox->resize(deviceBox->width(), deviceBox->height() + 150);
        QApplication::processEvents();
        QTest::qWait(20);
        for (int i = 0; i < 3; ++i) {
            QVERIFY2(qAbs(dev.at(i)->height() - baselineHeights.at(i)) <= 1,
                     qPrintable(QStringLiteral("device-discovery row %1 must stay "
                                               "content-sized (got %2, want %3)")
                                    .arg(i + 1).arg(dev.at(i)->height()).arg(baselineHeights.at(i))));
        }
        QVERIFY2(qAbs(gap(dev.at(0), dev.at(1)) - baselineGaps.at(0)) <= 1
                     && qAbs(gap(dev.at(1), dev.at(2)) - baselineGaps.at(1)) <= 1,
                 "device-discovery row spacing must stay uniform and non-growing");
        QVERIFY2(qAbs(baselineGaps.at(0) - baselineGaps.at(1)) <= 1,
                 "device-discovery row spacing must be uniform");
        QVERIFY2(baselineGaps.at(0) < 24, "device-discovery row spacing must be tight");

        // The diagnostics auto-refresh row is likewise pinned to its own
        // content height (the trailing stretch absorbs any extra height).
        const int autoRefreshHeight = window.m_autoRefreshDiagnostics->height();
        diagnosticsBox->resize(diagnosticsBox->width(), diagnosticsBox->height() + 150);
        QApplication::processEvents();
        QTest::qWait(20);
        QVERIFY2(qAbs(window.m_autoRefreshDiagnostics->height() - autoRefreshHeight) <= 1,
                 "the diagnostics auto-refresh row must not stretch");

        // A real window resize plus a splitter drag must keep the spacing
        // unchanged as well.
        window.resize(1000, 900);
        QTest::qWait(50);
        QApplication::processEvents();
        window.m_settingsSplitter->setSizes({600, 300});
        QTest::qWait(50);
        QApplication::processEvents();
        QVERIFY2(qAbs(gap(dev.at(0), dev.at(1)) - baselineGaps.at(0)) <= 1
                     && qAbs(gap(dev.at(1), dev.at(2)) - baselineGaps.at(1)) <= 1,
                 "device-discovery row spacing must not grow under splitter drag/resize");

        // The whole preferences row stays content-sized after the resize/drag:
        // the reclaimed height must not stretch the device-discovery box back to
        // its old ~9-line height.
        QVERIFY2(deviceBox->height() <= deviceContentHeight + 2,
                 qPrintable(QStringLiteral("the device discovery group box must stay "
                                           "content-sized after resize/drag (got %1 vs content %2)")
                                .arg(deviceBox->height()).arg(deviceContentHeight)));
    }

    // The capabilities table defaults to a fixed ~16 rows of entries, measured
    // with the same one-line row height the resize pass uses (not the earlier
    // 8-row minimum), and independent of the current row count.
    const int oneLineHeight = window.m_capabilityTable->fontMetrics().lineSpacing() + 10;
    QVERIFY2(qAbs(window.m_capabilityTable->minimumHeight() - 16 * oneLineHeight) <= 1,
             qPrintable(QStringLiteral("the capabilities table must default to ~16 rows "
                                       "(got %1 vs %2)")
                            .arg(window.m_capabilityTable->minimumHeight()).arg(16 * oneLineHeight)));

    // The safety controls stay reachable below the table: they live in the
    // lower pane (flowing vertically after the refresh button), keep a real
    // (non-zero) height, and the outer Settings scroll area can scroll to
    // reveal them instead of clipping them away.
    QVERIFY(lower->isAncestorOf(safetyBox));
    QVERIFY2(safetyBox->height() > 0, "the safety controls must not collapse to zero");
    QScrollArea *settingsScroll = nullptr;
    for (QScrollArea *scroll : window.findChildren<QScrollArea *>()) {
        if (scroll->widget()
            && scroll->widget()->findChild<QSplitter *>(QStringLiteral("settingsSplitter"))) {
            settingsScroll = scroll;
            break;
        }
    }
    QVERIFY2(settingsScroll, "the Settings page must be wrapped in a scroll area");
    {
        const int viewportHeight = settingsScroll->viewport()->height();
        const int contentBottom = settingsScroll->widget()->height();
        const int safetyBottom = safetyBox->mapTo(settingsScroll->widget(),
                                                  QPoint(0, safetyBox->height())).y();
        QVERIFY2(safetyBottom <= contentBottom,
                 "the safety controls must lie within the scrollable content");
        // Scrolling to the bottom brings the safety controls into the viewport.
        settingsScroll->verticalScrollBar()->setValue(settingsScroll->verticalScrollBar()->maximum());
        QApplication::processEvents();
        const int safetyBottomInViewport = safetyBox->mapTo(settingsScroll->viewport(),
                                                            QPoint(0, safetyBox->height())).y();
        QVERIFY2(safetyBottomInViewport <= viewportHeight + 1,
                 "scrolling to the bottom must bring the safety controls into view");

        // The 25%-minimum-visibility floor still holds after the drag/resize.
        const int resizedSplitterHeight = window.m_settingsSplitter->height();
        QVERIFY2(upper->minimumHeight() >= resizedSplitterHeight / 4,
                 "the upper pane must keep its ~25% minimum after resize");
        QVERIFY2(lower->minimumHeight() >= resizedSplitterHeight / 4,
                 "the lower pane must keep its ~25% minimum after resize");

        // A narrow resize reflows the plan (taller upper pane) and grows the
        // splitter's minimum; the 25% floor must track the enlarged splitter
        // instead of keeping a stale fraction.
        window.resize(520, 760);
        QTest::qWait(50);
        QApplication::processEvents();
        const int narrowSplitterHeight = window.m_settingsSplitter->height();
        QVERIFY2(upper->minimumHeight() >= narrowSplitterHeight / 4,
                 "the upper pane must keep its ~25% minimum after a narrow resize");
        QVERIFY2(lower->minimumHeight() >= narrowSplitterHeight / 4,
                 "the lower pane must keep its ~25% minimum after a narrow resize");
    }
}

// The capabilities table's default height is a FIXED ~16-row floor, independent
// of how many rows are currently populated. On first open the table has no
// capability rows (only the header line), and QAbstractScrollArea's
// content-derived minimumSizeHint() is tiny, so the fixed minimum must still
// hold: the table renders at least 16 rows tall with zero rows, and the
// mandatory safety controls stay below it inside the scrollable content.
void MainWindowUiTest::capabilityTableKeepsFixedHeightWhenEmpty()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_tabs->setCurrentIndex(window.m_tabs->indexOf(window.m_settingsPage));
    QTest::qWait(50);
    QApplication::processEvents();

    // Empty the table entirely so the fixed minimum is measured with zero rows.
    window.m_capabilityTable->setRowCount(0);
    QCOMPARE(window.m_capabilityTable->rowCount(), 0);

    // Re-apply the default splitter sizing against the empty table so the
    // content minimum reflects the fixed floor rather than the row count.
    window.updateSettingsSplitterDefaults();
    QTest::qWait(50);
    QApplication::processEvents();

    const int oneLineHeight = window.m_capabilityTable->fontMetrics().lineSpacing() + 10;
    QVERIFY2(oneLineHeight > 0, "the one-line row height must be measurable");

    // The fixed minimum is 16 rows, and it must hold even with zero rows (the
    // table's own minimumSizeHint() collapses to ~1 line when empty, so the
    // explicit setMinimumHeight() floor is what keeps it at 16 rows).
    QVERIFY2(qAbs(window.m_capabilityTable->minimumHeight() - 16 * oneLineHeight) <= 1,
             qPrintable(QStringLiteral("the empty capabilities table must keep its fixed ~16-row "
                                       "minimum (got %1 vs %2)")
                            .arg(window.m_capabilityTable->minimumHeight())
                            .arg(16 * oneLineHeight)));
    QVERIFY2(window.m_capabilityTable->height() >= 16 * oneLineHeight,
             qPrintable(QStringLiteral("the empty capabilities table must render at least 16 rows "
                                       "tall at first open (got %1 vs %2)")
                            .arg(window.m_capabilityTable->height())
                            .arg(16 * oneLineHeight)));

    // The safety controls sit below the (empty) table inside the scrollable
    // content, so the empty-table collapse can never push them out of reach.
    QGroupBox *safetyBox = nullptr;
    for (QGroupBox *group : window.findChildren<QGroupBox *>()) {
        if (group->title() == QStringLiteral("Mandatory safety controls")) {
            safetyBox = group;
        }
    }
    QVERIFY2(safetyBox, "the mandatory safety controls group box must exist");

    QScrollArea *settingsScroll = nullptr;
    for (QScrollArea *scroll : window.findChildren<QScrollArea *>()) {
        if (scroll->widget()
            && scroll->widget()->findChild<QSplitter *>(QStringLiteral("settingsSplitter"))) {
            settingsScroll = scroll;
            break;
        }
    }
    QVERIFY2(settingsScroll, "the Settings page must be wrapped in a scroll area");

    QWidget *content = settingsScroll->widget();
    const int tableBottom = window.m_capabilityTable->mapTo(content,
                                                            QPoint(0, window.m_capabilityTable->height())).y();
    const int safetyTop = safetyBox->mapTo(content, QPoint(0, 0)).y();
    QVERIFY2(tableBottom <= safetyTop,
             qPrintable(QStringLiteral("the safety controls must sit below the empty table "
                                       "(table bottom %1 vs safety top %2)")
                            .arg(tableBottom).arg(safetyTop)));
    QVERIFY2(safetyBox->mapTo(content, QPoint(0, safetyBox->height())).y() <= content->height(),
             "the safety controls must lie within the scrollable content");
}


// (b3) The scope-specific snapshot-rollback marker parses from the cached
// preamble into one row for BOTH scopes, available OR unavailable. The running
// host surfaces `Host snapshot rollback:` (missing-snapper -> snapper); the
// repair target surfaces `Snapshot rollback:` (missing-btrfs-progs ->
// btrfs-progs). The row is never filtered out when available, mirroring the
// storage-stack rows.
void MainWindowUiTest::capabilitySnapshotRollbackRowsParseFromCache()
{
    const QString backend = backendSection(QStringLiteral("dkms"));

    // Repair-target scope, available: the row is present with no package and is
    // not installable.
    const QString availablePreamble = repairCapabilitiesPreamble(
        QStringLiteral("available"), QStringLiteral("dkms executable present"))
        + QStringLiteral(
            "Snapshot rollback: available\n"
            "Snapshot rollback evidence: btrfs root /dev/sda2; top-level @.snapshots\n");
    const QList<Capability> available = CapabilityChecker::capabilitiesFromRepairEvidence(
        availablePreamble, backend);
    QCOMPARE(available.size(), 16); // 15 repair tools + snapshot rollback
    const Capability *availableSnap = snapshotCapabilityOf(available);
    QVERIFY(availableSnap);
    QVERIFY(availableSnap->available);
    QVERIFY(availableSnap->packageName.isEmpty());
    QVERIFY(!availableSnap->installable);
    QVERIFY(availableSnap->targetScope);
    QCOMPARE(availableSnap->scope, QCoreApplication::translate("CapabilityChecker", "Repair target"));
    QCOMPARE(availableSnap->command, QStringLiteral("btrfs root /dev/sda2; top-level @.snapshots"));

    // Repair-target scope, missing btrfs-progs: unavailable, installable
    // btrfs-progs.
    const QString missingTargetPreamble = repairCapabilitiesPreamble(
        QStringLiteral("available"), QStringLiteral("dkms executable present"))
        + QStringLiteral(
            "Snapshot rollback: unavailable|reason:missing-btrfs-progs\n"
            "Snapshot rollback evidence: btrfs\n");
    const QList<Capability> missingTarget = CapabilityChecker::capabilitiesFromRepairEvidence(
        missingTargetPreamble, backend);
    const Capability *missingTargetSnap = snapshotCapabilityOf(missingTarget);
    QVERIFY(missingTargetSnap);
    QVERIFY(!missingTargetSnap->available);
    QCOMPARE(missingTargetSnap->command, QStringLiteral("btrfs"));
    QCOMPARE(missingTargetSnap->packageName, QStringLiteral("btrfs-progs"));
    QVERIFY(missingTargetSnap->installable);
    QVERIFY(missingTargetSnap->targetScope);
    QCOMPARE(missingTargetSnap->note, QStringLiteral("reason:missing-btrfs-progs"));

    // Running-host scope, missing snapper: unavailable, installable snapper.
    const QString missingHostPreamble = repairCapabilitiesPreamble(
        QStringLiteral("available"), QStringLiteral("dkms executable present"))
        + QStringLiteral(
            "Host snapshot rollback: unavailable|reason:missing-snapper\n"
            "Host snapshot rollback evidence: snapper\n");
    const QList<Capability> missingHost = CapabilityChecker::capabilitiesFromRepairEvidence(
        missingHostPreamble, backend, QCoreApplication::translate("CapabilityChecker", "Host"),
        /*targetScope=*/false);
    const Capability *missingHostSnap = snapshotCapabilityOf(missingHost);
    QVERIFY(missingHostSnap);
    QVERIFY(!missingHostSnap->available);
    QCOMPARE(missingHostSnap->command, QStringLiteral("snapper"));
    QCOMPARE(missingHostSnap->packageName, QStringLiteral("snapper"));
    QVERIFY(missingHostSnap->installable);
    QVERIFY(!missingHostSnap->targetScope);
    QCOMPARE(missingHostSnap->scope, QCoreApplication::translate("CapabilityChecker", "Host"));
    QCOMPARE(missingHostSnap->note, QStringLiteral("reason:missing-snapper"));

    // Running-host scope, available: the row is present and not installable.
    const QString availableHostPreamble = repairCapabilitiesPreamble(
        QStringLiteral("available"), QStringLiteral("dkms executable present"))
        + QStringLiteral(
            "Host snapshot rollback: available\n"
            "Host snapshot rollback evidence: snapper 0.11.1; running root @; top-level @.snapshots\n");
    const QList<Capability> availableHost = CapabilityChecker::capabilitiesFromRepairEvidence(
        availableHostPreamble, backend, QCoreApplication::translate("CapabilityChecker", "Host"),
        /*targetScope=*/false);
    const Capability *availableHostSnap = snapshotCapabilityOf(availableHost);
    QVERIFY(availableHostSnap);
    QVERIFY(availableHostSnap->available);
    QVERIFY(availableHostSnap->packageName.isEmpty());
    QVERIFY(!availableHostSnap->installable);
    QVERIFY(!availableHostSnap->targetScope);

    // No snapshot marker (an older cache) keeps exactly the 15 tool rows.
    const QList<Capability> plain = CapabilityChecker::capabilitiesFromRepairEvidence(
        repairCapabilitiesPreamble(QStringLiteral("available"),
                                   QStringLiteral("dkms executable present")),
        backend);
    QCOMPARE(plain.size(), 15);
    QVERIFY(!snapshotCapabilityOf(plain));
}

// (b4) The derived capability rows are cached per scope: switching the scope
// selector host -> target -> host never re-runs diagnostics, and the target
// table stays populated from its (untouched) cached evidence.
void MainWindowUiTest::capabilityRowsCachedAcrossScopeSwitch()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    prepareRepairScope(window, /*host=*/false);

    const QString preamble = repairCapabilitiesPreamble(
        QStringLiteral("available"), QStringLiteral("dkms executable present"));
    const QString backend = backendSection(QStringLiteral("dkms"));
    window.m_targetDiagnosticCache.insert(QStringLiteral("capabilities"), preamble);
    window.m_targetDiagnosticCache.insert(QStringLiteral("backend"), backend);

    // Default (target) scope shows the target rows.
    window.m_capabilityScopeSelector->setCurrentIndex(0);
    window.refreshCapabilities();
    QVERIFY2(rowForCommand(window, QStringLiteral("dkms")) >= 0, "target scope shows the dkms row");

    // Switch to Host (empty host cache: the table shows the placeholder), then
    // back to target. The target cache must survive the round-trip untouched.
    window.m_capabilityScopeSelector->setCurrentIndex(1);
    window.refreshCapabilities();
    QCOMPARE(window.m_capabilityTable->rowCount(), 1);
    QCOMPARE(cellText(window, 0, 0), QStringLiteral("No capability evidence cached"));

    window.m_capabilityScopeSelector->setCurrentIndex(0);
    window.refreshCapabilities();
    const int dkmsRow = rowForCommand(window, QStringLiteral("dkms"));
    QVERIFY2(dkmsRow >= 0, "the target capabilities must still be populated after a host-scope round-trip");
    QCOMPARE(cellText(window, dkmsRow, 2),
             QCoreApplication::translate("CapabilityChecker", "Repair target"));
}

// (b4b) The REAL scope switch (Host Maintenance toggle) must preserve the
// committed repair target's diagnostic cache: entering Host Maintenance parks
// the target, and leaving it restores the target so its cached capability rows
// re-render without a re-diagnosis. Running-host diagnostics run in between
// must not clear the parked target cache, and switching the scope selector back
// to "Repair target" while maintenance is still active must re-render those
// rows (the identity keeps naming the parked target, not the cleared preview
// path).
void MainWindowUiTest::capabilityRowsSurviveHostMaintenanceRoundTrip()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;
    window.m_autoRefreshDiagnostics->setChecked(false);
    window.m_privilegedSessionReady = true;
    prepareRepairScope(window, /*host=*/false);
    // Resolve the protected host identity so selectHostForMaintenance() can
    // commit the running host, then return to target mode.
    prepareRepairScope(window, /*host=*/true);
    window.m_hostMaintenanceMode = false;

    const QString preamble = repairCapabilitiesPreamble(
        QStringLiteral("available"), QStringLiteral("dkms executable present"));
    const QString backend = backendSection(QStringLiteral("dkms"));
    window.m_targetDiagnosticCache.insert(QStringLiteral("capabilities"), preamble);
    window.m_targetDiagnosticCache.insert(QStringLiteral("backend"), backend);
    window.m_targetDiagnosticCacheIdentity = window.currentTargetDiagnosticCacheIdentity();

    // Default (target) scope shows the target rows.
    window.m_capabilityScopeSelector->setCurrentIndex(0);
    window.refreshCapabilities();
    QVERIFY2(rowForCommand(window, QStringLiteral("dkms")) >= 0, "target scope shows the dkms row");

    // Enter Host Maintenance: the committed target is parked (cleared from the
    // Systems tree), not discarded.
    window.selectHostForMaintenance();
    QVERIFY(window.m_hostMaintenanceMode);
    QVERIFY(window.m_previewTargetPath.isEmpty());

    // Run running-host diagnostics while maintenance is active (the host
    // round-trip the bug report describes). This must not touch the parked
    // target's cache.
    window.runAllDiagnostics();
    QVERIFY2(!window.m_hostDiagnosticCache.value(QStringLiteral("report")).trimmed().isEmpty(),
             "running-host diagnostics must populate the host cache");

    // Switching the capability scope selector back to "Repair target" while
    // Host Maintenance is still active must re-render the parked target's
    // cached rows.
    window.m_capabilityScopeSelector->setCurrentIndex(0);
    window.refreshCapabilities();
    int dkmsRow = rowForCommand(window, QStringLiteral("dkms"));
    QVERIFY2(dkmsRow >= 0,
             "the target capabilities must survive the host round-trip with host diagnostics run in between");
    QCOMPARE(cellText(window, dkmsRow, 2),
             QCoreApplication::translate("CapabilityChecker", "Repair target"));

    // Leave Host Maintenance: the committed target is restored.
    window.exitHostMaintenanceMode();
    QVERIFY(!window.m_hostMaintenanceMode);
    QVERIFY(!window.m_previewTargetPath.isEmpty());
    QCOMPARE(window.m_previewTargetPath, QStringLiteral("/dev/test-system"));

    // The target capability rows must still be populated from the untouched
    // target cache.
    window.refreshCapabilities();
    dkmsRow = rowForCommand(window, QStringLiteral("dkms"));
    QVERIFY2(dkmsRow >= 0, "the target capabilities must still be populated after a host-maintenance round-trip");
    QCOMPARE(cellText(window, dkmsRow, 2),
             QCoreApplication::translate("CapabilityChecker", "Repair target"));
}

// (b5) The Boot capabilities table Install action routes host-scope rows to the
// guarded install-host-package verb (on the running host) and target-scope rows
// to install-target-package. The helper command/arguments name the exact scope.
void MainWindowUiTest::capabilityInstallRoutesHostVsTarget()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    // Host-scope install: install-host-package on the running host identity.
    {
        MainWindow window;
        window.show();
        QTest::qWait(50);
        window.m_snapshotPreloadScheduled = true;
        window.m_autoRefreshDiagnostics->setChecked(false);
        prepareRepairScope(window, /*host=*/true);
        QString evidence = capabilityEvidence(false, true);
        evidence += QStringLiteral("Host snapshot rollback: unavailable|reason:missing-snapper\n");
        cacheRepairEvidence(window, evidence);

        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString capturePath = dir.filePath(QStringLiteral("host-install.log"));
        QVERIFY(startFakePrivilegedSession(window, capturePath, QStringLiteral("success")));
        closeRepairProgressDialogWhenDone(&window);
        acceptNextMessageBox(&window, QMessageBox::Yes);

        window.installMissingCapability(QStringLiteral("snapper"), /*targetScope=*/false);

        const QStringList args = capturedHelperArguments(capturePath);
        QCOMPARE(args, QStringList({QStringLiteral("install-host-package"),
                                    QStringLiteral("/dev/test-host"),
                                    QStringLiteral("/dev/test-host-root"),
                                    QStringLiteral("snapper")}));
    }

    // Target-scope install: install-target-package on the selected target.
    {
        MainWindow window;
        window.show();
        QTest::qWait(50);
        window.m_snapshotPreloadScheduled = true;
        window.m_autoRefreshDiagnostics->setChecked(false);
        prepareRepairScope(window, /*host=*/false);
        QString evidence = capabilityEvidence(false, true);
        evidence += QStringLiteral("Snapshot rollback: unavailable|reason:missing-btrfs-progs\n");
        cacheRepairEvidence(window, evidence);

        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString capturePath = dir.filePath(QStringLiteral("target-install.log"));
        QVERIFY(startFakePrivilegedSession(window, capturePath, QStringLiteral("success")));
        closeRepairProgressDialogWhenDone(&window);
        acceptNextMessageBox(&window, QMessageBox::Yes);

        window.installMissingCapability(QStringLiteral("btrfs-progs"), /*targetScope=*/true);

        const QStringList args = capturedHelperArguments(capturePath);
        QCOMPARE(args, QStringList({QStringLiteral("install-target-package"),
                                    QStringLiteral("/dev/test-system"),
                                    QStringLiteral("/dev/test-root"),
                                    QStringLiteral("btrfs-progs")}));
    }
}

// rawCapabilityReasonKey() reads the active-scope capability cache and must
// apply the same target-identity guard as repairToolAvailable()/capabilityState():
// a stale/foreign target cache returns empty (fail-closed) instead of yielding a
// reason key that names another target's evidence. Host scope is exempt (the
// host cache is keyed by the explicit Host Maintenance scope, not the target).
void MainWindowUiTest::rawCapabilityReasonKeyFailsClosedOnForeignTargetCache()
{
    MainWindow window;
    window.show();
    QTest::qWait(50);
    window.m_snapshotPreloadScheduled = true;

    // A matching (fresh) target cache yields the reason key.
    prepareRepairScope(window, /*host=*/false);
    QString evidence = capabilityEvidence(false, false);
    evidence.replace(QStringLiteral("Repair tool dkms: unavailable|DKMS is not installed"),
                     QStringLiteral("Repair tool dkms: unavailable|reason:missing-dkms"));
    cacheRepairEvidence(window, evidence);
    QCOMPARE(window.rawCapabilityReasonKey(QStringLiteral("dkms")), QStringLiteral("missing-dkms"));

    // Selecting a different target makes the cache foreign: fail closed empty.
    window.m_previewTargetPath = QStringLiteral("/dev/another-target");
    QCOMPARE(window.rawCapabilityReasonKey(QStringLiteral("dkms")), QString());

    // Host scope has no target-identity guard and keeps its own cached key.
    prepareRepairScope(window, /*host=*/true);
    QString hostEvidence = capabilityEvidence(false, false);
    hostEvidence.replace(QStringLiteral("Repair tool dkms: unavailable|DKMS is not installed"),
                         QStringLiteral("Repair tool dkms: unavailable|reason:missing-dkms"));
    cacheRepairEvidence(window, hostEvidence);
    QCOMPARE(window.rawCapabilityReasonKey(QStringLiteral("dkms")), QStringLiteral("missing-dkms"));
}
