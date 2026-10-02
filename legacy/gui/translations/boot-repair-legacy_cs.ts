<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS language="cs">
<context>
    <name>LegacyMainWindow</name>
    <message>
        <source>Validate environment</source>
        <translation>Potvrdit prostředí</translation>
    </message>
    <message>
        <source>Validate</source>
        <translation>Potvrdit</translation>
    </message>
    <message>
        <source>Check the selected system&apos;s mounts, filesystem metadata, boot files, mapper consistency and dependency readiness before any repair action. This is an independent safety preflight rather than an optional Full Repair stage.</source>
        <translation>Před jakoukoli opravou zkontrolujte montáž vybraného systému, metadata souborového systému, zaváděcí soubory, konzistenci mapu a připravenost na závislost. Jedná se o nezávislý bezpečnostní předlet spíše než volitelné Fall Opravy fáze.</translation>
    </message>
    <message>
        <source>Always preflight</source>
        <translation>Vždy před letem</translation>
    </message>
    <message>
        <source>File system repair</source>
        <translation>Oprava souborového systému</translation>
    </message>
    <message>
        <source>Check File Systems</source>
        <translation>Zkontrolujte souborové systémy</translation>
    </message>
    <message>
        <source>Run the read-only file system check for the selected system&apos;s root and /boot filesystems and report each device&apos;s check tool and result without changing anything. This legacy frontend exposes the read-only check only; device repair is not wired.</source>
        <translation>Spusťte kontrolu systému read- only souborového systému pro kořenové a / boot soubory vybraného systému a nahlaste kontrolní nástroj každého zařízení a výsledek bez změny. Tento odkaz zobrazuje pouze kontrolu read- only; oprava zařízení není připojena.</translation>
    </message>
    <message>
        <source>Full Repair plan: unavailable on this frontend - read-only check only</source>
        <translation>Kompletní plán oprav: není k dispozici na této frontě - pouze kontrola</translation>
    </message>
    <message>
        <source>Complete package configuration</source>
        <translation>Kompletní konfigurace balíku</translation>
    </message>
    <message>
        <source>Complete Configuration</source>
        <translation>Kompletní konfigurace</translation>
    </message>
    <message>
        <source>Complete interrupted dpkg package configuration in the selected repair system. This is the same stage controlled by Settings -&gt; Full Repair plan -&gt; Complete interrupted package configuration, but it can also be run independently here.</source>
        <translation>Kompletní přerušená konfigurace balíku dpkg ve zvoleném opravném systému. Toto je stejná etapa řízená Nastavení - &gt; Kompletní plán oprav - &gt; Kompletní přerušená konfigurace balíku, ale také může být spuštěna nezávisle zde.</translation>
    </message>
    <message>
        <source>Run the guarded dpkg-configure repair? The helper keeps its package-lock and runtime preflights.</source>
        <translation>Spustit hlídané nastavení dpkg-? Pomocník udržuje své balící-zámek a runtime předlety.</translation>
    </message>
    <message>
        <source>Repair broken dependencies</source>
        <translation>Opravy porušených závislostí</translation>
    </message>
    <message>
        <source>Repair Dependencies</source>
        <translation>Opravy závislostí</translation>
    </message>
    <message>
        <source>Repair package dependencies in the selected repair system after the mandatory safety preflight. This maps directly to Settings -&gt; Repair broken package dependencies.</source>
        <translation>Závislost balíčků na zvoleném systému oprav po povinném předletu bezpečnosti. Mapy přímo na Nastavení - &gt; Oprava rozbitých závislostí na balení.</translation>
    </message>
    <message>
        <source>Run the guarded fix-broken repair? The helper keeps its simulation-first preflight and runtime guards.</source>
        <translation>Spustit hlídanou opravu? Pomocník si nechává svou simulaci - první pre-let a runtime stráží.</translation>
    </message>
    <message>
        <source>Refresh package metadata</source>
        <translation>Obnovit metadata balíčků</translation>
    </message>
    <message>
        <source>Refresh Metadata</source>
        <translation>Obnovit metadata</translation>
    </message>
    <message>
        <source>Refresh APT metadata in the selected repair system without upgrading installed packages. This maps directly to Settings -&gt; Refresh package metadata.</source>
        <translation>Obnovit metadata APT ve zvoleném systému oprav bez modernizace instalovaných balíčků. Mapy přímo na Nastavení - &gt; Obnovit metadata balíčku.</translation>
    </message>
    <message>
        <source>Refresh package metadata for the selected scope? The helper requires a reachable, trusted APT source.</source>
        <translation>Obnovit metadata balíku pro vybraný rozsah? Pomocník vyžaduje dosažitelný důvěryhodný zdroj APT.</translation>
    </message>
    <message>
        <source>Upgrade installed packages</source>
        <translation>Aktualizovat nainstalované balíky</translation>
    </message>
    <message>
        <source>Simulate and Upgrade</source>
        <translation>Simulovat a aktualizovat</translation>
    </message>
    <message>
        <source>Simulate the APT transaction first, inspect proposed removals, then apply a safe upgrade. This maps directly to Settings -&gt; Upgrade installed packages.</source>
        <translation>Nejprve simulujte transakci APT, zkontrolujte navrhované odstranění a poté použijte bezpečné upgrade. Mapy přímo na Nastavení - &gt; Aktualizovat nainstalované balíčky.</translation>
    </message>
    <message>
        <source>Run the guarded apt-upgrade transaction? The helper keeps its simulation-first and source guards.</source>
        <translation>Spustit zabezpečenou transakci s upgradem? Pomocník si nechává simulaci - první a zdrojové stráže.</translation>
    </message>
    <message>
        <source>DKMS</source>
        <translation>DKMS</translation>
    </message>
    <message>
        <source>Rebuild DKMS</source>
        <translation>Přestavět DKMS</translation>
    </message>
    <message>
        <source>Rebuild out-of-tree kernel modules for kernels installed in the selected system. The helper refuses this action when DKMS is not installed; this legacy frontend exposes no DKMS action.</source>
        <translation>Rebuild out- of- tree kernelové moduly pro jádra instalovaná ve zvoleném systému. Pomocník tuto akci odmítá, pokud není DKMS nainstalován; tato odkaz nevykazuje žádnou akci DKMS.</translation>
    </message>
    <message>
        <source>Graphical login / display manager</source>
        <translation>Správce grafického přihlášení / zobrazení</translation>
    </message>
    <message>
        <source>Restore Graphical Login</source>
        <translation>Obnovit grafické přihlášení</translation>
    </message>
    <message>
        <source>Restore the legacy SysV display manager configured for the running host: the /etc/X11/default-display-manager entry and the missing runlevel S-symlink, with a backup and rollback, never starting the GUI. This is a host-scope stage on this legacy frontend.</source>
        <translation>Obnovit odkaz SysV displej správce nakonfigurován pro spuštěný hostitel: / etc / X11 / default-display-manager vstup a chybějící runlevel S-symlink, se zálohou a rollback, nikdy spuštění GUI. Tohle je fáze hostitelského prostoru na této památkové frontě.</translation>
    </message>
    <message>
        <source>Restore the graphical login configuration for the running host? The helper backs up /etc/X11/default-display-manager and the runlevel symlink state, restores the configured entry and the missing S-symlink, rolls back on any failure, and never starts the display manager.</source>
        <translation>Obnovit grafickou přihlašovací konfiguraci pro běžící hostitele? Pomocník zálohuje / etc / X11 / default-display-manager a runlevel symlink stav, obnovuje nakonfigurovaný vstup a chybějící S-symlink, vrací se zpět na jakékoliv selhání, a nikdy spustí správce displeje.</translation>
    </message>
    <message>
        <source>Initramfs</source>
        <translation>Initramfs</translation>
    </message>
    <message>
        <source>Rebuild Initramfs</source>
        <translation>Přestavět Inlitramfs</translation>
    </message>
    <message>
        <source>Rebuild initramfs images for the selected repair system only after mapper and crypttab consistency checks pass. The helper backs up each image before the apply. On Etch the stage runs through the guarded plain-chroot fallback (no unshare required).</source>
        <translation>Obnovit snímky initramfs pro vybraný opravárenský systém až poté, co projdou kontroly konzistence mapper a crypttab. Pomocník zálohuje každý obrázek před aplikací. Na Etch jeviště běží přes hlídané prostranství (není třeba se dělit).</translation>
    </message>
    <message>
        <source>Rebuild the initramfs for the selected scope? The helper keeps its mapper/crypttab and backup preflights.</source>
        <translation>Přestavět initramfs pro vybraný rozsah? Pomocník si nechává svůj mapper / krypttab a záložní předlety.</translation>
    </message>
    <message>
        <source>EFI / UKI bootloader</source>
        <translation>EFI / UKI bootloader</translation>
    </message>
    <message>
        <source>Repair EFI / UKI</source>
        <translation>Opravy EFI / UKI</translation>
    </message>
    <message>
        <source>Repair the selected system&apos;s EFI / UKI boot path. This legacy frontend exposes no EFI action; the Etch target is a BIOS/GRUB-legacy system.</source>
        <translation>Oprava zvolené zaváděcí cesty systému EFI / UKI. Tento odkaz ukazuje žádnou akci EFI; Etch cíl je BIOS / GRUB- dědictví systému.</translation>
    </message>
    <message>
        <source>GRUB configuration</source>
        <translation>Konfigurace GRUB</translation>
    </message>
    <message>
        <source>Regenerate GRUB</source>
        <translation>Regenerovat GRUB</translation>
    </message>
    <message>
        <source>Regenerate the selected repair system&apos;s GRUB menu/configuration after the mandatory safety preflight. The helper backs up menu.lst, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Po povinném předletu bezpečnosti obnovte nabídku / konfiguraci vybraného servisního systému GRUB. Pomocník zálohuje menu.lst, zachovává každý existující vstup a vrací se k jakémukoliv selhání.</translation>
    </message>
    <message>
        <source>Regenerate the GRUB configuration? The helper backs up the target menu/configuration, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Regenerovat konfiguraci GRUB? Pomocník zálohuje cílové menu / konfiguraci, zachovává každý existující vstup do zavazadlového prostoru a vrací se zpět k jakémukoli selhání.</translation>
    </message>
    <message>
        <source>extlinux configuration</source>
        <translation>Konfigurace extlinux</translation>
    </message>
    <message>
        <source>Regenerate extlinux</source>
        <translation>Regenerovat extlinux</translation>
    </message>
    <message>
        <source>Regenerate the selected system&apos;s extlinux bootloader configuration. This legacy frontend exposes no extlinux action.</source>
        <translation>Regenerovat konfiguraci bootloader vybraného systému extlinux. Tento odkaz ukazuje žádnou akci extlinux.</translation>
    </message>
    <message>
        <source>Boot stack reconciliation</source>
        <translation>Sesouhlasení zásobníků bot</translation>
    </message>
    <message>
        <source>Reconcile Boot Stack</source>
        <translation>Reconcile Boot Stack</translation>
    </message>
    <message>
        <source>Reconcile the selected repair system&apos;s boot stack in one guarded legacy pass: mapper/crypttab validation, initramfs rebuild and GRUB-legacy configuration regeneration, with the component backups and preflights unchanged. This is the Etch equivalent of the modern boot-stack reconciliation and stays out of the Full Repair plan.</source>
        <translation>Reconcile zvoleného zavazadlového zásobníku opravárenského systému v jednom hlídaném vstupním průsmyku: validace mapper / crypttab, obnova initramfs a regenerace konfigurace GRUB- dědictví, přičemž zálohy komponent a předlety nezměněny. Toto je Etch ekvivalent moderního boot- stack usmíření a zůstane mimo Full Opravy plánu.</translation>
    </message>
    <message>
        <source>Full Repair plan: Manual recovery tool</source>
        <translation>Kompletní plán oprav: ruční nástroj pro obnovu</translation>
    </message>
    <message>
        <source>Run the guarded boot-stack reconciliation? The helper runs the mapper/crypttab validation, the initramfs rebuild and the GRUB-legacy regeneration in one pass with every component preflight and backup.</source>
        <translation>Spustit střežené usmíření? Pomocník provádí validaci mapper / crypttab, přestavbu initramfs a regeneraci GRUB- dědictví v jednom průsmyku s každou komponentou předletu a zálohování.</translation>
    </message>
    <message>
        <source>Complete interrupted package configuration</source>
        <translation>Kompletní přerušení konfigurace balíku</translation>
    </message>
    <message>
        <source>Repair broken package dependencies</source>
        <translation>Opravy porušených závislostí na balení</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Vytvořit výchozí</translation>
    </message>
    <message>
        <source>Repair file system errors (read-only check first)</source>
        <translation>Oprava chyb v systému souboru (kontrola pouze pro čtení)</translation>
    </message>
    <message>
        <source>the legacy frontend exposes the read-only file system check only; per-device repair is not wired on this frontend (fail closed)</source>
        <translation>odkaz frontend vystavuje pouze kontrolu souborového systému pro čtení pouze; opravy per- zařízení není na tomto frontendu připojena (neúspěch uzavřen)</translation>
    </message>
    <message>
        <source>Upgrade installed packages (adaptive APT simulation)</source>
        <translation>Aktualizovat nainstalované balíky (adaptivní APT simulace)</translation>
    </message>
    <message>
        <source>Rebuild DKMS modules</source>
        <translation>Přestavět moduly DKMS</translation>
    </message>
    <message>
        <source>Restore graphical login manager</source>
        <translation>Obnovit grafický přihlašovací správce</translation>
    </message>
    <message>
        <source>Rebuild initramfs after mapper/crypttab validation</source>
        <translation>Znovu sestavit initramfs po validaci mapper / crypttab</translation>
    </message>
    <message>
        <source>Repair EFI / UKI boot path</source>
        <translation>Oprava zaváděcí cesty EFI / UKI</translation>
    </message>
    <message>
        <source>Update GRUB configuration</source>
        <translation>Aktualizovat konfiguraci GRUB</translation>
    </message>
    <message>
        <source>Update extlinux configuration</source>
        <translation>Aktualizovat konfiguraci extlinux</translation>
    </message>
    <message>
        <source>Choose Full Repair stages in Settings. Enabled stages run in the order shown. Each configurable stage also appears below as an individual tool; the Full Repair column mirrors its current Settings state. The boot tools (EFI / UKI bootloader, GRUB or extlinux configuration, boot-stack reconciliation and Make Default) are independent: run them in any order, and a later action re-verifies what an earlier one changed and reports its own result. The active scope is shown beside Repair: selected repair drive or Running Host maintenance.</source>
        <translation>Zvolte fáze plné opravy v Nastavení. Povolené etapy běží v uvedeném pořadí. Každá konfigurovatelná etapa se také objevuje níže jako individuální nástroj; sloupec Full Opravy odráží aktuální stav nastavení. Boot nástroje (EFI / UKI bootloader, GRUB nebo extlinux konfigurace, boot- stack sesouhlasení a make default) jsou nezávislé: spustit je v libovolném pořadí, a pozdější akce re- ověřuje, co dřívější jeden změnil a hlásí svůj vlastní výsledek. Aktivní rozsah je uveden vedle opravy: vybraný opravárenský pohon nebo údržba Běžící hostitel.</translation>
    </message>
    <message>
        <source>Run All diagnostics for the selected target or running host before starting Full Repair. The report is read-only evidence used to choose and confirm repair stages.</source>
        <translation>Spustit veškerou diagnostiku pro vybraný cíl nebo spuštěný hostitel před spuštěním Kompletní opravy. Zpráva je pouze dokladem, který se používá k výběru a potvrzení fází oprav.</translation>
    </message>
    <message>
        <source>Ready: required cached read-only diagnostics are available for the selected stages. Review them in Diagnostics or Logs before confirming.</source>
        <translation>Připravena: pro vybrané etapy je k dispozici požadovaná diagnostika pouze pro čtení. Před potvrzením si je prohlédněte v diagnostice nebo logech.</translation>
    </message>
    <message>
        <source>Environment validation</source>
        <translation>Ověřování životního prostředí</translation>
    </message>
    <message>
        <source>Summarizes the selected system, protection state, mounted identity and inspection readiness.</source>
        <translation>Shrnuje vybraný systém, stav ochrany, zamontovanou identitu a připravenost na kontrolu.</translation>
    </message>
    <message>
        <source>Distribution and boot backend profile</source>
        <translation>Distribuční a boot backend profil</translation>
    </message>
    <message>
        <source>Identifies the distribution family, package manager, initramfs generator, bootloader and current guarded repair capability.</source>
        <translation>Identifikuje distribuční rodinu, správce balíčků, generátor initramfs, bootloader a aktuální střežené opravy schopnosti.</translation>
    </message>
    <message>
        <source>Boot diagnostics</source>
        <translation>Diagnostika bot</translation>
    </message>
    <message>
        <source>Shows boot mounts and /boot contents plus storage evidence without changing the selected system.</source>
        <translation>Zobrazuje montáž a / boot obsah plus důkaz o uložení bez změny vybraného systému.</translation>
    </message>
    <message>
        <source>Boot evidence and selection history</source>
        <translation>Důkazy a historie výběru</translation>
    </message>
    <message>
        <source>Correlates the detected boot chain, bootloader selection, kernel/initramfs and unlock evidence.</source>
        <translation>Odpovídá zjištěnému řetězci boot, výběru bootloader, jádru / initramfs a odemknout důkazy.</translation>
    </message>
    <message>
        <source>Kernel / initramfs</source>
        <translation>Kernel / initramfs</translation>
    </message>
    <message>
        <source>Reviews kernel files and verifies matching initramfs images through a read-only inspection.</source>
        <translation>Recenze kernel soubory a ověřuje odpovídající initramfs obrázky prostřednictvím kontroly pouze pro čtení.</translation>
    </message>
    <message>
        <source>Reviews the GRUB configuration without changing boot files.</source>
        <translation>Recenze konfigurace GRUB bez změny zaváděcích souborů.</translation>
    </message>
    <message>
        <source>EFI / UKI boot state</source>
        <translation>EFI / UKI stav startu</translation>
    </message>
    <message>
        <source>Inspects EFI/UKI evidence; unavailable on this legacy BIOS frontend with the helper&apos;s probe reason.</source>
        <translation>Inspekce EFI / UKI důkazy; nedostupný na tomto odkazu BIOS frontend s důvodem pomocníka sondy.</translation>
    </message>
    <message>
        <source>Reviews the configured display manager and recent boot evidence without starting the GUI.</source>
        <translation>Recenze nakonfigurovaného manažera displeje a nejnovější boot důkazy bez spuštění GUI.</translation>
    </message>
    <message>
        <source>Boot errors</source>
        <translation>Chyby bota</translation>
    </message>
    <message>
        <source>Reads recent error-priority entries from the running host or selected repair system when available.</source>
        <translation>Čte nedávné chyby-prioritní položky z spuštěného hostitele nebo vybraný opravárenský systém, pokud je k dispozici.</translation>
    </message>
    <message>
        <source>Disk usage</source>
        <translation>Použití disků</translation>
    </message>
    <message>
        <source>Summarizes filesystem capacity and free space for the running host or read-only repair target.</source>
        <translation>Shrnuje kapacitu souborového systému a volný prostor pro běh hostitele nebo read- pouze opravit cíl.</translation>
    </message>
    <message>
        <source>File systems</source>
        <translation>Souborové systémy</translation>
    </message>
    <message>
        <source>Runs the read-only file system check for the selected system&apos;s root, /boot and other filesystems.</source>
        <translation>Spustí kontrolu pouze souborového systému pro kořenový, / boot a další souborový systém vybraného systému.</translation>
    </message>
    <message>
        <source>/etc/fstab review</source>
        <translation>/ etc / fstab review</translation>
    </message>
    <message>
        <source>Displays the running host or selected repair system&apos;s fstab; repair-system inspection is mounted read-only.</source>
        <translation>Zobrazuje běžící hostitele nebo vybraný opravárenský systém fstab; kontrola opravárenského systému je namontována pouze pro čtení.</translation>
    </message>
    <message>
        <source>Btrfs status</source>
        <translation>Stav Btrfs</translation>
    </message>
    <message>
        <source>Shows Btrfs filesystem and subvolume information when the target uses Btrfs.</source>
        <translation>Zobrazuje souborový systém Btrfs a subvolume informace, pokud cíl používá Btrfs.</translation>
    </message>
    <message>
        <source>Device-mapper ancestry</source>
        <translation>Předchůdce Devicemapper</translation>
    </message>
    <message>
        <source>Shows selected mapper ancestry and device-mapper state when available.</source>
        <translation>Zobrazuje vybraný předek mapu a stav zařízení mapper, pokud je k dispozici.</translation>
    </message>
    <message>
        <source>LUKS / crypttab evidence</source>
        <translation>LUKS / krypttab důkaz</translation>
    </message>
    <message>
        <source>Shows LUKS/mapped ancestry plus crypttab and fstab mapper references.</source>
        <translation>Ukazuje LUKS / mapované předky plus krypttab a fstab mapper reference.</translation>
    </message>
    <message>
        <source>Full diagnostic report</source>
        <translation>Kompletní diagnostická zpráva</translation>
    </message>
    <message>
        <source>Combines all read-only diagnostics for the selected scope (same as Run All).</source>
        <translation>Kombinuje veškerou diagnostiku pouze pro čtení pro vybraný rozsah (stejný jako Run All).</translation>
    </message>
    <message>
        <source>All entries</source>
        <translation>Všechny položky</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>Diagnostika</translation>
    </message>
    <message>
        <source>Repairs</source>
        <translation>Opravy</translation>
    </message>
    <message>
        <source>Package repair</source>
        <translation>Oprava balení</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Kopie souboru</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Objev zařízení</translation>
    </message>
    <message>
        <source>Host</source>
        <translation>Hostitel</translation>
    </message>
    <message>
        <source>Required for block-device inventory</source>
        <translation>Požadováno pro inventuru block-zařízení</translation>
    </message>
    <message>
        <source>Filesystem identification</source>
        <translation>Identifikace souborového systému</translation>
    </message>
    <message>
        <source>Used to identify filesystem metadata</source>
        <translation>Používá se k identifikaci metadat souborového systému</translation>
    </message>
    <message>
        <source>Mount inspection</source>
        <translation>Kontrola montáže</translation>
    </message>
    <message>
        <source>Used to understand active mounts</source>
        <translation>Používá se k pochopení aktivních zařízení</translation>
    </message>
    <message>
        <source>LUKS support</source>
        <translation>Podpora LUKS</translation>
    </message>
    <message>
        <source>Required to unlock encrypted targets</source>
        <translation>Vyžadováno k odemknutí zašifrovaných cílů</translation>
    </message>
    <message>
        <source>Btrfs support</source>
        <translation>Podpora Btrfs</translation>
    </message>
    <message>
        <source>Required for Btrfs inspection and snapshot rollback</source>
        <translation>Vyžadováno pro kontrolu Btrfs a zpětnou vazbu</translation>
    </message>
    <message>
        <source>Bidirectional file copy</source>
        <translation>Dvousměrná kopie souboru</translation>
    </message>
    <message>
        <source>Host/Repair</source>
        <translation>Hostitel / Opravy</translation>
    </message>
    <message>
        <source>Required for verified Host-to-Repair and Repair-to-Host transfer</source>
        <translation>Požadováno pro ověřený Host- to- Opravy a Repair- to- Host transfer</translation>
    </message>
    <message>
        <source>Chroot repair</source>
        <translation>Oprava kořenů</translation>
    </message>
    <message>
        <source>Required for target-side repair commands</source>
        <translation>Vyžadováno pro příkazy k opravě na cílovou stranu</translation>
    </message>
    <message>
        <source>Offline systemd repair</source>
        <translation>Offline opravy systému</translation>
    </message>
    <message>
        <source>Used to restore graphical.target and the configured display manager without starting the target GUI</source>
        <translation>Používá se k obnově grafiky. cíl a nakonfigurovaný správce displeje bez spuštění cílového GUI</translation>
    </message>
    <message>
        <source>UEFI NVRAM inspection</source>
        <translation>UEFI NVRAM inspekce</translation>
    </message>
    <message>
        <source>Used to preserve target EFI BootOrder during TUXEDO UKI rebuilds when efivars are available</source>
        <translation>Používá se k zachování cíle EFI BootOrder během přestavby TUXEDO UKI, když jsou efivars k dispozici</translation>
    </message>
    <message>
        <source>UKI verification</source>
        <translation>Ověření UKI</translation>
    </message>
    <message>
        <source>Used to verify the kernel embedded in a rebuilt unified kernel image</source>
        <translation>Používá se k ověření jádra vloženého do přestavěného jednotného jádra obrazu</translation>
    </message>
    <message>
        <source>GRUB EFI repair</source>
        <translation>Oprava GRUB EFI</translation>
    </message>
    <message>
        <source>Target/Host</source>
        <translation>Cíl / hostitel</translation>
    </message>
    <message>
        <source>Required only for conventional GRUB-based EFI systems</source>
        <translation>Vyžadováno pouze pro konvenční systémy EFI založené na GRUB</translation>
    </message>
    <message>
        <source>Target</source>
        <translation>Cíl</translation>
    </message>
    <message>
        <source>Debian-family GRUB helper</source>
        <translation>Debian- family GRUB helper</translation>
    </message>
    <message>
        <source>Portable GRUB configuration generator used by Arch and other non-Debian systems</source>
        <translation>Přenosný konfigurační generátor GRUB používaný systémy Arch a dalšími non-Debian</translation>
    </message>
    <message>
        <source>Initramfs rebuild</source>
        <translation>Initramfs obnova</translation>
    </message>
    <message>
        <source>Debian-family initramfs helper</source>
        <translation>Debian- family initramfs helper</translation>
    </message>
    <message>
        <source>Arch-family initramfs generator</source>
        <translation>Generátor arch- family initramfs</translation>
    </message>
    <message>
        <source>Alternative initramfs generator used by Arch and other distributions</source>
        <translation>Alternativní generátor initramfs používaný u Arch a jiných distribucí</translation>
    </message>
    <message>
        <source>Initramfs verification</source>
        <translation>Ověření na místě</translation>
    </message>
    <message>
        <source>Read-only verification for mkinitcpio images</source>
        <translation>Ověření pouze pro obrázky mkinitcpio</translation>
    </message>
    <message>
        <source>Read-only verification for dracut images</source>
        <translation>Ověření pouze pro obrázky dracut</translation>
    </message>
    <message>
        <source>systemd-boot inspection</source>
        <translation>Kontrola systemd-boot</translation>
    </message>
    <message>
        <source>Host/Target</source>
        <translation>Hostitel / Cíl</translation>
    </message>
    <message>
        <source>Read-only inspection of systemd-boot and generic UKI layouts</source>
        <translation>Read- only kontrola systemd-boot a generic UKI rozložení</translation>
    </message>
    <message>
        <source>Arch package manager</source>
        <translation>Správce balíčku Arch</translation>
    </message>
    <message>
        <source>Arch-family package database and transaction tool</source>
        <translation>Databáze arch- family balíčků a transakční nástroj</translation>
    </message>
    <message>
        <source>DKMS rebuild</source>
        <translation>Přestavba DKMS</translation>
    </message>
    <message>
        <source>Required only when target uses DKMS modules</source>
        <translation>Vyžaduje se pouze při použití modulů DKMS.</translation>
    </message>
    <message>
        <source>LVM inspection</source>
        <translation>Kontrola LVM</translation>
    </message>
    <message>
        <source>Optional LVM storage-stack support</source>
        <translation>Volitelná podpora LVM skladu</translation>
    </message>
    <message>
        <source>Software RAID</source>
        <translation>Software RAID</translation>
    </message>
    <message>
        <source>Optional Linux MD RAID support</source>
        <translation>Volitelná podpora Linux MD RAID</translation>
    </message>
    <message>
        <source>Process namespace isolation</source>
        <translation>Izolace jmenného prostoru procesu</translation>
    </message>
    <message>
        <source>Host+Target</source>
        <translation>Hostitel + Cíl</translation>
    </message>
    <message>
        <source>The ported helper falls back to a guarded plain chroot when unshare is absent</source>
        <translation>Přenášející se pomocník se vrací zpět k hlídanému prostému jádru, když není žádný podíl.</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Zrušit</translation>
    </message>
    <message>
        <source>OK</source>
        <translation>Dobře.</translation>
    </message>
    <message>
        <source>Yes</source>
        <translation>Ano.</translation>
    </message>
    <message>
        <source>No</source>
        <translation>Ne.</translation>
    </message>
    <message>
        <source>read-only helper diagnostic</source>
        <translation>dead- only helper diagnostics</translation>
    </message>
</context>
<context>
    <name>legacy::LegacyMainWindow</name>
    <message>
        <source>Boot Bitch Legacy (Etch / KDE 3.5 era)</source>
        <translation>Boot Bitch Legacy (Etch / KDE 3.5 éra)</translation>
    </message>
    <message>
        <source>Linux recovery and boot-repair utility</source>
        <translation>Linux recovery a boot- oprava utility</translation>
    </message>
    <message>
        <source>GUARDED REPAIR</source>
        <translation>ZÁRUČNÍ REPAIR</translation>
    </message>
    <message>
        <source>Ordinary repairs require an explicitly selected non-host target. The protected running host has a separate deliberate maintenance mode with the same guarded repair stages and requires privilege authorization.</source>
        <translation>Běžné opravy vyžadují výslovně vybraný nehostitelský cíl. Chráněný pojízdný hostitel má samostatný režim záměrné údržby se stejnými hlídanými fázemi oprav a vyžaduje oprávnění k výsadě.</translation>
    </message>
    <message>
        <source>Systems</source>
        <translation>Systémy</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>Diagnostika</translation>
    </message>
    <message>
        <source>Repair</source>
        <translation>Opravy</translation>
    </message>
    <message>
        <source>Chroot Shell</source>
        <translation>Chroot Shell</translation>
    </message>
    <message>
        <source>File Copy</source>
        <translation>Kopírovat soubor</translation>
    </message>
    <message>
        <source>Logs</source>
        <translation>Záznamy</translation>
    </message>
    <message>
        <source>Settings</source>
        <translation>Nastavení</translation>
    </message>
    <message>
        <source>(not created yet)</source>
        <translation>(dosud nebyl vytvořen)</translation>
    </message>
    <message>
        <source>Information</source>
        <translation>Informace</translation>
    </message>
    <message>
        <source>Close</source>
        <translation>Zavřít</translation>
    </message>
    <message>
        <source>&amp;File</source>
        <translation>&amp;Soubor</translation>
    </message>
    <message>
        <source>&amp;Refresh Devices</source>
        <translation>&amp;Obnovovací zařízení</translation>
    </message>
    <message>
        <source>&amp;Lock Administrator Session</source>
        <translation>Administrátorské zasedání &amp;Lock</translation>
    </message>
    <message>
        <source>&amp;Quit</source>
        <translation>&amp;Ukončení</translation>
    </message>
    <message>
        <source>&amp;View</source>
        <translation>&amp;pohled</translation>
    </message>
    <message>
        <source>&amp;Systems</source>
        <translation>&amp;systémy</translation>
    </message>
    <message>
        <source>&amp;Diagnostics</source>
        <translation>&amp;Diagnostika</translation>
    </message>
    <message>
        <source>&amp;Logs</source>
        <translation>&amp;záznamy</translation>
    </message>
    <message>
        <source>&amp;Settings</source>
        <translation>&amp;Nastavení</translation>
    </message>
    <message>
        <source>&amp;Auto-size Device Columns</source>
        <translation>&amp;Automatické velikosti Sloupce zařízení</translation>
    </message>
    <message>
        <source>&amp;Wrap Log Lines</source>
        <translation>&amp;Záznamy</translation>
    </message>
    <message>
        <source>&amp;Help</source>
        <translation>&amp;Nápověda</translation>
    </message>
    <message>
        <source>&amp;Using Boot Bitch</source>
        <translation>&amp;Using Boot Bitch</translation>
    </message>
    <message>
        <source>&amp;About Boot Bitch</source>
        <translation>&amp;O Boot Bitch</translation>
    </message>
    <message>
        <source>Device columns auto-sized. Drag headers to fine-tune widths.</source>
        <translation>Zařízení sloupky autovelikost. Přetáhněte hlavičky do jemných šířek.</translation>
    </message>
    <message>
        <source>A helper command is running; wait for it to finish before locking the session.</source>
        <translation>Povel pomocníka běží; počkejte, až to skončí před uzamčením session.</translation>
    </message>
    <message>
        <source>Administrator session locked; the next privileged action will request authorization.</source>
        <translation>Administrátorské zasedání uzamčeno; další privilegovaná akce bude vyžadovat povolení.</translation>
    </message>
    <message>
        <source>Using Boot Bitch</source>
        <translation>Použití Boot Bitch</translation>
    </message>
    <message>
        <source>Boot Bitch must run from a different booted Linux environment than the system being repaired. Use a Linux live medium or another Linux installation on a different physical drive.&lt;br&gt;&lt;br&gt;The running host is protected from ordinary repair-target selection, but it can be explicitly selected through &lt;b&gt;Host Maintenance&lt;/b&gt; for guarded native diagnostics and supported maintenance stages.&lt;br&gt;&lt;br&gt;Diagnostics follow the Systems page: the committed repair drive while Host Maintenance is off, or the protected running host while it is active.&lt;br&gt;&lt;br&gt;The first privileged action requests administrator authorization once for this Boot Bitch window; &lt;b&gt;File - Lock Administrator Session&lt;/b&gt; ends that helper session immediately. Every repair keeps the helper&apos;s own runtime preflights.</source>
        <translation>Boot Bitch musí běžet z jiného prostředí Linux, než je opravené zařízení. Použijte živé médium Linux nebo jinou instalaci Linux na jiný fyzický pohon. &lt; br &gt; &lt; br &gt; Běžící hostitel je chráněn před běžným výběrem opravárenských cílů, ale může být explicitně vybrán pomocí &lt; b &gt; Hostitelské údržby &lt; / b &gt; pro hlídané přirozené diagnostiky a podporované fáze údržby. &lt; br &gt; &lt; br &gt; Diagnostika se řídí záložkou Systems: oddaný opravárenský disk při vypnutí Host Maintenance, nebo chráněný pojízdný hostitel, když je aktivní. &lt; br &gt; &lt; br &gt; První privilegovaná akce žádá administrátora o autorizaci jednou pro toto okno Boot Bitch; &lt; b &gt; Soubor - Session správce Lock &lt; / b &gt; okamžitě ukončí tuto pomocné relaci. Každá oprava udržuje pomocníkovy vlastní předlety.</translation>
    </message>
    <message>
        <source>Select a physical drive; Boot Bitch resolves the most likely Linux system volume automatically. The running host stays protected from ordinary target repairs, with a separate explicit host-maintenance path for its own system. The Details button shows the protected host&apos;s facts in the details pane; selecting any drive row restores the per-drive pane.</source>
        <translation>Vyberte fyzický pohon; Boot Bitch automaticky vyřeší nejpravděpodobnější objem systému Linux. Běžící hostitel zůstává chráněn před běžnými opravami cíle, se samostatnou explicitní udržovací dráhou pro vlastní systém. Tlačítko Detaily zobrazuje údaje chráněného hostitele v panelu detailů; volba libovolného řádku mechaniky obnovuje panel per- drive.</translation>
    </message>
    <message>
        <source>Refresh Devices</source>
        <translation>Obnovit zařízení</translation>
    </message>
    <message>
        <source>Re-read the read-only kernel inventory (/proc/partitions, /proc/mounts, /proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* and the udev metadata database). No block device is opened and nothing is written.</source>
        <translation>Přečti si inventuru pouze pro čtení (/ proc / partions, / proc / monts, / proc / swaps, / sys / block, / dev / mapper, / dev / disk / by- * a udev metadata databáze). Žádné blokové zařízení není otevřeno a nic není napsáno.</translation>
    </message>
    <message>
        <source>[OK]</source>
        <translation>[OK]</translation>
    </message>
    <message>
        <source>The running system was detected and stays protected from ordinary target repairs.</source>
        <translation>Běžecký systém byl detekován a zůstává chráněn před běžnými opravami cíle.</translation>
    </message>
    <message>
        <source>Detecting running system...</source>
        <translation>Detekuji běžecký systém...</translation>
    </message>
    <message>
        <source>Detecting protected storage...</source>
        <translation>Detekuji chráněné úložiště...</translation>
    </message>
    <message>
        <source>PROTECTED</source>
        <translation>OCHRANA</translation>
    </message>
    <message>
        <source>The running host remains protected from ordinary repair-target operations.</source>
        <translation>Běžící hostitel zůstává chráněn před běžnými opravárenskými operacemi.</translation>
    </message>
    <message>
        <source>Details</source>
        <translation>Podrobnosti</translation>
    </message>
    <message>
        <source>Show read-only details for the protected running host.</source>
        <translation>Zobrazit pouze údaje pro chráněný běžící hostitel.</translation>
    </message>
    <message>
        <source>Host Maintenance</source>
        <translation>Údržba hostitele</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Vytvořit výchozí</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host (menu.lst default directive with a backup and rollback). Requires Host Maintenance and the cached host-default probe.</source>
        <translation>Udělat kanonické nainstalované jádro vstup výchozí GRUB-odkaz boot vstup na spuštěném hostiteli (menu.lst výchozí směrnice s zálohou a rollback). Vyžaduje Host Údržba a cached host- výchozí sonda.</translation>
    </message>
    <message>
        <source>Available repair targets</source>
        <translation>Dostupné cíle oprav</translation>
    </message>
    <message>
        <source>Most likely first</source>
        <translation>Nejpravděpodobněji první</translation>
    </message>
    <message>
        <source>Drives are listed by the read-only inventory. Select a row to inspect it; Select Target commits the selected non-host drive with its auto-resolved Linux root component.</source>
        <translation>Pohon je uveden v inventuře pouze pro čtení. Vyberte řádek pro jeho kontrolu; zvolte Target odešle vybraný non-host disk s jeho automaticky vyřešenou kořenovou složkou Linux.</translation>
    </message>
    <message>
        <source>Device</source>
        <translation>Zařízení</translation>
    </message>
    <message>
        <source>Size</source>
        <translation>Velikost</translation>
    </message>
    <message>
        <source>Type</source>
        <translation>Typ</translation>
    </message>
    <message>
        <source>Filesystem</source>
        <translation>Filesystem</translation>
    </message>
    <message>
        <source>Select Target</source>
        <translation>Vybrat cíl</translation>
    </message>
    <message>
        <source>Unlock</source>
        <translation>Odemknout</translation>
    </message>
    <message>
        <source>Authorize</source>
        <translation>Autorizace</translation>
    </message>
    <message>
        <source>Establish the privileged helper session for the current scope now instead of waiting for the next privileged action.</source>
        <translation>Uspořádat privilegované pomocné zasedání pro stávající oblast působnosti nyní místo čekání na další privilegované akce.</translation>
    </message>
    <message>
        <source>Committed target: none</source>
        <translation>Zadaný cíl: žádný</translation>
    </message>
    <message>
        <source>Unlock state for the selected drive; the LUKS passphrase is never logged.</source>
        <translation>Odemknout stav pro vybraný disk; heslo LUKS se nikdy nezaznamenává.</translation>
    </message>
    <message>
        <source>Unlock status</source>
        <translation>Stav odemykání</translation>
    </message>
    <message>
        <source>Read-only inventory plus helper-confirmed facts; mirrors the modern Qt6 Selected drive details panel.</source>
        <translation>Read- pouze inventář plus helper- potvrdil fakta; odráží moderní Qt6 Vybrané drive detail panel.</translation>
    </message>
    <message>
        <source>Selected drive details</source>
        <translation>Vybrané údaje o řízení</translation>
    </message>
    <message>
        <source>Field</source>
        <translation>Pole</translation>
    </message>
    <message>
        <source>Value</source>
        <translation>Hodnota</translation>
    </message>
    <message>
        <source>Run All runs every available read-only diagnostic for the current scope; selecting a check runs it alone. Diagnostics are read-only and are the only evidence source for the gated repair actions.</source>
        <translation>Spustit Vše běží každou dostupnou diagnostiku pouze pro aktuální rozsah; volba kontroly běží sám. Diagnostika je pouze pro čtení a je jediným zdrojem důkazů pro uzavřené opravy.</translation>
    </message>
    <message>
        <source>Target: none selected</source>
        <translation>Cíl: žádné vybrané</translation>
    </message>
    <message>
        <source>Diagnostics follow the committed repair target, or the protected running host while Host Maintenance is active.</source>
        <translation>Diagnostika se řídí závazným cílem opravy nebo chráněným provozním hostitelem, zatímco je Hostitelská údržba aktivní.</translation>
    </message>
    <message>
        <source>Run All</source>
        <translation>Spustit vše</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope.</source>
        <translation>Spustit vše - spustit každou dostupnou diagnostiku pouze pro aktuální rozsah.</translation>
    </message>
    <message>
        <source>Target configuration:</source>
        <translation>Konfigurace cíle:</translation>
    </message>
    <message>
        <source>Etch-era target configuration files; availability is probed read-only by the helper&apos;s diagnostics.</source>
        <translation>Konfigurační soubory Etch-era; dostupnost je kontrolována pouze diagnostikou pomocníka.</translation>
    </message>
    <message>
        <source>Edit Target File...</source>
        <translation>Upravit cílový soubor...</translation>
    </message>
    <message>
        <source>Runs one read-only diagnostic for the selected scope through the helper (`diagnose &lt;key&gt;` / `host-diagnose &lt;key&gt;`); Run All is the combined report.</source>
        <translation>Provede jednu diagnostiku pouze pro zvolený rozsah pomocí pomocníka (&apos;diagnose &lt; key &gt;&apos; / &apos;host- diagnose &lt; key &gt;&apos;); Spustit vše je kombinovaná zpráva.</translation>
    </message>
    <message>
        <source>Diagnostic checks</source>
        <translation>Diagnostické kontroly</translation>
    </message>
    <message>
        <source>Check</source>
        <translation>Kontrola</translation>
    </message>
    <message>
        <source>Selected diagnostic</source>
        <translation>Vybraná diagnostika</translation>
    </message>
    <message>
        <source>Select a diagnostic</source>
        <translation>Vyberte diagnostiku</translation>
    </message>
    <message>
        <source>Choose a diagnostic from the list.</source>
        <translation>Vyberte si diagnostiku ze seznamu.</translation>
    </message>
    <message>
        <source>Ready</source>
        <translation>Připraven</translation>
    </message>
    <message>
        <source>Results</source>
        <translation>Výsledky</translation>
    </message>
    <message>
        <source>Run Diagnostic</source>
        <translation>Spustit diagnostiku</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic for the current scope.</source>
        <translation>Spustit diagnostiku - spustit vybranou diagnostiku pouze pro aktuální rozsah.</translation>
    </message>
    <message>
        <source>Copy Results</source>
        <translation>Kopírovat výsledky</translation>
    </message>
    <message>
        <source>Save Results...</source>
        <translation>Uložit výsledky...</translation>
    </message>
    <message>
        <source>The Full Repair plan runs the selected legacy stages in order through the guarded helper; the individual tools run one stage at a time. Every action stays disabled until the cached capability lines say available and the helper keeps its runtime preflights.</source>
        <translation>Plán Full Repair vede vybrané statické fáze v pořadí přes hlídaného pomocníka; jednotlivé nástroje běží po etapě. Každá akce zůstává zakázána, dokud cached schopnosti linky říkají k dispozici a pomocník udržuje své runtime předlety.</translation>
    </message>
    <message>
        <source>Full Repair plan</source>
        <translation>Kompletní plán oprav</translation>
    </message>
    <message>
        <source>No stages selected</source>
        <translation>Není vybráno žádné stadium</translation>
    </message>
    <message>
        <source>Configure Plan...</source>
        <translation>Nastavit plán...</translation>
    </message>
    <message>
        <source>Open Settings to choose which Full Repair stages are part of the plan.</source>
        <translation>Otevřená nastavení pro volbu, které fáze Úplné opravy jsou součástí plánu.</translation>
    </message>
    <message>
        <source>Run Full Repair</source>
        <translation>Spustit kompletní opravy</translation>
    </message>
    <message>
        <source>Select a repair drive, or choose Host Maintenance on the protected running-host card.</source>
        <translation>Vyberte opravárenský disk, nebo vyberte Host Údržba na chráněné running- hostitelské karty.</translation>
    </message>
    <message>
        <source>Stage</source>
        <translation>Fáze</translation>
    </message>
    <message>
        <source>Individual repair tools</source>
        <translation>Nářadí pro individuální opravy</translation>
    </message>
    <message>
        <source>Tool</source>
        <translation>Nástroj</translation>
    </message>
    <message>
        <source>Full Repair</source>
        <translation>Úplné opravy</translation>
    </message>
    <message>
        <source>not reported</source>
        <translation>nehlášené</translation>
    </message>
    <message>
        <source>Selected tool</source>
        <translation>Vybraný nástroj</translation>
    </message>
    <message>
        <source>Select a repair tool</source>
        <translation>Vyberte nástroj pro opravy</translation>
    </message>
    <message>
        <source>Run Tool</source>
        <translation>Spustit nástroj</translation>
    </message>
    <message>
        <source>Select a tool to review its repair action.</source>
        <translation>Vyberte nástroj pro přezkoumání jeho opravy akce.</translation>
    </message>
    <message>
        <source>Write actions ask for confirmation and then run the helper&apos;s own runtime preflights; the GUI never weakens them. A repair that is not proven &apos;unchanged&apos; invalidates the cached diagnostics and disables the gated actions until diagnostics run again.</source>
        <translation>Psaní akce požádat o potvrzení a pak spustit pomocník vlastní runtime předlety; GUI nikdy oslabuje je. Oprava, která se neprokáže jako &quot;nezměněná&quot;, znehodnocuje cached diagnostiku a vyřazuje uzavřené akce, dokud diagnostika znovu nezačne.</translation>
    </message>
    <message>
        <source>Chroot shell</source>
        <translation>Chroot shell</translation>
    </message>
    <message>
        <source>Offline commands run one at a time in a fresh chroot and cannot answer interactive prompts (apt-get -y upgrade works). Host-shell commands run directly on the running host. The helper&apos;s probe lines gate the command field; the exact reason appears in the tooltip.</source>
        <translation>Příkazy Offline běží jeden po druhém v čerstvém chroot a nemůže odpovědět interaktivní podněty (apt- get -y upgrade funguje). Příkazy Host- shell běží přímo na spuštěném hostiteli. Pomocná sonda vede bránou povelové pole; přesný důvod se objeví v nástrojové špičce.</translation>
    </message>
    <message>
        <source>No &apos;Legacy feature shell:&apos; line is cached; run diagnostics for the selected scope to evaluate the helper&apos;s chroot/timeout containment probes (fail closed).</source>
        <translation>Ne &apos;Legacy funkce shell:&apos; řádek je cached; spustit diagnostiku pro zvolený rozsah pro vyhodnocení chroot / timeout controlling sond pomocníka (self closed).</translation>
    </message>
    <message>
        <source>Command</source>
        <translation>Příkaz</translation>
    </message>
    <message>
        <source>Command:</source>
        <translation>Příkaz:</translation>
    </message>
    <message>
        <source>One reviewed command string, passed to the helper as a single argument (no shell interpolation by the GUI).</source>
        <translation>Jeden revidovaný příkazový řetězec, předán pomocníkovi jako jeden argument (bez interpolace shell by the GUI).</translation>
    </message>
    <message>
        <source>Run Command</source>
        <translation>Spustit příkaz</translation>
    </message>
    <message>
        <source>Clear Output</source>
        <translation>Vyčistit výstup</translation>
    </message>
    <message>
        <source>The helper exposes `shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for an offline target chroot and `host-shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for the running host. Both keep the helper&apos;s runtime preflights; this tab enables the command only when the scope is committed, the session is authorized and the scope&apos;s Legacy feature probe reports available.</source>
        <translation>Pomocník zobrazí &quot;shell&quot; &lt; disk &gt; &lt; root &gt; &lt; příkaz &gt; &quot;pro offline cílový chroot a&quot; host- shell &lt; disk &gt; &lt; root &gt; &lt; příkaz &gt; &quot;pro běžící hostitel. Oba si ponechejte předlety na runtime; tato záložka umožňuje příkaz pouze v případě, že je spáchán rozsah, relace je povolena a funkce aplikace Legacy je k dispozici.</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Kopie souboru</translation>
    </message>
    <message>
        <source>Copy and verify files in either direction through the guarded helper (cp -a plus ownership restoration and a per-file byte-compare). The helper&apos;s file-copy probe gates the controls and keeps the direction and path containment checks.</source>
        <translation>Kopírovat a ověřovat soubory v obou směrech prostřednictvím hlídaného pomocníka (cp -a plus obnovení vlastnictví a per- file byte- compare). Pomocníkova kopírovací sonda ovládá ovládání a udržuje kontrolu směru a cesty.</translation>
    </message>
    <message>
        <source>Preview Changes</source>
        <translation>Náhled změn</translation>
    </message>
    <message>
        <source>Run a copy dry-run through the guarded helper. No files are changed.</source>
        <translation>Projeď kopii sušákem přes hlídaného pomocníka. Žádné soubory se nemění.</translation>
    </message>
    <message>
        <source>Copy and Verify</source>
        <translation>Kopírovat a ověřovat</translation>
    </message>
    <message>
        <source>Copy staged items and verify the result. Existing destination names are overwritten when source content differs; unrelated destination files are never deleted.</source>
        <translation>Kopírovat nahrané položky a ověřit výsledek. Stávající názvy míst určení jsou přepsány, pokud se obsah zdroje liší; nesouvisející cílové soubory se nikdy nevymažou.</translation>
    </message>
    <message>
        <source>Direction:</source>
        <translation>Směr:</translation>
    </message>
    <message>
        <source>Choose which system supplies the source files and which system receives them.</source>
        <translation>Vyberte si, který systém dodává zdrojové soubory a který systém je přijímá.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from this host</source>
        <translation>1. Vyberte zdrojové soubory nebo složky z tohoto hostitele</translation>
    </message>
    <message>
        <source>Source</source>
        <translation>Zdroj</translation>
    </message>
    <message>
        <source>Files and folders staged for the verified copy. The legacy backend copies with cp -a and restores ownership with chown --reference; every regular file is byte-compared after the copy.</source>
        <translation>Soubory a složky pro ověřenou kopii. Odkaz backend kopie s cp -a a obnovuje vlastnictví s chown --reference; každý pravidelný soubor je byte- ve srovnání s kopií.</translation>
    </message>
    <message>
        <source>Add Files...</source>
        <translation>Přidat soubory...</translation>
    </message>
    <message>
        <source>Add Folder...</source>
        <translation>Přidat složku...</translation>
    </message>
    <message>
        <source>Remove</source>
        <translation>Odstranit</translation>
    </message>
    <message>
        <source>Clear</source>
        <translation>Vyčistit</translation>
    </message>
    <message>
        <source>Clear the staged source list (nothing is copied or deleted).</source>
        <translation>Vymazat seznam nahraných zdrojů (nic není zkopírováno nebo smazáno).</translation>
    </message>
    <message>
        <source>2. Choose destination in repaired system</source>
        <translation>2. Vyberte si místo určení v opraveném systému</translation>
    </message>
    <message>
        <source>unavailable: see the helper&apos;s Legacy feature file-copy: probe reason above</source>
        <translation>nedostupný: viz soubor Legacy funkce pomocníka: důvod sondy výše</translation>
    </message>
    <message>
        <source>An absolute path inside the selected repair system (Host to Repair) or on the running host (Repair to Host).</source>
        <translation>Absolutní cesta uvnitř vybraného opravárenského systému (Host to Repair) nebo na běžícím hostiteli (Opravy na Host).</translation>
    </message>
    <message>
        <source>Browse Target Folders...</source>
        <translation>Procházet cílové složky...</translation>
    </message>
    <message>
        <source>Browse the selected repair system through the helper&apos;s temporary read-only mounts and choose an absolute destination path. No target files are changed while browsing.</source>
        <translation>Projděte vybraný opravárenský systém dočasným nastavením a vyberte absolutní cílovou cestu. Žádné cílové soubory se při prohlížení nemění.</translation>
    </message>
    <message>
        <source>3. Ownership and copy policy</source>
        <translation>3. Vlastnická a kopírovací politika</translation>
    </message>
    <message>
        <source>Ownership:</source>
        <translation>Vlastnictví:</translation>
    </message>
    <message>
        <source>Smart destination ownership (recommended)</source>
        <translation>Chytré vlastnictví místa určení (doporučeno)</translation>
    </message>
    <message>
        <source>Preserve source numeric UID/GID</source>
        <translation>Zachovat číselný zdroj UID / GID</translation>
    </message>
    <message>
        <source>Smart mode validates UID/GID identity mapping across the two systems and falls back to the destination-directory owner when the same numeric ID means a different account (the legacy backend implements it with chown --reference).</source>
        <translation>Smart mode validuje mapování identity UID / GID přes oba systémy a vrací se zpět k majiteli adresáře destination-, když stejné numerické ID znamená jiný účet (odkaz backend implementuje s vybroušený --reference).</translation>
    </message>
    <message>
        <source>Application log</source>
        <translation>Záznam aplikace</translation>
    </message>
    <message>
        <source>The complete session register; Save As... writes every entry even while a filter hides lines. If a writable system share mount exists at /host, Save As... starts there; otherwise the log directory is the fallback. Prior session files are listed read-only.</source>
        <translation>Kompletní relace registr; Uložit jako... píše každý záznam i když filtr skrývá řádky. Pokud existuje zapisovatelný systémový sdílený modul na / hostingu, Zde začíná Uložit jako...; jinak je adresář záznamu záloha. Soubory předchozí relace jsou uvedeny pouze pro čtení.</translation>
    </message>
    <message>
        <source>Session logs</source>
        <translation>Záznamy zasedání</translation>
    </message>
    <message>
        <source>Session</source>
        <translation>Sezení</translation>
    </message>
    <message>
        <source>The first entry is the live session; earlier files in the log directory are listed read-only below it.</source>
        <translation>První záznam je live session; starší soubory v adresáři záznamu jsou uvedeny pouze na číslech.</translation>
    </message>
    <message>
        <source>New Session Log</source>
        <translation>Nový protokol zasedání</translation>
    </message>
    <message>
        <source>Close the active session file; it becomes a prior session and the next log entry starts a new file.</source>
        <translation>Zavřít aktivní soubor relace; stane se z něj předchozí relace a další zápis záznamu spustí nový soubor.</translation>
    </message>
    <message>
        <source>Add Note</source>
        <translation>Přidat poznámku</translation>
    </message>
    <message>
        <source>Append a NOTE entry to the live session register.</source>
        <translation>Připojit položku POZN do živého registru zasedání.</translation>
    </message>
    <message>
        <source>Delete</source>
        <translation>Smazat</translation>
    </message>
    <message>
        <source>Delete the selected prior session file (the live session is never deleted).</source>
        <translation>Smazat vybraný předchozí soubor session (live session není nikdy smazán).</translation>
    </message>
    <message>
        <source>Refresh</source>
        <translation>Obnovit</translation>
    </message>
    <message>
        <source>Save As...</source>
        <translation>Uložit jako...</translation>
    </message>
    <message>
        <source>Save the complete session log (all entries, not just the current filter).</source>
        <translation>Uložit kompletní záznam relace (všechny položky, nejen současný filtr).</translation>
    </message>
    <message>
        <source>Clear Register</source>
        <translation>Vymazat registr</translation>
    </message>
    <message>
        <source>Clear the live register and view; prior session files are never modified.</source>
        <translation>Vymazat live registr a pohled; předchozí session soubory se nikdy nemění.</translation>
    </message>
    <message>
        <source>Search log:</source>
        <translation>Log hledání:</translation>
    </message>
    <message>
        <source>Type any characters to show matching log entries (case-insensitive). Save As always writes every entry.</source>
        <translation>Zadejte jakékoli znaky pro zobrazení odpovídajících záznamů záznamu (case- insensitive). Uložit Jako vždy píše každý záznam.</translation>
    </message>
    <message>
        <source>Filter:</source>
        <translation>Filtr:</translation>
    </message>
    <message>
        <source>Filter the visible log by entry kind. Selecting a diagnostic section shows the lines captured for that section; a workflow filter such as File system repair or Package repair shows its mapped repair lines (File copy has no lines in this frontend). Save As always writes every entry.</source>
        <translation>Filtrovat viditelné logaritmus podle druhu záznamu. Výběr diagnostické sekce ukazuje linky zachycené pro tento oddíl; filtr workflow, jako je oprava souborového systému nebo oprava Schránky, zobrazuje své mapované opravy (Kopie souboru nemá žádné linky v této frontě). Uložit Jako vždy píše každý záznam.</translation>
    </message>
    <message>
        <source>Settings are stored per user under ~/.qt/, one file per settings group (devicesrc, logsrc, diagnosticsrc, repairrc), and are saved immediately on every change and on close. Launch the GUI as the same user to keep your overrides; a GUI started as root keeps its own copies.</source>
        <translation>Nastavení je uloženo na jednoho uživatele pod ~ / .qt /, jeden soubor na skupinu nastavení (devicesrc, logsrc, diagnosticsrc, oprava) a je okamžitě uloženo na každou změnu a na konci. Spusťte GUI jako stejný uživatel, aby vaše přejetí; GUI začal jako root udržuje své vlastní kopie.</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Objev zařízení</translation>
    </message>
    <message>
        <source>Show devices without an identified Linux installation</source>
        <translation>Zobrazovat zařízení bez identifikované instalace Linux</translation>
    </message>
    <message>
        <source>Show removable and USB storage</source>
        <translation>Zobrazit odnímatelné a USB úložiště</translation>
    </message>
    <message>
        <source>Show encrypted devices before unlocking</source>
        <translation>Zobrazit zašifrovaná zařízení před odemknutím</translation>
    </message>
    <message>
        <source>When off, drives without a visible Linux filesystem are hidden unless they still contain an encrypted device and encrypted devices are shown.</source>
        <translation>Po vypnutí jsou disky bez viditelného Linux souborového systému skryty, pokud stále neobsahují zašifrované zařízení a jsou zobrazeny.</translation>
    </message>
    <message>
        <source>When off, removable and USB drives are hidden from the repair-target list.</source>
        <translation>Po vypnutí jsou odnímatelné a USB disky skryty ze seznamu opravárenských cílů.</translation>
    </message>
    <message>
        <source>When off, drives with an encrypted device are hidden until the volume is unlocked.</source>
        <translation>Po vypnutí jsou disky se zašifrovaným zařízením skryty, dokud není hlasitost odemčena.</translation>
    </message>
    <message>
        <source>Include the %1 stage in the Full Repair plan. The stage runs in the plan order shown on the Repair tab.</source>
        <translation>Zahrňte fázi %1 do plánu plné opravy. Stupeň běží v pořadí plánu uvedeném na záložce Opravy.</translation>
    </message>
    <message>
        <source>Automatically regenerate read-only diagnostics after repairs or target changes</source>
        <translation>Automaticky regenerovat diagnostiku pouze pro čtení po opravách nebo změnách cíle</translation>
    </message>
    <message>
        <source>Regenerates the cached read-only diagnostics for the current scope after an operation that invalidates them (LUKS unlock, target configuration edit). It runs only inside an already authorized administrator session and never opens an authorization prompt by itself.</source>
        <translation>Po operaci, která je zneplatňuje (odemknutí LUKS, úprava konfigurace cíle) obnovuje cached pouze diagnostiku aktuálního rozsahu. Vede pouze v již autorizované administrátorské relaci a nikdy sama neotevře autorizaci.</translation>
    </message>
    <message>
        <source>Wrap long log lines</source>
        <translation>Zabalte dlouhé logové řádky</translation>
    </message>
    <message>
        <source>Mandatory safety controls</source>
        <translation>Povinné bezpečnostní kontroly</translation>
    </message>
    <message>
        <source>Host capabilities and dependencies</source>
        <translation>Hostitelské schopnosti a závislost</translation>
    </message>
    <message>
        <source>Refresh Capabilities</source>
        <translation>Obnovit schopnosti</translation>
    </message>
    <message>
        <source>Re-run the read-only host capability probes (a PATH search, nothing is executed) and refresh the distribution summary.</source>
        <translation>Re- spusťte sondy schopnosti read- only host (vyhledávání PATH, nic není provedeno) a obnovte distribuční souhrn.</translation>
    </message>
    <message>
        <source>Install Missing Support...</source>
        <translation>Nainstalujte chybějící podporu...</translation>
    </message>
    <message>
        <source>Automatic installation will require explicit package mapping and privilege authorization.</source>
        <translation>Automatická instalace bude vyžadovat výslovné mapování balíků a oprávnění k výsadám.</translation>
    </message>
    <message>
        <source>Application configuration</source>
        <translation>Konfigurace aplikace</translation>
    </message>
    <message>
        <source>Boot Bitch Legacy</source>
        <translation>Boot Bitch Legacy</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list first.</source>
        <translation>Nejprve vyberte fyzický pohon v seznamu dostupných opravných cílů.</translation>
    </message>
    <message>
        <source>Protected system</source>
        <translation>Chráněný systém</translation>
    </message>
    <message>
        <source>The running system cannot be selected as a repair target. Use Host Maintenance for the protected running host or choose another disk.</source>
        <translation>Běžící systém nemůže být vybrán jako cíl opravy. Použijte Host Údržba pro chráněný běh hostitele nebo vyberte jiný disk.</translation>
    </message>
    <message>
        <source>Unlock or select a Linux system first</source>
        <translation>Odemknout nebo nejprve vybrat Linux systém</translation>
    </message>
    <message>
        <source>This encrypted drive has no visible Linux filesystem yet. Use Unlock, refresh devices, and select the target after its Linux root is detected.</source>
        <translation>Tento šifrovaný disk zatím nemá žádný viditelný Linux souborový systém. Použijte Odemknout, obnovit zařízení, a vyberte cíl po jeho Linux root je detekován.</translation>
    </message>
    <message>
        <source>The selected root component (%1) belongs to the running system and cannot be committed as a repair target. Use Host Maintenance for the protected running host.</source>
        <translation>Zvolená kořenová složka (%1) patří do běžeckého systému a nelze ji použít jako cíl opravy. Použijte Host Údržba pro chráněný běh hostitele.</translation>
    </message>
    <message>
        <source>repair target commit</source>
        <translation>revize cíle opravy</translation>
    </message>
    <message>
        <source>Repair drive selected: %1</source>
        <translation>Zvolená oprava pohonu: %1</translation>
    </message>
    <message>
        <source>; best detected system component: %1</source>
        <translation>; nejlepší zjištěná systémová složka: %1</translation>
    </message>
    <message>
        <source>. No mount or repair action was performed.</source>
        <translation>. Žádná montáž nebo opravy akce nebyla provedena.</translation>
    </message>
    <message>
        <source>Make Default unavailable</source>
        <translation>Udělat výchozí nedostupný</translation>
    </message>
    <message>
        <source>Make Default is a running-host action on this frontend; enter Host Maintenance first.</source>
        <translation>Make Implicitní je running- host akce na této frontend; zadat Host Údržba první.</translation>
    </message>
    <message>
        <source>Administrator authorization required</source>
        <translation>Požadované povolení správce</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize first.</source>
        <translation>Žádná administrátorská relace není aktivní; nejprve stiskněte Autorizaci.</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host?

The helper verifies /boot/grub/menu.lst, sets the `default &lt;N&gt;` directive to the canonical entry, backs the menu up first and restores it on any failure.</source>
        <translation>Udělat kanonickou nainstalovanou položku jádra výchozí položku GRUB-dědictví boot na spuštěném hostiteli?

Pomocník ověřuje / boot / grub / menu.lst, nastaví směrnici &quot;default &lt; N &gt;&quot; na kanonický vstup, nejprve zazálohuje menu a obnoví jej při každém selhání.</translation>
    </message>
    <message>
        <source>Host Maintenance unavailable</source>
        <translation>Údržba hostitele není k dispozici</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a committed repair target or a detected running host.</source>
        <translation>Běžící hostitelský cíl nemohl být detekován; diagnostika potřebuje závazný opravárenský cíl nebo zjištěný hostitel.</translation>
    </message>
    <message>
        <source>Exit Host Maintenance</source>
        <translation>Ukončit údržbu hostitele</translation>
    </message>
    <message>
        <source>Host Maintenance is active; diagnostics and gated repairs target the protected running host.</source>
        <translation>Hostitelská údržba je aktivní; diagnostika a řízené opravy se zaměřují na chráněného běžce.</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list on the Systems tab first, or use Host Maintenance for the protected running host.</source>
        <translation>Nejprve zvolte fyzickou mechaniku v seznamu dostupných opravných cílů v záložce Systems, nebo použijte Host Maintenance pro chráněný běžící hostitel.</translation>
    </message>
    <message>
        <source>The selected drive (%1) is the protected running host. Choose Host Maintenance on the Systems tab to run read-only host diagnostics and guarded host repairs; ordinary target repairs stay disabled.</source>
        <translation>Zvolený disk (%1) je chráněný běžící hostitel. Vyberte si Host Údržba na záložce Systems spustit jen pro čtení diagnostiky hostitelů a hlídané opravy hostitelů; běžné cílové opravy zůstávají zakázány.</translation>
    </message>
    <message>
        <source>No repair target is committed. Choose Select Target on the Systems tab (or Host Maintenance for the protected running host) first.</source>
        <translation>Žádný cíl není spáchán. Vyberte si cíl na záložce Systems (nebo Údržba hostitele pro chráněný běžící hostitel).</translation>
    </message>
    <message>
        <source>The selection changed after the target was committed. Choose Select Target again.</source>
        <translation>Po spáchání cíle se výběr změnil. Zvolte znovu vybraný cíl.</translation>
    </message>
    <message>
        <source>Diagnostics scope required</source>
        <translation>Požadovaný rozsah diagnostiky</translation>
    </message>
    <message>
        <source>Diagnostics scope unresolved</source>
        <translation>Diagnostický rozsah nevyřešen</translation>
    </message>
    <message>
        <source>The running host target could not be resolved; use Refresh Devices and commit a repair target or re-enter Host Maintenance.</source>
        <translation>Běžící hostitelský cíl nemohl být vyřešen; použít Refresh zařízení a spáchat opravu cíl nebo znovu vstoupit do hostitelské údržby.</translation>
    </message>
    <message>
        <source>Diagnostic check required</source>
        <translation>Požadovaná diagnostická kontrola</translation>
    </message>
    <message>
        <source>Select a diagnostic check in the list first.</source>
        <translation>Nejprve zvolte diagnostickou kontrolu v seznamu.</translation>
    </message>
    <message>
        <source>Repair target required</source>
        <translation>Požadovaný cíl opravy</translation>
    </message>
    <message>
        <source>Target file editing needs a committed repair target. Running-host maintenance has no target-file editing; commit an offline target first.</source>
        <translation>Editace cílových souborů potřebuje závazný cíl opravy. Údržba Running-host nemá žádnou editaci target-file; nejdříve odešlete offline cíl.</translation>
    </message>
    <message>
        <source>Configuration file required</source>
        <translation>Požadovaný konfigurační soubor</translation>
    </message>
    <message>
        <source>Select a target configuration file first.</source>
        <translation>Nejprve vyberte konfigurační soubor cíle.</translation>
    </message>
    <message>
        <source>Edit target %1</source>
        <translation>Upravit cíl %1</translation>
    </message>
    <message>
        <source>Edit this target file through the guarded administrator helper. A successful save invalidates cached diagnostics; rerun diagnostics before repair. Generated files such as /boot/grub/menu.lst may be replaced by the next bootloader update.</source>
        <translation>Upravit tento cílový soubor pomocí stráženého administrátora. Úspěšné uložení znehodnocuje cached diagnostiku; znovu diagnostiku před opravou. Generované soubory jako / boot / grub / menu.lst mohou být nahrazeny další aktualizací.</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Zrušit</translation>
    </message>
    <message>
        <source>Save Target File</source>
        <translation>Uložit cílový soubor</translation>
    </message>
    <message>
        <source>No changes to %1.</source>
        <translation>Žádné změny v %1.</translation>
    </message>
    <message>
        <source>Configuration write refused</source>
        <translation>Konfigurační zápis odmítnut</translation>
    </message>
    <message>
        <source>The edited content contains NUL bytes; the guarded write refuses it.</source>
        <translation>Upravený obsah obsahuje NUL bytes; hlídaný text jej odmítá.</translation>
    </message>
    <message>
        <source>File too large</source>
        <translation>Soubor příliš velký</translation>
    </message>
    <message>
        <source>The edited file is larger than 1 MiB. The guarded write refuses it; edit the file from a console instead.</source>
        <translation>Editovaný soubor je větší než 1 MiB. Opatrný zápis jej odmítá; edituje soubor z konzole.</translation>
    </message>
    <message>
        <source>Write target configuration</source>
        <translation>Napsat konfiguraci cíle</translation>
    </message>
    <message>
        <source>Write the edited contents to %1? This modifies the repair target and invalidates cached diagnostics.</source>
        <translation>Napsat upravený obsah na %1? To upravuje cíl opravy a znehodnocuje cached diagnostiku.</translation>
    </message>
    <message>
        <source>No diagnostic results to copy yet.</source>
        <translation>Zatím nemáme žádné diagnostické výsledky.</translation>
    </message>
    <message>
        <source>Diagnostic results copied to the clipboard.</source>
        <translation>Diagnostické výsledky zkopírovány do schránky.</translation>
    </message>
    <message>
        <source>No diagnostic results to save yet.</source>
        <translation>Zatím žádné diagnostické výsledky.</translation>
    </message>
    <message>
        <source>Text files (*.txt);;All files (*)</source>
        <translation>Textové soubory (* .txt);; Všechny soubory (*)</translation>
    </message>
    <message>
        <source>Save diagnostic results</source>
        <translation>Uložit diagnostické výsledky</translation>
    </message>
    <message>
        <source>Could not write %1.</source>
        <translation>Nelze napsat %1.</translation>
    </message>
    <message>
        <source>Diagnostic results saved to %1</source>
        <translation>Diagnostické výsledky uložené na %1</translation>
    </message>
    <message>
        <source>Unlock not available</source>
        <translation>Odemknout není k dispozici</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked. Select an offline repair target to unlock.</source>
        <translation>Chráněný běžící hostitel nemůže být odemčen. Vyberte offline opravu cíl odemknout.</translation>
    </message>
    <message>
        <source>No locked LUKS component is currently visible on this selected drive.</source>
        <translation>Na tomto zvoleném disku není momentálně vidět žádná uzamčená složka LUKS.</translation>
    </message>
    <message>
        <source>Confirm LUKS unlock</source>
        <translation>Potvrdit otevření LUKS</translation>
    </message>
    <message>
        <source>Unlock %1 on %2?

The helper opens a temporary device-mapper mapping with cryptsetup and keeps it open for this recovery session. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>Odemknout %1 na %2?

Pomocník otevře dočasné mapování zařízení s kryptsetupem a udržuje ho otevřený pro tuto rekonvalescenci. Passshrase cestuje přes soukromý klíčový soubor a nikdy není umístěn v příkazových argumentech nebo protokolech.</translation>
    </message>
    <message>
        <source>LUKS unlock</source>
        <translation>Odemknout LUKS</translation>
    </message>
    <message>
        <source>Unlock LUKS repair target</source>
        <translation>Odemknout cíl opravy LUKS</translation>
    </message>
    <message>
        <source>Enter the passphrase for %1.

It is sent only to cryptsetup over the helper&apos;s standard input and is never logged or placed on a command line.</source>
        <translation>Zadejte heslo pro %1.

Je odesílán pouze do kryptsetupu přes standardní vstup pomocníka a nikdy není přihlášen nebo umístěn na příkazovém řádku.</translation>
    </message>
    <message>
        <source>Passphrase required</source>
        <translation>Vyžadováno Passprase</translation>
    </message>
    <message>
        <source>An empty passphrase was not submitted. Enter the LUKS passphrase or choose Cancel.</source>
        <translation>Prázdné heslo nebylo předloženo. Zadejte heslo LUKS nebo vyberte Zrušit.</translation>
    </message>
    <message>
        <source>Unlock keyfile unavailable</source>
        <translation>Odemknout klíčový soubor není k dispozici</translation>
    </message>
    <message>
        <source>The LUKS passphrase could not be written to a private keyfile in %1; the unlock was not started.</source>
        <translation>Passplasa LUKS nemohla být zapsána do soukromého souboru v %1; odemknutí nebylo spuštěno.</translation>
    </message>
    <message>
        <source>Configuration write unavailable</source>
        <translation>Konfigurace není k dispozici</translation>
    </message>
    <message>
        <source>The edited content could not be written to a private temporary file in %1; the write was not started.</source>
        <translation>Editovaný obsah nemohl být zapsán do soukromého dočasného souboru v %1; zápis nebyl zahájen.</translation>
    </message>
    <message>
        <source>ERROR: cannot read the session log %1</source>
        <translation>Chyba: nelze přečíst protokol sezení %1</translation>
    </message>
    <message>
        <source>Viewing a prior session log (read-only): %1</source>
        <translation>Zobrazení předchozího záznamu relace (pouze pro čtení): %1</translation>
    </message>
    <message>
        <source>Session log list refreshed.</source>
        <translation>Session log list obnoven.</translation>
    </message>
    <message>
        <source>Note:</source>
        <translation>Poznámka:</translation>
    </message>
    <message>
        <source>Delete session log</source>
        <translation>Smazat záznam zasedání</translation>
    </message>
    <message>
        <source>Delete %1 permanently? This cannot be undone.</source>
        <translation>Smazat %1 natrvalo? To se nedá odčinit.</translation>
    </message>
    <message>
        <source>%1 changed while the confirmation was open; the delete was refused.</source>
        <translation>%1 se změnil, když bylo otevřeno potvrzení; smazání bylo zamítnuto.</translation>
    </message>
    <message>
        <source>Unable to delete %1.</source>
        <translation>Nelze smazat %1.</translation>
    </message>
    <message>
        <source>Device discovery filters updated.</source>
        <translation>Filtry pro objevování zařízení aktualizovány.</translation>
    </message>
    <message>
        <source>Unknown Linux distribution</source>
        <translation>Neznámá distribuce Linuxu</translation>
    </message>
    <message>
        <source>sudo / gksu (no KAuth on this frontend)</source>
        <translation>sudo / gksu (no KAuth on this frontend)</translation>
    </message>
    <message>
        <source>Available</source>
        <translation>K dispozici</translation>
    </message>
    <message>
        <source>Missing on this frontend</source>
        <translation>Chybějící na této frontě</translation>
    </message>
    <message>
        <source>Missing</source>
        <translation>Chybějící</translation>
    </message>
    <message>
        <source>No repair tool is selected.</source>
        <translation>Není vybrán žádný nástroj pro opravy.</translation>
    </message>
    <message>
        <source>A helper command is already running.</source>
        <translation>Pomocný příkaz už běží.</translation>
    </message>
    <message>
        <source>Restore Graphical Login is a host-scope stage on this legacy frontend; enter Host Maintenance to run it.</source>
        <translation>Obnovit Graphical Login je host- scope etapa na tomto odkazu frontend; zadat Host Údržba spustit.</translation>
    </message>
    <message>
        <source>The selected scope has no resolved root component; use Refresh Devices and commit the repair target again.</source>
        <translation>Zvolený rozsah nemá žádnou vyřešenou kořenovou složku; použijte Refresh Devices a znovu odešlete cíl opravy.</translation>
    </message>
    <message>
        <source>This legacy frontend exposes no %1 action; the helper reports the capability as available.</source>
        <translation>Tento odkaz ukazuje žádnou akci %1; pomocník hlásí schopnost, jak je k dispozici.</translation>
    </message>
    <message>
        <source>Run this guarded repair action using the cached read-only diagnostic evidence. A confirmation is shown first.</source>
        <translation>Spustit tuto střeženou opravu akci pomocí cached read- pouze diagnostické důkazy. Nejprve se zobrazí potvrzení.</translation>
    </message>
    <message>
        <source>Enabled in Settings</source>
        <translation>Povoleno v Nastavení</translation>
    </message>
    <message>
        <source>Disabled in Settings - enable it to include this stage</source>
        <translation>Zakázáno v Nastavení - umožní zahrnout tuto fázi</translation>
    </message>
    <message>
        <source>Unavailable: %1</source>
        <translation>Nedostupný: %1</translation>
    </message>
    <message>
        <source>
Unavailable: %1</source>
        <translation>
Nedostupný: %1</translation>
    </message>
    <message>
        <source>Repair tool unavailable</source>
        <translation>Nástroj pro opravy není k dispozici</translation>
    </message>
    <message>
        <source>Confirm repair</source>
        <translation>Potvrzení opravy</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Your saved selection is kept and its availability is re-checked when diagnostics for this scope complete.</source>
        <translation>Pro tuto fázi zatím žádný důkaz. Váš uložený výběr je zachován a jeho dostupnost je po dokončení diagnostiky znovu ověřena.</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Run diagnostics for the selected scope to populate the Full Repair plan; your selection is saved once the stage becomes available.</source>
        <translation>Pro tuto fázi zatím žádný důkaz. Spustit diagnostiku pro vybraný rozsah pro zalidnění plánu Kompletní opravy; Váš výběr se uloží, jakmile bude jeviště k dispozici.</translation>
    </message>
    <message>
        <source>Unknown Full Repair stage.</source>
        <translation>Neznámá fáze Úplné opravy.</translation>
    </message>
    <message>
        <source>No Full Repair stages are selected or available; use Configure Plan... to choose the stages.</source>
        <translation>Nejsou zvolena ani k dispozici žádná etapa Úplná oprava; pro výběr etap použijte konfigurační plán.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab to establish the session.</source>
        <translation>Je vyžadováno autorizace správce; stiskněte autorizaci na záložce Systems or Opravy pro zahájení relace.</translation>
    </message>
    <message>
        <source>Runs the selected stages using the cached read-only diagnostic evidence after privilege confirmation.</source>
        <translation>Spustí zvolené fáze za použití cachovaných diagnostických důkazů pouze po potvrzení práva.</translation>
    </message>
    <message>
        <source>No Full Repair stages selected - use Configure Plan... or Settings.</source>
        <translation>Žádné volitelné fáze Úplné opravy - použijte Nastavit plán... nebo Nastavení.</translation>
    </message>
    <message>
        <source>1 stage selected</source>
        <translation>1 zvolená etapa</translation>
    </message>
    <message>
        <source>%1 stages selected</source>
        <translation>Vybrané fáze %1</translation>
    </message>
    <message>
        <source>No repair stages are selected. Use Configure Plan... to choose the stages Full Repair will run.</source>
        <translation>Nejsou vybrány žádné fáze opravy. Použijte Konfigurační plán... pro výběr fází Plné opravy poběží.</translation>
    </message>
    <message>
        <source>No selected stages are available. %1</source>
        <translation>Nejsou k dispozici žádné vybrané fáze. %1</translation>
    </message>
    <message>
        <source>Full Repair unavailable</source>
        <translation>Úplné opravy nejsou k dispozici</translation>
    </message>
    <message>
        <source>Run the Full Repair plan?

The selected stages run in order through the helper&apos;s guarded repair command:

</source>
        <translation>Spustit kompletní plán oprav?

Vybrané fáze běží v pořadí přes příkaz obsluhy:

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
Pomocník udržuje každý runtime předlet; jeviště, které selže, zastaví plán.</translation>
    </message>
    <message>
        <source>Running %1 through the privileged helper... The Logs tab keeps the complete transcript.</source>
        <translation>Projíždíme %1 přes privilegovaného pomocníka... Záložka Záznamy vede kompletní přepis.</translation>
    </message>
    <message>
        <source>Shell scope required</source>
        <translation>Požadovaný rozsah Shell</translation>
    </message>
    <message>
        <source>No administrator session is active.

Press Authorize on the Systems or Repair tab to establish the session, or enter Host Maintenance / commit a repair target on the Systems tab; the chroot shell then reuses the cached authorization.</source>
        <translation>Žádná administrátorská relace není aktivní.

Stiskněte autorizaci na záložce Systems or Opravy pro nastavení relace, nebo zadejte cíl opravy Host Maintenance / odešlete na záložce Systems; shell chroot poté znovu použije cached autorizaci.</translation>
    </message>
    <message>
        <source>Shell command required</source>
        <translation>Požadovaný příkaz Shell</translation>
    </message>
    <message>
        <source>Enter the command to run first.</source>
        <translation>Nejdřív zadejte příkaz.</translation>
    </message>
    <message>
        <source>Confirm running-host command</source>
        <translation>Potvrdit příkaz running- host</translation>
    </message>
    <message>
        <source>Run this command as root on the protected running host?

%1

The helper keeps its runtime preflights; the command is passed as one argument and is never interpreted by the GUI.</source>
        <translation>Spustit tento příkaz jako kořen chráněného hostitele?

%1

Pomocník udržuje své runtime předlety; příkaz je předán jako jeden argument a nikdy není interpretován GUI.</translation>
    </message>
    <message>
        <source>Running %1...</source>
        <translation>Běžící %1...</translation>
    </message>
    <message>
        <source>Authorization scope required</source>
        <translation>Požadovaný rozsah oprávnění</translation>
    </message>
    <message>
        <source>administrator authorization</source>
        <translation>autorizace správce</translation>
    </message>
    <message>
        <source>Administrator authorization</source>
        <translation>Povolení správce</translation>
    </message>
    <message>
        <source>Administrator authorization is required for %1.

Enter the password for %2 (sudo). It is used only for this sudo authentication, is sent over a pipe and is never logged or placed on a command line. The authorization is cached for this session and reused by diagnostics and repairs.</source>
        <translation>Pro %1 je vyžadováno povolení správce.

Zadejte heslo pro %2 (sudo). Používá se pouze pro toto sudo ověření, je odeslán přes trubku a nikdy není přihlášen nebo umístěn na příkazové řádce. Autorizace je pro tuto relaci cachována a znovu použita diagnostikou a opravami.</translation>
    </message>
    <message>
        <source>your account</source>
        <translation>Váš účet</translation>
    </message>
    <message>
        <source>Password required</source>
        <translation>Požadované heslo</translation>
    </message>
    <message>
        <source>An empty password was not submitted. Enter the sudo password or choose Cancel.</source>
        <translation>Nebylo předloženo prázdné heslo. Zadejte sudo heslo nebo zvolte Zrušit.</translation>
    </message>
    <message>
        <source>Administrator authorization failed</source>
        <translation>Povolení správce selhalo</translation>
    </message>
    <message>
        <source>sudo did not accept the password: %1

The command was not started.</source>
        <translation>sudo nepřijal heslo: %1

Příkaz nebyl zahájen.</translation>
    </message>
    <message>
        <source>elevation requires an interactive sudo password; run the smoke as root or after `sudo -S -v` with --elevate &apos;sudo -n&apos;</source>
        <translation>elevace vyžaduje interaktivní sudo heslo; spustit kouř jako root nebo po &apos;sudo -S -v&apos; s --elevate &apos;sudo -n&apos;</translation>
    </message>
    <message>
        <source>No administrator session is active for %1.

Press Authorize on the Systems or Repair tab to establish the session now, or enter Host Maintenance / commit a repair target on the Systems tab; diagnostics and repairs then reuse the cached authorization.</source>
        <translation>Pro %1 není aktivní žádná administrátorská relace.

Stiskněte Autorizaci na záložce Systémy nebo Opravy pro nastavení relace nyní, nebo zadejte Host Údržba / odevzdání cíle opravy na záložce Systems; diagnostika a opravy pak znovu cached autorizace.</translation>
    </message>
    <message>
        <source>Administrator authorization expired</source>
        <translation>Povolení správce vypršelo</translation>
    </message>
    <message>
        <source>The cached administrator authorization for %1 expired or was refused.

Press Authorize on the Systems or Repair tab to re-establish the session, then run the command again. No command was started.</source>
        <translation>Platnost povolení správce pro %1 skončila nebo byla zamítnuta.

Stiskněte Autorizaci na záložce Systémy nebo opravy pro obnovení relace a pak spusťte příkaz znovu. Žádný příkaz nebyl zahájen.</translation>
    </message>
    <message>
        <source>Privileged operation completed successfully. Administrator authorization remains active for this session.</source>
        <translation>Privilegovaná operace byla úspěšně dokončena. Autorizace správce zůstává aktivní pro toto sezení.</translation>
    </message>
    <message>
        <source>Privileged operation stopped with an error. Administrator authorization remains active; review the output before closing.</source>
        <translation>Privilegovaná operace se zastavila s chybou. Autorizace administrátora zůstává aktivní; před zavřením zkontrolujte výstup.</translation>
    </message>
    <message>
        <source>Inspection is read-only.

</source>
        <translation>Inspekce je read- pouze.

</translation>
    </message>
    <message>
        <source>Configuration unavailable</source>
        <translation>Nastavení není k dispozici</translation>
    </message>
    <message>
        <source>The helper could not read %1:

%2</source>
        <translation>Pomocník nemohl číst %1:

%2</translation>
    </message>
    <message>
        <source>Configuration write failed</source>
        <translation>Konfigurace selhala</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
Result: mapping opened for this recovery session.</source>
        <translation>Stav: odemčen
Složka: %1
Mapper: %2
Metoda: helper lock (cryptsetup; passphrase prostřednictvím mode- 600 klíčového souboru, po použití smazán)
Výsledek: pro tuto rekonvalescenci bylo otevřeno mapování.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: the passphrase was not accepted; retry offered.</source>
        <translation>Stav: zamčen
Složka: %1
Metoda: helper lock (cryptsetup)
Chyba: heslo nebylo přijato, znovu nabízeno.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: %2</source>
        <translation>Stav: zamčen
Složka: %1
Metoda: helper lock (cryptsetup)
Chyba: %2</translation>
    </message>
    <message>
        <source>Passphrase not accepted</source>
        <translation>Passphrase není přijat</translation>
    </message>
    <message>
        <source>The LUKS passphrase was not accepted.

Try again?</source>
        <translation>Passprase LUKS nebylo přijato.

Zkusit to znovu?</translation>
    </message>
    <message>
        <source>[-] %1 - not run (the plan stopped before reaching this stage)</source>
        <translation>[-] %1 - není spuštěn (plán se zastavil před dosažením této fáze)</translation>
    </message>
    <message>
        <source>filesystem</source>
        <translation>filesystem</translation>
    </message>
    <message>
        <source>[-] %1 - no file system errors found - no changes</source>
        <translation>[-] %1 - nenalezeny chyby v systému souborů - žádné změny</translation>
    </message>
    <message>
        <source>[FAIL] %1 - not reported</source>
        <translation>[FAIL] %1 - nehlášeno</translation>
    </message>
    <message>
        <source>%1 stage(s) failed; review the helper output in Logs.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>the plan stopped before any stage completed.</source>
        <translation>plán se zastavil před dokončením jakékoliv fáze.</translation>
    </message>
    <message>
        <source>no repair was needed; cached diagnostics remain valid.</source>
        <translation>nebyla nutná žádná oprava; cached diagnostics i nadále platí.</translation>
    </message>
    <message>
        <source>repair completed; cached diagnostics were invalidated and must be regenerated.</source>
        <translation>opravy dokončeny; cached diagnostics byly zrušeny a musí být regenerovány.</translation>
    </message>
    <message>
        <source>%1 results: [OK] %2 successful | [FAIL] %3 failed | [-] %4 no repair needed - %5</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Re-run Diagnostic</source>
        <translation>Respun Diagnostic</translation>
    </message>
    <message>
        <source>Unavailable</source>
        <translation>Nedostupné</translation>
    </message>
    <message>
        <source>Commit this physical drive as the repair target.</source>
        <translation>Spusťte tento fyzický pohon jako cíl opravy.</translation>
    </message>
    <message>
        <source>Protection:</source>
        <translation>Ochrana:</translation>
    </message>
    <message>
        <source>Host shell</source>
        <translation>Hostitelská skořápka</translation>
    </message>
    <message>
        <source>Run on Host</source>
        <translation>Spustit na hostitele</translation>
    </message>
    <message>
        <source>Host Shell</source>
        <translation>Hostitel Shell</translation>
    </message>
    <message>
        <source>The read-only probe found no editable target configuration file in this target. %1</source>
        <translation>Sonda pouze pro čtení nenašla v tomto cíli žádný upravený konfigurační soubor. %1</translation>
    </message>
    <message>
        <source>Not present in the selected target (omitted): %1</source>
        <translation>Není přítomen ve zvoleném cíli (vynechán): %1</translation>
    </message>
    <message>
        <source>Run diagnostics to probe which target configuration files exist; the helper&apos;s read-only probe decides the list.</source>
        <translation>Spusťte diagnostiku pro vyhledávání, které konfigurační soubory jsou zaměřeny; o seznamu rozhoduje pouze sonda pomocníka.</translation>
    </message>
    <message>
        <source>Probed read-only by the helper; a saved edit invalidates the cached diagnostics.</source>
        <translation>Probed read- pouze pomocník; uložená editace znehodnocuje cached diagnostiku.</translation>
    </message>
    <message>
        <source>Host maintenance: %1</source>
        <translation>Údržba hostitele: %1</translation>
    </message>
    <message>
        <source>unresolved</source>
        <translation>nevyřešeno</translation>
    </message>
    <message>
        <source>Target: %1 + %2</source>
        <translation>Cíl: %1 + %2</translation>
    </message>
    <message>
        <source>Host command</source>
        <translation>Příkaz hostitele</translation>
    </message>
    <message>
        <source>Execute a command on the running host as root.</source>
        <translation>Proveďte příkaz na běžícím hostiteli jako kořen.</translation>
    </message>
    <message>
        <source>Execute a command inside the selected repair system as root.</source>
        <translation>Provést příkaz uvnitř vybraného systému oprav jako kořen.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab (or re-enter Host Maintenance / re-commit the repair target) to authorize this session.</source>
        <translation>Je vyžadováno oprávnění správce; stiskněte autorizaci na záložce Systems or Opravy (nebo znovu-enter Host Maintenance / re- odevzdání cíle opravy) pro autorizaci této relace.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root on the running host through the helper&apos;s guarded host-shell verb.</source>
        <translation>Spusťte jeden přezkoumaný příkaz jako kořen na běžícím hostiteli přes pomocníkův chráněný hostitelský-shell sloveso.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root inside the target chroot through the helper&apos;s guarded shell verb.</source>
        <translation>Spusťte jeden zrevidovaný příkaz jako kořen uvnitř cílového chroot skrz pomocníkovo střežené sloveso.</translation>
    </message>
    <message>
        <source>Run a command on the running host as root (sudo is not needed). Commands are executed directly on the active system; output is kept in this window and in the application log.</source>
        <translation>Spustit příkaz na spuštěném hostiteli jako root (sudo není potřeba). Příkazy se provádějí přímo na aktivním systému; výstup je uložen v tomto okně a v protokolu aplikace.</translation>
    </message>
    <message>
        <source>Run a command inside the selected repair system as root (sudo is not needed). Commands are executed one at a time in a fresh chroot and cannot answer interactive prompts; use non-interactive flags such as apt-get -y upgrade. Output is kept in this window and in the application log.</source>
        <translation>Spusťte příkaz uvnitř vybraného opravárenského systému jako root (sudo není potřeba). Příkazy jsou prováděny jeden po druhém v čerstvé chroot a nemůže odpovědět interaktivní podněty; používat neinteraktivní vlajky, jako apt- get -y upgrade. Výstup je uložen v tomto okně a v protokolu aplikace.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize or re-enter Host Maintenance / commit a repair target.</source>
        <translation>Žádná administrátorská relace není aktivní; stiskněte Autorizovat nebo znovu zadat Host Údržba / odevzdání cíle opravy.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from the repaired system</source>
        <translation>1. Vyberte zdrojové soubory nebo složky z opraveného systému</translation>
    </message>
    <message>
        <source>2. Choose destination on this host</source>
        <translation>2. Vyberte si cíl na tomto hostiteli</translation>
    </message>
    <message>
        <source>Add File Path...</source>
        <translation>Přidat cestu k souboru...</translation>
    </message>
    <message>
        <source>Add Folder Path...</source>
        <translation>Přidat Složka Cesta...</translation>
    </message>
    <message>
        <source>Browse...</source>
        <translation>Procházet...</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it (Host Maintenance does not provide a repair tree to browse).</source>
        <translation>Zvolte opravárenský disk v Systems před výběrem cíle uvnitř (Host Údržba neposkytuje opravárenský strom k prohlížení).</translation>
    </message>
    <message>
        <source>Choose a host destination folder directly.</source>
        <translation>Zvolte přímo adresář cíle hostitele.</translation>
    </message>
    <message>
        <source>Copy the staged files and folders with cp -a, restore ownership with chown --reference and byte-compare every regular file afterwards.</source>
        <translation>Kopírovat nahrané soubory a složky s cp -a, obnovit vlastnictví s šité --reference a byte- porovnat každý pravidelný soubor poté.</translation>
    </message>
    <message>
        <source>File Copy unavailable</source>
        <translation>Kopie souboru není k dispozici</translation>
    </message>
    <message>
        <source>Choose host destination folder</source>
        <translation>Vybrat adresář pro určení hostitele</translation>
    </message>
    <message>
        <source>Select a repair target</source>
        <translation>Vyberte si cíl opravy</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it.</source>
        <translation>Zvolte opravárenský disk v Systems před výběrem cíle uvnitř.</translation>
    </message>
    <message>
        <source>File Copy - Browse Target Folders</source>
        <translation>Kopie souboru - Procházet cílové složky</translation>
    </message>
    <message>
        <source>Browse Target Folders</source>
        <translation>Procházet cílové složky</translation>
    </message>
    <message>
        <source>The helper could not list the repair-system folder:

%1</source>
        <translation>Pomocník nemohl vyjmenovat složku opravárenského systému:

%1</translation>
    </message>
    <message>
        <source>BROWSE_ENTRY</source>
        <translation>BROWSE _ ENTRY</translation>
    </message>
    <message>
        <source>(choose this folder: %1)</source>
        <translation>(vyberte tuto složku: %1)</translation>
    </message>
    <message>
        <source>(parent folder)</source>
        <translation>(mateřská složka)</translation>
    </message>
    <message>
        <source>Select repair-system destination</source>
        <translation>Vybrat místo určení opravárenského systému</translation>
    </message>
    <message>
        <source>Choose a destination folder inside the repaired system (current folder: %1):</source>
        <translation>Vyberte cílovou složku uvnitř opraveného systému (aktuální složka: %1):</translation>
    </message>
    <message>
        <source>Folder</source>
        <translation>Složka</translation>
    </message>
    <message>
        <source>Open</source>
        <translation>Otevřít</translation>
    </message>
    <message>
        <source>(choose this folder:</source>
        <translation>(vyberte tuto složku:</translation>
    </message>
    <message>
        <source>Add files to copy</source>
        <translation>Přidat soubory ke kopírování</translation>
    </message>
    <message>
        <source>Add folder to copy</source>
        <translation>Přidat složku k kopírování</translation>
    </message>
    <message>
        <source>Stage at least one source and name a destination path first.</source>
        <translation>Fáze nejméně jeden zdroj a pojmenovat cílovou cestu jako první.</translation>
    </message>
    <message>
        <source>Copy the %1 staged item(s) to %2?

The helper keeps its direction and path containment checks; a sensitive repair-system destination is refused unless the helper approves it, and every regular file is byte-compared after the copy.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>File Copy - Copy and Verify</source>
        <translation>Kopírovat soubor - Kopírovat a ověřovat</translation>
    </message>
    <message>
        <source>File Copy - Preview Changes</source>
        <translation>Kopírování souboru - Náhled změn</translation>
    </message>
    <message>
        <source>Protected running host - details</source>
        <translation>Chráněný běžící hostitel - podrobnosti</translation>
    </message>
    <message>
        <source>not mounted (offline target)</source>
        <translation>není namontován (offline terč)</translation>
    </message>
    <message>
        <source>Select a drive to see its details.</source>
        <translation>Vyberte disk, abyste viděli jeho detaily.</translation>
    </message>
    <message>
        <source>Inspecting the selected component.</source>
        <translation>Kontrola vybrané součásti.</translation>
    </message>
    <message>
        <source>Helper-confirmed by the last read-only diagnostics.</source>
        <translation>Pomocník potvrdil poslední diagnostiku.</translation>
    </message>
    <message>
        <source>Read-only inventory only; run diagnostics to confirm.</source>
        <translation>Pouze read- pouze inventura; spustit diagnostiku potvrdit.</translation>
    </message>
    <message>
        <source>PROTECTED - running system; read-only details only</source>
        <translation>PROTECTED - běžící systém; pouze údaje pro čtení</translation>
    </message>
    <message>
        <source>Live / installer media - not selectable</source>
        <translation>Živá / instalační média - nelze zvolit</translation>
    </message>
    <message>
        <source>PROTECTED - running host scope</source>
        <translation>OCHRANA - spuštěná oblast působnosti hostitele</translation>
    </message>
    <message>
        <source>Unlock required before selection</source>
        <translation>Odemknout nutné před výběrem</translation>
    </message>
    <message>
        <source>Eligible repair candidate</source>
        <translation>Způsobilý uchazeč o opravu</translation>
    </message>
    <message>
        <source>Drive:</source>
        <translation>Drive:</translation>
    </message>
    <message>
        <source>Detected target:</source>
        <translation>Zjištěný cíl:</translation>
    </message>
    <message>
        <source>Pending inspection</source>
        <translation>Před inspekcí</translation>
    </message>
    <message>
        <source>Model / label:</source>
        <translation>Vzor / označení:</translation>
    </message>
    <message>
        <source>Status:</source>
        <translation>Stav:</translation>
    </message>
    <message>
        <source>Size:</source>
        <translation>Velikost:</translation>
    </message>
    <message>
        <source>Connection:</source>
        <translation>Připojení:</translation>
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
        <translation>Rozměry:</translation>
    </message>
    <message>
        <source>Running system protection unresolved</source>
        <translation>Ochrana provozních systémů nevyřešena</translation>
    </message>
    <message>
        <source>No protected physical backing disk was identified</source>
        <translation>Nebyl identifikován žádný chráněný fyzický podpůrný disk</translation>
    </message>
    <message>
        <source>Current running Linux system</source>
        <translation>Současný systém Linux</translation>
    </message>
    <message>
        <source>Critical mounts: %1</source>
        <translation>Kritická montáž: %1</translation>
    </message>
    <message>
        <source>Read-only protected-running-system facts: the helper OS fact plus the inventory model, device path, size, transport and critical mounts. Nothing here is probed destructively.</source>
        <translation>Read- only protected - running- system facts: helper OS fakt plus inventarizační model, cesta zařízení, velikost, doprava a kritické montáže. Nic tady není destruktivně prozkoumáno.</translation>
    </message>
    <message>
        <source>Select a drive to see unlock status.</source>
        <translation>Vyberte disk pro zobrazení stavu odemknutí.</translation>
    </message>
    <message>
        <source>State: protected
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
The protected running host cannot be unlocked or modified; unlock is available for an offline repair target only. Use Host Maintenance for the protected running host.</source>
        <translation>Stav: chráněný
Složka: %1
Mapper: (žádný)
Metoda: helper lock (cryptsetup; passphrase prostřednictvím mode- 600 klíčového souboru, po použití smazán)
Chráněný běžecký hostitel nelze odemknout nebo upravit; odemknout lze pouze pro offline opravu. Použijte Host Údržba pro chráněný běh hostitele.</translation>
    </message>
    <message>
        <source>(none detected)</source>
        <translation>(žádná zjištěná)</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
A locked LUKS container is visible on this drive; press Unlock to open it for this recovery session.</source>
        <translation>Stav: zamčen
Složka: %1
Mapper: (žádný)
Metoda: helper lock (cryptsetup; passphrase prostřednictvím mode- 600 klíčového souboru, po použití smazán)
Na tomto disku je viditelný uzamčený kontejner LUKS; stiskněte Odemknout a otevřete jej pro toto zotavení.</translation>
    </message>
    <message>
        <source>(visible mapper)</source>
        <translation>(viditelný mapper)</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: already open before this session (visible mapper; Boot Bitch will reuse it and will not close it).</source>
        <translation>Stav: odemčen
Složka: %1
Mapper: %2
Metoda: již otevřena před tímto zasedáním (viditelný mapper; Boot Bitch jej znovu použije a nezavře).</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper-confirmed mapping from the last read-only diagnostics.</source>
        <translation>Stav: odemčen
Složka: %1
Mapper: %2
Metoda: helper- potvrzené mapování z poslední diagnostiky pouze pro čtení.</translation>
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
        <translation>Stav: uzamčeno nebo není zjištěna žádná šifrovaná součást
Součást: (není zjištěna žádná)
Mapper: (žádný)
Metoda: helper lock (cryptsetup; passphrase prostřednictvím mode- 600 klíčového souboru, po použití smazán)
V tomto disku nebyla zaznamenána žádná uzamčená součástka LUKS ani žádná operace odemknutí.</translation>
    </message>
    <message>
        <source>unavailable - %1</source>
        <translation>není k dispozici - %1</translation>
    </message>
    <message>
        <source>Host maintenance:
%1</source>
        <translation>Údržba hostitele:
%1</translation>
    </message>
    <message>
        <source>Target:
%1 + %2</source>
        <translation>Cíl:
%1 + %2</translation>
    </message>
    <message>
        <source>Authorization required: diagnostics and repairs fail closed until you press Authorize.</source>
        <translation>Autorizace nutná: diagnostika a opravy selhávají, dokud nestisknete Autorizaci.</translation>
    </message>
    <message>
        <source>Re-establish the privileged helper session for the current scope now. The password is requested in the hidden-input modal and is never logged.</source>
        <translation>Znovuzavedení privilegovaného pomocníka pro současný rozsah. Heslo je vyžadováno v hidden- input modal a nikdy není přihlášeno.</translation>
    </message>
    <message>
        <source>Running...</source>
        <translation>Běžím...</translation>
    </message>
    <message>
        <source>Scope required</source>
        <translation>Oblast působnosti</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope; this unlocks the gated actions.</source>
        <translation>Spustit vše - spustit každou dostupnou diagnostiku pouze pro aktuální rozsah; to odemkne uzavřené akce.</translation>
    </message>
    <message>
        <source>Select a target and wait for any running command first.</source>
        <translation>Vyberte si cíl a počkejte na jakýkoliv povel.</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic through the helper.</source>
        <translation>Spustit diagnostiku - spustit vybranou diagnostiku pouze přes pomocníka.</translation>
    </message>
    <message>
        <source>Run All for the current scope and refresh the read-only backend profile.</source>
        <translation>Spusťte vše pro aktuální rozsah a obnovte profil read- pouze backend.</translation>
    </message>
    <message>
        <source>Read or edit the selected target configuration file through the guarded helper; a saved edit invalidates cached diagnostics.</source>
        <translation>Čtení nebo editace vybraného konfiguračního souboru cílového pomocí stráženého pomocníka; uložená editace znehodnocuje cached diagnostiku.</translation>
    </message>
    <message>
        <source>Host Maintenance has no target-file editing; commit an offline repair target first.</source>
        <translation>Host Údržba nemá žádnou editaci target- file; nejdříve odešlete offline cíl opravy.</translation>
    </message>
    <message>
        <source>Commit an offline repair target first.</source>
        <translation>Nejdřív udělejte offline opravu.</translation>
    </message>
    <message>
        <source>No target configuration file is available for this target; run diagnostics to probe the list.</source>
        <translation>Pro tento cíl není k dispozici žádný konfigurační soubor, spusťte diagnostiku pro vyhledávání seznamu.</translation>
    </message>
    <message>
        <source>Already Unlocked</source>
        <translation>Už odemčeno</translation>
    </message>
    <message>
        <source>An unlocked Linux filesystem is already visible on this drive. Boot Bitch will reuse the existing mapper and will not close or reopen a mapping created by this recovery session. Mounting happens during diagnostics (read-only) and repairs (read-write); data filesystems are never auto-mounted on selection.</source>
        <translation>Na tomto disku je již vidět odemčený Linux souborový systém. Boot Bitch opětovně využije stávající map a nezavře ani znovu neotevře mapování vytvořené touto rekonvalescencí. Montáž probíhá v průběhu diagnostiky (pouze pro čtení) a oprav (read- write); datové soubory nejsou při výběru nikdy automaticky instalovány.</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked; use Host Maintenance for the protected running host.</source>
        <translation>Chráněný běžecký hostitel nelze odemknout; použijte Host Údržba pro chráněný běžecký hostitel.</translation>
    </message>
    <message>
        <source>Host Maintenance is the current scope, but the selected offline drive can still be unlocked.</source>
        <translation>Údržba hostitele je aktuální rozsah, ale vybraný offline disk lze stále odemknout.</translation>
    </message>
    <message>
        <source>Unlock %1 using cryptsetup through the privileged helper. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>Odemknout %1 pomocí kryptsetup prostřednictvím privilegovaného pomocníka. Passshrase cestuje přes soukromý klíčový soubor a nikdy není umístěn v příkazových argumentech nebo protokolech.</translation>
    </message>
    <message>
        <source>The protected running system cannot be selected as a repair target; use Host Maintenance for the protected running host.</source>
        <translation>Chráněný běžecký systém nemůže být vybrán jako cíl opravy; pro chráněný běžecký hostitel použijte Host Maintenance.</translation>
    </message>
    <message>
        <source>Live / installer media is read-only boot media and cannot be selected as a repair target.</source>
        <translation>Live / installer média jsou pouze pro čtení a nemohou být zvolena jako cíl opravy.</translation>
    </message>
    <message>
        <source>Unlock the encrypted volume first; Select Target becomes available after a Linux filesystem is detected.</source>
        <translation>Nejprve odemkněte zašifrovaný objem; po zjištění Linuxového souborového systému je k dispozici Select Target.</translation>
    </message>
    <message>
        <source>Committed repair target. Repair, Diagnostics and File Copy target this physical drive until another drive is explicitly selected with Select Target.</source>
        <translation>Oddaný opravářský cíl. Opravy, diagnostika a kopírování souborů zacílí na tento fyzický disk, dokud není explicitně vybrán jiný disk s Vybrat cíl.</translation>
    </message>
    <message>
        <source>Commit %1 as the repair target; this leaves Host Maintenance and switches the scope to the selected drive.</source>
        <translation>Odeslat %1 jako cíl opravy; to ponechává Host Údržba a přepne rozsah na vybraný disk.</translation>
    </message>
    <message>
        <source>The running host target could not be detected.</source>
        <translation>Běžící hostitelský cíl nemohl být detekován.</translation>
    </message>
    <message>
        <source>Leave Host Maintenance and return to repair-target mode.</source>
        <translation>Nechte údržbu hostitele a vraťte se do servisního režimu.</translation>
    </message>
    <message>
        <source>Select the running host for deliberate guarded maintenance; the administrator authorization is requested here once and cached for the session.</source>
        <translation>Vyberte spuštěného hostitele pro záměrnou hlídanou údržbu; autorizace administrátora je požadována zde jednou a cached pro relaci.</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a repair target.</source>
        <translation>Běžící hostitelský cíl nemohl být detekován; diagnostika potřebuje opravit cíl.</translation>
    </message>
    <message>
        <source>Committed target: %1 + %2</source>
        <translation>Zadaný cíl: %1 + %2</translation>
    </message>
    <message>
        <source>Committed target: none (selection changed)</source>
        <translation>Zadaný cíl: žádný (změna výběru)</translation>
    </message>
    <message>
        <source>The selected scope has no resolved Linux root component; Refresh Devices and commit the repair target again.</source>
        <translation>Zvolený rozsah nemá vyřešenou kořenovou složku Linux; Obnovit zařízení a znovu provést opravu cíle.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize on the Systems or Repair tab to re-establish it. Run All reuses the cached authorization and never prompts by itself.</source>
        <translation>Není aktivní žádná administrátorská relace; stiskněte Autorizaci na záložce Systems or Opravy pro její obnovení. Spustit Vše znovu využívá cached oprávnění a nikdy podněcuje sám.</translation>
    </message>
    <message>
        <source>Run diagnostics for this scope to unlock the gated actions. Diagnostics are read-only and the only evidence source.</source>
        <translation>Spustit diagnostiku pro tento rozsah odemknout uzavřené akce. Diagnostika je pouze pro čtení a jediný zdroj důkazů.</translation>
    </message>
    <message>
        <source>A repair was not proven unchanged, so the cached diagnostics are invalidated. Run diagnostics again before another gated action.</source>
        <translation>Oprava nebyla prokázána beze změny, takže cached diagnostika jsou neplatné. Znovu spusťte diagnostiku před další uzavřenou akcí.</translation>
    </message>
    <message>
        <source>Gated actions reflect the cached capability lines; the helper still runs every runtime preflight when a command starts.</source>
        <translation>Gated akce odrážejí cached schopnosti linky; pomocník stále běží každý runtime předlet, když příkaz začíná.</translation>
    </message>
    <message>
        <source>Regenerating diagnostics automatically</source>
        <translation>Regenerování diagnostiky automaticky</translation>
    </message>
    <message>
        <source>Running diagnostic: %1</source>
        <translation>Běžící diagnostika: %1</translation>
    </message>
    <message>
        <source>Running all diagnostics</source>
        <translation>Spustit veškerou diagnostiku</translation>
    </message>
    <message>
        <source>Unlocking %1</source>
        <translation>Odemknutí %1</translation>
    </message>
    <message>
        <source>running host</source>
        <translation>spuštěný hostitel</translation>
    </message>
    <message>
        <source>offline target</source>
        <translation>offline cíl</translation>
    </message>
    <message>
        <source>host maintenance active</source>
        <translation>údržba hostitele aktivní</translation>
    </message>
    <message>
        <source>repair target committed</source>
        <translation>Cíl opravy</translation>
    </message>
    <message>
        <source>no committed scope</source>
        <translation>žádná závazná oblast působnosti</translation>
    </message>
    <message>
        <source>Current session</source>
        <translation>Aktuální zasedání</translation>
    </message>
    <message>
        <source>Log files (*.log);;All files (*)</source>
        <translation>Soubory záznamu (* .log);; Všechny soubory (*)</translation>
    </message>
    <message>
        <source>Save log as</source>
        <translation>Uložit záznam jako</translation>
    </message>
    <message>
        <source>About Boot Bitch</source>
        <translation>O Boot Bitch</translation>
    </message>
    <message>
        <source>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;&lt;p&gt;&lt;b&gt;Developer:&lt;/b&gt; CaptainMorgan12&lt;/p&gt;&lt;p&gt;This GUI is the package&apos;s entry point: a Qt 3.3.x frontend with the ported guarded helper for Debian Etch-era systems.&lt;/p&gt;&lt;p&gt;&lt;b&gt;Guarded repair mode:&lt;/b&gt; read-only diagnostics can inspect either the protected Running Host or an explicitly selected repair drive. The guarded Debian/APT package stages (interrupted configuration, broken dependencies, metadata refresh, upgrade), GRUB-legacy configuration regeneration, LUKS target unlock and guarded target-file editing run through the ported helper after confirmation; Host Maintenance enables the same supported stages natively on the active system after repeating the host identity and boot-mount checks.&lt;/p&gt;&lt;p&gt;The modern-only features - verified File Copy, Btrfs snapshot rollback, EFI/UKI and extlinux repair, boot-stack reconciliation and Make Default - are greyed with the helper&apos;s own probe reasons on this frontend; Arch/Alpine/Fedora package backends stay diagnostics-only here.&lt;/p&gt;&lt;p&gt;The first privileged action authorizes one cached administrator session per scope through a hidden-input modal (the Qt GUI remains unprivileged). It can be ended at any time from File - Lock Administrator Session.&lt;/p&gt;</source>
        <translation>&lt; h3 &gt; Boot Bitch %1 &lt; / h3 &gt; &lt; p &gt; &lt; b &gt; Vývojář: &lt; / b &gt; CaptainMorgan12 &lt; / p &gt; &lt; p &gt; Tento GUI je vstupní bod balíku: Qt 3.3.x frontend s přenosným hlídaným pomocníkem pro systémy Debian Etch-era. &lt; / p &gt; &lt; p &gt; &lt; b &gt; Opravený režim opravy: &lt; / b &gt; read-only diagnostics can control buďto the protected running Host nebo explicitně vybraný opravárenský pohon. Stadia hlídaného balíčku Debian / APT (přerušená konfigurace, porušené závislosti, obnova metadat, upgrade), regenerace konfigurace GRUB- dědictví, terč LUKS odemknout a hlídaná editace cílového souboru běží přes přenášený pomocník po potvrzení; Host Maintenance umožňuje stejné podporované etapy nativně na aktivním systému po opakování hostitelské identity a boot- montážní kontroly. &lt; / p &gt; &lt; p &gt; Moderní-pouze funkce - ověřený soubor Kopírování, Btrfs snímek zpět, EFI / UKI a extlinux opravy, boot- stack sesouhlasení a Make Výchozí - jsou šedivé s pomocníkem vlastní sondy důvody na této frontě; Arch / Alpine / Fedora balíček zálohuje zůstat diagnózy-pouze zde. &lt; / p &gt; &lt; p &gt; První privilegovaná akce opravňuje k jednomu relaci v cached administrátora na rozsah prostřednictvím hidden- input modal (Qt GUI zůstává neprivilegovaný). Může být kdykoliv ukončen z File - Lock Administrator Session. &lt; / p &gt;</translation>
    </message>
</context>
</TS>
