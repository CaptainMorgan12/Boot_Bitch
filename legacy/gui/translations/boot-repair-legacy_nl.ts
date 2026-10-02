<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS language="nl">
<context>
    <name>LegacyMainWindow</name>
    <message>
        <source>Validate environment</source>
        <translation>Omgeving valideren</translation>
    </message>
    <message>
        <source>Validate</source>
        <translation>Valideren</translation>
    </message>
    <message>
        <source>Check the selected system&apos;s mounts, filesystem metadata, boot files, mapper consistency and dependency readiness before any repair action. This is an independent safety preflight rather than an optional Full Repair stage.</source>
        <translation>Controleer de mounts van het geselecteerde systeem, filesystem metadata, boot bestanden, mapper consistentie en afhankelijkheidsbereidheid voordat een reparatie actie. Dit is een onafhankelijke veiligheidsvoorvlucht in plaats van een optionele Full Repair fase.</translation>
    </message>
    <message>
        <source>Always preflight</source>
        <translation>Altijd voor de vlucht</translation>
    </message>
    <message>
        <source>File system repair</source>
        <translation>Bestandssysteem reparatie</translation>
    </message>
    <message>
        <source>Check File Systems</source>
        <translation>Bestandssystemen controleren</translation>
    </message>
    <message>
        <source>Run the read-only file system check for the selected system&apos;s root and /boot filesystems and report each device&apos;s check tool and result without changing anything. This legacy frontend exposes the read-only check only; device repair is not wired.</source>
        <translation>Voer de read-only bestandssysteem controle op de root en /boot bestandssystemen van het geselecteerde systeem uit en rapporteer de controletool en het resultaat van elk apparaat zonder iets te veranderen. Deze legacy frontend stelt de alleen-lezen controle bloot; apparaat reparatie is niet bedraad.</translation>
    </message>
    <message>
        <source>Full Repair plan: unavailable on this frontend - read-only check only</source>
        <translation>Volledige reparatieplan: niet beschikbaar op deze frontend - alleen-lezen controleren</translation>
    </message>
    <message>
        <source>Complete package configuration</source>
        <translation>Complete pakketconfiguratie</translation>
    </message>
    <message>
        <source>Complete Configuration</source>
        <translation>Volledige configuratie</translation>
    </message>
    <message>
        <source>Complete interrupted dpkg package configuration in the selected repair system. This is the same stage controlled by Settings -&gt; Full Repair plan -&gt; Complete interrupted package configuration, but it can also be run independently here.</source>
        <translation>Volledige onderbroken dpkg pakketconfiguratie in het geselecteerde reparatiesysteem. Dit is dezelfde fase gecontroleerd door Instellingen -&gt; Volledig reparatieplan -&gt; Compleet onderbroken pakketconfiguratie, maar het kan hier ook onafhankelijk worden uitgevoerd.</translation>
    </message>
    <message>
        <source>Run the guarded dpkg-configure repair? The helper keeps its package-lock and runtime preflights.</source>
        <translation>De bewaakte dpkg-configuratie reparatie uitvoeren? De helper houdt zijn pakket-lock en runtime voor vluchten.</translation>
    </message>
    <message>
        <source>Repair broken dependencies</source>
        <translation>Reparatie gebroken afhankelijkheden</translation>
    </message>
    <message>
        <source>Repair Dependencies</source>
        <translation>Repareren afhankelijkheden</translation>
    </message>
    <message>
        <source>Repair package dependencies in the selected repair system after the mandatory safety preflight. This maps directly to Settings -&gt; Repair broken package dependencies.</source>
        <translation>Reparatiepakket afhankelijkheden in het geselecteerde reparatiesysteem na de verplichte veiligheidsvoorvlucht. Deze kaarten direct naar Instellingen -&gt; Reparatie gebroken pakket afhankelijkheden.</translation>
    </message>
    <message>
        <source>Run the guarded fix-broken repair? The helper keeps its simulation-first preflight and runtime guards.</source>
        <translation>De bewaakte reparatie uitvoeren? De helper houdt zijn simulatie-eerste voorvlucht en runtime bewakers.</translation>
    </message>
    <message>
        <source>Refresh package metadata</source>
        <translation>Pakketmetadata vernieuwen</translation>
    </message>
    <message>
        <source>Refresh Metadata</source>
        <translation>Metadata verversen</translation>
    </message>
    <message>
        <source>Refresh APT metadata in the selected repair system without upgrading installed packages. This maps directly to Settings -&gt; Refresh package metadata.</source>
        <translation>Vernieuw APT-metadata in het geselecteerde reparatiesysteem zonder geïnstalleerde pakketten te upgraden. Deze kaarten direct naar Instellingen -&gt; Pakketmetadata vernieuwen.</translation>
    </message>
    <message>
        <source>Refresh package metadata for the selected scope? The helper requires a reachable, trusted APT source.</source>
        <translation>Pakketmetadata vernieuwen voor de geselecteerde scope? De helper heeft een bereikbare, vertrouwde APT bron nodig.</translation>
    </message>
    <message>
        <source>Upgrade installed packages</source>
        <translation>Geïnstalleerde pakketten upgraden</translation>
    </message>
    <message>
        <source>Simulate and Upgrade</source>
        <translation>Simuleren en upgraden</translation>
    </message>
    <message>
        <source>Simulate the APT transaction first, inspect proposed removals, then apply a safe upgrade. This maps directly to Settings -&gt; Upgrade installed packages.</source>
        <translation>Simuleer eerst de APT-transactie, controleer de voorgestelde verwijderingen en pas daarna een veilige upgrade toe. Deze kaarten direct naar Instellingen -&gt; Geïnstalleerde pakketten upgraden.</translation>
    </message>
    <message>
        <source>Run the guarded apt-upgrade transaction? The helper keeps its simulation-first and source guards.</source>
        <translation>De bewaakte apt-upgrade transactie uitvoeren? De helper houdt de simulatie-eerste en bron bewakers.</translation>
    </message>
    <message>
        <source>DKMS</source>
        <translation>DKMS</translation>
    </message>
    <message>
        <source>Rebuild DKMS</source>
        <translation>DKMS herbouwen</translation>
    </message>
    <message>
        <source>Rebuild out-of-tree kernel modules for kernels installed in the selected system. The helper refuses this action when DKMS is not installed; this legacy frontend exposes no DKMS action.</source>
        <translation>Herbouw kernelmodules voor kernels die in het geselecteerde systeem zijn geïnstalleerd. De helper weigert deze actie wanneer DKMS niet is geïnstalleerd; deze legacy frontend stelt geen DKMS actie bloot.</translation>
    </message>
    <message>
        <source>Graphical login / display manager</source>
        <translation>Grafische login / display manager</translation>
    </message>
    <message>
        <source>Restore Graphical Login</source>
        <translation>Grafische login herstellen</translation>
    </message>
    <message>
        <source>Restore the legacy SysV display manager configured for the running host: the /etc/X11/default-display-manager entry and the missing runlevel S-symlink, with a backup and rollback, never starting the GUI. This is a host-scope stage on this legacy frontend.</source>
        <translation>Herstellen van de legacy SysV display manager geconfigureerd voor de draaiende host: de /etc/X11/default-display-manager ingang en de ontbrekende runlevel S-symlink, met een back-up en rollback, nooit het starten van de GUI. Dit is een host-scope podium op deze erfenis frontend.</translation>
    </message>
    <message>
        <source>Restore the graphical login configuration for the running host? The helper backs up /etc/X11/default-display-manager and the runlevel symlink state, restores the configured entry and the missing S-symlink, rolls back on any failure, and never starts the display manager.</source>
        <translation>De grafische aanmeldconfiguratie voor de draaiende host herstellen? De helper maakt een back-up van /etc/X11/default-display-manager en de runlevel symlink-status, herstelt de geconfigureerde ingang en de ontbrekende S-symlink, rolt terug op een fout en start nooit de displaymanager.</translation>
    </message>
    <message>
        <source>Initramfs</source>
        <translation>Initramfs</translation>
    </message>
    <message>
        <source>Rebuild Initramfs</source>
        <translation>Initramfs opnieuw bouwen</translation>
    </message>
    <message>
        <source>Rebuild initramfs images for the selected repair system only after mapper and crypttab consistency checks pass. The helper backs up each image before the apply. On Etch the stage runs through the guarded plain-chroot fallback (no unshare required).</source>
        <translation>Rebuild initramfs afbeeldingen voor het geselecteerde reparatiesysteem pas na mapper en crypttab consistentie controles passeren. De helper maakt een back-up van elke afbeelding voordat de toepassing. Op Etch loopt de etappe door de bewaakte vlak-chroot terugval (geen unshare vereist).</translation>
    </message>
    <message>
        <source>Rebuild the initramfs for the selected scope? The helper keeps its mapper/crypttab and backup preflights.</source>
        <translation>De initramfs herbouwen voor de geselecteerde scope? De helper houdt zijn mapper/crypttab en back-up preflights.</translation>
    </message>
    <message>
        <source>EFI / UKI bootloader</source>
        <translation>EFI / UKI bootloader</translation>
    </message>
    <message>
        <source>Repair EFI / UKI</source>
        <translation>Reparatie EFI / UKI</translation>
    </message>
    <message>
        <source>Repair the selected system&apos;s EFI / UKI boot path. This legacy frontend exposes no EFI action; the Etch target is a BIOS/GRUB-legacy system.</source>
        <translation>Repareer het EFI / UKI bootpad van het geselecteerde systeem. Deze legacy frontend stelt geen EFI actie bloot; het Etch doel is een BIOS/GRUB-legacy systeem.</translation>
    </message>
    <message>
        <source>GRUB configuration</source>
        <translation>GRUB configuratie</translation>
    </message>
    <message>
        <source>Regenerate GRUB</source>
        <translation>Regenereren GRUB</translation>
    </message>
    <message>
        <source>Regenerate the selected repair system&apos;s GRUB menu/configuration after the mandatory safety preflight. The helper backs up menu.lst, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Regenereer het GRUB-menu/configuratie van het geselecteerde reparatiesysteem na de verplichte veiligheidsvoorvlucht. De helper maakt een back-up van menu.lst, behoudt elke bestaande boot entry en rolt terug op elke storing.</translation>
    </message>
    <message>
        <source>Regenerate the GRUB configuration? The helper backs up the target menu/configuration, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>De GRUB configuratie regenereren? De helper maakt een back-up van het doelmenu/configuratie, behoudt elke bestaande boot-ingang en rolt terug op elke fout.</translation>
    </message>
    <message>
        <source>extlinux configuration</source>
        <translation>extlinux configuratie</translation>
    </message>
    <message>
        <source>Regenerate extlinux</source>
        <translation>Regenereren extlinux</translation>
    </message>
    <message>
        <source>Regenerate the selected system&apos;s extlinux bootloader configuration. This legacy frontend exposes no extlinux action.</source>
        <translation>De configuratie van de extlinux bootloader van het geselecteerde systeem regenereren. Deze legacy frontend stelt geen extlinux actie bloot.</translation>
    </message>
    <message>
        <source>Boot stack reconciliation</source>
        <translation>Koppeling van de opstartstapel</translation>
    </message>
    <message>
        <source>Reconcile Boot Stack</source>
        <translation>Reconcile Boot Stack</translation>
    </message>
    <message>
        <source>Reconcile the selected repair system&apos;s boot stack in one guarded legacy pass: mapper/crypttab validation, initramfs rebuild and GRUB-legacy configuration regeneration, with the component backups and preflights unchanged. This is the Etch equivalent of the modern boot-stack reconciliation and stays out of the Full Repair plan.</source>
        <translation>Vergelijk de boot stack van het geselecteerde reparatiesysteem in één bewaakte legacy pas: mapper/crypttab validatie, initramfs herbouw en GRUB-legacy configuratie regeneratie, met de component back-ups en preflights ongewijzigd. Dit is het Etch equivalent van de moderne boot-stack verzoening en blijft uit het Full Repair plan.</translation>
    </message>
    <message>
        <source>Full Repair plan: Manual recovery tool</source>
        <translation>Volledig Reparatieplan: Handmatig herstel gereedschap</translation>
    </message>
    <message>
        <source>Run the guarded boot-stack reconciliation? The helper runs the mapper/crypttab validation, the initramfs rebuild and the GRUB-legacy regeneration in one pass with every component preflight and backup.</source>
        <translation>De bewaakte boot-stack verzoening? De helper draait de mapper/crypttab validatie, de initramfs herbouw en de GRUB-legacy regeneratie in één pas met elke component voorvlucht en back-up.</translation>
    </message>
    <message>
        <source>Complete interrupted package configuration</source>
        <translation>Complete onderbroken pakketconfiguratie</translation>
    </message>
    <message>
        <source>Repair broken package dependencies</source>
        <translation>Reparatie gebroken pakket afhankelijkheden</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Standaard maken</translation>
    </message>
    <message>
        <source>Repair file system errors (read-only check first)</source>
        <translation>Repareer bestandssysteem fouten (alleen-lezen eerst controleren)</translation>
    </message>
    <message>
        <source>the legacy frontend exposes the read-only file system check only; per-device repair is not wired on this frontend (fail closed)</source>
        <translation>de legacy frontend onthult de alleen-lezen bestandssysteem controle alleen; per-apparaat reparatie is niet bedraad op deze frontend (fout gesloten)</translation>
    </message>
    <message>
        <source>Upgrade installed packages (adaptive APT simulation)</source>
        <translation>Upgrade geïnstalleerde pakketten (adaptive APT simulatie)</translation>
    </message>
    <message>
        <source>Rebuild DKMS modules</source>
        <translation>DKMS modules herbouwen</translation>
    </message>
    <message>
        <source>Restore graphical login manager</source>
        <translation>grafische aanmeldbeheer herstellen</translation>
    </message>
    <message>
        <source>Rebuild initramfs after mapper/crypttab validation</source>
        <translation>initramfs opnieuw opbouwen na mapper/crypttab-validatie</translation>
    </message>
    <message>
        <source>Repair EFI / UKI boot path</source>
        <translation>Reparatie EFI / UKI opstartpad</translation>
    </message>
    <message>
        <source>Update GRUB configuration</source>
        <translation>GRUB-configuratie bijwerken</translation>
    </message>
    <message>
        <source>Update extlinux configuration</source>
        <translation>extlinux-configuratie bijwerken</translation>
    </message>
    <message>
        <source>Choose Full Repair stages in Settings. Enabled stages run in the order shown. Each configurable stage also appears below as an individual tool; the Full Repair column mirrors its current Settings state. The boot tools (EFI / UKI bootloader, GRUB or extlinux configuration, boot-stack reconciliation and Make Default) are independent: run them in any order, and a later action re-verifies what an earlier one changed and reports its own result. The active scope is shown beside Repair: selected repair drive or Running Host maintenance.</source>
        <translation>Kies volledige reparatie stadia in Instellingen. Ingeschakelde stadia draaien in de getoonde volgorde. Elke configureerbare fase verschijnt ook hieronder als een individueel hulpmiddel; de Full Repair kolom weerspiegelt de huidige status van instellingen. De boot tools (EFI / UKI bootloader, GRUB of extlinux configuratie, boot-stack reconciliation en Make Default) zijn onafhankelijk: voer ze in elke volgorde uit, en een latere actie herverifieert wat een eerdere men veranderde en rapporteert zijn eigen resultaat. De actieve scope wordt naast Repair getoond: geselecteerde reparatie drive of Running Host onderhoud.</translation>
    </message>
    <message>
        <source>Run All diagnostics for the selected target or running host before starting Full Repair. The report is read-only evidence used to choose and confirm repair stages.</source>
        <translation>Voer alle diagnoses voor de geselecteerde doel of host draaien voordat u volledige reparatie. Het rapport is alleen-lezen bewijs gebruikt om te kiezen en te bevestigen reparatie stadia.</translation>
    </message>
    <message>
        <source>Ready: required cached read-only diagnostics are available for the selected stages. Review them in Diagnostics or Logs before confirming.</source>
        <translation>Klaar: de vereiste diagnostiek voor alleen-lezen is beschikbaar voor de geselecteerde stadia. Bekijk ze in Diagnostics of Logs alvorens te bevestigen.</translation>
    </message>
    <message>
        <source>Environment validation</source>
        <translation>Milieuvalidatie</translation>
    </message>
    <message>
        <source>Summarizes the selected system, protection state, mounted identity and inspection readiness.</source>
        <translation>Samengevat het geselecteerde systeem, de beschermingstoestand, gemonteerde identiteit en inspectie gereedheid.</translation>
    </message>
    <message>
        <source>Distribution and boot backend profile</source>
        <translation>Distributie- en bootbackendprofiel</translation>
    </message>
    <message>
        <source>Identifies the distribution family, package manager, initramfs generator, bootloader and current guarded repair capability.</source>
        <translation>Identificeert de distributie familie, pakketmanager, initramfs generator, bootloader en huidige bewaakte reparatie mogelijkheden.</translation>
    </message>
    <message>
        <source>Boot diagnostics</source>
        <translation>Opstartdiagnostiek</translation>
    </message>
    <message>
        <source>Shows boot mounts and /boot contents plus storage evidence without changing the selected system.</source>
        <translation>Toont boot mounts en /boot inhoud plus opslagbewijs zonder het geselecteerde systeem te wijzigen.</translation>
    </message>
    <message>
        <source>Boot evidence and selection history</source>
        <translation>Boot bewijs en selectie geschiedenis</translation>
    </message>
    <message>
        <source>Correlates the detected boot chain, bootloader selection, kernel/initramfs and unlock evidence.</source>
        <translation>Correleert de gedetecteerde bootchain, bootloader selectie, kernel/initramfs en ontgrendelt bewijs.</translation>
    </message>
    <message>
        <source>Kernel / initramfs</source>
        <translation>Kernel / initramfs</translation>
    </message>
    <message>
        <source>Reviews kernel files and verifies matching initramfs images through a read-only inspection.</source>
        <translation>Beoordeelt kernelbestanden en controleert matching initramfs-afbeeldingen via een alleen-lezen inspectie.</translation>
    </message>
    <message>
        <source>Reviews the GRUB configuration without changing boot files.</source>
        <translation>Bekijkt de GRUB configuratie zonder opstartbestanden te wijzigen.</translation>
    </message>
    <message>
        <source>EFI / UKI boot state</source>
        <translation>EFI / UKI opstartstatus</translation>
    </message>
    <message>
        <source>Inspects EFI/UKI evidence; unavailable on this legacy BIOS frontend with the helper&apos;s probe reason.</source>
        <translation>Inspecteert EFI/UKI bewijs; niet beschikbaar op deze nalatenschap BIOS frontend met de helper sonde reden.</translation>
    </message>
    <message>
        <source>Reviews the configured display manager and recent boot evidence without starting the GUI.</source>
        <translation>Bekijkt de geconfigureerde display manager en recente boot bewijs zonder het starten van de GUI.</translation>
    </message>
    <message>
        <source>Boot errors</source>
        <translation>Opstartfouten</translation>
    </message>
    <message>
        <source>Reads recent error-priority entries from the running host or selected repair system when available.</source>
        <translation>Leest recente foutpriority ingangen van de lopende host of geselecteerde reparatie systeem indien beschikbaar.</translation>
    </message>
    <message>
        <source>Disk usage</source>
        <translation>Schijfgebruik</translation>
    </message>
    <message>
        <source>Summarizes filesystem capacity and free space for the running host or read-only repair target.</source>
        <translation>Samengevat bestandssysteem capaciteit en vrije ruimte voor de lopende host of alleen-lezen reparatie doel.</translation>
    </message>
    <message>
        <source>File systems</source>
        <translation>Bestandssystemen</translation>
    </message>
    <message>
        <source>Runs the read-only file system check for the selected system&apos;s root, /boot and other filesystems.</source>
        <translation>Controleert het alleen-lezen bestandssysteem voor de root, /boot en andere bestandssystemen van het geselecteerde systeem.</translation>
    </message>
    <message>
        <source>/etc/fstab review</source>
        <translation>/etc/fstab-evaluatie</translation>
    </message>
    <message>
        <source>Displays the running host or selected repair system&apos;s fstab; repair-system inspection is mounted read-only.</source>
        <translation>Toont de draaiende host of de fstab van het geselecteerde reparatiesysteem; reparatie-systeem inspectie is alleen-lezen gemonteerd.</translation>
    </message>
    <message>
        <source>Btrfs status</source>
        <translation>Btrfs-status</translation>
    </message>
    <message>
        <source>Shows Btrfs filesystem and subvolume information when the target uses Btrfs.</source>
        <translation>Toont Btrfs bestandssysteem en subvolume informatie wanneer het doel Btrfs gebruikt.</translation>
    </message>
    <message>
        <source>Device-mapper ancestry</source>
        <translation>Apparaat-mapper-voorouderschap</translation>
    </message>
    <message>
        <source>Shows selected mapper ancestry and device-mapper state when available.</source>
        <translation>Toont de geselecteerde map- en apparaatmapperstatus indien beschikbaar.</translation>
    </message>
    <message>
        <source>LUKS / crypttab evidence</source>
        <translation>LUKS / crypttab bewijs</translation>
    </message>
    <message>
        <source>Shows LUKS/mapped ancestry plus crypttab and fstab mapper references.</source>
        <translation>Toont LUKS/afgebeelde voorouders plus crypttab en fstab mapper referenties.</translation>
    </message>
    <message>
        <source>Full diagnostic report</source>
        <translation>Volledig diagnoserapport</translation>
    </message>
    <message>
        <source>Combines all read-only diagnostics for the selected scope (same as Run All).</source>
        <translation>Combineert alle alleen-lezen diagnostiek voor de geselecteerde scope (hetzelfde als Run All).</translation>
    </message>
    <message>
        <source>All entries</source>
        <translation>Alle items</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>Diagnostica</translation>
    </message>
    <message>
        <source>Repairs</source>
        <translation>Reparaties</translation>
    </message>
    <message>
        <source>Package repair</source>
        <translation>Pakket reparatie</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Bestand kopiëren</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Apparaatontdekking</translation>
    </message>
    <message>
        <source>Host</source>
        <translation>Host</translation>
    </message>
    <message>
        <source>Required for block-device inventory</source>
        <translation>Vereist voor blokinventaris</translation>
    </message>
    <message>
        <source>Filesystem identification</source>
        <translation>Bestandssysteemidentificatie</translation>
    </message>
    <message>
        <source>Used to identify filesystem metadata</source>
        <translation>Gebruikt om bestandssysteemmetadata te identificeren</translation>
    </message>
    <message>
        <source>Mount inspection</source>
        <translation>Montageinspectie</translation>
    </message>
    <message>
        <source>Used to understand active mounts</source>
        <translation>Gebruikt om actieve mounts te begrijpen</translation>
    </message>
    <message>
        <source>LUKS support</source>
        <translation>LUKS-ondersteuning</translation>
    </message>
    <message>
        <source>Required to unlock encrypted targets</source>
        <translation>Vereist om versleutelde doelen te ontgrendelen</translation>
    </message>
    <message>
        <source>Btrfs support</source>
        <translation>Btrfs-ondersteuning</translation>
    </message>
    <message>
        <source>Required for Btrfs inspection and snapshot rollback</source>
        <translation>Vereist voor Btrfs inspectie en snapshot rollback</translation>
    </message>
    <message>
        <source>Bidirectional file copy</source>
        <translation>Bidirectionele bestandskopie</translation>
    </message>
    <message>
        <source>Host/Repair</source>
        <translation>Host/Repair</translation>
    </message>
    <message>
        <source>Required for verified Host-to-Repair and Repair-to-Host transfer</source>
        <translation>Vereist voor geverifieerde Host-to-Repair en Repareer-to-Host overdracht</translation>
    </message>
    <message>
        <source>Chroot repair</source>
        <translation>Chroot reparatie</translation>
    </message>
    <message>
        <source>Required for target-side repair commands</source>
        <translation>Vereist voor doel-kant reparatie commando&apos;s</translation>
    </message>
    <message>
        <source>Offline systemd repair</source>
        <translation>Offline gerepareerd systeem</translation>
    </message>
    <message>
        <source>Used to restore graphical.target and the configured display manager without starting the target GUI</source>
        <translation>Gebruikt om grafisch te herstellen. doel en de geconfigureerde displaymanager zonder de doel GUI te starten</translation>
    </message>
    <message>
        <source>UEFI NVRAM inspection</source>
        <translation>UEFI NVRAM inspectie</translation>
    </message>
    <message>
        <source>Used to preserve target EFI BootOrder during TUXEDO UKI rebuilds when efivars are available</source>
        <translation>Gebruikt om doel EFI BootOrder te behouden tijdens TUXEDO UKI herbouwt wanneer efivars beschikbaar zijn</translation>
    </message>
    <message>
        <source>UKI verification</source>
        <translation>UKI-keuring</translation>
    </message>
    <message>
        <source>Used to verify the kernel embedded in a rebuilt unified kernel image</source>
        <translation>Gebruikt om de kernel te verifiëren die is ingebed in een herbouwde uniforme kernel image</translation>
    </message>
    <message>
        <source>GRUB EFI repair</source>
        <translation>GRUB EFI reparatie</translation>
    </message>
    <message>
        <source>Target/Host</source>
        <translation>Doel/gast</translation>
    </message>
    <message>
        <source>Required only for conventional GRUB-based EFI systems</source>
        <translation>Alleen vereist voor conventionele GRUB-gebaseerde EFI-systemen</translation>
    </message>
    <message>
        <source>Target</source>
        <translation>Doel</translation>
    </message>
    <message>
        <source>Debian-family GRUB helper</source>
        <translation>Debian-familie GRUB helper</translation>
    </message>
    <message>
        <source>Portable GRUB configuration generator used by Arch and other non-Debian systems</source>
        <translation>Draagbare GRUB configuratiegenerator gebruikt door Arch en andere niet-Debian systemen</translation>
    </message>
    <message>
        <source>Initramfs rebuild</source>
        <translation>Initramfs herbouwen</translation>
    </message>
    <message>
        <source>Debian-family initramfs helper</source>
        <translation>Debian-familie initramfs helper</translation>
    </message>
    <message>
        <source>Arch-family initramfs generator</source>
        <translation>Boogfamilie initramfs-generator</translation>
    </message>
    <message>
        <source>Alternative initramfs generator used by Arch and other distributions</source>
        <translation>Alternatieve initramfs generator gebruikt door Arch en andere distributies</translation>
    </message>
    <message>
        <source>Initramfs verification</source>
        <translation>Controle van Initramfs</translation>
    </message>
    <message>
        <source>Read-only verification for mkinitcpio images</source>
        <translation>Alleen-lezen verificatie voor mkinitcpio-afbeeldingen</translation>
    </message>
    <message>
        <source>Read-only verification for dracut images</source>
        <translation>Alleen-lezen verificatie voor dracut-afbeeldingen</translation>
    </message>
    <message>
        <source>systemd-boot inspection</source>
        <translation>systemd-boot-inspectie</translation>
    </message>
    <message>
        <source>Host/Target</source>
        <translation>Host/Target</translation>
    </message>
    <message>
        <source>Read-only inspection of systemd-boot and generic UKI layouts</source>
        <translation>Alleen-lezen inspectie van systemd-boot en generieke UKI-layouts</translation>
    </message>
    <message>
        <source>Arch package manager</source>
        <translation>Arch pakketbeheer</translation>
    </message>
    <message>
        <source>Arch-family package database and transaction tool</source>
        <translation>Arch-family pakket database en transactie tool</translation>
    </message>
    <message>
        <source>DKMS rebuild</source>
        <translation>DKMS herbouwen</translation>
    </message>
    <message>
        <source>Required only when target uses DKMS modules</source>
        <translation>Alleen vereist wanneer het doel DKMS modules gebruikt</translation>
    </message>
    <message>
        <source>LVM inspection</source>
        <translation>LVM-inspectie</translation>
    </message>
    <message>
        <source>Optional LVM storage-stack support</source>
        <translation>Optionele LVM opslag-stack ondersteuning</translation>
    </message>
    <message>
        <source>Software RAID</source>
        <translation>Software RAID</translation>
    </message>
    <message>
        <source>Optional Linux MD RAID support</source>
        <translation>Optionele ondersteuning voor Linux MD RAID</translation>
    </message>
    <message>
        <source>Process namespace isolation</source>
        <translation>Process namespace isolatie</translation>
    </message>
    <message>
        <source>Host+Target</source>
        <translation>Host+ Doel</translation>
    </message>
    <message>
        <source>The ported helper falls back to a guarded plain chroot when unshare is absent</source>
        <translation>De geporteerde helper valt terug naar een bewaakte chroot wanneer niet gedeeld wordt</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Annuleren</translation>
    </message>
    <message>
        <source>OK</source>
        <translation>OK</translation>
    </message>
    <message>
        <source>Yes</source>
        <translation>Ja.</translation>
    </message>
    <message>
        <source>No</source>
        <translation>Nee</translation>
    </message>
    <message>
        <source>read-only helper diagnostic</source>
        <translation>read-only helper diagnose</translation>
    </message>
</context>
<context>
    <name>legacy::LegacyMainWindow</name>
    <message>
        <source>Boot Bitch Legacy (Etch / KDE 3.5 era)</source>
        <translation>Boot Bitch Legacy (Etch / KDE 3.5 tijdperk)</translation>
    </message>
    <message>
        <source>Linux recovery and boot-repair utility</source>
        <translation>Linux herstel en boot-reparatie hulpprogramma</translation>
    </message>
    <message>
        <source>GUARDED REPAIR</source>
        <translation>GUARDE REPARATIE</translation>
    </message>
    <message>
        <source>Ordinary repairs require an explicitly selected non-host target. The protected running host has a separate deliberate maintenance mode with the same guarded repair stages and requires privilege authorization.</source>
        <translation>Gewone reparaties vereisen een expliciet geselecteerd niet-hostdoel. De beschermde lopende host heeft een aparte doelbewuste onderhoudsmodus met dezelfde bewaakte reparatiefasen en vereist toestemming voor privileges.</translation>
    </message>
    <message>
        <source>Systems</source>
        <translation>Systemen</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>Diagnostica</translation>
    </message>
    <message>
        <source>Repair</source>
        <translation>Reparatie</translation>
    </message>
    <message>
        <source>Chroot Shell</source>
        <translation>Chroot Shell</translation>
    </message>
    <message>
        <source>File Copy</source>
        <translation>Bestand kopiëren</translation>
    </message>
    <message>
        <source>Logs</source>
        <translation>Logs</translation>
    </message>
    <message>
        <source>Settings</source>
        <translation>Instellingen</translation>
    </message>
    <message>
        <source>(not created yet)</source>
        <translation>(nog niet aangemaakt)</translation>
    </message>
    <message>
        <source>Information</source>
        <translation>Informatie</translation>
    </message>
    <message>
        <source>Close</source>
        <translation>Sluiten</translation>
    </message>
    <message>
        <source>&amp;File</source>
        <translation>-&amp;bestand</translation>
    </message>
    <message>
        <source>&amp;Refresh Devices</source>
        <translation>-&amp;apparaten verversen</translation>
    </message>
    <message>
        <source>&amp;Lock Administrator Session</source>
        <translation>&amp;Lock Administrator Sessie</translation>
    </message>
    <message>
        <source>&amp;Quit</source>
        <translation>&amp;Afsluiten</translation>
    </message>
    <message>
        <source>&amp;View</source>
        <translation>-&amp;weergave</translation>
    </message>
    <message>
        <source>&amp;Systems</source>
        <translation>-&amp;systemen</translation>
    </message>
    <message>
        <source>&amp;Diagnostics</source>
        <translation>-&amp;diagnostiek</translation>
    </message>
    <message>
        <source>&amp;Logs</source>
        <translation>-&amp;logs</translation>
    </message>
    <message>
        <source>&amp;Settings</source>
        <translation>-&amp;instellingen</translation>
    </message>
    <message>
        <source>&amp;Auto-size Device Columns</source>
        <translation>&amp;Device Kolommen voor automatische grootte</translation>
    </message>
    <message>
        <source>&amp;Wrap Log Lines</source>
        <translation>&amp;Wrap Loglijnen</translation>
    </message>
    <message>
        <source>&amp;Help</source>
        <translation>-&amp;hulp</translation>
    </message>
    <message>
        <source>&amp;Using Boot Bitch</source>
        <translation>&amp;gebruikt Boot Bitch</translation>
    </message>
    <message>
        <source>&amp;About Boot Bitch</source>
        <translation>&amp;Over Boot Bitch</translation>
    </message>
    <message>
        <source>Device columns auto-sized. Drag headers to fine-tune widths.</source>
        <translation>Apparaat kolommen automatisch. Sleep headers om de breedtes te verfijnen.</translation>
    </message>
    <message>
        <source>A helper command is running; wait for it to finish before locking the session.</source>
        <translation>Er wordt een hulpopdracht uitgevoerd; wacht tot het klaar is voordat de sessie wordt vergrendeld.</translation>
    </message>
    <message>
        <source>Administrator session locked; the next privileged action will request authorization.</source>
        <translation>Beheerder sessie vergrendeld; de volgende bevoorrechte actie zal toestemming vragen.</translation>
    </message>
    <message>
        <source>Using Boot Bitch</source>
        <translation>Gebruik van Boot Bitch</translation>
    </message>
    <message>
        <source>Boot Bitch must run from a different booted Linux environment than the system being repaired. Use a Linux live medium or another Linux installation on a different physical drive.&lt;br&gt;&lt;br&gt;The running host is protected from ordinary repair-target selection, but it can be explicitly selected through &lt;b&gt;Host Maintenance&lt;/b&gt; for guarded native diagnostics and supported maintenance stages.&lt;br&gt;&lt;br&gt;Diagnostics follow the Systems page: the committed repair drive while Host Maintenance is off, or the protected running host while it is active.&lt;br&gt;&lt;br&gt;The first privileged action requests administrator authorization once for this Boot Bitch window; &lt;b&gt;File - Lock Administrator Session&lt;/b&gt; ends that helper session immediately. Every repair keeps the helper&apos;s own runtime preflights.</source>
        <translation>Boot Bitch moet draaien vanuit een andere opgestarte Linux omgeving dan het systeem dat wordt gerepareerd. Gebruik een Linux live medium of een andere Linux installatie op een andere fysieke schijf.&lt;br&gt;&lt;br&gt; De lopende host is beschermd tegen gewone reparatie-doel selectie, maar het kan expliciet worden geselecteerd via &lt;b&gt;Host Maintenance&lt;/b&gt; voor bewaakte inheemse diagnostiek en ondersteunde onderhoudsfasen.&lt;br&gt;&lt;br&gt; Diagnostics volg de Systems pagina: de toegewijde reparatie schijf terwijl Host Maintenance is uitgeschakeld, of de beschermde lopende host terwijl het actief is.&lt;br&gt;&lt;br&gt; De eerste geprivilegieerde actie vraagt toestemming voor beheerder eenmaal voor dit Boot Bitch venster; &lt;b&gt;File - Lock Administrator Session&lt;/b&gt; eindigt die helper sessie onmiddellijk. Elke reparatie houdt de helper zijn eigen vluchttijd voor vluchten.</translation>
    </message>
    <message>
        <source>Select a physical drive; Boot Bitch resolves the most likely Linux system volume automatically. The running host stays protected from ordinary target repairs, with a separate explicit host-maintenance path for its own system. The Details button shows the protected host&apos;s facts in the details pane; selecting any drive row restores the per-drive pane.</source>
        <translation>Selecteer een fysieke schijf; Boot Bitch lost het meest waarschijnlijke Linux systeemvolume automatisch op. De lopende host blijft beschermd tegen gewone doel reparaties, met een aparte expliciete host-onderhoud pad voor zijn eigen systeem. De Details-knop toont de feiten van de beschermde host in het detailpaneel; het selecteren van een rij schijf herstelt de per-drive paneel.</translation>
    </message>
    <message>
        <source>Refresh Devices</source>
        <translation>Apparaten verversen</translation>
    </message>
    <message>
        <source>Re-read the read-only kernel inventory (/proc/partitions, /proc/mounts, /proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* and the udev metadata database). No block device is opened and nothing is written.</source>
        <translation>Herlees de alleen-lezen kernelinventaris (/proc/partitions, /proc/mounts, /proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* en de udev-metadatabase). Er wordt geen blokapparaat geopend en er is niets geschreven.</translation>
    </message>
    <message>
        <source>[OK]</source>
        <translation>[OK]</translation>
    </message>
    <message>
        <source>The running system was detected and stays protected from ordinary target repairs.</source>
        <translation>Het lopende systeem werd gedetecteerd en blijft beschermd tegen gewone doel reparaties.</translation>
    </message>
    <message>
        <source>Detecting running system...</source>
        <translation>Startsysteem wordt gedetecteerd...</translation>
    </message>
    <message>
        <source>Detecting protected storage...</source>
        <translation>Beschermde opslag detecteren...</translation>
    </message>
    <message>
        <source>PROTECTED</source>
        <translation>BESCHERMD</translation>
    </message>
    <message>
        <source>The running host remains protected from ordinary repair-target operations.</source>
        <translation>De lopende host blijft beschermd tegen gewone reparatie-doel operaties.</translation>
    </message>
    <message>
        <source>Details</source>
        <translation>Gegevens</translation>
    </message>
    <message>
        <source>Show read-only details for the protected running host.</source>
        <translation>Alleen-lezen details tonen voor de beschermde host.</translation>
    </message>
    <message>
        <source>Host Maintenance</source>
        <translation>Hostonderhoud</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Standaard maken</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host (menu.lst default directive with a backup and rollback). Requires Host Maintenance and the cached host-default probe.</source>
        <translation>Maak de canonieke geïnstalleerde kernel entry de standaard GRUB-legacy boot entry op de lopende host (menu.lst standaard richtlijn met een back-up en terugrol). Vereist Host Maintenance en de cache host-standaard sonde.</translation>
    </message>
    <message>
        <source>Available repair targets</source>
        <translation>Beschikbare reparatiedoelstellingen</translation>
    </message>
    <message>
        <source>Most likely first</source>
        <translation>Meest waarschijnlijk eerst</translation>
    </message>
    <message>
        <source>Drives are listed by the read-only inventory. Select a row to inspect it; Select Target commits the selected non-host drive with its auto-resolved Linux root component.</source>
        <translation>Drives worden vermeld door de alleen-lezen inventaris. Selecteer een rij om het te inspecteren; Selecteer Target commit de geselecteerde niet-host drive met de auto-opgeloste Linux root component.</translation>
    </message>
    <message>
        <source>Device</source>
        <translation>Apparaat</translation>
    </message>
    <message>
        <source>Size</source>
        <translation>Grootte</translation>
    </message>
    <message>
        <source>Type</source>
        <translation>Type</translation>
    </message>
    <message>
        <source>Filesystem</source>
        <translation>Bestandssysteem</translation>
    </message>
    <message>
        <source>Select Target</source>
        <translation>Doel selecteren</translation>
    </message>
    <message>
        <source>Unlock</source>
        <translation>Ontgrendelen</translation>
    </message>
    <message>
        <source>Authorize</source>
        <translation>Machtiging</translation>
    </message>
    <message>
        <source>Establish the privileged helper session for the current scope now instead of waiting for the next privileged action.</source>
        <translation>Stel nu de bevoorrechte hulpsessie op voor de huidige reikwijdte in plaats van te wachten op de volgende bevoorrechte actie.</translation>
    </message>
    <message>
        <source>Committed target: none</source>
        <translation>Toegewezen doel: geen</translation>
    </message>
    <message>
        <source>Unlock state for the selected drive; the LUKS passphrase is never logged.</source>
        <translation>Ontgrendel status voor het geselecteerde station; de LUKS wachtwoordzin is nooit geregistreerd.</translation>
    </message>
    <message>
        <source>Unlock status</source>
        <translation>Status ontgrendelen</translation>
    </message>
    <message>
        <source>Read-only inventory plus helper-confirmed facts; mirrors the modern Qt6 Selected drive details panel.</source>
        <translation>Alleen-lezen inventaris plus helper-bevestigde feiten; weerspiegelt de moderne Qt6 Geselecteerde schijf details paneel.</translation>
    </message>
    <message>
        <source>Selected drive details</source>
        <translation>Geselecteerde schijfdetails</translation>
    </message>
    <message>
        <source>Field</source>
        <translation>Veld</translation>
    </message>
    <message>
        <source>Value</source>
        <translation>Waarde</translation>
    </message>
    <message>
        <source>Run All runs every available read-only diagnostic for the current scope; selecting a check runs it alone. Diagnostics are read-only and are the only evidence source for the gated repair actions.</source>
        <translation>Run Alle draait elke beschikbare alleen-lezen diagnose voor de huidige scope; het selecteren van een controle draait het alleen. Diagnostics zijn alleen-lezen en zijn de enige bewijsbron voor de gated reparatie acties.</translation>
    </message>
    <message>
        <source>Target: none selected</source>
        <translation>Doel: geen geselecteerd</translation>
    </message>
    <message>
        <source>Diagnostics follow the committed repair target, or the protected running host while Host Maintenance is active.</source>
        <translation>Diagnostics volgen de vastgelegde reparatie doel, of de beschermde lopende host terwijl Host Maintenance is actief.</translation>
    </message>
    <message>
        <source>Run All</source>
        <translation>Alles uitvoeren</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope.</source>
        <translation>Alles uitvoeren - voer elke beschikbare alleen-lezen diagnose voor de huidige scope.</translation>
    </message>
    <message>
        <source>Target configuration:</source>
        <translation>Doelconfiguratie:</translation>
    </message>
    <message>
        <source>Etch-era target configuration files; availability is probed read-only by the helper&apos;s diagnostics.</source>
        <translation>Etch-era doel configuratie bestanden; beschikbaarheid wordt alleen-lezen door de helper diagnostiek onderzocht.</translation>
    </message>
    <message>
        <source>Edit Target File...</source>
        <translation>Doelbestand bewerken...</translation>
    </message>
    <message>
        <source>Runs one read-only diagnostic for the selected scope through the helper (`diagnose &lt;key&gt;` / `host-diagnose &lt;key&gt;`); Run All is the combined report.</source>
        <translation>Draait een alleen-lezen diagnostische voor de geselecteerde scope via de helper ( . diagnostic &lt;key&gt; . .host-diagnostic &lt;key&gt; . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .</translation>
    </message>
    <message>
        <source>Diagnostic checks</source>
        <translation>Diagnostische controles</translation>
    </message>
    <message>
        <source>Check</source>
        <translation>Controleren</translation>
    </message>
    <message>
        <source>Selected diagnostic</source>
        <translation>Geselecteerde diagnose</translation>
    </message>
    <message>
        <source>Select a diagnostic</source>
        <translation>Selecteer een diagnose</translation>
    </message>
    <message>
        <source>Choose a diagnostic from the list.</source>
        <translation>Kies een diagnose uit de lijst.</translation>
    </message>
    <message>
        <source>Ready</source>
        <translation>Klaar</translation>
    </message>
    <message>
        <source>Results</source>
        <translation>Resultaten</translation>
    </message>
    <message>
        <source>Run Diagnostic</source>
        <translation>Diagnostisch uitvoeren</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic for the current scope.</source>
        <translation>Diagnostisch uitvoeren - voer de geselecteerde alleen-lezen diagnose voor de huidige scope.</translation>
    </message>
    <message>
        <source>Copy Results</source>
        <translation>Resultaten kopiëren</translation>
    </message>
    <message>
        <source>Save Results...</source>
        <translation>Resultaten opslaan...</translation>
    </message>
    <message>
        <source>The Full Repair plan runs the selected legacy stages in order through the guarded helper; the individual tools run one stage at a time. Every action stays disabled until the cached capability lines say available and the helper keeps its runtime preflights.</source>
        <translation>Het Full Repair plan draait de geselecteerde legacy stadia in orde door middel van de bewaakte helper; de individuele tools lopen een fase tegelijk. Elke actie blijft uitgeschakeld totdat de cache-capaciteit lijnen zeggen beschikbaar en de helper houdt zijn runtime preflights.</translation>
    </message>
    <message>
        <source>Full Repair plan</source>
        <translation>Volledig reparatieplan</translation>
    </message>
    <message>
        <source>No stages selected</source>
        <translation>Geen stappen geselecteerd</translation>
    </message>
    <message>
        <source>Configure Plan...</source>
        <translation>Plan instellen...</translation>
    </message>
    <message>
        <source>Open Settings to choose which Full Repair stages are part of the plan.</source>
        <translation>Open Instellingen om te kiezen welke Full Repair stadia deel uitmaken van het plan.</translation>
    </message>
    <message>
        <source>Run Full Repair</source>
        <translation>Volledige reparatie uitvoeren</translation>
    </message>
    <message>
        <source>Select a repair drive, or choose Host Maintenance on the protected running-host card.</source>
        <translation>Selecteer een reparatie drive, of kies Host Maintenance op de beschermde running-host kaart.</translation>
    </message>
    <message>
        <source>Stage</source>
        <translation>Fase</translation>
    </message>
    <message>
        <source>Individual repair tools</source>
        <translation>Individuele reparatiegereedschappen</translation>
    </message>
    <message>
        <source>Tool</source>
        <translation>Hulpmiddel</translation>
    </message>
    <message>
        <source>Full Repair</source>
        <translation>Volledige reparatie</translation>
    </message>
    <message>
        <source>not reported</source>
        <translation>niet gemeld</translation>
    </message>
    <message>
        <source>Selected tool</source>
        <translation>Geselecteerd gereedschap</translation>
    </message>
    <message>
        <source>Select a repair tool</source>
        <translation>Selecteer een reparatiegereedschap</translation>
    </message>
    <message>
        <source>Run Tool</source>
        <translation>Gereedschap uitvoeren</translation>
    </message>
    <message>
        <source>Select a tool to review its repair action.</source>
        <translation>Selecteer een hulpmiddel om de reparatie actie te bekijken.</translation>
    </message>
    <message>
        <source>Write actions ask for confirmation and then run the helper&apos;s own runtime preflights; the GUI never weakens them. A repair that is not proven &apos;unchanged&apos; invalidates the cached diagnostics and disables the gated actions until diagnostics run again.</source>
        <translation>Schrijf acties vragen om bevestiging en voer dan de eigen runtime preflights van de helper uit; de GUI verzwakt ze nooit. Een reparatie die niet bewezen is &apos;ongewijzigd&apos; maakt de cache diagnostiek ongeldig en schakelt de gesloten handelingen uit totdat de diagnostiek weer loopt.</translation>
    </message>
    <message>
        <source>Chroot shell</source>
        <translation>Kraanschelp</translation>
    </message>
    <message>
        <source>Offline commands run one at a time in a fresh chroot and cannot answer interactive prompts (apt-get -y upgrade works). Host-shell commands run directly on the running host. The helper&apos;s probe lines gate the command field; the exact reason appears in the tooltip.</source>
        <translation>Offline commando&apos;s draaien één voor één in een frisse chroot en kunnen geen interactieve prompts beantwoorden (apt-get -y upgrade werkt). Host-shell commando&apos;s draaien direct op de lopende host. De sondelijnen van de helper poorten het commando veld; de exacte reden verschijnt in de tooltip.</translation>
    </message>
    <message>
        <source>No &apos;Legacy feature shell:&apos; line is cached; run diagnostics for the selected scope to evaluate the helper&apos;s chroot/timeout containment probes (fail closed).</source>
        <translation>Geen &apos;Legacy feature shell:&apos;-regel wordt gecached; voer diagnostieken uit voor de geselecteerde scope om de chroot/timeout insluitingssondes van de helper te evalueren (mislukt).</translation>
    </message>
    <message>
        <source>Command</source>
        <translation>Commando</translation>
    </message>
    <message>
        <source>Command:</source>
        <translation>Commando:</translation>
    </message>
    <message>
        <source>One reviewed command string, passed to the helper as a single argument (no shell interpolation by the GUI).</source>
        <translation>Een gerecenseerd commando string, doorgegeven aan de helper als een enkel argument (geen shell interpolatie door de GUI).</translation>
    </message>
    <message>
        <source>Run Command</source>
        <translation>Commando uitvoeren</translation>
    </message>
    <message>
        <source>Clear Output</source>
        <translation>Uitvoer wissen</translation>
    </message>
    <message>
        <source>The helper exposes `shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for an offline target chroot and `host-shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for the running host. Both keep the helper&apos;s runtime preflights; this tab enables the command only when the scope is committed, the session is authorized and the scope&apos;s Legacy feature probe reports available.</source>
        <translation>De helper ontmaskert de shell &lt;disk&gt; &lt;root&gt; &lt;commando&gt; Beide houden de runtime preflights van de helper; dit tabblad laat het commando alleen toe als de scope is ingeschakeld, de sessie is toegestaan en de scope&apos;s Legacy feature probe rapporten beschikbaar zijn.</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Bestand kopiëren</translation>
    </message>
    <message>
        <source>Copy and verify files in either direction through the guarded helper (cp -a plus ownership restoration and a per-file byte-compare). The helper&apos;s file-copy probe gates the controls and keeps the direction and path containment checks.</source>
        <translation>Kopieer en verifieer bestanden in beide richtingen via de bewaakte helper (cp -a plus eigendomsherstel en een byte-vergelijk per bestand). De helper&apos;s bestandskopie sonde poorten de controles en houdt de richting en pad insluiting controles.</translation>
    </message>
    <message>
        <source>Preview Changes</source>
        <translation>Wijzigingen voorbeeld</translation>
    </message>
    <message>
        <source>Run a copy dry-run through the guarded helper. No files are changed.</source>
        <translation>Doe een kopie door de bewaakte helper. Er worden geen bestanden gewijzigd.</translation>
    </message>
    <message>
        <source>Copy and Verify</source>
        <translation>Kopiëren en verifiëren</translation>
    </message>
    <message>
        <source>Copy staged items and verify the result. Existing destination names are overwritten when source content differs; unrelated destination files are never deleted.</source>
        <translation>Kopieer geënsceneerde items en verifieer het resultaat. Bestaande bestemmingsnamen worden overschreven wanneer broninhoud verschilt; niet-verbonden bestemmingsbestanden worden nooit verwijderd.</translation>
    </message>
    <message>
        <source>Direction:</source>
        <translation>Richting:</translation>
    </message>
    <message>
        <source>Choose which system supplies the source files and which system receives them.</source>
        <translation>Kies welk systeem de bronbestanden levert en welk systeem ze ontvangt.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from this host</source>
        <translation>1. Selecteer bronbestanden of mappen van deze host</translation>
    </message>
    <message>
        <source>Source</source>
        <translation>Bron</translation>
    </message>
    <message>
        <source>Files and folders staged for the verified copy. The legacy backend copies with cp -a and restores ownership with chown --reference; every regular file is byte-compared after the copy.</source>
        <translation>Bestanden en mappen geënsceneerd voor de geverifieerde kopie. De legacy backend kopieert met cp -a en herstelt eigenaarschap met clown --reference; elke reguliere bestand is byte-vergelijk na de kopie.</translation>
    </message>
    <message>
        <source>Add Files...</source>
        <translation>Bestanden toevoegen...</translation>
    </message>
    <message>
        <source>Add Folder...</source>
        <translation>Map toevoegen...</translation>
    </message>
    <message>
        <source>Remove</source>
        <translation>Verwijderen</translation>
    </message>
    <message>
        <source>Clear</source>
        <translation>Wissen</translation>
    </message>
    <message>
        <source>Clear the staged source list (nothing is copied or deleted).</source>
        <translation>De geënsceneerde bronlijst wissen (niets wordt gekopieerd of verwijderd).</translation>
    </message>
    <message>
        <source>2. Choose destination in repaired system</source>
        <translation>2. Kies bestemming in gerepareerd systeem</translation>
    </message>
    <message>
        <source>unavailable: see the helper&apos;s Legacy feature file-copy: probe reason above</source>
        <translation>niet beschikbaar: zie de Legacy-functie van de helper bestandskopie: sonde reden hierboven</translation>
    </message>
    <message>
        <source>An absolute path inside the selected repair system (Host to Repair) or on the running host (Repair to Host).</source>
        <translation>Een absoluut pad binnen het geselecteerde reparatiesysteem (Host to Repair) of op de lopende host (Repair to Host).</translation>
    </message>
    <message>
        <source>Browse Target Folders...</source>
        <translation>Doelmappen doorbladeren...</translation>
    </message>
    <message>
        <source>Browse the selected repair system through the helper&apos;s temporary read-only mounts and choose an absolute destination path. No target files are changed while browsing.</source>
        <translation>Blader door het geselecteerde reparatiesysteem via de tijdelijke alleen-lezen mounts van de helper en kies een absoluut bestemmingspad. Er worden geen doelbestanden gewijzigd tijdens het surfen.</translation>
    </message>
    <message>
        <source>3. Ownership and copy policy</source>
        <translation>3. Eigendom en kopieerbeleid</translation>
    </message>
    <message>
        <source>Ownership:</source>
        <translation>Eigendom:</translation>
    </message>
    <message>
        <source>Smart destination ownership (recommended)</source>
        <translation>Slimme eigendom van bestemming (aanbevolen)</translation>
    </message>
    <message>
        <source>Preserve source numeric UID/GID</source>
        <translation>Bewaar bron numerieke UID/GID</translation>
    </message>
    <message>
        <source>Smart mode validates UID/GID identity mapping across the two systems and falls back to the destination-directory owner when the same numeric ID means a different account (the legacy backend implements it with chown --reference).</source>
        <translation>Slimme modus valideert UID/GID-identiteitskartering over de twee systemen en valt terug naar de eigenaar van de bestemmingsdirectory wanneer hetzelfde numerieke ID een ander account betekent (de legacy backend implementeert het met clown --reference).</translation>
    </message>
    <message>
        <source>Application log</source>
        <translation>Programmalogboek</translation>
    </message>
    <message>
        <source>The complete session register; Save As... writes every entry even while a filter hides lines. If a writable system share mount exists at /host, Save As... starts there; otherwise the log directory is the fallback. Prior session files are listed read-only.</source>
        <translation>Het volledige sessieregister; Opslaan Als... schrijft elk item zelfs terwijl een filter regels verbergt. Als er een beschrijfbare systeemshare mount bestaat op /host, begint Save As...; anders is de log directory de fallback. Eerdere sessiebestanden staan alleen-lezen vermeld.</translation>
    </message>
    <message>
        <source>Session logs</source>
        <translation>Sessielogboeken</translation>
    </message>
    <message>
        <source>Session</source>
        <translation>Directoraat</translation>
    </message>
    <message>
        <source>The first entry is the live session; earlier files in the log directory are listed read-only below it.</source>
        <translation>Het eerste item is de live sessie; eerdere bestanden in de log directory staan er alleen-lezen onder.</translation>
    </message>
    <message>
        <source>New Session Log</source>
        <translation>Nieuwe sessielog</translation>
    </message>
    <message>
        <source>Close the active session file; it becomes a prior session and the next log entry starts a new file.</source>
        <translation>Sluit het actieve sessiebestand; het wordt een eerdere sessie en het volgende log item start een nieuw bestand.</translation>
    </message>
    <message>
        <source>Add Note</source>
        <translation>Opmerking toevoegen</translation>
    </message>
    <message>
        <source>Append a NOTE entry to the live session register.</source>
        <translation>Voeg een NOOT-invoer toe aan het live-sessieregister.</translation>
    </message>
    <message>
        <source>Delete</source>
        <translation>Verwijderen</translation>
    </message>
    <message>
        <source>Delete the selected prior session file (the live session is never deleted).</source>
        <translation>Verwijder het geselecteerde voorafgaande sessiebestand (de livesessie wordt nooit verwijderd).</translation>
    </message>
    <message>
        <source>Refresh</source>
        <translation>Verversen</translation>
    </message>
    <message>
        <source>Save As...</source>
        <translation>Opslaan als...</translation>
    </message>
    <message>
        <source>Save the complete session log (all entries, not just the current filter).</source>
        <translation>Sla het volledige sessielogboek op (alle items, niet alleen het huidige filter).</translation>
    </message>
    <message>
        <source>Clear Register</source>
        <translation>Register wissen</translation>
    </message>
    <message>
        <source>Clear the live register and view; prior session files are never modified.</source>
        <translation>Wis het live register en weergave; eerdere sessiebestanden worden nooit gewijzigd.</translation>
    </message>
    <message>
        <source>Search log:</source>
        <translation>Zoeklogboek:</translation>
    </message>
    <message>
        <source>Type any characters to show matching log entries (case-insensitive). Save As always writes every entry.</source>
        <translation>Typ tekens om overeenkomstige logitems te tonen (hoofdlettergevoelig). Opslaan Zoals altijd schrijft elke vermelding.</translation>
    </message>
    <message>
        <source>Filter:</source>
        <translation>Filter:</translation>
    </message>
    <message>
        <source>Filter the visible log by entry kind. Selecting a diagnostic section shows the lines captured for that section; a workflow filter such as File system repair or Package repair shows its mapped repair lines (File copy has no lines in this frontend). Save As always writes every entry.</source>
        <translation>Filter het zichtbare logboek op invoertype. Het selecteren van een kenmerkende sectie toont de lijnen gevangen voor dat gedeelte; een workflow filter zoals Bestandssysteem reparatie of Pakket reparatie toont de in kaart gebrachte reparatie lijnen (Bestand kopie heeft geen lijnen in deze frontend). Opslaan Zoals altijd schrijft elke vermelding.</translation>
    </message>
    <message>
        <source>Settings are stored per user under ~/.qt/, one file per settings group (devicesrc, logsrc, diagnosticsrc, repairrc), and are saved immediately on every change and on close. Launch the GUI as the same user to keep your overrides; a GUI started as root keeps its own copies.</source>
        <translation>Instellingen worden opgeslagen per gebruiker onder ~/.qt/, een bestand per instelling groep (devicesrc, logsrc, diagnosticsrc, repairrc), en worden onmiddellijk opgeslagen bij elke wijziging en bij sluiten. Start de GUI als dezelfde gebruiker om uw overrides te behouden; een GUI gestart als root bewaart zijn eigen kopieën.</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Apparaatontdekking</translation>
    </message>
    <message>
        <source>Show devices without an identified Linux installation</source>
        <translation>Devices tonen zonder geïdentificeerde Linux installatie</translation>
    </message>
    <message>
        <source>Show removable and USB storage</source>
        <translation>Verwijderbare en USB-opslag tonen</translation>
    </message>
    <message>
        <source>Show encrypted devices before unlocking</source>
        <translation>Versleutelde apparaten tonen voor het ontgrendelen</translation>
    </message>
    <message>
        <source>When off, drives without a visible Linux filesystem are hidden unless they still contain an encrypted device and encrypted devices are shown.</source>
        <translation>Wanneer uit, drives zonder een zichtbaar Linux bestandssysteem worden verborgen tenzij ze nog steeds een gecodeerd apparaat en gecodeerde apparaten worden weergegeven.</translation>
    </message>
    <message>
        <source>When off, removable and USB drives are hidden from the repair-target list.</source>
        <translation>Wanneer uit, verwijderbare en USB-drives zijn verborgen uit de reparatie-doellijst.</translation>
    </message>
    <message>
        <source>When off, drives with an encrypted device are hidden until the volume is unlocked.</source>
        <translation>Wanneer uit, rijden met een gecodeerd apparaat worden verborgen totdat het volume is ontgrendeld.</translation>
    </message>
    <message>
        <source>Include the %1 stage in the Full Repair plan. The stage runs in the plan order shown on the Repair tab.</source>
        <translation>Voeg de %1-fase toe aan het volledige reparatieplan. De etappe loopt in de planvolgorde weergegeven op het tabblad Reparatie.</translation>
    </message>
    <message>
        <source>Automatically regenerate read-only diagnostics after repairs or target changes</source>
        <translation>Automatisch read-only diagnostiek regenereren na reparaties of doelwijzigingen</translation>
    </message>
    <message>
        <source>Regenerates the cached read-only diagnostics for the current scope after an operation that invalidates them (LUKS unlock, target configuration edit). It runs only inside an already authorized administrator session and never opens an authorization prompt by itself.</source>
        <translation>Regenereert de gecachede alleen-lezen diagnostica voor de huidige scope na een operatie die ze ongeldig maakt (LUKS ontgrendelen, doelconfiguratie bewerken). Het draait alleen binnen een reeds geautoriseerde administrator sessie en opent nooit zelf een autorisatieprompt.</translation>
    </message>
    <message>
        <source>Wrap long log lines</source>
        <translation>Lange logregels omdraaien</translation>
    </message>
    <message>
        <source>Mandatory safety controls</source>
        <translation>Verplichte veiligheidscontroles</translation>
    </message>
    <message>
        <source>Host capabilities and dependencies</source>
        <translation>Hostmogelijkheden en afhankelijkheden</translation>
    </message>
    <message>
        <source>Refresh Capabilities</source>
        <translation>Mogelijkheden verversen</translation>
    </message>
    <message>
        <source>Re-run the read-only host capability probes (a PATH search, nothing is executed) and refresh the distribution summary.</source>
        <translation>Re-run de alleen-lezen host mogelijkheden sondes (een PATH zoekopdracht, er wordt niets uitgevoerd) en vernieuw de distributie samenvatting.</translation>
    </message>
    <message>
        <source>Install Missing Support...</source>
        <translation>Ontbrekende ondersteuning installeren...</translation>
    </message>
    <message>
        <source>Automatic installation will require explicit package mapping and privilege authorization.</source>
        <translation>Automatische installatie vereist expliciete pakket mapping en privilege machtiging.</translation>
    </message>
    <message>
        <source>Application configuration</source>
        <translation>Programmaconfiguratie</translation>
    </message>
    <message>
        <source>Boot Bitch Legacy</source>
        <translation>Boot Bitch Legacy</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list first.</source>
        <translation>Selecteer eerst een fysieke schijf in de lijst met beschikbare reparatiedoelen.</translation>
    </message>
    <message>
        <source>Protected system</source>
        <translation>Beschermd systeem</translation>
    </message>
    <message>
        <source>The running system cannot be selected as a repair target. Use Host Maintenance for the protected running host or choose another disk.</source>
        <translation>Het draaiende systeem kan niet als reparatiedoel worden geselecteerd. Gebruik Host Maintenance voor de beveiligde host of kies een andere schijf.</translation>
    </message>
    <message>
        <source>Unlock or select a Linux system first</source>
        <translation>Ontgrendelen of eerst een Linux-systeem selecteren</translation>
    </message>
    <message>
        <source>This encrypted drive has no visible Linux filesystem yet. Use Unlock, refresh devices, and select the target after its Linux root is detected.</source>
        <translation>Deze versleutelde schijf heeft nog geen zichtbaar Linux bestandssysteem. Gebruik Unlock, vernieuw apparaten, en selecteer het doel nadat de Linux root is gedetecteerd.</translation>
    </message>
    <message>
        <source>The selected root component (%1) belongs to the running system and cannot be committed as a repair target. Use Host Maintenance for the protected running host.</source>
        <translation>De geselecteerde root component (%1) behoort tot het draaiende systeem en kan niet als reparatiedoel worden vastgelegd. Gebruik Host Maintenance voor de beschermde host.</translation>
    </message>
    <message>
        <source>repair target commit</source>
        <translation>repareren doel commit</translation>
    </message>
    <message>
        <source>Repair drive selected: %1</source>
        <translation>Reparatiestation geselecteerd: %1</translation>
    </message>
    <message>
        <source>; best detected system component: %1</source>
        <translation>; best gedetecteerde systeemcomponent: %1</translation>
    </message>
    <message>
        <source>. No mount or repair action was performed.</source>
        <translation>. Er is geen montage- of reparatieactie uitgevoerd.</translation>
    </message>
    <message>
        <source>Make Default unavailable</source>
        <translation>Standaard niet beschikbaar maken</translation>
    </message>
    <message>
        <source>Make Default is a running-host action on this frontend; enter Host Maintenance first.</source>
        <translation>Make Standaard is een actieve-host actie op deze frontend; voer eerst Host Maintenance in.</translation>
    </message>
    <message>
        <source>Administrator authorization required</source>
        <translation>Vergunning vereist</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize first.</source>
        <translation>Er is geen beheerdersessie actief; druk eerst op Machtigen.</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host?

The helper verifies /boot/grub/menu.lst, sets the `default &lt;N&gt;` directive to the canonical entry, backs the menu up first and restores it on any failure.</source>
        <translation>Maak de canonieke geïnstalleerde kernel entry de standaard GRUB-legacy boot entry op de lopende host?

De helper overtuigt /boot/grub/menu.lst, stelt de richtlijn &quot;instant &lt;N&gt;&quot; in op de canonieke ingang, back-upt het menu als eerste en herstelt het bij een fout.</translation>
    </message>
    <message>
        <source>Host Maintenance unavailable</source>
        <translation>Hostonderhoud niet beschikbaar</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a committed repair target or a detected running host.</source>
        <translation>De lopende host target kon niet worden gedetecteerd; diagnostiek moet een vastgelegde reparatie doel of een gedetecteerde lopende host.</translation>
    </message>
    <message>
        <source>Exit Host Maintenance</source>
        <translation>Onderhoud van de gastheer afsluiten</translation>
    </message>
    <message>
        <source>Host Maintenance is active; diagnostics and gated repairs target the protected running host.</source>
        <translation>Host Maintenance is actief; diagnostiek en gated reparaties richten zich op de beschermde host.</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list on the Systems tab first, or use Host Maintenance for the protected running host.</source>
        <translation>Selecteer eerst een fysieke schijf in de lijst met beschikbare hersteldoelen op het tabblad Systems, of gebruik Host Maintenance voor de beschermde host.</translation>
    </message>
    <message>
        <source>The selected drive (%1) is the protected running host. Choose Host Maintenance on the Systems tab to run read-only host diagnostics and guarded host repairs; ordinary target repairs stay disabled.</source>
        <translation>Het geselecteerde station (%1) is de beschermde host. Kies Host Onderhoud op het tabblad Systems om alleen-lezen host diagnostiek en bewaakte host reparaties uit te voeren; gewone doel reparaties blijven uitgeschakeld.</translation>
    </message>
    <message>
        <source>No repair target is committed. Choose Select Target on the Systems tab (or Host Maintenance for the protected running host) first.</source>
        <translation>Er is geen reparatiedoel vastgelegd. Kies eerst het doel op het tabblad Systems (of Host Maintenance voor de beschermde host) selecteren.</translation>
    </message>
    <message>
        <source>The selection changed after the target was committed. Choose Select Target again.</source>
        <translation>De selectie veranderde nadat het doel was vastgelegd. Kies opnieuw doel selecteren.</translation>
    </message>
    <message>
        <source>Diagnostics scope required</source>
        <translation>Vereiste reikwijdte van de diagnose</translation>
    </message>
    <message>
        <source>Diagnostics scope unresolved</source>
        <translation>Diagnostische reikwijdte onopgelost</translation>
    </message>
    <message>
        <source>The running host target could not be resolved; use Refresh Devices and commit a repair target or re-enter Host Maintenance.</source>
        <translation>De lopende host target kon niet worden opgelost; gebruik Vernieuw apparaten en commit een reparatie doel of opnieuw invoeren Host Maintenance.</translation>
    </message>
    <message>
        <source>Diagnostic check required</source>
        <translation>Diagnostische controle vereist</translation>
    </message>
    <message>
        <source>Select a diagnostic check in the list first.</source>
        <translation>Selecteer eerst een diagnostische controle in de lijst.</translation>
    </message>
    <message>
        <source>Repair target required</source>
        <translation>Repareren doel vereist</translation>
    </message>
    <message>
        <source>Target file editing needs a committed repair target. Running-host maintenance has no target-file editing; commit an offline target first.</source>
        <translation>Doel bestand bewerken moet een vastgelegde reparatie doel. Het uitvoeren van-host onderhoud heeft geen doel-bestand bewerken; commit eerst een offline doel.</translation>
    </message>
    <message>
        <source>Configuration file required</source>
        <translation>Configuratiebestand vereist</translation>
    </message>
    <message>
        <source>Select a target configuration file first.</source>
        <translation>Selecteer eerst een doelconfiguratiebestand.</translation>
    </message>
    <message>
        <source>Edit target %1</source>
        <translation>Doel %1 bewerken</translation>
    </message>
    <message>
        <source>Edit this target file through the guarded administrator helper. A successful save invalidates cached diagnostics; rerun diagnostics before repair. Generated files such as /boot/grub/menu.lst may be replaced by the next bootloader update.</source>
        <translation>Bewerk dit doelbestand via de bewaakte beheerderhelper. Een succesvolle opslaan ongeldig maakt cached diagnostiek; opnieuw diagnostiek voor reparatie. Gegenereerde bestanden zoals /boot/grub/menu.lst kunnen worden vervangen door de volgende bootloader-update.</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Annuleren</translation>
    </message>
    <message>
        <source>Save Target File</source>
        <translation>Doelbestand opslaan</translation>
    </message>
    <message>
        <source>No changes to %1.</source>
        <translation>Geen wijzigingen in %1.</translation>
    </message>
    <message>
        <source>Configuration write refused</source>
        <translation>Configuratie schrijven geweigerd</translation>
    </message>
    <message>
        <source>The edited content contains NUL bytes; the guarded write refuses it.</source>
        <translation>De bewerkte inhoud bevat NUL bytes; het bewaakte schrijven weigert het.</translation>
    </message>
    <message>
        <source>File too large</source>
        <translation>Bestand te groot</translation>
    </message>
    <message>
        <source>The edited file is larger than 1 MiB. The guarded write refuses it; edit the file from a console instead.</source>
        <translation>Het bewerkte bestand is groter dan 1 MiB. Het bewaakte schrijven weigert het; het bestand bewerken vanaf een console in plaats daarvan.</translation>
    </message>
    <message>
        <source>Write target configuration</source>
        <translation>Doelconfiguratie schrijven</translation>
    </message>
    <message>
        <source>Write the edited contents to %1? This modifies the repair target and invalidates cached diagnostics.</source>
        <translation>De bewerkte inhoud naar %1 schrijven? Dit wijzigt de reparatie doel en ongeldig maakt gecached diagnostiek.</translation>
    </message>
    <message>
        <source>No diagnostic results to copy yet.</source>
        <translation>Nog geen diagnostische resultaten te kopiëren.</translation>
    </message>
    <message>
        <source>Diagnostic results copied to the clipboard.</source>
        <translation>Diagnostische resultaten gekopieerd naar het klembord.</translation>
    </message>
    <message>
        <source>No diagnostic results to save yet.</source>
        <translation>Nog geen diagnostische resultaten op te slaan.</translation>
    </message>
    <message>
        <source>Text files (*.txt);;All files (*)</source>
        <translation>Tekstbestanden (*.txt);;Alle bestanden (*)</translation>
    </message>
    <message>
        <source>Save diagnostic results</source>
        <translation>Diagnostische resultaten opslaan</translation>
    </message>
    <message>
        <source>Could not write %1.</source>
        <translation>Kon %1 niet schrijven.</translation>
    </message>
    <message>
        <source>Diagnostic results saved to %1</source>
        <translation>Kenmerkende resultaten opgeslagen in %1</translation>
    </message>
    <message>
        <source>Unlock not available</source>
        <translation>Ontgrendelen niet beschikbaar</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked. Select an offline repair target to unlock.</source>
        <translation>De beschermde host kan niet worden ontgrendeld. Selecteer een offline reparatiedoel om te ontgrendelen.</translation>
    </message>
    <message>
        <source>No locked LUKS component is currently visible on this selected drive.</source>
        <translation>Er is momenteel geen vaststaand LUKS-component zichtbaar op dit geselecteerde station.</translation>
    </message>
    <message>
        <source>Confirm LUKS unlock</source>
        <translation>LUKS ontgrendelen bevestigen</translation>
    </message>
    <message>
        <source>Unlock %1 on %2?

The helper opens a temporary device-mapper mapping with cryptsetup and keeps it open for this recovery session. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>%1 ontgrendelen op %2?

De helper opent een tijdelijke apparaat-mapper mapping met cryptsetup en houdt het open voor deze herstelsessie. De wachtwoordzin reist door een privé-sleutelbestand en wordt nooit in commandoargumenten of logs geplaatst.</translation>
    </message>
    <message>
        <source>LUKS unlock</source>
        <translation>LUKS ontgrendelen</translation>
    </message>
    <message>
        <source>Unlock LUKS repair target</source>
        <translation>Ontgrendel LUKS reparatiedoel</translation>
    </message>
    <message>
        <source>Enter the passphrase for %1.

It is sent only to cryptsetup over the helper&apos;s standard input and is never logged or placed on a command line.</source>
        <translation>Voer de wachtwoordzin in voor %1.

Het wordt alleen verzonden naar cryptsetup via de standaard invoer van de helper en wordt nooit gelogd of geplaatst op een commandoregel.</translation>
    </message>
    <message>
        <source>Passphrase required</source>
        <translation>Wachtwoordzin vereist</translation>
    </message>
    <message>
        <source>An empty passphrase was not submitted. Enter the LUKS passphrase or choose Cancel.</source>
        <translation>Een lege wachtwoordzin werd niet ingediend. Voer de LUKS wachtwoordzin in of kies Annuleren.</translation>
    </message>
    <message>
        <source>Unlock keyfile unavailable</source>
        <translation>Sleutelbestand openen is niet beschikbaar</translation>
    </message>
    <message>
        <source>The LUKS passphrase could not be written to a private keyfile in %1; the unlock was not started.</source>
        <translation>De LUKS wachtwoordzin kon niet geschreven worden naar een privé-sleutelbestand in %1; de unlock is niet gestart.</translation>
    </message>
    <message>
        <source>Configuration write unavailable</source>
        <translation>Schrijven van configuratie is niet beschikbaar</translation>
    </message>
    <message>
        <source>The edited content could not be written to a private temporary file in %1; the write was not started.</source>
        <translation>De bewerkte inhoud kon niet geschreven worden naar een privé tijdelijk bestand in %1; de schrijftekst is niet gestart.</translation>
    </message>
    <message>
        <source>ERROR: cannot read the session log %1</source>
        <translation>FOUT: kan het sessielog %1 niet lezen</translation>
    </message>
    <message>
        <source>Viewing a prior session log (read-only): %1</source>
        <translation>Een vorige sessielog bekijken (alleen-lezen): %1</translation>
    </message>
    <message>
        <source>Session log list refreshed.</source>
        <translation>Sessieloglijst ververst.</translation>
    </message>
    <message>
        <source>Note:</source>
        <translation>Opmerking:</translation>
    </message>
    <message>
        <source>Delete session log</source>
        <translation>Sessielog verwijderen</translation>
    </message>
    <message>
        <source>Delete %1 permanently? This cannot be undone.</source>
        <translation>%1 definitief verwijderen? Dit kan niet ongedaan worden gemaakt.</translation>
    </message>
    <message>
        <source>%1 changed while the confirmation was open; the delete was refused.</source>
        <translation>%1 veranderde tijdens het openen van de bevestiging; de verwijdering werd geweigerd.</translation>
    </message>
    <message>
        <source>Unable to delete %1.</source>
        <translation>Kon %1 niet verwijderen.</translation>
    </message>
    <message>
        <source>Device discovery filters updated.</source>
        <translation>Apparaatontdekkingsfilters bijgewerkt.</translation>
    </message>
    <message>
        <source>Unknown Linux distribution</source>
        <translation>Onbekende Linux distributie</translation>
    </message>
    <message>
        <source>sudo / gksu (no KAuth on this frontend)</source>
        <translation>sudo / gksu (geen KAuth op deze frontend)</translation>
    </message>
    <message>
        <source>Available</source>
        <translation>Beschikbaar</translation>
    </message>
    <message>
        <source>Missing on this frontend</source>
        <translation>Ontbrekend op deze frontend</translation>
    </message>
    <message>
        <source>Missing</source>
        <translation>Ontbrekend</translation>
    </message>
    <message>
        <source>No repair tool is selected.</source>
        <translation>Er is geen reparatiegereedschap geselecteerd.</translation>
    </message>
    <message>
        <source>A helper command is already running.</source>
        <translation>Er is al een helpercommando actief.</translation>
    </message>
    <message>
        <source>Restore Graphical Login is a host-scope stage on this legacy frontend; enter Host Maintenance to run it.</source>
        <translation>Grafische Login herstellen is een host-scope podium op deze legacy frontend; voer Host Maintenance om het uit te voeren.</translation>
    </message>
    <message>
        <source>The selected scope has no resolved root component; use Refresh Devices and commit the repair target again.</source>
        <translation>De geselecteerde scope heeft geen opgelost root component; gebruik Refresh Devices en commit het reparatiedoel opnieuw.</translation>
    </message>
    <message>
        <source>This legacy frontend exposes no %1 action; the helper reports the capability as available.</source>
        <translation>Deze legacy frontend stelt geen %1 actie bloot; de helper rapporteert de mogelijkheid als beschikbaar.</translation>
    </message>
    <message>
        <source>Run this guarded repair action using the cached read-only diagnostic evidence. A confirmation is shown first.</source>
        <translation>Voer deze bewaakte reparatie actie met behulp van de cache read-only kenmerkende bewijs. Een bevestiging wordt eerst getoond.</translation>
    </message>
    <message>
        <source>Enabled in Settings</source>
        <translation>Ingeschakeld in instellingen</translation>
    </message>
    <message>
        <source>Disabled in Settings - enable it to include this stage</source>
        <translation>Uitgeschakeld in instellingen - laat het toe om deze fase op te nemen</translation>
    </message>
    <message>
        <source>Unavailable: %1</source>
        <translation>Niet beschikbaar: %1</translation>
    </message>
    <message>
        <source>
Unavailable: %1</source>
        <translation>
Niet beschikbaar: %1</translation>
    </message>
    <message>
        <source>Repair tool unavailable</source>
        <translation>Reparatiegereedschap niet beschikbaar</translation>
    </message>
    <message>
        <source>Confirm repair</source>
        <translation>Bevestig reparatie</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Your saved selection is kept and its availability is re-checked when diagnostics for this scope complete.</source>
        <translation>Nog geen bewijs voor deze fase. Uw opgeslagen selectie wordt bewaard en de beschikbaarheid wordt opnieuw gecontroleerd wanneer de diagnostiek voor deze scope voltooid.</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Run diagnostics for the selected scope to populate the Full Repair plan; your selection is saved once the stage becomes available.</source>
        <translation>Nog geen bewijs voor deze fase. Voer diagnostiek voor de geselecteerde scope om het volledige reparatieplan te vullen; uw selectie wordt opgeslagen zodra het stadium beschikbaar is.</translation>
    </message>
    <message>
        <source>Unknown Full Repair stage.</source>
        <translation>Onbekende volledige reparatie fase.</translation>
    </message>
    <message>
        <source>No Full Repair stages are selected or available; use Configure Plan... to choose the stages.</source>
        <translation>Er zijn geen volledige herstelfasen geselecteerd of beschikbaar; gebruik Plan instellen... om de fasen te kiezen.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab to establish the session.</source>
        <translation>Administrator autorisatie is vereist; druk op Machtigen op het tabblad Systems of Repair om de sessie vast te stellen.</translation>
    </message>
    <message>
        <source>Runs the selected stages using the cached read-only diagnostic evidence after privilege confirmation.</source>
        <translation>Leidt de geselecteerde stadia met behulp van de cache read-only diagnostische bewijs na privilege bevestiging.</translation>
    </message>
    <message>
        <source>No Full Repair stages selected - use Configure Plan... or Settings.</source>
        <translation>Geen volledige herstelfasen geselecteerd - gebruik Plan configureren... of Instellingen.</translation>
    </message>
    <message>
        <source>1 stage selected</source>
        <translation>1 fase geselecteerd</translation>
    </message>
    <message>
        <source>%1 stages selected</source>
        <translation>%1-fasen geselecteerd</translation>
    </message>
    <message>
        <source>No repair stages are selected. Use Configure Plan... to choose the stages Full Repair will run.</source>
        <translation>Er worden geen herstelfasen geselecteerd. Gebruik Plan configureren... om de stadia volledig te repareren.</translation>
    </message>
    <message>
        <source>No selected stages are available. %1</source>
        <translation>Er zijn geen geselecteerde stadia beschikbaar. %1</translation>
    </message>
    <message>
        <source>Full Repair unavailable</source>
        <translation>Volledige reparatie is niet beschikbaar</translation>
    </message>
    <message>
        <source>Run the Full Repair plan?

The selected stages run in order through the helper&apos;s guarded repair command:

</source>
        <translation>Het volledige reparatieplan uitvoeren?

De geselecteerde stadia worden uitgevoerd in volgorde door middel van het bewaakte reparatie commando van de helper:

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
De helper houdt elke runtime preflight; een fase die mislukt stopt het plan.</translation>
    </message>
    <message>
        <source>Running %1 through the privileged helper... The Logs tab keeps the complete transcript.</source>
        <translation>%1 door de bevoorrechte helper... Het tabblad Logs bewaart het volledige transcript.</translation>
    </message>
    <message>
        <source>Shell scope required</source>
        <translation>Shell-bereik vereist</translation>
    </message>
    <message>
        <source>No administrator session is active.

Press Authorize on the Systems or Repair tab to establish the session, or enter Host Maintenance / commit a repair target on the Systems tab; the chroot shell then reuses the cached authorization.</source>
        <translation>Er is geen beheerdersessie actief.

Druk op Machtigen op het tabblad Systems of Repair om de sessie vast te stellen, of voer Host Maintenance / commit een reparatie doel op het tabblad Systems; de chroot shell hergebruikt vervolgens de cache-vergunning.</translation>
    </message>
    <message>
        <source>Shell command required</source>
        <translation>Shell commando vereist</translation>
    </message>
    <message>
        <source>Enter the command to run first.</source>
        <translation>Voer eerst het commando in om uit te voeren.</translation>
    </message>
    <message>
        <source>Confirm running-host command</source>
        <translation>Start-host commando bevestigen</translation>
    </message>
    <message>
        <source>Run this command as root on the protected running host?

%1

The helper keeps its runtime preflights; the command is passed as one argument and is never interpreted by the GUI.</source>
        <translation>Dit commando uitvoeren als root op de beschermde host?

%1

De helper houdt zijn runtime preflights; het commando wordt doorgegeven als één argument en wordt nooit geïnterpreteerd door de GUI.</translation>
    </message>
    <message>
        <source>Running %1...</source>
        <translation>%1 wordt gestart...</translation>
    </message>
    <message>
        <source>Authorization scope required</source>
        <translation>Vereiste reikwijdte van de vergunning</translation>
    </message>
    <message>
        <source>administrator authorization</source>
        <translation>Vergunning van de beheerder</translation>
    </message>
    <message>
        <source>Administrator authorization</source>
        <translation>Vergunning voor beheerders</translation>
    </message>
    <message>
        <source>Administrator authorization is required for %1.

Enter the password for %2 (sudo). It is used only for this sudo authentication, is sent over a pipe and is never logged or placed on a command line. The authorization is cached for this session and reused by diagnostics and repairs.</source>
        <translation>Administrator autorisatie is vereist voor %1.

Voer het wachtwoord in voor %2 (sudo). Het wordt alleen gebruikt voor deze sudo authenticatie, wordt verzonden via een pipe en wordt nooit aangemeld of geplaatst op een opdrachtregel. De toestemming wordt gecached voor deze sessie en hergebruikt door diagnostiek en reparaties.</translation>
    </message>
    <message>
        <source>your account</source>
        <translation>uw account</translation>
    </message>
    <message>
        <source>Password required</source>
        <translation>Wachtwoord vereist</translation>
    </message>
    <message>
        <source>An empty password was not submitted. Enter the sudo password or choose Cancel.</source>
        <translation>Er is geen leeg wachtwoord ingediend. Voer het sudo wachtwoord in of kies Annuleren.</translation>
    </message>
    <message>
        <source>Administrator authorization failed</source>
        <translation>Administrator toestemming mislukt</translation>
    </message>
    <message>
        <source>sudo did not accept the password: %1

The command was not started.</source>
        <translation>sudo heeft het wachtwoord niet geaccepteerd: %1

Het commando is niet gestart.</translation>
    </message>
    <message>
        <source>elevation requires an interactive sudo password; run the smoke as root or after `sudo -S -v` with --elevate &apos;sudo -n&apos;</source>
        <translation>hoogte vereist een interactief sudo-wachtwoord; run de rook als root of na</translation>
    </message>
    <message>
        <source>No administrator session is active for %1.

Press Authorize on the Systems or Repair tab to establish the session now, or enter Host Maintenance / commit a repair target on the Systems tab; diagnostics and repairs then reuse the cached authorization.</source>
        <translation>Geen beheerdersessie is actief voor %1.

Druk op Autoriseren op het tabblad Systems of Repair om de sessie nu vast te stellen, of voer Host Maintenance / commit een reparatie doel op het tabblad Systems; diagnostiek en reparaties vervolgens hergebruiken de cached autorisatie.</translation>
    </message>
    <message>
        <source>Administrator authorization expired</source>
        <translation>Vergunning van de beheerder verlopen</translation>
    </message>
    <message>
        <source>The cached administrator authorization for %1 expired or was refused.

Press Authorize on the Systems or Repair tab to re-establish the session, then run the command again. No command was started.</source>
        <translation>De cache administrator autorisatie voor %1 verlopen of werd geweigerd.

Druk op Authorize op het tabblad Systems of Repair om de sessie te herstellen en voer het commando opnieuw uit. Er is geen commando gestart.</translation>
    </message>
    <message>
        <source>Privileged operation completed successfully. Administrator authorization remains active for this session.</source>
        <translation>Geprivilegieerde operatie succesvol voltooid. Administrator autorisatie blijft actief voor deze sessie.</translation>
    </message>
    <message>
        <source>Privileged operation stopped with an error. Administrator authorization remains active; review the output before closing.</source>
        <translation>Geprivilegieerde operatie stopte met een fout. Administrator autorisatie blijft actief; controleer de uitvoer voor het sluiten.</translation>
    </message>
    <message>
        <source>Inspection is read-only.

</source>
        <translation>Inspectie is alleen-lezen.

</translation>
    </message>
    <message>
        <source>Configuration unavailable</source>
        <translation>Instellingen zijn niet beschikbaar</translation>
    </message>
    <message>
        <source>The helper could not read %1:

%2</source>
        <translation>De helper kon %1 niet lezen:

%2</translation>
    </message>
    <message>
        <source>Configuration write failed</source>
        <translation>Configuratie schrijven mislukt</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
Result: mapping opened for this recovery session.</source>
        <translation>Status: ontgrendeld
Component: %1
Mapper: %2
Methode: helper ontgrendelen (cryptsetup; wachtwoordzin via een modus-600 keyfile, verwijderd na gebruik)
Resultaat: mapping geopend voor deze herstelsessie.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: the passphrase was not accepted; retry offered.</source>
        <translation>Status: vergrendeld
Component: %1
Methode: helper ontgrendelen (cryptsetup)
Fout: de wachtwoordzin werd niet geaccepteerd; opnieuw proberen aangeboden.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: %2</source>
        <translation>Status: vergrendeld
Component: %1
Methode: helper ontgrendelen (cryptsetup)
Fout: %2</translation>
    </message>
    <message>
        <source>Passphrase not accepted</source>
        <translation>Wachtwoordzin niet geaccepteerd</translation>
    </message>
    <message>
        <source>The LUKS passphrase was not accepted.

Try again?</source>
        <translation>De LUKS wachtwoordzin werd niet geaccepteerd.

Nog een keer?</translation>
    </message>
    <message>
        <source>[-] %1 - not run (the plan stopped before reaching this stage)</source>
        <translation>[-] %1 - niet uitgevoerd (het plan stopte voordat het deze fase bereikte)</translation>
    </message>
    <message>
        <source>filesystem</source>
        <translation>bestandssysteem</translation>
    </message>
    <message>
        <source>[-] %1 - no file system errors found - no changes</source>
        <translation>[-] %1 - geen bestandssysteemfouten gevonden - geen wijzigingen</translation>
    </message>
    <message>
        <source>[FAIL] %1 - not reported</source>
        <translation>[FAIL] %1 - niet gerapporteerd</translation>
    </message>
    <message>
        <source>%1 stage(s) failed; review the helper output in Logs.</source>
        <translation>%1 stage(s) is mislukt; bekijk de helper uitvoer in Logs.</translation>
    </message>
    <message>
        <source>the plan stopped before any stage completed.</source>
        <translation>het plan is gestopt voordat een stadium is voltooid.</translation>
    </message>
    <message>
        <source>no repair was needed; cached diagnostics remain valid.</source>
        <translation>geen reparatie nodig was; cache diagnostiek blijft geldig.</translation>
    </message>
    <message>
        <source>repair completed; cached diagnostics were invalidated and must be regenerated.</source>
        <translation>reparatie voltooid; gecachede diagnoses werden ongeldig gemaakt en moeten worden geregenereerd.</translation>
    </message>
    <message>
        <source>%1 results: [OK] %2 successful | [FAIL] %3 failed | [-] %4 no repair needed - %5</source>
        <translation>%1 resultaten: [OK] %2 succesvol [FAIL] %3 mislukt [-] %4 geen reparatie nodig - %5</translation>
    </message>
    <message>
        <source>Re-run Diagnostic</source>
        <translation>Diagnostisch opnieuw uitvoeren</translation>
    </message>
    <message>
        <source>Unavailable</source>
        <translation>Niet beschikbaar</translation>
    </message>
    <message>
        <source>Commit this physical drive as the repair target.</source>
        <translation>Zet deze fysieke aandrijving op als het reparatiedoel.</translation>
    </message>
    <message>
        <source>Protection:</source>
        <translation>Bescherming:</translation>
    </message>
    <message>
        <source>Host shell</source>
        <translation>Host shell</translation>
    </message>
    <message>
        <source>Run on Host</source>
        <translation>Uitvoeren op host</translation>
    </message>
    <message>
        <source>Host Shell</source>
        <translation>Host Shell</translation>
    </message>
    <message>
        <source>The read-only probe found no editable target configuration file in this target. %1</source>
        <translation>De alleen-lezen sonde vond geen bewerkbaar doel configuratiebestand in dit doel. %1</translation>
    </message>
    <message>
        <source>Not present in the selected target (omitted): %1</source>
        <translation>Niet aanwezig in het geselecteerde doel (toegewezen): %1</translation>
    </message>
    <message>
        <source>Run diagnostics to probe which target configuration files exist; the helper&apos;s read-only probe decides the list.</source>
        <translation>Voer diagnostiek uit om te onderzoeken welke doelconfiguratiebestanden bestaan; de alleen-lezen sonde van de helper bepaalt de lijst.</translation>
    </message>
    <message>
        <source>Probed read-only by the helper; a saved edit invalidates the cached diagnostics.</source>
        <translation>Probed read-only door de helper; een opgeslagen bewerking maakt de cache diagnostiek ongeldig.</translation>
    </message>
    <message>
        <source>Host maintenance: %1</source>
        <translation>Host onderhoud: %1</translation>
    </message>
    <message>
        <source>unresolved</source>
        <translation>onopgelost</translation>
    </message>
    <message>
        <source>Target: %1 + %2</source>
        <translation>Doel: %1 + %2</translation>
    </message>
    <message>
        <source>Host command</source>
        <translation>Hostopdracht</translation>
    </message>
    <message>
        <source>Execute a command on the running host as root.</source>
        <translation>Voer een commando uit op de lopende host als root.</translation>
    </message>
    <message>
        <source>Execute a command inside the selected repair system as root.</source>
        <translation>Voer een commando uit binnen het geselecteerde reparatiesysteem als root.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab (or re-enter Host Maintenance / re-commit the repair target) to authorize this session.</source>
        <translation>Administrator autorisatie is vereist; druk op Authorization on the Systems or Repair tab (or re-enter Host Maintenance / re-commit the repair target) om deze sessie te autoriseren.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root on the running host through the helper&apos;s guarded host-shell verb.</source>
        <translation>Voer een herzien commando uit als root op de lopende host via het bewaakte host-shell werkwoord van de helper.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root inside the target chroot through the helper&apos;s guarded shell verb.</source>
        <translation>Voer een herzien commando als root in de doel chroot door de helper&apos;s bewaakte shell werkwoord.</translation>
    </message>
    <message>
        <source>Run a command on the running host as root (sudo is not needed). Commands are executed directly on the active system; output is kept in this window and in the application log.</source>
        <translation>Voer een commando uit op de lopende host als root (sudo is niet nodig). Commando&apos;s worden direct uitgevoerd op het actieve systeem; de uitvoer wordt bewaard in dit venster en in het programmalogboek.</translation>
    </message>
    <message>
        <source>Run a command inside the selected repair system as root (sudo is not needed). Commands are executed one at a time in a fresh chroot and cannot answer interactive prompts; use non-interactive flags such as apt-get -y upgrade. Output is kept in this window and in the application log.</source>
        <translation>Voer een commando uit in het geselecteerde reparatiesysteem als root (sudo is niet nodig). Commando&apos;s worden een voor een uitgevoerd in een frisse chroot en kunnen geen interactieve prompts beantwoorden; gebruik niet-interactieve vlaggen zoals apt-get -y upgrade. Uitvoer wordt bewaard in dit venster en in het programmalogboek.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize or re-enter Host Maintenance / commit a repair target.</source>
        <translation>Geen beheerdersessie is actief; druk op Autoriseren of opnieuw invoeren Host Maintenance / commit een reparatiedoel.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from the repaired system</source>
        <translation>1. Selecteer bronbestanden of mappen van het gerepareerde systeem</translation>
    </message>
    <message>
        <source>2. Choose destination on this host</source>
        <translation>2. Kies bestemming op deze host</translation>
    </message>
    <message>
        <source>Add File Path...</source>
        <translation>Bestandspad toevoegen...</translation>
    </message>
    <message>
        <source>Add Folder Path...</source>
        <translation>Mappad toevoegen...</translation>
    </message>
    <message>
        <source>Browse...</source>
        <translation>Bladeren...</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it (Host Maintenance does not provide a repair tree to browse).</source>
        <translation>Selecteer de reparatie drive in Systems voordat u een bestemming erin kiest (Host Maintenance biedt geen reparatie boom om te bladeren).</translation>
    </message>
    <message>
        <source>Choose a host destination folder directly.</source>
        <translation>Kies direct een bestemmingsmap.</translation>
    </message>
    <message>
        <source>Copy the staged files and folders with cp -a, restore ownership with chown --reference and byte-compare every regular file afterwards.</source>
        <translation>Kopieer de geënsceneerde bestanden en mappen met cp -a, herstel eigendom met clown --reference en byte-vergelijk elk normaal bestand achteraf.</translation>
    </message>
    <message>
        <source>File Copy unavailable</source>
        <translation>Bestand kopiëren is niet beschikbaar</translation>
    </message>
    <message>
        <source>Choose host destination folder</source>
        <translation>Kies host doelmap</translation>
    </message>
    <message>
        <source>Select a repair target</source>
        <translation>Selecteer een reparatiedoel</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it.</source>
        <translation>Selecteer de reparatie drive in Systems voordat u een bestemming erin kiest.</translation>
    </message>
    <message>
        <source>File Copy - Browse Target Folders</source>
        <translation>Bestand kopiëren - Blader door doelmappen</translation>
    </message>
    <message>
        <source>Browse Target Folders</source>
        <translation>Doelmappen doorbladeren</translation>
    </message>
    <message>
        <source>The helper could not list the repair-system folder:

%1</source>
        <translation>De helper kon de map reparatie-systeem niet tonen:

%1</translation>
    </message>
    <message>
        <source>BROWSE_ENTRY</source>
        <translation>BROWSE ENTRY</translation>
    </message>
    <message>
        <source>(choose this folder: %1)</source>
        <translation>(Kies deze map: %1)</translation>
    </message>
    <message>
        <source>(parent folder)</source>
        <translation>(oudermap)</translation>
    </message>
    <message>
        <source>Select repair-system destination</source>
        <translation>Selecteer bestemming reparatiesysteem</translation>
    </message>
    <message>
        <source>Choose a destination folder inside the repaired system (current folder: %1):</source>
        <translation>Kies een doelmap in het gerepareerde systeem (huidige map: %1):</translation>
    </message>
    <message>
        <source>Folder</source>
        <translation>Map</translation>
    </message>
    <message>
        <source>Open</source>
        <translation>Open</translation>
    </message>
    <message>
        <source>(choose this folder:</source>
        <translation>(Kies deze map:</translation>
    </message>
    <message>
        <source>Add files to copy</source>
        <translation>Bestanden toevoegen om te kopiëren</translation>
    </message>
    <message>
        <source>Add folder to copy</source>
        <translation>Map toevoegen aan kopiëren</translation>
    </message>
    <message>
        <source>Stage at least one source and name a destination path first.</source>
        <translation>Stage ten minste één bron en noem eerst een bestemmingspad.</translation>
    </message>
    <message>
        <source>Copy the %1 staged item(s) to %2?

The helper keeps its direction and path containment checks; a sensitive repair-system destination is refused unless the helper approves it, and every regular file is byte-compared after the copy.</source>
        <translation>Het %1 geënsceneerde item[s] kopiëren naar %2?

De helper behoudt zijn richting en pad insluiting controles; een gevoelige repareren-systeem bestemming wordt geweigerd, tenzij de helper het goedkeurt, en elke reguliere bestand is byte-vergelijk na de kopie.</translation>
    </message>
    <message>
        <source>File Copy - Copy and Verify</source>
        <translation>Bestand kopiëren - kopiëren en verifiëren</translation>
    </message>
    <message>
        <source>File Copy - Preview Changes</source>
        <translation>Bestand kopiëren - voorbeeldwijzigingen</translation>
    </message>
    <message>
        <source>Protected running host - details</source>
        <translation>Beschermde host - details</translation>
    </message>
    <message>
        <source>not mounted (offline target)</source>
        <translation>niet gemonteerd (offline doel)</translation>
    </message>
    <message>
        <source>Select a drive to see its details.</source>
        <translation>Selecteer een schijf om de details te zien.</translation>
    </message>
    <message>
        <source>Inspecting the selected component.</source>
        <translation>Inspecteren van de geselecteerde component.</translation>
    </message>
    <message>
        <source>Helper-confirmed by the last read-only diagnostics.</source>
        <translation>Helper bevestigd door de laatste alleen-lezen diagnostiek.</translation>
    </message>
    <message>
        <source>Read-only inventory only; run diagnostics to confirm.</source>
        <translation>Alleen-lezen inventaris; diagnostiek uitvoeren om te bevestigen.</translation>
    </message>
    <message>
        <source>PROTECTED - running system; read-only details only</source>
        <translation>PROTECTED - running system; alleen-lezen details</translation>
    </message>
    <message>
        <source>Live / installer media - not selectable</source>
        <translation>Live / installatiemedia - niet selecteerbaar</translation>
    </message>
    <message>
        <source>PROTECTED - running host scope</source>
        <translation>BESCHERMDE - draaiende host scope</translation>
    </message>
    <message>
        <source>Unlock required before selection</source>
        <translation>Ontgrendelen vereist voor selectie</translation>
    </message>
    <message>
        <source>Eligible repair candidate</source>
        <translation>In aanmerking komende herstelkandidaat</translation>
    </message>
    <message>
        <source>Drive:</source>
        <translation>Schijf:</translation>
    </message>
    <message>
        <source>Detected target:</source>
        <translation>Gedetecteerd doel:</translation>
    </message>
    <message>
        <source>Pending inspection</source>
        <translation>In afwachting van de inspectie</translation>
    </message>
    <message>
        <source>Model / label:</source>
        <translation>Model / etiket:</translation>
    </message>
    <message>
        <source>Status:</source>
        <translation>Status:</translation>
    </message>
    <message>
        <source>Size:</source>
        <translation>Grootte:</translation>
    </message>
    <message>
        <source>Connection:</source>
        <translation>Verbinding:</translation>
    </message>
    <message>
        <source>Filesystem:</source>
        <translation>Bestandssysteem:</translation>
    </message>
    <message>
        <source>UUID:</source>
        <translation>UUID:</translation>
    </message>
    <message>
        <source>Mounts:</source>
        <translation>Mounts:</translation>
    </message>
    <message>
        <source>Running system protection unresolved</source>
        <translation>Bescherming van het werkende systeem niet opgelost</translation>
    </message>
    <message>
        <source>No protected physical backing disk was identified</source>
        <translation>Er is geen beschermde fysieke backing disk geïdentificeerd</translation>
    </message>
    <message>
        <source>Current running Linux system</source>
        <translation>Huidig Linux-systeem draait</translation>
    </message>
    <message>
        <source>Critical mounts: %1</source>
        <translation>Kritische mounts: %1</translation>
    </message>
    <message>
        <source>Read-only protected-running-system facts: the helper OS fact plus the inventory model, device path, size, transport and critical mounts. Nothing here is probed destructively.</source>
        <translation>Alleen-lezen beschermde-runn-systeem feiten: de helper OS feit plus het inventaris model, apparaat pad, grootte, transport en kritische mounts. Niets hier is destructief.</translation>
    </message>
    <message>
        <source>Select a drive to see unlock status.</source>
        <translation>Selecteer een station om de ontgrendelstatus te zien.</translation>
    </message>
    <message>
        <source>State: protected
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
The protected running host cannot be unlocked or modified; unlock is available for an offline repair target only. Use Host Maintenance for the protected running host.</source>
        <translation>Staat: beschermd
Component: %1
Mapper: (geen)
Methode: helper ontgrendelen (cryptsetup; wachtwoordzin via een modus-600 keyfile, verwijderd na gebruik)
De beschermde lopende host kan niet worden ontgrendeld of gewijzigd; ontgrendelen is alleen beschikbaar voor een offline reparatiedoel. Gebruik Host Maintenance voor de beschermde host.</translation>
    </message>
    <message>
        <source>(none detected)</source>
        <translation>(geen gedetecteerd)</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
A locked LUKS container is visible on this drive; press Unlock to open it for this recovery session.</source>
        <translation>Status: vergrendeld
Component: %1
Mapper: (geen)
Methode: helper ontgrendelen (cryptsetup; wachtwoordzin via een modus-600 keyfile, verwijderd na gebruik)
Een afgesloten LUKS container is zichtbaar op deze schijf; druk op Unlock om deze te openen voor deze herstelsessie.</translation>
    </message>
    <message>
        <source>(visible mapper)</source>
        <translation>(zichtbare mapper)</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: already open before this session (visible mapper; Boot Bitch will reuse it and will not close it).</source>
        <translation>Status: ontgrendeld
Component: %1
Mapper: %2
Methode: reeds geopend voor deze sessie (zichtbare mapper; Boot Bitch zal het hergebruiken en zal het niet sluiten).</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper-confirmed mapping from the last read-only diagnostics.</source>
        <translation>Status: ontgrendeld
Component: %1
Mapper: %2
Methode: helper-bevestigde mapping van de laatste alleen-lezen diagnostiek.</translation>
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
        <translation>Status: vergrendeld of geen gecodeerd onderdeel gedetecteerd
Onderdeel: (geen gedetecteerd)
Mapper: (geen)
Methode: helper ontgrendelen (cryptsetup; wachtwoordzin via een modus-600 keyfile, verwijderd na gebruik)
Geen vergrendeld LUKS-component en geen ontgrendeling werd opgenomen voor deze schijf in de huidige sessie.</translation>
    </message>
    <message>
        <source>unavailable - %1</source>
        <translation>niet beschikbaar - %1</translation>
    </message>
    <message>
        <source>Host maintenance:
%1</source>
        <translation>Onderhoud van de gastheer:
%1</translation>
    </message>
    <message>
        <source>Target:
%1 + %2</source>
        <translation>Doel:
%1 + %2</translation>
    </message>
    <message>
        <source>Authorization required: diagnostics and repairs fail closed until you press Authorize.</source>
        <translation>Autorisatie vereist: diagnostiek en reparaties falen totdat u op Autorisatie drukt.</translation>
    </message>
    <message>
        <source>Re-establish the privileged helper session for the current scope now. The password is requested in the hidden-input modal and is never logged.</source>
        <translation>Herstellen van de bevoorrechte helper sessie voor het huidige bereik nu. Het wachtwoord wordt gevraagd in de hidden-input modal en is nooit aangemeld.</translation>
    </message>
    <message>
        <source>Running...</source>
        <translation>Rennen...</translation>
    </message>
    <message>
        <source>Scope required</source>
        <translation>Toepassingsgebied</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope; this unlocks the gated actions.</source>
        <translation>Alles uitvoeren - voer alle beschikbare alleen-lezen kenmerkende voor de huidige scope; dit ontgrendelt de gated acties.</translation>
    </message>
    <message>
        <source>Select a target and wait for any running command first.</source>
        <translation>Selecteer een doel en wacht eerst op een uitgevoerd commando.</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic through the helper.</source>
        <translation>Start Diagnostisch - voer de geselecteerde alleen-lezen diagnose via de helper.</translation>
    </message>
    <message>
        <source>Run All for the current scope and refresh the read-only backend profile.</source>
        <translation>Alles uitvoeren voor de huidige scope en het alleen-lezen backend profiel vernieuwen.</translation>
    </message>
    <message>
        <source>Read or edit the selected target configuration file through the guarded helper; a saved edit invalidates cached diagnostics.</source>
        <translation>Lees of bewerk het geselecteerde doelconfiguratiebestand via de bewaakte helper; een opgeslagen bewerking maakt de opgeslagen diagnose ongeldig.</translation>
    </message>
    <message>
        <source>Host Maintenance has no target-file editing; commit an offline repair target first.</source>
        <translation>Host Maintenance heeft geen doel-bestand bewerken; commit eerst een offline reparatiedoel.</translation>
    </message>
    <message>
        <source>Commit an offline repair target first.</source>
        <translation>Leg eerst een offline reparatie doel vast.</translation>
    </message>
    <message>
        <source>No target configuration file is available for this target; run diagnostics to probe the list.</source>
        <translation>Er is geen doelconfiguratiebestand beschikbaar voor dit doel; voer diagnostiek uit om de lijst te onderzoeken.</translation>
    </message>
    <message>
        <source>Already Unlocked</source>
        <translation>Al ontgrendeld</translation>
    </message>
    <message>
        <source>An unlocked Linux filesystem is already visible on this drive. Boot Bitch will reuse the existing mapper and will not close or reopen a mapping created by this recovery session. Mounting happens during diagnostics (read-only) and repairs (read-write); data filesystems are never auto-mounted on selection.</source>
        <translation>Een ontgrendeld Linux bestandssysteem is al zichtbaar op dit station. Boot Bitch zal de bestaande mapper hergebruiken en zal een door deze herstelsessie aangemaakte mapping niet sluiten of heropenen. Montage gebeurt tijdens diagnostiek (alleen-lezen) en reparaties (lezen-schrijven); data bestandssystemen worden nooit automatisch gemonteerd op selectie.</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked; use Host Maintenance for the protected running host.</source>
        <translation>De beschermde host kan niet worden ontgrendeld; gebruik Host Maintenance voor de beschermde host.</translation>
    </message>
    <message>
        <source>Host Maintenance is the current scope, but the selected offline drive can still be unlocked.</source>
        <translation>Host Maintenance is de huidige scope, maar de geselecteerde offline drive kan nog steeds worden ontgrendeld.</translation>
    </message>
    <message>
        <source>Unlock %1 using cryptsetup through the privileged helper. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>Ontgrendel %1 met behulp van cryptsetup via de bevoorrechte helper. De wachtwoordzin reist door een privé-sleutelbestand en wordt nooit in commandoargumenten of logs geplaatst.</translation>
    </message>
    <message>
        <source>The protected running system cannot be selected as a repair target; use Host Maintenance for the protected running host.</source>
        <translation>Het beschermde hardloopsysteem kan niet als reparatiedoel worden geselecteerd; gebruik Host Maintenance voor de beschermde host.</translation>
    </message>
    <message>
        <source>Live / installer media is read-only boot media and cannot be selected as a repair target.</source>
        <translation>Live / installer media is alleen-lezen boot media en kan niet worden geselecteerd als een reparatie doel.</translation>
    </message>
    <message>
        <source>Unlock the encrypted volume first; Select Target becomes available after a Linux filesystem is detected.</source>
        <translation>Ontgrendel eerst het gecodeerde volume; Selecteer Target wordt beschikbaar nadat een Linux-bestandssysteem is gedetecteerd.</translation>
    </message>
    <message>
        <source>Committed repair target. Repair, Diagnostics and File Copy target this physical drive until another drive is explicitly selected with Select Target.</source>
        <translation>Doelwit voor reparatie. Repareren, Diagnostics en Bestand Kopiëren doel deze fysieke schijf totdat een andere schijf is expliciet geselecteerd met Selecteer Doel.</translation>
    </message>
    <message>
        <source>Commit %1 as the repair target; this leaves Host Maintenance and switches the scope to the selected drive.</source>
        <translation>Commit %1 als het reparatiedoel; dit laat Host Maintenance en schakelt de scope naar de geselecteerde schijf.</translation>
    </message>
    <message>
        <source>The running host target could not be detected.</source>
        <translation>Het lopende hostdoel kon niet worden gedetecteerd.</translation>
    </message>
    <message>
        <source>Leave Host Maintenance and return to repair-target mode.</source>
        <translation>Verlaat Host Onderhoud en keer terug naar reparatie-doelmodus.</translation>
    </message>
    <message>
        <source>Select the running host for deliberate guarded maintenance; the administrator authorization is requested here once and cached for the session.</source>
        <translation>Selecteer de lopende host voor doelbewust bewaakt onderhoud; de beheerder toestemming wordt hier eenmaal gevraagd en gecached voor de sessie.</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a repair target.</source>
        <translation>De lopende host doel kon niet worden gedetecteerd; diagnostiek moet een reparatie doel.</translation>
    </message>
    <message>
        <source>Committed target: %1 + %2</source>
        <translation>Toegewezen doel: %1 + %2</translation>
    </message>
    <message>
        <source>Committed target: none (selection changed)</source>
        <translation>Toegewezen streefcijfer: geen (selectie gewijzigd)</translation>
    </message>
    <message>
        <source>The selected scope has no resolved Linux root component; Refresh Devices and commit the repair target again.</source>
        <translation>De geselecteerde scope heeft geen opgelost Linux root component; Vernieuw apparaten en commit het reparatiedoel opnieuw.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize on the Systems or Repair tab to re-establish it. Run All reuses the cached authorization and never prompts by itself.</source>
        <translation>Er is geen administratorsessie actief; druk op Authenticeren op het tabblad Systemen of Repareren om het te herstellen. Run All gebruikt de gecachede autorisatie en vraagt nooit zelf.</translation>
    </message>
    <message>
        <source>Run diagnostics for this scope to unlock the gated actions. Diagnostics are read-only and the only evidence source.</source>
        <translation>Doe diagnostiek voor deze scope om de gesloten acties te ontgrendelen. Diagnostics zijn alleen-lezen en de enige bewijsbron.</translation>
    </message>
    <message>
        <source>A repair was not proven unchanged, so the cached diagnostics are invalidated. Run diagnostics again before another gated action.</source>
        <translation>Een reparatie werd niet onveranderd bewezen, dus de cache diagnostiek is ongeldig. Voer de diagnostiek opnieuw uit voordat er weer iets gebeurt.</translation>
    </message>
    <message>
        <source>Gated actions reflect the cached capability lines; the helper still runs every runtime preflight when a command starts.</source>
        <translation>Gated acties weerspiegelen de gecachede vermogenslijnen; de helper draait nog steeds elke runtime preflight wanneer een commando start.</translation>
    </message>
    <message>
        <source>Regenerating diagnostics automatically</source>
        <translation>Regenererende diagnostiek automatisch</translation>
    </message>
    <message>
        <source>Running diagnostic: %1</source>
        <translation>Kenmerkend: %1</translation>
    </message>
    <message>
        <source>Running all diagnostics</source>
        <translation>Alle diagnostiek uitvoeren</translation>
    </message>
    <message>
        <source>Unlocking %1</source>
        <translation>%1 wordt geopend</translation>
    </message>
    <message>
        <source>running host</source>
        <translation>draaiende host</translation>
    </message>
    <message>
        <source>offline target</source>
        <translation>offline doel</translation>
    </message>
    <message>
        <source>host maintenance active</source>
        <translation>host onderhoud actief</translation>
    </message>
    <message>
        <source>repair target committed</source>
        <translation>Repareren doel vastgelegd</translation>
    </message>
    <message>
        <source>no committed scope</source>
        <translation>geen vastgelegd toepassingsgebied</translation>
    </message>
    <message>
        <source>Current session</source>
        <translation>Huidige zitting</translation>
    </message>
    <message>
        <source>Log files (*.log);;All files (*)</source>
        <translation>Logbestanden (*.log);;Alle bestanden (*)</translation>
    </message>
    <message>
        <source>Save log as</source>
        <translation>Log opslaan als</translation>
    </message>
    <message>
        <source>About Boot Bitch</source>
        <translation>Over Boot Bitch</translation>
    </message>
    <message>
        <source>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;&lt;p&gt;&lt;b&gt;Developer:&lt;/b&gt; CaptainMorgan12&lt;/p&gt;&lt;p&gt;This GUI is the package&apos;s entry point: a Qt 3.3.x frontend with the ported guarded helper for Debian Etch-era systems.&lt;/p&gt;&lt;p&gt;&lt;b&gt;Guarded repair mode:&lt;/b&gt; read-only diagnostics can inspect either the protected Running Host or an explicitly selected repair drive. The guarded Debian/APT package stages (interrupted configuration, broken dependencies, metadata refresh, upgrade), GRUB-legacy configuration regeneration, LUKS target unlock and guarded target-file editing run through the ported helper after confirmation; Host Maintenance enables the same supported stages natively on the active system after repeating the host identity and boot-mount checks.&lt;/p&gt;&lt;p&gt;The modern-only features - verified File Copy, Btrfs snapshot rollback, EFI/UKI and extlinux repair, boot-stack reconciliation and Make Default - are greyed with the helper&apos;s own probe reasons on this frontend; Arch/Alpine/Fedora package backends stay diagnostics-only here.&lt;/p&gt;&lt;p&gt;The first privileged action authorizes one cached administrator session per scope through a hidden-input modal (the Qt GUI remains unprivileged). It can be ended at any time from File - Lock Administrator Session.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;&lt;p&gt;&lt;b&gt;Ontwikkelaar:&lt;/b&gt; CaptainMorgan12&lt;/p&gt;&lt;p&gt; Deze GUI is het ingangspunt van het pakket: een Qt 3.3.x frontend met de geporteerde bewaakte helper voor Debian Etch-era-systemen.&lt;/p&gt;&lt;p&gt;&lt;b&gt;Bewaakte reparatiemodus:&lt;/b&gt; alleen-lezen diagnostica kan ofwel de beschermde Running Host inspecteren, ofwel een expliciet geselecteerde reparatieschijf. De bewaakte Debian/APT-pakketfasen (gestoorde configuratie, gebroken afhankelijkheden, metadata vernieuwen, upgraden), GRUB-legacy configuratieregeneratie, LUKS-doel ontgrendelen en bewaakte doelbestandbewerking uitvoeren door de geporteerde helper na bevestiging; Host Maintenance maakt dezelfde ondersteunde stadia in het actieve systeem na het herhalen van de host-identiteit en boot-mount controles.&lt;/p&gt;&lt;p&gt; De moderne-alleen-functies - geverifieerde bestandskopie, Btrfs snapshot rollback, EFI/UKI en extlinux reparatie, boot-stack verzoening en Make Default - zijn grijs met de helper eigen sonde redenen op deze frontend; Arch/Alpine/Fedora pakket backends blijven diagnostiek-alleen hier. &lt;/p&gt;&lt;p&gt; De eerste geprivilegieerde actie geeft toestemming voor een gecachede administratorsessie per scope via een verborgen ingangsmodus (de Qt GUI blijft onbevoorrecht). Het kan op elk moment worden beëindigd vanuit Bestand - Administrator-sessie vergrendelen.&lt;/p&gt;</translation>
    </message>
</context>
</TS>
