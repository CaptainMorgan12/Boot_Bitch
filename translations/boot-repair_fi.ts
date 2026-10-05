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
        <location filename="../src/MainWindow.cpp" line="831"/>
        <source>Environment validation</source>
        <translation>Ympäristön validointi</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="831"/>
        <source>Summarizes the selected system, protection state, mounted identity and inspection readiness.</source>
        <translation>Yhteenveto valitusta järjestelmästä, suojatilasta, asennetusta henkilöllisyydestä ja tarkastusvalmiudesta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="831"/>
        <source>Summarize the selected system, protection state and inspection readiness.</source>
        <translation>Yhteenveto valitusta järjestelmästä, suojatilasta ja tarkastusvalmiudesta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="832"/>
        <source>Distribution and boot backend profile</source>
        <translation>Jakelu- ja käynnistystukiprofiili</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="832"/>
        <source>Identifies the distribution family, package manager, initramfs generator, bootloader, ESP location and current guarded repair capability.</source>
        <translation>Tunnistaa jakeluperheen, pakettipäällikkö, initramfs generaattori, bootloader, ESP sijainti ja nykyinen suojattu korjauskyky.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="832"/>
        <source>Detect the distribution, bootloader, ESP and current repair capability.</source>
        <translation>Tunnista jakelu, bootloader, ESP ja nykyinen korjauskyky.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="833"/>
        <location filename="../src/MainWindow.cpp" line="15229"/>
        <location filename="../src/MainWindow.cpp" line="15248"/>
        <source>Boot diagnostics</source>
        <translation>Saapasdiagnostiikka</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="833"/>
        <source>Shows boot mounts, /boot and EFI contents plus storage evidence without changing the selected system.</source>
        <translation>Näyttää saappaat, /boot- ja EFI-sisällöt sekä varastointitodisteet muuttamatta valittua järjestelmää.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="833"/>
        <source>Show boot mounts, /boot and EFI contents read-only.</source>
        <translation>Näytä saappaat, /boot ja EFI-sisällöt vain luettuna.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="834"/>
        <source>Boot evidence and selection history</source>
        <translation>Todisteet ja valintahistoria</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="834"/>
        <source>Correlates the detected boot chain, bootloader selection, kernel/initramfs, snapshots, EFI and unlock evidence, including whether one or more LUKS prompts are expected.</source>
        <translation>Korjaa havaittua käynnistysketjua, bootloader-valintaa, ydintä/initramfs:ää, valokuvia, EFI:ää ja avaa todisteita, mukaan lukien yksi tai useampi LUKS-kehotus.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="834"/>
        <source>Correlate the boot chain, loader, kernel/initramfs and unlock evidence.</source>
        <translation>Korjaa käynnistysketju, kuormaaja, ydin/initramfs ja avaa todisteet.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="835"/>
        <source>Kernel / initramfs</source>
        <translation>Ytimen / initramfs</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="835"/>
        <source>Reviews kernel files and verifies matching initramfs images through a read-only inspection.</source>
        <translation>Tarkistaa ydintiedostoja ja todentaa täsmäävät initramfs-kuvat vain lukutarkastuksella.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="835"/>
        <source>Verify kernel files and matching initramfs images read-only.</source>
        <translation>Varmista ytimen tiedostot ja vastaavat initramfs kuvia vain lukea.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="836"/>
        <location filename="../src/MainWindow.cpp" line="6631"/>
        <location filename="../src/MainWindow.cpp" line="15228"/>
        <location filename="../src/MainWindow.cpp" line="15247"/>
        <location filename="../src/MainWindow.cpp" line="20344"/>
        <source>GRUB configuration</source>
        <translation>GRUB-konfiguraatio</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="836"/>
        <source>Reviews GRUB configuration and /etc/default/grub without changing boot files.</source>
        <translation>Arvostelut GRUB asetukset ja /etc/default/grub muuttamatta käynnistystiedostoja.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="836"/>
        <source>Review GRUB configuration and /etc/default/grub read-only.</source>
        <translation>Tarkista GRUB-asetukset ja /etc/default/grub read-only.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="837"/>
        <source>EFI / UKI boot state</source>
        <translation>EFI / UKI- käynnistystila</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="837"/>
        <source>Inspects the selected ESP, vendor or generic UKI images, systemd-boot loader files, embedded kernel/cmdline data and firmware entries with PARTUUID ownership classification.</source>
        <translation>Tarkastaa valitut ESP-, myyjä- tai geneeriset UKI-kuvat, systemd-boot-kuormaajatiedostot, upotetut ytimen/cmdline-tiedot ja firmware-merkinnät, joissa on PAPUUID-omistusluokitus.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="837"/>
        <source>Inspect the ESP, UKI images and firmware entries.</source>
        <translation>Tutustu ESP-, UKI-kuviin ja laiteohjelmistoihin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="838"/>
        <location filename="../src/MainWindow.cpp" line="6627"/>
        <location filename="../src/MainWindow.cpp" line="20273"/>
        <source>Graphical login / display manager</source>
        <translation>Graafinen kirjautuminen / näytönhallinta</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="838"/>
        <source>Reviews graphical.target, the configured display manager (for example SDDM, GDM/GDM3, LightDM, or another systemd manager), installed desktop packages, and recent boot/journal evidence without starting the GUI.</source>
        <translation>Arvostelut graafinen. kohde, konfiguroitu näyttö manager (esim. DSDM, GDM/GDM3, LightDM, tai muu järjestelmällinen manager), asennettu työpöytäpaketit, ja viimeaikaiset boot/journal näyttö käynnistämättä GUI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="838"/>
        <source>Review the configured display manager and login target.</source>
        <translation>Tarkista määritetty näytönhallinta ja kirjautumiskohde.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="839"/>
        <source>Graphics / GPU driver stack</source>
        <translation>Grafiikka / GPU ajuripino</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="839"/>
        <source>Reviews the kernel GPU driver: detected hardware, the loaded and blacklisted modules, installed driver packages, the expected driver, per-kernel DKMS build status, how far this boot reached and actionable journal driver errors.</source>
        <translation>Tarkistaa ytimen GPU-ajuri: Havaittu laitteisto, ladatut ja mustalla listalla moduulit, asennetut ohjainpaketit, odotettu kuljettaja, per-ydin DKMS rakentaa tilan, kuinka pitkälle tämä käynnistys saavuttaa ja toimii päiväkirjan ajuri virheitä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="839"/>
        <source>Review the kernel GPU driver and per-kernel DKMS build status read-only.</source>
        <translation>Tarkista ytimen GPU ajuri ja per-ydin DKMS rakentaa tila vain lukea.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="840"/>
        <source>Boot errors</source>
        <translation>Saapasvirheet</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="840"/>
        <source>Reads recent error-priority entries from the running host or selected repair system&apos;s persistent journal when available.</source>
        <translation>Luee viimeisimmät virheprioriteetti merkinnät käynnissä isäntä tai valitun korjausjärjestelmän jatkuva päiväkirja, jos saatavilla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="840"/>
        <source>Read recent boot error-priority journal entries.</source>
        <translation>Lue viimeaikaiset käynnistyksen virheprioriteetti päiväkirjan merkinnät.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="841"/>
        <source>Disk usage</source>
        <translation>Levyn käyttö</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="841"/>
        <source>Summarizes filesystem capacity/free space for the running host or read-only repair target.</source>
        <translation>Yhdistää tiedostojärjestelmän kapasiteetti / vapaa tila käynnissä isäntä tai luku-vain korjaus kohde.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="841"/>
        <source>Summarize filesystem capacity and free space.</source>
        <translation>Yhteenveto tiedostojärjestelmän kapasiteetti ja vapaa tila.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="842"/>
        <source>File systems</source>
        <translation>Tiedostojärjestelmät</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="842"/>
        <source>Runs the read-only file system check for the running host or selected repair system&apos;s root, /boot, ESP and /home filesystems and reports each device&apos;s check tool and result without changing anything.</source>
        <translation>Suorittaa luku-vain tiedostojärjestelmän tarkistus käynnissä isäntä tai valitun korjausjärjestelmän juuren, /boot, ESP ja /home tiedostojärjestelmät ja raportoi kunkin laitteen tarkistustyökalu ja tulos muuttamatta mitään.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="842"/>
        <source>Run the read-only file system check and report each device&apos;s result.</source>
        <translation>Suorita vain lukutiedostojärjestelmän tarkistus ja raportoi kunkin laitteen tulos.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="843"/>
        <source>fstab</source>
        <translation>fvab</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="843"/>
        <source>Displays the running host or selected repair system&apos;s fstab; repair-system inspection is mounted read-only.</source>
        <translation>Näyttää juoksuisännän tai valitun korjausjärjestelmän fstabin; korjausjärjestelmän tarkastus on asennettu vain luettavaksi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="843"/>
        <source>Display the selected system&apos;s fstab read-only.</source>
        <translation>Näytä valitun järjestelmän fstab-luku vain.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="844"/>
        <source>Btrfs status</source>
        <translation>Btrfs- tila</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="844"/>
        <source>Shows Btrfs filesystem and subvolume information for the running host or selected repair system.</source>
        <translation>Näyttää Btrfs tiedostojärjestelmän ja alivolume tiedot käynnissä isäntä tai valittu korjausjärjestelmä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="844"/>
        <source>Show Btrfs filesystem and subvolume information.</source>
        <translation>Näytä Btrfs-tiedostojärjestelmä ja alivolyymitiedot.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="845"/>
        <source>Mapper status</source>
        <translation>Mapper- tila</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="845"/>
        <source>Shows selected mapper ancestry, device-mapper state and cryptsetup status when available.</source>
        <translation>Näyttää valitut mapper esihistoria, laite-mapper tila ja cryptsetup tila, kun saatavilla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="845"/>
        <source>Show device-mapper ancestry and cryptsetup state.</source>
        <translation>Näytä laite-mapper esihistorian ja kryptauksen tila.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="846"/>
        <source>LUKS / crypttab</source>
        <translation>LUKS / kryptab</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="846"/>
        <source>Shows LUKS/mapped ancestry plus crypttab and fstab mapper references.</source>
        <translation>Näyttää LUKS/mapped esihistorian sekä kryptaab- ja fstab mapper-viittaukset.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="846"/>
        <source>Show LUKS ancestry plus crypttab and fstab mapper references.</source>
        <translation>Näytä LUKS esihistoria sekä kryptaab ja fstab mapper referenssit.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="847"/>
        <location filename="../src/MainWindow.cpp" line="16498"/>
        <source>Full diagnostic report</source>
        <translation>Täydellinen vianmääritysraportti</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="847"/>
        <source>Combines all read-only diagnostics for the selected scope in one privileged inspection session.</source>
        <translation>Yhdistää kaikki luku-vain diagnostiikat valittuun soveltamisalaan yhdessä etuoikeutetussa tarkastusistunnossa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="847"/>
        <source>Combine every read-only diagnostic for the selected scope.</source>
        <translation>Yhdistä jokainen luku-vain diagnostiikka valitun soveltamisalan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1250"/>
        <source>Alpine uses OpenRC, mkinitfs and syslinux/extlinux; boot-stack reconciliation is not enabled (run the initramfs and extlinux stages separately).</source>
        <translation>Alpine käyttää OpenRC:tä, mkinitfs:ää ja syslinux:ää/extlinux:ää; boot-stack-sekvenssien täsmäytys ei ole käytössä (käytä initramfs- ja extlinux-vaiheita erikseen).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1251"/>
        <source>Alpine boot-stack reconciliation is not enabled (run the initramfs and GRUB stages separately).</source>
        <translation>Alpine-keppien täsmäytys ei ole käytössä ( suorita initramfs- ja GRUB-vaiheet erikseen).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1252"/>
        <source>Alpine DKMS preflight found no build tree for installed kernel %1; install the matching headers and retry.</source>
        <translation>Alpine DKMS ennen lentoa ei löytynyt rakennettua puuta asennetulle ytimelle %1; asentaa vastaavat otsikot ja yrittää uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1253"/>
        <source>Alpine EFI-stub host default selection is not implemented; use the Alpine EFI repair stage for entry reconciliation.</source>
        <translation>Alpine EFI-Stub isäntä oletusvalintoja ei ole toteutettu; käytä Alpine EFI korjausvaihe maahantulon täsmäytys.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1254"/>
        <source>Alpine syslinux-EFI boot detected (EFI/syslinux/syslinux.efi); Make Default is not implemented for this backend.</source>
        <translation>Alpine syslinux-EFI boot havaittu (EFI/syslinux/syslinux.efi); Make Oletus ei ole toteutettu tälle taustaosalle.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1255"/>
        <source>Alpine syslinux-EFI boot detected (EFI/syslinux/syslinux.efi); guarded repair is not implemented.</source>
        <translation>Alpine syslinux-EFI boot havaittu (EFI/syslinux/syslinux.efi); vartioitu korjaus ei ole toteutettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1256"/>
        <source>Alpine&apos;s main repository has no proprietary NVIDIA DKMS driver; the GPU driver swap is not available on this target.</source>
        <translation>Alpin päävarastossa ei ole omistusoikeudellista NVIDIA DKMS -ajuria; tässä kohteessa ei ole saatavilla GPU-ajurivaihtoa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1257"/>
        <source>apk simulated no package changes and the package state is byte-identical.</source>
        <translation>apk simuloi ei pakettimuutoksia ja pakettitila on identtinen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1258"/>
        <source>APT package lists are byte-identical and no repository index was fetched.</source>
        <translation>APT-pakettiluettelot ovat identtisiä eikä arkistoindeksiä noudettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1259"/>
        <source>Standalone APK metadata refresh is not available on Alpine; use Upgrade installed packages for one guarded apk transaction.</source>
        <translation>Standalone APK-metatietojen päivitystä ei ole saatavilla Alpinessa; käytä Päivitys asennettuja paketteja yhteen suojattuun apk-tapahtumaan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1260"/>
        <source>Standalone APT metadata refresh is not available on Arch; use Upgrade installed packages for one full pacman transaction.</source>
        <translation>Standalone APT metadatan päivitystä ei ole saatavilla Archissa; käytä Päivitys asennettuja paketteja yhteen pacman-tapahtumaan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1261"/>
        <source>Arch boot-stack reconciliation requires available initramfs, GRUB and EFI repair prerequisites.</source>
        <translation>Arch boot-stack täsmäytys edellyttää saatavilla initramfs, GRUB ja EFI korjaus edellytykset.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1262"/>
        <source>Arch DKMS preflight found no build tree for installed kernel %1; install the matching headers and retry.</source>
        <translation>Arch DKMS ennen lentoa ei löytynyt rakennettua puuta asennetulle ytimelle %1; asenna vastaavat otsikot ja yritä uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1263"/>
        <location filename="../src/MainWindow.cpp" line="1264"/>
        <source>Backends: %1</source>
        <translation>Taustaosat: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1265"/>
        <source>BLS is not enabled; the default is a generated menuentry.</source>
        <translation>BLS ei ole käytössä; oletus on luotu valikko.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1266"/>
        <source>The initramfs backend is booster, which the current repair implementation does not handle.</source>
        <translation>initramfs-taustaosa on tehoste, jota nykyinen korjaus ei käsittele.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1267"/>
        <source>initramfs, EFI/UKI and GRUB artifacts are byte-identical.</source>
        <translation>initramfs, EFI/UKI ja GRUB artefaktit ovat identtisiä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1268"/>
        <source>initramfs and GRUB2 artifacts are byte-identical.</source>
        <translation>initramfs- ja GRUB2-esineet ovat identtisiä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1269"/>
        <source>Requires available initramfs and GRUB repair prerequisites.</source>
        <translation>Edellyttää saatavilla initramfs ja GRUB korjaus edellytykset.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1270"/>
        <source>Embedded cmdline does not reference the live LUKS UUID %1.</source>
        <translation>Upotettu cmdline ei viittaa live LUKS UUID %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1271"/>
        <source>Embedded cmdline does not reference the live root UUID %1.</source>
        <translation>Upotettu cmdline ei viittaa elävää juuria UUID %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1272"/>
        <source>Embedded cmdline does not select the live Btrfs subvolume /%1.</source>
        <translation>Upotettu cmdline ei valitse suoraa Btrfs-subvolyymia /%1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1273"/>
        <source>default.target and display-manager.service were already correct.</source>
        <translation>oletus.kohde ja näyttö-hallinta.palvelu oli jo oikein.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1274"/>
        <source>DKMS preflight found no build tree for installed kernel %1; install the matching kernel-devel packages and retry.</source>
        <translation>DKMS:n esilentoa varten ei löytynyt asennettua %1-ydintä varten rakennettua puuta; asenna vastaavat ytimen devel-paketit ja yritä uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1275"/>
        <source>dnf4 is not supported by the guarded rpm backend.</source>
        <translation>dnf4 ei saa tukea vartioidusta rpm-taustasta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1276"/>
        <source>No missing package files were detected; dnf5 check is reported as evidence.</source>
        <translation>Puuttuvia pakettitiedostoja ei havaittu; dnf5-tarkistus ilmoitetaan todisteena.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1277"/>
        <source>The dnf5 repository metadata cache is byte-identical.</source>
        <translation>Dnf5-tiedoston metatietovälimuisti on tavullinen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1278"/>
        <source>dnf5 simulated no package changes and the rpm database is byte-identical.</source>
        <translation>dnf5 simuloidaan ei pakettimuutoksia ja rpm-tietokanta on identtinen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1279"/>
        <source>dpkg reported no packages pending configuration.</source>
        <translation>dpkg ei raportoinut paketeista, jotka olisivat odottaneet kokoonpanoa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1280"/>
        <source>Alpine uses apk; dpkg configuration is not available on Alpine.</source>
        <translation>Alpine käyttää apk:ää; dpkg:n konfigurointia ei ole saatavilla Alpinella.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1281"/>
        <source>dpkg configuration is not available on Arch; use the Arch package transaction stages instead.</source>
        <translation>dpkg-konfiguraatiota ei ole saatavilla Archissa, vaan käytä Arch-paketin vaiheita.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1282"/>
        <source>Fedora uses rpm/dnf; dpkg configuration is not available.</source>
        <translation>Fedora käyttää rpm/dnf:ää; dpkg:n konfiguraatiota ei ole saatavilla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1283"/>
        <source>EFI boot artifacts and firmware entries are byte-identical.</source>
        <translation>EFI boot-esineet ja firmware-merkinnät ovat yksilöllisiä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1284"/>
        <source>EFI-stub entry repair requires efibootmgr in the recovery host.</source>
        <translation>EFI-sivun sisääntulo korjaus vaatii efibootmgr elvytys isäntä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1285"/>
        <source>EFI-stub boot requires UEFI firmware; the recovery host booted in legacy BIOS mode.</source>
        <translation>EFI-stab boot vaatii UEFI firmware; palautus isäntä käynnistyi vanhassa BIOS-tilassa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1286"/>
        <source>extlinux.conf is byte-identical.</source>
        <translation>extlinux.conf on identtinen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1287"/>
        <source>The detected extlinux configuration %1 is not /boot/extlinux.conf.</source>
        <translation>Havaittu extlinux-konfiguraatio %1 ei ole /boot/extlinux.conf.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1288"/>
        <source>extlinux default label %1 is already selected.</source>
        <translation>extlinux-oletusmerkki %1 on jo valittu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1289"/>
        <source>Extlinux default selection is only available for the running host.</source>
        <translation>Extlinux-oletusvalinta on käytettävissä vain käynnissä olevalle isäntälle.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1290"/>
        <source>extlinux default label set to %1.</source>
        <translation>extlinux-oletusetiketti asetettu %1:ään.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1291"/>
        <source>update-extlinux generated a byte-identical configuration.</source>
        <translation>update-extlinux loi tavu-identtisen kokoonpanon.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1292"/>
        <source>MBR, BIOS boot partition and i386-pc modules are byte-identical.</source>
        <translation>MBR, BIOS boot osio ja i386-PC moduulit ovat byte-identtinen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1293"/>
        <source>grubenv saved_entry already names the running kernel BLS entry %1.</source>
        <translation>grubenv tallennettu entry nimeää jo käynnissä olevan ytimen BLS merkintä %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1294"/>
        <source>grubenv saved_entry set to %1 (all other keys preserved).</source>
        <translation>grubenv tallennettu entry asetettu %1 (kaikki muut avaimet säilytetään).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1295"/>
        <source>The read-only check reported no errors.</source>
        <translation>Vain lukutarkastus ei ilmoittanut virheitä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1296"/>
        <source>The simulated fix-broken transaction proposed no package changes.</source>
        <translation>Simuloitu fix-breaked tapahtuma ehdotti mitään pakettimuutoksia.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1297"/>
        <source>GRUB detected on legacy BIOS; no EFI boot path is available.</source>
        <translation>GRUB havaittu perinteisillä BIOS; EFI boot polku ei ole saatavilla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1298"/>
        <source>grub.cfg and grubenv are byte-identical.</source>
        <translation>grub.cfg ja grubenv ovat tunnistettavia.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1299"/>
        <source>grub.cfg is byte-identical.</source>
        <translation>grub.cfg on identtinen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1300"/>
        <source>GRUB_DEFAULT is not set to saved; grubenv does not select the default entry.</source>
        <translation>GRUB DEFAULT ei ole tallennettu; grubenv ei valitse oletusmerkintää.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1301"/>
        <source>The conflicting module blacklist is already byte-identical.</source>
        <translation>Ristiriitainen moduuli musta lista on jo tavu-identtinen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1302"/>
        <source>No expected GPU driver could be derived from hardware, package recommendations or the OEM flavour.</source>
        <translation>Odotettua GPU-ajuria ei voitu saada laitteistosta, pakettisuosituksista tai OEM-mausta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1303"/>
        <source>Multiple GPU devices were detected with no explicit choice; the GPU driver swap is refused rather than guessing the primary GPU.</source>
        <translation>Useita GPU-laitteita löydettiin ilman nimenomaista vaihtoehtoa; GPU-ohjainvaihdosta ei hyväksytä sen sijaan, että arvattaisiin ensisijainen GPU.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1304"/>
        <source>No conflicting module needs to be blacklisted for the chosen driver stack.</source>
        <translation>Valitulle ajuripinolle ei tarvitse mustalla listalla olla ristiriitaista moduulia.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1305"/>
        <source>No older kernel is pinned to the GPU driver; nothing to remove.</source>
        <translation>GPU-ohjaimeen ei ole kiinnitetty vanhempaa ydintä; mitään poistettavaa ei ole.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1306"/>
        <source>No wrong driver package is installed; nothing to purge.</source>
        <translation>Väärää kuljettajapakettia ei ole asennettu; mitään puhdistavaa ei ole.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1307"/>
        <source>No concrete package set for the expected GPU driver could be derived from the detected package backend and installed-package evidence; the driver swap is refused rather than guessing.</source>
        <translation>Odotetulle GPU-ohjaajalle asetettua konkreettista pakettia ei voitu johtaa havaitusta pakettien taustaosasta ja asennetusta pakettinäytöstä; kuljettajavaihtoa ei hyväksytä eikä arvailla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1308"/>
        <source>The simulated driver purge proposed no package changes.</source>
        <translation>Simuloitu kuljettaja ei ehdottanut pakettien muuttamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1309"/>
        <source>The graphics driver stack is consistent: no driver mismatch and no failed or absent DKMS build were found, so the running driver was left untouched.</source>
        <translation>Graafinen ajuripino on johdonmukainen: kuljettajaa ei ollut ja DKMS-rakenne ei ollut vialla eikä poissa, joten kuljettaja jäi koskematta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1310"/>
        <source>BootOrder, labels and registrations already correct.</source>
        <translation>Saapasjärjestys, etiketit ja rekisteröinnit ovat jo oikein.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1311"/>
        <source>The running host EFI System Partition could not be resolved.</source>
        <translation>Juoksevaa isäntä EFI System Partitionia ei voitu ratkaista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1312"/>
        <source>The target dpkg database or executable is incomplete.</source>
        <translation>Kohde dpkg-tietokanta tai suoritustiedosto on puutteellinen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1313"/>
        <source>Rebuilt initramfs images are byte-identical.</source>
        <translation>Uudelleen rakennetut initramfs-kuvat ovat identtisiä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1314"/>
        <source>Less than 1 GiB of free Btrfs space is available.</source>
        <translation>Alle 1 GiB vapaata Btrfs tilaa on saatavilla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1315"/>
        <source>The grubenv file is missing or not a valid GRUB environment block.</source>
        <translation>Grubenv-tiedosto puuttuu tai ei kelpaa GRUB-ympäristöblokiksi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1316"/>
        <source>The simulated kernel-removal transaction proposed no package changes.</source>
        <translation>Simuloitu ytimen poistotapahtuma ei ehdottanut pakettimuutoksia.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1317"/>
        <source>The current kernel is intact; no removable, incomplete or orphaned kernel components were found.</source>
        <translation>Nykyinen ydin on ehjä; mitään irrotettavaa, epätäydellistä tai orvoksi jäänyttä ydinkomponenttia ei löytynyt.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1318"/>
        <source>legacy BIOS target; no EFI boot path is available.</source>
        <translation>perinteisiä BIOS-kohteita; EFI-polkua ei ole saatavilla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1319"/>
        <source>apk is not installed in the target.</source>
        <translation>apk:ää ei ole asennettu kohteeseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1320"/>
        <source>The target apk installed database is missing.</source>
        <translation>Kohteen apk asennettu tietokanta puuttuu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1321"/>
        <source>The target apk world file is missing.</source>
        <translation>Kohde apk maailman tiedosto puuttuu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1322"/>
        <source>apt-get is not installed in the target.</source>
        <translation>apt-get ei ole asennettu kohteeseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1323"/>
        <source>btrfs-progs is not installed.</source>
        <translation>btrfs-progia ei ole asennettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1324"/>
        <source>DKMS is not installed in the target.</source>
        <translation>DKMS-järjestelmää ei ole asennettu kohteeseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1325"/>
        <source>DKMS is not installed in the Alpine target system.</source>
        <translation>DKMS-järjestelmää ei ole asennettu Alppien kohdejärjestelmään.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1326"/>
        <source>DKMS is not installed in the Arch target system.</source>
        <translation>DKMS:ää ei ole asennettu Arch-kohdejärjestelmään.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1327"/>
        <source>dnf5 is not installed in the target system.</source>
        <translation>dnnf5 ei ole asennettu kohdejärjestelmään.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1328"/>
        <source>dpkg is not installed in the target.</source>
        <translation>dpkg:ää ei ole asennettu kohteeseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1329"/>
        <source>dracut is not installed in the target system.</source>
        <translation>dracut ei ole asennettu kohdejärjestelmään.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1330"/>
        <source>The dracut generator directory is missing from the target.</source>
        <translation>dracut-generaattorin hakemisto puuttuu kohteesta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1331"/>
        <source>efibootmgr is not installed; the running host default EFI entry cannot be changed.</source>
        <translation>efibootmgr ei ole asennettu; käynnissä olevaa isäntää EFI-merkintää ei voi muuttaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1332"/>
        <source>graphical.target is missing from the target.</source>
        <translation>graafinen kohde puuttuu kohteesta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1333"/>
        <source>grub2-editenv is not installed in the target.</source>
        <translation>grub2-editenv ei ole asennettu kohde.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1334"/>
        <source>grub2-install is not installed in the target.</source>
        <translation>grub2-install ei ole asennettu kohteeseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1335"/>
        <source>grub2-mkconfig is not installed in the target.</source>
        <translation>grub2-mkconfig ei ole asennettu kohteeseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1336"/>
        <source>%1 is missing.</source>
        <translation>%1 puuttuu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1337"/>
        <source>Requires GRUB configuration tooling.</source>
        <translation>Vaaditaan GRUB-asetustyökalu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1338"/>
        <source>grub-efi is not installed in the Alpine target.</source>
        <translation>Grub-efiä ei ole asennettu Alppien kohteeseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1339"/>
        <source>The x86_64-efi GRUB module directory is missing from the Alpine target.</source>
        <translation>Alppien kohteesta puuttuu x86 64-efi GRUB -moduulihakemisto.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1340"/>
        <source>Neither grub-mkconfig nor update-grub is installed in the target system.</source>
        <translation>Kohdejärjestelmään ei ole asennettu grub-mkconfig:ää eikä update-grub:ää.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1341"/>
        <source>grub-install missing.</source>
        <translation>grub-install puuttuu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1342"/>
        <source>grub-install is not installed in the Alpine target.</source>
        <translation>grub-install:ää ei ole asennettu Alppien kohteeseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1343"/>
        <source>The grub package is not installed in the Alpine target.</source>
        <translation>Ravintolaa ei ole asennettu Alppien kohteeseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1344"/>
        <source>The GRUB ZFS module (zfs.mod) is not installed in the target.</source>
        <translation>GRUB ZFS -moduulia (zfs.mod) ei ole asennettu kohteeseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1345"/>
        <source>update-initramfs/mkinitramfs are not installed in the target.</source>
        <translation>update-initramfs/mkinitrafeja ei ole asennettu kohteeseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1346"/>
        <source>lsinitrd is not installed in the target system; dracut image verification is unavailable.</source>
        <translation>Lsinitrd ei ole asennettu kohdejärjestelmään; dracut-kuvan tarkistus ei ole käytettävissä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1347"/>
        <source>mkinitcpio is not installed in the target system.</source>
        <translation>mkinitcpio ei ole asennettu kohdejärjestelmään.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1348"/>
        <source>mkinitfs is not installed in the target system.</source>
        <translation>mkinitfs ei ole asennettu kohdejärjestelmään.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1349"/>
        <source>The target has no mkinitfs configuration.</source>
        <translation>Kohteessa ei ole mkinitfs-konfiguraatiota.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1350"/>
        <source>objcopy is unavailable; TUX.EFI embedded sections cannot be verified.</source>
        <translation>objkopiaa ei ole saatavilla; TUX.EFI sulautettuja osia ei voida todentaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1351"/>
        <source>pacman is not installed in the target.</source>
        <translation>pacman:ää ei ole asennettu kohteeseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1352"/>
        <source>The target has no /etc/pacman.conf; refusing a package transaction.</source>
        <translation>Kohde ei ole /etc/pacman.conf; kieltäytyy pakettitapahtuma.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1353"/>
        <source>The target pacman database directory is missing.</source>
        <translation>Kohteen pacman tietokantahakemisto puuttuu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1354"/>
        <source>rpm is not installed in the target system.</source>
        <translation>rpm ei ole asennettu kohdejärjestelmään.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1355"/>
        <source>The target RPM database is missing.</source>
        <translation>Kohdetta ei ole.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1356"/>
        <source>snapper is not installed.</source>
        <translation>Nappulaa ei ole asennettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1357"/>
        <source>The syslinux package is not installed according to the detected package manager.</source>
        <translation>syslinux-pakettia ei ole asennettu havaitun paketinhallinnan mukaisesti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1358"/>
        <source>TUXEDO UKI builder create_boot_uki_base.sh is not installed in the target.</source>
        <translation>TUXEDO UKI builder create boot uki base.sh ei ole asennettu kohteeseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1359"/>
        <source>update-extlinux is not installed in the target.</source>
        <translation>update-extlinux ei ole asennettu kohde.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1360"/>
        <source>The running host has no /etc/update-extlinux.conf.</source>
        <translation>Juoksevalla isäntällä ei ole /etc/update-extlinux.conf.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1361"/>
        <source>update-extlinux is not installed in the running host.</source>
        <translation>update-extlinux ei ole asennettu käynnissä isäntä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1362"/>
        <source>The zfs-initramfs hook is not installed in the target (no /usr/share/initramfs-tools/hooks/zfs).</source>
        <translation>Zfs-initramf-koukkua ei ole asennettu kohteeseen (ei /usr/share/initramfs-tools/hooks/zfs).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1363"/>
        <source>Multiple extlinux entries reference the running kernel %1 (%2).</source>
        <translation>Useissa extlinux-kirjoituksissa viitataan juoksevaan ytimeen %1 (%2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1364"/>
        <source>The running @ root contains nested subvolumes that this release does not migrate: %1.</source>
        <translation>Juokseva @ root sisältää pesittyjä alivolumeja, että tämä julkaisu ei siirry: %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1365"/>
        <source>The proprietary NVIDIA driver needs network access to fetch the package set; this offline target has no usable APT sources.</source>
        <translation>Omistettu NVIDIA-ajuri tarvitsee verkkoyhteyden noutaakseen asetetun paketin; tällä offline-kohteella ei ole käytettävissä olevia APT-lähteitä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1366"/>
        <source>No EFI boot path was detected for the Alpine target (bootloader backend %1).</source>
        <translation>Alpine-kohteelle (Bootloader backend %1) ei havaittu EFI- käynnistyspolkua.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1367"/>
        <source>No installed Alpine kernels were found under target /boot.</source>
        <translation>Alppien siemeniä ei löytynyt kohteesta / saappaasta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1368"/>
        <source>No /boot (and no resolved EFI System Partition) is visible for the selected scope.</source>
        <translation>Valitulle ulottuvuudelle ei ole näkyvissä /boot (eikä ratkaistua EFI System Partitionia).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1369"/>
        <source>No installed versioned kernel packages were found; the newest kernel cannot be identified.</source>
        <translation>Asennettuja versioituja ydinpaketteja ei löytynyt; uusinta ydintä ei voida tunnistaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1370"/>
        <source>No installed kernel is missing its matching headers.</source>
        <translation>Asennettua ydintä ei ole jäljellä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1371"/>
        <source>No orphaned kernel build/support packages (linux-kbuild or other versioned helpers whose kernel is gone) were found.</source>
        <translation>Orvon ytimen rakenne-/tukipaketteja (linux-kbuild tai muita versioituja apureita, joiden ydin on poissa) ei löytynyt.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1372"/>
        <source>No orphaned kernel headers (headers without a matching kernel) were found.</source>
        <translation>Orpoja ytimen otsikoita (otsikot ilman vastaavaa ydintä) ei löytynyt.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1373"/>
        <source>No orphaned kernel components (module trees or boot files without an owning package) were found.</source>
        <translation>Orvon ytimen komponentteja (moduulipuut tai saapastiedostot ilman omistava paketti) ei löytynyt.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1374"/>
        <source>No incomplete kernel packages (an installed image without its on-disk files) were found.</source>
        <translation>Epätäydellinen ydinpaketti (asennettu kuva ilman levytiedostoja) ei löytynyt.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1375"/>
        <source>No removable kernels are installed (only the running and newest kernels are present).</source>
        <translation>Mitään irrotettavissa olevia ytimiä ei ole asennettu (vain juoksevat ja uusimmat ytimet ovat läsnä).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1376"/>
        <source>No newer repository kernel is available, or the repository is unreachable.</source>
        <translation>Uudempaa arkiston ydintä ei ole saatavilla, tai arkistoa ei voi tavoittaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1377"/>
        <source>The target has no configured apk repositories.</source>
        <translation>Kohteessa ei ole konfiguroituja apk-arkistoja.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1378"/>
        <source>The target has no configured APT sources to refresh.</source>
        <translation>Kohteella ei ole konfiguroituja APT-lähteitä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1379"/>
        <source>No EFI System Partition was identified for the selected Arch target.</source>
        <translation>Valitulle Arch-kohteelle ei tunnistettu EFI System Partitionia.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1380"/>
        <source>No non-rescue BLS entries are installed.</source>
        <translation>Muita kuin pelastus-BLS-tiedostoja ei ole asennettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1381"/>
        <source>No canonical EFI vendor loader was found on the running host ESP and grub-install is not available.</source>
        <translation>Suoritusisännästä ESP ei löytynyt kanonista EFI-toimittajakuormaajaa eikä grub-install:ää ole saatavilla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1382"/>
        <source>No supported display manager was detected in the target.</source>
        <translation>Kohdeessa ei havaittu tuettua näytönhallintaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1383"/>
        <source>No supported display manager unit is installed in the target.</source>
        <translation>Kohdeeseen ei ole asennettu tuettua näytönhallintayksikköä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1384"/>
        <source>The target has no enabled dnf repositories.</source>
        <translation>Kohde ei ole käytössä dnf-arkistoissa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1385"/>
        <source>The target has no enabled dnf repositories to refresh.</source>
        <translation>Kohde ei ole mahdollistanut dnf-arkistojen päivittämistä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1386"/>
        <source>No installed dracut kernels were found under target /boot.</source>
        <translation>Kohteen / saappaan alle ei löytynyt asennettuja dracut-ytimiä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1387"/>
        <source>No EFI-stub initramfs image (initramfs-*) is present at the EFI System Partition root.</source>
        <translation>EFI System Partition - juurilla ei ole EFI-stab initramfs-kuvaa (initramfs-*).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1388"/>
        <source>No EFI-stub kernel image (vmlinuz-*) is present at the EFI System Partition root.</source>
        <translation>EFI System Partition - juurilla ei ole EFI-ydinkuvaa (vmlinuz-*).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1389"/>
        <source>No EFI System Partition candidate on the selected disk.</source>
        <translation>Valitulla levyllä ei ole EFI System Partition -ehdokasta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1390"/>
        <source>No EFI System Partition is present or derivable on the selected disk.</source>
        <translation>EFI System Partition ei ole läsnä tai johdettu valitulla levyllä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1391"/>
        <source>No extlinux/syslinux configuration was detected in the target.</source>
        <translation>Kohteessa ei havaittu extlinux/syslinux-konfiguraatiota.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1392"/>
        <source>No extlinux/syslinux configuration was detected in the running host.</source>
        <translation>Suorituspalvelimessa ei havaittu extlinux/syslinux-konfiguraatiota.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1393"/>
        <source>The detected GRUB layout is not a grub2 layout.</source>
        <translation>Havaittu GRUB asettelu ei ole grub2 asettelu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1394"/>
        <source>No supported file system check tool is installed in the recovery environment.</source>
        <translation>Hyödyntämisympäristöön ei ole asennettu tuettua tiedostojärjestelmän tarkistustyökalua.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1395"/>
        <source>No class-03 GPU device and no target journal/module evidence were found; there is no GPU driver stack to repair.</source>
        <translation>Luokka-03 GPU-laitetta ja kohdelehteä/moduulia ei löytynyt; korjattavia GPU-ohjaimia ei ole.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1396"/>
        <source>The GPU driver stack looks healthy; no mismatch was proven, so the driver is not touched.</source>
        <translation>GPU ajuri pino näyttää terveeltä; mitään eroa ei todistettu, joten kuljettaja ei kosketa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1397"/>
        <source>No supported initramfs backend (mkinitfs, mkinitcpio, dracut or initramfs-tools) was detected.</source>
        <translation>Tuettua initramfs-taustalaitetta (mkinitfs, mkinitcpio, dracut tai initramfs-tools) ei havaittu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1398"/>
        <source>Unable to determine the live root filesystem UUID.</source>
        <translation>Ei voitu määrittää elävää juuritiedostojärjestelmää UUID.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1399"/>
        <source>No installed kernel module directories were found for mkinitcpio.</source>
        <translation>mkinitcpio:lle ei löytynyt asennettuja ydinmoduulihakemistoja.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1400"/>
        <source>No supported OpenRC display manager service is installed in the target.</source>
        <translation>Kohdeeseen ei ole asennettu tuettua OpenRC-näytönhallintapalvelua.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1401"/>
        <source>No guarded package-manager backend that can both answer installed-state and install packages was detected (detected: %1).</source>
        <translation>Yhtään suojattua pakettihallintataustaosaa, joka voi sekä vastata asennettuun tilaan että asentaa paketteja, ei havaittu ( Havaittu: %1).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1402"/>
        <source>No guarded package-manager backend was detected (detected: %1).</source>
        <translation>Suojattua pakettihallintataustaa ei havaittu ( Havaittu: %1).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1403"/>
        <source>No detected package manager can verify the syslinux package.</source>
        <translation>Mikään havaittu pakettien hallinta ei voi varmistaa syslinux-pakettia.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1404"/>
        <source>No supported reboot mechanism was found on the running host.</source>
        <translation>Käynnistävästä isäntästä ei löytynyt tuettua uudelleenkäynnistysmekanismia.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1405"/>
        <source>The selected scope&apos;s root, /boot, ESP and /home filesystems could not be resolved.</source>
        <translation>Valitun soveltamisalan juuria, /boot-, ESP- ja /home-tiedostojärjestelmiä ei voitu ratkaista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1406"/>
        <source>No supported service manager (systemd or OpenRC) was detected in the target.</source>
        <translation>Kohdeessa ei havaittu tuettua palvelupäällikköä (järjestelmällinen tai OpenRC).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1407"/>
        <source>No Snapper root configuration manages / with FSTYPE=btrfs.</source>
        <translation>Ei Snapper root asetukset hallita / FSTYPE=btrfs.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1408"/>
        <source>The selected scope has no supported file system type for a read-only check.</source>
        <translation>Valitulla soveltamisalalla ei ole tuettua tiedostojärjestelmän tyyppiä vain lukutarkistusta varten.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1409"/>
        <source>The running host root filesystem is %1, not Btrfs.</source>
        <translation>Juokseva isäntä juuritiedostojärjestelmä on %1, ei Btrfs.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1410"/>
        <source>The detected bootloader is %1; GRUB is not the selected bootloader.</source>
        <translation>Havaittu bootloader on %1; GRUB ei ole valittu bootloader.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1411"/>
        <source>The running root subvolume is %1, not the top-level @.</source>
        <translation>Juokseva juuriosamäärä on %1, ei huipputason @.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1412"/>
        <source>Unable to derive a running host UKI candidate.</source>
        <translation>Suoritettavaa UKI- ehdokasta ei voitu saada.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1413"/>
        <source>grubenv is already byte-identical; no nomodeset one-boot entry was written.</source>
        <translation>grubenv on jo identtinen; ei ole kirjoitettu nomodeset yhden saappaan merkintä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1414"/>
        <source>The nomodeset one-boot fallback requires a GRUB layout with a writable grubenv.</source>
        <translation>Nomodeset one-boot varalla vaatii GRUB asettelu kirjoitettava grubenv.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1415"/>
        <source>The nomodeset one-boot fallback is refused for UKI/EFI-stub layouts that cannot produce a one-shot artifact.</source>
        <translation>Nomodeset one-boot varauloskäyntiä ei hyväksytä UKI/EFI-pubin asetteluille, jotka eivät pysty tuottamaan yhden kuvan esinettä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1416"/>
        <source>The OpenRC default runlevel already enabled %1.</source>
        <translation>OpenRC- oletusjuoksutaso on jo käytössä %1:ssä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1417"/>
        <source>A package manager or package-manager lock is active.</source>
        <translation>Paketinhallinta tai pakettihallinta lukko on aktiivinen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1418"/>
        <source>The pacman transaction reported no packages to install, upgrade or remove.</source>
        <translation>pacman-tapahtumassa ei ilmoitettu mitään asennus-, päivitys- tai poistopaketteja.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1419"/>
        <source>The running host pins subvolid= in fstab or the kernel command line.</source>
        <translation>Juokseva isäntä nastat subvolid = fstab tai ytimen komentorivi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1420"/>
        <source>The running host root filesystem is read-only.</source>
        <translation>Juokseva isäntä juuritiedostojärjestelmä on vain luettava.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1421"/>
        <source>The rpm DKMS header correction (kernel-devel/akmods) is not implemented.</source>
        <translation>DKKMS-otsikkokorjausta (kernel-devel/akmods) ei toteuteta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1422"/>
        <source>The running kernel %1 has no installed BLS entry.</source>
        <translation>Juoksevassa ytimessä %1 ei ole asennettua BLS-merkintää.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1423"/>
        <source>The running kernel %1 has no entry in %2.</source>
        <translation>Juoksevan ytimen %1 ei ole merkintä %2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1424"/>
        <source>The running kernel release could not be determined (uname -r is empty).</source>
        <translation>Juoksevaa ytimen julkaisua ei voitu määrittää (uname -r on tyhjä).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1425"/>
        <source>Secure Boot requires a signed loader.</source>
        <translation>Secure Boot vaatii allekirjoitetun kuormaajan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1426"/>
        <source>The running host has a separate /boot filesystem outside the root snapshot.</source>
        <translation>Juoksevalla isäntällä on erillinen /boot-tiedostojärjestelmä juurikuvan ulkopuolella.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1427"/>
        <source>Another snapper command is running.</source>
        <translation>Toinen snapper-komento on käynnissä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1428"/>
        <source>syslinux/extlinux (BIOS) boot detected; no EFI boot path is available.</source>
        <translation>syslinux/extlinux (BIOS) -käynnistys havaittu; EFI- käynnistyspolkua ei ole saatavilla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1429"/>
        <source>A sysvinit display-manager script was detected; offline repair is not implemented for sysvinit.</source>
        <translation>Sysvinit näyttö-manager script havaittiin; offline korjausta ei toteuteta sysvinit.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1430"/>
        <source>A Timeshift btrfs snapshot inventory is present; Snapper @ rollback is not supported.</source>
        <translation>Timeshift btrfs -kuvaston inventaario on läsnä; Snapper @ rollback ei ole tuettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1431"/>
        <source>UEFI variables are not writable; the running host default EFI entry cannot be changed.</source>
        <translation>UEFI-muuttujat eivät ole kirjoitettavia; käynnissä olevaa isäntää oletus EFI ei voi muuttaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1432"/>
        <source>%1 is missing or empty.</source>
        <translation>%1 puuttuu tai on tyhjä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1433"/>
        <source>Embedded kernel %1 is not installed under /boot.</source>
        <translation>Upotettu ydin %1 ei ole asennettu /boot.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1434"/>
        <source>TUX.EFI does not embed a readable .cmdline section.</source>
        <translation>TUX.EFI ei upota luettavaa .cmdline-osaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1435"/>
        <source>TUX.EFI does not embed a kernel version (.uname).</source>
        <translation>TUX.EFI ei upota ytimen versiota (.Uname).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1436"/>
        <source>TUX.EFI is not a readable PE/COFF image (objcopy could not read .uname).</source>
        <translation>TUX.EFI ei ole luettavissa oleva PE/COFF-kuva (objcopy ei voi lukea .uname).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1437"/>
        <source>Unable to create a temporary file for UKI section inspection.</source>
        <translation>Väliaikaista tiedostoa UKI-osiotarkastukseen ei voitu luoda.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1438"/>
        <source>Backend %1 has no guarded implementation for stage %2.</source>
        <translation>Backend %1 ei ole vartioitu toteutus vaiheessa %2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1439"/>
        <source>The running kernel release could not be determined.</source>
        <translation>Juoksevaa ytimen vapautumista ei voitu määrittää.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1440"/>
        <source>The running host ZFS root pool is not resolvable (zpool is not installed or the pool is not imported).</source>
        <translation>Juokseva isäntä ZFS root allas ei ole resolved (zpool ei ole asennettu tai allasta ei tuoda).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1441"/>
        <source>The extlinux label %1 cannot be written to /etc/update-extlinux.conf safely.</source>
        <translation>extlinux-etikettiä %1 ei voi kirjoittaa /etc/update-extlinux.conf:lle turvallisesti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1442"/>
        <source>The simulated upgrade transaction proposed no package changes.</source>
        <translation>Simuloitu päivitystapahtuma ei ehdottanut pakettimuutoksia.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1443"/>
        <source>Validation is read-only.</source>
        <translation>Validointi on vain luettavaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1476"/>
        <source>Backed up Alpine EFI loader files to %1 (firmware fallback present before repair: %2).</source>
        <translation>Varmistettu Alpine EFI kuormaaja tiedostoja %1 (firmware varalla läsnä ennen korjausta: %2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1477"/>
        <source>PASS: refreshed the firmware fallback loader EFI/boot/bootx64.efi from %1.</source>
        <translation>PASS: Päivitetty firmware varakuormaaja EFI/boot/bootx64.efi %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1478"/>
        <source>PASS: Alpine EFI loader files verified under %1/EFI/%2</source>
        <translation>PASS: Alpine EFI kuormaaja tiedostot todennettu %1/EFI/%2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1479"/>
        <source>Restored the Alpine EFI loader files from the session backup %1.</source>
        <translation>Palautti Alpine EFI lataaja tiedostot istunnon varmuuskopio %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1480"/>
        <source>Detected %1 EFI-stub firmware entry(ies) on the selected ESP (%2); reconciling firmware state without file synthesis.</source>
        <translation>Havaittiin %1 EFI-stib firmware -merkintä [t] valitussa ESP:ssä (%2); firmware-tilan yhteensovittaminen ilman tiedostosynteesiä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1481"/>
        <source>PASS: Alpine EFI-stub firmware entries verified; no ESP files were changed.</source>
        <translation>PASS: Alpine EFI-stib firmware-tiedostot todennettu; ESP-tiedostoja ei muutettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1482"/>
        <source>Captured complete firmware entry state before Alpine GRUB EFI install: %1</source>
        <translation>Captured täydellinen firmware sisääntulotila ennen Alpine GRUB EFI asentaa: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1483"/>
        <source>Writable UEFI efivars/efibootmgr are unavailable; Alpine GRUB EFI repair will update loader files and the fallback copy only (read-only firmware-variable mode).</source>
        <translation>Kirjoitettava UEFI efivars / efibootmgr eivät ole saatavilla; Alpine GRUB EFI korjaus päivittää lataajan tiedostoja ja varakopioida vain (lukea vain firmware-variable mode).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1484"/>
        <source>SIMULATE/PREFLIGHT: Alpine GRUB EFI install target=%1 fs=%2 id=%3 mode=--no-nvram (helper-managed firmware entries)</source>
        <translation>SIMULATE/PREFLIGHT: Alpine GRUB EFI asenna kohde=%1 fs=%2 id=%3 mode=--no-nvram (auttajaohjatut laiteohjelmistot)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1485"/>
        <source>PASS: Alpine initramfs rebuilt and verified for %1</source>
        <translation>PASS: Alpine initramfs uudelleenrakennettu ja tarkistettu %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1486"/>
        <source>Alpine missing-file repair: reinstalling %1 package(s) with missing files: %2</source>
        <translation>Alpine puuttuvat-tiedosto korjaus: asentaminen %1 paketti(s) puuttuvat tiedostot: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1487"/>
        <source>Alpine missing-file detection: no missing package files; running the dependency-only apk fix transaction.</source>
        <translation>Alppien puuttuvan tiedoston havaitseminen: ei puuttuvia pakettitiedostoja; pelkästään riippuvuuden apk-korjaustapahtuman suorittaminen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1488"/>
        <source>Target %1 is already mounted at %2 (pre-existing source=%3 options=%4 id=%5); leaving it untouched.</source>
        <translation>Kohde %1 on jo asennettu %2 (olemassa oleva lähde=%3 vaihtoehtoja=%4 id=%5); jättäen sen koskemattomana.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1489"/>
        <source>Alpine apk preflight: %1</source>
        <translation>Alpine apk ennen lentoa: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1490"/>
        <source>REFUSED: apk simulation proposes %1 package changes (safety limit: %2).</source>
        <translation>EPÄÄMINEN: apk-simulaatio ehdottaa %1-paketin muutoksia (turvallisuusraja: %2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1491"/>
        <source>REFUSED: apk simulation reported unresolved or conflicting packages.</source>
        <translation>EPÄÄMINEN: apk-simulaatio raportoi ratkaisemattomista tai ristiriitaisista paketeista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1492"/>
        <source>REFUSED: apk simulation proposes a package downgrade.</source>
        <translation>EPÄÄMINEN: apk-simulaatiossa ehdotetaan paketin vähentämistä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1493"/>
        <source>REFUSED: apk simulation reported a locked package database.</source>
        <translation>EPÄÄMINEN: apk-simulaatio ilmoitti lukitun pakettitietokannan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1494"/>
        <source>REFUSED: apk simulation reported incomplete repository metadata.</source>
        <translation>EPÄÄMINEN: apk-simulaation mukaan tietovaraston metatiedot ovat puutteellisia.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1495"/>
        <source>REFUSED: apk simulation would remove &apos;%1&apos; without replacing it in the same transaction.</source>
        <translation>EPÄÄMINEN: apk-simulaatio poistaisi &quot;%1&quot; korvaamatta sitä samassa liiketoimessa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1496"/>
        <source>REFUSED: apk simulation reported an untrusted package signature.</source>
        <translation>EPÄÄMINEN: apk-simulaatio ilmoitti luottamattomasta paketin allekirjoituksesta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1497"/>
        <source>REFUSED: apk simulation reported an error Boot Bitch does not recognize as safe.</source>
        <translation>EPÄÄMINEN: apk simulointi ilmoitti virhe Boot Bitch ei tunnista turvallista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1498"/>
        <source>apk simulation replaces package &apos;%1&apos; in the same transaction; removal accepted.</source>
        <translation>apk-simulaatio korvaa paketin &apos;%1&apos; samassa tapahtumassa; poisto hyväksytty.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1499"/>
        <source>KNOWN ISSUE: APT correction is blocked by broken dependencies; simulating --fix-broken before retry.</source>
        <translation>TIEDON NUMERO: APT-korjausta estävät rikkoutuneet riippuvuudet; simulointi -- fix-breaked ennen uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1500"/>
        <source>KNOWN ISSUE: APT correction is blocked by interrupted dpkg configuration; completing dpkg once and re-simulating.</source>
        <translation>TIEDON NUMERO: APT-korjaus on estetty dpkg:n keskeytyksellä, dpkg:n valmistumisella kerran ja simulaatiolla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1501"/>
        <source>full-upgrade simulation did not produce an acceptable transaction; evaluating dist-upgrade.</source>
        <translation>Täysimittainen simulaatio ei tuottanut hyväksyttävää tapahtumaa; arviointi dist-upgrade.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1502"/>
        <source>Full-upgrade simulation was unavailable or unsafe; using the successful standard upgrade transaction.</source>
        <translation>Täysimittaista simulaatiota ei ollut saatavilla tai se ei ollut turvallista; käytössä on onnistunut standardipäivitystapahtuma.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1503"/>
        <source>apt intent translated: %1</source>
        <translation>apt aikomus käännetty: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1504"/>
        <source>APT package lists changed or a repository index was fetched; reporting the metadata refresh as changed.</source>
        <translation>APT-pakettiluettelot ovat muuttuneet tai arkistoindeksi noudettiin; metadatan päivitys on ilmoitettava sellaisena kuin se on muutettuna.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1505"/>
        <source>Target package policy rejected standard upgrade and requested full/dist upgrade; evaluating full-upgrade.</source>
        <translation>Tavoitepaketti politiikka hylkäsi standardin päivitys ja pyysi täyden / Dist päivitystä; arviointi täyden tason.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1506"/>
        <source>APT simulation proposes %1 non-protected package removal(s): %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1507"/>
        <source>REFUSED: APT simulation proposes removing essential packages.</source>
        <translation>EPÄÄMINEN: APT-simulaatiossa ehdotetaan keskeisten pakettien poistamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1508"/>
        <source>REFUSED: APT simulation would remove protected package &apos;%1&apos;.</source>
        <translation>EPÄÄMINEN: APT-simulaatio poistaisi suojatun paketin &quot;%1.&quot;</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1509"/>
        <source>REFUSED: APT simulation would remove %1 packages (safety limit: %2).</source>
        <translation>EPÄÄMINEN: APT-simulaatio poistaisi %1-paketit (turvallisuusraja: %2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1510"/>
        <source>Standard upgrade leaves packages pending; evaluating full-upgrade simulation.</source>
        <translation>Normaali päivitys jättää paketteja odottamaan; arvioi täysimittaista simulaatiota.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1511"/>
        <source>WARN: accepted release metadata change for %1; metadata refreshed.</source>
        <translation>WARN: hyväksytty julkaisumetatiedon muutos %1; metadatan päivitys.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1512"/>
        <source>WARNING: apt-get update refused a repository release metadata change for: %1</source>
        <translation>VAROITUS: apt-get update evättiin arkiston julkaisun metatiedon muutos: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1513"/>
        <source>WARNING: retrying metadata refresh with -o Acquire::AllowReleaseInfoChange=true (release-info changes only; signatures, keys and package verification remain enforced).</source>
        <translation>VAROITUS: uudelleenmetadatan päivittäminen -o Acquire::AllowReleaseInfoChange=true (vain julkaisu-info muutokset; allekirjoitukset, avaimet ja pakettien tarkistus ovat edelleen voimassa).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1514"/>
        <source>APT upgrade decision: &apos;%1&apos; selected from simulation results.</source>
        <translation>APT-päivityspäätös: &apos;%1&apos; valittu simulaatiotuloksista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1515"/>
        <source>apt upgrade is disabled by this distribution; running &apos;%1&apos; instead</source>
        <translation>apt-päivitys ei ole käytössä tällä jakelulla; sen sijaan käytetään &apos;%1&apos;</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1516"/>
        <source>PASS: Arch initramfs images rebuilt and verified.</source>
        <translation>PASS: Arch initramfs kuvat uudelleen ja todennettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1517"/>
        <source>AUTO-CORRECT: %1</source>
        <translation>AUTO-CORRECT: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1518"/>
        <source>AUTO-CORRECT: restored temporary stale mapper alias %1 -&gt; %2 for this repair request.</source>
        <translation>AUTO-CORRECT: kunnostettu väliaikainen vankkumaton mapper alias %1 -&gt; %2 tätä korjauspyyntöä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1519"/>
        <source>BEGIN: Chroot shell command</source>
        <translation>BEGIN: Chroot-kuorikomento</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1520"/>
        <source>BEGIN: Running-host shell command</source>
        <translation>Alku: Suoritus-ohjain komento</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1521"/>
        <source>BEGIN: %1</source>
        <translation>BEGIN: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1522"/>
        <source>BEGIN: package backend %1 (%2)</source>
        <translation>BEGIN: paketti taustaosa %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1523"/>
        <source>BEGIN: Refresh package metadata</source>
        <translation>BEGIN: Päivitä paketin metatiedot</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1524"/>
        <source>Arch GRUB layout detected; applying the guarded conventional EFI reinstall after initramfs preflight.</source>
        <translation>Arch GRUB asettelu havaittu; soveltamalla vartioitu tavanomainen EFI uudelleen asentaa jälkeen initramfs ennen lentoa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1525"/>
        <source>PASS: boot stack reconciliation completed after component simulations and verification.</source>
        <translation>PASS: boot pino täsmäytys suoritettu jälkeen komponenttien simulaatiot ja todentaminen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1526"/>
        <source>PASS: boot stack reconciliation completed; EFI/UKI and GRUB were reused from the earlier EFI / UKI stage while mapper/crypttab and initramfs were reconciled.</source>
        <translation>PASS: boot pino täsmäytys valmis; EFI/UKI ja GRUB uudelleenkäytettiin aikaisemmasta EFI / UKI vaiheessa ja mapper / cryptab ja initramfs täsmättiin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1527"/>
        <source>Boot-stack Arch EFI read-only preflight: PASS (%1, %2, id=%3)</source>
        <translation>Boot-stack Arch EFI luku vain ennen lentoa: PASS (%1, %2, id=%3)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1528"/>
        <source>Fedora BIOS GRUB2 layout detected; no EFI/UKI artifacts are part of boot-stack reconciliation.</source>
        <translation>Fedora BIOS GRUB2 asettelu havaittu; EFI/UKI-esineet eivät ole osa boot-stack täsmäytys.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1529"/>
        <source>No TUXEDO UKI builder detected; preserving the distribution&apos;s existing EFI layout during boot-stack reconciliation.</source>
        <translation>TUXEDO UKI -rakentajaa ei ole havaittu; jakelun nykyinen EFI-asettelu säilytetään käynnistyksen täsmäytyksen aikana.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1530"/>
        <source>NOTE: --post-efi is an EFI-only hint; the Fedora BIOS boot stack reconciles dracut and GRUB2 without an EFI stage.</source>
        <translation>HUOM: -- post-efi on vain EFI:n vihje; Fedora BIOS -kengät täsmäävät dracut:n ja GRUB2:n kanssa ilman EFI-vaihetta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1531"/>
        <source>SIMULATE/PREFLIGHT: complete boot-stack reconciliation</source>
        <translation>SIMULATE/PREFLIGHT: täydellinen boot-stack täsmäytys</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1532"/>
        <source>SKIP: boot-stack EFI/UKI rebuild skipped because the EFI / UKI bootloader stage already rebuilt and verified this layout in the same run (--post-efi).</source>
        <translation>SKIP: boot-stack EFI/UKI remontoitu koska EFI / UKI bootloader vaihe on jo rakennettu uudelleen ja todennettu tämän asettelun samassa juoksussa (- post-efi).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1533"/>
        <source>SKIP: boot-stack GRUB regeneration skipped because the EFI / UKI bootloader stage already regenerated the GRUB configuration in the same run (--post-efi).</source>
        <translation>SKIP: boot-stack GRUB regeneraatio ohitettiin, koska EFI / UKI bootloader vaihe jo regeneroitu GRUB kokoonpano samassa ajossa (- post-efi).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1534"/>
        <source>Boot-stack TUXEDO UKI read-only preflight: PASS (%1, %2)</source>
        <translation>Boot-stack TUXEDO UKI luku vain ennen lentoa: PASS (%1, %2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1535"/>
        <source>Bound recovery-host resolver into target chroot (temporary, read-only)</source>
        <translation>Sidottu elvytys-host resolver kohde Chroot (tilapäinen, luku-vain)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1536"/>
        <source>WARNING: btrfs check --repair is a last-resort operation that upstream documents as dangerous and can make a damaged filesystem worse. The caller must have explicit user confirmation and a backup.</source>
        <translation>VAROITUS: btrfs check --korjaus on viimeinen resort-operaatio, että yläjuoksulla asiakirjat ovat vaarallisia ja voi pahentaa vaurioitunut tiedostojärjestelmä. Soittajalla on oltava käyttäjän vahvistus ja varmuuskopio.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1537"/>
        <source>Chroot shell exit code: %1</source>
        <translation>Chroot kuori exit koodi: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1538"/>
        <location filename="../src/MainWindow.cpp" line="1539"/>
        <source>Command: %1</source>
        <translation>Komento: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1540"/>
        <source>Configuration read skipped: %1 has no parent directory in the target (bootloader backend: %2).</source>
        <translation>Asetukset luettu ohi: %1:llä ei ole emohakemistoa kohteessa (bootloader backend: %2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1541"/>
        <source>Backed up the ESP loader files to %1 before the GRUB EFI reinstall (firmware fallback present before repair: %2).</source>
        <translation>Varmistettu ESP lataaja tiedostot %1 ennen GRUB EFI restall (firmware varalla läsnä ennen korjausta: %2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1542"/>
        <source>PASS: refreshed the firmware fallback loader EFI/BOOT/BOOTX64.EFI (--removable).</source>
        <translation>PASS: Päivitetty firmware varakuormaaja EFI/BOOT/BOOTX64.EFI (--removable).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1543"/>
        <source>Restored the ESP loader files from the session backup %1.</source>
        <translation>Palautti ESP-lataustiedostot istunnon varmuuskopiosta %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1544"/>
        <source>COPY COMPLETE</source>
        <translation>KOKO TÄYDELLINEN</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1545"/>
        <source>COPY COMPLETE — SHA-256 verification FAILED</source>
        <translation>COPY TÄYDELLINEN</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1546"/>
        <source>FAILED verification: %1</source>
        <translation>FAIED-varmennus: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1547"/>
        <source>%1: %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1548"/>
        <source>rsync metadata/content re-check: PASS</source>
        <translation>rsync metadatan/sisällön uudelleentarkistus: PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1549"/>
        <source>SHA-256 verification FAILURES: %1 (the copy is not verified complete)</source>
        <translation>SHA-256 verifiointi FAILURES: %1 (kopiota ei ole tarkistettu kokonaan)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1550"/>
        <source>SHA-256 regular files verified: %1</source>
        <translation>SHA-256 säännöllinen tiedostot todennettu: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1551"/>
        <source>Source items: %1</source>
        <translation>Lähde: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1552"/>
        <source>Unrelated destination files: retained (no --delete used)</source>
        <translation>Liittymättömät kohdetiedostot: säilytetty (ei --poistettu)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1553"/>
        <source>Correction simulation exit code: %1 (%2)</source>
        <translation>Korjaussimulaation poistumiskoodi: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1554"/>
        <source>WARNING: could not mount target %1 from %2; the read-only diagnostic proceeds without it.</source>
        <translation>VAROITUS: kohdetta %1 ei voitu asentaa %2:stä; lukuavain diagnostinen etenee ilman sitä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1555"/>
        <source>Created target destination directory %1 with inherited owner %2:%3</source>
        <translation>Luonut kohde kohde hakemisto %1 perinnöllinen omistaja %2:%3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1556"/>
        <source>Mapper/crypttab consistency gate: PASS</source>
        <translation>Mapper / cryptab johdonmukaisuus portti: PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1557"/>
        <source>Mapper/crypttab gate: no target crypttab entries; PASS</source>
        <translation>Mapper/cryptab-portti: ei kohdekryptab-syötteitä; PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1558"/>
        <source>Mapper/crypttab gate: entry &apos;%1&apos; resolves outside the selected disk: %2</source>
        <translation>Mapper/cryptab-portti: merkintä &apos;%1&apos; ratkaisee valitun levyn ulkopuolella: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1559"/>
        <source>Mapper/crypttab gate: %1 -&gt; %2</source>
        <translation>Mapper/cryptab-portti: %1 -&gt; %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1560"/>
        <source>Mapper/crypttab gate: unresolved entry &apos;%1&apos; -&gt; &apos;%2&apos;</source>
        <translation>Mapper/cryptab-portti: ratkaisematon merkintä &apos;%1&apos; -&gt; &apos;%2&apos;</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1561"/>
        <source>Mapper/crypttab gate: unsupported source syntax for &apos;%1&apos;: %2</source>
        <translation>Mapper/cryptab-portti: tukeematon lähdesyntaksi &apos;%1&apos;:lle: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1562"/>
        <source>Destination: %1</source>
        <translation>Kohde: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1563"/>
        <source>Detected target OS: %1</source>
        <translation>Kohde: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1564"/>
        <source>WARNING: private /dev filter: the selected target disk identity is unavailable; no block device or mapper will be exposed to the target chroot.</source>
        <translation>VAROITUS: yksityinen /dev-suodatin: valittu kohdelevyn henkilöllisyys ei ole saatavilla; kohdechrootille ei anneta lohkolaitetta tai karttaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1565"/>
        <source>Private /dev filter: %1 allowed block devices/mappers copied; %2 non-essential device-tree entries refused (plain files, sockets, fifos, and devices outside the selected target disk).</source>
        <translation>Yksityinen /dev-suodatin: %1:n sallimat lohkolaitteet/mapperit kopioitu; %2:n muut kuin olennaiset laite-puumerkinnät hylätty (plain-tiedostot, pistorasiat, fifos ja valitun kohdelevyn ulkopuoliset laitteet).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1566"/>
        <source>DKMS preflight correction: build tree now present for %1</source>
        <translation>DKMS:n lentoa edeltävä korjaus: %1:n rakennuspuu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1567"/>
        <source>SIMULATE/PREFLIGHT: dracut DKMS rebuild (headers must already be installed; no package guessing is performed)</source>
        <translation>SIMULATE/PREFLIGHT: dracut DKMS:n uudelleenrakentaminen (otsimet on jo asennettava; paketin arvauksia ei tehdä)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1568"/>
        <location filename="../src/MainWindow.cpp" line="1569"/>
        <location filename="../src/MainWindow.cpp" line="1570"/>
        <source>DKMS preflight: headers/build tree present for %1</source>
        <translation>DKMS ennen lentoa: %1:n otsake/rakennuspuu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1571"/>
        <source>KNOWN ISSUE: DKMS reported missing kernel headers; re-running header preflight/correction once.</source>
        <translation>TIEDON NUMERO: DKMS ilmoitti puuttuvansa ytimen otsikot; uudelleen käynnissä otsikko ennen lentoa / korjaus kerran.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1572"/>
        <source>DKMS preflight: missing headers for %1; repository package %2 is available.</source>
        <translation>DKMS esilento: puuttuvat otsikot %1:lle; arkistopaketti %2 on saatavilla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1573"/>
        <source>WARNING: DKMS preflight found no build tree for %1 and %2 is unavailable from configured repositories.</source>
        <translation>VAROITUS: DKMS:n ennen lentoa ei löytynyt %1:lle eikä %2:lle rakennettua puuta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1574"/>
        <source>SIMULATE/PREFLIGHT: mkinitcpio DKMS rebuild (headers must already be installed; no package guessing is performed)</source>
        <translation>SIMULATE/PREFLIGHT: mkinitcpio DKMS:n uudelleenrakentaminen (otsikot on jo asennettava; ei paketin arvausta)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1575"/>
        <source>SIMULATE/PREFLIGHT: mkinitfs DKMS rebuild (headers must already be installed; no package guessing is performed)</source>
        <translation>SIMULATE/PREFLIGHT: mkinitfs DKMS:n uudelleenrakentaminen (otsakkeet on jo asennettava; ei paketin arvauksia)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1576"/>
        <source>PASS: DKMS autoinstall completed.</source>
        <translation>PASS: DKMS-automaattiasennus valmis.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1577"/>
        <source>SIMULATE/PREFLIGHT: DKMS module rebuild</source>
        <translation>SIMULATE/PREFLIGHT: DKMS-moduulin jälleenrakentaminen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1578"/>
        <source>%1 was configured offline only; Boot Bitch intentionally did not start a graphical session.</source>
        <translation>%1 konfiguroitu vain offline; Boot Bitch tarkoituksella ei aloittanut graafista istuntoa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1579"/>
        <source>%1 was configured offline only; Boot Bitch intentionally did not start a graphical session inside the chroot.</source>
        <translation>%1 konfiguroitu vain offline; Boot Bitch tarkoituksella ei aloittanut graafista istuntoa sisällä Chroot.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1580"/>
        <source>PASS: default.target -&gt; %1</source>
        <translation>PASS: oletustavoite -&gt; %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1581"/>
        <source>Detected display manager: %1 (%2)</source>
        <translation>Havaittu näytönhallinta: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1582"/>
        <source>Enabling %1 in the offline target (it will NOT be started inside the repair chroot).</source>
        <translation>%1:n mahdollistaminen offline-kohteessa (se EI käynnisty korjauschrootin sisällä).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1583"/>
        <source>WARNING: %1 ExecStart is not executable in the target: %2</source>
        <translation>VAROITUS: %1 ExecStart ei ole suorituskelpoinen kohteessa: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1584"/>
        <source>PASS: %1 ExecStart is present for headless preflight: %2</source>
        <translation>PASS: %1 ExecStart on läsnä päättömänä ennen lentoa: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1585"/>
        <source>PASS: display-manager.service -&gt; %1</source>
        <translation>PASS: näyttö-manager.service -&gt; %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1586"/>
        <source>PASS: %1 command present for headless preflight: %2</source>
        <translation>PASS: %1-komento paikalla päättömänä ennen lentoa: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1587"/>
        <source>Detected Alpine OpenRC display manager: %1 (%2)</source>
        <translation>Havaittu Alpine OpenRC -näytön hallinta: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1588"/>
        <source>PASS: /etc/runlevels/default/%1 -&gt; %2</source>
        <translation>PASS: /etc/run levels/default/%1 -&gt; %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1589"/>
        <source>WARNING: no explicit command= was found for %1; sh -n and rc-service -e still passed.</source>
        <translation>VAROITUS: %1:lle ei löytynyt nimenomaista komentoa; sh -n ja rc-service -e läpäistiin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1590"/>
        <source>%1 will be configured offline only; Boot Bitch will not start a graphical session.</source>
        <translation>%1 konfiguroidaan vain offline-tilassa; Boot Bitch ei käynnistä graafista istuntoa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1591"/>
        <source>Restoring the OpenRC default-runlevel link for %1 (offline only; the service is NOT started).</source>
        <translation>%1:n OpenRC-lähtötason linkin palauttaminen (vain offline; palvelua EI aloiteta).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1592"/>
        <source>SIMULATE/PREFLIGHT: OpenRC graphical login / display manager (offline only)</source>
        <translation>SIMULATE/PREFLIGHT: OpenRC graafinen kirjautuminen / näyttö hallinta (vain offline)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1593"/>
        <source>Display-manager preflight plan: set graphical.target default, enable %1, and repair display-manager.service offline.</source>
        <translation>Display-manager esilentosuunnitelma: aseta graafinen.kohde oletus, ota %1 käyttöön ja korjaa näyttö-manager.service offline.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1594"/>
        <source>Restoring graphical.target as the target default.</source>
        <translation>Palautetaan graafinen kohde oletuksena.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1595"/>
        <source>Selected %1 from last-boot display-manager journal evidence.</source>
        <translation>Valittu %1 viime saappaan näytön manager päiväkirjan todisteita.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1596"/>
        <source>Selected %1 from last-boot display-manager journal evidence because multiple managers are installed.</source>
        <translation>Valittu %1 viime saappaan näyttö-hallinta päiväkirjan todisteita, koska useita managereita on asennettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1597"/>
        <source>SIMULATE/PREFLIGHT: graphical login / display manager</source>
        <translation>SIMULATE/PREFLIGHT: graafinen kirjautuminen / näytön hallinta</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1598"/>
        <source>%1 is a static unit; preserving it through the display-manager.service alias.</source>
        <translation>%1 on staattinen yksikkö, joka säilyttää sen näytönohjaimen läpi. Palvelunimi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1599"/>
        <source>WARNING: systemd-analyze is unavailable; skipped headless display-manager verification.</source>
        <translation>VAROITUS: systemd-analysoiminen ei ole käytettävissä; ohitettu päätön näyttö-manager vahvistus.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1600"/>
        <source>WARNING: headless systemd verification reported issues for %1 (the manager will not be started during repair).</source>
        <translation>VAROITUS: Päätön järjestelmällinen todentaminen raportoitu ongelmia %1 (johtaja ei käynnisty korjauksen aikana).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1601"/>
        <source>PASS: headless systemd verification for %1</source>
        <translation>PASS: Päätön järjestelmällinen verifiointi %1:lle</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1602"/>
        <source>This transaction downloads approximately %1; the apply step reports its progress only when it finishes.</source>
        <translation>Tämä tapahtuma lataa noin %1; sovellettava vaihe raportoi sen etenemistä vain, kun se päättyy.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1603"/>
        <source>PASS: dnf5 upgrade removes only the old kernel package(s) (%1) — accepted as in-place kernel replacement.</source>
        <translation>PASS: dnf5 päivitys poistaa vain vanhan ytimen paketti(s) (%1) .</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1604"/>
        <source>dnf5 repository metadata cache rewritten (cache fingerprint changed); reporting the metadata refresh as changed.</source>
        <translation>dnf5 arkiston metadata välimuistin uudelleenkirjoitettu (välimuistin sormenjälki muuttunut); metadatan päivitys muuttunut.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1605"/>
        <source>PASS: dnf5 metadata cache refreshed (repository metadata cache is byte-identical).</source>
        <translation>PASS: dnf5 metadata välimuisti päivitetty (repository metadata välimuisti on byte-identtinen).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1606"/>
        <source>REFUSED: dnf5 simulation reported an unresolved or conflicting transaction.</source>
        <translation>EPÄÄMINEN: dnf5-simulaatio ilmoitti selvittämättömästä tai ristiriitaisesta tapahtumasta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1607"/>
        <source>REFUSED: dnf5 simulation proposes a package downgrade.</source>
        <translation>EPÄÄMINEN: dnf5-simulaatiossa ehdotetaan paketin vähentämistä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1608"/>
        <source>REFUSED: dnf5 simulation failed with exit code %1.</source>
        <translation>EPÄÄMISESTÄ: dnf5-simulaatio epäonnistui lähtökoodin %1 kanssa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1609"/>
        <source>REFUSED: dnf5 simulation reported a locked package database.</source>
        <translation>EPÄÄMINEN: dnf5-simulaatio ilmoitti lukitusta pakettitietokannasta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1610"/>
        <source>REFUSED: dnf5 simulation reported incomplete repository metadata.</source>
        <translation>EPÄÄMINEN: dnf5-simulaatio ilmoitti epätäydellisistä arkistometatiedoista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1611"/>
        <source>REFUSED: dnf5 upgrade would obsolete critical package &apos;%1&apos;.</source>
        <translation>EPÄÄMINEN: dnf5-päivitys olisi vanhentunut kriittinen paketti &quot;%1.&quot;</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1612"/>
        <source>REFUSED: dnf5 reinstall would replace &apos;%1&apos;, which is not one of the reinstalled packages.</source>
        <translation>EPÄÄMINEN: dnf5 reinstall korvaisi &quot;%1:n,&quot; joka ei kuulu asennettuihin pakkauksiin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1613"/>
        <source>REFUSED: dnf5 simulation proposes package removals.</source>
        <translation>EPÄÄMINEN: dnf5-simulaatiossa ehdotetaan pakettien poistamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1614"/>
        <source>REFUSED: dnf5 simulation would replace %1 packages (safety limit: %2).</source>
        <translation>EPÄÄMINEN: dnf5-simulaatio korvaisi %1-pakkaukset (turvallisuusraja: %2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1615"/>
        <source>REFUSED: dnf5 simulation reported an untrusted or missing package signature key.</source>
        <translation>EPÄÄMINEN: dnf5-simulaatio ilmoitti luottamattomasta tai puuttuvasta paketin allekirjoitusavaimesta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1616"/>
        <source>REFUSED: dnf5 simulation proposes %1 package changes (safety limit: %2).</source>
        <translation>EPÄÄMINEN: dnf5-simulaatiossa ehdotetaan %1-paketin muutoksia (turvallisuusraja: %2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1617"/>
        <source>REFUSED: dnf5 simulation reported an error Boot Bitch does not recognize as safe.</source>
        <translation>EPÄÄMINEN: dnf5-simulaatio ilmoitti virheen Boot Bitch ei tunnista sitä turvalliseksi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1618"/>
        <source>dnf5 preflight version: %1</source>
        <translation>dnf5-versio ennen lentoa: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1619"/>
        <source>SIMULATE/PREFLIGHT: dracut guarded per-kernel rebuild for %1 installed kernel(s) (single build per kernel; verified before any /boot write)</source>
        <translation>SIMULATE/PREFLIGHT: dracut vartioitu %1-ytimen uudelleenrakennettu (yksi rakenne ytimessä; todennettu ennen /boot kirjoitus)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1620"/>
        <source>dracut preflight version: %1</source>
        <translation>dracut esilentoversio: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1621"/>
        <source>e2fsck preen left uncorrected errors (exit code 4); running the confirmed forced repair pass.</source>
        <translation>e2fsck preen vasen korjaamaton virhe (poistumakoodi 4); käynnissä vahvistettu pakotettu korjauspassi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1622"/>
        <source>Annotating selected-system EFI entries with OS/model identity: %1 / %2 (PARTUUID %3).</source>
        <translation>Ilmoitetaan valittu järjestelmä EFI merkinnät OS / malli identiteetti: %1 / %2 (PARTUUID %3).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1623"/>
        <source>EFI bootloader ID: %1</source>
        <translation>EFI bootloader ID: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1624"/>
        <source>PASS: EFI BootOrder already groups each drive&apos;s firmware destinations by normal use.</source>
        <translation>PASS: EFI BootOrder ryhmittää jo jokaisen aseman firmware kohteet normaaliin käyttöön.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1625"/>
        <source>PASS: EFI BootOrder grouped by drive; all retained firmware entries remain present.</source>
        <translation>PASS: EFI BootOrder ryhmitelty aseman mukaan; kaikki säilytetyt firmware merkinnät pysyvät läsnä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1626"/>
        <source>Grouping EFI BootOrder by drive and normal boot use: %1</source>
        <translation>EFI BootOrderin ryhmittely ajon ja normaalin boot-käytön avulla: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1627"/>
        <source>Clearing BootNext introduced during EFI repair.</source>
        <translation>Clearing BootNext otettiin käyttöön EFI korjaus.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1628"/>
        <source>PASS: Boot%1 is first in BootOrder; all other entries were retained.</source>
        <translation>PASS: Boot%1 on ensimmäinen BootOrder; kaikki muut merkinnät pidettiin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1629"/>
        <source>PASS: selected ESP firmware destinations reconciled; removed Boot%1.</source>
        <translation>PASS: Valitut ESP firmware kohteet sovitettu; poistettu Boot%1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1630"/>
        <source>PASS: selected ESP firmware destinations are unique; no duplicate entries removed.</source>
        <translation>PASS: Valitut ESP-ohjelmistokohteet ovat ainutlaatuisia; kaksoismerkintää ei ole poistettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1631"/>
        <source>Removed firmware boot entry Boot%1.</source>
        <translation>Poistettu firmware boot enter Boot%1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1632"/>
        <source>PASS: %1 firmware entry restored as Boot%2 on selected system ESP %3.</source>
        <translation>PASS: %1 firmware sisääntulo palautettu Boot%2 valitussa järjestelmässä ESP %3.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1633"/>
        <source>PASS: generic EFI firmware entry restored as Boot%1 on selected system ESP %2.</source>
        <translation>PASS: yleinen EFI firmware merkintä palautettu Boot%1 valitussa järjestelmässä ESP %2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1634"/>
        <source>EFI grouping did not retain Boot%1 first; re-promoting it once.</source>
        <translation>EFI-ryhmä ei säilyttänyt Boot%1:ää ensin; sen edistäminen uudelleen kerran.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1635"/>
        <source>EFI entry identity preserved while firmware ID changed: Boot%1 -&gt; Boot%2.</source>
        <translation>EFI-tietue säilyi, kun ohjelmistotunniste muuttui: Boot%1 -&gt; Boot%2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1636"/>
        <source>iPXE/WebFAI EFI loader is absent on selected system ESP; no recovery firmware entry was created.</source>
        <translation>iPXE/WebFAI EFI-kuormaajaa ei ole valitussa järjestelmässä ESP; palautusohjelmistoja ei ole luotu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1637"/>
        <source>iPXE/WebFAI EFI loader is present on the selected system ESP, but firmware variables are not writable; no recovery firmware entry was created.</source>
        <translation>iPXE/WebFAI EFI-kuormaaja on mukana valitussa järjestelmässä ESP, mutta firmware-muuttujat eivät ole kirjoitettavia; firmware-tiedostoa ei ole luotu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1638"/>
        <source>Repair-target EFI entry label preserved while loader changed: Boot%1 -&gt; Boot%2.</source>
        <translation>Korjaa kohde EFI merkintä etiketti säilytetään samalla kuormaaja muuttunut: Boot%1 -&gt; Boot%2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1639"/>
        <source>EFI Boot%1 already names selected model; leaving label &apos;%2&apos; unchanged.</source>
        <translation>EFI Boot%1 jo nimittää valitun mallin, jolloin etiketti &apos;%2&apos; ei muutu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1640"/>
        <source>EFI Boot%1 (%2) label updated to &apos;%3&apos; on selected system ESP only.</source>
        <translation>EFI Boot%1 (%2) - nimilappu päivitetty &quot;%3&quot; vain valitussa järjestelmässä ESP.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1641"/>
        <source>PASS: selected ESP EFI labels retain the drive model after final BootOrder maintenance.</source>
        <translation>PASS: Valitut ESP EFI tarrat säilyttävät ajomallin viimeisen BootSertder-huollon jälkeen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1642"/>
        <source>Selected EFI loader is present at %1, but firmware variables are not writable; no generic firmware entry was created.</source>
        <translation>Valittu EFI-kuormaaja on läsnä %1:ssä, mutta firmware-muuttujat eivät ole kirjoitettavia; geneerisiä firmware-ohjelmia ei luotu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1643"/>
        <source>PASS: EFI loader files verified under %1/EFI/%2</source>
        <translation>PASS: EFI-kuormaajatiedostot todennettu %1/EFI/%2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1644"/>
        <source>Making Boot%1 the explicit default while preserving all other EFI entries: %2</source>
        <translation>Tehdään Boot%1:stä nimenomainen oletus säilyttäen samalla kaikki muut EFI-syötteet: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1645"/>
        <source>Multiple generic EFI entries already point to the selected system loader (%1); destination maintenance will retain one after the repair.</source>
        <translation>Useat yleiset EFI merkinnät osoittavat jo valittuun järjestelmäkuormaajaan (%1); määränpään huolto säilyttää yhden korjauksen jälkeen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1646"/>
        <source>Multiple iPXE/WebFAI entries already point to the selected system ESP (%1); destination maintenance will retain one after the repair.</source>
        <translation>Useat iPXE/WebFAI-merkinnät osoittavat jo valittuun ESP-järjestelmään (%1); kohdehuolto säilyttää yhden korjauksen jälkeen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1647"/>
        <source>Multiple TUXEDO UKI entries already point to the selected ESP (%1); destination maintenance will retain one after the repair.</source>
        <translation>Useat TUXEDO UKI merkinnät osoittavat jo valittuun ESP (%1); kohde huolto säilyttää yhden jälkeen korjaus.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1648"/>
        <source>Selected EFI vendor directory has no primary loader; no generic firmware entry was created.</source>
        <translation>Valitussa EFI-toimittajahakemistossa ei ole ensisijaista laturia; geneerisiä firmware-ohjelmia ei luotu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1649"/>
        <source>KNOWN ISSUE: firmware NVRAM registration failed; retrying once as a file-only --no-nvram reinstall.</source>
        <translation>TIEDON NUMERO: firmware NVRAM -rekisteröinti epäonnistui; yritetään uudelleen kerran vain tiedostona -- no-nvram uudelleenasennukseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1650"/>
        <source>EFI System Partition: %1 (%2)</source>
        <translation>EFI System Partition: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1651"/>
        <source>EFI registration mode: %1</source>
        <translation>EFI-rekisteröintitila: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1652"/>
        <source>EFI registration mode: Alpine --no-nvram install with helper-managed firmware entries (writable efivars: %1).</source>
        <translation>EFI-rekisteröintitila: Alpine --no-nvram asennusavulla ohjatut firmware-merkinnät (kirjoitettavat efivars: %1).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1653"/>
        <source>Removing duplicate/legacy selected-ESP firmware destination Boot%1.</source>
        <translation>Poistaa kaksois-/legacy valittu-ESP firmware kohde Boot%1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1654"/>
        <source>Renamed firmware boot entry Boot%1 to &apos;%2&apos;.</source>
        <translation>Nimetty firmware boot enter Boot%1 to &apos;%2&apos;.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1655"/>
        <source>Set EFI BootOrder to %1.</source>
        <translation>Aseta EFI BootOrder %1:ksi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1656"/>
        <source>EFI BootOrder already matches the requested order %1.</source>
        <translation>EFI BootOrder vastaa jo pyydettyä tilausta %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1657"/>
        <source>EFI repair read-only preflight: PASS (%1, %2, id=%3)</source>
        <translation>EFI korjaus vain ennen lentoa: PASS (%1, %2, id=%3)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1658"/>
        <source>EFI repair completed; simulating and regenerating the GRUB fallback configuration.</source>
        <translation>EFI korjaus valmis; simuloi ja uudistaa GRUB varakokoonpano.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1659"/>
        <source>Restoring reconciled BootNext: %1</source>
        <translation>Yhteensovitetun BootNextin palauttaminen: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1660"/>
        <source>Restoring reconciled EFI BootOrder (all pre-existing entries retained): %1</source>
        <translation>Yhteensovitetun EFI BootOrderin palauttaminen (kaikki aiemmat merkinnät säilytetään): %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1661"/>
        <source>Restoring %1 firmware entry &apos;%2&apos; for PARTUUID %3 (EFI path %4).</source>
        <translation>Palautetaan %1-ohjelmiston merkintä &quot;%2&quot; PAPUUID %3:lle (EFI-polku %4).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1662"/>
        <source>Restoring missing %1 firmware entry on selected system ESP %2 as &apos;%3&apos; (%4).</source>
        <translation>Palautetaan puuttuva %1-ohjelmistomerkintä valitussa järjestelmässä ESP %2 nimellä &apos;%3&apos; (%4).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1663"/>
        <source>Restoring missing generic EFI firmware entry on selected system ESP %1 as &apos;%2&apos; (%3).</source>
        <translation>Palautetaan puuttuva yleinen EFI-ohjelmistomerkintä valitussa järjestelmässä ESP %1 nimellä &apos;%2&apos; (%3).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1664"/>
        <source>Alpine EFI-stub repair read-only preflight: PASS (%1, %2)</source>
        <translation>Alpine EFI-stib korjaus luku vain ennen lentoa: PASS (%1, %2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1665"/>
        <source>TUXEDO UKI firmware entry is absent for the selected system ESP; creating only that selected-system entry as &apos;%1&apos;.</source>
        <translation>TUXEDO UKI firmware -merkintä puuttuu valitusta järjestelmästä ESP; luodaan vain se valittu järjestelmämerkintä &quot;%1.&quot;</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1666"/>
        <source>Writable UEFI efivars detected; first attempt will permit firmware registration.</source>
        <translation>Writable UEFI efivars havaittu; ensimmäinen yritys mahdollistaa firmware rekisteröinti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1667"/>
        <source>Writable UEFI efivars are unavailable; using the preflight-selected --no-nvram path.</source>
        <translation>Kirjoitettavat UEFI efivarit eivät ole käytettävissä; käytetään esilentoon valittua --nvram-polkua.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1668"/>
        <source>ESP mount: auto-mount failed: target=%1 reason=%2 hint=%3</source>
        <translation>ESP-asennus: Auto-mount epäonnistui: kohde=%1 syy=%2 vihje= %3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1669"/>
        <source>ESP mount: auto-mount failed: target=%1 reason=mounted source %2 is not a block device on the selected target disk hint=%3</source>
        <translation>ESP-asennus: Auto-mount epäonnistui: tavoite=%1 syy = asennettu lähde %2 ei ole lohko laite valittu kohdelevy vihje = %3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1670"/>
        <source>ESP mount: auto-mount failed: target=%1 reason=mounted source %2 is %3, not FAT hint=%4</source>
        <translation>ESP-asennus: Auto-mount epäonnistui: tavoite=%1 syy = asennettu lähde %2 on %3, ei FAT vihje = %4</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1671"/>
        <source>ESP mount cleanup: unmounted leaked ro layer target=%1 source=%2 id=%3</source>
        <translation>ESP asennus siivous: asentamaton ro kerros tavoite=%1 lähde=%2 id=%3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1672"/>
        <source>ESP mount preflight: target=%1 stack=%2 top-source=%3 top-options=%4 top-id=%5 verdict=%6 leaked-ro=%7</source>
        <translation>ESP mount preflight: kohde=%1 pino=%2 top-source=%3 top-optiot=%4 top-id=%5 tuomio=%6 vuoto-ro=%7</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1673"/>
        <source>ESP mount: mounted target=%1 source=%2 method=%3</source>
        <translation>ESP-asennus: asennettu kohde=%1-lähde=%2-menetelmä=%3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1674"/>
        <source>Backed up the extlinux configuration under %1</source>
        <translation>Varmistettu extlinux-konfiguraatiolla %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1675"/>
        <source>PASS: extlinux candidate references %1 boot artifact(s) present in the target</source>
        <translation>PASS: extlinux ehdokas viittaa %1 boot artefact [s] läsnä kohde</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1676"/>
        <source>PASS: running host default extlinux entry is LABEL %1 -&gt; LINUX %2 + INITRD %3.</source>
        <translation>PASS: käynnissä isäntä oletus extlinux merkintä on ETIKETTI %1 -&gt; LINUX %2 + INITRD %3.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1677"/>
        <source>Restored the pre-repair extlinux default configuration</source>
        <translation>Palautti extlinux:n oletusasetukset</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1678"/>
        <source>Extlinux default entry: label=%1 kernel=%2 action=set</source>
        <translation>Extlinux-oletusmerkintä: etiketti=%1-ydin=%2-toiminto=setti</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1679"/>
        <source>Extlinux default entry: label=%1 kernel=%2 action=unchanged</source>
        <translation>Extlinux-oletussyöte: etiketti=%1-ytimen=%2-toiminto=muuttumaton</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1680"/>
        <source>PASS: /boot/extlinux.conf regenerated and verified; the boot sector was not written.</source>
        <translation>PASS: /boot/extlinux.conf regeneroitu ja tarkistettu; käynnistyssektoria ei ole kirjoitettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1681"/>
        <source>Restored the pre-repair extlinux configuration</source>
        <translation>Korjaa extlinux-kokoonpano</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1682"/>
        <source>SIMULATE/PREFLIGHT: extlinux configuration regeneration via %1 (overwrite=0 trial; no boot sector write)</source>
        <translation>SIMULATE/PREFLIGHT: extlinux:n konfiguraation regeneraatio %1:n avulla (korvaa=0 tutkimus; ei käynnistyssektorin kirjoitusta)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1683"/>
        <source>Fedora default entry: saved_entry=%1 resolves=%2 target=%3 action=set</source>
        <translation>Fedora-oletussyöte: tallennettu entry=%1 ratkaisee=%2-tavoite=%3-toiminto=asetettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1684"/>
        <source>Fedora default entry: saved_entry=%1 resolves=yes target=%2 action=set</source>
        <translation>Fedora-oletussyöte: saved entry=%1 ratkaisee=kyllä kohde=%2 toiminto=asetettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1685"/>
        <source>Fedora default entry: saved_entry=%1 resolves=yes target=%2 action=unchanged</source>
        <translation>Fedora-oletussyöte: tallennettu entry=%1 ratkaisee=kyllä kohde=%2 toiminto=muuttumaton</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1686"/>
        <source>Backed up Fedora GRUB2 configuration artifacts to %1.</source>
        <translation>Varmistin Fedora GRUB2:n konfiguraatioesineet %1: lle.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1687"/>
        <source>PASS: Fedora GRUB2 configuration regenerated and verified.</source>
        <translation>PASS: Fedora GRUB2 asetus regeneroitu ja tarkistettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1688"/>
        <source>Restored the Fedora GRUB2 configuration artifacts from %1.</source>
        <translation>Palautti Fedora GRUB2-kokoonpanon %1:stä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1689"/>
        <source>SIMULATE/PREFLIGHT: generate Fedora GRUB2 configuration with --no-grubenv-update</source>
        <translation>SIMULATE/PREFLIGHT: luo Fedora GRUB2-asetukset -- no-grubenv-update</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1690"/>
        <source>PASS: Fedora GRUB2 trial configuration generated successfully.</source>
        <translation>PASS: Fedora GRUB2 kokeiluversio luotiin onnistuneesti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1691"/>
        <source>Fedora default entry: grubenv keys other than saved_entry preserved (sha256 %1…).</source>
        <translation>Fedora oletusmerkintä: grubenv avaimet muut kuin tallennettu entry säilynyt (sha256 %1…).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1692"/>
        <source>PASS: Fedora initramfs for %1 rebuilt byte-identical; temporary build discarded.</source>
        <translation>PASS: Fedora initramfs %1 restructed byte-identical; väliaikainen rakentaa hylätty.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1693"/>
        <source>PASS: Fedora initramfs rebuilt and verified for %1</source>
        <translation>PASS: Fedora initramfs uudelleenrakennettu ja tarkistettu %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1694"/>
        <source>File Copy %1: %2</source>
        <translation>Tiedostokopio %1: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1695"/>
        <source>Simulation exit code for &apos;--fix-broken install&apos;: %1</source>
        <translation>Simulointi exit code for &quot;-fix-broken install&quot;: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1696"/>
        <source>Released the helper&apos;s read-only mount(s) of %1 before offline repair.</source>
        <translation>Vapautti %1:n luku-avainkiinnikkeet ennen offline-korjausta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1697"/>
        <source>REPAIR: %1 (%2) on %3 (fstype=%4, uuid=%5, mount=%6)</source>
        <translation>KORJAUS: %1 (%2) on %3 (fstype=%4, uud=%5, mount=%6)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1698"/>
        <source>FAIL: file system repair (%1) reported unresolved issues for %2 (exit code %3).</source>
        <translation>FAIL: tiedostojärjestelmän korjaus (%1) ilmoitti ratkaisemattomia kysymyksiä %2 (poistumiskoodi %3).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1699"/>
        <source>PASS: file system repair (%1) corrected errors on %2 (exit code %3).</source>
        <translation>PASS: tiedostojärjestelmän korjaus (%1) korjatut virheet %2 (poistumiskoodi %3).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1700"/>
        <source>PASS: file system repair (%1) completed for %2 with exit code 0.</source>
        <translation>PASS: tiedostojärjestelmän korjaus (%1) valmis %2 kanssa exit koodi 0.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1701"/>
        <source>PASS: file system repair (%1) corrected errors on %2; a reboot is recommended before using the filesystem (exit code 2).</source>
        <translation>PASS: tiedostojärjestelmän korjaus (%1) korjatut virheet %2; uudelleenkäynnistys suositellaan ennen tiedostojärjestelmän käyttöä (poistumiskoodi 2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1702"/>
        <source>FAIL: file system repair (%1) timed out for %2 (exit code %3).</source>
        <translation>FAIL: tiedostojärjestelmän korjaus (%1) ajoitettu pois %2 (poistettu koodi %3).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1703"/>
        <source>File system inspection found no resolvable scope filesystems (root, /boot, ESP and /home).</source>
        <translation>Tiedostojärjestelmän tarkastus ei löytänyt resoluutiotiedostojärjestelmiä (root, /boot, ESP ja / kotiin).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1704"/>
        <source>File system inspection started (read-only; no repair tool is invoked).</source>
        <translation>Tiedostojärjestelmän tarkastus aloitettiin (vain luku; korjaustyökalua ei käytetä).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1705"/>
        <source>WARNING: fstab %1 device %2 is not on the selected disk %3; excluded from the file system scope.</source>
        <translation>VAROITUS: fstab %1-laite %2 ei ole valitulla levyllä %3; jätetään tiedostojärjestelmän soveltamisalan ulkopuolelle.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1706"/>
        <source>Captured complete firmware entry state before conventional GRUB EFI install: %1</source>
        <translation>Kaapattu täydellinen firmware sisääntulotila ennen tavanomaista GRUB EFI asennus: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1707"/>
        <source>SIMULATE/PREFLIGHT: GRUB EFI install target=%1 fs=%2 id=%3 mode=%4</source>
        <translation>SIMULATE/PREFLIGHT: GRUB EFI asenna kohde=%1 fs=%2 id=%3-tila=%4</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1708"/>
        <source>foreign-entry-added: %1</source>
        <translation>Suomeen lisätty: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1709"/>
        <source>foreign-entry-removed: %1</source>
        <translation>Ulkomailta poistuneet: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1710"/>
        <source>menu-entry-added: %1</source>
        <translation>Valikko lisätty: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1711"/>
        <source>GRUB menu entries added by the candidate: %1</source>
        <translation>GRUB valikko merkinnät lisätään ehdokas: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1712"/>
        <source>Using grub-mkconfig with an isolated output path for preflight.</source>
        <translation>grub-mkconfig:n käyttäminen erillisellä lähtöreitillä ennen lentoa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1713"/>
        <source>PASS: GRUB configuration regenerated and verified.</source>
        <translation>PASS: GRUB konfiguraatio regeneroitu ja tarkistettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1714"/>
        <source>SIMULATE/PREFLIGHT: generate GRUB configuration to temporary session output</source>
        <translation>SIMULATE/PREFLIGHT: luo GRUB-konfiguraatio tilapäiseen istuntolähtöön</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1715"/>
        <source>KNOWN ISSUE: GRUB trial failed on a stale mapper path; retrying once after compatibility alias correction.</source>
        <translation>TUNTEMATON NUMERO: GRUB-koe epäonnistui väljällä karttapolulla; yritetään uudelleen kerran yhteensopivuuden jälkeen alias korjaus.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1716"/>
        <source>Trial GRUB generation exit code: %1</source>
        <translation>Trial GRUB generation exit code: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1717"/>
        <source>PASS: GRUB trial configuration generated successfully.</source>
        <translation>PASS: GRUB kokeiluversio luotiin onnistuneesti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1718"/>
        <source>Backed up the package selection and modprobe.d state to %1.</source>
        <translation>Paketin valinta ja modprobe.d tila %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1719"/>
        <source>Wrote the blacklist for the conflicting module %1.</source>
        <translation>Kirjoitti mustan listan ristiriitaisesta moduulista %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1720"/>
        <source>GPU driver install plan: %1</source>
        <translation>GPU-ajurin asennussuunnitelma: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1721"/>
        <source>Set the one-boot nomodeset next_entry (consumed automatically after one boot).</source>
        <translation>Aseta yhden saappaan nomodeset next entry (merkitty automaattisesti yhden bootin jälkeen).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1722"/>
        <source>GPU driver purge plan: %1</source>
        <translation>GPU-ajurin puhdistussuunnitelma: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1723"/>
        <source>Restored the package selection and modprobe.d state from %1.</source>
        <translation>Palautti paketin valinnan ja modprobe.d tila %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1724"/>
        <source>Backed up GRUB2 BIOS boot code (MBR, BIOS boot partition, i386-pc modules) to %1.</source>
        <translation>Varmistettu GRUB2 BIOS boot koodi (MBR, BIOS boot osio, i386-pc moduulit) %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1725"/>
        <source>NOTE: GRUB2 BIOS boot-code reinstall is only implemented for a mounted repair target; the running host stays config-only.</source>
        <translation>HUOMAUTUS: GRUB2 BIOS-käynnistyskoodi uudelleenasentaminen toteutetaan vain asennettu korjauskohde; käynnissä isäntä pysyy vain config-.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1726"/>
        <source>PASS: GRUB2 BIOS boot code reinstalled and verified (partition table byte-identical).</source>
        <translation>PASS: GRUB2 BIOS-käynnistyskoodi uudelleen asennettuna ja todennettuna (osittainen taulukko).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1727"/>
        <source>GRUB2 boot-code probe: MBR and BIOS boot partition contain a GRUB signature; config-only regeneration.</source>
        <translation>GRUB2-käynnistin: MBR ja BIOS-saappaan osio sisältää GRUB- allekirjoituksen; ainoastaan konfigin regeneraatio.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1728"/>
        <source>REPAIR: reinstall GRUB2 BIOS boot code (%1 --target=i386-pc --boot-directory=/boot --recheck %2)</source>
        <translation>KORJAUS: Asenna uudelleen GRUB2 BIOS- käynnistyskoodi (%1 --tavoite=i386-kpl --boot-kansio=/boot -- tarkista %2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1729"/>
        <source>Restored the GRUB2 BIOS boot code from %1.</source>
        <translation>Palautti GRUB2 BIOS-alkukoodin %1:stä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1730"/>
        <source>GRUB2 config-only reconciliation requested; the BIOS boot-code reinstall substage is skipped.</source>
        <translation>GRUB2:lta vaaditaan vain konfigurointia; BIOS-käynnistyskoodi uudelleenasennetaan alavaiheen ulkopuolelle.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1731"/>
        <source>UEFI firmware detected; the GRUB2 BIOS boot-code reinstall substage is not applicable.</source>
        <translation>UEFI-ohjelmisto havaittu; GRUB2 BIOS- käynnistyskoodi uudelleen asentaa alavaiheen ei sovelleta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1732"/>
        <source>Host boot-stack Arch EFI read-only preflight: PASS (%1, %2, id=%3)</source>
        <translation>Isäntä-pino Arch EFI luku vain ennen lentoa: PASS (%1, %2, id=%3)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1733"/>
        <source>Host boot-stack TUXEDO UKI read-only preflight: PASS (%1, %2)</source>
        <translation>Isäntäkäynnistys TUXEDO UKI luku vain ennen lentoa: PASS (%1, %2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1734"/>
        <source>Host command guard: firmware variables are read-only to package/kernel/vendor hooks; the helper owns explicit firmware registration.</source>
        <translation>Isännän komennon vartiointi: firmware-muuttujat luetaan vain paketti-/ydin-/koukkuihin; auttaja omistaa nimenomaisen firmware-rekisteröinnin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1735"/>
        <source>Creating host %1 firmware entry on %2 as &apos;%3&apos; (%4); existing host, repair, and foreign entries are untouched.</source>
        <translation>Luodaan isäntä %1 firmware merkintä %2 nimellä &apos;%3&apos; (%4); olemassa isäntä, korjaus, ja ulkomaiset merkinnät ovat koskemattomia.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1736"/>
        <source>PASS: running host default EFI entry is Boot%1 on %2.</source>
        <translation>PASS: käynnissä isäntä oletus EFI merkintä on Boot%1 %2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1737"/>
        <source>WARNING: host destination is group-writable; the copied files may be modified by the owning group: %1</source>
        <translation>VAROITUS: isäntäkohde on ryhmäkirjoitettava; kopioita voi muuttaa omistava ryhmä: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1738"/>
        <source>Host disk: %1</source>
        <translation>Käyttölevy: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1739"/>
        <source>Backed up running-host EFI loader files to %1 before the guarded reinstall.</source>
        <translation>Varmistettu käynnissä-host EFI lataaja tiedostot %1 ennen vartioitu uudelleen asentaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1740"/>
        <source>Canonical running-host EFI loader verified: %1 (%2) on %3.</source>
        <translation>Canonical juoksu-host EFI kuormain todennettu: %1 (%2) %3.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1741"/>
        <source>Host EFI read-only preflight: PASS (%1, %2, id=%3)</source>
        <translation>Isäntä EFI luku vain ennen lentoa: PASS (%1, %2, id=%3)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1742"/>
        <source>EFI repair completed; regenerating the running host GRUB fallback configuration.</source>
        <translation>EFI korjaus valmis; regenerointi käynnissä isäntä GRUB varakokoonpano.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1743"/>
        <source>Restored the running-host EFI loader files from %1.</source>
        <translation>Palautti ajo-isäntä EFI-lataajan tiedostot %1:stä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1744"/>
        <source>Host Alpine EFI-stub read-only preflight: PASS (%1, %2)</source>
        <translation>Isäntä Alpine EFI-stib luku vain ennen lentoa: PASS (%1, %2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1745"/>
        <source>Writable UEFI variables are unavailable; host maintenance will preserve files and BootOrder without firmware registration.</source>
        <translation>Kirjoitettavat UEFI-muuttujat eivät ole saatavilla; isäntähuolto säilyttää tiedostot ja BootSirder ilman firmware-rekisteröintiä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1746"/>
        <source>Host %1 entries %2 point to %3 on %4; keeping active entry Boot%5 and pruning the remaining duplicates.</source>
        <translation>Host %1 merkinnät %2 pisteen %3 %4; pitämällä aktiivinen merkintä Boot%5 ja karsimalla loput kaksoiskappaleet.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1747"/>
        <source>Host EFI System Partition: %1 (%2) %3</source>
        <translation>Isäntä EFI System Partition: %1 (%2) %3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1748"/>
        <source>Native running-host repair preflight: PASS</source>
        <translation>Native juoksu-host korjaus ennen lentoa: PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1749"/>
        <source>All requested running-host repair stages completed successfully.</source>
        <translation>Kaikki vaaditut juoksu-host korjausvaiheet suoritettu onnistuneesti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1750"/>
        <source>ROLLBACK: restored BootOrder=%1, but the firmware state differs from the pre-change capture:</source>
        <translation>ROLLBACK: palautettu Boot Order=%1, mutta firmware-tila eroaa ennen muutosta kaappaus:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1751"/>
        <source>CRITICAL: cannot detach the promoted host rollback mounts; refusing to rename Btrfs roots during recovery.</source>
        <translation>CRITICAINEN: ei voi irrottaa promoted isännän kaatumistelineitä; kieltäytyä nimeämästä Btrfs juuret uudelleen palautumisen aikana.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1752"/>
        <location filename="../src/MainWindow.cpp" line="1873"/>
        <source>CRITICAL: could not determine the preserved root subvolume ID.</source>
        <translation>KRIITINEN: ei voitu määrittää säilynyttä juuri-alavolyymiä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1753"/>
        <source>CRITICAL: could not move the failed host rollback candidate out of @; refusing to promote the preserved root.</source>
        <translation>CRIITIC: Epäonnistunutta isäntäkandidaattia ei voitu siirtää pois @:stä, koska se kieltäytyi edistämästä säilynyttä juuria.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1754"/>
        <source>CRITICAL: could not move the nested @/.snapshots child back into the preserved root.</source>
        <translation>KRIITINEN: pesittyä lasta ei voitu siirtää takaisin säilyneeseen juureen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1755"/>
        <source>CRITICAL: could not remount the Btrfs top-level filesystem read-write during host rollback recovery.</source>
        <translation>CRITICA: ei voitu asentaa uudelleen Btrfs-ylätason tiedostojärjestelmän luku-kirjoitusta isäntäkääntäjän palautuksen aikana.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1756"/>
        <location filename="../src/MainWindow.cpp" line="1876"/>
        <source>CRITICAL: could not restore the previous Btrfs default subvolume.</source>
        <translation>CRITICAINEN: edellistä Btrfs-oletusosamäärää ei voitu palauttaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1757"/>
        <source>RUNNING-HOST SNAPSHOT ROLLBACK COMPLETE</source>
        <translation>RUNNING-Host snapshot rollback valmis</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1758"/>
        <source>ROLLBACK: created entry Boot%1 is active (BootCurrent/BootNext); it is retained but no longer promoted.</source>
        <translation>ROllBACK: luotu merkintä Boot%1 on aktiivinen (BootCurrent/BootNext); se säilytetään, mutta ei enää edistä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1759"/>
        <location filename="../src/MainWindow.cpp" line="1877"/>
        <source>Creating writable rollback candidate %1</source>
        <translation>Luodaan kirjoitettava vastakanteen ehdokas %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1760"/>
        <source>Host rollback default-subvolume update failed; automatically restoring the preserved root.</source>
        <translation>Host-rollback oletus-subvolyymi päivitys epäonnistui; automaattisesti palauttaa säilynyt root.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1761"/>
        <source>Pre-rollback boot artifact fingerprints: fstab=%1 grub=%2 cmdline=%3 uki=%4</source>
        <translation>Esi-rollback-saappaan esineen sormenjäljet: fstab=%1-murska=%2 cmdline=%3 uki=%4</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1762"/>
        <source>Migrating nested @/.snapshots child subvolume into the promoted root.</source>
        <translation>Muuttaa pesitty @/.snapshots lapsen osavolyymi ylennetty juuri.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1763"/>
        <source>Host rollback candidate mount/preflight failed; automatically restoring the preserved root.</source>
        <translation>Isännän nousukandidaatin asennus/ennakko epäonnistui; säilynyt juuri palautuu automaattisesti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1764"/>
        <source>PASS: the original @ and its boot stack were restored automatically. Failed rollback candidate retained as %1.</source>
        <translation>PASS: alkuperäinen @ ja sen boot pino palautettiin automaattisesti. %1:nä pidetyssä varaehdokkaassa epäonnistuttiin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1765"/>
        <source>CRITICAL: the original @ was restored, but its boot-stack reconciliation also failed. Manual boot repair is required before reboot.</source>
        <translation>CRIITIC: alkuperäinen @ palautettiin, mutta sen boot-stack sovittelu epäonnistui myös. Manuaalinen boot korjaus vaaditaan ennen uudelleenkäynnistystä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1766"/>
        <location filename="../src/MainWindow.cpp" line="1898"/>
        <source>CRITICAL: preserved root %1 is missing; refusing to move the active rollback candidate.</source>
        <translation>CRIITIC: säilynyt juuri %1 puuttuu; kieltäytyy siirtämästä aktiivinen rollback ehdokas.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1767"/>
        <location filename="../src/MainWindow.cpp" line="1933"/>
        <source>Previous root retained as: %1</source>
        <translation>Edellinen juuri säilytetään: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1768"/>
        <source>Promoted root: @ (subvolume ID %1); the running system keeps the previous root until reboot.</source>
        <translation>Edistetty root: @ (subvolume ID %1); käynnissä järjestelmä pitää edellisen root kunnes uudelleenkäynnistys.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1769"/>
        <source>HOST ROLLBACK RECOVERY: restoring the preserved pre-rollback @.</source>
        <translation>HOST ROllBACK REKISTERÖINTI: säilyttää pre-rollback @.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1770"/>
        <source>ROLLBACK: restored BootOrder=%1 and removed created entry Boot%2; firmware state equals the pre-change capture.</source>
        <translation>ROLLBACK: palautettu Saapasjärjestys=%1 ja poistettu luotu merkintä Boot%2; firmware tila on yhtä kuin ennen muutosta kaappaus.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1771"/>
        <source>Running-host rollback selected: snapshot %1.</source>
        <translation>Running-host rockback valittu: tilannekuva %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1772"/>
        <source>Rollback source remains unchanged; the running host keeps the current root until reboot.</source>
        <translation>Rollback-lähde pysyy muuttumattomana; käynnissä oleva isäntä pitää nykyisen juuren käynnistymiseen asti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1773"/>
        <source>Selected rollback target: %1</source>
        <translation>Valittu varakohde: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1774"/>
        <source>Host rollback candidate failed post-switch validation/boot reconciliation; automatically restoring the preserved root.</source>
        <translation>Isäntärock-ehdokas epäonnistui kytkimen jälkeisessä validoinnissa/saappaan täsmäyttämisessä; säilynyt juuri palautuu automaattisesti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1775"/>
        <source>Host root component: %1 (%2)</source>
        <translation>Isännän juurikomponentti: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1776"/>
        <source>Host root mount: / (subvolume=%1)</source>
        <translation>Isännän juuriliitos: / (osatilavuus=%1)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1777"/>
        <source>FAIL: Running-host shell command was cancelled at an interactive prompt.</source>
        <translation>Suoritus-ohjain komento peruttiin interaktiivisella käskyllä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1778"/>
        <source>Running-host shell exit code: %1</source>
        <translation>Running-host kuori exit koodi: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1779"/>
        <source>FAIL: Running-host shell command (exit code %1)</source>
        <translation>FAIL: Running-host-shell-komento (poistumiskoodi %1)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1780"/>
        <source>PASS: Running-host shell command</source>
        <translation>PASS: Suoritus-ohjain komento</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1781"/>
        <source>Host snapshot inventory skipped: %1 is not Btrfs.</source>
        <translation>Isäntäkuvan inventaario ohitettu: %1 ei ole Btrfs.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1782"/>
        <source>Host TUXEDO UKI firmware entry already present: Boot%1 on %2.</source>
        <translation>Isäntä TUXEDO UKI firmware-merkintä on jo läsnä: Boot%1 on %2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1783"/>
        <source>Host TUXEDO UKI file is present on %1, but firmware variables are not writable; host registration was not changed.</source>
        <translation>Isäntä TUXEDO UKI tiedosto on läsnä %1, mutta firmware muuttujat eivät ole kirjoitettavia; isäntä rekisteröinti ei muuttunut.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1784"/>
        <source>Host TUXEDO UKI read-only preflight: PASS (%1, %2)</source>
        <translation>Isäntä TUXEDO UKI luku vain ennen lentoa: PASS (%1, %2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1785"/>
        <source>PASS: host TUXEDO UKI firmware entry restored as Boot%1 on %2.</source>
        <translation>PASS: isäntä TUXEDO UKI firmware merkintä palautettu Boot%1 %2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1786"/>
        <source>Restoring missing host TUXEDO UKI firmware entry on %1 as &apos;%2&apos;; existing host, repair, and foreign entries are untouched.</source>
        <translation>Palauttaa puuttuvat isäntä TUXEDO UKI firmware merkintä %1 &apos;%2&apos;; olemassa isäntä, korjaus, ja ulkomaiset merkinnät ovat koskemattomia.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1787"/>
        <source>Running-host validation complete; no host files were changed.</source>
        <translation>Running-host validointi valmis; mitään isäntätiedostoja ei muutettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1788"/>
        <source>Running-host validation summary</source>
        <translation>Running-host validation -tiivistelmä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1789"/>
        <source>KNOWN ISSUE: %1 is missing; creating it instead of attempting an update.</source>
        <translation>TIEDON NUMERO: %1 puuttuu; luoda sen sijaan yrittää päivittää.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1790"/>
        <source>PASS: initramfs %1 verified by mkinitfs -l build-input listing</source>
        <translation>PASS: initramfs %1 todentanut mkinitfs -l sisäänrakennettu listaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1791"/>
        <source>SIMULATE/PREFLIGHT: initramfs generation for %1 installed kernel(s)</source>
        <translation>SIMULATE/PREFLIGHT: initramfs-tuotanto asennetuille %1-ytimille</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1792"/>
        <source>PASS: trial initramfs build for %1</source>
        <translation>PASS: kokeilu initramfs rakentaa %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1793"/>
        <source>PASS: initramfs verified for %1</source>
        <translation>PASS: initramfs todennettu %1:lle</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1794"/>
        <source>PASS: initramfs %1 verified by zcat/cpio listing</source>
        <translation>PASS: initramfs %1 todennettu zcat/cpio listaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1795"/>
        <source>KNOWN ISSUE: initrd.img-%1 is missing; creating it instead of attempting an update.</source>
        <translation>TIEDON NUMERO: initrd.img-%1 puuttuu; luoda sen sijaan yrittää päivittää.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1796"/>
        <source>Kernel cleanup plan: removing %1 versioned kernel package(s): %2</source>
        <translation>Ytimen puhdistussuunnitelma: poistamalla %1 versioitu ytimen paketti(s): %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1797"/>
        <source>Installing the missing headers for the newest kernel: %1</source>
        <translation>Puuttuvien otsikoiden asentaminen uusimpaan ytimeen: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1798"/>
        <source>Installing the repository-latest kernel image: %1</source>
        <translation>Varaston viimeisimmän ytimen kuvan asennus: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1799"/>
        <source>Orphaned kernel cleanup plan: removing the leftover on-disk components of kernel %1.</source>
        <translation>Orvon ytimen puhdistussuunnitelma: poistamalla jäljellä olevia osia ytimen %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1800"/>
        <source>FAIL: a protected kernel package was removed by the transaction; restoring protected package(s): %1</source>
        <translation>FAIL: Suojattu ytimen paketti poistettiin tapahtuma; palauttaa suojattu paketti(s): %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1801"/>
        <source>Kernel reinstall plan: reinstalling %1 incomplete kernel package(s): %2</source>
        <translation>Ytimen asentaminen uudelleen: %1:n epätäydellisen ytimen paketti [s] asentaminen uudelleen: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1802"/>
        <source>FAIL: kernel removal exited %1; cleaning leftover on-disk components of the removed kernels</source>
        <translation>FAIL: ytimen poisto poistettu %1; puhdistus jäännökset levyllä osat poistettu ytimiä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1803"/>
        <source>Cleaning leftover on-disk components of removed kernel %1 (zombie cleanup)</source>
        <translation>Poistetun ytimen %1:n (zombien puhdistus) jäänteiden puhdistus levyllä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1804"/>
        <source>%1 lives on the root filesystem; no separate mount required</source>
        <translation>%1 elää juuritiedostojärjestelmässä; erillistä asennusta ei tarvita</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1805"/>
        <source>PASS: lsinitcpio verified %1 Arch initramfs image(s)</source>
        <translation>PASS: lsinitcpio todennettu %1 Arch initramfs kuvat</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1806"/>
        <source>PASS: lsinitramfs verified /boot/initrd.img-%1</source>
        <translation>PASS: lsinitramfs todennettu /boot/initrd.img-%1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1807"/>
        <source>PASS: lsinitrd verified %1</source>
        <translation>PASS: Lsinitrd todennettu %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1808"/>
        <source>LUKS target is already unlocked by existing mapper: %1</source>
        <translation>LUKS-kohde on jo avattu olemassa olevan mapper: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1809"/>
        <source>Mapper name: %1</source>
        <translation>Mapper-nimi: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1810"/>
        <source>LUKS unlock complete. The mapper remains open for this recovery session.</source>
        <translation>LUKS-lukko valmis. Mapper on avoinna toipumisistuntoa varten.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1811"/>
        <source>Unlocking LUKS target %1</source>
        <translation>Avataan LUKS-tavoite %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1812"/>
        <source>Mandatory safety preflight: PASS</source>
        <translation>Pakollinen turvallisuus ennen lentoa: PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1813"/>
        <source>Mapper compatibility: %1 already resolves to %2</source>
        <translation>Mapper yhteensopivuus: %1 jo ratkaisee %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1814"/>
        <source>Mapper compatibility: temporary %1 -&gt; %2</source>
        <translation>Mapper yhteensopivuus: väliaikainen %1 -&gt; %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1815"/>
        <source>SIMULATE/PREFLIGHT: mkinitcpio trial builds for %1 installed kernel(s)</source>
        <translation>SIMULATE/PREFLIGHT: mkinitcpio kokeilu rakentaa %1 asennettujen ytimet(s)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1816"/>
        <source>PASS: trial mkinitcpio build for %1</source>
        <translation>PASS: kokeilu mkinitcpio rakentaa %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1817"/>
        <source>SIMULATE/PREFLIGHT: mkinitfs trial builds for %1 installed kernel(s)</source>
        <translation>SIMULATE/PREFLIGHT: mkinitfs-kokeilu rakentaa %1-asennettuun ytimeen(s)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1818"/>
        <source>PASS: trial mkinitfs build for %1</source>
        <translation>PASS: tutkimus mkinitfs rakentaa %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1819"/>
        <source>Mounted target ESP discovered by GPT type: %1 at /boot/efi (%2)</source>
        <translation>Asennettu kohde ESP löydetty GPT-tyypillä: %1 at /boot/efi (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1820"/>
        <source>Mounted target %1 from %2 (%3)</source>
        <translation>Asennettu kohde %1 %2:stä (%3)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1821"/>
        <source>Mounting target %1 from %2 (%3)</source>
        <translation>Asennuskohde %1 %2:stä (%3)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1822"/>
        <source>Mounting target Btrfs fstab subvolumes (%1)</source>
        <translation>Asennuskohteet Btrfs fstabin alatilavuudet (%1)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1823"/>
        <source>Mounting target %1 from %2</source>
        <translation>Asennuskohde %1 %2:stä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1824"/>
        <source>No TUXEDO UKI builder detected; retaining the distribution&apos;s existing EFI layout.</source>
        <translation>TUXEDO UKI -rakentajaa ei ole havaittu; jakelun nykyinen EFI-asettelu säilytetään.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1825"/>
        <source>FAIL: ntfsfix could not fully repair %1 (exit code %2); run Windows chkdsk /f for a real NTFS repair.</source>
        <translation>FAIL: ntffix ei pystynyt korjaamaan %1:ää (poistumiskoodi %2); suorita Windows chkdsk /f todellista NTFS korjausta varten.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1826"/>
        <source>NOTE: ntfsfix only clears the NTFS dirty state; Windows chkdsk /f is required for a real NTFS repair.</source>
        <translation>HUOMAUTUS: ntffix puhdistaa vain NTFS:n likaisen tilan; Windows chkdsk /f tarvitaan todelliseen NTFS-korjaukseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1827"/>
        <source>Orphaned kernel build/support cleanup plan: removing %1 package(s): %2</source>
        <translation>Orvon ytimen rakenne/tuki puhdistussuunnitelma: poistamalla %1-paketti(s): %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1828"/>
        <source>FAIL: orphaned kernel build/support removal exited %1</source>
        <translation>FAIL: orvon ytimen rakenne / tuki poisto poistettu %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1829"/>
        <source>Orphaned kernel headers cleanup plan: removing %1 header package(s): %2</source>
        <translation>Orvon ytimen otsakkeet puhdistus suunnitelma: poistamalla %1 otsakepaketti(s): %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1830"/>
        <source>FAIL: orphaned kernel headers removal exited %1; cleaning leftover header directories</source>
        <translation>FAIL: orvon ytimen otsakkeiden poisto poistunut %1; siivous ylijäämän otsakehakemistot</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1831"/>
        <source>Ownership validation: %1:%2 maps to %3:%4 on both sides; preserving numeric ownership</source>
        <translation>Omistajuuden validointi: %1:%2 kartat %3:%4 molemmin puolin; numeerisen omistusoikeuden säilyttäminen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1832"/>
        <source>Ownership validation: source %1:%2 (%3:%4) does not map identically on the destination side; using destination owner %5:%6</source>
        <translation>Omistajan validointi: lähde %1:%2 (%3:%4) ei kartalla identtisesti kohdepuolella; käyttäen kohdeomistajaa %5:%6</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1833"/>
        <source>Ownership policy: %1</source>
        <translation>Omistusoikeus: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1834"/>
        <source>Package backend %1 did not report a change status; treating the stage as changed.</source>
        <translation>Paketin taustaosa %1 ei ilmoittanut muutostilaa; käsittely vaiheessa muuttunut.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1835"/>
        <source>SKIP: package backend %1 is not runnable for stage &apos;%2&apos;: %3</source>
        <translation>SKIP: Paketin taustaosa %1 ei ole käytettävissä vaiheessa &quot;%2&quot;: %3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1836"/>
        <source>Package backend %1 change status: %2</source>
        <translation>Paketin taustaosan %1 muutostila: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1837"/>
        <source>Package stage &apos;%1&apos;: running %2 runnable backend(s): %3</source>
        <translation>Pakettivaihe &apos;%1&apos;: käynnissä %2-juoksutettava taustaosa [s]: %3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1838"/>
        <source>PackageKit daemon is running; it is only a conflict while it holds package-manager locks or spawns apt/dpkg.</source>
        <translation>PakettiKit Daemon on käynnissä; se on vain ristiriita, kun sillä on paketti-manageri lukot tai spawns apt/dpkg.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1839"/>
        <source>Arch pacman transaction completed successfully despite mirror fallback.</source>
        <translation>Arch pacman -tapahtuma suoritettu onnistuneesti huolimatta peilivarauksesta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1840"/>
        <source>WARN: pacman encountered %1 recoverable mirror retrieval failure(s).</source>
        <translation>WARN: pacman havaitsi %1:n palautuvan peilin hakuvirheen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1841"/>
        <source>PASS: Arch pacman transaction preflight resolved without removals.</source>
        <translation>PASS: Arch pacman tapahtuma ennen lentoa ratkaistu ilman poistoja.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1842"/>
        <source>REFUSED: pacman preflight proposes %1 packages (safety limit: 1000).</source>
        <translation>EPÄÄMINEN: pacman:ssä ennen lentoa ehdotetaan %1-paketteja (turvallisuusraja 1000).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1843"/>
        <source>REFUSED: pacman preflight reported repository, download or transaction integrity errors.</source>
        <translation>EPÄÄMINEN: pacman ennen lentoa raportoitu arkisto, lataus tai tapahtuma eheys virheet.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1844"/>
        <source>REFUSED: pacman preflight proposes removals or unresolved dependencies.</source>
        <translation>EPÄÄMINEN: pacman ennen lentoa ehdottaa poistoja tai ratkaisemattomia riippuvuuksia.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1845"/>
        <source>Arch pacman transaction sandbox prepared under %1; the target package database will not be used for preflight.</source>
        <translation>Arch pacman tapahtumahiekkalaatikko valmistettu %1; kohdepaketti tietokantaa ei käytetä ennen lentoa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1846"/>
        <source>SIMULATE/PREFLIGHT: full Arch pacman transaction (sandboxed database/cache; no target packages will be changed)</source>
        <translation>SIMULATE/PREFLIGHT: koko Arch pacman-tapahtuma (Sandboxed database/cache; kohdepaketteja ei muuteta)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1847"/>
        <source>PASS: dnf5 metadata cache refreshed.</source>
        <translation>PASS: dnf5-metadatavälimuisti päivitetty.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1848"/>
        <source>PASS: %1</source>
        <translation>PASS: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1849"/>
        <source>PASS: %1 completed through one full pacman transaction.</source>
        <translation>PASS: %1 suoritettu yhdellä täydellä pacman-tapahtumalla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1850"/>
        <source>PASS: Refresh package metadata</source>
        <translation>PASS: Päivitä paketin metatiedot</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1851"/>
        <source>PASS: Refresh package metadata (package lists byte-identical; no repository index was fetched)</source>
        <translation>PASS: Päivitä pakettien metatiedot (pakettiluettelot byte-identiteetti; arkistoindeksiä ei noudettu)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1852"/>
        <source>Host package-manager concurrency gate: PASS</source>
        <translation>Isäntäpaketti-hallintavaluutta portti: PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1853"/>
        <source>Post-upgrade dpkg audit:</source>
        <translation>Korkea-asteen jälkeinen dpkg-tarkastus:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1854"/>
        <source>PREVIEW COMPLETE — no files were changed.</source>
        <translation>ENNAKKO TÄYDELLINEN</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1855"/>
        <source>PREVIEW: target destination directory would be created: %1</source>
        <translation>ENNAKKO: kohde kohdehakemisto luodaan: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1856"/>
        <source>Protected host check: PASS</source>
        <translation>Suojattu isäntätarkistus: PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1857"/>
        <source>Scheduling running-host reboot through the system reboot command.</source>
        <translation>Käynnistetään järjestelmä uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1858"/>
        <source>Scheduling running-host reboot through OpenRC.</source>
        <translation>Suunnitelmia käynnissä-host käynnistää uudelleen OpenRC.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1859"/>
        <source>Scheduling running-host reboot through systemd.</source>
        <translation>Aikataulun mukaan isännät käynnistyvät uudelleen järjestelmällisesti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1860"/>
        <source>WARNING: refusing an unsafe target Btrfs mountpoint (not absolute): %1</source>
        <translation>VAROITUS: vaarallisen kohteen Btrfs asennuspiste (ei absoluuttinen): %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1861"/>
        <source>WARNING: refusing an unsafe target Btrfs subvolume mount path: %1</source>
        <translation>VAROITUS: vaarallisen kohteen Btrfs alavolyymi asennus polku: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1862"/>
        <source>WARNING: refusing an unsafe target %1 mount path: %2</source>
        <translation>VAROITUS: epäturvallisen kohteen %1 asennuspolku: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1863"/>
        <source>Remounting confirmed target filesystem read-write for file copy</source>
        <translation>Luodaan uudelleen vahvistettu kohdetiedostojärjestelmän lukukirjoitus tiedostokopiota varten</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1864"/>
        <source>Remounting confirmed target root read-write</source>
        <translation>Palautetaan vahvistettu kohde juurilukukirjoitus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1865"/>
        <source>Remounting target data filesystem %1 read-write</source>
        <translation>Kohteen datatiedostojärjestelmän muuttaminen %1 lukukirjoitus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1866"/>
        <source>Remounting target %1 read-write</source>
        <translation>Korjataan kohdetta %1 lukukirjoitus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1867"/>
        <source>All requested repair stages completed successfully.</source>
        <translation>Kaikki vaaditut korjausvaiheet valmistuivat onnistuneesti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1868"/>
        <source>Rollback boot-stack reconciliation: using simulation-first adaptive component workflows.</source>
        <translation>Rollback boot-stack täsmäytys: käyttämällä simulointi-ensimmäinen adaptiivinen komponentti työnkulkuja.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1869"/>
        <source>Rollback candidate default-subvolume update failed; automatically restoring the preserved root.</source>
        <translation>Rollback ehdokas oletus-subvolyymi päivitys epäonnistui; automaattisesti palauttaa säilynyt juuri.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1870"/>
        <source>Rollback candidate mount/preflight failed; automatically restoring the preserved root.</source>
        <translation>Rollback-ehdokkaan asennus/valmistelu epäonnistui; säilyneen juuren palauttaminen automaattisesti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1871"/>
        <source>Rollback candidate failed post-switch validation/boot reconciliation; automatically restoring the preserved root.</source>
        <translation>Rollback-ehdokas epäonnistui kytkimen jälkeisessä validoinnissa/saappaan täsmäyttämisessä; säilynyt juuri palautuu automaattisesti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1872"/>
        <source>CRITICAL: cannot detach promoted rollback mounts; refusing to rename Btrfs roots during recovery.</source>
        <translation>CRITICA: ei voi irrottaa promotoituja kaatumistelineitä, kieltäytyä nimeämästä Btrfs-juuria uudelleen palautumisen aikana.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1874"/>
        <source>CRITICAL: could not move failed rollback candidate out of @; refusing to promote the preserved root.</source>
        <translation>CRITICAINEN: ei voinut siirtää epäonnistunut rockback ehdokas pois @; kieltäytyä edistää säilynyt juuri.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1875"/>
        <source>CRITICAL: could not remount the Btrfs top-level filesystem read-write during rollback recovery.</source>
        <translation>CRITICA: Btrfs-ylätason tiedostojärjestelmän luku-kirjoitusta ei voitu asentaa uudelleen palautuksen aikana.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1878"/>
        <source>CRITICAL ROLLBACK FAILURE: could not restore original @ after default-subvolume failure. Do not reboot.</source>
        <translation>KRIITINEN ROLLBACK FILURE: ei voitu palauttaa alkuperäistä @ jälkeen oletus-subvolyymi vika. Älä käynnistä uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1879"/>
        <source>CRITICAL ROLLBACK FAILURE: automatic restoration was incomplete. Do not reboot until Btrfs/boot state is inspected manually.</source>
        <translation>KRIITINEN PYSÄKÖINTI: Automaattinen ennallistaminen oli kesken. Älä käynnistä uudelleen ennen kuin Btrfs/boot-tila on tarkastettu käsin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1880"/>
        <source>CRITICAL ROLLBACK FAILURE: could not restore original @ after candidate mount failure. Do not reboot.</source>
        <translation>CRIITIC ROLLBACK FILURE: ei voinut palauttaa alkuperäistä @ jälkeen ehdokas asentaa vikaa. Älä käynnistä uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1881"/>
        <source>Detaching target mounts before atomic @ name switch.</source>
        <translation>Irrotetaan kohteen liitokset ennen atomi@ nimikytkintä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1882"/>
        <source>ROLLBACK FAILED SAFELY: original @ restored; failed candidate retained as %1.</source>
        <translation>ROLLBACK PYSYVÄSTI: alkuperäinen @ palautettu; epäonnistunut ehdokas valittu %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1883"/>
        <source>ROLLBACK FAILED SAFELY: original @ restored after default-subvolume failure; failed candidate retained as %1.</source>
        <translation>ROLLBACK HALLINNOLLISESTI: alkuperäinen @ palautettu oletus-subvolyymi vika; epäonnistui ehdokas säilyttää %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1884"/>
        <source>ROLLBACK FAILED SAFELY: original @ restored after candidate mount failure; failed candidate retained as %1.</source>
        <translation>ROLLBACK HALLINNOLLISESTI: alkuperäinen @ palautettu jälkeen ehdokas asentaa vika; epäonnistui ehdokas säilyttää %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1885"/>
        <source>Rollback kernel %1 is missing initramfs before rebuild; update-initramfs will be asked to regenerate it.</source>
        <translation>Rollback-ytimen %1 puuttuu initramfs ennen jälleenrakentamista; update-initramfs pyydetään uudistamaan se.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1886"/>
        <source>Rollback kernel %1 has no matching modules directory.</source>
        <translation>Rollback-ytimen %1:ssä ei ole vastaavaa moduulihakemistoa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1887"/>
        <source>Rollback kernel pair before rebuild: %1</source>
        <translation>Rollback-ytimen pari ennen rakentamista: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1888"/>
        <source>Rollback kernel pair before rebuild: %1 (mkinitcpio flavor naming)</source>
        <translation>Rollback-ytimen pari ennen jälleenrakentamista: %1 (mkinitcpio makunimitys)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1889"/>
        <source>PASS: original @ and its boot stack were restored automatically. Failed rollback candidate retained as %1.</source>
        <translation>PASS: alkuperäinen @ ja sen boot pino palautettiin automaattisesti. %1:nä pidetyssä varaehdokkaassa epäonnistuttiin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1890"/>
        <source>CRITICAL: original @ was restored, but its boot-stack reconciliation also failed. Manual boot repair is required before reboot.</source>
        <translation>CRIITIC: alkuperäinen @ palautettiin, mutta sen boot-stack sovittelu epäonnistui myös. Manuaalinen boot korjaus vaaditaan ennen uudelleenkäynnistystä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1891"/>
        <source>Rollback preflight: snapshot root fstab is compatible with promoted @.</source>
        <translation>Rollback ennen lentoa: tilannekuva root fstab on yhteensopiva promoted @.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1892"/>
        <source>Rollback preflight: snapshot fstab has no root (/) entry.</source>
        <translation>Rollback ennen lentoa: kuvassa fstab ei ole juuria (/) merkintä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1893"/>
        <source>Rollback preflight: snapshot root fstab type is &apos;%1&apos;, not btrfs.</source>
        <translation>Rollback ennen lentoa: tilannekuva root fstab tyyppi on &apos;%1&apos;, ei btrfs.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1894"/>
        <source>Rollback preflight: snapshot fstab expects root subvolume &apos;%1&apos;, but transactional rollback promotes the selected snapshot to @.</source>
        <translation>Rollback ennen lentoa: tilannekuva fstab odottaa root subvolume &apos;%1&apos;, mutta transaktion palautus edistää valitun tilannekuvan @.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1895"/>
        <source>Rollback preflight: snapshot / fstab source cannot be resolved: %1</source>
        <translation>Perääntyminen ennen lentoa: tilannekuvaa / fstab-lähdettä ei voida ratkaista: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1896"/>
        <source>Rollback preflight: snapshot / fstab root source has an unsupported format: %1</source>
        <translation>Rollback ennen lentoa: kuva / ftab juurilähde on tueton muoto: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1897"/>
        <source>Rollback preflight: snapshot / fstab source resolves to %1 rather than selected root %2.</source>
        <translation>Rollback ennen lentoa: kuva / fstab-lähde ratkaisee %1:n eikä valitun juuri %2:n.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1899"/>
        <source>Preserved root name: %1</source>
        <translation>Säilötty juurinimi: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1900"/>
        <source>PASS: promoted rollback root and boot stack validated.</source>
        <translation>PASS: edistää rockback root ja boot pino validoitu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1901"/>
        <source>ROLLBACK RECOVERY: restoring preserved pre-rollback @.</source>
        <translation>ROLLBACK ELVYTYS: säilyneen pre-rollback @.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1902"/>
        <source>Transactional rollback selected: snapshot %1 (%2).</source>
        <translation>Transaktiokäännös valittu: valokuva %1 (%2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1903"/>
        <source>Rollback source snapshot will remain unchanged.</source>
        <translation>Rollback-lähdekuva pysyy ennallaan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1904"/>
        <source>Root component: %1 (%2)</source>
        <translation>Juurikomponentti: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1905"/>
        <source>Root component fallback: selected component %1 (%2) lacks /etc/os-release; resolved %3 (%4) from %5.</source>
        <translation>Juurikomponentin varaosa: valitusta %1:stä (%2) puuttuu /etc/os-release; ratkaistu %3 (%4) %5:stä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1906"/>
        <source>RPM missing-file repair: reinstalling %1 package(s) with missing files: %2</source>
        <translation>RPM puuttuu-file korjaus: asentaminen %1 paketti(s) puuttuvat tiedostot: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1907"/>
        <source>RPM missing-file detection: no missing package files; recording read-only dnf5 dependency-check evidence.</source>
        <translation>RPM puuttuu-tiedoston havaitseminen: ei puuttuvia pakettitiedostoja; tallennetaan vain lukea dnf5 huoltotodisteita.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1908"/>
        <source>rpm preflight version: %1</source>
        <translation>Rpm esilentoversio: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1909"/>
        <source>Running-host identity and boot-mount check: PASS</source>
        <translation>Running-host identiteetti ja boot-mount tarkistaa: PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1910"/>
        <source>Security: host-to-repair copies keep -aHAX (modes, ownership and xattrs are copied from the trusted host source).</source>
        <translation>Turvallisuus: isäntä-korjaus kopiot pitää -aHAX (tilat, omistajuus ja xatters kopioidaan luotettava isäntä lähde).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1911"/>
        <source>Security: setuid/setgid bits and file capabilities are removed on repair-to-host copies.</source>
        <translation>Turvallisuus: setuid/setgid-bitit ja tiedoston ominaisuudet poistetaan korjauksesta isännöiksi -kopioilla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1912"/>
        <source>Selected EFI System Partition: %1 (%2) mounted at %3</source>
        <translation>Valittu EFI System Partition: %1 (%2) asennettuna %3:lle</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1913"/>
        <source>SELinux enforcement: %1</source>
        <translation>Selinuxin valvonta: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1914"/>
        <source>SELinux status: %1</source>
        <translation>SELinux-status: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1915"/>
        <source>SELinux status: sestatus/getenforce are not installed in the target; recorded as unavailable.</source>
        <translation>SELinux status: sestatus/getenforce ei ole asennettu kohteeseen; tallennettu ei saatavilla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1916"/>
        <source>========================================</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1917"/>
        <source>Session log was NOT appended into the target: unsafe target log path.</source>
        <translation>Istunnon lokia EI ole liitetty kohteeseen: vaarallinen kohdelokin polku.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1918"/>
        <source>SIMULATE: apk %1 --simulate (no packages will be changed)</source>
        <translation>SIMULATE: apk %1 -- simuloidaan (paketteja ei muuteta)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1919"/>
        <source>SIMULATE: apt-get %1 (no packages will be changed)</source>
        <translation>SIMULATE: apt-get %1 (paketteja ei muuteta)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1920"/>
        <source>SIMULATE CORRECTION: %1</source>
        <translation>YKSINKERTAINEN KORJAUS: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1921"/>
        <source>SIMULATE: dnf5 %1 --assumeno (no packages will be changed)</source>
        <translation>SIMULATE: dnf5 %1 --assumeno (paketteja ei muuteta)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1922"/>
        <source>SIMULATE: apt-get --fix-broken install (no packages will be changed)</source>
        <translation>SIMULATE: apt-get --fix-broken install (paketteja ei muuteta)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1923"/>
        <source>Simulation exit code for &apos;%1&apos;: %2</source>
        <translation>&quot;%1&quot;:n simulointilähdön koodi: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1924"/>
        <source>WARNING: skipping unresolved Btrfs fstab entry %1 -&gt; %2</source>
        <translation>VAROITUS: ohitetaan ratkaisematon Btrfs fstab merkintä %1 -&gt; %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1925"/>
        <source>WARNING: skipping unresolved data fstab entry %1 -&gt; %2</source>
        <translation>VAROITUS: ratkaisemattomien tietojen ohittaminen fstab-tietue %1 -&gt; %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1926"/>
        <source>Target has snapper apt hooks; guarding the chroot shell against snapshots (temporary, read-only)</source>
        <translation>Kohde on napsija apt koukkuja; vartioi Chroot kuori vastaan valokuvia (tilapäinen, luku-vain)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1927"/>
        <source>WARNING: could not bind the read-only snapper guard over %1; the chroot shell continues without the snapshot kill-switch.</source>
        <translation>VAROITUS: %1:n päällä ei voitu sitoa vain lukunapsijasuojaa; ruskokuori jatkuu ilman kuvan tappokytkintä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1928"/>
        <source>Active writable root: @ (subvolume ID %1)</source>
        <translation>Aktiivinen kirjoitettava juuri: @ (subvolume ID %1)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1929"/>
        <source>Initramfs/UKI/GRUB reconciliation: PASS</source>
        <translation>Initramfs/UKI/GRUB täsmäytys: PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1930"/>
        <source>Btrfs default subvolume now points to the promoted @.</source>
        <translation>Btrfs:n oletusalivolyymi osoittaa nyt ylennettyyn @.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1931"/>
        <source>Snapshot inventory complete: %1 snapshot(s) found.</source>
        <translation>Snapshot-inventaario valmis: %1-kuvake [s] löytyi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1932"/>
        <source>Snapshot inventory skipped: %1 is not Btrfs.</source>
        <translation>Snapshot inventaario ohitettu: %1 ei ole Btrfs.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1934"/>
        <source>SNAPSHOT ROLLBACK COMPLETE</source>
        <translation>PALKKAUS PELASTETTU</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1935"/>
        <source>Selected snapshot: %1</source>
        <translation>Valittu kuvakuva: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1936"/>
        <source>Source snapshot retained unchanged.</source>
        <translation>Lähdekuva säilyi ennallaan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1937"/>
        <source>Sources: %1</source>
        <translation>Lähteet: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1938"/>
        <source>Removed leaked Boot Bitch session mount at %1 (source=%2)</source>
        <translation>Poistettu vuotanut Boot Bitch-istuinasennus %1:ssä (lähde=%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1939"/>
        <source>Target /dev: private writable tmpfs populated from the recovery host with a private devpts at /dev/pts; the running host /dev and its ptys are not modified or leaked.</source>
        <translation>Kohde /dev: Yksityiset kirjoitettavat tmpf:t, jotka on asutettu palautusisännästä yksityisellä devptillä osoitteessa /dev/pts; käynnissä oleva isäntä /dev ja sen ptyt eivät muutu tai vuoda.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1940"/>
        <source>Target disk: %1</source>
        <translation>Kohdelevy: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1941"/>
        <source>Target root mount: %1 (subvolume=%2)</source>
        <translation>Kohde juuriliitos: %1 (subvolyymi=%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1942"/>
        <source>TRY exit code: %1 (%2)</source>
        <translation>Kokeile poistumiskoodi: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1943"/>
        <source>TRY: %1</source>
        <translation>Kokeile: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1944"/>
        <source>PASS: TUXEDO UKI root/LUKS/subvolume binding verified.</source>
        <translation>PASS: TUXEDO UKI root/LUKS/subvolyymisitoutuminen todennettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1945"/>
        <source>Captured complete firmware entry state before UKI rebuild: %1</source>
        <translation>Kaapattu täydellinen firmware sisääntulotila ennen UKI uudelleen: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1946"/>
        <source>TUXEDO UKI cmdline: %1</source>
        <translation>TUXEDO UKI cmdline: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1947"/>
        <source>TUXEDO UKI embedded kernel: %1</source>
        <translation>TUXEDO UKI sulautettu ydin: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1948"/>
        <source>PASS: existing TUXEDO UKI validated for %1 on selected ESP %2; no replacement image was needed.</source>
        <translation>PASS: nykyinen TUXEDO UKI validoitu %1 valitulle ESP %2:lle; korvaavaa kuvaa ei tarvittu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1949"/>
        <source>KNOWN ISSUE: UKI kernel is stale; vendor UKI rebuild will correct it.</source>
        <translation>TIEDON NUMERO: UKI ydin on kulunut; toimittaja UKI jälleenrakentaminen korjaa sen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1950"/>
        <source>TUXEDO UKI layout detected; simulating/prereflighting the vendor boot path before rebuild.</source>
        <translation>TUXEDO UKI:n ulkoasu havaittu; simuloidaan/esitellään myyjän käynnistyspolkua ennen jälleenrakentamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1951"/>
        <source>KNOWN ISSUE: newest TUXEDO kernel %1 has no matching initramfs; rebuilding initramfs before UKI.</source>
        <translation>TIEDON NUMERO: uusin TUXEDO ydin %1 ei ole yhteensopiva initramfs; jälleenrakentaminen initramfs ennen UKI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1952"/>
        <source>Writable UEFI variables/efibootmgr are unavailable; UKI file rebuild will proceed without BootOrder restoration.</source>
        <translation>Kirjoitettavat UEFI-muuttujat / efibootmgr eivät ole käytettävissä; UKI-tiedoston uudelleenrakentaminen jatkuu ilman BootSerderin palauttamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1953"/>
        <source>WARNING: objcopy could not inspect the rebuilt UKI .uname section.</source>
        <translation>VAROITUS: objcopy ei voinut tarkastaa uudelleen rakennettu UKI .uname osa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1954"/>
        <source>UKI preflight: current embedded kernel=%1; target newest kernel=%2</source>
        <translation>UKI ennen lentoa: nykyinen upotettu ydin=%1; kohde uusin ydin=%2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1955"/>
        <source>PASS: TUXEDO UKI preflight prerequisites are satisfied.</source>
        <translation>PASS: TUXEDO UKI ennen lentoa edellytykset täyttyvät.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1956"/>
        <source>PASS: TUXEDO UKI rebuilt for %1 on selected ESP %2</source>
        <translation>PASS: TUXEDO UKI uudelleenrakennettu %1 valitulle ESP %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1957"/>
        <source>TUXEDO UKI repair read-only preflight: PASS (%1, %2)</source>
        <translation>TUXEDO UKI korjaus luku vain ennen lentoa: PASS (%1, %2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1958"/>
        <source>PASS: vendor UKI command produced a changed TUX.EFI image.</source>
        <translation>PASS: Myyjä UKI komento tuotti muuttunut TUX.EFI kuva.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1959"/>
        <source>WARNING: vendor UKI command completed without changing TUX.EFI; the existing image will be validated against the selected target before this repair is reported successful.</source>
        <translation>VAROITUS: Myyjä UKI-komento valmistunut ilman muutoksia TUX.EFI; nykyinen kuva validoidaan valitun kohteen mukaan ennen kuin tämä korjaus ilmoitetaan onnistuneeksi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1960"/>
        <source>/boot: %1</source>
        <translation>/boot: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1961"/>
        <source>/boot/efi: %1</source>
        <translation>/boot/efi: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1962"/>
        <source>Validation complete; no target files were changed.</source>
        <translation>Validointi valmis; kohdetiedostoja ei muutettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1963"/>
        <source>/etc/crypttab: %1</source>
        <translation>/etc/cryptab: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1964"/>
        <source>ESP mount candidate: %1</source>
        <translation>ESP-asennuskandidaatti: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1965"/>
        <source>/etc/fstab: %1</source>
        <translation>/etc/fstab: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1966"/>
        <source>GRUB config: %1</source>
        <translation>GRUB config: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1967"/>
        <source>Supported modifying backend: %1</source>
        <translation>Tuettu muokkaustaustaosa: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1968"/>
        <source>OS: %1</source>
        <translation>OS: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1969"/>
        <source>Root: %1</source>
        <translation>Juuri: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1970"/>
        <source>Root filesystem: %1</source>
        <translation>Juuritiedostojärjestelmä: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1971"/>
        <source>Root fs: %1</source>
        <translation>Juurifs: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1972"/>
        <source>Repair root mount source: %1</source>
        <translation>Korjaa juuriasennuslähde: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1973"/>
        <source>Root subvolume: %1</source>
        <translation>Juuriosamäärä: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1974"/>
        <source>Validation summary</source>
        <translation>Validoinnin yhteenveto</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1975"/>
        <source>Vendor command completed successfully: %1</source>
        <translation>Valmistajakomento suoritettu onnistuneesti: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1976"/>
        <source>Repair-to-host capability scan: xattr inspection tooling is unavailable; the rsync security.capability filter was applied and trusted.</source>
        <translation>Korjaus-to-host kyky skannaus: xattr tarkastus työkalut ei ole saatavilla; rsync turvallisuus. käyttökykysuodatinta käytettiin ja luotettiin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1977"/>
        <source>PASS: zpool scrub clean; clearing pool error counters with zpool clear.</source>
        <translation>PASS: zpool pesu puhdas; clearing allas virhelaskurit zpool selkeä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1984"/>
        <source>Alpine apk audit --system (missing-file detection)</source>
        <translation>Alpine apk -auditointijärjestelmä (puuttuvan tiedoston havaitseminen)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1985"/>
        <source>Alpine apk info --who-owns (%1 missing files)</source>
        <translation>Alpine apk info -- kuka-owns (%1 puuttuvat tiedostot)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1986"/>
        <source>Arch pacman full transaction preflight</source>
        <translation>Arch pacman koko tapahtuma ennen lentoa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1987"/>
        <source>Build and verify Fedora initramfs for %1 (temporary output only)</source>
        <translation>Rakenna ja varmista Fedora initramfs %1:lle (vain nykyulostulo)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1988"/>
        <source>Check Alpine grub-install availability/version</source>
        <translation>Tarkista Alpine grub-install:n saatavuus/versio</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1989"/>
        <source>Check grub-install availability/version</source>
        <translation>Tarkista grub-install:n saatavuus/versio</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1991"/>
        <source>Complete interrupted package configuration before correction</source>
        <translation>Keskeytä pakettien konfigurointi ennen korjausta</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1992"/>
        <source>Create missing initramfs for %1</source>
        <translation>Luo puuttuva initramfs %1: lle</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1993"/>
        <source>dnf5 dependency check (read-only evidence)</source>
        <translation>dnf5-riippuvuustarkastus (vain luettavat todisteet)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1994"/>
        <source>Inspect DKMS state</source>
        <translation>Tutki DKMS-valtio</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1995"/>
        <source>Inspect DKMS state for GPU driver</source>
        <translation>Tutki DKMS-järjestelmä GPU-ajurille</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1996"/>
        <source>Post-repair DKMS status</source>
        <translation>Korjaamisen jälkeinen DKMS-tilanne</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1997"/>
        <source>Purge the wrong GPU driver</source>
        <translation>Puhdista väärä GPU-ajuri</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1998"/>
        <source>Read Fedora grubenv (grub2-editenv list)</source>
        <translation>Lue Fedora grubenv (grub2-editenv-luettelo)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1999"/>
        <source>Rebuild Alpine initramfs for %1 with mkinitfs</source>
        <translation>Korjaa Alpine initramfs %1:lle mkinitfs:llä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2000"/>
        <source>Rebuild Arch initramfs images with mkinitcpio -P</source>
        <translation>Rakenna Arch initramfs-kuvat uudelleen mkinitcpio -P:llä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2002"/>
        <source>Record Alpine apk version</source>
        <translation>Record Alpine apk versio</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2003"/>
        <source>Record dnf5 version</source>
        <translation>Tallenna dnnf5-versio</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2004"/>
        <source>Record dracut version</source>
        <translation>Tallenna dracut versio</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2005"/>
        <source>Record grub2-install version</source>
        <translation>Record grub2-install version</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2006"/>
        <source>Record rpm version</source>
        <translation>Tallenteen rpm-versio</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2007"/>
        <source>Record SELinux enforcement mode</source>
        <translation>Tallenna SELinux-valvontatila</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2008"/>
        <source>Record SELinux status</source>
        <translation>Tallenna SELinuxin tila</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2009"/>
        <source>Refresh dnf5 package metadata (dnf5 makecache)</source>
        <translation>Päivitä dnf5-paketin metatiedot (dnf5 makecache)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2010"/>
        <source>Regenerate extlinux configuration (overwrite=0 trial; no boot sector write)</source>
        <translation>Uudista extlinux-konfiguraatio (korvaa=0 koe; ei käynnistyssektorin kirjoitusta)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2011"/>
        <source>Regenerate extlinux configuration (default=%1, overwrite=0 trial; no boot sector write)</source>
        <translation>Uudista extlinux-konfiguraatio (oletus=%1, overwrite=0 experience; no boot sector write)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2012"/>
        <source>Regenerate Fedora GRUB2 configuration (grub2-mkconfig --no-grubenv-update)</source>
        <translation>Luo Fedora GRUB2-kokoonpano (grub2-mkconfig --no-grubenv-update)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2014"/>
        <source>Regenerate GRUB configuration with grub-mkconfig</source>
        <translation>Luo GRUB-konfiguraatio uudelleen grub-mkconfig:llä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2015"/>
        <source>Reinstall Alpine GRUB EFI bootloader (--no-nvram)</source>
        <translation>Reinstall Alpine GRUB EFI bootloader (-- no-nvram)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2016"/>
        <source>Reinstall GRUB EFI bootloader</source>
        <translation>Asenna GRUB EFI-käynnistin uudelleen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2017"/>
        <source>Reinstall GRUB EFI fallback loader (--removable)</source>
        <translation>Asenna GRUB EFI varakuormaaja uudelleen (--poistettava)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2018"/>
        <source>Reinstall GRUB EFI bootloader files</source>
        <translation>Asenna GRUB EFI -käynnistintiedostot uudelleen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2019"/>
        <source>Reinstall GRUB2 BIOS boot code</source>
        <translation>Asenna GRUB2 BIOS- käynnistyskoodi uudelleen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2020"/>
        <source>Reinstall protected kernel packages (rollback)</source>
        <translation>Asenna suojatut ydinpaketit uudelleen (rollback)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2021"/>
        <location filename="../src/MainWindow.cpp" line="20153"/>
        <source>Remove orphaned kernel build support</source>
        <translation>Poista orvon ytimen rakennetuki</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2022"/>
        <source>Remove orphaned kernel headers</source>
        <translation>Poista orvot ytimen otsikot</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2023"/>
        <source>Remove superseded kernel packages</source>
        <translation>Poista korvatut ydinpaketit</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2024"/>
        <source>Repair broken dependencies before known package correction</source>
        <translation>Korjaa rikkoutuneet riippuvuudet ennen tunnettua pakettikorjausta</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2026"/>
        <source>Restore package selections after failed GPU repair</source>
        <translation>Palauta pakettivalinnat epäonnistuttuasi GPU-korjauksen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2027"/>
        <source>Retry DKMS autoinstall after header correction</source>
        <translation>Yritä DKMS:ää asentaa automaattisesti otsikkokorjauksen jälkeen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2028"/>
        <source>Retry GRUB configuration after mapper alias correction</source>
        <translation>Yritä GRUB-koostumusta uudelleen mapper-nimesikorjauksen jälkeen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2029"/>
        <source>Retry GRUB EFI bootloader files without NVRAM update</source>
        <translation>Yritä uudelleen GRUB EFI-käynnistystiedostoja ilman NVRAM-päivitystä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2030"/>
        <source>Retry initramfs for %1 after mapper alias correction</source>
        <translation>Kokeile initramfs:ää uudelleen %1:lle mapper-aliaskorjausten jälkeen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2031"/>
        <source>Retry trial initramfs build for %1 after mapper alias correction</source>
        <translation>Kokeile uudelleen initramfs rakentaa %1 jälkeen mapper alias korjaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2032"/>
        <source>RPM file ownership mapping (rpm -qf, %1 missing files)</source>
        <translation>RPM-tiedostojen omistuskartoitus (rpm -qf, %1 puuttuvat tiedostot)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2033"/>
        <source>RPM package file verification (rpm -Va --nofiledigest, read-only)</source>
        <translation>RPM paketti tiedoston verifiointi (rpm - Va -- nofiledigest, vain luku)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2034"/>
        <source>Set Fedora GRUB2 default BLS entry (%1)</source>
        <translation>Aseta Fedora GRUB2 -oletussyöte (%1)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2035"/>
        <source>Set one-boot nomodeset next_entry</source>
        <translation>Aseta yhden saappaan nomodeset next entry</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2036"/>
        <source>Syntax-check %1 OpenRC init script</source>
        <translation>Syntax-check %1 OpenRC init script</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2037"/>
        <source>Syntax-check trial Fedora GRUB2 configuration</source>
        <translation>Syntax-tarkistus kokeilu Fedora GRUB2 asetukset</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2038"/>
        <source>Syntax-check trial GRUB configuration</source>
        <translation>Syntax-check kokeiluversio GRUB asetukset</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2039"/>
        <source>Trial initramfs build for %1 (temporary output only)</source>
        <translation>Trial initramfs rakentaa %1 (tilapäistä ulostuloa vain)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2040"/>
        <source>Trial mkinitcpio build for %1 (temporary output only)</source>
        <translation>Trial mkinitcpio rakentaa %1 (tilapäinen ulostulo vain)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2041"/>
        <source>Trial mkinitfs build for %1 (temporary output only)</source>
        <translation>Trial mkinitfs rakentaa %1 (tilapäisen ulostulon vain)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2042"/>
        <source>Update initramfs for %1</source>
        <translation>Päivitä initramfs %1:lle</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2043"/>
        <source>Upgrade installed packages (%1)</source>
        <translation>Päivitä asennetut paketit (%1)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2044"/>
        <source>Verify Fedora GRUB2 configuration syntax after boot-code reinstall</source>
        <translation>Varmista Fedora GRUB2 -asetussyntaksi käynnistyskoodin uudelleenasennuksen jälkeen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2045"/>
        <source>Verify installed Fedora GRUB2 configuration syntax</source>
        <translation>Tarkista asennettu Fedora GRUB2 asetus syntaksi</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2046"/>
        <source>Verify installed GRUB configuration syntax</source>
        <translation>Tarkista asennettu GRUB-asetus syntaksi</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2047"/>
        <source>Verify mkinitfs build inputs for %1 (mkinitfs -l)</source>
        <translation>Varmista mkinitfs:n rakennussyötteet %1:lle (mkinitfs -l)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2048"/>
        <source>Verify %1 service exists (rc-service -e)</source>
        <translation>Tarkista %1-palvelu on olemassa (rc-palvelu -e)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2238"/>
        <location filename="../src/MainWindow.cpp" line="15107"/>
        <source>Repair tool %1 is unavailable.</source>
        <translation>Korjaustyökalu %1 ei ole käytettävissä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2244"/>
        <location filename="../src/MainWindow.cpp" line="15112"/>
        <source>Repair tool %1 has unknown or unavailable diagnostic evidence.</source>
        <translation>Korjaustyökalu %1 on tuntematon tai ei saatavilla diagnostisia todisteita.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2251"/>
        <location filename="../src/MainWindow.cpp" line="15115"/>
        <source>No capability evidence for repair tool %1 in the selected scope. Run diagnostics first.</source>
        <translation>Valitussa laajuudessa ei ole näyttöä korjaustyökalusta %1. Tee ensin diagnostiikka.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2381"/>
        <location filename="../src/MainWindow.cpp" line="2386"/>
        <location filename="../src/MainWindow.cpp" line="2457"/>
        <source>Running-host default boot entry selection is unavailable.</source>
        <translation>Running-host oletus käynnistyksen valinta ei ole käytettävissä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2396"/>
        <location filename="../src/MainWindow.cpp" line="2415"/>
        <source>The helper did not identify a bootable running-host default entry.</source>
        <translation>Auttaja ei tunnistanut käynnistyvää juoksu-host-oletusmerkintää.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="3689"/>
        <source>About %1</source>
        <translation>Tietoja %1:stä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4239"/>
        <source>Linux detected — %1</source>
        <translation>Linux havaittu %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4248"/>
        <source>Unlocked Linux filesystem — inspect to confirm</source>
        <translation>Avaa Linux- tiedostojärjestelmä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4251"/>
        <source>Likely Linux — inspect to confirm</source>
        <translation>Todennäköisesti Linux</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4254"/>
        <source>Likely Linux — encrypted</source>
        <translation>Todennäköisesti Linux salattu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4257"/>
        <source>Encrypted — unlock to inspect</source>
        <translation>Salattu Avaa tarkastaa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4260"/>
        <source>Linux-capable — inspect to confirm</source>
        <translation>Linux-capable</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4262"/>
        <source>Available for inspection</source>
        <translation>Käytettävissä tarkastusta varten</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4282"/>
        <source>Encrypted volume</source>
        <translation>Salattu määrä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4284"/>
        <source>%1 volume</source>
        <translation>%1-tilavuus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4286"/>
        <source>Partition</source>
        <translation>Jako</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4291"/>
        <source>Unnamed device</source>
        <translation>Nimeämätön laite</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5004"/>
        <source>Preparing the Boot Bitch administrator session; authorization may be requested before the action starts.</source>
        <translation>Boot Bitch-valvojaistunnon valmistelu; valtuutusta voidaan pyytää ennen toimen aloittamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5246"/>
        <location filename="../src/MainWindow.cpp" line="19696"/>
        <location filename="../src/MainWindow.cpp" line="22363"/>
        <source>Repair file system errors</source>
        <translation>Korjaa tiedostojärjestelmän virheet</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5254"/>
        <source>The read-only check found file system errors in the %1. Select the devices to repair and the mode to use. Each repair runs separately after the helper repeats its scope, mount-state and tool preflights.</source>
        <translation>%1: stä löytyi tiedostojärjestelmän virheitä vain lukemalla. Valitse korjattavat laitteet ja käytettävä tila. Jokainen korjaus toimii erikseen sen jälkeen, kun apuri toistaa sen laajuus, asennustila ja työkalu ennen lentoa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5266"/>
        <location filename="../src/MainWindow.cpp" line="6479"/>
        <location filename="../src/MainWindow.cpp" line="6480"/>
        <source>Repair</source>
        <translation>Korjaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5266"/>
        <location filename="../src/MainWindow.cpp" line="6106"/>
        <source>Device</source>
        <translation>Laite</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5267"/>
        <source>File system</source>
        <translation>Tiedostojärjestelmä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5267"/>
        <source>Mode</source>
        <translation>Tila</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5268"/>
        <source>Issue summary</source>
        <translation>Tiivistelmä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5661"/>
        <source>Select and inspect the protected host and repair targets.</source>
        <translation>Valitse ja tarkista suojatut isäntä- ja korjauskohteet.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5662"/>
        <source>Run read-only diagnostics for the current scope.</source>
        <translation>Suorita vain lukudiagnostiikka nykyisen soveltamisalan osalta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5663"/>
        <source>Manage UEFI firmware boot entries and view the boot menu.</source>
        <translation>Hallitse UEFI firmware-käynnistystuloksia ja näytä käynnistysvalikko.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5664"/>
        <source>Run guarded repair tools and the Full Repair plan.</source>
        <translation>Suorita vartioidut korjaustyökalut ja Full Repair suunnitelma.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5665"/>
        <source>Inspect and roll back Btrfs root snapshots.</source>
        <translation>Tarkasta ja kieri takaisin Btrfs juurikuvia.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5667"/>
        <source>Copy files between the host and the repair system.</source>
        <translation>Kopioi tiedostot isäntä ja korjausjärjestelmän välillä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5668"/>
        <source>Review the session log and diagnostic history.</source>
        <translation>Tarkista istunnon loki ja diagnostinen historia.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5669"/>
        <source>Configure the Full Repair plan and preferences.</source>
        <translation>Aseta koko korjaussuunnitelma ja asetukset.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6173"/>
        <location filename="../src/MainWindow.cpp" line="10735"/>
        <source>Authorization deferred; press Authorize.</source>
        <translation>Valtuutusta on lykätty.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6332"/>
        <source>Select a target configuration file to inspect or edit.</source>
        <translation>Valitse tarkastettava tai muokattava kohdeasetustiedosto.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6533"/>
        <location filename="../src/MainWindow.cpp" line="6681"/>
        <source>Select a repair drive, or enter Host Maintenance.</source>
        <translation>Valitse korjausasema tai syötä huoltoasema.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6628"/>
        <location filename="../src/MainWindow.cpp" line="20292"/>
        <location filename="../src/MainWindow.cpp" line="21824"/>
        <source>GPU driver repair</source>
        <translation>GPU-ohjaimen korjaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11353"/>
        <source>Choose an action</source>
        <translation>Valitse toiminto</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15947"/>
        <location filename="../src/MainWindow.cpp" line="16473"/>
        <source>Running diagnostics…</source>
        <translation>Suoritetaan diagnostiikkaa…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19880"/>
        <location filename="../src/MainWindow.cpp" line="19882"/>
        <location filename="../src/MainWindow.cpp" line="20299"/>
        <location filename="../src/MainWindow.cpp" line="20389"/>
        <source>Interactive tool — run individually</source>
        <translation>Interaktiivinen työkalu - suorita erikseen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20013"/>
        <source>Running kernel: %1.</source>
        <translation>Käynnissä oleva ydin: %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20016"/>
        <source>Newest installed kernel: %1.</source>
        <translation>Uusin asennettu ydin: %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20019"/>
        <source>Repository latest kernel: %1 (newer than the installed newest).</source>
        <translation>Versiovaraston viimeisin ydin: %1 (uusi kuin asennettu uusin).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20022"/>
        <source>No superseded (older) kernels are installed.</source>
        <translation>Korvattuja (vanhoja) siemeniä ei ole asennettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20024"/>
        <source>Superseded kernel(s) available for removal: %1.</source>
        <translation>Yliarvostetut ytimet, jotka voidaan poistaa: %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20037"/>
        <source>Approximately %1 MB would be restored.</source>
        <translation>Noin %1 MB palautetaan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20042"/>
        <source>Missing headers (latest kernel has no matching headers): %1.</source>
        <translation>Puuttuvat otsikot (viimeisimmässä ytimessä ei ole vastaavia otsikoita): %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20050"/>
        <source>Orphaned kernel components (no owning package): %1.</source>
        <translation>Orvon ytimen komponentit (ei omistuspakettia): %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20054"/>
        <source>Orphaned kernel build support (kernel gone): %1.</source>
        <translation>Orvon ytimen rakentaa tukea (ydin poissa): %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20058"/>
        <source>Incomplete kernels (installed package without its on-disk files): %1.</source>
        <translation>Epätäydellinen ytimiä (asennettu paketti ilman levytiedostoja): %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20137"/>
        <source>Remove oldest/superseded kernel(s)</source>
        <translation>Poista vanhin/yliarvioitu ydin(s)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20141"/>
        <source>Remove newest kernel</source>
        <translation>Poista uusin ydin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20145"/>
        <source>Remove orphaned headers</source>
        <translation>Poista orpojen otsikot</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20149"/>
        <source>Remove orphaned kernel components</source>
        <translation>Poista orvon ytimen komponentit</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20157"/>
        <source>Install missing headers</source>
        <translation>Asenna puuttuvat otsikot</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20161"/>
        <source>Reinstall/complete a kernel</source>
        <translation>Asenna/täydennä ydin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20165"/>
        <source>Install repository-latest kernel</source>
        <translation>Asenna arkiston viimeisin ydin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20463"/>
        <source>Choose a kernel repair action from the evidence.</source>
        <translation>Valitse ytimen korjaustoiminto todisteista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6643"/>
        <source>Mirrors the Settings → Full Repair plan checkbox.</source>
        <translation>Peilaa Asetukset → Koko korjaussuunnitelman valintaruutu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6878"/>
        <source>Load Btrfs root snapshots read-only.</source>
        <translation>Lataa Btrfs-juurikuvat vain luettavaksi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7737"/>
        <source>Marked for deletion; Save applies it.</source>
        <translation>Merkitty poistettavaksi; Tallenna soveltaa sitä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8091"/>
        <source>Move the selected boot entry up; not applied until Save.</source>
        <translation>Siirrä valittu käynnistysmerkintä ylös; ei sovelleta ennen Tallenna.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8097"/>
        <source>Move the selected boot entry down; not applied until Save.</source>
        <translation>Siirrä valittua käynnistysmerkintää alaspäin; ei sovelleta ennen Tallenna.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8129"/>
        <source>Read-only; switch to the EFI/UKI view to restore.</source>
        <translation>Lue vain; vaihda EFI/UKI-näkymään palauttaaksesi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9211"/>
        <source>Automatic install requires package mapping and authorization.</source>
        <translation>Automaattinen asennus vaatii pakettikartoituksen ja luvan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10721"/>
        <source>Authorization request already running.</source>
        <translation>Valtuutuspyyntö on jo käynnissä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11938"/>
        <source>Host maintenance is active; file copy and chroot stay target-only.</source>
        <translation>Isäntähuolto on aktiivinen; tiedostojen kopiointi ja chroot pysyvät vain kohde.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13063"/>
        <source>Run a guarded rsync dry-run.</source>
        <translation>Tee vartioitu Rsync kuivaajo.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17583"/>
        <source>Entries from this window.</source>
        <translation>Tämän ikkunan merkinnät.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19836"/>
        <source>Run the selected stages using the cached diagnostics.</source>
        <translation>Suorita valitut vaiheet välimuistin avulla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20297"/>
        <source>Repair GPU driver</source>
        <translation>Korjaa GPU-ajuri</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20437"/>
        <source>Run the read-only safety preflight.</source>
        <translation>Suorita vain lukeminen ennen lentoa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20439"/>
        <source>Check and repair the detected file systems.</source>
        <translation>Tarkista ja korjaa havaitut tiedostojärjestelmät.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20441"/>
        <source>Complete interrupted package configuration.</source>
        <translation>Keskeytä pakettien konfigurointi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20443"/>
        <source>Repair broken package dependencies.</source>
        <translation>Korjaa rikkinäiset pakettiriippuvuudet.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20445"/>
        <source>Refresh package metadata.</source>
        <translation>Päivitä paketin metatiedot.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20447"/>
        <source>Simulate and apply a safe package upgrade.</source>
        <translation>Simuloi ja käytä turvallista pakettipäivitystä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20449"/>
        <source>Rebuild DKMS kernel modules.</source>
        <translation>Rakenna DKMS-ydinmoduulit uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20451"/>
        <source>Restore the graphical login manager.</source>
        <translation>Palauta graafinen kirjautumishallinta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20453"/>
        <source>Rebuild initramfs images.</source>
        <translation>Rakenna initramfs-kuvat uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20455"/>
        <source>Repair the EFI / UKI boot path.</source>
        <translation>Korjaa EFI / UKI boot polku.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20457"/>
        <source>Regenerate the GRUB2 configuration.</source>
        <translation>Luo GRUB2-konfiguraatio.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20457"/>
        <source>Regenerate the GRUB configuration.</source>
        <translation>Uudista GRUB-kokoonpano.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20459"/>
        <source>Regenerate the extlinux configuration.</source>
        <translation>Uudista extlinux-kokoonpano.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20461"/>
        <source>Reconcile the boot stack.</source>
        <translation>Tutkikaa saappaat.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20465"/>
        <source>No repair workflow is registered.</source>
        <translation>Korjaustyötä ei ole rekisteröity.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20473"/>
        <source>Reuses this session&apos;s bootloader repair; mapper/crypttab and initramfs still run.</source>
        <translation>Käyttää uudelleen tämän istunnon bootloader korjaus; mapper / cryptab ja initramfs edelleen käynnissä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5321"/>
        <source>The read-only check reported issues.</source>
        <translation>Vain lukutarkastus raportoi ongelmia.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5336"/>
        <source>Offline repair modes refuse mounted filesystems; btrfs scrub and zpool scrub are online modes and require a mounted filesystem. btrfs check --repair asks for an extra backup warning before it runs.</source>
        <translation>Offline korjaustilat kieltäytyä asennettu tiedostojärjestelmät; btrfs pesu ja zpool pesu ovat online-tiloja ja vaativat asennettu tiedostojärjestelmä. btrfs tarkistaa -- korjaus pyytää lisävaroitus ennen kuin se toimii.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5344"/>
        <source>Run Selected Repairs</source>
        <translation>Suorita valitut korjaukset</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5538"/>
        <source>Linux recovery and boot-repair utility</source>
        <translation>Linuxin palautus- ja käynnistyskorjausapuohjelma</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5540"/>
        <source>GUARDED REPAIR  •  %1</source>
        <translation>GUARDED KORJAUS • %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5572"/>
        <location filename="../src/MainWindow.cpp" line="5576"/>
        <source>A Boot Bitch operation is running in the background.</source>
        <translation>Taustalla on Boot Bitch-operaatio.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5625"/>
        <source>Show previous tab</source>
        <translation>Näytä edellinen välilehti</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5630"/>
        <source>Show next tab</source>
        <translation>Näytä seuraava välilehti</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5649"/>
        <location filename="../src/MainWindow.cpp" line="5921"/>
        <location filename="../src/MainWindow.cpp" line="5971"/>
        <location filename="../src/MainWindow.cpp" line="5972"/>
        <source>Systems</source>
        <translation>Järjestelmät</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5650"/>
        <location filename="../src/MainWindow.cpp" line="5923"/>
        <location filename="../src/MainWindow.cpp" line="6298"/>
        <location filename="../src/MainWindow.cpp" line="6300"/>
        <location filename="../src/MainWindow.cpp" line="8985"/>
        <location filename="../src/MainWindow.cpp" line="9110"/>
        <source>Diagnostics</source>
        <translation>Diagnostiikka</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5651"/>
        <location filename="../src/MainWindow.cpp" line="7108"/>
        <location filename="../src/MainWindow.cpp" line="7110"/>
        <location filename="../src/MainWindow.cpp" line="7397"/>
        <source>EFI boot entries</source>
        <translation>EFI- käynnistysmerkinnät</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5652"/>
        <source>Repair</source>
        <comment>tab noun</comment>
        <translation>Korjaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5653"/>
        <location filename="../src/MainWindow.cpp" line="6761"/>
        <source>Snapshots</source>
        <translation>Kuvat</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5654"/>
        <location filename="../src/MainWindow.cpp" line="8575"/>
        <location filename="../src/MainWindow.cpp" line="13579"/>
        <location filename="../src/MainWindow.cpp" line="13609"/>
        <source>Chroot Shell</source>
        <translation>Chroot Shell</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5655"/>
        <location filename="../src/MainWindow.cpp" line="8654"/>
        <source>File Copy</source>
        <translation>Tiedostokopio</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5656"/>
        <location filename="../src/MainWindow.cpp" line="5925"/>
        <location filename="../src/MainWindow.cpp" line="8892"/>
        <source>Logs</source>
        <translation>Lokit</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5657"/>
        <location filename="../src/MainWindow.cpp" line="5927"/>
        <source>Settings</source>
        <translation>Asetukset</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5735"/>
        <source>Ready — guarded repair mode</source>
        <translation>Valmiina</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5897"/>
        <source>&amp;File</source>
        <translation>-&amp;tiedosto</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5898"/>
        <location filename="../src/MainWindow.cpp" line="5975"/>
        <source>Refresh Devices</source>
        <translation>Päivitä laitteet</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5904"/>
        <source>Lock Administrator Session</source>
        <translation>Lukituksen ylläpitäjän istunto</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5912"/>
        <source>Administrator session locked — owned target resources released; the next privileged action will request authorization.</source>
        <translation>Administrator istunto lukittu Omistettu kohderesurssit vapautetaan; seuraava etuoikeutettu toiminta pyytää luvan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5916"/>
        <source>Quit</source>
        <translation>Lopeta</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5920"/>
        <source>&amp;View</source>
        <translation>-&amp;näkymä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5931"/>
        <source>Auto-size Device Columns</source>
        <translation>Automaattisesti kootut laitteen sarakkeet</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5934"/>
        <source>Wrap Log Lines</source>
        <translation>Kääri lokirivit</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5939"/>
        <source>&amp;Help</source>
        <translation>- &amp;ohje</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5940"/>
        <location filename="../src/MainWindow.cpp" line="18844"/>
        <source>Using Boot Bitch</source>
        <translation>Boot Bitch:n käyttö</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5943"/>
        <location filename="../src/MainWindow.cpp" line="18867"/>
        <source>About Boot Bitch</source>
        <translation>Tietoja Boot Bitch:stä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5973"/>
        <source>Select a physical drive; Boot Bitch resolves the most likely Linux system volume automatically. The running host stays protected from ordinary target repairs, with a separate explicit host-maintenance path for its own system.</source>
        <translation>Valitse fyysinen asema; Boot Bitch ratkaisee todennäköisimmän Linux-järjestelmän äänenvoimakkuuden automaattisesti. Juokseva isäntä pysyy suojattuna tavallisilta kohteiden korjauksilta, ja sillä on oma erillinen huoltopolku omaa järjestelmää varten.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5976"/>
        <source>Re-scan connected storage and repair targets.</source>
        <translation>Skannaa uudelleen liitetyt varastointi- ja korjauskohteet.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6013"/>
        <source>Detecting running system…</source>
        <translation>Havaitaan ajojärjestelmä…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6024"/>
        <source>Detecting protected storage…</source>
        <translation>Suojatun säilytyksen havaitseminen…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6044"/>
        <source>PROTECTED</source>
        <translation>SUOJATTU</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6050"/>
        <source>The running host remains protected from ordinary repair-target operations.</source>
        <translation>Juokseva isäntä on edelleen suojattu tavallisilta korjauskohteilta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6053"/>
        <source>Details</source>
        <translation>Yksityiskohdat</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6055"/>
        <source>Show read-only details for the protected running host.</source>
        <translation>Näytä vain luettavia tietoja suojatulle ajoisännälle.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6059"/>
        <location filename="../src/MainWindow.cpp" line="9796"/>
        <source>Host Maintenance</source>
        <translation>Huolto</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6066"/>
        <location filename="../src/MainWindow.cpp" line="9926"/>
        <source>Make Default</source>
        <translation>Tee oletus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6083"/>
        <source>Available repair targets</source>
        <translation>Käytettävissä olevat korjaustavoitteet</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6084"/>
        <source>Repair targets</source>
        <translation>Korjaustavoitteet</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6085"/>
        <source>Drives are ranked by visible Linux, EFI, filesystem and encryption evidence. Select the top-level drive; partitions and mapped volumes are informational.</source>
        <translation>Asemat ovat näkyvien Linux-, EFI-, tiedostojärjestelmä- ja salausnäyttöjen rankaisemia. Valitse ylätason asema; osiot ja kartoitetut volyymit ovat informatiivisia.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6087"/>
        <source>Most likely first</source>
        <translation>Todennäköisesti ensin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6105"/>
        <source>Model / Label</source>
        <translation>Malli / merkki</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6105"/>
        <location filename="../src/MainWindow.cpp" line="6800"/>
        <location filename="../src/MainWindow.cpp" line="9158"/>
        <source>Status</source>
        <translation>Tila</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6105"/>
        <source>Connection</source>
        <translation>Yhteys</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6106"/>
        <source>Size</source>
        <translation>Koko</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6106"/>
        <source>Filesystem</source>
        <translation>Tiedostojärjestelmä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6150"/>
        <source>Select Target</source>
        <translation>Valitse kohde</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6154"/>
        <location filename="../src/MainWindow.cpp" line="9738"/>
        <location filename="../src/MainWindow.cpp" line="10090"/>
        <location filename="../src/MainWindow.cpp" line="11766"/>
        <source>Unlock</source>
        <translation>Avaa lukitus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6156"/>
        <location filename="../src/MainWindow.cpp" line="9740"/>
        <source>Select a drive whose detected target is a locked LUKS volume.</source>
        <translation>Valitse asema, jonka havaittu kohde on lukittu LUKS-tilavuus.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6177"/>
        <source>Authorize</source>
        <translation>Hyväksy</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6185"/>
        <location filename="../src/MainWindow.cpp" line="11935"/>
        <source>Committed target: none</source>
        <translation>Sitoutunut tavoite: ei ole</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6191"/>
        <source>Unlock status</source>
        <translation>Avaa lukitustila</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6205"/>
        <location filename="../src/MainWindow.cpp" line="9743"/>
        <source>Select a drive to see unlock status.</source>
        <translation>Valitse asema nähdäksesi avaimen tilan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6206"/>
        <location filename="../src/MainWindow.cpp" line="10085"/>
        <location filename="../src/MainWindow.cpp" line="10769"/>
        <source>No unlock operation recorded for this drive in the current session.</source>
        <translation>Asemalle ei tallenneta avaustoimintoa tässä istunnossa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6210"/>
        <source>Selected drive details</source>
        <translation>Valitut aseman tiedot</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6250"/>
        <source>Drive:</source>
        <translation>Aja:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6251"/>
        <source>Detected target:</source>
        <translation>Havaittu kohde:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6252"/>
        <source>Model / label:</source>
        <translation>Malli / etiketti:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6253"/>
        <source>Status:</source>
        <translation>Tila:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6254"/>
        <source>Size:</source>
        <translation>Koko:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6255"/>
        <source>Connection:</source>
        <translation>Yhteys:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6256"/>
        <source>Filesystem:</source>
        <translation>Tiedostojärjestelmä:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6257"/>
        <source>UUID:</source>
        <translation>Hei.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6258"/>
        <source>Mounts:</source>
        <translation>Liitännät:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6259"/>
        <source>Protection:</source>
        <translation>Suojaus:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6301"/>
        <source>Diagnostics follow the Systems page: the selected repair drive while Host Maintenance is off, or the protected running host while Host Maintenance is active. Running Host diagnostics are available only inside the explicit Host Maintenance scope: enter Host Maintenance on the protected running-host card in Systems first. Both scopes provide read-only diagnostics; host EFI/UKI checks inspect the active ESP and firmware entries, while repair-drive checks mount only that system read-only.</source>
        <translation>Diagnostiikka seuraa Systems-sivua: valittu korjausasema, kun isäntähuolto on pois päältä, tai suojattu juoksun isäntä, kun isäntähuolto on aktiivinen. Running Host -diagnostiikka on saatavilla vain nimenomaisen isäntähuollon soveltamisalan sisällä: kirjoita isännän huolto suojattuun juoksu-host -korttiin ensin. Molemmat soveltamisalat tarjoavat vain lukudiagnostiikkaa; isäntä EFI/UKI-tarkastukset tarkastavat aktiiviset ESP- ja firmware-syötteet, kun taas korjaus-asematarkistukset asentavat vain kyseisen järjestelmän.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6310"/>
        <location filename="../src/MainWindow.cpp" line="6483"/>
        <location filename="../src/MainWindow.cpp" line="6764"/>
        <location filename="../src/MainWindow.cpp" line="8579"/>
        <location filename="../src/MainWindow.cpp" line="8658"/>
        <location filename="../src/MainWindow.cpp" line="11881"/>
        <source>Target: none selected</source>
        <translation>Kohde: ei valittu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6315"/>
        <location filename="../src/MainWindow.cpp" line="16002"/>
        <location filename="../src/MainWindow.cpp" line="16521"/>
        <source>Run All</source>
        <translation>Suorita kaikki</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6316"/>
        <source>Run All — run every available read-only diagnostic for the current scope.</source>
        <translation>Suorita Kaikki ... Suorita kaikki saatavilla olevat luku-vain vianmääritys nykyisen soveltamisalan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6334"/>
        <source>Edit Target File…</source>
        <translation>Muokkaa kohdetiedostoa…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6342"/>
        <source>Target configuration:</source>
        <translation>Kohdeasetukset:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6353"/>
        <source>Diagnostic checks</source>
        <translation>Diagnostiset tarkastukset</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6372"/>
        <source>Selected diagnostic</source>
        <translation>Valittu vianmääritys</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6377"/>
        <location filename="../src/MainWindow.cpp" line="14147"/>
        <source>Select a diagnostic</source>
        <translation>Valitse vianmääritys</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6380"/>
        <location filename="../src/MainWindow.cpp" line="14148"/>
        <source>Choose a diagnostic from the list.</source>
        <translation>Valitse diagnostiikka luettelosta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6384"/>
        <location filename="../src/MainWindow.cpp" line="13484"/>
        <location filename="../src/MainWindow.cpp" line="21265"/>
        <source>Ready</source>
        <translation>Valmis</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6397"/>
        <source>Results</source>
        <translation>Tulokset</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6400"/>
        <location filename="../src/MainWindow.cpp" line="14150"/>
        <location filename="../src/MainWindow.cpp" line="14219"/>
        <location filename="../src/MainWindow.cpp" line="14228"/>
        <location filename="../src/MainWindow.cpp" line="14256"/>
        <source>Run Diagnostic</source>
        <translation>Suorita diagnostiikka</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6408"/>
        <source>Diagnostic results appear here.</source>
        <translation>Diagnostiset tulokset näkyvät tässä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6420"/>
        <source>Copy Results</source>
        <translation>Kopioi tulokset</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6421"/>
        <source>Save Results…</source>
        <translation>Tallenna tulokset…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6481"/>
        <source>Run one repair tool or use the Full Repair plan against the selected repair drive. Choose Host Maintenance on Systems to run the same supported, guarded stages against the protected Running Host. Choose Full Repair stages in Settings. Enabled stages run in the order shown. Each configurable stage also appears below as an individual tool; the Full Repair column mirrors its current Settings state. The boot tools (EFI / UKI bootloader, GRUB or extlinux configuration, boot-stack reconciliation and Make Default) are independent: run them in any order, and a later action re-verifies what an earlier one changed and reports its own result. The active scope is shown beside Repair: selected repair drive or Running Host maintenance.</source>
        <translation>Suorita yksi korjaustyökalu tai käytä Full Repair suunnitelma vastaan valittu korjausasema. Valitse Host Maintenance on Systems ajaa samat tuetut, vartioidut vaiheet suojattua Juokseva isäntä. Valitse täydellinen korjausvaiheet Asetuksista. Käytössä olevat vaiheet suoritetaan järjestyksessä. Jokainen konfiguroitava vaihe näkyy myös alla yksittäisenä työkaluna; Full Repair- sarake peilaa sen nykyistä asetustilaa. Käynnistystyökalut (EFI / UKI bootloader, GRUB tai extlinux konfiguraatio, boot-stack täsmäytys ja Make Oletus) ovat riippumattomia: suorita ne missä tahansa järjestyksessä, ja myöhemmin toiminta tarkistaa, mitä aikaisempi muutti ja raportoi oman tuloksensa. Aktiivinen laajuus näkyy vieressä Korjaus: valittu korjausasema tai Running Host huolto.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6487"/>
        <location filename="../src/MainWindow.cpp" line="9082"/>
        <source>Full Repair plan</source>
        <translation>Täydellinen korjaussuunnitelma</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6499"/>
        <location filename="../src/MainWindow.cpp" line="19773"/>
        <source>No stages selected</source>
        <translation>Ei valittuja vaiheita</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6505"/>
        <source>Run All diagnostics for the selected target or running host before starting Full Repair. The report is read-only evidence used to choose and confirm repair stages.</source>
        <translation>Suorita kaikki diagnostiikat valitun kohteen tai käynnissä isäntä ennen Full Repair. Raportti on pelkkä luettava todiste korjausvaiheiden valitsemisesta ja vahvistamisesta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6518"/>
        <source>Configure Plan…</source>
        <translation>Määritä Plan…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6519"/>
        <source>Open Settings to choose which Full Repair stages are part of the plan.</source>
        <translation>Avaa asetukset valita, mitkä Full Repair vaiheet ovat osa suunnitelmaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6530"/>
        <location filename="../src/MainWindow.cpp" line="21877"/>
        <source>Run Full Repair</source>
        <translation>Suorita koko korjaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6578"/>
        <source>Individual repair tools</source>
        <translation>Yksittäiset korjaustyökalut</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6584"/>
        <source>Tool</source>
        <translation>Työkalu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6584"/>
        <location filename="../src/MainWindow.cpp" line="21839"/>
        <location filename="../src/MainWindow.cpp" line="21924"/>
        <location filename="../src/MainWindow.cpp" line="21938"/>
        <source>Full Repair</source>
        <translation>Täysi korjaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6620"/>
        <location filename="../src/MainWindow.cpp" line="20218"/>
        <source>Validate environment</source>
        <translation>Validointiympäristö</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6621"/>
        <location filename="../src/MainWindow.cpp" line="20359"/>
        <location filename="../src/MainWindow.cpp" line="22304"/>
        <location filename="../src/MainWindow.cpp" line="22335"/>
        <source>File system repair</source>
        <translation>Tiedostojärjestelmän korjaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6622"/>
        <location filename="../src/MainWindow.cpp" line="20226"/>
        <location filename="../src/MainWindow.cpp" line="21712"/>
        <source>Complete package configuration</source>
        <translation>Täydellinen pakettien kokoonpano</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6623"/>
        <location filename="../src/MainWindow.cpp" line="20233"/>
        <location filename="../src/MainWindow.cpp" line="21716"/>
        <source>Repair broken dependencies</source>
        <translation>Korjaa rikkinäiset riippuvuudet</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6624"/>
        <location filename="../src/MainWindow.cpp" line="9089"/>
        <location filename="../src/MainWindow.cpp" line="19699"/>
        <location filename="../src/MainWindow.cpp" line="20244"/>
        <location filename="../src/MainWindow.cpp" line="21113"/>
        <location filename="../src/MainWindow.cpp" line="21720"/>
        <source>Refresh package metadata</source>
        <translation>Päivitä paketin metatiedot</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6625"/>
        <location filename="../src/MainWindow.cpp" line="19700"/>
        <location filename="../src/MainWindow.cpp" line="20255"/>
        <location filename="../src/MainWindow.cpp" line="21724"/>
        <source>Upgrade installed packages</source>
        <translation>Asennettujen pakettien päivittäminen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6626"/>
        <location filename="../src/MainWindow.cpp" line="20266"/>
        <source>DKMS</source>
        <translation>DKMS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6629"/>
        <location filename="../src/MainWindow.cpp" line="20301"/>
        <source>Initramfs</source>
        <translation>Initramfs</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6630"/>
        <location filename="../src/MainWindow.cpp" line="20313"/>
        <source>EFI / UKI bootloader</source>
        <translation>EFI / UKI-käynnistin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6632"/>
        <location filename="../src/MainWindow.cpp" line="20352"/>
        <source>extlinux configuration</source>
        <translation>extlinux- kokoonpano</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6633"/>
        <location filename="../src/MainWindow.cpp" line="20368"/>
        <source>Boot stack reconciliation</source>
        <translation>Boot pino täsmäytys</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6661"/>
        <source>Selected tool</source>
        <translation>Valittu työkalu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6676"/>
        <location filename="../src/MainWindow.cpp" line="20195"/>
        <source>Select a repair tool</source>
        <translation>Valitse korjaustyökalu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6679"/>
        <location filename="../src/MainWindow.cpp" line="20198"/>
        <source>Run Tool</source>
        <translation>Suorita työkalu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6685"/>
        <source>Select a tool to review its repair action.</source>
        <translation>Valitse työkalu tarkistaa sen korjaustoiminta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6760"/>
        <source>Btrfs snapshots</source>
        <translation>Btrfs-kuvat</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6762"/>
        <source>Inspect Btrfs root snapshots and perform a transactional rollback. Rollback keeps the source snapshot unchanged, preserves the current @ root, promotes a writable copy to @, rebuilds the boot stack and automatically restores the old root if post-switch validation fails. In Host Maintenance the same workflow targets the running host through Snapper @ snapshots and Boot Bitch @rollback-before-* undo points; the running host starts the promoted root only after a reboot.</source>
        <translation>Tutki Btrfs:n juurikuvat ja suorita transaktion palautus. Rollback pitää lähdekuvan ennallaan, säilyttää nykyisen @ rootin, edistää kirjoitettavaa kopiota @: lle, rakentaa käynnistyspinon uudelleen ja palauttaa vanhan juuren automaattisesti, jos kytkin epäonnistuu. Host Maintenancessa sama työnkulku kohdistuu käynnissä olevaan isäntään Snapper @-kuvien ja Boot Bitch @rollback-ennen-* perumispisteiden kautta; käynnissä oleva isäntä aloittaa promoted rootin vasta uudelleenkäynnistyksen jälkeen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6783"/>
        <location filename="../src/MainWindow.cpp" line="12820"/>
        <location filename="../src/MainWindow.cpp" line="12969"/>
        <source>Reboot Now</source>
        <translation>Käynnistä nyt uudelleen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6785"/>
        <location filename="../src/MainWindow.cpp" line="12821"/>
        <source>Later</source>
        <translation>Myöhemmin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6791"/>
        <source>Reboot reminder hidden; the running-host rollback stays staged until the host is rebooted.</source>
        <translation>Uudelleenkäynnistysmuistutus piilotettu; käynnissä-host takaisin pysyy lavastettu kunnes isäntä on uudelleenkäynnistetty.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6800"/>
        <source>Snapshot</source>
        <translation>Kuva</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6800"/>
        <source>Created</source>
        <translation>Luotu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6800"/>
        <source>Type</source>
        <translation>Tyyppi</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6800"/>
        <source>Description</source>
        <translation>Tavaran kuvaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6847"/>
        <source>Load snapshots to inspect recovery points. Snapshot discovery runs through the guarded read-only helper, so root-owned Snapper metadata does not need to be readable by the desktop user.</source>
        <translation>Lataa valokuvia palautuspisteiden tarkastamiseksi. Snapshot löytö kulkee vartioitu lukuavain apuri, joten root-omistettu Snapper metadata ei tarvitse lukea työpöydän käyttäjä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6859"/>
        <source>Snapshot inventory</source>
        <translation>Kuvaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6871"/>
        <source>Load Snapshots</source>
        <translation>Lataa kuvat</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6872"/>
        <source>Inspect Selected</source>
        <translation>Tutki valittu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6873"/>
        <source>Roll Back to Selected</source>
        <translation>Siirrä takaisin valittuun</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6942"/>
        <source>Boot %1</source>
        <translation>Boot %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6943"/>
        <location filename="../src/MainWindow.cpp" line="7051"/>
        <source>Label: %1</source>
        <translation>Merkitys: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6944"/>
        <source>Loader: %1</source>
        <translation>Loader: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6945"/>
        <location filename="../src/MainWindow.cpp" line="6997"/>
        <source>Class: %1</source>
        <translation>Luokka: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6946"/>
        <source>Role: %1</source>
        <translation>Rooli: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6947"/>
        <source>PARTUUID: %1</source>
        <translation>PARTUUID: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6949"/>
        <source>Drive: %1</source>
        <translation>Aja: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6952"/>
        <source>Listed in BootOrder: yes</source>
        <translation>Luettelo BootOrder: kyllä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6953"/>
        <source>Listed in BootOrder: no (omitted by firmware)</source>
        <translation>Luettelo BootOrder: ei (firmware)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6955"/>
        <source>This entry is currently active (BootCurrent).</source>
        <translation>Tämä merkintä on tällä hetkellä aktiivinen (BootCurrent).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6967"/>
        <source>GRUB menu</source>
        <translation>GRUB-valikko</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6970"/>
        <source>Direct (UKI)</source>
        <translation>Suora (UKI)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6973"/>
        <source>Network</source>
        <translation>Verkko</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6976"/>
        <source>Fallback</source>
        <translation>Perääntyminen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6979"/>
        <source>Firmware</source>
        <translation>Firmware</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6981"/>
        <source>Other</source>
        <translation>Muut</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6996"/>
        <location filename="../src/MainWindow.cpp" line="7071"/>
        <source>Menu entry: %1</source>
        <translation>Valikkomerkintä: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6997"/>
        <source>(none)</source>
        <translation>(ei yhtään)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6999"/>
        <source>Menuentry id: %1</source>
        <translation>Valikkotunniste: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7002"/>
        <source>Source: foreign (detected by os-prober)</source>
        <translation>Lähde: ulkomainen (todettu os-prober:ssä)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7003"/>
        <source>Source: native</source>
        <translation>Lähde: syntyperäinen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7005"/>
        <source>This entry is the current GRUB default.</source>
        <translation>Tämä merkintä on nykyinen GRUB-oletus.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7011"/>
        <source>Loads: %1</source>
        <translation>Kuormat: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7028"/>
        <source>Firmware boot entries on this disk</source>
        <translation>Firmware- käynnistysmerkinnät tällä levyllä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7030"/>
        <source>Switch to the EFI/UKI entries view and load it to list this disk&apos;s firmware entries.</source>
        <translation>Vaihda EFI/UKI-syötteisiin nähdäksesi ja lataa se luetellaksesi tämän levyn laiteohjelmistot.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7038"/>
        <source>Boot%1 — %2 — %3 (%4)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7134"/>
        <location filename="../src/MainWindow.cpp" line="8026"/>
        <source>No scope selected</source>
        <translation>Kohdetta ei ole valittu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7138"/>
        <source>EFI/UKI entries</source>
        <translation>EFI/UKI-merkinnät</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7159"/>
        <source>Boot</source>
        <translation>Saapas</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7159"/>
        <source>Label</source>
        <translation>Merkitys</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7159"/>
        <source>Loader</source>
        <translation>Loader</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7159"/>
        <source>Boots</source>
        <translation>Saappaat</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7159"/>
        <location filename="../src/MainWindow.cpp" line="7280"/>
        <source>Class</source>
        <translation>Luokka</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7159"/>
        <source>Drive</source>
        <translation>Aja</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7184"/>
        <source>What booting this firmware entry does.</source>
        <translation>Mitä firmwaren syöttäminen tekee?</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7197"/>
        <source>Select Host Maintenance on the protected running-host card, or a repair target in Systems, to manage firmware boot entries. Load Entries lists the current scope&apos;s UEFI entries read-only.</source>
        <translation>Valitse isännän huolto suojatulla juoksu-host-kortilla tai korjauskohteena Systems, hallita firmware boot merkinnät. Lataa Merkinnät listaa nykyisen soveltamisalan UEFI merkinnät vain lukea.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7204"/>
        <source>Firmware boot entries</source>
        <translation>Firmware- käynnistysmerkinnät</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7214"/>
        <source>Load Entries</source>
        <translation>Lataa tietueet</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7215"/>
        <source>Move Up</source>
        <translation>Siirrä ylös</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7216"/>
        <source>Move Down</source>
        <translation>Siirrä alas</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7218"/>
        <source>Delete Selected</source>
        <translation>Poista valitut</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7219"/>
        <location filename="../src/MainWindow.cpp" line="8537"/>
        <source>EFI-/UKI Restore</source>
        <translation>EFI-/UKI Palauta</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7280"/>
        <source>Menu entry</source>
        <translation>Valikkomerkintä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7280"/>
        <source>Default</source>
        <translation>Oletus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7327"/>
        <source>Load Menu</source>
        <translation>Lataa valikko</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7442"/>
        <source>Select Host Maintenance on the running host, or a repair target in Systems, to manage firmware boot entries.</source>
        <translation>Valitse käyttöisännän isäntähuolto tai järjestelmäkorjauksen kohde, jotta voit hallita firmware-käynnistyksiä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7480"/>
        <source>Loading firmware boot entries</source>
        <translation>Ladataan firmware-käynnistystiedostoja</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7487"/>
        <source>List firmware boot entries</source>
        <translation>Listaa firmware- käynnistysmerkinnät</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7502"/>
        <source>Firmware boot entries could not be loaded. See Logs for details.</source>
        <translation>Firmware-kansioita ei voitu ladata. Katso lisätietoja lokeista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7533"/>
        <source>Loaded %1 firmware boot entries.</source>
        <translation>Ladattu %1-ohjelmiston käynnistys.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7752"/>
        <source>Double-click to rename this boot entry. Blank labels are refused.</source>
        <translation>Tupla-napsauta nimetäksesi tämän saappaan. Tyhjiä etikettejä ei hyväksytä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8019"/>
        <source>Running host: %1</source>
        <translation>Suoritettava isäntä: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8020"/>
        <source>Running host: not selected</source>
        <translation>Juokseva isäntä: ei valittu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8023"/>
        <source>Repair target: %1</source>
        <translation>Korjaustavoite: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8024"/>
        <source>Repair target: not ready</source>
        <translation>Korjaa kohde: ei valmis</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8043"/>
        <location filename="../src/MainWindow.cpp" line="8054"/>
        <source>Running-host</source>
        <translation>Isäntä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8043"/>
        <location filename="../src/MainWindow.cpp" line="8054"/>
        <source>Repair-target</source>
        <translation>Korjaustavoite</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8053"/>
        <source>%1 firmware entry management is available. Load Entries to list the UEFI entries read-only.</source>
        <translation>%1 firmware entry management on käytettävissä. Lataa tietueet vain lukeaksesi UEFI- tietueet.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8077"/>
        <source>Load the %1 UEFI firmware boot entries read-only.</source>
        <translation>Lataa %1 UEFI firmware-saappaat vain luettavaksi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8078"/>
        <location filename="../src/MainWindow.cpp" line="8085"/>
        <source>running host&apos;s</source>
        <translation>isäntien käyttö</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8078"/>
        <location filename="../src/MainWindow.cpp" line="8085"/>
        <source>repair target&apos;s</source>
        <translation>Korjaa kohteen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8092"/>
        <location filename="../src/MainWindow.cpp" line="8098"/>
        <location filename="../src/MainWindow.cpp" line="8112"/>
        <source>Select a boot entry row first.</source>
        <translation>Valitse aloitusrivi ensin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8348"/>
        <source>Boot entry delete unavailable</source>
        <translation>Saapuminen ei ole käytettävissä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8353"/>
        <source>Select a boot entry to delete first.</source>
        <translation>Valitse ensin poistettava käynnistysmerkintä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8366"/>
        <source>Boot%1 is the currently active firmware entry (BootCurrent) and cannot be deleted.</source>
        <translation>Boot%1 on tällä hetkellä aktiivinen firmware-merkintä (BootCurrent), eikä sitä voida poistaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8370"/>
        <location filename="../src/MainWindow.cpp" line="8386"/>
        <source>Cannot delete boot entry</source>
        <translation>Ei voi poistaa käynnistysmerkintää</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8380"/>
        <source>Boot%1 belongs to the running host&apos;s own firmware and cannot be deleted while a repair target is selected.</source>
        <translation>Boot%1 kuuluu käynnissä olevan isännän omiin firmware, eikä sitä voida poistaa korjauskohteen valinnan aikana.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8383"/>
        <location filename="../src/MainWindow.cpp" line="8445"/>
        <source>To manage the running host&apos;s own entries, switch to Host Maintenance first.</source>
        <translation>Voit hallita käynnissä isännän omia merkintöjä, vaihtaa isäntä huolto ensin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8569"/>
        <location filename="../src/MainWindow.cpp" line="13568"/>
        <location filename="../src/MainWindow.cpp" line="13768"/>
        <source>Chroot shell</source>
        <translation>Chroot-kuori</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8576"/>
        <location filename="../src/MainWindow.cpp" line="13577"/>
        <source>Run a command inside the selected repair system as root (sudo is not needed). Commands are executed one at a time in a fresh chroot. When a command asks a question, Boot Bitch shows it in a popup and sends your answer back to the command; cancelling stops the command. Non-interactive flags such as dnf update -y or apt-get -y upgrade remain recommended for unattended runs. Output is kept in this window and in the application log.</source>
        <translation>Suorita komento valitun korjausjärjestelmän sisällä juurina (sudoa ei tarvita). Komennot suoritetaan yksi kerrallaan tuoreella krootilla. Kun komento esittää kysymyksen, Boot Bitch näyttää sen ponnahdusikkunassa ja lähettää vastauksesi takaisin komentoon; komento keskeytetään. Ei-interaktiiviset liput, kuten dnf update -y tai apt-get -y päivitys on edelleen suositeltavaa valvomattomia juoksuja. Tulosta säilytetään tässä ikkunassa ja sovelluslokissa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8585"/>
        <location filename="../src/MainWindow.cpp" line="13584"/>
        <source>Command, for example: dnf update -y or update-grub</source>
        <translation>Komento, esimerkiksi: dnf update -y tai update-grub</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8590"/>
        <location filename="../src/MainWindow.cpp" line="13590"/>
        <source>Run Command</source>
        <translation>Suorita komento</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8592"/>
        <location filename="../src/MainWindow.cpp" line="13557"/>
        <source>Execute the command inside the selected repair system as root.</source>
        <translation>Suorita komento valitun korjausjärjestelmän sisällä juurina.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8608"/>
        <source>Command output will appear here.</source>
        <translation>Komennon tulostus näkyy täällä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8615"/>
        <location filename="../src/MainWindow.cpp" line="13595"/>
        <source>Commands can modify the target system. Review each command before running it.</source>
        <translation>Komento voi muuttaa kohdejärjestelmää. Tarkista jokainen komento ennen sen suorittamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8618"/>
        <source>Clear Output</source>
        <translation>Tyhjennä tuloste</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8652"/>
        <source>File copy</source>
        <translation>Tiedostokopiointi</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8655"/>
        <source>Copy and verify files in either direction. Host to Repair remounts only the selected target filesystem read-write after safety checks; Repair to Host keeps the repair target read-only. Browse Target Folders reads the selected repair tree through temporary read-only mounts, while transfers use rsync without --delete, path containment, ownership validation and post-copy verification.</source>
        <translation>Kopioi ja varmista tiedostot kumpaan suuntaan tahansa. Korjaaja asentaa uudelleen vain valitun kohdetiedostojärjestelmän lukeman turvallisuustarkastusten jälkeen; Korjaa Host pitää korjauskohde vain lukea. Selaa Target-kansiot lukevat valitun korjauspuun tilapäisten lukutaukojen kautta, kun taas siirrot käyttävät rsync ilman --delete, polun hallinta, omistuksen validointi ja kopion jälkeinen todentaminen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8667"/>
        <source>Preview Changes</source>
        <translation>Esikatselumuutokset</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8668"/>
        <location filename="../src/MainWindow.cpp" line="13966"/>
        <source>Copy and Verify</source>
        <translation>Kopioi ja varmista</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8671"/>
        <source>Run an rsync dry-run through the guarded helper. No files are changed.</source>
        <translation>Aja Rsync kuivakäynnillä vartijan läpi. Tiedostoja ei ole muutettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8685"/>
        <source>Direction:</source>
        <translation>Suunta:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8687"/>
        <location filename="../src/MainWindow.cpp" line="13920"/>
        <source>Host → Repair</source>
        <translation>Host → korjaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8688"/>
        <location filename="../src/MainWindow.cpp" line="13920"/>
        <source>Repair → Host</source>
        <translation>Korjaa →-ohjain</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8689"/>
        <source>Choose which system supplies the source files and which system receives them.</source>
        <translation>Valitse mikä järjestelmä toimittaa lähdetiedostot ja mikä järjestelmä vastaanottaa ne.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8714"/>
        <location filename="../src/MainWindow.cpp" line="13033"/>
        <source>Add Files…</source>
        <translation>Lisää tiedostoja…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8715"/>
        <location filename="../src/MainWindow.cpp" line="13034"/>
        <source>Add Folder…</source>
        <translation>Lisää kansio…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8716"/>
        <source>Remove</source>
        <translation>Poista</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8717"/>
        <source>Remove the selected staged source entries from this list.</source>
        <translation>Poista valitut vaiheistetut lähdetiedot tästä luettelosta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8718"/>
        <location filename="../src/MainWindow.cpp" line="13131"/>
        <source>Clear</source>
        <translation>Tyhjennä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8719"/>
        <source>Clear every staged source entry from this list.</source>
        <translation>Tyhjennä jokainen vaiheistettu lähdemerkintä tästä luettelosta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8742"/>
        <source>Choose Path…</source>
        <translation>Valitse polku…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8748"/>
        <source>3. Ownership and copy policy</source>
        <translation>3. Omistus- ja kopiointipolitiikka</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8751"/>
        <source>Ownership:</source>
        <translation>Omistusoikeus:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8753"/>
        <source>Smart destination ownership (recommended)</source>
        <translation>Älykäs määränpään omistus (suositeltu)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8754"/>
        <source>Preserve source numeric UID/GID</source>
        <translation>Säilytä lähdenumero UID/GID</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8891"/>
        <source>Application log</source>
        <translation>Sovellusloki</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8893"/>
        <source>Shows this application&apos;s session activity. Session files are listed on the left; Save a copy when you need to share troubleshooting details.</source>
        <translation>Näyttää sovelluksen istuntotoiminnan. Istuntotiedostot ovat vasemmalla; Tallenna kopio, kun haluat jakaa vianmääritystietoja.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8895"/>
        <source>Save As…</source>
        <translation>Tallenna As…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8896"/>
        <source>Clear Register</source>
        <translation>Tyhjennä rekisteri</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8932"/>
        <source>Session logs</source>
        <translation>Istunnon lokit</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8946"/>
        <source>New Session Log</source>
        <translation>Uuden istunnon loki</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8947"/>
        <source>Delete</source>
        <translation>Poista</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8267"/>
        <source>Delete firmware boot entry</source>
        <translation>Poista laiteohjelmiston käynnistys</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8431"/>
        <source>Boot entry rename unavailable</source>
        <translation>Saapuminen uudelleennimeä ei ole saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8442"/>
        <source>Boot%1 belongs to the running host&apos;s own firmware and cannot be renamed while a repair target is selected.</source>
        <translation>Boot%1 kuuluu käynnissä olevan isännän omaan firmwareon, eikä sitä voida nimetä uudelleen, kun korjauskohde valitaan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8454"/>
        <source>Cannot rename boot entry</source>
        <translation>Saapasta ei voi nimetä uudelleen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8290"/>
        <source>Rename firmware boot entry</source>
        <translation>Nimeä firmware-käynnistys uudelleen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8489"/>
        <source>Boot entry labels cannot be blank; the previous label was kept.</source>
        <translation>Saapastuloetiketit eivät voi olla tyhjiä; edellinen etiketti säilytettiin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8490"/>
        <source>Boot entry label</source>
        <translation>Saapastulonimi</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8491"/>
        <source>A blank boot entry label is not allowed. The previous label was kept.</source>
        <translation>Tyhjä saapasmerkintä ei ole sallittu. Edellinen merkki säilytettiin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8499"/>
        <source>Boot%1 rename cancelled.</source>
        <translation>Boot%1-nimitys peruttu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8518"/>
        <source>Restore boot entries unavailable</source>
        <translation>Palauta käynnistyssyötteet ei saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8538"/>
        <source>Reinstall the EFI/UKI bootloader and re-create or re-label missing firmware entries</source>
        <translation>Asenna EFI/UKI-käynnistin uudelleen ja luo puuttuvat laiteohjelmistot uudelleen tai merkitse ne uudelleen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8547"/>
        <source>Restore / regenerate boot entries</source>
        <translation>Palauta/uudista käynnistysmerkinnät</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8949"/>
        <location filename="../src/MainWindow.cpp" line="13196"/>
        <source>Refresh</source>
        <translation>Päivitä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8953"/>
        <location filename="../src/MainWindow.cpp" line="17665"/>
        <source>Add Note</source>
        <translation>Lisää huomautus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8954"/>
        <source>Append a timestamped NOTE entry to the current session log.</source>
        <translation>Lisää aikaleimattu HUOMAUTUSmerkintä istuntolokiin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8969"/>
        <source>Search log:</source>
        <translation>Etsi loki:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8975"/>
        <source>Filter application log (fuzzy match; use @section, e.g. @errors, for whole diagnostic sections)…</source>
        <translation>Suodatinsovellusloki (pörröinen ottelu; käytä @sektiota, esim. @errorit, kokonaisissa diagnostisissa osissa)…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8984"/>
        <source>All entries</source>
        <translation>Kaikki merkinnät</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8986"/>
        <source>Repairs</source>
        <translation>Korjaukset</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9072"/>
        <source>Device discovery</source>
        <translation>Laitelöytö</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9074"/>
        <source>Show devices without an identified Linux installation</source>
        <translation>Näytä laitteet ilman tunnistettua Linux-asennusta</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9075"/>
        <source>Show removable and USB storage</source>
        <translation>Näytä irrotettava ja USB- tallennus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9076"/>
        <source>Show encrypted devices before unlocking</source>
        <translation>Näytä salatut laitteet ennen avaamista</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9084"/>
        <source>Repair file system errors (read-only check first)</source>
        <translation>Korjaa tiedostojärjestelmän virheet (lukea vain tarkistaa ensin)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1990"/>
        <location filename="../src/MainWindow.cpp" line="9087"/>
        <location filename="../src/MainWindow.cpp" line="19697"/>
        <location filename="../src/MainWindow.cpp" line="21857"/>
        <source>Complete interrupted package configuration</source>
        <translation>Keskeytä pakettien asetukset</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2025"/>
        <location filename="../src/MainWindow.cpp" line="9088"/>
        <location filename="../src/MainWindow.cpp" line="19698"/>
        <location filename="../src/MainWindow.cpp" line="21110"/>
        <source>Repair broken package dependencies</source>
        <translation>Korjaa rikkinäiset pakettiriippuvuudet</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9090"/>
        <location filename="../src/MainWindow.cpp" line="21116"/>
        <source>Upgrade installed packages (adaptive APT simulation)</source>
        <translation>Asennettujen pakettien päivittäminen (mukautuva APT-simulaatio)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2001"/>
        <location filename="../src/MainWindow.cpp" line="9091"/>
        <location filename="../src/MainWindow.cpp" line="19701"/>
        <location filename="../src/MainWindow.cpp" line="21861"/>
        <source>Rebuild DKMS modules</source>
        <translation>Rakenna DKMS-moduulit uudelleen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9092"/>
        <location filename="../src/MainWindow.cpp" line="21119"/>
        <source>Restore detected graphical login manager and graphical.target</source>
        <translation>Palauta havaittu graafinen kirjautumisen hallinta ja graafinen. kohde</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9093"/>
        <location filename="../src/MainWindow.cpp" line="19703"/>
        <location filename="../src/MainWindow.cpp" line="21122"/>
        <source>Rebuild initramfs after mapper/crypttab validation</source>
        <translation>Rakenna initramfs uudelleen mapper/cryptab-validoinnin jälkeen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9094"/>
        <location filename="../src/MainWindow.cpp" line="21104"/>
        <source>Repair EFI / UKI boot path (explicit target ESP repair)</source>
        <translation>Korjaa EFI / UKI boot polku (selitä kohde ESP korjaus)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9095"/>
        <location filename="../src/MainWindow.cpp" line="19705"/>
        <location filename="../src/MainWindow.cpp" line="21107"/>
        <source>Update GRUB configuration</source>
        <translation>Päivitä GRUB- asetus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9096"/>
        <location filename="../src/MainWindow.cpp" line="19706"/>
        <location filename="../src/MainWindow.cpp" line="21125"/>
        <source>Update extlinux configuration</source>
        <translation>Päivitä extlinux- asetus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9112"/>
        <source>Automatically regenerate read-only diagnostics after repairs or target changes</source>
        <translation>Uudista automaattisesti vain lukudiagnostiikka korjausten tai tavoitemuutosten jälkeen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9118"/>
        <source>Mandatory safety controls</source>
        <translation>Pakollinen turvavalvonta</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9121"/>
        <source>Protect every physical device backing /, /boot and /boot/efi</source>
        <translation>Suojaa jokaisen fyysisen laitteen tausta /, /boot ja /boot/efi</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9122"/>
        <source>Require explicit confirmation before package installation or repair actions</source>
        <translation>Vaadi nimenomainen vahvistus ennen pakettien asennus- tai korjaustoimia</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9123"/>
        <source>Require mapper/crypttab consistency before initramfs rebuild</source>
        <translation>Vaadi mapper/cryptab johdonmukaisuutta ennen initramfs uudelleen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9124"/>
        <source>Preserve pre-rollback Btrfs root and auto-restore it on validation failure</source>
        <translation>Säilytä pre-rollback Btrfs root ja automaattinen palauttaa sen validointivirhe</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9125"/>
        <source>Never log LUKS passphrases or authentication secrets</source>
        <translation>Älä koskaan kirjaudu LUKS:n salalauseisiin tai todennussalaisuuksiin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9135"/>
        <source>Host capabilities and dependencies</source>
        <translation>Isäntävalmiudet ja riippuvuussuhteet</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9150"/>
        <source>Distribution:</source>
        <translation>Jakautuminen:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9151"/>
        <source>Package manager family:</source>
        <translation>Pakettipäällikön perhe:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9152"/>
        <source>KAuth build support:</source>
        <translation>KAuthin rakennetuki:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9157"/>
        <source>Feature</source>
        <translation>Ominaisuus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9157"/>
        <source>Command</source>
        <translation>Komento</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9157"/>
        <source>Scope</source>
        <translation>Soveltamisala</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9158"/>
        <source>Suggested package</source>
        <translation>Ehdotettu paketti</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9158"/>
        <source>Notes</source>
        <translation>Huomautuksia</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9207"/>
        <source>Refresh Capabilities</source>
        <translation>Päivitä valmiudet</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9208"/>
        <source>Install Missing Support…</source>
        <translation>Asenna puuttuva tuki…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9435"/>
        <source>Scanning block devices…</source>
        <translation>Skannataan lohkolaitteet…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9446"/>
        <location filename="../src/MainWindow.cpp" line="9447"/>
        <source>Device scan failed</source>
        <translation>Laitteen skannaus epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9479"/>
        <source>Device scan complete — discovery did not modify storage</source>
        <translation>Laitteen skannaus täydellinen ... löytö ei muuttanut tallennusta</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9534"/>
        <source>Showing %1 of %2 repair candidate disk(s); %3 running-system disk(s) protected</source>
        <translation>Näytetään %1 %2 korjaus ehdokas levykkeet; %3 juoksu-järjestelmä levy [s] suojattu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9604"/>
        <source>Running system protection unresolved</source>
        <translation>Juoksevan järjestelmän suojaus selvittämätön</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9605"/>
        <source>No protected physical backing disk was identified</source>
        <translation>Suojattua fyysistä taustalevyä ei tunnistettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9615"/>
        <source>Current running Linux system</source>
        <translation>Nykyinen Linux-järjestelmä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9619"/>
        <source>Protected running-system storage</source>
        <translation>Suojattu ajojärjestelmän varastointi</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9620"/>
        <source>Critical mounts: %1</source>
        <translation>Kriittiset kiinnikkeet: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9780"/>
        <source>Showing protected running-host details</source>
        <translation>Näytetään suojatut ajo-isäntätiedot</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9837"/>
        <source>Host maintenance unavailable</source>
        <translation>Huoltoa ei ole saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9851"/>
        <source>Exit Host Maintenance</source>
        <translation>Poistuminen isäntähuolto</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9900"/>
        <source>Host default unavailable</source>
        <translation>Isännän oletus ei saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9906"/>
        <source>Make host the default boot entry</source>
        <translation>Asenna oletuskäynnistys</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9907"/>
        <source>Restore/ensure and select the running host&apos;s default boot entry?</source>
        <translation>Palauttaa / varmistaa ja valitse käynnissä olevan isännän oletuskäynnistys?</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9913"/>
        <location filename="../src/MainWindow.cpp" line="15185"/>
        <source>default boot entry</source>
        <translation>Oletuskäynnistys</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9914"/>
        <location filename="../src/MainWindow.cpp" line="15186"/>
        <source>%1 default boot entry</source>
        <translation>%1:n oletuskäynnistys</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9916"/>
        <source>Host disk: %1
Root: %2

Boot Bitch will identify the host ESP, restore/ensure the verified %3, and place it first in BootOrder. The fallback/recovery route and every other firmware entry stay bootable and are never deleted.</source>
        <translation>Käyttölevy: %1
Juuri: %2

Boot Bitch tunnistaa isäntä ESP, palauttaa / varmistaa todennetun %3, ja asettaa sen ensin BootSorder. Vara-/hyödyntämisreitti ja kaikki muut firmware-ohjelmistot pysyvät käynnistyskelpoisina eikä niitä koskaan poisteta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9920"/>
        <source>Host disk: %1
Root: %2

Boot Bitch will identify the running host&apos;s boot configuration, restore/ensure the verified %3, and select it as the default. Every other boot entry stays bootable and is never deleted.</source>
        <translation>Käyttölevy: %1
Juuri: %2

Boot Bitch tunnistaa juoksuisännän käynnistyskokoonpanon, palauttaa/varmistaa vahvistetun %3:n ja valitsee sen oletusarvoksi. Kaikki muut kengät pysyvät käynnistyskelpoisina, eikä niitä koskaan poisteta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9938"/>
        <source>Make host default boot entry</source>
        <translation>Tee oletuskäynnistys</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9989"/>
        <source>Host default set to %1</source>
        <translation>Aseta oletus %1:ään</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9992"/>
        <source>Host default verified</source>
        <translation>Oletus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9993"/>
        <source>%1

Full helper output is available in Logs.</source>
        <translation>%1

Täysi apuri tuloste on saatavilla lokeissa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10000"/>
        <source>Host default not verified: the helper did not name a verified boot entry.</source>
        <translation>Isännän oletus ei todennettu: auttaja ei nimennyt todennettua käynnistysmerkintää.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10001"/>
        <source>Host default not verified</source>
        <translation>Asennuksen oletusarvoa ei tarkistettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10003"/>
        <source>Host default operation failed: %1</source>
        <translation>Oletustoiminto epäonnistui: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10006"/>
        <source>Host default operation failed</source>
        <translation>Oletustoiminto epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10007"/>
        <source>The host default operation failed: %1

Full helper output is available in Logs.</source>
        <translation>Oletustoiminto epäonnistui: %1

Täysi apuri tuloste on saatavilla lokeissa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10027"/>
        <source>Pending inspection</source>
        <translation>Ennen tarkastusta</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10040"/>
        <source>PROTECTED — running system; read-only details only</source>
        <translation>SUOJATTU ajojärjestelmä; ainoastaan luettavat tiedot</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10042"/>
        <source>Live / installer media — not selectable</source>
        <translation>Live / asentaja mediat Ei valittavissa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10044"/>
        <source>Unlock required before selection</source>
        <translation>Avaa vaaditaan ennen valintaa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10045"/>
        <source>Eligible repair candidate</source>
        <translation>Tukikelpoiset korjausehdokkaat</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10059"/>
        <source>Commit this physical drive as the repair target.</source>
        <translation>Toimita tämä fyysinen asema korjauskohteena.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10062"/>
        <source>Live / installer media is read-only boot media and cannot be selected as a repair target.</source>
        <translation>Live / asentaja media on luku-avain media, eikä sitä voida valita korjauskohteeksi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10064"/>
        <source>Unlock the encrypted volume first; Select Target becomes available after a Linux filesystem is detected.</source>
        <translation>Avaa salattu äänenvoimakkuus ensin; Valitse kohde tulee saataville sen jälkeen, kun Linux-tiedostojärjestelmä on havaittu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10065"/>
        <source>No selectable repair target was detected on this drive.</source>
        <translation>Asemalla ei havaittu valittua korjauskohdetta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10066"/>
        <source>The protected running host cannot be selected as a repair target.</source>
        <translation>Suojattua käynnissä olevaa isäntää ei voida valita korjauskohteeksi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10082"/>
        <source>Already unlocked before this Boot Bitch session. No unlock operation was performed here; the visible mapper will be reused and will not be closed by Boot Bitch.</source>
        <translation>Avattu jo ennen Boot Bitch-istuntoa. Täällä ei suoritettu avaustoimintoa; näkyvää karttaa käytetään uudelleen eikä Boot Bitch sulje sitä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10090"/>
        <source>Already Unlocked</source>
        <translation>Avattu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10100"/>
        <source>No locked LUKS component is currently visible on this selected drive.</source>
        <translation>Tällä hetkellä valitulla asemalla ei ole lukittua LUKS-komponenttia.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10102"/>
        <source>The protected running host cannot be unlocked or modified by Boot Bitch.</source>
        <translation>Boot Bitch ei voi avata tai muokata suojattua juoksu isäntää.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10125"/>
        <location filename="../src/MainWindow.cpp" line="10161"/>
        <source>Target unavailable</source>
        <translation>Kohde ei saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10126"/>
        <source>The selected drive is no longer available. Refresh devices and select it again.</source>
        <translation>Valittua asemaa ei ole enää saatavilla. Päivitä laitteet ja valitse se uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10139"/>
        <source>Protected system</source>
        <translation>Suojattu järjestelmä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10140"/>
        <source>The running system cannot be selected as a repair target.</source>
        <translation>Ajojärjestelmää ei voi valita korjauskohteeksi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10149"/>
        <source>Live / installer media cannot be selected</source>
        <translation>Live-/asentajamediaa ei voi valita</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10150"/>
        <source>This drive carries read-only live/installer media. Insert or select a data drive instead; the boot media itself is never a repair target.</source>
        <translation>Tämä asema kuljettaa vain live-/asentajan mediaa. Aseta tai valitse data-asema sen sijaan; itse käynnistyslevy ei ole koskaan korjauskohde.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10156"/>
        <source>Unlock or select a Linux system first</source>
        <translation>Avaa tai valitse Linux-järjestelmä ensin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10157"/>
        <source>This encrypted drive has no visible Linux filesystem yet. Use Unlock, refresh devices, and select the target after its Linux root is detected.</source>
        <translation>Tällä salatulla asemalla ei ole vielä näkyvää Linux-tiedostojärjestelmää. Avaa, virkistä laitteita ja valitse kohde sen jälkeen, kun sen Linux root on havaittu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10162"/>
        <source>This drive has no resolvable target component. Refresh devices and select it again.</source>
        <translation>Tämä asema ei ole ratkaistavissa kohdekomponentti. Päivitä laitteet ja valitse se uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10180"/>
        <source>Repair drive selected: %1</source>
        <translation>Korjausasema valittu: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10331"/>
        <source>Requesting administrator authorization</source>
        <translation>Hallinnoija-valtuutuksen pyytäminen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10370"/>
        <location filename="../src/MainWindow.cpp" line="10513"/>
        <source>Administrator authorization active for this Boot Bitch window</source>
        <translation>Tämän Boot Bitch-ikkunan ylläpitäjän valtuutus aktiivinen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10720"/>
        <source>Requesting administrator authorization…</source>
        <translation>Pyydän valvoja-valtuutusta…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10733"/>
        <source>Administrator authorization deferred — diagnostics will regenerate after you authorize.</source>
        <translation>Administrator valtuutusta on lykätty.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10740"/>
        <source>Administrator authorization failed — press Authorize to retry.</source>
        <translation>Administrator-valtuutus epäonnistui ...</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10982"/>
        <location filename="../src/MainWindow.cpp" line="11025"/>
        <source>another privileged operation</source>
        <translation>toinen etuoikeutettu operaatio</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11029"/>
        <source>superseded by the scope change to %1 and was not queued</source>
        <translation>korvasi soveltamisalan muutos %1 eikä ollut jonossa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11030"/>
        <source>refused because &apos;%1&apos; is still running and was not queued. Wait for it to finish and retry, or cancel it</source>
        <translation>Kieltäytyi, koska &quot;%1&quot; on yhä käynnissä eikä ollut jonossa. Odota, että se valmistuu ja yrittää uudelleen, tai peruuttaa sen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11031"/>
        <source>Privileged request &apos;%1&apos; was %2.</source>
        <translation>%1 oli %2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11033"/>
        <source>ERROR: Privileged request &apos;%1&apos; was %2.
</source>
        <translation>%1 oli %2.
</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11117"/>
        <source>The administrator authorization session could not be established.</source>
        <translation>Hallinnoijan valtuutusistuntoa ei voitu perustaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11123"/>
        <source>Authorization unavailable</source>
        <translation>Valtuutus puuttuu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11133"/>
        <source>The privileged helper session is not running; the plan stopped before its request started.</source>
        <translation>Etuoikeutettu auttajan istunto ei ole käynnissä; suunnitelma pysähtyi ennen pyynnön alkua.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11145"/>
        <source>Using the authorized Boot Bitch administrator session. The GUI itself is still running as your normal user.</source>
        <translation>Käyttämällä valtuutettua Boot Bitch:n ylläpitäjäistuntoa. Itse käyttöliittymä toimii edelleen normaalina käyttäjänä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11190"/>
        <source>Privileged request &apos;%1&apos; exceeded the %2 safety limit; aborting the request and closing the unresponsive helper session so the UI cannot stay busy indefinitely.</source>
        <translation>Etuoikeutettu pyyntö &apos;%1&apos; ylitti %2:n turvallisuusrajan; pyynnön keskeyttäminen ja vastaamattoman auttajan istunnon sulkeminen, jotta käyttöliittymä ei voi pysyä kiireisenä loputtomiin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11193"/>
        <source>The privileged operation exceeded the %1 safety limit and was aborted. Review the output before retrying.</source>
        <translation>Etuoikeutettu toiminta ylitti %1 turvarajan ja se keskeytettiin. Tarkista tuloste ennen kuin yritän uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11304"/>
        <source>Protocol note: a PROMPT record exceeded the 1024-byte payload bound and was dropped; the helper was answered with an empty ANSWER record.</source>
        <translation>Protokolla huomautus: PROMPT-ennätys ylitti 1024-tavuisen kuorman ja se pudotettiin; apuriin vastattiin tyhjällä ANSWER-levyllä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11329"/>
        <source>ERROR: The privileged request exceeded the interactive prompt budget; treating the request as a protocol failure.</source>
        <translation>VIRHE: Etuoikeutettu pyyntö ylitti interaktiivisen nopean budjetin; käsitellään pyyntöä pöytäkirjan epäonnistumisena.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11352"/>
        <source>Shell command is asking for input</source>
        <translation>Shell- komento pyytää syötystä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11357"/>
        <source>The shell command is waiting for input. Review its output below, type the answer and press OK. Cancel — or an empty answer — stops the command.</source>
        <translation>Käsky odottaa syöttöä. Tarkista sen tulosteen alla, kirjoita vastaus ja paina OK. Peruuta komento tai tyhjä vastaus pysäytä komento.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11372"/>
        <source>Answer (for example: y, n, Y, I, N, Z, or a word)</source>
        <translation>Vastaus (esimerkiksi: y, n, Y, I, N, Z tai sana)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11377"/>
        <source>Send Answer</source>
        <translation>Lähetä vastaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11444"/>
        <source>Privileged operation completed successfully. Administrator authorization remains active for this Boot Bitch window.</source>
        <translation>Ensisijainen operaatio suoritettu onnistuneesti. Hallitsijan valtuutus pysyy aktiivisena tämän Boot Bitch-ikkunan osalta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11445"/>
        <source>Privileged operation stopped with an error. Administrator authorization remains active; review the output before closing.</source>
        <translation>Etuoikeutettu operaatio päättyi virheellä. Hallitsijan valtuutus pysyy aktiivisena; tarkista tuloste ennen sulkemista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11469"/>
        <source>The privileged helper session ended unexpectedly. The next root action will require authorization again.</source>
        <translation>Etuoikeutettu avustajakokous päättyi odottamatta. Seuraava juuritoiminta vaatii jälleen luvan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11568"/>
        <source>ERROR: Privileged request &apos;%1&apos; exceeded the %2 safety limit; the request was aborted and the unresponsive helper session was closed. The operation may not have completed.</source>
        <translation>VIRHE: Etuoikeutettu pyyntö &quot;%1&quot; ylitti %2 turvallisuusrajan; pyyntö keskeytettiin ja vastaamaton auttaja istunto suljettiin. Operaatiota ei ole saatettu päätökseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11658"/>
        <source>Unlock not required</source>
        <translation>Avaa lukitusta ei vaadita</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11660"/>
        <source>This drive already has an unlocked Linux filesystem. Boot Bitch will reuse the existing mapper.</source>
        <translation>Tällä asemalla on jo lukitsematon Linux-tiedostojärjestelmä. Boot Bitch käyttää olemassa olevaa karttaa uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11661"/>
        <source>The selected drive does not currently contain a locked LUKS component that needs to be opened.</source>
        <translation>Valittu asema ei tällä hetkellä sisällä lukittua LUKS-komponenttia, joka on avattava.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11668"/>
        <location filename="../src/MainWindow.cpp" line="11684"/>
        <source>Unlock unavailable</source>
        <translation>Avaa lukitusta ei saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11669"/>
        <location filename="../src/MainWindow.cpp" line="21254"/>
        <location filename="../src/MainWindow.cpp" line="21291"/>
        <source>The privileged Boot Bitch helper was not found. Rebuild or install this source tree.</source>
        <translation>Etuoikeutettua Boot Bitch-apulaista ei löydetty. Rakenna tai asenna tämä lähdepuu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11676"/>
        <source>Unlocking %1</source>
        <translation>Avataan %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11692"/>
        <source>Unlock LUKS repair target</source>
        <translation>Avaa LUKS-korjauskohde</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11720"/>
        <source>LUKS volume:</source>
        <translation>LUKS- tilavuus:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11740"/>
        <source>Enter the passphrase to unlock this volume.</source>
        <translation>Anna salalause avataksesi tämän äänenvoimakkuuden.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11747"/>
        <source>LUKS passphrase</source>
        <translation>LUKS-salalause</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11752"/>
        <source>Administrator authorization is already active. The passphrase is sent only to cryptsetup over the privileged helper pipe and is never logged or placed on a command line.</source>
        <translation>Valtuutus on jo voimassa. Salauslause lähetetään vain kryptattavaksi etuoikeutetun auttajaputken päälle eikä sitä koskaan kirjaudu tai sijoiteta komentoriville.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11782"/>
        <source>Passphrase required</source>
        <translation>Vaadittu salauslause</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11783"/>
        <source>An empty passphrase was not submitted. Enter the LUKS passphrase or choose Cancel.</source>
        <translation>Tyhjää salalausetta ei esitetty. Anna LUKS-salalause tai valitse Peruuta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11795"/>
        <source>Unlock LUKS target</source>
        <translation>Avaa LUKS-kohde</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11827"/>
        <source>Passphrase not accepted</source>
        <translation>Tunnuslausetta ei hyväksytä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11828"/>
        <source>The LUKS passphrase was not accepted.</source>
        <translation>LUKS-salalausetta ei hyväksytty.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11829"/>
        <source>Try again? Administrator authorization remains active, so only the disk passphrase will be requested again.</source>
        <translation>Yritä uudelleen? Hallitsijan valtuutus pysyy aktiivisena, joten vain levyn salalause pyydetään uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11870"/>
        <source>Encrypted target unlocked — select or reselect the repair target</source>
        <translation>Salattu kohde lukittu Valitse tai valitse uudelleen korjauskohde</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11885"/>
        <location filename="../src/MainWindow.cpp" line="11933"/>
        <source>Host maintenance: %1</source>
        <translation>Huolto: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11890"/>
        <source>Target: %1</source>
        <translation>Kohde: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11891"/>
        <source>Selected physical repair drive: %1</source>
        <translation>Valittu fyysinen korjausasema: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11910"/>
        <source>Running Host: %1</source>
        <translation>Suoritusohjain: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11936"/>
        <source>Committed target: %1</source>
        <translation>Kohde: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11975"/>
        <source>✓ SELECTED TARGET  •  %1</source>
        <translation>✓ VALITTU TAVOITE • %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12032"/>
        <source>The running host root is not Btrfs, so Btrfs snapshots are not available.</source>
        <translation>Juoksevan isäntäjuurta ei ole Btrfs, joten Btrfs-kuvia ei ole saatavilla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12033"/>
        <source>The selected Linux root is not Btrfs, so Btrfs snapshots are not available.</source>
        <translation>Valittu Linux root ei ole Btrfs, joten Btrfs-kuvia ei ole saatavilla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6879"/>
        <location filename="../src/MainWindow.cpp" line="12047"/>
        <source>Inspect the selected snapshot read-only.</source>
        <translation>Tarkasta vain valittu kuvakuva.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7192"/>
        <source>Select an entry to enable actions; double-click Label to rename.</source>
        <translation>Valitse syöte, jonka avulla toiminnot voidaan ottaa käyttöön; kaksoisnapsauta Etikettiä nimetä uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7227"/>
        <location filename="../src/MainWindow.cpp" line="8126"/>
        <source>Re-create or re-label missing UKI/fallback/WebFAI/iPXE entries.</source>
        <translation>Luo uudelleen tai uudelleen puuttuvat UKI/fallback/WebFAI/iPXE-merkinnät.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7307"/>
        <source>Generated boot menu in file order; read-only.</source>
        <translation>Luotu käynnistysvalikko tiedostojärjestyksessä; luku vain.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8103"/>
        <source>Apply pending reorder, delete and rename changes.</source>
        <translation>Käytä odottavaa uudelleentilausta, poista ja nimeä muutokset uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8111"/>
        <source>Mark the selected entry for deletion; Save applies it.</source>
        <translation>Merkitse valittu merkintä poistettavaksi; Tallenna soveltaa sitä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12048"/>
        <source>Select a snapshot row first.</source>
        <translation>Valitse ensin kuvakaappausrivi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12057"/>
        <location filename="../src/MainWindow.cpp" line="12068"/>
        <source>Select a valid Linux root snapshot first.</source>
        <translation>Valitse ensin kelvollinen Linuxin juurikuva.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12145"/>
        <source>Btrfs snapshot preload for %1 is deferred until the running privileged operation finishes; the request was not queued.</source>
        <translation>Btrfs-kuvakuvan esilatausta %1:lle lykätään siihen asti, kunnes käynnissä oleva etuoikeutettu toiminto päättyy; pyyntö ei ollut jonossa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12160"/>
        <source>Loading Btrfs snapshots in the background…</source>
        <translation>Btrfs-kuvien lataaminen taustalla…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12217"/>
        <source>Btrfs snapshot inventory is not applicable: the running host root filesystem is %1, not Btrfs.

No snapshot was loaded and the running host was not modified.</source>
        <translation>Btrfs-kuvan inventointia ei sovelleta: käynnissä oleva isäntäroot-tiedostojärjestelmä on %1, ei Btrfs.

Kuvaa ei ladattu eikä isäntää muutettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12218"/>
        <source>Btrfs snapshot inventory is not applicable: the selected target filesystem is %1, not Btrfs.

No snapshot was loaded and the target was not modified.</source>
        <translation>Btrfs-kuvan inventointia ei sovelleta: valittu kohdetiedostojärjestelmä on %1, ei Btrfs.

Kuvaa ei ladattu eikä kohdetta muutettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12232"/>
        <source>Snapshot inventory unavailable</source>
        <translation>Snapshot-inventaariota ei saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12261"/>
        <source>Btrfs snapshot inventory for %1 is deferred until the running privileged operation finishes; the request was not queued.</source>
        <translation>Btrfs-kuvan inventointia %1:lle lykätään, kunnes juoksun etuoikeutettu toiminta päättyy; pyyntö ei ollut jonossa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12267"/>
        <source>Loading Btrfs snapshots after the running operation finishes…</source>
        <translation>Btrfs-kuvien lataaminen suorituksen jälkeen päättyy…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12273"/>
        <source>Loading running-host Btrfs snapshots</source>
        <translation>Ladataan juoksuisäntä Btrfs-kuvia</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12274"/>
        <source>Loading Btrfs snapshots</source>
        <translation>Ladataan Btrfs-kuvia</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12289"/>
        <source>Load running-host Btrfs snapshots</source>
        <translation>Kuorma ajo-ohjaamo Btrfs kuvat</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12290"/>
        <source>Load Btrfs snapshots</source>
        <translation>Lataa Btrfs-kuvia</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12324"/>
        <source>Snapshot inventory failed. See Logs for details.</source>
        <translation>Kuvan inventaario epäonnistui. Katso lisätietoja lokeista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12377"/>
        <source>Loaded %1 running-host Btrfs root snapshot(s) read-only at %2. Select a row and choose Inspect Selected, or double-click a row, for snapshot-specific validation.

No snapshot or host file was modified.</source>
        <translation>Ladattu %1 juoksu-host Btrfs juurikuva [s] luku- vain %2. Valitse rivi ja valitse Inspect Valittu, tai kaksoisnapsauta riviä, jotta tilannekuvakohtainen validointi.

Katsausta tai isäntätiedostoa ei muutettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12379"/>
        <source>Loaded %1 Btrfs root snapshot(s) read-only at %2. Select a row and choose Inspect Selected, or double-click a row, for snapshot-specific validation.

No snapshot or target file was modified.</source>
        <translation>Ladattu %1 Btrfs juurikuva [s] vain %2:ssä. Valitse rivi ja valitse Inspect Valittu, tai kaksoisnapsauta riviä, jotta tilannekuvakohtainen validointi.

Katsausta tai kohdetiedostoa ei muutettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12383"/>
        <source>No Snapper-style Btrfs root snapshots or Boot Bitch rollback backups were found on the running host. The scan was read-only.</source>
        <translation>Suoritusisännästä ei löytynyt Snapper-tyylisiä Btrfs-juurikuvia tai Boot Bitch-kääntövarmuuksia. Skannaus oli vain luettava.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12384"/>
        <source>No Snapper-style Btrfs root snapshots were found on the selected target. The scan was read-only.</source>
        <translation>Valitusta kohteesta ei löytynyt Snapper-tyylisiä Btrfs-juurikuvia. Skannaus oli vain luettava.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12494"/>
        <source>Inspecting running-host snapshot %1</source>
        <translation>Tarkastaa juoksu-isäntä kuva %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12495"/>
        <source>Inspecting snapshot %1</source>
        <translation>Tarkkailukuva %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12503"/>
        <source>Inspect running-host Btrfs snapshot %1</source>
        <translation>Tarkasta juoksu-isäntä Btrfs kuva %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12504"/>
        <source>Inspect Btrfs snapshot %1</source>
        <translation>Tutki Btrfs-kuva %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12513"/>
        <source>Captured: %1

%2</source>
        <translation>Vangittu: %1

%2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12541"/>
        <source>Rolling back snapshot %1</source>
        <translation>Takakuvakuva %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12547"/>
        <source>Preflight snapshot rollback %1</source>
        <translation>%1:n lentoa edeltävä kuvakaappaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12556"/>
        <source>Rollback preflight captured: %1

%2</source>
        <translation>Paluu ennen lentoa kaapattu: %1

%2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12560"/>
        <source>Rollback preflight failed</source>
        <translation>Peruuttaminen ennen lentoa epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12561"/>
        <source>The selected snapshot did not pass the transactional rollback preflight. No rollback was performed.

Review the Snapshots details pane and Logs.</source>
        <translation>Valittu tilannekuva ei läpäissyt ennen lentoa suoritettavaa paluuta. Peruuttamista ei tehty.

Tarkista Snapshots tiedot paneeli ja lokit.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12567"/>
        <source>Confirm transactional snapshot rollback</source>
        <translation>Vahvista tapahtumakuvan tyhjennys</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12568"/>
        <source>Promote snapshot %1 to the normal writable @ root?</source>
        <translation>Edistää kuvan %1 normaali kirjoitettava @ root?</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12569"/>
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
        <location filename="../src/MainWindow.cpp" line="12577"/>
        <location filename="../src/MainWindow.cpp" line="12760"/>
        <source>Continue to Confirmation</source>
        <translation>Jatka vahvistamista</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12587"/>
        <location filename="../src/MainWindow.cpp" line="12770"/>
        <source>Type ROLLBACK to continue</source>
        <translation>Kirjoita ROLLBACK jatkamaan</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12588"/>
        <source>This operation changes the active Btrfs root and rebuilds boot artifacts.

Type ROLLBACK exactly to continue:</source>
        <translation>Tämä operaatio muuttaa aktiivista Btrfs-juurta ja rakentaa uudelleen käynnistysesineitä.

Tyyppi ROLLBACK jatkaa:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12601"/>
        <source>Roll back to Btrfs snapshot %1</source>
        <translation>Palaa Btrfs-kuvaan %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12610"/>
        <source>Rollback finished: %1

%2</source>
        <translation>Palautus valmis: %1

%2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12618"/>
        <source>Rollback completed: %1

%2

Snapshot inventory was cleared because the active root changed. Choose Load Snapshots to refresh it.</source>
        <translation>Palautus valmis: %1

%2

Kuvan inventaario tyhjennettiin, koska aktiivinen juuri muuttui. Valitse Lataa kuvakaappaukset sen päivittämiseksi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12626"/>
        <source>Snapshot rollback complete</source>
        <translation>Snapshot-vastaisku valmis</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12627"/>
        <source>Snapshot %1 was promoted to a writable @ root and the boot stack passed reconciliation.

The previous @ was retained under a timestamped @rollback-before-* name. Reboot using the normal boot path when ready.</source>
        <translation>Snapshot %1 ylennettiin kirjoitettavaksi @ root ja boot pino läpäisi sovinnon.

Edellinen @ säilytettiin aikaleimalla @rollback-ennen-* nimeä. Käynnistä uudelleen normaalilla käynnistyspolulla, kun olet valmis.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12636"/>
        <source>Rollback attempt finished: %1

%2

Cached diagnostics and snapshot inventory were cleared because the rollback request may have changed and/or restored target state.</source>
        <translation>Palautusyritys päättynyt: %1

%2

Akustiset vianmääritykset ja tilannekuvat tyhjennettiin, koska palautuspyyntö on saattanut muuttua ja/tai palauttaa kohdetilan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12644"/>
        <source>Snapshot rollback failed</source>
        <translation>Snapshot-käännös epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12645"/>
        <source>The rollback did not complete successfully. Do not reboot until you review the Snapshots output and Logs. The helper attempts to restore the preserved @ automatically when post-switch validation fails.</source>
        <translation>Se ei onnistunut. Älä käynnistä uudelleen ennen kuin olet tarkistanut Snapshots lähtö ja lokit. Auttaja yrittää palauttaa säilynyt @ automaattisesti, kun post-kytkin validointi epäonnistuu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12661"/>
        <source>Running-host snapshot rollback is only available in Host Maintenance.</source>
        <translation>Running-host-kuvan rollback on saatavilla vain isäntä huolto.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12678"/>
        <location filename="../src/MainWindow.cpp" line="12687"/>
        <source>Running-host snapshot rollback is unavailable.</source>
        <translation>Hyökkäyskuva ei ole käytettävissä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12681"/>
        <source>Running-host snapshot rollback has unknown capability evidence.</source>
        <translation>Hyökkäyskuvassa on tuntemattomia kykyjä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12688"/>
        <source>Run running-host diagnostics in Host Maintenance first.</source>
        <translation>Suorita isännän diagnostiikka.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12691"/>
        <source>Running-host snapshot rollback is available.</source>
        <translation>Running-host kuvakaappaus on saatavilla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12705"/>
        <source>Rollback already staged</source>
        <translation>Perääntyminen on jo järjestetty</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12706"/>
        <source>A running-host rollback to %1 is already staged and takes effect on the next reboot.</source>
        <translation>%1:n juoksuhyökkäys on jo lavastettu ja vaikuttaa seuraavaan uudelleenkäynnistykseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12708"/>
        <source>an earlier snapshot</source>
        <translation>aiempi tilannekatsaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12709"/>
        <source>snapshot %1</source>
        <translation>Kuva %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12710"/>
        <source>Rolling back again preserves the currently running root as a new @rollback-before-* backup and replaces the staged snapshot. The earlier staged root remains on disk but is no longer the recorded undo point.

Continue only if you intend to replace the staged rollback.</source>
        <translation>Takaisin kääntyminen säilyttää käynnissä olevan rootin uutena @rollback-ennen-* varmuuskopiona ja korvaa lavastetun tilannekuvan. Aiemmin lavastettu juuri pysyy levyllä, mutta se ei ole enää tallennettu perumispiste.

Jatka vain, jos aiot korvata lavastetun varauksen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12715"/>
        <source>Replace Staged Rollback</source>
        <translation>Korvaa vaiheittainen palautus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12723"/>
        <source>Rolling back running-host snapshot %1</source>
        <translation>Vierivä takaisin käynnissä-host kuva %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12729"/>
        <source>Preflight running-host rollback %1</source>
        <translation>%1:n lentoa edeltävä varalento</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12738"/>
        <source>Running-host rollback preflight captured: %1

%2</source>
        <translation>Suoritus-host-vastaisku ennen lentoa kaapattu: %1

%2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12742"/>
        <source>Host rollback preflight failed</source>
        <translation>Isännän paluu ennen lentoa epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12743"/>
        <source>The selected snapshot did not pass the running-host rollback preflight. No rollback was performed.

Review the Snapshots details pane and Logs.</source>
        <translation>Valittu kuva ei läpäissyt juoksu-host paluuta ennen lentoa. Peruuttamista ei tehty.

Tarkista Snapshots tiedot paneeli ja lokit.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12749"/>
        <source>Confirm running-host snapshot rollback</source>
        <translation>Vahvista ajo-isäntä-kuvan muutos</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12750"/>
        <source>Roll the running host back to snapshot %1?</source>
        <translation>Viedäänkö juontaja takaisin kuvaan %1?</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12751"/>
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
        <location filename="../src/MainWindow.cpp" line="12771"/>
        <source>This operation stages the selected snapshot as the running host&apos;s next root and rebuilds boot artifacts.

Type ROLLBACK exactly to continue:</source>
        <translation>Tämä operaatio vaiheittaa valitun kuvan juoksuisännän seuraavaksi juureksi ja rakentaa uudelleen käynnistysesineitä.

Tyyppi ROLLBACK jatkaa:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12784"/>
        <source>Roll back the running host to snapshot %1</source>
        <translation>Siirrä käynnissä oleva isäntä takaisin %1-kuvaan</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12793"/>
        <source>Running-host rollback finished: %1

%2</source>
        <translation>Running-host takaisin valmis: %1

%2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12805"/>
        <source>Running-host rollback staged: %1

%2

Snapshot inventory and cached running-host diagnostics were cleared because the next boot will start the promoted root. Reboot when ready; the reboot is never automatic.</source>
        <translation>Running-host-retkellä: %1

%2

Snapshot inventaario ja välimuistissa juokseva isännän diagnostiikka tyhjennettiin, - koska seuraava saapas käynnistää promotoidun juuren. Käynnistä uudelleen, kun olet valmis; uudelleenkäynnistys ei ole koskaan automaattinen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12815"/>
        <source>Running-host rollback staged</source>
        <translation>Running-host-retket</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12816"/>
        <source>Snapshot %1 is staged as the running host&apos;s next root.</source>
        <translation>Snapshot %1 on lavastettu juoksuisännän seuraavaksi juureksi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12817"/>
        <source>Reboot required — the running host will start snapshot %1 after the next reboot. The previous root was retained as an @rollback-before-* undo point and the boot stack passed reconciliation.

Use Reboot Now to reboot immediately, or Later to keep working and reboot manually.</source>
        <translation>Uudelleenkäynnistys vaaditaan ... käynnissä isäntä aloittaa kuvan %1 jälkeen seuraavan uudelleenkäynnistyksen. Edellinen juuri säilytettiin @rollback-ennen-* peruuttaa kohta ja boot pino läpäisi täsmäytys.

Käytä Reboot Nyt uudelleenkäynnistää välittömästi, tai Myöhemmin jatkaa työskentelyä ja uudelleenkäynnistä manuaalisesti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12840"/>
        <source>Running-host rollback attempt finished: %1

%2

Cached running-host diagnostics and snapshot inventory were cleared. Do not reboot until you review the Snapshots output and Logs.</source>
        <translation>Running-host-vastaiskuyritys päättynyt: %1

%2

Valmiina juoksevan isännän diagnostiikka ja tilannekuvat tyhjennettiin. Älä käynnistä uudelleen ennen kuin olet tarkistanut Snapshots lähtö ja lokit.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12847"/>
        <location filename="../src/MainWindow.cpp" line="12850"/>
        <source>Host rollback failed</source>
        <translation>Isännän palautus epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12848"/>
        <source>The running-host rollback did not complete and automatic recovery could not be proven. DO NOT REBOOT. Review the Snapshots output and Logs, and repair the boot stack from another system before rebooting.

The helper output carries HOST_ROLLBACK_RECOVERY and any CRITICAL lines verbatim.</source>
        <translation>Running-host takaisin ei ollut valmis eikä automaattista palautumista voitu todistaa. ÄLÄ REBOOT. Tarkista Snapshots lähtö ja lokit, ja korjata boot pino toisesta järjestelmästä ennen uudelleenkäynnistystä.

Auttimen lähtö kuljettaa HOST ROLLBACK RECOVERY ja kaikki CRITICAL lines sanatarkasti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12851"/>
        <source>The running-host rollback did not complete. The helper reports the preserved @ was restored automatically; do not reboot until you review the Snapshots output and Logs.</source>
        <translation>Running-host takaisin ei valmistunut. Auttaja raportoi säilyneen @:n palautuneen automaattisesti; älä käynnistä uudelleen ennen kuin tarkistat Snapshots-lähdön ja lokit.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12962"/>
        <location filename="../src/MainWindow.cpp" line="12963"/>
        <source>Reboot the running host now?</source>
        <translation>Käynnistätkö juontajan uudelleen?</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12964"/>
        <source>The rolled-back snapshot is already staged and takes effect on the next boot. Rebooting now signs out all users and closes unsaved work.

The host reboots only after this separate confirmation.</source>
        <translation>Kääritty takaisin-kuva on jo järjestetty ja tulee voimaan seuraavan boot. Käynnistän nyt kaikki käyttäjät ja suljen tallentamattomat työt.

Isäntä käynnistyy vasta tämän erillisen vahvistuksen jälkeen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12976"/>
        <source>Rebooting running host</source>
        <translation>Käynnistetään uudelleen käynnissä oleva isäntä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12981"/>
        <source>Reboot the running host</source>
        <translation>Käynnistä käynnissä oleva isäntä uudelleen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12989"/>
        <source>Running-host reboot scheduled; the host is restarting.</source>
        <translation>Suoritus-host uudelleenkäynnistys suunniteltu; isäntä käynnistää uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12996"/>
        <source>Host reboot not scheduled</source>
        <translation>Uudelleenkäynnistystä ei ole suunniteltu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12998"/>
        <source>The running host did not schedule a reboot. The staged rollback and its reminder remain in effect.</source>
        <translation>Juokseva isäntä ei suunnitellut uudelleenkäynnistystä. Lavastettu peruutus ja sen muistutus ovat edelleen voimassa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13016"/>
        <source>Repair → Host file copy</source>
        <translation>Korjaus → Kopioi tiedosto</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13017"/>
        <source>Host → Repair file copy</source>
        <translation>Host → Korjaustiedoston kopio</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13021"/>
        <source>1. Select source paths from repaired system</source>
        <translation>1. Valitse lähdepolut korjatusta järjestelmästä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13022"/>
        <source>2. Choose destination on this host</source>
        <translation>2. Valitse kohde tässä isäntä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13023"/>
        <source>Add File Path…</source>
        <translation>Lisää tiedostopolku…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13024"/>
        <source>Add Folder Path…</source>
        <translation>Lisää kansiopolku…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13026"/>
        <source>Enter an absolute folder path as it appears inside the repaired system.</source>
        <translation>Anna absoluuttinen kansiopolku sellaisena kuin se näkyy korjatussa järjestelmässä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13027"/>
        <source>Choose a host destination folder</source>
        <translation>Valitse kohdekansio</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13028"/>
        <source>Browse…</source>
        <translation>Selaa…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13031"/>
        <source>1. Select source files or folders from this host</source>
        <translation>1. Valitse lähdetiedostot tai kansiot tästä isäntä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13032"/>
        <source>2. Choose destination in repaired system</source>
        <translation>2. Valitse kohde korjatussa järjestelmässä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13035"/>
        <source>Choose one or more source files from the running host.</source>
        <translation>Valitse yksi tai useampi lähdekooditiedosto käynnissä olevasta isäntästä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13036"/>
        <source>Choose a source folder from the running host.</source>
        <translation>Valitse lähdekansio käynnissä olevasta isännästä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13038"/>
        <source>Select a repair target first</source>
        <translation>Valitse korjauskohde ensin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13039"/>
        <source>Choose an absolute destination path inside the repaired system</source>
        <translation>Valitse korjattuun järjestelmään absoluuttinen kohdepolku</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13040"/>
        <source>Browse Target Folders…</source>
        <translation>Selaa kohdekansioita…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13077"/>
        <source>Choose host destination folder</source>
        <translation>Valitse kohdekansio</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13090"/>
        <source>Select a repair target</source>
        <translation>Valitse korjauskohde</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13091"/>
        <source>Select the repair drive in Systems before choosing a destination inside it.</source>
        <translation>Valitse järjestelmän korjausasema ennen kohdetta sen sisällä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13101"/>
        <source>Select repair-system destination</source>
        <translation>Valitse korjausjärjestelmän kohde</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13108"/>
        <source>Choose a destination folder inside the repaired system</source>
        <translation>Valitse kohdekansio korjatun järjestelmän sisällä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13117"/>
        <source>Browse folders from the selected repaired system through temporary read-only mounts. Nothing is written while browsing, and the guarded copy validates the chosen path again before any write.</source>
        <translation>Selaa valitun korjatun järjestelmän kansioita tilapäisten lukulaitteiden avulla. Mitään ei kirjoiteta selailun aikana, ja vartioitu kopio validoi valitun polun uudelleen ennen kirjoittamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13126"/>
        <source>Absolute path inside repaired system (for example /home/user/Recovered)</source>
        <translation>Absoluuttinen polku korjatun järjestelmän sisällä (esim. /koti/käyttäjä/peritty)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13128"/>
        <source>Browse repair folders…</source>
        <translation>Selaa korjauskansioita…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13132"/>
        <source>Clear the destination path.</source>
        <translation>Tyhjentäkää kohde.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13147"/>
        <location filename="../src/MainWindow.cpp" line="13233"/>
        <source>Browse repaired-system folders</source>
        <translation>Selaa korjattuja järjestelmäkansioita</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13160"/>
        <source>Choose a folder from the repaired system</source>
        <translation>Valitse kansio korjatusta järjestelmästä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13169"/>
        <source>The selected repair filesystem is mounted read-only only for the current directory listing. The mount is removed after the request; the host filesystem is never used as the folder tree.</source>
        <translation>Valittu korjaustiedostojärjestelmä on asennettu vain luettavaksi nykyisen kansion listalle. Asennus poistetaan pyynnön jälkeen; isäntätiedostojärjestelmää ei koskaan käytetä kansiopuuna.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13194"/>
        <source>Up</source>
        <translation>Ylös</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13195"/>
        <source>Open Selected</source>
        <translation>Avaa valittu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13197"/>
        <source>Show the parent folder in the repaired system.</source>
        <translation>Näytä kantakansio korjatussa järjestelmässä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13198"/>
        <source>Open the selected repaired-system folder.</source>
        <translation>Avaa valittu korjattu järjestelmäkansio.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13199"/>
        <source>Reload this repaired-system folder through a fresh read-only mount.</source>
        <translation>Lataa tämä korjattu järjestelmäkansio uudelleen vain uuden lukuliitoksen läpi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13206"/>
        <source>Read-only target view; no files are modified.</source>
        <translation>Lue vain kohdenäkymä; tiedostoja ei muuteta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13212"/>
        <source>Choose This Folder</source>
        <translation>Valitse tämä kansio</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13225"/>
        <source>Invalid repair-system path.</source>
        <translation>Virheellinen korjausjärjestelmän polku.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13237"/>
        <source>Unable to read this repaired-system folder. Review Logs for the helper error.</source>
        <translation>Korjattua järjestelmäkansiota ei voitu lukea. Tarkista lokit auttaja virhe.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13267"/>
        <source>Current repaired-system folder: %1</source>
        <translation>Nykyinen korjattu järjestelmäkansio: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13271"/>
        <source>Read-only target view; %1 subfolder(s) found. Nothing was modified.</source>
        <translation>Read-ainoa kohdenäkymä; %1 alikansioita löytyi. Mitään ei muutettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13324"/>
        <location filename="../src/MainWindow.cpp" line="13355"/>
        <location filename="../src/MainWindow.cpp" line="13402"/>
        <source>Invalid repair path</source>
        <translation>Virheellinen korjauspolku</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13325"/>
        <location filename="../src/MainWindow.cpp" line="13356"/>
        <location filename="../src/MainWindow.cpp" line="13403"/>
        <source>Use an absolute path inside the repaired system. Parent-directory escape paths are not accepted.</source>
        <translation>Käytä absoluuttista polkua korjatun järjestelmän sisällä. Vanhempainhakemiston pakoteitä ei hyväksytä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13345"/>
        <source>Repair-system source file</source>
        <translation>Korjausjärjestelmän lähdetiedosto</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13346"/>
        <source>Enter an absolute file path inside the repaired system:</source>
        <translation>Anna absoluuttinen tiedostopolku korjattuun järjestelmään:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13370"/>
        <source>Select host files</source>
        <translation>Valitse isäntätiedostot</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13392"/>
        <source>Repair-system source folder</source>
        <translation>Korjausjärjestelmän lähdekansio</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13393"/>
        <source>Enter an absolute folder path inside the repaired system:</source>
        <translation>Anna korjattuun järjestelmään absoluuttinen kansiopolku:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13417"/>
        <source>Select host folder</source>
        <translation>Valitse kansio</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13457"/>
        <source>Add at least one source file or folder.</source>
        <translation>Lisää vähintään yksi lähdekooditiedosto tai kansio.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13461"/>
        <source>Choose a destination folder.</source>
        <translation>Valitse kohdekansio.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13465"/>
        <source>rsync is required for verified File Copy.</source>
        <translation>rsync vaaditaan todennettu tiedostokopio.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13474"/>
        <source>Repair → Host requires an existing, non-symlink host destination folder.</source>
        <translation>Korjaus → Isäntä vaatii olemassa olevan, ei-symlin isäntäkohdekansion.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13479"/>
        <source>Host → Repair requires a specific absolute path inside the repaired system.</source>
        <translation>Host → Korjaus vaatii tietyn absoluuttisen polun korjattuun järjestelmään.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13556"/>
        <location filename="../src/MainWindow.cpp" line="13612"/>
        <source>Execute a command on the running host as root.</source>
        <translation>Suorita komento käynnissä isännälle juurina.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13568"/>
        <location filename="../src/MainWindow.cpp" line="13768"/>
        <source>Host shell</source>
        <translation>Komentotulkki</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13576"/>
        <source>Run a command on the running host as root (sudo is not needed). Commands are executed directly on the active system. When a command asks a question, Boot Bitch shows it in a popup and sends your answer back to the command; cancelling stops the command. Non-interactive flags such as apt-get -y upgrade remain recommended for unattended runs. Output is kept in this window and in the application log.</source>
        <translation>Suorita käynnissä olevan isännän käsky juurina (suota ei tarvita). Komennot suoritetaan suoraan aktiivisessa järjestelmässä. Kun komento esittää kysymyksen, Boot Bitch näyttää sen ponnahdusikkunassa ja lähettää vastauksesi takaisin komentoon; komento keskeytetään. Ei-interaktiiviset liput, kuten apt-get-y-päivitys, ovat edelleen suositeltavia valvomattomille ajoille. Tulosta säilytetään tässä ikkunassa ja sovelluslokissa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13583"/>
        <source>Command to run on the running host, for example: apt update</source>
        <translation>Suoritettava komento, esimerkiksi: apt-päivitys</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13590"/>
        <source>Run on Host</source>
        <translation>Suorita isäntä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13594"/>
        <source>Commands can modify the running host. Review each command before running it.</source>
        <translation>Komento voi muuttaa käynnissä olevaa isäntää. Tarkista jokainen komento ennen sen suorittamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13608"/>
        <source>Shell</source>
        <translation>Shell</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13609"/>
        <source>Host Shell</source>
        <translation>Isäntäkuori</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5666"/>
        <location filename="../src/MainWindow.cpp" line="13613"/>
        <source>Execute a command inside the selected repair system as root.</source>
        <translation>Suorita komento valitun korjausjärjestelmän sisällä juurina.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6634"/>
        <location filename="../src/MainWindow.cpp" line="20183"/>
        <location filename="../src/MainWindow.cpp" line="20382"/>
        <source>Kernel repair</source>
        <translation>Ytimen korjaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13653"/>
        <source>• %1: %2 changed from &apos;%3&apos; to &apos;%4&apos;</source>
        <translation>• %1: %2 vaihdettu &apos;%3&apos;:stä &apos;%4&apos;:ksi</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13718"/>
        <source>A diagnostic or repair is still running; wait a moment and retry the command.</source>
        <translation>Diagnostiikka tai korjaus on vielä käynnissä; odota hetki ja yritä komentoa uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13724"/>
        <source>Host shell unavailable</source>
        <translation>Komentotulkkia ei ole saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13725"/>
        <source>Chroot shell unavailable</source>
        <translation>Chroot-kuori ei saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13740"/>
        <source>The running-host identity is unresolved; no host shell command was sent.</source>
        <translation>Juoksu-isäntä-identiteetti on selvittämätön; mitään komentoa ei lähetetty.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13741"/>
        <source>The repair target identity is incomplete; no chroot shell command was sent.</source>
        <translation>Korjaa kohde on epätäydellinen; Chroot- komentoa ei lähetetty.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13764"/>
        <source>Running host shell command</source>
        <translation>Suoritetaan isäntäkomentoa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13765"/>
        <source>Running chroot shell command</source>
        <translation>Suoritetaan chroot-komentoa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13812"/>
        <source>the affected repository</source>
        <translation>kyseinen tietokanta</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13818"/>
        <source>Repository metadata changed</source>
        <translation>Versiovaraston metatiedot muuttuneet</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13819"/>
        <source>The repository changed its release metadata:
%1</source>
        <translation>Arkisto muutti julkaisumetadataa:
%1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13825"/>
        <source>The package manager refused the change. Allowing it re-runs the command once with Acquire::AllowReleaseInfoChange=true; the change is accepted for this retry only. Signature, key and package verification remain enforced.

Retry command:
%1</source>
        <translation>Pakettipäällikkö kieltäytyi muutoksesta. Sallien sen uudelleen suorittaa komennon kerran Acquirella::AllowReleaseInfoChange=true; muutos hyväksytään vain tätä uudelleen. Allekirjoitus, avain ja pakettitarkastus on edelleen voimassa.

Uudelleenkomento:
%1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13833"/>
        <source>Allow and Retry</source>
        <translation>Salli ja yritä uudelleen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13852"/>
        <source>Host shell response incomplete</source>
        <translation>Komentotulkin vastaus epätäydellinen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13852"/>
        <source>Chroot shell response incomplete</source>
        <translation>Chroot-kuoren vaste epätäydellinen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13853"/>
        <source>The privileged helper did not return a complete response for this command. The request was treated as failed; review the output and Logs before retrying.</source>
        <translation>Etuoikeutettu apulainen ei palauttanut täydellistä vastausta tähän komentoon. Pyyntöä käsiteltiin epäonnistuneena; tarkista tuloste ja lokit ennen uudelleentarkastelua.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13887"/>
        <location filename="../src/MainWindow.cpp" line="13914"/>
        <source>File Copy unavailable</source>
        <translation>Tiedostokopiota ei ole saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13896"/>
        <source>Previewing file copy</source>
        <translation>Esikatselen tiedostokopiota</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13907"/>
        <source>Preview File Copy</source>
        <translation>Esikatselutiedoston kopiointi</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13921"/>
        <source>Smart destination ownership</source>
        <translation>Älykäs määränpään omistus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13944"/>
        <source>… %1 more item(s)</source>
        <translation>… %1 lisää kohdetta [s)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13949"/>
        <source>Confirm File Copy</source>
        <translation>Vahvista tiedostokopio</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13950"/>
        <source>%1 — copy and verify %2 source item(s)?</source>
        <translation>%1 ... Kopioi ja varmista %2-lähdetieto[s]?</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13952"/>
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
        <location filename="../src/MainWindow.cpp" line="13955"/>
        <source>The repair target stays read-only. Existing same-name files in the host destination may be overwritten. The privileged helper refuses system-critical host destinations.

</source>
        <translation>Korjaaja pysyy lukukunnossa. Olemassa olevat samannimiset tiedostot isäntäkohteessa voidaan korvata. Etuoikeutettu auttaja kieltäytyy järjestelmäkriittisistä isäntäkohteista.

</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13957"/>
        <source>The selected repair filesystem is promoted read-write only after host/target safety checks. Existing same-name target files may be overwritten.

</source>
        <translation>Valittua korjaustiedostojärjestelmää edistetään lukemalla vain isäntä-/kohdeturvallisuustarkastusten jälkeen. Olemassa olevat samannimiset kohdetiedostot voidaan korvata.

</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13959"/>
        <source>SENSITIVE TARGET PATH: this destination can change boot or operating-system files.

</source>
        <translation>SENSIIVINEN TAVOITEPATH: Tämä kohde voi muuttaa käynnistys- tai käyttöjärjestelmätiedostoja.

</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13962"/>
        <source>rsync does not use --delete, so unrelated destination files remain. A second checksum/metadata pass and SHA-256 verification of regular files run after the copy.</source>
        <translation>rsync ei käytä --delete, joten etuyhteydetön kohde tiedostot pysyvät. Toinen tarkistussumma/metadatapassi ja SHA-256-tarkistus säännöllisistä tiedostoista suoritetaan kopion jälkeen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13971"/>
        <source>Copying files</source>
        <translation>Kopioidaan tiedostoja</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13986"/>
        <source>File Copy — %1</source>
        <translation>Tiedoston kopiointi %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13997"/>
        <source>Available — KF6 KAuth linked</source>
        <translation>Saatavilla KF6 KAuth linked</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13999"/>
        <source>Not compiled — optional integration unavailable</source>
        <translation>Ei ole laadittua valinnaista integrointia</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14019"/>
        <location filename="../src/MainWindow.cpp" line="14031"/>
        <source>Available</source>
        <translation>Saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14019"/>
        <location filename="../src/MainWindow.cpp" line="14031"/>
        <source>Missing</source>
        <translation>Puuttuu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14110"/>
        <source>Running Host diagnostics require Host Maintenance. Choose Enter Host Maintenance on the protected running-host card in Systems first.</source>
        <translation>Running Isännän diagnostiikka vaatii ylläpitoa. Valitse Enter Isäntä Kunnossapito suojattu juoksu-host-kortti Systems ensin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14197"/>
        <source>Individual diagnostics have changed. Please re-run all diagnostics.</source>
        <translation>Yksittäiset diagnoosit ovat muuttuneet. Tarkista kaikki diagnostiikat.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14218"/>
        <source>Select a repair target in Systems.</source>
        <translation>Valitse korjauskohde järjestelmistä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14238"/>
        <source>Cached: %1 result</source>
        <translation>Välimuisti: %1:n tulos</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14242"/>
        <source>Cached: %1 — protected running-host result.</source>
        <translation>Välimuisti: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14243"/>
        <source>Re-run Diagnostic</source>
        <translation>Uudelleenajodiagnostiikka</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14247"/>
        <source>Please re-run this diagnostic after the last repair or target configuration change.</source>
        <translation>Suorita tämä vianmääritys uudelleen viimeisen korjaus- tai kohdeasetusmuutoksen jälkeen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14249"/>
        <source>Available — read-only selected repair-system inspection; administrator authorization is already active for this Boot Bitch window.</source>
        <translation>Saatavana vain luettu korjausjärjestelmän tarkastus; järjestelmänvalvojan valtuutus on jo käytössä tässä Boot Bitch-ikkunassa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14250"/>
        <source>Available — read-only selected repair-system inspection; the first root action will request administrator authorization.</source>
        <translation>Saatavana vain lukea valitun korjaus-järjestelmän tarkastus; ensimmäinen juuri toiminto pyytää ylläpitäjän valtuutus.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14253"/>
        <source>Please re-run this diagnostic after the last repair action.</source>
        <translation>Suorita tämä vianmääritys uudelleen viimeisen korjaustoimen jälkeen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14254"/>
        <source>Available — protected running host</source>
        <translation>Saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14510"/>
        <source>Run running-host diagnostics</source>
        <translation>Suorita isännän diagnostiikka</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14511"/>
        <source>Read-only running-host diagnostic</source>
        <translation>Vain ajo-ohjaindiagnostiikka</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14514"/>
        <source>Run All target diagnostics</source>
        <translation>Suorita kaikki kohdediagnostiikka</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14515"/>
        <source>Read-only target diagnostic</source>
        <translation>Vain lukukohdediagnostiikka</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14570"/>
        <source>Starting read-only %1 diagnostic &apos;%2&apos; on %3 (%4).</source>
        <translation>Käynnistetään %1-diagnostiikka &quot;%2&quot; %3:llä (%4).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14581"/>
        <source>Read-only %1 diagnostic &apos;%2&apos; %3.</source>
        <translation>Lue vain %1-diagnostiikka &apos;%2&apos; %3.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14583"/>
        <source>completed</source>
        <translation>valmis</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14583"/>
        <source>failed</source>
        <translation>epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14619"/>
        <location filename="../src/MainWindow.cpp" line="14699"/>
        <source>%1 finished with exit code 0 (success).</source>
        <translation>%1 päättyi lähtökoodiin 0 (menestys).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14634"/>
        <location filename="../src/MainWindow.cpp" line="14751"/>
        <source>Target diagnostic unavailable</source>
        <translation>Kohdediagnoosi ei saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14655"/>
        <source>Discarded the read-only target diagnostic &apos;%1&apos; because the active repair target changed while the request was running.</source>
        <translation>Hylättiin pelkkä lukukohdediagnostiikka &quot;%1,&quot; koska aktiivinen korjauskohde muuttui pyynnön ollessa käynnissä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14715"/>
        <location filename="../src/MainWindow.cpp" line="14750"/>
        <source>Host diagnostic unavailable</source>
        <translation>Isäntädiagnostiikka ei saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14936"/>
        <source>No repair target is ready for diagnostics.</source>
        <translation>Mikään korjauskohde ei ole valmis diagnostiikkaan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14939"/>
        <source>The selected repair target is ready for diagnostics.</source>
        <translation>Valittu korjauskohde on valmis diagnostiikkaan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14944"/>
        <source>Running Host diagnostics require Host Maintenance. Choose Host Maintenance on the protected running-host card in Systems first.</source>
        <translation>Running Isännän diagnostiikka vaatii ylläpitoa. Valitse ensin suojatun juoksu-host-kortin isäntähuolto.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14950"/>
        <source>The running host is not resolved for diagnostics.</source>
        <translation>Juoksevaa isäntää ei ole selvitetty diagnostiikkaan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14953"/>
        <source>The running host is ready for diagnostics.</source>
        <translation>Juokseva isäntä on valmis diagnostiikkaan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14968"/>
        <source>Target diagnostics were invalidated by a repair or target change.</source>
        <translation>Kohdediagnoosi mitätöitiin korjauksella tai kohteen muutoksella.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14972"/>
        <source>Target diagnostic sections were invalidated by a repair action and await regeneration.</source>
        <translation>Kohteen diagnostiset osat mitätöitiin korjaustoimi ja odotetaan uudistumista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14976"/>
        <source>No cached diagnostics belong to the selected target.</source>
        <translation>Mikään välimuistidiagnostiikka ei kuulu valittuun kohteeseen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14980"/>
        <source>No cached diagnostics exist for the selected target.</source>
        <translation>Valitulle kohteelle ei ole välimuistidiagnostiikkaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14987"/>
        <source>Running-host diagnostic sections were invalidated by a repair action and await regeneration.</source>
        <translation>Korjaaminen mitätöitiin ja odotetaan regeneraatiota.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14991"/>
        <source>No cached running-host diagnostics exist.</source>
        <translation>Mitään välimuistia ei ole.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15007"/>
        <source>Automatic diagnostics regeneration is disabled in Settings.</source>
        <translation>Automaattinen diagnostiikan regenerointi ei ole käytössä Asetuksissa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15010"/>
        <source>A diagnostic or repair operation is already in progress.</source>
        <translation>Diagnostinen tai korjaus on jo käynnissä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15020"/>
        <source>Current-scope diagnostic evidence is fresh.</source>
        <translation>Tämänhetkinen diagnostinen näyttö on tuore.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15043"/>
        <source>Unknown repair tool: %1.</source>
        <translation>Tuntematon korjaustyökalu: %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15047"/>
        <location filename="../src/MainWindow.cpp" line="15238"/>
        <location filename="../src/MainWindow.cpp" line="21234"/>
        <source>Select a repair target in Systems first.</source>
        <translation>Valitse ensin järjestelmän korjauskohde.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15050"/>
        <location filename="../src/MainWindow.cpp" line="15082"/>
        <source>Diagnostic evidence belongs to a different target. Run diagnostics for the selected target.</source>
        <translation>Diagnostiset todisteet kuuluvat eri kohteeseen. Suorita valitun kohteen diagnostiikka.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15058"/>
        <source>Target state changed after the last diagnostic or repair action. Please regenerate diagnostics before starting another repair.</source>
        <translation>Kohdetila muuttui viimeisen vianmäärityksen tai korjaustoimen jälkeen. Korjaa diagnostiikka ennen uuden korjauksen aloittamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15062"/>
        <location filename="../src/MainWindow.cpp" line="15101"/>
        <source>Repair tool %1 is available in the selected scope&apos;s cached diagnostics.</source>
        <translation>Korjaamotyökalu %1 on saatavilla valitun sovelluksen välimuistidiagnostiikassa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15153"/>
        <source>Unrecognised running-host default-entry capability evidence: %1. Run running-host diagnostics again, and update Boot Bitch if the helper format changed.</source>
        <translation>Tunnistamaton juoksu-isäntä-oletus-sisääntulokykyä osoittava näyttö: %1. Suorita juoksu-isäntä-diagnostiikka uudelleen ja päivitä Boot Bitch, jos auttajamuoto on muuttunut.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15159"/>
        <source>The running host&apos;s verified default boot entry can be restored or promoted.</source>
        <translation>Juoksevan isäntän todennettu oletuskäynnistys voidaan palauttaa tai edistää.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15165"/>
        <source>The cached running-host diagnostics do not decide whether the host default entry can be restored. Regenerate running-host diagnostics in Host Maintenance, and update Boot Bitch if the helper format changed.</source>
        <translation>Välimuistin juoksu-host diagnostiikka ei päätä, onko isäntä oletus syöte voidaan palauttaa. Uudista juoksu-isäntä-diagnostiikka isäntä huolto, ja päivittää Boot Bitch jos auttaja muodossa muuttunut.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15166"/>
        <source>No running-host default-entry capability evidence is cached. Run read-only running-host diagnostics in Host Maintenance before using Make Default.</source>
        <translation>- Ei ole. Suorita vain luku-asentajan diagnostiikka isännän kunnossapidossa ennen Make Oletuksen käyttöä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15230"/>
        <source>Run the %1 diagnostic for the protected running host before starting this maintenance action.</source>
        <translation>Suorita %1-diagnostiikka suojatulle juoksuisännälle ennen tämän huoltotoimen aloittamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15233"/>
        <source>Required read-only running-host diagnostics are cached for this maintenance action.</source>
        <translation>Vaaditaan vain ajo-ohjaindiagnostiikka.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15242"/>
        <source>Run the selected target diagnostic before starting this repair.</source>
        <translation>Suorita valittu kohdediagnostiikka ennen tämän korjauksen aloittamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15252"/>
        <source>Target state changed after the last diagnostic or repair action. Run the %1 diagnostic for this target before starting this repair.</source>
        <translation>Kohdetila muuttui viimeisen vianmäärityksen tai korjaustoimen jälkeen. Suorita %1-diagnostiikka tälle kohteelle ennen tämän korjauksen aloittamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15253"/>
        <source>Run the %1 diagnostic for this target before starting this repair.</source>
        <translation>Suorita %1-diagnostiikka tälle kohteelle ennen tämän korjauksen aloittamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15256"/>
        <source>Required read-only diagnostic evidence is cached for this repair tool.</source>
        <translation>Vaadittu luku vain diagnostinen näyttö on välimuistissa tätä korjaustyökalua.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15497"/>
        <source>package metadata refreshed — changes were applied</source>
        <translation>package metadatan päivittämiä muutoksia sovellettiin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15498"/>
        <source>package metadata already current — no changes</source>
        <translation>paketti metatiedot jo nykyinen ... ei muutoksia</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15499"/>
        <source>package metadata refresh failed</source>
        <translation>pakettimetadatan päivitys epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15500"/>
        <source>package metadata refresh not checked</source>
        <translation>pakettimetadatan päivitystä ei ole tarkistettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15501"/>
        <source>package metadata refresh did not run</source>
        <translation>paketti metadatan päivitystä ei suoritettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15503"/>
        <source>package configuration completed — changes were applied</source>
        <translation>pakettien konfigurointi valmistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15504"/>
        <source>nothing to configure</source>
        <translation>ei mitään konfiguroitavaa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15505"/>
        <source>package configuration failed</source>
        <translation>pakettien konfigurointi epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15506"/>
        <source>package configuration not checked</source>
        <translation>pakettien kokoonpanoa ei tarkastettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15507"/>
        <source>package configuration did not run</source>
        <translation>pakettien konfigurointia ei suoritettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15509"/>
        <source>broken dependencies repaired — changes were applied</source>
        <translation>rikkinäisiä riippuvuuksia korjattiin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15510"/>
        <source>no broken dependencies</source>
        <translation>ei rikkoutuneita riippuvuuksia</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15511"/>
        <source>broken dependency repair failed</source>
        <translation>rikki riippuvuus korjaus epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15512"/>
        <source>broken dependency repair not checked</source>
        <translation>rikki riippuvuus korjausta ei tarkastettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15513"/>
        <source>broken dependency repair did not run</source>
        <translation>rikki riippuvuus korjaus ei toiminut</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15515"/>
        <source>packages upgraded — changes were applied</source>
        <translation>Paketteja paranneltu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15516"/>
        <source>no packages to upgrade</source>
        <translation>ei päivitettäviä paketteja</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15517"/>
        <source>package upgrade failed</source>
        <translation>pakettipäivitys epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15518"/>
        <source>package upgrade not checked</source>
        <translation>pakettipäivitystä ei valittu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15519"/>
        <source>package upgrade did not run</source>
        <translation>pakettipäivitystä ei suoritettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15521"/>
        <source>DKMS modules rebuilt — changes were applied</source>
        <translation>DKMS-moduuleja rakennettiin uudelleen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15522"/>
        <source>DKMS modules already current — no changes</source>
        <translation>DKMS-moduulit eivät muutu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15523"/>
        <source>DKMS rebuild failed</source>
        <translation>DKMS:n jälleenrakennus epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15524"/>
        <source>DKMS rebuild not checked</source>
        <translation>DKMS-korjausta ei tarkistettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15525"/>
        <source>DKMS rebuild did not run</source>
        <translation>DKMS:n jälleenrakennusta ei toteutettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15527"/>
        <source>display manager repaired — changes were applied</source>
        <translation>Näytönhallinta korjattuja muutoksia sovellettiin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15528"/>
        <source>display manager already correct — no changes</source>
        <translation>Näytä hallinta jo korjata ... ei muutoksia</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15529"/>
        <source>display manager repair failed</source>
        <translation>näytönhallinan korjaus epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15530"/>
        <source>display manager repair not checked</source>
        <translation>näytönhallinan korjausta ei tarkastettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15531"/>
        <source>display manager repair did not run</source>
        <translation>näytönhallinan korjausta ei suoritettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15533"/>
        <source>initramfs rebuilt — images regenerated</source>
        <translation>initramfs regeneroitu kuvat</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15534"/>
        <source>initramfs already current — no changes</source>
        <translation>initramfs jo nykyisellään ei muutoksia</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15535"/>
        <source>initramfs rebuild failed</source>
        <translation>initramfs-korjaus epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15536"/>
        <source>initramfs rebuild not checked</source>
        <translation>initramfs-korjausta ei tarkistettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15537"/>
        <source>initramfs rebuild did not run</source>
        <translation>initramfs:n korjaus ei onnistunut</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15539"/>
        <source>EFI boot path repaired — changes were applied</source>
        <translation>EFI:n käynnistyspolku korjattiin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15540"/>
        <source>EFI boot path already correct — no changes</source>
        <translation>EFI- käynnistyspolku ei muutoksia</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15541"/>
        <source>EFI boot path repair failed</source>
        <translation>EFI- käynnistyspolun korjaus epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15542"/>
        <source>EFI boot path repair not checked</source>
        <translation>EFI-käynnistyspolun korjausta ei tarkistettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15543"/>
        <source>EFI boot path repair did not run</source>
        <translation>EFI- käynnistyspolun korjaus ei toiminut</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15545"/>
        <source>GRUB configuration regenerated — changes were applied</source>
        <translation>GRUB-asetusten regeneroituja muutoksia sovellettiin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15546"/>
        <source>GRUB configuration already current — no changes</source>
        <translation>GRUB: n asetus on jo voimassa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15547"/>
        <source>GRUB regeneration failed</source>
        <translation>GRUB regeneraatio epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15548"/>
        <source>GRUB regeneration not checked</source>
        <translation>GRUB regenerointia ei tarkastettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15549"/>
        <source>GRUB regeneration did not run</source>
        <translation>GRUB:n regenerointia ei suoritettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15551"/>
        <source>extlinux configuration regenerated — changes were applied</source>
        <translation>extlinux-asetusten regeneroituja muutoksia sovellettiin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15552"/>
        <source>extlinux configuration already current — no changes</source>
        <translation>extlinux: n asetus on jo voimassa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15553"/>
        <source>extlinux regeneration failed</source>
        <translation>extlinux regeneraatio epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15554"/>
        <source>extlinux regeneration not checked</source>
        <translation>extlinux regenerointia ei tarkastettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15555"/>
        <source>extlinux regeneration did not run</source>
        <translation>extlinux:n regenerointia ei suoritettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15557"/>
        <source>file system repaired — changes were applied</source>
        <translation>tiedostojärjestelmän korjatut muutokset tehtiin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15558"/>
        <source>no file system errors found — no changes</source>
        <translation>tiedostojärjestelmän virheitä ei löytynyt</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15559"/>
        <source>file system repair failed</source>
        <translation>tiedostojärjestelmän korjaus epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15560"/>
        <source>not all filesystems were checked</source>
        <translation>kaikkia tiedostojärjestelmiä ei tarkastettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15561"/>
        <source>file system repair did not run</source>
        <translation>tiedostojärjestelmän korjausta ei suoritettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15563"/>
        <source>boot stack reconciled — changes were applied</source>
        <translation>boot pino täsmäämään ... muutokset tehtiin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15564"/>
        <source>boot stack already current — no changes</source>
        <translation>boot pino jo nykyinen ... ei muutoksia</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15565"/>
        <source>boot stack repair failed</source>
        <translation>Käynnistyspinon korjaus epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15566"/>
        <source>boot stack not checked</source>
        <translation>boot pinoa ei tarkastettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15567"/>
        <source>boot stack repair did not run</source>
        <translation>boot pino korjaus ei toiminut</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15569"/>
        <source>validation completed — changes were applied</source>
        <translation>Validointia on muutettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15570"/>
        <source>validation is read-only — no changes</source>
        <translation>Validointi on vain luettavaa .</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15571"/>
        <source>validation failed</source>
        <translation>Validointi epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15572"/>
        <source>validation not checked</source>
        <translation>validointia ei tarkastettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15573"/>
        <source>validation did not run</source>
        <translation>validointia ei suoritettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15575"/>
        <source>host boot default updated — changes were applied</source>
        <translation>Käynnistyksen oletus päivitettiin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15576"/>
        <source>host boot default already correct — no changes</source>
        <translation>isäntäkäynnistyksen oletus korjasi jo</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15577"/>
        <source>host boot default update failed</source>
        <translation>Käynnistän oletuspäivitys epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15578"/>
        <source>host boot default not checked</source>
        <translation>isäntäkäynnistyksen oletus ei valittu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15579"/>
        <source>host boot default update did not run</source>
        <translation>Käynnistyksen oletuspäivitys ei toiminut</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15581"/>
        <source>firmware boot order updated — changes were applied</source>
        <translation>firmware boot order updated</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15582"/>
        <source>firmware boot order already current — no changes</source>
        <translation>firmware boot-tilaus on jo käynnissä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15583"/>
        <source>firmware boot order update failed</source>
        <translation>firmware- käynnistystilauksen päivitys epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15584"/>
        <source>firmware boot order not checked</source>
        <translation>firmware- käynnistystilaus ei tarkastettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15585"/>
        <source>firmware boot order update did not run</source>
        <translation>firmware- käynnistystilauksen päivitystä ei suoritettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15587"/>
        <source>firmware boot entry deleted — changes were applied</source>
        <translation>firmware boot -merkintä poistettuja muutoksia sovellettiin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15588"/>
        <source>firmware boot entry was already absent — no changes</source>
        <translation>firmware boot- merkintä oli jo poissa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15589"/>
        <source>firmware boot entry delete failed</source>
        <translation>firmware-käynnistyslopun poisto epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15590"/>
        <location filename="../src/MainWindow.cpp" line="15596"/>
        <source>firmware boot entry not checked</source>
        <translation>firmware boot -merkintää ei tarkastettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15591"/>
        <source>firmware boot entry delete did not run</source>
        <translation>firmware- käynnistyslopun poisto ei toiminut</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15593"/>
        <source>firmware boot entry renamed — changes were applied</source>
        <translation>firmware boot-maininnan nimeksi muutettiin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15594"/>
        <source>firmware boot entry label already correct — no changes</source>
        <translation>firmware boot entry - etiketti jo oikea</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15595"/>
        <source>firmware boot entry rename failed</source>
        <translation>firmware- käynnistysmerkinnän uudelleennimi epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15597"/>
        <source>firmware boot entry rename did not run</source>
        <translation>firmware- käynnistyksen uudelleennimi ei toiminut</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15599"/>
        <source>repair successful — changes were applied</source>
        <translation>Korjaavia muutoksia sovellettiin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15600"/>
        <source>no repair needed — no changes were detected</source>
        <translation>korjausta ei tarvittu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15601"/>
        <source>repair failed</source>
        <translation>korjaus epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15602"/>
        <source>not checked — no repair was attempted</source>
        <translation>ei tarkastettua.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15603"/>
        <source>repair did not run</source>
        <translation>korjausta ei suoritettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15929"/>
        <source>Automatic running-host diagnostics regeneration skipped: Host Maintenance is not active.</source>
        <translation>Automaattinen juoksu-host-diagnostiikka regeneraatio ohitettu: isäntä huolto ei ole aktiivinen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15939"/>
        <source>Regenerating target diagnostics</source>
        <translation>Regeneroidaan kohdediagnostiikka</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15940"/>
        <source>Regenerating running-host diagnostics</source>
        <translation>Regeneroiva ajo-ohjaindiagnostiikka</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15945"/>
        <location filename="../src/MainWindow.cpp" line="16445"/>
        <source>Running…</source>
        <translation>Running…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15995"/>
        <location filename="../src/MainWindow.cpp" line="16175"/>
        <source>%1 diagnostic run: %2</source>
        <translation>%1-diagnostiikka: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8222"/>
        <location filename="../src/MainWindow.cpp" line="15996"/>
        <location filename="../src/MainWindow.cpp" line="16175"/>
        <location filename="../src/MainWindow.cpp" line="16494"/>
        <source>Target</source>
        <translation>Kohde</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5547"/>
        <source>Ordinary repairs need a non-host target; the host uses its own maintenance mode.</source>
        <translation>Tavalliset korjaukset edellyttävät muuta kuin isäntäkohdetta; isäntä käyttää omaa huoltotilaansa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5906"/>
        <source>End the privileged session and close Boot Bitch-owned LUKS mappings.</source>
        <translation>Päätä etuoikeutettu istunto ja sulje Boot Bitch-omistettu LUKS kuvat.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6068"/>
        <source>Restore the host&apos;s default boot entry and make it default.</source>
        <translation>Palauta isännän oletuskäynnistysmerkintä ja tee siitä oletus.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6187"/>
        <source>No repair target committed; rows are inspection-only until Select Target.</source>
        <translation>Korjaa kohdetta ei ole tehty; rivit ovat vain tarkastus, kunnes Valitse kohde.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6260"/>
        <source>Best component visible before privileged probing.</source>
        <translation>Paras komponentti näkyvissä ennen etuoikeutettua etsintää.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6336"/>
        <source>Read or edit the selected target configuration file.</source>
        <translation>Lue tai muokkaa valittua kohdeasetustiedostoa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6422"/>
        <source>Copy the diagnostic results to the clipboard.</source>
        <translation>Kopioi vianmääritystulokset leikepöydälle.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6423"/>
        <source>Save the diagnostic results to a file.</source>
        <translation>Tallenna diagnostiset tulokset tiedostoon.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6784"/>
        <source>Reboot the host after confirmation; all users are signed out.</source>
        <translation>Käynnistä isäntä uudelleen vahvistuksen jälkeen; kaikki käyttäjät ovat kirjautuneet ulos.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6786"/>
        <source>Hide the reminder until Host Maintenance is re-entered.</source>
        <translation>Piilota muistutus, kunnes isäntähuolto palaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6880"/>
        <source>Run a read-only rollback preflight, then promote a writable copy to @.</source>
        <translation>Suorita lukuavainrollback ennen lentoa, sitten promote kirjoitettava kopio @.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7053"/>
        <location filename="../src/MainWindow.cpp" line="7073"/>
        <source>Kernel: %1</source>
        <translation>Ytimen: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7056"/>
        <location filename="../src/MainWindow.cpp" line="7076"/>
        <source>Initramfs: %1</source>
        <translation>Initramfs: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7059"/>
        <source>This entry is the current extlinux default.</source>
        <translation>Tämä merkintä on nykyinen extlinux-oletus.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7139"/>
        <location filename="../src/MainWindow.cpp" line="7319"/>
        <location filename="../src/MainWindow.cpp" line="7396"/>
        <source>Boot menu</source>
        <translation>Saapasvalikko</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7141"/>
        <source>Switch between the UEFI/UKI firmware entries and the read-only boot menu.</source>
        <translation>Vaihda UEFI/UKI-ohjelmistosyötteiden ja vain luettavan käynnistysvalikon välillä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7217"/>
        <location filename="../src/MainWindow.cpp" line="8226"/>
        <source>Save</source>
        <translation>Tallenna</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7312"/>
        <source>Switch the view selector to Boot menu to list the current scope&apos;s boot menu read-only.</source>
        <translation>Vaihda näkymän valitsija Boot-valikkoon luetellaksesi nykyisen komennon käynnistysvalikon.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7525"/>
        <source>No UEFI firmware boot entries were found — this system is BIOS/legacy or lacks efibootmgr, so there is no firmware boot order to manage.</source>
        <translation>Ei UEFI firmware boot- merkintöjä löytyi ... tämä järjestelmä on BIOS/legacy tai puuttuu efibootmgr, joten ei ole firmware boot- järjestyksessä hallita.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7556"/>
        <source>Loading boot menu</source>
        <translation>Ladataan aloitusvalikkoa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7563"/>
        <location filename="../src/MainWindow.cpp" line="7605"/>
        <location filename="../src/MainWindow.cpp" line="7633"/>
        <source>List boot menu</source>
        <translation>Listan käynnistysvalikko</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7577"/>
        <source>The boot menu could not be loaded. See Logs for details.</source>
        <translation>Alkuvalikkoa ei voitu ladata. Katso lisätietoja lokeista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7669"/>
        <source>No boot menu entries were found.</source>
        <translation>Alkuvalikkoa ei löytynyt.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7623"/>
        <location filename="../src/MainWindow.cpp" line="7651"/>
        <location filename="../src/MainWindow.cpp" line="7683"/>
        <source>Loaded %1 boot menu entries.</source>
        <translation>Ladattu %1- käynnistysvalikko.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7747"/>
        <source>Renamed to &quot;%1&quot; — Save applies it.</source>
        <translation>Nimetty &quot;%1&quot;:ksi Tallenna soveltaa sitä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8006"/>
        <source>Boot menu on %1</source>
        <translation>Saapasvalikko %1: llä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8038"/>
        <source>%1 boot menu entries loaded read-only.</source>
        <translation>%1- käynnistysvalikko ladattu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8041"/>
        <source>%1 boot menu viewing is available. Load Menu to list the boot menu read-only.</source>
        <translation>%1-valikon katselu on saatavilla. Lataa valikko luetellaksesi vain aloitusvalikon.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8051"/>
        <source>%1 firmware boot entries loaded. Reorder rows, edit labels or mark an entry for deletion, then Save to apply all changes.</source>
        <translation>%1-ohjelmiston käynnistysmerkinnät ladattu. Tilaa rivit uudelleen, muokkaa tarroja tai merkitse poistomerkintä, sitten Tallenna kaikkien muutosten soveltaminen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8084"/>
        <source>Load the %1 boot menu read-only.</source>
        <translation>Lataa %1-kengänvalikko.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8105"/>
        <source>No pending firmware boot entry changes. Reorder rows, edit labels or mark an entry for deletion first.</source>
        <translation>Firmaohjelmiston käynnistys ei muutu. Tilaa rivit uudelleen, muokkaa tarroja tai merkitse ensin poistomerkintä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8171"/>
        <source>Save firmware boot entry changes unavailable</source>
        <translation>Tallenna firmware boot entry muutoksia ei ole saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8175"/>
        <source>Load boot entries before saving changes.</source>
        <translation>Lataa käynnistysmerkinnät ennen muutosten tallentamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8189"/>
        <source>No pending firmware boot entry changes to save.</source>
        <translation>Ei odottavia firmwaren käynnistysmuutoksia tallennettavaksi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8202"/>
        <source>Reorder firmware boot order to: %1</source>
        <translation>Tilaa firmware- käynnistystilaus uudelleen: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8206"/>
        <source>Delete Boot%1 (%2)</source>
        <translation>Poista Boot%1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8212"/>
        <source>Rename Boot%1 (%2) to &quot;%3&quot;</source>
        <translation>Nimeä Boot%1 (%2) uudelleen &quot;%3&quot;:ksi</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8218"/>
        <source>Save firmware boot entry changes</source>
        <translation>Tallenna firmware- käynnistysmuutokset</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8219"/>
        <source>Apply all pending firmware boot entry changes?</source>
        <translation>Toteuttaa kaikki vireillä olevat firmware-käynnistysmuutokset?</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8220"/>
        <source>%1 disk: %2
Root: %3

%4

Each change runs through the verified helper, which re-checks the entry, writes it, reads it back and restores the previous state on any mismatch.</source>
        <translation>%1-levy: %2
Juuri: %3

%4

Jokainen muutos kulkee todennetun auttaja, joka tarkistaa tietueen uudelleen, kirjoittaa sen, lukee sen takaisin ja palauttaa edellisen tilan mahdolliset eroavuudet.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8222"/>
        <location filename="../src/MainWindow.cpp" line="15996"/>
        <location filename="../src/MainWindow.cpp" line="16175"/>
        <location filename="../src/MainWindow.cpp" line="16494"/>
        <source>Host</source>
        <translation>Palvelin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8245"/>
        <source>Save firmware boot order</source>
        <translation>Tallenna firmware- käynnistystilaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8256"/>
        <source>reorder: %1</source>
        <translation>uudelleenjärjestäminen: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8278"/>
        <source>delete Boot%1: %2</source>
        <translation>Poista Boot%1: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8301"/>
        <source>rename Boot%1: %2</source>
        <translation>Boot%1: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8321"/>
        <source>Firmware boot entry changes saved.</source>
        <translation>Firmware-käynnistyksen muutokset tallennettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8329"/>
        <source>Firmware boot entry changes failed: %1</source>
        <translation>Firmware-käynnistystietojen muutokset epäonnistuivat: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8331"/>
        <source>Firmware boot entry changes failed</source>
        <translation>Firmwaren käynnistysmuutos epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8332"/>
        <source>One or more firmware boot entry changes failed:
%1

Pending changes were kept so you can retry. Full helper output is available in Logs.</source>
        <translation>Yksi tai useampi firmware-ohjelman käynnistysmuutos epäonnistui:
%1

Odotettaessa muutoksia pidettiin, jotta voit yrittää uudelleen. Täysi apuri tuloste on saatavilla lokeissa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8397"/>
        <source>Deletion of Boot%1 cancelled.</source>
        <translation>Boot%1:n poisto peruttu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8404"/>
        <source>Boot%1 marked for deletion — press Save to apply.</source>
        <translation>Boot%1 merkitty poistettavaksi paina Tallenna käyttää.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8465"/>
        <source>Boot%1 rename staged — press Save to apply.</source>
        <translation>Boot%1 uudelleennimeä vaiheistettu paina Tallenna soveltaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8619"/>
        <source>Clear the command output window.</source>
        <translation>Tyhjennä komennon lähtöikkuna.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8672"/>
        <location filename="../src/MainWindow.cpp" line="13064"/>
        <source>Copy staged items and verify them; same-name destinations are overwritten.</source>
        <translation>Kopioi lavastetut kohteet ja varmista ne; samannimiset kohteet korvataan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8756"/>
        <source>Smart mode validates UID/GID mapping and falls back to the destination owner.</source>
        <translation>Älytila validoi UID/GID-kartoituksen ja palaa määränpään omistajalle.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8897"/>
        <source>Save the application log to a file.</source>
        <translation>Tallenna sovellusloki tiedostoon.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8898"/>
        <source>Clear the on-screen action register.</source>
        <translation>Tyhjennä näytön toimintarekisteri.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8950"/>
        <source>Start a new session log file.</source>
        <translation>Käynnistä uusi istuntolokitiedosto.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8951"/>
        <source>Delete the selected session log.</source>
        <translation>Poista valittu istuntoloki.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8952"/>
        <source>Refresh the session log list.</source>
        <translation>Päivitä istuntolokiluettelo.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8977"/>
        <source>Fuzzy filter; a term starting with @ selects a whole diagnostic section.</source>
        <translation>Fuzzy suodatin; termi alkaen @ valitsee koko diagnostinen osio.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8995"/>
        <source>Filter the log by entry kind.</source>
        <translation>Suodata loki maahantulon lajikohtaisesti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9085"/>
        <source>Read-only check of root, /boot, ESP and /home, then per-device repair.</source>
        <translation>Lue vain juuri, /boot, ESP ja / kotiin, sitten per-laite korjaus.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9113"/>
        <source>Regenerate cached diagnostics after repairs or target changes.</source>
        <translation>Uudista välimuistidiagnostiikka korjausten tai kohdemuutosten jälkeen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9209"/>
        <source>Re-probe the available repair capabilities.</source>
        <translation>Suorita uudelleen käytettävissä olevat korjausominaisuudet.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11359"/>
        <source>Select an action below:</source>
        <translation>Valitse alla oleva toiminto:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11373"/>
        <source>Number of the action (0 = Abort)</source>
        <translation>Toimenpiteen määrä (0 = Keskeytys)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11986"/>
        <source>Committed repair target: %1</source>
        <translation>Sitoutunut korjaus tavoite: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12040"/>
        <source>List host root snapshots read-only.</source>
        <translation>Listaa juurikuvat vain luettavaksi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12041"/>
        <source>List root snapshots read-only.</source>
        <translation>Lue vain juurikuvat.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12073"/>
        <source>Validate a rollback plan, promote a writable copy to @, and reconcile the boot stack.</source>
        <translation>Validoidaan varasuunnitelma, edistetään kirjoitettava kopio @, ja sovittaa boot pino.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12074"/>
        <source>Validate a rollback plan, promote a writable copy to @, and reconcile the bootloader.</source>
        <translation>Validoida varasuunnitelma, mainostaa kirjoitettava kopio @, ja sovittaa bootloader.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12426"/>
        <source>Snapshot #%1 · %2</source>
        <translation>Snapshot #%1 · %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13025"/>
        <source>Absolute path inside the repaired system, e.g. /home/user/document.txt.</source>
        <translation>Absoluuttinen polku korjattuun järjestelmään, esim. /home/user/document.txt.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13041"/>
        <source>Browse read-only and choose an absolute destination path.</source>
        <translation>Selaa vain lukea ja valitse ehdoton kohde polku.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13552"/>
        <source>Commands stay disabled until diagnostics generation completes.</source>
        <translation>Komennot pysyvät poissa käytöstä, kunnes diagnostiikka on valmis.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15188"/>
        <source>Restore the host&apos;s %1 and place it first in BootOrder.</source>
        <translation>Palauta isännän %1 ja aseta se ensin BootOrder:ään.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15189"/>
        <source>Restore the host&apos;s %1 and make it the default.</source>
        <translation>Palauta isännän %1 ja tee siitä oletus.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16038"/>
        <source>A privileged operation is already running; the diagnostic was not started. Try again when it finishes.</source>
        <translation>Etuoikeutettu operaatio on jo käynnissä; diagnoosia ei aloitettu. Yritä uudelleen, kun se loppuu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16049"/>
        <location filename="../src/MainWindow.cpp" line="16409"/>
        <source>Host diagnostic refused: %1</source>
        <translation>Isännän vianmääritys hylätty: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16053"/>
        <location filename="../src/MainWindow.cpp" line="16413"/>
        <source>Host Maintenance required</source>
        <translation>Huolto vaaditaan</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16084"/>
        <source>Running diagnostic: %1</source>
        <translation>Suoritusdiagnostiikka: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16188"/>
        <location filename="../src/MainWindow.cpp" line="16423"/>
        <source>No repair target selected</source>
        <translation>Korjaa kohdetta ei ole valittu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16196"/>
        <source>Read target configuration</source>
        <translation>Lue kohteen asetukset</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16200"/>
        <source>Configuration unavailable</source>
        <translation>Asetukset eivät ole saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16211"/>
        <source>Edit target %1</source>
        <translation>Muokkaa kohdetta %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16214"/>
        <source>Edit this target file through the guarded administrator helper. A successful save invalidates cached diagnostics; rerun diagnostics before repair. Generated files such as /boot/grub/grub.cfg may be replaced by the next bootloader update.</source>
        <translation>Muokkaa tätä kohdetiedostoa vartioidun järjestelmänvalvojan avulla. Onnistunut säästää mitätöi välimuistin diagnostiikka; uudelleen diagnostiikka ennen korjausta. Luodut tiedostot, kuten /boot/grub/grub.cfg, voidaan korvata seuraavalla bootloader-päivityksellä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16226"/>
        <source>Save Target File</source>
        <translation>Tallenna kohdetiedosto</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16233"/>
        <location filename="../src/MainWindow.cpp" line="16275"/>
        <source>Write target configuration</source>
        <translation>Kirjoita kohdeasetukset</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16234"/>
        <source>Write the edited contents to %1? This modifies the repair target and invalidates cached diagnostics.</source>
        <translation>Kirjoittaa muokatun sisällön %1? Tämä muuttaa korjauskohdetta ja mitätöi välimuistin diagnostiikan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16245"/>
        <source>Configuration write refused</source>
        <translation>Asetusten kirjoittaminen hylättiin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16246"/>
        <source>The edited content contains NUL bytes; the guarded write refuses it.</source>
        <translation>Editoitu sisältö sisältää NUL tavuja; vartioitu kirjoitus kieltäytyy siitä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16251"/>
        <source>File too large</source>
        <translation>Tiedosto liian suuri</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16252"/>
        <source>The edited file is larger than 1 MiB; the guarded write refuses it. Edit the file from a console instead.</source>
        <translation>Editoitu tiedosto on suurempi kuin 1 MiB; vartioitu kirjoitus kieltäytyy siitä. Muokkaa tiedostoa konsolista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16257"/>
        <source>Configuration write unavailable</source>
        <translation>Asetuksen kirjoitusta ei ole saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16258"/>
        <source>The edited content could not be written to a private temporary file in the application log directory; the write was not started.</source>
        <translation>Muokkattua sisältöä ei voitu kirjoittaa sovelluslokihakemistossa olevaan väliaikaiseen tiedostoon; kirjoitusta ei aloitettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16286"/>
        <source>Configuration write failed</source>
        <translation>Asetuksen kirjoitus epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16306"/>
        <source>Configuration saved</source>
        <translation>Asetukset tallennettu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16390"/>
        <source>The full diagnostic report is already running; results appear when it completes.</source>
        <translation>Koko diagnostinen raportti on jo käynnissä; tulokset näkyvät, kun se valmistuu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16394"/>
        <source>Full diagnostics will run after the current diagnostic finishes.</source>
        <translation>Täydellinen diagnostiikka suoritetaan nykyisten diagnostisten viimeistelyjen jälkeen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16424"/>
        <source>Select a repair target in Systems, or enter Host Maintenance for running-host diagnostics.</source>
        <translation>Valitse järjestelmäkorjauksen kohde tai syötä isännän diagnostiikkaa varten.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16430"/>
        <source>Regenerating diagnostics automatically</source>
        <translation>Regeneroidaan vianmääritys automaattisesti</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16431"/>
        <source>Running all diagnostics</source>
        <translation>Kaikki diagnostiikat</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16496"/>
        <source>Starting all available read-only diagnostics (%1 scope).</source>
        <translation>Aloitetaan kaikki saatavilla olevat luku-vain diagnostiikat (%1-laajuus).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16517"/>
        <source>All available read-only diagnostic summaries generated and cached by diagnostic.</source>
        <translation>Kaikki saatavilla olevat vain diagnostiset yhteenvetoja luotu ja välimuistissa diagnostinen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16518"/>
        <source>Run All diagnostics failed; failed output was not cached as successful diagnostic data.</source>
        <translation>Suorita Kaikki diagnostiikka epäonnistui; epäonnistui ulostulo ei välimuistissa kuin onnistunut diagnostinen tiedot.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16628"/>
        <source>Regenerating diagnostics</source>
        <translation>Regenerointidiagnostiikka</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16638"/>
        <source>Regenerating read-only diagnostics…</source>
        <translation>Regenerointi vain lukudiagnostiikka…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16660"/>
        <source>Read-only diagnostics regenerated automatically.</source>
        <translation>Vain lukudiagnostiikka regeneroitu automaattisesti.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16661"/>
        <source>Automatic diagnostics regeneration did not complete; repair actions remain disabled until diagnostics are regenerated.</source>
        <translation>Automaattinen diagnostiikan regenerointi ei valmistunut; korjaustoimet pysyvät poissa käytöstä kunnes diagnostiikka on uusittu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16744"/>
        <source>Diagnostic results copied</source>
        <translation>Diagnostiset tulokset kopioitu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16769"/>
        <source>Unable to save diagnostics</source>
        <translation>Diagnostiikkaa ei voitu tallentaa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16775"/>
        <source>Diagnostics saved to %1</source>
        <translation>%1: lle tallennetut diagnostiikat</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16784"/>
        <source>Save application log</source>
        <translation>Tallenna sovellusloki</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16793"/>
        <source>Unable to save log</source>
        <translation>Lokia ei voitu tallentaa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16801"/>
        <source>Log saved to %1</source>
        <translation>Loki tallennettu %1: lle</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16858"/>
        <location filename="../src/MainWindow.cpp" line="17578"/>
        <source> · %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16886"/>
        <source>%1 — %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17570"/>
        <source>Current session</source>
        <translation>Nykyinen istunto</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17574"/>
        <source>Current session — not started</source>
        <translation>Nykyinen istunto ei alkanut</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17576"/>
        <source> — %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17628"/>
        <source>Viewing prior session log %1 — generated %2 — read only</source>
        <translation>Katselen aiemman istunnon loki %1 ... luotu %2... Lue vain</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17666"/>
        <source>Note:</source>
        <translation>Huom:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17702"/>
        <location filename="../src/MainWindow.cpp" line="17709"/>
        <source>Delete session log</source>
        <translation>Poista istuntoloki</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17703"/>
        <source>Delete %1 permanently? This cannot be undone.</source>
        <translation>Poista %1 pysyvästi? Tätä ei voi perua.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17710"/>
        <source>Unable to delete %1.</source>
        <translation>%1:ää ei voitu poistaa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18522"/>
        <source>(no matching entries for this filter)</source>
        <translation>(ei täsmääviä merkintöjä tälle suodattimelle)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18845"/>
        <source>Boot Bitch must run from a different booted Linux environment than the system being repaired. Use a Linux live USB or another Linux installation on a different physical drive. The running host is protected from ordinary repair-target selection, but it can be explicitly selected through Host Maintenance for guarded native diagnostics and supported maintenance stages. Diagnostics follow the Systems page: the selected repair drive while Host Maintenance is off, or the protected running host while it is active. The first root action requests administrator authorization once for this Boot Bitch window; File → Lock Administrator Session ends that helper session immediately.</source>
        <translation>Boot Bitch:n on oltava erilaisessa Linux-käyttöympäristössä kuin korjattavassa järjestelmässä. Käytä Linux live USB tai toinen Linux asennus eri fyysisellä asemalla. Juokseva isäntä on suojattu tavalliselta korjauskohteen valinnalta, mutta se voidaan nimenomaisesti valita isännän kunnossapidon kautta vartioituun diagnostiikkaan ja tuettuihin huoltovaiheisiin. Diagnostiikka seuraa Systems-sivua: valittu korjausasema, kun isäntähuolto on pois päältä, tai suojattu juoksu isäntä, kun se on aktiivinen. Ensimmäinen juuritoiminta pyytää järjestelmänvalvojan valtuutusta kerran tähän Boot Bitch-ikkunaan; Tiedosto → Lock Administrator istunto päättyy, että auttaja istunto välittömästi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18860"/>
        <source>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;Boot Bitch %1 &lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18861"/>
        <source>&lt;p&gt;&lt;b&gt;Developer:&lt;/b&gt; CaptainMorgan12&lt;/p&gt;&lt;p&gt;A native Qt 6 Linux recovery and boot-repair utility.&lt;/p&gt;</source>
        <translation>&lt;p&gt;&lt;b&gt;Kehikko:&lt;/b&gt; KapteeniMorgan12&lt;/p&gt;&lt;p&gt;Alkuperäinen Qt 6 Linux-hyödyke- ja käynnistyskorjausapuohjelma.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18863"/>
        <source>&lt;p&gt;&lt;b&gt;Guarded repair mode:&lt;/b&gt; read-only diagnostics can inspect either the protected Running Host or an explicitly selected repair drive. Debian/Ubuntu-family repairs can run through a privileged helper after confirmation; Host Maintenance enables the same supported stages natively on the active system after repeating the host identity and boot-mount checks.&lt;/p&gt;</source>
        <translation>&lt;p&gt;&lt;b&gt;Varattu korjaustila: &lt;/b&gt; lukuavain vianmääritys voi tarkastaa joko suojatun ohjauslaitteen tai erikseen valitun korjausaseman. Debian/Ubuntu-perheen korjaukset voidaan suorittaa etuoikeutetun auttajan kautta vahvistuksen jälkeen; isäntähuolto mahdollistaa samat tuetut vaiheet natiivisti aktiivisessa järjestelmässä sen jälkeen, kun isäntähenkilöllisyys ja käynnistysmomentti on toistettu. &lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18864"/>
        <source>&lt;p&gt;Arch-family systems expose backend profiling and guarded package, initramfs, GRUB and conventional EFI repairs when their transaction-specific preflights pass. Fedora/RPM-family systems expose guarded dnf package transactions, dracut initramfs rebuilds, GRUB2 configuration and boot-code repair, and the systemd GDM display path from the same probe evidence; other RPM-family systems remain diagnostics-only until their transaction backend is implemented.&lt;/p&gt;</source>
        <translation>&lt;p&gt;Arch-perhejärjestelmät paljastavat taustaprofiloinnin ja suojatun paketin, initramfs:n, GRUB:n ja tavanomaisten EFI:n korjaukset, kun niiden tapahtumakohtaiset esilennot päättyvät. Fedora/RPM-perhejärjestelmät paljastavat suojatut dnf-pakettitapahtumat, dracut initramfs:n uudelleenrakennukset, GRUB2:n konfiguroinnin ja käynnistyskoodin korjaamisen sekä järjestelmän GDM-näyttöpolun samasta luotainnäytöstä; muut RPM-perhejärjestelmät pysyvät diagnostiikassa vain siihen asti, kunnes niiden tapahtumatausta on toteutettu.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18865"/>
        <source>&lt;p&gt;The first privileged action authorizes one narrow helper session for the current Boot Bitch window while the Qt GUI remains unprivileged. It can be ended at any time from File → Lock Administrator Session.&lt;/p&gt;</source>
        <translation>&lt;p&gt;Ensimmäinen etuoikeutettu toiminto mahdollistaa yhden kapean auttajan istunnon nykyiseen Boot Bitch-ikkunaan, kun Qt-käyttöliittymä on edelleen vailla etuoikeutta. Se voidaan lopettaa milloin tahansa tiedoston → Lock Administrator Session. &lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18866"/>
        <source>&lt;p&gt;LUKS target unlock, verified bidirectional File Copy, read-only host/repair diagnostics, transactional Btrfs snapshot rollback, offline graphical login recovery, distribution-aware EFI/UKI repair and boot-stack reconciliation are enabled through the guarded helper. Snapshot rollback preserves the previous @ and automatically restores it if critical post-switch reconciliation fails.&lt;/p&gt;</source>
        <translation>&lt;p&gt;LUKS-kohteen lukija, todennettu kaksisuuntainen tiedostokopio, luku-/korjausdiagnostiikka, tapahtumakohtainen Btrfs-kuvakaappausrockback, offline-graafinen kirjautumisten palautus, jakelu-aware EFI/UKI-korjaus ja boot-stack täsmäytys ovat käytössä suojatun apurin kautta. Snapshot-käännös säilyttää edellisen @:n ja palauttaa sen automaattisesti, jos kriittinen kytkinten jälkeinen täsmäytys epäonnistuu. &lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19702"/>
        <location filename="../src/MainWindow.cpp" line="21732"/>
        <source>Restore detected graphical login manager</source>
        <translation>Palauta havaittu graafinen kirjautumishallinta</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19704"/>
        <source>Reinstall EFI bootloader</source>
        <translation>Asenna EFI-käynnistin uudelleen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19718"/>
        <location filename="../src/MainWindow.cpp" line="21160"/>
        <source>Reinstall Alpine GRUB EFI loader</source>
        <translation>Asenna Alpine GRUB EFI -kuormaaja uudelleen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19720"/>
        <location filename="../src/MainWindow.cpp" line="21163"/>
        <source>Regenerate Alpine GRUB configuration</source>
        <translation>Uudista Alpine GRUB -kokoonpano</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19730"/>
        <location filename="../src/MainWindow.cpp" line="21133"/>
        <source>Update GRUB2 configuration</source>
        <translation>Päivitä GRUB2-asetus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19732"/>
        <location filename="../src/MainWindow.cpp" line="21136"/>
        <source>Reinstall GRUB2 bootloader</source>
        <translation>Asenna GRUB2-käynnistin uudelleen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19739"/>
        <location filename="../src/MainWindow.cpp" line="21207"/>
        <source>Rebuild initramfs (dracut)</source>
        <translation>Uudelleenrakentaminen initramfs (dracut)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19767"/>
        <source>%1. %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19769"/>
        <source>Execution order %1: %2</source>
        <translation>Teloitusmääräys %1: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19775"/>
        <source>No Full Repair stages selected — use Configure Plan… or Settings.</source>
        <translation>Ei täyskorjausvaiheita valittu . Käytä Configure Plan… tai Asetukset.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19780"/>
        <source>1 stage selected</source>
        <translation>1 vaihe valittu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19781"/>
        <source>%1 stages selected</source>
        <translation>%1-vaiheet valittu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19813"/>
        <source>No repair stages are selected. Use Configure Plan… to choose the stages Full Repair will run.</source>
        <translation>Korjausvaiheita ei ole valittu. Käytä Configure Plan… valita vaiheet Full Repair suoritetaan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19814"/>
        <source>No selected stages are available. %1</source>
        <translation>Valittuja vaiheita ei ole saatavilla. %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19821"/>
        <source> Please regenerate diagnostics before starting another repair.</source>
        <translation>Korjaa diagnostiikka ennen uuden korjauksen aloittamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19824"/>
        <source>Ready: required cached read-only diagnostics are available for the selected stages. Review them in Diagnostics or Logs before confirming.</source>
        <translation>Valmiina: valittuihin vaiheisiin on saatavilla välimuistia vain lukudiagnostiikkaa varten. Tarkista ne Diagnostiikassa tai lokissa ennen vahvistamista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19830"/>
        <location filename="../src/MainWindow.cpp" line="21839"/>
        <source>No Full Repair stages are selected.</source>
        <translation>Täydellistä korjausvaihetta ei ole valittu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19849"/>
        <source>Disabled in Settings</source>
        <translation>Ei käytössä asetuksissa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19856"/>
        <source>Enabled in Settings</source>
        <translation>Käytössä asetuksissa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19857"/>
        <source>Disabled in Settings — enable it to include this stage</source>
        <translation>Ei käytössä Asetuksissa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19861"/>
        <source>Always preflight</source>
        <translation>Aina ennen lentoa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19892"/>
        <source>Manual recovery tool</source>
        <translation>Manuaalinen talteenottotyökalu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19897"/>
        <source>Unavailable: %1</source>
        <translation>Ei saatavilla: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20046"/>
        <source>Orphaned headers (no matching kernel): %1.</source>
        <translation>Orvot otsikot (ei vastaavaa ydintä): %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20196"/>
        <source>Select a tool to review its workflow.</source>
        <translation>Valitse työkalu tarkistaa sen työnkulku.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20219"/>
        <source>Check the running host or selected repair system&apos;s mounts, filesystem metadata, boot files, mapper consistency and dependency readiness before any repair action. This is an independent safety preflight rather than an optional Full Repair stage.</source>
        <translation>Tarkista käynnissä isäntä tai valittu korjausjärjestelmän liittimet, tiedostojärjestelmän metatiedot, käynnistystiedostot, mapper johdonmukaisuus ja riippuvuus valmius ennen mitään korjausta. Tämä on riippumaton lentoa edeltävä turvallisuusvaihe eikä valinnainen täyskorjausvaihe.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20222"/>
        <source>Validate</source>
        <translation>Validointi</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20224"/>
        <source>Full Repair: automatic safety preflight</source>
        <translation>Täysi korjaus: automaattinen turvallisuus ennen lentoa</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20227"/>
        <source>Complete interrupted dpkg package configuration in the running host or selected repair system. This is the same stage controlled by Settings → Full Repair plan → Complete interrupted package configuration, but it can also be run independently here.</source>
        <translation>Täydellinen keskeytynyt dpkg paketti kokoonpano käynnissä isäntä tai valittu korjausjärjestelmä. Tämä on sama vaihe ohjataan Asetukset → Full Repair Plan → Complete keskeytynyt pakettien kokoonpano, mutta se voidaan myös ajaa itsenäisesti täällä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20229"/>
        <source>Complete Configuration</source>
        <translation>Täydellinen asetukset</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20231"/>
        <location filename="../src/MainWindow.cpp" line="20242"/>
        <location filename="../src/MainWindow.cpp" line="20253"/>
        <location filename="../src/MainWindow.cpp" line="20264"/>
        <location filename="../src/MainWindow.cpp" line="20271"/>
        <location filename="../src/MainWindow.cpp" line="20290"/>
        <location filename="../src/MainWindow.cpp" line="20310"/>
        <location filename="../src/MainWindow.cpp" line="20335"/>
        <location filename="../src/MainWindow.cpp" line="20350"/>
        <location filename="../src/MainWindow.cpp" line="20357"/>
        <location filename="../src/MainWindow.cpp" line="20366"/>
        <source>Full Repair plan: %1</source>
        <translation>Koko korjaussuunnitelma: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20234"/>
        <source>Repair package dependencies in the running host or selected repair system after the mandatory safety preflight. Debian/Ubuntu uses APT; Arch uses one sandbox-preflighted full pacman transaction; Alpine uses apk fix with a simulation first. This maps directly to Settings → Repair broken package dependencies.</source>
        <translation>Korjaa pakettiriippuvuus käynnissä isäntä tai valittu korjausjärjestelmä jälkeen pakollinen turvallisuus ennen lentoa. Debian/Ubuntu käyttää APT:tä; Arch käyttää yhtä hiekkalaatikkoa edeltävää täyttä pacman-tapahtumaa; Alpine käyttää apk-korjausta ensin simulaatiolla. Tämä kartoittaa suoraan Asetukset → Korjaus rikki pakettiriippuvuudet.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20240"/>
        <source>Repair Dependencies</source>
        <translation>Korjaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20245"/>
        <source>Refresh Debian/Ubuntu APT metadata in the running host or selected repair system without upgrading installed packages. Arch and Alpine deliberately refuse a partial metadata-only transaction; their guarded upgrade refreshes the package index itself. This maps directly to Settings → Refresh package metadata.</source>
        <translation>Päivitä Debian/Ubuntu APT -metadata käynnissä olevassa isäntä- tai valitussa korjausjärjestelmässä ilman asennettujen pakettien päivittämistä. Arch ja Alpine kieltäytyvät tarkoituksella osittaisesta metatietotapahtumasta; niiden vartioitu päivitys päivittää itse pakettiindeksin. Tämä kartoittaa suoraan Asetukset → Päivitä paketin metatiedot.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20251"/>
        <source>Refresh Metadata</source>
        <translation>Päivitä metatiedot</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20256"/>
        <source>Simulate the distribution&apos;s package transaction first, inspect proposed removals, then apply a safe upgrade. Debian/Ubuntu chooses an APT mode; Arch runs one full pacman transaction; Alpine runs one guarded apk upgrade transaction. This maps directly to Settings → Upgrade installed packages.</source>
        <translation>Simuloi jakelun pakettitapahtuma ensin, tarkasta ehdotetut poistot ja tee sitten turvallinen päivitys. Debian/Ubuntu valitsee APT-tilan; Arch ajaa yhden täyden pacman-tapahtuman; Alpine käyttää yhtä suojattua apk-päivitystapahtumaa. Tämä kartat suoraan Asetukset → Päivitys asennettu paketteja.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20262"/>
        <source>Simulate and Upgrade</source>
        <translation>Simuloi ja paranna</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20267"/>
        <source>Rebuild out-of-tree kernel modules for kernels installed in the running host or selected repair system. The helper refuses this action when DKMS is not installed in the selected system.</source>
        <translation>Rakennetaan uudelleen out-of-puun ytimen moduulit ytimiä asennettu käynnissä isäntä tai valittu korjausjärjestelmä. Auttaja kieltäytyy tästä, kun DKMS:ää ei ole asennettu valittuun järjestelmään.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20269"/>
        <location filename="../src/MainWindow.cpp" line="21728"/>
        <source>Rebuild DKMS</source>
        <translation>Uudelleenrakentaminen DKMS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20274"/>
        <source>Restore the display manager identified from the running host or selected repair system&apos;s boot evidence and configuration (for example SDDM, GDM/GDM3, LightDM, or another systemd manager), set graphical.target as the default, and repair display-manager.service. </source>
        <translation>Palauta näytön hallinta tunnistettu käynnissä isäntä tai valitun korjausjärjestelmän käynnistysnäyttö ja kokoonpano (esim. DSDM, GDM / GDM3, LightDM, tai muu järjestelmällinen manager), asettaa graafisesti. kohde oletuksena ja korjata näytön hallinta. Palvelu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20277"/>
        <source>The detected backend is GDM (Fedora naming); its configuration lives in /etc/gdm/custom.conf. </source>
        <translation>Havaittu taustaosa on GDM (Fedora nameing); sen kokoonpano elää /etc/gdm/custom.conf.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20280"/>
        <source>The detected backend is GDM3; its configuration lives in /etc/gdm3/daemon.conf. </source>
        <translation>Havaittu taustaosa on GDM3; sen kokoonpano elää /etc/gdm3/daemon.conf.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20283"/>
        <location filename="../src/MainWindow.cpp" line="20329"/>
        <source>The detected backend is %1. </source>
        <translation>Havaittu taustaosa on %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20285"/>
        <source>When OpenRC is the detected service manager, the same action restores the detected manager&apos;s default runlevel symlink without starting it. Boot Bitch deliberately does not start a graphical session inside a repair chroot or host helper; use Diagnostics to inspect installed packages, service configuration, and recent boot/journal evidence first when graphical boot fails.</source>
        <translation>Kun OpenRC on havaittu huoltopäällikkö, sama toiminto palauttaa todetun managerin oletusjuoksutason symlinkin käynnistämättä sitä. Boot Bitch ei tarkoituksellisesti aloita graafista istuntoa korjauschroot- tai isäntäavustimen sisällä; käyttää Diagnostics-järjestelmää tarkastaakseen asennetut paketit, palvelun konfiguraatio ja viimeaikaiset boot/journal-todisteet ensin, kun graafinen käynnistys epäonnistuu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20288"/>
        <source>Restore Graphical Login</source>
        <translation>Palauta graafinen kirjautuminen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20302"/>
        <source>Rebuild initramfs images for the running host or selected repair system only after mapper and crypttab consistency checks pass. The helper uses update-initramfs on Debian/Ubuntu, transaction-specific mkinitcpio trials on Arch, and mkinitfs trials on Alpine.</source>
        <translation>Rakenna initramfs-kuvat uudelleen käynnissä olevaan isäntään tai valittuun korjausjärjestelmään vasta kun mapper- ja cryptab-yhteensopivuustarkistus on läpäissyt. Apuri käyttää update-initramfs:ää Debian/Ubuntussa, tapahtumakohtaisia mkinitcpio-kokeita Archilla ja mkinitfs-kokeita Alpinella.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20308"/>
        <source>Rebuild Initramfs</source>
        <translation>Initramfsin uudelleenrakentaminen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20323"/>
        <source>The detected syslinux/extlinux layout is repaired through its configuration and boot code, never through EFI. </source>
        <translation>Havaittu syslinux/extlinux-asettelu korjataan sen konfiguraatiolla ja käynnistyskoodilla, ei koskaan EFI:n kautta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20333"/>
        <source>Repair EFI / UKI</source>
        <translation>Korjaus EFI / UKI</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20338"/>
        <source>GRUB2 configuration</source>
        <translation>GRUB2-konfiguraatio</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20342"/>
        <source>Regenerate GRUB2</source>
        <translation>Regeneroi GRUB2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20345"/>
        <source>Regenerate the running host or selected repair system&apos;s GRUB menu/configuration after the mandatory safety preflight. Debian/Ubuntu uses update-grub; Arch and Alpine use grub-mkconfig with an isolated trial output, an entry-preserving guard and a rollback when a previous menu entry would be lost. This does not reinstall EFI loader files; use EFI / UKI bootloader when the firmware loader itself needs repair.</source>
        <translation>Uudista ajon isäntä tai valitun korjausjärjestelmän GRUB-valikko/konfiguraatio pakollisen lentoa edeltävän turvallisuuslennon jälkeen. Debian/Ubuntu käyttää update-grub:ää; Arch and Alpine use grub-mkconfig:ää, jossa on erillinen koetuloste, sisäänpääsyä säilyttävä suoja ja varauloskäynti, kun edellinen valikkomerkintä katoaa. Tämä ei asenna EFI-kuormaajatiedostoja uudelleen; käytä EFI / UKI-käynnistintä, kun firmware-kuormaaja itse tarvitsee korjausta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20347"/>
        <source>Regenerate GRUB</source>
        <translation>Regeneroi GRUB</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20353"/>
        <source>Regenerate the running host or selected repair system&apos;s extlinux bootloader configuration after the mandatory safety preflight. The detected syslinux/extlinux backend uses update-extlinux with an entry-preserving guard and a boot-artifact backup; existing boot entries are never dropped. This regenerates the configuration only and does not reinstall bootloader files.</source>
        <translation>Uudista käyttöisännän tai valitun korjausjärjestelmän extlinux-käynnistinkokoonpano pakollisen turvalennon jälkeen. Havaittu syslinux/extlinux backend käyttää update-extlinux sisääntulosuojalla ja boot-artifact-varmistuksella; olemassa olevia käynnistysmerkintöjä ei koskaan pudoteta. Tämä uusii vain konfiguraation eikä asenna uudelleen bootloader-tiedostoja.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20355"/>
        <source>Regenerate extlinux</source>
        <translation>Regeneroi extlinux</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20364"/>
        <source>Check File Systems</source>
        <translation>Tarkista tiedostojärjestelmät</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20378"/>
        <source>Reconcile Boot Stack</source>
        <translation>Reconcile Boot Stack</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20380"/>
        <source>Full Repair plan: Manual recovery tool</source>
        <translation>Koko korjaussuunnitelma: Manuaalinen palautustyökalu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20387"/>
        <source>Repair kernels</source>
        <translation>Korjaamon siemenet</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20391"/>
        <source>Unknown repair tool</source>
        <translation>Tuntematon korjaustyökalu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20392"/>
        <source>No repair workflow is registered for this item.</source>
        <translation>Tätä kohtaa varten ei ole rekisteröity korjaustyötä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20393"/>
        <source>Unavailable</source>
        <translation>Ei saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20395"/>
        <source>Full Repair plan: unavailable</source>
        <translation>Täydellinen korjaussuunnitelma: ei saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20419"/>
        <source>
Reuse: the bootloader repair from this session will be reused (the duplicate bootloader rebuild and GRUB regeneration are skipped).</source>
        <translation>
Uudelleenkäyttö: bootloader korjaus tästä istunnosta käytetään uudelleen (kaksinkertainen bootloader jälleenrakentaminen ja GRUB regeneration ohitetaan).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20429"/>
        <source>
Unavailable: %1</source>
        <translation>
Ei saatavilla: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21071"/>
        <source> This stage is off by default: select it here to include it in Full Repair.</source>
        <translation>Tämä vaihe on pois päältä oletuksena: valitse se tästä sisällyttää sen Full Repair.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21143"/>
        <source>Repair broken package dependencies (all detected package managers)</source>
        <translation>Korjaa rikkinäiset pakettiriippuvuudet (kaikki havaitut pakettipäälliköt)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21146"/>
        <source>Upgrade installed packages (all detected package managers)</source>
        <translation>Asennettujen pakettien päivittäminen (kaikki havaitut pakettien hallinnoijat)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21153"/>
        <source>Repair Alpine packages (apk fix)</source>
        <translation>Korjaa Alpine paketteja (apk fix)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21156"/>
        <source>Upgrade Alpine packages (apk upgrade)</source>
        <translation>Päivitä Alpine paketteja (apk päivitys)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21172"/>
        <source>Repair Arch package dependencies (full pacman transaction)</source>
        <translation>Korjaa Arch-paketin riippuvuudet (täydellinen pacman-tapahtuma)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21175"/>
        <source>Upgrade Arch packages (full pacman transaction)</source>
        <translation>Päivitä Arch-paketit (täydellinen pacman-tapahtuma)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21183"/>
        <source>Repair Fedora packages (dnf)</source>
        <translation>Korjaa Fedora-paketit (dnf)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21186"/>
        <source>Refresh package metadata (dnf makecache)</source>
        <translation>Päivitä paketin metatiedot (dnf makecache)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21189"/>
        <source>Upgrade Fedora packages (dnf upgrade)</source>
        <translation>Päivitä Fedora paketteja (dnf päivitys)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21194"/>
        <source>Restore detected graphical login manager (OpenRC runlevel)</source>
        <translation>Palauta havaittu graafinen kirjautumisohjelma (OpenRC juoksutaso)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21202"/>
        <source>Restore detected graphical login manager (%1)</source>
        <translation>Palauta havaittu graafinen kirjautumishallinta (%1)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21211"/>
        <source>Rebuild initramfs (mkinitfs)</source>
        <translation>Rakenna initramfs uudelleen (mkinitfs)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21215"/>
        <source>Rebuild initramfs (mkinitcpio)</source>
        <translation>Uudelleenrakentaminen initramfs (mkinitcpio)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21220"/>
        <location filename="../src/MainWindow.cpp" line="21754"/>
        <source>Regenerate extlinux configuration</source>
        <translation>Luo extlinux- asetus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21241"/>
        <source>The running host is protected and cannot be repaired from itself.</source>
        <translation>Juokseva isäntä on suojattu eikä sitä voi korjata itsestään.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21245"/>
        <source>The selected Linux root is still LUKS-encrypted. Unlock it first, refresh devices, then select the mapped filesystem.</source>
        <translation>Valittu Linux root on edelleen LUKS-salattu. Avaa se ensin, virkistä laitteet, valitse sitten kartoitettu tiedostojärjestelmä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21249"/>
        <source>No mountable Linux root filesystem has been identified on the selected target.</source>
        <translation>Valitussa kohteessa ei ole löytynyt asennettavissa olevaa Linuxin juuritiedostojärjestelmää.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21260"/>
        <source>pkexec/Polkit is required to authorize repair operations.</source>
        <translation>Pkexec/Polkit on annettava korjausten käyttöön.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21280"/>
        <source>The running host disk and root component could not be resolved.</source>
        <translation>Juoksevaa isäntälevyä ja juurikomponenttia ei voitu ratkaista.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21287"/>
        <source>The selected host identity is no longer marked as the protected running system. Refresh devices before retrying.</source>
        <translation>Valittua isäntätunnusta ei ole enää merkitty suojatuksi ajojärjestelmäksi. Päivitä laitteita ennen kuin yrität uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21297"/>
        <source>pkexec/Polkit is required to authorize host maintenance.</source>
        <translation>Pkexec/Polkit on annettava käyttöön isäntähuolto.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21302"/>
        <source>Running host is ready for explicit guarded maintenance.</source>
        <translation>Juokseva isäntä on valmiina vartioituun huoltoon.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21310"/>
        <source>Select Host Maintenance on the protected running-host card first.</source>
        <translation>Valitse ensin suojatun juoksu-host-kortin isäntähuolto.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21323"/>
        <source>Confirm changes to the running host</source>
        <translation>Vahvista muutokset käynnissä olevaan isäntään</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21324"/>
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
        <location filename="../src/MainWindow.cpp" line="21331"/>
        <source>Confirm changes to the selected repair system</source>
        <translation>Vahvista muutokset valittuun korjausjärjestelmään</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21332"/>
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
        <location filename="../src/MainWindow.cpp" line="21341"/>
        <source>Run Repair</source>
        <translation>Suorita korjaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21360"/>
        <source>Repair unavailable</source>
        <translation>Korjausta ei saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21432"/>
        <source>Repair: %1…</source>
        <translation>Korjaus: %1…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21606"/>
        <source>%1 failed</source>
        <translation>%1 epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21607"/>
        <source>The repair action failed. Review the captured helper output in Logs for the failing stage and its reason.</source>
        <translation>Korjaaminen epäonnistui. Tarkista kaapattu auttaja lähtö lokeissa vika vaiheessa ja sen syy.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21659"/>
        <source>A file system check and repair is already running. Wait for it to finish before starting another repair; this request was not queued.</source>
        <translation>Tiedostojärjestelmän tarkistus ja korjaus on jo käynnissä. Odota, että se päättyy ennen kuin aloitat toisen korjauksen; tämä pyyntö ei ollut jonossa.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21663"/>
        <source>File system repair in progress</source>
        <translation>Tiedostojärjestelmän korjaus käynnissä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21681"/>
        <location filename="../src/MainWindow.cpp" line="21846"/>
        <source>Repair diagnostics required</source>
        <translation>Tarvittava korjausdiagnostiikka</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8529"/>
        <location filename="../src/MainWindow.cpp" line="21685"/>
        <location filename="../src/MainWindow.cpp" line="21791"/>
        <source>Repair tool unavailable</source>
        <translation>Korjaustyökalua ei saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5301"/>
        <source>Device: %1 · File system: %2 · Mount: %3 · Check tool: %4</source>
        <translation>Laite: %1 · Tiedostojärjestelmä: %2 · Mount: %3 · Tarkista työkalu: %4</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6061"/>
        <location filename="../src/MainWindow.cpp" line="9797"/>
        <source>Host Maintenance — run guarded repair stages against the running system.</source>
        <translation>Isäntä Huolto Aja vartioidut korjausvaiheet vastaan ajojärjestelmä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6088"/>
        <source>Click a column header to sort by it; click again to reverse the order.</source>
        <translation>Napsauta sarakkeen otsikkoa lajitellaksesi sen; klikkaa uudelleen peruuttaaksesi tilauksen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6179"/>
        <source>Establish the privileged helper session for the current scope now.</source>
        <translation>Auttajaistunto on järjestettävä nyt.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6401"/>
        <source>Run the selected read-only diagnostic for the current scope.</source>
        <translation>Suorita valittu luku vain vianmääritys nykyisen soveltamisalan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7079"/>
        <source>This entry is the current GRUB legacy default.</source>
        <translation>Tämä merkintä on nykyinen GRUB perintö oletus.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7111"/>
        <source>Manage UEFI firmware boot entries. Entries are listed read-only; Move Up/Move Down reorder them in the table only, Delete Selected marks an entry for deletion and double-clicking the Label column stages a rename. Save commits all staged changes (reorder, delete and rename) through the verified helper. The active (BootCurrent) entry can never be deleted or renamed. Changes are verified and rolled back on any mismatch.

Firmware boot entries live in the running host&apos;s UEFI NVRAM (there is only one firmware). In Host Maintenance you manage the running host&apos;s own entries; with a repair target selected you manage that drive&apos;s entries, which are the NVRAM entries whose PARTUUID points to that drive&apos;s EFI system partition. A repair target has no firmware of its own.

These are the UEFI firmware entries; the GRUB boot menu (the list of kernels/OSes inside GRUB) is separate and is regenerated by the GRUB or boot-stack (reconciliation) repair.

Class:
• host — the entry boots the running host&apos;s own EFI system partition.
• target — the entry boots the selected repair target&apos;s EFI system partition (shown in Host Maintenance only when a target is selected).
• foreign — the entry points to a different drive or EFI partition (another OS, a USB stick, or an old disk).
• unknown — the entry&apos;s device path could not be resolved to a GPT partition (typically firmware built-ins such as the setup menu).

Common loader names:
• EFI/BOOT/BOOTX64.EFI — the removable-media fallback; the default path firmware uses to boot a USB drive, not tied to one OS.
• iPXE.efi — the WebFAI network-recovery loader (TUXEDO OS); booting it starts the network recovery/install environment.
• oem##.efi — manufacturer/OEM boot applications (recovery or diagnostics); not part of the OS boot chain and left untouched.
• EFI/BOOT/TUX.EFI — the TUXEDO OS unified kernel image (UKI).

Distribution loaders: Arch EFI/arch/grubx64.efi, Alpine (EFI) EFI/alpine/grubx64.efi, Fedora EFI/fedora/shimx64.efi (then grubx64.efi), Ubuntu/Debian EFI/ubuntu/shimx64.efi; shim*.efi is the Secure-Boot shim. Debian 4.0 &quot;Etch&quot; is BIOS-only and has no .efi boot entries.

Boot menu (read-only):
Use the view selector above to switch between the UEFI/UKI firmware entries and the boot menu. The boot menu lists the kernels/OS entries of the system&apos;s bootloader exactly as they appear in its configuration, in file order, with the current default entry marked with ✓. On GRUB systems this is the GRUB menu (grub.cfg); on BIOS/extlinux systems it is the extlinux/syslinux menu (/boot/extlinux.conf or a syslinux.cfg); on Debian Etch (GRUB legacy 0.97) it is the menu.lst menu (/boot/grub/menu.lst). This view is strictly read-only — it never regenerates or edits the bootloader configuration.</source>
        <translation>Hallitse UEFI firmware- käynnistysmerkintää. Merkinnät on lueteltu vain luettuna; Siirrä ylös/siirry alas tilata ne vain taulukossa, Poista Valitut merkit merkintä poistaminen ja kaksoisnapsautuksella Label sarakkeen vaiheissa uudelleennimi. Tallenna toimittaa kaikki vaiheitetut muutokset (uudelleenjärjestäminen, poistaminen ja uudelleennimeäminen) todennetun auttimen kautta. Aktiivista (BootCurrent) merkintää ei voi koskaan poistaa tai nimetä uudelleen. Muutokset varmennetaan ja palautetaan mahdollisiin epäsuhtiin.

Firmware-saappaat elävät käynnissä olevan isännän UEFI NVRAM (on vain yksi firmware). Host Maintenancessa hoidat juoksuisännän omia tietueita; valitsemallasi korjauskohteella hallitset kyseisen aseman tietueita, jotka ovat NVRAM-syötteitä, joiden PARTUUID osoittaa kyseisen aseman EFI-järjestelmän osioon. Korjauskohteella ei ole omia tietoja.

Nämä ovat UEFI-ohjelmiston merkinnät; GRUB-kengänvalikko (luettelo siemenistä/OS:istä GRUB:n sisällä) on erillinen, ja se regeneroidaan GRUB:n tai boot-stackin avulla.

Luokka:
• isännöi sisääntuloa, joka käynnistää juoksuisännän oman EFI-järjestelmän osion.
• kohde ... saat valitun korjauskohteen EFI-järjestelmän osion (esiintyy isäntähuollossa vain, kun kohde on valittu).
• ulkomainen ... tulopisteet eri asemaan tai EFI osioon (toinen käyttöjärjestelmä, USB-tikku tai vanha levy).
• tuntematon

Yleiset kuormaajan nimet:
• EFI/BOOT/BOOTX64.EFI
• iPXE.efi
• oem#.efi
• EFI/BOOT/TUX.EFI

Jakelukuormaajat: Arch EFI/arch/grubx64.efi, Alpine (EFI) EFI/alpine/grubx64.efi, Fedora EFI/fedora/shimx64.efi (ns. grubx64.efi), Ubuntu/Debian EFI/ubuntu/shimx64.efi; shim*.efi on Secure-Boot shim. Debian 4.0 &quot;Etch&quot; on BIOS-vain, eikä sillä ole .efi-saappaat.

Saapasvalikko (vain luku):
Käytä yllä olevaa näkymän valitsinta vaihtaaksesi UEFI/UKI-ohjelmistosyötteiden ja käynnistysvalikon välillä. Käynnistysvalikko listaa järjestelmän bootloaderin ytimiä/OS-syötteitä juuri niin kuin ne näkyvät sen kokoonpanossa, tiedostojärjestyksessä, ja nykyinen oletusmerkintä on merkitty ✓:llä. GRUB-järjestelmissä tämä on GRUB-valikko (grub.cfg); BIOS/extlinux-järjestelmissä se on extlinux/syslinux-valikko (/boot/extlinux.conf tai syslinux.cfg); Debian Etch (GRUB-perintö 0.97), se on valikko.lst-valikko (/boot/grub/menu.lst). Tämä näkymä on tiukasti lukea vain ... Se ei koskaan uusi tai muokata bootloader asetukset.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7664"/>
        <source>No boot menu configuration was found on this drive. The system may have no GRUB, extlinux/syslinux or GRUB legacy boot menu, or its configuration lives elsewhere.</source>
        <translation>Asemasta ei löytynyt käynnistysvalikon asetuksia. Järjestelmässä ei saa olla GRUB:ää, extlinux:ää/syslinux:ää tai GRUB:n perinteistä käynnistysvalikkoa tai sen kokoonpano elää muualla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9702"/>
        <source>Inspect this partition/volume. %1</source>
        <translation>Tarkista tämä osio/tilavuus. %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9707"/>
        <source>Select this physical drive as the repair target. %1</source>
        <translation>Valitse tämä fyysinen asema korjauskohteeksi. %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9852"/>
        <source>Exit Host Maintenance — return to ordinary repair-target mode.</source>
        <translation>Poistu isäntä huolto ... palata tavalliseen korjaus-kohdetilaan.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10094"/>
        <source>Unlock %1 via cryptsetup through the privileged helper; the passphrase is never logged.</source>
        <translation>Avaa %1 salausjärjestelmän kautta etuoikeutetun auttimen kautta; salauslausetta ei koskaan kirjata.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10097"/>
        <source>An unlocked Linux filesystem is already visible; Boot Bitch reuses the existing mapper.</source>
        <translation>Avaamaton Linux-tiedostojärjestelmä on jo näkyvissä; Boot Bitch käyttää olemassa olevaa karttaa uudelleen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10983"/>
        <source>Privileged request &apos;%1&apos; is queued behind the running &apos;%2&apos; and will start when it finishes.</source>
        <translation>Etuoikeutettu pyyntö &apos;%1&apos; on jonossa juoksun &apos;%2&apos; takana ja alkaa, kun se päättyy.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11012"/>
        <source>Privileged request &apos;%1&apos; waited for the running &apos;%2&apos; but it did not finish in time; the request was refused.</source>
        <translation>Etuoikeutettu pyyntö &apos;%1&apos; odotti käynnissä olevaa &apos;%2&apos; mutta se ei päättynyt ajoissa; pyyntö hylättiin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11015"/>
        <source>ERROR: Privileged request &apos;%1&apos; was refused because &apos;%2&apos; is still running after the queue wait limit; wait for it to finish and retry, or cancel it.
</source>
        <translation>VIRHE: Etuoikeutettu pyyntö &apos;%1&apos; hylättiin, koska &apos;%2&apos; on edelleen käynnissä jonotusrajan jälkeen; odota, että se päättyy ja yrittää uudelleen, tai peruuttaa sen.
</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11887"/>
        <source>Running host selected for guarded maintenance: %1 · Root component: %2</source>
        <translation>%1 · Juurikomponentti: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11894"/>
        <source> · Automatically detected system component: %1</source>
        <translation>· Automaattisesti havaittu järjestelmän komponentti: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11912"/>
        <source>Running host selected for the Host Shell: %1 · Root component: %2</source>
        <translation>Isäntä Shell: %1 · Juurikomponentti: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11940"/>
        <source>Row selection is inspection only; press Select Target to commit a repair drive.</source>
        <translation>Rivivalinta on vain tarkastus; paina Valitse Target korjataksesi aseman.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11941"/>
        <source>Committed repair target: Repair, Diagnostics, Snapshots and File Copy target this drive until Select Target picks another. %1</source>
        <translation>Sitoutunut korjaus tavoite: Korjaus, Diagnostiikka, Snapshots ja Tiedosto Kopioi tämän aseman kunnes Valitse Target valitsee toisen. %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13129"/>
        <source>Open a read-only folder view; the target is unmounted after each listing.</source>
        <translation>Avaa vain lukukansion näkymä; kohde ei ole asennettu jokaisen listauksen jälkeen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14241"/>
        <source>Cached %1 result; use Re-run for fresh data.</source>
        <translation>Cached %1 tulos; käytä Re-run tuoreita tietoja.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21066"/>
        <source>No cached capability evidence yet; your saved selection is kept and re-checked after diagnostics.</source>
        <translation>Säästetty valintasi säilytetään ja tarkistetaan uudelleen diagnostiikan jälkeen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21690"/>
        <source>Validate running host</source>
        <translation>Validoidaan käynnissä oleva palvelin</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21695"/>
        <source>Validate repair target</source>
        <translation>Validoidaan korjauskohde</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21709"/>
        <location filename="../src/MainWindow.cpp" line="21853"/>
        <location filename="../src/MainWindow.cpp" line="22239"/>
        <source>running host</source>
        <translation>käynnissä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21710"/>
        <location filename="../src/MainWindow.cpp" line="21854"/>
        <location filename="../src/MainWindow.cpp" line="22240"/>
        <source>selected repair system</source>
        <translation>valittu korjausjärjestelmä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21714"/>
        <source>Complete interrupted package configuration in the %1 with dpkg --configure -a</source>
        <translation>Täydellinen keskeytyspakettien kokoonpano %1: ssä dpkg: llä -- configure -a</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21718"/>
        <source>Repair broken APT package dependencies in the %1</source>
        <translation>Korjaa rikkinäiset APT-pakettiriippuvuudet %1:ssä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21722"/>
        <source>Refresh APT package metadata in the %1</source>
        <translation>Päivitä APT-paketin metatiedot %1:ssä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21726"/>
        <source>Run the distribution-specific transaction preflight, then choose a safe upgrade for the %1</source>
        <translation>Suorita jakelukohtainen tapahtuma ennen lentoa, valitse turvallinen päivitys %1:lle</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21730"/>
        <source>Run DKMS autoinstall in the %1</source>
        <translation>Suorita DKMS autoinstall %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21734"/>
        <source>Set graphical.target as the %1&apos;s default boot target</source>
        <translation>Aseta graafinen kohde %1:n oletuskäynnistyskohteeksi</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21735"/>
        <source>Enable the detected display manager and repair display-manager.service without starting it inside a chroot or host helper</source>
        <translation>Ota huomioon havaittu näytönhallinta ja korjaus näytön hallinta. palvelu aloittamatta sitä sisällä Chroot tai isäntä apuri</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21737"/>
        <source>Rebuild initramfs</source>
        <translation>Rakenna initramfs uudelleen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21739"/>
        <source>Trial-build and then rebuild all initramfs images for the %1</source>
        <translation>Kokeile rakentaa ja sitten rakentaa kaikki initramfs kuvia %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21741"/>
        <source>Repair EFI / UKI bootloader</source>
        <translation>Korjaus EFI / UKI bootloader</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21745"/>
        <location filename="../src/MainWindow.cpp" line="21769"/>
        <source>detected bootloader</source>
        <translation>havaittu bootloader</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21747"/>
        <source>Repair the %1&apos;s %2 boot path through its guarded installer or vendor builder, preserving every other boot entry</source>
        <translation>Korjaa %1 %2-käynnistyspolku vartioidun asentajan tai myyjän rakentajan kautta, säilyttäen kaikki muut saappaan merkinnät</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21748"/>
        <source>Restore one verified default-loader entry when the guarded installer only wrote files</source>
        <translation>Palauta yksi todennettu oletuslataajan merkintä, kun suojattu asentaja vain kirjoitti tiedostoja</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2013"/>
        <location filename="../src/MainWindow.cpp" line="21750"/>
        <source>Regenerate GRUB configuration</source>
        <translation>Luo GRUB- asetus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20237"/>
        <source> Fedora/RPM restores missing or corrupt package files via one simulated dnf reinstall transaction.</source>
        <translation>Fedora/RPM palauttaa puuttuvat tai korruptoituneet pakettitiedostot yhden simuloidun dnf:n avulla.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20248"/>
        <source> Fedora uses dnf makecache as its standalone metadata stage.</source>
        <translation>Fedora käyttää dnf makecachea erillisenä metatietona.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20259"/>
        <source> Fedora runs one guarded dnf upgrade transaction after a removal- and downgrade-free, signature-checked simulation.</source>
        <translation>Fedora käyttää yhtä vartioitua dnf-päivitystapahtumaa poistovapaan ja alennuksen jälkeen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20293"/>
        <source>Walk an evidence-guided interactive session to repair the kernel GPU driver (nvidia/nouveau/amdgpu/radeon/mesa), applying only the small guarded step you choose with its own rollback. Two decision points guide the session: kernel-vs-driver (replace the newest kernel the driver cannot build against, or remove the older kernel the driver is pinned to) and wrong-driver (purge the incorrect driver, install the correct one, and rebuild it against every installed kernel). A separate one-boot nomodeset fallback reaches a console on a broken driver.</source>
        <translation>Kävele näyttöohjatun interaktiivisen istunnon korjata ytimen GPU-ajuri (nvidia/nuveau/amdgpu/radeon/mesa), soveltamalla vain pieni vartioitu vaihe valitset oman rollback. Kaksi päätöspistettä opastaa istuntoa: ytimen-vs-driver (vaihda uusin ydin, jota kuljettaja ei voi rakentaa, tai poista vanhempi ydin, johon kuljettaja on kiinnitetty) ja väärä-driver (puhdista väärä kuljettaja, asenna oikea, ja rakentaa se uudelleen kaikkia asennettu ydin). Erillinen yksisaapas nomodeset-järjestelmä saavuttaa rikkinäisen kuljettajan konsolin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20305"/>
        <source> The detected dracut backend pairs each installed kernel with its /boot vmlinuz and initramfs image, verifies a trial build with lsinitrd, and backs up each image so a failed verification restores it.</source>
        <translation>Havaittu dracut backend paria kunkin asennetun ytimen /boot vmlinuz ja initramfs kuva, tarkastaa kokeilun rakentaa lsinitrd, ja varmuuskopioi jokaisen kuvan niin epäonnistunut todentaminen palauttaa sen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20314"/>
        <source>Repair the running host or selected repair system&apos;s boot path with the detected backend&apos;s guarded installer or vendor builder, restoring one verified default-loader entry and preserving every other ESP&apos;s firmware entries and BootOrder. </source>
        <translation>Korjaa juoksuisännän tai valitun korjausjärjestelmän käynnistyspolku havaittujen taustaosan vartioituun asentajaan tai myyjärakentajaan, palauta yksi todennettu oletuslatain ja säilytä kaikki muut ESP:n laiteohjelmistomerkinnät ja BootOrder.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20317"/>
        <source>The detected vendor UKI layout (for example the TUXEDO TUX.EFI builder) is rebuilt or re-registered through its official builder. </source>
        <translation>Havaittu myyjä UKI asettelu (esimerkiksi TUXEDO TUX.EFI rakentaja) rakennetaan uudelleen tai rekisteröidään uudelleen virallisen rakennuttajan kautta.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20320"/>
        <source>The detected Fedora GRUB2/BLS backend repairs the boot code on a BIOS layout, leaving /boot/grub2/grub.cfg and the BLS entries intact. </source>
        <translation>Havaittu Fedora GRUB2/BLS backend korjaa boot koodin BIOS asettelu, jättäen /boot/grub2/grub.cfg ja BLS merkinnät ehjät.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20326"/>
        <source>The detected GRUB EFI layout uses a guarded grub-install on the validated ESP and restores one verified vendor-loader firmware entry if the installer only writes files. </source>
        <translation>Havaittu GRUB EFI asettelu käyttää vartioitua grub-install:ää validoidussa ESP:ssä ja palauttaa yhden todennetun myyjä-loader-ohjelmiston merkinnän, jos asentaja kirjoittaa vain tiedostoja.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20331"/>
        <source>A failed install restores the ESP backup; decoded entries keep their distribution/vendor label and drive model, and unrelated entries on other disks are never removed.</source>
        <translation>Epäonnistunut asennus palauttaa ESP:n varmuuskopion; dekoodatut merkinnät säilyttävät jakelu-/vendor-etikettinsä ja -ajomallinsa, eikä muita levyjä ole koskaan poistettu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20339"/>
        <source>Regenerate the running host or selected repair system&apos;s GRUB2 configuration (grub2-mkconfig --no-grubenv-update, /boot/grub2/grub.cfg with the BLS entries preserved) after the mandatory safety preflight, using an entry-preserving guard and a rollback so no menu or BLS entry is lost. When the read-only boot-code probe finds the MBR or BIOS boot partition broken, the same stage performs a guarded grub2-install --target=i386-pc boot-code reinstall with backup and rollback; it never writes firmware NVRAM or reinstalls EFI loader files — use EFI / UKI bootloader when the firmware loader itself needs repair.</source>
        <translation>Uudista juoksuisäntä tai valitun korjausjärjestelmän GRUB2-asetukset (grub2-mkconfig --no-grubenv-update, /boot/grub2/grub.cfg säilytetyillä BLS-merkinnöillä) pakollisen lentoa edeltävän lennon jälkeen käyttäen sisäänpääsynestosuojaa ja rollbackia, joten valikkoa tai BLS-merkintää ei menetetä. Kun MBR- tai BIOS-saappaan osio on rikki, sama vaihe suorittaa vartioidun grub2-asennuksen --kohde=i386-PC-saappaankoodin uudelleenasennuksen varmuuskopiolla ja rollbackilla; se ei koskaan kirjoita firmware NVRAM:ää tai asenna EFI-kuormaajatiedostoja uudelleen .</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20360"/>
        <source>Run a read-only file system check on the selected system&apos;s root, /boot, ESP and /home filesystems, then repair only the devices that report errors. Detected tools are used per file system — e2fsck/xfs_repair and the other native checkers, btrfs/zpool scrub, and the FAT/exFAT/NTFS tools — with the risky btrfs check --repair gated behind a separate backup confirmation. Offline tools refuse mounted filesystems, the online modes run where available, the running host root is never repaired offline, and unsupported filesystems are reported rather than guessed about.</source>
        <translation>Suorita vain lukutiedostojärjestelmän tarkistus valitun järjestelmän juurelle, /bootille, ESP:lle ja /home-tiedostojärjestelmille ja korjaa sitten vain virheet ilmoittavia laitteita. Havaittuja työkaluja käytetään tiedostojärjestelmää kohti .../e2fsck/xfs repair ja muut natiivit tarkistajat, btrfs/zpool scrub ja FAT/exFAT/NTFS -työkalut, joissa on riskialtis btrfs-tarkistus -- korjataan erillisen varmistuksen jälkeen. Offline-työkalut hylkäävät asennettuja tiedostojärjestelmiä, online-tilat toimivat, käynnissä olevaa isäntäjuurta ei koskaan korjata offline-tilassa, ja tuettuja tiedostojärjestelmiä raportoidaan sen sijaan, että arvattaisiin.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20369"/>
        <source>Reconcile a repaired or restored root with its boot artifacts: validate mapper/crypttab, rebuild installed-kernel initramfs images, repair the detected bootloader path (vendor UKI builder, GRUB EFI, extlinux or Fedora BLS), reconcile one canonical default destination per purpose, and regenerate the detected bootloader configuration. </source>
        <translation>Reconcile korjattu tai palautettu root sen boot artefacts: validoi mapper / cryptab, uudelleen asennettu-ydin initramfs kuvia, korjata havaittu bootloader polku (vektori UKI rakentaja, GRUB EFI, extlinux tai Fedora BLS), sovittaa yksi kanoninen oletuskohde käyttötarkoitusta kohden, ja uudistaa havaittu bootloader kokoonpano.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20372"/>
        <source>On a Fedora BIOS target this validates mapper/crypttab, rebuilds the dracut initramfs images and regenerates the GRUB2 configuration (config-only; the guarded bootloader reinstall stays in the GRUB stage). </source>
        <translation>Fedora BIOS -kohteessa tämä validoi mapper/cryptab-kohteen, rakentaa dracut initramfs -kuvat uudelleen ja uudistaa GRUB2-konfiguraation (vain konfiguraatio; vartioitu bootloader pysyy GRUB-vaiheessa).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20375"/>
        <source>When the EFI / UKI bootloader repair already ran for the same system in this session, this action reuses it (skipping the duplicate bootloader rebuild and GRUB regeneration) while mapper/crypttab validation and initramfs reconciliation still run. This is the focused recovery action for a root/boot mismatch after a partial update or snapshot restore; it never deletes kernels or unrelated boot entries.</source>
        <translation>Kun EFI / UKI bootloader korjaus jo toimi samassa järjestelmässä tässä istunnossa, tämä toiminto käyttää sitä uudelleen (slipping dumple bootloader restruction and GRUB regeneration) samalla mapper / cryptab validointi ja initramfs täsmäytys vielä käynnissä. Tämä on kohdennettu palautustoimi root/boot-epäsuhdannetta varten osittaisen päivityksen tai tilannekatsauksen palauttamisen jälkeen; se ei koskaan poista ytimiä tai etuyhteydetöntä käynnistysmerkintää.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20383"/>
        <source>Review the installed kernel inventory and choose one concrete kernel repair action: remove the oldest/superseded or newest kernel, remove orphaned headers/components/build support, install missing headers, reinstall an incomplete kernel, or install the repository-latest kernel. Nothing is removed or modified until you pick an action and confirm it. The running kernel is always protected, and the remaining kernels&apos; initramfs images and the bootloader configuration are regenerated after a removal.</source>
        <translation>Tarkista asennetun ytimen varasto ja valitse yksi konkreettinen ytimen korjaustoiminto: poista vanhin/ylin tai uusin ydin, poista orvot otsikot/komponentit/rakentamistuki, asenna puuttuvat otsikot, asenna uudelleen keskeneräinen ydin tai asenna arkiston viimeisin ydin. Mitään ei poisteta tai muuteta ennen kuin valitset toimen ja vahvistat sen. Juokseva ydin on aina suojattu, ja loput ytimien initramfs-kuvat ja bootloader-asetukset regeneroidaan poiston jälkeen.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21752"/>
        <source>Trial-generate and then regenerate the %1&apos;s GRUB configuration</source>
        <translation>Trial generate and then regenerate the %1 GRUB configuration</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21756"/>
        <source>Regenerate the %1&apos;s extlinux bootloader configuration with an entry-preserving guard</source>
        <translation>Uudista %1 extlinux -käynnistyslaitteen konfiguraatio sisääntulosuojalla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21758"/>
        <source>Reconcile boot stack</source>
        <translation>Reconcile boot pino</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21771"/>
        <source>Validate mapper/crypttab against the %1</source>
        <translation>Validoidaan mapper/cryptab %1:ää vastaan</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21772"/>
        <source>Trial-build and rebuild initramfs for installed kernels</source>
        <translation>Trial-rakentaminen ja jälleenrakentaminen initramfs asennettuja ytimiä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21773"/>
        <source>Repair the detected %1 boot path, removing only duplicate default destinations</source>
        <translation>Korjaa havaittu %1- käynnistyspolku, poista vain kaksoisoletuskohteet</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21774"/>
        <source>Regenerate the detected bootloader configuration</source>
        <translation>Uusia havaittujen bootloader-asetusten asetukset</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21776"/>
        <source>Reuse the bootloader repair already completed for this %1: skip the duplicate bootloader rebuild and GRUB regeneration</source>
        <translation>Uudelleenkäytä tätä %1:tä varten valmiiksi saatettua bootloader-korjausta: jätä väliin kaksoiskomponenttikäynnistys ja GRUB:n uudistaminen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21792"/>
        <source>No repair workflow is registered for the selected tool.</source>
        <translation>Valitulle työkalulle ei ole rekisteröity korjaustöitä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21803"/>
        <location filename="../src/MainWindow.cpp" line="21875"/>
        <source>Read-only diagnostics completed; review the full evidence in Logs before confirming repair.</source>
        <translation>Lue vain diagnostiikka valmis; tarkista kaikki todisteet lokit ennen vahvistamista korjaus.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21856"/>
        <source>Run the read-only file system check, then repair the selected devices</source>
        <translation>Suorita vain lukutiedostojärjestelmän tarkistus ja korjaa valitut laitteet</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21858"/>
        <source>Repair broken APT dependencies</source>
        <translation>Korjaa rikkinäiset APT-riippuvuudet</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21859"/>
        <source>Refresh APT package metadata</source>
        <translation>Päivitä APT-paketin metatiedot</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21860"/>
        <source>Simulate APT first, then choose a safe upgrade/full-upgrade/dist-upgrade transaction</source>
        <translation>Simuloi APT ensin, valitse sitten turvallinen päivitys/täydellinen/dist-upgrade tapahtuma</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21862"/>
        <source>Restore the detected display manager and graphical.target without starting the GUI inside chroot</source>
        <translation>Palauta havaittu näytönhallinta ja graafinen. kohde aloittamatta GUI sisällä Chroot</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21863"/>
        <source>Rebuild all initramfs images</source>
        <translation>Rakenna kaikki initramfs-kuvat uudelleen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21864"/>
        <source>Repair the %1 EFI / UKI boot path using its validated ESP</source>
        <translation>Korjaa %1 EFI / UKI boot polku käyttämällä validoitua ESP</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21865"/>
        <source>Regenerate the %1&apos;s GRUB configuration</source>
        <translation>Uudista %1 GRUB -kokoonpano</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21866"/>
        <source>Regenerate the %1&apos;s extlinux configuration</source>
        <translation>Uusi %1 extlinux -kokoonpano</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21946"/>
        <source>The Full Repair plan is running. The read-only file system check pre-stage runs first, then the selected repair stages; each stage&apos;s output streams here as it completes.</source>
        <translation>Koko korjaussuunnitelma on käynnissä. Luku-vain tiedostojärjestelmä tarkistaa pre-stage toimii ensin, sitten valitut korjausvaiheet; kunkin vaiheen lähtövirrat täällä, kun se valmistuu.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21964"/>
        <source>Full Repair plan finished with failures. Review the per-stage results (✓/✗/▪) in Logs, then close this window.</source>
        <translation>Täydellinen korjaussuunnitelma päättyi epäonnistumisiin. Tarkista kunkin vaiheen tulokset (✓/✗/▪) in Logs ja sulje tämä ikkuna.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21965"/>
        <source>Full Repair plan finished. Review the per-stage results (✓/✗/▪) in Logs, then close this window.</source>
        <translation>Koko korjaussuunnitelma valmis. Tarkista kunkin vaiheen tulokset (✓/✗/▪) in Logs ja sulje tämä ikkuna.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22164"/>
        <source>File system check unavailable</source>
        <translation>Tiedostojärjestelmän tarkistus ei ole käytettävissä</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22190"/>
        <source>Check File Systems (Full Repair pre-stage)</source>
        <translation>Tarkista tiedostojärjestelmät (koko korjausta edeltävä vaihe)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22191"/>
        <source>Check File Systems (manual)</source>
        <translation>Tarkista tiedostojärjestelmät (manual)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22205"/>
        <source>File system check failed</source>
        <translation>Tiedostojärjestelmän tarkistus epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22206"/>
        <source>The read-only file system check failed. Review the captured helper output in Logs for the reason.</source>
        <translation>Vain lukutiedostojen tarkistus epäonnistui. Tarkista kaapattu auttajan tuloste lokeissa syystä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22222"/>
        <source>File system repair unavailable</source>
        <translation>Tiedostojärjestelmän korjausta ei ole saatavilla</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22235"/>
        <source>Checking and repairing file systems (Full Repair)</source>
        <translation>Tiedostojärjestelmien tarkistaminen ja korjaaminen (Full Repair)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22236"/>
        <source>Checking and repairing file systems</source>
        <translation>Tiedostojärjestelmien tarkastaminen ja korjaaminen</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22286"/>
        <source>%1 mounted filesystem(s) were skipped because their check tools are offline-only</source>
        <translation>%1:n asennettu tiedostojärjestelmä [s] ohitettiin, koska niiden tarkistustyökalut ovat offline-vain</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22290"/>
        <source>%1 filesystem(s) are unsupported or have no installed check tool</source>
        <translation>%1-tiedostojärjestelmät [s] eivät ole tuettuja tai niillä ei ole asennettua tarkistustyökalua</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22294"/>
        <source>No file system errors were detected on the %1&apos;s root, /boot, ESP or /home filesystems. No repair was run.</source>
        <translation>Tiedostojärjestelmän virheitä ei havaittu %1-juurta, /boot-, ESP- tai /home-tiedostojärjestelmissä. Korjauksia ei tehty.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22296"/>
        <source>No repairable file system errors were detected on the %1&apos;s root, /boot, ESP or /home filesystems, but %2. No repair was run.</source>
        <translation>%1:n juurissa, /bootissa, ESP:ssä tai /home-tiedostojärjestelmissä ei havaittu korjattavia tiedostojärjestelmän virheitä, mutta %2:ssä. Korjauksia ei tehty.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22328"/>
        <source>The read-only check found issues on %1 mounted filesystem(s) that have no safe repair mode from this environment. Unmount the filesystem and check again, or repair it from a live system that is not using it.</source>
        <translation>Lue vain tarkistus löysi ongelmia %1 asennettu tiedostojärjestelmä [s], joilla ei ole turvallista korjaustilaa tästä ympäristöstä. Avaa tiedostojärjestelmä ja tarkista uudelleen tai korjaa se elävästä järjestelmästä, joka ei käytä sitä.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22361"/>
        <source>Run %1 repair on %2</source>
        <translation>Suorita %1 korjaus %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22384"/>
        <source>Dangerous Btrfs repair</source>
        <translation>Vaarallinen Btrfs korjaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22385"/>
        <source>btrfs check --repair is a last-resort tool</source>
        <translation>btrfs-tarkistus --korjaus on viimeinen resort-työkalu</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22386"/>
        <source>Upstream Btrfs documentation warns that check --repair can make a damaged filesystem worse and can lose data. Back up everything you can reach first. Prefer a scrub, a rescue, or a fresh backup/restore when either is possible.

Run btrfs check --repair on:
%1</source>
        <translation>Btrfs-dokumentaatio varoittaa, että tarkistus --korjaus voi pahentaa vaurioitunutta tiedostojärjestelmää ja menettää dataa. Taaksepäin kaikki, mihin tavoitat ensin. Suosin pesua, pelastusta tai tuoretta varmuuskopiota/korjausta, kun kumpi tahansa on mahdollista.

Suorita btrfs-tarkistus -- korjaus:
%1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22392"/>
        <source>Run dangerous Btrfs repair</source>
        <translation>Suorita vaarallinen Btrfs korjaus</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22429"/>
        <source>File system repair (%1)</source>
        <translation>Tiedostojärjestelmän korjaus (%1)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22430"/>
        <source>File system repair (%1, manual)</source>
        <translation>Tiedostojärjestelmän korjaus (%1, manuaalinen)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22468"/>
        <source>File system repair failed</source>
        <translation>Tiedostojärjestelmän korjaus epäonnistui</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22469"/>
        <source>One or more file system repairs failed. Review the captured helper output in Logs for the failing device and its reason.</source>
        <translation>Yksi tai useampi tiedostojärjestelmän korjaus epäonnistui. Tarkista kaapattu auttajan lähtö lokeissa vikalaitteen ja sen syyn vuoksi.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22685"/>
        <source>Device columns auto-sized. Drag headers to fine-tune widths.</source>
        <translation>Laitepatruunat automaattisesti. Vedä otsikkoja hienosäätimeen.</translation>
    </message>
</context>
</TS>
