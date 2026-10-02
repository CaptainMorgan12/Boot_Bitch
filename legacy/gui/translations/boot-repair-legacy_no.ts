<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS language="no">
<context>
    <name>LegacyMainWindow</name>
    <message>
        <source>Validate environment</source>
        <translation>Valider miljø</translation>
    </message>
    <message>
        <source>Validate</source>
        <translation>Valider</translation>
    </message>
    <message>
        <source>Check the selected system&apos;s mounts, filesystem metadata, boot files, mapper consistency and dependency readiness before any repair action. This is an independent safety preflight rather than an optional Full Repair stage.</source>
        <translation>Kontroller det valgte systemets monteringer, filsystemmetadata, oppstartsfiler, kartleggerkonsistens og avhengighetsberedskap før noen reparasjonshandling. Dette er en uavhengig sikkerhetspreflight i stedet for en valgfri Full Repair-fase.</translation>
    </message>
    <message>
        <source>Always preflight</source>
        <translation>Alltid preflight</translation>
    </message>
    <message>
        <source>File system repair</source>
        <translation>Filsystemreparasjon</translation>
    </message>
    <message>
        <source>Check File Systems</source>
        <translation>Sjekk filsystemer</translation>
    </message>
    <message>
        <source>Run the read-only file system check for the selected system&apos;s root and /boot filesystems and report each device&apos;s check tool and result without changing anything. This legacy frontend exposes the read-only check only; device repair is not wired.</source>
        <translation>Kjør den skrivebeskyttede filsystemskontrollen for det valgte systemets rot- og /boot-filsystem og rapporter hver enhets kontrollverktøy og resultat uten å endre noe. Denne gamle frontend avslører kun lesebeskyttede kontroller; enhetsreparasjon er ikke kablet.</translation>
    </message>
    <message>
        <source>Full Repair plan: unavailable on this frontend - read-only check only</source>
        <translation>Full Repair plan: utilgjengelig på denne frontend - kun skrivebeskyttet sjekk</translation>
    </message>
    <message>
        <source>Complete package configuration</source>
        <translation>Komplett pakkekonfigurasjon</translation>
    </message>
    <message>
        <source>Complete Configuration</source>
        <translation>Komplett konfigurasjon</translation>
    </message>
    <message>
        <source>Complete interrupted dpkg package configuration in the selected repair system. This is the same stage controlled by Settings -&gt; Full Repair plan -&gt; Complete interrupted package configuration, but it can also be run independently here.</source>
        <translation>Komplett avbrutt dpkg pakkekonfigurasjon i det valgte reparasjonssystemet. Dette er det samme trinnet kontrollert av Innstillinger -&gt; Full reparasjonsplan -&gt; Komplett avbrutt pakkekonfigurasjon, men den kan også kjøres uavhengig her.</translation>
    </message>
    <message>
        <source>Run the guarded dpkg-configure repair? The helper keeps its package-lock and runtime preflights.</source>
        <translation>Kjør den bevoktede dpkg-konfigurere reparasjon? Hjelperen holder pakken-lås og kjøretid preflights.</translation>
    </message>
    <message>
        <source>Repair broken dependencies</source>
        <translation>Reparer ødelagte avhengigheter</translation>
    </message>
    <message>
        <source>Repair Dependencies</source>
        <translation>Reparasjon av avhengighet</translation>
    </message>
    <message>
        <source>Repair package dependencies in the selected repair system after the mandatory safety preflight. This maps directly to Settings -&gt; Repair broken package dependencies.</source>
        <translation>Reparasjon pakkeavhengigheter i det valgte reparasjonssystemet etter obligatorisk sikkerhetsforbedring. Dette kartene direkte til Innstillinger -&gt; Reparasjon av ødelagt pakkeavhengighet.</translation>
    </message>
    <message>
        <source>Run the guarded fix-broken repair? The helper keeps its simulation-first preflight and runtime guards.</source>
        <translation>Kjør den vaktede reparasjonen? Hjelperen holder sin simulering-første preflight og kjøretid vakter.</translation>
    </message>
    <message>
        <source>Refresh package metadata</source>
        <translation>Oppdater pakkemetadata</translation>
    </message>
    <message>
        <source>Refresh Metadata</source>
        <translation>Oppdater metadata</translation>
    </message>
    <message>
        <source>Refresh APT metadata in the selected repair system without upgrading installed packages. This maps directly to Settings -&gt; Refresh package metadata.</source>
        <translation>Oppdater APT metadata i det valgte reparasjonssystemet uten å oppgradere installerte pakker. Dette kartene direkte til Innstillinger -&gt; Oppdater pakkemetadata.</translation>
    </message>
    <message>
        <source>Refresh package metadata for the selected scope? The helper requires a reachable, trusted APT source.</source>
        <translation>Oppdatere pakkemetadata for det valgte omfanget? Hjelperen krever en tilgjengelig, pålitelig APT-kilde.</translation>
    </message>
    <message>
        <source>Upgrade installed packages</source>
        <translation>Oppgrader installerte pakker</translation>
    </message>
    <message>
        <source>Simulate and Upgrade</source>
        <translation>Simulering og oppgradering</translation>
    </message>
    <message>
        <source>Simulate the APT transaction first, inspect proposed removals, then apply a safe upgrade. This maps directly to Settings -&gt; Upgrade installed packages.</source>
        <translation>Simulere APT-transaksjonen først, inspisere foreslåtte fjerninger, deretter bruke en sikker oppgradering. Dette kartene direkte til Innstillinger -&gt; Oppgrader installerte pakker.</translation>
    </message>
    <message>
        <source>Run the guarded apt-upgrade transaction? The helper keeps its simulation-first and source guards.</source>
        <translation>Kjøre den vaktede apt-upgrade transaksjonen? Hjelperen holder sin simulering-første og kilde vakt.</translation>
    </message>
    <message>
        <source>DKMS</source>
        <translation>DKMS</translation>
    </message>
    <message>
        <source>Rebuild DKMS</source>
        <translation>Bygg om DKMS</translation>
    </message>
    <message>
        <source>Rebuild out-of-tree kernel modules for kernels installed in the selected system. The helper refuses this action when DKMS is not installed; this legacy frontend exposes no DKMS action.</source>
        <translation>Bygg ut av trekjernemoduler for kjerner installert i det valgte systemet. Hjelperen nekter denne handlingen når DKMS ikke er installert; denne arvelige frontend avslører ingen DKMS-handling.</translation>
    </message>
    <message>
        <source>Graphical login / display manager</source>
        <translation>Grafisk innlogging / skjermhåndterer</translation>
    </message>
    <message>
        <source>Restore Graphical Login</source>
        <translation>Gjenopprett grafisk innlogging</translation>
    </message>
    <message>
        <source>Restore the legacy SysV display manager configured for the running host: the /etc/X11/default-display-manager entry and the missing runlevel S-symlink, with a backup and rollback, never starting the GUI. This is a host-scope stage on this legacy frontend.</source>
        <translation>Gjenopprett den gamle SysV-skjerm manageren konfigurert for den kjørernde verten: /etc/X11/default-display-manager-oppføringen og den manglende kjørenivå S-symlink, med en backup og rulle tilbake, aldri starter GUI. Dette er et host-scope-scene i denne arvefronten.</translation>
    </message>
    <message>
        <source>Restore the graphical login configuration for the running host? The helper backs up /etc/X11/default-display-manager and the runlevel symlink state, restores the configured entry and the missing S-symlink, rolls back on any failure, and never starts the display manager.</source>
        <translation>Gjenopprette den grafiske innloggingskonfigurasjonen for den kjørende verten? Hjelperen sikkerhetskopierer /etc/X11/default-display-manager og runlevel-symlink-tilstanden, gjenoppretter konfigurert oppføring og den manglende S-symlink, ruller tilbake på noen feil, og starter aldri skjermen manager.</translation>
    </message>
    <message>
        <source>Initramfs</source>
        <translation>Initramfs</translation>
    </message>
    <message>
        <source>Rebuild Initramfs</source>
        <translation>Rebuild Iitramfs</translation>
    </message>
    <message>
        <source>Rebuild initramfs images for the selected repair system only after mapper and crypttab consistency checks pass. The helper backs up each image before the apply. On Etch the stage runs through the guarded plain-chroot fallback (no unshare required).</source>
        <translation>Bygg om initramfs bilder for det valgte reparasjonssystemet bare etter kartlegger og crypttab konsistens kontroller passerer. Hjelperen støtter hvert bilde før det gjelder. På etch går scenen gjennom den bevoktede slette-krone tilbakefall (ingen uandel nødvendig).</translation>
    </message>
    <message>
        <source>Rebuild the initramfs for the selected scope? The helper keeps its mapper/crypttab and backup preflights.</source>
        <translation>Bygge om initramfs for det valgte omfanget? Hjelperen holder kartleggeren/krypterer og sikkerhetskopierer.</translation>
    </message>
    <message>
        <source>EFI / UKI bootloader</source>
        <translation>EFI / UKI bootloader</translation>
    </message>
    <message>
        <source>Repair EFI / UKI</source>
        <translation>Reparasjon EFI / UKI</translation>
    </message>
    <message>
        <source>Repair the selected system&apos;s EFI / UKI boot path. This legacy frontend exposes no EFI action; the Etch target is a BIOS/GRUB-legacy system.</source>
        <translation>Reparer det valgte systemets EFI / UKI oppstartssti. Denne arvelige frontend avslører ingen EFI-handling; Etch-målet er et BIOS/GRUB-legacy-system.</translation>
    </message>
    <message>
        <source>GRUB configuration</source>
        <translation>GRUB konfigurasjon</translation>
    </message>
    <message>
        <source>Regenerate GRUB</source>
        <translation>Regenerere GRUB</translation>
    </message>
    <message>
        <source>Regenerate the selected repair system&apos;s GRUB menu/configuration after the mandatory safety preflight. The helper backs up menu.lst, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Generer det valgte reparasjonssystemets GRUB-meny/oppsett etter den obligatoriske sikkerhetsforbedringen. Hjelperen sikkerhetskopierer menu.lt, bevarer hver eksisterende oppstartsoppføring og ruller tilbake på alle feil.</translation>
    </message>
    <message>
        <source>Regenerate the GRUB configuration? The helper backs up the target menu/configuration, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Generere GRUB-konfigurasjonen? Hjelperen sikkerhetskopierer målmenyen/konfigurasjonen, bevarer alle eksisterende oppstartsoppføringer og ruller tilbake på alle feil.</translation>
    </message>
    <message>
        <source>extlinux configuration</source>
        <translation>extlinux konfigurasjon</translation>
    </message>
    <message>
        <source>Regenerate extlinux</source>
        <translation>Regenerere extlinux</translation>
    </message>
    <message>
        <source>Regenerate the selected system&apos;s extlinux bootloader configuration. This legacy frontend exposes no extlinux action.</source>
        <translation>Generer det valgte systemets extlinux oppstartslasterkonfigurasjon. Denne arvelige frontend avslører ingen extlinux handling.</translation>
    </message>
    <message>
        <source>Boot stack reconciliation</source>
        <translation>Boot stakk forsoning</translation>
    </message>
    <message>
        <source>Reconcile Boot Stack</source>
        <translation>Reconcile Boot Stack</translation>
    </message>
    <message>
        <source>Reconcile the selected repair system&apos;s boot stack in one guarded legacy pass: mapper/crypttab validation, initramfs rebuild and GRUB-legacy configuration regeneration, with the component backups and preflights unchanged. This is the Etch equivalent of the modern boot-stack reconciliation and stays out of the Full Repair plan.</source>
        <translation>Reconcile det valgte reparasjonssystemets boot stabel i ett bevoktet arv pass: kartlegger/krypttab validering, initramfs gjenoppbygging og GRUB-legacy konfigurasjon regenerering, med komponent backups og preflights uendret. Dette er etch-ekvivalenten til den moderne boot-stack forsoning og holder seg ut av Full Repair-planen.</translation>
    </message>
    <message>
        <source>Full Repair plan: Manual recovery tool</source>
        <translation>Full Repair plan: Manuell gjenoppretting verktøy</translation>
    </message>
    <message>
        <source>Run the guarded boot-stack reconciliation? The helper runs the mapper/crypttab validation, the initramfs rebuild and the GRUB-legacy regeneration in one pass with every component preflight and backup.</source>
        <translation>Kjøre den bevoktede boot-stack forsoning? Hjelperen kjører kartlegger/kryptab-valideringen, initramfs gjenoppbygging og GRUB-legacy regenerering i ett pass med hver komponent forhåndsbelysning og backup.</translation>
    </message>
    <message>
        <source>Complete interrupted package configuration</source>
        <translation>Fullført avbrutt pakkekonfigurasjon</translation>
    </message>
    <message>
        <source>Repair broken package dependencies</source>
        <translation>Reparer ødelagt pakkeavhengigheter</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Gjør standard</translation>
    </message>
    <message>
        <source>Repair file system errors (read-only check first)</source>
        <translation>Reparere filsystemfeil (les bare sjekk først)</translation>
    </message>
    <message>
        <source>the legacy frontend exposes the read-only file system check only; per-device repair is not wired on this frontend (fail closed)</source>
        <translation>den gamle frontend avslører kun lesebeskyttede filsystemskontroll; per enhet reparasjon er ikke kablet i denne frontend (feil lukket)</translation>
    </message>
    <message>
        <source>Upgrade installed packages (adaptive APT simulation)</source>
        <translation>Oppgrader installerte pakker (adaptiv APT-simulering)</translation>
    </message>
    <message>
        <source>Rebuild DKMS modules</source>
        <translation>Bygg om DKMS-moduler</translation>
    </message>
    <message>
        <source>Restore graphical login manager</source>
        <translation>Gjenopprette grafisk innloggingshåndtering</translation>
    </message>
    <message>
        <source>Rebuild initramfs after mapper/crypttab validation</source>
        <translation>Bygg initramfs på nytt etter validering av kartlegger/kryptertab</translation>
    </message>
    <message>
        <source>Repair EFI / UKI boot path</source>
        <translation>Reparasjon EFI / UKI boot bane</translation>
    </message>
    <message>
        <source>Update GRUB configuration</source>
        <translation>Oppdater GRUB-konfigurasjon</translation>
    </message>
    <message>
        <source>Update extlinux configuration</source>
        <translation>Oppdater extlinux-konfigurasjon</translation>
    </message>
    <message>
        <source>Choose Full Repair stages in Settings. Enabled stages run in the order shown. Each configurable stage also appears below as an individual tool; the Full Repair column mirrors its current Settings state. The boot tools (EFI / UKI bootloader, GRUB or extlinux configuration, boot-stack reconciliation and Make Default) are independent: run them in any order, and a later action re-verifies what an earlier one changed and reports its own result. The active scope is shown beside Repair: selected repair drive or Running Host maintenance.</source>
        <translation>Velg Full Reparasjon stadier i Innstillinger. Aktiverte stadier kjører i den viste rekkefølgen. Hver konfigurerbar fase vises også under som et enkelt verktøy; Full Repair kolonnen speiler sin nåværende innstillingstilstand. Startverktøyene (EFI / UKI bootloader, GRUB eller extlinux konfigurasjon, boot-stack forsoning og Make Default) er uavhengige: kjører dem i enhver rekkefølge, og en senere handling bekrefter hva en tidligere endret og rapporterer sitt eget resultat. Det aktive omfanget vises ved siden av Repair: valgt reparasjonsstasjon eller vedlikehold av kjørerverten.</translation>
    </message>
    <message>
        <source>Run All diagnostics for the selected target or running host before starting Full Repair. The report is read-only evidence used to choose and confirm repair stages.</source>
        <translation>Kjør alle diagnoser for det valgte målet eller kjører vert før du starter Full Repair. Rapporten er skrivebeskyttet bevis som brukes til å velge og bekrefte reparasjonsfaser.</translation>
    </message>
    <message>
        <source>Ready: required cached read-only diagnostics are available for the selected stages. Review them in Diagnostics or Logs before confirming.</source>
        <translation>Klar: De nødvendige cachede lesebeskyttede diagnostikkene er tilgjengelige for de valgte stadiene. Se dem i Diagnostics eller Logs før bekreftelse.</translation>
    </message>
    <message>
        <source>Environment validation</source>
        <translation>Miljøvalidering</translation>
    </message>
    <message>
        <source>Summarizes the selected system, protection state, mounted identity and inspection readiness.</source>
        <translation>Oppsummerer det valgte systemet, beskyttelsestilstanden, montert identitet og inspeksjonsberedskap.</translation>
    </message>
    <message>
        <source>Distribution and boot backend profile</source>
        <translation>Distribusjon og oppstart backend profil</translation>
    </message>
    <message>
        <source>Identifies the distribution family, package manager, initramfs generator, bootloader and current guarded repair capability.</source>
        <translation>Identifiserer distribusjonsfamilien, pakke manager, initramfs generator, bootloader og nåværende bevoktet reparasjonsevne.</translation>
    </message>
    <message>
        <source>Boot diagnostics</source>
        <translation>Boot-diagnostikk</translation>
    </message>
    <message>
        <source>Shows boot mounts and /boot contents plus storage evidence without changing the selected system.</source>
        <translation>Viser boot monteringer og /boot innhold pluss lagringsbevis uten å endre det valgte systemet.</translation>
    </message>
    <message>
        <source>Boot evidence and selection history</source>
        <translation>Boot bevis og utvalg historie</translation>
    </message>
    <message>
        <source>Correlates the detected boot chain, bootloader selection, kernel/initramfs and unlock evidence.</source>
        <translation>Korrelaterer den detekterte oppstartskjeden, oppstartslastervalg, kjerne/initramfs og låse opp bevis.</translation>
    </message>
    <message>
        <source>Kernel / initramfs</source>
        <translation>Kernel / initramfs</translation>
    </message>
    <message>
        <source>Reviews kernel files and verifies matching initramfs images through a read-only inspection.</source>
        <translation>Vurderinger kjernefiler og verifiserer samsvarende initramfs bilder gjennom en skrivebeskyttet inspeksjon.</translation>
    </message>
    <message>
        <source>Reviews the GRUB configuration without changing boot files.</source>
        <translation>Vurdering av GRUB-konfigurasjonen uten å endre oppstartsfiler.</translation>
    </message>
    <message>
        <source>EFI / UKI boot state</source>
        <translation>EFI / UKI boot state</translation>
    </message>
    <message>
        <source>Inspects EFI/UKI evidence; unavailable on this legacy BIOS frontend with the helper&apos;s probe reason.</source>
        <translation>Inspeksjoner EFI/UKI bevis; utilgjengelig på denne arven BIOS frontend med hjelpens sonde grunn.</translation>
    </message>
    <message>
        <source>Reviews the configured display manager and recent boot evidence without starting the GUI.</source>
        <translation>Anmeld den konfigurerte skjermhåndtereren og nylige oppstartsdokumenter uten å starte GUI.</translation>
    </message>
    <message>
        <source>Boot errors</source>
        <translation>Boot feil</translation>
    </message>
    <message>
        <source>Reads recent error-priority entries from the running host or selected repair system when available.</source>
        <translation>Leser nylige feilprioritetsoppføringer fra den kjørernde verten eller valgte reparasjonssystem når det er tilgjengelig.</translation>
    </message>
    <message>
        <source>Disk usage</source>
        <translation>Diskbruk</translation>
    </message>
    <message>
        <source>Summarizes filesystem capacity and free space for the running host or read-only repair target.</source>
        <translation>Summerer filsystemkapasiteten og fri plass for den løpende verten eller lesebeskyttet reparasjonsmål.</translation>
    </message>
    <message>
        <source>File systems</source>
        <translation>Filsystem</translation>
    </message>
    <message>
        <source>Runs the read-only file system check for the selected system&apos;s root, /boot and other filesystems.</source>
        <translation>Kjører det skrivebeskyttede filsystemets sjekk for det valgte systemets rot, / boot og andre filsystemer.</translation>
    </message>
    <message>
        <source>/etc/fstab review</source>
        <translation>/etc/fstab review</translation>
    </message>
    <message>
        <source>Displays the running host or selected repair system&apos;s fstab; repair-system inspection is mounted read-only.</source>
        <translation>Viser den kjørende verten eller valgte reparasjonssystemets fstab; reparasjonssystemets inspeksjon er montert leselig.</translation>
    </message>
    <message>
        <source>Btrfs status</source>
        <translation>Btrfs status</translation>
    </message>
    <message>
        <source>Shows Btrfs filesystem and subvolume information when the target uses Btrfs.</source>
        <translation>Viser Btrfs-filsystem og undervoluminformasjon når målet bruker Btrfs.</translation>
    </message>
    <message>
        <source>Device-mapper ancestry</source>
        <translation>Enhetskartlegging</translation>
    </message>
    <message>
        <source>Shows selected mapper ancestry and device-mapper state when available.</source>
        <translation>Viser valgt kartlegger forfedre og enhetskarttilstand når det er tilgjengelig.</translation>
    </message>
    <message>
        <source>LUKS / crypttab evidence</source>
        <translation>LUKS / krypttab bevis</translation>
    </message>
    <message>
        <source>Shows LUKS/mapped ancestry plus crypttab and fstab mapper references.</source>
        <translation>Viser LUKS/kartlagt opphav pluss crypttab og fstab kartleggere referanser.</translation>
    </message>
    <message>
        <source>Full diagnostic report</source>
        <translation>Full diagnostisk rapport</translation>
    </message>
    <message>
        <source>Combines all read-only diagnostics for the selected scope (same as Run All).</source>
        <translation>Kombinerer alle lesebeskyttede diagnostikk for det valgte omfanget (som Run All).</translation>
    </message>
    <message>
        <source>All entries</source>
        <translation>Alle oppføringer</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>Diagnostikk</translation>
    </message>
    <message>
        <source>Repairs</source>
        <translation>Reparasjoner</translation>
    </message>
    <message>
        <source>Package repair</source>
        <translation>Pakkereparasjon</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Filkopi</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Oppdaging av enhet</translation>
    </message>
    <message>
        <source>Host</source>
        <translation>Vært</translation>
    </message>
    <message>
        <source>Required for block-device inventory</source>
        <translation>Nødvendig for blokkeringsoversikt</translation>
    </message>
    <message>
        <source>Filesystem identification</source>
        <translation>Filsystemidentifikasjon</translation>
    </message>
    <message>
        <source>Used to identify filesystem metadata</source>
        <translation>Brukes til å identifisere metadata for filsystemer</translation>
    </message>
    <message>
        <source>Mount inspection</source>
        <translation>Monteringskontroll</translation>
    </message>
    <message>
        <source>Used to understand active mounts</source>
        <translation>Brukes til å forstå aktive monteringer</translation>
    </message>
    <message>
        <source>LUKS support</source>
        <translation>LUKS-støtte</translation>
    </message>
    <message>
        <source>Required to unlock encrypted targets</source>
        <translation>Nødvendig for å låse opp krypterte mål</translation>
    </message>
    <message>
        <source>Btrfs support</source>
        <translation>Btrfs-støtte</translation>
    </message>
    <message>
        <source>Required for Btrfs inspection and snapshot rollback</source>
        <translation>Nødvendig for Btrfs inspeksjon og øyeblikksbilde tilbakerulling</translation>
    </message>
    <message>
        <source>Bidirectional file copy</source>
        <translation>Bidirectional fil copy</translation>
    </message>
    <message>
        <source>Host/Repair</source>
        <translation>Vært/reparasjon</translation>
    </message>
    <message>
        <source>Required for verified Host-to-Repair and Repair-to-Host transfer</source>
        <translation>Nødvendig for verifisert vert-til-reparasjon og reparasjon-til-vert overføring</translation>
    </message>
    <message>
        <source>Chroot repair</source>
        <translation>Kroot reparasjon</translation>
    </message>
    <message>
        <source>Required for target-side repair commands</source>
        <translation>Nødvendig for reparasjonskommandoer på målsiden</translation>
    </message>
    <message>
        <source>Offline systemd repair</source>
        <translation>Frakoblet systemreparasjon</translation>
    </message>
    <message>
        <source>Used to restore graphical.target and the configured display manager without starting the target GUI</source>
        <translation>Brukes til å gjenopprette grafisk. mål og den konfigurerte skjermhåndtereren uten å starte målgrensesnittet</translation>
    </message>
    <message>
        <source>UEFI NVRAM inspection</source>
        <translation>UEFI NVRAM inspeksjon</translation>
    </message>
    <message>
        <source>Used to preserve target EFI BootOrder during TUXEDO UKI rebuilds when efivars are available</source>
        <translation>Brukes til å bevare målet EFI BootOrder under TUXEDO UKI gjenoppbygging når efivars er tilgjengelige</translation>
    </message>
    <message>
        <source>UKI verification</source>
        <translation>UKI verifisering</translation>
    </message>
    <message>
        <source>Used to verify the kernel embedded in a rebuilt unified kernel image</source>
        <translation>Brukes til å verifisere kjernen innebygd i et gjenoppbygd samlet kjernebilde</translation>
    </message>
    <message>
        <source>GRUB EFI repair</source>
        <translation>GRUB EFI reparasjon</translation>
    </message>
    <message>
        <source>Target/Host</source>
        <translation>Mål/vert</translation>
    </message>
    <message>
        <source>Required only for conventional GRUB-based EFI systems</source>
        <translation>Kun nødvendig for konvensjonelle GRUB-baserte EFI-systemer</translation>
    </message>
    <message>
        <source>Target</source>
        <translation>Mål</translation>
    </message>
    <message>
        <source>Debian-family GRUB helper</source>
        <translation>Debian-familie GRUB-hjelper</translation>
    </message>
    <message>
        <source>Portable GRUB configuration generator used by Arch and other non-Debian systems</source>
        <translation>Bærbar GRUB konfigurasjonsgenerator som brukes av Arch og andre ikke-debiske systemer</translation>
    </message>
    <message>
        <source>Initramfs rebuild</source>
        <translation>Initramfs gjenoppbygging</translation>
    </message>
    <message>
        <source>Debian-family initramfs helper</source>
        <translation>Debian-familien initramfs-hjelper</translation>
    </message>
    <message>
        <source>Arch-family initramfs generator</source>
        <translation>Arch-familie initramfs generator</translation>
    </message>
    <message>
        <source>Alternative initramfs generator used by Arch and other distributions</source>
        <translation>Alternativ initramfs generator som brukes av Arch og andre distribusjoner</translation>
    </message>
    <message>
        <source>Initramfs verification</source>
        <translation>Initramfs verifisering</translation>
    </message>
    <message>
        <source>Read-only verification for mkinitcpio images</source>
        <translation>Lesbar verifisering av mkinitcpio-bilder</translation>
    </message>
    <message>
        <source>Read-only verification for dracut images</source>
        <translation>Lesbare verifisering av dracut bilder</translation>
    </message>
    <message>
        <source>systemd-boot inspection</source>
        <translation>systemd-boot inspeksjon</translation>
    </message>
    <message>
        <source>Host/Target</source>
        <translation>Vært/Target</translation>
    </message>
    <message>
        <source>Read-only inspection of systemd-boot and generic UKI layouts</source>
        <translation>Lesbar inspeksjon av systemd-boot og generiske UKI layouter</translation>
    </message>
    <message>
        <source>Arch package manager</source>
        <translation>Arch pakke manager</translation>
    </message>
    <message>
        <source>Arch-family package database and transaction tool</source>
        <translation>Arkivpakkedatabase og transaksjonsverktøy</translation>
    </message>
    <message>
        <source>DKMS rebuild</source>
        <translation>DKMS gjenoppbygging</translation>
    </message>
    <message>
        <source>Required only when target uses DKMS modules</source>
        <translation>Trenger kun når målet bruker DKMS-moduler</translation>
    </message>
    <message>
        <source>LVM inspection</source>
        <translation>LVM-kontroll</translation>
    </message>
    <message>
        <source>Optional LVM storage-stack support</source>
        <translation>Valgfri LVM-lagringsstøtte</translation>
    </message>
    <message>
        <source>Software RAID</source>
        <translation>Programvare-RAID</translation>
    </message>
    <message>
        <source>Optional Linux MD RAID support</source>
        <translation>Valgfri Linux MD RAID-støtte</translation>
    </message>
    <message>
        <source>Process namespace isolation</source>
        <translation>Prosessnavnerommet isolasjon</translation>
    </message>
    <message>
        <source>Host+Target</source>
        <translation>Vert+ Mål</translation>
    </message>
    <message>
        <source>The ported helper falls back to a guarded plain chroot when unshare is absent</source>
        <translation>Den portede hjelperen faller tilbake til en bevoktet vanlig krone når unshare er fraværende</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Avbryt</translation>
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
        <translation>Nei</translation>
    </message>
    <message>
        <source>read-only helper diagnostic</source>
        <translation>lesbar hjelpediagnostikk</translation>
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
        <translation>Linux gjenoppretting og oppstartsreparasjonsverktøy</translation>
    </message>
    <message>
        <source>GUARDED REPAIR</source>
        <translation>GUARDERT REPAIR</translation>
    </message>
    <message>
        <source>Ordinary repairs require an explicitly selected non-host target. The protected running host has a separate deliberate maintenance mode with the same guarded repair stages and requires privilege authorization.</source>
        <translation>Vanlige reparasjoner krever et eksplisitt valgt ikke-vert-mål. Den beskyttede løpsverten har en egen bevisst vedlikeholdsmodus med de samme bevoktede reparasjonsfasene og krever privilegiumstillatelse.</translation>
    </message>
    <message>
        <source>Systems</source>
        <translation>Systemer</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>Diagnostikk</translation>
    </message>
    <message>
        <source>Repair</source>
        <translation>Reparasjon</translation>
    </message>
    <message>
        <source>Chroot Shell</source>
        <translation>Groot Shell</translation>
    </message>
    <message>
        <source>File Copy</source>
        <translation>Filkopi</translation>
    </message>
    <message>
        <source>Logs</source>
        <translation>Logger</translation>
    </message>
    <message>
        <source>Settings</source>
        <translation>Innstillinger</translation>
    </message>
    <message>
        <source>(not created yet)</source>
        <translation>(Ikke laget enda)</translation>
    </message>
    <message>
        <source>Information</source>
        <translation>Informasjon</translation>
    </message>
    <message>
        <source>Close</source>
        <translation>Lukk</translation>
    </message>
    <message>
        <source>&amp;File</source>
        <translation>-&amp;fil</translation>
    </message>
    <message>
        <source>&amp;Refresh Devices</source>
        <translation>&amp;Oppfrisk enheter</translation>
    </message>
    <message>
        <source>&amp;Lock Administrator Session</source>
        <translation>&amp;Lock Administrator Session</translation>
    </message>
    <message>
        <source>&amp;Quit</source>
        <translation>&amp;Avslutt</translation>
    </message>
    <message>
        <source>&amp;View</source>
        <translation>&amp;Vis</translation>
    </message>
    <message>
        <source>&amp;Systems</source>
        <translation>&amp;Systems</translation>
    </message>
    <message>
        <source>&amp;Diagnostics</source>
        <translation>&amp;diagnostikk</translation>
    </message>
    <message>
        <source>&amp;Logs</source>
        <translation>&amp;Logs</translation>
    </message>
    <message>
        <source>&amp;Settings</source>
        <translation>-&amp;innstillinger</translation>
    </message>
    <message>
        <source>&amp;Auto-size Device Columns</source>
        <translation>&amp;Auto-størrelse enhetskolonner</translation>
    </message>
    <message>
        <source>&amp;Wrap Log Lines</source>
        <translation>&amp;Wrap Log Lines</translation>
    </message>
    <message>
        <source>&amp;Help</source>
        <translation>&amp;Hjelp</translation>
    </message>
    <message>
        <source>&amp;Using Boot Bitch</source>
        <translation>&amp;bruker Boot Bitch</translation>
    </message>
    <message>
        <source>&amp;About Boot Bitch</source>
        <translation>&amp;Om Boot Bitch</translation>
    </message>
    <message>
        <source>Device columns auto-sized. Drag headers to fine-tune widths.</source>
        <translation>Enhetskolonner automatisk. Dra overskrifter til finpunne bredder.</translation>
    </message>
    <message>
        <source>A helper command is running; wait for it to finish before locking the session.</source>
        <translation>En hjelpekommando kjører; vent på at den skal avsluttes før den låses.</translation>
    </message>
    <message>
        <source>Administrator session locked; the next privileged action will request authorization.</source>
        <translation>Administrator økt låst; neste privilegerte handling vil be om autorisasjon.</translation>
    </message>
    <message>
        <source>Using Boot Bitch</source>
        <translation>Bruker Boot Bitch</translation>
    </message>
    <message>
        <source>Boot Bitch must run from a different booted Linux environment than the system being repaired. Use a Linux live medium or another Linux installation on a different physical drive.&lt;br&gt;&lt;br&gt;The running host is protected from ordinary repair-target selection, but it can be explicitly selected through &lt;b&gt;Host Maintenance&lt;/b&gt; for guarded native diagnostics and supported maintenance stages.&lt;br&gt;&lt;br&gt;Diagnostics follow the Systems page: the committed repair drive while Host Maintenance is off, or the protected running host while it is active.&lt;br&gt;&lt;br&gt;The first privileged action requests administrator authorization once for this Boot Bitch window; &lt;b&gt;File - Lock Administrator Session&lt;/b&gt; ends that helper session immediately. Every repair keeps the helper&apos;s own runtime preflights.</source>
        <translation>Boot Bitch må kjøre fra en annen oppstart Linux-miljø enn systemet som blir reparert. Bruk en Linux live-medium eller en annen Linux-installasjon på en annen fysisk stasjon.&lt;br&gt;&lt;br&gt; Kjøreverten er beskyttet mot vanlig reparasjonsmålvalg, men den kan eksplisitt velges gjennom &lt;b&gt;Host vedlikehold&lt;/b&gt; for bevoktet morsmålsdiagnosikk og støttet vedlikeholdsstadier.&lt;br&gt;&lt;br&gt; Diagnostika følger systemsiden: den engasjerte reparasjonsstasjonen mens vertsvedlikehold er av, eller den beskyttede kjørerverten mens den er aktiv.&lt;br&gt;&lt;br&gt; Den første privilegerte handlingen ber administrator om autorisasjon én gang for dette Boot Bitch-vinduet; &lt;b&gt;File - Lock Administrator Session&lt;/b&gt; avslutter den hjelpeøkten umiddelbart. Hver reparasjon holder hjelpens egne kjøretid førfly.</translation>
    </message>
    <message>
        <source>Select a physical drive; Boot Bitch resolves the most likely Linux system volume automatically. The running host stays protected from ordinary target repairs, with a separate explicit host-maintenance path for its own system. The Details button shows the protected host&apos;s facts in the details pane; selecting any drive row restores the per-drive pane.</source>
        <translation>Velg en fysisk stasjon; Boot Bitch løser det mest sannsynlige Linux systemvolumet automatisk. Den løpende verten forblir beskyttet mot vanlige målreparasjoner, med en separat eksplisitt vertsvedlikeholdsvei for sitt eget system. Detaljer-knappen viser den beskyttede vertens fakta i detaljruten; å velge en enhetsrad gjenoppretter per drivruten.</translation>
    </message>
    <message>
        <source>Refresh Devices</source>
        <translation>Oppdater enheter</translation>
    </message>
    <message>
        <source>Re-read the read-only kernel inventory (/proc/partitions, /proc/mounts, /proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* and the udev metadata database). No block device is opened and nothing is written.</source>
        <translation>Les om den lesebeskyttede kjerneoversikten (/proc/partitions, /proc/mounts, /proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* og udev metadata databasen). Ingen blokkenhet er åpnet og ingenting er skrevet.</translation>
    </message>
    <message>
        <source>[OK]</source>
        <translation>[OK]</translation>
    </message>
    <message>
        <source>The running system was detected and stays protected from ordinary target repairs.</source>
        <translation>Kjøresystemet ble oppdaget og forblir beskyttet mot vanlige målreparasjoner.</translation>
    </message>
    <message>
        <source>Detecting running system...</source>
        <translation>Oppdager kjøresystem...</translation>
    </message>
    <message>
        <source>Detecting protected storage...</source>
        <translation>Oppdage beskyttet lagring...</translation>
    </message>
    <message>
        <source>PROTECTED</source>
        <translation>PROTECTED</translation>
    </message>
    <message>
        <source>The running host remains protected from ordinary repair-target operations.</source>
        <translation>Den løpende verten forblir beskyttet mot vanlige reparasjonsmål.</translation>
    </message>
    <message>
        <source>Details</source>
        <translation>Detaljer</translation>
    </message>
    <message>
        <source>Show read-only details for the protected running host.</source>
        <translation>Vis skrivebeskyttede detaljer for den beskyttede verten.</translation>
    </message>
    <message>
        <source>Host Maintenance</source>
        <translation>Vertsvedlikehold</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Gjør standard</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host (menu.lst default directive with a backup and rollback). Requires Host Maintenance and the cached host-default probe.</source>
        <translation>Gjør den kanonisk installerte kjerneoppføringen til standard GRUB-legacy oppstartsoppføring på den kjørende verten (menu.lt standarddirektiv med en sikkerhetskopi og rulle tilbake). Krever vertsvedlikehold og den cachede vertsstandardproben.</translation>
    </message>
    <message>
        <source>Available repair targets</source>
        <translation>Tilgjengelige reparasjonsmål</translation>
    </message>
    <message>
        <source>Most likely first</source>
        <translation>Mest sannsynlig først</translation>
    </message>
    <message>
        <source>Drives are listed by the read-only inventory. Select a row to inspect it; Select Target commits the selected non-host drive with its auto-resolved Linux root component.</source>
        <translation>Driver er oppført av den lesebeskyttede inventar. Velg en rad for å inspisere den; Velg Target forplikter den valgte ikke-vert-stasjonen med sin automatisk løst Linux rotkomponent.</translation>
    </message>
    <message>
        <source>Device</source>
        <translation>Enhet</translation>
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
        <translation>Velg mål</translation>
    </message>
    <message>
        <source>Unlock</source>
        <translation>Lås opp</translation>
    </message>
    <message>
        <source>Authorize</source>
        <translation>autorisering</translation>
    </message>
    <message>
        <source>Establish the privileged helper session for the current scope now instead of waiting for the next privileged action.</source>
        <translation>Etabler den privilegerte hjelpeøkten for det nåværende omfanget nå i stedet for å vente på neste privilegerte handling.</translation>
    </message>
    <message>
        <source>Committed target: none</source>
        <translation>Besvart mål: ingen</translation>
    </message>
    <message>
        <source>Unlock state for the selected drive; the LUKS passphrase is never logged.</source>
        <translation>Lås opp tilstand for den valgte stasjonen; LUKS passord er aldri logget.</translation>
    </message>
    <message>
        <source>Unlock status</source>
        <translation>Lås opp status</translation>
    </message>
    <message>
        <source>Read-only inventory plus helper-confirmed facts; mirrors the modern Qt6 Selected drive details panel.</source>
        <translation>Lesbar inventar pluss hjelpebekreftede fakta; speiler det moderne Qt6 valgte drivdetaljpanelet.</translation>
    </message>
    <message>
        <source>Selected drive details</source>
        <translation>Utvalgte kjøredetaljer</translation>
    </message>
    <message>
        <source>Field</source>
        <translation>Felt</translation>
    </message>
    <message>
        <source>Value</source>
        <translation>Verdi</translation>
    </message>
    <message>
        <source>Run All runs every available read-only diagnostic for the current scope; selecting a check runs it alone. Diagnostics are read-only and are the only evidence source for the gated repair actions.</source>
        <translation>Kjør alle kjører alle tilgjengelige lesebare diagnostikk for det gjeldende omfanget; velger en sjekk kjører det alene. Diagnostics er kun leselige og er den eneste beviskilden for de gated reparasjon handlinger.</translation>
    </message>
    <message>
        <source>Target: none selected</source>
        <translation>Mål: ingen valgt</translation>
    </message>
    <message>
        <source>Diagnostics follow the committed repair target, or the protected running host while Host Maintenance is active.</source>
        <translation>Diagnostikere følger det forpliktede reparasjonsmålet, eller den beskyttede løpende verten mens Host Maintenance er aktiv.</translation>
    </message>
    <message>
        <source>Run All</source>
        <translation>Kjør alle</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope.</source>
        <translation>Kjør alle - kjøre alle tilgjengelige lesebare diagnostikk for det aktuelle omfanget.</translation>
    </message>
    <message>
        <source>Target configuration:</source>
        <translation>Målkonfigurasjon:</translation>
    </message>
    <message>
        <source>Etch-era target configuration files; availability is probed read-only by the helper&apos;s diagnostics.</source>
        <translation>Etch-era-målkonfigurasjonsfiler; tilgjengelighet er kun lesebeskyttet av hjelperens diagnostikk.</translation>
    </message>
    <message>
        <source>Edit Target File...</source>
        <translation>Rediger målfil...</translation>
    </message>
    <message>
        <source>Runs one read-only diagnostic for the selected scope through the helper (`diagnose &lt;key&gt;` / `host-diagnose &lt;key&gt;`); Run All is the combined report.</source>
        <translation>Kjører én lesebeskyttet diagnostikk for det valgte omfanget gjennom hjelperen («diagnose &lt;key&gt;») / «host-diagnose &lt;key&gt;»); Kjør alt er den kombinerte rapporten.</translation>
    </message>
    <message>
        <source>Diagnostic checks</source>
        <translation>Diagnostiske kontroller</translation>
    </message>
    <message>
        <source>Check</source>
        <translation>Sjekk</translation>
    </message>
    <message>
        <source>Selected diagnostic</source>
        <translation>Valgt diagnostikk</translation>
    </message>
    <message>
        <source>Select a diagnostic</source>
        <translation>Velg en diagnose</translation>
    </message>
    <message>
        <source>Choose a diagnostic from the list.</source>
        <translation>Velg diagnose fra listen.</translation>
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
        <translation>Kjør diagnostisk</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic for the current scope.</source>
        <translation>Kjør diagnostikk - kjør den valgte skrivebeskyttede diagnosen for det aktuelle omfanget.</translation>
    </message>
    <message>
        <source>Copy Results</source>
        <translation>Kopier resultat</translation>
    </message>
    <message>
        <source>Save Results...</source>
        <translation>Lagre resultater...</translation>
    </message>
    <message>
        <source>The Full Repair plan runs the selected legacy stages in order through the guarded helper; the individual tools run one stage at a time. Every action stays disabled until the cached capability lines say available and the helper keeps its runtime preflights.</source>
        <translation>Full Repair-planen kjører de valgte arvestadiene i rekkefølge gjennom den bevoktede hjelperen; de enkelte verktøyene kjører ett trinn om gangen. Hver handling forblir deaktivert til de cachede funksjonslinjene sier tilgjengelig og hjelperen holder sin kjøretid forhåndsbelysning.</translation>
    </message>
    <message>
        <source>Full Repair plan</source>
        <translation>Full reparasjonsplan</translation>
    </message>
    <message>
        <source>No stages selected</source>
        <translation>Ingen stadier valgt</translation>
    </message>
    <message>
        <source>Configure Plan...</source>
        <translation>Konfigurer plan...</translation>
    </message>
    <message>
        <source>Open Settings to choose which Full Repair stages are part of the plan.</source>
        <translation>Åpne innstillinger for å velge hvilke full reparasjonsfaser som er en del av planen.</translation>
    </message>
    <message>
        <source>Run Full Repair</source>
        <translation>Kjør full reparasjon</translation>
    </message>
    <message>
        <source>Select a repair drive, or choose Host Maintenance on the protected running-host card.</source>
        <translation>Velg en reparasjonsstasjon, eller velg vertsvedlikehold på det beskyttede kjører-vert-kortet.</translation>
    </message>
    <message>
        <source>Stage</source>
        <translation>Stage</translation>
    </message>
    <message>
        <source>Individual repair tools</source>
        <translation>Individuelle reparasjonsverktøy</translation>
    </message>
    <message>
        <source>Tool</source>
        <translation>Verktøy</translation>
    </message>
    <message>
        <source>Full Repair</source>
        <translation>Full Reparasjon</translation>
    </message>
    <message>
        <source>not reported</source>
        <translation>ikke rapportert</translation>
    </message>
    <message>
        <source>Selected tool</source>
        <translation>Valgt verktøy</translation>
    </message>
    <message>
        <source>Select a repair tool</source>
        <translation>Velg et reparasjonsverktøy</translation>
    </message>
    <message>
        <source>Run Tool</source>
        <translation>Kjøreverktøy</translation>
    </message>
    <message>
        <source>Select a tool to review its repair action.</source>
        <translation>Velg et verktøy for å se gjennom reparasjonshandlingen.</translation>
    </message>
    <message>
        <source>Write actions ask for confirmation and then run the helper&apos;s own runtime preflights; the GUI never weakens them. A repair that is not proven &apos;unchanged&apos; invalidates the cached diagnostics and disables the gated actions until diagnostics run again.</source>
        <translation>Skriv handlinger ber om bekreftelse og kjør deretter hjelperens egen kjøretid preflights; GUI svekker dem aldri. En reparasjon som ikke er bevist &apos;uendret&apos; ugyldiggjør de cachede diagnostikkene og deaktiverer de gated actions til diagnosen kjører igjen.</translation>
    </message>
    <message>
        <source>Chroot shell</source>
        <translation>Root skall</translation>
    </message>
    <message>
        <source>Offline commands run one at a time in a fresh chroot and cannot answer interactive prompts (apt-get -y upgrade works). Host-shell commands run directly on the running host. The helper&apos;s probe lines gate the command field; the exact reason appears in the tooltip.</source>
        <translation>Frakoblede kommandoer kjører en om gangen i en frisk chroot og kan ikke svare på interaktive spørsmål (apt-get -y oppgradering fungerer). Host-shell-kommandoer kjører direkte på den løpende verten. Hjelperens probe linjer port kommandofeltet; den nøyaktige grunnen vises i verktøytipset.</translation>
    </message>
    <message>
        <source>No &apos;Legacy feature shell:&apos; line is cached; run diagnostics for the selected scope to evaluate the helper&apos;s chroot/timeout containment probes (fail closed).</source>
        <translation>Ingen &apos;Legacy-funksjonsskal:&apos;-linje er cached; kjøre diagnostikk for det valgte omfanget for å evaluere hjelperens rot-/tidutholdningssonder (feil lukket).</translation>
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
        <translation>En anmeldt kommandostreng, sendt til hjelperen som et enkelt argument (ingen skal interpolasjon av GUI).</translation>
    </message>
    <message>
        <source>Run Command</source>
        <translation>Kjør kommando</translation>
    </message>
    <message>
        <source>Clear Output</source>
        <translation>Tøm utdata</translation>
    </message>
    <message>
        <source>The helper exposes `shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for an offline target chroot and `host-shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for the running host. Both keep the helper&apos;s runtime preflights; this tab enables the command only when the scope is committed, the session is authorized and the scope&apos;s Legacy feature probe reports available.</source>
        <translation>Hjelperen avslører `shell &lt;disk&gt; &lt;root&gt; &lt;kommando&gt;` for en offline målkrone og `host-shell &lt;disk&gt; &lt;root&gt; &lt;kommando&gt;` for den løpende verten. Begge holder hjelpens kjøretid preflights; Denne fanen aktiverer kommandoen bare når omfanget er utført, sesjonen er autorisert og omfangets Legacy-funksjon sonde rapporter tilgjengelig.</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Filkopi</translation>
    </message>
    <message>
        <source>Copy and verify files in either direction through the guarded helper (cp -a plus ownership restoration and a per-file byte-compare). The helper&apos;s file-copy probe gates the controls and keeps the direction and path containment checks.</source>
        <translation>Kopier og verifiser filer i hver retning gjennom den bevoktede hjelpen (cp -a pluss eierskap restaurering og en per-fil byte-kompare). Hjelperens fil-kopi sondeporter kontroller og holder retning og sti inneslutningskontroller.</translation>
    </message>
    <message>
        <source>Preview Changes</source>
        <translation>Forhåndsvisningsendringer</translation>
    </message>
    <message>
        <source>Run a copy dry-run through the guarded helper. No files are changed.</source>
        <translation>Kjør en kopi tørrkjørt gjennom den bevoktede hjelpen. Ingen filer endres.</translation>
    </message>
    <message>
        <source>Copy and Verify</source>
        <translation>Kopier og verifiser</translation>
    </message>
    <message>
        <source>Copy staged items and verify the result. Existing destination names are overwritten when source content differs; unrelated destination files are never deleted.</source>
        <translation>Kopier iscenesatte elementer og verifiser resultatet. Eksisterende destinasjonsnavn overskrives når kildeinnholdet er forskjellig; ikke-relaterte destinasjonsfiler slettes aldri.</translation>
    </message>
    <message>
        <source>Direction:</source>
        <translation>Retning:</translation>
    </message>
    <message>
        <source>Choose which system supplies the source files and which system receives them.</source>
        <translation>Velg hvilket system som leverer kildefiler og hvilket system som mottar dem.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from this host</source>
        <translation>1. Velg kildefiler eller mapper fra denne verten</translation>
    </message>
    <message>
        <source>Source</source>
        <translation>Kilde</translation>
    </message>
    <message>
        <source>Files and folders staged for the verified copy. The legacy backend copies with cp -a and restores ownership with chown --reference; every regular file is byte-compared after the copy.</source>
        <translation>Filer og mapper arrangert for den bekreftede kopien. De gamle backend kopier med cp -a og gjenoppretter eierskap med chown --referanse; hver vanlig fil er byte-sammenliknet etter kopien.</translation>
    </message>
    <message>
        <source>Add Files...</source>
        <translation>Legg til filer...</translation>
    </message>
    <message>
        <source>Add Folder...</source>
        <translation>Legg til mappe...</translation>
    </message>
    <message>
        <source>Remove</source>
        <translation>Fjern</translation>
    </message>
    <message>
        <source>Clear</source>
        <translation>Fjern</translation>
    </message>
    <message>
        <source>Clear the staged source list (nothing is copied or deleted).</source>
        <translation>Tøm den iscenesatte kildelisten (ingenting kopieres eller slettes).</translation>
    </message>
    <message>
        <source>2. Choose destination in repaired system</source>
        <translation>2. Velg destinasjon i reparert system</translation>
    </message>
    <message>
        <source>unavailable: see the helper&apos;s Legacy feature file-copy: probe reason above</source>
        <translation>utilgjengelig: se hjelperens Legacy-funksjon fil-kopi: sonde grunn over</translation>
    </message>
    <message>
        <source>An absolute path inside the selected repair system (Host to Repair) or on the running host (Repair to Host).</source>
        <translation>En absolutt vei inne i det valgte reparasjonssystemet (Vert å reparere) eller på den kjørende verten (reparasjon til verten).</translation>
    </message>
    <message>
        <source>Browse Target Folders...</source>
        <translation>Bla gjennom målmapper...</translation>
    </message>
    <message>
        <source>Browse the selected repair system through the helper&apos;s temporary read-only mounts and choose an absolute destination path. No target files are changed while browsing.</source>
        <translation>Bla gjennom det valgte reparasjonssystemet gjennom hjelperens midlertidige skrivebeskyttede monteringer og velg en absolutt destinasjonssti. Ingen målfiler endres mens du surfer.</translation>
    </message>
    <message>
        <source>3. Ownership and copy policy</source>
        <translation>3. Eierskap og kopipolitikk</translation>
    </message>
    <message>
        <source>Ownership:</source>
        <translation>Eierskap:</translation>
    </message>
    <message>
        <source>Smart destination ownership (recommended)</source>
        <translation>Smart destinasjon eierskap (anbefalt)</translation>
    </message>
    <message>
        <source>Preserve source numeric UID/GID</source>
        <translation>Bevar kilde numerisk UID/GID</translation>
    </message>
    <message>
        <source>Smart mode validates UID/GID identity mapping across the two systems and falls back to the destination-directory owner when the same numeric ID means a different account (the legacy backend implements it with chown --reference).</source>
        <translation>Smart modus validerer UID/GID-identitetskartlegging på tvers av de to systemene og faller tilbake til destinasjonskataloge eieren når samme numeriske ID betyr en annen konto (den gamle motoren implementerer den med chown --referanse).</translation>
    </message>
    <message>
        <source>Application log</source>
        <translation>Programlogg</translation>
    </message>
    <message>
        <source>The complete session register; Save As... writes every entry even while a filter hides lines. If a writable system share mount exists at /host, Save As... starts there; otherwise the log directory is the fallback. Prior session files are listed read-only.</source>
        <translation>Det komplette øktregisteret; Lagre som... skriver hver oppføring selv om et filter skjuler linjer. Hvis det finnes en bærbar systemdelingsmontering ved /host, starter Lagre som... der; ellers er loggkatalogen tilbakefall. Tidligere øktfiler er oppført skrivebare.</translation>
    </message>
    <message>
        <source>Session logs</source>
        <translation>Øktlogger</translation>
    </message>
    <message>
        <source>Session</source>
        <translation>Sesjon</translation>
    </message>
    <message>
        <source>The first entry is the live session; earlier files in the log directory are listed read-only below it.</source>
        <translation>Den første oppføringen er live-økten; tidligere filer i loggkatalogen er oppført bare under den.</translation>
    </message>
    <message>
        <source>New Session Log</source>
        <translation>Ny øktlogg</translation>
    </message>
    <message>
        <source>Close the active session file; it becomes a prior session and the next log entry starts a new file.</source>
        <translation>Lukk den aktive øktfilen; det blir en tidligere økt og neste loggoppføring starter en ny fil.</translation>
    </message>
    <message>
        <source>Add Note</source>
        <translation>Legg til notat</translation>
    </message>
    <message>
        <source>Append a NOTE entry to the live session register.</source>
        <translation>Legg til en NOTE-oppføring i live-øktsregisteret.</translation>
    </message>
    <message>
        <source>Delete</source>
        <translation>Slett</translation>
    </message>
    <message>
        <source>Delete the selected prior session file (the live session is never deleted).</source>
        <translation>Slett den valgte tidligere øktfilen (den live sesjonen slettes aldri).</translation>
    </message>
    <message>
        <source>Refresh</source>
        <translation>Oppdater</translation>
    </message>
    <message>
        <source>Save As...</source>
        <translation>Lagre som...</translation>
    </message>
    <message>
        <source>Save the complete session log (all entries, not just the current filter).</source>
        <translation>Lagre den komplette øktloggen (alle oppføringene, ikke bare det gjeldende filteret).</translation>
    </message>
    <message>
        <source>Clear Register</source>
        <translation>Fjern register</translation>
    </message>
    <message>
        <source>Clear the live register and view; prior session files are never modified.</source>
        <translation>Tøm liveregisteret og vis; tidligere øktfiler endres aldri.</translation>
    </message>
    <message>
        <source>Search log:</source>
        <translation>Søkelogg:</translation>
    </message>
    <message>
        <source>Type any characters to show matching log entries (case-insensitive). Save As always writes every entry.</source>
        <translation>Skriv inn tegn for å vise matchende loggoppføringer (case-insensitive). Lagre Som alltid skriver alle innlegg.</translation>
    </message>
    <message>
        <source>Filter:</source>
        <translation>Filtrer:</translation>
    </message>
    <message>
        <source>Filter the visible log by entry kind. Selecting a diagnostic section shows the lines captured for that section; a workflow filter such as File system repair or Package repair shows its mapped repair lines (File copy has no lines in this frontend). Save As always writes every entry.</source>
        <translation>Filtrer den synlige loggen etter oppføringstype. Velger du en diagnostisk seksjon viser linjer som er tatt opp for den delen; et arbeidsflytfilter som filsystemreparasjon eller pakkereparasjon viser sine kartlagde reparasjonslinjer (Filkopien har ingen linjer i denne frontenden). Lagre Som alltid skriver alle innlegg.</translation>
    </message>
    <message>
        <source>Settings are stored per user under ~/.qt/, one file per settings group (devicesrc, logsrc, diagnosticsrc, repairrc), and are saved immediately on every change and on close. Launch the GUI as the same user to keep your overrides; a GUI started as root keeps its own copies.</source>
        <translation>Innstillinger lagres per bruker under ~/.qt/, en fil per innstillingsgruppe (devicesrc, logsrc, diagnosticsrc, reparatrc), og lagres umiddelbart på hver endring og på slutten. Start GUI som samme bruker for å holde overstyrene dine; et GUI startet som root holder sine egne kopier.</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Oppdaging av enhet</translation>
    </message>
    <message>
        <source>Show devices without an identified Linux installation</source>
        <translation>Vis enheter uten identifisert Linux-installasjon</translation>
    </message>
    <message>
        <source>Show removable and USB storage</source>
        <translation>Vis flyttbar og USB-lagring</translation>
    </message>
    <message>
        <source>Show encrypted devices before unlocking</source>
        <translation>Vis krypterte enheter før de låses opp</translation>
    </message>
    <message>
        <source>When off, drives without a visible Linux filesystem are hidden unless they still contain an encrypted device and encrypted devices are shown.</source>
        <translation>Når av, blir stasjoner uten et synlig Linux-filsystem skjult med mindre de fortsatt inneholder en kryptert enhet og krypterte enheter vises.</translation>
    </message>
    <message>
        <source>When off, removable and USB drives are hidden from the repair-target list.</source>
        <translation>Når av, er flyttbare og USB-stasjoner skjult fra reparasjonsmållisten.</translation>
    </message>
    <message>
        <source>When off, drives with an encrypted device are hidden until the volume is unlocked.</source>
        <translation>Når av, er stasjoner med en kryptert enhet skjult til volumet er låst.</translation>
    </message>
    <message>
        <source>Include the %1 stage in the Full Repair plan. The stage runs in the plan order shown on the Repair tab.</source>
        <translation>Ta med %1 i hele reparasjonen. Sceneet kjører i planfølgen vist på fanen Reparasjon.</translation>
    </message>
    <message>
        <source>Automatically regenerate read-only diagnostics after repairs or target changes</source>
        <translation>Automatisk regenerere lesebeskyttede diagnoser etter reparasjon eller målendringer</translation>
    </message>
    <message>
        <source>Regenerates the cached read-only diagnostics for the current scope after an operation that invalidates them (LUKS unlock, target configuration edit). It runs only inside an already authorized administrator session and never opens an authorization prompt by itself.</source>
        <translation>Genererer den cachede lesebeskyttede diagnostikken for gjeldende omfang etter en operasjon som ugyldiggjør dem (LUKS låser opp, målkonfigurasjonsredigering). Det kjører bare inne i en allerede autorisert administrator økt og åpner aldri en autorisasjonsprompt av seg selv.</translation>
    </message>
    <message>
        <source>Wrap long log lines</source>
        <translation>Bryt lange logglinjer</translation>
    </message>
    <message>
        <source>Mandatory safety controls</source>
        <translation>Obligatorisk sikkerhetskontroll</translation>
    </message>
    <message>
        <source>Host capabilities and dependencies</source>
        <translation>Vertsfunksjoner og avhengigheter</translation>
    </message>
    <message>
        <source>Refresh Capabilities</source>
        <translation>Oppfrisk funksjoner</translation>
    </message>
    <message>
        <source>Re-run the read-only host capability probes (a PATH search, nothing is executed) and refresh the distribution summary.</source>
        <translation>Kjør de skrivebeskyttede vertskapasitetsproben (et PATH-søk, ingenting utføres) og oppdatere distribusjonssamandraget.</translation>
    </message>
    <message>
        <source>Install Missing Support...</source>
        <translation>Installere manglende støtte...</translation>
    </message>
    <message>
        <source>Automatic installation will require explicit package mapping and privilege authorization.</source>
        <translation>Automatisk installasjon vil kreve eksplisitt pakkekartlegging og privilegier autorisasjon.</translation>
    </message>
    <message>
        <source>Application configuration</source>
        <translation>Programoppsett</translation>
    </message>
    <message>
        <source>Boot Bitch Legacy</source>
        <translation>Boot Bitch Legacy</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list first.</source>
        <translation>Velg en fysisk stasjon i listen over tilgjengelige reparasjonsmål først.</translation>
    </message>
    <message>
        <source>Protected system</source>
        <translation>Beskyttet system</translation>
    </message>
    <message>
        <source>The running system cannot be selected as a repair target. Use Host Maintenance for the protected running host or choose another disk.</source>
        <translation>Kjøresystemet kan ikke velges som reparasjonsmål. Bruk vertsvedlikehold for den beskyttede verten eller velg en annen disk.</translation>
    </message>
    <message>
        <source>Unlock or select a Linux system first</source>
        <translation>Lås opp eller velg først et Linux-system</translation>
    </message>
    <message>
        <source>This encrypted drive has no visible Linux filesystem yet. Use Unlock, refresh devices, and select the target after its Linux root is detected.</source>
        <translation>Denne krypterte stasjonen har ikke noe synlig Linux-filsystem ennå. Bruk Lås opp, oppdater enhetene og velg målet etter at Linux-roten er oppdaget.</translation>
    </message>
    <message>
        <source>The selected root component (%1) belongs to the running system and cannot be committed as a repair target. Use Host Maintenance for the protected running host.</source>
        <translation>Den valgte rotkomponenten (%1) tilhører kjøresystemet og kan ikke utføres som et reparasjonsmål. Bruk vertsvedlikehold for den beskyttede verten.</translation>
    </message>
    <message>
        <source>repair target commit</source>
        <translation>Reparasjon målforpliktelse</translation>
    </message>
    <message>
        <source>Repair drive selected: %1</source>
        <translation>Reparasjon stasjon valgt: %1</translation>
    </message>
    <message>
        <source>; best detected system component: %1</source>
        <translation>; best detektert systemkomponent: %1</translation>
    </message>
    <message>
        <source>. No mount or repair action was performed.</source>
        <translation>Ingen monterings- eller reparasjonstiltak ble utført.</translation>
    </message>
    <message>
        <source>Make Default unavailable</source>
        <translation>Gjør standard utilgjengelig</translation>
    </message>
    <message>
        <source>Make Default is a running-host action on this frontend; enter Host Maintenance first.</source>
        <translation>Make Default er en handling som kjører host i denne frontenden; skriv inn vertsvedlikehold først.</translation>
    </message>
    <message>
        <source>Administrator authorization required</source>
        <translation>Administrator autorisasjon kreves</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize first.</source>
        <translation>Ingen administratorøkt er aktiv; trykk autorisering først.</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host?

The helper verifies /boot/grub/menu.lst, sets the `default &lt;N&gt;` directive to the canonical entry, backs the menu up first and restores it on any failure.</source>
        <translation>Gjør den kanonisk installerte kjerneoppføringen til standard GRUB-legacy oppstartsoppføring på den kjørende verten?

Hjelperen verifiserer /boot/grub/menu.lt, angir \&quot;standard &lt;N&gt;\&quot;-direktivet til den kanoniske oppføringen, backer menyen opp først og gjenoppretter det på enhver feil.</translation>
    </message>
    <message>
        <source>Host Maintenance unavailable</source>
        <translation>Vertsvedlikehold er ikke tilgjengelig</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a committed repair target or a detected running host.</source>
        <translation>Det løpende vertsmålet kunne ikke påvises; diagnostikk trenger et forpliktet reparasjonsmål eller en detektert kjørende vert.</translation>
    </message>
    <message>
        <source>Exit Host Maintenance</source>
        <translation>Avslutt vert vedlikehold</translation>
    </message>
    <message>
        <source>Host Maintenance is active; diagnostics and gated repairs target the protected running host.</source>
        <translation>Vertsvedlikehold er aktivt; diagnostiske og portede reparasjoner målrette den beskyttede løpende verten.</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list on the Systems tab first, or use Host Maintenance for the protected running host.</source>
        <translation>Velg en fysisk stasjon i listen over tilgjengelige reparasjonsmål i kategorien Systemer først, eller bruk vertsvedlikehold for den beskyttede verten.</translation>
    </message>
    <message>
        <source>The selected drive (%1) is the protected running host. Choose Host Maintenance on the Systems tab to run read-only host diagnostics and guarded host repairs; ordinary target repairs stay disabled.</source>
        <translation>Den valgte stasjonen (%1) er den beskyttede verten. Velg Vertsvedlikehold på Systems-fanen for å kjøre lesebeskyttede vertsdiagnostikker og bevoktede vertsreparasjoner.</translation>
    </message>
    <message>
        <source>No repair target is committed. Choose Select Target on the Systems tab (or Host Maintenance for the protected running host) first.</source>
        <translation>Ingen reparasjonsmål er forpliktet. Velg Velg mål på systemfanen (eller vertsvedlikehold for den beskyttede verten) først.</translation>
    </message>
    <message>
        <source>The selection changed after the target was committed. Choose Select Target again.</source>
        <translation>Valget endret seg etter at målet ble utført. Velg Velg mål igjen.</translation>
    </message>
    <message>
        <source>Diagnostics scope required</source>
        <translation>Diagnostikkområde som kreves</translation>
    </message>
    <message>
        <source>Diagnostics scope unresolved</source>
        <translation>Diagnostisk omfang uløst</translation>
    </message>
    <message>
        <source>The running host target could not be resolved; use Refresh Devices and commit a repair target or re-enter Host Maintenance.</source>
        <translation>Det løpende vertsmålet kunne ikke løses; bruk Oppdater enheter og foreta et reparasjonsmål eller gå inn i vertsvedlikehold.</translation>
    </message>
    <message>
        <source>Diagnostic check required</source>
        <translation>Diagnostisk kontroll kreves</translation>
    </message>
    <message>
        <source>Select a diagnostic check in the list first.</source>
        <translation>Velg først en diagnostisk sjekk i listen.</translation>
    </message>
    <message>
        <source>Repair target required</source>
        <translation>Reparasjonsmål som kreves</translation>
    </message>
    <message>
        <source>Target file editing needs a committed repair target. Running-host maintenance has no target-file editing; commit an offline target first.</source>
        <translation>Målfilredigering trenger et forpliktet reparasjonsmål. Vedlikehold av kjører-vert har ingen målfilredigering; begå et offline mål først.</translation>
    </message>
    <message>
        <source>Configuration file required</source>
        <translation>Konfigurasjonsfil kreves</translation>
    </message>
    <message>
        <source>Select a target configuration file first.</source>
        <translation>Velg først en målkonfigurasjonsfil.</translation>
    </message>
    <message>
        <source>Edit target %1</source>
        <translation>Rediger mål %1</translation>
    </message>
    <message>
        <source>Edit this target file through the guarded administrator helper. A successful save invalidates cached diagnostics; rerun diagnostics before repair. Generated files such as /boot/grub/menu.lst may be replaced by the next bootloader update.</source>
        <translation>Rediger denne målfilen gjennom den bevoktede administratorhjelperen. En vellykket spare ugyldiggjør cached diagnostics; kjøre diagnostikk før reparasjon. Genererte filer som /boot/grub/menu.lt kan erstattes av neste oppstartslasteroppdatering.</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Avbryt</translation>
    </message>
    <message>
        <source>Save Target File</source>
        <translation>Lagre målfil</translation>
    </message>
    <message>
        <source>No changes to %1.</source>
        <translation>Ingen endringer i %1.</translation>
    </message>
    <message>
        <source>Configuration write refused</source>
        <translation>Konfigurasjon avvist</translation>
    </message>
    <message>
        <source>The edited content contains NUL bytes; the guarded write refuses it.</source>
        <translation>Redigert innhold inneholder NUL bytes; den bevoktede skriver nekter det.</translation>
    </message>
    <message>
        <source>File too large</source>
        <translation>Filen er for stor</translation>
    </message>
    <message>
        <source>The edited file is larger than 1 MiB. The guarded write refuses it; edit the file from a console instead.</source>
        <translation>Redigert fil er større enn 1 MiB. Den bevoktede skriveren nekter det; rediger filen fra en konsoll i stedet.</translation>
    </message>
    <message>
        <source>Write target configuration</source>
        <translation>Skriv målkonfigurasjon</translation>
    </message>
    <message>
        <source>Write the edited contents to %1? This modifies the repair target and invalidates cached diagnostics.</source>
        <translation>Skriv redigert innhold til %1? Dette endrer reparasjonsmålet og ugyldiggjør cache-diagnostikk.</translation>
    </message>
    <message>
        <source>No diagnostic results to copy yet.</source>
        <translation>Ingen diagnostiske resultater å kopiere ennå.</translation>
    </message>
    <message>
        <source>Diagnostic results copied to the clipboard.</source>
        <translation>Diagnostiske resultater kopiert til utklippstavlen.</translation>
    </message>
    <message>
        <source>No diagnostic results to save yet.</source>
        <translation>Ingen diagnostiske resultater å spare ennå.</translation>
    </message>
    <message>
        <source>Text files (*.txt);;All files (*)</source>
        <translation>Tekstfiler (*.txt);;Alle filer (*)</translation>
    </message>
    <message>
        <source>Save diagnostic results</source>
        <translation>Lagre diagnostiske resultater</translation>
    </message>
    <message>
        <source>Could not write %1.</source>
        <translation>Kunne ikke skrive %1.</translation>
    </message>
    <message>
        <source>Diagnostic results saved to %1</source>
        <translation>Diagnostiske resultater lagret på %1</translation>
    </message>
    <message>
        <source>Unlock not available</source>
        <translation>Lås opp ikke tilgjengelig</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked. Select an offline repair target to unlock.</source>
        <translation>Den beskyttede verten kan ikke låses opp. Velg et offline reparasjonsmål for å låse opp.</translation>
    </message>
    <message>
        <source>No locked LUKS component is currently visible on this selected drive.</source>
        <translation>Ingen låst LUKS-komponent er for tiden synlig på denne valgte stasjonen.</translation>
    </message>
    <message>
        <source>Confirm LUKS unlock</source>
        <translation>Bekreft LUKS lås opp</translation>
    </message>
    <message>
        <source>Unlock %1 on %2?

The helper opens a temporary device-mapper mapping with cryptsetup and keeps it open for this recovery session. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>Lås opp %1 på %2?

Hjelperen åpner en midlertidig enhetskartlegging med kryptosett og holder den åpen for denne gjenopprettingsøkten. Passordfrasen reiser gjennom en privat nøkkelfil og plasseres aldri i kommandoargumenter eller logger.</translation>
    </message>
    <message>
        <source>LUKS unlock</source>
        <translation>LUKS låse opp</translation>
    </message>
    <message>
        <source>Unlock LUKS repair target</source>
        <translation>Lås opp LUKS-reparasjonsmål</translation>
    </message>
    <message>
        <source>Enter the passphrase for %1.

It is sent only to cryptsetup over the helper&apos;s standard input and is never logged or placed on a command line.</source>
        <translation>Skriv inn passordet for %1.

Den sendes bare til kryptsett over hjelperens standardinngang og er aldri logget eller plassert på en kommandolinje.</translation>
    </message>
    <message>
        <source>Passphrase required</source>
        <translation>Passordfrase kreves</translation>
    </message>
    <message>
        <source>An empty passphrase was not submitted. Enter the LUKS passphrase or choose Cancel.</source>
        <translation>En tom password ble ikke sendt. Skriv inn LUKS passord eller velg Avbryt.</translation>
    </message>
    <message>
        <source>Unlock keyfile unavailable</source>
        <translation>Lås opp nøkkelfil som ikke er tilgjengelig</translation>
    </message>
    <message>
        <source>The LUKS passphrase could not be written to a private keyfile in %1; the unlock was not started.</source>
        <translation>LUKS-passordet kunne ikke skrives til en privat nøkkelfil i %1; låsen ble ikke startet.</translation>
    </message>
    <message>
        <source>Configuration write unavailable</source>
        <translation>Konfigurasjon skrive utilgjengelig</translation>
    </message>
    <message>
        <source>The edited content could not be written to a private temporary file in %1; the write was not started.</source>
        <translation>Klarte ikke å skrive til en privat midlertidig fil i %1; skrivingen ble ikke startet.</translation>
    </message>
    <message>
        <source>ERROR: cannot read the session log %1</source>
        <translation>FEIL: Kan ikke lese øktloggen %1</translation>
    </message>
    <message>
        <source>Viewing a prior session log (read-only): %1</source>
        <translation>Viser en tidligere økt logg (lesbar): %1</translation>
    </message>
    <message>
        <source>Session log list refreshed.</source>
        <translation>Sesjonslogglisten er oppdatert.</translation>
    </message>
    <message>
        <source>Note:</source>
        <translation>Merk:</translation>
    </message>
    <message>
        <source>Delete session log</source>
        <translation>Slett øktlogg</translation>
    </message>
    <message>
        <source>Delete %1 permanently? This cannot be undone.</source>
        <translation>Slette %1 permanent? Dette kan ikke angres.</translation>
    </message>
    <message>
        <source>%1 changed while the confirmation was open; the delete was refused.</source>
        <translation>%1 endret seg mens bekreftelsen var åpen; sletten ble nektet.</translation>
    </message>
    <message>
        <source>Unable to delete %1.</source>
        <translation>Kan ikke slette %1.</translation>
    </message>
    <message>
        <source>Device discovery filters updated.</source>
        <translation>Oppdagingsfiltre for enhet oppdatert.</translation>
    </message>
    <message>
        <source>Unknown Linux distribution</source>
        <translation>Ukjend Linux distribusjon</translation>
    </message>
    <message>
        <source>sudo / gksu (no KAuth on this frontend)</source>
        <translation>sudo / gksu (ingen KAuth på denne frontend)</translation>
    </message>
    <message>
        <source>Available</source>
        <translation>Tilgjengelig</translation>
    </message>
    <message>
        <source>Missing on this frontend</source>
        <translation>Manglende i denne frontend</translation>
    </message>
    <message>
        <source>Missing</source>
        <translation>Manglende</translation>
    </message>
    <message>
        <source>No repair tool is selected.</source>
        <translation>Ingen reparasjonsverktøy er valgt.</translation>
    </message>
    <message>
        <source>A helper command is already running.</source>
        <translation>En hjelperkommando kjører allerede.</translation>
    </message>
    <message>
        <source>Restore Graphical Login is a host-scope stage on this legacy frontend; enter Host Maintenance to run it.</source>
        <translation>Gjenopprett Graphical Login er et host-scope-trinn på denne gamle frontend; Skriv inn vertsvedlikehold for å kjøre den.</translation>
    </message>
    <message>
        <source>The selected scope has no resolved root component; use Refresh Devices and commit the repair target again.</source>
        <translation>Det valgte omfanget har ingen løst rotkomponent; bruk Oppdater enheter og gjør reparasjonsmålet igjen.</translation>
    </message>
    <message>
        <source>This legacy frontend exposes no %1 action; the helper reports the capability as available.</source>
        <translation>Denne gamle frontend avslører ingen %1-handling; hjelperen rapporterer evnen som tilgjengelig.</translation>
    </message>
    <message>
        <source>Run this guarded repair action using the cached read-only diagnostic evidence. A confirmation is shown first.</source>
        <translation>Kjøre denne bevoktede reparasjonshandlingen ved hjelp av den cachede skrivebare diagnostiske bevis. En bekreftelse vises først.</translation>
    </message>
    <message>
        <source>Enabled in Settings</source>
        <translation>Aktivert i innstillinger</translation>
    </message>
    <message>
        <source>Disabled in Settings - enable it to include this stage</source>
        <translation>Deaktivert i innstillinger - gjør det mulig å inkludere dette trinnet</translation>
    </message>
    <message>
        <source>Unavailable: %1</source>
        <translation>Ikke tilgjengelig: %1</translation>
    </message>
    <message>
        <source>
Unavailable: %1</source>
        <translation>
Ikke tilgjengelig: %1</translation>
    </message>
    <message>
        <source>Repair tool unavailable</source>
        <translation>Reparasjonsverktøy ikke tilgjengelig</translation>
    </message>
    <message>
        <source>Confirm repair</source>
        <translation>Bekreft reparasjon</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Your saved selection is kept and its availability is re-checked when diagnostics for this scope complete.</source>
        <translation>Ingen cachede egenskaper bevis for dette trinnet ennå. Ditt lagrede utvalg holdes og dets tilgjengelighet kontrolleres på nytt når diagnostikk for dette omfanget er fullført.</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Run diagnostics for the selected scope to populate the Full Repair plan; your selection is saved once the stage becomes available.</source>
        <translation>Ingen cachede egenskaper bevis for dette trinnet ennå. Kjør diagnostikk for det valgte omfanget for å populere Full Repair-planen; valget lagres når scenen blir tilgjengelig.</translation>
    </message>
    <message>
        <source>Unknown Full Repair stage.</source>
        <translation>Ukjent fullt reparasjonstrinn.</translation>
    </message>
    <message>
        <source>No Full Repair stages are selected or available; use Configure Plan... to choose the stages.</source>
        <translation>Ingen fullstendige reparasjonsfaser er valgt eller tilgjengelig; bruk Konfigurasjonsplan for å velge trinnene.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab to establish the session.</source>
        <translation>Administrator autorisasjon er nødvendig; trykk Autorisasjon på Systemer eller Reparasjon-fanen for å etablere sesjonen.</translation>
    </message>
    <message>
        <source>Runs the selected stages using the cached read-only diagnostic evidence after privilege confirmation.</source>
        <translation>Kjører de valgte stadiene ved hjelp av den cachede lesebeskyttede diagnostiske bevis etter privilegiebekreftelse.</translation>
    </message>
    <message>
        <source>No Full Repair stages selected - use Configure Plan... or Settings.</source>
        <translation>Ingen Full Repair stadier valgt - bruk Konfigurer Plan... eller Innstillinger.</translation>
    </message>
    <message>
        <source>1 stage selected</source>
        <translation>1 trinn valgt</translation>
    </message>
    <message>
        <source>%1 stages selected</source>
        <translation>%1-faser valgt</translation>
    </message>
    <message>
        <source>No repair stages are selected. Use Configure Plan... to choose the stages Full Repair will run.</source>
        <translation>Ingen reparasjonsfaser er valgt. Bruk Konfigurer Plan... å velge trinnene Full Repair vil kjøres.</translation>
    </message>
    <message>
        <source>No selected stages are available. %1</source>
        <translation>Ingen utvalgte stadier er tilgjengelige. %1</translation>
    </message>
    <message>
        <source>Full Repair unavailable</source>
        <translation>Full Reparasjon utilgjengelig</translation>
    </message>
    <message>
        <source>Run the Full Repair plan?

The selected stages run in order through the helper&apos;s guarded repair command:

</source>
        <translation>Kjøre hele reparasjonen?

De valgte trinnene kjører i rekkefølge gjennom hjelpeens bevoktede reparasjonskommando:

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
Hjelperen holder hver kjøringstid preflight; et stadium som ikke stopper planen.</translation>
    </message>
    <message>
        <source>Running %1 through the privileged helper... The Logs tab keeps the complete transcript.</source>
        <translation>Kjøre %1 gjennom den privilegerte hjelp... Fanen Logs beholder den fullstendige transkripsjonen.</translation>
    </message>
    <message>
        <source>Shell scope required</source>
        <translation>Shell-område kreves</translation>
    </message>
    <message>
        <source>No administrator session is active.

Press Authorize on the Systems or Repair tab to establish the session, or enter Host Maintenance / commit a repair target on the Systems tab; the chroot shell then reuses the cached authorization.</source>
        <translation>Ingen administratorøkt er aktiv.

Trykk på autorisasjon på Systemer eller Reparasjon-fanen for å etablere sesjonen, eller skriv inn vertsvedlikehold / foreta et reparasjonsmål på Systems-fanen; rotskalet gjenbruker deretter den cachede autorisasjonen.</translation>
    </message>
    <message>
        <source>Shell command required</source>
        <translation>Shell-kommando kreves</translation>
    </message>
    <message>
        <source>Enter the command to run first.</source>
        <translation>Skriv inn kommandoen for å kjøre først.</translation>
    </message>
    <message>
        <source>Confirm running-host command</source>
        <translation>Bekreft kommando for kjøring-vert</translation>
    </message>
    <message>
        <source>Run this command as root on the protected running host?

%1

The helper keeps its runtime preflights; the command is passed as one argument and is never interpreted by the GUI.</source>
        <translation>Kjøre denne kommandoen som rot på den beskyttede kjøreverten?

%1

Hjelperen holder sin kjøringstid preflights; kommandoen blir passert som ett argument og blir aldri tolket av GUI.</translation>
    </message>
    <message>
        <source>Running %1...</source>
        <translation>Kjører %1...</translation>
    </message>
    <message>
        <source>Authorization scope required</source>
        <translation>Godkjenningsområde kreves</translation>
    </message>
    <message>
        <source>administrator authorization</source>
        <translation>administratorgodkjenning</translation>
    </message>
    <message>
        <source>Administrator authorization</source>
        <translation>Administrator autorisasjon</translation>
    </message>
    <message>
        <source>Administrator authorization is required for %1.

Enter the password for %2 (sudo). It is used only for this sudo authentication, is sent over a pipe and is never logged or placed on a command line. The authorization is cached for this session and reused by diagnostics and repairs.</source>
        <translation>Administrator autorisasjon er nødvendig for %1.

Skriv passordet til %2 (sudo). Den brukes kun for denne sudo-autentiseringen, sendes over et rør og logges aldri eller plasseres på en kommandolinje. Autorisasjonen er cached for denne sesjonen og gjenbrukes ved diagnostikk og reparasjon.</translation>
    </message>
    <message>
        <source>your account</source>
        <translation>din konto</translation>
    </message>
    <message>
        <source>Password required</source>
        <translation>Passord kreves</translation>
    </message>
    <message>
        <source>An empty password was not submitted. Enter the sudo password or choose Cancel.</source>
        <translation>Et tomt passord ble ikke sendt. Skriv inn sudo-passordet eller velg Avbryt.</translation>
    </message>
    <message>
        <source>Administrator authorization failed</source>
        <translation>Administrator autorisasjon mislyktes</translation>
    </message>
    <message>
        <source>sudo did not accept the password: %1

The command was not started.</source>
        <translation>sudo godtok ikke passordet: %1

Kommandoen ble ikke startet.</translation>
    </message>
    <message>
        <source>elevation requires an interactive sudo password; run the smoke as root or after `sudo -S -v` with --elevate &apos;sudo -n&apos;</source>
        <translation>høyde krever et interaktivt sudo passord; kjøre røyken som rot eller etter &apos;sudo -S -v&apos; med --elevate &apos;sudo -n&apos;</translation>
    </message>
    <message>
        <source>No administrator session is active for %1.

Press Authorize on the Systems or Repair tab to establish the session now, or enter Host Maintenance / commit a repair target on the Systems tab; diagnostics and repairs then reuse the cached authorization.</source>
        <translation>Ingen administratorøkt er aktiv for %1.

Trykk på autorisasjon på Systemer eller Reparasjon-fanen for å etablere sesjonen nå, eller skriv inn vertsvedlikehold / begår et reparasjonsmål på Systems-fanen; diagnostikk og reparasjoner gjenbruk deretter den cachede autorisasjonen.</translation>
    </message>
    <message>
        <source>Administrator authorization expired</source>
        <translation>Administrator autorisasjon utløpt</translation>
    </message>
    <message>
        <source>The cached administrator authorization for %1 expired or was refused.

Press Authorize on the Systems or Repair tab to re-establish the session, then run the command again. No command was started.</source>
        <translation>Den cachede administrator autorisasjonen for %1 utløp eller ble nektet.

Trykk på Autoriser på Systemer eller Reparer fanen for å gjenopprette sesjonen, og deretter kjøre kommandoen igjen. Ingen kommando ble startet.</translation>
    </message>
    <message>
        <source>Privileged operation completed successfully. Administrator authorization remains active for this session.</source>
        <translation>Priviligert operasjon fullført. Administrator autorisasjon forblir aktiv for denne sesjonen.</translation>
    </message>
    <message>
        <source>Privileged operation stopped with an error. Administrator authorization remains active; review the output before closing.</source>
        <translation>Priviligert operasjon stoppet med en feil. Administrator autorisasjon forblir aktiv, gjennomles utgangen før det avsluttes.</translation>
    </message>
    <message>
        <source>Inspection is read-only.

</source>
        <translation>Inspeksjon er skrivebar.

</translation>
    </message>
    <message>
        <source>Configuration unavailable</source>
        <translation>Konfigurasjon ikke tilgjengelig</translation>
    </message>
    <message>
        <source>The helper could not read %1:

%2</source>
        <translation>Hjelperen kunne ikke lese %1:

%2</translation>
    </message>
    <message>
        <source>Configuration write failed</source>
        <translation>Oppsettsskriving mislyktes</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
Result: mapping opened for this recovery session.</source>
        <translation>Tilstand: ulåst
Komponent: %1
Kartlegger: %2
Metode: hjelperlås opp (kryptsett; passord via en modus-600-nøkkelfil, slettet etter bruk)
Resultat: Kartlegging åpnet for denne gjenopprettingsøkten.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: the passphrase was not accepted; retry offered.</source>
        <translation>Tilstand: låst
Komponent: %1
Metode: Hjelperlås opp (kryptsett)
Feil: Passwordet ble ikke akseptert; prøv igjen tilbys.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: %2</source>
        <translation>Tilstand: låst
Komponent: %1
Metode: Hjelperlås opp (kryptsett)
Feil: %2</translation>
    </message>
    <message>
        <source>Passphrase not accepted</source>
        <translation>Passordfrase ikke akseptert</translation>
    </message>
    <message>
        <source>The LUKS passphrase was not accepted.

Try again?</source>
        <translation>LUKS-passordet ble ikke akseptert.

Prøve igjen?</translation>
    </message>
    <message>
        <source>[-] %1 - not run (the plan stopped before reaching this stage)</source>
        <translation>[-] %1 - ikke kjøre (planen stoppet før nå dette trinnet)</translation>
    </message>
    <message>
        <source>filesystem</source>
        <translation>Filsystem</translation>
    </message>
    <message>
        <source>[-] %1 - no file system errors found - no changes</source>
        <translation>[-] %1 - ingen filsystemfeil funnet - ingen endringer</translation>
    </message>
    <message>
        <source>[FAIL] %1 - not reported</source>
        <translation>[FAIL] %1 - ikke rapportert</translation>
    </message>
    <message>
        <source>%1 stage(s) failed; review the helper output in Logs.</source>
        <translation>%1-fasen mislyktes; se på hjelpeutgangen i logger.</translation>
    </message>
    <message>
        <source>the plan stopped before any stage completed.</source>
        <translation>Planen stoppet før et stadium fullført.</translation>
    </message>
    <message>
        <source>no repair was needed; cached diagnostics remain valid.</source>
        <translation>Ingen reparasjon var nødvendig; cached diagnostics fortsatt gyldig.</translation>
    </message>
    <message>
        <source>repair completed; cached diagnostics were invalidated and must be regenerated.</source>
        <translation>reparasjon fullført; cached diagnostics ble ugyldiggjort og må regenereres.</translation>
    </message>
    <message>
        <source>%1 results: [OK] %2 successful | [FAIL] %3 failed | [-] %4 no repair needed - %5</source>
        <translation>%1-resultater: [OK] %2 suksessfull | [FAIL] %3 mislykkes | [-] %4 ingen reparasjon nødvendig - %5</translation>
    </message>
    <message>
        <source>Re-run Diagnostic</source>
        <translation>Re-kjør diagnostisk</translation>
    </message>
    <message>
        <source>Unavailable</source>
        <translation>Ikke tilgjengelig</translation>
    </message>
    <message>
        <source>Commit this physical drive as the repair target.</source>
        <translation>Legg denne fysiske stasjonen som reparasjonsmålet.</translation>
    </message>
    <message>
        <source>Protection:</source>
        <translation>Beskyttelse:</translation>
    </message>
    <message>
        <source>Host shell</source>
        <translation>Vertsskal</translation>
    </message>
    <message>
        <source>Run on Host</source>
        <translation>Kjør på vert</translation>
    </message>
    <message>
        <source>Host Shell</source>
        <translation>Vært Shell</translation>
    </message>
    <message>
        <source>The read-only probe found no editable target configuration file in this target. %1</source>
        <translation>Den skrivebare sonden fant ingen redigerbar målkonfigurasjonsfil i dette målet. %1</translation>
    </message>
    <message>
        <source>Not present in the selected target (omitted): %1</source>
        <translation>Ikke tilstede i det valgte målet (mottatt): %1</translation>
    </message>
    <message>
        <source>Run diagnostics to probe which target configuration files exist; the helper&apos;s read-only probe decides the list.</source>
        <translation>Kjør diagnostikk til å sonde hvilke mål konfigurasjonsfiler som eksisterer; hjelperens skrivebare probe bestemmer listen.</translation>
    </message>
    <message>
        <source>Probed read-only by the helper; a saved edit invalidates the cached diagnostics.</source>
        <translation>Forbedret skrivebeskyttet av hjelperen; en lagret redigering ugyldiggjør den cachede diagnostikken.</translation>
    </message>
    <message>
        <source>Host maintenance: %1</source>
        <translation>Vertsvedlikehold: %1</translation>
    </message>
    <message>
        <source>unresolved</source>
        <translation>Uoppklart</translation>
    </message>
    <message>
        <source>Target: %1 + %2</source>
        <translation>Mål: %1 + %2</translation>
    </message>
    <message>
        <source>Host command</source>
        <translation>Vertskommando</translation>
    </message>
    <message>
        <source>Execute a command on the running host as root.</source>
        <translation>Kjør en kommando på den kjørernde verten som rot.</translation>
    </message>
    <message>
        <source>Execute a command inside the selected repair system as root.</source>
        <translation>Kjøre en kommando i det valgte reparasjonssystemet som rot.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab (or re-enter Host Maintenance / re-commit the repair target) to authorize this session.</source>
        <translation>Administrator autorisasjon er nødvendig; trykk Autorisasjon på Systemer eller Repair-fanen (eller skriv inn vertsvedlikehold / re-kommitere reparasjonsmålet) for å autorisere denne sesjonen.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root on the running host through the helper&apos;s guarded host-shell verb.</source>
        <translation>Kjøre en revurdert kommando som rot på den løpende verten gjennom hjelperens bevoktede vertsskal verb.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root inside the target chroot through the helper&apos;s guarded shell verb.</source>
        <translation>Kjør én omdøpt kommando som rot inne i målet croot gjennom hjelperens bevoktede skall verb.</translation>
    </message>
    <message>
        <source>Run a command on the running host as root (sudo is not needed). Commands are executed directly on the active system; output is kept in this window and in the application log.</source>
        <translation>Kjøre en kommando på den løpende verten som root (sudo er ikke nødvendig). Kommandoer kjøres direkte på det aktive systemet. Utgangen holdes i dette vinduet og i programloggen.</translation>
    </message>
    <message>
        <source>Run a command inside the selected repair system as root (sudo is not needed). Commands are executed one at a time in a fresh chroot and cannot answer interactive prompts; use non-interactive flags such as apt-get -y upgrade. Output is kept in this window and in the application log.</source>
        <translation>Kjøre en kommando inne i det valgte reparasjonssystemet som root (sudo er ikke nødvendig). Kommandoer utføres én om gangen i en frisk krone og kan ikke svare på interaktive spørsmål; bruk ikke-interaktive flagg som apt-get -y oppgradering. Utgang holdes i dette vinduet og i programloggen.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize or re-enter Host Maintenance / commit a repair target.</source>
        <translation>Ingen administratorøkt er aktiv; trykk Autorisering eller gjeninnfør vertsvedlikehold / forplikte et reparasjonsmål.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from the repaired system</source>
        <translation>1. Velg kildefiler eller mapper fra det reparerte systemet</translation>
    </message>
    <message>
        <source>2. Choose destination on this host</source>
        <translation>2. Velg reisemål på denne verten</translation>
    </message>
    <message>
        <source>Add File Path...</source>
        <translation>Legg til filsti...</translation>
    </message>
    <message>
        <source>Add Folder Path...</source>
        <translation>Legg til mappesti...</translation>
    </message>
    <message>
        <source>Browse...</source>
        <translation>Bla gjennom...</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it (Host Maintenance does not provide a repair tree to browse).</source>
        <translation>Velg reparasjonsstasjonen i Systems før du velger et reisemål inne i det (Vertsvedlikehold gir ikke et reparasjonstre å bla gjennom).</translation>
    </message>
    <message>
        <source>Choose a host destination folder directly.</source>
        <translation>Velg en vertsdestinasjonsmappe direkte.</translation>
    </message>
    <message>
        <source>Copy the staged files and folders with cp -a, restore ownership with chown --reference and byte-compare every regular file afterwards.</source>
        <translation>Kopier de iscenesatte filene og mappene med cp -a, gjenopprett eierskap med chown --referanse og byte-komparere hver vanlige fil etterpå.</translation>
    </message>
    <message>
        <source>File Copy unavailable</source>
        <translation>Fil Kopier utilgjengelig</translation>
    </message>
    <message>
        <source>Choose host destination folder</source>
        <translation>Velg vertsmålmappe</translation>
    </message>
    <message>
        <source>Select a repair target</source>
        <translation>Velg et reparasjonsmål</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it.</source>
        <translation>Velg reparasjonsstasjonen i Systems før du velger et destinasjonssted inne i det.</translation>
    </message>
    <message>
        <source>File Copy - Browse Target Folders</source>
        <translation>Filkopi - Bla gjennom målmapper</translation>
    </message>
    <message>
        <source>Browse Target Folders</source>
        <translation>Bla gjennom målmapper</translation>
    </message>
    <message>
        <source>The helper could not list the repair-system folder:

%1</source>
        <translation>Hjelperen kunne ikke liste opp reparasjonssystemmappen:

%1</translation>
    </message>
    <message>
        <source>BROWSE_ENTRY</source>
        <translation>BROWSE ENTRY</translation>
    </message>
    <message>
        <source>(choose this folder: %1)</source>
        <translation>(Velg denne mappen: %1)</translation>
    </message>
    <message>
        <source>(parent folder)</source>
        <translation>(foreldermappe)</translation>
    </message>
    <message>
        <source>Select repair-system destination</source>
        <translation>Velg reparasjonssystemdestinasjon</translation>
    </message>
    <message>
        <source>Choose a destination folder inside the repaired system (current folder: %1):</source>
        <translation>Velg en målmappe inne i det reparerte systemet (nåværende mappe: %1):</translation>
    </message>
    <message>
        <source>Folder</source>
        <translation>Mappe</translation>
    </message>
    <message>
        <source>Open</source>
        <translation>Åpne</translation>
    </message>
    <message>
        <source>(choose this folder:</source>
        <translation>(Velg denne mappen:</translation>
    </message>
    <message>
        <source>Add files to copy</source>
        <translation>Legg til filer å kopiere</translation>
    </message>
    <message>
        <source>Add folder to copy</source>
        <translation>Legg til mappe å kopiere</translation>
    </message>
    <message>
        <source>Stage at least one source and name a destination path first.</source>
        <translation>Trinn minst én kilde og navngi en målvei først.</translation>
    </message>
    <message>
        <source>Copy the %1 staged item(s) to %2?

The helper keeps its direction and path containment checks; a sensitive repair-system destination is refused unless the helper approves it, and every regular file is byte-compared after the copy.</source>
        <translation>Kopier %1 iscenesatt element(er) til %2?

Hjelperen beholder retnings- og baneinnbeholdskontrollene; et sensitivt reparasjonssystem blir nektet med mindre hjelperen godkjenner den, og hver vanlig fil er byte-sammenliknet etter kopien.</translation>
    </message>
    <message>
        <source>File Copy - Copy and Verify</source>
        <translation>Filkopi - Kopier og bekreft</translation>
    </message>
    <message>
        <source>File Copy - Preview Changes</source>
        <translation>Filkopi - Forhåndsvisningsendringer</translation>
    </message>
    <message>
        <source>Protected running host - details</source>
        <translation>Beskyttet løpende vert - detaljer</translation>
    </message>
    <message>
        <source>not mounted (offline target)</source>
        <translation>ikke montert (offline mål)</translation>
    </message>
    <message>
        <source>Select a drive to see its details.</source>
        <translation>Velg en enhet for å se detaljene.</translation>
    </message>
    <message>
        <source>Inspecting the selected component.</source>
        <translation>Inspeksjon av den valgte komponenten.</translation>
    </message>
    <message>
        <source>Helper-confirmed by the last read-only diagnostics.</source>
        <translation>Hjelpebekreftet av den siste lesebeskyttede diagnosen.</translation>
    </message>
    <message>
        <source>Read-only inventory only; run diagnostics to confirm.</source>
        <translation>Bare lesebeskyttet lager; kjører diagnostikk å bekrefte.</translation>
    </message>
    <message>
        <source>PROTECTED - running system; read-only details only</source>
        <translation>PROTECTED - kjøresystem; kun skrivebare detaljer</translation>
    </message>
    <message>
        <source>Live / installer media - not selectable</source>
        <translation>Live / installer medier - ikke valgbar</translation>
    </message>
    <message>
        <source>PROTECTED - running host scope</source>
        <translation>PROTECTED - kjørende vertsområde</translation>
    </message>
    <message>
        <source>Unlock required before selection</source>
        <translation>Lås opp nødvendig før valg</translation>
    </message>
    <message>
        <source>Eligible repair candidate</source>
        <translation>kvalifiserte reparasjonskandidater</translation>
    </message>
    <message>
        <source>Drive:</source>
        <translation>Kjør:</translation>
    </message>
    <message>
        <source>Detected target:</source>
        <translation>Oppdaget mål:</translation>
    </message>
    <message>
        <source>Pending inspection</source>
        <translation>Avventende inspeksjon</translation>
    </message>
    <message>
        <source>Model / label:</source>
        <translation>Modell / etikett:</translation>
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
        <translation>Tilkobling:</translation>
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
        <translation>Monter:</translation>
    </message>
    <message>
        <source>Running system protection unresolved</source>
        <translation>Beskyttelse av driftssystemet uløst</translation>
    </message>
    <message>
        <source>No protected physical backing disk was identified</source>
        <translation>Ingen beskyttet fysisk støttedisk ble identifisert</translation>
    </message>
    <message>
        <source>Current running Linux system</source>
        <translation>Nåværende Linux-system</translation>
    </message>
    <message>
        <source>Critical mounts: %1</source>
        <translation>Kritiske monteringer: %1</translation>
    </message>
    <message>
        <source>Read-only protected-running-system facts: the helper OS fact plus the inventory model, device path, size, transport and critical mounts. Nothing here is probed destructively.</source>
        <translation>Lesbare beskyttede-kjøring-system fakta: hjelper OS fakta pluss lagermodell, enhetssti, størrelse, transport og kritiske monteringer. Ingenting her er probede destruktivt.</translation>
    </message>
    <message>
        <source>Select a drive to see unlock status.</source>
        <translation>Velg en enhet for å se opplåsingsstatus.</translation>
    </message>
    <message>
        <source>State: protected
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
The protected running host cannot be unlocked or modified; unlock is available for an offline repair target only. Use Host Maintenance for the protected running host.</source>
        <translation>Stat: beskyttet
Komponent: %1
Kartlegger: (ingen)
Metode: hjelperlås opp (kryptsett; passord via en modus-600-nøkkelfil, slettet etter bruk)
Den beskyttede verten kan ikke låses opp eller endres; Lås opp er kun tilgjengelig for et offline-reparasjonsmål. Bruk vertsvedlikehold for den beskyttede verten.</translation>
    </message>
    <message>
        <source>(none detected)</source>
        <translation>(ingen oppdaget)</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
A locked LUKS container is visible on this drive; press Unlock to open it for this recovery session.</source>
        <translation>Tilstand: låst
Komponent: %1
Kartlegger: (ingen)
Metode: hjelperlås opp (kryptsett; passord via en modus-600-nøkkelfil, slettet etter bruk)
En låst LUKS-beholder er synlig på denne enheten; trykk Lås opp for å åpne den for denne gjenopprettingsøkten.</translation>
    </message>
    <message>
        <source>(visible mapper)</source>
        <translation>(synlig kartlegger)</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: already open before this session (visible mapper; Boot Bitch will reuse it and will not close it).</source>
        <translation>Tilstand: ulåst
Komponent: %1
Kartlegger: %2
Metode: allerede åpen før denne sesjonen (synlig kartlegger; Boot Bitch vil gjenbruke den og vil ikke lukke den).</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper-confirmed mapping from the last read-only diagnostics.</source>
        <translation>Tilstand: ulåst
Komponent: %1
Kartlegger: %2
Metode: hjelpebekreftet kartlegging fra den siste lesebeskyttede diagnosen.</translation>
    </message>
    <message>
        <source>(mapper)</source>
        <translation>(kartlegger)</translation>
    </message>
    <message>
        <source>State: locked or no encrypted component detected
Component: (none detected)
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
No locked LUKS component and no unlock operation were recorded for this drive in the current session.</source>
        <translation>Tilstand: låst eller ingen kryptert komponent oppdaget
Komponent: (ingen oppdaget)
Kartlegger: (ingen)
Metode: hjelperlås opp (kryptsett; passord via en modus-600-nøkkelfil, slettet etter bruk)
Ingen låst LUKS komponent og ingen opplåsingsoperasjon ble registrert for denne enheten i den aktuelle sesjonen.</translation>
    </message>
    <message>
        <source>unavailable - %1</source>
        <translation>utilgjengelig - %1</translation>
    </message>
    <message>
        <source>Host maintenance:
%1</source>
        <translation>Vertsvedlikehold:
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
        <translation>Autorisasjon kreves: diagnostisering og reparasjon mislykkes til du trykker på autorisering.</translation>
    </message>
    <message>
        <source>Re-establish the privileged helper session for the current scope now. The password is requested in the hidden-input modal and is never logged.</source>
        <translation>Gjenopprett den privilegerte hjelpeøkten for det nåværende omfanget nå. Passordet er bedt om i den skjulte input-modulen og er aldri logget.</translation>
    </message>
    <message>
        <source>Running...</source>
        <translation>Kjører...</translation>
    </message>
    <message>
        <source>Scope required</source>
        <translation>Omfang som kreves</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope; this unlocks the gated actions.</source>
        <translation>Kjør alle - kjøre alle tilgjengelige lesebare diagnostikk for det aktuelle omfanget; dette låser opp de portede handlingene.</translation>
    </message>
    <message>
        <source>Select a target and wait for any running command first.</source>
        <translation>Velg et mål og vent på alle kommandoer som kjører først.</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic through the helper.</source>
        <translation>Kjør Diagnostisk - kjøre den valgte skrivebeskyttede diagnosen gjennom hjelperen.</translation>
    </message>
    <message>
        <source>Run All for the current scope and refresh the read-only backend profile.</source>
        <translation>Kjør alt for gjeldende omfang og oppdater profilen for skrivebeskyttet backend.</translation>
    </message>
    <message>
        <source>Read or edit the selected target configuration file through the guarded helper; a saved edit invalidates cached diagnostics.</source>
        <translation>Les eller rediger den valgte målkonfigurasjonsfilen gjennom den bevoktede hjelpen; en lagret redigering ugyldiggjør cachediagnostikk.</translation>
    </message>
    <message>
        <source>Host Maintenance has no target-file editing; commit an offline repair target first.</source>
        <translation>Vertsvedlikehold har ingen målfilredigering; foreta en offline reparasjon mål først.</translation>
    </message>
    <message>
        <source>Commit an offline repair target first.</source>
        <translation>Legg til et offline-reparasjonsmål først.</translation>
    </message>
    <message>
        <source>No target configuration file is available for this target; run diagnostics to probe the list.</source>
        <translation>Ingen målkonfigurasjonsfil er tilgjengelig for dette målet; kjører diagnostikk for å sonde listen.</translation>
    </message>
    <message>
        <source>Already Unlocked</source>
        <translation>Allerede låst</translation>
    </message>
    <message>
        <source>An unlocked Linux filesystem is already visible on this drive. Boot Bitch will reuse the existing mapper and will not close or reopen a mapping created by this recovery session. Mounting happens during diagnostics (read-only) and repairs (read-write); data filesystems are never auto-mounted on selection.</source>
        <translation>Et opplåst Linux-filsystem er allerede synlig på denne stasjonen. Boot Bitch vil gjenbruke eksisterende kartlegger og vil ikke lukke eller åpne en kartlegging opprettet av denne gjenopprettingsøkten. Montering skjer under diagnostikk (lesebeskyttet) og reparasjoner (leseskrive); datafilsystemene blir aldri automatisk montert på utvalget.</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked; use Host Maintenance for the protected running host.</source>
        <translation>Den beskyttede verten kan ikke låses opp; bruk vertsvedlikehold for den beskyttede verten.</translation>
    </message>
    <message>
        <source>Host Maintenance is the current scope, but the selected offline drive can still be unlocked.</source>
        <translation>Vertsvedlikehold er det aktuelle omfanget, men den valgte offline-stasjonen kan fortsatt låses opp.</translation>
    </message>
    <message>
        <source>Unlock %1 using cryptsetup through the privileged helper. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>Lås opp %1 ved hjelp av cryptsetup gjennom den privilegerte hjelperen. Passordfrasen reiser gjennom en privat nøkkelfil og plasseres aldri i kommandoargumenter eller logger.</translation>
    </message>
    <message>
        <source>The protected running system cannot be selected as a repair target; use Host Maintenance for the protected running host.</source>
        <translation>Det beskyttede kjøresystemet kan ikke velges som et reparasjonsmål; bruk vertsvedlikehold for den beskyttede verten.</translation>
    </message>
    <message>
        <source>Live / installer media is read-only boot media and cannot be selected as a repair target.</source>
        <translation>Live / installatør media er lesebeskyttede oppstartsmedier og kan ikke velges som et reparasjonsmål.</translation>
    </message>
    <message>
        <source>Unlock the encrypted volume first; Select Target becomes available after a Linux filesystem is detected.</source>
        <translation>Lås opp det krypterte volumet først; Velg Mål blir tilgjengelig etter at et Linux-filsystem er oppdaget.</translation>
    </message>
    <message>
        <source>Committed repair target. Repair, Diagnostics and File Copy target this physical drive until another drive is explicitly selected with Select Target.</source>
        <translation>Betalte reparasjonsmål. Reparasjon, diagnostikk og filkopi mål denne fysiske enheten til en annen enhet er eksplisitt valgt med Velg mål.</translation>
    </message>
    <message>
        <source>Commit %1 as the repair target; this leaves Host Maintenance and switches the scope to the selected drive.</source>
        <translation>Legg inn %1 som reparasjonsmål; dette forlater vertsvedlikehold og bytter omfang til den valgte stasjonen.</translation>
    </message>
    <message>
        <source>The running host target could not be detected.</source>
        <translation>Den løpende vertsmålet kunne ikke oppdages.</translation>
    </message>
    <message>
        <source>Leave Host Maintenance and return to repair-target mode.</source>
        <translation>La vertsvedlikehold og gå tilbake til reparasjonsmålmodus.</translation>
    </message>
    <message>
        <source>Select the running host for deliberate guarded maintenance; the administrator authorization is requested here once and cached for the session.</source>
        <translation>Velg den løpende verten for bevisst bevoktet vedlikehold; administratorens autorisasjon blir bedt her en gang og cached for sesjonen.</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a repair target.</source>
        <translation>Det løpende vertsmålet kan ikke påvises; diagnostikk trenger et reparasjonsmål.</translation>
    </message>
    <message>
        <source>Committed target: %1 + %2</source>
        <translation>Belagt mål: %1 + %2</translation>
    </message>
    <message>
        <source>Committed target: none (selection changed)</source>
        <translation>Interessert mål: ingen (valg endret)</translation>
    </message>
    <message>
        <source>The selected scope has no resolved Linux root component; Refresh Devices and commit the repair target again.</source>
        <translation>Det valgte omfanget har ingen løst Linux rotkomponent; Oppfrisk enheter og gjør reparasjonsmålet igjen.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize on the Systems or Repair tab to re-establish it. Run All reuses the cached authorization and never prompts by itself.</source>
        <translation>Ingen administratorøkt er aktiv; trykk Autorisering på Systemer eller Reparasjon-fanen for å gjenopprette den. Kjør alle gjenbruker den cachede autorisasjonen og aldri spør av seg selv.</translation>
    </message>
    <message>
        <source>Run diagnostics for this scope to unlock the gated actions. Diagnostics are read-only and the only evidence source.</source>
        <translation>Kjør diagnostikk for dette omfanget for å låse opp de portede handlingene. Diagnostikk er bare leselig og den eneste beviskilden.</translation>
    </message>
    <message>
        <source>A repair was not proven unchanged, so the cached diagnostics are invalidated. Run diagnostics again before another gated action.</source>
        <translation>En reparasjon ble ikke bevist uendret, så de cachede diagnostikkene er ugyldiggjort. Kjør diagnose igjen før en annen gated handling.</translation>
    </message>
    <message>
        <source>Gated actions reflect the cached capability lines; the helper still runs every runtime preflight when a command starts.</source>
        <translation>Gated handlinger reflekterer de cachede funksjonslinjene; hjelperen kjører fortsatt hver kjøretid preflight når en kommando starter.</translation>
    </message>
    <message>
        <source>Regenerating diagnostics automatically</source>
        <translation>Regenererende diagnostikk automatisk</translation>
    </message>
    <message>
        <source>Running diagnostic: %1</source>
        <translation>Kjørediagnostikk: %1</translation>
    </message>
    <message>
        <source>Running all diagnostics</source>
        <translation>Kjøre alle diagnoser</translation>
    </message>
    <message>
        <source>Unlocking %1</source>
        <translation>Låser opp %1</translation>
    </message>
    <message>
        <source>running host</source>
        <translation>kjører vert</translation>
    </message>
    <message>
        <source>offline target</source>
        <translation>offline mål</translation>
    </message>
    <message>
        <source>host maintenance active</source>
        <translation>vertsvedlikehold aktiv</translation>
    </message>
    <message>
        <source>repair target committed</source>
        <translation>Reparasjonsmål utført</translation>
    </message>
    <message>
        <source>no committed scope</source>
        <translation>Ingen forpliktet omfang</translation>
    </message>
    <message>
        <source>Current session</source>
        <translation>Gjeldende økt</translation>
    </message>
    <message>
        <source>Log files (*.log);;All files (*)</source>
        <translation>Loggfiler (*.log);;Alle filer (*)</translation>
    </message>
    <message>
        <source>Save log as</source>
        <translation>Lagre logg som</translation>
    </message>
    <message>
        <source>About Boot Bitch</source>
        <translation>Om Boot Bitch</translation>
    </message>
    <message>
        <source>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;&lt;p&gt;&lt;b&gt;Developer:&lt;/b&gt; CaptainMorgan12&lt;/p&gt;&lt;p&gt;This GUI is the package&apos;s entry point: a Qt 3.3.x frontend with the ported guarded helper for Debian Etch-era systems.&lt;/p&gt;&lt;p&gt;&lt;b&gt;Guarded repair mode:&lt;/b&gt; read-only diagnostics can inspect either the protected Running Host or an explicitly selected repair drive. The guarded Debian/APT package stages (interrupted configuration, broken dependencies, metadata refresh, upgrade), GRUB-legacy configuration regeneration, LUKS target unlock and guarded target-file editing run through the ported helper after confirmation; Host Maintenance enables the same supported stages natively on the active system after repeating the host identity and boot-mount checks.&lt;/p&gt;&lt;p&gt;The modern-only features - verified File Copy, Btrfs snapshot rollback, EFI/UKI and extlinux repair, boot-stack reconciliation and Make Default - are greyed with the helper&apos;s own probe reasons on this frontend; Arch/Alpine/Fedora package backends stay diagnostics-only here.&lt;/p&gt;&lt;p&gt;The first privileged action authorizes one cached administrator session per scope through a hidden-input modal (the Qt GUI remains unprivileged). It can be ended at any time from File - Lock Administrator Session.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;&lt;p&gt;&lt;b&gt;Developer:&lt;/b&gt; CaptainMorgan12&lt;/p&gt;&lt;p&gt; Denne GUI er pakkens inngangspunkt: en Qt 3.3.x frontend med den porterte bevoktede hjelperen for Debian Etch-era-systemer.&lt;/p&gt;&lt;p&gt;&lt;b&gt;Guarded reparasjonsmodus:&lt;/b&gt; lesebeskyttede diagnostikk kan inspisere enten den beskyttet Kjøreverten eller en eksplisitt valgt reparasjonsstasjon. De bevoktede Debian/APT-pakkestadiene (avbrutt konfigurasjon, brutt avhengighet, metadataoppdatering, oppgradering), GRUB-legacy-konfigurasjon regenerering, LUKS-målopplåsing og bevoktet målfilredigering kjører gjennom den porterte hjelperen etter bekreftelse; Vertsvedlikehold gjør det mulig for de samme støttede stadiene på det aktive systemet etter å ha gjentatt vertsidentitets- og oppstartsmonteringskontrollene.&lt;/p&gt; De moderne funksjonene - bekreftet File Copy, Btrfs øyeblikksbilderollback, EFI/UKI og extlinux reparasjon, boot-stack forsoning og Make Default - er grått med hjelperens egne probe grunner på denne frontend; Arch/Alpine/Fedora pakkebackends forblir diagnostics-bare her. &lt;/p&gt;&lt;p&gt; Den første privilegerte handlingen autoriserer én cached administratorøkt per omfang gjennom en skjult-input modal (Qt GUI forblir upriviligert). Den kan avsluttes når som helst fra File - Lock Administrator Session.&lt;/p&gt;</translation>
    </message>
</context>
</TS>
