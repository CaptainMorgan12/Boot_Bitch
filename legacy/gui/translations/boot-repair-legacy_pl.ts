<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS language="pl">
<context>
    <name>LegacyMainWindow</name>
    <message>
        <source>Validate environment</source>
        <translation>Sprawdzić środowisko</translation>
    </message>
    <message>
        <source>Validate</source>
        <translation>Walidacja</translation>
    </message>
    <message>
        <source>Check the selected system&apos;s mounts, filesystem metadata, boot files, mapper consistency and dependency readiness before any repair action. This is an independent safety preflight rather than an optional Full Repair stage.</source>
        <translation>Przed każdą akcją naprawczą należy sprawdzić zamontowane przez wybrany system góry, metadane systemu plików, pliki startowe, spójność mappera i gotowość do zależności. Jest to niezależny przedlot bezpieczeństwa, a nie fakultatywny etap pełnej naprawy.</translation>
    </message>
    <message>
        <source>Always preflight</source>
        <translation>Zawsze przed lotem</translation>
    </message>
    <message>
        <source>File system repair</source>
        <translation>Naprawa systemu plików</translation>
    </message>
    <message>
        <source>Check File Systems</source>
        <translation>Sprawdź systemy plików</translation>
    </message>
    <message>
        <source>Run the read-only file system check for the selected system&apos;s root and /boot filesystems and report each device&apos;s check tool and result without changing anything. This legacy frontend exposes the read-only check only; device repair is not wired.</source>
        <translation>Uruchom sprawdzanie systemu plików tylko do odczytu dla wybranych systemów plików root i / boot i zgłoś narzędzie kontrolne każdego urządzenia i wynik bez zmiany niczego. Ten dziedziczny front eksponuje tylko sprawdzanie; naprawa urządzenia nie jest przewodowa.</translation>
    </message>
    <message>
        <source>Full Repair plan: unavailable on this frontend - read-only check only</source>
        <translation>Pełny plan naprawy: niedostępny na tym froncie - sprawdź tylko ponownie</translation>
    </message>
    <message>
        <source>Complete package configuration</source>
        <translation>Pełna konfiguracja pakietu</translation>
    </message>
    <message>
        <source>Complete Configuration</source>
        <translation>Pełna konfiguracja</translation>
    </message>
    <message>
        <source>Complete interrupted dpkg package configuration in the selected repair system. This is the same stage controlled by Settings -&gt; Full Repair plan -&gt; Complete interrupted package configuration, but it can also be run independently here.</source>
        <translation>Kompletna przerwana konfiguracja pakietu dpkg w wybranym systemie naprawy. Jest to ten sam etap kontrolowany przez Ustawienia - &gt; Pełny plan naprawy - &gt; Kompletna przerwana konfiguracja pakietu, ale może być również prowadzona niezależnie.</translation>
    </message>
    <message>
        <source>Run the guarded dpkg-configure repair? The helper keeps its package-lock and runtime preflights.</source>
        <translation>Uruchomić strzeżony dpkg- configure naprawy? Pomocnik utrzymuje swój pakiet - blokada i runtime przedlotów.</translation>
    </message>
    <message>
        <source>Repair broken dependencies</source>
        <translation>Napraw uszkodzone zależności</translation>
    </message>
    <message>
        <source>Repair Dependencies</source>
        <translation>Zależności od naprawy</translation>
    </message>
    <message>
        <source>Repair package dependencies in the selected repair system after the mandatory safety preflight. This maps directly to Settings -&gt; Repair broken package dependencies.</source>
        <translation>Napraw zależności pakietów w wybranym systemie naprawy po obowiązkowym przedlocie bezpieczeństwa. Mapy bezpośrednio na Ustawienia - &gt; Napraw uszkodzone zależności pakietów.</translation>
    </message>
    <message>
        <source>Run the guarded fix-broken repair? The helper keeps its simulation-first preflight and runtime guards.</source>
        <translation>Uruchomić zabezpieczoną naprawę? Pomocnik utrzymuje symulacje - pierwszy przed lotem i strażników w czasie biegu.</translation>
    </message>
    <message>
        <source>Refresh package metadata</source>
        <translation>Odśwież metadane pakietu</translation>
    </message>
    <message>
        <source>Refresh Metadata</source>
        <translation>Odśwież metadane</translation>
    </message>
    <message>
        <source>Refresh APT metadata in the selected repair system without upgrading installed packages. This maps directly to Settings -&gt; Refresh package metadata.</source>
        <translation>Odśwież metadane APT w wybranym systemie naprawy bez aktualizacji zainstalowanych pakietów. Mapy bezpośrednio na Ustawienia - &gt; Odśwież metadane pakietów.</translation>
    </message>
    <message>
        <source>Refresh package metadata for the selected scope? The helper requires a reachable, trusted APT source.</source>
        <translation>Odświeżyć metadane pakietów dla wybranego zakresu? Pomocnik wymaga dostępnego, zaufanego źródła APT.</translation>
    </message>
    <message>
        <source>Upgrade installed packages</source>
        <translation>Aktualizacja zainstalowanych pakietów</translation>
    </message>
    <message>
        <source>Simulate and Upgrade</source>
        <translation>Symulować i ulepszać</translation>
    </message>
    <message>
        <source>Simulate the APT transaction first, inspect proposed removals, then apply a safe upgrade. This maps directly to Settings -&gt; Upgrade installed packages.</source>
        <translation>Symuluj najpierw transakcję APT, sprawdź proponowane przeprowadzki, a następnie zastosuj bezpieczną aktualizację. Mapy bezpośrednio na Ustawienia - &gt; Aktualizacja zainstalowanych pakietów.</translation>
    </message>
    <message>
        <source>Run the guarded apt-upgrade transaction? The helper keeps its simulation-first and source guards.</source>
        <translation>Uruchomić zabezpieczoną transakcję apt- upgrade? Pomocnik utrzymuje symulacje - pierwszy i straż źródłowa.</translation>
    </message>
    <message>
        <source>DKMS</source>
        <translation>DKMS</translation>
    </message>
    <message>
        <source>Rebuild DKMS</source>
        <translation>Przebudowa DKMS</translation>
    </message>
    <message>
        <source>Rebuild out-of-tree kernel modules for kernels installed in the selected system. The helper refuses this action when DKMS is not installed; this legacy frontend exposes no DKMS action.</source>
        <translation>Przebudowa modułów jądra-of- tree dla jąder zainstalowanych w wybranym systemie. Pomocnik odmawia tej akcji, gdy DKMS nie jest zainstalowany; ten dziedziczny front nie ujawnia żadnej akcji DKMS.</translation>
    </message>
    <message>
        <source>Graphical login / display manager</source>
        <translation>Graficzny menedżer logowania / wyświetlania</translation>
    </message>
    <message>
        <source>Restore Graphical Login</source>
        <translation>Przywróć logowanie graficzne</translation>
    </message>
    <message>
        <source>Restore the legacy SysV display manager configured for the running host: the /etc/X11/default-display-manager entry and the missing runlevel S-symlink, with a backup and rollback, never starting the GUI. This is a host-scope stage on this legacy frontend.</source>
        <translation>Przywrócenie dotychczasowego menedżera wyświetlacza SysV skonfigurowanego dla bieżącego hosta: wpis / etc / X11 / default- display- manager oraz brakujące dowiązanie S- symlink poziomu runlevel, z backup i rollback, nigdy nie uruchamia GUI. Jest to etap hostscope na tym dziedzicznym frontendzie.</translation>
    </message>
    <message>
        <source>Restore the graphical login configuration for the running host? The helper backs up /etc/X11/default-display-manager and the runlevel symlink state, restores the configured entry and the missing S-symlink, rolls back on any failure, and never starts the display manager.</source>
        <translation>Przywrócić konfigurację logowania dla uruchomionego komputera? Pomocnik backup / etc / X11 / default- display- manager i stan symlink runlevel, przywraca skonfigurowany wpis i brakujące S- symlink, cofa się z powrotem na żadnej awarii i nigdy nie uruchamia menedżera wyświetlacza.</translation>
    </message>
    <message>
        <source>Initramfs</source>
        <translation>Manitramfs</translation>
    </message>
    <message>
        <source>Rebuild Initramfs</source>
        <translation>Rebuild Initramfs</translation>
    </message>
    <message>
        <source>Rebuild initramfs images for the selected repair system only after mapper and crypttab consistency checks pass. The helper backs up each image before the apply. On Etch the stage runs through the guarded plain-chroot fallback (no unshare required).</source>
        <translation>Przebudować obrazy initramfs dla wybranego systemu naprawy dopiero po przejściu kontroli spójności mapper i crypttab. Pomocnik backup każdy obraz przed aplikacji. Na Etch scena przebiega przez strzeżony asfalt chroot (brak braku udziału).</translation>
    </message>
    <message>
        <source>Rebuild the initramfs for the selected scope? The helper keeps its mapper/crypttab and backup preflights.</source>
        <translation>Odbudować initramfs dla wybranego zakresu? Pomocnik utrzymuje swoje mapper / crypttab i backup przedlotów.</translation>
    </message>
    <message>
        <source>EFI / UKI bootloader</source>
        <translation>Ładowarka EFI / UKI</translation>
    </message>
    <message>
        <source>Repair EFI / UKI</source>
        <translation>Naprawa EFI / UKI</translation>
    </message>
    <message>
        <source>Repair the selected system&apos;s EFI / UKI boot path. This legacy frontend exposes no EFI action; the Etch target is a BIOS/GRUB-legacy system.</source>
        <translation>Napraw ścieżkę startową EFI / UKI wybranego systemu. Ten dziedziczny front nie ujawnia żadnych działań EFI; docelowym celem Etch jest system BIOS / GRUB- legacy.</translation>
    </message>
    <message>
        <source>GRUB configuration</source>
        <translation>Konfiguracja GRUB</translation>
    </message>
    <message>
        <source>Regenerate GRUB</source>
        <translation>Regeneruj GRUB</translation>
    </message>
    <message>
        <source>Regenerate the selected repair system&apos;s GRUB menu/configuration after the mandatory safety preflight. The helper backs up menu.lst, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Regeneruj wybrane menu / konfigurację systemu naprawczego GRUB po obowiązkowym przedlocie bezpieczeństwa. Pomocnik cofa menu.lst, zachowuje każdy istniejący wpis i wraca do każdej awarii.</translation>
    </message>
    <message>
        <source>Regenerate the GRUB configuration? The helper backs up the target menu/configuration, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Regenerować konfigurację GRUB? Pomocnik kopie zapasowe menu / konfiguracji docelowej, zachowuje każdy istniejący wpis startowy i przewija się z powrotem na każdą awarię.</translation>
    </message>
    <message>
        <source>extlinux configuration</source>
        <translation>Konfiguracja extlinux</translation>
    </message>
    <message>
        <source>Regenerate extlinux</source>
        <translation>Regeneruj extlinux</translation>
    </message>
    <message>
        <source>Regenerate the selected system&apos;s extlinux bootloader configuration. This legacy frontend exposes no extlinux action.</source>
        <translation>Regeneruj konfigurację bootloadera extlinux wybranego systemu. Ten dziedziczny front nie ujawnia żadnych działań extlinux.</translation>
    </message>
    <message>
        <source>Boot stack reconciliation</source>
        <translation>Pojednanie stosu boot</translation>
    </message>
    <message>
        <source>Reconcile Boot Stack</source>
        <translation>Reconcile Boot Stack</translation>
    </message>
    <message>
        <source>Reconcile the selected repair system&apos;s boot stack in one guarded legacy pass: mapper/crypttab validation, initramfs rebuild and GRUB-legacy configuration regeneration, with the component backups and preflights unchanged. This is the Etch equivalent of the modern boot-stack reconciliation and stays out of the Full Repair plan.</source>
        <translation>Odtworzyć stos startowy wybranego systemu naprawczego w jednej strzeżonej przepustce: walidacja mapper / crypttab, odbudowa initramfs i regeneracja konfiguracji GRUB-, z kopii zapasowych komponentów i lotów wstępnych bez zmian. Jest to odpowiednik Etch nowoczesnego pojednania stos boot- i pozostaje poza planem pełnej naprawy.</translation>
    </message>
    <message>
        <source>Full Repair plan: Manual recovery tool</source>
        <translation>Pełny plan naprawy: Ręczne narzędzie do odzyskiwania</translation>
    </message>
    <message>
        <source>Run the guarded boot-stack reconciliation? The helper runs the mapper/crypttab validation, the initramfs rebuild and the GRUB-legacy regeneration in one pass with every component preflight and backup.</source>
        <translation>Uruchomić strzeżone pojednanie? Pomocnik uruchamia walidację mapper / crypttab, odbudowę initramfs i regenerację odziedziczoną przez GRUB- za jednym razem z każdym elementem przed lotem i backup.</translation>
    </message>
    <message>
        <source>Complete interrupted package configuration</source>
        <translation>Pełna konfiguracja przerwanego pakietu</translation>
    </message>
    <message>
        <source>Repair broken package dependencies</source>
        <translation>Napraw uszkodzone zależności pakietów</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Domyślne</translation>
    </message>
    <message>
        <source>Repair file system errors (read-only check first)</source>
        <translation>Napraw błędy systemowe plików (najpierw sprawdź ponownie)</translation>
    </message>
    <message>
        <source>the legacy frontend exposes the read-only file system check only; per-device repair is not wired on this frontend (fail closed)</source>
        <translation>dziedziczny front eksponuje tylko sprawdzanie systemu plików tylko do odczytu; naprawa urządzenia nie jest podłączona na tym froncie (awaria zamknięta)</translation>
    </message>
    <message>
        <source>Upgrade installed packages (adaptive APT simulation)</source>
        <translation>Aktualizacja zainstalowanych pakietów (adaptive APT simulation)</translation>
    </message>
    <message>
        <source>Rebuild DKMS modules</source>
        <translation>Przebudowa modułów DKMS</translation>
    </message>
    <message>
        <source>Restore graphical login manager</source>
        <translation>Przywróć graficzny menedżer logowania</translation>
    </message>
    <message>
        <source>Rebuild initramfs after mapper/crypttab validation</source>
        <translation>Przebudowa initramfs po walidacji mapper / crypttab</translation>
    </message>
    <message>
        <source>Repair EFI / UKI boot path</source>
        <translation>Napraw ścieżkę startową EFI / UKI</translation>
    </message>
    <message>
        <source>Update GRUB configuration</source>
        <translation>Aktualizuj konfigurację GRUB</translation>
    </message>
    <message>
        <source>Update extlinux configuration</source>
        <translation>Aktualizuj konfigurację extlinux</translation>
    </message>
    <message>
        <source>Choose Full Repair stages in Settings. Enabled stages run in the order shown. Each configurable stage also appears below as an individual tool; the Full Repair column mirrors its current Settings state. The boot tools (EFI / UKI bootloader, GRUB or extlinux configuration, boot-stack reconciliation and Make Default) are independent: run them in any order, and a later action re-verifies what an earlier one changed and reports its own result. The active scope is shown beside Repair: selected repair drive or Running Host maintenance.</source>
        <translation>Wybierz pełne etapy naprawy w ustawieniach. Włączone etapy uruchamiane w podanej kolejności. Każdy konfigurowalny etap pojawia się również poniżej jako indywidualne narzędzie; kolumna Pełna Naprawa odzwierciedla jej obecny stan Ustawienia. Narzędzia startowe (bootloader EFI / UKI, konfiguracja GRUB lub extlinux, pojednanie boot- stock i Make Default) są niezależne: uruchamiają je w dowolnej kolejności, a późniejsza akcja ponownie sprawdza, co wcześniej zmienił i zgłasza swój wynik. Aktywny zakres jest pokazany obok naprawy: wybrany napęd naprawczy lub obsługa serwisowa.</translation>
    </message>
    <message>
        <source>Run All diagnostics for the selected target or running host before starting Full Repair. The report is read-only evidence used to choose and confirm repair stages.</source>
        <translation>Uruchom całą diagnostykę dla wybranego celu lub uruchomionego hosta przed rozpoczęciem pełnej naprawy. Raport jest jedynie dowodami wykorzystywanymi do wyboru i potwierdzenia etapów naprawy.</translation>
    </message>
    <message>
        <source>Ready: required cached read-only diagnostics are available for the selected stages. Review them in Diagnostics or Logs before confirming.</source>
        <translation>Gotowy: wymagana diagnostyka tylko dla wybranych etapów. Przejrzyj je w diagnostyce lub logach przed potwierdzeniem.</translation>
    </message>
    <message>
        <source>Environment validation</source>
        <translation>Walidacja środowiska</translation>
    </message>
    <message>
        <source>Summarizes the selected system, protection state, mounted identity and inspection readiness.</source>
        <translation>Podsumowanie wybranego systemu, stanu ochrony, zamontowanej tożsamości i gotowości inspekcji.</translation>
    </message>
    <message>
        <source>Distribution and boot backend profile</source>
        <translation>Dystrybucja i profil boot back end</translation>
    </message>
    <message>
        <source>Identifies the distribution family, package manager, initramfs generator, bootloader and current guarded repair capability.</source>
        <translation>Identyfikuje rodzinę dystrybucyjną, menedżera pakietów, generator initramfs, bootloader i aktualne możliwości naprawy strzeżonej.</translation>
    </message>
    <message>
        <source>Boot diagnostics</source>
        <translation>Diagnostyka buta</translation>
    </message>
    <message>
        <source>Shows boot mounts and /boot contents plus storage evidence without changing the selected system.</source>
        <translation>Pokazuje elementy montowania i / boot oraz dowody przechowywania bez zmiany wybranego systemu.</translation>
    </message>
    <message>
        <source>Boot evidence and selection history</source>
        <translation>Dowody i historia wyboru</translation>
    </message>
    <message>
        <source>Correlates the detected boot chain, bootloader selection, kernel/initramfs and unlock evidence.</source>
        <translation>Koreluje wykryty łańcuch startowy, wybór bootloadera, jądro / initramfs i odblokować dowody.</translation>
    </message>
    <message>
        <source>Kernel / initramfs</source>
        <translation>Jądro / initramfs</translation>
    </message>
    <message>
        <source>Reviews kernel files and verifies matching initramfs images through a read-only inspection.</source>
        <translation>Przegląda pliki jądra i weryfikuje pasujące obrazy initramfs poprzez kontrolę tylko read-.</translation>
    </message>
    <message>
        <source>Reviews the GRUB configuration without changing boot files.</source>
        <translation>Przegląda konfigurację GRUB bez zmiany plików startowych.</translation>
    </message>
    <message>
        <source>EFI / UKI boot state</source>
        <translation>EFI / UKI stan uruchamiania</translation>
    </message>
    <message>
        <source>Inspects EFI/UKI evidence; unavailable on this legacy BIOS frontend with the helper&apos;s probe reason.</source>
        <translation>Kontroluje dowody EFI / UKI; niedostępne na tym dziedzicznym froncie BIOS z powodu sondy pomocnika.</translation>
    </message>
    <message>
        <source>Reviews the configured display manager and recent boot evidence without starting the GUI.</source>
        <translation>Recenzje skonfigurowanego menedżera wyświetlania i najnowszych danych o uruchomieniu bez uruchamiania GUI.</translation>
    </message>
    <message>
        <source>Boot errors</source>
        <translation>Błąd buta</translation>
    </message>
    <message>
        <source>Reads recent error-priority entries from the running host or selected repair system when available.</source>
        <translation>Odczytuje ostatnie wpisy o priorytecie błędu z działającego hosta lub wybranego systemu naprawy, gdy jest dostępny.</translation>
    </message>
    <message>
        <source>Disk usage</source>
        <translation>Wykorzystanie dysku</translation>
    </message>
    <message>
        <source>Summarizes filesystem capacity and free space for the running host or read-only repair target.</source>
        <translation>Summaryzuje pojemność systemu plików i wolną przestrzeń dla działającego hosta lub tylko read- celu naprawy.</translation>
    </message>
    <message>
        <source>File systems</source>
        <translation>Systemy plików</translation>
    </message>
    <message>
        <source>Runs the read-only file system check for the selected system&apos;s root, /boot and other filesystems.</source>
        <translation>Uruchamia kontrolę systemu plików tylko do odczytu dla wybranego systemu root, / boot i innych systemów plików.</translation>
    </message>
    <message>
        <source>/etc/fstab review</source>
        <translation>/ etc / fstab review</translation>
    </message>
    <message>
        <source>Displays the running host or selected repair system&apos;s fstab; repair-system inspection is mounted read-only.</source>
        <translation>Wyświetla uruchomiony host lub wybrany system naprawczy fstab; kontrola systemu naprawczego jest montowana tylko ponownie.</translation>
    </message>
    <message>
        <source>Btrfs status</source>
        <translation>Status Btrfs</translation>
    </message>
    <message>
        <source>Shows Btrfs filesystem and subvolume information when the target uses Btrfs.</source>
        <translation>Pokazuje system plików Btrfs i informacje o podgłośnieniu, gdy cel wykorzystuje Btrfs.</translation>
    </message>
    <message>
        <source>Device-mapper ancestry</source>
        <translation>Pochodzenie device- mapper</translation>
    </message>
    <message>
        <source>Shows selected mapper ancestry and device-mapper state when available.</source>
        <translation>Pokazuje wybrany stan przodków maperów i device- maperów, kiedy jest dostępny.</translation>
    </message>
    <message>
        <source>LUKS / crypttab evidence</source>
        <translation>Dowody LUKS / crypttab</translation>
    </message>
    <message>
        <source>Shows LUKS/mapped ancestry plus crypttab and fstab mapper references.</source>
        <translation>Pokazuje LUKS / mapped przodków plus crypttab i referencje fstab mapper.</translation>
    </message>
    <message>
        <source>Full diagnostic report</source>
        <translation>Pełny raport diagnostyczny</translation>
    </message>
    <message>
        <source>Combines all read-only diagnostics for the selected scope (same as Run All).</source>
        <translation>Łączy całą diagnostykę tylko do odczytu dla wybranego zakresu (tak samo jak Run All).</translation>
    </message>
    <message>
        <source>All entries</source>
        <translation>Wszystkie wpisy</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>Diagnostyka</translation>
    </message>
    <message>
        <source>Repairs</source>
        <translation>Naprawa</translation>
    </message>
    <message>
        <source>Package repair</source>
        <translation>Naprawa opakowań</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Kopia pliku</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Odkrycie urządzenia</translation>
    </message>
    <message>
        <source>Host</source>
        <translation>Host</translation>
    </message>
    <message>
        <source>Required for block-device inventory</source>
        <translation>Wymagane do inwentaryzacji urządzeń blokujących</translation>
    </message>
    <message>
        <source>Filesystem identification</source>
        <translation>Identyfikacja systemu plików</translation>
    </message>
    <message>
        <source>Used to identify filesystem metadata</source>
        <translation>Używane do identyfikacji metadanych systemu plików</translation>
    </message>
    <message>
        <source>Mount inspection</source>
        <translation>Kontrola montowania</translation>
    </message>
    <message>
        <source>Used to understand active mounts</source>
        <translation>Zrozumienie aktywnych wzniesień</translation>
    </message>
    <message>
        <source>LUKS support</source>
        <translation>Obsługa LUKS</translation>
    </message>
    <message>
        <source>Required to unlock encrypted targets</source>
        <translation>Wymagane do odblokowania zaszyfrowanych celów</translation>
    </message>
    <message>
        <source>Btrfs support</source>
        <translation>Obsługa Btrfs</translation>
    </message>
    <message>
        <source>Required for Btrfs inspection and snapshot rollback</source>
        <translation>Wymagane do kontroli Btrfs i przewijania migawki</translation>
    </message>
    <message>
        <source>Bidirectional file copy</source>
        <translation>Kopia plików dwukierunkowych</translation>
    </message>
    <message>
        <source>Host/Repair</source>
        <translation>Host / Naprawa</translation>
    </message>
    <message>
        <source>Required for verified Host-to-Repair and Repair-to-Host transfer</source>
        <translation>Wymagane dla zweryfikowanych Host- to- Naprawa i Naprawa - to- Transfer hosta</translation>
    </message>
    <message>
        <source>Chroot repair</source>
        <translation>Naprawa korzenia</translation>
    </message>
    <message>
        <source>Required for target-side repair commands</source>
        <translation>Wymagane dla poleceń naprawy po stronie docelowej</translation>
    </message>
    <message>
        <source>Offline systemd repair</source>
        <translation>Naprawa systemowa offline</translation>
    </message>
    <message>
        <source>Used to restore graphical.target and the configured display manager without starting the target GUI</source>
        <translation>Przywracanie grafiki. docelowy i skonfigurowany menedżer wyświetlania bez uruchamiania docelowego GUI</translation>
    </message>
    <message>
        <source>UEFI NVRAM inspection</source>
        <translation>Kontrola UEFI NVRAM</translation>
    </message>
    <message>
        <source>Used to preserve target EFI BootOrder during TUXEDO UKI rebuilds when efivars are available</source>
        <translation>Używane do zachowania celu EFI BootOrder podczas przebudowy TUXEDO UKI, gdy efivars są dostępne</translation>
    </message>
    <message>
        <source>UKI verification</source>
        <translation>Weryfikacja UKI</translation>
    </message>
    <message>
        <source>Used to verify the kernel embedded in a rebuilt unified kernel image</source>
        <translation>Używany do weryfikacji jądra osadzonego w odbudowanym, ujednoliconym obrazie jądra</translation>
    </message>
    <message>
        <source>GRUB EFI repair</source>
        <translation>GRUB EFI naprawa</translation>
    </message>
    <message>
        <source>Target/Host</source>
        <translation>Cel / Host</translation>
    </message>
    <message>
        <source>Required only for conventional GRUB-based EFI systems</source>
        <translation>Wymagane tylko dla konwencjonalnych systemów EFI opartych na GRUBX</translation>
    </message>
    <message>
        <source>Target</source>
        <translation>Cel</translation>
    </message>
    <message>
        <source>Debian-family GRUB helper</source>
        <translation>Debian- family GRUB helper</translation>
    </message>
    <message>
        <source>Portable GRUB configuration generator used by Arch and other non-Debian systems</source>
        <translation>Przenośny generator konfiguracyjny GRUB stosowany przez systemy Arch i inne systemy nie-Debiana</translation>
    </message>
    <message>
        <source>Initramfs rebuild</source>
        <translation>Odbudowa Initramps</translation>
    </message>
    <message>
        <source>Debian-family initramfs helper</source>
        <translation>Debian- family initramfs helper</translation>
    </message>
    <message>
        <source>Arch-family initramfs generator</source>
        <translation>Generator initramfs</translation>
    </message>
    <message>
        <source>Alternative initramfs generator used by Arch and other distributions</source>
        <translation>Alternatywny generator initramfs stosowany przez łuk i inne dystrybucje</translation>
    </message>
    <message>
        <source>Initramfs verification</source>
        <translation>Weryfikacja initrampów</translation>
    </message>
    <message>
        <source>Read-only verification for mkinitcpio images</source>
        <translation>Weryfikacja tylko odczytu obrazów mkinitcpio</translation>
    </message>
    <message>
        <source>Read-only verification for dracut images</source>
        <translation>Weryfikacja tylko odczytu obrazów dracut</translation>
    </message>
    <message>
        <source>systemd-boot inspection</source>
        <translation>Kontrola systemd-boot</translation>
    </message>
    <message>
        <source>Host/Target</source>
        <translation>Host / Target</translation>
    </message>
    <message>
        <source>Read-only inspection of systemd-boot and generic UKI layouts</source>
        <translation>Kontrola tylko systemd-boot i układów typu UKI</translation>
    </message>
    <message>
        <source>Arch package manager</source>
        <translation>Kierownik pakietu Arch</translation>
    </message>
    <message>
        <source>Arch-family package database and transaction tool</source>
        <translation>Baza danych i narzędzie transakcji Arch- family</translation>
    </message>
    <message>
        <source>DKMS rebuild</source>
        <translation>Odbudowa DKMS</translation>
    </message>
    <message>
        <source>Required only when target uses DKMS modules</source>
        <translation>Wymagane tylko wtedy, gdy cel wykorzystuje moduły DKMS</translation>
    </message>
    <message>
        <source>LVM inspection</source>
        <translation>Kontrola LVM</translation>
    </message>
    <message>
        <source>Optional LVM storage-stack support</source>
        <translation>Opcjonalne wsparcie storagestock LVM</translation>
    </message>
    <message>
        <source>Software RAID</source>
        <translation>RAID oprogramowania</translation>
    </message>
    <message>
        <source>Optional Linux MD RAID support</source>
        <translation>Opcjonalne wsparcie Linux MD RAID</translation>
    </message>
    <message>
        <source>Process namespace isolation</source>
        <translation>Izolacja przestrzeni nazw procesu</translation>
    </message>
    <message>
        <source>Host+Target</source>
        <translation>Host + Cel</translation>
    </message>
    <message>
        <source>The ported helper falls back to a guarded plain chroot when unshare is absent</source>
        <translation>Przywieziony pomocnik wraca do chronionego chroota, gdy brak akcji</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Anuluj</translation>
    </message>
    <message>
        <source>OK</source>
        <translation>OK</translation>
    </message>
    <message>
        <source>Yes</source>
        <translation>Tak.</translation>
    </message>
    <message>
        <source>No</source>
        <translation>Nie.</translation>
    </message>
    <message>
        <source>read-only helper diagnostic</source>
        <translation>diagnostyka wyłącznie pomocnicza</translation>
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
        <translation>Linux recovery and boot- naprawa narzędzia</translation>
    </message>
    <message>
        <source>GUARDED REPAIR</source>
        <translation>DOCHODY Z GWARANCJI</translation>
    </message>
    <message>
        <source>Ordinary repairs require an explicitly selected non-host target. The protected running host has a separate deliberate maintenance mode with the same guarded repair stages and requires privilege authorization.</source>
        <translation>Zwykłe naprawy wymagają wyraźnie wybranego celu nie-gospodarza. Chroniony gospodarz posiada osobny tryb konserwacji z tymi samymi, strzeżonymi etapami naprawy i wymaga autoryzacji przywileju.</translation>
    </message>
    <message>
        <source>Systems</source>
        <translation>Systemy</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>Diagnostyka</translation>
    </message>
    <message>
        <source>Repair</source>
        <translation>Naprawa</translation>
    </message>
    <message>
        <source>Chroot Shell</source>
        <translation>Chroot Shell</translation>
    </message>
    <message>
        <source>File Copy</source>
        <translation>Kopiuj</translation>
    </message>
    <message>
        <source>Logs</source>
        <translation>Logi</translation>
    </message>
    <message>
        <source>Settings</source>
        <translation>Ustawienia</translation>
    </message>
    <message>
        <source>(not created yet)</source>
        <translation>(jeszcze nie stworzony)</translation>
    </message>
    <message>
        <source>Information</source>
        <translation>Informacje</translation>
    </message>
    <message>
        <source>Close</source>
        <translation>Zamknij</translation>
    </message>
    <message>
        <source>&amp;File</source>
        <translation>&amp;Plik</translation>
    </message>
    <message>
        <source>&amp;Refresh Devices</source>
        <translation>&amp;Urządzenia do odświeżania</translation>
    </message>
    <message>
        <source>&amp;Lock Administrator Session</source>
        <translation>&amp;Lock Administrator Sesja</translation>
    </message>
    <message>
        <source>&amp;Quit</source>
        <translation>&amp;Zakończ</translation>
    </message>
    <message>
        <source>&amp;View</source>
        <translation>&amp;Widok</translation>
    </message>
    <message>
        <source>&amp;Systems</source>
        <translation>&amp;Systemy</translation>
    </message>
    <message>
        <source>&amp;Diagnostics</source>
        <translation>&amp;Diagnostyka</translation>
    </message>
    <message>
        <source>&amp;Logs</source>
        <translation>&amp;Logi</translation>
    </message>
    <message>
        <source>&amp;Settings</source>
        <translation>&amp;Ustawienia</translation>
    </message>
    <message>
        <source>&amp;Auto-size Device Columns</source>
        <translation>&amp;Kolumny urządzeń</translation>
    </message>
    <message>
        <source>&amp;Wrap Log Lines</source>
        <translation>&amp;Linie dziennika zawijania</translation>
    </message>
    <message>
        <source>&amp;Help</source>
        <translation>&amp;Pomoc</translation>
    </message>
    <message>
        <source>&amp;Using Boot Bitch</source>
        <translation>&amp;Stosowanie leku Boot Bitch</translation>
    </message>
    <message>
        <source>&amp;About Boot Bitch</source>
        <translation>&amp;O Boot Bitch</translation>
    </message>
    <message>
        <source>Device columns auto-sized. Drag headers to fine-tune widths.</source>
        <translation>Kolumny urządzenia są automatycznie wielkości. Przeciągnij nagłówki do szerokości melodii.</translation>
    </message>
    <message>
        <source>A helper command is running; wait for it to finish before locking the session.</source>
        <translation>Polecenie pomocnika jest uruchomione; czekać na jego zakończenie przed zablokowaniem sesji.</translation>
    </message>
    <message>
        <source>Administrator session locked; the next privileged action will request authorization.</source>
        <translation>Sesja administratora zablokowana; kolejne uprzywilejowane działanie będzie wymagać autoryzacji.</translation>
    </message>
    <message>
        <source>Using Boot Bitch</source>
        <translation>Korzystanie z Boot Bitch</translation>
    </message>
    <message>
        <source>Boot Bitch must run from a different booted Linux environment than the system being repaired. Use a Linux live medium or another Linux installation on a different physical drive.&lt;br&gt;&lt;br&gt;The running host is protected from ordinary repair-target selection, but it can be explicitly selected through &lt;b&gt;Host Maintenance&lt;/b&gt; for guarded native diagnostics and supported maintenance stages.&lt;br&gt;&lt;br&gt;Diagnostics follow the Systems page: the committed repair drive while Host Maintenance is off, or the protected running host while it is active.&lt;br&gt;&lt;br&gt;The first privileged action requests administrator authorization once for this Boot Bitch window; &lt;b&gt;File - Lock Administrator Session&lt;/b&gt; ends that helper session immediately. Every repair keeps the helper&apos;s own runtime preflights.</source>
        <translation>Boot Bitch musi działać z innego środowiska Linuksa niż system, który jest naprawiany. Użyj nośnika na żywo Linuksa lub innej instalacji na innym dysku fizycznym. &lt; br &gt; &lt; br &gt; Prowadzący host jest chroniony przed zwykłą selekcją celów naprawy, ale może być wyraźnie wybrany przez &lt; b &gt; Konserwacja hosta &lt; / b &gt; dla strzeżonej rodzimej diagnostyki i wspieranych etapów konserwacji. &lt; br &gt; &lt; br &gt; Diagnostyka postępuje zgodnie ze stroną systemową: dedykowany napęd naprawczy podczas wyłączania obsługi technicznej lub chroniony host podczas pracy. &lt; br &gt; &lt; br &gt; Pierwsze uprzywilejowane działanie wymaga autoryzacji administratora raz dla tego okna Boot Bitch; &lt; b &gt; Plik - Zablokuj sesję Administratora &lt; / b &gt; natychmiast kończy tę sesję pomocniczą. Każda naprawa utrzymuje własne przedloty pomocnika.</translation>
    </message>
    <message>
        <source>Select a physical drive; Boot Bitch resolves the most likely Linux system volume automatically. The running host stays protected from ordinary target repairs, with a separate explicit host-maintenance path for its own system. The Details button shows the protected host&apos;s facts in the details pane; selecting any drive row restores the per-drive pane.</source>
        <translation>Wybierz napęd fizyczny; Boot Bitch automatycznie rozwiązuje najbardziej prawdopodobny wolumin systemu Linux. Prowadzący host pozostaje chroniony przed zwykłymi naprawami docelowymi, z oddzielną wyraźną ścieżką utrzymania hosta dla własnego systemu. Przycisk Szczegóły pokazuje fakty chronionego hosta w tafli szczegółów; wybierając dowolny wiersz napędu przywraca taflę per- drive.</translation>
    </message>
    <message>
        <source>Refresh Devices</source>
        <translation>Urządzenia odświeżające</translation>
    </message>
    <message>
        <source>Re-read the read-only kernel inventory (/proc/partitions, /proc/mounts, /proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* and the udev metadata database). No block device is opened and nothing is written.</source>
        <translation>Odczytaj ponownie wykaz tylko do odczytu jądra (/ proc / partytions, / proc / mounts, / proc / swaps, / sys / block, / dev / mapper, / dev / disk / by- * i bazę metadanych udev). Żadne urządzenie blokowe nie jest otwarte i nic nie jest napisane.</translation>
    </message>
    <message>
        <source>[OK]</source>
        <translation>[OK]</translation>
    </message>
    <message>
        <source>The running system was detected and stays protected from ordinary target repairs.</source>
        <translation>System uruchomiony został wykryty i jest chroniony przed zwykłymi naprawami.</translation>
    </message>
    <message>
        <source>Detecting running system...</source>
        <translation>Wykrywam system...</translation>
    </message>
    <message>
        <source>Detecting protected storage...</source>
        <translation>Wykrywanie chronionej pamięci masowej...</translation>
    </message>
    <message>
        <source>PROTECTED</source>
        <translation>OCHRONA</translation>
    </message>
    <message>
        <source>The running host remains protected from ordinary repair-target operations.</source>
        <translation>Prowadzący host pozostaje chroniony przed zwykłymi operacjami naprowadzania.</translation>
    </message>
    <message>
        <source>Details</source>
        <translation>Szczegóły</translation>
    </message>
    <message>
        <source>Show read-only details for the protected running host.</source>
        <translation>Pokaż tylko szczegóły dla chronionego hosta.</translation>
    </message>
    <message>
        <source>Host Maintenance</source>
        <translation>Konserwacja hosta</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Domyślne</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host (menu.lst default directive with a backup and rollback). Requires Host Maintenance and the cached host-default probe.</source>
        <translation>Dodać Canonical zainstalowany wpis jądra domyślnym GRUB- legacy wpis boot na uruchomionym komputerze (menu.lst domyślna dyrektywa z kopii zapasowej i rollback). Wymaga konserwacji komputera i buforowanej sondy host- default.</translation>
    </message>
    <message>
        <source>Available repair targets</source>
        <translation>Dostępne cele naprawy</translation>
    </message>
    <message>
        <source>Most likely first</source>
        <translation>Najprawdopodobniej pierwszy</translation>
    </message>
    <message>
        <source>Drives are listed by the read-only inventory. Select a row to inspect it; Select Target commits the selected non-host drive with its auto-resolved Linux root component.</source>
        <translation>Napędy są wymienione w spisie tylko do odczytu. Wybierz wiersz, aby go sprawdzić; Wybierz Target zobowiązuje wybrany napęd nie-host z jego auto- rozwiązany komponent główny Linux.</translation>
    </message>
    <message>
        <source>Device</source>
        <translation>Urządzenie</translation>
    </message>
    <message>
        <source>Size</source>
        <translation>Rozmiar</translation>
    </message>
    <message>
        <source>Type</source>
        <translation>Rodzaj</translation>
    </message>
    <message>
        <source>Filesystem</source>
        <translation>System plików</translation>
    </message>
    <message>
        <source>Select Target</source>
        <translation>Wybierz cel</translation>
    </message>
    <message>
        <source>Unlock</source>
        <translation>Odblokuj</translation>
    </message>
    <message>
        <source>Authorize</source>
        <translation>Autoryzacja</translation>
    </message>
    <message>
        <source>Establish the privileged helper session for the current scope now instead of waiting for the next privileged action.</source>
        <translation>Ustanowienie uprzywilejowanej sesji pomocniczej dla obecnego zakresu, zamiast czekania na kolejne uprzywilejowane działanie.</translation>
    </message>
    <message>
        <source>Committed target: none</source>
        <translation>Cel: brak</translation>
    </message>
    <message>
        <source>Unlock state for the selected drive; the LUKS passphrase is never logged.</source>
        <translation>Odblokuj stan wybranego dysku; passphrase LUKS nigdy nie jest zalogowany.</translation>
    </message>
    <message>
        <source>Unlock status</source>
        <translation>Odblokuj status</translation>
    </message>
    <message>
        <source>Read-only inventory plus helper-confirmed facts; mirrors the modern Qt6 Selected drive details panel.</source>
        <translation>Read- only invent plus helper- potwierdzone fakty; lusterka nowoczesny panel Qt6 Wybrane szczegóły napędu.</translation>
    </message>
    <message>
        <source>Selected drive details</source>
        <translation>Wybrane szczegóły dysku</translation>
    </message>
    <message>
        <source>Field</source>
        <translation>Pole</translation>
    </message>
    <message>
        <source>Value</source>
        <translation>Wartość</translation>
    </message>
    <message>
        <source>Run All runs every available read-only diagnostic for the current scope; selecting a check runs it alone. Diagnostics are read-only and are the only evidence source for the gated repair actions.</source>
        <translation>Uruchom Wszystkie uruchamia każdą dostępną diagnostykę tylko do odczytu dla bieżącego zakresu; wybierając czek uruchamia ją samodzielnie. Diagnostyka jest tylko read- i są jedynym źródłem dowodów dla działań związanych z naprawą.</translation>
    </message>
    <message>
        <source>Target: none selected</source>
        <translation>Cel: nie wybrano żadnego</translation>
    </message>
    <message>
        <source>Diagnostics follow the committed repair target, or the protected running host while Host Maintenance is active.</source>
        <translation>Diagnostyka postępuje zgodnie z zadeklarowanym celem naprawy, lub chronionym hostem podczas konserwacji hosta jest aktywny.</translation>
    </message>
    <message>
        <source>Run All</source>
        <translation>Uruchom wszystkie</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope.</source>
        <translation>Uruchom wszystkie - uruchom każdą dostępną diagnostykę tylko dla bieżącego zakresu.</translation>
    </message>
    <message>
        <source>Target configuration:</source>
        <translation>Konfiguracja celu:</translation>
    </message>
    <message>
        <source>Etch-era target configuration files; availability is probed read-only by the helper&apos;s diagnostics.</source>
        <translation>Pliki konfiguracyjne Etchera; dostępność jest sprawdzana tylko przez diagnostykę pomocnika.</translation>
    </message>
    <message>
        <source>Edit Target File...</source>
        <translation>Edytuj plik docelowy...</translation>
    </message>
    <message>
        <source>Runs one read-only diagnostic for the selected scope through the helper (`diagnose &lt;key&gt;` / `host-diagnose &lt;key&gt;`); Run All is the combined report.</source>
        <translation>Przeprowadza jedną diagnostykę tylko do odczytu dla wybranego zakresu za pomocą helpera (&apos;diagnoza &lt; key &gt;&apos; / &apos;diagnoza host- &lt; key &gt;&apos;); Run All jest połączonym raportem.</translation>
    </message>
    <message>
        <source>Diagnostic checks</source>
        <translation>Kontrole diagnostyczne</translation>
    </message>
    <message>
        <source>Check</source>
        <translation>Sprawdź</translation>
    </message>
    <message>
        <source>Selected diagnostic</source>
        <translation>Wybrana diagnostyka</translation>
    </message>
    <message>
        <source>Select a diagnostic</source>
        <translation>Wybierz diagnostykę</translation>
    </message>
    <message>
        <source>Choose a diagnostic from the list.</source>
        <translation>Wybierz diagnostykę z listy.</translation>
    </message>
    <message>
        <source>Ready</source>
        <translation>Gotowe</translation>
    </message>
    <message>
        <source>Results</source>
        <translation>Wyniki</translation>
    </message>
    <message>
        <source>Run Diagnostic</source>
        <translation>Uruchom diagnostykę</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic for the current scope.</source>
        <translation>Uruchom Diagnostic - uruchom wybraną diagnostykę tylko do odczytu dla bieżącego zakresu.</translation>
    </message>
    <message>
        <source>Copy Results</source>
        <translation>Kopiuj wyniki</translation>
    </message>
    <message>
        <source>Save Results...</source>
        <translation>Zapisz wyniki...</translation>
    </message>
    <message>
        <source>The Full Repair plan runs the selected legacy stages in order through the guarded helper; the individual tools run one stage at a time. Every action stays disabled until the cached capability lines say available and the helper keeps its runtime preflights.</source>
        <translation>Plan Pełna Naprawa prowadzi wybrane dotychczasowe etapy w porządku przez strzeżonego pomocnika; poszczególne narzędzia działają jeden etap na raz. Każda akcja pozostaje wyłączona do czasu, gdy linie funkcji cached mówią, że są dostępne, a pomocnik utrzymuje swoje loty przed startem.</translation>
    </message>
    <message>
        <source>Full Repair plan</source>
        <translation>Pełny plan naprawy</translation>
    </message>
    <message>
        <source>No stages selected</source>
        <translation>Nie wybrano etapów</translation>
    </message>
    <message>
        <source>Configure Plan...</source>
        <translation>Konfiguracja planu...</translation>
    </message>
    <message>
        <source>Open Settings to choose which Full Repair stages are part of the plan.</source>
        <translation>Otwórz ustawienia, aby wybrać, które pełne etapy naprawy są częścią planu.</translation>
    </message>
    <message>
        <source>Run Full Repair</source>
        <translation>Uruchom pełną naprawę</translation>
    </message>
    <message>
        <source>Select a repair drive, or choose Host Maintenance on the protected running-host card.</source>
        <translation>Wybierz napęd naprawczy, lub wybierz Maintenance Host na chronionej karcie running- host.</translation>
    </message>
    <message>
        <source>Stage</source>
        <translation>Etap</translation>
    </message>
    <message>
        <source>Individual repair tools</source>
        <translation>Indywidualne narzędzia naprawcze</translation>
    </message>
    <message>
        <source>Tool</source>
        <translation>Narzędzie</translation>
    </message>
    <message>
        <source>Full Repair</source>
        <translation>Pełna naprawa</translation>
    </message>
    <message>
        <source>not reported</source>
        <translation>niezgłoszone</translation>
    </message>
    <message>
        <source>Selected tool</source>
        <translation>Wybrane narzędzie</translation>
    </message>
    <message>
        <source>Select a repair tool</source>
        <translation>Wybierz narzędzie naprawcze</translation>
    </message>
    <message>
        <source>Run Tool</source>
        <translation>Uruchom narzędzie</translation>
    </message>
    <message>
        <source>Select a tool to review its repair action.</source>
        <translation>Wybierz narzędzie do przeglądu jego działania naprawczego.</translation>
    </message>
    <message>
        <source>Write actions ask for confirmation and then run the helper&apos;s own runtime preflights; the GUI never weakens them. A repair that is not proven &apos;unchanged&apos; invalidates the cached diagnostics and disables the gated actions until diagnostics run again.</source>
        <translation>Pisać działania poprosić o potwierdzenie, a następnie uruchomić własne przedlotów pomocnika; GUI nigdy ich nie osłabia. Naprawa, która nie jest udowodniona jako &quot;niezmieniona&quot;, unieważnia diagnostykę buforowaną i wyłącza działania faliste aż do ponownego uruchomienia diagnostyki.</translation>
    </message>
    <message>
        <source>Chroot shell</source>
        <translation>Skorupa korzeniowa</translation>
    </message>
    <message>
        <source>Offline commands run one at a time in a fresh chroot and cannot answer interactive prompts (apt-get -y upgrade works). Host-shell commands run directly on the running host. The helper&apos;s probe lines gate the command field; the exact reason appears in the tooltip.</source>
        <translation>Polecenia offline uruchamiają jeden na raz w nowym chroot i nie mogą odpowiadać na interaktywne polecenia (działa apt- get -y upgrade). Polecenia host- shell uruchamiane bezpośrednio na uruchomionym komputerze. Sonda pomocnika otwiera pole dowodzenia; dokładny powód pojawia się w podpowiedzi.</translation>
    </message>
    <message>
        <source>No &apos;Legacy feature shell:&apos; line is cached; run diagnostics for the selected scope to evaluate the helper&apos;s chroot/timeout containment probes (fail closed).</source>
        <translation>Nie ma powłoki &apos;Legacy faction shell:&apos; line is cached; run diagnostics for the selected scope to assessment the helper &apos;s chroot / timeout conduction sonds (fail closed).</translation>
    </message>
    <message>
        <source>Command</source>
        <translation>Polecenie</translation>
    </message>
    <message>
        <source>Command:</source>
        <translation>Polecenie:</translation>
    </message>
    <message>
        <source>One reviewed command string, passed to the helper as a single argument (no shell interpolation by the GUI).</source>
        <translation>Jeden sprawdzony łańcuch poleceń, przekazywany do helpera jako pojedynczy argument (brak interpolacji powłoki przez GUI).</translation>
    </message>
    <message>
        <source>Run Command</source>
        <translation>Uruchom polecenie</translation>
    </message>
    <message>
        <source>Clear Output</source>
        <translation>Wyczyść wyjście</translation>
    </message>
    <message>
        <source>The helper exposes `shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for an offline target chroot and `host-shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for the running host. Both keep the helper&apos;s runtime preflights; this tab enables the command only when the scope is committed, the session is authorized and the scope&apos;s Legacy feature probe reports available.</source>
        <translation>Pomocnik ujawnia powłokę &lt; dysk &gt; &lt; root &gt; &lt; command &gt; &apos;dla chroot celu offline i&apos; host- shell &lt; disk &gt; &lt; root &gt; &lt; command &gt; &apos;dla bieżącego komputera. Zarówno zachować przedlotów pomocnika; ta zakładka umożliwia polecenie tylko wtedy, gdy zakres jest zaangażowany, sesja jest autoryzowana i zakres Legacy funkcji sonda raporty dostępne.</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Kopia pliku</translation>
    </message>
    <message>
        <source>Copy and verify files in either direction through the guarded helper (cp -a plus ownership restoration and a per-file byte-compare). The helper&apos;s file-copy probe gates the controls and keeps the direction and path containment checks.</source>
        <translation>Kopiuj i weryfikuj pliki w obu kierunkach przez strzeżonego pomocnika (cp -a plus renowacja własności i per- file byte- compare). Sonda do kopiowania plików pomocnika blokuje sterowanie i kontroluje kierunek i trasę.</translation>
    </message>
    <message>
        <source>Preview Changes</source>
        <translation>Podgląd zmian</translation>
    </message>
    <message>
        <source>Run a copy dry-run through the guarded helper. No files are changed.</source>
        <translation>Przepuść kopię przez strzeżonego pomocnika. Nie zmieniono plików.</translation>
    </message>
    <message>
        <source>Copy and Verify</source>
        <translation>Kopiuj i weryfikuj</translation>
    </message>
    <message>
        <source>Copy staged items and verify the result. Existing destination names are overwritten when source content differs; unrelated destination files are never deleted.</source>
        <translation>Kopiuj elementy i weryfikuj wynik. Istniejące nazwy miejsc docelowych są nadpisywane, gdy zawartość źródła jest różna; niepowiązane pliki miejsc przeznaczenia nigdy nie są usuwane.</translation>
    </message>
    <message>
        <source>Direction:</source>
        <translation>Kierunek:</translation>
    </message>
    <message>
        <source>Choose which system supplies the source files and which system receives them.</source>
        <translation>Wybierz, który system dostarcza pliki źródłowe i który system je otrzymuje.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from this host</source>
        <translation>1. Wybierz pliki źródłowe lub foldery z tego komputera</translation>
    </message>
    <message>
        <source>Source</source>
        <translation>Źródło</translation>
    </message>
    <message>
        <source>Files and folders staged for the verified copy. The legacy backend copies with cp -a and restores ownership with chown --reference; every regular file is byte-compared after the copy.</source>
        <translation>Pliki i foldery ustawione dla zweryfikowanej kopii. Kopie backend z cp -a i przywraca własność z chown --reference; każdy zwykły plik jest porównywany po kopii.</translation>
    </message>
    <message>
        <source>Add Files...</source>
        <translation>Dodaj pliki...</translation>
    </message>
    <message>
        <source>Add Folder...</source>
        <translation>Dodaj folder...</translation>
    </message>
    <message>
        <source>Remove</source>
        <translation>Usuń</translation>
    </message>
    <message>
        <source>Clear</source>
        <translation>Czysto.</translation>
    </message>
    <message>
        <source>Clear the staged source list (nothing is copied or deleted).</source>
        <translation>Wyczyść listę źródeł (nic nie jest kopiowane lub usunięte).</translation>
    </message>
    <message>
        <source>2. Choose destination in repaired system</source>
        <translation>2. Wybierz cel w naprawionym systemie</translation>
    </message>
    <message>
        <source>unavailable: see the helper&apos;s Legacy feature file-copy: probe reason above</source>
        <translation>niedostępny: patrz funkcja legacy pomocnika - kopia pliku: powód sondy powyżej</translation>
    </message>
    <message>
        <source>An absolute path inside the selected repair system (Host to Repair) or on the running host (Repair to Host).</source>
        <translation>Ścieżka bezwzględna wewnątrz wybranego systemu naprawy (Host do naprawy) lub na działającym hoście (Naprawa do Hosta).</translation>
    </message>
    <message>
        <source>Browse Target Folders...</source>
        <translation>Przeglądaj foldery docelowe...</translation>
    </message>
    <message>
        <source>Browse the selected repair system through the helper&apos;s temporary read-only mounts and choose an absolute destination path. No target files are changed while browsing.</source>
        <translation>Przejrzyj wybrany system naprawy poprzez tymczasowe podpory pomocnika i wybierz bezwzględną ścieżkę docelową. Podczas przeglądania nie zmienia się plików docelowych.</translation>
    </message>
    <message>
        <source>3. Ownership and copy policy</source>
        <translation>3. Polityka własności i kopiowania</translation>
    </message>
    <message>
        <source>Ownership:</source>
        <translation>Własność:</translation>
    </message>
    <message>
        <source>Smart destination ownership (recommended)</source>
        <translation>Inteligentna własność miejsca przeznaczenia (zalecane)</translation>
    </message>
    <message>
        <source>Preserve source numeric UID/GID</source>
        <translation>Zachowaj źródło numeryczne UID / GID</translation>
    </message>
    <message>
        <source>Smart mode validates UID/GID identity mapping across the two systems and falls back to the destination-directory owner when the same numeric ID means a different account (the legacy backend implements it with chown --reference).</source>
        <translation>Tryb Smart potwierdza mapowanie tożsamości UID / GID w obu systemach i wraca do właściciela katalogu destination-, gdy ten sam identyfikator numeryczny oznacza inne konto (dotychczasowy backend implementuje go z chown -- reference).</translation>
    </message>
    <message>
        <source>Application log</source>
        <translation>Dziennik aplikacji</translation>
    </message>
    <message>
        <source>The complete session register; Save As... writes every entry even while a filter hides lines. If a writable system share mount exists at /host, Save As... starts there; otherwise the log directory is the fallback. Prior session files are listed read-only.</source>
        <translation>Kompletny rejestr sesji; Zapisz jako... pisze każdy wpis, nawet gdy filtr ukrywa linie. Jeśli w / host istnieje monta współdzielona systemowa, Save As... rozpoczyna się tam; w przeciwnym razie katalog dziennika jest rezerwowy. Poprzednie pliki sesji są wyświetlane tylko do odczytu.</translation>
    </message>
    <message>
        <source>Session logs</source>
        <translation>Rejestry sesji</translation>
    </message>
    <message>
        <source>Session</source>
        <translation>Sesja</translation>
    </message>
    <message>
        <source>The first entry is the live session; earlier files in the log directory are listed read-only below it.</source>
        <translation>Pierwszy wpis to sesja na żywo; wcześniejsze pliki w katalogu dziennika są wymienione tylko poniżej.</translation>
    </message>
    <message>
        <source>New Session Log</source>
        <translation>Nowy dziennik sesji</translation>
    </message>
    <message>
        <source>Close the active session file; it becomes a prior session and the next log entry starts a new file.</source>
        <translation>Zamknij aktywny plik sesji; stanie się on sesją poprzedzającą i następny wpis dziennika rozpocznie nowy plik.</translation>
    </message>
    <message>
        <source>Add Note</source>
        <translation>Dodaj notatkę</translation>
    </message>
    <message>
        <source>Append a NOTE entry to the live session register.</source>
        <translation>Dołącz wpis NOTE do rejestru sesji na żywo.</translation>
    </message>
    <message>
        <source>Delete</source>
        <translation>Usuń</translation>
    </message>
    <message>
        <source>Delete the selected prior session file (the live session is never deleted).</source>
        <translation>Usuń wybrany plik poprzedniej sesji (sesja na żywo nigdy nie jest usunięta).</translation>
    </message>
    <message>
        <source>Refresh</source>
        <translation>Odśwież</translation>
    </message>
    <message>
        <source>Save As...</source>
        <translation>Zapisz jako...</translation>
    </message>
    <message>
        <source>Save the complete session log (all entries, not just the current filter).</source>
        <translation>Zapisz cały dziennik sesji (wszystkie wpisy, nie tylko bieżący filtr).</translation>
    </message>
    <message>
        <source>Clear Register</source>
        <translation>Wyczyść rejestr</translation>
    </message>
    <message>
        <source>Clear the live register and view; prior session files are never modified.</source>
        <translation>Wyczyść rejestr i podgląd na żywo; pliki poprzedniej sesji nigdy nie są modyfikowane.</translation>
    </message>
    <message>
        <source>Search log:</source>
        <translation>Dziennik wyszukiwania:</translation>
    </message>
    <message>
        <source>Type any characters to show matching log entries (case-insensitive). Save As always writes every entry.</source>
        <translation>Wpisz wszystkie znaki, aby wyświetlić pasujące wpisy dziennika (niewrażliwe na przypadek). Zapisz Jak zawsze pisze każdy wpis.</translation>
    </message>
    <message>
        <source>Filter:</source>
        <translation>Filtr:</translation>
    </message>
    <message>
        <source>Filter the visible log by entry kind. Selecting a diagnostic section shows the lines captured for that section; a workflow filter such as File system repair or Package repair shows its mapped repair lines (File copy has no lines in this frontend). Save As always writes every entry.</source>
        <translation>Filtruj widoczny dziennik według rodzaju wpisu. Wybór sekcji diagnostycznej pokazuje linie uchwycone dla tej sekcji; filtr przepływu pracy, taki jak naprawa systemu plików lub naprawa pakietu, pokazuje swoje mapowane linie naprawcze (kopia pliku nie ma linii w tym froncie). Zapisz Jak zawsze pisze każdy wpis.</translation>
    </message>
    <message>
        <source>Settings are stored per user under ~/.qt/, one file per settings group (devicesrc, logsrc, diagnosticsrc, repairrc), and are saved immediately on every change and on close. Launch the GUI as the same user to keep your overrides; a GUI started as root keeps its own copies.</source>
        <translation>Ustawienia są zapisywane na użytkownika w pozycji ~ / .qt /, jeden plik na grupę ustawień (devicesrc, logsrc, diagnosticsrc, naprawa) i są zapisywane natychmiast przy każdej zmianie i w pobliżu. Uruchom GUI jako tego samego użytkownika, aby utrzymać swoje nadjazdy; GUI zaczął jako root zachowuje własne kopie.</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Odkrycie urządzenia</translation>
    </message>
    <message>
        <source>Show devices without an identified Linux installation</source>
        <translation>Pokaż urządzenia bez zidentyfikowanej instalacji Linuksa</translation>
    </message>
    <message>
        <source>Show removable and USB storage</source>
        <translation>Pokaż demontowalną i pamięć masową USB</translation>
    </message>
    <message>
        <source>Show encrypted devices before unlocking</source>
        <translation>Pokaż zaszyfrowane urządzenia przed odblokowaniem</translation>
    </message>
    <message>
        <source>When off, drives without a visible Linux filesystem are hidden unless they still contain an encrypted device and encrypted devices are shown.</source>
        <translation>Po wyłączeniu, dyski bez widocznego systemu plików Linux są ukryte, chyba że nadal zawierają zaszyfrowane urządzenie i są wyświetlane zaszyfrowane urządzenia.</translation>
    </message>
    <message>
        <source>When off, removable and USB drives are hidden from the repair-target list.</source>
        <translation>Po wyłączeniu dyski wymienne i USB są ukryte z listy docelowej naprawy.</translation>
    </message>
    <message>
        <source>When off, drives with an encrypted device are hidden until the volume is unlocked.</source>
        <translation>Po wyłączeniu dyski z zaszyfrowanym urządzeniem są ukryte do czasu odblokowania głośności.</translation>
    </message>
    <message>
        <source>Include the %1 stage in the Full Repair plan. The stage runs in the plan order shown on the Repair tab.</source>
        <translation>Włączyć etap %1 do planu pełnego naprawy. Etap działa w kolejności planu pokazanej na karcie Napraw.</translation>
    </message>
    <message>
        <source>Automatically regenerate read-only diagnostics after repairs or target changes</source>
        <translation>Automatycznie regeneruj diagnostykę tylko do odczytu po naprawach lub zmianach docelowych</translation>
    </message>
    <message>
        <source>Regenerates the cached read-only diagnostics for the current scope after an operation that invalidates them (LUKS unlock, target configuration edit). It runs only inside an already authorized administrator session and never opens an authorization prompt by itself.</source>
        <translation>Regeneruje diagnostykę tylko do odczytu dla bieżącego zakresu po operacji, która je unieważnia (LUKS odblokować, docelowa edycja konfiguracji). Działa tylko wewnątrz już autoryzowanej sesji administratora i nigdy nie otwiera samego polecenia autoryzacji.</translation>
    </message>
    <message>
        <source>Wrap long log lines</source>
        <translation>Zawiń długie wiersze dziennika</translation>
    </message>
    <message>
        <source>Mandatory safety controls</source>
        <translation>Obowiązkowe kontrole bezpieczeństwa</translation>
    </message>
    <message>
        <source>Host capabilities and dependencies</source>
        <translation>Możliwości i zależności hosta</translation>
    </message>
    <message>
        <source>Refresh Capabilities</source>
        <translation>Odśwież możliwości</translation>
    </message>
    <message>
        <source>Re-run the read-only host capability probes (a PATH search, nothing is executed) and refresh the distribution summary.</source>
        <translation>Uruchom ponownie sondy funkcji tylko read- host (wyszukiwanie PATH, nic nie jest wykonywane) i odśwież podsumowanie dystrybucji.</translation>
    </message>
    <message>
        <source>Install Missing Support...</source>
        <translation>Zainstaluj brakujące wsparcie...</translation>
    </message>
    <message>
        <source>Automatic installation will require explicit package mapping and privilege authorization.</source>
        <translation>Automatyczna instalacja wymaga wyraźnego mapowania pakietów i autoryzacji przywileju.</translation>
    </message>
    <message>
        <source>Application configuration</source>
        <translation>Konfiguracja aplikacji</translation>
    </message>
    <message>
        <source>Boot Bitch Legacy</source>
        <translation>Boot Bitch Legacy</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list first.</source>
        <translation>Wybierz napęd fizyczny na liście dostępnych celów naprawy.</translation>
    </message>
    <message>
        <source>Protected system</source>
        <translation>System chroniony</translation>
    </message>
    <message>
        <source>The running system cannot be selected as a repair target. Use Host Maintenance for the protected running host or choose another disk.</source>
        <translation>System uruchomiony nie może być wybrany jako cel naprawczy. Użyj Maintenance hosta dla chronionego komputera lub wybierz inny dysk.</translation>
    </message>
    <message>
        <source>Unlock or select a Linux system first</source>
        <translation>Odblokuj lub wybierz najpierw system Linux</translation>
    </message>
    <message>
        <source>This encrypted drive has no visible Linux filesystem yet. Use Unlock, refresh devices, and select the target after its Linux root is detected.</source>
        <translation>Ten zaszyfrowany dysk nie ma jeszcze widocznego systemu plików Linux. Użyj Odblokuj, odśwież urządzenia i wybierz cel po wykryciu jego głównego Linuksa.</translation>
    </message>
    <message>
        <source>The selected root component (%1) belongs to the running system and cannot be committed as a repair target. Use Host Maintenance for the protected running host.</source>
        <translation>Wybrany składnik root (%1) należy do systemu operacyjnego i nie może być uznany za cel naprawczy. Użyj Maintenance hosta dla chronionego hosta.</translation>
    </message>
    <message>
        <source>repair target commit</source>
        <translation>cel naprawy commit</translation>
    </message>
    <message>
        <source>Repair drive selected: %1</source>
        <translation>Wybrany napęd naprawczy: %1</translation>
    </message>
    <message>
        <source>; best detected system component: %1</source>
        <translation>; najlepiej wykryty element systemu: %1</translation>
    </message>
    <message>
        <source>. No mount or repair action was performed.</source>
        <translation>. Nie dokonano żadnych działań montowania ani naprawy.</translation>
    </message>
    <message>
        <source>Make Default unavailable</source>
        <translation>Domyślne niedostępne</translation>
    </message>
    <message>
        <source>Make Default is a running-host action on this frontend; enter Host Maintenance first.</source>
        <translation>Make Default to akcja running- host na tym froncie; najpierw wprowadź Maintenance Host.</translation>
    </message>
    <message>
        <source>Administrator authorization required</source>
        <translation>Wymagane zezwolenie administratora</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize first.</source>
        <translation>Sesja administratora nie jest aktywna; najpierw naciśnij Autoryzuj.</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host?

The helper verifies /boot/grub/menu.lst, sets the `default &lt;N&gt;` directive to the canonical entry, backs the menu up first and restores it on any failure.</source>
        <translation>Dodać Canonical zainstalowany wpis jądra domyślnym GRUB- legacy wpis boot na uruchomionym komputerze?

Helper weryfikuje / boot / grub / menu.lst, ustawia dyrektywę &apos;domyślnie &lt; N &gt;&apos; do wpisu kanonicznego, najpierw cofa menu i przywraca je przy każdej awarii.</translation>
    </message>
    <message>
        <source>Host Maintenance unavailable</source>
        <translation>Konserwacja hosta niedostępna</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a committed repair target or a detected running host.</source>
        <translation>Nie można było wykryć docelowej liczby goszczącej; diagnostyka wymaga wyznaczonego celu naprawczego lub wykrytego hosta.</translation>
    </message>
    <message>
        <source>Exit Host Maintenance</source>
        <translation>Wyjście Konserwacja hosta</translation>
    </message>
    <message>
        <source>Host Maintenance is active; diagnostics and gated repairs target the protected running host.</source>
        <translation>Host Maintenance jest aktywny; diagnostyka i naprawy faliste skierowane do chronionego hosta biegania.</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list on the Systems tab first, or use Host Maintenance for the protected running host.</source>
        <translation>Wybierz napęd fizyczny na liście Dostępne cele naprawy w zakładce Systemy najpierw, lub użyj Maintenance Host dla chronionego hosta.</translation>
    </message>
    <message>
        <source>The selected drive (%1) is the protected running host. Choose Host Maintenance on the Systems tab to run read-only host diagnostics and guarded host repairs; ordinary target repairs stay disabled.</source>
        <translation>Wybrany dysk (%1) jest chronionym hostem. Wybierz Maintenance Host w zakładce Systems, aby przeprowadzić diagnostykę tylko dla nosicieli i naprawy z ochroną; zwykłe naprawy docelowe pozostają wyłączone.</translation>
    </message>
    <message>
        <source>No repair target is committed. Choose Select Target on the Systems tab (or Host Maintenance for the protected running host) first.</source>
        <translation>Nie ma celu naprawczego. Wybierz najpierw wybrany cel w zakładce Systemy (lub utrzymanie hosta dla chronionego hosta).</translation>
    </message>
    <message>
        <source>The selection changed after the target was committed. Choose Select Target again.</source>
        <translation>Wybór zmienił się po realizacji celu. Wybierz ponownie cel.</translation>
    </message>
    <message>
        <source>Diagnostics scope required</source>
        <translation>Wymagany zakres diagnostyki</translation>
    </message>
    <message>
        <source>Diagnostics scope unresolved</source>
        <translation>Zakres diagnostyki nierozwiązany</translation>
    </message>
    <message>
        <source>The running host target could not be resolved; use Refresh Devices and commit a repair target or re-enter Host Maintenance.</source>
        <translation>Bieżący goniec docelowy nie mógł zostać rozwiązany; używać urządzeń Refresh i popełnić cel naprawy lub ponownie wprowadzić Host Maintenance.</translation>
    </message>
    <message>
        <source>Diagnostic check required</source>
        <translation>Wymagana kontrola diagnostyczna</translation>
    </message>
    <message>
        <source>Select a diagnostic check in the list first.</source>
        <translation>Wybierz najpierw kontrolę diagnostyczną na liście.</translation>
    </message>
    <message>
        <source>Repair target required</source>
        <translation>Wymagany cel naprawy</translation>
    </message>
    <message>
        <source>Target file editing needs a committed repair target. Running-host maintenance has no target-file editing; commit an offline target first.</source>
        <translation>Edycja pliku docelowego wymaga wyznaczonego celu naprawy. Konserwacja Running- host nie ma edycji pliku celowego-; commit najpierw cel offline.</translation>
    </message>
    <message>
        <source>Configuration file required</source>
        <translation>Wymagany plik konfiguracyjny</translation>
    </message>
    <message>
        <source>Select a target configuration file first.</source>
        <translation>Wybierz najpierw docelowy plik konfiguracyjny.</translation>
    </message>
    <message>
        <source>Edit target %1</source>
        <translation>Edytuj docelowy %1</translation>
    </message>
    <message>
        <source>Edit this target file through the guarded administrator helper. A successful save invalidates cached diagnostics; rerun diagnostics before repair. Generated files such as /boot/grub/menu.lst may be replaced by the next bootloader update.</source>
        <translation>Edytuj ten plik docelowy przez opiekuna administratora. Udane zapisać unieważnia diagnostykę cached; ponownie uruchomić diagnostykę przed naprawą. Generowane pliki, takie jak / boot / grub / menu.lst, mogą zostać zastąpione przez następną aktualizację bootloadera.</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Anuluj</translation>
    </message>
    <message>
        <source>Save Target File</source>
        <translation>Zapisz plik docelowy</translation>
    </message>
    <message>
        <source>No changes to %1.</source>
        <translation>Brak zmian w %1.</translation>
    </message>
    <message>
        <source>Configuration write refused</source>
        <translation>Odmowa zapisu konfiguracji</translation>
    </message>
    <message>
        <source>The edited content contains NUL bytes; the guarded write refuses it.</source>
        <translation>Edytowana zawartość zawiera bajty NUL; zastrzeżony zapis odmawia.</translation>
    </message>
    <message>
        <source>File too large</source>
        <translation>Plik zbyt duży</translation>
    </message>
    <message>
        <source>The edited file is larger than 1 MiB. The guarded write refuses it; edit the file from a console instead.</source>
        <translation>Edytowany plik jest większy niż 1 MiB. Chroniony zapis odmawia, zamiast tego edytuje plik z konsoli.</translation>
    </message>
    <message>
        <source>Write target configuration</source>
        <translation>Zapisz konfigurację docelową</translation>
    </message>
    <message>
        <source>Write the edited contents to %1? This modifies the repair target and invalidates cached diagnostics.</source>
        <translation>Zapisać edytowaną zawartość do %1? To modyfikuje cel naprawy i unieważnia diagnostykę cached.</translation>
    </message>
    <message>
        <source>No diagnostic results to copy yet.</source>
        <translation>Nie ma jeszcze wyników diagnostycznych.</translation>
    </message>
    <message>
        <source>Diagnostic results copied to the clipboard.</source>
        <translation>Wyniki diagnostyczne skopiowane do schowka.</translation>
    </message>
    <message>
        <source>No diagnostic results to save yet.</source>
        <translation>Nie ma jeszcze wyników diagnostycznych.</translation>
    </message>
    <message>
        <source>Text files (*.txt);;All files (*)</source>
        <translation>Pliki tekstowe (* .txt);; Wszystkie pliki (*)</translation>
    </message>
    <message>
        <source>Save diagnostic results</source>
        <translation>Zapisz wyniki diagnostyczne</translation>
    </message>
    <message>
        <source>Could not write %1.</source>
        <translation>Nie można zapisać %1.</translation>
    </message>
    <message>
        <source>Diagnostic results saved to %1</source>
        <translation>Wyniki diagnostyczne zapisane do %1</translation>
    </message>
    <message>
        <source>Unlock not available</source>
        <translation>Odblokuj niedostępny</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked. Select an offline repair target to unlock.</source>
        <translation>Chroniony gospodarz nie może być odblokowany. Wybierz cel naprawy offline do odblokowania.</translation>
    </message>
    <message>
        <source>No locked LUKS component is currently visible on this selected drive.</source>
        <translation>Na wybranym dysku nie jest obecnie widoczny żaden zablokowany komponent LUKS.</translation>
    </message>
    <message>
        <source>Confirm LUKS unlock</source>
        <translation>Potwierdź odblokowanie LUKS</translation>
    </message>
    <message>
        <source>Unlock %1 on %2?

The helper opens a temporary device-mapper mapping with cryptsetup and keeps it open for this recovery session. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>Odblokować %1 na %2?

Pomocnik otwiera tymczasowe mapowanie device- mapper z kryptsetup i utrzymuje je otwarte dla tej sesji odzyskiwania. Passsphrase podróżuje przez prywatny plik keyfile i nigdy nie jest umieszczony w argumentach poleceń lub logach.</translation>
    </message>
    <message>
        <source>LUKS unlock</source>
        <translation>LUKS odblokuj</translation>
    </message>
    <message>
        <source>Unlock LUKS repair target</source>
        <translation>Odblokuj cel naprawy LUKS</translation>
    </message>
    <message>
        <source>Enter the passphrase for %1.

It is sent only to cryptsetup over the helper&apos;s standard input and is never logged or placed on a command line.</source>
        <translation>Wprowadź hasło dla %1.

Jest on wysyłany tylko do szyfrowania nad standardowym wejściem pomocnika i nigdy nie jest zalogowany ani umieszczony na linii poleceń.</translation>
    </message>
    <message>
        <source>Passphrase required</source>
        <translation>Wymagane hasło</translation>
    </message>
    <message>
        <source>An empty passphrase was not submitted. Enter the LUKS passphrase or choose Cancel.</source>
        <translation>Nie zgłoszono pustego hasła. Wprowadź hasło LUKS lub wybierz Anuluj.</translation>
    </message>
    <message>
        <source>Unlock keyfile unavailable</source>
        <translation>Odblokuj niedostępny plik klawiszy</translation>
    </message>
    <message>
        <source>The LUKS passphrase could not be written to a private keyfile in %1; the unlock was not started.</source>
        <translation>Passphrase LUKS nie można zapisać do prywatnego pliku kluczowego w %1; odblokowanie nie zostało uruchomione.</translation>
    </message>
    <message>
        <source>Configuration write unavailable</source>
        <translation>Konfiguracja zapisu niedostępna</translation>
    </message>
    <message>
        <source>The edited content could not be written to a private temporary file in %1; the write was not started.</source>
        <translation>Nie można zapisać edytowanej treści do prywatnego pliku tymczasowego w %1; zapis nie został uruchomiony.</translation>
    </message>
    <message>
        <source>ERROR: cannot read the session log %1</source>
        <translation>ERROR: nie można odczytać dziennika sesji %1</translation>
    </message>
    <message>
        <source>Viewing a prior session log (read-only): %1</source>
        <translation>Przeglądanie dziennika poprzedniej sesji (tylko read-): %1</translation>
    </message>
    <message>
        <source>Session log list refreshed.</source>
        <translation>Odświeżenie listy dzienników sesji.</translation>
    </message>
    <message>
        <source>Note:</source>
        <translation>Uwaga:</translation>
    </message>
    <message>
        <source>Delete session log</source>
        <translation>Usuń dziennik sesji</translation>
    </message>
    <message>
        <source>Delete %1 permanently? This cannot be undone.</source>
        <translation>Usunąć %1 na stałe? Tego nie da się cofnąć.</translation>
    </message>
    <message>
        <source>%1 changed while the confirmation was open; the delete was refused.</source>
        <translation>%1 zmienił się podczas otwarcia potwierdzenia; usunięto go.</translation>
    </message>
    <message>
        <source>Unable to delete %1.</source>
        <translation>Nie można usunąć %1.</translation>
    </message>
    <message>
        <source>Device discovery filters updated.</source>
        <translation>Zaktualizowane filtry wyszukiwania urządzeń.</translation>
    </message>
    <message>
        <source>Unknown Linux distribution</source>
        <translation>Nieznana dystrybucja Linuksa</translation>
    </message>
    <message>
        <source>sudo / gksu (no KAuth on this frontend)</source>
        <translation>sudo / gksu (brak KAuth na tym froncie)</translation>
    </message>
    <message>
        <source>Available</source>
        <translation>Dostępne</translation>
    </message>
    <message>
        <source>Missing on this frontend</source>
        <translation>Zaginęła na tym froncie</translation>
    </message>
    <message>
        <source>Missing</source>
        <translation>Brak</translation>
    </message>
    <message>
        <source>No repair tool is selected.</source>
        <translation>Nie wybrano żadnego narzędzia naprawczego.</translation>
    </message>
    <message>
        <source>A helper command is already running.</source>
        <translation>Polecenie pomocy już działa.</translation>
    </message>
    <message>
        <source>Restore Graphical Login is a host-scope stage on this legacy frontend; enter Host Maintenance to run it.</source>
        <translation>Przywracanie logowania graficznego to etap host- scope na tym dziedzicznym froncie; wprowadź Maintenance Host, aby go uruchomić.</translation>
    </message>
    <message>
        <source>The selected scope has no resolved root component; use Refresh Devices and commit the repair target again.</source>
        <translation>Wybrany zakres nie zawiera rozwiązanego elementu podstawowego; użyj urządzeń Refresh i ponownie wyłącz cel naprawy.</translation>
    </message>
    <message>
        <source>This legacy frontend exposes no %1 action; the helper reports the capability as available.</source>
        <translation>Ten dziedziczny front nie ujawnia żadnych działań %1; pomocnik zgłasza zdolność, jak dostępne.</translation>
    </message>
    <message>
        <source>Run this guarded repair action using the cached read-only diagnostic evidence. A confirmation is shown first.</source>
        <translation>Uruchom to strzeżone działanie naprawcze, używając tylko odczytanych dowodów diagnostycznych. Najpierw zostanie pokazane potwierdzenie.</translation>
    </message>
    <message>
        <source>Enabled in Settings</source>
        <translation>Włączone w ustawieniach</translation>
    </message>
    <message>
        <source>Disabled in Settings - enable it to include this stage</source>
        <translation>Wyłącz w ustawieniach - włącz ten etap</translation>
    </message>
    <message>
        <source>Unavailable: %1</source>
        <translation>Niedostępne: %1</translation>
    </message>
    <message>
        <source>
Unavailable: %1</source>
        <translation>
Niedostępne: %1</translation>
    </message>
    <message>
        <source>Repair tool unavailable</source>
        <translation>Narzędzie do naprawy niedostępne</translation>
    </message>
    <message>
        <source>Confirm repair</source>
        <translation>Potwierdź naprawa</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Your saved selection is kept and its availability is re-checked when diagnostics for this scope complete.</source>
        <translation>Nie ma jeszcze dowodów na tę scenę. Zapisany wybór jest zachowany, a jego dostępność jest ponownie sprawdzana, gdy diagnostyka tego zakresu zakończona.</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Run diagnostics for the selected scope to populate the Full Repair plan; your selection is saved once the stage becomes available.</source>
        <translation>Nie ma jeszcze dowodów na tę scenę. Uruchom diagnostykę dla wybranego zakresu, aby zapełnić plan pełnej naprawy; Twój wybór jest zapisany po udostępnieniu sceny.</translation>
    </message>
    <message>
        <source>Unknown Full Repair stage.</source>
        <translation>Nieznany etap pełnej naprawy.</translation>
    </message>
    <message>
        <source>No Full Repair stages are selected or available; use Configure Plan... to choose the stages.</source>
        <translation>Nie wybiera się ani nie udostępnia pełnych etapów naprawy; użyj Konfiguracja planu... do wyboru etapów.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab to establish the session.</source>
        <translation>Wymagane jest zezwolenie administratora; naciśnij Autoryzacja na karcie Systemy lub Napraw, aby rozpocząć sesję.</translation>
    </message>
    <message>
        <source>Runs the selected stages using the cached read-only diagnostic evidence after privilege confirmation.</source>
        <translation>Przeprowadza wybrane etapy używając tylko odczytanych dowodów diagnostycznych po potwierdzeniu przywileju.</translation>
    </message>
    <message>
        <source>No Full Repair stages selected - use Configure Plan... or Settings.</source>
        <translation>Nie wybrano pełnych etapów naprawy - użyj Konfiguracja planu... lub ustawień.</translation>
    </message>
    <message>
        <source>1 stage selected</source>
        <translation>Wybrany 1 etap</translation>
    </message>
    <message>
        <source>%1 stages selected</source>
        <translation>Wybrane etapy %1</translation>
    </message>
    <message>
        <source>No repair stages are selected. Use Configure Plan... to choose the stages Full Repair will run.</source>
        <translation>Nie wybrano żadnych etapów naprawy. Użyj Konfiguracja planu... aby wybrać etapy Pełna Naprawa będzie działać.</translation>
    </message>
    <message>
        <source>No selected stages are available. %1</source>
        <translation>Nie są dostępne wybrane etapy. %1</translation>
    </message>
    <message>
        <source>Full Repair unavailable</source>
        <translation>Pełna naprawa niedostępna</translation>
    </message>
    <message>
        <source>Run the Full Repair plan?

The selected stages run in order through the helper&apos;s guarded repair command:

</source>
        <translation>Uruchomić plan pełnej naprawy?

Wybrane etapy przebiegają w kolejności przez polecenie naprawy strzeżonego pomocnika:

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
Pomocnik utrzymuje każdy bieg przed lotem; etap, który zawiedzie zatrzymuje plan.</translation>
    </message>
    <message>
        <source>Running %1 through the privileged helper... The Logs tab keeps the complete transcript.</source>
        <translation>Prowadzenie %1 przez uprzywilejowanego pomocnika... Karta Logs zachowuje kompletny zapis.</translation>
    </message>
    <message>
        <source>Shell scope required</source>
        <translation>Wymagany zakres Shell</translation>
    </message>
    <message>
        <source>No administrator session is active.

Press Authorize on the Systems or Repair tab to establish the session, or enter Host Maintenance / commit a repair target on the Systems tab; the chroot shell then reuses the cached authorization.</source>
        <translation>Sesja administratora nie jest aktywna.

Naciśnij przycisk Autoryzacja na karcie Systemy lub Napraw, aby utworzyć sesję, lub wprowadź Host Maintenance / commit cel naprawy na karcie Systemy; powłoka chroot następnie ponownie wykorzystuje uprawnienia buforowane.</translation>
    </message>
    <message>
        <source>Shell command required</source>
        <translation>Wymagane polecenie Shell</translation>
    </message>
    <message>
        <source>Enter the command to run first.</source>
        <translation>Wprowadź polecenie, które ma być uruchomione jako pierwsze.</translation>
    </message>
    <message>
        <source>Confirm running-host command</source>
        <translation>Potwierdź polecenie running- host</translation>
    </message>
    <message>
        <source>Run this command as root on the protected running host?

%1

The helper keeps its runtime preflights; the command is passed as one argument and is never interpreted by the GUI.</source>
        <translation>Uruchom to polecenie jako root na chronionym komputerze?

%1

Pomocnik utrzymuje swoje loty przedterminowe; komenda jest przekazywana jako jeden argument i nigdy nie jest interpretowana przez GUI.</translation>
    </message>
    <message>
        <source>Running %1...</source>
        <translation>Uruchomienie %1...</translation>
    </message>
    <message>
        <source>Authorization scope required</source>
        <translation>Wymagany zakres autoryzacji</translation>
    </message>
    <message>
        <source>administrator authorization</source>
        <translation>Upoważnienie administratora</translation>
    </message>
    <message>
        <source>Administrator authorization</source>
        <translation>Upoważnienie administratora</translation>
    </message>
    <message>
        <source>Administrator authorization is required for %1.

Enter the password for %2 (sudo). It is used only for this sudo authentication, is sent over a pipe and is never logged or placed on a command line. The authorization is cached for this session and reused by diagnostics and repairs.</source>
        <translation>Dla %1 wymagane jest zezwolenie administratora.

Wprowadź hasło dla %2 (sudo). Jest używany tylko do uwierzytelniania sudo, jest wysyłany przez rurę i nigdy nie jest zalogowany lub umieszczony na linii poleceń. Upoważnienie jest buforowane do tej sesji i ponownie wykorzystywane przez diagnostykę i naprawy.</translation>
    </message>
    <message>
        <source>your account</source>
        <translation>Twoje konto</translation>
    </message>
    <message>
        <source>Password required</source>
        <translation>Wymagane hasło</translation>
    </message>
    <message>
        <source>An empty password was not submitted. Enter the sudo password or choose Cancel.</source>
        <translation>Nie przesłano pustego hasła. Wprowadź hasło sudo lub wybierz Anuluj.</translation>
    </message>
    <message>
        <source>Administrator authorization failed</source>
        <translation>Upoważnienie administratora zawiodło</translation>
    </message>
    <message>
        <source>sudo did not accept the password: %1

The command was not started.</source>
        <translation>sudo nie zaakceptował hasła: %1

Polecenie nie zostało uruchomione.</translation>
    </message>
    <message>
        <source>elevation requires an interactive sudo password; run the smoke as root or after `sudo -S -v` with --elevate &apos;sudo -n&apos;</source>
        <translation>elewacja wymaga interaktywnego hasła sudo; uruchom dym jako root lub po &apos;sudo -S -v&apos; z --elevate &apos;sudo -n&apos;</translation>
    </message>
    <message>
        <source>No administrator session is active for %1.

Press Authorize on the Systems or Repair tab to establish the session now, or enter Host Maintenance / commit a repair target on the Systems tab; diagnostics and repairs then reuse the cached authorization.</source>
        <translation>Dla %1 nie jest aktywna sesja administratora.

Naciśnij przycisk Autoryzacja na karcie Systemów lub Napraw, aby rozpocząć sesję teraz, lub wprowadzić Host Maintenance / popełnić cel naprawy na karcie Systemów; diagnostyka i naprawy następnie ponownie używać uprawnienia buforowanego.</translation>
    </message>
    <message>
        <source>Administrator authorization expired</source>
        <translation>Upoważnienie administratora wygasło</translation>
    </message>
    <message>
        <source>The cached administrator authorization for %1 expired or was refused.

Press Authorize on the Systems or Repair tab to re-establish the session, then run the command again. No command was started.</source>
        <translation>Upoważnienie administratora dla %1 wygasło lub zostało odrzucone.

Naciśnij Autoryzacja na karcie Systemy lub Napraw, aby ponownie ustanowić sesję, a następnie uruchomić polecenie ponownie. Nie uruchomiono żadnego dowództwa.</translation>
    </message>
    <message>
        <source>Privileged operation completed successfully. Administrator authorization remains active for this session.</source>
        <translation>Uprzywilejowana operacja zakończona pomyślnie. Upoważnienie administratora pozostaje aktywne w tej sesji.</translation>
    </message>
    <message>
        <source>Privileged operation stopped with an error. Administrator authorization remains active; review the output before closing.</source>
        <translation>Uprzywilejowana operacja zatrzymała się z błędem. Upoważnienie administratora pozostaje aktywne; należy sprawdzić wynik przed zamknięciem.</translation>
    </message>
    <message>
        <source>Inspection is read-only.

</source>
        <translation>Inspekcja jest gotowa.

</translation>
    </message>
    <message>
        <source>Configuration unavailable</source>
        <translation>Konfiguracja niedostępna</translation>
    </message>
    <message>
        <source>The helper could not read %1:

%2</source>
        <translation>Pomocnik nie mógł odczytać %1:

%2</translation>
    </message>
    <message>
        <source>Configuration write failed</source>
        <translation>Błąd zapisu konfiguracji</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
Result: mapping opened for this recovery session.</source>
        <translation>Państwo: otwarte
Składnik: %1
Mapper: %2
Metoda: helper odblokować (cryptsetup; passphrase poprzez mode- 600 keyfile, usunięte po użyciu)
Wynik: Mapowanie otwarte dla tej sesji odzyskiwania.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: the passphrase was not accepted; retry offered.</source>
        <translation>Państwo: zablokowane
Składnik: %1
Metoda: odblokowanie helpera (cryptsetup)
Błąd: passphrase nie zostało zaakceptowane; ponownie spróbować.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: %2</source>
        <translation>Państwo: zablokowane
Składnik: %1
Metoda: odblokowanie helpera (cryptsetup)
Błąd: %2</translation>
    </message>
    <message>
        <source>Passphrase not accepted</source>
        <translation>Passphrase nieakceptowane</translation>
    </message>
    <message>
        <source>The LUKS passphrase was not accepted.

Try again?</source>
        <translation>Passphraza LUKS nie została przyjęta.

Jeszcze raz?</translation>
    </message>
    <message>
        <source>[-] %1 - not run (the plan stopped before reaching this stage)</source>
        <translation>[-] %1 - nie uruchomić (plan zatrzymał się przed osiągnięciem tego etapu)</translation>
    </message>
    <message>
        <source>filesystem</source>
        <translation>system plików</translation>
    </message>
    <message>
        <source>[-] %1 - no file system errors found - no changes</source>
        <translation>[-] %1 - brak błędów systemu plików - brak zmian</translation>
    </message>
    <message>
        <source>[FAIL] %1 - not reported</source>
        <translation>[FAIL] %1 - niezgłoszone</translation>
    </message>
    <message>
        <source>%1 stage(s) failed; review the helper output in Logs.</source>
        <translation>Etap (-y) %1 nie powiódł się; sprawdź wyjście helpera w logach.</translation>
    </message>
    <message>
        <source>the plan stopped before any stage completed.</source>
        <translation>plan zatrzymał się przed zakończeniem jakiegokolwiek etapu.</translation>
    </message>
    <message>
        <source>no repair was needed; cached diagnostics remain valid.</source>
        <translation>nie było konieczne naprawa; diagnostyka buforowa pozostaje ważna.</translation>
    </message>
    <message>
        <source>repair completed; cached diagnostics were invalidated and must be regenerated.</source>
        <translation>zakończono naprawę; diagnostykę buforową unieważniono i należy ją zregenerować.</translation>
    </message>
    <message>
        <source>%1 results: [OK] %2 successful | [FAIL] %3 failed | [-] %4 no repair needed - %5</source>
        <translation>Wyniki %1: [OK] %2 z sukcesem: 124; [FAIL] %3 nie powiodło się: 124; [-] %4 nie wymaga naprawy - %5</translation>
    </message>
    <message>
        <source>Re-run Diagnostic</source>
        <translation>Ponownie uruchomić diagnostykę</translation>
    </message>
    <message>
        <source>Unavailable</source>
        <translation>Niedostępne</translation>
    </message>
    <message>
        <source>Commit this physical drive as the repair target.</source>
        <translation>Podajcie ten napęd fizyczny jako cel naprawczy.</translation>
    </message>
    <message>
        <source>Protection:</source>
        <translation>Ochrona:</translation>
    </message>
    <message>
        <source>Host shell</source>
        <translation>Powłoka hosta</translation>
    </message>
    <message>
        <source>Run on Host</source>
        <translation>Uruchom na komputerze</translation>
    </message>
    <message>
        <source>Host Shell</source>
        <translation>Host Shell</translation>
    </message>
    <message>
        <source>The read-only probe found no editable target configuration file in this target. %1</source>
        <translation>Sonda tylko do odczytu nie znalazła edytowalnego pliku konfiguracyjnego. %1</translation>
    </message>
    <message>
        <source>Not present in the selected target (omitted): %1</source>
        <translation>Nieobecny w wybranym celu (pominięty): %1</translation>
    </message>
    <message>
        <source>Run diagnostics to probe which target configuration files exist; the helper&apos;s read-only probe decides the list.</source>
        <translation>Uruchom diagnostykę, aby zbadać, które pliki konfiguracyjne docelowe istnieją; sonda pomocnika tylko read- decyduje o liście.</translation>
    </message>
    <message>
        <source>Probed read-only by the helper; a saved edit invalidates the cached diagnostics.</source>
        <translation>Probed read- tylko przez pomocnika; zapisywana edycja unieważnia diagnostykę cached.</translation>
    </message>
    <message>
        <source>Host maintenance: %1</source>
        <translation>Obsługa techniczna: %1</translation>
    </message>
    <message>
        <source>unresolved</source>
        <translation>nierozwiązane</translation>
    </message>
    <message>
        <source>Target: %1 + %2</source>
        <translation>Cel: %1 + %2</translation>
    </message>
    <message>
        <source>Host command</source>
        <translation>Polecenie hosta</translation>
    </message>
    <message>
        <source>Execute a command on the running host as root.</source>
        <translation>Wykonaj polecenie na uruchomionym komputerze jako root.</translation>
    </message>
    <message>
        <source>Execute a command inside the selected repair system as root.</source>
        <translation>Wykonaj polecenie wewnątrz wybranego systemu naprawy jako root.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab (or re-enter Host Maintenance / re-commit the repair target) to authorize this session.</source>
        <translation>Wymagane jest zezwolenie administratora; naciśnij przycisk Autoryzacja na karcie Systemy lub Napraw (lub wpisz ponownie Konserwację Hosta / ponownie wyłącz cel naprawy), aby autoryzować tę sesję.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root on the running host through the helper&apos;s guarded host-shell verb.</source>
        <translation>Uruchom jedną komendę jako root na uruchomionym komputerze poprzez strzeżony czasownik powłoki host-.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root inside the target chroot through the helper&apos;s guarded shell verb.</source>
        <translation>Uruchom jedną komendę jako root wewnątrz chroot docelowy poprzez strzeżony czasownik powłoki pomocnika.</translation>
    </message>
    <message>
        <source>Run a command on the running host as root (sudo is not needed). Commands are executed directly on the active system; output is kept in this window and in the application log.</source>
        <translation>Uruchom polecenie na uruchomionym komputerze jako root (sudo nie jest potrzebne). Polecenia są wykonywane bezpośrednio w systemie aktywnym; wyjście jest przechowywane w tym oknie i w dzienniku aplikacji.</translation>
    </message>
    <message>
        <source>Run a command inside the selected repair system as root (sudo is not needed). Commands are executed one at a time in a fresh chroot and cannot answer interactive prompts; use non-interactive flags such as apt-get -y upgrade. Output is kept in this window and in the application log.</source>
        <translation>Uruchom polecenie wewnątrz wybranego systemu naprawy jako root (sudo nie jest potrzebne). Polecenia są wykonywane pojedynczo w nowym chroocie i nie mogą odpowiadać na interaktywne zapowiedzi; używają nieinteraktywnych flag, takich jak apt- get -y upgrade. Wyjście jest przechowywane w tym oknie i w dzienniku aplikacji.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize or re-enter Host Maintenance / commit a repair target.</source>
        <translation>Nie jest aktywna sesja administratora; naciśnij Autoryzuj lub wpisz ponownie Host Maintenance / commit cel naprawy.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from the repaired system</source>
        <translation>1. Wybierz pliki źródłowe lub foldery z naprawionego systemu</translation>
    </message>
    <message>
        <source>2. Choose destination on this host</source>
        <translation>2. Wybierz cel na tym komputerze</translation>
    </message>
    <message>
        <source>Add File Path...</source>
        <translation>Dodaj ścieżkę pliku...</translation>
    </message>
    <message>
        <source>Add Folder Path...</source>
        <translation>Dodaj ścieżkę folderu...</translation>
    </message>
    <message>
        <source>Browse...</source>
        <translation>Przeglądaj...</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it (Host Maintenance does not provide a repair tree to browse).</source>
        <translation>Wybierz napęd naprawczy w Systemach przed wybraniem miejsca przeznaczenia wewnątrz niego (Host Maintenance nie zapewnia drzewa naprawczego do przeglądania).</translation>
    </message>
    <message>
        <source>Choose a host destination folder directly.</source>
        <translation>Wybierz bezpośrednio folder docelowy hosta.</translation>
    </message>
    <message>
        <source>Copy the staged files and folders with cp -a, restore ownership with chown --reference and byte-compare every regular file afterwards.</source>
        <translation>Kopiuj pliki i foldery z cp -a, przywracaj własność z chowem --reference i byte- porównuj następnie każdy plik regularny.</translation>
    </message>
    <message>
        <source>File Copy unavailable</source>
        <translation>Plik Kopiuj niedostępny</translation>
    </message>
    <message>
        <source>Choose host destination folder</source>
        <translation>Wybierz folder docelowy serwera</translation>
    </message>
    <message>
        <source>Select a repair target</source>
        <translation>Wybierz cel naprawy</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it.</source>
        <translation>Wybierz napęd naprawczy w Systemach przed wyborem miejsca docelowego wewnątrz niego.</translation>
    </message>
    <message>
        <source>File Copy - Browse Target Folders</source>
        <translation>Kopia pliku - Przeglądaj foldery docelowe</translation>
    </message>
    <message>
        <source>Browse Target Folders</source>
        <translation>Przeglądaj foldery docelowe</translation>
    </message>
    <message>
        <source>The helper could not list the repair-system folder:

%1</source>
        <translation>Pomocnik nie mógł wymienić folderu systemowego:

%1</translation>
    </message>
    <message>
        <source>BROWSE_ENTRY</source>
        <translation>BRAK WPISU</translation>
    </message>
    <message>
        <source>(choose this folder: %1)</source>
        <translation>(wybierz ten folder: %1)</translation>
    </message>
    <message>
        <source>(parent folder)</source>
        <translation>(folder macierzysty)</translation>
    </message>
    <message>
        <source>Select repair-system destination</source>
        <translation>Wybierz cel systemu naprawy</translation>
    </message>
    <message>
        <source>Choose a destination folder inside the repaired system (current folder: %1):</source>
        <translation>Wybierz folder docelowy wewnątrz naprawionego systemu (bieżący folder: %1):</translation>
    </message>
    <message>
        <source>Folder</source>
        <translation>Katalog</translation>
    </message>
    <message>
        <source>Open</source>
        <translation>Otwórz</translation>
    </message>
    <message>
        <source>(choose this folder:</source>
        <translation>(Wybierz ten folder:</translation>
    </message>
    <message>
        <source>Add files to copy</source>
        <translation>Dodaj pliki do kopiowania</translation>
    </message>
    <message>
        <source>Add folder to copy</source>
        <translation>Dodaj folder do kopiowania</translation>
    </message>
    <message>
        <source>Stage at least one source and name a destination path first.</source>
        <translation>Staw co najmniej jedno źródło i najpierw podaj ścieżkę docelową.</translation>
    </message>
    <message>
        <source>Copy the %1 staged item(s) to %2?

The helper keeps its direction and path containment checks; a sensitive repair-system destination is refused unless the helper approves it, and every regular file is byte-compared after the copy.</source>
        <translation>Skopiować element (y) %1 na %2?

Pomocnik utrzymuje kontrolę kierunku i trasy; wrażliwe miejsce docelowe systemu napraw jest odrzucane, chyba że pomocnik go zatwierdzi, a każdy zwykły plik jest porównywany po kopii.</translation>
    </message>
    <message>
        <source>File Copy - Copy and Verify</source>
        <translation>Kopiuj - Kopiuj i weryfikuj</translation>
    </message>
    <message>
        <source>File Copy - Preview Changes</source>
        <translation>Kopia pliku - Podgląd zmian</translation>
    </message>
    <message>
        <source>Protected running host - details</source>
        <translation>Chroniony host running - szczegóły</translation>
    </message>
    <message>
        <source>not mounted (offline target)</source>
        <translation>niezamontowane (cel offline)</translation>
    </message>
    <message>
        <source>Select a drive to see its details.</source>
        <translation>Wybierz dysk, aby zobaczyć jego szczegóły.</translation>
    </message>
    <message>
        <source>Inspecting the selected component.</source>
        <translation>Kontrola wybranego elementu.</translation>
    </message>
    <message>
        <source>Helper-confirmed by the last read-only diagnostics.</source>
        <translation>Pomocna potwierdzona przez ostatnią diagnostykę.</translation>
    </message>
    <message>
        <source>Read-only inventory only; run diagnostics to confirm.</source>
        <translation>Tylko do odczytu; uruchomić diagnostykę w celu potwierdzenia.</translation>
    </message>
    <message>
        <source>PROTECTED - running system; read-only details only</source>
        <translation>PROTECTED - system operacyjny; tylko więcej szczegółów</translation>
    </message>
    <message>
        <source>Live / installer media - not selectable</source>
        <translation>Media live / installer - nie do wyboru</translation>
    </message>
    <message>
        <source>PROTECTED - running host scope</source>
        <translation>OCHRONA - uruchomiony zakres hosta</translation>
    </message>
    <message>
        <source>Unlock required before selection</source>
        <translation>Odblokuj wymagane przed wyborem</translation>
    </message>
    <message>
        <source>Eligible repair candidate</source>
        <translation>Kwalifikujący się kandydat do naprawy</translation>
    </message>
    <message>
        <source>Drive:</source>
        <translation>Napęd:</translation>
    </message>
    <message>
        <source>Detected target:</source>
        <translation>Wykryty cel:</translation>
    </message>
    <message>
        <source>Pending inspection</source>
        <translation>Do czasu inspekcji</translation>
    </message>
    <message>
        <source>Model / label:</source>
        <translation>Wzór / etykieta:</translation>
    </message>
    <message>
        <source>Status:</source>
        <translation>Status:</translation>
    </message>
    <message>
        <source>Size:</source>
        <translation>Rozmiar:</translation>
    </message>
    <message>
        <source>Connection:</source>
        <translation>Połączenie:</translation>
    </message>
    <message>
        <source>Filesystem:</source>
        <translation>System plików:</translation>
    </message>
    <message>
        <source>UUID:</source>
        <translation>UUID:</translation>
    </message>
    <message>
        <source>Mounts:</source>
        <translation>Góry:</translation>
    </message>
    <message>
        <source>Running system protection unresolved</source>
        <translation>Ochrona systemu uruchomionego nierozwiązana</translation>
    </message>
    <message>
        <source>No protected physical backing disk was identified</source>
        <translation>Nie zidentyfikowano chronionego fizycznego dysku zapasowego</translation>
    </message>
    <message>
        <source>Current running Linux system</source>
        <translation>Bieżący system Linux</translation>
    </message>
    <message>
        <source>Critical mounts: %1</source>
        <translation>Kolumny krytyczne: %1</translation>
    </message>
    <message>
        <source>Read-only protected-running-system facts: the helper OS fact plus the inventory model, device path, size, transport and critical mounts. Nothing here is probed destructively.</source>
        <translation>Read- only protected-running-system facts: the helper OS fact plus the invent model, device path, size, transport and critical mounts. Nic tu nie jest niszczące.</translation>
    </message>
    <message>
        <source>Select a drive to see unlock status.</source>
        <translation>Wybierz dysk, aby zobaczyć status odblokowania.</translation>
    </message>
    <message>
        <source>State: protected
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
The protected running host cannot be unlocked or modified; unlock is available for an offline repair target only. Use Host Maintenance for the protected running host.</source>
        <translation>Państwo: chronione
Składnik: %1
Mapper: (brak)
Metoda: helper odblokować (cryptsetup; passphrase poprzez mode- 600 keyfile, usunięte po użyciu)
Chroniony host nie może być odblokowany ani zmodyfikowany; odblokowanie jest dostępne tylko dla celów naprawy offline. Użyj Maintenance hosta dla chronionego hosta.</translation>
    </message>
    <message>
        <source>(none detected)</source>
        <translation>(nie wykryto)</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
A locked LUKS container is visible on this drive; press Unlock to open it for this recovery session.</source>
        <translation>Państwo: zablokowane
Składnik: %1
Mapper: (brak)
Metoda: helper odblokować (cryptsetup; passphrase poprzez mode- 600 keyfile, usunięte po użyciu)
Zablokowany pojemnik LUKS jest widoczny na tym dysku; naciśnij przycisk Odblokuj, aby otworzyć go do tej sesji odzyskiwania.</translation>
    </message>
    <message>
        <source>(visible mapper)</source>
        <translation>(widoczny mapper)</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: already open before this session (visible mapper; Boot Bitch will reuse it and will not close it).</source>
        <translation>Państwo: otwarte
Składnik: %1
Mapper: %2
Metoda: już otwarta przed tą sesją (widoczny mapper; Boot Bitch ponownie go użyje i nie zamknie).</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper-confirmed mapping from the last read-only diagnostics.</source>
        <translation>Państwo: otwarte
Składnik: %1
Mapper: %2
Metoda: mapowanie potwierdzone pomocą z ostatniej diagnostyki tylko do odczytu.</translation>
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
        <translation>Stan: zablokowany lub nie wykryto zaszyfrowanego elementu
Składnik: (niewykryty)
Mapper: (brak)
Metoda: helper odblokować (cryptsetup; passphrase poprzez mode- 600 keyfile, usunięte po użyciu)
W trakcie bieżącej sesji nie zarejestrowano zablokowanego komponentu LUKS ani operacji odblokowania tego dysku.</translation>
    </message>
    <message>
        <source>unavailable - %1</source>
        <translation>niedostępny - %1</translation>
    </message>
    <message>
        <source>Host maintenance:
%1</source>
        <translation>Obsługa techniczna:
%1</translation>
    </message>
    <message>
        <source>Target:
%1 + %2</source>
        <translation>Cel:
%1 + %2</translation>
    </message>
    <message>
        <source>Authorization required: diagnostics and repairs fail closed until you press Authorize.</source>
        <translation>Wymagana autoryzacja: diagnostyka i naprawy zakończone do momentu naciśnięcia Autoryzacji.</translation>
    </message>
    <message>
        <source>Re-establish the privileged helper session for the current scope now. The password is requested in the hidden-input modal and is never logged.</source>
        <translation>Ponownie ustanowić uprzywilejowaną sesję pomocniczą dla obecnego zakresu. Hasło jest wymagane w modale hidden- input i nigdy nie jest zalogowane.</translation>
    </message>
    <message>
        <source>Running...</source>
        <translation>Bieganie...</translation>
    </message>
    <message>
        <source>Scope required</source>
        <translation>Wymagany zakres</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope; this unlocks the gated actions.</source>
        <translation>Uruchom wszystkie - uruchom każdą dostępną diagnostykę tylko do odczytu dla bieżącego zakresu; to odblokuje działania z bramką.</translation>
    </message>
    <message>
        <source>Select a target and wait for any running command first.</source>
        <translation>Wybierz cel i najpierw czekaj na dowolne polecenie.</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic through the helper.</source>
        <translation>Uruchom Diagnostic - uruchom wybraną diagnostykę tylko przez pomocnika.</translation>
    </message>
    <message>
        <source>Run All for the current scope and refresh the read-only backend profile.</source>
        <translation>Uruchom wszystko dla bieżącego zakresu i odśwież profil tylko do odczytu.</translation>
    </message>
    <message>
        <source>Read or edit the selected target configuration file through the guarded helper; a saved edit invalidates cached diagnostics.</source>
        <translation>Odczytywanie lub edytowanie wybranego pliku konfiguracyjnego przez strzeżonego pomocnika; zapisywana edycja unieważnia diagnostykę buforową.</translation>
    </message>
    <message>
        <source>Host Maintenance has no target-file editing; commit an offline repair target first.</source>
        <translation>Host Maintenance nie ma edycji pliku celowego-; w pierwszej kolejności commit an offline repair target.</translation>
    </message>
    <message>
        <source>Commit an offline repair target first.</source>
        <translation>Najpierw zleć naprawę offline.</translation>
    </message>
    <message>
        <source>No target configuration file is available for this target; run diagnostics to probe the list.</source>
        <translation>Dla tego celu nie jest dostępny plik konfiguracyjny docelowy; uruchom diagnostykę, aby zbadać listę.</translation>
    </message>
    <message>
        <source>Already Unlocked</source>
        <translation>Już odblokowane</translation>
    </message>
    <message>
        <source>An unlocked Linux filesystem is already visible on this drive. Boot Bitch will reuse the existing mapper and will not close or reopen a mapping created by this recovery session. Mounting happens during diagnostics (read-only) and repairs (read-write); data filesystems are never auto-mounted on selection.</source>
        <translation>Odblokowany system plików Linux jest już widoczny na tym dysku. Boot Bitch ponownie użyje istniejącego mappera i nie zamknie ani nie otworzy mapowania stworzonego przez tę sesję odzyskiwania. Montaż odbywa się podczas diagnostyki (tylko do odczytu) i napraw (read- write); systemy plików danych nigdy nie są automatycznie montowane przy wyborze.</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked; use Host Maintenance for the protected running host.</source>
        <translation>Chroniony host running nie może być odblokowany; użyj Host Maintenance dla chronionego hosta running.</translation>
    </message>
    <message>
        <source>Host Maintenance is the current scope, but the selected offline drive can still be unlocked.</source>
        <translation>Host Maintenance jest aktualnym zakresem, ale wybrany dysk offline może być nadal odblokowany.</translation>
    </message>
    <message>
        <source>Unlock %1 using cryptsetup through the privileged helper. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>Odblokuj %1 używając cryptsetup poprzez uprzywilejowanego pomocnika. Passsphrase podróżuje przez prywatny plik keyfile i nigdy nie jest umieszczony w argumentach poleceń lub logach.</translation>
    </message>
    <message>
        <source>The protected running system cannot be selected as a repair target; use Host Maintenance for the protected running host.</source>
        <translation>Chroniony system nie może być wybrany jako cel naprawczy; użyj Maintenance Host dla chronionego hosta.</translation>
    </message>
    <message>
        <source>Live / installer media is read-only boot media and cannot be selected as a repair target.</source>
        <translation>Media live / installer są tylko nośnikami startowymi i nie mogą być wybrane jako cel naprawczy.</translation>
    </message>
    <message>
        <source>Unlock the encrypted volume first; Select Target becomes available after a Linux filesystem is detected.</source>
        <translation>Najpierw odblokuj zaszyfrowany wolumin; Select Target staje się dostępny po wykryciu systemu plików Linux.</translation>
    </message>
    <message>
        <source>Committed repair target. Repair, Diagnostics and File Copy target this physical drive until another drive is explicitly selected with Select Target.</source>
        <translation>Cel naprawczy. Naprawa, Diagnostyka i Kopiowanie plików mają na celu ten dysk fizyczny, aż inny dysk zostanie wyraźnie wybrany za pomocą Select Target.</translation>
    </message>
    <message>
        <source>Commit %1 as the repair target; this leaves Host Maintenance and switches the scope to the selected drive.</source>
        <translation>Zgłoś %1 jako cel naprawy; to pozostawia Host Maintenance i przełącza zakres do wybranego napędu.</translation>
    </message>
    <message>
        <source>The running host target could not be detected.</source>
        <translation>Bieżący obiekt nie mógł zostać wykryty.</translation>
    </message>
    <message>
        <source>Leave Host Maintenance and return to repair-target mode.</source>
        <translation>Opuść Konserwację Hosta i wróć do trybu naprowadzania.</translation>
    </message>
    <message>
        <source>Select the running host for deliberate guarded maintenance; the administrator authorization is requested here once and cached for the session.</source>
        <translation>Wybierz obsługujący host dla celowej konserwacji strzeżonej; autoryzacja administratora jest wymagana tutaj raz i buforowane na sesję.</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a repair target.</source>
        <translation>Bieżący goniec nie mógł zostać wykryty; diagnostyka potrzebuje celu naprawczego.</translation>
    </message>
    <message>
        <source>Committed target: %1 + %2</source>
        <translation>Cel: %1 + %2</translation>
    </message>
    <message>
        <source>Committed target: none (selection changed)</source>
        <translation>Zaangażowany cel: brak (zmiana wyboru)</translation>
    </message>
    <message>
        <source>The selected scope has no resolved Linux root component; Refresh Devices and commit the repair target again.</source>
        <translation>Wybrany zakres nie ma rozwiązanego komponentu głównego Linuksa; Odśwież urządzenia i ponownie wyłącz cel naprawy.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize on the Systems or Repair tab to re-establish it. Run All reuses the cached authorization and never prompts by itself.</source>
        <translation>Sesja administratora nie jest aktywna; naciśnij Autoryzacja na karcie Systemy lub Napraw, aby ją ponownie ustanowić. Uruchom wszystkie ponownie używa autoryzacji cached i nigdy nie podpowiada sam.</translation>
    </message>
    <message>
        <source>Run diagnostics for this scope to unlock the gated actions. Diagnostics are read-only and the only evidence source.</source>
        <translation>Uruchom diagnostykę dla tego zakresu, aby odblokować strzeżone działania. Diagnostyka jest tylko do odczytu i jedynym źródłem dowodów.</translation>
    </message>
    <message>
        <source>A repair was not proven unchanged, so the cached diagnostics are invalidated. Run diagnostics again before another gated action.</source>
        <translation>Naprawa nie została udowodniona bez zmian, więc diagnostyka buforowa jest unieważniona. Przeprowadź diagnostykę jeszcze raz przed kolejną akcji.</translation>
    </message>
    <message>
        <source>Gated actions reflect the cached capability lines; the helper still runs every runtime preflight when a command starts.</source>
        <translation>Akcje przyporządkowane odzwierciedlają linie funkcji buforowanych; pomocnik nadal działa w każdym locie przed startem podczas startu komendy.</translation>
    </message>
    <message>
        <source>Regenerating diagnostics automatically</source>
        <translation>Automatyczna regeneracja diagnostyki</translation>
    </message>
    <message>
        <source>Running diagnostic: %1</source>
        <translation>Diagnostyka bieżąca: %1</translation>
    </message>
    <message>
        <source>Running all diagnostics</source>
        <translation>Wszystkie diagnostyki</translation>
    </message>
    <message>
        <source>Unlocking %1</source>
        <translation>Odblokowywanie %1</translation>
    </message>
    <message>
        <source>running host</source>
        <translation>uruchomiony host</translation>
    </message>
    <message>
        <source>offline target</source>
        <translation>cel offline</translation>
    </message>
    <message>
        <source>host maintenance active</source>
        <translation>obsługa techniczna hosta</translation>
    </message>
    <message>
        <source>repair target committed</source>
        <translation>cel naprawczy</translation>
    </message>
    <message>
        <source>no committed scope</source>
        <translation>brak przydzielonego zakresu</translation>
    </message>
    <message>
        <source>Current session</source>
        <translation>Bieżąca sesja</translation>
    </message>
    <message>
        <source>Log files (*.log);;All files (*)</source>
        <translation>Pliki dziennika (* .log);; Wszystkie pliki (*)</translation>
    </message>
    <message>
        <source>Save log as</source>
        <translation>Zapisz dziennik jako</translation>
    </message>
    <message>
        <source>About Boot Bitch</source>
        <translation>O Boot Bitch</translation>
    </message>
    <message>
        <source>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;&lt;p&gt;&lt;b&gt;Developer:&lt;/b&gt; CaptainMorgan12&lt;/p&gt;&lt;p&gt;This GUI is the package&apos;s entry point: a Qt 3.3.x frontend with the ported guarded helper for Debian Etch-era systems.&lt;/p&gt;&lt;p&gt;&lt;b&gt;Guarded repair mode:&lt;/b&gt; read-only diagnostics can inspect either the protected Running Host or an explicitly selected repair drive. The guarded Debian/APT package stages (interrupted configuration, broken dependencies, metadata refresh, upgrade), GRUB-legacy configuration regeneration, LUKS target unlock and guarded target-file editing run through the ported helper after confirmation; Host Maintenance enables the same supported stages natively on the active system after repeating the host identity and boot-mount checks.&lt;/p&gt;&lt;p&gt;The modern-only features - verified File Copy, Btrfs snapshot rollback, EFI/UKI and extlinux repair, boot-stack reconciliation and Make Default - are greyed with the helper&apos;s own probe reasons on this frontend; Arch/Alpine/Fedora package backends stay diagnostics-only here.&lt;/p&gt;&lt;p&gt;The first privileged action authorizes one cached administrator session per scope through a hidden-input modal (the Qt GUI remains unprivileged). It can be ended at any time from File - Lock Administrator Session.&lt;/p&gt;</source>
        <translation>&lt; h3 &gt; Boot Bitch %1 &lt; / h3 &gt; &lt; p &gt; &lt; b &gt; Programista: &lt; / b &gt; CaptainMorgan12 &lt; / p &gt; &lt; p &gt; Ten interfejs graficzny jest punktem wejścia pakietu: Qt 3.3.x frontend z przeniesionym, strzeżonym pomocnikiem dla systemów Etch- era Debiana. &lt; / p &gt; &lt; p &gt; &lt; b &gt; Strzeżony tryb naprawy: &lt; / b &gt; Diagnostyka tylko do odczytu może przeprowadzić kontrolę albo chronionego komputera, albo wyraźnie wybranego napędu naprawczego. Strzeżone etapy pakietu Debian / APT (przerwana konfiguracja, zepsute zależności, odświeżanie metadanych, upgrade), dotychczasowa regeneracja konfiguracji GRUB-, docelowa edycja plików LUKS odblokowuje i strzeżona edycja plików celowniczych po potwierdzeniu; Host Maintenance umożliwia te same obsługiwane etapy w systemie aktywnym po powtórzeniu tożsamości hosta i kontroli montowania boot-. &lt; / p &gt; &lt; p &gt; Funkcje modernizowane - zweryfikowane kopiowanie plików, zwroty migawek Btrfs, naprawy EFI / UKI i extlinux, pojednanie boot- stosu i Make Domyślny - są szarości z własnych powodów sondy pomocnika na tym froncie; Arch / Alpine / Fedora backends pozostać diagnostyką tylko tutaj. &lt; / p &gt; Pierwsze uprzywilejowane działanie zezwala na jedną sesję administratora buforowanego za pomocą środka hidden- input (interfejs Qt pozostaje nieuprzywilejowany). Można to zakończyć w dowolnym momencie z pliku - blokada sesji administratora. &lt; / p &gt;</translation>
    </message>
</context>
</TS>
