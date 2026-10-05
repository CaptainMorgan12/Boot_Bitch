<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ru">
<context>
    <name>CapabilityChecker</name>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="248"/>
        <source>Device discovery</source>
        <translation>Открытие устройства</translation>
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
        <translation>ведущий</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="248"/>
        <source>Required for block-device inventory</source>
        <translation>Требуется для инвентаризации блок-устройства</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="249"/>
        <source>Filesystem identification</source>
        <translation>Идентификация файловой системы</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="249"/>
        <source>Used to identify filesystem metadata</source>
        <translation>Используется для идентификации метаданных файловой системы</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="250"/>
        <source>Mount inspection</source>
        <translation>Горная инспекция</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="250"/>
        <source>Used to understand active mounts</source>
        <translation>Используется для понимания активных креплений</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="251"/>
        <source>LUKS support</source>
        <translation>Поддержка LUKS</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="251"/>
        <source>Required to unlock encrypted targets</source>
        <translation>Требуется для разблокировки зашифрованных целей</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="252"/>
        <source>Btrfs support</source>
        <translation>Поддержка Btrfs</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="252"/>
        <source>Required for Btrfs inspection and snapshot rollback</source>
        <translation>Требуется для проверки Btrfs и мгновенного отката</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="253"/>
        <source>Bidirectional file copy</source>
        <translation>Двунаправленная копия файла</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="253"/>
        <location filename="../src/CapabilityChecker.cpp" line="255"/>
        <source>Host/Repair</source>
        <translation>Хост/ремонт</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="253"/>
        <source>Required for verified Host-to-Repair and Repair-to-Host transfer</source>
        <translation>Требуется для верифицированного переноса хоста в ремонт и ремонта в хост</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="254"/>
        <source>Chroot repair</source>
        <translation>Корневой ремонт</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="254"/>
        <source>Required for target-side repair commands</source>
        <translation>Требуется для команд восстановления на стороне цели</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="255"/>
        <source>Offline systemd repair</source>
        <translation>Оффлайн системный ремонт</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="255"/>
        <source>Used to restore graphical.target and the configured display manager without starting the target GUI</source>
        <translation>Используется для восстановления графики. Целевой и настроенный дисплеевый менеджер без запуска целевого графического интерфейса</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="256"/>
        <source>UEFI NVRAM inspection</source>
        <translation>Проверка UEFI NVRAM</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="256"/>
        <source>Used to preserve target EFI BootOrder during TUXEDO UKI rebuilds when efivars are available</source>
        <translation>Используется для сохранения целевого EFI BootOrder во время восстановления TUXEDO UKI, когда доступны эфивары.</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="257"/>
        <source>UKI verification</source>
        <translation>Проверка UKI</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="257"/>
        <source>Used to verify the kernel embedded in a rebuilt unified kernel image</source>
        <translation>Используется для проверки ядра, встроенного в восстановленное унифицированное изображение ядра</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="258"/>
        <source>GRUB EFI repair</source>
        <translation>Ремонт GRUB EFI</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="258"/>
        <source>Target/Host</source>
        <translation>Цель/хостинг</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="258"/>
        <source>Required only for conventional GRUB-based EFI systems</source>
        <translation>Требуется только для обычных систем EFI на основе GRUB.</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="259"/>
        <location filename="../src/CapabilityChecker.cpp" line="260"/>
        <source>GRUB configuration</source>
        <translation>Конфигурация GRUB</translation>
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
        <translation>Цель</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="259"/>
        <source>Debian-family GRUB helper</source>
        <translation>Помощник Debian-Family GRUB</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="260"/>
        <source>Portable GRUB configuration generator used by Arch and other non-Debian systems</source>
        <translation>Портативный генератор конфигурации GRUB, используемый Arch и другими не-Debian системами</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="261"/>
        <location filename="../src/CapabilityChecker.cpp" line="262"/>
        <location filename="../src/CapabilityChecker.cpp" line="263"/>
        <source>Initramfs rebuild</source>
        <translation>Восстановление Initramfs</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="261"/>
        <source>Debian-family initramfs helper</source>
        <translation>Семья Debian initramfs</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="262"/>
        <source>Arch-family initramfs generator</source>
        <translation>Архисемейный генератор initramfs</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="263"/>
        <source>Alternative initramfs generator used by Arch and other distributions</source>
        <translation>Альтернативный генератор initramfs, используемый Arch и другими дистрибутивами</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="264"/>
        <location filename="../src/CapabilityChecker.cpp" line="265"/>
        <source>Initramfs verification</source>
        <translation>Проверка Initramfs</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="264"/>
        <source>Read-only verification for mkinitcpio images</source>
        <translation>Проверка только для чтения изображений mkinitcpio</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="265"/>
        <source>Read-only verification for dracut images</source>
        <translation>Проверка только для чтения изображений dracut</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="266"/>
        <source>systemd-boot inspection</source>
        <translation>Инспекция systemd-boot</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="266"/>
        <location filename="../src/CapabilityChecker.cpp" line="267"/>
        <source>Host/Target</source>
        <translation>Хост/цель</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="266"/>
        <source>Read-only inspection of systemd-boot and generic UKI layouts</source>
        <translation>Проверка systemd-boot и общих макетов UKI</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="267"/>
        <source>Arch package manager</source>
        <translation>Арка менеджер пакетов</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="267"/>
        <source>Arch-family package database and transaction tool</source>
        <translation>База данных пакетов Arch-family и инструмент транзакций</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="268"/>
        <source>DKMS rebuild</source>
        <translation>Реконструкция DKMS</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="268"/>
        <source>Required only when target uses DKMS modules</source>
        <translation>Требуется только при использовании модулей DKMS.</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="269"/>
        <source>LVM inspection</source>
        <translation>Инспекция LVM</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="269"/>
        <source>Optional LVM storage-stack support</source>
        <translation>Опциональная поддержка хранилища LVM</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="270"/>
        <source>Software RAID</source>
        <translation>Программное обеспечение RAID</translation>
    </message>
    <message>
        <location filename="../src/CapabilityChecker.cpp" line="270"/>
        <source>Optional Linux MD RAID support</source>
        <translation>Поддержка Linux MD RAID</translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <location filename="../src/MainWindow.cpp" line="831"/>
        <source>Environment validation</source>
        <translation>Проверка окружающей среды</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="831"/>
        <source>Summarizes the selected system, protection state, mounted identity and inspection readiness.</source>
        <translation>Обобщает выбранную систему, состояние защиты, установленную идентичность и готовность к проверке.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="831"/>
        <source>Summarize the selected system, protection state and inspection readiness.</source>
        <translation>Обобщить выбранную систему, состояние защиты и инспекционной готовности.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="832"/>
        <source>Distribution and boot backend profile</source>
        <translation>Профиль Backend и Boot Backend</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="832"/>
        <source>Identifies the distribution family, package manager, initramfs generator, bootloader, ESP location and current guarded repair capability.</source>
        <translation>Идентифицирует семейство дистрибутивов, менеджер пакетов, генератор initramfs, загрузчик, местоположение ESP и текущую охраняемую возможность ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="832"/>
        <source>Detect the distribution, bootloader, ESP and current repair capability.</source>
        <translation>Обнаружение дистрибутива, загрузчика, ESP и возможности текущего ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="833"/>
        <source>Package state</source>
        <translation>Состояние упаковки</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="833"/>
        <source>Summarizes pending configuration, broken dependencies and missing package files for every detected package manager without changing the selected system.</source>
        <translation>Обобщает отложенную конфигурацию, неработающие зависимости и отсутствующие файлы пакетов для каждого обнаруженного менеджера пакетов без изменения выбранной системы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="833"/>
        <source>Summarize pending configuration and broken dependency state read-only.</source>
        <translation>Резюмируйте ожидающую конфигурацию и сломанное состояние зависимости только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="834"/>
        <source>Boot diagnostics</source>
        <translation>Диагностика бута</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="834"/>
        <source>Shows boot mounts, /boot and EFI contents plus storage evidence without changing the selected system.</source>
        <translation>Показывает загрузочные крепления, /boot и содержимое EFI, а также доказательства хранения без изменения выбранной системы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="834"/>
        <source>Show boot mounts, /boot and EFI contents read-only.</source>
        <translation>Показывайте загрузочные крепления, /boot и содержимое EFI только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="835"/>
        <source>Boot evidence and selection history</source>
        <translation>Доказательства сапоги и история отбора</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="835"/>
        <source>Correlates the detected boot chain, bootloader selection, kernel/initramfs, snapshots, EFI and unlock evidence, including whether one or more LUKS prompts are expected.</source>
        <translation>Сопоставляет обнаруженную цепочку загрузки, выбор загрузчика, ядро / initramfs, снимки, EFI и доказательства разблокировки, в том числе, ожидается ли одна или несколько подсказок LUKS.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="835"/>
        <source>Correlate the boot chain, loader, kernel/initramfs and unlock evidence.</source>
        <translation>Сопоставьте цепочку загрузки, загрузчик, ядро/initramfs и разблокируйте улики.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="836"/>
        <source>Kernel / initramfs</source>
        <translation>Ядро / initramfs</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="836"/>
        <source>Reviews kernel files and verifies matching initramfs images through a read-only inspection.</source>
        <translation>Обзор файлов ядра и проверка соответствия изображений initramfs с помощью проверки только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="836"/>
        <source>Verify kernel files and matching initramfs images read-only.</source>
        <translation>Проверяйте файлы ядра и сопоставляйте изображения initramfs только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="837"/>
        <location filename="../src/MainWindow.cpp" line="6719"/>
        <location filename="../src/MainWindow.cpp" line="20565"/>
        <source>GRUB configuration</source>
        <translation>Конфигурация GRUB</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="837"/>
        <source>Reviews GRUB configuration and /etc/default/grub without changing boot files.</source>
        <translation>Обзор конфигурации GRUB и /etc/default/grub без изменения файлов загрузки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="837"/>
        <source>Review GRUB configuration and /etc/default/grub read-only.</source>
        <translation>Обзор конфигурации GRUB и /etc/default/grub только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="838"/>
        <source>EFI / UKI boot state</source>
        <translation>EFI / UKI состояние загрузки</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="838"/>
        <source>Inspects the selected ESP, vendor or generic UKI images, systemd-boot loader files, embedded kernel/cmdline data and firmware entries with PARTUUID ownership classification.</source>
        <translation>Проверяет выбранные ESP, изображения поставщика или общие изображения UKI, файлы загрузчика systemd-boot, встроенные данные ядра / cmdline и записи прошивки с классификацией собственности PARTUUID.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="838"/>
        <source>Inspect the ESP, UKI images and firmware entries.</source>
        <translation>Проверьте изображения ESP, UKI и записи прошивки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="839"/>
        <location filename="../src/MainWindow.cpp" line="6715"/>
        <location filename="../src/MainWindow.cpp" line="20494"/>
        <source>Graphical login / display manager</source>
        <translation>Графический логин / дисплеев менеджер</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="839"/>
        <source>Reviews graphical.target, the configured display manager (for example SDDM, GDM/GDM3, LightDM, or another systemd manager), installed desktop packages, and recent boot/journal evidence without starting the GUI.</source>
        <translation>Обзоры graphical.target, настроенного дисплея (например, SDDM, GDM/GDM3, LightDM или другого системного менеджера), установленных пакетов рабочего стола и недавних доказательств загрузки/журнала без запуска GUI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="839"/>
        <source>Review the configured display manager and login target.</source>
        <translation>Просмотрите настроенный менеджер отображения и цель входа.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="840"/>
        <source>Graphics / GPU driver stack</source>
        <translation>Графика / стек драйверов GPU</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="840"/>
        <source>Reviews the kernel GPU driver: detected hardware, the loaded and blacklisted modules, installed driver packages, the expected driver, per-kernel DKMS build status, how far this boot reached and actionable journal driver errors.</source>
        <translation>Обзор драйвера GPU ядра: обнаруженное оборудование, загруженные и занесенные в черный список модули, установленные пакеты драйверов, ожидаемый драйвер, состояние сборки DKMS на ядро, насколько эта загрузка достигла и осуществимых ошибок драйвера журнала.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="840"/>
        <source>Review the kernel GPU driver and per-kernel DKMS build status read-only.</source>
        <translation>Просмотрите драйвер GPU ядра и DKMS сборки на ядро только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="841"/>
        <source>Boot errors</source>
        <translation>Ошибки загрузки</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="841"/>
        <source>Reads recent error-priority entries from the running host or selected repair system&apos;s persistent journal when available.</source>
        <translation>Читает последние записи с приоритетом ошибки из постоянного журнала запущенного хоста или выбранной системы ремонта, когда это доступно.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="841"/>
        <source>Read recent boot error-priority journal entries.</source>
        <translation>Прочитайте последние записи журнала с ошибкой загрузки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="842"/>
        <source>Disk usage</source>
        <translation>Использование диска</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="842"/>
        <source>Summarizes filesystem capacity/free space for the running host or read-only repair target.</source>
        <translation>Обобщает емкость файловой системы / свободное пространство для запуска хоста или цели восстановления только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="842"/>
        <source>Summarize filesystem capacity and free space.</source>
        <translation>Обобщение емкости файловой системы и свободного пространства.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="843"/>
        <source>File systems</source>
        <translation>Файловые системы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="843"/>
        <source>Runs the read-only file system check for the running host or selected repair system&apos;s root, /boot, ESP and /home filesystems and reports each device&apos;s check tool and result without changing anything.</source>
        <translation>Запускает проверку файловой системы только для чтения для запуска хоста или выбранной системы восстановления root, /boot, ESP и /home файловых систем и сообщает инструмент проверки каждого устройства и результат, ничего не меняя.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="843"/>
        <source>Run the read-only file system check and report each device&apos;s result.</source>
        <translation>Запустите проверку файловой системы только для чтения и сообщите о результатах каждого устройства.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="844"/>
        <source>fstab</source>
        <translation>нож</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="844"/>
        <source>Displays the running host or selected repair system&apos;s fstab; repair-system inspection is mounted read-only.</source>
        <translation>Отображает запущенный хост или выбранную систему ремонта fstab; проверка ремонтной системы устанавливается только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="844"/>
        <source>Display the selected system&apos;s fstab read-only.</source>
        <translation>Отображение fstab выбранной системы только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="845"/>
        <source>Btrfs status</source>
        <translation>Btrfs статус</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="845"/>
        <source>Shows Btrfs filesystem and subvolume information for the running host or selected repair system.</source>
        <translation>Показывает файловую систему Btrfs и информацию об объеме для запуска хоста или выбранной системы восстановления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="845"/>
        <source>Show Btrfs filesystem and subvolume information.</source>
        <translation>Показать файловую систему Btrfs и информацию об объеме.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="846"/>
        <source>Mapper status</source>
        <translation>Статус медведя</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="846"/>
        <source>Shows selected mapper ancestry, device-mapper state and cryptsetup status when available.</source>
        <translation>Показывает выбранную родословную картографа, состояние карты устройства и статус cryptsetup, когда это доступно.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="846"/>
        <source>Show device-mapper ancestry and cryptsetup state.</source>
        <translation>Покажите родословную устройства и состояние криптозащиты.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="847"/>
        <source>LUKS / crypttab</source>
        <translation>LUKS/crypttab</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="847"/>
        <source>Shows LUKS/mapped ancestry plus crypttab and fstab mapper references.</source>
        <translation>Показывает LUKS/картографическую родословную плюс ссылки на crypttab и fstab mapper.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="847"/>
        <source>Show LUKS ancestry plus crypttab and fstab mapper references.</source>
        <translation>Покажите родословную LUKS плюс ссылки на crypttab и fstab mapper.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="848"/>
        <location filename="../src/MainWindow.cpp" line="16599"/>
        <source>Full diagnostic report</source>
        <translation>Полный диагностический отчет</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="848"/>
        <source>Combines all read-only diagnostics for the selected scope in one privileged inspection session.</source>
        <translation>Комбинирует всю диагностику только для чтения для выбранной области в одной привилегированной инспекционной сессии.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="848"/>
        <source>Combine every read-only diagnostic for the selected scope.</source>
        <translation>Объедините каждую диагностику только для чтения для выбранной области.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1317"/>
        <source>akmods preflight found no build tree for installed kernel %1; install the matching kernel-devel packages and retry.</source>
        <translation>В предполетном полете akmods не было найдено дерева сборки для установленного ядра %1; установите соответствующие пакеты ядра и повторите попытку.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1318"/>
        <source>akms preflight found no build tree for installed kernel %1; install the matching headers and retry.</source>
        <translation>В предполетном полете akms не было найдено дерева сборки для установленного ядра %1; установить соответствующие заголовки и повторную попытку.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1319"/>
        <source>Alpine uses OpenRC, mkinitfs and syslinux/extlinux; boot-stack reconciliation is not enabled (run the initramfs and extlinux stages separately).</source>
        <translation>Alpine использует OpenRC, mkinitfs и syslinux/extlinux; выверка загрузочного стека не включена (запуск этапов initramfs и extlinux отдельно).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1320"/>
        <source>Alpine boot-stack reconciliation is not enabled (run the initramfs and GRUB stages separately).</source>
        <translation>Выверка альпийского загрузочного стека не включена (запуск этапов initramfs и GRUB отдельно).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1321"/>
        <source>Alpine DKMS preflight found no build tree for installed kernel %1; install the matching headers and retry.</source>
        <translation>Альпийский предполет DKMS не обнаружил дерева сборки для установленного ядра %1; установите соответствующие заголовки и повторите попытку.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1322"/>
        <source>Alpine EFI-stub host default selection is not implemented; use the Alpine EFI repair stage for entry reconciliation.</source>
        <translation>Выбор по умолчанию хоста Alpine EFI-stub не реализован; для согласования входа используйте этап ремонта Alpine EFI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1323"/>
        <source>Alpine syslinux-EFI boot detected (EFI/syslinux/syslinux.efi); Make Default is not implemented for this backend.</source>
        <translation>Обнаружена загрузка Alpine syslinux-EFI (EFI/syslinux/syslinux.efi); Make Default не реализован для этого бэкэнда.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1324"/>
        <source>Alpine syslinux-EFI boot detected (EFI/syslinux/syslinux.efi); guarded repair is not implemented.</source>
        <translation>Обнаружена альпийская загрузка syslinux-EFI (EFI/syslinux/syslinux.efi); охраняемый ремонт не осуществляется.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1325"/>
        <source>Alpine&apos;s main repository has no proprietary NVIDIA DKMS driver; the GPU driver swap is not available on this target.</source>
        <translation>Основное хранилище Alpine не имеет собственного драйвера NVIDIA DKMS; своп драйвера GPU недоступен для этой цели.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1326"/>
        <source>apk simulated no package changes and the package state is byte-identical.</source>
        <translation>apk смоделировал отсутствие изменений пакета, и состояние пакета является байт-идентичным.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1327"/>
        <source>APT package lists are byte-identical and no repository index was fetched.</source>
        <translation>Списки пакетов APT являются байт-идентичными, и индекс репозитория не был получен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1328"/>
        <source>Standalone APK metadata refresh is not available on Alpine; use Upgrade installed packages for one guarded apk transaction.</source>
        <translation>Автономное обновление метаданных APK недоступно на Alpine; используйте установленные пакеты обновления для одной защищенной транзакции apk.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1329"/>
        <source>Standalone APT metadata refresh is not available on Arch; use Upgrade installed packages for one full pacman transaction.</source>
        <translation>Автономное обновление метаданных APT недоступно на Arch; используйте установленные пакеты обновления для одной полной транзакции pacman.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1330"/>
        <source>Arch boot-stack reconciliation requires available initramfs, GRUB and EFI repair prerequisites.</source>
        <translation>Для согласования загрузочного стека требуется наличие необходимых условий для ремонта initramfs, GRUB и EFI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1331"/>
        <source>Arch DKMS preflight found no build tree for installed kernel %1; install the matching headers and retry.</source>
        <translation>Arch DKMS preflight не нашел дерева сборки для установленного ядра %1; установите соответствующие заголовки и повторите попытку.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1332"/>
        <location filename="../src/MainWindow.cpp" line="1333"/>
        <source>Backends: %1</source>
        <translation>Источник: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1334"/>
        <source>BLS is not enabled; the default is a generated menuentry.</source>
        <translation>BLS не включен; по умолчанию генерируется меню.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1335"/>
        <source>The initramfs backend is booster, which the current repair implementation does not handle.</source>
        <translation>Бэкэнд initramfs является бустером, с которым не справляется текущая реализация ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1336"/>
        <source>initramfs, EFI/UKI and GRUB artifacts are byte-identical.</source>
        <translation>Артефакты initramfs, EFI/UKI и GRUB являются байтоидентичными.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1337"/>
        <source>initramfs and GRUB2 artifacts are byte-identical.</source>
        <translation>Артефакты initramfs и GRUB2 являются байт-идентичными.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1338"/>
        <source>Requires available initramfs and GRUB repair prerequisites.</source>
        <translation>Требуется наличие необходимых условий для ремонта initramfs и GRUB.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1339"/>
        <source>Embedded cmdline does not reference the live LUKS UUID %1.</source>
        <translation>Встроенный cmdline не ссылается на Live LUKS UUID %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1340"/>
        <source>Embedded cmdline does not reference the live root UUID %1.</source>
        <translation>Встроенный cmdline не ссылается на UUID %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1341"/>
        <source>Embedded cmdline does not select the live Btrfs subvolume /%1.</source>
        <translation>Встроенная cmdline не выбирает живой подобъем Btrfs /%1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1342"/>
        <source>default.target and display-manager.service were already correct.</source>
        <translation>default.target и display-manager.service уже были корректны.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1343"/>
        <source>The display manager configuration is already correct, but the last boot stopped at a text console (the display manager did not start); the failure is likely elsewhere (driver or service crash).</source>
        <translation>Конфигурация дисплея уже верна, но последняя загрузка остановилась на текстовой консоли (дисплеер не запускался); сбой, скорее всего, в другом месте (автомобиль или сбой службы).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1344"/>
        <source>DKMS preflight found no build tree for installed kernel %1; install the matching kernel-devel packages and retry.</source>
        <translation>В предполетном полете DKMS не было найдено дерева сборки для установленного ядра %1; установите соответствующие пакеты ядра и повторите попытку.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1345"/>
        <source>dnf4 is not supported by the guarded rpm backend.</source>
        <translation>dnf4 не поддерживается защищенным rpm backend.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1346"/>
        <source>No missing package files were detected; dnf5 check is reported as evidence.</source>
        <translation>Отсутствующие файлы пакетов не были обнаружены; проверка dnf5 является доказательством.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1347"/>
        <source>The dnf5 repository metadata cache is byte-identical.</source>
        <translation>Кэш метаданных хранилища dnf5 является байт-идентификационным.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1348"/>
        <source>dnf5 simulated no package changes and the rpm database is byte-identical.</source>
        <translation>dnf5 имитирует отсутствие изменений пакета, а база данных rpm является байт-идентичной.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1349"/>
        <source>dpkg reported no packages pending configuration.</source>
        <translation>dpkg не сообщил ни о каких пакетах в ожидании конфигурации.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1350"/>
        <source>Alpine uses apk; dpkg configuration is not available on Alpine.</source>
        <translation>Alpine использует apk; конфигурация dpkg недоступна на Alpine.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1351"/>
        <source>dpkg configuration is not available on Arch; use the Arch package transaction stages instead.</source>
        <translation>Конфигурация dpkg недоступна на Arch; вместо этого используйте этапы транзакций пакета Arch.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1352"/>
        <source>Fedora uses rpm/dnf; dpkg configuration is not available.</source>
        <translation>Fedora использует rpm/dnf; конфигурация dpkg недоступна.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1353"/>
        <source>EFI boot artifacts and firmware entries are byte-identical.</source>
        <translation>Загрузочные артефакты EFI и записи прошивки являются байт-идентичными.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1354"/>
        <source>EFI-stub entry repair requires efibootmgr in the recovery host.</source>
        <translation>Ремонт входа EFI-стуба требует efibootmgr в хосте восстановления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1355"/>
        <source>EFI-stub boot requires UEFI firmware; the recovery host booted in legacy BIOS mode.</source>
        <translation>Загрузка EFI-stub требует прошивки UEFI; хост восстановления загружен в устаревшем режиме BIOS.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1356"/>
        <source>extlinux.conf is byte-identical.</source>
        <translation>extlinux.conf является байт-идентичным.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1357"/>
        <source>The detected extlinux configuration %1 is not /boot/extlinux.conf.</source>
        <translation>Обнаруженная конфигурация extlinux %1 не является /boot/extlinux.conf.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1358"/>
        <source>extlinux default label %1 is already selected.</source>
        <translation>extlinux по умолчанию уже выбран %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1359"/>
        <source>Extlinux default selection is only available for the running host.</source>
        <translation>Выбор по умолчанию Extlinux доступен только для запущенного хоста.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1360"/>
        <source>extlinux default label set to %1.</source>
        <translation>По умолчанию extlinux установлен на %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1361"/>
        <source>update-extlinux generated a byte-identical configuration.</source>
        <translation>Update-extlinux генерирует байт-идентичные конфигурации.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1362"/>
        <source>MBR, BIOS boot partition and i386-pc modules are byte-identical.</source>
        <translation>MBR, загрузочный раздел BIOS и модули i386-pc являются байт-идентичными.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1363"/>
        <source>grubenv saved_entry already names the running kernel BLS entry %1.</source>
        <translation>grubenv save entry уже называет запущенное ядро BLS входом %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1364"/>
        <source>grubenv saved_entry set to %1 (all other keys preserved).</source>
        <translation>grubenv save entry установлен на %1 (сохранены все остальные ключи).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1365"/>
        <source>The File Copy made no diagnostic-relevant filesystem change.</source>
        <translation>Копия файла не произвела никаких диагностических изменений файловой системы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1366"/>
        <source>The read-only check reported no errors.</source>
        <translation>Проверка только для чтения не выявила ошибок.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1367"/>
        <source>The simulated fix-broken transaction proposed no package changes.</source>
        <translation>Смоделированная транзакция с фиксированными изменениями не предлагала никаких изменений пакета.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1368"/>
        <source>GRUB detected on legacy BIOS; no EFI boot path is available.</source>
        <translation>GRUB обнаружен на устаревшем BIOS; путь загрузки EFI недоступен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1369"/>
        <source>grub.cfg and grubenv are byte-identical.</source>
        <translation>grub.cfg и grubenv являются байт-идентичными.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1370"/>
        <source>grub.cfg is byte-identical.</source>
        <translation>grub.cfg является байт-идентичным.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1371"/>
        <source>GRUB_DEFAULT is not set to saved; grubenv does not select the default entry.</source>
        <translation>GRUB DEFAULT не настроен на сохранение; grubenv не выбирает вход по умолчанию.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1372"/>
        <source>The conflicting module blacklist is already byte-identical.</source>
        <translation>Противоречивый модуль «черного списка» уже байт-идентичен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1373"/>
        <source>No expected GPU driver could be derived from hardware, package recommendations or the OEM flavour.</source>
        <translation>Ни один ожидаемый драйвер графического процессора не может быть получен из аппаратного обеспечения, рекомендаций по упаковке или вкуса OEM.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1374"/>
        <source>Multiple GPU devices were detected with no explicit choice; the GPU driver swap is refused rather than guessing the primary GPU.</source>
        <translation>Несколько устройств GPU были обнаружены без явного выбора; замена драйвера GPU отклоняется, а не угадывается основной GPU.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1375"/>
        <source>No conflicting module needs to be blacklisted for the chosen driver stack.</source>
        <translation>Никакой конфликтующий модуль не должен быть занесен в черный список для выбранного стека драйверов.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1376"/>
        <source>No older kernel is pinned to the GPU driver; nothing to remove.</source>
        <translation>Ни одно старое ядро не прикреплено к драйверу GPU; ничего не нужно удалять.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1377"/>
        <source>No wrong driver package is installed; nothing to purge.</source>
        <translation>Не установлена неправильная упаковка водителя; нечего чистить.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1378"/>
        <source>No concrete package set for the expected GPU driver could be derived from the detected package backend and installed-package evidence; the driver swap is refused rather than guessing.</source>
        <translation>Никакой конкретный пакет, установленный для ожидаемого драйвера GPU, не может быть получен из обнаруженного бэкэнда пакета и доказательств установленного пакета; обмен драйвером отклоняется, а не угадывается.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1379"/>
        <source>The simulated driver purge proposed no package changes.</source>
        <translation>Моделированная чистка водителя не предлагала никаких изменений пакета.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1380"/>
        <source>The graphics driver stack is consistent: no driver mismatch and no failed or absent DKMS build were found, so the running driver was left untouched.</source>
        <translation>Графический стек драйверов является последовательным: не было обнаружено несоответствия драйверов и не было найдено неисправной или отсутствующей сборки DKMS, поэтому работающий драйвер остался нетронутым.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1381"/>
        <source>BootOrder, labels and registrations already correct.</source>
        <translation>Заказ, этикетки и регистрация уже исправны.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1382"/>
        <source>The running host EFI System Partition could not be resolved.</source>
        <translation>Запуск хоста EFI System Partition не был разрешен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1383"/>
        <source>The target dpkg database or executable is incomplete.</source>
        <translation>Целевая база данных или исполняемый файл dpkg является неполной.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1384"/>
        <source>Rebuilt initramfs images are byte-identical.</source>
        <translation>Восстановленные изображения initramfs являются байт-идентичными.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1385"/>
        <source>Less than 1 GiB of free Btrfs space is available.</source>
        <translation>Доступно менее 1 ГБ свободного пространства Btrfs.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1386"/>
        <source>The grubenv file is missing or not a valid GRUB environment block.</source>
        <translation>В файле grubenv отсутствует или отсутствует действующий блок среды GRUB.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1387"/>
        <source>The simulated kernel-removal transaction proposed no package changes.</source>
        <translation>Симулированная транзакция удаления ядра не предлагала никаких изменений пакета.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1388"/>
        <source>The current kernel is intact; no removable, incomplete or orphaned kernel components were found.</source>
        <translation>Нынешнее ядро не повреждено; не было обнаружено никаких съемных, неполных или осиротевших компонентов ядра.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1389"/>
        <source>legacy BIOS target; no EFI boot path is available.</source>
        <translation>Унаследованная цель BIOS; путь загрузки EFI недоступен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1390"/>
        <source>apk is not installed in the target.</source>
        <translation>apk не устанавливается в цель.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1391"/>
        <source>The target apk installed database is missing.</source>
        <translation>Целевая база данных apk отсутствует.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1392"/>
        <source>The target apk world file is missing.</source>
        <translation>Отсутствует целевой файл apk.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1393"/>
        <source>apt-get is not installed in the target.</source>
        <translation>apt-get не устанавливается в цель.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1394"/>
        <source>btrfs-progs is not installed.</source>
        <translation>btrfs-progs не устанавливается.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1395"/>
        <source>DKMS is not installed in the target.</source>
        <translation>DKMS не устанавливается в цель.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1396"/>
        <source>DKMS is not installed in the Alpine target system.</source>
        <translation>DKMS не устанавливается в альпийской системе.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1397"/>
        <source>DKMS is not installed in the Arch target system.</source>
        <translation>DKMS не устанавливается в системе мишеней Arch.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1398"/>
        <source>dnf5 is not installed in the target system.</source>
        <translation>dnf5 не устанавливается в целевой системе.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1399"/>
        <source>dpkg is not installed in the target.</source>
        <translation>dpkg не устанавливается в цель.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1400"/>
        <source>dracut is not installed in the target system.</source>
        <translation>dracut не устанавливается в целевую систему.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1401"/>
        <source>The dracut generator directory is missing from the target.</source>
        <translation>Каталог генераторов dracut отсутствует.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1402"/>
        <source>efibootmgr is not installed; the running host default EFI entry cannot be changed.</source>
        <translation>efibootmgr не установлен; вход EFI по умолчанию не может быть изменен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1403"/>
        <source>graphical.target is missing from the target.</source>
        <translation>Графический.target отсутствует в цели.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1404"/>
        <source>grub2-editenv is not installed in the target.</source>
        <translation>grub2-editenv не устанавливается в цель.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1405"/>
        <source>grub2-install is not installed in the target.</source>
        <translation>grub2-установка не устанавливается в цель.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1406"/>
        <source>grub2-mkconfig is not installed in the target.</source>
        <translation>grub2-mkconfig не устанавливается в цель.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1407"/>
        <source>%1 is missing.</source>
        <translation>%1 отсутствует.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1408"/>
        <source>Requires GRUB configuration tooling.</source>
        <translation>Требуется инструментарий конфигурации GRUB.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1409"/>
        <source>grub-efi is not installed in the Alpine target.</source>
        <translation>Гриб-эфи не устанавливается в альпийскую цель.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1410"/>
        <source>The x86_64-efi GRUB module directory is missing from the Alpine target.</source>
        <translation>Каталог модулей x86 64-efi GRUB отсутствует в альпийской цели.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1411"/>
        <source>Neither grub-mkconfig nor update-grub is installed in the target system.</source>
        <translation>Ни grub-mkconfig, ни update-grub не установлены в целевой системе.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1412"/>
        <source>grub-install missing.</source>
        <translation>grub-install отсутствует.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1413"/>
        <source>grub-install is not installed in the Alpine target.</source>
        <translation>grub-install не устанавливается в альпийскую цель.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1414"/>
        <source>The grub package is not installed in the Alpine target.</source>
        <translation>Пакет граба не установлен в альпийской цели.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1415"/>
        <source>The GRUB ZFS module (zfs.mod) is not installed in the target.</source>
        <translation>Модуль GRUB ZFS (zfs.mod) не устанавливается в цель.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1416"/>
        <source>update-initramfs/mkinitramfs are not installed in the target.</source>
        <translation>update-initramfs/mkinitramfs не устанавливаются в цель.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1417"/>
        <source>lsinitrd is not installed in the target system; dracut image verification is unavailable.</source>
        <translation>Lsinitrd не установлен в системе-мишени; проверка изображения dracut недоступна.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1418"/>
        <source>mkinitcpio is not installed in the target system.</source>
        <translation>mkinitcpio не устанавливается в целевую систему.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1419"/>
        <source>mkinitfs is not installed in the target system.</source>
        <translation>mkinitfs не устанавливается в целевую систему.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1420"/>
        <source>The target has no mkinitfs configuration.</source>
        <translation>Объект не имеет конфигурации mkinitfs.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1421"/>
        <source>objcopy is unavailable; TUX.EFI embedded sections cannot be verified.</source>
        <translation>TUX.EFI встроенные разделы не могут быть проверены.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1422"/>
        <source>pacman is not installed in the target.</source>
        <translation>pacman не устанавливается в цель.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1423"/>
        <source>The target has no /etc/pacman.conf; refusing a package transaction.</source>
        <translation>Цель не имеет /etc/pacman.conf; отказ от пакетной транзакции.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1424"/>
        <source>The target pacman database directory is missing.</source>
        <translation>Отсутствует целевой каталог базы данных pacman.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1425"/>
        <source>rpm is not installed in the target system.</source>
        <translation>rpm не устанавливается в целевой системе.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1426"/>
        <source>The target RPM database is missing.</source>
        <translation>Целевая база данных RPM отсутствует.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1427"/>
        <source>snapper is not installed.</source>
        <translation>Snapper не устанавливается.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1428"/>
        <source>The syslinux package is not installed according to the detected package manager.</source>
        <translation>Пакет syslinux не устанавливается согласно обнаруженному диспетчеру пакетов.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1429"/>
        <source>TUXEDO UKI builder create_boot_uki_base.sh is not installed in the target.</source>
        <translation>TUXEDO UKI builder create boot uki base.sh не устанавливается в цель.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1430"/>
        <source>update-extlinux is not installed in the target.</source>
        <translation>Update-extlinux не устанавливается в цель.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1431"/>
        <source>The running host has no /etc/update-extlinux.conf.</source>
        <translation>У хоста нет /etc/update-extlinux.conf.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1432"/>
        <source>update-extlinux is not installed in the running host.</source>
        <translation>Update-extlinux не устанавливается в запущенном хосте.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1433"/>
        <source>The zfs-initramfs hook is not installed in the target (no /usr/share/initramfs-tools/hooks/zfs).</source>
        <translation>Крюк zfs-initramfs не устанавливается в цель (нет /usr/share/initramfs-tools/hooks/zfs).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1434"/>
        <source>Multiple extlinux entries reference the running kernel %1 (%2).</source>
        <translation>Несколько записей extlinux ссылаются на работающее ядро %1 (%2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1435"/>
        <source>The running @ root contains nested subvolumes that this release does not migrate: %1.</source>
        <translation>Запущенный корень @ содержит вложенные объемы, которые этот выпуск не мигрирует: %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1436"/>
        <source>The proprietary NVIDIA driver needs network access to fetch the package set; this offline target has no usable APT sources.</source>
        <translation>Проприетарный драйвер NVIDIA нуждается в сетевом доступе для набора пакетов; эта офлайн-цель не имеет полезных источников APT.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1437"/>
        <source>No EFI boot path was detected for the Alpine target (bootloader backend %1).</source>
        <translation>Путь загрузки EFI не был обнаружен для альпийской цели (загрузчик %1).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1438"/>
        <source>No installed Alpine kernels were found under target /boot.</source>
        <translation>Никаких установленных альпийских ядер под прицелом/загрузкой обнаружено не было.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1439"/>
        <source>No /boot (and no resolved EFI System Partition) is visible for the selected scope.</source>
        <translation>Нет/загрузка (и не разрешенный системный раздел EFI) видна для выбранной области.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1440"/>
        <source>No installed versioned kernel packages were found; the newest kernel cannot be identified.</source>
        <translation>Не было найдено ни одной установленной версии пакетов ядра; новейшее ядро не может быть идентифицировано.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1441"/>
        <source>No installed kernel is missing its matching headers.</source>
        <translation>Ни одно установленное ядро не лишено соответствующих заголовков.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1442"/>
        <source>No orphaned kernel build/support packages (linux-kbuild or other versioned helpers whose kernel is gone) were found.</source>
        <translation>Никаких осиротевших пакетов сборки / поддержки ядра (linux-kbuild или другие версии помощников, чье ядро исчезло) не было найдено.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1443"/>
        <source>No orphaned kernel headers (headers without a matching kernel) were found.</source>
        <translation>Никаких сиротских заголовков ядра (заголовки без соответствующего ядра) обнаружено не было.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1444"/>
        <source>No orphaned kernel components (module trees or boot files without an owning package) were found.</source>
        <translation>Никаких осиротевших компонентов ядра (модульные деревья или загрузочные файлы без собственного пакета) обнаружено не было.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1445"/>
        <source>No incomplete kernel packages (an installed image without its on-disk files) were found.</source>
        <translation>Неполных пакетов ядра (установленное изображение без файлов на диске) обнаружено не было.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1446"/>
        <source>No removable kernels are installed (only the running and newest kernels are present).</source>
        <translation>Съемные ядра не устанавливаются (присутствуют только работающие и новейшие ядра).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1447"/>
        <source>No newer repository kernel is available, or the repository is unreachable.</source>
        <translation>Нет более нового ядра репозитория или репозиторий недостижим.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1448"/>
        <source>The target has no configured apk repositories.</source>
        <translation>Цель не имеет настроенных репозиториев apk.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1449"/>
        <source>The target has no configured APT sources to refresh.</source>
        <translation>Цель не имеет настроенных источников APT для обновления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1450"/>
        <source>No EFI System Partition was identified for the selected Arch target.</source>
        <translation>Для выбранной цели EFI System Partition не был идентифицирован.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1451"/>
        <source>No non-rescue BLS entries are installed.</source>
        <translation>Нет неспасательных записей BLS.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1452"/>
        <source>No canonical EFI vendor loader was found on the running host ESP and grub-install is not available.</source>
        <translation>Канонический погрузчик EFI не был найден на запущенном хосте ESP, а grub-install недоступен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1453"/>
        <source>No supported display manager was detected in the target.</source>
        <translation>В цели не было обнаружено поддерживаемого дисплея.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1454"/>
        <source>No supported display manager unit is installed in the target.</source>
        <translation>В цели не устанавливается поддерживаемый блок дисплея.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1455"/>
        <source>The target has no enabled dnf repositories.</source>
        <translation>Объект не имеет репозиториев dnf.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1456"/>
        <source>The target has no enabled dnf repositories to refresh.</source>
        <translation>Цель не имеет репозиториев dnf для обновления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1457"/>
        <source>No installed dracut kernels were found under target /boot.</source>
        <translation>Никаких установленных ядер dracut под прицелом/загрузкой обнаружено не было.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1458"/>
        <source>No EFI-stub initramfs image (initramfs-*) is present at the EFI System Partition root.</source>
        <translation>Изображение EFI-стуба initramfs (initramfs-*) отсутствует в корне системного раздела EFI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1459"/>
        <source>No EFI-stub kernel image (vmlinuz-*) is present at the EFI System Partition root.</source>
        <translation>Изображение ядра EFI-стуба (vmlinuz-*) отсутствует в корне системного раздела EFI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1460"/>
        <source>No EFI System Partition candidate on the selected disk.</source>
        <translation>На выбранном диске нет кандидата на системный раздел EFI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1461"/>
        <source>No EFI System Partition is present or derivable on the selected disk.</source>
        <translation>На выбранном диске отсутствует системный раздел EFI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1462"/>
        <source>No extlinux/syslinux configuration was detected in the target.</source>
        <translation>Конфигурация extlinux/syslinux не была обнаружена.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1463"/>
        <source>No extlinux/syslinux configuration was detected in the running host.</source>
        <translation>Конфигурация extlinux/syslinux не была обнаружена в запущенном хосте.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1464"/>
        <source>The detected GRUB layout is not a grub2 layout.</source>
        <translation>Обнаруженная компоновка GRUB не является компоновкой grub2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1465"/>
        <source>No supported file system check tool is installed in the recovery environment.</source>
        <translation>В среде восстановления не устанавливается поддерживаемый инструмент проверки файловой системы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1466"/>
        <source>No class-03 GPU device and no target journal/module evidence were found; there is no GPU driver stack to repair.</source>
        <translation>Не было найдено GPU-устройства класса 03 и не было найдено доказательств в журнале/модуле; нет стека драйверов GPU для ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1467"/>
        <source>The GPU driver stack looks healthy; no mismatch was proven, so the driver is not touched.</source>
        <translation>Стек драйверов GPU выглядит здоровым; не было доказано несоответствие, поэтому водитель не тронут.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1468"/>
        <source>No supported initramfs backend (mkinitfs, mkinitcpio, dracut or initramfs-tools) was detected.</source>
        <translation>Поддержка initramfs (mkinitfs, mkinitcpio, dracut или initramfs-tools) отсутствует.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1469"/>
        <source>Unable to determine the live root filesystem UUID.</source>
        <translation>Невозможно определить UUID файловой системы Live root.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1470"/>
        <source>No installed kernel module directories were found for mkinitcpio.</source>
        <translation>Для mkinitcpio не было обнаружено установленных каталогов модулей ядра.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1471"/>
        <source>No supported OpenRC display manager service is installed in the target.</source>
        <translation>В цели не устанавливается поддерживаемая служба дисплея OpenRC.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1472"/>
        <source>No guarded package-manager backend that can both answer installed-state and install packages was detected (detected: %1).</source>
        <translation>Не было обнаружено ни одного защищенного бэкэнда менеджера пакетов, который мог бы отвечать как за установленное состояние, так и за установленные пакеты (обнаружено: %1).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1473"/>
        <source>No guarded package-manager backend was detected (detected: %1).</source>
        <translation>Защищенного бэкэнда менеджера пакетов обнаружено не было (обнаружено: %1).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1474"/>
        <source>No detected package manager can verify the syslinux package.</source>
        <translation>Ни один менеджер пакетов не может проверить пакет syslinux.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1475"/>
        <source>No supported reboot mechanism was found on the running host.</source>
        <translation>На бегущем хосте не было обнаружено поддерживаемого механизма перезагрузки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1476"/>
        <source>The selected scope&apos;s root, /boot, ESP and /home filesystems could not be resolved.</source>
        <translation>Корень выбранной области, /boot, ESP и /home файловые системы не могут быть решены.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1477"/>
        <source>No supported service manager (systemd or OpenRC) was detected in the target.</source>
        <translation>Ни один поддерживаемый сервис-менеджер (системный или OpenRC) не был обнаружен в цели.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1478"/>
        <source>No Snapper root configuration manages / with FSTYPE=btrfs.</source>
        <translation>Ни одна конфигурация корня Snapper не управляет / с FSTYPE = btrfs.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1479"/>
        <source>The selected scope has no supported file system type for a read-only check.</source>
        <translation>Выбранная область не имеет поддерживаемого типа файловой системы для проверки только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1480"/>
        <source>The running host root filesystem is %1, not Btrfs.</source>
        <translation>Корневая файловая система хоста - это %1, а не Btrfs.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1481"/>
        <source>The detected bootloader is %1; GRUB is not the selected bootloader.</source>
        <translation>Обнаруженный загрузчик %1; GRUB не является выбранным загрузчиком.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1482"/>
        <source>The running root subvolume is %1, not the top-level @.</source>
        <translation>Бегущий корневой подобъем — %1, а не верхний уровень @.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1483"/>
        <source>Unable to derive a running host UKI candidate.</source>
        <translation>Невозможно получить хост-кандидат UKI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1484"/>
        <source>grubenv is already byte-identical; no nomodeset one-boot entry was written.</source>
        <translation>grubenv уже является байт-идентичным; не было написано ни одной записи с одной загрузкой.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1485"/>
        <source>The nomodeset one-boot fallback requires a GRUB layout with a writable grubenv.</source>
        <translation>Номодесетный запасной вариант с одной загрузкой требует макета GRUB с письменным грабенвом.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1486"/>
        <source>The nomodeset one-boot fallback is refused for UKI/EFI-stub layouts that cannot produce a one-shot artifact.</source>
        <translation>Номодесетный запасной вариант с одной загрузкой отклоняется для макетов UKI / EFI-стубов, которые не могут производить артефакт с одним выстрелом.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1487"/>
        <source>The OpenRC default runlevel already enabled %1.</source>
        <translation>Уровень выполнения OpenRC по умолчанию уже включил %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1488"/>
        <source>A package manager or package-manager lock is active.</source>
        <translation>Блокировка менеджера пакетов или менеджера пакетов активна.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1489"/>
        <source>The pacman transaction reported no packages to install, upgrade or remove.</source>
        <translation>Транзакция pacman не предусматривала установки, обновления или удаления пакетов.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1490"/>
        <source>The running host pins subvolid= in fstab or the kernel command line.</source>
        <translation>Запускаемые хост-пины subvolid= в fstab или командной строке ядра.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1491"/>
        <source>The running host root filesystem is read-only.</source>
        <translation>Запускаемая хост-корневая файловая система только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1492"/>
        <source>The rpm DKMS header correction (kernel-devel/akmods) is not implemented.</source>
        <translation>Коррекция заголовка rpm DKMS (kernel-devel/akmods) не реализована.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1493"/>
        <source>The running kernel %1 has no installed BLS entry.</source>
        <translation>Работающее ядро %1 не имеет установленного входа BLS.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1494"/>
        <source>The running kernel %1 has no entry in %2.</source>
        <translation>Работающее ядро %1 не имеет входа в %2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1495"/>
        <source>The running kernel release could not be determined (uname -r is empty).</source>
        <translation>Высвобождение ядра не может быть определено (имя -r пусто).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1496"/>
        <source>Secure Boot requires a signed loader.</source>
        <translation>Безопасная загрузка требует подписанного погрузчика.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1497"/>
        <source>The running host has a separate /boot filesystem outside the root snapshot.</source>
        <translation>Запущенный хост имеет отдельную файловую систему /boot вне снимка корня.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1498"/>
        <source>The shell command made no diagnostic-relevant filesystem change.</source>
        <translation>Команда оболочки не вносила никаких изменений в файловую систему, имеющих отношение к диагностике.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1499"/>
        <source>Another snapper command is running.</source>
        <translation>Еще одна команда Snapper работает.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1500"/>
        <source>syslinux/extlinux (BIOS) boot detected; no EFI boot path is available.</source>
        <translation>Загрузка syslinux/extlinux (BIOS) обнаружена; путь загрузки EFI недоступен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1501"/>
        <source>A sysvinit display-manager script was detected; offline repair is not implemented for sysvinit.</source>
        <translation>Был обнаружен скрипт дисплея-менеджера сисвинита; для сисвинита не реализован офлайн-ремонт.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1502"/>
        <source>A Timeshift btrfs snapshot inventory is present; Snapper @ rollback is not supported.</source>
        <translation>Присутствует инвентарь моментальных снимков Timeshift btrfs; Snapper @ rollback не поддерживается.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1503"/>
        <source>UEFI variables are not writable; the running host default EFI entry cannot be changed.</source>
        <translation>Переменные UEFI не записываются; вход EFI по умолчанию не может быть изменен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1504"/>
        <source>%1 is missing or empty.</source>
        <translation>%1 отсутствует или пуст.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1505"/>
        <source>Embedded kernel %1 is not installed under /boot.</source>
        <translation>Встроенное ядро %1 не устанавливается под /boot.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1506"/>
        <source>TUX.EFI does not embed a readable .cmdline section.</source>
        <translation>TUX.EFI не встраивает читаемый раздел .cmdline.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1507"/>
        <source>TUX.EFI does not embed a kernel version (.uname).</source>
        <translation>TUX.EFI не встраивает версию ядра (.uname).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1508"/>
        <source>TUX.EFI is not a readable PE/COFF image (objcopy could not read .uname).</source>
        <translation>TUX.EFI не является читабельным изображением PE/COFF (обжкопия не может читать .uname).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1509"/>
        <source>Unable to create a temporary file for UKI section inspection.</source>
        <translation>Невозможно создать временный файл для проверки раздела UKI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1510"/>
        <source>Backend %1 has no guarded implementation for stage %2.</source>
        <translation>Backend %1 не имеет защищенной реализации для этапа %2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1511"/>
        <source>The running kernel release could not be determined.</source>
        <translation>Высвобождение бегущего ядра определить не удалось.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1512"/>
        <source>The running host ZFS root pool is not resolvable (zpool is not installed or the pool is not imported).</source>
        <translation>Бассейн ZFS не разрешим (zpool не установлен или бассейн не импортирован).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1513"/>
        <source>The extlinux label %1 cannot be written to /etc/update-extlinux.conf safely.</source>
        <translation>Маркировка extlinux %1 не может быть безопасно записана на /etc/update-extlinux.conf.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1514"/>
        <source>The simulated upgrade transaction proposed no package changes.</source>
        <translation>Симулированная транзакция обновления не предлагала никаких изменений пакета.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1515"/>
        <source>Validation is read-only.</source>
        <translation>Валидация только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1548"/>
        <source>akmods preflight: headers/build tree present for %1</source>
        <translation>akmods Preflight: заголовки / дерево сборки для %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1549"/>
        <source>PASS: akmods rebuild completed.</source>
        <translation>Реконструкция akmods завершена.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1550"/>
        <source>SIMULATE/PREFLIGHT: akmods module rebuild (headers must already be installed; no package guessing is performed)</source>
        <translation>SIMULATE/PREFLIGHT: восстановление модуля akmods (заголовки должны быть уже установлены; не выполняется угадывание пакета)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1551"/>
        <source>akms preflight: headers/build tree present for %1</source>
        <translation>akms Preflight: заголовки / дерево сборки для %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1552"/>
        <source>PASS: akms rebuild completed.</source>
        <translation>Реконструкция akms завершена.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1553"/>
        <source>SIMULATE/PREFLIGHT: akms module rebuild (headers must already be installed; no package guessing is performed)</source>
        <translation>SIMULATE/PREFLIGHT: восстановление модуля akms (заголовки уже установлены; не выполняется угадывание пакета)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1554"/>
        <source>Backed up Alpine EFI loader files to %1 (firmware fallback present before repair: %2).</source>
        <translation>Резервное копирование файлов загрузчика Alpine EFI в %1 (запас программного обеспечения, присутствующий перед ремонтом: %2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1555"/>
        <source>PASS: refreshed the firmware fallback loader EFI/boot/bootx64.efi from %1.</source>
        <translation>PASS: обновил встроенный резервный погрузчик EFI/boot/bootx64.efi от %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1556"/>
        <source>PASS: Alpine EFI loader files verified under %1/EFI/%2</source>
        <translation>PASS: файлы загрузчика Alpine EFI, проверенные на %1/EFI/%2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1557"/>
        <source>Restored the Alpine EFI loader files from the session backup %1.</source>
        <translation>Восстановлены файлы загрузчика Alpine EFI из резервного копирования сеанса %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1558"/>
        <source>Detected %1 EFI-stub firmware entry(ies) on the selected ESP (%2); reconciling firmware state without file synthesis.</source>
        <translation>Обнаружена запись(и) прошивки %1 на выбранном ESP (%2); примирение состояния прошивки без синтеза файлов.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1559"/>
        <source>PASS: Alpine EFI-stub firmware entries verified; no ESP files were changed.</source>
        <translation>PASS: прошивка Alpine EFI-stub верифицирована; файлы ESP не были изменены.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1560"/>
        <source>Captured complete firmware entry state before Alpine GRUB EFI install: %1</source>
        <translation>Полное состояние ввода прошивки перед установкой Alpine GRUB EFI: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1561"/>
        <source>Writable UEFI efivars/efibootmgr are unavailable; Alpine GRUB EFI repair will update loader files and the fallback copy only (read-only firmware-variable mode).</source>
        <translation>Записываемые UEFI efivars/efibootmgr недоступны; Alpine GRUB EFI repair обновит файлы загрузчика и резервную копию (только для чтения в режиме переменной прошивки).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1562"/>
        <source>SIMULATE/PREFLIGHT: Alpine GRUB EFI install target=%1 fs=%2 id=%3 mode=--no-nvram (helper-managed firmware entries)</source>
        <translation>SIMULATE/PREFLIGHT: Alpine GRUB EFI install target=%1 fs=%2 id=%3 mode=-no-nvram (Helper-Managed firmware entries)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1563"/>
        <source>PASS: Alpine initramfs rebuilt and verified for %1</source>
        <translation>PASS: Alpine initramfs восстановлен и проверен на %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1564"/>
        <source>Alpine missing-file repair: reinstalling %1 package(s) with missing files: %2</source>
        <translation>Восстановление альпийских недостающих файлов: переустановка пакета (пакетов) %1 с отсутствующими файлами: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1565"/>
        <source>Alpine missing-file detection: no missing package files; running the dependency-only apk fix transaction.</source>
        <translation>Обнаружение альпийских отсутствующих файлов: отсутствие отсутствующих файлов пакетов; запуск транзакции apk для исправления зависимостей.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1566"/>
        <source>Target %1 is already mounted at %2 (pre-existing source=%3 options=%4 id=%5); leaving it untouched.</source>
        <translation>Target %1 уже установлен на %2 (существующий источник = параметры %3 = %4 id = %5); оставляя его нетронутым.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1567"/>
        <source>Alpine apk preflight: %1</source>
        <translation>Альпийский apk: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1568"/>
        <source>REFUSED: apk simulation proposes %1 package changes (safety limit: %2).</source>
        <translation>Моделирование apk предлагает изменения пакета %1 (предел безопасности: %2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1569"/>
        <source>REFUSED: apk simulation reported unresolved or conflicting packages.</source>
        <translation>Моделирование apk показало неразрешенные или противоречивые пакеты.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1570"/>
        <source>REFUSED: apk simulation proposes a package downgrade.</source>
        <translation>Моделирование apk предлагает понижение пакета.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1571"/>
        <source>REFUSED: apk simulation reported a locked package database.</source>
        <translation>Моделирование apk сообщило о закрытой базе данных пакетов.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1572"/>
        <source>REFUSED: apk simulation reported incomplete repository metadata.</source>
        <translation>Отказ: моделирование apk сообщило о неполных метаданных хранилища.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1573"/>
        <source>REFUSED: apk simulation would remove &apos;%1&apos; without replacing it in the same transaction.</source>
        <translation>Моделирование apk удалит %1, не заменив его в той же транзакции.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1574"/>
        <source>REFUSED: apk simulation reported an untrusted package signature.</source>
        <translation>apk-симуляция сообщила о ненадежной подписи пакета.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1575"/>
        <source>REFUSED: apk simulation reported an error Boot Bitch does not recognize as safe.</source>
        <translation>apk сообщил об ошибке, которую Boot Bitch не признает безопасной.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1576"/>
        <source>apk simulation replaces package &apos;%1&apos; in the same transaction; removal accepted.</source>
        <translation>Моделирование apk заменяет пакет %1 в той же транзакции.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1577"/>
        <source>KNOWN ISSUE: APT correction is blocked by broken dependencies; simulating --fix-broken before retry.</source>
        <translation>Коррекция APT блокируется сломанными зависимостями; имитирует -фиксируется до повторного использования.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1578"/>
        <source>KNOWN ISSUE: APT correction is blocked by interrupted dpkg configuration; completing dpkg once and re-simulating.</source>
        <translation>Коррекция APT блокируется прерываемой конфигурацией dpkg; завершение dpkg один раз и повторное моделирование.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1579"/>
        <source>full-upgrade simulation did not produce an acceptable transaction; evaluating dist-upgrade.</source>
        <translation>Моделирование полного обновления не привело к приемлемой транзакции; оценка дист-обновления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1580"/>
        <source>Full-upgrade simulation was unavailable or unsafe; using the successful standard upgrade transaction.</source>
        <translation>Моделирование полного обновления было недоступно или небезопасно; использование успешной транзакции стандартного обновления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1581"/>
        <source>apt intent translated: %1</source>
        <translation>Намерение apt переводится как %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1582"/>
        <source>APT package lists changed or a repository index was fetched; reporting the metadata refresh as changed.</source>
        <translation>Изменились списки пакетов APT или был получен индекс хранилища; сообщение об обновлении метаданных по мере изменения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1583"/>
        <source>Target package policy rejected standard upgrade and requested full/dist upgrade; evaluating full-upgrade.</source>
        <translation>Политика целевого пакета отклонила стандартное обновление и запросила полное обновление; оценка полного обновления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1584"/>
        <source>APT simulation proposes %1 non-protected package removal(s): %2</source>
        <translation>Моделирование APT предлагает незащищенное удаление упаковки %1: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1585"/>
        <source>REFUSED: APT simulation proposes removing essential packages.</source>
        <translation>Отказ: моделирование APT предполагает удаление основных пакетов.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1586"/>
        <source>REFUSED: APT simulation would remove protected package &apos;%1&apos;.</source>
        <translation>Моделирование APT удалит защищенный пакет %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1587"/>
        <source>REFUSED: APT simulation would remove %1 packages (safety limit: %2).</source>
        <translation>Моделирование APT удалит пакеты %1 (ограничение безопасности: %2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1588"/>
        <source>Standard upgrade leaves packages pending; evaluating full-upgrade simulation.</source>
        <translation>Стандартное обновление оставляет пакеты в ожидании; оценка моделирования полного обновления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1589"/>
        <source>WARN: accepted release metadata change for %1; metadata refreshed.</source>
        <translation>Предупреждение: принятое изменение метаданных высвобождения для %1; обновленные метаданные.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1590"/>
        <source>WARNING: apt-get update refused a repository release metadata change for: %1</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: обновление apt-get отказало в изменении метаданных репозитория для: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1591"/>
        <source>WARNING: retrying metadata refresh with -o Acquire::AllowReleaseInfoChange=true (release-info changes only; signatures, keys and package verification remain enforced).</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: повторное обновление метаданных с помощью -o Acquire::AllowReleaseInfoChange=true (изменение только информации о выпуске; сигнатуры, ключи и проверка пакета остаются в силе).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1592"/>
        <source>APT upgrade decision: &apos;%1&apos; selected from simulation results.</source>
        <translation>Решение об обновлении APT: %1 выбран из результатов моделирования.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1593"/>
        <source>apt upgrade is disabled by this distribution; running &apos;%1&apos; instead</source>
        <translation>Модернизация apt отключена этим дистрибутивом; вместо этого работает %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1594"/>
        <source>PASS: Arch initramfs images rebuilt and verified.</source>
        <translation>Изображения Arch initramfs восстановлены и проверены.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1595"/>
        <source>AUTO-CORRECT: %1</source>
        <translation>AUTO-CORRECT: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1596"/>
        <source>AUTO-CORRECT: restored temporary stale mapper alias %1 -&gt; %2 for this repair request.</source>
        <translation>AUTO-CORRECT: восстановленный временный несвежий псевдоним %1 -&gt; %2 для этого запроса на ремонт.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1597"/>
        <source>BEGIN: Chroot shell command</source>
        <translation>Оригинальное название: Chroot shell command</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1598"/>
        <source>BEGIN: Running-host shell command</source>
        <translation>Оригинальное название: Running-host shell command</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1599"/>
        <source>BEGIN: %1</source>
        <translation>Исполнитель: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1600"/>
        <source>BEGIN: package backend %1 (%2)</source>
        <translation>BEGIN: пакетный бэкэнд %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1601"/>
        <source>BEGIN: Refresh package metadata</source>
        <translation>BEGIN: Обновить метаданные пакета</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1602"/>
        <source>Arch GRUB layout detected; applying the guarded conventional EFI reinstall after initramfs preflight.</source>
        <translation>Обнаружена компоновка Arch GRUB; применение охраняемой обычной переустановки EFI после предполетного полета initramfs.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1603"/>
        <source>PASS: boot stack reconciliation completed after component simulations and verification.</source>
        <translation>PASS: сверка багажника завершена после моделирования компонентов и проверки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1604"/>
        <source>PASS: boot stack reconciliation completed; EFI/UKI and GRUB were reused from the earlier EFI / UKI stage while mapper/crypttab and initramfs were reconciled.</source>
        <translation>PASS: сверка загрузочного стека завершена; EFI/UKI и GRUB были повторно использованы с более ранней стадии EFI/UKI, в то время как Mapper/crypttab и initramfs были согласованы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1605"/>
        <source>Boot-stack Arch EFI read-only preflight: PASS (%1, %2, id=%3)</source>
        <translation>Арка загрузки EFI только для чтения перед полетом: PASS (%1, %2, id=%3)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1606"/>
        <source>Fedora BIOS GRUB2 layout detected; no EFI/UKI artifacts are part of boot-stack reconciliation.</source>
        <translation>Обнаружен макет Fedora BIOS GRUB2; никакие артефакты EFI/UKI не являются частью сверки загрузочного стека.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1607"/>
        <source>No TUXEDO UKI builder detected; preserving the distribution&apos;s existing EFI layout during boot-stack reconciliation.</source>
        <translation>Ни один конструктор TUXEDO UKI не был обнаружен; сохраняя существующую компоновку дистрибутива EFI во время сверки загрузочного стека.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1608"/>
        <source>NOTE: --post-efi is an EFI-only hint; the Fedora BIOS boot stack reconciles dracut and GRUB2 without an EFI stage.</source>
        <translation>ПРИМЕЧАНИЕ: -post-efi - это подсказка только для EFI; загрузочный стек Fedora BIOS примиряет dracut и GRUB2 без этапа EFI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1609"/>
        <source>SIMULATE/PREFLIGHT: complete boot-stack reconciliation</source>
        <translation>SIMULATE/PREFLIGHT: полное согласование загрузочного стека</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1610"/>
        <source>SKIP: boot-stack EFI/UKI rebuild skipped because the EFI / UKI bootloader stage already rebuilt and verified this layout in the same run (--post-efi).</source>
        <translation>SKIP: загрузочный стек EFI / UKI перестроен, потому что этап загрузчика EFI / UKI уже перестроился и проверил эту компоновку в том же режиме (--post-efi).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1611"/>
        <source>SKIP: boot-stack GRUB regeneration skipped because the EFI / UKI bootloader stage already regenerated the GRUB configuration in the same run (--post-efi).</source>
        <translation>SKIP: регенерация загрузочного стека GRUB пропущена, потому что этап загрузчика EFI / UKI уже регенерировал конфигурацию GRUB в том же режиме (--post-efi).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1612"/>
        <source>Boot-stack TUXEDO UKI read-only preflight: PASS (%1, %2)</source>
        <translation>Загрузочный стек TUXEDO UKI только для чтения: PASS (%1, %2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1613"/>
        <source>Bound recovery-host resolver into target chroot (temporary, read-only)</source>
        <translation>Связанный разрешитель восстановления-хоста в целевой chroot (временный, только для чтения)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1614"/>
        <source>WARNING: btrfs check --repair is a last-resort operation that upstream documents as dangerous and can make a damaged filesystem worse. The caller must have explicit user confirmation and a backup.</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: btrfs check - repair - это операция на последнем месте, которая может сделать поврежденную файловую систему еще хуже. Абонент должен иметь явное подтверждение пользователя и резервную копию.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1615"/>
        <source>Chroot shell exit code: %1</source>
        <translation>Код выхода Chroot shell: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1616"/>
        <location filename="../src/MainWindow.cpp" line="1617"/>
        <source>Command: %1</source>
        <translation>Разработчик: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1618"/>
        <source>Configuration read skipped: %1 has no parent directory in the target (bootloader backend: %2).</source>
        <translation>Конфигурация пропущена: %1 не имеет родительского каталога в цели (бэкэнд загрузчика: %2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1619"/>
        <source>Backed up the ESP loader files to %1 before the GRUB EFI reinstall (firmware fallback present before repair: %2).</source>
        <translation>Резервное копирование файлов загрузчика ESP в %1 перед переустановкой GRUB EFI (резервный запас программного обеспечения перед ремонтом: %2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1620"/>
        <source>PASS: refreshed the firmware fallback loader EFI/BOOT/BOOTX64.EFI (--removable).</source>
        <translation>PASS: обновил встроенный резервный погрузчик EFI/BOOT/BOOTX64.EFI (съемный).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1621"/>
        <source>Restored the ESP loader files from the session backup %1.</source>
        <translation>Восстановил файлы загрузчика ESP из резервного копирования сеанса %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1622"/>
        <source>COPY COMPLETE</source>
        <translation>Полный комплект копий</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1623"/>
        <source>COPY COMPLETE — SHA-256 verification FAILED</source>
        <translation>COPY COMPLETE — проверка SHA-256 не выполнена</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1624"/>
        <source>FAILED verification: %1</source>
        <translation>Неверная проверка: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1625"/>
        <source>%1: %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1626"/>
        <source>rsync metadata/content re-check: PASS</source>
        <translation>Перепроверка метаданных/контента: PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1627"/>
        <source>SHA-256 verification FAILURES: %1 (the copy is not verified complete)</source>
        <translation>Проверка SHA-256: %1 (копия не проверена полностью)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1628"/>
        <source>SHA-256 regular files verified: %1</source>
        <translation>Обычные файлы SHA-256: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1629"/>
        <source>Source items: %1</source>
        <translation>Источник: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1630"/>
        <source>Unrelated destination files: retained (no --delete used)</source>
        <translation>Несвязанные файлы назначения: сохранены (нет - удаление используется)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1631"/>
        <source>Correction simulation exit code: %1 (%2)</source>
        <translation>Код выхода симуляции коррекции: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1632"/>
        <source>WARNING: could not mount target %1 from %2; the read-only diagnostic proceeds without it.</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: не удалось установить цель %1 из %2; диагностика только для чтения протекает без нее.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1633"/>
        <source>Created target destination directory %1 with inherited owner %2:%3</source>
        <translation>Создан каталог целевого назначения %1 с унаследованным владельцем %2:%3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1634"/>
        <source>Mapper/crypttab consistency gate: PASS</source>
        <translation>Затвор согласованности Mapper/crypttab: PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1635"/>
        <source>Mapper/crypttab gate: no target crypttab entries; PASS</source>
        <translation>Mapper/crypttab gate: нет целевых записей crypttab;</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1636"/>
        <source>Mapper/crypttab gate: entry &apos;%1&apos; resolves outside the selected disk: %2</source>
        <translation>Mapper/crypttab gate: запись &quot;%1&quot; разрешает выход за пределы выбранного диска: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1637"/>
        <source>Mapper/crypttab gate: %1 -&gt; %2</source>
        <translation>Mapper/crypttab gate: %1 -&gt; %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1638"/>
        <source>Mapper/crypttab gate: unresolved entry &apos;%1&apos; -&gt; &apos;%2&apos;</source>
        <translation>Mapper/crypttab gate: unresolved entry &apos;%1&apos; -&gt; &apos;%2&apos;</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1639"/>
        <source>Mapper/crypttab gate: unsupported source syntax for &apos;%1&apos;: %2</source>
        <translation>Mapper/crypttab gate: неподдерживаемый синтаксис источника для %1: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1640"/>
        <source>Destination: %1</source>
        <translation>Место назначения: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1641"/>
        <source>Detected target OS: %1</source>
        <translation>Обнаружена целевая ОС: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1642"/>
        <source>WARNING: private /dev filter: the selected target disk identity is unavailable; no block device or mapper will be exposed to the target chroot.</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: частный/дев-фильтр: выбранный идентификатор целевого диска недоступен; ни одно блочное устройство или картограф не будет подвергаться воздействию целевого корня.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1643"/>
        <source>Private /dev filter: %1 allowed block devices/mappers copied; %2 non-essential device-tree entries refused (plain files, sockets, fifos, and devices outside the selected target disk).</source>
        <translation>Частный/дев-фильтр: %1 позволяет скопировать блок-устройства/карты; %2 несущественные записи в дереве устройств отклоняются (простые файлы, розетки, фифос и устройства за пределами выбранного целевого диска).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1644"/>
        <source>DKMS preflight correction: build tree now present for %1</source>
        <translation>Предполетная коррекция DKMS: дерево сборки теперь доступно для %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1645"/>
        <source>SIMULATE/PREFLIGHT: dracut DKMS rebuild (headers must already be installed; no package guessing is performed)</source>
        <translation>SIMULATE/PREFLIGHT: реконструкция DKMS dracut (заголовки уже установлены; не производится угадывание пакета)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1646"/>
        <location filename="../src/MainWindow.cpp" line="1647"/>
        <location filename="../src/MainWindow.cpp" line="1648"/>
        <source>DKMS preflight: headers/build tree present for %1</source>
        <translation>Предполет DKMS: заголовки / дерево для %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1649"/>
        <source>KNOWN ISSUE: DKMS reported missing kernel headers; re-running header preflight/correction once.</source>
        <translation>ЗНАЙТЕ ИССУЭ: DKMS сообщила о пропаже заголовков ядра; повторный запуск заголовка перед полетом / исправление один раз.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1650"/>
        <source>DKMS preflight: missing headers for %1; repository package %2 is available.</source>
        <translation>Предполет DKMS: недостающие заголовки для %1; доступен пакет хранилища %2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1651"/>
        <source>WARNING: DKMS preflight found no build tree for %1 and %2 is unavailable from configured repositories.</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: Предполет DKMS не обнаружил, что дерево сборки для %1 и %2 недоступно из настроенных репозиториев.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1652"/>
        <source>SIMULATE/PREFLIGHT: mkinitcpio DKMS rebuild (headers must already be installed; no package guessing is performed)</source>
        <translation>SIMULATE/PREFLIGHT: реконструкция DKMS mkinitcpio (заголовки уже установлены; не выполняется угадывание пакета)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1653"/>
        <source>SIMULATE/PREFLIGHT: mkinitfs DKMS rebuild (headers must already be installed; no package guessing is performed)</source>
        <translation>SIMULATE/PREFLIGHT: реконструкция DKMS mkinitfs (заголовки уже установлены; не выполняется угадывание пакета)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1654"/>
        <source>PASS: DKMS autoinstall completed.</source>
        <translation>PASS: автоинсталляция DKMS завершена.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1655"/>
        <source>SIMULATE/PREFLIGHT: DKMS module rebuild</source>
        <translation>SIMULATE/PREFLIGHT: восстановление модуля DKMS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1656"/>
        <source>%1 was configured offline only; Boot Bitch intentionally did not start a graphical session.</source>
        <translation>%1 был настроен только в автономном режиме; Boot Bitch намеренно не запускал графический сеанс.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1657"/>
        <source>%1 was configured offline only; Boot Bitch intentionally did not start a graphical session inside the chroot.</source>
        <translation>%1 был настроен только в автономном режиме; Boot Bitch намеренно не запускал графическую сессию внутри chroot.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1658"/>
        <source>PASS: default.target -&gt; %1</source>
        <translation>PASS: default.target -&gt; %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1659"/>
        <source>Detected display manager: %1 (%2)</source>
        <translation>Дисплейный менеджер: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1660"/>
        <source>Enabling %1 in the offline target (it will NOT be started inside the repair chroot).</source>
        <translation>Включение %1 в офлайн-цель (не будет запускаться внутри ремонтного корня).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1661"/>
        <source>WARNING: %1 ExecStart is not executable in the target: %2</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: %1 ExecStart не выполняется в целевом режиме: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1662"/>
        <source>PASS: %1 ExecStart is present for headless preflight: %2</source>
        <translation>%1 ExecStart присутствует для безголового полета: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1663"/>
        <source>PASS: display-manager.service -&gt; %1</source>
        <translation>PASS: display-manager.service -&gt; %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1664"/>
        <source>PASS: %1 command present for headless preflight: %2</source>
        <translation>PASS: %1 Command Present for Headless Preflight: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1665"/>
        <source>Detected Alpine OpenRC display manager: %1 (%2)</source>
        <translation>Дисплейный менеджер Alpine OpenRC: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1666"/>
        <source>PASS: /etc/runlevels/default/%1 -&gt; %2</source>
        <translation>PASS: /etc/runlevels/default/%1 -&gt; %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1667"/>
        <source>WARNING: no explicit command= was found for %1; sh -n and rc-service -e still passed.</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: для %1 не было найдено явной команды =; sh-n и rc-service -e все же прошли.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1668"/>
        <source>%1 will be configured offline only; Boot Bitch will not start a graphical session.</source>
        <translation>%1 будет настроен только в автономном режиме; Boot Bitch не запустит графическую сессию.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1669"/>
        <source>Restoring the OpenRC default-runlevel link for %1 (offline only; the service is NOT started).</source>
        <translation>Восстановление ссылки OpenRC по умолчанию для %1 (только в автономном режиме; служба НЕ запущена).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1670"/>
        <source>SIMULATE/PREFLIGHT: OpenRC graphical login / display manager (offline only)</source>
        <translation>SIMULATE/PREFLIGHT: OpenRC графический логин / дисплеи (только в автономном режиме)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1671"/>
        <source>Display-manager preflight plan: set graphical.target default, enable %1, and repair display-manager.service offline.</source>
        <translation>План предварительного полета дисплея-менеджера: установите графический.target по умолчанию, включите %1 и восстановите дисплей-manager.service в автономном режиме.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1672"/>
        <source>Restoring graphical.target as the target default.</source>
        <translation>Восстановление графического.target в качестве целевого по умолчанию.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1673"/>
        <source>Selected %1 from last-boot display-manager journal evidence.</source>
        <translation>Выбранный %1 из последних данных журнала дисплея-менеджера.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1674"/>
        <source>Selected %1 from last-boot display-manager journal evidence because multiple managers are installed.</source>
        <translation>Выбранный %1 из журнала дисплеев с последней загрузкой, поскольку установлено несколько менеджеров.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1675"/>
        <source>SIMULATE/PREFLIGHT: graphical login / display manager</source>
        <translation>SIMULATE/PREFLIGHT: графический логин/дисплеевый менеджер</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1676"/>
        <source>%1 is a static unit; preserving it through the display-manager.service alias.</source>
        <translation>%1 является статическим блоком, сохраняя его через дисплеи. служебный псевдоним.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1677"/>
        <source>WARNING: systemd-analyze is unavailable; skipped headless display-manager verification.</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: системный анализ недоступен; пропущена проверка безголового дисплея-менеджера.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1678"/>
        <source>WARNING: headless systemd verification reported issues for %1 (the manager will not be started during repair).</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: безголовая система проверки сообщила о проблемах для %1 (менеджер не будет запущен во время ремонта).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1679"/>
        <source>PASS: headless systemd verification for %1</source>
        <translation>PASS: проверка безголовой системы для %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1680"/>
        <source>This transaction downloads approximately %1; the apply step reports its progress only when it finishes.</source>
        <translation>Эта транзакция загружает приблизительно %1; шаг приложения сообщает о своем прогрессе только после завершения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1681"/>
        <source>PASS: dnf5 upgrade removes only the old kernel package(s) (%1) — accepted as in-place kernel replacement.</source>
        <translation>PASS: обновление dnf5 удаляет только старый пакет (пакеты) ядра (%1), принятый в качестве замены ядра.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1682"/>
        <source>dnf5 repository metadata cache rewritten (cache fingerprint changed); reporting the metadata refresh as changed.</source>
        <translation>dnf5 кэш метаданных репозитория переписан (отпечаток кэша изменен); сообщение об обновлении метаданных изменено.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1683"/>
        <source>PASS: dnf5 metadata cache refreshed (repository metadata cache is byte-identical).</source>
        <translation>PASS: обновленный кэш метаданных dnf5 (кэш метаданных в хранилище является байт-идентификационным).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1684"/>
        <source>REFUSED: dnf5 simulation reported an unresolved or conflicting transaction.</source>
        <translation>Моделирование dnf5 сообщило о неразрешенной или конфликтующей транзакции.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1685"/>
        <source>REFUSED: dnf5 simulation proposes a package downgrade.</source>
        <translation>ОТВЕТ: симуляция dnf5 предлагает понижение рейтинга.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1686"/>
        <source>REFUSED: dnf5 simulation failed with exit code %1.</source>
        <translation>Отказ: симуляция dnf5 провалилась с выходным кодом %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1687"/>
        <source>REFUSED: dnf5 simulation reported a locked package database.</source>
        <translation>Dnf5-симуляция сообщила о закрытой базе данных пакетов.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1688"/>
        <source>REFUSED: dnf5 simulation reported incomplete repository metadata.</source>
        <translation>Отказ: моделирование dnf5 сообщило о неполных метаданных хранилища.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1689"/>
        <source>REFUSED: dnf5 upgrade would obsolete critical package &apos;%1&apos;.</source>
        <translation>Обновление dnf5 будет устаревшим критическим пакетом %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1690"/>
        <source>REFUSED: dnf5 reinstall would replace &apos;%1&apos;, which is not one of the reinstalled packages.</source>
        <translation>Переустановка dnf5 заменит %1, который не является одним из переустановленных пакетов.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1691"/>
        <source>REFUSED: dnf5 simulation proposes package removals.</source>
        <translation>Отказ: моделирование dnf5 предлагает удаление пакетов.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1692"/>
        <source>REFUSED: dnf5 simulation would replace %1 packages (safety limit: %2).</source>
        <translation>Dnf5 заменит пакеты %1 (ограничение безопасности: %2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1693"/>
        <source>REFUSED: dnf5 simulation reported an untrusted or missing package signature key.</source>
        <translation>Dnf5-симуляция сообщала о ненадежном или отсутствующем ключе подписи пакета.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1694"/>
        <source>REFUSED: dnf5 simulation proposes %1 package changes (safety limit: %2).</source>
        <translation>В моделировании dnf5 предлагаются изменения пакета %1 (предел безопасности: %2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1695"/>
        <source>REFUSED: dnf5 simulation reported an error Boot Bitch does not recognize as safe.</source>
        <translation>Моделирование dnf5 сообщило об ошибке, которую Boot Bitch не признает безопасной.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1696"/>
        <source>dnf5 preflight version: %1</source>
        <translation>Предполетная версия dnf5: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1697"/>
        <source>SIMULATE/PREFLIGHT: dracut guarded per-kernel rebuild for %1 installed kernel(s) (single build per kernel; verified before any /boot write)</source>
        <translation>SIMULATE/PREFLIGHT: dracut guarded per-kernel rebuild for %1 installed kernel(s) (single build per kernel; verified before any /boot write)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1698"/>
        <source>dracut preflight version: %1</source>
        <translation>Предполетная версия dracut: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1699"/>
        <source>e2fsck preen left uncorrected errors (exit code 4); running the confirmed forced repair pass.</source>
        <translation>e2fsck preen оставил неисправленные ошибки (код выхода 4); запустив подтвержденный принудительный пропуск на ремонт.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1700"/>
        <source>Annotating selected-system EFI entries with OS/model identity: %1 / %2 (PARTUUID %3).</source>
        <translation>Аннотация записей выбранной системы EFI с идентификатором ОС / модели: %1 / %2 (PARTUUID %3).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1701"/>
        <source>EFI bootloader ID: %1</source>
        <translation>ID загрузчика EFI: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1702"/>
        <source>PASS: EFI BootOrder already groups each drive&apos;s firmware destinations by normal use.</source>
        <translation>PASS: EFI BootOrder уже группирует назначения прошивки каждого диска по обычному использованию.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1703"/>
        <source>PASS: EFI BootOrder grouped by drive; all retained firmware entries remain present.</source>
        <translation>PASS: EFI BootOrder сгруппирован по диску; все сохраненные записи прошивки остаются.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1704"/>
        <source>Grouping EFI BootOrder by drive and normal boot use: %1</source>
        <translation>Группировка EFI BootOrder по приводу и нормальному использованию загрузки: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1705"/>
        <source>Clearing BootNext introduced during EFI repair.</source>
        <translation>Очистка BootNext вводится при ремонте EFI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1706"/>
        <source>PASS: Boot%1 is first in BootOrder; all other entries were retained.</source>
        <translation>Boot%1 является первым в BootOrder; все остальные записи были сохранены.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1707"/>
        <source>PASS: selected ESP firmware destinations reconciled; removed Boot%1.</source>
        <translation>PASS: выбранные пункты назначения прошивки ESP согласованы; удален Boot%1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1708"/>
        <source>PASS: selected ESP firmware destinations are unique; no duplicate entries removed.</source>
        <translation>PASS: выбранные пункты назначения прошивки ESP уникальны; дубликаты не удаляются.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1709"/>
        <source>Removed firmware boot entry Boot%1.</source>
        <translation>Удалена встроенная загрузка Boot%1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1710"/>
        <source>PASS: %1 firmware entry restored as Boot%2 on selected system ESP %3.</source>
        <translation>Ввод прошивки %1 восстановлен в виде Boot%2 на выбранной системе ESP %3.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1711"/>
        <source>PASS: generic EFI firmware entry restored as Boot%1 on selected system ESP %2.</source>
        <translation>PASS: общий вход прошивки EFI восстановлен в виде Boot%1 на выбранной системе ESP %2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1712"/>
        <source>EFI grouping did not retain Boot%1 first; re-promoting it once.</source>
        <translation>Группа EFI не сохранила Boot%1 первой; повторно продвигая его один раз.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1713"/>
        <source>EFI entry identity preserved while firmware ID changed: Boot%1 -&gt; Boot%2.</source>
        <translation>Идентификатор входа EFI сохраняется при изменении идентификатора прошивки: Boot%1 -&gt; Boot%2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1714"/>
        <source>iPXE/WebFAI EFI loader is absent on selected system ESP; no recovery firmware entry was created.</source>
        <translation>Загрузчик iPXE/WebFAI EFI отсутствует в выбранной системе ESP; ввод прошивки восстановления не производился.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1715"/>
        <source>iPXE/WebFAI EFI loader is present on the selected system ESP, but firmware variables are not writable; no recovery firmware entry was created.</source>
        <translation>Погрузчик iPXE/WebFAI EFI присутствует в выбранной системе ESP, но переменные прошивки не записываются; запись прошивки восстановления не была создана.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1716"/>
        <source>Repair-target EFI entry label preserved while loader changed: Boot%1 -&gt; Boot%2.</source>
        <translation>Ремонтно-целевой EFI сохранён при смене погрузчика: Boot%1 -&gt; Boot%2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1717"/>
        <source>EFI Boot%1 already names selected model; leaving label &apos;%2&apos; unchanged.</source>
        <translation>EFI Boot%1 уже назвал выбранную модель, оставив маркировку %2 без изменений.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1718"/>
        <source>EFI Boot%1 (%2) label updated to &apos;%3&apos; on selected system ESP only.</source>
        <translation>Маркировка EFI Boot%1 (%2) обновлена до «%3» только на выбранной системе ESP.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1719"/>
        <source>PASS: selected ESP EFI labels retain the drive model after final BootOrder maintenance.</source>
        <translation>PASS: выбранные метки ESP EFI сохраняют модель привода после окончательного обслуживания BootOrder.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1720"/>
        <source>Selected EFI loader is present at %1, but firmware variables are not writable; no generic firmware entry was created.</source>
        <translation>Выбранный погрузчик EFI присутствует в %1, но переменные прошивки не записываются; не было создано никакой общей записи прошивки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1721"/>
        <source>PASS: EFI loader files verified under %1/EFI/%2</source>
        <translation>PASS: файлы загрузчика EFI, проверенные в %1/EFI/%2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1722"/>
        <source>Making Boot%1 the explicit default while preserving all other EFI entries: %2</source>
        <translation>Создание Boot%1 по явному умолчанию при сохранении всех других записей EFI: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1723"/>
        <source>Multiple generic EFI entries already point to the selected system loader (%1); destination maintenance will retain one after the repair.</source>
        <translation>Несколько общих записей EFI уже указывают на выбранный системный погрузчик (%1); техническое обслуживание пункта назначения сохранит один после ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1724"/>
        <source>Multiple iPXE/WebFAI entries already point to the selected system ESP (%1); destination maintenance will retain one after the repair.</source>
        <translation>Несколько записей iPXE/WebFAI уже указывают на выбранную систему ESP (%1); техническое обслуживание пункта назначения сохранит одну после ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1725"/>
        <source>Multiple TUXEDO UKI entries already point to the selected ESP (%1); destination maintenance will retain one after the repair.</source>
        <translation>Несколько записей TUXEDO UKI уже указывают на выбранный ESP (%1); техническое обслуживание пункта назначения сохранит один после ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1726"/>
        <source>Selected EFI vendor directory has no primary loader; no generic firmware entry was created.</source>
        <translation>В выбранном каталоге поставщиков EFI нет основного загрузчика; не было создано ни одной общей записи прошивки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1727"/>
        <source>KNOWN ISSUE: firmware NVRAM registration failed; retrying once as a file-only --no-nvram reinstall.</source>
        <translation>ЗНАЙТЕ ИССУАЛЬНЫЙ: регистрация прошивки NVRAM не удалась; перезагрузка один раз в виде переустановки только файла - no-nvram.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1728"/>
        <source>EFI System Partition: %1 (%2)</source>
        <translation>Системный раздел EFI: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1729"/>
        <source>EFI registration mode: %1</source>
        <translation>Режим регистрации EFI: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1730"/>
        <source>EFI registration mode: Alpine --no-nvram install with helper-managed firmware entries (writable efivars: %1).</source>
        <translation>Режим регистрации EFI: установка Alpine-no-nvram с записями прошивки, управляемыми помощниками (письменные efivars: %1).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1731"/>
        <source>Removing duplicate/legacy selected-ESP firmware destination Boot%1.</source>
        <translation>Удаление дубликата/наследства выбранного-ESP прошивки назначения Boot%1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1732"/>
        <source>Renamed firmware boot entry Boot%1 to &apos;%2&apos;.</source>
        <translation>Переименованная запись загрузки прошивки Boot%1 в «%2».</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1733"/>
        <source>Set EFI BootOrder to %1.</source>
        <translation>Установите EFI BootOrder на %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1734"/>
        <source>EFI BootOrder already matches the requested order %1.</source>
        <translation>EFI BootOrder уже соответствует запрошенному заказу %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1735"/>
        <source>EFI repair read-only preflight: PASS (%1, %2, id=%3)</source>
        <translation>EFI ремонт только для чтения перед полетом: PASS (%1, %2, id=%3)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1736"/>
        <source>EFI repair completed; simulating and regenerating the GRUB fallback configuration.</source>
        <translation>Ремонт EFI завершен; моделирование и регенерация резервной конфигурации GRUB.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1737"/>
        <source>Restoring reconciled BootNext: %1</source>
        <translation>Восстановление согласованного BootNext: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1738"/>
        <source>Restoring reconciled EFI BootOrder (all pre-existing entries retained): %1</source>
        <translation>Восстановление согласованного BootOrder EFI (сохранены все ранее существовавшие записи): %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1739"/>
        <source>Restoring %1 firmware entry &apos;%2&apos; for PARTUUID %3 (EFI path %4).</source>
        <translation>Восстановление ввода прошивки %1 «%2» для PARTUUID %3 (EFI Path %4).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1740"/>
        <source>Restoring missing %1 firmware entry on selected system ESP %2 as &apos;%3&apos; (%4).</source>
        <translation>Восстановление отсутствующей записи прошивки %1 в выбранной системе ESP %2 как «%3» (%4).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1741"/>
        <source>Restoring missing generic EFI firmware entry on selected system ESP %1 as &apos;%2&apos; (%3).</source>
        <translation>Восстановление отсутствующей общей записи прошивки EFI в выбранной системе ESP %1 как «%2» (%3).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1742"/>
        <source>Alpine EFI-stub repair read-only preflight: PASS (%1, %2)</source>
        <translation>Предполетный ремонт альпийской EFI-стубы: PASS (%1, %2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1743"/>
        <source>TUXEDO UKI firmware entry is absent for the selected system ESP; creating only that selected-system entry as &apos;%1&apos;.</source>
        <translation>Запись прошивки TUXEDO UKI отсутствует для выбранной системы ESP; создание только этой записи выбранной системы как «%1».</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1744"/>
        <source>Writable UEFI efivars detected; first attempt will permit firmware registration.</source>
        <translation>Обнаружена запись UEFI efivars; первая попытка позволит зарегистрировать прошивку.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1745"/>
        <source>Writable UEFI efivars are unavailable; using the preflight-selected --no-nvram path.</source>
        <translation>Написанные эфивары UEFI недоступны; с использованием выбранного перед полетом пути -no-nvram.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1746"/>
        <source>ESP mount: auto-mount failed: target=%1 reason=%2 hint=%3</source>
        <translation>ESP mount: Automount failed: target=%1 reason=%2 hint= %3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1747"/>
        <source>ESP mount: auto-mount failed: target=%1 reason=mounted source %2 is not a block device on the selected target disk hint=%3</source>
        <translation>ESP mount: automount failed: target=%1 reason=mounted source %2 is not a block device on the selected target disk hint= %3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1748"/>
        <source>ESP mount: auto-mount failed: target=%1 reason=mounted source %2 is %3, not FAT hint=%4</source>
        <translation>ESP mount: automount failed: target=%1 reason=mounted source %2 is %3, not FAT hint= %4</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1749"/>
        <source>ESP mount cleanup: unmounted leaked ro layer target=%1 source=%2 id=%3</source>
        <translation>Очистка крепления ESP: немонтированная утечка целевого слоя ro = источник %1 = идентификатор %2 = %3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1750"/>
        <source>ESP mount preflight: target=%1 stack=%2 top-source=%3 top-options=%4 top-id=%5 verdict=%6 leaked-ro=%7</source>
        <translation>ESP mount preflight: target=%1 stack=%2 top-source=%3 top-options=%4 top-id=%5 verdict=%6 leaked-ro=%7 top-options</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1751"/>
        <source>ESP mount: mounted target=%1 source=%2 method=%3</source>
        <translation>ESP: установленная цель = источник %1 = метод %2 = %3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1752"/>
        <source>Backed up the extlinux configuration under %1</source>
        <translation>Резервное копирование конфигурации extlinux под %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1753"/>
        <source>PASS: extlinux candidate references %1 boot artifact(s) present in the target</source>
        <translation>PASS: кандидат extlinux ссылается на загрузочный артефакт (артефакты) %1, присутствующий в цели</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1754"/>
        <source>PASS: running host default extlinux entry is LABEL %1 -&gt; LINUX %2 + INITRD %3.</source>
        <translation>PASS: запущенный хост по умолчанию extlinux вход - LABEL %1 -&gt; LINUX %2 + INITRD %3.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1755"/>
        <source>Restored the pre-repair extlinux default configuration</source>
        <translation>Восстановлена предремонтная конфигурация extlinux по умолчанию</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1756"/>
        <source>Extlinux default entry: label=%1 kernel=%2 action=set</source>
        <translation>Запись по умолчанию Extlinux: label=%1 ядро=%2 действие=set</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1757"/>
        <source>Extlinux default entry: label=%1 kernel=%2 action=unchanged</source>
        <translation>Запись по умолчанию Extlinux: label=%1 ядро=%2 действие=без изменений</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1758"/>
        <source>PASS: /boot/extlinux.conf regenerated and verified; the boot sector was not written.</source>
        <translation>PASS: /boot/extlinux.conf восстановлен и проверен; загрузочный сектор не был написан.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1759"/>
        <source>Restored the pre-repair extlinux configuration</source>
        <translation>Восстановлена предремонтная конфигурация extlinux</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1760"/>
        <source>SIMULATE/PREFLIGHT: extlinux configuration regeneration via %1 (overwrite=0 trial; no boot sector write)</source>
        <translation>SIMULATE/PREFLIGHT: регенерация конфигурации extlinux через %1 (перезапись = 0 пробная версия; загрузочный сектор не записывается)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1761"/>
        <source>Fedora default entry: saved_entry=%1 resolves=%2 target=%3 action=set</source>
        <translation>Fedora default entry: save entry=%1 resolves=%2 target=%3 action=set</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1762"/>
        <source>Fedora default entry: saved_entry=%1 resolves=yes target=%2 action=set</source>
        <translation>Fedora default entry: save entry=%1 resolves=yes target=%2 action=set</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1763"/>
        <source>Fedora default entry: saved_entry=%1 resolves=yes target=%2 action=unchanged</source>
        <translation>Fedora default entry: save entry=%1 resolves=yes target=%2 action=unchanged</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1764"/>
        <source>Backed up Fedora GRUB2 configuration artifacts to %1.</source>
        <translation>Резервное копирование артефактов конфигурации Fedora GRUB2 в %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1765"/>
        <source>PASS: Fedora GRUB2 configuration regenerated and verified.</source>
        <translation>Конфигурация Fedora GRUB2 регенерирована и проверена.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1766"/>
        <source>Restored the Fedora GRUB2 configuration artifacts from %1.</source>
        <translation>Восстановлены конфигурационные артефакты Fedora GRUB2 от %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1767"/>
        <source>SIMULATE/PREFLIGHT: generate Fedora GRUB2 configuration with --no-grubenv-update</source>
        <translation>SIMULATE/PREFLIGHT: генерировать конфигурацию Fedora GRUB2 без обновления</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1768"/>
        <source>PASS: Fedora GRUB2 trial configuration generated successfully.</source>
        <translation>Пробная конфигурация Fedora GRUB2 успешно сгенерирована.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1769"/>
        <source>Fedora default entry: grubenv keys other than saved_entry preserved (sha256 %1…).</source>
        <translation>Запись по умолчанию Fedora: сохранены ключи grubenv, отличные от save entry (sha256 %1…).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1770"/>
        <source>PASS: Fedora initramfs for %1 rebuilt byte-identical; temporary build discarded.</source>
        <translation>PASS: Fedora initramfs для %1 восстановленный байт-идентичный; временная сборка выброшена.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1771"/>
        <source>PASS: Fedora initramfs rebuilt and verified for %1</source>
        <translation>Fedora initramfs восстановлена и проверена на %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1772"/>
        <source>File Copy %1: %2</source>
        <translation>Оригинальное название: %1: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1773"/>
        <source>File Copy changed diagnostic-relevant files.</source>
        <translation>Копия файлов изменила диагностические файлы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1774"/>
        <source>File Copy made no diagnostic-relevant filesystem change; cached diagnostics remain valid.</source>
        <translation>Копия файла не внесла никаких изменений в файловую систему; кэшированная диагностика остается в силе.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1775"/>
        <source>Simulation exit code for &apos;--fix-broken install&apos;: %1</source>
        <translation>Код выхода для симуляции &quot;--fix-broken install&quot;: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1776"/>
        <source>Released the helper&apos;s read-only mount(s) of %1 before offline repair.</source>
        <translation>Выпустили только для чтения крепление(ы) помощника %1 перед офлайн-ремонтом.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1777"/>
        <source>REPAIR: %1 (%2) on %3 (fstype=%4, uuid=%5, mount=%6)</source>
        <translation>REPAIR: %1 (%2) на %3 (fstype=%4, uuid=%5, mount=%6)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1778"/>
        <source>FAIL: file system repair (%1) reported unresolved issues for %2 (exit code %3).</source>
        <translation>FAIL: Ремонт файловой системы (%1) сообщил о нерешенных проблемах для %2 (код выхода %3).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1779"/>
        <source>PASS: file system repair (%1) corrected errors on %2 (exit code %3).</source>
        <translation>PASS: исправление ошибок файловой системы (%1) на %2 (код выхода %3).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1780"/>
        <source>PASS: file system repair (%1) completed for %2 with exit code 0.</source>
        <translation>PASS: Ремонт файловой системы (%1) завершен для %2 с выходным кодом 0.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1781"/>
        <source>PASS: file system repair (%1) corrected errors on %2; a reboot is recommended before using the filesystem (exit code 2).</source>
        <translation>PASS: исправление ошибок файловой системы (%1) на %2; перед использованием файловой системы рекомендуется перезагрузка (код выхода 2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1782"/>
        <source>FAIL: file system repair (%1) timed out for %2 (exit code %3).</source>
        <translation>FAIL: Ремонт файловой системы (%1) приурочен к %2 (код выхода %3).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1783"/>
        <source>File system inspection found no resolvable scope filesystems (root, /boot, ESP and /home).</source>
        <translation>Проверка файловой системы не обнаружила разрешимых файловых систем (root, /boot, ESP и /home).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1784"/>
        <source>File system inspection started (read-only; no repair tool is invoked).</source>
        <translation>Началась проверка файловой системы (только чтение; не используется инструмент для ремонта).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1785"/>
        <source>WARNING: fstab %1 device %2 is not on the selected disk %3; excluded from the file system scope.</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: fstab %1 устройство %2 не находится на выбранном диске %3; исключено из области действия файловой системы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1786"/>
        <source>Captured complete firmware entry state before conventional GRUB EFI install: %1</source>
        <translation>Полное состояние ввода прошивки перед обычной установкой GRUB EFI: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1787"/>
        <source>SIMULATE/PREFLIGHT: GRUB EFI install target=%1 fs=%2 id=%3 mode=%4</source>
        <translation>SIMULATE/PREFLIGHT: GRUB EFI install target=%1 fs=%2 id=%3 mode=%4</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1788"/>
        <source>foreign-entry-added: %1</source>
        <translation>Добавлено: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1789"/>
        <source>foreign-entry-removed: %1</source>
        <translation>Иностранный вход: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1790"/>
        <source>menu-entry-added: %1</source>
        <translation>Добавлено меню: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1791"/>
        <source>GRUB menu entries added by the candidate: %1</source>
        <translation>Меню GRUB, добавленное кандидатом: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1792"/>
        <source>Using grub-mkconfig with an isolated output path for preflight.</source>
        <translation>Использование grub-mkconfig с изолированным выходом для предполетного полета.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1793"/>
        <source>PASS: GRUB configuration regenerated and verified.</source>
        <translation>Конфигурация GRUB регенерирована и проверена.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1794"/>
        <source>SIMULATE/PREFLIGHT: generate GRUB configuration to temporary session output</source>
        <translation>SIMULATE/PREFLIGHT: генерация конфигурации GRUB для вывода временного сеанса</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1795"/>
        <source>KNOWN ISSUE: GRUB trial failed on a stale mapper path; retrying once after compatibility alias correction.</source>
        <translation>Испытание GRUB потерпело неудачу на устаревшем пути картографа; повторная попытка один раз после коррекции псевдонима совместимости.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1796"/>
        <source>Trial GRUB generation exit code: %1</source>
        <translation>Пробный выходной код поколения GRUB: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1797"/>
        <source>PASS: GRUB trial configuration generated successfully.</source>
        <translation>PASS: успешно сгенерированная тестовая конфигурация GRUB.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1798"/>
        <source>Backed up the package selection and modprobe.d state to %1.</source>
        <translation>Резервное копирование выбора пакета и состояния modprobe.d в %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1799"/>
        <source>Wrote the blacklist for the conflicting module %1.</source>
        <translation>Написал черный список для конфликтующего модуля %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1800"/>
        <source>GPU driver install plan: %1</source>
        <translation>План установки драйвера GPU: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1801"/>
        <source>Set the one-boot nomodeset next_entry (consumed automatically after one boot).</source>
        <translation>Установите one-boot nomodeset next entry (потребляется автоматически после одной загрузки).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1802"/>
        <source>GPU driver purge plan: %1</source>
        <translation>План очистки драйверов GPU: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1803"/>
        <source>Restored the package selection and modprobe.d state from %1.</source>
        <translation>Восстановлен выбор пакета и состояние modprobe.d от %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1804"/>
        <source>Backed up GRUB2 BIOS boot code (MBR, BIOS boot partition, i386-pc modules) to %1.</source>
        <translation>Резервное копирование кода загрузки GRUB2 BIOS (MBR, раздел загрузки BIOS, модули i386-pc) в %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1805"/>
        <source>NOTE: GRUB2 BIOS boot-code reinstall is only implemented for a mounted repair target; the running host stays config-only.</source>
        <translation>Примечание: Переустановка загрузочного кода GRUB2 BIOS реализована только для установленной цели ремонта; работающий хост остается только конфигурацией.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1806"/>
        <source>PASS: GRUB2 BIOS boot code reinstalled and verified (partition table byte-identical).</source>
        <translation>PASS: загрузочный код GRUB2 BIOS переустанавливается и проверяется (байт-идентичный в таблице разделов).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1807"/>
        <source>GRUB2 boot-code probe: MBR and BIOS boot partition contain a GRUB signature; config-only regeneration.</source>
        <translation>Зонд загрузочного кода GRUB2: раздел загрузки MBR и BIOS содержит сигнатуру GRUB; регенерация только конфигурацией.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1808"/>
        <source>REPAIR: reinstall GRUB2 BIOS boot code (%1 --target=i386-pc --boot-directory=/boot --recheck %2)</source>
        <translation>REPAIR: переустановить загрузочный код GRUB2 BIOS (%1 --target=i386-pc --boot-directory=/boot --recheck %2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1809"/>
        <source>Restored the GRUB2 BIOS boot code from %1.</source>
        <translation>Восстановлен загрузочный код GRUB2 BIOS от %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1810"/>
        <source>GRUB2 config-only reconciliation requested; the BIOS boot-code reinstall substage is skipped.</source>
        <translation>Запрашивается только выверка конфигурации GRUB2; подстанция переустановки загрузочного кода BIOS пропускается.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1811"/>
        <source>UEFI firmware detected; the GRUB2 BIOS boot-code reinstall substage is not applicable.</source>
        <translation>Обнаружена прошивка UEFI; подстанция переустановки загрузочного кода GRUB2 BIOS не применима.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1812"/>
        <source>Host boot-stack Arch EFI read-only preflight: PASS (%1, %2, id=%3)</source>
        <translation>Арка EFI только для чтения: PASS (%1, %2, id=%3)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1813"/>
        <source>Host boot-stack TUXEDO UKI read-only preflight: PASS (%1, %2)</source>
        <translation>Загрузочный стек TUXEDO UKI только для чтения: PASS (%1, %2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1814"/>
        <source>Host command guard: firmware variables are read-only to package/kernel/vendor hooks; the helper owns explicit firmware registration.</source>
        <translation>Охрана команды хоста: переменные прошивки считываются только для крючков пакета / ядра / поставщика; помощник владеет явной регистрацией прошивки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1815"/>
        <source>Creating host %1 firmware entry on %2 as &apos;%3&apos; (%4); existing host, repair, and foreign entries are untouched.</source>
        <translation>Создание записи прошивки хоста %1 на %2 как «%3» (%4); существующие записи хоста, ремонта и посторонние записи не затронуты.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1816"/>
        <source>PASS: running host default EFI entry is Boot%1 on %2.</source>
        <translation>PASS: Запуск хоста по умолчанию EFI является Boot%1 на %2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1817"/>
        <source>WARNING: host destination is group-writable; the copied files may be modified by the owning group: %1</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: назначение хоста является групповым; скопированные файлы могут быть изменены владеющей группой: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1818"/>
        <source>Host disk: %1</source>
        <translation>Диск хоста: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1819"/>
        <source>Backed up running-host EFI loader files to %1 before the guarded reinstall.</source>
        <translation>Резервное копирование файлов загрузчика EFI в %1 перед защищенной переустановкой.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1820"/>
        <source>Canonical running-host EFI loader verified: %1 (%2) on %3.</source>
        <translation>Канонический загрузчик EFI проверен: %1 (%2) на %3.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1821"/>
        <source>Host EFI read-only preflight: PASS (%1, %2, id=%3)</source>
        <translation>Ведущий EFI только для чтения: PASS (%1, %2, id=%3)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1822"/>
        <source>EFI repair completed; regenerating the running host GRUB fallback configuration.</source>
        <translation>Завершен ремонт EFI; регенерация конфигурации резервного хоста GRUB.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1823"/>
        <source>Restored the running-host EFI loader files from %1.</source>
        <translation>Восстановил файлы загрузчика EFI из %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1824"/>
        <source>Host Alpine EFI-stub read-only preflight: PASS (%1, %2)</source>
        <translation>Ведущий Alpine EFI-stub только для чтения: PASS (%1, %2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1825"/>
        <source>Writable UEFI variables are unavailable; host maintenance will preserve files and BootOrder without firmware registration.</source>
        <translation>Записываемые переменные UEFI недоступны; обслуживание хоста будет сохранять файлы и BootOrder без регистрации прошивки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1826"/>
        <source>Host %1 entries %2 point to %3 on %4; keeping active entry Boot%5 and pruning the remaining duplicates.</source>
        <translation>Ведущие %1 записи %2 указывают на %3 на %4; сохраняя активный вход Boot%5 и обрезая оставшиеся дубликаты.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1827"/>
        <source>Host EFI System Partition: %1 (%2) %3</source>
        <translation>Системный раздел EFI: %1 (%2) %3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1828"/>
        <source>Native running-host repair preflight: PASS</source>
        <translation>Предполетный ремонт хозяина: PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1829"/>
        <source>All requested running-host repair stages completed successfully.</source>
        <translation>Все запрошенные этапы ремонта бегунов успешно завершены.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1830"/>
        <source>ROLLBACK: restored BootOrder=%1, but the firmware state differs from the pre-change capture:</source>
        <translation>ROLLBACK: восстановленный BootOrder=%1, но состояние прошивки отличается от захвата перед изменением:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1831"/>
        <source>CRITICAL: cannot detach the promoted host rollback mounts; refusing to rename Btrfs roots during recovery.</source>
        <translation>КРИТИЧЕСКИЙ: не может отсоединить продвигаемые хосты откатных креплений; отказываясь переименовать корни Btrfs во время восстановления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1832"/>
        <location filename="../src/MainWindow.cpp" line="1953"/>
        <source>CRITICAL: could not determine the preserved root subvolume ID.</source>
        <translation>КРИТИЧЕСКИЙ: не смог определить сохраненный идентификатор подобъема корня.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1833"/>
        <source>CRITICAL: could not move the failed host rollback candidate out of @; refusing to promote the preserved root.</source>
        <translation>КРИТИЧЕСКИЙ: не смог переместить несостоявшегося хоста-кандидата на откат из @; отказавшись продвигать сохранившийся корень.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1834"/>
        <source>CRITICAL: could not move the nested @/.snapshots child back into the preserved root.</source>
        <translation>КРИТИЧЕСКИЙ: не смог переместить вложенного @/.snapshots ребенка обратно в сохранившийся корень.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1835"/>
        <source>CRITICAL: could not remount the Btrfs top-level filesystem read-write during host rollback recovery.</source>
        <translation>CRITICAL: не удалось перемонтировать файловую систему верхнего уровня Btrfs во время восстановления отката хоста.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1836"/>
        <location filename="../src/MainWindow.cpp" line="1956"/>
        <source>CRITICAL: could not restore the previous Btrfs default subvolume.</source>
        <translation>CRITICAL: не удалось восстановить предыдущий объем Btrfs по умолчанию.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1837"/>
        <source>RUNNING-HOST SNAPSHOT ROLLBACK COMPLETE</source>
        <translation>Бегущий-хозяин SNAPSHOT ROLLBACK COMPLETE</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1838"/>
        <source>ROLLBACK: created entry Boot%1 is active (BootCurrent/BootNext); it is retained but no longer promoted.</source>
        <translation>Boot%1 активен (BootCurrent/BootNext); он сохраняется, но больше не продвигается.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1839"/>
        <location filename="../src/MainWindow.cpp" line="1957"/>
        <source>Creating writable rollback candidate %1</source>
        <translation>Создание письменного кандидата на откат %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1840"/>
        <source>Host rollback default-subvolume update failed; automatically restoring the preserved root.</source>
        <translation>Обновление хоста по умолчанию не удалось; автоматически восстанавливается сохраненный корень.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1841"/>
        <source>Pre-rollback boot artifact fingerprints: fstab=%1 grub=%2 cmdline=%3 uki=%4</source>
        <translation>Отпечатки пальцев артефакта перед загрузкой: fstab=%1 grub=%2 cmdline=%3 uki=%4</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1842"/>
        <source>Migrating nested @/.snapshots child subvolume into the promoted root.</source>
        <translation>Миграция вложенного @/.snapshots ребенка в продвигаемый корень.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1843"/>
        <source>Host rollback candidate mount/preflight failed; automatically restoring the preserved root.</source>
        <translation>Кандидат на откат хоста mount/preflight потерпел неудачу; автоматически восстанавливается сохраненный корень.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1844"/>
        <source>PASS: the original @ and its boot stack were restored automatically. Failed rollback candidate retained as %1.</source>
        <translation>PASS: оригинальный @ и его багажник были восстановлены автоматически. Неудавшийся кандидат на откат сохранился как %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1845"/>
        <source>CRITICAL: the original @ was restored, but its boot-stack reconciliation also failed. Manual boot repair is required before reboot.</source>
        <translation>КРИТИЧЕСКИЙ: оригинал @ был восстановлен, но его примирение с загрузочным стеком также провалилось. Перед перезагрузкой требуется ручной ремонт багажника.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1846"/>
        <location filename="../src/MainWindow.cpp" line="1978"/>
        <source>CRITICAL: preserved root %1 is missing; refusing to move the active rollback candidate.</source>
        <translation>Сохраненный корень %1 отсутствует; отказ от перемещения активного откатного кандидата.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1847"/>
        <location filename="../src/MainWindow.cpp" line="2015"/>
        <source>Previous root retained as: %1</source>
        <translation>Корень сохранился как: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1848"/>
        <source>Promoted root: @ (subvolume ID %1); the running system keeps the previous root until reboot.</source>
        <translation>Продвигаемый корень: @ (подобный ID %1); запущенная система сохраняет предыдущий корень до перезагрузки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1849"/>
        <source>HOST ROLLBACK RECOVERY: restoring the preserved pre-rollback @.</source>
        <translation>HOST ROLLBACK RECOVERY: Восстановление сохраненного предварительного отката @.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1850"/>
        <source>ROLLBACK: restored BootOrder=%1 and removed created entry Boot%2; firmware state equals the pre-change capture.</source>
        <translation>ROLLBACK: восстановленный BootOrder=%1 и удаленный созданный вход Boot%2; состояние прошивки равно захвату перед изменением.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1851"/>
        <source>Running-host rollback selected: snapshot %1.</source>
        <translation>Выбранный откат бегущего хоста: снимок %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1852"/>
        <source>Rollback source remains unchanged; the running host keeps the current root until reboot.</source>
        <translation>Источник Rollback остается неизменным; работающий хост сохраняет текущий корень до перезагрузки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1853"/>
        <source>Selected rollback target: %1</source>
        <translation>Выбранная цель отката: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1854"/>
        <source>Host rollback candidate failed post-switch validation/boot reconciliation; automatically restoring the preserved root.</source>
        <translation>Кандидат на откат хоста не прошел проверку после переключения / сверку загрузки; автоматически восстанавливается сохраненный корень.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1855"/>
        <source>Host root component: %1 (%2)</source>
        <translation>Корневой компонент хоста: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1856"/>
        <source>Host root mount: / (subvolume=%1)</source>
        <translation>Установка корня хоста: / (подобный = %1)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1857"/>
        <source>FAIL: Running-host shell command was cancelled at an interactive prompt.</source>
        <translation>Неудача: команда оболочки бегущего хозяина была отменена в интерактивном режиме.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1858"/>
        <source>Running-host shell exit code: %1</source>
        <translation>Код выхода оболочки Running-host: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1859"/>
        <source>FAIL: Running-host shell command (exit code %1)</source>
        <translation>FAIL: команда оболочки Running-host (код выхода %1)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1860"/>
        <source>PASS: Running-host shell command</source>
        <translation>PASS: команда Running-host shell</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1861"/>
        <source>Host snapshot inventory skipped: %1 is not Btrfs.</source>
        <translation>Пропущенный инвентарь снимков хоста: %1 не является Btrfs.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1862"/>
        <source>Host TUXEDO UKI firmware entry already present: Boot%1 on %2.</source>
        <translation>Ввод прошивки TUXEDO UKI уже присутствует: Boot%1 на %2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1863"/>
        <source>Host TUXEDO UKI file is present on %1, but firmware variables are not writable; host registration was not changed.</source>
        <translation>Файл Host TUXEDO UKI присутствует на %1, но переменные прошивки не записываются; регистрация хоста не была изменена.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1864"/>
        <source>Host TUXEDO UKI read-only preflight: PASS (%1, %2)</source>
        <translation>Ведущий TUXEDO UKI только для чтения: PASS (%1, %2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1865"/>
        <source>PASS: host TUXEDO UKI firmware entry restored as Boot%1 on %2.</source>
        <translation>PASS: вход прошивки TUXEDO UKI восстановлен в виде Boot%1 на %2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1866"/>
        <source>Restoring missing host TUXEDO UKI firmware entry on %1 as &apos;%2&apos;; existing host, repair, and foreign entries are untouched.</source>
        <translation>Восстановление отсутствующей записи прошивки TUXEDO UKI на %1 как «%2»; существующие записи хоста, ремонта и посторонние записи нетронуты.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1867"/>
        <source>Running-host validation complete; no host files were changed.</source>
        <translation>Проверка запуска-хоста завершена; никакие файлы хоста не были изменены.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1868"/>
        <source>Running-host validation summary</source>
        <translation>Резюме проверки запускающего хоста</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1869"/>
        <source>KNOWN ISSUE: %1 is missing; creating it instead of attempting an update.</source>
        <translation>%1 отсутствует; создание его вместо попытки обновления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1870"/>
        <source>PASS: initramfs %1 verified by mkinitfs -l build-input listing</source>
        <translation>PASS: initramfs %1 подтвержден списком встроенных входов mkinitfs-l</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1871"/>
        <source>SIMULATE/PREFLIGHT: initramfs generation for %1 installed kernel(s)</source>
        <translation>SIMULATE/PREFLIGHT: генерация initramfs для установленного ядра %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1872"/>
        <source>PASS: trial initramfs build for %1</source>
        <translation>PASS: пробная сборка initramfs для %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1873"/>
        <source>PASS: initramfs verified for %1</source>
        <translation>PASS: initramfs подтвержден для %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1874"/>
        <source>PASS: initramfs %1 verified by zcat/cpio listing</source>
        <translation>PASS: initramfs %1 подтвержден списком zcat/cpio</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1875"/>
        <source>KNOWN ISSUE: initrd.img-%1 is missing; creating it instead of attempting an update.</source>
        <translation>Initrd.img-%1 отсутствует; его создание вместо попытки обновления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1876"/>
        <source>Kernel cleanup plan: removing %1 versioned kernel package(s): %2</source>
        <translation>План очистки ядра: удаление пакета (пакетов) ядра версии %1: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1877"/>
        <source>Installing the missing headers for the newest kernel: %1</source>
        <translation>Установка недостающих заголовков для новейшего ядра: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1878"/>
        <source>Installing the repository-latest kernel image: %1</source>
        <translation>Установка последнего изображения ядра репозитория: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1879"/>
        <source>Orphaned kernel cleanup plan: removing the leftover on-disk components of kernel %1.</source>
        <translation>План очищения ядра: удаление оставшихся на диске компонентов ядра %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1880"/>
        <source>FAIL: a protected kernel package was removed by the transaction; restoring protected package(s): %1</source>
        <translation>Неисправность: защищенный пакет ядра был удален транзакцией; восстановление защищенного пакета (ов): %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1881"/>
        <source>Kernel reinstall plan: reinstalling %1 incomplete kernel package(s): %2</source>
        <translation>План переустановки ядра: переустановка неполного пакета (пакетов) ядра %1: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1882"/>
        <source>FAIL: kernel removal exited %1; cleaning leftover on-disk components of the removed kernels</source>
        <translation>Неисправность: удаление ядра вышло из %1; очистка оставшихся на диске компонентов удаленных ядер</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1883"/>
        <source>Cleaning leftover on-disk components of removed kernel %1 (zombie cleanup)</source>
        <translation>Очистка оставшихся на диске компонентов удаленного ядра %1 (очистка зомби)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1884"/>
        <source>%1 lives on the root filesystem; no separate mount required</source>
        <translation>%1 работает в корневой файловой системе; отдельного крепления не требуется.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1885"/>
        <source>PASS: lsinitcpio verified %1 Arch initramfs image(s)</source>
        <translation>PASS: lsinitcpio verified %1 Arch initramfs image(s)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1886"/>
        <source>PASS: lsinitramfs verified /boot/initrd.img-%1</source>
        <translation>PASS: lsinitramfs verified /boot/initrd.img-%1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1887"/>
        <source>PASS: lsinitrd verified %1</source>
        <translation>PASS: верифицированный %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1888"/>
        <source>LUKS target is already unlocked by existing mapper: %1</source>
        <translation>Цель LUKS уже разблокирована существующим картографом: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1889"/>
        <source>Mapper name: %1</source>
        <translation>Оригинальное название: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1890"/>
        <source>LUKS unlock complete. The mapper remains open for this recovery session.</source>
        <translation>Разблокировка LUKS завершена. Картограф остается открытым для этой сессии восстановления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1891"/>
        <source>Unlocking LUKS target %1</source>
        <translation>Разблокировка цели LUKS %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1892"/>
        <source>Mandatory safety preflight: PASS</source>
        <translation>Обязательный предполет безопасности: PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1893"/>
        <source>Mapper compatibility: %1 already resolves to %2</source>
        <translation>Совместимость с картером: %1 уже разрешен для %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1894"/>
        <source>Mapper compatibility: temporary %1 -&gt; %2</source>
        <translation>Совместимость с маппером: временный %1 -&gt; %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1895"/>
        <source>SIMULATE/PREFLIGHT: mkinitcpio trial builds for %1 installed kernel(s)</source>
        <translation>SIMULATE/PREFLIGHT: тестовые сборки mkinitcpio для установленного ядра %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1896"/>
        <source>PASS: trial mkinitcpio build for %1</source>
        <translation>PASS: пробная сборка mkinitcpio для %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1897"/>
        <source>SIMULATE/PREFLIGHT: mkinitfs trial builds for %1 installed kernel(s)</source>
        <translation>SIMULATE/PREFLIGHT: тестовые сборки mkinitfs для установленного ядра %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1898"/>
        <source>PASS: trial mkinitfs build for %1</source>
        <translation>PASS: пробная сборка mkinitfs для %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1899"/>
        <source>Mounted target ESP discovered by GPT type: %1 at /boot/efi (%2)</source>
        <translation>Установленная цель ESP, обнаруженная по типу GPT: %1 в /boot/efi (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1900"/>
        <source>Mounted target %1 from %2 (%3)</source>
        <translation>Установленная цель %1 от %2 (%3)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1901"/>
        <source>Mounting target %1 from %2 (%3)</source>
        <translation>Установка цели %1 из %2 (%3)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1902"/>
        <source>Mounting target Btrfs fstab subvolumes (%1)</source>
        <translation>Установка целевых субобъемов Btrfs fstab (%1)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1903"/>
        <source>Mounting target %1 from %2</source>
        <translation>Установка цели %1 из %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1904"/>
        <source>No TUXEDO UKI builder detected; retaining the distribution&apos;s existing EFI layout.</source>
        <translation>Строитель TUXEDO UKI не обнаружен, сохранив существующую компоновку дистрибутива EFI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1905"/>
        <source>FAIL: ntfsfix could not fully repair %1 (exit code %2); run Windows chkdsk /f for a real NTFS repair.</source>
        <translation>FAIL: ntfsfix не смог полностью восстановить %1 (код выхода %2); запустить Windows chkdsk /f для реального ремонта NTFS.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1906"/>
        <source>NOTE: ntfsfix only clears the NTFS dirty state; Windows chkdsk /f is required for a real NTFS repair.</source>
        <translation>Примечание: ntfsfix очищает только грязное состояние NTFS; Windows chkdsk /f требуется для реального ремонта NTFS.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1907"/>
        <source>Orphaned kernel build/support cleanup plan: removing %1 package(s): %2</source>
        <translation>План сборки/очистки опорного ядра: удаление пакета (пакетов) %1: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1908"/>
        <source>FAIL: orphaned kernel build/support removal exited %1</source>
        <translation>FAIL: удаление осиротевшего ядра вышло из %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1909"/>
        <source>Orphaned kernel headers cleanup plan: removing %1 header package(s): %2</source>
        <translation>План очистки заголовков ядра: удаление пакета (пакетов) заголовков %1: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1910"/>
        <source>FAIL: orphaned kernel headers removal exited %1; cleaning leftover header directories</source>
        <translation>FAIL: удаление заголовков сиротского ядра вышло из %1; очистка оставшихся каталогов заголовков</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1911"/>
        <source>Ownership validation: %1:%2 maps to %3:%4 on both sides; preserving numeric ownership</source>
        <translation>Проверка прав собственности: карты %1:%2 для %3:%4 с обеих сторон; сохранение числового владения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1912"/>
        <source>Ownership validation: source %1:%2 (%3:%4) does not map identically on the destination side; using destination owner %5:%6</source>
        <translation>Проверка прав собственности: источник %1:%2 (%3:%4) не отображается одинаково на стороне назначения; используя владельца назначения %5:%6</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1913"/>
        <source>Ownership policy: %1</source>
        <translation>Политика владения: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1914"/>
        <source>Package backend %1 did not report a change status; treating the stage as changed.</source>
        <translation>Пакетный бэкэнд %1 не сообщал об изменении статуса; рассматривая стадию как изменённую.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1915"/>
        <source>SKIP: package backend %1 is not runnable for stage &apos;%2&apos;: %3</source>
        <translation>SKIP: пакетный бэкэнд %1 не работает для этапа %2: %3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1916"/>
        <source>Package backend %1 change status: %2</source>
        <translation>Пакет обновления %1 Change Status: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1917"/>
        <source>Package stage &apos;%1&apos;: running %2 runnable backend(s): %3</source>
        <translation>Этап пакета «%1»: запускается %2 управляемый бэкэнд(ы): %3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1918"/>
        <source>PackageKit daemon is running; it is only a conflict while it holds package-manager locks or spawns apt/dpkg.</source>
        <translation>Демон PackageKit работает; это только конфликт, в то время как он держит блокировщик пакетов или порождает apt / dpkg.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1919"/>
        <source>Arch pacman transaction completed successfully despite mirror fallback.</source>
        <translation>Транзакция Arch pacman успешно завершена, несмотря на зеркальный откат.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1920"/>
        <source>WARN: pacman encountered %1 recoverable mirror retrieval failure(s).</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: pacman столкнулся с неисправностью (неисправностями) восстановления зеркала %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1921"/>
        <source>PASS: Arch pacman transaction preflight resolved without removals.</source>
        <translation>PASS: предполетная транзакция Arch pacman разрешена без удаления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1922"/>
        <source>REFUSED: pacman preflight proposes %1 packages (safety limit: 1000).</source>
        <translation>Предполетный рейс pacman предлагает пакеты %1 (ограничение безопасности: 1000).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1923"/>
        <source>REFUSED: pacman preflight reported repository, download or transaction integrity errors.</source>
        <translation>Предполетный отчет pacman об ошибках репозитория, загрузки или целостности транзакции.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1924"/>
        <source>REFUSED: pacman preflight proposes removals or unresolved dependencies.</source>
        <translation>Предполетный рейс pacman предлагает удаление или неразрешенные зависимости.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1925"/>
        <source>Arch pacman transaction sandbox prepared under %1; the target package database will not be used for preflight.</source>
        <translation>Arch pacman — песочница транзакций, подготовленная под %1; база данных целевого пакета не будет использоваться для предварительного полета.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1926"/>
        <source>SIMULATE/PREFLIGHT: full Arch pacman transaction (sandboxed database/cache; no target packages will be changed)</source>
        <translation>SIMULATE/PREFLIGHT: полная транзакция Arch pacman (база данных/кэш в песочнице; целевые пакеты не будут изменены)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1927"/>
        <source>PASS: dnf5 metadata cache refreshed.</source>
        <translation>PASS: кэш метаданных dnf5 обновлен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1928"/>
        <source>PASS: %1</source>
        <translation>PASS: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1929"/>
        <source>PASS: %1 completed through one full pacman transaction.</source>
        <translation>%1 завершается через одну полную транзакцию pacman.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1930"/>
        <source>PASS: Refresh package metadata</source>
        <translation>PASS: Обновить метаданные пакета</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1931"/>
        <source>PASS: Refresh package metadata (package lists byte-identical; no repository index was fetched)</source>
        <translation>PASS: Обновить метаданные пакета (списки пакетов байт-идентичные; индекс хранилища не был получен)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1932"/>
        <source>Host package-manager concurrency gate: PASS</source>
        <translation>Host package-manager concurrency gate: PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1933"/>
        <source>Post-upgrade dpkg audit:</source>
        <translation>После обновления dpkg:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1934"/>
        <source>PREVIEW COMPLETE — no files were changed.</source>
        <translation>PREVIEW COMPLETE — файлы не были изменены.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1935"/>
        <source>PREVIEW: target destination directory would be created: %1</source>
        <translation>PREVIEW: будет создан каталог целевого назначения: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1936"/>
        <source>Protected host check: PASS</source>
        <translation>Защищенная проверка хоста: PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1937"/>
        <source>Scheduling running-host reboot through the system reboot command.</source>
        <translation>Планирование перезагрузки хоста через команду перезагрузки системы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1938"/>
        <source>Scheduling running-host reboot through OpenRC.</source>
        <translation>Планирование перезагрузки бегущего хоста через OpenRC.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1939"/>
        <source>Scheduling running-host reboot through systemd.</source>
        <translation>Планирование перезагрузки бегущего хоста через систему.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1940"/>
        <source>WARNING: refusing an unsafe target Btrfs mountpoint (not absolute): %1</source>
        <translation>Предупреждение: отказ от небезопасной цели Btrfs (не абсолютный): %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1941"/>
        <source>WARNING: refusing an unsafe target Btrfs subvolume mount path: %1</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: отказ от небезопасной траектории установки подкачки Btrfs: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1942"/>
        <source>WARNING: refusing an unsafe target %1 mount path: %2</source>
        <translation>Предупреждение: отказ от небезопасной траектории установки %1: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1943"/>
        <source>Remounting confirmed target filesystem read-write for file copy</source>
        <translation>Восстановление подтвержденной целевой файловой системы read-write для копирования файла</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1944"/>
        <source>Remounting confirmed target root read-write</source>
        <translation>Восстановление подтвержденного целевого корня read-write</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1945"/>
        <source>Remounting target data filesystem %1 read-write</source>
        <translation>Восстановление файловой системы целевых данных %1 read-write</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1946"/>
        <source>Remounting target %1 read-write</source>
        <translation>Восстановление целевой %1 read-write</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1947"/>
        <source>All requested repair stages completed successfully.</source>
        <translation>Все требуемые этапы ремонта успешно завершены.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1948"/>
        <source>Rollback boot-stack reconciliation: using simulation-first adaptive component workflows.</source>
        <translation>Выверка загрузочного стека в обратном направлении: использование рабочих процессов адаптивных компонентов первого моделирования.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1949"/>
        <source>Rollback candidate default-subvolume update failed; automatically restoring the preserved root.</source>
        <translation>Обновление Rollback-кандидата по умолчанию не удалось; автоматически восстанавливается сохраненный корень.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1950"/>
        <source>Rollback candidate mount/preflight failed; automatically restoring the preserved root.</source>
        <translation>Откат кандидата mount/preflight не удался; автоматически восстанавливается сохраненный корень.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1951"/>
        <source>Rollback candidate failed post-switch validation/boot reconciliation; automatically restoring the preserved root.</source>
        <translation>Кандидат в откат провалил проверку после переключения / сверку загрузки; автоматически восстанавливая сохраненный корень.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1952"/>
        <source>CRITICAL: cannot detach promoted rollback mounts; refusing to rename Btrfs roots during recovery.</source>
        <translation>КРИТИЧЕСКИЙ: не может отсоединять продвигаемые откатные крепления; отказ от переименования корней Btrfs при восстановлении.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1954"/>
        <source>CRITICAL: could not move failed rollback candidate out of @; refusing to promote the preserved root.</source>
        <translation>КРИТИЧЕСКИЙ: не смог выдвинуть несостоявшегося кандидата на откат из @; отказавшись продвигать сохранившийся корень.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1955"/>
        <source>CRITICAL: could not remount the Btrfs top-level filesystem read-write during rollback recovery.</source>
        <translation>CRITICAL: не удалось перемонтировать файловую систему верхнего уровня Btrfs во время восстановления отката.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1958"/>
        <source>CRITICAL ROLLBACK FAILURE: could not restore original @ after default-subvolume failure. Do not reboot.</source>
        <translation>CRITICAL ROLLBACK FAILURE: не удалось восстановить исходный @ после сбоя в объеме по умолчанию. Не перезагружай.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1959"/>
        <source>CRITICAL ROLLBACK FAILURE: automatic restoration was incomplete. Do not reboot until Btrfs/boot state is inspected manually.</source>
        <translation>КРИТИЧЕСКИЙ РОЛЛБАК ФАЙЛУР: Автоматическое восстановление было неполным. Не перезагружайтесь до тех пор, пока состояние Btrfs/boot не будет проверено вручную.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1960"/>
        <source>CRITICAL ROLLBACK FAILURE: could not restore original @ after candidate mount failure. Do not reboot.</source>
        <translation>CRITICAL ROLLBACK FAILURE: не удалось восстановить исходный @ после сбоя установки кандидата. Не перезагружай.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1961"/>
        <source>Detaching target mounts before atomic @ name switch.</source>
        <translation>Отсоединяя целевые установки перед атомным переключателем имени.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1962"/>
        <source>ROLLBACK FAILED SAFELY: original @ restored; failed candidate retained as %1.</source>
        <translation>ROLLBACK FAILED SAFELY: оригинальный @ восстановлен; неудавшийся кандидат сохранился как %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1963"/>
        <source>ROLLBACK FAILED SAFELY: original @ restored after default-subvolume failure; failed candidate retained as %1.</source>
        <translation>ROLLBACK FAILED SAFELY: оригинальный @ восстановлен после отказа по умолчанию; неудавшийся кандидат сохранился как %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1964"/>
        <source>ROLLBACK FAILED SAFELY: original @ restored after candidate mount failure; failed candidate retained as %1.</source>
        <translation>ROLLBACK FAILED SAFELY: оригинальный @ восстановлен после отказа установки; неудавшийся кандидат сохранен как %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1965"/>
        <source>Rollback kernel %1 is missing initramfs before rebuild; update-initramfs will be asked to regenerate it.</source>
        <translation>Ядро Роллбэка %1 отсутствует initramfs перед восстановлением; update-initramfs будет предложено регенерировать его.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1966"/>
        <source>Rollback kernel %1 has no matching modules directory.</source>
        <translation>Ядро Rollback %1 не имеет каталога соответствующих модулей.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1967"/>
        <source>Rollback kernel pair before rebuild: %1</source>
        <translation>Пара ядер отката перед восстановлением: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1968"/>
        <source>Rollback kernel pair before rebuild: %1 (mkinitcpio flavor naming)</source>
        <translation>Пара ядер отката перед восстановлением: %1 (название вкуса mkinitcpio)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1969"/>
        <source>PASS: original @ and its boot stack were restored automatically. Failed rollback candidate retained as %1.</source>
        <translation>PASS: оригинальный @ и его багажник были восстановлены автоматически. Неудавшийся кандидат на откат сохранился как %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1970"/>
        <source>CRITICAL: original @ was restored, but its boot-stack reconciliation also failed. Manual boot repair is required before reboot.</source>
        <translation>КРИТИЧЕСКИЙ: Оригинальный @ был восстановлен, но его сверка загрузочного стека также не удалась. Перед перезагрузкой требуется ручной ремонт багажника.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1971"/>
        <source>Rollback preflight: snapshot root fstab is compatible with promoted @.</source>
        <translation>Rollback preflight: snapshot root fstab совместим с промоутером @.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1972"/>
        <source>Rollback preflight: snapshot fstab has no root (/) entry.</source>
        <translation>Предварительный полет в обратном направлении: снимок fstab не имеет корневой (/) записи.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1973"/>
        <source>Rollback preflight: snapshot root fstab type is &apos;%1&apos;, not btrfs.</source>
        <translation>Перед полетом: снимок корня fstab типа «%1», а не btrfs.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1974"/>
        <source>Rollback preflight: snapshot fstab expects root subvolume &apos;%1&apos;, but transactional rollback promotes the selected snapshot to @.</source>
        <translation>Rollback preflight: snapshot fstab ожидает root subvolume &apos;%1&apos;, но транзакционный откат продвигает выбранный снимок на @.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1975"/>
        <source>Rollback preflight: snapshot / fstab source cannot be resolved: %1</source>
        <translation>Откат перед полетом: снимок / источник fstab не может быть разрешен: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1976"/>
        <source>Rollback preflight: snapshot / fstab root source has an unsupported format: %1</source>
        <translation>Rollback preflight: snapshot / fstab root source имеет неподдерживаемый формат: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1977"/>
        <source>Rollback preflight: snapshot / fstab source resolves to %1 rather than selected root %2.</source>
        <translation>Откат перед полетом: источник моментального снимка / fstab разрешает %1, а не выбранный корень %2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1979"/>
        <source>Preserved root name: %1</source>
        <translation>Оригинальное название: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1980"/>
        <source>PASS: promoted rollback root and boot stack validated.</source>
        <translation>PASS: продвигаемый корень отката и стек загрузки проверены.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1981"/>
        <source>ROLLBACK RECOVERY: restoring preserved pre-rollback @.</source>
        <translation>ROLLBACK RECOVERY: Восстановление сохраненного предварительного отката @.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1982"/>
        <source>Transactional rollback selected: snapshot %1 (%2).</source>
        <translation>Выбран транзакционный откат: снимок %1 (%2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1983"/>
        <source>Rollback source snapshot will remain unchanged.</source>
        <translation>Снимок источника Rollback останется неизменным.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1984"/>
        <source>Root component: %1 (%2)</source>
        <translation>Корневой компонент: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1985"/>
        <source>Root component fallback: selected component %1 (%2) lacks /etc/os-release; resolved %3 (%4) from %5.</source>
        <translation>Корневой компонент: выбранный компонент %1 (%2) не имеет /etc/os-release; разрешенный %3 (%4) от %5.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1986"/>
        <source>RPM missing-file repair: reinstalling %1 package(s) with missing files: %2</source>
        <translation>Восстановление недостающих файлов RPM: переустановка пакета (пакетов) %1 с отсутствующими файлами: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1987"/>
        <source>RPM missing-file detection: no missing package files; recording read-only dnf5 dependency-check evidence.</source>
        <translation>Обнаружение отсутствующих файлов RPM: отсутствие отсутствующих файлов пакетов; запись только для чтения dnf5.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1988"/>
        <source>rpm preflight version: %1</source>
        <translation>Предполетная версия: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1989"/>
        <source>Running-host identity and boot-mount check: PASS</source>
        <translation>Идентификация бегущего хозяина и проверка загрузки: PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1990"/>
        <source>Security: host-to-repair copies keep -aHAX (modes, ownership and xattrs are copied from the trusted host source).</source>
        <translation>Безопасность: копии хоста для ремонта сохраняют -aHAX (режимы, права собственности и xattrs копируются из надежного источника хоста).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1991"/>
        <source>Security: setuid/setgid bits and file capabilities are removed on repair-to-host copies.</source>
        <translation>Безопасность: биты setuid/setgid и возможности файлов удаляются на копиях для восстановления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1992"/>
        <source>Selected EFI System Partition: %1 (%2) mounted at %3</source>
        <translation>Выбранный системный раздел EFI: %1 (%2), установленный на %3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1993"/>
        <source>SELinux enforcement: %1</source>
        <translation>Системные требования SELinux: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1994"/>
        <source>SELinux status: %1</source>
        <translation>Состояние SELinux: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1995"/>
        <source>SELinux status: sestatus/getenforce are not installed in the target; recorded as unavailable.</source>
        <translation>Статус SELinux: сестатус/гетенфорс не устанавливаются в цель; регистрируются как недоступные.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1996"/>
        <source>========================================</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1997"/>
        <source>Session log was NOT appended into the target: unsafe target log path.</source>
        <translation>Журнал сеанса не был добавлен в цель: небезопасный путь регистрации цели.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1998"/>
        <source>Shell command changed diagnostic-relevant files.</source>
        <translation>Команда Shell изменила диагностические файлы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1999"/>
        <source>Shell command made no diagnostic-relevant filesystem change; cached diagnostics remain valid.</source>
        <translation>Команда Shell не внесла никаких изменений в файловую систему; кэшированная диагностика остается в силе.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2000"/>
        <source>SIMULATE: apk %1 --simulate (no packages will be changed)</source>
        <translation>SIMULATE: apk %1 - симуляция (пакеты не будут изменены)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2001"/>
        <source>SIMULATE: apt-get %1 (no packages will be changed)</source>
        <translation>SIMULATE: apt-get %1 (пакеты не будут изменены)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2002"/>
        <source>SIMULATE CORRECTION: %1</source>
        <translation>Упрощенная коррекция: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2003"/>
        <source>SIMULATE: dnf5 %1 --assumeno (no packages will be changed)</source>
        <translation>SIMULATE: dnf5 %1-assumeno (пакеты не будут изменены)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2004"/>
        <source>SIMULATE: apt-get --fix-broken install (no packages will be changed)</source>
        <translation>SIMULATE: установка apt-get-fix-broken (пакеты не будут изменены)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2005"/>
        <source>Simulation exit code for &apos;%1&apos;: %2</source>
        <translation>Код выхода симуляции для %1: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2006"/>
        <source>WARNING: skipping unresolved Btrfs fstab entry %1 -&gt; %2</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: пропуск неразрешенной записи Btrfs fstab %1 -&gt; %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2007"/>
        <source>WARNING: skipping unresolved data fstab entry %1 -&gt; %2</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: пропуск неразрешенной записи fstab данных %1 -&gt; %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2008"/>
        <source>Target has snapper apt hooks; guarding the chroot shell against snapshots (temporary, read-only)</source>
        <translation>Цель имеет крючки apt; защита раковины корня от снимков (временный, только для чтения)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2009"/>
        <source>WARNING: could not bind the read-only snapper guard over %1; the chroot shell continues without the snapshot kill-switch.</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: не может связать охрану только для чтения над %1; оболочка корня продолжается без переключателя.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2010"/>
        <source>Active writable root: @ (subvolume ID %1)</source>
        <translation>Активный корень записи: @ (подобный ID %1)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2011"/>
        <source>Initramfs/UKI/GRUB reconciliation: PASS</source>
        <translation>Выверка initramfs/UKI/GRUB: PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2012"/>
        <source>Btrfs default subvolume now points to the promoted @.</source>
        <translation>Объём по умолчанию Btrfs теперь указывает на продвигаемый @.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2013"/>
        <source>Snapshot inventory complete: %1 snapshot(s) found.</source>
        <translation>Завершена инвентаризация снимков: найден снимок (снимки) %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2014"/>
        <source>Snapshot inventory skipped: %1 is not Btrfs.</source>
        <translation>Пропущенный инвентарь снимков: %1 не является Btrfs.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2016"/>
        <source>SNAPSHOT ROLLBACK COMPLETE</source>
        <translation>Снэпшот Роллбэк комплет</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2017"/>
        <source>Selected snapshot: %1</source>
        <translation>Выбранный снимок: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2018"/>
        <source>Source snapshot retained unchanged.</source>
        <translation>Источник фото сохранился без изменений.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2019"/>
        <source>Sources: %1</source>
        <translation>Источник: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2020"/>
        <source>Removed leaked Boot Bitch session mount at %1 (source=%2)</source>
        <translation>Удаленный утечка Boot Bitch сессионное крепление на %1 (источник = %2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2021"/>
        <source>Target /dev: private writable tmpfs populated from the recovery host with a private devpts at /dev/pts; the running host /dev and its ptys are not modified or leaked.</source>
        <translation>Target /dev: частные записываемые tmpfs, загруженные из хоста восстановления с частными devpts в /dev/pts; запущенный хост /dev и его ptys не изменены или не просочились.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2022"/>
        <source>Target disk: %1</source>
        <translation>Целевой диск: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2023"/>
        <source>Target root mount: %1 (subvolume=%2)</source>
        <translation>Целевой корневой крепление: %1 (подобный = %2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2024"/>
        <source>TRY exit code: %1 (%2)</source>
        <translation>Код выхода TRY: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2025"/>
        <source>TRY: %1</source>
        <translation>Попробуй: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2026"/>
        <source>PASS: TUXEDO UKI root/LUKS/subvolume binding verified.</source>
        <translation>Проверено связывание TUXEDO UKI root/LUKS/subvolume.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2027"/>
        <source>Captured complete firmware entry state before UKI rebuild: %1</source>
        <translation>Полное состояние ввода прошивки перед восстановлением UKI: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2028"/>
        <source>TUXEDO UKI cmdline: %1</source>
        <translation>TUXEDO UKI cmdline: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2029"/>
        <source>TUXEDO UKI embedded kernel: %1</source>
        <translation>Встроенное ядро TUXEDO UKI: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2030"/>
        <source>PASS: existing TUXEDO UKI validated for %1 on selected ESP %2; no replacement image was needed.</source>
        <translation>PASS: существующий TUXEDO UKI, подтвержденный для %1 на выбранном ESP %2; замена изображения не требуется.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2031"/>
        <source>KNOWN ISSUE: UKI kernel is stale; vendor UKI rebuild will correct it.</source>
        <translation>Ядро UKI устарело; реконструкция UKI исправит его.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2032"/>
        <source>TUXEDO UKI layout detected; simulating/prereflighting the vendor boot path before rebuild.</source>
        <translation>Обнаружена компоновка TUXEDO UKI; имитация/предварительный полет пути загрузки поставщика перед восстановлением.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2033"/>
        <source>KNOWN ISSUE: newest TUXEDO kernel %1 has no matching initramfs; rebuilding initramfs before UKI.</source>
        <translation>Новейшее ядро TUXEDO %1 не имеет соответствующего initramfs; восстановление initramfs до UKI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2034"/>
        <source>Writable UEFI variables/efibootmgr are unavailable; UKI file rebuild will proceed without BootOrder restoration.</source>
        <translation>Перезаписываемые переменные UEFI/efibootmgr недоступны; восстановление файла UKI будет происходить без восстановления BootOrder.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2035"/>
        <source>WARNING: objcopy could not inspect the rebuilt UKI .uname section.</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: Обжкопия не смогла осмотреть перестроенный раздел UKI .uname.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2036"/>
        <source>UKI preflight: current embedded kernel=%1; target newest kernel=%2</source>
        <translation>UKI preflight: текущее встроенное ядро = %1; целевое новейшее ядро = %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2037"/>
        <source>PASS: TUXEDO UKI preflight prerequisites are satisfied.</source>
        <translation>Предварительные условия полета TUXEDO UKI выполнены.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2038"/>
        <source>PASS: TUXEDO UKI rebuilt for %1 on selected ESP %2</source>
        <translation>TUXEDO UKI восстановлен для %1 на выбранном ESP %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2039"/>
        <source>TUXEDO UKI repair read-only preflight: PASS (%1, %2)</source>
        <translation>TUXEDO UKI ремонт только для чтения перед полетом: PASS (%1, %2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2040"/>
        <source>PASS: vendor UKI command produced a changed TUX.EFI image.</source>
        <translation>Команда UKI произвела измененное изображение TUX.EFI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2041"/>
        <source>WARNING: vendor UKI command completed without changing TUX.EFI; the existing image will be validated against the selected target before this repair is reported successful.</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: команда поставщика UKI выполнена без изменения TUX.EFI; существующее изображение будет проверено на выбранную цель до того, как будет сообщено об успешном ремонте.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2042"/>
        <source>/boot: %1</source>
        <translation>/boot: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2043"/>
        <source>/boot/efi: %1</source>
        <translation>/boot/efi: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2044"/>
        <source>Validation complete; no target files were changed.</source>
        <translation>Проверка завершена; целевые файлы не были изменены.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2045"/>
        <source>/etc/crypttab: %1</source>
        <translation>/etc/crypttab: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2046"/>
        <source>ESP mount candidate: %1</source>
        <translation>Кандидат на установку ESP: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2047"/>
        <source>/etc/fstab: %1</source>
        <translation>/etc/fstab: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2048"/>
        <source>GRUB config: %1</source>
        <translation>Конфигурация GRUB: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2049"/>
        <source>Supported modifying backend: %1</source>
        <translation>Поддерживаемый модифицирующий бэкэнд: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2050"/>
        <source>OS: %1</source>
        <translation>Производитель: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2051"/>
        <source>Root: %1</source>
        <translation>Источник: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2052"/>
        <source>Root filesystem: %1</source>
        <translation>Корневая файловая система: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2053"/>
        <source>Root fs: %1</source>
        <translation>Корень fs: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2054"/>
        <source>Repair root mount source: %1</source>
        <translation>Ремонт корневого монтажного источника: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2055"/>
        <source>Root subvolume: %1</source>
        <translation>Корневой объем: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2056"/>
        <source>Validation summary</source>
        <translation>Сводная валидация</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2057"/>
        <source>Vendor command completed successfully: %1</source>
        <translation>Командование поставщика выполнено успешно: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2058"/>
        <source>Repair-to-host capability scan: xattr inspection tooling is unavailable; the rsync security.capability filter was applied and trusted.</source>
        <translation>Сканирование возможностей ремонта до хоста: инструментарий проверки xattr недоступен; безопасность rsync. Функциональный фильтр применялся и пользовался доверием.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2059"/>
        <source>PASS: zpool scrub clean; clearing pool error counters with zpool clear.</source>
        <translation>PASS: zpool scrub clean; очистка бассейна счетчики ошибок с zpool clear.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2066"/>
        <source>Alpine apk audit --system (missing-file detection)</source>
        <translation>Альпийский аудит apk - система (детектирование недостающих файлов)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2067"/>
        <source>Alpine apk info --who-owns (%1 missing files)</source>
        <translation>Информация о Alpine apk - кто владеет (отсутствующие файлы %1)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2068"/>
        <source>Arch pacman full transaction preflight</source>
        <translation>Arch pacman полная трансакция</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2069"/>
        <source>Build and verify Fedora initramfs for %1 (temporary output only)</source>
        <translation>Создайте и проверьте Fedora initramfs для %1 (только временный выход)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2070"/>
        <source>Check Alpine grub-install availability/version</source>
        <translation>Alpine grub-install Доступность/версия</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2071"/>
        <source>Check grub-install availability/version</source>
        <translation>Проверьте доступность/версию grub-install</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2073"/>
        <source>Complete interrupted package configuration before correction</source>
        <translation>Полная конфигурация пакета перед коррекцией</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2074"/>
        <source>Create missing initramfs for %1</source>
        <translation>Недостающий initramfs для %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2075"/>
        <source>dnf5 dependency check (read-only evidence)</source>
        <translation>dnf5 проверка зависимостей (только для чтения)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2076"/>
        <source>Inspect akmods state</source>
        <translation>Проверить состояние akmods</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2077"/>
        <source>Inspect akms state</source>
        <translation>Проверить состояние akms</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2078"/>
        <source>Inspect DKMS state</source>
        <translation>Проверить состояние DKMS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2079"/>
        <source>Inspect DKMS state for GPU driver</source>
        <translation>Проверить состояние DKMS для драйвера GPU</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2080"/>
        <source>Post-repair akmods status</source>
        <translation>Постремонтный статус akmods</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2081"/>
        <source>Post-repair akms status</source>
        <translation>Постремонтный статус akms</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2082"/>
        <source>Post-repair DKMS status</source>
        <translation>Постремонтный статус DKMS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2083"/>
        <source>Purge the wrong GPU driver</source>
        <translation>Очистите неправильный драйвер GPU</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2084"/>
        <source>Read Fedora grubenv (grub2-editenv list)</source>
        <translation>Fedora grubenv (список grub2-editenv)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2085"/>
        <source>Rebuild akmods modules for kernel %1</source>
        <translation>Модули akmods для ядра %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2086"/>
        <location filename="../src/MainWindow.cpp" line="17221"/>
        <source>Rebuild AKMS modules</source>
        <translation>Реконструкция модулей AKMS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2087"/>
        <source>Rebuild Alpine initramfs for %1 with mkinitfs</source>
        <translation>Альпийский initramfs для %1 с mkinitfs</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2088"/>
        <source>Rebuild Arch initramfs images with mkinitcpio -P</source>
        <translation>Восстановление изображений Arch initramfs с помощью mkinitcpio-P</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2090"/>
        <source>Record Alpine apk version</source>
        <translation>Альпийская версия apk</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2091"/>
        <source>Record dnf5 version</source>
        <translation>Рекордная версия dnf5</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2092"/>
        <source>Record dracut version</source>
        <translation>Рекордная версия dracut</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2093"/>
        <source>Record grub2-install version</source>
        <translation>Рекордная версия grub2-install</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2094"/>
        <source>Record rpm version</source>
        <translation>Рекордная версия rpm</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2095"/>
        <source>Record SELinux enforcement mode</source>
        <translation>Режим защиты Record SELinux</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2096"/>
        <source>Record SELinux status</source>
        <translation>Рекордный статус SELinux</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2097"/>
        <source>Refresh dnf5 package metadata (dnf5 makecache)</source>
        <translation>Обновить метаданные пакета dnf5 (dnf5 makecache)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2098"/>
        <source>Regenerate extlinux configuration (overwrite=0 trial; no boot sector write)</source>
        <translation>Регенерировать конфигурацию extlinux (перезапись = 0 проб; загрузочный сектор не записывается)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2099"/>
        <source>Regenerate extlinux configuration (default=%1, overwrite=0 trial; no boot sector write)</source>
        <translation>Восстановление конфигурации extlinux (по умолчанию = %1, перезапись = 0 проб; загрузочный сектор не записывается)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2100"/>
        <source>Regenerate Fedora GRUB2 configuration (grub2-mkconfig --no-grubenv-update)</source>
        <translation>Регенерировать конфигурацию Fedora GRUB2 (grub2-mkconfig-no-grubenv-update)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2102"/>
        <source>Regenerate GRUB configuration with grub-mkconfig</source>
        <translation>Восстановление конфигурации GRUB с помощью grub-mkconfig</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2103"/>
        <source>Reinstall Alpine GRUB EFI bootloader (--no-nvram)</source>
        <translation>Reinstall Alpine GRUB EFI bootloader (--no-nvram)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2104"/>
        <source>Reinstall GRUB EFI bootloader</source>
        <translation>Установить загрузчик GRUB EFI</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2105"/>
        <source>Reinstall GRUB EFI fallback loader (--removable)</source>
        <translation>Установить резервный погрузчик GRUB EFI (съемный)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2106"/>
        <source>Reinstall GRUB EFI bootloader files</source>
        <translation>Переустановка файлов загрузчика GRUB EFI</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2107"/>
        <source>Reinstall GRUB2 BIOS boot code</source>
        <translation>Переустановить загрузочный код GRUB2 BIOS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2108"/>
        <source>Reinstall protected kernel packages (rollback)</source>
        <translation>Установка защищенных пакетов ядра (rollback)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2109"/>
        <location filename="../src/MainWindow.cpp" line="20356"/>
        <source>Remove orphaned kernel build support</source>
        <translation>Удалить поддержку сиротского ядра</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2110"/>
        <source>Remove orphaned kernel headers</source>
        <translation>Удалить сиротские заголовки ядра</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2111"/>
        <source>Remove superseded kernel packages</source>
        <translation>Удалить замещенные пакеты ядра</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2112"/>
        <source>Repair broken dependencies before known package correction</source>
        <translation>Ремонт сломанных зависимостей до известной коррекции пакета</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2114"/>
        <source>Restore package selections after failed GPU repair</source>
        <translation>Восстановление пакетов после неудачного ремонта GPU</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2115"/>
        <source>Retry DKMS autoinstall after header correction</source>
        <translation>Автоустановка DKMS после коррекции заголовка</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2116"/>
        <source>Retry GRUB configuration after mapper alias correction</source>
        <translation>Повторить конфигурацию GRUB после коррекции псевдонима картографа</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2117"/>
        <source>Retry GRUB EFI bootloader files without NVRAM update</source>
        <translation>Перезагрузка файлов GRUB EFI без обновления NVRAM</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2118"/>
        <source>Retry initramfs for %1 after mapper alias correction</source>
        <translation>Повторите initramfs для %1 после коррекции псевдонима картографа</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2119"/>
        <source>Retry trial initramfs build for %1 after mapper alias correction</source>
        <translation>Пробная сборка initramfs для %1 после коррекции псевдонима картографа</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2120"/>
        <source>RPM file ownership mapping (rpm -qf, %1 missing files)</source>
        <translation>Картирование владения файлами RPM (rpm -qf, %1 отсутствующие файлы)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2121"/>
        <source>RPM package file verification (rpm -Va --nofiledigest, read-only)</source>
        <translation>Проверка файлов пакета RPM (rpm -Va -nofiledigest, только для чтения)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2122"/>
        <source>Set Fedora GRUB2 default BLS entry (%1)</source>
        <translation>Установка Fedora GRUB2 по умолчанию BLS (%1)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2123"/>
        <source>Set one-boot nomodeset next_entry</source>
        <translation>Установить one-boot nomodeset next entry</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2124"/>
        <source>Syntax-check %1 OpenRC init script</source>
        <translation>Синтаксическая проверка скрипта %1 OpenRC init</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2125"/>
        <source>Syntax-check trial Fedora GRUB2 configuration</source>
        <translation>Проверка синтаксиса Fedora GRUB2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2126"/>
        <source>Syntax-check trial GRUB configuration</source>
        <translation>Проверка синтаксиса конфигурации GRUB</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2127"/>
        <source>Trial initramfs build for %1 (temporary output only)</source>
        <translation>Пробная сборка initramfs для %1 (только временный выход)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2128"/>
        <source>Trial mkinitcpio build for %1 (temporary output only)</source>
        <translation>Пробная сборка mkinitcpio для %1 (только временный выход)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2129"/>
        <source>Trial mkinitfs build for %1 (temporary output only)</source>
        <translation>Пробная сборка mkinitfs для %1 (только временный выход)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2130"/>
        <source>Update initramfs for %1</source>
        <translation>Обновление initramfs для %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2131"/>
        <source>Upgrade installed packages (%1)</source>
        <translation>Обновление установленных пакетов (%1)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2132"/>
        <source>Verify Fedora GRUB2 configuration syntax after boot-code reinstall</source>
        <translation>Проверьте синтаксис конфигурации Fedora GRUB2 после переустановки загрузочного кода</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2133"/>
        <source>Verify installed Fedora GRUB2 configuration syntax</source>
        <translation>Проверьте установленный синтаксис конфигурации Fedora GRUB2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2134"/>
        <source>Verify installed GRUB configuration syntax</source>
        <translation>Проверьте установленный синтаксис конфигурации GRUB</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2135"/>
        <source>Verify mkinitfs build inputs for %1 (mkinitfs -l)</source>
        <translation>Проверка входов сборки mkinitfs для %1 (mkinitfs-l)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2136"/>
        <source>Verify %1 service exists (rc-service -e)</source>
        <translation>Проверка наличия службы %1 (rc-service -e)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2326"/>
        <location filename="../src/MainWindow.cpp" line="15185"/>
        <source>Repair tool %1 is unavailable.</source>
        <translation>Ремонтный инструмент %1 недоступен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2332"/>
        <location filename="../src/MainWindow.cpp" line="15190"/>
        <source>Repair tool %1 has unknown or unavailable diagnostic evidence.</source>
        <translation>Ремонтный инструмент %1 имеет неизвестные или недоступные диагностические доказательства.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2339"/>
        <location filename="../src/MainWindow.cpp" line="15193"/>
        <source>No capability evidence for repair tool %1 in the selected scope. Run diagnostics first.</source>
        <translation>Нет доказательств возможности ремонта инструмента %1 в выбранной области. Сначала проведите диагностику.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2469"/>
        <location filename="../src/MainWindow.cpp" line="2474"/>
        <location filename="../src/MainWindow.cpp" line="2545"/>
        <source>Running-host default boot entry selection is unavailable.</source>
        <translation>Выбор загрузки Running-host по умолчанию недоступен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2484"/>
        <location filename="../src/MainWindow.cpp" line="2503"/>
        <source>The helper did not identify a bootable running-host default entry.</source>
        <translation>Помощник не идентифицировал загрузочную запись по умолчанию.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="3777"/>
        <source>About %1</source>
        <translation>О компании %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4327"/>
        <source>Linux detected — %1</source>
        <translation>Обнаружен Linux — %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4336"/>
        <source>Unlocked Linux filesystem — inspect to confirm</source>
        <translation>Разблокированная файловая система Linux — проверка для подтверждения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4339"/>
        <source>Likely Linux — inspect to confirm</source>
        <translation>Вероятнее всего Linux — проверьте</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4342"/>
        <source>Likely Linux — encrypted</source>
        <translation>Likely Linux — зашифрованный</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4345"/>
        <source>Encrypted — unlock to inspect</source>
        <translation>Зашифрованный — разблокировать для проверки</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4348"/>
        <source>Linux-capable — inspect to confirm</source>
        <translation>Linux-совместимый — проверить, чтобы подтвердить</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4350"/>
        <source>Available for inspection</source>
        <translation>Доступно для инспекции</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4370"/>
        <source>Encrypted volume</source>
        <translation>Зашифрованный том</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4372"/>
        <source>%1 volume</source>
        <translation>Объем %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4374"/>
        <source>Partition</source>
        <translation>Разделение</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4379"/>
        <source>Unnamed device</source>
        <translation>Неназванное устройство</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5092"/>
        <source>Preparing the Boot Bitch administrator session; authorization may be requested before the action starts.</source>
        <translation>Подготовка сеанса администратора Boot Bitch; авторизация может быть запрошена до начала действия.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5334"/>
        <location filename="../src/MainWindow.cpp" line="19895"/>
        <location filename="../src/MainWindow.cpp" line="22706"/>
        <source>Repair file system errors</source>
        <translation>Исправление ошибок файловой системы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5342"/>
        <source>The read-only check found file system errors in the %1. Select the devices to repair and the mode to use. Each repair runs separately after the helper repeats its scope, mount-state and tool preflights.</source>
        <translation>Проверка только для чтения обнаружила ошибки файловой системы в %1. Выберите устройства для ремонта и режим использования. Каждый ремонт выполняется отдельно после того, как помощник повторяет свой объем, состояние крепления и предполетные полёты.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5354"/>
        <location filename="../src/MainWindow.cpp" line="6567"/>
        <location filename="../src/MainWindow.cpp" line="6568"/>
        <source>Repair</source>
        <translation>ремонт</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5354"/>
        <location filename="../src/MainWindow.cpp" line="6194"/>
        <source>Device</source>
        <translation>Устройство</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5355"/>
        <source>File system</source>
        <translation>Файловая система</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5355"/>
        <source>Mode</source>
        <translation>Режим</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5356"/>
        <source>Issue summary</source>
        <translation>Резюме проблемы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5749"/>
        <source>Select and inspect the protected host and repair targets.</source>
        <translation>Выберите и проверьте защищенный хост и отремонтируйте цели.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5750"/>
        <source>Run read-only diagnostics for the current scope.</source>
        <translation>Запустите диагностику только для чтения для текущего объема.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5751"/>
        <source>Manage UEFI firmware boot entries and view the boot menu.</source>
        <translation>Управляйте загрузками прошивки UEFI и просматривайте меню загрузки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5752"/>
        <source>Run guarded repair tools and the Full Repair plan.</source>
        <translation>Запускайте охраняемые ремонтные инструменты и план полного ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5753"/>
        <source>Inspect and roll back Btrfs root snapshots.</source>
        <translation>Проверяйте и откатывайте снимки корня Btrfs.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5755"/>
        <source>Copy files between the host and the repair system.</source>
        <translation>Копирование файлов между хостом и системой ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5756"/>
        <source>Review the session log and diagnostic history.</source>
        <translation>Просмотрите журнал сеанса и историю диагностики.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5757"/>
        <source>Configure the Full Repair plan and preferences.</source>
        <translation>Настройка плана полного ремонта и предпочтений.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6261"/>
        <location filename="../src/MainWindow.cpp" line="10806"/>
        <source>Authorization deferred; press Authorize.</source>
        <translation>Разрешение отложено; разрешение прессы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6420"/>
        <source>Select a target configuration file to inspect or edit.</source>
        <translation>Выберите целевой файл конфигурации для проверки или редактирования.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6621"/>
        <location filename="../src/MainWindow.cpp" line="6769"/>
        <source>Select a repair drive, or enter Host Maintenance.</source>
        <translation>Выберите ремонтный привод или введите обслуживание хоста.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6716"/>
        <location filename="../src/MainWindow.cpp" line="20513"/>
        <location filename="../src/MainWindow.cpp" line="22152"/>
        <source>GPU driver repair</source>
        <translation>Ремонт драйверов GPU</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11427"/>
        <source>Choose an action</source>
        <translation>Выберите действие</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15293"/>
        <location filename="../src/MainWindow.cpp" line="15316"/>
        <location filename="../src/MainWindow.cpp" line="15317"/>
        <location filename="../src/MainWindow.cpp" line="20391"/>
        <location filename="../src/MainWindow.cpp" line="20394"/>
        <location filename="../src/MainWindow.cpp" line="20660"/>
        <location filename="../src/MainWindow.cpp" line="22138"/>
        <location filename="../src/MainWindow.cpp" line="22141"/>
        <source>, </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16048"/>
        <location filename="../src/MainWindow.cpp" line="16574"/>
        <source>Running diagnostics…</source>
        <translation>Диагностика …</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16918"/>
        <source>Clear the action register and cached diagnostics?</source>
        <translation>Очистить реестр действий и кэшированную диагностику?</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16919"/>
        <source>Cached diagnostics will also be cleared, and repair tools will be disabled until diagnostics are regenerated.</source>
        <translation>Кашедная диагностика также будет очищена, а инструменты для ремонта будут отключены до тех пор, пока диагностика не будет восстановлена.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16948"/>
        <source>Cached diagnostics cleared; repair tools are disabled until diagnostics are regenerated.</source>
        <translation>Кашедная диагностика очищена; ремонтные инструменты отключены до восстановления диагностики.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17218"/>
        <source>Rebuild akmods modules</source>
        <translation>Реконструкция модулей akmods</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17230"/>
        <source>Rebuild akmods</source>
        <translation>Реконструкция akmods</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17233"/>
        <source>Rebuild AKMS</source>
        <translation>Обновление AKMS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20083"/>
        <location filename="../src/MainWindow.cpp" line="20085"/>
        <location filename="../src/MainWindow.cpp" line="20520"/>
        <location filename="../src/MainWindow.cpp" line="20610"/>
        <source>Interactive tool — run individually</source>
        <translation>Интерактивный инструмент — запускается индивидуально</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20216"/>
        <source>Running kernel: %1.</source>
        <translation>Запуск ядра: %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20219"/>
        <source>Newest installed kernel: %1.</source>
        <translation>Новейшее ядро: %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20222"/>
        <source>Repository latest kernel: %1 (newer than the installed newest).</source>
        <translation>Репозиторий последнего ядра: %1 (новее установленного новейшего).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20225"/>
        <source>No superseded (older) kernels are installed.</source>
        <translation>Не устанавливаются замещаемые (старые) ядра.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20227"/>
        <source>Superseded kernel(s) available for removal: %1.</source>
        <translation>Замененное ядро (ядра), доступное для удаления: %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20240"/>
        <source>Approximately %1 MB would be restored.</source>
        <translation>Примерно %1 MB будет восстановлен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20245"/>
        <source>Missing headers (latest kernel has no matching headers): %1.</source>
        <translation>Отсутствующие заголовки (последнее ядро не имеет совпадающих заголовков): %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20253"/>
        <source>Orphaned kernel components (no owning package): %1.</source>
        <translation>Компоненты ядра с орфаном (не имеющие упаковки): %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20257"/>
        <source>Orphaned kernel build support (kernel gone): %1.</source>
        <translation>Поддержка сборки ядра (ядро исчезло): %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20261"/>
        <source>Incomplete kernels (installed package without its on-disk files): %1.</source>
        <translation>Неполные ядра (установленный пакет без файлов на диске): %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20340"/>
        <source>Remove oldest/superseded kernel(s)</source>
        <translation>Удалить старейшее/вытесненное ядро(ядра)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20344"/>
        <source>Remove newest kernel</source>
        <translation>Удалить новейшее ядро</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20348"/>
        <source>Remove orphaned headers</source>
        <translation>Удалить осиротевшие головы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20352"/>
        <source>Remove orphaned kernel components</source>
        <translation>Удалить осиротевшие компоненты ядра</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20360"/>
        <source>Install missing headers</source>
        <translation>Установите недостающие заголовки</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20364"/>
        <source>Reinstall/complete a kernel</source>
        <translation>Восстановление/заполнение ядра</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20368"/>
        <source>Install repository-latest kernel</source>
        <translation>Установите ядро Repository-Last</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20391"/>
        <source>Kernel repair refused: run the %1 diagnostic first (no cached kernel evidence).</source>
        <translation>Ремонт ядра отказался: сначала проведите диагностику %1 (нет доказательств кэширования ядра).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20393"/>
        <source>Kernel repair unavailable</source>
        <translation>Ремонт ядра недоступен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20394"/>
        <source>Run the %1 diagnostic before starting kernel repair.</source>
        <translation>Проведите диагностику %1 перед началом ремонта ядра.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20488"/>
        <source>Rebuild out-of-tree kernel modules for kernels installed in the running host or selected repair system using the detected %1 build system. The helper refuses this action when the detected build system is not installed in the selected system.</source>
        <translation>Восстановление модулей ядра из дерева для ядер, установленных в работающем хосте или выбранной системе ремонта с использованием обнаруженной системы сборки %1. Помощник отказывается от этого действия, когда обнаруженная система сборки не установлена в выбранной системе.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20651"/>
        <location filename="../src/MainWindow.cpp" line="20665"/>
        <source>
Status: unavailable — %1</source>
        <translation>
Статус: недоступен - %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20658"/>
        <source>
Status: unavailable — missing diagnostic: %1</source>
        <translation>
Статус: недоступен — отсутствует диагностика: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20685"/>
        <source>Rebuild %1 kernel modules.</source>
        <translation>Восстановление модулей ядра %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20699"/>
        <source>Choose a kernel repair action from the evidence.</source>
        <translation>Выберите действие по ремонту ядра из доказательств.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6731"/>
        <source>Mirrors the Settings → Full Repair plan checkbox.</source>
        <translation>Зеркала в флажке плана полного ремонта Settings →.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6966"/>
        <source>Load Btrfs root snapshots read-only.</source>
        <translation>Загрузите снимки корня Btrfs только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7825"/>
        <source>Marked for deletion; Save applies it.</source>
        <translation>Помечено для удаления; Сохранить применяется.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8179"/>
        <source>Move the selected boot entry up; not applied until Save.</source>
        <translation>Переместите выбранную загрузочную запись; не применяется до сохранения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8185"/>
        <source>Move the selected boot entry down; not applied until Save.</source>
        <translation>Переместите выбранную загрузочную запись вниз; не применяется до сохранения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8217"/>
        <source>Read-only; switch to the EFI/UKI view to restore.</source>
        <translation>Переключитесь на вид EFI/UKI для восстановления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9282"/>
        <source>Automatic install requires package mapping and authorization.</source>
        <translation>Автоматическая установка требует отображения пакетов и авторизации.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10792"/>
        <source>Authorization request already running.</source>
        <translation>Запрос на авторизацию уже запущен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12012"/>
        <source>Host maintenance is active; file copy and chroot stay target-only.</source>
        <translation>Поддержка хоста активна; копия файла и chroot остаются только целевыми.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13137"/>
        <source>Run a guarded rsync dry-run.</source>
        <translation>Запускай охраняемую рысь.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17782"/>
        <source>Entries from this window.</source>
        <translation>Вход из этого окна.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20035"/>
        <source>Run the selected stages using the cached diagnostics.</source>
        <translation>Запустите выбранные этапы с помощью кэшированной диагностики.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20518"/>
        <source>Repair GPU driver</source>
        <translation>Ремонт драйвера GPU</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20673"/>
        <source>Run the read-only safety preflight.</source>
        <translation>Запускайте только для чтения предохранитель.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20675"/>
        <source>Check and repair the detected file systems.</source>
        <translation>Проверка и ремонт обнаруженных файловых систем.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20677"/>
        <source>Complete interrupted package configuration.</source>
        <translation>Полная прерванная конфигурация пакета.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20679"/>
        <source>Repair broken package dependencies.</source>
        <translation>Ремонт сломанных пакетных зависимостей.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20681"/>
        <source>Refresh package metadata.</source>
        <translation>Обновить метаданные пакета.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20683"/>
        <source>Simulate and apply a safe package upgrade.</source>
        <translation>Моделируйте и применяйте безопасное обновление пакета.</translation>
    </message>
    <message>
        <source>Rebuild DKMS kernel modules.</source>
        <translation type="vanished">Восстановление модулей ядра DKMS.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20687"/>
        <source>Restore the graphical login manager.</source>
        <translation>Восстановите графический менеджер входа.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20689"/>
        <source>Rebuild initramfs images.</source>
        <translation>Восстановление изображений initramfs.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20691"/>
        <source>Repair the EFI / UKI boot path.</source>
        <translation>Ремонт дорожки загрузки EFI/UKI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20693"/>
        <source>Regenerate the GRUB2 configuration.</source>
        <translation>Восстановление конфигурации GRUB2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20693"/>
        <source>Regenerate the GRUB configuration.</source>
        <translation>Регенерируйте конфигурацию GRUB.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20695"/>
        <source>Regenerate the extlinux configuration.</source>
        <translation>Восстановление конфигурации extlinux.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20697"/>
        <source>Reconcile the boot stack.</source>
        <translation>Примирить багажник.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20701"/>
        <source>No repair workflow is registered.</source>
        <translation>Ремонтный процесс не регистрируется.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20709"/>
        <source>Reuses this session&apos;s bootloader repair; mapper/crypttab and initramfs still run.</source>
        <translation>Повторное использование загрузчика этой сессии; Mapper/crypttab и initramfs все еще работают.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5409"/>
        <source>The read-only check reported issues.</source>
        <translation>Проверка только для чтения сообщила о проблемах.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5424"/>
        <source>Offline repair modes refuse mounted filesystems; btrfs scrub and zpool scrub are online modes and require a mounted filesystem. btrfs check --repair asks for an extra backup warning before it runs.</source>
        <translation>Режимы автономного ремонта отказываются от установленных файловых систем; btrfs scrub и zpool scrub являются онлайн-режимами и требуют установленной файловой системы. btrfs check — ремонт требует дополнительного резервного предупреждения перед запуском.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5432"/>
        <source>Run Selected Repairs</source>
        <translation>Запуск выбранных ремонтов</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5626"/>
        <source>Linux recovery and boot-repair utility</source>
        <translation>Восстановление Linux и загрузочная утилита</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5628"/>
        <source>GUARDED REPAIR  •  %1</source>
        <translation>Обсуждение • %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5660"/>
        <location filename="../src/MainWindow.cpp" line="5664"/>
        <source>A Boot Bitch operation is running in the background.</source>
        <translation>Операция Boot Bitch выполняется в фоновом режиме.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5713"/>
        <source>Show previous tab</source>
        <translation>Показать предыдущую вкладку</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5718"/>
        <source>Show next tab</source>
        <translation>Показать следующую вкладку</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5737"/>
        <location filename="../src/MainWindow.cpp" line="6009"/>
        <location filename="../src/MainWindow.cpp" line="6059"/>
        <location filename="../src/MainWindow.cpp" line="6060"/>
        <source>Systems</source>
        <translation>Системы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5738"/>
        <location filename="../src/MainWindow.cpp" line="6011"/>
        <location filename="../src/MainWindow.cpp" line="6386"/>
        <location filename="../src/MainWindow.cpp" line="6388"/>
        <location filename="../src/MainWindow.cpp" line="9056"/>
        <location filename="../src/MainWindow.cpp" line="9181"/>
        <source>Diagnostics</source>
        <translation>диагностика</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5739"/>
        <location filename="../src/MainWindow.cpp" line="7196"/>
        <location filename="../src/MainWindow.cpp" line="7198"/>
        <location filename="../src/MainWindow.cpp" line="7485"/>
        <source>EFI boot entries</source>
        <translation>Загрузка EFI</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5740"/>
        <source>Repair</source>
        <comment>tab noun</comment>
        <translation>ремонт</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5741"/>
        <location filename="../src/MainWindow.cpp" line="6849"/>
        <source>Snapshots</source>
        <translation>Снимки</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5742"/>
        <location filename="../src/MainWindow.cpp" line="8663"/>
        <location filename="../src/MainWindow.cpp" line="13653"/>
        <location filename="../src/MainWindow.cpp" line="13683"/>
        <source>Chroot Shell</source>
        <translation>Chroot Shell</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5743"/>
        <location filename="../src/MainWindow.cpp" line="8742"/>
        <source>File Copy</source>
        <translation>Копия файла</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5744"/>
        <location filename="../src/MainWindow.cpp" line="6013"/>
        <location filename="../src/MainWindow.cpp" line="8980"/>
        <source>Logs</source>
        <translation>Лог</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5745"/>
        <location filename="../src/MainWindow.cpp" line="6015"/>
        <source>Settings</source>
        <translation>Настройки</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5823"/>
        <source>Ready — guarded repair mode</source>
        <translation>Готовый — режим охраняемого ремонта</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5985"/>
        <source>&amp;File</source>
        <translation>&amp;Файл</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5986"/>
        <location filename="../src/MainWindow.cpp" line="6063"/>
        <source>Refresh Devices</source>
        <translation>Обновить устройства</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5992"/>
        <source>Lock Administrator Session</source>
        <translation>Закрытие сессии администратора</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6000"/>
        <source>Administrator session locked — owned target resources released; the next privileged action will request authorization.</source>
        <translation>Сессия администратора заблокирована — выделены собственные целевые ресурсы; следующее привилегированное действие потребует авторизации.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6004"/>
        <source>Quit</source>
        <translation>Бросить</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6008"/>
        <source>&amp;View</source>
        <translation>&amp;Обзор</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6019"/>
        <source>Auto-size Device Columns</source>
        <translation>Колонки устройств автоматического размера</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6022"/>
        <source>Wrap Log Lines</source>
        <translation>Оберните лог-линии</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6027"/>
        <source>&amp;Help</source>
        <translation>&amp;помогает</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6028"/>
        <location filename="../src/MainWindow.cpp" line="19043"/>
        <source>Using Boot Bitch</source>
        <translation>Использование Boot Bitch</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6031"/>
        <location filename="../src/MainWindow.cpp" line="19066"/>
        <source>About Boot Bitch</source>
        <translation>Обсуждение Boot Bitch</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6061"/>
        <source>Select a physical drive; Boot Bitch resolves the most likely Linux system volume automatically. The running host stays protected from ordinary target repairs, with a separate explicit host-maintenance path for its own system.</source>
        <translation>Выберите физический диск; Boot Bitch автоматически решает наиболее вероятный объем системы Linux. Работающий хост остается защищенным от обычного ремонта цели, с отдельным явным способом обслуживания хоста для своей собственной системы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6064"/>
        <source>Re-scan connected storage and repair targets.</source>
        <translation>Повторное сканирование подключенных объектов хранения и ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6101"/>
        <source>Detecting running system…</source>
        <translation>Обнаружение запущенной системы …</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6112"/>
        <source>Detecting protected storage…</source>
        <translation>Защищенное хранилище …</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6132"/>
        <source>PROTECTED</source>
        <translation>защищенный</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6138"/>
        <source>The running host remains protected from ordinary repair-target operations.</source>
        <translation>Работающий хост остается защищенным от обычных операций по ремонту.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6141"/>
        <source>Details</source>
        <translation>Подробности</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6143"/>
        <source>Show read-only details for the protected running host.</source>
        <translation>Покажите только для чтения детали для защищенного хоста.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6147"/>
        <location filename="../src/MainWindow.cpp" line="9867"/>
        <source>Host Maintenance</source>
        <translation>Обслуживание хоста</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6154"/>
        <location filename="../src/MainWindow.cpp" line="9997"/>
        <source>Make Default</source>
        <translation>Сделать Default</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6171"/>
        <source>Available repair targets</source>
        <translation>Доступные ремонтные цели</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6172"/>
        <source>Repair targets</source>
        <translation>Ремонтные цели</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6173"/>
        <source>Drives are ranked by visible Linux, EFI, filesystem and encryption evidence. Select the top-level drive; partitions and mapped volumes are informational.</source>
        <translation>Диски ранжируются по видимым Linux, EFI, файловой системе и доказательствам шифрования. Выберите диск верхнего уровня; разделы и отображенные тома являются информационными.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6175"/>
        <source>Most likely first</source>
        <translation>Скорее всего, первым</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6193"/>
        <source>Model / Label</source>
        <translation>Модель / этикетка</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6193"/>
        <location filename="../src/MainWindow.cpp" line="6888"/>
        <location filename="../src/MainWindow.cpp" line="9229"/>
        <source>Status</source>
        <translation>статус</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6193"/>
        <source>Connection</source>
        <translation>Подключение</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6194"/>
        <source>Size</source>
        <translation>Размер</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6194"/>
        <source>Filesystem</source>
        <translation>Файловая система</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6238"/>
        <source>Select Target</source>
        <translation>Выберите цель</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6242"/>
        <location filename="../src/MainWindow.cpp" line="9809"/>
        <location filename="../src/MainWindow.cpp" line="10161"/>
        <location filename="../src/MainWindow.cpp" line="11840"/>
        <source>Unlock</source>
        <translation>Разблокировать</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6244"/>
        <location filename="../src/MainWindow.cpp" line="9811"/>
        <source>Select a drive whose detected target is a locked LUKS volume.</source>
        <translation>Выберите диск, целью которого является заблокированный объем LUKS.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6265"/>
        <source>Authorize</source>
        <translation>разрешать</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6273"/>
        <location filename="../src/MainWindow.cpp" line="12009"/>
        <source>Committed target: none</source>
        <translation>Обязательная цель: нет</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6279"/>
        <source>Unlock status</source>
        <translation>Статус разблокировки</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6293"/>
        <location filename="../src/MainWindow.cpp" line="9814"/>
        <source>Select a drive to see unlock status.</source>
        <translation>Выберите диск, чтобы увидеть статус разблокировки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6294"/>
        <location filename="../src/MainWindow.cpp" line="10156"/>
        <location filename="../src/MainWindow.cpp" line="10840"/>
        <source>No unlock operation recorded for this drive in the current session.</source>
        <translation>Операция разблокировки не записывалась для этого диска в текущей сессии.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6298"/>
        <source>Selected drive details</source>
        <translation>Выбранные детали диска</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6338"/>
        <source>Drive:</source>
        <translation>Водитель:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6339"/>
        <source>Detected target:</source>
        <translation>Обнаруженная цель:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6340"/>
        <source>Model / label:</source>
        <translation>Модель / этикетка:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6341"/>
        <source>Status:</source>
        <translation>Статус:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6342"/>
        <source>Size:</source>
        <translation>Размер:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6343"/>
        <source>Connection:</source>
        <translation>Подключение:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6344"/>
        <source>Filesystem:</source>
        <translation>Файловая система:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6345"/>
        <source>UUID:</source>
        <translation>UUID:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6346"/>
        <source>Mounts:</source>
        <translation>Горы:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6347"/>
        <source>Protection:</source>
        <translation>Защита:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6389"/>
        <source>Diagnostics follow the Systems page: the selected repair drive while Host Maintenance is off, or the protected running host while Host Maintenance is active. Running Host diagnostics are available only inside the explicit Host Maintenance scope: enter Host Maintenance on the protected running-host card in Systems first. Both scopes provide read-only diagnostics; host EFI/UKI checks inspect the active ESP and firmware entries, while repair-drive checks mount only that system read-only.</source>
        <translation>Диагностика следует за страницей «Системы»: выбранный привод для ремонта во время обслуживания хоста отключен или защищенный работающий хост во время активного обслуживания хоста. Диагностика Running Host доступна только в явной области обслуживания хоста: сначала введите обслуживание хоста на защищенной карте хоста в системах. Обе области обеспечивают диагностику только для чтения; хост EFI / UKI проверяет активные записи ESP и прошивки, в то время как проверки на ремонт-драйв устанавливают только эту систему только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6398"/>
        <location filename="../src/MainWindow.cpp" line="6571"/>
        <location filename="../src/MainWindow.cpp" line="6852"/>
        <location filename="../src/MainWindow.cpp" line="8667"/>
        <location filename="../src/MainWindow.cpp" line="8746"/>
        <location filename="../src/MainWindow.cpp" line="11955"/>
        <source>Target: none selected</source>
        <translation>Цель: не выбрано</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6403"/>
        <location filename="../src/MainWindow.cpp" line="16103"/>
        <location filename="../src/MainWindow.cpp" line="16622"/>
        <source>Run All</source>
        <translation>Беги все</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6404"/>
        <source>Run All — run every available read-only diagnostic for the current scope.</source>
        <translation>Запустите все - запустите все доступные диагностические тесты только для чтения для текущего объема.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6422"/>
        <source>Edit Target File…</source>
        <translation>Обновление Target File…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6430"/>
        <source>Target configuration:</source>
        <translation>Конфигурация цели:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6441"/>
        <source>Diagnostic checks</source>
        <translation>Диагностические проверки</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6460"/>
        <source>Selected diagnostic</source>
        <translation>Выбранная диагностика</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6465"/>
        <location filename="../src/MainWindow.cpp" line="14220"/>
        <source>Select a diagnostic</source>
        <translation>Выберите диагностику</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6468"/>
        <location filename="../src/MainWindow.cpp" line="14221"/>
        <source>Choose a diagnostic from the list.</source>
        <translation>Выберите диагностику из списка.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6472"/>
        <location filename="../src/MainWindow.cpp" line="13558"/>
        <location filename="../src/MainWindow.cpp" line="21506"/>
        <source>Ready</source>
        <translation>Готовы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6485"/>
        <source>Results</source>
        <translation>Результаты</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6488"/>
        <location filename="../src/MainWindow.cpp" line="14223"/>
        <location filename="../src/MainWindow.cpp" line="14292"/>
        <location filename="../src/MainWindow.cpp" line="14301"/>
        <location filename="../src/MainWindow.cpp" line="14329"/>
        <source>Run Diagnostic</source>
        <translation>Бег Диагностика</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6496"/>
        <source>Diagnostic results appear here.</source>
        <translation>Результаты диагностики появляются здесь.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6508"/>
        <source>Copy Results</source>
        <translation>Копировать результаты</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6509"/>
        <source>Save Results…</source>
        <translation>Сохранить результаты …</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6569"/>
        <source>Run one repair tool or use the Full Repair plan against the selected repair drive. Choose Host Maintenance on Systems to run the same supported, guarded stages against the protected Running Host. Choose Full Repair stages in Settings. Enabled stages run in the order shown. Each configurable stage also appears below as an individual tool; the Full Repair column mirrors its current Settings state. The boot tools (EFI / UKI bootloader, GRUB or extlinux configuration, boot-stack reconciliation and Make Default) are independent: run them in any order, and a later action re-verifies what an earlier one changed and reports its own result. The active scope is shown beside Repair: selected repair drive or Running Host maintenance.</source>
        <translation>Запустите один ремонтный инструмент или используйте план полного ремонта против выбранного ремонтного привода. Выберите техническое обслуживание хоста в системах для запуска тех же поддерживаемых, охраняемых этапов против защищенного запускающего хоста. Выберите этапы полного ремонта в настройках. Включенные этапы выполняются в показанном порядке. Каждый настраиваемый этап также отображается ниже как отдельный инструмент; колонка «Полный ремонт» отражает его текущее состояние «Настройки». Инструменты загрузки (EFI / UKI загрузчик, GRUB или extlinux конфигурация, сверка загрузочного стека и по умолчанию) независимы: запустите их в любом порядке, а более позднее действие перепроверяет, что изменилось ранее, и сообщает о своем собственном результате. Активный диапазон показан рядом с ремонтом: выбранный ремонтный привод или техническое обслуживание Running Host.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6575"/>
        <location filename="../src/MainWindow.cpp" line="9153"/>
        <source>Full Repair plan</source>
        <translation>Полный план ремонта</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6587"/>
        <location filename="../src/MainWindow.cpp" line="19972"/>
        <source>No stages selected</source>
        <translation>Не выбраны этапы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6593"/>
        <source>Run All diagnostics for the selected target or running host before starting Full Repair. The report is read-only evidence used to choose and confirm repair stages.</source>
        <translation>Запустите всю диагностику выбранной цели или запуска хоста перед началом полного ремонта. Отчет представляет собой только показания, используемые для выбора и подтверждения этапов ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6606"/>
        <source>Configure Plan…</source>
        <translation>Настройка Plan…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6607"/>
        <source>Open Settings to choose which Full Repair stages are part of the plan.</source>
        <translation>Откройте настройки, чтобы выбрать, какие этапы полного ремонта являются частью плана.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6618"/>
        <location filename="../src/MainWindow.cpp" line="22205"/>
        <source>Run Full Repair</source>
        <translation>Запуск полного ремонта</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6666"/>
        <source>Individual repair tools</source>
        <translation>Индивидуальные ремонтные инструменты</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6672"/>
        <source>Tool</source>
        <translation>Инструмент</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6672"/>
        <location filename="../src/MainWindow.cpp" line="22167"/>
        <location filename="../src/MainWindow.cpp" line="22267"/>
        <location filename="../src/MainWindow.cpp" line="22281"/>
        <source>Full Repair</source>
        <translation>Полный ремонт</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6708"/>
        <location filename="../src/MainWindow.cpp" line="20439"/>
        <source>Validate environment</source>
        <translation>Проверить окружающую среду</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6709"/>
        <location filename="../src/MainWindow.cpp" line="20580"/>
        <location filename="../src/MainWindow.cpp" line="22647"/>
        <location filename="../src/MainWindow.cpp" line="22678"/>
        <source>File system repair</source>
        <translation>Ремонт файловой системы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6710"/>
        <location filename="../src/MainWindow.cpp" line="20447"/>
        <location filename="../src/MainWindow.cpp" line="21997"/>
        <source>Complete package configuration</source>
        <translation>Полная комплектация пакета</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6711"/>
        <location filename="../src/MainWindow.cpp" line="20454"/>
        <location filename="../src/MainWindow.cpp" line="22001"/>
        <source>Repair broken dependencies</source>
        <translation>Ремонт сломанных зависимостей</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6712"/>
        <location filename="../src/MainWindow.cpp" line="9160"/>
        <location filename="../src/MainWindow.cpp" line="19898"/>
        <location filename="../src/MainWindow.cpp" line="20465"/>
        <location filename="../src/MainWindow.cpp" line="21349"/>
        <location filename="../src/MainWindow.cpp" line="22005"/>
        <source>Refresh package metadata</source>
        <translation>Обновить метаданные пакета</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6713"/>
        <location filename="../src/MainWindow.cpp" line="19899"/>
        <location filename="../src/MainWindow.cpp" line="20476"/>
        <location filename="../src/MainWindow.cpp" line="22009"/>
        <source>Upgrade installed packages</source>
        <translation>Обновление установленных пакетов</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6714"/>
        <source>DKMS</source>
        <translation>DKMS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6717"/>
        <location filename="../src/MainWindow.cpp" line="20522"/>
        <source>Initramfs</source>
        <translation>Инитрамфы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6718"/>
        <location filename="../src/MainWindow.cpp" line="20534"/>
        <source>EFI / UKI bootloader</source>
        <translation>EFI/UKI загрузчик</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6720"/>
        <location filename="../src/MainWindow.cpp" line="20573"/>
        <source>extlinux configuration</source>
        <translation>Конфигурация extlinux</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6721"/>
        <location filename="../src/MainWindow.cpp" line="20589"/>
        <source>Boot stack reconciliation</source>
        <translation>Обсуждение Boot Stack</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6749"/>
        <source>Selected tool</source>
        <translation>Выбранный инструмент</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6764"/>
        <location filename="../src/MainWindow.cpp" line="20416"/>
        <source>Select a repair tool</source>
        <translation>Выберите инструмент для ремонта</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6767"/>
        <location filename="../src/MainWindow.cpp" line="20419"/>
        <source>Run Tool</source>
        <translation>Скачать Run Tool</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6773"/>
        <source>Select a tool to review its repair action.</source>
        <translation>Выберите инструмент для проверки его действия по ремонту.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6848"/>
        <source>Btrfs snapshots</source>
        <translation>Снимки Btrfs</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6850"/>
        <source>Inspect Btrfs root snapshots and perform a transactional rollback. Rollback keeps the source snapshot unchanged, preserves the current @ root, promotes a writable copy to @, rebuilds the boot stack and automatically restores the old root if post-switch validation fails. In Host Maintenance the same workflow targets the running host through Snapper @ snapshots and Boot Bitch @rollback-before-* undo points; the running host starts the promoted root only after a reboot.</source>
        <translation>Осмотрите снимки корня Btrfs и выполните транзакционный откат. Rollback сохраняет исходный снимок без изменений, сохраняет текущий @ root, продвигает написанную копию на @, восстанавливает стек загрузки и автоматически восстанавливает старый корень, если валидация после переключения не удается. В техническом обслуживании хоста тот же рабочий процесс нацелен на бегущего хоста через Snapper @ snapshots и Boot Bitch @rollback-before-* undo points; бегущий хост запускает продвигаемый корень только после перезагрузки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6871"/>
        <location filename="../src/MainWindow.cpp" line="12894"/>
        <location filename="../src/MainWindow.cpp" line="13043"/>
        <source>Reboot Now</source>
        <translation>Перезагрузить сейчас</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6873"/>
        <location filename="../src/MainWindow.cpp" line="12895"/>
        <source>Later</source>
        <translation>Позже</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6879"/>
        <source>Reboot reminder hidden; the running-host rollback stays staged until the host is rebooted.</source>
        <translation>Напоминание о перезагрузке скрыто; откат бегущего хозяина остается инсценированным, пока хозяин не перезагрузится.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6888"/>
        <source>Snapshot</source>
        <translation>Снимок</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6888"/>
        <source>Created</source>
        <translation>созданный</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6888"/>
        <source>Type</source>
        <translation>Тип</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6888"/>
        <source>Description</source>
        <translation>Описание</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6935"/>
        <source>Load snapshots to inspect recovery points. Snapshot discovery runs through the guarded read-only helper, so root-owned Snapper metadata does not need to be readable by the desktop user.</source>
        <translation>Снимки нагрузки для проверки точек восстановления. Открытие Snapshot проходит через защищенный помощник только для чтения, поэтому принадлежащие корневым пользователям метаданные Snapper не должны быть читаемыми пользователем рабочего стола.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6947"/>
        <source>Snapshot inventory</source>
        <translation>инвентарь снимков</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6959"/>
        <source>Load Snapshots</source>
        <translation>Загрузить Snapshots</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6960"/>
        <source>Inspect Selected</source>
        <translation>Проверка выбранная</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6961"/>
        <source>Roll Back to Selected</source>
        <translation>Обратно в избранное</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7030"/>
        <source>Boot %1</source>
        <translation>Загрузка %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7031"/>
        <location filename="../src/MainWindow.cpp" line="7139"/>
        <source>Label: %1</source>
        <translation>Метка: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7032"/>
        <source>Loader: %1</source>
        <translation>Погрузчик: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7033"/>
        <location filename="../src/MainWindow.cpp" line="7085"/>
        <source>Class: %1</source>
        <translation>Класс: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7034"/>
        <source>Role: %1</source>
        <translation>Роль: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7035"/>
        <source>PARTUUID: %1</source>
        <translation>PARTUUID: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7037"/>
        <source>Drive: %1</source>
        <translation>Диск: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7040"/>
        <source>Listed in BootOrder: yes</source>
        <translation>Запись в BootOrder: да</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7041"/>
        <source>Listed in BootOrder: no (omitted by firmware)</source>
        <translation>BootOrder: нет (опущено прошивкой)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7043"/>
        <source>This entry is currently active (BootCurrent).</source>
        <translation>В настоящее время эта запись активна (BootCurrent).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7055"/>
        <source>GRUB menu</source>
        <translation>Меню GRUB</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7058"/>
        <source>Direct (UKI)</source>
        <translation>Прямой (UKI)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7061"/>
        <source>Network</source>
        <translation>Сеть</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7064"/>
        <source>Fallback</source>
        <translation>Обсуждение Fallback</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7067"/>
        <source>Firmware</source>
        <translation>Прошивка</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7069"/>
        <source>Other</source>
        <translation>другой</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7084"/>
        <location filename="../src/MainWindow.cpp" line="7159"/>
        <source>Menu entry: %1</source>
        <translation>Меню: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7085"/>
        <source>(none)</source>
        <translation>(нет)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7087"/>
        <source>Menuentry id: %1</source>
        <translation>id меню: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7090"/>
        <source>Source: foreign (detected by os-prober)</source>
        <translation>Источник: Foreign (обнаружено os-prober)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7091"/>
        <source>Source: native</source>
        <translation>Источник: native</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7093"/>
        <source>This entry is the current GRUB default.</source>
        <translation>Эта запись является текущим по умолчанию GRUB.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7099"/>
        <source>Loads: %1</source>
        <translation>Нагрузки: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7116"/>
        <source>Firmware boot entries on this disk</source>
        <translation>Загрузка прошивки на этом диске</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7118"/>
        <source>Switch to the EFI/UKI entries view and load it to list this disk&apos;s firmware entries.</source>
        <translation>Переключитесь на запись EFI/UKI и загрузите ее, чтобы перечислить записи прошивки этого диска.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7126"/>
        <source>Boot%1 — %2 — %3 (%4)</source>
        <translation>Boot%1 — %2 — %3 (%4)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7222"/>
        <location filename="../src/MainWindow.cpp" line="8114"/>
        <source>No scope selected</source>
        <translation>Нет выбранной области</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7226"/>
        <source>EFI/UKI entries</source>
        <translation>Записи EFI/UKI</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7247"/>
        <source>Boot</source>
        <translation>Бут</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7247"/>
        <source>Label</source>
        <translation>этикетка</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7247"/>
        <source>Loader</source>
        <translation>погрузчик</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7247"/>
        <source>Boots</source>
        <translation>сапоги</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7247"/>
        <location filename="../src/MainWindow.cpp" line="7368"/>
        <source>Class</source>
        <translation>Класс</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7247"/>
        <source>Drive</source>
        <translation>Драйв</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7272"/>
        <source>What booting this firmware entry does.</source>
        <translation>Что делает загрузка этой записи прошивки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7285"/>
        <source>Select Host Maintenance on the protected running-host card, or a repair target in Systems, to manage firmware boot entries. Load Entries lists the current scope&apos;s UEFI entries read-only.</source>
        <translation>Выберите техническое обслуживание хоста на защищенной хост-карте или цель ремонта в системах для управления загрузками прошивки. Load Entries перечисляет записи UEFI текущего объема только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7292"/>
        <source>Firmware boot entries</source>
        <translation>Загрузка Firmware</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7302"/>
        <source>Load Entries</source>
        <translation>Загрузка Entries</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7303"/>
        <source>Move Up</source>
        <translation>двигаться вверх</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7304"/>
        <source>Move Down</source>
        <translation>Двигаться вниз</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7306"/>
        <source>Delete Selected</source>
        <translation>Удалить выбранный</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7307"/>
        <location filename="../src/MainWindow.cpp" line="8625"/>
        <source>EFI-/UKI Restore</source>
        <translation>Восстановление EFI/UKI</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7368"/>
        <source>Menu entry</source>
        <translation>Вход в меню</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7368"/>
        <source>Default</source>
        <translation>по умолчанию</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7415"/>
        <source>Load Menu</source>
        <translation>Меню нагрузки</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7530"/>
        <source>Select Host Maintenance on the running host, or a repair target in Systems, to manage firmware boot entries.</source>
        <translation>Выберите обслуживание хоста на запущенном хосте или цель ремонта в системах для управления загрузками прошивки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7568"/>
        <source>Loading firmware boot entries</source>
        <translation>Загрузка загрузочных записей прошивки</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7575"/>
        <source>List firmware boot entries</source>
        <translation>Список загрузок прошивки</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7590"/>
        <source>Firmware boot entries could not be loaded. See Logs for details.</source>
        <translation>Загрузка прошивки не может быть загружена. Смотрите журналы для деталей.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7621"/>
        <source>Loaded %1 firmware boot entries.</source>
        <translation>Загрузка прошивки %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7840"/>
        <source>Double-click to rename this boot entry. Blank labels are refused.</source>
        <translation>Дважды щелкните, чтобы переименовать эту загрузочную запись. В чистых этикетках отказывают.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8107"/>
        <source>Running host: %1</source>
        <translation>Ведущий: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8108"/>
        <source>Running host: not selected</source>
        <translation>Ведущий: не выбран</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8111"/>
        <source>Repair target: %1</source>
        <translation>Цель ремонта: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8112"/>
        <source>Repair target: not ready</source>
        <translation>Цель ремонта: не готова</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8131"/>
        <location filename="../src/MainWindow.cpp" line="8142"/>
        <source>Running-host</source>
        <translation>бегущий хозяин</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8131"/>
        <location filename="../src/MainWindow.cpp" line="8142"/>
        <source>Repair-target</source>
        <translation>Цель ремонта</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8141"/>
        <source>%1 firmware entry management is available. Load Entries to list the UEFI entries read-only.</source>
        <translation>Доступно управление входом прошивки %1. Загрузка Записи для перечисления записей UEFI только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8165"/>
        <source>Load the %1 UEFI firmware boot entries read-only.</source>
        <translation>Загрузите загрузочные записи прошивки %1 UEFI только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8166"/>
        <location filename="../src/MainWindow.cpp" line="8173"/>
        <source>running host&apos;s</source>
        <translation>бегущий хозяин</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8166"/>
        <location filename="../src/MainWindow.cpp" line="8173"/>
        <source>repair target&apos;s</source>
        <translation>Цель ремонта</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8180"/>
        <location filename="../src/MainWindow.cpp" line="8186"/>
        <location filename="../src/MainWindow.cpp" line="8200"/>
        <source>Select a boot entry row first.</source>
        <translation>Сначала выберите строку входа в багажник.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8436"/>
        <source>Boot entry delete unavailable</source>
        <translation>Boot entry скачать бесплатно</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8441"/>
        <source>Select a boot entry to delete first.</source>
        <translation>Сначала выберите загрузочную запись, чтобы удалить.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8454"/>
        <source>Boot%1 is the currently active firmware entry (BootCurrent) and cannot be deleted.</source>
        <translation>Boot%1 является активным входом прошивки (BootCurrent) и не может быть удален.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8458"/>
        <location filename="../src/MainWindow.cpp" line="8474"/>
        <source>Cannot delete boot entry</source>
        <translation>Невозможно удалить boot entry</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8468"/>
        <source>Boot%1 belongs to the running host&apos;s own firmware and cannot be deleted while a repair target is selected.</source>
        <translation>Boot%1 принадлежит к собственной прошивке хоста и не может быть удален при выборе цели ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8471"/>
        <location filename="../src/MainWindow.cpp" line="8533"/>
        <source>To manage the running host&apos;s own entries, switch to Host Maintenance first.</source>
        <translation>Чтобы управлять собственными записями бегущего хоста, сначала переключитесь на обслуживание хоста.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8657"/>
        <location filename="../src/MainWindow.cpp" line="13642"/>
        <location filename="../src/MainWindow.cpp" line="13842"/>
        <source>Chroot shell</source>
        <translation>Корневая раковина</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8664"/>
        <location filename="../src/MainWindow.cpp" line="13651"/>
        <source>Run a command inside the selected repair system as root (sudo is not needed). Commands are executed one at a time in a fresh chroot. When a command asks a question, Boot Bitch shows it in a popup and sends your answer back to the command; cancelling stops the command. Non-interactive flags such as dnf update -y or apt-get -y upgrade remain recommended for unattended runs. Output is kept in this window and in the application log.</source>
        <translation>Запустите команду внутри выбранной системы ремонта как root (судо не требуется). Команды выполняются по одной за раз в свежем корне. Когда команда задает вопрос, Boot Bitch показывает его всплывающим окном и отправляет ответ обратно в команду; отмена останавливает команду. Неинтерактивные флаги, такие как обновление dnf -y или обновление apt-get -y, по-прежнему рекомендуются для необслуживаемых запусков. Выход хранится в этом окне и в журнале приложений.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8673"/>
        <location filename="../src/MainWindow.cpp" line="13658"/>
        <source>Command, for example: dnf update -y or update-grub</source>
        <translation>Командная строка: dnf Update -y или update-grub</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8678"/>
        <location filename="../src/MainWindow.cpp" line="13664"/>
        <source>Run Command</source>
        <translation>Запускайте команду</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8680"/>
        <location filename="../src/MainWindow.cpp" line="13631"/>
        <source>Execute the command inside the selected repair system as root.</source>
        <translation>Выполните команду внутри выбранной системы ремонта как root.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8696"/>
        <source>Command output will appear here.</source>
        <translation>Командный выход появится здесь.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8703"/>
        <location filename="../src/MainWindow.cpp" line="13669"/>
        <source>Commands can modify the target system. Review each command before running it.</source>
        <translation>Команды могут изменять целевую систему. Проверяйте каждую команду перед запуском.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8706"/>
        <source>Clear Output</source>
        <translation>Чистый результат</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8740"/>
        <source>File copy</source>
        <translation>Копия файла</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8743"/>
        <source>Copy and verify files in either direction. Host to Repair remounts only the selected target filesystem read-write after safety checks; Repair to Host keeps the repair target read-only. Browse Target Folders reads the selected repair tree through temporary read-only mounts, while transfers use rsync without --delete, path containment, ownership validation and post-copy verification.</source>
        <translation>Копировать и проверять файлы в любом направлении. Host to Repair восстанавливает только выбранную целевую файловую систему read-write после проверки безопасности; Ремонт хоста сохраняет цель ремонта только для чтения. Browse Target Folders считывает выбранное дерево ремонта через временные крепления только для чтения, в то время как передачи используют rsync без удаления, сдерживания пути, проверки собственности и проверки после копирования.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8755"/>
        <source>Preview Changes</source>
        <translation>Превью Изменения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8756"/>
        <location filename="../src/MainWindow.cpp" line="14039"/>
        <source>Copy and Verify</source>
        <translation>Копировать и проверять</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8759"/>
        <source>Run an rsync dry-run through the guarded helper. No files are changed.</source>
        <translation>Пробежать рысью через охраняемого помощника. Файлы не меняются.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8773"/>
        <source>Direction:</source>
        <translation>Направление:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8775"/>
        <location filename="../src/MainWindow.cpp" line="13993"/>
        <source>Host → Repair</source>
        <translation>Ремонт →</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8776"/>
        <location filename="../src/MainWindow.cpp" line="13993"/>
        <source>Repair → Host</source>
        <translation>Ремонт → Host</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8777"/>
        <source>Choose which system supplies the source files and which system receives them.</source>
        <translation>Выберите, какая система поставляет исходные файлы и какая система их получает.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8802"/>
        <location filename="../src/MainWindow.cpp" line="13107"/>
        <source>Add Files…</source>
        <translation>Добавление файлов …</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8803"/>
        <location filename="../src/MainWindow.cpp" line="13108"/>
        <source>Add Folder…</source>
        <translation>Добавить Folder…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8804"/>
        <source>Remove</source>
        <translation>Удалить</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8805"/>
        <source>Remove the selected staged source entries from this list.</source>
        <translation>Удалите выбранные элементы из этого списка.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8806"/>
        <location filename="../src/MainWindow.cpp" line="13205"/>
        <location filename="../src/MainWindow.cpp" line="16922"/>
        <source>Clear</source>
        <translation>Чисто.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8807"/>
        <source>Clear every staged source entry from this list.</source>
        <translation>Очистите каждую постановочную запись источника из этого списка.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8830"/>
        <source>Choose Path…</source>
        <translation>Выберите Path…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8836"/>
        <source>3. Ownership and copy policy</source>
        <translation>3. Политика владения и копирования</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8839"/>
        <source>Ownership:</source>
        <translation>Право собственности:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8841"/>
        <source>Smart destination ownership (recommended)</source>
        <translation>Умное владение пунктом назначения (рекомендуется)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8842"/>
        <source>Preserve source numeric UID/GID</source>
        <translation>Сохранить исходный номер UID/GID</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8979"/>
        <source>Application log</source>
        <translation>Журнал приложений</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8981"/>
        <source>Shows this application&apos;s session activity. Session files are listed on the left; Save a copy when you need to share troubleshooting details.</source>
        <translation>Показывает сессионную активность этого приложения. Файлы сеанса перечислены слева; Сохраните копию, когда вам нужно поделиться деталями устранения неполадок.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8983"/>
        <source>Save As…</source>
        <translation>Сохранить As…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8984"/>
        <location filename="../src/MainWindow.cpp" line="16917"/>
        <source>Clear Register</source>
        <translation>Чистый регистр</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9003"/>
        <source>Session logs</source>
        <translation>Журналы сеансов</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9017"/>
        <source>New Session Log</source>
        <translation>Новый Session Log</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9018"/>
        <source>Delete</source>
        <translation>Исключить</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8355"/>
        <source>Delete firmware boot entry</source>
        <translation>Delete firmware boot entry скачать</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8519"/>
        <source>Boot entry rename unavailable</source>
        <translation>Boot entry переименовали в недоступную</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8530"/>
        <source>Boot%1 belongs to the running host&apos;s own firmware and cannot be renamed while a repair target is selected.</source>
        <translation>Boot%1 принадлежит к собственной прошивке хоста и не может быть переименован при выборе цели ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8542"/>
        <source>Cannot rename boot entry</source>
        <translation>Невозможно переименовать boot entry</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8378"/>
        <source>Rename firmware boot entry</source>
        <translation>Переименовать firmware boot entry</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8577"/>
        <source>Boot entry labels cannot be blank; the previous label was kept.</source>
        <translation>Загрузочные этикетки не могут быть пустыми; предыдущая этикетка была сохранена.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8578"/>
        <source>Boot entry label</source>
        <translation>Загрузочная этикетка</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8579"/>
        <source>A blank boot entry label is not allowed. The previous label was kept.</source>
        <translation>Не допускается пустая загрузочная этикетка. Предыдущий ярлык был сохранен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8587"/>
        <source>Boot%1 rename cancelled.</source>
        <translation>Переименование Boot%1 отменено.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8606"/>
        <source>Restore boot entries unavailable</source>
        <translation>Восстановить загрузочные записи недоступны</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8626"/>
        <source>Reinstall the EFI/UKI bootloader and re-create or re-label missing firmware entries</source>
        <translation>Переустановить загрузчик EFI/UKI и воссоздать или перемаркировать недостающие записи прошивки</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8635"/>
        <source>Restore / regenerate boot entries</source>
        <translation>Восстановление / регенерация загрузочных записей</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9020"/>
        <location filename="../src/MainWindow.cpp" line="13270"/>
        <source>Refresh</source>
        <translation>освежить</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9024"/>
        <location filename="../src/MainWindow.cpp" line="17864"/>
        <source>Add Note</source>
        <translation>Добавить комментарий</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9025"/>
        <source>Append a timestamped NOTE entry to the current session log.</source>
        <translation>Добавить в журнал текущей сессии запись ПРИМЕЧАНИЯ с меткой времени.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9040"/>
        <source>Search log:</source>
        <translation>Журнал поиска:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9046"/>
        <source>Filter application log (fuzzy match; use @section, e.g. @errors, for whole diagnostic sections)…</source>
        <translation>Журнал приложений фильтров (нечеткое совпадение; используйте @section, например @errors, для целых диагностических разделов) …</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9055"/>
        <source>All entries</source>
        <translation>Все записи</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9057"/>
        <source>Repairs</source>
        <translation>ремонт</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9143"/>
        <source>Device discovery</source>
        <translation>Открытие устройства</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9145"/>
        <source>Show devices without an identified Linux installation</source>
        <translation>Показать устройства без идентифицированной установки Linux</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9146"/>
        <source>Show removable and USB storage</source>
        <translation>Показать съемное и USB-хранилище</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9147"/>
        <source>Show encrypted devices before unlocking</source>
        <translation>Показать зашифрованные устройства перед разблокировкой</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9155"/>
        <source>Repair file system errors (read-only check first)</source>
        <translation>Исправление ошибок файловой системы (проверка только для чтения)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2072"/>
        <location filename="../src/MainWindow.cpp" line="9158"/>
        <location filename="../src/MainWindow.cpp" line="19896"/>
        <location filename="../src/MainWindow.cpp" line="22185"/>
        <source>Complete interrupted package configuration</source>
        <translation>Полная прерванная конфигурация пакета</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2113"/>
        <location filename="../src/MainWindow.cpp" line="9159"/>
        <location filename="../src/MainWindow.cpp" line="19897"/>
        <location filename="../src/MainWindow.cpp" line="21346"/>
        <source>Repair broken package dependencies</source>
        <translation>Ремонт сломанных зависимостей пакета</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9161"/>
        <location filename="../src/MainWindow.cpp" line="21352"/>
        <source>Upgrade installed packages (adaptive APT simulation)</source>
        <translation>Обновление установленных пакетов (адаптивное моделирование APT)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2089"/>
        <location filename="../src/MainWindow.cpp" line="9162"/>
        <location filename="../src/MainWindow.cpp" line="17223"/>
        <source>Rebuild DKMS modules</source>
        <translation>Восстановление модулей DKMS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9163"/>
        <location filename="../src/MainWindow.cpp" line="21360"/>
        <source>Restore detected graphical login manager and graphical.target</source>
        <translation>Восстановить обнаруженный графический менеджер входа и графический. цель</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9164"/>
        <location filename="../src/MainWindow.cpp" line="19902"/>
        <location filename="../src/MainWindow.cpp" line="21363"/>
        <source>Rebuild initramfs after mapper/crypttab validation</source>
        <translation>Восстановление initramfs после валидации mapper/crypttab</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9165"/>
        <location filename="../src/MainWindow.cpp" line="21340"/>
        <source>Repair EFI / UKI boot path (explicit target ESP repair)</source>
        <translation>Ремонт загрузочного пути EFI / UKI (явный целевой ремонт ESP)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9166"/>
        <location filename="../src/MainWindow.cpp" line="19904"/>
        <location filename="../src/MainWindow.cpp" line="21343"/>
        <source>Update GRUB configuration</source>
        <translation>Обновление конфигурации GRUB</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9167"/>
        <location filename="../src/MainWindow.cpp" line="19905"/>
        <location filename="../src/MainWindow.cpp" line="21366"/>
        <source>Update extlinux configuration</source>
        <translation>Обновление конфигурации extlinux</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9183"/>
        <source>Automatically regenerate read-only diagnostics after repairs or target changes</source>
        <translation>Автоматически регенерировать диагностику только для чтения после ремонта или изменения цели</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9189"/>
        <source>Mandatory safety controls</source>
        <translation>Обязательный контроль безопасности</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9192"/>
        <source>Protect every physical device backing /, /boot and /boot/efi</source>
        <translation>Защита каждого физического устройства /, /boot и /boot/efi</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9193"/>
        <source>Require explicit confirmation before package installation or repair actions</source>
        <translation>Требует явного подтверждения перед установкой упаковки или ремонтом.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9194"/>
        <source>Require mapper/crypttab consistency before initramfs rebuild</source>
        <translation>Требуется согласованность с картографом/криптабом перед восстановлением initramfs</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9195"/>
        <source>Preserve pre-rollback Btrfs root and auto-restore it on validation failure</source>
        <translation>Сохранить корень Btrfs и автоматически восстановить его при сбое проверки</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9196"/>
        <source>Never log LUKS passphrases or authentication secrets</source>
        <translation>Никогда не записывайте парольные фразы LUKS или секреты аутентификации.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9206"/>
        <source>Host capabilities and dependencies</source>
        <translation>Возможности и зависимости хоста</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9221"/>
        <source>Distribution:</source>
        <translation>Распределение:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9222"/>
        <source>Package manager family:</source>
        <translation>Семья менеджеров пакетов:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9223"/>
        <source>KAuth build support:</source>
        <translation>Поддержка KAuth build:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9228"/>
        <source>Feature</source>
        <translation>Особенность</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9228"/>
        <source>Command</source>
        <translation>Командование</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9228"/>
        <source>Scope</source>
        <translation>охват</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9229"/>
        <source>Suggested package</source>
        <translation>Предлагаемый пакет</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9229"/>
        <source>Notes</source>
        <translation>Заметки</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9278"/>
        <source>Refresh Capabilities</source>
        <translation>Обновить возможности</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9279"/>
        <source>Install Missing Support…</source>
        <translation>Недостающая поддержка …</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9506"/>
        <source>Scanning block devices…</source>
        <translation>Сканирующие блоки устройств …</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9517"/>
        <location filename="../src/MainWindow.cpp" line="9518"/>
        <source>Device scan failed</source>
        <translation>Сканирование устройства провалилось</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9550"/>
        <source>Device scan complete — discovery did not modify storage</source>
        <translation>Сканирование устройства завершено — обнаружение не изменило хранилище</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9605"/>
        <source>Showing %1 of %2 repair candidate disk(s); %3 running-system disk(s) protected</source>
        <translation>Показаны %1 диска (дисков)-кандидатов на ремонт %2; защищенный (защищенный) диск (диски) операционной системы %3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9675"/>
        <source>Running system protection unresolved</source>
        <translation>Защита операционной системы не решена</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9676"/>
        <source>No protected physical backing disk was identified</source>
        <translation>Защищенный физический бэк-диск не обнаружен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9686"/>
        <source>Current running Linux system</source>
        <translation>Текущая работающая система Linux</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9690"/>
        <source>Protected running-system storage</source>
        <translation>Защищенное хранилище операционной системы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9691"/>
        <source>Critical mounts: %1</source>
        <translation>Критические крепления: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9851"/>
        <source>Showing protected running-host details</source>
        <translation>Показ защищенных деталей бегущего хоста</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9908"/>
        <source>Host maintenance unavailable</source>
        <translation>Обслуживание хоста недоступно</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9922"/>
        <source>Exit Host Maintenance</source>
        <translation>Обслуживание Exit Host</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9971"/>
        <source>Host default unavailable</source>
        <translation>Host default недоступен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9977"/>
        <source>Make host the default boot entry</source>
        <translation>Сделайте хост по умолчанию boot entry</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9978"/>
        <source>Restore/ensure and select the running host&apos;s default boot entry?</source>
        <translation>Восстановить/убедиться и выбрать загрузочный вход запущенного хоста по умолчанию?</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9984"/>
        <location filename="../src/MainWindow.cpp" line="15263"/>
        <source>default boot entry</source>
        <translation>Загрузка по умолчанию</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9985"/>
        <location filename="../src/MainWindow.cpp" line="15264"/>
        <source>%1 default boot entry</source>
        <translation>Загрузка по умолчанию %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9987"/>
        <source>Host disk: %1
Root: %2

Boot Bitch will identify the host ESP, restore/ensure the verified %3, and place it first in BootOrder. The fallback/recovery route and every other firmware entry stay bootable and are never deleted.</source>
        <translation>Диск хоста: %1
Источник: %2

Boot Bitch идентифицирует хост ESP, восстанавливает/обеспечивает верифицированный %3 и помещает его первым в BootOrder. Маршрут резервного копирования / восстановления и каждая другая запись прошивки остаются загрузочными и никогда не удаляются.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9991"/>
        <source>Host disk: %1
Root: %2

Boot Bitch will identify the running host&apos;s boot configuration, restore/ensure the verified %3, and select it as the default. Every other boot entry stays bootable and is never deleted.</source>
        <translation>Диск хоста: %1
Источник: %2

Boot Bitch идентифицирует конфигурацию загрузки запущенного хоста, восстановит / обеспечит верифицированный %3 и выберет его по умолчанию. Каждая другая загрузочная запись остается загрузочной и никогда не удаляется.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10009"/>
        <source>Make host default boot entry</source>
        <translation>Загрузка host default boot entry</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10060"/>
        <source>Host default set to %1</source>
        <translation>Хостинг по умолчанию установлен на %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10063"/>
        <source>Host default verified</source>
        <translation>Проверенный дефолт хоста</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10064"/>
        <source>%1

Full helper output is available in Logs.</source>
        <translation>%1

Полный вспомогательный выход доступен в журналах.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10071"/>
        <source>Host default not verified: the helper did not name a verified boot entry.</source>
        <translation>Хост по умолчанию не проверен: помощник не назвал верифицированную загрузочную запись.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10072"/>
        <source>Host default not verified</source>
        <translation>Дефолт хоста не проверен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10074"/>
        <source>Host default operation failed: %1</source>
        <translation>Операция по умолчанию не удалась: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10077"/>
        <source>Host default operation failed</source>
        <translation>Операция по умолчанию Host не удалась</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10078"/>
        <source>The host default operation failed: %1

Full helper output is available in Logs.</source>
        <translation>Операция хоста по умолчанию не удалась: %1

Полный вспомогательный выход доступен в журналах.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10098"/>
        <source>Pending inspection</source>
        <translation>Предстоящая проверка</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10111"/>
        <source>PROTECTED — running system; read-only details only</source>
        <translation>Защищено — запущенная система; только детали для чтения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10113"/>
        <source>Live / installer media — not selectable</source>
        <translation>Live / Installer Media - не выбираемый</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10115"/>
        <source>Unlock required before selection</source>
        <translation>Unlock требуется перед выбором</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10116"/>
        <source>Eligible repair candidate</source>
        <translation>Кандидат на ремонт</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10130"/>
        <source>Commit this physical drive as the repair target.</source>
        <translation>Совершите этот физический драйв в качестве цели ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10133"/>
        <source>Live / installer media is read-only boot media and cannot be selected as a repair target.</source>
        <translation>Живые / установочные носители - это загрузочные носители только для чтения и не могут быть выбраны в качестве цели ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10135"/>
        <source>Unlock the encrypted volume first; Select Target becomes available after a Linux filesystem is detected.</source>
        <translation>Сначала разблокируйте зашифрованный том; после обнаружения файловой системы Linux становится доступен Select Target.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10136"/>
        <source>No selectable repair target was detected on this drive.</source>
        <translation>На этом приводе не было обнаружено какой-либо выборочной цели ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10137"/>
        <source>The protected running host cannot be selected as a repair target.</source>
        <translation>Защищенный работающий хост не может быть выбран в качестве цели ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10153"/>
        <source>Already unlocked before this Boot Bitch session. No unlock operation was performed here; the visible mapper will be reused and will not be closed by Boot Bitch.</source>
        <translation>Уже разблокирован перед сессией Boot Bitch. Здесь не проводилась операция разблокировки; видимый картограф будет повторно использоваться и не будет закрыт Boot Bitch.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10161"/>
        <source>Already Unlocked</source>
        <translation>Уже разблокирован</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10171"/>
        <source>No locked LUKS component is currently visible on this selected drive.</source>
        <translation>На этом выбранном диске в настоящее время не видно запертого компонента LUKS.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10173"/>
        <source>The protected running host cannot be unlocked or modified by Boot Bitch.</source>
        <translation>Защищенный хост не может быть разблокирован или изменен Boot Bitch.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10196"/>
        <location filename="../src/MainWindow.cpp" line="10232"/>
        <source>Target unavailable</source>
        <translation>Цель недоступна</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10197"/>
        <source>The selected drive is no longer available. Refresh devices and select it again.</source>
        <translation>Выбранный диск больше не доступен. Обновите устройства и выберите его снова.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10210"/>
        <source>Protected system</source>
        <translation>Защищенная система</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10211"/>
        <source>The running system cannot be selected as a repair target.</source>
        <translation>Система не может быть выбрана в качестве цели ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10220"/>
        <source>Live / installer media cannot be selected</source>
        <translation>Live / Installer Media не может быть выбран</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10221"/>
        <source>This drive carries read-only live/installer media. Insert or select a data drive instead; the boot media itself is never a repair target.</source>
        <translation>Этот диск несет только для чтения живые / инсталляторы. Вставьте или выберите диск данных; загрузочный носитель сам по себе никогда не является целью ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10227"/>
        <source>Unlock or select a Linux system first</source>
        <translation>Сначала разблокируйте или выберите систему Linux</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10228"/>
        <source>This encrypted drive has no visible Linux filesystem yet. Use Unlock, refresh devices, and select the target after its Linux root is detected.</source>
        <translation>Этот зашифрованный диск еще не имеет видимой файловой системы Linux. Используйте Unlock, обновите устройства и выберите цель после обнаружения корня Linux.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10233"/>
        <source>This drive has no resolvable target component. Refresh devices and select it again.</source>
        <translation>Этот диск не имеет разрешимого целевого компонента. Обновите устройства и выберите его снова.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10251"/>
        <source>Repair drive selected: %1</source>
        <translation>Выбранный привод для ремонта: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10402"/>
        <source>Requesting administrator authorization</source>
        <translation>Запросить разрешение администратора</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10441"/>
        <location filename="../src/MainWindow.cpp" line="10584"/>
        <source>Administrator authorization active for this Boot Bitch window</source>
        <translation>Активное разрешение администратора для этого окна Boot Bitch</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10791"/>
        <source>Requesting administrator authorization…</source>
        <translation>Запрос авторизации администратора …</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10804"/>
        <source>Administrator authorization deferred — diagnostics will regenerate after you authorize.</source>
        <translation>Разрешение администратора отложено — диагностика восстановится после вашего разрешения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10811"/>
        <source>Administrator authorization failed — press Authorize to retry.</source>
        <translation>Разрешение администратора не удалось — пресса Разрешить на повторную попытку.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11056"/>
        <location filename="../src/MainWindow.cpp" line="11099"/>
        <source>another privileged operation</source>
        <translation>Еще одна привилегированная операция</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11103"/>
        <source>superseded by the scope change to %1 and was not queued</source>
        <translation>Заменен изменением области охвата на %1 и не стоял в очереди</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11104"/>
        <source>refused because &apos;%1&apos; is still running and was not queued. Wait for it to finish and retry, or cancel it</source>
        <translation>Он отказался, потому что %1 все еще работает и не стоял в очереди. Подождите, пока он закончит и повторит, или отмените его.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11105"/>
        <source>Privileged request &apos;%1&apos; was %2.</source>
        <translation>Привилегированным запросом %1 был %2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11107"/>
        <source>ERROR: Privileged request &apos;%1&apos; was %2.
</source>
        <translation>Привилегированным запросом %1 был %2.
</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11191"/>
        <source>The administrator authorization session could not be established.</source>
        <translation>Сессия авторизации администратора не может быть создана.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11197"/>
        <source>Authorization unavailable</source>
        <translation>Авторизация недоступна</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11207"/>
        <source>The privileged helper session is not running; the plan stopped before its request started.</source>
        <translation>Сеанс привилегированного помощника не работает; план остановился до того, как его просьба началась.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11219"/>
        <source>Using the authorized Boot Bitch administrator session. The GUI itself is still running as your normal user.</source>
        <translation>Использование авторизованной сессии администратора Boot Bitch. Сам GUI по-прежнему работает как обычный пользователь.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11264"/>
        <source>Privileged request &apos;%1&apos; exceeded the %2 safety limit; aborting the request and closing the unresponsive helper session so the UI cannot stay busy indefinitely.</source>
        <translation>Привилегированный запрос «%1» превысил предел безопасности %2; отмена запроса и закрытие неотзывчивой сессии помощника, чтобы пользовательский интерфейс не мог оставаться занятым бесконечно.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11267"/>
        <source>The privileged operation exceeded the %1 safety limit and was aborted. Review the output before retrying.</source>
        <translation>Привилегированная операция превысила предел безопасности %1 и была прервана. Просмотрите выход перед повторным использованием.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11378"/>
        <source>Protocol note: a PROMPT record exceeded the 1024-byte payload bound and was dropped; the helper was answered with an empty ANSWER record.</source>
        <translation>Протокольное примечание: запись PROMPT превысила предел полезной нагрузки в 1024 байта и была сброшена; помощнику ответили пустой записью ANSWER.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11403"/>
        <source>ERROR: The privileged request exceeded the interactive prompt budget; treating the request as a protocol failure.</source>
        <translation>Ошибка: привилегированный запрос превысил интерактивный оперативный бюджет; рассматривая запрос как сбой протокола.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11426"/>
        <source>Shell command is asking for input</source>
        <translation>Команда Shell просит ввести</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11431"/>
        <source>The shell command is waiting for input. Review its output below, type the answer and press OK. Cancel — or an empty answer — stops the command.</source>
        <translation>Командование снаряда ждет ввода. Просмотрите его вывод ниже, введите ответ и нажмите OK. Отмена — или пустой ответ — останавливает команду.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11446"/>
        <source>Answer (for example: y, n, Y, I, N, Z, or a word)</source>
        <translation>Ответ (например: y, n, Y, I, N, Z или слово)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11451"/>
        <source>Send Answer</source>
        <translation>Отправить ответ</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11518"/>
        <source>Privileged operation completed successfully. Administrator authorization remains active for this Boot Bitch window.</source>
        <translation>Привилегированная операция успешно завершена. Для этого окна Boot Bitch остается активным разрешение администратора.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11519"/>
        <source>Privileged operation stopped with an error. Administrator authorization remains active; review the output before closing.</source>
        <translation>Привилегированная операция прекратилась с ошибкой. Разрешение администратора остается активным; просмотрите результаты до закрытия.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11543"/>
        <source>The privileged helper session ended unexpectedly. The next root action will require authorization again.</source>
        <translation>Сеанс привилегированного помощника закончился неожиданно. Следующее действие корня потребует повторного авторизации.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11642"/>
        <source>ERROR: Privileged request &apos;%1&apos; exceeded the %2 safety limit; the request was aborted and the unresponsive helper session was closed. The operation may not have completed.</source>
        <translation>Привилегированный запрос %1 превысил предел безопасности %2; запрос был прерван, а неотзывчивый сеанс помощника был закрыт. Операция может не завершиться.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11732"/>
        <source>Unlock not required</source>
        <translation>Unlock не требуется</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11734"/>
        <source>This drive already has an unlocked Linux filesystem. Boot Bitch will reuse the existing mapper.</source>
        <translation>Этот диск уже имеет разблокированную файловую систему Linux. Boot Bitch будет использовать существующий картограф.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11735"/>
        <source>The selected drive does not currently contain a locked LUKS component that needs to be opened.</source>
        <translation>Выбранный диск в настоящее время не содержит запертого компонента LUKS, который необходимо открыть.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11742"/>
        <location filename="../src/MainWindow.cpp" line="11758"/>
        <source>Unlock unavailable</source>
        <translation>Unlock Недоступно</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11743"/>
        <location filename="../src/MainWindow.cpp" line="21495"/>
        <location filename="../src/MainWindow.cpp" line="21532"/>
        <source>The privileged Boot Bitch helper was not found. Rebuild or install this source tree.</source>
        <translation>Привилегированный помощник Boot Bitch не найден. Восстановление или установка этого дерева.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11750"/>
        <source>Unlocking %1</source>
        <translation>Разблокировка %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11766"/>
        <source>Unlock LUKS repair target</source>
        <translation>Разблокировать цель ремонта LUKS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11794"/>
        <source>LUKS volume:</source>
        <translation>Объем LUKS:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11814"/>
        <source>Enter the passphrase to unlock this volume.</source>
        <translation>Введите парольную фразу, чтобы разблокировать этот том.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11821"/>
        <source>LUKS passphrase</source>
        <translation>Фраза LUKS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11826"/>
        <source>Administrator authorization is already active. The passphrase is sent only to cryptsetup over the privileged helper pipe and is never logged or placed on a command line.</source>
        <translation>Разрешение администратора уже действует. Пассфраза отправляется только для криптозащиты по привилегированной вспомогательной трубе и никогда не регистрируется или не размещается в командной строке.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11856"/>
        <source>Passphrase required</source>
        <translation>Требуется пароль</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11857"/>
        <source>An empty passphrase was not submitted. Enter the LUKS passphrase or choose Cancel.</source>
        <translation>Пустая фраза не была представлена. Введите парольную фразу LUKS или выберите Отменить.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11869"/>
        <source>Unlock LUKS target</source>
        <translation>Разблокировать цель LUKS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11901"/>
        <source>Passphrase not accepted</source>
        <translation>Перефразирование не принято</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11902"/>
        <source>The LUKS passphrase was not accepted.</source>
        <translation>LUKS не был принят.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11903"/>
        <source>Try again? Administrator authorization remains active, so only the disk passphrase will be requested again.</source>
        <translation>Попробуй еще раз? Авторизация администратора остается активной, поэтому будет запрошена только парольная фраза диска.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11944"/>
        <source>Encrypted target unlocked — select or reselect the repair target</source>
        <translation>Зашифрованная цель разблокирована — выберите или перевыберите цель ремонта</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11959"/>
        <location filename="../src/MainWindow.cpp" line="12007"/>
        <source>Host maintenance: %1</source>
        <translation>Обслуживание хоста: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11964"/>
        <source>Target: %1</source>
        <translation>Цель: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11965"/>
        <source>Selected physical repair drive: %1</source>
        <translation>Выбранный физический привод для ремонта: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11984"/>
        <source>Running Host: %1</source>
        <translation>Разработчик: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12010"/>
        <source>Committed target: %1</source>
        <translation>Цель: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12049"/>
        <source>✓ SELECTED TARGET  •  %1</source>
        <translation>✓ SELECTED TARGET • %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12106"/>
        <source>The running host root is not Btrfs, so Btrfs snapshots are not available.</source>
        <translation>Корень хоста не является Btrfs, поэтому снимки Btrfs недоступны.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12107"/>
        <source>The selected Linux root is not Btrfs, so Btrfs snapshots are not available.</source>
        <translation>Выбранный корень Linux не является Btrfs, поэтому снимки Btrfs недоступны.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6967"/>
        <location filename="../src/MainWindow.cpp" line="12121"/>
        <source>Inspect the selected snapshot read-only.</source>
        <translation>Проверьте выбранный снимок только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7280"/>
        <source>Select an entry to enable actions; double-click Label to rename.</source>
        <translation>Выберите запись для включения действий; дважды щелкните Label для переименования.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7315"/>
        <location filename="../src/MainWindow.cpp" line="8214"/>
        <source>Re-create or re-label missing UKI/fallback/WebFAI/iPXE entries.</source>
        <translation>Восстановление или повторное обозначение отсутствующих записей UKI/fallback/WebFAI/iPXE.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7395"/>
        <source>Generated boot menu in file order; read-only.</source>
        <translation>Сгенерированное меню загрузки в порядке файла; только чтение.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8191"/>
        <source>Apply pending reorder, delete and rename changes.</source>
        <translation>Применить ожидающий переупорядочения, удалить и переименовать изменения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8199"/>
        <source>Mark the selected entry for deletion; Save applies it.</source>
        <translation>Отметьте выбранную запись для удаления; Save применяет ее.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12122"/>
        <source>Select a snapshot row first.</source>
        <translation>Сначала выберите строку снимка.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12131"/>
        <location filename="../src/MainWindow.cpp" line="12142"/>
        <source>Select a valid Linux root snapshot first.</source>
        <translation>Сначала выберите действительный снимок корня Linux.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12219"/>
        <source>Btrfs snapshot preload for %1 is deferred until the running privileged operation finishes; the request was not queued.</source>
        <translation>Предварительная загрузка снимка Btrfs для %1 откладывается до окончания выполнения привилегированной операции; запрос не стоял в очереди.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12234"/>
        <source>Loading Btrfs snapshots in the background…</source>
        <translation>Загрузка снимков Btrfs в фоновом режиме …</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12291"/>
        <source>Btrfs snapshot inventory is not applicable: the running host root filesystem is %1, not Btrfs.

No snapshot was loaded and the running host was not modified.</source>
        <translation>Кадастр снимков Btrfs неприменим: запущенная корневая файловая система хоста - это %1, а не Btrfs.

Ни один снимок не был загружен, а бегущий хост не был изменен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12292"/>
        <source>Btrfs snapshot inventory is not applicable: the selected target filesystem is %1, not Btrfs.

No snapshot was loaded and the target was not modified.</source>
        <translation>Опись Btrfs не применима: выбранная целевая файловая система - %1, а не Btrfs.

Ни один снимок не был загружен, и цель не была изменена.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12306"/>
        <source>Snapshot inventory unavailable</source>
        <translation>Инвентарь снимков недоступен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12335"/>
        <source>Btrfs snapshot inventory for %1 is deferred until the running privileged operation finishes; the request was not queued.</source>
        <translation>Опись моментального снимка Btrfs для %1 откладывается до окончания привилегированной операции; запрос не стоял в очереди.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12341"/>
        <source>Loading Btrfs snapshots after the running operation finishes…</source>
        <translation>Загрузка снимков Btrfs после завершения работы …</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12347"/>
        <source>Loading running-host Btrfs snapshots</source>
        <translation>Загрузка бегущего хоста Btrfs</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12348"/>
        <source>Loading Btrfs snapshots</source>
        <translation>Загрузка снимков Btrfs</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12363"/>
        <source>Load running-host Btrfs snapshots</source>
        <translation>Загрузка бегущего хоста Btrfs</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12364"/>
        <source>Load Btrfs snapshots</source>
        <translation>Скачать Btrfs snapshots</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12398"/>
        <source>Snapshot inventory failed. See Logs for details.</source>
        <translation>Снимки провалились. Смотрите журналы для деталей.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12451"/>
        <source>Loaded %1 running-host Btrfs root snapshot(s) read-only at %2. Select a row and choose Inspect Selected, or double-click a row, for snapshot-specific validation.

No snapshot or host file was modified.</source>
        <translation>Загруженный %1-хост Btrfs root snapshot(s) только для чтения на %2. Выберите строку и выберите Inspect Selected или дважды щелкните строку для валидации для конкретного снимка.

Ни один снимок или хост-файл не был изменен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12453"/>
        <source>Loaded %1 Btrfs root snapshot(s) read-only at %2. Select a row and choose Inspect Selected, or double-click a row, for snapshot-specific validation.

No snapshot or target file was modified.</source>
        <translation>Загруженный %1 Btrfs root snapshot(s) только для чтения на %2. Выберите строку и выберите Inspect Selected или дважды щелкните строку для валидации для конкретного снимка.

Ни один снимок или целевой файл не был изменен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12457"/>
        <source>No Snapper-style Btrfs root snapshots or Boot Bitch rollback backups were found on the running host. The scan was read-only.</source>
        <translation>На бегущем хосте не было обнаружено снимков корня Btrfs в стиле Snapper или резервных копий Boot Bitch. Сканирование было только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12458"/>
        <source>No Snapper-style Btrfs root snapshots were found on the selected target. The scan was read-only.</source>
        <translation>На выбранной цели не было обнаружено снимков корня Btrfs в стиле Snapper. Сканирование было только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12568"/>
        <source>Inspecting running-host snapshot %1</source>
        <translation>Снимок бегущего хоста %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12569"/>
        <source>Inspecting snapshot %1</source>
        <translation>Скриншоты %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12577"/>
        <source>Inspect running-host Btrfs snapshot %1</source>
        <translation>Осмотр бегущего хоста Btrfs snapshot %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12578"/>
        <source>Inspect Btrfs snapshot %1</source>
        <translation>Проверить снимок Btrfs %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12587"/>
        <source>Captured: %1

%2</source>
        <translation>Производитель: %1

%2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12615"/>
        <source>Rolling back snapshot %1</source>
        <translation>Откат назад снимок %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12621"/>
        <source>Preflight snapshot rollback %1</source>
        <translation>Скриншоты из игры Preflight Rollback %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12630"/>
        <source>Rollback preflight captured: %1

%2</source>
        <translation>Захваченный обратный рейс: %1

%2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12634"/>
        <source>Rollback preflight failed</source>
        <translation>Rollback Preflight провалился</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12635"/>
        <source>The selected snapshot did not pass the transactional rollback preflight. No rollback was performed.

Review the Snapshots details pane and Logs.</source>
        <translation>Выбранный снимок не прошел предполетный транзакционный откат. Откат не производился.

Обзор панели деталей Snapshots и журналов.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12641"/>
        <source>Confirm transactional snapshot rollback</source>
        <translation>Подтвердить транзакционный моментальный откат</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12642"/>
        <source>Promote snapshot %1 to the normal writable @ root?</source>
        <translation>Продвигать снимок %1 до нормального записного @ root?</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12643"/>
        <source>Target disk: %1
Linux filesystem: %2

Boot Bitch will keep the source snapshot unchanged, preserve the current @ under a timestamped rollback backup, set the promoted copy as the Btrfs default, then rebuild/verify initramfs and the detected bootloader path (UKI, GRUB EFI, extlinux or Fedora BLS) and its configuration.

If a critical post-switch validation or boot-stack stage fails, Boot Bitch will automatically restore the preserved @ and reconcile its boot stack.

Separate Btrfs subvolumes such as /home remain outside the root rollback according to the target fstab.</source>
        <translation>Целевой диск: %1
Файловая система Linux: %2

Boot Bitch сохранит исходный снимок без изменений, сохранит текущий @ в резервной копии с временными метками, установит раскрученную копию по умолчанию Btrfs, затем восстановит / проверит initramfs и обнаруженный путь загрузчика (UKI, GRUB EFI, extlinux или Fedora BLS) и его конфигурацию.

Если критическая валидация после переключателя или стадия загрузки не сработает, Boot Bitch автоматически восстановит сохраненный @ и согласует свой стек загрузки.

Отдельные объемы Btrfs, такие как /home, остаются за пределами отката корня в соответствии с целевым fstab.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12651"/>
        <location filename="../src/MainWindow.cpp" line="12834"/>
        <source>Continue to Confirmation</source>
        <translation>Продолжайте подтверждать</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12661"/>
        <location filename="../src/MainWindow.cpp" line="12844"/>
        <source>Type ROLLBACK to continue</source>
        <translation>ROLLBACK продолжится</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12662"/>
        <source>This operation changes the active Btrfs root and rebuilds boot artifacts.

Type ROLLBACK exactly to continue:</source>
        <translation>Эта операция изменяет активный корень Btrfs и восстанавливает загрузочные артефакты.

Введите ROLLBACK, чтобы продолжить:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12675"/>
        <source>Roll back to Btrfs snapshot %1</source>
        <translation>Скачать Btrfs snapshot %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12684"/>
        <source>Rollback finished: %1

%2</source>
        <translation>Скриншоты из игры %1

%2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12692"/>
        <source>Rollback completed: %1

%2

Snapshot inventory was cleared because the active root changed. Choose Load Snapshots to refresh it.</source>
        <translation>Завершено обновление: %1

%2

Опись снимков была очищена, потому что активный корень изменился. Выберите Load Snapshots, чтобы обновить его.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12700"/>
        <source>Snapshot rollback complete</source>
        <translation>Скриншоты из игры Snapshot Rollback Complete</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12701"/>
        <source>Snapshot %1 was promoted to a writable @ root and the boot stack passed reconciliation.

The previous @ was retained under a timestamped @rollback-before-* name. Reboot using the normal boot path when ready.</source>
        <translation>Snapshot %1 был повышен до написанного @ root, и стек загрузки прошел согласование.

Предыдущий @ был сохранен под меткой @rollback-before-*. Перезагрузка с использованием обычного пути загрузки, когда он готов.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12710"/>
        <source>Rollback attempt finished: %1

%2

Cached diagnostics and snapshot inventory were cleared because the rollback request may have changed and/or restored target state.</source>
        <translation>Попытка отката завершена: %1

%2

Кашедная диагностика и инвентаризация снимков были очищены, потому что запрос на откат мог изменить и / или восстановить целевое состояние.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12718"/>
        <source>Snapshot rollback failed</source>
        <translation>Откат снимка провалился</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12719"/>
        <source>The rollback did not complete successfully. Do not reboot until you review the Snapshots output and Logs. The helper attempts to restore the preserved @ automatically when post-switch validation fails.</source>
        <translation>Откат не завершился успешно. Не перезагружайте, пока не просмотрите выход Snapshots и журналы. Помощник пытается восстановить сохраненный @ автоматически, когда валидация после переключения не удается.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12735"/>
        <source>Running-host snapshot rollback is only available in Host Maintenance.</source>
        <translation>Running-host snapshot rollback доступен только в Host Maintenance.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12752"/>
        <location filename="../src/MainWindow.cpp" line="12761"/>
        <source>Running-host snapshot rollback is unavailable.</source>
        <translation>Running-host snapshot rollback недоступен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12755"/>
        <source>Running-host snapshot rollback has unknown capability evidence.</source>
        <translation>Откат снимков бегущего хозяина имеет неизвестные доказательства способности.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12762"/>
        <source>Run running-host diagnostics in Host Maintenance first.</source>
        <translation>Сначала запустите диагностику бегущего хоста в обслуживании хоста.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12765"/>
        <source>Running-host snapshot rollback is available.</source>
        <translation>Running-host snapshot rollback доступен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12779"/>
        <source>Rollback already staged</source>
        <translation>Роллбэк уже поставлен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12780"/>
        <source>A running-host rollback to %1 is already staged and takes effect on the next reboot.</source>
        <translation>Откат бегущего хоста к %1 уже инсценирован и вступает в силу при следующей перезагрузке.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12782"/>
        <source>an earlier snapshot</source>
        <translation>Более ранний снимок</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12783"/>
        <source>snapshot %1</source>
        <translation>Фотография %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12784"/>
        <source>Rolling back again preserves the currently running root as a new @rollback-before-* backup and replaces the staged snapshot. The earlier staged root remains on disk but is no longer the recorded undo point.

Continue only if you intend to replace the staged rollback.</source>
        <translation>Откат снова сохраняет текущий корень в качестве новой резервной копии @rollback-before-* и заменяет постановочный снимок. Ранний постановочный корень остается на диске, но больше не является записанной точкой отмены.

Продолжайте только в том случае, если вы намерены заменить постановочный откат.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12789"/>
        <source>Replace Staged Rollback</source>
        <translation>Заменить Staged Rollback</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12797"/>
        <source>Rolling back running-host snapshot %1</source>
        <translation>Откат назад бегущий хост снимок %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12803"/>
        <source>Preflight running-host rollback %1</source>
        <translation>Откат бегущего хозяина %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12812"/>
        <source>Running-host rollback preflight captured: %1

%2</source>
        <translation>Откат бегущего хозяина захвачен: %1

%2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12816"/>
        <source>Host rollback preflight failed</source>
        <translation>Host Rollback Preflight провалился</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12817"/>
        <source>The selected snapshot did not pass the running-host rollback preflight. No rollback was performed.

Review the Snapshots details pane and Logs.</source>
        <translation>Выбранный снимок не прошел предполетный откат бегущего хозяина. Откат не производился.

Обзор панели деталей Snapshots и журналов.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12823"/>
        <source>Confirm running-host snapshot rollback</source>
        <translation>Подтвердить откат бегущего хозяина</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12824"/>
        <source>Roll the running host back to snapshot %1?</source>
        <translation>Переверните бегущий хост обратно на снимок %1?</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12825"/>
        <source>Running host: %1
Root filesystem: %2

The running host keeps running the current root until you reboot. On the next reboot it will start the selected snapshot instead.

Snapper/Boot Bitch take a snapshot of the current system first, so the present state is preserved automatically as an @rollback-before-* undo point.

Boot Bitch cannot guarantee that no data is lost: changes made after the selected snapshot are not part of the rolled-back root, and separate subvolumes such as /home are not rolled back.

All users are signed out and unsaved work is lost when the host reboots. The reboot is never automatic.</source>
        <translation>Ведущий: %1
Корневая файловая система: %2

Бегущий хост продолжает запускать текущий корень, пока вы не перезагрузитесь. На следующей перезагрузке он начнет выбранный снимок.

Snapper/Boot Bitch сначала делает снимок текущей системы, поэтому настоящее состояние автоматически сохраняется в виде точки отмены @rollback-before-*.

Boot Bitch не может гарантировать, что никакие данные не будут потеряны: изменения, внесенные после выбранного снимка, не являются частью корня с откатанной спинкой, а отдельные объемы, такие как /home, не откатываются назад.

Все пользователи подписываются и несохраненная работа теряется при перезагрузке хоста. Перезагрузка никогда не бывает автоматической.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12845"/>
        <source>This operation stages the selected snapshot as the running host&apos;s next root and rebuilds boot artifacts.

Type ROLLBACK exactly to continue:</source>
        <translation>Эта операция делает выбранный снимок следующим корнем бегущего хоста и восстанавливает загрузочные артефакты.

Введите ROLLBACK, чтобы продолжить:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12858"/>
        <source>Roll back the running host to snapshot %1</source>
        <translation>Переверните бегущий хост на снимок %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12867"/>
        <source>Running-host rollback finished: %1

%2</source>
        <translation>Откат бегунов закончился: %1

%2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12879"/>
        <source>Running-host rollback staged: %1

%2

Snapshot inventory and cached running-host diagnostics were cleared because the next boot will start the promoted root. Reboot when ready; the reboot is never automatic.</source>
        <translation>Откат бегущего хозяина: %1

%2

Опись снимков и кэшированная диагностика бегущего хоста были очищены, потому что следующий ботинок запустит продвигаемый корень. Перезагрузка, когда она готова; перезагрузка никогда не бывает автоматической.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12889"/>
        <source>Running-host rollback staged</source>
        <translation>Бегущий хозяин Rollback постановочный</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12890"/>
        <source>Snapshot %1 is staged as the running host&apos;s next root.</source>
        <translation>Snapshot %1 является следующим корнем бегущего хоста.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12891"/>
        <source>Reboot required — the running host will start snapshot %1 after the next reboot. The previous root was retained as an @rollback-before-* undo point and the boot stack passed reconciliation.

Use Reboot Now to reboot immediately, or Later to keep working and reboot manually.</source>
        <translation>Требуется перезагрузка — запущенный хост запустит снимок %1 после следующей перезагрузки. Предыдущий корень был сохранен в виде точки отмены @rollback-before-*, а стек загрузки прошел согласование.

Используйте Reboot Now для немедленной перезагрузки или Later для продолжения работы и перезагрузки вручную.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12914"/>
        <source>Running-host rollback attempt finished: %1

%2

Cached running-host diagnostics and snapshot inventory were cleared. Do not reboot until you review the Snapshots output and Logs.</source>
        <translation>Попытка отката бегущего хозяина завершена: %1

%2

Кэшированная диагностика бегущего хозяина и инвентаризация снимков были очищены. Не перезагружайте, пока не просмотрите выход Snapshots и журналы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12921"/>
        <location filename="../src/MainWindow.cpp" line="12924"/>
        <source>Host rollback failed</source>
        <translation>Host Rollback провалился</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12922"/>
        <source>The running-host rollback did not complete and automatic recovery could not be proven. DO NOT REBOOT. Review the Snapshots output and Logs, and repair the boot stack from another system before rebooting.

The helper output carries HOST_ROLLBACK_RECOVERY and any CRITICAL lines verbatim.</source>
        <translation>Откат бегущего хоста не был завершен и автоматическое восстановление не могло быть доказано. Не перечитывайте. Просмотрите выход Snapshots и журналы и отремонтируйте стек загрузки из другой системы перед перезагрузкой.

Помощник выводит HOST ROLLBACK RECOVERY и любые критические линии дословно.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12925"/>
        <source>The running-host rollback did not complete. The helper reports the preserved @ was restored automatically; do not reboot until you review the Snapshots output and Logs.</source>
        <translation>Откат бегущего хозяина не завершился. Помощник сообщает, что сохраненный @ был восстановлен автоматически; не перезагружайте, пока вы не просмотрите выход Snapshots и журналы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13036"/>
        <location filename="../src/MainWindow.cpp" line="13037"/>
        <source>Reboot the running host now?</source>
        <translation>Перезагрузить бегущего хозяина?</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13038"/>
        <source>The rolled-back snapshot is already staged and takes effect on the next boot. Rebooting now signs out all users and closes unsaved work.

The host reboots only after this separate confirmation.</source>
        <translation>Снимок отката уже поставлен и вступает в силу на следующем ботинке. Перезагрузка теперь выявляет всех пользователей и закрывает несохраненную работу.

Хозяин перезагружается только после этого отдельного подтверждения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13050"/>
        <source>Rebooting running host</source>
        <translation>Перезагрузка бегущего хоста</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13055"/>
        <source>Reboot the running host</source>
        <translation>Перезагрузить бегущий хост</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13063"/>
        <source>Running-host reboot scheduled; the host is restarting.</source>
        <translation>Запуск перезагрузки хоста запланирован; хост перезапускается.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13070"/>
        <source>Host reboot not scheduled</source>
        <translation>Перезагрузка хоста не запланирована</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13072"/>
        <source>The running host did not schedule a reboot. The staged rollback and its reminder remain in effect.</source>
        <translation>Ведущий не планировал перезагрузку. Постановочный откат и его напоминание остаются в силе.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13090"/>
        <source>Repair → Host file copy</source>
        <translation>Ремонт → Копия файла Host</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13091"/>
        <source>Host → Repair file copy</source>
        <translation>→ Восстановить копию файла</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13095"/>
        <source>1. Select source paths from repaired system</source>
        <translation>1.Выберите пути источника из отремонтированной системы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13096"/>
        <source>2. Choose destination on this host</source>
        <translation>2.Выберите пункт назначения на этом хосте</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13097"/>
        <source>Add File Path…</source>
        <translation>Добавить файл Path…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13098"/>
        <source>Add Folder Path…</source>
        <translation>Добавить папку Path…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13100"/>
        <source>Enter an absolute folder path as it appears inside the repaired system.</source>
        <translation>Введите абсолютный путь папки, как он появляется в восстановленной системе.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13101"/>
        <source>Choose a host destination folder</source>
        <translation>Выберите папку host destination</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13102"/>
        <source>Browse…</source>
        <translation>Browse…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13105"/>
        <source>1. Select source files or folders from this host</source>
        <translation>1.Выберите исходные файлы или папки этого хоста</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13106"/>
        <source>2. Choose destination in repaired system</source>
        <translation>2.Выберите пункт назначения в отремонтированной системе</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13109"/>
        <source>Choose one or more source files from the running host.</source>
        <translation>Выберите один или несколько исходных файлов из запущенного хоста.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13110"/>
        <source>Choose a source folder from the running host.</source>
        <translation>Выберите исходную папку из запущенного хоста.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13112"/>
        <source>Select a repair target first</source>
        <translation>Сначала выберите цель ремонта</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13113"/>
        <source>Choose an absolute destination path inside the repaired system</source>
        <translation>Выберите абсолютный путь назначения внутри отремонтированной системы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13114"/>
        <source>Browse Target Folders…</source>
        <translation>Обсуждение Target Folders…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13151"/>
        <source>Choose host destination folder</source>
        <translation>Выберите папку host destination</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13164"/>
        <source>Select a repair target</source>
        <translation>Выберите цель ремонта</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13165"/>
        <source>Select the repair drive in Systems before choosing a destination inside it.</source>
        <translation>Выберите ремонтный привод в системах, прежде чем выбрать пункт назначения внутри него.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13175"/>
        <source>Select repair-system destination</source>
        <translation>Выберите пункт назначения ремонтной системы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13182"/>
        <source>Choose a destination folder inside the repaired system</source>
        <translation>Выберите папку назначения внутри восстановленной системы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13191"/>
        <source>Browse folders from the selected repaired system through temporary read-only mounts. Nothing is written while browsing, and the guarded copy validates the chosen path again before any write.</source>
        <translation>Просмотрите папки из выбранной отремонтированной системы через временные крепления только для чтения. Ничего не написано во время просмотра, и защищенная копия подтверждает выбранный путь еще раз перед любой записью.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13200"/>
        <source>Absolute path inside repaired system (for example /home/user/Recovered)</source>
        <translation>Абсолютный путь внутри отремонтированной системы (например, /home/user/Recovered)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13202"/>
        <source>Browse repair folders…</source>
        <translation>Ремонтные папки …</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13206"/>
        <source>Clear the destination path.</source>
        <translation>Очистите путь назначения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13221"/>
        <location filename="../src/MainWindow.cpp" line="13307"/>
        <source>Browse repaired-system folders</source>
        <translation>Просмотр исправленных системных папок</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13234"/>
        <source>Choose a folder from the repaired system</source>
        <translation>Выберите папку из отремонтированной системы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13243"/>
        <source>The selected repair filesystem is mounted read-only only for the current directory listing. The mount is removed after the request; the host filesystem is never used as the folder tree.</source>
        <translation>Выбранная файловая система восстановления монтируется только для чтения только для текущего списка каталогов. Монтаж удаляется после запроса; файловая система хоста никогда не используется в качестве дерева папок.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13268"/>
        <source>Up</source>
        <translation>Вверх</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13269"/>
        <source>Open Selected</source>
        <translation>Открытый выбор</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13271"/>
        <source>Show the parent folder in the repaired system.</source>
        <translation>Покажите родительскую папку в отремонтированной системе.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13272"/>
        <source>Open the selected repaired-system folder.</source>
        <translation>Откройте выбранную папку исправленной системы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13273"/>
        <source>Reload this repaired-system folder through a fresh read-only mount.</source>
        <translation>Перезагрузите эту исправленную системную папку через новое крепление только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13280"/>
        <source>Read-only target view; no files are modified.</source>
        <translation>Просмотр только для чтения; файлы не изменяются.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13286"/>
        <source>Choose This Folder</source>
        <translation>Выберите эту папку</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13299"/>
        <source>Invalid repair-system path.</source>
        <translation>Недействительный путь ремонтной системы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13311"/>
        <source>Unable to read this repaired-system folder. Review Logs for the helper error.</source>
        <translation>Невозможно прочитать эту исправленную системную папку. Отзывы об ошибке Helper.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13341"/>
        <source>Current repaired-system folder: %1</source>
        <translation>Текущая папка исправленной системы: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13345"/>
        <source>Read-only target view; %1 subfolder(s) found. Nothing was modified.</source>
        <translation>Вид цели только для чтения; найдена подпапка %1. Ничего не было изменено.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13398"/>
        <location filename="../src/MainWindow.cpp" line="13429"/>
        <location filename="../src/MainWindow.cpp" line="13476"/>
        <source>Invalid repair path</source>
        <translation>Неверный путь ремонта</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13399"/>
        <location filename="../src/MainWindow.cpp" line="13430"/>
        <location filename="../src/MainWindow.cpp" line="13477"/>
        <source>Use an absolute path inside the repaired system. Parent-directory escape paths are not accepted.</source>
        <translation>Используйте абсолютный путь внутри отремонтированной системы. Пути побега родителей не принимаются.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13419"/>
        <source>Repair-system source file</source>
        <translation>Ремонт исходного файла системы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13420"/>
        <source>Enter an absolute file path inside the repaired system:</source>
        <translation>Введите абсолютный путь файла в восстановленную систему:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13444"/>
        <source>Select host files</source>
        <translation>Выберите host файлы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13466"/>
        <source>Repair-system source folder</source>
        <translation>Исходная папка системы восстановления</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13467"/>
        <source>Enter an absolute folder path inside the repaired system:</source>
        <translation>Введите абсолютный путь папки внутри отремонтированной системы:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13491"/>
        <source>Select host folder</source>
        <translation>Выберите папку host</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13531"/>
        <source>Add at least one source file or folder.</source>
        <translation>Добавьте хотя бы один исходный файл или папку.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13535"/>
        <source>Choose a destination folder.</source>
        <translation>Выберите папку назначения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13539"/>
        <source>rsync is required for verified File Copy.</source>
        <translation>rsync требуется для верифицированной копии файла.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13548"/>
        <source>Repair → Host requires an existing, non-symlink host destination folder.</source>
        <translation>Ремонт → Хост требует существующей папки назначения хоста без симлинков.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13553"/>
        <source>Host → Repair requires a specific absolute path inside the repaired system.</source>
        <translation>Ремонт хоста → требует определенного абсолютного пути внутри отремонтированной системы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13630"/>
        <location filename="../src/MainWindow.cpp" line="13686"/>
        <source>Execute a command on the running host as root.</source>
        <translation>Выполните команду на бегущем хосте в качестве корня.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13642"/>
        <location filename="../src/MainWindow.cpp" line="13842"/>
        <source>Host shell</source>
        <translation>Хранитель снаряда</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13650"/>
        <source>Run a command on the running host as root (sudo is not needed). Commands are executed directly on the active system. When a command asks a question, Boot Bitch shows it in a popup and sends your answer back to the command; cancelling stops the command. Non-interactive flags such as apt-get -y upgrade remain recommended for unattended runs. Output is kept in this window and in the application log.</source>
        <translation>Запустите команду на бегущем хосте как root (судо не требуется). Команды выполняются непосредственно в активной системе. Когда команда задает вопрос, Boot Bitch показывает его всплывающим окном и отправляет ответ обратно в команду; отмена останавливает команду. Неинтерактивные флаги, такие как обновление apt-get-y, по-прежнему рекомендуются для необслуживаемых запусков. Выход хранится в этом окне и в журнале приложений.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13657"/>
        <source>Command to run on the running host, for example: apt update</source>
        <translation>Команда для запуска на запущенном хосте, например: обновление apt</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13664"/>
        <source>Run on Host</source>
        <translation>Беги по хосту</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13668"/>
        <source>Commands can modify the running host. Review each command before running it.</source>
        <translation>Команды могут изменять работающий хост. Проверяйте каждую команду перед запуском.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13682"/>
        <source>Shell</source>
        <translation>Shell</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13683"/>
        <source>Host Shell</source>
        <translation>Ведущий Shell</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5754"/>
        <location filename="../src/MainWindow.cpp" line="13687"/>
        <source>Execute a command inside the selected repair system as root.</source>
        <translation>Выполните команду внутри выбранной системы ремонта как root.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6722"/>
        <location filename="../src/MainWindow.cpp" line="20404"/>
        <location filename="../src/MainWindow.cpp" line="20603"/>
        <source>Kernel repair</source>
        <translation>Ремонт ядра</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13727"/>
        <source>• %1: %2 changed from &apos;%3&apos; to &apos;%4&apos;</source>
        <translation>• %1: %2 изменился с «%3» на «%4»</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13792"/>
        <source>A diagnostic or repair is still running; wait a moment and retry the command.</source>
        <translation>Диагностика или ремонт все еще работают; подождите минуту и повторите команду.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13798"/>
        <source>Host shell unavailable</source>
        <translation>Host shell недоступен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13799"/>
        <source>Chroot shell unavailable</source>
        <translation>Chroot shell недоступен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13814"/>
        <source>The running-host identity is unresolved; no host shell command was sent.</source>
        <translation>Идентификация бегущего хоста неразрешена; команда оболочки хоста не была отправлена.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13815"/>
        <source>The repair target identity is incomplete; no chroot shell command was sent.</source>
        <translation>Идентификатор цели ремонта неполный; командная оболочка не была отправлена.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13838"/>
        <source>Running host shell command</source>
        <translation>Запуск команды Host shell</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13839"/>
        <source>Running chroot shell command</source>
        <translation>Запуск команды Chroot shell</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13886"/>
        <source>the affected repository</source>
        <translation>Пострадавшее хранилище</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13892"/>
        <source>Repository metadata changed</source>
        <translation>Изменились метаданные хранилища</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13893"/>
        <source>The repository changed its release metadata:
%1</source>
        <translation>Репозиторий изменил свои метаданные выпуска:
%1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13899"/>
        <source>The package manager refused the change. Allowing it re-runs the command once with Acquire::AllowReleaseInfoChange=true; the change is accepted for this retry only. Signature, key and package verification remain enforced.

Retry command:
%1</source>
        <translation>Менеджер пакета отказался от изменений. Разрешить повторно запустить команду один раз с помощью Acquire:: AllowReleaseInfoChange=true; изменение принимается только для этого повтора. Проверка подписи, ключа и пакета остается в силе.

Командование Retry:
%1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13907"/>
        <source>Allow and Retry</source>
        <translation>Разрешить и повторить</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13926"/>
        <source>Host shell response incomplete</source>
        <translation>Реакция Host shell неполная</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13926"/>
        <source>Chroot shell response incomplete</source>
        <translation>Реакция Chroot shell неполная</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13927"/>
        <source>The privileged helper did not return a complete response for this command. The request was treated as failed; review the output and Logs before retrying.</source>
        <translation>Привилегированный помощник не вернул полного ответа на эту команду. Запрос был рассмотрен как несостоявшийся; проверьте выход и журналы перед повторным использованием.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13960"/>
        <location filename="../src/MainWindow.cpp" line="13987"/>
        <source>File Copy unavailable</source>
        <translation>File Copy недоступен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13969"/>
        <source>Previewing file copy</source>
        <translation>Предварительный просмотр копии файла</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13980"/>
        <source>Preview File Copy</source>
        <translation>Предварительный просмотр File Copy</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13994"/>
        <source>Smart destination ownership</source>
        <translation>Умное владение</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14017"/>
        <source>… %1 more item(s)</source>
        <translation>… %1 More item(s)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14022"/>
        <source>Confirm File Copy</source>
        <translation>Подтвердить копию файла</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14023"/>
        <source>%1 — copy and verify %2 source item(s)?</source>
        <translation>%1 — копирование и проверка исходного элемента(ов) %2?</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14025"/>
        <source>Destination: %1
Ownership: %2

Sources:
• %3

</source>
        <translation>Место назначения: %1
Производитель: %2

Источники:
• %3

</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14028"/>
        <source>The repair target stays read-only. Existing same-name files in the host destination may be overwritten. The privileged helper refuses system-critical host destinations.

</source>
        <translation>Цель ремонта остается только для чтения. Существующие файлы с тем же именем в пункте назначения хоста могут быть перезаписаны. Привилегированный помощник отказывается от критически важных для системы мест назначения.

</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14030"/>
        <source>The selected repair filesystem is promoted read-write only after host/target safety checks. Existing same-name target files may be overwritten.

</source>
        <translation>Выбранная файловая система ремонта продвигается для чтения-записи только после проверки безопасности хоста/цели. Существующие целевые файлы могут быть перезаписаны.

</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14032"/>
        <source>SENSITIVE TARGET PATH: this destination can change boot or operating-system files.

</source>
        <translation>SENSITIVE TARGET PATH: этот пункт назначения может изменять загрузочные или операционные файлы.

</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14035"/>
        <source>rsync does not use --delete, so unrelated destination files remain. A second checksum/metadata pass and SHA-256 verification of regular files run after the copy.</source>
        <translation>rsync не использует — удаляет, поэтому несвязанные файлы назначения остаются. Второй контрольный пакет и проверка SHA-256 обычных файлов выполняются после копии.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14044"/>
        <source>Copying files</source>
        <translation>Копирование файлов</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14059"/>
        <source>File Copy — %1</source>
        <translation>Копия файла - %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14070"/>
        <source>Available — KF6 KAuth linked</source>
        <translation>Доступно: KF6 KAuth</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14072"/>
        <source>Not compiled — optional integration unavailable</source>
        <translation>Не скомпилировано — опциональная интеграция недоступна</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14092"/>
        <location filename="../src/MainWindow.cpp" line="14104"/>
        <source>Available</source>
        <translation>Доступный</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14092"/>
        <location filename="../src/MainWindow.cpp" line="14104"/>
        <source>Missing</source>
        <translation>Пропавший</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14183"/>
        <source>Running Host diagnostics require Host Maintenance. Choose Enter Host Maintenance on the protected running-host card in Systems first.</source>
        <translation>Диагностика хоста требует обслуживания хоста. Сначала выберите Enter Host Maintenance на защищенной хост-карте в Systems.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14270"/>
        <source>Individual diagnostics have changed. Please re-run all diagnostics.</source>
        <translation>Изменилась индивидуальная диагностика. Пожалуйста, повторите диагностику.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14291"/>
        <source>Select a repair target in Systems.</source>
        <translation>Выберите цель ремонта в системах.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14311"/>
        <source>Cached: %1 result</source>
        <translation>Кашед: результат %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14315"/>
        <source>Cached: %1 — protected running-host result.</source>
        <translation>Cached: %1 — защищенный результат выполнения хоста.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14316"/>
        <source>Re-run Diagnostic</source>
        <translation>Повторная диагностика</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14320"/>
        <source>Please re-run this diagnostic after the last repair or target configuration change.</source>
        <translation>Пожалуйста, повторите эту диагностику после последнего ремонта или изменения конфигурации цели.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14322"/>
        <source>Available — read-only selected repair-system inspection; administrator authorization is already active for this Boot Bitch window.</source>
        <translation>Доступно — только для чтения выбрана проверка ремонтной системы; авторизация администратора уже активна для этого окна Boot Bitch.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14323"/>
        <source>Available — read-only selected repair-system inspection; the first root action will request administrator authorization.</source>
        <translation>Доступно — только для чтения выбранная проверка ремонтной системы; первое действие root запросит разрешение администратора.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14326"/>
        <source>Please re-run this diagnostic after the last repair action.</source>
        <translation>Пожалуйста, повторите диагностику после последнего ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14327"/>
        <source>Available — protected running host</source>
        <translation>Доступно: защищенный бегущий хост</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14588"/>
        <source>Run running-host diagnostics</source>
        <translation>Диагностика бегущего хозяина</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14589"/>
        <source>Read-only running-host diagnostic</source>
        <translation>Диагностика только для бегущих хостов</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14592"/>
        <source>Run All target diagnostics</source>
        <translation>Запуск всех целевых диагностических</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14593"/>
        <source>Read-only target diagnostic</source>
        <translation>Диагностика только для чтения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14648"/>
        <source>Starting read-only %1 diagnostic &apos;%2&apos; on %3 (%4).</source>
        <translation>Начало диагностики %1 только для чтения «%2» на %3 (%4).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14659"/>
        <source>Read-only %1 diagnostic &apos;%2&apos; %3.</source>
        <translation>Только для чтения %1 диагностический %2 %3.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14661"/>
        <source>completed</source>
        <translation>завершено</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14661"/>
        <source>failed</source>
        <translation>неудачный</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14697"/>
        <location filename="../src/MainWindow.cpp" line="14777"/>
        <source>%1 finished with exit code 0 (success).</source>
        <translation>%1 завершается выходным кодом 0 (успех).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14712"/>
        <location filename="../src/MainWindow.cpp" line="14829"/>
        <source>Target diagnostic unavailable</source>
        <translation>Диагностика цели недоступна</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14733"/>
        <source>Discarded the read-only target diagnostic &apos;%1&apos; because the active repair target changed while the request was running.</source>
        <translation>Отбросили цель диагностики %1 только для чтения, потому что активная цель ремонта изменилась во время выполнения запроса.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14793"/>
        <location filename="../src/MainWindow.cpp" line="14828"/>
        <source>Host diagnostic unavailable</source>
        <translation>Диагностика хоста недоступна</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15014"/>
        <source>No repair target is ready for diagnostics.</source>
        <translation>Ни одна цель не готова для диагностики.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15017"/>
        <source>The selected repair target is ready for diagnostics.</source>
        <translation>Выбранная цель ремонта готова к диагностике.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15022"/>
        <source>Running Host diagnostics require Host Maintenance. Choose Host Maintenance on the protected running-host card in Systems first.</source>
        <translation>Диагностика хоста требует обслуживания хоста. Сначала выберите обслуживание хоста на защищенной карте бегущего хоста в системах.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15028"/>
        <source>The running host is not resolved for diagnostics.</source>
        <translation>Бегущий хозяин не решен для диагностики.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15031"/>
        <source>The running host is ready for diagnostics.</source>
        <translation>Ведущий готов к диагностике.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15046"/>
        <source>Target diagnostics were invalidated by a repair or target change.</source>
        <translation>Диагностика цели была признана недействительной в результате ремонта или изменения цели.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15050"/>
        <source>Target diagnostic sections were invalidated by a repair action and await regeneration.</source>
        <translation>Целевые диагностические секции были признаны недействительными в результате ремонта и ожидают регенерации.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15054"/>
        <source>No cached diagnostics belong to the selected target.</source>
        <translation>Кэшированная диагностика не относится к выбранной цели.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15058"/>
        <source>No cached diagnostics exist for the selected target.</source>
        <translation>Для выбранной цели не существует кэшированной диагностики.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15065"/>
        <source>Running-host diagnostic sections were invalidated by a repair action and await regeneration.</source>
        <translation>Диагностические секции бегущего хозяина были признаны недействительными в результате ремонта и ожидают регенерации.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15069"/>
        <source>No cached running-host diagnostics exist.</source>
        <translation>Никакой кэшированной диагностики бегущего хозяина не существует.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15085"/>
        <source>Automatic diagnostics regeneration is disabled in Settings.</source>
        <translation>Регенерация автоматической диагностики отключена в настройках.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15088"/>
        <source>A diagnostic or repair operation is already in progress.</source>
        <translation>Диагностическая или ремонтная работа уже ведется.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15098"/>
        <source>Current-scope diagnostic evidence is fresh.</source>
        <translation>Современные диагностические данные свежи.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15121"/>
        <source>Unknown repair tool: %1.</source>
        <translation>Неизвестный инструмент для ремонта: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15125"/>
        <location filename="../src/MainWindow.cpp" line="15301"/>
        <location filename="../src/MainWindow.cpp" line="21475"/>
        <source>Select a repair target in Systems first.</source>
        <translation>Сначала выберите цель ремонта в системе.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15128"/>
        <location filename="../src/MainWindow.cpp" line="15160"/>
        <source>Diagnostic evidence belongs to a different target. Run diagnostics for the selected target.</source>
        <translation>Диагностические данные принадлежат другой цели. Проведите диагностику выбранной цели.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15136"/>
        <source>Target state changed after the last diagnostic or repair action. Please regenerate diagnostics before starting another repair.</source>
        <translation>Состояние цели изменилось после последнего диагностического или ремонтного действия. Пожалуйста, восстановите диагностику перед началом ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15140"/>
        <location filename="../src/MainWindow.cpp" line="15179"/>
        <source>Repair tool %1 is available in the selected scope&apos;s cached diagnostics.</source>
        <translation>Ремонтный инструмент %1 доступен в кэшированной диагностике выбранной области.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15231"/>
        <source>Unrecognised running-host default-entry capability evidence: %1. Run running-host diagnostics again, and update Boot Bitch if the helper format changed.</source>
        <translation>Непризнанные доказательства возможностей ввода по умолчанию: %1. Запустите диагностику хоста и обновите Boot Bitch, если формат помощника изменился.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15237"/>
        <source>The running host&apos;s verified default boot entry can be restored or promoted.</source>
        <translation>Проверенная запись загрузки хоста по умолчанию может быть восстановлена или повышена.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15243"/>
        <source>The cached running-host diagnostics do not decide whether the host default entry can be restored. Regenerate running-host diagnostics in Host Maintenance, and update Boot Bitch if the helper format changed.</source>
        <translation>Кэшированная диагностика бегущего хоста не решает, можно ли восстановить вход по умолчанию. Восстановите диагностику бегущего хоста в обслуживании хоста и обновите Boot Bitch, если формат помощника изменился.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15244"/>
        <source>No running-host default-entry capability evidence is cached. Run read-only running-host diagnostics in Host Maintenance before using Make Default.</source>
        <translation>Ни одно доказательство возможности запуска-хоста по умолчанию не кэшируется. Запустите диагностику только для чтения в обслуживании хоста перед использованием Make Default.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15293"/>
        <source>Run the %1 diagnostic for the protected running host before starting this maintenance action.</source>
        <translation>Запустите диагностику %1 для защищенного хоста перед началом этого действия по техническому обслуживанию.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15296"/>
        <source>Required read-only running-host diagnostics are cached for this maintenance action.</source>
        <translation>Требуемая диагностика бегущего хоста только для чтения кэшируется для этого действия по техническому обслуживанию.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15305"/>
        <source>Run the selected target diagnostic before starting this repair.</source>
        <translation>Запустите выбранную целевую диагностику перед началом этого ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15316"/>
        <source>Target state changed after the last diagnostic or repair action. Run the %1 diagnostic for this target before starting this repair.</source>
        <translation>Состояние цели изменилось после последнего диагностического или ремонтного действия. Проведите диагностику %1 для этой цели перед началом ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15317"/>
        <source>Run the %1 diagnostic for this target before starting this repair.</source>
        <translation>Проведите диагностику %1 для этой цели перед началом ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15320"/>
        <source>Required read-only diagnostic evidence is cached for this repair tool.</source>
        <translation>Для этого инструмента ремонта кэшируются необходимые диагностические данные только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15598"/>
        <source>package metadata refreshed — changes were applied</source>
        <translation>Обновлены метаданные пакета — внесены изменения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15599"/>
        <source>package metadata already current — no changes</source>
        <translation>Метаданные пакета уже актуальны — никаких изменений</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15600"/>
        <source>package metadata refresh failed</source>
        <translation>Обновление метаданных не удалось</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15601"/>
        <source>package metadata refresh not checked</source>
        <translation>Обновление метаданных пакета не проверено</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15602"/>
        <source>package metadata refresh did not run</source>
        <translation>Обновление метаданных пакета не запускалось</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15604"/>
        <source>package configuration completed — changes were applied</source>
        <translation>Конфигурация пакета завершена — внесены изменения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15605"/>
        <source>nothing to configure</source>
        <translation>Нечего настраивать</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15606"/>
        <source>package configuration failed</source>
        <translation>Конфигурация пакета не удалась</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15607"/>
        <source>package configuration not checked</source>
        <translation>Конфигурация пакета не проверяется</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15608"/>
        <source>package configuration did not run</source>
        <translation>Конфигурация пакета не работает</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15610"/>
        <source>broken dependencies repaired — changes were applied</source>
        <translation>Исправлены неисправные зависимости — внесены изменения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15611"/>
        <source>no broken dependencies</source>
        <translation>Нет сломанных зависимостей</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15612"/>
        <source>broken dependency repair failed</source>
        <translation>Неудачный ремонт сломанной зависимости</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15613"/>
        <source>broken dependency repair not checked</source>
        <translation>Непроверенный ремонт зависимостей</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15614"/>
        <source>broken dependency repair did not run</source>
        <translation>Неисправный ремонт зависимостей не работает</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15616"/>
        <source>packages upgraded — changes were applied</source>
        <translation>пакеты модернизированы — внесены изменения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15617"/>
        <source>no packages to upgrade</source>
        <translation>Нет пакетов для обновления</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15618"/>
        <source>package upgrade failed</source>
        <translation>Обновление пакета провалилось</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15619"/>
        <source>package upgrade not checked</source>
        <translation>Обновление пакета не проверено</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15620"/>
        <source>package upgrade did not run</source>
        <translation>Обновление пакета не запущено</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15622"/>
        <source>DKMS modules rebuilt — changes were applied</source>
        <translation>Модули ДКМС перестроены — внесены изменения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15623"/>
        <source>DKMS modules already current — no changes</source>
        <translation>Модули DKMS уже работают — изменений нет</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15624"/>
        <source>DKMS rebuild failed</source>
        <translation>Восстановление КДМС провалилось</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15625"/>
        <source>DKMS rebuild not checked</source>
        <translation>Восстановление КДМС не проверено</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15626"/>
        <source>DKMS rebuild did not run</source>
        <translation>Восстановление ДКМС не проводилось</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15628"/>
        <source>display manager repaired — changes were applied</source>
        <translation>Ремонт дисплея — внесены изменения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15629"/>
        <source>display manager already correct — no changes</source>
        <translation>Дисплей-менеджер уже исправен — никаких изменений</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15630"/>
        <source>display manager repair failed</source>
        <translation>Ремонт дисплея провалился</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15631"/>
        <source>display manager repair not checked</source>
        <translation>Ремонт дисплея не проверен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15632"/>
        <source>display manager repair did not run</source>
        <translation>Ремонт дисплея не состоялся</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15634"/>
        <source>initramfs rebuilt — images regenerated</source>
        <translation>initramfs реконструирован — изображения регенерированы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15635"/>
        <source>initramfs already current — no changes</source>
        <translation>initramfs уже работает — никаких изменений</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15636"/>
        <source>initramfs rebuild failed</source>
        <translation>Реконструкция initramfs провалилась</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15637"/>
        <source>initramfs rebuild not checked</source>
        <translation>Реконструкция initramfs не проверена</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15638"/>
        <source>initramfs rebuild did not run</source>
        <translation>Реконструкция initramfs не состоялась</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15640"/>
        <source>EFI boot path repaired — changes were applied</source>
        <translation>Загрузочный путь EFI отремонтирован — внесены изменения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15641"/>
        <source>EFI boot path already correct — no changes</source>
        <translation>Путь загрузки EFI уже правильный - никаких изменений</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15642"/>
        <source>EFI boot path repair failed</source>
        <translation>Ремонт загрузочного пути EFI провалился</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15643"/>
        <source>EFI boot path repair not checked</source>
        <translation>Ремонт загрузочного пути EFI не проверен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15644"/>
        <source>EFI boot path repair did not run</source>
        <translation>Ремонт загрузочного пути EFI не состоялся</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15646"/>
        <source>GRUB configuration regenerated — changes were applied</source>
        <translation>Регенерирована конфигурация GRUB — внесены изменения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15647"/>
        <source>GRUB configuration already current — no changes</source>
        <translation>Конфигурация GRUB уже актуальна — никаких изменений</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15648"/>
        <source>GRUB regeneration failed</source>
        <translation>Регенерация GRUB провалилась</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15649"/>
        <source>GRUB regeneration not checked</source>
        <translation>Регенерация GRUB не проверена</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15650"/>
        <source>GRUB regeneration did not run</source>
        <translation>Регенерация GRUB не прошла</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15652"/>
        <source>extlinux configuration regenerated — changes were applied</source>
        <translation>Регенерирована конфигурация extlinux — внесены изменения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15653"/>
        <source>extlinux configuration already current — no changes</source>
        <translation>Конфигурация extlinux уже актуальна — никаких изменений</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15654"/>
        <source>extlinux regeneration failed</source>
        <translation>Регенерация extlinux провалилась</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15655"/>
        <source>extlinux regeneration not checked</source>
        <translation>Регенерация extlinux не проверена</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15656"/>
        <source>extlinux regeneration did not run</source>
        <translation>Регенерация extlinux не прошла</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15658"/>
        <source>file system repaired — changes were applied</source>
        <translation>Ремонт файловой системы — изменения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15659"/>
        <source>no file system errors found — no changes</source>
        <translation>Не найдено ошибок файловой системы — нет изменений</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15660"/>
        <source>file system repair failed</source>
        <translation>Ремонт файловой системы провалился</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15661"/>
        <source>not all filesystems were checked</source>
        <translation>Не все файловые системы были проверены.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15662"/>
        <source>file system repair did not run</source>
        <translation>Ремонт файловой системы не выполняется</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15664"/>
        <source>boot stack reconciled — changes were applied</source>
        <translation>Загрузочный стек помирился — были применены изменения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15665"/>
        <source>boot stack already current — no changes</source>
        <translation>Загрузочный стек уже существующий - никаких изменений</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15666"/>
        <source>boot stack repair failed</source>
        <translation>Ремонт багажника провалился</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15667"/>
        <source>boot stack not checked</source>
        <translation>Загрузочный стек не проверен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15668"/>
        <source>boot stack repair did not run</source>
        <translation>Ремонт багажника не состоялся</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15670"/>
        <source>validation completed — changes were applied</source>
        <translation>Валидация завершена — внесены изменения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15671"/>
        <source>validation is read-only — no changes</source>
        <translation>Валидация только для чтения - никаких изменений</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15672"/>
        <source>validation failed</source>
        <translation>валидация провалилась</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15673"/>
        <source>validation not checked</source>
        <translation>валидация не проверена</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15674"/>
        <source>validation did not run</source>
        <translation>Проверка не проводилась</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15676"/>
        <source>host boot default updated — changes were applied</source>
        <translation>Обновленный по умолчанию хост-загрузка — изменения применены</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15677"/>
        <source>host boot default already correct — no changes</source>
        <translation>Загрузка хоста по умолчанию уже верна — никаких изменений</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15678"/>
        <source>host boot default update failed</source>
        <translation>Обновление хост-загрузки по умолчанию провалилось</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15679"/>
        <source>host boot default not checked</source>
        <translation>хост-загрузка по умолчанию не проверяется</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15680"/>
        <source>host boot default update did not run</source>
        <translation>Обновление host boot default не работает</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15682"/>
        <source>firmware boot order updated — changes were applied</source>
        <translation>Обновлен порядок загрузки прошивки — внесены изменения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15683"/>
        <source>firmware boot order already current — no changes</source>
        <translation>Загрузка прошивки уже актуальна - никаких изменений</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15684"/>
        <source>firmware boot order update failed</source>
        <translation>Обновление загрузки прошивки не удалось</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15685"/>
        <source>firmware boot order not checked</source>
        <translation>Заказ загрузки прошивки не проверен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15686"/>
        <source>firmware boot order update did not run</source>
        <translation>Обновление загрузки прошивки не запускалось</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15688"/>
        <source>firmware boot entry deleted — changes were applied</source>
        <translation>Загрузка прошивки удалена — были внесены изменения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15689"/>
        <source>firmware boot entry was already absent — no changes</source>
        <translation>Загрузка прошивки уже отсутствовала — никаких изменений</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15690"/>
        <source>firmware boot entry delete failed</source>
        <translation>Загрузка прошивки удалить не удалось</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15691"/>
        <location filename="../src/MainWindow.cpp" line="15697"/>
        <source>firmware boot entry not checked</source>
        <translation>Загрузка прошивки не проверена</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15692"/>
        <source>firmware boot entry delete did not run</source>
        <translation>Прошивка boot entry delete не запускается</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15694"/>
        <source>firmware boot entry renamed — changes were applied</source>
        <translation>Загрузка прошивки переименовывалась — вносились изменения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15695"/>
        <source>firmware boot entry label already correct — no changes</source>
        <translation>Маркировка загрузки прошивки уже верна - никаких изменений</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15696"/>
        <source>firmware boot entry rename failed</source>
        <translation>Загрузка прошивки переименована в неудачную</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15698"/>
        <source>firmware boot entry rename did not run</source>
        <translation>Прошивка Boot Entry Name не запускается</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15700"/>
        <source>repair successful — changes were applied</source>
        <translation>Успешный ремонт – изменения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15701"/>
        <source>no repair needed — no changes were detected</source>
        <translation>Ремонт не требуется — никаких изменений не обнаружено.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15702"/>
        <source>repair failed</source>
        <translation>Неудачный ремонт</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15703"/>
        <source>not checked — no repair was attempted</source>
        <translation>Не проверено — ремонт не был предпринят</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15704"/>
        <source>repair did not run</source>
        <translation>Ремонт не проводился</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16030"/>
        <source>Automatic running-host diagnostics regeneration skipped: Host Maintenance is not active.</source>
        <translation>Пропущена регенерация автоматической диагностики бегущего хоста: техническое обслуживание хоста неактивно.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16040"/>
        <source>Regenerating target diagnostics</source>
        <translation>Восстановление целевой диагностики</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16041"/>
        <source>Regenerating running-host diagnostics</source>
        <translation>Регенерация диагностики бегущего хоста</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16046"/>
        <location filename="../src/MainWindow.cpp" line="16546"/>
        <source>Running…</source>
        <translation>Запуск …</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16096"/>
        <location filename="../src/MainWindow.cpp" line="16276"/>
        <source>%1 diagnostic run: %2</source>
        <translation>%1 диагностический запуск: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8310"/>
        <location filename="../src/MainWindow.cpp" line="16097"/>
        <location filename="../src/MainWindow.cpp" line="16276"/>
        <location filename="../src/MainWindow.cpp" line="16595"/>
        <source>Target</source>
        <translation>Цель</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5635"/>
        <source>Ordinary repairs need a non-host target; the host uses its own maintenance mode.</source>
        <translation>Обычный ремонт нуждается в нехозяйской цели; хост использует свой собственный режим обслуживания.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5994"/>
        <source>End the privileged session and close Boot Bitch-owned LUKS mappings.</source>
        <translation>Завершите привилегированную сессию и закройте карты LUKS, принадлежащие Boot Bitch.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6156"/>
        <source>Restore the host&apos;s default boot entry and make it default.</source>
        <translation>Восстановите вход загрузки хоста по умолчанию и сделайте его по умолчанию.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6275"/>
        <source>No repair target committed; rows are inspection-only until Select Target.</source>
        <translation>Цели ремонта не выполняются; ряды проверяются только до выбранной цели.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6348"/>
        <source>Best component visible before privileged probing.</source>
        <translation>Лучший компонент, видимый до привилегированного зондирования.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6424"/>
        <source>Read or edit the selected target configuration file.</source>
        <translation>Прочитайте или отредактируйте выбранный файл конфигурации цели.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6510"/>
        <source>Copy the diagnostic results to the clipboard.</source>
        <translation>Копируйте результаты диагностики в буфер обмена.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6511"/>
        <source>Save the diagnostic results to a file.</source>
        <translation>Сохранить результаты диагностики в файл.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6872"/>
        <source>Reboot the host after confirmation; all users are signed out.</source>
        <translation>Перезагрузите хост после подтверждения; все пользователи подписаны.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6874"/>
        <source>Hide the reminder until Host Maintenance is re-entered.</source>
        <translation>Спрячьте напоминание до тех пор, пока хозяин не вернется.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6968"/>
        <source>Run a read-only rollback preflight, then promote a writable copy to @.</source>
        <translation>Запустите предварительный полет только для чтения, а затем продвигайте письменную копию на @.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7141"/>
        <location filename="../src/MainWindow.cpp" line="7161"/>
        <source>Kernel: %1</source>
        <translation>Ядро: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7144"/>
        <location filename="../src/MainWindow.cpp" line="7164"/>
        <source>Initramfs: %1</source>
        <translation>Инитрамфы: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7147"/>
        <source>This entry is the current extlinux default.</source>
        <translation>Эта запись является текущим по умолчанию extlinux.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7227"/>
        <location filename="../src/MainWindow.cpp" line="7407"/>
        <location filename="../src/MainWindow.cpp" line="7484"/>
        <source>Boot menu</source>
        <translation>Загрузочное меню</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7229"/>
        <source>Switch between the UEFI/UKI firmware entries and the read-only boot menu.</source>
        <translation>Переключитесь между входами прошивки UEFI / UKI и меню загрузки только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7305"/>
        <location filename="../src/MainWindow.cpp" line="8314"/>
        <source>Save</source>
        <translation>Спасти</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7400"/>
        <source>Switch the view selector to Boot menu to list the current scope&apos;s boot menu read-only.</source>
        <translation>Переключите селектор просмотра в меню загрузки, чтобы перечислить меню загрузки текущего объема только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7613"/>
        <source>No UEFI firmware boot entries were found — this system is BIOS/legacy or lacks efibootmgr, so there is no firmware boot order to manage.</source>
        <translation>Загрузочные записи прошивки UEFI не были найдены — эта система является BIOS / унаследована или не имеет efibootmgr, поэтому нет порядка загрузки прошивки для управления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7644"/>
        <source>Loading boot menu</source>
        <translation>Меню загрузки багажника</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7651"/>
        <location filename="../src/MainWindow.cpp" line="7693"/>
        <location filename="../src/MainWindow.cpp" line="7721"/>
        <source>List boot menu</source>
        <translation>Список загрузок меню</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7665"/>
        <source>The boot menu could not be loaded. See Logs for details.</source>
        <translation>Меню загрузки невозможно загрузить. Смотрите журналы для деталей.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7757"/>
        <source>No boot menu entries were found.</source>
        <translation>Меню загрузки не найдено.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7711"/>
        <location filename="../src/MainWindow.cpp" line="7739"/>
        <location filename="../src/MainWindow.cpp" line="7771"/>
        <source>Loaded %1 boot menu entries.</source>
        <translation>Загрузка меню %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7835"/>
        <source>Renamed to &quot;%1&quot; — Save applies it.</source>
        <translation>Переименовано в «%1» — Save применяет его.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8094"/>
        <source>Boot menu on %1</source>
        <translation>Загрузочное меню на %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8126"/>
        <source>%1 boot menu entries loaded read-only.</source>
        <translation>Загрузка меню %1 загружается только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8129"/>
        <source>%1 boot menu viewing is available. Load Menu to list the boot menu read-only.</source>
        <translation>Доступно меню загрузки %1. Меню загрузки, чтобы перечислить меню загрузки только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8139"/>
        <source>%1 firmware boot entries loaded. Reorder rows, edit labels or mark an entry for deletion, then Save to apply all changes.</source>
        <translation>Загрузка прошивки %1. Заказать строки, отредактировать ярлыки или отметить запись для удаления, затем сохранить, чтобы применить все изменения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8172"/>
        <source>Load the %1 boot menu read-only.</source>
        <translation>Загрузите меню загрузки %1 только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8193"/>
        <source>No pending firmware boot entry changes. Reorder rows, edit labels or mark an entry for deletion first.</source>
        <translation>Никаких отложенных изменений загрузки прошивки. Заказать строки, отредактировать ярлыки или пометить запись для удаления в первую очередь.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8259"/>
        <source>Save firmware boot entry changes unavailable</source>
        <translation>Сохранить изменения загрузки прошивки недоступно</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8263"/>
        <source>Load boot entries before saving changes.</source>
        <translation>Загрузите загрузочные записи перед сохранением изменений.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8277"/>
        <source>No pending firmware boot entry changes to save.</source>
        <translation>Никаких отложенных изменений загрузки прошивки для сохранения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8290"/>
        <source>Reorder firmware boot order to: %1</source>
        <translation>Заказ на загрузку прошивки: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8294"/>
        <source>Delete Boot%1 (%2)</source>
        <translation>Удалить Boot%1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8300"/>
        <source>Rename Boot%1 (%2) to &quot;%3&quot;</source>
        <translation>Переименовать Boot%1 (%2) в «%3»</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8306"/>
        <source>Save firmware boot entry changes</source>
        <translation>Сохраните изменения загрузки прошивки</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8307"/>
        <source>Apply all pending firmware boot entry changes?</source>
        <translation>Применять все ожидающие изменения загрузки прошивки?</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8308"/>
        <source>%1 disk: %2
Root: %3

%4

Each change runs through the verified helper, which re-checks the entry, writes it, reads it back and restores the previous state on any mismatch.</source>
        <translation>Диск %1: %2
Источник: %3

%4

Каждое изменение проходит через проверенного помощника, который перепроверяет запись, пишет ее, читает ее обратно и восстанавливает предыдущее состояние при любом несоответствии.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8310"/>
        <location filename="../src/MainWindow.cpp" line="16097"/>
        <location filename="../src/MainWindow.cpp" line="16276"/>
        <location filename="../src/MainWindow.cpp" line="16595"/>
        <source>Host</source>
        <translation>ведущий</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8333"/>
        <source>Save firmware boot order</source>
        <translation>Сохранить загрузочный файл</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8344"/>
        <source>reorder: %1</source>
        <translation>Режим работы: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8366"/>
        <source>delete Boot%1: %2</source>
        <translation>Скачать Boot%1: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8389"/>
        <source>rename Boot%1: %2</source>
        <translation>Оригинальное название: Boot%1: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8409"/>
        <source>Firmware boot entry changes saved.</source>
        <translation>Сохранены изменения загрузки прошивки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8417"/>
        <source>Firmware boot entry changes failed: %1</source>
        <translation>Изменения загрузки прошивки не удались: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8419"/>
        <source>Firmware boot entry changes failed</source>
        <translation>Изменения загрузки прошивки не удались</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8420"/>
        <source>One or more firmware boot entry changes failed:
%1

Pending changes were kept so you can retry. Full helper output is available in Logs.</source>
        <translation>Одно или несколько изменений загрузки прошивки не удалось:
%1

В ожидании изменений были сохранены, чтобы вы могли повторить. Полный вспомогательный выход доступен в журналах.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8485"/>
        <source>Deletion of Boot%1 cancelled.</source>
        <translation>Удаление Boot%1 отменено.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8492"/>
        <source>Boot%1 marked for deletion — press Save to apply.</source>
        <translation>Boot%1, помеченный для удаления — нажмите Save to apply.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8553"/>
        <source>Boot%1 rename staged — press Save to apply.</source>
        <translation>Boot%1 переименовали в сценический — нажмите Save to application.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8707"/>
        <source>Clear the command output window.</source>
        <translation>Очистите окно вывода команды.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8760"/>
        <location filename="../src/MainWindow.cpp" line="13138"/>
        <source>Copy staged items and verify them; same-name destinations are overwritten.</source>
        <translation>Копируйте постановочные элементы и проверяйте их; одноименные пункты назначения перезаписываются.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8844"/>
        <source>Smart mode validates UID/GID mapping and falls back to the destination owner.</source>
        <translation>Смарт-режим проверяет отображение UID / GID и возвращается к владельцу назначения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8985"/>
        <source>Save the application log to a file.</source>
        <translation>Сохраните журнал приложения в файл.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8986"/>
        <source>Clear the on-screen action register.</source>
        <translation>Очистите экранный регистр действий.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9021"/>
        <source>Start a new session log file.</source>
        <translation>Запустите новый файл журнала сеанса.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9022"/>
        <source>Delete the selected session log.</source>
        <translation>Исключить выбранный журнал сеанса.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9023"/>
        <source>Refresh the session log list.</source>
        <translation>Обновить список журналов сессии.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9048"/>
        <source>Fuzzy filter; a term starting with @ selects a whole diagnostic section.</source>
        <translation>Нечеткий фильтр; термин, начинающийся с @, выбирает целый диагностический раздел.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9066"/>
        <source>Filter the log by entry kind.</source>
        <translation>Фильтруйте журнал по типу записи.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9156"/>
        <source>Read-only check of root, /boot, ESP and /home, then per-device repair.</source>
        <translation>Проверка только для чтения root, /boot, ESP и /home, а затем ремонт на устройстве.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9184"/>
        <source>Regenerate cached diagnostics after repairs or target changes.</source>
        <translation>Регенерировать кэшированную диагностику после ремонта или изменения цели.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9280"/>
        <source>Re-probe the available repair capabilities.</source>
        <translation>Проверить имеющиеся возможности ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11433"/>
        <source>Select an action below:</source>
        <translation>Выберите действие ниже:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11447"/>
        <source>Number of the action (0 = Abort)</source>
        <translation>Число действия (0 = аборт)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12060"/>
        <source>Committed repair target: %1</source>
        <translation>Цель ремонта: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12114"/>
        <source>List host root snapshots read-only.</source>
        <translation>Список снимков корня хоста только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12115"/>
        <source>List root snapshots read-only.</source>
        <translation>Список root-снимков только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12147"/>
        <source>Validate a rollback plan, promote a writable copy to @, and reconcile the boot stack.</source>
        <translation>Проверяйте план отката, продвигайте написанную копию на @ и примиряйте стек загрузки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12148"/>
        <source>Validate a rollback plan, promote a writable copy to @, and reconcile the bootloader.</source>
        <translation>Проверить план отката, продвинуть письменную копию на @ и примирить загрузчика.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12500"/>
        <source>Snapshot #%1 · %2</source>
        <translation>Снимок #%1 · %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13099"/>
        <source>Absolute path inside the repaired system, e.g. /home/user/document.txt.</source>
        <translation>Абсолютный путь внутри отремонтированной системы, например /home/user/document.txt.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13115"/>
        <source>Browse read-only and choose an absolute destination path.</source>
        <translation>Просмотрите только чтение и выберите абсолютный путь назначения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13626"/>
        <source>Commands stay disabled until diagnostics generation completes.</source>
        <translation>Команды остаются отключенными до завершения генерации диагностики.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15266"/>
        <source>Restore the host&apos;s %1 and place it first in BootOrder.</source>
        <translation>Восстановите %1 хоста и поместите его первым в BootOrder.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15267"/>
        <source>Restore the host&apos;s %1 and make it the default.</source>
        <translation>Восстановите %1 хоста и сделайте его по умолчанию.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16139"/>
        <source>A privileged operation is already running; the diagnostic was not started. Try again when it finishes.</source>
        <translation>Привилегированная операция уже запущена, диагностика не начата. Попробуйте еще раз, когда закончите.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16150"/>
        <location filename="../src/MainWindow.cpp" line="16510"/>
        <source>Host diagnostic refused: %1</source>
        <translation>Отказ в диагностике хозяина: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16154"/>
        <location filename="../src/MainWindow.cpp" line="16514"/>
        <source>Host Maintenance required</source>
        <translation>Обслуживание хоста необходимо</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16185"/>
        <source>Running diagnostic: %1</source>
        <translation>Диагностика: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16289"/>
        <location filename="../src/MainWindow.cpp" line="16524"/>
        <source>No repair target selected</source>
        <translation>Не выбрана цель ремонта</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16297"/>
        <source>Read target configuration</source>
        <translation>Читать целевую конфигурацию</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16301"/>
        <source>Configuration unavailable</source>
        <translation>Конфигурация недоступна</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16312"/>
        <source>Edit target %1</source>
        <translation>Редактировать цель %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16315"/>
        <source>Edit this target file through the guarded administrator helper. A successful save invalidates cached diagnostics; rerun diagnostics before repair. Generated files such as /boot/grub/grub.cfg may be replaced by the next bootloader update.</source>
        <translation>Редактируйте этот целевой файл через охраняемого помощника администратора. Успешное сохранение аннулирует кэшированную диагностику; повторите диагностику перед ремонтом. Сгенерированные файлы, такие как /boot/grub/grub.cfg, могут быть заменены следующим обновлением загрузчика.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16327"/>
        <source>Save Target File</source>
        <translation>Сохранить целевой файл</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16334"/>
        <location filename="../src/MainWindow.cpp" line="16376"/>
        <source>Write target configuration</source>
        <translation>Записать целевую конфигурацию</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16335"/>
        <source>Write the edited contents to %1? This modifies the repair target and invalidates cached diagnostics.</source>
        <translation>Написать отредактированный контент на %1? Это изменяет цель ремонта и аннулирует кэшированную диагностику.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16346"/>
        <source>Configuration write refused</source>
        <translation>Конфигурация письма отказала</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16347"/>
        <source>The edited content contains NUL bytes; the guarded write refuses it.</source>
        <translation>Отредактированный контент содержит байты NUL; охраняемая запись отказывается от него.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16352"/>
        <source>File too large</source>
        <translation>Файл слишком большой</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16353"/>
        <source>The edited file is larger than 1 MiB; the guarded write refuses it. Edit the file from a console instead.</source>
        <translation>Отредактированный файл больше 1 МиБ; охраняемая запись отказывается от него. Вместо этого отредактируйте файл с консоли.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16358"/>
        <source>Configuration write unavailable</source>
        <translation>Конфигурация Write Unavailable</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16359"/>
        <source>The edited content could not be written to a private temporary file in the application log directory; the write was not started.</source>
        <translation>Отредактированный контент не мог быть записан в частный временный файл в каталоге журнала приложений; запись не была начата.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16387"/>
        <source>Configuration write failed</source>
        <translation>Конфигурация писать не удалось</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16407"/>
        <source>Configuration saved</source>
        <translation>Конфигурация сохранена</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16491"/>
        <source>The full diagnostic report is already running; results appear when it completes.</source>
        <translation>Полный диагностический отчет уже запущен; результаты появляются, когда он завершается.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16495"/>
        <source>Full diagnostics will run after the current diagnostic finishes.</source>
        <translation>Полная диагностика будет проводиться после текущей диагностики.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16525"/>
        <source>Select a repair target in Systems, or enter Host Maintenance for running-host diagnostics.</source>
        <translation>Выберите цель ремонта в системах или введите хост-поддержку для диагностики бегущих хостов.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16531"/>
        <source>Regenerating diagnostics automatically</source>
        <translation>Автоматическая регенерация диагностики</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16532"/>
        <source>Running all diagnostics</source>
        <translation>Запуск всех диагностических</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16597"/>
        <source>Starting all available read-only diagnostics (%1 scope).</source>
        <translation>Запуск всей доступной диагностики только для чтения (%1).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16618"/>
        <source>All available read-only diagnostic summaries generated and cached by diagnostic.</source>
        <translation>Все доступные диагностические резюме, созданные и кэшированные диагностикой.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16619"/>
        <source>Run All diagnostics failed; failed output was not cached as successful diagnostic data.</source>
        <translation>Запуск Все диагностики не удался; неудавшийся вывод не был кэширован как успешные диагностические данные.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16729"/>
        <source>Regenerating diagnostics</source>
        <translation>Регенерирующая диагностика</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16739"/>
        <source>Regenerating read-only diagnostics…</source>
        <translation>Восстановление диагностики только для чтения …</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16761"/>
        <source>Read-only diagnostics regenerated automatically.</source>
        <translation>Диагностика только для чтения восстанавливается автоматически.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16762"/>
        <source>Automatic diagnostics regeneration did not complete; repair actions remain disabled until diagnostics are regenerated.</source>
        <translation>Регенерация автоматической диагностики не завершена, ремонтные работы остаются отключенными до восстановления диагностики.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16845"/>
        <source>Diagnostic results copied</source>
        <translation>Диагностические результаты скопированы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16870"/>
        <source>Unable to save diagnostics</source>
        <translation>Не удается сохранить диагностику</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16876"/>
        <source>Diagnostics saved to %1</source>
        <translation>Диагностика сохранена в %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16885"/>
        <source>Save application log</source>
        <translation>Сохранить журнал приложений</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16894"/>
        <source>Unable to save log</source>
        <translation>Не удается сохранить лог</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16902"/>
        <source>Log saved to %1</source>
        <translation>Лог сохранен для %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17012"/>
        <location filename="../src/MainWindow.cpp" line="17777"/>
        <source> · %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17040"/>
        <source>%1 — %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17769"/>
        <source>Current session</source>
        <translation>Нынешняя сессия</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17773"/>
        <source>Current session — not started</source>
        <translation>Нынешняя сессия не началась</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17775"/>
        <source> — %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17827"/>
        <source>Viewing prior session log %1 — generated %2 — read only</source>
        <translation>Просмотр журнала предыдущей сессии %1 — сгенерированный %2 — читать только</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17865"/>
        <source>Note:</source>
        <translation>Примечание:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17901"/>
        <location filename="../src/MainWindow.cpp" line="17908"/>
        <source>Delete session log</source>
        <translation>Исключить журнал сессии</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17902"/>
        <source>Delete %1 permanently? This cannot be undone.</source>
        <translation>Удалить %1 навсегда Это нельзя отменить.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17909"/>
        <source>Unable to delete %1.</source>
        <translation>Невозможно удалить %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18721"/>
        <source>(no matching entries for this filter)</source>
        <translation>(нет соответствующих записей для этого фильтра)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19044"/>
        <source>Boot Bitch must run from a different booted Linux environment than the system being repaired. Use a Linux live USB or another Linux installation on a different physical drive. The running host is protected from ordinary repair-target selection, but it can be explicitly selected through Host Maintenance for guarded native diagnostics and supported maintenance stages. Diagnostics follow the Systems page: the selected repair drive while Host Maintenance is off, or the protected running host while it is active. The first root action requests administrator authorization once for this Boot Bitch window; File → Lock Administrator Session ends that helper session immediately.</source>
        <translation>Boot Bitch должен работать в другой загруженной среде Linux, чем отремонтированная система. Используйте Linux Live USB или другую установку Linux на другом физическом диске. Бегущий хост защищен от обычного выбора цели ремонта, но он может быть явно выбран через техническое обслуживание хоста для защищенной нативной диагностики и поддерживаемых этапов обслуживания. Диагностика следует за страницей «Системы»: выбранный накопитель для ремонта во время обслуживания хоста отключен или защищенный работающий хост во время его активности. Первое действие root требует авторизации администратора для этого окна Boot Bitch; Сессия администратора блокировки файлов → немедленно завершает эту сессию помощника.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19059"/>
        <source>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19060"/>
        <source>&lt;p&gt;&lt;b&gt;Developer:&lt;/b&gt; CaptainMorgan12&lt;/p&gt;&lt;p&gt;A native Qt 6 Linux recovery and boot-repair utility.&lt;/p&gt;</source>
        <translation>&lt;p&gt;&lt;b&gt;Разработчик:&lt;/b&gt; CaptainMorgan12&lt;/p&gt;&lt;p&gt;Нативная утилита восстановления и восстановления Qt 6 Linux.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19062"/>
        <source>&lt;p&gt;&lt;b&gt;Guarded repair mode:&lt;/b&gt; read-only diagnostics can inspect either the protected Running Host or an explicitly selected repair drive. Debian/Ubuntu-family repairs can run through a privileged helper after confirmation; Host Maintenance enables the same supported stages natively on the active system after repeating the host identity and boot-mount checks.&lt;/p&gt;</source>
        <translation>&lt;p&gt;&lt;b&gt;Охраняемый режим ремонта: &lt;/b&gt; Диагностика только для чтения может проверить защищенный Running Host или явно выбранный привод для ремонта. Ремонт семейства Debian/Ubuntu может выполняться через привилегированного помощника после подтверждения; техническое обслуживание хоста позволяет выполнять те же поддерживаемые этапы в активной системе после повторения проверки личности хоста и загрузки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19063"/>
        <source>&lt;p&gt;Arch-family systems expose backend profiling and guarded package, initramfs, GRUB and conventional EFI repairs when their transaction-specific preflights pass. Fedora/RPM-family systems expose guarded dnf package transactions, dracut initramfs rebuilds, GRUB2 configuration and boot-code repair, and the systemd GDM display path from the same probe evidence; other RPM-family systems remain diagnostics-only until their transaction backend is implemented.&lt;/p&gt;</source>
        <translation>&lt;p&gt;Семейные системы обнажают бэкэнд-профилирование и защищенный пакет, initramfs, GRUB и обычный EFI ремонт, когда проходят их предварительные полеты. Системы семейства Fedora/RPM выявляют защищенные транзакции пакетов dnf, dracut initramfs восстанавливает, конфигурирует GRUB2 и восстанавливает загрузочный код, а также систематизированный путь отображения GDM из тех же доказательств зонда; другие системы семейства RPM остаются диагностическими только до тех пор, пока не будет реализован их бэкэнд транзакций.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19064"/>
        <source>&lt;p&gt;The first privileged action authorizes one narrow helper session for the current Boot Bitch window while the Qt GUI remains unprivileged. It can be ended at any time from File → Lock Administrator Session.&lt;/p&gt;</source>
        <translation>Первое привилегированное действие разрешает один узкий сеанс помощника для текущего окна Boot Bitch, в то время как Qt GUI остается непривилегированным. Он может быть завершен в любое время с сеанса администратора блокировки файлов →.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19065"/>
        <source>&lt;p&gt;LUKS target unlock, verified bidirectional File Copy, read-only host/repair diagnostics, transactional Btrfs snapshot rollback, offline graphical login recovery, distribution-aware EFI/UKI repair and boot-stack reconciliation are enabled through the guarded helper. Snapshot rollback preserves the previous @ and automatically restores it if critical post-switch reconciliation fails.&lt;/p&gt;</source>
        <translation>&lt;p&gt;Разблокировка цели LUKS, верифицированная двунаправленная копия файла, диагностика хоста / ремонта только для чтения, откат снимка транзакционного Btrfs, восстановление автономного графического входа в систему, исправление дистрибутива EFI / UKI и сверка загрузочного стека включены через охраняемого помощника. Откат снимка сохраняет предыдущий @ и автоматически восстанавливает его, если критическое выверка после переключения не удается.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19901"/>
        <location filename="../src/MainWindow.cpp" line="22017"/>
        <source>Restore detected graphical login manager</source>
        <translation>Восстановление обнаруженного графического менеджера входа</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19903"/>
        <source>Reinstall EFI bootloader</source>
        <translation>Переустановить загрузчик EFI</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19917"/>
        <location filename="../src/MainWindow.cpp" line="21401"/>
        <source>Reinstall Alpine GRUB EFI loader</source>
        <translation>Reinstall Alpine GRUB EFI погрузчик</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19919"/>
        <location filename="../src/MainWindow.cpp" line="21404"/>
        <source>Regenerate Alpine GRUB configuration</source>
        <translation>Восстановление конфигурации Alpine GRUB</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19929"/>
        <location filename="../src/MainWindow.cpp" line="21374"/>
        <source>Update GRUB2 configuration</source>
        <translation>Обновление конфигурации GRUB2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19931"/>
        <location filename="../src/MainWindow.cpp" line="21377"/>
        <source>Reinstall GRUB2 bootloader</source>
        <translation>Reinstall загрузчик GRUB2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19938"/>
        <location filename="../src/MainWindow.cpp" line="21448"/>
        <source>Rebuild initramfs (dracut)</source>
        <translation>Восстановление initramfs (dracut)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19966"/>
        <source>%1. %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19968"/>
        <source>Execution order %1: %2</source>
        <translation>Исполнительный приказ %1: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19974"/>
        <source>No Full Repair stages selected — use Configure Plan… or Settings.</source>
        <translation>Не выбраны этапы полного ремонта - используйте конфигурацию Plan… или настройки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19979"/>
        <source>1 stage selected</source>
        <translation>1 этап выбранный</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19980"/>
        <source>%1 stages selected</source>
        <translation>Выбранные стадии %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20012"/>
        <source>No repair stages are selected. Use Configure Plan… to choose the stages Full Repair will run.</source>
        <translation>Стадии ремонта не выбираются. Используйте Configure Plan…, чтобы выбрать этапы полного ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20013"/>
        <source>No selected stages are available. %1</source>
        <translation>Отобранные этапы отсутствуют. %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20020"/>
        <source> Please regenerate diagnostics before starting another repair.</source>
        <translation>Пожалуйста, восстановите диагностику перед началом ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20023"/>
        <source>Ready: required cached read-only diagnostics are available for the selected stages. Review them in Diagnostics or Logs before confirming.</source>
        <translation>Готовы: для выбранных этапов доступна необходимая кэшированная диагностика только для чтения. Просмотрите их в диагностике или журналах, прежде чем подтвердить.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20029"/>
        <location filename="../src/MainWindow.cpp" line="22167"/>
        <source>No Full Repair stages are selected.</source>
        <translation>Полноценный ремонт не подбирается.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20048"/>
        <source>Disabled in Settings</source>
        <translation>Инвалиды в настройках</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20055"/>
        <source>Enabled in Settings</source>
        <translation>Включено в настройках</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20056"/>
        <source>Disabled in Settings — enable it to include this stage</source>
        <translation>Инвалиды в настройках — позволяют включить этот этап</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20060"/>
        <source>Always preflight</source>
        <translation>Всегда предполетный</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20095"/>
        <source>Manual recovery tool</source>
        <translation>Ручной инструмент восстановления</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20100"/>
        <source>Unavailable: %1</source>
        <translation>Недоступно: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20249"/>
        <source>Orphaned headers (no matching kernel): %1.</source>
        <translation>Орфанированные заголовки (без соответствующего ядра): %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20417"/>
        <source>Select a tool to review its workflow.</source>
        <translation>Выберите инструмент для просмотра его рабочего процесса.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20440"/>
        <source>Check the running host or selected repair system&apos;s mounts, filesystem metadata, boot files, mapper consistency and dependency readiness before any repair action. This is an independent safety preflight rather than an optional Full Repair stage.</source>
        <translation>Проверьте работающий хост или выбранные установки системы ремонта, метаданные файловой системы, загрузочные файлы, согласованность картографа и готовность к зависимости перед любым действием по ремонту. Это самостоятельный предполет безопасности, а не факультативный этап полного ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20443"/>
        <source>Validate</source>
        <translation>подтверждать</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20445"/>
        <source>Full Repair: automatic safety preflight</source>
        <translation>Полный ремонт: автоматическая предполетная безопасность</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20448"/>
        <source>Complete interrupted dpkg package configuration in the running host or selected repair system. This is the same stage controlled by Settings → Full Repair plan → Complete interrupted package configuration, but it can also be run independently here.</source>
        <translation>Полная прерываемая конфигурация пакета dpkg в запущенном хосте или выбранной системе ремонта. Эта же стадия управляется планом полного ремонта Settings → Full Repair Plan → Complete с прерванной конфигурацией пакета, но она также может быть запущена самостоятельно.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20450"/>
        <source>Complete Configuration</source>
        <translation>Полная конфигурация</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20452"/>
        <location filename="../src/MainWindow.cpp" line="20463"/>
        <location filename="../src/MainWindow.cpp" line="20474"/>
        <location filename="../src/MainWindow.cpp" line="20485"/>
        <location filename="../src/MainWindow.cpp" line="20492"/>
        <location filename="../src/MainWindow.cpp" line="20511"/>
        <location filename="../src/MainWindow.cpp" line="20531"/>
        <location filename="../src/MainWindow.cpp" line="20556"/>
        <location filename="../src/MainWindow.cpp" line="20571"/>
        <location filename="../src/MainWindow.cpp" line="20578"/>
        <location filename="../src/MainWindow.cpp" line="20587"/>
        <source>Full Repair plan: %1</source>
        <translation>Полный ремонт: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20455"/>
        <source>Repair package dependencies in the running host or selected repair system after the mandatory safety preflight. Debian/Ubuntu uses APT; Arch uses one sandbox-preflighted full pacman transaction; Alpine uses apk fix with a simulation first. This maps directly to Settings → Repair broken package dependencies.</source>
        <translation>Зависимости пакета ремонта в запущенном хосте или выбранной системе ремонта после обязательного предполетного полета. Debian/Ubuntu использует APT; Arch использует одну предполетную транзакцию pacman; Alpine сначала использует исправление apk с симуляцией. Это отображается непосредственно в настройках → Ремонт сломанных зависимостей пакета.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20461"/>
        <source>Repair Dependencies</source>
        <translation>Ремонт зависимостей</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20466"/>
        <source>Refresh Debian/Ubuntu APT metadata in the running host or selected repair system without upgrading installed packages. Arch and Alpine deliberately refuse a partial metadata-only transaction; their guarded upgrade refreshes the package index itself. This maps directly to Settings → Refresh package metadata.</source>
        <translation>Обновите метаданные Debian/Ubuntu APT в запущенном хосте или выбранной системе ремонта без обновления установленных пакетов. Arch и Alpine сознательно отказываются от частичной транзакции только с метаданными; их защищенное обновление обновляет сам индекс пакетов. Эта карта отображается непосредственно в метаданных пакета Settings → Refresh.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20472"/>
        <source>Refresh Metadata</source>
        <translation>Обновление метаданных</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20477"/>
        <source>Simulate the distribution&apos;s package transaction first, inspect proposed removals, then apply a safe upgrade. Debian/Ubuntu chooses an APT mode; Arch runs one full pacman transaction; Alpine runs one guarded apk upgrade transaction. This maps directly to Settings → Upgrade installed packages.</source>
        <translation>Сначала имитируйте транзакцию пакета дистрибуции, проверьте предлагаемые удаления, а затем примените безопасное обновление. Debian/Ubuntu выбирает режим APT; Arch запускает одну полную транзакцию pacman; Alpine запускает одну защищенную транзакцию обновления apk. Эта карта отображается непосредственно в установленных пакетах Settings → Upgrade.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20483"/>
        <source>Simulate and Upgrade</source>
        <translation>Моделирование и обновление</translation>
    </message>
    <message>
        <source>Rebuild out-of-tree kernel modules for kernels installed in the running host or selected repair system. The helper refuses this action when DKMS is not installed in the selected system.</source>
        <translation type="vanished">Восстановление модулей ядра из дерева для ядер, установленных в работающем хосте или выбранной системе ремонта. Помощник отказывается от этого действия, когда DKMS не установлена в выбранной системе.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17235"/>
        <source>Rebuild DKMS</source>
        <translation>Восстановление DKMS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20495"/>
        <source>Restore the display manager identified from the running host or selected repair system&apos;s boot evidence and configuration (for example SDDM, GDM/GDM3, LightDM, or another systemd manager), set graphical.target as the default, and repair display-manager.service. </source>
        <translation>Восстановление дисплея дисплея, идентифицированного из запущенного хоста или выбранной системы восстановления загрузочных данных и конфигурации (например, SDDM, GDM/GDM3, LightDM или другого системного менеджера), задается графическим. Цель как по умолчанию, так и ремонт дисплея-менеджера. обслуживание.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20498"/>
        <source>The detected backend is GDM (Fedora naming); its configuration lives in /etc/gdm/custom.conf. </source>
        <translation>Обнаруженный бэкэнд - GDM (название Fedora); его конфигурация живет в /etc/gdm/custom.conf.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20501"/>
        <source>The detected backend is GDM3; its configuration lives in /etc/gdm3/daemon.conf. </source>
        <translation>Обнаруженный бэкэнд - GDM3; его конфигурация живет в /etc/gdm3/daemon.conf.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20504"/>
        <location filename="../src/MainWindow.cpp" line="20550"/>
        <source>The detected backend is %1. </source>
        <translation>Обнаруженный бэкэнд — %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20506"/>
        <source>When OpenRC is the detected service manager, the same action restores the detected manager&apos;s default runlevel symlink without starting it. Boot Bitch deliberately does not start a graphical session inside a repair chroot or host helper; use Diagnostics to inspect installed packages, service configuration, and recent boot/journal evidence first when graphical boot fails.</source>
        <translation>Когда OpenRC является обнаруженным диспетчером службы, то это же действие восстанавливает обнаруженный менеджер по умолчанию, не запуская его. Boot Bitch намеренно не запускает графическую сессию внутри ремонтного сервера или помощника хоста; используйте Диагностику для проверки установленных пакетов, конфигурации службы и недавних доказательств загрузки / журнала, когда графическая загрузка выходит из строя.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20509"/>
        <source>Restore Graphical Login</source>
        <translation>Восстановить графический логин</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20523"/>
        <source>Rebuild initramfs images for the running host or selected repair system only after mapper and crypttab consistency checks pass. The helper uses update-initramfs on Debian/Ubuntu, transaction-specific mkinitcpio trials on Arch, and mkinitfs trials on Alpine.</source>
        <translation>Восстановите изображения initramfs для работающего хоста или выбранной системы ремонта только после прохождения проверки согласованности картографа и crypttab. Помощник использует update-initramfs на Debian/Ubuntu, mkinitcpio на Arch и mkinitfs на Alpine.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20529"/>
        <source>Rebuild Initramfs</source>
        <translation>Восстановление Initramfs</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20544"/>
        <source>The detected syslinux/extlinux layout is repaired through its configuration and boot code, never through EFI. </source>
        <translation>Обнаруженная компоновка syslinux/extlinux восстанавливается через конфигурацию и загрузочный код, а не через EFI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20554"/>
        <source>Repair EFI / UKI</source>
        <translation>Ремонт EFI/UKI</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20559"/>
        <source>GRUB2 configuration</source>
        <translation>Конфигурация GRUB2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20563"/>
        <source>Regenerate GRUB2</source>
        <translation>Регенерировать RUB2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20566"/>
        <source>Regenerate the running host or selected repair system&apos;s GRUB menu/configuration after the mandatory safety preflight. Debian/Ubuntu uses update-grub; Arch and Alpine use grub-mkconfig with an isolated trial output, an entry-preserving guard and a rollback when a previous menu entry would be lost. This does not reinstall EFI loader files; use EFI / UKI bootloader when the firmware loader itself needs repair.</source>
        <translation>Восстановите меню/конфигурацию GRUB запущенного хоста или выбранной системы ремонта после обязательного предварительного полета. Debian/Ubuntu использует update-grub; Arch и Alpine используют grub-mkconfig с изолированным пробным выходом, защитой для сохранения входа и откатом, когда предыдущая запись меню будет потеряна. Это не переустанавливает файлы загрузчика EFI; используйте загрузчик EFI / UKI, когда загрузчик прошивки сам нуждается в ремонте.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20568"/>
        <source>Regenerate GRUB</source>
        <translation>Регенерировать GRUB</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20574"/>
        <source>Regenerate the running host or selected repair system&apos;s extlinux bootloader configuration after the mandatory safety preflight. The detected syslinux/extlinux backend uses update-extlinux with an entry-preserving guard and a boot-artifact backup; existing boot entries are never dropped. This regenerates the configuration only and does not reinstall bootloader files.</source>
        <translation>Восстановите конфигурацию загрузчика extlinux для запуска или выбранной системы ремонта после обязательного предварительного полета. Обнаруженный бэкэнд syslinux / extlinux использует обновление-extlinux с защитой для сохранения входа и резервной копией загрузочного артефакта; существующие загрузочные записи никогда не сбрасываются. Это восстанавливает только конфигурацию и не переустанавливает файлы загрузчика.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20576"/>
        <source>Regenerate extlinux</source>
        <translation>Регенерировать extlinux</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20585"/>
        <source>Check File Systems</source>
        <translation>Проверьте файловые системы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20599"/>
        <source>Reconcile Boot Stack</source>
        <translation>Обсуждение Boot Stack</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20601"/>
        <source>Full Repair plan: Manual recovery tool</source>
        <translation>Полный план ремонта: инструмент ручного восстановления</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20608"/>
        <source>Repair kernels</source>
        <translation>Ремонт ядер</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20612"/>
        <source>Unknown repair tool</source>
        <translation>Неизвестный ремонтный инструмент</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20613"/>
        <source>No repair workflow is registered for this item.</source>
        <translation>Ремонтные работы по данному пункту не регистрируются.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20614"/>
        <source>Unavailable</source>
        <translation>Недоступный</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20616"/>
        <source>Full Repair plan: unavailable</source>
        <translation>Полный план ремонта: недоступен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20640"/>
        <source>
Reuse: the bootloader repair from this session will be reused (the duplicate bootloader rebuild and GRUB regeneration are skipped).</source>
        <translation>
Повторное использование: восстановление загрузчика с этого сеанса будет повторно использовано (пропущена реконструкция дубликата загрузчика и регенерация GRUB).</translation>
    </message>
    <message>
        <source>
Unavailable: %1</source>
        <translation type="vanished">
Недоступно: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21307"/>
        <source> This stage is off by default: select it here to include it in Full Repair.</source>
        <translation>Этот этап выключен по умолчанию: выберите его здесь, чтобы включить его в полный ремонт.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21384"/>
        <source>Repair broken package dependencies (all detected package managers)</source>
        <translation>Ремонт сломанных зависимостей пакетов (все обнаруженные менеджеры пакетов)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21387"/>
        <source>Upgrade installed packages (all detected package managers)</source>
        <translation>Обновление установленных пакетов (все обнаруженные менеджеры пакетов)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21394"/>
        <source>Repair Alpine packages (apk fix)</source>
        <translation>Ремонт альпийских упаковок (apk)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21397"/>
        <source>Upgrade Alpine packages (apk upgrade)</source>
        <translation>Обновление альпийских пакетов (apk)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21413"/>
        <source>Repair Arch package dependencies (full pacman transaction)</source>
        <translation>Зависимость пакета Arch (полная транзакция pacman)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21416"/>
        <source>Upgrade Arch packages (full pacman transaction)</source>
        <translation>Обновление пакетов Arch (полная транзакция pacman)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21424"/>
        <source>Repair Fedora packages (dnf)</source>
        <translation>Ремонт пакетов Fedora (dnf)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21427"/>
        <source>Refresh package metadata (dnf makecache)</source>
        <translation>Обновление метаданных пакета (dnf makecache)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21430"/>
        <source>Upgrade Fedora packages (dnf upgrade)</source>
        <translation>Обновление пакетов Fedora (dnf)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21435"/>
        <source>Restore detected graphical login manager (OpenRC runlevel)</source>
        <translation>Восстановление обнаруженного графического менеджера входа в систему (OpenRC runlevel)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21443"/>
        <source>Restore detected graphical login manager (%1)</source>
        <translation>Восстановление обнаруженного графического менеджера входа (%1)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21452"/>
        <source>Rebuild initramfs (mkinitfs)</source>
        <translation>Восстановление initramfs (mkinitfs)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21456"/>
        <source>Rebuild initramfs (mkinitcpio)</source>
        <translation>Восстановление initramfs (mkinitcpio)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21461"/>
        <location filename="../src/MainWindow.cpp" line="22039"/>
        <source>Regenerate extlinux configuration</source>
        <translation>Регенерировать конфигурацию extlinux</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21482"/>
        <source>The running host is protected and cannot be repaired from itself.</source>
        <translation>Бегущий хост защищен и не может быть отремонтирован сам от себя.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21486"/>
        <source>The selected Linux root is still LUKS-encrypted. Unlock it first, refresh devices, then select the mapped filesystem.</source>
        <translation>Выбранный корень Linux по-прежнему зашифрован LUKS. Сначала разблокируйте его, обновите устройства, затем выберите отображенную файловую систему.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21490"/>
        <source>No mountable Linux root filesystem has been identified on the selected target.</source>
        <translation>Ни одна установленная корневая файловая система Linux не была идентифицирована по выбранной цели.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21501"/>
        <source>pkexec/Polkit is required to authorize repair operations.</source>
        <translation>Для проведения ремонтных работ требуется pkexec/Polkit.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21521"/>
        <source>The running host disk and root component could not be resolved.</source>
        <translation>Запущенный хост-диск и корневой компонент не могли быть решены.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21528"/>
        <source>The selected host identity is no longer marked as the protected running system. Refresh devices before retrying.</source>
        <translation>Выбранный идентификатор хоста больше не помечается как защищенная работающая система. Обновите устройства перед повторным использованием.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21538"/>
        <source>pkexec/Polkit is required to authorize host maintenance.</source>
        <translation>pkexec/Polkit требуется для авторизации обслуживания хоста.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21543"/>
        <source>Running host is ready for explicit guarded maintenance.</source>
        <translation>Запуск хоста готов к явному охраняемому обслуживанию.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21551"/>
        <source>Select Host Maintenance on the protected running-host card first.</source>
        <translation>Сначала выберите обслуживание хоста на защищенной карте бегущего хоста.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21564"/>
        <source>Confirm changes to the running host</source>
        <translation>Подтвердить изменения в бегущем хосте</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21565"/>
        <source>Host disk: %1
Linux root: %2

The following guarded changes will run natively on the active system:
• %3

The helper independently re-checks that the selected disk backs /, /boot and /boot/efi, and applies the same package, mapper, EFI/NVRAM and GRUB preservation safeguards used for repair targets.</source>
        <translation>Диск хоста: %1
Корень Linux: %2

Следующие охраняемые изменения будут работать на активной системе:
• %3

Помощник самостоятельно перепроверяет, что выбранный диск поддерживает /, /boot и /boot/efi, и применяет тот же пакет, картограф, EFI/NVRAM и GRUB защитные средства, используемые для ремонта целей.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21572"/>
        <source>Confirm changes to the selected repair system</source>
        <translation>Подтвердить изменения в выбранной системе ремонта</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21573"/>
        <source>Target disk: %1
Linux root: %2

The following target-system changes will run:
• %3

The running host is independently re-checked and refused by the privileged helper.</source>
        <translation>Целевой диск: %1
Корень Linux: %2

Будут проведены следующие изменения в целевой системе:
• %3

Ведущий самостоятельно перепроверяется и отказывает привилегированному помощнику.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21582"/>
        <source>Run Repair</source>
        <translation>Запуск ремонта</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21601"/>
        <source>Repair unavailable</source>
        <translation>Ремонт недоступен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21689"/>
        <source>Repair: %1…</source>
        <translation>Ремонт: %1…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21868"/>
        <source>%1 failed</source>
        <translation>%1 провалился</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21869"/>
        <source>The repair action failed. Review the captured helper output in Logs for the failing stage and its reason.</source>
        <translation>Ремонтные работы провалились. Просмотрите захваченный вывод помощника в журналах для стадии отказа и его причины.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21944"/>
        <source>A file system check and repair is already running. Wait for it to finish before starting another repair; this request was not queued.</source>
        <translation>Проверка и ремонт файловой системы уже запущены. Подождите, пока он закончит, прежде чем начинать очередной ремонт; эта просьба не стояла в очереди.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21948"/>
        <source>File system repair in progress</source>
        <translation>Ремонт файловой системы продолжается</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21966"/>
        <location filename="../src/MainWindow.cpp" line="22174"/>
        <source>Repair diagnostics required</source>
        <translation>Требуется ремонтная диагностика</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8617"/>
        <location filename="../src/MainWindow.cpp" line="21970"/>
        <location filename="../src/MainWindow.cpp" line="22076"/>
        <source>Repair tool unavailable</source>
        <translation>Ремонтный инструмент недоступен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5389"/>
        <source>Device: %1 · File system: %2 · Mount: %3 · Check tool: %4</source>
        <translation>Устройство: %1 · Файловая система: %2 · Гора: %3 · Инструмент проверки: %4</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6149"/>
        <location filename="../src/MainWindow.cpp" line="9868"/>
        <source>Host Maintenance — run guarded repair stages against the running system.</source>
        <translation>Техническое обслуживание хоста — запускать охраняемые этапы ремонта против запущенной системы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6176"/>
        <source>Click a column header to sort by it; click again to reverse the order.</source>
        <translation>Нажмите на заголовок столбца, чтобы сортировать по нему; нажмите снова, чтобы изменить порядок.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6267"/>
        <source>Establish the privileged helper session for the current scope now.</source>
        <translation>Установите привилегированную сессию помощника для текущего объема сейчас.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6489"/>
        <source>Run the selected read-only diagnostic for the current scope.</source>
        <translation>Запустите выбранную диагностику только для чтения для текущего объема.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7167"/>
        <source>This entry is the current GRUB legacy default.</source>
        <translation>Эта запись является текущим по умолчанию наследием GRUB.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7199"/>
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
        <translation>Управляйте загрузкой прошивки UEFI. Записи перечислены только для чтения; Переместите вверх / Переместите вниз переупорядочив их только в таблице, Удалите Выбранный отмечает запись для удаления и двойного щелчка столбца Label этапы переименования. Save совершает все поэтапные изменения (заказ, удаление и переименование) через проверенного помощника. Активная запись (BootCurrent) никогда не может быть удалена или переименована. Изменения проверяются и откатываются от любого несоответствия.

Загрузочные записи прошивки живут в UEFI NVRAM (есть только одна прошивка). В обслуживании хоста вы управляете собственными записями запущенного хоста; с выбранной целью ремонта вы управляете записями этого диска, которые являются записями NVRAM, PARTUUID которых указывает на системный раздел EFI этого диска. Цель ремонта не имеет собственной прошивки.

Это записи прошивки UEFI; меню загрузки GRUB (список ядер / ОС внутри GRUB) отдельно и регенерируется ремонтом GRUB или загрузочного стека (примирение).

Класс:
• host — загрузка входа в систему EFI собственного хоста.
Цель • — загрузка входа выбранной ремонтной цели EFI системного раздела (показана в Host Maintenance только при выборе цели).
• foreign — точки входа на другой диск или раздел EFI (другая ОС, USB-накопитель или старый диск).
• неизвестен — путь входа в устройство не может быть разрешен к разделу GPT (обычно встроенные прошивки, такие как меню настройки).

Общие имена погрузчиков:
• EFI/BOOT/BOOTX64.EFI — запасной вариант съемных носителей; прошивка по умолчанию использует для загрузки USB-накопителя, не привязанного к одной ОС.
• iPXE.efi — загрузчик сетевого восстановления WebFAI (TUXEDO OS); загрузка его запускает среду восстановления/установки сети.
• oem##.efi — загрузочные приложения производителя/OEM (восстановление или диагностика); не являются частью загрузочной цепи ОС и остаются нетронутыми.
• EFI/BOOT/TUX.EFI — унифицированное изображение ядра TUXEDO OS (UKI).

Распределительные погрузчики: Arch EFI/arch/grubx64.efi, Alpine (EFI) EFI/alpine/grubx64.efi, Fedora EFI/fedora/shimx64.efi (тогда grubx64.efi), Ubuntu/Debian EFI/ubuntu/shimx64.efi; shim*.efi - это Secure-Boot shim. Debian 4.0 «Etch» является только BIOS и не имеет загрузочных записей .efi.

Меню загрузки (только для чтения):
Используйте селектор просмотра выше, чтобы переключаться между входами прошивки UEFI / UKI и меню загрузки. В меню загрузки перечислены ядра / записи ОС загрузчика системы точно так же, как они отображаются в его конфигурации, в порядке файла, с текущей записью по умолчанию, отмеченной ✓. На системах GRUB это меню GRUB (grub.cfg); на системах BIOS/extlinux это меню extlinux/syslinux (/boot/extlinux.conf или syslinux.cfg); на Debian Etch (GRUB legacy 0.97) это меню.lst (/boot/grub/menu.lst). Этот вид строго читается — он никогда не регенерирует и не редактирует конфигурацию загрузчика.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7752"/>
        <source>No boot menu configuration was found on this drive. The system may have no GRUB, extlinux/syslinux or GRUB legacy boot menu, or its configuration lives elsewhere.</source>
        <translation>На этом диске не было найдено никакой конфигурации меню загрузки. Система может не иметь меню загрузки GRUB, extlinux/syslinux или GRUB, или ее конфигурация живет в другом месте.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9773"/>
        <source>Inspect this partition/volume. %1</source>
        <translation>Проверьте этот раздел/объем. %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9778"/>
        <source>Select this physical drive as the repair target. %1</source>
        <translation>Выберите этот физический привод в качестве цели ремонта. %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9923"/>
        <source>Exit Host Maintenance — return to ordinary repair-target mode.</source>
        <translation>Exit Host Maintenance — возврат к обычному режиму ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10165"/>
        <source>Unlock %1 via cryptsetup through the privileged helper; the passphrase is never logged.</source>
        <translation>Разблокируйте %1 с помощью cryptsetup через привилегированного помощника; парольная фраза никогда не регистрируется.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10168"/>
        <source>An unlocked Linux filesystem is already visible; Boot Bitch reuses the existing mapper.</source>
        <translation>Разблокированная файловая система Linux уже видна; Boot Bitch повторно использует существующий картограф.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11057"/>
        <source>Privileged request &apos;%1&apos; is queued behind the running &apos;%2&apos; and will start when it finishes.</source>
        <translation>Привилегированный запрос «%1» стоит в очереди за запущенным «%2» и начинается, когда он заканчивается.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11086"/>
        <source>Privileged request &apos;%1&apos; waited for the running &apos;%2&apos; but it did not finish in time; the request was refused.</source>
        <translation>Привилегированный запрос «%1» ждал запуска «%2», но он не закончился вовремя.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11089"/>
        <source>ERROR: Privileged request &apos;%1&apos; was refused because &apos;%2&apos; is still running after the queue wait limit; wait for it to finish and retry, or cancel it.
</source>
        <translation>В привилегированном запросе «%1» было отказано, потому что «%2» все еще работает после лимита ожидания очереди; дождитесь, пока он закончит и повторит или отменит его.
</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11961"/>
        <source>Running host selected for guarded maintenance: %1 · Root component: %2</source>
        <translation>Запуск хоста, выбранного для охранного обслуживания: %1 · Корневой компонент: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11968"/>
        <source> · Automatically detected system component: %1</source>
        <translation>Автоматически обнаруженный компонент системы: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11986"/>
        <source>Running host selected for the Host Shell: %1 · Root component: %2</source>
        <translation>Запуск хоста, выбранного для хоста Shell: %1 · Корневой компонент: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12014"/>
        <source>Row selection is inspection only; press Select Target to commit a repair drive.</source>
        <translation>Выбор строки - это только проверка; нажмите Select Target, чтобы выполнить ремонтный привод.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12015"/>
        <source>Committed repair target: Repair, Diagnostics, Snapshots and File Copy target this drive until Select Target picks another. %1</source>
        <translation>Цель ремонта: ремонт, диагностика, снимки и копирование файлов нацелены на этот диск, пока выбранная цель не выберет другую. %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13203"/>
        <source>Open a read-only folder view; the target is unmounted after each listing.</source>
        <translation>Откройте вид папки только для чтения; цель не установлена после каждого листинга.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14314"/>
        <source>Cached %1 result; use Re-run for fresh data.</source>
        <translation>Результат кэширования %1; использование повторного запуска для свежих данных.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21302"/>
        <source>No cached capability evidence yet; your saved selection is kept and re-checked after diagnostics.</source>
        <translation>Доказательств кэшированных возможностей пока нет; сохраненный выбор сохраняется и перепроверяется после диагностики.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21975"/>
        <source>Validate running host</source>
        <translation>Проверка бегущего хоста</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21980"/>
        <source>Validate repair target</source>
        <translation>Цель валидного ремонта</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21994"/>
        <location filename="../src/MainWindow.cpp" line="22181"/>
        <location filename="../src/MainWindow.cpp" line="22582"/>
        <source>running host</source>
        <translation>бегущий хост</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21995"/>
        <location filename="../src/MainWindow.cpp" line="22182"/>
        <location filename="../src/MainWindow.cpp" line="22583"/>
        <source>selected repair system</source>
        <translation>Выбранная система ремонта</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="21999"/>
        <source>Complete interrupted package configuration in the %1 with dpkg --configure -a</source>
        <translation>Полная конфигурация прерванного пакета в %1 с dpkg -configure -a</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22003"/>
        <source>Repair broken APT package dependencies in the %1</source>
        <translation>Ремонт сломанных зависимостей пакета APT в %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22007"/>
        <source>Refresh APT package metadata in the %1</source>
        <translation>Обновить метаданные пакета APT в %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22011"/>
        <source>Run the distribution-specific transaction preflight, then choose a safe upgrade for the %1</source>
        <translation>Запустите предполетную транзакцию для конкретной дистрибуции, а затем выберите безопасное обновление для %1.</translation>
    </message>
    <message>
        <source>Run DKMS autoinstall in the %1</source>
        <translation type="vanished">Запустите автоинсталляцию DKMS в %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22019"/>
        <source>Set graphical.target as the %1&apos;s default boot target</source>
        <translation>Установите graphical.target в качестве загрузочной цели по умолчанию %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22020"/>
        <source>Enable the detected display manager and repair display-manager.service without starting it inside a chroot or host helper</source>
        <translation>Включите обнаруженный дисплеем менеджер и ремонт дисплея-менеджера. сервис, не запуская его внутри chroot или помощника хоста</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22022"/>
        <source>Rebuild initramfs</source>
        <translation>Реконструкция initramfs</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22024"/>
        <source>Trial-build and then rebuild all initramfs images for the %1</source>
        <translation>Пробная сборка, а затем восстановление всех изображений initramfs для %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22026"/>
        <source>Repair EFI / UKI bootloader</source>
        <translation>Ремонт загрузчика EFI/UKI</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22030"/>
        <location filename="../src/MainWindow.cpp" line="22054"/>
        <source>detected bootloader</source>
        <translation>обнаруженный загрузчик</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22032"/>
        <source>Repair the %1&apos;s %2 boot path through its guarded installer or vendor builder, preserving every other boot entry</source>
        <translation>Отремонтируйте путь загрузки %1 %2 через своего охраняемого установщика или конструктора поставщиков, сохраняя каждый другой вход в загрузку.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22033"/>
        <source>Restore one verified default-loader entry when the guarded installer only wrote files</source>
        <translation>Восстановите одну верифицированную запись по умолчанию, когда защищенный установщик писал только файлы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2101"/>
        <location filename="../src/MainWindow.cpp" line="22035"/>
        <source>Regenerate GRUB configuration</source>
        <translation>Регенерировать конфигурацию GRUB</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20458"/>
        <source> Fedora/RPM restores missing or corrupt package files via one simulated dnf reinstall transaction.</source>
        <translation>Fedora/RPM восстанавливает отсутствующие или поврежденные файлы пакетов с помощью одной смоделированной транзакции переустановки dnf.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20469"/>
        <source> Fedora uses dnf makecache as its standalone metadata stage.</source>
        <translation>Fedora использует dnf в качестве отдельной стадии метаданных.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20480"/>
        <source> Fedora runs one guarded dnf upgrade transaction after a removal- and downgrade-free, signature-checked simulation.</source>
        <translation>Fedora проводит одну защищенную транзакцию обновления dnf после симуляции без удаления и понижения рейтинга.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20514"/>
        <source>Walk an evidence-guided interactive session to repair the kernel GPU driver (nvidia/nouveau/amdgpu/radeon/mesa), applying only the small guarded step you choose with its own rollback. Two decision points guide the session: kernel-vs-driver (replace the newest kernel the driver cannot build against, or remove the older kernel the driver is pinned to) and wrong-driver (purge the incorrect driver, install the correct one, and rebuild it against every installed kernel). A separate one-boot nomodeset fallback reaches a console on a broken driver.</source>
        <translation>Пройдите интерактивную сессию, управляемую доказательствами, чтобы отремонтировать драйвер графического процессора ядра (nvidia/nouveau/amdgpu/radeon/mesa), применяя только небольшой защищенный шаг, который вы выбираете, с собственным откатом. Два пункта принятия решения направляют сессию: ядро-vs-водитель (заменить новейшее ядро, против которого водитель не может построить, или удалить старое ядро, к которому прикреплен водитель) и неправильный водитель (очистить неправильный водитель, установить правильный и восстановить его против каждого установленного ядра). Отдельный однозарядный номодесетный запасной достигает консоли на сломанном драйвере.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20526"/>
        <source> The detected dracut backend pairs each installed kernel with its /boot vmlinuz and initramfs image, verifies a trial build with lsinitrd, and backs up each image so a failed verification restores it.</source>
        <translation>Обнаруженный бэкэнд dracut соединяет каждое установленное ядро с его изображением /boot vmlinuz и initramfs, проверяет пробную сборку с lsinitrd и резервирует каждое изображение, поэтому неудачная проверка восстанавливает его.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20535"/>
        <source>Repair the running host or selected repair system&apos;s boot path with the detected backend&apos;s guarded installer or vendor builder, restoring one verified default-loader entry and preserving every other ESP&apos;s firmware entries and BootOrder. </source>
        <translation>Отремонтируйте путь загрузки запущенного хоста или выбранной системы ремонта с помощью защищенного установщика или конструктора поставщика обнаруженного бэкэнда, восстановив одну верифицированную запись по умолчанию и сохранив все другие записи прошивки ESP и BootOrder.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20538"/>
        <source>The detected vendor UKI layout (for example the TUXEDO TUX.EFI builder) is rebuilt or re-registered through its official builder. </source>
        <translation>Обнаруженная вендором компоновка UKI (например, TUXEDO TUX.EFI builder) перестраивается или перерегистрируется через своего официального конструктора.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20541"/>
        <source>The detected Fedora GRUB2/BLS backend repairs the boot code on a BIOS layout, leaving /boot/grub2/grub.cfg and the BLS entries intact. </source>
        <translation>Обнаруженный сервер Fedora GRUB2/BLS восстанавливает код загрузки на макете BIOS, оставляя /boot/grub2/grub.cfg и записи BLS нетронутыми.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20547"/>
        <source>The detected GRUB EFI layout uses a guarded grub-install on the validated ESP and restores one verified vendor-loader firmware entry if the installer only writes files. </source>
        <translation>Обнаруженный макет GRUB EFI использует защищенный grub-install на проверенном ESP и восстанавливает одну верифицированную запись прошивки поставщика-загрузчика, если установщик пишет только файлы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20552"/>
        <source>A failed install restores the ESP backup; decoded entries keep their distribution/vendor label and drive model, and unrelated entries on other disks are never removed.</source>
        <translation>Неудачная установка восстанавливает резервную копию ESP; декодированные записи сохраняют свою ярлык дистрибутива / поставщика и модель диска, а несвязанные записи на других дисках никогда не удаляются.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20560"/>
        <source>Regenerate the running host or selected repair system&apos;s GRUB2 configuration (grub2-mkconfig --no-grubenv-update, /boot/grub2/grub.cfg with the BLS entries preserved) after the mandatory safety preflight, using an entry-preserving guard and a rollback so no menu or BLS entry is lost. When the read-only boot-code probe finds the MBR or BIOS boot partition broken, the same stage performs a guarded grub2-install --target=i386-pc boot-code reinstall with backup and rollback; it never writes firmware NVRAM or reinstalls EFI loader files — use EFI / UKI bootloader when the firmware loader itself needs repair.</source>
        <translation>Восстановите конфигурацию GRUB2 запущенного хоста или выбранной системы ремонта (grub2-mkconfig - no-grubenv-update, /boot/grub2/grub.cfg с сохраненными записями BLS) после обязательного предполетного полета с использованием предохранителя для сохранения входа и отката, чтобы не было потеряно меню или запись BLS. Когда зонд только для чтения загрузочного кода обнаруживает, что раздел загрузки MBR или BIOS сломан, на той же стадии выполняется защищенная переустановка загрузочного кода grub2 -target=i386-pc с резервным копированием и откатом; он никогда не пишет прошивки NVRAM или переустанавливает файлы загрузчика EFI - используйте загрузчик EFI / UKI, когда сам загрузчик прошивки нуждается в ремонте.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20581"/>
        <source>Run a read-only file system check on the selected system&apos;s root, /boot, ESP and /home filesystems, then repair only the devices that report errors. Detected tools are used per file system — e2fsck/xfs_repair and the other native checkers, btrfs/zpool scrub, and the FAT/exFAT/NTFS tools — with the risky btrfs check --repair gated behind a separate backup confirmation. Offline tools refuse mounted filesystems, the online modes run where available, the running host root is never repaired offline, and unsupported filesystems are reported rather than guessed about.</source>
        <translation>Запустите проверку файловой системы только для чтения на корневой /boot, ESP и /home файловых системах выбранной системы, а затем исправьте только устройства, которые сообщают об ошибках. Обнаруженные инструменты используются в файловой системе — e2fsck/xfs repair и другие нативные шашки, btrfs/zpool scrub и инструменты FAT/exFAT/NTFS — с рискованной проверкой btrfs — repair закрыты за отдельным подтверждением резервного копирования. Оффлайн-инструменты отказываются от установленных файловых систем, онлайн-режимы запускаются там, где это доступно, запущенный корень хоста никогда не восстанавливается в автономном режиме, а неподдерживаемые файловые системы сообщаются, а не угадываются.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20590"/>
        <source>Reconcile a repaired or restored root with its boot artifacts: validate mapper/crypttab, rebuild installed-kernel initramfs images, repair the detected bootloader path (vendor UKI builder, GRUB EFI, extlinux or Fedora BLS), reconcile one canonical default destination per purpose, and regenerate the detected bootloader configuration. </source>
        <translation>Согласуйте отремонтированный или восстановленный корень с его загрузочными артефактами: валидируйте Mapper / Cryptotab, восстанавливайте изображения установленного ядра initramfs, ремонтируйте обнаруженный путь загрузчика (разработчик UKI, GRUB EFI, extlinux или Fedora BLS), примиряйте один канонический пункт назначения по умолчанию для цели и восстанавливайте обнаруженную конфигурацию загрузчика.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20593"/>
        <source>On a Fedora BIOS target this validates mapper/crypttab, rebuilds the dracut initramfs images and regenerates the GRUB2 configuration (config-only; the guarded bootloader reinstall stays in the GRUB stage). </source>
        <translation>На мишени Fedora BIOS это проверяет Mapper /crypttab, восстанавливает изображения dracut initramfs и восстанавливает конфигурацию GRUB2 (только конфигурация; переустановка защищенного загрузчика остается на этапе GRUB).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20596"/>
        <source>When the EFI / UKI bootloader repair already ran for the same system in this session, this action reuses it (skipping the duplicate bootloader rebuild and GRUB regeneration) while mapper/crypttab validation and initramfs reconciliation still run. This is the focused recovery action for a root/boot mismatch after a partial update or snapshot restore; it never deletes kernels or unrelated boot entries.</source>
        <translation>Когда загрузчик EFI / UKI уже работал для той же системы в этом сеансе, это действие повторно использует его (пропуск дубликата восстановления загрузчика и регенерации GRUB), в то время как проверка картографа / шифрования и сверка initramfs все еще работают. Это целенаправленное действие восстановления для несоответствия корня / загрузки после частичного обновления или восстановления снимка; он никогда не удаляет ядра или несвязанные загрузочные записи.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="20604"/>
        <source>Review the installed kernel inventory and choose one concrete kernel repair action: remove the oldest/superseded or newest kernel, remove orphaned headers/components/build support, install missing headers, reinstall an incomplete kernel, or install the repository-latest kernel. Nothing is removed or modified until you pick an action and confirm it. The running kernel is always protected, and the remaining kernels&apos; initramfs images and the bootloader configuration are regenerated after a removal.</source>
        <translation>Просмотрите установленный инвентарь ядра и выберите одно действие по ремонту бетонного ядра: удалите старейшее / замененное или новейшее ядро, удалите осиротевшие заголовки / компоненты / поддержку сборки, установите недостающие заголовки, переустановите неполное ядро или установите ядро репозитория. Ничего не удаляется и не изменяется, пока вы не выберете действие и не подтвердите его. Работающее ядро всегда защищено, а оставшиеся изображения ядра initramfs и конфигурация загрузчика восстанавливаются после удаления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22015"/>
        <source>Rebuild out-of-tree kernel modules in the %1 using the detected %2 build system</source>
        <translation>Восстановление модулей ядра в %1 с использованием обнаруженной системы сборки %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22037"/>
        <source>Trial-generate and then regenerate the %1&apos;s GRUB configuration</source>
        <translation>Пробная генерация, а затем регенерация конфигурации %1 GRUB</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22041"/>
        <source>Regenerate the %1&apos;s extlinux bootloader configuration with an entry-preserving guard</source>
        <translation>Регенерировать конфигурацию загрузчика %1 extlinux с помощью защитного устройства для сохранения входа</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22043"/>
        <source>Reconcile boot stack</source>
        <translation>Обсуждение Boot Stack</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22056"/>
        <source>Validate mapper/crypttab against the %1</source>
        <translation>Валидатный картограф/криптаб против %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22057"/>
        <source>Trial-build and rebuild initramfs for installed kernels</source>
        <translation>Испытание и восстановление initramfs для установленных ядер</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22058"/>
        <source>Repair the detected %1 boot path, removing only duplicate default destinations</source>
        <translation>Ремонт обнаруженного пути загрузки %1, удаление только дублирующих пунктов назначения по умолчанию</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22059"/>
        <source>Regenerate the detected bootloader configuration</source>
        <translation>Восстановление обнаруженной конфигурации загрузчика</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22061"/>
        <source>Reuse the bootloader repair already completed for this %1: skip the duplicate bootloader rebuild and GRUB regeneration</source>
        <translation>Повторное использование уже завершенного ремонта загрузчика для этого %1: пропустите восстановление дубликата загрузчика и регенерацию GRUB</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22077"/>
        <source>No repair workflow is registered for the selected tool.</source>
        <translation>Ремонтный процесс для выбранного инструмента не регистрируется.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22088"/>
        <location filename="../src/MainWindow.cpp" line="22203"/>
        <source>Read-only diagnostics completed; review the full evidence in Logs before confirming repair.</source>
        <translation>Завершена диагностика только для чтения; просмотрите все доказательства в журналах, прежде чем подтвердить ремонт.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22138"/>
        <source>GPU driver repair refused: run the %1 diagnostic first (no cached GPU evidence).</source>
        <translation>Ремонт драйверов GPU отказался: сначала запустите диагностику %1 (без кэшированных доказательств GPU).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22140"/>
        <source>GPU driver repair unavailable</source>
        <translation>Ремонт драйверов GPU недоступен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22141"/>
        <source>Run the %1 diagnostic before starting the GPU driver repair.</source>
        <translation>Запустите диагностику %1 перед началом ремонта драйвера GPU.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22184"/>
        <source>Run the read-only file system check, then repair the selected devices</source>
        <translation>Запустите проверку файловой системы только для чтения, а затем отремонтируйте выбранные устройства</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22186"/>
        <source>Repair broken APT dependencies</source>
        <translation>Ремонт сломанных зависимостей APT</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22187"/>
        <source>Refresh APT package metadata</source>
        <translation>Обновить метаданные пакета APT</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22188"/>
        <source>Simulate APT first, then choose a safe upgrade/full-upgrade/dist-upgrade transaction</source>
        <translation>Сначала имитируйте APT, а затем выберите безопасную транзакцию обновления / полного обновления / отключения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22190"/>
        <source>Restore the detected display manager and graphical.target without starting the GUI inside chroot</source>
        <translation>Восстановление обнаруженного дисплея и графического. Цель без запуска GUI внутри chroot</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22191"/>
        <source>Rebuild all initramfs images</source>
        <translation>Восстановление всех изображений initramfs</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22192"/>
        <source>Repair the %1 EFI / UKI boot path using its validated ESP</source>
        <translation>Ремонт загрузочного пути %1 EFI / UKI с использованием проверенного ESP</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22193"/>
        <source>Regenerate the %1&apos;s GRUB configuration</source>
        <translation>Регенерировать конфигурацию %1 GRUB</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22194"/>
        <source>Regenerate the %1&apos;s extlinux configuration</source>
        <translation>Регенерировать конфигурацию %1 extlinux</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22289"/>
        <source>The Full Repair plan is running. The read-only file system check pre-stage runs first, then the selected repair stages; each stage&apos;s output streams here as it completes.</source>
        <translation>План полного ремонта работает. Сначала выполняется проверка файловой системы только для чтения, затем выбранные этапы восстановления; выходной поток каждого этапа здесь по мере его завершения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22307"/>
        <source>Full Repair plan finished with failures. Review the per-stage results (✓/✗/▪) in Logs, then close this window.</source>
        <translation>Полный ремонт закончился неудачами. Просмотрите результаты каждой стадии (✓/✗/▪) в журналах, затем закройте это окно.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22308"/>
        <source>Full Repair plan finished. Review the per-stage results (✓/✗/▪) in Logs, then close this window.</source>
        <translation>Полный план ремонта завершен. Просмотрите результаты каждой стадии (✓/✗/▪) в журналах, затем закройте это окно.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22507"/>
        <source>File system check unavailable</source>
        <translation>Проверка файловой системы недоступна</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22533"/>
        <source>Check File Systems (Full Repair pre-stage)</source>
        <translation>Проверка файловых систем (полный ремонт на стадии)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22534"/>
        <source>Check File Systems (manual)</source>
        <translation>Проверьте файловые системы (ручные)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22548"/>
        <source>File system check failed</source>
        <translation>Проверка файловой системы провалилась</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22549"/>
        <source>The read-only file system check failed. Review the captured helper output in Logs for the reason.</source>
        <translation>Проверка файловой системы только для чтения не удалась. Просмотрите захваченный вывод помощника в журналах по этой причине.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22565"/>
        <source>File system repair unavailable</source>
        <translation>Ремонт файловой системы недоступен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22578"/>
        <source>Checking and repairing file systems (Full Repair)</source>
        <translation>Проверка и ремонт файловых систем (полный ремонт)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22579"/>
        <source>Checking and repairing file systems</source>
        <translation>Проверка и ремонт файловых систем</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22629"/>
        <source>%1 mounted filesystem(s) were skipped because their check tools are offline-only</source>
        <translation>Файловая система (ы), установленная на %1, была пропущена, потому что их инструменты проверки отключены только</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22633"/>
        <source>%1 filesystem(s) are unsupported or have no installed check tool</source>
        <translation>Файловая система %1 не поддерживается или не имеет установленного инструмента проверки</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22637"/>
        <source>No file system errors were detected on the %1&apos;s root, /boot, ESP or /home filesystems. No repair was run.</source>
        <translation>Ошибки файловой системы не были обнаружены на %1 root, /boot, ESP или /home. Ремонт не проводился.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22639"/>
        <source>No repairable file system errors were detected on the %1&apos;s root, /boot, ESP or /home filesystems, but %2. No repair was run.</source>
        <translation>Никаких исправленных ошибок файловой системы не было обнаружено на корне %1, /boot, ESP или /home файловых системах, но %2. Ремонт не проводился.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22671"/>
        <source>The read-only check found issues on %1 mounted filesystem(s) that have no safe repair mode from this environment. Unmount the filesystem and check again, or repair it from a live system that is not using it.</source>
        <translation>Проверка только для чтения обнаружила проблемы в установленной файловой системе %1, которые не имеют безопасного режима восстановления из этой среды. Отсоедините файловую систему и проверьте снова или отремонтируйте ее из живой системы, которая ее не использует.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22704"/>
        <source>Run %1 repair on %2</source>
        <translation>Ремонт %1 на %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22727"/>
        <source>Dangerous Btrfs repair</source>
        <translation>Опасный ремонт Btrfs</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22728"/>
        <source>btrfs check --repair is a last-resort tool</source>
        <translation>Проверка btrfs - ремонт - инструмент последней инстанции</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22729"/>
        <source>Upstream Btrfs documentation warns that check --repair can make a damaged filesystem worse and can lose data. Back up everything you can reach first. Prefer a scrub, a rescue, or a fresh backup/restore when either is possible.

Run btrfs check --repair on:
%1</source>
        <translation>Документация Btrfs предупреждает, что исправление может ухудшить поврежденную файловую систему и привести к потере данных. Подкрепите все, что вы можете достичь в первую очередь. Предпочитаете скраб, спасение или свежее резервное копирование / восстановление, когда это возможно.

Проверка btrfs - ремонт:
%1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22735"/>
        <source>Run dangerous Btrfs repair</source>
        <translation>Опасный ремонт Btrfs</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22772"/>
        <source>File system repair (%1)</source>
        <translation>Ремонт файловой системы (%1)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22773"/>
        <source>File system repair (%1, manual)</source>
        <translation>Ремонт файловой системы (%1, ручной)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22811"/>
        <source>File system repair failed</source>
        <translation>Ремонт файловой системы провалился</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="22812"/>
        <source>One or more file system repairs failed. Review the captured helper output in Logs for the failing device and its reason.</source>
        <translation>Одно или несколько исправлений файловой системы не удались. Просмотрите захваченный вывод помощника в журналах для отказа устройства и его причины.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="23028"/>
        <source>Device columns auto-sized. Drag headers to fine-tune widths.</source>
        <translation>Колонны устройств автоматического размера. Перетаскивайте заголовки для точной настройки ширины.</translation>
    </message>
</context>
</TS>
