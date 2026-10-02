<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS language="fr">
<context>
    <name>LegacyMainWindow</name>
    <message>
        <source>Validate environment</source>
        <translation>Valider l&apos;environnement</translation>
    </message>
    <message>
        <source>Validate</source>
        <translation>Valider</translation>
    </message>
    <message>
        <source>Check the selected system&apos;s mounts, filesystem metadata, boot files, mapper consistency and dependency readiness before any repair action. This is an independent safety preflight rather than an optional Full Repair stage.</source>
        <translation>Vérifiez les montages du système sélectionné, les métadonnées du système de fichiers, les fichiers de démarrage, la cohérence mapper et la préparation à la dépendance avant toute action de réparation. Il s&apos;agit d&apos;un prévol de sécurité indépendant plutôt que d&apos;une étape de réparation complète en option.</translation>
    </message>
    <message>
        <source>Always preflight</source>
        <translation>Toujours avant le vol</translation>
    </message>
    <message>
        <source>File system repair</source>
        <translation>Réparation du système de fichiers</translation>
    </message>
    <message>
        <source>Check File Systems</source>
        <translation>Vérifier les systèmes de fichiers</translation>
    </message>
    <message>
        <source>Run the read-only file system check for the selected system&apos;s root and /boot filesystems and report each device&apos;s check tool and result without changing anything. This legacy frontend exposes the read-only check only; device repair is not wired.</source>
        <translation>Exécutez la vérification du système de fichiers en lecture seule pour les systèmes de fichiers racine et /boot sélectionnés et rapportez l&apos;outil de vérification de chaque appareil et le résultat sans rien changer. Cette façade historique expose la vérification en lecture seule seulement; la réparation de l&apos;appareil n&apos;est pas câblée.</translation>
    </message>
    <message>
        <source>Full Repair plan: unavailable on this frontend - read-only check only</source>
        <translation>Plan complet de réparation: indisponible sur cette façade - en lecture seule vérifier seulement</translation>
    </message>
    <message>
        <source>Complete package configuration</source>
        <translation>Terminer la configuration des paquets</translation>
    </message>
    <message>
        <source>Complete Configuration</source>
        <translation>Terminer la configuration</translation>
    </message>
    <message>
        <source>Complete interrupted dpkg package configuration in the selected repair system. This is the same stage controlled by Settings -&gt; Full Repair plan -&gt; Complete interrupted package configuration, but it can also be run independently here.</source>
        <translation>Configuration complète du paquet dpkg interrompu dans le système de réparation sélectionné. C&apos;est la même étape contrôlée par Paramètres -&gt; Plan complet de réparation -&gt; Configuration complète du paquet interrompu, mais il peut également être exécuté indépendamment ici.</translation>
    </message>
    <message>
        <source>Run the guarded dpkg-configure repair? The helper keeps its package-lock and runtime preflights.</source>
        <translation>Exécuter la réparation de configuration de dpkg ? L&apos;assistant garde ses prévols de verrouillage et d&apos;exécution.</translation>
    </message>
    <message>
        <source>Repair broken dependencies</source>
        <translation>Réparer les dépendances défectueuses</translation>
    </message>
    <message>
        <source>Repair Dependencies</source>
        <translation>Réparer les dépendances</translation>
    </message>
    <message>
        <source>Repair package dependencies in the selected repair system after the mandatory safety preflight. This maps directly to Settings -&gt; Repair broken package dependencies.</source>
        <translation>Réparer les dépendances du paquet dans le système de réparation sélectionné après le prévol obligatoire de sécurité. Cette carte se trouve directement dans Paramètres -&gt; Réparer les dépendances du paquet cassé.</translation>
    </message>
    <message>
        <source>Run the guarded fix-broken repair? The helper keeps its simulation-first preflight and runtime guards.</source>
        <translation>Exécuter la réparation surveillée cassée ? L&apos;assistant garde ses gardes de prévol et d&apos;exécution.</translation>
    </message>
    <message>
        <source>Refresh package metadata</source>
        <translation>Actualiser les métadonnées des paquets</translation>
    </message>
    <message>
        <source>Refresh Metadata</source>
        <translation>Actualiser les métadonnées</translation>
    </message>
    <message>
        <source>Refresh APT metadata in the selected repair system without upgrading installed packages. This maps directly to Settings -&gt; Refresh package metadata.</source>
        <translation>Actualiser les métadonnées APT dans le système de réparation sélectionné sans mettre à jour les paquets installés. Cette carte se trouve directement dans Paramètres -&gt; Actualiser les métadonnées du paquet.</translation>
    </message>
    <message>
        <source>Refresh package metadata for the selected scope? The helper requires a reachable, trusted APT source.</source>
        <translation>Actualiser les métadonnées du paquet pour la portée sélectionnée? L&apos;aide nécessite une source APT accessible et fiable.</translation>
    </message>
    <message>
        <source>Upgrade installed packages</source>
        <translation>Mettre à niveau les paquets installés</translation>
    </message>
    <message>
        <source>Simulate and Upgrade</source>
        <translation>Simuler et mettre à niveau</translation>
    </message>
    <message>
        <source>Simulate the APT transaction first, inspect proposed removals, then apply a safe upgrade. This maps directly to Settings -&gt; Upgrade installed packages.</source>
        <translation>Simulez d&apos;abord la transaction APT, inspectez les retraits proposés, puis appliquez une mise à niveau sûre. Cette carte se trouve directement dans Paramètres -&gt; Mettre à niveau les paquets installés.</translation>
    </message>
    <message>
        <source>Run the guarded apt-upgrade transaction? The helper keeps its simulation-first and source guards.</source>
        <translation>Exécuter la transaction surveillée ? L&apos;assistant garde sa première simulation et ses gardes source.</translation>
    </message>
    <message>
        <source>DKMS</source>
        <translation>DKMS</translation>
    </message>
    <message>
        <source>Rebuild DKMS</source>
        <translation>Reconstruire DKMS</translation>
    </message>
    <message>
        <source>Rebuild out-of-tree kernel modules for kernels installed in the selected system. The helper refuses this action when DKMS is not installed; this legacy frontend exposes no DKMS action.</source>
        <translation>Reconstruisez des modules de noyau en dehors de l&apos;arbre pour les noyaux installés dans le système sélectionné. L&apos;assistant refuse cette action lorsque DKMS n&apos;est pas installé; cette façade n&apos;expose aucune action DKMS.</translation>
    </message>
    <message>
        <source>Graphical login / display manager</source>
        <translation>Connexion graphique / gestionnaire d&apos;affichage</translation>
    </message>
    <message>
        <source>Restore Graphical Login</source>
        <translation>Restaurer la connexion graphique</translation>
    </message>
    <message>
        <source>Restore the legacy SysV display manager configured for the running host: the /etc/X11/default-display-manager entry and the missing runlevel S-symlink, with a backup and rollback, never starting the GUI. This is a host-scope stage on this legacy frontend.</source>
        <translation>Restaurer l&apos;ancien gestionnaire d&apos;affichage SysV configuré pour l&apos;hôte en cours d&apos;exécution : l&apos;entrée /etc/X11/default-display-manager et le lien S-symlink d&apos;exécution manquant, avec une sauvegarde et un retour en arrière, ne démarre jamais l&apos;interface graphique. C&apos;est une scène d&apos;hôte sur cette façade.</translation>
    </message>
    <message>
        <source>Restore the graphical login configuration for the running host? The helper backs up /etc/X11/default-display-manager and the runlevel symlink state, restores the configured entry and the missing S-symlink, rolls back on any failure, and never starts the display manager.</source>
        <translation>Restaurer la configuration graphique de connexion pour l&apos;hôte en cours d&apos;exécution ? Le helper sauvegarde /etc/X11/default-display-manager et l&apos;état de runlevel symlink, restaure l&apos;entrée configurée et le S-symlink manquant, retourne sur toute défaillance, et ne démarre jamais le gestionnaire d&apos;affichage.</translation>
    </message>
    <message>
        <source>Initramfs</source>
        <translation>Initramfs</translation>
    </message>
    <message>
        <source>Rebuild Initramfs</source>
        <translation>Reconstruire les initramfs</translation>
    </message>
    <message>
        <source>Rebuild initramfs images for the selected repair system only after mapper and crypttab consistency checks pass. The helper backs up each image before the apply. On Etch the stage runs through the guarded plain-chroot fallback (no unshare required).</source>
        <translation>Reconstruisez des images initramfs pour le système de réparation sélectionné seulement après que mapper et crypttab contrôles de cohérence passent. L&apos;aide sauvegarde chaque image avant l&apos;application. Sur Etch, l&apos;étape passe par l&apos;arrière-plan protégé de la racine plate (pas de partage nécessaire).</translation>
    </message>
    <message>
        <source>Rebuild the initramfs for the selected scope? The helper keeps its mapper/crypttab and backup preflights.</source>
        <translation>Reconstruire le initramfs pour la portée sélectionnée? L&apos;aide garde son mapper/crypttab et sauvegarde des prévols.</translation>
    </message>
    <message>
        <source>EFI / UKI bootloader</source>
        <translation>Chargeur de démarrage EFI / UKI</translation>
    </message>
    <message>
        <source>Repair EFI / UKI</source>
        <translation>Réparer EFI / UKI</translation>
    </message>
    <message>
        <source>Repair the selected system&apos;s EFI / UKI boot path. This legacy frontend exposes no EFI action; the Etch target is a BIOS/GRUB-legacy system.</source>
        <translation>Réparer le chemin de démarrage EFI / UKI du système sélectionné. Cette façade historique n&apos;expose aucune action EFI; la cible Etch est un système BIOS/GRUB-legacy.</translation>
    </message>
    <message>
        <source>GRUB configuration</source>
        <translation>Configuration GRUB</translation>
    </message>
    <message>
        <source>Regenerate GRUB</source>
        <translation>Régénérer GRUB</translation>
    </message>
    <message>
        <source>Regenerate the selected repair system&apos;s GRUB menu/configuration after the mandatory safety preflight. The helper backs up menu.lst, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Régénérer le menu/configuration GRUB du système de réparation sélectionné après le prévol de sécurité obligatoire. L&apos;assistant sauvegarde menu.lst, conserve chaque entrée de démarrage existante et retourne sur toute défaillance.</translation>
    </message>
    <message>
        <source>Regenerate the GRUB configuration? The helper backs up the target menu/configuration, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Régénérer la configuration GRUB ? L&apos;assistant sauvegarde le menu/configuration cible, conserve chaque entrée de démarrage existante et retourne sur toute défaillance.</translation>
    </message>
    <message>
        <source>extlinux configuration</source>
        <translation>Configuration extlinux</translation>
    </message>
    <message>
        <source>Regenerate extlinux</source>
        <translation>Régénérer extlinux</translation>
    </message>
    <message>
        <source>Regenerate the selected system&apos;s extlinux bootloader configuration. This legacy frontend exposes no extlinux action.</source>
        <translation>Régénérer la configuration extlinux du système sélectionné. Cette façade historique n&apos;expose aucune action extlinux.</translation>
    </message>
    <message>
        <source>Boot stack reconciliation</source>
        <translation>Réconciliation de la pile de démarrage</translation>
    </message>
    <message>
        <source>Reconcile Boot Stack</source>
        <translation>Réconcilier la pile de démarrage</translation>
    </message>
    <message>
        <source>Reconcile the selected repair system&apos;s boot stack in one guarded legacy pass: mapper/crypttab validation, initramfs rebuild and GRUB-legacy configuration regeneration, with the component backups and preflights unchanged. This is the Etch equivalent of the modern boot-stack reconciliation and stays out of the Full Repair plan.</source>
        <translation>Reconcile la pile de démarrage du système de réparation sélectionné en un seul passe gardé : validation mapper/crypttab, reconstruction initramfs et régénération de configuration GRUB-legacy, les sauvegardes des composants et prévols inchangés. Il s&apos;agit de l&apos;équivalent Etch de la réconciliation boot-stack moderne et reste hors du plan de réparation complète.</translation>
    </message>
    <message>
        <source>Full Repair plan: Manual recovery tool</source>
        <translation>Plan complet de réparation: outil de récupération manuelle</translation>
    </message>
    <message>
        <source>Run the guarded boot-stack reconciliation? The helper runs the mapper/crypttab validation, the initramfs rebuild and the GRUB-legacy regeneration in one pass with every component preflight and backup.</source>
        <translation>Tu as fait la réconciliation ? L&apos;assistant exécute la validation mapper/crypttab, la reconstruction initramfs et la régénération GRUB-legacy en un seul passage avec chaque composant prévol et sauvegarde.</translation>
    </message>
    <message>
        <source>Complete interrupted package configuration</source>
        <translation>Terminer la configuration de paquets interrompue</translation>
    </message>
    <message>
        <source>Repair broken package dependencies</source>
        <translation>Réparer les dépendances de paquets cassées</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Définir par défaut</translation>
    </message>
    <message>
        <source>Repair file system errors (read-only check first)</source>
        <translation>Réparer les erreurs du système de fichiers (vérification en lecture seule d&apos;abord)</translation>
    </message>
    <message>
        <source>the legacy frontend exposes the read-only file system check only; per-device repair is not wired on this frontend (fail closed)</source>
        <translation>l&apos;ancienne façade expose la vérification du système de fichiers en lecture seule; la réparation par appareil n&apos;est pas câblée sur cette façade (échec fermé)</translation>
    </message>
    <message>
        <source>Upgrade installed packages (adaptive APT simulation)</source>
        <translation>Mettre à niveau les paquets installés (simulation APT adaptative)</translation>
    </message>
    <message>
        <source>Rebuild DKMS modules</source>
        <translation>Reconstruire les modules DKMS</translation>
    </message>
    <message>
        <source>Restore graphical login manager</source>
        <translation>Restaurer le gestionnaire de connexion graphique</translation>
    </message>
    <message>
        <source>Rebuild initramfs after mapper/crypttab validation</source>
        <translation>Reconstruire initramfs après validation mapper/crypttab</translation>
    </message>
    <message>
        <source>Repair EFI / UKI boot path</source>
        <translation>Réparation du chemin de démarrage EFI / UKI</translation>
    </message>
    <message>
        <source>Update GRUB configuration</source>
        <translation>Mettre à jour la configuration GRUB</translation>
    </message>
    <message>
        <source>Update extlinux configuration</source>
        <translation>Mettre à jour la configuration extlinux</translation>
    </message>
    <message>
        <source>Choose Full Repair stages in Settings. Enabled stages run in the order shown. Each configurable stage also appears below as an individual tool; the Full Repair column mirrors its current Settings state. The boot tools (EFI / UKI bootloader, GRUB or extlinux configuration, boot-stack reconciliation and Make Default) are independent: run them in any order, and a later action re-verifies what an earlier one changed and reports its own result. The active scope is shown beside Repair: selected repair drive or Running Host maintenance.</source>
        <translation>Choisissez les étapes de réparation complète dans Paramètres. Activer les étapes dans l&apos;ordre affiché. Chaque étape configurable apparaît également ci-dessous en tant qu&apos;outil individuel ; la colonne de réparation complète reflète son état actuel de paramètres. Les outils de démarrage (EFI / UKI bootloader, GRUB ou extlinux configuration, boot-stack reconciliation et Make Default) sont indépendants : lancez-les dans n&apos;importe quel ordre, et une action ultérieure re-vérifie ce qu&apos;un précédent a changé et rapporte son propre résultat. Le champ d&apos;application actif est affiché à côté de Réparer : le lecteur de réparation sélectionné ou la maintenance de l&apos;hôte de course.</translation>
    </message>
    <message>
        <source>Run All diagnostics for the selected target or running host before starting Full Repair. The report is read-only evidence used to choose and confirm repair stages.</source>
        <translation>Exécutez tous les diagnostics pour la cible ou l&apos;hôte sélectionné avant de commencer la réparation complète. Le rapport est une preuve en lecture seule utilisée pour choisir et confirmer les étapes de réparation.</translation>
    </message>
    <message>
        <source>Ready: required cached read-only diagnostics are available for the selected stages. Review them in Diagnostics or Logs before confirming.</source>
        <translation>Prêt : les diagnostics en lecture seule sont disponibles pour les étapes sélectionnées. Consultez-les dans Diagnostics ou Logs avant de confirmer.</translation>
    </message>
    <message>
        <source>Environment validation</source>
        <translation>Validation de l&apos;environnement</translation>
    </message>
    <message>
        <source>Summarizes the selected system, protection state, mounted identity and inspection readiness.</source>
        <translation>Résume le système sélectionné, l&apos;état de protection, l&apos;identité montée et la préparation à l&apos;inspection.</translation>
    </message>
    <message>
        <source>Distribution and boot backend profile</source>
        <translation>Distribution et profil du moteur de démarrage</translation>
    </message>
    <message>
        <source>Identifies the distribution family, package manager, initramfs generator, bootloader and current guarded repair capability.</source>
        <translation>Indique la famille de distribution, le gestionnaire de paquets, le générateur initramfs, le chargeur d&apos;amorçage et la capacité de réparation actuelle.</translation>
    </message>
    <message>
        <source>Boot diagnostics</source>
        <translation>Diagnostics de démarrage</translation>
    </message>
    <message>
        <source>Shows boot mounts and /boot contents plus storage evidence without changing the selected system.</source>
        <translation>Affiche les montages de démarrage et /boot contenu plus preuve de stockage sans changer le système sélectionné.</translation>
    </message>
    <message>
        <source>Boot evidence and selection history</source>
        <translation>Preuve de démarrage et historique de sélection</translation>
    </message>
    <message>
        <source>Correlates the detected boot chain, bootloader selection, kernel/initramfs and unlock evidence.</source>
        <translation>Corrèle la chaîne de démarrage détectée, la sélection du chargeur de démarrage, le noyau/initramfs et déverrouiller les preuves.</translation>
    </message>
    <message>
        <source>Kernel / initramfs</source>
        <translation>Noyau / initramfs</translation>
    </message>
    <message>
        <source>Reviews kernel files and verifies matching initramfs images through a read-only inspection.</source>
        <translation>Examine les fichiers du noyau et vérifie la correspondance des images initramfs par une inspection en lecture seule.</translation>
    </message>
    <message>
        <source>Reviews the GRUB configuration without changing boot files.</source>
        <translation>Examine la configuration GRUB sans modifier les fichiers de démarrage.</translation>
    </message>
    <message>
        <source>EFI / UKI boot state</source>
        <translation>État de démarrage EFI / UKI</translation>
    </message>
    <message>
        <source>Inspects EFI/UKI evidence; unavailable on this legacy BIOS frontend with the helper&apos;s probe reason.</source>
        <translation>Inspecter les preuves EFI/UKI; ne pas être disponible sur cette façade du BIOS avec la raison de la sonde de l&apos;aide.</translation>
    </message>
    <message>
        <source>Reviews the configured display manager and recent boot evidence without starting the GUI.</source>
        <translation>Examine le gestionnaire d&apos;affichage configuré et les preuves récentes de démarrage sans démarrer l&apos;interface graphique.</translation>
    </message>
    <message>
        <source>Boot errors</source>
        <translation>Erreurs de démarrage</translation>
    </message>
    <message>
        <source>Reads recent error-priority entries from the running host or selected repair system when available.</source>
        <translation>Lire les entrées récentes de priorité d&apos;erreur de l&apos;hôte en cours d&apos;exécution ou du système de réparation sélectionné lorsque disponible.</translation>
    </message>
    <message>
        <source>Disk usage</source>
        <translation>Utilisation du disque</translation>
    </message>
    <message>
        <source>Summarizes filesystem capacity and free space for the running host or read-only repair target.</source>
        <translation>Résume la capacité du système de fichiers et l&apos;espace libre pour l&apos;hôte en cours d&apos;exécution ou la cible de réparation en lecture seule.</translation>
    </message>
    <message>
        <source>File systems</source>
        <translation>Systèmes de fichiers</translation>
    </message>
    <message>
        <source>Runs the read-only file system check for the selected system&apos;s root, /boot and other filesystems.</source>
        <translation>Exécute la vérification du système de fichiers en lecture seule pour la racine du système sélectionné, /boot et d&apos;autres systèmes de fichiers.</translation>
    </message>
    <message>
        <source>/etc/fstab review</source>
        <translation>/etc/fstab examen</translation>
    </message>
    <message>
        <source>Displays the running host or selected repair system&apos;s fstab; repair-system inspection is mounted read-only.</source>
        <translation>Affiche le fstab de l&apos;hôte en cours d&apos;exécution ou du système de réparation sélectionné; l&apos;inspection du système de réparation est montée en lecture seule.</translation>
    </message>
    <message>
        <source>Btrfs status</source>
        <translation>Statut Btrfs</translation>
    </message>
    <message>
        <source>Shows Btrfs filesystem and subvolume information when the target uses Btrfs.</source>
        <translation>Affiche les informations du système de fichiers Btrfs et du sous-volume lorsque la cible utilise Btrfs.</translation>
    </message>
    <message>
        <source>Device-mapper ancestry</source>
        <translation>Ancêtre de l&apos;appareil-cuivre</translation>
    </message>
    <message>
        <source>Shows selected mapper ancestry and device-mapper state when available.</source>
        <translation>Affiche l&apos;origine de la carte sélectionnée et l&apos;état de l&apos;appareil-cuivre lorsque disponible.</translation>
    </message>
    <message>
        <source>LUKS / crypttab evidence</source>
        <translation>LUKS / preuve crypttab</translation>
    </message>
    <message>
        <source>Shows LUKS/mapped ancestry plus crypttab and fstab mapper references.</source>
        <translation>Affiche les références LUKS/mappered ancestry plus crypttab et fstab mapper.</translation>
    </message>
    <message>
        <source>Full diagnostic report</source>
        <translation>Rapport de diagnostic complet</translation>
    </message>
    <message>
        <source>Combines all read-only diagnostics for the selected scope (same as Run All).</source>
        <translation>Combine tous les diagnostics en lecture seule pour la portée sélectionnée (même que Run All).</translation>
    </message>
    <message>
        <source>All entries</source>
        <translation>Toutes les entrées</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>Diagnostics</translation>
    </message>
    <message>
        <source>Repairs</source>
        <translation>Réparations</translation>
    </message>
    <message>
        <source>Package repair</source>
        <translation>Réparation du colis</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Copie de fichiers</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Découverte des périphériques</translation>
    </message>
    <message>
        <source>Host</source>
        <translation>Hôte</translation>
    </message>
    <message>
        <source>Required for block-device inventory</source>
        <translation>Requis pour l&apos;inventaire des dispositifs à blocs</translation>
    </message>
    <message>
        <source>Filesystem identification</source>
        <translation>Identification du système de fichiers</translation>
    </message>
    <message>
        <source>Used to identify filesystem metadata</source>
        <translation>Utilisé pour identifier les métadonnées du système de fichiers</translation>
    </message>
    <message>
        <source>Mount inspection</source>
        <translation>Inspection de montage</translation>
    </message>
    <message>
        <source>Used to understand active mounts</source>
        <translation>Utilisé pour comprendre les montages actifs</translation>
    </message>
    <message>
        <source>LUKS support</source>
        <translation>Support LUKS</translation>
    </message>
    <message>
        <source>Required to unlock encrypted targets</source>
        <translation>Requis pour déverrouiller les cibles chiffrées</translation>
    </message>
    <message>
        <source>Btrfs support</source>
        <translation>Support Btrfs</translation>
    </message>
    <message>
        <source>Required for Btrfs inspection and snapshot rollback</source>
        <translation>Requis pour l&apos;inspection Btrfs et le retour instantané</translation>
    </message>
    <message>
        <source>Bidirectional file copy</source>
        <translation>Copie de fichier bidirectionnel</translation>
    </message>
    <message>
        <source>Host/Repair</source>
        <translation>Hôte/Réparation</translation>
    </message>
    <message>
        <source>Required for verified Host-to-Repair and Repair-to-Host transfer</source>
        <translation>Requis pour le transfert d&apos;hôte-réparation et de réparation à domicile vérifié</translation>
    </message>
    <message>
        <source>Chroot repair</source>
        <translation>Réparation des racines</translation>
    </message>
    <message>
        <source>Required for target-side repair commands</source>
        <translation>Requis pour les commandes de réparation côté cible</translation>
    </message>
    <message>
        <source>Offline systemd repair</source>
        <translation>Réparation hors ligne</translation>
    </message>
    <message>
        <source>Used to restore graphical.target and the configured display manager without starting the target GUI</source>
        <translation>Utilisé pour restaurer graphique. cible et gestionnaire d&apos;affichage configuré sans démarrer l&apos;interface graphique cible</translation>
    </message>
    <message>
        <source>UEFI NVRAM inspection</source>
        <translation>Contrôle UEFI NVRAM</translation>
    </message>
    <message>
        <source>Used to preserve target EFI BootOrder during TUXEDO UKI rebuilds when efivars are available</source>
        <translation>Utilisé pour préserver la cible EFI BootOrder lors de la reconstruction TUXEDO UKI lorsque les efivars sont disponibles</translation>
    </message>
    <message>
        <source>UKI verification</source>
        <translation>Vérification UKI</translation>
    </message>
    <message>
        <source>Used to verify the kernel embedded in a rebuilt unified kernel image</source>
        <translation>Utilisé pour vérifier le noyau intégré dans une image du noyau unifiée reconstruite</translation>
    </message>
    <message>
        <source>GRUB EFI repair</source>
        <translation>GRUB EFI réparation</translation>
    </message>
    <message>
        <source>Target/Host</source>
        <translation>Cible/Hôte</translation>
    </message>
    <message>
        <source>Required only for conventional GRUB-based EFI systems</source>
        <translation>Requis uniquement pour les systèmes EFI classiques basés sur GRUB</translation>
    </message>
    <message>
        <source>Target</source>
        <translation>Objectif</translation>
    </message>
    <message>
        <source>Debian-family GRUB helper</source>
        <translation>Aide à la famille Debian GRUB</translation>
    </message>
    <message>
        <source>Portable GRUB configuration generator used by Arch and other non-Debian systems</source>
        <translation>Générateur de configuration portable GRUB utilisé par Arch et d&apos;autres systèmes non Debian</translation>
    </message>
    <message>
        <source>Initramfs rebuild</source>
        <translation>Reconstruction des initramfs</translation>
    </message>
    <message>
        <source>Debian-family initramfs helper</source>
        <translation>Aide à la famille Debian initramfs</translation>
    </message>
    <message>
        <source>Arch-family initramfs generator</source>
        <translation>Générateur de la famille Arch initramfs</translation>
    </message>
    <message>
        <source>Alternative initramfs generator used by Arch and other distributions</source>
        <translation>Générateur alternatif initramfs utilisé par Arch et autres distributions</translation>
    </message>
    <message>
        <source>Initramfs verification</source>
        <translation>Vérification des initramfs</translation>
    </message>
    <message>
        <source>Read-only verification for mkinitcpio images</source>
        <translation>Vérification en lecture seule pour les images mkinitcpio</translation>
    </message>
    <message>
        <source>Read-only verification for dracut images</source>
        <translation>Vérification en lecture seule pour les images dracut</translation>
    </message>
    <message>
        <source>systemd-boot inspection</source>
        <translation>Contrôle systemd-boot</translation>
    </message>
    <message>
        <source>Host/Target</source>
        <translation>Hôte/cible</translation>
    </message>
    <message>
        <source>Read-only inspection of systemd-boot and generic UKI layouts</source>
        <translation>Inspection en lecture seule de systemd-boot et des mises en page UKI génériques</translation>
    </message>
    <message>
        <source>Arch package manager</source>
        <translation>Gestionnaire de paquets Arch</translation>
    </message>
    <message>
        <source>Arch-family package database and transaction tool</source>
        <translation>Base de données et outil transactionnel Arch-family</translation>
    </message>
    <message>
        <source>DKMS rebuild</source>
        <translation>Reconstruction de DKMS</translation>
    </message>
    <message>
        <source>Required only when target uses DKMS modules</source>
        <translation>Requis uniquement lorsque la cible utilise des modules DKMS</translation>
    </message>
    <message>
        <source>LVM inspection</source>
        <translation>Contrôle LVM</translation>
    </message>
    <message>
        <source>Optional LVM storage-stack support</source>
        <translation>Support optionnel de stockage-pierre LVM</translation>
    </message>
    <message>
        <source>Software RAID</source>
        <translation>Logiciel RAID</translation>
    </message>
    <message>
        <source>Optional Linux MD RAID support</source>
        <translation>Support optionnel Linux MD RAID</translation>
    </message>
    <message>
        <source>Process namespace isolation</source>
        <translation>Isolation de l&apos;espace de noms de processus</translation>
    </message>
    <message>
        <source>Host+Target</source>
        <translation>Hôte+ Objectif</translation>
    </message>
    <message>
        <source>The ported helper falls back to a guarded plain chroot when unshare is absent</source>
        <translation>L&apos;aide portée revient à une chroot bien gardée quand il n&apos;y a pas de partage</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Annuler</translation>
    </message>
    <message>
        <source>OK</source>
        <translation>Très bien.</translation>
    </message>
    <message>
        <source>Yes</source>
        <translation>Oui</translation>
    </message>
    <message>
        <source>No</source>
        <translation>Numéro</translation>
    </message>
    <message>
        <source>read-only helper diagnostic</source>
        <translation>diagnostic d&apos;aide en lecture seule</translation>
    </message>
</context>
<context>
    <name>legacy::LegacyMainWindow</name>
    <message>
        <source>Boot Bitch Legacy (Etch / KDE 3.5 era)</source>
        <translation>Boot Bitch Legacy (ère Etch / KDE 3.5)</translation>
    </message>
    <message>
        <source>Linux recovery and boot-repair utility</source>
        <translation>Récupération Linux et utilitaire de réparation de démarrage</translation>
    </message>
    <message>
        <source>GUARDED REPAIR</source>
        <translation>RÉPARATION PROTÉGÉE</translation>
    </message>
    <message>
        <source>Ordinary repairs require an explicitly selected non-host target. The protected running host has a separate deliberate maintenance mode with the same guarded repair stages and requires privilege authorization.</source>
        <translation>Les réparations ordinaires nécessitent une cible non hôte explicitement sélectionnée. L&apos;hôte de course protégé a un mode d&apos;entretien délibéré séparé avec les mêmes étapes de réparation surveillées et nécessite une autorisation de privilège.</translation>
    </message>
    <message>
        <source>Systems</source>
        <translation>Systèmes</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>Diagnostics</translation>
    </message>
    <message>
        <source>Repair</source>
        <translation>Réparer</translation>
    </message>
    <message>
        <source>Chroot Shell</source>
        <translation>Shell chroot</translation>
    </message>
    <message>
        <source>File Copy</source>
        <translation>Copie de fichiers</translation>
    </message>
    <message>
        <source>Logs</source>
        <translation>Journaux</translation>
    </message>
    <message>
        <source>Settings</source>
        <translation>Paramètres</translation>
    </message>
    <message>
        <source>(not created yet)</source>
        <translation>(pas encore créé)</translation>
    </message>
    <message>
        <source>Information</source>
        <translation>Informations</translation>
    </message>
    <message>
        <source>Close</source>
        <translation>Fermer</translation>
    </message>
    <message>
        <source>&amp;File</source>
        <translation>&amp;Fichier</translation>
    </message>
    <message>
        <source>&amp;Refresh Devices</source>
        <translation>&amp;Rénover les appareils</translation>
    </message>
    <message>
        <source>&amp;Lock Administrator Session</source>
        <translation>&amp;Session de l&apos;administrateur de verrouillage</translation>
    </message>
    <message>
        <source>&amp;Quit</source>
        <translation>&amp;Quitter</translation>
    </message>
    <message>
        <source>&amp;View</source>
        <translation>&amp;Affichage</translation>
    </message>
    <message>
        <source>&amp;Systems</source>
        <translation>&amp;Systèmes</translation>
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
        <translation>&amp;Paramètres</translation>
    </message>
    <message>
        <source>&amp;Auto-size Device Columns</source>
        <translation>&amp;Colonnes de périphériques de taille automatique</translation>
    </message>
    <message>
        <source>&amp;Wrap Log Lines</source>
        <translation>&amp;Lignes de log de l&apos;enveloppe</translation>
    </message>
    <message>
        <source>&amp;Help</source>
        <translation>&amp;Aide</translation>
    </message>
    <message>
        <source>&amp;Using Boot Bitch</source>
        <translation>&amp;Utilisation de Boot Bitch</translation>
    </message>
    <message>
        <source>&amp;About Boot Bitch</source>
        <translation>À &amp;propos de Boot Bitch</translation>
    </message>
    <message>
        <source>Device columns auto-sized. Drag headers to fine-tune widths.</source>
        <translation>Colonnes de périphérique taille automatique. Faites glisser les en-têtes vers des largeurs fines.</translation>
    </message>
    <message>
        <source>A helper command is running; wait for it to finish before locking the session.</source>
        <translation>Une commande helper est en cours d&apos;exécution; attendez qu&apos;elle finisse avant de verrouiller la session.</translation>
    </message>
    <message>
        <source>Administrator session locked; the next privileged action will request authorization.</source>
        <translation>Session d&apos;administrateur verrouillée; la prochaine action privilégiée demandera l&apos;autorisation.</translation>
    </message>
    <message>
        <source>Using Boot Bitch</source>
        <translation>Utilisation de Boot Bitch</translation>
    </message>
    <message>
        <source>Boot Bitch must run from a different booted Linux environment than the system being repaired. Use a Linux live medium or another Linux installation on a different physical drive.&lt;br&gt;&lt;br&gt;The running host is protected from ordinary repair-target selection, but it can be explicitly selected through &lt;b&gt;Host Maintenance&lt;/b&gt; for guarded native diagnostics and supported maintenance stages.&lt;br&gt;&lt;br&gt;Diagnostics follow the Systems page: the committed repair drive while Host Maintenance is off, or the protected running host while it is active.&lt;br&gt;&lt;br&gt;The first privileged action requests administrator authorization once for this Boot Bitch window; &lt;b&gt;File - Lock Administrator Session&lt;/b&gt; ends that helper session immediately. Every repair keeps the helper&apos;s own runtime preflights.</source>
        <translation>Boot Bitch doit fonctionner à partir d&apos;un environnement Linux démarré différent de celui du système en cours de réparation. Utilisez un support live Linux ou une autre installation Linux sur un disque physique différent.&lt;br&gt;&lt;br&gt; L&apos;hôte en cours d&apos;exécution est protégé contre la sélection de cibles de réparation ordinaires, mais il peut être explicitement sélectionné par &lt;b&gt;Host Maintenance&lt;/b&gt; pour les diagnostics natifs protégés et les étapes de maintenance supportées.&lt;br&gt;&lt;br&gt; Les diagnostics suivent la page Systèmes : le lecteur de réparation engagé pendant que la maintenance de l&apos;hôte est désactivée, ou l&apos;hôte protégé pendant qu&apos;il est actif.&lt;br&gt;&lt;br&gt; La première action privilégiée demande l&apos;autorisation de l&apos;administrateur une fois pour cette fenêtre Boot Bitch; &lt;b&gt;File - Lock Administrator Session&lt;/b&gt; termine immédiatement cette session d&apos;aide. Chaque réparation garde les prévols de l&apos;aide.</translation>
    </message>
    <message>
        <source>Select a physical drive; Boot Bitch resolves the most likely Linux system volume automatically. The running host stays protected from ordinary target repairs, with a separate explicit host-maintenance path for its own system. The Details button shows the protected host&apos;s facts in the details pane; selecting any drive row restores the per-drive pane.</source>
        <translation>Sélectionnez un lecteur physique ; Boot Bitch résout automatiquement le volume système Linux le plus probable. L&apos;hôte en cours d&apos;exécution reste protégé contre les réparations de cibles ordinaires, avec un chemin d&apos;entretien d&apos;hôte explicite séparé pour son propre système. Le bouton Détails affiche les faits de l&apos;hôte protégé dans le panneau de détails; la sélection de n&apos;importe quelle ligne de disque restaure le volet par disque.</translation>
    </message>
    <message>
        <source>Refresh Devices</source>
        <translation>Actualiser les périphériques</translation>
    </message>
    <message>
        <source>Re-read the read-only kernel inventory (/proc/partitions, /proc/mounts, /proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* and the udev metadata database). No block device is opened and nothing is written.</source>
        <translation>Relire l&apos;inventaire du noyau en lecture seule (/proc/partitions, /proc/mounts, /proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* et la base de métadonnées udev). Aucun périphérique bloc n&apos;est ouvert et rien n&apos;est écrit.</translation>
    </message>
    <message>
        <source>[OK]</source>
        <translation>[OK]</translation>
    </message>
    <message>
        <source>The running system was detected and stays protected from ordinary target repairs.</source>
        <translation>Le système de fonctionnement a été détecté et reste protégé contre les réparations ordinaires.</translation>
    </message>
    <message>
        <source>Detecting running system...</source>
        <translation>Détection du système en cours d&apos;exécution...</translation>
    </message>
    <message>
        <source>Detecting protected storage...</source>
        <translation>Détecter le stockage protégé...</translation>
    </message>
    <message>
        <source>PROTECTED</source>
        <translation>PROTÉGÉ</translation>
    </message>
    <message>
        <source>The running host remains protected from ordinary repair-target operations.</source>
        <translation>L&apos;hôte en cours d&apos;exécution reste protégé contre les opérations ordinaires de réparation-cible.</translation>
    </message>
    <message>
        <source>Details</source>
        <translation>Détails</translation>
    </message>
    <message>
        <source>Show read-only details for the protected running host.</source>
        <translation>Afficher les détails en lecture seule pour l&apos;hôte d&apos;exécution protégé.</translation>
    </message>
    <message>
        <source>Host Maintenance</source>
        <translation>Maintenance de l&apos;hôte</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Définir par défaut</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host (menu.lst default directive with a backup and rollback). Requires Host Maintenance and the cached host-default probe.</source>
        <translation>Faites de l&apos;entrée canonique installée du noyau l&apos;entrée par défaut du démarrage GRUB-legacy sur l&apos;hôte en cours d&apos;exécution (menu.lst directive par défaut avec sauvegarde et retour). Nécessite la maintenance de l&apos;hôte et la sonde par défaut de l&apos;hôte en cache.</translation>
    </message>
    <message>
        <source>Available repair targets</source>
        <translation>Cibles de réparation disponibles</translation>
    </message>
    <message>
        <source>Most likely first</source>
        <translation>Le plus probable en premier</translation>
    </message>
    <message>
        <source>Drives are listed by the read-only inventory. Select a row to inspect it; Select Target commits the selected non-host drive with its auto-resolved Linux root component.</source>
        <translation>Les disques sont répertoriés par l&apos;inventaire en lecture seule. Sélectionnez une ligne pour l&apos;inspecter; Sélectionnez Target commet le disque non hôte sélectionné avec son composant racine Linux résolu automatiquement.</translation>
    </message>
    <message>
        <source>Device</source>
        <translation>Appareil</translation>
    </message>
    <message>
        <source>Size</source>
        <translation>Taille</translation>
    </message>
    <message>
        <source>Type</source>
        <translation>Type</translation>
    </message>
    <message>
        <source>Filesystem</source>
        <translation>Système de fichiers</translation>
    </message>
    <message>
        <source>Select Target</source>
        <translation>Sélectionner la cible</translation>
    </message>
    <message>
        <source>Unlock</source>
        <translation>Déverrouiller</translation>
    </message>
    <message>
        <source>Authorize</source>
        <translation>Autoriser</translation>
    </message>
    <message>
        <source>Establish the privileged helper session for the current scope now instead of waiting for the next privileged action.</source>
        <translation>Établir la session d&apos;aide privilégiée pour la portée actuelle maintenant au lieu d&apos;attendre la prochaine action privilégiée.</translation>
    </message>
    <message>
        <source>Committed target: none</source>
        <translation>Cible validée : aucune</translation>
    </message>
    <message>
        <source>Unlock state for the selected drive; the LUKS passphrase is never logged.</source>
        <translation>Déverrouiller l&apos;état du lecteur sélectionné; la phrase de passe LUKS n&apos;est jamais enregistrée.</translation>
    </message>
    <message>
        <source>Unlock status</source>
        <translation>État du déverrouillage</translation>
    </message>
    <message>
        <source>Read-only inventory plus helper-confirmed facts; mirrors the modern Qt6 Selected drive details panel.</source>
        <translation>En lecture seule l&apos;inventaire et les faits confirmés par l&apos;aide; reflète le panneau de détails de disque Qt6 sélectionné moderne.</translation>
    </message>
    <message>
        <source>Selected drive details</source>
        <translation>Détails du lecteur sélectionné</translation>
    </message>
    <message>
        <source>Field</source>
        <translation>Champ</translation>
    </message>
    <message>
        <source>Value</source>
        <translation>Valeur</translation>
    </message>
    <message>
        <source>Run All runs every available read-only diagnostic for the current scope; selecting a check runs it alone. Diagnostics are read-only and are the only evidence source for the gated repair actions.</source>
        <translation>Exécuter Tous exécute tous les diagnostics disponibles en lecture seule pour la portée actuelle; sélectionner un contrôle exécute seul. Les diagnostics sont en lecture seule et sont la seule source de preuves pour les actions de réparation fermée.</translation>
    </message>
    <message>
        <source>Target: none selected</source>
        <translation>Objectif : aucune sélection</translation>
    </message>
    <message>
        <source>Diagnostics follow the committed repair target, or the protected running host while Host Maintenance is active.</source>
        <translation>Les diagnostics suivent la cible de réparation engagée, ou l&apos;hôte de fonctionnement protégé pendant que l&apos;Host Maintenance est active.</translation>
    </message>
    <message>
        <source>Run All</source>
        <translation>Tout exécuter</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope.</source>
        <translation>Exécutez All - exécutez tous les diagnostics disponibles en lecture seule pour la portée actuelle.</translation>
    </message>
    <message>
        <source>Target configuration:</source>
        <translation>Configuration de la cible :</translation>
    </message>
    <message>
        <source>Etch-era target configuration files; availability is probed read-only by the helper&apos;s diagnostics.</source>
        <translation>Fichiers de configuration de cible de l&apos;ère ETCH; la disponibilité est en lecture seule par le diagnostic de l&apos;aide.</translation>
    </message>
    <message>
        <source>Edit Target File...</source>
        <translation>Modifier le fichier cible...</translation>
    </message>
    <message>
        <source>Runs one read-only diagnostic for the selected scope through the helper (`diagnose &lt;key&gt;` / `host-diagnose &lt;key&gt;`); Run All is the combined report.</source>
        <translation>Exécute un seul diagnostic en lecture seule pour la portée sélectionnée par l&apos;aide (`diagnose &lt;key&gt;` / `host-diagnose &lt;key&gt;`); Exécuter Tous est le rapport combiné.</translation>
    </message>
    <message>
        <source>Diagnostic checks</source>
        <translation>Vérifications de diagnostic</translation>
    </message>
    <message>
        <source>Check</source>
        <translation>Vérifier</translation>
    </message>
    <message>
        <source>Selected diagnostic</source>
        <translation>Diagnostic sélectionné</translation>
    </message>
    <message>
        <source>Select a diagnostic</source>
        <translation>Sélectionner un diagnostic</translation>
    </message>
    <message>
        <source>Choose a diagnostic from the list.</source>
        <translation>Choisissez un diagnostic dans la liste.</translation>
    </message>
    <message>
        <source>Ready</source>
        <translation>Prêt</translation>
    </message>
    <message>
        <source>Results</source>
        <translation>Résultats</translation>
    </message>
    <message>
        <source>Run Diagnostic</source>
        <translation>Exécuter un diagnostic</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic for the current scope.</source>
        <translation>Exécutez le diagnostic - exécutez le diagnostic en lecture seule sélectionné pour la portée actuelle.</translation>
    </message>
    <message>
        <source>Copy Results</source>
        <translation>Copier les résultats</translation>
    </message>
    <message>
        <source>Save Results...</source>
        <translation>Enregistrer les résultats...</translation>
    </message>
    <message>
        <source>The Full Repair plan runs the selected legacy stages in order through the guarded helper; the individual tools run one stage at a time. Every action stays disabled until the cached capability lines say available and the helper keeps its runtime preflights.</source>
        <translation>Le plan de réparation complète exécute les étapes d&apos;héritage sélectionnées dans l&apos;ordre par l&apos;aide gardée; les outils individuels exécutent une étape à la fois. Chaque action reste désactivée jusqu&apos;à ce que les lignes de capacité en cache disent disponibles et que l&apos;aide garde ses prévols d&apos;exécution.</translation>
    </message>
    <message>
        <source>Full Repair plan</source>
        <translation>Plan de réparation complète</translation>
    </message>
    <message>
        <source>No stages selected</source>
        <translation>Aucune étape sélectionnée</translation>
    </message>
    <message>
        <source>Configure Plan...</source>
        <translation>Configurer le plan...</translation>
    </message>
    <message>
        <source>Open Settings to choose which Full Repair stages are part of the plan.</source>
        <translation>Ouvrez les paramètres pour choisir les étapes de réparation complètes qui font partie du plan.</translation>
    </message>
    <message>
        <source>Run Full Repair</source>
        <translation>Exécuter la réparation complète</translation>
    </message>
    <message>
        <source>Select a repair drive, or choose Host Maintenance on the protected running-host card.</source>
        <translation>Sélectionnez un lecteur de réparation, ou choisissez Maintenance de l&apos;hôte sur la carte d&apos;hôte d&apos;exécution protégée.</translation>
    </message>
    <message>
        <source>Stage</source>
        <translation>Étape</translation>
    </message>
    <message>
        <source>Individual repair tools</source>
        <translation>Outils de réparation individuels</translation>
    </message>
    <message>
        <source>Tool</source>
        <translation>Outil</translation>
    </message>
    <message>
        <source>Full Repair</source>
        <translation>Réparation complète</translation>
    </message>
    <message>
        <source>not reported</source>
        <translation>non signalé</translation>
    </message>
    <message>
        <source>Selected tool</source>
        <translation>Outil sélectionné</translation>
    </message>
    <message>
        <source>Select a repair tool</source>
        <translation>Sélectionner un outil de réparation</translation>
    </message>
    <message>
        <source>Run Tool</source>
        <translation>Exécuter l&apos;outil</translation>
    </message>
    <message>
        <source>Select a tool to review its repair action.</source>
        <translation>Sélectionnez un outil pour revoir son action de réparation.</translation>
    </message>
    <message>
        <source>Write actions ask for confirmation and then run the helper&apos;s own runtime preflights; the GUI never weakens them. A repair that is not proven &apos;unchanged&apos; invalidates the cached diagnostics and disables the gated actions until diagnostics run again.</source>
        <translation>Écrire les actions demandent confirmation et ensuite exécuter les propres prévols de l&apos;aide; l&apos;interface graphique ne les affaiblit jamais. Une réparation qui n&apos;est pas prouvée « non changée » invalide le diagnostic mis en cache et désactive les actions sécurisées jusqu&apos;à ce que le diagnostic soit à nouveau exécuté.</translation>
    </message>
    <message>
        <source>Chroot shell</source>
        <translation>Shell chroot</translation>
    </message>
    <message>
        <source>Offline commands run one at a time in a fresh chroot and cannot answer interactive prompts (apt-get -y upgrade works). Host-shell commands run directly on the running host. The helper&apos;s probe lines gate the command field; the exact reason appears in the tooltip.</source>
        <translation>Les commandes hors ligne s&apos;exécutent une à la fois dans un chroot frais et ne peuvent répondre à des invites interactives (apt-get -y upgrade works). Les commandes host-shell fonctionnent directement sur l&apos;hôte en cours d&apos;exécution. Les lignes de la sonde de l&apos;aide vont dans le champ de commande ; la raison exacte apparaît dans l&apos;outil.</translation>
    </message>
    <message>
        <source>No &apos;Legacy feature shell:&apos; line is cached; run diagnostics for the selected scope to evaluate the helper&apos;s chroot/timeout containment probes (fail closed).</source>
        <translation>Aucune ligne &apos;Legacy feature shell:&apos; n&apos;est mise en cache; exécutez un diagnostic pour la portée sélectionnée afin d&apos;évaluer les sondes de confinement chroot/timeout de l&apos;aide (fail close).</translation>
    </message>
    <message>
        <source>Command</source>
        <translation>Commande</translation>
    </message>
    <message>
        <source>Command:</source>
        <translation>Commande :</translation>
    </message>
    <message>
        <source>One reviewed command string, passed to the helper as a single argument (no shell interpolation by the GUI).</source>
        <translation>Une chaîne de commande a été revue, passée à l&apos;aide comme argument unique (pas d&apos;interpolation de shell par l&apos;interface graphique).</translation>
    </message>
    <message>
        <source>Run Command</source>
        <translation>Exécuter la commande</translation>
    </message>
    <message>
        <source>Clear Output</source>
        <translation>Effacer la sortie</translation>
    </message>
    <message>
        <source>The helper exposes `shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for an offline target chroot and `host-shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for the running host. Both keep the helper&apos;s runtime preflights; this tab enables the command only when the scope is committed, the session is authorized and the scope&apos;s Legacy feature probe reports available.</source>
        <translation>L&apos;aide expose `shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` pour une cible hors ligne chroot et `host-shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` pour l&apos;hôte en cours d&apos;exécution. Les deux gardent les prévols d&apos;exécution de l&apos;aide; cet onglet permet la commande seulement lorsque la portée est engagée, la session est autorisée et les rapports de la sonde de la fonctionnalité Legacy de la portée sont disponibles.</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Copie de fichiers</translation>
    </message>
    <message>
        <source>Copy and verify files in either direction through the guarded helper (cp -a plus ownership restoration and a per-file byte-compare). The helper&apos;s file-copy probe gates the controls and keeps the direction and path containment checks.</source>
        <translation>Copier et vérifier les fichiers dans les deux sens par l&apos;intermédiaire de l&apos;aide gardée (cp -a plus la restauration de la propriété et un byte-compare par fichier). La sonde de copie de fichier de l&apos;aide entre dans les commandes et garde la direction et la trajectoire des contrôles de confinement.</translation>
    </message>
    <message>
        <source>Preview Changes</source>
        <translation>Aperçu des modifications</translation>
    </message>
    <message>
        <source>Run a copy dry-run through the guarded helper. No files are changed.</source>
        <translation>Passez une copie à sec par l&apos;aide gardée. Aucun fichier n&apos;est changé.</translation>
    </message>
    <message>
        <source>Copy and Verify</source>
        <translation>Copier et vérifier</translation>
    </message>
    <message>
        <source>Copy staged items and verify the result. Existing destination names are overwritten when source content differs; unrelated destination files are never deleted.</source>
        <translation>Copiez les éléments mis en scène et vérifiez le résultat. Les noms de destination existants sont écrasés lorsque le contenu de la source diffère; les fichiers de destination non liés ne sont jamais supprimés.</translation>
    </message>
    <message>
        <source>Direction:</source>
        <translation>Direction:</translation>
    </message>
    <message>
        <source>Choose which system supplies the source files and which system receives them.</source>
        <translation>Choisissez quel système fournit les fichiers sources et quel système les reçoit.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from this host</source>
        <translation>1. Sélectionnez les fichiers source ou les dossiers de cet hôte</translation>
    </message>
    <message>
        <source>Source</source>
        <translation>Source</translation>
    </message>
    <message>
        <source>Files and folders staged for the verified copy. The legacy backend copies with cp -a and restores ownership with chown --reference; every regular file is byte-compared after the copy.</source>
        <translation>Fichiers et dossiers mis en scène pour la copie vérifiée. L&apos;héritage des copies de backend avec cp -a et restaure la propriété avec chown --reference; chaque fichier régulier est comparé par octets après la copie.</translation>
    </message>
    <message>
        <source>Add Files...</source>
        <translation>Ajouter des fichiers...</translation>
    </message>
    <message>
        <source>Add Folder...</source>
        <translation>Ajouter un dossier...</translation>
    </message>
    <message>
        <source>Remove</source>
        <translation>Supprimer</translation>
    </message>
    <message>
        <source>Clear</source>
        <translation>Effacer</translation>
    </message>
    <message>
        <source>Clear the staged source list (nothing is copied or deleted).</source>
        <translation>Effacer la liste des sources par étapes (rien n&apos;est copié ou supprimé).</translation>
    </message>
    <message>
        <source>2. Choose destination in repaired system</source>
        <translation>2. Choisissez la destination dans le système réparé</translation>
    </message>
    <message>
        <source>unavailable: see the helper&apos;s Legacy feature file-copy: probe reason above</source>
        <translation>non disponible : voir la copie de la fonction Legacy de l&apos;aide : raison de la sonde ci-dessus</translation>
    </message>
    <message>
        <source>An absolute path inside the selected repair system (Host to Repair) or on the running host (Repair to Host).</source>
        <translation>Un chemin absolu à l&apos;intérieur du système de réparation sélectionné (Host to Repair) ou sur l&apos;hôte en cours d&apos;exécution (Repair to Host).</translation>
    </message>
    <message>
        <source>Browse Target Folders...</source>
        <translation>Parcourir les dossiers cibles...</translation>
    </message>
    <message>
        <source>Browse the selected repair system through the helper&apos;s temporary read-only mounts and choose an absolute destination path. No target files are changed while browsing.</source>
        <translation>Parcourez le système de réparation sélectionné à travers les montages temporaires en lecture seule de l&apos;aide et choisissez un chemin de destination absolu. Aucun fichier cible n&apos;est changé pendant la navigation.</translation>
    </message>
    <message>
        <source>3. Ownership and copy policy</source>
        <translation>3. Politique de propriété et de copie</translation>
    </message>
    <message>
        <source>Ownership:</source>
        <translation>Propriété :</translation>
    </message>
    <message>
        <source>Smart destination ownership (recommended)</source>
        <translation>Propriété intelligente des destinations (recommandé)</translation>
    </message>
    <message>
        <source>Preserve source numeric UID/GID</source>
        <translation>Préserver la source numérique UID/GID</translation>
    </message>
    <message>
        <source>Smart mode validates UID/GID identity mapping across the two systems and falls back to the destination-directory owner when the same numeric ID means a different account (the legacy backend implements it with chown --reference).</source>
        <translation>Le mode intelligent valide la cartographie d&apos;identité UID/GID à travers les deux systèmes et revient au propriétaire du répertoire de destination lorsque le même ID numérique signifie un compte différent (le backend l&apos;implémente avec chown --reference).</translation>
    </message>
    <message>
        <source>Application log</source>
        <translation>Journal des applications</translation>
    </message>
    <message>
        <source>The complete session register; Save As... writes every entry even while a filter hides lines. If a writable system share mount exists at /host, Save As... starts there; otherwise the log directory is the fallback. Prior session files are listed read-only.</source>
        <translation>Le registre de session complet ; Save As... écrit chaque entrée même quand un filtre cache des lignes. Si un système enregistrable partage mount existe à /host, Enregistrer sous... commence là ; sinon le répertoire log est le retour. Les fichiers des sessions antérieures sont listés en lecture seule.</translation>
    </message>
    <message>
        <source>Session logs</source>
        <translation>Journaux de session</translation>
    </message>
    <message>
        <source>Session</source>
        <translation>Séance</translation>
    </message>
    <message>
        <source>The first entry is the live session; earlier files in the log directory are listed read-only below it.</source>
        <translation>La première entrée est la session en direct; les fichiers antérieurs dans le répertoire log sont listés en lecture seule ci-dessous.</translation>
    </message>
    <message>
        <source>New Session Log</source>
        <translation>Nouveau journal de session</translation>
    </message>
    <message>
        <source>Close the active session file; it becomes a prior session and the next log entry starts a new file.</source>
        <translation>Fermez le fichier de session actif; il devient une session antérieure et la prochaine entrée de journal commence un nouveau fichier.</translation>
    </message>
    <message>
        <source>Add Note</source>
        <translation>Ajouter une note</translation>
    </message>
    <message>
        <source>Append a NOTE entry to the live session register.</source>
        <translation>Ajouter une entrée NOTE au registre de session en direct.</translation>
    </message>
    <message>
        <source>Delete</source>
        <translation>Supprimer</translation>
    </message>
    <message>
        <source>Delete the selected prior session file (the live session is never deleted).</source>
        <translation>Supprimer le fichier de session précédente sélectionné (la session en direct n&apos;est jamais supprimée).</translation>
    </message>
    <message>
        <source>Refresh</source>
        <translation>Actualiser</translation>
    </message>
    <message>
        <source>Save As...</source>
        <translation>Enregistrer sous...</translation>
    </message>
    <message>
        <source>Save the complete session log (all entries, not just the current filter).</source>
        <translation>Enregistrer le journal de session complet (toutes les entrées, pas seulement le filtre actuel).</translation>
    </message>
    <message>
        <source>Clear Register</source>
        <translation>Effacer le registre</translation>
    </message>
    <message>
        <source>Clear the live register and view; prior session files are never modified.</source>
        <translation>Effacer le registre en direct et afficher; les fichiers de session précédente ne sont jamais modifiés.</translation>
    </message>
    <message>
        <source>Search log:</source>
        <translation>Journal de recherche :</translation>
    </message>
    <message>
        <source>Type any characters to show matching log entries (case-insensitive). Save As always writes every entry.</source>
        <translation>Saisissez tous les caractères pour afficher les entrées de journal correspondantes (insensibles aux cas). Enregistrer Comme toujours écrit chaque entrée.</translation>
    </message>
    <message>
        <source>Filter:</source>
        <translation>Filtre &amp;#160;:</translation>
    </message>
    <message>
        <source>Filter the visible log by entry kind. Selecting a diagnostic section shows the lines captured for that section; a workflow filter such as File system repair or Package repair shows its mapped repair lines (File copy has no lines in this frontend). Save As always writes every entry.</source>
        <translation>Filtrer le journal visible par type d&apos;entrée. La sélection d&apos;une section de diagnostic montre les lignes saisies pour cette section; un filtre de flux de travail comme la réparation du système de fichiers ou la réparation du paquet montre ses lignes de réparation cartographiées (la copie de fichier n&apos;a pas de lignes dans cette façade). Enregistrer Comme toujours écrit chaque entrée.</translation>
    </message>
    <message>
        <source>Settings are stored per user under ~/.qt/, one file per settings group (devicesrc, logsrc, diagnosticsrc, repairrc), and are saved immediately on every change and on close. Launch the GUI as the same user to keep your overrides; a GUI started as root keeps its own copies.</source>
        <translation>Les paramètres sont stockés par utilisateur sous ~/.qt/, un fichier par groupe de paramètres (devicesrc, logrc, diagnosticsrc, réparrc), et sont enregistrés immédiatement sur chaque changement et à la fin. Lancez l&apos;interface graphique en tant que même utilisateur pour conserver vos commandes; une interface graphique a commencé comme racine conserve ses propres copies.</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Découverte des périphériques</translation>
    </message>
    <message>
        <source>Show devices without an identified Linux installation</source>
        <translation>Afficher les périphériques sans installation Linux identifiée</translation>
    </message>
    <message>
        <source>Show removable and USB storage</source>
        <translation>Afficher le stockage amovible et USB</translation>
    </message>
    <message>
        <source>Show encrypted devices before unlocking</source>
        <translation>Afficher les périphériques chiffrés avant le déverrouillage</translation>
    </message>
    <message>
        <source>When off, drives without a visible Linux filesystem are hidden unless they still contain an encrypted device and encrypted devices are shown.</source>
        <translation>Lorsqu&apos;ils sont éteints, les lecteurs sans système de fichiers Linux visible sont cachés à moins qu&apos;ils ne contiennent encore un périphérique chiffré et que les périphériques chiffrés soient affichés.</translation>
    </message>
    <message>
        <source>When off, removable and USB drives are hidden from the repair-target list.</source>
        <translation>Lorsqu&apos;ils sont éteints, les lecteurs amovibles et USB sont cachés de la liste des cibles de réparation.</translation>
    </message>
    <message>
        <source>When off, drives with an encrypted device are hidden until the volume is unlocked.</source>
        <translation>Une fois éteint, les disques avec un périphérique chiffré sont cachés jusqu&apos;à ce que le volume soit déverrouillé.</translation>
    </message>
    <message>
        <source>Include the %1 stage in the Full Repair plan. The stage runs in the plan order shown on the Repair tab.</source>
        <translation>Inclure l&apos;étape %1 dans le plan de réparation complet. L&apos;étape tourne dans l&apos;ordre de plan affiché sur l&apos;onglet Réparation.</translation>
    </message>
    <message>
        <source>Automatically regenerate read-only diagnostics after repairs or target changes</source>
        <translation>Régénérer automatiquement les diagnostics en lecture seule après les réparations ou les changements de cible</translation>
    </message>
    <message>
        <source>Regenerates the cached read-only diagnostics for the current scope after an operation that invalidates them (LUKS unlock, target configuration edit). It runs only inside an already authorized administrator session and never opens an authorization prompt by itself.</source>
        <translation>Régénère les diagnostics en lecture seule mis en cache pour la portée actuelle après une opération qui les invalide (déverrouillage LUKS, modification de la configuration de la cible). Il fonctionne seulement à l&apos;intérieur d&apos;une session d&apos;administrateur déjà autorisée et n&apos;ouvre jamais une invitation d&apos;autorisation par lui-même.</translation>
    </message>
    <message>
        <source>Wrap long log lines</source>
        <translation>Envelopper les longues lignes de log</translation>
    </message>
    <message>
        <source>Mandatory safety controls</source>
        <translation>Contrôles de sécurité obligatoires</translation>
    </message>
    <message>
        <source>Host capabilities and dependencies</source>
        <translation>Capacités et dépendances de l&apos;hôte</translation>
    </message>
    <message>
        <source>Refresh Capabilities</source>
        <translation>Rafraîchir les capacités</translation>
    </message>
    <message>
        <source>Re-run the read-only host capability probes (a PATH search, nothing is executed) and refresh the distribution summary.</source>
        <translation>Renouvelez les sondes de capacité d&apos;hôte en lecture seule (une recherche PATH, rien n&apos;est exécuté) et rafraîchissez le résumé de distribution.</translation>
    </message>
    <message>
        <source>Install Missing Support...</source>
        <translation>Installer le support manquant...</translation>
    </message>
    <message>
        <source>Automatic installation will require explicit package mapping and privilege authorization.</source>
        <translation>L&apos;installation automatique nécessitera une cartographie explicite des paquets et une autorisation de privilège.</translation>
    </message>
    <message>
        <source>Application configuration</source>
        <translation>Configuration de l&apos;application</translation>
    </message>
    <message>
        <source>Boot Bitch Legacy</source>
        <translation>Boot Bitch Héritage</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list first.</source>
        <translation>Sélectionnez d&apos;abord un lecteur physique dans la liste des cibles de réparation disponibles.</translation>
    </message>
    <message>
        <source>Protected system</source>
        <translation>Système protégé</translation>
    </message>
    <message>
        <source>The running system cannot be selected as a repair target. Use Host Maintenance for the protected running host or choose another disk.</source>
        <translation>Le système d&apos;exécution ne peut pas être sélectionné comme cible de réparation. Utilisez Maintenance de l&apos;hôte pour l&apos;hôte protégé ou choisissez un autre disque.</translation>
    </message>
    <message>
        <source>Unlock or select a Linux system first</source>
        <translation>Déverrouillez ou sélectionnez d&apos;abord un système Linux</translation>
    </message>
    <message>
        <source>This encrypted drive has no visible Linux filesystem yet. Use Unlock, refresh devices, and select the target after its Linux root is detected.</source>
        <translation>Ce lecteur chiffré n&apos;a pas encore de système de fichiers Linux visible. Utilisez Déverrouiller, rafraîchir les appareils et sélectionnez la cible après la détection de sa racine Linux.</translation>
    </message>
    <message>
        <source>The selected root component (%1) belongs to the running system and cannot be committed as a repair target. Use Host Maintenance for the protected running host.</source>
        <translation>Le composant racine sélectionné (%1) appartient au système en cours d&apos;exécution et ne peut pas être engagé comme cible de réparation. Utiliser la maintenance de l&apos;hôte pour l&apos;hôte en cours d&apos;exécution protégé.</translation>
    </message>
    <message>
        <source>repair target commit</source>
        <translation>commit cible de réparation</translation>
    </message>
    <message>
        <source>Repair drive selected: %1</source>
        <translation>Le lecteur de réparation sélectionné: %1</translation>
    </message>
    <message>
        <source>; best detected system component: %1</source>
        <translation>; composant système le mieux détecté: %1</translation>
    </message>
    <message>
        <source>. No mount or repair action was performed.</source>
        <translation>. Aucune action de montage ou de réparation n&apos;a été effectuée.</translation>
    </message>
    <message>
        <source>Make Default unavailable</source>
        <translation>Définir par défaut indisponible</translation>
    </message>
    <message>
        <source>Make Default is a running-host action on this frontend; enter Host Maintenance first.</source>
        <translation>Make Default est une action de l&apos;hôte en cours d&apos;exécution sur cette interface; entrez d&apos;abord Maintenance de l&apos;hôte.</translation>
    </message>
    <message>
        <source>Administrator authorization required</source>
        <translation>Autorisation de l&apos;administrateur requise</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize first.</source>
        <translation>Aucune session administrateur n&apos;est active; appuyez d&apos;abord sur Autoriser.</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host?

The helper verifies /boot/grub/menu.lst, sets the `default &lt;N&gt;` directive to the canonical entry, backs the menu up first and restores it on any failure.</source>
        <translation>Faire de l&apos;entrée canonique installée du noyau l&apos;entrée par défaut du démarrage GRUB-legacy sur l&apos;hôte en cours d&apos;exécution ?

L&apos;aide vérifie /boot/grub/menu.lst, définit la directive `default &lt;N&gt;` à l&apos;entrée canonique, sauvegarde le menu en premier et la restaure sur toute défaillance.</translation>
    </message>
    <message>
        <source>Host Maintenance unavailable</source>
        <translation>Maintenance de l&apos;hôte indisponible</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a committed repair target or a detected running host.</source>
        <translation>La cible d&apos;hôte en cours d&apos;exécution n&apos;a pas pu être détectée; les diagnostics nécessitent une cible de réparation engagée ou un hôte en cours d&apos;exécution détecté.</translation>
    </message>
    <message>
        <source>Exit Host Maintenance</source>
        <translation>Quitter la maintenance de l&apos;hôte</translation>
    </message>
    <message>
        <source>Host Maintenance is active; diagnostics and gated repairs target the protected running host.</source>
        <translation>L&apos;entretien de l&apos;hôte est actif; les diagnostics et les réparations sécurisées ciblent l&apos;hôte protégé.</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list on the Systems tab first, or use Host Maintenance for the protected running host.</source>
        <translation>Sélectionnez un lecteur physique dans la liste des cibles de réparation disponibles dans l&apos;onglet Systèmes d&apos;abord, ou utilisez Maintenance de l&apos;hôte pour l&apos;hôte d&apos;exécution protégé.</translation>
    </message>
    <message>
        <source>The selected drive (%1) is the protected running host. Choose Host Maintenance on the Systems tab to run read-only host diagnostics and guarded host repairs; ordinary target repairs stay disabled.</source>
        <translation>Le lecteur sélectionné (%1) est l&apos;hôte d&apos;exécution protégé. Choisissez Maintenance de l&apos;hôte sur l&apos;onglet Systèmes pour exécuter en lecture seule des diagnostics d&apos;hôte et des réparations d&apos;hôte gardées; les réparations de cible ordinaires restent désactivées.</translation>
    </message>
    <message>
        <source>No repair target is committed. Choose Select Target on the Systems tab (or Host Maintenance for the protected running host) first.</source>
        <translation>Aucune cible de réparation n&apos;est engagée. Choisissez Sélectionner la cible dans l&apos;onglet Systèmes (ou Maintenance d&apos;hôte pour l&apos;hôte protégé).</translation>
    </message>
    <message>
        <source>The selection changed after the target was committed. Choose Select Target again.</source>
        <translation>La sélection a changé après l&apos;engagement de la cible. Choisissez à nouveau Select Target.</translation>
    </message>
    <message>
        <source>Diagnostics scope required</source>
        <translation>Portée de diagnostic requise</translation>
    </message>
    <message>
        <source>Diagnostics scope unresolved</source>
        <translation>Portée de diagnostic non résolue</translation>
    </message>
    <message>
        <source>The running host target could not be resolved; use Refresh Devices and commit a repair target or re-enter Host Maintenance.</source>
        <translation>La cible de l&apos;hôte en cours d&apos;exécution n&apos;a pas pu être résolue; utiliser les appareils Rafraîchir et lancer une cible de réparation ou réentrer la maintenance de l&apos;hôte.</translation>
    </message>
    <message>
        <source>Diagnostic check required</source>
        <translation>Vérification de diagnostic requise</translation>
    </message>
    <message>
        <source>Select a diagnostic check in the list first.</source>
        <translation>Sélectionnez d&apos;abord une vérification diagnostique dans la liste.</translation>
    </message>
    <message>
        <source>Repair target required</source>
        <translation>Objectif de réparation requis</translation>
    </message>
    <message>
        <source>Target file editing needs a committed repair target. Running-host maintenance has no target-file editing; commit an offline target first.</source>
        <translation>L&apos;édition de fichier cible nécessite une cible de réparation engagée. La maintenance de l&apos;hôte-exécuter n&apos;a pas d&apos;édition de fichier cible; commit une cible hors ligne d&apos;abord.</translation>
    </message>
    <message>
        <source>Configuration file required</source>
        <translation>Fichier de configuration requis</translation>
    </message>
    <message>
        <source>Select a target configuration file first.</source>
        <translation>Sélectionnez d&apos;abord un fichier de configuration cible.</translation>
    </message>
    <message>
        <source>Edit target %1</source>
        <translation>Modifier la cible %1</translation>
    </message>
    <message>
        <source>Edit this target file through the guarded administrator helper. A successful save invalidates cached diagnostics; rerun diagnostics before repair. Generated files such as /boot/grub/menu.lst may be replaced by the next bootloader update.</source>
        <translation>Modifier ce fichier cible par l&apos;aide de l&apos;administrateur protégé. Un succès de sauvegarde invalide les diagnostics mis en cache; relance le diagnostic avant réparation. Les fichiers générés tels que /boot/grub/menu.lst peuvent être remplacés par la prochaine mise à jour de bootloader.</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Annuler</translation>
    </message>
    <message>
        <source>Save Target File</source>
        <translation>Enregistrer le fichier cible</translation>
    </message>
    <message>
        <source>No changes to %1.</source>
        <translation>Aucun changement à %1.</translation>
    </message>
    <message>
        <source>Configuration write refused</source>
        <translation>Configuration écrite refusée</translation>
    </message>
    <message>
        <source>The edited content contains NUL bytes; the guarded write refuses it.</source>
        <translation>Le contenu édité contient des octets NUL ; l&apos;écriture gardée le refuse.</translation>
    </message>
    <message>
        <source>File too large</source>
        <translation>Dossier trop grand</translation>
    </message>
    <message>
        <source>The edited file is larger than 1 MiB. The guarded write refuses it; edit the file from a console instead.</source>
        <translation>Le fichier édité est plus grand que 1 MiB. L&apos;écriture gardée le refuse ; éditer le fichier à partir d&apos;une console à la place.</translation>
    </message>
    <message>
        <source>Write target configuration</source>
        <translation>Écrire la configuration de la cible</translation>
    </message>
    <message>
        <source>Write the edited contents to %1? This modifies the repair target and invalidates cached diagnostics.</source>
        <translation>Écrire le contenu édité à %1? Cela modifie la cible de réparation et invalide les diagnostics mis en cache.</translation>
    </message>
    <message>
        <source>No diagnostic results to copy yet.</source>
        <translation>Aucun résultat diagnostique à copier.</translation>
    </message>
    <message>
        <source>Diagnostic results copied to the clipboard.</source>
        <translation>Les résultats diagnostiques ont été copiés dans le presse-papiers.</translation>
    </message>
    <message>
        <source>No diagnostic results to save yet.</source>
        <translation>Aucun résultat diagnostique à enregistrer.</translation>
    </message>
    <message>
        <source>Text files (*.txt);;All files (*)</source>
        <translation>Fichiers texte (*.txt);;Tous les fichiers (*)</translation>
    </message>
    <message>
        <source>Save diagnostic results</source>
        <translation>Enregistrer les résultats du diagnostic</translation>
    </message>
    <message>
        <source>Could not write %1.</source>
        <translation>Impossible d&apos;écrire %1.</translation>
    </message>
    <message>
        <source>Diagnostic results saved to %1</source>
        <translation>Résultats diagnostiques enregistrés sur %1</translation>
    </message>
    <message>
        <source>Unlock not available</source>
        <translation>Déverrouillage indisponible</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked. Select an offline repair target to unlock.</source>
        <translation>L&apos;hôte d&apos;exécution protégé ne peut pas être déverrouillé. Sélectionnez une cible de réparation hors ligne à débloquer.</translation>
    </message>
    <message>
        <source>No locked LUKS component is currently visible on this selected drive.</source>
        <translation>Aucun composant LUKS verrouillé n&apos;est actuellement visible sur ce lecteur sélectionné.</translation>
    </message>
    <message>
        <source>Confirm LUKS unlock</source>
        <translation>Confirmer le déverrouillage LUKS</translation>
    </message>
    <message>
        <source>Unlock %1 on %2?

The helper opens a temporary device-mapper mapping with cryptsetup and keeps it open for this recovery session. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>Déverrouiller %1 sur %2 ?

L&apos;aide ouvre une cartographie temporaire de périphérique-mapping avec cryptsetup et la garde ouverte pour cette session de récupération. La phrase de passe traverse un fichier de clés privé et n&apos;est jamais placée dans des arguments de commande ou des journaux.</translation>
    </message>
    <message>
        <source>LUKS unlock</source>
        <translation>Déverrouillage LUKS</translation>
    </message>
    <message>
        <source>Unlock LUKS repair target</source>
        <translation>Déverrouiller la cible de réparation LUKS</translation>
    </message>
    <message>
        <source>Enter the passphrase for %1.

It is sent only to cryptsetup over the helper&apos;s standard input and is never logged or placed on a command line.</source>
        <translation>Saisissez la phrase de passe pour %1.

Il est envoyé uniquement à cryptsetup sur l&apos;entrée standard de l&apos;aide et n&apos;est jamais enregistré ou placé sur une ligne de commande.</translation>
    </message>
    <message>
        <source>Passphrase required</source>
        <translation>Phrase secrète requise</translation>
    </message>
    <message>
        <source>An empty passphrase was not submitted. Enter the LUKS passphrase or choose Cancel.</source>
        <translation>Une phrase de passe vide n&apos;a pas été soumise. Entrez la phrase de passe LUKS ou choisissez Annuler.</translation>
    </message>
    <message>
        <source>Unlock keyfile unavailable</source>
        <translation>Déverrouiller le fichier clé non disponible</translation>
    </message>
    <message>
        <source>The LUKS passphrase could not be written to a private keyfile in %1; the unlock was not started.</source>
        <translation>La phrase de passe LUKS ne pouvait pas être écrite dans un fichier de clés privé dans %1; le déverrouillage n&apos;a pas été démarré.</translation>
    </message>
    <message>
        <source>Configuration write unavailable</source>
        <translation>Configuration écrire indisponible</translation>
    </message>
    <message>
        <source>The edited content could not be written to a private temporary file in %1; the write was not started.</source>
        <translation>Le contenu édité ne pouvait pas être écrit à un fichier temporaire privé dans %1; l&apos;écriture n&apos;a pas été commencée.</translation>
    </message>
    <message>
        <source>ERROR: cannot read the session log %1</source>
        <translation>ERROR: ne peut pas lire le journal de session %1</translation>
    </message>
    <message>
        <source>Viewing a prior session log (read-only): %1</source>
        <translation>Affichage d&apos;un journal de session antérieur (lecture seule): %1</translation>
    </message>
    <message>
        <source>Session log list refreshed.</source>
        <translation>La liste des sessions est rafraîchie.</translation>
    </message>
    <message>
        <source>Note:</source>
        <translation>Remarque:</translation>
    </message>
    <message>
        <source>Delete session log</source>
        <translation>Supprimer le journal des sessions</translation>
    </message>
    <message>
        <source>Delete %1 permanently? This cannot be undone.</source>
        <translation>Supprimer définitivement %1? Cela ne peut pas être annulé.</translation>
    </message>
    <message>
        <source>%1 changed while the confirmation was open; the delete was refused.</source>
        <translation>%1 a changé pendant que la confirmation était ouverte; la suppression a été refusée.</translation>
    </message>
    <message>
        <source>Unable to delete %1.</source>
        <translation>Impossible de supprimer %1.</translation>
    </message>
    <message>
        <source>Device discovery filters updated.</source>
        <translation>Filtres de découverte de périphérique mis à jour.</translation>
    </message>
    <message>
        <source>Unknown Linux distribution</source>
        <translation>Distribution Linux inconnue</translation>
    </message>
    <message>
        <source>sudo / gksu (no KAuth on this frontend)</source>
        <translation>sudo / gksu (pas de KAuth sur cette façade)</translation>
    </message>
    <message>
        <source>Available</source>
        <translation>Disponible</translation>
    </message>
    <message>
        <source>Missing on this frontend</source>
        <translation>Il manque sur cette façade</translation>
    </message>
    <message>
        <source>Missing</source>
        <translation>Manque</translation>
    </message>
    <message>
        <source>No repair tool is selected.</source>
        <translation>Aucun outil de réparation sélectionné.</translation>
    </message>
    <message>
        <source>A helper command is already running.</source>
        <translation>Une commande d&apos;aide est déjà en cours.</translation>
    </message>
    <message>
        <source>Restore Graphical Login is a host-scope stage on this legacy frontend; enter Host Maintenance to run it.</source>
        <translation>Restaurer Graphical Login est une étape de l&apos;hôte-scope sur cette façade; entrez Maintenance de l&apos;hôte pour l&apos;exécuter.</translation>
    </message>
    <message>
        <source>The selected scope has no resolved root component; use Refresh Devices and commit the repair target again.</source>
        <translation>La portée sélectionnée n&apos;a pas de composant racine résolu; utilisez Rénover les appareils et validez à nouveau la cible de réparation.</translation>
    </message>
    <message>
        <source>This legacy frontend exposes no %1 action; the helper reports the capability as available.</source>
        <translation>Cette façade n&apos;expose aucune action %1; l&apos;aide signale la capacité disponible.</translation>
    </message>
    <message>
        <source>Run this guarded repair action using the cached read-only diagnostic evidence. A confirmation is shown first.</source>
        <translation>Exécutez cette action de réparation protégée en utilisant les preuves de diagnostic en lecture seule mises en cache. Une confirmation est d&apos;abord affichée.</translation>
    </message>
    <message>
        <source>Enabled in Settings</source>
        <translation>Activé dans les paramètres</translation>
    </message>
    <message>
        <source>Disabled in Settings - enable it to include this stage</source>
        <translation>Handicapés dans les paramètres - lui permettre d&apos;inclure cette étape</translation>
    </message>
    <message>
        <source>Unavailable: %1</source>
        <translation>Indisponible : %1</translation>
    </message>
    <message>
        <source>
Unavailable: %1</source>
        <translation>
Indisponible : %1</translation>
    </message>
    <message>
        <source>Repair tool unavailable</source>
        <translation>Outil de réparation indisponible</translation>
    </message>
    <message>
        <source>Confirm repair</source>
        <translation>Confirmer la réparation</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Your saved selection is kept and its availability is re-checked when diagnostics for this scope complete.</source>
        <translation>Aucune preuve de capacité mise en cache pour cette étape. Votre sélection sauvegardée est conservée et sa disponibilité est revérifiée lorsque les diagnostics pour cette portée sont complets.</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Run diagnostics for the selected scope to populate the Full Repair plan; your selection is saved once the stage becomes available.</source>
        <translation>Aucune preuve de capacité mise en cache pour cette étape. Exécutez des diagnostics pour la portée sélectionnée pour remplir le plan de réparation complète; votre sélection est sauvegardée une fois que l&apos;étape devient disponible.</translation>
    </message>
    <message>
        <source>Unknown Full Repair stage.</source>
        <translation>Étape de réparation complète inconnue.</translation>
    </message>
    <message>
        <source>No Full Repair stages are selected or available; use Configure Plan... to choose the stages.</source>
        <translation>Aucune étape de réparation complète n&apos;est sélectionnée ou disponible; utilisez Configurer Plan... pour choisir les étapes.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab to establish the session.</source>
        <translation>Autorisation de l&apos;administrateur est nécessaire; appuyez sur Autoriser sur l&apos;onglet Systèmes ou Réparation pour établir la session.</translation>
    </message>
    <message>
        <source>Runs the selected stages using the cached read-only diagnostic evidence after privilege confirmation.</source>
        <translation>Exécute les étapes sélectionnées en utilisant la preuve de diagnostic en lecture seule mise en cache après la confirmation de privilège.</translation>
    </message>
    <message>
        <source>No Full Repair stages selected - use Configure Plan... or Settings.</source>
        <translation>Aucune étape de réparation complète sélectionnée - utilisez Configurer le plan... ou les paramètres.</translation>
    </message>
    <message>
        <source>1 stage selected</source>
        <translation>1 étape sélectionnée</translation>
    </message>
    <message>
        <source>%1 stages selected</source>
        <translation>Étapes %1 sélectionnées</translation>
    </message>
    <message>
        <source>No repair stages are selected. Use Configure Plan... to choose the stages Full Repair will run.</source>
        <translation>Aucune étape de réparation n&apos;est sélectionnée. Utilisez Configurer Plan... pour choisir les étapes Full Repair s&apos;exécutera.</translation>
    </message>
    <message>
        <source>No selected stages are available. %1</source>
        <translation>Aucune étape sélectionnée n&apos;est disponible. %1</translation>
    </message>
    <message>
        <source>Full Repair unavailable</source>
        <translation>Réparation complète non disponible</translation>
    </message>
    <message>
        <source>Run the Full Repair plan?

The selected stages run in order through the helper&apos;s guarded repair command:

</source>
        <translation>Exécuter le plan complet de réparation ?

Les étapes sélectionnées s&apos;exécutent dans l&apos;ordre par la commande de réparation surveillée de l&apos;aide :

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
L&apos;aide garde chaque prévol d&apos;exécution; une étape qui échoue arrête le plan.</translation>
    </message>
    <message>
        <source>Running %1 through the privileged helper... The Logs tab keeps the complete transcript.</source>
        <translation>Exécuter %1 à travers l&apos;aide privilégiée... L&apos;onglet Logs conserve la transcription complète.</translation>
    </message>
    <message>
        <source>Shell scope required</source>
        <translation>Portée Shell requise</translation>
    </message>
    <message>
        <source>No administrator session is active.

Press Authorize on the Systems or Repair tab to establish the session, or enter Host Maintenance / commit a repair target on the Systems tab; the chroot shell then reuses the cached authorization.</source>
        <translation>Aucune session d&apos;administrateur n&apos;est active.

Appuyez sur Autoriser sur l&apos;onglet Systèmes ou Réparation pour établir la session, ou entrez Maintenance de l&apos;hôte / commit une cible de réparation sur l&apos;onglet Systèmes ; le shell chroot réutilise ensuite l&apos;autorisation mise en cache.</translation>
    </message>
    <message>
        <source>Shell command required</source>
        <translation>Commande Shell requise</translation>
    </message>
    <message>
        <source>Enter the command to run first.</source>
        <translation>Saisissez la commande à exécuter en premier.</translation>
    </message>
    <message>
        <source>Confirm running-host command</source>
        <translation>Confirmer la commande en cours d&apos;exécution-hôte</translation>
    </message>
    <message>
        <source>Run this command as root on the protected running host?

%1

The helper keeps its runtime preflights; the command is passed as one argument and is never interpreted by the GUI.</source>
        <translation>Exécuter cette commande en tant que racine sur l&apos;hôte d&apos;exécution protégé ?

%1

L&apos;aide garde ses prévols d&apos;exécution; la commande est passée comme un argument et n&apos;est jamais interprétée par l&apos;interface graphique.</translation>
    </message>
    <message>
        <source>Running %1...</source>
        <translation>Lancer %1...</translation>
    </message>
    <message>
        <source>Authorization scope required</source>
        <translation>Portée de l&apos;autorisation requise</translation>
    </message>
    <message>
        <source>administrator authorization</source>
        <translation>autorisation de l&apos;administrateur</translation>
    </message>
    <message>
        <source>Administrator authorization</source>
        <translation>Autorisation de l&apos;administrateur</translation>
    </message>
    <message>
        <source>Administrator authorization is required for %1.

Enter the password for %2 (sudo). It is used only for this sudo authentication, is sent over a pipe and is never logged or placed on a command line. The authorization is cached for this session and reused by diagnostics and repairs.</source>
        <translation>Une autorisation d&apos;administrateur est requise pour %1.

Saisissez le mot de passe pour %2 (sudo). Il est utilisé uniquement pour cette authentification sudo, est envoyé sur un tuyau et n&apos;est jamais enregistré ou placé sur une ligne de commande. L&apos;autorisation est mise en cache pour cette session et réutilisée par des diagnostics et des réparations.</translation>
    </message>
    <message>
        <source>your account</source>
        <translation>votre compte</translation>
    </message>
    <message>
        <source>Password required</source>
        <translation>Mot de passe requis</translation>
    </message>
    <message>
        <source>An empty password was not submitted. Enter the sudo password or choose Cancel.</source>
        <translation>Un mot de passe vide n&apos;a pas été soumis. Saisissez le mot de passe sudo ou choisissez Annuler.</translation>
    </message>
    <message>
        <source>Administrator authorization failed</source>
        <translation>Échec de l&apos;autorisation de l&apos;administrateur</translation>
    </message>
    <message>
        <source>sudo did not accept the password: %1

The command was not started.</source>
        <translation>sudo n&apos;a pas accepté le mot de passe: %1

Le commandement n&apos;a pas commencé.</translation>
    </message>
    <message>
        <source>elevation requires an interactive sudo password; run the smoke as root or after `sudo -S -v` with --elevate &apos;sudo -n&apos;</source>
        <translation>élévation nécessite un mot de passe sudo interactif; exécutez la fumée comme racine ou après `sudo -S -v` avec --elevate &apos;sudo -n&apos;</translation>
    </message>
    <message>
        <source>No administrator session is active for %1.

Press Authorize on the Systems or Repair tab to establish the session now, or enter Host Maintenance / commit a repair target on the Systems tab; diagnostics and repairs then reuse the cached authorization.</source>
        <translation>Aucune session administrateur n&apos;est active pour %1.

Appuyez sur Autoriser sur l&apos;onglet Systèmes ou Réparation pour établir la session maintenant, ou entrez Maintenance de l&apos;hôte / commit une cible de réparation sur l&apos;onglet Systèmes; diagnostics et réparations puis réutiliser l&apos;autorisation mise en cache.</translation>
    </message>
    <message>
        <source>Administrator authorization expired</source>
        <translation>Autorisation de l&apos;administrateur expirée</translation>
    </message>
    <message>
        <source>The cached administrator authorization for %1 expired or was refused.

Press Authorize on the Systems or Repair tab to re-establish the session, then run the command again. No command was started.</source>
        <translation>L&apos;autorisation d&apos;administrateur en cache pour %1 a expiré ou a été refusée.

Appuyez sur Autoriser sur l&apos;onglet Systèmes ou Réparation pour rétablir la session, puis exécutez à nouveau la commande. Aucun commandement n&apos;a été lancé.</translation>
    </message>
    <message>
        <source>Privileged operation completed successfully. Administrator authorization remains active for this session.</source>
        <translation>Opération privilégiée terminée avec succès. L&apos;autorisation de l&apos;administrateur demeure active pour cette session.</translation>
    </message>
    <message>
        <source>Privileged operation stopped with an error. Administrator authorization remains active; review the output before closing.</source>
        <translation>L&apos;opération privilégiée s&apos;est arrêtée avec une erreur. L&apos;autorisation de l&apos;administrateur demeure active; examiner le produit avant la clôture.</translation>
    </message>
    <message>
        <source>Inspection is read-only.

</source>
        <translation>L&apos;inspection est en lecture seule.

</translation>
    </message>
    <message>
        <source>Configuration unavailable</source>
        <translation>Configuration non disponible</translation>
    </message>
    <message>
        <source>The helper could not read %1:

%2</source>
        <translation>L&apos;aide n&apos;a pas pu lire %1:

%2</translation>
    </message>
    <message>
        <source>Configuration write failed</source>
        <translation>La configuration a échoué</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
Result: mapping opened for this recovery session.</source>
        <translation>État : déverrouillé
Composante : %1
Carter: %2
Méthode: déverrouillage de l&apos;aide (cryptsetup; passphrase via un fichier de clés mode-600, supprimé après utilisation)
Résultat : la cartographie est ouverte pour cette session de récupération.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: the passphrase was not accepted; retry offered.</source>
        <translation>État: verrouillé
Composante : %1
Méthode : déverrouillage de l&apos;aide (cryptsetup)
Erreur : la phrase de passe n&apos;a pas été acceptée; réessayer offert.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: %2</source>
        <translation>État: verrouillé
Composante : %1
Méthode : déverrouillage de l&apos;aide (cryptsetup)
Erreur : %2</translation>
    </message>
    <message>
        <source>Passphrase not accepted</source>
        <translation>Phrase secrète non acceptée</translation>
    </message>
    <message>
        <source>The LUKS passphrase was not accepted.

Try again?</source>
        <translation>La phrase de passe LUKS n&apos;a pas été acceptée.

Essayer encore ?</translation>
    </message>
    <message>
        <source>[-] %1 - not run (the plan stopped before reaching this stage)</source>
        <translation>[-] %1 - ne pas courir (le plan s&apos;est arrêté avant d&apos;atteindre ce stade)</translation>
    </message>
    <message>
        <source>filesystem</source>
        <translation>système de fichiers</translation>
    </message>
    <message>
        <source>[-] %1 - no file system errors found - no changes</source>
        <translation>[-] %1 - aucune erreur du système de fichiers trouvée - aucun changement</translation>
    </message>
    <message>
        <source>[FAIL] %1 - not reported</source>
        <translation>[FAIL] %1 - Non déclaré</translation>
    </message>
    <message>
        <source>%1 stage(s) failed; review the helper output in Logs.</source>
        <translation>%1 étape(s) a échoué; examiner la sortie de l&apos;aide dans Logs.</translation>
    </message>
    <message>
        <source>the plan stopped before any stage completed.</source>
        <translation>le plan s&apos;est arrêté avant toute étape terminée.</translation>
    </message>
    <message>
        <source>no repair was needed; cached diagnostics remain valid.</source>
        <translation>Aucune réparation n&apos;était nécessaire; les diagnostics mis en cache demeurent valides.</translation>
    </message>
    <message>
        <source>repair completed; cached diagnostics were invalidated and must be regenerated.</source>
        <translation>réparation terminée; les diagnostics en cache ont été invalidés et doivent être régénérés.</translation>
    </message>
    <message>
        <source>%1 results: [OK] %2 successful | [FAIL] %3 failed | [-] %4 no repair needed - %5</source>
        <translation>%1 résultats: [OK] %2 succès , [FAIL] %3 échec , [-] %4 aucune réparation nécessaire - %5</translation>
    </message>
    <message>
        <source>Re-run Diagnostic</source>
        <translation>Réexécuter le diagnostic</translation>
    </message>
    <message>
        <source>Unavailable</source>
        <translation>Indisponible</translation>
    </message>
    <message>
        <source>Commit this physical drive as the repair target.</source>
        <translation>Commettez ce disque physique comme cible de réparation.</translation>
    </message>
    <message>
        <source>Protection:</source>
        <translation>Protection :</translation>
    </message>
    <message>
        <source>Host shell</source>
        <translation>Shell de l&apos;hôte</translation>
    </message>
    <message>
        <source>Run on Host</source>
        <translation>Exécuter sur l&apos;hôte</translation>
    </message>
    <message>
        <source>Host Shell</source>
        <translation>Shell de l&apos;hôte</translation>
    </message>
    <message>
        <source>The read-only probe found no editable target configuration file in this target. %1</source>
        <translation>La sonde en lecture seule n&apos;a trouvé aucun fichier de configuration de cible modifiable dans cette cible. %1</translation>
    </message>
    <message>
        <source>Not present in the selected target (omitted): %1</source>
        <translation>Non présent dans la cible sélectionnée (obtenue): %1</translation>
    </message>
    <message>
        <source>Run diagnostics to probe which target configuration files exist; the helper&apos;s read-only probe decides the list.</source>
        <translation>Exécuter des diagnostics pour déterminer quels fichiers de configuration de cible existent; la sonde en lecture seule de l&apos;aide décide de la liste.</translation>
    </message>
    <message>
        <source>Probed read-only by the helper; a saved edit invalidates the cached diagnostics.</source>
        <translation>Probed read-only par l&apos;aide; une édition enregistrée invalide les diagnostics mis en cache.</translation>
    </message>
    <message>
        <source>Host maintenance: %1</source>
        <translation>Entretien de l&apos;hôte : %1</translation>
    </message>
    <message>
        <source>unresolved</source>
        <translation>non réglées</translation>
    </message>
    <message>
        <source>Target: %1 + %2</source>
        <translation>Objectif : %1 + %2</translation>
    </message>
    <message>
        <source>Host command</source>
        <translation>Commande hôte</translation>
    </message>
    <message>
        <source>Execute a command on the running host as root.</source>
        <translation>Exécuter une commande sur l&apos;hôte en cours d&apos;exécution comme root.</translation>
    </message>
    <message>
        <source>Execute a command inside the selected repair system as root.</source>
        <translation>Exécuter une commande à l&apos;intérieur du système de réparation sélectionné comme racine.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab (or re-enter Host Maintenance / re-commit the repair target) to authorize this session.</source>
        <translation>L&apos;autorisation de l&apos;administrateur est requise; appuyez sur Autoriser sur l&apos;onglet Systèmes ou Réparation (ou réentrer Maintenance de l&apos;hôte / réengager la cible de réparation) pour autoriser cette session.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root on the running host through the helper&apos;s guarded host-shell verb.</source>
        <translation>Exécutez une commande revue comme racine sur l&apos;hôte en cours d&apos;exécution à travers le verbe host-shell gardé de l&apos;aide.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root inside the target chroot through the helper&apos;s guarded shell verb.</source>
        <translation>Exécutez une commande revue comme racine dans le chroot cible à travers le verbe shell gardé de l&apos;aide.</translation>
    </message>
    <message>
        <source>Run a command on the running host as root (sudo is not needed). Commands are executed directly on the active system; output is kept in this window and in the application log.</source>
        <translation>Exécutez une commande sur l&apos;hôte en cours d&apos;exécution comme root (le sudo n&apos;est pas nécessaire). Les commandes sont exécutées directement sur le système actif; la sortie est conservée dans cette fenêtre et dans le journal des applications.</translation>
    </message>
    <message>
        <source>Run a command inside the selected repair system as root (sudo is not needed). Commands are executed one at a time in a fresh chroot and cannot answer interactive prompts; use non-interactive flags such as apt-get -y upgrade. Output is kept in this window and in the application log.</source>
        <translation>Exécutez une commande à l&apos;intérieur du système de réparation sélectionné comme root (sudo n&apos;est pas nécessaire). Les commandes sont exécutées une à la fois dans un chroot frais et ne peuvent pas répondre à des invites interactives; utilisez des drapeaux non interactifs comme apt-get -y mise à jour. La sortie est conservée dans cette fenêtre et dans le journal des applications.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize or re-enter Host Maintenance / commit a repair target.</source>
        <translation>Aucune session administrateur n&apos;est active; appuyez sur Autoriser ou réentrer Maintenance de l&apos;hôte / engager une cible de réparation.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from the repaired system</source>
        <translation>1. Sélectionnez les fichiers ou dossiers sources du système réparé</translation>
    </message>
    <message>
        <source>2. Choose destination on this host</source>
        <translation>2. Choisissez une destination sur cet hôte</translation>
    </message>
    <message>
        <source>Add File Path...</source>
        <translation>Ajouter un chemin de fichier...</translation>
    </message>
    <message>
        <source>Add Folder Path...</source>
        <translation>Ajouter le chemin du dossier...</translation>
    </message>
    <message>
        <source>Browse...</source>
        <translation>Parcourir...</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it (Host Maintenance does not provide a repair tree to browse).</source>
        <translation>Sélectionnez le disque de réparation dans Systèmes avant de choisir une destination à l&apos;intérieur (Host Maintenance ne fournit pas un arbre de réparation à parcourir).</translation>
    </message>
    <message>
        <source>Choose a host destination folder directly.</source>
        <translation>Choisissez directement un dossier de destination hôte.</translation>
    </message>
    <message>
        <source>Copy the staged files and folders with cp -a, restore ownership with chown --reference and byte-compare every regular file afterwards.</source>
        <translation>Copier les fichiers et dossiers mis en scène avec cp -a, restaurer la propriété avec chown --reference et comparer tous les fichiers réguliers par la suite.</translation>
    </message>
    <message>
        <source>File Copy unavailable</source>
        <translation>Copie de fichiers indisponible</translation>
    </message>
    <message>
        <source>Choose host destination folder</source>
        <translation>Choisissez le dossier de destination hôte</translation>
    </message>
    <message>
        <source>Select a repair target</source>
        <translation>Sélectionner une cible de réparation</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it.</source>
        <translation>Sélectionnez le disque de réparation dans Systèmes avant de choisir une destination à l&apos;intérieur.</translation>
    </message>
    <message>
        <source>File Copy - Browse Target Folders</source>
        <translation>Copie de fichier - Parcourir les dossiers cibles</translation>
    </message>
    <message>
        <source>Browse Target Folders</source>
        <translation>Parcourir les dossiers cibles</translation>
    </message>
    <message>
        <source>The helper could not list the repair-system folder:

%1</source>
        <translation>L&apos;aide n&apos;a pas pu énumérer le dossier du système de réparation :

%1</translation>
    </message>
    <message>
        <source>BROWSE_ENTRY</source>
        <translation>ENTREPRISE</translation>
    </message>
    <message>
        <source>(choose this folder: %1)</source>
        <translation>(choisir ce dossier : %1)</translation>
    </message>
    <message>
        <source>(parent folder)</source>
        <translation>(dossier parent)</translation>
    </message>
    <message>
        <source>Select repair-system destination</source>
        <translation>Sélectionner la destination du système de réparation</translation>
    </message>
    <message>
        <source>Choose a destination folder inside the repaired system (current folder: %1):</source>
        <translation>Choisissez un dossier de destination dans le système réparé (dossier actuel : %1) :</translation>
    </message>
    <message>
        <source>Folder</source>
        <translation>Dossier</translation>
    </message>
    <message>
        <source>Open</source>
        <translation>Ouvrir</translation>
    </message>
    <message>
        <source>(choose this folder:</source>
        <translation>(choisir ce dossier :</translation>
    </message>
    <message>
        <source>Add files to copy</source>
        <translation>Ajouter des fichiers à copier</translation>
    </message>
    <message>
        <source>Add folder to copy</source>
        <translation>Ajouter un dossier à copier</translation>
    </message>
    <message>
        <source>Stage at least one source and name a destination path first.</source>
        <translation>Étape au moins une source et nommer un chemin de destination d&apos;abord.</translation>
    </message>
    <message>
        <source>Copy the %1 staged item(s) to %2?

The helper keeps its direction and path containment checks; a sensitive repair-system destination is refused unless the helper approves it, and every regular file is byte-compared after the copy.</source>
        <translation>Copier le(s) élément(s) étape(s) %1 vers %2?

L&apos;aide garde sa direction et contrôle de confinement du chemin; une destination sensible du système de réparation est refusée à moins que l&apos;aide l&apos;approuve, et chaque fichier régulier est comparé par octets après la copie.</translation>
    </message>
    <message>
        <source>File Copy - Copy and Verify</source>
        <translation>Copie de fichier - Copier et vérifier</translation>
    </message>
    <message>
        <source>File Copy - Preview Changes</source>
        <translation>Copie de fichier - Prévisualiser les modifications</translation>
    </message>
    <message>
        <source>Protected running host - details</source>
        <translation>Hôte de course protégé - détails</translation>
    </message>
    <message>
        <source>not mounted (offline target)</source>
        <translation>Non monté (cible hors ligne)</translation>
    </message>
    <message>
        <source>Select a drive to see its details.</source>
        <translation>Sélectionnez un lecteur pour voir ses détails.</translation>
    </message>
    <message>
        <source>Inspecting the selected component.</source>
        <translation>Inspecter le composant sélectionné.</translation>
    </message>
    <message>
        <source>Helper-confirmed by the last read-only diagnostics.</source>
        <translation>Confirmé par le helper lors des derniers diagnostics en lecture seule.</translation>
    </message>
    <message>
        <source>Read-only inventory only; run diagnostics to confirm.</source>
        <translation>Inventaire en lecture seule uniquement ; exécutez les diagnostics pour confirmer.</translation>
    </message>
    <message>
        <source>PROTECTED - running system; read-only details only</source>
        <translation>PROTÉGÉ - système d&apos;exploitation; détails en lecture seule</translation>
    </message>
    <message>
        <source>Live / installer media - not selectable</source>
        <translation>Supports Live / installateur - non sélectionnable</translation>
    </message>
    <message>
        <source>PROTECTED - running host scope</source>
        <translation>PROTÉGÉ - portée de l&apos;hôte</translation>
    </message>
    <message>
        <source>Unlock required before selection</source>
        <translation>Déverrouillage requis avant la sélection</translation>
    </message>
    <message>
        <source>Eligible repair candidate</source>
        <translation>Candidat à la réparation admissible</translation>
    </message>
    <message>
        <source>Drive:</source>
        <translation>Lecteur :</translation>
    </message>
    <message>
        <source>Detected target:</source>
        <translation>Cible détectée :</translation>
    </message>
    <message>
        <source>Pending inspection</source>
        <translation>Inspection en attente</translation>
    </message>
    <message>
        <source>Model / label:</source>
        <translation>Modèle / étiquette :</translation>
    </message>
    <message>
        <source>Status:</source>
        <translation>État :</translation>
    </message>
    <message>
        <source>Size:</source>
        <translation>Taille :</translation>
    </message>
    <message>
        <source>Connection:</source>
        <translation>Connexion :</translation>
    </message>
    <message>
        <source>Filesystem:</source>
        <translation>Système de fichiers :</translation>
    </message>
    <message>
        <source>UUID:</source>
        <translation>UUID :</translation>
    </message>
    <message>
        <source>Mounts:</source>
        <translation>Points de montage :</translation>
    </message>
    <message>
        <source>Running system protection unresolved</source>
        <translation>Protection du système de roulement non résolue</translation>
    </message>
    <message>
        <source>No protected physical backing disk was identified</source>
        <translation>Aucun disque de support physique protégé n&apos;a été identifié</translation>
    </message>
    <message>
        <source>Current running Linux system</source>
        <translation>Système Linux actuellement en cours d&apos;exécution</translation>
    </message>
    <message>
        <source>Critical mounts: %1</source>
        <translation>Points de montage critiques : %1</translation>
    </message>
    <message>
        <source>Read-only protected-running-system facts: the helper OS fact plus the inventory model, device path, size, transport and critical mounts. Nothing here is probed destructively.</source>
        <translation>Données du système de fonctionnement protégé en lecture seule : le helper OS fact plus le modèle d&apos;inventaire, le chemin du périphérique, la taille, le transport et les montages critiques. Rien ici n&apos;est sondé de manière destructrice.</translation>
    </message>
    <message>
        <source>Select a drive to see unlock status.</source>
        <translation>Sélectionnez un lecteur pour voir l&apos;état de déverrouillage.</translation>
    </message>
    <message>
        <source>State: protected
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
The protected running host cannot be unlocked or modified; unlock is available for an offline repair target only. Use Host Maintenance for the protected running host.</source>
        <translation>État : protégé
Composante : %1
Cartouche : (aucune)
Méthode: déverrouillage de l&apos;aide (cryptsetup; passphrase via un fichier de clés mode-600, supprimé après utilisation)
L&apos;hôte d&apos;exécution protégé ne peut pas être déverrouillé ou modifié; le déverrouillage est disponible uniquement pour une cible de réparation hors ligne. Utiliser la maintenance de l&apos;hôte pour l&apos;hôte en cours d&apos;exécution protégé.</translation>
    </message>
    <message>
        <source>(none detected)</source>
        <translation>(aucune détection)</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
A locked LUKS container is visible on this drive; press Unlock to open it for this recovery session.</source>
        <translation>État: verrouillé
Composante : %1
Cartouche : (aucune)
Méthode: déverrouillage de l&apos;aide (cryptsetup; passphrase via un fichier de clés mode-600, supprimé après utilisation)
Un conteneur LUKS verrouillé est visible sur ce disque; appuyez sur Déverrouiller pour l&apos;ouvrir pour cette session de récupération.</translation>
    </message>
    <message>
        <source>(visible mapper)</source>
        <translation>(carte visible)</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: already open before this session (visible mapper; Boot Bitch will reuse it and will not close it).</source>
        <translation>État : déverrouillé
Composante : %1
Carter: %2
Méthode : déjà ouverte avant cette session (carter visible ; Boot Bitch la réutilisera et ne la fermera pas).</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper-confirmed mapping from the last read-only diagnostics.</source>
        <translation>État : déverrouillé
Composante : %1
Carter: %2
Méthode : cartographie confirmée par l&apos;aide à partir des derniers diagnostics en lecture seule.</translation>
    </message>
    <message>
        <source>(mapper)</source>
        <translation>(cuivre)</translation>
    </message>
    <message>
        <source>State: locked or no encrypted component detected
Component: (none detected)
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
No locked LUKS component and no unlock operation were recorded for this drive in the current session.</source>
        <translation>État : fermé ou aucun composant chiffré détecté
Composante : (aucune détection)
Cartouche : (aucune)
Méthode: déverrouillage de l&apos;aide (cryptsetup; passphrase via un fichier de clés mode-600, supprimé après utilisation)
Aucun composant LUKS verrouillé et aucune opération de déverrouillage n&apos;ont été enregistrés pour ce lecteur dans la session en cours.</translation>
    </message>
    <message>
        <source>unavailable - %1</source>
        <translation>non disponible - %1</translation>
    </message>
    <message>
        <source>Host maintenance:
%1</source>
        <translation>Entretien de l&apos;hôte :
%1</translation>
    </message>
    <message>
        <source>Target:
%1 + %2</source>
        <translation>Objectif :
%1 + %2</translation>
    </message>
    <message>
        <source>Authorization required: diagnostics and repairs fail closed until you press Authorize.</source>
        <translation>Autorisation requise: les diagnostics et les réparations échouent fermé jusqu&apos;à ce que vous appuyez sur Autoriser.</translation>
    </message>
    <message>
        <source>Re-establish the privileged helper session for the current scope now. The password is requested in the hidden-input modal and is never logged.</source>
        <translation>Rétablir la session d&apos;aide privilégiée pour la portée actuelle maintenant. Le mot de passe est demandé dans le mode d&apos;entrée caché et n&apos;est jamais enregistré.</translation>
    </message>
    <message>
        <source>Running...</source>
        <translation>Courir...</translation>
    </message>
    <message>
        <source>Scope required</source>
        <translation>Portée requise</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope; this unlocks the gated actions.</source>
        <translation>Exécutez All - exécutez chaque diagnostic en lecture seule disponible pour la portée actuelle; cela déverrouille les actions sécurisées.</translation>
    </message>
    <message>
        <source>Select a target and wait for any running command first.</source>
        <translation>Sélectionnez une cible et attendez d&apos;abord toute commande en cours d&apos;exécution.</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic through the helper.</source>
        <translation>Exécutez le diagnostic - exécutez le diagnostic en lecture seule sélectionné par l&apos;aide.</translation>
    </message>
    <message>
        <source>Run All for the current scope and refresh the read-only backend profile.</source>
        <translation>Exécutez All pour la portée actuelle et rafraîchissez le profil de backend en lecture seule.</translation>
    </message>
    <message>
        <source>Read or edit the selected target configuration file through the guarded helper; a saved edit invalidates cached diagnostics.</source>
        <translation>Lire ou modifier le fichier de configuration de la cible sélectionné à travers l&apos;aide gardée; une modification enregistrée invalide les diagnostics mis en cache.</translation>
    </message>
    <message>
        <source>Host Maintenance has no target-file editing; commit an offline repair target first.</source>
        <translation>Host Maintenance n&apos;a pas d&apos;édition de fichier cible; commit une cible de réparation hors ligne d&apos;abord.</translation>
    </message>
    <message>
        <source>Commit an offline repair target first.</source>
        <translation>Commettre une cible de réparation hors ligne d&apos;abord.</translation>
    </message>
    <message>
        <source>No target configuration file is available for this target; run diagnostics to probe the list.</source>
        <translation>Aucun fichier de configuration de cible n&apos;est disponible pour cette cible; exécutez des diagnostics pour sonder la liste.</translation>
    </message>
    <message>
        <source>Already Unlocked</source>
        <translation>Déjà déverrouillé</translation>
    </message>
    <message>
        <source>An unlocked Linux filesystem is already visible on this drive. Boot Bitch will reuse the existing mapper and will not close or reopen a mapping created by this recovery session. Mounting happens during diagnostics (read-only) and repairs (read-write); data filesystems are never auto-mounted on selection.</source>
        <translation>Un système de fichiers Linux déverrouillé est déjà visible sur ce lecteur. Boot Bitch réutilisera le mapper existant et ne fermera pas ou ne rouvrira pas une cartographie créée par cette session de récupération. Le montage se produit lors des diagnostics (lecture seule) et des réparations (lecture-écriture); les systèmes de fichiers de données ne sont jamais montés automatiquement sur la sélection.</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked; use Host Maintenance for the protected running host.</source>
        <translation>L&apos;hôte d&apos;exécution protégé ne peut pas être déverrouillé; utilisez la maintenance de l&apos;hôte d&apos;exécution protégé.</translation>
    </message>
    <message>
        <source>Host Maintenance is the current scope, but the selected offline drive can still be unlocked.</source>
        <translation>La maintenance de l&apos;hôte est la portée actuelle, mais le lecteur hors ligne sélectionné peut encore être déverrouillé.</translation>
    </message>
    <message>
        <source>Unlock %1 using cryptsetup through the privileged helper. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>Déverrouillez %1 en utilisant cryptsetup par l&apos;aide privilégiée. La phrase de passe traverse un fichier de clés privé et n&apos;est jamais placée dans des arguments de commande ou des journaux.</translation>
    </message>
    <message>
        <source>The protected running system cannot be selected as a repair target; use Host Maintenance for the protected running host.</source>
        <translation>Le système d&apos;exécution protégé ne peut pas être sélectionné comme cible de réparation; utilisez la maintenance d&apos;hôte pour l&apos;hôte d&apos;exécution protégé.</translation>
    </message>
    <message>
        <source>Live / installer media is read-only boot media and cannot be selected as a repair target.</source>
        <translation>Live / support d&apos;installation est un support de démarrage en lecture seule et ne peut pas être sélectionné comme cible de réparation.</translation>
    </message>
    <message>
        <source>Unlock the encrypted volume first; Select Target becomes available after a Linux filesystem is detected.</source>
        <translation>Déverrouillez d&apos;abord le volume chiffré ; Select Target devient disponible après la détection d&apos;un système de fichiers Linux.</translation>
    </message>
    <message>
        <source>Committed repair target. Repair, Diagnostics and File Copy target this physical drive until another drive is explicitly selected with Select Target.</source>
        <translation>Cible de réparation engagée. Réparation, diagnostic et copie de fichier cible ce lecteur physique jusqu&apos;à ce qu&apos;un autre lecteur soit explicitement sélectionné avec Select Target.</translation>
    </message>
    <message>
        <source>Commit %1 as the repair target; this leaves Host Maintenance and switches the scope to the selected drive.</source>
        <translation>Commit %1 en tant que cible de réparation ; cela laisse l&apos;Host Maintenance et change la portée sur le lecteur sélectionné.</translation>
    </message>
    <message>
        <source>The running host target could not be detected.</source>
        <translation>La cible de l&apos;hôte n&apos;a pas pu être détectée.</translation>
    </message>
    <message>
        <source>Leave Host Maintenance and return to repair-target mode.</source>
        <translation>Laisser l&apos;entretien de l&apos;hôte et revenir en mode cible de réparation.</translation>
    </message>
    <message>
        <source>Select the running host for deliberate guarded maintenance; the administrator authorization is requested here once and cached for the session.</source>
        <translation>Sélectionnez l&apos;hôte en cours d&apos;exécution pour une maintenance délibérément gardée; l&apos;autorisation de l&apos;administrateur est demandée ici une fois et mise en cache pour la session.</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a repair target.</source>
        <translation>La cible de l&apos;hôte en marche n&apos;a pas pu être détectée; les diagnostics ont besoin d&apos;une cible de réparation.</translation>
    </message>
    <message>
        <source>Committed target: %1 + %2</source>
        <translation>Cible engagée : %1 + %2</translation>
    </message>
    <message>
        <source>Committed target: none (selection changed)</source>
        <translation>Cible engagée : aucune (sélection modifiée)</translation>
    </message>
    <message>
        <source>The selected scope has no resolved Linux root component; Refresh Devices and commit the repair target again.</source>
        <translation>La portée sélectionnée n&apos;a pas de composant racine de Linux résolu; Rafraîchir les périphériques et recommencer la cible de réparation.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize on the Systems or Repair tab to re-establish it. Run All reuses the cached authorization and never prompts by itself.</source>
        <translation>Aucune session administrateur n&apos;est active; appuyez sur Autoriser sur l&apos;onglet Systèmes ou Réparation pour la rétablir. Exécuter All réutilise l&apos;autorisation mise en cache et n&apos;invite jamais par lui-même.</translation>
    </message>
    <message>
        <source>Run diagnostics for this scope to unlock the gated actions. Diagnostics are read-only and the only evidence source.</source>
        <translation>Lancez des diagnostics pour que cette portée déverrouille les actions sécurisées. Les diagnostics sont en lecture seule et la seule source de preuves.</translation>
    </message>
    <message>
        <source>A repair was not proven unchanged, so the cached diagnostics are invalidated. Run diagnostics again before another gated action.</source>
        <translation>Une réparation n&apos;a pas été prouvée inchangée, de sorte que les diagnostics mis en cache sont invalidés. Exécutez de nouveau le diagnostic avant une autre action fermée.</translation>
    </message>
    <message>
        <source>Gated actions reflect the cached capability lines; the helper still runs every runtime preflight when a command starts.</source>
        <translation>Les actions gated reflètent les lignes de capacité en cache; l&apos;assistant exécute toujours chaque prévol d&apos;exécution quand une commande démarre.</translation>
    </message>
    <message>
        <source>Regenerating diagnostics automatically</source>
        <translation>Régénérer automatiquement le diagnostic</translation>
    </message>
    <message>
        <source>Running diagnostic: %1</source>
        <translation>Diagnostic de fonctionnement: %1</translation>
    </message>
    <message>
        <source>Running all diagnostics</source>
        <translation>Exécution de tous les diagnostics</translation>
    </message>
    <message>
        <source>Unlocking %1</source>
        <translation>Déverrouillage %1</translation>
    </message>
    <message>
        <source>running host</source>
        <translation>l&apos;hôte</translation>
    </message>
    <message>
        <source>offline target</source>
        <translation>cible hors ligne</translation>
    </message>
    <message>
        <source>host maintenance active</source>
        <translation>entretien de l&apos;hôte actif</translation>
    </message>
    <message>
        <source>repair target committed</source>
        <translation>cible de réparation engagée</translation>
    </message>
    <message>
        <source>no committed scope</source>
        <translation>aucune portée engagée</translation>
    </message>
    <message>
        <source>Current session</source>
        <translation>Session en cours</translation>
    </message>
    <message>
        <source>Log files (*.log);;All files (*)</source>
        <translation>Fichiers journaux (*.log);Tous les fichiers (*)</translation>
    </message>
    <message>
        <source>Save log as</source>
        <translation>Enregistrer le journal sous</translation>
    </message>
    <message>
        <source>About Boot Bitch</source>
        <translation>À propos de Boot Bitch</translation>
    </message>
    <message>
        <source>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;&lt;p&gt;&lt;b&gt;Developer:&lt;/b&gt; CaptainMorgan12&lt;/p&gt;&lt;p&gt;This GUI is the package&apos;s entry point: a Qt 3.3.x frontend with the ported guarded helper for Debian Etch-era systems.&lt;/p&gt;&lt;p&gt;&lt;b&gt;Guarded repair mode:&lt;/b&gt; read-only diagnostics can inspect either the protected Running Host or an explicitly selected repair drive. The guarded Debian/APT package stages (interrupted configuration, broken dependencies, metadata refresh, upgrade), GRUB-legacy configuration regeneration, LUKS target unlock and guarded target-file editing run through the ported helper after confirmation; Host Maintenance enables the same supported stages natively on the active system after repeating the host identity and boot-mount checks.&lt;/p&gt;&lt;p&gt;The modern-only features - verified File Copy, Btrfs snapshot rollback, EFI/UKI and extlinux repair, boot-stack reconciliation and Make Default - are greyed with the helper&apos;s own probe reasons on this frontend; Arch/Alpine/Fedora package backends stay diagnostics-only here.&lt;/p&gt;&lt;p&gt;The first privileged action authorizes one cached administrator session per scope through a hidden-input modal (the Qt GUI remains unprivileged). It can be ended at any time from File - Lock Administrator Session.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;&lt;p&gt;&lt;b&gt;Développeur:&lt;/b&gt;CaptainMorgan12&lt;/p&gt;&lt;p&gt; Cette interface graphique est le point d&apos;entrée du paquet : un frontend Qt 3.3.x avec l&apos;aide portée gardée pour les systèmes Debian Etch-era.&lt;/p&gt;&lt;p&gt;&lt;b&gt;Mode de réparation garanti :&lt;/b&gt; Les diagnostics en lecture seule peuvent inspecter soit l&apos;hôte de fonctionnement protégé, soit un lecteur de réparation explicitement sélectionné. Les étapes du paquetage surveillé Debian/APT (configuration interrompue, dépendances cassées, actualisation des métadonnées, mise à jour), la régénération de configuration GRUB-legacy, le déverrouillage de la cible LUKS et l&apos;édition du fichier cible gardé passent par l&apos;aide portée après confirmation; Maintenance de l&apos;hôte permet les mêmes étapes supportées nativement sur le système actif après avoir répété l&apos;identité de l&apos;hôte et les vérifications de démarrage.&lt;/p&gt;&lt;p&gt; Les fonctionnalités modernes - vérification de la copie de fichier, snapshot back Btrfs, la réparation EFI/UKI et extlinux, la réconciliation de boot-stack et Make Default - sont grisées avec les propres raisons de la sonde de l&apos;aide sur ce frontend; Arch/Alpine/Fedora package backends restent diagnostics seulement ici. &lt;/p&gt;&lt;p&gt; La première action privilégiée autorise une session d&apos;administrateur mise en cache par champ à travers un mode d&apos;entrée caché (l&apos;interface graphique Qt reste privilégiée). Il peut être terminé à tout moment à partir de File - Lock Administrator Session.&lt;/p&gt;</translation>
    </message>
</context>
</TS>
