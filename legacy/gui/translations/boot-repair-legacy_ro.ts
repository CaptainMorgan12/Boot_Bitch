<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS language="ro">
<context>
    <name>LegacyMainWindow</name>
    <message>
        <source>Validate environment</source>
        <translation>Validarea mediului</translation>
    </message>
    <message>
        <source>Validate</source>
        <translation>Validare</translation>
    </message>
    <message>
        <source>Check the selected system&apos;s mounts, filesystem metadata, boot files, mapper consistency and dependency readiness before any repair action. This is an independent safety preflight rather than an optional Full Repair stage.</source>
        <translation>Verificați montajele sistemului selectat, metadatele sistemului de fișiere, fișierele de boot, consistența mapper și disponibilitatea dependenței înainte de orice acțiune de reparații. Acesta este mai degrabă un zbor independent de siguranță decât o etapă opțională de reparații complete.</translation>
    </message>
    <message>
        <source>Always preflight</source>
        <translation>Întotdeauna înainte de zbor</translation>
    </message>
    <message>
        <source>File system repair</source>
        <translation>Reparația sistemului de fișiere</translation>
    </message>
    <message>
        <source>Check File Systems</source>
        <translation>Verificați sistemele de fișiere</translation>
    </message>
    <message>
        <source>Run the read-only file system check for the selected system&apos;s root and /boot filesystems and report each device&apos;s check tool and result without changing anything. This legacy frontend exposes the read-only check only; device repair is not wired.</source>
        <translation>Rulați verificarea sistemului de fișiere numai pentru citire pentru sistemul selectat de rădăcină și /boot de fișiere și raportează instrumentul de verificare al fiecărui dispozitiv și rezultatul fără a schimba nimic. Acest frontend moștenire expune numai verificarea numai citire; repararea dispozitivului nu este cu fir.</translation>
    </message>
    <message>
        <source>Full Repair plan: unavailable on this frontend - read-only check only</source>
        <translation>Plan complet de reparații: indisponibil pe acest frontend - numai verificare numai citire</translation>
    </message>
    <message>
        <source>Complete package configuration</source>
        <translation>Configurare pachet complet</translation>
    </message>
    <message>
        <source>Complete Configuration</source>
        <translation>Configurare completă</translation>
    </message>
    <message>
        <source>Complete interrupted dpkg package configuration in the selected repair system. This is the same stage controlled by Settings -&gt; Full Repair plan -&gt; Complete interrupted package configuration, but it can also be run independently here.</source>
        <translation>Configurația completă întreruptă a pachetului dpkg în sistemul de reparații selectat. Aceasta este aceeași etapă controlată de Setări -&gt; Plan complet de reparații - &gt; Configurația completă întreruptă a pachetului, dar poate fi rulat și independent aici.</translation>
    </message>
    <message>
        <source>Run the guarded dpkg-configure repair? The helper keeps its package-lock and runtime preflights.</source>
        <translation>Rulați reparatia pazita DPkg-configurare? Ajutorul îşi menţine pachetele şi zborurile înainte de zbor.</translation>
    </message>
    <message>
        <source>Repair broken dependencies</source>
        <translation>Repararea dependențelor rupte</translation>
    </message>
    <message>
        <source>Repair Dependencies</source>
        <translation>Dependențe de reparații</translation>
    </message>
    <message>
        <source>Repair package dependencies in the selected repair system after the mandatory safety preflight. This maps directly to Settings -&gt; Repair broken package dependencies.</source>
        <translation>Dependențe de pachete de reparații în sistemul de reparații selectat după pre-zborul de siguranță obligatoriu. Această hartă direct către Setări - &gt; Repararea dependențe pachet rupt.</translation>
    </message>
    <message>
        <source>Run the guarded fix-broken repair? The helper keeps its simulation-first preflight and runtime guards.</source>
        <translation>Să ruleze reparaţiile stricate? Ajutor păstrează simularea-primul pre-zbor și runtime guards.</translation>
    </message>
    <message>
        <source>Refresh package metadata</source>
        <translation>Actualizează metadatele pachetului</translation>
    </message>
    <message>
        <source>Refresh Metadata</source>
        <translation>Actualizează metadatele</translation>
    </message>
    <message>
        <source>Refresh APT metadata in the selected repair system without upgrading installed packages. This maps directly to Settings -&gt; Refresh package metadata.</source>
        <translation>Reîmprospătați metadatele APT în sistemul de reparații selectat fără modernizarea pachetelor instalate. Această hartă direct către Setări - &gt; Reîmprospătează metadatele pachetului.</translation>
    </message>
    <message>
        <source>Refresh package metadata for the selected scope? The helper requires a reachable, trusted APT source.</source>
        <translation>Reîmprospătați metadatele pachetului pentru domeniul de aplicare selectat? Ajutorul necesită o sursă accesibilă, de încredere, APT.</translation>
    </message>
    <message>
        <source>Upgrade installed packages</source>
        <translation>Actualizează pachetele instalate</translation>
    </message>
    <message>
        <source>Simulate and Upgrade</source>
        <translation>Simulează și actualizează</translation>
    </message>
    <message>
        <source>Simulate the APT transaction first, inspect proposed removals, then apply a safe upgrade. This maps directly to Settings -&gt; Upgrade installed packages.</source>
        <translation>Simulați mai întâi tranzacția APT, inspectați absorbțiile propuse, apoi aplicați o actualizare în condiții de siguranță. Această hartă direct către Setări - &gt; Actualizează pachetele instalate.</translation>
    </message>
    <message>
        <source>Run the guarded apt-upgrade transaction? The helper keeps its simulation-first and source guards.</source>
        <translation>Să conduci tranzacţia păzită? Ajutorul îşi păstrează prima simulare şi paznicii de la sursă.</translation>
    </message>
    <message>
        <source>DKMS</source>
        <translation>DKMS</translation>
    </message>
    <message>
        <source>Rebuild DKMS</source>
        <translation>Reconstruiește DKMS</translation>
    </message>
    <message>
        <source>Rebuild out-of-tree kernel modules for kernels installed in the selected system. The helper refuses this action when DKMS is not installed; this legacy frontend exposes no DKMS action.</source>
        <translation>Reconstrucți module de nucleu out-of-tree pentru nuclee instalate în sistemul selectat. Ajutorul refuză această acțiune atunci când DKMS nu este instalat; acest frontend moștenit nu expune nicio acțiune DKMS.</translation>
    </message>
    <message>
        <source>Graphical login / display manager</source>
        <translation>Autentificare grafică / manager de afișare</translation>
    </message>
    <message>
        <source>Restore Graphical Login</source>
        <translation>Restaurează autentificare grafică</translation>
    </message>
    <message>
        <source>Restore the legacy SysV display manager configured for the running host: the /etc/X11/default-display-manager entry and the missing runlevel S-symlink, with a backup and rollback, never starting the GUI. This is a host-scope stage on this legacy frontend.</source>
        <translation>Restaurați moștenirea SysV display manager configurat pentru gazda care rulează: /etc/X11/default-display-manager intrare și lipsă runlevel S-simlink, cu un backup și rollback, niciodată nu începe GUI. Aceasta este o scenă de gazdă-scop pe acest frontend moștenire.</translation>
    </message>
    <message>
        <source>Restore the graphical login configuration for the running host? The helper backs up /etc/X11/default-display-manager and the runlevel symlink state, restores the configured entry and the missing S-symlink, rolls back on any failure, and never starts the display manager.</source>
        <translation>Restaurați configurația grafică de autentificare pentru gazda care rulează? Ajutor backs up /etc/X11/default-display-manager și starea runlevel Symlink, restabilește intrarea configurată și S-simlink lipsă, se rostogolește înapoi pe orice eșec, și niciodată nu începe managerul de afișare.</translation>
    </message>
    <message>
        <source>Initramfs</source>
        <translation>Initramfs</translation>
    </message>
    <message>
        <source>Rebuild Initramfs</source>
        <translation>Reconstrucția initramfs</translation>
    </message>
    <message>
        <source>Rebuild initramfs images for the selected repair system only after mapper and crypttab consistency checks pass. The helper backs up each image before the apply. On Etch the stage runs through the guarded plain-chroot fallback (no unshare required).</source>
        <translation>Reconstruiţi imagini initramfs pentru sistemul de reparaţii selectat numai după verificarea consistenţei mapper şi crypttab. Ajutorul susţine fiecare imagine înainte de aplicare. Pe Etch scena trece prin reteaua pazita (nu este necesara nepartajarea).</translation>
    </message>
    <message>
        <source>Rebuild the initramfs for the selected scope? The helper keeps its mapper/crypttab and backup preflights.</source>
        <translation>Reconstrucția initramfs pentru domeniul de aplicare selectat? Ajutor păstrează mapper / crypttab și de rezervă preflights.</translation>
    </message>
    <message>
        <source>EFI / UKI bootloader</source>
        <translation>EFI / UKI bootloader</translation>
    </message>
    <message>
        <source>Repair EFI / UKI</source>
        <translation>Reparaţii EFI / UKI</translation>
    </message>
    <message>
        <source>Repair the selected system&apos;s EFI / UKI boot path. This legacy frontend exposes no EFI action; the Etch target is a BIOS/GRUB-legacy system.</source>
        <translation>Repara calea de boot EFI / UKI a sistemului selectat. Acest frontend moștenit nu expune nicio acțiune EFI; ținta Etch este un sistem BIOS/GRUB-legacy.</translation>
    </message>
    <message>
        <source>GRUB configuration</source>
        <translation>Configurare GRUB</translation>
    </message>
    <message>
        <source>Regenerate GRUB</source>
        <translation>Regenerează GRUB</translation>
    </message>
    <message>
        <source>Regenerate the selected repair system&apos;s GRUB menu/configuration after the mandatory safety preflight. The helper backs up menu.lst, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Regenerați meniul/configurarea GRUB a sistemului de reparații selectat după preconfigurarea obligatorie a siguranței. Ajutorul susţine meniul.Ist, păstrează fiecare intrare de boot existentă şi se rostogoleşte înapoi pe orice eşec.</translation>
    </message>
    <message>
        <source>Regenerate the GRUB configuration? The helper backs up the target menu/configuration, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Regeneraţi configuraţia GRUB? Ajutorul susţine meniul/configurarea ţintei, păstrează fiecare intrare şi se rostogoleşte înapoi pe orice eşec.</translation>
    </message>
    <message>
        <source>extlinux configuration</source>
        <translation>Configurare extlinux</translation>
    </message>
    <message>
        <source>Regenerate extlinux</source>
        <translation>Regenerează extlinux</translation>
    </message>
    <message>
        <source>Regenerate the selected system&apos;s extlinux bootloader configuration. This legacy frontend exposes no extlinux action.</source>
        <translation>Regenerați configurația bootloaderului extlinux a sistemului selectat. Acest frontend moștenire expune nici o acțiune extlinux.</translation>
    </message>
    <message>
        <source>Boot stack reconciliation</source>
        <translation>Reconcilierea stiva de boot</translation>
    </message>
    <message>
        <source>Reconcile Boot Stack</source>
        <translation>Reconcile Boot Stack</translation>
    </message>
    <message>
        <source>Reconcile the selected repair system&apos;s boot stack in one guarded legacy pass: mapper/crypttab validation, initramfs rebuild and GRUB-legacy configuration regeneration, with the component backups and preflights unchanged. This is the Etch equivalent of the modern boot-stack reconciliation and stays out of the Full Repair plan.</source>
        <translation>Reconciliați stiva de boot a sistemului de reparații selectat într-un singur permis de moștenire păzit: validarea mapper/cripttab, reconstruirea initramfs și regenerarea configurației GRUB-legatery, cu componentele de rezervă și prezblând neschimbat. Acesta este echivalentul Etch al reconcilierii moderne boot-stack și rămâne în afara planului de reparații complete.</translation>
    </message>
    <message>
        <source>Full Repair plan: Manual recovery tool</source>
        <translation>Plan de reparații complete: instrument de recuperare manuală</translation>
    </message>
    <message>
        <source>Run the guarded boot-stack reconciliation? The helper runs the mapper/crypttab validation, the initramfs rebuild and the GRUB-legacy regeneration in one pass with every component preflight and backup.</source>
        <translation>Rulați reconcilierea boot-stack păzit? Ajutorul ruleaza validarea mapper / crypttab, reconstructia initramfs si regenerarea GRUB-legacy intr-o singura trecere cu fiecare componenta de pre-zbor si backup.</translation>
    </message>
    <message>
        <source>Complete interrupted package configuration</source>
        <translation>Configuraţia completă întreruptă a ambalajului</translation>
    </message>
    <message>
        <source>Repair broken package dependencies</source>
        <translation>Reparații defect de dependență pachet</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Implicit</translation>
    </message>
    <message>
        <source>Repair file system errors (read-only check first)</source>
        <translation>Erorile sistemului de fișiere de reparații (verificări numai pentru citire mai întâi)</translation>
    </message>
    <message>
        <source>the legacy frontend exposes the read-only file system check only; per-device repair is not wired on this frontend (fail closed)</source>
        <translation>frontend moștenire expune numai verificarea sistemului de fișiere numai pentru citire; reparația per dispozitiv nu este cu fir pe acest frontend (închidere eșuată)</translation>
    </message>
    <message>
        <source>Upgrade installed packages (adaptive APT simulation)</source>
        <translation>Actualizează pachetele instalate ( Simulare adaptivă APT)</translation>
    </message>
    <message>
        <source>Rebuild DKMS modules</source>
        <translation>Reconstrucția modulelor DKMS</translation>
    </message>
    <message>
        <source>Restore graphical login manager</source>
        <translation>Restaurează administratorul de autentificare grafică</translation>
    </message>
    <message>
        <source>Rebuild initramfs after mapper/crypttab validation</source>
        <translation>Reconstruiește initramfs după validarea mapper/cripttab</translation>
    </message>
    <message>
        <source>Repair EFI / UKI boot path</source>
        <translation>Reparație Tra 26X / Tra27X cale de boot</translation>
    </message>
    <message>
        <source>Update GRUB configuration</source>
        <translation>Actualizează configurația GRUB</translation>
    </message>
    <message>
        <source>Update extlinux configuration</source>
        <translation>Actualizează configurația extlinux</translation>
    </message>
    <message>
        <source>Choose Full Repair stages in Settings. Enabled stages run in the order shown. Each configurable stage also appears below as an individual tool; the Full Repair column mirrors its current Settings state. The boot tools (EFI / UKI bootloader, GRUB or extlinux configuration, boot-stack reconciliation and Make Default) are independent: run them in any order, and a later action re-verifies what an earlier one changed and reports its own result. The active scope is shown beside Repair: selected repair drive or Running Host maintenance.</source>
        <translation>Alegeți etapele de reparații complete în setări. Etape activate rula în ordinea afișată. Fiecare etapă configurabilă apare și mai jos ca instrument individual; coloana de reparații complete reflectă starea actuală a Configurărilor. Uneltele de boot (EFI / UKI bootloader, GRUB sau extlinux configuraţie, boot-stack reconciliere şi Make Implicit) sunt independente: executaţi-le în orice ordine, şi o acţiune ulterioară re-verifică ceea ce o mai devreme a schimbat şi raportează propriul rezultat. Domeniul de aplicare activ este afişat lângă Reparare: unitate de reparaţii selectate sau de întreţinere Rularea gazde.</translation>
    </message>
    <message>
        <source>Run All diagnostics for the selected target or running host before starting Full Repair. The report is read-only evidence used to choose and confirm repair stages.</source>
        <translation>Rulați toate diagnosticele pentru ținta selectată sau gazda care rulează înainte de a începe reparația completă. Raportul este o dovadă doar pentru citire utilizată pentru a alege și confirma etapele de reparații.</translation>
    </message>
    <message>
        <source>Ready: required cached read-only diagnostics are available for the selected stages. Review them in Diagnostics or Logs before confirming.</source>
        <translation>Gata: pentru etapele selectate sunt disponibile diagnosticele necesare numai pentru citire. Examinați-le în Diagnostic sau jurnale înainte de confirmarea.</translation>
    </message>
    <message>
        <source>Environment validation</source>
        <translation>Validarea mediului</translation>
    </message>
    <message>
        <source>Summarizes the selected system, protection state, mounted identity and inspection readiness.</source>
        <translation>Sumarizează sistemul selectat, starea de protecție, disponibilitatea de identitate montat și inspecție.</translation>
    </message>
    <message>
        <source>Distribution and boot backend profile</source>
        <translation>Profilul de distribuție și boot-backend</translation>
    </message>
    <message>
        <source>Identifies the distribution family, package manager, initramfs generator, bootloader and current guarded repair capability.</source>
        <translation>Identifică familia de distribuție, managerul de pachete, generatorul initramfs, bootloader și capacitatea curentă de reparații păzite.</translation>
    </message>
    <message>
        <source>Boot diagnostics</source>
        <translation>Diagnosticarea ghetelor</translation>
    </message>
    <message>
        <source>Shows boot mounts and /boot contents plus storage evidence without changing the selected system.</source>
        <translation>Afișează montări de boot și / conținut boot plus dovezi de stocare fără a schimba sistemul selectat.</translation>
    </message>
    <message>
        <source>Boot evidence and selection history</source>
        <translation>Proba de boot și istoricul de selecție</translation>
    </message>
    <message>
        <source>Correlates the detected boot chain, bootloader selection, kernel/initramfs and unlock evidence.</source>
        <translation>Corelează lanțul de boot detectat, selecția bootloader, nucleu/initramfs și deblocarea dovezilor.</translation>
    </message>
    <message>
        <source>Kernel / initramfs</source>
        <translation>Kernel / initramfs</translation>
    </message>
    <message>
        <source>Reviews kernel files and verifies matching initramfs images through a read-only inspection.</source>
        <translation>Revizuiește fișierele de nucleu și verifică potrivirea imaginilor initramfs printr-o inspecție numai-citire.</translation>
    </message>
    <message>
        <source>Reviews the GRUB configuration without changing boot files.</source>
        <translation>Revizuiește configurația GRUB fără a schimba fișierele de boot.</translation>
    </message>
    <message>
        <source>EFI / UKI boot state</source>
        <translation>EFI / UKI starea de pornire</translation>
    </message>
    <message>
        <source>Inspects EFI/UKI evidence; unavailable on this legacy BIOS frontend with the helper&apos;s probe reason.</source>
        <translation>Inspectează dovezile EFI/UKI; indisponibile pe acest frontend BIOS moștenire cu motivul sondei ajutorului.</translation>
    </message>
    <message>
        <source>Reviews the configured display manager and recent boot evidence without starting the GUI.</source>
        <translation>Revizuiește managerul de afișare configurat și dovezile recente de boot fără a porni GUI.</translation>
    </message>
    <message>
        <source>Boot errors</source>
        <translation>Erori de sarcină</translation>
    </message>
    <message>
        <source>Reads recent error-priority entries from the running host or selected repair system when available.</source>
        <translation>Citește intrările recente de eroare-prioritate de la gazda sau sistemul de reparații selectat atunci când sunt disponibile.</translation>
    </message>
    <message>
        <source>Disk usage</source>
        <translation>Utilizarea discului</translation>
    </message>
    <message>
        <source>Summarizes filesystem capacity and free space for the running host or read-only repair target.</source>
        <translation>Summarizes filesystem capacity and free space for the running hoste or ready- only reparation tinta.</translation>
    </message>
    <message>
        <source>File systems</source>
        <translation>Sisteme de fișiere</translation>
    </message>
    <message>
        <source>Runs the read-only file system check for the selected system&apos;s root, /boot and other filesystems.</source>
        <translation>Rulează verificarea sistemului de fișiere numai pentru citire pentru rădăcina sistemului selectat, /boot și alte sisteme de fișiere.</translation>
    </message>
    <message>
        <source>/etc/fstab review</source>
        <translation>/etc/fstab review</translation>
    </message>
    <message>
        <source>Displays the running host or selected repair system&apos;s fstab; repair-system inspection is mounted read-only.</source>
        <translation>Afișează fstab-ul gazdei de funcționare sau al sistemului de reparații selectat; inspecția sistemului de reparații este montată numai pe citire.</translation>
    </message>
    <message>
        <source>Btrfs status</source>
        <translation>Situația Btrfs</translation>
    </message>
    <message>
        <source>Shows Btrfs filesystem and subvolume information when the target uses Btrfs.</source>
        <translation>Afișează sistemul de fișiere Btrfs și subvolumul de informații atunci când ținta utilizează Btrfs.</translation>
    </message>
    <message>
        <source>Device-mapper ancestry</source>
        <translation>Ascensiune dispozitiv-mapper</translation>
    </message>
    <message>
        <source>Shows selected mapper ancestry and device-mapper state when available.</source>
        <translation>Afișează ancesiunile mapei selectate și starea mapper a dispozitivului atunci când sunt disponibile.</translation>
    </message>
    <message>
        <source>LUKS / crypttab evidence</source>
        <translation>LUKS / dovezi Cripttab</translation>
    </message>
    <message>
        <source>Shows LUKS/mapped ancestry plus crypttab and fstab mapper references.</source>
        <translation>Afișează referințe LUKS/mapped ancestry plus cripttab și fstab mapper.</translation>
    </message>
    <message>
        <source>Full diagnostic report</source>
        <translation>Raport complet de diagnostic</translation>
    </message>
    <message>
        <source>Combines all read-only diagnostics for the selected scope (same as Run All).</source>
        <translation>Combine toate diagnosticele de citire numai pentru domeniul de aplicare selectat (la fel ca Run All).</translation>
    </message>
    <message>
        <source>All entries</source>
        <translation>Toate intrările</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>Diagnostice</translation>
    </message>
    <message>
        <source>Repairs</source>
        <translation>Reparații</translation>
    </message>
    <message>
        <source>Package repair</source>
        <translation>Reparații pachete</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Copie fișier</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Descoperirea dispozitivului</translation>
    </message>
    <message>
        <source>Host</source>
        <translation>Gazdă</translation>
    </message>
    <message>
        <source>Required for block-device inventory</source>
        <translation>Necesar pentru inventarul dispozitivului de blocare</translation>
    </message>
    <message>
        <source>Filesystem identification</source>
        <translation>Identificarea sistemului de fișiere</translation>
    </message>
    <message>
        <source>Used to identify filesystem metadata</source>
        <translation>Utilizat pentru a identifica metadatele sistemului de fișiere</translation>
    </message>
    <message>
        <source>Mount inspection</source>
        <translation>Inspecție la montare</translation>
    </message>
    <message>
        <source>Used to understand active mounts</source>
        <translation>Folosit pentru a înțelege montari active</translation>
    </message>
    <message>
        <source>LUKS support</source>
        <translation>Suport LUKS</translation>
    </message>
    <message>
        <source>Required to unlock encrypted targets</source>
        <translation>Necesar pentru a debloca țintele criptate</translation>
    </message>
    <message>
        <source>Btrfs support</source>
        <translation>Suport Btrfs</translation>
    </message>
    <message>
        <source>Required for Btrfs inspection and snapshot rollback</source>
        <translation>Necesar pentru inspectia Btrfs si inregistrare instantanee</translation>
    </message>
    <message>
        <source>Bidirectional file copy</source>
        <translation>Copie de fișier bidirecțională</translation>
    </message>
    <message>
        <source>Host/Repair</source>
        <translation>Gazdă/Reparație</translation>
    </message>
    <message>
        <source>Required for verified Host-to-Repair and Repair-to-Host transfer</source>
        <translation>Necesar pentru transferul de gazda la reparatie si reparatie-la-spatiu verificat</translation>
    </message>
    <message>
        <source>Chroot repair</source>
        <translation>Reparații Chroot</translation>
    </message>
    <message>
        <source>Required for target-side repair commands</source>
        <translation>Necesar pentru comenzile de reparații la distanță</translation>
    </message>
    <message>
        <source>Offline systemd repair</source>
        <translation>Reparaţii de sistem offline</translation>
    </message>
    <message>
        <source>Used to restore graphical.target and the configured display manager without starting the target GUI</source>
        <translation>Folosit pentru a restabili grafic. țintă și managerul de afișare configurat fără a porni GUI țintă</translation>
    </message>
    <message>
        <source>UEFI NVRAM inspection</source>
        <translation>Inspecție UEFI NVRAM</translation>
    </message>
    <message>
        <source>Used to preserve target EFI BootOrder during TUXEDO UKI rebuilds when efivars are available</source>
        <translation>Utilizat pentru a păstra țintă EFI BootOrder în timpul TUXEDO UKI reconstruiește atunci când efivars sunt disponibile</translation>
    </message>
    <message>
        <source>UKI verification</source>
        <translation>Verificarea UKI</translation>
    </message>
    <message>
        <source>Used to verify the kernel embedded in a rebuilt unified kernel image</source>
        <translation>Folosit pentru a verifica nucleul încorporat într-o imagine de nucleu unificat reconstruit</translation>
    </message>
    <message>
        <source>GRUB EFI repair</source>
        <translation>Reparaţii GRUB EFI</translation>
    </message>
    <message>
        <source>Target/Host</source>
        <translation>Țintă/Host</translation>
    </message>
    <message>
        <source>Required only for conventional GRUB-based EFI systems</source>
        <translation>Necesar numai pentru sistemele convenţionale bazate pe GRUB EFI</translation>
    </message>
    <message>
        <source>Target</source>
        <translation>Țintă</translation>
    </message>
    <message>
        <source>Debian-family GRUB helper</source>
        <translation>Debian-familie GRUB ajutor</translation>
    </message>
    <message>
        <source>Portable GRUB configuration generator used by Arch and other non-Debian systems</source>
        <translation>Generator portabil de configurare GRUB utilizat de Arc și alte sisteme non-debiene</translation>
    </message>
    <message>
        <source>Initramfs rebuild</source>
        <translation>Initramfs reconstruiește</translation>
    </message>
    <message>
        <source>Debian-family initramfs helper</source>
        <translation>Debian-familie initramfs ajutor</translation>
    </message>
    <message>
        <source>Arch-family initramfs generator</source>
        <translation>Generator Arch-familie initramfs</translation>
    </message>
    <message>
        <source>Alternative initramfs generator used by Arch and other distributions</source>
        <translation>Generator alternativ initramfs utilizat de Arc și alte distribuții</translation>
    </message>
    <message>
        <source>Initramfs verification</source>
        <translation>Verificarea initramfs</translation>
    </message>
    <message>
        <source>Read-only verification for mkinitcpio images</source>
        <translation>Verificarea numai pentru citire a imaginilor mkinitcpio</translation>
    </message>
    <message>
        <source>Read-only verification for dracut images</source>
        <translation>Verificarea numai pentru citire a imaginilor dracut</translation>
    </message>
    <message>
        <source>systemd-boot inspection</source>
        <translation>Inspecția systemd-boot</translation>
    </message>
    <message>
        <source>Host/Target</source>
        <translation>Gazdă/Ţintă</translation>
    </message>
    <message>
        <source>Read-only inspection of systemd-boot and generic UKI layouts</source>
        <translation>Inspecția numai în citire a systemd-boot și a formatelor generice UKI</translation>
    </message>
    <message>
        <source>Arch package manager</source>
        <translation>Administrator pachet arc</translation>
    </message>
    <message>
        <source>Arch-family package database and transaction tool</source>
        <translation>Baza de date a pachetelor și a instrumentului de tranzacționare pentru copii de familie</translation>
    </message>
    <message>
        <source>DKMS rebuild</source>
        <translation>DKMS reconstruiește</translation>
    </message>
    <message>
        <source>Required only when target uses DKMS modules</source>
        <translation>Necesar numai atunci când obiectivul utilizează module DKMS</translation>
    </message>
    <message>
        <source>LVM inspection</source>
        <translation>Inspecția LVM</translation>
    </message>
    <message>
        <source>Optional LVM storage-stack support</source>
        <translation>Suport de stocare LVM opțional</translation>
    </message>
    <message>
        <source>Software RAID</source>
        <translation>Software RAID</translation>
    </message>
    <message>
        <source>Optional Linux MD RAID support</source>
        <translation>Suport opțional Linux MD RAID</translation>
    </message>
    <message>
        <source>Process namespace isolation</source>
        <translation>Izolarea namespațiului procesului</translation>
    </message>
    <message>
        <source>Host+Target</source>
        <translation>Gazda. Țintă</translation>
    </message>
    <message>
        <source>The ported helper falls back to a guarded plain chroot when unshare is absent</source>
        <translation>Ajutor portat cade înapoi la un chroot simplu păzit atunci când nu este partajat</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Renunță</translation>
    </message>
    <message>
        <source>OK</source>
        <translation>Bine.</translation>
    </message>
    <message>
        <source>Yes</source>
        <translation>Da.</translation>
    </message>
    <message>
        <source>No</source>
        <translation>Nu.</translation>
    </message>
    <message>
        <source>read-only helper diagnostic</source>
        <translation>diagnostic de ajutor numai pentru citire</translation>
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
        <translation>Utilitar de recuperare Linux și boot-reparație</translation>
    </message>
    <message>
        <source>GUARDED REPAIR</source>
        <translation>REPARATOR GARDED</translation>
    </message>
    <message>
        <source>Ordinary repairs require an explicitly selected non-host target. The protected running host has a separate deliberate maintenance mode with the same guarded repair stages and requires privilege authorization.</source>
        <translation>Reparaţiile obişnuite necesită o ţintă ne-gazdă selectată explicit. Gazda care ruleaza protejata are un mod separat de intretinere deliberata cu aceleasi etape de reparatii pazite si necesita autorizatie privilegiata.</translation>
    </message>
    <message>
        <source>Systems</source>
        <translation>Sisteme</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>Diagnostice</translation>
    </message>
    <message>
        <source>Repair</source>
        <translation>Reparații</translation>
    </message>
    <message>
        <source>Chroot Shell</source>
        <translation>Chroot Shell</translation>
    </message>
    <message>
        <source>File Copy</source>
        <translation>Copie fișier</translation>
    </message>
    <message>
        <source>Logs</source>
        <translation>Jurnale</translation>
    </message>
    <message>
        <source>Settings</source>
        <translation>Configurări</translation>
    </message>
    <message>
        <source>(not created yet)</source>
        <translation>(necreat încă)</translation>
    </message>
    <message>
        <source>Information</source>
        <translation>Informații</translation>
    </message>
    <message>
        <source>Close</source>
        <translation>Închide</translation>
    </message>
    <message>
        <source>&amp;File</source>
        <translation>&amp;Fișier</translation>
    </message>
    <message>
        <source>&amp;Refresh Devices</source>
        <translation>&amp;Dispozitive de reîmprospătare</translation>
    </message>
    <message>
        <source>&amp;Lock Administrator Session</source>
        <translation>&amp;Lock Session Administrator</translation>
    </message>
    <message>
        <source>&amp;Quit</source>
        <translation>&amp;renunţă</translation>
    </message>
    <message>
        <source>&amp;View</source>
        <translation>&amp;Vizualizare</translation>
    </message>
    <message>
        <source>&amp;Systems</source>
        <translation>&amp;Sisteme</translation>
    </message>
    <message>
        <source>&amp;Diagnostics</source>
        <translation>&amp;Diagnosticul</translation>
    </message>
    <message>
        <source>&amp;Logs</source>
        <translation>&amp;Jurnale</translation>
    </message>
    <message>
        <source>&amp;Settings</source>
        <translation>&amp;Configurări</translation>
    </message>
    <message>
        <source>&amp;Auto-size Device Columns</source>
        <translation>&amp;Coloană de dispozitiv de mărime automată</translation>
    </message>
    <message>
        <source>&amp;Wrap Log Lines</source>
        <translation>&amp;Wrap Log Lines</translation>
    </message>
    <message>
        <source>&amp;Help</source>
        <translation>&amp;Ajutor</translation>
    </message>
    <message>
        <source>&amp;Using Boot Bitch</source>
        <translation>&amp;Folosind Boot Bitch</translation>
    </message>
    <message>
        <source>&amp;About Boot Bitch</source>
        <translation>&amp;Despre Boot Bitch</translation>
    </message>
    <message>
        <source>Device columns auto-sized. Drag headers to fine-tune widths.</source>
        <translation>Coloane de dispozitiv de dimensiuni auto. Târăşte capetele la lăţimea fină.</translation>
    </message>
    <message>
        <source>A helper command is running; wait for it to finish before locking the session.</source>
        <translation>O comandă de ajutor rulează; așteptați pentru a termina înainte de blocare sesiunea.</translation>
    </message>
    <message>
        <source>Administrator session locked; the next privileged action will request authorization.</source>
        <translation>Sesiunea de administrator blocată; următoarea acțiune privilegiată va solicita autorizare.</translation>
    </message>
    <message>
        <source>Using Boot Bitch</source>
        <translation>Folosind Boot Bitch</translation>
    </message>
    <message>
        <source>Boot Bitch must run from a different booted Linux environment than the system being repaired. Use a Linux live medium or another Linux installation on a different physical drive.&lt;br&gt;&lt;br&gt;The running host is protected from ordinary repair-target selection, but it can be explicitly selected through &lt;b&gt;Host Maintenance&lt;/b&gt; for guarded native diagnostics and supported maintenance stages.&lt;br&gt;&lt;br&gt;Diagnostics follow the Systems page: the committed repair drive while Host Maintenance is off, or the protected running host while it is active.&lt;br&gt;&lt;br&gt;The first privileged action requests administrator authorization once for this Boot Bitch window; &lt;b&gt;File - Lock Administrator Session&lt;/b&gt; ends that helper session immediately. Every repair keeps the helper&apos;s own runtime preflights.</source>
        <translation>Boot Bitch trebuie să ruleze dintr-un mediu Linux diferit de sistemul reparat. Utilizați un mediu Linux live sau un alt sistem Linux pe o unitate fizică diferită. Gazda care rulează este protejată împotriva selecţiei obişnuite de reparaţii-ţintă, dar poate fi selectată explicit prin &lt;b&gt;Host Mentenanţă&lt;/b&gt; pentru diagnosticarea nativă păzită şi etapele de întreţinere susţinute. Diagnosticul urmează pagina Systems: unitatea de reparații comisă în timp ce Host Maintenance este oprit sau gazda de funcționare protejată în timp ce este activă. Prima acțiune privilegiată solicită autorizarea administratorului o dată pentru această fereastră Boot Bitch; &lt;b&gt;File - Lock Administrator Session&lt;/b&gt; se încheie imediat sesiunea de ajutor. Fiecare reparaţie păstrează propriile precursoare.</translation>
    </message>
    <message>
        <source>Select a physical drive; Boot Bitch resolves the most likely Linux system volume automatically. The running host stays protected from ordinary target repairs, with a separate explicit host-maintenance path for its own system. The Details button shows the protected host&apos;s facts in the details pane; selecting any drive row restores the per-drive pane.</source>
        <translation>Selectaţi o unitate fizică; Boot Bitch rezolvă cel mai probabil volumul sistemului Linux automat. Gazda care ruleaza ramane protejata de reparatiile tinta obisnuite, cu o cale separata explicita de intretinere a gazdelor pentru propriul sistem. Butonul Details arată faptele gazdei protejate în panoul de detalii; selectarea oricărui rând de comandă restabilește panoul per-drive.</translation>
    </message>
    <message>
        <source>Refresh Devices</source>
        <translation>Reîmprospătează dispozitivele</translation>
    </message>
    <message>
        <source>Re-read the read-only kernel inventory (/proc/partitions, /proc/mounts, /proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* and the udev metadata database). No block device is opened and nothing is written.</source>
        <translation>Re-citește inventarul nucleului numai pentru citire (/proc/partiții, /proc/mounts, /proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* și baza de date a metadatelor udev). Nici un dispozitiv de blocare este deschis și nimic nu este scris.</translation>
    </message>
    <message>
        <source>[OK]</source>
        <translation>Bine.</translation>
    </message>
    <message>
        <source>The running system was detected and stays protected from ordinary target repairs.</source>
        <translation>Sistemul de funcţionare a fost detectat şi rămâne protejat de reparaţiile obişnuite ale ţintelor.</translation>
    </message>
    <message>
        <source>Detecting running system...</source>
        <translation>Detectez sistemul de rulare...</translation>
    </message>
    <message>
        <source>Detecting protected storage...</source>
        <translation>Detectez depozitul protejat...</translation>
    </message>
    <message>
        <source>PROTECTED</source>
        <translation>PROTEJAT</translation>
    </message>
    <message>
        <source>The running host remains protected from ordinary repair-target operations.</source>
        <translation>Gazda care rulează rămâne protejată împotriva operațiunilor obișnuite de reparații-țintă.</translation>
    </message>
    <message>
        <source>Details</source>
        <translation>Detalii</translation>
    </message>
    <message>
        <source>Show read-only details for the protected running host.</source>
        <translation>Arată detalii doar pentru gazda care rulează protejată.</translation>
    </message>
    <message>
        <source>Host Maintenance</source>
        <translation>Întreţinere gazdă</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Implicit</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host (menu.lst default directive with a backup and rollback). Requires Host Maintenance and the cached host-default probe.</source>
        <translation>Asigurați intrarea kernel-ului instalat canonic intrarea implicită GRUB-legacy de boot pe gazda care rulează (directiva implicită menu.lst cu o copie de rezervă și rollback). Necesită Host Maintenance și sonda gazdă cache-default.</translation>
    </message>
    <message>
        <source>Available repair targets</source>
        <translation>Obiective de reparații disponibile</translation>
    </message>
    <message>
        <source>Most likely first</source>
        <translation>Cel mai probabil primul</translation>
    </message>
    <message>
        <source>Drives are listed by the read-only inventory. Select a row to inspect it; Select Target commits the selected non-host drive with its auto-resolved Linux root component.</source>
        <translation>Drive-urile sunt enumerate de inventarul numai pentru citire. Selectaţi un rând pentru a-l inspecta; Selectaţi Target se angajează unitatea aleasă non-host cu componenta sa Linux rădăcină auto-rezolvată.</translation>
    </message>
    <message>
        <source>Device</source>
        <translation>Dispozitiv</translation>
    </message>
    <message>
        <source>Size</source>
        <translation>Dimensiune</translation>
    </message>
    <message>
        <source>Type</source>
        <translation>Tip</translation>
    </message>
    <message>
        <source>Filesystem</source>
        <translation>Sistem de fișiere</translation>
    </message>
    <message>
        <source>Select Target</source>
        <translation>Alegeți ținta</translation>
    </message>
    <message>
        <source>Unlock</source>
        <translation>Deblochează</translation>
    </message>
    <message>
        <source>Authorize</source>
        <translation>Autorizare</translation>
    </message>
    <message>
        <source>Establish the privileged helper session for the current scope now instead of waiting for the next privileged action.</source>
        <translation>Înființarea sesiunii de ajutor privilegiat pentru domeniul de aplicare actual acum în loc de așteptare pentru următoarea acțiune privilegiată.</translation>
    </message>
    <message>
        <source>Committed target: none</source>
        <translation>Țintă angajată: niciuna</translation>
    </message>
    <message>
        <source>Unlock state for the selected drive; the LUKS passphrase is never logged.</source>
        <translation>Deblochează starea pentru unitatea selectată; fraza de acces LUKS nu este niciodată autentificată.</translation>
    </message>
    <message>
        <source>Unlock status</source>
        <translation>Deblochează starea</translation>
    </message>
    <message>
        <source>Read-only inventory plus helper-confirmed facts; mirrors the modern Qt6 Selected drive details panel.</source>
        <translation>Inventar numai-read plus fapte confirmate de ajutor; oglindește panoul modern Qt6 Detalii de unitate selectate.</translation>
    </message>
    <message>
        <source>Selected drive details</source>
        <translation>Detalii de unitate selectate</translation>
    </message>
    <message>
        <source>Field</source>
        <translation>Câmp</translation>
    </message>
    <message>
        <source>Value</source>
        <translation>Valoare</translation>
    </message>
    <message>
        <source>Run All runs every available read-only diagnostic for the current scope; selecting a check runs it alone. Diagnostics are read-only and are the only evidence source for the gated repair actions.</source>
        <translation>Rulați Toate ruleaza fiecare diagnostic disponibil numai citire pentru domeniul de aplicare curent; selectarea unui cec ruleaza singur. Diagnosticele sunt doar citite și sunt singura sursă de dovezi pentru acțiunile de reparații portate.</translation>
    </message>
    <message>
        <source>Target: none selected</source>
        <translation>Țintă: niciunul selectat</translation>
    </message>
    <message>
        <source>Diagnostics follow the committed repair target, or the protected running host while Host Maintenance is active.</source>
        <translation>Diagnosticul urmează ținta de reparații comise, sau gazda care rulează protejat în timp ce Host Maintenance este activ.</translation>
    </message>
    <message>
        <source>Run All</source>
        <translation>Rulează toate</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope.</source>
        <translation>Rulați toate - executați fiecare diagnostic disponibil numai citire pentru domeniul de aplicare curent.</translation>
    </message>
    <message>
        <source>Target configuration:</source>
        <translation>Configurare țintă:</translation>
    </message>
    <message>
        <source>Etch-era target configuration files; availability is probed read-only by the helper&apos;s diagnostics.</source>
        <translation>Etch-era fișiere de configurare țintă; disponibilitatea este cercetată-doar prin diagnosticarea ajutorului.</translation>
    </message>
    <message>
        <source>Edit Target File...</source>
        <translation>Editează fișierul țintă...</translation>
    </message>
    <message>
        <source>Runs one read-only diagnostic for the selected scope through the helper (`diagnose &lt;key&gt;` / `host-diagnose &lt;key&gt;`); Run All is the combined report.</source>
        <translation>Rulează un diagnostic numai-citire pentru domeniul de aplicare selectat prin ajutorul (</translation>
    </message>
    <message>
        <source>Diagnostic checks</source>
        <translation>Controale diagnostice</translation>
    </message>
    <message>
        <source>Check</source>
        <translation>Verificat</translation>
    </message>
    <message>
        <source>Selected diagnostic</source>
        <translation>Diagnostic ales</translation>
    </message>
    <message>
        <source>Select a diagnostic</source>
        <translation>Alegeți un diagnostic</translation>
    </message>
    <message>
        <source>Choose a diagnostic from the list.</source>
        <translation>Alege un diagnostic din listă.</translation>
    </message>
    <message>
        <source>Ready</source>
        <translation>Gata.</translation>
    </message>
    <message>
        <source>Results</source>
        <translation>Rezultate</translation>
    </message>
    <message>
        <source>Run Diagnostic</source>
        <translation>Rulează diagnosticul</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic for the current scope.</source>
        <translation>Rulați Diagnostic - executați diagnosticul selectat numai pentru citire pentru domeniul de aplicare curent.</translation>
    </message>
    <message>
        <source>Copy Results</source>
        <translation>Copiază rezultatele</translation>
    </message>
    <message>
        <source>Save Results...</source>
        <translation>Salvează rezultatele...</translation>
    </message>
    <message>
        <source>The Full Repair plan runs the selected legacy stages in order through the guarded helper; the individual tools run one stage at a time. Every action stays disabled until the cached capability lines say available and the helper keeps its runtime preflights.</source>
        <translation>Planul de reparații complete rulează etapele moștenite selectate în ordine prin ajutorul păzit; instrumentele individuale rulează o singură etapă la un moment dat. Fiecare acţiune rămâne dezactivată până când liniile de capacitate cache spun că este disponibilă şi ajutorul îşi păstrează precursoarele.</translation>
    </message>
    <message>
        <source>Full Repair plan</source>
        <translation>Plan complet de reparații</translation>
    </message>
    <message>
        <source>No stages selected</source>
        <translation>Nicio etapă selectată</translation>
    </message>
    <message>
        <source>Configure Plan...</source>
        <translation>Configurează planul...</translation>
    </message>
    <message>
        <source>Open Settings to choose which Full Repair stages are part of the plan.</source>
        <translation>Setări deschise pentru a alege care etape de reparare completă fac parte din plan.</translation>
    </message>
    <message>
        <source>Run Full Repair</source>
        <translation>Execută reparații complete</translation>
    </message>
    <message>
        <source>Select a repair drive, or choose Host Maintenance on the protected running-host card.</source>
        <translation>Selectaţi o unitate de reparaţii, sau alegeţi Hoste Întreţinere pe cardul protejat de funcţionare-host.</translation>
    </message>
    <message>
        <source>Stage</source>
        <translation>Etapa</translation>
    </message>
    <message>
        <source>Individual repair tools</source>
        <translation>Unelte de reparații individuale</translation>
    </message>
    <message>
        <source>Tool</source>
        <translation>Instrument</translation>
    </message>
    <message>
        <source>Full Repair</source>
        <translation>Reparație completă</translation>
    </message>
    <message>
        <source>not reported</source>
        <translation>neraportat</translation>
    </message>
    <message>
        <source>Selected tool</source>
        <translation>Utilitar selectat</translation>
    </message>
    <message>
        <source>Select a repair tool</source>
        <translation>Alegeți un instrument de reparații</translation>
    </message>
    <message>
        <source>Run Tool</source>
        <translation>Comment</translation>
    </message>
    <message>
        <source>Select a tool to review its repair action.</source>
        <translation>Selectaţi un instrument pentru a revizui acţiunea sa de reparare.</translation>
    </message>
    <message>
        <source>Write actions ask for confirmation and then run the helper&apos;s own runtime preflights; the GUI never weakens them. A repair that is not proven &apos;unchanged&apos; invalidates the cached diagnostics and disables the gated actions until diagnostics run again.</source>
        <translation>Scrie acțiunile cere confirmarea și apoi executați propriile precursoare ale ajutorului; GUI nu le slăbește niciodată. O reparaţie care nu este dovedită &quot;neschimbată&quot; invalidează diagnosticul cache şi dezactivează acţiunile poartă până la diagnosticarea rula din nou.</translation>
    </message>
    <message>
        <source>Chroot shell</source>
        <translation>coajă comestibilă</translation>
    </message>
    <message>
        <source>Offline commands run one at a time in a fresh chroot and cannot answer interactive prompts (apt-get -y upgrade works). Host-shell commands run directly on the running host. The helper&apos;s probe lines gate the command field; the exact reason appears in the tooltip.</source>
        <translation>Comenzi offline rula unul la un moment dat într-o prospețime chroot și nu poate răspunde prompte interactive (apt-get-y upgrade works). Comenzile Hoste-shell rulează direct pe gazda care rulează. Liniile sondei ajutătorului poartă câmpul de comandă; motivul exact apare în vârful uneltei.</translation>
    </message>
    <message>
        <source>No &apos;Legacy feature shell:&apos; line is cached; run diagnostics for the selected scope to evaluate the helper&apos;s chroot/timeout containment probes (fail closed).</source>
        <translation>Nici o &quot;cochilie caracteristică Legacy:&quot; linia este cached; rulați diagnostice pentru domeniul de aplicare selectat pentru a evalua chroot ajutor / timeout sonde de izolare (inchise fail).</translation>
    </message>
    <message>
        <source>Command</source>
        <translation>Comandă</translation>
    </message>
    <message>
        <source>Command:</source>
        <translation>Comandă:</translation>
    </message>
    <message>
        <source>One reviewed command string, passed to the helper as a single argument (no shell interpolation by the GUI).</source>
        <translation>Un șir de comandă revizuit, a trecut la ajutor ca un singur argument (nici o interpolare coajă de către GUI).</translation>
    </message>
    <message>
        <source>Run Command</source>
        <translation>Execută comanda</translation>
    </message>
    <message>
        <source>Clear Output</source>
        <translation>Șterge ieșirea</translation>
    </message>
    <message>
        <source>The helper exposes `shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for an offline target chroot and `host-shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for the running host. Both keep the helper&apos;s runtime preflights; this tab enables the command only when the scope is committed, the session is authorized and the scope&apos;s Legacy feature probe reports available.</source>
        <translation>helperul expune shell &lt;disk&gt; &lt;root&gt; &lt;count&gt; &lt;count&gt; Ambele păstrează pre-zborurile pe timp de rulare ale ajutorului; această filă permite comanda numai atunci când domeniul de aplicare este comis, sesiunea este autorizată și rapoartele de caracteristică Legacy disponibile.</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Copie fișier</translation>
    </message>
    <message>
        <source>Copy and verify files in either direction through the guarded helper (cp -a plus ownership restoration and a per-file byte-compare). The helper&apos;s file-copy probe gates the controls and keeps the direction and path containment checks.</source>
        <translation>Copiați și verificați fișierele în orice direcție prin ajutorul păzit (cp -a plus restaurarea proprietății și un octet-compare per fișier). Sonda de copiere a fişierului ajutătorului poartă comenzile şi păstrează controalele de izolare a direcţiei şi traseului.</translation>
    </message>
    <message>
        <source>Preview Changes</source>
        <translation>Modificări de previzualizare</translation>
    </message>
    <message>
        <source>Run a copy dry-run through the guarded helper. No files are changed.</source>
        <translation>Verifică o copie prin ajutorul păzit. Niciun dosar nu este schimbat.</translation>
    </message>
    <message>
        <source>Copy and Verify</source>
        <translation>Copiază și verifică</translation>
    </message>
    <message>
        <source>Copy staged items and verify the result. Existing destination names are overwritten when source content differs; unrelated destination files are never deleted.</source>
        <translation>Copiați elementele înscenate și verificați rezultatul. Numele de destinație existente sunt suprascrise atunci când conținutul sursei diferă; fișierele de destinație care nu au legătură nu sunt niciodată șterse.</translation>
    </message>
    <message>
        <source>Direction:</source>
        <translation>Direcţie:</translation>
    </message>
    <message>
        <source>Choose which system supplies the source files and which system receives them.</source>
        <translation>Alege ce sistem furnizează fișierele sursă și ce sistem le primește.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from this host</source>
        <translation>1. Selectaţi fişiere sursă sau foldere din această gazdă</translation>
    </message>
    <message>
        <source>Source</source>
        <translation>Sursă</translation>
    </message>
    <message>
        <source>Files and folders staged for the verified copy. The legacy backend copies with cp -a and restores ownership with chown --reference; every regular file is byte-compared after the copy.</source>
        <translation>Fișiere și dosare puse în scenă pentru copia verificată. Moștenirea copii backend cu cp -a și restabilește proprietatea cu crown -- referință; fiecare fișier obișnuit este octet-comparativ cu copia.</translation>
    </message>
    <message>
        <source>Add Files...</source>
        <translation>Adaugă fișiere...</translation>
    </message>
    <message>
        <source>Add Folder...</source>
        <translation>Adaugă dosar...</translation>
    </message>
    <message>
        <source>Remove</source>
        <translation>Elimină</translation>
    </message>
    <message>
        <source>Clear</source>
        <translation>Clar</translation>
    </message>
    <message>
        <source>Clear the staged source list (nothing is copied or deleted).</source>
        <translation>Ștergeți lista de surse înscenată (nimic nu este copiat sau șters).</translation>
    </message>
    <message>
        <source>2. Choose destination in repaired system</source>
        <translation>2. Alegeţi destinaţia în sistem reparat</translation>
    </message>
    <message>
        <source>unavailable: see the helper&apos;s Legacy feature file-copy: probe reason above</source>
        <translation>indisponibil: a se vedea fişierul-copie Legacy de ajutor: sonda motiv de mai sus</translation>
    </message>
    <message>
        <source>An absolute path inside the selected repair system (Host to Repair) or on the running host (Repair to Host).</source>
        <translation>O cale absolută în interiorul sistemului de reparații selectat (Host to Repair) sau pe gazda care rulează (Reparare la gazda).</translation>
    </message>
    <message>
        <source>Browse Target Folders...</source>
        <translation>Navighează dosarele țintă...</translation>
    </message>
    <message>
        <source>Browse the selected repair system through the helper&apos;s temporary read-only mounts and choose an absolute destination path. No target files are changed while browsing.</source>
        <translation>Răsfoiţi sistemul de reparaţii selectat prin montări temporare numai-citite de ajutor şi alegeţi o cale de destinaţie absolută. Nu sunt schimbate fișiere țintă în timpul navigării.</translation>
    </message>
    <message>
        <source>3. Ownership and copy policy</source>
        <translation>3. Proprietatea și politica de copiere</translation>
    </message>
    <message>
        <source>Ownership:</source>
        <translation>Proprietate:</translation>
    </message>
    <message>
        <source>Smart destination ownership (recommended)</source>
        <translation>Proprietate inteligentă de destinație (recomandată)</translation>
    </message>
    <message>
        <source>Preserve source numeric UID/GID</source>
        <translation>Păstrați sursa numeric UID/GID</translation>
    </message>
    <message>
        <source>Smart mode validates UID/GID identity mapping across the two systems and falls back to the destination-directory owner when the same numeric ID means a different account (the legacy backend implements it with chown --reference).</source>
        <translation>Modul inteligent validează cartografierea identităţii UID/IGD în cadrul celor două sisteme şi revine proprietarului depozitarului destinaţiei atunci când acelaşi ID numeric înseamnă un cont diferit (backend-ul moștenit îl pune în aplicare cu referinţă --).</translation>
    </message>
    <message>
        <source>Application log</source>
        <translation>Jurnal de aplicare</translation>
    </message>
    <message>
        <source>The complete session register; Save As... writes every entry even while a filter hides lines. If a writable system share mount exists at /host, Save As... starts there; otherwise the log directory is the fallback. Prior session files are listed read-only.</source>
        <translation>Registrul complet al sesiunii; Salvare ca... scrie fiecare intrare chiar și în timp ce un filtru ascunde linii. În cazul în care există un montaj de partajare a sistemului scris la /host, Save As... începe acolo; în caz contrar, directorul jurnal este rezerva. Fișierele de sesiune anterioare sunt enumerate numai citite.</translation>
    </message>
    <message>
        <source>Session logs</source>
        <translation>Jurnalele sesiunii</translation>
    </message>
    <message>
        <source>Session</source>
        <translation>Sesiune</translation>
    </message>
    <message>
        <source>The first entry is the live session; earlier files in the log directory are listed read-only below it.</source>
        <translation>Prima intrare este sesiunea live; fișierele anterioare din directorul jurnalului sunt enumerate mai jos.</translation>
    </message>
    <message>
        <source>New Session Log</source>
        <translation>Jurnal sesiune nouă</translation>
    </message>
    <message>
        <source>Close the active session file; it becomes a prior session and the next log entry starts a new file.</source>
        <translation>Închide fișierul sesiune activă; devine o sesiune anterioară și următoarea înregistrare jurnal începe un fișier nou.</translation>
    </message>
    <message>
        <source>Add Note</source>
        <translation>Adaugă notă</translation>
    </message>
    <message>
        <source>Append a NOTE entry to the live session register.</source>
        <translation>Adaugă o NOTĂ în registrul sesiunii live.</translation>
    </message>
    <message>
        <source>Delete</source>
        <translation>Șterge</translation>
    </message>
    <message>
        <source>Delete the selected prior session file (the live session is never deleted).</source>
        <translation>Șterge fișierul de sesiune anterior selectat (sesiunea live nu este niciodată ștersă).</translation>
    </message>
    <message>
        <source>Refresh</source>
        <translation>Reîmprospătează</translation>
    </message>
    <message>
        <source>Save As...</source>
        <translation>Salvează ca...</translation>
    </message>
    <message>
        <source>Save the complete session log (all entries, not just the current filter).</source>
        <translation>Salvează jurnalul complet al sesiunii (toate intrările, nu doar filtrul curent).</translation>
    </message>
    <message>
        <source>Clear Register</source>
        <translation>Curăță registrul</translation>
    </message>
    <message>
        <source>Clear the live register and view; prior session files are never modified.</source>
        <translation>Curățați registrul și vizualizarea în direct; fișierele sesiunii anterioare nu sunt niciodată modificate.</translation>
    </message>
    <message>
        <source>Search log:</source>
        <translation>Jurnal de căutare:</translation>
    </message>
    <message>
        <source>Type any characters to show matching log entries (case-insensitive). Save As always writes every entry.</source>
        <translation>Tastați orice caractere pentru a afișa înregistrări de jurnal de potrivire (caz insensibil). Salvează Ca întotdeauna scrie fiecare intrare.</translation>
    </message>
    <message>
        <source>Filter:</source>
        <translation>Filtru:</translation>
    </message>
    <message>
        <source>Filter the visible log by entry kind. Selecting a diagnostic section shows the lines captured for that section; a workflow filter such as File system repair or Package repair shows its mapped repair lines (File copy has no lines in this frontend). Save As always writes every entry.</source>
        <translation>Filtrați jurnalul vizibil prin tipul de intrare. Selectarea unei secțiuni de diagnosticare arată liniile capturate pentru acea secțiune; un filtru de flux de lucru, cum ar fi repararea sistemului de fișiere sau repararea pachetelor arată liniile sale de reparații cartografiate (Copia de file nu are linii în acest frontend). Salvează Ca întotdeauna scrie fiecare intrare.</translation>
    </message>
    <message>
        <source>Settings are stored per user under ~/.qt/, one file per settings group (devicesrc, logsrc, diagnosticsrc, repairrc), and are saved immediately on every change and on close. Launch the GUI as the same user to keep your overrides; a GUI started as root keeps its own copies.</source>
        <translation>Configurările sunt stocate pentru fiecare utilizator sub ~/.qt/, un fișier pentru fiecare grup de setări (dispozitivsrc, logsrc, diagnosticsrc, reparator), și sunt salvate imediat pe fiecare schimbare și pe aproape. Lansați GUI ca același utilizator pentru a păstra suprascrie; un GUI a început ca rădăcină păstrează propriile copii.</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Descoperirea dispozitivului</translation>
    </message>
    <message>
        <source>Show devices without an identified Linux installation</source>
        <translation>Afișează dispozitivele fără instalare Linux identificată</translation>
    </message>
    <message>
        <source>Show removable and USB storage</source>
        <translation>Afișează stocarea detașabilă și USB</translation>
    </message>
    <message>
        <source>Show encrypted devices before unlocking</source>
        <translation>Afișează dispozitivele criptate înainte de deblocare</translation>
    </message>
    <message>
        <source>When off, drives without a visible Linux filesystem are hidden unless they still contain an encrypted device and encrypted devices are shown.</source>
        <translation>Când sunt oprite, drive-urile fără un sistem de fișiere Linux vizibil sunt ascunse dacă nu conțin încă un dispozitiv criptat și sunt afișate dispozitive criptate.</translation>
    </message>
    <message>
        <source>When off, removable and USB drives are hidden from the repair-target list.</source>
        <translation>Când sunt oprite, motoarele detaşabile şi USB sunt ascunse de lista de reparaţii-ţintă.</translation>
    </message>
    <message>
        <source>When off, drives with an encrypted device are hidden until the volume is unlocked.</source>
        <translation>Când este oprit, motoarele cu un dispozitiv criptat sunt ascunse până când volumul este deblocat.</translation>
    </message>
    <message>
        <source>Include the %1 stage in the Full Repair plan. The stage runs in the plan order shown on the Repair tab.</source>
        <translation>Include etapa %1 în planul de reparații complete. Scena ruleaza in ordinea planului prezentata in fila Repair.</translation>
    </message>
    <message>
        <source>Automatically regenerate read-only diagnostics after repairs or target changes</source>
        <translation>Regenerați automat diagnosticele de citire numai după reparații sau modificări țintă</translation>
    </message>
    <message>
        <source>Regenerates the cached read-only diagnostics for the current scope after an operation that invalidates them (LUKS unlock, target configuration edit). It runs only inside an already authorized administrator session and never opens an authorization prompt by itself.</source>
        <translation>regenerează diagnosticele de citire doar prin cache pentru domeniul de aplicare curent după o operațiune care le invalidează (LUKS deblocare, editarea configurației țintei). Se execută doar într-o sesiune de administrator deja autorizat și nu deschide o autorizație promptă de la sine.</translation>
    </message>
    <message>
        <source>Wrap long log lines</source>
        <translation>Învelește linii lungi de jurnal</translation>
    </message>
    <message>
        <source>Mandatory safety controls</source>
        <translation>Controale obligatorii privind siguranța</translation>
    </message>
    <message>
        <source>Host capabilities and dependencies</source>
        <translation>Capacități-gazdă și dependențe</translation>
    </message>
    <message>
        <source>Refresh Capabilities</source>
        <translation>Actualizează capacitățile</translation>
    </message>
    <message>
        <source>Re-run the read-only host capability probes (a PATH search, nothing is executed) and refresh the distribution summary.</source>
        <translation>Re-rulați sondele de capacitate numai pentru gazde (o căutare PATH, nimic nu este executat) și reîmprospătați rezumatul distribuției.</translation>
    </message>
    <message>
        <source>Install Missing Support...</source>
        <translation>Instalează suport lipsă...</translation>
    </message>
    <message>
        <source>Automatic installation will require explicit package mapping and privilege authorization.</source>
        <translation>Instalarea automată va necesita cartografierea explicită a pachetelor și autorizarea privilegiilor.</translation>
    </message>
    <message>
        <source>Application configuration</source>
        <translation>Configurare aplicație</translation>
    </message>
    <message>
        <source>Boot Bitch Legacy</source>
        <translation>Boot Bitch Legacy</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list first.</source>
        <translation>Selectați o unitate fizică în lista țintelor de reparații disponibile mai întâi.</translation>
    </message>
    <message>
        <source>Protected system</source>
        <translation>Sistem protejat</translation>
    </message>
    <message>
        <source>The running system cannot be selected as a repair target. Use Host Maintenance for the protected running host or choose another disk.</source>
        <translation>Sistemul de funcționare nu poate fi selectat ca țintă de reparații. Utilizați Host Întreținere pentru gazda care rulează protejat sau alege un alt disc.</translation>
    </message>
    <message>
        <source>Unlock or select a Linux system first</source>
        <translation>Deblochează sau selectează mai întâi un sistem Linux</translation>
    </message>
    <message>
        <source>This encrypted drive has no visible Linux filesystem yet. Use Unlock, refresh devices, and select the target after its Linux root is detected.</source>
        <translation>Această unitate criptată nu are încă un sistem de fișiere Linux vizibil. Utilizați Deblocare, dispozitive de reîmprospătare și selectați ținta după detectarea rădăcinii Linux.</translation>
    </message>
    <message>
        <source>The selected root component (%1) belongs to the running system and cannot be committed as a repair target. Use Host Maintenance for the protected running host.</source>
        <translation>Componenta rădăcină selectată (%1) aparține sistemului de funcționare și nu poate fi angajată ca țintă de reparații. Utilizați Host Întreținere pentru gazda care rulează protejat.</translation>
    </message>
    <message>
        <source>repair target commit</source>
        <translation>comiterea obiectivului de reparare</translation>
    </message>
    <message>
        <source>Repair drive selected: %1</source>
        <translation>Unitate de reparații selectată: %1</translation>
    </message>
    <message>
        <source>; best detected system component: %1</source>
        <translation>; cea mai bună componentă a sistemului detectat: %1</translation>
    </message>
    <message>
        <source>. No mount or repair action was performed.</source>
        <translation>Nu s-a efectuat nicio acţiune de montare sau reparaţie.</translation>
    </message>
    <message>
        <source>Make Default unavailable</source>
        <translation>Inventează implicit</translation>
    </message>
    <message>
        <source>Make Default is a running-host action on this frontend; enter Host Maintenance first.</source>
        <translation>Make Implicit este o acțiune running-host pe acest frontend; introduceți mai întâi Host Maintenance.</translation>
    </message>
    <message>
        <source>Administrator authorization required</source>
        <translation>Autorizaţie de administrare necesară</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize first.</source>
        <translation>Nici o sesiune de administrator este activ; apăsați Autorizați mai întâi.</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host?

The helper verifies /boot/grub/menu.lst, sets the `default &lt;N&gt;` directive to the canonical entry, backs the menu up first and restores it on any failure.</source>
        <translation>Face intrarea kernelului instalat canonic intrarea implicită GRUB-legacy pe gazda care rulează?

Ajutorul verifică /boot/grub/meniu.lst, stabileşte directiva de intrare canonică, susţine meniul şi îl restabileşte pe orice eşec.</translation>
    </message>
    <message>
        <source>Host Maintenance unavailable</source>
        <translation>Întreținere gazdă indisponibilă</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a committed repair target or a detected running host.</source>
        <translation>Ţinta gazdă care rulează nu a putut fi detectată; diagnosticele necesită o ţintă de reparaţie sau o gazdă de funcţionare detectată.</translation>
    </message>
    <message>
        <source>Exit Host Maintenance</source>
        <translation>Exit Host Întreținere</translation>
    </message>
    <message>
        <source>Host Maintenance is active; diagnostics and gated repairs target the protected running host.</source>
        <translation>Host Întreținere este activ; diagnostice și reparații cu poarta țintiți gazda care rulează protejat.</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list on the Systems tab first, or use Host Maintenance for the protected running host.</source>
        <translation>Selectaţi o unitate fizică în lista de ţinte de reparaţii disponibile pe fila Systems primul, sau utilizaţi Host Întreţinere pentru gazda care rulează protejate.</translation>
    </message>
    <message>
        <source>The selected drive (%1) is the protected running host. Choose Host Maintenance on the Systems tab to run read-only host diagnostics and guarded host repairs; ordinary target repairs stay disabled.</source>
        <translation>Drive-ul selectat (%1) este gazda care rulează protejată. Alegeți Host Maintenance pe tab-ul Systems pentru a rula doar de citire a diagnosticului gazdelor și reparațiile gazdelor păzite; reparațiile țintă obișnuite rămân dezactivate.</translation>
    </message>
    <message>
        <source>No repair target is committed. Choose Select Target on the Systems tab (or Host Maintenance for the protected running host) first.</source>
        <translation>Nicio ţintă de reparaţie nu este angajată. Alegeți Selectați ținta pe tab-ul Systems (sau Host Mentenanță pentru gazda protejată care rulează).</translation>
    </message>
    <message>
        <source>The selection changed after the target was committed. Choose Select Target again.</source>
        <translation>Selecţia s-a schimbat după ce ţinta a fost angajată. Alegeţi din nou ţinta.</translation>
    </message>
    <message>
        <source>Diagnostics scope required</source>
        <translation>Domeniul de aplicare al diagnosticului necesar</translation>
    </message>
    <message>
        <source>Diagnostics scope unresolved</source>
        <translation>Domeniul de aplicare al diagnosticului nerezolvat</translation>
    </message>
    <message>
        <source>The running host target could not be resolved; use Refresh Devices and commit a repair target or re-enter Host Maintenance.</source>
        <translation>Ţinta gazdă care rulează nu a putut fi rezolvată; utilizaţi Dispozitive Refresh şi angajaţi o ţintă de reparaţie sau reintraţi în întreţinerea gazdelor.</translation>
    </message>
    <message>
        <source>Diagnostic check required</source>
        <translation>Verificarea diagnostică necesară</translation>
    </message>
    <message>
        <source>Select a diagnostic check in the list first.</source>
        <translation>Selectaţi mai întâi o verificare a diagnosticului în listă.</translation>
    </message>
    <message>
        <source>Repair target required</source>
        <translation>Ţinta de reparare necesară</translation>
    </message>
    <message>
        <source>Target file editing needs a committed repair target. Running-host maintenance has no target-file editing; commit an offline target first.</source>
        <translation>Editarea fișierului țintă are nevoie de o țintă de reparații angajate. Întreținerea Running-host nu are nici o editare fișier țintă; comite o țintă offline mai întâi.</translation>
    </message>
    <message>
        <source>Configuration file required</source>
        <translation>Fișier de configurare necesar</translation>
    </message>
    <message>
        <source>Select a target configuration file first.</source>
        <translation>Alegeți mai întâi un fișier de configurare țintă.</translation>
    </message>
    <message>
        <source>Edit target %1</source>
        <translation>Editează ținta %1</translation>
    </message>
    <message>
        <source>Edit this target file through the guarded administrator helper. A successful save invalidates cached diagnostics; rerun diagnostics before repair. Generated files such as /boot/grub/menu.lst may be replaced by the next bootloader update.</source>
        <translation>Editează acest fișier țintă prin ajutorul administratorului păzit. O salvare reușită invalidează diagnosticarea cache; rerulați diagnostice înainte de reparații. Fișierele generate, cum ar fi /boot/grub/meniu.st pot fi înlocuite cu următoarea actualizare a bootloader-ului.</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Renunță</translation>
    </message>
    <message>
        <source>Save Target File</source>
        <translation>Salvează fișierul țintă</translation>
    </message>
    <message>
        <source>No changes to %1.</source>
        <translation>Nicio modificare a %1.</translation>
    </message>
    <message>
        <source>Configuration write refused</source>
        <translation>Scrierea configurației refuzată</translation>
    </message>
    <message>
        <source>The edited content contains NUL bytes; the guarded write refuses it.</source>
        <translation>Conținutul editat conține octeți NUL; scrierea păzită îl refuză.</translation>
    </message>
    <message>
        <source>File too large</source>
        <translation>Fișier prea mare</translation>
    </message>
    <message>
        <source>The edited file is larger than 1 MiB. The guarded write refuses it; edit the file from a console instead.</source>
        <translation>Fișierul editat este mai mare de 1 MiB. Scrisul pazit il refuza; editeaza fisierul dintr-o consola in schimb.</translation>
    </message>
    <message>
        <source>Write target configuration</source>
        <translation>Scrie configurația țintei</translation>
    </message>
    <message>
        <source>Write the edited contents to %1? This modifies the repair target and invalidates cached diagnostics.</source>
        <translation>Scrieți conținutul editat pe %1? Acest lucru modifică ținta de reparații și invalidează diagnosticarea cache.</translation>
    </message>
    <message>
        <source>No diagnostic results to copy yet.</source>
        <translation>Nici un rezultat diagnostic pentru a copia încă.</translation>
    </message>
    <message>
        <source>Diagnostic results copied to the clipboard.</source>
        <translation>Rezultatele de diagnostic copiate în clipboard.</translation>
    </message>
    <message>
        <source>No diagnostic results to save yet.</source>
        <translation>Nici un rezultat diagnostic pentru a salva încă.</translation>
    </message>
    <message>
        <source>Text files (*.txt);;All files (*)</source>
        <translation>Fișiere text (*.txt);;Toate fișierele (*)</translation>
    </message>
    <message>
        <source>Save diagnostic results</source>
        <translation>Salvează rezultatele diagnosticului</translation>
    </message>
    <message>
        <source>Could not write %1.</source>
        <translation>Nu am putut scrie %1.</translation>
    </message>
    <message>
        <source>Diagnostic results saved to %1</source>
        <translation>Rezultatele de diagnostic salvate în %1</translation>
    </message>
    <message>
        <source>Unlock not available</source>
        <translation>Deblocare indisponibilă</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked. Select an offline repair target to unlock.</source>
        <translation>Gazda care rulează protejată nu poate fi deblocată. Alegeți o țintă de reparații offline pentru a debloca.</translation>
    </message>
    <message>
        <source>No locked LUKS component is currently visible on this selected drive.</source>
        <translation>Nicio componentă LUKS blocată nu este vizibilă în prezent pe această unitate selectată.</translation>
    </message>
    <message>
        <source>Confirm LUKS unlock</source>
        <translation>Confirmă deblocarea LUKS</translation>
    </message>
    <message>
        <source>Unlock %1 on %2?

The helper opens a temporary device-mapper mapping with cryptsetup and keeps it open for this recovery session. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>Deblocare %1 pe %2?

Ajutorul deschide o cartografiere temporară a dispozitivului-mapper cu cryptsetup și păstrează deschis pentru această sesiune de recuperare. Parola călătorește printr-un fișier cheie privat și nu este niciodată plasat în argumente de comandă sau jurnale.</translation>
    </message>
    <message>
        <source>LUKS unlock</source>
        <translation>TRA22X deblocare</translation>
    </message>
    <message>
        <source>Unlock LUKS repair target</source>
        <translation>Deblocare țintă de reparații LUKS</translation>
    </message>
    <message>
        <source>Enter the passphrase for %1.

It is sent only to cryptsetup over the helper&apos;s standard input and is never logged or placed on a command line.</source>
        <translation>Introduceți fraza de acces pentru %1.

Acesta este trimis doar pentru a cripta peste intrare standard de ajutor și nu este niciodată autentificat sau plasat pe o linie de comandă.</translation>
    </message>
    <message>
        <source>Passphrase required</source>
        <translation>Fraza de pas necesară</translation>
    </message>
    <message>
        <source>An empty passphrase was not submitted. Enter the LUKS passphrase or choose Cancel.</source>
        <translation>Nu a fost depusă o frază de acces goală. Introduceți fraza de acces LUKS sau selectați Anulează.</translation>
    </message>
    <message>
        <source>Unlock keyfile unavailable</source>
        <translation>Deblocare fișier cheie indisponibil</translation>
    </message>
    <message>
        <source>The LUKS passphrase could not be written to a private keyfile in %1; the unlock was not started.</source>
        <translation>Parola LUKS nu a putut fi scrisă într-un fişier de chei privat în %1; deblocarea nu a fost pornită.</translation>
    </message>
    <message>
        <source>Configuration write unavailable</source>
        <translation>Scriere configurare indisponibilă</translation>
    </message>
    <message>
        <source>The edited content could not be written to a private temporary file in %1; the write was not started.</source>
        <translation>Conținutul editat nu a putut fi scris într-un fișier temporar privat în %1; scrierea nu a fost începută.</translation>
    </message>
    <message>
        <source>ERROR: cannot read the session log %1</source>
        <translation>ERROR: nu se poate citi jurnalul de sesiune %1</translation>
    </message>
    <message>
        <source>Viewing a prior session log (read-only): %1</source>
        <translation>Vizualizarea unui jurnal al sesiunii anterioare (numai pentru citire): %1</translation>
    </message>
    <message>
        <source>Session log list refreshed.</source>
        <translation>Lista jurnal sesiune reîmprospătat.</translation>
    </message>
    <message>
        <source>Note:</source>
        <translation>Notã:</translation>
    </message>
    <message>
        <source>Delete session log</source>
        <translation>Șterge jurnalul sesiunii</translation>
    </message>
    <message>
        <source>Delete %1 permanently? This cannot be undone.</source>
        <translation>Șterge %1 permanent? Asta nu poate fi anulată.</translation>
    </message>
    <message>
        <source>%1 changed while the confirmation was open; the delete was refused.</source>
        <translation>%1 s-a schimbat în timp ce confirmarea a fost deschisă; ștergerea a fost refuzată.</translation>
    </message>
    <message>
        <source>Unable to delete %1.</source>
        <translation>Imposibil de șters %1.</translation>
    </message>
    <message>
        <source>Device discovery filters updated.</source>
        <translation>Filtre de descoperire a dispozitivului actualizate.</translation>
    </message>
    <message>
        <source>Unknown Linux distribution</source>
        <translation>Distribuţie necunoscută Linux</translation>
    </message>
    <message>
        <source>sudo / gksu (no KAuth on this frontend)</source>
        <translation>sudo / gksu (nu KAuth pe acest frontend)</translation>
    </message>
    <message>
        <source>Available</source>
        <translation>Disponibil</translation>
    </message>
    <message>
        <source>Missing on this frontend</source>
        <translation>Lipsește pe acest frontend</translation>
    </message>
    <message>
        <source>Missing</source>
        <translation>Lipsă</translation>
    </message>
    <message>
        <source>No repair tool is selected.</source>
        <translation>Nu este selectat niciun instrument de reparații.</translation>
    </message>
    <message>
        <source>A helper command is already running.</source>
        <translation>O comandă de ajutor este deja difuzate.</translation>
    </message>
    <message>
        <source>Restore Graphical Login is a host-scope stage on this legacy frontend; enter Host Maintenance to run it.</source>
        <translation>Restaurare Login Graphical este o etapă-scop gazdă pe acest frontend moștenire; introduceți Host Maintenance pentru a rula.</translation>
    </message>
    <message>
        <source>The selected scope has no resolved root component; use Refresh Devices and commit the repair target again.</source>
        <translation>Domeniul de aplicare selectat nu are nicio componentă rădăcină rezolvată; utilizați Dispozitive Reîmprospătare și angajați din nou ținta de reparații.</translation>
    </message>
    <message>
        <source>This legacy frontend exposes no %1 action; the helper reports the capability as available.</source>
        <translation>Acest frontend moștenire expune nicio acțiune %1; ajutorul raportează capacitatea ca fiind disponibilă.</translation>
    </message>
    <message>
        <source>Run this guarded repair action using the cached read-only diagnostic evidence. A confirmation is shown first.</source>
        <translation>Rulaţi această acţiune de reparaţii păzită folosind probele de diagnosticare numai pe baza unei cache. O confirmare este afișată mai întâi.</translation>
    </message>
    <message>
        <source>Enabled in Settings</source>
        <translation>Activat în configurări</translation>
    </message>
    <message>
        <source>Disabled in Settings - enable it to include this stage</source>
        <translation>Dezactivat în Setări - permiteți-i să includă această etapă</translation>
    </message>
    <message>
        <source>Unavailable: %1</source>
        <translation>Indisponibil: %1</translation>
    </message>
    <message>
        <source>
Unavailable: %1</source>
        <translation>
Indisponibil: %1</translation>
    </message>
    <message>
        <source>Repair tool unavailable</source>
        <translation>Dispozitiv de reparații indisponibil</translation>
    </message>
    <message>
        <source>Confirm repair</source>
        <translation>Confirmă repararea</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Your saved selection is kept and its availability is re-checked when diagnostics for this scope complete.</source>
        <translation>Nici o dovadă de capacitate cache pentru această etapă încă. Selecţia dvs. salvată este păstrată şi disponibilitatea sa este re-verificat atunci când diagnostice pentru acest domeniu de aplicare complet.</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Run diagnostics for the selected scope to populate the Full Repair plan; your selection is saved once the stage becomes available.</source>
        <translation>Nici o dovadă de capacitate cache pentru această etapă încă. Rulați diagnostice pentru domeniul de aplicare selectat pentru a popula planul de reparații complete; selecția dumneavoastră este salvată odată ce etapa devine disponibilă.</translation>
    </message>
    <message>
        <source>Unknown Full Repair stage.</source>
        <translation>Stadiul de reparaţii complete necunoscut.</translation>
    </message>
    <message>
        <source>No Full Repair stages are selected or available; use Configure Plan... to choose the stages.</source>
        <translation>Nu sunt selectate sau disponibile etape de reparare completă; utilizaţi Planul de configurare... pentru a alege etapele.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab to establish the session.</source>
        <translation>Este necesară autorizarea administratorului; apăsaţi pe Autorizaţie pe fila Sisteme sau Reparaţii pentru a stabili sesiunea.</translation>
    </message>
    <message>
        <source>Runs the selected stages using the cached read-only diagnostic evidence after privilege confirmation.</source>
        <translation>Rulează etapele selectate folosind dovezile de diagnosticare numai-citite cache după confirmarea privilegiilor.</translation>
    </message>
    <message>
        <source>No Full Repair stages selected - use Configure Plan... or Settings.</source>
        <translation>Nu sunt selectate etape de reparații complete - utilizați Configurați Plan... sau Setări.</translation>
    </message>
    <message>
        <source>1 stage selected</source>
        <translation>1 etapă selectată</translation>
    </message>
    <message>
        <source>%1 stages selected</source>
        <translation>Stadii %1 selectate</translation>
    </message>
    <message>
        <source>No repair stages are selected. Use Configure Plan... to choose the stages Full Repair will run.</source>
        <translation>Nu sunt selectate etape de reparații. Folosește Planul de configurare... pentru a alege etapele de reparații complete va rula.</translation>
    </message>
    <message>
        <source>No selected stages are available. %1</source>
        <translation>Nu sunt disponibile etape selectate. %1</translation>
    </message>
    <message>
        <source>Full Repair unavailable</source>
        <translation>Reparație completă indisponibilă</translation>
    </message>
    <message>
        <source>Run the Full Repair plan?

The selected stages run in order through the helper&apos;s guarded repair command:

</source>
        <translation>Rulați planul de reparații complete?

Etapele selectate se execută în ordine prin comanda de reparare păzită a ajutorului:

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
Ajutorul menţine fiecare zbor înainte de zbor; o etapă care eşuează opreşte planul.</translation>
    </message>
    <message>
        <source>Running %1 through the privileged helper... The Logs tab keeps the complete transcript.</source>
        <translation>Rularea %1 prin ajutorul privilegiat... Fila jurnalele păstrează transcrierea completă.</translation>
    </message>
    <message>
        <source>Shell scope required</source>
        <translation>Domeniul de aplicare Shell necesar</translation>
    </message>
    <message>
        <source>No administrator session is active.

Press Authorize on the Systems or Repair tab to establish the session, or enter Host Maintenance / commit a repair target on the Systems tab; the chroot shell then reuses the cached authorization.</source>
        <translation>Nicio sesiune a administratorului nu este activă.

Apăsați Autorizați pe fila Systems sau Reparare pentru a stabili sesiunea, sau introduceți Host Maintenance / comiteți o țintă de reparații pe tab-ul Systems; scoica Chroot refolosi apoi autorizația cached.</translation>
    </message>
    <message>
        <source>Shell command required</source>
        <translation>Comanda Shell necesară</translation>
    </message>
    <message>
        <source>Enter the command to run first.</source>
        <translation>Introduceţi comanda pentru a rula mai întâi.</translation>
    </message>
    <message>
        <source>Confirm running-host command</source>
        <translation>Confirmă comanda gazdei care rulează</translation>
    </message>
    <message>
        <source>Run this command as root on the protected running host?

%1

The helper keeps its runtime preflights; the command is passed as one argument and is never interpreted by the GUI.</source>
        <translation>Rulați această comandă ca rădăcină pe gazda care rulează protejat?

%1

Ajutorul își păstrează pre-zborurile; comanda este transmisă ca un argument și nu este niciodată interpretată de GUI.</translation>
    </message>
    <message>
        <source>Running %1...</source>
        <translation>Rulez %1...</translation>
    </message>
    <message>
        <source>Authorization scope required</source>
        <translation>Sfera de autorizare necesară</translation>
    </message>
    <message>
        <source>administrator authorization</source>
        <translation>Autorizare administrator</translation>
    </message>
    <message>
        <source>Administrator authorization</source>
        <translation>Autorizaţie de administrator</translation>
    </message>
    <message>
        <source>Administrator authorization is required for %1.

Enter the password for %2 (sudo). It is used only for this sudo authentication, is sent over a pipe and is never logged or placed on a command line. The authorization is cached for this session and reused by diagnostics and repairs.</source>
        <translation>Autorizaţia de administrare este necesară pentru %1.

Introduceți parola pentru %2 (sudo). Acesta este utilizat numai pentru această autentificare sudo, este trimis peste o conductă și nu este niciodată autentificat sau plasat pe o linie de comandă. Autorizatia este rezervata pentru aceasta sesiune si reutilizata prin diagnostice si reparatii.</translation>
    </message>
    <message>
        <source>your account</source>
        <translation>contul dumneavoastră</translation>
    </message>
    <message>
        <source>Password required</source>
        <translation>Parolă necesară</translation>
    </message>
    <message>
        <source>An empty password was not submitted. Enter the sudo password or choose Cancel.</source>
        <translation>Nu a fost depusă o parolă goală. Introduceți parola sudo sau selectați Anulează.</translation>
    </message>
    <message>
        <source>Administrator authorization failed</source>
        <translation>Autorizația administratorului a eșuat</translation>
    </message>
    <message>
        <source>sudo did not accept the password: %1

The command was not started.</source>
        <translation>Sudo nu a acceptat parola: %1

Comanda nu a început.</translation>
    </message>
    <message>
        <source>elevation requires an interactive sudo password; run the smoke as root or after `sudo -S -v` with --elevate &apos;sudo -n&apos;</source>
        <translation>elevație necesită o parolă sudo interactivă; rulați fumul ca rădăcină sau după sudo -S -v</translation>
    </message>
    <message>
        <source>No administrator session is active for %1.

Press Authorize on the Systems or Repair tab to establish the session now, or enter Host Maintenance / commit a repair target on the Systems tab; diagnostics and repairs then reuse the cached authorization.</source>
        <translation>Nicio sesiune de administrare nu este activă pentru %1.

Apăsați Autorizați pe fila Sisteme sau reparații pentru a stabili sesiunea acum, sau introduceți Host Maintenance / comiteți o țintă de reparații pe tab-ul Sisteme; diagnostice și reparații apoi reutilizarea autorizației cached.</translation>
    </message>
    <message>
        <source>Administrator authorization expired</source>
        <translation>Autorizaţia de administrare a expirat</translation>
    </message>
    <message>
        <source>The cached administrator authorization for %1 expired or was refused.

Press Authorize on the Systems or Repair tab to re-establish the session, then run the command again. No command was started.</source>
        <translation>Autorizaţia de administrare a %1 a expirat sau a fost refuzată.

Apăsați Autorizați pe fila Sisteme sau reparații pentru a restabili sesiunea, apoi executați comanda din nou. Nu a început nicio comandă.</translation>
    </message>
    <message>
        <source>Privileged operation completed successfully. Administrator authorization remains active for this session.</source>
        <translation>Operaţiune privilegiată încheiată cu succes. Autorizaţia administratorului rămâne activă pentru această sesiune.</translation>
    </message>
    <message>
        <source>Privileged operation stopped with an error. Administrator authorization remains active; review the output before closing.</source>
        <translation>Operaţiunea privilegiată s-a oprit cu o eroare. Autorizaţia de administrare rămâne activă; revizuiţi producţia înainte de închidere.</translation>
    </message>
    <message>
        <source>Inspection is read-only.

</source>
        <translation>Inspecţia e doar pentru citire.

</translation>
    </message>
    <message>
        <source>Configuration unavailable</source>
        <translation>Configurare indisponibilă</translation>
    </message>
    <message>
        <source>The helper could not read %1:

%2</source>
        <translation>Ajutor nu a putut citi %1:

%2</translation>
    </message>
    <message>
        <source>Configuration write failed</source>
        <translation>Scrierea configurației a eșuat</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
Result: mapping opened for this recovery session.</source>
        <translation>Stat: deblocat
Componentă: %1
Maper: %2
Metoda: helper deblocare (criptsetup; passfrase via a mode-600 keyfile, șterse după utilizare)
Rezultat: cartografiere deschisă pentru această sesiune de recuperare.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: the passphrase was not accepted; retry offered.</source>
        <translation>Stat: blocat
Componentă: %1
Metoda: deblocare ajutor (cryptsetup)
Eroare: fraza de acces nu a fost acceptată; reincercarea oferită.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: %2</source>
        <translation>Stat: blocat
Componentă: %1
Metoda: deblocare ajutor (cryptsetup)
Eroare: %2</translation>
    </message>
    <message>
        <source>Passphrase not accepted</source>
        <translation>Fraza de pas nu este acceptată</translation>
    </message>
    <message>
        <source>The LUKS passphrase was not accepted.

Try again?</source>
        <translation>Fraza de acces LUKS nu a fost acceptată.

Încercați din nou?</translation>
    </message>
    <message>
        <source>[-] %1 - not run (the plan stopped before reaching this stage)</source>
        <translation>[-] %1 - nu rulează (planul s-a oprit înainte de a atinge această etapă)</translation>
    </message>
    <message>
        <source>filesystem</source>
        <translation>sistem de fișiere</translation>
    </message>
    <message>
        <source>[-] %1 - no file system errors found - no changes</source>
        <translation>[-] %1 - nicio eroare a sistemului de fișiere găsită - nicio modificare</translation>
    </message>
    <message>
        <source>[FAIL] %1 - not reported</source>
        <translation>%1 - neraportat</translation>
    </message>
    <message>
        <source>%1 stage(s) failed; review the helper output in Logs.</source>
        <translation>Stadiul %1 (s) a eșuat; revizuiți ieșirea ajutorului în jurnale.</translation>
    </message>
    <message>
        <source>the plan stopped before any stage completed.</source>
        <translation>planul s-a oprit înainte de orice etapă finalizată.</translation>
    </message>
    <message>
        <source>no repair was needed; cached diagnostics remain valid.</source>
        <translation>nu a fost necesară nicio reparaţie; diagnosticul prin cache rămâne valabil.</translation>
    </message>
    <message>
        <source>repair completed; cached diagnostics were invalidated and must be regenerated.</source>
        <translation>repararea finalizată; diagnosticarea prin cache a fost invalidată și trebuie regenerată.</translation>
    </message>
    <message>
        <source>%1 results: [OK] %2 successful | [FAIL] %3 failed | [-] %4 no repair needed - %5</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Re-run Diagnostic</source>
        <translation>Reluare diagnostic</translation>
    </message>
    <message>
        <source>Unavailable</source>
        <translation>Indisponibil</translation>
    </message>
    <message>
        <source>Commit this physical drive as the repair target.</source>
        <translation>Comite această unitate fizică ca țintă de reparații.</translation>
    </message>
    <message>
        <source>Protection:</source>
        <translation>Protecţie:</translation>
    </message>
    <message>
        <source>Host shell</source>
        <translation>Înveliş de gazdă</translation>
    </message>
    <message>
        <source>Run on Host</source>
        <translation>Rulează pe gazdă</translation>
    </message>
    <message>
        <source>Host Shell</source>
        <translation>Gazdă Shell</translation>
    </message>
    <message>
        <source>The read-only probe found no editable target configuration file in this target. %1</source>
        <translation>Sonda nu a găsit niciun fişier de configurare a ţintei în această ţintă. %1</translation>
    </message>
    <message>
        <source>Not present in the selected target (omitted): %1</source>
        <translation>Nu este prezent în ținta selectată (ajustată): %1</translation>
    </message>
    <message>
        <source>Run diagnostics to probe which target configuration files exist; the helper&apos;s read-only probe decides the list.</source>
        <translation>Rulați diagnostice pentru a testa ce fișiere de configurare țintă există; sonda numai-citit a ajutorului decide lista.</translation>
    </message>
    <message>
        <source>Probed read-only by the helper; a saved edit invalidates the cached diagnostics.</source>
        <translation>Probed citit-doar de către ajutor; o editare salvată invalidează diagnosticul cache.</translation>
    </message>
    <message>
        <source>Host maintenance: %1</source>
        <translation>Întreţinere gazdă: %1</translation>
    </message>
    <message>
        <source>unresolved</source>
        <translation>nerezolvat</translation>
    </message>
    <message>
        <source>Target: %1 + %2</source>
        <translation>Obiectiv: %1 + %2</translation>
    </message>
    <message>
        <source>Host command</source>
        <translation>Comandă gazdă</translation>
    </message>
    <message>
        <source>Execute a command on the running host as root.</source>
        <translation>Execută comanda gazdei care rulează ca rădăcină.</translation>
    </message>
    <message>
        <source>Execute a command inside the selected repair system as root.</source>
        <translation>Execută o comandă în interiorul sistemului de reparații selectat ca rădăcină.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab (or re-enter Host Maintenance / re-commit the repair target) to authorize this session.</source>
        <translation>Autorizaţia de administrare este necesară; apăsaţi pe Autorizaţie pe fila Sisteme sau Reparaţii (sau reintraţi în întreţinerea gazdelor/recomandaţi ţinta de reparaţii) pentru a autoriza această sesiune.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root on the running host through the helper&apos;s guarded host-shell verb.</source>
        <translation>Rulați o comandă revizuită ca rădăcină pe gazda care rulează prin verbul pazit de ajutor gazdă-shell.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root inside the target chroot through the helper&apos;s guarded shell verb.</source>
        <translation>Rulați o comandă revizuită ca rădăcină în interiorul chroot țintă prin verbul scoica păzit de ajutor.</translation>
    </message>
    <message>
        <source>Run a command on the running host as root (sudo is not needed). Commands are executed directly on the active system; output is kept in this window and in the application log.</source>
        <translation>Rulați o comandă pe gazda care rulează ca rădăcină (sudo nu este necesar). Comenzile sunt executate direct pe sistemul activ; ieşirea este păstrată în această fereastră şi în jurnalul de aplicare.</translation>
    </message>
    <message>
        <source>Run a command inside the selected repair system as root (sudo is not needed). Commands are executed one at a time in a fresh chroot and cannot answer interactive prompts; use non-interactive flags such as apt-get -y upgrade. Output is kept in this window and in the application log.</source>
        <translation>Rulați o comandă în interiorul sistemului de reparații selectat ca rădăcină (sudo nu este necesar). Comenzile sunt executate una la un moment dat într-un chroot proaspăt și nu pot răspunde prompte interactive; utilizați steaguri inactive, cum ar fi apt-get-y upgrade. Ieșirea este păstrată în această fereastră și în jurnalul de aplicații.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize or re-enter Host Maintenance / commit a repair target.</source>
        <translation>Nici o sesiune de administrator este activ; apăsați Autorizați sau re-intrați Host Întreținere / comiteți o țintă de reparații.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from the repaired system</source>
        <translation>1. Selectaţi fişiere sursă sau foldere din sistemul reparat</translation>
    </message>
    <message>
        <source>2. Choose destination on this host</source>
        <translation>2. Alegeţi destinaţia de pe această gazdă</translation>
    </message>
    <message>
        <source>Add File Path...</source>
        <translation>Adaugă calea fișierului...</translation>
    </message>
    <message>
        <source>Add Folder Path...</source>
        <translation>Adaugă cale dosar...</translation>
    </message>
    <message>
        <source>Browse...</source>
        <translation>Răsfoieşte...</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it (Host Maintenance does not provide a repair tree to browse).</source>
        <translation>Selectaţi unitatea de reparaţii în sisteme înainte de a alege o destinaţie în interiorul ei (Host Mentainment nu oferă un arbore de reparare pentru a naviga).</translation>
    </message>
    <message>
        <source>Choose a host destination folder directly.</source>
        <translation>Alegeți direct un dosar destinatie gazdă.</translation>
    </message>
    <message>
        <source>Copy the staged files and folders with cp -a, restore ownership with chown --reference and byte-compare every regular file afterwards.</source>
        <translation>Copiați fișierele și folderele înscenate cu cp -a, restabiliți proprietatea cu crown -- referințe și octeți compare fiecare fișier obișnuit după aceea.</translation>
    </message>
    <message>
        <source>File Copy unavailable</source>
        <translation>Copie fișier indisponibilă</translation>
    </message>
    <message>
        <source>Choose host destination folder</source>
        <translation>Alegeți dosarul destinație gazdă</translation>
    </message>
    <message>
        <source>Select a repair target</source>
        <translation>Alegeți o țintă de reparații</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it.</source>
        <translation>Selectaţi unitatea de reparaţii în sisteme înainte de a alege o destinaţie în interiorul acestuia.</translation>
    </message>
    <message>
        <source>File Copy - Browse Target Folders</source>
        <translation>Copie fișier - Navighează dosarele țintă</translation>
    </message>
    <message>
        <source>Browse Target Folders</source>
        <translation>Navighează dosarele țintă</translation>
    </message>
    <message>
        <source>The helper could not list the repair-system folder:

%1</source>
        <translation>Ajutorul nu a putut lista dosarul sistemului de reparații:

%1</translation>
    </message>
    <message>
        <source>BROWSE_ENTRY</source>
        <translation>CREȘTERE ENTRY</translation>
    </message>
    <message>
        <source>(choose this folder: %1)</source>
        <translation>(Alegeți acest dosar: %1)</translation>
    </message>
    <message>
        <source>(parent folder)</source>
        <translation>(folderul părinte)</translation>
    </message>
    <message>
        <source>Select repair-system destination</source>
        <translation>Alegeți destinația sistemului de reparații</translation>
    </message>
    <message>
        <source>Choose a destination folder inside the repaired system (current folder: %1):</source>
        <translation>Alegeți un dosar de destinație în interiorul sistemului reparat (folderul curent: %1):</translation>
    </message>
    <message>
        <source>Folder</source>
        <translation>Dosar</translation>
    </message>
    <message>
        <source>Open</source>
        <translation>Deschide</translation>
    </message>
    <message>
        <source>(choose this folder:</source>
        <translation>(Alegeți acest dosar:</translation>
    </message>
    <message>
        <source>Add files to copy</source>
        <translation>Adaugă fișiere de copiat</translation>
    </message>
    <message>
        <source>Add folder to copy</source>
        <translation>Adaugă dosar la copie</translation>
    </message>
    <message>
        <source>Stage at least one source and name a destination path first.</source>
        <translation>Etapa cel puțin o sursă și numele o cale de destinație mai întâi.</translation>
    </message>
    <message>
        <source>Copy the %1 staged item(s) to %2?

The helper keeps its direction and path containment checks; a sensitive repair-system destination is refused unless the helper approves it, and every regular file is byte-compared after the copy.</source>
        <translation>Copiați elementul pus în scenă %1 (s) la %2?

Asistentul îşi păstrează direcţia şi calea de control de izolare; o destinaţie sensibilă a sistemului de reparaţii este refuzată, cu excepţia cazului în care ajutorul o aprobă, şi fiecare fişier regulat este octet-comparativ după copie.</translation>
    </message>
    <message>
        <source>File Copy - Copy and Verify</source>
        <translation>Copiere fișier - Copiere și verificare</translation>
    </message>
    <message>
        <source>File Copy - Preview Changes</source>
        <translation>Copie fișier - Modificări de previzualizare</translation>
    </message>
    <message>
        <source>Protected running host - details</source>
        <translation>Gazda de rulare protejată - detalii</translation>
    </message>
    <message>
        <source>not mounted (offline target)</source>
        <translation>nemontat (țintă offline)</translation>
    </message>
    <message>
        <source>Select a drive to see its details.</source>
        <translation>Selectaţi o unitate pentru a vedea detaliile sale.</translation>
    </message>
    <message>
        <source>Inspecting the selected component.</source>
        <translation>Inspectarea componentei selectate.</translation>
    </message>
    <message>
        <source>Helper-confirmed by the last read-only diagnostics.</source>
        <translation>Ajutor confirmat de ultimele diagnostice.</translation>
    </message>
    <message>
        <source>Read-only inventory only; run diagnostics to confirm.</source>
        <translation>Numai inventar de citire; rulați diagnostice pentru a confirma.</translation>
    </message>
    <message>
        <source>PROTECTED - running system; read-only details only</source>
        <translation>PROTECŢIE - sistem de funcţionare; numai detalii de citire</translation>
    </message>
    <message>
        <source>Live / installer media - not selectable</source>
        <translation>Medii live / instalatoare - neselectabile</translation>
    </message>
    <message>
        <source>PROTECTED - running host scope</source>
        <translation>PROTECŢIE - funcţionarea domeniului gazdă</translation>
    </message>
    <message>
        <source>Unlock required before selection</source>
        <translation>Deblocare necesară înainte de selecție</translation>
    </message>
    <message>
        <source>Eligible repair candidate</source>
        <translation>Candidat la reparații eligibile</translation>
    </message>
    <message>
        <source>Drive:</source>
        <translation>Condu:</translation>
    </message>
    <message>
        <source>Detected target:</source>
        <translation>Ţinta detectată:</translation>
    </message>
    <message>
        <source>Pending inspection</source>
        <translation>În așteptarea inspecției</translation>
    </message>
    <message>
        <source>Model / label:</source>
        <translation>Model / etichetă:</translation>
    </message>
    <message>
        <source>Status:</source>
        <translation>Stare:</translation>
    </message>
    <message>
        <source>Size:</source>
        <translation>Mărime:</translation>
    </message>
    <message>
        <source>Connection:</source>
        <translation>Conexiune:</translation>
    </message>
    <message>
        <source>Filesystem:</source>
        <translation>Sistem de fișiere:</translation>
    </message>
    <message>
        <source>UUID:</source>
        <translation>UUID:</translation>
    </message>
    <message>
        <source>Mounts:</source>
        <translation>Monturi:</translation>
    </message>
    <message>
        <source>Running system protection unresolved</source>
        <translation>Protecția sistemului de rulare nerezolvată</translation>
    </message>
    <message>
        <source>No protected physical backing disk was identified</source>
        <translation>Nu a fost identificat nici un disc de suport fizic protejat</translation>
    </message>
    <message>
        <source>Current running Linux system</source>
        <translation>Sistem Linux curent</translation>
    </message>
    <message>
        <source>Critical mounts: %1</source>
        <translation>Monturi critice: %1</translation>
    </message>
    <message>
        <source>Read-only protected-running-system facts: the helper OS fact plus the inventory model, device path, size, transport and critical mounts. Nothing here is probed destructively.</source>
        <translation>Read-numai protejate-running-sistem de fapte: helper OS fapt plus modelul de inventar, calea dispozitivului, dimensiunea, transport și montari critice. Nimic aici nu este cercetat distructiv.</translation>
    </message>
    <message>
        <source>Select a drive to see unlock status.</source>
        <translation>Alegeți o unitate pentru a vedea starea de deblocare.</translation>
    </message>
    <message>
        <source>State: protected
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
The protected running host cannot be unlocked or modified; unlock is available for an offline repair target only. Use Host Maintenance for the protected running host.</source>
        <translation>Stat: protejat
Componentă: %1
Maper: (nici unul)
Metoda: helper deblocare (criptsetup; passfrase via a mode-600 keyfile, șterse după utilizare)
Gazda care rulează protejată nu poate fi deblocată sau modificată; deblocarea este disponibilă doar pentru o țintă de reparații offline. Utilizați Host Întreținere pentru gazda care rulează protejat.</translation>
    </message>
    <message>
        <source>(none detected)</source>
        <translation>(nici unul detectat)</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
A locked LUKS container is visible on this drive; press Unlock to open it for this recovery session.</source>
        <translation>Stat: blocat
Componentă: %1
Maper: (nici unul)
Metoda: helper deblocare (criptsetup; passfrase via a mode-600 keyfile, șterse după utilizare)
Un container închis LUKS este vizibil pe această unitate; apăsați Deblocare pentru a-l deschide pentru această sesiune de recuperare.</translation>
    </message>
    <message>
        <source>(visible mapper)</source>
        <translation>(Mapper vizibil)</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: already open before this session (visible mapper; Boot Bitch will reuse it and will not close it).</source>
        <translation>Stat: deblocat
Componentă: %1
Maper: %2
Metoda: deja deschisă înainte de această sesiune (maper vizibil; Boot Bitch o va refolosi și nu o va închide).</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper-confirmed mapping from the last read-only diagnostics.</source>
        <translation>Stat: deblocat
Componentă: %1
Maper: %2
Metoda: cartografiere confirmată de un ajutor de la ultimele diagnostice.</translation>
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
        <translation>Stare: componentă blocată sau necriptată detectată
Componentă: (nici una detectată)
Maper: (nici unul)
Metoda: helper deblocare (criptsetup; passfrase via a mode-600 keyfile, șterse după utilizare)
Nicio componentă LUKS blocată și nicio operațiune de deblocare nu au fost înregistrate pentru această unitate în sesiunea curentă.</translation>
    </message>
    <message>
        <source>unavailable - %1</source>
        <translation>indisponibil - %1</translation>
    </message>
    <message>
        <source>Host maintenance:
%1</source>
        <translation>Întreţinerea gazdelor:
%1</translation>
    </message>
    <message>
        <source>Target:
%1 + %2</source>
        <translation>Țintă:
%1 + %2</translation>
    </message>
    <message>
        <source>Authorization required: diagnostics and repairs fail closed until you press Authorize.</source>
        <translation>Autorizatie necesara: diagnosticarea si reparatiile nu se inchid pana cand nu apesi Autorizare.</translation>
    </message>
    <message>
        <source>Re-establish the privileged helper session for the current scope now. The password is requested in the hidden-input modal and is never logged.</source>
        <translation>Restabileşte sesiunea de ajutor privilegiat pentru domeniul de aplicare actual acum. Parola este solicitata in modul de introducere ascunsa si nu este niciodata logata.</translation>
    </message>
    <message>
        <source>Running...</source>
        <translation>Alergând...</translation>
    </message>
    <message>
        <source>Scope required</source>
        <translation>Domeniul de aplicare necesar</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope; this unlocks the gated actions.</source>
        <translation>Rulați Toate - executați fiecare diagnostic disponibil numai citire pentru domeniul de aplicare curent; acest lucru deblochează acțiunile poartă.</translation>
    </message>
    <message>
        <source>Select a target and wait for any running command first.</source>
        <translation>Alegeți o țintă și așteptați pentru orice comandă de funcționare mai întâi.</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic through the helper.</source>
        <translation>Run Diagnostic - executați diagnosticul numai-citit selectat prin ajutorul.</translation>
    </message>
    <message>
        <source>Run All for the current scope and refresh the read-only backend profile.</source>
        <translation>Rulați Toate pentru domeniul de aplicare curent și reîmprospătați profilul backend numai-read.</translation>
    </message>
    <message>
        <source>Read or edit the selected target configuration file through the guarded helper; a saved edit invalidates cached diagnostics.</source>
        <translation>Citiți sau editați fișierul de configurare țintă selectat prin ajutorul păzit; o editare salvată invalidează diagnosticarea cache.</translation>
    </message>
    <message>
        <source>Host Maintenance has no target-file editing; commit an offline repair target first.</source>
        <translation>Host Întreținere nu are nici o editare fișier țintă; comite o țintă de reparații offline mai întâi.</translation>
    </message>
    <message>
        <source>Commit an offline repair target first.</source>
        <translation>Comite o ţintă de reparaţii offline mai întâi.</translation>
    </message>
    <message>
        <source>No target configuration file is available for this target; run diagnostics to probe the list.</source>
        <translation>Nu este disponibil niciun fișier de configurare țintă pentru această țintă; executați diagnostice pentru a verifica lista.</translation>
    </message>
    <message>
        <source>Already Unlocked</source>
        <translation>Deblocat deja</translation>
    </message>
    <message>
        <source>An unlocked Linux filesystem is already visible on this drive. Boot Bitch will reuse the existing mapper and will not close or reopen a mapping created by this recovery session. Mounting happens during diagnostics (read-only) and repairs (read-write); data filesystems are never auto-mounted on selection.</source>
        <translation>Un sistem de fișiere Linux deblocat este deja vizibil pe această unitate. Boot Bitch va refolosi harta existentă și nu va închide sau redeschide o cartografiere creată de această sesiune de recuperare. Montarea are loc în timpul diagnosticului (numai pentru citire) și al reparațiilor (citire-scriere); sistemele de fișiere de date nu sunt niciodată montate automat la selecție.</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked; use Host Maintenance for the protected running host.</source>
        <translation>Gazda care rulează protejată nu poate fi deblocată; utilizați Host Mentainment pentru gazda care rulează protejată.</translation>
    </message>
    <message>
        <source>Host Maintenance is the current scope, but the selected offline drive can still be unlocked.</source>
        <translation>Întreținerea gazdelor este domeniul de aplicare curent, dar unitatea offline selectată poate fi încă deblocată.</translation>
    </message>
    <message>
        <source>Unlock %1 using cryptsetup through the privileged helper. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>Deblocare %1 folosind cryptsetup prin ajutorul privilegiat. Parola călătorește printr-un fișier cheie privat și nu este niciodată plasat în argumente de comandă sau jurnale.</translation>
    </message>
    <message>
        <source>The protected running system cannot be selected as a repair target; use Host Maintenance for the protected running host.</source>
        <translation>Sistemul de funcţionare protejat nu poate fi selectat ca ţintă de reparaţii; utilizaţi Host Mentainment pentru gazda care rulează protejată.</translation>
    </message>
    <message>
        <source>Live / installer media is read-only boot media and cannot be selected as a repair target.</source>
        <translation>Mass-media live / instalator este doar de pornire și nu poate fi selectat ca țintă de reparații.</translation>
    </message>
    <message>
        <source>Unlock the encrypted volume first; Select Target becomes available after a Linux filesystem is detected.</source>
        <translation>Deblochează mai întâi volumul criptat; Selectează ținta devine disponibilă după detectarea unui sistem de fișiere Linux.</translation>
    </message>
    <message>
        <source>Committed repair target. Repair, Diagnostics and File Copy target this physical drive until another drive is explicitly selected with Select Target.</source>
        <translation>Ţinta de reparaţii. Repair, Diagnostics and File Copy țintă această unitate fizică până când un alt unitate este selectat în mod explicit cu Select Target.</translation>
    </message>
    <message>
        <source>Commit %1 as the repair target; this leaves Host Maintenance and switches the scope to the selected drive.</source>
        <translation>Comite %1 ca țintă de reparații; aceasta lasă Host Maintenance și comută domeniul de aplicare pe unitatea selectată.</translation>
    </message>
    <message>
        <source>The running host target could not be detected.</source>
        <translation>Ţinta gazdă care rulează nu a putut fi detectată.</translation>
    </message>
    <message>
        <source>Leave Host Maintenance and return to repair-target mode.</source>
        <translation>Lăsați Host Întreținere și reveniți la modul de reparații-țintă.</translation>
    </message>
    <message>
        <source>Select the running host for deliberate guarded maintenance; the administrator authorization is requested here once and cached for the session.</source>
        <translation>Selectați gazda care rulează pentru întreținere păzită deliberată; autorizația administratorului este solicitată aici o dată și cached pentru sesiune.</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a repair target.</source>
        <translation>Ţinta gazdă care rulează nu a putut fi detectată; diagnosticele necesită o ţintă de reparaţie.</translation>
    </message>
    <message>
        <source>Committed target: %1 + %2</source>
        <translation>Țintă angajată: %1 + %2</translation>
    </message>
    <message>
        <source>Committed target: none (selection changed)</source>
        <translation>Țintă angajată: niciuna (selecție modificată)</translation>
    </message>
    <message>
        <source>The selected scope has no resolved Linux root component; Refresh Devices and commit the repair target again.</source>
        <translation>Domeniul de aplicare selectat nu a rezolvat componenta rădăcină Linux; Reîmprospătați Dispozitivele și angajați din nou ținta de reparații.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize on the Systems or Repair tab to re-establish it. Run All reuses the cached authorization and never prompts by itself.</source>
        <translation>Nici o sesiune de administrator este activ; apăsați Autorizați pe fila Sisteme sau reparații pentru a o restabili. Rulați Toate reutilizează autorizația cached și nu se cere de la sine.</translation>
    </message>
    <message>
        <source>Run diagnostics for this scope to unlock the gated actions. Diagnostics are read-only and the only evidence source.</source>
        <translation>Rulați diagnostice pentru acest domeniu de aplicare pentru a debloca acțiunile poarta. Diagnosticele sunt doar citite și singura sursă de dovezi.</translation>
    </message>
    <message>
        <source>A repair was not proven unchanged, so the cached diagnostics are invalidated. Run diagnostics again before another gated action.</source>
        <translation>O reparaţie nu s-a dovedit neschimbată, aşa că diagnosticul cache este invalidat. Rulați din nou diagnostice înainte de o altă acțiune poarta.</translation>
    </message>
    <message>
        <source>Gated actions reflect the cached capability lines; the helper still runs every runtime preflight when a command starts.</source>
        <translation>Acţiunile Gated reflectă liniile de capacitate cache; ajutorul funcţionează încă la fiecare zbor înainte de începerea unei comenzi.</translation>
    </message>
    <message>
        <source>Regenerating diagnostics automatically</source>
        <translation>Diagnosticări regeneratoare automat</translation>
    </message>
    <message>
        <source>Running diagnostic: %1</source>
        <translation>Diagnostic de rulare: %1</translation>
    </message>
    <message>
        <source>Running all diagnostics</source>
        <translation>Rularea tuturor diagnosticelor</translation>
    </message>
    <message>
        <source>Unlocking %1</source>
        <translation>Deblocare %1</translation>
    </message>
    <message>
        <source>running host</source>
        <translation>gazdă de rulare</translation>
    </message>
    <message>
        <source>offline target</source>
        <translation>obiectiv offline</translation>
    </message>
    <message>
        <source>host maintenance active</source>
        <translation>Întreţinere gazdă activă</translation>
    </message>
    <message>
        <source>repair target committed</source>
        <translation>obiectiv de reparații angajat</translation>
    </message>
    <message>
        <source>no committed scope</source>
        <translation>fără domeniu de aplicare angajat</translation>
    </message>
    <message>
        <source>Current session</source>
        <translation>Sesiunea curentă</translation>
    </message>
    <message>
        <source>Log files (*.log);;All files (*)</source>
        <translation>Fișiere jurnal (*.log);;Toate fișierele (*)</translation>
    </message>
    <message>
        <source>Save log as</source>
        <translation>Salvează jurnalul ca</translation>
    </message>
    <message>
        <source>About Boot Bitch</source>
        <translation>Despre Boot Bitch</translation>
    </message>
    <message>
        <source>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;&lt;p&gt;&lt;b&gt;Developer:&lt;/b&gt; CaptainMorgan12&lt;/p&gt;&lt;p&gt;This GUI is the package&apos;s entry point: a Qt 3.3.x frontend with the ported guarded helper for Debian Etch-era systems.&lt;/p&gt;&lt;p&gt;&lt;b&gt;Guarded repair mode:&lt;/b&gt; read-only diagnostics can inspect either the protected Running Host or an explicitly selected repair drive. The guarded Debian/APT package stages (interrupted configuration, broken dependencies, metadata refresh, upgrade), GRUB-legacy configuration regeneration, LUKS target unlock and guarded target-file editing run through the ported helper after confirmation; Host Maintenance enables the same supported stages natively on the active system after repeating the host identity and boot-mount checks.&lt;/p&gt;&lt;p&gt;The modern-only features - verified File Copy, Btrfs snapshot rollback, EFI/UKI and extlinux repair, boot-stack reconciliation and Make Default - are greyed with the helper&apos;s own probe reasons on this frontend; Arch/Alpine/Fedora package backends stay diagnostics-only here.&lt;/p&gt;&lt;p&gt;The first privileged action authorizes one cached administrator session per scope through a hidden-input modal (the Qt GUI remains unprivileged). It can be ended at any time from File - Lock Administrator Session.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;&lt;p&gt;&lt;b&gt;Developer:&lt;/b&gt; CaptainMorgan12&lt;/p&gt;&lt;p&gt; Acest GUI este punctul de intrare al pachetului: un frontend Qt 3.3.x cu ajutorul portat pentru sistemele Debian Etch-era. &lt;/p&gt; &lt;p&gt; &lt;b&gt;Diagnoza numai pentru citire poate inspecta fie gazda de rulare protejată, fie o unitate de reparații selectată în mod explicit. Etapele de pachete Debian/APT păzite (configurație întreruptă, dependențe rupte, reîmprospătare metadate, actualizare), regenerarea configurației GRUB-legacy, deblocarea țintei LUKS și editarea fișierului țintă păzită rula prin ajutorul portat după confirmare; Host Maintenance permite aceleași etape sprijinite nativ pe sistemul activ după repetarea identității gazdei și a controalelor boot-mount.&lt;/p&gt; Caracteristicile moderne - verificate File Copy, Btrfs snapshot rollback, EFI/UKI și extlinux reparație, boot-stack reconciliere și Make Implicit - sunt grided cu propriile motive de sondă de ajutor pe acest frontend; Arch/Alpine/Fedora pachete backend-uri rămâne diagnostic-doar aici. &lt;/p&gt; &lt; p &gt; Prima acțiune privilegiată autorizează o sesiune cache de administrator pe domeniu de aplicare printr-un mod ascuns de introducere (GUI Qt rămâne neprivilegat). Acesta poate fi încheiat în orice moment din fișier - Lock Administrator sesiune.&lt;/p&gt;</translation>
    </message>
</context>
</TS>
