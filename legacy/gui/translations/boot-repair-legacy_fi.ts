<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS language="fi">
<context>
    <name>LegacyMainWindow</name>
    <message>
        <source>Validate environment</source>
        <translation>Validointiympäristö</translation>
    </message>
    <message>
        <source>Validate</source>
        <translation>Validointi</translation>
    </message>
    <message>
        <source>Check the selected system&apos;s mounts, filesystem metadata, boot files, mapper consistency and dependency readiness before any repair action. This is an independent safety preflight rather than an optional Full Repair stage.</source>
        <translation>Tarkista valitun järjestelmän liitokset, tiedostojärjestelmän metatiedot, käynnistystiedostot, mapper johdonmukaisuus ja riippuvuus valmius ennen mitään korjausta. Tämä on riippumaton lentoa edeltävä turvallisuusvaihe eikä valinnainen täyskorjausvaihe.</translation>
    </message>
    <message>
        <source>Always preflight</source>
        <translation>Aina ennen lentoa</translation>
    </message>
    <message>
        <source>File system repair</source>
        <translation>Tiedostojärjestelmän korjaus</translation>
    </message>
    <message>
        <source>Check File Systems</source>
        <translation>Tarkista tiedostojärjestelmät</translation>
    </message>
    <message>
        <source>Run the read-only file system check for the selected system&apos;s root and /boot filesystems and report each device&apos;s check tool and result without changing anything. This legacy frontend exposes the read-only check only; device repair is not wired.</source>
        <translation>Suorita vain lukutiedostojärjestelmän tarkistus valitun järjestelmän juuri- ja /boot-tiedostojärjestelmät ja raportoi kunkin laitteen tarkistustyökalu ja tulos muuttamatta mitään. Tämä vanha etulinja paljastaa vain luku-vain tarkistus; laitteen korjaus ei ole kytketty.</translation>
    </message>
    <message>
        <source>Full Repair plan: unavailable on this frontend - read-only check only</source>
        <translation>Täydellinen korjaussuunnitelma: ei saatavilla tällä rintamalla - vain lukutarkistus</translation>
    </message>
    <message>
        <source>Complete package configuration</source>
        <translation>Täydellinen pakettien kokoonpano</translation>
    </message>
    <message>
        <source>Complete Configuration</source>
        <translation>Täydellinen asetukset</translation>
    </message>
    <message>
        <source>Complete interrupted dpkg package configuration in the selected repair system. This is the same stage controlled by Settings -&gt; Full Repair plan -&gt; Complete interrupted package configuration, but it can also be run independently here.</source>
        <translation>Täydellinen keskeytys dpkg-paketin konfiguraatio valitussa korjausjärjestelmässä. Tämä on sama vaihe, jota valvoo Asetukset -&gt; Koko korjaussuunnitelma -&gt; Täydellinen keskeytyspakettien konfigurointi, mutta se voidaan suorittaa myös itsenäisesti täällä.</translation>
    </message>
    <message>
        <source>Run the guarded dpkg-configure repair? The helper keeps its package-lock and runtime preflights.</source>
        <translation>Tehdäänkö vartioitu dpkg-konfigurointi? Apuri pitää pakettilukon ja ajoajan ennen lentoa.</translation>
    </message>
    <message>
        <source>Repair broken dependencies</source>
        <translation>Korjaa rikkinäiset riippuvuudet</translation>
    </message>
    <message>
        <source>Repair Dependencies</source>
        <translation>Korjaus</translation>
    </message>
    <message>
        <source>Repair package dependencies in the selected repair system after the mandatory safety preflight. This maps directly to Settings -&gt; Repair broken package dependencies.</source>
        <translation>Korjaa pakettiriippuvuudet valitussa korjausjärjestelmässä pakollisen esilennon jälkeen. Tämä kartta suoraan Asetukset -&gt; Korjaa rikkinäiset pakettiriippuvuudet.</translation>
    </message>
    <message>
        <source>Run the guarded fix-broken repair? The helper keeps its simulation-first preflight and runtime guards.</source>
        <translation>Suorittaa vartioitu korjaus? Auttaja pitää simulaatio-ensimmäinen lentoa ja ajoaika vartijat.</translation>
    </message>
    <message>
        <source>Refresh package metadata</source>
        <translation>Päivitä paketin metatiedot</translation>
    </message>
    <message>
        <source>Refresh Metadata</source>
        <translation>Päivitä metatiedot</translation>
    </message>
    <message>
        <source>Refresh APT metadata in the selected repair system without upgrading installed packages. This maps directly to Settings -&gt; Refresh package metadata.</source>
        <translation>Päivitä APT-metatiedot valitussa korjausjärjestelmässä päivittämättä asennettuja paketteja. Tämä kartta suoraan Asetukset -&gt; Päivitä paketin metatiedot.</translation>
    </message>
    <message>
        <source>Refresh package metadata for the selected scope? The helper requires a reachable, trusted APT source.</source>
        <translation>Päivitä pakettien metatiedot valitulle soveltamisalalle? Auttaja tarvitsee tavoitettavissa olevan luotettavan APT-lähteen.</translation>
    </message>
    <message>
        <source>Upgrade installed packages</source>
        <translation>Asennettujen pakettien päivittäminen</translation>
    </message>
    <message>
        <source>Simulate and Upgrade</source>
        <translation>Simuloi ja paranna</translation>
    </message>
    <message>
        <source>Simulate the APT transaction first, inspect proposed removals, then apply a safe upgrade. This maps directly to Settings -&gt; Upgrade installed packages.</source>
        <translation>Simuloi ensin APT-tapahtuma, tarkista ehdotetut poistot ja käytä sitten turvallista päivitystä. Tämä kartta suoraan Asetukset -&gt; Päivitä asennetut paketit.</translation>
    </message>
    <message>
        <source>Run the guarded apt-upgrade transaction? The helper keeps its simulation-first and source guards.</source>
        <translation>Suoritatko vartioidut kaupat? Auttaja pitää simulaatio-ensimmäinen ja lähde vartijat.</translation>
    </message>
    <message>
        <source>DKMS</source>
        <translation>DKMS</translation>
    </message>
    <message>
        <source>Rebuild DKMS</source>
        <translation>Uudelleenrakentaminen DKMS</translation>
    </message>
    <message>
        <source>Rebuild out-of-tree kernel modules for kernels installed in the selected system. The helper refuses this action when DKMS is not installed; this legacy frontend exposes no DKMS action.</source>
        <translation>Rakennetaan uudelleen out-of-puun ytimen moduulit ytimiä asennettu valittuun järjestelmään. Avustaja kieltäytyy toimimasta, kun DKMS:ää ei ole asennettu.</translation>
    </message>
    <message>
        <source>Graphical login / display manager</source>
        <translation>Graafinen kirjautuminen / näytönhallinta</translation>
    </message>
    <message>
        <source>Restore Graphical Login</source>
        <translation>Palauta graafinen kirjautuminen</translation>
    </message>
    <message>
        <source>Restore the legacy SysV display manager configured for the running host: the /etc/X11/default-display-manager entry and the missing runlevel S-symlink, with a backup and rollback, never starting the GUI. This is a host-scope stage on this legacy frontend.</source>
        <translation>Palauta vanha SysV-näyttöohjain, joka on määritetty käynnissä olevalle isännälle: /etc/X11/default-display-manager-merkintä ja puuttuva juoksutason S-symlink, varmuuskopiolla ja rollbackilla, älä koskaan käynnistä käyttöliittymää. Tämä on isännän lava tässä perintörintamassa.</translation>
    </message>
    <message>
        <source>Restore the graphical login configuration for the running host? The helper backs up /etc/X11/default-display-manager and the runlevel symlink state, restores the configured entry and the missing S-symlink, rolls back on any failure, and never starts the display manager.</source>
        <translation>Palauttaa graafisen kirjautumisasetukset käynnissä isäntä? Auttaja tukee /etc/X11/default-display-manageria ja juoksutason symlink-tilaa, palauttaa konfiguroidun tietueen ja puuttuvan S-symlin, rullaa takaisin vikaan eikä koskaan käynnistä näytönhallintaa.</translation>
    </message>
    <message>
        <source>Initramfs</source>
        <translation>Initramfs</translation>
    </message>
    <message>
        <source>Rebuild Initramfs</source>
        <translation>Initramfsin uudelleenrakentaminen</translation>
    </message>
    <message>
        <source>Rebuild initramfs images for the selected repair system only after mapper and crypttab consistency checks pass. The helper backs up each image before the apply. On Etch the stage runs through the guarded plain-chroot fallback (no unshare required).</source>
        <translation>Rakenna initramfs-kuvat uudelleen valittuun korjausjärjestelmään vasta mapper- ja cryptab-yhteensopivuustarkistusten jälkeen. Auttaja varmuuskopioi jokaisen kuvan ennen sovellusta. Etsissä lava kulkee vartioidun tasangon varalla (ei tarvitse jakaa).</translation>
    </message>
    <message>
        <source>Rebuild the initramfs for the selected scope? The helper keeps its mapper/crypttab and backup preflights.</source>
        <translation>Rakenna initramfs uudelleen valitulle alueelle? Auttaja pitää mapper/cryptab ja varmuuskopiot ennen lentoa.</translation>
    </message>
    <message>
        <source>EFI / UKI bootloader</source>
        <translation>EFI / UKI-käynnistin</translation>
    </message>
    <message>
        <source>Repair EFI / UKI</source>
        <translation>Korjaus EFI / UKI</translation>
    </message>
    <message>
        <source>Repair the selected system&apos;s EFI / UKI boot path. This legacy frontend exposes no EFI action; the Etch target is a BIOS/GRUB-legacy system.</source>
        <translation>Korjaa valitun järjestelmän EFI / UKI käynnistyspolku. Tämä perinteinen eturintama ei paljasta EFI-toimintaa; Etch-kohde on BIOS/GRUB-legaattijärjestelmä.</translation>
    </message>
    <message>
        <source>GRUB configuration</source>
        <translation>GRUB- kokoonpano</translation>
    </message>
    <message>
        <source>Regenerate GRUB</source>
        <translation>Regeneroi GRUB</translation>
    </message>
    <message>
        <source>Regenerate the selected repair system&apos;s GRUB menu/configuration after the mandatory safety preflight. The helper backs up menu.lst, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Uudista valitun korjausjärjestelmän GRUB-valikko/konfiguraatio pakollisen turvallisuuslennon jälkeen. Auttaja tukee valikkoa.lst, säilyttää kaikki olemassa olevat saappaat ja rullaa takaisin epäonnistumisia.</translation>
    </message>
    <message>
        <source>Regenerate the GRUB configuration? The helper backs up the target menu/configuration, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Luo GRUB-konfiguraatio uudelleen? Apuri tukee kohdevalikkoa/konfiguraatiota, säilyttää kaikki olemassa olevat käynnistyssyötteet ja rullaa takaisin vikaan.</translation>
    </message>
    <message>
        <source>extlinux configuration</source>
        <translation>extlinux- kokoonpano</translation>
    </message>
    <message>
        <source>Regenerate extlinux</source>
        <translation>Regeneroi extlinux</translation>
    </message>
    <message>
        <source>Regenerate the selected system&apos;s extlinux bootloader configuration. This legacy frontend exposes no extlinux action.</source>
        <translation>Uudista valitun järjestelmän extlinux-käynnistinasetukset. Tämä perinteinen eturintama ei paljasta extlinux-toimintaa.</translation>
    </message>
    <message>
        <source>Boot stack reconciliation</source>
        <translation>Boot pino täsmäytys</translation>
    </message>
    <message>
        <source>Reconcile Boot Stack</source>
        <translation>Reconcile Boot Stack</translation>
    </message>
    <message>
        <source>Reconcile the selected repair system&apos;s boot stack in one guarded legacy pass: mapper/crypttab validation, initramfs rebuild and GRUB-legacy configuration regeneration, with the component backups and preflights unchanged. This is the Etch equivalent of the modern boot-stack reconciliation and stays out of the Full Repair plan.</source>
        <translation>Reconcile valitun korjausjärjestelmän boot pino yhdessä vartioidussa perintöpassissa: mapper/cryptab validointi, initramfs uudelleenrakentaminen ja GRUB-legacy kokoonpanon uudistaminen, komponentti varmuuskopioita ja esilentoja muuttumaton. Tämä on Etch vastaa modernia boot-stack sovintoa ja pysyy pois Full Repair suunnitelma.</translation>
    </message>
    <message>
        <source>Full Repair plan: Manual recovery tool</source>
        <translation>Koko korjaussuunnitelma: Manuaalinen palautustyökalu</translation>
    </message>
    <message>
        <source>Run the guarded boot-stack reconciliation? The helper runs the mapper/crypttab validation, the initramfs rebuild and the GRUB-legacy regeneration in one pass with every component preflight and backup.</source>
        <translation>Tehdä vartioitu sovinto? Auttaja suorittaa mapper/cryptab-validoinnin, initramfs:n jälleenrakentamisen ja GRUB-legacy-regeneraation yhdellä syöttökerralla jokaisen osan ennen lentoa ja varmuuskopiota.</translation>
    </message>
    <message>
        <source>Complete interrupted package configuration</source>
        <translation>Keskeytä pakettien asetukset</translation>
    </message>
    <message>
        <source>Repair broken package dependencies</source>
        <translation>Korjaa rikkinäiset pakettiriippuvuudet</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Tee oletus</translation>
    </message>
    <message>
        <source>Repair file system errors (read-only check first)</source>
        <translation>Korjaa tiedostojärjestelmän virheet (lukea vain tarkistaa ensin)</translation>
    </message>
    <message>
        <source>the legacy frontend exposes the read-only file system check only; per-device repair is not wired on this frontend (fail closed)</source>
        <translation>Perinteinen Frontend paljastaa vain lukutiedostojen järjestelmän tarkistus; laitekohtainen korjaus ei ole kytketty tällä rintamalla (vika suljettu)</translation>
    </message>
    <message>
        <source>Upgrade installed packages (adaptive APT simulation)</source>
        <translation>Asennettujen pakettien päivittäminen (mukautuva APT-simulaatio)</translation>
    </message>
    <message>
        <source>Rebuild DKMS modules</source>
        <translation>Rakenna DKMS-moduulit uudelleen</translation>
    </message>
    <message>
        <source>Restore graphical login manager</source>
        <translation>Palauta graafinen kirjautumishallinta</translation>
    </message>
    <message>
        <source>Rebuild initramfs after mapper/crypttab validation</source>
        <translation>Rakenna initramfs uudelleen mapper/cryptab-validoinnin jälkeen</translation>
    </message>
    <message>
        <source>Repair EFI / UKI boot path</source>
        <translation>Korjaus EFI / UKI boot polku</translation>
    </message>
    <message>
        <source>Update GRUB configuration</source>
        <translation>Päivitä GRUB- asetus</translation>
    </message>
    <message>
        <source>Update extlinux configuration</source>
        <translation>Päivitä extlinux- asetus</translation>
    </message>
    <message>
        <source>Choose Full Repair stages in Settings. Enabled stages run in the order shown. Each configurable stage also appears below as an individual tool; the Full Repair column mirrors its current Settings state. The boot tools (EFI / UKI bootloader, GRUB or extlinux configuration, boot-stack reconciliation and Make Default) are independent: run them in any order, and a later action re-verifies what an earlier one changed and reports its own result. The active scope is shown beside Repair: selected repair drive or Running Host maintenance.</source>
        <translation>Valitse täydellinen korjausvaiheet Asetuksista. Käytössä olevat vaiheet suoritetaan järjestyksessä. Jokainen konfiguroitava vaihe näkyy myös alla yksittäisenä työkaluna; Full Repair- sarake peilaa sen nykyistä asetustilaa. Käynnistystyökalut (EFI / UKI bootloader, GRUB tai extlinux konfiguraatio, boot-stack täsmäytys ja Make Oletus) ovat riippumattomia: suorita ne missä tahansa järjestyksessä, ja myöhemmin toiminta tarkistaa, mitä aikaisempi muutti ja raportoi oman tuloksensa. Aktiivinen laajuus näkyy vieressä Korjaus: valittu korjausasema tai Running Host huolto.</translation>
    </message>
    <message>
        <source>Run All diagnostics for the selected target or running host before starting Full Repair. The report is read-only evidence used to choose and confirm repair stages.</source>
        <translation>Suorita kaikki diagnostiikat valitun kohteen tai käynnissä isäntä ennen Full Repair. Raportti on pelkkä luettava todiste korjausvaiheiden valitsemisesta ja vahvistamisesta.</translation>
    </message>
    <message>
        <source>Ready: required cached read-only diagnostics are available for the selected stages. Review them in Diagnostics or Logs before confirming.</source>
        <translation>Valmiina: valittuihin vaiheisiin on saatavilla välimuistia vain lukudiagnostiikkaa varten. Tarkista ne Diagnostiikassa tai lokissa ennen vahvistamista.</translation>
    </message>
    <message>
        <source>Environment validation</source>
        <translation>Ympäristön validointi</translation>
    </message>
    <message>
        <source>Summarizes the selected system, protection state, mounted identity and inspection readiness.</source>
        <translation>Yhteenveto valitusta järjestelmästä, suojatilasta, asennetusta henkilöllisyydestä ja tarkastusvalmiudesta.</translation>
    </message>
    <message>
        <source>Distribution and boot backend profile</source>
        <translation>Jakelu- ja käynnistystukiprofiili</translation>
    </message>
    <message>
        <source>Identifies the distribution family, package manager, initramfs generator, bootloader and current guarded repair capability.</source>
        <translation>Tunnistaa jakeluperheen, paketinhallinnan, initramfs-generaattorin, bootloaderin ja nykyisen suojatun korjauskyvyn.</translation>
    </message>
    <message>
        <source>Boot diagnostics</source>
        <translation>Saapasdiagnostiikka</translation>
    </message>
    <message>
        <source>Shows boot mounts and /boot contents plus storage evidence without changing the selected system.</source>
        <translation>Näyttää käynnistyskiinnikettä ja /boot-sisältöä sekä tallennustodisteita muuttamatta valittua järjestelmää.</translation>
    </message>
    <message>
        <source>Boot evidence and selection history</source>
        <translation>Todisteet ja valintahistoria</translation>
    </message>
    <message>
        <source>Correlates the detected boot chain, bootloader selection, kernel/initramfs and unlock evidence.</source>
        <translation>Korjaa havaittu käynnistysketju, bootloader valinta, ydin / initramfs ja avata todisteita.</translation>
    </message>
    <message>
        <source>Kernel / initramfs</source>
        <translation>Ytimen / initramfs</translation>
    </message>
    <message>
        <source>Reviews kernel files and verifies matching initramfs images through a read-only inspection.</source>
        <translation>Tarkistaa ydintiedostoja ja todentaa täsmäävät initramfs-kuvat vain lukutarkastuksella.</translation>
    </message>
    <message>
        <source>Reviews the GRUB configuration without changing boot files.</source>
        <translation>Tarkistaa GRUB-asetukset muuttamatta käynnistystiedostoja.</translation>
    </message>
    <message>
        <source>EFI / UKI boot state</source>
        <translation>EFI / UKI- käynnistystila</translation>
    </message>
    <message>
        <source>Inspects EFI/UKI evidence; unavailable on this legacy BIOS frontend with the helper&apos;s probe reason.</source>
        <translation>Tarkkailee EFI/UKI-todisteita; ei saatavilla tästä vanhasta BIOS-etulinjasta auttajan luotaimen syyllä.</translation>
    </message>
    <message>
        <source>Reviews the configured display manager and recent boot evidence without starting the GUI.</source>
        <translation>Tarkistaa konfiguroidun näytönhallinnan ja viimeaikaiset käynnistystodisteet aloittamatta käyttöliittymää.</translation>
    </message>
    <message>
        <source>Boot errors</source>
        <translation>Saapasvirheet</translation>
    </message>
    <message>
        <source>Reads recent error-priority entries from the running host or selected repair system when available.</source>
        <translation>Lue tuoreimmat virheprioriteettimerkinnät käynnissä olevasta isäntästä tai valitusta korjausjärjestelmästä, kun ne ovat saatavilla.</translation>
    </message>
    <message>
        <source>Disk usage</source>
        <translation>Levyn käyttö</translation>
    </message>
    <message>
        <source>Summarizes filesystem capacity and free space for the running host or read-only repair target.</source>
        <translation>Summarizes tiedostojärjestelmän kapasiteetti ja vapaa tila käynnissä isäntä tai luku-vain korjaus kohde.</translation>
    </message>
    <message>
        <source>File systems</source>
        <translation>Tiedostojärjestelmät</translation>
    </message>
    <message>
        <source>Runs the read-only file system check for the selected system&apos;s root, /boot and other filesystems.</source>
        <translation>Suorittaa luku-vain tiedostojärjestelmän tarkistus valitun järjestelmän juuren, /saappaan ja muiden tiedostojärjestelmien.</translation>
    </message>
    <message>
        <source>/etc/fstab review</source>
        <translation>/etc/fstab-arviointi</translation>
    </message>
    <message>
        <source>Displays the running host or selected repair system&apos;s fstab; repair-system inspection is mounted read-only.</source>
        <translation>Näyttää juoksuisännän tai valitun korjausjärjestelmän fstabin; korjausjärjestelmän tarkastus on asennettu vain luettavaksi.</translation>
    </message>
    <message>
        <source>Btrfs status</source>
        <translation>Btrfs- tila</translation>
    </message>
    <message>
        <source>Shows Btrfs filesystem and subvolume information when the target uses Btrfs.</source>
        <translation>Näyttää Btrfs-tiedostojärjestelmän ja subvolume-tiedot, kun kohde käyttää Btrfs:ää.</translation>
    </message>
    <message>
        <source>Device-mapper ancestry</source>
        <translation>Laite-mapper esiasteet</translation>
    </message>
    <message>
        <source>Shows selected mapper ancestry and device-mapper state when available.</source>
        <translation>Näyttää valitut mapper esi-isä ja laite-mapper tila, kun saatavilla.</translation>
    </message>
    <message>
        <source>LUKS / crypttab evidence</source>
        <translation>LUKS / kryptaab-todisteet</translation>
    </message>
    <message>
        <source>Shows LUKS/mapped ancestry plus crypttab and fstab mapper references.</source>
        <translation>Näyttää LUKS/mapped esihistorian sekä kryptaab- ja fstab mapper-viittaukset.</translation>
    </message>
    <message>
        <source>Full diagnostic report</source>
        <translation>Täydellinen vianmääritysraportti</translation>
    </message>
    <message>
        <source>Combines all read-only diagnostics for the selected scope (same as Run All).</source>
        <translation>Yhdistää kaikki luku-vain diagnostiikka valitun soveltamisalan (sama kuin Suori Kaikki).</translation>
    </message>
    <message>
        <source>All entries</source>
        <translation>Kaikki merkinnät</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>Diagnostiikka</translation>
    </message>
    <message>
        <source>Repairs</source>
        <translation>Korjaukset</translation>
    </message>
    <message>
        <source>Package repair</source>
        <translation>Pakkausten korjaus</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Tiedostokopiointi</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Laitelöytö</translation>
    </message>
    <message>
        <source>Host</source>
        <translation>Palvelin</translation>
    </message>
    <message>
        <source>Required for block-device inventory</source>
        <translation>Vaaditaan laitelohkon inventointia</translation>
    </message>
    <message>
        <source>Filesystem identification</source>
        <translation>Tiedostojärjestelmän tunnistus</translation>
    </message>
    <message>
        <source>Used to identify filesystem metadata</source>
        <translation>Käytetään tiedostojärjestelmän metatietojen tunnistamiseen</translation>
    </message>
    <message>
        <source>Mount inspection</source>
        <translation>Asennuksen tarkastus</translation>
    </message>
    <message>
        <source>Used to understand active mounts</source>
        <translation>Käytetään aktiivisten kiinnikkeiden ymmärtämiseen</translation>
    </message>
    <message>
        <source>LUKS support</source>
        <translation>LUKS-tuki</translation>
    </message>
    <message>
        <source>Required to unlock encrypted targets</source>
        <translation>Vaaditaan avaamaan salattuja kohteita</translation>
    </message>
    <message>
        <source>Btrfs support</source>
        <translation>Btrfs-tuki</translation>
    </message>
    <message>
        <source>Required for Btrfs inspection and snapshot rollback</source>
        <translation>Vaaditaan Btrfs-tarkastusta ja kuvien varalaskua varten</translation>
    </message>
    <message>
        <source>Bidirectional file copy</source>
        <translation>Kaksisuuntainen tiedostokopio</translation>
    </message>
    <message>
        <source>Host/Repair</source>
        <translation>Host/Repair</translation>
    </message>
    <message>
        <source>Required for verified Host-to-Repair and Repair-to-Host transfer</source>
        <translation>Vaaditaan todennettua Host-to-repair ja korjaus-to-ost siirto</translation>
    </message>
    <message>
        <source>Chroot repair</source>
        <translation>Chroot korjaus</translation>
    </message>
    <message>
        <source>Required for target-side repair commands</source>
        <translation>Vaaditaan maalin puolen korjauskomentoja varten</translation>
    </message>
    <message>
        <source>Offline systemd repair</source>
        <translation>Offline järjestelmäkorjaus</translation>
    </message>
    <message>
        <source>Used to restore graphical.target and the configured display manager without starting the target GUI</source>
        <translation>Sitä käytettiin graafisen restaurointiin. kohde ja konfiguroitu näytönhallinta aloittamatta kohdekäyttöliittymää</translation>
    </message>
    <message>
        <source>UEFI NVRAM inspection</source>
        <translation>UEFI NVRAM-tarkastus</translation>
    </message>
    <message>
        <source>Used to preserve target EFI BootOrder during TUXEDO UKI rebuilds when efivars are available</source>
        <translation>Käytetään säilyttämään kohde EFI BootOrder aikana TUXEDO UKI jälleenrakentaa kun efivars on saatavilla</translation>
    </message>
    <message>
        <source>UKI verification</source>
        <translation>UKI-tarkastus</translation>
    </message>
    <message>
        <source>Used to verify the kernel embedded in a rebuilt unified kernel image</source>
        <translation>Käytetään uudelleen rakennetussa ytimen kuvassa olevan ytimen todentamiseen</translation>
    </message>
    <message>
        <source>GRUB EFI repair</source>
        <translation>GRUB EFI korjaus</translation>
    </message>
    <message>
        <source>Target/Host</source>
        <translation>Tavoite/Host</translation>
    </message>
    <message>
        <source>Required only for conventional GRUB-based EFI systems</source>
        <translation>Vaaditaan ainoastaan perinteisissä GRUB-pohjaisissa EFI-järjestelmissä</translation>
    </message>
    <message>
        <source>Target</source>
        <translation>Kohde</translation>
    </message>
    <message>
        <source>Debian-family GRUB helper</source>
        <translation>Debian-perhe GRUB -apulainen</translation>
    </message>
    <message>
        <source>Portable GRUB configuration generator used by Arch and other non-Debian systems</source>
        <translation>Arch- ja muiden kuin Debian-järjestelmien käyttämä kannettava GRUB-kokoonpanogeneraattori</translation>
    </message>
    <message>
        <source>Initramfs rebuild</source>
        <translation>Initramfsin jälleenrakentaminen</translation>
    </message>
    <message>
        <source>Debian-family initramfs helper</source>
        <translation>Debian-perhe initramfs -apulainen</translation>
    </message>
    <message>
        <source>Arch-family initramfs generator</source>
        <translation>Arkkiperheen initramfs generaattori</translation>
    </message>
    <message>
        <source>Alternative initramfs generator used by Arch and other distributions</source>
        <translation>Archin ja muiden jakelujen käyttämä vaihtoehtoinen initramfs-generaattori</translation>
    </message>
    <message>
        <source>Initramfs verification</source>
        <translation>Initramfs-varmennus</translation>
    </message>
    <message>
        <source>Read-only verification for mkinitcpio images</source>
        <translation>Vain mkinitcpio-kuvien tarkistus</translation>
    </message>
    <message>
        <source>Read-only verification for dracut images</source>
        <translation>Lue vain dracut-kuvien tarkistus</translation>
    </message>
    <message>
        <source>systemd-boot inspection</source>
        <translation>systemd-boot-tarkastus</translation>
    </message>
    <message>
        <source>Host/Target</source>
        <translation>Kohde</translation>
    </message>
    <message>
        <source>Read-only inspection of systemd-boot and generic UKI layouts</source>
        <translation>systemd-boot:n ja yleisten UKI:n ulkoasujen lukutilatarkastus</translation>
    </message>
    <message>
        <source>Arch package manager</source>
        <translation>Arch-paketinhallinta</translation>
    </message>
    <message>
        <source>Arch-family package database and transaction tool</source>
        <translation>Arch-perhe paketti tietokanta ja tapahtuma työkalu</translation>
    </message>
    <message>
        <source>DKMS rebuild</source>
        <translation>DKMS:n jälleenrakentaminen</translation>
    </message>
    <message>
        <source>Required only when target uses DKMS modules</source>
        <translation>Vaaditaan vain, kun kohde käyttää DKMS-moduuleja</translation>
    </message>
    <message>
        <source>LVM inspection</source>
        <translation>LVM-tarkastus</translation>
    </message>
    <message>
        <source>Optional LVM storage-stack support</source>
        <translation>Valinnainen LVM-varastotuki</translation>
    </message>
    <message>
        <source>Software RAID</source>
        <translation>Ohjelmisto RAID</translation>
    </message>
    <message>
        <source>Optional Linux MD RAID support</source>
        <translation>Valinnainen Linux MD RAID -tuki</translation>
    </message>
    <message>
        <source>Process namespace isolation</source>
        <translation>Prosessin nimiavaruuden eristäminen</translation>
    </message>
    <message>
        <source>Host+Target</source>
        <translation>Host+ Kohde</translation>
    </message>
    <message>
        <source>The ported helper falls back to a guarded plain chroot when unshare is absent</source>
        <translation>Siirretty apuri kaatuu takaisin vartioituun tasangolle kun jakamaton on poissa</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Peruuta</translation>
    </message>
    <message>
        <source>OK</source>
        <translation>Selvä.</translation>
    </message>
    <message>
        <source>Yes</source>
        <translation>Kyllä</translation>
    </message>
    <message>
        <source>No</source>
        <translation>Ei</translation>
    </message>
    <message>
        <source>read-only helper diagnostic</source>
        <translation>lukuavain apurin vianmääritys</translation>
    </message>
</context>
<context>
    <name>legacy::LegacyMainWindow</name>
    <message>
        <source>Boot Bitch Legacy (Etch / KDE 3.5 era)</source>
        <translation>Boot Bitch Legacy (Etsi / KDE 3,5 aikakausi)</translation>
    </message>
    <message>
        <source>Linux recovery and boot-repair utility</source>
        <translation>Linuxin palautus- ja käynnistyskorjausapuohjelma</translation>
    </message>
    <message>
        <source>GUARDED REPAIR</source>
        <translation>VAROITUS</translation>
    </message>
    <message>
        <source>Ordinary repairs require an explicitly selected non-host target. The protected running host has a separate deliberate maintenance mode with the same guarded repair stages and requires privilege authorization.</source>
        <translation>Tavalliset korjaukset edellyttävät nimenomaisesti valittua muuta kuin isäntäkohdetta. Suojatulla käyttöisännällä on erillinen tahallinen huoltotila, jossa on samat vartioidut korjausvaiheet, ja se vaatii etuoikeuden.</translation>
    </message>
    <message>
        <source>Systems</source>
        <translation>Järjestelmät</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>Diagnostiikka</translation>
    </message>
    <message>
        <source>Repair</source>
        <translation>Korjaus</translation>
    </message>
    <message>
        <source>Chroot Shell</source>
        <translation>Chroot Shell</translation>
    </message>
    <message>
        <source>File Copy</source>
        <translation>Tiedostokopio</translation>
    </message>
    <message>
        <source>Logs</source>
        <translation>Lokit</translation>
    </message>
    <message>
        <source>Settings</source>
        <translation>Asetukset</translation>
    </message>
    <message>
        <source>(not created yet)</source>
        <translation>(ei vielä luotu)</translation>
    </message>
    <message>
        <source>Information</source>
        <translation>Tiedot</translation>
    </message>
    <message>
        <source>Close</source>
        <translation>Sulje</translation>
    </message>
    <message>
        <source>&amp;File</source>
        <translation>-&amp;tiedosto</translation>
    </message>
    <message>
        <source>&amp;Refresh Devices</source>
        <translation>-&amp;päivityslaitteet</translation>
    </message>
    <message>
        <source>&amp;Lock Administrator Session</source>
        <translation>&amp;Lock Administrator istunto</translation>
    </message>
    <message>
        <source>&amp;Quit</source>
        <translation>- &amp;lopettaminen</translation>
    </message>
    <message>
        <source>&amp;View</source>
        <translation>-&amp;näkymä</translation>
    </message>
    <message>
        <source>&amp;Systems</source>
        <translation>-&amp;järjestelmät</translation>
    </message>
    <message>
        <source>&amp;Diagnostics</source>
        <translation>-&amp;diagnostiikka</translation>
    </message>
    <message>
        <source>&amp;Logs</source>
        <translation>-&amp;lokit</translation>
    </message>
    <message>
        <source>&amp;Settings</source>
        <translation>- &amp;asetukset</translation>
    </message>
    <message>
        <source>&amp;Auto-size Device Columns</source>
        <translation>&amp;Automaattinen koko laitteen sarakkeet</translation>
    </message>
    <message>
        <source>&amp;Wrap Log Lines</source>
        <translation>-&amp;käärelokirivit</translation>
    </message>
    <message>
        <source>&amp;Help</source>
        <translation>- &amp;ohje</translation>
    </message>
    <message>
        <source>&amp;Using Boot Bitch</source>
        <translation>&amp;Boot Bitch:n käyttö</translation>
    </message>
    <message>
        <source>&amp;About Boot Bitch</source>
        <translation>&amp;Tietoja Boot Bitch:stä</translation>
    </message>
    <message>
        <source>Device columns auto-sized. Drag headers to fine-tune widths.</source>
        <translation>Laitepatruunat automaattisesti. Vedä otsikkoja hienosäätimeen.</translation>
    </message>
    <message>
        <source>A helper command is running; wait for it to finish before locking the session.</source>
        <translation>Auttaja-komento on käynnissä; odota sen päättymistä ennen istunnon lukitsemista.</translation>
    </message>
    <message>
        <source>Administrator session locked; the next privileged action will request authorization.</source>
        <translation>Hallitsijan istunto lukittu; seuraava etuoikeutettu toimi pyytää valtuutusta.</translation>
    </message>
    <message>
        <source>Using Boot Bitch</source>
        <translation>Boot Bitch:n käyttö</translation>
    </message>
    <message>
        <source>Boot Bitch must run from a different booted Linux environment than the system being repaired. Use a Linux live medium or another Linux installation on a different physical drive.&lt;br&gt;&lt;br&gt;The running host is protected from ordinary repair-target selection, but it can be explicitly selected through &lt;b&gt;Host Maintenance&lt;/b&gt; for guarded native diagnostics and supported maintenance stages.&lt;br&gt;&lt;br&gt;Diagnostics follow the Systems page: the committed repair drive while Host Maintenance is off, or the protected running host while it is active.&lt;br&gt;&lt;br&gt;The first privileged action requests administrator authorization once for this Boot Bitch window; &lt;b&gt;File - Lock Administrator Session&lt;/b&gt; ends that helper session immediately. Every repair keeps the helper&apos;s own runtime preflights.</source>
        <translation>Boot Bitch:n on oltava erilaisessa Linux-käyttöympäristössä kuin korjattavassa järjestelmässä. Käytä Linux live medium tai muuta Linux asennus eri fyysisellä asemalla. &lt;br &gt; &lt;br&gt; &gt; Juokseva isäntä on suojattu tavanomaiselta korjauskohdevalinnalta, mutta se voidaan nimenomaisesti valita &lt;b&gt;Host Maintenance &lt;/b&gt; -toiminnolla vartioidulle omalle diagnostiikalle ja tuetuille huoltovaiheille. &lt;br&gt;&gt;&lt;br&gt; Diagnostiikka seuraa Systems-sivua: sitoutunut korjausasema, kun isäntähuolto on pois päältä, tai suojattu juoksun isäntä, kun se on aktiivinen. &lt;br&gt;&lt;br&gt; Ensimmäinen etuoikeutettu toiminto pyytää järjestelmänvalvojan valtuutusta kerran tähän Boot Bitch-ikkunaan; &lt;b&gt;Tieto - Lukituksen ylläpitäjän istunto &lt;/b&gt; lopettaa tämän auttajan istunnon välittömästi. Jokainen korjaus pitää auttajalle oman ajoajan ennen lentoa.</translation>
    </message>
    <message>
        <source>Select a physical drive; Boot Bitch resolves the most likely Linux system volume automatically. The running host stays protected from ordinary target repairs, with a separate explicit host-maintenance path for its own system. The Details button shows the protected host&apos;s facts in the details pane; selecting any drive row restores the per-drive pane.</source>
        <translation>Valitse fyysinen asema; Boot Bitch ratkaisee todennäköisimmän Linux-järjestelmän äänenvoimakkuuden automaattisesti. Juokseva isäntä pysyy suojattuna tavallisilta kohteiden korjauksilta, ja sillä on oma erillinen huoltopolku omaa järjestelmää varten. Yksityiskohdat-painike näyttää suojatun isännän faktat yksityiskohtapaneelissa; minkä tahansa asemarivin valinta palauttaa per-ajopaneelin.</translation>
    </message>
    <message>
        <source>Refresh Devices</source>
        <translation>Päivitä laitteet</translation>
    </message>
    <message>
        <source>Re-read the read-only kernel inventory (/proc/partitions, /proc/mounts, /proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* and the udev metadata database). No block device is opened and nothing is written.</source>
        <translation>Lue uudelleen vain luettavan ytimen inventaario (/prosessi/osastot, /prosessi/vuori, /prosessi/aaltoja, /sys/block, /dev/mapper, /dev/levy/by-* ja udev metadatatietokanta). Mikään lohkolaite ei ole auki eikä mitään ole kirjoitettu.</translation>
    </message>
    <message>
        <source>[OK]</source>
        <translation>[OK]</translation>
    </message>
    <message>
        <source>The running system was detected and stays protected from ordinary target repairs.</source>
        <translation>Ajojärjestelmä havaittiin ja pysyy suojattuna tavallisilta korjauskohteilta.</translation>
    </message>
    <message>
        <source>Detecting running system...</source>
        <translation>Havaitaan ajojärjestelmä...</translation>
    </message>
    <message>
        <source>Detecting protected storage...</source>
        <translation>Havaitaan suojattu varastointi...</translation>
    </message>
    <message>
        <source>PROTECTED</source>
        <translation>SUOJATTU</translation>
    </message>
    <message>
        <source>The running host remains protected from ordinary repair-target operations.</source>
        <translation>Juokseva isäntä on edelleen suojattu tavallisilta korjauskohteilta.</translation>
    </message>
    <message>
        <source>Details</source>
        <translation>Yksityiskohdat</translation>
    </message>
    <message>
        <source>Show read-only details for the protected running host.</source>
        <translation>Näytä vain luettavia tietoja suojatulle ajoisännälle.</translation>
    </message>
    <message>
        <source>Host Maintenance</source>
        <translation>Huolto</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Tee oletus</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host (menu.lst default directive with a backup and rollback). Requires Host Maintenance and the cached host-default probe.</source>
        <translation>Tee asennetun ytimen syöte oletuksena GRUB-legacy-käynnistys syöttää käynnissä isäntä (menu.lst oletusdirektiivi varmuuskopiointi ja rollback). Vaatii isäntähuoltoa ja välimuistia.</translation>
    </message>
    <message>
        <source>Available repair targets</source>
        <translation>Käytettävissä olevat korjaustavoitteet</translation>
    </message>
    <message>
        <source>Most likely first</source>
        <translation>Todennäköisesti ensin</translation>
    </message>
    <message>
        <source>Drives are listed by the read-only inventory. Select a row to inspect it; Select Target commits the selected non-host drive with its auto-resolved Linux root component.</source>
        <translation>Asemat on lueteltu vain luku-inventaariossa. Valitse rivi tarkastaaksesi sen; Valitse Target sitoutuu valittuun ei-host-asemaan, jossa on automaattisesti ratkaistu Linuxin juurikomponentti.</translation>
    </message>
    <message>
        <source>Device</source>
        <translation>Laite</translation>
    </message>
    <message>
        <source>Size</source>
        <translation>Koko</translation>
    </message>
    <message>
        <source>Type</source>
        <translation>Tyyppi</translation>
    </message>
    <message>
        <source>Filesystem</source>
        <translation>Tiedostojärjestelmä</translation>
    </message>
    <message>
        <source>Select Target</source>
        <translation>Valitse kohde</translation>
    </message>
    <message>
        <source>Unlock</source>
        <translation>Avaa lukitus</translation>
    </message>
    <message>
        <source>Authorize</source>
        <translation>Hyväksy</translation>
    </message>
    <message>
        <source>Establish the privileged helper session for the current scope now instead of waiting for the next privileged action.</source>
        <translation>Perustetaan etuoikeutettu auttaja-istunto nyt sen sijaan, että odottaisimme seuraavaa etuoikeutettua toimintaa.</translation>
    </message>
    <message>
        <source>Committed target: none</source>
        <translation>Sitoutunut tavoite: ei ole</translation>
    </message>
    <message>
        <source>Unlock state for the selected drive; the LUKS passphrase is never logged.</source>
        <translation>Avaa valitun aseman lukitustila; LUKS-salalausetta ei ole koskaan kirjattu.</translation>
    </message>
    <message>
        <source>Unlock status</source>
        <translation>Avaa lukitustila</translation>
    </message>
    <message>
        <source>Read-only inventory plus helper-confirmed facts; mirrors the modern Qt6 Selected drive details panel.</source>
        <translation>Lue vain varasto ja auttajan vahvistamat faktat; peilaa moderni Qt6 Selected drive details paneeli.</translation>
    </message>
    <message>
        <source>Selected drive details</source>
        <translation>Valitut aseman tiedot</translation>
    </message>
    <message>
        <source>Field</source>
        <translation>Kenttä</translation>
    </message>
    <message>
        <source>Value</source>
        <translation>Arvo</translation>
    </message>
    <message>
        <source>Run All runs every available read-only diagnostic for the current scope; selecting a check runs it alone. Diagnostics are read-only and are the only evidence source for the gated repair actions.</source>
        <translation>Suorita Kaikki suorittaa kaikki saatavilla olevat luku-vain vianmääritys nykyisen soveltamisalan; valinta tarkistaa sen yksin. Diagnostikot ovat vain luku-ja ovat ainoa todiste lähde aidattu korjaus toimia.</translation>
    </message>
    <message>
        <source>Target: none selected</source>
        <translation>Kohde: ei valittu</translation>
    </message>
    <message>
        <source>Diagnostics follow the committed repair target, or the protected running host while Host Maintenance is active.</source>
        <translation>Diagnostiikka seuraa sitoutunut korjauskohde, tai suojattu käynnissä isäntä, kun isäntä huolto on aktiivinen.</translation>
    </message>
    <message>
        <source>Run All</source>
        <translation>Suorita kaikki</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope.</source>
        <translation>Suorita Kaikki - suorita kaikki saatavilla olevat luku-vain vianmääritys nykyisen soveltamisalan.</translation>
    </message>
    <message>
        <source>Target configuration:</source>
        <translation>Kohdeasetukset:</translation>
    </message>
    <message>
        <source>Etch-era target configuration files; availability is probed read-only by the helper&apos;s diagnostics.</source>
        <translation>Etsi-aika kohde asetukset tiedostot; saatavuus on skannattu vain lukemaan auttimen diagnostiikka.</translation>
    </message>
    <message>
        <source>Edit Target File...</source>
        <translation>Muokkaa kohdetiedostoa...</translation>
    </message>
    <message>
        <source>Runs one read-only diagnostic for the selected scope through the helper (`diagnose &lt;key&gt;` / `host-diagnose &lt;key&gt;`); Run All is the combined report.</source>
        <translation>Suorittaa vain yhden lukudiagnostisen valitun soveltamisalan kautta auttajan (...diagnoosi &lt;key&gt;... /...Host-diagnoosi &lt;key&gt;.</translation>
    </message>
    <message>
        <source>Diagnostic checks</source>
        <translation>Diagnostiset tarkastukset</translation>
    </message>
    <message>
        <source>Check</source>
        <translation>Tarkista</translation>
    </message>
    <message>
        <source>Selected diagnostic</source>
        <translation>Valittu vianmääritys</translation>
    </message>
    <message>
        <source>Select a diagnostic</source>
        <translation>Valitse vianmääritys</translation>
    </message>
    <message>
        <source>Choose a diagnostic from the list.</source>
        <translation>Valitse diagnostiikka luettelosta.</translation>
    </message>
    <message>
        <source>Ready</source>
        <translation>Valmis</translation>
    </message>
    <message>
        <source>Results</source>
        <translation>Tulokset</translation>
    </message>
    <message>
        <source>Run Diagnostic</source>
        <translation>Suorita diagnostiikka</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic for the current scope.</source>
        <translation>Run Diagnostic - suorita valittu luku vain vianmääritys nykyisen soveltamisalan.</translation>
    </message>
    <message>
        <source>Copy Results</source>
        <translation>Kopioi tulokset</translation>
    </message>
    <message>
        <source>Save Results...</source>
        <translation>Tallenna tulokset...</translation>
    </message>
    <message>
        <source>The Full Repair plan runs the selected legacy stages in order through the guarded helper; the individual tools run one stage at a time. Every action stays disabled until the cached capability lines say available and the helper keeps its runtime preflights.</source>
        <translation>Full Repair -suunnitelma ohjaa valittuja perintövaiheita vartioidun apurin kautta; yksittäiset työkalut kulkevat yksivaihe kerrallaan. Jokainen toiminta pysyy poissa käytöstä, kunnes välimuistissa olevat valmiuslinjat ovat käytettävissä ja apuri pitää ajoaikansa ennen lentoa.</translation>
    </message>
    <message>
        <source>Full Repair plan</source>
        <translation>Täydellinen korjaussuunnitelma</translation>
    </message>
    <message>
        <source>No stages selected</source>
        <translation>Ei valittuja vaiheita</translation>
    </message>
    <message>
        <source>Configure Plan...</source>
        <translation>Määrittele suunnitelma...</translation>
    </message>
    <message>
        <source>Open Settings to choose which Full Repair stages are part of the plan.</source>
        <translation>Avaa asetukset valita, mitkä Full Repair vaiheet ovat osa suunnitelmaa.</translation>
    </message>
    <message>
        <source>Run Full Repair</source>
        <translation>Suorita koko korjaus</translation>
    </message>
    <message>
        <source>Select a repair drive, or choose Host Maintenance on the protected running-host card.</source>
        <translation>Valitse korjausasema tai valitse isännän huolto suojatulla juoksu-host-kortilla.</translation>
    </message>
    <message>
        <source>Stage</source>
        <translation>Vaihe</translation>
    </message>
    <message>
        <source>Individual repair tools</source>
        <translation>Yksittäiset korjaustyökalut</translation>
    </message>
    <message>
        <source>Tool</source>
        <translation>Työkalu</translation>
    </message>
    <message>
        <source>Full Repair</source>
        <translation>Täysi korjaus</translation>
    </message>
    <message>
        <source>not reported</source>
        <translation>ei raportoitu</translation>
    </message>
    <message>
        <source>Selected tool</source>
        <translation>Valittu työkalu</translation>
    </message>
    <message>
        <source>Select a repair tool</source>
        <translation>Valitse korjaustyökalu</translation>
    </message>
    <message>
        <source>Run Tool</source>
        <translation>Suorita työkalu</translation>
    </message>
    <message>
        <source>Select a tool to review its repair action.</source>
        <translation>Valitse työkalu tarkistaa sen korjaustoiminta.</translation>
    </message>
    <message>
        <source>Write actions ask for confirmation and then run the helper&apos;s own runtime preflights; the GUI never weakens them. A repair that is not proven &apos;unchanged&apos; invalidates the cached diagnostics and disables the gated actions until diagnostics run again.</source>
        <translation>Kirjoita toiminnot pyytää vahvistusta ja sitten ajaa auttajan omia ajoaika ennen lentoa; käyttöliittymä ei koskaan heikennä niitä. Korjaus, joka ei ole todistettu &apos;muuttumaton&apos; mitätöi välimuistin diagnostiikan ja poistaa aidat toiminnot kunnes diagnostiikka toimii uudelleen.</translation>
    </message>
    <message>
        <source>Chroot shell</source>
        <translation>Chroot-kuori</translation>
    </message>
    <message>
        <source>Offline commands run one at a time in a fresh chroot and cannot answer interactive prompts (apt-get -y upgrade works). Host-shell commands run directly on the running host. The helper&apos;s probe lines gate the command field; the exact reason appears in the tooltip.</source>
        <translation>Offline-komennot ajavat yksi kerrallaan tuoreessa chroot eikä voi vastata interaktiivisiin kehotuksiin (apt-get -y päivitys toimii). Host-shell-komennot suoritetaan suoraan käynnissä isäntä. Avustajan luotainlinjat avaavat komentokentän; tarkka syy näkyy työkaluvihjeessä.</translation>
    </message>
    <message>
        <source>No &apos;Legacy feature shell:&apos; line is cached; run diagnostics for the selected scope to evaluate the helper&apos;s chroot/timeout containment probes (fail closed).</source>
        <translation>Ei &apos;Legacy ominaisuus komentotulkki:&apos; rivi on välimuistissa; suorita diagnostiikka valitun soveltamisalan arvioida auttajan Chroot / aikakatkaisu estoanturit (vika suljettu).</translation>
    </message>
    <message>
        <source>Command</source>
        <translation>Komento</translation>
    </message>
    <message>
        <source>Command:</source>
        <translation>Komento:</translation>
    </message>
    <message>
        <source>One reviewed command string, passed to the helper as a single argument (no shell interpolation by the GUI).</source>
        <translation>Yksi tarkistettu komentojono, siirretty apurille yhtenä argumenttina (ei komentotulkin interpolointia käyttöliittymässä).</translation>
    </message>
    <message>
        <source>Run Command</source>
        <translation>Suorita komento</translation>
    </message>
    <message>
        <source>Clear Output</source>
        <translation>Tyhjennä tuloste</translation>
    </message>
    <message>
        <source>The helper exposes `shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for an offline target chroot and `host-shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for the running host. Both keep the helper&apos;s runtime preflights; this tab enables the command only when the scope is committed, the session is authorized and the scope&apos;s Legacy feature probe reports available.</source>
        <translation>Auttaja altistaa ... ... &lt; levy &gt; &lt; root &gt; &lt; command&gt;. &lt; command&gt;... Molemmat pitävät auttajalla juoksuaika ennen lentoa; tämä välilehti mahdollistaa komennon vain silloin, kun kohde on tehty, istunto on sallittu ja komennon Legacy-ominaisuus on käytettävissä.</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Tiedostokopiointi</translation>
    </message>
    <message>
        <source>Copy and verify files in either direction through the guarded helper (cp -a plus ownership restoration and a per-file byte-compare). The helper&apos;s file-copy probe gates the controls and keeps the direction and path containment checks.</source>
        <translation>Kopioi ja todenna tiedostot kumpaankin suuntaan vartioidun apurin kautta (cp -a sekä omistusten palauttaminen ja per-file byte-vertaus). Auttimen tiedostokopion luotain avaa ohjaimet ja pitää suunnan ja polun estotarkistukset.</translation>
    </message>
    <message>
        <source>Preview Changes</source>
        <translation>Esikatselumuutokset</translation>
    </message>
    <message>
        <source>Run a copy dry-run through the guarded helper. No files are changed.</source>
        <translation>Aja kopio vartioituun apuriin. Tiedostoja ei ole muutettu.</translation>
    </message>
    <message>
        <source>Copy and Verify</source>
        <translation>Kopioi ja varmista</translation>
    </message>
    <message>
        <source>Copy staged items and verify the result. Existing destination names are overwritten when source content differs; unrelated destination files are never deleted.</source>
        <translation>Kopioi lavastetut kohdat ja varmista tulos. Olemassa olevat kohdenimet korvataan, kun lähdesisältö poikkeaa; etuyhteydettömiä kohdetiedostoja ei koskaan poisteta.</translation>
    </message>
    <message>
        <source>Direction:</source>
        <translation>Suunta:</translation>
    </message>
    <message>
        <source>Choose which system supplies the source files and which system receives them.</source>
        <translation>Valitse mikä järjestelmä toimittaa lähdetiedostot ja mikä järjestelmä vastaanottaa ne.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from this host</source>
        <translation>1. Valitse lähdetiedostot tai kansiot tästä isäntä</translation>
    </message>
    <message>
        <source>Source</source>
        <translation>Lähde</translation>
    </message>
    <message>
        <source>Files and folders staged for the verified copy. The legacy backend copies with cp -a and restores ownership with chown --reference; every regular file is byte-compared after the copy.</source>
        <translation>Tarkistetun kopion tiedostot ja kansiot. Perinteinen backend kopiot cp -a ja palauttaa omistusta cown -- referenssi; jokainen säännöllinen tiedosto on byte-verrattuna kopion.</translation>
    </message>
    <message>
        <source>Add Files...</source>
        <translation>Lisää tiedostoja...</translation>
    </message>
    <message>
        <source>Add Folder...</source>
        <translation>Lisää kansio...</translation>
    </message>
    <message>
        <source>Remove</source>
        <translation>Poista</translation>
    </message>
    <message>
        <source>Clear</source>
        <translation>Tyhjennä</translation>
    </message>
    <message>
        <source>Clear the staged source list (nothing is copied or deleted).</source>
        <translation>Tyhjennä vaiheistettu lähdeluettelo (mitään ei kopioida tai poisteta).</translation>
    </message>
    <message>
        <source>2. Choose destination in repaired system</source>
        <translation>2. Valitse kohde korjatussa järjestelmässä</translation>
    </message>
    <message>
        <source>unavailable: see the helper&apos;s Legacy feature file-copy: probe reason above</source>
        <translation>Ei saatavilla: katso auttaja Legacy ominaisuus tiedosto-kopio: luotain syy edellä</translation>
    </message>
    <message>
        <source>An absolute path inside the selected repair system (Host to Repair) or on the running host (Repair to Host).</source>
        <translation>Absoluuttinen polku valitun korjausjärjestelmän sisällä (Host to Repair) tai juoksuisännällä (Repair to Host).</translation>
    </message>
    <message>
        <source>Browse Target Folders...</source>
        <translation>Selaa kohdekansioita...</translation>
    </message>
    <message>
        <source>Browse the selected repair system through the helper&apos;s temporary read-only mounts and choose an absolute destination path. No target files are changed while browsing.</source>
        <translation>Selaa valittua korjausjärjestelmää auttimen tilapäisten lukulaitteiden avulla ja valitse ehdoton kohdepolku. Kohdetiedostoja ei muuteta selattaessa.</translation>
    </message>
    <message>
        <source>3. Ownership and copy policy</source>
        <translation>3. Omistus- ja kopiointipolitiikka</translation>
    </message>
    <message>
        <source>Ownership:</source>
        <translation>Omistusoikeus:</translation>
    </message>
    <message>
        <source>Smart destination ownership (recommended)</source>
        <translation>Älykäs määränpään omistus (suositeltu)</translation>
    </message>
    <message>
        <source>Preserve source numeric UID/GID</source>
        <translation>Säilytä lähdenumero UID/GID</translation>
    </message>
    <message>
        <source>Smart mode validates UID/GID identity mapping across the two systems and falls back to the destination-directory owner when the same numeric ID means a different account (the legacy backend implements it with chown --reference).</source>
        <translation>Älytila validoi UID/GID-tunnistuskartoituksen kahden järjestelmän välillä ja siirtyy takaisin kohde-hakemiston omistajalle, kun sama numeerinen ID tarkoittaa eri tiliä (perinteinen backend toteuttaa sen cown --reference).</translation>
    </message>
    <message>
        <source>Application log</source>
        <translation>Sovellusloki</translation>
    </message>
    <message>
        <source>The complete session register; Save As... writes every entry even while a filter hides lines. If a writable system share mount exists at /host, Save As... starts there; otherwise the log directory is the fallback. Prior session files are listed read-only.</source>
        <translation>Koko istuntorekisteri; Tallenna As... kirjoittaa jokaisen merkinnän vaikka suodatin piilottaa rivit. Jos kirjoitettava järjestelmäosake on olemassa /hostissa, Tallenna nimellä... alkaa sieltä; muuten lokihakemisto on varalla. Aiemmat istuntotiedostot on listattu vain luettavaksi.</translation>
    </message>
    <message>
        <source>Session logs</source>
        <translation>Istunnon lokit</translation>
    </message>
    <message>
        <source>Session</source>
        <translation>Istunto</translation>
    </message>
    <message>
        <source>The first entry is the live session; earlier files in the log directory are listed read-only below it.</source>
        <translation>Ensimmäinen merkintä on live-istunto; aiemmat lokihakemiston tiedostot on lueteltu vain sen alla.</translation>
    </message>
    <message>
        <source>New Session Log</source>
        <translation>Uuden istunnon loki</translation>
    </message>
    <message>
        <source>Close the active session file; it becomes a prior session and the next log entry starts a new file.</source>
        <translation>Sulje aktiivinen istuntotiedosto; siitä tulee aiempi istunto ja seuraava lokikirja alkaa uuden tiedoston.</translation>
    </message>
    <message>
        <source>Add Note</source>
        <translation>Lisää huomautus</translation>
    </message>
    <message>
        <source>Append a NOTE entry to the live session register.</source>
        <translation>Lisää HUOMAUTUSmerkintä live-istuntorekisteriin.</translation>
    </message>
    <message>
        <source>Delete</source>
        <translation>Poista</translation>
    </message>
    <message>
        <source>Delete the selected prior session file (the live session is never deleted).</source>
        <translation>Poista valittu istuntotiedosto (elävää istuntoa ei koskaan poisteta).</translation>
    </message>
    <message>
        <source>Refresh</source>
        <translation>Päivitä</translation>
    </message>
    <message>
        <source>Save As...</source>
        <translation>Pelasta...</translation>
    </message>
    <message>
        <source>Save the complete session log (all entries, not just the current filter).</source>
        <translation>Tallenna koko istuntoloki (kaikki merkinnät, ei vain nykyinen suodatin).</translation>
    </message>
    <message>
        <source>Clear Register</source>
        <translation>Tyhjennä rekisteri</translation>
    </message>
    <message>
        <source>Clear the live register and view; prior session files are never modified.</source>
        <translation>Tyhjennä live-rekisteri ja -näkymä; aiempia istuntotiedostoja ei koskaan muuteta.</translation>
    </message>
    <message>
        <source>Search log:</source>
        <translation>Etsi loki:</translation>
    </message>
    <message>
        <source>Type any characters to show matching log entries (case-insensitive). Save As always writes every entry.</source>
        <translation>Syötä kaikki merkit, jotka näyttävät vastaavat lokitiedot (tapaus ei-herkkä). Tallenna Kuten aina, kirjoittaa jokainen merkintä.</translation>
    </message>
    <message>
        <source>Filter:</source>
        <translation>Suodin:</translation>
    </message>
    <message>
        <source>Filter the visible log by entry kind. Selecting a diagnostic section shows the lines captured for that section; a workflow filter such as File system repair or Package repair shows its mapped repair lines (File copy has no lines in this frontend). Save As always writes every entry.</source>
        <translation>Suodata näkyvissä oleva loki kirjoitustavan mukaan. Diagnostisen osion valitseminen näyttää kyseiselle osiolle tallennetut linjat; työnkulkusuodatin, kuten Tiedostojärjestelmän korjaus tai Paketin korjaus, näyttää sen kartoitetut korjauslinjat (Tietokopiossa ei ole viivoja tässä rintamassa). Tallenna Kuten aina, kirjoittaa jokainen merkintä.</translation>
    </message>
    <message>
        <source>Settings are stored per user under ~/.qt/, one file per settings group (devicesrc, logsrc, diagnosticsrc, repairrc), and are saved immediately on every change and on close. Launch the GUI as the same user to keep your overrides; a GUI started as root keeps its own copies.</source>
        <translation>Asetukset tallennetaan käyttäjää kohti ~/.qt/, yksi tiedosto kutakin asetusryhmää kohden (laite, logrc, diagnosticsrc, korjausrc) ja tallennetaan välittömästi jokaisen muutoksen yhteydessä ja läheltä. Käynnistä käyttöliittymä saman käyttäjänä pitääksesi ohituksesi; juurina alkanut käyttöliittymä pitää omat kopionsa.</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Laitelöytö</translation>
    </message>
    <message>
        <source>Show devices without an identified Linux installation</source>
        <translation>Näytä laitteet ilman tunnistettua Linux-asennusta</translation>
    </message>
    <message>
        <source>Show removable and USB storage</source>
        <translation>Näytä irrotettava ja USB- tallennus</translation>
    </message>
    <message>
        <source>Show encrypted devices before unlocking</source>
        <translation>Näytä salatut laitteet ennen avaamista</translation>
    </message>
    <message>
        <source>When off, drives without a visible Linux filesystem are hidden unless they still contain an encrypted device and encrypted devices are shown.</source>
        <translation>Ovella olevat asemat, joissa ei ole näkyvää Linux-tiedostojärjestelmää, ovat piilossa, ellei niissä ole yhä salattua laitetta ja salattuja laitteita näytetään.</translation>
    </message>
    <message>
        <source>When off, removable and USB drives are hidden from the repair-target list.</source>
        <translation>Kun pois päältä, irrotettavat ja USB-asemat ovat piilossa korjauskohdelistalta.</translation>
    </message>
    <message>
        <source>When off, drives with an encrypted device are hidden until the volume is unlocked.</source>
        <translation>Kun pois päältä, asemat salatulla laitteella piilotetaan kunnes äänenvoimakkuus on lukittu.</translation>
    </message>
    <message>
        <source>Include the %1 stage in the Full Repair plan. The stage runs in the plan order shown on the Repair tab.</source>
        <translation>Sisältää %1 vaihe Full Repair suunnitelma. Vaihe kulkee korjausvälilehdessä esitetyssä suunnitelmajärjestyksessä.</translation>
    </message>
    <message>
        <source>Automatically regenerate read-only diagnostics after repairs or target changes</source>
        <translation>Uudista automaattisesti vain lukudiagnostiikka korjausten tai tavoitemuutosten jälkeen</translation>
    </message>
    <message>
        <source>Regenerates the cached read-only diagnostics for the current scope after an operation that invalidates them (LUKS unlock, target configuration edit). It runs only inside an already authorized administrator session and never opens an authorization prompt by itself.</source>
        <translation>Regeneroi välimuistin luku vain diagnostiikan nykyisen soveltamisalan jälkeen toiminnon, joka mitätöi ne (LUKS open, kohde asetukset). Se toimii vain jo hyväksytyn ylläpitäjän istunnon sisällä eikä koskaan avaa valtuutuskehotusta itsestään.</translation>
    </message>
    <message>
        <source>Wrap long log lines</source>
        <translation>Kääri pitkät lokiviivat</translation>
    </message>
    <message>
        <source>Mandatory safety controls</source>
        <translation>Pakollinen turvavalvonta</translation>
    </message>
    <message>
        <source>Host capabilities and dependencies</source>
        <translation>Isäntävalmiudet ja riippuvuussuhteet</translation>
    </message>
    <message>
        <source>Refresh Capabilities</source>
        <translation>Päivitä valmiudet</translation>
    </message>
    <message>
        <source>Re-run the read-only host capability probes (a PATH search, nothing is executed) and refresh the distribution summary.</source>
        <translation>Suorita uudelleen luku-vain isäntä valmiuksia luotaimet (PATH haku, mitään ei suoriteta) ja päivittää jakelu yhteenveto.</translation>
    </message>
    <message>
        <source>Install Missing Support...</source>
        <translation>Asenna puuttuva tuki...</translation>
    </message>
    <message>
        <source>Automatic installation will require explicit package mapping and privilege authorization.</source>
        <translation>Automaattinen asennus edellyttää nimenomaista paketti kartoitus ja etuoikeus valtuutus.</translation>
    </message>
    <message>
        <source>Application configuration</source>
        <translation>Sovelluskokoonpano</translation>
    </message>
    <message>
        <source>Boot Bitch Legacy</source>
        <translation>Boot Bitch Legacy</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list first.</source>
        <translation>Valitse ensin fyysinen asema käytettävissä olevista korjauskohteista.</translation>
    </message>
    <message>
        <source>Protected system</source>
        <translation>Suojattu järjestelmä</translation>
    </message>
    <message>
        <source>The running system cannot be selected as a repair target. Use Host Maintenance for the protected running host or choose another disk.</source>
        <translation>Ajojärjestelmää ei voi valita korjauskohteeksi. Käytä isännän huoltoa suojatussa ajossa tai valitse toinen levy.</translation>
    </message>
    <message>
        <source>Unlock or select a Linux system first</source>
        <translation>Avaa tai valitse Linux-järjestelmä ensin</translation>
    </message>
    <message>
        <source>This encrypted drive has no visible Linux filesystem yet. Use Unlock, refresh devices, and select the target after its Linux root is detected.</source>
        <translation>Tällä salatulla asemalla ei ole vielä näkyvää Linux-tiedostojärjestelmää. Avaa, virkistä laitteita ja valitse kohde sen jälkeen, kun sen Linux root on havaittu.</translation>
    </message>
    <message>
        <source>The selected root component (%1) belongs to the running system and cannot be committed as a repair target. Use Host Maintenance for the protected running host.</source>
        <translation>Valittu juurikomponentti (%1) kuuluu ajojärjestelmään eikä sitä voida tehdä korjauskohteena. Käytä palvelinta suojatussa käyttöisännässä.</translation>
    </message>
    <message>
        <source>repair target commit</source>
        <translation>korjauskohteen toimitus</translation>
    </message>
    <message>
        <source>Repair drive selected: %1</source>
        <translation>Korjausasema valittu: %1</translation>
    </message>
    <message>
        <source>; best detected system component: %1</source>
        <translation>; Paras havaittu järjestelmän komponentti: %1</translation>
    </message>
    <message>
        <source>. No mount or repair action was performed.</source>
        <translation>. Asennusta tai korjausta ei suoritettu.</translation>
    </message>
    <message>
        <source>Make Default unavailable</source>
        <translation>Oletus ei ole käytettävissä</translation>
    </message>
    <message>
        <source>Make Default is a running-host action on this frontend; enter Host Maintenance first.</source>
        <translation>Tee oletus on käynnissä-host toiminto tässä rintamassa; kirjoita isäntä huolto ensin.</translation>
    </message>
    <message>
        <source>Administrator authorization required</source>
        <translation>Hallitsijan lupa vaaditaan</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize first.</source>
        <translation>Mikään ylläpitäjän istunto ei ole aktiivinen; paina Authorize ensin.</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host?

The helper verifies /boot/grub/menu.lst, sets the `default &lt;N&gt;` directive to the canonical entry, backs the menu up first and restores it on any failure.</source>
        <translation>Tehdä kanoninen asennettu ytimen merkintä oletuksena GRUB-legacy käynnistyksen käynnissä isäntä?

Auttaja tarkistaa /boot/grub/menu.lst, asettaa ...oletus &lt;N&gt;...-direktiivin kanoniseen tuloon, peruu valikosta ensin ja palauttaa sen epäonnistuessa.</translation>
    </message>
    <message>
        <source>Host Maintenance unavailable</source>
        <translation>Huolto ei ole käytettävissä</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a committed repair target or a detected running host.</source>
        <translation>Juoksevaa isäntäkohdetta ei voitu havaita; diagnostiikkaan tarvitaan sitoutunut korjauskohde tai havaittu käynnissä oleva isäntä.</translation>
    </message>
    <message>
        <source>Exit Host Maintenance</source>
        <translation>Poistuminen isäntähuolto</translation>
    </message>
    <message>
        <source>Host Maintenance is active; diagnostics and gated repairs target the protected running host.</source>
        <translation>Isäntä Huolto on aktiivinen; diagnostiikka ja aidattu korjaukset kohteena suojattu käynnissä isäntä.</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list on the Systems tab first, or use Host Maintenance for the protected running host.</source>
        <translation>Valitse fyysinen asema käytettävissä korjauskohteiden luettelossa Systems-välilehden ensin, tai käytä Host Maintenance suojattu käynnissä isäntä.</translation>
    </message>
    <message>
        <source>The selected drive (%1) is the protected running host. Choose Host Maintenance on the Systems tab to run read-only host diagnostics and guarded host repairs; ordinary target repairs stay disabled.</source>
        <translation>Valittu asema (%1) on suojattu ajon isäntä. Valitse Isännän huolto Systems-välilehdestä suorittaaksesi luku-ohjausta ja vartioituja isännän korjauksia; tavalliset kohteet pysyvät poissa käytöstä.</translation>
    </message>
    <message>
        <source>No repair target is committed. Choose Select Target on the Systems tab (or Host Maintenance for the protected running host) first.</source>
        <translation>Korjaavaa kohdetta ei ole. Valitse Valitse Target Systems-välilehdestä (tai suojatun käyttöisännän ylläpito) ensin.</translation>
    </message>
    <message>
        <source>The selection changed after the target was committed. Choose Select Target again.</source>
        <translation>Valinta muuttui kohteen sitomisen jälkeen. Valitse kohde uudelleen.</translation>
    </message>
    <message>
        <source>Diagnostics scope required</source>
        <translation>Vaadittu diagnostiikan laajuus</translation>
    </message>
    <message>
        <source>Diagnostics scope unresolved</source>
        <translation>Diagnostiikan laajuus selvittämätön</translation>
    </message>
    <message>
        <source>The running host target could not be resolved; use Refresh Devices and commit a repair target or re-enter Host Maintenance.</source>
        <translation>Juoksevaa isäntäkohdetta ei voitu ratkaista; käytä Päivitä laitteita ja tee korjauskohde tai palaa isäntähuoltoon.</translation>
    </message>
    <message>
        <source>Diagnostic check required</source>
        <translation>Diagnostinen tarkistus vaaditaan</translation>
    </message>
    <message>
        <source>Select a diagnostic check in the list first.</source>
        <translation>Valitse ensin diagnostinen tarkistus luettelosta.</translation>
    </message>
    <message>
        <source>Repair target required</source>
        <translation>Tarvittava korjauskohde</translation>
    </message>
    <message>
        <source>Target file editing needs a committed repair target. Running-host maintenance has no target-file editing; commit an offline target first.</source>
        <translation>Kohdetiedoston muokkaus tarvitsee sidotun korjauskohteen. Running-host huolto ei ole kohde-tiedoston editointi; tee offline kohde ensin.</translation>
    </message>
    <message>
        <source>Configuration file required</source>
        <translation>Asetukset vaaditaan</translation>
    </message>
    <message>
        <source>Select a target configuration file first.</source>
        <translation>Valitse kohdeasetustiedosto ensin.</translation>
    </message>
    <message>
        <source>Edit target %1</source>
        <translation>Muokkaa kohdetta %1</translation>
    </message>
    <message>
        <source>Edit this target file through the guarded administrator helper. A successful save invalidates cached diagnostics; rerun diagnostics before repair. Generated files such as /boot/grub/menu.lst may be replaced by the next bootloader update.</source>
        <translation>Muokkaa tätä kohdetiedostoa vartioidun järjestelmänvalvojan avulla. Onnistunut säästää mitätöi välimuistin diagnostiikka; uudelleen diagnostiikka ennen korjausta. Luotuja tiedostoja, kuten /boot/grub/menu.lst voidaan korvata seuraavalla bootloader-päivityksellä.</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Peruuta</translation>
    </message>
    <message>
        <source>Save Target File</source>
        <translation>Tallenna kohdetiedosto</translation>
    </message>
    <message>
        <source>No changes to %1.</source>
        <translation>Ei muutoksia %1:ään.</translation>
    </message>
    <message>
        <source>Configuration write refused</source>
        <translation>Asetusten kirjoittaminen hylättiin</translation>
    </message>
    <message>
        <source>The edited content contains NUL bytes; the guarded write refuses it.</source>
        <translation>Editoitu sisältö sisältää NUL tavuja; vartioitu kirjoitus kieltäytyy siitä.</translation>
    </message>
    <message>
        <source>File too large</source>
        <translation>Tiedosto liian suuri</translation>
    </message>
    <message>
        <source>The edited file is larger than 1 MiB. The guarded write refuses it; edit the file from a console instead.</source>
        <translation>Editoitu tiedosto on suurempi kuin 1 MiB. Vartioitu kirjoitus torjuu sen; muokkaa tiedostoa sen sijaan konsolista.</translation>
    </message>
    <message>
        <source>Write target configuration</source>
        <translation>Kirjoita kohdeasetukset</translation>
    </message>
    <message>
        <source>Write the edited contents to %1? This modifies the repair target and invalidates cached diagnostics.</source>
        <translation>Kirjoittaa muokatun sisällön %1? Tämä muuttaa korjauskohdetta ja mitätöi välimuistin diagnostiikan.</translation>
    </message>
    <message>
        <source>No diagnostic results to copy yet.</source>
        <translation>Ei diagnostisia tuloksia vielä.</translation>
    </message>
    <message>
        <source>Diagnostic results copied to the clipboard.</source>
        <translation>Diagnostiset tulokset kopioitu leikepöydälle.</translation>
    </message>
    <message>
        <source>No diagnostic results to save yet.</source>
        <translation>Ei diagnostisia tuloksia.</translation>
    </message>
    <message>
        <source>Text files (*.txt);;All files (*)</source>
        <translation>Tekstitiedostot (*.txt);;Kaikki tiedostot (*)</translation>
    </message>
    <message>
        <source>Save diagnostic results</source>
        <translation>Tallenna diagnostiset tulokset</translation>
    </message>
    <message>
        <source>Could not write %1.</source>
        <translation>%1:ää ei voitu kirjoittaa.</translation>
    </message>
    <message>
        <source>Diagnostic results saved to %1</source>
        <translation>Diagnostiset tulokset tallennettu %1: lle</translation>
    </message>
    <message>
        <source>Unlock not available</source>
        <translation>Avaa lukitus ei saatavilla</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked. Select an offline repair target to unlock.</source>
        <translation>Suojattua isäntää ei voi avata. Valitse avattava offline-korjauskohde.</translation>
    </message>
    <message>
        <source>No locked LUKS component is currently visible on this selected drive.</source>
        <translation>Tällä hetkellä valitulla asemalla ei ole lukittua LUKS-komponenttia.</translation>
    </message>
    <message>
        <source>Confirm LUKS unlock</source>
        <translation>Vahvista LUKS-lukitus</translation>
    </message>
    <message>
        <source>Unlock %1 on %2?

The helper opens a temporary device-mapper mapping with cryptsetup and keeps it open for this recovery session. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>Avaa %1 %2: llä?

Auttaja avaa väliaikaisen laite-mapper-kartoituksen kryptalla ja pitää sen auki tätä palautusistuntoa varten. Salasana kulkee yksityisen avaintiedoston läpi eikä sitä koskaan sijoiteta komentoargumentteihin tai lokeihin.</translation>
    </message>
    <message>
        <source>LUKS unlock</source>
        <translation>LUKS-lukitus</translation>
    </message>
    <message>
        <source>Unlock LUKS repair target</source>
        <translation>Avaa LUKS-korjauskohde</translation>
    </message>
    <message>
        <source>Enter the passphrase for %1.

It is sent only to cryptsetup over the helper&apos;s standard input and is never logged or placed on a command line.</source>
        <translation>Anna %1:n salasana.

Se lähetetään vain kryptaukseen auttimen vakiosyötteen yli eikä sitä koskaan kirjaudu tai sijoiteta komentoriville.</translation>
    </message>
    <message>
        <source>Passphrase required</source>
        <translation>Vaadittu salauslause</translation>
    </message>
    <message>
        <source>An empty passphrase was not submitted. Enter the LUKS passphrase or choose Cancel.</source>
        <translation>Tyhjää salalausetta ei esitetty. Anna LUKS-salalause tai valitse Peruuta.</translation>
    </message>
    <message>
        <source>Unlock keyfile unavailable</source>
        <translation>Avaa avaintiedosto ei saatavilla</translation>
    </message>
    <message>
        <source>The LUKS passphrase could not be written to a private keyfile in %1; the unlock was not started.</source>
        <translation>LUKS-salalausetta ei voitu kirjoittaa %1:n yksityiselle avaintiedostolle; avausta ei aloitettu.</translation>
    </message>
    <message>
        <source>Configuration write unavailable</source>
        <translation>Asetuksen kirjoitusta ei ole saatavilla</translation>
    </message>
    <message>
        <source>The edited content could not be written to a private temporary file in %1; the write was not started.</source>
        <translation>Editoitua sisältöä ei voitu kirjoittaa %1:n yksityiseen väliaikaiseen tiedostoon; kirjoitusta ei aloitettu.</translation>
    </message>
    <message>
        <source>ERROR: cannot read the session log %1</source>
        <translation>VIROR: istuntolokia %1 ei voi lukea</translation>
    </message>
    <message>
        <source>Viewing a prior session log (read-only): %1</source>
        <translation>Edellisen istuntolokin katselu (vain luku): %1</translation>
    </message>
    <message>
        <source>Session log list refreshed.</source>
        <translation>Istuntolokilista päivitetty.</translation>
    </message>
    <message>
        <source>Note:</source>
        <translation>Huom:</translation>
    </message>
    <message>
        <source>Delete session log</source>
        <translation>Poista istuntoloki</translation>
    </message>
    <message>
        <source>Delete %1 permanently? This cannot be undone.</source>
        <translation>Poista %1 pysyvästi? Tätä ei voi perua.</translation>
    </message>
    <message>
        <source>%1 changed while the confirmation was open; the delete was refused.</source>
        <translation>%1 muuttui vahvistuksen ollessa auki; poisto hylättiin.</translation>
    </message>
    <message>
        <source>Unable to delete %1.</source>
        <translation>%1:ää ei voitu poistaa.</translation>
    </message>
    <message>
        <source>Device discovery filters updated.</source>
        <translation>Laitteen löytösuodattimet päivitetty.</translation>
    </message>
    <message>
        <source>Unknown Linux distribution</source>
        <translation>Tuntematon Linux-jakelu</translation>
    </message>
    <message>
        <source>sudo / gksu (no KAuth on this frontend)</source>
        <translation>sudo / gksu (ei KAuth tässä rintamassa)</translation>
    </message>
    <message>
        <source>Available</source>
        <translation>Saatavilla</translation>
    </message>
    <message>
        <source>Missing on this frontend</source>
        <translation>Kadonnut tästä rintamasta</translation>
    </message>
    <message>
        <source>Missing</source>
        <translation>Puuttuu</translation>
    </message>
    <message>
        <source>No repair tool is selected.</source>
        <translation>Korjaustyökalua ei ole valittu.</translation>
    </message>
    <message>
        <source>A helper command is already running.</source>
        <translation>Apukomento on jo käynnissä.</translation>
    </message>
    <message>
        <source>Restore Graphical Login is a host-scope stage on this legacy frontend; enter Host Maintenance to run it.</source>
        <translation>Palauta Graafinen Login on isäntä-scope vaiheessa tämän perinteisen rintaman; kirjoita isännän huolto suorittaa sen.</translation>
    </message>
    <message>
        <source>The selected scope has no resolved root component; use Refresh Devices and commit the repair target again.</source>
        <translation>Valitulla soveltamisalalla ei ole ratkaistua juurikomponenttia; käytä Päivitä laitteita ja toimita korjauskohde uudelleen.</translation>
    </message>
    <message>
        <source>This legacy frontend exposes no %1 action; the helper reports the capability as available.</source>
        <translation>Tämä perinteinen rintama ei paljasta mitään %1-toimintaa; auttaja raportoi kyvyistä.</translation>
    </message>
    <message>
        <source>Run this guarded repair action using the cached read-only diagnostic evidence. A confirmation is shown first.</source>
        <translation>Suorita tämä vartioitu korjaus käyttäen välimuistin diagnostisia todisteita. Ensin näytetään vahvistus.</translation>
    </message>
    <message>
        <source>Enabled in Settings</source>
        <translation>Käytössä asetuksissa</translation>
    </message>
    <message>
        <source>Disabled in Settings - enable it to include this stage</source>
        <translation>Ei käytössä Asetukset - voit sisällyttää tämän vaiheen</translation>
    </message>
    <message>
        <source>Unavailable: %1</source>
        <translation>Ei saatavilla: %1</translation>
    </message>
    <message>
        <source>
Unavailable: %1</source>
        <translation>
Ei saatavilla: %1</translation>
    </message>
    <message>
        <source>Repair tool unavailable</source>
        <translation>Korjaustyökalua ei saatavilla</translation>
    </message>
    <message>
        <source>Confirm repair</source>
        <translation>Vahvista korjaus</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Your saved selection is kept and its availability is re-checked when diagnostics for this scope complete.</source>
        <translation>Tälle näyttämölle ei ole välimuistia. Tallennettu valintasi säilytetään ja sen saatavuus tarkistetaan uudelleen, kun tämän soveltamisalan diagnostiikka on valmis.</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Run diagnostics for the selected scope to populate the Full Repair plan; your selection is saved once the stage becomes available.</source>
        <translation>Tälle näyttämölle ei ole välimuistia. Suorita diagnostiikka valitun soveltamisalan populate Full Repair suunnitelma; valinta tallennetaan, kun vaihe tulee saataville.</translation>
    </message>
    <message>
        <source>Unknown Full Repair stage.</source>
        <translation>Tuntematon täyskorjausvaihe.</translation>
    </message>
    <message>
        <source>No Full Repair stages are selected or available; use Configure Plan... to choose the stages.</source>
        <translation>Full Repair -vaiheita ei ole valittu tai käytettävissä; käytä Configure Plan...</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab to establish the session.</source>
        <translation>Administrator authority is required; press Authorize on the Systems or Repair välilehti määrittää istunnon.</translation>
    </message>
    <message>
        <source>Runs the selected stages using the cached read-only diagnostic evidence after privilege confirmation.</source>
        <translation>Suorittaa valitut vaiheet käyttäen välimuistissa vain diagnostisia todisteita etuoikeutensa vahvistuksen jälkeen.</translation>
    </message>
    <message>
        <source>No Full Repair stages selected - use Configure Plan... or Settings.</source>
        <translation>Ei täyskorjausvaiheita valittu - käytä Asetukset... tai Asetukset.</translation>
    </message>
    <message>
        <source>1 stage selected</source>
        <translation>1 vaihe valittu</translation>
    </message>
    <message>
        <source>%1 stages selected</source>
        <translation>%1-vaiheet valittu</translation>
    </message>
    <message>
        <source>No repair stages are selected. Use Configure Plan... to choose the stages Full Repair will run.</source>
        <translation>Korjausvaiheita ei ole valittu. Käytä Configure Plan... valita vaiheet Full Repair suoritetaan.</translation>
    </message>
    <message>
        <source>No selected stages are available. %1</source>
        <translation>Valittuja vaiheita ei ole saatavilla. %1</translation>
    </message>
    <message>
        <source>Full Repair unavailable</source>
        <translation>Täysi korjaus puuttuu</translation>
    </message>
    <message>
        <source>Run the Full Repair plan?

The selected stages run in order through the helper&apos;s guarded repair command:

</source>
        <translation>Tehdä koko korjaussuunnitelma?

Valitut vaiheet kulkevat järjestyksessä auttajan suojatun korjauskomennon kautta:

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
Auttaja pitää jokaisen ajoajan ennen lentoa; vaihe, joka epäonnistuu pysäyttää suunnitelman.</translation>
    </message>
    <message>
        <source>Running %1 through the privileged helper... The Logs tab keeps the complete transcript.</source>
        <translation>Ajan %1 kautta etuoikeutettu apuri... Lokit-välilehti pitää koko tekstin.</translation>
    </message>
    <message>
        <source>Shell scope required</source>
        <translation>Shell-alue vaaditaan</translation>
    </message>
    <message>
        <source>No administrator session is active.

Press Authorize on the Systems or Repair tab to establish the session, or enter Host Maintenance / commit a repair target on the Systems tab; the chroot shell then reuses the cached authorization.</source>
        <translation>Mikään ylläpitäjän istunto ei ole aktiivinen.

Lehdistölle Authorize on Systems or Repair välilehden perustaa istunnon, tai syöttää Host Huolto / toimittaa korjauskohteen Systems välilehden; Chroot kuori sitten uudelleenkäyttää välimuistin valtuutus.</translation>
    </message>
    <message>
        <source>Shell command required</source>
        <translation>Shell-komento vaaditaan</translation>
    </message>
    <message>
        <source>Enter the command to run first.</source>
        <translation>Syötä komento ajaa ensin.</translation>
    </message>
    <message>
        <source>Confirm running-host command</source>
        <translation>Vahvista juoksu- isäntäkomento</translation>
    </message>
    <message>
        <source>Run this command as root on the protected running host?

%1

The helper keeps its runtime preflights; the command is passed as one argument and is never interpreted by the GUI.</source>
        <translation>Suorittaa tämän komennon juurina suojattuun käynnissä olevaan isäntään?

%1

Auttaja pitää ajoaikansa ennen lentoa; komentoa käytetään yhtenä argumenttina eikä sitä koskaan tulkita käyttöliittymässä.</translation>
    </message>
    <message>
        <source>Running %1...</source>
        <translation>%1...</translation>
    </message>
    <message>
        <source>Authorization scope required</source>
        <translation>Valtuutuksen laajuus</translation>
    </message>
    <message>
        <source>administrator authorization</source>
        <translation>Hallinnointi</translation>
    </message>
    <message>
        <source>Administrator authorization</source>
        <translation>Hallintoviranomaisen valtuutus</translation>
    </message>
    <message>
        <source>Administrator authorization is required for %1.

Enter the password for %2 (sudo). It is used only for this sudo authentication, is sent over a pipe and is never logged or placed on a command line. The authorization is cached for this session and reused by diagnostics and repairs.</source>
        <translation>%1:lle tarvitaan hallintoviranomaisen lupa.

Anna %2:n (sudo) salasana. Sitä käytetään vain tähän sudo-tunnistukseen, lähetetään putkelle eikä sitä koskaan kirjaudu tai sijoiteta komentoriville. Valtuutus on välimuistissa tätä istuntoa varten ja käyttää uudelleen diagnostiikka ja korjaukset.</translation>
    </message>
    <message>
        <source>your account</source>
        <translation>tilisi</translation>
    </message>
    <message>
        <source>Password required</source>
        <translation>Salasana vaaditaan</translation>
    </message>
    <message>
        <source>An empty password was not submitted. Enter the sudo password or choose Cancel.</source>
        <translation>Tyhjää salasanaa ei toimitettu. Anna sudo salasana tai valitse Peruuta.</translation>
    </message>
    <message>
        <source>Administrator authorization failed</source>
        <translation>Hallinnoijan valtuutus epäonnistui</translation>
    </message>
    <message>
        <source>sudo did not accept the password: %1

The command was not started.</source>
        <translation>sudo ei hyväksynyt salasanaa: %1

Komentoa ei aloitettu.</translation>
    </message>
    <message>
        <source>elevation requires an interactive sudo password; run the smoke as root or after `sudo -S -v` with --elevate &apos;sudo -n&apos;</source>
        <translation>korkeussijainti vaatii interaktiivisen sudo salasanan; suorita savu juurina tai niiden jälkeen ...</translation>
    </message>
    <message>
        <source>No administrator session is active for %1.

Press Authorize on the Systems or Repair tab to establish the session now, or enter Host Maintenance / commit a repair target on the Systems tab; diagnostics and repairs then reuse the cached authorization.</source>
        <translation>%1:n ylläpitäjäistunto ei ole aktiivinen.

Paina Authorize on Systems tai Korjaa välilehti perustaa istunnon nyt, tai kirjoita isännän huolto / tee korjauskohde Systems välilehden; diagnostiikka ja korjaukset sitten uudelleen välimuistin valtuutus.</translation>
    </message>
    <message>
        <source>Administrator authorization expired</source>
        <translation>Hallinnointiviranomaisen lupa päättyi</translation>
    </message>
    <message>
        <source>The cached administrator authorization for %1 expired or was refused.

Press Authorize on the Systems or Repair tab to re-establish the session, then run the command again. No command was started.</source>
        <translation>%1:n välimuistin ylläpitäjän lupa päättyi tai hylättiin.

Paina Authorize on Systems tai Korjaa välilehti palauttaa istunnon, sitten ajaa komento uudelleen. Komentoa ei aloitettu.</translation>
    </message>
    <message>
        <source>Privileged operation completed successfully. Administrator authorization remains active for this session.</source>
        <translation>Ensisijainen operaatio suoritettu onnistuneesti. Hallitsijan valtuutus on edelleen aktiivinen tässä istunnossa.</translation>
    </message>
    <message>
        <source>Privileged operation stopped with an error. Administrator authorization remains active; review the output before closing.</source>
        <translation>Etuoikeutettu operaatio päättyi virheellä. Hallitsijan valtuutus pysyy aktiivisena; tarkista tuloste ennen sulkemista.</translation>
    </message>
    <message>
        <source>Inspection is read-only.

</source>
        <translation>Tarkastus on vain luettava.

</translation>
    </message>
    <message>
        <source>Configuration unavailable</source>
        <translation>Asetukset eivät ole saatavilla</translation>
    </message>
    <message>
        <source>The helper could not read %1:

%2</source>
        <translation>Auttaja ei voinut lukea %1:ää:

%2</translation>
    </message>
    <message>
        <source>Configuration write failed</source>
        <translation>Asetuksen kirjoitus epäonnistui</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
Result: mapping opened for this recovery session.</source>
        <translation>Tila: lukittu
Komponentti: %1
Mapper: %2
Menetelmä: auttimen avaus (cryptsetup; salauslauseen kautta mode-600 avaintiedosto, poistettu käytön jälkeen)
Tulos: kartoitus avattu tätä palautumisistuntoa varten.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: the passphrase was not accepted; retry offered.</source>
        <translation>Tila: lukittu
Komponentti: %1
Menetelmä: auttimen avaus (cryptsetup)
Virhe: Salasanaa ei hyväksytty; yritä uudelleen.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: %2</source>
        <translation>Tila: lukittu
Komponentti: %1
Menetelmä: auttimen avaus (cryptsetup)
Virhe: %2</translation>
    </message>
    <message>
        <source>Passphrase not accepted</source>
        <translation>Tunnuslausetta ei hyväksytä</translation>
    </message>
    <message>
        <source>The LUKS passphrase was not accepted.

Try again?</source>
        <translation>LUKS-salalausetta ei hyväksytty.

Yritä uudelleen?</translation>
    </message>
    <message>
        <source>[-] %1 - not run (the plan stopped before reaching this stage)</source>
        <translation>[-] %1 - ei käynnissä (suunnitelma pysähtyi ennen tämän vaiheen saavuttamista)</translation>
    </message>
    <message>
        <source>filesystem</source>
        <translation>tiedostojärjestelmä</translation>
    </message>
    <message>
        <source>[-] %1 - no file system errors found - no changes</source>
        <translation>[-] %1 - tiedostojärjestelmän virheitä ei löytynyt - ei muutoksia</translation>
    </message>
    <message>
        <source>[FAIL] %1 - not reported</source>
        <translation>[FAIL] %1 - ei raportoitu</translation>
    </message>
    <message>
        <source>%1 stage(s) failed; review the helper output in Logs.</source>
        <translation>%1-vaihe [s] epäonnistui; tarkista auttimen tuloste lokeissa.</translation>
    </message>
    <message>
        <source>the plan stopped before any stage completed.</source>
        <translation>suunnitelma päättyi ennen minkään vaiheen päättymistä.</translation>
    </message>
    <message>
        <source>no repair was needed; cached diagnostics remain valid.</source>
        <translation>Korjauksia ei tarvittu; välimuistidiagnostiikka on edelleen voimassa.</translation>
    </message>
    <message>
        <source>repair completed; cached diagnostics were invalidated and must be regenerated.</source>
        <translation>Korjaaminen suoritettu; välimuistidiagnostiikka mitätöitiin ja se on uusittava.</translation>
    </message>
    <message>
        <source>%1 results: [OK] %2 successful | [FAIL] %3 failed | [-] %4 no repair needed - %5</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Re-run Diagnostic</source>
        <translation>Uudelleenajodiagnostiikka</translation>
    </message>
    <message>
        <source>Unavailable</source>
        <translation>Ei saatavilla</translation>
    </message>
    <message>
        <source>Commit this physical drive as the repair target.</source>
        <translation>Toimita tämä fyysinen asema korjauskohteena.</translation>
    </message>
    <message>
        <source>Protection:</source>
        <translation>Suojaus:</translation>
    </message>
    <message>
        <source>Host shell</source>
        <translation>Komentotulkki</translation>
    </message>
    <message>
        <source>Run on Host</source>
        <translation>Suorita isäntä</translation>
    </message>
    <message>
        <source>Host Shell</source>
        <translation>Isäntä Shell</translation>
    </message>
    <message>
        <source>The read-only probe found no editable target configuration file in this target. %1</source>
        <translation>Vain lukuanturi ei löytänyt muokattavissa olevaa kohdeasetustiedostoa tästä kohteesta. %1</translation>
    </message>
    <message>
        <source>Not present in the selected target (omitted): %1</source>
        <translation>Ei valittuun kohteeseen (merkitty): %1</translation>
    </message>
    <message>
        <source>Run diagnostics to probe which target configuration files exist; the helper&apos;s read-only probe decides the list.</source>
        <translation>Suorita diagnostiikka luotaimeen, jonka asetustiedostot ovat olemassa; auttajan lukuluotain päättää luettelon.</translation>
    </message>
    <message>
        <source>Probed read-only by the helper; a saved edit invalidates the cached diagnostics.</source>
        <translation>Vain apurin lukema; tallennettu muokkaus mitätöi välimuistin diagnostiikan.</translation>
    </message>
    <message>
        <source>Host maintenance: %1</source>
        <translation>Huolto: %1</translation>
    </message>
    <message>
        <source>unresolved</source>
        <translation>ratkaisematon</translation>
    </message>
    <message>
        <source>Target: %1 + %2</source>
        <translation>Kohde: %1 + %2</translation>
    </message>
    <message>
        <source>Host command</source>
        <translation>Komento</translation>
    </message>
    <message>
        <source>Execute a command on the running host as root.</source>
        <translation>Suorita komento käynnissä isännälle juurina.</translation>
    </message>
    <message>
        <source>Execute a command inside the selected repair system as root.</source>
        <translation>Suorita komento valitun korjausjärjestelmän sisällä juurina.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab (or re-enter Host Maintenance / re-commit the repair target) to authorize this session.</source>
        <translation>Administrator authority is required; paina Authorize on the Systems or Repair sale (tai re-enter Isäntä Huolto / uudelleen sitoutua korjauskohde) jotta tämä istunto.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root on the running host through the helper&apos;s guarded host-shell verb.</source>
        <translation>Suorita yksi arvosteltu komento juuri käynnissä isäntä läpi auttaja vartioitu isäntäkuori verbi.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root inside the target chroot through the helper&apos;s guarded shell verb.</source>
        <translation>Suorita yksi arvosteltu komento juurina kohteen sisällä Chroot kautta auttaja vartioitu kuori verbi.</translation>
    </message>
    <message>
        <source>Run a command on the running host as root (sudo is not needed). Commands are executed directly on the active system; output is kept in this window and in the application log.</source>
        <translation>Suorita käynnissä olevan isännän käsky juurina (suota ei tarvita). Komennot suoritetaan suoraan aktiivisessa järjestelmässä; tuloste säilytetään tässä ikkunassa ja sovelluslokissa.</translation>
    </message>
    <message>
        <source>Run a command inside the selected repair system as root (sudo is not needed). Commands are executed one at a time in a fresh chroot and cannot answer interactive prompts; use non-interactive flags such as apt-get -y upgrade. Output is kept in this window and in the application log.</source>
        <translation>Suorita komento valitun korjausjärjestelmän sisällä juurina (sudoa ei tarvita). Komennot suoritetaan yksi kerrallaan tuoreissa chroot eikä voi vastata interaktiivisia kehotuksia; käytä ei-interaktiiviset liput kuten apt-get-y päivitys. Tulosta säilytetään tässä ikkunassa ja sovelluslokissa.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize or re-enter Host Maintenance / commit a repair target.</source>
        <translation>Ei ylläpitäjän istunto on aktiivinen; paina Authorize or re-enter Host Maintenance / tee korjauskohde.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from the repaired system</source>
        <translation>1. Valitse lähdetiedostot tai kansiot korjatusta järjestelmästä</translation>
    </message>
    <message>
        <source>2. Choose destination on this host</source>
        <translation>2. Valitse kohde tässä isäntä</translation>
    </message>
    <message>
        <source>Add File Path...</source>
        <translation>Lisää tiedostopolku...</translation>
    </message>
    <message>
        <source>Add Folder Path...</source>
        <translation>Lisää kansion polku...</translation>
    </message>
    <message>
        <source>Browse...</source>
        <translation>Selaa...</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it (Host Maintenance does not provide a repair tree to browse).</source>
        <translation>Valitse järjestelmän korjausasema ennen sen sisällä olevan kohteen valintaa (Host Maintenance ei tarjoa selattavaa korjauspuuta).</translation>
    </message>
    <message>
        <source>Choose a host destination folder directly.</source>
        <translation>Valitse kohdekansio suoraan.</translation>
    </message>
    <message>
        <source>Copy the staged files and folders with cp -a, restore ownership with chown --reference and byte-compare every regular file afterwards.</source>
        <translation>Kopioi lavastetut tiedostot ja kansiot cp -a:lla, palauta omistajuus cownilla --referenssillä ja byte-vertaa jokainen säännöllinen tiedosto jälkikäteen.</translation>
    </message>
    <message>
        <source>File Copy unavailable</source>
        <translation>Tiedostokopiota ei ole saatavilla</translation>
    </message>
    <message>
        <source>Choose host destination folder</source>
        <translation>Valitse kohdekansio</translation>
    </message>
    <message>
        <source>Select a repair target</source>
        <translation>Valitse korjauskohde</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it.</source>
        <translation>Valitse järjestelmän korjausasema ennen kohdetta sen sisällä.</translation>
    </message>
    <message>
        <source>File Copy - Browse Target Folders</source>
        <translation>Tiedostokopio - Selaa kohdekansioita</translation>
    </message>
    <message>
        <source>Browse Target Folders</source>
        <translation>Selaa kohdekansioita</translation>
    </message>
    <message>
        <source>The helper could not list the repair-system folder:

%1</source>
        <translation>Auttaja ei voinut listata korjausjärjestelmäkansiota:

%1</translation>
    </message>
    <message>
        <source>BROWSE_ENTRY</source>
        <translation>BROWSE ENTRY</translation>
    </message>
    <message>
        <source>(choose this folder: %1)</source>
        <translation>(Valitse tämä kansio: %1)</translation>
    </message>
    <message>
        <source>(parent folder)</source>
        <translation>(esikansio)</translation>
    </message>
    <message>
        <source>Select repair-system destination</source>
        <translation>Valitse korjausjärjestelmän kohde</translation>
    </message>
    <message>
        <source>Choose a destination folder inside the repaired system (current folder: %1):</source>
        <translation>Valitse korjattuun järjestelmään kuuluva kohdekansio (nykyinen kansio: %1):</translation>
    </message>
    <message>
        <source>Folder</source>
        <translation>Kansio</translation>
    </message>
    <message>
        <source>Open</source>
        <translation>Avaa</translation>
    </message>
    <message>
        <source>(choose this folder:</source>
        <translation>(Valitse tämä kansio:</translation>
    </message>
    <message>
        <source>Add files to copy</source>
        <translation>Lisää tiedostoja kopioitavaksi</translation>
    </message>
    <message>
        <source>Add folder to copy</source>
        <translation>Lisää kansio kopioitavaksi</translation>
    </message>
    <message>
        <source>Stage at least one source and name a destination path first.</source>
        <translation>Vaihe vähintään yksi lähde ja nimetä kohde polku ensin.</translation>
    </message>
    <message>
        <source>Copy the %1 staged item(s) to %2?

The helper keeps its direction and path containment checks; a sensitive repair-system destination is refused unless the helper approves it, and every regular file is byte-compared after the copy.</source>
        <translation>Kopioi %1 lavastetut kohteet [s] %2?

Auttaja pitää suunnistus- ja reittirajoitustarkistuksensa; herkkä korjausjärjestelmäkohde hylätään, ellei auttaja hyväksy sitä, ja jokainen säännöllinen tiedosto on tavuvertainen kopion jälkeen.</translation>
    </message>
    <message>
        <source>File Copy - Copy and Verify</source>
        <translation>Tiedostokopio - Kopioi ja tarkista</translation>
    </message>
    <message>
        <source>File Copy - Preview Changes</source>
        <translation>Tiedostokopio - Esikatselumuutokset</translation>
    </message>
    <message>
        <source>Protected running host - details</source>
        <translation>Suojattu ajon isäntä - tiedot</translation>
    </message>
    <message>
        <source>not mounted (offline target)</source>
        <translation>ei asennettu (offline kohde)</translation>
    </message>
    <message>
        <source>Select a drive to see its details.</source>
        <translation>Valitse asema nähdäksesi sen yksityiskohdat.</translation>
    </message>
    <message>
        <source>Inspecting the selected component.</source>
        <translation>Tarkastan valitun osan.</translation>
    </message>
    <message>
        <source>Helper-confirmed by the last read-only diagnostics.</source>
        <translation>-Auttajan vahvistama viime diagnostiikka.</translation>
    </message>
    <message>
        <source>Read-only inventory only; run diagnostics to confirm.</source>
        <translation>Lue vain inventaario; suorita diagnostiikka varmistaa.</translation>
    </message>
    <message>
        <source>PROTECTED - running system; read-only details only</source>
        <translation>SUOJATTU - ajojärjestelmä; ainoastaan luettavat tiedot</translation>
    </message>
    <message>
        <source>Live / installer media - not selectable</source>
        <translation>Live / asentaja media - ei valittavissa</translation>
    </message>
    <message>
        <source>PROTECTED - running host scope</source>
        <translation>SUOJATTU - isäntäalueen käyttö</translation>
    </message>
    <message>
        <source>Unlock required before selection</source>
        <translation>Avaa vaaditaan ennen valintaa</translation>
    </message>
    <message>
        <source>Eligible repair candidate</source>
        <translation>Tukikelpoiset korjausehdokkaat</translation>
    </message>
    <message>
        <source>Drive:</source>
        <translation>Aja:</translation>
    </message>
    <message>
        <source>Detected target:</source>
        <translation>Havaittu kohde:</translation>
    </message>
    <message>
        <source>Pending inspection</source>
        <translation>Ennen tarkastusta</translation>
    </message>
    <message>
        <source>Model / label:</source>
        <translation>Malli / etiketti:</translation>
    </message>
    <message>
        <source>Status:</source>
        <translation>Tila:</translation>
    </message>
    <message>
        <source>Size:</source>
        <translation>Koko:</translation>
    </message>
    <message>
        <source>Connection:</source>
        <translation>Yhteys:</translation>
    </message>
    <message>
        <source>Filesystem:</source>
        <translation>Tiedostojärjestelmä:</translation>
    </message>
    <message>
        <source>UUID:</source>
        <translation>Hei.</translation>
    </message>
    <message>
        <source>Mounts:</source>
        <translation>Liitännät:</translation>
    </message>
    <message>
        <source>Running system protection unresolved</source>
        <translation>Juoksevan järjestelmän suojaus selvittämätön</translation>
    </message>
    <message>
        <source>No protected physical backing disk was identified</source>
        <translation>Suojattua fyysistä taustalevyä ei tunnistettu</translation>
    </message>
    <message>
        <source>Current running Linux system</source>
        <translation>Nykyinen Linux-järjestelmä</translation>
    </message>
    <message>
        <source>Critical mounts: %1</source>
        <translation>Kriittiset kiinnikkeet: %1</translation>
    </message>
    <message>
        <source>Read-only protected-running-system facts: the helper OS fact plus the inventory model, device path, size, transport and critical mounts. Nothing here is probed destructively.</source>
        <translation>Lue vain suojattu-juoksu-järjestelmä faktat: auttaja käyttöjärjestelmä fakta sekä varastomalli, laitteen polku, koko, kuljetus ja kriittiset liittimet. Täällä ei ole mitään tuhoisaa.</translation>
    </message>
    <message>
        <source>Select a drive to see unlock status.</source>
        <translation>Valitse asema nähdäksesi avaimen tilan.</translation>
    </message>
    <message>
        <source>State: protected
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
The protected running host cannot be unlocked or modified; unlock is available for an offline repair target only. Use Host Maintenance for the protected running host.</source>
        <translation>Valtio: suojattu
Komponentti: %1
Mapper: (ei mitään)
Menetelmä: auttimen avaus (cryptsetup; salauslauseen kautta mode-600 avaintiedosto, poistettu käytön jälkeen)
Suojattua ajon isäntää ei voi avata tai muokata; avaus on käytettävissä vain offline-korjaukseen. Käytä palvelinta suojatussa käyttöisännässä.</translation>
    </message>
    <message>
        <source>(none detected)</source>
        <translation>(ei havaittu)</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
A locked LUKS container is visible on this drive; press Unlock to open it for this recovery session.</source>
        <translation>Tila: lukittu
Komponentti: %1
Mapper: (ei mitään)
Menetelmä: auttimen avaus (cryptsetup; salauslauseen kautta mode-600 avaintiedosto, poistettu käytön jälkeen)
Asemalla näkyy lukittu LUKS-säiliö; paina Avaa avataksesi sen tätä palautumisistuntoa varten.</translation>
    </message>
    <message>
        <source>(visible mapper)</source>
        <translation>(näkyvä kartta)</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: already open before this session (visible mapper; Boot Bitch will reuse it and will not close it).</source>
        <translation>Tila: lukittu
Komponentti: %1
Mapper: %2
Menetelmä: auki jo ennen tätä istuntoa (näkyvä kartta; Boot Bitch käyttää sitä uudelleen eikä sulje sitä).</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper-confirmed mapping from the last read-only diagnostics.</source>
        <translation>Tila: lukittu
Komponentti: %1
Mapper: %2
Menetelmä: auttajavarmistettu kartoitus viimeisimmästä luku-vain diagnostiikasta.</translation>
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
        <translation>Tila: lukittu tai salattua osaa ei ole havaittu
Osa: (ei havaittu)
Mapper: (ei mitään)
Menetelmä: auttimen avaus (cryptsetup; salauslauseen kautta mode-600 avaintiedosto, poistettu käytön jälkeen)
Tätä asemaa varten ei tallennettu lukittua LUKS-komponenttia eikä avaustoimintoa.</translation>
    </message>
    <message>
        <source>unavailable - %1</source>
        <translation>Ei saatavilla - %1</translation>
    </message>
    <message>
        <source>Host maintenance:
%1</source>
        <translation>Huolto:
%1</translation>
    </message>
    <message>
        <source>Target:
%1 + %2</source>
        <translation>Kohde:
%1 + %2</translation>
    </message>
    <message>
        <source>Authorization required: diagnostics and repairs fail closed until you press Authorize.</source>
        <translation>Valtuutus vaaditaan: diagnostiikka ja korjaukset epäonnistuvat kunnes painat Authorization.</translation>
    </message>
    <message>
        <source>Re-establish the privileged helper session for the current scope now. The password is requested in the hidden-input modal and is never logged.</source>
        <translation>Palauta etuoikeutettu auttajan istunto nykyisen soveltamisalan. Salasanaa pyydetään piilotettuun modaaliin eikä sitä koskaan kirjaudu sisään.</translation>
    </message>
    <message>
        <source>Running...</source>
        <translation>Juokse...</translation>
    </message>
    <message>
        <source>Scope required</source>
        <translation>Soveltamisala</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope; this unlocks the gated actions.</source>
        <translation>Suorita kaikki - suorita kaikki saatavilla olevat luku-vain vianmääritys nykyisen soveltamisalan; tämä avaa aidatut toiminnot.</translation>
    </message>
    <message>
        <source>Select a target and wait for any running command first.</source>
        <translation>Valitse kohde ja odota mitä tahansa käynnissä olevaa komentoa ensin.</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic through the helper.</source>
        <translation>Suorita Diagnostiikka - suorita valittu luku vain vianmääritys auttimen kautta.</translation>
    </message>
    <message>
        <source>Run All for the current scope and refresh the read-only backend profile.</source>
        <translation>Suorita Kaikki nykyisen soveltamisalan ja päivittää luku-vain taustaosa profiili.</translation>
    </message>
    <message>
        <source>Read or edit the selected target configuration file through the guarded helper; a saved edit invalidates cached diagnostics.</source>
        <translation>Lue tai muokkaa valittua kohdeasetustiedostoa vartioidun apurin kautta; tallennettu muokkaus mitätöi välimuistidiagnostiikan.</translation>
    </message>
    <message>
        <source>Host Maintenance has no target-file editing; commit an offline repair target first.</source>
        <translation>Isäntä Kunnossapito ei ole kohde-tiedoston editointi; tee offline-korjauksen kohde ensin.</translation>
    </message>
    <message>
        <source>Commit an offline repair target first.</source>
        <translation>Toimita offline-korjauskohde ensin.</translation>
    </message>
    <message>
        <source>No target configuration file is available for this target; run diagnostics to probe the list.</source>
        <translation>Kohdekonfigurointitiedostoa ei ole saatavilla tälle kohteelle; suorita diagnostiikkaluetteloa varten.</translation>
    </message>
    <message>
        <source>Already Unlocked</source>
        <translation>Avattu</translation>
    </message>
    <message>
        <source>An unlocked Linux filesystem is already visible on this drive. Boot Bitch will reuse the existing mapper and will not close or reopen a mapping created by this recovery session. Mounting happens during diagnostics (read-only) and repairs (read-write); data filesystems are never auto-mounted on selection.</source>
        <translation>Asemalla näkyy jo lukitsematon Linux-tiedostojärjestelmä. Boot Bitch käyttää olemassa olevaa karttaa uudelleen eikä sulje tai avaa uudelleen tämän palautusistunnon luomaa kartoitusta. Asennus tapahtuu diagnostiikan aikana (vain luku) ja korjausten aikana (luku-kirjoitus); tiedostojärjestelmät eivät koskaan automaattisesti kiinnitä niitä valintaan.</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked; use Host Maintenance for the protected running host.</source>
        <translation>Suojattua ajon isäntää ei voi avata; käytä suojatun ajon isäntää.</translation>
    </message>
    <message>
        <source>Host Maintenance is the current scope, but the selected offline drive can still be unlocked.</source>
        <translation>Isäntähuolto on nykyinen soveltamisala, mutta valittu offline-asema voidaan silti avata.</translation>
    </message>
    <message>
        <source>Unlock %1 using cryptsetup through the privileged helper. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>Avaa %1 käyttäen cryptsetup kautta etuoikeutettu apuri. Salasana kulkee yksityisen avaintiedoston läpi eikä sitä koskaan sijoiteta komentoargumentteihin tai lokeihin.</translation>
    </message>
    <message>
        <source>The protected running system cannot be selected as a repair target; use Host Maintenance for the protected running host.</source>
        <translation>Suojattua ajojärjestelmää ei voida valita korjauskohteeksi; käytä suojatun ajoisännän isäntähuoltoa.</translation>
    </message>
    <message>
        <source>Live / installer media is read-only boot media and cannot be selected as a repair target.</source>
        <translation>Live / asentaja media on luku-avain media, eikä sitä voida valita korjauskohteeksi.</translation>
    </message>
    <message>
        <source>Unlock the encrypted volume first; Select Target becomes available after a Linux filesystem is detected.</source>
        <translation>Avaa salattu äänenvoimakkuus ensin; Valitse kohde tulee saataville sen jälkeen, kun Linux-tiedostojärjestelmä on havaittu.</translation>
    </message>
    <message>
        <source>Committed repair target. Repair, Diagnostics and File Copy target this physical drive until another drive is explicitly selected with Select Target.</source>
        <translation>Korjattu kohde. Korjaa, Diagnostics ja Tiedostokopio kohdistaa tämän fyysisen aseman kunnes toinen asema on nimenomaisesti valittu Valitse Target.</translation>
    </message>
    <message>
        <source>Commit %1 as the repair target; this leaves Host Maintenance and switches the scope to the selected drive.</source>
        <translation>Toimita %1 korjauskohteena; tämä jättää isäntähuollon ja siirtää soveltamisalan valitulle asemalle.</translation>
    </message>
    <message>
        <source>The running host target could not be detected.</source>
        <translation>Kohdetta ei voitu havaita.</translation>
    </message>
    <message>
        <source>Leave Host Maintenance and return to repair-target mode.</source>
        <translation>Jätä isäntä huolto ja palata korjaus-kohde tilassa.</translation>
    </message>
    <message>
        <source>Select the running host for deliberate guarded maintenance; the administrator authorization is requested here once and cached for the session.</source>
        <translation>Valitse ajettava isäntä tarkoituksellisesti vartioituun ylläpitoon; järjestelmänvalvojan valtuutusta pyydetään täällä kerran ja se välitetään istuntoon.</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a repair target.</source>
        <translation>Kohdetta ei voitu havaita; diagnostiikka tarvitsee korjauskohteen.</translation>
    </message>
    <message>
        <source>Committed target: %1 + %2</source>
        <translation>Sitoutunut tavoite: %1 + %2</translation>
    </message>
    <message>
        <source>Committed target: none (selection changed)</source>
        <translation>Kohde: ei mikään (valinta muuttunut)</translation>
    </message>
    <message>
        <source>The selected scope has no resolved Linux root component; Refresh Devices and commit the repair target again.</source>
        <translation>Valitulla soveltamisalalla ei ole ratkaistua Linuxin juurikomponenttia; Päivitä laitteet ja toimita korjauskohde uudelleen.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize on the Systems or Repair tab to re-establish it. Run All reuses the cached authorization and never prompts by itself.</source>
        <translation>Mikään ylläpitäjän istunto ei ole aktiivinen; paina Authorize on Systems tai Korjaus välilehti palauttaa sen. Suorita Kaikki käyttää uudelleen välimuistin valtuutusta eikä koskaan pyydä itse.</translation>
    </message>
    <message>
        <source>Run diagnostics for this scope to unlock the gated actions. Diagnostics are read-only and the only evidence source.</source>
        <translation>Suorita diagnostiikka tämän soveltamisalan avata aidattu toimia. Diagnostiikka on ainoa todiste.</translation>
    </message>
    <message>
        <source>A repair was not proven unchanged, so the cached diagnostics are invalidated. Run diagnostics again before another gated action.</source>
        <translation>Korjausta ei ole muutettu, joten välimuistin diagnostiikka mitätöidään. Suorita diagnostiikat uudelleen ennen toista aidattua toimintaa.</translation>
    </message>
    <message>
        <source>Gated actions reflect the cached capability lines; the helper still runs every runtime preflight when a command starts.</source>
        <translation>Aidatut toiminnot heijastavat välimuistin valmiuslinjoja; apuri toimii edelleen joka juoksuaika ennen lentoa, kun komento alkaa.</translation>
    </message>
    <message>
        <source>Regenerating diagnostics automatically</source>
        <translation>Regeneroidaan vianmääritys automaattisesti</translation>
    </message>
    <message>
        <source>Running diagnostic: %1</source>
        <translation>Suoritusdiagnostiikka: %1</translation>
    </message>
    <message>
        <source>Running all diagnostics</source>
        <translation>Kaikki diagnostiikat</translation>
    </message>
    <message>
        <source>Unlocking %1</source>
        <translation>Avataan %1</translation>
    </message>
    <message>
        <source>running host</source>
        <translation>käynnissä</translation>
    </message>
    <message>
        <source>offline target</source>
        <translation>offline- kohde</translation>
    </message>
    <message>
        <source>host maintenance active</source>
        <translation>isäntähuolto aktiivinen</translation>
    </message>
    <message>
        <source>repair target committed</source>
        <translation>Korjaa kohde</translation>
    </message>
    <message>
        <source>no committed scope</source>
        <translation>ei sitoumuksia</translation>
    </message>
    <message>
        <source>Current session</source>
        <translation>Nykyinen istunto</translation>
    </message>
    <message>
        <source>Log files (*.log);;All files (*)</source>
        <translation>Lokitiedostot (*.log);; Kaikki tiedostot (*)</translation>
    </message>
    <message>
        <source>Save log as</source>
        <translation>Tallenna loki nimellä</translation>
    </message>
    <message>
        <source>About Boot Bitch</source>
        <translation>Tietoja Boot Bitch:stä</translation>
    </message>
    <message>
        <source>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;&lt;p&gt;&lt;b&gt;Developer:&lt;/b&gt; CaptainMorgan12&lt;/p&gt;&lt;p&gt;This GUI is the package&apos;s entry point: a Qt 3.3.x frontend with the ported guarded helper for Debian Etch-era systems.&lt;/p&gt;&lt;p&gt;&lt;b&gt;Guarded repair mode:&lt;/b&gt; read-only diagnostics can inspect either the protected Running Host or an explicitly selected repair drive. The guarded Debian/APT package stages (interrupted configuration, broken dependencies, metadata refresh, upgrade), GRUB-legacy configuration regeneration, LUKS target unlock and guarded target-file editing run through the ported helper after confirmation; Host Maintenance enables the same supported stages natively on the active system after repeating the host identity and boot-mount checks.&lt;/p&gt;&lt;p&gt;The modern-only features - verified File Copy, Btrfs snapshot rollback, EFI/UKI and extlinux repair, boot-stack reconciliation and Make Default - are greyed with the helper&apos;s own probe reasons on this frontend; Arch/Alpine/Fedora package backends stay diagnostics-only here.&lt;/p&gt;&lt;p&gt;The first privileged action authorizes one cached administrator session per scope through a hidden-input modal (the Qt GUI remains unprivileged). It can be ended at any time from File - Lock Administrator Session.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;Boot Bitch %1 &lt;/h3&gt;&lt;p&gt;&lt;b&gt;Kehikko:&lt;/b&gt; KapteeniMorgan12&lt;/p&gt;&lt;p&gt; Tämä käyttöliittymä on paketin sisääntulopiste: Qt 3.3.x-etuosa, jossa on Debian Etch-era-järjestelmien vartioitu apulaite.&lt;/p&gt;&lt;p&gt;&lt;b&gt;Varattu korjaustila: &lt;/b&gt; lukuavain diagnostiikka voi tarkastaa joko suojatun juoksuohjaimen tai erikseen valitun korjausaseman. Debian/APT-pakettien vartioidut vaiheet (suljetut konfiguraatiot, rikkoutuneet riippuvuudet, metatiedon päivittäminen, päivitys), GRUB-legacy-asetusten uudistaminen, LUKS-kohteen avaaminen ja vartioitu kohdetiedostojen muokkaus kulkevat siirretyn apurin läpi vahvistuksen jälkeen; isäntähuolto mahdollistaa samat tuetut vaiheet natiivisti aktiivisessa järjestelmässä sen jälkeen, kun isäntäidentiteetti ja käynnistysmäärä on uusittu.&lt;/p&gt; &lt;p&gt; &lt;p&gt; Modernit ominaisuudet - todennettu File Copy, Btrfs scatchrock rollback, EFI/UKI ja extlinux korjaus, boot-stack täsmäytys ja Make Oletus - harmaat auttajan omat luotain syitä tässä rintamassa; Arch/Alpine/Fedora paketti taustat pysyvät diagnostiikka-vain täällä. &lt;/p&gt;&lt;p&gt; Ensimmäinen etuoikeutettu toimi mahdollistaa yhden välimuistin ylläpitäjän istunnon kutakin soveltamisalaa kohden piilotetun syöttötavan kautta (Qt-käyttöliittymä on edelleen vailla etuoikeutta). Se voidaan lopettaa milloin tahansa tiedostosta - Lukituksen ylläpitäjän istunto. &lt;/p&gt;</translation>
    </message>
</context>
</TS>
