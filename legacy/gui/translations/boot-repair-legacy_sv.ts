<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS language="sv">
<context>
    <name>LegacyMainWindow</name>
    <message>
        <source>Validate environment</source>
        <translation>Validera miljö</translation>
    </message>
    <message>
        <source>Validate</source>
        <translation>Validera</translation>
    </message>
    <message>
        <source>Check the selected system&apos;s mounts, filesystem metadata, boot files, mapper consistency and dependency readiness before any repair action. This is an independent safety preflight rather than an optional Full Repair stage.</source>
        <translation>Kontrollera det valda systemets fästen, filsystemmetadata, startfiler, mapper konsistens och beroendeberedskap innan någon reparation åtgärd. Detta är en oberoende säkerhetspreflight snarare än en valfri Full Repair-fas.</translation>
    </message>
    <message>
        <source>Always preflight</source>
        <translation>Alltid preflight</translation>
    </message>
    <message>
        <source>File system repair</source>
        <translation>Filsystem reparation</translation>
    </message>
    <message>
        <source>Check File Systems</source>
        <translation>Kontrollera filsystem</translation>
    </message>
    <message>
        <source>Run the read-only file system check for the selected system&apos;s root and /boot filesystems and report each device&apos;s check tool and result without changing anything. This legacy frontend exposes the read-only check only; device repair is not wired.</source>
        <translation>Kör det lätta filsystemet kontroll för det valda systemets rot och / start filsystem och rapportera varje enhets kontrollverktyg och resultat utan att ändra någonting. Detta arvsfrontend avslöjar endast den lätta kontrollen; enhetsreparation är inte trådbunden.</translation>
    </message>
    <message>
        <source>Full Repair plan: unavailable on this frontend - read-only check only</source>
        <translation>Full Repair plan: otillgänglig på denna frontend - kontrollera endast</translation>
    </message>
    <message>
        <source>Complete package configuration</source>
        <translation>Komplett paketkonfiguration</translation>
    </message>
    <message>
        <source>Complete Configuration</source>
        <translation>Fullständig konfiguration</translation>
    </message>
    <message>
        <source>Complete interrupted dpkg package configuration in the selected repair system. This is the same stage controlled by Settings -&gt; Full Repair plan -&gt; Complete interrupted package configuration, but it can also be run independently here.</source>
        <translation>Fullständig avbruten dpkg-paketkonfiguration i det valda reparationssystemet. Detta är samma stadium som styrs av Inställningar -&gt; Full Repair plan -&gt; Fullständig avbruten paketkonfiguration, men den kan också köras oberoende här.</translation>
    </message>
    <message>
        <source>Run the guarded dpkg-configure repair? The helper keeps its package-lock and runtime preflights.</source>
        <translation>Kör den skyddade dpkg-konfigurationsreparationen? Hjälparen håller sina paket-lock och runtime preflights.</translation>
    </message>
    <message>
        <source>Repair broken dependencies</source>
        <translation>Reparera brutna beroenden</translation>
    </message>
    <message>
        <source>Repair Dependencies</source>
        <translation>Reparationsberoende</translation>
    </message>
    <message>
        <source>Repair package dependencies in the selected repair system after the mandatory safety preflight. This maps directly to Settings -&gt; Repair broken package dependencies.</source>
        <translation>Reparationspaketberoende i det valda reparationssystemet efter obligatorisk säkerhetspreflight. Detta kartlägger direkt till Inställningar -&gt; Reparera brutna paketberoende.</translation>
    </message>
    <message>
        <source>Run the guarded fix-broken repair? The helper keeps its simulation-first preflight and runtime guards.</source>
        <translation>Kör den bevakade fix-broken reparation? Hjälparen håller sin simulerings-första preflight och runtime vakter.</translation>
    </message>
    <message>
        <source>Refresh package metadata</source>
        <translation>Refresh paket metadata</translation>
    </message>
    <message>
        <source>Refresh Metadata</source>
        <translation>Refresh Metadata</translation>
    </message>
    <message>
        <source>Refresh APT metadata in the selected repair system without upgrading installed packages. This maps directly to Settings -&gt; Refresh package metadata.</source>
        <translation>Uppdatera APT metadata i det valda reparationssystemet utan att uppgradera installerade paket. Detta kartlägger direkt till Inställningar -&gt; Refresh paketmetadata.</translation>
    </message>
    <message>
        <source>Refresh package metadata for the selected scope? The helper requires a reachable, trusted APT source.</source>
        <translation>Refresh paketmetadata för det valda omfattningen? Hjälparen kräver en nåbar, pålitlig APT-källa.</translation>
    </message>
    <message>
        <source>Upgrade installed packages</source>
        <translation>Uppgradera installerade paket</translation>
    </message>
    <message>
        <source>Simulate and Upgrade</source>
        <translation>Simulera och uppgradera</translation>
    </message>
    <message>
        <source>Simulate the APT transaction first, inspect proposed removals, then apply a safe upgrade. This maps directly to Settings -&gt; Upgrade installed packages.</source>
        <translation>Simulera APT-transaktionen först, inspektera föreslagna borttagningar, sedan tillämpa en säker uppgradering. Detta kartlägger direkt till Inställningar -&gt; Uppgradera installerade paket.</translation>
    </message>
    <message>
        <source>Run the guarded apt-upgrade transaction? The helper keeps its simulation-first and source guards.</source>
        <translation>Kör den bevakade apt-upgrade transaktionen? Hjälparen håller sina simulerings-första och källskydd.</translation>
    </message>
    <message>
        <source>DKMS</source>
        <translation>DKMS</translation>
    </message>
    <message>
        <source>Rebuild DKMS</source>
        <translation>Ombyggd DKMS</translation>
    </message>
    <message>
        <source>Rebuild out-of-tree kernel modules for kernels installed in the selected system. The helper refuses this action when DKMS is not installed; this legacy frontend exposes no DKMS action.</source>
        <translation>Ombygga out-of-tree kärnmoduler för kärnor installerade i det valda systemet. Hjälparen vägrar denna åtgärd när DKMS inte är installerat; denna arvsfrontend avslöjar ingen DKMS-åtgärd.</translation>
    </message>
    <message>
        <source>Graphical login / display manager</source>
        <translation>Grafisk inloggning / display manager</translation>
    </message>
    <message>
        <source>Restore Graphical Login</source>
        <translation>Återställ grafisk inloggning</translation>
    </message>
    <message>
        <source>Restore the legacy SysV display manager configured for the running host: the /etc/X11/default-display-manager entry and the missing runlevel S-symlink, with a backup and rollback, never starting the GUI. This is a host-scope stage on this legacy frontend.</source>
        <translation>Återställ arvet SysV display manager konfigurerad för den löpande värden: /etc / X11 / standard-display-manager inträde och den saknade runlevel S-symlink, med en backup och rollback, aldrig starta GUI. Detta är ett host-scope-steg på denna arvsfrontend.</translation>
    </message>
    <message>
        <source>Restore the graphical login configuration for the running host? The helper backs up /etc/X11/default-display-manager and the runlevel symlink state, restores the configured entry and the missing S-symlink, rolls back on any failure, and never starts the display manager.</source>
        <translation>Återställ grafisk inloggningskonfiguration för den löpande värden? Hjälparen backar upp /etc/X11/default-display-manager och runlevel symlink state, återställer den konfigurerade posten och den saknade S-symlink, rullar tillbaka på något misslyckande och startar aldrig displayhanteraren.</translation>
    </message>
    <message>
        <source>Initramfs</source>
        <translation>Initramfs</translation>
    </message>
    <message>
        <source>Rebuild Initramfs</source>
        <translation>Rebuild Initramfs</translation>
    </message>
    <message>
        <source>Rebuild initramfs images for the selected repair system only after mapper and crypttab consistency checks pass. The helper backs up each image before the apply. On Etch the stage runs through the guarded plain-chroot fallback (no unshare required).</source>
        <translation>Ombygga initramfs-bilder för det valda reparationssystemet först efter kartläggning och crypttab konsistenskontroller passerar. Hjälparen backar upp varje bild innan appliceringen. På Etch går scenen genom den skyddade slätten fallback (ingen odelning krävs).</translation>
    </message>
    <message>
        <source>Rebuild the initramfs for the selected scope? The helper keeps its mapper/crypttab and backup preflights.</source>
        <translation>Bygg om initramfs för vald räckvidd? Hjälparen håller sin mapper/crypttab och backup preflights.</translation>
    </message>
    <message>
        <source>EFI / UKI bootloader</source>
        <translation>EFI / UKI bootloader</translation>
    </message>
    <message>
        <source>Repair EFI / UKI</source>
        <translation>Reparation EFI / UKI</translation>
    </message>
    <message>
        <source>Repair the selected system&apos;s EFI / UKI boot path. This legacy frontend exposes no EFI action; the Etch target is a BIOS/GRUB-legacy system.</source>
        <translation>Reparera det valda systemets EFI / UKI startväg. Denna arvsfront exponerar ingen EFI-åtgärd; Etch-målet är ett BIOS/GRUB-legacy-system.</translation>
    </message>
    <message>
        <source>GRUB configuration</source>
        <translation>GRUB konfiguration</translation>
    </message>
    <message>
        <source>Regenerate GRUB</source>
        <translation>Regenerera GRUB</translation>
    </message>
    <message>
        <source>Regenerate the selected repair system&apos;s GRUB menu/configuration after the mandatory safety preflight. The helper backs up menu.lst, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Regenerera det valda reparationssystemets GRUB-meny/konfiguration efter den obligatoriska säkerhetspreflighten. Hjälparen backar upp meny.lst, bevarar varje befintlig start och rullar tillbaka på eventuella misslyckanden.</translation>
    </message>
    <message>
        <source>Regenerate the GRUB configuration? The helper backs up the target menu/configuration, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Regenerera konfigurationen GRUB? Hjälparen backar upp målmenyn/konfigurationen, bevarar varje befintlig start och rullar tillbaka på eventuella fel.</translation>
    </message>
    <message>
        <source>extlinux configuration</source>
        <translation>extlinux konfiguration</translation>
    </message>
    <message>
        <source>Regenerate extlinux</source>
        <translation>Regenerera extlinux</translation>
    </message>
    <message>
        <source>Regenerate the selected system&apos;s extlinux bootloader configuration. This legacy frontend exposes no extlinux action.</source>
        <translation>Regenerera det valda systemets extlinux bootloader konfiguration. Denna arvsfront avslöjar ingen extlinux-åtgärd.</translation>
    </message>
    <message>
        <source>Boot stack reconciliation</source>
        <translation>Boot Stack försoning</translation>
    </message>
    <message>
        <source>Reconcile Boot Stack</source>
        <translation>Reconcile Boot Stack</translation>
    </message>
    <message>
        <source>Reconcile the selected repair system&apos;s boot stack in one guarded legacy pass: mapper/crypttab validation, initramfs rebuild and GRUB-legacy configuration regeneration, with the component backups and preflights unchanged. This is the Etch equivalent of the modern boot-stack reconciliation and stays out of the Full Repair plan.</source>
        <translation>Förena det valda reparationssystemets startstack i ett bevakat arvspass: mapper / kryptab validering, initramfs ombyggnad och GRUB-legacy konfiguration regenerering, med komponentbackups och preflights oförändrade. Detta är Etch motsvarigheten till den moderna boot-stack försoning och stannar utanför Full Repair planen.</translation>
    </message>
    <message>
        <source>Full Repair plan: Manual recovery tool</source>
        <translation>Full reparationsplan: Manuell återställningsverktyg</translation>
    </message>
    <message>
        <source>Run the guarded boot-stack reconciliation? The helper runs the mapper/crypttab validation, the initramfs rebuild and the GRUB-legacy regeneration in one pass with every component preflight and backup.</source>
        <translation>Kör den skyddade boot-stack försoning? Hjälparen kör mapper / kryptab validering, initramfs ombyggnad och GRUB-legacy regenerering i ett pass med varje komponent preflight och backup.</translation>
    </message>
    <message>
        <source>Complete interrupted package configuration</source>
        <translation>Fullständig avbruten paketkonfiguration</translation>
    </message>
    <message>
        <source>Repair broken package dependencies</source>
        <translation>Reparera brutna paketberoende</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Gör standard</translation>
    </message>
    <message>
        <source>Repair file system errors (read-only check first)</source>
        <translation>Reparera filsystemfel (läs endast kontrollera först)</translation>
    </message>
    <message>
        <source>the legacy frontend exposes the read-only file system check only; per-device repair is not wired on this frontend (fail closed)</source>
        <translation>arvsfronten avslöjar endast det lätta filsystemet kontroll; per-enhet reparation är inte trådbunden på denna frontend (svik stängd)</translation>
    </message>
    <message>
        <source>Upgrade installed packages (adaptive APT simulation)</source>
        <translation>Uppgradera installerade paket (adaptiv APT-simulering)</translation>
    </message>
    <message>
        <source>Rebuild DKMS modules</source>
        <translation>Ombygga DKMS-moduler</translation>
    </message>
    <message>
        <source>Restore graphical login manager</source>
        <translation>Återställ grafisk inloggningshanterare</translation>
    </message>
    <message>
        <source>Rebuild initramfs after mapper/crypttab validation</source>
        <translation>Rebuild initramfs efter mapper/crypttab validering</translation>
    </message>
    <message>
        <source>Repair EFI / UKI boot path</source>
        <translation>Reparera EFI / UKI startväg</translation>
    </message>
    <message>
        <source>Update GRUB configuration</source>
        <translation>Uppdatera GRUB konfiguration</translation>
    </message>
    <message>
        <source>Update extlinux configuration</source>
        <translation>Uppdatera extlinux konfiguration</translation>
    </message>
    <message>
        <source>Choose Full Repair stages in Settings. Enabled stages run in the order shown. Each configurable stage also appears below as an individual tool; the Full Repair column mirrors its current Settings state. The boot tools (EFI / UKI bootloader, GRUB or extlinux configuration, boot-stack reconciliation and Make Default) are independent: run them in any order, and a later action re-verifies what an earlier one changed and reports its own result. The active scope is shown beside Repair: selected repair drive or Running Host maintenance.</source>
        <translation>Välj Full Repair-steg i Inställningar. Aktiverade stadier körs i den ordning som visas. Varje konfigurerbart stadium visas också nedan som ett individuellt verktyg; Full Repair-kolumnen speglar dess nuvarande inställningar. Boot-verktygen (EFI / UKI bootloader, GRUB eller extlinux konfiguration, boot-stack försoning och Make Default) är oberoende: kör dem i någon ordning, och en senare åtgärd verifierar vad en tidigare ändrade och rapporterar sitt eget resultat. Den aktiva omfattningen visas bredvid Repair: vald reparationsenhet eller Running Host-underhåll.</translation>
    </message>
    <message>
        <source>Run All diagnostics for the selected target or running host before starting Full Repair. The report is read-only evidence used to choose and confirm repair stages.</source>
        <translation>Kör alla diagnostik för det valda målet eller körvärden innan du startar Full Repair. Rapporten är lättbevis som används för att välja och bekräfta reparationsstadier.</translation>
    </message>
    <message>
        <source>Ready: required cached read-only diagnostics are available for the selected stages. Review them in Diagnostics or Logs before confirming.</source>
        <translation>Klar: krävs cachad lätt diagnostik är tillgängliga för de valda stadierna. Granska dem i diagnostik eller loggar innan du bekräftar.</translation>
    </message>
    <message>
        <source>Environment validation</source>
        <translation>Miljö validering</translation>
    </message>
    <message>
        <source>Summarizes the selected system, protection state, mounted identity and inspection readiness.</source>
        <translation>Sammanfattar det valda systemet, skyddstillståndet, monterad identitet och inspektionsberedskap.</translation>
    </message>
    <message>
        <source>Distribution and boot backend profile</source>
        <translation>Distribution och boot backend profile</translation>
    </message>
    <message>
        <source>Identifies the distribution family, package manager, initramfs generator, bootloader and current guarded repair capability.</source>
        <translation>Identifierar distributionsfamiljen, pakethanteraren, initramfs generator, bootloader och nuvarande bevakad reparationskapacitet.</translation>
    </message>
    <message>
        <source>Boot diagnostics</source>
        <translation>Boot diagnostics</translation>
    </message>
    <message>
        <source>Shows boot mounts and /boot contents plus storage evidence without changing the selected system.</source>
        <translation>Visar startmonteringar och / startinnehåll plus lagringsbevis utan att ändra det valda systemet.</translation>
    </message>
    <message>
        <source>Boot evidence and selection history</source>
        <translation>Boot bevis och urvalshistoria</translation>
    </message>
    <message>
        <source>Correlates the detected boot chain, bootloader selection, kernel/initramfs and unlock evidence.</source>
        <translation>Korrelerar den upptäckta startkedjan, bootloader urval, kärna / initramfs och låsa upp bevis.</translation>
    </message>
    <message>
        <source>Kernel / initramfs</source>
        <translation>Kernel / initramfs</translation>
    </message>
    <message>
        <source>Reviews kernel files and verifies matching initramfs images through a read-only inspection.</source>
        <translation>Recensioner kärnfiler och verifierar matchande initramfs bilder genom en lätt inspektion.</translation>
    </message>
    <message>
        <source>Reviews the GRUB configuration without changing boot files.</source>
        <translation>Recensioner GRUB-konfigurationen utan att ändra startfiler.</translation>
    </message>
    <message>
        <source>EFI / UKI boot state</source>
        <translation>EFI / UKI boot state</translation>
    </message>
    <message>
        <source>Inspects EFI/UKI evidence; unavailable on this legacy BIOS frontend with the helper&apos;s probe reason.</source>
        <translation>Inspekterar EFI / UKI bevis; otillgänglig på detta arv BIOS frontend med hjälparens sond skäl.</translation>
    </message>
    <message>
        <source>Reviews the configured display manager and recent boot evidence without starting the GUI.</source>
        <translation>Recensioner den konfigurerade displayhanteraren och senaste startbevis utan att starta GUI.</translation>
    </message>
    <message>
        <source>Boot errors</source>
        <translation>Boot fel</translation>
    </message>
    <message>
        <source>Reads recent error-priority entries from the running host or selected repair system when available.</source>
        <translation>Läser nyligen felprioriterade poster från den löpande värden eller valda reparationssystemet när det är tillgängligt.</translation>
    </message>
    <message>
        <source>Disk usage</source>
        <translation>Diskanvändning</translation>
    </message>
    <message>
        <source>Summarizes filesystem capacity and free space for the running host or read-only repair target.</source>
        <translation>Sammanfattar filsystemkapacitet och ledigt utrymme för den löpande värden eller läs-bara reparationsmålet.</translation>
    </message>
    <message>
        <source>File systems</source>
        <translation>Filsystem</translation>
    </message>
    <message>
        <source>Runs the read-only file system check for the selected system&apos;s root, /boot and other filesystems.</source>
        <translation>Kör det lätta filsystemet kontroll för det valda systemets rot, / start och andra filsystem.</translation>
    </message>
    <message>
        <source>/etc/fstab review</source>
        <translation>/etc/fstab review</translation>
    </message>
    <message>
        <source>Displays the running host or selected repair system&apos;s fstab; repair-system inspection is mounted read-only.</source>
        <translation>Visar den körvärd eller valda reparationssystemets fstab; reparationssystemets inspektion monteras endast.</translation>
    </message>
    <message>
        <source>Btrfs status</source>
        <translation>Btrfs status</translation>
    </message>
    <message>
        <source>Shows Btrfs filesystem and subvolume information when the target uses Btrfs.</source>
        <translation>Visar Btrfs filsystem och subvolume information när målet använder Btrfs.</translation>
    </message>
    <message>
        <source>Device-mapper ancestry</source>
        <translation>Device-mapper förfäder</translation>
    </message>
    <message>
        <source>Shows selected mapper ancestry and device-mapper state when available.</source>
        <translation>Visar vald mapper anor och enhet-mapper tillstånd när det är tillgängligt.</translation>
    </message>
    <message>
        <source>LUKS / crypttab evidence</source>
        <translation>LUKS / kryptab bevis</translation>
    </message>
    <message>
        <source>Shows LUKS/mapped ancestry plus crypttab and fstab mapper references.</source>
        <translation>Visar LUKS/mappade anor plus crypttab och fstab mapper referenser.</translation>
    </message>
    <message>
        <source>Full diagnostic report</source>
        <translation>Full diagnostisk rapport</translation>
    </message>
    <message>
        <source>Combines all read-only diagnostics for the selected scope (same as Run All).</source>
        <translation>Kombinerar alla lättlästa diagnostik för det valda omfattningen (samma som Run All).</translation>
    </message>
    <message>
        <source>All entries</source>
        <translation>Alla inlägg</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>Diagnostik</translation>
    </message>
    <message>
        <source>Repairs</source>
        <translation>Reparationer</translation>
    </message>
    <message>
        <source>Package repair</source>
        <translation>Paket reparation</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>File copy</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Device discovery</translation>
    </message>
    <message>
        <source>Host</source>
        <translation>Värd</translation>
    </message>
    <message>
        <source>Required for block-device inventory</source>
        <translation>Krävs för block-device inventory</translation>
    </message>
    <message>
        <source>Filesystem identification</source>
        <translation>Filsystemidentifiering</translation>
    </message>
    <message>
        <source>Used to identify filesystem metadata</source>
        <translation>Används för att identifiera filsystem metadata</translation>
    </message>
    <message>
        <source>Mount inspection</source>
        <translation>Mount inspektion</translation>
    </message>
    <message>
        <source>Used to understand active mounts</source>
        <translation>Används för att förstå aktiva fästen</translation>
    </message>
    <message>
        <source>LUKS support</source>
        <translation>LUKS stöd</translation>
    </message>
    <message>
        <source>Required to unlock encrypted targets</source>
        <translation>Krävs för att låsa upp krypterade mål</translation>
    </message>
    <message>
        <source>Btrfs support</source>
        <translation>Btrfs stöd</translation>
    </message>
    <message>
        <source>Required for Btrfs inspection and snapshot rollback</source>
        <translation>Krävs för Btrfs inspektion och snapshot rollback</translation>
    </message>
    <message>
        <source>Bidirectional file copy</source>
        <translation>Bidirectional file copy</translation>
    </message>
    <message>
        <source>Host/Repair</source>
        <translation>Värd/Repair</translation>
    </message>
    <message>
        <source>Required for verified Host-to-Repair and Repair-to-Host transfer</source>
        <translation>Krävs för verifierad Host-to-Repair och Repair-to-Host-överföring</translation>
    </message>
    <message>
        <source>Chroot repair</source>
        <translation>Chroot reparation</translation>
    </message>
    <message>
        <source>Required for target-side repair commands</source>
        <translation>Krävs för mål-side reparation kommandon</translation>
    </message>
    <message>
        <source>Offline systemd repair</source>
        <translation>Offline systemd reparation</translation>
    </message>
    <message>
        <source>Used to restore graphical.target and the configured display manager without starting the target GUI</source>
        <translation>Används för att återställa grafisk. mål och den konfigurerade displayhanteraren utan att starta målet GUI</translation>
    </message>
    <message>
        <source>UEFI NVRAM inspection</source>
        <translation>UEFI NVRAM kontroll</translation>
    </message>
    <message>
        <source>Used to preserve target EFI BootOrder during TUXEDO UKI rebuilds when efivars are available</source>
        <translation>Används för att bevara målet EFI BootOrder under TUXEDO UKI ombyggnader när efivars finns tillgängliga</translation>
    </message>
    <message>
        <source>UKI verification</source>
        <translation>UKI verifiering</translation>
    </message>
    <message>
        <source>Used to verify the kernel embedded in a rebuilt unified kernel image</source>
        <translation>Används för att verifiera kärnan inbäddad i en ombyggd enhetsbild</translation>
    </message>
    <message>
        <source>GRUB EFI repair</source>
        <translation>GRUB EFI reparation</translation>
    </message>
    <message>
        <source>Target/Host</source>
        <translation>Mål/Host</translation>
    </message>
    <message>
        <source>Required only for conventional GRUB-based EFI systems</source>
        <translation>Krävs endast för konventionella GRUB-baserade EFI-system</translation>
    </message>
    <message>
        <source>Target</source>
        <translation>Mål</translation>
    </message>
    <message>
        <source>Debian-family GRUB helper</source>
        <translation>Debian-familjen GRUB hjälpare</translation>
    </message>
    <message>
        <source>Portable GRUB configuration generator used by Arch and other non-Debian systems</source>
        <translation>Portable GRUB konfigurationsgenerator som används av Arch och andra icke-Debiansystem</translation>
    </message>
    <message>
        <source>Initramfs rebuild</source>
        <translation>Initramfs återuppbyggs</translation>
    </message>
    <message>
        <source>Debian-family initramfs helper</source>
        <translation>Debian-familjen initramfs hjälpare</translation>
    </message>
    <message>
        <source>Arch-family initramfs generator</source>
        <translation>Arch-family initramfs generator</translation>
    </message>
    <message>
        <source>Alternative initramfs generator used by Arch and other distributions</source>
        <translation>Alternativ initramfs-generator som används av Arch och andra distributioner</translation>
    </message>
    <message>
        <source>Initramfs verification</source>
        <translation>Initramfs verifiering</translation>
    </message>
    <message>
        <source>Read-only verification for mkinitcpio images</source>
        <translation>Läs-bara verifiering för mkinitcpio bilder</translation>
    </message>
    <message>
        <source>Read-only verification for dracut images</source>
        <translation>Läs-bara verifiering för dracut bilder</translation>
    </message>
    <message>
        <source>systemd-boot inspection</source>
        <translation>systemd-boot inspektion</translation>
    </message>
    <message>
        <source>Host/Target</source>
        <translation>Värd/mål</translation>
    </message>
    <message>
        <source>Read-only inspection of systemd-boot and generic UKI layouts</source>
        <translation>Läs-bara inspektion av systemd-boot och generiska UKI layouter</translation>
    </message>
    <message>
        <source>Arch package manager</source>
        <translation>Arch Pack Manager</translation>
    </message>
    <message>
        <source>Arch-family package database and transaction tool</source>
        <translation>Arch-family paketdatabas och transaktionsverktyg</translation>
    </message>
    <message>
        <source>DKMS rebuild</source>
        <translation>DKMS byggs om</translation>
    </message>
    <message>
        <source>Required only when target uses DKMS modules</source>
        <translation>Krävs endast när målet använder DKMS-moduler</translation>
    </message>
    <message>
        <source>LVM inspection</source>
        <translation>LVM inspektion</translation>
    </message>
    <message>
        <source>Optional LVM storage-stack support</source>
        <translation>Optionell LVM lagringsstöd</translation>
    </message>
    <message>
        <source>Software RAID</source>
        <translation>Programvara RAID</translation>
    </message>
    <message>
        <source>Optional Linux MD RAID support</source>
        <translation>Valfri Linux MD RAID stöd</translation>
    </message>
    <message>
        <source>Process namespace isolation</source>
        <translation>Process namespace isolering</translation>
    </message>
    <message>
        <source>Host+Target</source>
        <translation>Värd+ Mål</translation>
    </message>
    <message>
        <source>The ported helper falls back to a guarded plain chroot when unshare is absent</source>
        <translation>Den porterade hjälparen faller tillbaka till en skyddad vanlig krom när oakt är frånvarande</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Avbokning</translation>
    </message>
    <message>
        <source>OK</source>
        <translation>OK</translation>
    </message>
    <message>
        <source>Yes</source>
        <translation>Ja ja ja ja ja</translation>
    </message>
    <message>
        <source>No</source>
        <translation>Ingen</translation>
    </message>
    <message>
        <source>read-only helper diagnostic</source>
        <translation>Läs-bara hjälpare diagnostik</translation>
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
        <translation>Linux återhämtning och boot-repair verktyg</translation>
    </message>
    <message>
        <source>GUARDED REPAIR</source>
        <translation>Guarded Repair</translation>
    </message>
    <message>
        <source>Ordinary repairs require an explicitly selected non-host target. The protected running host has a separate deliberate maintenance mode with the same guarded repair stages and requires privilege authorization.</source>
        <translation>Vanliga reparationer kräver ett explicit valt non-host-mål. Den skyddade körvärden har ett separat avsiktligt underhållsläge med samma bevakade reparationssteg och kräver godkännande av privilegier.</translation>
    </message>
    <message>
        <source>Systems</source>
        <translation>System</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>Diagnostik</translation>
    </message>
    <message>
        <source>Repair</source>
        <translation>Reparation</translation>
    </message>
    <message>
        <source>Chroot Shell</source>
        <translation>Chroot Shell</translation>
    </message>
    <message>
        <source>File Copy</source>
        <translation>File Copy</translation>
    </message>
    <message>
        <source>Logs</source>
        <translation>Loggar</translation>
    </message>
    <message>
        <source>Settings</source>
        <translation>Inställningar</translation>
    </message>
    <message>
        <source>(not created yet)</source>
        <translation>(Inte skapad ännu)</translation>
    </message>
    <message>
        <source>Information</source>
        <translation>Information om information</translation>
    </message>
    <message>
        <source>Close</source>
        <translation>Nära</translation>
    </message>
    <message>
        <source>&amp;File</source>
        <translation>-&amp;fil</translation>
    </message>
    <message>
        <source>&amp;Refresh Devices</source>
        <translation>&amp;refresh enheter</translation>
    </message>
    <message>
        <source>&amp;Lock Administrator Session</source>
        <translation>&amp;Lock Administrator Session</translation>
    </message>
    <message>
        <source>&amp;Quit</source>
        <translation>&amp;Quit</translation>
    </message>
    <message>
        <source>&amp;View</source>
        <translation>&amp;View</translation>
    </message>
    <message>
        <source>&amp;Systems</source>
        <translation>&amp;Systems</translation>
    </message>
    <message>
        <source>&amp;Diagnostics</source>
        <translation>&amp;Diagnostik</translation>
    </message>
    <message>
        <source>&amp;Logs</source>
        <translation>&amp;Logs</translation>
    </message>
    <message>
        <source>&amp;Settings</source>
        <translation>&amp;Inställningar</translation>
    </message>
    <message>
        <source>&amp;Auto-size Device Columns</source>
        <translation>&amp;Auto-storlek Device Columns</translation>
    </message>
    <message>
        <source>&amp;Wrap Log Lines</source>
        <translation>&amp;Wrap Log Lines</translation>
    </message>
    <message>
        <source>&amp;Help</source>
        <translation>&amp;Hjälp</translation>
    </message>
    <message>
        <source>&amp;Using Boot Bitch</source>
        <translation>&amp;Använda Boot Bitch</translation>
    </message>
    <message>
        <source>&amp;About Boot Bitch</source>
        <translation>&amp;Om Boot Bitch</translation>
    </message>
    <message>
        <source>Device columns auto-sized. Drag headers to fine-tune widths.</source>
        <translation>Enhet kolumner auto storlek. Dra rubriker till finjustera bredder.</translation>
    </message>
    <message>
        <source>A helper command is running; wait for it to finish before locking the session.</source>
        <translation>En hjälpare kommando körs; vänta på att det slutar innan låsa sessionen.</translation>
    </message>
    <message>
        <source>Administrator session locked; the next privileged action will request authorization.</source>
        <translation>Administratörssession låst; nästa privilegierade åtgärd kommer att begära tillstånd.</translation>
    </message>
    <message>
        <source>Using Boot Bitch</source>
        <translation>Använda Boot Bitch</translation>
    </message>
    <message>
        <source>Boot Bitch must run from a different booted Linux environment than the system being repaired. Use a Linux live medium or another Linux installation on a different physical drive.&lt;br&gt;&lt;br&gt;The running host is protected from ordinary repair-target selection, but it can be explicitly selected through &lt;b&gt;Host Maintenance&lt;/b&gt; for guarded native diagnostics and supported maintenance stages.&lt;br&gt;&lt;br&gt;Diagnostics follow the Systems page: the committed repair drive while Host Maintenance is off, or the protected running host while it is active.&lt;br&gt;&lt;br&gt;The first privileged action requests administrator authorization once for this Boot Bitch window; &lt;b&gt;File - Lock Administrator Session&lt;/b&gt; ends that helper session immediately. Every repair keeps the helper&apos;s own runtime preflights.</source>
        <translation>Boot Bitch måste köras från en annan startad Linux-miljö än systemet som repareras. Använd ett Linux live-medium eller en annan Linux-installation på en annan fysisk enhet.&lt;br&gt;&lt;br&gt; Den löpande värden är skyddad från vanliga reparationsmål val, men det kan uttryckligen väljas genom &lt;b&gt;Host Maintenance&lt;/b&gt; för bevakad infödd diagnostik och understödda underhållssteg.&lt;br&gt;&lt;br&gt; Diagnostik följer sidan System: den engagerade reparationsenheten medan Värd Underhåll är avstängd, eller den skyddade körvärden medan den är aktiv.&lt;br&gt;&lt;br&gt; Den första privilegierade åtgärden begär administratörstillstånd en gång för detta Boot Bitch-fönster; &lt;b&gt;File - Lock Administrator Session&lt;/b&gt; slutar att hjälparsessionen omedelbart. Varje reparation håller hjälparens egna runtime preflights.</translation>
    </message>
    <message>
        <source>Select a physical drive; Boot Bitch resolves the most likely Linux system volume automatically. The running host stays protected from ordinary target repairs, with a separate explicit host-maintenance path for its own system. The Details button shows the protected host&apos;s facts in the details pane; selecting any drive row restores the per-drive pane.</source>
        <translation>Välj en fysisk enhet; Boot Bitch löser den mest sannolika Linux-systemvolymen automatiskt. Den löpande värden förblir skyddad från vanliga målreparationer, med en separat explicit värdunderhållsväg för sitt eget system. Detaljknappen visar den skyddade värdens fakta i detaljrutan; välja vilken enhetsrad som återställer per-drive-rutan.</translation>
    </message>
    <message>
        <source>Refresh Devices</source>
        <translation>Refresh Devices</translation>
    </message>
    <message>
        <source>Re-read the read-only kernel inventory (/proc/partitions, /proc/mounts, /proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* and the udev metadata database). No block device is opened and nothing is written.</source>
        <translation>Läs bara inventeringen (/proc/partitions,/proc/mounts,/proc/swaps,/sys/block,/dev/mapper,/dev/disk/by-* och udev metadatabasen). Ingen block enhet öppnas och ingenting är skrivet.</translation>
    </message>
    <message>
        <source>[OK]</source>
        <translation>[OK]</translation>
    </message>
    <message>
        <source>The running system was detected and stays protected from ordinary target repairs.</source>
        <translation>Systemet upptäcktes och skyddas från vanliga målreparationer.</translation>
    </message>
    <message>
        <source>Detecting running system...</source>
        <translation>Detektera körsystem...</translation>
    </message>
    <message>
        <source>Detecting protected storage...</source>
        <translation>Detektera skyddad lagring...</translation>
    </message>
    <message>
        <source>PROTECTED</source>
        <translation>Protekterad</translation>
    </message>
    <message>
        <source>The running host remains protected from ordinary repair-target operations.</source>
        <translation>Den löpande värden förblir skyddad från vanliga reparationsmål operationer.</translation>
    </message>
    <message>
        <source>Details</source>
        <translation>Detaljer</translation>
    </message>
    <message>
        <source>Show read-only details for the protected running host.</source>
        <translation>Visa endast detaljer för den skyddade körvärden.</translation>
    </message>
    <message>
        <source>Host Maintenance</source>
        <translation>Värd underhåll</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Gör standard</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host (menu.lst default directive with a backup and rollback). Requires Host Maintenance and the cached host-default probe.</source>
        <translation>Gör den kanoniska installerade kärnan in i standard GRUB-legacy start inträde på den löpande värden (meny.lst standarddirektiv med en backup och rollback). Kräver värd underhåll och den cachade värd-default sonden.</translation>
    </message>
    <message>
        <source>Available repair targets</source>
        <translation>Tillgängliga reparationsmål</translation>
    </message>
    <message>
        <source>Most likely first</source>
        <translation>Mest troligt första</translation>
    </message>
    <message>
        <source>Drives are listed by the read-only inventory. Select a row to inspect it; Select Target commits the selected non-host drive with its auto-resolved Linux root component.</source>
        <translation>Körningar listas av den lätta inventeringen. Välj en rad för att inspektera den; Välj Target begår den valda icke-värdenheten med sin auto-upplösta Linux-rotkomponent.</translation>
    </message>
    <message>
        <source>Device</source>
        <translation>Enhet</translation>
    </message>
    <message>
        <source>Size</source>
        <translation>Storlek</translation>
    </message>
    <message>
        <source>Type</source>
        <translation>Typ</translation>
    </message>
    <message>
        <source>Filesystem</source>
        <translation>Filsystem</translation>
    </message>
    <message>
        <source>Select Target</source>
        <translation>Välj mål</translation>
    </message>
    <message>
        <source>Unlock</source>
        <translation>Lås upp</translation>
    </message>
    <message>
        <source>Authorize</source>
        <translation>auktorisera</translation>
    </message>
    <message>
        <source>Establish the privileged helper session for the current scope now instead of waiting for the next privileged action.</source>
        <translation>Upprätta den privilegierade hjälparsessionen för det aktuella omfånget nu istället för att vänta på nästa privilegierade åtgärd.</translation>
    </message>
    <message>
        <source>Committed target: none</source>
        <translation>Kommitterat mål: ingen</translation>
    </message>
    <message>
        <source>Unlock state for the selected drive; the LUKS passphrase is never logged.</source>
        <translation>Lås upp tillstånd för den valda enheten; LUKS-passfrasen är aldrig inloggad.</translation>
    </message>
    <message>
        <source>Unlock status</source>
        <translation>Lås upp status</translation>
    </message>
    <message>
        <source>Read-only inventory plus helper-confirmed facts; mirrors the modern Qt6 Selected drive details panel.</source>
        <translation>Read-only inventering plus hjälpbekräftade fakta; speglar den moderna Qt6 Selected driv detaljer panel.</translation>
    </message>
    <message>
        <source>Selected drive details</source>
        <translation>Valda driv detaljer</translation>
    </message>
    <message>
        <source>Field</source>
        <translation>Fält</translation>
    </message>
    <message>
        <source>Value</source>
        <translation>Värde</translation>
    </message>
    <message>
        <source>Run All runs every available read-only diagnostic for the current scope; selecting a check runs it alone. Diagnostics are read-only and are the only evidence source for the gated repair actions.</source>
        <translation>Kör Alla kör varje tillgänglig lätt diagnostik för det aktuella omfattningen; välja en check kör den ensam. Diagnostik är lättlästa och är den enda beviskällan för gated reparation åtgärder.</translation>
    </message>
    <message>
        <source>Target: none selected</source>
        <translation>Mål: ingen vald</translation>
    </message>
    <message>
        <source>Diagnostics follow the committed repair target, or the protected running host while Host Maintenance is active.</source>
        <translation>Diagnostik följer det engagerade reparationsmålet, eller den skyddade körvärden medan värdunderhåll är aktiv.</translation>
    </message>
    <message>
        <source>Run All</source>
        <translation>Kör alla</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope.</source>
        <translation>Kör alla - kör varje tillgänglig lätt diagnostik för nuvarande omfattning.</translation>
    </message>
    <message>
        <source>Target configuration:</source>
        <translation>Målkonfiguration:</translation>
    </message>
    <message>
        <source>Etch-era target configuration files; availability is probed read-only by the helper&apos;s diagnostics.</source>
        <translation>Etch-era målkonfigurationsfiler; tillgänglighet är probed read-only av hjälparens diagnostik.</translation>
    </message>
    <message>
        <source>Edit Target File...</source>
        <translation>Redigera målfil...</translation>
    </message>
    <message>
        <source>Runs one read-only diagnostic for the selected scope through the helper (`diagnose &lt;key&gt;` / `host-diagnose &lt;key&gt;`); Run All is the combined report.</source>
        <translation>Kör en lätt diagnostik för den valda omfattningen genom hjälparen (&quot;diagnos &lt;key&gt;&quot; / &quot;host-diagnose &lt;key&gt;&quot;); Run All är den kombinerade rapporten.</translation>
    </message>
    <message>
        <source>Diagnostic checks</source>
        <translation>Diagnostiska kontroller</translation>
    </message>
    <message>
        <source>Check</source>
        <translation>Kontrollera</translation>
    </message>
    <message>
        <source>Selected diagnostic</source>
        <translation>Vald diagnostik</translation>
    </message>
    <message>
        <source>Select a diagnostic</source>
        <translation>Välj en diagnostik</translation>
    </message>
    <message>
        <source>Choose a diagnostic from the list.</source>
        <translation>Välj en diagnostik från listan.</translation>
    </message>
    <message>
        <source>Ready</source>
        <translation>Redo</translation>
    </message>
    <message>
        <source>Results</source>
        <translation>Resultat</translation>
    </message>
    <message>
        <source>Run Diagnostic</source>
        <translation>Kör Diagnostic</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic for the current scope.</source>
        <translation>Kör Diagnostic - kör den valda lättläst diagnostik för det aktuella omfattningen.</translation>
    </message>
    <message>
        <source>Copy Results</source>
        <translation>Kopiera resultat</translation>
    </message>
    <message>
        <source>Save Results...</source>
        <translation>Spara resultat...</translation>
    </message>
    <message>
        <source>The Full Repair plan runs the selected legacy stages in order through the guarded helper; the individual tools run one stage at a time. Every action stays disabled until the cached capability lines say available and the helper keeps its runtime preflights.</source>
        <translation>Den fullständiga reparationsplanen kör de valda arvsstegen för att genom den skyddade hjälparen; de enskilda verktygen kör ett steg i taget. Varje åtgärd förblir funktionshindrad tills de cachade kapacitetslinjerna säger tillgängliga och hjälparen håller sina runtime preflights.</translation>
    </message>
    <message>
        <source>Full Repair plan</source>
        <translation>Full reparation plan</translation>
    </message>
    <message>
        <source>No stages selected</source>
        <translation>Inga steg valda</translation>
    </message>
    <message>
        <source>Configure Plan...</source>
        <translation>Konfigurera plan...</translation>
    </message>
    <message>
        <source>Open Settings to choose which Full Repair stages are part of the plan.</source>
        <translation>Öppna inställningar för att välja vilka Full Repair-steg som ingår i planen.</translation>
    </message>
    <message>
        <source>Run Full Repair</source>
        <translation>Kör full reparation</translation>
    </message>
    <message>
        <source>Select a repair drive, or choose Host Maintenance on the protected running-host card.</source>
        <translation>Välj en reparationsenhet, eller välj värdunderhåll på det skyddade körkortet.</translation>
    </message>
    <message>
        <source>Stage</source>
        <translation>Stage</translation>
    </message>
    <message>
        <source>Individual repair tools</source>
        <translation>Individuella reparationsverktyg</translation>
    </message>
    <message>
        <source>Tool</source>
        <translation>Verktyg</translation>
    </message>
    <message>
        <source>Full Repair</source>
        <translation>Full reparation</translation>
    </message>
    <message>
        <source>not reported</source>
        <translation>inte rapporterade</translation>
    </message>
    <message>
        <source>Selected tool</source>
        <translation>Valt verktyg</translation>
    </message>
    <message>
        <source>Select a repair tool</source>
        <translation>Välj ett reparationsverktyg</translation>
    </message>
    <message>
        <source>Run Tool</source>
        <translation>Kör verktyg</translation>
    </message>
    <message>
        <source>Select a tool to review its repair action.</source>
        <translation>Välj ett verktyg för att granska dess reparationsåtgärder.</translation>
    </message>
    <message>
        <source>Write actions ask for confirmation and then run the helper&apos;s own runtime preflights; the GUI never weakens them. A repair that is not proven &apos;unchanged&apos; invalidates the cached diagnostics and disables the gated actions until diagnostics run again.</source>
        <translation>Skriv handlingar be om bekräftelse och sedan köra hjälparens egna runtime preflights; GUI försvagar dem aldrig. En reparation som inte bevisas &quot;oförändrad&quot; ogiltigförklarar den cachade diagnostiken och inaktiverar gated åtgärder tills diagnostiken går igen.</translation>
    </message>
    <message>
        <source>Chroot shell</source>
        <translation>Chroot Shell</translation>
    </message>
    <message>
        <source>Offline commands run one at a time in a fresh chroot and cannot answer interactive prompts (apt-get -y upgrade works). Host-shell commands run directly on the running host. The helper&apos;s probe lines gate the command field; the exact reason appears in the tooltip.</source>
        <translation>Offline-kommandon kör en i taget i en ny krom och kan inte svara på interaktiva prompts (apt-get-y uppgraderingsarbeten). Host-shell kommandon kör direkt på den löpande värden. Hjälparens sondlinjer gate kommandofältet; den exakta orsaken visas i verktygsspetsen.</translation>
    </message>
    <message>
        <source>No &apos;Legacy feature shell:&apos; line is cached; run diagnostics for the selected scope to evaluate the helper&apos;s chroot/timeout containment probes (fail closed).</source>
        <translation>Inget &quot;Legacy-funktionsskal:&quot; -linje är cachad; kör diagnostik för det valda omfattningen för att utvärdera hjälparens krot / timeout-innehållningsprober (svik stängd).</translation>
    </message>
    <message>
        <source>Command</source>
        <translation>Kommando</translation>
    </message>
    <message>
        <source>Command:</source>
        <translation>Kommando:</translation>
    </message>
    <message>
        <source>One reviewed command string, passed to the helper as a single argument (no shell interpolation by the GUI).</source>
        <translation>En granskad kommandosträng, gick till hjälparen som ett enda argument (ingen skalinterpolering av GUI).</translation>
    </message>
    <message>
        <source>Run Command</source>
        <translation>Run Command</translation>
    </message>
    <message>
        <source>Clear Output</source>
        <translation>Clear Output</translation>
    </message>
    <message>
        <source>The helper exposes `shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for an offline target chroot and `host-shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for the running host. Both keep the helper&apos;s runtime preflights; this tab enables the command only when the scope is committed, the session is authorized and the scope&apos;s Legacy feature probe reports available.</source>
        <translation>Hjälparen exponerar &quot;shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;&quot; för en offline målkrot och &quot;host-shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;&quot; för den löpande värden. Båda håller hjälparens runtime preflights; den här fliken möjliggör kommandot först när omfattningen begås, är sessionen auktoriserad och omfattningens Legacy-funktionsprobe-rapporter tillgängliga.</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>File copy</translation>
    </message>
    <message>
        <source>Copy and verify files in either direction through the guarded helper (cp -a plus ownership restoration and a per-file byte-compare). The helper&apos;s file-copy probe gates the controls and keeps the direction and path containment checks.</source>
        <translation>Kopiera och verifiera filer i endera riktningen genom den skyddade hjälparen (cp -a plus ägande restaurering och en per-fil byte-compare). Hjälparens fil-kopia sond grindar kontrollerna och håller riktningen och baninnehåll kontroller.</translation>
    </message>
    <message>
        <source>Preview Changes</source>
        <translation>Förhandsgranskningsförändringar</translation>
    </message>
    <message>
        <source>Run a copy dry-run through the guarded helper. No files are changed.</source>
        <translation>Kör en kopia torr kör genom den skyddade hjälparen. Inga filer ändras.</translation>
    </message>
    <message>
        <source>Copy and Verify</source>
        <translation>Kopiera och verifiera</translation>
    </message>
    <message>
        <source>Copy staged items and verify the result. Existing destination names are overwritten when source content differs; unrelated destination files are never deleted.</source>
        <translation>Kopiera iscensatta objekt och verifiera resultatet. Befintliga destinationsnamn skrivs över när källinnehållet skiljer sig; orelaterade destinationsfiler raderas aldrig.</translation>
    </message>
    <message>
        <source>Direction:</source>
        <translation>Riktning:</translation>
    </message>
    <message>
        <source>Choose which system supplies the source files and which system receives them.</source>
        <translation>Välj vilket system som levererar källfilerna och vilket system som får dem.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from this host</source>
        <translation>1. Välj källfiler eller mappar från denna värd</translation>
    </message>
    <message>
        <source>Source</source>
        <translation>Källa</translation>
    </message>
    <message>
        <source>Files and folders staged for the verified copy. The legacy backend copies with cp -a and restores ownership with chown --reference; every regular file is byte-compared after the copy.</source>
        <translation>Filer och mappar iscensatta för verifierad kopia. Arvet backend kopior med cp -a och återställer ägande med chown -referens; varje vanlig fil är byte-jämfört efter kopian.</translation>
    </message>
    <message>
        <source>Add Files...</source>
        <translation>Lägga till filer...</translation>
    </message>
    <message>
        <source>Add Folder...</source>
        <translation>Lägg till mapp...</translation>
    </message>
    <message>
        <source>Remove</source>
        <translation>Ta bort</translation>
    </message>
    <message>
        <source>Clear</source>
        <translation>Clear</translation>
    </message>
    <message>
        <source>Clear the staged source list (nothing is copied or deleted).</source>
        <translation>Rensa den iscensatta källlistan (ingen kopieras eller raderas).</translation>
    </message>
    <message>
        <source>2. Choose destination in repaired system</source>
        <translation>2. Välj destination i reparerat system</translation>
    </message>
    <message>
        <source>unavailable: see the helper&apos;s Legacy feature file-copy: probe reason above</source>
        <translation>otillgänglig: se hjälparens Legacy-funktionsfilkopi: sondförklaring ovan</translation>
    </message>
    <message>
        <source>An absolute path inside the selected repair system (Host to Repair) or on the running host (Repair to Host).</source>
        <translation>En absolut väg inuti det valda reparationssystemet (Host to Repair) eller på den löpande värden (Repair to Host).</translation>
    </message>
    <message>
        <source>Browse Target Folders...</source>
        <translation>Bläddra bland målmappar...</translation>
    </message>
    <message>
        <source>Browse the selected repair system through the helper&apos;s temporary read-only mounts and choose an absolute destination path. No target files are changed while browsing.</source>
        <translation>Bläddra i det valda reparationssystemet genom hjälparens tillfälliga lätta monteringar och välj en absolut destinationsväg. Inga målfiler ändras när du surfar.</translation>
    </message>
    <message>
        <source>3. Ownership and copy policy</source>
        <translation>3. Ägarskap och kopieringspolicy</translation>
    </message>
    <message>
        <source>Ownership:</source>
        <translation>Ägarskap:</translation>
    </message>
    <message>
        <source>Smart destination ownership (recommended)</source>
        <translation>Smart destinationsägande (rekommenderas)</translation>
    </message>
    <message>
        <source>Preserve source numeric UID/GID</source>
        <translation>Bevara numeriska UID/GID</translation>
    </message>
    <message>
        <source>Smart mode validates UID/GID identity mapping across the two systems and falls back to the destination-directory owner when the same numeric ID means a different account (the legacy backend implements it with chown --reference).</source>
        <translation>Smart läge validerar UID / GID-identitetskartläggning över de två systemen och faller tillbaka till destinationsdirektorns ägare när samma numeriska ID betyder ett annat konto (arvet backend implementerar det med chown-referens).</translation>
    </message>
    <message>
        <source>Application log</source>
        <translation>Applikationslogg</translation>
    </message>
    <message>
        <source>The complete session register; Save As... writes every entry even while a filter hides lines. If a writable system share mount exists at /host, Save As... starts there; otherwise the log directory is the fallback. Prior session files are listed read-only.</source>
        <translation>Det fullständiga sessionsregistret; Spara som ... skriver varje post även när ett filter döljer rader. Om ett skrivbart system dela montering finns på /host, Save As... börjar där; annars log katalogen är nedgången. Tidigare sessionsfiler listas endast läsa.</translation>
    </message>
    <message>
        <source>Session logs</source>
        <translation>Session loggar</translation>
    </message>
    <message>
        <source>Session</source>
        <translation>Session</translation>
    </message>
    <message>
        <source>The first entry is the live session; earlier files in the log directory are listed read-only below it.</source>
        <translation>Den första posten är live-sessionen; tidigare filer i log-katalogen listas endast under den.</translation>
    </message>
    <message>
        <source>New Session Log</source>
        <translation>Ny Session Log</translation>
    </message>
    <message>
        <source>Close the active session file; it becomes a prior session and the next log entry starts a new file.</source>
        <translation>Stäng den aktiva sessionsfilen; det blir en föregående session och nästa loginmatning startar en ny fil.</translation>
    </message>
    <message>
        <source>Add Note</source>
        <translation>Lägg till Note</translation>
    </message>
    <message>
        <source>Append a NOTE entry to the live session register.</source>
        <translation>Skicka ett meddelande till live sessionsregistret.</translation>
    </message>
    <message>
        <source>Delete</source>
        <translation>Delete</translation>
    </message>
    <message>
        <source>Delete the selected prior session file (the live session is never deleted).</source>
        <translation>Ta bort den valda förhandsfilen (den levande sessionen raderas aldrig).</translation>
    </message>
    <message>
        <source>Refresh</source>
        <translation>Refresh</translation>
    </message>
    <message>
        <source>Save As...</source>
        <translation>Spara som...</translation>
    </message>
    <message>
        <source>Save the complete session log (all entries, not just the current filter).</source>
        <translation>Spara hela sessionsloggen (alla poster, inte bara det aktuella filtret).</translation>
    </message>
    <message>
        <source>Clear Register</source>
        <translation>Clear Register</translation>
    </message>
    <message>
        <source>Clear the live register and view; prior session files are never modified.</source>
        <translation>Rensa live-registret och vyn; tidigare sessionsfiler ändras aldrig.</translation>
    </message>
    <message>
        <source>Search log:</source>
        <translation>Sök logga:</translation>
    </message>
    <message>
        <source>Type any characters to show matching log entries (case-insensitive). Save As always writes every entry.</source>
        <translation>Skriv alla tecken för att visa matchande loggposter (case-insensitive). Spara Som alltid skriver varje post.</translation>
    </message>
    <message>
        <source>Filter:</source>
        <translation>Filter:</translation>
    </message>
    <message>
        <source>Filter the visible log by entry kind. Selecting a diagnostic section shows the lines captured for that section; a workflow filter such as File system repair or Package repair shows its mapped repair lines (File copy has no lines in this frontend). Save As always writes every entry.</source>
        <translation>Filtrera den synliga loggen genom inträdestyp. Välja en diagnostisk sektion visar de rader som fångats för det avsnittet; ett arbetsflödesfilter som reparation av filer eller reparation av paket visar sina kartlagda reparationslinjer (File copy har inga linjer i denna frontend). Spara Som alltid skriver varje post.</translation>
    </message>
    <message>
        <source>Settings are stored per user under ~/.qt/, one file per settings group (devicesrc, logsrc, diagnosticsrc, repairrc), and are saved immediately on every change and on close. Launch the GUI as the same user to keep your overrides; a GUI started as root keeps its own copies.</source>
        <translation>Inställningar lagras per användare under ~/.qt/, en fil per inställningar grupp (enheter, logsrc, diagnosticsrc, reparatörc), och sparas omedelbart på varje förändring och på nära håll. Starta GUI som samma användare för att hålla dina övertoner; en GUI började som rot håller sina egna kopior.</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Device discovery</translation>
    </message>
    <message>
        <source>Show devices without an identified Linux installation</source>
        <translation>Visa enheter utan identifierad Linux-installation</translation>
    </message>
    <message>
        <source>Show removable and USB storage</source>
        <translation>Visa flyttbar och USB-lagring</translation>
    </message>
    <message>
        <source>Show encrypted devices before unlocking</source>
        <translation>Visa krypterade enheter innan du låser upp</translation>
    </message>
    <message>
        <source>When off, drives without a visible Linux filesystem are hidden unless they still contain an encrypted device and encrypted devices are shown.</source>
        <translation>När du är av visas enheter utan ett synligt Linux-filsystem om de fortfarande innehåller en krypterad enhet och krypterade enheter visas.</translation>
    </message>
    <message>
        <source>When off, removable and USB drives are hidden from the repair-target list.</source>
        <translation>När de är borttagbara och USB-enheter är dolda från reparationsmållistan.</translation>
    </message>
    <message>
        <source>When off, drives with an encrypted device are hidden until the volume is unlocked.</source>
        <translation>När av, kör med en krypterad enhet är dold tills volymen är låst.</translation>
    </message>
    <message>
        <source>Include the %1 stage in the Full Repair plan. The stage runs in the plan order shown on the Repair tab.</source>
        <translation>Inkludera %1-steget i Full Repair-planen. Scenen går i planordningen som visas på fliken Repair.</translation>
    </message>
    <message>
        <source>Automatically regenerate read-only diagnostics after repairs or target changes</source>
        <translation>Regenerera automatiskt lätt diagnostik efter reparationer eller måländringar</translation>
    </message>
    <message>
        <source>Regenerates the cached read-only diagnostics for the current scope after an operation that invalidates them (LUKS unlock, target configuration edit). It runs only inside an already authorized administrator session and never opens an authorization prompt by itself.</source>
        <translation>Regenererar den cachade lätta diagnostiken för det aktuella omfattningen efter en operation som ogiltigförklarar dem (LUKS låsa upp, målkonfigurationsredigering). Det går bara inuti en redan auktoriserad administratörssession och öppnar aldrig en auktorisation som drivs av sig själv.</translation>
    </message>
    <message>
        <source>Wrap long log lines</source>
        <translation>Wrap long log lines</translation>
    </message>
    <message>
        <source>Mandatory safety controls</source>
        <translation>Obligatoriska säkerhetskontroller</translation>
    </message>
    <message>
        <source>Host capabilities and dependencies</source>
        <translation>Värdkapacitet och beroenden</translation>
    </message>
    <message>
        <source>Refresh Capabilities</source>
        <translation>Refresh kapacitet</translation>
    </message>
    <message>
        <source>Re-run the read-only host capability probes (a PATH search, nothing is executed) and refresh the distribution summary.</source>
        <translation>Re-run the read-only host capability probes (en PATH-sökning, ingenting utförs) och uppdatera distributionssammanfattningen.</translation>
    </message>
    <message>
        <source>Install Missing Support...</source>
        <translation>Installera saknad support...</translation>
    </message>
    <message>
        <source>Automatic installation will require explicit package mapping and privilege authorization.</source>
        <translation>Automatisk installation kommer att kräva explicit paketkartläggning och privilegieringstillstånd.</translation>
    </message>
    <message>
        <source>Application configuration</source>
        <translation>Applikationskonfiguration</translation>
    </message>
    <message>
        <source>Boot Bitch Legacy</source>
        <translation>Boot Bitch Legacy</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list first.</source>
        <translation>Välj en fysisk enhet i listan Tillgängliga reparationsmål först.</translation>
    </message>
    <message>
        <source>Protected system</source>
        <translation>Skyddat system</translation>
    </message>
    <message>
        <source>The running system cannot be selected as a repair target. Use Host Maintenance for the protected running host or choose another disk.</source>
        <translation>Körsystemet kan inte väljas som ett reparationsmål. Använd värdunderhåll för den skyddade körvärden eller välj en annan disk.</translation>
    </message>
    <message>
        <source>Unlock or select a Linux system first</source>
        <translation>Lås upp eller välj ett Linux-system först</translation>
    </message>
    <message>
        <source>This encrypted drive has no visible Linux filesystem yet. Use Unlock, refresh devices, and select the target after its Linux root is detected.</source>
        <translation>Denna krypterade enhet har inget synligt Linux-filsystem än. Använd Lås upp, uppdatera enheter och välj målet efter att Linux-roten upptäcks.</translation>
    </message>
    <message>
        <source>The selected root component (%1) belongs to the running system and cannot be committed as a repair target. Use Host Maintenance for the protected running host.</source>
        <translation>Den valda rotkomponenten (%1) tillhör operativsystemet och kan inte begås som ett reparationsmål. Använd Host Maintenance för den skyddade körvärden.</translation>
    </message>
    <message>
        <source>repair target commit</source>
        <translation>Reparationsmål</translation>
    </message>
    <message>
        <source>Repair drive selected: %1</source>
        <translation>Reparationsenhet vald: %1</translation>
    </message>
    <message>
        <source>; best detected system component: %1</source>
        <translation>bäst upptäckt systemkomponent: %1</translation>
    </message>
    <message>
        <source>. No mount or repair action was performed.</source>
        <translation>Ingen montering eller reparation utfördes.</translation>
    </message>
    <message>
        <source>Make Default unavailable</source>
        <translation>Gör standard otillgänglig</translation>
    </message>
    <message>
        <source>Make Default is a running-host action on this frontend; enter Host Maintenance first.</source>
        <translation>Gör Default är en löpande handling på denna frontend; ange värdunderhåll först.</translation>
    </message>
    <message>
        <source>Administrator authorization required</source>
        <translation>Administratörstillstånd krävs</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize first.</source>
        <translation>Ingen administratörssession är aktiv; tryck först.</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host?

The helper verifies /boot/grub/menu.lst, sets the `default &lt;N&gt;` directive to the canonical entry, backs the menu up first and restores it on any failure.</source>
        <translation>Gör den kanoniska installerade kärnan in i standard GRUB-legacy start inträde på den löpande värden?

Hjälparen verifierar /boot / grub /menu.lst, ställer in &quot;standard &lt;N&gt;&quot; -direktivet till den kanoniska posten, stöder menyn upp först och återställer den på något fel.</translation>
    </message>
    <message>
        <source>Host Maintenance unavailable</source>
        <translation>Värd underhåll otillgänglig</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a committed repair target or a detected running host.</source>
        <translation>Den löpande värdmålet kunde inte upptäckas; diagnostik behöver ett engagerat reparationsmål eller en upptäckt körvärd.</translation>
    </message>
    <message>
        <source>Exit Host Maintenance</source>
        <translation>Exit Host Underhåll</translation>
    </message>
    <message>
        <source>Host Maintenance is active; diagnostics and gated repairs target the protected running host.</source>
        <translation>Värd Underhåll är aktiv; diagnostik och gated reparationer riktar sig till den skyddade körvärden.</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list on the Systems tab first, or use Host Maintenance for the protected running host.</source>
        <translation>Välj en fysisk enhet i listan Tillgängliga reparationsmål på fliken System först eller använd Värd Underhåll för den skyddade körvärden.</translation>
    </message>
    <message>
        <source>The selected drive (%1) is the protected running host. Choose Host Maintenance on the Systems tab to run read-only host diagnostics and guarded host repairs; ordinary target repairs stay disabled.</source>
        <translation>Den valda enheten (%1) är den skyddade körvärden. Välj Host Maintenance på fliken System för att köra endast värddiagnostik och bevakade värdreparationer; vanliga målreparationer förblir funktionshindrade.</translation>
    </message>
    <message>
        <source>No repair target is committed. Choose Select Target on the Systems tab (or Host Maintenance for the protected running host) first.</source>
        <translation>Inget reparationsmål begås. Välj Välj Mål på fliken System (eller Värd Underhåll för den skyddade körvärden) först.</translation>
    </message>
    <message>
        <source>The selection changed after the target was committed. Choose Select Target again.</source>
        <translation>Urvalet ändrades efter att målet begicks. Välj Välj Target igen.</translation>
    </message>
    <message>
        <source>Diagnostics scope required</source>
        <translation>Diagnostik omfattning krävs</translation>
    </message>
    <message>
        <source>Diagnostics scope unresolved</source>
        <translation>Diagnostik omfattning olöst</translation>
    </message>
    <message>
        <source>The running host target could not be resolved; use Refresh Devices and commit a repair target or re-enter Host Maintenance.</source>
        <translation>Det löpande värdmålet kunde inte lösas; använd Refresh Devices och begå ett reparationsmål eller återinträda värdunderhåll.</translation>
    </message>
    <message>
        <source>Diagnostic check required</source>
        <translation>Diagnostisk kontroll krävs</translation>
    </message>
    <message>
        <source>Select a diagnostic check in the list first.</source>
        <translation>Välj en diagnostisk check i listan först.</translation>
    </message>
    <message>
        <source>Repair target required</source>
        <translation>Reparationsmål krävs</translation>
    </message>
    <message>
        <source>Target file editing needs a committed repair target. Running-host maintenance has no target-file editing; commit an offline target first.</source>
        <translation>Målfilredigering behöver ett engagerat reparationsmål. Running-host underhåll har ingen målfil redigering; begå ett offline mål först.</translation>
    </message>
    <message>
        <source>Configuration file required</source>
        <translation>Konfigurationsfil krävs</translation>
    </message>
    <message>
        <source>Select a target configuration file first.</source>
        <translation>Välj en målkonfigurationsfil först.</translation>
    </message>
    <message>
        <source>Edit target %1</source>
        <translation>Redigera målet %1</translation>
    </message>
    <message>
        <source>Edit this target file through the guarded administrator helper. A successful save invalidates cached diagnostics; rerun diagnostics before repair. Generated files such as /boot/grub/menu.lst may be replaced by the next bootloader update.</source>
        <translation>Redigera denna målfil genom den bevakade administratörshjälparen. En framgångsrik spara ogiltigförklarar cached diagnostics; kör diagnostik före reparation. Genererade filer som /boot/grub/menu.lst kan ersättas med nästa bootloader-uppdatering.</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Avbokning</translation>
    </message>
    <message>
        <source>Save Target File</source>
        <translation>Spara målfil</translation>
    </message>
    <message>
        <source>No changes to %1.</source>
        <translation>Inga ändringar i %1.</translation>
    </message>
    <message>
        <source>Configuration write refused</source>
        <translation>Konfigurationsskrivning vägrade</translation>
    </message>
    <message>
        <source>The edited content contains NUL bytes; the guarded write refuses it.</source>
        <translation>Det redigerade innehållet innehåller NUL byte, den bevakade skriva vägrar det.</translation>
    </message>
    <message>
        <source>File too large</source>
        <translation>Fil för stor</translation>
    </message>
    <message>
        <source>The edited file is larger than 1 MiB. The guarded write refuses it; edit the file from a console instead.</source>
        <translation>Den redigerade filen är större än 1 MiB. Den bevakade skriva vägrar det; redigera filen från en konsol istället.</translation>
    </message>
    <message>
        <source>Write target configuration</source>
        <translation>Skriv målkonfiguration</translation>
    </message>
    <message>
        <source>Write the edited contents to %1? This modifies the repair target and invalidates cached diagnostics.</source>
        <translation>Skriv det redigerade innehållet till %1? Detta ändrar reparationsmålet och ogiltigförklarar cached diagnostics.</translation>
    </message>
    <message>
        <source>No diagnostic results to copy yet.</source>
        <translation>Inga diagnostiska resultat att kopiera ännu.</translation>
    </message>
    <message>
        <source>Diagnostic results copied to the clipboard.</source>
        <translation>Diagnostiska resultat kopierade till klippbordet.</translation>
    </message>
    <message>
        <source>No diagnostic results to save yet.</source>
        <translation>Inga diagnostiska resultat att spara ännu.</translation>
    </message>
    <message>
        <source>Text files (*.txt);;All files (*)</source>
        <translation>Textfiler (*.txt); Alla filer (*)</translation>
    </message>
    <message>
        <source>Save diagnostic results</source>
        <translation>Spara diagnostiska resultat</translation>
    </message>
    <message>
        <source>Could not write %1.</source>
        <translation>Kan inte skriva %1.</translation>
    </message>
    <message>
        <source>Diagnostic results saved to %1</source>
        <translation>Diagnostiska resultat sparade till %1</translation>
    </message>
    <message>
        <source>Unlock not available</source>
        <translation>Lås inte tillgänglig</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked. Select an offline repair target to unlock.</source>
        <translation>Den skyddade körvärden kan inte låsas upp. Välj ett offline reparationsmål för att låsa upp.</translation>
    </message>
    <message>
        <source>No locked LUKS component is currently visible on this selected drive.</source>
        <translation>Ingen låst LUKS-komponent syns för närvarande på den här valda enheten.</translation>
    </message>
    <message>
        <source>Confirm LUKS unlock</source>
        <translation>Bekräfta LUKS låsa upp</translation>
    </message>
    <message>
        <source>Unlock %1 on %2?

The helper opens a temporary device-mapper mapping with cryptsetup and keeps it open for this recovery session. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>Lås %1 på %2?

Hjälparen öppnar en tillfällig kartläggning av enheten-mapper med kryptering och håller den öppen för denna återhämtningssession. Passfrasen reser genom en privat nyckelfil och placeras aldrig i kommandoargument eller loggar.</translation>
    </message>
    <message>
        <source>LUKS unlock</source>
        <translation>LUKS låsa upp</translation>
    </message>
    <message>
        <source>Unlock LUKS repair target</source>
        <translation>Lås upp LUKS reparationsmål</translation>
    </message>
    <message>
        <source>Enter the passphrase for %1.

It is sent only to cryptsetup over the helper&apos;s standard input and is never logged or placed on a command line.</source>
        <translation>Ange passfrasen för %1.

Det skickas bara för att kryptera över hjälparens standardinmatning och är aldrig inloggad eller placerad på en kommandorad.</translation>
    </message>
    <message>
        <source>Passphrase required</source>
        <translation>Passfras krävs</translation>
    </message>
    <message>
        <source>An empty passphrase was not submitted. Enter the LUKS passphrase or choose Cancel.</source>
        <translation>En tom passfras lämnades inte in. Ange LUKS-passfrasen eller välj Avbryt.</translation>
    </message>
    <message>
        <source>Unlock keyfile unavailable</source>
        <translation>Lås upp keyfile otillgänglig</translation>
    </message>
    <message>
        <source>The LUKS passphrase could not be written to a private keyfile in %1; the unlock was not started.</source>
        <translation>LUKS-passfrasen kunde inte skrivas till en privat nyckelfil i %1; låsningen startades inte.</translation>
    </message>
    <message>
        <source>Configuration write unavailable</source>
        <translation>Konfigurationen skriver otillgänglig</translation>
    </message>
    <message>
        <source>The edited content could not be written to a private temporary file in %1; the write was not started.</source>
        <translation>Det redigerade innehållet kunde inte skrivas till en privat tillfällig fil i %1; skrivandet startades inte.</translation>
    </message>
    <message>
        <source>ERROR: cannot read the session log %1</source>
        <translation>ERROR: kan inte läsa sessionsloggen %1</translation>
    </message>
    <message>
        <source>Viewing a prior session log (read-only): %1</source>
        <translation>Visa en tidigare sessionslogg (läs endast): %1</translation>
    </message>
    <message>
        <source>Session log list refreshed.</source>
        <translation>Session log lista uppdateras.</translation>
    </message>
    <message>
        <source>Note:</source>
        <translation>Notera:</translation>
    </message>
    <message>
        <source>Delete session log</source>
        <translation>Delete session log</translation>
    </message>
    <message>
        <source>Delete %1 permanently? This cannot be undone.</source>
        <translation>Ta bort %1 permanent? Detta kan inte vara ogjort.</translation>
    </message>
    <message>
        <source>%1 changed while the confirmation was open; the delete was refused.</source>
        <translation>%1 ändrades medan bekräftelsen var öppen; raden vägrades.</translation>
    </message>
    <message>
        <source>Unable to delete %1.</source>
        <translation>Kan inte ta bort %1.</translation>
    </message>
    <message>
        <source>Device discovery filters updated.</source>
        <translation>Device discovery filter uppdateras.</translation>
    </message>
    <message>
        <source>Unknown Linux distribution</source>
        <translation>Okänd Linux-distribution</translation>
    </message>
    <message>
        <source>sudo / gksu (no KAuth on this frontend)</source>
        <translation>sudo / gksu (ingen KAuth på denna frontend)</translation>
    </message>
    <message>
        <source>Available</source>
        <translation>Finns tillgänglig</translation>
    </message>
    <message>
        <source>Missing on this frontend</source>
        <translation>Saknas på denna frontend</translation>
    </message>
    <message>
        <source>Missing</source>
        <translation>Saknar</translation>
    </message>
    <message>
        <source>No repair tool is selected.</source>
        <translation>Inget reparationsverktyg väljs.</translation>
    </message>
    <message>
        <source>A helper command is already running.</source>
        <translation>En hjälpare kommando är redan igång.</translation>
    </message>
    <message>
        <source>Restore Graphical Login is a host-scope stage on this legacy frontend; enter Host Maintenance to run it.</source>
        <translation>Återställ grafisk inloggning är ett värd-scope-steg på denna arvsfrontend; ange värdunderhåll för att köra den.</translation>
    </message>
    <message>
        <source>The selected scope has no resolved root component; use Refresh Devices and commit the repair target again.</source>
        <translation>Den valda omfattningen har ingen löst rotkomponent; använd Uppfriskningsenheter och begå reparationsmålet igen.</translation>
    </message>
    <message>
        <source>This legacy frontend exposes no %1 action; the helper reports the capability as available.</source>
        <translation>Denna arvsfront exponerar ingen %1-åtgärd; hjälparen rapporterar förmågan som tillgänglig.</translation>
    </message>
    <message>
        <source>Run this guarded repair action using the cached read-only diagnostic evidence. A confirmation is shown first.</source>
        <translation>Kör denna skyddade reparationsåtgärd med hjälp av cachade lätta diagnostiska bevis. En bekräftelse visas först.</translation>
    </message>
    <message>
        <source>Enabled in Settings</source>
        <translation>Aktiverad i Inställningar</translation>
    </message>
    <message>
        <source>Disabled in Settings - enable it to include this stage</source>
        <translation>Inaktiverad i inställningar - gör det möjligt att inkludera detta steg</translation>
    </message>
    <message>
        <source>Unavailable: %1</source>
        <translation>Otillgänglig: %1</translation>
    </message>
    <message>
        <source>
Unavailable: %1</source>
        <translation>
Otillgänglig: %1</translation>
    </message>
    <message>
        <source>Repair tool unavailable</source>
        <translation>Reparationsverktyg otillgängligt</translation>
    </message>
    <message>
        <source>Confirm repair</source>
        <translation>Bekräfta reparation</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Your saved selection is kept and its availability is re-checked when diagnostics for this scope complete.</source>
        <translation>Inga cachade bevis för detta skede ännu. Ditt sparade urval hålls och dess tillgänglighet återkontrolleras när diagnostiken för denna omfattning är fullständig.</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Run diagnostics for the selected scope to populate the Full Repair plan; your selection is saved once the stage becomes available.</source>
        <translation>Inga cachade bevis för detta skede ännu. Kör diagnostik för det valda omfattningen för att fylla den fullständiga reparationsplanen; ditt val sparas när scenen blir tillgänglig.</translation>
    </message>
    <message>
        <source>Unknown Full Repair stage.</source>
        <translation>Okänd Full Repair scen.</translation>
    </message>
    <message>
        <source>No Full Repair stages are selected or available; use Configure Plan... to choose the stages.</source>
        <translation>Inga fullständiga reparationssteg väljs eller är tillgängliga; använd Konfigurationsplan ... för att välja scenerna.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab to establish the session.</source>
        <translation>Administratörstillstånd krävs; tryck på Auktorisera på fliken System eller reparation för att fastställa sessionen.</translation>
    </message>
    <message>
        <source>Runs the selected stages using the cached read-only diagnostic evidence after privilege confirmation.</source>
        <translation>Kör de valda stegen med hjälp av cachade lätta diagnostiska bevis efter privilegiebekräftelse.</translation>
    </message>
    <message>
        <source>No Full Repair stages selected - use Configure Plan... or Settings.</source>
        <translation>Inga fullständiga reparationssteg valda - använd Konfigurationsplan ... eller Inställningar.</translation>
    </message>
    <message>
        <source>1 stage selected</source>
        <translation>1 etapp vald</translation>
    </message>
    <message>
        <source>%1 stages selected</source>
        <translation>%1-steg valda</translation>
    </message>
    <message>
        <source>No repair stages are selected. Use Configure Plan... to choose the stages Full Repair will run.</source>
        <translation>Inga reparationssteg väljs. Använd konfigurationsplan ... för att välja stegen Full Repair kommer att köras.</translation>
    </message>
    <message>
        <source>No selected stages are available. %1</source>
        <translation>Inga utvalda steg finns tillgängliga. %1</translation>
    </message>
    <message>
        <source>Full Repair unavailable</source>
        <translation>Full reparation otillgänglig</translation>
    </message>
    <message>
        <source>Run the Full Repair plan?

The selected stages run in order through the helper&apos;s guarded repair command:

</source>
        <translation>Kör hela reparationsplanen?

De valda stegen körs i ordning genom hjälparens bevakade reparationskommando:

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
Hjälparen håller varje runtime preflight; ett stadium som misslyckas stoppar planen.</translation>
    </message>
    <message>
        <source>Running %1 through the privileged helper... The Logs tab keeps the complete transcript.</source>
        <translation>Kör %1 genom den privilegierade hjälparen Fliken Logs håller hela transkriptet.</translation>
    </message>
    <message>
        <source>Shell scope required</source>
        <translation>Shell omfattning krävs</translation>
    </message>
    <message>
        <source>No administrator session is active.

Press Authorize on the Systems or Repair tab to establish the session, or enter Host Maintenance / commit a repair target on the Systems tab; the chroot shell then reuses the cached authorization.</source>
        <translation>Ingen administratörssession är aktiv.

Tryck på Auktorisera på fliken System eller Repair för att upprätta sessionen, eller ange värdunderhåll / begå ett reparationsmål på fliken Systems; krotskalet återanvänder sedan det cachade tillståndet.</translation>
    </message>
    <message>
        <source>Shell command required</source>
        <translation>Shell kommando krävs</translation>
    </message>
    <message>
        <source>Enter the command to run first.</source>
        <translation>Ange kommandot att springa först.</translation>
    </message>
    <message>
        <source>Confirm running-host command</source>
        <translation>Bekräfta kör-host kommandot</translation>
    </message>
    <message>
        <source>Run this command as root on the protected running host?

%1

The helper keeps its runtime preflights; the command is passed as one argument and is never interpreted by the GUI.</source>
        <translation>Kör detta kommando som rot på den skyddade körvärden?

%1

Hjälparen håller sina runtime preflights; kommandot överförs som ett argument och tolkas aldrig av GUI.</translation>
    </message>
    <message>
        <source>Running %1...</source>
        <translation>Kör %1...</translation>
    </message>
    <message>
        <source>Authorization scope required</source>
        <translation>Tillståndsområde krävs</translation>
    </message>
    <message>
        <source>administrator authorization</source>
        <translation>administratörstillstånd</translation>
    </message>
    <message>
        <source>Administrator authorization</source>
        <translation>Administratörstillstånd</translation>
    </message>
    <message>
        <source>Administrator authorization is required for %1.

Enter the password for %2 (sudo). It is used only for this sudo authentication, is sent over a pipe and is never logged or placed on a command line. The authorization is cached for this session and reused by diagnostics and repairs.</source>
        <translation>Administratörstillstånd krävs för %1.

Ange lösenordet för %2 (sudo). Den används endast för denna sudo autentisering, skickas över ett rör och är aldrig inloggad eller placerad på en kommandorad. Behörigheten är cachad för denna session och återanvänds av diagnostik och reparationer.</translation>
    </message>
    <message>
        <source>your account</source>
        <translation>Ditt konto</translation>
    </message>
    <message>
        <source>Password required</source>
        <translation>Lösenord krävs</translation>
    </message>
    <message>
        <source>An empty password was not submitted. Enter the sudo password or choose Cancel.</source>
        <translation>Ett tomt lösenord lämnades inte in. Ange sudo lösenord eller välj Avbryt.</translation>
    </message>
    <message>
        <source>Administrator authorization failed</source>
        <translation>Administratörstillstånd misslyckades</translation>
    </message>
    <message>
        <source>sudo did not accept the password: %1

The command was not started.</source>
        <translation>Sudo accepterade inte lösenordet: %1

Kommandot startades inte.</translation>
    </message>
    <message>
        <source>elevation requires an interactive sudo password; run the smoke as root or after `sudo -S -v` with --elevate &apos;sudo -n&apos;</source>
        <translation>höjning kräver ett interaktivt sudo-lösenord; kör röken som rot eller efter &quot;sudo -S-v&quot; med -höj &quot;sudo -n&quot;</translation>
    </message>
    <message>
        <source>No administrator session is active for %1.

Press Authorize on the Systems or Repair tab to establish the session now, or enter Host Maintenance / commit a repair target on the Systems tab; diagnostics and repairs then reuse the cached authorization.</source>
        <translation>Ingen administratörssession är aktiv för %1.

Tryck på Auktorisera på fliken Systems eller Repair för att etablera sessionen nu, eller ange värdunderhåll / begå ett reparationsmål på fliken Systems; diagnostik och reparationer återanvänd sedan det cachade tillståndet.</translation>
    </message>
    <message>
        <source>Administrator authorization expired</source>
        <translation>Administratörstillstånd löpte ut</translation>
    </message>
    <message>
        <source>The cached administrator authorization for %1 expired or was refused.

Press Authorize on the Systems or Repair tab to re-establish the session, then run the command again. No command was started.</source>
        <translation>Det cachade administratörstillståndet för %1 upphörde eller vägrades.

Tryck på Auktorisera på fliken Systems or Repair för att återställa sessionen och kör sedan kommandot igen. Inget kommando startades.</translation>
    </message>
    <message>
        <source>Privileged operation completed successfully. Administrator authorization remains active for this session.</source>
        <translation>Den privilegierade operationen slutfördes framgångsrikt. Administratörstillståndet är fortfarande aktivt för denna session.</translation>
    </message>
    <message>
        <source>Privileged operation stopped with an error. Administrator authorization remains active; review the output before closing.</source>
        <translation>Den privilegierade operationen stannade med ett fel. Administratörstillstånd är fortfarande aktivt; granska utgången innan du stänger.</translation>
    </message>
    <message>
        <source>Inspection is read-only.

</source>
        <translation>Inspektion är lättläst.

</translation>
    </message>
    <message>
        <source>Configuration unavailable</source>
        <translation>Konfiguration otillgänglig</translation>
    </message>
    <message>
        <source>The helper could not read %1:

%2</source>
        <translation>Hjälparen kunde inte läsa %1:

%2</translation>
    </message>
    <message>
        <source>Configuration write failed</source>
        <translation>Configuration write misslyckades</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
Result: mapping opened for this recovery session.</source>
        <translation>State: olåst
Komponent: %1
Mapper: %2
Metod: hjälpare låsa upp (cryptsetup; passfras via en läge-600 keyfile, raderad efter användning)
Resultat: kartläggning öppnas för denna återhämtningssession.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: the passphrase was not accepted; retry offered.</source>
        <translation>State: låst
Komponent: %1
Metod: hjälpare låsa upp (cryptsetup)
Fel: passfrasen accepterades inte; retry erbjuds.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: %2</source>
        <translation>State: låst
Komponent: %1
Metod: hjälpare låsa upp (cryptsetup)
Fel: %2</translation>
    </message>
    <message>
        <source>Passphrase not accepted</source>
        <translation>Passfras inte accepterad</translation>
    </message>
    <message>
        <source>The LUKS passphrase was not accepted.

Try again?</source>
        <translation>LUKS-passfrasen accepterades inte.

Försök igen?</translation>
    </message>
    <message>
        <source>[-] %1 - not run (the plan stopped before reaching this stage)</source>
        <translation>%1 - inte springa (planen stannade innan du når detta stadium)</translation>
    </message>
    <message>
        <source>filesystem</source>
        <translation>Filsystem</translation>
    </message>
    <message>
        <source>[-] %1 - no file system errors found - no changes</source>
        <translation>%1 - inga filsystemfel hittades - inga ändringar</translation>
    </message>
    <message>
        <source>[FAIL] %1 - not reported</source>
        <translation>[FAIL] %1 - inte rapporterat</translation>
    </message>
    <message>
        <source>%1 stage(s) failed; review the helper output in Logs.</source>
        <translation>%1 scen(s) misslyckades; granska hjälparutgången i Logs.</translation>
    </message>
    <message>
        <source>the plan stopped before any stage completed.</source>
        <translation>Planen stannade innan något stadium slutfördes.</translation>
    </message>
    <message>
        <source>no repair was needed; cached diagnostics remain valid.</source>
        <translation>Ingen reparation behövdes; cachad diagnostik är fortfarande giltig.</translation>
    </message>
    <message>
        <source>repair completed; cached diagnostics were invalidated and must be regenerated.</source>
        <translation>reparation färdig; cachad diagnostik var ogiltig och måste regenereras.</translation>
    </message>
    <message>
        <source>%1 results: [OK] %2 successful | [FAIL] %3 failed | [-] %4 no repair needed - %5</source>
        <translation>%1 resultat: [OK] %2 framgångsrik | [FAIL] %3 misslyckades | [-] %4 ingen reparation behövs - %5</translation>
    </message>
    <message>
        <source>Re-run Diagnostic</source>
        <translation>Re-run Diagnostic</translation>
    </message>
    <message>
        <source>Unavailable</source>
        <translation>Otillgänglig</translation>
    </message>
    <message>
        <source>Commit this physical drive as the repair target.</source>
        <translation>Skicka denna fysiska enhet som reparationsmål.</translation>
    </message>
    <message>
        <source>Protection:</source>
        <translation>Skydd:</translation>
    </message>
    <message>
        <source>Host shell</source>
        <translation>Värdskal</translation>
    </message>
    <message>
        <source>Run on Host</source>
        <translation>Kör på Host</translation>
    </message>
    <message>
        <source>Host Shell</source>
        <translation>Värd Shell</translation>
    </message>
    <message>
        <source>The read-only probe found no editable target configuration file in this target. %1</source>
        <translation>Den lätta sonden fann ingen redigerbar målkonfigurationsfil i detta mål. %1</translation>
    </message>
    <message>
        <source>Not present in the selected target (omitted): %1</source>
        <translation>Inte närvarande i det valda målet (utgivet): %1</translation>
    </message>
    <message>
        <source>Run diagnostics to probe which target configuration files exist; the helper&apos;s read-only probe decides the list.</source>
        <translation>Kör diagnostik för att undersöka vilka målkonfigurationsfiler som finns; Hjälparens läs-bara sond bestämmer listan.</translation>
    </message>
    <message>
        <source>Probed read-only by the helper; a saved edit invalidates the cached diagnostics.</source>
        <translation>Probed read-only av hjälparen; en sparad redigering ogiltigförklarar den cachade diagnostiken.</translation>
    </message>
    <message>
        <source>Host maintenance: %1</source>
        <translation>Värd underhåll: %1</translation>
    </message>
    <message>
        <source>unresolved</source>
        <translation>olöst</translation>
    </message>
    <message>
        <source>Target: %1 + %2</source>
        <translation>Mål: %1 + %2</translation>
    </message>
    <message>
        <source>Host command</source>
        <translation>Värd kommando</translation>
    </message>
    <message>
        <source>Execute a command on the running host as root.</source>
        <translation>Utför ett kommando på den löpande värden som rot.</translation>
    </message>
    <message>
        <source>Execute a command inside the selected repair system as root.</source>
        <translation>Utför ett kommando i det valda reparationssystemet som rot.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab (or re-enter Host Maintenance / re-commit the repair target) to authorize this session.</source>
        <translation>Administratörstillstånd krävs; tryck på Auktorisera på fliken System eller Repair (eller återinträda Värd Underhåll / återbetala reparationsmålet) för att godkänna denna session.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root on the running host through the helper&apos;s guarded host-shell verb.</source>
        <translation>Kör ett granskat kommando som rot på den löpande värden genom hjälparens skyddade värd-shell verb.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root inside the target chroot through the helper&apos;s guarded shell verb.</source>
        <translation>Kör ett granskat kommando som rot inuti målkroten genom hjälparens bevakade skalverb.</translation>
    </message>
    <message>
        <source>Run a command on the running host as root (sudo is not needed). Commands are executed directly on the active system; output is kept in this window and in the application log.</source>
        <translation>Kör ett kommando på den löpande värden som rot (sudo behövs inte). Kommandon utförs direkt på det aktiva systemet; utgången hålls i detta fönster och i applikationsloggen.</translation>
    </message>
    <message>
        <source>Run a command inside the selected repair system as root (sudo is not needed). Commands are executed one at a time in a fresh chroot and cannot answer interactive prompts; use non-interactive flags such as apt-get -y upgrade. Output is kept in this window and in the application log.</source>
        <translation>Kör ett kommando i det valda reparationssystemet som rot (sudo behövs inte). Kommandon utförs en i taget i en ny krom och kan inte svara på interaktiva uppmaningar; använd icke-interaktiva flaggor som apt-get-y uppgradering. Utgången hålls i detta fönster och i applikationsloggen.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize or re-enter Host Maintenance / commit a repair target.</source>
        <translation>Ingen administratörssession är aktiv; tryck på Auktorisera eller återinträda värdunderhåll / begå ett reparationsmål.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from the repaired system</source>
        <translation>1. Välj källfiler eller mappar från det reparerade systemet</translation>
    </message>
    <message>
        <source>2. Choose destination on this host</source>
        <translation>2. Välj destination på denna värd</translation>
    </message>
    <message>
        <source>Add File Path...</source>
        <translation>Lägg till File Path...</translation>
    </message>
    <message>
        <source>Add Folder Path...</source>
        <translation>Lägg till mappväg...</translation>
    </message>
    <message>
        <source>Browse...</source>
        <translation>Bläddra...</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it (Host Maintenance does not provide a repair tree to browse).</source>
        <translation>Välj reparationsenheten i Systems innan du väljer en destination inuti den (Host Maintenance ger inte ett reparationsträd att bläddra).</translation>
    </message>
    <message>
        <source>Choose a host destination folder directly.</source>
        <translation>Välj en värd destination mapp direkt.</translation>
    </message>
    <message>
        <source>Copy the staged files and folders with cp -a, restore ownership with chown --reference and byte-compare every regular file afterwards.</source>
        <translation>Kopiera de iscensatta filerna och mappar med cp -a, återställa ägande med chown -referens och byte-compare varje vanlig fil efteråt.</translation>
    </message>
    <message>
        <source>File Copy unavailable</source>
        <translation>File Copy otillgänglig</translation>
    </message>
    <message>
        <source>Choose host destination folder</source>
        <translation>Välj värd destination mapp</translation>
    </message>
    <message>
        <source>Select a repair target</source>
        <translation>Välj ett reparationsmål</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it.</source>
        <translation>Välj reparationsenheten i Systems innan du väljer en destination inuti den.</translation>
    </message>
    <message>
        <source>File Copy - Browse Target Folders</source>
        <translation>File Copy - Bläddra bland målmappar</translation>
    </message>
    <message>
        <source>Browse Target Folders</source>
        <translation>Bläddra bland målmappar</translation>
    </message>
    <message>
        <source>The helper could not list the repair-system folder:

%1</source>
        <translation>Hjälparen kunde inte lista mappen reparationssystem:

%1</translation>
    </message>
    <message>
        <source>BROWSE_ENTRY</source>
        <translation>BROWSE ENTRY</translation>
    </message>
    <message>
        <source>(choose this folder: %1)</source>
        <translation>(Välj den här mappen: %1)</translation>
    </message>
    <message>
        <source>(parent folder)</source>
        <translation>(förälder mapp)</translation>
    </message>
    <message>
        <source>Select repair-system destination</source>
        <translation>Välj destination för reparationssystem</translation>
    </message>
    <message>
        <source>Choose a destination folder inside the repaired system (current folder: %1):</source>
        <translation>Välj en destinationsmapp inuti det reparerade systemet (nuvarande mapp: %1):</translation>
    </message>
    <message>
        <source>Folder</source>
        <translation>Mamma</translation>
    </message>
    <message>
        <source>Open</source>
        <translation>Öppet Öppet Öppet</translation>
    </message>
    <message>
        <source>(choose this folder:</source>
        <translation>(Välj den här mappen:</translation>
    </message>
    <message>
        <source>Add files to copy</source>
        <translation>Lägg till filer för att kopiera</translation>
    </message>
    <message>
        <source>Add folder to copy</source>
        <translation>Lägg till mapp för att kopiera</translation>
    </message>
    <message>
        <source>Stage at least one source and name a destination path first.</source>
        <translation>Steg minst en källa och namnge en destinationsväg först.</translation>
    </message>
    <message>
        <source>Copy the %1 staged item(s) to %2?

The helper keeps its direction and path containment checks; a sensitive repair-system destination is refused unless the helper approves it, and every regular file is byte-compared after the copy.</source>
        <translation>Kopiera %1 iscensatt objekt till %2?

Hjälparen håller sina riktnings- och väginnehållskontroller; en känslig reparationssystem destination vägras om inte hjälparen godkänner det, och varje vanlig fil är byte-jämförd efter kopian.</translation>
    </message>
    <message>
        <source>File Copy - Copy and Verify</source>
        <translation>File Copy - Kopiera och verifiera</translation>
    </message>
    <message>
        <source>File Copy - Preview Changes</source>
        <translation>File Copy - Förhandsgranska ändringar</translation>
    </message>
    <message>
        <source>Protected running host - details</source>
        <translation>Skyddad körvärd - detaljer</translation>
    </message>
    <message>
        <source>not mounted (offline target)</source>
        <translation>inte monterat (offline mål)</translation>
    </message>
    <message>
        <source>Select a drive to see its details.</source>
        <translation>Välj en enhet för att se dess detaljer.</translation>
    </message>
    <message>
        <source>Inspecting the selected component.</source>
        <translation>Inspektera den valda komponenten.</translation>
    </message>
    <message>
        <source>Helper-confirmed by the last read-only diagnostics.</source>
        <translation>Hjälpare bekräftad av den sista lättlästa diagnostiken.</translation>
    </message>
    <message>
        <source>Read-only inventory only; run diagnostics to confirm.</source>
        <translation>Läs-bara lager endast; kör diagnostik för att bekräfta.</translation>
    </message>
    <message>
        <source>PROTECTED - running system; read-only details only</source>
        <translation>PROTECTED - körsystem; lätta detaljer endast</translation>
    </message>
    <message>
        <source>Live / installer media - not selectable</source>
        <translation>Live / installer media - inte valbart</translation>
    </message>
    <message>
        <source>PROTECTED - running host scope</source>
        <translation>PROTECTED - kör värd omfattning</translation>
    </message>
    <message>
        <source>Unlock required before selection</source>
        <translation>Lås upp krävs innan valet</translation>
    </message>
    <message>
        <source>Eligible repair candidate</source>
        <translation>Berättigad reparation kandidat</translation>
    </message>
    <message>
        <source>Drive:</source>
        <translation>Kör:</translation>
    </message>
    <message>
        <source>Detected target:</source>
        <translation>Upptäckt mål:</translation>
    </message>
    <message>
        <source>Pending inspection</source>
        <translation>I väntan på inspektion</translation>
    </message>
    <message>
        <source>Model / label:</source>
        <translation>Modell/etikett:</translation>
    </message>
    <message>
        <source>Status:</source>
        <translation>Status:</translation>
    </message>
    <message>
        <source>Size:</source>
        <translation>Storlek:</translation>
    </message>
    <message>
        <source>Connection:</source>
        <translation>Anslutning:</translation>
    </message>
    <message>
        <source>Filesystem:</source>
        <translation>Filsystem:</translation>
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
        <translation>Körsystemskydd olöst</translation>
    </message>
    <message>
        <source>No protected physical backing disk was identified</source>
        <translation>Ingen skyddad fysisk backing disk identifierades</translation>
    </message>
    <message>
        <source>Current running Linux system</source>
        <translation>Nuvarande Linux-system</translation>
    </message>
    <message>
        <source>Critical mounts: %1</source>
        <translation>Kritiska fästen: %1</translation>
    </message>
    <message>
        <source>Read-only protected-running-system facts: the helper OS fact plus the inventory model, device path, size, transport and critical mounts. Nothing here is probed destructively.</source>
        <translation>Läs-bara skyddade-running-system fakta: Hjälparen OS-fakta plus lager modell, enhetsväg, storlek, transport och kritiska fästen. Inget här är probed destruktivt.</translation>
    </message>
    <message>
        <source>Select a drive to see unlock status.</source>
        <translation>Välj en enhet för att se låsa upp status.</translation>
    </message>
    <message>
        <source>State: protected
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
The protected running host cannot be unlocked or modified; unlock is available for an offline repair target only. Use Host Maintenance for the protected running host.</source>
        <translation>State: skyddad
Komponent: %1
Mapper: (ingen)
Metod: hjälpare låsa upp (cryptsetup; passfras via en läge-600 keyfile, raderad efter användning)
Den skyddade körvärden kan inte låsas upp eller modifieras; låsa upp är endast tillgänglig för ett offline-reparationsmål. Använd Host Maintenance för den skyddade körvärden.</translation>
    </message>
    <message>
        <source>(none detected)</source>
        <translation>(ingen upptäckt)</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
A locked LUKS container is visible on this drive; press Unlock to open it for this recovery session.</source>
        <translation>State: låst
Komponent: %1
Mapper: (ingen)
Metod: hjälpare låsa upp (cryptsetup; passfras via en läge-600 keyfile, raderad efter användning)
En låst LUKS behållare är synlig på den här enheten; tryck Lås upp för att öppna den för denna återhämtningssession.</translation>
    </message>
    <message>
        <source>(visible mapper)</source>
        <translation>(synlig mapper)</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: already open before this session (visible mapper; Boot Bitch will reuse it and will not close it).</source>
        <translation>State: olåst
Komponent: %1
Mapper: %2
Metod: redan öppen före denna session (synlig mapp; Boot Bitch kommer att återanvända den och kommer inte att stänga den).</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper-confirmed mapping from the last read-only diagnostics.</source>
        <translation>State: olåst
Komponent: %1
Mapper: %2
Metod: hjälpbekräftad kartläggning från den sista lättlästa diagnostiken.</translation>
    </message>
    <message>
        <source>(mapper)</source>
        <translation>(mapper)</translation>
    </message>
    <message>
        <source>State: locked or no encrypted component detected
Component: (none detected)
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
No locked LUKS component and no unlock operation were recorded for this drive in the current session.</source>
        <translation>Stat: låst eller ingen krypterad komponent upptäckt
Komponent: (ingen upptäckt)
Mapper: (ingen)
Metod: hjälpare låsa upp (cryptsetup; passfras via en läge-600 keyfile, raderad efter användning)
Ingen låst LUKS-komponent och ingen låsningsoperation registrerades för den här enheten i den aktuella sessionen.</translation>
    </message>
    <message>
        <source>unavailable - %1</source>
        <translation>Otillgänglig - %1</translation>
    </message>
    <message>
        <source>Host maintenance:
%1</source>
        <translation>Värd underhåll:
%1</translation>
    </message>
    <message>
        <source>Target:
%1 + %2</source>
        <translation>Mål:
%1 + %2</translation>
    </message>
    <message>
        <source>Authorization required: diagnostics and repairs fail closed until you press Authorize.</source>
        <translation>Tillstånd krävs: diagnostik och reparationer stängdes inte förrän du trycker på Auktorisering.</translation>
    </message>
    <message>
        <source>Re-establish the privileged helper session for the current scope now. The password is requested in the hidden-input modal and is never logged.</source>
        <translation>Återupprätta den privilegierade hjälparsessionen för nuvarande omfattning nu. Lösenordet begärs i dold ingångsmodalen och är aldrig inloggad.</translation>
    </message>
    <message>
        <source>Running...</source>
        <translation>Kör...</translation>
    </message>
    <message>
        <source>Scope required</source>
        <translation>Scope krävs</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope; this unlocks the gated actions.</source>
        <translation>Kör alla - kör varje tillgänglig lätt diagnostik för nuvarande omfattning; detta låser upp gated åtgärder.</translation>
    </message>
    <message>
        <source>Select a target and wait for any running command first.</source>
        <translation>Välj ett mål och vänta på någon körkommando först.</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic through the helper.</source>
        <translation>Kör Diagnostic - kör den valda lätta diagnostiken genom hjälparen.</translation>
    </message>
    <message>
        <source>Run All for the current scope and refresh the read-only backend profile.</source>
        <translation>Kör allt för nuvarande omfattning och uppdatera läs-bara backend-profilen.</translation>
    </message>
    <message>
        <source>Read or edit the selected target configuration file through the guarded helper; a saved edit invalidates cached diagnostics.</source>
        <translation>Läs eller redigera den valda målkonfigurationsfilen genom den skyddade hjälparen; en sparad redigering ogiltigförklarar cachad diagnostik.</translation>
    </message>
    <message>
        <source>Host Maintenance has no target-file editing; commit an offline repair target first.</source>
        <translation>Värd underhåll har ingen målfil redigering; begå ett offline reparation mål först.</translation>
    </message>
    <message>
        <source>Commit an offline repair target first.</source>
        <translation>Skicka ett offline reparationsmål först.</translation>
    </message>
    <message>
        <source>No target configuration file is available for this target; run diagnostics to probe the list.</source>
        <translation>Ingen målkonfigurationsfil är tillgänglig för detta mål; kör diagnostik för att undersöka listan.</translation>
    </message>
    <message>
        <source>Already Unlocked</source>
        <translation>Redan låst</translation>
    </message>
    <message>
        <source>An unlocked Linux filesystem is already visible on this drive. Boot Bitch will reuse the existing mapper and will not close or reopen a mapping created by this recovery session. Mounting happens during diagnostics (read-only) and repairs (read-write); data filesystems are never auto-mounted on selection.</source>
        <translation>Ett låst Linux-filsystem är redan synligt på den här enheten. Boot Bitch kommer att återanvända den befintliga mappen och kommer inte att stänga eller öppna en kartläggning som skapats av denna återhämtningssession. Montering sker under diagnostik (läs-bara) och reparationer (läs-skriv); datafilsystem är aldrig automatiskt monterade på val.</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked; use Host Maintenance for the protected running host.</source>
        <translation>Den skyddade körvärden kan inte låsas upp; använd Värd Underhåll för den skyddade körvärden.</translation>
    </message>
    <message>
        <source>Host Maintenance is the current scope, but the selected offline drive can still be unlocked.</source>
        <translation>Värd Underhåll är det aktuella omfattningen, men den valda offline-enheten kan fortfarande låsas upp.</translation>
    </message>
    <message>
        <source>Unlock %1 using cryptsetup through the privileged helper. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>Lås upp %1 med cryptsetup genom den privilegierade hjälparen. Passfrasen reser genom en privat nyckelfil och placeras aldrig i kommandoargument eller loggar.</translation>
    </message>
    <message>
        <source>The protected running system cannot be selected as a repair target; use Host Maintenance for the protected running host.</source>
        <translation>Det skyddade körsystemet kan inte väljas som ett reparationsmål; använd värdunderhåll för den skyddade körvärden.</translation>
    </message>
    <message>
        <source>Live / installer media is read-only boot media and cannot be selected as a repair target.</source>
        <translation>Live / installer media är lätt startmedia och kan inte väljas som ett reparationsmål.</translation>
    </message>
    <message>
        <source>Unlock the encrypted volume first; Select Target becomes available after a Linux filesystem is detected.</source>
        <translation>Lås upp den krypterade volymen först; Välj Target blir tillgängligt efter att ett Linux-filsystem har upptäckts.</translation>
    </message>
    <message>
        <source>Committed repair target. Repair, Diagnostics and File Copy target this physical drive until another drive is explicitly selected with Select Target.</source>
        <translation>Kommitterat reparationsmål. Reparation, Diagnostics och File Copy riktar denna fysiska enhet tills en annan enhet uttryckligen väljs med Select Target.</translation>
    </message>
    <message>
        <source>Commit %1 as the repair target; this leaves Host Maintenance and switches the scope to the selected drive.</source>
        <translation>Begå %1 som reparationsmål; detta lämnar Värd Underhåll och byter omfattningen till den valda enheten.</translation>
    </message>
    <message>
        <source>The running host target could not be detected.</source>
        <translation>Det löpande värdmålet kunde inte upptäckas.</translation>
    </message>
    <message>
        <source>Leave Host Maintenance and return to repair-target mode.</source>
        <translation>Lämna Host Maintenance och återgå till reparationsmålläge.</translation>
    </message>
    <message>
        <source>Select the running host for deliberate guarded maintenance; the administrator authorization is requested here once and cached for the session.</source>
        <translation>Välj den löpande värden för avsiktligt skyddat underhåll; administratörstillstånd begärs här en gång och cachas för sessionen.</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a repair target.</source>
        <translation>Den löpande värdmålet kunde inte upptäckas; diagnostik behöver ett reparationsmål.</translation>
    </message>
    <message>
        <source>Committed target: %1 + %2</source>
        <translation>Kommitterat mål: %1 + %2</translation>
    </message>
    <message>
        <source>Committed target: none (selection changed)</source>
        <translation>Kommitterat mål: ingen (valet ändras)</translation>
    </message>
    <message>
        <source>The selected scope has no resolved Linux root component; Refresh Devices and commit the repair target again.</source>
        <translation>Den valda omfattningen har ingen löst Linux-rotkomponent; Uppdatera enheter och begå reparationsmålet igen.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize on the Systems or Repair tab to re-establish it. Run All reuses the cached authorization and never prompts by itself.</source>
        <translation>Ingen administratörssession är aktiv; tryck på Auktorisera på fliken System eller Repair för att återställa den. Kör alla återanvänder cachad auktorisation och aldrig uppmanar av sig själv.</translation>
    </message>
    <message>
        <source>Run diagnostics for this scope to unlock the gated actions. Diagnostics are read-only and the only evidence source.</source>
        <translation>Kör diagnostik för detta utrymme för att låsa upp gated åtgärder. Diagnostik är lättlästa och den enda beviskällan.</translation>
    </message>
    <message>
        <source>A repair was not proven unchanged, so the cached diagnostics are invalidated. Run diagnostics again before another gated action.</source>
        <translation>En reparation bevisades inte oförändrad, så den cachade diagnostiken är ogiltig. Kör diagnostik igen innan en annan gated åtgärd.</translation>
    </message>
    <message>
        <source>Gated actions reflect the cached capability lines; the helper still runs every runtime preflight when a command starts.</source>
        <translation>Gated actions återspeglar cachade kapacitetslinjer; hjälparen kör fortfarande varje runtime preflight när ett kommando börjar.</translation>
    </message>
    <message>
        <source>Regenerating diagnostics automatically</source>
        <translation>Regenerera diagnostik automatiskt</translation>
    </message>
    <message>
        <source>Running diagnostic: %1</source>
        <translation>Kör diagnostik: %1</translation>
    </message>
    <message>
        <source>Running all diagnostics</source>
        <translation>Kör all diagnostik</translation>
    </message>
    <message>
        <source>Unlocking %1</source>
        <translation>Lås upp %1</translation>
    </message>
    <message>
        <source>running host</source>
        <translation>running host</translation>
    </message>
    <message>
        <source>offline target</source>
        <translation>Offline mål</translation>
    </message>
    <message>
        <source>host maintenance active</source>
        <translation>värd underhåll aktiv</translation>
    </message>
    <message>
        <source>repair target committed</source>
        <translation>Reparationsmål</translation>
    </message>
    <message>
        <source>no committed scope</source>
        <translation>Ingen engagerad omfattning</translation>
    </message>
    <message>
        <source>Current session</source>
        <translation>Nuvarande session</translation>
    </message>
    <message>
        <source>Log files (*.log);;All files (*)</source>
        <translation>Logfiler (*.log); Alla filer (*)</translation>
    </message>
    <message>
        <source>Save log as</source>
        <translation>Spara logg som</translation>
    </message>
    <message>
        <source>About Boot Bitch</source>
        <translation>Om Boot Bitch</translation>
    </message>
    <message>
        <source>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;&lt;p&gt;&lt;b&gt;Developer:&lt;/b&gt; CaptainMorgan12&lt;/p&gt;&lt;p&gt;This GUI is the package&apos;s entry point: a Qt 3.3.x frontend with the ported guarded helper for Debian Etch-era systems.&lt;/p&gt;&lt;p&gt;&lt;b&gt;Guarded repair mode:&lt;/b&gt; read-only diagnostics can inspect either the protected Running Host or an explicitly selected repair drive. The guarded Debian/APT package stages (interrupted configuration, broken dependencies, metadata refresh, upgrade), GRUB-legacy configuration regeneration, LUKS target unlock and guarded target-file editing run through the ported helper after confirmation; Host Maintenance enables the same supported stages natively on the active system after repeating the host identity and boot-mount checks.&lt;/p&gt;&lt;p&gt;The modern-only features - verified File Copy, Btrfs snapshot rollback, EFI/UKI and extlinux repair, boot-stack reconciliation and Make Default - are greyed with the helper&apos;s own probe reasons on this frontend; Arch/Alpine/Fedora package backends stay diagnostics-only here.&lt;/p&gt;&lt;p&gt;The first privileged action authorizes one cached administrator session per scope through a hidden-input modal (the Qt GUI remains unprivileged). It can be ended at any time from File - Lock Administrator Session.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;&lt;p&gt;&lt;b&gt;Developer:&lt;/b&gt; CaptainMorgan12&lt;/p&gt;&lt;p&gt; Denna GUI är paketets ingångspunkt: en Qt 3.3.x frontend med den porterade bevakade hjälparen för Debian Etch-era-system.&lt;/p&gt;&lt;p&gt;&lt;b&gt;Guarded reparation mode:&lt;/b&gt; lätt diagnostik kan inspektera antingen den skyddade Running Host eller en explicit vald reparationsenhet. De bevakade Debian/APT-paketstadierna (avbruten konfiguration, brutna beroenden, metadatauppdatering, uppgradering), GRUB-legacy konfigurationsregenerering, LUKS-mål låsa upp och bevakad målfilredigering körs genom den porterade hjälparen efter bekräftelse; Host Maintenance möjliggör samma stödda stadier inhemskt på det aktiva systemet efter att ha upprepat värdidentiteten och startkontrollerna.&lt;/p&gt;&lt;p&gt; De moderna funktionerna - verifierad File Copy, Btrfs snapshot rollback, EFI / UKI och extlinux reparation, boot-stack försoning och Make Default - är gråade med hjälparens egna sond skäl på denna frontend; Arch / Alpine / Fedora paket backends stanna diagnostik bara här. &lt;/p&gt;&lt;p&gt; Den första privilegierade åtgärden tillåter en cachad administratörssession per omfattning genom en dold ingångsmodal (Qt GUI förblir oprivilegierad). Det kan avslutas när som helst från File - Lock Administrator Session.&lt;/p&gt;</translation>
    </message>
</context>
</TS>
