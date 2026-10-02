<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS language="el">
<context>
    <name>LegacyMainWindow</name>
    <message>
        <source>Validate environment</source>
        <translation>Επικύρωση περιβάλλοντος</translation>
    </message>
    <message>
        <source>Validate</source>
        <translation>Επικύρωση</translation>
    </message>
    <message>
        <source>Check the selected system&apos;s mounts, filesystem metadata, boot files, mapper consistency and dependency readiness before any repair action. This is an independent safety preflight rather than an optional Full Repair stage.</source>
        <translation>Ελέγξτε τις βάσεις του επιλεγμένου συστήματος, τα μεταδεδομένα συστήματος αρχείων, τα αρχεία εκκίνησης, τη συνέπεια χαρτογράφησης και την ετοιμότητα εξάρτησης πριν από οποιαδήποτε ενέργεια επισκευής. Πρόκειται για ανεξάρτητη προ πτήσης ασφάλεια και όχι για προαιρετικό στάδιο πλήρους επισκευής.</translation>
    </message>
    <message>
        <source>Always preflight</source>
        <translation>Πάντα προ πτήσης</translation>
    </message>
    <message>
        <source>File system repair</source>
        <translation>Επισκευή συστήματος αρχείων</translation>
    </message>
    <message>
        <source>Check File Systems</source>
        <translation>Έλεγχος συστημάτων αρχείων</translation>
    </message>
    <message>
        <source>Run the read-only file system check for the selected system&apos;s root and /boot filesystems and report each device&apos;s check tool and result without changing anything. This legacy frontend exposes the read-only check only; device repair is not wired.</source>
        <translation>Εκτέλεση του ελέγχου του συστήματος αρχείων μόνο ανάγνωσης για τα συστήματα αρχείων root και /boot του επιλεγμένου συστήματος και αναφορά του εργαλείου ελέγχου της κάθε συσκευής και αποτέλεσμα χωρίς αλλαγή τίποτα. Αυτό το ιστορικό περιβάλλον εκθέτει μόνο τον έλεγχο ανάγνωσης, η επισκευή της συσκευής δεν είναι ενσύρματη.</translation>
    </message>
    <message>
        <source>Full Repair plan: unavailable on this frontend - read-only check only</source>
        <translation>Πλήρες σχέδιο επισκευής: μη διαθέσιμο σε αυτό το περιβάλλον - μόνο για ανάγνωση</translation>
    </message>
    <message>
        <source>Complete package configuration</source>
        <translation>Πλήρης ρύθμιση πακέτου</translation>
    </message>
    <message>
        <source>Complete Configuration</source>
        <translation>Πλήρης ρύθμιση</translation>
    </message>
    <message>
        <source>Complete interrupted dpkg package configuration in the selected repair system. This is the same stage controlled by Settings -&gt; Full Repair plan -&gt; Complete interrupted package configuration, but it can also be run independently here.</source>
        <translation>Πλήρης διακοπτόμενη διαμόρφωση πακέτου dpkg στο επιλεγμένο σύστημα επισκευής. Αυτό είναι το ίδιο στάδιο που ελέγχεται από Settings -&gt; Πλήρες σχέδιο επισκευής -&gt; Πλήρης διακοπτόμενη ρύθμιση πακέτου, αλλά μπορεί επίσης να εκτελεστεί ανεξάρτητα εδώ.</translation>
    </message>
    <message>
        <source>Run the guarded dpkg-configure repair? The helper keeps its package-lock and runtime preflights.</source>
        <translation>Να τρέξουμε την επιδιόρθωση των dpkg; Ο βοηθός κρατάει το πακέτο του κλειδωμένο και το χρόνο πτήσης.</translation>
    </message>
    <message>
        <source>Repair broken dependencies</source>
        <translation>Επισκευή κατεστραμμένων εξαρτήσεων</translation>
    </message>
    <message>
        <source>Repair Dependencies</source>
        <translation>Εξαρτήσεις επισκευής</translation>
    </message>
    <message>
        <source>Repair package dependencies in the selected repair system after the mandatory safety preflight. This maps directly to Settings -&gt; Repair broken package dependencies.</source>
        <translation>Εξαρτήσεις πακέτου επισκευής στο επιλεγμένο σύστημα επισκευής μετά την υποχρεωτική προ πτήσης ασφάλειας. Αυτό χάρτες απευθείας στις Settings -&gt; Επισκευή χαλασμένων εξαρτήσεων πακέτου.</translation>
    </message>
    <message>
        <source>Run the guarded fix-broken repair? The helper keeps its simulation-first preflight and runtime guards.</source>
        <translation>Να τρέξουμε την επιδιορθωμένη επισκευή; Ο βοηθός κρατάει την πρώτη του πτήση και τους φύλακες.</translation>
    </message>
    <message>
        <source>Refresh package metadata</source>
        <translation>Ανανέωση μεταδεδομένων πακέτου</translation>
    </message>
    <message>
        <source>Refresh Metadata</source>
        <translation>Ανανέωση μεταδεδομένων</translation>
    </message>
    <message>
        <source>Refresh APT metadata in the selected repair system without upgrading installed packages. This maps directly to Settings -&gt; Refresh package metadata.</source>
        <translation>Ανανέωση μεταδεδομένων APT στο επιλεγμένο σύστημα επισκευής χωρίς αναβάθμιση εγκατεστημένων πακέτων. Αυτό χάρτες απευθείας στις Settings -&gt; Ανανέωση μεταδεδομένων πακέτου.</translation>
    </message>
    <message>
        <source>Refresh package metadata for the selected scope? The helper requires a reachable, trusted APT source.</source>
        <translation>Ανανέωση μεταδεδομένων πακέτου για το επιλεγμένο πεδίο; Ο βοηθός απαιτεί μια προσιτή, αξιόπιστη πηγή APT.</translation>
    </message>
    <message>
        <source>Upgrade installed packages</source>
        <translation>Αναβάθμιση εγκατεστημένων πακέτων</translation>
    </message>
    <message>
        <source>Simulate and Upgrade</source>
        <translation>Εξομοίωση και αναβάθμιση</translation>
    </message>
    <message>
        <source>Simulate the APT transaction first, inspect proposed removals, then apply a safe upgrade. This maps directly to Settings -&gt; Upgrade installed packages.</source>
        <translation>Προσομοίωση της συναλλαγής APT πρώτα, επιθεώρηση προτεινόμενες αφαιρέσεις, στη συνέχεια, εφαρμόστε μια ασφαλή αναβάθμιση. Αυτό χάρτες απευθείας στις Settings -&gt; Αναβάθμιση εγκατεστημένα πακέτα.</translation>
    </message>
    <message>
        <source>Run the guarded apt-upgrade transaction? The helper keeps its simulation-first and source guards.</source>
        <translation>Εκτελέστε την φυλασσόμενη συναλλαγή εύστοχα; Ο βοηθός κρατάει την εξομοίωση πρώτη και πηγαίους φρουρούς.</translation>
    </message>
    <message>
        <source>DKMS</source>
        <translation>DKMS</translation>
    </message>
    <message>
        <source>Rebuild DKMS</source>
        <translation>Ανακατασκευή DKMS</translation>
    </message>
    <message>
        <source>Rebuild out-of-tree kernel modules for kernels installed in the selected system. The helper refuses this action when DKMS is not installed; this legacy frontend exposes no DKMS action.</source>
        <translation>Επανακατασκευή εκτός δέντρου συστοιχιών πυρήνα για πυρήνες εγκατεστημένους στο επιλεγμένο σύστημα. Ο βοηθός αρνείται αυτή την ενέργεια όταν το DKMS δεν είναι εγκατεστημένο· αυτό το frontend κληρονομιάς δεν εκθέτει καμία ενέργεια DKMS.</translation>
    </message>
    <message>
        <source>Graphical login / display manager</source>
        <translation>Γραφικός διαχειριστής σύνδεσης / οθόνης</translation>
    </message>
    <message>
        <source>Restore Graphical Login</source>
        <translation>Επαναφορά γραφικής σύνδεσης</translation>
    </message>
    <message>
        <source>Restore the legacy SysV display manager configured for the running host: the /etc/X11/default-display-manager entry and the missing runlevel S-symlink, with a backup and rollback, never starting the GUI. This is a host-scope stage on this legacy frontend.</source>
        <translation>Επαναφορά του κληρονομικού διαχειριστή εμφάνισης SysV που έχει ρυθμιστεί για τον τρέχοντα υπολογιστή: η καταχώρηση /etc/X11/προεπιλεγμένη-διαχειριστής αναπαραγωγής και το χαμένο runlevel S-symlink, με αντίγραφο ασφαλείας και rollback, που δεν ξεκινά ποτέ το GUI. Αυτή είναι μια σκηνή-ξενιστής-σκόπιο σε αυτό το frontend κληρονομιά.</translation>
    </message>
    <message>
        <source>Restore the graphical login configuration for the running host? The helper backs up /etc/X11/default-display-manager and the runlevel symlink state, restores the configured entry and the missing S-symlink, rolls back on any failure, and never starts the display manager.</source>
        <translation>Επαναφορά των γραφικών ρυθμίσεων σύνδεσης για τον τρέχοντα υπολογιστή; Ο βοηθός υποστηρίζει μέχρι /etc/X11/προεπιλεγμένος-display-manager και το runlevel symlink κατάσταση, αποκαθιστά τη ρυθμισμένη είσοδο και το λείπει S-symlink, κυλά πίσω σε οποιαδήποτε αποτυχία, και ποτέ δεν ξεκινά ο διαχειριστής οθόνης.</translation>
    </message>
    <message>
        <source>Initramfs</source>
        <translation>Ίνιτραμφ</translation>
    </message>
    <message>
        <source>Rebuild Initramfs</source>
        <translation>Ανακατασκευή Initramfs</translation>
    </message>
    <message>
        <source>Rebuild initramfs images for the selected repair system only after mapper and crypttab consistency checks pass. The helper backs up each image before the apply. On Etch the stage runs through the guarded plain-chroot fallback (no unshare required).</source>
        <translation>Ξαναχτίστε τις εικόνες initramfs για το επιλεγμένο σύστημα επισκευής μόνο μετά τους ελέγχους συνέπειας χαρτογράφησης και κρυπτογράφησης. Ο βοηθός υποστηρίζει κάθε εικόνα πριν από την εφαρμογή. Στο Etch το στάδιο τρέχει μέσα από το φυλασσόμενο fallback plain-chroot (δεν απαιτείται μη συμμετοχή).</translation>
    </message>
    <message>
        <source>Rebuild the initramfs for the selected scope? The helper keeps its mapper/crypttab and backup preflights.</source>
        <translation>Ξαναχτίστε το initramfs για το επιλεγμένο πεδίο; Ο βοηθός κρατάει το χαρτονόμισμα και το αντίγραφο ασφαλείας του.</translation>
    </message>
    <message>
        <source>EFI / UKI bootloader</source>
        <translation>Φορτωτής εκκίνησης EFI / UKI</translation>
    </message>
    <message>
        <source>Repair EFI / UKI</source>
        <translation>Επισκευή EFI / UKI</translation>
    </message>
    <message>
        <source>Repair the selected system&apos;s EFI / UKI boot path. This legacy frontend exposes no EFI action; the Etch target is a BIOS/GRUB-legacy system.</source>
        <translation>Επισκευή της διαδρομής εκκίνησης EFI / UKI του επιλεγμένου συστήματος. Αυτό το frontend κληρονομιά εκθέτει καμία δράση EFI; ο στόχος Etch είναι ένα σύστημα BIOS/GRUB-legacy.</translation>
    </message>
    <message>
        <source>GRUB configuration</source>
        <translation>configuration GRUB</translation>
    </message>
    <message>
        <source>Regenerate GRUB</source>
        <translation>Αναγεννημένη GRUB</translation>
    </message>
    <message>
        <source>Regenerate the selected repair system&apos;s GRUB menu/configuration after the mandatory safety preflight. The helper backs up menu.lst, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Αναδημιουργήστε το μενού/ρυθμίσεις GRUB του επιλεγμένου συστήματος επισκευής μετά την υποχρεωτική προ πτήσης ασφάλειας. Ο βοηθός υποστηρίζει menu.lst, διατηρεί κάθε υπάρχουσα είσοδο εκκίνησης και γυρίζει πίσω σε οποιαδήποτε αποτυχία.</translation>
    </message>
    <message>
        <source>Regenerate the GRUB configuration? The helper backs up the target menu/configuration, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Αναδημιουργία της διαμόρφωσης GRUB; Ο βοηθός υποστηρίζει το μενού-στόχο, διατηρεί κάθε υπάρχουσα είσοδο εκκίνησης και επιστρέφει σε οποιαδήποτε αποτυχία.</translation>
    </message>
    <message>
        <source>extlinux configuration</source>
        <translation>configuration extlinux</translation>
    </message>
    <message>
        <source>Regenerate extlinux</source>
        <translation>Αναγεννημένη extlinux</translation>
    </message>
    <message>
        <source>Regenerate the selected system&apos;s extlinux bootloader configuration. This legacy frontend exposes no extlinux action.</source>
        <translation>Αναδημιουργία ρυθμίσεων φορτωτή εκκίνησης extlinux του επιλεγμένου συστήματος. Αυτό το frontend κληρονομιάς δεν αποκαλύπτει καμία δράση extlinux.</translation>
    </message>
    <message>
        <source>Boot stack reconciliation</source>
        <translation>Συμφιλίωση στοίβας εκκίνησης</translation>
    </message>
    <message>
        <source>Reconcile Boot Stack</source>
        <translation>Στοίβα εκκίνησης Reconcile</translation>
    </message>
    <message>
        <source>Reconcile the selected repair system&apos;s boot stack in one guarded legacy pass: mapper/crypttab validation, initramfs rebuild and GRUB-legacy configuration regeneration, with the component backups and preflights unchanged. This is the Etch equivalent of the modern boot-stack reconciliation and stays out of the Full Repair plan.</source>
        <translation>Συνδέστε τη στοίβα εκκίνησης του επιλεγμένου συστήματος επισκευής σε ένα φυλασσόμενο κληροδοτημένο πάσο: mapper/crypttab validation, initramfs ανακατασκευή και αναγέννηση διαμόρφωσης legacy GRUB, με το εξάρτημα backups και preflights αμετάβλητα. Αυτό είναι το ισοδύναμο Etch της σύγχρονης συμφιλίωσης μποτών-stack και μένει έξω από το σχέδιο πλήρους επισκευής.</translation>
    </message>
    <message>
        <source>Full Repair plan: Manual recovery tool</source>
        <translation>Πλήρες σχέδιο επισκευής: Χειροκίνητο εργαλείο αποκατάστασης</translation>
    </message>
    <message>
        <source>Run the guarded boot-stack reconciliation? The helper runs the mapper/crypttab validation, the initramfs rebuild and the GRUB-legacy regeneration in one pass with every component preflight and backup.</source>
        <translation>Να τρέξουμε την προστατευμένη συμφιλίωση; Ο βοηθός εκτελεί την επικύρωση mapper/crypttab, την ανακατασκευή initramfs και την αναγέννηση legacy GRUB σε ένα πέρασμα με κάθε συστατικό προ πτήσης και αντιγράφων ασφαλείας.</translation>
    </message>
    <message>
        <source>Complete interrupted package configuration</source>
        <translation>Ολοκληρωμένες ρυθμίσεις πακέτων</translation>
    </message>
    <message>
        <source>Repair broken package dependencies</source>
        <translation>Επισκευή σπασμένων εξαρτήσεων συσκευασίας</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Προκαθορισμένο</translation>
    </message>
    <message>
        <source>Repair file system errors (read-only check first)</source>
        <translation>Σφάλματα συστήματος αρχείων επισκευής (μόνο για ανάγνωση)</translation>
    </message>
    <message>
        <source>the legacy frontend exposes the read-only file system check only; per-device repair is not wired on this frontend (fail closed)</source>
        <translation>το ιστορικό περιβάλλον εκθέτει μόνο τον έλεγχο του συστήματος αρχείων ανάγνωσης μόνο· η επισκευή ανά συσκευή δεν είναι ενσύρματη σε αυτό το περιβάλλον (αποτυχία κλεισίματος)</translation>
    </message>
    <message>
        <source>Upgrade installed packages (adaptive APT simulation)</source>
        <translation>Αναβάθμιση εγκατεστημένων πακέτων (προσομοιωτική προσομοίωση APT)</translation>
    </message>
    <message>
        <source>Rebuild DKMS modules</source>
        <translation>Ανακατασκευή αρθρωμάτων DKMS</translation>
    </message>
    <message>
        <source>Restore graphical login manager</source>
        <translation>Επαναφορά διαχειριστή γραφικής σύνδεσης</translation>
    </message>
    <message>
        <source>Rebuild initramfs after mapper/crypttab validation</source>
        <translation>Επανακατασκευή initramfs μετά την επικύρωση χάρτη/κρυπτογράφησης</translation>
    </message>
    <message>
        <source>Repair EFI / UKI boot path</source>
        <translation>Επισκευή διαδρομής εκκίνησης EFI/UKI</translation>
    </message>
    <message>
        <source>Update GRUB configuration</source>
        <translation>Ενημέρωση ρυθμίσεων GRUB</translation>
    </message>
    <message>
        <source>Update extlinux configuration</source>
        <translation>Ενημέρωση ρυθμίσεων extlinux</translation>
    </message>
    <message>
        <source>Choose Full Repair stages in Settings. Enabled stages run in the order shown. Each configurable stage also appears below as an individual tool; the Full Repair column mirrors its current Settings state. The boot tools (EFI / UKI bootloader, GRUB or extlinux configuration, boot-stack reconciliation and Make Default) are independent: run them in any order, and a later action re-verifies what an earlier one changed and reports its own result. The active scope is shown beside Repair: selected repair drive or Running Host maintenance.</source>
        <translation>Επιλέξτε τα στάδια πλήρους επισκευής στις Settings. Ενεργοποιημένα στάδια εκτελούνται με τη σειρά που εμφανίζεται. Κάθε διαμορφωμένο στάδιο εμφανίζεται επίσης παρακάτω ως ένα μεμονωμένο εργαλείο.Η στήλη Full Repair καθρεφτίζει την τρέχουσα κατάσταση ρυθμίσεων. Τα εργαλεία εκκίνησης (EFI / UKI bootloader, GRUB ή extlinux διαμόρφωσης, boot-stack συμφιλίωσης και Make Default) είναι ανεξάρτητα: εκτελέστε τα με οποιαδήποτε σειρά, και μια μεταγενέστερη ενέργεια επαναβεβαιώνει τι άλλαξε ένα προηγούμενο και αναφέρει το δικό του αποτέλεσμα. Το ενεργό πεδίο δράσης εμφανίζεται δίπλα στην Επισκευή: επιλεγμένη μονάδα επισκευής ή λειτουργία συντήρησης Host.</translation>
    </message>
    <message>
        <source>Run All diagnostics for the selected target or running host before starting Full Repair. The report is read-only evidence used to choose and confirm repair stages.</source>
        <translation>Εκτέλεση όλων των διαγνωστικών για τον επιλεγμένο στόχο ή τρέχοντα υπολογιστή πριν από την έναρξη της πλήρους επισκευής. Η αναφορά είναι μόνο για ανάγνωση στοιχεία που χρησιμοποιούνται για την επιλογή και την επιβεβαίωση των σταδίων επισκευής.</translation>
    </message>
    <message>
        <source>Ready: required cached read-only diagnostics are available for the selected stages. Review them in Diagnostics or Logs before confirming.</source>
        <translation>Έτοιμο: απαιτούνται διαγνωστικά μόνο ανάγνωσης για τα επιλεγμένα στάδια. Επανεξέτασέ τα σε διαγνωστικά ή αρχεία καταγραφής πριν το επιβεβαιώσεις.</translation>
    </message>
    <message>
        <source>Environment validation</source>
        <translation>Επικύρωση περιβάλλοντος</translation>
    </message>
    <message>
        <source>Summarizes the selected system, protection state, mounted identity and inspection readiness.</source>
        <translation>Συνοψίζει το επιλεγμένο σύστημα, την κατάσταση προστασίας, την προσαρτημένη ταυτότητα και την ετοιμότητα επιθεώρησης.</translation>
    </message>
    <message>
        <source>Distribution and boot backend profile</source>
        <translation>Προφίλ συστήματος υποστήριξης διανομής και εκκίνησης</translation>
    </message>
    <message>
        <source>Identifies the distribution family, package manager, initramfs generator, bootloader and current guarded repair capability.</source>
        <translation>Προσδιορίζει την οικογένεια διανομής, διαχειριστή πακέτων, γεννήτρια initramfs, φορτωτή εκκίνησης και τρέχουσα φυλασσόμενη ικανότητα επισκευής.</translation>
    </message>
    <message>
        <source>Boot diagnostics</source>
        <translation>Διαγνωστικά εκκίνησης</translation>
    </message>
    <message>
        <source>Shows boot mounts and /boot contents plus storage evidence without changing the selected system.</source>
        <translation>Εμφάνιση προσάρτησης εκκίνησης και περιεχόμενο /boot συν στοιχεία αποθήκευσης χωρίς αλλαγή του επιλεγμένου συστήματος.</translation>
    </message>
    <message>
        <source>Boot evidence and selection history</source>
        <translation>Στοιχεία εκκίνησης και ιστορικό επιλογής</translation>
    </message>
    <message>
        <source>Correlates the detected boot chain, bootloader selection, kernel/initramfs and unlock evidence.</source>
        <translation>Διασταυρώνει την ανιχνευμένη αλυσίδα εκκίνησης, την επιλογή φορτωτή εκκίνησης, τον πυρήνα/initramfs και ξεκλειδώνει στοιχεία.</translation>
    </message>
    <message>
        <source>Kernel / initramfs</source>
        <translation>Πυρήνας / initramfs</translation>
    </message>
    <message>
        <source>Reviews kernel files and verifies matching initramfs images through a read-only inspection.</source>
        <translation>Κριτικές αρχείων πυρήνα και επαληθεύει το ταίριασμα των εικόνων initramfs μέσω μιας επιθεώρησης μόνο για ανάγνωση.</translation>
    </message>
    <message>
        <source>Reviews the GRUB configuration without changing boot files.</source>
        <translation>Κριτικές των ρυθμίσεων GRUB χωρίς αλλαγή αρχείων εκκίνησης.</translation>
    </message>
    <message>
        <source>EFI / UKI boot state</source>
        <translation>Κατάσταση εκκίνησης EFI / UKI</translation>
    </message>
    <message>
        <source>Inspects EFI/UKI evidence; unavailable on this legacy BIOS frontend with the helper&apos;s probe reason.</source>
        <translation>Επιθεωρεί τα στοιχεία EFI/UKI· δεν είναι διαθέσιμα σε αυτό το frontend κληρονομιάς BIOS με τον λόγο ανίχνευσης του βοηθού.</translation>
    </message>
    <message>
        <source>Reviews the configured display manager and recent boot evidence without starting the GUI.</source>
        <translation>Επανεξετάζει τον ρυθμισμένο διαχειριστή οθόνης και τα πρόσφατα στοιχεία εκκίνησης χωρίς εκκίνηση του GUI.</translation>
    </message>
    <message>
        <source>Boot errors</source>
        <translation>Σφάλματα εκκίνησης</translation>
    </message>
    <message>
        <source>Reads recent error-priority entries from the running host or selected repair system when available.</source>
        <translation>Διαβάζει πρόσφατες καταχωρήσεις προτεραιότητας σφαλμάτων από τον τρέχοντα υπολογιστή ή το επιλεγμένο σύστημα επισκευής όταν είναι διαθέσιμο.</translation>
    </message>
    <message>
        <source>Disk usage</source>
        <translation>Χρήση δίσκου</translation>
    </message>
    <message>
        <source>Summarizes filesystem capacity and free space for the running host or read-only repair target.</source>
        <translation>Συνοψίζει τη χωρητικότητα του συστήματος αρχείων και τον ελεύθερο χώρο για τον τρέχοντα στόχο επισκευής ή μόνο ανάγνωσης.</translation>
    </message>
    <message>
        <source>File systems</source>
        <translation>Συστήματα αρχείων</translation>
    </message>
    <message>
        <source>Runs the read-only file system check for the selected system&apos;s root, /boot and other filesystems.</source>
        <translation>Εκτέλεση ελέγχου του συστήματος αρχείων μόνο ανάγνωσης για τη ρίζα του επιλεγμένου συστήματος, /boot και άλλα συστήματα αρχείων.</translation>
    </message>
    <message>
        <source>/etc/fstab review</source>
        <translation>/etc/fstab αναθεώρηση</translation>
    </message>
    <message>
        <source>Displays the running host or selected repair system&apos;s fstab; repair-system inspection is mounted read-only.</source>
        <translation>Εμφανίζει τον τρέχοντα υπολογιστή ή το fstab του επιλεγμένου συστήματος επισκευής.</translation>
    </message>
    <message>
        <source>Btrfs status</source>
        <translation>Κατάσταση Btrfs</translation>
    </message>
    <message>
        <source>Shows Btrfs filesystem and subvolume information when the target uses Btrfs.</source>
        <translation>Εμφανίζει το σύστημα αρχείων Btrfs και τις πληροφορίες υποόγκο όταν ο στόχος χρησιμοποιεί Btrfs.</translation>
    </message>
    <message>
        <source>Device-mapper ancestry</source>
        <translation>Προγονική κατασκευή συσκευής</translation>
    </message>
    <message>
        <source>Shows selected mapper ancestry and device-mapper state when available.</source>
        <translation>Εμφάνιση επιλεγμένης γενεάς mapper και κατάστασης συσκευής-mapper όταν είναι διαθέσιμη.</translation>
    </message>
    <message>
        <source>LUKS / crypttab evidence</source>
        <translation>LUKS / στοιχεία κρυπτών</translation>
    </message>
    <message>
        <source>Shows LUKS/mapped ancestry plus crypttab and fstab mapper references.</source>
        <translation>Εμφανίζει αναφορές LUKS/mapped cancer συν cryptab και fstab mapper.</translation>
    </message>
    <message>
        <source>Full diagnostic report</source>
        <translation>Πλήρης διαγνωστική αναφορά</translation>
    </message>
    <message>
        <source>Combines all read-only diagnostics for the selected scope (same as Run All).</source>
        <translation>Συνδυάζει όλα τα διαγνωστικά μόνο ανάγνωσης για το επιλεγμένο πεδίο εφαρμογής (ίδιο με το Run All).</translation>
    </message>
    <message>
        <source>All entries</source>
        <translation>Όλες οι καταχωρήσεις</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>Διαγνωστικά</translation>
    </message>
    <message>
        <source>Repairs</source>
        <translation>Επισκευές</translation>
    </message>
    <message>
        <source>Package repair</source>
        <translation>Επισκευή συσκευασίας</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Αντιγραφή αρχείου</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Ανακάλυψη συσκευής</translation>
    </message>
    <message>
        <source>Host</source>
        <translation>Υπολογιστής</translation>
    </message>
    <message>
        <source>Required for block-device inventory</source>
        <translation>Απαιτούνται για την απογραφή κατά τμήματα</translation>
    </message>
    <message>
        <source>Filesystem identification</source>
        <translation>Ταυτότητα συστήματος αρχείων</translation>
    </message>
    <message>
        <source>Used to identify filesystem metadata</source>
        <translation>Χρησιμοποιείται για την αναγνώριση μεταδεδομένων συστήματος αρχείων</translation>
    </message>
    <message>
        <source>Mount inspection</source>
        <translation>Επιθεώρηση επιτόπου</translation>
    </message>
    <message>
        <source>Used to understand active mounts</source>
        <translation>Χρησιμοποιείται για την κατανόηση ενεργών προσομοιώσεων</translation>
    </message>
    <message>
        <source>LUKS support</source>
        <translation>Υποστήριξη LUKS</translation>
    </message>
    <message>
        <source>Required to unlock encrypted targets</source>
        <translation>Απαιτούνται να ξεκλειδωθούν κρυπτογραφημένοι στόχοι</translation>
    </message>
    <message>
        <source>Btrfs support</source>
        <translation>Υποστήριξη Btrfs</translation>
    </message>
    <message>
        <source>Required for Btrfs inspection and snapshot rollback</source>
        <translation>Απαιτούνται για επιθεώρηση Btrfs και στιγμιότυπο rollback</translation>
    </message>
    <message>
        <source>Bidirectional file copy</source>
        <translation>Αντίγραφο αρχείου διπλής κατεύθυνσης</translation>
    </message>
    <message>
        <source>Host/Repair</source>
        <translation>Υπολογιστής/επισκευή</translation>
    </message>
    <message>
        <source>Required for verified Host-to-Repair and Repair-to-Host transfer</source>
        <translation>Απαιτούνται για επαληθευμένη μεταφορά Host-to-Επισκευή και επισκευή-to-Host</translation>
    </message>
    <message>
        <source>Chroot repair</source>
        <translation>Επισκευή Chroot</translation>
    </message>
    <message>
        <source>Required for target-side repair commands</source>
        <translation>Απαιτούνται για τις εντολές επισκευής του στόχου</translation>
    </message>
    <message>
        <source>Offline systemd repair</source>
        <translation>Επισκευή με σύστημα offline</translation>
    </message>
    <message>
        <source>Used to restore graphical.target and the configured display manager without starting the target GUI</source>
        <translation>Χρησιμοποιείται για την αποκατάσταση γραφικών. Στόχος και ρυθμισμένος διαχειριστής οθόνης χωρίς εκκίνηση του γραφικού περιβάλλοντος προορισμού</translation>
    </message>
    <message>
        <source>UEFI NVRAM inspection</source>
        <translation>Επιθεώρηση UEFI NVRAM</translation>
    </message>
    <message>
        <source>Used to preserve target EFI BootOrder during TUXEDO UKI rebuilds when efivars are available</source>
        <translation>Χρησιμοποιείται για τη διατήρηση του στόχου EFI BootOrder κατά τη διάρκεια του TUXEDO UKI ανακατασκευάζει όταν είναι διαθέσιμα efivars</translation>
    </message>
    <message>
        <source>UKI verification</source>
        <translation>Επαλήθευση UKI</translation>
    </message>
    <message>
        <source>Used to verify the kernel embedded in a rebuilt unified kernel image</source>
        <translation>Χρησιμοποιείται για την επαλήθευση του πυρήνα που είναι ενσωματωμένος σε μια ανοικοδομημένη ενοποιημένη εικόνα πυρήνα</translation>
    </message>
    <message>
        <source>GRUB EFI repair</source>
        <translation>Επισκευή GRUB EFI</translation>
    </message>
    <message>
        <source>Target/Host</source>
        <translation>Στόχος/Host</translation>
    </message>
    <message>
        <source>Required only for conventional GRUB-based EFI systems</source>
        <translation>Απαιτούνται μόνο για συμβατικά συστήματα EFI με βάση το GRUB</translation>
    </message>
    <message>
        <source>Target</source>
        <translation>Στόχος</translation>
    </message>
    <message>
        <source>Debian-family GRUB helper</source>
        <translation>Βοηθός της οικογένειας Debian GRUB</translation>
    </message>
    <message>
        <source>Portable GRUB configuration generator used by Arch and other non-Debian systems</source>
        <translation>Φορητή γεννήτρια διαμόρφωσης GRUB που χρησιμοποιείται από Arch και άλλα μη-Debian συστήματα</translation>
    </message>
    <message>
        <source>Initramfs rebuild</source>
        <translation>Initramfs ανοικοδόμηση</translation>
    </message>
    <message>
        <source>Debian-family initramfs helper</source>
        <translation>Βοηθός της οικογένειας Debian initramfs</translation>
    </message>
    <message>
        <source>Arch-family initramfs generator</source>
        <translation>Αψίδα-οικογένεια initramfs γεννήτρια</translation>
    </message>
    <message>
        <source>Alternative initramfs generator used by Arch and other distributions</source>
        <translation>Εναλλακτική γεννήτρια initramfs που χρησιμοποιείται από Arch και άλλες διανομές</translation>
    </message>
    <message>
        <source>Initramfs verification</source>
        <translation>Επαλήθευση Initramfs</translation>
    </message>
    <message>
        <source>Read-only verification for mkinitcpio images</source>
        <translation>Επαλήθευση μόνο ανάγνωσης για εικόνες mkinitcpio</translation>
    </message>
    <message>
        <source>Read-only verification for dracut images</source>
        <translation>Επαλήθευση μόνο για εικόνες dracut</translation>
    </message>
    <message>
        <source>systemd-boot inspection</source>
        <translation>Επιθεώρηση systemd-boot</translation>
    </message>
    <message>
        <source>Host/Target</source>
        <translation>Υπολογιστής/Φορέας</translation>
    </message>
    <message>
        <source>Read-only inspection of systemd-boot and generic UKI layouts</source>
        <translation>Επιθεώρηση μόνο για ανάγνωση των διατάξεων systemd-boot και των γενικών διατάξεων UKI</translation>
    </message>
    <message>
        <source>Arch package manager</source>
        <translation>Αρχικός διαχειριστής πακέτων</translation>
    </message>
    <message>
        <source>Arch-family package database and transaction tool</source>
        <translation>Αρχικό οικογενειακό πακέτο βάσης δεδομένων και εργαλείο συναλλαγών</translation>
    </message>
    <message>
        <source>DKMS rebuild</source>
        <translation>Ανακατασκευή DKMS</translation>
    </message>
    <message>
        <source>Required only when target uses DKMS modules</source>
        <translation>Απαιτούνται μόνο όταν ο στόχος χρησιμοποιεί μονάδες DKMS</translation>
    </message>
    <message>
        <source>LVM inspection</source>
        <translation>Επιθεώρηση LVM</translation>
    </message>
    <message>
        <source>Optional LVM storage-stack support</source>
        <translation>Προαιρετική υποστήριξη αποθήκευσης LVM</translation>
    </message>
    <message>
        <source>Software RAID</source>
        <translation>RAID λογισμικού</translation>
    </message>
    <message>
        <source>Optional Linux MD RAID support</source>
        <translation>Προαιρετική υποστήριξη Linux MD RAID</translation>
    </message>
    <message>
        <source>Process namespace isolation</source>
        <translation>Απομόνωση χώρου ονομάτων διεργασίας</translation>
    </message>
    <message>
        <source>Host+Target</source>
        <translation>Υπολογιστής+ Στόχος</translation>
    </message>
    <message>
        <source>The ported helper falls back to a guarded plain chroot when unshare is absent</source>
        <translation>Ο σημαδεμένος βοηθός πέφτει πίσω σε ένα φυλασσόμενο χρώμιο όταν δεν υπάρχει μερίδιο</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Ακύρωση</translation>
    </message>
    <message>
        <source>OK</source>
        <translation>Εντάξει.</translation>
    </message>
    <message>
        <source>Yes</source>
        <translation>Ναι.</translation>
    </message>
    <message>
        <source>No</source>
        <translation>Όχι.</translation>
    </message>
    <message>
        <source>read-only helper diagnostic</source>
        <translation>διαγνωστικός βοηθός μόνο ανάγνωσης</translation>
    </message>
</context>
<context>
    <name>legacy::LegacyMainWindow</name>
    <message>
        <source>Boot Bitch Legacy (Etch / KDE 3.5 era)</source>
        <translation>Κληρονομιά Boot Bitch (Etch / KDE 3.5 εποχή)</translation>
    </message>
    <message>
        <source>Linux recovery and boot-repair utility</source>
        <translation>Εργαλείο ανάκτησης και επισκευής εκκίνησης Linux</translation>
    </message>
    <message>
        <source>GUARDED REPAIR</source>
        <translation>ΕΓΓΥΗΜΕΝΗ ΕΠΙΣΚΕΥΗ</translation>
    </message>
    <message>
        <source>Ordinary repairs require an explicitly selected non-host target. The protected running host has a separate deliberate maintenance mode with the same guarded repair stages and requires privilege authorization.</source>
        <translation>Οι συνήθεις επισκευές απαιτούν σαφώς επιλεγμένο στόχο μη-ξενιστή. Ο προστατευόμενος χειριστής λειτουργίας διαθέτει ξεχωριστή σκόπιμη λειτουργία συντήρησης με τα ίδια φυλασσόμενα στάδια επισκευής και απαιτεί άδεια προνομίου.</translation>
    </message>
    <message>
        <source>Systems</source>
        <translation>Συστήματα</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>Διαγνωστικά</translation>
    </message>
    <message>
        <source>Repair</source>
        <translation>Επισκευή</translation>
    </message>
    <message>
        <source>Chroot Shell</source>
        <translation>Χρώμιο Shell</translation>
    </message>
    <message>
        <source>File Copy</source>
        <translation>Αντιγραφή αρχείου</translation>
    </message>
    <message>
        <source>Logs</source>
        <translation>Καταγραφές</translation>
    </message>
    <message>
        <source>Settings</source>
        <translation>Settings</translation>
    </message>
    <message>
        <source>(not created yet)</source>
        <translation>(δεν έχει δημιουργηθεί ακόμα)</translation>
    </message>
    <message>
        <source>Information</source>
        <translation>Πληροφορίες</translation>
    </message>
    <message>
        <source>Close</source>
        <translation>Κλείσιμο</translation>
    </message>
    <message>
        <source>&amp;File</source>
        <translation>&amp;Αρχείο</translation>
    </message>
    <message>
        <source>&amp;Refresh Devices</source>
        <translation>&amp;Συσκευές ανανέωσης</translation>
    </message>
    <message>
        <source>&amp;Lock Administrator Session</source>
        <translation>&amp;Συνεδρία διαχειριστή κλειδαριών</translation>
    </message>
    <message>
        <source>&amp;Quit</source>
        <translation>&amp;Έξοδος</translation>
    </message>
    <message>
        <source>&amp;View</source>
        <translation>&amp;Προβολή</translation>
    </message>
    <message>
        <source>&amp;Systems</source>
        <translation>&amp;Συστήματα</translation>
    </message>
    <message>
        <source>&amp;Diagnostics</source>
        <translation>&amp;Διαγνωστικά</translation>
    </message>
    <message>
        <source>&amp;Logs</source>
        <translation>&amp;Καταγραφές</translation>
    </message>
    <message>
        <source>&amp;Settings</source>
        <translation>&amp;Settings</translation>
    </message>
    <message>
        <source>&amp;Auto-size Device Columns</source>
        <translation>&amp;Στήλες συσκευής αυτόματου μεγέθους</translation>
    </message>
    <message>
        <source>&amp;Wrap Log Lines</source>
        <translation>&amp;Γραμμές καταγραφής αναδίπλωσης</translation>
    </message>
    <message>
        <source>&amp;Help</source>
        <translation>&amp;Βοήθεια</translation>
    </message>
    <message>
        <source>&amp;Using Boot Bitch</source>
        <translation>Χρήση &amp;Boot Bitch</translation>
    </message>
    <message>
        <source>&amp;About Boot Bitch</source>
        <translation>Σχετικά με &amp;Boot Bitch</translation>
    </message>
    <message>
        <source>Device columns auto-sized. Drag headers to fine-tune widths.</source>
        <translation>Στήλες συσκευής αυτόματης μεγέθους. Σύρετε κεφαλές σε λεπτά πλάτη.</translation>
    </message>
    <message>
        <source>A helper command is running; wait for it to finish before locking the session.</source>
        <translation>Μια εντολή βοηθού τρέχει; περιμένετε να τελειώσει πριν κλειδώσετε τη συνεδρία.</translation>
    </message>
    <message>
        <source>Administrator session locked; the next privileged action will request authorization.</source>
        <translation>Η επόμενη προνομιακή ενέργεια θα ζητήσει έγκριση.</translation>
    </message>
    <message>
        <source>Using Boot Bitch</source>
        <translation>Χρήση Boot Bitch</translation>
    </message>
    <message>
        <source>Boot Bitch must run from a different booted Linux environment than the system being repaired. Use a Linux live medium or another Linux installation on a different physical drive.&lt;br&gt;&lt;br&gt;The running host is protected from ordinary repair-target selection, but it can be explicitly selected through &lt;b&gt;Host Maintenance&lt;/b&gt; for guarded native diagnostics and supported maintenance stages.&lt;br&gt;&lt;br&gt;Diagnostics follow the Systems page: the committed repair drive while Host Maintenance is off, or the protected running host while it is active.&lt;br&gt;&lt;br&gt;The first privileged action requests administrator authorization once for this Boot Bitch window; &lt;b&gt;File - Lock Administrator Session&lt;/b&gt; ends that helper session immediately. Every repair keeps the helper&apos;s own runtime preflights.</source>
        <translation>Boot Bitch πρέπει να τρέξει από ένα διαφορετικό περιβάλλον Linux εκκίνησης από το σύστημα που επισκευάζεται. Χρησιμοποιήστε ένα Linux ζωντανό μέσο ή μια άλλη εγκατάσταση Linux σε μια διαφορετική φυσική κίνηση.&lt;br&gt;&lt;br&gt; Ο τρέχοντας υπολογιστής προστατεύεται από τη συνηθισμένη επιλογή επιδιόρθωσης-στόχου, αλλά μπορεί να επιλεγεί ρητά μέσω της συντήρησης &lt;b&gt;Host &lt;/b&gt; για φυλασσόμενα τοπικά διαγνωστικά και υποστηριζόμενα στάδια συντήρησης.&lt;br&gt;&lt;br&gt; Τα διαγνωστικά ακολουθούν τη σελίδα των Συστημάτων: η αφοσιωμένη μονάδα επισκευής ενώ η συντήρηση του υπολογιστή είναι εκτός λειτουργίας, ή ο προστατευόμενος τρέχοντας υπολογιστής ενώ είναι ενεργός.&lt;br&gt;&lt;br&gt; Η πρώτη προνομιακή ενέργεια ζητά εξουσιοδότηση διαχειριστή μία φορά για αυτό το παράθυρο Boot Bitch? &lt;b&gt;Αρχείο - Κλείδωμα συνεδρία διαχειριστή&lt;/b&gt; τελειώνει αμέσως αυτή τη συνεδρία βοηθού. Κάθε επισκευή κρατάει το δικό του χρόνο πτήσης.</translation>
    </message>
    <message>
        <source>Select a physical drive; Boot Bitch resolves the most likely Linux system volume automatically. The running host stays protected from ordinary target repairs, with a separate explicit host-maintenance path for its own system. The Details button shows the protected host&apos;s facts in the details pane; selecting any drive row restores the per-drive pane.</source>
        <translation>Επιλέξτε μια φυσική κίνηση; Boot Bitch επιλύει το πιο πιθανό όγκο του συστήματος Linux αυτόματα. Ο τρέχοντας ξενιστής παραμένει προστατευμένος από τις συνήθεις επισκευές στόχου, με ξεχωριστή ρητή διαδρομή φιλοξενίας-συντήρησης για το δικό του σύστημα. Το κουμπί Λεπτομέρειες δείχνει τα δεδομένα του προστατευμένου υπολογιστή στο πλαίσιο λεπτομέρειες; επιλέγοντας οποιαδήποτε σειρά κίνησης αποκαθιστά το πάσσαλο ανά οδηγό.</translation>
    </message>
    <message>
        <source>Refresh Devices</source>
        <translation>Ανανέωση συσκευών</translation>
    </message>
    <message>
        <source>Re-read the read-only kernel inventory (/proc/partitions, /proc/mounts, /proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* and the udev metadata database). No block device is opened and nothing is written.</source>
        <translation>Ξαναδιαβάστε την απογραφή πυρήνα μόνο ανάγνωσης (/proc/partitions, /proc/mounts, /proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* και τη βάση δεδομένων μεταδεδομένων udev). Δεν ανοίγει καμία συσκευή μπλοκ και τίποτα δεν είναι γραμμένο.</translation>
    </message>
    <message>
        <source>[OK]</source>
        <translation>[Εντάξει]</translation>
    </message>
    <message>
        <source>The running system was detected and stays protected from ordinary target repairs.</source>
        <translation>Το σύστημα εκτέλεσης εντοπίστηκε και παραμένει προστατευμένο από τις συνήθεις επισκευές στόχου.</translation>
    </message>
    <message>
        <source>Detecting running system...</source>
        <translation>Ανίχνευση συστήματος...</translation>
    </message>
    <message>
        <source>Detecting protected storage...</source>
        <translation>Ανίχνευση προστατευμένης αποθήκευσης...</translation>
    </message>
    <message>
        <source>PROTECTED</source>
        <translation>ΠΡΟΣΤΑΣΙΑ</translation>
    </message>
    <message>
        <source>The running host remains protected from ordinary repair-target operations.</source>
        <translation>Ο εν ενεργεία ξενιστής παραμένει προστατευμένος από τις συνήθεις εργασίες επισκευής-στόχου.</translation>
    </message>
    <message>
        <source>Details</source>
        <translation>Λεπτομέρειες</translation>
    </message>
    <message>
        <source>Show read-only details for the protected running host.</source>
        <translation>Εμφάνιση λεπτομερειών μόνο ανάγνωσης για τον προστατευμένο τρέχοντα υπολογιστή.</translation>
    </message>
    <message>
        <source>Host Maintenance</source>
        <translation>Συντήρηση υπολογιστή</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Προκαθορισμένο</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host (menu.lst default directive with a backup and rollback). Requires Host Maintenance and the cached host-default probe.</source>
        <translation>Κάντε την κανονική εγκατεστημένη είσοδο πυρήνα η προκαθορισμένη καταχώρηση εκκίνησης GRUB-legacy στον τρέχοντα υπολογιστή (menu.lst προεπιλεγμένη οδηγία με αντίγραφο ασφαλείας και rollback). Απαιτεί Συντήρηση Οικοδεσπότου και τον κρυμμένο ανιχνευτή προεπιλογής.</translation>
    </message>
    <message>
        <source>Available repair targets</source>
        <translation>Διαθέσιμοι στόχοι επισκευής</translation>
    </message>
    <message>
        <source>Most likely first</source>
        <translation>Μάλλον πρώτα.</translation>
    </message>
    <message>
        <source>Drives are listed by the read-only inventory. Select a row to inspect it; Select Target commits the selected non-host drive with its auto-resolved Linux root component.</source>
        <translation>Οι οδηγοί παρατίθενται από την απογραφή μόνο για ανάγνωση. Επιλέξτε μια γραμμή για να την επιθεωρήσετε.Επιλέξτε το Target δεσμεύει την επιλεγμένη μη-ξενιστή μονάδα με το αυτόματο-διαλυμένο ριζικό συστατικό του Linux.</translation>
    </message>
    <message>
        <source>Device</source>
        <translation>Συσκευή</translation>
    </message>
    <message>
        <source>Size</source>
        <translation>Μέγεθος</translation>
    </message>
    <message>
        <source>Type</source>
        <translation>Τύπος</translation>
    </message>
    <message>
        <source>Filesystem</source>
        <translation>Σύστημα αρχείων</translation>
    </message>
    <message>
        <source>Select Target</source>
        <translation>Επιλογή στόχου</translation>
    </message>
    <message>
        <source>Unlock</source>
        <translation>Ξεκλείδωμα</translation>
    </message>
    <message>
        <source>Authorize</source>
        <translation>Έγκριση</translation>
    </message>
    <message>
        <source>Establish the privileged helper session for the current scope now instead of waiting for the next privileged action.</source>
        <translation>Καθιερώστε την προνομιακή συνεδρία βοηθού για την τρέχουσα εμβέλεια τώρα αντί να περιμένετε για την επόμενη προνομιακή δράση.</translation>
    </message>
    <message>
        <source>Committed target: none</source>
        <translation>Δεσμευμένος στόχος: κανένας</translation>
    </message>
    <message>
        <source>Unlock state for the selected drive; the LUKS passphrase is never logged.</source>
        <translation>Ξεκλείδωμα κατάστασης για την επιλεγμένη μονάδα κίνησης: η φράση πρόσβασης LUKS δεν καταγράφεται ποτέ.</translation>
    </message>
    <message>
        <source>Unlock status</source>
        <translation>Ξεκλείδωμα κατάστασης</translation>
    </message>
    <message>
        <source>Read-only inventory plus helper-confirmed facts; mirrors the modern Qt6 Selected drive details panel.</source>
        <translation>Απογραφή μόνο ανάγνωσης συν helper-επιβεβαιωμένα γεγονότα; καθρέφει το σύγχρονο Qt6 Επιλεγμένο πίνακα λεπτομερειών κίνησης.</translation>
    </message>
    <message>
        <source>Selected drive details</source>
        <translation>Επιλεγμένες λεπτομέρειες κίνησης</translation>
    </message>
    <message>
        <source>Field</source>
        <translation>Πεδίο</translation>
    </message>
    <message>
        <source>Value</source>
        <translation>Τιμή</translation>
    </message>
    <message>
        <source>Run All runs every available read-only diagnostic for the current scope; selecting a check runs it alone. Diagnostics are read-only and are the only evidence source for the gated repair actions.</source>
        <translation>Εκτέλεση όλων τρέχει κάθε διαθέσιμο διαγνωστικό μόνο ανάγνωσης για το τρέχον πεδίο, επιλέγοντας ένα έλεγχο τρέχει μόνο του. Τα διαγνωστικά είναι μόνο για ανάγνωση και είναι η μόνη πηγή στοιχείων για τις εγκλωβισμένες ενέργειες επισκευής.</translation>
    </message>
    <message>
        <source>Target: none selected</source>
        <translation>Στόχος: δεν επιλέχθηκε</translation>
    </message>
    <message>
        <source>Diagnostics follow the committed repair target, or the protected running host while Host Maintenance is active.</source>
        <translation>Τα διαγνωστικά ακολουθούν τον δεσμευμένο στόχο επισκευής, ή τον προστατευμένο τρέχοντα ξενιστή ενώ η συντήρηση υποδοχής είναι ενεργή.</translation>
    </message>
    <message>
        <source>Run All</source>
        <translation>Εκτέλεση όλων</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope.</source>
        <translation>Εκτέλεση όλων - εκτελέστε κάθε διαθέσιμο διαγνωστικό μόνο ανάγνωσης για το τρέχον πεδίο εφαρμογής.</translation>
    </message>
    <message>
        <source>Target configuration:</source>
        <translation>configuration στόχου:</translation>
    </message>
    <message>
        <source>Etch-era target configuration files; availability is probed read-only by the helper&apos;s diagnostics.</source>
        <translation>Τα αρχεία ρυθμίσεων στόχων Etch-era· η διαθεσιμότητα ελέγχεται μόνο από τα διαγνωστικά του βοηθού.</translation>
    </message>
    <message>
        <source>Edit Target File...</source>
        <translation>Επεξεργασία αρχείου στόχου...</translation>
    </message>
    <message>
        <source>Runs one read-only diagnostic for the selected scope through the helper (`diagnose &lt;key&gt;` / `host-diagnose &lt;key&gt;`); Run All is the combined report.</source>
        <translation>Εκτέλεση ενός διαγνωστικού μόνο ανάγνωσης για το επιλεγμένο πεδίο εφαρμογής μέσω του βοηθού (`διάγνωση &lt;key&gt;` / `host-diagnose &lt;key&gt;`; Εκτέλεση Όλα είναι η συνδυασμένη αναφορά.</translation>
    </message>
    <message>
        <source>Diagnostic checks</source>
        <translation>Διαγνωστικοί έλεγχοι</translation>
    </message>
    <message>
        <source>Check</source>
        <translation>Έλεγχος</translation>
    </message>
    <message>
        <source>Selected diagnostic</source>
        <translation>Επιλεγμένο διαγνωστικό</translation>
    </message>
    <message>
        <source>Select a diagnostic</source>
        <translation>Επιλέξτε ένα διαγνωστικό</translation>
    </message>
    <message>
        <source>Choose a diagnostic from the list.</source>
        <translation>Επιλέξτε ένα διαγνωστικό από τη λίστα.</translation>
    </message>
    <message>
        <source>Ready</source>
        <translation>Έτοιμος</translation>
    </message>
    <message>
        <source>Results</source>
        <translation>Αποτελέσματα</translation>
    </message>
    <message>
        <source>Run Diagnostic</source>
        <translation>Εκτέλεση διαγνωστικού</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic for the current scope.</source>
        <translation>Εκτέλεση διαγνωστικού - εκτελέστε το επιλεγμένο διαγνωστικό μόνο ανάγνωσης για το τρέχον πεδίο εφαρμογής.</translation>
    </message>
    <message>
        <source>Copy Results</source>
        <translation>Αντιγραφή αποτελεσμάτων</translation>
    </message>
    <message>
        <source>Save Results...</source>
        <translation>Αποθήκευση αποτελεσμάτων...</translation>
    </message>
    <message>
        <source>The Full Repair plan runs the selected legacy stages in order through the guarded helper; the individual tools run one stage at a time. Every action stays disabled until the cached capability lines say available and the helper keeps its runtime preflights.</source>
        <translation>Το σχέδιο Full Repair τρέχει τα επιλεγμένα στάδια κληρονομιάς σε τάξη μέσω του φυλασσόμενου βοηθού; τα μεμονωμένα εργαλεία τρέχει ένα στάδιο κάθε φορά. Κάθε ενέργεια παραμένει απενεργοποιημένη μέχρι οι κρυμμένες γραμμές δυνατότητας να είναι διαθέσιμες και ο βοηθός να διατηρεί το χρόνο εκτέλεσης προ πτήσης.</translation>
    </message>
    <message>
        <source>Full Repair plan</source>
        <translation>Πλήρες σχέδιο επισκευής</translation>
    </message>
    <message>
        <source>No stages selected</source>
        <translation>Δεν επιλέχθηκαν στάδια</translation>
    </message>
    <message>
        <source>Configure Plan...</source>
        <translation>Configure σχεδίου...</translation>
    </message>
    <message>
        <source>Open Settings to choose which Full Repair stages are part of the plan.</source>
        <translation>Άνοιγμα ρυθμίσεων για να επιλέξετε ποια στάδια Full Repair είναι μέρος του σχεδίου.</translation>
    </message>
    <message>
        <source>Run Full Repair</source>
        <translation>Εκτέλεση πλήρους επισκευής</translation>
    </message>
    <message>
        <source>Select a repair drive, or choose Host Maintenance on the protected running-host card.</source>
        <translation>Επιλέξτε μια μονάδα επισκευής, ή επιλέξτε Συντήρηση υπολογιστή στην προστατευμένη κάρτα run-host.</translation>
    </message>
    <message>
        <source>Stage</source>
        <translation>Στάδιο</translation>
    </message>
    <message>
        <source>Individual repair tools</source>
        <translation>Μεμονωμένα εργαλεία επισκευής</translation>
    </message>
    <message>
        <source>Tool</source>
        <translation>Εργαλείο</translation>
    </message>
    <message>
        <source>Full Repair</source>
        <translation>Πλήρης επισκευή</translation>
    </message>
    <message>
        <source>not reported</source>
        <translation>δεν έχει αναφερθεί</translation>
    </message>
    <message>
        <source>Selected tool</source>
        <translation>Επιλεγμένο εργαλείο</translation>
    </message>
    <message>
        <source>Select a repair tool</source>
        <translation>Επιλέξτε ένα εργαλείο επισκευής</translation>
    </message>
    <message>
        <source>Run Tool</source>
        <translation>Εκτέλεση εργαλείου</translation>
    </message>
    <message>
        <source>Select a tool to review its repair action.</source>
        <translation>Επιλέξτε ένα εργαλείο για την αναθεώρηση της ενέργειας επισκευής του.</translation>
    </message>
    <message>
        <source>Write actions ask for confirmation and then run the helper&apos;s own runtime preflights; the GUI never weakens them. A repair that is not proven &apos;unchanged&apos; invalidates the cached diagnostics and disables the gated actions until diagnostics run again.</source>
        <translation>Γράψτε τις ενέργειες ζητήστε επιβεβαίωση και στη συνέχεια εκτελέστε τις προ πτήσης του ίδιου του βοηθού, το GUI δεν τις αποδυναμώνει ποτέ. Μια επισκευή που δεν έχει αποδειχθεί &apos;αμετάβλητη&apos; ακυρώνει τα λανθάνοντα διαγνωστικά και απενεργοποιεί τις κλειστές ενέργειες μέχρι τα διαγνωστικά να τρέξουν ξανά.</translation>
    </message>
    <message>
        <source>Chroot shell</source>
        <translation>Κοχύλι Chroot</translation>
    </message>
    <message>
        <source>Offline commands run one at a time in a fresh chroot and cannot answer interactive prompts (apt-get -y upgrade works). Host-shell commands run directly on the running host. The helper&apos;s probe lines gate the command field; the exact reason appears in the tooltip.</source>
        <translation>Οι εντολές offline τρέχουν ένα κάθε φορά σε ένα φρέσκο chroot και δεν μπορούν να απαντήσουν σε διαδραστικές εντολές (apt-get -y αναβαθμίσεις εργασίες). Οι εντολές host-shell εκτελούνται απευθείας στον τρέχοντα υπολογιστή. Οι γραμμές του καθετήρα ανοίγουν το πεδίο εντολών. Ο ακριβής λόγος εμφανίζεται στην εργαλειοθήκη.</translation>
    </message>
    <message>
        <source>No &apos;Legacy feature shell:&apos; line is cached; run diagnostics for the selected scope to evaluate the helper&apos;s chroot/timeout containment probes (fail closed).</source>
        <translation>Δεν είναι κρυμμένη η γραμμή λειτουργίας &apos;Λεγκέι&apos;· εκτελέστε τα διαγνωστικά για το επιλεγμένο πεδίο για να αξιολογήσετε τους καθετήρες περιορισμού chroot/timeout του βοηθού (αποτυχία κλεισίματος).</translation>
    </message>
    <message>
        <source>Command</source>
        <translation>Εντολή</translation>
    </message>
    <message>
        <source>Command:</source>
        <translation>Εντολή:</translation>
    </message>
    <message>
        <source>One reviewed command string, passed to the helper as a single argument (no shell interpolation by the GUI).</source>
        <translation>Μία αναθεωρημένη συμβολοσειρά εντολών, πέρασε στον βοηθό ως ένα μόνο επιχείρημα (χωρίς παρεμβολή κελύφους από το GUI).</translation>
    </message>
    <message>
        <source>Run Command</source>
        <translation>Εκτέλεση εντολής</translation>
    </message>
    <message>
        <source>Clear Output</source>
        <translation>Καθαρισμός εξόδου</translation>
    </message>
    <message>
        <source>The helper exposes `shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for an offline target chroot and `host-shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for the running host. Both keep the helper&apos;s runtime preflights; this tab enables the command only when the scope is committed, the session is authorized and the scope&apos;s Legacy feature probe reports available.</source>
        <translation>Ο βοηθός εκθέτει `shell &lt; disk&gt; &lt;root&gt; &lt; command&gt;` για ένα offline στόχο chroot και `host-shell &lt; disk&gt; &lt;root&gt; &lt; command&gt;` για τον τρέχοντα υπολογιστή. Και οι δύο διατηρούν το χρόνο εκτέλεσης των προπτήσεων του βοηθού· αυτή η καρτέλα επιτρέπει την εντολή μόνο όταν το πεδίο εφαρμογής είναι δεσμευμένο, η συνεδρία είναι εγκεκριμένη και οι αναφορές λειτουργίας Legacy του πεδίου εφαρμογής είναι διαθέσιμες.</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Αντιγραφή αρχείου</translation>
    </message>
    <message>
        <source>Copy and verify files in either direction through the guarded helper (cp -a plus ownership restoration and a per-file byte-compare). The helper&apos;s file-copy probe gates the controls and keeps the direction and path containment checks.</source>
        <translation>Αντιγραφή και επαλήθευση αρχείων προς οποιαδήποτε κατεύθυνση μέσω του φυλασσόμενου βοηθού (cp -a συν αποκατάσταση ιδιοκτησίας και ένα ανά αρχείο byte-compare). Ο ανιχνευτής αρχείων του βοηθού ανοίγει τα χειριστήρια και ελέγχει την κατεύθυνση και την πορεία.</translation>
    </message>
    <message>
        <source>Preview Changes</source>
        <translation>Προεπισκόπηση αλλαγών</translation>
    </message>
    <message>
        <source>Run a copy dry-run through the guarded helper. No files are changed.</source>
        <translation>Τρέξε ένα αντίγραφο στο φρουρούμενο βοηθό. Δεν άλλαξαν τα αρχεία.</translation>
    </message>
    <message>
        <source>Copy and Verify</source>
        <translation>Αντιγραφή και επαλήθευση</translation>
    </message>
    <message>
        <source>Copy staged items and verify the result. Existing destination names are overwritten when source content differs; unrelated destination files are never deleted.</source>
        <translation>Αντιγραφή σταδίων και επαλήθευση του αποτελέσματος. Τα υπάρχοντα ονόματα προορισμού αντικαθίστανται όταν το περιεχόμενο πηγής διαφέρει; τα μη συνδεδεμένα αρχεία προορισμού δεν διαγράφονται ποτέ.</translation>
    </message>
    <message>
        <source>Direction:</source>
        <translation>Κατεύθυνση:</translation>
    </message>
    <message>
        <source>Choose which system supplies the source files and which system receives them.</source>
        <translation>Επιλέξτε ποιο σύστημα παρέχει τα αρχεία πηγής και ποιο σύστημα τα λαμβάνει.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from this host</source>
        <translation>1. Επιλέξτε πηγαία αρχεία ή φακέλους από αυτόν τον υπολογιστή</translation>
    </message>
    <message>
        <source>Source</source>
        <translation>Πηγή</translation>
    </message>
    <message>
        <source>Files and folders staged for the verified copy. The legacy backend copies with cp -a and restores ownership with chown --reference; every regular file is byte-compared after the copy.</source>
        <translation>Αρχεία και φάκελοι σκηνοθετημένα για το επικυρωμένο αντίγραφο. Το ιστορικό σύστημα υποστήριξης αντιγράφων με cp -a και αποκαθιστά την ιδιοκτησία με chown --αναφορά; κάθε κανονικό αρχείο byte-συγκρίνεται μετά το αντίγραφο.</translation>
    </message>
    <message>
        <source>Add Files...</source>
        <translation>Προσθήκη αρχείων...</translation>
    </message>
    <message>
        <source>Add Folder...</source>
        <translation>Προσθήκη φακέλου...</translation>
    </message>
    <message>
        <source>Remove</source>
        <translation>Αφαίρεση</translation>
    </message>
    <message>
        <source>Clear</source>
        <translation>Καθαρισμός</translation>
    </message>
    <message>
        <source>Clear the staged source list (nothing is copied or deleted).</source>
        <translation>Καθαρίστε τον στημένο κατάλογο πηγής (τίποτα δεν αντιγράφεται ή διαγράφεται).</translation>
    </message>
    <message>
        <source>2. Choose destination in repaired system</source>
        <translation>2. Επιλέξτε προορισμό σε επισκευασμένο σύστημα</translation>
    </message>
    <message>
        <source>unavailable: see the helper&apos;s Legacy feature file-copy: probe reason above</source>
        <translation>Μη διαθέσιμο: δείτε το χαρακτηριστικό Legacy του βοηθού: παραπάνω λόγος ανίχνευσης</translation>
    </message>
    <message>
        <source>An absolute path inside the selected repair system (Host to Repair) or on the running host (Repair to Host).</source>
        <translation>Μια απόλυτη διαδρομή μέσα στο επιλεγμένο σύστημα επισκευής (Host to Repair) ή στον τρέχοντα υπολογιστή (Repair to Host).</translation>
    </message>
    <message>
        <source>Browse Target Folders...</source>
        <translation>Περιήγηση φακέλων στόχων...</translation>
    </message>
    <message>
        <source>Browse the selected repair system through the helper&apos;s temporary read-only mounts and choose an absolute destination path. No target files are changed while browsing.</source>
        <translation>Περιήγηση στο επιλεγμένο σύστημα επισκευής μέσω των προσωρινών προσαρτημάτων μόνο ανάγνωσης του βοηθού και επιλογή μιας διαδρομής απόλυτου προορισμού. Δεν αλλάζουν τα αρχεία προορισμού κατά την περιήγηση.</translation>
    </message>
    <message>
        <source>3. Ownership and copy policy</source>
        <translation>3. Πολιτική ιδιοκτησίας και αντιγραφής</translation>
    </message>
    <message>
        <source>Ownership:</source>
        <translation>Ιδιοκτησία:</translation>
    </message>
    <message>
        <source>Smart destination ownership (recommended)</source>
        <translation>Έξυπνη ιδιοκτησία προορισμού (προτείνεται)</translation>
    </message>
    <message>
        <source>Preserve source numeric UID/GID</source>
        <translation>Διατήρηση αριθμού πηγής UID/GID</translation>
    </message>
    <message>
        <source>Smart mode validates UID/GID identity mapping across the two systems and falls back to the destination-directory owner when the same numeric ID means a different account (the legacy backend implements it with chown --reference).</source>
        <translation>Έξυπνη λειτουργία επικυρώνει UID/GID χαρτογράφηση ταυτότητας σε όλα τα δύο συστήματα και πέφτει πίσω στον ιδιοκτήτη προορισμού-καταλόγου όταν ο ίδιος αριθμητικός ID σημαίνει διαφορετικό λογαριασμό (το ιστορικό σύστημα υποστήριξης το υλοποιεί με chown --reference).</translation>
    </message>
    <message>
        <source>Application log</source>
        <translation>Ημερολόγιο εφαρμογής</translation>
    </message>
    <message>
        <source>The complete session register; Save As... writes every entry even while a filter hides lines. If a writable system share mount exists at /host, Save As... starts there; otherwise the log directory is the fallback. Prior session files are listed read-only.</source>
        <translation>Το πλήρες αρχείο συνεδρίας; Αποθήκευση ως... γράφει κάθε καταχώρηση ακόμη και ενώ ένα φίλτρο κρύβει γραμμές. Αν υπάρχει μια εγγράψιμη βάση κοινής χρήσης συστήματος στο /host, Αποθήκευση ως... εκκινεί εκεί· διαφορετικά ο κατάλογος καταγραφής είναι το fallback. Τα αρχεία προηγούμενης συνεδρίας αναφέρονται μόνο για ανάγνωση.</translation>
    </message>
    <message>
        <source>Session logs</source>
        <translation>Καταγραφές συνεδριών</translation>
    </message>
    <message>
        <source>Session</source>
        <translation>Συνεδρία</translation>
    </message>
    <message>
        <source>The first entry is the live session; earlier files in the log directory are listed read-only below it.</source>
        <translation>Η πρώτη καταχώρηση είναι η ζωντανή συνεδρία; νωρίτερα αρχεία στον κατάλογο καταγραφής αναφέρονται μόνο ανάγνωση κάτω από αυτό.</translation>
    </message>
    <message>
        <source>New Session Log</source>
        <translation>Νέα καταγραφή συνεδρίας</translation>
    </message>
    <message>
        <source>Close the active session file; it becomes a prior session and the next log entry starts a new file.</source>
        <translation>Κλείσιμο του ενεργού αρχείου συνεδρίας· γίνεται μια προηγούμενη συνεδρία και η επόμενη καταχώρηση καταγραφής ξεκινά ένα νέο αρχείο.</translation>
    </message>
    <message>
        <source>Add Note</source>
        <translation>Προσθήκη σημείωσης</translation>
    </message>
    <message>
        <source>Append a NOTE entry to the live session register.</source>
        <translation>Προσθήκη καταχώρησης σημείωσης στο μητρώο ζωντανής συνεδρίας.</translation>
    </message>
    <message>
        <source>Delete</source>
        <translation>Διαγραφή</translation>
    </message>
    <message>
        <source>Delete the selected prior session file (the live session is never deleted).</source>
        <translation>Διαγραφή του επιλεγμένου αρχείου προηγούμενης συνεδρίας (η ζωντανή συνεδρία δεν διαγράφεται ποτέ).</translation>
    </message>
    <message>
        <source>Refresh</source>
        <translation>Ανανέωση</translation>
    </message>
    <message>
        <source>Save As...</source>
        <translation>Αποθήκευση ως...</translation>
    </message>
    <message>
        <source>Save the complete session log (all entries, not just the current filter).</source>
        <translation>Αποθήκευση πλήρους καταγραφής συνεδρίας (όλες οι καταχωρήσεις, όχι μόνο το τρέχον φίλτρο).</translation>
    </message>
    <message>
        <source>Clear Register</source>
        <translation>Καθαρισμός μητρώου</translation>
    </message>
    <message>
        <source>Clear the live register and view; prior session files are never modified.</source>
        <translation>Καθαρισμός της ζωντανής εγγραφής και προβολής. Τα αρχεία προηγούμενης συνεδρίας δεν τροποποιούνται ποτέ.</translation>
    </message>
    <message>
        <source>Search log:</source>
        <translation>Καταγραφή αναζήτησης:</translation>
    </message>
    <message>
        <source>Type any characters to show matching log entries (case-insensitive). Save As always writes every entry.</source>
        <translation>Πληκτρολογήστε τυχόν χαρακτήρες για να εμφανίσετε αντιστοιχισμένες καταχωρήσεις καταγραφής (case-insensible). Αποθήκευση Όπως πάντα γράφει κάθε καταχώρηση.</translation>
    </message>
    <message>
        <source>Filter:</source>
        <translation>Φίλτρο:</translation>
    </message>
    <message>
        <source>Filter the visible log by entry kind. Selecting a diagnostic section shows the lines captured for that section; a workflow filter such as File system repair or Package repair shows its mapped repair lines (File copy has no lines in this frontend). Save As always writes every entry.</source>
        <translation>Φιλτράρισμα της ορατής καταγραφής κατά είδος εισόδου. Η επιλογή ενός διαγνωστικού τμήματος δείχνει τις γραμμές που συλλαμβάνονται για το τμήμα αυτό· ένα φίλτρο ροής εργασίας όπως η επισκευή συστήματος αρχείων ή η επισκευή πακέτων δείχνει τις χαρτογραφημένες γραμμές επισκευής του (το αντίγραφο του αρχείου δεν έχει γραμμές σε αυτό το frontend). Αποθήκευση Όπως πάντα γράφει κάθε καταχώρηση.</translation>
    </message>
    <message>
        <source>Settings are stored per user under ~/.qt/, one file per settings group (devicesrc, logsrc, diagnosticsrc, repairrc), and are saved immediately on every change and on close. Launch the GUI as the same user to keep your overrides; a GUI started as root keeps its own copies.</source>
        <translation>Οι ρυθμίσεις αποθηκεύονται ανά χρήστη υπό ~/.qt/, ένα αρχείο ανά ομάδα ρυθμίσεων (devicesrc, logsrc, diagnosticsrc, repairrc), και αποθηκεύονται αμέσως σε κάθε αλλαγή και στο κλείσιμο. Εκκίνηση του GUI με τον ίδιο χρήστη για να κρατήσει τις παρακάμψεις σας; ένα GUI ξεκίνησε ως ρίζα κρατά τα δικά του αντίγραφα.</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Ανακάλυψη συσκευής</translation>
    </message>
    <message>
        <source>Show devices without an identified Linux installation</source>
        <translation>Εμφάνιση συσκευών χωρίς καθορισμένη εγκατάσταση Linux</translation>
    </message>
    <message>
        <source>Show removable and USB storage</source>
        <translation>Εμφάνιση αφαιρούμενης και αποθήκευσης USB</translation>
    </message>
    <message>
        <source>Show encrypted devices before unlocking</source>
        <translation>Εμφάνιση κρυπτογραφημένων συσκευών πριν το ξεκλείδωμα</translation>
    </message>
    <message>
        <source>When off, drives without a visible Linux filesystem are hidden unless they still contain an encrypted device and encrypted devices are shown.</source>
        <translation>Όταν απενεργοποιηθούν, οι δίσκοι χωρίς ορατό σύστημα αρχείων Linux είναι κρυμμένοι εκτός αν περιέχουν ακόμα μια κρυπτογραφημένη συσκευή και εμφανίζονται κρυπτογραφημένες συσκευές.</translation>
    </message>
    <message>
        <source>When off, removable and USB drives are hidden from the repair-target list.</source>
        <translation>Όταν απενεργοποιηθεί, αφαιρούμενες και USB drives κρύβονται από τη λίστα επισκευής-στόχος.</translation>
    </message>
    <message>
        <source>When off, drives with an encrypted device are hidden until the volume is unlocked.</source>
        <translation>Όταν είναι κλειστό, οι οδηγοί με μια κρυπτογραφημένη συσκευή είναι κρυμμένοι μέχρι να ξεκλειδωθεί η ένταση.</translation>
    </message>
    <message>
        <source>Include the %1 stage in the Full Repair plan. The stage runs in the plan order shown on the Repair tab.</source>
        <translation>Συμπεριλάβετε το στάδιο %1 στο σχέδιο πλήρους επισκευής. Το στάδιο τρέχει με τη σειρά του σχεδίου που φαίνεται στην καρτέλα επισκευής.</translation>
    </message>
    <message>
        <source>Automatically regenerate read-only diagnostics after repairs or target changes</source>
        <translation>Αυτόματη αναγέννηση διαγνωστικών μόνο ανάγνωσης μετά από επισκευές ή αλλαγές στόχου</translation>
    </message>
    <message>
        <source>Regenerates the cached read-only diagnostics for the current scope after an operation that invalidates them (LUKS unlock, target configuration edit). It runs only inside an already authorized administrator session and never opens an authorization prompt by itself.</source>
        <translation>Επαναδραστηριοποιεί τα λανθάνοντα διαγνωστικά μόνο ανάγνωσης για το τρέχον πεδίο εφαρμογής μετά από μια λειτουργία που τα ακυρώνει (LUKS ξεκλειδώνει, επεξεργασία ρυθμίσεων στόχου). Τρέχει μόνο μέσα σε μια ήδη εγκεκριμένη συνεδρία διαχειριστή και ποτέ δεν ανοίγει μια εντολή εξουσιοδότησης από μόνη της.</translation>
    </message>
    <message>
        <source>Wrap long log lines</source>
        <translation>Τύλιξε μακριές γραμμές καταγραφής</translation>
    </message>
    <message>
        <source>Mandatory safety controls</source>
        <translation>Υποχρεωτικές διαδικασίες ασφάλειας</translation>
    </message>
    <message>
        <source>Host capabilities and dependencies</source>
        <translation>Ικανότητες και εξαρτήσεις υποδοχής</translation>
    </message>
    <message>
        <source>Refresh Capabilities</source>
        <translation>Ανανέωση ικανοτήτων</translation>
    </message>
    <message>
        <source>Re-run the read-only host capability probes (a PATH search, nothing is executed) and refresh the distribution summary.</source>
        <translation>Επανεκτελέστε τους καθετήρες ικανότητας host μόνο ανάγνωσης (μια αναζήτηση PATH, τίποτα δεν εκτελείται) και ανανεώστε τη σύνοψη διανομής.</translation>
    </message>
    <message>
        <source>Install Missing Support...</source>
        <translation>Εγκατάσταση ελλείπουσας υποστήριξης...</translation>
    </message>
    <message>
        <source>Automatic installation will require explicit package mapping and privilege authorization.</source>
        <translation>Η αυτόματη εγκατάσταση απαιτεί σαφή χαρτογράφηση πακέτων και εξουσιοδότηση προνομίων.</translation>
    </message>
    <message>
        <source>Application configuration</source>
        <translation>configuration εφαρμογής</translation>
    </message>
    <message>
        <source>Boot Bitch Legacy</source>
        <translation>Κληρονομιά Boot Bitch</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list first.</source>
        <translation>Επιλέξτε πρώτα μια φυσική κίνηση στη λίστα των διαθέσιμων στόχων επισκευής.</translation>
    </message>
    <message>
        <source>Protected system</source>
        <translation>Σύστημα προστασίας</translation>
    </message>
    <message>
        <source>The running system cannot be selected as a repair target. Use Host Maintenance for the protected running host or choose another disk.</source>
        <translation>Το σύστημα εκτέλεσης δεν μπορεί να επιλεγεί ως στόχος επισκευής. Χρησιμοποιήστε Συντήρηση υπολογιστή για τον προστατευμένο τρέχοντα υπολογιστή ή επιλέξτε άλλο δίσκο.</translation>
    </message>
    <message>
        <source>Unlock or select a Linux system first</source>
        <translation>Ξεκλείδωμα ή επιλογή συστήματος Linux πρώτα</translation>
    </message>
    <message>
        <source>This encrypted drive has no visible Linux filesystem yet. Use Unlock, refresh devices, and select the target after its Linux root is detected.</source>
        <translation>Αυτή η κρυπτογραφημένη κίνηση δεν έχει ορατό σύστημα αρχείων Linux ακόμα. Χρησιμοποιήστε Ξεκλειδώστε, ανανεώστε τις συσκευές και επιλέξτε το στόχο μετά την ανίχνευση της ρίζας του Linux.</translation>
    </message>
    <message>
        <source>The selected root component (%1) belongs to the running system and cannot be committed as a repair target. Use Host Maintenance for the protected running host.</source>
        <translation>Το επιλεγμένο ριζικό συστατικό (%1) ανήκει στο λειτουργικό σύστημα και δεν μπορεί να δεσμευτεί ως στόχος επισκευής. Χρησιμοποιήστε Συντήρηση υπολογιστή για τον προστατευμένο τρέχοντα ξενιστή.</translation>
    </message>
    <message>
        <source>repair target commit</source>
        <translation>δέσμευση στόχου επισκευής</translation>
    </message>
    <message>
        <source>Repair drive selected: %1</source>
        <translation>Επισκευή δίσκου επιλεγμένου: %1</translation>
    </message>
    <message>
        <source>; best detected system component: %1</source>
        <translation>· καλύτερο ανιχνευμένο συστατικό του συστήματος: %1</translation>
    </message>
    <message>
        <source>. No mount or repair action was performed.</source>
        <translation>. Δεν έγινε καμία ενέργεια τοποθέτησης ή επισκευής.</translation>
    </message>
    <message>
        <source>Make Default unavailable</source>
        <translation>Μη διαθέσιμο το προκαθορισμένο</translation>
    </message>
    <message>
        <source>Make Default is a running-host action on this frontend; enter Host Maintenance first.</source>
        <translation>Το Make Default είναι μια ενέργεια run-host σε αυτό το περιβάλλον.</translation>
    </message>
    <message>
        <source>Administrator authorization required</source>
        <translation>Απαιτείται εξουσιοδότηση διαχειριστή</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize first.</source>
        <translation>Καμία συνεδρία διαχειριστή δεν είναι ενεργή, πιέστε την εξουσιοδότηση πρώτα.</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host?

The helper verifies /boot/grub/menu.lst, sets the `default &lt;N&gt;` directive to the canonical entry, backs the menu up first and restores it on any failure.</source>
        <translation>Κάντε την κανονική εγκατεστημένη είσοδο πυρήνα στην προκαθορισμένη καταχώρηση εκκίνησης GRUB-legacy στον τρέχοντα υπολογιστή;

Ο βοηθός επαληθεύει /boot/grub/menu.lst, θέτει την οδηγία `default &lt;N&gt;` στην κανονική είσοδο, υποστηρίζει το μενού πρώτα και το αποκαθιστά σε οποιαδήποτε αποτυχία.</translation>
    </message>
    <message>
        <source>Host Maintenance unavailable</source>
        <translation>Συντήρηση υπολογιστή μη διαθέσιμη</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a committed repair target or a detected running host.</source>
        <translation>Ο τρέχοντας στόχος του ξενιστή δεν ήταν δυνατόν να ανιχνευθεί· τα διαγνωστικά χρειάζονται έναν δεσμευμένο στόχο επισκευής ή έναν ανιχνευμένο τρέχοντα ξενιστή.</translation>
    </message>
    <message>
        <source>Exit Host Maintenance</source>
        <translation>Συντήρηση υπολογιστή εξόδου</translation>
    </message>
    <message>
        <source>Host Maintenance is active; diagnostics and gated repairs target the protected running host.</source>
        <translation>Η συντήρηση του υπολογιστή είναι ενεργή· τα διαγνωστικά και οι κλειστές επισκευές στοχεύουν τον προστατευόμενο τρέχοντα ξενιστή.</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list on the Systems tab first, or use Host Maintenance for the protected running host.</source>
        <translation>Επιλέξτε μια φυσική κίνηση στη λίστα με τους διαθέσιμους στόχους επισκευής στην καρτέλα Systems πρώτα, ή χρησιμοποιήστε τη συντήρηση Host για τον προστατευμένο υπολογιστή που εκτελείται.</translation>
    </message>
    <message>
        <source>The selected drive (%1) is the protected running host. Choose Host Maintenance on the Systems tab to run read-only host diagnostics and guarded host repairs; ordinary target repairs stay disabled.</source>
        <translation>Η επιλεγμένη μονάδα (%1) είναι ο προστατευόμενος χειριστής λειτουργίας. Επιλέξτε τη συντήρηση του υπολογιστή στην καρτέλα συστημάτων για να εκτελέσετε τα διαγνωστικά μόνο ανάγνωσης και τις φυλασσόμενες επισκευές ξενιστών.</translation>
    </message>
    <message>
        <source>No repair target is committed. Choose Select Target on the Systems tab (or Host Maintenance for the protected running host) first.</source>
        <translation>Δεν υπάρχει στόχος επισκευής. Επιλέξτε πρώτα τον στόχο στην καρτέλα Systems (ή Συντήρηση υπολογιστή για τον προστατευμένο υπολογιστή που εκτελείται).</translation>
    </message>
    <message>
        <source>The selection changed after the target was committed. Choose Select Target again.</source>
        <translation>Η επιλογή άλλαξε μετά τη δέσμευση του στόχου. Επιλέξτε ξανά τον στόχο επιλογής.</translation>
    </message>
    <message>
        <source>Diagnostics scope required</source>
        <translation>Απαιτούμενο πεδίο διαγνωστικών</translation>
    </message>
    <message>
        <source>Diagnostics scope unresolved</source>
        <translation>Ανεπίλυτο πεδίο διαγνωστικών</translation>
    </message>
    <message>
        <source>The running host target could not be resolved; use Refresh Devices and commit a repair target or re-enter Host Maintenance.</source>
        <translation>Ο τρέχοντας στόχος host δεν μπορούσε να επιλυθεί.Χρησιμοποιήστε Refresh Devices και δεσμεύστε έναν στόχο επισκευής ή ξαναμπείτε στη συντήρηση host.</translation>
    </message>
    <message>
        <source>Diagnostic check required</source>
        <translation>Απαιτείται διαγνωστικός έλεγχος</translation>
    </message>
    <message>
        <source>Select a diagnostic check in the list first.</source>
        <translation>Επιλέξτε πρώτα έναν διαγνωστικό έλεγχο στη λίστα.</translation>
    </message>
    <message>
        <source>Repair target required</source>
        <translation>Απαιτούμενος στόχος επισκευής</translation>
    </message>
    <message>
        <source>Target file editing needs a committed repair target. Running-host maintenance has no target-file editing; commit an offline target first.</source>
        <translation>Η επεξεργασία αρχείων στόχου χρειάζεται έναν αφοσιωμένο στόχο επισκευής. Η συντήρηση του Running-host δεν έχει επεξεργασία αρχείου στόχου.</translation>
    </message>
    <message>
        <source>Configuration file required</source>
        <translation>Απαιτούμενο αρχείο ρύθμισης</translation>
    </message>
    <message>
        <source>Select a target configuration file first.</source>
        <translation>Επιλέξτε πρώτα ένα αρχείο ρυθμίσεων προορισμού.</translation>
    </message>
    <message>
        <source>Edit target %1</source>
        <translation>Επεξεργασία στόχου %1</translation>
    </message>
    <message>
        <source>Edit this target file through the guarded administrator helper. A successful save invalidates cached diagnostics; rerun diagnostics before repair. Generated files such as /boot/grub/menu.lst may be replaced by the next bootloader update.</source>
        <translation>Επεξεργασία αυτού του αρχείου στόχου μέσω του φυλασσόμενου βοηθού διαχειριστή. Μια επιτυχής αποθήκευση ακυρώνει cached διαγνωστικά? επανεκκίνηση διαγνωστικά πριν την επισκευή. Τα δημιουργημένα αρχεία όπως το /boot/grub/menu.lst μπορούν να αντικατασταθούν από την επόμενη ενημέρωση του bootloader.</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Ακύρωση</translation>
    </message>
    <message>
        <source>Save Target File</source>
        <translation>Αποθήκευση αρχείου προορισμού</translation>
    </message>
    <message>
        <source>No changes to %1.</source>
        <translation>Καμία αλλαγή στο %1.</translation>
    </message>
    <message>
        <source>Configuration write refused</source>
        <translation>Άρνηση εγγραφής ρυθμίσεων</translation>
    </message>
    <message>
        <source>The edited content contains NUL bytes; the guarded write refuses it.</source>
        <translation>Το επεξεργασμένο περιεχόμενο περιέχει ψηφιολέξεις NUL· το φυλασσόμενο κείμενο το αρνείται.</translation>
    </message>
    <message>
        <source>File too large</source>
        <translation>Αρχείο πολύ μεγάλο</translation>
    </message>
    <message>
        <source>The edited file is larger than 1 MiB. The guarded write refuses it; edit the file from a console instead.</source>
        <translation>Το επεξεργασμένο αρχείο είναι μεγαλύτερο από 1 MiB. Η φυλασσόμενη γραφή το αρνείται· επεξεργαστείτε το αρχείο από μια κονσόλα.</translation>
    </message>
    <message>
        <source>Write target configuration</source>
        <translation>Εγγραφή ρυθμίσεων προορισμού</translation>
    </message>
    <message>
        <source>Write the edited contents to %1? This modifies the repair target and invalidates cached diagnostics.</source>
        <translation>Γράψτε τα επεξεργασμένα περιεχόμενα σε %1; Αυτό τροποποιεί τον στόχο επισκευής και ακυρώνει τα κρυφά διαγνωστικά.</translation>
    </message>
    <message>
        <source>No diagnostic results to copy yet.</source>
        <translation>Δεν υπάρχουν διαγνωστικά αποτελέσματα για αντιγραφή ακόμα.</translation>
    </message>
    <message>
        <source>Diagnostic results copied to the clipboard.</source>
        <translation>Τα διαγνωστικά αποτελέσματα αντιγράφηκαν στο πρόχειρο.</translation>
    </message>
    <message>
        <source>No diagnostic results to save yet.</source>
        <translation>Δεν υπάρχουν διαγνωστικά αποτελέσματα για αποθήκευση ακόμα.</translation>
    </message>
    <message>
        <source>Text files (*.txt);;All files (*)</source>
        <translation>Αρχεία κειμένου (*.txt);; Όλα τα αρχεία (*)</translation>
    </message>
    <message>
        <source>Save diagnostic results</source>
        <translation>Αποθήκευση διαγνωστικών αποτελεσμάτων</translation>
    </message>
    <message>
        <source>Could not write %1.</source>
        <translation>Αδυναμία εγγραφής του %1.</translation>
    </message>
    <message>
        <source>Diagnostic results saved to %1</source>
        <translation>Διαγνωστικά αποτελέσματα αποθηκευμένα στο %1</translation>
    </message>
    <message>
        <source>Unlock not available</source>
        <translation>Το ξεκλείδωμα δεν είναι διαθέσιμο</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked. Select an offline repair target to unlock.</source>
        <translation>Ο προστατευόμενος υπολογιστής δεν μπορεί να ξεκλειδωθεί. Επιλέξτε έναν εκτός σύνδεσης στόχο επισκευής για ξεκλείδωμα.</translation>
    </message>
    <message>
        <source>No locked LUKS component is currently visible on this selected drive.</source>
        <translation>Κανένα κλειδωμένο συστατικό LUKS δεν είναι ορατό σε αυτήν την επιλεγμένη κίνηση.</translation>
    </message>
    <message>
        <source>Confirm LUKS unlock</source>
        <translation>Επιβεβαίωση ξεκλειδώματος LUKS</translation>
    </message>
    <message>
        <source>Unlock %1 on %2?

The helper opens a temporary device-mapper mapping with cryptsetup and keeps it open for this recovery session. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>Ξεκλειδώστε το %1 στο %2;

Ο βοηθός ανοίγει μια προσωρινή χαρτογράφηση συσκευής-maper με cryptsetup και το κρατά ανοιχτό για αυτή τη συνεδρία ανάκτησης. Η φράση πρόσβασης ταξιδεύει μέσω ενός ιδιωτικού αρχείου κλειδιού και δεν τοποθετείται ποτέ σε παραμέτρους εντολών ή καταγραφές.</translation>
    </message>
    <message>
        <source>LUKS unlock</source>
        <translation>Ξεκλειδώστε το LUKS</translation>
    </message>
    <message>
        <source>Unlock LUKS repair target</source>
        <translation>Ξεκλείδωμα στόχου επισκευής LUKS</translation>
    </message>
    <message>
        <source>Enter the passphrase for %1.

It is sent only to cryptsetup over the helper&apos;s standard input and is never logged or placed on a command line.</source>
        <translation>Εισάγετε τη φράση πρόσβασης για το %1.

Αποστέλλεται μόνο στην κρυπτογράφηση πάνω από την τυπική είσοδο του βοηθού και ποτέ δεν καταγράφεται ή τοποθετείται σε γραμμή εντολών.</translation>
    </message>
    <message>
        <source>Passphrase required</source>
        <translation>Απαιτείται φράση πρόσβασης</translation>
    </message>
    <message>
        <source>An empty passphrase was not submitted. Enter the LUKS passphrase or choose Cancel.</source>
        <translation>Δεν υποβλήθηκε κενή φράση πρόσβασης. Εισάγετε τη φράση πρόσβασης LUKS ή επιλέξτε Ακύρωση.</translation>
    </message>
    <message>
        <source>Unlock keyfile unavailable</source>
        <translation>Ξεκλείδωμα αρχείου κλειδιού μη διαθέσιμου</translation>
    </message>
    <message>
        <source>The LUKS passphrase could not be written to a private keyfile in %1; the unlock was not started.</source>
        <translation>Η φράση πρόσβασης LUKS δεν μπορούσε να γραφτεί σε ένα ιδιωτικό αρχείο κλειδιού στο %1. Το ξεκλείδωμα δεν ξεκίνησε.</translation>
    </message>
    <message>
        <source>Configuration write unavailable</source>
        <translation>Configuration εγγραφής μη διαθέσιμη</translation>
    </message>
    <message>
        <source>The edited content could not be written to a private temporary file in %1; the write was not started.</source>
        <translation>Το επεξεργασμένο περιεχόμενο δεν ήταν δυνατό να γραφτεί σε ιδιωτικό προσωρινό αρχείο στο %1· η εγγραφή δεν ξεκίνησε.</translation>
    </message>
    <message>
        <source>ERROR: cannot read the session log %1</source>
        <translation>ΣΦΑΛΜΑ: δεν μπορεί να διαβάσει το αρχείο καταγραφής συνεδρίας %1</translation>
    </message>
    <message>
        <source>Viewing a prior session log (read-only): %1</source>
        <translation>Προβολή καταγραφής προηγούμενης συνεδρίας (μόνο ανάγνωση): %1</translation>
    </message>
    <message>
        <source>Session log list refreshed.</source>
        <translation>Η λίστα καταγραφής συνεδρίας ανανεώθηκε.</translation>
    </message>
    <message>
        <source>Note:</source>
        <translation>Σημείωση:</translation>
    </message>
    <message>
        <source>Delete session log</source>
        <translation>Διαγραφή καταγραφής συνεδρίας</translation>
    </message>
    <message>
        <source>Delete %1 permanently? This cannot be undone.</source>
        <translation>Διαγραφή %1 μόνιμα; Αυτό δεν μπορεί να αναιρεθεί.</translation>
    </message>
    <message>
        <source>%1 changed while the confirmation was open; the delete was refused.</source>
        <translation>Το %1 άλλαξε ενώ η επιβεβαίωση ήταν ανοικτή· η διαγραφή απορρίφθηκε.</translation>
    </message>
    <message>
        <source>Unable to delete %1.</source>
        <translation>Αδυναμία διαγραφής του %1.</translation>
    </message>
    <message>
        <source>Device discovery filters updated.</source>
        <translation>Φίλτρα ανακάλυψης συσκευών ενημερωμένα.</translation>
    </message>
    <message>
        <source>Unknown Linux distribution</source>
        <translation>Άγνωστη διανομή Linux</translation>
    </message>
    <message>
        <source>sudo / gksu (no KAuth on this frontend)</source>
        <translation>sudo / gksu (δεν υπάρχει KAuth σε αυτό το frontend)</translation>
    </message>
    <message>
        <source>Available</source>
        <translation>Διαθέσιμο</translation>
    </message>
    <message>
        <source>Missing on this frontend</source>
        <translation>Λείπει σε αυτό το περιβάλλον</translation>
    </message>
    <message>
        <source>Missing</source>
        <translation>Λείπει</translation>
    </message>
    <message>
        <source>No repair tool is selected.</source>
        <translation>Δεν επιλέχθηκε εργαλείο επισκευής.</translation>
    </message>
    <message>
        <source>A helper command is already running.</source>
        <translation>Μια εντολή βοηθού τρέχει ήδη.</translation>
    </message>
    <message>
        <source>Restore Graphical Login is a host-scope stage on this legacy frontend; enter Host Maintenance to run it.</source>
        <translation>Επαναφορά γραφικών Login είναι ένα στάδιο host-scope σε αυτό το frontend κληρονομιά? εισάγετε Host Συντήρηση για να το εκτελέσετε.</translation>
    </message>
    <message>
        <source>The selected scope has no resolved root component; use Refresh Devices and commit the repair target again.</source>
        <translation>Το επιλεγμένο πεδίο εφαρμογής δεν έχει επιλυμένο συστατικό ρίζας.Χρησιμοποιήστε Refresh Devices και δεσμεύστε ξανά τον στόχο επισκευής.</translation>
    </message>
    <message>
        <source>This legacy frontend exposes no %1 action; the helper reports the capability as available.</source>
        <translation>Αυτό το frontend κληρονομιά εκθέτει καμία ενέργεια %1; ο βοηθός αναφέρει την ικανότητα ως διαθέσιμη.</translation>
    </message>
    <message>
        <source>Run this guarded repair action using the cached read-only diagnostic evidence. A confirmation is shown first.</source>
        <translation>Εκτελέστε αυτή τη φυλασσόμενη ενέργεια επισκευής χρησιμοποιώντας τα κρυμμένα διαγνωστικά στοιχεία μόνο για ανάγνωση. Μια επιβεβαίωση εμφανίζεται πρώτη.</translation>
    </message>
    <message>
        <source>Enabled in Settings</source>
        <translation>Ενεργοποίηση στις ρυθμίσεις</translation>
    </message>
    <message>
        <source>Disabled in Settings - enable it to include this stage</source>
        <translation>Απενεργοποίηση στις ρυθμίσεις - ενεργοποίηση της ενσωμάτωσης αυτού του σταδίου</translation>
    </message>
    <message>
        <source>Unavailable: %1</source>
        <translation>Μη διαθέσιμο: %1</translation>
    </message>
    <message>
        <source>
Unavailable: %1</source>
        <translation>
Μη διαθέσιμο: %1</translation>
    </message>
    <message>
        <source>Repair tool unavailable</source>
        <translation>Εργαλείο επισκευής μη διαθέσιμο</translation>
    </message>
    <message>
        <source>Confirm repair</source>
        <translation>Επιβεβαίωση επισκευής</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Your saved selection is kept and its availability is re-checked when diagnostics for this scope complete.</source>
        <translation>Δεν υπάρχουν αποδείξεις για αυτό το στάδιο ακόμα. Η αποθηκευμένη επιλογή σας διατηρείται και η διαθεσιμότητά της ελέγχεται εκ νέου όταν η διαγνωστική για αυτό το πεδίο εφαρμογής ολοκληρωθεί.</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Run diagnostics for the selected scope to populate the Full Repair plan; your selection is saved once the stage becomes available.</source>
        <translation>Δεν υπάρχουν αποδείξεις για αυτό το στάδιο ακόμα. Εκτέλεση διαγνωστικών για το επιλεγμένο πεδίο εφαρμογής για την αξιοποίηση του πλήρους σχεδίου επισκευής.Η επιλογή σας αποθηκεύεται μόλις το στάδιο γίνει διαθέσιμο.</translation>
    </message>
    <message>
        <source>Unknown Full Repair stage.</source>
        <translation>Άγνωστο στάδιο πλήρους επισκευής.</translation>
    </message>
    <message>
        <source>No Full Repair stages are selected or available; use Configure Plan... to choose the stages.</source>
        <translation>Δεν επιλέχθηκαν ή είναι διαθέσιμα πλήρη στάδια επισκευής, χρήση Configure Σχεδίου... για να επιλέξετε τα στάδια.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab to establish the session.</source>
        <translation>Απαιτείται άδεια διαχειριστή, πιέστε την άδεια στην καρτέλα Συστημάτων ή Επισκευής για την καθιέρωση της συνεδρίας.</translation>
    </message>
    <message>
        <source>Runs the selected stages using the cached read-only diagnostic evidence after privilege confirmation.</source>
        <translation>Εκτέλεση των επιλεγμένων σταδίων με χρήση των κρυφών διαγνωστικών στοιχείων μόνο ανάγνωσης μετά την επιβεβαίωση του προνομίου.</translation>
    </message>
    <message>
        <source>No Full Repair stages selected - use Configure Plan... or Settings.</source>
        <translation>Δεν επιλέχθηκαν στάδια πλήρους επισκευής - χρήση Configure Σχεδίου... ή Settings.</translation>
    </message>
    <message>
        <source>1 stage selected</source>
        <translation>Επιλέχθηκε 1 στάδιο</translation>
    </message>
    <message>
        <source>%1 stages selected</source>
        <translation>Επιλεγμένα στάδια %1</translation>
    </message>
    <message>
        <source>No repair stages are selected. Use Configure Plan... to choose the stages Full Repair will run.</source>
        <translation>Δεν επιλέχθηκαν στάδια επισκευής. Χρήση Configure Σχεδίου... για να επιλέξετε τα στάδια Πλήρης επισκευή θα τρέξει.</translation>
    </message>
    <message>
        <source>No selected stages are available. %1</source>
        <translation>Δεν υπάρχουν επιλεγμένα στάδια. %1</translation>
    </message>
    <message>
        <source>Full Repair unavailable</source>
        <translation>Πλήρης επισκευή μη διαθέσιμη</translation>
    </message>
    <message>
        <source>Run the Full Repair plan?

The selected stages run in order through the helper&apos;s guarded repair command:

</source>
        <translation>Να εκτελέσω το σχέδιο πλήρους επισκευής;

Τα επιλεγμένα στάδια τρέχουν κατά σειρά μέσω της εντολής επισκευής του βοηθού:

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
Ο βοηθός διατηρεί κάθε πτήση πριν από την πτήση, ένα στάδιο που αποτυγχάνει σταματά το σχέδιο.</translation>
    </message>
    <message>
        <source>Running %1 through the privileged helper... The Logs tab keeps the complete transcript.</source>
        <translation>Εκτέλεση %1 μέσω του προνομιούχου βοηθού... Η καρτέλα Logs κρατά το πλήρες αντίγραφο.</translation>
    </message>
    <message>
        <source>Shell scope required</source>
        <translation>Απαιτούμενο πεδίο εφαρμογής Shell</translation>
    </message>
    <message>
        <source>No administrator session is active.

Press Authorize on the Systems or Repair tab to establish the session, or enter Host Maintenance / commit a repair target on the Systems tab; the chroot shell then reuses the cached authorization.</source>
        <translation>Καμία συνεδρία διαχειριστή δεν είναι ενεργή.

Πατήστε Εξουσιοδότηση στην καρτέλα Συστημάτων ή Επισκευής για να καθιερώσετε τη συνεδρία, ή εισάγετε Συντήρηση υπολογιστή / να κάνετε έναν στόχο επισκευής στην καρτέλα Συστήματα; το κέλυφος chroot στη συνέχεια επαναχρησιμοποιεί την λανθάνουσα εξουσιοδότηση.</translation>
    </message>
    <message>
        <source>Shell command required</source>
        <translation>Απαιτείται εντολή Shell</translation>
    </message>
    <message>
        <source>Enter the command to run first.</source>
        <translation>Εισάγετε την εντολή για να εκτελέσετε πρώτοι.</translation>
    </message>
    <message>
        <source>Confirm running-host command</source>
        <translation>Επιβεβαίωση εντολής εκτέλεσης- host</translation>
    </message>
    <message>
        <source>Run this command as root on the protected running host?

%1

The helper keeps its runtime preflights; the command is passed as one argument and is never interpreted by the GUI.</source>
        <translation>Εκτέλεση αυτής της εντολής ως ρίζα στον προστατευμένο υπολογιστή;

%1

Ο βοηθός διατηρεί το χρόνο runtime του προ πτήσης· η εντολή περνά ως ένα επιχείρημα και δεν ερμηνεύεται ποτέ από το GUI.</translation>
    </message>
    <message>
        <source>Running %1...</source>
        <translation>Εκτέλεση %1...</translation>
    </message>
    <message>
        <source>Authorization scope required</source>
        <translation>Απαιτούμενο πεδίο εφαρμογής αδειοδότησης</translation>
    </message>
    <message>
        <source>administrator authorization</source>
        <translation>Άδεια διαχειριστή</translation>
    </message>
    <message>
        <source>Administrator authorization</source>
        <translation>Άδεια διαχειριστή</translation>
    </message>
    <message>
        <source>Administrator authorization is required for %1.

Enter the password for %2 (sudo). It is used only for this sudo authentication, is sent over a pipe and is never logged or placed on a command line. The authorization is cached for this session and reused by diagnostics and repairs.</source>
        <translation>Άδεια διαχειριστή απαιτείται για %1.

Εισάγετε τον κωδικό πρόσβασης για %2 (sudo). Χρησιμοποιείται μόνο για αυτή την ταυτοποίηση sudo, αποστέλλεται πάνω από έναν σωλήνα και ποτέ δεν καταγράφεται ή τοποθετείται σε μια γραμμή εντολών. Η εξουσιοδότηση είναι κρυμμένη για αυτή τη συνεδρία και επαναχρησιμοποιείται από διαγνωστικά και επισκευές.</translation>
    </message>
    <message>
        <source>your account</source>
        <translation>ο λογαριασμός σας</translation>
    </message>
    <message>
        <source>Password required</source>
        <translation>Απαιτείται κωδικός πρόσβασης</translation>
    </message>
    <message>
        <source>An empty password was not submitted. Enter the sudo password or choose Cancel.</source>
        <translation>Δεν υποβλήθηκε κενός κωδικός πρόσβασης. Εισάγετε τον κωδικό sudo ή επιλέξτε Ακύρωση.</translation>
    </message>
    <message>
        <source>Administrator authorization failed</source>
        <translation>Αποτυχία εξουσιοδότησης διαχειριστή</translation>
    </message>
    <message>
        <source>sudo did not accept the password: %1

The command was not started.</source>
        <translation>ο sudo δεν αποδέχτηκε τον κωδικό πρόσβασης: %1

Η εντολή δεν ξεκίνησε.</translation>
    </message>
    <message>
        <source>elevation requires an interactive sudo password; run the smoke as root or after `sudo -S -v` with --elevate &apos;sudo -n&apos;</source>
        <translation>Η ανύψωση απαιτεί ένα διαδραστικό κωδικό πρόσβασης sudo; εκτελέστε τον καπνό ως ρίζα ή μετά `sudo -S -v` με -- αφαιρέστε &apos;sudo -n&apos;</translation>
    </message>
    <message>
        <source>No administrator session is active for %1.

Press Authorize on the Systems or Repair tab to establish the session now, or enter Host Maintenance / commit a repair target on the Systems tab; diagnostics and repairs then reuse the cached authorization.</source>
        <translation>Καμία συνεδρία διαχειριστή δεν είναι ενεργή για %1.

Πατήστε Εξουσιοδότηση στην καρτέλα Συστημάτων ή Επισκευής για να καθιερώσετε τη συνεδρία τώρα, ή εισάγετε Συντήρηση Οικοδεσπιστή / δεσμέψτε έναν στόχο επισκευής στην καρτέλα Συστημάτων; Διαγνωστικά και επισκευές στη συνέχεια επαναχρησιμοποιήστε την κρυφή εξουσιοδότηση.</translation>
    </message>
    <message>
        <source>Administrator authorization expired</source>
        <translation>Η άδεια διαχειριστή έληξε.</translation>
    </message>
    <message>
        <source>The cached administrator authorization for %1 expired or was refused.

Press Authorize on the Systems or Repair tab to re-establish the session, then run the command again. No command was started.</source>
        <translation>Η λανθάνουσα εξουσιοδότηση διαχειριστή για %1 έληξε ή απορρίφθηκε.

Πατήστε Εξουσιοδότηση στην καρτέλα Συστημάτων ή Επισκευής για την επαναφορά της συνεδρίας, έπειτα εκτελέστε ξανά την εντολή. Δεν ξεκίνησε καμία εντολή.</translation>
    </message>
    <message>
        <source>Privileged operation completed successfully. Administrator authorization remains active for this session.</source>
        <translation>Η προνομιακή λειτουργία ολοκληρώθηκε με επιτυχία. Η εξουσιοδότηση διαχειριστή παραμένει ενεργή για αυτή τη συνεδρία.</translation>
    </message>
    <message>
        <source>Privileged operation stopped with an error. Administrator authorization remains active; review the output before closing.</source>
        <translation>Προνομιακή λειτουργία σταμάτησε με ένα λάθος. Η εξουσιοδότηση διαχειριστή παραμένει ενεργή.</translation>
    </message>
    <message>
        <source>Inspection is read-only.

</source>
        <translation>Η επιθεώρηση είναι μόνο για ανάγνωση.

</translation>
    </message>
    <message>
        <source>Configuration unavailable</source>
        <translation>unavailable μη διαθέσιμη</translation>
    </message>
    <message>
        <source>The helper could not read %1:

%2</source>
        <translation>Ο βοηθός δεν μπορούσε να διαβάσει %1:

%2</translation>
    </message>
    <message>
        <source>Configuration write failed</source>
        <translation>Αποτυχία εγγραφής ρυθμίσεων</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
Result: mapping opened for this recovery session.</source>
        <translation>Κατάσταση: ξεκλείδωτη
Συστατικό: %1
Χαρτογράφηση: %2
Μέθοδος: ξεκλείδωμα βοηθού (κρυπτογράφηση· φράση πρόσβασης μέσω ενός αρχείου πλήκτρων mode-600, διαγραμμένο μετά τη χρήση)
Αποτέλεσμα: η χαρτογράφηση άνοιξε για αυτή τη συνεδρία ανάκτησης.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: the passphrase was not accepted; retry offered.</source>
        <translation>Κατάσταση: κλειδωμένη
Συστατικό: %1
Μέθοδος: ο βοηθός ξεκλειδώνει (κρυπτογράφηση)
Σφάλμα: η φράση πρόσβασης δεν έγινε αποδεκτή.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: %2</source>
        <translation>Κατάσταση: κλειδωμένη
Συστατικό: %1
Μέθοδος: ο βοηθός ξεκλειδώνει (κρυπτογράφηση)
Σφάλμα: %2</translation>
    </message>
    <message>
        <source>Passphrase not accepted</source>
        <translation>Η φράση πρόσβασης δεν έγινε αποδεκτή</translation>
    </message>
    <message>
        <source>The LUKS passphrase was not accepted.

Try again?</source>
        <translation>Η φράση πρόσβασης LUKS δεν έγινε αποδεκτή.

Προσπάθησε ξανά;</translation>
    </message>
    <message>
        <source>[-] %1 - not run (the plan stopped before reaching this stage)</source>
        <translation>[-] %1 - δεν τρέχει (το σχέδιο σταμάτησε πριν φτάσει σε αυτό το στάδιο)</translation>
    </message>
    <message>
        <source>filesystem</source>
        <translation>σύστημα αρχείων</translation>
    </message>
    <message>
        <source>[-] %1 - no file system errors found - no changes</source>
        <translation>[-] %1 - δεν βρέθηκαν σφάλματα συστήματος αρχείων - δεν βρέθηκαν αλλαγές</translation>
    </message>
    <message>
        <source>[FAIL] %1 - not reported</source>
        <translation>[FAIL] %1 - δεν έχει αναφερθεί</translation>
    </message>
    <message>
        <source>%1 stage(s) failed; review the helper output in Logs.</source>
        <translation>Το(α) στάδιο(τα) %1 απέτυχε.</translation>
    </message>
    <message>
        <source>the plan stopped before any stage completed.</source>
        <translation>το σχέδιο σταμάτησε πριν ολοκληρωθεί οποιοδήποτε στάδιο.</translation>
    </message>
    <message>
        <source>no repair was needed; cached diagnostics remain valid.</source>
        <translation>δεν χρειάστηκε επισκευή· τα κρυφά διαγνωστικά παραμένουν έγκυρα.</translation>
    </message>
    <message>
        <source>repair completed; cached diagnostics were invalidated and must be regenerated.</source>
        <translation>η επισκευή ολοκληρώθηκε· τα κρυφά διαγνωστικά ακυρώθηκαν και πρέπει να αναγεννηθούν.</translation>
    </message>
    <message>
        <source>%1 results: [OK] %2 successful | [FAIL] %3 failed | [-] %4 no repair needed - %5</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Re-run Diagnostic</source>
        <translation>Επαναφορά διαγνωστικού</translation>
    </message>
    <message>
        <source>Unavailable</source>
        <translation>Μη διαθέσιμο</translation>
    </message>
    <message>
        <source>Commit this physical drive as the repair target.</source>
        <translation>Δέσε αυτή τη φυσική κίνηση ως στόχο επισκευής.</translation>
    </message>
    <message>
        <source>Protection:</source>
        <translation>Προστασία:</translation>
    </message>
    <message>
        <source>Host shell</source>
        <translation>κέλυφος υπολογιστή</translation>
    </message>
    <message>
        <source>Run on Host</source>
        <translation>Εκτέλεση σε υπολογιστή</translation>
    </message>
    <message>
        <source>Host Shell</source>
        <translation>Υπολογιστής Shell</translation>
    </message>
    <message>
        <source>The read-only probe found no editable target configuration file in this target. %1</source>
        <translation>Ο ανιχνευτής μόνο ανάγνωσης δεν βρήκε επεξεργάσιμο αρχείο ρυθμίσεων στόχου σε αυτόν τον στόχο. %1</translation>
    </message>
    <message>
        <source>Not present in the selected target (omitted): %1</source>
        <translation>Δεν υπάρχει στον επιλεγμένο στόχο (υποβλήθηκε): %1</translation>
    </message>
    <message>
        <source>Run diagnostics to probe which target configuration files exist; the helper&apos;s read-only probe decides the list.</source>
        <translation>Εκτελέστε τα διαγνωστικά για να διερευνήσετε ποια αρχεία ρυθμίσεων του στόχου υπάρχουν.</translation>
    </message>
    <message>
        <source>Probed read-only by the helper; a saved edit invalidates the cached diagnostics.</source>
        <translation>Προσαρμόστηκε μόνο ανάγνωση από τον βοηθό; μια αποθηκευμένη επεξεργασία ακυρώνει τα λανθάνοντα διαγνωστικά.</translation>
    </message>
    <message>
        <source>Host maintenance: %1</source>
        <translation>Συντήρηση υπολογιστή: %1</translation>
    </message>
    <message>
        <source>unresolved</source>
        <translation>ανεπίλυτο</translation>
    </message>
    <message>
        <source>Target: %1 + %2</source>
        <translation>Στόχος: %1 + %2</translation>
    </message>
    <message>
        <source>Host command</source>
        <translation>Εντολή υπολογιστή</translation>
    </message>
    <message>
        <source>Execute a command on the running host as root.</source>
        <translation>Εκτελέστε μια εντολή στον τρέχοντα υπολογιστή ως ρίζα.</translation>
    </message>
    <message>
        <source>Execute a command inside the selected repair system as root.</source>
        <translation>Εκτέλεση μιας εντολής μέσα στο επιλεγμένο σύστημα επισκευής ως ρίζα.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab (or re-enter Host Maintenance / re-commit the repair target) to authorize this session.</source>
        <translation>Απαιτείται άδεια διαχειριστή.Πιέστε το Eγγραφή στην καρτέλα Συστημάτων ή Επισκευής (ή επανεισαγάγετε Συντήρηση υπολογιστή / re-commit τον στόχο επισκευής) για να εξουσιοδοτήσετε αυτή τη συνεδρία.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root on the running host through the helper&apos;s guarded host-shell verb.</source>
        <translation>Εκτέλεση μιας αναθεωρημένης εντολής ως ρίζα στον τρέχοντα υπολογιστή μέσω του φυλασσόμενου verbήματος host-shell του βοηθού.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root inside the target chroot through the helper&apos;s guarded shell verb.</source>
        <translation>Εκτελέστε μια αναθεωρημένη εντολή ως ρίζα μέσα στο στόχο chroot μέσα από το φυλασσόμενο verbήμα κέλυφος του βοηθού.</translation>
    </message>
    <message>
        <source>Run a command on the running host as root (sudo is not needed). Commands are executed directly on the active system; output is kept in this window and in the application log.</source>
        <translation>Εκτελέστε μια εντολή στον τρέχοντα υπολογιστή ως ρίζα (δεν χρειάζεται sudo). Οι εντολές εκτελούνται απευθείας στο ενεργό σύστημα· η έξοδος διατηρείται σε αυτό το παράθυρο και στο αρχείο καταγραφής εφαρμογών.</translation>
    </message>
    <message>
        <source>Run a command inside the selected repair system as root (sudo is not needed). Commands are executed one at a time in a fresh chroot and cannot answer interactive prompts; use non-interactive flags such as apt-get -y upgrade. Output is kept in this window and in the application log.</source>
        <translation>Εκτέλεση μιας εντολής στο εσωτερικό του επιλεγμένου συστήματος επισκευής ως ρίζα (δεν απαιτείται sudo). Οι εντολές εκτελούνται ένα-ένα σε ένα φρέσκο chroot και δεν μπορούν να απαντήσουν σε διαδραστικές υποδείξεις?Χρησιμοποιήστε μη-διαδραστικές σημαίες, όπως apt-get -y αναβάθμιση. Η έξοδος διατηρείται σε αυτό το παράθυρο και στο αρχείο καταγραφής εφαρμογών.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize or re-enter Host Maintenance / commit a repair target.</source>
        <translation>Καμία συνεδρία διαχειριστή δεν είναι ενεργή; πατήστε Εξουσιοδότηση ή επανεισαγάγετε Συντήρηση υπολογιστή / δεσμευθείτε έναν στόχο επισκευής.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from the repaired system</source>
        <translation>1. Επιλέξτε πηγαία αρχεία ή φακέλους από το επισκευασμένο σύστημα</translation>
    </message>
    <message>
        <source>2. Choose destination on this host</source>
        <translation>2. Επιλέξτε προορισμό σε αυτόν τον υπολογιστή</translation>
    </message>
    <message>
        <source>Add File Path...</source>
        <translation>Προσθήκη διαδρομής αρχείου...</translation>
    </message>
    <message>
        <source>Add Folder Path...</source>
        <translation>Προσθήκη διαδρομής φακέλου...</translation>
    </message>
    <message>
        <source>Browse...</source>
        <translation>Περιήγηση...</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it (Host Maintenance does not provide a repair tree to browse).</source>
        <translation>Επιλέξτε την μονάδα επισκευής σε Συστήματα πριν επιλέξετε έναν προορισμό μέσα σε αυτό (Host Συντήρηση δεν παρέχει ένα δέντρο επισκευής για να περιηγηθείτε).</translation>
    </message>
    <message>
        <source>Choose a host destination folder directly.</source>
        <translation>Επιλέξτε έναν φάκελο προορισμού host άμεσα.</translation>
    </message>
    <message>
        <source>Copy the staged files and folders with cp -a, restore ownership with chown --reference and byte-compare every regular file afterwards.</source>
        <translation>Αντιγραφή των σκηνοθετημένων αρχείων και φακέλων με cp -a, αποκατάσταση της ιδιοκτησίας με chown -- αναφορά και byte-compare κάθε κανονικό αρχείο στη συνέχεια.</translation>
    </message>
    <message>
        <source>File Copy unavailable</source>
        <translation>Αντιγραφή αρχείου μη διαθέσιμου</translation>
    </message>
    <message>
        <source>Choose host destination folder</source>
        <translation>Επιλογή φακέλου προορισμού υπολογιστή</translation>
    </message>
    <message>
        <source>Select a repair target</source>
        <translation>Επιλέξτε έναν στόχο επισκευής</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it.</source>
        <translation>Επιλέξτε την μονάδα επισκευής σε Συστήματα πριν επιλέξετε έναν προορισμό μέσα σε αυτό.</translation>
    </message>
    <message>
        <source>File Copy - Browse Target Folders</source>
        <translation>Αντιγραφή αρχείου - Περιήγηση φακέλων στόχων</translation>
    </message>
    <message>
        <source>Browse Target Folders</source>
        <translation>Περιήγηση φακέλων προορισμού</translation>
    </message>
    <message>
        <source>The helper could not list the repair-system folder:

%1</source>
        <translation>Ο βοηθός δεν μπόρεσε να απαριθμήσει το φάκελο επισκευής συστήματος:

%1</translation>
    </message>
    <message>
        <source>BROWSE_ENTRY</source>
        <translation>ΒΡΑΧΥΠΡΟΘΕΣΜΟΙ</translation>
    </message>
    <message>
        <source>(choose this folder: %1)</source>
        <translation>(επιλέξτε αυτόν τον φάκελο: %1)</translation>
    </message>
    <message>
        <source>(parent folder)</source>
        <translation>(Μητρικός φάκελος)</translation>
    </message>
    <message>
        <source>Select repair-system destination</source>
        <translation>Επιλογή προορισμού συστήματος επισκευής</translation>
    </message>
    <message>
        <source>Choose a destination folder inside the repaired system (current folder: %1):</source>
        <translation>Επιλέξτε ένα φάκελο προορισμού μέσα στο επισκευασμένο σύστημα (τρέχων φάκελος: %1):</translation>
    </message>
    <message>
        <source>Folder</source>
        <translation>Φάκελος</translation>
    </message>
    <message>
        <source>Open</source>
        <translation>Άνοιγμα</translation>
    </message>
    <message>
        <source>(choose this folder:</source>
        <translation>(επιλέξτε αυτόν το φάκελο:</translation>
    </message>
    <message>
        <source>Add files to copy</source>
        <translation>Προσθήκη αρχείων για αντιγραφή</translation>
    </message>
    <message>
        <source>Add folder to copy</source>
        <translation>Προσθήκη φακέλου στην αντιγραφή</translation>
    </message>
    <message>
        <source>Stage at least one source and name a destination path first.</source>
        <translation>Στάδιο τουλάχιστον μία πηγή και όνομα ένα μονοπάτι προορισμού πρώτα.</translation>
    </message>
    <message>
        <source>Copy the %1 staged item(s) to %2?

The helper keeps its direction and path containment checks; a sensitive repair-system destination is refused unless the helper approves it, and every regular file is byte-compared after the copy.</source>
        <translation>Αντιγραφή του ή των σταδίων %1 σε %2;

Ο βοηθός διατηρεί την κατεύθυνση και τους ελέγχους περιορισμού της διαδρομής του. Ένας ευαίσθητος προορισμός επισκευών-συστήματος απορρίπτεται, εκτός αν ο βοηθός τον εγκρίνει, και κάθε κανονικό αρχείο byte-συγκρίνεται μετά το αντίγραφο.</translation>
    </message>
    <message>
        <source>File Copy - Copy and Verify</source>
        <translation>Αντιγραφή αρχείου - Αντιγραφή και επαλήθευση</translation>
    </message>
    <message>
        <source>File Copy - Preview Changes</source>
        <translation>Αντιγραφή αρχείου - Αλλαγή προεπισκόπησης</translation>
    </message>
    <message>
        <source>Protected running host - details</source>
        <translation>Προστατευόμενη λειτουργία host - λεπτομέρειες</translation>
    </message>
    <message>
        <source>not mounted (offline target)</source>
        <translation>δεν έχει τοποθετηθεί (εκτός σύνδεσης στόχος)</translation>
    </message>
    <message>
        <source>Select a drive to see its details.</source>
        <translation>Επιλέξτε μια κίνηση για να δείτε τις λεπτομέρειες της.</translation>
    </message>
    <message>
        <source>Inspecting the selected component.</source>
        <translation>Έλεγχος του επιλεγμένου συστατικού.</translation>
    </message>
    <message>
        <source>Helper-confirmed by the last read-only diagnostics.</source>
        <translation>Ο βοηθός επιβεβαιώθηκε από τα τελευταία διαγνωστικά μόνο ανάγνωσης.</translation>
    </message>
    <message>
        <source>Read-only inventory only; run diagnostics to confirm.</source>
        <translation>Μόνο για ανάγνωση-μόνο απογραφή; εκτελέστε τα διαγνωστικά για να επιβεβαιώσετε.</translation>
    </message>
    <message>
        <source>PROTECTED - running system; read-only details only</source>
        <translation>ΠΡΟΣΤΑΤΕΥΟΜΕΝΟ - λειτουργικό σύστημα; μόνο για ανάγνωση-μόνο λεπτομέρειες</translation>
    </message>
    <message>
        <source>Live / installer media - not selectable</source>
        <translation>Live / installer media - δεν επιλέγεται</translation>
    </message>
    <message>
        <source>PROTECTED - running host scope</source>
        <translation>ΠΡΟΣΤΑΤΕΥΟΜΕΝΟ - τρέχει πεδίο υποδοχής</translation>
    </message>
    <message>
        <source>Unlock required before selection</source>
        <translation>Ξεκλείδωμα που απαιτείται πριν από την επιλογή</translation>
    </message>
    <message>
        <source>Eligible repair candidate</source>
        <translation>Επιλέξιμος υποψήφιος επισκευής</translation>
    </message>
    <message>
        <source>Drive:</source>
        <translation>Οδήγα:</translation>
    </message>
    <message>
        <source>Detected target:</source>
        <translation>Ανιχνευμένος στόχος:</translation>
    </message>
    <message>
        <source>Pending inspection</source>
        <translation>Εν αναμονή της επιθεώρησης</translation>
    </message>
    <message>
        <source>Model / label:</source>
        <translation>Μοντέλο / ετικέτα:</translation>
    </message>
    <message>
        <source>Status:</source>
        <translation>Κατάσταση:</translation>
    </message>
    <message>
        <source>Size:</source>
        <translation>Μέγεθος:</translation>
    </message>
    <message>
        <source>Connection:</source>
        <translation>Σύνδεση:</translation>
    </message>
    <message>
        <source>Filesystem:</source>
        <translation>Σύστημα αρχείων:</translation>
    </message>
    <message>
        <source>UUID:</source>
        <translation>- Ναι.</translation>
    </message>
    <message>
        <source>Mounts:</source>
        <translation>Προσαρτήματα:</translation>
    </message>
    <message>
        <source>Running system protection unresolved</source>
        <translation>Ανεπίλυτη προστασία του συστήματος κίνησης</translation>
    </message>
    <message>
        <source>No protected physical backing disk was identified</source>
        <translation>Δεν αναγνωρίστηκε προστατευμένος φυσικός δίσκος υποστήριξης</translation>
    </message>
    <message>
        <source>Current running Linux system</source>
        <translation>Τρέχον σύστημα Linux</translation>
    </message>
    <message>
        <source>Critical mounts: %1</source>
        <translation>Κριτικές βάσεις: %1</translation>
    </message>
    <message>
        <source>Read-only protected-running-system facts: the helper OS fact plus the inventory model, device path, size, transport and critical mounts. Nothing here is probed destructively.</source>
        <translation>Μόνο ανάγνωση-προστατευόμενα-τρέχοντας-συστήματα γεγονότα: ο βοηθός OS γεγονός συν το μοντέλο απογραφής, τη διαδρομή της συσκευής, το μέγεθος, τη μεταφορά και τα κρίσιμα mounts. Τίποτα εδώ δεν ερευνάται καταστροφικά.</translation>
    </message>
    <message>
        <source>Select a drive to see unlock status.</source>
        <translation>Επιλέξτε μια κίνηση για να δείτε την κατάσταση ξεκλειδώματος.</translation>
    </message>
    <message>
        <source>State: protected
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
The protected running host cannot be unlocked or modified; unlock is available for an offline repair target only. Use Host Maintenance for the protected running host.</source>
        <translation>Κράτος: προστατευόμενη
Συστατικό: %1
Mapper: (Δεν υπάρχει)
Μέθοδος: ξεκλείδωμα βοηθού (κρυπτογράφηση· φράση πρόσβασης μέσω ενός αρχείου πλήκτρων mode-600, διαγραμμένο μετά τη χρήση)
Ο προστατευόμενος υπολογιστής που εκτελείται δεν μπορεί να ξεκλειδωθεί ή να τροποποιηθεί· το ξεκλείδωμα είναι διαθέσιμο μόνο για έναν εκτός σύνδεσης στόχο επισκευής. Χρησιμοποιήστε Συντήρηση υπολογιστή για τον προστατευμένο τρέχοντα ξενιστή.</translation>
    </message>
    <message>
        <source>(none detected)</source>
        <translation>(Δεν ανιχνεύθηκε κανείς)</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
A locked LUKS container is visible on this drive; press Unlock to open it for this recovery session.</source>
        <translation>Κατάσταση: κλειδωμένη
Συστατικό: %1
Mapper: (Δεν υπάρχει)
Μέθοδος: ξεκλείδωμα βοηθού (κρυπτογράφηση· φράση πρόσβασης μέσω ενός αρχείου πλήκτρων mode-600, διαγραμμένο μετά τη χρήση)
Ένα κλειδωμένο δοχείο LUKS είναι ορατό σε αυτή την κίνηση; πατήστε Ξεκλειδώστε για να το ανοίξετε για αυτή τη συνεδρία αποκατάστασης.</translation>
    </message>
    <message>
        <source>(visible mapper)</source>
        <translation>(ορατή χαρτογράφηση)</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: already open before this session (visible mapper; Boot Bitch will reuse it and will not close it).</source>
        <translation>Κατάσταση: ξεκλείδωτη
Συστατικό: %1
Χαρτογράφηση: %2
Μέθοδος: Ανοίγει ήδη πριν από αυτή τη συνεδρία (ορατός χάρτης; Boot Bitch θα το επαναχρησιμοποιήσει και δεν θα το κλείσει).</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper-confirmed mapping from the last read-only diagnostics.</source>
        <translation>Κατάσταση: ξεκλείδωτη
Συστατικό: %1
Χαρτογράφηση: %2
Μέθοδος: επιβεβαιωμένη χαρτογράφηση βοηθού από τα τελευταία διαγνωστικά μόνο ανάγνωσης.</translation>
    </message>
    <message>
        <source>(mapper)</source>
        <translation>(Μαχαίρι)</translation>
    </message>
    <message>
        <source>State: locked or no encrypted component detected
Component: (none detected)
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
No locked LUKS component and no unlock operation were recorded for this drive in the current session.</source>
        <translation>Κατάσταση: κλειδωμένο ή μη ανιχνεύσιμο κρυπτογραφημένο συστατικό
Συστατικό: (δεν ανιχνεύθηκε)
Mapper: (Δεν υπάρχει)
Μέθοδος: ξεκλείδωμα βοηθού (κρυπτογράφηση· φράση πρόσβασης μέσω ενός αρχείου πλήκτρων mode-600, διαγραμμένο μετά τη χρήση)
Κανένα κλειδωμένο συστατικό LUKS και καμία λειτουργία ξεκλειδώματος δεν καταγράφηκαν για αυτή την κίνηση στην τρέχουσα συνεδρία.</translation>
    </message>
    <message>
        <source>unavailable - %1</source>
        <translation>Μη διαθέσιμο - %1</translation>
    </message>
    <message>
        <source>Host maintenance:
%1</source>
        <translation>Συντήρηση υπολογιστή:
%1</translation>
    </message>
    <message>
        <source>Target:
%1 + %2</source>
        <translation>Στόχος:
%1 + %2</translation>
    </message>
    <message>
        <source>Authorization required: diagnostics and repairs fail closed until you press Authorize.</source>
        <translation>Απαιτείται εξουσιοδότηση: τα διαγνωστικά και οι επισκευές αποτυγχάνουν μέχρι να πατήσετε την άδεια.</translation>
    </message>
    <message>
        <source>Re-establish the privileged helper session for the current scope now. The password is requested in the hidden-input modal and is never logged.</source>
        <translation>Αποκαταστήστε την προνομιακή συνεδρία βοήθειας για το τρέχον πεδίο εφαρμογής τώρα. Ο κωδικός πρόσβασης ζητείται στο modal κρυφής εισόδου και δεν καταγράφεται ποτέ.</translation>
    </message>
    <message>
        <source>Running...</source>
        <translation>Τρέξιμο...</translation>
    </message>
    <message>
        <source>Scope required</source>
        <translation>Απαιτούμενο πεδίο εφαρμογής</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope; this unlocks the gated actions.</source>
        <translation>Εκτέλεση όλων - εκτελέστε κάθε διαθέσιμο διαγνωστικό μόνο ανάγνωσης για το τρέχον πεδίο εφαρμογής? Αυτό ξεκλειδώνει τις ενέργειες πύλη.</translation>
    </message>
    <message>
        <source>Select a target and wait for any running command first.</source>
        <translation>Επιλέξτε ένα στόχο και περιμένετε για οποιαδήποτε εντολή που εκτελείται πρώτα.</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic through the helper.</source>
        <translation>Εκτέλεση διαγνωστικού - εκτελέστε το επιλεγμένο διαγνωστικό μόνο ανάγνωσης μέσω του βοηθού.</translation>
    </message>
    <message>
        <source>Run All for the current scope and refresh the read-only backend profile.</source>
        <translation>Εκτέλεση όλων για την τρέχουσα εμβέλεια και ανανέωση του προφίλ υποστήριξης μόνο ανάγνωσης.</translation>
    </message>
    <message>
        <source>Read or edit the selected target configuration file through the guarded helper; a saved edit invalidates cached diagnostics.</source>
        <translation>Διαβάστε ή επεξεργαστείτε το επιλεγμένο αρχείο ρυθμίσεων στόχου μέσω του φυλασσόμενου βοηθού: μια αποθηκευμένη επεξεργασία ακυρώνει τα κρυφά διαγνωστικά.</translation>
    </message>
    <message>
        <source>Host Maintenance has no target-file editing; commit an offline repair target first.</source>
        <translation>Συντήρηση Host δεν έχει επεξεργασία αρχείου στόχου? Διαπράττετε ένα offline στόχο επισκευής πρώτα.</translation>
    </message>
    <message>
        <source>Commit an offline repair target first.</source>
        <translation>Διαθέστε έναν εκτός σύνδεσης στόχο επισκευής πρώτα.</translation>
    </message>
    <message>
        <source>No target configuration file is available for this target; run diagnostics to probe the list.</source>
        <translation>Δεν είναι διαθέσιμο αρχείο ρυθμίσεων στόχου για αυτόν το στόχο, εκτελέστε τα διαγνωστικά για να διερευνήσετε τη λίστα.</translation>
    </message>
    <message>
        <source>Already Unlocked</source>
        <translation>Ήδη ξεκλειδωμένο</translation>
    </message>
    <message>
        <source>An unlocked Linux filesystem is already visible on this drive. Boot Bitch will reuse the existing mapper and will not close or reopen a mapping created by this recovery session. Mounting happens during diagnostics (read-only) and repairs (read-write); data filesystems are never auto-mounted on selection.</source>
        <translation>Ένα ξεκλείδωτο σύστημα αρχείων Linux είναι ήδη ορατό σε αυτή την κίνηση. Το Boot Bitch θα επαναχρησιμοποιήσει τον υπάρχοντα χάρτη και δεν θα κλείσει ούτε θα ανοίξει ξανά μια χαρτογράφηση που δημιουργήθηκε από αυτή τη συνεδρία ανάκτησης. Η τοποθέτηση γίνεται κατά τη διάρκεια διαγνωστικών (μόνο ανάγνωση) και επισκευών (ανάγνωση-γραφή)· τα συστήματα αρχείων δεδομένων δεν τοποθετούνται ποτέ αυτόματα στην επιλογή.</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked; use Host Maintenance for the protected running host.</source>
        <translation>Ο προστατευόμενος τρέχοντας υπολογιστής δεν μπορεί να ξεκλειδωθεί· χρησιμοποιήστε τη συντήρηση του υπολογιστή για τον προστατευμένο τρέχοντα υπολογιστή.</translation>
    </message>
    <message>
        <source>Host Maintenance is the current scope, but the selected offline drive can still be unlocked.</source>
        <translation>Συντήρηση υπολογιστή είναι η τρέχουσα εμβέλεια, αλλά η επιλεγμένη εκτός σύνδεσης κίνηση μπορεί ακόμα να ξεκλειδωθεί.</translation>
    </message>
    <message>
        <source>Unlock %1 using cryptsetup through the privileged helper. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>Ξεκλειδώστε το %1 χρησιμοποιώντας το cryptsetup μέσω του προνομιούχου βοηθού. Η φράση πρόσβασης ταξιδεύει μέσω ενός ιδιωτικού αρχείου κλειδιού και δεν τοποθετείται ποτέ σε παραμέτρους εντολών ή καταγραφές.</translation>
    </message>
    <message>
        <source>The protected running system cannot be selected as a repair target; use Host Maintenance for the protected running host.</source>
        <translation>Το προστατευμένο σύστημα εκτέλεσης δεν μπορεί να επιλεγεί ως στόχος επισκευής.Χρησιμοποιήστε τη συντήρηση υπολογιστή για τον προστατευμένο υπολογιστή εκτέλεσης.</translation>
    </message>
    <message>
        <source>Live / installer media is read-only boot media and cannot be selected as a repair target.</source>
        <translation>Τα live / installer media είναι μόνο για ανάγνωση και δεν μπορούν να επιλεγούν ως στόχος επισκευής.</translation>
    </message>
    <message>
        <source>Unlock the encrypted volume first; Select Target becomes available after a Linux filesystem is detected.</source>
        <translation>Ξεκλειδώστε πρώτα τον κρυπτογραφημένο τόμο: Το Select Target γίνεται διαθέσιμο μετά την ανίχνευση ενός συστήματος αρχείων Linux.</translation>
    </message>
    <message>
        <source>Committed repair target. Repair, Diagnostics and File Copy target this physical drive until another drive is explicitly selected with Select Target.</source>
        <translation>Δεσμευμένος στόχος επισκευής. Επισκευή, Διαγνωστικά και Αντιγραφή αρχείων στοχοθετούν αυτή τη φυσική κίνηση μέχρι μια άλλη κίνηση να επιλεγεί ρητά με τον επιλεγμένο στόχο.</translation>
    </message>
    <message>
        <source>Commit %1 as the repair target; this leaves Host Maintenance and switches the scope to the selected drive.</source>
        <translation>Συμπεριλάβετε το %1 ως στόχο επισκευής· αυτό αφήνει τη συντήρηση του υπολογιστή και αλλάζει το πεδίο εφαρμογής στην επιλεγμένη κίνηση.</translation>
    </message>
    <message>
        <source>The running host target could not be detected.</source>
        <translation>Ο στόχος δεν εντοπίστηκε.</translation>
    </message>
    <message>
        <source>Leave Host Maintenance and return to repair-target mode.</source>
        <translation>Αφήστε τη συντήρηση του υπολογιστή και επιστρέψτε σε λειτουργία επισκευής-στόχου.</translation>
    </message>
    <message>
        <source>Select the running host for deliberate guarded maintenance; the administrator authorization is requested here once and cached for the session.</source>
        <translation>Επιλέξτε τον τρέχοντα υπολογιστή για σκόπιμη φύλαξη συντήρησης.Η άδεια διαχειριστή ζητείται εδώ μία φορά και είναι κρυμμένη για τη συνεδρία.</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a repair target.</source>
        <translation>Ο τρέχων στόχος ξενιστή δεν μπόρεσε να ανιχνευθεί· τα διαγνωστικά χρειάζονται έναν στόχο επισκευής.</translation>
    </message>
    <message>
        <source>Committed target: %1 + %2</source>
        <translation>Δεσμευμένος στόχος: %1 + %2</translation>
    </message>
    <message>
        <source>Committed target: none (selection changed)</source>
        <translation>Δεσμευμένος στόχος: καμία (αλλαγή επιλογής)</translation>
    </message>
    <message>
        <source>The selected scope has no resolved Linux root component; Refresh Devices and commit the repair target again.</source>
        <translation>Το επιλεγμένο πεδίο εφαρμογής δεν έχει επιλύσει το ριζικό συστατικό του Linux.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize on the Systems or Repair tab to re-establish it. Run All reuses the cached authorization and never prompts by itself.</source>
        <translation>Καμία συνεδρία διαχειριστή δεν είναι ενεργή· πατήστε Εξουσιοδότηση στην καρτέλα Συστημάτων ή Επισκευής για να την επαναφέρετε. Εκτελέστε όλες τις επαναχρησιμοποιήσεις της λανθάνουσας εξουσιοδότησης και ποτέ δεν οδηγεί μόνη της.</translation>
    </message>
    <message>
        <source>Run diagnostics for this scope to unlock the gated actions. Diagnostics are read-only and the only evidence source.</source>
        <translation>Εκτέλεση διαγνωστικών για αυτό το πεδίο για να ξεκλειδώσετε τις ενέργειες πύλη. Τα διαγνωστικά είναι μόνο για ανάγνωση και η μόνη πηγή στοιχείων.</translation>
    </message>
    <message>
        <source>A repair was not proven unchanged, so the cached diagnostics are invalidated. Run diagnostics again before another gated action.</source>
        <translation>Μια επισκευή δεν αποδείχθηκε αμετάβλητη, οπότε τα κρυμμένα διαγνωστικά ακυρώνονται. Εκτελέστε τα διαγνωστικά και πάλι πριν από μια άλλη πύλη δράσης.</translation>
    </message>
    <message>
        <source>Gated actions reflect the cached capability lines; the helper still runs every runtime preflight when a command starts.</source>
        <translation>Οι πτυσσόμενες ενέργειες αντικατοπτρίζουν τις κρυμμένες γραμμές ικανότητας· ο βοηθός εξακολουθεί να τρέχει κάθε προ πτήσης χρόνου λειτουργίας όταν ξεκινά μια εντολή.</translation>
    </message>
    <message>
        <source>Regenerating diagnostics automatically</source>
        <translation>Αυτόματη αναγέννηση διαγνωστικών</translation>
    </message>
    <message>
        <source>Running diagnostic: %1</source>
        <translation>Εκτέλεση διαγνωστικού: %1</translation>
    </message>
    <message>
        <source>Running all diagnostics</source>
        <translation>Εκτέλεση όλων των διαγνωστικών</translation>
    </message>
    <message>
        <source>Unlocking %1</source>
        <translation>Ξεκλείδωμα %1</translation>
    </message>
    <message>
        <source>running host</source>
        <translation>εκτέλεση υπολογιστή</translation>
    </message>
    <message>
        <source>offline target</source>
        <translation>εκτός σύνδεσης στόχος</translation>
    </message>
    <message>
        <source>host maintenance active</source>
        <translation>ενεργή συντήρηση υπολογιστή</translation>
    </message>
    <message>
        <source>repair target committed</source>
        <translation>στόχος επισκευής δεσμευμένος</translation>
    </message>
    <message>
        <source>no committed scope</source>
        <translation>χωρίς δεσμευμένο πεδίο εφαρμογής</translation>
    </message>
    <message>
        <source>Current session</source>
        <translation>Τρέχουσα συνεδρία</translation>
    </message>
    <message>
        <source>Log files (*.log);;All files (*)</source>
        <translation>Αρχεία καταγραφής (*.log);; Όλα τα αρχεία (*)</translation>
    </message>
    <message>
        <source>Save log as</source>
        <translation>Αποθήκευση καταγραφής ως</translation>
    </message>
    <message>
        <source>About Boot Bitch</source>
        <translation>Σχετικά με το Boot Bitch</translation>
    </message>
    <message>
        <source>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;&lt;p&gt;&lt;b&gt;Developer:&lt;/b&gt; CaptainMorgan12&lt;/p&gt;&lt;p&gt;This GUI is the package&apos;s entry point: a Qt 3.3.x frontend with the ported guarded helper for Debian Etch-era systems.&lt;/p&gt;&lt;p&gt;&lt;b&gt;Guarded repair mode:&lt;/b&gt; read-only diagnostics can inspect either the protected Running Host or an explicitly selected repair drive. The guarded Debian/APT package stages (interrupted configuration, broken dependencies, metadata refresh, upgrade), GRUB-legacy configuration regeneration, LUKS target unlock and guarded target-file editing run through the ported helper after confirmation; Host Maintenance enables the same supported stages natively on the active system after repeating the host identity and boot-mount checks.&lt;/p&gt;&lt;p&gt;The modern-only features - verified File Copy, Btrfs snapshot rollback, EFI/UKI and extlinux repair, boot-stack reconciliation and Make Default - are greyed with the helper&apos;s own probe reasons on this frontend; Arch/Alpine/Fedora package backends stay diagnostics-only here.&lt;/p&gt;&lt;p&gt;The first privileged action authorizes one cached administrator session per scope through a hidden-input modal (the Qt GUI remains unprivileged). It can be ended at any time from File - Lock Administrator Session.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;&lt;p&gt;&lt;b&gt;Αναπτήρας:&lt;/b&gt; Captain Morgan12&lt;/p&gt;&lt;p&gt; Αυτό το GUI είναι το σημείο εισόδου του πακέτου: ένα γραφικό περιβάλλον Qt 3.3.x με το φορητό φυλασσόμενο βοηθό για τα συστήματα Etch-era του Debian. &lt;/p&gt;&lt;p&gt;&lt;b&gt;Εγγυημένη λειτουργία επισκευής: &lt;/b&gt; Μόνο για ανάγνωση τα διαγνωστικά μπορούν να επιθεωρήσουν είτε τον προστατευόμενο υπολογιστή που τρέχει είτε μια ρητά επιλεγμένη μονάδα επισκευής. Τα φυλασσόμενα στάδια πακέτων Debian/APT (διακοπτόμενες ρυθμίσεις, διακοπτόμενες εξαρτήσεις, ανανέωση μεταδεδομένων, αναβάθμιση), αναγέννηση ρυθμίσεων GRUB-legacy, στόχος LUKS ξεκλειδώνει και φυλασσόμενη επεξεργασία αρχείου-στόχου τρέχει μέσα από τον φορητό βοηθό μετά την επιβεβαίωση; Συντήρηση υπολογιστή επιτρέπει τα ίδια υποστηριζόμενα στάδια εγγενώς στο ενεργό σύστημα μετά την επανάληψη της ταυτότητας του υπολογιστή και ελέγχου εκκίνησης-προσάρτησης.&lt;/p&gt;&lt;p&gt; Τα σύγχρονα μόνο χαρακτηριστικά - επαληθευμένη αντιγραφή αρχείων, στιγμιότυπα Btrfs rollback, επισκευή EFI/UKI και extlinux, συμφιλίωση εκκίνησης-stack και προκαθορισμένο - είναι γκριζωσμένα με τους λόγους καθετήρα του βοηθού σε αυτό το frontend; Arch/Alpine/Fedora πακέτα υποστήριξης παραμένουν διαγνωστικά μόνο εδώ. &lt;/p&gt;&lt;p&gt; Η πρώτη προνομιακή ενέργεια επιτρέπει μια κρυφή συνεδρία διαχειριστή ανά πεδίο εφαρμογής μέσω ενός μέσου κρυφής εισόδου (το GUI Qt παραμένει μη προνομιούχο). Μπορεί να τερματιστεί ανά πάσα στιγμή από τη συνεδρία διαχειριστή αρχείων - κλειδώματος.&lt;/p&gt;</translation>
    </message>
</context>
</TS>
