<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="fi">
<context>
    <name>CapabilityChecker</name>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="248"/>
        <source>Device discovery</source>
        <translation>Laitelöytö</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="248"/>
        <location filename="../src/CapabilityChecker.cpp" line="249"/>
        <location filename="../src/CapabilityChecker.cpp" line="250"/>
        <location filename="../src/CapabilityChecker.cpp" line="251"/>
        <location filename="../src/CapabilityChecker.cpp" line="252"/>
        <location filename="../src/CapabilityChecker.cpp" line="254"/>
        <location filename="../src/CapabilityChecker.cpp" line="256"/>
        <location filename="../src/CapabilityChecker.cpp" line="257"/>
        <location filename="../src/CapabilityChecker.cpp" line="269"/>
        <location filename="../src/CapabilityChecker.cpp" line="270"/>
        <source>Host</source>
        <translation>Palvelin</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="248"/>
        <source>Required for block-device inventory</source>
        <translation>Vaaditaan laitelohkon inventointia</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="249"/>
        <source>Filesystem identification</source>
        <translation>Tiedostojärjestelmän tunnistus</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="249"/>
        <source>Used to identify filesystem metadata</source>
        <translation>Käytetään tiedostojärjestelmän metatietojen tunnistamiseen</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="250"/>
        <source>Mount inspection</source>
        <translation>Asennuksen tarkastus</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="250"/>
        <source>Used to understand active mounts</source>
        <translation>Käytetään aktiivisten kiinnikkeiden ymmärtämiseen</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="251"/>
        <source>LUKS support</source>
        <translation>LUKS-tuki</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="251"/>
        <source>Required to unlock encrypted targets</source>
        <translation>Vaaditaan avaamaan salattuja kohteita</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="252"/>
        <source>Btrfs support</source>
        <translation>Btrfs-tuki</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="252"/>
        <source>Required for Btrfs inspection and snapshot rollback</source>
        <translation>Vaaditaan Btrfs-tarkastusta ja kuvien varalaskua varten</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="253"/>
        <source>Bidirectional file copy</source>
        <translation>Kaksisuuntainen tiedostokopio</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="253"/>
        <location filename="../src/CapabilityChecker.cpp" line="255"/>
        <source>Host/Repair</source>
        <translation>Host/Repair</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="253"/>
        <source>Required for verified Host-to-Repair and Repair-to-Host transfer</source>
        <translation>Vaaditaan todennettua Host-to-repair ja korjaus-to-ost siirto</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="254"/>
        <source>Chroot repair</source>
        <translation>Chroot korjaus</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="254"/>
        <source>Required for target-side repair commands</source>
        <translation>Vaaditaan maalin puolen korjauskomentoja varten</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="255"/>
        <source>Offline systemd repair</source>
        <translation>Offline järjestelmäkorjaus</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="255"/>
        <source>Used to restore graphical.target and the configured display manager without starting the target GUI</source>
        <translation>Sitä käytettiin graafisen restaurointiin. kohde ja konfiguroitu näytönhallinta aloittamatta kohdekäyttöliittymää</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="256"/>
        <source>UEFI NVRAM inspection</source>
        <translation>UEFI NVRAM -tarkastus</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="256"/>
        <source>Used to preserve target EFI BootOrder during TUXEDO UKI rebuilds when efivars are available</source>
        <translation>Käytetään säilyttämään kohde EFI Boot Order aikana TUXEDO UKI jälleenrakentaa kun efivars on saatavilla</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="257"/>
        <source>UKI verification</source>
        <translation>UKI-verifiointi</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="257"/>
        <source>Used to verify the kernel embedded in a rebuilt unified kernel image</source>
        <translation>Käytetään uudelleen rakennetussa ytimen kuvassa olevan ytimen todentamiseen</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="258"/>
        <source>GRUB EFI repair</source>
        <translation>GRUB EFI korjaus</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="258"/>
        <source>Target/Host</source>
        <translation>Tavoite/Host</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="258"/>
        <source>Required only for conventional GRUB-based EFI systems</source>
        <translation>Vaaditaan ainoastaan perinteisissä GRUB-pohjaisissa EFI-järjestelmissä</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="259"/>
        <location filename="../src/CapabilityChecker.cpp" line="260"/>
        <source>GRUB configuration</source>
        <translation>GRUB-konfiguraatio</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="259"/>
        <location filename="../src/CapabilityChecker.cpp" line="260"/>
        <location filename="../src/CapabilityChecker.cpp" line="261"/>
        <location filename="../src/CapabilityChecker.cpp" line="262"/>
        <location filename="../src/CapabilityChecker.cpp" line="263"/>
        <location filename="../src/CapabilityChecker.cpp" line="264"/>
        <location filename="../src/CapabilityChecker.cpp" line="265"/>
        <location filename="../src/CapabilityChecker.cpp" line="268"/>
        <source>Target</source>
        <translation>Kohde</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="259"/>
        <source>Debian-family GRUB helper</source>
        <translation>Debian-perhe GRUB -apulainen</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="260"/>
        <source>Portable GRUB configuration generator used by Arch and other non-Debian systems</source>
        <translation>Archin ja muiden kuin Debian-järjestelmien käyttämä siirrettävä GRUB-konfiguraatiogeneraattori</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="261"/>
        <location filename="../src/CapabilityChecker.cpp" line="262"/>
        <location filename="../src/CapabilityChecker.cpp" line="263"/>
        <source>Initramfs rebuild</source>
        <translation>Initramfsin jälleenrakentaminen</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="261"/>
        <source>Debian-family initramfs helper</source>
        <translation>Debian-perhe initramfs -apulainen</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="262"/>
        <source>Arch-family initramfs generator</source>
        <translation>Arkkiperheen initramfs generaattori</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="263"/>
        <source>Alternative initramfs generator used by Arch and other distributions</source>
        <translation>Archin ja muiden jakelujen käyttämä vaihtoehtoinen initramfs-generaattori</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="264"/>
        <location filename="../src/CapabilityChecker.cpp" line="265"/>
        <source>Initramfs verification</source>
        <translation>Initramfs-varmennus</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="264"/>
        <source>Read-only verification for mkinitcpio images</source>
        <translation>Vain mkinitcpio-kuvien tarkistus</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="265"/>
        <source>Read-only verification for dracut images</source>
        <translation>Lue vain dracut-kuvien tarkistus</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="266"/>
        <source>systemd-boot inspection</source>
        <translation>systemd-boot-tarkastus</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="266"/>
        <location filename="../src/CapabilityChecker.cpp" line="267"/>
        <source>Host/Target</source>
        <translation>Kohde</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="266"/>
        <source>Read-only inspection of systemd-boot and generic UKI layouts</source>
        <translation>systemd-boot:n ja yleisten UKI:n ulkoasujen uudelleentarkastelu</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="267"/>
        <source>Arch package manager</source>
        <translation>Arch-paketinhallinta</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="267"/>
        <source>Arch-family package database and transaction tool</source>
        <translation>Arch-perhe paketti tietokanta ja tapahtuma työkalu</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="268"/>
        <source>DKMS rebuild</source>
        <translation>DKMS:n jälleenrakentaminen</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="268"/>
        <source>Required only when target uses DKMS modules</source>
        <translation>Vaaditaan vain, kun kohde käyttää DKMS-moduuleja</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="269"/>
        <source>LVM inspection</source>
        <translation>LVM-tarkastus</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="269"/>
        <source>Optional LVM storage-stack support</source>
        <translation>Valinnainen LVM-varastotuki</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="270"/>
        <source>Software RAID</source>
        <translation>Ohjelmisto RAID</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="270"/>
        <source>Optional Linux MD RAID support</source>
        <translation>Valinnainen Linux MD RAID -tuki</translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <location filename="../src/MainWindow.cpp" line="666"/>
        <source>Environment validation</source>
        <translation>Ympäristön validointi</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="666"/>
        <source>Summarizes the selected system, protection state, mounted identity and inspection readiness.</source>
        <translation>Yhteenveto valitusta järjestelmästä, suojatilasta, asennetusta henkilöllisyydestä ja tarkastusvalmiudesta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="667"/>
        <source>Distribution and boot backend profile</source>
        <translation>Jakelu- ja käynnistystukiprofiili</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="667"/>
        <source>Identifies the distribution family, package manager, initramfs generator, bootloader, ESP location and current guarded repair capability.</source>
        <translation>Tunnistaa jakeluperheen, pakettipäällikkö, initramfs generaattori, bootloader, ESP sijainti ja nykyinen suojattu korjauskyky.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="668"/>
        <location filename="../src/MainWindow.cpp" line="12685"/>
        <location filename="../src/MainWindow.cpp" line="12704"/>
        <source>Boot diagnostics</source>
        <translation>Saapasdiagnostiikka</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="668"/>
        <source>Shows boot mounts, /boot and EFI contents plus storage evidence without changing the selected system.</source>
        <translation>Näyttää saappaat, /boot- ja EFI-sisällöt sekä varastointitodisteet muuttamatta valittua järjestelmää.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="669"/>
        <source>Boot evidence and selection history</source>
        <translation>Todisteet ja valintahistoria</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="669"/>
        <source>Correlates the detected boot chain, bootloader selection, kernel/initramfs, snapshots, EFI and unlock evidence, including whether one or more LUKS prompts are expected.</source>
        <translation>Korjaa havaittua käynnistysketjua, bootloader-valintaa, ydintä/initramfs:ää, valokuvia, EFI:ää ja avaa todisteita, mukaan lukien yksi tai useampi LUKS-kehotus.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="670"/>
        <source>Kernel / initramfs</source>
        <translation>Ytimen / initramfs</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="670"/>
        <source>Reviews kernel files and verifies matching initramfs images through a read-only inspection.</source>
        <translation>Tarkistaa ydintiedostoja ja todentaa täsmäävät initramfs-kuvat vain lukutarkastuksella.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="671"/>
        <location filename="../src/MainWindow.cpp" line="5924"/>
        <location filename="../src/MainWindow.cpp" line="12684"/>
        <location filename="../src/MainWindow.cpp" line="12703"/>
        <location filename="../src/MainWindow.cpp" line="17423"/>
        <source>GRUB configuration</source>
        <translation>GRUB-konfiguraatio</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="671"/>
        <source>Reviews GRUB configuration and /etc/default/grub without changing boot files.</source>
        <translation>Arvostelut GRUB asetukset ja /etc/default/grub muuttamatta käynnistystiedostoja.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="672"/>
        <source>EFI / UKI boot state</source>
        <translation>EFI / UKI- käynnistystila</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="672"/>
        <source>Inspects the selected ESP, vendor or generic UKI images, systemd-boot loader files, embedded kernel/cmdline data and firmware entries with PARTUUID ownership classification.</source>
        <translation>Tarkastaa valitut ESP-, myyjä- tai geneeriset UKI-kuvat, systemd-boot-kuormaajatiedostot, upotetut ytimen/cmdline-tiedot ja firmware-merkinnät, joissa on PAPUUID-omistusluokitus.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="673"/>
        <location filename="../src/MainWindow.cpp" line="5921"/>
        <location filename="../src/MainWindow.cpp" line="17360"/>
        <source>Graphical login / display manager</source>
        <translation>Graafinen kirjautuminen / näytönhallinta</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="673"/>
        <source>Reviews graphical.target, the configured display manager (for example SDDM, GDM/GDM3, LightDM, or another systemd manager), installed desktop packages, and recent boot/journal evidence without starting the GUI.</source>
        <translation>Arvostelut graafinen. kohde, konfiguroitu näyttö manager (esim. DSDM, GDM/GDM3, LightDM, tai muu järjestelmällinen manager), asennettu työpöytäpaketit, ja viimeaikaiset boot/journal näyttö käynnistämättä GUI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="674"/>
        <source>Boot errors</source>
        <translation>Saapasvirheet</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="674"/>
        <source>Reads recent error-priority entries from the running host or selected repair system&apos;s persistent journal when available.</source>
        <translation>Luee viimeisimmät virheprioriteetti merkinnät käynnissä isäntä tai valitun korjausjärjestelmän jatkuva päiväkirja, jos saatavilla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="675"/>
        <source>Disk usage</source>
        <translation>Levyn käyttö</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="675"/>
        <source>Summarizes filesystem capacity/free space for the running host or read-only repair target.</source>
        <translation>Yhdistää tiedostojärjestelmän kapasiteetti / vapaa tila käynnissä isäntä tai luku-vain korjaus kohde.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="676"/>
        <source>File systems</source>
        <translation>Tiedostojärjestelmät</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="676"/>
        <source>Runs the read-only file system check for the running host or selected repair system&apos;s root, /boot, ESP and /home filesystems and reports each device&apos;s check tool and result without changing anything.</source>
        <translation>Suorittaa luku-vain tiedostojärjestelmän tarkistus käynnissä isäntä tai valitun korjausjärjestelmän juuren, /boot, ESP ja /home tiedostojärjestelmät ja raportoi kunkin laitteen tarkistustyökalu ja tulos muuttamatta mitään.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="677"/>
        <source>fstab</source>
        <translation>fvab</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="677"/>
        <source>Displays the running host or selected repair system&apos;s fstab; repair-system inspection is mounted read-only.</source>
        <translation>Näyttää juoksuisännän tai valitun korjausjärjestelmän fstabin; korjausjärjestelmän tarkastus on asennettu vain luettavaksi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="678"/>
        <source>Btrfs status</source>
        <translation>Btrfs- tila</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="678"/>
        <source>Shows Btrfs filesystem and subvolume information for the running host or selected repair system.</source>
        <translation>Näyttää Btrfs tiedostojärjestelmän ja alivolume tiedot käynnissä isäntä tai valittu korjausjärjestelmä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="679"/>
        <source>Mapper status</source>
        <translation>Mapper- tila</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="679"/>
        <source>Shows selected mapper ancestry, device-mapper state and cryptsetup status when available.</source>
        <translation>Näyttää valitut mapper esihistoria, laite-mapper tila ja cryptsetup tila, kun saatavilla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="680"/>
        <source>LUKS / crypttab</source>
        <translation>LUKS / kryptab</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="680"/>
        <source>Shows LUKS/mapped ancestry plus crypttab and fstab mapper references.</source>
        <translation>Näyttää LUKS/mapped esihistorian sekä kryptaab- ja fstab mapper-viittaukset.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="681"/>
        <location filename="../src/MainWindow.cpp" line="13912"/>
        <source>Full diagnostic report</source>
        <translation>Täydellinen vianmääritysraportti</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="681"/>
        <source>Combines all read-only diagnostics for the selected scope in one privileged inspection session.</source>
        <translation>Yhdistää kaikki luku-vain diagnostiikat valittuun soveltamisalaan yhdessä etuoikeutetussa tarkastusistunnossa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1073"/>
        <source>Alpine uses OpenRC, mkinitfs and syslinux/extlinux; boot-stack reconciliation is not enabled (run the initramfs and extlinux stages separately).</source>
        <translation>Alpine käyttää OpenRC:tä, mkinitfs:ää ja syslinux:ää/extlinux:ää; boot-stack-sekvenssien täsmäytys ei ole käytössä (käytä initramfs- ja extlinux-vaiheita erikseen).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1074"/>
        <source>Alpine boot-stack reconciliation is not enabled (run the initramfs and GRUB stages separately).</source>
        <translation>Alpine-keppien täsmäytys ei ole käytössä ( suorita initramfs- ja GRUB-vaiheet erikseen).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1075"/>
        <source>Alpine DKMS preflight found no build tree for installed kernel %1; install the matching headers and retry.</source>
        <translation>Alpine DKMS ennen lentoa ei löytynyt rakennettua puuta asennetulle ytimelle %1; asentaa vastaavat otsikot ja yrittää uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1076"/>
        <source>Alpine EFI-stub host default selection is not implemented; use the Alpine EFI repair stage for entry reconciliation.</source>
        <translation>Alpine EFI-Stub isäntä oletusvalintoja ei ole toteutettu; käytä Alpine EFI korjausvaihe maahantulon täsmäytys.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1077"/>
        <source>Alpine syslinux-EFI boot detected (EFI/syslinux/syslinux.efi); Make Default is not implemented for this backend.</source>
        <translation>Alpine syslinux-EFI boot havaittu (EFI/syslinux/syslinux.efi); Make Oletus ei ole toteutettu tälle taustaosalle.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1078"/>
        <source>Alpine syslinux-EFI boot detected (EFI/syslinux/syslinux.efi); guarded repair is not implemented.</source>
        <translation>Alpine syslinux-EFI boot havaittu (EFI/syslinux/syslinux.efi); vartioitu korjaus ei ole toteutettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1079"/>
        <source>apk simulated no package changes and the package state is byte-identical.</source>
        <translation>apk simuloi ei pakettimuutoksia ja pakettitila on identtinen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1080"/>
        <source>APT package lists are byte-identical and no repository index was fetched.</source>
        <translation>APT-pakettiluettelot ovat identtisiä eikä arkistoindeksiä noudettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1081"/>
        <source>Standalone APK metadata refresh is not available on Alpine; use Upgrade installed packages for one guarded apk transaction.</source>
        <translation>Standalone APK-metatietojen päivitystä ei ole saatavilla Alpinessa; käytä Päivitys asennettuja paketteja yhteen suojattuun apk-tapahtumaan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1082"/>
        <source>Standalone APT metadata refresh is not available on Arch; use Upgrade installed packages for one full pacman transaction.</source>
        <translation>Standalone APT metadatan päivitystä ei ole saatavilla Archissa; käytä Päivitys asennettuja paketteja yhteen pacman-tapahtumaan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1083"/>
        <source>Arch boot-stack reconciliation requires available initramfs, GRUB and EFI repair prerequisites.</source>
        <translation>Arch boot-stack täsmäytys edellyttää saatavilla initramfs, GRUB ja EFI korjaus edellytykset.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1084"/>
        <source>Arch DKMS preflight found no build tree for installed kernel %1; install the matching headers and retry.</source>
        <translation>Arch DKMS ennen lentoa ei löytynyt rakennettua puuta asennetulle ytimelle %1; asenna vastaavat otsikot ja yritä uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1085"/>
        <location filename="../src/MainWindow.cpp" line="1086"/>
        <source>Backends: %1</source>
        <translation>Taustaosat: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1087"/>
        <source>BLS is not enabled; the default is a generated menuentry.</source>
        <translation>BLS ei ole käytössä; oletus on luotu valikko.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1088"/>
        <source>The initramfs backend is booster, which the current repair implementation does not handle.</source>
        <translation>initramfs-taustaosa on tehoste, jota nykyinen korjaus ei käsittele.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1089"/>
        <source>initramfs, EFI/UKI and GRUB artifacts are byte-identical.</source>
        <translation>initramfs, EFI/UKI ja GRUB artefaktit ovat identtisiä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1090"/>
        <source>initramfs and GRUB2 artifacts are byte-identical.</source>
        <translation>initramfs- ja GRUB2-esineet ovat identtisiä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1091"/>
        <source>Requires available initramfs and GRUB repair prerequisites.</source>
        <translation>Edellyttää saatavilla initramfs ja GRUB korjaus edellytykset.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1092"/>
        <source>Embedded cmdline does not reference the live LUKS UUID %1.</source>
        <translation>Upotettu cmdline ei viittaa live LUKS UUID %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1093"/>
        <source>Embedded cmdline does not reference the live root UUID %1.</source>
        <translation>Upotettu cmdline ei viittaa elävää juuria UUID %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1094"/>
        <source>Embedded cmdline does not select the live Btrfs subvolume /%1.</source>
        <translation>Upotettu cmdline ei valitse suoraa Btrfs-subvolyymia /%1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1095"/>
        <source>default.target and display-manager.service were already correct.</source>
        <translation>oletus.kohde ja näyttö-hallinta.palvelu oli jo oikein.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1096"/>
        <source>DKMS preflight found no build tree for installed kernel %1; install the matching kernel-devel packages and retry.</source>
        <translation>DKMS:n esilentoa varten ei löytynyt asennettua %1-ydintä varten rakennettua puuta; asenna vastaavat ytimen devel-paketit ja yritä uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1097"/>
        <source>dnf4 is not supported by the guarded rpm backend.</source>
        <translation>dnf4 ei saa tukea vartioidusta rpm-taustasta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1098"/>
        <source>No missing package files were detected; dnf5 check is reported as evidence.</source>
        <translation>Puuttuvia pakettitiedostoja ei havaittu; dnf5-tarkistus ilmoitetaan todisteena.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1099"/>
        <source>The dnf5 repository metadata cache is byte-identical.</source>
        <translation>Dnf5-tiedoston metatietovälimuisti on tavullinen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1100"/>
        <source>dnf5 simulated no package changes and the rpm database is byte-identical.</source>
        <translation>dnf5 simuloidaan ei pakettimuutoksia ja rpm-tietokanta on identtinen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1101"/>
        <source>dpkg reported no packages pending configuration.</source>
        <translation>dpkg ei raportoinut paketeista, jotka olisivat odottaneet kokoonpanoa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1102"/>
        <source>Alpine uses apk; dpkg configuration is not available on Alpine.</source>
        <translation>Alpine käyttää apk:ää; dpkg:n konfigurointia ei ole saatavilla Alpinella.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1103"/>
        <source>dpkg configuration is not available on Arch; use the Arch package transaction stages instead.</source>
        <translation>dpkg-konfiguraatiota ei ole saatavilla Archissa, vaan käytä Arch-paketin vaiheita.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1104"/>
        <source>Fedora uses rpm/dnf; dpkg configuration is not available.</source>
        <translation>Fedora käyttää rpm/dnf:ää; dpkg:n konfiguraatiota ei ole saatavilla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1105"/>
        <source>EFI boot artifacts and firmware entries are byte-identical.</source>
        <translation>EFI boot-esineet ja firmware-merkinnät ovat yksilöllisiä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1106"/>
        <source>EFI-stub entry repair requires efibootmgr in the recovery host.</source>
        <translation>EFI-sivun sisääntulo korjaus vaatii efibootmgr elvytys isäntä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1107"/>
        <source>EFI-stub boot requires UEFI firmware; the recovery host booted in legacy BIOS mode.</source>
        <translation>EFI-stab boot vaatii UEFI firmware; palautus isäntä käynnistyi vanhassa BIOS-tilassa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1108"/>
        <source>extlinux.conf is byte-identical.</source>
        <translation>extlinux.conf on identtinen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1109"/>
        <source>The detected extlinux configuration %1 is not /boot/extlinux.conf.</source>
        <translation>Havaittu extlinux-konfiguraatio %1 ei ole /boot/extlinux.conf.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1110"/>
        <source>extlinux default label %1 is already selected.</source>
        <translation>extlinux-oletusmerkki %1 on jo valittu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1111"/>
        <source>Extlinux default selection is only available for the running host.</source>
        <translation>Extlinux-oletusvalinta on käytettävissä vain käynnissä olevalle isäntälle.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1112"/>
        <source>extlinux default label set to %1.</source>
        <translation>extlinux-oletusetiketti asetettu %1:ään.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1113"/>
        <source>update-extlinux generated a byte-identical configuration.</source>
        <translation>update-extlinux loi tavu-identtisen kokoonpanon.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1114"/>
        <source>MBR, BIOS boot partition and i386-pc modules are byte-identical.</source>
        <translation>MBR, BIOS boot osio ja i386-PC moduulit ovat byte-identtinen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1115"/>
        <source>grubenv saved_entry already names the running kernel BLS entry %1.</source>
        <translation>grubenv tallennettu entry nimeää jo käynnissä olevan ytimen BLS merkintä %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1116"/>
        <source>grubenv saved_entry set to %1 (all other keys preserved).</source>
        <translation>grubenv tallennettu entry asetettu %1 (kaikki muut avaimet säilytetään).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1117"/>
        <source>The read-only check reported no errors.</source>
        <translation>Vain lukutarkastus ei ilmoittanut virheitä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1118"/>
        <source>The simulated fix-broken transaction proposed no package changes.</source>
        <translation>Simuloitu fix-breaked tapahtuma ehdotti mitään pakettimuutoksia.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1119"/>
        <source>GRUB detected on legacy BIOS; no EFI boot path is available.</source>
        <translation>GRUB havaittu perinteisillä BIOS; EFI boot polku ei ole saatavilla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1120"/>
        <source>grub.cfg and grubenv are byte-identical.</source>
        <translation>grub.cfg ja grubenv ovat tunnistettavia.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1121"/>
        <source>grub.cfg is byte-identical.</source>
        <translation>grub.cfg on identtinen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1122"/>
        <source>GRUB_DEFAULT is not set to saved; grubenv does not select the default entry.</source>
        <translation>GRUB DEFAULT ei ole tallennettu; grubenv ei valitse oletusmerkintää.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1123"/>
        <source>BootOrder, labels and registrations already correct.</source>
        <translation>Saapasjärjestys, etiketit ja rekisteröinnit ovat jo oikein.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1124"/>
        <source>The running host EFI System Partition could not be resolved.</source>
        <translation>Juoksevaa isäntä EFI System Partitionia ei voitu ratkaista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1125"/>
        <source>The target dpkg database or executable is incomplete.</source>
        <translation>Kohde dpkg-tietokanta tai suoritustiedosto on puutteellinen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1126"/>
        <source>Rebuilt initramfs images are byte-identical.</source>
        <translation>Uudelleen rakennetut initramfs-kuvat ovat identtisiä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1127"/>
        <source>Less than 1 GiB of free Btrfs space is available.</source>
        <translation>Alle 1 GiB vapaata Btrfs tilaa on saatavilla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1128"/>
        <source>The grubenv file is missing or not a valid GRUB environment block.</source>
        <translation>Grubenv-tiedosto puuttuu tai ei kelpaa GRUB-ympäristöblokiksi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1129"/>
        <source>legacy BIOS target; no EFI boot path is available.</source>
        <translation>perinteisiä BIOS-kohteita; EFI-polkua ei ole saatavilla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1130"/>
        <source>apk is not installed in the target.</source>
        <translation>apk:ää ei ole asennettu kohteeseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1131"/>
        <source>The target apk installed database is missing.</source>
        <translation>Kohteen apk asennettu tietokanta puuttuu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1132"/>
        <source>The target apk world file is missing.</source>
        <translation>Kohde apk maailman tiedosto puuttuu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1133"/>
        <source>apt-get is not installed in the target.</source>
        <translation>apt-get ei ole asennettu kohteeseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1134"/>
        <source>btrfs-progs is not installed.</source>
        <translation>btrfs-progia ei ole asennettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1135"/>
        <source>DKMS is not installed in the target.</source>
        <translation>DKMS-järjestelmää ei ole asennettu kohteeseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1136"/>
        <source>DKMS is not installed in the Alpine target system.</source>
        <translation>DKMS-järjestelmää ei ole asennettu Alppien kohdejärjestelmään.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1137"/>
        <source>DKMS is not installed in the Arch target system.</source>
        <translation>DKMS:ää ei ole asennettu Arch-kohdejärjestelmään.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1138"/>
        <source>dnf5 is not installed in the target system.</source>
        <translation>dnnf5 ei ole asennettu kohdejärjestelmään.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1139"/>
        <source>dpkg is not installed in the target.</source>
        <translation>dpkg:ää ei ole asennettu kohteeseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1140"/>
        <source>dracut is not installed in the target system.</source>
        <translation>dracut ei ole asennettu kohdejärjestelmään.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1141"/>
        <source>The dracut generator directory is missing from the target.</source>
        <translation>dracut-generaattorin hakemisto puuttuu kohteesta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1142"/>
        <source>efibootmgr is not installed; the running host default EFI entry cannot be changed.</source>
        <translation>efibootmgr ei ole asennettu; käynnissä olevaa isäntää EFI-merkintää ei voi muuttaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1143"/>
        <source>graphical.target is missing from the target.</source>
        <translation>graafinen kohde puuttuu kohteesta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1144"/>
        <source>grub2-editenv is not installed in the target.</source>
        <translation>grub2-editenv ei ole asennettu kohde.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1145"/>
        <source>grub2-install is not installed in the target.</source>
        <translation>grub2-install ei ole asennettu kohteeseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1146"/>
        <source>grub2-mkconfig is not installed in the target.</source>
        <translation>grub2-mkconfig ei ole asennettu kohteeseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1147"/>
        <source>%1 is missing.</source>
        <translation>%1 puuttuu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1148"/>
        <source>Requires GRUB configuration tooling.</source>
        <translation>Vaaditaan GRUB-asetustyökalu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1149"/>
        <source>grub-efi is not installed in the Alpine target.</source>
        <translation>Grub-efiä ei ole asennettu Alppien kohteeseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1150"/>
        <source>The x86_64-efi GRUB module directory is missing from the Alpine target.</source>
        <translation>Alppien kohteesta puuttuu x86 64-efi GRUB -moduulihakemisto.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1151"/>
        <source>Neither grub-mkconfig nor update-grub is installed in the target system.</source>
        <translation>Kohdejärjestelmään ei ole asennettu grub-mkconfig:ää eikä update-grub:ää.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1152"/>
        <source>grub-install missing.</source>
        <translation>grub-install puuttuu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1153"/>
        <source>grub-install is not installed in the Alpine target.</source>
        <translation>grub-install:ää ei ole asennettu Alppien kohteeseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1154"/>
        <source>The grub package is not installed in the Alpine target.</source>
        <translation>Ravintolaa ei ole asennettu Alppien kohteeseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1155"/>
        <source>The GRUB ZFS module (zfs.mod) is not installed in the target.</source>
        <translation>GRUB ZFS -moduulia (zfs.mod) ei ole asennettu kohteeseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1156"/>
        <source>update-initramfs/mkinitramfs are not installed in the target.</source>
        <translation>update-initramfs/mkinitrafeja ei ole asennettu kohteeseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1157"/>
        <source>lsinitrd is not installed in the target system; dracut image verification is unavailable.</source>
        <translation>Lsinitrd ei ole asennettu kohdejärjestelmään; dracut-kuvan tarkistus ei ole käytettävissä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1158"/>
        <source>mkinitcpio is not installed in the target system.</source>
        <translation>mkinitcpio ei ole asennettu kohdejärjestelmään.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1159"/>
        <source>mkinitfs is not installed in the target system.</source>
        <translation>mkinitfs ei ole asennettu kohdejärjestelmään.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1160"/>
        <source>The target has no mkinitfs configuration.</source>
        <translation>Kohteessa ei ole mkinitfs-konfiguraatiota.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1161"/>
        <source>objcopy is unavailable; TUX.EFI embedded sections cannot be verified.</source>
        <translation>objkopiaa ei ole saatavilla; TUX.EFI sulautettuja osia ei voida todentaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1162"/>
        <source>pacman is not installed in the target.</source>
        <translation>pacman:ää ei ole asennettu kohteeseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1163"/>
        <source>The target has no /etc/pacman.conf; refusing a package transaction.</source>
        <translation>Kohde ei ole /etc/pacman.conf; kieltäytyy pakettitapahtuma.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1164"/>
        <source>The target pacman database directory is missing.</source>
        <translation>Kohteen pacman tietokantahakemisto puuttuu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1165"/>
        <source>rpm is not installed in the target system.</source>
        <translation>rpm ei ole asennettu kohdejärjestelmään.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1166"/>
        <source>The target RPM database is missing.</source>
        <translation>Kohdetta ei ole.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1167"/>
        <source>snapper is not installed.</source>
        <translation>Nappulaa ei ole asennettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1168"/>
        <source>The syslinux package is not installed according to the detected package manager.</source>
        <translation>syslinux-pakettia ei ole asennettu havaitun paketinhallinnan mukaisesti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1169"/>
        <source>TUXEDO UKI builder create_boot_uki_base.sh is not installed in the target.</source>
        <translation>TUXEDO UKI builder create boot uki base.sh ei ole asennettu kohteeseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1170"/>
        <source>update-extlinux is not installed in the target.</source>
        <translation>update-extlinux ei ole asennettu kohde.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1171"/>
        <source>The running host has no /etc/update-extlinux.conf.</source>
        <translation>Juoksevalla isäntällä ei ole /etc/update-extlinux.conf.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1172"/>
        <source>update-extlinux is not installed in the running host.</source>
        <translation>update-extlinux ei ole asennettu käynnissä isäntä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1173"/>
        <source>The zfs-initramfs hook is not installed in the target (no /usr/share/initramfs-tools/hooks/zfs).</source>
        <translation>Zfs-initramf-koukkua ei ole asennettu kohteeseen (ei /usr/share/initramfs-tools/hooks/zfs).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1174"/>
        <source>Multiple extlinux entries reference the running kernel %1 (%2).</source>
        <translation>Useissa extlinux-kirjoituksissa viitataan juoksevaan ytimeen %1 (%2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1175"/>
        <source>The running @ root contains nested subvolumes that this release does not migrate: %1.</source>
        <translation>Juokseva @ root sisältää pesittyjä alivolumeja, että tämä julkaisu ei siirry: %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1176"/>
        <source>No EFI boot path was detected for the Alpine target (bootloader backend %1).</source>
        <translation>Alpine-kohteelle (Bootloader backend %1) ei havaittu EFI- käynnistyspolkua.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1177"/>
        <source>No installed Alpine kernels were found under target /boot.</source>
        <translation>Alppien siemeniä ei löytynyt kohteesta / saappaasta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1178"/>
        <source>The target has no configured apk repositories.</source>
        <translation>Kohteessa ei ole konfiguroituja apk-arkistoja.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1179"/>
        <source>The target has no configured APT sources to refresh.</source>
        <translation>Kohteella ei ole konfiguroituja APT-lähteitä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1180"/>
        <source>No EFI System Partition was identified for the selected Arch target.</source>
        <translation>Valitulle Arch-kohteelle ei tunnistettu EFI System Partitionia.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1181"/>
        <source>No non-rescue BLS entries are installed.</source>
        <translation>Muita kuin pelastus-BLS-tiedostoja ei ole asennettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1182"/>
        <source>No canonical EFI vendor loader was found on the running host ESP and grub-install is not available.</source>
        <translation>Suoritusisännästä ESP ei löytynyt kanonista EFI-toimittajakuormaajaa eikä grub-install:ää ole saatavilla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1183"/>
        <source>No supported display manager was detected in the target.</source>
        <translation>Kohdeessa ei havaittu tuettua näytönhallintaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1184"/>
        <source>No supported display manager unit is installed in the target.</source>
        <translation>Kohdeeseen ei ole asennettu tuettua näytönhallintayksikköä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1185"/>
        <source>The target has no enabled dnf repositories.</source>
        <translation>Kohde ei ole käytössä dnf-arkistoissa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1186"/>
        <source>The target has no enabled dnf repositories to refresh.</source>
        <translation>Kohde ei ole mahdollistanut dnf-arkistojen päivittämistä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1187"/>
        <source>No installed dracut kernels were found under target /boot.</source>
        <translation>Kohteen / saappaan alle ei löytynyt asennettuja dracut-ytimiä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1188"/>
        <source>No EFI-stub initramfs image (initramfs-*) is present at the EFI System Partition root.</source>
        <translation>EFI System Partition - juurilla ei ole EFI-stab initramfs-kuvaa (initramfs-*).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1189"/>
        <source>No EFI-stub kernel image (vmlinuz-*) is present at the EFI System Partition root.</source>
        <translation>EFI System Partition - juurilla ei ole EFI-ydinkuvaa (vmlinuz-*).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1190"/>
        <source>No EFI System Partition candidate on the selected disk.</source>
        <translation>Valitulla levyllä ei ole EFI System Partition -ehdokasta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1191"/>
        <source>No EFI System Partition is present or derivable on the selected disk.</source>
        <translation>EFI System Partition ei ole läsnä tai johdettu valitulla levyllä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1192"/>
        <source>No extlinux/syslinux configuration was detected in the target.</source>
        <translation>Kohteessa ei havaittu extlinux/syslinux-konfiguraatiota.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1193"/>
        <source>No extlinux/syslinux configuration was detected in the running host.</source>
        <translation>Suorituspalvelimessa ei havaittu extlinux/syslinux-konfiguraatiota.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1194"/>
        <source>The detected GRUB layout is not a grub2 layout.</source>
        <translation>Havaittu GRUB asettelu ei ole grub2 asettelu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1195"/>
        <source>No supported file system check tool is installed in the recovery environment.</source>
        <translation>Hyödyntämisympäristöön ei ole asennettu tuettua tiedostojärjestelmän tarkistustyökalua.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1196"/>
        <source>No supported initramfs backend (mkinitfs, mkinitcpio, dracut or initramfs-tools) was detected.</source>
        <translation>Tuettua initramfs-taustalaitetta (mkinitfs, mkinitcpio, dracut tai initramfs-tools) ei havaittu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1197"/>
        <source>Unable to determine the live root filesystem UUID.</source>
        <translation>Ei voitu määrittää elävää juuritiedostojärjestelmää UUID.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1198"/>
        <source>No installed kernel module directories were found for mkinitcpio.</source>
        <translation>mkinitcpio:lle ei löytynyt asennettuja ydinmoduulihakemistoja.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1199"/>
        <source>No supported OpenRC display manager service is installed in the target.</source>
        <translation>Kohdeeseen ei ole asennettu tuettua OpenRC-näytönhallintapalvelua.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1200"/>
        <source>No guarded package-manager backend was detected (detected: %1).</source>
        <translation>Suojattua pakettihallintataustaa ei havaittu ( Havaittu: %1).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1201"/>
        <source>No detected package manager can verify the syslinux package.</source>
        <translation>Mikään havaittu pakettien hallinta ei voi varmistaa syslinux-pakettia.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1202"/>
        <source>No supported reboot mechanism was found on the running host.</source>
        <translation>Käynnistävästä isäntästä ei löytynyt tuettua uudelleenkäynnistysmekanismia.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1203"/>
        <source>The selected scope&apos;s root, /boot, ESP and /home filesystems could not be resolved.</source>
        <translation>Valitun soveltamisalan juuria, /boot-, ESP- ja /home-tiedostojärjestelmiä ei voitu ratkaista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1204"/>
        <source>No supported service manager (systemd or OpenRC) was detected in the target.</source>
        <translation>Kohdeessa ei havaittu tuettua palvelupäällikköä (järjestelmällinen tai OpenRC).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1205"/>
        <source>No Snapper root configuration manages / with FSTYPE=btrfs.</source>
        <translation>Ei Snapper root asetukset hallita / FSTYPE=btrfs.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1206"/>
        <source>The selected scope has no supported file system type for a read-only check.</source>
        <translation>Valitulla soveltamisalalla ei ole tuettua tiedostojärjestelmän tyyppiä vain lukutarkistusta varten.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1207"/>
        <source>The running host root filesystem is %1, not Btrfs.</source>
        <translation>Juokseva isäntä juuritiedostojärjestelmä on %1, ei Btrfs.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1208"/>
        <source>The detected bootloader is %1; GRUB is not the selected bootloader.</source>
        <translation>Havaittu bootloader on %1; GRUB ei ole valittu bootloader.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1209"/>
        <source>The running root subvolume is %1, not the top-level @.</source>
        <translation>Juokseva juuriosamäärä on %1, ei huipputason @.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1210"/>
        <source>Unable to derive a running host UKI candidate.</source>
        <translation>Suoritettavaa UKI- ehdokasta ei voitu saada.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1211"/>
        <source>The OpenRC default runlevel already enabled %1.</source>
        <translation>OpenRC- oletusjuoksutaso on jo käytössä %1:ssä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1212"/>
        <source>A package manager or package-manager lock is active.</source>
        <translation>Paketinhallinta tai pakettihallinta lukko on aktiivinen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1213"/>
        <source>The pacman transaction reported no packages to install, upgrade or remove.</source>
        <translation>pacman-tapahtumassa ei ilmoitettu mitään asennus-, päivitys- tai poistopaketteja.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1214"/>
        <source>The running host pins subvolid= in fstab or the kernel command line.</source>
        <translation>Juokseva isäntä nastat subvolid = fstab tai ytimen komentorivi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1215"/>
        <source>The running host root filesystem is read-only.</source>
        <translation>Juokseva isäntä juuritiedostojärjestelmä on vain luettava.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1216"/>
        <source>The rpm DKMS header correction (kernel-devel/akmods) is not implemented.</source>
        <translation>DKKMS-otsikkokorjausta (kernel-devel/akmods) ei toteuteta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1217"/>
        <source>The running kernel %1 has no installed BLS entry.</source>
        <translation>Juoksevassa ytimessä %1 ei ole asennettua BLS-merkintää.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1218"/>
        <source>The running kernel %1 has no entry in %2.</source>
        <translation>Juoksevan ytimen %1 ei ole merkintä %2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1219"/>
        <source>Secure Boot requires a signed loader.</source>
        <translation>Secure Boot vaatii allekirjoitetun kuormaajan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1220"/>
        <source>The running host has a separate /boot filesystem outside the root snapshot.</source>
        <translation>Juoksevalla isäntällä on erillinen /boot-tiedostojärjestelmä juurikuvan ulkopuolella.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1221"/>
        <source>Another snapper command is running.</source>
        <translation>Toinen snapper-komento on käynnissä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1222"/>
        <source>syslinux/extlinux (BIOS) boot detected; no EFI boot path is available.</source>
        <translation>syslinux/extlinux (BIOS) -käynnistys havaittu; EFI- käynnistyspolkua ei ole saatavilla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1223"/>
        <source>A sysvinit display-manager script was detected; offline repair is not implemented for sysvinit.</source>
        <translation>Sysvinit näyttö-manager script havaittiin; offline korjausta ei toteuteta sysvinit.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1224"/>
        <source>A Timeshift btrfs snapshot inventory is present; Snapper @ rollback is not supported.</source>
        <translation>Timeshift btrfs -kuvaston inventaario on läsnä; Snapper @ rollback ei ole tuettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1225"/>
        <source>UEFI variables are not writable; the running host default EFI entry cannot be changed.</source>
        <translation>UEFI-muuttujat eivät ole kirjoitettavia; käynnissä olevaa isäntää oletus EFI ei voi muuttaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1226"/>
        <source>%1 is missing or empty.</source>
        <translation>%1 puuttuu tai on tyhjä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1227"/>
        <source>Embedded kernel %1 is not installed under /boot.</source>
        <translation>Upotettu ydin %1 ei ole asennettu /boot.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1228"/>
        <source>TUX.EFI does not embed a readable .cmdline section.</source>
        <translation>TUX.EFI ei upota luettavaa .cmdline-osaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1229"/>
        <source>TUX.EFI does not embed a kernel version (.uname).</source>
        <translation>TUX.EFI ei upota ytimen versiota (.Uname).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1230"/>
        <source>TUX.EFI is not a readable PE/COFF image (objcopy could not read .uname).</source>
        <translation>TUX.EFI ei ole luettavissa oleva PE/COFF-kuva (objcopy ei voi lukea .uname).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1231"/>
        <source>Unable to create a temporary file for UKI section inspection.</source>
        <translation>Väliaikaista tiedostoa UKI-osiotarkastukseen ei voitu luoda.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1232"/>
        <source>Backend %1 has no guarded implementation for stage %2.</source>
        <translation>Backend %1 ei ole vartioitu toteutus vaiheessa %2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1233"/>
        <source>The running kernel release could not be determined.</source>
        <translation>Juoksevaa ytimen vapautumista ei voitu määrittää.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1234"/>
        <source>The running host ZFS root pool is not resolvable (zpool is not installed or the pool is not imported).</source>
        <translation>Juokseva isäntä ZFS root allas ei ole resolved (zpool ei ole asennettu tai allasta ei tuoda).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1235"/>
        <source>The extlinux label %1 cannot be written to /etc/update-extlinux.conf safely.</source>
        <translation>extlinux-etikettiä %1 ei voi kirjoittaa /etc/update-extlinux.conf:lle turvallisesti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1236"/>
        <source>The simulated upgrade transaction proposed no package changes.</source>
        <translation>Simuloitu päivitystapahtuma ei ehdottanut pakettimuutoksia.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1237"/>
        <source>Validation is read-only.</source>
        <translation>Validointi on vain luettavaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1270"/>
        <source>Backed up Alpine EFI loader files to %1 (firmware fallback present before repair: %2).</source>
        <translation>Varmistettu Alpine EFI kuormaaja tiedostoja %1 (firmware varalla läsnä ennen korjausta: %2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1271"/>
        <source>PASS: refreshed the firmware fallback loader EFI/boot/bootx64.efi from %1.</source>
        <translation>PASS: Päivitetty firmware varakuormaaja EFI/boot/bootx64.efi %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1272"/>
        <source>PASS: Alpine EFI loader files verified under %1/EFI/%2</source>
        <translation>PASS: Alpine EFI kuormaaja tiedostot todennettu %1/EFI/%2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1273"/>
        <source>Restored the Alpine EFI loader files from the session backup %1.</source>
        <translation>Palautti Alpine EFI lataaja tiedostot istunnon varmuuskopio %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1274"/>
        <source>Detected %1 EFI-stub firmware entry(ies) on the selected ESP (%2); reconciling firmware state without file synthesis.</source>
        <translation>Havaittiin %1 EFI-stib firmware -merkintä [t] valitussa ESP:ssä (%2); firmware-tilan yhteensovittaminen ilman tiedostosynteesiä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1275"/>
        <source>PASS: Alpine EFI-stub firmware entries verified; no ESP files were changed.</source>
        <translation>PASS: Alpine EFI-stib firmware-tiedostot todennettu; ESP-tiedostoja ei muutettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1276"/>
        <source>Captured complete firmware entry state before Alpine GRUB EFI install: %1</source>
        <translation>Captured täydellinen firmware sisääntulotila ennen Alpine GRUB EFI asentaa: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1277"/>
        <source>Writable UEFI efivars/efibootmgr are unavailable; Alpine GRUB EFI repair will update loader files and the fallback copy only (read-only firmware-variable mode).</source>
        <translation>Kirjoitettava UEFI efivars / efibootmgr eivät ole saatavilla; Alpine GRUB EFI korjaus päivittää lataajan tiedostoja ja varakopioida vain (lukea vain firmware-variable mode).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1278"/>
        <source>SIMULATE/PREFLIGHT: Alpine GRUB EFI install target=%1 fs=%2 id=%3 mode=--no-nvram (helper-managed firmware entries)</source>
        <translation>SIMULATE/PREFLIGHT: Alpine GRUB EFI asenna kohde=%1 fs=%2 id=%3 mode=--no-nvram (auttajaohjatut laiteohjelmistot)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1279"/>
        <source>PASS: Alpine initramfs rebuilt and verified for %1</source>
        <translation>PASS: Alpine initramfs uudelleenrakennettu ja tarkistettu %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1280"/>
        <source>Alpine missing-file repair: reinstalling %1 package(s) with missing files: %2</source>
        <translation>Alpine puuttuvat-tiedosto korjaus: asentaminen %1 paketti(s) puuttuvat tiedostot: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1281"/>
        <source>Alpine missing-file detection: no missing package files; running the dependency-only apk fix transaction.</source>
        <translation>Alppien puuttuvan tiedoston havaitseminen: ei puuttuvia pakettitiedostoja; pelkästään riippuvuuden apk-korjaustapahtuman suorittaminen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1282"/>
        <source>Target %1 is already mounted at %2 (pre-existing source=%3 options=%4 id=%5); leaving it untouched.</source>
        <translation>Kohde %1 on jo asennettu %2 (olemassa oleva lähde=%3 vaihtoehtoja=%4 id=%5); jättäen sen koskemattomana.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1283"/>
        <source>Alpine apk preflight: %1</source>
        <translation>Alpine apk ennen lentoa: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1284"/>
        <source>REFUSED: apk simulation proposes %1 package changes (safety limit: %2).</source>
        <translation>EPÄÄMINEN: apk-simulaatio ehdottaa %1-paketin muutoksia (turvallisuusraja: %2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1285"/>
        <source>REFUSED: apk simulation reported unresolved or conflicting packages.</source>
        <translation>EPÄÄMINEN: apk-simulaatio raportoi ratkaisemattomista tai ristiriitaisista paketeista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1286"/>
        <source>REFUSED: apk simulation proposes a package downgrade.</source>
        <translation>EPÄÄMINEN: apk-simulaatiossa ehdotetaan paketin vähentämistä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1287"/>
        <source>REFUSED: apk simulation reported a locked package database.</source>
        <translation>EPÄÄMINEN: apk-simulaatio ilmoitti lukitun pakettitietokannan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1288"/>
        <source>REFUSED: apk simulation reported incomplete repository metadata.</source>
        <translation>EPÄÄMINEN: apk-simulaation mukaan tietovaraston metatiedot ovat puutteellisia.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1289"/>
        <source>REFUSED: apk simulation would remove &apos;%1&apos; without replacing it in the same transaction.</source>
        <translation>EPÄÄMINEN: apk-simulaatio poistaisi &quot;%1&quot; korvaamatta sitä samassa liiketoimessa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1290"/>
        <source>REFUSED: apk simulation reported an untrusted package signature.</source>
        <translation>EPÄÄMINEN: apk-simulaatio ilmoitti luottamattomasta paketin allekirjoituksesta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1291"/>
        <source>REFUSED: apk simulation reported an error Boot Bitch does not recognize as safe.</source>
        <translation>EPÄÄMINEN: apk simulointi ilmoitti virhe Boot Bitch ei tunnista turvallista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1292"/>
        <source>apk simulation replaces package &apos;%1&apos; in the same transaction; removal accepted.</source>
        <translation>apk-simulaatio korvaa paketin &apos;%1&apos; samassa tapahtumassa; poisto hyväksytty.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1293"/>
        <source>KNOWN ISSUE: APT correction is blocked by broken dependencies; simulating --fix-broken before retry.</source>
        <translation>TIEDON NUMERO: APT-korjausta estävät rikkoutuneet riippuvuudet; simulointi -- fix-breaked ennen uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1294"/>
        <source>KNOWN ISSUE: APT correction is blocked by interrupted dpkg configuration; completing dpkg once and re-simulating.</source>
        <translation>TIEDON NUMERO: APT-korjaus on estetty dpkg:n keskeytyksellä, dpkg:n valmistumisella kerran ja simulaatiolla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1295"/>
        <source>full-upgrade simulation did not produce an acceptable transaction; evaluating dist-upgrade.</source>
        <translation>Täysimittainen simulaatio ei tuottanut hyväksyttävää tapahtumaa; arviointi dist-upgrade.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1296"/>
        <source>Full-upgrade simulation was unavailable or unsafe; using the successful standard upgrade transaction.</source>
        <translation>Täysimittaista simulaatiota ei ollut saatavilla tai se ei ollut turvallista; käytössä on onnistunut standardipäivitystapahtuma.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1297"/>
        <source>apt intent translated: %1</source>
        <translation>apt aikomus käännetty: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1298"/>
        <source>APT package lists changed or a repository index was fetched; reporting the metadata refresh as changed.</source>
        <translation>APT-pakettiluettelot ovat muuttuneet tai arkistoindeksi noudettiin; metadatan päivitys on ilmoitettava sellaisena kuin se on muutettuna.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1299"/>
        <source>Target package policy rejected standard upgrade and requested full/dist upgrade; evaluating full-upgrade.</source>
        <translation>Tavoitepaketti politiikka hylkäsi standardin päivitys ja pyysi täyden / Dist päivitystä; arviointi täyden tason.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1300"/>
        <source>APT simulation proposes %1 non-protected package removal(s): %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1301"/>
        <source>REFUSED: APT simulation proposes removing essential packages.</source>
        <translation>EPÄÄMINEN: APT-simulaatiossa ehdotetaan keskeisten pakettien poistamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1302"/>
        <source>REFUSED: APT simulation would remove protected package &apos;%1&apos;.</source>
        <translation>EPÄÄMINEN: APT-simulaatio poistaisi suojatun paketin &quot;%1.&quot;</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1303"/>
        <source>REFUSED: APT simulation would remove %1 packages (safety limit: %2).</source>
        <translation>EPÄÄMINEN: APT-simulaatio poistaisi %1-paketit (turvallisuusraja: %2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1304"/>
        <source>Standard upgrade leaves packages pending; evaluating full-upgrade simulation.</source>
        <translation>Normaali päivitys jättää paketteja odottamaan; arvioi täysimittaista simulaatiota.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1305"/>
        <source>WARN: accepted release metadata change for %1; metadata refreshed.</source>
        <translation>WARN: hyväksytty julkaisumetatiedon muutos %1; metadatan päivitys.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1306"/>
        <source>WARNING: apt-get update refused a repository release metadata change for: %1</source>
        <translation>VAROITUS: apt-get update evättiin arkiston julkaisun metatiedon muutos: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1307"/>
        <source>WARNING: retrying metadata refresh with -o Acquire::AllowReleaseInfoChange=true (release-info changes only; signatures, keys and package verification remain enforced).</source>
        <translation>VAROITUS: uudelleenmetadatan päivittäminen -o Acquire::AllowReleaseInfoChange=true (vain julkaisu-info muutokset; allekirjoitukset, avaimet ja pakettien tarkistus ovat edelleen voimassa).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1308"/>
        <source>APT upgrade decision: &apos;%1&apos; selected from simulation results.</source>
        <translation>APT-päivityspäätös: &apos;%1&apos; valittu simulaatiotuloksista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1309"/>
        <source>apt upgrade is disabled by this distribution; running &apos;%1&apos; instead</source>
        <translation>apt-päivitys ei ole käytössä tällä jakelulla; sen sijaan käytetään &apos;%1&apos;</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1310"/>
        <source>PASS: Arch initramfs images rebuilt and verified.</source>
        <translation>PASS: Arch initramfs kuvat uudelleen ja todennettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1311"/>
        <source>AUTO-CORRECT: %1</source>
        <translation>AUTO-CORRECT: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1312"/>
        <source>AUTO-CORRECT: restored temporary stale mapper alias %1 -&gt; %2 for this repair request.</source>
        <translation>AUTO-CORRECT: kunnostettu väliaikainen vankkumaton mapper alias %1 -&gt; %2 tätä korjauspyyntöä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1313"/>
        <source>BEGIN: Chroot shell command</source>
        <translation>BEGIN: Chroot-kuorikomento</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1314"/>
        <source>BEGIN: Running-host shell command</source>
        <translation>Alku: Suoritus-ohjain komento</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1315"/>
        <source>BEGIN: %1</source>
        <translation>BEGIN: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1316"/>
        <source>BEGIN: package backend %1 (%2)</source>
        <translation>BEGIN: paketti taustaosa %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1317"/>
        <source>BEGIN: Refresh package metadata</source>
        <translation>BEGIN: Päivitä paketin metatiedot</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1318"/>
        <source>Arch GRUB layout detected; applying the guarded conventional EFI reinstall after initramfs preflight.</source>
        <translation>Arch GRUB asettelu havaittu; soveltamalla vartioitu tavanomainen EFI uudelleen asentaa jälkeen initramfs ennen lentoa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1319"/>
        <source>PASS: boot stack reconciliation completed after component simulations and verification.</source>
        <translation>PASS: boot pino täsmäytys suoritettu jälkeen komponenttien simulaatiot ja todentaminen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1320"/>
        <source>PASS: boot stack reconciliation completed; EFI/UKI and GRUB were reused from the earlier EFI / UKI stage while mapper/crypttab and initramfs were reconciled.</source>
        <translation>PASS: boot pino täsmäytys valmis; EFI/UKI ja GRUB uudelleenkäytettiin aikaisemmasta EFI / UKI vaiheessa ja mapper / cryptab ja initramfs täsmättiin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1321"/>
        <source>Boot-stack Arch EFI read-only preflight: PASS (%1, %2, id=%3)</source>
        <translation>Boot-stack Arch EFI luku vain ennen lentoa: PASS (%1, %2, id=%3)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1322"/>
        <source>Fedora BIOS GRUB2 layout detected; no EFI/UKI artifacts are part of boot-stack reconciliation.</source>
        <translation>Fedora BIOS GRUB2 asettelu havaittu; EFI/UKI-esineet eivät ole osa boot-stack täsmäytys.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1323"/>
        <source>No TUXEDO UKI builder detected; preserving the distribution&apos;s existing EFI layout during boot-stack reconciliation.</source>
        <translation>TUXEDO UKI -rakentajaa ei ole havaittu; jakelun nykyinen EFI-asettelu säilytetään käynnistyksen täsmäytyksen aikana.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1324"/>
        <source>NOTE: --post-efi is an EFI-only hint; the Fedora BIOS boot stack reconciles dracut and GRUB2 without an EFI stage.</source>
        <translation>HUOM: -- post-efi on vain EFI:n vihje; Fedora BIOS -kengät täsmäävät dracut:n ja GRUB2:n kanssa ilman EFI-vaihetta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1325"/>
        <source>SIMULATE/PREFLIGHT: complete boot-stack reconciliation</source>
        <translation>SIMULATE/PREFLIGHT: täydellinen boot-stack täsmäytys</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1326"/>
        <source>SKIP: boot-stack EFI/UKI rebuild skipped because the EFI / UKI bootloader stage already rebuilt and verified this layout in the same run (--post-efi).</source>
        <translation>SKIP: boot-stack EFI/UKI remontoitu koska EFI / UKI bootloader vaihe on jo rakennettu uudelleen ja todennettu tämän asettelun samassa juoksussa (- post-efi).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1327"/>
        <source>SKIP: boot-stack GRUB regeneration skipped because the EFI / UKI bootloader stage already regenerated the GRUB configuration in the same run (--post-efi).</source>
        <translation>SKIP: boot-stack GRUB regeneraatio ohitettiin, koska EFI / UKI bootloader vaihe jo regeneroitu GRUB kokoonpano samassa ajossa (- post-efi).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1328"/>
        <source>Boot-stack TUXEDO UKI read-only preflight: PASS (%1, %2)</source>
        <translation>Boot-stack TUXEDO UKI luku vain ennen lentoa: PASS (%1, %2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1329"/>
        <source>Bound recovery-host resolver into target chroot (temporary, read-only)</source>
        <translation>Sidottu elvytys-host resolver kohde Chroot (tilapäinen, luku-vain)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1330"/>
        <source>WARNING: btrfs check --repair is a last-resort operation that upstream documents as dangerous and can make a damaged filesystem worse. The caller must have explicit user confirmation and a backup.</source>
        <translation>VAROITUS: btrfs check --korjaus on viimeinen resort-operaatio, että yläjuoksulla asiakirjat ovat vaarallisia ja voi pahentaa vaurioitunut tiedostojärjestelmä. Soittajalla on oltava käyttäjän vahvistus ja varmuuskopio.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1331"/>
        <source>Chroot shell exit code: %1</source>
        <translation>Chroot kuori exit koodi: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1332"/>
        <location filename="../src/MainWindow.cpp" line="1333"/>
        <source>Command: %1</source>
        <translation>Komento: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1334"/>
        <source>Configuration read skipped: %1 has no parent directory in the target (bootloader backend: %2).</source>
        <translation>Asetukset luettu ohi: %1:llä ei ole emohakemistoa kohteessa (bootloader backend: %2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1335"/>
        <source>Backed up the ESP loader files to %1 before the GRUB EFI reinstall (firmware fallback present before repair: %2).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1336"/>
        <source>PASS: refreshed the firmware fallback loader EFI/BOOT/BOOTX64.EFI (--removable).</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1337"/>
        <source>Restored the ESP loader files from the session backup %1.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1338"/>
        <source>COPY COMPLETE</source>
        <translation>KOKO TÄYDELLINEN</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1339"/>
        <source>COPY COMPLETE — SHA-256 verification FAILED</source>
        <translation>COPY TÄYDELLINEN</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1340"/>
        <source>FAILED verification: %1</source>
        <translation>FAIED-varmennus: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1341"/>
        <source>%1: %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1342"/>
        <source>rsync metadata/content re-check: PASS</source>
        <translation>rsync metadatan/sisällön uudelleentarkistus: PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1343"/>
        <source>SHA-256 verification FAILURES: %1 (the copy is not verified complete)</source>
        <translation>SHA-256 verifiointi FAILURES: %1 (kopiota ei ole tarkistettu kokonaan)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1344"/>
        <source>SHA-256 regular files verified: %1</source>
        <translation>SHA-256 säännöllinen tiedostot todennettu: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1345"/>
        <source>Source items: %1</source>
        <translation>Lähde: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1346"/>
        <source>Unrelated destination files: retained (no --delete used)</source>
        <translation>Liittymättömät kohdetiedostot: säilytetty (ei --poistettu)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1347"/>
        <source>Correction simulation exit code: %1 (%2)</source>
        <translation>Korjaussimulaation poistumiskoodi: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1348"/>
        <source>WARNING: could not mount target %1 from %2; the read-only diagnostic proceeds without it.</source>
        <translation>VAROITUS: kohdetta %1 ei voitu asentaa %2:stä; lukuavain diagnostinen etenee ilman sitä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1349"/>
        <source>Created target destination directory %1 with inherited owner %2:%3</source>
        <translation>Luonut kohde kohde hakemisto %1 perinnöllinen omistaja %2:%3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1350"/>
        <source>Mapper/crypttab consistency gate: PASS</source>
        <translation>Mapper / cryptab johdonmukaisuus portti: PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1351"/>
        <source>Mapper/crypttab gate: no target crypttab entries; PASS</source>
        <translation>Mapper/cryptab-portti: ei kohdekryptab-syötteitä; PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1352"/>
        <source>Mapper/crypttab gate: entry &apos;%1&apos; resolves outside the selected disk: %2</source>
        <translation>Mapper/cryptab-portti: merkintä &apos;%1&apos; ratkaisee valitun levyn ulkopuolella: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1353"/>
        <source>Mapper/crypttab gate: %1 -&gt; %2</source>
        <translation>Mapper/cryptab-portti: %1 -&gt; %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1354"/>
        <source>Mapper/crypttab gate: unresolved entry &apos;%1&apos; -&gt; &apos;%2&apos;</source>
        <translation>Mapper/cryptab-portti: ratkaisematon merkintä &apos;%1&apos; -&gt; &apos;%2&apos;</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1355"/>
        <source>Mapper/crypttab gate: unsupported source syntax for &apos;%1&apos;: %2</source>
        <translation>Mapper/cryptab-portti: tukeematon lähdesyntaksi &apos;%1&apos;:lle: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1356"/>
        <source>Destination: %1</source>
        <translation>Kohde: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1357"/>
        <source>Detected target OS: %1</source>
        <translation>Kohde: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1358"/>
        <source>WARNING: private /dev filter: the selected target disk identity is unavailable; no block device or mapper will be exposed to the target chroot.</source>
        <translation>VAROITUS: yksityinen /dev-suodatin: valittu kohdelevyn henkilöllisyys ei ole saatavilla; kohdechrootille ei anneta lohkolaitetta tai karttaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1359"/>
        <source>Private /dev filter: %1 allowed block devices/mappers copied; %2 non-essential device-tree entries refused (plain files, sockets, fifos, and devices outside the selected target disk).</source>
        <translation>Yksityinen /dev-suodatin: %1:n sallimat lohkolaitteet/mapperit kopioitu; %2:n muut kuin olennaiset laite-puumerkinnät hylätty (plain-tiedostot, pistorasiat, fifos ja valitun kohdelevyn ulkopuoliset laitteet).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1360"/>
        <source>DKMS preflight correction: build tree now present for %1</source>
        <translation>DKMS:n lentoa edeltävä korjaus: %1:n rakennuspuu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1361"/>
        <source>SIMULATE/PREFLIGHT: dracut DKMS rebuild (headers must already be installed; no package guessing is performed)</source>
        <translation>SIMULATE/PREFLIGHT: dracut DKMS:n uudelleenrakentaminen (otsimet on jo asennettava; paketin arvauksia ei tehdä)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1362"/>
        <location filename="../src/MainWindow.cpp" line="1363"/>
        <location filename="../src/MainWindow.cpp" line="1364"/>
        <source>DKMS preflight: headers/build tree present for %1</source>
        <translation>DKMS ennen lentoa: %1:n otsake/rakennuspuu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1365"/>
        <source>KNOWN ISSUE: DKMS reported missing kernel headers; re-running header preflight/correction once.</source>
        <translation>TIEDON NUMERO: DKMS ilmoitti puuttuvansa ytimen otsikot; uudelleen käynnissä otsikko ennen lentoa / korjaus kerran.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1366"/>
        <source>DKMS preflight: missing headers for %1; repository package %2 is available.</source>
        <translation>DKMS esilento: puuttuvat otsikot %1:lle; arkistopaketti %2 on saatavilla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1367"/>
        <source>WARNING: DKMS preflight found no build tree for %1 and %2 is unavailable from configured repositories.</source>
        <translation>VAROITUS: DKMS:n ennen lentoa ei löytynyt %1:lle eikä %2:lle rakennettua puuta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1368"/>
        <source>SIMULATE/PREFLIGHT: mkinitcpio DKMS rebuild (headers must already be installed; no package guessing is performed)</source>
        <translation>SIMULATE/PREFLIGHT: mkinitcpio DKMS:n uudelleenrakentaminen (otsikot on jo asennettava; ei paketin arvausta)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1369"/>
        <source>SIMULATE/PREFLIGHT: mkinitfs DKMS rebuild (headers must already be installed; no package guessing is performed)</source>
        <translation>SIMULATE/PREFLIGHT: mkinitfs DKMS:n uudelleenrakentaminen (otsakkeet on jo asennettava; ei paketin arvauksia)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1370"/>
        <source>PASS: DKMS autoinstall completed.</source>
        <translation>PASS: DKMS-automaattiasennus valmis.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1371"/>
        <source>SIMULATE/PREFLIGHT: DKMS module rebuild</source>
        <translation>SIMULATE/PREFLIGHT: DKMS-moduulin jälleenrakentaminen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1372"/>
        <source>%1 was configured offline only; Boot Bitch intentionally did not start a graphical session.</source>
        <translation>%1 konfiguroitu vain offline; Boot Bitch tarkoituksella ei aloittanut graafista istuntoa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1373"/>
        <source>%1 was configured offline only; Boot Bitch intentionally did not start a graphical session inside the chroot.</source>
        <translation>%1 konfiguroitu vain offline; Boot Bitch tarkoituksella ei aloittanut graafista istuntoa sisällä Chroot.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1374"/>
        <source>PASS: default.target -&gt; %1</source>
        <translation>PASS: oletustavoite -&gt; %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1375"/>
        <source>Detected display manager: %1 (%2)</source>
        <translation>Havaittu näytönhallinta: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1376"/>
        <source>Enabling %1 in the offline target (it will NOT be started inside the repair chroot).</source>
        <translation>%1:n mahdollistaminen offline-kohteessa (se EI käynnisty korjauschrootin sisällä).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1377"/>
        <source>WARNING: %1 ExecStart is not executable in the target: %2</source>
        <translation>VAROITUS: %1 ExecStart ei ole suorituskelpoinen kohteessa: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1378"/>
        <source>PASS: %1 ExecStart is present for headless preflight: %2</source>
        <translation>PASS: %1 ExecStart on läsnä päättömänä ennen lentoa: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1379"/>
        <source>PASS: display-manager.service -&gt; %1</source>
        <translation>PASS: näyttö-manager.service -&gt; %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1380"/>
        <source>PASS: %1 command present for headless preflight: %2</source>
        <translation>PASS: %1-komento paikalla päättömänä ennen lentoa: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1381"/>
        <source>Detected Alpine OpenRC display manager: %1 (%2)</source>
        <translation>Havaittu Alpine OpenRC -näytön hallinta: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1382"/>
        <source>PASS: /etc/runlevels/default/%1 -&gt; %2</source>
        <translation>PASS: /etc/run levels/default/%1 -&gt; %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1383"/>
        <source>WARNING: no explicit command= was found for %1; sh -n and rc-service -e still passed.</source>
        <translation>VAROITUS: %1:lle ei löytynyt nimenomaista komentoa; sh -n ja rc-service -e läpäistiin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1384"/>
        <source>%1 will be configured offline only; Boot Bitch will not start a graphical session.</source>
        <translation>%1 konfiguroidaan vain offline-tilassa; Boot Bitch ei käynnistä graafista istuntoa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1385"/>
        <source>Restoring the OpenRC default-runlevel link for %1 (offline only; the service is NOT started).</source>
        <translation>%1:n OpenRC-lähtötason linkin palauttaminen (vain offline; palvelua EI aloiteta).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1386"/>
        <source>SIMULATE/PREFLIGHT: OpenRC graphical login / display manager (offline only)</source>
        <translation>SIMULATE/PREFLIGHT: OpenRC graafinen kirjautuminen / näyttö hallinta (vain offline)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1387"/>
        <source>Display-manager preflight plan: set graphical.target default, enable %1, and repair display-manager.service offline.</source>
        <translation>Display-manager esilentosuunnitelma: aseta graafinen.kohde oletus, ota %1 käyttöön ja korjaa näyttö-manager.service offline.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1388"/>
        <source>Restoring graphical.target as the target default.</source>
        <translation>Palautetaan graafinen kohde oletuksena.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1389"/>
        <source>Selected %1 from last-boot display-manager journal evidence.</source>
        <translation>Valittu %1 viime saappaan näytön manager päiväkirjan todisteita.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1390"/>
        <source>Selected %1 from last-boot display-manager journal evidence because multiple managers are installed.</source>
        <translation>Valittu %1 viime saappaan näyttö-hallinta päiväkirjan todisteita, koska useita managereita on asennettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1391"/>
        <source>SIMULATE/PREFLIGHT: graphical login / display manager</source>
        <translation>SIMULATE/PREFLIGHT: graafinen kirjautuminen / näytön hallinta</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1392"/>
        <source>%1 is a static unit; preserving it through the display-manager.service alias.</source>
        <translation>%1 on staattinen yksikkö, joka säilyttää sen näytönohjaimen läpi. Palvelunimi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1393"/>
        <source>WARNING: systemd-analyze is unavailable; skipped headless display-manager verification.</source>
        <translation>VAROITUS: systemd-analysoiminen ei ole käytettävissä; ohitettu päätön näyttö-manager vahvistus.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1394"/>
        <source>WARNING: headless systemd verification reported issues for %1 (the manager will not be started during repair).</source>
        <translation>VAROITUS: Päätön järjestelmällinen todentaminen raportoitu ongelmia %1 (johtaja ei käynnisty korjauksen aikana).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1395"/>
        <source>PASS: headless systemd verification for %1</source>
        <translation>PASS: Päätön järjestelmällinen verifiointi %1:lle</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1396"/>
        <source>This transaction downloads approximately %1; the apply step reports its progress only when it finishes.</source>
        <translation>Tämä tapahtuma lataa noin %1; sovellettava vaihe raportoi sen etenemistä vain, kun se päättyy.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1397"/>
        <source>PASS: dnf5 upgrade removes only the old kernel package(s) (%1) — accepted as in-place kernel replacement.</source>
        <translation>PASS: dnf5 päivitys poistaa vain vanhan ytimen paketti(s) (%1) .</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1398"/>
        <source>dnf5 repository metadata cache rewritten (cache fingerprint changed); reporting the metadata refresh as changed.</source>
        <translation>dnf5 arkiston metadata välimuistin uudelleenkirjoitettu (välimuistin sormenjälki muuttunut); metadatan päivitys muuttunut.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1399"/>
        <source>PASS: dnf5 metadata cache refreshed (repository metadata cache is byte-identical).</source>
        <translation>PASS: dnf5 metadata välimuisti päivitetty (repository metadata välimuisti on byte-identtinen).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1400"/>
        <source>REFUSED: dnf5 simulation reported an unresolved or conflicting transaction.</source>
        <translation>EPÄÄMINEN: dnf5-simulaatio ilmoitti selvittämättömästä tai ristiriitaisesta tapahtumasta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1401"/>
        <source>REFUSED: dnf5 simulation proposes a package downgrade.</source>
        <translation>EPÄÄMINEN: dnf5-simulaatiossa ehdotetaan paketin vähentämistä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1402"/>
        <source>REFUSED: dnf5 simulation failed with exit code %1.</source>
        <translation>EPÄÄMISESTÄ: dnf5-simulaatio epäonnistui lähtökoodin %1 kanssa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1403"/>
        <source>REFUSED: dnf5 simulation reported a locked package database.</source>
        <translation>EPÄÄMINEN: dnf5-simulaatio ilmoitti lukitusta pakettitietokannasta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1404"/>
        <source>REFUSED: dnf5 simulation reported incomplete repository metadata.</source>
        <translation>EPÄÄMINEN: dnf5-simulaatio ilmoitti epätäydellisistä arkistometatiedoista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1405"/>
        <source>REFUSED: dnf5 upgrade would obsolete critical package &apos;%1&apos;.</source>
        <translation>EPÄÄMINEN: dnf5-päivitys olisi vanhentunut kriittinen paketti &quot;%1.&quot;</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1406"/>
        <source>REFUSED: dnf5 reinstall would replace &apos;%1&apos;, which is not one of the reinstalled packages.</source>
        <translation>EPÄÄMINEN: dnf5 reinstall korvaisi &quot;%1:n,&quot; joka ei kuulu asennettuihin pakkauksiin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1407"/>
        <source>REFUSED: dnf5 simulation proposes package removals.</source>
        <translation>EPÄÄMINEN: dnf5-simulaatiossa ehdotetaan pakettien poistamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1408"/>
        <source>REFUSED: dnf5 simulation would replace %1 packages (safety limit: %2).</source>
        <translation>EPÄÄMINEN: dnf5-simulaatio korvaisi %1-pakkaukset (turvallisuusraja: %2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1409"/>
        <source>REFUSED: dnf5 simulation reported an untrusted or missing package signature key.</source>
        <translation>EPÄÄMINEN: dnf5-simulaatio ilmoitti luottamattomasta tai puuttuvasta paketin allekirjoitusavaimesta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1410"/>
        <source>REFUSED: dnf5 simulation proposes %1 package changes (safety limit: %2).</source>
        <translation>EPÄÄMINEN: dnf5-simulaatiossa ehdotetaan %1-paketin muutoksia (turvallisuusraja: %2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1411"/>
        <source>REFUSED: dnf5 simulation reported an error Boot Bitch does not recognize as safe.</source>
        <translation>EPÄÄMINEN: dnf5-simulaatio ilmoitti virheen Boot Bitch ei tunnista sitä turvalliseksi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1412"/>
        <source>dnf5 preflight version: %1</source>
        <translation>dnf5-versio ennen lentoa: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1413"/>
        <source>SIMULATE/PREFLIGHT: dracut guarded per-kernel rebuild for %1 installed kernel(s) (single build per kernel; verified before any /boot write)</source>
        <translation>SIMULATE/PREFLIGHT: dracut vartioitu %1-ytimen uudelleenrakennettu (yksi rakenne ytimessä; todennettu ennen /boot kirjoitus)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1414"/>
        <source>dracut preflight version: %1</source>
        <translation>dracut esilentoversio: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1415"/>
        <source>e2fsck preen left uncorrected errors (exit code 4); running the confirmed forced repair pass.</source>
        <translation>e2fsck preen vasen korjaamaton virhe (poistumakoodi 4); käynnissä vahvistettu pakotettu korjauspassi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1416"/>
        <source>Annotating selected-system EFI entries with OS/model identity: %1 / %2 (PARTUUID %3).</source>
        <translation>Ilmoitetaan valittu järjestelmä EFI merkinnät OS / malli identiteetti: %1 / %2 (PARTUUID %3).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1417"/>
        <source>EFI bootloader ID: %1</source>
        <translation>EFI bootloader ID: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1418"/>
        <source>PASS: EFI BootOrder already groups each drive&apos;s firmware destinations by normal use.</source>
        <translation>PASS: EFI BootOrder ryhmittää jo jokaisen aseman firmware kohteet normaaliin käyttöön.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1419"/>
        <source>PASS: EFI BootOrder grouped by drive; all retained firmware entries remain present.</source>
        <translation>PASS: EFI BootOrder ryhmitelty aseman mukaan; kaikki säilytetyt firmware merkinnät pysyvät läsnä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1420"/>
        <source>Grouping EFI BootOrder by drive and normal boot use: %1</source>
        <translation>EFI BootOrderin ryhmittely ajon ja normaalin boot-käytön avulla: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1421"/>
        <source>Clearing BootNext introduced during EFI repair.</source>
        <translation>Clearing BootNext otettiin käyttöön EFI korjaus.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1422"/>
        <source>PASS: Boot%1 is first in BootOrder; all other entries were retained.</source>
        <translation>PASS: Boot%1 on ensimmäinen BootOrder; kaikki muut merkinnät pidettiin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1423"/>
        <source>PASS: selected ESP firmware destinations reconciled; removed Boot%1.</source>
        <translation>PASS: Valitut ESP firmware kohteet sovitettu; poistettu Boot%1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1424"/>
        <source>PASS: selected ESP firmware destinations are unique; no duplicate entries removed.</source>
        <translation>PASS: Valitut ESP-ohjelmistokohteet ovat ainutlaatuisia; kaksoismerkintää ei ole poistettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1425"/>
        <source>PASS: %1 firmware entry restored as Boot%2 on selected system ESP %3.</source>
        <translation>PASS: %1 firmware sisääntulo palautettu Boot%2 valitussa järjestelmässä ESP %3.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1426"/>
        <source>PASS: generic EFI firmware entry restored as Boot%1 on selected system ESP %2.</source>
        <translation>PASS: yleinen EFI firmware merkintä palautettu Boot%1 valitussa järjestelmässä ESP %2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1427"/>
        <source>EFI grouping did not retain Boot%1 first; re-promoting it once.</source>
        <translation>EFI-ryhmä ei säilyttänyt Boot%1:ää ensin; sen edistäminen uudelleen kerran.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1428"/>
        <source>EFI entry identity preserved while firmware ID changed: Boot%1 -&gt; Boot%2.</source>
        <translation>EFI-tietue säilyi, kun ohjelmistotunniste muuttui: Boot%1 -&gt; Boot%2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1429"/>
        <source>iPXE/WebFAI EFI loader is absent on selected system ESP; no recovery firmware entry was created.</source>
        <translation>iPXE/WebFAI EFI-kuormaajaa ei ole valitussa järjestelmässä ESP; palautusohjelmistoja ei ole luotu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1430"/>
        <source>iPXE/WebFAI EFI loader is present on the selected system ESP, but firmware variables are not writable; no recovery firmware entry was created.</source>
        <translation>iPXE/WebFAI EFI-kuormaaja on mukana valitussa järjestelmässä ESP, mutta firmware-muuttujat eivät ole kirjoitettavia; firmware-tiedostoa ei ole luotu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1431"/>
        <source>Repair-target EFI entry label preserved while loader changed: Boot%1 -&gt; Boot%2.</source>
        <translation>Korjaa kohde EFI merkintä etiketti säilytetään samalla kuormaaja muuttunut: Boot%1 -&gt; Boot%2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1432"/>
        <source>EFI Boot%1 already names selected model; leaving label &apos;%2&apos; unchanged.</source>
        <translation>EFI Boot%1 jo nimittää valitun mallin, jolloin etiketti &apos;%2&apos; ei muutu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1433"/>
        <source>EFI Boot%1 (%2) label updated to &apos;%3&apos; on selected system ESP only.</source>
        <translation>EFI Boot%1 (%2) - nimilappu päivitetty &quot;%3&quot; vain valitussa järjestelmässä ESP.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1434"/>
        <source>PASS: selected ESP EFI labels retain the drive model after final BootOrder maintenance.</source>
        <translation>PASS: Valitut ESP EFI tarrat säilyttävät ajomallin viimeisen BootSertder-huollon jälkeen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1435"/>
        <source>Selected EFI loader is present at %1, but firmware variables are not writable; no generic firmware entry was created.</source>
        <translation>Valittu EFI-kuormaaja on läsnä %1:ssä, mutta firmware-muuttujat eivät ole kirjoitettavia; geneerisiä firmware-ohjelmia ei luotu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1436"/>
        <source>PASS: EFI loader files verified under %1/EFI/%2</source>
        <translation>PASS: EFI-kuormaajatiedostot todennettu %1/EFI/%2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1437"/>
        <source>Making Boot%1 the explicit default while preserving all other EFI entries: %2</source>
        <translation>Tehdään Boot%1:stä nimenomainen oletus säilyttäen samalla kaikki muut EFI-syötteet: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1438"/>
        <source>Multiple generic EFI entries already point to the selected system loader (%1); destination maintenance will retain one after the repair.</source>
        <translation>Useat yleiset EFI merkinnät osoittavat jo valittuun järjestelmäkuormaajaan (%1); määränpään huolto säilyttää yhden korjauksen jälkeen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1439"/>
        <source>Multiple iPXE/WebFAI entries already point to the selected system ESP (%1); destination maintenance will retain one after the repair.</source>
        <translation>Useat iPXE/WebFAI-merkinnät osoittavat jo valittuun ESP-järjestelmään (%1); kohdehuolto säilyttää yhden korjauksen jälkeen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1440"/>
        <source>Multiple TUXEDO UKI entries already point to the selected ESP (%1); destination maintenance will retain one after the repair.</source>
        <translation>Useat TUXEDO UKI merkinnät osoittavat jo valittuun ESP (%1); kohde huolto säilyttää yhden jälkeen korjaus.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1441"/>
        <source>Selected EFI vendor directory has no primary loader; no generic firmware entry was created.</source>
        <translation>Valitussa EFI-toimittajahakemistossa ei ole ensisijaista laturia; geneerisiä firmware-ohjelmia ei luotu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1442"/>
        <source>KNOWN ISSUE: firmware NVRAM registration failed; retrying once as a file-only --no-nvram reinstall.</source>
        <translation>TIEDON NUMERO: firmware NVRAM -rekisteröinti epäonnistui; yritetään uudelleen kerran vain tiedostona -- no-nvram uudelleenasennukseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1443"/>
        <source>EFI System Partition: %1 (%2)</source>
        <translation>EFI System Partition: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1444"/>
        <source>EFI registration mode: %1</source>
        <translation>EFI-rekisteröintitila: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1445"/>
        <source>EFI registration mode: Alpine --no-nvram install with helper-managed firmware entries (writable efivars: %1).</source>
        <translation>EFI-rekisteröintitila: Alpine --no-nvram asennusavulla ohjatut firmware-merkinnät (kirjoitettavat efivars: %1).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1446"/>
        <source>Removing duplicate/legacy selected-ESP firmware destination Boot%1.</source>
        <translation>Poistaa kaksois-/legacy valittu-ESP firmware kohde Boot%1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1447"/>
        <source>EFI repair read-only preflight: PASS (%1, %2, id=%3)</source>
        <translation>EFI korjaus vain ennen lentoa: PASS (%1, %2, id=%3)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1448"/>
        <source>EFI repair completed; simulating and regenerating the GRUB fallback configuration.</source>
        <translation>EFI korjaus valmis; simuloi ja uudistaa GRUB varakokoonpano.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1449"/>
        <source>Restoring reconciled BootNext: %1</source>
        <translation>Yhteensovitetun BootNextin palauttaminen: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1450"/>
        <source>Restoring reconciled EFI BootOrder (all pre-existing entries retained): %1</source>
        <translation>Yhteensovitetun EFI BootOrderin palauttaminen (kaikki aiemmat merkinnät säilytetään): %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1451"/>
        <source>Restoring %1 firmware entry &apos;%2&apos; for PARTUUID %3 (EFI path %4).</source>
        <translation>Palautetaan %1-ohjelmiston merkintä &quot;%2&quot; PAPUUID %3:lle (EFI-polku %4).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1452"/>
        <source>Restoring missing %1 firmware entry on selected system ESP %2 as &apos;%3&apos; (%4).</source>
        <translation>Palautetaan puuttuva %1-ohjelmistomerkintä valitussa järjestelmässä ESP %2 nimellä &apos;%3&apos; (%4).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1453"/>
        <source>Restoring missing generic EFI firmware entry on selected system ESP %1 as &apos;%2&apos; (%3).</source>
        <translation>Palautetaan puuttuva yleinen EFI-ohjelmistomerkintä valitussa järjestelmässä ESP %1 nimellä &apos;%2&apos; (%3).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1454"/>
        <source>Alpine EFI-stub repair read-only preflight: PASS (%1, %2)</source>
        <translation>Alpine EFI-stib korjaus luku vain ennen lentoa: PASS (%1, %2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1455"/>
        <source>TUXEDO UKI firmware entry is absent for the selected system ESP; creating only that selected-system entry as &apos;%1&apos;.</source>
        <translation>TUXEDO UKI firmware -merkintä puuttuu valitusta järjestelmästä ESP; luodaan vain se valittu järjestelmämerkintä &quot;%1.&quot;</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1456"/>
        <source>Writable UEFI efivars detected; first attempt will permit firmware registration.</source>
        <translation>Writable UEFI efivars havaittu; ensimmäinen yritys mahdollistaa firmware rekisteröinti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1457"/>
        <source>Writable UEFI efivars are unavailable; using the preflight-selected --no-nvram path.</source>
        <translation>Kirjoitettavat UEFI efivarit eivät ole käytettävissä; käytetään esilentoon valittua --nvram-polkua.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1458"/>
        <source>ESP mount: auto-mount failed: target=%1 reason=%2 hint=%3</source>
        <translation>ESP-asennus: Auto-mount epäonnistui: kohde=%1 syy=%2 vihje= %3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1459"/>
        <source>ESP mount: auto-mount failed: target=%1 reason=mounted source %2 is not a block device on the selected target disk hint=%3</source>
        <translation>ESP-asennus: Auto-mount epäonnistui: tavoite=%1 syy = asennettu lähde %2 ei ole lohko laite valittu kohdelevy vihje = %3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1460"/>
        <source>ESP mount: auto-mount failed: target=%1 reason=mounted source %2 is %3, not FAT hint=%4</source>
        <translation>ESP-asennus: Auto-mount epäonnistui: tavoite=%1 syy = asennettu lähde %2 on %3, ei FAT vihje = %4</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1461"/>
        <source>ESP mount cleanup: unmounted leaked ro layer target=%1 source=%2 id=%3</source>
        <translation>ESP asennus siivous: asentamaton ro kerros tavoite=%1 lähde=%2 id=%3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1462"/>
        <source>ESP mount preflight: target=%1 stack=%2 top-source=%3 top-options=%4 top-id=%5 verdict=%6 leaked-ro=%7</source>
        <translation>ESP mount preflight: kohde=%1 pino=%2 top-source=%3 top-optiot=%4 top-id=%5 tuomio=%6 vuoto-ro=%7</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1463"/>
        <source>ESP mount: mounted target=%1 source=%2 method=%3</source>
        <translation>ESP-asennus: asennettu kohde=%1-lähde=%2-menetelmä=%3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1464"/>
        <source>Backed up the extlinux configuration under %1</source>
        <translation>Varmistettu extlinux-konfiguraatiolla %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1465"/>
        <source>PASS: extlinux candidate references %1 boot artifact(s) present in the target</source>
        <translation>PASS: extlinux ehdokas viittaa %1 boot artefact [s] läsnä kohde</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1466"/>
        <source>PASS: running host default extlinux entry is LABEL %1 -&gt; LINUX %2 + INITRD %3.</source>
        <translation>PASS: käynnissä isäntä oletus extlinux merkintä on ETIKETTI %1 -&gt; LINUX %2 + INITRD %3.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1467"/>
        <source>Restored the pre-repair extlinux default configuration</source>
        <translation>Palautti extlinux:n oletusasetukset</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1468"/>
        <source>Extlinux default entry: label=%1 kernel=%2 action=set</source>
        <translation>Extlinux-oletusmerkintä: etiketti=%1-ydin=%2-toiminto=setti</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1469"/>
        <source>Extlinux default entry: label=%1 kernel=%2 action=unchanged</source>
        <translation>Extlinux-oletussyöte: etiketti=%1-ytimen=%2-toiminto=muuttumaton</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1470"/>
        <source>PASS: /boot/extlinux.conf regenerated and verified; the boot sector was not written.</source>
        <translation>PASS: /boot/extlinux.conf regeneroitu ja tarkistettu; käynnistyssektoria ei ole kirjoitettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1471"/>
        <source>Restored the pre-repair extlinux configuration</source>
        <translation>Korjaa extlinux-kokoonpano</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1472"/>
        <source>SIMULATE/PREFLIGHT: extlinux configuration regeneration via %1 (overwrite=0 trial; no boot sector write)</source>
        <translation>SIMULATE/PREFLIGHT: extlinux:n konfiguraation regeneraatio %1:n avulla (korvaa=0 tutkimus; ei käynnistyssektorin kirjoitusta)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1473"/>
        <source>Fedora default entry: saved_entry=%1 resolves=%2 target=%3 action=set</source>
        <translation>Fedora-oletussyöte: tallennettu entry=%1 ratkaisee=%2-tavoite=%3-toiminto=asetettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1474"/>
        <source>Fedora default entry: saved_entry=%1 resolves=yes target=%2 action=set</source>
        <translation>Fedora-oletussyöte: saved entry=%1 ratkaisee=kyllä kohde=%2 toiminto=asetettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1475"/>
        <source>Fedora default entry: saved_entry=%1 resolves=yes target=%2 action=unchanged</source>
        <translation>Fedora-oletussyöte: tallennettu entry=%1 ratkaisee=kyllä kohde=%2 toiminto=muuttumaton</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1476"/>
        <source>Backed up Fedora GRUB2 configuration artifacts to %1.</source>
        <translation>Varmistin Fedora GRUB2:n konfiguraatioesineet %1: lle.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1477"/>
        <source>PASS: Fedora GRUB2 configuration regenerated and verified.</source>
        <translation>PASS: Fedora GRUB2 asetus regeneroitu ja tarkistettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1478"/>
        <source>Restored the Fedora GRUB2 configuration artifacts from %1.</source>
        <translation>Palautti Fedora GRUB2-kokoonpanon %1:stä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1479"/>
        <source>SIMULATE/PREFLIGHT: generate Fedora GRUB2 configuration with --no-grubenv-update</source>
        <translation>SIMULATE/PREFLIGHT: luo Fedora GRUB2-asetukset -- no-grubenv-update</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1480"/>
        <source>PASS: Fedora GRUB2 trial configuration generated successfully.</source>
        <translation>PASS: Fedora GRUB2 kokeiluversio luotiin onnistuneesti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1481"/>
        <source>Fedora default entry: grubenv keys other than saved_entry preserved (sha256 %1…).</source>
        <translation>Fedora oletusmerkintä: grubenv avaimet muut kuin tallennettu entry säilynyt (sha256 %1…).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1482"/>
        <source>PASS: Fedora initramfs for %1 rebuilt byte-identical; temporary build discarded.</source>
        <translation>PASS: Fedora initramfs %1 restructed byte-identical; väliaikainen rakentaa hylätty.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1483"/>
        <source>PASS: Fedora initramfs rebuilt and verified for %1</source>
        <translation>PASS: Fedora initramfs uudelleenrakennettu ja tarkistettu %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1484"/>
        <source>File Copy %1: %2</source>
        <translation>Tiedostokopio %1: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1485"/>
        <source>Simulation exit code for &apos;--fix-broken install&apos;: %1</source>
        <translation>Simulointi exit code for &quot;-fix-broken install&quot;: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1486"/>
        <source>Released the helper&apos;s read-only mount(s) of %1 before offline repair.</source>
        <translation>Vapautti %1:n luku-avainkiinnikkeet ennen offline-korjausta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1487"/>
        <source>REPAIR: %1 (%2) on %3 (fstype=%4, uuid=%5, mount=%6)</source>
        <translation>KORJAUS: %1 (%2) on %3 (fstype=%4, uud=%5, mount=%6)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1488"/>
        <source>FAIL: file system repair (%1) reported unresolved issues for %2 (exit code %3).</source>
        <translation>FAIL: tiedostojärjestelmän korjaus (%1) ilmoitti ratkaisemattomia kysymyksiä %2 (poistumiskoodi %3).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1489"/>
        <source>PASS: file system repair (%1) corrected errors on %2 (exit code %3).</source>
        <translation>PASS: tiedostojärjestelmän korjaus (%1) korjatut virheet %2 (poistumiskoodi %3).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1490"/>
        <source>PASS: file system repair (%1) completed for %2 with exit code 0.</source>
        <translation>PASS: tiedostojärjestelmän korjaus (%1) valmis %2 kanssa exit koodi 0.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1491"/>
        <source>PASS: file system repair (%1) corrected errors on %2; a reboot is recommended before using the filesystem (exit code 2).</source>
        <translation>PASS: tiedostojärjestelmän korjaus (%1) korjatut virheet %2; uudelleenkäynnistys suositellaan ennen tiedostojärjestelmän käyttöä (poistumiskoodi 2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1492"/>
        <source>FAIL: file system repair (%1) timed out for %2 (exit code %3).</source>
        <translation>FAIL: tiedostojärjestelmän korjaus (%1) ajoitettu pois %2 (poistettu koodi %3).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1493"/>
        <source>File system inspection found no resolvable scope filesystems (root, /boot, ESP and /home).</source>
        <translation>Tiedostojärjestelmän tarkastus ei löytänyt resoluutiotiedostojärjestelmiä (root, /boot, ESP ja / kotiin).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1494"/>
        <source>File system inspection started (read-only; no repair tool is invoked).</source>
        <translation>Tiedostojärjestelmän tarkastus aloitettiin (vain luku; korjaustyökalua ei käytetä).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1495"/>
        <source>WARNING: fstab %1 device %2 is not on the selected disk %3; excluded from the file system scope.</source>
        <translation>VAROITUS: fstab %1-laite %2 ei ole valitulla levyllä %3; jätetään tiedostojärjestelmän soveltamisalan ulkopuolelle.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1496"/>
        <source>Captured complete firmware entry state before conventional GRUB EFI install: %1</source>
        <translation>Kaapattu täydellinen firmware sisääntulotila ennen tavanomaista GRUB EFI asennus: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1497"/>
        <source>SIMULATE/PREFLIGHT: GRUB EFI install target=%1 fs=%2 id=%3 mode=%4</source>
        <translation>SIMULATE/PREFLIGHT: GRUB EFI asenna kohde=%1 fs=%2 id=%3-tila=%4</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1498"/>
        <source>foreign-entry-added: %1</source>
        <translation>Suomeen lisätty: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1499"/>
        <source>foreign-entry-removed: %1</source>
        <translation>Ulkomailta poistuneet: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1500"/>
        <source>menu-entry-added: %1</source>
        <translation>Valikko lisätty: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1501"/>
        <source>GRUB menu entries added by the candidate: %1</source>
        <translation>GRUB valikko merkinnät lisätään ehdokas: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1502"/>
        <source>Using grub-mkconfig with an isolated output path for preflight.</source>
        <translation>grub-mkconfig:n käyttäminen erillisellä lähtöreitillä ennen lentoa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1503"/>
        <source>PASS: GRUB configuration regenerated and verified.</source>
        <translation>PASS: GRUB konfiguraatio regeneroitu ja tarkistettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1504"/>
        <source>SIMULATE/PREFLIGHT: generate GRUB configuration to temporary session output</source>
        <translation>SIMULATE/PREFLIGHT: luo GRUB-konfiguraatio tilapäiseen istuntolähtöön</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1505"/>
        <source>KNOWN ISSUE: GRUB trial failed on a stale mapper path; retrying once after compatibility alias correction.</source>
        <translation>TUNTEMATON NUMERO: GRUB-koe epäonnistui väljällä karttapolulla; yritetään uudelleen kerran yhteensopivuuden jälkeen alias korjaus.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1506"/>
        <source>Trial GRUB generation exit code: %1</source>
        <translation>Trial GRUB generation exit code: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1507"/>
        <source>PASS: GRUB trial configuration generated successfully.</source>
        <translation>PASS: GRUB kokeiluversio luotiin onnistuneesti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1508"/>
        <source>Backed up GRUB2 BIOS boot code (MBR, BIOS boot partition, i386-pc modules) to %1.</source>
        <translation>Varmistettu GRUB2 BIOS boot koodi (MBR, BIOS boot osio, i386-pc moduulit) %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1509"/>
        <source>NOTE: GRUB2 BIOS boot-code reinstall is only implemented for a mounted repair target; the running host stays config-only.</source>
        <translation>HUOMAUTUS: GRUB2 BIOS-käynnistyskoodi uudelleenasentaminen toteutetaan vain asennettu korjauskohde; käynnissä isäntä pysyy vain config-.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1510"/>
        <source>PASS: GRUB2 BIOS boot code reinstalled and verified (partition table byte-identical).</source>
        <translation>PASS: GRUB2 BIOS-käynnistyskoodi uudelleen asennettuna ja todennettuna (osittainen taulukko).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1511"/>
        <source>GRUB2 boot-code probe: MBR and BIOS boot partition contain a GRUB signature; config-only regeneration.</source>
        <translation>GRUB2-käynnistin: MBR ja BIOS-saappaan osio sisältää GRUB- allekirjoituksen; ainoastaan konfigin regeneraatio.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1512"/>
        <source>REPAIR: reinstall GRUB2 BIOS boot code (%1 --target=i386-pc --boot-directory=/boot --recheck %2)</source>
        <translation>KORJAUS: Asenna uudelleen GRUB2 BIOS- käynnistyskoodi (%1 --tavoite=i386-kpl --boot-kansio=/boot -- tarkista %2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1513"/>
        <source>Restored the GRUB2 BIOS boot code from %1.</source>
        <translation>Palautti GRUB2 BIOS-alkukoodin %1:stä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1514"/>
        <source>GRUB2 config-only reconciliation requested; the BIOS boot-code reinstall substage is skipped.</source>
        <translation>GRUB2:lta vaaditaan vain konfigurointia; BIOS-käynnistyskoodi uudelleenasennetaan alavaiheen ulkopuolelle.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1515"/>
        <source>UEFI firmware detected; the GRUB2 BIOS boot-code reinstall substage is not applicable.</source>
        <translation>UEFI-ohjelmisto havaittu; GRUB2 BIOS- käynnistyskoodi uudelleen asentaa alavaiheen ei sovelleta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1516"/>
        <source>Host boot-stack Arch EFI read-only preflight: PASS (%1, %2, id=%3)</source>
        <translation>Isäntä-pino Arch EFI luku vain ennen lentoa: PASS (%1, %2, id=%3)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1517"/>
        <source>Host boot-stack TUXEDO UKI read-only preflight: PASS (%1, %2)</source>
        <translation>Isäntäkäynnistys TUXEDO UKI luku vain ennen lentoa: PASS (%1, %2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1518"/>
        <source>Host command guard: firmware variables are read-only to package/kernel/vendor hooks; the helper owns explicit firmware registration.</source>
        <translation>Isännän komennon vartiointi: firmware-muuttujat luetaan vain paketti-/ydin-/koukkuihin; auttaja omistaa nimenomaisen firmware-rekisteröinnin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1519"/>
        <source>Creating host %1 firmware entry on %2 as &apos;%3&apos; (%4); existing host, repair, and foreign entries are untouched.</source>
        <translation>Luodaan isäntä %1 firmware merkintä %2 nimellä &apos;%3&apos; (%4); olemassa isäntä, korjaus, ja ulkomaiset merkinnät ovat koskemattomia.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1520"/>
        <source>PASS: running host default EFI entry is Boot%1 on %2.</source>
        <translation>PASS: käynnissä isäntä oletus EFI merkintä on Boot%1 %2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1521"/>
        <source>WARNING: host destination is group-writable; the copied files may be modified by the owning group: %1</source>
        <translation>VAROITUS: isäntäkohde on ryhmäkirjoitettava; kopioita voi muuttaa omistava ryhmä: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1522"/>
        <source>Host disk: %1</source>
        <translation>Käyttölevy: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1523"/>
        <source>Backed up running-host EFI loader files to %1 before the guarded reinstall.</source>
        <translation>Varmistettu käynnissä-host EFI lataaja tiedostot %1 ennen vartioitu uudelleen asentaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1524"/>
        <source>Canonical running-host EFI loader verified: %1 (%2) on %3.</source>
        <translation>Canonical juoksu-host EFI kuormain todennettu: %1 (%2) %3.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1525"/>
        <source>Host EFI read-only preflight: PASS (%1, %2, id=%3)</source>
        <translation>Isäntä EFI luku vain ennen lentoa: PASS (%1, %2, id=%3)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1526"/>
        <source>EFI repair completed; regenerating the running host GRUB fallback configuration.</source>
        <translation>EFI korjaus valmis; regenerointi käynnissä isäntä GRUB varakokoonpano.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1527"/>
        <source>Restored the running-host EFI loader files from %1.</source>
        <translation>Palautti ajo-isäntä EFI-lataajan tiedostot %1:stä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1528"/>
        <source>Host Alpine EFI-stub read-only preflight: PASS (%1, %2)</source>
        <translation>Isäntä Alpine EFI-stib luku vain ennen lentoa: PASS (%1, %2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1529"/>
        <source>Writable UEFI variables are unavailable; host maintenance will preserve files and BootOrder without firmware registration.</source>
        <translation>Kirjoitettavat UEFI-muuttujat eivät ole saatavilla; isäntähuolto säilyttää tiedostot ja BootSirder ilman firmware-rekisteröintiä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1530"/>
        <source>Host %1 entries %2 point to %3 on %4; keeping active entry Boot%5 and pruning the remaining duplicates.</source>
        <translation>Host %1 merkinnät %2 pisteen %3 %4; pitämällä aktiivinen merkintä Boot%5 ja karsimalla loput kaksoiskappaleet.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1531"/>
        <source>Host EFI System Partition: %1 (%2) %3</source>
        <translation>Isäntä EFI System Partition: %1 (%2) %3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1532"/>
        <source>Native running-host repair preflight: PASS</source>
        <translation>Native juoksu-host korjaus ennen lentoa: PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1533"/>
        <source>All requested running-host repair stages completed successfully.</source>
        <translation>Kaikki vaaditut juoksu-host korjausvaiheet suoritettu onnistuneesti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1534"/>
        <source>ROLLBACK: restored BootOrder=%1, but the firmware state differs from the pre-change capture:</source>
        <translation>ROLLBACK: palautettu Boot Order=%1, mutta firmware-tila eroaa ennen muutosta kaappaus:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1535"/>
        <source>CRITICAL: cannot detach the promoted host rollback mounts; refusing to rename Btrfs roots during recovery.</source>
        <translation>CRITICAINEN: ei voi irrottaa promoted isännän kaatumistelineitä; kieltäytyä nimeämästä Btrfs juuret uudelleen palautumisen aikana.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1536"/>
        <location filename="../src/MainWindow.cpp" line="1645"/>
        <source>CRITICAL: could not determine the preserved root subvolume ID.</source>
        <translation>KRIITINEN: ei voitu määrittää säilynyttä juuri-alavolyymiä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1537"/>
        <source>CRITICAL: could not move the failed host rollback candidate out of @; refusing to promote the preserved root.</source>
        <translation>CRIITIC: Epäonnistunutta isäntäkandidaattia ei voitu siirtää pois @:stä, koska se kieltäytyi edistämästä säilynyttä juuria.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1538"/>
        <source>CRITICAL: could not move the nested @/.snapshots child back into the preserved root.</source>
        <translation>KRIITINEN: pesittyä lasta ei voitu siirtää takaisin säilyneeseen juureen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1539"/>
        <source>CRITICAL: could not remount the Btrfs top-level filesystem read-write during host rollback recovery.</source>
        <translation>CRITICA: ei voitu asentaa uudelleen Btrfs-ylätason tiedostojärjestelmän luku-kirjoitusta isäntäkääntäjän palautuksen aikana.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1540"/>
        <location filename="../src/MainWindow.cpp" line="1648"/>
        <source>CRITICAL: could not restore the previous Btrfs default subvolume.</source>
        <translation>CRITICAINEN: edellistä Btrfs-oletusosamäärää ei voitu palauttaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1541"/>
        <source>RUNNING-HOST SNAPSHOT ROLLBACK COMPLETE</source>
        <translation>RUNNING-Host snapshot rollback valmis</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1542"/>
        <source>ROLLBACK: created entry Boot%1 is active (BootCurrent/BootNext); it is retained but no longer promoted.</source>
        <translation>ROllBACK: luotu merkintä Boot%1 on aktiivinen (BootCurrent/BootNext); se säilytetään, mutta ei enää edistä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1543"/>
        <location filename="../src/MainWindow.cpp" line="1649"/>
        <source>Creating writable rollback candidate %1</source>
        <translation>Luodaan kirjoitettava vastakanteen ehdokas %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1544"/>
        <source>Host rollback default-subvolume update failed; automatically restoring the preserved root.</source>
        <translation>Host-rollback oletus-subvolyymi päivitys epäonnistui; automaattisesti palauttaa säilynyt root.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1545"/>
        <source>Pre-rollback boot artifact fingerprints: fstab=%1 grub=%2 cmdline=%3 uki=%4</source>
        <translation>Esi-rollback-saappaan esineen sormenjäljet: fstab=%1-murska=%2 cmdline=%3 uki=%4</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1546"/>
        <source>Migrating nested @/.snapshots child subvolume into the promoted root.</source>
        <translation>Muuttaa pesitty @/.snapshots lapsen osavolyymi ylennetty juuri.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1547"/>
        <source>Host rollback candidate mount/preflight failed; automatically restoring the preserved root.</source>
        <translation>Isännän nousukandidaatin asennus/ennakko epäonnistui; säilynyt juuri palautuu automaattisesti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1548"/>
        <source>PASS: the original @ and its boot stack were restored automatically. Failed rollback candidate retained as %1.</source>
        <translation>PASS: alkuperäinen @ ja sen boot pino palautettiin automaattisesti. %1:nä pidetyssä varaehdokkaassa epäonnistuttiin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1549"/>
        <source>CRITICAL: the original @ was restored, but its boot-stack reconciliation also failed. Manual boot repair is required before reboot.</source>
        <translation>CRIITIC: alkuperäinen @ palautettiin, mutta sen boot-stack sovittelu epäonnistui myös. Manuaalinen boot korjaus vaaditaan ennen uudelleenkäynnistystä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1550"/>
        <location filename="../src/MainWindow.cpp" line="1670"/>
        <source>CRITICAL: preserved root %1 is missing; refusing to move the active rollback candidate.</source>
        <translation>CRIITIC: säilynyt juuri %1 puuttuu; kieltäytyy siirtämästä aktiivinen rollback ehdokas.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1551"/>
        <location filename="../src/MainWindow.cpp" line="1705"/>
        <source>Previous root retained as: %1</source>
        <translation>Edellinen juuri säilytetään: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1552"/>
        <source>Promoted root: @ (subvolume ID %1); the running system keeps the previous root until reboot.</source>
        <translation>Edistetty root: @ (subvolume ID %1); käynnissä järjestelmä pitää edellisen root kunnes uudelleenkäynnistys.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1553"/>
        <source>HOST ROLLBACK RECOVERY: restoring the preserved pre-rollback @.</source>
        <translation>HOST ROllBACK REKISTERÖINTI: säilyttää pre-rollback @.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1554"/>
        <source>ROLLBACK: restored BootOrder=%1 and removed created entry Boot%2; firmware state equals the pre-change capture.</source>
        <translation>ROLLBACK: palautettu Saapasjärjestys=%1 ja poistettu luotu merkintä Boot%2; firmware tila on yhtä kuin ennen muutosta kaappaus.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1555"/>
        <source>Running-host rollback selected: snapshot %1.</source>
        <translation>Running-host rockback valittu: tilannekuva %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1556"/>
        <source>Rollback source remains unchanged; the running host keeps the current root until reboot.</source>
        <translation>Rollback-lähde pysyy muuttumattomana; käynnissä oleva isäntä pitää nykyisen juuren käynnistymiseen asti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1557"/>
        <source>Selected rollback target: %1</source>
        <translation>Valittu varakohde: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1558"/>
        <source>Host rollback candidate failed post-switch validation/boot reconciliation; automatically restoring the preserved root.</source>
        <translation>Isäntärock-ehdokas epäonnistui kytkimen jälkeisessä validoinnissa/saappaan täsmäyttämisessä; säilynyt juuri palautuu automaattisesti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1559"/>
        <source>Host root component: %1 (%2)</source>
        <translation>Isännän juurikomponentti: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1560"/>
        <source>Host root mount: / (subvolume=%1)</source>
        <translation>Isännän juuriliitos: / (osatilavuus=%1)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1561"/>
        <source>FAIL: Running-host shell command was cancelled at an interactive prompt.</source>
        <translation>Suoritus-ohjain komento peruttiin interaktiivisella käskyllä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1562"/>
        <source>Running-host shell exit code: %1</source>
        <translation>Running-host kuori exit koodi: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1563"/>
        <source>FAIL: Running-host shell command (exit code %1)</source>
        <translation>FAIL: Running-host-shell-komento (poistumiskoodi %1)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1564"/>
        <source>PASS: Running-host shell command</source>
        <translation>PASS: Suoritus-ohjain komento</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1565"/>
        <source>Host snapshot inventory skipped: %1 is not Btrfs.</source>
        <translation>Isäntäkuvan inventaario ohitettu: %1 ei ole Btrfs.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1566"/>
        <source>Host TUXEDO UKI firmware entry already present: Boot%1 on %2.</source>
        <translation>Isäntä TUXEDO UKI firmware-merkintä on jo läsnä: Boot%1 on %2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1567"/>
        <source>Host TUXEDO UKI file is present on %1, but firmware variables are not writable; host registration was not changed.</source>
        <translation>Isäntä TUXEDO UKI tiedosto on läsnä %1, mutta firmware muuttujat eivät ole kirjoitettavia; isäntä rekisteröinti ei muuttunut.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1568"/>
        <source>Host TUXEDO UKI read-only preflight: PASS (%1, %2)</source>
        <translation>Isäntä TUXEDO UKI luku vain ennen lentoa: PASS (%1, %2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1569"/>
        <source>PASS: host TUXEDO UKI firmware entry restored as Boot%1 on %2.</source>
        <translation>PASS: isäntä TUXEDO UKI firmware merkintä palautettu Boot%1 %2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1570"/>
        <source>Restoring missing host TUXEDO UKI firmware entry on %1 as &apos;%2&apos;; existing host, repair, and foreign entries are untouched.</source>
        <translation>Palauttaa puuttuvat isäntä TUXEDO UKI firmware merkintä %1 &apos;%2&apos;; olemassa isäntä, korjaus, ja ulkomaiset merkinnät ovat koskemattomia.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1571"/>
        <source>Running-host validation complete; no host files were changed.</source>
        <translation>Running-host validointi valmis; mitään isäntätiedostoja ei muutettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1572"/>
        <source>Running-host validation summary</source>
        <translation>Running-host validation -tiivistelmä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1573"/>
        <source>KNOWN ISSUE: %1 is missing; creating it instead of attempting an update.</source>
        <translation>TIEDON NUMERO: %1 puuttuu; luoda sen sijaan yrittää päivittää.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1574"/>
        <source>PASS: initramfs %1 verified by mkinitfs -l build-input listing</source>
        <translation>PASS: initramfs %1 todentanut mkinitfs -l sisäänrakennettu listaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1575"/>
        <source>SIMULATE/PREFLIGHT: initramfs generation for %1 installed kernel(s)</source>
        <translation>SIMULATE/PREFLIGHT: initramfs-tuotanto asennetuille %1-ytimille</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1576"/>
        <source>PASS: trial initramfs build for %1</source>
        <translation>PASS: kokeilu initramfs rakentaa %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1577"/>
        <source>PASS: initramfs verified for %1</source>
        <translation>PASS: initramfs todennettu %1:lle</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1578"/>
        <source>PASS: initramfs %1 verified by zcat/cpio listing</source>
        <translation>PASS: initramfs %1 todennettu zcat/cpio listaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1579"/>
        <source>KNOWN ISSUE: initrd.img-%1 is missing; creating it instead of attempting an update.</source>
        <translation>TIEDON NUMERO: initrd.img-%1 puuttuu; luoda sen sijaan yrittää päivittää.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1580"/>
        <source>%1 lives on the root filesystem; no separate mount required</source>
        <translation>%1 elää juuritiedostojärjestelmässä; erillistä asennusta ei tarvita</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1581"/>
        <source>PASS: lsinitcpio verified %1 Arch initramfs image(s)</source>
        <translation>PASS: lsinitcpio todennettu %1 Arch initramfs kuvat</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1582"/>
        <source>PASS: lsinitramfs verified /boot/initrd.img-%1</source>
        <translation>PASS: lsinitramfs todennettu /boot/initrd.img-%1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1583"/>
        <source>PASS: lsinitrd verified %1</source>
        <translation>PASS: Lsinitrd todennettu %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1584"/>
        <source>LUKS target is already unlocked by existing mapper: %1</source>
        <translation>LUKS-kohde on jo avattu olemassa olevan mapper: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1585"/>
        <source>Mapper name: %1</source>
        <translation>Mapper-nimi: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1586"/>
        <source>LUKS unlock complete. The mapper remains open for this recovery session.</source>
        <translation>LUKS-lukko valmis. Mapper on avoinna toipumisistuntoa varten.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1587"/>
        <source>Unlocking LUKS target %1</source>
        <translation>Avataan LUKS-tavoite %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1588"/>
        <source>Mandatory safety preflight: PASS</source>
        <translation>Pakollinen turvallisuus ennen lentoa: PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1589"/>
        <source>Mapper compatibility: %1 already resolves to %2</source>
        <translation>Mapper yhteensopivuus: %1 jo ratkaisee %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1590"/>
        <source>Mapper compatibility: temporary %1 -&gt; %2</source>
        <translation>Mapper yhteensopivuus: väliaikainen %1 -&gt; %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1591"/>
        <source>SIMULATE/PREFLIGHT: mkinitcpio trial builds for %1 installed kernel(s)</source>
        <translation>SIMULATE/PREFLIGHT: mkinitcpio kokeilu rakentaa %1 asennettujen ytimet(s)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1592"/>
        <source>PASS: trial mkinitcpio build for %1</source>
        <translation>PASS: kokeilu mkinitcpio rakentaa %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1593"/>
        <source>SIMULATE/PREFLIGHT: mkinitfs trial builds for %1 installed kernel(s)</source>
        <translation>SIMULATE/PREFLIGHT: mkinitfs-kokeilu rakentaa %1-asennettuun ytimeen(s)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1594"/>
        <source>PASS: trial mkinitfs build for %1</source>
        <translation>PASS: tutkimus mkinitfs rakentaa %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1595"/>
        <source>Mounted target ESP discovered by GPT type: %1 at /boot/efi (%2)</source>
        <translation>Asennettu kohde ESP löydetty GPT-tyypillä: %1 at /boot/efi (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1596"/>
        <source>Mounted target %1 from %2 (%3)</source>
        <translation>Asennettu kohde %1 %2:stä (%3)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1597"/>
        <source>Mounting target %1 from %2 (%3)</source>
        <translation>Asennuskohde %1 %2:stä (%3)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1598"/>
        <source>Mounting target Btrfs fstab subvolumes (%1)</source>
        <translation>Asennuskohteet Btrfs fstabin alatilavuudet (%1)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1599"/>
        <source>Mounting target %1 from %2</source>
        <translation>Asennuskohde %1 %2:stä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1600"/>
        <source>No TUXEDO UKI builder detected; retaining the distribution&apos;s existing EFI layout.</source>
        <translation>TUXEDO UKI -rakentajaa ei ole havaittu; jakelun nykyinen EFI-asettelu säilytetään.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1601"/>
        <source>FAIL: ntfsfix could not fully repair %1 (exit code %2); run Windows chkdsk /f for a real NTFS repair.</source>
        <translation>FAIL: ntffix ei pystynyt korjaamaan %1:ää (poistumiskoodi %2); suorita Windows chkdsk /f todellista NTFS korjausta varten.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1602"/>
        <source>NOTE: ntfsfix only clears the NTFS dirty state; Windows chkdsk /f is required for a real NTFS repair.</source>
        <translation>HUOMAUTUS: ntffix puhdistaa vain NTFS:n likaisen tilan; Windows chkdsk /f tarvitaan todelliseen NTFS-korjaukseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1603"/>
        <source>Ownership validation: %1:%2 maps to %3:%4 on both sides; preserving numeric ownership</source>
        <translation>Omistajuuden validointi: %1:%2 kartat %3:%4 molemmin puolin; numeerisen omistusoikeuden säilyttäminen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1604"/>
        <source>Ownership validation: source %1:%2 (%3:%4) does not map identically on the destination side; using destination owner %5:%6</source>
        <translation>Omistajan validointi: lähde %1:%2 (%3:%4) ei kartalla identtisesti kohdepuolella; käyttäen kohdeomistajaa %5:%6</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1605"/>
        <source>Ownership policy: %1</source>
        <translation>Omistusoikeus: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1606"/>
        <source>Package backend %1 did not report a change status; treating the stage as changed.</source>
        <translation>Paketin taustaosa %1 ei ilmoittanut muutostilaa; käsittely vaiheessa muuttunut.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1607"/>
        <source>SKIP: package backend %1 is not runnable for stage &apos;%2&apos;: %3</source>
        <translation>SKIP: Paketin taustaosa %1 ei ole käytettävissä vaiheessa &quot;%2&quot;: %3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1608"/>
        <source>Package backend %1 change status: %2</source>
        <translation>Paketin taustaosan %1 muutostila: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1609"/>
        <source>Package stage &apos;%1&apos;: running %2 runnable backend(s): %3</source>
        <translation>Pakettivaihe &apos;%1&apos;: käynnissä %2-juoksutettava taustaosa [s]: %3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1610"/>
        <source>PackageKit daemon is running; it is only a conflict while it holds package-manager locks or spawns apt/dpkg.</source>
        <translation>PakettiKit Daemon on käynnissä; se on vain ristiriita, kun sillä on paketti-manageri lukot tai spawns apt/dpkg.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1611"/>
        <source>Arch pacman transaction completed successfully despite mirror fallback.</source>
        <translation>Arch pacman -tapahtuma suoritettu onnistuneesti huolimatta peilivarauksesta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1612"/>
        <source>WARN: pacman encountered %1 recoverable mirror retrieval failure(s).</source>
        <translation>WARN: pacman havaitsi %1:n palautuvan peilin hakuvirheen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1613"/>
        <source>PASS: Arch pacman transaction preflight resolved without removals.</source>
        <translation>PASS: Arch pacman tapahtuma ennen lentoa ratkaistu ilman poistoja.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1614"/>
        <source>REFUSED: pacman preflight proposes %1 packages (safety limit: 1000).</source>
        <translation>EPÄÄMINEN: pacman:ssä ennen lentoa ehdotetaan %1-paketteja (turvallisuusraja 1000).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1615"/>
        <source>REFUSED: pacman preflight reported repository, download or transaction integrity errors.</source>
        <translation>EPÄÄMINEN: pacman ennen lentoa raportoitu arkisto, lataus tai tapahtuma eheys virheet.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1616"/>
        <source>REFUSED: pacman preflight proposes removals or unresolved dependencies.</source>
        <translation>EPÄÄMINEN: pacman ennen lentoa ehdottaa poistoja tai ratkaisemattomia riippuvuuksia.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1617"/>
        <source>Arch pacman transaction sandbox prepared under %1; the target package database will not be used for preflight.</source>
        <translation>Arch pacman tapahtumahiekkalaatikko valmistettu %1; kohdepaketti tietokantaa ei käytetä ennen lentoa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1618"/>
        <source>SIMULATE/PREFLIGHT: full Arch pacman transaction (sandboxed database/cache; no target packages will be changed)</source>
        <translation>SIMULATE/PREFLIGHT: koko Arch pacman-tapahtuma (Sandboxed database/cache; kohdepaketteja ei muuteta)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1619"/>
        <source>PASS: dnf5 metadata cache refreshed.</source>
        <translation>PASS: dnf5-metadatavälimuisti päivitetty.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1620"/>
        <source>PASS: %1</source>
        <translation>PASS: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1621"/>
        <source>PASS: %1 completed through one full pacman transaction.</source>
        <translation>PASS: %1 suoritettu yhdellä täydellä pacman-tapahtumalla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1622"/>
        <source>PASS: Refresh package metadata</source>
        <translation>PASS: Päivitä paketin metatiedot</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1623"/>
        <source>PASS: Refresh package metadata (package lists byte-identical; no repository index was fetched)</source>
        <translation>PASS: Päivitä pakettien metatiedot (pakettiluettelot byte-identiteetti; arkistoindeksiä ei noudettu)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1624"/>
        <source>Host package-manager concurrency gate: PASS</source>
        <translation>Isäntäpaketti-hallintavaluutta portti: PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1625"/>
        <source>Post-upgrade dpkg audit:</source>
        <translation>Korkea-asteen jälkeinen dpkg-tarkastus:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1626"/>
        <source>PREVIEW COMPLETE — no files were changed.</source>
        <translation>ENNAKKO TÄYDELLINEN</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1627"/>
        <source>PREVIEW: target destination directory would be created: %1</source>
        <translation>ENNAKKO: kohde kohdehakemisto luodaan: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1628"/>
        <source>Protected host check: PASS</source>
        <translation>Suojattu isäntätarkistus: PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1629"/>
        <source>Scheduling running-host reboot through the system reboot command.</source>
        <translation>Käynnistetään järjestelmä uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1630"/>
        <source>Scheduling running-host reboot through OpenRC.</source>
        <translation>Suunnitelmia käynnissä-host käynnistää uudelleen OpenRC.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1631"/>
        <source>Scheduling running-host reboot through systemd.</source>
        <translation>Aikataulun mukaan isännät käynnistyvät uudelleen järjestelmällisesti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1632"/>
        <source>WARNING: refusing an unsafe target Btrfs mountpoint (not absolute): %1</source>
        <translation>VAROITUS: vaarallisen kohteen Btrfs asennuspiste (ei absoluuttinen): %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1633"/>
        <source>WARNING: refusing an unsafe target Btrfs subvolume mount path: %1</source>
        <translation>VAROITUS: vaarallisen kohteen Btrfs alavolyymi asennus polku: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1634"/>
        <source>WARNING: refusing an unsafe target %1 mount path: %2</source>
        <translation>VAROITUS: epäturvallisen kohteen %1 asennuspolku: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1635"/>
        <source>Remounting confirmed target filesystem read-write for file copy</source>
        <translation>Luodaan uudelleen vahvistettu kohdetiedostojärjestelmän lukukirjoitus tiedostokopiota varten</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1636"/>
        <source>Remounting confirmed target root read-write</source>
        <translation>Palautetaan vahvistettu kohde juurilukukirjoitus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1637"/>
        <source>Remounting target data filesystem %1 read-write</source>
        <translation>Kohteen datatiedostojärjestelmän muuttaminen %1 lukukirjoitus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1638"/>
        <source>Remounting target %1 read-write</source>
        <translation>Korjataan kohdetta %1 lukukirjoitus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1639"/>
        <source>All requested repair stages completed successfully.</source>
        <translation>Kaikki vaaditut korjausvaiheet valmistuivat onnistuneesti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1640"/>
        <source>Rollback boot-stack reconciliation: using simulation-first adaptive component workflows.</source>
        <translation>Rollback boot-stack täsmäytys: käyttämällä simulointi-ensimmäinen adaptiivinen komponentti työnkulkuja.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1641"/>
        <source>Rollback candidate default-subvolume update failed; automatically restoring the preserved root.</source>
        <translation>Rollback ehdokas oletus-subvolyymi päivitys epäonnistui; automaattisesti palauttaa säilynyt juuri.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1642"/>
        <source>Rollback candidate mount/preflight failed; automatically restoring the preserved root.</source>
        <translation>Rollback-ehdokkaan asennus/valmistelu epäonnistui; säilyneen juuren palauttaminen automaattisesti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1643"/>
        <source>Rollback candidate failed post-switch validation/boot reconciliation; automatically restoring the preserved root.</source>
        <translation>Rollback-ehdokas epäonnistui kytkimen jälkeisessä validoinnissa/saappaan täsmäyttämisessä; säilynyt juuri palautuu automaattisesti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1644"/>
        <source>CRITICAL: cannot detach promoted rollback mounts; refusing to rename Btrfs roots during recovery.</source>
        <translation>CRITICA: ei voi irrottaa promotoituja kaatumistelineitä, kieltäytyä nimeämästä Btrfs-juuria uudelleen palautumisen aikana.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1646"/>
        <source>CRITICAL: could not move failed rollback candidate out of @; refusing to promote the preserved root.</source>
        <translation>CRITICAINEN: ei voinut siirtää epäonnistunut rockback ehdokas pois @; kieltäytyä edistää säilynyt juuri.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1647"/>
        <source>CRITICAL: could not remount the Btrfs top-level filesystem read-write during rollback recovery.</source>
        <translation>CRITICA: Btrfs-ylätason tiedostojärjestelmän luku-kirjoitusta ei voitu asentaa uudelleen palautuksen aikana.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1650"/>
        <source>CRITICAL ROLLBACK FAILURE: could not restore original @ after default-subvolume failure. Do not reboot.</source>
        <translation>KRIITINEN ROLLBACK FILURE: ei voitu palauttaa alkuperäistä @ jälkeen oletus-subvolyymi vika. Älä käynnistä uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1651"/>
        <source>CRITICAL ROLLBACK FAILURE: automatic restoration was incomplete. Do not reboot until Btrfs/boot state is inspected manually.</source>
        <translation>KRIITINEN PYSÄKÖINTI: Automaattinen ennallistaminen oli kesken. Älä käynnistä uudelleen ennen kuin Btrfs/boot-tila on tarkastettu käsin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1652"/>
        <source>CRITICAL ROLLBACK FAILURE: could not restore original @ after candidate mount failure. Do not reboot.</source>
        <translation>CRIITIC ROLLBACK FILURE: ei voinut palauttaa alkuperäistä @ jälkeen ehdokas asentaa vikaa. Älä käynnistä uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1653"/>
        <source>Detaching target mounts before atomic @ name switch.</source>
        <translation>Irrotetaan kohteen liitokset ennen atomi@ nimikytkintä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1654"/>
        <source>ROLLBACK FAILED SAFELY: original @ restored; failed candidate retained as %1.</source>
        <translation>ROLLBACK PYSYVÄSTI: alkuperäinen @ palautettu; epäonnistunut ehdokas valittu %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1655"/>
        <source>ROLLBACK FAILED SAFELY: original @ restored after default-subvolume failure; failed candidate retained as %1.</source>
        <translation>ROLLBACK HALLINNOLLISESTI: alkuperäinen @ palautettu oletus-subvolyymi vika; epäonnistui ehdokas säilyttää %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1656"/>
        <source>ROLLBACK FAILED SAFELY: original @ restored after candidate mount failure; failed candidate retained as %1.</source>
        <translation>ROLLBACK HALLINNOLLISESTI: alkuperäinen @ palautettu jälkeen ehdokas asentaa vika; epäonnistui ehdokas säilyttää %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1657"/>
        <source>Rollback kernel %1 is missing initramfs before rebuild; update-initramfs will be asked to regenerate it.</source>
        <translation>Rollback-ytimen %1 puuttuu initramfs ennen jälleenrakentamista; update-initramfs pyydetään uudistamaan se.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1658"/>
        <source>Rollback kernel %1 has no matching modules directory.</source>
        <translation>Rollback-ytimen %1:ssä ei ole vastaavaa moduulihakemistoa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1659"/>
        <source>Rollback kernel pair before rebuild: %1</source>
        <translation>Rollback-ytimen pari ennen rakentamista: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1660"/>
        <source>Rollback kernel pair before rebuild: %1 (mkinitcpio flavor naming)</source>
        <translation>Rollback-ytimen pari ennen jälleenrakentamista: %1 (mkinitcpio makunimitys)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1661"/>
        <source>PASS: original @ and its boot stack were restored automatically. Failed rollback candidate retained as %1.</source>
        <translation>PASS: alkuperäinen @ ja sen boot pino palautettiin automaattisesti. %1:nä pidetyssä varaehdokkaassa epäonnistuttiin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1662"/>
        <source>CRITICAL: original @ was restored, but its boot-stack reconciliation also failed. Manual boot repair is required before reboot.</source>
        <translation>CRIITIC: alkuperäinen @ palautettiin, mutta sen boot-stack sovittelu epäonnistui myös. Manuaalinen boot korjaus vaaditaan ennen uudelleenkäynnistystä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1663"/>
        <source>Rollback preflight: snapshot root fstab is compatible with promoted @.</source>
        <translation>Rollback ennen lentoa: tilannekuva root fstab on yhteensopiva promoted @.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1664"/>
        <source>Rollback preflight: snapshot fstab has no root (/) entry.</source>
        <translation>Rollback ennen lentoa: kuvassa fstab ei ole juuria (/) merkintä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1665"/>
        <source>Rollback preflight: snapshot root fstab type is &apos;%1&apos;, not btrfs.</source>
        <translation>Rollback ennen lentoa: tilannekuva root fstab tyyppi on &apos;%1&apos;, ei btrfs.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1666"/>
        <source>Rollback preflight: snapshot fstab expects root subvolume &apos;%1&apos;, but transactional rollback promotes the selected snapshot to @.</source>
        <translation>Rollback ennen lentoa: tilannekuva fstab odottaa root subvolume &apos;%1&apos;, mutta transaktion palautus edistää valitun tilannekuvan @.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1667"/>
        <source>Rollback preflight: snapshot / fstab source cannot be resolved: %1</source>
        <translation>Perääntyminen ennen lentoa: tilannekuvaa / fstab-lähdettä ei voida ratkaista: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1668"/>
        <source>Rollback preflight: snapshot / fstab root source has an unsupported format: %1</source>
        <translation>Rollback ennen lentoa: kuva / ftab juurilähde on tueton muoto: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1669"/>
        <source>Rollback preflight: snapshot / fstab source resolves to %1 rather than selected root %2.</source>
        <translation>Rollback ennen lentoa: kuva / fstab-lähde ratkaisee %1:n eikä valitun juuri %2:n.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1671"/>
        <source>Preserved root name: %1</source>
        <translation>Säilötty juurinimi: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1672"/>
        <source>PASS: promoted rollback root and boot stack validated.</source>
        <translation>PASS: edistää rockback root ja boot pino validoitu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1673"/>
        <source>ROLLBACK RECOVERY: restoring preserved pre-rollback @.</source>
        <translation>ROLLBACK ELVYTYS: säilyneen pre-rollback @.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1674"/>
        <source>Transactional rollback selected: snapshot %1 (%2).</source>
        <translation>Transaktiokäännös valittu: valokuva %1 (%2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1675"/>
        <source>Rollback source snapshot will remain unchanged.</source>
        <translation>Rollback-lähdekuva pysyy ennallaan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1676"/>
        <source>Root component: %1 (%2)</source>
        <translation>Juurikomponentti: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1677"/>
        <source>Root component fallback: selected component %1 (%2) lacks /etc/os-release; resolved %3 (%4) from %5.</source>
        <translation>Juurikomponentin varaosa: valitusta %1:stä (%2) puuttuu /etc/os-release; ratkaistu %3 (%4) %5:stä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1678"/>
        <source>RPM missing-file repair: reinstalling %1 package(s) with missing files: %2</source>
        <translation>RPM puuttuu-file korjaus: asentaminen %1 paketti(s) puuttuvat tiedostot: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1679"/>
        <source>RPM missing-file detection: no missing package files; recording read-only dnf5 dependency-check evidence.</source>
        <translation>RPM puuttuu-tiedoston havaitseminen: ei puuttuvia pakettitiedostoja; tallennetaan vain lukea dnf5 huoltotodisteita.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1680"/>
        <source>rpm preflight version: %1</source>
        <translation>Rpm esilentoversio: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1681"/>
        <source>Running-host identity and boot-mount check: PASS</source>
        <translation>Running-host identiteetti ja boot-mount tarkistaa: PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1682"/>
        <source>Security: host-to-repair copies keep -aHAX (modes, ownership and xattrs are copied from the trusted host source).</source>
        <translation>Turvallisuus: isäntä-korjaus kopiot pitää -aHAX (tilat, omistajuus ja xatters kopioidaan luotettava isäntä lähde).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1683"/>
        <source>Security: setuid/setgid bits and file capabilities are removed on repair-to-host copies.</source>
        <translation>Turvallisuus: setuid/setgid-bitit ja tiedoston ominaisuudet poistetaan korjauksesta isännöiksi -kopioilla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1684"/>
        <source>Selected EFI System Partition: %1 (%2) mounted at %3</source>
        <translation>Valittu EFI System Partition: %1 (%2) asennettuna %3:lle</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1685"/>
        <source>SELinux enforcement: %1</source>
        <translation>Selinuxin valvonta: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1686"/>
        <source>SELinux status: %1</source>
        <translation>SELinux-status: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1687"/>
        <source>SELinux status: sestatus/getenforce are not installed in the target; recorded as unavailable.</source>
        <translation>SELinux status: sestatus/getenforce ei ole asennettu kohteeseen; tallennettu ei saatavilla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1688"/>
        <source>========================================</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1689"/>
        <source>Session log was NOT appended into the target: unsafe target log path.</source>
        <translation>Istunnon lokia EI ole liitetty kohteeseen: vaarallinen kohdelokin polku.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1690"/>
        <source>SIMULATE: apk %1 --simulate (no packages will be changed)</source>
        <translation>SIMULATE: apk %1 -- simuloidaan (paketteja ei muuteta)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1691"/>
        <source>SIMULATE: apt-get %1 (no packages will be changed)</source>
        <translation>SIMULATE: apt-get %1 (paketteja ei muuteta)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1692"/>
        <source>SIMULATE CORRECTION: %1</source>
        <translation>YKSINKERTAINEN KORJAUS: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1693"/>
        <source>SIMULATE: dnf5 %1 --assumeno (no packages will be changed)</source>
        <translation>SIMULATE: dnf5 %1 --assumeno (paketteja ei muuteta)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1694"/>
        <source>SIMULATE: apt-get --fix-broken install (no packages will be changed)</source>
        <translation>SIMULATE: apt-get --fix-broken install (paketteja ei muuteta)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1695"/>
        <source>Simulation exit code for &apos;%1&apos;: %2</source>
        <translation>&quot;%1&quot;:n simulointilähdön koodi: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1696"/>
        <source>WARNING: skipping unresolved Btrfs fstab entry %1 -&gt; %2</source>
        <translation>VAROITUS: ohitetaan ratkaisematon Btrfs fstab merkintä %1 -&gt; %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1697"/>
        <source>WARNING: skipping unresolved data fstab entry %1 -&gt; %2</source>
        <translation>VAROITUS: ratkaisemattomien tietojen ohittaminen fstab-tietue %1 -&gt; %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1698"/>
        <source>Target has snapper apt hooks; guarding the chroot shell against snapshots (temporary, read-only)</source>
        <translation>Kohde on napsija apt koukkuja; vartioi Chroot kuori vastaan valokuvia (tilapäinen, luku-vain)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1699"/>
        <source>WARNING: could not bind the read-only snapper guard over %1; the chroot shell continues without the snapshot kill-switch.</source>
        <translation>VAROITUS: %1:n päällä ei voitu sitoa vain lukunapsijasuojaa; ruskokuori jatkuu ilman kuvan tappokytkintä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1700"/>
        <source>Active writable root: @ (subvolume ID %1)</source>
        <translation>Aktiivinen kirjoitettava juuri: @ (subvolume ID %1)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1701"/>
        <source>Initramfs/UKI/GRUB reconciliation: PASS</source>
        <translation>Initramfs/UKI/GRUB täsmäytys: PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1702"/>
        <source>Btrfs default subvolume now points to the promoted @.</source>
        <translation>Btrfs:n oletusalivolyymi osoittaa nyt ylennettyyn @.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1703"/>
        <source>Snapshot inventory complete: %1 snapshot(s) found.</source>
        <translation>Snapshot-inventaario valmis: %1-kuvake [s] löytyi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1704"/>
        <source>Snapshot inventory skipped: %1 is not Btrfs.</source>
        <translation>Snapshot inventaario ohitettu: %1 ei ole Btrfs.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1706"/>
        <source>SNAPSHOT ROLLBACK COMPLETE</source>
        <translation>PALKKAUS PELASTETTU</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1707"/>
        <source>Selected snapshot: %1</source>
        <translation>Valittu kuvakuva: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1708"/>
        <source>Source snapshot retained unchanged.</source>
        <translation>Lähdekuva säilyi ennallaan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1709"/>
        <source>Sources: %1</source>
        <translation>Lähteet: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1710"/>
        <source>Target /dev: private writable tmpfs populated from the recovery host with a private devpts at /dev/pts; the running host /dev and its ptys are not modified or leaked.</source>
        <translation>Kohde /dev: Yksityiset kirjoitettavat tmpf:t, jotka on asutettu palautusisännästä yksityisellä devptillä osoitteessa /dev/pts; käynnissä oleva isäntä /dev ja sen ptyt eivät muutu tai vuoda.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1711"/>
        <source>Target disk: %1</source>
        <translation>Kohdelevy: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1712"/>
        <source>Target root mount: %1 (subvolume=%2)</source>
        <translation>Kohde juuriliitos: %1 (subvolyymi=%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1713"/>
        <source>TRY exit code: %1 (%2)</source>
        <translation>Kokeile poistumiskoodi: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1714"/>
        <source>TRY: %1</source>
        <translation>Kokeile: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1715"/>
        <source>PASS: TUXEDO UKI root/LUKS/subvolume binding verified.</source>
        <translation>PASS: TUXEDO UKI root/LUKS/subvolyymisitoutuminen todennettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1716"/>
        <source>Captured complete firmware entry state before UKI rebuild: %1</source>
        <translation>Kaapattu täydellinen firmware sisääntulotila ennen UKI uudelleen: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1717"/>
        <source>TUXEDO UKI cmdline: %1</source>
        <translation>TUXEDO UKI cmdline: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1718"/>
        <source>TUXEDO UKI embedded kernel: %1</source>
        <translation>TUXEDO UKI sulautettu ydin: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1719"/>
        <source>PASS: existing TUXEDO UKI validated for %1 on selected ESP %2; no replacement image was needed.</source>
        <translation>PASS: nykyinen TUXEDO UKI validoitu %1 valitulle ESP %2:lle; korvaavaa kuvaa ei tarvittu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1720"/>
        <source>KNOWN ISSUE: UKI kernel is stale; vendor UKI rebuild will correct it.</source>
        <translation>TIEDON NUMERO: UKI ydin on kulunut; toimittaja UKI jälleenrakentaminen korjaa sen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1721"/>
        <source>TUXEDO UKI layout detected; simulating/prereflighting the vendor boot path before rebuild.</source>
        <translation>TUXEDO UKI:n ulkoasu havaittu; simuloidaan/esitellään myyjän käynnistyspolkua ennen jälleenrakentamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1722"/>
        <source>KNOWN ISSUE: newest TUXEDO kernel %1 has no matching initramfs; rebuilding initramfs before UKI.</source>
        <translation>TIEDON NUMERO: uusin TUXEDO ydin %1 ei ole yhteensopiva initramfs; jälleenrakentaminen initramfs ennen UKI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1723"/>
        <source>Writable UEFI variables/efibootmgr are unavailable; UKI file rebuild will proceed without BootOrder restoration.</source>
        <translation>Kirjoitettavat UEFI-muuttujat / efibootmgr eivät ole käytettävissä; UKI-tiedoston uudelleenrakentaminen jatkuu ilman BootSerderin palauttamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1724"/>
        <source>WARNING: objcopy could not inspect the rebuilt UKI .uname section.</source>
        <translation>VAROITUS: objcopy ei voinut tarkastaa uudelleen rakennettu UKI .uname osa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1725"/>
        <source>UKI preflight: current embedded kernel=%1; target newest kernel=%2</source>
        <translation>UKI ennen lentoa: nykyinen upotettu ydin=%1; kohde uusin ydin=%2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1726"/>
        <source>PASS: TUXEDO UKI preflight prerequisites are satisfied.</source>
        <translation>PASS: TUXEDO UKI ennen lentoa edellytykset täyttyvät.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1727"/>
        <source>PASS: TUXEDO UKI rebuilt for %1 on selected ESP %2</source>
        <translation>PASS: TUXEDO UKI uudelleenrakennettu %1 valitulle ESP %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1728"/>
        <source>TUXEDO UKI repair read-only preflight: PASS (%1, %2)</source>
        <translation>TUXEDO UKI korjaus luku vain ennen lentoa: PASS (%1, %2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1729"/>
        <source>PASS: vendor UKI command produced a changed TUX.EFI image.</source>
        <translation>PASS: Myyjä UKI komento tuotti muuttunut TUX.EFI kuva.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1730"/>
        <source>WARNING: vendor UKI command completed without changing TUX.EFI; the existing image will be validated against the selected target before this repair is reported successful.</source>
        <translation>VAROITUS: Myyjä UKI-komento valmistunut ilman muutoksia TUX.EFI; nykyinen kuva validoidaan valitun kohteen mukaan ennen kuin tämä korjaus ilmoitetaan onnistuneeksi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1731"/>
        <source>/boot: %1</source>
        <translation>/boot: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1732"/>
        <source>/boot/efi: %1</source>
        <translation>/boot/efi: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1733"/>
        <source>Validation complete; no target files were changed.</source>
        <translation>Validointi valmis; kohdetiedostoja ei muutettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1734"/>
        <source>/etc/crypttab: %1</source>
        <translation>/etc/cryptab: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1735"/>
        <source>ESP mount candidate: %1</source>
        <translation>ESP-asennuskandidaatti: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1736"/>
        <source>/etc/fstab: %1</source>
        <translation>/etc/fstab: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1737"/>
        <source>GRUB config: %1</source>
        <translation>GRUB config: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1738"/>
        <source>Supported modifying backend: %1</source>
        <translation>Tuettu muokkaustaustaosa: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1739"/>
        <source>OS: %1</source>
        <translation>OS: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1740"/>
        <source>Root: %1</source>
        <translation>Juuri: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1741"/>
        <source>Root filesystem: %1</source>
        <translation>Juuritiedostojärjestelmä: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1742"/>
        <source>Root fs: %1</source>
        <translation>Juurifs: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1743"/>
        <source>Repair root mount source: %1</source>
        <translation>Korjaa juuriasennuslähde: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1744"/>
        <source>Root subvolume: %1</source>
        <translation>Juuriosamäärä: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1745"/>
        <source>Validation summary</source>
        <translation>Validoinnin yhteenveto</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1746"/>
        <source>Vendor command completed successfully: %1</source>
        <translation>Valmistajakomento suoritettu onnistuneesti: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1747"/>
        <source>Repair-to-host capability scan: xattr inspection tooling is unavailable; the rsync security.capability filter was applied and trusted.</source>
        <translation>Korjaus-to-host kyky skannaus: xattr tarkastus työkalut ei ole saatavilla; rsync turvallisuus. käyttökykysuodatinta käytettiin ja luotettiin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1748"/>
        <source>PASS: zpool scrub clean; clearing pool error counters with zpool clear.</source>
        <translation>PASS: zpool pesu puhdas; clearing allas virhelaskurit zpool selkeä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1921"/>
        <location filename="../src/MainWindow.cpp" line="12565"/>
        <source>Repair tool %1 is unavailable.</source>
        <translation>Korjaustyökalu %1 ei ole käytettävissä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1927"/>
        <location filename="../src/MainWindow.cpp" line="12570"/>
        <source>Repair tool %1 has unknown or unavailable diagnostic evidence.</source>
        <translation>Korjaustyökalu %1 on tuntematon tai ei saatavilla diagnostisia todisteita.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1934"/>
        <location filename="../src/MainWindow.cpp" line="12573"/>
        <source>No capability evidence for repair tool %1 in the selected scope. Run diagnostics first.</source>
        <translation>Valitussa laajuudessa ei ole näyttöä korjaustyökalusta %1. Tee ensin diagnostiikka.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2064"/>
        <location filename="../src/MainWindow.cpp" line="2069"/>
        <location filename="../src/MainWindow.cpp" line="2140"/>
        <source>Running-host default boot entry selection is unavailable.</source>
        <translation>Running-host oletus käynnistyksen valinta ei ole käytettävissä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2079"/>
        <location filename="../src/MainWindow.cpp" line="2098"/>
        <source>The helper did not identify a bootable running-host default entry.</source>
        <translation>Auttaja ei tunnistanut käynnistyvää juoksu-host-oletusmerkintää.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="3335"/>
        <source>About %1</source>
        <translation>Tietoja %1:stä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="3885"/>
        <source>Linux detected — %1</source>
        <translation>Linux havaittu %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="3894"/>
        <source>Unlocked Linux filesystem — inspect to confirm</source>
        <translation>Avaa Linux- tiedostojärjestelmä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="3897"/>
        <source>Likely Linux — inspect to confirm</source>
        <translation>Todennäköisesti Linux</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="3900"/>
        <source>Likely Linux — encrypted</source>
        <translation>Todennäköisesti Linux salattu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="3903"/>
        <source>Encrypted — unlock to inspect</source>
        <translation>Salattu Avaa tarkastaa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="3906"/>
        <source>Linux-capable — inspect to confirm</source>
        <translation>Linux-capable</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="3908"/>
        <source>Available for inspection</source>
        <translation>Käytettävissä tarkastusta varten</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="3928"/>
        <source>Encrypted volume</source>
        <translation>Salattu määrä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="3930"/>
        <source>%1 volume</source>
        <translation>%1-tilavuus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="3932"/>
        <source>Partition</source>
        <translation>Jako</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="3937"/>
        <source>Unnamed device</source>
        <translation>Nimeämätön laite</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4385"/>
        <source>Preparing the Boot Bitch administrator session; authorization may be requested before the action starts.</source>
        <translation>Boot Bitch-valvojaistunnon valmistelu; valtuutusta voidaan pyytää ennen toimen aloittamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4627"/>
        <location filename="../src/MainWindow.cpp" line="17061"/>
        <location filename="../src/MainWindow.cpp" line="19369"/>
        <source>Repair file system errors</source>
        <translation>Korjaa tiedostojärjestelmän virheet</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4635"/>
        <source>The read-only check found file system errors in the %1. Select the devices to repair and the mode to use. Each repair runs separately after the helper repeats its scope, mount-state and tool preflights.</source>
        <translation>%1: stä löytyi tiedostojärjestelmän virheitä vain lukemalla. Valitse korjattavat laitteet ja käytettävä tila. Jokainen korjaus toimii erikseen sen jälkeen, kun apuri toistaa sen laajuus, asennustila ja työkalu ennen lentoa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4647"/>
        <location filename="../src/MainWindow.cpp" line="5773"/>
        <location filename="../src/MainWindow.cpp" line="5774"/>
        <source>Repair</source>
        <translation>Korjaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4647"/>
        <location filename="../src/MainWindow.cpp" line="5407"/>
        <source>Device</source>
        <translation>Laite</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4648"/>
        <source>File system</source>
        <translation>Tiedostojärjestelmä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4648"/>
        <source>Mode</source>
        <translation>Tila</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4649"/>
        <source>Issue summary</source>
        <translation>Tiivistelmä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4682"/>
        <source>Device: %1
File system: %2
Mount: %3
Check tool: %4</source>
        <translation>Laite: %1
Tiedostojärjestelmä: %2
Mount: %3
Tarkista työkalu: %4</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4702"/>
        <source>The read-only check reported issues.</source>
        <translation>Vain lukutarkastus raportoi ongelmia.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4717"/>
        <source>Offline repair modes refuse mounted filesystems; btrfs scrub and zpool scrub are online modes and require a mounted filesystem. btrfs check --repair asks for an extra backup warning before it runs.</source>
        <translation>Offline korjaustilat kieltäytyä asennettu tiedostojärjestelmät; btrfs pesu ja zpool pesu ovat online-tiloja ja vaativat asennettu tiedostojärjestelmä. btrfs tarkistaa -- korjaus pyytää lisävaroitus ennen kuin se toimii.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4725"/>
        <source>Run Selected Repairs</source>
        <translation>Suorita valitut korjaukset</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4919"/>
        <source>Linux recovery and boot-repair utility</source>
        <translation>Linuxin palautus- ja käynnistyskorjausapuohjelma</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4921"/>
        <source>GUARDED REPAIR  •  %1</source>
        <translation>GUARDED KORJAUS • %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4928"/>
        <source>Ordinary repairs require an explicitly selected non-host target. The protected running host has a separate deliberate maintenance mode with the same guarded repair stages and requires privilege authorization.</source>
        <translation>Tavalliset korjaukset edellyttävät nimenomaisesti valittua muuta kuin isäntäkohdetta. Suojatulla käyttöisännällä on erillinen tahallinen huoltotila, jossa on samat vartioidut korjausvaiheet, ja se vaatii etuoikeuden.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4953"/>
        <location filename="../src/MainWindow.cpp" line="4957"/>
        <source>A Boot Bitch operation is running in the background.</source>
        <translation>Taustalla on Boot Bitch-operaatio.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4994"/>
        <source>Show previous tab</source>
        <translation>Näytä edellinen välilehti</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4999"/>
        <source>Show next tab</source>
        <translation>Näytä seuraava välilehti</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5018"/>
        <location filename="../src/MainWindow.cpp" line="5027"/>
        <location filename="../src/MainWindow.cpp" line="5222"/>
        <location filename="../src/MainWindow.cpp" line="5272"/>
        <location filename="../src/MainWindow.cpp" line="5273"/>
        <source>Systems</source>
        <translation>Järjestelmät</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5019"/>
        <location filename="../src/MainWindow.cpp" line="5027"/>
        <location filename="../src/MainWindow.cpp" line="5224"/>
        <location filename="../src/MainWindow.cpp" line="5594"/>
        <location filename="../src/MainWindow.cpp" line="5596"/>
        <location filename="../src/MainWindow.cpp" line="6628"/>
        <location filename="../src/MainWindow.cpp" line="6757"/>
        <source>Diagnostics</source>
        <translation>Diagnostiikka</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5020"/>
        <location filename="../src/MainWindow.cpp" line="5027"/>
        <source>Repair</source>
        <comment>tab noun</comment>
        <translation>Korjaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5021"/>
        <location filename="../src/MainWindow.cpp" line="5028"/>
        <location filename="../src/MainWindow.cpp" line="6053"/>
        <source>Snapshots</source>
        <translation>Kuvat</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5022"/>
        <location filename="../src/MainWindow.cpp" line="5028"/>
        <location filename="../src/MainWindow.cpp" line="6223"/>
        <location filename="../src/MainWindow.cpp" line="11121"/>
        <location filename="../src/MainWindow.cpp" line="11151"/>
        <source>Chroot Shell</source>
        <translation>Chroot Shell</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5023"/>
        <location filename="../src/MainWindow.cpp" line="5028"/>
        <location filename="../src/MainWindow.cpp" line="6300"/>
        <source>File Copy</source>
        <translation>Tiedostokopio</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5024"/>
        <location filename="../src/MainWindow.cpp" line="5028"/>
        <location filename="../src/MainWindow.cpp" line="5226"/>
        <location filename="../src/MainWindow.cpp" line="6538"/>
        <source>Logs</source>
        <translation>Lokit</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5025"/>
        <location filename="../src/MainWindow.cpp" line="5028"/>
        <location filename="../src/MainWindow.cpp" line="5228"/>
        <source>Settings</source>
        <translation>Asetukset</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5074"/>
        <source>Ready — guarded repair mode</source>
        <translation>Valmiina</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5198"/>
        <source>&amp;File</source>
        <translation>-&amp;tiedosto</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5199"/>
        <location filename="../src/MainWindow.cpp" line="5276"/>
        <source>Refresh Devices</source>
        <translation>Päivitä laitteet</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5205"/>
        <source>Lock Administrator Session</source>
        <translation>Lukituksen ylläpitäjän istunto</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5207"/>
        <source>Ends Boot Bitch&apos;s current privileged helper session, closes any LUKS mappings opened by Boot Bitch, and requires authorization again for the next root action. Pre-existing external mappings are left alone.</source>
        <translation>Päättää Boot Bitch:n nykyisen etuoikeutetun auttajan istunnon, sulkee kaikki LUKS:n avaamat kuvat ja vaatii luvan seuraavaan juuritoimintoon. Olemassa olevat ulkoiset kartat jätetään rauhaan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5213"/>
        <source>Administrator session locked — owned target resources released; the next privileged action will request authorization.</source>
        <translation>Administrator istunto lukittu Omistettu kohderesurssit vapautetaan; seuraava etuoikeutettu toiminta pyytää luvan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5217"/>
        <source>Quit</source>
        <translation>Lopeta</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5221"/>
        <source>&amp;View</source>
        <translation>-&amp;näkymä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5232"/>
        <source>Auto-size Device Columns</source>
        <translation>Automaattisesti kootut laitteen sarakkeet</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5235"/>
        <source>Wrap Log Lines</source>
        <translation>Kääri lokirivit</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5240"/>
        <source>&amp;Help</source>
        <translation>- &amp;ohje</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5241"/>
        <location filename="../src/MainWindow.cpp" line="16212"/>
        <source>Using Boot Bitch</source>
        <translation>Boot Bitch:n käyttö</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5244"/>
        <location filename="../src/MainWindow.cpp" line="16235"/>
        <source>About Boot Bitch</source>
        <translation>Tietoja Boot Bitch:stä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5274"/>
        <source>Select a physical drive; Boot Bitch resolves the most likely Linux system volume automatically. The running host stays protected from ordinary target repairs, with a separate explicit host-maintenance path for its own system.</source>
        <translation>Valitse fyysinen asema; Boot Bitch ratkaisee todennäköisimmän Linux-järjestelmän äänenvoimakkuuden automaattisesti. Juokseva isäntä pysyy suojattuna tavallisilta kohteiden korjauksilta, ja sillä on oma erillinen huoltopolku omaa järjestelmää varten.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5313"/>
        <source>Detecting running system…</source>
        <translation>Havaitaan ajojärjestelmä…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5324"/>
        <source>Detecting protected storage…</source>
        <translation>Suojatun säilytyksen havaitseminen…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5344"/>
        <source>PROTECTED</source>
        <translation>SUOJATTU</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5350"/>
        <source>The running host remains protected from ordinary repair-target operations.</source>
        <translation>Juokseva isäntä on edelleen suojattu tavallisilta korjauskohteilta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5353"/>
        <source>Details</source>
        <translation>Yksityiskohdat</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5355"/>
        <source>Show read-only details for the protected running host.</source>
        <translation>Näytä vain luettavia tietoja suojatulle ajoisännälle.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5359"/>
        <location filename="../src/MainWindow.cpp" line="7441"/>
        <source>Host Maintenance</source>
        <translation>Huolto</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5361"/>
        <location filename="../src/MainWindow.cpp" line="7442"/>
        <source>Host Maintenance — select the running host for deliberate guarded maintenance. All supported repair stages run against the active system; running-host Snapper @ snapshots are available in the Snapshots tab, while the chroot shell and file-copy workflows remain separate target tools.</source>
        <translation>Isäntä Huolto Valitse käynnissä isäntä tarkoituksellisesti vartioitu huolto. Kaikki tuetut korjausvaiheet kulkevat aktiivista järjestelmää vastaan; juoksu-host Snapper @ kuvakaappaukset ovat saatavilla Snapshots-välilehdessä, kun taas Chroot kuori ja tiedosto-kopio työnkulun pysyvät erillisiä kohdetyökaluja.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5366"/>
        <location filename="../src/MainWindow.cpp" line="7565"/>
        <source>Make Default</source>
        <translation>Tee oletus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5368"/>
        <source>Restore/ensure the running host&apos;s verified default boot entry and select it as the default while preserving every other boot entry.</source>
        <translation>Palauta/varmista juoksuisännän todennettu oletuskäynnistysmerkintä ja valitse se oletuksena säilyttäen samalla kaikki muut käynnistyssyötteet.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5383"/>
        <source>Available repair targets</source>
        <translation>Käytettävissä olevat korjaustavoitteet</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5384"/>
        <source>Repair targets</source>
        <translation>Korjaustavoitteet</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5385"/>
        <source>Drives are ranked by visible Linux, EFI, filesystem and encryption evidence. Select the top-level drive; partitions and mapped volumes are informational.</source>
        <translation>Asemat ovat näkyvien Linux-, EFI-, tiedostojärjestelmä- ja salausnäyttöjen rankaisemia. Valitse ylätason asema; osiot ja kartoitetut volyymit ovat informatiivisia.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5387"/>
        <source>Most likely first</source>
        <translation>Todennäköisesti ensin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5388"/>
        <source>Candidates are ranked by visible Linux, EFI, filesystem and encryption evidence. Click a column header to sort by that column; click it again to reverse the order.</source>
        <translation>Ehdokkaat on listattu näkyvillä Linux, EFI, tiedostojärjestelmä ja salaus näyttö. Napsauta sarakkeen otsikkoa lajitellaksesi tämän sarakkeen; klikkaa sitä uudelleen peruuttaaksesi tilauksen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5406"/>
        <source>Model / Label</source>
        <translation>Malli / merkki</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5406"/>
        <location filename="../src/MainWindow.cpp" line="6092"/>
        <location filename="../src/MainWindow.cpp" line="6806"/>
        <source>Status</source>
        <translation>Tila</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5406"/>
        <source>Connection</source>
        <translation>Yhteys</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5407"/>
        <source>Size</source>
        <translation>Koko</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5407"/>
        <source>Filesystem</source>
        <translation>Tiedostojärjestelmä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5451"/>
        <source>Select Target</source>
        <translation>Valitse kohde</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5455"/>
        <location filename="../src/MainWindow.cpp" line="7383"/>
        <location filename="../src/MainWindow.cpp" line="7729"/>
        <location filename="../src/MainWindow.cpp" line="9261"/>
        <location filename="../src/MainWindow.cpp" line="9340"/>
        <source>Unlock</source>
        <translation>Avaa lukitus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5457"/>
        <location filename="../src/MainWindow.cpp" line="7385"/>
        <source>Select a drive whose detected target is a locked LUKS volume.</source>
        <translation>Valitse asema, jonka havaittu kohde on lukittu LUKS-tilavuus.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5469"/>
        <location filename="../src/MainWindow.cpp" line="8374"/>
        <source>Administrator authorization was deferred for the current scope. Diagnostics and repairs stay available; the next privileged action will request authorization again, or press Authorize to establish the session now.</source>
        <translation>Hallintoviranomaisen valtuutusta lykättiin nykyisen soveltamisalan osalta. Diagnostiikka ja korjaukset pysyvät saatavilla; seuraava etuoikeutettu toiminta pyytää valtuutusta uudelleen, tai paina Valtuuta perustaa istunto nyt.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5473"/>
        <source>Authorize</source>
        <translation>Hyväksy</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5475"/>
        <source>Establish the privileged Boot Bitch helper session for the current scope now instead of waiting for the next privileged action.</source>
        <translation>Perusta etuoikeutettu Boot Bitch auttaja istunto nykyisen soveltamisalan nyt sijaan odottaa seuraavaa etuoikeutettua toimintaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5481"/>
        <location filename="../src/MainWindow.cpp" line="9501"/>
        <source>Committed target: none</source>
        <translation>Sitoutunut tavoite: ei ole</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5483"/>
        <source>No repair target has been committed yet. Row selection is inspection only until Select Target is pressed.</source>
        <translation>Korjaavaa kohdetta ei ole vielä tehty. Rivivalinta on tarkastus vain, kunnes Valitse kohde painetaan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5487"/>
        <source>Unlock status</source>
        <translation>Avaa lukitustila</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5501"/>
        <location filename="../src/MainWindow.cpp" line="7388"/>
        <source>Select a drive to see unlock status.</source>
        <translation>Valitse asema nähdäksesi avaimen tilan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5502"/>
        <location filename="../src/MainWindow.cpp" line="7724"/>
        <location filename="../src/MainWindow.cpp" line="8408"/>
        <source>No unlock operation recorded for this drive in the current session.</source>
        <translation>Asemalle ei tallenneta avaustoimintoa tässä istunnossa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5506"/>
        <source>Selected drive details</source>
        <translation>Valitut aseman tiedot</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5546"/>
        <source>Drive:</source>
        <translation>Aja:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5547"/>
        <source>Detected target:</source>
        <translation>Havaittu kohde:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5548"/>
        <source>Model / label:</source>
        <translation>Malli / etiketti:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5549"/>
        <source>Status:</source>
        <translation>Tila:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5550"/>
        <source>Size:</source>
        <translation>Koko:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5551"/>
        <source>Connection:</source>
        <translation>Yhteys:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5552"/>
        <source>Filesystem:</source>
        <translation>Tiedostojärjestelmä:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5553"/>
        <source>UUID:</source>
        <translation>Hei.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5554"/>
        <source>Mounts:</source>
        <translation>Liitännät:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5555"/>
        <source>Protection:</source>
        <translation>Suojaus:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5556"/>
        <source>Best system component visible without privileged probing. Read-only inspection will later resolve closed encryption and Btrfs root subvolumes automatically.</source>
        <translation>Paras järjestelmäkomponentti näkyvissä ilman etuoikeutettua tutkailua. Lue vain tarkastus myöhemmin ratkaista suljettu salaus ja Btrfs juuri alivolyymit automaattisesti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5597"/>
        <source>Diagnostics follow the Systems page: the selected repair drive while Host Maintenance is off, or the protected running host while Host Maintenance is active. Running Host diagnostics are available only inside the explicit Host Maintenance scope: enter Host Maintenance on the protected running-host card in Systems first. Both scopes provide read-only diagnostics; host EFI/UKI checks inspect the active ESP and firmware entries, while repair-drive checks mount only that system read-only.</source>
        <translation>Diagnostiikka seuraa Systems-sivua: valittu korjausasema, kun isäntähuolto on pois päältä, tai suojattu juoksun isäntä, kun isäntähuolto on aktiivinen. Running Host -diagnostiikka on saatavilla vain nimenomaisen isäntähuollon soveltamisalan sisällä: kirjoita isännän huolto suojattuun juoksu-host -korttiin ensin. Molemmat soveltamisalat tarjoavat vain lukudiagnostiikkaa; isäntä EFI/UKI-tarkastukset tarkastavat aktiiviset ESP- ja firmware-syötteet, kun taas korjaus-asematarkistukset asentavat vain kyseisen järjestelmän.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5606"/>
        <location filename="../src/MainWindow.cpp" line="5777"/>
        <location filename="../src/MainWindow.cpp" line="6056"/>
        <location filename="../src/MainWindow.cpp" line="6227"/>
        <location filename="../src/MainWindow.cpp" line="6304"/>
        <location filename="../src/MainWindow.cpp" line="9447"/>
        <source>Target: none selected</source>
        <translation>Kohde: ei valittu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5611"/>
        <location filename="../src/MainWindow.cpp" line="13419"/>
        <location filename="../src/MainWindow.cpp" line="13935"/>
        <source>Run All</source>
        <translation>Suorita kaikki</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5612"/>
        <source>Run All — run every available read-only diagnostic for the current scope.</source>
        <translation>Suorita Kaikki ... Suorita kaikki saatavilla olevat luku-vain vianmääritys nykyisen soveltamisalan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5628"/>
        <source>Select a target configuration file to inspect or edit through the guarded helper.</source>
        <translation>Valitse kohdeasetustiedosto, joka tarkistetaan tai muokataan suojatun apurin kautta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5630"/>
        <source>Edit Target File…</source>
        <translation>Muokkaa kohdetiedostoa…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5632"/>
        <source>Read or edit the selected target configuration file. Changes invalidate cached diagnostics.</source>
        <translation>Lue tai muokkaa valittua kohdeasetustiedostoa. Muutokset mitätöivät välimuistidiagnostiikan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5638"/>
        <source>Target configuration:</source>
        <translation>Kohdeasetukset:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5649"/>
        <source>Diagnostic checks</source>
        <translation>Diagnostiset tarkastukset</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5668"/>
        <source>Selected diagnostic</source>
        <translation>Valittu vianmääritys</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5673"/>
        <location filename="../src/MainWindow.cpp" line="11608"/>
        <source>Select a diagnostic</source>
        <translation>Valitse vianmääritys</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5676"/>
        <location filename="../src/MainWindow.cpp" line="11609"/>
        <source>Choose a diagnostic from the list.</source>
        <translation>Valitse diagnostiikka luettelosta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5680"/>
        <location filename="../src/MainWindow.cpp" line="11025"/>
        <location filename="../src/MainWindow.cpp" line="18302"/>
        <source>Ready</source>
        <translation>Valmis</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5693"/>
        <source>Results</source>
        <translation>Tulokset</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5696"/>
        <location filename="../src/MainWindow.cpp" line="11611"/>
        <location filename="../src/MainWindow.cpp" line="11678"/>
        <location filename="../src/MainWindow.cpp" line="11687"/>
        <location filename="../src/MainWindow.cpp" line="11715"/>
        <source>Run Diagnostic</source>
        <translation>Suorita diagnostiikka</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5697"/>
        <source>Run Diagnostic — run the selected read-only diagnostic for the current scope; cached results offer a re-run.</source>
        <translation>Suorita Diagnostic</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5704"/>
        <source>Diagnostic results appear here.</source>
        <translation>Diagnostiset tulokset näkyvät tässä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5716"/>
        <source>Copy Results</source>
        <translation>Kopioi tulokset</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5717"/>
        <source>Save Results…</source>
        <translation>Tallenna tulokset…</translation>
    </message>
    <message>
        <source>Run one repair tool or use the Full Repair plan against the selected repair drive. Choose Host Maintenance on Systems to run the same supported, guarded stages against the protected Running Host.</source>
        <translation type="vanished">Suorita yksi korjaustyökalu tai käytä Full Repair suunnitelma vastaan valittu korjausasema. Valitse Host Maintenance on Systems ajaa samat tuetut, vartioidut vaiheet suojattua Juokseva isäntä.</translation>
    </message>
    <message>
        <source>Choose Full Repair stages in Settings. Enabled stages run in the order shown. Each configurable stage also appears below as an individual tool; the Full Repair column mirrors its current Settings state. The boot tools (EFI / UKI bootloader, GRUB or extlinux configuration, boot-stack reconciliation and Make Default) are independent: run them in any order, and a later action re-verifies what an earlier one changed and reports its own result. The active scope is shown beside Repair: selected repair drive or Running Host maintenance.</source>
        <translation type="vanished">Valitse täydellinen korjausvaiheet Asetuksista. Käytössä olevat vaiheet suoritetaan järjestyksessä. Jokainen konfiguroitava vaihe näkyy myös alla yksittäisenä työkaluna; Full Repair- sarake peilaa sen nykyistä asetustilaa. Käynnistystyökalut (EFI / UKI bootloader, GRUB tai extlinux konfiguraatio, boot-stack täsmäytys ja Make Oletus) ovat riippumattomia: suorita ne missä tahansa järjestyksessä, ja myöhemmin toiminta tarkistaa, mitä aikaisempi muutti ja raportoi oman tuloksensa. Aktiivinen laajuus näkyy vieressä Korjaus: valittu korjausasema tai Running Host huolto.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5775"/>
        <source>Run one repair tool or use the Full Repair plan against the selected repair drive. Choose Host Maintenance on Systems to run the same supported, guarded stages against the protected Running Host. Choose Full Repair stages in Settings. Enabled stages run in the order shown. Each configurable stage also appears below as an individual tool; the Full Repair column mirrors its current Settings state. The boot tools (EFI / UKI bootloader, GRUB or extlinux configuration, boot-stack reconciliation and Make Default) are independent: run them in any order, and a later action re-verifies what an earlier one changed and reports its own result. The active scope is shown beside Repair: selected repair drive or Running Host maintenance.</source>
        <translation>Suorita yksi korjaustyökalu tai käytä Full Repair suunnitelma vastaan valittu korjausasema. Valitse Host Maintenance on Systems ajaa samat tuetut, vartioidut vaiheet suojattua Juokseva isäntä. Valitse täydellinen korjausvaiheet Asetuksista. Käytössä olevat vaiheet suoritetaan järjestyksessä. Jokainen konfiguroitava vaihe näkyy myös alla yksittäisenä työkaluna; Full Repair- sarake peilaa sen nykyistä asetustilaa. Käynnistystyökalut (EFI / UKI bootloader, GRUB tai extlinux konfiguraatio, boot-stack täsmäytys ja Make Oletus) ovat riippumattomia: suorita ne missä tahansa järjestyksessä, ja myöhemmin toiminta tarkistaa, mitä aikaisempi muutti ja raportoi oman tuloksensa. Aktiivinen laajuus näkyy vieressä Korjaus: valittu korjausasema tai Running Host huolto.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5781"/>
        <location filename="../src/MainWindow.cpp" line="6727"/>
        <source>Full Repair plan</source>
        <translation>Täydellinen korjaussuunnitelma</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5793"/>
        <location filename="../src/MainWindow.cpp" line="17138"/>
        <source>No stages selected</source>
        <translation>Ei valittuja vaiheita</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5799"/>
        <source>Run All diagnostics for the selected target or running host before starting Full Repair. The report is read-only evidence used to choose and confirm repair stages.</source>
        <translation>Suorita kaikki diagnostiikat valitun kohteen tai käynnissä isäntä ennen Full Repair. Raportti on pelkkä luettava todiste korjausvaiheiden valitsemisesta ja vahvistamisesta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5812"/>
        <source>Configure Plan…</source>
        <translation>Määritä Plan…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5813"/>
        <source>Open Settings to choose which Full Repair stages are part of the plan.</source>
        <translation>Avaa asetukset valita, mitkä Full Repair vaiheet ovat osa suunnitelmaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5824"/>
        <location filename="../src/MainWindow.cpp" line="18883"/>
        <source>Run Full Repair</source>
        <translation>Suorita koko korjaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5827"/>
        <location filename="../src/MainWindow.cpp" line="5973"/>
        <source>Select a repair drive, or choose Host Maintenance on the protected running-host card.</source>
        <translation>Valitse korjausasema tai valitse isännän huolto suojatulla juoksu-host-kortilla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5872"/>
        <source>Individual repair tools</source>
        <translation>Yksittäiset korjaustyökalut</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5878"/>
        <source>Tool</source>
        <translation>Työkalu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5878"/>
        <location filename="../src/MainWindow.cpp" line="18845"/>
        <location filename="../src/MainWindow.cpp" line="18930"/>
        <location filename="../src/MainWindow.cpp" line="18944"/>
        <source>Full Repair</source>
        <translation>Täysi korjaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5914"/>
        <location filename="../src/MainWindow.cpp" line="17305"/>
        <source>Validate environment</source>
        <translation>Validointiympäristö</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5915"/>
        <location filename="../src/MainWindow.cpp" line="17438"/>
        <location filename="../src/MainWindow.cpp" line="19310"/>
        <location filename="../src/MainWindow.cpp" line="19341"/>
        <source>File system repair</source>
        <translation>Tiedostojärjestelmän korjaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5916"/>
        <location filename="../src/MainWindow.cpp" line="17313"/>
        <location filename="../src/MainWindow.cpp" line="18743"/>
        <source>Complete package configuration</source>
        <translation>Täydellinen pakettien kokoonpano</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5917"/>
        <location filename="../src/MainWindow.cpp" line="17320"/>
        <location filename="../src/MainWindow.cpp" line="18747"/>
        <source>Repair broken dependencies</source>
        <translation>Korjaa rikkinäiset riippuvuudet</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5918"/>
        <location filename="../src/MainWindow.cpp" line="6736"/>
        <location filename="../src/MainWindow.cpp" line="17064"/>
        <location filename="../src/MainWindow.cpp" line="17331"/>
        <location filename="../src/MainWindow.cpp" line="18150"/>
        <location filename="../src/MainWindow.cpp" line="18751"/>
        <source>Refresh package metadata</source>
        <translation>Päivitä paketin metatiedot</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5919"/>
        <location filename="../src/MainWindow.cpp" line="17065"/>
        <location filename="../src/MainWindow.cpp" line="17342"/>
        <location filename="../src/MainWindow.cpp" line="18755"/>
        <source>Upgrade installed packages</source>
        <translation>Asennettujen pakettien päivittäminen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5920"/>
        <location filename="../src/MainWindow.cpp" line="17353"/>
        <source>DKMS</source>
        <translation>DKMS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5922"/>
        <location filename="../src/MainWindow.cpp" line="17379"/>
        <source>Initramfs</source>
        <translation>Initramfs</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5923"/>
        <location filename="../src/MainWindow.cpp" line="17391"/>
        <source>EFI / UKI bootloader</source>
        <translation>EFI / UKI-käynnistin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5925"/>
        <location filename="../src/MainWindow.cpp" line="17431"/>
        <source>extlinux configuration</source>
        <translation>extlinux- kokoonpano</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5926"/>
        <location filename="../src/MainWindow.cpp" line="17453"/>
        <source>Boot stack reconciliation</source>
        <translation>Boot pino täsmäytys</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5935"/>
        <source>Mirrors the corresponding Settings → Full Repair plan checkbox. Individual tools remain runnable independently.</source>
        <translation>Peilaa vastaavat asetukset → Koko korjaussuunnitelman valintaruutu. Yksittäiset työkalut pysyvät itsenäisesti käytettävissä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5953"/>
        <source>Selected tool</source>
        <translation>Valittu työkalu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5968"/>
        <location filename="../src/MainWindow.cpp" line="17282"/>
        <source>Select a repair tool</source>
        <translation>Valitse korjaustyökalu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5971"/>
        <location filename="../src/MainWindow.cpp" line="17285"/>
        <source>Run Tool</source>
        <translation>Suorita työkalu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5977"/>
        <source>Select a tool to review its repair action.</source>
        <translation>Valitse työkalu tarkistaa sen korjaustoiminta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6052"/>
        <source>Btrfs snapshots</source>
        <translation>Btrfs-kuvat</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6054"/>
        <source>Inspect Btrfs root snapshots and perform a transactional rollback. Rollback keeps the source snapshot unchanged, preserves the current @ root, promotes a writable copy to @, rebuilds the boot stack and automatically restores the old root if post-switch validation fails. In Host Maintenance the same workflow targets the running host through Snapper @ snapshots and Boot Bitch @rollback-before-* undo points; the running host starts the promoted root only after a reboot.</source>
        <translation>Tutki Btrfs:n juurikuvat ja suorita transaktion palautus. Rollback pitää lähdekuvan ennallaan, säilyttää nykyisen @ rootin, edistää kirjoitettavaa kopiota @: lle, rakentaa käynnistyspinon uudelleen ja palauttaa vanhan juuren automaattisesti, jos kytkin epäonnistuu. Host Maintenancessa sama työnkulku kohdistuu käynnissä olevaan isäntään Snapper @-kuvien ja Boot Bitch @rollback-ennen-* perumispisteiden kautta; käynnissä oleva isäntä aloittaa promoted rootin vasta uudelleenkäynnistyksen jälkeen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6075"/>
        <location filename="../src/MainWindow.cpp" line="10361"/>
        <location filename="../src/MainWindow.cpp" line="10510"/>
        <source>Reboot Now</source>
        <translation>Käynnistä nyt uudelleen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6076"/>
        <source>Reboots the running host after a separate confirmation; all users are signed out and unsaved work is lost.</source>
        <translation>Käynnistää juoksuisännän uudelleen erillisen vahvistuksen jälkeen; kaikki käyttäjät ovat kirjautuneet ulos ja tallentamaton työ katoaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6077"/>
        <location filename="../src/MainWindow.cpp" line="10362"/>
        <source>Later</source>
        <translation>Myöhemmin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6078"/>
        <source>Hides the reboot reminder until Host Maintenance is re-entered; the staged rollback stays in effect.</source>
        <translation>Piilotetaan uudelleenkäynnistysmuistutus, kunnes isäntähuolto on palautettu; lavastettu palautus pysyy voimassa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6083"/>
        <source>Reboot reminder hidden; the running-host rollback stays staged until the host is rebooted.</source>
        <translation>Uudelleenkäynnistysmuistutus piilotettu; käynnissä-host takaisin pysyy lavastettu kunnes isäntä on uudelleenkäynnistetty.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6092"/>
        <source>Snapshot</source>
        <translation>Kuva</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6092"/>
        <source>Created</source>
        <translation>Luotu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6092"/>
        <source>Type</source>
        <translation>Tyyppi</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6092"/>
        <source>Description</source>
        <translation>Tavaran kuvaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6139"/>
        <source>Load snapshots to inspect recovery points. Snapshot discovery runs through the guarded read-only helper, so root-owned Snapper metadata does not need to be readable by the desktop user.</source>
        <translation>Lataa valokuvia palautuspisteiden tarkastamiseksi. Snapshot löytö kulkee vartioitu lukuavain apuri, joten root-omistettu Snapper metadata ei tarvitse lukea työpöydän käyttäjä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6151"/>
        <source>Snapshot inventory</source>
        <translation>Kuvaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6163"/>
        <source>Load Snapshots</source>
        <translation>Lataa kuvat</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6164"/>
        <source>Inspect Selected</source>
        <translation>Tutki valittu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6165"/>
        <source>Roll Back to Selected</source>
        <translation>Siirrä takaisin valittuun</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6170"/>
        <source>Load Btrfs root snapshots through the privileged helper using read-only mounts.</source>
        <translation>Lataa Btrfs-root-kuvia etuoikeutetun apurin kautta lukuavainasennusten avulla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6171"/>
        <source>Inspect the selected snapshot read-only, including OS metadata, fstab/crypttab and visible kernel files.</source>
        <translation>Tarkasta vain valittu kuvakuva, mukaan lukien käyttöjärjestelmän metatiedot, fstab/cryptab ja näkyvät ytimen tiedostot.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6172"/>
        <source>Run a read-only rollback preflight, then promote a writable copy of the selected snapshot to @ with automatic root restoration if boot-stack reconciliation fails.</source>
        <translation>Suorita luku-vain rollback ennen lentoa, sitten promote kirjoitettava kopio valitun tilannekuvan @ automaattinen juurien palautus jos boot-stack täsmäytys epäonnistuu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6180"/>
        <source>Select a snapshot to enable the actions above, or double-click a row to inspect it read-only.</source>
        <translation>Valitse kuvake, jonka avulla yllä olevat toiminnot voidaan ottaa käyttöön, tai kaksoisnapsauta riviä sen tarkistamiseksi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6217"/>
        <location filename="../src/MainWindow.cpp" line="11110"/>
        <location filename="../src/MainWindow.cpp" line="11248"/>
        <source>Chroot shell</source>
        <translation>Chroot-kuori</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6224"/>
        <location filename="../src/MainWindow.cpp" line="11119"/>
        <source>Run a command inside the selected repair system as root (sudo is not needed). Commands are executed one at a time in a fresh chroot. When a command asks a question, Boot Bitch shows it in a popup and sends your answer back to the command; cancelling stops the command. Non-interactive flags such as dnf update -y or apt-get -y upgrade remain recommended for unattended runs. Output is kept in this window and in the application log.</source>
        <translation>Suorita komento valitun korjausjärjestelmän sisällä juurina (sudoa ei tarvita). Komennot suoritetaan yksi kerrallaan tuoreella krootilla. Kun komento esittää kysymyksen, Boot Bitch näyttää sen ponnahdusikkunassa ja lähettää vastauksesi takaisin komentoon; komento keskeytetään. Ei-interaktiiviset liput, kuten dnf update -y tai apt-get -y päivitys on edelleen suositeltavaa valvomattomia juoksuja. Tulosta säilytetään tässä ikkunassa ja sovelluslokissa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6233"/>
        <location filename="../src/MainWindow.cpp" line="11126"/>
        <source>Command, for example: dnf update -y or update-grub</source>
        <translation>Komento, esimerkiksi: dnf update -y tai update-grub</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6237"/>
        <location filename="../src/MainWindow.cpp" line="11132"/>
        <source>Run Command</source>
        <translation>Suorita komento</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6239"/>
        <location filename="../src/MainWindow.cpp" line="11099"/>
        <source>Execute the command inside the selected repair system as root.</source>
        <translation>Suorita komento valitun korjausjärjestelmän sisällä juurina.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6255"/>
        <source>Command output will appear here.</source>
        <translation>Komennon tulostus näkyy täällä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6262"/>
        <location filename="../src/MainWindow.cpp" line="11137"/>
        <source>Commands can modify the target system. Review each command before running it.</source>
        <translation>Komento voi muuttaa kohdejärjestelmää. Tarkista jokainen komento ennen sen suorittamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6265"/>
        <source>Clear Output</source>
        <translation>Tyhjennä tuloste</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6298"/>
        <source>File copy</source>
        <translation>Tiedostokopiointi</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6301"/>
        <source>Copy and verify files in either direction. Host to Repair remounts only the selected target filesystem read-write after safety checks; Repair to Host keeps the repair target read-only. Browse Target Folders reads the selected repair tree through temporary read-only mounts, while transfers use rsync without --delete, path containment, ownership validation and post-copy verification.</source>
        <translation>Kopioi ja varmista tiedostot kumpaan suuntaan tahansa. Korjaaja asentaa uudelleen vain valitun kohdetiedostojärjestelmän lukeman turvallisuustarkastusten jälkeen; Korjaa Host pitää korjauskohde vain lukea. Selaa Target-kansiot lukevat valitun korjauspuun tilapäisten lukutaukojen kautta, kun taas siirrot käyttävät rsync ilman --delete, polun hallinta, omistuksen validointi ja kopion jälkeinen todentaminen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6313"/>
        <source>Preview Changes</source>
        <translation>Esikatselumuutokset</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6314"/>
        <location filename="../src/MainWindow.cpp" line="11447"/>
        <source>Copy and Verify</source>
        <translation>Kopioi ja varmista</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6317"/>
        <source>Run an rsync dry-run through the guarded helper. No files are changed.</source>
        <translation>Aja Rsync kuivakäynnillä vartijan läpi. Tiedostoja ei ole muutettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6318"/>
        <source>Copy staged items and verify the result. Existing destination names are overwritten when source content differs; unrelated destination files are never deleted.</source>
        <translation>Kopioi lavastetut kohdat ja varmista tulos. Olemassa olevat kohdenimet korvataan, kun lähdesisältö poikkeaa; etuyhteydettömiä kohdetiedostoja ei koskaan poisteta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6331"/>
        <source>Direction:</source>
        <translation>Suunta:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6333"/>
        <location filename="../src/MainWindow.cpp" line="11401"/>
        <source>Host → Repair</source>
        <translation>Host → korjaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6334"/>
        <location filename="../src/MainWindow.cpp" line="11401"/>
        <source>Repair → Host</source>
        <translation>Korjaa →-ohjain</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6335"/>
        <source>Choose which system supplies the source files and which system receives them.</source>
        <translation>Valitse mikä järjestelmä toimittaa lähdetiedostot ja mikä järjestelmä vastaanottaa ne.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6360"/>
        <location filename="../src/MainWindow.cpp" line="10574"/>
        <source>Add Files…</source>
        <translation>Lisää tiedostoja…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6361"/>
        <location filename="../src/MainWindow.cpp" line="10575"/>
        <source>Add Folder…</source>
        <translation>Lisää kansio…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6362"/>
        <source>Remove</source>
        <translation>Poista</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6363"/>
        <source>Remove the selected staged source entries from this list.</source>
        <translation>Poista valitut vaiheistetut lähdetiedot tästä luettelosta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6364"/>
        <location filename="../src/MainWindow.cpp" line="10672"/>
        <source>Clear</source>
        <translation>Tyhjennä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6365"/>
        <source>Clear every staged source entry from this list.</source>
        <translation>Tyhjennä jokainen vaiheistettu lähdemerkintä tästä luettelosta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6388"/>
        <source>Choose Path…</source>
        <translation>Valitse polku…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6394"/>
        <source>3. Ownership and copy policy</source>
        <translation>3. Omistus- ja kopiointipolitiikka</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6397"/>
        <source>Ownership:</source>
        <translation>Omistusoikeus:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6399"/>
        <source>Smart destination ownership (recommended)</source>
        <translation>Älykäs määränpään omistus (suositeltu)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6400"/>
        <source>Preserve source numeric UID/GID</source>
        <translation>Säilytä lähdenumero UID/GID</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6402"/>
        <source>Smart mode validates UID/GID identity mapping across the two systems and falls back to the destination-directory owner when the same numeric ID means a different account.</source>
        <translation>Smart-tila validoi UID/GID-tunnistuskartoituksen näiden kahden järjestelmän välillä ja palaa kohde-hakemiston omistajalle, kun sama numerotunnus tarkoittaa eri tiliä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6537"/>
        <source>Application log</source>
        <translation>Sovellusloki</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6539"/>
        <source>Shows this application&apos;s session activity. Session files are listed on the left; Save a copy when you need to share troubleshooting details.</source>
        <translation>Näyttää sovelluksen istuntotoiminnan. Istuntotiedostot ovat vasemmalla; Tallenna kopio, kun haluat jakaa vianmääritystietoja.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6541"/>
        <source>Save As…</source>
        <translation>Tallenna As…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6542"/>
        <source>Clear Register</source>
        <translation>Tyhjennä rekisteri</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6576"/>
        <source>Session logs</source>
        <translation>Istunnon lokit</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6590"/>
        <source>New Session Log</source>
        <translation>Uuden istunnon loki</translation>
    </message>
    <message>
        <source>Truncate the current session log and start it over with a cleared-by-user entry. Prior session files are never modified.</source>
        <translation type="vanished">Suorita nykyinen istuntoloki ja käynnistä se uudelleen käyttäjäkohtaisella kirjauksella. Aiempia istuntotiedostoja ei koskaan muuteta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6591"/>
        <source>Delete</source>
        <translation>Poista</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6593"/>
        <location filename="../src/MainWindow.cpp" line="10737"/>
        <source>Refresh</source>
        <translation>Päivitä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6594"/>
        <location filename="../src/MainWindow.cpp" line="15033"/>
        <source>Add Note</source>
        <translation>Lisää huomautus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6595"/>
        <source>Append a timestamped NOTE entry to the current session log.</source>
        <translation>Lisää aikaleimattu HUOMAUTUSmerkintä istuntolokiin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6610"/>
        <source>Search log:</source>
        <translation>Etsi loki:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6616"/>
        <source>Filter application log (fuzzy match; use @section, e.g. @errors, for whole diagnostic sections)…</source>
        <translation>Suodatinsovellusloki (pörröinen ottelu; käytä @sektiota, esim. @errorit, kokonaisissa diagnostisissa osissa)…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6618"/>
        <source>Type any characters to show matching application-log entries. Matching is case-insensitive and fuzzy. A term beginning with @ selects a complete diagnostic section by key or title (for example @errors, @boot or @all) together with the repair entries related to that section; other terms keep the fuzzy line match and combine as AND.</source>
        <translation>Kirjoita kaikki merkit, joilla näytetään vastaavat sovelluslokimerkinnät. Samankaltaisuus on asiatonta ja pörröistä. Termi, joka alkaa @: lla, valitsee täydellisen diagnostisen osion avaimen tai otsikon mukaan (esim. @errorit, @boot tai @all) sekä kyseiseen osaan liittyvät korjausmerkinnät; muut termit pitävät sumean viivan ottelun ja yhdistävät sen AND: nä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6627"/>
        <source>All entries</source>
        <translation>Kaikki merkinnät</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6629"/>
        <source>Repairs</source>
        <translation>Korjaukset</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6638"/>
        <source>Filter the visible log by entry kind. Selecting a diagnostic section shows that complete section plus the repair entries recorded for its related repair tools; a workflow filter such as File system repair additionally shows the whole storage evidence sections it works on.</source>
        <translation>Suodata näkyvissä oleva loki kirjoitustavan mukaan. Diagnostisen osion valinta osoittaa, että täydellinen osa ja korjausmerkinnät kirjataan sen liittyviä korjaustyökaluja; työnkulun suodatin kuten Tiedoston järjestelmän korjaus näyttää lisäksi koko tallennusnäyttö osiot se toimii.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6717"/>
        <source>Device discovery</source>
        <translation>Laitelöytö</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6719"/>
        <source>Show devices without an identified Linux installation</source>
        <translation>Näytä laitteet ilman tunnistettua Linux-asennusta</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6720"/>
        <source>Show removable and USB storage</source>
        <translation>Näytä irrotettava ja USB- tallennus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6721"/>
        <source>Show encrypted devices before unlocking</source>
        <translation>Näytä salatut laitteet ennen avaamista</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6729"/>
        <source>Repair file system errors (read-only check first)</source>
        <translation>Korjaa tiedostojärjestelmän virheet (lukea vain tarkistaa ensin)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6730"/>
        <source>Runs a read-only file system check for the root, /boot, ESP and /home filesystems, then offers an explicit per-device repair for each filesystem that reports errors. Offline repair tools refuse mounted filesystems; btrfs scrub and zpool scrub are online modes.</source>
        <translation>Suorittaa luku-vain tiedostojärjestelmän tarkistaa juuri, /boot, ESP ja /home tiedostojärjestelmät, sitten tarjoaa nimenomaisen per laite korjaus jokaiselle tiedostojärjestelmä, joka raportoi virheitä. Offline korjaustyökalut ei asennettu tiedostojärjestelmät; btrfs pesu ja zpool pesu ovat online-tilat.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6734"/>
        <location filename="../src/MainWindow.cpp" line="17062"/>
        <location filename="../src/MainWindow.cpp" line="18863"/>
        <source>Complete interrupted package configuration</source>
        <translation>Keskeytä pakettien asetukset</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6735"/>
        <location filename="../src/MainWindow.cpp" line="17063"/>
        <location filename="../src/MainWindow.cpp" line="18147"/>
        <source>Repair broken package dependencies</source>
        <translation>Korjaa rikkinäiset pakettiriippuvuudet</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6737"/>
        <location filename="../src/MainWindow.cpp" line="18153"/>
        <source>Upgrade installed packages (adaptive APT simulation)</source>
        <translation>Asennettujen pakettien päivittäminen (mukautuva APT-simulaatio)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6738"/>
        <location filename="../src/MainWindow.cpp" line="17066"/>
        <location filename="../src/MainWindow.cpp" line="18867"/>
        <source>Rebuild DKMS modules</source>
        <translation>Rakenna DKMS-moduulit uudelleen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6739"/>
        <location filename="../src/MainWindow.cpp" line="18156"/>
        <source>Restore detected graphical login manager and graphical.target</source>
        <translation>Palauta havaittu graafinen kirjautumisen hallinta ja graafinen. kohde</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6740"/>
        <location filename="../src/MainWindow.cpp" line="17068"/>
        <location filename="../src/MainWindow.cpp" line="18159"/>
        <source>Rebuild initramfs after mapper/crypttab validation</source>
        <translation>Rakenna initramfs uudelleen mapper/cryptab-validoinnin jälkeen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6741"/>
        <location filename="../src/MainWindow.cpp" line="18141"/>
        <source>Repair EFI / UKI boot path (explicit target ESP repair)</source>
        <translation>Korjaa EFI / UKI boot polku (selitä kohde ESP korjaus)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6742"/>
        <location filename="../src/MainWindow.cpp" line="17070"/>
        <location filename="../src/MainWindow.cpp" line="18144"/>
        <source>Update GRUB configuration</source>
        <translation>Päivitä GRUB- asetus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6743"/>
        <location filename="../src/MainWindow.cpp" line="17071"/>
        <location filename="../src/MainWindow.cpp" line="18162"/>
        <source>Update extlinux configuration</source>
        <translation>Päivitä extlinux- asetus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6759"/>
        <source>Automatically regenerate read-only diagnostics after repairs or target changes</source>
        <translation>Uudista automaattisesti vain lukudiagnostiikka korjausten tai tavoitemuutosten jälkeen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6760"/>
        <source>Regenerates the cached read-only diagnostics for the current Diagnostics scope after a repair, target change, or other evidence invalidation. The refresh runs only inside an already authorized administrator session; it never triggers a new Polkit prompt.</source>
        <translation>Regeneroi välimuistin luku-vain diagnostiikan nykyisen Diagnostics soveltamisalan jälkeen korjaus, kohteen muutos, tai muut todisteet mitätöidään. Virkistäminen toimii vain jo hyväksytyn ylläpitäjän istunnon sisällä; se ei koskaan laukaise uutta Polkit-kehotusta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6766"/>
        <source>Mandatory safety controls</source>
        <translation>Pakollinen turvavalvonta</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6769"/>
        <source>Protect every physical device backing /, /boot and /boot/efi</source>
        <translation>Suojaa jokaisen fyysisen laitteen tausta /, /boot ja /boot/efi</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6770"/>
        <source>Require explicit confirmation before package installation or repair actions</source>
        <translation>Vaadi nimenomainen vahvistus ennen pakettien asennus- tai korjaustoimia</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6771"/>
        <source>Require mapper/crypttab consistency before initramfs rebuild</source>
        <translation>Vaadi mapper/cryptab johdonmukaisuutta ennen initramfs uudelleen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6772"/>
        <source>Preserve pre-rollback Btrfs root and auto-restore it on validation failure</source>
        <translation>Säilytä pre-rollback Btrfs root ja automaattinen palauttaa sen validointivirhe</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6773"/>
        <source>Never log LUKS passphrases or authentication secrets</source>
        <translation>Älä koskaan kirjaudu LUKS:n salalauseisiin tai todennussalaisuuksiin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6783"/>
        <source>Host capabilities and dependencies</source>
        <translation>Isäntävalmiudet ja riippuvuussuhteet</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6798"/>
        <source>Distribution:</source>
        <translation>Jakautuminen:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6799"/>
        <source>Package manager family:</source>
        <translation>Pakettipäällikön perhe:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6800"/>
        <source>KAuth build support:</source>
        <translation>KAuthin rakennetuki:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6805"/>
        <source>Feature</source>
        <translation>Ominaisuus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6805"/>
        <source>Command</source>
        <translation>Komento</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6805"/>
        <source>Scope</source>
        <translation>Soveltamisala</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6806"/>
        <source>Suggested package</source>
        <translation>Ehdotettu paketti</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6806"/>
        <source>Notes</source>
        <translation>Huomautuksia</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6855"/>
        <source>Refresh Capabilities</source>
        <translation>Päivitä valmiudet</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6856"/>
        <source>Install Missing Support…</source>
        <translation>Asenna puuttuva tuki…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6858"/>
        <source>Automatic installation will require explicit package mapping and privilege authorization.</source>
        <translation>Automaattinen asennus edellyttää nimenomaista paketti kartoitus ja etuoikeus valtuutus.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7082"/>
        <source>Scanning block devices…</source>
        <translation>Skannataan lohkolaitteet…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7093"/>
        <location filename="../src/MainWindow.cpp" line="7094"/>
        <source>Device scan failed</source>
        <translation>Laitteen skannaus epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7126"/>
        <source>Device scan complete — discovery did not modify storage</source>
        <translation>Laitteen skannaus täydellinen ... löytö ei muuttanut tallennusta</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7181"/>
        <source>Showing %1 of %2 repair candidate disk(s); %3 running-system disk(s) protected</source>
        <translation>Näytetään %1 %2 korjaus ehdokas levykkeet; %3 juoksu-järjestelmä levy [s] suojattu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7251"/>
        <source>Running system protection unresolved</source>
        <translation>Juoksevan järjestelmän suojaus selvittämätön</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7252"/>
        <source>No protected physical backing disk was identified</source>
        <translation>Suojattua fyysistä taustalevyä ei tunnistettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7262"/>
        <source>Current running Linux system</source>
        <translation>Nykyinen Linux-järjestelmä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7266"/>
        <source>Protected running-system storage</source>
        <translation>Suojattu ajojärjestelmän varastointi</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7267"/>
        <source>Critical mounts: %1</source>
        <translation>Kriittiset kiinnikkeet: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7347"/>
        <source>Select to inspect this partition/volume. Select Target still chooses the physical drive and its preferred Linux root.
%1</source>
        <translation>Valitse tarkastaaksesi tämän osion/tilavuuden. Valitse Kohde edelleen valitsee fyysisen aseman ja sen suositun Linux rootin.
%1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7352"/>
        <source>Select this physical drive as the repair target. Expand it only to view technical partition/volume details.
%1</source>
        <translation>Valitse tämä fyysinen asema korjauskohteeksi. Laajenna sitä vain nähdäksesi teknisen osion/volyymin yksityiskohdat.
%1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7425"/>
        <source>Showing protected running-host details</source>
        <translation>Näytetään suojatut ajo-isäntätiedot</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7479"/>
        <source>Host maintenance unavailable</source>
        <translation>Huoltoa ei ole saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7493"/>
        <source>Exit Host Maintenance</source>
        <translation>Poistuminen isäntähuolto</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7494"/>
        <source>Exit Host Maintenance — leave running-host maintenance and return to ordinary repair-target mode.</source>
        <translation>Poistu Host Maintenance</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7539"/>
        <source>Host default unavailable</source>
        <translation>Isännän oletus ei saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7545"/>
        <source>Make host the default boot entry</source>
        <translation>Asenna oletuskäynnistys</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7546"/>
        <source>Restore/ensure and select the running host&apos;s default boot entry?</source>
        <translation>Palauttaa / varmistaa ja valitse käynnissä olevan isännän oletuskäynnistys?</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7552"/>
        <location filename="../src/MainWindow.cpp" line="12643"/>
        <source>default boot entry</source>
        <translation>Oletuskäynnistys</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7553"/>
        <location filename="../src/MainWindow.cpp" line="12644"/>
        <source>%1 default boot entry</source>
        <translation>%1:n oletuskäynnistys</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7555"/>
        <source>Host disk: %1
Root: %2

Boot Bitch will identify the host ESP, restore/ensure the verified %3, and place it first in BootOrder. The fallback/recovery route and every other firmware entry stay bootable and are never deleted.</source>
        <translation>Käyttölevy: %1
Juuri: %2

Boot Bitch tunnistaa isäntä ESP, palauttaa / varmistaa todennetun %3, ja asettaa sen ensin BootSorder. Vara-/hyödyntämisreitti ja kaikki muut firmware-ohjelmistot pysyvät käynnistyskelpoisina eikä niitä koskaan poisteta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7559"/>
        <source>Host disk: %1
Root: %2

Boot Bitch will identify the running host&apos;s boot configuration, restore/ensure the verified %3, and select it as the default. Every other boot entry stays bootable and is never deleted.</source>
        <translation>Käyttölevy: %1
Juuri: %2

Boot Bitch tunnistaa juoksuisännän käynnistyskokoonpanon, palauttaa/varmistaa vahvistetun %3:n ja valitsee sen oletusarvoksi. Kaikki muut kengät pysyvät käynnistyskelpoisina, eikä niitä koskaan poisteta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7577"/>
        <source>Make host default boot entry</source>
        <translation>Tee oletuskäynnistys</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7628"/>
        <source>Host default set to %1</source>
        <translation>Aseta oletus %1:ään</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7631"/>
        <source>Host default verified</source>
        <translation>Oletus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7632"/>
        <source>%1

Full helper output is available in Logs.</source>
        <translation>%1

Täysi apuri tuloste on saatavilla lokeissa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7639"/>
        <source>Host default not verified: the helper did not name a verified boot entry.</source>
        <translation>Isännän oletus ei todennettu: auttaja ei nimennyt todennettua käynnistysmerkintää.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7640"/>
        <source>Host default not verified</source>
        <translation>Asennuksen oletusarvoa ei tarkistettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7642"/>
        <source>Host default operation failed: %1</source>
        <translation>Oletustoiminto epäonnistui: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7645"/>
        <source>Host default operation failed</source>
        <translation>Oletustoiminto epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7646"/>
        <source>The host default operation failed: %1

Full helper output is available in Logs.</source>
        <translation>Oletustoiminto epäonnistui: %1

Täysi apuri tuloste on saatavilla lokeissa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7666"/>
        <source>Pending inspection</source>
        <translation>Ennen tarkastusta</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7679"/>
        <source>PROTECTED — running system; read-only details only</source>
        <translation>SUOJATTU ajojärjestelmä; ainoastaan luettavat tiedot</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7681"/>
        <source>Live / installer media — not selectable</source>
        <translation>Live / asentaja mediat Ei valittavissa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7683"/>
        <source>Unlock required before selection</source>
        <translation>Avaa vaaditaan ennen valintaa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7684"/>
        <source>Eligible repair candidate</source>
        <translation>Tukikelpoiset korjausehdokkaat</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7698"/>
        <source>Commit this physical drive as the repair target.</source>
        <translation>Toimita tämä fyysinen asema korjauskohteena.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7701"/>
        <source>Live / installer media is read-only boot media and cannot be selected as a repair target.</source>
        <translation>Live / asentaja media on luku-avain media, eikä sitä voida valita korjauskohteeksi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7703"/>
        <source>Unlock the encrypted volume first; Select Target becomes available after a Linux filesystem is detected.</source>
        <translation>Avaa salattu äänenvoimakkuus ensin; Valitse kohde tulee saataville sen jälkeen, kun Linux-tiedostojärjestelmä on havaittu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7704"/>
        <source>No selectable repair target was detected on this drive.</source>
        <translation>Asemalla ei havaittu valittua korjauskohdetta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7705"/>
        <source>The protected running host cannot be selected as a repair target.</source>
        <translation>Suojattua käynnissä olevaa isäntää ei voida valita korjauskohteeksi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7721"/>
        <source>Already unlocked before this Boot Bitch session. No unlock operation was performed here; the visible mapper will be reused and will not be closed by Boot Bitch.</source>
        <translation>Avattu jo ennen Boot Bitch-istuntoa. Täällä ei suoritettu avaustoimintoa; näkyvää karttaa käytetään uudelleen eikä Boot Bitch sulje sitä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7729"/>
        <source>Already Unlocked</source>
        <translation>Avattu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7733"/>
        <source>Unlock %1 using cryptsetup through the privileged helper. The passphrase is sent on standard input and is never placed in command arguments or logs.</source>
        <translation>Avaa %1 käyttäen cryptsetup kautta etuoikeutettu apuri. Salalause lähetetään vakiosyötteellä eikä sitä koskaan sijoiteta komentoargumentteihin tai lokeihin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7736"/>
        <source>An unlocked Linux filesystem is already visible on this drive. Boot Bitch will reuse the existing mapper and will not close or reopen a mapping created by another recovery tool.</source>
        <translation>Asemalla näkyy jo lukitsematon Linux-tiedostojärjestelmä. Boot Bitch käyttää olemassa olevaa karttaa uudelleen eikä sulje tai avaa uudelleen toisen palautustyökalun luomaa kartoitusta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7739"/>
        <source>No locked LUKS component is currently visible on this selected drive.</source>
        <translation>Tällä hetkellä valitulla asemalla ei ole lukittua LUKS-komponenttia.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7741"/>
        <source>The protected running host cannot be unlocked or modified by Boot Bitch.</source>
        <translation>Boot Bitch ei voi avata tai muokata suojattua juoksu isäntää.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7764"/>
        <location filename="../src/MainWindow.cpp" line="7800"/>
        <source>Target unavailable</source>
        <translation>Kohde ei saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7765"/>
        <source>The selected drive is no longer available. Refresh devices and select it again.</source>
        <translation>Valittua asemaa ei ole enää saatavilla. Päivitä laitteet ja valitse se uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7778"/>
        <source>Protected system</source>
        <translation>Suojattu järjestelmä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7779"/>
        <source>The running system cannot be selected as a repair target.</source>
        <translation>Ajojärjestelmää ei voi valita korjauskohteeksi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7788"/>
        <source>Live / installer media cannot be selected</source>
        <translation>Live-/asentajamediaa ei voi valita</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7789"/>
        <source>This drive carries read-only live/installer media. Insert or select a data drive instead; the boot media itself is never a repair target.</source>
        <translation>Tämä asema kuljettaa vain live-/asentajan mediaa. Aseta tai valitse data-asema sen sijaan; itse käynnistyslevy ei ole koskaan korjauskohde.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7795"/>
        <source>Unlock or select a Linux system first</source>
        <translation>Avaa tai valitse Linux-järjestelmä ensin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7796"/>
        <source>This encrypted drive has no visible Linux filesystem yet. Use Unlock, refresh devices, and select the target after its Linux root is detected.</source>
        <translation>Tällä salatulla asemalla ei ole vielä näkyvää Linux-tiedostojärjestelmää. Avaa, virkistä laitteita ja valitse kohde sen jälkeen, kun sen Linux root on havaittu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7801"/>
        <source>This drive has no resolvable target component. Refresh devices and select it again.</source>
        <translation>Tämä asema ei ole ratkaistavissa kohdekomponentti. Päivitä laitteet ja valitse se uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7819"/>
        <source>Repair drive selected: %1</source>
        <translation>Korjausasema valittu: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7970"/>
        <source>Requesting administrator authorization</source>
        <translation>Hallinnoija-valtuutuksen pyytäminen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8009"/>
        <location filename="../src/MainWindow.cpp" line="8152"/>
        <source>Administrator authorization active for this Boot Bitch window</source>
        <translation>Tämän Boot Bitch-ikkunan ylläpitäjän valtuutus aktiivinen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8359"/>
        <source>Requesting administrator authorization…</source>
        <translation>Pyydän valvoja-valtuutusta…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8360"/>
        <source>The single Polkit authorization request is already running. The Authorize button re-enables when it finishes.</source>
        <translation>Yksi Polkit-lupapyyntö on jo käynnissä. Valtuuta-painike uudelleen käyttöön, kun se valmistuu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8372"/>
        <source>Administrator authorization deferred — diagnostics will regenerate after you authorize.</source>
        <translation>Administrator valtuutusta on lykätty.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8379"/>
        <source>Administrator authorization failed — press Authorize to retry.</source>
        <translation>Administrator-valtuutus epäonnistui ...</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8614"/>
        <source>another privileged operation</source>
        <translation>toinen etuoikeutettu operaatio</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8618"/>
        <source>superseded by the scope change to %1 and was not queued</source>
        <translation>korvasi soveltamisalan muutos %1 eikä ollut jonossa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8619"/>
        <source>refused because &apos;%1&apos; is still running and was not queued. Wait for it to finish and retry, or cancel it</source>
        <translation>Kieltäytyi, koska &quot;%1&quot; on yhä käynnissä eikä ollut jonossa. Odota, että se valmistuu ja yrittää uudelleen, tai peruuttaa sen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8620"/>
        <source>Privileged request &apos;%1&apos; was %2.</source>
        <translation>%1 oli %2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8622"/>
        <source>ERROR: Privileged request &apos;%1&apos; was %2.
</source>
        <translation>%1 oli %2.
</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8695"/>
        <source>The administrator authorization session could not be established.</source>
        <translation>Hallinnoijan valtuutusistuntoa ei voitu perustaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8701"/>
        <source>Authorization unavailable</source>
        <translation>Valtuutus puuttuu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8711"/>
        <source>The privileged helper session is not running; the plan stopped before its request started.</source>
        <translation>Etuoikeutettu auttajan istunto ei ole käynnissä; suunnitelma pysähtyi ennen pyynnön alkua.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8723"/>
        <source>Using the authorized Boot Bitch administrator session. The GUI itself is still running as your normal user.</source>
        <translation>Käyttämällä valtuutettua Boot Bitch:n ylläpitäjäistuntoa. Itse käyttöliittymä toimii edelleen normaalina käyttäjänä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8768"/>
        <source>Privileged request &apos;%1&apos; exceeded the %2 safety limit; aborting the request and closing the unresponsive helper session so the UI cannot stay busy indefinitely.</source>
        <translation>Etuoikeutettu pyyntö &apos;%1&apos; ylitti %2:n turvallisuusrajan; pyynnön keskeyttäminen ja vastaamattoman auttajan istunnon sulkeminen, jotta käyttöliittymä ei voi pysyä kiireisenä loputtomiin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8771"/>
        <source>The privileged operation exceeded the %1 safety limit and was aborted. Review the output before retrying.</source>
        <translation>Etuoikeutettu toiminta ylitti %1 turvarajan ja se keskeytettiin. Tarkista tuloste ennen kuin yritän uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8880"/>
        <source>Protocol note: a PROMPT record exceeded the 1024-byte payload bound and was dropped; the helper was answered with an empty ANSWER record.</source>
        <translation>Protokolla huomautus: PROMPT-ennätys ylitti 1024-tavuisen kuorman ja se pudotettiin; apuriin vastattiin tyhjällä ANSWER-levyllä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8905"/>
        <source>ERROR: The privileged request exceeded the interactive prompt budget; treating the request as a protocol failure.</source>
        <translation>VIRHE: Etuoikeutettu pyyntö ylitti interaktiivisen nopean budjetin; käsitellään pyyntöä pöytäkirjan epäonnistumisena.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8925"/>
        <source>Shell command is asking for input</source>
        <translation>Shell- komento pyytää syötystä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8928"/>
        <source>The shell command is waiting for input. Review its output below, type the answer and press OK. Cancel — or an empty answer — stops the command.</source>
        <translation>Käsky odottaa syöttöä. Tarkista sen tulosteen alla, kirjoita vastaus ja paina OK. Peruuta komento tai tyhjä vastaus pysäytä komento.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8942"/>
        <source>Answer (for example: y, n, Y, I, N, Z, or a word)</source>
        <translation>Vastaus (esimerkiksi: y, n, Y, I, N, Z tai sana)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8946"/>
        <source>Send Answer</source>
        <translation>Lähetä vastaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9013"/>
        <source>Privileged operation completed successfully. Administrator authorization remains active for this Boot Bitch window.</source>
        <translation>Ensisijainen operaatio suoritettu onnistuneesti. Hallitsijan valtuutus pysyy aktiivisena tämän Boot Bitch-ikkunan osalta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9014"/>
        <source>Privileged operation stopped with an error. Administrator authorization remains active; review the output before closing.</source>
        <translation>Etuoikeutettu operaatio päättyi virheellä. Hallitsijan valtuutus pysyy aktiivisena; tarkista tuloste ennen sulkemista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9038"/>
        <source>The privileged helper session ended unexpectedly. The next root action will require authorization again.</source>
        <translation>Etuoikeutettu avustajakokous päättyi odottamatta. Seuraava juuritoiminta vaatii jälleen luvan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9137"/>
        <source>ERROR: Privileged request &apos;%1&apos; exceeded the %2 safety limit; the request was aborted and the unresponsive helper session was closed. The operation may not have completed.</source>
        <translation>VIRHE: Etuoikeutettu pyyntö &quot;%1&quot; ylitti %2 turvallisuusrajan; pyyntö keskeytettiin ja vastaamaton auttaja istunto suljettiin. Operaatiota ei ole saatettu päätökseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9227"/>
        <source>Unlock not required</source>
        <translation>Avaa lukitusta ei vaadita</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9229"/>
        <source>This drive already has an unlocked Linux filesystem. Boot Bitch will reuse the existing mapper.</source>
        <translation>Tällä asemalla on jo lukitsematon Linux-tiedostojärjestelmä. Boot Bitch käyttää olemassa olevaa karttaa uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9230"/>
        <source>The selected drive does not currently contain a locked LUKS component that needs to be opened.</source>
        <translation>Valittu asema ei tällä hetkellä sisällä lukittua LUKS-komponenttia, joka on avattava.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9237"/>
        <location filename="../src/MainWindow.cpp" line="9244"/>
        <location filename="../src/MainWindow.cpp" line="9270"/>
        <source>Unlock unavailable</source>
        <translation>Avaa lukitusta ei saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9238"/>
        <location filename="../src/MainWindow.cpp" line="18291"/>
        <location filename="../src/MainWindow.cpp" line="18328"/>
        <source>The privileged Boot Bitch helper was not found. Rebuild or install this source tree.</source>
        <translation>Etuoikeutettua Boot Bitch-apulaista ei löydetty. Rakenna tai asenna tämä lähdepuu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9245"/>
        <source>pkexec/Polkit is required to authorize LUKS unlock operations.</source>
        <translation>Pkexec/Polkit vaaditaan LUKS:n laukaisun sallimiseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9252"/>
        <location filename="../src/MainWindow.cpp" line="9298"/>
        <source>Unlock encrypted repair target</source>
        <translation>Avaa salattu korjauskohde</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9253"/>
        <source>Unlock %1?</source>
        <translation>Avaa %1?</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9254"/>
        <source>Target disk: %1

Boot Bitch will ask Polkit for authorization and open a temporary device-mapper mapping. A mapping opened by Boot Bitch remains available for this authorized app session so later diagnostics and repairs can reuse it, then closes when you lock the administrator session or exit. A mapping that was already open before Boot Bitch attached to it is reused but never closed by Boot Bitch.</source>
        <translation>Kohdelevy: %1

Boot Bitch pyytää Polkitilta luvan ja avaa väliaikaisen laitekartoituksen. Boot Bitch:n avaama kartoitus on edelleen saatavilla tätä valtuutettua sovellusistuntoa varten, joten myöhemmin diagnostiikat ja korjaukset voivat käyttää sitä uudelleen, minkä jälkeen se sulkeutuu, kun lukitset järjestelmänvalvojan istunnon tai poistut. Jo ennen Boot Bitch:ää avattua kartoitusta käytetään uudelleen, mutta Boot Bitch ei koskaan sulje sitä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9266"/>
        <source>Unlocking %1</source>
        <translation>Avataan %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9278"/>
        <source>Unlock LUKS repair target</source>
        <translation>Avaa LUKS-korjauskohde</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9307"/>
        <source>LUKS volume:</source>
        <translation>LUKS- tilavuus:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9317"/>
        <source>Enter the passphrase to unlock this volume.</source>
        <translation>Anna salalause avataksesi tämän äänenvoimakkuuden.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9324"/>
        <source>LUKS passphrase</source>
        <translation>LUKS-salalause</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9329"/>
        <source>Administrator authorization is already active. The passphrase is sent only to cryptsetup over the privileged helper pipe and is never logged or placed on a command line.</source>
        <translation>Valtuutus on jo voimassa. Salauslause lähetetään vain kryptattavaksi etuoikeutetun auttajaputken päälle eikä sitä koskaan kirjaudu tai sijoiteta komentoriville.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9356"/>
        <source>Passphrase required</source>
        <translation>Vaadittu salauslause</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9357"/>
        <source>An empty passphrase was not submitted. Enter the LUKS passphrase or choose Cancel.</source>
        <translation>Tyhjää salalausetta ei esitetty. Anna LUKS-salalause tai valitse Peruuta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9368"/>
        <source>Unlock LUKS target</source>
        <translation>Avaa LUKS-kohde</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9397"/>
        <source>Passphrase not accepted</source>
        <translation>Tunnuslausetta ei hyväksytä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9398"/>
        <source>The LUKS passphrase was not accepted.</source>
        <translation>LUKS-salalausetta ei hyväksytty.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9399"/>
        <source>Try again? Administrator authorization remains active, so only the disk passphrase will be requested again.</source>
        <translation>Yritä uudelleen? Hallitsijan valtuutus pysyy aktiivisena, joten vain levyn salalause pyydetään uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9440"/>
        <source>Encrypted target unlocked — select or reselect the repair target</source>
        <translation>Salattu kohde lukittu Valitse tai valitse uudelleen korjauskohde</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9451"/>
        <location filename="../src/MainWindow.cpp" line="9499"/>
        <source>Host maintenance: %1</source>
        <translation>Huolto: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9453"/>
        <source>Running host selected for guarded maintenance: %1
Root component: %2</source>
        <translation>Valmiina vartioituun kunnossapitoon: %1
Juurikomponentti: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9456"/>
        <source>Target: %1</source>
        <translation>Kohde: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9457"/>
        <source>Selected physical repair drive: %1</source>
        <translation>Valittu fyysinen korjausasema: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9460"/>
        <source>
Automatically detected system component: %1</source>
        <translation>
Automaattisesti havaittu järjestelmäkomponentti: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9476"/>
        <source>Running Host: %1</source>
        <translation>Suoritusohjain: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9478"/>
        <source>Running host selected for the Host Shell: %1
Root component: %2</source>
        <translation>Isäntäkuorelle valittu käynnissä oleva isäntä: %1
Juurikomponentti: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9502"/>
        <source>Committed target: %1</source>
        <translation>Kohde: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9504"/>
        <source>Explicit native running-host maintenance is active. Ordinary target repairs, file copy and chroot remain unavailable; running-host Snapper @ snapshot rollback is available in the Snapshots tab.</source>
        <translation>Nimenomaan juoksu-isäntä huolto on aktiivinen. Tavallisia kohteita korjauksia, tiedostojen kopiointia ja chroot edelleen ei ole saatavilla; käynnissä-host Snapper @ tilannekuva rollback on saatavilla Snapshots-välilehdessä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9506"/>
        <source>Row selection is inspection only. Press Select Target to commit a repair drive.</source>
        <translation>Rivivalikoima on vain tarkastus. Paina Valitse Target tehdäksesi korjausaseman.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9507"/>
        <source>Committed repair target. Repair, Diagnostics, Snapshots and File Copy target this physical drive until another drive is explicitly selected with Select Target.
%1</source>
        <translation>Korjattu kohde. Korjaaminen, Diagnostiikka, Snapshots ja Tiedoston kopiointi kohdistaa tämän fyysisen aseman kunnes toinen asema on nimenomaisesti valittu Valitse Target.
%1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9541"/>
        <source>✓ SELECTED TARGET  •  %1</source>
        <translation>✓ VALITTU TAVOITE • %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9552"/>
        <source>Committed repair target. Clicking another row changes only the inspection highlight; repair actions continue to target %1 until Select Target is pressed on another physical drive.
%2</source>
        <translation>Korjattu kohde. Napsauttamalla toista riviä muuttaa vain tarkastuksen kohokohta; korjaustoimet jatkavat %1 kunnes Valitse Target painetaan toiseen fyysiseen asemaan.
%2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9598"/>
        <source>The running host root is not Btrfs, so Btrfs snapshots are not available.</source>
        <translation>Juoksevan isäntäjuurta ei ole Btrfs, joten Btrfs-kuvia ei ole saatavilla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9599"/>
        <source>The selected Linux root is not Btrfs, so Btrfs snapshots are not available.</source>
        <translation>Valittu Linux root ei ole Btrfs, joten Btrfs-kuvia ei ole saatavilla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9606"/>
        <source>Enumerate running-host root snapshots through a temporary privileged read-only Btrfs mount.</source>
        <translation>Lue juoksu-isäntä juurikuvia väliaikaisen etuoikeutetun luku-vain Btrfs-asennus.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9607"/>
        <source>Enumerate root snapshots through a temporary privileged read-only Btrfs mount.</source>
        <translation>Numeroida juurikuvia väliaikaisen etuoikeutetun luku-vain Btrfs-asennus.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9613"/>
        <source>Inspect the selected snapshot read-only.</source>
        <translation>Tarkasta vain valittu kuvakuva.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9614"/>
        <source>Select a snapshot row first.</source>
        <translation>Valitse ensin kuvakaappausrivi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9623"/>
        <location filename="../src/MainWindow.cpp" line="9634"/>
        <source>Select a valid Linux root snapshot first.</source>
        <translation>Valitse ensin kelvollinen Linuxin juurikuva.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9639"/>
        <source>Validate a running-host rollback plan read-only, preserve the running @ as @rollback-before-*, promote a writable snapshot copy and reconcile the boot stack in a scratch chroot. A reboot is required and is never automatic.</source>
        <translation>Validoida juoksu-host rollback suunnitelma vain lukea-vain, säilyttää käynnissä @ kuten @rollback-ennen-*, edistää kirjoitettava kuvakopio ja sovittaa saappaan pino naarmuuntua. Uudelleenkäynnistys vaaditaan eikä se ole koskaan automaattinen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9640"/>
        <source>Validate a rollback plan read-only, preserve the current @ root, promote a writable snapshot copy, reconcile initramfs and the detected bootloader path and auto-restore the old @ if validation fails.</source>
        <translation>Validoidaan vain varasuunnitelma, säilytetään nykyinen @ root, edistetään kirjoitettavaa valokuvakopiota, sovitetaan yhteen initramfs ja havaittu bootloader polku ja palautetaan vanha @ jos validointi epäonnistuu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9711"/>
        <source>Btrfs snapshot preload for %1 is deferred until the running privileged operation finishes; the request was not queued.</source>
        <translation>Btrfs-kuvakuvan esilatausta %1:lle lykätään siihen asti, kunnes käynnissä oleva etuoikeutettu toiminto päättyy; pyyntö ei ollut jonossa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9726"/>
        <source>Loading Btrfs snapshots in the background…</source>
        <translation>Btrfs-kuvien lataaminen taustalla…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9783"/>
        <source>Btrfs snapshot inventory is not applicable: the running host root filesystem is %1, not Btrfs.

No snapshot was loaded and the running host was not modified.</source>
        <translation>Btrfs-kuvan inventointia ei sovelleta: käynnissä oleva isäntäroot-tiedostojärjestelmä on %1, ei Btrfs.

Kuvaa ei ladattu eikä isäntää muutettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9784"/>
        <source>Btrfs snapshot inventory is not applicable: the selected target filesystem is %1, not Btrfs.

No snapshot was loaded and the target was not modified.</source>
        <translation>Btrfs-kuvan inventointia ei sovelleta: valittu kohdetiedostojärjestelmä on %1, ei Btrfs.

Kuvaa ei ladattu eikä kohdetta muutettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9798"/>
        <source>Snapshot inventory unavailable</source>
        <translation>Snapshot-inventaariota ei saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9827"/>
        <source>Btrfs snapshot inventory for %1 is deferred until the running privileged operation finishes; the request was not queued.</source>
        <translation>Btrfs-kuvan inventointia %1:lle lykätään, kunnes juoksun etuoikeutettu toiminta päättyy; pyyntö ei ollut jonossa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9833"/>
        <source>Loading Btrfs snapshots after the running operation finishes…</source>
        <translation>Btrfs-kuvien lataaminen suorituksen jälkeen päättyy…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9839"/>
        <source>Loading running-host Btrfs snapshots</source>
        <translation>Ladataan juoksuisäntä Btrfs-kuvia</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9840"/>
        <source>Loading Btrfs snapshots</source>
        <translation>Ladataan Btrfs-kuvia</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9855"/>
        <source>Load running-host Btrfs snapshots</source>
        <translation>Kuorma ajo-ohjaamo Btrfs kuvat</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9856"/>
        <source>Load Btrfs snapshots</source>
        <translation>Lataa Btrfs-kuvia</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9890"/>
        <source>Snapshot inventory failed. See Logs for details.</source>
        <translation>Kuvan inventaario epäonnistui. Katso lisätietoja lokeista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9943"/>
        <source>Loaded %1 running-host Btrfs root snapshot(s) read-only at %2. Select a row and choose Inspect Selected, or double-click a row, for snapshot-specific validation.

No snapshot or host file was modified.</source>
        <translation>Ladattu %1 juoksu-host Btrfs juurikuva [s] luku- vain %2. Valitse rivi ja valitse Inspect Valittu, tai kaksoisnapsauta riviä, jotta tilannekuvakohtainen validointi.

Katsausta tai isäntätiedostoa ei muutettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9945"/>
        <source>Loaded %1 Btrfs root snapshot(s) read-only at %2. Select a row and choose Inspect Selected, or double-click a row, for snapshot-specific validation.

No snapshot or target file was modified.</source>
        <translation>Ladattu %1 Btrfs juurikuva [s] vain %2:ssä. Valitse rivi ja valitse Inspect Valittu, tai kaksoisnapsauta riviä, jotta tilannekuvakohtainen validointi.

Katsausta tai kohdetiedostoa ei muutettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9949"/>
        <source>No Snapper-style Btrfs root snapshots or Boot Bitch rollback backups were found on the running host. The scan was read-only.</source>
        <translation>Suoritusisännästä ei löytynyt Snapper-tyylisiä Btrfs-juurikuvia tai Boot Bitch-kääntövarmuuksia. Skannaus oli vain luettava.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9950"/>
        <source>No Snapper-style Btrfs root snapshots were found on the selected target. The scan was read-only.</source>
        <translation>Valitusta kohteesta ei löytynyt Snapper-tyylisiä Btrfs-juurikuvia. Skannaus oli vain luettava.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10035"/>
        <source>Inspecting running-host snapshot %1</source>
        <translation>Tarkastaa juoksu-isäntä kuva %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10036"/>
        <source>Inspecting snapshot %1</source>
        <translation>Tarkkailukuva %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10044"/>
        <source>Inspect running-host Btrfs snapshot %1</source>
        <translation>Tarkasta juoksu-isäntä Btrfs kuva %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10045"/>
        <source>Inspect Btrfs snapshot %1</source>
        <translation>Tutki Btrfs-kuva %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10054"/>
        <source>Captured: %1

%2</source>
        <translation>Vangittu: %1

%2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10082"/>
        <source>Rolling back snapshot %1</source>
        <translation>Takakuvakuva %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10088"/>
        <source>Preflight snapshot rollback %1</source>
        <translation>%1:n lentoa edeltävä kuvakaappaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10097"/>
        <source>Rollback preflight captured: %1

%2</source>
        <translation>Paluu ennen lentoa kaapattu: %1

%2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10101"/>
        <source>Rollback preflight failed</source>
        <translation>Peruuttaminen ennen lentoa epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10102"/>
        <source>The selected snapshot did not pass the transactional rollback preflight. No rollback was performed.

Review the Snapshots details pane and Logs.</source>
        <translation>Valittu tilannekuva ei läpäissyt ennen lentoa suoritettavaa paluuta. Peruuttamista ei tehty.

Tarkista Snapshots tiedot paneeli ja lokit.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10108"/>
        <source>Confirm transactional snapshot rollback</source>
        <translation>Vahvista tapahtumakuvan tyhjennys</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10109"/>
        <source>Promote snapshot %1 to the normal writable @ root?</source>
        <translation>Edistää kuvan %1 normaali kirjoitettava @ root?</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10110"/>
        <source>Target disk: %1
Linux filesystem: %2

Boot Bitch will keep the source snapshot unchanged, preserve the current @ under a timestamped rollback backup, set the promoted copy as the Btrfs default, then rebuild/verify initramfs and the detected bootloader path (UKI, GRUB EFI, extlinux or Fedora BLS) and its configuration.

If a critical post-switch validation or boot-stack stage fails, Boot Bitch will automatically restore the preserved @ and reconcile its boot stack.

Separate Btrfs subvolumes such as /home remain outside the root rollback according to the target fstab.</source>
        <translation>Kohdelevy: %1
Linux-tiedostojärjestelmä: %2

Boot Bitch pitää lähdekuvan muuttumattomana, säilyttää nykyisen @:n aikaleimatun varmuuskopioinnin alla, asettaa promoted-kopion Btrfs-oletuksena, sitten jälleenrakentaa/tarkistaa initramfs:n ja havaitun bootloader-polun (UKI, GRUB EFI, extlinux tai Fedora BLS) ja sen kokoonpanon.

Jos kriittinen post-kytkin validointi tai boot-stack vaiheessa epäonnistuu, Boot Bitch palauttaa automaattisesti säilynyt @ ja sovittaa sen boot pino.

Erilliset Btrfs-osavolyymit, kuten /koti, jäävät root rollbackin ulkopuolelle kohteen fstabin mukaisesti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10118"/>
        <location filename="../src/MainWindow.cpp" line="10301"/>
        <source>Continue to Confirmation</source>
        <translation>Jatka vahvistamista</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10128"/>
        <location filename="../src/MainWindow.cpp" line="10311"/>
        <source>Type ROLLBACK to continue</source>
        <translation>Kirjoita ROLLBACK jatkamaan</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10129"/>
        <source>This operation changes the active Btrfs root and rebuilds boot artifacts.

Type ROLLBACK exactly to continue:</source>
        <translation>Tämä operaatio muuttaa aktiivista Btrfs-juurta ja rakentaa uudelleen käynnistysesineitä.

Tyyppi ROLLBACK jatkaa:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10142"/>
        <source>Roll back to Btrfs snapshot %1</source>
        <translation>Palaa Btrfs-kuvaan %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10151"/>
        <source>Rollback finished: %1

%2</source>
        <translation>Palautus valmis: %1

%2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10159"/>
        <source>Rollback completed: %1

%2

Snapshot inventory was cleared because the active root changed. Choose Load Snapshots to refresh it.</source>
        <translation>Palautus valmis: %1

%2

Kuvan inventaario tyhjennettiin, koska aktiivinen juuri muuttui. Valitse Lataa kuvakaappaukset sen päivittämiseksi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10167"/>
        <source>Snapshot rollback complete</source>
        <translation>Snapshot-vastaisku valmis</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10168"/>
        <source>Snapshot %1 was promoted to a writable @ root and the boot stack passed reconciliation.

The previous @ was retained under a timestamped @rollback-before-* name. Reboot using the normal boot path when ready.</source>
        <translation>Snapshot %1 ylennettiin kirjoitettavaksi @ root ja boot pino läpäisi sovinnon.

Edellinen @ säilytettiin aikaleimalla @rollback-ennen-* nimeä. Käynnistä uudelleen normaalilla käynnistyspolulla, kun olet valmis.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10177"/>
        <source>Rollback attempt finished: %1

%2

Cached diagnostics and snapshot inventory were cleared because the rollback request may have changed and/or restored target state.</source>
        <translation>Palautusyritys päättynyt: %1

%2

Akustiset vianmääritykset ja tilannekuvat tyhjennettiin, koska palautuspyyntö on saattanut muuttua ja/tai palauttaa kohdetilan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10185"/>
        <source>Snapshot rollback failed</source>
        <translation>Snapshot-käännös epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10186"/>
        <source>The rollback did not complete successfully. Do not reboot until you review the Snapshots output and Logs. The helper attempts to restore the preserved @ automatically when post-switch validation fails.</source>
        <translation>Se ei onnistunut. Älä käynnistä uudelleen ennen kuin olet tarkistanut Snapshots lähtö ja lokit. Auttaja yrittää palauttaa säilynyt @ automaattisesti, kun post-kytkin validointi epäonnistuu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10202"/>
        <source>Running-host snapshot rollback is only available in Host Maintenance.</source>
        <translation>Running-host-kuvan rollback on saatavilla vain isäntä huolto.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10219"/>
        <location filename="../src/MainWindow.cpp" line="10228"/>
        <source>Running-host snapshot rollback is unavailable.</source>
        <translation>Hyökkäyskuva ei ole käytettävissä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10222"/>
        <source>Running-host snapshot rollback has unknown capability evidence.</source>
        <translation>Hyökkäyskuvassa on tuntemattomia kykyjä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10229"/>
        <source>Run running-host diagnostics in Host Maintenance first.</source>
        <translation>Suorita isännän diagnostiikka.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10232"/>
        <source>Running-host snapshot rollback is available.</source>
        <translation>Running-host kuvakaappaus on saatavilla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10246"/>
        <source>Rollback already staged</source>
        <translation>Perääntyminen on jo järjestetty</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10247"/>
        <source>A running-host rollback to %1 is already staged and takes effect on the next reboot.</source>
        <translation>%1:n juoksuhyökkäys on jo lavastettu ja vaikuttaa seuraavaan uudelleenkäynnistykseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10249"/>
        <source>an earlier snapshot</source>
        <translation>aiempi tilannekatsaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10250"/>
        <source>snapshot %1</source>
        <translation>Kuva %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10251"/>
        <source>Rolling back again preserves the currently running root as a new @rollback-before-* backup and replaces the staged snapshot. The earlier staged root remains on disk but is no longer the recorded undo point.

Continue only if you intend to replace the staged rollback.</source>
        <translation>Takaisin kääntyminen säilyttää käynnissä olevan rootin uutena @rollback-ennen-* varmuuskopiona ja korvaa lavastetun tilannekuvan. Aiemmin lavastettu juuri pysyy levyllä, mutta se ei ole enää tallennettu perumispiste.

Jatka vain, jos aiot korvata lavastetun varauksen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10256"/>
        <source>Replace Staged Rollback</source>
        <translation>Korvaa vaiheittainen palautus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10264"/>
        <source>Rolling back running-host snapshot %1</source>
        <translation>Vierivä takaisin käynnissä-host kuva %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10270"/>
        <source>Preflight running-host rollback %1</source>
        <translation>%1:n lentoa edeltävä varalento</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10279"/>
        <source>Running-host rollback preflight captured: %1

%2</source>
        <translation>Suoritus-host-vastaisku ennen lentoa kaapattu: %1

%2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10283"/>
        <source>Host rollback preflight failed</source>
        <translation>Isännän paluu ennen lentoa epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10284"/>
        <source>The selected snapshot did not pass the running-host rollback preflight. No rollback was performed.

Review the Snapshots details pane and Logs.</source>
        <translation>Valittu kuva ei läpäissyt juoksu-host paluuta ennen lentoa. Peruuttamista ei tehty.

Tarkista Snapshots tiedot paneeli ja lokit.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10290"/>
        <source>Confirm running-host snapshot rollback</source>
        <translation>Vahvista ajo-isäntä-kuvan muutos</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10291"/>
        <source>Roll the running host back to snapshot %1?</source>
        <translation>Viedäänkö juontaja takaisin kuvaan %1?</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10292"/>
        <source>Running host: %1
Root filesystem: %2

The running host keeps running the current root until you reboot. On the next reboot it will start the selected snapshot instead.

Snapper/Boot Bitch take a snapshot of the current system first, so the present state is preserved automatically as an @rollback-before-* undo point.

Boot Bitch cannot guarantee that no data is lost: changes made after the selected snapshot are not part of the rolled-back root, and separate subvolumes such as /home are not rolled back.

All users are signed out and unsaved work is lost when the host reboots. The reboot is never automatic.</source>
        <translation>Suoritettava isäntä: %1
Juuritiedostojärjestelmä: %2

Juokseva isäntä jatkaa käynnissä nykyisen juuren kunnes käynnistät uudelleen. Seuraavassa uudelleenkäynnistyksessä se käynnistää valitun kuvan sen sijaan.

Snapper/Boot Bitch ottaa ensin kuvan nykyisestä järjestelmästä, joten nykyinen tila säilyy automaattisesti @rollback-ennen-*-perumispisteenä.

Boot Bitch ei voi taata, että tietoja ei menetetä: valitun tilannekuvan jälkeen tehdyt muutokset eivät ole osa rullattua back-juurta, eikä erillisiä alavolyymejä, kuten /home, ole rullattu takaisin.

Kaikki käyttäjät kirjautuvat ulos ja tallentamaton työ katoaa, kun isäntä käynnistää uudelleen. Uudelleenkäynnistys ei ole koskaan automaattinen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10312"/>
        <source>This operation stages the selected snapshot as the running host&apos;s next root and rebuilds boot artifacts.

Type ROLLBACK exactly to continue:</source>
        <translation>Tämä operaatio vaiheittaa valitun kuvan juoksuisännän seuraavaksi juureksi ja rakentaa uudelleen käynnistysesineitä.

Tyyppi ROLLBACK jatkaa:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10325"/>
        <source>Roll back the running host to snapshot %1</source>
        <translation>Siirrä käynnissä oleva isäntä takaisin %1-kuvaan</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10334"/>
        <source>Running-host rollback finished: %1

%2</source>
        <translation>Running-host takaisin valmis: %1

%2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10346"/>
        <source>Running-host rollback staged: %1

%2

Snapshot inventory and cached running-host diagnostics were cleared because the next boot will start the promoted root. Reboot when ready; the reboot is never automatic.</source>
        <translation>Running-host-retkellä: %1

%2

Snapshot inventaario ja välimuistissa juokseva isännän diagnostiikka tyhjennettiin, - koska seuraava saapas käynnistää promotoidun juuren. Käynnistä uudelleen, kun olet valmis; uudelleenkäynnistys ei ole koskaan automaattinen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10356"/>
        <source>Running-host rollback staged</source>
        <translation>Running-host-retket</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10357"/>
        <source>Snapshot %1 is staged as the running host&apos;s next root.</source>
        <translation>Snapshot %1 on lavastettu juoksuisännän seuraavaksi juureksi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10358"/>
        <source>Reboot required — the running host will start snapshot %1 after the next reboot. The previous root was retained as an @rollback-before-* undo point and the boot stack passed reconciliation.

Use Reboot Now to reboot immediately, or Later to keep working and reboot manually.</source>
        <translation>Uudelleenkäynnistys vaaditaan ... käynnissä isäntä aloittaa kuvan %1 jälkeen seuraavan uudelleenkäynnistyksen. Edellinen juuri säilytettiin @rollback-ennen-* peruuttaa kohta ja boot pino läpäisi täsmäytys.

Käytä Reboot Nyt uudelleenkäynnistää välittömästi, tai Myöhemmin jatkaa työskentelyä ja uudelleenkäynnistä manuaalisesti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10381"/>
        <source>Running-host rollback attempt finished: %1

%2

Cached running-host diagnostics and snapshot inventory were cleared. Do not reboot until you review the Snapshots output and Logs.</source>
        <translation>Running-host-vastaiskuyritys päättynyt: %1

%2

Valmiina juoksevan isännän diagnostiikka ja tilannekuvat tyhjennettiin. Älä käynnistä uudelleen ennen kuin olet tarkistanut Snapshots lähtö ja lokit.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10388"/>
        <location filename="../src/MainWindow.cpp" line="10391"/>
        <source>Host rollback failed</source>
        <translation>Isännän palautus epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10389"/>
        <source>The running-host rollback did not complete and automatic recovery could not be proven. DO NOT REBOOT. Review the Snapshots output and Logs, and repair the boot stack from another system before rebooting.

The helper output carries HOST_ROLLBACK_RECOVERY and any CRITICAL lines verbatim.</source>
        <translation>Running-host takaisin ei ollut valmis eikä automaattista palautumista voitu todistaa. ÄLÄ REBOOT. Tarkista Snapshots lähtö ja lokit, ja korjata boot pino toisesta järjestelmästä ennen uudelleenkäynnistystä.

Auttimen lähtö kuljettaa HOST ROLLBACK RECOVERY ja kaikki CRITICAL lines sanatarkasti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10392"/>
        <source>The running-host rollback did not complete. The helper reports the preserved @ was restored automatically; do not reboot until you review the Snapshots output and Logs.</source>
        <translation>Running-host takaisin ei valmistunut. Auttaja raportoi säilyneen @:n palautuneen automaattisesti; älä käynnistä uudelleen ennen kuin tarkistat Snapshots-lähdön ja lokit.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10503"/>
        <location filename="../src/MainWindow.cpp" line="10504"/>
        <source>Reboot the running host now?</source>
        <translation>Käynnistätkö juontajan uudelleen?</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10505"/>
        <source>The rolled-back snapshot is already staged and takes effect on the next boot. Rebooting now signs out all users and closes unsaved work.

The host reboots only after this separate confirmation.</source>
        <translation>Kääritty takaisin-kuva on jo järjestetty ja tulee voimaan seuraavan boot. Käynnistän nyt kaikki käyttäjät ja suljen tallentamattomat työt.

Isäntä käynnistyy vasta tämän erillisen vahvistuksen jälkeen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10517"/>
        <source>Rebooting running host</source>
        <translation>Käynnistetään uudelleen käynnissä oleva isäntä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10522"/>
        <source>Reboot the running host</source>
        <translation>Käynnistä käynnissä oleva isäntä uudelleen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10530"/>
        <source>Running-host reboot scheduled; the host is restarting.</source>
        <translation>Suoritus-host uudelleenkäynnistys suunniteltu; isäntä käynnistää uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10537"/>
        <source>Host reboot not scheduled</source>
        <translation>Uudelleenkäynnistystä ei ole suunniteltu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10539"/>
        <source>The running host did not schedule a reboot. The staged rollback and its reminder remain in effect.</source>
        <translation>Juokseva isäntä ei suunnitellut uudelleenkäynnistystä. Lavastettu peruutus ja sen muistutus ovat edelleen voimassa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10557"/>
        <source>Repair → Host file copy</source>
        <translation>Korjaus → Kopioi tiedosto</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10558"/>
        <source>Host → Repair file copy</source>
        <translation>Host → Korjaustiedoston kopio</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10562"/>
        <source>1. Select source paths from repaired system</source>
        <translation>1. Valitse lähdepolut korjatusta järjestelmästä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10563"/>
        <source>2. Choose destination on this host</source>
        <translation>2. Valitse kohde tässä isäntä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10564"/>
        <source>Add File Path…</source>
        <translation>Lisää tiedostopolku…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10565"/>
        <source>Add Folder Path…</source>
        <translation>Lisää kansiopolku…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10566"/>
        <source>Enter an absolute file path as it appears inside the repaired system, for example /home/user/document.txt.</source>
        <translation>Anna absoluuttinen tiedostopolku sellaisena kuin se näkyy korjatun järjestelmän sisällä, esimerkiksi /home/user/document.txt.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10567"/>
        <source>Enter an absolute folder path as it appears inside the repaired system.</source>
        <translation>Anna absoluuttinen kansiopolku sellaisena kuin se näkyy korjatussa järjestelmässä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10568"/>
        <source>Choose a host destination folder</source>
        <translation>Valitse kohdekansio</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10569"/>
        <source>Browse…</source>
        <translation>Selaa…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10572"/>
        <source>1. Select source files or folders from this host</source>
        <translation>1. Valitse lähdetiedostot tai kansiot tästä isäntä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10573"/>
        <source>2. Choose destination in repaired system</source>
        <translation>2. Valitse kohde korjatussa järjestelmässä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10576"/>
        <source>Choose one or more source files from the running host.</source>
        <translation>Valitse yksi tai useampi lähdekooditiedosto käynnissä olevasta isäntästä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10577"/>
        <source>Choose a source folder from the running host.</source>
        <translation>Valitse lähdekansio käynnissä olevasta isännästä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10579"/>
        <source>Select a repair target first</source>
        <translation>Valitse korjauskohde ensin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10580"/>
        <source>Choose an absolute destination path inside the repaired system</source>
        <translation>Valitse korjattuun järjestelmään absoluuttinen kohdepolku</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10581"/>
        <source>Browse Target Folders…</source>
        <translation>Selaa kohdekansioita…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10582"/>
        <source>Browse the selected repair system through temporary read-only mounts and choose an absolute destination path. No target files are changed while browsing.</source>
        <translation>Selaa valittua korjausjärjestelmää tilapäisten lukulaitteiden avulla ja valitse ehdoton kohdepolku. Kohdetiedostoja ei muuteta selattaessa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10604"/>
        <source>Run a guarded rsync dry-run. The selected repair filesystem remains read-only.</source>
        <translation>Tee vartioitu Rsync kuivaajo. Valittu korjaustiedostojärjestelmä pysyy vain luettavana.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10605"/>
        <source>Copy staged items and verify them. Existing same-name destination content can be overwritten; unrelated destination files are never deleted.</source>
        <translation>Kopioi lavastetut kohteet ja varmista ne. Olemassa oleva samanniminen kohdesisältö voidaan korvata; etuyhteydettömiä kohdetiedostoja ei koskaan poisteta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10618"/>
        <source>Choose host destination folder</source>
        <translation>Valitse kohdekansio</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10631"/>
        <source>Select a repair target</source>
        <translation>Valitse korjauskohde</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10632"/>
        <source>Select the repair drive in Systems before choosing a destination inside it.</source>
        <translation>Valitse järjestelmän korjausasema ennen kohdetta sen sisällä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10642"/>
        <source>Select repair-system destination</source>
        <translation>Valitse korjausjärjestelmän kohde</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10649"/>
        <source>Choose a destination folder inside the repaired system</source>
        <translation>Valitse kohdekansio korjatun järjestelmän sisällä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10658"/>
        <source>Browse folders from the selected repaired system through temporary read-only mounts. Nothing is written while browsing, and the guarded copy validates the chosen path again before any write.</source>
        <translation>Selaa valitun korjatun järjestelmän kansioita tilapäisten lukulaitteiden avulla. Mitään ei kirjoiteta selailun aikana, ja vartioitu kopio validoi valitun polun uudelleen ennen kirjoittamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10667"/>
        <source>Absolute path inside repaired system (for example /home/user/Recovered)</source>
        <translation>Absoluuttinen polku korjatun järjestelmän sisällä (esim. /koti/käyttäjä/peritty)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10669"/>
        <source>Browse repair folders…</source>
        <translation>Selaa korjauskansioita…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10670"/>
        <source>Open a read-only view of the selected repair system&apos;s folders. The target is unmounted again after each directory listing.</source>
        <translation>Avaa vain lukunäkymä valitun korjausjärjestelmän kansioista. Kohdetta ei ole asennettu uudelleen jokaisen listan jälkeen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10673"/>
        <source>Clear the destination path.</source>
        <translation>Tyhjentäkää kohde.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10688"/>
        <location filename="../src/MainWindow.cpp" line="10774"/>
        <source>Browse repaired-system folders</source>
        <translation>Selaa korjattuja järjestelmäkansioita</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10701"/>
        <source>Choose a folder from the repaired system</source>
        <translation>Valitse kansio korjatusta järjestelmästä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10710"/>
        <source>The selected repair filesystem is mounted read-only only for the current directory listing. The mount is removed after the request; the host filesystem is never used as the folder tree.</source>
        <translation>Valittu korjaustiedostojärjestelmä on asennettu vain luettavaksi nykyisen kansion listalle. Asennus poistetaan pyynnön jälkeen; isäntätiedostojärjestelmää ei koskaan käytetä kansiopuuna.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10735"/>
        <source>Up</source>
        <translation>Ylös</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10736"/>
        <source>Open Selected</source>
        <translation>Avaa valittu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10738"/>
        <source>Show the parent folder in the repaired system.</source>
        <translation>Näytä kantakansio korjatussa järjestelmässä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10739"/>
        <source>Open the selected repaired-system folder.</source>
        <translation>Avaa valittu korjattu järjestelmäkansio.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10740"/>
        <source>Reload this repaired-system folder through a fresh read-only mount.</source>
        <translation>Lataa tämä korjattu järjestelmäkansio uudelleen vain uuden lukuliitoksen läpi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10747"/>
        <source>Read-only target view; no files are modified.</source>
        <translation>Lue vain kohdenäkymä; tiedostoja ei muuteta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10753"/>
        <source>Choose This Folder</source>
        <translation>Valitse tämä kansio</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10766"/>
        <source>Invalid repair-system path.</source>
        <translation>Virheellinen korjausjärjestelmän polku.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10778"/>
        <source>Unable to read this repaired-system folder. Review Logs for the helper error.</source>
        <translation>Korjattua järjestelmäkansiota ei voitu lukea. Tarkista lokit auttaja virhe.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10808"/>
        <source>Current repaired-system folder: %1</source>
        <translation>Nykyinen korjattu järjestelmäkansio: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10812"/>
        <source>Read-only target view; %1 subfolder(s) found. Nothing was modified.</source>
        <translation>Read-ainoa kohdenäkymä; %1 alikansioita löytyi. Mitään ei muutettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10865"/>
        <location filename="../src/MainWindow.cpp" line="10896"/>
        <location filename="../src/MainWindow.cpp" line="10943"/>
        <source>Invalid repair path</source>
        <translation>Virheellinen korjauspolku</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10866"/>
        <location filename="../src/MainWindow.cpp" line="10897"/>
        <location filename="../src/MainWindow.cpp" line="10944"/>
        <source>Use an absolute path inside the repaired system. Parent-directory escape paths are not accepted.</source>
        <translation>Käytä absoluuttista polkua korjatun järjestelmän sisällä. Vanhempainhakemiston pakoteitä ei hyväksytä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10886"/>
        <source>Repair-system source file</source>
        <translation>Korjausjärjestelmän lähdetiedosto</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10887"/>
        <source>Enter an absolute file path inside the repaired system:</source>
        <translation>Anna absoluuttinen tiedostopolku korjattuun järjestelmään:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10911"/>
        <source>Select host files</source>
        <translation>Valitse isäntätiedostot</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10933"/>
        <source>Repair-system source folder</source>
        <translation>Korjausjärjestelmän lähdekansio</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10934"/>
        <source>Enter an absolute folder path inside the repaired system:</source>
        <translation>Anna korjattuun järjestelmään absoluuttinen kansiopolku:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10958"/>
        <source>Select host folder</source>
        <translation>Valitse kansio</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10998"/>
        <source>Add at least one source file or folder.</source>
        <translation>Lisää vähintään yksi lähdekooditiedosto tai kansio.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11002"/>
        <source>Choose a destination folder.</source>
        <translation>Valitse kohdekansio.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11006"/>
        <source>rsync is required for verified File Copy.</source>
        <translation>rsync vaaditaan todennettu tiedostokopio.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11015"/>
        <source>Repair → Host requires an existing, non-symlink host destination folder.</source>
        <translation>Korjaus → Isäntä vaatii olemassa olevan, ei-symlin isäntäkohdekansion.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11020"/>
        <source>Host → Repair requires a specific absolute path inside the repaired system.</source>
        <translation>Host → Korjaus vaatii tietyn absoluuttisen polun korjattuun järjestelmään.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11093"/>
        <source>Read-only diagnostics are being generated. Commands stay disabled until the refresh completes so no command runs against a half-refreshed evidence state.</source>
        <translation>Vain lukudiagnostiikkaa luodaan. Komennot pysyvät poissa käytöstä, kunnes päivitys on valmis. Mikään komento ei ole vastaan puoliksi virkistynyttä todistusaineistoa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11098"/>
        <location filename="../src/MainWindow.cpp" line="11154"/>
        <source>Execute a command on the running host as root.</source>
        <translation>Suorita komento käynnissä isännälle juurina.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11110"/>
        <location filename="../src/MainWindow.cpp" line="11248"/>
        <source>Host shell</source>
        <translation>Komentotulkki</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11118"/>
        <source>Run a command on the running host as root (sudo is not needed). Commands are executed directly on the active system. When a command asks a question, Boot Bitch shows it in a popup and sends your answer back to the command; cancelling stops the command. Non-interactive flags such as apt-get -y upgrade remain recommended for unattended runs. Output is kept in this window and in the application log.</source>
        <translation>Suorita käynnissä olevan isännän käsky juurina (suota ei tarvita). Komennot suoritetaan suoraan aktiivisessa järjestelmässä. Kun komento esittää kysymyksen, Boot Bitch näyttää sen ponnahdusikkunassa ja lähettää vastauksesi takaisin komentoon; komento keskeytetään. Ei-interaktiiviset liput, kuten apt-get-y-päivitys, ovat edelleen suositeltavia valvomattomille ajoille. Tulosta säilytetään tässä ikkunassa ja sovelluslokissa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11125"/>
        <source>Command to run on the running host, for example: apt update</source>
        <translation>Suoritettava komento, esimerkiksi: apt-päivitys</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11132"/>
        <source>Run on Host</source>
        <translation>Suorita isäntä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11136"/>
        <source>Commands can modify the running host. Review each command before running it.</source>
        <translation>Komento voi muuttaa käynnissä olevaa isäntää. Tarkista jokainen komento ennen sen suorittamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11150"/>
        <source>Shell</source>
        <translation>Shell</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11151"/>
        <source>Host Shell</source>
        <translation>Isäntäkuori</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11155"/>
        <source>Execute a command inside the selected repair system as root.</source>
        <translation>Suorita komento valitun korjausjärjestelmän sisällä juurina.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11195"/>
        <source>• %1: %2 changed from &apos;%3&apos; to &apos;%4&apos;</source>
        <translation>• %1: %2 vaihdettu &apos;%3&apos;:stä &apos;%4&apos;:ksi</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11219"/>
        <source>Host shell unavailable</source>
        <translation>Komentotulkkia ei ole saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11220"/>
        <source>Chroot shell unavailable</source>
        <translation>Chroot-kuori ei saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11235"/>
        <source>The running-host identity is unresolved; no host shell command was sent.</source>
        <translation>Juoksu-isäntä-identiteetti on selvittämätön; mitään komentoa ei lähetetty.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11236"/>
        <source>The repair target identity is incomplete; no chroot shell command was sent.</source>
        <translation>Korjaa kohde on epätäydellinen; Chroot- komentoa ei lähetetty.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11244"/>
        <source>Running host shell command</source>
        <translation>Suoritetaan isäntäkomentoa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11245"/>
        <source>Running chroot shell command</source>
        <translation>Suoritetaan chroot-komentoa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11292"/>
        <source>the affected repository</source>
        <translation>kyseinen tietokanta</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11298"/>
        <source>Repository metadata changed</source>
        <translation>Versiovaraston metatiedot muuttuneet</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11299"/>
        <source>The repository changed its release metadata:
%1</source>
        <translation>Arkisto muutti julkaisumetadataa:
%1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11305"/>
        <source>The package manager refused the change. Allowing it re-runs the command once with Acquire::AllowReleaseInfoChange=true; the change is accepted for this retry only. Signature, key and package verification remain enforced.

Retry command:
%1</source>
        <translation>Pakettipäällikkö kieltäytyi muutoksesta. Sallien sen uudelleen suorittaa komennon kerran Acquirella::AllowReleaseInfoChange=true; muutos hyväksytään vain tätä uudelleen. Allekirjoitus, avain ja pakettitarkastus on edelleen voimassa.

Uudelleenkomento:
%1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11313"/>
        <source>Allow and Retry</source>
        <translation>Salli ja yritä uudelleen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11332"/>
        <source>Host shell response incomplete</source>
        <translation>Komentotulkin vastaus epätäydellinen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11332"/>
        <source>Chroot shell response incomplete</source>
        <translation>Chroot-kuoren vaste epätäydellinen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11333"/>
        <source>The privileged helper did not return a complete response for this command. The request was treated as failed; review the output and Logs before retrying.</source>
        <translation>Etuoikeutettu apulainen ei palauttanut täydellistä vastausta tähän komentoon. Pyyntöä käsiteltiin epäonnistuneena; tarkista tuloste ja lokit ennen uudelleentarkastelua.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11368"/>
        <location filename="../src/MainWindow.cpp" line="11395"/>
        <source>File Copy unavailable</source>
        <translation>Tiedostokopiota ei ole saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11377"/>
        <source>Previewing file copy</source>
        <translation>Esikatselen tiedostokopiota</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11388"/>
        <source>Preview File Copy</source>
        <translation>Esikatselutiedoston kopiointi</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11402"/>
        <source>Smart destination ownership</source>
        <translation>Älykäs määränpään omistus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11425"/>
        <source>… %1 more item(s)</source>
        <translation>… %1 lisää kohdetta [s)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11430"/>
        <source>Confirm File Copy</source>
        <translation>Vahvista tiedostokopio</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11431"/>
        <source>%1 — copy and verify %2 source item(s)?</source>
        <translation>%1 ... Kopioi ja varmista %2-lähdetieto[s]?</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11433"/>
        <source>Destination: %1
Ownership: %2

Sources:
• %3

</source>
        <translation>Kohde: %1
Omistusoikeus: %2

Lähteet:
• %3

</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11436"/>
        <source>The repair target stays read-only. Existing same-name files in the host destination may be overwritten. The privileged helper refuses system-critical host destinations.

</source>
        <translation>Korjaaja pysyy lukukunnossa. Olemassa olevat samannimiset tiedostot isäntäkohteessa voidaan korvata. Etuoikeutettu auttaja kieltäytyy järjestelmäkriittisistä isäntäkohteista.

</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11438"/>
        <source>The selected repair filesystem is promoted read-write only after host/target safety checks. Existing same-name target files may be overwritten.

</source>
        <translation>Valittua korjaustiedostojärjestelmää edistetään lukemalla vain isäntä-/kohdeturvallisuustarkastusten jälkeen. Olemassa olevat samannimiset kohdetiedostot voidaan korvata.

</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11440"/>
        <source>SENSITIVE TARGET PATH: this destination can change boot or operating-system files.

</source>
        <translation>SENSIIVINEN TAVOITEPATH: Tämä kohde voi muuttaa käynnistys- tai käyttöjärjestelmätiedostoja.

</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11443"/>
        <source>rsync does not use --delete, so unrelated destination files remain. A second checksum/metadata pass and SHA-256 verification of regular files run after the copy.</source>
        <translation>rsync ei käytä --delete, joten etuyhteydetön kohde tiedostot pysyvät. Toinen tarkistussumma/metadatapassi ja SHA-256-tarkistus säännöllisistä tiedostoista suoritetaan kopion jälkeen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11452"/>
        <source>Copying files</source>
        <translation>Kopioidaan tiedostoja</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11467"/>
        <source>File Copy — %1</source>
        <translation>Tiedoston kopiointi %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11478"/>
        <source>Available — KF6 KAuth linked</source>
        <translation>Saatavilla KF6 KAuth linked</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11480"/>
        <source>Not compiled — optional integration unavailable</source>
        <translation>Ei ole laadittua valinnaista integrointia</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11492"/>
        <source>Available</source>
        <translation>Saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11492"/>
        <source>Missing</source>
        <translation>Puuttuu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11571"/>
        <source>Running Host diagnostics require Host Maintenance. Choose Enter Host Maintenance on the protected running-host card in Systems first.</source>
        <translation>Running Isännän diagnostiikka vaatii ylläpitoa. Valitse Enter Isäntä Kunnossapito suojattu juoksu-host-kortti Systems ensin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11658"/>
        <source>Individual diagnostics have changed. Please re-run all diagnostics.</source>
        <translation>Yksittäiset diagnoosit ovat muuttuneet. Tarkista kaikki diagnostiikat.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11665"/>
        <source>The selected system changed after a repair action. Please re-run the %1 diagnostic before reviewing or running this repair action.</source>
        <translation>Valittu järjestelmä muuttui korjaustoimen jälkeen. Suorita %1-diagnostiikka uudelleen ennen tämän korjaustoimen tarkistamista tai suorittamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11677"/>
        <source>Select a repair target in Systems.</source>
        <translation>Valitse korjauskohde järjestelmistä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11697"/>
        <source>Cached: %1 result</source>
        <translation>Välimuisti: %1:n tulos</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11700"/>
        <source>Cached: %1 — read-only selected repair-system result. Selecting another diagnostic and returning here keeps this output; use Re-run only for fresh data.</source>
        <translation>Tallelokero: %1 ... Lue vain valittu korjausjärjestelmä tulos. Toisen diagnostisen ja palaavan vianmäärityksen valinta pitää tämän tuloksen; käytä Re-runia vain tuoreisiin tietoihin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11701"/>
        <source>Cached: %1 — protected running-host result.</source>
        <translation>Välimuisti: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11702"/>
        <source>Re-run Diagnostic</source>
        <translation>Uudelleenajodiagnostiikka</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11706"/>
        <source>Please re-run this diagnostic after the last repair or target configuration change.</source>
        <translation>Suorita tämä vianmääritys uudelleen viimeisen korjaus- tai kohdeasetusmuutoksen jälkeen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11708"/>
        <source>Available — read-only selected repair-system inspection; administrator authorization is already active for this Boot Bitch window.</source>
        <translation>Saatavana vain luettu korjausjärjestelmän tarkastus; järjestelmänvalvojan valtuutus on jo käytössä tässä Boot Bitch-ikkunassa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11709"/>
        <source>Available — read-only selected repair-system inspection; the first root action will request administrator authorization.</source>
        <translation>Saatavana vain lukea valitun korjaus-järjestelmän tarkastus; ensimmäinen juuri toiminto pyytää ylläpitäjän valtuutus.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11712"/>
        <source>Please re-run this diagnostic after the last repair action.</source>
        <translation>Suorita tämä vianmääritys uudelleen viimeisen korjaustoimen jälkeen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11713"/>
        <source>Available — protected running host</source>
        <translation>Saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11969"/>
        <source>Run running-host diagnostics</source>
        <translation>Suorita isännän diagnostiikka</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11970"/>
        <source>Read-only running-host diagnostic</source>
        <translation>Vain ajo-ohjaindiagnostiikka</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11973"/>
        <source>Run All target diagnostics</source>
        <translation>Suorita kaikki kohdediagnostiikka</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11974"/>
        <source>Read-only target diagnostic</source>
        <translation>Vain lukukohdediagnostiikka</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12029"/>
        <source>Starting read-only %1 diagnostic &apos;%2&apos; on %3 (%4).</source>
        <translation>Käynnistetään %1-diagnostiikka &quot;%2&quot; %3:llä (%4).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12040"/>
        <source>Read-only %1 diagnostic &apos;%2&apos; %3.</source>
        <translation>Lue vain %1-diagnostiikka &apos;%2&apos; %3.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12042"/>
        <source>completed</source>
        <translation>valmis</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12042"/>
        <source>failed</source>
        <translation>epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12078"/>
        <location filename="../src/MainWindow.cpp" line="12158"/>
        <source>%1 finished with exit code 0 (success).</source>
        <translation>%1 päättyi lähtökoodiin 0 (menestys).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12093"/>
        <location filename="../src/MainWindow.cpp" line="12210"/>
        <source>Target diagnostic unavailable</source>
        <translation>Kohdediagnoosi ei saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12114"/>
        <source>Discarded the read-only target diagnostic &apos;%1&apos; because the active repair target changed while the request was running.</source>
        <translation>Hylättiin pelkkä lukukohdediagnostiikka &quot;%1,&quot; koska aktiivinen korjauskohde muuttui pyynnön ollessa käynnissä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12174"/>
        <location filename="../src/MainWindow.cpp" line="12209"/>
        <source>Host diagnostic unavailable</source>
        <translation>Isäntädiagnostiikka ei saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12395"/>
        <source>No repair target is ready for diagnostics.</source>
        <translation>Mikään korjauskohde ei ole valmis diagnostiikkaan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12398"/>
        <source>The selected repair target is ready for diagnostics.</source>
        <translation>Valittu korjauskohde on valmis diagnostiikkaan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12403"/>
        <source>Running Host diagnostics require Host Maintenance. Choose Host Maintenance on the protected running-host card in Systems first.</source>
        <translation>Running Isännän diagnostiikka vaatii ylläpitoa. Valitse ensin suojatun juoksu-host-kortin isäntähuolto.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12409"/>
        <source>The running host is not resolved for diagnostics.</source>
        <translation>Juoksevaa isäntää ei ole selvitetty diagnostiikkaan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12412"/>
        <source>The running host is ready for diagnostics.</source>
        <translation>Juokseva isäntä on valmis diagnostiikkaan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12427"/>
        <source>Target diagnostics were invalidated by a repair or target change.</source>
        <translation>Kohdediagnoosi mitätöitiin korjauksella tai kohteen muutoksella.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12431"/>
        <source>Target diagnostic sections were invalidated by a repair action and await regeneration.</source>
        <translation>Kohteen diagnostiset osat mitätöitiin korjaustoimi ja odotetaan uudistumista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12435"/>
        <source>No cached diagnostics belong to the selected target.</source>
        <translation>Mikään välimuistidiagnostiikka ei kuulu valittuun kohteeseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12439"/>
        <source>No cached diagnostics exist for the selected target.</source>
        <translation>Valitulle kohteelle ei ole välimuistidiagnostiikkaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12446"/>
        <source>Running-host diagnostic sections were invalidated by a repair action and await regeneration.</source>
        <translation>Korjaaminen mitätöitiin ja odotetaan regeneraatiota.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12450"/>
        <source>No cached running-host diagnostics exist.</source>
        <translation>Mitään välimuistia ei ole.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12466"/>
        <source>Automatic diagnostics regeneration is disabled in Settings.</source>
        <translation>Automaattinen diagnostiikan regenerointi ei ole käytössä Asetuksissa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12469"/>
        <source>A diagnostic or repair operation is already in progress.</source>
        <translation>Diagnostinen tai korjaus on jo käynnissä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12479"/>
        <source>Current-scope diagnostic evidence is fresh.</source>
        <translation>Tämänhetkinen diagnostinen näyttö on tuore.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12501"/>
        <source>Unknown repair tool: %1.</source>
        <translation>Tuntematon korjaustyökalu: %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12505"/>
        <location filename="../src/MainWindow.cpp" line="12694"/>
        <location filename="../src/MainWindow.cpp" line="18271"/>
        <source>Select a repair target in Systems first.</source>
        <translation>Valitse ensin järjestelmän korjauskohde.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12508"/>
        <location filename="../src/MainWindow.cpp" line="12540"/>
        <source>Diagnostic evidence belongs to a different target. Run diagnostics for the selected target.</source>
        <translation>Diagnostiset todisteet kuuluvat eri kohteeseen. Suorita valitun kohteen diagnostiikka.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12516"/>
        <source>Target state changed after the last diagnostic or repair action. Please regenerate diagnostics before starting another repair.</source>
        <translation>Kohdetila muuttui viimeisen vianmäärityksen tai korjaustoimen jälkeen. Korjaa diagnostiikka ennen uuden korjauksen aloittamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12520"/>
        <location filename="../src/MainWindow.cpp" line="12559"/>
        <source>Repair tool %1 is available in the selected scope&apos;s cached diagnostics.</source>
        <translation>Korjaamotyökalu %1 on saatavilla valitun sovelluksen välimuistidiagnostiikassa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12611"/>
        <source>Unrecognised running-host default-entry capability evidence: %1. Run running-host diagnostics again, and update Boot Bitch if the helper format changed.</source>
        <translation>Tunnistamaton juoksu-isäntä-oletus-sisääntulokykyä osoittava näyttö: %1. Suorita juoksu-isäntä-diagnostiikka uudelleen ja päivitä Boot Bitch, jos auttajamuoto on muuttunut.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12617"/>
        <source>The running host&apos;s verified default boot entry can be restored or promoted.</source>
        <translation>Juoksevan isäntän todennettu oletuskäynnistys voidaan palauttaa tai edistää.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12623"/>
        <source>The cached running-host diagnostics do not decide whether the host default entry can be restored. Regenerate running-host diagnostics in Host Maintenance, and update Boot Bitch if the helper format changed.</source>
        <translation>Välimuistin juoksu-host diagnostiikka ei päätä, onko isäntä oletus syöte voidaan palauttaa. Uudista juoksu-isäntä-diagnostiikka isäntä huolto, ja päivittää Boot Bitch jos auttaja muodossa muuttunut.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12624"/>
        <source>No running-host default-entry capability evidence is cached. Run read-only running-host diagnostics in Host Maintenance before using Make Default.</source>
        <translation>- Ei ole. Suorita vain luku-asentajan diagnostiikka isännän kunnossapidossa ennen Make Oletuksen käyttöä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12646"/>
        <source>Restore/ensure the running host&apos;s verified %1 and place it first in BootOrder while preserving every other firmware entry.</source>
        <translation>Palauta/varmista, että käynnissä oleva isäntä on todennettu %1 ja aseta se ensin BootSertderiin säilyttäen samalla kaikki muut laiteohjelmistot.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12647"/>
        <source>Restore/ensure the running host&apos;s verified %1 and select it as the default while preserving every other boot entry.</source>
        <translation>Palauta/varmista juoksuisännän todennettu %1 ja valitse se oletuksena säilyttäen samalla kaikki muut käynnistyssyötteet.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12686"/>
        <source>Run the %1 diagnostic for the protected running host before starting this maintenance action.</source>
        <translation>Suorita %1-diagnostiikka suojatulle juoksuisännälle ennen tämän huoltotoimen aloittamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12689"/>
        <source>Required read-only running-host diagnostics are cached for this maintenance action.</source>
        <translation>Vaaditaan vain ajo-ohjaindiagnostiikka.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12698"/>
        <source>Run the selected target diagnostic before starting this repair.</source>
        <translation>Suorita valittu kohdediagnostiikka ennen tämän korjauksen aloittamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12708"/>
        <source>Target state changed after the last diagnostic or repair action. Run the %1 diagnostic for this target before starting this repair.</source>
        <translation>Kohdetila muuttui viimeisen vianmäärityksen tai korjaustoimen jälkeen. Suorita %1-diagnostiikka tälle kohteelle ennen tämän korjauksen aloittamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12709"/>
        <source>Run the %1 diagnostic for this target before starting this repair.</source>
        <translation>Suorita %1-diagnostiikka tälle kohteelle ennen tämän korjauksen aloittamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12712"/>
        <source>Required read-only diagnostic evidence is cached for this repair tool.</source>
        <translation>Vaadittu luku vain diagnostinen näyttö on välimuistissa tätä korjaustyökalua.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12934"/>
        <source>package metadata refreshed — changes were applied</source>
        <translation>package metadatan päivittämiä muutoksia sovellettiin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12935"/>
        <source>package metadata already current — no changes</source>
        <translation>paketti metatiedot jo nykyinen ... ei muutoksia</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12936"/>
        <source>package metadata refresh failed</source>
        <translation>pakettimetadatan päivitys epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12937"/>
        <source>package metadata refresh not checked</source>
        <translation>pakettimetadatan päivitystä ei ole tarkistettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12938"/>
        <source>package metadata refresh did not run</source>
        <translation>paketti metadatan päivitystä ei suoritettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12940"/>
        <source>package configuration completed — changes were applied</source>
        <translation>pakettien konfigurointi valmistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12941"/>
        <source>nothing to configure</source>
        <translation>ei mitään konfiguroitavaa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12942"/>
        <source>package configuration failed</source>
        <translation>pakettien konfigurointi epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12943"/>
        <source>package configuration not checked</source>
        <translation>pakettien kokoonpanoa ei tarkastettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12944"/>
        <source>package configuration did not run</source>
        <translation>pakettien konfigurointia ei suoritettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12946"/>
        <source>broken dependencies repaired — changes were applied</source>
        <translation>rikkinäisiä riippuvuuksia korjattiin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12947"/>
        <source>no broken dependencies</source>
        <translation>ei rikkoutuneita riippuvuuksia</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12948"/>
        <source>broken dependency repair failed</source>
        <translation>rikki riippuvuus korjaus epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12949"/>
        <source>broken dependency repair not checked</source>
        <translation>rikki riippuvuus korjausta ei tarkastettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12950"/>
        <source>broken dependency repair did not run</source>
        <translation>rikki riippuvuus korjaus ei toiminut</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12952"/>
        <source>packages upgraded — changes were applied</source>
        <translation>Paketteja paranneltu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12953"/>
        <source>no packages to upgrade</source>
        <translation>ei päivitettäviä paketteja</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12954"/>
        <source>package upgrade failed</source>
        <translation>pakettipäivitys epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12955"/>
        <source>package upgrade not checked</source>
        <translation>pakettipäivitystä ei valittu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12956"/>
        <source>package upgrade did not run</source>
        <translation>pakettipäivitystä ei suoritettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12958"/>
        <source>DKMS modules rebuilt — changes were applied</source>
        <translation>DKMS-moduuleja rakennettiin uudelleen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12959"/>
        <source>DKMS modules already current — no changes</source>
        <translation>DKMS-moduulit eivät muutu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12960"/>
        <source>DKMS rebuild failed</source>
        <translation>DKMS:n jälleenrakennus epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12961"/>
        <source>DKMS rebuild not checked</source>
        <translation>DKMS-korjausta ei tarkistettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12962"/>
        <source>DKMS rebuild did not run</source>
        <translation>DKMS:n jälleenrakennusta ei toteutettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12964"/>
        <source>display manager repaired — changes were applied</source>
        <translation>Näytönhallinta korjattuja muutoksia sovellettiin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12965"/>
        <source>display manager already correct — no changes</source>
        <translation>Näytä hallinta jo korjata ... ei muutoksia</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12966"/>
        <source>display manager repair failed</source>
        <translation>näytönhallinan korjaus epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12967"/>
        <source>display manager repair not checked</source>
        <translation>näytönhallinan korjausta ei tarkastettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12968"/>
        <source>display manager repair did not run</source>
        <translation>näytönhallinan korjausta ei suoritettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12970"/>
        <source>initramfs rebuilt — images regenerated</source>
        <translation>initramfs regeneroitu kuvat</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12971"/>
        <source>initramfs already current — no changes</source>
        <translation>initramfs jo nykyisellään ei muutoksia</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12972"/>
        <source>initramfs rebuild failed</source>
        <translation>initramfs-korjaus epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12973"/>
        <source>initramfs rebuild not checked</source>
        <translation>initramfs-korjausta ei tarkistettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12974"/>
        <source>initramfs rebuild did not run</source>
        <translation>initramfs:n korjaus ei onnistunut</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12976"/>
        <source>EFI boot path repaired — changes were applied</source>
        <translation>EFI:n käynnistyspolku korjattiin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12977"/>
        <source>EFI boot path already correct — no changes</source>
        <translation>EFI- käynnistyspolku ei muutoksia</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12978"/>
        <source>EFI boot path repair failed</source>
        <translation>EFI- käynnistyspolun korjaus epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12979"/>
        <source>EFI boot path repair not checked</source>
        <translation>EFI-käynnistyspolun korjausta ei tarkistettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12980"/>
        <source>EFI boot path repair did not run</source>
        <translation>EFI- käynnistyspolun korjaus ei toiminut</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12982"/>
        <source>GRUB configuration regenerated — changes were applied</source>
        <translation>GRUB-asetusten regeneroituja muutoksia sovellettiin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12983"/>
        <source>GRUB configuration already current — no changes</source>
        <translation>GRUB: n asetus on jo voimassa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12984"/>
        <source>GRUB regeneration failed</source>
        <translation>GRUB regeneraatio epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12985"/>
        <source>GRUB regeneration not checked</source>
        <translation>GRUB regenerointia ei tarkastettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12986"/>
        <source>GRUB regeneration did not run</source>
        <translation>GRUB:n regenerointia ei suoritettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12988"/>
        <source>extlinux configuration regenerated — changes were applied</source>
        <translation>extlinux-asetusten regeneroituja muutoksia sovellettiin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12989"/>
        <source>extlinux configuration already current — no changes</source>
        <translation>extlinux: n asetus on jo voimassa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12990"/>
        <source>extlinux regeneration failed</source>
        <translation>extlinux regeneraatio epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12991"/>
        <source>extlinux regeneration not checked</source>
        <translation>extlinux regenerointia ei tarkastettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12992"/>
        <source>extlinux regeneration did not run</source>
        <translation>extlinux:n regenerointia ei suoritettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12994"/>
        <source>file system repaired — changes were applied</source>
        <translation>tiedostojärjestelmän korjatut muutokset tehtiin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12995"/>
        <source>no file system errors found — no changes</source>
        <translation>tiedostojärjestelmän virheitä ei löytynyt</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12996"/>
        <source>file system repair failed</source>
        <translation>tiedostojärjestelmän korjaus epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12997"/>
        <source>not all filesystems were checked</source>
        <translation>kaikkia tiedostojärjestelmiä ei tarkastettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12998"/>
        <source>file system repair did not run</source>
        <translation>tiedostojärjestelmän korjausta ei suoritettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13000"/>
        <source>boot stack reconciled — changes were applied</source>
        <translation>boot pino täsmäämään ... muutokset tehtiin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13001"/>
        <source>boot stack already current — no changes</source>
        <translation>boot pino jo nykyinen ... ei muutoksia</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13002"/>
        <source>boot stack repair failed</source>
        <translation>Käynnistyspinon korjaus epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13003"/>
        <source>boot stack not checked</source>
        <translation>boot pinoa ei tarkastettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13004"/>
        <source>boot stack repair did not run</source>
        <translation>boot pino korjaus ei toiminut</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13006"/>
        <source>validation completed — changes were applied</source>
        <translation>Validointia on muutettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13007"/>
        <source>validation is read-only — no changes</source>
        <translation>Validointi on vain luettavaa .</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13008"/>
        <source>validation failed</source>
        <translation>Validointi epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13009"/>
        <source>validation not checked</source>
        <translation>validointia ei tarkastettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13010"/>
        <source>validation did not run</source>
        <translation>validointia ei suoritettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13012"/>
        <source>host boot default updated — changes were applied</source>
        <translation>Käynnistyksen oletus päivitettiin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13013"/>
        <source>host boot default already correct — no changes</source>
        <translation>isäntäkäynnistyksen oletus korjasi jo</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13014"/>
        <source>host boot default update failed</source>
        <translation>Käynnistän oletuspäivitys epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13015"/>
        <source>host boot default not checked</source>
        <translation>isäntäkäynnistyksen oletus ei valittu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13016"/>
        <source>host boot default update did not run</source>
        <translation>Käynnistyksen oletuspäivitys ei toiminut</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13018"/>
        <source>repair successful — changes were applied</source>
        <translation>Korjaavia muutoksia sovellettiin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13019"/>
        <source>no repair needed — no changes were detected</source>
        <translation>korjausta ei tarvittu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13020"/>
        <source>repair failed</source>
        <translation>korjaus epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13021"/>
        <source>not checked — no repair was attempted</source>
        <translation>ei tarkastettua.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13022"/>
        <source>repair did not run</source>
        <translation>korjausta ei suoritettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13348"/>
        <source>Automatic running-host diagnostics regeneration skipped: Host Maintenance is not active.</source>
        <translation>Automaattinen juoksu-host-diagnostiikka regeneraatio ohitettu: isäntä huolto ei ole aktiivinen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13358"/>
        <source>Regenerating target diagnostics</source>
        <translation>Regeneroidaan kohdediagnostiikka</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13359"/>
        <source>Regenerating running-host diagnostics</source>
        <translation>Regeneroiva ajo-ohjaindiagnostiikka</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13364"/>
        <location filename="../src/MainWindow.cpp" line="13860"/>
        <source>Running…</source>
        <translation>Running…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13412"/>
        <location filename="../src/MainWindow.cpp" line="13590"/>
        <source>%1 diagnostic run: %2</source>
        <translation>%1-diagnostiikka: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13413"/>
        <location filename="../src/MainWindow.cpp" line="13590"/>
        <location filename="../src/MainWindow.cpp" line="13908"/>
        <source>Target</source>
        <translation>Kohde</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13413"/>
        <location filename="../src/MainWindow.cpp" line="13590"/>
        <location filename="../src/MainWindow.cpp" line="13908"/>
        <source>Host</source>
        <translation>Palvelin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13453"/>
        <source>A privileged operation is already running; the diagnostic was not started. Try again when it finishes.</source>
        <translation>Etuoikeutettu operaatio on jo käynnissä; diagnoosia ei aloitettu. Yritä uudelleen, kun se loppuu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13464"/>
        <location filename="../src/MainWindow.cpp" line="13824"/>
        <source>Host diagnostic refused: %1</source>
        <translation>Isännän vianmääritys hylätty: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13468"/>
        <location filename="../src/MainWindow.cpp" line="13828"/>
        <source>Host Maintenance required</source>
        <translation>Huolto vaaditaan</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13499"/>
        <source>Running diagnostic: %1</source>
        <translation>Suoritusdiagnostiikka: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13603"/>
        <location filename="../src/MainWindow.cpp" line="13838"/>
        <source>No repair target selected</source>
        <translation>Korjaa kohdetta ei ole valittu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13611"/>
        <source>Read target configuration</source>
        <translation>Lue kohteen asetukset</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13615"/>
        <source>Configuration unavailable</source>
        <translation>Asetukset eivät ole saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13626"/>
        <source>Edit target %1</source>
        <translation>Muokkaa kohdetta %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13629"/>
        <source>Edit this target file through the guarded administrator helper. A successful save invalidates cached diagnostics; rerun diagnostics before repair. Generated files such as /boot/grub/grub.cfg may be replaced by the next bootloader update.</source>
        <translation>Muokkaa tätä kohdetiedostoa vartioidun järjestelmänvalvojan avulla. Onnistunut säästää mitätöi välimuistin diagnostiikka; uudelleen diagnostiikka ennen korjausta. Luodut tiedostot, kuten /boot/grub/grub.cfg, voidaan korvata seuraavalla bootloader-päivityksellä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13641"/>
        <source>Save Target File</source>
        <translation>Tallenna kohdetiedosto</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13648"/>
        <location filename="../src/MainWindow.cpp" line="13690"/>
        <source>Write target configuration</source>
        <translation>Kirjoita kohdeasetukset</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13649"/>
        <source>Write the edited contents to %1? This modifies the repair target and invalidates cached diagnostics.</source>
        <translation>Kirjoittaa muokatun sisällön %1? Tämä muuttaa korjauskohdetta ja mitätöi välimuistin diagnostiikan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13660"/>
        <source>Configuration write refused</source>
        <translation>Asetusten kirjoittaminen hylättiin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13661"/>
        <source>The edited content contains NUL bytes; the guarded write refuses it.</source>
        <translation>Editoitu sisältö sisältää NUL tavuja; vartioitu kirjoitus kieltäytyy siitä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13666"/>
        <source>File too large</source>
        <translation>Tiedosto liian suuri</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13667"/>
        <source>The edited file is larger than 1 MiB; the guarded write refuses it. Edit the file from a console instead.</source>
        <translation>Editoitu tiedosto on suurempi kuin 1 MiB; vartioitu kirjoitus kieltäytyy siitä. Muokkaa tiedostoa konsolista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13672"/>
        <source>Configuration write unavailable</source>
        <translation>Asetuksen kirjoitusta ei ole saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13673"/>
        <source>The edited content could not be written to a private temporary file in the application log directory; the write was not started.</source>
        <translation>Muokkattua sisältöä ei voitu kirjoittaa sovelluslokihakemistossa olevaan väliaikaiseen tiedostoon; kirjoitusta ei aloitettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13701"/>
        <source>Configuration write failed</source>
        <translation>Asetuksen kirjoitus epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13721"/>
        <source>Configuration saved</source>
        <translation>Asetukset tallennettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13805"/>
        <source>The full diagnostic report is already running; results appear when it completes.</source>
        <translation>Koko diagnostinen raportti on jo käynnissä; tulokset näkyvät, kun se valmistuu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13809"/>
        <source>Full diagnostics will run after the current diagnostic finishes.</source>
        <translation>Täydellinen diagnostiikka suoritetaan nykyisten diagnostisten viimeistelyjen jälkeen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13839"/>
        <source>Select a repair target in Systems, or enter Host Maintenance for running-host diagnostics.</source>
        <translation>Valitse järjestelmäkorjauksen kohde tai syötä isännän diagnostiikkaa varten.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13845"/>
        <source>Regenerating diagnostics automatically</source>
        <translation>Regeneroidaan vianmääritys automaattisesti</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13846"/>
        <source>Running all diagnostics</source>
        <translation>Kaikki diagnostiikat</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13910"/>
        <source>Starting all available read-only diagnostics (%1 scope).</source>
        <translation>Aloitetaan kaikki saatavilla olevat luku-vain diagnostiikat (%1-laajuus).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13931"/>
        <source>All available read-only diagnostic summaries generated and cached by diagnostic.</source>
        <translation>Kaikki saatavilla olevat vain diagnostiset yhteenvetoja luotu ja välimuistissa diagnostinen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13932"/>
        <source>Run All diagnostics failed; failed output was not cached as successful diagnostic data.</source>
        <translation>Suorita Kaikki diagnostiikka epäonnistui; epäonnistui ulostulo ei välimuistissa kuin onnistunut diagnostinen tiedot.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14040"/>
        <source>Regenerating diagnostics</source>
        <translation>Regenerointidiagnostiikka</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14050"/>
        <source>Regenerating read-only diagnostics…</source>
        <translation>Regenerointi vain lukudiagnostiikka…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14072"/>
        <source>Read-only diagnostics regenerated automatically.</source>
        <translation>Vain lukudiagnostiikka regeneroitu automaattisesti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14073"/>
        <source>Automatic diagnostics regeneration did not complete; repair actions remain disabled until diagnostics are regenerated.</source>
        <translation>Automaattinen diagnostiikan regenerointi ei valmistunut; korjaustoimet pysyvät poissa käytöstä kunnes diagnostiikka on uusittu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14112"/>
        <source>Diagnostic results copied</source>
        <translation>Diagnostiset tulokset kopioitu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14137"/>
        <source>Unable to save diagnostics</source>
        <translation>Diagnostiikkaa ei voitu tallentaa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14143"/>
        <source>Diagnostics saved to %1</source>
        <translation>%1: lle tallennetut diagnostiikat</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14152"/>
        <source>Save application log</source>
        <translation>Tallenna sovellusloki</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14161"/>
        <source>Unable to save log</source>
        <translation>Lokia ei voitu tallentaa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14169"/>
        <source>Log saved to %1</source>
        <translation>Loki tallennettu %1: lle</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14226"/>
        <location filename="../src/MainWindow.cpp" line="14946"/>
        <source> · %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14254"/>
        <source>%1 — %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14938"/>
        <source>Current session</source>
        <translation>Nykyinen istunto</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14942"/>
        <source>Current session — not started</source>
        <translation>Nykyinen istunto ei alkanut</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14944"/>
        <source> — %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14951"/>
        <source>Entries from this window. A fresh launch starts empty; a session file is created once a running-host or repair-target scope is identified.</source>
        <translation>Tämän ikkunan merkinnät. Uusi lanseeraus alkaa tyhjänä; istuntotiedosto luodaan sen jälkeen, kun juoksu-isäntä tai korjaus-kohde on tunnistettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14996"/>
        <source>Viewing prior session log %1 — generated %2 — read only</source>
        <translation>Katselen aiemman istunnon loki %1 ... luotu %2... Lue vain</translation>
    </message>
    <message>
        <source>Clear the current session log %1? The active file is truncated and cannot be restored. Prior session files are not touched.</source>
        <translation type="vanished">Tyhjennä nykyinen istuntoloki %1? Aktiivinen tiedosto on tynkätty eikä sitä voi palauttaa. Aiempia istuntotiedostoja ei kosketa.</translation>
    </message>
    <message>
        <source>No session log file exists yet because no scope has been identified. Clear the in-memory entries only?</source>
        <translation type="vanished">Istunnon lokitiedostoa ei ole vielä olemassa, koska kohdetta ei ole tunnistettu. Tyhjentäkää vain muistikuvat?</translation>
    </message>
    <message>
        <source>Clear session log</source>
        <translation type="vanished">Tyhjennä istuntoloki</translation>
    </message>
    <message>
        <source>Unable to truncate %1.</source>
        <translation type="vanished">%1:ää ei voitu katkaista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15034"/>
        <source>Note:</source>
        <translation>Huom:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15070"/>
        <location filename="../src/MainWindow.cpp" line="15077"/>
        <source>Delete session log</source>
        <translation>Poista istuntoloki</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15071"/>
        <source>Delete %1 permanently? This cannot be undone.</source>
        <translation>Poista %1 pysyvästi? Tätä ei voi perua.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15078"/>
        <source>Unable to delete %1.</source>
        <translation>%1:ää ei voitu poistaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15890"/>
        <source>(no matching entries for this filter)</source>
        <translation>(ei täsmääviä merkintöjä tälle suodattimelle)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16213"/>
        <source>Boot Bitch must run from a different booted Linux environment than the system being repaired. Use a Linux live USB or another Linux installation on a different physical drive. The running host is protected from ordinary repair-target selection, but it can be explicitly selected through Host Maintenance for guarded native diagnostics and supported maintenance stages. Diagnostics follow the Systems page: the selected repair drive while Host Maintenance is off, or the protected running host while it is active. The first root action requests administrator authorization once for this Boot Bitch window; File → Lock Administrator Session ends that helper session immediately.</source>
        <translation>Boot Bitch:n on oltava erilaisessa Linux-käyttöympäristössä kuin korjattavassa järjestelmässä. Käytä Linux live USB tai toinen Linux asennus eri fyysisellä asemalla. Juokseva isäntä on suojattu tavalliselta korjauskohteen valinnalta, mutta se voidaan nimenomaisesti valita isännän kunnossapidon kautta vartioituun diagnostiikkaan ja tuettuihin huoltovaiheisiin. Diagnostiikka seuraa Systems-sivua: valittu korjausasema, kun isäntähuolto on pois päältä, tai suojattu juoksu isäntä, kun se on aktiivinen. Ensimmäinen juuritoiminta pyytää järjestelmänvalvojan valtuutusta kerran tähän Boot Bitch-ikkunaan; Tiedosto → Lock Administrator istunto päättyy, että auttaja istunto välittömästi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16228"/>
        <source>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;Boot Bitch %1 &lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16229"/>
        <source>&lt;p&gt;&lt;b&gt;Developer:&lt;/b&gt; CaptainMorgan12&lt;/p&gt;&lt;p&gt;A native Qt 6 Linux recovery and boot-repair utility.&lt;/p&gt;</source>
        <translation>&lt;p&gt;&lt;b&gt;Kehikko:&lt;/b&gt; KapteeniMorgan12&lt;/p&gt;&lt;p&gt;Alkuperäinen Qt 6 Linux-hyödyke- ja käynnistyskorjausapuohjelma.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16231"/>
        <source>&lt;p&gt;&lt;b&gt;Guarded repair mode:&lt;/b&gt; read-only diagnostics can inspect either the protected Running Host or an explicitly selected repair drive. Debian/Ubuntu-family repairs can run through a privileged helper after confirmation; Host Maintenance enables the same supported stages natively on the active system after repeating the host identity and boot-mount checks.&lt;/p&gt;</source>
        <translation>&lt;p&gt;&lt;b&gt;Varattu korjaustila: &lt;/b&gt; lukuavain vianmääritys voi tarkastaa joko suojatun ohjauslaitteen tai erikseen valitun korjausaseman. Debian/Ubuntu-perheen korjaukset voidaan suorittaa etuoikeutetun auttajan kautta vahvistuksen jälkeen; isäntähuolto mahdollistaa samat tuetut vaiheet natiivisti aktiivisessa järjestelmässä sen jälkeen, kun isäntähenkilöllisyys ja käynnistysmomentti on toistettu. &lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16232"/>
        <source>&lt;p&gt;Arch-family systems expose backend profiling and guarded package, initramfs, GRUB and conventional EFI repairs when their transaction-specific preflights pass. Fedora/RPM-family systems expose guarded dnf package transactions, dracut initramfs rebuilds, GRUB2 configuration and boot-code repair, and the systemd GDM display path from the same probe evidence; other RPM-family systems remain diagnostics-only until their transaction backend is implemented.&lt;/p&gt;</source>
        <translation>&lt;p&gt;Arch-perhejärjestelmät paljastavat taustaprofiloinnin ja suojatun paketin, initramfs:n, GRUB:n ja tavanomaisten EFI:n korjaukset, kun niiden tapahtumakohtaiset esilennot päättyvät. Fedora/RPM-perhejärjestelmät paljastavat suojatut dnf-pakettitapahtumat, dracut initramfs:n uudelleenrakennukset, GRUB2:n konfiguroinnin ja käynnistyskoodin korjaamisen sekä järjestelmän GDM-näyttöpolun samasta luotainnäytöstä; muut RPM-perhejärjestelmät pysyvät diagnostiikassa vain siihen asti, kunnes niiden tapahtumatausta on toteutettu.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16233"/>
        <source>&lt;p&gt;The first privileged action authorizes one narrow helper session for the current Boot Bitch window while the Qt GUI remains unprivileged. It can be ended at any time from File → Lock Administrator Session.&lt;/p&gt;</source>
        <translation>&lt;p&gt;Ensimmäinen etuoikeutettu toiminto mahdollistaa yhden kapean auttajan istunnon nykyiseen Boot Bitch-ikkunaan, kun Qt-käyttöliittymä on edelleen vailla etuoikeutta. Se voidaan lopettaa milloin tahansa tiedoston → Lock Administrator Session. &lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16234"/>
        <source>&lt;p&gt;LUKS target unlock, verified bidirectional File Copy, read-only host/repair diagnostics, transactional Btrfs snapshot rollback, offline graphical login recovery, distribution-aware EFI/UKI repair and boot-stack reconciliation are enabled through the guarded helper. Snapshot rollback preserves the previous @ and automatically restores it if critical post-switch reconciliation fails.&lt;/p&gt;</source>
        <translation>&lt;p&gt;LUKS-kohteen lukija, todennettu kaksisuuntainen tiedostokopio, luku-/korjausdiagnostiikka, tapahtumakohtainen Btrfs-kuvakaappausrockback, offline-graafinen kirjautumisten palautus, jakelu-aware EFI/UKI-korjaus ja boot-stack täsmäytys ovat käytössä suojatun apurin kautta. Snapshot-käännös säilyttää edellisen @:n ja palauttaa sen automaattisesti, jos kriittinen kytkinten jälkeinen täsmäytys epäonnistuu. &lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17067"/>
        <location filename="../src/MainWindow.cpp" line="18763"/>
        <source>Restore detected graphical login manager</source>
        <translation>Palauta havaittu graafinen kirjautumishallinta</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17069"/>
        <source>Reinstall EFI bootloader</source>
        <translation>Asenna EFI-käynnistin uudelleen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17083"/>
        <location filename="../src/MainWindow.cpp" line="18197"/>
        <source>Reinstall Alpine GRUB EFI loader</source>
        <translation>Asenna Alpine GRUB EFI -kuormaaja uudelleen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17085"/>
        <location filename="../src/MainWindow.cpp" line="18200"/>
        <source>Regenerate Alpine GRUB configuration</source>
        <translation>Uudista Alpine GRUB -kokoonpano</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17095"/>
        <location filename="../src/MainWindow.cpp" line="18170"/>
        <source>Update GRUB2 configuration</source>
        <translation>Päivitä GRUB2-asetus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17097"/>
        <location filename="../src/MainWindow.cpp" line="18173"/>
        <source>Reinstall GRUB2 bootloader</source>
        <translation>Asenna GRUB2-käynnistin uudelleen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17104"/>
        <location filename="../src/MainWindow.cpp" line="18244"/>
        <source>Rebuild initramfs (dracut)</source>
        <translation>Uudelleenrakentaminen initramfs (dracut)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17132"/>
        <source>%1. %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17134"/>
        <source>Execution order %1: %2</source>
        <translation>Teloitusmääräys %1: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17140"/>
        <source>No Full Repair stages selected — use Configure Plan… or Settings.</source>
        <translation>Ei täyskorjausvaiheita valittu . Käytä Configure Plan… tai Asetukset.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17145"/>
        <source>1 stage selected</source>
        <translation>1 vaihe valittu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17146"/>
        <source>%1 stages selected</source>
        <translation>%1-vaiheet valittu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17178"/>
        <source>No repair stages are selected. Use Configure Plan… to choose the stages Full Repair will run.</source>
        <translation>Korjausvaiheita ei ole valittu. Käytä Configure Plan… valita vaiheet Full Repair suoritetaan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17179"/>
        <source>No selected stages are available. %1</source>
        <translation>Valittuja vaiheita ei ole saatavilla. %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17186"/>
        <source> Please regenerate diagnostics before starting another repair.</source>
        <translation>Korjaa diagnostiikka ennen uuden korjauksen aloittamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17189"/>
        <source>Ready: required cached read-only diagnostics are available for the selected stages. Review them in Diagnostics or Logs before confirming.</source>
        <translation>Valmiina: valittuihin vaiheisiin on saatavilla välimuistia vain lukudiagnostiikkaa varten. Tarkista ne Diagnostiikassa tai lokissa ennen vahvistamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17195"/>
        <location filename="../src/MainWindow.cpp" line="18845"/>
        <source>No Full Repair stages are selected.</source>
        <translation>Täydellistä korjausvaihetta ei ole valittu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17201"/>
        <source>Runs the selected stages using the cached read-only diagnostic evidence after privilege confirmation.</source>
        <translation>Suorittaa valitut vaiheet käyttäen välimuistissa vain diagnostisia todisteita etuoikeutensa vahvistuksen jälkeen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17214"/>
        <source>Disabled in Settings</source>
        <translation>Ei käytössä asetuksissa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17221"/>
        <source>Enabled in Settings</source>
        <translation>Käytössä asetuksissa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17222"/>
        <source>Disabled in Settings — enable it to include this stage</source>
        <translation>Ei käytössä Asetuksissa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17226"/>
        <source>Always preflight</source>
        <translation>Aina ennen lentoa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17250"/>
        <source>Manual recovery tool</source>
        <translation>Manuaalinen talteenottotyökalu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17255"/>
        <source>Unavailable: %1</source>
        <translation>Ei saatavilla: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17283"/>
        <source>Select a tool to review its workflow.</source>
        <translation>Valitse työkalu tarkistaa sen työnkulku.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17306"/>
        <source>Check the running host or selected repair system&apos;s mounts, filesystem metadata, boot files, mapper consistency and dependency readiness before any repair action. This is an independent safety preflight rather than an optional Full Repair stage.</source>
        <translation>Tarkista käynnissä isäntä tai valittu korjausjärjestelmän liittimet, tiedostojärjestelmän metatiedot, käynnistystiedostot, mapper johdonmukaisuus ja riippuvuus valmius ennen mitään korjausta. Tämä on riippumaton lentoa edeltävä turvallisuusvaihe eikä valinnainen täyskorjausvaihe.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17309"/>
        <source>Validate</source>
        <translation>Validointi</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17311"/>
        <source>Full Repair: automatic safety preflight</source>
        <translation>Täysi korjaus: automaattinen turvallisuus ennen lentoa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17314"/>
        <source>Complete interrupted dpkg package configuration in the running host or selected repair system. This is the same stage controlled by Settings → Full Repair plan → Complete interrupted package configuration, but it can also be run independently here.</source>
        <translation>Täydellinen keskeytynyt dpkg paketti kokoonpano käynnissä isäntä tai valittu korjausjärjestelmä. Tämä on sama vaihe ohjataan Asetukset → Full Repair Plan → Complete keskeytynyt pakettien kokoonpano, mutta se voidaan myös ajaa itsenäisesti täällä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17316"/>
        <source>Complete Configuration</source>
        <translation>Täydellinen asetukset</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17318"/>
        <location filename="../src/MainWindow.cpp" line="17329"/>
        <location filename="../src/MainWindow.cpp" line="17340"/>
        <location filename="../src/MainWindow.cpp" line="17351"/>
        <location filename="../src/MainWindow.cpp" line="17358"/>
        <location filename="../src/MainWindow.cpp" line="17377"/>
        <location filename="../src/MainWindow.cpp" line="17388"/>
        <location filename="../src/MainWindow.cpp" line="17413"/>
        <location filename="../src/MainWindow.cpp" line="17429"/>
        <location filename="../src/MainWindow.cpp" line="17436"/>
        <location filename="../src/MainWindow.cpp" line="17451"/>
        <source>Full Repair plan: %1</source>
        <translation>Koko korjaussuunnitelma: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17321"/>
        <source>Repair package dependencies in the running host or selected repair system after the mandatory safety preflight. Debian/Ubuntu uses APT; Arch uses one sandbox-preflighted full pacman transaction; Alpine uses apk fix with a simulation first. This maps directly to Settings → Repair broken package dependencies.</source>
        <translation>Korjaa pakettiriippuvuus käynnissä isäntä tai valittu korjausjärjestelmä jälkeen pakollinen turvallisuus ennen lentoa. Debian/Ubuntu käyttää APT:tä; Arch käyttää yhtä hiekkalaatikkoa edeltävää täyttä pacman-tapahtumaa; Alpine käyttää apk-korjausta ensin simulaatiolla. Tämä kartoittaa suoraan Asetukset → Korjaus rikki pakettiriippuvuudet.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17324"/>
        <source> Fedora/RPM systems use rpm verification to find missing or corrupt package files and restore them with one simulated dnf reinstall transaction; dependency problems reported by dnf check are shown but never auto-removed.</source>
        <translation>Fedora/RPM-järjestelmät käyttävät rpm-varmennusta löytääkseen puuttuvat tai korruptoituneet pakettitiedostot ja palauttaakseen ne yhdellä simuloidulla dnf-restall-tapahtumalla; dnf-tarkistuksen ilmoittamat riippuvuusongelmat näytetään, mutta niitä ei koskaan poisteta automaattisesti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17327"/>
        <source>Repair Dependencies</source>
        <translation>Korjaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17332"/>
        <source>Refresh Debian/Ubuntu APT metadata in the running host or selected repair system without upgrading installed packages. Arch and Alpine deliberately refuse a partial metadata-only transaction; their guarded upgrade refreshes the package index itself. This maps directly to Settings → Refresh package metadata.</source>
        <translation>Päivitä Debian/Ubuntu APT -metadata käynnissä olevassa isäntä- tai valitussa korjausjärjestelmässä ilman asennettujen pakettien päivittämistä. Arch ja Alpine kieltäytyvät tarkoituksella osittaisesta metatietotapahtumasta; niiden vartioitu päivitys päivittää itse pakettiindeksin. Tämä kartoittaa suoraan Asetukset → Päivitä paketin metatiedot.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17335"/>
        <source> Fedora refreshes dnf metadata (dnf makecache) as its standalone metadata stage; the cache write is always reported as changed.</source>
        <translation>Fedora virkistää dnf-metadataa (dnf makecache) erillisenä metatietona; välimuistin kirjoitus ilmoitetaan aina muuttuneena.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17338"/>
        <source>Refresh Metadata</source>
        <translation>Päivitä metatiedot</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17343"/>
        <source>Simulate the distribution&apos;s package transaction first, inspect proposed removals, then apply a safe upgrade. Debian/Ubuntu chooses an APT mode; Arch runs one full pacman transaction; Alpine runs one guarded apk upgrade transaction. This maps directly to Settings → Upgrade installed packages.</source>
        <translation>Simuloi jakelun pakettitapahtuma ensin, tarkasta ehdotetut poistot ja tee sitten turvallinen päivitys. Debian/Ubuntu valitsee APT-tilan; Arch ajaa yhden täyden pacman-tapahtuman; Alpine käyttää yhtä suojattua apk-päivitystapahtumaa. Tämä kartat suoraan Asetukset → Päivitys asennettu paketteja.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17346"/>
        <source> Fedora runs one guarded dnf upgrade transaction: the simulated transaction must be removal- and downgrade-free, signature-checked and bounded before the exact same command is applied.</source>
        <translation>Fedora suorittaa yhden suojatun dnf-päivitystapahtuman: simuloidun tapahtuman on oltava poisto- ja laskuvapaa, allekirjoitustarkennettu ja rajoitettu ennen täsmälleen samaa komentoa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17349"/>
        <source>Simulate and Upgrade</source>
        <translation>Simuloi ja paranna</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17354"/>
        <source>Rebuild out-of-tree kernel modules for kernels installed in the running host or selected repair system. The helper refuses this action when DKMS is not installed in the selected system.</source>
        <translation>Rakennetaan uudelleen out-of-puun ytimen moduulit ytimiä asennettu käynnissä isäntä tai valittu korjausjärjestelmä. Auttaja kieltäytyy tästä, kun DKMS:ää ei ole asennettu valittuun järjestelmään.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17356"/>
        <location filename="../src/MainWindow.cpp" line="18759"/>
        <source>Rebuild DKMS</source>
        <translation>Uudelleenrakentaminen DKMS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17361"/>
        <source>Restore the display manager identified from the running host or selected repair system&apos;s boot evidence and configuration (for example SDDM, GDM/GDM3, LightDM, or another systemd manager), set graphical.target as the default, and repair display-manager.service. </source>
        <translation>Palauta näytön hallinta tunnistettu käynnissä isäntä tai valitun korjausjärjestelmän käynnistysnäyttö ja kokoonpano (esim. DSDM, GDM / GDM3, LightDM, tai muu järjestelmällinen manager), asettaa graafisesti. kohde oletuksena ja korjata näytön hallinta. Palvelu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17364"/>
        <source>The detected backend is GDM (Fedora naming); its configuration lives in /etc/gdm/custom.conf. </source>
        <translation>Havaittu taustaosa on GDM (Fedora nameing); sen kokoonpano elää /etc/gdm/custom.conf.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17367"/>
        <source>The detected backend is GDM3; its configuration lives in /etc/gdm3/daemon.conf. </source>
        <translation>Havaittu taustaosa on GDM3; sen kokoonpano elää /etc/gdm3/daemon.conf.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17370"/>
        <location filename="../src/MainWindow.cpp" line="17407"/>
        <source>The detected backend is %1. </source>
        <translation>Havaittu taustaosa on %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17372"/>
        <source>When OpenRC is the detected service manager, the same action restores the detected manager&apos;s default runlevel symlink without starting it. Boot Bitch deliberately does not start a graphical session inside a repair chroot or host helper; use Diagnostics to inspect installed packages, service configuration, and recent boot/journal evidence first when graphical boot fails.</source>
        <translation>Kun OpenRC on havaittu huoltopäällikkö, sama toiminto palauttaa todetun managerin oletusjuoksutason symlinkin käynnistämättä sitä. Boot Bitch ei tarkoituksellisesti aloita graafista istuntoa korjauschroot- tai isäntäavustimen sisällä; käyttää Diagnostics-järjestelmää tarkastaakseen asennetut paketit, palvelun konfiguraatio ja viimeaikaiset boot/journal-todisteet ensin, kun graafinen käynnistys epäonnistuu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17375"/>
        <source>Restore Graphical Login</source>
        <translation>Palauta graafinen kirjautuminen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17380"/>
        <source>Rebuild initramfs images for the running host or selected repair system only after mapper and crypttab consistency checks pass. The helper uses update-initramfs on Debian/Ubuntu, transaction-specific mkinitcpio trials on Arch, and mkinitfs trials on Alpine.</source>
        <translation>Rakenna initramfs-kuvat uudelleen käynnissä olevaan isäntään tai valittuun korjausjärjestelmään vasta kun mapper- ja cryptab-yhteensopivuustarkistus on läpäissyt. Apuri käyttää update-initramfs:ää Debian/Ubuntussa, tapahtumakohtaisia mkinitcpio-kokeita Archilla ja mkinitfs-kokeita Alpinella.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17383"/>
        <source> The detected dracut backend pairs every installed kernel with its /boot/vmlinuz-&lt;kver&gt; and /boot/initramfs-&lt;kver&gt;.img, runs a trial build to a temporary path first, verifies the image with lsinitrd, and backs up each image before the apply so a failed verification restores the previous initramfs.</source>
        <translation>Havaittu dracut backend paria jokaisen asennetun ytimen /boot/vmlinuz-&lt;kver&gt; ja /boot/initramfs-&lt;kver&gt;.img, suorittaa kokeilun rakentaa väliaikaiselle polulle ensin, tarkastaa kuvan lsinitrd, ja varmuuskopioi jokaisen kuvan ennen soveltamista, joten epäonnistui tarkistus palauttaa edellisen initramfs.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17386"/>
        <source>Rebuild Initramfs</source>
        <translation>Initramfsin uudelleenrakentaminen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17392"/>
        <source>Repair the running host or selected repair system&apos;s boot path with the detected backend. The stage uses the guarded installer or vendor builder for the detected layout, restores one verified default-loader entry when a guarded installer only writes files, and preserves every other ESP&apos;s firmware entries and BootOrder. </source>
        <translation>Korjaa käynnissä oleva isäntä tai valitun korjausjärjestelmän käynnistyspolku havaitulla taustaosalla. Vaihe käyttää vartioitua asentajaa tai myyjän rakentajaa havaittuun asetteluun, palauttaa yhden todennetun oletuslataajan merkinnän, kun vartioitu asentaja kirjoittaa vain tiedostoja ja säilyttää kaikki muut ESP:n laiteohjelmistot ja BootSertder.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17395"/>
        <source>The detected vendor UKI layout (for example the TUXEDO create_boot_uki_base.sh / TUX.EFI builder) is rebuilt or re-registered through its official builder. </source>
        <translation>Havaittu myyjä UKI asettelu (esimerkiksi TUXEDO create boot uki base.sh / TUX.EFI rakentaja) on uudelleenrakennettu tai uudelleen rekisteröity sen virallisen rakentajan kautta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17398"/>
        <source>The detected Fedora GRUB2/BLS backend keeps /boot/grub2/grub.cfg and the BLS entries intact, repairs the boot code on a BIOS layout, and leaves the firmware default to the separate Make Default action. </source>
        <translation>Havaittu Fedora GRUB2/BLS backend pitää /boot/grub2/grub.cfg ja BLS merkinnät ehjinä, korjaa käynnistyskoodin BIOS-asettelussa ja jättää firmware-ohjelman oletusarvon erilliselle Make Oletustoiminnolle.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17401"/>
        <source>The detected syslinux/extlinux layout is repaired through its configuration and boot code, never through EFI. </source>
        <translation>Havaittu syslinux/extlinux-asettelu korjataan sen konfiguraatiolla ja käynnistyskoodilla, ei koskaan EFI:n kautta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17404"/>
        <source>The detected GRUB EFI layout uses a guarded grub-install on the validated ESP and restores one verified vendor-loader firmware entry if the guarded installer only writes files. </source>
        <translation>Havaittu GRUB EFI asettelu käyttää vartioitua grub-install:ää validoidussa ESP:ssä ja palauttaa yhden todennetun myyjän lataajan laiteohjelmiston merkinnän, jos suojattu asentaja kirjoittaa vain tiedostoja.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17409"/>
        <source>On Alpine UEFI GRUB systems it backs up the ESP loader files, runs grub-install --target=x86_64-efi --bootloader-id=&lt;detected&gt; --boot-directory=/boot --no-nvram, refreshes the EFI/boot/bootx64.efi fallback copy when the layout had one, and reconciles one firmware entry for the detected loader; a failed install restores the ESP backup. An Alpine EFI-stub-only system is detected and reported, and the stage reconciles captured firmware entries only without synthesising kernel command lines. Afterward, decoded entries on each maintained ESP retain their distribution/vendor label and receive that drive&apos;s model once; an existing model name is not duplicated. Unrelated entries on other disks are never removed.</source>
        <translation>Alpine UEFI GRUB -järjestelmissä se varmuuskopioi ESP-kuormaajatiedostot, toimii grub-install --targetti=x86 64-efi --bootloader-id=&lt;detected&gt; --boot-hakemisto=/boot --nvram, virkistää EFI/boot/bootx64.efi-toistokopion, kun layoutissa oli yksi, ja sovittaa yhteen yhden fiback-tiedoston havaitulle kuormaajalle; epäonnistui asentamisessa palauttaa ESP-varmistuksen. Alpine EFI-stib-ainoa järjestelmä havaitaan ja raportoidaan, ja vaihe sovittaa yhteen kiinni firmware merkinnät vain synteesi ytimen komentolinjoja. Jälkeenpäin kunkin säilytetyn ESP-järjestelmän koodatut merkinnät säilyttävät jakelu-/myyjäetikettinsä ja saavat kyseisen ajon mallin kerran; olemassa olevaa mallin nimeä ei kopioida. Muita levyjä ei koskaan poisteta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17411"/>
        <source>Repair EFI / UKI</source>
        <translation>Korjaus EFI / UKI</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17416"/>
        <source>GRUB2 configuration</source>
        <translation>GRUB2-konfiguraatio</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17417"/>
        <source>Regenerate the running host or selected repair system&apos;s GRUB2 configuration after the mandatory safety preflight. Fedora ships grub2-mkconfig and stores /boot/grub2/grub.cfg with /boot/grub2/grubenv and BLS entries under /boot/loader/entries; the stage regenerates the configuration with --no-grubenv-update, an entry-preserving guard and a rollback when a previous menu entry or BLS entry would be lost. When the read-only boot-code probe finds the MBR or BIOS boot partition broken, the same stage performs a guarded Reinstall GRUB2 bootloader (grub2-install --target=i386-pc --boot-directory=/boot) with MBR and bios_grub backup and rollback; a healthy boot code stays config-only. This never writes firmware NVRAM and does not reinstall EFI loader files; use EFI / UKI bootloader when the firmware loader itself needs repair.</source>
        <translation>Virkistä käyttöisännän tai valitun korjausjärjestelmän GRUB2-konfiguraatiota pakollisen turvallisuuslennon jälkeen. Fedora-alukset grub2-mkconfig ja myymälät /boot/grub2/grub.cfg /boot/grub2/grubenv ja BLS-merkinnät kohtaan /boot/loader/entries; vaihe uudistaa konfiguraation --no-grubenv-update, sisääntulo-säilöövä vartija ja rollback, kun aiempi valikko tai BLS-merkintä menetetään. Kun luku-vain käynnistyskoodin luotain havaitsee MBR:n tai BIOS:n saappaan osion rikki, sama vaihe suorittaa vartioidun GRUB2:n bootloaderin (grub2-install --target=i386-pc --boot-directory=/boot) MBR:n ja bios grubin varmuuskopion ja rollbackin kanssa; terve käynnistyskoodi pysyy vain konfigina. Tämä ei koskaan kirjoita firmware NVRAM eikä asenna EFI-kuormaajatiedostoja uudelleen; käytä EFI / UKI bootloaderia, kun firmware-kuormaaja itse tarvitsee korjausta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17421"/>
        <source>Regenerate GRUB2</source>
        <translation>Regeneroi GRUB2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17424"/>
        <source>Regenerate the running host or selected repair system&apos;s GRUB menu/configuration after the mandatory safety preflight. Debian/Ubuntu uses update-grub; Arch and Alpine use grub-mkconfig with an isolated trial output, an entry-preserving guard and a rollback when a previous menu entry would be lost. This does not reinstall EFI loader files; use EFI / UKI bootloader when the firmware loader itself needs repair.</source>
        <translation>Uudista ajon isäntä tai valitun korjausjärjestelmän GRUB-valikko/konfiguraatio pakollisen lentoa edeltävän turvallisuuslennon jälkeen. Debian/Ubuntu käyttää update-grub:ää; Arch and Alpine use grub-mkconfig:ää, jossa on erillinen koetuloste, sisäänpääsyä säilyttävä suoja ja varauloskäynti, kun edellinen valikkomerkintä katoaa. Tämä ei asenna EFI-kuormaajatiedostoja uudelleen; käytä EFI / UKI-käynnistintä, kun firmware-kuormaaja itse tarvitsee korjausta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17426"/>
        <source>Regenerate GRUB</source>
        <translation>Regeneroi GRUB</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17432"/>
        <source>Regenerate the running host or selected repair system&apos;s extlinux bootloader configuration after the mandatory safety preflight. The detected syslinux/extlinux backend uses update-extlinux with an entry-preserving guard and a boot-artifact backup; existing boot entries are never dropped. This regenerates the configuration only and does not reinstall bootloader files.</source>
        <translation>Uudista käyttöisännän tai valitun korjausjärjestelmän extlinux-käynnistinkokoonpano pakollisen turvalennon jälkeen. Havaittu syslinux/extlinux backend käyttää update-extlinux sisääntulosuojalla ja boot-artifact-varmistuksella; olemassa olevia käynnistysmerkintöjä ei koskaan pudoteta. Tämä uusii vain konfiguraation eikä asenna uudelleen bootloader-tiedostoja.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17434"/>
        <source>Regenerate extlinux</source>
        <translation>Regeneroi extlinux</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17439"/>
        <source>Run a read-only file system check for the selected system&apos;s root, /boot, ESP and /home filesystems, then repair only the devices that report errors. ext2/3/4 uses e2fsck, XFS xfs_repair, Btrfs check/rescue/scrub, FAT fsck.fat, exFAT fsck.exfat, NTFS ntfsfix (limited Linux-side repair; Windows chkdsk is still required), F2FS fsck.f2fs (repair only, no read-only check), JFS jfs_fsck, ReiserFS reiserfsck and ZFS zpool. Offline tools refuse mounted filesystems; btrfs scrub and zpool scrub are online modes, and btrfs check --repair requires a separate backup confirmation because upstream flags it as dangerous. The running host root is never repaired offline, and unsupported filesystems are reported rather than guessed about. This maps directly to Settings → Repair file system errors (read-only check first).</source>
        <translation>Suorita luku-vain tiedostojärjestelmä tarkistaa valitun järjestelmän juuri, /boot, ESP ja /home tiedostojärjestelmät, sitten korjata vain laitteita, jotka ilmoittavat virheitä. ext2/3/4 käyttää e2fsck, XFS xfs repair, Btrfs check/rescue/scrub, FAT fsck.fat, exFAT fsck.exfat, NTFS ntffix (rajoitettu Linux-side korjaus; Windows chkdsksk tarvitaan edelleen), F2FS fsck.f2fs (korjaa vain, ei lueta vain tarkistus), JFS jfs fsck, ReiserFS reiserfsck ja ZFS zpool. Offline työkalut kieltäytyä asennettu tiedostojärjestelmät; btrfs scrub ja zpool scrub ovat online-tiloja, ja btrfs tarkistaa -- korjaus vaatii erillisen varmuuskopion vahvistuksen, koska ylävirtaan lippuja se vaarallinen. Juoksevaa isäntäjuurta ei koskaan korjata offline-tilassa, ja tuettuja tiedostojärjestelmiä raportoidaan pikemminkin kuin arvataan. Tämä kartoittaa suoraan Asetukset → Korjaus tiedostojärjestelmän virheet (lukea vain tarkistaa ensin).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17449"/>
        <source>Check File Systems</source>
        <translation>Tarkista tiedostojärjestelmät</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17454"/>
        <source>Reconcile a repaired or restored root with its boot artifacts: validate mapper/crypttab, rebuild installed-kernel initramfs images, repair the detected bootloader path (the vendor UKI builder when that layout is present, GRUB EFI, extlinux or Fedora BLS), reconcile one canonical default destination per purpose, and regenerate the detected bootloader configuration. </source>
        <translation>Reconcile korjattu tai kunnostettu juuri sen boot artefacts: validoi mapper / cryptab, jälleenrakentaa asennettu-ydin initramfs kuvia, korjata havaittu bootloader polku (toimittaja UKI rakentaja, kun tämä asettelu on olemassa, GRUB EFI, extlinux tai Fedora BLS), sovittaa yksi kanoninen oletuskohde käyttötarkoitusta kohden, ja uudistaa havaittu bootloader kokoonpano.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17457"/>
        <source>On a Fedora BIOS target the same reconciliation validates mapper/crypttab, rebuilds the dracut initramfs images and regenerates the GRUB2 configuration (config-only; the guarded bootloader reinstall stays in the GRUB stage). </source>
        <translation>Fedora BIOS-kohteella sama täsmäytys validoi mapper/cryptab-tiedoston, rakentaa dracut initramfs-kuvat uudelleen ja uudistaa GRUB2-konfigin (vain konfiguraatio; vartioitu bootloader pysyy GRUB-vaiheessa).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17460"/>
        <source>When the EFI / UKI bootloader repair already ran for the same system in this session, this action reuses it: the duplicate bootloader rebuild and GRUB regeneration are skipped and logged, while mapper/crypttab validation and initramfs reconciliation still run. The boot tools are independent: EFI / UKI bootloader repair, GRUB or extlinux configuration, boot-stack reconciliation and Make Default can be run in any order, and a later action re-verifies what an earlier one changed and reports its own result instead of replacing it. This is the focused recovery action for a root/boot mismatch after a partial update or snapshot restore; it does not delete kernels or unrelated boot entries.</source>
        <translation>Kun EFI / UKI bootloader korjaus jo toimi samassa järjestelmässä tässä istunnossa, tämä toiminto käyttää sitä uudelleen: kaksinkertainen bootloader restruction ja GRUB regeneration ohitetaan ja kirjataan, kun mapper / cryptab validointi ja initramfs täsmäytys vielä käynnissä. Käynnistystyökalut ovat riippumattomia: EFI / UKI bootloader korjaus, GRUB tai extlinux kokoonpano, boot-stack täsmäytys ja Make Oletus voidaan suorittaa missä tahansa järjestyksessä, ja myöhemmin toiminta tarkistaa, mitä aiemmin muuttunut ja raportoi oman tuloksen sijasta korvaa sen. Tämä on kohdennettu palautustoimi juuri-/saapasepäsuhdan varalta osittaisen päivityksen tai tilannekatsauksen palauttamisen jälkeen; se ei poista ytimiä tai muita vastaavia alkumerkintöjä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17464"/>
        <source>Reconcile Boot Stack</source>
        <translation>Reconcile Boot Stack</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17466"/>
        <source>Full Repair plan: Manual recovery tool</source>
        <translation>Koko korjaussuunnitelma: Manuaalinen palautustyökalu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17468"/>
        <source>Unknown repair tool</source>
        <translation>Tuntematon korjaustyökalu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17469"/>
        <source>No repair workflow is registered for this item.</source>
        <translation>Tätä kohtaa varten ei ole rekisteröity korjaustyötä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17470"/>
        <source>Unavailable</source>
        <translation>Ei saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17472"/>
        <source>Full Repair plan: unavailable</source>
        <translation>Täydellinen korjaussuunnitelma: ei saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17496"/>
        <source>
Reuse: the bootloader repair from this session will be reused (the duplicate bootloader rebuild and GRUB regeneration are skipped).</source>
        <translation>
Uudelleenkäyttö: bootloader korjaus tästä istunnosta käytetään uudelleen (kaksinkertainen bootloader jälleenrakentaminen ja GRUB regeneration ohitetaan).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17500"/>
        <source>
Unavailable: %1</source>
        <translation>
Ei saatavilla: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17509"/>
        <source>Reuses the bootloader repair from this session: the second UKI rebuild and duplicate GRUB regeneration are skipped when the earlier EFI / UKI stage already rebuilt and verified the detected layout. Mapper/crypttab validation and initramfs reconciliation still run.</source>
        <translation>Käyttää bootloader korjaus tästä istunnosta: toinen UKI jälleenrakentaa ja kopioida GRUB regeneraation ohitetaan, kun aikaisempi EFI / UKI vaiheessa jo rakennettu ja todennettu havaittu ulkoasu. Mapper/cryptab validointi ja initramfs täsmäytys edelleen käynnissä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17510"/>
        <source>Run this guarded repair action using the cached read-only diagnostic evidence. A confirmation is shown first.</source>
        <translation>Suorita tämä vartioitu korjaus käyttäen välimuistin diagnostisia todisteita. Ensin näytetään vahvistus.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18102"/>
        <source>No cached capability evidence for this stage yet. Your saved selection is kept and its availability is re-checked when diagnostics for this scope complete.</source>
        <translation>Tälle näyttämölle ei ole välimuistia. Tallennettu valintasi säilytetään ja sen saatavuus tarkistetaan uudelleen, kun tämän soveltamisalan diagnostiikka on valmis.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18108"/>
        <source> This stage is off by default: select it here to include it in Full Repair.</source>
        <translation>Tämä vaihe on pois päältä oletuksena: valitse se tästä sisällyttää sen Full Repair.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18180"/>
        <source>Repair broken package dependencies (all detected package managers)</source>
        <translation>Korjaa rikkinäiset pakettiriippuvuudet (kaikki havaitut pakettipäälliköt)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18183"/>
        <source>Upgrade installed packages (all detected package managers)</source>
        <translation>Asennettujen pakettien päivittäminen (kaikki havaitut pakettien hallinnoijat)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18190"/>
        <source>Repair Alpine packages (apk fix)</source>
        <translation>Korjaa Alpine paketteja (apk fix)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18193"/>
        <source>Upgrade Alpine packages (apk upgrade)</source>
        <translation>Päivitä Alpine paketteja (apk päivitys)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18209"/>
        <source>Repair Arch package dependencies (full pacman transaction)</source>
        <translation>Korjaa Arch-paketin riippuvuudet (täydellinen pacman-tapahtuma)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18212"/>
        <source>Upgrade Arch packages (full pacman transaction)</source>
        <translation>Päivitä Arch-paketit (täydellinen pacman-tapahtuma)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18220"/>
        <source>Repair Fedora packages (dnf)</source>
        <translation>Korjaa Fedora-paketit (dnf)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18223"/>
        <source>Refresh package metadata (dnf makecache)</source>
        <translation>Päivitä paketin metatiedot (dnf makecache)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18226"/>
        <source>Upgrade Fedora packages (dnf upgrade)</source>
        <translation>Päivitä Fedora paketteja (dnf päivitys)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18231"/>
        <source>Restore detected graphical login manager (OpenRC runlevel)</source>
        <translation>Palauta havaittu graafinen kirjautumisohjelma (OpenRC juoksutaso)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18239"/>
        <source>Restore detected graphical login manager (%1)</source>
        <translation>Palauta havaittu graafinen kirjautumishallinta (%1)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18248"/>
        <source>Rebuild initramfs (mkinitfs)</source>
        <translation>Rakenna initramfs uudelleen (mkinitfs)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18252"/>
        <source>Rebuild initramfs (mkinitcpio)</source>
        <translation>Uudelleenrakentaminen initramfs (mkinitcpio)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18257"/>
        <location filename="../src/MainWindow.cpp" line="18785"/>
        <source>Regenerate extlinux configuration</source>
        <translation>Luo extlinux- asetus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18278"/>
        <source>The running host is protected and cannot be repaired from itself.</source>
        <translation>Juokseva isäntä on suojattu eikä sitä voi korjata itsestään.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18282"/>
        <source>The selected Linux root is still LUKS-encrypted. Unlock it first, refresh devices, then select the mapped filesystem.</source>
        <translation>Valittu Linux root on edelleen LUKS-salattu. Avaa se ensin, virkistä laitteet, valitse sitten kartoitettu tiedostojärjestelmä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18286"/>
        <source>No mountable Linux root filesystem has been identified on the selected target.</source>
        <translation>Valitussa kohteessa ei ole löytynyt asennettavissa olevaa Linuxin juuritiedostojärjestelmää.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18297"/>
        <source>pkexec/Polkit is required to authorize repair operations.</source>
        <translation>Pkexec/Polkit on annettava korjausten käyttöön.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18317"/>
        <source>The running host disk and root component could not be resolved.</source>
        <translation>Juoksevaa isäntälevyä ja juurikomponenttia ei voitu ratkaista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18324"/>
        <source>The selected host identity is no longer marked as the protected running system. Refresh devices before retrying.</source>
        <translation>Valittua isäntätunnusta ei ole enää merkitty suojatuksi ajojärjestelmäksi. Päivitä laitteita ennen kuin yrität uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18334"/>
        <source>pkexec/Polkit is required to authorize host maintenance.</source>
        <translation>Pkexec/Polkit on annettava käyttöön isäntähuolto.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18339"/>
        <source>Running host is ready for explicit guarded maintenance.</source>
        <translation>Juokseva isäntä on valmiina vartioituun huoltoon.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18347"/>
        <source>Select Host Maintenance on the protected running-host card first.</source>
        <translation>Valitse ensin suojatun juoksu-host-kortin isäntähuolto.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18360"/>
        <source>Confirm changes to the running host</source>
        <translation>Vahvista muutokset käynnissä olevaan isäntään</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18361"/>
        <source>Host disk: %1
Linux root: %2

The following guarded changes will run natively on the active system:
• %3

The helper independently re-checks that the selected disk backs /, /boot and /boot/efi, and applies the same package, mapper, EFI/NVRAM and GRUB preservation safeguards used for repair targets.</source>
        <translation>Käyttölevy: %1
Linux-juuri: %2

Seuraavat suojatut muutokset tapahtuvat natiivisti aktiivisessa järjestelmässä:
• %3

Auttaja tarkistaa itsenäisesti, että valitut levyn taustat /, /boot ja /boot/efi, ja soveltaa samaa pakettia, mapper, EFI/NVRAM ja GRUB säilytyssuojat käytetään korjauskohteisiin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18368"/>
        <source>Confirm changes to the selected repair system</source>
        <translation>Vahvista muutokset valittuun korjausjärjestelmään</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18369"/>
        <source>Target disk: %1
Linux root: %2

The following target-system changes will run:
• %3

The running host is independently re-checked and refused by the privileged helper.</source>
        <translation>Kohdelevy: %1
Linux-juuri: %2

Seuraavat tavoitejärjestelmän muutokset tehdään:
• %3

Juokseva isäntä tarkastetaan itsenäisesti uudelleen ja etuoikeutettu avustaja kieltäytyy.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18378"/>
        <source>Run Repair</source>
        <translation>Suorita korjaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18397"/>
        <source>Repair unavailable</source>
        <translation>Korjausta ei saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18637"/>
        <source>%1 failed</source>
        <translation>%1 epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18638"/>
        <source>The repair action failed. Review the captured helper output in Logs for the failing stage and its reason.</source>
        <translation>Korjaaminen epäonnistui. Tarkista kaapattu auttaja lähtö lokeissa vika vaiheessa ja sen syy.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18690"/>
        <source>A file system check and repair is already running. Wait for it to finish before starting another repair; this request was not queued.</source>
        <translation>Tiedostojärjestelmän tarkistus ja korjaus on jo käynnissä. Odota, että se päättyy ennen kuin aloitat toisen korjauksen; tämä pyyntö ei ollut jonossa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18694"/>
        <source>File system repair in progress</source>
        <translation>Tiedostojärjestelmän korjaus käynnissä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18712"/>
        <location filename="../src/MainWindow.cpp" line="18852"/>
        <source>Repair diagnostics required</source>
        <translation>Tarvittava korjausdiagnostiikka</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18716"/>
        <location filename="../src/MainWindow.cpp" line="18810"/>
        <source>Repair tool unavailable</source>
        <translation>Korjaustyökalua ei saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18721"/>
        <source>Validate running host</source>
        <translation>Validoidaan käynnissä oleva palvelin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18726"/>
        <source>Validate repair target</source>
        <translation>Validoidaan korjauskohde</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18740"/>
        <location filename="../src/MainWindow.cpp" line="18859"/>
        <location filename="../src/MainWindow.cpp" line="19245"/>
        <source>running host</source>
        <translation>käynnissä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18741"/>
        <location filename="../src/MainWindow.cpp" line="18860"/>
        <location filename="../src/MainWindow.cpp" line="19246"/>
        <source>selected repair system</source>
        <translation>valittu korjausjärjestelmä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18745"/>
        <source>Complete interrupted package configuration in the %1 with dpkg --configure -a</source>
        <translation>Täydellinen keskeytyspakettien kokoonpano %1: ssä dpkg: llä -- configure -a</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18749"/>
        <source>Repair broken APT package dependencies in the %1</source>
        <translation>Korjaa rikkinäiset APT-pakettiriippuvuudet %1:ssä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18753"/>
        <source>Refresh APT package metadata in the %1</source>
        <translation>Päivitä APT-paketin metatiedot %1:ssä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18757"/>
        <source>Run the distribution-specific transaction preflight, then choose a safe upgrade for the %1</source>
        <translation>Suorita jakelukohtainen tapahtuma ennen lentoa, valitse turvallinen päivitys %1:lle</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18761"/>
        <source>Run DKMS autoinstall in the %1</source>
        <translation>Suorita DKMS autoinstall %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18765"/>
        <source>Set graphical.target as the %1&apos;s default boot target</source>
        <translation>Aseta graafinen kohde %1:n oletuskäynnistyskohteeksi</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18766"/>
        <source>Enable the detected display manager and repair display-manager.service without starting it inside a chroot or host helper</source>
        <translation>Ota huomioon havaittu näytönhallinta ja korjaus näytön hallinta. palvelu aloittamatta sitä sisällä Chroot tai isäntä apuri</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18768"/>
        <source>Rebuild initramfs</source>
        <translation>Rakenna initramfs uudelleen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18770"/>
        <source>Trial-build and then rebuild all initramfs images for the %1</source>
        <translation>Kokeile rakentaa ja sitten rakentaa kaikki initramfs kuvia %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18772"/>
        <source>Repair EFI / UKI bootloader</source>
        <translation>Korjaus EFI / UKI bootloader</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18776"/>
        <location filename="../src/MainWindow.cpp" line="18800"/>
        <source>detected bootloader</source>
        <translation>havaittu bootloader</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18778"/>
        <source>Repair the %1&apos;s %2 boot path through its guarded installer or vendor builder, preserving every other boot entry</source>
        <translation>Korjaa %1 %2-käynnistyspolku vartioidun asentajan tai myyjän rakentajan kautta, säilyttäen kaikki muut saappaan merkinnät</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18779"/>
        <source>Restore one verified default-loader entry when the guarded installer only wrote files</source>
        <translation>Palauta yksi todennettu oletuslataajan merkintä, kun suojattu asentaja vain kirjoitti tiedostoja</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18781"/>
        <source>Regenerate GRUB configuration</source>
        <translation>Luo GRUB- asetus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18783"/>
        <source>Trial-generate and then regenerate the %1&apos;s GRUB configuration</source>
        <translation>Trial generate and then regenerate the %1 GRUB configuration</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18787"/>
        <source>Regenerate the %1&apos;s extlinux bootloader configuration with an entry-preserving guard</source>
        <translation>Uudista %1 extlinux -käynnistyslaitteen konfiguraatio sisääntulosuojalla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18789"/>
        <source>Reconcile boot stack</source>
        <translation>Reconcile boot pino</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18802"/>
        <source>Validate mapper/crypttab against the %1</source>
        <translation>Validoidaan mapper/cryptab %1:ää vastaan</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18803"/>
        <source>Trial-build and rebuild initramfs for installed kernels</source>
        <translation>Trial-rakentaminen ja jälleenrakentaminen initramfs asennettuja ytimiä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18804"/>
        <source>Repair the detected %1 boot path, removing only duplicate default destinations</source>
        <translation>Korjaa havaittu %1- käynnistyspolku, poista vain kaksoisoletuskohteet</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18805"/>
        <source>Regenerate the detected bootloader configuration</source>
        <translation>Uusia havaittujen bootloader-asetusten asetukset</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18807"/>
        <source>Reuse the bootloader repair already completed for this %1: skip the duplicate bootloader rebuild and GRUB regeneration</source>
        <translation>Uudelleenkäytä tätä %1:tä varten valmiiksi saatettua bootloader-korjausta: jätä väliin kaksoiskomponenttikäynnistys ja GRUB:n uudistaminen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18811"/>
        <source>No repair workflow is registered for the selected tool.</source>
        <translation>Valitulle työkalulle ei ole rekisteröity korjaustöitä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18822"/>
        <location filename="../src/MainWindow.cpp" line="18881"/>
        <source>Read-only diagnostics completed; review the full evidence in Logs before confirming repair.</source>
        <translation>Lue vain diagnostiikka valmis; tarkista kaikki todisteet lokit ennen vahvistamista korjaus.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18862"/>
        <source>Run the read-only file system check, then repair the selected devices</source>
        <translation>Suorita vain lukutiedostojärjestelmän tarkistus ja korjaa valitut laitteet</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18864"/>
        <source>Repair broken APT dependencies</source>
        <translation>Korjaa rikkinäiset APT-riippuvuudet</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18865"/>
        <source>Refresh APT package metadata</source>
        <translation>Päivitä APT-paketin metatiedot</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18866"/>
        <source>Simulate APT first, then choose a safe upgrade/full-upgrade/dist-upgrade transaction</source>
        <translation>Simuloi APT ensin, valitse sitten turvallinen päivitys/täydellinen/dist-upgrade tapahtuma</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18868"/>
        <source>Restore the detected display manager and graphical.target without starting the GUI inside chroot</source>
        <translation>Palauta havaittu näytönhallinta ja graafinen. kohde aloittamatta GUI sisällä Chroot</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18869"/>
        <source>Rebuild all initramfs images</source>
        <translation>Rakenna kaikki initramfs-kuvat uudelleen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18870"/>
        <source>Repair the %1 EFI / UKI boot path using its validated ESP</source>
        <translation>Korjaa %1 EFI / UKI boot polku käyttämällä validoitua ESP</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18871"/>
        <source>Regenerate the %1&apos;s GRUB configuration</source>
        <translation>Uudista %1 GRUB -kokoonpano</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18872"/>
        <source>Regenerate the %1&apos;s extlinux configuration</source>
        <translation>Uusi %1 extlinux -kokoonpano</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18952"/>
        <source>The Full Repair plan is running. The read-only file system check pre-stage runs first, then the selected repair stages; each stage&apos;s output streams here as it completes.</source>
        <translation>Koko korjaussuunnitelma on käynnissä. Luku-vain tiedostojärjestelmä tarkistaa pre-stage toimii ensin, sitten valitut korjausvaiheet; kunkin vaiheen lähtövirrat täällä, kun se valmistuu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18970"/>
        <source>Full Repair plan finished with failures. Review the per-stage results (✓/✗/▪) in Logs, then close this window.</source>
        <translation>Täydellinen korjaussuunnitelma päättyi epäonnistumisiin. Tarkista kunkin vaiheen tulokset (✓/✗/▪) in Logs ja sulje tämä ikkuna.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18971"/>
        <source>Full Repair plan finished. Review the per-stage results (✓/✗/▪) in Logs, then close this window.</source>
        <translation>Koko korjaussuunnitelma valmis. Tarkista kunkin vaiheen tulokset (✓/✗/▪) in Logs ja sulje tämä ikkuna.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19170"/>
        <source>File system check unavailable</source>
        <translation>Tiedostojärjestelmän tarkistus ei ole käytettävissä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19196"/>
        <source>Check File Systems (Full Repair pre-stage)</source>
        <translation>Tarkista tiedostojärjestelmät (koko korjausta edeltävä vaihe)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19197"/>
        <source>Check File Systems (manual)</source>
        <translation>Tarkista tiedostojärjestelmät (manual)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19211"/>
        <source>File system check failed</source>
        <translation>Tiedostojärjestelmän tarkistus epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19212"/>
        <source>The read-only file system check failed. Review the captured helper output in Logs for the reason.</source>
        <translation>Vain lukutiedostojen tarkistus epäonnistui. Tarkista kaapattu auttajan tuloste lokeissa syystä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19228"/>
        <source>File system repair unavailable</source>
        <translation>Tiedostojärjestelmän korjausta ei ole saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19241"/>
        <source>Checking and repairing file systems (Full Repair)</source>
        <translation>Tiedostojärjestelmien tarkistaminen ja korjaaminen (Full Repair)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19242"/>
        <source>Checking and repairing file systems</source>
        <translation>Tiedostojärjestelmien tarkastaminen ja korjaaminen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19292"/>
        <source>%1 mounted filesystem(s) were skipped because their check tools are offline-only</source>
        <translation>%1:n asennettu tiedostojärjestelmä [s] ohitettiin, koska niiden tarkistustyökalut ovat offline-vain</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19296"/>
        <source>%1 filesystem(s) are unsupported or have no installed check tool</source>
        <translation>%1-tiedostojärjestelmät [s] eivät ole tuettuja tai niillä ei ole asennettua tarkistustyökalua</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19300"/>
        <source>No file system errors were detected on the %1&apos;s root, /boot, ESP or /home filesystems. No repair was run.</source>
        <translation>Tiedostojärjestelmän virheitä ei havaittu %1-juurta, /boot-, ESP- tai /home-tiedostojärjestelmissä. Korjauksia ei tehty.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19302"/>
        <source>No repairable file system errors were detected on the %1&apos;s root, /boot, ESP or /home filesystems, but %2. No repair was run.</source>
        <translation>%1:n juurissa, /bootissa, ESP:ssä tai /home-tiedostojärjestelmissä ei havaittu korjattavia tiedostojärjestelmän virheitä, mutta %2:ssä. Korjauksia ei tehty.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19334"/>
        <source>The read-only check found issues on %1 mounted filesystem(s) that have no safe repair mode from this environment. Unmount the filesystem and check again, or repair it from a live system that is not using it.</source>
        <translation>Lue vain tarkistus löysi ongelmia %1 asennettu tiedostojärjestelmä [s], joilla ei ole turvallista korjaustilaa tästä ympäristöstä. Avaa tiedostojärjestelmä ja tarkista uudelleen tai korjaa se elävästä järjestelmästä, joka ei käytä sitä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19367"/>
        <source>Run %1 repair on %2</source>
        <translation>Suorita %1 korjaus %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19390"/>
        <source>Dangerous Btrfs repair</source>
        <translation>Vaarallinen Btrfs korjaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19391"/>
        <source>btrfs check --repair is a last-resort tool</source>
        <translation>btrfs-tarkistus --korjaus on viimeinen resort-työkalu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19392"/>
        <source>Upstream Btrfs documentation warns that check --repair can make a damaged filesystem worse and can lose data. Back up everything you can reach first. Prefer a scrub, a rescue, or a fresh backup/restore when either is possible.

Run btrfs check --repair on:
%1</source>
        <translation>Btrfs-dokumentaatio varoittaa, että tarkistus --korjaus voi pahentaa vaurioitunutta tiedostojärjestelmää ja menettää dataa. Taaksepäin kaikki, mihin tavoitat ensin. Suosin pesua, pelastusta tai tuoretta varmuuskopiota/korjausta, kun kumpi tahansa on mahdollista.

Suorita btrfs-tarkistus -- korjaus:
%1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19398"/>
        <source>Run dangerous Btrfs repair</source>
        <translation>Suorita vaarallinen Btrfs korjaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19435"/>
        <source>File system repair (%1)</source>
        <translation>Tiedostojärjestelmän korjaus (%1)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19436"/>
        <source>File system repair (%1, manual)</source>
        <translation>Tiedostojärjestelmän korjaus (%1, manuaalinen)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19474"/>
        <source>File system repair failed</source>
        <translation>Tiedostojärjestelmän korjaus epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19475"/>
        <source>One or more file system repairs failed. Review the captured helper output in Logs for the failing device and its reason.</source>
        <translation>Yksi tai useampi tiedostojärjestelmän korjaus epäonnistui. Tarkista kaapattu auttajan lähtö lokeissa vikalaitteen ja sen syyn vuoksi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19691"/>
        <source>Device columns auto-sized. Drag headers to fine-tune widths.</source>
        <translation>Laitepatruunat automaattisesti. Vedä otsikkoja hienosäätimeen.</translation>
    </message>
</context>
</TS>
