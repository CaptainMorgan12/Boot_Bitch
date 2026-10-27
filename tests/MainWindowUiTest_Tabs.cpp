// Part of the boot-repair-ui-tests read-only regression suite: dynamic tab
// visibility for the Snapshots and EFI boot entries tabs. The two dynamic tabs
// are fully REMOVED from the tab bar when unavailable (never merely greyed out
// with setTabEnabled), and logical visibility is exposed through
// snapshotsTabVisible()/bootEntriesTabVisible() so the tests assert hidden/shown
// without depending on the tab-bar's shifting indices. Everything derives from
// the cached capability preamble; nothing here issues a privileged request.
#include "MainWindowUiTest.h"
#include "UiTestHelpers.h"

using namespace UiTestDetail;

namespace {

// True when the given logical page widget is actually present in the tab bar
// (the removeTab/insertTab outcome), independent of its logical AppTab index.
bool tabIsPresent(MainWindow &window, QWidget *page)
{
    return page && window.m_tabs->indexOf(page) >= 0;
}

} // namespace

// The Snapshots tab follows the COMPLETED SNAPSHOT INVENTORY: it is hidden
// until the scope's inventory found at least one snapshot, and shown whenever
// it did — even when the rollback capability is refused (separate /boot,
// non-@ root, no snapper config), because the read-only listing still works.
// The Rollback button carries the rollback guard separately.  A missing
// producer tool hides the tab too: the install affordance lives in the Boot
// capabilities table, never a hidden Snapshots tab showing an empty list.
void MainWindowUiTest::snapshotsTabVisibilityFollowsSnapshotCapability()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    // Non-Btrfs root (no snapshot support): the tab is hidden.
    {
        MainWindow window;
        window.show();
        QTest::qWait(50);
        window.m_snapshotPreloadScheduled = true;
        prepareRepairScope(window, /*host=*/false);
        QString evidence = capabilityEvidence(false, true);
        evidence += QStringLiteral("Snapshot rollback: unavailable|reason:not-btrfs-root|param:ext4\n");
        cacheRepairEvidence(window, evidence);
        window.refreshCapabilities();

        QVERIFY(!window.snapshotsTabVisible());
        QVERIFY(!tabIsPresent(window, window.m_snapshotsPage));
    }

    // No snapshot-rollback evidence at all: the tab is hidden (fail closed).
    {
        MainWindow window;
        window.show();
        QTest::qWait(50);
        window.m_snapshotPreloadScheduled = true;
        prepareRepairScope(window, /*host=*/false);
        cacheRepairEvidence(window, capabilityEvidence(false, true));
        window.refreshCapabilities();

        QVERIFY(!window.snapshotsTabVisible());
        QVERIFY(!tabIsPresent(window, window.m_snapshotsPage));
    }

    // Available capability but NO inventory yet: the tab stays hidden until
    // the inventory completes (the inventory drives the visibility).
    {
        MainWindow window;
        window.show();
        QTest::qWait(50);
        window.m_snapshotPreloadScheduled = true;
        prepareRepairScope(window, /*host=*/false);
        QString evidence = capabilityEvidence(false, true);
        evidence += QStringLiteral("Snapshot rollback: available\n");
        cacheRepairEvidence(window, evidence);
        window.refreshCapabilities();

        QVERIFY(!window.snapshotsTabVisible());
        QVERIFY(!tabIsPresent(window, window.m_snapshotsPage));
    }

    // A completed inventory shows the tab even when the rollback is refused
    // (the Fedora separate-/boot case): the listing works, the Rollback
    // button carries the specific refusal.
    {
        MainWindow window;
        window.show();
        QTest::qWait(50);
        window.m_snapshotPreloadScheduled = true;
        prepareRepairScope(window, /*host=*/false);
        QString evidence = capabilityEvidence(false, true);
        evidence += QStringLiteral("Snapshot rollback: unavailable|reason:separate-boot\n");
        cacheRepairEvidence(window, evidence);
        window.refreshCapabilities();
        window.m_snapshotInventoryCount = 1;
        window.updateTabVisibility();

        QVERIFY(window.snapshotsTabVisible());
        QVERIFY(tabIsPresent(window, window.m_snapshotsPage));
    }

    // A missing producer tool HIDES the tab: the install action belongs to the
    // Boot capabilities table, not a hidden Snapshots tab with an empty list.
    {
        MainWindow window;
        window.show();
        QTest::qWait(50);
        window.m_snapshotPreloadScheduled = true;
        prepareRepairScope(window, /*host=*/false);
        QString evidence = capabilityEvidence(false, true);
        evidence += QStringLiteral("Snapshot rollback: unavailable|reason:missing-btrfs-progs\n");
        cacheRepairEvidence(window, evidence);
        window.refreshCapabilities();

        QVERIFY(!window.snapshotsTabVisible());
        QVERIFY(!tabIsPresent(window, window.m_snapshotsPage));
    }
}

// The EFI boot entries tab is hidden (removed) when the selected scope has no
// boot-entry or boot-menu information (no UEFI firmware boot path AND no
// GRUB/extlinux boot menu) and shown when a boot path or boot menu is present.
void MainWindowUiTest::bootEntriesTabVisibilityFollowsBootEvidence()
{
    ScopedSessionLogDir logDir;
    QVERIFY(logDir.isValid());

    // BIOS-only target with no GRUB/extlinux menu: every boot-relevant
    // capability key is unavailable, so the tab is hidden.
    {
        MainWindow window;
        window.show();
        QTest::qWait(50);
        window.m_snapshotPreloadScheduled = true;
        prepareRepairScope(window, /*host=*/false);
        QString evidence = capabilityEvidence(false, true);
        evidence.replace(QStringLiteral("Repair tool efi: available"),
                         QStringLiteral("Repair tool efi: unavailable|reason:legacy-bios-no-efi"));
        evidence.replace(QStringLiteral("Repair tool grub: available"),
                         QStringLiteral("Repair tool grub: unavailable|reason:missing-grub-config"));
        cacheRepairEvidence(window, evidence);
        window.refreshCapabilities();

        QVERIFY(!window.bootEntriesTabVisible());
        QVERIFY(!tabIsPresent(window, window.m_bootEntriesPage));
    }

    // No boot-relevant evidence at all: the tab is hidden (fail closed).
    {
        MainWindow window;
        window.show();
        QTest::qWait(50);
        window.m_snapshotPreloadScheduled = true;
        prepareRepairScope(window, /*host=*/false);
        window.refreshCapabilities();

        QVERIFY(!window.bootEntriesTabVisible());
        QVERIFY(!tabIsPresent(window, window.m_bootEntriesPage));
    }

    // A UEFI firmware boot path (efi available) shows the tab.
    {
        MainWindow window;
        window.show();
        QTest::qWait(50);
        window.m_snapshotPreloadScheduled = true;
        prepareRepairScope(window, /*host=*/false);
        cacheRepairEvidence(window, capabilityEvidence(false, true)); // efi + grub available
        window.refreshCapabilities();

        QVERIFY(window.bootEntriesTabVisible());
        QVERIFY(tabIsPresent(window, window.m_bootEntriesPage));
    }

    // A GRUB boot menu alone (BIOS target, grub available, efi unavailable)
    // still shows the tab.
    {
        MainWindow window;
        window.show();
        QTest::qWait(50);
        window.m_snapshotPreloadScheduled = true;
        prepareRepairScope(window, /*host=*/false);
        QString evidence = capabilityEvidence(false, true);
        evidence.replace(QStringLiteral("Repair tool efi: available"),
                         QStringLiteral("Repair tool efi: unavailable|reason:legacy-bios-no-efi"));
        cacheRepairEvidence(window, evidence); // grub stays available
        window.refreshCapabilities();

        QVERIFY(window.bootEntriesTabVisible());
        QVERIFY(tabIsPresent(window, window.m_bootEntriesPage));
    }
}
