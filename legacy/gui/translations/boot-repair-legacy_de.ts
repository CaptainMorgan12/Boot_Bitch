<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS language="de">
<context>
    <name>LegacyMainWindow</name>
    <message>
        <source>Validate environment</source>
        <translation>Umgebung validieren</translation>
    </message>
    <message>
        <source>Validate</source>
        <translation>Validieren</translation>
    </message>
    <message>
        <source>Check the selected system&apos;s mounts, filesystem metadata, boot files, mapper consistency and dependency readiness before any repair action. This is an independent safety preflight rather than an optional Full Repair stage.</source>
        <translation>Überprüfen Sie die Mounts des ausgewählten Systems, die Metadaten des Dateisystems, die Boot-Dateien, die Konsistenz des Mappers und die Abhängigkeitsbereitschaft vor Reparaturmaßnahmen. Dies ist ein unabhängiger Sicherheitsvorflug und keine optionale vollständige Reparaturphase.</translation>
    </message>
    <message>
        <source>Always preflight</source>
        <translation>Immer Preflight</translation>
    </message>
    <message>
        <source>File system repair</source>
        <translation>Dateisystemreparatur</translation>
    </message>
    <message>
        <source>Check File Systems</source>
        <translation>Dateisysteme prüfen</translation>
    </message>
    <message>
        <source>Run the read-only file system check for the selected system&apos;s root and /boot filesystems and report each device&apos;s check tool and result without changing anything. This legacy frontend exposes the read-only check only; device repair is not wired.</source>
        <translation>Führen Sie die schreibgeschützte Dateisystemprüfung für die Root- und /Boot-Dateisysteme des ausgewählten Systems aus und melden Sie das Prüfwerkzeug und Ergebnis jedes Geräts, ohne etwas zu ändern. Dieses Legacy-Frontend zeigt nur die Read-Only-Prüfung; die Gerätereparatur ist nicht verdrahtet.</translation>
    </message>
    <message>
        <source>Full Repair plan: unavailable on this frontend - read-only check only</source>
        <translation>Vollständiger Reparaturplan: auf diesem Frontend nicht verfügbar - nur Lese-Check</translation>
    </message>
    <message>
        <source>Complete package configuration</source>
        <translation>Paketkonfiguration abschließen</translation>
    </message>
    <message>
        <source>Complete Configuration</source>
        <translation>Konfiguration abschließen</translation>
    </message>
    <message>
        <source>Complete interrupted dpkg package configuration in the selected repair system. This is the same stage controlled by Settings -&gt; Full Repair plan -&gt; Complete interrupted package configuration, but it can also be run independently here.</source>
        <translation>Komplette unterbrochene dpkg-Paketkonfiguration im ausgewählten Reparatursystem. Dies ist die gleiche Phase, die von den Einstellungen gesteuert wird -&gt; Vollständiger Reparaturplan -&gt; Komplette unterbrochene Paketkonfiguration, kann aber auch hier eigenständig ausgeführt werden.</translation>
    </message>
    <message>
        <source>Run the guarded dpkg-configure repair? The helper keeps its package-lock and runtime preflights.</source>
        <translation>Führen Sie die bewachte dpkg-configure Reparatur aus? Der Helfer behält seine Paketsperre und Laufzeitvorflüge.</translation>
    </message>
    <message>
        <source>Repair broken dependencies</source>
        <translation>Defekte Abhängigkeiten reparieren</translation>
    </message>
    <message>
        <source>Repair Dependencies</source>
        <translation>Abhängigkeiten reparieren</translation>
    </message>
    <message>
        <source>Repair package dependencies in the selected repair system after the mandatory safety preflight. This maps directly to Settings -&gt; Repair broken package dependencies.</source>
        <translation>Reparaturpaketabhängigkeiten im ausgewählten Reparatursystem nach dem obligatorischen Sicherheitsvorflug. Diese Karten direkt zu Einstellungen -&gt; Reparieren Sie defekte Paketabhängigkeiten.</translation>
    </message>
    <message>
        <source>Run the guarded fix-broken repair? The helper keeps its simulation-first preflight and runtime guards.</source>
        <translation>Führen Sie die bewachte Fix-gebrochene Reparatur durch? Der Helfer behält seine simulationsersten Vorflug- und Laufzeitwächter.</translation>
    </message>
    <message>
        <source>Refresh package metadata</source>
        <translation>Paketmetadaten aktualisieren</translation>
    </message>
    <message>
        <source>Refresh Metadata</source>
        <translation>Metadaten aktualisieren</translation>
    </message>
    <message>
        <source>Refresh APT metadata in the selected repair system without upgrading installed packages. This maps directly to Settings -&gt; Refresh package metadata.</source>
        <translation>Aktualisieren Sie APT-Metadaten im ausgewählten Reparatursystem, ohne die installierten Pakete zu aktualisieren. Diese Karten direkt zu Einstellungen -&gt; Paket-Metadaten aktualisieren.</translation>
    </message>
    <message>
        <source>Refresh package metadata for the selected scope? The helper requires a reachable, trusted APT source.</source>
        <translation>Paket-Metadaten für den ausgewählten Umfang aktualisieren? Der Helfer benötigt eine erreichbare, vertrauenswürdige APT-Quelle.</translation>
    </message>
    <message>
        <source>Upgrade installed packages</source>
        <translation>Installierte Pakete aktualisieren</translation>
    </message>
    <message>
        <source>Simulate and Upgrade</source>
        <translation>Simulieren und aktualisieren</translation>
    </message>
    <message>
        <source>Simulate the APT transaction first, inspect proposed removals, then apply a safe upgrade. This maps directly to Settings -&gt; Upgrade installed packages.</source>
        <translation>Simulieren Sie zuerst die APT-Transaktion, prüfen Sie die vorgeschlagenen Entfernungen und wenden Sie dann ein sicheres Upgrade an. Diese Karten direkt zu Einstellungen -&gt; Installierte Pakete aktualisieren.</translation>
    </message>
    <message>
        <source>Run the guarded apt-upgrade transaction? The helper keeps its simulation-first and source guards.</source>
        <translation>Führen Sie die bewachte apt-upgrade-Transaktion aus? Der Helfer behält seine Simulation-First und Source Guards.</translation>
    </message>
    <message>
        <source>DKMS</source>
        <translation>DKMS</translation>
    </message>
    <message>
        <source>Rebuild DKMS</source>
        <translation>DKMS neu aufbauen</translation>
    </message>
    <message>
        <source>Rebuild out-of-tree kernel modules for kernels installed in the selected system. The helper refuses this action when DKMS is not installed; this legacy frontend exposes no DKMS action.</source>
        <translation>Erstellen Sie Out-of-Tree-Kernelmodule für Kernel, die im ausgewählten System installiert sind. Der Helfer lehnt diese Aktion ab, wenn DKMS nicht installiert ist; dieses Legacy-Frontend zeigt keine DKMS-Aktion.</translation>
    </message>
    <message>
        <source>Graphical login / display manager</source>
        <translation>Grafischer Login / Display-Manager</translation>
    </message>
    <message>
        <source>Restore Graphical Login</source>
        <translation>Grafischen Login wiederherstellen</translation>
    </message>
    <message>
        <source>Restore the legacy SysV display manager configured for the running host: the /etc/X11/default-display-manager entry and the missing runlevel S-symlink, with a backup and rollback, never starting the GUI. This is a host-scope stage on this legacy frontend.</source>
        <translation>Stellen Sie den für den laufenden Host konfigurierten SysV-Anzeigemanager wiederhergestellt: den Eintrag /etc/X11/default-display-manager und den fehlenden Runlevel-Symlink mit Backup und Rollback, ohne die GUI zu starten. Dies ist eine Host-Scope-Bühne auf diesem Legacy-Frontend.</translation>
    </message>
    <message>
        <source>Restore the graphical login configuration for the running host? The helper backs up /etc/X11/default-display-manager and the runlevel symlink state, restores the configured entry and the missing S-symlink, rolls back on any failure, and never starts the display manager.</source>
        <translation>Wiederherstellen der grafischen Anmeldekonfiguration für den laufenden Host? Der Helfer sichert /etc/X11/default-display-manager und den runlevel-Symlink-Zustand, stellt den konfigurierten Eintrag und den fehlenden S-Symlink wieder her, rollt bei einem Fehler zurück und startet den Display-Manager niemals.</translation>
    </message>
    <message>
        <source>Initramfs</source>
        <translation>Initramfs</translation>
    </message>
    <message>
        <source>Rebuild Initramfs</source>
        <translation>initramfs neu aufbauen</translation>
    </message>
    <message>
        <source>Rebuild initramfs images for the selected repair system only after mapper and crypttab consistency checks pass. The helper backs up each image before the apply. On Etch the stage runs through the guarded plain-chroot fallback (no unshare required).</source>
        <translation>Bauen Sie initramfs-Bilder für das ausgewählte Reparatursystem erst nach Passieren von Mapping- und crypttab-Konsistenzprüfungen wieder auf. Der Helfer sichert jedes Bild vor der Anwendung. Auf Etch läuft die Bühne durch den bewachten Plain-Chroot-Fallback (keine Unshare erforderlich).</translation>
    </message>
    <message>
        <source>Rebuild the initramfs for the selected scope? The helper keeps its mapper/crypttab and backup preflights.</source>
        <translation>Bauen Sie das initramfs für den ausgewählten Bereich neu auf? Der Helfer behält seine Mapper/Crypttab- und Backup-Preflights.</translation>
    </message>
    <message>
        <source>EFI / UKI bootloader</source>
        <translation>EFI-/UKI-Bootloader</translation>
    </message>
    <message>
        <source>Repair EFI / UKI</source>
        <translation>EFI / UKI reparieren</translation>
    </message>
    <message>
        <source>Repair the selected system&apos;s EFI / UKI boot path. This legacy frontend exposes no EFI action; the Etch target is a BIOS/GRUB-legacy system.</source>
        <translation>Reparieren Sie den EFI / UKI-Bootpfad des ausgewählten Systems. Dieses Legacy-Frontend zeigt keine EFI-Aktion; das Etch-Ziel ist ein BIOS / GRUB-Legacy-System.</translation>
    </message>
    <message>
        <source>GRUB configuration</source>
        <translation>GRUB-Konfiguration</translation>
    </message>
    <message>
        <source>Regenerate GRUB</source>
        <translation>GRUB regenerieren</translation>
    </message>
    <message>
        <source>Regenerate the selected repair system&apos;s GRUB menu/configuration after the mandatory safety preflight. The helper backs up menu.lst, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Regenerieren Sie das GRUB-Menü/die Konfiguration des ausgewählten Reparatursystems nach dem obligatorischen Sicherheitsvorflug. Der Helfer sichert menu.lst, bewahrt jeden vorhandenen Booteintrag und rollt bei einem Fehler zurück.</translation>
    </message>
    <message>
        <source>Regenerate the GRUB configuration? The helper backs up the target menu/configuration, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Regenerieren Sie die GRUB Konfiguration? Der Helfer sichert das Zielmenü / die Zielkonfiguration, bewahrt jeden vorhandenen Booteintrag und rollt bei einem Fehler zurück.</translation>
    </message>
    <message>
        <source>extlinux configuration</source>
        <translation>extlinux-Konfiguration</translation>
    </message>
    <message>
        <source>Regenerate extlinux</source>
        <translation>extlinux regenerieren</translation>
    </message>
    <message>
        <source>Regenerate the selected system&apos;s extlinux bootloader configuration. This legacy frontend exposes no extlinux action.</source>
        <translation>Regenerieren Sie die extlinux-Bootloader-Konfiguration des ausgewählten Systems. Dieses Legacy-Frontend zeigt keine extlinux-Aktion.</translation>
    </message>
    <message>
        <source>Boot stack reconciliation</source>
        <translation>Boot-Stack-Abgleich</translation>
    </message>
    <message>
        <source>Reconcile Boot Stack</source>
        <translation>Boot-Stack abgleichen</translation>
    </message>
    <message>
        <source>Reconcile the selected repair system&apos;s boot stack in one guarded legacy pass: mapper/crypttab validation, initramfs rebuild and GRUB-legacy configuration regeneration, with the component backups and preflights unchanged. This is the Etch equivalent of the modern boot-stack reconciliation and stays out of the Full Repair plan.</source>
        <translation>Stellen Sie den Boot-Stack des ausgewählten Reparatursystems in einem geschützten Legacy-Pass in Einklang: Mapper/Crypttab-Validierung, initramfs-Wiederaufbau und GRUB-Legacy-Konfigurationsregeneration, wobei die Komponenten-Backups und Preflights unverändert bleiben. Dies ist das Äquivalent des modernen Boot-Stack-Abgleichs und bleibt aus dem Full Repair-Plan heraus.</translation>
    </message>
    <message>
        <source>Full Repair plan: Manual recovery tool</source>
        <translation>Vollständiger Reparaturplan: Manuelles Wiederherstellungswerkzeug</translation>
    </message>
    <message>
        <source>Run the guarded boot-stack reconciliation? The helper runs the mapper/crypttab validation, the initramfs rebuild and the GRUB-legacy regeneration in one pass with every component preflight and backup.</source>
        <translation>Führen Sie die bewachte Boot-Stack-Versöhnung aus? Der Helfer führt die Mapper-/Crypttab-Validierung, den initramfs-Umbau und die GRUB-Legacy-Regeneration in einem Durchgang mit jedem Komponenten-Preflight und Backup durch.</translation>
    </message>
    <message>
        <source>Complete interrupted package configuration</source>
        <translation>Unterbrochene Paketkonfiguration abschließen</translation>
    </message>
    <message>
        <source>Repair broken package dependencies</source>
        <translation>Defekte Paketabhängigkeiten reparieren</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Als Standard festlegen</translation>
    </message>
    <message>
        <source>Repair file system errors (read-only check first)</source>
        <translation>Dateisystemfehler reparieren (zuerst schreibgeschützte Prüfung)</translation>
    </message>
    <message>
        <source>the legacy frontend exposes the read-only file system check only; per-device repair is not wired on this frontend (fail closed)</source>
        <translation>Das Legacy-Frontend stellt nur die schreibgeschützte Dateisystemprüfung frei; die Reparatur pro Gerät ist nicht an diesem Frontend verdrahtet (Fail closed)</translation>
    </message>
    <message>
        <source>Upgrade installed packages (adaptive APT simulation)</source>
        <translation>Installierte Pakete aktualisieren (adaptive APT-Simulation)</translation>
    </message>
    <message>
        <source>Rebuild DKMS modules</source>
        <translation>DKMS-Module neu aufbauen</translation>
    </message>
    <message>
        <source>Restore graphical login manager</source>
        <translation>Wiederherstellung des grafischen Login-Managers</translation>
    </message>
    <message>
        <source>Rebuild initramfs after mapper/crypttab validation</source>
        <translation>initramfs nach Mapper-/Crypttab-Prüfung neu aufbauen</translation>
    </message>
    <message>
        <source>Repair EFI / UKI boot path</source>
        <translation>EFI / UKI-Bootpfad reparieren</translation>
    </message>
    <message>
        <source>Update GRUB configuration</source>
        <translation>GRUB-Konfiguration aktualisieren</translation>
    </message>
    <message>
        <source>Update extlinux configuration</source>
        <translation>extlinux-Konfiguration aktualisieren</translation>
    </message>
    <message>
        <source>Choose Full Repair stages in Settings. Enabled stages run in the order shown. Each configurable stage also appears below as an individual tool; the Full Repair column mirrors its current Settings state. The boot tools (EFI / UKI bootloader, GRUB or extlinux configuration, boot-stack reconciliation and Make Default) are independent: run them in any order, and a later action re-verifies what an earlier one changed and reports its own result. The active scope is shown beside Repair: selected repair drive or Running Host maintenance.</source>
        <translation>Wählen Sie vollständige Reparaturphasen in den Einstellungen. Aktivierte Stufen laufen in der gezeigten Reihenfolge. Jede konfigurierbare Stufe erscheint auch unten als individuelles Werkzeug; die Spalte Volle Reparatur spiegelt den aktuellen Einstellungszustand wider. Die Boot-Tools (EFI / UKI-Bootloader, GRUB- oder extlinux-Konfiguration, Boot-Stack-Abgleich und Make Default) sind unabhängig: Führen Sie sie in beliebiger Reihenfolge aus, und eine spätere Aktion überprüft erneut, was eine frühere geändert hat, und meldet ihr eigenes Ergebnis. Der aktive Bereich wird neben Reparatur angezeigt: ausgewählte Reparaturfahrt oder Running Host Wartung.</translation>
    </message>
    <message>
        <source>Run All diagnostics for the selected target or running host before starting Full Repair. The report is read-only evidence used to choose and confirm repair stages.</source>
        <translation>Führen Sie alle Diagnosen für das ausgewählte Ziel oder den ausgeführten Host aus, bevor Sie die vollständige Reparatur starten. Der Bericht ist ein schreibgeschützter Nachweis, der zur Auswahl und Bestätigung von Reparaturphasen verwendet wird.</translation>
    </message>
    <message>
        <source>Ready: required cached read-only diagnostics are available for the selected stages. Review them in Diagnostics or Logs before confirming.</source>
        <translation>Ready: Die erforderliche zwischengespeicherte Read-Only-Diagnose ist für die ausgewählten Phasen verfügbar. Überprüfen Sie sie in Diagnose oder Protokolle, bevor Sie bestätigen.</translation>
    </message>
    <message>
        <source>Environment validation</source>
        <translation>Umgebungsprüfung</translation>
    </message>
    <message>
        <source>Summarizes the selected system, protection state, mounted identity and inspection readiness.</source>
        <translation>Fasst das ausgewählte System, den Schutzzustand, die montierte Identität und die Inspektionsbereitschaft zusammen.</translation>
    </message>
    <message>
        <source>Distribution and boot backend profile</source>
        <translation>Verteilung und Boot Backend Profil</translation>
    </message>
    <message>
        <source>Identifies the distribution family, package manager, initramfs generator, bootloader and current guarded repair capability.</source>
        <translation>Identifiziert die Verteilungsfamilie, den Paketmanager, den initramfs-Generator, den Bootloader und die aktuell geschützte Reparaturfähigkeit.</translation>
    </message>
    <message>
        <source>Boot diagnostics</source>
        <translation>Startdiagnose</translation>
    </message>
    <message>
        <source>Shows boot mounts and /boot contents plus storage evidence without changing the selected system.</source>
        <translation>Zeigt Boot-Mounts und /Boot-Inhalte sowie Speichernachweise an, ohne das ausgewählte System zu ändern.</translation>
    </message>
    <message>
        <source>Boot evidence and selection history</source>
        <translation>Boot-Beweis und Auswahlhistorie</translation>
    </message>
    <message>
        <source>Correlates the detected boot chain, bootloader selection, kernel/initramfs and unlock evidence.</source>
        <translation>Korreliert die erkannten Boot-Kette, Bootloader-Auswahl, Kernel / initramfs und entsperrt Beweise.</translation>
    </message>
    <message>
        <source>Kernel / initramfs</source>
        <translation>Kernel / initramfs</translation>
    </message>
    <message>
        <source>Reviews kernel files and verifies matching initramfs images through a read-only inspection.</source>
        <translation>Überprüfen Sie Kerneldateien und überprüfen Sie übereinstimmende initramfs-Bilder durch eine schreibgeschützte Inspektion.</translation>
    </message>
    <message>
        <source>Reviews the GRUB configuration without changing boot files.</source>
        <translation>Überprüfen Sie die GRUB-Konfiguration, ohne Boot-Dateien zu ändern.</translation>
    </message>
    <message>
        <source>EFI / UKI boot state</source>
        <translation>EFI / UKI-Bootzustand</translation>
    </message>
    <message>
        <source>Inspects EFI/UKI evidence; unavailable on this legacy BIOS frontend with the helper&apos;s probe reason.</source>
        <translation>Inspiziert EFI/UKI-Beweise; auf diesem Legacy-BIOS-Frontend mit dem Sondengrund des Helfers nicht verfügbar.</translation>
    </message>
    <message>
        <source>Reviews the configured display manager and recent boot evidence without starting the GUI.</source>
        <translation>Überprüfen Sie den konfigurierten Display-Manager und den letzten Boot-Beweis, ohne die GUI zu starten.</translation>
    </message>
    <message>
        <source>Boot errors</source>
        <translation>Startfehler</translation>
    </message>
    <message>
        <source>Reads recent error-priority entries from the running host or selected repair system when available.</source>
        <translation>Lesen Sie die letzten Fehlerprioritätseinträge des laufenden Hosts oder des ausgewählten Reparatursystems, wenn verfügbar.</translation>
    </message>
    <message>
        <source>Disk usage</source>
        <translation>Speicherplatznutzung</translation>
    </message>
    <message>
        <source>Summarizes filesystem capacity and free space for the running host or read-only repair target.</source>
        <translation>Fasst die Dateisystemkapazität und den freien Speicherplatz für den laufenden Host oder das schreibgeschützte Reparaturziel zusammen.</translation>
    </message>
    <message>
        <source>File systems</source>
        <translation>Dateisysteme</translation>
    </message>
    <message>
        <source>Runs the read-only file system check for the selected system&apos;s root, /boot and other filesystems.</source>
        <translation>Führt die schreibgeschützte Dateisystemprüfung für die Root-, /Boot- und anderen Dateisysteme des ausgewählten Systems aus.</translation>
    </message>
    <message>
        <source>/etc/fstab review</source>
        <translation>/etc/fstab review</translation>
    </message>
    <message>
        <source>Displays the running host or selected repair system&apos;s fstab; repair-system inspection is mounted read-only.</source>
        <translation>Zeigt den Fstab des laufenden Hosts oder des ausgewählten Reparatursystems an; die Inspektion des Reparatursystems ist schreibgeschützt montiert.</translation>
    </message>
    <message>
        <source>Btrfs status</source>
        <translation>Btrfs-Status</translation>
    </message>
    <message>
        <source>Shows Btrfs filesystem and subvolume information when the target uses Btrfs.</source>
        <translation>Zeigt Btrfs-Dateisystem- und Untervolumeninformationen an, wenn das Ziel Btrfs verwendet.</translation>
    </message>
    <message>
        <source>Device-mapper ancestry</source>
        <translation>Device-Mapper-Abstammung</translation>
    </message>
    <message>
        <source>Shows selected mapper ancestry and device-mapper state when available.</source>
        <translation>Zeigt ausgewählte Mapper-Abstammung und Device-Mapper-Zustand an, wenn verfügbar.</translation>
    </message>
    <message>
        <source>LUKS / crypttab evidence</source>
        <translation>LUKS / crypttab Beweis</translation>
    </message>
    <message>
        <source>Shows LUKS/mapped ancestry plus crypttab and fstab mapper references.</source>
        <translation>Zeigt LUKS/mapped ancestry plus crypttab und fstab mapper Referenzen.</translation>
    </message>
    <message>
        <source>Full diagnostic report</source>
        <translation>Vollständiger Diagnosebericht</translation>
    </message>
    <message>
        <source>Combines all read-only diagnostics for the selected scope (same as Run All).</source>
        <translation>Kombiniert alle schreibgeschützten Diagnosen für den ausgewählten Bereich (gleich wie Run All).</translation>
    </message>
    <message>
        <source>All entries</source>
        <translation>Alle Einträge</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>Diagnose</translation>
    </message>
    <message>
        <source>Repairs</source>
        <translation>Reparaturen</translation>
    </message>
    <message>
        <source>Package repair</source>
        <translation>Paketreparatur</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Dateikopie</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Geräteerkennung</translation>
    </message>
    <message>
        <source>Host</source>
        <translation>Gastgeber</translation>
    </message>
    <message>
        <source>Required for block-device inventory</source>
        <translation>Erforderlich für Block-Device-Inventar</translation>
    </message>
    <message>
        <source>Filesystem identification</source>
        <translation>Dateisystemkennung</translation>
    </message>
    <message>
        <source>Used to identify filesystem metadata</source>
        <translation>Verwendung zur Identifizierung von Dateisystem-Metadaten</translation>
    </message>
    <message>
        <source>Mount inspection</source>
        <translation>Montageinspektion</translation>
    </message>
    <message>
        <source>Used to understand active mounts</source>
        <translation>Verwendet, um aktive Mounts zu verstehen</translation>
    </message>
    <message>
        <source>LUKS support</source>
        <translation>LUKS Unterstützung</translation>
    </message>
    <message>
        <source>Required to unlock encrypted targets</source>
        <translation>Erforderlich zum Entsperren verschlüsselter Ziele</translation>
    </message>
    <message>
        <source>Btrfs support</source>
        <translation>Btrfs Unterstützung</translation>
    </message>
    <message>
        <source>Required for Btrfs inspection and snapshot rollback</source>
        <translation>Erforderlich für Btrfs-Inspektion und Snapshot-Rollback</translation>
    </message>
    <message>
        <source>Bidirectional file copy</source>
        <translation>Bidirektionale Dateikopie</translation>
    </message>
    <message>
        <source>Host/Repair</source>
        <translation>Host/Reparatur</translation>
    </message>
    <message>
        <source>Required for verified Host-to-Repair and Repair-to-Host transfer</source>
        <translation>Erforderlich für verifizierte Host-to-Repair und Repair-to-Host Übertragung</translation>
    </message>
    <message>
        <source>Chroot repair</source>
        <translation>Chroot-Reparatur</translation>
    </message>
    <message>
        <source>Required for target-side repair commands</source>
        <translation>Erforderlich für zielseitige Reparaturbefehle</translation>
    </message>
    <message>
        <source>Offline systemd repair</source>
        <translation>Offline-Systemreparatur</translation>
    </message>
    <message>
        <source>Used to restore graphical.target and the configured display manager without starting the target GUI</source>
        <translation>Wird verwendet, um grafische wiederherstellen. target und der konfigurierte Display-Manager, ohne die Ziel-GUI zu starten</translation>
    </message>
    <message>
        <source>UEFI NVRAM inspection</source>
        <translation>UEFI NVRAM Inspektion</translation>
    </message>
    <message>
        <source>Used to preserve target EFI BootOrder during TUXEDO UKI rebuilds when efivars are available</source>
        <translation>Wird verwendet, um das Ziel EFI BootOrder während des TUXEDO UKI-Wiederaufbaus zu erhalten, wenn Efivars verfügbar sind</translation>
    </message>
    <message>
        <source>UKI verification</source>
        <translation>UKI-Prüfung</translation>
    </message>
    <message>
        <source>Used to verify the kernel embedded in a rebuilt unified kernel image</source>
        <translation>Wird verwendet, um den Kernel zu verifizieren, der in ein neu erstelltes einheitliches Kernelbild eingebettet ist</translation>
    </message>
    <message>
        <source>GRUB EFI repair</source>
        <translation>GRUB EFI Reparatur</translation>
    </message>
    <message>
        <source>Target/Host</source>
        <translation>Ziel/Host</translation>
    </message>
    <message>
        <source>Required only for conventional GRUB-based EFI systems</source>
        <translation>Nur für konventionelle GRUB-basierte EFI-Systeme erforderlich</translation>
    </message>
    <message>
        <source>Target</source>
        <translation>Ziel</translation>
    </message>
    <message>
        <source>Debian-family GRUB helper</source>
        <translation>Debian-Familie GRUB Helfer</translation>
    </message>
    <message>
        <source>Portable GRUB configuration generator used by Arch and other non-Debian systems</source>
        <translation>Portabler GRUB-Konfigurationsgenerator, der von Arch und anderen Nicht-Debian-Systemen verwendet wird</translation>
    </message>
    <message>
        <source>Initramfs rebuild</source>
        <translation>Wiederaufbau von Initramfs</translation>
    </message>
    <message>
        <source>Debian-family initramfs helper</source>
        <translation>Debian-Familie initramfs Helfer</translation>
    </message>
    <message>
        <source>Arch-family initramfs generator</source>
        <translation>Erzfamilie initramfs Generator</translation>
    </message>
    <message>
        <source>Alternative initramfs generator used by Arch and other distributions</source>
        <translation>Alternative initramfs Generator von Arch und anderen Distributionen verwendet</translation>
    </message>
    <message>
        <source>Initramfs verification</source>
        <translation>Initramfs-Überprüfung</translation>
    </message>
    <message>
        <source>Read-only verification for mkinitcpio images</source>
        <translation>Read-Only-Verifizierung für mkinitcpio-Bilder</translation>
    </message>
    <message>
        <source>Read-only verification for dracut images</source>
        <translation>Read-Only-Verifizierung für dracut-Bilder</translation>
    </message>
    <message>
        <source>systemd-boot inspection</source>
        <translation>systemd-boot-Prüfung</translation>
    </message>
    <message>
        <source>Host/Target</source>
        <translation>Gastgeber/Ziel</translation>
    </message>
    <message>
        <source>Read-only inspection of systemd-boot and generic UKI layouts</source>
        <translation>Read-only Inspektion von systemd-boot und generischen UKI Layouts</translation>
    </message>
    <message>
        <source>Arch package manager</source>
        <translation>Arch Package Manager</translation>
    </message>
    <message>
        <source>Arch-family package database and transaction tool</source>
        <translation>Arch-Familie Paketdatenbank und Transaktionstool</translation>
    </message>
    <message>
        <source>DKMS rebuild</source>
        <translation>DKMS Wiederaufbau</translation>
    </message>
    <message>
        <source>Required only when target uses DKMS modules</source>
        <translation>Nur erforderlich, wenn target DKMS-Module verwendet</translation>
    </message>
    <message>
        <source>LVM inspection</source>
        <translation>LVM-Inspektion</translation>
    </message>
    <message>
        <source>Optional LVM storage-stack support</source>
        <translation>Optionale LVM-Stack-Unterstützung</translation>
    </message>
    <message>
        <source>Software RAID</source>
        <translation>Software-RAID</translation>
    </message>
    <message>
        <source>Optional Linux MD RAID support</source>
        <translation>Optionale Linux MD RAID Unterstützung</translation>
    </message>
    <message>
        <source>Process namespace isolation</source>
        <translation>Isolierung des Prozessnamensraums</translation>
    </message>
    <message>
        <source>Host+Target</source>
        <translation>Host + Ziel</translation>
    </message>
    <message>
        <source>The ported helper falls back to a guarded plain chroot when unshare is absent</source>
        <translation>Der portierte Helfer fällt zurück zu einem bewachten einfachen chroot, wenn unshare fehlt</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Stornierung</translation>
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
        <translation>Nein</translation>
    </message>
    <message>
        <source>read-only helper diagnostic</source>
        <translation>Read-Only-Helferdiagnose</translation>
    </message>
</context>
<context>
    <name>legacy::LegacyMainWindow</name>
    <message>
        <source>Boot Bitch Legacy (Etch / KDE 3.5 era)</source>
        <translation>Boot Bitch Legacy (Ära Etch / KDE 3.5)</translation>
    </message>
    <message>
        <source>Linux recovery and boot-repair utility</source>
        <translation>Linux Recovery und Boot-Repair Utility</translation>
    </message>
    <message>
        <source>GUARDED REPAIR</source>
        <translation>GESCHÜTZTE REPARATUR</translation>
    </message>
    <message>
        <source>Ordinary repairs require an explicitly selected non-host target. The protected running host has a separate deliberate maintenance mode with the same guarded repair stages and requires privilege authorization.</source>
        <translation>Gewöhnliche Reparaturen erfordern ein explizit ausgewähltes Nicht-Host-Ziel. Der geschützte Laufhost verfügt über einen separaten absichtlichen Wartungsmodus mit den gleichen geschützten Reparaturstufen und erfordert eine Berechtigungsberechtigung.</translation>
    </message>
    <message>
        <source>Systems</source>
        <translation>Systeme</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>Diagnose</translation>
    </message>
    <message>
        <source>Repair</source>
        <translation>Reparieren</translation>
    </message>
    <message>
        <source>Chroot Shell</source>
        <translation>Chroot-Shell</translation>
    </message>
    <message>
        <source>File Copy</source>
        <translation>Dateikopie</translation>
    </message>
    <message>
        <source>Logs</source>
        <translation>Protokolle</translation>
    </message>
    <message>
        <source>Settings</source>
        <translation>Einstellungen</translation>
    </message>
    <message>
        <source>(not created yet)</source>
        <translation>(noch nicht erstellt)</translation>
    </message>
    <message>
        <source>Information</source>
        <translation>Information</translation>
    </message>
    <message>
        <source>Close</source>
        <translation>Schließen</translation>
    </message>
    <message>
        <source>&amp;File</source>
        <translation>&amp;Datei</translation>
    </message>
    <message>
        <source>&amp;Refresh Devices</source>
        <translation>&amp;Refresh Geräte</translation>
    </message>
    <message>
        <source>&amp;Lock Administrator Session</source>
        <translation>&amp;Lock Administrator Sitzung</translation>
    </message>
    <message>
        <source>&amp;Quit</source>
        <translation>&amp;beendet</translation>
    </message>
    <message>
        <source>&amp;View</source>
        <translation>&amp;Ansicht</translation>
    </message>
    <message>
        <source>&amp;Systems</source>
        <translation>-&amp;Systeme</translation>
    </message>
    <message>
        <source>&amp;Diagnostics</source>
        <translation>&amp;Diagnose</translation>
    </message>
    <message>
        <source>&amp;Logs</source>
        <translation>&amp;Logs</translation>
    </message>
    <message>
        <source>&amp;Settings</source>
        <translation>&amp;Einstellungen</translation>
    </message>
    <message>
        <source>&amp;Auto-size Device Columns</source>
        <translation>&amp;Auto-Size Gerätespalten</translation>
    </message>
    <message>
        <source>&amp;Wrap Log Lines</source>
        <translation>&amp;Wrap Log Linien</translation>
    </message>
    <message>
        <source>&amp;Help</source>
        <translation>&amp;Hilfe</translation>
    </message>
    <message>
        <source>&amp;Using Boot Bitch</source>
        <translation>&amp;mit Boot Bitch</translation>
    </message>
    <message>
        <source>&amp;About Boot Bitch</source>
        <translation>ü&amp;ber Boot Bitch</translation>
    </message>
    <message>
        <source>Device columns auto-sized. Drag headers to fine-tune widths.</source>
        <translation>Gerätesäulen automatisch dimensioniert. Ziehen Sie Header auf Feinabstimmungsbreiten.</translation>
    </message>
    <message>
        <source>A helper command is running; wait for it to finish before locking the session.</source>
        <translation>Ein Helferbefehl läuft; warten Sie, bis er beendet ist, bevor Sie die Sitzung sperren.</translation>
    </message>
    <message>
        <source>Administrator session locked; the next privileged action will request authorization.</source>
        <translation>Administratorsitzung gesperrt; die nächste privilegierte Aktion fordert die Autorisierung an.</translation>
    </message>
    <message>
        <source>Using Boot Bitch</source>
        <translation>Mit Boot Bitch</translation>
    </message>
    <message>
        <source>Boot Bitch must run from a different booted Linux environment than the system being repaired. Use a Linux live medium or another Linux installation on a different physical drive.&lt;br&gt;&lt;br&gt;The running host is protected from ordinary repair-target selection, but it can be explicitly selected through &lt;b&gt;Host Maintenance&lt;/b&gt; for guarded native diagnostics and supported maintenance stages.&lt;br&gt;&lt;br&gt;Diagnostics follow the Systems page: the committed repair drive while Host Maintenance is off, or the protected running host while it is active.&lt;br&gt;&lt;br&gt;The first privileged action requests administrator authorization once for this Boot Bitch window; &lt;b&gt;File - Lock Administrator Session&lt;/b&gt; ends that helper session immediately. Every repair keeps the helper&apos;s own runtime preflights.</source>
        <translation>Boot Bitch muss aus einer anderen gestarteten Linux-Umgebung laufen als das System, das repariert wird. Verwenden Sie ein Linux-Live-Medium oder eine andere Linux-Installation auf einem anderen physischen Laufwerk.&lt;br&gt;&lt;br&gt; Der laufende Host ist vor der gewöhnlichen Reparaturzielauswahl geschützt, kann jedoch explizit durch &lt;b&gt;Host Maintenance&lt;/b&gt; für geschützte native Diagnosen und unterstützte Wartungsschritte ausgewählt werden.&lt;br&gt;&lt;br&gt; Die Diagnose folgt der Systemseite: die engagierte Reparaturfahrt, während die Host-Wartung ausgeschaltet ist, oder der geschützte laufende Host, während er aktiv ist.&lt;br&gt;&lt;br&gt; Die erste privilegierte Aktion fordert die Administratorautorisierung einmal für dieses Boot Bitch-Fenster an; &lt;b&gt;Datei - Sperren der Administratorsitzung&lt;/b&gt; beendet diese Helfersitzung sofort. Jede Reparatur behält die eigenen Laufzeit-Preflights des Helfers.</translation>
    </message>
    <message>
        <source>Select a physical drive; Boot Bitch resolves the most likely Linux system volume automatically. The running host stays protected from ordinary target repairs, with a separate explicit host-maintenance path for its own system. The Details button shows the protected host&apos;s facts in the details pane; selecting any drive row restores the per-drive pane.</source>
        <translation>Wählen Sie ein physisches Laufwerk; Boot Bitch löst das wahrscheinlichste Linux-Systemvolumen automatisch auf. Der laufende Host bleibt vor normalen Zielreparaturen geschützt, mit einem separaten expliziten Host-Wartungspfad für sein eigenes System. Die Schaltfläche Details zeigt die Fakten des geschützten Hosts im Detailbereich an; Auswählen einer beliebigen Laufwerkszeile stellt den Laufwerksbereich wieder her.</translation>
    </message>
    <message>
        <source>Refresh Devices</source>
        <translation>Geräte aktualisieren</translation>
    </message>
    <message>
        <source>Re-read the read-only kernel inventory (/proc/partitions, /proc/mounts, /proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* and the udev metadata database). No block device is opened and nothing is written.</source>
        <translation>Lesen Sie das Read-Only-Kernel-Inventar (/proc/Partitionen, /proc/mounts, /proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* und die udev-Metadatendatenbank). Es wird kein Blockgerät geöffnet und nichts geschrieben.</translation>
    </message>
    <message>
        <source>[OK]</source>
        <translation>[OK]</translation>
    </message>
    <message>
        <source>The running system was detected and stays protected from ordinary target repairs.</source>
        <translation>Das laufende System wurde erkannt und bleibt vor normalen Reparaturen geschützt.</translation>
    </message>
    <message>
        <source>Detecting running system...</source>
        <translation>Laufendes System wird erkannt...</translation>
    </message>
    <message>
        <source>Detecting protected storage...</source>
        <translation>Erkennung geschützter Lagerung...</translation>
    </message>
    <message>
        <source>PROTECTED</source>
        <translation>GESCHÜTZT</translation>
    </message>
    <message>
        <source>The running host remains protected from ordinary repair-target operations.</source>
        <translation>Der laufende Host bleibt vor normalen Reparaturzieloperationen geschützt.</translation>
    </message>
    <message>
        <source>Details</source>
        <translation>Einzelheiten</translation>
    </message>
    <message>
        <source>Show read-only details for the protected running host.</source>
        <translation>Zeigen Sie schreibgeschützte Details für den geschützten laufenden Host an.</translation>
    </message>
    <message>
        <source>Host Maintenance</source>
        <translation>Host-Wartung</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Als Standard festlegen</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host (menu.lst default directive with a backup and rollback). Requires Host Maintenance and the cached host-default probe.</source>
        <translation>Machen Sie den kanonisch installierten Kerneleintrag zum standardmäßigen GRUB-Legacy-Booteintrag auf dem laufenden Host (menu.lst Standard-Direktive mit Backup und Rollback). Benötigt Host Maintenance und die zwischengespeicherte Host-Default-Sonde.</translation>
    </message>
    <message>
        <source>Available repair targets</source>
        <translation>Verfügbare Reparaturziele</translation>
    </message>
    <message>
        <source>Most likely first</source>
        <translation>Wahrscheinlichste zuerst</translation>
    </message>
    <message>
        <source>Drives are listed by the read-only inventory. Select a row to inspect it; Select Target commits the selected non-host drive with its auto-resolved Linux root component.</source>
        <translation>Laufwerke werden im Read-Only-Inventar aufgeführt. Wählen Sie eine Zeile aus, um sie zu überprüfen; Select Target verpflichtet das ausgewählte Nicht-Host-Laufwerk mit seiner automatisch aufgelösten Linux-Root-Komponente.</translation>
    </message>
    <message>
        <source>Device</source>
        <translation>Vorrichtung</translation>
    </message>
    <message>
        <source>Size</source>
        <translation>Größe</translation>
    </message>
    <message>
        <source>Type</source>
        <translation>Typ</translation>
    </message>
    <message>
        <source>Filesystem</source>
        <translation>Dateisystem</translation>
    </message>
    <message>
        <source>Select Target</source>
        <translation>Ziel auswählen</translation>
    </message>
    <message>
        <source>Unlock</source>
        <translation>Entsperren</translation>
    </message>
    <message>
        <source>Authorize</source>
        <translation>Autorisieren</translation>
    </message>
    <message>
        <source>Establish the privileged helper session for the current scope now instead of waiting for the next privileged action.</source>
        <translation>Legen Sie jetzt die privilegierte Helfersitzung für den aktuellen Umfang fest, anstatt auf die nächste privilegierte Aktion zu warten.</translation>
    </message>
    <message>
        <source>Committed target: none</source>
        <translation>Festgelegtes Ziel: keines</translation>
    </message>
    <message>
        <source>Unlock state for the selected drive; the LUKS passphrase is never logged.</source>
        <translation>Freigabezustand für das ausgewählte Laufwerk; die LUKS-Passphrase wird nie protokolliert.</translation>
    </message>
    <message>
        <source>Unlock status</source>
        <translation>Entsperrstatus</translation>
    </message>
    <message>
        <source>Read-only inventory plus helper-confirmed facts; mirrors the modern Qt6 Selected drive details panel.</source>
        <translation>Read-only Inventar plus Helfer bestätigte Fakten; Spiegelt das moderne Qt6 Selected Drive Details Panel.</translation>
    </message>
    <message>
        <source>Selected drive details</source>
        <translation>Details des ausgewählten Laufwerks</translation>
    </message>
    <message>
        <source>Field</source>
        <translation>Feld</translation>
    </message>
    <message>
        <source>Value</source>
        <translation>Wert</translation>
    </message>
    <message>
        <source>Run All runs every available read-only diagnostic for the current scope; selecting a check runs it alone. Diagnostics are read-only and are the only evidence source for the gated repair actions.</source>
        <translation>Run All führt jede verfügbare schreibgeschützte Diagnose für den aktuellen Umfang aus; die Auswahl eines Checks führt ihn alleine aus. Diagnosen sind schreibgeschützt und die einzige Beweisquelle für die abgeschlossenen Reparaturaktionen.</translation>
    </message>
    <message>
        <source>Target: none selected</source>
        <translation>Ziel: keine ausgewählt</translation>
    </message>
    <message>
        <source>Diagnostics follow the committed repair target, or the protected running host while Host Maintenance is active.</source>
        <translation>Die Diagnose folgt dem festgelegten Reparaturziel oder dem geschützten laufenden Host, während die Hostwartung aktiv ist.</translation>
    </message>
    <message>
        <source>Run All</source>
        <translation>Alle ausführen</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope.</source>
        <translation>Run All - Führen Sie jede verfügbare schreibgeschützte Diagnose für den aktuellen Umfang aus.</translation>
    </message>
    <message>
        <source>Target configuration:</source>
        <translation>Zielkonfiguration:</translation>
    </message>
    <message>
        <source>Etch-era target configuration files; availability is probed read-only by the helper&apos;s diagnostics.</source>
        <translation>Etch-Ära Zielkonfigurationsdateien; die Verfügbarkeit wird durch die Diagnose des Helfers schreibgeschützt geprüft.</translation>
    </message>
    <message>
        <source>Edit Target File...</source>
        <translation>Zieldatei bearbeiten...</translation>
    </message>
    <message>
        <source>Runs one read-only diagnostic for the selected scope through the helper (`diagnose &lt;key&gt;` / `host-diagnose &lt;key&gt;`); Run All is the combined report.</source>
        <translation>Führt eine schreibgeschützte Diagnose für den ausgewählten Bereich über den Helfer aus („Diagnose &lt;key&gt;) / („Host-Diagnose &lt;key&gt;); Run All ist der kombinierte Bericht.</translation>
    </message>
    <message>
        <source>Diagnostic checks</source>
        <translation>Diagnoseprüfungen</translation>
    </message>
    <message>
        <source>Check</source>
        <translation>Überprüfung</translation>
    </message>
    <message>
        <source>Selected diagnostic</source>
        <translation>Ausgewählte Diagnose</translation>
    </message>
    <message>
        <source>Select a diagnostic</source>
        <translation>Wählen Sie eine Diagnose</translation>
    </message>
    <message>
        <source>Choose a diagnostic from the list.</source>
        <translation>Wählen Sie eine Diagnose aus der Liste.</translation>
    </message>
    <message>
        <source>Ready</source>
        <translation>Bereit</translation>
    </message>
    <message>
        <source>Results</source>
        <translation>Ergebnisse</translation>
    </message>
    <message>
        <source>Run Diagnostic</source>
        <translation>Diagnose ausführen</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic for the current scope.</source>
        <translation>Diagnose ausführen - Führen Sie die ausgewählte schreibgeschützte Diagnose für den aktuellen Umfang aus.</translation>
    </message>
    <message>
        <source>Copy Results</source>
        <translation>Ergebnisse kopieren</translation>
    </message>
    <message>
        <source>Save Results...</source>
        <translation>Ergebnisse speichern...</translation>
    </message>
    <message>
        <source>The Full Repair plan runs the selected legacy stages in order through the guarded helper; the individual tools run one stage at a time. Every action stays disabled until the cached capability lines say available and the helper keeps its runtime preflights.</source>
        <translation>Der Full Repair-Plan führt die ausgewählten Legacy-Stufen durch den bewachten Helfer aus; Die einzelnen Werkzeuge laufen eine Stufe nach der anderen. Jede Aktion bleibt deaktiviert, bis die zwischengespeicherten Fähigkeitszeilen verfügbar sind und der Helfer seine Laufzeitvorflüge behält.</translation>
    </message>
    <message>
        <source>Full Repair plan</source>
        <translation>Plan für die vollständige Reparatur</translation>
    </message>
    <message>
        <source>No stages selected</source>
        <translation>Keine Phasen ausgewählt</translation>
    </message>
    <message>
        <source>Configure Plan...</source>
        <translation>Plan konfigurieren...</translation>
    </message>
    <message>
        <source>Open Settings to choose which Full Repair stages are part of the plan.</source>
        <translation>Öffnen Sie Einstellungen, um auszuwählen, welche vollständigen Reparaturphasen Teil des Plans sind.</translation>
    </message>
    <message>
        <source>Run Full Repair</source>
        <translation>Vollständige Reparatur ausführen</translation>
    </message>
    <message>
        <source>Select a repair drive, or choose Host Maintenance on the protected running-host card.</source>
        <translation>Wählen Sie eine Reparaturfahrt oder wählen Sie Host Maintenance auf der geschützten Running-Host-Karte.</translation>
    </message>
    <message>
        <source>Stage</source>
        <translation>Phase</translation>
    </message>
    <message>
        <source>Individual repair tools</source>
        <translation>Einzelne Reparaturwerkzeuge</translation>
    </message>
    <message>
        <source>Tool</source>
        <translation>Werkzeug</translation>
    </message>
    <message>
        <source>Full Repair</source>
        <translation>Vollständige Reparatur</translation>
    </message>
    <message>
        <source>not reported</source>
        <translation>nicht gemeldet</translation>
    </message>
    <message>
        <source>Selected tool</source>
        <translation>Ausgewähltes Werkzeug</translation>
    </message>
    <message>
        <source>Select a repair tool</source>
        <translation>Reparaturwerkzeug auswählen</translation>
    </message>
    <message>
        <source>Run Tool</source>
        <translation>Werkzeug ausführen</translation>
    </message>
    <message>
        <source>Select a tool to review its repair action.</source>
        <translation>Wählen Sie ein Werkzeug, um seine Reparaturaktion zu überprüfen.</translation>
    </message>
    <message>
        <source>Write actions ask for confirmation and then run the helper&apos;s own runtime preflights; the GUI never weakens them. A repair that is not proven &apos;unchanged&apos; invalidates the cached diagnostics and disables the gated actions until diagnostics run again.</source>
        <translation>Schreibaktionen bitten um Bestätigung und führen dann die eigenen Laufzeitvorflüge des Helfers aus; die GUI schwächt sie nie. Eine Reparatur, die nicht als &quot;unverändert&quot; erwiesen ist, macht die zwischengespeicherte Diagnose ungültig und deaktiviert die Gated-Aktionen, bis die Diagnose erneut ausgeführt wird.</translation>
    </message>
    <message>
        <source>Chroot shell</source>
        <translation>Chroot-Shell</translation>
    </message>
    <message>
        <source>Offline commands run one at a time in a fresh chroot and cannot answer interactive prompts (apt-get -y upgrade works). Host-shell commands run directly on the running host. The helper&apos;s probe lines gate the command field; the exact reason appears in the tooltip.</source>
        <translation>Offline-Befehle laufen einzeln in einem neuen chroot und können keine interaktiven Eingabeaufforderungen beantworten (apt-get-y Upgrade funktioniert). Host-Shell-Befehle laufen direkt auf dem laufenden Host. Die Sondenzeilen des Helfers geben das Befehlsfeld an; der genaue Grund erscheint im Tooltip.</translation>
    </message>
    <message>
        <source>No &apos;Legacy feature shell:&apos; line is cached; run diagnostics for the selected scope to evaluate the helper&apos;s chroot/timeout containment probes (fail closed).</source>
        <translation>Keine &apos;Legacy-Feature-Shell:&apos;-Zeile wird zwischengespeichert; führen Sie Diagnosen für den ausgewählten Bereich aus, um die Chroot / Timeout-Containment-Sonden des Helfers zu bewerten (Fail closed).</translation>
    </message>
    <message>
        <source>Command</source>
        <translation>Kommando</translation>
    </message>
    <message>
        <source>Command:</source>
        <translation>Befehl:</translation>
    </message>
    <message>
        <source>One reviewed command string, passed to the helper as a single argument (no shell interpolation by the GUI).</source>
        <translation>Eine überprüfte Befehlsfolge wurde als einzelnes Argument an den Helfer übergeben (keine Shell-Interpolation durch die GUI).</translation>
    </message>
    <message>
        <source>Run Command</source>
        <translation>Befehl ausführen</translation>
    </message>
    <message>
        <source>Clear Output</source>
        <translation>Ausgabe löschen</translation>
    </message>
    <message>
        <source>The helper exposes `shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for an offline target chroot and `host-shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for the running host. Both keep the helper&apos;s runtime preflights; this tab enables the command only when the scope is committed, the session is authorized and the scope&apos;s Legacy feature probe reports available.</source>
        <translation>Der Helfer stellt `shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` für ein Offline-Ziel chroot und `host-shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` für den laufenden Host frei. Beide behalten die Laufzeit-Preflights des Helfers; Diese Registerkarte aktiviert den Befehl nur, wenn der Umfang festgelegt ist, die Sitzung autorisiert ist und die Legacy-Feature-Sondenberichte des Bereichs verfügbar sind.</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Dateikopie</translation>
    </message>
    <message>
        <source>Copy and verify files in either direction through the guarded helper (cp -a plus ownership restoration and a per-file byte-compare). The helper&apos;s file-copy probe gates the controls and keeps the direction and path containment checks.</source>
        <translation>Kopieren und Verifizieren von Dateien in beide Richtungen durch den bewachten Helfer (cp -a plus Eigentumswiederherstellung und ein Byte-Vergleich per Datei). Die File-Copy-Sonde des Helfers führt die Kontrollen durch und behält die Richtungs- und Pfadeindämmungsprüfungen bei.</translation>
    </message>
    <message>
        <source>Preview Changes</source>
        <translation>Vorschau der Änderungen</translation>
    </message>
    <message>
        <source>Run a copy dry-run through the guarded helper. No files are changed.</source>
        <translation>Führen Sie eine Kopie Dry-Run durch den bewachten Helfer. Es werden keine Dateien geändert.</translation>
    </message>
    <message>
        <source>Copy and Verify</source>
        <translation>Kopieren und prüfen</translation>
    </message>
    <message>
        <source>Copy staged items and verify the result. Existing destination names are overwritten when source content differs; unrelated destination files are never deleted.</source>
        <translation>Kopieren Sie inszenierte Elemente und überprüfen Sie das Ergebnis. Bestehende Zielnamen werden überschrieben, wenn sich der Quellinhalt unterscheidet; nicht verwandte Zieldateien werden niemals gelöscht.</translation>
    </message>
    <message>
        <source>Direction:</source>
        <translation>Richtung:</translation>
    </message>
    <message>
        <source>Choose which system supplies the source files and which system receives them.</source>
        <translation>Wählen Sie aus, welches System die Quelldateien liefert und welches System sie empfängt.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from this host</source>
        <translation>1. Wählen Sie Quelldateien oder Ordner von diesem Host</translation>
    </message>
    <message>
        <source>Source</source>
        <translation>Quelle</translation>
    </message>
    <message>
        <source>Files and folders staged for the verified copy. The legacy backend copies with cp -a and restores ownership with chown --reference; every regular file is byte-compared after the copy.</source>
        <translation>Dateien und Ordner für die verifizierte Kopie inszeniert. Das Legacy-Backend kopiert mit cp -a und stellt das Eigentum mit chown --reference wieder her; jede reguläre Datei wird nach der Kopie byteverglichen.</translation>
    </message>
    <message>
        <source>Add Files...</source>
        <translation>Dateien hinzufügen...</translation>
    </message>
    <message>
        <source>Add Folder...</source>
        <translation>Folder hinzufügen...</translation>
    </message>
    <message>
        <source>Remove</source>
        <translation>Entfernen</translation>
    </message>
    <message>
        <source>Clear</source>
        <translation>Leeren</translation>
    </message>
    <message>
        <source>Clear the staged source list (nothing is copied or deleted).</source>
        <translation>Löschen Sie die inszenierte Quellliste (nichts wird kopiert oder gelöscht).</translation>
    </message>
    <message>
        <source>2. Choose destination in repaired system</source>
        <translation>2. Bestimmungsort im reparierten System wählen</translation>
    </message>
    <message>
        <source>unavailable: see the helper&apos;s Legacy feature file-copy: probe reason above</source>
        <translation>nicht verfügbar: siehe Dateikopie der Legacy-Funktion des Helfers: Sonde Grund oben</translation>
    </message>
    <message>
        <source>An absolute path inside the selected repair system (Host to Repair) or on the running host (Repair to Host).</source>
        <translation>Ein absoluter Pfad innerhalb des ausgewählten Reparatursystems (Host to Repair) oder auf dem laufenden Host (Repair to Host).</translation>
    </message>
    <message>
        <source>Browse Target Folders...</source>
        <translation>Zielordner durchsuchen...</translation>
    </message>
    <message>
        <source>Browse the selected repair system through the helper&apos;s temporary read-only mounts and choose an absolute destination path. No target files are changed while browsing.</source>
        <translation>Durchsuchen Sie das ausgewählte Reparatursystem durch die temporären Read-Only-Halterungen des Helfers und wählen Sie einen absoluten Zielpfad. Beim Browsen werden keine Zieldateien geändert.</translation>
    </message>
    <message>
        <source>3. Ownership and copy policy</source>
        <translation>3. Eigentums- und Kopierpolitik</translation>
    </message>
    <message>
        <source>Ownership:</source>
        <translation>Eigentum:</translation>
    </message>
    <message>
        <source>Smart destination ownership (recommended)</source>
        <translation>Smart Destination Ownership (empfohlen)</translation>
    </message>
    <message>
        <source>Preserve source numeric UID/GID</source>
        <translation>Numerische UID/GID der Konservierungsquelle</translation>
    </message>
    <message>
        <source>Smart mode validates UID/GID identity mapping across the two systems and falls back to the destination-directory owner when the same numeric ID means a different account (the legacy backend implements it with chown --reference).</source>
        <translation>Der intelligente Modus validiert die UID/GID-Identitätszuordnung für die beiden Systeme und fällt auf den Besitzer des Zielverzeichnisses zurück, wenn dieselbe numerische ID ein anderes Konto bedeutet (das Legacy-Backend implementiert es mit chown --reference).</translation>
    </message>
    <message>
        <source>Application log</source>
        <translation>Anwendungsprotokoll</translation>
    </message>
    <message>
        <source>The complete session register; Save As... writes every entry even while a filter hides lines. If a writable system share mount exists at /host, Save As... starts there; otherwise the log directory is the fallback. Prior session files are listed read-only.</source>
        <translation>Das vollständige Sitzungsregister; Save As... schreibt jeden Eintrag, auch wenn ein Filter Zeilen verbirgt. Wenn ein beschreibbares Systemfreigabe-Halter bei /host existiert, beginnt Save As... dort; ansonsten ist das Log-Verzeichnis das Fallback. Vorherige Sitzungsdateien werden schreibgeschützt aufgelistet.</translation>
    </message>
    <message>
        <source>Session logs</source>
        <translation>Sitzungsprotokolle</translation>
    </message>
    <message>
        <source>Session</source>
        <translation>Sitzung</translation>
    </message>
    <message>
        <source>The first entry is the live session; earlier files in the log directory are listed read-only below it.</source>
        <translation>Der erste Eintrag ist die Live-Sitzung; frühere Dateien im Log-Verzeichnis sind darunter nur lesbar aufgeführt.</translation>
    </message>
    <message>
        <source>New Session Log</source>
        <translation>Neues Session Log</translation>
    </message>
    <message>
        <source>Close the active session file; it becomes a prior session and the next log entry starts a new file.</source>
        <translation>Schließen Sie die aktive Sitzungsdatei; sie wird zu einer vorherigen Sitzung und der nächste Protokolleintrag startet eine neue Datei.</translation>
    </message>
    <message>
        <source>Add Note</source>
        <translation>Anmerkungen hinzufügen</translation>
    </message>
    <message>
        <source>Append a NOTE entry to the live session register.</source>
        <translation>Fügen Sie einen NOTE-Eintrag zum Live-Session-Register hinzu.</translation>
    </message>
    <message>
        <source>Delete</source>
        <translation>Löschen</translation>
    </message>
    <message>
        <source>Delete the selected prior session file (the live session is never deleted).</source>
        <translation>Löschen Sie die ausgewählte vorherige Sitzungsdatei (die Live-Sitzung wird niemals gelöscht).</translation>
    </message>
    <message>
        <source>Refresh</source>
        <translation>Aktualisieren</translation>
    </message>
    <message>
        <source>Save As...</source>
        <translation>Sparen Sie, wie...</translation>
    </message>
    <message>
        <source>Save the complete session log (all entries, not just the current filter).</source>
        <translation>Speichern Sie das vollständige Sitzungsprotokoll (alle Einträge, nicht nur den aktuellen Filter).</translation>
    </message>
    <message>
        <source>Clear Register</source>
        <translation>Register leeren</translation>
    </message>
    <message>
        <source>Clear the live register and view; prior session files are never modified.</source>
        <translation>Löschen Sie das Live-Register und die Ansicht; vorherige Sitzungsdateien werden nie geändert.</translation>
    </message>
    <message>
        <source>Search log:</source>
        <translation>Suchprotokoll:</translation>
    </message>
    <message>
        <source>Type any characters to show matching log entries (case-insensitive). Save As always writes every entry.</source>
        <translation>Geben Sie alle Zeichen ein, um übereinstimmende Protokolleinträge anzuzeigen (fallunempfindlich). Sparen Wie immer schreibt jeder Eintrag.</translation>
    </message>
    <message>
        <source>Filter:</source>
        <translation>Filter:</translation>
    </message>
    <message>
        <source>Filter the visible log by entry kind. Selecting a diagnostic section shows the lines captured for that section; a workflow filter such as File system repair or Package repair shows its mapped repair lines (File copy has no lines in this frontend). Save As always writes every entry.</source>
        <translation>Filtern Sie das sichtbare Protokoll nach Eintragsart. Das Auswählen eines Diagnoseabschnitts zeigt die für diesen Abschnitt erfassten Zeilen an; ein Workflowfilter wie Dateisystemreparatur oder Paketreparatur zeigt die zugeordneten Reparaturlinien an (Dateikopie hat keine Zeilen in diesem Frontend). Sparen Wie immer schreibt jeder Eintrag.</translation>
    </message>
    <message>
        <source>Settings are stored per user under ~/.qt/, one file per settings group (devicesrc, logsrc, diagnosticsrc, repairrc), and are saved immediately on every change and on close. Launch the GUI as the same user to keep your overrides; a GUI started as root keeps its own copies.</source>
        <translation>Die Einstellungen werden pro Benutzer unter ~/.qt/, einer Datei pro Einstellungsgruppe (devicesrc, logsrc, diagnosticsrc, repairrc) gespeichert und bei jeder Änderung und beim Schließen sofort gespeichert. Starten Sie die GUI als derselbe Benutzer, um Ihre Überschreibungen beizubehalten; eine GUI, die als root gestartet wurde, behält ihre eigenen Kopien.</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Geräteerkennung</translation>
    </message>
    <message>
        <source>Show devices without an identified Linux installation</source>
        <translation>Geräte ohne identifizierte Linux-Installation anzeigen</translation>
    </message>
    <message>
        <source>Show removable and USB storage</source>
        <translation>Wechselbar und USB-Speicher anzeigen</translation>
    </message>
    <message>
        <source>Show encrypted devices before unlocking</source>
        <translation>Verschlüsselte Geräte vor dem Entsperren anzeigen</translation>
    </message>
    <message>
        <source>When off, drives without a visible Linux filesystem are hidden unless they still contain an encrypted device and encrypted devices are shown.</source>
        <translation>Beim Ausschalten werden Laufwerke ohne sichtbares Linux-Dateisystem ausgeblendet, es sei denn, sie enthalten noch ein verschlüsseltes Gerät und verschlüsselte Geräte werden angezeigt.</translation>
    </message>
    <message>
        <source>When off, removable and USB drives are hidden from the repair-target list.</source>
        <translation>Wenn Sie ausgeschaltet sind, werden Wechsel- und USB-Laufwerke aus der Reparaturzielliste ausgeblendet.</translation>
    </message>
    <message>
        <source>When off, drives with an encrypted device are hidden until the volume is unlocked.</source>
        <translation>Wenn Sie ausgeschaltet sind, werden Laufwerke mit einem verschlüsselten Gerät ausgeblendet, bis das Volume entsperrt ist.</translation>
    </message>
    <message>
        <source>Include the %1 stage in the Full Repair plan. The stage runs in the plan order shown on the Repair tab.</source>
        <translation>Fügen Sie die %1-Stufe in den vollständigen Reparaturplan ein. Die Bühne läuft in der Planreihenfolge, die auf der Repair-Registerkarte angezeigt wird.</translation>
    </message>
    <message>
        <source>Automatically regenerate read-only diagnostics after repairs or target changes</source>
        <translation>Automatische Regeneration der Read-Only-Diagnose nach Reparaturen oder Zieländerungen</translation>
    </message>
    <message>
        <source>Regenerates the cached read-only diagnostics for the current scope after an operation that invalidates them (LUKS unlock, target configuration edit). It runs only inside an already authorized administrator session and never opens an authorization prompt by itself.</source>
        <translation>Regeneriert die zwischengespeicherte schreibgeschützte Diagnose für den aktuellen Bereich nach einer Operation, die sie ungültig macht (LUKS entsperren, Zielkonfiguration bearbeiten). Es läuft nur innerhalb einer bereits autorisierten Administratorsitzung und öffnet niemals eine Autorisierungsaufforderung von selbst.</translation>
    </message>
    <message>
        <source>Wrap long log lines</source>
        <translation>Lange Baumstämme</translation>
    </message>
    <message>
        <source>Mandatory safety controls</source>
        <translation>Obligatorische Sicherheitskontrollen</translation>
    </message>
    <message>
        <source>Host capabilities and dependencies</source>
        <translation>Hostfähigkeiten und Abhängigkeiten</translation>
    </message>
    <message>
        <source>Refresh Capabilities</source>
        <translation>Auffrischungsfunktionen</translation>
    </message>
    <message>
        <source>Re-run the read-only host capability probes (a PATH search, nothing is executed) and refresh the distribution summary.</source>
        <translation>Führen Sie die Read-Only-Host-Fähigkeits-Sonden erneut aus (eine PATH-Suche, nichts wird ausgeführt) und aktualisieren Sie die Verteilungszusammenfassung.</translation>
    </message>
    <message>
        <source>Install Missing Support...</source>
        <translation>Fehlende Unterstützung installieren...</translation>
    </message>
    <message>
        <source>Automatic installation will require explicit package mapping and privilege authorization.</source>
        <translation>Die automatische Installation erfordert eine explizite Paketzuordnung und Berechtigungsberechtigung.</translation>
    </message>
    <message>
        <source>Application configuration</source>
        <translation>Anwendungskonfiguration</translation>
    </message>
    <message>
        <source>Boot Bitch Legacy</source>
        <translation>Boot Bitch Vermächtnis</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list first.</source>
        <translation>Wählen Sie zuerst ein physisches Laufwerk in der Liste der verfügbaren Reparaturziele aus.</translation>
    </message>
    <message>
        <source>Protected system</source>
        <translation>Geschütztes System</translation>
    </message>
    <message>
        <source>The running system cannot be selected as a repair target. Use Host Maintenance for the protected running host or choose another disk.</source>
        <translation>Das laufende System kann nicht als Reparaturziel ausgewählt werden. Verwenden Sie Host Maintenance für den geschützten ausgeführten Host oder wählen Sie eine andere Festplatte aus.</translation>
    </message>
    <message>
        <source>Unlock or select a Linux system first</source>
        <translation>Entsperren oder wählen Sie zuerst ein Linux-System aus</translation>
    </message>
    <message>
        <source>This encrypted drive has no visible Linux filesystem yet. Use Unlock, refresh devices, and select the target after its Linux root is detected.</source>
        <translation>Dieses verschlüsselte Laufwerk hat noch kein sichtbares Linux-Dateisystem. Verwenden Sie Entsperren, aktualisieren Sie Geräte und wählen Sie das Ziel aus, nachdem die Linux-Root erkannt wurde.</translation>
    </message>
    <message>
        <source>The selected root component (%1) belongs to the running system and cannot be committed as a repair target. Use Host Maintenance for the protected running host.</source>
        <translation>Die ausgewählte Root-Komponente (%1) gehört zum laufenden System und kann nicht als Reparaturziel festgelegt werden. Verwenden Sie Host Maintenance für den geschützten laufenden Host.</translation>
    </message>
    <message>
        <source>repair target commit</source>
        <translation>Reparaturzielbindung</translation>
    </message>
    <message>
        <source>Repair drive selected: %1</source>
        <translation>Reparaturantrieb ausgewählt: %1</translation>
    </message>
    <message>
        <source>; best detected system component: %1</source>
        <translation>; am besten erkannte Systemkomponente: %1</translation>
    </message>
    <message>
        <source>. No mount or repair action was performed.</source>
        <translation>Es wurde keine Montage- oder Reparaturaktion durchgeführt.</translation>
    </message>
    <message>
        <source>Make Default unavailable</source>
        <translation>Als Standard festlegen nicht verfügbar</translation>
    </message>
    <message>
        <source>Make Default is a running-host action on this frontend; enter Host Maintenance first.</source>
        <translation>Make Default ist eine Running-Host-Aktion auf diesem Frontend; geben Sie zuerst Host Maintenance ein.</translation>
    </message>
    <message>
        <source>Administrator authorization required</source>
        <translation>Administratorautorisierung erforderlich</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize first.</source>
        <translation>Keine Administratorsitzung ist aktiv; drücken Sie zuerst Autorisieren.</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host?

The helper verifies /boot/grub/menu.lst, sets the `default &lt;N&gt;` directive to the canonical entry, backs the menu up first and restores it on any failure.</source>
        <translation>Machen Sie den kanonisch installierten Kerneleintrag zum standardmäßigen GRUB-Legacy-Booteintrag auf dem laufenden Host?

Der Helfer überprüft /boot/grub/menu.lst, setzt die `default &lt;N&gt;`-Direktive auf den kanonischen Eintrag, sichert das Menü zuerst und stellt es bei jedem Fehler wieder her.</translation>
    </message>
    <message>
        <source>Host Maintenance unavailable</source>
        <translation>Host-Wartung nicht verfügbar</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a committed repair target or a detected running host.</source>
        <translation>Das laufende Host-Ziel konnte nicht erkannt werden; Diagnosen erfordern ein festgelegtes Reparaturziel oder einen erkannten laufenden Host.</translation>
    </message>
    <message>
        <source>Exit Host Maintenance</source>
        <translation>Host-Wartung beenden</translation>
    </message>
    <message>
        <source>Host Maintenance is active; diagnostics and gated repairs target the protected running host.</source>
        <translation>Host Maintenance ist aktiv; Diagnose und Gated-Reparaturen zielen auf den geschützten laufenden Host ab.</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list on the Systems tab first, or use Host Maintenance for the protected running host.</source>
        <translation>Wählen Sie zuerst ein physisches Laufwerk in der Liste Verfügbare Reparaturziele auf der Registerkarte Systeme aus oder verwenden Sie die Hostwartung für den geschützten ausgeführten Host.</translation>
    </message>
    <message>
        <source>The selected drive (%1) is the protected running host. Choose Host Maintenance on the Systems tab to run read-only host diagnostics and guarded host repairs; ordinary target repairs stay disabled.</source>
        <translation>Das ausgewählte Laufwerk (%1) ist der geschützte laufende Host. Wählen Sie Host Maintenance auf der Registerkarte Systeme, um schreibgeschützte Hostdiagnosen und bewachte Hostreparaturen auszuführen; gewöhnliche Zielreparaturen bleiben deaktiviert.</translation>
    </message>
    <message>
        <source>No repair target is committed. Choose Select Target on the Systems tab (or Host Maintenance for the protected running host) first.</source>
        <translation>Es wird kein Reparaturziel festgelegt. Wählen Sie zuerst Ziel auf der Registerkarte Systeme (oder Hostwartung für den geschützten ausgeführten Host) aus.</translation>
    </message>
    <message>
        <source>The selection changed after the target was committed. Choose Select Target again.</source>
        <translation>Die Auswahl änderte sich, nachdem das Ziel festgelegt wurde. Wählen Sie erneut Ziel auswählen.</translation>
    </message>
    <message>
        <source>Diagnostics scope required</source>
        <translation>Diagnosebereich erforderlich</translation>
    </message>
    <message>
        <source>Diagnostics scope unresolved</source>
        <translation>Diagnosebereich nicht aufgelöst</translation>
    </message>
    <message>
        <source>The running host target could not be resolved; use Refresh Devices and commit a repair target or re-enter Host Maintenance.</source>
        <translation>Das laufende Host-Ziel konnte nicht gelöst werden; Verwenden Sie Refresh-Geräte und begehen Sie ein Reparaturziel oder geben Sie die Host-Wartung erneut ein.</translation>
    </message>
    <message>
        <source>Diagnostic check required</source>
        <translation>Diagnoseprüfung erforderlich</translation>
    </message>
    <message>
        <source>Select a diagnostic check in the list first.</source>
        <translation>Wählen Sie zuerst eine Diagnoseprüfung in der Liste aus.</translation>
    </message>
    <message>
        <source>Repair target required</source>
        <translation>Reparaturziel erforderlich</translation>
    </message>
    <message>
        <source>Target file editing needs a committed repair target. Running-host maintenance has no target-file editing; commit an offline target first.</source>
        <translation>Die Bearbeitung von Zieldateien erfordert ein festgelegtes Reparaturziel. Running-Host-Wartung hat keine Zieldateibearbeitung; Begehen Sie zuerst ein Offline-Ziel.</translation>
    </message>
    <message>
        <source>Configuration file required</source>
        <translation>Konfigurationsdatei erforderlich</translation>
    </message>
    <message>
        <source>Select a target configuration file first.</source>
        <translation>Wählen Sie zuerst eine Zielkonfigurationsdatei aus.</translation>
    </message>
    <message>
        <source>Edit target %1</source>
        <translation>Ziel %1 bearbeiten</translation>
    </message>
    <message>
        <source>Edit this target file through the guarded administrator helper. A successful save invalidates cached diagnostics; rerun diagnostics before repair. Generated files such as /boot/grub/menu.lst may be replaced by the next bootloader update.</source>
        <translation>Bearbeiten Sie diese Zieldatei über den geschützten Administratorhelfer. Ein erfolgreiches Speichern macht die zwischengespeicherte Diagnose ungültig; Diagnose vor der Reparatur wiederholen. Generierte Dateien wie /boot/grub/menu.lst können durch das nächste Bootloader-Update ersetzt werden.</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Stornierung</translation>
    </message>
    <message>
        <source>Save Target File</source>
        <translation>Zieldatei speichern</translation>
    </message>
    <message>
        <source>No changes to %1.</source>
        <translation>Keine Änderungen an %1.</translation>
    </message>
    <message>
        <source>Configuration write refused</source>
        <translation>Konfigurationsschreiben abgelehnt</translation>
    </message>
    <message>
        <source>The edited content contains NUL bytes; the guarded write refuses it.</source>
        <translation>Der bearbeitete Inhalt enthält NUL-Bytes; der bewachte Schreiber weigert sich.</translation>
    </message>
    <message>
        <source>File too large</source>
        <translation>Datei zu groß</translation>
    </message>
    <message>
        <source>The edited file is larger than 1 MiB. The guarded write refuses it; edit the file from a console instead.</source>
        <translation>Die bearbeitete Datei ist größer als 1 MiB. Der bewachte Schreiber lehnt es ab; Bearbeiten Sie stattdessen die Datei von einer Konsole.</translation>
    </message>
    <message>
        <source>Write target configuration</source>
        <translation>Schreibzielkonfiguration</translation>
    </message>
    <message>
        <source>Write the edited contents to %1? This modifies the repair target and invalidates cached diagnostics.</source>
        <translation>Schreiben Sie die bearbeiteten Inhalte auf %1? Dadurch wird das Reparaturziel geändert und die zwischengespeicherte Diagnose ungültig gemacht.</translation>
    </message>
    <message>
        <source>No diagnostic results to copy yet.</source>
        <translation>Noch keine diagnostischen Ergebnisse zu kopieren.</translation>
    </message>
    <message>
        <source>Diagnostic results copied to the clipboard.</source>
        <translation>Diagnoseergebnisse kopiert in die Zwischenablage.</translation>
    </message>
    <message>
        <source>No diagnostic results to save yet.</source>
        <translation>Noch sind keine diagnostischen Ergebnisse zu retten.</translation>
    </message>
    <message>
        <source>Text files (*.txt);;All files (*)</source>
        <translation>Textdateien (*.txt);;Alle Dateien (*)</translation>
    </message>
    <message>
        <source>Save diagnostic results</source>
        <translation>Diagnoseergebnisse speichern</translation>
    </message>
    <message>
        <source>Could not write %1.</source>
        <translation>Ich konnte %1 nicht schreiben.</translation>
    </message>
    <message>
        <source>Diagnostic results saved to %1</source>
        <translation>Diagnoseergebnisse auf %1 gespeichert</translation>
    </message>
    <message>
        <source>Unlock not available</source>
        <translation>Entsperren nicht verfügbar</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked. Select an offline repair target to unlock.</source>
        <translation>Der geschützte laufende Host kann nicht freigeschaltet werden. Wählen Sie ein Offline-Reparaturziel zum Entsperren.</translation>
    </message>
    <message>
        <source>No locked LUKS component is currently visible on this selected drive.</source>
        <translation>Derzeit ist keine gesperrte LUKS-Komponente auf diesem ausgewählten Laufwerk sichtbar.</translation>
    </message>
    <message>
        <source>Confirm LUKS unlock</source>
        <translation>LUKS Entsperrung bestätigen</translation>
    </message>
    <message>
        <source>Unlock %1 on %2?

The helper opens a temporary device-mapper mapping with cryptsetup and keeps it open for this recovery session. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>Entsperren Sie %1 auf %2?

Der Helfer öffnet ein temporäres Geräte-Mapper-Mapping mit cryptsetup und hält es für diese Wiederherstellungssitzung offen. Die Passphrase durchläuft eine private Schlüsseldatei und wird niemals in Befehlsargumenten oder Protokollen platziert.</translation>
    </message>
    <message>
        <source>LUKS unlock</source>
        <translation>LUKS Entsperren</translation>
    </message>
    <message>
        <source>Unlock LUKS repair target</source>
        <translation>LUKS Reparaturziel freischalten</translation>
    </message>
    <message>
        <source>Enter the passphrase for %1.

It is sent only to cryptsetup over the helper&apos;s standard input and is never logged or placed on a command line.</source>
        <translation>Geben Sie die Passphrase für %1 ein.

Es wird nur über die Standardeingabe des Helfers an cryptsetup gesendet und niemals protokolliert oder in einer Befehlszeile platziert.</translation>
    </message>
    <message>
        <source>Passphrase required</source>
        <translation>Passphrase erforderlich</translation>
    </message>
    <message>
        <source>An empty passphrase was not submitted. Enter the LUKS passphrase or choose Cancel.</source>
        <translation>Eine leere Passphrase wurde nicht eingereicht. Geben Sie die LUKS-Passphrase ein oder wählen Sie Abbrechen.</translation>
    </message>
    <message>
        <source>Unlock keyfile unavailable</source>
        <translation>Entsperren Keyfile nicht verfügbar</translation>
    </message>
    <message>
        <source>The LUKS passphrase could not be written to a private keyfile in %1; the unlock was not started.</source>
        <translation>Die LUKS-Passphrase konnte nicht in eine private Schlüsseldatei in %1 geschrieben werden; das Entsperren wurde nicht gestartet.</translation>
    </message>
    <message>
        <source>Configuration write unavailable</source>
        <translation>Configuration Write nicht verfügbar</translation>
    </message>
    <message>
        <source>The edited content could not be written to a private temporary file in %1; the write was not started.</source>
        <translation>Der bearbeitete Inhalt konnte nicht in eine private temporäre Datei in %1 geschrieben werden; das Schreiben wurde nicht gestartet.</translation>
    </message>
    <message>
        <source>ERROR: cannot read the session log %1</source>
        <translation>FEHLER: kann das Sitzungsprotokoll %1 nicht lesen</translation>
    </message>
    <message>
        <source>Viewing a prior session log (read-only): %1</source>
        <translation>Anzeigen eines vorherigen Sitzungsprotokolls (nur lesen): %1</translation>
    </message>
    <message>
        <source>Session log list refreshed.</source>
        <translation>Sitzungsprotokollliste aktualisiert.</translation>
    </message>
    <message>
        <source>Note:</source>
        <translation>Anmerkung:</translation>
    </message>
    <message>
        <source>Delete session log</source>
        <translation>Sitzungsprotokoll löschen</translation>
    </message>
    <message>
        <source>Delete %1 permanently? This cannot be undone.</source>
        <translation>%1 dauerhaft löschen? Das kann nicht rückgängig gemacht werden.</translation>
    </message>
    <message>
        <source>%1 changed while the confirmation was open; the delete was refused.</source>
        <translation>%1 änderte sich, während die Bestätigung offen war; das Löschen wurde abgelehnt.</translation>
    </message>
    <message>
        <source>Unable to delete %1.</source>
        <translation>Kann %1 nicht löschen.</translation>
    </message>
    <message>
        <source>Device discovery filters updated.</source>
        <translation>Geräteerkennungsfilter aktualisiert.</translation>
    </message>
    <message>
        <source>Unknown Linux distribution</source>
        <translation>Unbekannte Linux-Distribution</translation>
    </message>
    <message>
        <source>sudo / gksu (no KAuth on this frontend)</source>
        <translation>sudo / gksu (kein KAuth auf diesem Frontend)</translation>
    </message>
    <message>
        <source>Available</source>
        <translation>verfügbar</translation>
    </message>
    <message>
        <source>Missing on this frontend</source>
        <translation>Fehlendes Frontend</translation>
    </message>
    <message>
        <source>Missing</source>
        <translation>Fehlend</translation>
    </message>
    <message>
        <source>No repair tool is selected.</source>
        <translation>Kein Reparaturwerkzeug ausgewählt.</translation>
    </message>
    <message>
        <source>A helper command is already running.</source>
        <translation>Ein Helferbefehl läuft bereits.</translation>
    </message>
    <message>
        <source>Restore Graphical Login is a host-scope stage on this legacy frontend; enter Host Maintenance to run it.</source>
        <translation>Wiederherstellen Graphical Login ist eine Host-Scope-Phase auf diesem Legacy-Frontend; geben Sie Host Maintenance ein, um es auszuführen.</translation>
    </message>
    <message>
        <source>The selected scope has no resolved root component; use Refresh Devices and commit the repair target again.</source>
        <translation>Der ausgewählte Scope hat keine aufgelöste Root-Komponente; Verwenden Sie Refresh Devices und verpflichten Sie das Reparaturziel erneut.</translation>
    </message>
    <message>
        <source>This legacy frontend exposes no %1 action; the helper reports the capability as available.</source>
        <translation>Dieses Legacy-Frontend zeigt keine %1-Aktion; der Helfer meldet die Fähigkeit als verfügbar.</translation>
    </message>
    <message>
        <source>Run this guarded repair action using the cached read-only diagnostic evidence. A confirmation is shown first.</source>
        <translation>Führen Sie diese geschützte Reparaturaktion mit dem zwischengespeicherten schreibgeschützten Diagnosenachweis aus. Eine Bestätigung wird zuerst angezeigt.</translation>
    </message>
    <message>
        <source>Enabled in Settings</source>
        <translation>In Einstellungen aktiviert</translation>
    </message>
    <message>
        <source>Disabled in Settings - enable it to include this stage</source>
        <translation>Deaktiviert in Einstellungen - Aktivieren Sie diese Phase</translation>
    </message>
    <message>
        <source>Unavailable: %1</source>
        <translation>Nicht verfügbar: %1</translation>
    </message>
    <message>
        <source>
Unavailable: %1</source>
        <translation>
Nicht verfügbar: %1</translation>
    </message>
    <message>
        <source>Repair tool unavailable</source>
        <translation>Reparaturwerkzeug nicht verfügbar</translation>
    </message>
    <message>
        <source>Confirm repair</source>
        <translation>Reparatur bestätigen</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Your saved selection is kept and its availability is re-checked when diagnostics for this scope complete.</source>
        <translation>Es gibt noch keine zwischengespeicherten Fähigkeiten für diese Phase. Ihre gespeicherte Auswahl wird beibehalten und ihre Verfügbarkeit wird erneut überprüft, wenn die Diagnose für diesen Bereich abgeschlossen ist.</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Run diagnostics for the selected scope to populate the Full Repair plan; your selection is saved once the stage becomes available.</source>
        <translation>Es gibt noch keine zwischengespeicherten Fähigkeiten für diese Phase. Führen Sie die Diagnose für den ausgewählten Bereich aus, um den vollständigen Reparaturplan zu füllen; Ihre Auswahl wird gespeichert, sobald die Bühne verfügbar ist.</translation>
    </message>
    <message>
        <source>Unknown Full Repair stage.</source>
        <translation>Unbekannte Phase der vollständigen Reparatur.</translation>
    </message>
    <message>
        <source>No Full Repair stages are selected or available; use Configure Plan... to choose the stages.</source>
        <translation>Es sind keine vollständigen Reparaturphasen ausgewählt oder verfügbar; Verwenden Sie Configure Plan ..., um die Phasen auszuwählen.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab to establish the session.</source>
        <translation>Administratorautorisierung ist erforderlich; drücken Sie auf der Registerkarte Systeme oder Reparatur autorisieren, um die Sitzung einzurichten.</translation>
    </message>
    <message>
        <source>Runs the selected stages using the cached read-only diagnostic evidence after privilege confirmation.</source>
        <translation>Führt die ausgewählten Phasen mit dem zwischengespeicherten schreibgeschützten Diagnosenachweis nach der Berechtigungsbestätigung aus.</translation>
    </message>
    <message>
        <source>No Full Repair stages selected - use Configure Plan... or Settings.</source>
        <translation>Keine vollständigen Reparaturphasen ausgewählt - verwenden Sie Configure Plan ... oder Einstellungen.</translation>
    </message>
    <message>
        <source>1 stage selected</source>
        <translation>1 Stufe ausgewählt</translation>
    </message>
    <message>
        <source>%1 stages selected</source>
        <translation>%1 Stufen ausgewählt</translation>
    </message>
    <message>
        <source>No repair stages are selected. Use Configure Plan... to choose the stages Full Repair will run.</source>
        <translation>Es werden keine Reparaturstufen ausgewählt. Verwenden Sie Configure Plan ..., um die Phasen Vollständige Reparatur auszuwählen.</translation>
    </message>
    <message>
        <source>No selected stages are available. %1</source>
        <translation>Es stehen keine ausgewählten Stufen zur Verfügung. %1</translation>
    </message>
    <message>
        <source>Full Repair unavailable</source>
        <translation>Vollständige Reparatur nicht verfügbar</translation>
    </message>
    <message>
        <source>Run the Full Repair plan?

The selected stages run in order through the helper&apos;s guarded repair command:

</source>
        <translation>Führen Sie den vollständigen Reparaturplan aus?

Die ausgewählten Stufen laufen in der Reihenfolge durch den bewachten Reparaturbefehl des Helfers:

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
Der Helfer hält jeden Laufzeit-Preflight; eine Phase, die fehlschlägt, stoppt den Plan.</translation>
    </message>
    <message>
        <source>Running %1 through the privileged helper... The Logs tab keeps the complete transcript.</source>
        <translation>Laufen %1 durch den privilegierten Helfer... Die Registerkarte Logs behält das vollständige Transkript.</translation>
    </message>
    <message>
        <source>Shell scope required</source>
        <translation>Shell Umfang erforderlich</translation>
    </message>
    <message>
        <source>No administrator session is active.

Press Authorize on the Systems or Repair tab to establish the session, or enter Host Maintenance / commit a repair target on the Systems tab; the chroot shell then reuses the cached authorization.</source>
        <translation>Keine Administratorsitzung ist aktiv.

Drücken Sie auf der Registerkarte Systeme oder Reparatur autorisieren, um die Sitzung einzurichten, oder geben Sie die Host-Wartung / ein Reparaturziel auf der Registerkarte Systeme ein; die chroot-Shell verwendet dann die zwischengespeicherte Autorisierung wieder.</translation>
    </message>
    <message>
        <source>Shell command required</source>
        <translation>Shell Befehl erforderlich</translation>
    </message>
    <message>
        <source>Enter the command to run first.</source>
        <translation>Geben Sie den Befehl ein, zuerst zu laufen.</translation>
    </message>
    <message>
        <source>Confirm running-host command</source>
        <translation>Bestätigen Sie das Running-Host Kommando</translation>
    </message>
    <message>
        <source>Run this command as root on the protected running host?

%1

The helper keeps its runtime preflights; the command is passed as one argument and is never interpreted by the GUI.</source>
        <translation>Führen Sie diesen Befehl als root auf dem geschützten ausgeführten Host aus?

%1

Der Helfer behält seine Laufzeit-Preflights; der Befehl wird als ein Argument übergeben und niemals von der GUI interpretiert.</translation>
    </message>
    <message>
        <source>Running %1...</source>
        <translation>Ausführen von %1...</translation>
    </message>
    <message>
        <source>Authorization scope required</source>
        <translation>Erforderlicher Genehmigungsumfang</translation>
    </message>
    <message>
        <source>administrator authorization</source>
        <translation>Administratorautorisierung</translation>
    </message>
    <message>
        <source>Administrator authorization</source>
        <translation>Administratorautorisierung</translation>
    </message>
    <message>
        <source>Administrator authorization is required for %1.

Enter the password for %2 (sudo). It is used only for this sudo authentication, is sent over a pipe and is never logged or placed on a command line. The authorization is cached for this session and reused by diagnostics and repairs.</source>
        <translation>Die Administratorautorisierung ist für %1 erforderlich.

Geben Sie das Passwort für %2 (sudo) ein. Es wird nur für diese sudo-authentifizierung verwendet, wird über ein pipe gesendet und niemals protokolliert oder auf einer befehlszeile platziert. Die Autorisierung wird für diese Sitzung zwischengespeichert und durch Diagnosen und Reparaturen wiederverwendet.</translation>
    </message>
    <message>
        <source>your account</source>
        <translation>Dein Konto</translation>
    </message>
    <message>
        <source>Password required</source>
        <translation>Passwort erforderlich</translation>
    </message>
    <message>
        <source>An empty password was not submitted. Enter the sudo password or choose Cancel.</source>
        <translation>Ein leeres Passwort wurde nicht übermittelt. Geben Sie das sudo-Passwort ein oder wählen Sie Abbrechen.</translation>
    </message>
    <message>
        <source>Administrator authorization failed</source>
        <translation>Administratorautorisierung fehlgeschlagen</translation>
    </message>
    <message>
        <source>sudo did not accept the password: %1

The command was not started.</source>
        <translation>sudo akzeptierte das Passwort nicht: %1

Der Befehl wurde nicht gestartet.</translation>
    </message>
    <message>
        <source>elevation requires an interactive sudo password; run the smoke as root or after `sudo -S -v` with --elevate &apos;sudo -n&apos;</source>
        <translation>Erhöhen erfordert ein interaktives sudo-Passwort; führen Sie den Rauch als root oder nach &apos;sudo -S -v&apos; mit --erhöhen &apos;sudo -n&apos;</translation>
    </message>
    <message>
        <source>No administrator session is active for %1.

Press Authorize on the Systems or Repair tab to establish the session now, or enter Host Maintenance / commit a repair target on the Systems tab; diagnostics and repairs then reuse the cached authorization.</source>
        <translation>Keine Administratorsitzung ist für %1 aktiv.

Drücken Sie auf der Registerkarte Systeme oder Reparatur Autorisieren, um die Sitzung jetzt einzurichten, oder geben Sie die Host-Wartung / ein Reparaturziel auf der Registerkarte Systeme ein; Diagnose und Reparaturen verwenden dann die zwischengespeicherte Autorisierung erneut.</translation>
    </message>
    <message>
        <source>Administrator authorization expired</source>
        <translation>Administratorautorisierung abgelaufen</translation>
    </message>
    <message>
        <source>The cached administrator authorization for %1 expired or was refused.

Press Authorize on the Systems or Repair tab to re-establish the session, then run the command again. No command was started.</source>
        <translation>Die zwischengespeicherte Administratorautorisierung für %1 ist abgelaufen oder wurde abgelehnt.

Drücken Sie auf der Registerkarte Systeme oder Reparatur autorisieren, um die Sitzung neu einzurichten, und führen Sie den Befehl erneut aus. Es wurde kein Befehl gestartet.</translation>
    </message>
    <message>
        <source>Privileged operation completed successfully. Administrator authorization remains active for this session.</source>
        <translation>Privilegierter Betrieb erfolgreich abgeschlossen. Die Administratorautorisierung bleibt für diese Sitzung aktiv.</translation>
    </message>
    <message>
        <source>Privileged operation stopped with an error. Administrator authorization remains active; review the output before closing.</source>
        <translation>Privilegierte Operation wurde mit einem Fehler gestoppt. Die Administratorautorisierung bleibt aktiv; überprüfen Sie die Ausgabe vor dem Schließen.</translation>
    </message>
    <message>
        <source>Inspection is read-only.

</source>
        <translation>Die Inspektion ist read-only.

</translation>
    </message>
    <message>
        <source>Configuration unavailable</source>
        <translation>Konfiguration nicht verfügbar</translation>
    </message>
    <message>
        <source>The helper could not read %1:

%2</source>
        <translation>Der Helfer konnte %1 nicht lesen:

%2</translation>
    </message>
    <message>
        <source>Configuration write failed</source>
        <translation>Configuration Write fehlgeschlagen</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
Result: mapping opened for this recovery session.</source>
        <translation>Staat: entsperrt
Komponente: %1
Mapper: %2
Methode: Helfer entsperren (Cryptsetup; Passphrase über eine Mode-600-Keyfile, nach Gebrauch gelöscht)
Ergebnis: Das Mapping wurde für diese Wiederherstellungssitzung geöffnet.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: the passphrase was not accepted; retry offered.</source>
        <translation>Zustand: gesperrt
Komponente: %1
Methode: Helfer entsperren (Cryptsetup)
Fehler: Die Passphrase wurde nicht akzeptiert; Wiederholung angeboten.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: %2</source>
        <translation>Zustand: gesperrt
Komponente: %1
Methode: Helfer entsperren (Cryptsetup)
Fehler: %2</translation>
    </message>
    <message>
        <source>Passphrase not accepted</source>
        <translation>Passphrase nicht akzeptiert</translation>
    </message>
    <message>
        <source>The LUKS passphrase was not accepted.

Try again?</source>
        <translation>Die LUKS-Passphrase wurde nicht akzeptiert.

Versuchen Sie es noch einmal?</translation>
    </message>
    <message>
        <source>[-] %1 - not run (the plan stopped before reaching this stage)</source>
        <translation>[-] %1 - nicht laufen (der Plan gestoppt, bevor diese Phase zu erreichen)</translation>
    </message>
    <message>
        <source>filesystem</source>
        <translation>Dateisystem</translation>
    </message>
    <message>
        <source>[-] %1 - no file system errors found - no changes</source>
        <translation>[-] %1 - keine Dateisystemfehler gefunden - keine Änderungen</translation>
    </message>
    <message>
        <source>[FAIL] %1 - not reported</source>
        <translation>[FAIL] %1 - nicht gemeldet</translation>
    </message>
    <message>
        <source>%1 stage(s) failed; review the helper output in Logs.</source>
        <translation>%1-Stufe(n) ist fehlgeschlagen; Überprüfen Sie die Helferausgabe in Logs.</translation>
    </message>
    <message>
        <source>the plan stopped before any stage completed.</source>
        <translation>Der Plan wurde gestoppt, bevor eine Phase abgeschlossen wurde.</translation>
    </message>
    <message>
        <source>no repair was needed; cached diagnostics remain valid.</source>
        <translation>Es war keine Reparatur erforderlich; die zwischengespeicherte Diagnose bleibt gültig.</translation>
    </message>
    <message>
        <source>repair completed; cached diagnostics were invalidated and must be regenerated.</source>
        <translation>Reparatur abgeschlossen; zwischengespeicherte Diagnosen wurden ungültig gemacht und müssen regeneriert werden.</translation>
    </message>
    <message>
        <source>%1 results: [OK] %2 successful | [FAIL] %3 failed | [-] %4 no repair needed - %5</source>
        <translation>%1 Ergebnisse: [OK] %2 erfolgreich | [FAIL] %3 fehlgeschlagen | [-] %4 keine Reparatur erforderlich - %5</translation>
    </message>
    <message>
        <source>Re-run Diagnostic</source>
        <translation>Diagnose erneut ausführen</translation>
    </message>
    <message>
        <source>Unavailable</source>
        <translation>Nicht verfügbar</translation>
    </message>
    <message>
        <source>Commit this physical drive as the repair target.</source>
        <translation>Begehen Sie diesen physischen Antrieb als Reparaturziel.</translation>
    </message>
    <message>
        <source>Protection:</source>
        <translation>Schutz:</translation>
    </message>
    <message>
        <source>Host shell</source>
        <translation>Host-Shell</translation>
    </message>
    <message>
        <source>Run on Host</source>
        <translation>Auf Host ausführen</translation>
    </message>
    <message>
        <source>Host Shell</source>
        <translation>Host-Shell</translation>
    </message>
    <message>
        <source>The read-only probe found no editable target configuration file in this target. %1</source>
        <translation>Die schreibgeschützte Sonde fand keine editierbare Zielkonfigurationsdatei in diesem Ziel. %1</translation>
    </message>
    <message>
        <source>Not present in the selected target (omitted): %1</source>
        <translation>Nicht im ausgewählten Ziel vorhanden (ausgelassen): %1</translation>
    </message>
    <message>
        <source>Run diagnostics to probe which target configuration files exist; the helper&apos;s read-only probe decides the list.</source>
        <translation>Führen Sie Diagnosen aus, um zu untersuchen, welche Zielkonfigurationsdateien vorhanden sind; die schreibgeschützte Sonde des Helfers entscheidet über die Liste.</translation>
    </message>
    <message>
        <source>Probed read-only by the helper; a saved edit invalidates the cached diagnostics.</source>
        <translation>Probed read-only durch den Helfer; eine gespeicherte Bearbeitung ungültig macht die zwischengespeicherte Diagnose.</translation>
    </message>
    <message>
        <source>Host maintenance: %1</source>
        <translation>Host-Wartung: %1</translation>
    </message>
    <message>
        <source>unresolved</source>
        <translation>ungelöst</translation>
    </message>
    <message>
        <source>Target: %1 + %2</source>
        <translation>Ziel: %1 + %2</translation>
    </message>
    <message>
        <source>Host command</source>
        <translation>Befehlshaber</translation>
    </message>
    <message>
        <source>Execute a command on the running host as root.</source>
        <translation>Führen Sie einen Befehl auf dem laufenden Host als root aus.</translation>
    </message>
    <message>
        <source>Execute a command inside the selected repair system as root.</source>
        <translation>Führen Sie einen Befehl innerhalb des ausgewählten Reparatursystems als root aus.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab (or re-enter Host Maintenance / re-commit the repair target) to authorize this session.</source>
        <translation>Administratorautorisierung ist erforderlich; drücken Sie auf der Registerkarte Systeme oder Reparatur Autorisieren (oder geben Sie die Hostwartung erneut ein / übertragen Sie das Reparaturziel erneut), um diese Sitzung zu autorisieren.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root on the running host through the helper&apos;s guarded host-shell verb.</source>
        <translation>Führen Sie einen überprüften Befehl als root auf dem laufenden Host durch das bewachte Host-Shell-Verb des Helfers aus.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root inside the target chroot through the helper&apos;s guarded shell verb.</source>
        <translation>Führen Sie einen überprüften Befehl als root innerhalb der Ziel-Chroot durch das geschützte Shell-Verb des Helfers aus.</translation>
    </message>
    <message>
        <source>Run a command on the running host as root (sudo is not needed). Commands are executed directly on the active system; output is kept in this window and in the application log.</source>
        <translation>Führen Sie einen Befehl auf dem laufenden Host als root aus (sudo wird nicht benötigt). Befehle werden direkt auf dem aktiven System ausgeführt; die Ausgabe wird in diesem Fenster und im Anwendungsprotokoll gespeichert.</translation>
    </message>
    <message>
        <source>Run a command inside the selected repair system as root (sudo is not needed). Commands are executed one at a time in a fresh chroot and cannot answer interactive prompts; use non-interactive flags such as apt-get -y upgrade. Output is kept in this window and in the application log.</source>
        <translation>Führen Sie einen Befehl innerhalb des ausgewählten Reparatursystems als root aus (sudo wird nicht benötigt). Befehle werden einzeln in einem neuen chroot ausgeführt und können keine interaktiven Eingabeaufforderungen beantworten; verwenden Sie nicht-interaktive Flags wie apt-get-y Upgrade. Die Ausgabe wird in diesem Fenster und im Anwendungsprotokoll gespeichert.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize or re-enter Host Maintenance / commit a repair target.</source>
        <translation>Keine Administratorsitzung ist aktiv; Drücken Sie Autorisieren oder erneut eingeben Host Maintenance / ein Reparaturziel festlegen.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from the repaired system</source>
        <translation>1. Wählen Sie Quelldateien oder Ordner aus dem reparierten System</translation>
    </message>
    <message>
        <source>2. Choose destination on this host</source>
        <translation>2. Wählen Sie das Ziel auf diesem Host</translation>
    </message>
    <message>
        <source>Add File Path...</source>
        <translation>File Path hinzufügen...</translation>
    </message>
    <message>
        <source>Add Folder Path...</source>
        <translation>Folder Path hinzufügen...</translation>
    </message>
    <message>
        <source>Browse...</source>
        <translation>Durchsuchen...</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it (Host Maintenance does not provide a repair tree to browse).</source>
        <translation>Wählen Sie die Reparaturfahrt in Systems aus, bevor Sie ein Ziel darin auswählen (Host Maintenance bietet keinen Reparaturbaum zum Durchsuchen).</translation>
    </message>
    <message>
        <source>Choose a host destination folder directly.</source>
        <translation>Wählen Sie einen Hostzielordner direkt aus.</translation>
    </message>
    <message>
        <source>Copy the staged files and folders with cp -a, restore ownership with chown --reference and byte-compare every regular file afterwards.</source>
        <translation>Kopieren Sie die inszenierten Dateien und Ordner mit cp -a, stellen Sie das Eigentum mit chown --reference wieder her und vergleichen Sie anschließend jede reguläre Datei.</translation>
    </message>
    <message>
        <source>File Copy unavailable</source>
        <translation>Dateikopie nicht verfügbar</translation>
    </message>
    <message>
        <source>Choose host destination folder</source>
        <translation>Hostzielordner auswählen</translation>
    </message>
    <message>
        <source>Select a repair target</source>
        <translation>Reparaturziel auswählen</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it.</source>
        <translation>Wählen Sie die Reparaturfahrt in Systems aus, bevor Sie ein Ziel darin auswählen.</translation>
    </message>
    <message>
        <source>File Copy - Browse Target Folders</source>
        <translation>File Copy - Zielordner durchsuchen</translation>
    </message>
    <message>
        <source>Browse Target Folders</source>
        <translation>Zielordner durchsuchen</translation>
    </message>
    <message>
        <source>The helper could not list the repair-system folder:

%1</source>
        <translation>Der Helfer konnte den Reparatursystemordner nicht auflisten:

%1</translation>
    </message>
    <message>
        <source>BROWSE_ENTRY</source>
        <translation>BROWSE EINTRAG</translation>
    </message>
    <message>
        <source>(choose this folder: %1)</source>
        <translation>(Wählen Sie diesen Ordner: %1)</translation>
    </message>
    <message>
        <source>(parent folder)</source>
        <translation>(Zwischenordner)</translation>
    </message>
    <message>
        <source>Select repair-system destination</source>
        <translation>Bestimmungsort des Reparatursystems auswählen</translation>
    </message>
    <message>
        <source>Choose a destination folder inside the repaired system (current folder: %1):</source>
        <translation>Wählen Sie einen Zielordner innerhalb des reparierten Systems (aktueller Ordner: %1):</translation>
    </message>
    <message>
        <source>Folder</source>
        <translation>Ordner</translation>
    </message>
    <message>
        <source>Open</source>
        <translation>Offen</translation>
    </message>
    <message>
        <source>(choose this folder:</source>
        <translation>(Wählen Sie diesen Ordner:</translation>
    </message>
    <message>
        <source>Add files to copy</source>
        <translation>Dateien zum Kopieren hinzufügen</translation>
    </message>
    <message>
        <source>Add folder to copy</source>
        <translation>Ordner zum Kopieren hinzufügen</translation>
    </message>
    <message>
        <source>Stage at least one source and name a destination path first.</source>
        <translation>Führen Sie mindestens eine Quelle durch und benennen Sie zuerst einen Zielpfad.</translation>
    </message>
    <message>
        <source>Copy the %1 staged item(s) to %2?

The helper keeps its direction and path containment checks; a sensitive repair-system destination is refused unless the helper approves it, and every regular file is byte-compared after the copy.</source>
        <translation>Kopieren Sie die %1-Stufe(n) in %2?

Der Helfer behält seine Richtungs- und Pfadeindämmungsüberprüfungen bei; ein empfindliches Reparatursystem wird abgelehnt, es sei denn, der Helfer genehmigt es, und jede reguläre Datei wird nach der Kopie byteverglichen.</translation>
    </message>
    <message>
        <source>File Copy - Copy and Verify</source>
        <translation>File Copy - Kopieren und Verifizieren</translation>
    </message>
    <message>
        <source>File Copy - Preview Changes</source>
        <translation>File Copy - Vorschau Änderungen</translation>
    </message>
    <message>
        <source>Protected running host - details</source>
        <translation>Geschützter Running Host - Details</translation>
    </message>
    <message>
        <source>not mounted (offline target)</source>
        <translation>nicht montiert (Offline-Ziel)</translation>
    </message>
    <message>
        <source>Select a drive to see its details.</source>
        <translation>Wählen Sie ein Laufwerk, um seine Details zu sehen.</translation>
    </message>
    <message>
        <source>Inspecting the selected component.</source>
        <translation>Prüfung der ausgewählten Komponente.</translation>
    </message>
    <message>
        <source>Helper-confirmed by the last read-only diagnostics.</source>
        <translation>Vom Helfer durch die letzte schreibgeschützte Diagnose bestätigt.</translation>
    </message>
    <message>
        <source>Read-only inventory only; run diagnostics to confirm.</source>
        <translation>Nur schreibgeschütztes Inventar; zur Bestätigung Diagnose ausführen.</translation>
    </message>
    <message>
        <source>PROTECTED - running system; read-only details only</source>
        <translation>PROTECTED - Laufendes System; nur schreibgeschützte Details</translation>
    </message>
    <message>
        <source>Live / installer media - not selectable</source>
        <translation>Live / Installer Medien - nicht wählbar</translation>
    </message>
    <message>
        <source>PROTECTED - running host scope</source>
        <translation>PROTECTED - Running Host Scope</translation>
    </message>
    <message>
        <source>Unlock required before selection</source>
        <translation>Vor der Auswahl muss entsperrt werden</translation>
    </message>
    <message>
        <source>Eligible repair candidate</source>
        <translation>Förderfähiger Reparaturkandidat</translation>
    </message>
    <message>
        <source>Drive:</source>
        <translation>Laufwerk:</translation>
    </message>
    <message>
        <source>Detected target:</source>
        <translation>Erkanntes Ziel:</translation>
    </message>
    <message>
        <source>Pending inspection</source>
        <translation>Prüfung ausstehend</translation>
    </message>
    <message>
        <source>Model / label:</source>
        <translation>Modell / Bezeichnung:</translation>
    </message>
    <message>
        <source>Status:</source>
        <translation>Status:</translation>
    </message>
    <message>
        <source>Size:</source>
        <translation>Größe:</translation>
    </message>
    <message>
        <source>Connection:</source>
        <translation>Verbindung:</translation>
    </message>
    <message>
        <source>Filesystem:</source>
        <translation>Dateisystem:</translation>
    </message>
    <message>
        <source>UUID:</source>
        <translation>UUID:</translation>
    </message>
    <message>
        <source>Mounts:</source>
        <translation>Einhängepunkte:</translation>
    </message>
    <message>
        <source>Running system protection unresolved</source>
        <translation>Schutz des laufenden Systems ungelöst</translation>
    </message>
    <message>
        <source>No protected physical backing disk was identified</source>
        <translation>Keine geschützte physische Stützscheibe wurde identifiziert</translation>
    </message>
    <message>
        <source>Current running Linux system</source>
        <translation>Aktuell laufendes Linux-System</translation>
    </message>
    <message>
        <source>Critical mounts: %1</source>
        <translation>Kritische Mounts: %1</translation>
    </message>
    <message>
        <source>Read-only protected-running-system facts: the helper OS fact plus the inventory model, device path, size, transport and critical mounts. Nothing here is probed destructively.</source>
        <translation>Read-only protected-running-system facts: die Helfer-OS-Fakt plus das Inventarmodell, den Gerätepfad, die Größe, den Transport und die kritischen Halterungen. Nichts hier wird destruktiv untersucht.</translation>
    </message>
    <message>
        <source>Select a drive to see unlock status.</source>
        <translation>Wählen Sie ein Laufwerk aus, um den Entsperrstatus zu sehen.</translation>
    </message>
    <message>
        <source>State: protected
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
The protected running host cannot be unlocked or modified; unlock is available for an offline repair target only. Use Host Maintenance for the protected running host.</source>
        <translation>Staat: geschützt
Komponente: %1
Mapper: (keine)
Methode: Helfer entsperren (Cryptsetup; Passphrase über eine Mode-600-Keyfile, nach Gebrauch gelöscht)
Der geschützte ausgeführte Host kann nicht entsperrt oder geändert werden; Entsperrung ist nur für ein Offline-Reparaturziel verfügbar. Verwenden Sie Host Maintenance für den geschützten laufenden Host.</translation>
    </message>
    <message>
        <source>(none detected)</source>
        <translation>(keine festgestellt)</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
A locked LUKS container is visible on this drive; press Unlock to open it for this recovery session.</source>
        <translation>Zustand: gesperrt
Komponente: %1
Mapper: (keine)
Methode: Helfer entsperren (Cryptsetup; Passphrase über eine Mode-600-Keyfile, nach Gebrauch gelöscht)
Ein gesperrter LUKS-Container ist auf diesem Laufwerk sichtbar; drücken Sie Entsperren, um ihn für diese Wiederherstellungssitzung zu öffnen.</translation>
    </message>
    <message>
        <source>(visible mapper)</source>
        <translation>(sichtbarer Mapper)</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: already open before this session (visible mapper; Boot Bitch will reuse it and will not close it).</source>
        <translation>Staat: entsperrt
Komponente: %1
Mapper: %2
Methode: bereits vor dieser Sitzung geöffnet (sichtbarer Mapper; Boot Bitch wird es wiederverwenden und nicht schließen).</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper-confirmed mapping from the last read-only diagnostics.</source>
        <translation>Staat: entsperrt
Komponente: %1
Mapper: %2
Methode: Helfer-bestätigtes Mapping aus der letzten Read-Only-Diagnose.</translation>
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
        <translation>Zustand: gesperrt oder keine verschlüsselte Komponente erkannt
Komponente: (keine erkannt)
Mapper: (keine)
Methode: Helfer entsperren (Cryptsetup; Passphrase über eine Mode-600-Keyfile, nach Gebrauch gelöscht)
Für dieses Laufwerk wurden in der aktuellen Sitzung keine gesperrte LUKS-Komponente und kein Entsperrvorgang aufgezeichnet.</translation>
    </message>
    <message>
        <source>unavailable - %1</source>
        <translation>nicht verfügbar - %1</translation>
    </message>
    <message>
        <source>Host maintenance:
%1</source>
        <translation>Instandhaltung des Hosts:
%1</translation>
    </message>
    <message>
        <source>Target:
%1 + %2</source>
        <translation>Ziel:
%1 + %2</translation>
    </message>
    <message>
        <source>Authorization required: diagnostics and repairs fail closed until you press Authorize.</source>
        <translation>Autorisierung erforderlich: Diagnosen und Reparaturen werden nicht geschlossen, bis Sie Autorisieren drücken.</translation>
    </message>
    <message>
        <source>Re-establish the privileged helper session for the current scope now. The password is requested in the hidden-input modal and is never logged.</source>
        <translation>Stellen Sie jetzt die privilegierte Helfersitzung für den aktuellen Umfang wieder her. Das Passwort wird im Hidden-Input-Modal angefordert und nie protokolliert.</translation>
    </message>
    <message>
        <source>Running...</source>
        <translation>Laufen...</translation>
    </message>
    <message>
        <source>Scope required</source>
        <translation>Anwendungsbereich erforderlich</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope; this unlocks the gated actions.</source>
        <translation>Run All - Führen Sie jede verfügbare schreibgeschützte Diagnose für den aktuellen Bereich aus; Dies entsperrt die Gated-Aktionen.</translation>
    </message>
    <message>
        <source>Select a target and wait for any running command first.</source>
        <translation>Wählen Sie ein Ziel aus und warten Sie zuerst auf einen ausgeführten Befehl.</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic through the helper.</source>
        <translation>Diagnose ausführen - Führen Sie die ausgewählte schreibgeschützte Diagnose über den Helfer aus.</translation>
    </message>
    <message>
        <source>Run All for the current scope and refresh the read-only backend profile.</source>
        <translation>Führen Sie All für den aktuellen Bereich aus und aktualisieren Sie das schreibgeschützte Backend-Profil.</translation>
    </message>
    <message>
        <source>Read or edit the selected target configuration file through the guarded helper; a saved edit invalidates cached diagnostics.</source>
        <translation>Lesen oder Bearbeiten der ausgewählten Zielkonfigurationsdatei über den bewachten Helfer; eine gespeicherte Bearbeitung entwertet die zwischengespeicherte Diagnose.</translation>
    </message>
    <message>
        <source>Host Maintenance has no target-file editing; commit an offline repair target first.</source>
        <translation>Host Maintenance hat keine Zieldateibearbeitung; Begehen Sie zuerst ein Offline-Reparaturziel.</translation>
    </message>
    <message>
        <source>Commit an offline repair target first.</source>
        <translation>Begehen Sie zuerst ein Offline-Reparaturziel.</translation>
    </message>
    <message>
        <source>No target configuration file is available for this target; run diagnostics to probe the list.</source>
        <translation>Für dieses Ziel ist keine Zielkonfigurationsdatei verfügbar; führen Sie Diagnosen aus, um die Liste zu untersuchen.</translation>
    </message>
    <message>
        <source>Already Unlocked</source>
        <translation>Bereits entsperrt</translation>
    </message>
    <message>
        <source>An unlocked Linux filesystem is already visible on this drive. Boot Bitch will reuse the existing mapper and will not close or reopen a mapping created by this recovery session. Mounting happens during diagnostics (read-only) and repairs (read-write); data filesystems are never auto-mounted on selection.</source>
        <translation>Ein entsperrtes Linux-Dateisystem ist bereits auf diesem Laufwerk sichtbar. Boot Bitch wird den vorhandenen Mapper wiederverwenden und ein von dieser Wiederherstellungssitzung erstelltes Mapping nicht schließen oder erneut öffnen. Die Montage erfolgt während der Diagnose (read-only) und Reparaturen (read-write); Datendateisysteme werden bei der Auswahl nie automatisch gemountet.</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked; use Host Maintenance for the protected running host.</source>
        <translation>Der geschützte ausgeführte Host kann nicht entsperrt werden; verwenden Sie Host Maintenance für den geschützten ausgeführten Host.</translation>
    </message>
    <message>
        <source>Host Maintenance is the current scope, but the selected offline drive can still be unlocked.</source>
        <translation>Host Maintenance ist der aktuelle Umfang, aber das ausgewählte Offline-Laufwerk kann weiterhin entsperrt werden.</translation>
    </message>
    <message>
        <source>Unlock %1 using cryptsetup through the privileged helper. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>Entsperren Sie %1 mit cryptsetup durch den privilegierten Helfer. Die Passphrase durchläuft eine private Schlüsseldatei und wird niemals in Befehlsargumenten oder Protokollen platziert.</translation>
    </message>
    <message>
        <source>The protected running system cannot be selected as a repair target; use Host Maintenance for the protected running host.</source>
        <translation>Das geschützte laufende System kann nicht als Reparaturziel ausgewählt werden; verwenden Sie Host Maintenance für den geschützten laufenden Host.</translation>
    </message>
    <message>
        <source>Live / installer media is read-only boot media and cannot be selected as a repair target.</source>
        <translation>Live / Installer-Medien sind schreibgeschützte Boot-Medien und können nicht als Reparaturziel ausgewählt werden.</translation>
    </message>
    <message>
        <source>Unlock the encrypted volume first; Select Target becomes available after a Linux filesystem is detected.</source>
        <translation>Entsperren Sie zuerst das verschlüsselte Volume; Select Target wird verfügbar, nachdem ein Linux-Dateisystem erkannt wurde.</translation>
    </message>
    <message>
        <source>Committed repair target. Repair, Diagnostics and File Copy target this physical drive until another drive is explicitly selected with Select Target.</source>
        <translation>Verpflichtetes Reparaturziel. Reparatur, Diagnose und Dateikopie zielen auf dieses physische Laufwerk, bis ein anderes Laufwerk explizit mit Select Target ausgewählt wird.</translation>
    </message>
    <message>
        <source>Commit %1 as the repair target; this leaves Host Maintenance and switches the scope to the selected drive.</source>
        <translation>Legen Sie %1 als Reparaturziel fest; dies verlässt die Host-Wartung und schaltet den Umfang auf das ausgewählte Laufwerk.</translation>
    </message>
    <message>
        <source>The running host target could not be detected.</source>
        <translation>Das laufende Host-Ziel konnte nicht erkannt werden.</translation>
    </message>
    <message>
        <source>Leave Host Maintenance and return to repair-target mode.</source>
        <translation>Verlassen Sie die Host-Wartung und kehren Sie in den Reparaturzielmodus zurück.</translation>
    </message>
    <message>
        <source>Select the running host for deliberate guarded maintenance; the administrator authorization is requested here once and cached for the session.</source>
        <translation>Wählen Sie den ausgeführten Host für bewusst geschützte Wartung aus; die Administratorautorisierung wird hier einmal angefordert und für die Sitzung zwischengespeichert.</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a repair target.</source>
        <translation>Das laufende Host-Ziel konnte nicht erkannt werden; die Diagnose benötigt ein Reparaturziel.</translation>
    </message>
    <message>
        <source>Committed target: %1 + %2</source>
        <translation>Engagiertes Ziel: %1 + %2</translation>
    </message>
    <message>
        <source>Committed target: none (selection changed)</source>
        <translation>Engagiertes Ziel: keine (Auswahl geändert)</translation>
    </message>
    <message>
        <source>The selected scope has no resolved Linux root component; Refresh Devices and commit the repair target again.</source>
        <translation>Der ausgewählte Bereich hat keine aufgelöste Linux-Root-Komponente; Geräte auffrischen und das Reparaturziel erneut festlegen.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize on the Systems or Repair tab to re-establish it. Run All reuses the cached authorization and never prompts by itself.</source>
        <translation>Keine Administratorsitzung ist aktiv; drücken Sie auf der Registerkarte Systeme oder Reparatur autorisieren, um sie wiederherzustellen. Run All verwendet die zwischengespeicherte Autorisierung wieder und fordert niemals von selbst auf.</translation>
    </message>
    <message>
        <source>Run diagnostics for this scope to unlock the gated actions. Diagnostics are read-only and the only evidence source.</source>
        <translation>Führen Sie Diagnosen für diesen Bereich aus, um die Gated-Aktionen freizuschalten. Diagnosen sind read-only und die einzige Evidenzquelle.</translation>
    </message>
    <message>
        <source>A repair was not proven unchanged, so the cached diagnostics are invalidated. Run diagnostics again before another gated action.</source>
        <translation>Eine Reparatur wurde nicht unverändert nachgewiesen, so dass die zwischengespeicherten Diagnosen ungültig sind. Führen Sie die Diagnose erneut aus, bevor Sie eine weitere Gated Action ausführen.</translation>
    </message>
    <message>
        <source>Gated actions reflect the cached capability lines; the helper still runs every runtime preflight when a command starts.</source>
        <translation>Gated-Aktionen spiegeln die zwischengespeicherten Fähigkeitszeilen wider; der Helfer führt weiterhin jeden Laufzeit-Vorflug aus, wenn ein Befehl gestartet wird.</translation>
    </message>
    <message>
        <source>Regenerating diagnostics automatically</source>
        <translation>Diagnose automatisch regenerieren</translation>
    </message>
    <message>
        <source>Running diagnostic: %1</source>
        <translation>Laufende Diagnose: %1</translation>
    </message>
    <message>
        <source>Running all diagnostics</source>
        <translation>Alle Diagnosen ausführen</translation>
    </message>
    <message>
        <source>Unlocking %1</source>
        <translation>Entsperrung %1</translation>
    </message>
    <message>
        <source>running host</source>
        <translation>Laufender Gastgeber</translation>
    </message>
    <message>
        <source>offline target</source>
        <translation>Offline-Ziel</translation>
    </message>
    <message>
        <source>host maintenance active</source>
        <translation>Host-Wartung aktiv</translation>
    </message>
    <message>
        <source>repair target committed</source>
        <translation>Zugesagtes Reparaturziel</translation>
    </message>
    <message>
        <source>no committed scope</source>
        <translation>kein verbindlicher Anwendungsbereich</translation>
    </message>
    <message>
        <source>Current session</source>
        <translation>Aktuelle Sitzung</translation>
    </message>
    <message>
        <source>Log files (*.log);;All files (*)</source>
        <translation>Logfiles (*.log);;Alle Dateien (*)</translation>
    </message>
    <message>
        <source>Save log as</source>
        <translation>Protokoll speichern als</translation>
    </message>
    <message>
        <source>About Boot Bitch</source>
        <translation>Über Boot Bitch</translation>
    </message>
    <message>
        <source>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;&lt;p&gt;&lt;b&gt;Developer:&lt;/b&gt; CaptainMorgan12&lt;/p&gt;&lt;p&gt;This GUI is the package&apos;s entry point: a Qt 3.3.x frontend with the ported guarded helper for Debian Etch-era systems.&lt;/p&gt;&lt;p&gt;&lt;b&gt;Guarded repair mode:&lt;/b&gt; read-only diagnostics can inspect either the protected Running Host or an explicitly selected repair drive. The guarded Debian/APT package stages (interrupted configuration, broken dependencies, metadata refresh, upgrade), GRUB-legacy configuration regeneration, LUKS target unlock and guarded target-file editing run through the ported helper after confirmation; Host Maintenance enables the same supported stages natively on the active system after repeating the host identity and boot-mount checks.&lt;/p&gt;&lt;p&gt;The modern-only features - verified File Copy, Btrfs snapshot rollback, EFI/UKI and extlinux repair, boot-stack reconciliation and Make Default - are greyed with the helper&apos;s own probe reasons on this frontend; Arch/Alpine/Fedora package backends stay diagnostics-only here.&lt;/p&gt;&lt;p&gt;The first privileged action authorizes one cached administrator session per scope through a hidden-input modal (the Qt GUI remains unprivileged). It can be ended at any time from File - Lock Administrator Session.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;&lt;p&gt;&lt;b&gt;Entwickler:&lt;/b&gt; CaptainMorgan12&lt;/p&gt;&lt;p&gt; Diese GUI ist der Einstiegspunkt des Pakets: ein Qt 3.3.x-Frontend mit dem portierten bewachten Helfer für Debian Etch-Systeme.&lt;/p&gt;&lt;p&gt;&lt;b&gt;Guarded repair mode:&lt;/b&gt; read-only diagnostics kann entweder den geschützten Running Host oder ein explizit ausgewähltes Reparaturlaufwerk inspizieren. Die geschützten Debian/APT-Paketphasen (unterbrochene Konfiguration, defekte Abhängigkeiten, Metadaten-Aktualisierung, Upgrade), GRUB-Legacy-Konfigurationsregeneration, LUKS-Zielentsperrung und geschützte Zieldateibearbeitung laufen nach Bestätigung durch den portierten Helfer; Host Maintenance ermöglicht die gleichen unterstützten Phasen nativ auf dem aktiven System nach Wiederholung der Host-Identität und Boot-Mount-Prüfungen.&lt;/p&gt;&lt;p&gt; Die modernen Funktionen - verifizierte Dateikopie, Btrfs Snapshot Rollback, EFI / UKI und extlinux Reparatur, Boot-Stack-Abgleich und Make Default - sind mit den eigenen Sondengründen des Helfers auf diesem Frontend vergraut; Arch / Alpen / Fedora Paket-Backends bleiben nur hier Diagnose. &lt;/p&gt;&lt;p&gt; Die erste privilegierte Aktion autorisiert eine zwischengespeicherte Administratorsitzung pro Scope über ein verstecktes Eingabemodal (die Qt-GUI bleibt unprivilegiert). Es kann jederzeit von File - Lock Administrator Session beendet werden.&lt;/p&gt;</translation>
    </message>
</context>
</TS>
