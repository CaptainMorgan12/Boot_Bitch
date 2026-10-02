<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS language="tr">
<context>
    <name>LegacyMainWindow</name>
    <message>
        <source>Validate environment</source>
        <translation>Geçerli ortam</translation>
    </message>
    <message>
        <source>Validate</source>
        <translation>Geçerlilik</translation>
    </message>
    <message>
        <source>Check the selected system&apos;s mounts, filesystem metadata, boot files, mapper consistency and dependency readiness before any repair action. This is an independent safety preflight rather than an optional Full Repair stage.</source>
        <translation>Seçilen sistemin dağlarını kontrol edin, dosyasystem metadata, boot dosyaları, haritaper tutarlılık ve herhangi bir onarım eyleminden önce hazırlık. Bu, isteğe bağlı Full Onarım aşamasından ziyade bağımsız bir güvenlik ışığıdır.</translation>
    </message>
    <message>
        <source>Always preflight</source>
        <translation>Her zaman Preflight</translation>
    </message>
    <message>
        <source>File system repair</source>
        <translation>Dosya sistemi onarım sistemi</translation>
    </message>
    <message>
        <source>Check File Systems</source>
        <translation>Check File Systems</translation>
    </message>
    <message>
        <source>Run the read-only file system check for the selected system&apos;s root and /boot filesystems and report each device&apos;s check tool and result without changing anything. This legacy frontend exposes the read-only check only; device repair is not wired.</source>
        <translation>Seçilen sistemin kök ve /boot dosya sistemleri için sadece dosya sistemini kontrol edin ve her cihazın kontrol aracını raporlayın ve hiçbir şeyi değiştirmeden sonuç alın. Bu miras cephesi sadece okumayı ortaya çıkarır; cihaz onarımı tel değildir.</translation>
    </message>
    <message>
        <source>Full Repair plan: unavailable on this frontend - read-only check only</source>
        <translation>Full Tamir planı: Bu ön uçta mevcut değil - sadece sadece sadece kontrol</translation>
    </message>
    <message>
        <source>Complete package configuration</source>
        <translation>Tamam paketi yapılandırma</translation>
    </message>
    <message>
        <source>Complete Configuration</source>
        <translation>Tamamlayıcı</translation>
    </message>
    <message>
        <source>Complete interrupted dpkg package configuration in the selected repair system. This is the same stage controlled by Settings -&gt; Full Repair plan -&gt; Complete interrupted package configuration, but it can also be run independently here.</source>
        <translation>Seçilen onarım sisteminde dpkg paketi yapılandırmasını tamamlayın. Bu, Ayarlar tarafından kontrol edilen aynı aşamadır -&gt; Full Tamir planı -&gt; Tamamlanmış paket yapılandırması, ancak burada bağımsız olarak da çalıştırılabilir.</translation>
    </message>
    <message>
        <source>Run the guarded dpkg-configure repair? The helper keeps its package-lock and runtime preflights.</source>
        <translation>Korumalı dpkg-konfigure onarımı çalıştırın mı? Yardımcı paketi tutar ve zaman ön ışıklarını çalıştırır.</translation>
    </message>
    <message>
        <source>Repair broken dependencies</source>
        <translation>Tamir kırık bağımlılıkları</translation>
    </message>
    <message>
        <source>Repair Dependencies</source>
        <translation>Onarım Bağımlılığı</translation>
    </message>
    <message>
        <source>Repair package dependencies in the selected repair system after the mandatory safety preflight. This maps directly to Settings -&gt; Repair broken package dependencies.</source>
        <translation>Onarım paketi, zorunlu güvenlik ön ışığından sonra seçilmiş onarım sisteminde bağımlılıklara bağlıdır. Bu haritalar doğrudan Ayarlar -&gt; Tamir kırık paketi bağımlılıklara bağlıdır.</translation>
    </message>
    <message>
        <source>Run the guarded fix-broken repair? The helper keeps its simulation-first preflight and runtime guards.</source>
        <translation>Korumalı onarımı çalıştırın mı? Yardımcı, simülasyonu ilk ön ışığı ve runtime gardiyanlarını tutar.</translation>
    </message>
    <message>
        <source>Refresh package metadata</source>
        <translation>Yeni paket metadata</translation>
    </message>
    <message>
        <source>Refresh Metadata</source>
        <translation>Metadata</translation>
    </message>
    <message>
        <source>Refresh APT metadata in the selected repair system without upgrading installed packages. This maps directly to Settings -&gt; Refresh package metadata.</source>
        <translation>Mevcut paketleri yükseltmeden seçilmiş onarım sisteminde APT metadata&apos;yı yenileyin. Bu haritalar doğrudan Ayarlar -&gt; Yeni paket metadata.</translation>
    </message>
    <message>
        <source>Refresh package metadata for the selected scope? The helper requires a reachable, trusted APT source.</source>
        <translation>Seçilmiş kapsamı için yeni paket metadata mı? Yardımcı, erişilebilir, güvenilir APT kaynağı gerektirir.</translation>
    </message>
    <message>
        <source>Upgrade installed packages</source>
        <translation>Yükseltilmiş paket paketleri</translation>
    </message>
    <message>
        <source>Simulate and Upgrade</source>
        <translation>Simulate ve Yükseltme</translation>
    </message>
    <message>
        <source>Simulate the APT transaction first, inspect proposed removals, then apply a safe upgrade. This maps directly to Settings -&gt; Upgrade installed packages.</source>
        <translation>APT işlemi ilk olarak, denetim önerilen geri yüklemeleri uygular, sonra güvenli bir yükseltme uygular. Bu haritalar doğrudan Ayarlar -&gt; Yükleme paketi.</translation>
    </message>
    <message>
        <source>Run the guarded apt-upgrade transaction? The helper keeps its simulation-first and source guards.</source>
        <translation>Korumalı apt-upgrad işlemi çalıştırın mı? Yardımcı, simülasyonunu ilk ve kaynak koruyucularını tutar.</translation>
    </message>
    <message>
        <source>DKMS</source>
        <translation>DKMS</translation>
    </message>
    <message>
        <source>Rebuild DKMS</source>
        <translation>Rebuild DKMS</translation>
    </message>
    <message>
        <source>Rebuild out-of-tree kernel modules for kernels installed in the selected system. The helper refuses this action when DKMS is not installed; this legacy frontend exposes no DKMS action.</source>
        <translation>Seçilmiş sistemde yüklenen çekirdekler için kutu çekirdek modüllerini yeniden inşa edin. Yardımcı DKMS kurulduğunda bu eylemi reddeder; bu miras cephesi DKMS eylemini açığa çıkarır.</translation>
    </message>
    <message>
        <source>Graphical login / display manager</source>
        <translation>Grafiksel giriş / ekran yöneticisi</translation>
    </message>
    <message>
        <source>Restore Graphical Login</source>
        <translation>Grafiksel Girişi</translation>
    </message>
    <message>
        <source>Restore the legacy SysV display manager configured for the running host: the /etc/X11/default-display-manager entry and the missing runlevel S-symlink, with a backup and rollback, never starting the GUI. This is a host-scope stage on this legacy frontend.</source>
        <translation>Çalışan ev sahibi için yapılandırılan miras SysV ekran yöneticisini geri yükleyin: / v/X11/default-display-manager girişi ve eksik run level S-symlink, bir yedekleme ve rollback ile hiçbir zaman GUI&apos;ye başlama. Bu, bu miras cephesinde ev sahibi bir sahnedir.</translation>
    </message>
    <message>
        <source>Restore the graphical login configuration for the running host? The helper backs up /etc/X11/default-display-manager and the runlevel symlink state, restores the configured entry and the missing S-symlink, rolls back on any failure, and never starts the display manager.</source>
        <translation>Çalışan ev sahibi için grafik giriş konfigürasyonunu geri yükleyin mi? helper geri döndü / etc/X11/default-display-manager ve run level symlink devleti, yapılandırılan girişi geri yükleyin ve eksik S-symlink, herhangi bir başarısızlıkta geri döner ve asla ekran yöneticisine başlamaz.</translation>
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
        <translation>Seçilen onarım sistemi için initramfs görüntüleri sadece haritadan sonra ve şifre tutarlı kontroller geçer. Yardımcı her görüntüyü uygulamadan önce geri döndürür. Etch&apos;da sahne, bekçili düz-köklü düşüşle çalışır (gösterilmedi).</translation>
    </message>
    <message>
        <source>Rebuild the initramfs for the selected scope? The helper keeps its mapper/crypttab and backup preflights.</source>
        <translation>initramfs&apos;i seçilen kapsamı yeniden inşa etmek? Yardımcı haritasını/crypttabını ve yedekleme ön ışıklarını tutar.</translation>
    </message>
    <message>
        <source>EFI / UKI bootloader</source>
        <translation>EFI / UKI bootloader</translation>
    </message>
    <message>
        <source>Repair EFI / UKI</source>
        <translation>EFI / UKI</translation>
    </message>
    <message>
        <source>Repair the selected system&apos;s EFI / UKI boot path. This legacy frontend exposes no EFI action; the Etch target is a BIOS/GRUB-legacy system.</source>
        <translation>Seçilen sistemin EFI / UKI boot yolunu tamir edin. Bu miras cephesi hiçbir EFI eylemi ortaya çıkar; Etch hedefi BIOS/GRUB-legacy sistemidir.</translation>
    </message>
    <message>
        <source>GRUB configuration</source>
        <translation>GRUB yapılandırma</translation>
    </message>
    <message>
        <source>Regenerate GRUB</source>
        <translation>Regenerate GRUB</translation>
    </message>
    <message>
        <source>Regenerate the selected repair system&apos;s GRUB menu/configuration after the mandatory safety preflight. The helper backs up menu.lst, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Seçilmiş onarım sistemi GRUB menüsü / konfigürasyonu zorunlu güvenlik ön ışığından sonra yeniden yapılandırın. Yardımcı menüyü geri döndürür, mevcut her giriş girişi korur ve herhangi bir başarısızlıkta geri döner.</translation>
    </message>
    <message>
        <source>Regenerate the GRUB configuration? The helper backs up the target menu/configuration, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>GRUB konfigürasyonunu Yeniden Üretin mi? Yardımcı, hedef menü / yapılandırmayı geri döndürür, mevcut tüm önyükleme girişini korur ve herhangi bir başarısızlıkta geri döner.</translation>
    </message>
    <message>
        <source>extlinux configuration</source>
        <translation>extlinux yapılandırma</translation>
    </message>
    <message>
        <source>Regenerate extlinux</source>
        <translation>Regenerate extlinux</translation>
    </message>
    <message>
        <source>Regenerate the selected system&apos;s extlinux bootloader configuration. This legacy frontend exposes no extlinux action.</source>
        <translation>Seçilen sistemin extlinux bootloader konfigürasyonunu yeniden tanımlamak. Bu miras cephesi hiçbir extlinux eylemi açığa çıkarmıyor.</translation>
    </message>
    <message>
        <source>Boot stack reconciliation</source>
        <translation>Bot uzlaşması</translation>
    </message>
    <message>
        <source>Reconcile Boot Stack</source>
        <translation>Reconcile Boot Stack</translation>
    </message>
    <message>
        <source>Reconcile the selected repair system&apos;s boot stack in one guarded legacy pass: mapper/crypttab validation, initramfs rebuild and GRUB-legacy configuration regeneration, with the component backups and preflights unchanged. This is the Etch equivalent of the modern boot-stack reconciliation and stays out of the Full Repair plan.</source>
        <translation>Seçilmiş onarım sisteminin önyükleme bandı tek bir bekçili miras geçişte: haritaper/crypttab geçerlilik, initramfs yeniden inşa ve GRUB-legacy configuration Yenileme, bileşen yedekleri ve ön ışıkları olmadan. Bu, modern boot-stack uzlaşmasının Etch eşdeğerdir ve Full Onarım planının dışında kalır.</translation>
    </message>
    <message>
        <source>Full Repair plan: Manual recovery tool</source>
        <translation>Full Tamir planı: Manual kurtarma aracı</translation>
    </message>
    <message>
        <source>Run the guarded boot-stack reconciliation? The helper runs the mapper/crypttab validation, the initramfs rebuild and the GRUB-legacy regeneration in one pass with every component preflight and backup.</source>
        <translation>Korumalı boot-stack uzlaşmasını çalıştırın mı? helper haritaper / şifre doğrulamayı çalışır, initramfs yeniden inşa eder ve GRUB-legacy Yeniliği her bileşen ön ve yedekleme ile geçer.</translation>
    </message>
    <message>
        <source>Complete interrupted package configuration</source>
        <translation>Tamamlanmış paketi yapılandırma</translation>
    </message>
    <message>
        <source>Repair broken package dependencies</source>
        <translation>Tamir kırık paketi bağımlılıklara bağlı</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Varsayılan olun</translation>
    </message>
    <message>
        <source>Repair file system errors (read-only check first)</source>
        <translation>Tamir dosya sistemi hataları (yalnızca ilk önce kontrol edin)</translation>
    </message>
    <message>
        <source>the legacy frontend exposes the read-only file system check only; per-device repair is not wired on this frontend (fail closed)</source>
        <translation>Geleneksel ön uç sadece dosya sistemini kontrol eder; per-device onarımı bu ön uçta tel değildir (fail kapalı)</translation>
    </message>
    <message>
        <source>Upgrade installed packages (adaptive APT simulation)</source>
        <translation>Yükseltilmiş paketler (a Adapt APT Simülasyonu)</translation>
    </message>
    <message>
        <source>Rebuild DKMS modules</source>
        <translation>Rebuild DKMS modülleri</translation>
    </message>
    <message>
        <source>Restore graphical login manager</source>
        <translation>Grafiksel giriş yöneticisi geri yükleme</translation>
    </message>
    <message>
        <source>Rebuild initramfs after mapper/crypttab validation</source>
        <translation>Rebuild initramfs haritaper /crypttab doğrulamadan sonra</translation>
    </message>
    <message>
        <source>Repair EFI / UKI boot path</source>
        <translation>EFI / UKI boot yolu</translation>
    </message>
    <message>
        <source>Update GRUB configuration</source>
        <translation>Güncelleme GRUB yapılandırma</translation>
    </message>
    <message>
        <source>Update extlinux configuration</source>
        <translation>Güncelleme extlinux yapılandırma</translation>
    </message>
    <message>
        <source>Choose Full Repair stages in Settings. Enabled stages run in the order shown. Each configurable stage also appears below as an individual tool; the Full Repair column mirrors its current Settings state. The boot tools (EFI / UKI bootloader, GRUB or extlinux configuration, boot-stack reconciliation and Make Default) are independent: run them in any order, and a later action re-verifies what an earlier one changed and reports its own result. The active scope is shown beside Repair: selected repair drive or Running Host maintenance.</source>
        <translation>Ayarlarda Full Tamir aşamalarını seçin. Enabled aşamalar gösterilen sırayla çalışır. Her yapılandırılabilir aşama aynı zamanda bireysel bir araç olarak da görünür; Full Tamir sütunu aynaları mevcut ayarlar durumu. Önyükleme araçları (EFI / UKI bootloader, GRUB veya extlinux konfigürasyonu, boot-stack uzlaşması ve Temsil Edicisi) bağımsızdır: herhangi bir sırayla onları çalıştırın ve daha sonra bir eylem daha öncekinin ne değiştirdiğini ve kendi sonuçlarını rapor eder. Aktif kapsamı Onarımın yanında gösterilir: Seçilmiş onarım sürüşü veya Host bakım.</translation>
    </message>
    <message>
        <source>Run All diagnostics for the selected target or running host before starting Full Repair. The report is read-only evidence used to choose and confirm repair stages.</source>
        <translation>Seçilen hedef için tüm tanıları çalıştırın veya Full Repair başlamadan önce ev sahibi koşma. Rapor, onarım aşamalarını seçmek ve doğrulamak için kullanılan yalnızca kanıtlardır.</translation>
    </message>
    <message>
        <source>Ready: required cached read-only diagnostics are available for the selected stages. Review them in Diagnostics or Logs before confirming.</source>
        <translation>Hazır: gerekli önbellekli okuma-sadece tanıları seçilmiş aşamalar için mevcuttur. Onları doğrulamadan önce Teşhis veya Logs&apos;te gözden geçirin.</translation>
    </message>
    <message>
        <source>Environment validation</source>
        <translation>Çevre Geçerliliği</translation>
    </message>
    <message>
        <source>Summarizes the selected system, protection state, mounted identity and inspection readiness.</source>
        <translation>Seçilen sistem, koruma devleti, monte edilmiş kimlik ve denetim hazırlığı.</translation>
    </message>
    <message>
        <source>Distribution and boot backend profile</source>
        <translation>Dağıtım ve önyükleme profili</translation>
    </message>
    <message>
        <source>Identifies the distribution family, package manager, initramfs generator, bootloader and current guarded repair capability.</source>
        <translation>Dağıtım ailesini, paket yöneticisini, initramfs jeneratörü, bootloader ve mevcut muhafız onarım kapasitesini belirtir.</translation>
    </message>
    <message>
        <source>Boot diagnostics</source>
        <translation>Parmak teşhisleri</translation>
    </message>
    <message>
        <source>Shows boot mounts and /boot contents plus storage evidence without changing the selected system.</source>
        <translation>Önyükleme hatları ve /boot içeriği, seçilen sistemi değiştirmeden artı depolama kanıtlarını gösterir.</translation>
    </message>
    <message>
        <source>Boot evidence and selection history</source>
        <translation>Boot kanıtları ve seçimi tarihi</translation>
    </message>
    <message>
        <source>Correlates the detected boot chain, bootloader selection, kernel/initramfs and unlock evidence.</source>
        <translation>tespit edilen boot zincirini, bootloader seçimi, çekirdek/initramfs&apos;i ilişkilendirir ve kanıtları açar.</translation>
    </message>
    <message>
        <source>Kernel / initramfs</source>
        <translation>Anahtar / initramfs</translation>
    </message>
    <message>
        <source>Reviews kernel files and verifies matching initramfs images through a read-only inspection.</source>
        <translation>Yorumlar çekirdeği dosyaları ve sadece bir inceleme aracılığıyla initramfs görüntülerini eşleştirin.</translation>
    </message>
    <message>
        <source>Reviews the GRUB configuration without changing boot files.</source>
        <translation>GRUB konfigürasyonunu boot dosyalarını değiştirmeden değerlendirin.</translation>
    </message>
    <message>
        <source>EFI / UKI boot state</source>
        <translation>EFI / UKI boot state</translation>
    </message>
    <message>
        <source>Inspects EFI/UKI evidence; unavailable on this legacy BIOS frontend with the helper&apos;s probe reason.</source>
        <translation>Inspects EFI/UKI kanıtları; bu mirası BIOS cephesinde yardımcının son nedeni ile mevcut değildir.</translation>
    </message>
    <message>
        <source>Reviews the configured display manager and recent boot evidence without starting the GUI.</source>
        <translation>Güvenlik başlamadan yapısal ekran yöneticisi ve son boot kanıtlarını yorumlar.</translation>
    </message>
    <message>
        <source>Boot errors</source>
        <translation>Parmak hataları hataları</translation>
    </message>
    <message>
        <source>Reads recent error-priority entries from the running host or selected repair system when available.</source>
        <translation>Mevcut olduğunda çalışan ev sahibi veya seçilmiş onarım sisteminden son hata önceliklerini okuyun.</translation>
    </message>
    <message>
        <source>Disk usage</source>
        <translation>Disk kullanımı</translation>
    </message>
    <message>
        <source>Summarizes filesystem capacity and free space for the running host or read-only repair target.</source>
        <translation>Çalışan ev sahibi veya okuma-sadece onarım hedefi için dosya sistemi kapasitesi ve ücretsiz alan.</translation>
    </message>
    <message>
        <source>File systems</source>
        <translation>Dosya sistemleri</translation>
    </message>
    <message>
        <source>Runs the read-only file system check for the selected system&apos;s root, /boot and other filesystems.</source>
        <translation>Seçilen sistemin kök, /boot ve diğer dosya sistemleri için okuma-sadece dosya sistemini kontrol edin.</translation>
    </message>
    <message>
        <source>/etc/fstab review</source>
        <translation>/ etc/fstab inceleme</translation>
    </message>
    <message>
        <source>Displays the running host or selected repair system&apos;s fstab; repair-system inspection is mounted read-only.</source>
        <translation>Çalışan ev sahibi veya seçilmiş onarım sistemi fstabını gösterir; onarım sistemi denetimi yalnızca okunur.</translation>
    </message>
    <message>
        <source>Btrfs status</source>
        <translation>Btrfs durumu</translation>
    </message>
    <message>
        <source>Shows Btrfs filesystem and subvolume information when the target uses Btrfs.</source>
        <translation>Btrfs dosya sistemi ve altvolume bilgilerini hedef Btrfs kullanıyor.</translation>
    </message>
    <message>
        <source>Device-mapper ancestry</source>
        <translation>Device-mapper soyuy</translation>
    </message>
    <message>
        <source>Shows selected mapper ancestry and device-mapper state when available.</source>
        <translation>Gösteriler mevcut olduğunda haritalı soy ve cihaz-mapper durumunu gösterir.</translation>
    </message>
    <message>
        <source>LUKS / crypttab evidence</source>
        <translation>LUKS / şifreli kanıtlar</translation>
    </message>
    <message>
        <source>Shows LUKS/mapped ancestry plus crypttab and fstab mapper references.</source>
        <translation>Shows LUKS / sağlanmış soy artı şifrelenmiş ve fstab harita referansları.</translation>
    </message>
    <message>
        <source>Full diagnostic report</source>
        <translation>Tam tanı raporu</translation>
    </message>
    <message>
        <source>Combines all read-only diagnostics for the selected scope (same as Run All).</source>
        <translation>Seçilen alan için tüm okuma-sadece tanıları birleştirin (hepsi Run All)</translation>
    </message>
    <message>
        <source>All entries</source>
        <translation>Bütün girişler</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>Tanıklar</translation>
    </message>
    <message>
        <source>Repairs</source>
        <translation>Onarımlar</translation>
    </message>
    <message>
        <source>Package repair</source>
        <translation>Paket onarım paketi</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Dosya kopyası</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Cihaz keşfi</translation>
    </message>
    <message>
        <source>Host</source>
        <translation>Host Host Host</translation>
    </message>
    <message>
        <source>Required for block-device inventory</source>
        <translation>Blok-device envanter için gerekli</translation>
    </message>
    <message>
        <source>Filesystem identification</source>
        <translation>Filesystem tanımlama</translation>
    </message>
    <message>
        <source>Used to identify filesystem metadata</source>
        <translation>filesystem metadata tanımlamak için kullanılır</translation>
    </message>
    <message>
        <source>Mount inspection</source>
        <translation>Mount denetimi</translation>
    </message>
    <message>
        <source>Used to understand active mounts</source>
        <translation>Aktif dağları anlamak için kullanılır</translation>
    </message>
    <message>
        <source>LUKS support</source>
        <translation>LUKS desteği</translation>
    </message>
    <message>
        <source>Required to unlock encrypted targets</source>
        <translation>Şifreli hedefleri kilidini açmak için gerekli</translation>
    </message>
    <message>
        <source>Btrfs support</source>
        <translation>Btrfs desteği</translation>
    </message>
    <message>
        <source>Required for Btrfs inspection and snapshot rollback</source>
        <translation>Btrfs denetim ve anlık geri dönüş için gerekli</translation>
    </message>
    <message>
        <source>Bidirectional file copy</source>
        <translation>Biyön dosyası kopya</translation>
    </message>
    <message>
        <source>Host/Repair</source>
        <translation>Host/Remate</translation>
    </message>
    <message>
        <source>Required for verified Host-to-Repair and Repair-to-Host transfer</source>
        <translation>Doğrulanmış Host-to-Remate ve Onarım-to-Host transfer için gerekli</translation>
    </message>
    <message>
        <source>Chroot repair</source>
        <translation>Chroot onarım</translation>
    </message>
    <message>
        <source>Required for target-side repair commands</source>
        <translation>Hedef onarım komutları için gerekli</translation>
    </message>
    <message>
        <source>Offline systemd repair</source>
        <translation>Offline sistemid onarım</translation>
    </message>
    <message>
        <source>Used to restore graphical.target and the configured display manager without starting the target GUI</source>
        <translation>Grafik geri yüklemek için kullanılır. Hedef ve yapısal ekran yöneticisi hedef GUI başlamadan önce</translation>
    </message>
    <message>
        <source>UEFI NVRAM inspection</source>
        <translation>UEFI NVRAM denetim</translation>
    </message>
    <message>
        <source>Used to preserve target EFI BootOrder during TUXEDO UKI rebuilds when efivars are available</source>
        <translation>EFI BootOrder&apos;i TUXEDO UKI&apos;i efivars mevcut olduğunda yeniden inşa etmek için kullanılır</translation>
    </message>
    <message>
        <source>UKI verification</source>
        <translation>UKI doğrulama</translation>
    </message>
    <message>
        <source>Used to verify the kernel embedded in a rebuilt unified kernel image</source>
        <translation>Yeniden inşa edilmiş bir çekirdek görüntüsünde bulunan çekirdeği doğrulamak için kullanılır</translation>
    </message>
    <message>
        <source>GRUB EFI repair</source>
        <translation>GRUB EFI onarım</translation>
    </message>
    <message>
        <source>Target/Host</source>
        <translation>Hedef/Host</translation>
    </message>
    <message>
        <source>Required only for conventional GRUB-based EFI systems</source>
        <translation>Sadece geleneksel GRUB tabanlı EFI sistemleri için gerekli</translation>
    </message>
    <message>
        <source>Target</source>
        <translation>Hedef Hedef Hedef Hedef Hedef</translation>
    </message>
    <message>
        <source>Debian-family GRUB helper</source>
        <translation>Debian-aile GRUB yardımcı</translation>
    </message>
    <message>
        <source>Portable GRUB configuration generator used by Arch and other non-Debian systems</source>
        <translation>Portre GRUB yapılandırma jeneratörü Arch ve diğer non-Debian sistemleri tarafından kullanılan</translation>
    </message>
    <message>
        <source>Initramfs rebuild</source>
        <translation>Initramfs yeniden inşa ediliyor</translation>
    </message>
    <message>
        <source>Debian-family initramfs helper</source>
        <translation>Debian-aile initramfs yardımcı</translation>
    </message>
    <message>
        <source>Arch-family initramfs generator</source>
        <translation>Arch-family initramfs jeneratörü</translation>
    </message>
    <message>
        <source>Alternative initramfs generator used by Arch and other distributions</source>
        <translation>Alternatif initramfs jeneratörü Arch ve diğer dağıtımlar tarafından kullanılan</translation>
    </message>
    <message>
        <source>Initramfs verification</source>
        <translation>Initramfs doğrulama</translation>
    </message>
    <message>
        <source>Read-only verification for mkinitcpio images</source>
        <translation>mkinitcpio görüntüleri için sadece doğrulama</translation>
    </message>
    <message>
        <source>Read-only verification for dracut images</source>
        <translation>dracut görüntüleri için sadece doğrulama</translation>
    </message>
    <message>
        <source>systemd-boot inspection</source>
        <translation>systemd-boot denetim</translation>
    </message>
    <message>
        <source>Host/Target</source>
        <translation>Host/Target</translation>
    </message>
    <message>
        <source>Read-only inspection of systemd-boot and generic UKI layouts</source>
        <translation>systemd-boot ve genel UKI düzeninin sadece denetimini okuyun</translation>
    </message>
    <message>
        <source>Arch package manager</source>
        <translation>Arch paketi yöneticisi</translation>
    </message>
    <message>
        <source>Arch-family package database and transaction tool</source>
        <translation>Arch-family paketi veritabanı ve işlem aracı</translation>
    </message>
    <message>
        <source>DKMS rebuild</source>
        <translation>DKMS yeniden inşa</translation>
    </message>
    <message>
        <source>Required only when target uses DKMS modules</source>
        <translation>Hedef yalnızca DKMS modüllerini kullandığında gereklidir</translation>
    </message>
    <message>
        <source>LVM inspection</source>
        <translation>LVM denetim</translation>
    </message>
    <message>
        <source>Optional LVM storage-stack support</source>
        <translation>Seçmeli LVM depolama-stack desteği</translation>
    </message>
    <message>
        <source>Software RAID</source>
        <translation>Software RAID</translation>
    </message>
    <message>
        <source>Optional Linux MD RAID support</source>
        <translation>Seçmeli Linux MD RAID desteği</translation>
    </message>
    <message>
        <source>Process namespace isolation</source>
        <translation>Process namespace izolasyon</translation>
    </message>
    <message>
        <source>Host+Target</source>
        <translation>Host+ Hedef Hedef Hedef Hedef Hedef</translation>
    </message>
    <message>
        <source>The ported helper falls back to a guarded plain chroot when unshare is absent</source>
        <translation>Limanlı yardımcı, paylaşımın olmadığı bir gardiyana geri döner.</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Cancel Cancel Cancel Cancel</translation>
    </message>
    <message>
        <source>OK</source>
        <translation>Tamam tamam</translation>
    </message>
    <message>
        <source>Yes</source>
        <translation>Evet Evet Evet</translation>
    </message>
    <message>
        <source>No</source>
        <translation>Hayır hayır hayır</translation>
    </message>
    <message>
        <source>read-only helper diagnostic</source>
        <translation>Sadece yardımcı tanı</translation>
    </message>
</context>
<context>
    <name>legacy::LegacyMainWindow</name>
    <message>
        <source>Boot Bitch Legacy (Etch / KDE 3.5 era)</source>
        <translation>Boot Bitch Legacy (Etch / KDE 3.5 çağı)</translation>
    </message>
    <message>
        <source>Linux recovery and boot-repair utility</source>
        <translation>Linux kurtarma ve boot-remate faydalı</translation>
    </message>
    <message>
        <source>GUARDED REPAIR</source>
        <translation>GUARDED REPAIR</translation>
    </message>
    <message>
        <source>Ordinary repairs require an explicitly selected non-host target. The protected running host has a separate deliberate maintenance mode with the same guarded repair stages and requires privilege authorization.</source>
        <translation>Sıradan onarımlar, açıkça seçilmiş olmayan bir hedef gerektirir. Korumalı koşu ev sahibi aynı bekçi onarım aşamalarıyla ayrı bir kasıtlı bakım moduna sahiptir ve ayrıcalık izni gerektirir.</translation>
    </message>
    <message>
        <source>Systems</source>
        <translation>Sistemler</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>Tanıklar</translation>
    </message>
    <message>
        <source>Repair</source>
        <translation>Tamir onarımı</translation>
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
        <translation>Logs Logs</translation>
    </message>
    <message>
        <source>Settings</source>
        <translation>Ayarlar</translation>
    </message>
    <message>
        <source>(not created yet)</source>
        <translation>( henüz yaratılmadı)</translation>
    </message>
    <message>
        <source>Information</source>
        <translation>Bilgi Bilgileri</translation>
    </message>
    <message>
        <source>Close</source>
        <translation>Close Close Close Close Close</translation>
    </message>
    <message>
        <source>&amp;File</source>
        <translation>&amp;Dosya</translation>
    </message>
    <message>
        <source>&amp;Refresh Devices</source>
        <translation>&amp;Yenileme Cihazları</translation>
    </message>
    <message>
        <source>&amp;Lock Administrator Session</source>
        <translation>&amp;Lock Yönetici Sean</translation>
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
        <translation>&amp;Teşhisleri</translation>
    </message>
    <message>
        <source>&amp;Logs</source>
        <translation>&amp;Logs</translation>
    </message>
    <message>
        <source>&amp;Settings</source>
        <translation>&amp;Ayarları</translation>
    </message>
    <message>
        <source>&amp;Auto-size Device Columns</source>
        <translation>&amp;Auto-size Device Columns</translation>
    </message>
    <message>
        <source>&amp;Wrap Log Lines</source>
        <translation>&amp;</translation>
    </message>
    <message>
        <source>&amp;Help</source>
        <translation>&amp;Yardım</translation>
    </message>
    <message>
        <source>&amp;Using Boot Bitch</source>
        <translation>&amp;kullanarak Boot Bitch</translation>
    </message>
    <message>
        <source>&amp;About Boot Bitch</source>
        <translation>&amp;Hakkında</translation>
    </message>
    <message>
        <source>Device columns auto-sized. Drag headers to fine-tune widths.</source>
        <translation>Device columns auto-size. İyi genişliklere kaydırın.</translation>
    </message>
    <message>
        <source>A helper command is running; wait for it to finish before locking the session.</source>
        <translation>Bir yardımcı komut çalışır; oturumu kilitlemeden önce bitirmek için bekleyin.</translation>
    </message>
    <message>
        <source>Administrator session locked; the next privileged action will request authorization.</source>
        <translation>Yönetici oturumu kilitlendi; bir sonraki ayrıcalıklı eylem yetki talep edecek.</translation>
    </message>
    <message>
        <source>Using Boot Bitch</source>
        <translation>Boot Bitch</translation>
    </message>
    <message>
        <source>Boot Bitch must run from a different booted Linux environment than the system being repaired. Use a Linux live medium or another Linux installation on a different physical drive.&lt;br&gt;&lt;br&gt;The running host is protected from ordinary repair-target selection, but it can be explicitly selected through &lt;b&gt;Host Maintenance&lt;/b&gt; for guarded native diagnostics and supported maintenance stages.&lt;br&gt;&lt;br&gt;Diagnostics follow the Systems page: the committed repair drive while Host Maintenance is off, or the protected running host while it is active.&lt;br&gt;&lt;br&gt;The first privileged action requests administrator authorization once for this Boot Bitch window; &lt;b&gt;File - Lock Administrator Session&lt;/b&gt; ends that helper session immediately. Every repair keeps the helper&apos;s own runtime preflights.</source>
        <translation>Boot Bitch, sistem tamir edildiğinden farklı bir booted Linux ortamından geçmelidir. Farklı fiziksel bir sürücü üzerinde bir Linux canlı orta veya başka bir Linux kurulumu kullanın.&lt;br&gt;&lt;br&gt;&lt;br&gt;&lt;br&gt; Koşu ev sahibi sıradan onarım- hedef seçiminden korunuyor, ancak açıkça &lt;b&gt;Host Care&lt;/b&gt; Korumalı yerli tanılar ve bakım aşamaları için &lt;br&gt;&lt;br&gt;&lt;br&gt;&lt;br&gt;&lt;br&gt; Tanılar Sistem sayfasını takip eder: Host Bakım kapalıyken yapılan onarım sürüşü veya aktifken korunan koşu ev sahibi.&lt;br&gt;&lt;br&gt;&lt;br&gt; İlk ayrıcalıklı eylem, bu Boot Bitch penceresi için bir kez yönetici izni talep eder; &lt;b&gt;File - Lock Administrator Session&lt;/b&gt; hemen o yardımcı seansı biter. Her onarım, yardımçının kendi koşu zamanı ön ışıklarını tutar.</translation>
    </message>
    <message>
        <source>Select a physical drive; Boot Bitch resolves the most likely Linux system volume automatically. The running host stays protected from ordinary target repairs, with a separate explicit host-maintenance path for its own system. The Details button shows the protected host&apos;s facts in the details pane; selecting any drive row restores the per-drive pane.</source>
        <translation>Fiziksel bir sürücü seçin; Boot Bitch en olası Linux sistemi hacmini otomatik olarak çözüyor. Çalışan ev sahibi sıradan hedef onarımlarından korunuyor, kendi sistemi için ayrı açık bir ev sahibi yolla. Detaylar düğmesi, korumalı ev sahibinin gerçekleri ayrıntıları panete gösterir; herhangi bir sürücü sırasını per-drive pane geri döndürür.</translation>
    </message>
    <message>
        <source>Refresh Devices</source>
        <translation>Yenileme Cihazları</translation>
    </message>
    <message>
        <source>Re-read the read-only kernel inventory (/proc/partitions, /proc/mounts, /proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* and the udev metadata database). No block device is opened and nothing is written.</source>
        <translation>Sadece çekirdek envanteri (/proc/partitions, /proc / sent, /proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* ve udev metadata veritabanı). Blok cihazı açılmaz ve hiçbir şey yazılır.</translation>
    </message>
    <message>
        <source>[OK]</source>
        <translation>[OK]</translation>
    </message>
    <message>
        <source>The running system was detected and stays protected from ordinary target repairs.</source>
        <translation>Koşu sistemi tespit edildi ve sıradan hedef onarımlardan korunuyor.</translation>
    </message>
    <message>
        <source>Detecting running system...</source>
        <translation>İşletim sistemi ...</translation>
    </message>
    <message>
        <source>Detecting protected storage...</source>
        <translation>Korumalı depolama...</translation>
    </message>
    <message>
        <source>PROTECTED</source>
        <translation>PROTECTED</translation>
    </message>
    <message>
        <source>The running host remains protected from ordinary repair-target operations.</source>
        <translation>Çalışan ev sahibi sıradan onarım hedef operasyonlarından korunuyor.</translation>
    </message>
    <message>
        <source>Details</source>
        <translation>Detaylar</translation>
    </message>
    <message>
        <source>Show read-only details for the protected running host.</source>
        <translation>Korumalı koşu ev sahibi için sadece ayrıntıları okuyun.</translation>
    </message>
    <message>
        <source>Host Maintenance</source>
        <translation>Host Bakım</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Varsayılan olun</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host (menu.lst default directive with a backup and rollback). Requires Host Maintenance and the cached host-default probe.</source>
        <translation>Kanonik yüklü çekirdek girişi, çalışan ev sahibi üzerinde varsayılan GRUB-legacy boot girişi yapın (menu.lst default yönergesi ile bir yedek ve rollback). Host Bakım ve önbellek host-default Pro gerektirir.</translation>
    </message>
    <message>
        <source>Available repair targets</source>
        <translation>Mevcut onarım hedefleri</translation>
    </message>
    <message>
        <source>Most likely first</source>
        <translation>En büyük olasılıkla ilk</translation>
    </message>
    <message>
        <source>Drives are listed by the read-only inventory. Select a row to inspect it; Select Target commits the selected non-host drive with its auto-resolved Linux root component.</source>
        <translation>Sürücüler okuma-sadece envanter tarafından listelenmiştir. Bunu denetlemek için bir sıra seçin; Hedef, otomatik olarak çözülmemiş Linux kök bileşeni ile seçilmiş olmayan sürüşü taahhüt eder.</translation>
    </message>
    <message>
        <source>Device</source>
        <translation>Device Device Device</translation>
    </message>
    <message>
        <source>Size</source>
        <translation>Boyut Boyutu</translation>
    </message>
    <message>
        <source>Type</source>
        <translation>Tipi Tipi Tipi Tipi</translation>
    </message>
    <message>
        <source>Filesystem</source>
        <translation>Filesystem</translation>
    </message>
    <message>
        <source>Select Target</source>
        <translation>Hedef seçin</translation>
    </message>
    <message>
        <source>Unlock</source>
        <translation>Unlock</translation>
    </message>
    <message>
        <source>Authorize</source>
        <translation>Authorize</translation>
    </message>
    <message>
        <source>Establish the privileged helper session for the current scope now instead of waiting for the next privileged action.</source>
        <translation>Mevcut kapsamı için ayrıcalıklı yardım oturumu kurmak, bir sonraki ayrıcalıklı eylemi beklemek yerine.</translation>
    </message>
    <message>
        <source>Committed target: none</source>
        <translation>Müthiş hedef: Hiçbir şey</translation>
    </message>
    <message>
        <source>Unlock state for the selected drive; the LUKS passphrase is never logged.</source>
        <translation>Seçilen sürücü için sınırsız devlet; LUKS passphrase asla giriş değildir.</translation>
    </message>
    <message>
        <source>Unlock status</source>
        <translation>Unlock statüsü</translation>
    </message>
    <message>
        <source>Read-only inventory plus helper-confirmed facts; mirrors the modern Qt6 Selected drive details panel.</source>
        <translation>Sadece envanter artı yardımcı gerçekleri okuyun; Modern Qt6 Seçilmiş sürücü ayrıntıları paneli.</translation>
    </message>
    <message>
        <source>Selected drive details</source>
        <translation>Seçilmiş sürücü detayları</translation>
    </message>
    <message>
        <source>Field</source>
        <translation>Field</translation>
    </message>
    <message>
        <source>Value</source>
        <translation>Değer Değer Değer Değer Değer Değer Değer Değer Değer Değer Değer Değer</translation>
    </message>
    <message>
        <source>Run All runs every available read-only diagnostic for the current scope; selecting a check runs it alone. Diagnostics are read-only and are the only evidence source for the gated repair actions.</source>
        <translation>Run All, mevcut kapsamı için her mevcut okuma-sadece tanı taşır; bir çek seçmek tek başına çalışır. Tanılar sadece okunur ve kapı tamir eylemleri için tek kanıt kaynağıdır.</translation>
    </message>
    <message>
        <source>Target: none selected</source>
        <translation>Hedef: Hiçbiri seçmedi</translation>
    </message>
    <message>
        <source>Diagnostics follow the committed repair target, or the protected running host while Host Maintenance is active.</source>
        <translation>Tanılar kararlı onarım hedefi takip eder veya Host Bakım aktifken korunan koşu hostunu takip eder.</translation>
    </message>
    <message>
        <source>Run All</source>
        <translation>Run All Run</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope.</source>
        <translation>Run All - mevcut kapsamı için her mevcut okuma-sadece tanı çalıştırın.</translation>
    </message>
    <message>
        <source>Target configuration:</source>
        <translation>Hedef konfigürasyonu:</translation>
    </message>
    <message>
        <source>Etch-era target configuration files; availability is probed read-only by the helper&apos;s diagnostics.</source>
        <translation>Etch-era hedef yapılandırma dosyaları; kullanılabilirlik sadece yardımcının tanıları tarafından okunur.</translation>
    </message>
    <message>
        <source>Edit Target File...</source>
        <translation>Edit Target File...</translation>
    </message>
    <message>
        <source>Runs one read-only diagnostic for the selected scope through the helper (`diagnose &lt;key&gt;` / `host-diagnose &lt;key&gt;`); Run All is the combined report.</source>
        <translation>Seçilmiş kapsamı sadece yardımcı aracılığıyla okunuşur (&apos;diagnose &lt;key&gt;&apos; / &quot;host-diagnose &lt;key&gt;&quot;; Run All, birleşik rapordur.</translation>
    </message>
    <message>
        <source>Diagnostic checks</source>
        <translation>Tanı kontrolleri</translation>
    </message>
    <message>
        <source>Check</source>
        <translation>Check Check Check Check</translation>
    </message>
    <message>
        <source>Selected diagnostic</source>
        <translation>Seçilmiş tanı</translation>
    </message>
    <message>
        <source>Select a diagnostic</source>
        <translation>Bir teşhis seçin</translation>
    </message>
    <message>
        <source>Choose a diagnostic from the list.</source>
        <translation>Listeden bir teşhis seçin.</translation>
    </message>
    <message>
        <source>Ready</source>
        <translation>Hazır Hazır Hazır Hazır</translation>
    </message>
    <message>
        <source>Results</source>
        <translation>Sonuçlar Sonuçlar Sonuçlar Sonuçlar</translation>
    </message>
    <message>
        <source>Run Diagnostic</source>
        <translation>Tanıyın</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic for the current scope.</source>
        <translation>Run Simulator - mevcut kapsamı için seçilmiş okuma-sadece tanıyı çalıştırın.</translation>
    </message>
    <message>
        <source>Copy Results</source>
        <translation>Kopya Sonuçlar</translation>
    </message>
    <message>
        <source>Save Results...</source>
        <translation>Sonuçlar Kaydet...</translation>
    </message>
    <message>
        <source>The Full Repair plan runs the selected legacy stages in order through the guarded helper; the individual tools run one stage at a time. Every action stays disabled until the cached capability lines say available and the helper keeps its runtime preflights.</source>
        <translation>Full Tamir planı, korumalı yardımcı aracılığıyla seçilmiş miras aşamalarını işletiyor; bireysel araçlar bir seferde bir aşama yönetiyor. Her eylem, önbellek kapasite hatlarının mevcut olduğu zamana kadar devre dışı kalır ve yardımcı, runtime preflights tutar.</translation>
    </message>
    <message>
        <source>Full Repair plan</source>
        <translation>Full Tamir planı</translation>
    </message>
    <message>
        <source>No stages selected</source>
        <translation>Seçilen aşamalar</translation>
    </message>
    <message>
        <source>Configure Plan...</source>
        <translation>Configure Plan...</translation>
    </message>
    <message>
        <source>Open Settings to choose which Full Repair stages are part of the plan.</source>
        <translation>Tam onarım aşamalarının planın bir parçası olduğunu seçmek için açık ayarlar.</translation>
    </message>
    <message>
        <source>Run Full Repair</source>
        <translation>Run Full Repair</translation>
    </message>
    <message>
        <source>Select a repair drive, or choose Host Maintenance on the protected running-host card.</source>
        <translation>Bir onarım sürücüsü seçin veya korumalı koşu-host kartında Host Bakım seçin.</translation>
    </message>
    <message>
        <source>Stage</source>
        <translation>Aşama Aşama Aşama</translation>
    </message>
    <message>
        <source>Individual repair tools</source>
        <translation>Bireysel onarım araçları</translation>
    </message>
    <message>
        <source>Tool</source>
        <translation>Tool Tool Tool</translation>
    </message>
    <message>
        <source>Full Repair</source>
        <translation>Full Onarım Full</translation>
    </message>
    <message>
        <source>not reported</source>
        <translation>bildirilen rapor değil</translation>
    </message>
    <message>
        <source>Selected tool</source>
        <translation>Seçilmiş alet</translation>
    </message>
    <message>
        <source>Select a repair tool</source>
        <translation>Bir onarım aracı seçin</translation>
    </message>
    <message>
        <source>Run Tool</source>
        <translation>Run Tool</translation>
    </message>
    <message>
        <source>Select a tool to review its repair action.</source>
        <translation>onarım eylemlerini gözden geçirmek için bir araç seçin.</translation>
    </message>
    <message>
        <source>Write actions ask for confirmation and then run the helper&apos;s own runtime preflights; the GUI never weakens them. A repair that is not proven &apos;unchanged&apos; invalidates the cached diagnostics and disables the gated actions until diagnostics run again.</source>
        <translation>Eylemler onay isteyin ve sonra yardımçının kendi koşu zaman ön ışıklarını çalıştırın; GUI onları asla zayıflatmıyor. Önbellekli tanıları geçersiz kılan ve tekrar yapılana kadar kapılı eylemleri devre dışı bırakan bir onarım.</translation>
    </message>
    <message>
        <source>Chroot shell</source>
        <translation>Chroot kabuk</translation>
    </message>
    <message>
        <source>Offline commands run one at a time in a fresh chroot and cannot answer interactive prompts (apt-get -y upgrade works). Host-shell commands run directly on the running host. The helper&apos;s probe lines gate the command field; the exact reason appears in the tooltip.</source>
        <translation>Offline komutlar taze bir parçada bir seferde çalışır ve interaktif hızlı cevap veremez (apt-get -y upgrade works). Host-shell komutları doğrudan çalışan ev sahibi üzerinde çalışır. helper&apos;in son hatları komut alanını kontrol eder; tam sebep araçtipinde görünür.</translation>
    </message>
    <message>
        <source>No &apos;Legacy feature shell:&apos; line is cached; run diagnostics for the selected scope to evaluate the helper&apos;s chroot/timeout containment probes (fail closed).</source>
        <translation>Hiçbir &apos;Legacy özelliği kabuk:&apos; hattı önbelleklenir; yardımcının chroot/timeout holdingleri (fail kapalı).</translation>
    </message>
    <message>
        <source>Command</source>
        <translation>Komut</translation>
    </message>
    <message>
        <source>Command:</source>
        <translation>Komutan:</translation>
    </message>
    <message>
        <source>One reviewed command string, passed to the helper as a single argument (no shell interpolation by the GUI).</source>
        <translation>Bir incelenen komut dizesi, tek bir argüman olarak yardımcıya geçti ( GUI tarafından kabuk interpolasyon).</translation>
    </message>
    <message>
        <source>Run Command</source>
        <translation>Run Komutanlığı</translation>
    </message>
    <message>
        <source>Clear Output</source>
        <translation>Clear Çıkış</translation>
    </message>
    <message>
        <source>The helper exposes `shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for an offline target chroot and `host-shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for the running host. Both keep the helper&apos;s runtime preflights; this tab enables the command only when the scope is committed, the session is authorized and the scope&apos;s Legacy feature probe reports available.</source>
        <translation>helper, &quot;shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;&quot; çevrimdışı bir hedef chroot ve &quot;host-shell &lt;disk&gt; &lt;root&gt; &lt;command&gt; Her ikisi de yardımçının runtime preflights tutmaktadır; bu sekme yalnızca kapsamın işlendiği zaman, seans yetkilidir ve mevcut olan Mirassal özellik raporlarına sahiptir.</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Dosya kopyası</translation>
    </message>
    <message>
        <source>Copy and verify files in either direction through the guarded helper (cp -a plus ownership restoration and a per-file byte-compare). The helper&apos;s file-copy probe gates the controls and keeps the direction and path containment checks.</source>
        <translation>Korumalı yardımcı (cp -a artı mülkiyet restorasyonu ve bir per-file byte-compare) aracılığıyla dosyaları kopyalayın ve doğrulayın. helper&apos;in dosya-copy probe kapıları kontrolleri kontrol eder ve yön ve yol kontrolleri tutar.</translation>
    </message>
    <message>
        <source>Preview Changes</source>
        <translation>Preview Changes Changes</translation>
    </message>
    <message>
        <source>Run a copy dry-run through the guarded helper. No files are changed.</source>
        <translation>Korumalı yardımcı aracılığıyla bir kopyasını çalıştırın. Hiçbir dosya değiştirilemez.</translation>
    </message>
    <message>
        <source>Copy and Verify</source>
        <translation>Kopyalama ve Doğrulama</translation>
    </message>
    <message>
        <source>Copy staged items and verify the result. Existing destination names are overwritten when source content differs; unrelated destination files are never deleted.</source>
        <translation>Kopyalanmış eşyaları kopyalayın ve sonucu doğrulayın. Mevcut hedef isimleri kaynak içeriği farklı olduğunda yazılır; ilgili varış dosyaları asla silinmez.</translation>
    </message>
    <message>
        <source>Direction:</source>
        <translation>Yön:</translation>
    </message>
    <message>
        <source>Choose which system supplies the source files and which system receives them.</source>
        <translation>Hangi sistem kaynak dosyalarını tedarik eder ve hangi sistem onları alır.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from this host</source>
        <translation>1. Kaynak dosyaları veya klasörleri bu ev sahibinden seçin</translation>
    </message>
    <message>
        <source>Source</source>
        <translation>Kaynak Kaynağı</translation>
    </message>
    <message>
        <source>Files and folders staged for the verified copy. The legacy backend copies with cp -a and restores ownership with chown --reference; every regular file is byte-compared after the copy.</source>
        <translation>Dosyalar ve klasörler doğrulanmış kopya için sahnelendi. cp ile mirası geri kazanan kopyalar -a ve teslimiyetle -reference; her düzenli dosya kopyadan sonra iptal edilir.</translation>
    </message>
    <message>
        <source>Add Files...</source>
        <translation>Dosyalar ekleyin...</translation>
    </message>
    <message>
        <source>Add Folder...</source>
        <translation>Add Folder...</translation>
    </message>
    <message>
        <source>Remove</source>
        <translation>Kaldırın</translation>
    </message>
    <message>
        <source>Clear</source>
        <translation>Clear Clear Clear Clear</translation>
    </message>
    <message>
        <source>Clear the staged source list (nothing is copied or deleted).</source>
        <translation>Sahneli kaynak listesini açın (hiçbir şey kopyalanır veya silinir).</translation>
    </message>
    <message>
        <source>2. Choose destination in repaired system</source>
        <translation>2. onarılma sisteminde hedef seçin</translation>
    </message>
    <message>
        <source>unavailable: see the helper&apos;s Legacy feature file-copy: probe reason above</source>
        <translation>Yok: helper&apos;in Miras özelliği dosyasının kopyasını görün: Yukarıdaki son sebep:</translation>
    </message>
    <message>
        <source>An absolute path inside the selected repair system (Host to Repair) or on the running host (Repair to Host).</source>
        <translation>Seçilmiş onarım sistemi içinde mutlak bir yol (Host to Onarım) veya koşu hostunda (Ev sahibine devretme).</translation>
    </message>
    <message>
        <source>Browse Target Folders...</source>
        <translation>Hedef Folders&apos;a göz atın...</translation>
    </message>
    <message>
        <source>Browse the selected repair system through the helper&apos;s temporary read-only mounts and choose an absolute destination path. No target files are changed while browsing.</source>
        <translation>Seçilmiş onarım sistemi, yardımcının geçici okuması ile göz atın ve mutlak bir varış yolunu seçin. Tarama sırasında hedef dosyaları değiştirilemez.</translation>
    </message>
    <message>
        <source>3. Ownership and copy policy</source>
        <translation>3. Mülkiyet ve kopyalama politikası</translation>
    </message>
    <message>
        <source>Ownership:</source>
        <translation>Sahiplik:</translation>
    </message>
    <message>
        <source>Smart destination ownership (recommended)</source>
        <translation>Akıllı destinasyon sahipliği (recommended)</translation>
    </message>
    <message>
        <source>Preserve source numeric UID/GID</source>
        <translation>Kaynak numeric UID/GID</translation>
    </message>
    <message>
        <source>Smart mode validates UID/GID identity mapping across the two systems and falls back to the destination-directory owner when the same numeric ID means a different account (the legacy backend implements it with chown --reference).</source>
        <translation>Akıllı mod, UID/GID kimlik haritasını iki sistem üzerinden doğruluyor ve aynı sayısal kimlik farklı bir hesap anlamına geldiğinde varış sahibine geri dönüyor ( mirasın geri dönüşleri)</translation>
    </message>
    <message>
        <source>Application log</source>
        <translation>Başvuru logu</translation>
    </message>
    <message>
        <source>The complete session register; Save As... writes every entry even while a filter hides lines. If a writable system share mount exists at /host, Save As... starts there; otherwise the log directory is the fallback. Prior session files are listed read-only.</source>
        <translation>Tamamlanan oturum kaydı; Kaydet As... bir filtre çizerken bile her girişi yazın. Eğer ritable bir sistem payı var /host, Save As... orada başlar; aksi takdirde log rehberi geri çekilmedir. Önceki oturum dosyaları sadece okunur.</translation>
    </message>
    <message>
        <source>Session logs</source>
        <translation>Oturum logları</translation>
    </message>
    <message>
        <source>Session</source>
        <translation>Oturum Oturum</translation>
    </message>
    <message>
        <source>The first entry is the live session; earlier files in the log directory are listed read-only below it.</source>
        <translation>İlk giriş canlı seanstır; log dizinindeki önceki dosyalar sadece aşağıda listelenmiştir.</translation>
    </message>
    <message>
        <source>New Session Log</source>
        <translation>Yeni Oturum Girişi</translation>
    </message>
    <message>
        <source>Close the active session file; it becomes a prior session and the next log entry starts a new file.</source>
        <translation>Aktif dosyayı kapatın; önceki bir oturum haline gelir ve bir sonraki giriş yeni bir dosya başlatır.</translation>
    </message>
    <message>
        <source>Add Note</source>
        <translation>Add Not Add Note Add Add Add Not Add Note Add Add Note Add Add Add Not Add Not Add Note Add Add Add Add Not Add Note Add Add Add Add Note Add Add Add Not Add Add Note Add Add Add Add Add Not Add Note Add Add Add Add Add Not Add Not Add Add Note Add Add Add Add Add Not Add Note Add Add Add Not Add Not Add Not Add Not Add Not Add Not Add Not Add Not Add Not Add Note Add Add Add Add Add Not Add Not Add Add Add Add Note Note Note Add Add Add Add Add Not Add Add Add Add Add Not Add Not Add Not Add Not Add Not Add Not Add Not Add Not Add Not Add Add Not Add Add Add Add Note Note Note Add Add Add Add Add Add Add Add Add Add Add Add Add Add Add Add Not Add Add Add Add Add Add Note Note Note Note Note Note Note Note Note Add Add Add Add Add Add Add Add Add Add Add Add Add Not Add Add Add Add Add Add Add Add Add Add Add Add Add Add Add Add Add Add Add Add Add Add Add Add Add Add Add Note Note Note Note Note Note Note Add Add Add Add Add Add Add Add Add Add Add Add Add Not Add Add Add Add Add Add Add Add Add Add Add Add Add Add Add Add Add Add Add Add</translation>
    </message>
    <message>
        <source>Append a NOTE entry to the live session register.</source>
        <translation>Canlı oturum kaydına bir NOT girişi gönderin.</translation>
    </message>
    <message>
        <source>Delete</source>
        <translation>Delete</translation>
    </message>
    <message>
        <source>Delete the selected prior session file (the live session is never deleted).</source>
        <translation>Seçilmiş önceki oturum dosyasını (yaşam seansı asla silinmez).</translation>
    </message>
    <message>
        <source>Refresh</source>
        <translation>Yenileme</translation>
    </message>
    <message>
        <source>Save As...</source>
        <translation>Kurtarın...</translation>
    </message>
    <message>
        <source>Save the complete session log (all entries, not just the current filter).</source>
        <translation>Tamam seans logunu kurtar (tüm girişler, sadece mevcut filtre değil).</translation>
    </message>
    <message>
        <source>Clear Register</source>
        <translation>Clear Register</translation>
    </message>
    <message>
        <source>Clear the live register and view; prior session files are never modified.</source>
        <translation>Canlı kayıt ve görüşü açıklayın; seans dosyaları asla değiştirilemez.</translation>
    </message>
    <message>
        <source>Search log:</source>
        <translation>Arama Girişi:</translation>
    </message>
    <message>
        <source>Type any characters to show matching log entries (case-insensitive). Save As always writes every entry.</source>
        <translation>Eş giriş giriş girişlerini göstermek için herhangi bir karakter yazın (case-in sensitive). Kaydet Kaydet Kaydet Her zaman her giriş yazarken.</translation>
    </message>
    <message>
        <source>Filter:</source>
        <translation>Filtre:</translation>
    </message>
    <message>
        <source>Filter the visible log by entry kind. Selecting a diagnostic section shows the lines captured for that section; a workflow filter such as File system repair or Package repair shows its mapped repair lines (File copy has no lines in this frontend). Save As always writes every entry.</source>
        <translation>Giriş türü ile görünür log filtreleyin. Bir teşhis bölümü bu bölüm için yakalanan hatları gösterir; File sistemi onarım veya Paket onarımı gibi bir iş akışı filtresi haritalı onarım hatları (File kopyası bu ön uçta hiçbir çizgiye sahiptir). Kaydet Kaydet Kaydet Her zaman her giriş yazarken.</translation>
    </message>
    <message>
        <source>Settings are stored per user under ~/.qt/, one file per settings group (devicesrc, logsrc, diagnosticsrc, repairrc), and are saved immediately on every change and on close. Launch the GUI as the same user to keep your overrides; a GUI started as root keeps its own copies.</source>
        <translation>Ayarlar kullanıcı başına ~ /.qt/, ayarlar grubu başına bir dosya (devicesrc, logsrc, tanısrc, onarımrc), ve hemen her değişiklik ve yakın zamanda kaydedilir. GUI&apos;yi overrides&apos;i tutmak için aynı kullanıcı olarak başlatın; bir GUI kendi kopyalarını tutar.</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Cihaz keşfi</translation>
    </message>
    <message>
        <source>Show devices without an identified Linux installation</source>
        <translation>Tanımlanmış bir Linux yükleme olmadan ekran cihazları</translation>
    </message>
    <message>
        <source>Show removable and USB storage</source>
        <translation>Açık ve USB depolama</translation>
    </message>
    <message>
        <source>Show encrypted devices before unlocking</source>
        <translation>Kilitlenmeden önce şifreli cihazlar göster</translation>
    </message>
    <message>
        <source>When off, drives without a visible Linux filesystem are hidden unless they still contain an encrypted device and encrypted devices are shown.</source>
        <translation>Ne zaman dışarıda görünür bir Linux dosya sistemi olmadan sürücüler hala şifreli bir cihaz ve şifreli cihazlar içermediği sürece gizlidir.</translation>
    </message>
    <message>
        <source>When off, removable and USB drives are hidden from the repair-target list.</source>
        <translation>Ne zaman kapalı, çıkarılabilir ve USB sürücüleri onarım hedef listesinden gizlidir.</translation>
    </message>
    <message>
        <source>When off, drives with an encrypted device are hidden until the volume is unlocked.</source>
        <translation>Ne zaman dışarıda, şifreli bir cihazla sürücüler, hacimin kilidini açana kadar gizlidir.</translation>
    </message>
    <message>
        <source>Include the %1 stage in the Full Repair plan. The stage runs in the plan order shown on the Repair tab.</source>
        <translation>%1 aşamasını Full Tamir planında ekleyin. Sahne, onarım sekmesinde gösterilen plan düzeninde çalışır.</translation>
    </message>
    <message>
        <source>Automatically regenerate read-only diagnostics after repairs or target changes</source>
        <translation>Onarım veya hedef değişikliklerden sonra otomatik olarak tekrarlanabilir okuma-sadece tanı</translation>
    </message>
    <message>
        <source>Regenerates the cached read-only diagnostics for the current scope after an operation that invalidates them (LUKS unlock, target configuration edit). It runs only inside an already authorized administrator session and never opens an authorization prompt by itself.</source>
        <translation>Mevcut kapsamı için önbellekli okuma-sadece tanıları onları geçersiz kılan bir işlemden sonra (LUKS kilidini, hedef yapılandırma düzenleme). Sadece zaten yetkili bir yönetici oturumu içinde çalışır ve asla kendi başına bir yetki açmaz.</translation>
    </message>
    <message>
        <source>Wrap long log lines</source>
        <translation>uzun log hatları</translation>
    </message>
    <message>
        <source>Mandatory safety controls</source>
        <translation>Mandatory güvenlik kontrolleri</translation>
    </message>
    <message>
        <source>Host capabilities and dependencies</source>
        <translation>Host yetenekleri ve bağımlılıkları</translation>
    </message>
    <message>
        <source>Refresh Capabilities</source>
        <translation>Yenileme</translation>
    </message>
    <message>
        <source>Re-run the read-only host capability probes (a PATH search, nothing is executed) and refresh the distribution summary.</source>
        <translation>Sadece ev sahibi yeteneği (a PATH arama, hiçbir şey idam edilmez) ve dağıtım özetini yenileme.</translation>
    </message>
    <message>
        <source>Install Missing Support...</source>
        <translation>Eksik Destek yükleyin...</translation>
    </message>
    <message>
        <source>Automatic installation will require explicit package mapping and privilege authorization.</source>
        <translation>Otomatik yükleme açık paket haritalama ve ayrıcalık izni gerektirecektir.</translation>
    </message>
    <message>
        <source>Application configuration</source>
        <translation>Uygulama yapılandırma</translation>
    </message>
    <message>
        <source>Boot Bitch Legacy</source>
        <translation>Boot Bitch Legacy</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list first.</source>
        <translation>Mevcut onarım hedefleri listesinde ilk olarak fiziksel bir sürücü seçin.</translation>
    </message>
    <message>
        <source>Protected system</source>
        <translation>Korumalı sistem</translation>
    </message>
    <message>
        <source>The running system cannot be selected as a repair target. Use Host Maintenance for the protected running host or choose another disk.</source>
        <translation>Koşu sistemi bir onarım hedefi olarak seçilebilir. Korumalı koşu ev sahibi için ev sahibi bakım kullanın veya başka bir disk seçin.</translation>
    </message>
    <message>
        <source>Unlock or select a Linux system first</source>
        <translation>Unlock veya Linux sistemini ilk önce seçin</translation>
    </message>
    <message>
        <source>This encrypted drive has no visible Linux filesystem yet. Use Unlock, refresh devices, and select the target after its Linux root is detected.</source>
        <translation>Bu şifreli sürücü henüz görünür Linux dosya sistemi yoktur. Unlock, yenileme cihazları kullanın ve Linux kökünün tespit edilmesinden sonra hedefi seçin.</translation>
    </message>
    <message>
        <source>The selected root component (%1) belongs to the running system and cannot be committed as a repair target. Use Host Maintenance for the protected running host.</source>
        <translation>Seçilen kök bileşeni (%1) çalışan sisteme aittir ve bir onarım hedefi olarak belirlenemez. Korumalı koşu ev sahibi için Host Bakım kullanın.</translation>
    </message>
    <message>
        <source>repair target commit</source>
        <translation>onarım hedefi</translation>
    </message>
    <message>
        <source>Repair drive selected: %1</source>
        <translation>Seçilmiş onarım: %1</translation>
    </message>
    <message>
        <source>; best detected system component: %1</source>
        <translation>En iyi tespit edilen sistem bileşeni: %1</translation>
    </message>
    <message>
        <source>. No mount or repair action was performed.</source>
        <translation>. Hiçbir yükleme veya onarım eylemi yapıldı.</translation>
    </message>
    <message>
        <source>Make Default unavailable</source>
        <translation>Varsayılan olarak kullanılamaz</translation>
    </message>
    <message>
        <source>Make Default is a running-host action on this frontend; enter Host Maintenance first.</source>
        <translation>Varsayılanlik bu cephede çalışan bir aksiyondur; önce Host Bakım&apos;a girin.</translation>
    </message>
    <message>
        <source>Administrator authorization required</source>
        <translation>Yöneticilerin yetkilendirilmesi gerekli</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize first.</source>
        <translation>Hiçbir yönetici oturumu aktif değildir; ilk önce basın.</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host?

The helper verifies /boot/grub/menu.lst, sets the `default &lt;N&gt;` directive to the canonical entry, backs the menu up first and restores it on any failure.</source>
        <translation>Kanonik yüklü çekirdek girişi, çalışan ev sahibine varsayılan GRUB-legacy boot girişi yapabilir mi?

Destekleyici /boot/grub/menu.lst, &quot;default &lt;N&gt;&quot; yönergesini ilk olarak geri döndürür ve herhangi bir başarısızlık üzerine geri döndürür.</translation>
    </message>
    <message>
        <source>Host Maintenance unavailable</source>
        <translation>Host Bakım Yok</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a committed repair target or a detected running host.</source>
        <translation>Koşu ev sahibi hedefi tespit edilemeyebilir; tanıların kararlı bir onarım hedefine veya tespit edilen bir ev sahibine ihtiyacı vardır.</translation>
    </message>
    <message>
        <source>Exit Host Maintenance</source>
        <translation>Çıkış Host Bakım</translation>
    </message>
    <message>
        <source>Host Maintenance is active; diagnostics and gated repairs target the protected running host.</source>
        <translation>Host Bakım aktiftir; tanılar ve kapılı onarımlar korumalı koşu hostunu hedef alır.</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list on the Systems tab first, or use Host Maintenance for the protected running host.</source>
        <translation>Mevcut onarım hedefleri listesinde ilk olarak fiziksel bir sürücü seçin veya korumalı koşu ev sahibi için Host Bakım kullanın.</translation>
    </message>
    <message>
        <source>The selected drive (%1) is the protected running host. Choose Host Maintenance on the Systems tab to run read-only host diagnostics and guarded host repairs; ordinary target repairs stay disabled.</source>
        <translation>Seçilen sürücü (%1) korumalı koşu ev sahibidir. Sadece ev sahibi tanıları çalıştırmak ve ev sahibi onarımları yürütmek için Sistem sekmesinde ev sahibi bakım seçin; sıradan hedef onarımları devre dışı kalır.</translation>
    </message>
    <message>
        <source>No repair target is committed. Choose Select Target on the Systems tab (or Host Maintenance for the protected running host) first.</source>
        <translation>Hiçbir onarım hedefi işlemez. İlk önce korunan koşu ev sahibi için (veya Host Bakım) için Hedef seçin.</translation>
    </message>
    <message>
        <source>The selection changed after the target was committed. Choose Select Target again.</source>
        <translation>Hedef işlendikten sonra seçim değişti. Hedefi tekrar seçin.</translation>
    </message>
    <message>
        <source>Diagnostics scope required</source>
        <translation>Tanı kapsamı gerekli</translation>
    </message>
    <message>
        <source>Diagnostics scope unresolved</source>
        <translation>Tanılar kapsamı çözülmemiş</translation>
    </message>
    <message>
        <source>The running host target could not be resolved; use Refresh Devices and commit a repair target or re-enter Host Maintenance.</source>
        <translation>Koşu ev sahibi hedefi çözülmedi; Yenileme Cihazları kullanın ve bir onarım hedefi veya yeniden giriş Host Bakım&apos;ı işleyebilir.</translation>
    </message>
    <message>
        <source>Diagnostic check required</source>
        <translation>Tanı kontrolü gerekli</translation>
    </message>
    <message>
        <source>Select a diagnostic check in the list first.</source>
        <translation>İlk önce listede bir tanı kontrolü seçin.</translation>
    </message>
    <message>
        <source>Repair target required</source>
        <translation>Gerekli onarım hedefi gerekli</translation>
    </message>
    <message>
        <source>Target file editing needs a committed repair target. Running-host maintenance has no target-file editing; commit an offline target first.</source>
        <translation>Hedef dosya düzenlemenin kararlı bir onarım hedefine ihtiyacı vardır. Run-host maintenance&apos;in hedef-file düzenlemesi yoktur; önce çevrimdışı bir hedef yapın.</translation>
    </message>
    <message>
        <source>Configuration file required</source>
        <translation>Gerekli Belgeler</translation>
    </message>
    <message>
        <source>Select a target configuration file first.</source>
        <translation>Önce bir hedef yapılandırma dosyası seçin.</translation>
    </message>
    <message>
        <source>Edit target %1</source>
        <translation>Edit hedef %1X</translation>
    </message>
    <message>
        <source>Edit this target file through the guarded administrator helper. A successful save invalidates cached diagnostics; rerun diagnostics before repair. Generated files such as /boot/grub/menu.lst may be replaced by the next bootloader update.</source>
        <translation>Korumalı yönetici yardımcısı aracılığıyla bu hedef dosyayı analiz edin. Başarılı bir kurtarma önbellekli tanılar; onarımdan önce yeniden teşhis. /boot/grub/menu.lst gibi genleştirilmiş dosyalar bir sonraki bootloader güncellemesi tarafından değiştirilebilir.</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Cancel Cancel Cancel Cancel</translation>
    </message>
    <message>
        <source>Save Target File</source>
        <translation>Hedef Dosyasını Kurtarın</translation>
    </message>
    <message>
        <source>No changes to %1.</source>
        <translation>%1 için hiçbir değişiklik yok.</translation>
    </message>
    <message>
        <source>Configuration write refused</source>
        <translation>Revizyon yazısı reddedildi</translation>
    </message>
    <message>
        <source>The edited content contains NUL bytes; the guarded write refuses it.</source>
        <translation>Düzenlenen içerik NUL bytes içerir; bekçili yazı bunu reddediyor.</translation>
    </message>
    <message>
        <source>File too large</source>
        <translation>Dosya çok büyük büyük büyük büyük</translation>
    </message>
    <message>
        <source>The edited file is larger than 1 MiB. The guarded write refuses it; edit the file from a console instead.</source>
        <translation>Düzenlenen dosya 1 MiB&apos;den daha büyük. Korumalı yazı bunu reddediyor; dosyayı yerine bir konsoldan düzenler.</translation>
    </message>
    <message>
        <source>Write target configuration</source>
        <translation>Write Target configuration</translation>
    </message>
    <message>
        <source>Write the edited contents to %1? This modifies the repair target and invalidates cached diagnostics.</source>
        <translation>%1 için düzenlenmiş içerikleri yazın? Bu, onarım hedefini değiştirir ve önbellek tanıları geçersiz kılar.</translation>
    </message>
    <message>
        <source>No diagnostic results to copy yet.</source>
        <translation>Henüz kopyalanması için teşhis sonuçları yok.</translation>
    </message>
    <message>
        <source>Diagnostic results copied to the clipboard.</source>
        <translation>Tanı sonuçları panoya kopyaladı.</translation>
    </message>
    <message>
        <source>No diagnostic results to save yet.</source>
        <translation>Henüz kurtarmak için teşhis sonuçları yok.</translation>
    </message>
    <message>
        <source>Text files (*.txt);;All files (*)</source>
        <translation>Text dosyaları (*.txt);; Tüm dosyalar (*)</translation>
    </message>
    <message>
        <source>Save diagnostic results</source>
        <translation>Tanık sonuçları Kaydet</translation>
    </message>
    <message>
        <source>Could not write %1.</source>
        <translation>%1 yazamaz.</translation>
    </message>
    <message>
        <source>Diagnostic results saved to %1</source>
        <translation>Tanı sonuçları %1&apos;e kurtarıldı</translation>
    </message>
    <message>
        <source>Unlock not available</source>
        <translation>Unlock mevcut değil</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked. Select an offline repair target to unlock.</source>
        <translation>Korumalı koşu ev sahibi açıklanamaz. kilidini açmak için çevrimdışı bir onarım hedefi seçin.</translation>
    </message>
    <message>
        <source>No locked LUKS component is currently visible on this selected drive.</source>
        <translation>Kilitli LUKS bileşeni şu anda bu seçilmiş sürücüde görünür.</translation>
    </message>
    <message>
        <source>Confirm LUKS unlock</source>
        <translation>LUKS kilidini açın</translation>
    </message>
    <message>
        <source>Unlock %1 on %2?

The helper opens a temporary device-mapper mapping with cryptsetup and keeps it open for this recovery session. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>%2&apos;te Unlock %1?

Yardımcı, şifrele geçici bir cihaz-mapper haritasını açar ve bu kurtarma seansı için açık tutar. Geçiş özel bir dezavantajla seyahat eder ve hiçbir zaman komut argümanlarına veya loglara yerleştirilir.</translation>
    </message>
    <message>
        <source>LUKS unlock</source>
        <translation>LUKS kilidini açın</translation>
    </message>
    <message>
        <source>Unlock LUKS repair target</source>
        <translation>Unlock LUKS onarım hedefi</translation>
    </message>
    <message>
        <source>Enter the passphrase for %1.

It is sent only to cryptsetup over the helper&apos;s standard input and is never logged or placed on a command line.</source>
        <translation>%1 için geçiş yapın.

Sadece yardımcı standart girişi üzerinden şifrelenmek için gönderilir ve hiçbir zaman bir komut hattına girişilir veya yerleştirilir.</translation>
    </message>
    <message>
        <source>Passphrase required</source>
        <translation>Passphrase gerekli</translation>
    </message>
    <message>
        <source>An empty passphrase was not submitted. Enter the LUKS passphrase or choose Cancel.</source>
        <translation>Boş bir geçiş yapılmadı. LUKS passphrase girin veya Cancel&apos;u seçin.</translation>
    </message>
    <message>
        <source>Unlock keyfile unavailable</source>
        <translation>Unlock Prole kullanılamaz</translation>
    </message>
    <message>
        <source>The LUKS passphrase could not be written to a private keyfile in %1; the unlock was not started.</source>
        <translation>LUKS passphrase %1&apos;te özel bir dezavantajla yazılamaz; kilidini başlatmıyordu.</translation>
    </message>
    <message>
        <source>Configuration write unavailable</source>
        <translation>Yapı yazılmadı</translation>
    </message>
    <message>
        <source>The edited content could not be written to a private temporary file in %1; the write was not started.</source>
        <translation>Düzenlenen içerik %1&apos;te özel geçici bir dosyaya yazılamaz; yazı başlamadı.</translation>
    </message>
    <message>
        <source>ERROR: cannot read the session log %1</source>
        <translation>ERROR: Oturum log %1</translation>
    </message>
    <message>
        <source>Viewing a prior session log (read-only): %1</source>
        <translation>Önceki bir seansı görmek (yalnızca): %1</translation>
    </message>
    <message>
        <source>Session log list refreshed.</source>
        <translation>Oturum log listesi yenilendi.</translation>
    </message>
    <message>
        <source>Note:</source>
        <translation>Not:</translation>
    </message>
    <message>
        <source>Delete session log</source>
        <translation>Delete seansı</translation>
    </message>
    <message>
        <source>Delete %1 permanently? This cannot be undone.</source>
        <translation>Delete %1 kalıcı olarak? Bu geri alınmaz.</translation>
    </message>
    <message>
        <source>%1 changed while the confirmation was open; the delete was refused.</source>
        <translation>%1 onay açıkken değişti; silin reddedildi.</translation>
    </message>
    <message>
        <source>Unable to delete %1.</source>
        <translation>%1&apos;i silmek için kullanılamaz.</translation>
    </message>
    <message>
        <source>Device discovery filters updated.</source>
        <translation>Cihaz keşif filtreleri güncellendi.</translation>
    </message>
    <message>
        <source>Unknown Linux distribution</source>
        <translation>Bilinmeyen Linux dağıtım</translation>
    </message>
    <message>
        <source>sudo / gksu (no KAuth on this frontend)</source>
        <translation>Sudo / gksu (bu cephede KAuth)</translation>
    </message>
    <message>
        <source>Available</source>
        <translation>Mevcut kullanılabilir</translation>
    </message>
    <message>
        <source>Missing on this frontend</source>
        <translation>Bu cephede eksik</translation>
    </message>
    <message>
        <source>Missing</source>
        <translation>Eksikliği Eksikliği Eksik</translation>
    </message>
    <message>
        <source>No repair tool is selected.</source>
        <translation>Hiçbir onarım aracı seçilir.</translation>
    </message>
    <message>
        <source>A helper command is already running.</source>
        <translation>Bir yardımcı komut zaten çalışıyor.</translation>
    </message>
    <message>
        <source>Restore Graphical Login is a host-scope stage on this legacy frontend; enter Host Maintenance to run it.</source>
        <translation>Grafiksel Giriş, bu miras cephesinde ev sahibi bir aşamadır; onu çalıştırmak için Host Bakım girin.</translation>
    </message>
    <message>
        <source>The selected scope has no resolved root component; use Refresh Devices and commit the repair target again.</source>
        <translation>Seçilen kapsamın bir kök bileşeni yoktur; Yenileme Cihazları kullanın ve yeniden onarım hedefi işlemeyin.</translation>
    </message>
    <message>
        <source>This legacy frontend exposes no %1 action; the helper reports the capability as available.</source>
        <translation>Bu miras ön uçları %1 eylemini ortaya çıkarır; yardımcı cihazı mevcut olarak yeteneğini bildiriyor.</translation>
    </message>
    <message>
        <source>Run this guarded repair action using the cached read-only diagnostic evidence. A confirmation is shown first.</source>
        <translation>Bu koruyucu onarım eylemi önbellek okuma-sadece tanı kanıtlarını kullanarak çalıştırın. Bir onay ilk olarak gösterilir.</translation>
    </message>
    <message>
        <source>Enabled in Settings</source>
        <translation>Enabled in Settings</translation>
    </message>
    <message>
        <source>Disabled in Settings - enable it to include this stage</source>
        <translation>Ayarlarda Engelli - bu aşamayı içerecek şekilde etkinleştirin</translation>
    </message>
    <message>
        <source>Unavailable: %1</source>
        <translation>Un available: %1</translation>
    </message>
    <message>
        <source>
Unavailable: %1</source>
        <translation>
Un available: %1</translation>
    </message>
    <message>
        <source>Repair tool unavailable</source>
        <translation>Onarım aracı mevcut değil</translation>
    </message>
    <message>
        <source>Confirm repair</source>
        <translation>Onaylı onarım</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Your saved selection is kept and its availability is re-checked when diagnostics for this scope complete.</source>
        <translation>Henüz bu aşama için önbellek kapasite kanıtı yok. Tasarruf edilen seçiminiz tutulur ve kullanılabilirliği bu kapsamın tam olarak tanındığında yeniden kontrol edilir.</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Run diagnostics for the selected scope to populate the Full Repair plan; your selection is saved once the stage becomes available.</source>
        <translation>Henüz bu aşama için önbellek kapasite kanıtı yok. Seçilen kapsamın Full Tamir planını populate etmek için teşhis yapın; seçiminiz bir kez sahne mevcut hale gelir.</translation>
    </message>
    <message>
        <source>Unknown Full Repair stage.</source>
        <translation>Bilinmeyen Full Tamir aşaması.</translation>
    </message>
    <message>
        <source>No Full Repair stages are selected or available; use Configure Plan... to choose the stages.</source>
        <translation>Tam onarım aşamaları seçilir veya mevcut değildir; Configure Planı kullanın... aşamaları seçmek için.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab to establish the session.</source>
        <translation>Yönetici izni gereklidir; oturum kurmak için Sistem veya Onarım sekmesine İzin Vermek.</translation>
    </message>
    <message>
        <source>Runs the selected stages using the cached read-only diagnostic evidence after privilege confirmation.</source>
        <translation>Önbellekli okuma-sadece tanı kanıtlarını ayrıcalık onayından sonra kullanarak seçilen aşamaları çalıştırın.</translation>
    </message>
    <message>
        <source>No Full Repair stages selected - use Configure Plan... or Settings.</source>
        <translation>Seçilen Tam Onarım aşamaları yok - Configure Plan kullanın... veya ayarlar.</translation>
    </message>
    <message>
        <source>1 stage selected</source>
        <translation>1 aşama seçilmiş</translation>
    </message>
    <message>
        <source>%1 stages selected</source>
        <translation>%1 aşamaları seçilmiş</translation>
    </message>
    <message>
        <source>No repair stages are selected. Use Configure Plan... to choose the stages Full Repair will run.</source>
        <translation>Hiçbir onarım aşaması seçilir. Configure Plan kullanın... Tam Onarım&apos;ı seçmek için.</translation>
    </message>
    <message>
        <source>No selected stages are available. %1</source>
        <translation>Seçilen aşamalar mevcut değildir. %1</translation>
    </message>
    <message>
        <source>Full Repair unavailable</source>
        <translation>Tam Onarım Yok</translation>
    </message>
    <message>
        <source>Run the Full Repair plan?

The selected stages run in order through the helper&apos;s guarded repair command:

</source>
        <translation>Full Tamir planını çalıştırın mı?

Seçilmiş aşamalar, yardımcının muhafız onarım komutunu kullanarak çalışır:

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
helper her runtime preflight tutar; planı durduramayan bir sahne.</translation>
    </message>
    <message>
        <source>Running %1 through the privileged helper... The Logs tab keeps the complete transcript.</source>
        <translation>%1&apos;i ayrıcalıklı yardımcı aracılığıyla çalıştırın... Logs sekmesi tam transkript tutar.</translation>
    </message>
    <message>
        <source>Shell scope required</source>
        <translation>Shell kapsamı gerekli</translation>
    </message>
    <message>
        <source>No administrator session is active.

Press Authorize on the Systems or Repair tab to establish the session, or enter Host Maintenance / commit a repair target on the Systems tab; the chroot shell then reuses the cached authorization.</source>
        <translation>Hiçbir yönetici oturumu aktif değildir.

Oturumu kurmak için Sistem veya Onarım sekmesine İzin Vermek veya Host Bakım&apos;a girmek / Sistem sekmesine bir onarım hedefi uygulamak; o zaman önbelleği yeniden kullanın.</translation>
    </message>
    <message>
        <source>Shell command required</source>
        <translation>Shell komut gerekli</translation>
    </message>
    <message>
        <source>Enter the command to run first.</source>
        <translation>İlk önce çalıştırmak için komut girin.</translation>
    </message>
    <message>
        <source>Confirm running-host command</source>
        <translation>Koşu-host komutunu onaylayın</translation>
    </message>
    <message>
        <source>Run this command as root on the protected running host?

%1

The helper keeps its runtime preflights; the command is passed as one argument and is never interpreted by the GUI.</source>
        <translation>Bu komutu korumalı koşu hostunda kök olarak çalıştırın mı?

%1

helper runtime preflights tutuyor; komut bir argüman olarak geçti ve hiçbir zaman GUI tarafından yorumlanmadı.</translation>
    </message>
    <message>
        <source>Running %1...</source>
        <translation>%1 koşmak...</translation>
    </message>
    <message>
        <source>Authorization scope required</source>
        <translation>Yazarlaşma kapsamı gerekli</translation>
    </message>
    <message>
        <source>administrator authorization</source>
        <translation>yönetici yetkilendirme</translation>
    </message>
    <message>
        <source>Administrator authorization</source>
        <translation>Yöneticilerin yetkilendirilmesi</translation>
    </message>
    <message>
        <source>Administrator authorization is required for %1.

Enter the password for %2 (sudo). It is used only for this sudo authentication, is sent over a pipe and is never logged or placed on a command line. The authorization is cached for this session and reused by diagnostics and repairs.</source>
        <translation>Yönetim izni %1 için gereklidir.

%2 için parola girin (sudo). Sadece bu sudo doğrulama için kullanılır, bir boru üzerinden gönderilir ve hiçbir zaman bir komut hattına giriş veya yerleştirilir. Bu oturum için izin verilir ve tanı ve onarımlar ile yeniden kullanılabilir.</translation>
    </message>
    <message>
        <source>your account</source>
        <translation>Hesabınız</translation>
    </message>
    <message>
        <source>Password required</source>
        <translation>Şifre gerekli</translation>
    </message>
    <message>
        <source>An empty password was not submitted. Enter the sudo password or choose Cancel.</source>
        <translation>Boş bir şifre gönderilmedi. Sudo şifresini girin veya Cancel&apos;u seçin.</translation>
    </message>
    <message>
        <source>Administrator authorization failed</source>
        <translation>Yönetici izni başarısız oldu</translation>
    </message>
    <message>
        <source>sudo did not accept the password: %1

The command was not started.</source>
        <translation>Sudo şifreyi kabul etmedi: %1

Komut başlamadı.</translation>
    </message>
    <message>
        <source>elevation requires an interactive sudo password; run the smoke as root or after `sudo -S -v` with --elevate &apos;sudo -n&apos;</source>
        <translation>Yükseliş, interaktif bir sudo şifresi gerektirir; dumanı kök olarak çalıştırın veya sonra “sudo -S -v” ile değiştirin - &apos;sudo -n&apos;</translation>
    </message>
    <message>
        <source>No administrator session is active for %1.

Press Authorize on the Systems or Repair tab to establish the session now, or enter Host Maintenance / commit a repair target on the Systems tab; diagnostics and repairs then reuse the cached authorization.</source>
        <translation>Hiçbir yönetici oturumu %1 için aktif değildir.

Oturumu kurmak için Sistem veya Onarım sekmesine Yazarlık edin, ya da Host Bakım / Sistem sekmesinde bir onarım hedefi yapar; teşhisler ve onarımlar sonra önbellekli izni tekrarlar.</translation>
    </message>
    <message>
        <source>Administrator authorization expired</source>
        <translation>Yönetici izni süresi dolmuş</translation>
    </message>
    <message>
        <source>The cached administrator authorization for %1 expired or was refused.

Press Authorize on the Systems or Repair tab to re-establish the session, then run the command again. No command was started.</source>
        <translation>%1 için önbellek yönetici izni sona erdi veya reddedildi.

Oturumu yeniden kurmak için Sistem veya Onarım sekmesini Yazarlaştırın, sonra komutu tekrar çalıştırın. Hiçbir komut başlamadı.</translation>
    </message>
    <message>
        <source>Privileged operation completed successfully. Administrator authorization remains active for this session.</source>
        <translation>Privileged işlemi başarıyla tamamlandı. Yönetici izni bu oturum için aktif kalır.</translation>
    </message>
    <message>
        <source>Privileged operation stopped with an error. Administrator authorization remains active; review the output before closing.</source>
        <translation>Privileged operasyonu bir hatayla durdu. Yönetici izni aktif kalır; kapatmadan önce çıktıyı gözden geçirin.</translation>
    </message>
    <message>
        <source>Inspection is read-only.

</source>
        <translation>Muayene sadece okunur.

</translation>
    </message>
    <message>
        <source>Configuration unavailable</source>
        <translation>Oluşturulmadı</translation>
    </message>
    <message>
        <source>The helper could not read %1:

%2</source>
        <translation>Yardımcı %1&apos;i okumadı:

%2</translation>
    </message>
    <message>
        <source>Configuration write failed</source>
        <translation>Yapı yazısı başarısız oldu</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
Result: mapping opened for this recovery session.</source>
        <translation>Devlet: Açılan
Bitirme: %1
Mapper: %2
Yöntem: helper (cryptsetup; bir mod-600 Prole aracılığıyla geçiş, kullanımdan sonra silin)
Sonuç: Bu kurtarma seansı için haritalama açıldı.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: the passphrase was not accepted; retry offered.</source>
        <translation>Devlet: kilitli
Bitirme: %1
Yöntem: yardımcı (cryptsetup)
Hata: passphrase kabul edilmedi; yeniden deneme teklif edildi.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: %2</source>
        <translation>Devlet: kilitli
Bitirme: %1
Yöntem: yardımcı (cryptsetup)
Hata: %2</translation>
    </message>
    <message>
        <source>Passphrase not accepted</source>
        <translation>Passphrase kabul edilmedi</translation>
    </message>
    <message>
        <source>The LUKS passphrase was not accepted.

Try again?</source>
        <translation>LUKS passphrase kabul edilmedi.

Tekrar deneyin?</translation>
    </message>
    <message>
        <source>[-] %1 - not run (the plan stopped before reaching this stage)</source>
        <translation>%1 - koşmak ( planı bu aşamaya ulaşmadan önce durduruldu)</translation>
    </message>
    <message>
        <source>filesystem</source>
        <translation>filesystem</translation>
    </message>
    <message>
        <source>[-] %1 - no file system errors found - no changes</source>
        <translation>%1 - hiçbir dosya sistemi hataları bulunamadı - hiçbir değişiklik yok</translation>
    </message>
    <message>
        <source>[FAIL] %1 - not reported</source>
        <translation>[FAIL] %1 - raporlanmadı</translation>
    </message>
    <message>
        <source>%1 stage(s) failed; review the helper output in Logs.</source>
        <translation>%1 aşaması (s) başarısız oldu; Logs&apos;taki yardımcı çıktıyı gözden geçirin.</translation>
    </message>
    <message>
        <source>the plan stopped before any stage completed.</source>
        <translation>Plan herhangi bir aşamada tamamlanmadan önce durduruldu.</translation>
    </message>
    <message>
        <source>no repair was needed; cached diagnostics remain valid.</source>
        <translation>Hiçbir onarım gerekli değildi; önbellekli tanılar geçerli kalır.</translation>
    </message>
    <message>
        <source>repair completed; cached diagnostics were invalidated and must be regenerated.</source>
        <translation>onarım tamamlandı; önbellekli tanılar geçersizydi ve yeniden oluşturulmalıdır.</translation>
    </message>
    <message>
        <source>%1 results: [OK] %2 successful | [FAIL] %3 failed | [-] %4 no repair needed - %5</source>
        <translation>%1 sonuçları: [OK] %2 başarılı | [FAIL] %3 başarısız oldu | [-] %4 hiçbir onarım gerekli - %5</translation>
    </message>
    <message>
        <source>Re-run Diagnostic</source>
        <translation>Re-run Teşhis</translation>
    </message>
    <message>
        <source>Unavailable</source>
        <translation>Un available</translation>
    </message>
    <message>
        <source>Commit this physical drive as the repair target.</source>
        <translation>Bu fiziksel sürücüyü onarım hedefi olarak kabul edin.</translation>
    </message>
    <message>
        <source>Protection:</source>
        <translation>Koruma:</translation>
    </message>
    <message>
        <source>Host shell</source>
        <translation>Ev sahibi kabuk</translation>
    </message>
    <message>
        <source>Run on Host</source>
        <translation>Host üzerinde Run</translation>
    </message>
    <message>
        <source>Host Shell</source>
        <translation>Host Shell</translation>
    </message>
    <message>
        <source>The read-only probe found no editable target configuration file in this target. %1</source>
        <translation>Okunma yalnızca bu hedefte düzenlenebilir bir hedef yapılandırma dosyası bulamadı. %1</translation>
    </message>
    <message>
        <source>Not present in the selected target (omitted): %1</source>
        <translation>Seçilmiş hedefte mevcut değil (mevcut): %1</translation>
    </message>
    <message>
        <source>Run diagnostics to probe which target configuration files exist; the helper&apos;s read-only probe decides the list.</source>
        <translation>Hangi hedef yapılandırma dosyalarının var olduğunu araştırmak için teşhis yapın; yardımcının okuması yalnızca listeye karar verir.</translation>
    </message>
    <message>
        <source>Probed read-only by the helper; a saved edit invalidates the cached diagnostics.</source>
        <translation>Probed sadece yardımcı tarafından okunur; kurtarılmış bir düzenleme önbellekli tanıları geçersiz kılar.</translation>
    </message>
    <message>
        <source>Host maintenance: %1</source>
        <translation>Host bakım: %1</translation>
    </message>
    <message>
        <source>unresolved</source>
        <translation>Çözülmemiş</translation>
    </message>
    <message>
        <source>Target: %1 + %2</source>
        <translation>Hedef: %1 + %2</translation>
    </message>
    <message>
        <source>Host command</source>
        <translation>Host komut</translation>
    </message>
    <message>
        <source>Execute a command on the running host as root.</source>
        <translation>Bir komutu, çalışanın kök olarak yerine getirir.</translation>
    </message>
    <message>
        <source>Execute a command inside the selected repair system as root.</source>
        <translation>Seçilen onarım sisteminde kök olarak bir komut uygulayın.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab (or re-enter Host Maintenance / re-commit the repair target) to authorize this session.</source>
        <translation>Yönetici izni gereklidir; Sistem veya Onarım sekmesinde Authorize (veya Host Bakım / onarım hedefine yeniden giriş) bu seansı yazarken.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root on the running host through the helper&apos;s guarded host-shell verb.</source>
        <translation>Bir kumandayı, yardımcının muhafız ev sahibi tarafından yönetilen koşuta kök olarak çalıştırın.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root inside the target chroot through the helper&apos;s guarded shell verb.</source>
        <translation>Hedefin içinde kök olarak bir komut çalıştırın, yardımcının bekçili kabuğu fiilinden.</translation>
    </message>
    <message>
        <source>Run a command on the running host as root (sudo is not needed). Commands are executed directly on the active system; output is kept in this window and in the application log.</source>
        <translation>Operasyonda kök olarak bir komut çalıştırın (sudo gerekli değildir). Komutlar doğrudan aktif sistem üzerinde yürütülür; çıktı bu pencerede ve uygulama logunda tutulur.</translation>
    </message>
    <message>
        <source>Run a command inside the selected repair system as root (sudo is not needed). Commands are executed one at a time in a fresh chroot and cannot answer interactive prompts; use non-interactive flags such as apt-get -y upgrade. Output is kept in this window and in the application log.</source>
        <translation>Seçilen onarım sisteminde kök olarak bir komut çalıştırın (sudo gerekli değildir). Komutlar taze bir kümede bir anda bir kez idam edilir ve interaktif olarak cevap veremez; apt-get -y yükseltme gibi non-interaktif bayraklar kullanın. Çıktı bu pencerede ve uygulama logunda tutulur.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize or re-enter Host Maintenance / commit a repair target.</source>
        <translation>Hiçbir yönetici oturumu aktif değildir; basın Authorize veya re-enter Host Bakım / bir onarım hedefi yapar.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from the repaired system</source>
        <translation>1. Kaynak dosyaları veya klasörleri tamir sistemden seçin</translation>
    </message>
    <message>
        <source>2. Choose destination on this host</source>
        <translation>2. Bu ev hostunda hedef seçin</translation>
    </message>
    <message>
        <source>Add File Path...</source>
        <translation>File Path ekleyin...</translation>
    </message>
    <message>
        <source>Add Folder Path...</source>
        <translation>Add Folder Path...</translation>
    </message>
    <message>
        <source>Browse...</source>
        <translation>Göze...</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it (Host Maintenance does not provide a repair tree to browse).</source>
        <translation>Bir hedef seçmeden önce sistemlerdeki onarım sürüşünü seçin (Host Bakım, göz atmak için bir onarım ağacı sağlamaz).</translation>
    </message>
    <message>
        <source>Choose a host destination folder directly.</source>
        <translation>Ev sahibi bir hedef klasörü doğrudan seçin.</translation>
    </message>
    <message>
        <source>Copy the staged files and folders with cp -a, restore ownership with chown --reference and byte-compare every regular file afterwards.</source>
        <translation>Sahnedeki dosyaları ve klasörleri cp ile kopyalayın -a, sahipliği chown -reference and byte-compare her düzenli dosyayı daha sonra.</translation>
    </message>
    <message>
        <source>File Copy unavailable</source>
        <translation>Dosya Kopyaı Yok Yok Yok</translation>
    </message>
    <message>
        <source>Choose host destination folder</source>
        <translation>Ev sahibi hedef klasörü seçin</translation>
    </message>
    <message>
        <source>Select a repair target</source>
        <translation>Bir onarım hedefi seçin</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it.</source>
        <translation>İçinde bir hedef seçmeden önce sistemlerdeki onarım sürüşünü seçin.</translation>
    </message>
    <message>
        <source>File Copy - Browse Target Folders</source>
        <translation>Dosya Kopyaı - Hedef Katlayıcılara Göz</translation>
    </message>
    <message>
        <source>Browse Target Folders</source>
        <translation>Hedef Folders</translation>
    </message>
    <message>
        <source>The helper could not list the repair-system folder:

%1</source>
        <translation>Yardımcı onarım sistemi klasörü listeleyemedi:

%1</translation>
    </message>
    <message>
        <source>BROWSE_ENTRY</source>
        <translation>BROWSE ENTRY</translation>
    </message>
    <message>
        <source>(choose this folder: %1)</source>
        <translation>(Bu klasöre bakınız: %1)</translation>
    </message>
    <message>
        <source>(parent folder)</source>
        <translation>(baba klasörü)</translation>
    </message>
    <message>
        <source>Select repair-system destination</source>
        <translation>onarım sistemi destinasyonunu seçin</translation>
    </message>
    <message>
        <source>Choose a destination folder inside the repaired system (current folder: %1):</source>
        <translation>onarım sistemi içinde bir hedef klasörü seçin (şimdiki klasör: %1):</translation>
    </message>
    <message>
        <source>Folder</source>
        <translation>Folder</translation>
    </message>
    <message>
        <source>Open</source>
        <translation>Open Open Open Open</translation>
    </message>
    <message>
        <source>(choose this folder:</source>
        <translation>(Bu klasöre bakınız:</translation>
    </message>
    <message>
        <source>Add files to copy</source>
        <translation>kopyalamak için dosyaları ekleyin</translation>
    </message>
    <message>
        <source>Add folder to copy</source>
        <translation>kopyalamak için klasör ekleyin</translation>
    </message>
    <message>
        <source>Stage at least one source and name a destination path first.</source>
        <translation>Aşama en az bir kaynak ve ilk önce bir varış yolu adı.</translation>
    </message>
    <message>
        <source>Copy the %1 staged item(s) to %2?

The helper keeps its direction and path containment checks; a sensitive repair-system destination is refused unless the helper approves it, and every regular file is byte-compared after the copy.</source>
        <translation>%1 sahnelenmiş öğeyi %2&apos;e kopyalayın?

helper yönünü tutar ve yol kontrollerini tutar; yardımlayıcının onayladığı sürece hassas bir onarım-sistem hedefi reddedilir ve her düzenli dosya kopyadan sonra iptal edilir.</translation>
    </message>
    <message>
        <source>File Copy - Copy and Verify</source>
        <translation>Dosya Kopyaı - Kopyalama ve Doğrulama</translation>
    </message>
    <message>
        <source>File Copy - Preview Changes</source>
        <translation>File Copy - Preview Changes</translation>
    </message>
    <message>
        <source>Protected running host - details</source>
        <translation>Ev sahibi koşmak - detaylar</translation>
    </message>
    <message>
        <source>not mounted (offline target)</source>
        <translation>monte edilmedi (kapı hedef)</translation>
    </message>
    <message>
        <source>Select a drive to see its details.</source>
        <translation>Detaylarını görmek için bir sürücü seçin.</translation>
    </message>
    <message>
        <source>Inspecting the selected component.</source>
        <translation>Seçilen bileşeni göz önünde bulundurun.</translation>
    </message>
    <message>
        <source>Helper-confirmed by the last read-only diagnostics.</source>
        <translation>Son okuma-sadece tanı ile yardımcı olun.</translation>
    </message>
    <message>
        <source>Read-only inventory only; run diagnostics to confirm.</source>
        <translation>Sadece sadece envanteri okuyun; doğrulanması için teşhis yapın.</translation>
    </message>
    <message>
        <source>PROTECTED - running system; read-only details only</source>
        <translation>PROTECTED - koşu sistemi; sadece ayrıntılar sadece</translation>
    </message>
    <message>
        <source>Live / installer media - not selectable</source>
        <translation>Live / installer media - seçilemez</translation>
    </message>
    <message>
        <source>PROTECTED - running host scope</source>
        <translation>PROTECTED - host scope</translation>
    </message>
    <message>
        <source>Unlock required before selection</source>
        <translation>Seçilmeden önce gerekli</translation>
    </message>
    <message>
        <source>Eligible repair candidate</source>
        <translation>Uygun onarım adayı</translation>
    </message>
    <message>
        <source>Drive:</source>
        <translation>Drive:</translation>
    </message>
    <message>
        <source>Detected target:</source>
        <translation>Hedef:</translation>
    </message>
    <message>
        <source>Pending inspection</source>
        <translation>Pending denetimi</translation>
    </message>
    <message>
        <source>Model / label:</source>
        <translation>Model / etiket:</translation>
    </message>
    <message>
        <source>Status:</source>
        <translation>Durum:</translation>
    </message>
    <message>
        <source>Size:</source>
        <translation>Boyut:</translation>
    </message>
    <message>
        <source>Connection:</source>
        <translation>Bağlantı:</translation>
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
        <translation>Mounts:</translation>
    </message>
    <message>
        <source>Running system protection unresolved</source>
        <translation>Koşu sistemi koruması çözülmemiş</translation>
    </message>
    <message>
        <source>No protected physical backing disk was identified</source>
        <translation>Korumalı fiziksel gerileme diski tespit edilmedi</translation>
    </message>
    <message>
        <source>Current running Linux system</source>
        <translation>Mevcut işletim Linux sistemi</translation>
    </message>
    <message>
        <source>Critical mounts: %1</source>
        <translation>Eleştirel dağlar: %1</translation>
    </message>
    <message>
        <source>Read-only protected-running-system facts: the helper OS fact plus the inventory model, device path, size, transport and critical mounts. Nothing here is probed destructively.</source>
        <translation>Sadece korumalı sistem gerçeklerini okuyun: yardımcı OS aslında envanter modeli, cihaz yolu, boyut, ulaşım ve kritik yükler. Burada hiçbir şey yıkıcıdır.</translation>
    </message>
    <message>
        <source>Select a drive to see unlock status.</source>
        <translation>Açık durumu görmek için bir sürücü seçin.</translation>
    </message>
    <message>
        <source>State: protected
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
The protected running host cannot be unlocked or modified; unlock is available for an offline repair target only. Use Host Maintenance for the protected running host.</source>
        <translation>Devlet: Korumalı
Bitirme: %1
Mapper: (none)
Yöntem: helper (cryptsetup; bir mod-600 Prole aracılığıyla geçiş, kullanımdan sonra silin)
Korumalı koşu ev sahibi açıklanamaz veya değiştirilemez; yalnızca çevrimdışı bir onarım hedefi için açıklanır. Korumalı koşu ev sahibi için Host Bakım kullanın.</translation>
    </message>
    <message>
        <source>(none detected)</source>
        <translation>(biri tespit edildi)</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
A locked LUKS container is visible on this drive; press Unlock to open it for this recovery session.</source>
        <translation>Devlet: kilitli
Bitirme: %1
Mapper: (none)
Yöntem: helper (cryptsetup; bir mod-600 Prole aracılığıyla geçiş, kullanımdan sonra silin)
Kilitli bir LUKS konteyneri bu sürücüde görünür; bu kurtarma seansı için açmak için Unlock.</translation>
    </message>
    <message>
        <source>(visible mapper)</source>
        <translation>(visible mapper)</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: already open before this session (visible mapper; Boot Bitch will reuse it and will not close it).</source>
        <translation>Devlet: Açılan
Bitirme: %1
Mapper: %2
Yöntem: Bu seanstan önce zaten açık (görünmez haritaper; Boot Bitch onu tekrarlayacak ve kapatmayacak).</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper-confirmed mapping from the last read-only diagnostics.</source>
        <translation>Devlet: Açılan
Bitirme: %1
Mapper: %2
Yöntem: son okuma-sadece tanılardan yardım edilen haritalama.</translation>
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
        <translation>Devlet: kilitli veya şifreli bir bileşen tespit edilmedi
Bitirme: (biri tespit edildi)
Mapper: (none)
Yöntem: helper (cryptsetup; bir mod-600 Prole aracılığıyla geçiş, kullanımdan sonra silin)
Kilitli LUKS bileşeni ve mevcut oturumda bu sürücü için açık bir operasyon kaydedilmedi.</translation>
    </message>
    <message>
        <source>unavailable - %1</source>
        <translation>Yok - %1</translation>
    </message>
    <message>
        <source>Host maintenance:
%1</source>
        <translation>Host bakım:
%1</translation>
    </message>
    <message>
        <source>Target:
%1 + %2</source>
        <translation>Hedef: Hedef:
%1 + %2</translation>
    </message>
    <message>
        <source>Authorization required: diagnostics and repairs fail closed until you press Authorize.</source>
        <translation>Yazarlaşma gerekli: teşhis ve onarımlar, Yazarlarize&apos;ye kadar kapatılamaz.</translation>
    </message>
    <message>
        <source>Re-establish the privileged helper session for the current scope now. The password is requested in the hidden-input modal and is never logged.</source>
        <translation>Mevcut kapsamı için ayrıcalıklı yardım seansını yeniden kurdu. Şifre gizli modalde talep edilir ve asla giriş değildir.</translation>
    </message>
    <message>
        <source>Running...</source>
        <translation>Koşu...</translation>
    </message>
    <message>
        <source>Scope required</source>
        <translation>Gerekli Kapsam Gerekli</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope; this unlocks the gated actions.</source>
        <translation>Run All - mevcut kapsamı için her mevcut okuma-sadece tanı çalıştırın; bu kapılı eylemleri açar.</translation>
    </message>
    <message>
        <source>Select a target and wait for any running command first.</source>
        <translation>Bir hedef seçin ve önce çalışan herhangi bir komut bekleyin.</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic through the helper.</source>
        <translation>Run Teşhis - seçilen okuma-sadece tanıyı yardımcı aracılığıyla çalıştırın.</translation>
    </message>
    <message>
        <source>Run All for the current scope and refresh the read-only backend profile.</source>
        <translation>Mevcut kapsamı için her şeyi çalıştırın ve sadece arka uç profili tazeleyin.</translation>
    </message>
    <message>
        <source>Read or edit the selected target configuration file through the guarded helper; a saved edit invalidates cached diagnostics.</source>
        <translation>Korumalı yardımcı aracılığıyla seçilen hedef yapılandırma dosyasını okuyun veya düzenler; kurtarılmış bir düzenleme önbellekli tanılar.</translation>
    </message>
    <message>
        <source>Host Maintenance has no target-file editing; commit an offline repair target first.</source>
        <translation>Host Bakım&apos;nın hedefi yoktur; ilk önce çevrimdışı bir onarım hedefi uygulayın.</translation>
    </message>
    <message>
        <source>Commit an offline repair target first.</source>
        <translation>İlk önce çevrimdışı bir onarım hedefi.</translation>
    </message>
    <message>
        <source>No target configuration file is available for this target; run diagnostics to probe the list.</source>
        <translation>Bu hedef yapılandırma dosyası bu hedef için mevcut değildir; listeyi icat etmek için teşhis yapın.</translation>
    </message>
    <message>
        <source>Already Unlocked</source>
        <translation>Zaten Unlocked</translation>
    </message>
    <message>
        <source>An unlocked Linux filesystem is already visible on this drive. Boot Bitch will reuse the existing mapper and will not close or reopen a mapping created by this recovery session. Mounting happens during diagnostics (read-only) and repairs (read-write); data filesystems are never auto-mounted on selection.</source>
        <translation>Açıklanmış bir Linux dosya sistemi zaten bu sürücüde görünür. Boot Bitch mevcut haritayı yeniden kullanacaktır ve bu kurtarma seansı tarafından yaratılan bir haritayı kapatmayacaktır. Mounting tanı sırasında (yalnızca) ve onarımlar sırasında gerçekleşir (read-write); veri dosya sistemleri asla seçimde otomatik değildir.</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked; use Host Maintenance for the protected running host.</source>
        <translation>Korumalı koşu ev sahipliği açıklanamaz; korumalı çalışan ev sahibi için Host Bakım kullanın.</translation>
    </message>
    <message>
        <source>Host Maintenance is the current scope, but the selected offline drive can still be unlocked.</source>
        <translation>Host Bakım şu anki kapsamıdır, ancak seçilmiş çevrimdışı sürüş hala açıklanabilir.</translation>
    </message>
    <message>
        <source>Unlock %1 using cryptsetup through the privileged helper. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>Unlock %1, ayrıcalıklı yardımcı aracılığıyla şifreleme kullanıyor. Geçiş özel bir dezavantajla seyahat eder ve hiçbir zaman komut argümanlarına veya loglara yerleştirilir.</translation>
    </message>
    <message>
        <source>The protected running system cannot be selected as a repair target; use Host Maintenance for the protected running host.</source>
        <translation>Korumalı koşu sistemi bir onarım hedefi olarak seçilebilir; korumalı çalışan ev sahibi için Host Bakım kullanın.</translation>
    </message>
    <message>
        <source>Live / installer media is read-only boot media and cannot be selected as a repair target.</source>
        <translation>Live / installer medyası sadece boot medyası okunur ve onarım hedefi olarak seçilebilir.</translation>
    </message>
    <message>
        <source>Unlock the encrypted volume first; Select Target becomes available after a Linux filesystem is detected.</source>
        <translation>İlk önce şifreli hacmi açın; Hedefi bir Linux dosya sistemi tespit edildikten sonra kullanılabilir.</translation>
    </message>
    <message>
        <source>Committed repair target. Repair, Diagnostics and File Copy target this physical drive until another drive is explicitly selected with Select Target.</source>
        <translation>onarım hedefi. Onarım, Tanılar ve Dosya Kopyası, bu fiziksel sürüşü başka bir sürücüye açıkça seçilmiş olana kadar hedef alır.</translation>
    </message>
    <message>
        <source>Commit %1 as the repair target; this leaves Host Maintenance and switches the scope to the selected drive.</source>
        <translation>Komit %1 onarım hedefi olarak; Bu ev sahibi bakım bırakıyor ve seçilen sürücüye kapsamı kilitliyor.</translation>
    </message>
    <message>
        <source>The running host target could not be detected.</source>
        <translation>Koşu ev sahibi hedefi tespit edilemez.</translation>
    </message>
    <message>
        <source>Leave Host Maintenance and return to repair-target mode.</source>
        <translation>Host Bakım&apos;yı bırakın ve onarım-target modunda geri dönün.</translation>
    </message>
    <message>
        <source>Select the running host for deliberate guarded maintenance; the administrator authorization is requested here once and cached for the session.</source>
        <translation>Gizli bakım için çalışan ev sahibi seçin; yönetici izni burada bir kez talep edilir ve seans için önbelli.</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a repair target.</source>
        <translation>Koşu ev sahibi hedefi tespit edilemeyebilir; tanıların bir onarım hedefine ihtiyacı vardır.</translation>
    </message>
    <message>
        <source>Committed target: %1 + %2</source>
        <translation>Uygulamalı hedef: %1 + %2</translation>
    </message>
    <message>
        <source>Committed target: none (selection changed)</source>
        <translation>Müthiş hedef: Hiçbir şey (seleksiyon değişti)</translation>
    </message>
    <message>
        <source>The selected scope has no resolved Linux root component; Refresh Devices and commit the repair target again.</source>
        <translation>Seçilmiş kapsamın Linux kök bileşenini çözmedi; Yenileme cihazları ve onarım hedefi tekrar işliyor.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize on the Systems or Repair tab to re-establish it. Run All reuses the cached authorization and never prompts by itself.</source>
        <translation>Hiçbir yönetici seansı aktif değildir; Sistem veya Onarım sekmesi üzerinde yeniden kurmak için yazın. Önbellekli yetkiyi tüm yeniden çalıştırın ve asla kendi başına araymayın.</translation>
    </message>
    <message>
        <source>Run diagnostics for this scope to unlock the gated actions. Diagnostics are read-only and the only evidence source.</source>
        <translation>Kapılı eylemleri kilidini açmak için bu kapsamın teşhislerini çalıştırın. Tanılar okunur ve tek kanıt kaynağı.</translation>
    </message>
    <message>
        <source>A repair was not proven unchanged, so the cached diagnostics are invalidated. Run diagnostics again before another gated action.</source>
        <translation>Bir onarım kanıtlanmadı, bu yüzden önbellekli tanı geçersizdir. Başka bir kapılı eylemden önce tekrar teşhis yapın.</translation>
    </message>
    <message>
        <source>Gated actions reflect the cached capability lines; the helper still runs every runtime preflight when a command starts.</source>
        <translation>Kapılı eylemler önbellek kapasite hatları yansıtır; yardımcı hala bir komut başladığında her zaman ön ışık çalışır.</translation>
    </message>
    <message>
        <source>Regenerating diagnostics automatically</source>
        <translation>Regeneating tanıları otomatik olarak</translation>
    </message>
    <message>
        <source>Running diagnostic: %1</source>
        <translation>Kontrollü teşhis: %1</translation>
    </message>
    <message>
        <source>Running all diagnostics</source>
        <translation>Tüm tanıları çalıştırın</translation>
    </message>
    <message>
        <source>Unlocking %1</source>
        <translation>%1</translation>
    </message>
    <message>
        <source>running host</source>
        <translation>Koşu host</translation>
    </message>
    <message>
        <source>offline target</source>
        <translation>çevrimdışı hedef</translation>
    </message>
    <message>
        <source>host maintenance active</source>
        <translation>Aktif bakım aktif aktif</translation>
    </message>
    <message>
        <source>repair target committed</source>
        <translation>onarım hedefi kararlı</translation>
    </message>
    <message>
        <source>no committed scope</source>
        <translation>Hiçbir taahhüt kapsamı yok</translation>
    </message>
    <message>
        <source>Current session</source>
        <translation>Şimdiki seans</translation>
    </message>
    <message>
        <source>Log files (*.log);;All files (*)</source>
        <translation>Log dosyaları (*.log);; Tüm dosyalar (*)</translation>
    </message>
    <message>
        <source>Save log as</source>
        <translation>Save log as Kaydet</translation>
    </message>
    <message>
        <source>About Boot Bitch</source>
        <translation>Boot Bitch Hakkında</translation>
    </message>
    <message>
        <source>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;&lt;p&gt;&lt;b&gt;Developer:&lt;/b&gt; CaptainMorgan12&lt;/p&gt;&lt;p&gt;This GUI is the package&apos;s entry point: a Qt 3.3.x frontend with the ported guarded helper for Debian Etch-era systems.&lt;/p&gt;&lt;p&gt;&lt;b&gt;Guarded repair mode:&lt;/b&gt; read-only diagnostics can inspect either the protected Running Host or an explicitly selected repair drive. The guarded Debian/APT package stages (interrupted configuration, broken dependencies, metadata refresh, upgrade), GRUB-legacy configuration regeneration, LUKS target unlock and guarded target-file editing run through the ported helper after confirmation; Host Maintenance enables the same supported stages natively on the active system after repeating the host identity and boot-mount checks.&lt;/p&gt;&lt;p&gt;The modern-only features - verified File Copy, Btrfs snapshot rollback, EFI/UKI and extlinux repair, boot-stack reconciliation and Make Default - are greyed with the helper&apos;s own probe reasons on this frontend; Arch/Alpine/Fedora package backends stay diagnostics-only here.&lt;/p&gt;&lt;p&gt;The first privileged action authorizes one cached administrator session per scope through a hidden-input modal (the Qt GUI remains unprivileged). It can be ended at any time from File - Lock Administrator Session.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;&lt;p&gt;&lt;b&gt;De Developmenter:&lt;/b&gt; KaptanMorgan12&lt;/p&gt;&lt;p&gt;&lt;p&gt; Bu GUI paketin giriş noktası: bir Qt 3.3.x önend ile ported muhafız sistemi için yardımcı olan yardımcıdır.&lt;/p&gt;&lt;p&gt;&lt;b&gt;Guarded onarım modu:&lt;/b&gt; read-only tanılamalar, korumalı Run Host veya açıkça seçilmiş bir onarım sürüşünü inceleyebilir. Korumalı Leicester/APT paket aşamaları (önemli konfigürasyon, kırık bağımlılıklar, metadata yenilenme, yükseltme), GRUB-legacy konfigürasyonu yenileme, LUKS hedefi, limanlı yardım yoluyla hareket eden hedef ayarlandığında; Host Bakım, ev sahibi kimliğini ve önbellek kontrollerini tekrarlamadan sonra aynı şekilde aktif sisteme olanak sağlar.&lt;/p&gt;&lt;p&gt;&lt;p&gt;&lt;p&gt;&lt;p&gt; Modern-sadece özellikler - doğrulanmış Dosya Kopyası, Btrfs snapshot rollback, EFI /UKI ve extlinux onarımı, boot-stack uzlaşması ve Varsayılan olun - bu cephede yardımcı olan kendi araştırma nedenleri ile grileştirilir; Arch/Alpine/Fedora paketi geri döndü. &lt;/p&gt;&lt;p&gt; İlk ayrıcalıklı eylem, gizli bir dosya aracılığıyla bir önbellek yönetici seansı yazar ( Qt GUI eksik kalır). Dosyadan herhangi bir zamanda sona erebilir - Lock Manager Session.&lt;/p&gt;</translation>
    </message>
</context>
</TS>
