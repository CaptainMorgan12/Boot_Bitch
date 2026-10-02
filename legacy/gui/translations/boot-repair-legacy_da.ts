<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS language="da">
<context>
    <name>LegacyMainWindow</name>
    <message>
        <source>Validate environment</source>
        <translation>Validér miljø</translation>
    </message>
    <message>
        <source>Validate</source>
        <translation>Valideret</translation>
    </message>
    <message>
        <source>Check the selected system&apos;s mounts, filesystem metadata, boot files, mapper consistency and dependency readiness before any repair action. This is an independent safety preflight rather than an optional Full Repair stage.</source>
        <translation>Kontroller det valgte systems ophæng, filsystem metadata, boot filer, mapper konsistens og afhængighed parathed før nogen reparation handling. Dette er en uafhængig sikkerhed før flyvning snarere end en valgfri Full Repair fase.</translation>
    </message>
    <message>
        <source>Always preflight</source>
        <translation>Altid før flyvning</translation>
    </message>
    <message>
        <source>File system repair</source>
        <translation>Filsystem reparation</translation>
    </message>
    <message>
        <source>Check File Systems</source>
        <translation>Kontrollér filsystemer</translation>
    </message>
    <message>
        <source>Run the read-only file system check for the selected system&apos;s root and /boot filesystems and report each device&apos;s check tool and result without changing anything. This legacy frontend exposes the read-only check only; device repair is not wired.</source>
        <translation>Kør den read- only filsystem check for det valgte systems root og / boot filsystemer og rapportere hver enheds check værktøj og resultat uden at ændre noget. Denne arv frontend udsætter read- only check kun; enhed reparation er ikke kappet.</translation>
    </message>
    <message>
        <source>Full Repair plan: unavailable on this frontend - read-only check only</source>
        <translation>Fuld reparationsplan: ikke tilgængelig på denne brugerflade - kun tjek</translation>
    </message>
    <message>
        <source>Complete package configuration</source>
        <translation>Komplet pakkekonfiguration</translation>
    </message>
    <message>
        <source>Complete Configuration</source>
        <translation>Fuldstændig indstilling</translation>
    </message>
    <message>
        <source>Complete interrupted dpkg package configuration in the selected repair system. This is the same stage controlled by Settings -&gt; Full Repair plan -&gt; Complete interrupted package configuration, but it can also be run independently here.</source>
        <translation>Komplet afbrudt dpkg pakkekonfiguration i det valgte reparationssystem. Dette er den samme fase styret af Indstillinger - &gt; Fuld reparationsplan - &gt; Fuldstændig afbrudt pakkekonfiguration, men det kan også køres uafhængigt her.</translation>
    </message>
    <message>
        <source>Run the guarded dpkg-configure repair? The helper keeps its package-lock and runtime preflights.</source>
        <translation>Køre den bevogtede dpkg- configure reparation? Hjælpen holder pakhus- og runtime-førflyvningerne.</translation>
    </message>
    <message>
        <source>Repair broken dependencies</source>
        <translation>Reparation af brudte afhængigheder</translation>
    </message>
    <message>
        <source>Repair Dependencies</source>
        <translation>Reparationsudgifter</translation>
    </message>
    <message>
        <source>Repair package dependencies in the selected repair system after the mandatory safety preflight. This maps directly to Settings -&gt; Repair broken package dependencies.</source>
        <translation>Reparation pakke afhængigheder i det valgte reparationssystem efter den obligatoriske sikkerhed før flyvning. Dette kort direkte til Indstillinger - &gt; Reparation brudt pakke afhængigheder.</translation>
    </message>
    <message>
        <source>Run the guarded fix-broken repair? The helper keeps its simulation-first preflight and runtime guards.</source>
        <translation>Køre den bevogtede fix- brudt reparation? Hjælpen beholder sin simulering- første før flyvning og runtime vagter.</translation>
    </message>
    <message>
        <source>Refresh package metadata</source>
        <translation>Genopfrisk pakkemetadata</translation>
    </message>
    <message>
        <source>Refresh Metadata</source>
        <translation>Genopfrisk metadata</translation>
    </message>
    <message>
        <source>Refresh APT metadata in the selected repair system without upgrading installed packages. This maps directly to Settings -&gt; Refresh package metadata.</source>
        <translation>Genopfrisk APT metadata i det valgte reparationssystem uden opgradering af installerede pakker. Dette kort direkte til Indstillinger - &gt; Genopfrisk pakkemetadata.</translation>
    </message>
    <message>
        <source>Refresh package metadata for the selected scope? The helper requires a reachable, trusted APT source.</source>
        <translation>Genopfrisk pakkemetadata for det valgte anvendelsesområde? Hjælpen kræver en pålidelig APT-kilde.</translation>
    </message>
    <message>
        <source>Upgrade installed packages</source>
        <translation>Opgrader installerede pakker</translation>
    </message>
    <message>
        <source>Simulate and Upgrade</source>
        <translation>Simulere og opgradere</translation>
    </message>
    <message>
        <source>Simulate the APT transaction first, inspect proposed removals, then apply a safe upgrade. This maps directly to Settings -&gt; Upgrade installed packages.</source>
        <translation>Simulere APT transaktion først, inspicer foreslåede optag, derefter anvende en sikker opgradering. Dette kort direkte til Indstillinger - &gt; Opgrader installerede pakker.</translation>
    </message>
    <message>
        <source>Run the guarded apt-upgrade transaction? The helper keeps its simulation-first and source guards.</source>
        <translation>Kør den bevogtede apt- opgradering transaktion? Hjælpen beholder sin simulering - først og kilde vagter.</translation>
    </message>
    <message>
        <source>DKMS</source>
        <translation>DKMS</translation>
    </message>
    <message>
        <source>Rebuild DKMS</source>
        <translation>Genopbyg DKMS</translation>
    </message>
    <message>
        <source>Rebuild out-of-tree kernel modules for kernels installed in the selected system. The helper refuses this action when DKMS is not installed; this legacy frontend exposes no DKMS action.</source>
        <translation>Genopbygge out- of- tree kernel moduler til kerner installeret i det valgte system. Hjælpen afviser denne handling, når DKMS ikke er installeret; denne arv viser ingen DKMS handling.</translation>
    </message>
    <message>
        <source>Graphical login / display manager</source>
        <translation>Grafisk login / display manager</translation>
    </message>
    <message>
        <source>Restore Graphical Login</source>
        <translation>Gendan grafisk login</translation>
    </message>
    <message>
        <source>Restore the legacy SysV display manager configured for the running host: the /etc/X11/default-display-manager entry and the missing runlevel S-symlink, with a backup and rollback, never starting the GUI. This is a host-scope stage on this legacy frontend.</source>
        <translation>Gendanne den gamle SysV skærm manager konfigureret til den kørende vært: / etc / X11 / default- display- manager indgang og den manglende runlevel S- symlink, med en backup og rollback, aldrig starte GUI. Dette er en host- scope scene på denne arv frontend.</translation>
    </message>
    <message>
        <source>Restore the graphical login configuration for the running host? The helper backs up /etc/X11/default-display-manager and the runlevel symlink state, restores the configured entry and the missing S-symlink, rolls back on any failure, and never starts the display manager.</source>
        <translation>Gendanne den grafiske login konfiguration for den kørende vært? Hjælpen bakker op / etc / X11 / default- display- manager og runlevel symlink tilstand, genopretter den indstillede indgang og den manglende S-symlink, ruller tilbage på enhver fejl, og aldrig starter display manager.</translation>
    </message>
    <message>
        <source>Initramfs</source>
        <translation>Citrusfrugter</translation>
    </message>
    <message>
        <source>Rebuild Initramfs</source>
        <translation>Genopbyg initramfs</translation>
    </message>
    <message>
        <source>Rebuild initramfs images for the selected repair system only after mapper and crypttab consistency checks pass. The helper backs up each image before the apply. On Etch the stage runs through the guarded plain-chroot fallback (no unshare required).</source>
        <translation>Genopbygge initramfs billeder til det valgte reparationssystem kun efter mapper og crypttab konsistens kontrol bestået. Hjælpen bakker hvert billede op før ansøgningen. På Etch scenen løber gennem den bevogtede sletter-chroot fallback (ingen unshare kræves).</translation>
    </message>
    <message>
        <source>Rebuild the initramfs for the selected scope? The helper keeps its mapper/crypttab and backup preflights.</source>
        <translation>Genopbygge initramfs for det valgte anvendelsesområde? Hjælpen holder sin mapper / crypttab og backup forflyvninger.</translation>
    </message>
    <message>
        <source>EFI / UKI bootloader</source>
        <translation>EFI / UKI bootloader</translation>
    </message>
    <message>
        <source>Repair EFI / UKI</source>
        <translation>Reparation af EFI / UKI</translation>
    </message>
    <message>
        <source>Repair the selected system&apos;s EFI / UKI boot path. This legacy frontend exposes no EFI action; the Etch target is a BIOS/GRUB-legacy system.</source>
        <translation>Reparer det valgte systems EFI / UKI startsti. Denne arv frontend udsætter ingen EFI handling; Etch målet er en BIOS / GRUB- arv system.</translation>
    </message>
    <message>
        <source>GRUB configuration</source>
        <translation>GRUB konfiguration</translation>
    </message>
    <message>
        <source>Regenerate GRUB</source>
        <translation>Regenerate GRUB</translation>
    </message>
    <message>
        <source>Regenerate the selected repair system&apos;s GRUB menu/configuration after the mandatory safety preflight. The helper backs up menu.lst, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Regenerér det valgte reparationssystems GRUB menu / konfiguration efter den obligatoriske sikkerhedsperiode. Hjælpen bakkes op menuerne, bevarer alle eksisterende boot entry og ruller tilbage på enhver fiasko.</translation>
    </message>
    <message>
        <source>Regenerate the GRUB configuration? The helper backs up the target menu/configuration, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Regenerere GRUB konfigurationen? Hjælpen bakker op målmenuen / konfigurationen, bevarer alle eksisterende boot indgang og ruller tilbage på enhver fejl.</translation>
    </message>
    <message>
        <source>extlinux configuration</source>
        <translation>extlinux- konfiguration</translation>
    </message>
    <message>
        <source>Regenerate extlinux</source>
        <translation>Regenerate extlinux</translation>
    </message>
    <message>
        <source>Regenerate the selected system&apos;s extlinux bootloader configuration. This legacy frontend exposes no extlinux action.</source>
        <translation>Regenerér det valgte systems extlinux bootloader konfiguration. Denne arv viser ingen extlinux handling.</translation>
    </message>
    <message>
        <source>Boot stack reconciliation</source>
        <translation>Boot stack forsoning</translation>
    </message>
    <message>
        <source>Reconcile Boot Stack</source>
        <translation>Reconcile Boot stak</translation>
    </message>
    <message>
        <source>Reconcile the selected repair system&apos;s boot stack in one guarded legacy pass: mapper/crypttab validation, initramfs rebuild and GRUB-legacy configuration regeneration, with the component backups and preflights unchanged. This is the Etch equivalent of the modern boot-stack reconciliation and stays out of the Full Repair plan.</source>
        <translation>Reconcile det valgte reparationssystems boot stack i en bevogtet arv pass: mapper / crypttab validering, initramfs genopbygge og GRUB- nedarvede konfiguration regenerering, med komponenten sikkerhedskopier og præ-flyvninger uændret. Dette er Etch svarer til den moderne boot- stack forsoning og forbliver ude af fuld reparation plan.</translation>
    </message>
    <message>
        <source>Full Repair plan: Manual recovery tool</source>
        <translation>Fuld reparation plan: Manuelt opsving værktøj</translation>
    </message>
    <message>
        <source>Run the guarded boot-stack reconciliation? The helper runs the mapper/crypttab validation, the initramfs rebuild and the GRUB-legacy regeneration in one pass with every component preflight and backup.</source>
        <translation>Køre den bevogtede bootstack forsoning? Hjælpen kører mapper / crypttab validering, initramfs genopbygge og GRUB-arv regenerering i et pass med hver komponent før flyvning og backup.</translation>
    </message>
    <message>
        <source>Complete interrupted package configuration</source>
        <translation>Komplette afbrudt pakkekonfiguration</translation>
    </message>
    <message>
        <source>Repair broken package dependencies</source>
        <translation>Reparer brudte pakkeafhængigheder</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Lav standard</translation>
    </message>
    <message>
        <source>Repair file system errors (read-only check first)</source>
        <translation>Reparer fil system fejl (read- kun tjek først)</translation>
    </message>
    <message>
        <source>the legacy frontend exposes the read-only file system check only; per-device repair is not wired on this frontend (fail closed)</source>
        <translation>den nedarvede frontend udsætter den read- only filsystem check kun; per- enhed reparation er ikke koblet på denne frontend (mislykkes lukket)</translation>
    </message>
    <message>
        <source>Upgrade installed packages (adaptive APT simulation)</source>
        <translation>Opgrader installerede pakker (adaptiv APT-simulering)</translation>
    </message>
    <message>
        <source>Rebuild DKMS modules</source>
        <translation>Genopbyg DKMS-moduler</translation>
    </message>
    <message>
        <source>Restore graphical login manager</source>
        <translation>Gendan grafisk loginhåndtering</translation>
    </message>
    <message>
        <source>Rebuild initramfs after mapper/crypttab validation</source>
        <translation>Genopbyg initramfs efter godkendelse af mapper / crypttab</translation>
    </message>
    <message>
        <source>Repair EFI / UKI boot path</source>
        <translation>Reparation EFI / UKI boot sti</translation>
    </message>
    <message>
        <source>Update GRUB configuration</source>
        <translation>Opdatér GRUB konfiguration</translation>
    </message>
    <message>
        <source>Update extlinux configuration</source>
        <translation>Opdatér extlinux- konfiguration</translation>
    </message>
    <message>
        <source>Choose Full Repair stages in Settings. Enabled stages run in the order shown. Each configurable stage also appears below as an individual tool; the Full Repair column mirrors its current Settings state. The boot tools (EFI / UKI bootloader, GRUB or extlinux configuration, boot-stack reconciliation and Make Default) are independent: run them in any order, and a later action re-verifies what an earlier one changed and reports its own result. The active scope is shown beside Repair: selected repair drive or Running Host maintenance.</source>
        <translation>Vælg fuld reparation stadier i Indstillinger. Aktiverede faser kører i den angivne rækkefølge. Hver konfigurerbar fase også vises nedenfor som et individuelt værktøj; Full Repair kolonne afspejler sin nuværende Indstillinger tilstand. De bootværktøjer (EFI / UKI bootloader, GRUB eller extlinux konfiguration, boot- stack forsoning og Make Standard) er uafhængige: køre dem i enhver rækkefølge, og en senere handling re- verificerer, hvad en tidligere ændret og rapporterer sit eget resultat. Det aktive omfang vises ved siden af Reparation: udvalgt reparation drev eller Running Host vedligeholdelse.</translation>
    </message>
    <message>
        <source>Run All diagnostics for the selected target or running host before starting Full Repair. The report is read-only evidence used to choose and confirm repair stages.</source>
        <translation>Kør Alle diagnostik for det valgte mål eller kørende vært, før du starter Fuld Reparation. Rapporten er read- kun beviser bruges til at vælge og bekræfte reparationsstadier.</translation>
    </message>
    <message>
        <source>Ready: required cached read-only diagnostics are available for the selected stages. Review them in Diagnostics or Logs before confirming.</source>
        <translation>Klar: krævede cache read- only diagnostik er tilgængelige for de valgte trin. Gennemgå dem i Diagnostik eller Logs før bekræftelse.</translation>
    </message>
    <message>
        <source>Environment validation</source>
        <translation>Miljøvalidering</translation>
    </message>
    <message>
        <source>Summarizes the selected system, protection state, mounted identity and inspection readiness.</source>
        <translation>Sammenfatter det valgte system, beskyttelsestilstand, monteret identitet og inspektionsberedskab.</translation>
    </message>
    <message>
        <source>Distribution and boot backend profile</source>
        <translation>Distribution og boot- backen- profil</translation>
    </message>
    <message>
        <source>Identifies the distribution family, package manager, initramfs generator, bootloader and current guarded repair capability.</source>
        <translation>Identificerer distribution familie, pakke manager, initramfs generator, bootloader og nuværende bevogtet reparation kapacitet.</translation>
    </message>
    <message>
        <source>Boot diagnostics</source>
        <translation>Boot diagnostik</translation>
    </message>
    <message>
        <source>Shows boot mounts and /boot contents plus storage evidence without changing the selected system.</source>
        <translation>Viser boot ophæng og / boot indhold plus lagerbeviser uden at ændre det valgte system.</translation>
    </message>
    <message>
        <source>Boot evidence and selection history</source>
        <translation>Boot dokumentation og udvælgelse historie</translation>
    </message>
    <message>
        <source>Correlates the detected boot chain, bootloader selection, kernel/initramfs and unlock evidence.</source>
        <translation>Sammenligner den detekterede bootkæde, valg af bootloader, kerne / initramfs og låse beviser op.</translation>
    </message>
    <message>
        <source>Kernel / initramfs</source>
        <translation>Kernel / initramfs</translation>
    </message>
    <message>
        <source>Reviews kernel files and verifies matching initramfs images through a read-only inspection.</source>
        <translation>Anmeldelser kerne filer og verificerer matchende initramfs billeder gennem en read- only inspektion.</translation>
    </message>
    <message>
        <source>Reviews the GRUB configuration without changing boot files.</source>
        <translation>Anmelder GRUB konfiguration uden at ændre boot-filer.</translation>
    </message>
    <message>
        <source>EFI / UKI boot state</source>
        <translation>EFI / UKI boot- tilstand</translation>
    </message>
    <message>
        <source>Inspects EFI/UKI evidence; unavailable on this legacy BIOS frontend with the helper&apos;s probe reason.</source>
        <translation>Kontrollerer EFI / UKI beviser; ikke tilgængelig på denne arv BIOS frontend med hjælperens sonde grund.</translation>
    </message>
    <message>
        <source>Reviews the configured display manager and recent boot evidence without starting the GUI.</source>
        <translation>Anmelder den konfigurerede display manager og seneste boot beviser uden at starte GUI.</translation>
    </message>
    <message>
        <source>Boot errors</source>
        <translation>Boot fejl</translation>
    </message>
    <message>
        <source>Reads recent error-priority entries from the running host or selected repair system when available.</source>
        <translation>Læser nylige errorprioriterede poster fra den kørende vært eller valgte reparationssystem, når de er tilgængelige.</translation>
    </message>
    <message>
        <source>Disk usage</source>
        <translation>Brug af disk</translation>
    </message>
    <message>
        <source>Summarizes filesystem capacity and free space for the running host or read-only repair target.</source>
        <translation>Opsummere filsystem kapacitet og ledig plads til den kørende vært eller read- kun reparation mål.</translation>
    </message>
    <message>
        <source>File systems</source>
        <translation>Filsystemer</translation>
    </message>
    <message>
        <source>Runs the read-only file system check for the selected system&apos;s root, /boot and other filesystems.</source>
        <translation>Kører den read- only filsystem check for det valgte systems root, / boot og andre filsystemer.</translation>
    </message>
    <message>
        <source>/etc/fstab review</source>
        <translation>/ etc / fstab review</translation>
    </message>
    <message>
        <source>Displays the running host or selected repair system&apos;s fstab; repair-system inspection is mounted read-only.</source>
        <translation>Viser den kørende vært eller det valgte reparationssystems fstab; inspektion af reparationssystemet er kun monteret read- only.</translation>
    </message>
    <message>
        <source>Btrfs status</source>
        <translation>Btrfs-status</translation>
    </message>
    <message>
        <source>Shows Btrfs filesystem and subvolume information when the target uses Btrfs.</source>
        <translation>Viser Btrfs filsystem og undervolumen information, når målet bruger Btrfs.</translation>
    </message>
    <message>
        <source>Device-mapper ancestry</source>
        <translation>Device- mapper herkomst</translation>
    </message>
    <message>
        <source>Shows selected mapper ancestry and device-mapper state when available.</source>
        <translation>Viser den valgte mapper herkomst og device- mapper tilstand når den er tilgængelig.</translation>
    </message>
    <message>
        <source>LUKS / crypttab evidence</source>
        <translation>LUKS / crypttab</translation>
    </message>
    <message>
        <source>Shows LUKS/mapped ancestry plus crypttab and fstab mapper references.</source>
        <translation>Viser LUKS / kortlagt herkomst plus crypttab og fstab mapper referencer.</translation>
    </message>
    <message>
        <source>Full diagnostic report</source>
        <translation>Fuld diagnostisk rapport</translation>
    </message>
    <message>
        <source>Combines all read-only diagnostics for the selected scope (same as Run All).</source>
        <translation>Kombinerer alle read- only diagnostik til det valgte anvendelsesområde (samme som Kør alle).</translation>
    </message>
    <message>
        <source>All entries</source>
        <translation>Alle indgange</translation>
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
        <translation>Pakkereparation</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Fil kopi</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Enhedsopdagelse</translation>
    </message>
    <message>
        <source>Host</source>
        <translation>Vært</translation>
    </message>
    <message>
        <source>Required for block-device inventory</source>
        <translation>Krævet for block- enhedsopgørelse</translation>
    </message>
    <message>
        <source>Filesystem identification</source>
        <translation>Identifikation af filsystem</translation>
    </message>
    <message>
        <source>Used to identify filesystem metadata</source>
        <translation>Bruges til at identificere filsystemmetadata</translation>
    </message>
    <message>
        <source>Mount inspection</source>
        <translation>Monteringsinspektion</translation>
    </message>
    <message>
        <source>Used to understand active mounts</source>
        <translation>Bruges til at forstå aktive monteringer</translation>
    </message>
    <message>
        <source>LUKS support</source>
        <translation>LUKS-understøttelse</translation>
    </message>
    <message>
        <source>Required to unlock encrypted targets</source>
        <translation>Krævet at låse krypterede mål op</translation>
    </message>
    <message>
        <source>Btrfs support</source>
        <translation>Btrfs-understøttelse</translation>
    </message>
    <message>
        <source>Required for Btrfs inspection and snapshot rollback</source>
        <translation>Krævet til Btrfs inspektion og snapshot rollback</translation>
    </message>
    <message>
        <source>Bidirectional file copy</source>
        <translation>Tovejs filkopi</translation>
    </message>
    <message>
        <source>Host/Repair</source>
        <translation>Vært / reparation</translation>
    </message>
    <message>
        <source>Required for verified Host-to-Repair and Repair-to-Host transfer</source>
        <translation>Krævet for verificeret Host-to- Repair og Repair-to- Host overførsel</translation>
    </message>
    <message>
        <source>Chroot repair</source>
        <translation>Krydderreparation</translation>
    </message>
    <message>
        <source>Required for target-side repair commands</source>
        <translation>Krævet for target- side reparation kommandoer</translation>
    </message>
    <message>
        <source>Offline systemd repair</source>
        <translation>Offline systemd reparation</translation>
    </message>
    <message>
        <source>Used to restore graphical.target and the configured display manager without starting the target GUI</source>
        <translation>Bruges til at gendanne grafisk. mål og den konfigurerede skærm manager uden at starte målet GUI</translation>
    </message>
    <message>
        <source>UEFI NVRAM inspection</source>
        <translation>UEFI NVRAM-inspektion</translation>
    </message>
    <message>
        <source>Used to preserve target EFI BootOrder during TUXEDO UKI rebuilds when efivars are available</source>
        <translation>Bruges til at bevare målet EFI BootOrder under TUXEDO UKI genopbygninger, når efivars er tilgængelige</translation>
    </message>
    <message>
        <source>UKI verification</source>
        <translation>UKI-verifikation</translation>
    </message>
    <message>
        <source>Used to verify the kernel embedded in a rebuilt unified kernel image</source>
        <translation>Bruges til at verificere kernen i et genopbygget samlet kernebillede</translation>
    </message>
    <message>
        <source>GRUB EFI repair</source>
        <translation>GRUB EFI reparation</translation>
    </message>
    <message>
        <source>Target/Host</source>
        <translation>Mål / vært</translation>
    </message>
    <message>
        <source>Required only for conventional GRUB-based EFI systems</source>
        <translation>Kræves kun for konventionelle GRUBbaserede EFI-systemer</translation>
    </message>
    <message>
        <source>Target</source>
        <translation>Mål</translation>
    </message>
    <message>
        <source>Debian-family GRUB helper</source>
        <translation>Familie GRUB hjælper</translation>
    </message>
    <message>
        <source>Portable GRUB configuration generator used by Arch and other non-Debian systems</source>
        <translation>Bærbar GRUB konfigurationsgenerator brugt af Arch og andre ikke-Debian-systemer</translation>
    </message>
    <message>
        <source>Initramfs rebuild</source>
        <translation>Initramfs genopbygning</translation>
    </message>
    <message>
        <source>Debian-family initramfs helper</source>
        <translation>Familie initramfs hjælper</translation>
    </message>
    <message>
        <source>Arch-family initramfs generator</source>
        <translation>Arch- familie initramfs generator</translation>
    </message>
    <message>
        <source>Alternative initramfs generator used by Arch and other distributions</source>
        <translation>Alternativ initramfs generator brugt af Arch og andre distributioner</translation>
    </message>
    <message>
        <source>Initramfs verification</source>
        <translation>Verifikation af initramfs</translation>
    </message>
    <message>
        <source>Read-only verification for mkinitcpio images</source>
        <translation>Read- only verification for mkinitcpio-billeder</translation>
    </message>
    <message>
        <source>Read-only verification for dracut images</source>
        <translation>Read- only verifikation for dracut-billeder</translation>
    </message>
    <message>
        <source>systemd-boot inspection</source>
        <translation>systemd-boot-inspektion</translation>
    </message>
    <message>
        <source>Host/Target</source>
        <translation>Vært / mål</translation>
    </message>
    <message>
        <source>Read-only inspection of systemd-boot and generic UKI layouts</source>
        <translation>Read- kun inspektion af systemd-boot og generiske UKI layout</translation>
    </message>
    <message>
        <source>Arch package manager</source>
        <translation>Arch pakkehåndtering</translation>
    </message>
    <message>
        <source>Arch-family package database and transaction tool</source>
        <translation>Arch- family pakkedatabase og transaktionsværktøj</translation>
    </message>
    <message>
        <source>DKMS rebuild</source>
        <translation>Genopbygning af DKMS</translation>
    </message>
    <message>
        <source>Required only when target uses DKMS modules</source>
        <translation>Kræves kun, når målet bruger DKMS moduler</translation>
    </message>
    <message>
        <source>LVM inspection</source>
        <translation>LVM-inspektion</translation>
    </message>
    <message>
        <source>Optional LVM storage-stack support</source>
        <translation>Valgfri LVM storage- stack support</translation>
    </message>
    <message>
        <source>Software RAID</source>
        <translation>Software RAID</translation>
    </message>
    <message>
        <source>Optional Linux MD RAID support</source>
        <translation>Valgfri Linux MD RAID-støtte</translation>
    </message>
    <message>
        <source>Process namespace isolation</source>
        <translation>Behandlingsnavnetolerance</translation>
    </message>
    <message>
        <source>Host+Target</source>
        <translation>Vært + Mål</translation>
    </message>
    <message>
        <source>The ported helper falls back to a guarded plain chroot when unshare is absent</source>
        <translation>Den porterede hjælper falder tilbage til en bevogtet almindelig chromot, når unshare er fraværende</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Annullér</translation>
    </message>
    <message>
        <source>OK</source>
        <translation>OK</translation>
    </message>
    <message>
        <source>Yes</source>
        <translation>Ja</translation>
    </message>
    <message>
        <source>No</source>
        <translation>Nej</translation>
    </message>
    <message>
        <source>read-only helper diagnostic</source>
        <translation>read- only helper diagnosis</translation>
    </message>
</context>
<context>
    <name>legacy::LegacyMainWindow</name>
    <message>
        <source>Boot Bitch Legacy (Etch / KDE 3.5 era)</source>
        <translation>Boot Bitch Legacy (Etch / KDE 3.5 æra)</translation>
    </message>
    <message>
        <source>Linux recovery and boot-repair utility</source>
        <translation>Linux opsving og boot- reparation nytte</translation>
    </message>
    <message>
        <source>GUARDED REPAIR</source>
        <translation>SIKKERHEDSSTILLELSE</translation>
    </message>
    <message>
        <source>Ordinary repairs require an explicitly selected non-host target. The protected running host has a separate deliberate maintenance mode with the same guarded repair stages and requires privilege authorization.</source>
        <translation>Almindelige reparationer kræver en eksplicit valgt ikke-vært mål. Den beskyttede løbende vært har en separat bevidst vedligeholdelsesfunktion med de samme bevogtede reparationsstadier og kræver privilegeret tilladelse.</translation>
    </message>
    <message>
        <source>Systems</source>
        <translation>Systemer</translation>
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
        <translation>Fil Kopiér</translation>
    </message>
    <message>
        <source>Logs</source>
        <translation>Logge</translation>
    </message>
    <message>
        <source>Settings</source>
        <translation>Indstillinger</translation>
    </message>
    <message>
        <source>(not created yet)</source>
        <translation>(ikke skabt endnu)</translation>
    </message>
    <message>
        <source>Information</source>
        <translation>Oplysninger</translation>
    </message>
    <message>
        <source>Close</source>
        <translation>Luk</translation>
    </message>
    <message>
        <source>&amp;File</source>
        <translation>- &amp;fil</translation>
    </message>
    <message>
        <source>&amp;Refresh Devices</source>
        <translation>&amp;Genopfrisk enheder</translation>
    </message>
    <message>
        <source>&amp;Lock Administrator Session</source>
        <translation>- &amp;låseadministrator session</translation>
    </message>
    <message>
        <source>&amp;Quit</source>
        <translation>&amp;Afslut</translation>
    </message>
    <message>
        <source>&amp;View</source>
        <translation>- &amp;visning</translation>
    </message>
    <message>
        <source>&amp;Systems</source>
        <translation>- &amp;systemer</translation>
    </message>
    <message>
        <source>&amp;Diagnostics</source>
        <translation>&amp;Diagnostics</translation>
    </message>
    <message>
        <source>&amp;Logs</source>
        <translation>&amp;Logs</translation>
    </message>
    <message>
        <source>&amp;Settings</source>
        <translation>- &amp;indstillinger</translation>
    </message>
    <message>
        <source>&amp;Auto-size Device Columns</source>
        <translation>&amp;Autosize enhedskolonner</translation>
    </message>
    <message>
        <source>&amp;Wrap Log Lines</source>
        <translation>- &amp;wrap- loglinjer</translation>
    </message>
    <message>
        <source>&amp;Help</source>
        <translation>- &amp;hjælp</translation>
    </message>
    <message>
        <source>&amp;Using Boot Bitch</source>
        <translation>&amp;bruger Boot Bitch</translation>
    </message>
    <message>
        <source>&amp;About Boot Bitch</source>
        <translation>&amp;Om Boot Bitch</translation>
    </message>
    <message>
        <source>Device columns auto-sized. Drag headers to fine-tune widths.</source>
        <translation>Enhedskolonner automatisk størrelse. Træk headere til fine- tune bredder.</translation>
    </message>
    <message>
        <source>A helper command is running; wait for it to finish before locking the session.</source>
        <translation>En hjælpekommando kører; vent til den er færdig før du låser sessionen.</translation>
    </message>
    <message>
        <source>Administrator session locked; the next privileged action will request authorization.</source>
        <translation>Administrator session låst; den næste privilegerede handling vil anmode om godkendelse.</translation>
    </message>
    <message>
        <source>Using Boot Bitch</source>
        <translation>Brug af Boot Bitch</translation>
    </message>
    <message>
        <source>Boot Bitch must run from a different booted Linux environment than the system being repaired. Use a Linux live medium or another Linux installation on a different physical drive.&lt;br&gt;&lt;br&gt;The running host is protected from ordinary repair-target selection, but it can be explicitly selected through &lt;b&gt;Host Maintenance&lt;/b&gt; for guarded native diagnostics and supported maintenance stages.&lt;br&gt;&lt;br&gt;Diagnostics follow the Systems page: the committed repair drive while Host Maintenance is off, or the protected running host while it is active.&lt;br&gt;&lt;br&gt;The first privileged action requests administrator authorization once for this Boot Bitch window; &lt;b&gt;File - Lock Administrator Session&lt;/b&gt; ends that helper session immediately. Every repair keeps the helper&apos;s own runtime preflights.</source>
        <translation>Boot Bitch skal køre fra et andet startet Linux-miljø end det system, der repareres. Brug et Linux levende medie eller en anden Linux-installation på et andet fysisk drev. &lt; br &gt; &lt; br &gt; Den kørende vært er beskyttet mod almindelige valg af reparations- mål, men den kan eksplicit vælges gennem &lt; b &gt; Vært Vedligeholdelse &lt; / b &gt; for bevogtet indfødte diagnostik og understøttede vedligeholdelsesstadier. &lt; br &gt; &lt; br &gt; Diagnostik følger siden Systemer: den engagerede reparation drev, mens Vært Vedligeholdelse er slukket, eller den beskyttede kører vært, mens det er aktivt. &lt; br &gt; &lt; br &gt; Den første privilegerede handling anmoder administrator godkendelse én gang for dette Boot Bitch vindue; &lt; b &gt; File - Lock Administrator Session &lt; / b &gt; slutter denne hjælper session med det samme. Hver reparation holder hjælperens egne runtime-fly.</translation>
    </message>
    <message>
        <source>Select a physical drive; Boot Bitch resolves the most likely Linux system volume automatically. The running host stays protected from ordinary target repairs, with a separate explicit host-maintenance path for its own system. The Details button shows the protected host&apos;s facts in the details pane; selecting any drive row restores the per-drive pane.</source>
        <translation>Vælg et fysisk drev; Boot Bitch løser den mest sandsynlige Linux-system volumen automatisk. Den løbende vært forbliver beskyttet mod almindelige mål reparationer, med en separat eksplicit host-vedligeholdelse sti til sit eget system. Knappen Detaljer viser den beskyttede værts fakta i detaljer ruden; valg af drev række genopretter per- drev ruden.</translation>
    </message>
    <message>
        <source>Refresh Devices</source>
        <translation>Genopfrisk enheder</translation>
    </message>
    <message>
        <source>Re-read the read-only kernel inventory (/proc/partitions, /proc/mounts, /proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* and the udev metadata database). No block device is opened and nothing is written.</source>
        <translation>Re- read the read- only kernel inventory (/ proc / partitions, / proc / mounts, / proc / swaps, / sys / block, / dev / mapper, / dev / disk / by- * and the udev metadata database). Ingen blok enhed er åbnet og intet er skrevet.</translation>
    </message>
    <message>
        <source>[OK]</source>
        <translation>[OK]</translation>
    </message>
    <message>
        <source>The running system was detected and stays protected from ordinary target repairs.</source>
        <translation>Det løbende system blev opdaget og forbliver beskyttet mod almindelige mål reparationer.</translation>
    </message>
    <message>
        <source>Detecting running system...</source>
        <translation>Detektering af løbesystem...</translation>
    </message>
    <message>
        <source>Detecting protected storage...</source>
        <translation>Detektering af beskyttet opbevaring...</translation>
    </message>
    <message>
        <source>PROTECTED</source>
        <translation>BESKYTTELSE</translation>
    </message>
    <message>
        <source>The running host remains protected from ordinary repair-target operations.</source>
        <translation>Den løbende vært forbliver beskyttet mod almindelige reparationsmål operationer.</translation>
    </message>
    <message>
        <source>Details</source>
        <translation>Detaljer</translation>
    </message>
    <message>
        <source>Show read-only details for the protected running host.</source>
        <translation>Vis kun detaljer for den beskyttede kørende vært.</translation>
    </message>
    <message>
        <source>Host Maintenance</source>
        <translation>Vært Vedligeholdelse</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Lav standard</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host (menu.lst default directive with a backup and rollback). Requires Host Maintenance and the cached host-default probe.</source>
        <translation>Gør den kanoniske installerede kerne indgang standard GRUB- legacy boot indgang på den kørende vært (menu.lst standard direktiv med en backup og rollback). Kræver host vedligeholdelse og cachet host-standard sonde.</translation>
    </message>
    <message>
        <source>Available repair targets</source>
        <translation>Tilgængelige reparationsmål</translation>
    </message>
    <message>
        <source>Most likely first</source>
        <translation>Sandsynligvis først.</translation>
    </message>
    <message>
        <source>Drives are listed by the read-only inventory. Select a row to inspect it; Select Target commits the selected non-host drive with its auto-resolved Linux root component.</source>
        <translation>Driver er opført på den read- only inventar. Vælg en række til at inspicere den; Vælg Target forpligter den valgte ikke-vært drev med sin autoløst Linux root komponent.</translation>
    </message>
    <message>
        <source>Device</source>
        <translation>Enhed</translation>
    </message>
    <message>
        <source>Size</source>
        <translation>Størrelse</translation>
    </message>
    <message>
        <source>Type</source>
        <translation>Type</translation>
    </message>
    <message>
        <source>Filesystem</source>
        <translation>Filsystem</translation>
    </message>
    <message>
        <source>Select Target</source>
        <translation>Vælg mål</translation>
    </message>
    <message>
        <source>Unlock</source>
        <translation>Lås op</translation>
    </message>
    <message>
        <source>Authorize</source>
        <translation>Godkend</translation>
    </message>
    <message>
        <source>Establish the privileged helper session for the current scope now instead of waiting for the next privileged action.</source>
        <translation>Etablere den privilegerede hjælpesesession for den nuværende rækkevidde nu i stedet for at vente på den næste privilegerede handling.</translation>
    </message>
    <message>
        <source>Committed target: none</source>
        <translation>Målsætning: ingen</translation>
    </message>
    <message>
        <source>Unlock state for the selected drive; the LUKS passphrase is never logged.</source>
        <translation>Lås tilstand op for det valgte drev; LUKS passfrase er aldrig logget.</translation>
    </message>
    <message>
        <source>Unlock status</source>
        <translation>Lås status op</translation>
    </message>
    <message>
        <source>Read-only inventory plus helper-confirmed facts; mirrors the modern Qt6 Selected drive details panel.</source>
        <translation>Read- only inventar plus helper- bekræftede fakta; afspejler den moderne Qt6 Udvalgte drev detaljer panel.</translation>
    </message>
    <message>
        <source>Selected drive details</source>
        <translation>Udvalgte drev</translation>
    </message>
    <message>
        <source>Field</source>
        <translation>Felt</translation>
    </message>
    <message>
        <source>Value</source>
        <translation>Værdi</translation>
    </message>
    <message>
        <source>Run All runs every available read-only diagnostic for the current scope; selecting a check runs it alone. Diagnostics are read-only and are the only evidence source for the gated repair actions.</source>
        <translation>Kør Alle kører alle tilgængelige read- only diagnostik for det aktuelle omfang; at vælge en check kører det alene. Diagnostics er read- kun og er den eneste dokumentation kilde til de gated reparation handlinger.</translation>
    </message>
    <message>
        <source>Target: none selected</source>
        <translation>Mål: ingen valgt</translation>
    </message>
    <message>
        <source>Diagnostics follow the committed repair target, or the protected running host while Host Maintenance is active.</source>
        <translation>Diagnostik følger det engagerede reparationsmål, eller den beskyttede løbende vært, mens Vært Vedligeholdelse er aktiv.</translation>
    </message>
    <message>
        <source>Run All</source>
        <translation>Kør alle</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope.</source>
        <translation>Kør All - kør alle tilgængelige read- only diagnostik for det aktuelle anvendelsesområde.</translation>
    </message>
    <message>
        <source>Target configuration:</source>
        <translation>Målkonfiguration:</translation>
    </message>
    <message>
        <source>Etch-era target configuration files; availability is probed read-only by the helper&apos;s diagnostics.</source>
        <translation>Etch- æra mål konfigurationsfiler; tilgængelighed er provided read- kun af helpens diagnostik.</translation>
    </message>
    <message>
        <source>Edit Target File...</source>
        <translation>Redigér målfil...</translation>
    </message>
    <message>
        <source>Runs one read-only diagnostic for the selected scope through the helper (`diagnose &lt;key&gt;` / `host-diagnose &lt;key&gt;`); Run All is the combined report.</source>
        <translation>Kører en read- only diagnose for det valgte anvendelsesområde gennem hjælper (&apos;diagnostice &lt; key &gt;&apos; / &apos;host- diagnostice &lt; key &gt;&apos;); Kør Alt er den kombinerede rapport.</translation>
    </message>
    <message>
        <source>Diagnostic checks</source>
        <translation>Diagnostisk kontrol</translation>
    </message>
    <message>
        <source>Check</source>
        <translation>Tjek</translation>
    </message>
    <message>
        <source>Selected diagnostic</source>
        <translation>Udvalgte diagnoser</translation>
    </message>
    <message>
        <source>Select a diagnostic</source>
        <translation>Vælg en diagnose</translation>
    </message>
    <message>
        <source>Choose a diagnostic from the list.</source>
        <translation>Vælg en diagnose fra listen.</translation>
    </message>
    <message>
        <source>Ready</source>
        <translation>Klar</translation>
    </message>
    <message>
        <source>Results</source>
        <translation>Resultater</translation>
    </message>
    <message>
        <source>Run Diagnostic</source>
        <translation>Kør diagnostisk</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic for the current scope.</source>
        <translation>Kør Diagnostik - kør den valgte read- only diagnostik for det aktuelle anvendelsesområde.</translation>
    </message>
    <message>
        <source>Copy Results</source>
        <translation>Kopiér resultater</translation>
    </message>
    <message>
        <source>Save Results...</source>
        <translation>Gem resultater...</translation>
    </message>
    <message>
        <source>The Full Repair plan runs the selected legacy stages in order through the guarded helper; the individual tools run one stage at a time. Every action stays disabled until the cached capability lines say available and the helper keeps its runtime preflights.</source>
        <translation>Full Repair planen kører de valgte nedarvede stadier i rækkefølge gennem den bevogtede hjælper; de enkelte værktøjer kører et trin ad gangen. Hver handling forbliver deaktiveret, indtil cached kapacitet linjer siger til rådighed, og hjælperen holder sin runtime præ-flyvninger.</translation>
    </message>
    <message>
        <source>Full Repair plan</source>
        <translation>Fuld reparationsplan</translation>
    </message>
    <message>
        <source>No stages selected</source>
        <translation>Ingen trin valgt</translation>
    </message>
    <message>
        <source>Configure Plan...</source>
        <translation>Indstil plan...</translation>
    </message>
    <message>
        <source>Open Settings to choose which Full Repair stages are part of the plan.</source>
        <translation>Åbn Indstillinger for at vælge, hvilke fuld reparation faser er en del af planen.</translation>
    </message>
    <message>
        <source>Run Full Repair</source>
        <translation>Kør fuld reparation</translation>
    </message>
    <message>
        <source>Select a repair drive, or choose Host Maintenance on the protected running-host card.</source>
        <translation>Vælg en reparation drev, eller vælg Vært Vedligeholdelse på den beskyttede running- host kort.</translation>
    </message>
    <message>
        <source>Stage</source>
        <translation>Fase</translation>
    </message>
    <message>
        <source>Individual repair tools</source>
        <translation>Individuelle reparationsværktøjer</translation>
    </message>
    <message>
        <source>Tool</source>
        <translation>Værktøj</translation>
    </message>
    <message>
        <source>Full Repair</source>
        <translation>Fuld reparation</translation>
    </message>
    <message>
        <source>not reported</source>
        <translation>ikke indberettet</translation>
    </message>
    <message>
        <source>Selected tool</source>
        <translation>Valgt værktøj</translation>
    </message>
    <message>
        <source>Select a repair tool</source>
        <translation>Vælg et reparationsværktøj</translation>
    </message>
    <message>
        <source>Run Tool</source>
        <translation>Kør værktøj</translation>
    </message>
    <message>
        <source>Select a tool to review its repair action.</source>
        <translation>Vælg et værktøj til at gennemgå sin reparation handling.</translation>
    </message>
    <message>
        <source>Write actions ask for confirmation and then run the helper&apos;s own runtime preflights; the GUI never weakens them. A repair that is not proven &apos;unchanged&apos; invalidates the cached diagnostics and disables the gated actions until diagnostics run again.</source>
        <translation>Skriv handlinger bede om bekræftelse og derefter køre hjælperens egne runtime pre-flyrejser; GUI aldrig svækker dem. En reparation, der ikke er bevist &quot;uændret&quot; invaliderer cached diagnostik og deaktiverer de gatede handlinger, indtil diagnostik kører igen.</translation>
    </message>
    <message>
        <source>Chroot shell</source>
        <translation>Chroot shell</translation>
    </message>
    <message>
        <source>Offline commands run one at a time in a fresh chroot and cannot answer interactive prompts (apt-get -y upgrade works). Host-shell commands run directly on the running host. The helper&apos;s probe lines gate the command field; the exact reason appears in the tooltip.</source>
        <translation>Offline kommandoer kører en ad gangen i en frisk chromot og kan ikke besvare interaktive prompts (apt- get - y opgraderingsværker). Host- shell kommandoer kører direkte på den kørende vært. Hjælperens sonde linjer låser kommandofeltet; den nøjagtige årsag vises i værktøjstippet.</translation>
    </message>
    <message>
        <source>No &apos;Legacy feature shell:&apos; line is cached; run diagnostics for the selected scope to evaluate the helper&apos;s chroot/timeout containment probes (fail closed).</source>
        <translation>Ingen &apos;Legacy feature shell:&apos; linje er cachet; køre diagnostik for det valgte omfang til at evaluere helperens chrom- / timeout indeslutning prober (mislykkes lukket).</translation>
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
        <translation>En revideret kommandostreng, passeret til hjælper som et enkelt argument (ingen skal interpolation af GUI).</translation>
    </message>
    <message>
        <source>Run Command</source>
        <translation>Kør kommando</translation>
    </message>
    <message>
        <source>Clear Output</source>
        <translation>Ryd uddata</translation>
    </message>
    <message>
        <source>The helper exposes `shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for an offline target chroot and `host-shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for the running host. Both keep the helper&apos;s runtime preflights; this tab enables the command only when the scope is committed, the session is authorized and the scope&apos;s Legacy feature probe reports available.</source>
        <translation>Den hjælper udsætter &apos;shell &lt; disk &gt; &lt; root &gt; &lt; kommando &gt;&apos; for en offline mål chromot og &apos;host- shell &lt; disk &gt; &lt; root &gt; &lt; kommando &gt;&apos; for den kørende vært. Begge holde hjælperens runtime pre-flight; denne fane gør det muligt for kommandoen kun, når anvendelsesområdet er forpligtet, sessionen er godkendt, og omfanget Legacy funktion sonde rapporter til rådighed.</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Fil kopi</translation>
    </message>
    <message>
        <source>Copy and verify files in either direction through the guarded helper (cp -a plus ownership restoration and a per-file byte-compare). The helper&apos;s file-copy probe gates the controls and keeps the direction and path containment checks.</source>
        <translation>Kopiér og verificer filer i begge retninger gennem den bevogtede hjælper (cp -a plus ejerskab restaurering og en per- fil byte- sammenligne). Hjælperens filkopisonde låser kontrollerne og holder kontrol af retning og sti.</translation>
    </message>
    <message>
        <source>Preview Changes</source>
        <translation>Forhåndsvis ændringer</translation>
    </message>
    <message>
        <source>Run a copy dry-run through the guarded helper. No files are changed.</source>
        <translation>Kør en kopi-løb gennem den bevogtede hjælper. Ingen filer er ændret.</translation>
    </message>
    <message>
        <source>Copy and Verify</source>
        <translation>Kopiér og verificer</translation>
    </message>
    <message>
        <source>Copy staged items and verify the result. Existing destination names are overwritten when source content differs; unrelated destination files are never deleted.</source>
        <translation>Kopiér iscenesatte elementer og verificer resultatet. Eksisterende destinationsnavne overskrives, når kildeindholdet er forskelligt; ikke-relaterede destinationsfiler slettes aldrig.</translation>
    </message>
    <message>
        <source>Direction:</source>
        <translation>Retning:</translation>
    </message>
    <message>
        <source>Choose which system supplies the source files and which system receives them.</source>
        <translation>Vælg hvilket system der leverer kildefilerne, og hvilket system der modtager dem.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from this host</source>
        <translation>1. Vælg kildefiler eller mapper fra denne vært</translation>
    </message>
    <message>
        <source>Source</source>
        <translation>Kilde</translation>
    </message>
    <message>
        <source>Files and folders staged for the verified copy. The legacy backend copies with cp -a and restores ownership with chown --reference; every regular file is byte-compared after the copy.</source>
        <translation>Filer og mapper iscenesat for den verificerede kopi. Den arv backend kopier med cp -a og genskaber ejerskab med chown -- reference; hver almindelig fil er byte- sammenlignet efter kopien.</translation>
    </message>
    <message>
        <source>Add Files...</source>
        <translation>Tilføj filer...</translation>
    </message>
    <message>
        <source>Add Folder...</source>
        <translation>Tilføj mappe...</translation>
    </message>
    <message>
        <source>Remove</source>
        <translation>Fjern</translation>
    </message>
    <message>
        <source>Clear</source>
        <translation>Ryd</translation>
    </message>
    <message>
        <source>Clear the staged source list (nothing is copied or deleted).</source>
        <translation>Ryd den iscenesatte kildeliste (intet kopieres eller slettes).</translation>
    </message>
    <message>
        <source>2. Choose destination in repaired system</source>
        <translation>2. Vælg destination i repareret system</translation>
    </message>
    <message>
        <source>unavailable: see the helper&apos;s Legacy feature file-copy: probe reason above</source>
        <translation>Utilgængelig: se hjælperens Legacy funktion file- copy: sonde årsag ovenfor</translation>
    </message>
    <message>
        <source>An absolute path inside the selected repair system (Host to Repair) or on the running host (Repair to Host).</source>
        <translation>En absolut sti inde i det valgte reparationssystem (Vært til reparation) eller på den kørende vært (Reparer til Vært).</translation>
    </message>
    <message>
        <source>Browse Target Folders...</source>
        <translation>Gennemse Målmapper...</translation>
    </message>
    <message>
        <source>Browse the selected repair system through the helper&apos;s temporary read-only mounts and choose an absolute destination path. No target files are changed while browsing.</source>
        <translation>Gennemse det valgte reparationssystem gennem hjælperens midlertidige read- only mounts og vælg en absolut destination sti. Ingen målfiler ændres under browsing.</translation>
    </message>
    <message>
        <source>3. Ownership and copy policy</source>
        <translation>3. Ejendomsret og kopipolitik</translation>
    </message>
    <message>
        <source>Ownership:</source>
        <translation>Ejerskab:</translation>
    </message>
    <message>
        <source>Smart destination ownership (recommended)</source>
        <translation>Smart destination ejerskab (anbefales)</translation>
    </message>
    <message>
        <source>Preserve source numeric UID/GID</source>
        <translation>Reservekilde numerisk UID / GID</translation>
    </message>
    <message>
        <source>Smart mode validates UID/GID identity mapping across the two systems and falls back to the destination-directory owner when the same numeric ID means a different account (the legacy backend implements it with chown --reference).</source>
        <translation>Smart mode validerer UID / GID identitet kortlægning på tværs af de to systemer og falder tilbage til destination- mappeejer, når det samme numeriske ID betyder en anden konto (den nedarvede backend implementerer det med chown -- reference).</translation>
    </message>
    <message>
        <source>Application log</source>
        <translation>Programlog</translation>
    </message>
    <message>
        <source>The complete session register; Save As... writes every entry even while a filter hides lines. If a writable system share mount exists at /host, Save As... starts there; otherwise the log directory is the fallback. Prior session files are listed read-only.</source>
        <translation>Den komplette session register; Gem som... skriver hver indgang, selv mens et filter skjuler linjer. Hvis en skrivbar systemdelingsmount eksisterer på / vært, Gem som... starter der; ellers logmappen er fallback. Forudgående sessionsfiler er kun listet med.</translation>
    </message>
    <message>
        <source>Session logs</source>
        <translation>Sessionslogfiler</translation>
    </message>
    <message>
        <source>Session</source>
        <translation>Session</translation>
    </message>
    <message>
        <source>The first entry is the live session; earlier files in the log directory are listed read-only below it.</source>
        <translation>Den første indgang er den levende session; tidligere filer i logmappen er angivet read- kun under det.</translation>
    </message>
    <message>
        <source>New Session Log</source>
        <translation>Ny session- log</translation>
    </message>
    <message>
        <source>Close the active session file; it becomes a prior session and the next log entry starts a new file.</source>
        <translation>Luk den aktive session fil; det bliver en forudgående session og den næste log indgang starter en ny fil.</translation>
    </message>
    <message>
        <source>Add Note</source>
        <translation>Tilføj note</translation>
    </message>
    <message>
        <source>Append a NOTE entry to the live session register.</source>
        <translation>Tilføj en NOTE indgang til live session registret.</translation>
    </message>
    <message>
        <source>Delete</source>
        <translation>Slet</translation>
    </message>
    <message>
        <source>Delete the selected prior session file (the live session is never deleted).</source>
        <translation>Slet den valgte forudgående session fil (den levende session slettes aldrig).</translation>
    </message>
    <message>
        <source>Refresh</source>
        <translation>Genopfrisk</translation>
    </message>
    <message>
        <source>Save As...</source>
        <translation>Gem som...</translation>
    </message>
    <message>
        <source>Save the complete session log (all entries, not just the current filter).</source>
        <translation>Gem hele sessionsloggen (alle indgange, ikke kun det aktuelle filter).</translation>
    </message>
    <message>
        <source>Clear Register</source>
        <translation>Ryd register</translation>
    </message>
    <message>
        <source>Clear the live register and view; prior session files are never modified.</source>
        <translation>Ryd live register og visning; tidligere session filer ændres aldrig.</translation>
    </message>
    <message>
        <source>Search log:</source>
        <translation>Søgelog:</translation>
    </message>
    <message>
        <source>Type any characters to show matching log entries (case-insensitive). Save As always writes every entry.</source>
        <translation>Skriv eventuelle tegn for at vise matchende logindgange (case- insensitive). Gem Som altid skriver hver indgang.</translation>
    </message>
    <message>
        <source>Filter:</source>
        <translation>Filter:</translation>
    </message>
    <message>
        <source>Filter the visible log by entry kind. Selecting a diagnostic section shows the lines captured for that section; a workflow filter such as File system repair or Package repair shows its mapped repair lines (File copy has no lines in this frontend). Save As always writes every entry.</source>
        <translation>Filtrer den synlige log efter entry art. Valg af en diagnostisk sektion viser de linjer, der er fanget for dette afsnit; et workflow filter såsom File system reparation eller pakke reparation viser sine kortlagte reparation linjer (File copy har ingen linjer i denne frontend). Gem Som altid skriver hver indgang.</translation>
    </message>
    <message>
        <source>Settings are stored per user under ~/.qt/, one file per settings group (devicesrc, logsrc, diagnosticsrc, repairrc), and are saved immediately on every change and on close. Launch the GUI as the same user to keep your overrides; a GUI started as root keeps its own copies.</source>
        <translation>Indstillinger gemmes pr. bruger under ~ / .qt /, en fil pr. indstillingsgruppe (devicesrc, logsrc, diagnosticsrc, repairrc), og gemmes straks på hver ændring og på tæt hold. Start GUI som den samme bruger til at holde dine overrides; en GUI startede som root holder sine egne kopier.</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Enhedsopdagelse</translation>
    </message>
    <message>
        <source>Show devices without an identified Linux installation</source>
        <translation>Vis enheder uden en identificeret Linux-installation</translation>
    </message>
    <message>
        <source>Show removable and USB storage</source>
        <translation>Vis aftagelig og USB opbevaring</translation>
    </message>
    <message>
        <source>Show encrypted devices before unlocking</source>
        <translation>Vis krypterede enheder før åbning</translation>
    </message>
    <message>
        <source>When off, drives without a visible Linux filesystem are hidden unless they still contain an encrypted device and encrypted devices are shown.</source>
        <translation>Når de slukkes, er drev uden et synligt Linux-filsystem skjult, medmindre de stadig indeholder en krypteret enhed, og krypterede enheder vises.</translation>
    </message>
    <message>
        <source>When off, removable and USB drives are hidden from the repair-target list.</source>
        <translation>Når slukket, flytbare og USB-drev er skjult fra reparationsmållisten.</translation>
    </message>
    <message>
        <source>When off, drives with an encrypted device are hidden until the volume is unlocked.</source>
        <translation>Når slukket, drev med en krypteret enhed er skjult, indtil lydstyrken er ulåst.</translation>
    </message>
    <message>
        <source>Include the %1 stage in the Full Repair plan. The stage runs in the plan order shown on the Repair tab.</source>
        <translation>Inkludér %1 fase i fuld reparation plan. Scenen kører i planen rækkefølge vist på knappen Reparer.</translation>
    </message>
    <message>
        <source>Automatically regenerate read-only diagnostics after repairs or target changes</source>
        <translation>Automatisk regenerere read- only diagnostik efter reparationer eller mål ændringer</translation>
    </message>
    <message>
        <source>Regenerates the cached read-only diagnostics for the current scope after an operation that invalidates them (LUKS unlock, target configuration edit). It runs only inside an already authorized administrator session and never opens an authorization prompt by itself.</source>
        <translation>Regenererer cachen read- only diagnostik for det nuværende omfang efter en operation, der ugyldiggør dem (LUKS låse, mål konfiguration redigering). Det kører kun inden for en allerede autoriseret administrator session og aldrig åbner en tilladelse prompt af sig selv.</translation>
    </message>
    <message>
        <source>Wrap long log lines</source>
        <translation>Wrap lange loglinjer</translation>
    </message>
    <message>
        <source>Mandatory safety controls</source>
        <translation>Obligatorisk sikkerhedskontrol</translation>
    </message>
    <message>
        <source>Host capabilities and dependencies</source>
        <translation>Værtskapacitet og afhængighed</translation>
    </message>
    <message>
        <source>Refresh Capabilities</source>
        <translation>Genopfrisk kapaciteter</translation>
    </message>
    <message>
        <source>Re-run the read-only host capability probes (a PATH search, nothing is executed) and refresh the distribution summary.</source>
        <translation>Kør de read- only vært kapacitet prober (en PATH søgning, intet er udført) og opdatere distributionen resumé.</translation>
    </message>
    <message>
        <source>Install Missing Support...</source>
        <translation>Installér manglende understøttelse...</translation>
    </message>
    <message>
        <source>Automatic installation will require explicit package mapping and privilege authorization.</source>
        <translation>Automatisk installation vil kræve eksplicitte pakkekortlægning og privilegeret godkendelse.</translation>
    </message>
    <message>
        <source>Application configuration</source>
        <translation>Programkonfiguration</translation>
    </message>
    <message>
        <source>Boot Bitch Legacy</source>
        <translation>Boot Bitch Legacy</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list first.</source>
        <translation>Vælg et fysisk drev i listen til rådighed reparation mål først.</translation>
    </message>
    <message>
        <source>Protected system</source>
        <translation>Beskyttet system</translation>
    </message>
    <message>
        <source>The running system cannot be selected as a repair target. Use Host Maintenance for the protected running host or choose another disk.</source>
        <translation>Det kørende system kan ikke vælges som reparationsmål. Brug Vært Vedligeholdelse til den beskyttede kørende vært eller vælg en anden disk.</translation>
    </message>
    <message>
        <source>Unlock or select a Linux system first</source>
        <translation>Lås op eller vælg et Linux-system først</translation>
    </message>
    <message>
        <source>This encrypted drive has no visible Linux filesystem yet. Use Unlock, refresh devices, and select the target after its Linux root is detected.</source>
        <translation>Dette krypterede drev har endnu ingen synlig Linux-filsystem. Brug Lås op, opdater enheder, og vælg målet efter dens Linux root er opdaget.</translation>
    </message>
    <message>
        <source>The selected root component (%1) belongs to the running system and cannot be committed as a repair target. Use Host Maintenance for the protected running host.</source>
        <translation>Den valgte rodkomponent (%1) tilhører det kørende system og kan ikke bruges som reparationsmål. Brug Vært Vedligeholdelse til den beskyttede kørende vært.</translation>
    </message>
    <message>
        <source>repair target commit</source>
        <translation>reparation mål engagement</translation>
    </message>
    <message>
        <source>Repair drive selected: %1</source>
        <translation>Reparationsdrev valgt: %1</translation>
    </message>
    <message>
        <source>; best detected system component: %1</source>
        <translation>; bedst detekterede systemkomponent: %1</translation>
    </message>
    <message>
        <source>. No mount or repair action was performed.</source>
        <translation>. Ingen montering eller reparation handling blev udført.</translation>
    </message>
    <message>
        <source>Make Default unavailable</source>
        <translation>Gør standard utilgængelig</translation>
    </message>
    <message>
        <source>Make Default is a running-host action on this frontend; enter Host Maintenance first.</source>
        <translation>Gør Standard er en running- host handling på denne brugerflade; indtaste Vært Vedligeholdelse først.</translation>
    </message>
    <message>
        <source>Administrator authorization required</source>
        <translation>Krævet fuldmagt</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize first.</source>
        <translation>Ingen administrator session er aktiv; tryk på Godkend først.</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host?

The helper verifies /boot/grub/menu.lst, sets the `default &lt;N&gt;` directive to the canonical entry, backs the menu up first and restores it on any failure.</source>
        <translation>Gør den kanoniske installerede kernel indgang standard GRUB- legacy boot indgang på den kørende vært?

Hjælpen verificerer / boot / grub / menu1, sætter &quot;standard &lt; N &gt;&quot; direktivet til den kanoniske indgang, bakker menuen op først og genskaber det på enhver fejl.</translation>
    </message>
    <message>
        <source>Host Maintenance unavailable</source>
        <translation>Værtsvedligeholdelse ikke tilgængelig</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a committed repair target or a detected running host.</source>
        <translation>Det kørende værtsmål kunne ikke detekteres; diagnostik har brug for et engageret reparationsmål eller en detekteret kørende vært.</translation>
    </message>
    <message>
        <source>Exit Host Maintenance</source>
        <translation>Exit Værtsvedligeholdelse</translation>
    </message>
    <message>
        <source>Host Maintenance is active; diagnostics and gated repairs target the protected running host.</source>
        <translation>Vært Vedligeholdelse er aktiv; diagnostik og gated reparationer mål den beskyttede kører vært.</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list on the Systems tab first, or use Host Maintenance for the protected running host.</source>
        <translation>Vælg et fysisk drev i listen Tilgængelige reparationsmål på fanebladet Systemer først, eller brug Vært Vedligeholdelse til den beskyttede kørende vært.</translation>
    </message>
    <message>
        <source>The selected drive (%1) is the protected running host. Choose Host Maintenance on the Systems tab to run read-only host diagnostics and guarded host repairs; ordinary target repairs stay disabled.</source>
        <translation>Det valgte drev (%1) er den beskyttede kørende vært. Vælg Vært Vedligeholdelse på fanebladet Systemer til at køre read- only vært diagnostik og bevogtet vært reparationer; almindelige mål reparationer ophold deaktiveret.</translation>
    </message>
    <message>
        <source>No repair target is committed. Choose Select Target on the Systems tab (or Host Maintenance for the protected running host) first.</source>
        <translation>Der er ingen reparationsmål. Vælg Vælg mål på fanebladet Systemer (eller Vært Vedligeholdelse for den beskyttede kørende vært) først.</translation>
    </message>
    <message>
        <source>The selection changed after the target was committed. Choose Select Target again.</source>
        <translation>Udvælgelsen blev ændret, efter at målet var indgået. Vælg Vælg mål igen.</translation>
    </message>
    <message>
        <source>Diagnostics scope required</source>
        <translation>Diagnostisk anvendelsesområde</translation>
    </message>
    <message>
        <source>Diagnostics scope unresolved</source>
        <translation>Diagnostisk anvendelsesområde uløst</translation>
    </message>
    <message>
        <source>The running host target could not be resolved; use Refresh Devices and commit a repair target or re-enter Host Maintenance.</source>
        <translation>Den kørende vært mål kunne ikke løses; bruge Genopfrisk enheder og begå en reparation mål eller re- indtaste Vært Vedligeholdelse.</translation>
    </message>
    <message>
        <source>Diagnostic check required</source>
        <translation>Diagnostisk kontrol påkrævet</translation>
    </message>
    <message>
        <source>Select a diagnostic check in the list first.</source>
        <translation>Vælg først en diagnostisk kontrol i listen.</translation>
    </message>
    <message>
        <source>Repair target required</source>
        <translation>Reparationsmål påkrævet</translation>
    </message>
    <message>
        <source>Target file editing needs a committed repair target. Running-host maintenance has no target-file editing; commit an offline target first.</source>
        <translation>Målfil redigering har brug for en engageret reparation mål. Running- host vedligeholdelse har ingen target- fil redigering; begå et offline mål først.</translation>
    </message>
    <message>
        <source>Configuration file required</source>
        <translation>Indstillingsfil påkrævet</translation>
    </message>
    <message>
        <source>Select a target configuration file first.</source>
        <translation>Vælg en målindstillingsfil først.</translation>
    </message>
    <message>
        <source>Edit target %1</source>
        <translation>Redigér mål %1</translation>
    </message>
    <message>
        <source>Edit this target file through the guarded administrator helper. A successful save invalidates cached diagnostics; rerun diagnostics before repair. Generated files such as /boot/grub/menu.lst may be replaced by the next bootloader update.</source>
        <translation>Redigér denne målfil gennem den bevogtede administrator hjælper. En vellykket gemme ugyldiggør cachet diagnostik; genkøre diagnostik før reparation. Genererede filer såsom / boot / grub / menu.lst kan erstattes af den næste bootloader opdatering.</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Annullér</translation>
    </message>
    <message>
        <source>Save Target File</source>
        <translation>Gem målfil</translation>
    </message>
    <message>
        <source>No changes to %1.</source>
        <translation>Ingen ændringer til %1.</translation>
    </message>
    <message>
        <source>Configuration write refused</source>
        <translation>Indstillingsskrivning nægtet</translation>
    </message>
    <message>
        <source>The edited content contains NUL bytes; the guarded write refuses it.</source>
        <translation>Det redigerede indhold indeholder NUL bytes; den bevogtede skrive nægter det.</translation>
    </message>
    <message>
        <source>File too large</source>
        <translation>Fil for stor</translation>
    </message>
    <message>
        <source>The edited file is larger than 1 MiB. The guarded write refuses it; edit the file from a console instead.</source>
        <translation>Den redigerede fil er større end 1 MiB. Den bevogtede skrive nægter det; redigere filen fra en konsol i stedet.</translation>
    </message>
    <message>
        <source>Write target configuration</source>
        <translation>Skriv målkonfiguration</translation>
    </message>
    <message>
        <source>Write the edited contents to %1? This modifies the repair target and invalidates cached diagnostics.</source>
        <translation>Skrive det redigerede indhold til %1? Dette ændrer reparation mål og ugyldiggør cached diagnostik.</translation>
    </message>
    <message>
        <source>No diagnostic results to copy yet.</source>
        <translation>Ingen diagnostiske resultater at kopiere endnu.</translation>
    </message>
    <message>
        <source>Diagnostic results copied to the clipboard.</source>
        <translation>Diagnostiske resultater kopieret til udklipsholderen.</translation>
    </message>
    <message>
        <source>No diagnostic results to save yet.</source>
        <translation>Ingen diagnostiske resultater endnu.</translation>
    </message>
    <message>
        <source>Text files (*.txt);;All files (*)</source>
        <translation>Tekstfiler (*. txt); Alle filer (*)</translation>
    </message>
    <message>
        <source>Save diagnostic results</source>
        <translation>Gem diagnostiske resultater</translation>
    </message>
    <message>
        <source>Could not write %1.</source>
        <translation>Kunne ikke skrive %1.</translation>
    </message>
    <message>
        <source>Diagnostic results saved to %1</source>
        <translation>Diagnostiske resultater gemt til %1</translation>
    </message>
    <message>
        <source>Unlock not available</source>
        <translation>Lås ikke tilgængelig</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked. Select an offline repair target to unlock.</source>
        <translation>Den beskyttede løbevært kan ikke låses op. Vælg en offline reparation mål at låse op.</translation>
    </message>
    <message>
        <source>No locked LUKS component is currently visible on this selected drive.</source>
        <translation>Ingen låst LUKS komponent er i øjeblikket synlig på dette valgte drev.</translation>
    </message>
    <message>
        <source>Confirm LUKS unlock</source>
        <translation>Bekræft LUKS- låsning</translation>
    </message>
    <message>
        <source>Unlock %1 on %2?

The helper opens a temporary device-mapper mapping with cryptsetup and keeps it open for this recovery session. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>Lås %1 op på %2?

Hjælpen åbner en midlertidig device- mapper kortlægning med cryptsetup og holder den åben for denne recovery session. Passphrase rejser gennem en privat nøglefil og er aldrig placeret i kommando argumenter eller logs.</translation>
    </message>
    <message>
        <source>LUKS unlock</source>
        <translation>LUKS- låsning</translation>
    </message>
    <message>
        <source>Unlock LUKS repair target</source>
        <translation>Lås LUKS reparationsmål op</translation>
    </message>
    <message>
        <source>Enter the passphrase for %1.

It is sent only to cryptsetup over the helper&apos;s standard input and is never logged or placed on a command line.</source>
        <translation>Indtast kodeordet for %1.

Det sendes kun til kryptsetup over hjælperens standard input og er aldrig logget eller placeret på en kommandolinje.</translation>
    </message>
    <message>
        <source>Passphrase required</source>
        <translation>Passersætning påkrævet</translation>
    </message>
    <message>
        <source>An empty passphrase was not submitted. Enter the LUKS passphrase or choose Cancel.</source>
        <translation>Der blev ikke indgivet en tom passerseddel. Indtast LUKS kodeord eller vælg Annullér.</translation>
    </message>
    <message>
        <source>Unlock keyfile unavailable</source>
        <translation>Lås nøglefil utilgængelig</translation>
    </message>
    <message>
        <source>The LUKS passphrase could not be written to a private keyfile in %1; the unlock was not started.</source>
        <translation>LUKS passfrase kunne ikke skrives til en privat nøglefil i %1; låsen blev ikke startet.</translation>
    </message>
    <message>
        <source>Configuration write unavailable</source>
        <translation>Indstillingsskrivning ikke tilgængelig</translation>
    </message>
    <message>
        <source>The edited content could not be written to a private temporary file in %1; the write was not started.</source>
        <translation>Det redigerede indhold kunne ikke skrives til en privat midlertidig fil i %1; skriften blev ikke startet.</translation>
    </message>
    <message>
        <source>ERROR: cannot read the session log %1</source>
        <translation>ERROR: kan ikke læse sessionsloggen %1</translation>
    </message>
    <message>
        <source>Viewing a prior session log (read-only): %1</source>
        <translation>Visning af en tidligere session log (read- only): %1</translation>
    </message>
    <message>
        <source>Session log list refreshed.</source>
        <translation>Sessionslogliste genopfrisket.</translation>
    </message>
    <message>
        <source>Note:</source>
        <translation>Bemærk:</translation>
    </message>
    <message>
        <source>Delete session log</source>
        <translation>Slet sessionslog</translation>
    </message>
    <message>
        <source>Delete %1 permanently? This cannot be undone.</source>
        <translation>Slet %1 permanent? Det kan ikke gøres om.</translation>
    </message>
    <message>
        <source>%1 changed while the confirmation was open; the delete was refused.</source>
        <translation>%1 ændrede sig, mens bekræftelsen var åben; sletningen blev afvist.</translation>
    </message>
    <message>
        <source>Unable to delete %1.</source>
        <translation>Kan ikke slette %1.</translation>
    </message>
    <message>
        <source>Device discovery filters updated.</source>
        <translation>Enhedssøgningsfiltre opdateret.</translation>
    </message>
    <message>
        <source>Unknown Linux distribution</source>
        <translation>Ukendt Linux distribution</translation>
    </message>
    <message>
        <source>sudo / gksu (no KAuth on this frontend)</source>
        <translation>sudo / gksu (ingen KAuth på denne front)</translation>
    </message>
    <message>
        <source>Available</source>
        <translation>Tilgængelig</translation>
    </message>
    <message>
        <source>Missing on this frontend</source>
        <translation>Mangler på denne grænseflade</translation>
    </message>
    <message>
        <source>Missing</source>
        <translation>Mangler</translation>
    </message>
    <message>
        <source>No repair tool is selected.</source>
        <translation>Ingen reparation værktøj er valgt.</translation>
    </message>
    <message>
        <source>A helper command is already running.</source>
        <translation>En hjælpekommando kører allerede.</translation>
    </message>
    <message>
        <source>Restore Graphical Login is a host-scope stage on this legacy frontend; enter Host Maintenance to run it.</source>
        <translation>Gendan Graphical Login er en host- scope scene på denne arv frontend; indtaste Værtsvedligeholdelse til at køre det.</translation>
    </message>
    <message>
        <source>The selected scope has no resolved root component; use Refresh Devices and commit the repair target again.</source>
        <translation>Det valgte anvendelsesområde har ingen løst rodkomponent; brug Genopfrisk enheder og begå reparation mål igen.</translation>
    </message>
    <message>
        <source>This legacy frontend exposes no %1 action; the helper reports the capability as available.</source>
        <translation>Denne arv frontend afslører ingen %1 handling; hjælper rapporterer kapaciteten som tilgængelig.</translation>
    </message>
    <message>
        <source>Run this guarded repair action using the cached read-only diagnostic evidence. A confirmation is shown first.</source>
        <translation>Kør denne bevogtet reparation handling ved hjælp af cache read- kun diagnostiske beviser. En bekræftelse vises først.</translation>
    </message>
    <message>
        <source>Enabled in Settings</source>
        <translation>Aktiveret i Indstillinger</translation>
    </message>
    <message>
        <source>Disabled in Settings - enable it to include this stage</source>
        <translation>Deaktiveret i Indstillinger - gør det muligt at inkludere denne fase</translation>
    </message>
    <message>
        <source>Unavailable: %1</source>
        <translation>Utilgængelig: %1</translation>
    </message>
    <message>
        <source>
Unavailable: %1</source>
        <translation>
Utilgængelig: %1</translation>
    </message>
    <message>
        <source>Repair tool unavailable</source>
        <translation>Reparation værktøj ikke tilgængelig</translation>
    </message>
    <message>
        <source>Confirm repair</source>
        <translation>Bekræft reparation</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Your saved selection is kept and its availability is re-checked when diagnostics for this scope complete.</source>
        <translation>Ingen beviser for denne fase endnu. Dit gemte valg holdes, og dets tilgængelighed kontrolleres igen, når diagnostik for dette anvendelsesområde er komplet.</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Run diagnostics for the selected scope to populate the Full Repair plan; your selection is saved once the stage becomes available.</source>
        <translation>Ingen beviser for denne fase endnu. Kør diagnostik for det valgte omfang til at befolke fuld reparation plan; dit valg gemmes, når scenen bliver tilgængelig.</translation>
    </message>
    <message>
        <source>Unknown Full Repair stage.</source>
        <translation>Ukendt fuld reparation fase.</translation>
    </message>
    <message>
        <source>No Full Repair stages are selected or available; use Configure Plan... to choose the stages.</source>
        <translation>Ingen fuld reparation faser er valgt eller tilgængelige; brug Indstil plan... for at vælge de stadier.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab to establish the session.</source>
        <translation>Administrator godkendelse er påkrævet; tryk på Godkend på Systemer eller Reparer fanen for at etablere sessionen.</translation>
    </message>
    <message>
        <source>Runs the selected stages using the cached read-only diagnostic evidence after privilege confirmation.</source>
        <translation>Kører de valgte stadier ved hjælp af cached read- only diagnostisk bevis efter bekræftelse af privilegier.</translation>
    </message>
    <message>
        <source>No Full Repair stages selected - use Configure Plan... or Settings.</source>
        <translation>Ingen komplette reparationsstadier valgt - brug Indstil plan... eller indstillinger.</translation>
    </message>
    <message>
        <source>1 stage selected</source>
        <translation>1 trin valgt</translation>
    </message>
    <message>
        <source>%1 stages selected</source>
        <translation>%1-trin valgt</translation>
    </message>
    <message>
        <source>No repair stages are selected. Use Configure Plan... to choose the stages Full Repair will run.</source>
        <translation>Ingen reparationstrin er valgt. Brug Indstil plan... til at vælge de stadier Fuld Reparation vil køre.</translation>
    </message>
    <message>
        <source>No selected stages are available. %1</source>
        <translation>Ingen udvalgte trin er tilgængelige. %1</translation>
    </message>
    <message>
        <source>Full Repair unavailable</source>
        <translation>Fuld Reparation utilgængelig</translation>
    </message>
    <message>
        <source>Run the Full Repair plan?

The selected stages run in order through the helper&apos;s guarded repair command:

</source>
        <translation>Køre hele reparationsplanen?

De valgte stadier kører i rækkefølge gennem hjælperens bevogtede reparation kommando:

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
Hjælpen holder hver runtime før flyvning; en fase, der mislykkes stopper planen.</translation>
    </message>
    <message>
        <source>Running %1 through the privileged helper... The Logs tab keeps the complete transcript.</source>
        <translation>Kører %1 gennem den privilegerede hjælper... Fanebladet Logs holder hele udskriften.</translation>
    </message>
    <message>
        <source>Shell scope required</source>
        <translation>Shell anvendelsesområde</translation>
    </message>
    <message>
        <source>No administrator session is active.

Press Authorize on the Systems or Repair tab to establish the session, or enter Host Maintenance / commit a repair target on the Systems tab; the chroot shell then reuses the cached authorization.</source>
        <translation>Ingen administrator session er aktiv.

Tryk på Godkend på fanebladet Systemer eller Reparer for at oprette sessionen, eller indtast Værtsvedligeholdelse / lav et reparationsmål på fanebladet Systemer; kromatskallen genbruger derefter den cachede godkendelse.</translation>
    </message>
    <message>
        <source>Shell command required</source>
        <translation>Shell- kommando påkrævet</translation>
    </message>
    <message>
        <source>Enter the command to run first.</source>
        <translation>Indtast kommandoen til at køre først.</translation>
    </message>
    <message>
        <source>Confirm running-host command</source>
        <translation>Bekræft køring- værtskommando</translation>
    </message>
    <message>
        <source>Run this command as root on the protected running host?

%1

The helper keeps its runtime preflights; the command is passed as one argument and is never interpreted by the GUI.</source>
        <translation>Kør denne kommando som root på den beskyttede kørende vært?

%1

Hjælpen holder sine runtime forflyvninger; kommandoen er bestået som et argument og er aldrig fortolkes af GUI.</translation>
    </message>
    <message>
        <source>Running %1...</source>
        <translation>Kører %1...</translation>
    </message>
    <message>
        <source>Authorization scope required</source>
        <translation>Krævet godkendelsesomfang</translation>
    </message>
    <message>
        <source>administrator authorization</source>
        <translation>administrator authorisation</translation>
    </message>
    <message>
        <source>Administrator authorization</source>
        <translation>Fuldmægtig</translation>
    </message>
    <message>
        <source>Administrator authorization is required for %1.

Enter the password for %2 (sudo). It is used only for this sudo authentication, is sent over a pipe and is never logged or placed on a command line. The authorization is cached for this session and reused by diagnostics and repairs.</source>
        <translation>Der kræves fuldmagt til %1.

Indtast kodeordet for %2 (sudo). Det bruges kun til denne sudo godkendelse, sendes over et rør og er aldrig logget eller placeret på en kommandolinje. Tilladelsen er cached for denne session og genbruges af diagnostik og reparationer.</translation>
    </message>
    <message>
        <source>your account</source>
        <translation>din konto</translation>
    </message>
    <message>
        <source>Password required</source>
        <translation>Adgangskode påkrævet</translation>
    </message>
    <message>
        <source>An empty password was not submitted. Enter the sudo password or choose Cancel.</source>
        <translation>En tom adgangskode blev ikke indsendt. Indtast sudo- kodeordet eller vælg Annullér.</translation>
    </message>
    <message>
        <source>Administrator authorization failed</source>
        <translation>Administratortilladelse mislykkedes</translation>
    </message>
    <message>
        <source>sudo did not accept the password: %1

The command was not started.</source>
        <translation>sudo ikke acceptere password: %1

Kommandoen var ikke startet.</translation>
    </message>
    <message>
        <source>elevation requires an interactive sudo password; run the smoke as root or after `sudo -S -v` with --elevate &apos;sudo -n&apos;</source>
        <translation>elevation kræver en interaktiv sudo adgangskode; køre røgen som root eller efter &apos;sudo - S - v&apos; med -- elevate &apos;sudo - n&apos;</translation>
    </message>
    <message>
        <source>No administrator session is active for %1.

Press Authorize on the Systems or Repair tab to establish the session now, or enter Host Maintenance / commit a repair target on the Systems tab; diagnostics and repairs then reuse the cached authorization.</source>
        <translation>Ingen administrator session er aktiv for %1.

Tryk på Godkend på fanebladet Systemer eller Reparer for at etablere sessionen nu, eller indtast Værtsvedligeholdelse / commit en reparation mål på fanebladet Systemer; diagnostik og reparationer derefter genbruge den cache godkendelse.</translation>
    </message>
    <message>
        <source>Administrator authorization expired</source>
        <translation>Administratortilladelse udløbet</translation>
    </message>
    <message>
        <source>The cached administrator authorization for %1 expired or was refused.

Press Authorize on the Systems or Repair tab to re-establish the session, then run the command again. No command was started.</source>
        <translation>Den cache administrator tilladelse til %1 udløb eller blev afvist.

Tryk på Godkend på fanebladet Systemer eller Reparer for at genetablere sessionen, og kør kommandoen igen. Ingen kommando blev startet.</translation>
    </message>
    <message>
        <source>Privileged operation completed successfully. Administrator authorization remains active for this session.</source>
        <translation>Opretholdt operation gennemført med succes. Administratortilladelse forbliver aktiv i denne session.</translation>
    </message>
    <message>
        <source>Privileged operation stopped with an error. Administrator authorization remains active; review the output before closing.</source>
        <translation>Operationen stoppede med en fejl. Administrator godkendelse forbliver aktiv; gennemgå produktionen før lukning.</translation>
    </message>
    <message>
        <source>Inspection is read-only.

</source>
        <translation>Inspektionen er klar.

</translation>
    </message>
    <message>
        <source>Configuration unavailable</source>
        <translation>Indstilling ikke tilgængelig</translation>
    </message>
    <message>
        <source>The helper could not read %1:

%2</source>
        <translation>Hjælpen kunne ikke læse %1:

%2</translation>
    </message>
    <message>
        <source>Configuration write failed</source>
        <translation>Indstillingsskrivning mislykkedes</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
Result: mapping opened for this recovery session.</source>
        <translation>Tilstand: ulåst
Komponent: %1
Mapper: %2
Metode: helper unlock (cryptsetup; passfrase via en mode- 600 keyfile, slettet efter brug)
Resultat: kortlægning åbnet for denne inddrivelse session.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: the passphrase was not accepted; retry offered.</source>
        <translation>Stat: låst
Komponent: %1
Metode: helper unlock (kryptsetup)
Fejl: Adgangsfrasen blev ikke accepteret; prøv igen tilbudt.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: %2</source>
        <translation>Stat: låst
Komponent: %1
Metode: helper unlock (kryptsetup)
Fejl: %2</translation>
    </message>
    <message>
        <source>Passphrase not accepted</source>
        <translation>Passersætning ikke accepteret</translation>
    </message>
    <message>
        <source>The LUKS passphrase was not accepted.

Try again?</source>
        <translation>LUKS passfrase blev ikke accepteret.

Prøv igen?</translation>
    </message>
    <message>
        <source>[-] %1 - not run (the plan stopped before reaching this stage)</source>
        <translation>[-] %1 - ikke køre (planen stoppet, før du når dette stadium)</translation>
    </message>
    <message>
        <source>filesystem</source>
        <translation>filsystem</translation>
    </message>
    <message>
        <source>[-] %1 - no file system errors found - no changes</source>
        <translation>[-] %1 - ingen filsystemfejl fundet - ingen ændringer</translation>
    </message>
    <message>
        <source>[FAIL] %1 - not reported</source>
        <translation>[FAIL] %1 - ikke indberettet</translation>
    </message>
    <message>
        <source>%1 stage(s) failed; review the helper output in Logs.</source>
        <translation>%1 fase (r) mislykkedes; gennemgå hjælpeuddata i Logs.</translation>
    </message>
    <message>
        <source>the plan stopped before any stage completed.</source>
        <translation>planen blev stoppet, før nogen fase var afsluttet.</translation>
    </message>
    <message>
        <source>no repair was needed; cached diagnostics remain valid.</source>
        <translation>ingen reparation var nødvendig; cached diagnostik forbliver gyldig.</translation>
    </message>
    <message>
        <source>repair completed; cached diagnostics were invalidated and must be regenerated.</source>
        <translation>reparation afsluttet; cached diagnostik blev ugyldig og skal regenereres.</translation>
    </message>
    <message>
        <source>%1 results: [OK] %2 successful | [FAIL] %3 failed | [-] %4 no repair needed - %5</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Re-run Diagnostic</source>
        <translation>Ret- run Diagnostic</translation>
    </message>
    <message>
        <source>Unavailable</source>
        <translation>Utilgængelig</translation>
    </message>
    <message>
        <source>Commit this physical drive as the repair target.</source>
        <translation>Lav dette fysiske drev som reparationsmål.</translation>
    </message>
    <message>
        <source>Protection:</source>
        <translation>Beskyttelse:</translation>
    </message>
    <message>
        <source>Host shell</source>
        <translation>Værtsskal</translation>
    </message>
    <message>
        <source>Run on Host</source>
        <translation>Kør på vært</translation>
    </message>
    <message>
        <source>Host Shell</source>
        <translation>Vært Shell</translation>
    </message>
    <message>
        <source>The read-only probe found no editable target configuration file in this target. %1</source>
        <translation>Den read- only sonde fundet ingen redigerbare mål konfiguration fil i dette mål. %1</translation>
    </message>
    <message>
        <source>Not present in the selected target (omitted): %1</source>
        <translation>Ikke til stede i det valgte mål (udeladt): %1</translation>
    </message>
    <message>
        <source>Run diagnostics to probe which target configuration files exist; the helper&apos;s read-only probe decides the list.</source>
        <translation>Kør diagnostik for at sonde hvilke målkonfigurationsfiler der findes; hjælperens read- only sonde bestemmer listen.</translation>
    </message>
    <message>
        <source>Probed read-only by the helper; a saved edit invalidates the cached diagnostics.</source>
        <translation>Probet read- kun af hjælper; en gemt redigere ugyldiggør cached diagnostik.</translation>
    </message>
    <message>
        <source>Host maintenance: %1</source>
        <translation>Vært vedligeholdelse: %1</translation>
    </message>
    <message>
        <source>unresolved</source>
        <translation>Uafsluttet</translation>
    </message>
    <message>
        <source>Target: %1 + %2</source>
        <translation>Mål: %1 + %2</translation>
    </message>
    <message>
        <source>Host command</source>
        <translation>Værtskommando</translation>
    </message>
    <message>
        <source>Execute a command on the running host as root.</source>
        <translation>Udfør en kommando på den kørende vært som root.</translation>
    </message>
    <message>
        <source>Execute a command inside the selected repair system as root.</source>
        <translation>Kør en kommando inde i det valgte reparationssystem som root.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab (or re-enter Host Maintenance / re-commit the repair target) to authorize this session.</source>
        <translation>Administrator godkendelse er påkrævet; tryk på Godkend på Systemer eller Reparation fanen (eller re- indtaste Værts Vedligeholdelse / re- forpligte reparation mål) for at tillade denne session.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root on the running host through the helper&apos;s guarded host-shell verb.</source>
        <translation>Kør en revideret kommando som root på den kørende vært gennem hjælperens bevogtet host-shell verbum.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root inside the target chroot through the helper&apos;s guarded shell verb.</source>
        <translation>Kør en revideret kommando som root inde i målet chromot gennem helpens bevogtede shell verbum.</translation>
    </message>
    <message>
        <source>Run a command on the running host as root (sudo is not needed). Commands are executed directly on the active system; output is kept in this window and in the application log.</source>
        <translation>Kør en kommando på den kørende vært som root (sudo er ikke nødvendigt). Kommandoer udføres direkte på det aktive system; output holdes i dette vindue og i programloggen.</translation>
    </message>
    <message>
        <source>Run a command inside the selected repair system as root (sudo is not needed). Commands are executed one at a time in a fresh chroot and cannot answer interactive prompts; use non-interactive flags such as apt-get -y upgrade. Output is kept in this window and in the application log.</source>
        <translation>Kør en kommando inde i det valgte reparationssystem som root (sudo er ikke nødvendigt). Kommandoer udføres en ad gangen i en frisk chromot og kan ikke besvare interaktive prompts; bruge ikke-interaktive flag såsom apt- get -y opgradering. Output holdes i dette vindue og i programloggen.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize or re-enter Host Maintenance / commit a repair target.</source>
        <translation>Ingen administrator session er aktiv; tryk på Godkend eller genindtast Vært Vedligeholdelse / begå en reparation mål.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from the repaired system</source>
        <translation>1. Vælg kildefiler eller mapper fra det reparerede system</translation>
    </message>
    <message>
        <source>2. Choose destination on this host</source>
        <translation>2. Vælg destination på denne vært</translation>
    </message>
    <message>
        <source>Add File Path...</source>
        <translation>Tilføj Fil sti...</translation>
    </message>
    <message>
        <source>Add Folder Path...</source>
        <translation>Tilføj mappesti...</translation>
    </message>
    <message>
        <source>Browse...</source>
        <translation>Gennemse...</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it (Host Maintenance does not provide a repair tree to browse).</source>
        <translation>Vælg reparation drev i Systems, før du vælger en destination inde i det (Vært Vedligeholdelse giver ikke en reparation træ til at gennemse).</translation>
    </message>
    <message>
        <source>Choose a host destination folder directly.</source>
        <translation>Vælg en modtagermappe direkte.</translation>
    </message>
    <message>
        <source>Copy the staged files and folders with cp -a, restore ownership with chown --reference and byte-compare every regular file afterwards.</source>
        <translation>Kopier de iscenesatte filer og mapper med cp -a, gendanne ejerskab med chown -- reference og byte- sammenligne hver almindelig fil bagefter.</translation>
    </message>
    <message>
        <source>File Copy unavailable</source>
        <translation>Fil Kopiér ikke tilgængelig</translation>
    </message>
    <message>
        <source>Choose host destination folder</source>
        <translation>Vælg værtsdestinationsmappe</translation>
    </message>
    <message>
        <source>Select a repair target</source>
        <translation>Vælg et reparationsmål</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it.</source>
        <translation>Vælg reparation drev i Systems, før du vælger en destination inde i det.</translation>
    </message>
    <message>
        <source>File Copy - Browse Target Folders</source>
        <translation>Fil Kopiér - Gennemse målmapper</translation>
    </message>
    <message>
        <source>Browse Target Folders</source>
        <translation>Gennemse målmapper</translation>
    </message>
    <message>
        <source>The helper could not list the repair-system folder:

%1</source>
        <translation>Hjælpen kunne ikke liste reparationssystemmappen:

%1</translation>
    </message>
    <message>
        <source>BROWSE_ENTRY</source>
        <translation>BROWSE _ ENTRY</translation>
    </message>
    <message>
        <source>(choose this folder: %1)</source>
        <translation>(vælg denne mappe: %1)</translation>
    </message>
    <message>
        <source>(parent folder)</source>
        <translation>(forældremappe)</translation>
    </message>
    <message>
        <source>Select repair-system destination</source>
        <translation>Vælg reparationssystemdestination</translation>
    </message>
    <message>
        <source>Choose a destination folder inside the repaired system (current folder: %1):</source>
        <translation>Vælg en destinationsmappe inde i det reparerede system (nuværende mappe: %1):</translation>
    </message>
    <message>
        <source>Folder</source>
        <translation>Mappe</translation>
    </message>
    <message>
        <source>Open</source>
        <translation>Åbn</translation>
    </message>
    <message>
        <source>(choose this folder:</source>
        <translation>(vælg denne mappe:</translation>
    </message>
    <message>
        <source>Add files to copy</source>
        <translation>Tilføj filer der skal kopieres</translation>
    </message>
    <message>
        <source>Add folder to copy</source>
        <translation>Tilføj mappe der skal kopieres</translation>
    </message>
    <message>
        <source>Stage at least one source and name a destination path first.</source>
        <translation>Trin mindst én kilde og navngive en destination sti først.</translation>
    </message>
    <message>
        <source>Copy the %1 staged item(s) to %2?

The helper keeps its direction and path containment checks; a sensitive repair-system destination is refused unless the helper approves it, and every regular file is byte-compared after the copy.</source>
        <translation>Kopier %1 iscenesat element (er) til %2?

Hjælpen holder sin retning og sti indeslutning kontrol; en følsom repair-system destination nægtes, medmindre hjælperen godkender det, og hver almindelig fil er byte- sammenlignet efter kopien.</translation>
    </message>
    <message>
        <source>File Copy - Copy and Verify</source>
        <translation>File Copy - Kopiér og verificer</translation>
    </message>
    <message>
        <source>File Copy - Preview Changes</source>
        <translation>Fil Kopiér - forhåndsvis ændringer</translation>
    </message>
    <message>
        <source>Protected running host - details</source>
        <translation>Beskyttet løbende vært - detaljer</translation>
    </message>
    <message>
        <source>not mounted (offline target)</source>
        <translation>ikke monteret (offline mål)</translation>
    </message>
    <message>
        <source>Select a drive to see its details.</source>
        <translation>Vælg et drev for at se dets detaljer.</translation>
    </message>
    <message>
        <source>Inspecting the selected component.</source>
        <translation>Undersøger den valgte komponent.</translation>
    </message>
    <message>
        <source>Helper-confirmed by the last read-only diagnostics.</source>
        <translation>Helper- bekræftet af den sidste read- kun diagnostik.</translation>
    </message>
    <message>
        <source>Read-only inventory only; run diagnostics to confirm.</source>
        <translation>Read- only inventar kun; køre diagnostik at bekræfte.</translation>
    </message>
    <message>
        <source>PROTECTED - running system; read-only details only</source>
        <translation>BESKYTTEDE - køresystem; kun detaljer</translation>
    </message>
    <message>
        <source>Live / installer media - not selectable</source>
        <translation>Live / installer medier - ikke valgbar</translation>
    </message>
    <message>
        <source>PROTECTED - running host scope</source>
        <translation>BESKYTTEDE - kørende værtsområde</translation>
    </message>
    <message>
        <source>Unlock required before selection</source>
        <translation>Lås op før markering</translation>
    </message>
    <message>
        <source>Eligible repair candidate</source>
        <translation>Støtteberettiget reparationskandidat</translation>
    </message>
    <message>
        <source>Drive:</source>
        <translation>Kør:</translation>
    </message>
    <message>
        <source>Detected target:</source>
        <translation>Detekteret mål:</translation>
    </message>
    <message>
        <source>Pending inspection</source>
        <translation>Indtil inspektionen er afsluttet</translation>
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
        <translation>Størrelse:</translation>
    </message>
    <message>
        <source>Connection:</source>
        <translation>Forbindelse:</translation>
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
        <translation>Monteringer:</translation>
    </message>
    <message>
        <source>Running system protection unresolved</source>
        <translation>Beskyttelse af køresystemet uløst</translation>
    </message>
    <message>
        <source>No protected physical backing disk was identified</source>
        <translation>Ingen beskyttet fysisk backing disk blev identificeret</translation>
    </message>
    <message>
        <source>Current running Linux system</source>
        <translation>Nuværende kørende Linux-system</translation>
    </message>
    <message>
        <source>Critical mounts: %1</source>
        <translation>Kritiske monteringer: %1</translation>
    </message>
    <message>
        <source>Read-only protected-running-system facts: the helper OS fact plus the inventory model, device path, size, transport and critical mounts. Nothing here is probed destructively.</source>
        <translation>Read- only protected-running-system fakta: hjælperen OS faktum plus lagermodel, enhed sti, størrelse, transport og kritiske monteringer. Intet her er fundet destruktivt.</translation>
    </message>
    <message>
        <source>Select a drive to see unlock status.</source>
        <translation>Vælg et drev til at se låse status.</translation>
    </message>
    <message>
        <source>State: protected
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
The protected running host cannot be unlocked or modified; unlock is available for an offline repair target only. Use Host Maintenance for the protected running host.</source>
        <translation>Tilstand: beskyttet
Komponent: %1
Mapper: (ingen)
Metode: helper unlock (cryptsetup; passfrase via en mode- 600 keyfile, slettet efter brug)
Den beskyttede løbevært kan ikke låses op eller ændres; låsen er kun tilgængelig for et offline reparationsmål. Brug Vært Vedligeholdelse til den beskyttede kørende vært.</translation>
    </message>
    <message>
        <source>(none detected)</source>
        <translation>(ingen opdaget)</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
A locked LUKS container is visible on this drive; press Unlock to open it for this recovery session.</source>
        <translation>Stat: låst
Komponent: %1
Mapper: (ingen)
Metode: helper unlock (cryptsetup; passfrase via en mode- 600 keyfile, slettet efter brug)
En låst LUKS beholder er synlig på dette drev; trykke på Lås op for at åbne det for denne opsving session.</translation>
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
        <translation>Tilstand: ulåst
Komponent: %1
Mapper: %2
Metode: allerede åben før denne session (synlig mapper; Boot Bitch vil genbruge det og vil ikke lukke det).</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper-confirmed mapping from the last read-only diagnostics.</source>
        <translation>Tilstand: ulåst
Komponent: %1
Mapper: %2
Metode: helper- bekræftet kortlægning fra den sidste read- kun diagnostik.</translation>
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
        <translation>Tilstand: låst eller ingen krypteret komponent detekteret
Komponent: (ikke påvist)
Mapper: (ingen)
Metode: helper unlock (cryptsetup; passfrase via en mode- 600 keyfile, slettet efter brug)
Ingen låst LUKS komponent og ingen låsning operation blev registreret for dette drev i den aktuelle session.</translation>
    </message>
    <message>
        <source>unavailable - %1</source>
        <translation>Utilgængelig - %1</translation>
    </message>
    <message>
        <source>Host maintenance:
%1</source>
        <translation>Vært vedligeholdelse:
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
        <translation>Tilladelse påkrævet: Diagnostik og reparationer mislykkes lukket, indtil du trykker på Godkend.</translation>
    </message>
    <message>
        <source>Re-establish the privileged helper session for the current scope now. The password is requested in the hidden-input modal and is never logged.</source>
        <translation>Genetablere den privilegerede hjælpesesession for den nuværende rækkevidde nu. Adgangskoden ønskes i hidden- input modal og er aldrig logget.</translation>
    </message>
    <message>
        <source>Running...</source>
        <translation>Løber...</translation>
    </message>
    <message>
        <source>Scope required</source>
        <translation>Anvendelsesområde</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope; this unlocks the gated actions.</source>
        <translation>Kør All - kør alle tilgængelige read- only diagnostik for det aktuelle anvendelsesområde; dette låser op for de gatede handlinger.</translation>
    </message>
    <message>
        <source>Select a target and wait for any running command first.</source>
        <translation>Vælg et mål og vent på enhver kørende kommando først.</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic through the helper.</source>
        <translation>Kør Diagnostic - kør den valgte read- only diagnostik gennem hjælperen.</translation>
    </message>
    <message>
        <source>Run All for the current scope and refresh the read-only backend profile.</source>
        <translation>Kør Alt for det aktuelle anvendelsesområde og genopfriske den read- only backend profil.</translation>
    </message>
    <message>
        <source>Read or edit the selected target configuration file through the guarded helper; a saved edit invalidates cached diagnostics.</source>
        <translation>Læs eller rediger den valgte målindstillingsfil gennem den bevogtede hjælper; en gemt redigering uvaliderer cachede diagnostik.</translation>
    </message>
    <message>
        <source>Host Maintenance has no target-file editing; commit an offline repair target first.</source>
        <translation>Vært Vedligeholdelse har ingen target- fil redigering; begå en offline reparation mål først.</translation>
    </message>
    <message>
        <source>Commit an offline repair target first.</source>
        <translation>Lav et offline reparationsmål først.</translation>
    </message>
    <message>
        <source>No target configuration file is available for this target; run diagnostics to probe the list.</source>
        <translation>Der er ingen målindstillingsfil til rådighed for dette mål. Kør diagnostik for at gennemsøge listen.</translation>
    </message>
    <message>
        <source>Already Unlocked</source>
        <translation>Allerede ulåst</translation>
    </message>
    <message>
        <source>An unlocked Linux filesystem is already visible on this drive. Boot Bitch will reuse the existing mapper and will not close or reopen a mapping created by this recovery session. Mounting happens during diagnostics (read-only) and repairs (read-write); data filesystems are never auto-mounted on selection.</source>
        <translation>Et ulåst Linux-filsystem er allerede synligt på dette drev. Boot Bitch vil genbruge den eksisterende mapper og vil ikke lukke eller genåbne en kortlægning skabt af denne recovery session. Montering sker under diagnostik (read- only) og reparationer (read- write); data filsystemer er aldrig automatisk monteret på valg.</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked; use Host Maintenance for the protected running host.</source>
        <translation>Den beskyttede kørende vært kan ikke låses op; brug Vært Vedligeholdelse for den beskyttede kørende vært.</translation>
    </message>
    <message>
        <source>Host Maintenance is the current scope, but the selected offline drive can still be unlocked.</source>
        <translation>Vært Vedligeholdelse er det aktuelle omfang, men det valgte offline drev kan stadig låses op.</translation>
    </message>
    <message>
        <source>Unlock %1 using cryptsetup through the privileged helper. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>Lås %1 op ved hjælp af kryptering gennem den privilegerede hjælper. Passphrase rejser gennem en privat nøglefil og er aldrig placeret i kommando argumenter eller logs.</translation>
    </message>
    <message>
        <source>The protected running system cannot be selected as a repair target; use Host Maintenance for the protected running host.</source>
        <translation>Det beskyttede løbesystem kan ikke vælges som reparationsmål; brug Vært Vedligeholdelse for den beskyttede løbevært.</translation>
    </message>
    <message>
        <source>Live / installer media is read-only boot media and cannot be selected as a repair target.</source>
        <translation>Live / installer medier er read- only boot medier og kan ikke vælges som en reparation mål.</translation>
    </message>
    <message>
        <source>Unlock the encrypted volume first; Select Target becomes available after a Linux filesystem is detected.</source>
        <translation>Lås den krypterede lydstyrke op først; Select Target bliver tilgængelig efter et Linux-filsystem detekteres.</translation>
    </message>
    <message>
        <source>Committed repair target. Repair, Diagnostics and File Copy target this physical drive until another drive is explicitly selected with Select Target.</source>
        <translation>Udstyret reparationsmål. Reparation, diagnosticering og fil Kopiér mål dette fysiske drev, indtil et andet drev er udtrykkeligt valgt med Select Target.</translation>
    </message>
    <message>
        <source>Commit %1 as the repair target; this leaves Host Maintenance and switches the scope to the selected drive.</source>
        <translation>Commit %1 som reparationsmål; dette efterlader Vært Vedligeholdelse og skifter anvendelsesområdet til det valgte drev.</translation>
    </message>
    <message>
        <source>The running host target could not be detected.</source>
        <translation>Det løbende værtsmål kunne ikke spores.</translation>
    </message>
    <message>
        <source>Leave Host Maintenance and return to repair-target mode.</source>
        <translation>Efterlad Vært Vedligeholdelse og vend tilbage til reparations- mål tilstand.</translation>
    </message>
    <message>
        <source>Select the running host for deliberate guarded maintenance; the administrator authorization is requested here once and cached for the session.</source>
        <translation>Vælg den kørende vært for bevidst bevogtet vedligeholdelse; administrator godkendelse er anmodet her én gang og cached for sessionen.</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a repair target.</source>
        <translation>Den løbende vært mål kunne ikke detekteres; diagnostik har brug for en reparation mål.</translation>
    </message>
    <message>
        <source>Committed target: %1 + %2</source>
        <translation>Målsætning: %1 + %2</translation>
    </message>
    <message>
        <source>Committed target: none (selection changed)</source>
        <translation>Indsendt mål: ingen (markering ændret)</translation>
    </message>
    <message>
        <source>The selected scope has no resolved Linux root component; Refresh Devices and commit the repair target again.</source>
        <translation>Det valgte anvendelsesområde har ingen løst Linux root komponent; Genopfrisk enheder og begå reparation mål igen.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize on the Systems or Repair tab to re-establish it. Run All reuses the cached authorization and never prompts by itself.</source>
        <translation>Ingen administrator session er aktiv; tryk på Godkend på Systemer eller Reparer fanen for at genetablere det. Kør Alle genbruger den cached tilladelse og aldrig beder af sig selv.</translation>
    </message>
    <message>
        <source>Run diagnostics for this scope to unlock the gated actions. Diagnostics are read-only and the only evidence source.</source>
        <translation>Kør diagnostik for dette omfang for at låse op for de gatede handlinger. Diagnostik er kun klar og den eneste kilde.</translation>
    </message>
    <message>
        <source>A repair was not proven unchanged, so the cached diagnostics are invalidated. Run diagnostics again before another gated action.</source>
        <translation>En reparation blev ikke bevist uændret, så cached diagnostik er ugyldiggjort. Kør diagnostik igen før en anden bølge handling.</translation>
    </message>
    <message>
        <source>Gated actions reflect the cached capability lines; the helper still runs every runtime preflight when a command starts.</source>
        <translation>Gaterede handlinger afspejler cachede kapacitetslinjer; hjælperen kører stadig hver runtime før flyvning, når en kommando starter.</translation>
    </message>
    <message>
        <source>Regenerating diagnostics automatically</source>
        <translation>Regenererende diagnostik automatisk</translation>
    </message>
    <message>
        <source>Running diagnostic: %1</source>
        <translation>Kører diagnostik: %1</translation>
    </message>
    <message>
        <source>Running all diagnostics</source>
        <translation>Kør alle diagnostik</translation>
    </message>
    <message>
        <source>Unlocking %1</source>
        <translation>Åbn %1</translation>
    </message>
    <message>
        <source>running host</source>
        <translation>kørende vært</translation>
    </message>
    <message>
        <source>offline target</source>
        <translation>offline mål</translation>
    </message>
    <message>
        <source>host maintenance active</source>
        <translation>værtsvedligeholdelsesaktiv</translation>
    </message>
    <message>
        <source>repair target committed</source>
        <translation>reparationsmål forpligtet</translation>
    </message>
    <message>
        <source>no committed scope</source>
        <translation>ingen forpligtelse</translation>
    </message>
    <message>
        <source>Current session</source>
        <translation>Løbende session</translation>
    </message>
    <message>
        <source>Log files (*.log);;All files (*)</source>
        <translation>Logfiler (*. log); Alle filer (*)</translation>
    </message>
    <message>
        <source>Save log as</source>
        <translation>Gem log som</translation>
    </message>
    <message>
        <source>About Boot Bitch</source>
        <translation>Om Boot Bitch</translation>
    </message>
    <message>
        <source>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;&lt;p&gt;&lt;b&gt;Developer:&lt;/b&gt; CaptainMorgan12&lt;/p&gt;&lt;p&gt;This GUI is the package&apos;s entry point: a Qt 3.3.x frontend with the ported guarded helper for Debian Etch-era systems.&lt;/p&gt;&lt;p&gt;&lt;b&gt;Guarded repair mode:&lt;/b&gt; read-only diagnostics can inspect either the protected Running Host or an explicitly selected repair drive. The guarded Debian/APT package stages (interrupted configuration, broken dependencies, metadata refresh, upgrade), GRUB-legacy configuration regeneration, LUKS target unlock and guarded target-file editing run through the ported helper after confirmation; Host Maintenance enables the same supported stages natively on the active system after repeating the host identity and boot-mount checks.&lt;/p&gt;&lt;p&gt;The modern-only features - verified File Copy, Btrfs snapshot rollback, EFI/UKI and extlinux repair, boot-stack reconciliation and Make Default - are greyed with the helper&apos;s own probe reasons on this frontend; Arch/Alpine/Fedora package backends stay diagnostics-only here.&lt;/p&gt;&lt;p&gt;The first privileged action authorizes one cached administrator session per scope through a hidden-input modal (the Qt GUI remains unprivileged). It can be ended at any time from File - Lock Administrator Session.&lt;/p&gt;</source>
        <translation>&lt; h3 &gt; Boot Bitch %1 &lt; / h3 &gt; &lt; p &gt; &lt; b &gt; Udvikler: &lt; / b &gt; CaptainMorgan12 &lt; / p &gt; &lt; p &gt; Denne GUI er pakkens indgang: en Qt 3.3.x-frontend med den porterede bevogtet hjælper til Debian Etch- era-systemer. &lt; / p &gt; &lt; p &gt; &lt; b &gt; Bevogtet reparationstilstand: &lt; / b &gt; read- only diagnostik kan inspicere enten den beskyttede Running Host eller et eksplicit udvalgt reparationdrev. De bevogtede Debian / APT-pakkefaser (afbrudt konfiguration, brudte afhængigheder, metadata genopfriske, opgradering), GRUB- nedarvede konfigurationsregenerering, LUKS-mållåsning og bevogtet target- filredigering kører gennem den porterede hjælper efter bekræftelse; Værtsvedligeholdelse muliggør de samme understøttede faser indbygget på det aktive system efter at have gentaget værtens identitet og boot- mount kontroller. &lt; / p &gt; &lt; p &gt; De moderniserede-kun funktioner - verificeret File Copy, Btrfs snapshot rollback, EFI / UKI og extlinux reparation, boot- stack forsoning og Make Standard - er gråt med hjælperens egne sonde grunde på denne frontend; Arch / Alpine / Fedora pakke backends ophold diagnostics-kun her. &lt; / p &gt; &lt; p &gt; Den første privilegerede aktion giver tilladelse til en cachet administrator session pr. omfang gennem en skjult input-modal (Qt GUI forbliver uprivilegeret). Det kan afsluttes når som helst fra Fil - Lås Administrator Session. &lt; / p &gt;</translation>
    </message>
</context>
</TS>
