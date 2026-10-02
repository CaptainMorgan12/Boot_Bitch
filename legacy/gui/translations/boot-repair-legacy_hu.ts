<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS language="hu">
<context>
    <name>LegacyMainWindow</name>
    <message>
        <source>Validate environment</source>
        <translation>A környezet jóváhagyása</translation>
    </message>
    <message>
        <source>Validate</source>
        <translation>Jóváhagyás</translation>
    </message>
    <message>
        <source>Check the selected system&apos;s mounts, filesystem metadata, boot files, mapper consistency and dependency readiness before any repair action. This is an independent safety preflight rather than an optional Full Repair stage.</source>
        <translation>Ellenőrizzék a kiválasztott rendszer szerelvényeit, fájlrendszer metaadatait, boot fájlokat, a térkép konzisztenciáját és a függőségi készséget a javítás előtt. Ez egy független biztonsági repülés előtt, nem egy opcionális teljes javítási szakasz.</translation>
    </message>
    <message>
        <source>Always preflight</source>
        <translation>Mindig repülés előtt</translation>
    </message>
    <message>
        <source>File system repair</source>
        <translation>Fájlrendszer javítása</translation>
    </message>
    <message>
        <source>Check File Systems</source>
        <translation>Fájlrendszerek ellenőrzése</translation>
    </message>
    <message>
        <source>Run the read-only file system check for the selected system&apos;s root and /boot filesystems and report each device&apos;s check tool and result without changing anything. This legacy frontend exposes the read-only check only; device repair is not wired.</source>
        <translation>Futtasd le az olvasófájlrendszer ellenőrzését a kiválasztott rendszer root és / boot fájlrendszereire, és jelentsd be minden eszköz ellenőrző eszközét és eredményét anélkül, hogy bármit megváltoztatnál. Ez a hagyatéki front csak a leolvasott ellenőrzést mutatja ki; az eszközjavítás nem vezetékes.</translation>
    </message>
    <message>
        <source>Full Repair plan: unavailable on this frontend - read-only check only</source>
        <translation>Teljes javítási terv: ezen a fronton nem érhető el - read- only check</translation>
    </message>
    <message>
        <source>Complete package configuration</source>
        <translation>A csomag teljes konfigurációja</translation>
    </message>
    <message>
        <source>Complete Configuration</source>
        <translation>Teljes beállítás</translation>
    </message>
    <message>
        <source>Complete interrupted dpkg package configuration in the selected repair system. This is the same stage controlled by Settings -&gt; Full Repair plan -&gt; Complete interrupted package configuration, but it can also be run independently here.</source>
        <translation>Teljes megszakított dpkg csomag konfiguráció a kiválasztott javítási rendszerben. Ez ugyanaz a szakasz által ellenőrzött Beállítások - &gt; Teljes javítási terv - &gt; Teljes megszakított csomag konfiguráció, de itt is lehet futtatni függetlenül.</translation>
    </message>
    <message>
        <source>Run the guarded dpkg-configure repair? The helper keeps its package-lock and runtime preflights.</source>
        <translation>Futtassuk le az őrzött dpkg- konfigurálást? A segítő megtartja a csomagját és a kifutós lámpáit.</translation>
    </message>
    <message>
        <source>Repair broken dependencies</source>
        <translation>Törött függések javítása</translation>
    </message>
    <message>
        <source>Repair Dependencies</source>
        <translation>Javítási követelmények</translation>
    </message>
    <message>
        <source>Repair package dependencies in the selected repair system after the mandatory safety preflight. This maps directly to Settings -&gt; Repair broken package dependencies.</source>
        <translation>Javítási csomag függések a kiválasztott javítási rendszer után kötelező biztonsági repülés. Ez a térkép közvetlenül a Beállítások - &gt; Javítás törött csomag függések.</translation>
    </message>
    <message>
        <source>Run the guarded fix-broken repair? The helper keeps its simulation-first preflight and runtime guards.</source>
        <translation>Lefuttatni az őrzött javítást? A segítő megtartja a szimulációját, az első repülés előtti és futási idejű őröket.</translation>
    </message>
    <message>
        <source>Refresh package metadata</source>
        <translation>Csomagmetaadatok frissítése</translation>
    </message>
    <message>
        <source>Refresh Metadata</source>
        <translation>A metaadatok frissítése</translation>
    </message>
    <message>
        <source>Refresh APT metadata in the selected repair system without upgrading installed packages. This maps directly to Settings -&gt; Refresh package metadata.</source>
        <translation>Az APT metaadatok frissítése a kiválasztott javítási rendszerben a telepített csomagok korszerűsítése nélkül. Ez a térkép közvetlenül a Beállítások - &gt; Csomagmetaadatok frissítése.</translation>
    </message>
    <message>
        <source>Refresh package metadata for the selected scope? The helper requires a reachable, trusted APT source.</source>
        <translation>A csomag metaadatainak frissítése a kiválasztott hatókörhöz? A segítőnek szüksége van egy elérhető, megbízható APT forrásra.</translation>
    </message>
    <message>
        <source>Upgrade installed packages</source>
        <translation>Telepített csomagok frissítése</translation>
    </message>
    <message>
        <source>Simulate and Upgrade</source>
        <translation>Szimuláció és korszerűsítés</translation>
    </message>
    <message>
        <source>Simulate the APT transaction first, inspect proposed removals, then apply a safe upgrade. This maps directly to Settings -&gt; Upgrade installed packages.</source>
        <translation>Először szimuláljuk az APT tranzakciót, megvizsgáljuk a javasolt eltávolításokat, majd biztonságos frissítést alkalmazunk. Ez a térkép közvetlenül a Beállítások - &gt; A telepített csomagok frissítése.</translation>
    </message>
    <message>
        <source>Run the guarded apt-upgrade transaction? The helper keeps its simulation-first and source guards.</source>
        <translation>Lefuttatni az őrzött apt-upgrade tranzakciót? A segítő megtartja a szimulációját, az első és a forrásőröket.</translation>
    </message>
    <message>
        <source>DKMS</source>
        <translation>DKMS</translation>
    </message>
    <message>
        <source>Rebuild DKMS</source>
        <translation>DKMS újjáépítése</translation>
    </message>
    <message>
        <source>Rebuild out-of-tree kernel modules for kernels installed in the selected system. The helper refuses this action when DKMS is not installed; this legacy frontend exposes no DKMS action.</source>
        <translation>A kiválasztott rendszerbe telepített magmagmagmagokhoz a fa magmagmagmodulokat építsük újra. A segítő elutasítja ezt az intézkedést, ha a DKMS nincs telepítve; ez a hagyaték frontend nem mutat DKMS műveletet.</translation>
    </message>
    <message>
        <source>Graphical login / display manager</source>
        <translation>Grafikus bejelentkezés / kijelző</translation>
    </message>
    <message>
        <source>Restore Graphical Login</source>
        <translation>Grafikus bejelentkezés visszaállítása</translation>
    </message>
    <message>
        <source>Restore the legacy SysV display manager configured for the running host: the /etc/X11/default-display-manager entry and the missing runlevel S-symlink, with a backup and rollback, never starting the GUI. This is a host-scope stage on this legacy frontend.</source>
        <translation>Az / etc / X11 / default- display- manager bejegyzés és a hiányzó Runlevel S- szimlink, mentéssel és visszafordítással, a GUI elindításával. Ez egy hostscore szakasz ezen a hagyatéki fronton.</translation>
    </message>
    <message>
        <source>Restore the graphical login configuration for the running host? The helper backs up /etc/X11/default-display-manager and the runlevel symlink state, restores the configured entry and the missing S-symlink, rolls back on any failure, and never starts the display manager.</source>
        <translation>Vissza a grafikus bejelentkezési konfiguráció a futó host? A segítő támogatja az / etc / X11 / default- display- managert és a futásszintű szimlink állapotot, visszaállítja a beállított bejegyzést és a hiányzó S- szimlinket, visszaáll minden hibára, és soha nem indítja el a kijelzőt.</translation>
    </message>
    <message>
        <source>Initramfs</source>
        <translation>InitramfName</translation>
    </message>
    <message>
        <source>Rebuild Initramfs</source>
        <translation>Initramfs újjáépítése</translation>
    </message>
    <message>
        <source>Rebuild initramfs images for the selected repair system only after mapper and crypttab consistency checks pass. The helper backs up each image before the apply. On Etch the stage runs through the guarded plain-chroot fallback (no unshare required).</source>
        <translation>A initramfs képek újraépítése a kiválasztott javítórendszerhez csak a térkép és a crypttab konzisztencia ellenőrzése után. A segítő az alkalmazás előtt minden képet megerősít. Az Etch a színpad fut keresztül az őrzött egyszerű-chroot visszaesés (nincs szükség megosztásra).</translation>
    </message>
    <message>
        <source>Rebuild the initramfs for the selected scope? The helper keeps its mapper/crypttab and backup preflights.</source>
        <translation>Újjáépíti a initramfs-et a kiválasztott hatókörhöz? A segítő megtartja a mapper / crypttab és tartalék preflights.</translation>
    </message>
    <message>
        <source>EFI / UKI bootloader</source>
        <translation>EFI / UKI bootloader</translation>
    </message>
    <message>
        <source>Repair EFI / UKI</source>
        <translation>EFI / UKI javítás</translation>
    </message>
    <message>
        <source>Repair the selected system&apos;s EFI / UKI boot path. This legacy frontend exposes no EFI action; the Etch target is a BIOS/GRUB-legacy system.</source>
        <translation>A kijelölt rendszer EFI / UKI rendszerindítási útvonalának javítása. Ez a hagyatéki front nem mutat EFI tevékenységet; az Etch cél egy BIOS / GRUB- örökölt rendszer.</translation>
    </message>
    <message>
        <source>GRUB configuration</source>
        <translation>GRUB konfiguráció</translation>
    </message>
    <message>
        <source>Regenerate GRUB</source>
        <translation>GRUB regeneráció</translation>
    </message>
    <message>
        <source>Regenerate the selected repair system&apos;s GRUB menu/configuration after the mandatory safety preflight. The helper backs up menu.lst, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Regenerálja a kiválasztott javítórendszer GRUB menüjét / konfigurációját a kötelező repülés előtt. A segítő támogatja a menu.lst-et, megőriz minden létező boot bejegyzést, és minden hibára visszafordul.</translation>
    </message>
    <message>
        <source>Regenerate the GRUB configuration? The helper backs up the target menu/configuration, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Újraéleszteni a GRUB konfigurációt? A segítő támogatja a cél menü / konfiguráció, megőrzi minden meglévő boot bejegyzést, és gurul vissza minden hiba.</translation>
    </message>
    <message>
        <source>extlinux configuration</source>
        <translation>extlinux konfiguráció</translation>
    </message>
    <message>
        <source>Regenerate extlinux</source>
        <translation>A extlinux regenerálása</translation>
    </message>
    <message>
        <source>Regenerate the selected system&apos;s extlinux bootloader configuration. This legacy frontend exposes no extlinux action.</source>
        <translation>A kijelölt rendszer extlinux bootloader konfigurációjának visszaállítása. Ez a hagyaték nem mutat extlinux akciót.</translation>
    </message>
    <message>
        <source>Boot stack reconciliation</source>
        <translation>Boot stack megbékélés</translation>
    </message>
    <message>
        <source>Reconcile Boot Stack</source>
        <translation>Reconcile Boot Stack</translation>
    </message>
    <message>
        <source>Reconcile the selected repair system&apos;s boot stack in one guarded legacy pass: mapper/crypttab validation, initramfs rebuild and GRUB-legacy configuration regeneration, with the component backups and preflights unchanged. This is the Etch equivalent of the modern boot-stack reconciliation and stays out of the Full Repair plan.</source>
        <translation>A kiválasztott javítórendszer bakancskészletének rekonstruálása egy őrzött öröklődésben: mapper / crypttab validálás, initramfs újjáépítés és GRUB- restate konfigurációs regeneráció, a komponens mentések és a preflights változatlanul. Ez az Etch megfelelője a modern boot- stack megbékélésnek, és kimarad a teljes javítási tervből.</translation>
    </message>
    <message>
        <source>Full Repair plan: Manual recovery tool</source>
        <translation>Teljes javítási terv: Kézi helyreállítási eszköz</translation>
    </message>
    <message>
        <source>Run the guarded boot-stack reconciliation? The helper runs the mapper/crypttab validation, the initramfs rebuild and the GRUB-legacy regeneration in one pass with every component preflight and backup.</source>
        <translation>Lefuttatni az őrzött készleteket? A segítő futtatja a mapper / crypttab validálást, a initramfs újjáépítését és a GRUB- resource regenerációt egy pass-ban minden komponens repülés előtti és tartalék.</translation>
    </message>
    <message>
        <source>Complete interrupted package configuration</source>
        <translation>Teljes megszakított csomagkonfiguráció</translation>
    </message>
    <message>
        <source>Repair broken package dependencies</source>
        <translation>Törött csomagolási függések javítása</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Alapértelmezés</translation>
    </message>
    <message>
        <source>Repair file system errors (read-only check first)</source>
        <translation>Javítás fájlrendszer hibák (read- only check first)</translation>
    </message>
    <message>
        <source>the legacy frontend exposes the read-only file system check only; per-device repair is not wired on this frontend (fail closed)</source>
        <translation>a hagyatéki frontend csak a read- only file system check-et tárja fel; a per- device javítás nem vezetékes ezen a frontensen (nem sikerült bezárni)</translation>
    </message>
    <message>
        <source>Upgrade installed packages (adaptive APT simulation)</source>
        <translation>Telepített csomagok frissítése (adaptív APT szimuláció)</translation>
    </message>
    <message>
        <source>Rebuild DKMS modules</source>
        <translation>DKMS modulok újjáépítése</translation>
    </message>
    <message>
        <source>Restore graphical login manager</source>
        <translation>Grafikus bejelentkezési kezelő helyreállítása</translation>
    </message>
    <message>
        <source>Rebuild initramfs after mapper/crypttab validation</source>
        <translation>A initramfs újraépítése a mappa / crypttab validálása után</translation>
    </message>
    <message>
        <source>Repair EFI / UKI boot path</source>
        <translation>EFI / UKI boot path javítása</translation>
    </message>
    <message>
        <source>Update GRUB configuration</source>
        <translation>GRUB konfiguráció frissítése</translation>
    </message>
    <message>
        <source>Update extlinux configuration</source>
        <translation>extlinux konfiguráció frissítése</translation>
    </message>
    <message>
        <source>Choose Full Repair stages in Settings. Enabled stages run in the order shown. Each configurable stage also appears below as an individual tool; the Full Repair column mirrors its current Settings state. The boot tools (EFI / UKI bootloader, GRUB or extlinux configuration, boot-stack reconciliation and Make Default) are independent: run them in any order, and a later action re-verifies what an earlier one changed and reports its own result. The active scope is shown beside Repair: selected repair drive or Running Host maintenance.</source>
        <translation>Válassza ki a Beállítások teljes javítási szakaszait. Engedélyezett szakaszok fut a megadott sorrendben. Minden konfigurálható szakasz az alábbiakban is megjelenik, mint egy egyedi eszköz; a Teljes Javítás oszlop tükrözi a jelenlegi Beállítások állapotát. A boot tooler eszközök (EFI / UKI bootloader, GRUB vagy extlinux konfiguráció, boot- stack megbékélés és Make default) függetlenek: futtatják őket minden sorrendben, és egy későbbi művelet újra ellenőrzi, hogy egy korábbi mit változott, és jelenti a saját eredményét. Az aktív hatókör a Javítás mellett látható: a kiválasztott javítási meghajtó vagy a Running Host karbantartás.</translation>
    </message>
    <message>
        <source>Run All diagnostics for the selected target or running host before starting Full Repair. The report is read-only evidence used to choose and confirm repair stages.</source>
        <translation>Futtasd le az összes diagnosztikát a kiválasztott célhoz vagy futtasd a gazdatestet a teljes javítás megkezdése előtt. A jelentés csak a javítási szakaszok kiválasztására és megerősítésére használható.</translation>
    </message>
    <message>
        <source>Ready: required cached read-only diagnostics are available for the selected stages. Review them in Diagnostics or Logs before confirming.</source>
        <translation>Kész: szükséges tárolt olvasó- csak diagnosztika áll rendelkezésre a kiválasztott szakaszok. Vizsgálja meg őket Diagnosztika vagy Logs, mielőtt megerősíti.</translation>
    </message>
    <message>
        <source>Environment validation</source>
        <translation>Környezeti hitelesítés</translation>
    </message>
    <message>
        <source>Summarizes the selected system, protection state, mounted identity and inspection readiness.</source>
        <translation>Összefoglalja a kiválasztott rendszert, védelmi állapotot, felszerelt személyazonosságot és ellenőrzési készséget.</translation>
    </message>
    <message>
        <source>Distribution and boot backend profile</source>
        <translation>Distribution and boot backend profil</translation>
    </message>
    <message>
        <source>Identifies the distribution family, package manager, initramfs generator, bootloader and current guarded repair capability.</source>
        <translation>Azonosítja az elosztó családot, csomagkezelő, initramfs generátor, bootloader és jelenlegi őrzött javítási képesség.</translation>
    </message>
    <message>
        <source>Boot diagnostics</source>
        <translation>Boot diagnosztika</translation>
    </message>
    <message>
        <source>Shows boot mounts and /boot contents plus storage evidence without changing the selected system.</source>
        <translation>Megjeleníti a boot mounts és / boot tartalom plusz tárolási bizonyíték megváltoztatása nélkül a kiválasztott rendszer.</translation>
    </message>
    <message>
        <source>Boot evidence and selection history</source>
        <translation>Boot bizonyíték és kiválasztási előzmények</translation>
    </message>
    <message>
        <source>Correlates the detected boot chain, bootloader selection, kernel/initramfs and unlock evidence.</source>
        <translation>Korreláció az észlelt boot lánc, bootloader kiválasztása, kernel / initramfs és kinyit bizonyítékot.</translation>
    </message>
    <message>
        <source>Kernel / initramfs</source>
        <translation>Kernel / initramfs</translation>
    </message>
    <message>
        <source>Reviews kernel files and verifies matching initramfs images through a read-only inspection.</source>
        <translation>Felülvizsgálja a kernel fájlokat, és ellenőrzi az egyező initramfs képeket egy csak olvasható ellenőrzés.</translation>
    </message>
    <message>
        <source>Reviews the GRUB configuration without changing boot files.</source>
        <translation>Áttekinti a GRUB konfigurációt anélkül, hogy megváltoztatná a boot fájlokat.</translation>
    </message>
    <message>
        <source>EFI / UKI boot state</source>
        <translation>EFI / UKI boot state</translation>
    </message>
    <message>
        <source>Inspects EFI/UKI evidence; unavailable on this legacy BIOS frontend with the helper&apos;s probe reason.</source>
        <translation>Ellenőrzi EFI / UKI bizonyíték; nem elérhető ezen örökölt BIOS frontend a segítő szonda oka.</translation>
    </message>
    <message>
        <source>Reviews the configured display manager and recent boot evidence without starting the GUI.</source>
        <translation>Áttekinti a konfigurált kijelző manager és a legújabb boot bizonyíték indítása nélkül a GUI.</translation>
    </message>
    <message>
        <source>Boot errors</source>
        <translation>Elindítási hibák</translation>
    </message>
    <message>
        <source>Reads recent error-priority entries from the running host or selected repair system when available.</source>
        <translation>A futtató gazdatestből vagy a kiválasztott javítórendszerből a legújabb hibaprioritás bejegyzéseket olvassa, ha van ilyen.</translation>
    </message>
    <message>
        <source>Disk usage</source>
        <translation>Lemezhasználat</translation>
    </message>
    <message>
        <source>Summarizes filesystem capacity and free space for the running host or read-only repair target.</source>
        <translation>Összefoglalja a fájlrendszer kapacitását és szabad teret a futó gazdatest vagy csak read- javítási cél számára.</translation>
    </message>
    <message>
        <source>File systems</source>
        <translation>Fájlrendszerek</translation>
    </message>
    <message>
        <source>Runs the read-only file system check for the selected system&apos;s root, /boot and other filesystems.</source>
        <translation>A kijelölt rendszer root, / boot és más fájlrendszereinek leolvasását futtatja.</translation>
    </message>
    <message>
        <source>/etc/fstab review</source>
        <translation>/ etc / fstab felülvizsgálat</translation>
    </message>
    <message>
        <source>Displays the running host or selected repair system&apos;s fstab; repair-system inspection is mounted read-only.</source>
        <translation>Megjeleníti a futtató gazdatest vagy a kiválasztott javítórendszer fstab; javítás-rendszer ellenőrzés telepített read- csak.</translation>
    </message>
    <message>
        <source>Btrfs status</source>
        <translation>Btrfs státus</translation>
    </message>
    <message>
        <source>Shows Btrfs filesystem and subvolume information when the target uses Btrfs.</source>
        <translation>Megjeleníti a Btrfs fájlrendszert és a részmennyiséget, ha a cél Btrfs-et használ.</translation>
    </message>
    <message>
        <source>Device-mapper ancestry</source>
        <translation>Eszközök - Mapper ősei</translation>
    </message>
    <message>
        <source>Shows selected mapper ancestry and device-mapper state when available.</source>
        <translation>Jelzi a kiválasztott Mapper eredetét és a device- mapper állapotot, ha rendelkezésre áll.</translation>
    </message>
    <message>
        <source>LUKS / crypttab evidence</source>
        <translation>LUKS / crypttab bizonyíték</translation>
    </message>
    <message>
        <source>Shows LUKS/mapped ancestry plus crypttab and fstab mapper references.</source>
        <translation>Megjeleníti a LUKS / feltérképezett eredetiség plusz crypttab és fstab mapper referenciákat.</translation>
    </message>
    <message>
        <source>Full diagnostic report</source>
        <translation>Teljes diagnosztikai jelentés</translation>
    </message>
    <message>
        <source>Combines all read-only diagnostics for the selected scope (same as Run All).</source>
        <translation>Kombinálja az összes read-only diagnosztika a kiválasztott alkalmazási terület (ugyanaz, mint a Run All).</translation>
    </message>
    <message>
        <source>All entries</source>
        <translation>Minden bejegyzés</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>Diagnosztika</translation>
    </message>
    <message>
        <source>Repairs</source>
        <translation>Javítások</translation>
    </message>
    <message>
        <source>Package repair</source>
        <translation>Csomagjavítás</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Fájlmásolat</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Eszközfelfedezés</translation>
    </message>
    <message>
        <source>Host</source>
        <translation>Host</translation>
    </message>
    <message>
        <source>Required for block-device inventory</source>
        <translation>A blokkolóeszköz leltárhoz szükséges</translation>
    </message>
    <message>
        <source>Filesystem identification</source>
        <translation>A fájlrendszer azonosítása</translation>
    </message>
    <message>
        <source>Used to identify filesystem metadata</source>
        <translation>A fájlrendszer metaadatainak azonosítására használt</translation>
    </message>
    <message>
        <source>Mount inspection</source>
        <translation>A hajótest vizsgálata</translation>
    </message>
    <message>
        <source>Used to understand active mounts</source>
        <translation>Az aktív kötegek megértéséhez használt</translation>
    </message>
    <message>
        <source>LUKS support</source>
        <translation>LUKS támogatás</translation>
    </message>
    <message>
        <source>Required to unlock encrypted targets</source>
        <translation>Titkosított célok feloldásához szükséges</translation>
    </message>
    <message>
        <source>Btrfs support</source>
        <translation>Btrfs támogatás</translation>
    </message>
    <message>
        <source>Required for Btrfs inspection and snapshot rollback</source>
        <translation>A Btrfs vizsgálathoz és a pillanatfelvétel visszafordításához szükséges</translation>
    </message>
    <message>
        <source>Bidirectional file copy</source>
        <translation>Kétirányú fájlmásolat</translation>
    </message>
    <message>
        <source>Host/Repair</source>
        <translation>Host / Javítás</translation>
    </message>
    <message>
        <source>Required for verified Host-to-Repair and Repair-to-Host transfer</source>
        <translation>Szükséges a hitelesített Host- to- javítási és Repair- to- Host transzfer</translation>
    </message>
    <message>
        <source>Chroot repair</source>
        <translation>Chroot javítás</translation>
    </message>
    <message>
        <source>Required for target-side repair commands</source>
        <translation>Target- side javítási parancsokhoz szükséges</translation>
    </message>
    <message>
        <source>Offline systemd repair</source>
        <translation>Offline rendszerjavítás</translation>
    </message>
    <message>
        <source>Used to restore graphical.target and the configured display manager without starting the target GUI</source>
        <translation>A grafika helyreállítására használták. a cél és a beállított megjelenítéskezelő a cél GUI indítása nélkül</translation>
    </message>
    <message>
        <source>UEFI NVRAM inspection</source>
        <translation>UEFI NVRAM vizsgálat</translation>
    </message>
    <message>
        <source>Used to preserve target EFI BootOrder during TUXEDO UKI rebuilds when efivars are available</source>
        <translation>A cél megőrzésére használt EFI BootOrder a TUXEDO UKI alatt, amikor efivarok állnak rendelkezésre</translation>
    </message>
    <message>
        <source>UKI verification</source>
        <translation>UKI ellenőrzés</translation>
    </message>
    <message>
        <source>Used to verify the kernel embedded in a rebuilt unified kernel image</source>
        <translation>Az újraépített egységes kernel képbe ágyazott kernel ellenőrzésére</translation>
    </message>
    <message>
        <source>GRUB EFI repair</source>
        <translation>GRUB EFI javítás</translation>
    </message>
    <message>
        <source>Target/Host</source>
        <translation>Cél / fogadó</translation>
    </message>
    <message>
        <source>Required only for conventional GRUB-based EFI systems</source>
        <translation>Kizárólag hagyományos GRUB- alapú EFI rendszerekhez szükséges</translation>
    </message>
    <message>
        <source>Target</source>
        <translation>Cél</translation>
    </message>
    <message>
        <source>Debian-family GRUB helper</source>
        <translation>Debian- család GRUB segítő</translation>
    </message>
    <message>
        <source>Portable GRUB configuration generator used by Arch and other non-Debian systems</source>
        <translation>Az Arch és más nem Debian rendszerek által használt hordozható GRUB konfigurációs generátor</translation>
    </message>
    <message>
        <source>Initramfs rebuild</source>
        <translation>Initramfs újjáépítése</translation>
    </message>
    <message>
        <source>Debian-family initramfs helper</source>
        <translation>Debian- család initramfs segítő</translation>
    </message>
    <message>
        <source>Arch-family initramfs generator</source>
        <translation>Arch- family initramfs generátor</translation>
    </message>
    <message>
        <source>Alternative initramfs generator used by Arch and other distributions</source>
        <translation>Arch és más elosztók által használt alternatív initramfs generátor</translation>
    </message>
    <message>
        <source>Initramfs verification</source>
        <translation>Initramfs hitelesítése</translation>
    </message>
    <message>
        <source>Read-only verification for mkinitcpio images</source>
        <translation>Csak a mkinitcpio képek visszaolvasása</translation>
    </message>
    <message>
        <source>Read-only verification for dracut images</source>
        <translation>Csak a dracut képek visszaolvasása</translation>
    </message>
    <message>
        <source>systemd-boot inspection</source>
        <translation>systemd-boot vizsgálat</translation>
    </message>
    <message>
        <source>Host/Target</source>
        <translation>Host / Cél</translation>
    </message>
    <message>
        <source>Read-only inspection of systemd-boot and generic UKI layouts</source>
        <translation>A systemd-boot és a generikus UKI elrendezések újbóli ellenőrzése</translation>
    </message>
    <message>
        <source>Arch package manager</source>
        <translation>Arch csomagkezelő</translation>
    </message>
    <message>
        <source>Arch-family package database and transaction tool</source>
        <translation>Arch- family csomag adatbázis és tranzakciós eszköz</translation>
    </message>
    <message>
        <source>DKMS rebuild</source>
        <translation>DKMS újjáépítése</translation>
    </message>
    <message>
        <source>Required only when target uses DKMS modules</source>
        <translation>Csak akkor kötelező, ha a cél DKMS modulokat használ</translation>
    </message>
    <message>
        <source>LVM inspection</source>
        <translation>LVM-vizsgálat</translation>
    </message>
    <message>
        <source>Optional LVM storage-stack support</source>
        <translation>Opcionális LVM storage-stack támogatás</translation>
    </message>
    <message>
        <source>Software RAID</source>
        <translation>Szoftver RAID</translation>
    </message>
    <message>
        <source>Optional Linux MD RAID support</source>
        <translation>Opcionális Linux MD RAID támogatás</translation>
    </message>
    <message>
        <source>Process namespace isolation</source>
        <translation>A folyamat névtérbeli elkülönítése</translation>
    </message>
    <message>
        <source>Host+Target</source>
        <translation>Host + Cél</translation>
    </message>
    <message>
        <source>The ported helper falls back to a guarded plain chroot when unshare is absent</source>
        <translation>A kiküldött segítő visszaesik egy őrzött sima csootba, ha nincs közös kapcsolat.</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Törlés</translation>
    </message>
    <message>
        <source>OK</source>
        <translation>Oké.</translation>
    </message>
    <message>
        <source>Yes</source>
        <translation>Igen.</translation>
    </message>
    <message>
        <source>No</source>
        <translation>Nem.</translation>
    </message>
    <message>
        <source>read-only helper diagnostic</source>
        <translation>read- only helper diagnoster</translation>
    </message>
</context>
<context>
    <name>legacy::LegacyMainWindow</name>
    <message>
        <source>Boot Bitch Legacy (Etch / KDE 3.5 era)</source>
        <translation>Boot Bitch Legacy (Etch / KDE 3.5 era)</translation>
    </message>
    <message>
        <source>Linux recovery and boot-repair utility</source>
        <translation>Linux helyreállítási és boot- javítási segédprogram</translation>
    </message>
    <message>
        <source>GUARDED REPAIR</source>
        <translation>AZ IRÁNYÍTOTT HATÁSKÖR</translation>
    </message>
    <message>
        <source>Ordinary repairs require an explicitly selected non-host target. The protected running host has a separate deliberate maintenance mode with the same guarded repair stages and requires privilege authorization.</source>
        <translation>A rendes javítások kifejezetten kiválasztott, nem gazdatestet igényelnek. A védett futtató gép külön szándékos karbantartási móddal rendelkezik, ugyanolyan őrzött javítási fázisokkal, és kiváltsági engedélyt igényel.</translation>
    </message>
    <message>
        <source>Systems</source>
        <translation>Rendszerek</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>Diagnosztika</translation>
    </message>
    <message>
        <source>Repair</source>
        <translation>Javítás</translation>
    </message>
    <message>
        <source>Chroot Shell</source>
        <translation>Chroot Shell</translation>
    </message>
    <message>
        <source>File Copy</source>
        <translation>Fájl másolása</translation>
    </message>
    <message>
        <source>Logs</source>
        <translation>Bejelentkezés</translation>
    </message>
    <message>
        <source>Settings</source>
        <translation>Beállítások</translation>
    </message>
    <message>
        <source>(not created yet)</source>
        <translation>(még nem jött létre)</translation>
    </message>
    <message>
        <source>Information</source>
        <translation>Tájékoztatás</translation>
    </message>
    <message>
        <source>Close</source>
        <translation>Bezárás</translation>
    </message>
    <message>
        <source>&amp;File</source>
        <translation>&amp;fájl</translation>
    </message>
    <message>
        <source>&amp;Refresh Devices</source>
        <translation>&amp;Frissítő eszközök</translation>
    </message>
    <message>
        <source>&amp;Lock Administrator Session</source>
        <translation>&amp;Lock Administrator Session</translation>
    </message>
    <message>
        <source>&amp;Quit</source>
        <translation>&amp;Kilépés</translation>
    </message>
    <message>
        <source>&amp;View</source>
        <translation>&amp;nézet</translation>
    </message>
    <message>
        <source>&amp;Systems</source>
        <translation>&amp;rendszerek</translation>
    </message>
    <message>
        <source>&amp;Diagnostics</source>
        <translation>&amp;diagnosztika</translation>
    </message>
    <message>
        <source>&amp;Logs</source>
        <translation>&amp;naplók</translation>
    </message>
    <message>
        <source>&amp;Settings</source>
        <translation>&amp;beállítások</translation>
    </message>
    <message>
        <source>&amp;Auto-size Device Columns</source>
        <translation>&amp;automatikus méretű eszközoszlopok</translation>
    </message>
    <message>
        <source>&amp;Wrap Log Lines</source>
        <translation>&amp;Wrap napló vonalak</translation>
    </message>
    <message>
        <source>&amp;Help</source>
        <translation>&amp;Segítség</translation>
    </message>
    <message>
        <source>&amp;Using Boot Bitch</source>
        <translation>&amp;Boot Bitch-szel</translation>
    </message>
    <message>
        <source>&amp;About Boot Bitch</source>
        <translation>&amp;A Boot Bitch-ről</translation>
    </message>
    <message>
        <source>Device columns auto-sized. Drag headers to fine-tune widths.</source>
        <translation>Eszközoszlopok automatikus méretűek. Húzza a fejléceket a finomhangolásra.</translation>
    </message>
    <message>
        <source>A helper command is running; wait for it to finish before locking the session.</source>
        <translation>A segítő parancs fut; várja meg, amíg befejeződik, mielőtt lezárja az ülést.</translation>
    </message>
    <message>
        <source>Administrator session locked; the next privileged action will request authorization.</source>
        <translation>Felelős ülés lezárva; a következő privilegizált intézkedés engedélyt kér.</translation>
    </message>
    <message>
        <source>Using Boot Bitch</source>
        <translation>Boot Bitch használata</translation>
    </message>
    <message>
        <source>Boot Bitch must run from a different booted Linux environment than the system being repaired. Use a Linux live medium or another Linux installation on a different physical drive.&lt;br&gt;&lt;br&gt;The running host is protected from ordinary repair-target selection, but it can be explicitly selected through &lt;b&gt;Host Maintenance&lt;/b&gt; for guarded native diagnostics and supported maintenance stages.&lt;br&gt;&lt;br&gt;Diagnostics follow the Systems page: the committed repair drive while Host Maintenance is off, or the protected running host while it is active.&lt;br&gt;&lt;br&gt;The first privileged action requests administrator authorization once for this Boot Bitch window; &lt;b&gt;File - Lock Administrator Session&lt;/b&gt; ends that helper session immediately. Every repair keeps the helper&apos;s own runtime preflights.</source>
        <translation>A Boot Bitch-nek máshonnan kell futnia, mint a javított rendszernek. Linux élő médium vagy más Linux telepítés használata egy másik fizikai meghajtón. &lt; br &gt; &lt; br &gt; A futó gazdatest védett a közönséges javítástól, de kifejezetten kiválasztható &lt; b &gt; Host Maintenance &lt; / b &gt; segítségével őrzött natív diagnosztikákhoz és támogatott karbantartási fázisokhoz. &lt; br &gt; &lt; br &gt; Diagnosztika követi a Systems oldalt: az elkötelezett javítási meghajtó, amíg a host karbantartás kikapcsolt, vagy a védett futó host, amíg aktív. &lt; br &gt; &lt; br &gt; Az első privilegizált akció egyszer kér rendszergazdai engedélyt erre a Boot Bitch ablakra; &lt; b &gt; Fájl - Lock Administrator Session &lt; / b &gt; azonnal befejeződik. Minden javításnál ott van a segítő saját kifutó lámpája.</translation>
    </message>
    <message>
        <source>Select a physical drive; Boot Bitch resolves the most likely Linux system volume automatically. The running host stays protected from ordinary target repairs, with a separate explicit host-maintenance path for its own system. The Details button shows the protected host&apos;s facts in the details pane; selecting any drive row restores the per-drive pane.</source>
        <translation>Válassza ki a fizikai meghajtót; a Boot Bitch automatikusan feloldja a legvalószínűbb Linux rendszermennyiséget. A futó host védett marad a közönséges céljavításoktól, külön, explicit host- karbantartási útvonalat biztosítva saját rendszeréhez. A Részletek gomb a védett gazdatest tényeit mutatja a részletekben; a meghajtó sor kiválasztása visszaállítja a perdrive táblát.</translation>
    </message>
    <message>
        <source>Refresh Devices</source>
        <translation>Frissítő eszközök</translation>
    </message>
    <message>
        <source>Re-read the read-only kernel inventory (/proc/partitions, /proc/mounts, /proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* and the udev metadata database). No block device is opened and nothing is written.</source>
        <translation>Re- read-only kernel leltár (/ proc / partíciók, / proc / mounts, / proc / swap, / sys / block, / dev / mapper, / dev / disk / by- * és az udev metaadatok adatbázisa). Nincs kinyitott blokk eszköz, és semmi sincs megírva.</translation>
    </message>
    <message>
        <source>[OK]</source>
        <translation>[OK]</translation>
    </message>
    <message>
        <source>The running system was detected and stays protected from ordinary target repairs.</source>
        <translation>A futtató rendszert felismerték, és megvédték őket a normál céljavításoktól.</translation>
    </message>
    <message>
        <source>Detecting running system...</source>
        <translation>A futórendszer észlelése...</translation>
    </message>
    <message>
        <source>Detecting protected storage...</source>
        <translation>Védett tárolás észlelése...</translation>
    </message>
    <message>
        <source>PROTECTED</source>
        <translation>VÉDELEM</translation>
    </message>
    <message>
        <source>The running host remains protected from ordinary repair-target operations.</source>
        <translation>A futó gazdatest továbbra is védve van a hagyományos javításoktól.</translation>
    </message>
    <message>
        <source>Details</source>
        <translation>Részletek</translation>
    </message>
    <message>
        <source>Show read-only details for the protected running host.</source>
        <translation>A védett futtató adatainak megjelenítése.</translation>
    </message>
    <message>
        <source>Host Maintenance</source>
        <translation>Háztartási karbantartás</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Alapértelmezés</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host (menu.lst default directive with a backup and rollback). Requires Host Maintenance and the cached host-default probe.</source>
        <translation>Legyen a kanonikai telepített kernel bejegyzés az alapértelmezett GRUB- örökölt boot bejegyzés a futó host (menu.lst alapértelmezett irányelv egy biztonsági mentés és tekercs). Szükség van Host Maintenance és a cache host- alapértelmezett szonda.</translation>
    </message>
    <message>
        <source>Available repair targets</source>
        <translation>A rendelkezésre álló javítási célok</translation>
    </message>
    <message>
        <source>Most likely first</source>
        <translation>Valószínűleg először</translation>
    </message>
    <message>
        <source>Drives are listed by the read-only inventory. Select a row to inspect it; Select Target commits the selected non-host drive with its auto-resolved Linux root component.</source>
        <translation>A meghajtókat csak az olvasott leltár sorolja fel. Válasszon ki egy sort, hogy ellenőrizze azt; A Select Target a kiválasztott non-host meghajtót az automatikus megoldású Linux root komponensével.</translation>
    </message>
    <message>
        <source>Device</source>
        <translation>Eszköz</translation>
    </message>
    <message>
        <source>Size</source>
        <translation>Méret</translation>
    </message>
    <message>
        <source>Type</source>
        <translation>Típus</translation>
    </message>
    <message>
        <source>Filesystem</source>
        <translation>Fájlrendszer</translation>
    </message>
    <message>
        <source>Select Target</source>
        <translation>A cél kiválasztása</translation>
    </message>
    <message>
        <source>Unlock</source>
        <translation>Nyisd ki!</translation>
    </message>
    <message>
        <source>Authorize</source>
        <translation>Engedélyezés</translation>
    </message>
    <message>
        <source>Establish the privileged helper session for the current scope now instead of waiting for the next privileged action.</source>
        <translation>Létre kell hozni a jelenlegi hatókör kiváltságos segítői ülését, ahelyett, hogy a következő kiváltságos cselekvésre várnánk.</translation>
    </message>
    <message>
        <source>Committed target: none</source>
        <translation>Elkötelezett cél: nincs</translation>
    </message>
    <message>
        <source>Unlock state for the selected drive; the LUKS passphrase is never logged.</source>
        <translation>A kijelölt meghajtó bontási állapota; a LUKS jelszó soha nem van bejelentkezve.</translation>
    </message>
    <message>
        <source>Unlock status</source>
        <translation>Kioldási állapot</translation>
    </message>
    <message>
        <source>Read-only inventory plus helper-confirmed facts; mirrors the modern Qt6 Selected drive details panel.</source>
        <translation>Újra-csak leltár plusz segítő-megerősített tények; tükrözi a modern Qt6 választott meghajtó részletek panel.</translation>
    </message>
    <message>
        <source>Selected drive details</source>
        <translation>Kiválasztott vezetési adatok</translation>
    </message>
    <message>
        <source>Field</source>
        <translation>mező</translation>
    </message>
    <message>
        <source>Value</source>
        <translation>Érték</translation>
    </message>
    <message>
        <source>Run All runs every available read-only diagnostic for the current scope; selecting a check runs it alone. Diagnostics are read-only and are the only evidence source for the gated repair actions.</source>
        <translation>Run Minden fut minden elérhető read-only diagnosztika a jelenlegi alkalmazási terület; kiválasztása egy ellenőrzés fut egyedül. Diagnosztika csak olvasható, és az egyetlen bizonyíték forrása a zárt javítási intézkedések.</translation>
    </message>
    <message>
        <source>Target: none selected</source>
        <translation>Cél: nincs kijelölve</translation>
    </message>
    <message>
        <source>Diagnostics follow the committed repair target, or the protected running host while Host Maintenance is active.</source>
        <translation>Diagnosztika kövesse az elkötelezett javítási cél, vagy a védett futó host, amíg a host karbantartás aktív.</translation>
    </message>
    <message>
        <source>Run All</source>
        <translation>Futtatás</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope.</source>
        <translation>Run All - futtassa le az összes elérhető read-only diagnosztika a jelenlegi alkalmazási kör.</translation>
    </message>
    <message>
        <source>Target configuration:</source>
        <translation>Célkonfiguráció:</translation>
    </message>
    <message>
        <source>Etch-era target configuration files; availability is probed read-only by the helper&apos;s diagnostics.</source>
        <translation>Etch- era célkonfigurációs fájlok; a rendelkezésre állás csak a segítő diagnosztikája alapján tesztelhető.</translation>
    </message>
    <message>
        <source>Edit Target File...</source>
        <translation>Célfájl szerkesztése...</translation>
    </message>
    <message>
        <source>Runs one read-only diagnostic for the selected scope through the helper (`diagnose &lt;key&gt;` / `host-diagnose &lt;key&gt;`); Run All is the combined report.</source>
        <translation>Futtatja a kiválasztott hatókör egy read- only diagnosztikáját (&quot;diagnózis &lt; kulcs &gt;&quot; / &quot;host- diagnózis &lt; kulcs &gt;&quot;); Run All a kombinált jelentés.</translation>
    </message>
    <message>
        <source>Diagnostic checks</source>
        <translation>Diagnosztikai ellenőrzések</translation>
    </message>
    <message>
        <source>Check</source>
        <translation>Ellenőrzés</translation>
    </message>
    <message>
        <source>Selected diagnostic</source>
        <translation>Válogatott diagnosztika</translation>
    </message>
    <message>
        <source>Select a diagnostic</source>
        <translation>Válasszon ki egy diagnosztikát</translation>
    </message>
    <message>
        <source>Choose a diagnostic from the list.</source>
        <translation>Válasszon egy diagnosztikát a listáról.</translation>
    </message>
    <message>
        <source>Ready</source>
        <translation>Kész</translation>
    </message>
    <message>
        <source>Results</source>
        <translation>Eredmények</translation>
    </message>
    <message>
        <source>Run Diagnostic</source>
        <translation>Diagnosztikai futtatás</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic for the current scope.</source>
        <translation>Futtatás Diagnosztikai - futtassa a kiválasztott csak olvasható diagnosztika az aktuális alkalmazási kör.</translation>
    </message>
    <message>
        <source>Copy Results</source>
        <translation>Eredmények másolása</translation>
    </message>
    <message>
        <source>Save Results...</source>
        <translation>Eredmények mentése...</translation>
    </message>
    <message>
        <source>The Full Repair plan runs the selected legacy stages in order through the guarded helper; the individual tools run one stage at a time. Every action stays disabled until the cached capability lines say available and the helper keeps its runtime preflights.</source>
        <translation>A teljes javítási terv futtatja a kiválasztott örökölt szakaszok sorrendben az őrzött segítő; az egyes eszközök fut egy szakaszban egy időben. Minden művelet mindaddig üzemképtelen marad, amíg a tárhelyező azt nem mondja, hogy rendelkezésre áll, és a segítő megőrzi a futási idejét.</translation>
    </message>
    <message>
        <source>Full Repair plan</source>
        <translation>Teljes javítási terv</translation>
    </message>
    <message>
        <source>No stages selected</source>
        <translation>Nincs kijelölve szakasz</translation>
    </message>
    <message>
        <source>Configure Plan...</source>
        <translation>A terv beállítása...</translation>
    </message>
    <message>
        <source>Open Settings to choose which Full Repair stages are part of the plan.</source>
        <translation>Megnyitás a Beállítások kiválasztásához, mely teljes javítási szakaszok képezik a terv részét.</translation>
    </message>
    <message>
        <source>Run Full Repair</source>
        <translation>Teljes javítás futtatása</translation>
    </message>
    <message>
        <source>Select a repair drive, or choose Host Maintenance on the protected running-host card.</source>
        <translation>Válassza ki a javítási meghajtót, vagy válassza Host Maintenance a védett Running- host kártyát.</translation>
    </message>
    <message>
        <source>Stage</source>
        <translation>Fokozat</translation>
    </message>
    <message>
        <source>Individual repair tools</source>
        <translation>Egyedi szerszámok</translation>
    </message>
    <message>
        <source>Tool</source>
        <translation>Eszköz</translation>
    </message>
    <message>
        <source>Full Repair</source>
        <translation>Teljes javítás</translation>
    </message>
    <message>
        <source>not reported</source>
        <translation>nem jelentett</translation>
    </message>
    <message>
        <source>Selected tool</source>
        <translation>Kiválasztott eszköz</translation>
    </message>
    <message>
        <source>Select a repair tool</source>
        <translation>Válasszon ki egy javítóeszközt</translation>
    </message>
    <message>
        <source>Run Tool</source>
        <translation>Eszköz</translation>
    </message>
    <message>
        <source>Select a tool to review its repair action.</source>
        <translation>Válasszon ki egy eszközt a javítási művelet felülvizsgálatára.</translation>
    </message>
    <message>
        <source>Write actions ask for confirmation and then run the helper&apos;s own runtime preflights; the GUI never weakens them. A repair that is not proven &apos;unchanged&apos; invalidates the cached diagnostics and disables the gated actions until diagnostics run again.</source>
        <translation>Írj cselekvéseket kér megerősítést, majd futtassa a segítő saját futási ideje preferenciák; a GUI soha nem gyengíti őket. A javítást, amely nem bizonyított &quot;változatlan&quot; érvényteleníti a tárolt diagnosztikai és letiltja a shed akciók, amíg a diagnosztika újra fut.</translation>
    </message>
    <message>
        <source>Chroot shell</source>
        <translation>Póréhagyma</translation>
    </message>
    <message>
        <source>Offline commands run one at a time in a fresh chroot and cannot answer interactive prompts (apt-get -y upgrade works). Host-shell commands run directly on the running host. The helper&apos;s probe lines gate the command field; the exact reason appears in the tooltip.</source>
        <translation>Offline parancsok futnak egy időben egy friss chroot, és nem válaszol interaktív parancsok (apt- get -y upgrade működik). Host- shell parancsok futnak közvetlenül a futó host. A segítő szondája a parancsmezőn keresztül halad; a pontos ok megjelenik a szerszámtippben.</translation>
    </message>
    <message>
        <source>No &apos;Legacy feature shell:&apos; line is cached; run diagnostics for the selected scope to evaluate the helper&apos;s chroot/timeout containment probes (fail closed).</source>
        <translation>Nincs &apos;Legacy funkció shell:&apos; vonal tárolja; futtassa diagnosztika a kiválasztott alkalmazási terület, hogy értékelje a helper chroot / timeout elszigetelési szondák (hiba zárva).</translation>
    </message>
    <message>
        <source>Command</source>
        <translation>Parancs</translation>
    </message>
    <message>
        <source>Command:</source>
        <translation>Parancs:</translation>
    </message>
    <message>
        <source>One reviewed command string, passed to the helper as a single argument (no shell interpolation by the GUI).</source>
        <translation>Egy felülvizsgált parancs sztring, át a segítő egyetlen argumentum (nincs shell interpolation a GUI).</translation>
    </message>
    <message>
        <source>Run Command</source>
        <translation>A parancs futtatása</translation>
    </message>
    <message>
        <source>Clear Output</source>
        <translation>Kimenet törlése</translation>
    </message>
    <message>
        <source>The helper exposes `shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for an offline target chroot and `host-shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for the running host. Both keep the helper&apos;s runtime preflights; this tab enables the command only when the scope is committed, the session is authorized and the scope&apos;s Legacy feature probe reports available.</source>
        <translation>A helper a &apos;shell &lt; disk &gt; &lt; root &gt; &lt; parancs &gt;&apos; -t egy offline cél chroot-ra és a &apos;host- shell &lt; disk &gt; &lt; root &gt; &lt; parancs &gt;&apos; -t teszi ki a futtató gazdatestre. Mindkettő tartsa meg a segítő futási ideje preflights; ez a lap lehetővé teszi a parancs csak akkor, ha a hatály van lekötve, az ülés engedélyezett, és a hatály Legacy funkció szonda jelentések állnak rendelkezésre.</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Fájlmásolat</translation>
    </message>
    <message>
        <source>Copy and verify files in either direction through the guarded helper (cp -a plus ownership restoration and a per-file byte-compare). The helper&apos;s file-copy probe gates the controls and keeps the direction and path containment checks.</source>
        <translation>A fájlok másolása és ellenőrzése bármelyik irányban az őrzött segítőn keresztül (cp -a plusz tulajdonosi helyreállítás és per- file byte- comparation). A segítő file-copy szondája kinyitja a vezérlőket, és megtartja az irány- és útvonalellenőrzést.</translation>
    </message>
    <message>
        <source>Preview Changes</source>
        <translation>Változások előnézete</translation>
    </message>
    <message>
        <source>Run a copy dry-run through the guarded helper. No files are changed.</source>
        <translation>Futass le egy másolatot az őrzött segítőn. Nincs változás.</translation>
    </message>
    <message>
        <source>Copy and Verify</source>
        <translation>Másolás és ellenőrzés</translation>
    </message>
    <message>
        <source>Copy staged items and verify the result. Existing destination names are overwritten when source content differs; unrelated destination files are never deleted.</source>
        <translation>Másolja a beállított elemeket, és ellenőrizze az eredményt. A meglévő célnevek akkor kerülnek felülírásra, ha a forrástartalom eltérő; a nem kapcsolódó rendeltetési fájlok soha nem törlésre kerülnek.</translation>
    </message>
    <message>
        <source>Direction:</source>
        <translation>Irány:</translation>
    </message>
    <message>
        <source>Choose which system supplies the source files and which system receives them.</source>
        <translation>Válassza ki, melyik rendszer szállítja a forrásfájlokat, és melyik rendszer kapja azokat.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from this host</source>
        <translation>1. Válassza ki a forrásfájlokat vagy mappákat ebből a gazdatestből</translation>
    </message>
    <message>
        <source>Source</source>
        <translation>Forrás</translation>
    </message>
    <message>
        <source>Files and folders staged for the verified copy. The legacy backend copies with cp -a and restores ownership with chown --reference; every regular file is byte-compared after the copy.</source>
        <translation>A fájlok és mappák a hitelesített másolathoz vannak rendezve. Az örökölt háttérmásolatok cp -a-val, és visszaállítja a tulajdonjogot chown - reference; minden reguláris fájl byte- compared after the copy.</translation>
    </message>
    <message>
        <source>Add Files...</source>
        <translation>Fájlok hozzáadása...</translation>
    </message>
    <message>
        <source>Add Folder...</source>
        <translation>A mappa hozzáadása...</translation>
    </message>
    <message>
        <source>Remove</source>
        <translation>Eltávolítás</translation>
    </message>
    <message>
        <source>Clear</source>
        <translation>Tiszta</translation>
    </message>
    <message>
        <source>Clear the staged source list (nothing is copied or deleted).</source>
        <translation>A beállított forráslista törlése (semmi sem másolható vagy törölhető).</translation>
    </message>
    <message>
        <source>2. Choose destination in repaired system</source>
        <translation>2. Válassza ki a célállomást a javított rendszerben</translation>
    </message>
    <message>
        <source>unavailable: see the helper&apos;s Legacy feature file-copy: probe reason above</source>
        <translation>Nem elérhető: lásd a helper &apos;s Legacy funkció fájl-másolás: szonda ok fenti</translation>
    </message>
    <message>
        <source>An absolute path inside the selected repair system (Host to Repair) or on the running host (Repair to Host).</source>
        <translation>Egy abszolút útvonal a kiválasztott javítási rendszeren (Host to Javítás) vagy a futó hoston (Javítás a hostra) belül.</translation>
    </message>
    <message>
        <source>Browse Target Folders...</source>
        <translation>A célmappák böngészése...</translation>
    </message>
    <message>
        <source>Browse the selected repair system through the helper&apos;s temporary read-only mounts and choose an absolute destination path. No target files are changed while browsing.</source>
        <translation>Böngésszen a kiválasztott javítórendszerbe a segítő ideiglenes, csak olvasóegységén keresztül, és válasszon egy abszolút célútvonalat. A célfájlok nem változnak böngészés közben.</translation>
    </message>
    <message>
        <source>3. Ownership and copy policy</source>
        <translation>3. Tulajdoni és másolási politika</translation>
    </message>
    <message>
        <source>Ownership:</source>
        <translation>Tulajdonjog:</translation>
    </message>
    <message>
        <source>Smart destination ownership (recommended)</source>
        <translation>Intelligens rendeltetési hely tulajdonjoga (ajánlott)</translation>
    </message>
    <message>
        <source>Preserve source numeric UID/GID</source>
        <translation>Tartalékforrás numerikus UID / GID</translation>
    </message>
    <message>
        <source>Smart mode validates UID/GID identity mapping across the two systems and falls back to the destination-directory owner when the same numeric ID means a different account (the legacy backend implements it with chown --reference).</source>
        <translation>Smart mode validálja az UID / GID identitás feltérképezését a két rendszerben, és visszaesik a destination- könyvtár tulajdonosára, amikor ugyanaz a numerikus azonosító egy másik fiókot jelent (a megörökölt háttér chown -- reference használatával valósítja meg).</translation>
    </message>
    <message>
        <source>Application log</source>
        <translation>Alkalmazási napló</translation>
    </message>
    <message>
        <source>The complete session register; Save As... writes every entry even while a filter hides lines. If a writable system share mount exists at /host, Save As... starts there; otherwise the log directory is the fallback. Prior session files are listed read-only.</source>
        <translation>A teljes munkamenet register; Mentés As... ír minden bejegyzést, még akkor is, ha a szűrő elrejti a sorokat. Ha létezik egy írható rendszermegosztási mount a / host oldalon, akkor a Save As... itt kezdődik; különben a log könyvtár a visszaesés. Az előzetes munkamenet fájlok csak olvasható.</translation>
    </message>
    <message>
        <source>Session logs</source>
        <translation>Ülésnaplók</translation>
    </message>
    <message>
        <source>Session</source>
        <translation>Munkamenet</translation>
    </message>
    <message>
        <source>The first entry is the live session; earlier files in the log directory are listed read-only below it.</source>
        <translation>Az első bejegyzés a live session; korábbi fájlok a naplókönyvtárban vannak felsorolva read- csak alatta.</translation>
    </message>
    <message>
        <source>New Session Log</source>
        <translation>Új munkamenet napló</translation>
    </message>
    <message>
        <source>Close the active session file; it becomes a prior session and the next log entry starts a new file.</source>
        <translation>Az aktív munkamenet fájl bezárása; ez lesz egy előzetes munkamenet, és a következő log bejegyzés kezdődik egy új fájlt.</translation>
    </message>
    <message>
        <source>Add Note</source>
        <translation>Megjegyzés hozzáadása</translation>
    </message>
    <message>
        <source>Append a NOTE entry to the live session register.</source>
        <translation>Megjegyzés csatolása az élő munkamenet-regisztrációhoz.</translation>
    </message>
    <message>
        <source>Delete</source>
        <translation>Törlés</translation>
    </message>
    <message>
        <source>Delete the selected prior session file (the live session is never deleted).</source>
        <translation>A kijelölt korábbi munkamenet fájl törlése (az élő munkamenet soha nem törlődik).</translation>
    </message>
    <message>
        <source>Refresh</source>
        <translation>Frissítés</translation>
    </message>
    <message>
        <source>Save As...</source>
        <translation>Mentés...</translation>
    </message>
    <message>
        <source>Save the complete session log (all entries, not just the current filter).</source>
        <translation>Mentse a teljes munkamenet napló (minden bejegyzés, nem csak az aktuális szűrő).</translation>
    </message>
    <message>
        <source>Clear Register</source>
        <translation>A nyilvántartás törlése</translation>
    </message>
    <message>
        <source>Clear the live register and view; prior session files are never modified.</source>
        <translation>Törölje az élő regisztert és nézetet; az előzetes munkamenet fájlok soha nem módosulnak.</translation>
    </message>
    <message>
        <source>Search log:</source>
        <translation>Keresés naplója:</translation>
    </message>
    <message>
        <source>Type any characters to show matching log entries (case-insensitive). Save As always writes every entry.</source>
        <translation>Írja be a karaktereket, hogy megjelenítse egyező log bejegyzések (esetérzéketlen). Mentés Ahogy mindig ír minden bejegyzést.</translation>
    </message>
    <message>
        <source>Filter:</source>
        <translation>Szűrő:</translation>
    </message>
    <message>
        <source>Filter the visible log by entry kind. Selecting a diagnostic section shows the lines captured for that section; a workflow filter such as File system repair or Package repair shows its mapped repair lines (File copy has no lines in this frontend). Save As always writes every entry.</source>
        <translation>Szűrjük át a látható logot belépési típusonként. A diagnosztikai szakasz kiválasztása az adott szakaszhoz rögzített sorokat mutatja; a munkafolyamat szűrője, mint például a fájlrendszer javítása vagy a csomagjavítás, a feltérképezett javítóvonalakat mutatja (a fájlmásolatnak nincs vonala ezen a fronton). Mentés Ahogy mindig ír minden bejegyzést.</translation>
    </message>
    <message>
        <source>Settings are stored per user under ~/.qt/, one file per settings group (devicesrc, logsrc, diagnosticsrc, repairrc), and are saved immediately on every change and on close. Launch the GUI as the same user to keep your overrides; a GUI started as root keeps its own copies.</source>
        <translation>A beállítások felhasználónként a ~ / .qt /, egy fájl beállítási csoportonként (devicesrc, logsrc, diagnosticsrc, repairc) kerülnek tárolásra, és azonnal elmentésre kerülnek minden változásra és zárásra. Indítsa el a GUI, mint ugyanaz a felhasználó, hogy tartsa a felülírás; egy GUI indult root megtartja a saját másolatait.</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Eszközfelfedezés</translation>
    </message>
    <message>
        <source>Show devices without an identified Linux installation</source>
        <translation>Eszközök megjelenítése azonosított Linux telepítés nélkül</translation>
    </message>
    <message>
        <source>Show removable and USB storage</source>
        <translation>Kivehető és USB tároló megjelenítése</translation>
    </message>
    <message>
        <source>Show encrypted devices before unlocking</source>
        <translation>Titkosított eszközök megjelenítése a feloldás előtt</translation>
    </message>
    <message>
        <source>When off, drives without a visible Linux filesystem are hidden unless they still contain an encrypted device and encrypted devices are shown.</source>
        <translation>Ha ki, meghajtók nélkül látható Linux fájlrendszer rejtett, kivéve, ha még mindig tartalmaz egy titkosított eszköz és titkosított eszközök jelennek meg.</translation>
    </message>
    <message>
        <source>When off, removable and USB drives are hidden from the repair-target list.</source>
        <translation>Amikor ki, eltávolítható és USB meghajtók vannak elrejtve a javítási céllista.</translation>
    </message>
    <message>
        <source>When off, drives with an encrypted device are hidden until the volume is unlocked.</source>
        <translation>Amikor ki, meghajtók egy titkosított eszköz van elrejtve, amíg a hangerő van nyitva.</translation>
    </message>
    <message>
        <source>Include the %1 stage in the Full Repair plan. The stage runs in the plan order shown on the Repair tab.</source>
        <translation>A teljes javítási tervben szerepeljen a %1 szakasz. A szakasz fut a terv sorrend látható a javítási lapon.</translation>
    </message>
    <message>
        <source>Automatically regenerate read-only diagnostics after repairs or target changes</source>
        <translation>Automatikusan regenerálódik csak a javításokat vagy a cél megváltoztatását követő diagnosztika</translation>
    </message>
    <message>
        <source>Regenerates the cached read-only diagnostics for the current scope after an operation that invalidates them (LUKS unlock, target configuration edit). It runs only inside an already authorized administrator session and never opens an authorization prompt by itself.</source>
        <translation>Regenerálja a tárolt read-only diagnosztikát az aktuális hatály után egy művelet, amely érvényteleníti őket (LUKS feloldása, cél beállítás szerkesztése). Csak egy már engedélyezett rendszergazdai munkamenetben fut, és soha nem nyit meg önmagától egy engedélyezési parancsot.</translation>
    </message>
    <message>
        <source>Wrap long log lines</source>
        <translation>Hosszú log vonalak tekercselése</translation>
    </message>
    <message>
        <source>Mandatory safety controls</source>
        <translation>Kötelező biztonsági ellenőrzések</translation>
    </message>
    <message>
        <source>Host capabilities and dependencies</source>
        <translation>A fogadó képesség és a függőség</translation>
    </message>
    <message>
        <source>Refresh Capabilities</source>
        <translation>Új képességek</translation>
    </message>
    <message>
        <source>Re-run the read-only host capability probes (a PATH search, nothing is executed) and refresh the distribution summary.</source>
        <translation>Futtassa újra az olvasási képességszondákat (egy PATH keresés, semmi nincs végrehajtva) és frissítse a disztribúciós összefoglalót.</translation>
    </message>
    <message>
        <source>Install Missing Support...</source>
        <translation>Hiányzó támogatás telepítése...</translation>
    </message>
    <message>
        <source>Automatic installation will require explicit package mapping and privilege authorization.</source>
        <translation>Automatikus telepítés explicit csomagfeltérképezést és kiváltság engedélyezést igényel.</translation>
    </message>
    <message>
        <source>Application configuration</source>
        <translation>Alkalmazási konfiguráció</translation>
    </message>
    <message>
        <source>Boot Bitch Legacy</source>
        <translation>Boot Bitch Legacy</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list first.</source>
        <translation>Válasszon egy fizikai meghajtót a rendelkezésre álló javítási célok listájában.</translation>
    </message>
    <message>
        <source>Protected system</source>
        <translation>Védett rendszer</translation>
    </message>
    <message>
        <source>The running system cannot be selected as a repair target. Use Host Maintenance for the protected running host or choose another disk.</source>
        <translation>A futtató rendszert nem lehet javítási célként kiválasztani. Használja Host Maintenance a védett futó host vagy válasszon egy másik lemezt.</translation>
    </message>
    <message>
        <source>Unlock or select a Linux system first</source>
        <translation>A Linux rendszer megnyitása vagy kiválasztása</translation>
    </message>
    <message>
        <source>This encrypted drive has no visible Linux filesystem yet. Use Unlock, refresh devices, and select the target after its Linux root is detected.</source>
        <translation>Ennek a titkosított meghajtónak még nincs látható Linux fájlrendszer. Az Unlock, frissítő eszközök használata, és válassza ki a célpontot a Linux root felismerése után.</translation>
    </message>
    <message>
        <source>The selected root component (%1) belongs to the running system and cannot be committed as a repair target. Use Host Maintenance for the protected running host.</source>
        <translation>A kiválasztott root komponens (%1) a futó rendszerhez tartozik, és javítási célként nem követhető el. Használja Host Karbantartás a védett futó host.</translation>
    </message>
    <message>
        <source>repair target commit</source>
        <translation>javítási cél</translation>
    </message>
    <message>
        <source>Repair drive selected: %1</source>
        <translation>Választott javítási meghajtó: %1</translation>
    </message>
    <message>
        <source>; best detected system component: %1</source>
        <translation>; a legjobban érzékelt rendszerelem: %1</translation>
    </message>
    <message>
        <source>. No mount or repair action was performed.</source>
        <translation>Nem hajtottak végre szerelési vagy javítási műveletet.</translation>
    </message>
    <message>
        <source>Make Default unavailable</source>
        <translation>Az alapértelmezett nem érhető el</translation>
    </message>
    <message>
        <source>Make Default is a running-host action on this frontend; enter Host Maintenance first.</source>
        <translation>A make default egy futtatás-host akció ezen a frontend; írja be a host maintenance először.</translation>
    </message>
    <message>
        <source>Administrator authorization required</source>
        <translation>Adminisztratív felhatalmazás szükséges</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize first.</source>
        <translation>Nincs adminisztrátor munkamenet aktív; nyomja meg az engedélyezést először.</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host?

The helper verifies /boot/grub/menu.lst, sets the `default &lt;N&gt;` directive to the canonical entry, backs the menu up first and restores it on any failure.</source>
        <translation>Legyen a kanonikus telepített kernel bejegyzés az alapértelmezett GRUB- örökség boot bejegyzés a futó host?

A segéd ellenőrzi a / boot / grub / menu.lst-et, az &quot;alapértelmezett &lt; N &gt;&quot; -re állítja az &quot;alapértelmezett &lt; N &gt;&quot; -et &quot;a kanonikai bejegyzéshez, először támogatja a menüt, és minden hibára visszaállítja.</translation>
    </message>
    <message>
        <source>Host Maintenance unavailable</source>
        <translation>A fogadó karbantartás nem elérhető</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a committed repair target or a detected running host.</source>
        <translation>A futó gazdatestet nem lehetett észlelni; a diagnosztikának szüksége van egy elkötelezett javítási célpontra vagy egy érzékelt futtató gazdatestre.</translation>
    </message>
    <message>
        <source>Exit Host Maintenance</source>
        <translation>Exit Host Karbantartás</translation>
    </message>
    <message>
        <source>Host Maintenance is active; diagnostics and gated repairs target the protected running host.</source>
        <translation>A Host Maintenance aktív; a diagnosztika és a zárt javítások a védett gazdatestet célozzák.</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list on the Systems tab first, or use Host Maintenance for the protected running host.</source>
        <translation>Válasszon egy fizikai meghajtót a rendelkezésre álló javítási célpontok listájában a Systems fülén először, vagy használja a Host Maintenance-t a védett futtató gazdatesthez.</translation>
    </message>
    <message>
        <source>The selected drive (%1) is the protected running host. Choose Host Maintenance on the Systems tab to run read-only host diagnostics and guarded host repairs; ordinary target repairs stay disabled.</source>
        <translation>A kiválasztott meghajtó (%1) a védett futtató. Válassza ki a Host Maintenance-t a Systems fülére, hogy lefuttassa a csak olvasható host diagnosztikát és őrzött host javításokat; a közönséges céljavítás nem működik.</translation>
    </message>
    <message>
        <source>No repair target is committed. Choose Select Target on the Systems tab (or Host Maintenance for the protected running host) first.</source>
        <translation>Nincs javítási célpont. Válassza ki a Célpontot a Systems fülre (vagy a védett futtató kiszolgálóhoz).</translation>
    </message>
    <message>
        <source>The selection changed after the target was committed. Choose Select Target again.</source>
        <translation>A kiválasztás megváltozott, miután a célpontot elkövettük. Válassza ki újra a Célpontot.</translation>
    </message>
    <message>
        <source>Diagnostics scope required</source>
        <translation>Diagnosztikai hatály szükséges</translation>
    </message>
    <message>
        <source>Diagnostics scope unresolved</source>
        <translation>Diagnosztikai hatókör megoldatlan</translation>
    </message>
    <message>
        <source>The running host target could not be resolved; use Refresh Devices and commit a repair target or re-enter Host Maintenance.</source>
        <translation>A futó host cél nem sikerült megoldani; használja a Frissítő eszközök, és kötelezze el a javítási cél vagy újra be host karbantartás.</translation>
    </message>
    <message>
        <source>Diagnostic check required</source>
        <translation>Diagnosztikai ellenőrzés szükséges</translation>
    </message>
    <message>
        <source>Select a diagnostic check in the list first.</source>
        <translation>Válasszon ki egy diagnosztikai ellenőrzést a listán először.</translation>
    </message>
    <message>
        <source>Repair target required</source>
        <translation>Javítási cél szükséges</translation>
    </message>
    <message>
        <source>Target file editing needs a committed repair target. Running-host maintenance has no target-file editing; commit an offline target first.</source>
        <translation>A célfájlszerkesztéshez egy elkötelezett javítási cél kell. Running- host karbantartás nincs Target- fájl szerkesztés; elkötelezettség offline cél először.</translation>
    </message>
    <message>
        <source>Configuration file required</source>
        <translation>Konfigurációs fájl szükséges</translation>
    </message>
    <message>
        <source>Select a target configuration file first.</source>
        <translation>Válassza ki először a célkonfigurációs fájlt.</translation>
    </message>
    <message>
        <source>Edit target %1</source>
        <translation>A %1 cél szerkesztése</translation>
    </message>
    <message>
        <source>Edit this target file through the guarded administrator helper. A successful save invalidates cached diagnostics; rerun diagnostics before repair. Generated files such as /boot/grub/menu.lst may be replaced by the next bootloader update.</source>
        <translation>A célfájl szerkesztése az őrzött rendszergazda segítőjén keresztül. A sikeres mentés érvényteleníti caced diagnostics; rerun diagnostics javítás előtt. Az olyan generált fájlok, mint a / boot / grub / menu.lst, helyettesíthetők a következő bootloader frissítéssel.</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Törlés</translation>
    </message>
    <message>
        <source>Save Target File</source>
        <translation>Célfájl mentése</translation>
    </message>
    <message>
        <source>No changes to %1.</source>
        <translation>Nincs változás a %1-en.</translation>
    </message>
    <message>
        <source>Configuration write refused</source>
        <translation>A konfigurációs írás elutasítva</translation>
    </message>
    <message>
        <source>The edited content contains NUL bytes; the guarded write refuses it.</source>
        <translation>A szerkesztett tartalom NUL bájtokat tartalmaz; az őrzött írás elutasítja.</translation>
    </message>
    <message>
        <source>File too large</source>
        <translation>Túl nagy fájl</translation>
    </message>
    <message>
        <source>The edited file is larger than 1 MiB. The guarded write refuses it; edit the file from a console instead.</source>
        <translation>A szerkesztett fájl nagyobb, mint 1 MiB. Az őrzött írás elutasítja; a fájl szerkesztése helyett konzol.</translation>
    </message>
    <message>
        <source>Write target configuration</source>
        <translation>Célkonfiguráció írása</translation>
    </message>
    <message>
        <source>Write the edited contents to %1? This modifies the repair target and invalidates cached diagnostics.</source>
        <translation>Megírod a szerkesztett tartalmat a %1-nek? Ez módosítja a javítási cél és érvényteleníti a tárolt diagnosztika.</translation>
    </message>
    <message>
        <source>No diagnostic results to copy yet.</source>
        <translation>Még nincs másolandó diagnosztikai eredmény.</translation>
    </message>
    <message>
        <source>Diagnostic results copied to the clipboard.</source>
        <translation>A vágólapra másolt diagnosztikai eredmények.</translation>
    </message>
    <message>
        <source>No diagnostic results to save yet.</source>
        <translation>Még nincs mentendő diagnosztikai eredmény.</translation>
    </message>
    <message>
        <source>Text files (*.txt);;All files (*)</source>
        <translation>Szövegfájlok (* .txt);; Minden fájl (*)</translation>
    </message>
    <message>
        <source>Save diagnostic results</source>
        <translation>A diagnosztikai eredmények mentése</translation>
    </message>
    <message>
        <source>Could not write %1.</source>
        <translation>Nem sikerült írni a %1-et.</translation>
    </message>
    <message>
        <source>Diagnostic results saved to %1</source>
        <translation>A %1-nek elmentett diagnosztikai eredmények</translation>
    </message>
    <message>
        <source>Unlock not available</source>
        <translation>Nem érhető el</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked. Select an offline repair target to unlock.</source>
        <translation>A védett gazdatestet nem lehet kinyitni. Válasszon ki egy offline javítási célt.</translation>
    </message>
    <message>
        <source>No locked LUKS component is currently visible on this selected drive.</source>
        <translation>A kijelölt meghajtón jelenleg egyetlen LUKS komponens sem látható.</translation>
    </message>
    <message>
        <source>Confirm LUKS unlock</source>
        <translation>A LUKS feloldásának megerősítése</translation>
    </message>
    <message>
        <source>Unlock %1 on %2?

The helper opens a temporary device-mapper mapping with cryptsetup and keeps it open for this recovery session. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>A %1 feloldása a %2-en?

A segítő megnyit egy ideiglenes device- mapper feltérképezést cryptsetup-al, és nyitva tartja erre a helyreállítási ülésre. A jelszó áthalad egy privát kulcsfájlon, és soha nem kerül be a parancs argumentumok vagy naplók.</translation>
    </message>
    <message>
        <source>LUKS unlock</source>
        <translation>LUKS feloldás</translation>
    </message>
    <message>
        <source>Unlock LUKS repair target</source>
        <translation>A LUKS javítási cél feloldása</translation>
    </message>
    <message>
        <source>Enter the passphrase for %1.

It is sent only to cryptsetup over the helper&apos;s standard input and is never logged or placed on a command line.</source>
        <translation>Adja meg a %1 jelszavát.

Ez csak a cryptsetting felett a helper standard bemenet, és soha nem naplózza, vagy helyezi a parancssort.</translation>
    </message>
    <message>
        <source>Passphrase required</source>
        <translation>A jeladó szükséges</translation>
    </message>
    <message>
        <source>An empty passphrase was not submitted. Enter the LUKS passphrase or choose Cancel.</source>
        <translation>Üres jelszót nem nyújtottak be. Adja meg a LUKS jelszót, vagy válassza a Törlés opciót.</translation>
    </message>
    <message>
        <source>Unlock keyfile unavailable</source>
        <translation>A kulcsfájl megnyitása nem érhető el</translation>
    </message>
    <message>
        <source>The LUKS passphrase could not be written to a private keyfile in %1; the unlock was not started.</source>
        <translation>A LUKS jelszót nem lehetett egy %1-ben található privát kulcsfájlra írni; a feloldást nem indították el.</translation>
    </message>
    <message>
        <source>Configuration write unavailable</source>
        <translation>A beállítófájl nem érhető el</translation>
    </message>
    <message>
        <source>The edited content could not be written to a private temporary file in %1; the write was not started.</source>
        <translation>A szerkesztett tartalom nem írható egy %1-ben található privát ideiglenes fájlba; az írás nem kezdődött el.</translation>
    </message>
    <message>
        <source>ERROR: cannot read the session log %1</source>
        <translation>ERROR: nem tudja elolvasni a munkamenet napló %1</translation>
    </message>
    <message>
        <source>Viewing a prior session log (read-only): %1</source>
        <translation>Előzetes munkamenet-napló megtekintése (csak olvasható): %1</translation>
    </message>
    <message>
        <source>Session log list refreshed.</source>
        <translation>A munkamenet naplóbejegyzések frissítve.</translation>
    </message>
    <message>
        <source>Note:</source>
        <translation>Megjegyzés:</translation>
    </message>
    <message>
        <source>Delete session log</source>
        <translation>A munkamenet naplójának törlése</translation>
    </message>
    <message>
        <source>Delete %1 permanently? This cannot be undone.</source>
        <translation>A %1 végleges törlése? Ezt nem lehet visszacsinálni.</translation>
    </message>
    <message>
        <source>%1 changed while the confirmation was open; the delete was refused.</source>
        <translation>A %1 megváltozott, amíg a visszaigazolás nyitva volt; a törlést elutasították.</translation>
    </message>
    <message>
        <source>Unable to delete %1.</source>
        <translation>A %1 törlése nem sikerült.</translation>
    </message>
    <message>
        <source>Device discovery filters updated.</source>
        <translation>Eszközfelismerő szűrők frissítve.</translation>
    </message>
    <message>
        <source>Unknown Linux distribution</source>
        <translation>Ismeretlen Linux disztribúció</translation>
    </message>
    <message>
        <source>sudo / gksu (no KAuth on this frontend)</source>
        <translation>sudo / gksu (nincs kauth ezen a fronton)</translation>
    </message>
    <message>
        <source>Available</source>
        <translation>Rendelkezésre álló</translation>
    </message>
    <message>
        <source>Missing on this frontend</source>
        <translation>Eltűnt ezen a fronton</translation>
    </message>
    <message>
        <source>Missing</source>
        <translation>Hiányzik</translation>
    </message>
    <message>
        <source>No repair tool is selected.</source>
        <translation>Nincs kiválasztva javítási eszköz.</translation>
    </message>
    <message>
        <source>A helper command is already running.</source>
        <translation>Egy segítő parancs már fut.</translation>
    </message>
    <message>
        <source>Restore Graphical Login is a host-scope stage on this legacy frontend; enter Host Maintenance to run it.</source>
        <translation>A Graphical Login visszaállítása egy host- scope szakasz ezen a hagyatéki frontend; írja be a Host Maintenance futtatni.</translation>
    </message>
    <message>
        <source>The selected scope has no resolved root component; use Refresh Devices and commit the repair target again.</source>
        <translation>A kiválasztott alkalmazási kör nem rendelkezik megoldott root komponens; használja a Frissítő eszközök és a javítási cél újra.</translation>
    </message>
    <message>
        <source>This legacy frontend exposes no %1 action; the helper reports the capability as available.</source>
        <translation>Ez az örökölt frontend nem mutat %1 akció; a segítő jelenti a képesség a rendelkezésre álló.</translation>
    </message>
    <message>
        <source>Run this guarded repair action using the cached read-only diagnostic evidence. A confirmation is shown first.</source>
        <translation>Futtasd le ezt az őrzött javítást a tárca csak diagnosztikai bizonyítékkal. A megerősítés először látható.</translation>
    </message>
    <message>
        <source>Enabled in Settings</source>
        <translation>Beállítások</translation>
    </message>
    <message>
        <source>Disabled in Settings - enable it to include this stage</source>
        <translation>Letiltva a Beállítások - lehetővé teszi, hogy tartalmazza ezt a szakaszt</translation>
    </message>
    <message>
        <source>Unavailable: %1</source>
        <translation>Nem elérhető: %1</translation>
    </message>
    <message>
        <source>
Unavailable: %1</source>
        <translation>
Nem elérhető: %1</translation>
    </message>
    <message>
        <source>Repair tool unavailable</source>
        <translation>Javító eszköz nem elérhető</translation>
    </message>
    <message>
        <source>Confirm repair</source>
        <translation>Javítás megerősítése</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Your saved selection is kept and its availability is re-checked when diagnostics for this scope complete.</source>
        <translation>Nincs bizonyíték erre a fázisra. A mentett kiválasztás megmarad, és a rendelkezésre álló újra ellenőrizni, ha a diagnosztika ezt a területet.</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Run diagnostics for the selected scope to populate the Full Repair plan; your selection is saved once the stage becomes available.</source>
        <translation>Nincs bizonyíték erre a fázisra. Futtasd le a diagnosztika a kiválasztott alkalmazási terület, hogy benépesítse a teljes javítási terv; a kiválasztás mentett, amint a szakasz elérhetővé válik.</translation>
    </message>
    <message>
        <source>Unknown Full Repair stage.</source>
        <translation>Ismeretlen teljes javítási szakasz.</translation>
    </message>
    <message>
        <source>No Full Repair stages are selected or available; use Configure Plan... to choose the stages.</source>
        <translation>Nincs teljes javítási szakaszok vannak kiválasztva vagy elérhető; használja a Configure Plan... a szakaszok kiválasztásához.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab to establish the session.</source>
        <translation>Administrator engedélyezve van; nyomja meg az engedélyezést a Systems vagy javítási lap létrehozása az ülés.</translation>
    </message>
    <message>
        <source>Runs the selected stages using the cached read-only diagnostic evidence after privilege confirmation.</source>
        <translation>Futtatja a kiválasztott szakaszokat a tárca csak diagnosztikai bizonyíték után kiváltság megerősítése.</translation>
    </message>
    <message>
        <source>No Full Repair stages selected - use Configure Plan... or Settings.</source>
        <translation>Nincs kiválasztva a teljes javítási szakasz - használja a beállítási tervet... vagy a Beállítások.</translation>
    </message>
    <message>
        <source>1 stage selected</source>
        <translation>1 szakasz kijelölve</translation>
    </message>
    <message>
        <source>%1 stages selected</source>
        <translation>Kiválasztott %1 szakaszok</translation>
    </message>
    <message>
        <source>No repair stages are selected. Use Configure Plan... to choose the stages Full Repair will run.</source>
        <translation>A javítási szakaszok nem kerülnek kiválasztásra. A Konfigurációs Terv segítségével kiválaszthatjuk a teljes javítás szakaszait.</translation>
    </message>
    <message>
        <source>No selected stages are available. %1</source>
        <translation>Nincs kijelölt szakasz. %1</translation>
    </message>
    <message>
        <source>Full Repair unavailable</source>
        <translation>Nem érhető el teljesen</translation>
    </message>
    <message>
        <source>Run the Full Repair plan?

The selected stages run in order through the helper&apos;s guarded repair command:

</source>
        <translation>Lefuttatni a teljes javítási tervet?

A kiválasztott szakaszok sorrendben futnak a segítő őrzött javítási parancs:

</translation>
    </message>
    <message>
        <source>  %1. %2
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>
The helper keeps every runtime preflight; a stage that fails stops the plan.</source>
        <translation>
A segítő minden futási időt a repülés előtt tart; a szakasz, amely nem sikerül, leállítja a tervet.</translation>
    </message>
    <message>
        <source>Running %1 through the privileged helper... The Logs tab keeps the complete transcript.</source>
        <translation>A %1-et a kiváltságos segítőn keresztül... A naplók tab megtartja a teljes átiratot.</translation>
    </message>
    <message>
        <source>Shell scope required</source>
        <translation>Shell hatókör szükséges</translation>
    </message>
    <message>
        <source>No administrator session is active.

Press Authorize on the Systems or Repair tab to establish the session, or enter Host Maintenance / commit a repair target on the Systems tab; the chroot shell then reuses the cached authorization.</source>
        <translation>Nincs adminisztrátor munkamenet aktív.

Nyomja meg a Systems vagy javítási lap létrehozása a munkamenet, vagy írja be a Host Maintenance / kötelezze a javítási cél a Systems tab; a chroot shell majd újrahasználja a tárolt engedély.</translation>
    </message>
    <message>
        <source>Shell command required</source>
        <translation>Shell parancs szükséges</translation>
    </message>
    <message>
        <source>Enter the command to run first.</source>
        <translation>Adja meg az első futási parancsot.</translation>
    </message>
    <message>
        <source>Confirm running-host command</source>
        <translation>Running- host parancs megerősítése</translation>
    </message>
    <message>
        <source>Run this command as root on the protected running host?

%1

The helper keeps its runtime preflights; the command is passed as one argument and is never interpreted by the GUI.</source>
        <translation>Futtassuk ezt a parancsot root-ként a védett futtató gazdatesten?

%1

A segítő megőrzi a futási idejét, a parancsot egy érvként fogadják el, és a GUI soha nem értelmezi.</translation>
    </message>
    <message>
        <source>Running %1...</source>
        <translation>%1 futtatás...</translation>
    </message>
    <message>
        <source>Authorization scope required</source>
        <translation>Engedélyezési hatály szükséges</translation>
    </message>
    <message>
        <source>administrator authorization</source>
        <translation>Igazgatói felhatalmazás</translation>
    </message>
    <message>
        <source>Administrator authorization</source>
        <translation>Adminisztratív felhatalmazás</translation>
    </message>
    <message>
        <source>Administrator authorization is required for %1.

Enter the password for %2 (sudo). It is used only for this sudo authentication, is sent over a pipe and is never logged or placed on a command line. The authorization is cached for this session and reused by diagnostics and repairs.</source>
        <translation>A %1-hez adminisztratív engedély szükséges.

Adja meg a %2 (sudo) jelszót. Csak erre a sudo hitelesítésre használják, átküldik egy csőre, és soha nem naplózzák, illetve nem helyezik parancssorra. Az engedélyt erre az ülésre rögzítik, és újrahasználják diagnózisok és javítások.</translation>
    </message>
    <message>
        <source>your account</source>
        <translation>az Ön számlája</translation>
    </message>
    <message>
        <source>Password required</source>
        <translation>Jelszó szükséges</translation>
    </message>
    <message>
        <source>An empty password was not submitted. Enter the sudo password or choose Cancel.</source>
        <translation>Üres jelszó nem érkezett. Adja meg a sudo jelszót, vagy válassza a Törlés opciót.</translation>
    </message>
    <message>
        <source>Administrator authorization failed</source>
        <translation>A tisztviselő engedélye nem sikerült</translation>
    </message>
    <message>
        <source>sudo did not accept the password: %1

The command was not started.</source>
        <translation>sudo nem fogadta el a jelszót: %1

A parancs még nem kezdődött el.</translation>
    </message>
    <message>
        <source>elevation requires an interactive sudo password; run the smoke as root or after `sudo -S -v` with --elevate &apos;sudo -n&apos;</source>
        <translation>Magasság igényel egy interaktív sudo jelszót; futtasd a füstöt root vagy után &apos;sudo -S -v&apos; -val --emelje &apos;sudo -n&apos;</translation>
    </message>
    <message>
        <source>No administrator session is active for %1.

Press Authorize on the Systems or Repair tab to establish the session now, or enter Host Maintenance / commit a repair target on the Systems tab; diagnostics and repairs then reuse the cached authorization.</source>
        <translation>A %1-hez egyetlen rendszergazdai munkamenet sem aktív.

Nyomja meg a Systems vagy javítási lap létrehozása a munkamenet most, vagy adja meg a Host Maintenance / kötelezze a javítási cél a Systems fülre; diagnosztika és javítások, majd újra a tárolt engedély.</translation>
    </message>
    <message>
        <source>Administrator authorization expired</source>
        <translation>A tisztviselő engedélye lejárt</translation>
    </message>
    <message>
        <source>The cached administrator authorization for %1 expired or was refused.

Press Authorize on the Systems or Repair tab to re-establish the session, then run the command again. No command was started.</source>
        <translation>A %1 gyorsítótáras rendszergazdai engedélye lejárt vagy elutasították.

Nyomja meg a Systems vagy Javító lap újra létrehozni a munkamenet, majd futtassa a parancsot újra. Nem indult parancs.</translation>
    </message>
    <message>
        <source>Privileged operation completed successfully. Administrator authorization remains active for this session.</source>
        <translation>A kiváltságos művelet sikeresen befejeződött. A bizottsági felhatalmazás továbbra is aktív ezen az ülésen.</translation>
    </message>
    <message>
        <source>Privileged operation stopped with an error. Administrator authorization remains active; review the output before closing.</source>
        <translation>A kiváltságos művelet egy hibával véget ért. Administrator authorisation still active; review the output before closing.</translation>
    </message>
    <message>
        <source>Inspection is read-only.

</source>
        <translation>Az ellenőrzés kész.

</translation>
    </message>
    <message>
        <source>Configuration unavailable</source>
        <translation>A beállítás nem érhető el</translation>
    </message>
    <message>
        <source>The helper could not read %1:

%2</source>
        <translation>A segítő nem tudta elolvasni a %1-et:

%2</translation>
    </message>
    <message>
        <source>Configuration write failed</source>
        <translation>Nem sikerült beállítani</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
Result: mapping opened for this recovery session.</source>
        <translation>Állapot: nyitva
komponens: %1
Mapper: %2
Módszer: helper display (cryptset; passphrase mode- 600 keyfile-on keresztül, használat után törölve)
Eredmény: feltérképezés megnyitva erre a helyreállítási ülésre.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: the passphrase was not accepted; retry offered.</source>
        <translation>Állam: zárolva
komponens: %1
Módszer: helper kinyit (cryptset)
Hiba: a jelszót nem fogadták el; visszafizetés ajánlott.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: %2</source>
        <translation>Állam: zárolva
komponens: %1
Módszer: helper kinyit (cryptset)
Hiba: %2</translation>
    </message>
    <message>
        <source>Passphrase not accepted</source>
        <translation>Passphrase nem elfogadott</translation>
    </message>
    <message>
        <source>The LUKS passphrase was not accepted.

Try again?</source>
        <translation>A LUKS jelszót nem fogadták el.

Megint?</translation>
    </message>
    <message>
        <source>[-] %1 - not run (the plan stopped before reaching this stage)</source>
        <translation>[-] %1 - nem fut (a terv megállt, mielőtt elérte ezt a szakaszt)</translation>
    </message>
    <message>
        <source>filesystem</source>
        <translation>fájlrendszer</translation>
    </message>
    <message>
        <source>[-] %1 - no file system errors found - no changes</source>
        <translation>[-] %1 - nincs fájlrendszerhiba - nincs változás</translation>
    </message>
    <message>
        <source>[FAIL] %1 - not reported</source>
        <translation>%1 - nem jelentett</translation>
    </message>
    <message>
        <source>%1 stage(s) failed; review the helper output in Logs.</source>
        <translation>A %1 fázis (ok) nem sikerült; vizsgálja felül a segédkimenetet a naplókban.</translation>
    </message>
    <message>
        <source>the plan stopped before any stage completed.</source>
        <translation>a terv abbamaradt, mielőtt bármilyen szakasz befejeződött volna.</translation>
    </message>
    <message>
        <source>no repair was needed; cached diagnostics remain valid.</source>
        <translation>Nincs szükség javításra; a tárolt diagnosztika továbbra is érvényes.</translation>
    </message>
    <message>
        <source>repair completed; cached diagnostics were invalidated and must be regenerated.</source>
        <translation>javítás befejeződött; a tárcsa diagnosztikáját érvénytelenítették, és regenerálni kell.</translation>
    </message>
    <message>
        <source>%1 results: [OK] %2 successful | [FAIL] %3 failed | [-] %4 no repair needed - %5</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Re-run Diagnostic</source>
        <translation>Újrafuttatás diagnosztika</translation>
    </message>
    <message>
        <source>Unavailable</source>
        <translation>Nem elérhető</translation>
    </message>
    <message>
        <source>Commit this physical drive as the repair target.</source>
        <translation>Ezt a fizikai meghajtót a javítási célpontnak.</translation>
    </message>
    <message>
        <source>Protection:</source>
        <translation>Védelem:</translation>
    </message>
    <message>
        <source>Host shell</source>
        <translation>Hordozóhéj</translation>
    </message>
    <message>
        <source>Run on Host</source>
        <translation>Run on Host</translation>
    </message>
    <message>
        <source>Host Shell</source>
        <translation>Host Shell</translation>
    </message>
    <message>
        <source>The read-only probe found no editable target configuration file in this target. %1</source>
        <translation>A read- only szonda nem talált szerkeszthető célkonfigurációs fájlt. %1</translation>
    </message>
    <message>
        <source>Not present in the selected target (omitted): %1</source>
        <translation>Nem szerepel a kiválasztott célban (elhagyva): %1</translation>
    </message>
    <message>
        <source>Run diagnostics to probe which target configuration files exist; the helper&apos;s read-only probe decides the list.</source>
        <translation>Futtasd le a diagnosztikát, hogy megtudd, melyik beállítási fájl létezik; a segítő csak olvasója dönti el a listát.</translation>
    </message>
    <message>
        <source>Probed read-only by the helper; a saved edit invalidates the cached diagnostics.</source>
        <translation>Probed read- csak a helper; a mentett szerkesztő érvényteleníti a tárolt diagnosztika.</translation>
    </message>
    <message>
        <source>Host maintenance: %1</source>
        <translation>Host karbantartás: %1</translation>
    </message>
    <message>
        <source>unresolved</source>
        <translation>megoldatlan</translation>
    </message>
    <message>
        <source>Target: %1 + %2</source>
        <translation>Cél: %1 + %2</translation>
    </message>
    <message>
        <source>Host command</source>
        <translation>Vezérlés</translation>
    </message>
    <message>
        <source>Execute a command on the running host as root.</source>
        <translation>Hajtson végre egy parancsot a futtató gazdatestre root-ként.</translation>
    </message>
    <message>
        <source>Execute a command inside the selected repair system as root.</source>
        <translation>A kijelölt javítórendszerben root parancs végrehajtása.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab (or re-enter Host Maintenance / re-commit the repair target) to authorize this session.</source>
        <translation>Administrator authorisation is need; press Authorize on the Systems or Repair tab (or reenter Host Maintenance / re- comment the repair target) to authorisation this session.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root on the running host through the helper&apos;s guarded host-shell verb.</source>
        <translation>Futtass egy felülvizsgált parancsot root-ként a futó gazdatestre a segítő őrzött host- shell igéjén keresztül.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root inside the target chroot through the helper&apos;s guarded shell verb.</source>
        <translation>Futtass egy felülvizsgált parancsot root-ként a cél chroot-ban a segítő őrzött héja ige.</translation>
    </message>
    <message>
        <source>Run a command on the running host as root (sudo is not needed). Commands are executed directly on the active system; output is kept in this window and in the application log.</source>
        <translation>Futtass parancsot a futtató gazdatestre root-ként (sudo nem szükséges). A parancsokat közvetlenül az aktív rendszeren hajtják végre; a kimenetet ebben az ablakban és az alkalmazás naplójában tartják.</translation>
    </message>
    <message>
        <source>Run a command inside the selected repair system as root (sudo is not needed). Commands are executed one at a time in a fresh chroot and cannot answer interactive prompts; use non-interactive flags such as apt-get -y upgrade. Output is kept in this window and in the application log.</source>
        <translation>Futtass egy parancsot a kiválasztott javítórendszeren belül root-ként (sudo nem szükséges). A parancsokat egyszerre, egy friss chroot-ban hajtják végre, és nem válaszolnak interaktív parancsokra; nem interaktív zászlók, mint például az apt-get -y frissítés használata. A kimenetet ebben az ablakban és az alkalmazás naplójában tartjuk.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize or re-enter Host Maintenance / commit a repair target.</source>
        <translation>Nincs adminisztrátor munkamenet aktív; nyomja meg a Host Maintenance engedélyezését vagy újranépesítését / a javítási cél lekötését.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from the repaired system</source>
        <translation>1. Válassza ki a javított rendszerből a forrásfájlokat vagy mappákat</translation>
    </message>
    <message>
        <source>2. Choose destination on this host</source>
        <translation>2. Válassza ki a célállomást</translation>
    </message>
    <message>
        <source>Add File Path...</source>
        <translation>Fájl...</translation>
    </message>
    <message>
        <source>Add Folder Path...</source>
        <translation>A mappa menete...</translation>
    </message>
    <message>
        <source>Browse...</source>
        <translation>Böngészés...</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it (Host Maintenance does not provide a repair tree to browse).</source>
        <translation>Válassza ki a javítási meghajtót Systems kiválasztása előtt egy cél benne (Host Maintenance nem biztosít a javítási fa böngészni).</translation>
    </message>
    <message>
        <source>Choose a host destination folder directly.</source>
        <translation>Válassza ki közvetlenül a célmappát.</translation>
    </message>
    <message>
        <source>Copy the staged files and folders with cp -a, restore ownership with chown --reference and byte-compare every regular file afterwards.</source>
        <translation>Másold le a beállított fájlokat és mappákat cp-a-val, állítsd vissza a tulajdonjogot Chown-nal - referencia és byte- hasonlíts össze minden reguláris fájlt utána.</translation>
    </message>
    <message>
        <source>File Copy unavailable</source>
        <translation>A fájl másolása nem érhető el</translation>
    </message>
    <message>
        <source>Choose host destination folder</source>
        <translation>Válassza ki a célmappát</translation>
    </message>
    <message>
        <source>Select a repair target</source>
        <translation>Javítási cél kiválasztása</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it.</source>
        <translation>Válassza ki a javítási meghajtót Systems, mielőtt kiválasztja a cél benne.</translation>
    </message>
    <message>
        <source>File Copy - Browse Target Folders</source>
        <translation>Fájl másolat - A célmappák böngészése</translation>
    </message>
    <message>
        <source>Browse Target Folders</source>
        <translation>A célmappák böngészése</translation>
    </message>
    <message>
        <source>The helper could not list the repair-system folder:

%1</source>
        <translation>A segítő nem tudta felsorolni a javítórendszer mappáját:

%1</translation>
    </message>
    <message>
        <source>BROWSE_ENTRY</source>
        <translation>BROWSE _ ENTRY</translation>
    </message>
    <message>
        <source>(choose this folder: %1)</source>
        <translation>(Válassza ki ezt a mappát: %1)</translation>
    </message>
    <message>
        <source>(parent folder)</source>
        <translation>(szülőmappa)</translation>
    </message>
    <message>
        <source>Select repair-system destination</source>
        <translation>Válassza ki a javítórendszer rendeltetését</translation>
    </message>
    <message>
        <source>Choose a destination folder inside the repaired system (current folder: %1):</source>
        <translation>Válassza ki a javított rendszerben található célmappát (jelenlegi mappa: %1):</translation>
    </message>
    <message>
        <source>Folder</source>
        <translation>Mappa</translation>
    </message>
    <message>
        <source>Open</source>
        <translation>Megnyitás</translation>
    </message>
    <message>
        <source>(choose this folder:</source>
        <translation>(Válassza ki ezt a mappát:</translation>
    </message>
    <message>
        <source>Add files to copy</source>
        <translation>Fájlok hozzáadása a másoláshoz</translation>
    </message>
    <message>
        <source>Add folder to copy</source>
        <translation>A másolás mappája</translation>
    </message>
    <message>
        <source>Stage at least one source and name a destination path first.</source>
        <translation>Legalább egy forrás, és meg kell nevezni a célútvonalat.</translation>
    </message>
    <message>
        <source>Copy the %1 staged item(s) to %2?

The helper keeps its direction and path containment checks; a sensitive repair-system destination is refused unless the helper approves it, and every regular file is byte-compared after the copy.</source>
        <translation>Másolja a %1 beállított elem (ek) a %2?

A segítő fenntartja az irányt és az útvonal-elszigetelési ellenőrzéseket; egy érzékeny javítórendszer rendeltetését elutasítják, hacsak a segítő nem hagyja jóvá, és minden reguláris fájlt a másolat után kell összehasonlítani.</translation>
    </message>
    <message>
        <source>File Copy - Copy and Verify</source>
        <translation>Fájl másolás - Másolás és ellenőrzés</translation>
    </message>
    <message>
        <source>File Copy - Preview Changes</source>
        <translation>Fájl másolása - A módosítások előnézete</translation>
    </message>
    <message>
        <source>Protected running host - details</source>
        <translation>Védett futó host - részletek</translation>
    </message>
    <message>
        <source>not mounted (offline target)</source>
        <translation>nem szerelt (offline cél)</translation>
    </message>
    <message>
        <source>Select a drive to see its details.</source>
        <translation>Válasszon ki egy meghajtót a részletek megtekintéséhez.</translation>
    </message>
    <message>
        <source>Inspecting the selected component.</source>
        <translation>A kiválasztott komponens ellenőrzése.</translation>
    </message>
    <message>
        <source>Helper-confirmed by the last read-only diagnostics.</source>
        <translation>A segítséget az utolsó csak olvasott diagnosztika megerősítette.</translation>
    </message>
    <message>
        <source>Read-only inventory only; run diagnostics to confirm.</source>
        <translation>Csak a leltárt olvasd; futtass diagnosztikát, hogy megerősítsd.</translation>
    </message>
    <message>
        <source>PROTECTED - running system; read-only details only</source>
        <translation>PROTECTED - futó rendszer; csak olvasási részletek</translation>
    </message>
    <message>
        <source>Live / installer media - not selectable</source>
        <translation>Élő / telepítő média - nem választható</translation>
    </message>
    <message>
        <source>PROTECTED - running host scope</source>
        <translation>VÉDELEM - a gazdatest működési köre</translation>
    </message>
    <message>
        <source>Unlock required before selection</source>
        <translation>Kiválasztás előtt szükséges feloldás</translation>
    </message>
    <message>
        <source>Eligible repair candidate</source>
        <translation>Támogatható javítási jelölt</translation>
    </message>
    <message>
        <source>Drive:</source>
        <translation>Drive:</translation>
    </message>
    <message>
        <source>Detected target:</source>
        <translation>Azonosított célpont:</translation>
    </message>
    <message>
        <source>Pending inspection</source>
        <translation>A vizsgálat befejezéséig</translation>
    </message>
    <message>
        <source>Model / label:</source>
        <translation>Minta / címke:</translation>
    </message>
    <message>
        <source>Status:</source>
        <translation>Állapot:</translation>
    </message>
    <message>
        <source>Size:</source>
        <translation>Méret:</translation>
    </message>
    <message>
        <source>Connection:</source>
        <translation>Kapcsolat:</translation>
    </message>
    <message>
        <source>Filesystem:</source>
        <translation>Fájlrendszer:</translation>
    </message>
    <message>
        <source>UUID:</source>
        <translation>UUID:</translation>
    </message>
    <message>
        <source>Mounts:</source>
        <translation>Összeg:</translation>
    </message>
    <message>
        <source>Running system protection unresolved</source>
        <translation>A menetrendszer védelme megoldatlan</translation>
    </message>
    <message>
        <source>No protected physical backing disk was identified</source>
        <translation>Nem azonosítottak védett fizikai háttérlemezt</translation>
    </message>
    <message>
        <source>Current running Linux system</source>
        <translation>Aktuális Linux rendszer</translation>
    </message>
    <message>
        <source>Critical mounts: %1</source>
        <translation>Kritikus kötegek: %1</translation>
    </message>
    <message>
        <source>Read-only protected-running-system facts: the helper OS fact plus the inventory model, device path, size, transport and critical mounts. Nothing here is probed destructively.</source>
        <translation>Read-only protected-running-system facts: the helper OS fact plus the restaurant model, device path, size, transport and critical mounts. Itt semmi sincs megsemmisítve.</translation>
    </message>
    <message>
        <source>Select a drive to see unlock status.</source>
        <translation>Válasszon ki egy meghajtót a feloldási állapot megtekintéséhez.</translation>
    </message>
    <message>
        <source>State: protected
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
The protected running host cannot be unlocked or modified; unlock is available for an offline repair target only. Use Host Maintenance for the protected running host.</source>
        <translation>Állam: védett
komponens: %1
Mapper: (nincs)
Módszer: helper display (cryptset; passphrase mode- 600 keyfile-on keresztül, használat után törölve)
A védett futtató nem nyitható vagy módosítható; csak offline javításra használható. Használja Host Karbantartás a védett futó host.</translation>
    </message>
    <message>
        <source>(none detected)</source>
        <translation>(nincs észlelve)</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
A locked LUKS container is visible on this drive; press Unlock to open it for this recovery session.</source>
        <translation>Állam: zárolva
komponens: %1
Mapper: (nincs)
Módszer: helper display (cryptset; passphrase mode- 600 keyfile-on keresztül, használat után törölve)
Ezen a meghajtón egy lezárt LUKS tartály látható; Nyomja meg az Unlock gombot, hogy megnyissa ezt a helyreállítási munkamenetet.</translation>
    </message>
    <message>
        <source>(visible mapper)</source>
        <translation>(látható térkép)</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: already open before this session (visible mapper; Boot Bitch will reuse it and will not close it).</source>
        <translation>Állapot: nyitva
komponens: %1
Mapper: %2
Módszer: már az ülés előtt nyitva van (látható térkép; Boot Bitch újrahasználja, és nem zárja be).</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper-confirmed mapping from the last read-only diagnostics.</source>
        <translation>Állapot: nyitva
komponens: %1
Mapper: %2
Módszer: a segélyhívó által megerősített leképezés az utolsó csak olvasható diagnosztikáról.</translation>
    </message>
    <message>
        <source>(mapper)</source>
        <translation>(Mapper)</translation>
    </message>
    <message>
        <source>State: locked or no encrypted component detected
Component: (none detected)
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
No locked LUKS component and no unlock operation were recorded for this drive in the current session.</source>
        <translation>Állapot: titkosított komponens zárolva vagy nem észlelve
Alkatrész: (nincs észlelve)
Mapper: (nincs)
Módszer: helper display (cryptset; passphrase mode- 600 keyfile-on keresztül, használat után törölve)
Nem rögzített LUKS komponens és nyitási művelet nem került rögzítésre erre a meghajtóra az aktuális munkamenetben.</translation>
    </message>
    <message>
        <source>unavailable - %1</source>
        <translation>Nem elérhető - %1</translation>
    </message>
    <message>
        <source>Host maintenance:
%1</source>
        <translation>Host karbantartás:
%1</translation>
    </message>
    <message>
        <source>Target:
%1 + %2</source>
        <translation>Célpont:
%1 + %2</translation>
    </message>
    <message>
        <source>Authorization required: diagnostics and repairs fail closed until you press Authorize.</source>
        <translation>Engedélyezés szükséges: a diagnosztika és a javítások nem zárulnak, amíg meg nem nyomja az engedélyezést.</translation>
    </message>
    <message>
        <source>Re-establish the privileged helper session for the current scope now. The password is requested in the hidden-input modal and is never logged.</source>
        <translation>Állítsák újra a kiváltságos segítő ülést a jelenlegi területre. A jelszót a rejtett input modalban kérik, és soha nem naplózzák.</translation>
    </message>
    <message>
        <source>Running...</source>
        <translation>Futni...</translation>
    </message>
    <message>
        <source>Scope required</source>
        <translation>Szükséges hatály</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope; this unlocks the gated actions.</source>
        <translation>Run All - futtassa le az összes rendelkezésre álló read-only diagnosztika a jelenlegi hatókör; ez nyitja meg a zárt műveleteket.</translation>
    </message>
    <message>
        <source>Select a target and wait for any running command first.</source>
        <translation>Válassza ki a célt, és várjon a futó parancsra.</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic through the helper.</source>
        <translation>Futtatás Diagnosztikai - futtassa le a kiválasztott read-only diagnosztika a segítő.</translation>
    </message>
    <message>
        <source>Run All for the current scope and refresh the read-only backend profile.</source>
        <translation>Futtassa le az All-t az aktuális hatókörhöz, és frissítse fel a backend profilt.</translation>
    </message>
    <message>
        <source>Read or edit the selected target configuration file through the guarded helper; a saved edit invalidates cached diagnostics.</source>
        <translation>A kijelölt célkonfigurációs fájl olvasása vagy szerkesztése az őrzött segítőn keresztül; a mentett szerkesztés érvényteleníti a tárolt diagnosztikát.</translation>
    </message>
    <message>
        <source>Host Maintenance has no target-file editing; commit an offline repair target first.</source>
        <translation>A Host Maintenance-nek nincs cél- fájlszerkesztése; először egy offline javítási célt végezzen el.</translation>
    </message>
    <message>
        <source>Commit an offline repair target first.</source>
        <translation>Először állítson be egy offline javítási célt.</translation>
    </message>
    <message>
        <source>No target configuration file is available for this target; run diagnostics to probe the list.</source>
        <translation>A célkonfigurációs fájl nem áll rendelkezésre erre a célra; futtasd le a diagnosztikát a lista kiszűréséhez.</translation>
    </message>
    <message>
        <source>Already Unlocked</source>
        <translation>Kibontva</translation>
    </message>
    <message>
        <source>An unlocked Linux filesystem is already visible on this drive. Boot Bitch will reuse the existing mapper and will not close or reopen a mapping created by this recovery session. Mounting happens during diagnostics (read-only) and repairs (read-write); data filesystems are never auto-mounted on selection.</source>
        <translation>Egy nyitott Linux fájlrendszer már látható ezen a meghajtón. A Boot Bitch újra felhasználja a meglévő térképészt, és nem zárja be vagy nem nyitja meg újra a helyreállítási ülés által létrehozott térképeket. Felszerelés történik a diagnosztika (read- only) és javítások (read- write) során; adatfájlok soha nem auto-telepített kiválasztásra.</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked; use Host Maintenance for the protected running host.</source>
        <translation>A védett futtató nem lehet kinyitni; használja a Host Maintenance a védett futtató gazdatest.</translation>
    </message>
    <message>
        <source>Host Maintenance is the current scope, but the selected offline drive can still be unlocked.</source>
        <translation>A Host Maintenance a jelenlegi hatókör, de a kiválasztott offline meghajtó még nyitva lehet.</translation>
    </message>
    <message>
        <source>Unlock %1 using cryptsetup through the privileged helper. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>A %1 feloldása titkosítással a kiváltságos segítőn keresztül. A jelszó áthalad egy privát kulcsfájlon, és soha nem kerül be a parancs argumentumok vagy naplók.</translation>
    </message>
    <message>
        <source>The protected running system cannot be selected as a repair target; use Host Maintenance for the protected running host.</source>
        <translation>A védett futtató rendszert nem lehet javítási célként kiválasztani; a Host Maintenance használata a védett futtató gazdatesthez.</translation>
    </message>
    <message>
        <source>Live / installer media is read-only boot media and cannot be selected as a repair target.</source>
        <translation>Az élő / installáló médiát csak indító médiát lehet használni, és nem lehet javítási célként kiválasztani.</translation>
    </message>
    <message>
        <source>Unlock the encrypted volume first; Select Target becomes available after a Linux filesystem is detected.</source>
        <translation>A titkosított kötet feloldása; A Select Target a Linux fájlrendszer észlelése után válik elérhetővé.</translation>
    </message>
    <message>
        <source>Committed repair target. Repair, Diagnostics and File Copy target this physical drive until another drive is explicitly selected with Select Target.</source>
        <translation>Megjavított célpont. Javítás, diagnosztika és fájl másolás cél ez a fizikai meghajtó, amíg egy másik meghajtó kifejezetten kiválasztják a Select Target.</translation>
    </message>
    <message>
        <source>Commit %1 as the repair target; this leaves Host Maintenance and switches the scope to the selected drive.</source>
        <translation>A %1 a javítási cél, így a Host Maintenance a kiválasztott meghajtóra váltja a hatókört.</translation>
    </message>
    <message>
        <source>The running host target could not be detected.</source>
        <translation>A futó célpontot nem lehetett észlelni.</translation>
    </message>
    <message>
        <source>Leave Host Maintenance and return to repair-target mode.</source>
        <translation>Hagyja el a Host Maintenance-t, és térjen vissza a javításra.</translation>
    </message>
    <message>
        <source>Select the running host for deliberate guarded maintenance; the administrator authorization is requested here once and cached for the session.</source>
        <translation>Válassza ki a futtató host szándékos őrzött karbantartás; a rendszergazdai engedély kell ide egyszer, és cacched az ülés.</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a repair target.</source>
        <translation>A futó célpontot nem lehetett észlelni, a diagnosztikának javításra van szüksége.</translation>
    </message>
    <message>
        <source>Committed target: %1 + %2</source>
        <translation>Célpont: %1 + %2</translation>
    </message>
    <message>
        <source>Committed target: none (selection changed)</source>
        <translation>Elkötelezett cél: nincs (a kiválasztás megváltozott)</translation>
    </message>
    <message>
        <source>The selected scope has no resolved Linux root component; Refresh Devices and commit the repair target again.</source>
        <translation>A kijelölt szkópnak nincs megoldott Linux root komponense; Frissítse az eszközöket és hajtsa végre újra a javítási célt.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize on the Systems or Repair tab to re-establish it. Run All reuses the cached authorization and never prompts by itself.</source>
        <translation>Nincs rendszergazdai munkamenet aktív; nyomja meg az engedélyezést a Systems vagy Javító lap újra létrehozni. Run All újrahasználja a tárolt engedélyt, és soha nem ösztönzi magától.</translation>
    </message>
    <message>
        <source>Run diagnostics for this scope to unlock the gated actions. Diagnostics are read-only and the only evidence source.</source>
        <translation>Futtass diagnosztikát erre a hatványra, hogy kinyisd a rejtett akciókat. Diagnosztika csak olvasható és az egyetlen bizonyíték forrása.</translation>
    </message>
    <message>
        <source>A repair was not proven unchanged, so the cached diagnostics are invalidated. Run diagnostics again before another gated action.</source>
        <translation>A javítást nem bizonyították változatlan, így a tárolt diagnosztika érvénytelen. Futtasd le újra a diagnosztikát, mielőtt újabb akció indul.</translation>
    </message>
    <message>
        <source>Gated actions reflect the cached capability lines; the helper still runs every runtime preflight when a command starts.</source>
        <translation>A rögzített műveletek tükrözik a tárolt képességvonalakat; a segítő még mindig fut minden futásidő előtt, amikor a parancs elkezdődik.</translation>
    </message>
    <message>
        <source>Regenerating diagnostics automatically</source>
        <translation>Automatikus diagnosztika regenerálása</translation>
    </message>
    <message>
        <source>Running diagnostic: %1</source>
        <translation>Futtatás diagnosztika: %1</translation>
    </message>
    <message>
        <source>Running all diagnostics</source>
        <translation>Minden diagnosztika futtatása</translation>
    </message>
    <message>
        <source>Unlocking %1</source>
        <translation>A %1 feloldása</translation>
    </message>
    <message>
        <source>running host</source>
        <translation>futó host</translation>
    </message>
    <message>
        <source>offline target</source>
        <translation>offline cél</translation>
    </message>
    <message>
        <source>host maintenance active</source>
        <translation>host karbantartás aktív</translation>
    </message>
    <message>
        <source>repair target committed</source>
        <translation>Javítási cél</translation>
    </message>
    <message>
        <source>no committed scope</source>
        <translation>nincs kötelezettségvállalás hatálya</translation>
    </message>
    <message>
        <source>Current session</source>
        <translation>Jelenlegi munkamenet</translation>
    </message>
    <message>
        <source>Log files (*.log);;All files (*)</source>
        <translation>Log fájlok (* .log);; Minden fájl (*)</translation>
    </message>
    <message>
        <source>Save log as</source>
        <translation>A napló mentése</translation>
    </message>
    <message>
        <source>About Boot Bitch</source>
        <translation>A Boot Bitch-ről</translation>
    </message>
    <message>
        <source>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;&lt;p&gt;&lt;b&gt;Developer:&lt;/b&gt; CaptainMorgan12&lt;/p&gt;&lt;p&gt;This GUI is the package&apos;s entry point: a Qt 3.3.x frontend with the ported guarded helper for Debian Etch-era systems.&lt;/p&gt;&lt;p&gt;&lt;b&gt;Guarded repair mode:&lt;/b&gt; read-only diagnostics can inspect either the protected Running Host or an explicitly selected repair drive. The guarded Debian/APT package stages (interrupted configuration, broken dependencies, metadata refresh, upgrade), GRUB-legacy configuration regeneration, LUKS target unlock and guarded target-file editing run through the ported helper after confirmation; Host Maintenance enables the same supported stages natively on the active system after repeating the host identity and boot-mount checks.&lt;/p&gt;&lt;p&gt;The modern-only features - verified File Copy, Btrfs snapshot rollback, EFI/UKI and extlinux repair, boot-stack reconciliation and Make Default - are greyed with the helper&apos;s own probe reasons on this frontend; Arch/Alpine/Fedora package backends stay diagnostics-only here.&lt;/p&gt;&lt;p&gt;The first privileged action authorizes one cached administrator session per scope through a hidden-input modal (the Qt GUI remains unprivileged). It can be ended at any time from File - Lock Administrator Session.&lt;/p&gt;</source>
        <translation>&lt; h3 &gt; Boot Bitch %1 &lt; / h3 &gt; &lt; p &gt; &lt; b &gt; Fejlesztő: &lt; / b &gt; Captain Morgan12 &lt; / p &gt; &lt; p &gt; Ez a GUI a csomag belépési pontja: egy Qt 3.3.x frontend a Debian Etch- era rendszerekhez rendelt őrzött helperrel. &lt; / p &gt; &lt; p &gt; &lt; b &gt; Őrzött javítási mód: &lt; / b &gt; read- only diagnostics lehet ellenőrizni a védett Running Host vagy egy kifejezetten kiválasztott javítási meghajtót. Az őrzött Debian / APT csomag szakaszai (megszakított konfiguráció, hibás függések, metaadatok frissítése, frissítés), GRUB- örökölt konfigurációs regeneráció, LUKS cél kinyit és őrzött Target- fájl szerkesztés fut keresztül a portált helper után megerősítés; Host Maintenance lehetővé teszi ugyanazokat a támogatott szakaszokat nutly az aktív rendszeren, miután megismételte a host identity és boot- mount check. &lt; / p &gt; &lt; p &gt; A Modern - only funkciók - hitelesített File Copy, Btrfs pillanatfelvétel tekercs, EFI / UKI és extlinux javítás, boot- stack megbékélés és Make Alapértelmezés - a segítő saját szonda indokai ezen a fronton; Arch / Alpine / Fedora csomag backends marad diagnoszták - csak itt. &lt; / p &gt; &lt; p &gt; Az első privilegizált intézkedés hatókörenként egy zárt rendszergazdai munkamenetet engedélyez rejtett input modál segítségével (a Qt GUI továbbra is kiváltságos marad). A program bármikor befejezhető a File - Lock Administrator munkamenetből. &lt; / p &gt;</translation>
    </message>
</context>
</TS>
