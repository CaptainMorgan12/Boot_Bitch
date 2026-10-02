<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS language="it">
<context>
    <name>LegacyMainWindow</name>
    <message>
        <source>Validate environment</source>
        <translation>Convalida ambiente</translation>
    </message>
    <message>
        <source>Validate</source>
        <translation>Convalida</translation>
    </message>
    <message>
        <source>Check the selected system&apos;s mounts, filesystem metadata, boot files, mapper consistency and dependency readiness before any repair action. This is an independent safety preflight rather than an optional Full Repair stage.</source>
        <translation>Controllare i supporti del sistema selezionato, i metadati del filesystem, i file di avvio, la consistenza del mapper e la disponibilità di dipendenza prima di qualsiasi azione di riparazione. Questo è un preflight di sicurezza indipendente piuttosto che una fase di riparazione completa opzionale.</translation>
    </message>
    <message>
        <source>Always preflight</source>
        <translation>Sempre prefabbricato</translation>
    </message>
    <message>
        <source>File system repair</source>
        <translation>Riparazione file system</translation>
    </message>
    <message>
        <source>Check File Systems</source>
        <translation>Controllare i file system</translation>
    </message>
    <message>
        <source>Run the read-only file system check for the selected system&apos;s root and /boot filesystems and report each device&apos;s check tool and result without changing anything. This legacy frontend exposes the read-only check only; device repair is not wired.</source>
        <translation>Eseguire il controllo del file system di sola lettura per i filesystem root e /boot del sistema selezionato e segnalare lo strumento di controllo di ogni dispositivo e il risultato senza cambiare nulla. Questo frontend legacy espone solo il controllo di sola lettura; la riparazione del dispositivo non è cablata.</translation>
    </message>
    <message>
        <source>Full Repair plan: unavailable on this frontend - read-only check only</source>
        <translation>Piano di riparazione completo: non disponibile su questo frontend - solo controllare</translation>
    </message>
    <message>
        <source>Complete package configuration</source>
        <translation>Configurazione completa del pacchetto</translation>
    </message>
    <message>
        <source>Complete Configuration</source>
        <translation>Configurazione completa</translation>
    </message>
    <message>
        <source>Complete interrupted dpkg package configuration in the selected repair system. This is the same stage controlled by Settings -&gt; Full Repair plan -&gt; Complete interrupted package configuration, but it can also be run independently here.</source>
        <translation>Configurazione completa del pacchetto dpkg interrotta nel sistema di riparazione selezionato. Questa è la stessa fase controllata da Impostazioni -&gt; Piano di riparazione completo -&gt; Completa configurazione del pacchetto interrotta, ma può anche essere eseguito indipendentemente qui.</translation>
    </message>
    <message>
        <source>Run the guarded dpkg-configure repair? The helper keeps its package-lock and runtime preflights.</source>
        <translation>Eseguire la riparazione dpkg-configure sorvegliato? L&apos;aiutante mantiene il suo pacchetto-lock e runtime preflights.</translation>
    </message>
    <message>
        <source>Repair broken dependencies</source>
        <translation>Riparare le dipendenze rotte</translation>
    </message>
    <message>
        <source>Repair Dependencies</source>
        <translation>Dipendenze di riparazione</translation>
    </message>
    <message>
        <source>Repair package dependencies in the selected repair system after the mandatory safety preflight. This maps directly to Settings -&gt; Repair broken package dependencies.</source>
        <translation>Le dipendenze del pacchetto di riparazione nel sistema di riparazione selezionato dopo il preflight di sicurezza obbligatorio. Questa mappa direttamente a Impostazioni -&gt; Riparare le dipendenze dei pacchetti rotti.</translation>
    </message>
    <message>
        <source>Run the guarded fix-broken repair? The helper keeps its simulation-first preflight and runtime guards.</source>
        <translation>Eseguire la riparazione riparata e rotta sorvegliata? L&apos;aiutante mantiene la sua simulazione-primo preflight e le guardie runtime.</translation>
    </message>
    <message>
        <source>Refresh package metadata</source>
        <translation>metadati del pacchetto Refresh</translation>
    </message>
    <message>
        <source>Refresh Metadata</source>
        <translation>Aggiornare i metadati</translation>
    </message>
    <message>
        <source>Refresh APT metadata in the selected repair system without upgrading installed packages. This maps directly to Settings -&gt; Refresh package metadata.</source>
        <translation>Aggiornare i metadati APT nel sistema di riparazione selezionato senza aggiornare i pacchetti installati. Questa mappa direttamente a Impostazioni -&gt; metadati di pacchetto rinfrescante.</translation>
    </message>
    <message>
        <source>Refresh package metadata for the selected scope? The helper requires a reachable, trusted APT source.</source>
        <translation>I metadati del pacchetto per il campo selezionato? L&apos;aiutante richiede una fonte APT raggiungibile e affidabile.</translation>
    </message>
    <message>
        <source>Upgrade installed packages</source>
        <translation>Aggiorna i pacchetti installati</translation>
    </message>
    <message>
        <source>Simulate and Upgrade</source>
        <translation>Simulare e aggiornare</translation>
    </message>
    <message>
        <source>Simulate the APT transaction first, inspect proposed removals, then apply a safe upgrade. This maps directly to Settings -&gt; Upgrade installed packages.</source>
        <translation>Simulare prima la transazione APT, ispezionare le rimozioni proposte, quindi applicare un aggiornamento sicuro. Questa mappa direttamente a Impostazioni -&gt; Aggiorna i pacchetti installati.</translation>
    </message>
    <message>
        <source>Run the guarded apt-upgrade transaction? The helper keeps its simulation-first and source guards.</source>
        <translation>Eseguire la transazione apt-upgrade sorvegliata? L&apos;aiutante mantiene la sua prima simulazione e le guardie di sorgente.</translation>
    </message>
    <message>
        <source>DKMS</source>
        <translation>DK</translation>
    </message>
    <message>
        <source>Rebuild DKMS</source>
        <translation>Ricostruire DKMS</translation>
    </message>
    <message>
        <source>Rebuild out-of-tree kernel modules for kernels installed in the selected system. The helper refuses this action when DKMS is not installed; this legacy frontend exposes no DKMS action.</source>
        <translation>Ricostruire i moduli del kernel out-of-tree per i kernel installati nel sistema selezionato. L&apos;aiutante rifiuta questa azione quando DKMS non è installato; questo frontend legacy non espone azione DKMS.</translation>
    </message>
    <message>
        <source>Graphical login / display manager</source>
        <translation>Gestione grafica di login / display</translation>
    </message>
    <message>
        <source>Restore Graphical Login</source>
        <translation>Ripristinare il Login grafico</translation>
    </message>
    <message>
        <source>Restore the legacy SysV display manager configured for the running host: the /etc/X11/default-display-manager entry and the missing runlevel S-symlink, with a backup and rollback, never starting the GUI. This is a host-scope stage on this legacy frontend.</source>
        <translation>Ripristinare il gestore di visualizzazione SysV legacy configurato per l&apos;host in esecuzione: l&apos;ingresso /etc/X11/default-display-manager e il runlevel S-symlink mancante, con un backup e rollback, non avviare mai la GUI. Questo è uno stadio host-scope su questo frontend legacy.</translation>
    </message>
    <message>
        <source>Restore the graphical login configuration for the running host? The helper backs up /etc/X11/default-display-manager and the runlevel symlink state, restores the configured entry and the missing S-symlink, rolls back on any failure, and never starts the display manager.</source>
        <translation>Ripristinare la configurazione di login grafica per l&apos;host in esecuzione? L&apos;aiutante supporta /etc/X11/default-display-manager e lo stato di runlevel symlink, ripristina l&apos;ingresso configurato e il S-symlink mancante, torna su qualsiasi guasto e non avvia mai il display manager.</translation>
    </message>
    <message>
        <source>Initramfs</source>
        <translation>Initramfs</translation>
    </message>
    <message>
        <source>Rebuild Initramfs</source>
        <translation>Ricostruire Initramfs</translation>
    </message>
    <message>
        <source>Rebuild initramfs images for the selected repair system only after mapper and crypttab consistency checks pass. The helper backs up each image before the apply. On Etch the stage runs through the guarded plain-chroot fallback (no unshare required).</source>
        <translation>Ricostruire immagini initramfs per il sistema di riparazione selezionato solo dopo mapper e crypttab controllo consistenza passa. L&apos;aiutante supporta ogni immagine prima dell&apos;applicazione. Su Etch il palco scorre attraverso il ripiegamento di base sorvegliato (non è richiesta alcuna condivisione).</translation>
    </message>
    <message>
        <source>Rebuild the initramfs for the selected scope? The helper keeps its mapper/crypttab and backup preflights.</source>
        <translation>Ricostruire il initramfs per il campo selezionato? L&apos;aiutante mantiene il suo mapper/crypttab e preflight di backup.</translation>
    </message>
    <message>
        <source>EFI / UKI bootloader</source>
        <translation>EFI / UKI bootloader</translation>
    </message>
    <message>
        <source>Repair EFI / UKI</source>
        <translation>Riparazione EFI / UKI</translation>
    </message>
    <message>
        <source>Repair the selected system&apos;s EFI / UKI boot path. This legacy frontend exposes no EFI action; the Etch target is a BIOS/GRUB-legacy system.</source>
        <translation>Riparare il percorso di avvio EFI / UKI del sistema selezionato. Questo frontend legacy non espone alcuna azione EFI; l&apos;obiettivo Etch è un sistema BIOS/GRUB-legacy.</translation>
    </message>
    <message>
        <source>GRUB configuration</source>
        <translation>Configurazione GRUB</translation>
    </message>
    <message>
        <source>Regenerate GRUB</source>
        <translation>Rigenera GRUB</translation>
    </message>
    <message>
        <source>Regenerate the selected repair system&apos;s GRUB menu/configuration after the mandatory safety preflight. The helper backs up menu.lst, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Rigenerare il menu/configurazione GRUB del sistema di riparazione selezionato dopo il preflight di sicurezza obbligatorio. L&apos;aiutante supporta menu.lst, conserva ogni entrata di avvio esistente e torna su qualsiasi guasto.</translation>
    </message>
    <message>
        <source>Regenerate the GRUB configuration? The helper backs up the target menu/configuration, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Rigenerare la configurazione GRUB? L&apos;aiutante supporta il menu/configurazione di destinazione, conserva ogni entrata di avvio esistente e torna su qualsiasi guasto.</translation>
    </message>
    <message>
        <source>extlinux configuration</source>
        <translation>Configurazione extlinux</translation>
    </message>
    <message>
        <source>Regenerate extlinux</source>
        <translation>Rigenera extlinux</translation>
    </message>
    <message>
        <source>Regenerate the selected system&apos;s extlinux bootloader configuration. This legacy frontend exposes no extlinux action.</source>
        <translation>Rigenerare la configurazione del bootloader extlinux del sistema selezionato. Questo frontend legacy non espone alcuna azione extlinux.</translation>
    </message>
    <message>
        <source>Boot stack reconciliation</source>
        <translation>Riconciliazione dello stack di avvio</translation>
    </message>
    <message>
        <source>Reconcile Boot Stack</source>
        <translation>Reconcile Boot Stack</translation>
    </message>
    <message>
        <source>Reconcile the selected repair system&apos;s boot stack in one guarded legacy pass: mapper/crypttab validation, initramfs rebuild and GRUB-legacy configuration regeneration, with the component backups and preflights unchanged. This is the Etch equivalent of the modern boot-stack reconciliation and stays out of the Full Repair plan.</source>
        <translation>Riconciliare lo stack di avvio del sistema di riparazione selezionato in un unico passaggio di legacy custodito: la validazione di mapper/crypttab, la ricostruzione initramfs e la rigenerazione della configurazione di GRUB-legacy, con i backup dei componenti e preflights invariati. Questo è l&apos;equivalente di Etch della riconciliazione moderna di boot-stack e rimane fuori dal piano Full Repair.</translation>
    </message>
    <message>
        <source>Full Repair plan: Manual recovery tool</source>
        <translation>Piano di riparazione completo: strumento di recupero manuale</translation>
    </message>
    <message>
        <source>Run the guarded boot-stack reconciliation? The helper runs the mapper/crypttab validation, the initramfs rebuild and the GRUB-legacy regeneration in one pass with every component preflight and backup.</source>
        <translation>Eseguite la riconciliazione con gli stivali sorvegliati? L&apos;aiutante gestisce la validazione mapper/crypttab, la ricostruzione initramfs e la rigenerazione della GRUB-legacy in un unico passaggio con ogni componente preflight e backup.</translation>
    </message>
    <message>
        <source>Complete interrupted package configuration</source>
        <translation>Configurazione completa dei pacchetti interrotti</translation>
    </message>
    <message>
        <source>Repair broken package dependencies</source>
        <translation>Riparare le dipendenze del pacchetto rotto</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Predefinito</translation>
    </message>
    <message>
        <source>Repair file system errors (read-only check first)</source>
        <translation>Riparare gli errori del file system (controllare prima di sola lettura)</translation>
    </message>
    <message>
        <source>the legacy frontend exposes the read-only file system check only; per-device repair is not wired on this frontend (fail closed)</source>
        <translation>il frontend legacy espone solo il controllo del file system di sola lettura; la riparazione per dispositivo non è cablata su questo frontend (fine chiuso)</translation>
    </message>
    <message>
        <source>Upgrade installed packages (adaptive APT simulation)</source>
        <translation>Aggiornare i pacchetti installati (simulazione APT adattativa)</translation>
    </message>
    <message>
        <source>Rebuild DKMS modules</source>
        <translation>Ricostruire i moduli DKMS</translation>
    </message>
    <message>
        <source>Restore graphical login manager</source>
        <translation>Ripristinare grafico login manager</translation>
    </message>
    <message>
        <source>Rebuild initramfs after mapper/crypttab validation</source>
        <translation>Ricostruire initramfs dopo la convalida di mapper/crypttab</translation>
    </message>
    <message>
        <source>Repair EFI / UKI boot path</source>
        <translation>Riparazione percorso di avvio EFI / UKI</translation>
    </message>
    <message>
        <source>Update GRUB configuration</source>
        <translation>Aggiornare la configurazione GRUB</translation>
    </message>
    <message>
        <source>Update extlinux configuration</source>
        <translation>Aggiornare la configurazione extlinux</translation>
    </message>
    <message>
        <source>Choose Full Repair stages in Settings. Enabled stages run in the order shown. Each configurable stage also appears below as an individual tool; the Full Repair column mirrors its current Settings state. The boot tools (EFI / UKI bootloader, GRUB or extlinux configuration, boot-stack reconciliation and Make Default) are independent: run them in any order, and a later action re-verifies what an earlier one changed and reports its own result. The active scope is shown beside Repair: selected repair drive or Running Host maintenance.</source>
        <translation>Scegliere le fasi di riparazione completa in Impostazioni. Attivare le fasi di esecuzione nell&apos;ordine indicato. Ogni fase configurabile appare anche di seguito come uno strumento individuale; la colonna Full Repair rispecchia lo stato delle impostazioni attuali. Gli strumenti di avvio (EFI / UKI bootloader, GRUB o extlinux configurazione, boot-stack riconciliazione e Make Default) sono indipendenti: eseguirli in qualsiasi ordine e un&apos;azione successiva ri-verifica ciò che un precedente ha cambiato e riferisce il proprio risultato. Lo scopo attivo è mostrato accanto a Riparazione: unità di riparazione selezionata o manutenzione di Host in esecuzione.</translation>
    </message>
    <message>
        <source>Run All diagnostics for the selected target or running host before starting Full Repair. The report is read-only evidence used to choose and confirm repair stages.</source>
        <translation>Eseguire tutte le diagnostica per il target selezionato o l&apos;host in esecuzione prima di iniziare la Riparazione completa. Il rapporto è prova di sola lettura utilizzata per scegliere e confermare le fasi di riparazione.</translation>
    </message>
    <message>
        <source>Ready: required cached read-only diagnostics are available for the selected stages. Review them in Diagnostics or Logs before confirming.</source>
        <translation>Pronto: la diagnostica di sola lettura memorizzata nella cache è disponibile per le fasi selezionate. Verificali in Diagnostics o Logs prima di confermare.</translation>
    </message>
    <message>
        <source>Environment validation</source>
        <translation>Validazione dell&apos;ambiente</translation>
    </message>
    <message>
        <source>Summarizes the selected system, protection state, mounted identity and inspection readiness.</source>
        <translation>Sostiene il sistema selezionato, lo stato di protezione, l&apos;identità montata e la disponibilità di ispezione.</translation>
    </message>
    <message>
        <source>Distribution and boot backend profile</source>
        <translation>Profilo di distribuzione e boot backend</translation>
    </message>
    <message>
        <source>Identifies the distribution family, package manager, initramfs generator, bootloader and current guarded repair capability.</source>
        <translation>Identifica la famiglia di distribuzione, gestore di pacchetti, generatore initramfs, bootloader e la capacità di riparazione sorvegliata corrente.</translation>
    </message>
    <message>
        <source>Boot diagnostics</source>
        <translation>Diagnostica di avvio</translation>
    </message>
    <message>
        <source>Shows boot mounts and /boot contents plus storage evidence without changing the selected system.</source>
        <translation>Mostra i supporti di avvio e /boot contenuti più prove di archiviazione senza cambiare il sistema selezionato.</translation>
    </message>
    <message>
        <source>Boot evidence and selection history</source>
        <translation>Prova di avvio e cronologia di selezione</translation>
    </message>
    <message>
        <source>Correlates the detected boot chain, bootloader selection, kernel/initramfs and unlock evidence.</source>
        <translation>Correlate la catena di avvio rilevata, selezione bootloader, kernel/initramfs e sbloccare le prove.</translation>
    </message>
    <message>
        <source>Kernel / initramfs</source>
        <translation>Kernel / initramfs</translation>
    </message>
    <message>
        <source>Reviews kernel files and verifies matching initramfs images through a read-only inspection.</source>
        <translation>Recensioni file del kernel e verifica le immagini initramfs corrispondenti attraverso un&apos;ispezione di sola lettura.</translation>
    </message>
    <message>
        <source>Reviews the GRUB configuration without changing boot files.</source>
        <translation>Recensioni la configurazione GRUB senza modificare i file di avvio.</translation>
    </message>
    <message>
        <source>EFI / UKI boot state</source>
        <translation>EFI / UKI stato di avvio</translation>
    </message>
    <message>
        <source>Inspects EFI/UKI evidence; unavailable on this legacy BIOS frontend with the helper&apos;s probe reason.</source>
        <translation>Ispezione delle prove EFI/UKI; non disponibile su questo frontend BIOS legacy con la ragione della sonda dell&apos;aiutante.</translation>
    </message>
    <message>
        <source>Reviews the configured display manager and recent boot evidence without starting the GUI.</source>
        <translation>Recensioni il display manager configurato e le recenti prove di avvio senza avviare la GUI.</translation>
    </message>
    <message>
        <source>Boot errors</source>
        <translation>Errori di avvio</translation>
    </message>
    <message>
        <source>Reads recent error-priority entries from the running host or selected repair system when available.</source>
        <translation>Legge recenti voci di errore-priorità dal host in esecuzione o sistema di riparazione selezionato quando disponibile.</translation>
    </message>
    <message>
        <source>Disk usage</source>
        <translation>Utilizzo del disco</translation>
    </message>
    <message>
        <source>Summarizes filesystem capacity and free space for the running host or read-only repair target.</source>
        <translation>Summarizes capacità del filesystem e spazio libero per l&apos;host in esecuzione o obiettivo di riparazione di sola lettura.</translation>
    </message>
    <message>
        <source>File systems</source>
        <translation>Sistemi di file</translation>
    </message>
    <message>
        <source>Runs the read-only file system check for the selected system&apos;s root, /boot and other filesystems.</source>
        <translation>Esegue il controllo del file system di sola lettura per il root del sistema selezionato, /boot e altri filesystem.</translation>
    </message>
    <message>
        <source>/etc/fstab review</source>
        <translation>Recensione /etc/fstab</translation>
    </message>
    <message>
        <source>Displays the running host or selected repair system&apos;s fstab; repair-system inspection is mounted read-only.</source>
        <translation>Visualizza l&apos;host in esecuzione o il fstab del sistema di riparazione selezionato; l&apos;ispezione del sistema di riparazione è montata in sola lettura.</translation>
    </message>
    <message>
        <source>Btrfs status</source>
        <translation>Stato Btrfs</translation>
    </message>
    <message>
        <source>Shows Btrfs filesystem and subvolume information when the target uses Btrfs.</source>
        <translation>Mostra il filesystem Btrfs e le informazioni subvolume quando il target utilizza Btrfs.</translation>
    </message>
    <message>
        <source>Device-mapper ancestry</source>
        <translation>Anteprima del dispositivo</translation>
    </message>
    <message>
        <source>Shows selected mapper ancestry and device-mapper state when available.</source>
        <translation>Mostra ancestry mapper selezionato e stato del dispositivo-mapper quando disponibile.</translation>
    </message>
    <message>
        <source>LUKS / crypttab evidence</source>
        <translation>LUKS / crypttab prove</translation>
    </message>
    <message>
        <source>Shows LUKS/mapped ancestry plus crypttab and fstab mapper references.</source>
        <translation>Mostra i riferimenti LUKS/mapped ancestry plus crypttab e fstab mapper.</translation>
    </message>
    <message>
        <source>Full diagnostic report</source>
        <translation>Rapporto diagnostico completo</translation>
    </message>
    <message>
        <source>Combines all read-only diagnostics for the selected scope (same as Run All).</source>
        <translation>Combina tutte le diagnostica di sola lettura per il campo selezionato (stesso come Run All).</translation>
    </message>
    <message>
        <source>All entries</source>
        <translation>Tutte le voci</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>Diagnostica</translation>
    </message>
    <message>
        <source>Repairs</source>
        <translation>Riparazioni</translation>
    </message>
    <message>
        <source>Package repair</source>
        <translation>Riparazione del pacchetto</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Copia file</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Rilevamento dispositivi</translation>
    </message>
    <message>
        <source>Host</source>
        <translation>Host</translation>
    </message>
    <message>
        <source>Required for block-device inventory</source>
        <translation>Richiesto per l&apos;inventario del dispositivo di blocco</translation>
    </message>
    <message>
        <source>Filesystem identification</source>
        <translation>Identificazione del file system</translation>
    </message>
    <message>
        <source>Used to identify filesystem metadata</source>
        <translation>Utilizzato per identificare i metadati del filesystem</translation>
    </message>
    <message>
        <source>Mount inspection</source>
        <translation>Ispezione del montaggio</translation>
    </message>
    <message>
        <source>Used to understand active mounts</source>
        <translation>Usato per capire i supporti attivi</translation>
    </message>
    <message>
        <source>LUKS support</source>
        <translation>Supporto LUKS</translation>
    </message>
    <message>
        <source>Required to unlock encrypted targets</source>
        <translation>Richiesto per sbloccare obiettivi crittografati</translation>
    </message>
    <message>
        <source>Btrfs support</source>
        <translation>Supporto Btrfs</translation>
    </message>
    <message>
        <source>Required for Btrfs inspection and snapshot rollback</source>
        <translation>Richiesto per l&apos;ispezione Btrfs e rollback snapshot</translation>
    </message>
    <message>
        <source>Bidirectional file copy</source>
        <translation>Copia del file bidirezionale</translation>
    </message>
    <message>
        <source>Host/Repair</source>
        <translation>Host/Repair</translation>
    </message>
    <message>
        <source>Required for verified Host-to-Repair and Repair-to-Host transfer</source>
        <translation>Richiesto per il trasferimento verificato Host-to-Repair e Repair-to-Host</translation>
    </message>
    <message>
        <source>Chroot repair</source>
        <translation>Riparazione Chroot</translation>
    </message>
    <message>
        <source>Required for target-side repair commands</source>
        <translation>Richiesto per i comandi di riparazione lato obiettivo</translation>
    </message>
    <message>
        <source>Offline systemd repair</source>
        <translation>Riparazione sistemata offline</translation>
    </message>
    <message>
        <source>Used to restore graphical.target and the configured display manager without starting the target GUI</source>
        <translation>Usato per ripristinare la grafica. target e il display manager configurato senza avviare la GUI di destinazione</translation>
    </message>
    <message>
        <source>UEFI NVRAM inspection</source>
        <translation>Controllo UEFI NVRAM</translation>
    </message>
    <message>
        <source>Used to preserve target EFI BootOrder during TUXEDO UKI rebuilds when efivars are available</source>
        <translation>Utilizzato per preservare il target EFI BootOrder durante TUXEDO UKI ricostruisce quando efivars sono disponibili</translation>
    </message>
    <message>
        <source>UKI verification</source>
        <translation>Verifica UKI</translation>
    </message>
    <message>
        <source>Used to verify the kernel embedded in a rebuilt unified kernel image</source>
        <translation>Utilizzato per verificare il kernel incorporato in un&apos;immagine unificata del kernel ricostruita</translation>
    </message>
    <message>
        <source>GRUB EFI repair</source>
        <translation>GRUB EFI riparazione</translation>
    </message>
    <message>
        <source>Target/Host</source>
        <translation>Obiettivo/Host</translation>
    </message>
    <message>
        <source>Required only for conventional GRUB-based EFI systems</source>
        <translation>Richiesto solo per i sistemi EFI basati su GRUB convenzionali</translation>
    </message>
    <message>
        <source>Target</source>
        <translation>Obiettivo</translation>
    </message>
    <message>
        <source>Debian-family GRUB helper</source>
        <translation>Debian-family GRUB helper</translation>
    </message>
    <message>
        <source>Portable GRUB configuration generator used by Arch and other non-Debian systems</source>
        <translation>Generatore di configurazione GRUB portatile utilizzato da Arch e altri sistemi non Debian</translation>
    </message>
    <message>
        <source>Initramfs rebuild</source>
        <translation>Initramfs ricostruisce</translation>
    </message>
    <message>
        <source>Debian-family initramfs helper</source>
        <translation>Debian-famiglia initramfs helper</translation>
    </message>
    <message>
        <source>Arch-family initramfs generator</source>
        <translation>Generatore initramfs Archfamiglia</translation>
    </message>
    <message>
        <source>Alternative initramfs generator used by Arch and other distributions</source>
        <translation>Generatore alternativo initramfs utilizzato da Arch e altre distribuzioni</translation>
    </message>
    <message>
        <source>Initramfs verification</source>
        <translation>Verifica initramfs</translation>
    </message>
    <message>
        <source>Read-only verification for mkinitcpio images</source>
        <translation>Verifica di sola lettura per immagini mkinitcpio</translation>
    </message>
    <message>
        <source>Read-only verification for dracut images</source>
        <translation>Verifica di sola lettura per immagini dracut</translation>
    </message>
    <message>
        <source>systemd-boot inspection</source>
        <translation>Controllo systemd-boot</translation>
    </message>
    <message>
        <source>Host/Target</source>
        <translation>Host/Target</translation>
    </message>
    <message>
        <source>Read-only inspection of systemd-boot and generic UKI layouts</source>
        <translation>Controllo di sola lettura dei layout systemd-boot e UKI generici</translation>
    </message>
    <message>
        <source>Arch package manager</source>
        <translation>Arch package manager</translation>
    </message>
    <message>
        <source>Arch-family package database and transaction tool</source>
        <translation>Banca dati e strumento di transazione del pacchetto della famiglia</translation>
    </message>
    <message>
        <source>DKMS rebuild</source>
        <translation>Ricostruire DKMS</translation>
    </message>
    <message>
        <source>Required only when target uses DKMS modules</source>
        <translation>Richiesto solo quando il target utilizza moduli DKMS</translation>
    </message>
    <message>
        <source>LVM inspection</source>
        <translation>ispezione LVM</translation>
    </message>
    <message>
        <source>Optional LVM storage-stack support</source>
        <translation>Supporto opzionale LVM</translation>
    </message>
    <message>
        <source>Software RAID</source>
        <translation>RAID del software</translation>
    </message>
    <message>
        <source>Optional Linux MD RAID support</source>
        <translation>Supporto opzionale Linux MD RAID</translation>
    </message>
    <message>
        <source>Process namespace isolation</source>
        <translation>Isolamento del namespace del processo</translation>
    </message>
    <message>
        <source>Host+Target</source>
        <translation>Host+ Obiettivo</translation>
    </message>
    <message>
        <source>The ported helper falls back to a guarded plain chroot when unshare is absent</source>
        <translation>L&apos;aiutante portato rientra in un chroot normale sorvegliato quando unshare è assente</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Annulla</translation>
    </message>
    <message>
        <source>OK</source>
        <translation>Ok.</translation>
    </message>
    <message>
        <source>Yes</source>
        <translation>Sì.</translation>
    </message>
    <message>
        <source>No</source>
        <translation>No.</translation>
    </message>
    <message>
        <source>read-only helper diagnostic</source>
        <translation>diagnostica di helper di sola lettura</translation>
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
        <translation>utilità di recupero Linux e boot-repair</translation>
    </message>
    <message>
        <source>GUARDED REPAIR</source>
        <translation>REPAIR GUARDO</translation>
    </message>
    <message>
        <source>Ordinary repairs require an explicitly selected non-host target. The protected running host has a separate deliberate maintenance mode with the same guarded repair stages and requires privilege authorization.</source>
        <translation>Le riparazioni ordinarie richiedono un target non host esplicitamente selezionato. L&apos;host in esecuzione protetto ha una modalità di manutenzione deliberata separata con le stesse fasi di riparazione sorvegliate e richiede l&apos;autorizzazione di privilegi.</translation>
    </message>
    <message>
        <source>Systems</source>
        <translation>Sistemi</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>Diagnostica</translation>
    </message>
    <message>
        <source>Repair</source>
        <translation>Riparazione</translation>
    </message>
    <message>
        <source>Chroot Shell</source>
        <translation>Chroot Shell</translation>
    </message>
    <message>
        <source>File Copy</source>
        <translation>Copia file</translation>
    </message>
    <message>
        <source>Logs</source>
        <translation>Logs</translation>
    </message>
    <message>
        <source>Settings</source>
        <translation>Impostazioni impostazioni</translation>
    </message>
    <message>
        <source>(not created yet)</source>
        <translation>(non ancora creato)</translation>
    </message>
    <message>
        <source>Information</source>
        <translation>Informazioni</translation>
    </message>
    <message>
        <source>Close</source>
        <translation>Chiudi</translation>
    </message>
    <message>
        <source>&amp;File</source>
        <translation>&amp;File</translation>
    </message>
    <message>
        <source>&amp;Refresh Devices</source>
        <translation>&amp;Dispositivi di aggiornamento</translation>
    </message>
    <message>
        <source>&amp;Lock Administrator Session</source>
        <translation>&amp;Lock Administrator Sessione</translation>
    </message>
    <message>
        <source>&amp;Quit</source>
        <translation>&amp;Quit</translation>
    </message>
    <message>
        <source>&amp;View</source>
        <translation>&amp;Vista</translation>
    </message>
    <message>
        <source>&amp;Systems</source>
        <translation>&amp;Sistemi</translation>
    </message>
    <message>
        <source>&amp;Diagnostics</source>
        <translation>&amp;Diagnostica</translation>
    </message>
    <message>
        <source>&amp;Logs</source>
        <translation>&amp;Logs</translation>
    </message>
    <message>
        <source>&amp;Settings</source>
        <translation>&amp;Impostazioni</translation>
    </message>
    <message>
        <source>&amp;Auto-size Device Columns</source>
        <translation>&amp;Colonne del dispositivo a dimensione automatica</translation>
    </message>
    <message>
        <source>&amp;Wrap Log Lines</source>
        <translation>Linee di registro &amp;Wrap</translation>
    </message>
    <message>
        <source>&amp;Help</source>
        <translation>&amp;Aiuto</translation>
    </message>
    <message>
        <source>&amp;Using Boot Bitch</source>
        <translation>&amp;Utilizzo di Boot Bitch</translation>
    </message>
    <message>
        <source>&amp;About Boot Bitch</source>
        <translation>&amp;Informazioni su Boot Bitch</translation>
    </message>
    <message>
        <source>Device columns auto-sized. Drag headers to fine-tune widths.</source>
        <translation>Colonne del dispositivo auto-dimensionate. Trascina le intestazioni verso larghezze sottili.</translation>
    </message>
    <message>
        <source>A helper command is running; wait for it to finish before locking the session.</source>
        <translation>Un comando helper è in esecuzione; aspetta che finisca prima di chiudere la sessione.</translation>
    </message>
    <message>
        <source>Administrator session locked; the next privileged action will request authorization.</source>
        <translation>sessione di amministratore bloccata; la prossima azione privilegiata richiederà l&apos;autorizzazione.</translation>
    </message>
    <message>
        <source>Using Boot Bitch</source>
        <translation>Utilizzo di Boot Bitch</translation>
    </message>
    <message>
        <source>Boot Bitch must run from a different booted Linux environment than the system being repaired. Use a Linux live medium or another Linux installation on a different physical drive.&lt;br&gt;&lt;br&gt;The running host is protected from ordinary repair-target selection, but it can be explicitly selected through &lt;b&gt;Host Maintenance&lt;/b&gt; for guarded native diagnostics and supported maintenance stages.&lt;br&gt;&lt;br&gt;Diagnostics follow the Systems page: the committed repair drive while Host Maintenance is off, or the protected running host while it is active.&lt;br&gt;&lt;br&gt;The first privileged action requests administrator authorization once for this Boot Bitch window; &lt;b&gt;File - Lock Administrator Session&lt;/b&gt; ends that helper session immediately. Every repair keeps the helper&apos;s own runtime preflights.</source>
        <translation>Boot Bitch deve essere eseguito da un ambiente Linux diverso da quello riparato dal sistema. Utilizzare un Linux live medium o un&apos;altra installazione Linux su un&apos;unità fisica diversa. L&apos;host in esecuzione è protetto dalla selezione ordinaria di riparazione-target, ma può essere esplicitamente selezionato attraverso &lt;b&gt;Host Maintenance &lt;/b&gt; per la diagnostica nativo custodita e le fasi di manutenzione sostenute. I sistemi diagnostici seguono la pagina Sistemi: l&apos;unità di riparazione impegnata mentre la manutenzione host è spenta, o l&apos;host corrente protetto mentre è attivo. La prima azione privilegiata richiede l&apos;autorizzazione dell&apos;amministratore una volta per questa finestra Boot Bitch; &lt;b&gt;File - Lock Administrator Session &lt;/b&gt; termina immediatamente quella sessione di helper. Ogni riparazione mantiene i preflight di runtime dell&apos;aiutante.</translation>
    </message>
    <message>
        <source>Select a physical drive; Boot Bitch resolves the most likely Linux system volume automatically. The running host stays protected from ordinary target repairs, with a separate explicit host-maintenance path for its own system. The Details button shows the protected host&apos;s facts in the details pane; selecting any drive row restores the per-drive pane.</source>
        <translation>Selezionare un&apos;unità fisica; Boot Bitch risolve automaticamente il volume di sistema Linux più probabile. L&apos;host in esecuzione rimane protetto dalle normali riparazioni di destinazione, con un percorso di manutenzione esplicito separato per il proprio sistema. Il pulsante Dettagli mostra i fatti dell&apos;host protetto nel riquadro dei dettagli; selezionare qualsiasi riga di unità ripristina il riquadro per-drive.</translation>
    </message>
    <message>
        <source>Refresh Devices</source>
        <translation>Dispositivi di aggiornamento</translation>
    </message>
    <message>
        <source>Re-read the read-only kernel inventory (/proc/partitions, /proc/mounts, /proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* and the udev metadata database). No block device is opened and nothing is written.</source>
        <translation>Rileggere l&apos;inventario del kernel di sola lettura (/proc/partitions, /proc/mounts, /proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* e il database dei metadati udev). Nessun dispositivo di blocco è aperto e nulla è scritto.</translation>
    </message>
    <message>
        <source>[OK]</source>
        <translation>[OK]</translation>
    </message>
    <message>
        <source>The running system was detected and stays protected from ordinary target repairs.</source>
        <translation>Il sistema di funzionamento è stato rilevato e rimane protetto dalle normali riparazioni di destinazione.</translation>
    </message>
    <message>
        <source>Detecting running system...</source>
        <translation>Rilevamento del sistema di...</translation>
    </message>
    <message>
        <source>Detecting protected storage...</source>
        <translation>Rilevamento della conservazione protetta...</translation>
    </message>
    <message>
        <source>PROTECTED</source>
        <translation>PROTEZIONE</translation>
    </message>
    <message>
        <source>The running host remains protected from ordinary repair-target operations.</source>
        <translation>L&apos;host in esecuzione rimane protetto dalle normali operazioni di riparazione-target.</translation>
    </message>
    <message>
        <source>Details</source>
        <translation>Dettagli</translation>
    </message>
    <message>
        <source>Show read-only details for the protected running host.</source>
        <translation>Mostra i dettagli in sola lettura per l&apos;host in esecuzione protetto.</translation>
    </message>
    <message>
        <source>Host Maintenance</source>
        <translation>Manutenzione host</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Predefinito</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host (menu.lst default directive with a backup and rollback). Requires Host Maintenance and the cached host-default probe.</source>
        <translation>Fare l&apos;ingresso del kernel installato canonico l&apos;ingresso di avvio di default GRUB-legacy sull&apos;host in esecuzione (menu.lst direttiva di default con un backup e rollback). Richiede la manutenzione host e la sonda di default host cache.</translation>
    </message>
    <message>
        <source>Available repair targets</source>
        <translation>Obiettivi di riparazione disponibili</translation>
    </message>
    <message>
        <source>Most likely first</source>
        <translation>Molto probabilmente prima</translation>
    </message>
    <message>
        <source>Drives are listed by the read-only inventory. Select a row to inspect it; Select Target commits the selected non-host drive with its auto-resolved Linux root component.</source>
        <translation>Le unità sono elencate dall&apos;inventario di sola lettura. Selezionare una riga per ispezionarla; Select Target impegna l&apos;unità non host selezionata con il suo componente root Linux auto-risolto.</translation>
    </message>
    <message>
        <source>Device</source>
        <translation>Dispositivo</translation>
    </message>
    <message>
        <source>Size</source>
        <translation>Dimensione</translation>
    </message>
    <message>
        <source>Type</source>
        <translation>Tipo</translation>
    </message>
    <message>
        <source>Filesystem</source>
        <translation>File system</translation>
    </message>
    <message>
        <source>Select Target</source>
        <translation>Selezionare Target</translation>
    </message>
    <message>
        <source>Unlock</source>
        <translation>Sblocca</translation>
    </message>
    <message>
        <source>Authorize</source>
        <translation>Autorizzazione</translation>
    </message>
    <message>
        <source>Establish the privileged helper session for the current scope now instead of waiting for the next privileged action.</source>
        <translation>Stabilire la sessione privilegiata di helper per la portata attuale ora invece di aspettare la prossima azione privilegiata.</translation>
    </message>
    <message>
        <source>Committed target: none</source>
        <translation>Obiettivo: nessuno</translation>
    </message>
    <message>
        <source>Unlock state for the selected drive; the LUKS passphrase is never logged.</source>
        <translation>Sblocca lo stato per l&apos;unità selezionata; la passphrase LUKS non è mai registrata.</translation>
    </message>
    <message>
        <source>Unlock status</source>
        <translation>Sblocca lo stato</translation>
    </message>
    <message>
        <source>Read-only inventory plus helper-confirmed facts; mirrors the modern Qt6 Selected drive details panel.</source>
        <translation>inventario di sola lettura più fatti confermati dall&apos;aiutante; rispecchia il moderno pannello di dettagli dell&apos;azionamento selezionato Qt6.</translation>
    </message>
    <message>
        <source>Selected drive details</source>
        <translation>Dati di unità selezionati</translation>
    </message>
    <message>
        <source>Field</source>
        <translation>Campo</translation>
    </message>
    <message>
        <source>Value</source>
        <translation>Valore</translation>
    </message>
    <message>
        <source>Run All runs every available read-only diagnostic for the current scope; selecting a check runs it alone. Diagnostics are read-only and are the only evidence source for the gated repair actions.</source>
        <translation>Eseguire Tutte esegue ogni diagnostica disponibile di sola lettura per l&apos;ambito corrente; selezionare un controllo viene eseguito da solo. I sistemi diagnostici sono di sola lettura e sono l&apos;unica fonte di prova per le azioni di riparazione gated.</translation>
    </message>
    <message>
        <source>Target: none selected</source>
        <translation>Obiettivo: nessuno selezionato</translation>
    </message>
    <message>
        <source>Diagnostics follow the committed repair target, or the protected running host while Host Maintenance is active.</source>
        <translation>I sistemi diagnostici seguono l&apos;obiettivo di riparazione commesso, o l&apos;host in esecuzione protetto mentre la manutenzione host è attiva.</translation>
    </message>
    <message>
        <source>Run All</source>
        <translation>Tutti</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope.</source>
        <translation>Eseguire Tutti - eseguire ogni diagnostica disponibile di sola lettura per l&apos;ambito corrente.</translation>
    </message>
    <message>
        <source>Target configuration:</source>
        <translation>Configurazione obiettivo:</translation>
    </message>
    <message>
        <source>Etch-era target configuration files; availability is probed read-only by the helper&apos;s diagnostics.</source>
        <translation>File di configurazione di destinazione di Etch-era; la disponibilità è probed in sola lettura dalla diagnostica dell&apos;aiutante.</translation>
    </message>
    <message>
        <source>Edit Target File...</source>
        <translation>Modificare Target File...</translation>
    </message>
    <message>
        <source>Runs one read-only diagnostic for the selected scope through the helper (`diagnose &lt;key&gt;` / `host-diagnose &lt;key&gt;`); Run All is the combined report.</source>
        <translation>Esegue una diagnostica di sola lettura per il campo selezionato attraverso l&apos;aiutante (`diagnosi &lt;key&gt;` / `host-diagnose &lt;key&gt;`); Eseguire Tutto è il rapporto combinato.</translation>
    </message>
    <message>
        <source>Diagnostic checks</source>
        <translation>Controlli diagnostici</translation>
    </message>
    <message>
        <source>Check</source>
        <translation>Check</translation>
    </message>
    <message>
        <source>Selected diagnostic</source>
        <translation>Diagnostica selezionata</translation>
    </message>
    <message>
        <source>Select a diagnostic</source>
        <translation>Selezionare una diagnostica</translation>
    </message>
    <message>
        <source>Choose a diagnostic from the list.</source>
        <translation>Scegli una diagnostica dall&apos;elenco.</translation>
    </message>
    <message>
        <source>Ready</source>
        <translation>Pronti</translation>
    </message>
    <message>
        <source>Results</source>
        <translation>Risultati</translation>
    </message>
    <message>
        <source>Run Diagnostic</source>
        <translation>Eseguire diagnostica</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic for the current scope.</source>
        <translation>Eseguire Diagnostic - eseguire la diagnostica di sola lettura selezionata per l&apos;ambito corrente.</translation>
    </message>
    <message>
        <source>Copy Results</source>
        <translation>Copia i risultati</translation>
    </message>
    <message>
        <source>Save Results...</source>
        <translation>Salva i risultati...</translation>
    </message>
    <message>
        <source>The Full Repair plan runs the selected legacy stages in order through the guarded helper; the individual tools run one stage at a time. Every action stays disabled until the cached capability lines say available and the helper keeps its runtime preflights.</source>
        <translation>Il piano di riparazione completa esegue le fasi legacy selezionate in ordine attraverso l&apos;aiutante sorvegliato; i singoli strumenti eseguire una fase alla volta. Ogni azione rimane disabilitata fino a quando le linee di funzionalità cache dicono disponibili e l&apos;aiutante mantiene i suoi preflight runtime.</translation>
    </message>
    <message>
        <source>Full Repair plan</source>
        <translation>Piano di riparazione completo</translation>
    </message>
    <message>
        <source>No stages selected</source>
        <translation>Nessuna fase selezionata</translation>
    </message>
    <message>
        <source>Configure Plan...</source>
        <translation>Configurare il Piano...</translation>
    </message>
    <message>
        <source>Open Settings to choose which Full Repair stages are part of the plan.</source>
        <translation>Aprire Impostazioni per scegliere quali fasi di riparazione completa sono parte del piano.</translation>
    </message>
    <message>
        <source>Run Full Repair</source>
        <translation>Eseguire la riparazione completa</translation>
    </message>
    <message>
        <source>Select a repair drive, or choose Host Maintenance on the protected running-host card.</source>
        <translation>Selezionare un&apos;unità di riparazione, o scegliere Manutenzione Host sulla scheda di host in esecuzione protetta.</translation>
    </message>
    <message>
        <source>Stage</source>
        <translation>Fase</translation>
    </message>
    <message>
        <source>Individual repair tools</source>
        <translation>Strumenti di riparazione individuali</translation>
    </message>
    <message>
        <source>Tool</source>
        <translation>Strumento</translation>
    </message>
    <message>
        <source>Full Repair</source>
        <translation>Riparazione completa</translation>
    </message>
    <message>
        <source>not reported</source>
        <translation>non segnalato</translation>
    </message>
    <message>
        <source>Selected tool</source>
        <translation>Strumento selezionato</translation>
    </message>
    <message>
        <source>Select a repair tool</source>
        <translation>Selezionare uno strumento di riparazione</translation>
    </message>
    <message>
        <source>Run Tool</source>
        <translation>Strumento di esecuzione</translation>
    </message>
    <message>
        <source>Select a tool to review its repair action.</source>
        <translation>Selezionare uno strumento per rivedere la sua azione di riparazione.</translation>
    </message>
    <message>
        <source>Write actions ask for confirmation and then run the helper&apos;s own runtime preflights; the GUI never weakens them. A repair that is not proven &apos;unchanged&apos; invalidates the cached diagnostics and disables the gated actions until diagnostics run again.</source>
        <translation>Scrivere azioni chiedere la conferma e poi eseguire i preflights runtime dell&apos;aiutante; la GUI non li indebolisce mai. Una riparazione che non è comprovata &apos;non modificata&apos; invalida la diagnostica cache e disabilita le azioni gated fino a quando la diagnostica non viene eseguita di nuovo.</translation>
    </message>
    <message>
        <source>Chroot shell</source>
        <translation>shell Chroot</translation>
    </message>
    <message>
        <source>Offline commands run one at a time in a fresh chroot and cannot answer interactive prompts (apt-get -y upgrade works). Host-shell commands run directly on the running host. The helper&apos;s probe lines gate the command field; the exact reason appears in the tooltip.</source>
        <translation>I comandi offline vengono eseguiti uno alla volta in un chroot fresco e non possono rispondere a richieste interattive (apt-get -y upgrade works). I comandi Host-shell vengono eseguiti direttamente sull&apos;host in esecuzione. La sonda dell&apos;aiutante porta il campo di comando; la ragione esatta appare nella punta degli strumenti.</translation>
    </message>
    <message>
        <source>No &apos;Legacy feature shell:&apos; line is cached; run diagnostics for the selected scope to evaluate the helper&apos;s chroot/timeout containment probes (fail closed).</source>
        <translation>Nessuna shell &apos;Legacy feature:&apos; linea è memorizzata nella cache; eseguire la diagnostica per l&apos;ambito selezionato per valutare le sonde di contenimento chroot/timeout dell&apos;aiutante (fallito chiuso).</translation>
    </message>
    <message>
        <source>Command</source>
        <translation>Comando</translation>
    </message>
    <message>
        <source>Command:</source>
        <translation>Comando:</translation>
    </message>
    <message>
        <source>One reviewed command string, passed to the helper as a single argument (no shell interpolation by the GUI).</source>
        <translation>Una stringa di comando revisionata, passata all&apos;aiutante come un unico argomento (nessuna interpolazione della shell dalla GUI).</translation>
    </message>
    <message>
        <source>Run Command</source>
        <translation>Comando</translation>
    </message>
    <message>
        <source>Clear Output</source>
        <translation>Uscita libera</translation>
    </message>
    <message>
        <source>The helper exposes `shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for an offline target chroot and `host-shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for the running host. Both keep the helper&apos;s runtime preflights; this tab enables the command only when the scope is committed, the session is authorized and the scope&apos;s Legacy feature probe reports available.</source>
        <translation>L&apos;aiutante espone `shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` per un target offline chroot e `host-shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` per l&apos;hosting. Entrambi mantengono i preflight runtime dell&apos;aiutante; questa scheda consente al comando solo quando lo scopo è commesso, la sessione è autorizzata e i rapporti di sonda legacy di portata disponibili.</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Copia file</translation>
    </message>
    <message>
        <source>Copy and verify files in either direction through the guarded helper (cp -a plus ownership restoration and a per-file byte-compare). The helper&apos;s file-copy probe gates the controls and keeps the direction and path containment checks.</source>
        <translation>Copiare e verificare i file in entrambe le direzioni attraverso l&apos;aiutante custodito (cp -a plus ownership recovery and a per-file byte-compare). La sonda di file-copia dell&apos;aiutante porta i controlli e mantiene i controlli di contenimento della direzione e del percorso.</translation>
    </message>
    <message>
        <source>Preview Changes</source>
        <translation>Cambiamenti di anteprima</translation>
    </message>
    <message>
        <source>Run a copy dry-run through the guarded helper. No files are changed.</source>
        <translation>Eseguire una copia a secco attraverso l&apos;aiutante sorvegliato. Nessun file è cambiato.</translation>
    </message>
    <message>
        <source>Copy and Verify</source>
        <translation>Copia e verifica</translation>
    </message>
    <message>
        <source>Copy staged items and verify the result. Existing destination names are overwritten when source content differs; unrelated destination files are never deleted.</source>
        <translation>Copia gli elementi in fase e verifica il risultato. I nomi di destinazione esistenti vengono sovrascritti quando il contenuto di origine differisce; i file di destinazione non correlati non vengono mai eliminati.</translation>
    </message>
    <message>
        <source>Direction:</source>
        <translation>Direzione:</translation>
    </message>
    <message>
        <source>Choose which system supplies the source files and which system receives them.</source>
        <translation>Scegliere quale sistema fornisce i file sorgente e quale sistema li riceve.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from this host</source>
        <translation>1. Selezionare i file di origine o le cartelle da questo host</translation>
    </message>
    <message>
        <source>Source</source>
        <translation>Fonte</translation>
    </message>
    <message>
        <source>Files and folders staged for the verified copy. The legacy backend copies with cp -a and restores ownership with chown --reference; every regular file is byte-compared after the copy.</source>
        <translation>File e cartelle predisposti per la copia verificata. Il backend legacy copie con cp -a e ripristina la proprietà con chown --reference; ogni file regolare è byte-compared dopo la copia.</translation>
    </message>
    <message>
        <source>Add Files...</source>
        <translation>Aggiungi file...</translation>
    </message>
    <message>
        <source>Add Folder...</source>
        <translation>Aggiungi cartella...</translation>
    </message>
    <message>
        <source>Remove</source>
        <translation>Rimuovi</translation>
    </message>
    <message>
        <source>Clear</source>
        <translation>Libero</translation>
    </message>
    <message>
        <source>Clear the staged source list (nothing is copied or deleted).</source>
        <translation>Cancellare l&apos;elenco di origine in fase (niente viene copiato o cancellato).</translation>
    </message>
    <message>
        <source>2. Choose destination in repaired system</source>
        <translation>2. Scegli la destinazione nel sistema riparato</translation>
    </message>
    <message>
        <source>unavailable: see the helper&apos;s Legacy feature file-copy: probe reason above</source>
        <translation>non disponibile: vedi il file-copia della caratteristica Legacy dell&apos;aiutante: la ragione della sonda sopra</translation>
    </message>
    <message>
        <source>An absolute path inside the selected repair system (Host to Repair) or on the running host (Repair to Host).</source>
        <translation>Un percorso assoluto all&apos;interno del sistema di riparazione selezionato (Host to Repair) o sull&apos;host in esecuzione (Repair to Host).</translation>
    </message>
    <message>
        <source>Browse Target Folders...</source>
        <translation>Sfoglia le cartelle di destinazione...</translation>
    </message>
    <message>
        <source>Browse the selected repair system through the helper&apos;s temporary read-only mounts and choose an absolute destination path. No target files are changed while browsing.</source>
        <translation>Sfoglia il sistema di riparazione selezionato attraverso il supporto di sola lettura temporaneo dell&apos;aiutante e scegli un percorso di destinazione assoluto. Nessun file di destinazione sono cambiati durante la navigazione.</translation>
    </message>
    <message>
        <source>3. Ownership and copy policy</source>
        <translation>3. Politica di proprietà e copia</translation>
    </message>
    <message>
        <source>Ownership:</source>
        <translation>Proprietario:</translation>
    </message>
    <message>
        <source>Smart destination ownership (recommended)</source>
        <translation>Proprietà di destinazione intelligente (consigliato)</translation>
    </message>
    <message>
        <source>Preserve source numeric UID/GID</source>
        <translation>Conservare la sorgente numerica UID/GID</translation>
    </message>
    <message>
        <source>Smart mode validates UID/GID identity mapping across the two systems and falls back to the destination-directory owner when the same numeric ID means a different account (the legacy backend implements it with chown --reference).</source>
        <translation>La modalità Smart convalida la mappatura dell&apos;identità UID/GID attraverso i due sistemi e rientra nel proprietario della directory di destinazione quando lo stesso ID numerico significa un account diverso (il backend legacy lo implementa con chown --reference).</translation>
    </message>
    <message>
        <source>Application log</source>
        <translation>Registro delle applicazioni</translation>
    </message>
    <message>
        <source>The complete session register; Save As... writes every entry even while a filter hides lines. If a writable system share mount exists at /host, Save As... starts there; otherwise the log directory is the fallback. Prior session files are listed read-only.</source>
        <translation>Il registro di sessione completo; Save As... scrive ogni voce anche mentre un filtro nasconde le linee. Se un supporto di condivisione di sistema scrivibile esiste a /host, Save As... inizia lì; altrimenti la directory di log è il fallback. I file di sessione precedenti sono elencati solo in lettura.</translation>
    </message>
    <message>
        <source>Session logs</source>
        <translation>Tronchi di sessione</translation>
    </message>
    <message>
        <source>Session</source>
        <translation>Sessione</translation>
    </message>
    <message>
        <source>The first entry is the live session; earlier files in the log directory are listed read-only below it.</source>
        <translation>La prima voce è la sessione dal vivo; i file precedenti nella directory di registro sono elencati solo sotto di esso.</translation>
    </message>
    <message>
        <source>New Session Log</source>
        <translation>Nuova sessione</translation>
    </message>
    <message>
        <source>Close the active session file; it becomes a prior session and the next log entry starts a new file.</source>
        <translation>Chiudere il file di sessione attiva; diventa una sessione precedente e la successiva voce di registro inizia un nuovo file.</translation>
    </message>
    <message>
        <source>Add Note</source>
        <translation>Aggiungi nota</translation>
    </message>
    <message>
        <source>Append a NOTE entry to the live session register.</source>
        <translation>Appendere un NOTA iscrizione al registro di sessione dal vivo.</translation>
    </message>
    <message>
        <source>Delete</source>
        <translation>Cancella</translation>
    </message>
    <message>
        <source>Delete the selected prior session file (the live session is never deleted).</source>
        <translation>Eliminare il file di sessione precedente selezionato (la sessione in diretta non viene mai cancellata).</translation>
    </message>
    <message>
        <source>Refresh</source>
        <translation>Rifiuti</translation>
    </message>
    <message>
        <source>Save As...</source>
        <translation>Salva come...</translation>
    </message>
    <message>
        <source>Save the complete session log (all entries, not just the current filter).</source>
        <translation>Salvare il registro di sessione completo (tutte le voci, non solo il filtro corrente).</translation>
    </message>
    <message>
        <source>Clear Register</source>
        <translation>Cancella registro</translation>
    </message>
    <message>
        <source>Clear the live register and view; prior session files are never modified.</source>
        <translation>Cancellare il registro vivo e visualizzare; i file di sessione precedenti non sono mai modificati.</translation>
    </message>
    <message>
        <source>Search log:</source>
        <translation>Registro di ricerca:</translation>
    </message>
    <message>
        <source>Type any characters to show matching log entries (case-insensitive). Save As always writes every entry.</source>
        <translation>Digitare qualsiasi carattere per visualizzare le voci di registro corrispondenti (case-insensibili). Salva Come sempre scrive ogni voce.</translation>
    </message>
    <message>
        <source>Filter:</source>
        <translation>Filtro:</translation>
    </message>
    <message>
        <source>Filter the visible log by entry kind. Selecting a diagnostic section shows the lines captured for that section; a workflow filter such as File system repair or Package repair shows its mapped repair lines (File copy has no lines in this frontend). Save As always writes every entry.</source>
        <translation>Filtra il registro visibile per tipo di entrata. La selezione di una sezione diagnostica mostra le linee catturate per quella sezione; un filtro del flusso di lavoro come la riparazione del file system o la riparazione del pacchetto mostra le sue linee di riparazione mappate (la copia del file non ha linee in questo frontend). Salva Come sempre scrive ogni voce.</translation>
    </message>
    <message>
        <source>Settings are stored per user under ~/.qt/, one file per settings group (devicesrc, logsrc, diagnosticsrc, repairrc), and are saved immediately on every change and on close. Launch the GUI as the same user to keep your overrides; a GUI started as root keeps its own copies.</source>
        <translation>Le impostazioni vengono memorizzate per utente sotto ~/.qt/, un file per gruppo di impostazioni (devicesrc, logsrc, diagnosticasrc, Repairrc), e vengono salvate immediatamente su ogni cambiamento e in prossimità. Avviare la GUI come lo stesso utente per mantenere i tuoi overrides; una GUI ha iniziato come radice mantiene le proprie copie.</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Rilevamento dispositivi</translation>
    </message>
    <message>
        <source>Show devices without an identified Linux installation</source>
        <translation>Mostra dispositivi senza un&apos;installazione Linux identificata</translation>
    </message>
    <message>
        <source>Show removable and USB storage</source>
        <translation>Mostra archiviazione estraibile e USB</translation>
    </message>
    <message>
        <source>Show encrypted devices before unlocking</source>
        <translation>Mostra dispositivi crittografati prima di sbloccare</translation>
    </message>
    <message>
        <source>When off, drives without a visible Linux filesystem are hidden unless they still contain an encrypted device and encrypted devices are shown.</source>
        <translation>Quando è spento, le unità senza un filesystem Linux visibile sono nascoste a meno che non contengono ancora un dispositivo crittografato e i dispositivi crittografati vengono visualizzati.</translation>
    </message>
    <message>
        <source>When off, removable and USB drives are hidden from the repair-target list.</source>
        <translation>Quando è spento, le unità rimovibili e USB sono nascoste dall&apos;elenco di riparazione-target.</translation>
    </message>
    <message>
        <source>When off, drives with an encrypted device are hidden until the volume is unlocked.</source>
        <translation>Quando è spento, le unità con un dispositivo crittografato sono nascoste fino a quando il volume è sbloccato.</translation>
    </message>
    <message>
        <source>Include the %1 stage in the Full Repair plan. The stage runs in the plan order shown on the Repair tab.</source>
        <translation>Includere la fase %1 nel piano di riparazione completa. La fase viene eseguita nell&apos;ordine di piano mostrato nella scheda Riparazione.</translation>
    </message>
    <message>
        <source>Automatically regenerate read-only diagnostics after repairs or target changes</source>
        <translation>Rigenera automaticamente la diagnostica di sola lettura dopo le riparazioni o i cambiamenti di destinazione</translation>
    </message>
    <message>
        <source>Regenerates the cached read-only diagnostics for the current scope after an operation that invalidates them (LUKS unlock, target configuration edit). It runs only inside an already authorized administrator session and never opens an authorization prompt by itself.</source>
        <translation>Rigenera la diagnostica di sola lettura memorizzata nella cache per l&apos;ambito corrente dopo un&apos;operazione che li invalida (LUKS sblocca, modifica della configurazione di destinazione). Funziona solo all&apos;interno di una sessione di amministratore già autorizzata e non apre mai un prompt di autorizzazione da solo.</translation>
    </message>
    <message>
        <source>Wrap long log lines</source>
        <translation>Linee di registro lunghe</translation>
    </message>
    <message>
        <source>Mandatory safety controls</source>
        <translation>Controlli di sicurezza obbligatori</translation>
    </message>
    <message>
        <source>Host capabilities and dependencies</source>
        <translation>Capacità e dipendenze host</translation>
    </message>
    <message>
        <source>Refresh Capabilities</source>
        <translation>Capacità di aggiornamento</translation>
    </message>
    <message>
        <source>Re-run the read-only host capability probes (a PATH search, nothing is executed) and refresh the distribution summary.</source>
        <translation>Ripristina le sonde di sola capacità host (una ricerca PATH, nulla viene eseguito) e aggiorna il riepilogo della distribuzione.</translation>
    </message>
    <message>
        <source>Install Missing Support...</source>
        <translation>Installare il supporto mancante...</translation>
    </message>
    <message>
        <source>Automatic installation will require explicit package mapping and privilege authorization.</source>
        <translation>L&apos;installazione automatica richiederà una mappatura esplicita del pacchetto e l&apos;autorizzazione di privilegi.</translation>
    </message>
    <message>
        <source>Application configuration</source>
        <translation>Configurazione delle applicazioni</translation>
    </message>
    <message>
        <source>Boot Bitch Legacy</source>
        <translation>Boot Bitch Legacy</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list first.</source>
        <translation>Selezionare prima un&apos;unità fisica nell&apos;elenco dei target di riparazione disponibili.</translation>
    </message>
    <message>
        <source>Protected system</source>
        <translation>Sistema protetto</translation>
    </message>
    <message>
        <source>The running system cannot be selected as a repair target. Use Host Maintenance for the protected running host or choose another disk.</source>
        <translation>Il sistema in esecuzione non può essere selezionato come obiettivo di riparazione. Utilizzare la manutenzione host per l&apos;host corrente protetto o scegliere un altro disco.</translation>
    </message>
    <message>
        <source>Unlock or select a Linux system first</source>
        <translation>Sblocca o seleziona prima un sistema Linux</translation>
    </message>
    <message>
        <source>This encrypted drive has no visible Linux filesystem yet. Use Unlock, refresh devices, and select the target after its Linux root is detected.</source>
        <translation>Questa unità crittografata non ha ancora un filesystem Linux visibile. Usa Sblocca, aggiorna i dispositivi e seleziona l&apos;obiettivo dopo che la radice di Linux viene rilevata.</translation>
    </message>
    <message>
        <source>The selected root component (%1) belongs to the running system and cannot be committed as a repair target. Use Host Maintenance for the protected running host.</source>
        <translation>Il componente root selezionato (%1) appartiene al sistema in esecuzione e non può essere commesso come obiettivo di riparazione. Utilizzare Host Maintenance per l&apos;host in esecuzione protetto.</translation>
    </message>
    <message>
        <source>repair target commit</source>
        <translation>destinazione di riparazione commit</translation>
    </message>
    <message>
        <source>Repair drive selected: %1</source>
        <translation>Unità di riparazione selezionata: %1</translation>
    </message>
    <message>
        <source>; best detected system component: %1</source>
        <translation>; migliore componente di sistema rilevato: %1</translation>
    </message>
    <message>
        <source>. No mount or repair action was performed.</source>
        <translation>. Non è stata eseguita alcuna azione di montaggio o riparazione.</translation>
    </message>
    <message>
        <source>Make Default unavailable</source>
        <translation>Fare Predefinito non disponibile</translation>
    </message>
    <message>
        <source>Make Default is a running-host action on this frontend; enter Host Maintenance first.</source>
        <translation>Make Default è un&apos;azione in esecuzione-host su questo frontend; inserire prima Host Maintenance.</translation>
    </message>
    <message>
        <source>Administrator authorization required</source>
        <translation>Autorizzazione amministratore richiesta</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize first.</source>
        <translation>Nessuna sessione di amministratore è attiva; premere Autorizza prima.</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host?

The helper verifies /boot/grub/menu.lst, sets the `default &lt;N&gt;` directive to the canonical entry, backs the menu up first and restores it on any failure.</source>
        <translation>Rendere l&apos;ingresso del kernel installato canonico l&apos;ingresso di avvio di default GRUB-legacy sull&apos;host in esecuzione?

L&apos;aiutante verifica /boot/grub/menu.lst, imposta la direttiva `default &lt;N&gt;` all&apos;ingresso canonico, sostiene il menu in primo luogo e lo ripristina su qualsiasi fallimento.</translation>
    </message>
    <message>
        <source>Host Maintenance unavailable</source>
        <translation>Manutenzione Host non disponibile</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a committed repair target or a detected running host.</source>
        <translation>L&apos;obiettivo host in esecuzione non potrebbe essere rilevato; la diagnostica ha bisogno di un obiettivo di riparazione impegnato o di un host in esecuzione rilevato.</translation>
    </message>
    <message>
        <source>Exit Host Maintenance</source>
        <translation>Manutenzione host di uscita</translation>
    </message>
    <message>
        <source>Host Maintenance is active; diagnostics and gated repairs target the protected running host.</source>
        <translation>La manutenzione host è attiva; la diagnostica e le riparazioni gated mirano all&apos;host corrente protetto.</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list on the Systems tab first, or use Host Maintenance for the protected running host.</source>
        <translation>Selezionare un&apos;unità fisica nell&apos;elenco degli obiettivi di riparazione disponibili nella scheda Sistemi prima, o utilizzare Host Maintenance per l&apos;host in esecuzione protetto.</translation>
    </message>
    <message>
        <source>The selected drive (%1) is the protected running host. Choose Host Maintenance on the Systems tab to run read-only host diagnostics and guarded host repairs; ordinary target repairs stay disabled.</source>
        <translation>L&apos;unità selezionata (%1) è l&apos;host in esecuzione protetto. Scegliere Host Manutenzione sulla scheda Sistemi per eseguire la diagnostica host di sola lettura e le riparazioni host sorvegliate; le riparazioni di destinazione ordinarie rimanere disabilitate.</translation>
    </message>
    <message>
        <source>No repair target is committed. Choose Select Target on the Systems tab (or Host Maintenance for the protected running host) first.</source>
        <translation>Nessun obiettivo di riparazione è impegnato. Scegli Seleziona obiettivo sulla scheda Sistemi (o Manutenzione Host per l&apos;host in esecuzione protetto) prima.</translation>
    </message>
    <message>
        <source>The selection changed after the target was committed. Choose Select Target again.</source>
        <translation>La selezione è cambiata dopo che l&apos;obiettivo è stato commesso. Scegliere Seleziona destinazione di nuovo.</translation>
    </message>
    <message>
        <source>Diagnostics scope required</source>
        <translation>Campo diagnostico richiesto</translation>
    </message>
    <message>
        <source>Diagnostics scope unresolved</source>
        <translation>Scopo diagnostico non risolto</translation>
    </message>
    <message>
        <source>The running host target could not be resolved; use Refresh Devices and commit a repair target or re-enter Host Maintenance.</source>
        <translation>L&apos;obiettivo host in esecuzione non potrebbe essere risolto; utilizzare Refresh Devices e commettere un obiettivo di riparazione o rientro Host Maintenance.</translation>
    </message>
    <message>
        <source>Diagnostic check required</source>
        <translation>Controllo diagnostico richiesto</translation>
    </message>
    <message>
        <source>Select a diagnostic check in the list first.</source>
        <translation>Selezionare prima un controllo diagnostico nell&apos;elenco.</translation>
    </message>
    <message>
        <source>Repair target required</source>
        <translation>Obiettivo di riparazione richiesto</translation>
    </message>
    <message>
        <source>Target file editing needs a committed repair target. Running-host maintenance has no target-file editing; commit an offline target first.</source>
        <translation>L&apos;editing dei file di destinazione ha bisogno di un obiettivo di riparazione impegnato. La manutenzione dell&apos;host in esecuzione non ha alcuna modifica del file di destinazione; si impegna prima un target offline.</translation>
    </message>
    <message>
        <source>Configuration file required</source>
        <translation>File di configurazione richiesto</translation>
    </message>
    <message>
        <source>Select a target configuration file first.</source>
        <translation>Selezionare prima un file di configurazione di destinazione.</translation>
    </message>
    <message>
        <source>Edit target %1</source>
        <translation>Modifica obiettivo %1</translation>
    </message>
    <message>
        <source>Edit this target file through the guarded administrator helper. A successful save invalidates cached diagnostics; rerun diagnostics before repair. Generated files such as /boot/grub/menu.lst may be replaced by the next bootloader update.</source>
        <translation>Modificare questo file di destinazione tramite l&apos;helper dell&apos;amministratore protetto. Un salvataggio di successo invalida la diagnostica cache; eseguire la diagnostica prima della riparazione. I file generati come /boot/grub/menu.lst possono essere sostituiti dal prossimo aggiornamento del bootloader.</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Annulla</translation>
    </message>
    <message>
        <source>Save Target File</source>
        <translation>Salva il file di destinazione</translation>
    </message>
    <message>
        <source>No changes to %1.</source>
        <translation>Nessuna modifica a %1.</translation>
    </message>
    <message>
        <source>Configuration write refused</source>
        <translation>Configurazione scritta rifiutata</translation>
    </message>
    <message>
        <source>The edited content contains NUL bytes; the guarded write refuses it.</source>
        <translation>Il contenuto modificato contiene byte NUL; la scrittura custodita lo rifiuta.</translation>
    </message>
    <message>
        <source>File too large</source>
        <translation>File troppo grande</translation>
    </message>
    <message>
        <source>The edited file is larger than 1 MiB. The guarded write refuses it; edit the file from a console instead.</source>
        <translation>Il file modificato è più grande di 1 MiB. La scrittura custodita lo rifiuta; modifica invece il file da una console.</translation>
    </message>
    <message>
        <source>Write target configuration</source>
        <translation>Scrivi la configurazione di destinazione</translation>
    </message>
    <message>
        <source>Write the edited contents to %1? This modifies the repair target and invalidates cached diagnostics.</source>
        <translation>Scrivi il contenuto modificato a %1? Questo modifica il bersaglio di riparazione e invalida la diagnostica cache.</translation>
    </message>
    <message>
        <source>No diagnostic results to copy yet.</source>
        <translation>Nessun risultato diagnostico da copiare ancora.</translation>
    </message>
    <message>
        <source>Diagnostic results copied to the clipboard.</source>
        <translation>Risultati diagnostici copiati al tabellone.</translation>
    </message>
    <message>
        <source>No diagnostic results to save yet.</source>
        <translation>Nessun risultato diagnostico per salvare ancora.</translation>
    </message>
    <message>
        <source>Text files (*.txt);;All files (*)</source>
        <translation>File di testo (*.txt);; Tutti i file (*)</translation>
    </message>
    <message>
        <source>Save diagnostic results</source>
        <translation>Salvare i risultati diagnostici</translation>
    </message>
    <message>
        <source>Could not write %1.</source>
        <translation>Non poteva scrivere %1.</translation>
    </message>
    <message>
        <source>Diagnostic results saved to %1</source>
        <translation>Risultati diagnostici salvati su %1</translation>
    </message>
    <message>
        <source>Unlock not available</source>
        <translation>Sbloccaggio non disponibile</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked. Select an offline repair target to unlock.</source>
        <translation>L&apos;host corrente protetto non può essere sbloccato. Selezionare un obiettivo di riparazione offline per sbloccare.</translation>
    </message>
    <message>
        <source>No locked LUKS component is currently visible on this selected drive.</source>
        <translation>Nessun componente LUKS bloccato è attualmente visibile su questa unità selezionata.</translation>
    </message>
    <message>
        <source>Confirm LUKS unlock</source>
        <translation>Confermare LUKS sbloccare</translation>
    </message>
    <message>
        <source>Unlock %1 on %2?

The helper opens a temporary device-mapper mapping with cryptsetup and keeps it open for this recovery session. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>Sblocca %1 su %2?

L&apos;aiutante apre una mappatura temporanea con cryptsetup e la mantiene aperta per questa sessione di recupero. La passphrase viaggia attraverso un keyfile privato e non viene mai posta in argomenti di comando o log.</translation>
    </message>
    <message>
        <source>LUKS unlock</source>
        <translation>LUKS sbloccare</translation>
    </message>
    <message>
        <source>Unlock LUKS repair target</source>
        <translation>Sblocca il bersaglio di riparazione LUKS</translation>
    </message>
    <message>
        <source>Enter the passphrase for %1.

It is sent only to cryptsetup over the helper&apos;s standard input and is never logged or placed on a command line.</source>
        <translation>Inserisci il passphrase per %1.

Viene inviato solo a cryptsetup sopra l&apos;ingresso standard dell&apos;aiutante e non viene mai registrato o inserito su una riga di comando.</translation>
    </message>
    <message>
        <source>Passphrase required</source>
        <translation>Passphrase richiesto</translation>
    </message>
    <message>
        <source>An empty passphrase was not submitted. Enter the LUKS passphrase or choose Cancel.</source>
        <translation>Un passphrase vuoto non è stato presentato. Inserire la passphrase LUKS o scegliere Annulla.</translation>
    </message>
    <message>
        <source>Unlock keyfile unavailable</source>
        <translation>Sblocca il keyfile non disponibile</translation>
    </message>
    <message>
        <source>The LUKS passphrase could not be written to a private keyfile in %1; the unlock was not started.</source>
        <translation>Il passphrase LUKS non poteva essere scritto a un keyfile privato in %1; lo sblocco non era iniziato.</translation>
    </message>
    <message>
        <source>Configuration write unavailable</source>
        <translation>Configurazione non disponibile</translation>
    </message>
    <message>
        <source>The edited content could not be written to a private temporary file in %1; the write was not started.</source>
        <translation>Il contenuto modificato non poteva essere scritto in un file temporaneo privato in %1; la scrittura non era iniziata.</translation>
    </message>
    <message>
        <source>ERROR: cannot read the session log %1</source>
        <translation>ERROR: non è possibile leggere il registro di sessione %1</translation>
    </message>
    <message>
        <source>Viewing a prior session log (read-only): %1</source>
        <translation>Visualizzazione di un registro di sessione precedente (solo lettura): %1</translation>
    </message>
    <message>
        <source>Session log list refreshed.</source>
        <translation>Elenco dei registri di sessione aggiornato.</translation>
    </message>
    <message>
        <source>Note:</source>
        <translation>Nota:</translation>
    </message>
    <message>
        <source>Delete session log</source>
        <translation>Cancella il registro di sessione</translation>
    </message>
    <message>
        <source>Delete %1 permanently? This cannot be undone.</source>
        <translation>Eliminare %1 in modo permanente? Questo non può essere annullato.</translation>
    </message>
    <message>
        <source>%1 changed while the confirmation was open; the delete was refused.</source>
        <translation>%1 è cambiato mentre la conferma è stata aperta; l&apos;eliminazione è stata rifiutata.</translation>
    </message>
    <message>
        <source>Unable to delete %1.</source>
        <translation>Non è possibile eliminare %1.</translation>
    </message>
    <message>
        <source>Device discovery filters updated.</source>
        <translation>Filtri di scoperta del dispositivo aggiornati.</translation>
    </message>
    <message>
        <source>Unknown Linux distribution</source>
        <translation>Distribuzione Linux sconosciuta</translation>
    </message>
    <message>
        <source>sudo / gksu (no KAuth on this frontend)</source>
        <translation>sudo / gksu (no KAuth su questo frontend)</translation>
    </message>
    <message>
        <source>Available</source>
        <translation>Disponibile</translation>
    </message>
    <message>
        <source>Missing on this frontend</source>
        <translation>Mancando su questo frontend</translation>
    </message>
    <message>
        <source>Missing</source>
        <translation>Mancato</translation>
    </message>
    <message>
        <source>No repair tool is selected.</source>
        <translation>Nessun strumento di riparazione è selezionato.</translation>
    </message>
    <message>
        <source>A helper command is already running.</source>
        <translation>Un comando helper è già in esecuzione.</translation>
    </message>
    <message>
        <source>Restore Graphical Login is a host-scope stage on this legacy frontend; enter Host Maintenance to run it.</source>
        <translation>Restore Graphical Login è una fase host-scope su questo frontend legacy; inserire Host Maintenance per eseguirlo.</translation>
    </message>
    <message>
        <source>The selected scope has no resolved root component; use Refresh Devices and commit the repair target again.</source>
        <translation>L&apos;ambito selezionato non ha componenti root risolti; utilizzare Refresh Devices e commettere nuovamente l&apos;obiettivo di riparazione.</translation>
    </message>
    <message>
        <source>This legacy frontend exposes no %1 action; the helper reports the capability as available.</source>
        <translation>Questo frontend legacy non espone alcuna azione %1; l&apos;aiutante segnala la capacità come disponibile.</translation>
    </message>
    <message>
        <source>Run this guarded repair action using the cached read-only diagnostic evidence. A confirmation is shown first.</source>
        <translation>Eseguire questa azione di riparazione protetta utilizzando le prove diagnostiche di sola lettura cache. Una conferma viene mostrata prima.</translation>
    </message>
    <message>
        <source>Enabled in Settings</source>
        <translation>Attivato in Impostazioni</translation>
    </message>
    <message>
        <source>Disabled in Settings - enable it to include this stage</source>
        <translation>Disabili in Impostazioni - consente di includere questa fase</translation>
    </message>
    <message>
        <source>Unavailable: %1</source>
        <translation>Non disponibile: %1</translation>
    </message>
    <message>
        <source>
Unavailable: %1</source>
        <translation>
Non disponibile: %1</translation>
    </message>
    <message>
        <source>Repair tool unavailable</source>
        <translation>Strumento di riparazione non disponibile</translation>
    </message>
    <message>
        <source>Confirm repair</source>
        <translation>Conferma la riparazione</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Your saved selection is kept and its availability is re-checked when diagnostics for this scope complete.</source>
        <translation>Non ci sono ancora prove di capacità memorizzate per questa fase. La selezione salvata viene mantenuta e la sua disponibilità viene ricontrollata quando la diagnostica per questo scopo è completa.</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Run diagnostics for the selected scope to populate the Full Repair plan; your selection is saved once the stage becomes available.</source>
        <translation>Non ci sono ancora prove di capacità memorizzate per questa fase. Eseguire la diagnostica per il campo selezionato per popolare il piano di riparazione completa; la selezione viene salvata una volta che la fase diventa disponibile.</translation>
    </message>
    <message>
        <source>Unknown Full Repair stage.</source>
        <translation>Sconosciuto fase di riparazione completa.</translation>
    </message>
    <message>
        <source>No Full Repair stages are selected or available; use Configure Plan... to choose the stages.</source>
        <translation>Non sono selezionate o disponibili le fasi di Riparazione completa; utilizzare Configure Plan... per scegliere le fasi.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab to establish the session.</source>
        <translation>È richiesta l&apos;autorizzazione dell&apos;amministratore; premere Autorizza sulla scheda Sistemi o Riparazione per stabilire la sessione.</translation>
    </message>
    <message>
        <source>Runs the selected stages using the cached read-only diagnostic evidence after privilege confirmation.</source>
        <translation>Esegue le fasi selezionate utilizzando le prove diagnostiche di sola lettura memorizzate nella cache dopo la conferma del privilegio.</translation>
    </message>
    <message>
        <source>No Full Repair stages selected - use Configure Plan... or Settings.</source>
        <translation>Nessuna fase di Riparazione completa selezionata - utilizzare Configure Plan... o Impostazioni.</translation>
    </message>
    <message>
        <source>1 stage selected</source>
        <translation>1 fase selezionata</translation>
    </message>
    <message>
        <source>%1 stages selected</source>
        <translation>Stadi %1 selezionati</translation>
    </message>
    <message>
        <source>No repair stages are selected. Use Configure Plan... to choose the stages Full Repair will run.</source>
        <translation>Nessuna fase di riparazione è selezionata. Utilizzare Configure Plan... per scegliere le fasi Full Repair verrà eseguito.</translation>
    </message>
    <message>
        <source>No selected stages are available. %1</source>
        <translation>Non sono disponibili tappe selezionate. %1</translation>
    </message>
    <message>
        <source>Full Repair unavailable</source>
        <translation>Riparazione completa non disponibile</translation>
    </message>
    <message>
        <source>Run the Full Repair plan?

The selected stages run in order through the helper&apos;s guarded repair command:

</source>
        <translation>Eseguire il piano di riparazione completa?

Le fasi selezionate vengono eseguite in ordine tramite il comando di riparazione custodito dell&apos;aiutante:

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
L&apos;aiutante mantiene ogni preflight runtime; uno stadio che non riesce a fermare il piano.</translation>
    </message>
    <message>
        <source>Running %1 through the privileged helper... The Logs tab keeps the complete transcript.</source>
        <translation>Eseguire %1 attraverso l&apos;aiuto privilegiato... La scheda Logs mantiene la trascrizione completa.</translation>
    </message>
    <message>
        <source>Shell scope required</source>
        <translation>Campo di applicazione Shell richiesto</translation>
    </message>
    <message>
        <source>No administrator session is active.

Press Authorize on the Systems or Repair tab to establish the session, or enter Host Maintenance / commit a repair target on the Systems tab; the chroot shell then reuses the cached authorization.</source>
        <translation>Nessuna sessione di amministratore è attiva.

Premere Autorizza sulla scheda Sistemi o Riparazioni per stabilire la sessione, o inserire Manutenzione host / effettuare un obiettivo di riparazione sulla scheda Sistemi; la shell chroot quindi riutilizza l&apos;autorizzazione cache.</translation>
    </message>
    <message>
        <source>Shell command required</source>
        <translation>comando Shell richiesto</translation>
    </message>
    <message>
        <source>Enter the command to run first.</source>
        <translation>Inserisci il comando per eseguire prima.</translation>
    </message>
    <message>
        <source>Confirm running-host command</source>
        <translation>Confermare il comando run-host</translation>
    </message>
    <message>
        <source>Run this command as root on the protected running host?

%1

The helper keeps its runtime preflights; the command is passed as one argument and is never interpreted by the GUI.</source>
        <translation>Eseguire questo comando come root sull&apos;host corrente protetto?

%1

L&apos;aiutante mantiene i suoi preflight runtime; il comando viene passato come un argomento e non viene mai interpretato dalla GUI.</translation>
    </message>
    <message>
        <source>Running %1...</source>
        <translation>Correre %1...</translation>
    </message>
    <message>
        <source>Authorization scope required</source>
        <translation>Campo di applicazione obbligatorio</translation>
    </message>
    <message>
        <source>administrator authorization</source>
        <translation>autorizzazione dell&apos;amministratore</translation>
    </message>
    <message>
        <source>Administrator authorization</source>
        <translation>Autorizzazione amministratore</translation>
    </message>
    <message>
        <source>Administrator authorization is required for %1.

Enter the password for %2 (sudo). It is used only for this sudo authentication, is sent over a pipe and is never logged or placed on a command line. The authorization is cached for this session and reused by diagnostics and repairs.</source>
        <translation>L&apos;autorizzazione dell&apos;amministratore è richiesta per %1.

Inserisci la password per %2 (sudo). Viene utilizzato solo per questa autenticazione sudo, viene inviato su un tubo e non viene mai registrato o inserito su una riga di comando. L&apos;autorizzazione viene memorizzata in cache per questa sessione e riutilizzata da diagnostica e riparazioni.</translation>
    </message>
    <message>
        <source>your account</source>
        <translation>il tuo account</translation>
    </message>
    <message>
        <source>Password required</source>
        <translation>Password richiesta</translation>
    </message>
    <message>
        <source>An empty password was not submitted. Enter the sudo password or choose Cancel.</source>
        <translation>Una password vuota non è stata presentata. Inserisci la password sudo o scegli Annulla.</translation>
    </message>
    <message>
        <source>Administrator authorization failed</source>
        <translation>L&apos;autorizzazione dell&apos;amministratore è fallita</translation>
    </message>
    <message>
        <source>sudo did not accept the password: %1

The command was not started.</source>
        <translation>sudo non ha accettato la password: %1

Il comando non è stato avviato.</translation>
    </message>
    <message>
        <source>elevation requires an interactive sudo password; run the smoke as root or after `sudo -S -v` with --elevate &apos;sudo -n&apos;</source>
        <translation>elevazione richiede una password sudo interattiva; eseguire il fumo come radice o dopo `sudo -S -v` con --elevate &apos;sudo -n&apos;</translation>
    </message>
    <message>
        <source>No administrator session is active for %1.

Press Authorize on the Systems or Repair tab to establish the session now, or enter Host Maintenance / commit a repair target on the Systems tab; diagnostics and repairs then reuse the cached authorization.</source>
        <translation>Nessuna sessione di amministratore è attiva per %1.

Premere Autorizza sulla scheda Sistemi o Riparazioni per stabilire la sessione ora, o inserire Manutenzione host / effettuare un obiettivo di riparazione sulla scheda Sistemi; diagnostica e riparazioni quindi riutilizzare l&apos;autorizzazione cache.</translation>
    </message>
    <message>
        <source>Administrator authorization expired</source>
        <translation>Autorizzazione amministratore scaduta</translation>
    </message>
    <message>
        <source>The cached administrator authorization for %1 expired or was refused.

Press Authorize on the Systems or Repair tab to re-establish the session, then run the command again. No command was started.</source>
        <translation>L&apos;autorizzazione dell&apos;amministratore cache per %1 è scaduta o è stata rifiutata.

Premere Autorizza sulla scheda Sistemi o Riparazioni per ripristinare la sessione, quindi eseguire nuovamente il comando. Nessun comando è stato avviato.</translation>
    </message>
    <message>
        <source>Privileged operation completed successfully. Administrator authorization remains active for this session.</source>
        <translation>Funzionamento privilegiato completato con successo. L&apos;autorizzazione dell&apos;amministratore rimane attiva per questa sessione.</translation>
    </message>
    <message>
        <source>Privileged operation stopped with an error. Administrator authorization remains active; review the output before closing.</source>
        <translation>L&apos;operazione Privileged si è fermata con un errore. L&apos;autorizzazione dell&apos;amministratore rimane attiva; controlla l&apos;output prima della chiusura.</translation>
    </message>
    <message>
        <source>Inspection is read-only.

</source>
        <translation>L&apos;ispezione è di sola lettura.

</translation>
    </message>
    <message>
        <source>Configuration unavailable</source>
        <translation>Configurazione non disponibile</translation>
    </message>
    <message>
        <source>The helper could not read %1:

%2</source>
        <translation>L&apos;aiutante non poteva leggere %1:

%2</translation>
    </message>
    <message>
        <source>Configuration write failed</source>
        <translation>La scrittura di configurazione fallita</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
Result: mapping opened for this recovery session.</source>
        <translation>Stato: sbloccato
Componente: %1
Mapper: %2
Metodo: helper sbloccare (cryptsetup; passphrase tramite un file di tasti mode-600, cancellato dopo l&apos;uso)
Risultato: mappatura aperta per questa sessione di recupero.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: the passphrase was not accepted; retry offered.</source>
        <translation>Stato: chiuso
Componente: %1
Metodo: helper sbloccare (cryptsetup)
Errore: la passphrase non è stata accettata; riprova offerta.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: %2</source>
        <translation>Stato: chiuso
Componente: %1
Metodo: helper sbloccare (cryptsetup)
Errore: %2</translation>
    </message>
    <message>
        <source>Passphrase not accepted</source>
        <translation>Passphrase non accettato</translation>
    </message>
    <message>
        <source>The LUKS passphrase was not accepted.

Try again?</source>
        <translation>La passphrase LUKS non è stata accettata.

Riprova?</translation>
    </message>
    <message>
        <source>[-] %1 - not run (the plan stopped before reaching this stage)</source>
        <translation>[-] %1 - non eseguire (il piano si è fermato prima di raggiungere questa fase)</translation>
    </message>
    <message>
        <source>filesystem</source>
        <translation>file system</translation>
    </message>
    <message>
        <source>[-] %1 - no file system errors found - no changes</source>
        <translation>[-] %1 - nessun errore di file system trovato - no modifiche</translation>
    </message>
    <message>
        <source>[FAIL] %1 - not reported</source>
        <translation>[FAIL] %1 - non segnalato</translation>
    </message>
    <message>
        <source>%1 stage(s) failed; review the helper output in Logs.</source>
        <translation>%1 stadio(s) fallito; rivedere l&apos;uscita helper in Logs.</translation>
    </message>
    <message>
        <source>the plan stopped before any stage completed.</source>
        <translation>il piano si è fermato prima di qualsiasi fase completata.</translation>
    </message>
    <message>
        <source>no repair was needed; cached diagnostics remain valid.</source>
        <translation>nessuna riparazione era necessaria; la diagnostica cache rimane valida.</translation>
    </message>
    <message>
        <source>repair completed; cached diagnostics were invalidated and must be regenerated.</source>
        <translation>riparazione completata; la diagnostica cache è stata invalidata e deve essere rigenerata.</translation>
    </message>
    <message>
        <source>%1 results: [OK] %2 successful | [FAIL] %3 failed | [-] %4 no repair needed - %5</source>
        <translation>Risultati %1: [OK] %2 successo | [FAIL] %3 fallito | [-] %4 nessuna riparazione necessaria - %5</translation>
    </message>
    <message>
        <source>Re-run Diagnostic</source>
        <translation>Diagnostica a ricircolo</translation>
    </message>
    <message>
        <source>Unavailable</source>
        <translation>Non disponibile</translation>
    </message>
    <message>
        <source>Commit this physical drive as the repair target.</source>
        <translation>Impedire questa unità fisica come obiettivo di riparazione.</translation>
    </message>
    <message>
        <source>Protection:</source>
        <translation>Protezione:</translation>
    </message>
    <message>
        <source>Host shell</source>
        <translation>shell host</translation>
    </message>
    <message>
        <source>Run on Host</source>
        <translation>Correre su Host</translation>
    </message>
    <message>
        <source>Host Shell</source>
        <translation>Host Shell</translation>
    </message>
    <message>
        <source>The read-only probe found no editable target configuration file in this target. %1</source>
        <translation>La sonda di sola lettura non ha trovato nessun file di configurazione di destinazione modificabile in questo obiettivo. %1</translation>
    </message>
    <message>
        <source>Not present in the selected target (omitted): %1</source>
        <translation>Non presente nel target selezionato (omesso): %1</translation>
    </message>
    <message>
        <source>Run diagnostics to probe which target configuration files exist; the helper&apos;s read-only probe decides the list.</source>
        <translation>Eseguire la diagnostica per sonda che i file di configurazione di destinazione esistono; la sonda di sola lettura dell&apos;aiutante decide l&apos;elenco.</translation>
    </message>
    <message>
        <source>Probed read-only by the helper; a saved edit invalidates the cached diagnostics.</source>
        <translation>Probed in sola lettura dall&apos;aiutante; una modifica salvata invalida la diagnostica cache.</translation>
    </message>
    <message>
        <source>Host maintenance: %1</source>
        <translation>Manutenzione host: %1</translation>
    </message>
    <message>
        <source>unresolved</source>
        <translation>irrisolto</translation>
    </message>
    <message>
        <source>Target: %1 + %2</source>
        <translation>Obiettivo: %1 + %2</translation>
    </message>
    <message>
        <source>Host command</source>
        <translation>Comando host</translation>
    </message>
    <message>
        <source>Execute a command on the running host as root.</source>
        <translation>Eseguire un comando sull&apos;host in esecuzione come root.</translation>
    </message>
    <message>
        <source>Execute a command inside the selected repair system as root.</source>
        <translation>Eseguire un comando all&apos;interno del sistema di riparazione selezionato come root.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab (or re-enter Host Maintenance / re-commit the repair target) to authorize this session.</source>
        <translation>È richiesta l&apos;autorizzazione dell&apos;amministratore; premere Autorizza sulla scheda Sistemi o Riparazione (o rientro Host Maintenance / ricommettere l&apos;obiettivo di riparazione) per autorizzare questa sessione.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root on the running host through the helper&apos;s guarded host-shell verb.</source>
        <translation>Eseguire un comando recensito come root sull&apos;host in esecuzione attraverso il verbo host-shell protetto dell&apos;aiutante.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root inside the target chroot through the helper&apos;s guarded shell verb.</source>
        <translation>Eseguire un comando rivisto come root all&apos;interno del chroot di destinazione attraverso il verbo di shell protetto dell&apos;aiutante.</translation>
    </message>
    <message>
        <source>Run a command on the running host as root (sudo is not needed). Commands are executed directly on the active system; output is kept in this window and in the application log.</source>
        <translation>Eseguire un comando sull&apos;host in esecuzione come root (sudo non è necessario). I comandi vengono eseguiti direttamente sul sistema attivo; l&apos;output viene mantenuto in questa finestra e nel registro delle applicazioni.</translation>
    </message>
    <message>
        <source>Run a command inside the selected repair system as root (sudo is not needed). Commands are executed one at a time in a fresh chroot and cannot answer interactive prompts; use non-interactive flags such as apt-get -y upgrade. Output is kept in this window and in the application log.</source>
        <translation>Eseguire un comando all&apos;interno del sistema di riparazione selezionato come root (sudo non è necessario). I comandi vengono eseguiti uno alla volta in un chroot fresco e non possono rispondere a richieste interattive; utilizzare bandiere non interattive come l&apos;aggiornamento apt-get-y. L&apos;uscita è mantenuta in questa finestra e nel registro delle applicazioni.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize or re-enter Host Maintenance / commit a repair target.</source>
        <translation>Nessuna sessione di amministratore è attiva; premere Autorizzazione o rientro Host Maintenance / effettuare un obiettivo di riparazione.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from the repaired system</source>
        <translation>1. Selezionare i file di origine o le cartelle dal sistema riparato</translation>
    </message>
    <message>
        <source>2. Choose destination on this host</source>
        <translation>2. Scegli la destinazione su questo host</translation>
    </message>
    <message>
        <source>Add File Path...</source>
        <translation>Aggiungi File Path...</translation>
    </message>
    <message>
        <source>Add Folder Path...</source>
        <translation>Aggiungi percorso cartella...</translation>
    </message>
    <message>
        <source>Browse...</source>
        <translation>Sfoglia...</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it (Host Maintenance does not provide a repair tree to browse).</source>
        <translation>Selezionare l&apos;unità di riparazione in Sistemi prima di scegliere una destinazione all&apos;interno di esso (Host Maintenance non fornisce un albero di riparazione da navigare).</translation>
    </message>
    <message>
        <source>Choose a host destination folder directly.</source>
        <translation>Scegli una cartella di destinazione host direttamente.</translation>
    </message>
    <message>
        <source>Copy the staged files and folders with cp -a, restore ownership with chown --reference and byte-compare every regular file afterwards.</source>
        <translation>Copiare i file e le cartelle in fase con cp -a, ripristinare la proprietà con chown --referenza e byte-compare ogni file regolare dopo.</translation>
    </message>
    <message>
        <source>File Copy unavailable</source>
        <translation>Copia file non disponibile</translation>
    </message>
    <message>
        <source>Choose host destination folder</source>
        <translation>Scegli la cartella di destinazione host</translation>
    </message>
    <message>
        <source>Select a repair target</source>
        <translation>Selezionare un obiettivo di riparazione</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it.</source>
        <translation>Selezionare l&apos;unità di riparazione in Sistemi prima di scegliere una destinazione all&apos;interno di esso.</translation>
    </message>
    <message>
        <source>File Copy - Browse Target Folders</source>
        <translation>Copia file - Sfoglia le cartelle di destinazione</translation>
    </message>
    <message>
        <source>Browse Target Folders</source>
        <translation>Sfoglia le cartelle di destinazione</translation>
    </message>
    <message>
        <source>The helper could not list the repair-system folder:

%1</source>
        <translation>L&apos;aiutante non poteva elencare la cartella del sistema di riparazione:

%1</translation>
    </message>
    <message>
        <source>BROWSE_ENTRY</source>
        <translation>BROWSEY:</translation>
    </message>
    <message>
        <source>(choose this folder: %1)</source>
        <translation>(scelga questa cartella: %1)</translation>
    </message>
    <message>
        <source>(parent folder)</source>
        <translation>(cartella dei genitori)</translation>
    </message>
    <message>
        <source>Select repair-system destination</source>
        <translation>Selezionare la destinazione del sistema di riparazione</translation>
    </message>
    <message>
        <source>Choose a destination folder inside the repaired system (current folder: %1):</source>
        <translation>Scegliere una cartella di destinazione all&apos;interno del sistema riparato (cartella corrente: %1):</translation>
    </message>
    <message>
        <source>Folder</source>
        <translation>Cartella</translation>
    </message>
    <message>
        <source>Open</source>
        <translation>Apri</translation>
    </message>
    <message>
        <source>(choose this folder:</source>
        <translation>(scelga questa cartella:</translation>
    </message>
    <message>
        <source>Add files to copy</source>
        <translation>Aggiungi file per copiare</translation>
    </message>
    <message>
        <source>Add folder to copy</source>
        <translation>Aggiungi cartella per copiare</translation>
    </message>
    <message>
        <source>Stage at least one source and name a destination path first.</source>
        <translation>Fase almeno una fonte e nominare un percorso di destinazione prima.</translation>
    </message>
    <message>
        <source>Copy the %1 staged item(s) to %2?

The helper keeps its direction and path containment checks; a sensitive repair-system destination is refused unless the helper approves it, and every regular file is byte-compared after the copy.</source>
        <translation>Copiare il %1 oggetti in scena a %2?

L&apos;aiutante mantiene i controlli di contenimento della direzione e del percorso; una destinazione sensibile del sistema di riparazione viene rifiutata a meno che l&apos;aiutante non lo approvi, e ogni file regolare è byte-compared dopo la copia.</translation>
    </message>
    <message>
        <source>File Copy - Copy and Verify</source>
        <translation>Copia file - Copia e verifica</translation>
    </message>
    <message>
        <source>File Copy - Preview Changes</source>
        <translation>Copia file - Anteprima modifiche</translation>
    </message>
    <message>
        <source>Protected running host - details</source>
        <translation>Host corrente protetta - dettagli</translation>
    </message>
    <message>
        <source>not mounted (offline target)</source>
        <translation>non montato (obiettivo offline)</translation>
    </message>
    <message>
        <source>Select a drive to see its details.</source>
        <translation>Selezionare un&apos;unità per vedere i suoi dettagli.</translation>
    </message>
    <message>
        <source>Inspecting the selected component.</source>
        <translation>Ispezione del componente selezionato.</translation>
    </message>
    <message>
        <source>Helper-confirmed by the last read-only diagnostics.</source>
        <translation>Assistente confermato dall&apos;ultima diagnostica di sola lettura.</translation>
    </message>
    <message>
        <source>Read-only inventory only; run diagnostics to confirm.</source>
        <translation>Solo inventario di sola lettura; eseguire la diagnostica per confermare.</translation>
    </message>
    <message>
        <source>PROTECTED - running system; read-only details only</source>
        <translation>PROTECTED - sistema di esecuzione; solo dettagli di sola lettura</translation>
    </message>
    <message>
        <source>Live / installer media - not selectable</source>
        <translation>Live / installer media - non selezionabile</translation>
    </message>
    <message>
        <source>PROTECTED - running host scope</source>
        <translation>PROTECTED - campo host in esecuzione</translation>
    </message>
    <message>
        <source>Unlock required before selection</source>
        <translation>Sblocco richiesto prima della selezione</translation>
    </message>
    <message>
        <source>Eligible repair candidate</source>
        <translation>Eleggibile candidato alla riparazione</translation>
    </message>
    <message>
        <source>Drive:</source>
        <translation>Guida:</translation>
    </message>
    <message>
        <source>Detected target:</source>
        <translation>Obiettivo individuato:</translation>
    </message>
    <message>
        <source>Pending inspection</source>
        <translation>Ispezione dei prestiti</translation>
    </message>
    <message>
        <source>Model / label:</source>
        <translation>Modello / etichetta:</translation>
    </message>
    <message>
        <source>Status:</source>
        <translation>Stato:</translation>
    </message>
    <message>
        <source>Size:</source>
        <translation>Dimensioni:</translation>
    </message>
    <message>
        <source>Connection:</source>
        <translation>Connessione:</translation>
    </message>
    <message>
        <source>Filesystem:</source>
        <translation>Filesystem:</translation>
    </message>
    <message>
        <source>UUID:</source>
        <translation>UUID:</translation>
    </message>
    <message>
        <source>Mounts:</source>
        <translation>Montanti:</translation>
    </message>
    <message>
        <source>Running system protection unresolved</source>
        <translation>Protezione del sistema in esecuzione irrisolta</translation>
    </message>
    <message>
        <source>No protected physical backing disk was identified</source>
        <translation>Nessun disco di supporto fisico protetto è stato identificato</translation>
    </message>
    <message>
        <source>Current running Linux system</source>
        <translation>Sistema Linux corrente</translation>
    </message>
    <message>
        <source>Critical mounts: %1</source>
        <translation>Supporti critici: %1</translation>
    </message>
    <message>
        <source>Read-only protected-running-system facts: the helper OS fact plus the inventory model, device path, size, transport and critical mounts. Nothing here is probed destructively.</source>
        <translation>Fatti del sistema di gestione protetta: il fatto del sistema operativo helper più il modello di inventario, il percorso del dispositivo, le dimensioni, il trasporto e i supporti critici. Qui non c&apos;è nulla di distruttivo.</translation>
    </message>
    <message>
        <source>Select a drive to see unlock status.</source>
        <translation>Selezionare un&apos;unità per vedere lo stato di sblocco.</translation>
    </message>
    <message>
        <source>State: protected
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
The protected running host cannot be unlocked or modified; unlock is available for an offline repair target only. Use Host Maintenance for the protected running host.</source>
        <translation>Stato: protetto
Componente: %1
Mapper: (none)
Metodo: helper sbloccare (cryptsetup; passphrase tramite un file di tasti mode-600, cancellato dopo l&apos;uso)
L&apos;host in esecuzione protetto non può essere sbloccato o modificato; lo sblocco è disponibile solo per un obiettivo di riparazione offline. Utilizzare Host Maintenance per l&apos;host in esecuzione protetto.</translation>
    </message>
    <message>
        <source>(none detected)</source>
        <translation>(non rilevato)</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
A locked LUKS container is visible on this drive; press Unlock to open it for this recovery session.</source>
        <translation>Stato: chiuso
Componente: %1
Mapper: (none)
Metodo: helper sbloccare (cryptsetup; passphrase tramite un file di tasti mode-600, cancellato dopo l&apos;uso)
Un contenitore LUKS bloccato è visibile su questa unità; premere Sblocco per aprirlo per questa sessione di recupero.</translation>
    </message>
    <message>
        <source>(visible mapper)</source>
        <translation>(mamma visibile)</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: already open before this session (visible mapper; Boot Bitch will reuse it and will not close it).</source>
        <translation>Stato: sbloccato
Componente: %1
Mapper: %2
Metodo: già aperto prima di questa sessione (mapper visibile; Boot Bitch lo riutilizza e non lo chiuderà).</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper-confirmed mapping from the last read-only diagnostics.</source>
        <translation>Stato: sbloccato
Componente: %1
Mapper: %2
Metodo: mappatura confermata dall&apos;ultimo diagnostica di sola lettura.</translation>
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
        <translation>Stato: bloccato o nessun componente crittografato rilevato
Componente: (non rilevato)
Mapper: (none)
Metodo: helper sbloccare (cryptsetup; passphrase tramite un file di tasti mode-600, cancellato dopo l&apos;uso)
Nessun componente LUKS bloccato e nessuna operazione di sblocco sono stati registrati per questa unità nella sessione corrente.</translation>
    </message>
    <message>
        <source>unavailable - %1</source>
        <translation>non disponibile - %1</translation>
    </message>
    <message>
        <source>Host maintenance:
%1</source>
        <translation>Manutenzione host:
%1</translation>
    </message>
    <message>
        <source>Target:
%1 + %2</source>
        <translation>Obiettivo:
%1 + %2</translation>
    </message>
    <message>
        <source>Authorization required: diagnostics and repairs fail closed until you press Authorize.</source>
        <translation>Autorizzazione necessaria: diagnostica e riparazioni falliscono chiuso fino a quando si preme Autorizzazione.</translation>
    </message>
    <message>
        <source>Re-establish the privileged helper session for the current scope now. The password is requested in the hidden-input modal and is never logged.</source>
        <translation>Ristabilire la sessione di helper privilegiata per l&apos;attuale ambito. La password è richiesta in modalità di input nascosto e non è mai registrata.</translation>
    </message>
    <message>
        <source>Running...</source>
        <translation>Correre...</translation>
    </message>
    <message>
        <source>Scope required</source>
        <translation>Campo di applicazione richiesto</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope; this unlocks the gated actions.</source>
        <translation>Eseguire Tutti - eseguire ogni diagnostica disponibile di sola lettura per la portata corrente; questo sblocca le azioni gated.</translation>
    </message>
    <message>
        <source>Select a target and wait for any running command first.</source>
        <translation>Selezionare un obiettivo e attendere prima qualsiasi comando in esecuzione.</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic through the helper.</source>
        <translation>Eseguire Diagnostic - eseguire la diagnostica di sola lettura selezionata tramite l&apos;helper.</translation>
    </message>
    <message>
        <source>Run All for the current scope and refresh the read-only backend profile.</source>
        <translation>Eseguire tutto per la portata corrente e aggiornare il profilo di backend di sola lettura.</translation>
    </message>
    <message>
        <source>Read or edit the selected target configuration file through the guarded helper; a saved edit invalidates cached diagnostics.</source>
        <translation>Leggere o modificare il file di configurazione selezionato tramite l&apos;helper protetto; una modifica salvata invalida la diagnostica cache.</translation>
    </message>
    <message>
        <source>Host Maintenance has no target-file editing; commit an offline repair target first.</source>
        <translation>Host Maintenance non ha alcun target-file di editing; si impegna prima un target di riparazione offline.</translation>
    </message>
    <message>
        <source>Commit an offline repair target first.</source>
        <translation>Impedire prima un bersaglio di riparazione offline.</translation>
    </message>
    <message>
        <source>No target configuration file is available for this target; run diagnostics to probe the list.</source>
        <translation>Nessun file di configurazione di destinazione è disponibile per questo obiettivo; eseguire la diagnostica per sondare l&apos;elenco.</translation>
    </message>
    <message>
        <source>Already Unlocked</source>
        <translation>Già sbloccato</translation>
    </message>
    <message>
        <source>An unlocked Linux filesystem is already visible on this drive. Boot Bitch will reuse the existing mapper and will not close or reopen a mapping created by this recovery session. Mounting happens during diagnostics (read-only) and repairs (read-write); data filesystems are never auto-mounted on selection.</source>
        <translation>Un filesystem Linux sbloccato è già visibile su questa unità. Boot Bitch riuserà il mapper esistente e non chiuderà o riaprirà una mappatura creata da questa sessione di recupero. Il montaggio avviene durante la diagnostica (solo lettura) e le riparazioni (let-write); i filesystem di dati non vengono mai montati automaticamente sulla selezione.</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked; use Host Maintenance for the protected running host.</source>
        <translation>L&apos;host in esecuzione protetto non può essere sbloccato; utilizzare Host Maintenance per l&apos;host in esecuzione protetto.</translation>
    </message>
    <message>
        <source>Host Maintenance is the current scope, but the selected offline drive can still be unlocked.</source>
        <translation>La manutenzione host è la portata corrente, ma l&apos;unità offline selezionata può ancora essere sbloccata.</translation>
    </message>
    <message>
        <source>Unlock %1 using cryptsetup through the privileged helper. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>Sbloccare %1 usando cryptsetup attraverso l&apos;aiutante privilegiato. La passphrase viaggia attraverso un keyfile privato e non viene mai posta in argomenti di comando o log.</translation>
    </message>
    <message>
        <source>The protected running system cannot be selected as a repair target; use Host Maintenance for the protected running host.</source>
        <translation>Il sistema di esecuzione protetto non può essere selezionato come obiettivo di riparazione; utilizzare Host Maintenance per l&apos;host in esecuzione protetto.</translation>
    </message>
    <message>
        <source>Live / installer media is read-only boot media and cannot be selected as a repair target.</source>
        <translation>Live / installer media è il supporto di avvio di sola lettura e non può essere selezionato come obiettivo di riparazione.</translation>
    </message>
    <message>
        <source>Unlock the encrypted volume first; Select Target becomes available after a Linux filesystem is detected.</source>
        <translation>Sblocca prima il volume crittografato; Select Target diventa disponibile dopo che viene rilevato un filesystem Linux.</translation>
    </message>
    <message>
        <source>Committed repair target. Repair, Diagnostics and File Copy target this physical drive until another drive is explicitly selected with Select Target.</source>
        <translation>Obiettivo di riparazione autorizzato. Riparazione, Diagnostica e Copia file bersaglio di questo disco fisico fino a quando un&apos;altra unità è esplicitamente selezionata con Select Target.</translation>
    </message>
    <message>
        <source>Commit %1 as the repair target; this leaves Host Maintenance and switches the scope to the selected drive.</source>
        <translation>Impegnare %1 come obiettivo di riparazione; questo lascia Host Maintenance e passa la portata all&apos;unità selezionata.</translation>
    </message>
    <message>
        <source>The running host target could not be detected.</source>
        <translation>L&apos;obiettivo host in esecuzione non potrebbe essere rilevato.</translation>
    </message>
    <message>
        <source>Leave Host Maintenance and return to repair-target mode.</source>
        <translation>Lasciare la manutenzione host e tornare alla modalità riparazione-target.</translation>
    </message>
    <message>
        <source>Select the running host for deliberate guarded maintenance; the administrator authorization is requested here once and cached for the session.</source>
        <translation>Selezionare l&apos;host in esecuzione per la manutenzione preventiva deliberata; l&apos;autorizzazione dell&apos;amministratore viene richiesta qui una volta e memorizzata per la sessione.</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a repair target.</source>
        <translation>L&apos;obiettivo host in esecuzione non potrebbe essere rilevato; la diagnostica ha bisogno di un obiettivo di riparazione.</translation>
    </message>
    <message>
        <source>Committed target: %1 + %2</source>
        <translation>Obiettivo: %1 + %2</translation>
    </message>
    <message>
        <source>Committed target: none (selection changed)</source>
        <translation>Obiettivo: nessuno (selezione cambiata)</translation>
    </message>
    <message>
        <source>The selected scope has no resolved Linux root component; Refresh Devices and commit the repair target again.</source>
        <translation>L&apos;ambito selezionato non ha un componente radice Linux risolto; Refresh Devices e committere nuovamente l&apos;obiettivo di riparazione.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize on the Systems or Repair tab to re-establish it. Run All reuses the cached authorization and never prompts by itself.</source>
        <translation>Nessuna sessione di amministratore è attiva; premere Autorizza sulla scheda Sistemi o Riparazione per ripristinarla. Eseguire Tutti riutilizza l&apos;autorizzazione cache e non richiede mai da solo.</translation>
    </message>
    <message>
        <source>Run diagnostics for this scope to unlock the gated actions. Diagnostics are read-only and the only evidence source.</source>
        <translation>Eseguire la diagnostica per questo scopo per sbloccare le azioni gated. I sistemi diagnostici sono di sola lettura e l&apos;unica fonte di prova.</translation>
    </message>
    <message>
        <source>A repair was not proven unchanged, so the cached diagnostics are invalidated. Run diagnostics again before another gated action.</source>
        <translation>Una riparazione non è stata dimostrata invariata, quindi la diagnostica cache è invalidata. Eseguire la diagnostica di nuovo prima di un&apos;altra azione gated.</translation>
    </message>
    <message>
        <source>Gated actions reflect the cached capability lines; the helper still runs every runtime preflight when a command starts.</source>
        <translation>Le azioni raccolte riflettono le linee di funzionalità cache; l&apos;aiutante esegue ancora ogni preflight runtime quando inizia un comando.</translation>
    </message>
    <message>
        <source>Regenerating diagnostics automatically</source>
        <translation>Rigenerazione automatica della diagnostica</translation>
    </message>
    <message>
        <source>Running diagnostic: %1</source>
        <translation>Diagnostica in esecuzione: %1</translation>
    </message>
    <message>
        <source>Running all diagnostics</source>
        <translation>Eseguire tutte le diagnostica</translation>
    </message>
    <message>
        <source>Unlocking %1</source>
        <translation>Sbloccaggio %1</translation>
    </message>
    <message>
        <source>running host</source>
        <translation>host in esecuzione</translation>
    </message>
    <message>
        <source>offline target</source>
        <translation>target offline</translation>
    </message>
    <message>
        <source>host maintenance active</source>
        <translation>manutenzione host attiva</translation>
    </message>
    <message>
        <source>repair target committed</source>
        <translation>obiettivo di riparazione impegnato</translation>
    </message>
    <message>
        <source>no committed scope</source>
        <translation>nessuna portata impegnata</translation>
    </message>
    <message>
        <source>Current session</source>
        <translation>Sessione attuale</translation>
    </message>
    <message>
        <source>Log files (*.log);;All files (*)</source>
        <translation>File di registro (*.log);; Tutti i file (*)</translation>
    </message>
    <message>
        <source>Save log as</source>
        <translation>Salvare il log come</translation>
    </message>
    <message>
        <source>About Boot Bitch</source>
        <translation>Informazioni su Boot Bitch</translation>
    </message>
    <message>
        <source>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;&lt;p&gt;&lt;b&gt;Developer:&lt;/b&gt; CaptainMorgan12&lt;/p&gt;&lt;p&gt;This GUI is the package&apos;s entry point: a Qt 3.3.x frontend with the ported guarded helper for Debian Etch-era systems.&lt;/p&gt;&lt;p&gt;&lt;b&gt;Guarded repair mode:&lt;/b&gt; read-only diagnostics can inspect either the protected Running Host or an explicitly selected repair drive. The guarded Debian/APT package stages (interrupted configuration, broken dependencies, metadata refresh, upgrade), GRUB-legacy configuration regeneration, LUKS target unlock and guarded target-file editing run through the ported helper after confirmation; Host Maintenance enables the same supported stages natively on the active system after repeating the host identity and boot-mount checks.&lt;/p&gt;&lt;p&gt;The modern-only features - verified File Copy, Btrfs snapshot rollback, EFI/UKI and extlinux repair, boot-stack reconciliation and Make Default - are greyed with the helper&apos;s own probe reasons on this frontend; Arch/Alpine/Fedora package backends stay diagnostics-only here.&lt;/p&gt;&lt;p&gt;The first privileged action authorizes one cached administrator session per scope through a hidden-input modal (the Qt GUI remains unprivileged). It can be ended at any time from File - Lock Administrator Session.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;Boot Bitch %1 &lt;/h3&gt; &lt; &lt; &lt; &gt; &gt; Sviluppatore: &lt;/b&gt; CaptainMorgan12 Questa GUI è il punto di ingresso del pacchetto: un frontend Qt 3.3.x con l&apos;ausilio di protezione a porte per sistemi Debian Etch-era. Le fasi del pacchetto Debian/APT custodite (configurazione interrotta, dipendenze interrotte, metadati rinfrescano, aggiornano), GRUB-legacy la rigenerazione della configurazione, LUKS sblocca e controlla l&apos;editing dei file di destinazione controllati tramite l&apos;helper portato dopo la conferma; Host Maintenance consente gli stessi stadi supportati nativamente sul sistema attivo dopo aver ripetuto l&apos;identità host e i controlli di boot-mount. Le caratteristiche di soli moderni - verificato File Copy, Btrfs snapshot rollback, EFI/UKI e extlinux riparazione, boot-stack riconciliazione e Make Default - sono grigiati con i motivi di sonda dell&apos;helper su questo frontend; Arch/Alpine/Fedora pacchetto backends rimanere diagnostica-solo qui. &lt; &gt; &gt; La prima azione privilegiata autorizza una sessione di amministratore cache per ambito attraverso un modale di input nascosto (la GUI di Qt rimane non privata). Può essere terminato in qualsiasi momento da File - Lock Administrator Session.</translation>
    </message>
</context>
</TS>
