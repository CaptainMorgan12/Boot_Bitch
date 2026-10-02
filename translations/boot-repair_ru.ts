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
        <location filename="../src/MainWindow.cpp" line="666"/>
        <source>Environment validation</source>
        <translation>Проверка окружающей среды</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="666"/>
        <source>Summarizes the selected system, protection state, mounted identity and inspection readiness.</source>
        <translation>Обобщает выбранную систему, состояние защиты, установленную идентичность и готовность к проверке.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="667"/>
        <source>Distribution and boot backend profile</source>
        <translation>Профиль Backend и Boot Backend</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="667"/>
        <source>Identifies the distribution family, package manager, initramfs generator, bootloader, ESP location and current guarded repair capability.</source>
        <translation>Идентифицирует семейство дистрибутивов, менеджер пакетов, генератор initramfs, загрузчик, местоположение ESP и текущую охраняемую возможность ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="668"/>
        <location filename="../src/MainWindow.cpp" line="12685"/>
        <location filename="../src/MainWindow.cpp" line="12704"/>
        <source>Boot diagnostics</source>
        <translation>Диагностика бута</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="668"/>
        <source>Shows boot mounts, /boot and EFI contents plus storage evidence without changing the selected system.</source>
        <translation>Показывает загрузочные крепления, /boot и содержимое EFI, а также доказательства хранения без изменения выбранной системы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="669"/>
        <source>Boot evidence and selection history</source>
        <translation>Доказательства сапоги и история отбора</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="669"/>
        <source>Correlates the detected boot chain, bootloader selection, kernel/initramfs, snapshots, EFI and unlock evidence, including whether one or more LUKS prompts are expected.</source>
        <translation>Сопоставляет обнаруженную цепочку загрузки, выбор загрузчика, ядро / initramfs, снимки, EFI и доказательства разблокировки, в том числе, ожидается ли одна или несколько подсказок LUKS.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="670"/>
        <source>Kernel / initramfs</source>
        <translation>Ядро / initramfs</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="670"/>
        <source>Reviews kernel files and verifies matching initramfs images through a read-only inspection.</source>
        <translation>Обзор файлов ядра и проверка соответствия изображений initramfs с помощью проверки только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="671"/>
        <location filename="../src/MainWindow.cpp" line="5924"/>
        <location filename="../src/MainWindow.cpp" line="12684"/>
        <location filename="../src/MainWindow.cpp" line="12703"/>
        <location filename="../src/MainWindow.cpp" line="17423"/>
        <source>GRUB configuration</source>
        <translation>Конфигурация GRUB</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="671"/>
        <source>Reviews GRUB configuration and /etc/default/grub without changing boot files.</source>
        <translation>Обзор конфигурации GRUB и /etc/default/grub без изменения файлов загрузки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="672"/>
        <source>EFI / UKI boot state</source>
        <translation>EFI / UKI состояние загрузки</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="672"/>
        <source>Inspects the selected ESP, vendor or generic UKI images, systemd-boot loader files, embedded kernel/cmdline data and firmware entries with PARTUUID ownership classification.</source>
        <translation>Проверяет выбранные ESP, изображения поставщика или общие изображения UKI, файлы загрузчика systemd-boot, встроенные данные ядра / cmdline и записи прошивки с классификацией собственности PARTUUID.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="673"/>
        <location filename="../src/MainWindow.cpp" line="5921"/>
        <location filename="../src/MainWindow.cpp" line="17360"/>
        <source>Graphical login / display manager</source>
        <translation>Графический логин / дисплеев менеджер</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="673"/>
        <source>Reviews graphical.target, the configured display manager (for example SDDM, GDM/GDM3, LightDM, or another systemd manager), installed desktop packages, and recent boot/journal evidence without starting the GUI.</source>
        <translation>Обзоры graphical.target, настроенного дисплея (например, SDDM, GDM/GDM3, LightDM или другого системного менеджера), установленных пакетов рабочего стола и недавних доказательств загрузки/журнала без запуска GUI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="674"/>
        <source>Boot errors</source>
        <translation>Ошибки загрузки</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="674"/>
        <source>Reads recent error-priority entries from the running host or selected repair system&apos;s persistent journal when available.</source>
        <translation>Читает последние записи с приоритетом ошибки из постоянного журнала запущенного хоста или выбранной системы ремонта, когда это доступно.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="675"/>
        <source>Disk usage</source>
        <translation>Использование диска</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="675"/>
        <source>Summarizes filesystem capacity/free space for the running host or read-only repair target.</source>
        <translation>Обобщает емкость файловой системы / свободное пространство для запуска хоста или цели восстановления только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="676"/>
        <source>File systems</source>
        <translation>Файловые системы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="676"/>
        <source>Runs the read-only file system check for the running host or selected repair system&apos;s root, /boot, ESP and /home filesystems and reports each device&apos;s check tool and result without changing anything.</source>
        <translation>Запускает проверку файловой системы только для чтения для запуска хоста или выбранной системы восстановления root, /boot, ESP и /home файловых систем и сообщает инструмент проверки каждого устройства и результат, ничего не меняя.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="677"/>
        <source>fstab</source>
        <translation>нож</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="677"/>
        <source>Displays the running host or selected repair system&apos;s fstab; repair-system inspection is mounted read-only.</source>
        <translation>Отображает запущенный хост или выбранную систему ремонта fstab; проверка ремонтной системы устанавливается только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="678"/>
        <source>Btrfs status</source>
        <translation>Btrfs статус</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="678"/>
        <source>Shows Btrfs filesystem and subvolume information for the running host or selected repair system.</source>
        <translation>Показывает файловую систему Btrfs и информацию об объеме для запуска хоста или выбранной системы восстановления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="679"/>
        <source>Mapper status</source>
        <translation>Статус медведя</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="679"/>
        <source>Shows selected mapper ancestry, device-mapper state and cryptsetup status when available.</source>
        <translation>Показывает выбранную родословную картографа, состояние карты устройства и статус cryptsetup, когда это доступно.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="680"/>
        <source>LUKS / crypttab</source>
        <translation>LUKS/crypttab</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="680"/>
        <source>Shows LUKS/mapped ancestry plus crypttab and fstab mapper references.</source>
        <translation>Показывает LUKS/картографическую родословную плюс ссылки на crypttab и fstab mapper.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="681"/>
        <location filename="../src/MainWindow.cpp" line="13912"/>
        <source>Full diagnostic report</source>
        <translation>Полный диагностический отчет</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="681"/>
        <source>Combines all read-only diagnostics for the selected scope in one privileged inspection session.</source>
        <translation>Комбинирует всю диагностику только для чтения для выбранной области в одной привилегированной инспекционной сессии.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1073"/>
        <source>Alpine uses OpenRC, mkinitfs and syslinux/extlinux; boot-stack reconciliation is not enabled (run the initramfs and extlinux stages separately).</source>
        <translation>Alpine использует OpenRC, mkinitfs и syslinux/extlinux; выверка загрузочного стека не включена (запуск этапов initramfs и extlinux отдельно).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1074"/>
        <source>Alpine boot-stack reconciliation is not enabled (run the initramfs and GRUB stages separately).</source>
        <translation>Выверка альпийского загрузочного стека не включена (запуск этапов initramfs и GRUB отдельно).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1075"/>
        <source>Alpine DKMS preflight found no build tree for installed kernel %1; install the matching headers and retry.</source>
        <translation>Альпийский предполет DKMS не обнаружил дерева сборки для установленного ядра %1; установите соответствующие заголовки и повторите попытку.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1076"/>
        <source>Alpine EFI-stub host default selection is not implemented; use the Alpine EFI repair stage for entry reconciliation.</source>
        <translation>Выбор по умолчанию хоста Alpine EFI-stub не реализован; для согласования входа используйте этап ремонта Alpine EFI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1077"/>
        <source>Alpine syslinux-EFI boot detected (EFI/syslinux/syslinux.efi); Make Default is not implemented for this backend.</source>
        <translation>Обнаружена загрузка Alpine syslinux-EFI (EFI/syslinux/syslinux.efi); Make Default не реализован для этого бэкэнда.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1078"/>
        <source>Alpine syslinux-EFI boot detected (EFI/syslinux/syslinux.efi); guarded repair is not implemented.</source>
        <translation>Обнаружена альпийская загрузка syslinux-EFI (EFI/syslinux/syslinux.efi); охраняемый ремонт не осуществляется.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1079"/>
        <source>apk simulated no package changes and the package state is byte-identical.</source>
        <translation>apk смоделировал отсутствие изменений пакета, и состояние пакета является байт-идентичным.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1080"/>
        <source>APT package lists are byte-identical and no repository index was fetched.</source>
        <translation>Списки пакетов APT являются байт-идентичными, и индекс репозитория не был получен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1081"/>
        <source>Standalone APK metadata refresh is not available on Alpine; use Upgrade installed packages for one guarded apk transaction.</source>
        <translation>Автономное обновление метаданных APK недоступно на Alpine; используйте установленные пакеты обновления для одной защищенной транзакции apk.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1082"/>
        <source>Standalone APT metadata refresh is not available on Arch; use Upgrade installed packages for one full pacman transaction.</source>
        <translation>Автономное обновление метаданных APT недоступно на Arch; используйте установленные пакеты обновления для одной полной транзакции pacman.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1083"/>
        <source>Arch boot-stack reconciliation requires available initramfs, GRUB and EFI repair prerequisites.</source>
        <translation>Для согласования загрузочного стека требуется наличие необходимых условий для ремонта initramfs, GRUB и EFI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1084"/>
        <source>Arch DKMS preflight found no build tree for installed kernel %1; install the matching headers and retry.</source>
        <translation>Arch DKMS preflight не нашел дерева сборки для установленного ядра %1; установите соответствующие заголовки и повторите попытку.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1085"/>
        <location filename="../src/MainWindow.cpp" line="1086"/>
        <source>Backends: %1</source>
        <translation>Источник: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1087"/>
        <source>BLS is not enabled; the default is a generated menuentry.</source>
        <translation>BLS не включен; по умолчанию генерируется меню.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1088"/>
        <source>The initramfs backend is booster, which the current repair implementation does not handle.</source>
        <translation>Бэкэнд initramfs является бустером, с которым не справляется текущая реализация ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1089"/>
        <source>initramfs, EFI/UKI and GRUB artifacts are byte-identical.</source>
        <translation>Артефакты initramfs, EFI/UKI и GRUB являются байтоидентичными.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1090"/>
        <source>initramfs and GRUB2 artifacts are byte-identical.</source>
        <translation>Артефакты initramfs и GRUB2 являются байт-идентичными.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1091"/>
        <source>Requires available initramfs and GRUB repair prerequisites.</source>
        <translation>Требуется наличие необходимых условий для ремонта initramfs и GRUB.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1092"/>
        <source>Embedded cmdline does not reference the live LUKS UUID %1.</source>
        <translation>Встроенный cmdline не ссылается на Live LUKS UUID %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1093"/>
        <source>Embedded cmdline does not reference the live root UUID %1.</source>
        <translation>Встроенный cmdline не ссылается на UUID %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1094"/>
        <source>Embedded cmdline does not select the live Btrfs subvolume /%1.</source>
        <translation>Встроенная cmdline не выбирает живой подобъем Btrfs /%1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1095"/>
        <source>default.target and display-manager.service were already correct.</source>
        <translation>default.target и display-manager.service уже были корректны.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1096"/>
        <source>DKMS preflight found no build tree for installed kernel %1; install the matching kernel-devel packages and retry.</source>
        <translation>В предполетном полете DKMS не было найдено дерева сборки для установленного ядра %1; установите соответствующие пакеты ядра и повторите попытку.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1097"/>
        <source>dnf4 is not supported by the guarded rpm backend.</source>
        <translation>dnf4 не поддерживается защищенным rpm backend.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1098"/>
        <source>No missing package files were detected; dnf5 check is reported as evidence.</source>
        <translation>Отсутствующие файлы пакетов не были обнаружены; проверка dnf5 является доказательством.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1099"/>
        <source>The dnf5 repository metadata cache is byte-identical.</source>
        <translation>Кэш метаданных хранилища dnf5 является байт-идентификационным.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1100"/>
        <source>dnf5 simulated no package changes and the rpm database is byte-identical.</source>
        <translation>dnf5 имитирует отсутствие изменений пакета, а база данных rpm является байт-идентичной.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1101"/>
        <source>dpkg reported no packages pending configuration.</source>
        <translation>dpkg не сообщил ни о каких пакетах в ожидании конфигурации.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1102"/>
        <source>Alpine uses apk; dpkg configuration is not available on Alpine.</source>
        <translation>Alpine использует apk; конфигурация dpkg недоступна на Alpine.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1103"/>
        <source>dpkg configuration is not available on Arch; use the Arch package transaction stages instead.</source>
        <translation>Конфигурация dpkg недоступна на Arch; вместо этого используйте этапы транзакций пакета Arch.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1104"/>
        <source>Fedora uses rpm/dnf; dpkg configuration is not available.</source>
        <translation>Fedora использует rpm/dnf; конфигурация dpkg недоступна.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1105"/>
        <source>EFI boot artifacts and firmware entries are byte-identical.</source>
        <translation>Загрузочные артефакты EFI и записи прошивки являются байт-идентичными.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1106"/>
        <source>EFI-stub entry repair requires efibootmgr in the recovery host.</source>
        <translation>Ремонт входа EFI-стуба требует efibootmgr в хосте восстановления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1107"/>
        <source>EFI-stub boot requires UEFI firmware; the recovery host booted in legacy BIOS mode.</source>
        <translation>Загрузка EFI-stub требует прошивки UEFI; хост восстановления загружен в устаревшем режиме BIOS.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1108"/>
        <source>extlinux.conf is byte-identical.</source>
        <translation>extlinux.conf является байт-идентичным.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1109"/>
        <source>The detected extlinux configuration %1 is not /boot/extlinux.conf.</source>
        <translation>Обнаруженная конфигурация extlinux %1 не является /boot/extlinux.conf.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1110"/>
        <source>extlinux default label %1 is already selected.</source>
        <translation>extlinux по умолчанию уже выбран %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1111"/>
        <source>Extlinux default selection is only available for the running host.</source>
        <translation>Выбор по умолчанию Extlinux доступен только для запущенного хоста.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1112"/>
        <source>extlinux default label set to %1.</source>
        <translation>По умолчанию extlinux установлен на %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1113"/>
        <source>update-extlinux generated a byte-identical configuration.</source>
        <translation>Update-extlinux генерирует байт-идентичные конфигурации.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1114"/>
        <source>MBR, BIOS boot partition and i386-pc modules are byte-identical.</source>
        <translation>MBR, загрузочный раздел BIOS и модули i386-pc являются байт-идентичными.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1115"/>
        <source>grubenv saved_entry already names the running kernel BLS entry %1.</source>
        <translation>grubenv save entry уже называет запущенное ядро BLS входом %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1116"/>
        <source>grubenv saved_entry set to %1 (all other keys preserved).</source>
        <translation>grubenv save entry установлен на %1 (сохранены все остальные ключи).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1117"/>
        <source>The read-only check reported no errors.</source>
        <translation>Проверка только для чтения не выявила ошибок.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1118"/>
        <source>The simulated fix-broken transaction proposed no package changes.</source>
        <translation>Смоделированная транзакция с фиксированными изменениями не предлагала никаких изменений пакета.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1119"/>
        <source>GRUB detected on legacy BIOS; no EFI boot path is available.</source>
        <translation>GRUB обнаружен на устаревшем BIOS; путь загрузки EFI недоступен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1120"/>
        <source>grub.cfg and grubenv are byte-identical.</source>
        <translation>grub.cfg и grubenv являются байт-идентичными.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1121"/>
        <source>grub.cfg is byte-identical.</source>
        <translation>grub.cfg является байт-идентичным.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1122"/>
        <source>GRUB_DEFAULT is not set to saved; grubenv does not select the default entry.</source>
        <translation>GRUB DEFAULT не настроен на сохранение; grubenv не выбирает вход по умолчанию.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1123"/>
        <source>BootOrder, labels and registrations already correct.</source>
        <translation>Заказ, этикетки и регистрация уже исправны.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1124"/>
        <source>The running host EFI System Partition could not be resolved.</source>
        <translation>Запуск хоста EFI System Partition не был разрешен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1125"/>
        <source>The target dpkg database or executable is incomplete.</source>
        <translation>Целевая база данных или исполняемый файл dpkg является неполной.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1126"/>
        <source>Rebuilt initramfs images are byte-identical.</source>
        <translation>Восстановленные изображения initramfs являются байт-идентичными.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1127"/>
        <source>Less than 1 GiB of free Btrfs space is available.</source>
        <translation>Доступно менее 1 ГБ свободного пространства Btrfs.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1128"/>
        <source>The grubenv file is missing or not a valid GRUB environment block.</source>
        <translation>В файле grubenv отсутствует или отсутствует действующий блок среды GRUB.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1129"/>
        <source>legacy BIOS target; no EFI boot path is available.</source>
        <translation>Унаследованная цель BIOS; путь загрузки EFI недоступен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1130"/>
        <source>apk is not installed in the target.</source>
        <translation>apk не устанавливается в цель.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1131"/>
        <source>The target apk installed database is missing.</source>
        <translation>Целевая база данных apk отсутствует.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1132"/>
        <source>The target apk world file is missing.</source>
        <translation>Отсутствует целевой файл apk.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1133"/>
        <source>apt-get is not installed in the target.</source>
        <translation>apt-get не устанавливается в цель.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1134"/>
        <source>btrfs-progs is not installed.</source>
        <translation>btrfs-progs не устанавливается.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1135"/>
        <source>DKMS is not installed in the target.</source>
        <translation>DKMS не устанавливается в цель.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1136"/>
        <source>DKMS is not installed in the Alpine target system.</source>
        <translation>DKMS не устанавливается в альпийской системе.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1137"/>
        <source>DKMS is not installed in the Arch target system.</source>
        <translation>DKMS не устанавливается в системе мишеней Arch.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1138"/>
        <source>dnf5 is not installed in the target system.</source>
        <translation>dnf5 не устанавливается в целевой системе.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1139"/>
        <source>dpkg is not installed in the target.</source>
        <translation>dpkg не устанавливается в цель.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1140"/>
        <source>dracut is not installed in the target system.</source>
        <translation>dracut не устанавливается в целевую систему.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1141"/>
        <source>The dracut generator directory is missing from the target.</source>
        <translation>Каталог генераторов dracut отсутствует.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1142"/>
        <source>efibootmgr is not installed; the running host default EFI entry cannot be changed.</source>
        <translation>efibootmgr не установлен; вход EFI по умолчанию не может быть изменен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1143"/>
        <source>graphical.target is missing from the target.</source>
        <translation>Графический.target отсутствует в цели.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1144"/>
        <source>grub2-editenv is not installed in the target.</source>
        <translation>grub2-editenv не устанавливается в цель.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1145"/>
        <source>grub2-install is not installed in the target.</source>
        <translation>grub2-установка не устанавливается в цель.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1146"/>
        <source>grub2-mkconfig is not installed in the target.</source>
        <translation>grub2-mkconfig не устанавливается в цель.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1147"/>
        <source>%1 is missing.</source>
        <translation>%1 отсутствует.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1148"/>
        <source>Requires GRUB configuration tooling.</source>
        <translation>Требуется инструментарий конфигурации GRUB.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1149"/>
        <source>grub-efi is not installed in the Alpine target.</source>
        <translation>Гриб-эфи не устанавливается в альпийскую цель.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1150"/>
        <source>The x86_64-efi GRUB module directory is missing from the Alpine target.</source>
        <translation>Каталог модулей x86 64-efi GRUB отсутствует в альпийской цели.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1151"/>
        <source>Neither grub-mkconfig nor update-grub is installed in the target system.</source>
        <translation>Ни grub-mkconfig, ни update-grub не установлены в целевой системе.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1152"/>
        <source>grub-install missing.</source>
        <translation>grub-install отсутствует.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1153"/>
        <source>grub-install is not installed in the Alpine target.</source>
        <translation>grub-install не устанавливается в альпийскую цель.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1154"/>
        <source>The grub package is not installed in the Alpine target.</source>
        <translation>Пакет граба не установлен в альпийской цели.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1155"/>
        <source>The GRUB ZFS module (zfs.mod) is not installed in the target.</source>
        <translation>Модуль GRUB ZFS (zfs.mod) не устанавливается в цель.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1156"/>
        <source>update-initramfs/mkinitramfs are not installed in the target.</source>
        <translation>update-initramfs/mkinitramfs не устанавливаются в цель.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1157"/>
        <source>lsinitrd is not installed in the target system; dracut image verification is unavailable.</source>
        <translation>Lsinitrd не установлен в системе-мишени; проверка изображения dracut недоступна.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1158"/>
        <source>mkinitcpio is not installed in the target system.</source>
        <translation>mkinitcpio не устанавливается в целевую систему.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1159"/>
        <source>mkinitfs is not installed in the target system.</source>
        <translation>mkinitfs не устанавливается в целевую систему.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1160"/>
        <source>The target has no mkinitfs configuration.</source>
        <translation>Объект не имеет конфигурации mkinitfs.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1161"/>
        <source>objcopy is unavailable; TUX.EFI embedded sections cannot be verified.</source>
        <translation>TUX.EFI встроенные разделы не могут быть проверены.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1162"/>
        <source>pacman is not installed in the target.</source>
        <translation>pacman не устанавливается в цель.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1163"/>
        <source>The target has no /etc/pacman.conf; refusing a package transaction.</source>
        <translation>Цель не имеет /etc/pacman.conf; отказ от пакетной транзакции.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1164"/>
        <source>The target pacman database directory is missing.</source>
        <translation>Отсутствует целевой каталог базы данных pacman.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1165"/>
        <source>rpm is not installed in the target system.</source>
        <translation>rpm не устанавливается в целевой системе.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1166"/>
        <source>The target RPM database is missing.</source>
        <translation>Целевая база данных RPM отсутствует.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1167"/>
        <source>snapper is not installed.</source>
        <translation>Snapper не устанавливается.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1168"/>
        <source>The syslinux package is not installed according to the detected package manager.</source>
        <translation>Пакет syslinux не устанавливается согласно обнаруженному диспетчеру пакетов.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1169"/>
        <source>TUXEDO UKI builder create_boot_uki_base.sh is not installed in the target.</source>
        <translation>TUXEDO UKI builder create boot uki base.sh не устанавливается в цель.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1170"/>
        <source>update-extlinux is not installed in the target.</source>
        <translation>Update-extlinux не устанавливается в цель.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1171"/>
        <source>The running host has no /etc/update-extlinux.conf.</source>
        <translation>У хоста нет /etc/update-extlinux.conf.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1172"/>
        <source>update-extlinux is not installed in the running host.</source>
        <translation>Update-extlinux не устанавливается в запущенном хосте.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1173"/>
        <source>The zfs-initramfs hook is not installed in the target (no /usr/share/initramfs-tools/hooks/zfs).</source>
        <translation>Крюк zfs-initramfs не устанавливается в цель (нет /usr/share/initramfs-tools/hooks/zfs).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1174"/>
        <source>Multiple extlinux entries reference the running kernel %1 (%2).</source>
        <translation>Несколько записей extlinux ссылаются на работающее ядро %1 (%2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1175"/>
        <source>The running @ root contains nested subvolumes that this release does not migrate: %1.</source>
        <translation>Запущенный корень @ содержит вложенные объемы, которые этот выпуск не мигрирует: %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1176"/>
        <source>No EFI boot path was detected for the Alpine target (bootloader backend %1).</source>
        <translation>Путь загрузки EFI не был обнаружен для альпийской цели (загрузчик %1).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1177"/>
        <source>No installed Alpine kernels were found under target /boot.</source>
        <translation>Никаких установленных альпийских ядер под прицелом/загрузкой обнаружено не было.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1178"/>
        <source>The target has no configured apk repositories.</source>
        <translation>Цель не имеет настроенных репозиториев apk.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1179"/>
        <source>The target has no configured APT sources to refresh.</source>
        <translation>Цель не имеет настроенных источников APT для обновления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1180"/>
        <source>No EFI System Partition was identified for the selected Arch target.</source>
        <translation>Для выбранной цели EFI System Partition не был идентифицирован.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1181"/>
        <source>No non-rescue BLS entries are installed.</source>
        <translation>Нет неспасательных записей BLS.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1182"/>
        <source>No canonical EFI vendor loader was found on the running host ESP and grub-install is not available.</source>
        <translation>Канонический погрузчик EFI не был найден на запущенном хосте ESP, а grub-install недоступен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1183"/>
        <source>No supported display manager was detected in the target.</source>
        <translation>В цели не было обнаружено поддерживаемого дисплея.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1184"/>
        <source>No supported display manager unit is installed in the target.</source>
        <translation>В цели не устанавливается поддерживаемый блок дисплея.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1185"/>
        <source>The target has no enabled dnf repositories.</source>
        <translation>Объект не имеет репозиториев dnf.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1186"/>
        <source>The target has no enabled dnf repositories to refresh.</source>
        <translation>Цель не имеет репозиториев dnf для обновления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1187"/>
        <source>No installed dracut kernels were found under target /boot.</source>
        <translation>Никаких установленных ядер dracut под прицелом/загрузкой обнаружено не было.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1188"/>
        <source>No EFI-stub initramfs image (initramfs-*) is present at the EFI System Partition root.</source>
        <translation>Изображение EFI-стуба initramfs (initramfs-*) отсутствует в корне системного раздела EFI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1189"/>
        <source>No EFI-stub kernel image (vmlinuz-*) is present at the EFI System Partition root.</source>
        <translation>Изображение ядра EFI-стуба (vmlinuz-*) отсутствует в корне системного раздела EFI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1190"/>
        <source>No EFI System Partition candidate on the selected disk.</source>
        <translation>На выбранном диске нет кандидата на системный раздел EFI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1191"/>
        <source>No EFI System Partition is present or derivable on the selected disk.</source>
        <translation>На выбранном диске отсутствует системный раздел EFI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1192"/>
        <source>No extlinux/syslinux configuration was detected in the target.</source>
        <translation>Конфигурация extlinux/syslinux не была обнаружена.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1193"/>
        <source>No extlinux/syslinux configuration was detected in the running host.</source>
        <translation>Конфигурация extlinux/syslinux не была обнаружена в запущенном хосте.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1194"/>
        <source>The detected GRUB layout is not a grub2 layout.</source>
        <translation>Обнаруженная компоновка GRUB не является компоновкой grub2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1195"/>
        <source>No supported file system check tool is installed in the recovery environment.</source>
        <translation>В среде восстановления не устанавливается поддерживаемый инструмент проверки файловой системы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1196"/>
        <source>No supported initramfs backend (mkinitfs, mkinitcpio, dracut or initramfs-tools) was detected.</source>
        <translation>Поддержка initramfs (mkinitfs, mkinitcpio, dracut или initramfs-tools) отсутствует.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1197"/>
        <source>Unable to determine the live root filesystem UUID.</source>
        <translation>Невозможно определить UUID файловой системы Live root.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1198"/>
        <source>No installed kernel module directories were found for mkinitcpio.</source>
        <translation>Для mkinitcpio не было обнаружено установленных каталогов модулей ядра.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1199"/>
        <source>No supported OpenRC display manager service is installed in the target.</source>
        <translation>В цели не устанавливается поддерживаемая служба дисплея OpenRC.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1200"/>
        <source>No guarded package-manager backend was detected (detected: %1).</source>
        <translation>Защищенного бэкэнда менеджера пакетов обнаружено не было (обнаружено: %1).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1201"/>
        <source>No detected package manager can verify the syslinux package.</source>
        <translation>Ни один менеджер пакетов не может проверить пакет syslinux.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1202"/>
        <source>No supported reboot mechanism was found on the running host.</source>
        <translation>На бегущем хосте не было обнаружено поддерживаемого механизма перезагрузки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1203"/>
        <source>The selected scope&apos;s root, /boot, ESP and /home filesystems could not be resolved.</source>
        <translation>Корень выбранной области, /boot, ESP и /home файловые системы не могут быть решены.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1204"/>
        <source>No supported service manager (systemd or OpenRC) was detected in the target.</source>
        <translation>Ни один поддерживаемый сервис-менеджер (системный или OpenRC) не был обнаружен в цели.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1205"/>
        <source>No Snapper root configuration manages / with FSTYPE=btrfs.</source>
        <translation>Ни одна конфигурация корня Snapper не управляет / с FSTYPE = btrfs.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1206"/>
        <source>The selected scope has no supported file system type for a read-only check.</source>
        <translation>Выбранная область не имеет поддерживаемого типа файловой системы для проверки только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1207"/>
        <source>The running host root filesystem is %1, not Btrfs.</source>
        <translation>Корневая файловая система хоста - это %1, а не Btrfs.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1208"/>
        <source>The detected bootloader is %1; GRUB is not the selected bootloader.</source>
        <translation>Обнаруженный загрузчик %1; GRUB не является выбранным загрузчиком.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1209"/>
        <source>The running root subvolume is %1, not the top-level @.</source>
        <translation>Бегущий корневой подобъем — %1, а не верхний уровень @.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1210"/>
        <source>Unable to derive a running host UKI candidate.</source>
        <translation>Невозможно получить хост-кандидат UKI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1211"/>
        <source>The OpenRC default runlevel already enabled %1.</source>
        <translation>Уровень выполнения OpenRC по умолчанию уже включил %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1212"/>
        <source>A package manager or package-manager lock is active.</source>
        <translation>Блокировка менеджера пакетов или менеджера пакетов активна.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1213"/>
        <source>The pacman transaction reported no packages to install, upgrade or remove.</source>
        <translation>Транзакция pacman не предусматривала установки, обновления или удаления пакетов.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1214"/>
        <source>The running host pins subvolid= in fstab or the kernel command line.</source>
        <translation>Запускаемые хост-пины subvolid= в fstab или командной строке ядра.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1215"/>
        <source>The running host root filesystem is read-only.</source>
        <translation>Запускаемая хост-корневая файловая система только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1216"/>
        <source>The rpm DKMS header correction (kernel-devel/akmods) is not implemented.</source>
        <translation>Коррекция заголовка rpm DKMS (kernel-devel/akmods) не реализована.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1217"/>
        <source>The running kernel %1 has no installed BLS entry.</source>
        <translation>Работающее ядро %1 не имеет установленного входа BLS.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1218"/>
        <source>The running kernel %1 has no entry in %2.</source>
        <translation>Работающее ядро %1 не имеет входа в %2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1219"/>
        <source>Secure Boot requires a signed loader.</source>
        <translation>Безопасная загрузка требует подписанного погрузчика.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1220"/>
        <source>The running host has a separate /boot filesystem outside the root snapshot.</source>
        <translation>Запущенный хост имеет отдельную файловую систему /boot вне снимка корня.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1221"/>
        <source>Another snapper command is running.</source>
        <translation>Еще одна команда Snapper работает.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1222"/>
        <source>syslinux/extlinux (BIOS) boot detected; no EFI boot path is available.</source>
        <translation>Загрузка syslinux/extlinux (BIOS) обнаружена; путь загрузки EFI недоступен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1223"/>
        <source>A sysvinit display-manager script was detected; offline repair is not implemented for sysvinit.</source>
        <translation>Был обнаружен скрипт дисплея-менеджера сисвинита; для сисвинита не реализован офлайн-ремонт.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1224"/>
        <source>A Timeshift btrfs snapshot inventory is present; Snapper @ rollback is not supported.</source>
        <translation>Присутствует инвентарь моментальных снимков Timeshift btrfs; Snapper @ rollback не поддерживается.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1225"/>
        <source>UEFI variables are not writable; the running host default EFI entry cannot be changed.</source>
        <translation>Переменные UEFI не записываются; вход EFI по умолчанию не может быть изменен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1226"/>
        <source>%1 is missing or empty.</source>
        <translation>%1 отсутствует или пуст.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1227"/>
        <source>Embedded kernel %1 is not installed under /boot.</source>
        <translation>Встроенное ядро %1 не устанавливается под /boot.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1228"/>
        <source>TUX.EFI does not embed a readable .cmdline section.</source>
        <translation>TUX.EFI не встраивает читаемый раздел .cmdline.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1229"/>
        <source>TUX.EFI does not embed a kernel version (.uname).</source>
        <translation>TUX.EFI не встраивает версию ядра (.uname).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1230"/>
        <source>TUX.EFI is not a readable PE/COFF image (objcopy could not read .uname).</source>
        <translation>TUX.EFI не является читабельным изображением PE/COFF (обжкопия не может читать .uname).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1231"/>
        <source>Unable to create a temporary file for UKI section inspection.</source>
        <translation>Невозможно создать временный файл для проверки раздела UKI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1232"/>
        <source>Backend %1 has no guarded implementation for stage %2.</source>
        <translation>Backend %1 не имеет защищенной реализации для этапа %2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1233"/>
        <source>The running kernel release could not be determined.</source>
        <translation>Высвобождение бегущего ядра определить не удалось.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1234"/>
        <source>The running host ZFS root pool is not resolvable (zpool is not installed or the pool is not imported).</source>
        <translation>Бассейн ZFS не разрешим (zpool не установлен или бассейн не импортирован).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1235"/>
        <source>The extlinux label %1 cannot be written to /etc/update-extlinux.conf safely.</source>
        <translation>Маркировка extlinux %1 не может быть безопасно записана на /etc/update-extlinux.conf.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1236"/>
        <source>The simulated upgrade transaction proposed no package changes.</source>
        <translation>Симулированная транзакция обновления не предлагала никаких изменений пакета.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1237"/>
        <source>Validation is read-only.</source>
        <translation>Валидация только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1270"/>
        <source>Backed up Alpine EFI loader files to %1 (firmware fallback present before repair: %2).</source>
        <translation>Резервное копирование файлов загрузчика Alpine EFI в %1 (запас программного обеспечения, присутствующий перед ремонтом: %2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1271"/>
        <source>PASS: refreshed the firmware fallback loader EFI/boot/bootx64.efi from %1.</source>
        <translation>PASS: обновил встроенный резервный погрузчик EFI/boot/bootx64.efi от %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1272"/>
        <source>PASS: Alpine EFI loader files verified under %1/EFI/%2</source>
        <translation>PASS: файлы загрузчика Alpine EFI, проверенные на %1/EFI/%2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1273"/>
        <source>Restored the Alpine EFI loader files from the session backup %1.</source>
        <translation>Восстановлены файлы загрузчика Alpine EFI из резервного копирования сеанса %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1274"/>
        <source>Detected %1 EFI-stub firmware entry(ies) on the selected ESP (%2); reconciling firmware state without file synthesis.</source>
        <translation>Обнаружена запись(и) прошивки %1 на выбранном ESP (%2); примирение состояния прошивки без синтеза файлов.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1275"/>
        <source>PASS: Alpine EFI-stub firmware entries verified; no ESP files were changed.</source>
        <translation>PASS: прошивка Alpine EFI-stub верифицирована; файлы ESP не были изменены.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1276"/>
        <source>Captured complete firmware entry state before Alpine GRUB EFI install: %1</source>
        <translation>Полное состояние ввода прошивки перед установкой Alpine GRUB EFI: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1277"/>
        <source>Writable UEFI efivars/efibootmgr are unavailable; Alpine GRUB EFI repair will update loader files and the fallback copy only (read-only firmware-variable mode).</source>
        <translation>Записываемые UEFI efivars/efibootmgr недоступны; Alpine GRUB EFI repair обновит файлы загрузчика и резервную копию (только для чтения в режиме переменной прошивки).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1278"/>
        <source>SIMULATE/PREFLIGHT: Alpine GRUB EFI install target=%1 fs=%2 id=%3 mode=--no-nvram (helper-managed firmware entries)</source>
        <translation>SIMULATE/PREFLIGHT: Alpine GRUB EFI install target=%1 fs=%2 id=%3 mode=-no-nvram (Helper-Managed firmware entries)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1279"/>
        <source>PASS: Alpine initramfs rebuilt and verified for %1</source>
        <translation>PASS: Alpine initramfs восстановлен и проверен на %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1280"/>
        <source>Alpine missing-file repair: reinstalling %1 package(s) with missing files: %2</source>
        <translation>Восстановление альпийских недостающих файлов: переустановка пакета (пакетов) %1 с отсутствующими файлами: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1281"/>
        <source>Alpine missing-file detection: no missing package files; running the dependency-only apk fix transaction.</source>
        <translation>Обнаружение альпийских отсутствующих файлов: отсутствие отсутствующих файлов пакетов; запуск транзакции apk для исправления зависимостей.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1282"/>
        <source>Target %1 is already mounted at %2 (pre-existing source=%3 options=%4 id=%5); leaving it untouched.</source>
        <translation>Target %1 уже установлен на %2 (существующий источник = параметры %3 = %4 id = %5); оставляя его нетронутым.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1283"/>
        <source>Alpine apk preflight: %1</source>
        <translation>Альпийский apk: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1284"/>
        <source>REFUSED: apk simulation proposes %1 package changes (safety limit: %2).</source>
        <translation>Моделирование apk предлагает изменения пакета %1 (предел безопасности: %2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1285"/>
        <source>REFUSED: apk simulation reported unresolved or conflicting packages.</source>
        <translation>Моделирование apk показало неразрешенные или противоречивые пакеты.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1286"/>
        <source>REFUSED: apk simulation proposes a package downgrade.</source>
        <translation>Моделирование apk предлагает понижение пакета.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1287"/>
        <source>REFUSED: apk simulation reported a locked package database.</source>
        <translation>Моделирование apk сообщило о закрытой базе данных пакетов.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1288"/>
        <source>REFUSED: apk simulation reported incomplete repository metadata.</source>
        <translation>Отказ: моделирование apk сообщило о неполных метаданных хранилища.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1289"/>
        <source>REFUSED: apk simulation would remove &apos;%1&apos; without replacing it in the same transaction.</source>
        <translation>Моделирование apk удалит %1, не заменив его в той же транзакции.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1290"/>
        <source>REFUSED: apk simulation reported an untrusted package signature.</source>
        <translation>apk-симуляция сообщила о ненадежной подписи пакета.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1291"/>
        <source>REFUSED: apk simulation reported an error Boot Bitch does not recognize as safe.</source>
        <translation>apk сообщил об ошибке, которую Boot Bitch не признает безопасной.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1292"/>
        <source>apk simulation replaces package &apos;%1&apos; in the same transaction; removal accepted.</source>
        <translation>Моделирование apk заменяет пакет %1 в той же транзакции.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1293"/>
        <source>KNOWN ISSUE: APT correction is blocked by broken dependencies; simulating --fix-broken before retry.</source>
        <translation>Коррекция APT блокируется сломанными зависимостями; имитирует -фиксируется до повторного использования.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1294"/>
        <source>KNOWN ISSUE: APT correction is blocked by interrupted dpkg configuration; completing dpkg once and re-simulating.</source>
        <translation>Коррекция APT блокируется прерываемой конфигурацией dpkg; завершение dpkg один раз и повторное моделирование.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1295"/>
        <source>full-upgrade simulation did not produce an acceptable transaction; evaluating dist-upgrade.</source>
        <translation>Моделирование полного обновления не привело к приемлемой транзакции; оценка дист-обновления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1296"/>
        <source>Full-upgrade simulation was unavailable or unsafe; using the successful standard upgrade transaction.</source>
        <translation>Моделирование полного обновления было недоступно или небезопасно; использование успешной транзакции стандартного обновления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1297"/>
        <source>apt intent translated: %1</source>
        <translation>Намерение apt переводится как %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1298"/>
        <source>APT package lists changed or a repository index was fetched; reporting the metadata refresh as changed.</source>
        <translation>Изменились списки пакетов APT или был получен индекс хранилища; сообщение об обновлении метаданных по мере изменения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1299"/>
        <source>Target package policy rejected standard upgrade and requested full/dist upgrade; evaluating full-upgrade.</source>
        <translation>Политика целевого пакета отклонила стандартное обновление и запросила полное обновление; оценка полного обновления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1300"/>
        <source>APT simulation proposes %1 non-protected package removal(s): %2</source>
        <translation>Моделирование APT предлагает незащищенное удаление упаковки %1: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1301"/>
        <source>REFUSED: APT simulation proposes removing essential packages.</source>
        <translation>Отказ: моделирование APT предполагает удаление основных пакетов.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1302"/>
        <source>REFUSED: APT simulation would remove protected package &apos;%1&apos;.</source>
        <translation>Моделирование APT удалит защищенный пакет %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1303"/>
        <source>REFUSED: APT simulation would remove %1 packages (safety limit: %2).</source>
        <translation>Моделирование APT удалит пакеты %1 (ограничение безопасности: %2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1304"/>
        <source>Standard upgrade leaves packages pending; evaluating full-upgrade simulation.</source>
        <translation>Стандартное обновление оставляет пакеты в ожидании; оценка моделирования полного обновления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1305"/>
        <source>WARN: accepted release metadata change for %1; metadata refreshed.</source>
        <translation>Предупреждение: принятое изменение метаданных высвобождения для %1; обновленные метаданные.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1306"/>
        <source>WARNING: apt-get update refused a repository release metadata change for: %1</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: обновление apt-get отказало в изменении метаданных репозитория для: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1307"/>
        <source>WARNING: retrying metadata refresh with -o Acquire::AllowReleaseInfoChange=true (release-info changes only; signatures, keys and package verification remain enforced).</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: повторное обновление метаданных с помощью -o Acquire::AllowReleaseInfoChange=true (изменение только информации о выпуске; сигнатуры, ключи и проверка пакета остаются в силе).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1308"/>
        <source>APT upgrade decision: &apos;%1&apos; selected from simulation results.</source>
        <translation>Решение об обновлении APT: %1 выбран из результатов моделирования.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1309"/>
        <source>apt upgrade is disabled by this distribution; running &apos;%1&apos; instead</source>
        <translation>Модернизация apt отключена этим дистрибутивом; вместо этого работает %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1310"/>
        <source>PASS: Arch initramfs images rebuilt and verified.</source>
        <translation>Изображения Arch initramfs восстановлены и проверены.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1311"/>
        <source>AUTO-CORRECT: %1</source>
        <translation>AUTO-CORRECT: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1312"/>
        <source>AUTO-CORRECT: restored temporary stale mapper alias %1 -&gt; %2 for this repair request.</source>
        <translation>AUTO-CORRECT: восстановленный временный несвежий псевдоним %1 -&gt; %2 для этого запроса на ремонт.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1313"/>
        <source>BEGIN: Chroot shell command</source>
        <translation>Оригинальное название: Chroot shell command</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1314"/>
        <source>BEGIN: Running-host shell command</source>
        <translation>Оригинальное название: Running-host shell command</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1315"/>
        <source>BEGIN: %1</source>
        <translation>Исполнитель: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1316"/>
        <source>BEGIN: package backend %1 (%2)</source>
        <translation>BEGIN: пакетный бэкэнд %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1317"/>
        <source>BEGIN: Refresh package metadata</source>
        <translation>BEGIN: Обновить метаданные пакета</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1318"/>
        <source>Arch GRUB layout detected; applying the guarded conventional EFI reinstall after initramfs preflight.</source>
        <translation>Обнаружена компоновка Arch GRUB; применение охраняемой обычной переустановки EFI после предполетного полета initramfs.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1319"/>
        <source>PASS: boot stack reconciliation completed after component simulations and verification.</source>
        <translation>PASS: сверка багажника завершена после моделирования компонентов и проверки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1320"/>
        <source>PASS: boot stack reconciliation completed; EFI/UKI and GRUB were reused from the earlier EFI / UKI stage while mapper/crypttab and initramfs were reconciled.</source>
        <translation>PASS: сверка загрузочного стека завершена; EFI/UKI и GRUB были повторно использованы с более ранней стадии EFI/UKI, в то время как Mapper/crypttab и initramfs были согласованы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1321"/>
        <source>Boot-stack Arch EFI read-only preflight: PASS (%1, %2, id=%3)</source>
        <translation>Арка загрузки EFI только для чтения перед полетом: PASS (%1, %2, id=%3)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1322"/>
        <source>Fedora BIOS GRUB2 layout detected; no EFI/UKI artifacts are part of boot-stack reconciliation.</source>
        <translation>Обнаружен макет Fedora BIOS GRUB2; никакие артефакты EFI/UKI не являются частью сверки загрузочного стека.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1323"/>
        <source>No TUXEDO UKI builder detected; preserving the distribution&apos;s existing EFI layout during boot-stack reconciliation.</source>
        <translation>Ни один конструктор TUXEDO UKI не был обнаружен; сохраняя существующую компоновку дистрибутива EFI во время сверки загрузочного стека.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1324"/>
        <source>NOTE: --post-efi is an EFI-only hint; the Fedora BIOS boot stack reconciles dracut and GRUB2 without an EFI stage.</source>
        <translation>ПРИМЕЧАНИЕ: -post-efi - это подсказка только для EFI; загрузочный стек Fedora BIOS примиряет dracut и GRUB2 без этапа EFI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1325"/>
        <source>SIMULATE/PREFLIGHT: complete boot-stack reconciliation</source>
        <translation>SIMULATE/PREFLIGHT: полное согласование загрузочного стека</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1326"/>
        <source>SKIP: boot-stack EFI/UKI rebuild skipped because the EFI / UKI bootloader stage already rebuilt and verified this layout in the same run (--post-efi).</source>
        <translation>SKIP: загрузочный стек EFI / UKI перестроен, потому что этап загрузчика EFI / UKI уже перестроился и проверил эту компоновку в том же режиме (--post-efi).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1327"/>
        <source>SKIP: boot-stack GRUB regeneration skipped because the EFI / UKI bootloader stage already regenerated the GRUB configuration in the same run (--post-efi).</source>
        <translation>SKIP: регенерация загрузочного стека GRUB пропущена, потому что этап загрузчика EFI / UKI уже регенерировал конфигурацию GRUB в том же режиме (--post-efi).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1328"/>
        <source>Boot-stack TUXEDO UKI read-only preflight: PASS (%1, %2)</source>
        <translation>Загрузочный стек TUXEDO UKI только для чтения: PASS (%1, %2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1329"/>
        <source>Bound recovery-host resolver into target chroot (temporary, read-only)</source>
        <translation>Связанный разрешитель восстановления-хоста в целевой chroot (временный, только для чтения)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1330"/>
        <source>WARNING: btrfs check --repair is a last-resort operation that upstream documents as dangerous and can make a damaged filesystem worse. The caller must have explicit user confirmation and a backup.</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: btrfs check - repair - это операция на последнем месте, которая может сделать поврежденную файловую систему еще хуже. Абонент должен иметь явное подтверждение пользователя и резервную копию.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1331"/>
        <source>Chroot shell exit code: %1</source>
        <translation>Код выхода Chroot shell: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1332"/>
        <location filename="../src/MainWindow.cpp" line="1333"/>
        <source>Command: %1</source>
        <translation>Разработчик: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1334"/>
        <source>Configuration read skipped: %1 has no parent directory in the target (bootloader backend: %2).</source>
        <translation>Конфигурация пропущена: %1 не имеет родительского каталога в цели (бэкэнд загрузчика: %2).</translation>
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
        <translation>Полный комплект копий</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1339"/>
        <source>COPY COMPLETE — SHA-256 verification FAILED</source>
        <translation>COPY COMPLETE — проверка SHA-256 не выполнена</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1340"/>
        <source>FAILED verification: %1</source>
        <translation>Неверная проверка: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1341"/>
        <source>%1: %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1342"/>
        <source>rsync metadata/content re-check: PASS</source>
        <translation>Перепроверка метаданных/контента: PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1343"/>
        <source>SHA-256 verification FAILURES: %1 (the copy is not verified complete)</source>
        <translation>Проверка SHA-256: %1 (копия не проверена полностью)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1344"/>
        <source>SHA-256 regular files verified: %1</source>
        <translation>Обычные файлы SHA-256: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1345"/>
        <source>Source items: %1</source>
        <translation>Источник: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1346"/>
        <source>Unrelated destination files: retained (no --delete used)</source>
        <translation>Несвязанные файлы назначения: сохранены (нет - удаление используется)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1347"/>
        <source>Correction simulation exit code: %1 (%2)</source>
        <translation>Код выхода симуляции коррекции: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1348"/>
        <source>WARNING: could not mount target %1 from %2; the read-only diagnostic proceeds without it.</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: не удалось установить цель %1 из %2; диагностика только для чтения протекает без нее.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1349"/>
        <source>Created target destination directory %1 with inherited owner %2:%3</source>
        <translation>Создан каталог целевого назначения %1 с унаследованным владельцем %2:%3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1350"/>
        <source>Mapper/crypttab consistency gate: PASS</source>
        <translation>Затвор согласованности Mapper/crypttab: PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1351"/>
        <source>Mapper/crypttab gate: no target crypttab entries; PASS</source>
        <translation>Mapper/crypttab gate: нет целевых записей crypttab;</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1352"/>
        <source>Mapper/crypttab gate: entry &apos;%1&apos; resolves outside the selected disk: %2</source>
        <translation>Mapper/crypttab gate: запись &quot;%1&quot; разрешает выход за пределы выбранного диска: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1353"/>
        <source>Mapper/crypttab gate: %1 -&gt; %2</source>
        <translation>Mapper/crypttab gate: %1 -&gt; %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1354"/>
        <source>Mapper/crypttab gate: unresolved entry &apos;%1&apos; -&gt; &apos;%2&apos;</source>
        <translation>Mapper/crypttab gate: unresolved entry &apos;%1&apos; -&gt; &apos;%2&apos;</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1355"/>
        <source>Mapper/crypttab gate: unsupported source syntax for &apos;%1&apos;: %2</source>
        <translation>Mapper/crypttab gate: неподдерживаемый синтаксис источника для %1: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1356"/>
        <source>Destination: %1</source>
        <translation>Место назначения: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1357"/>
        <source>Detected target OS: %1</source>
        <translation>Обнаружена целевая ОС: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1358"/>
        <source>WARNING: private /dev filter: the selected target disk identity is unavailable; no block device or mapper will be exposed to the target chroot.</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: частный/дев-фильтр: выбранный идентификатор целевого диска недоступен; ни одно блочное устройство или картограф не будет подвергаться воздействию целевого корня.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1359"/>
        <source>Private /dev filter: %1 allowed block devices/mappers copied; %2 non-essential device-tree entries refused (plain files, sockets, fifos, and devices outside the selected target disk).</source>
        <translation>Частный/дев-фильтр: %1 позволяет скопировать блок-устройства/карты; %2 несущественные записи в дереве устройств отклоняются (простые файлы, розетки, фифос и устройства за пределами выбранного целевого диска).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1360"/>
        <source>DKMS preflight correction: build tree now present for %1</source>
        <translation>Предполетная коррекция DKMS: дерево сборки теперь доступно для %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1361"/>
        <source>SIMULATE/PREFLIGHT: dracut DKMS rebuild (headers must already be installed; no package guessing is performed)</source>
        <translation>SIMULATE/PREFLIGHT: реконструкция DKMS dracut (заголовки уже установлены; не производится угадывание пакета)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1362"/>
        <location filename="../src/MainWindow.cpp" line="1363"/>
        <location filename="../src/MainWindow.cpp" line="1364"/>
        <source>DKMS preflight: headers/build tree present for %1</source>
        <translation>Предполет DKMS: заголовки / дерево для %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1365"/>
        <source>KNOWN ISSUE: DKMS reported missing kernel headers; re-running header preflight/correction once.</source>
        <translation>ЗНАЙТЕ ИССУЭ: DKMS сообщила о пропаже заголовков ядра; повторный запуск заголовка перед полетом / исправление один раз.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1366"/>
        <source>DKMS preflight: missing headers for %1; repository package %2 is available.</source>
        <translation>Предполет DKMS: недостающие заголовки для %1; доступен пакет хранилища %2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1367"/>
        <source>WARNING: DKMS preflight found no build tree for %1 and %2 is unavailable from configured repositories.</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: Предполет DKMS не обнаружил, что дерево сборки для %1 и %2 недоступно из настроенных репозиториев.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1368"/>
        <source>SIMULATE/PREFLIGHT: mkinitcpio DKMS rebuild (headers must already be installed; no package guessing is performed)</source>
        <translation>SIMULATE/PREFLIGHT: реконструкция DKMS mkinitcpio (заголовки уже установлены; не выполняется угадывание пакета)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1369"/>
        <source>SIMULATE/PREFLIGHT: mkinitfs DKMS rebuild (headers must already be installed; no package guessing is performed)</source>
        <translation>SIMULATE/PREFLIGHT: реконструкция DKMS mkinitfs (заголовки уже установлены; не выполняется угадывание пакета)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1370"/>
        <source>PASS: DKMS autoinstall completed.</source>
        <translation>PASS: автоинсталляция DKMS завершена.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1371"/>
        <source>SIMULATE/PREFLIGHT: DKMS module rebuild</source>
        <translation>SIMULATE/PREFLIGHT: восстановление модуля DKMS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1372"/>
        <source>%1 was configured offline only; Boot Bitch intentionally did not start a graphical session.</source>
        <translation>%1 был настроен только в автономном режиме; Boot Bitch намеренно не запускал графический сеанс.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1373"/>
        <source>%1 was configured offline only; Boot Bitch intentionally did not start a graphical session inside the chroot.</source>
        <translation>%1 был настроен только в автономном режиме; Boot Bitch намеренно не запускал графическую сессию внутри chroot.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1374"/>
        <source>PASS: default.target -&gt; %1</source>
        <translation>PASS: default.target -&gt; %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1375"/>
        <source>Detected display manager: %1 (%2)</source>
        <translation>Дисплейный менеджер: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1376"/>
        <source>Enabling %1 in the offline target (it will NOT be started inside the repair chroot).</source>
        <translation>Включение %1 в офлайн-цель (не будет запускаться внутри ремонтного корня).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1377"/>
        <source>WARNING: %1 ExecStart is not executable in the target: %2</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: %1 ExecStart не выполняется в целевом режиме: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1378"/>
        <source>PASS: %1 ExecStart is present for headless preflight: %2</source>
        <translation>%1 ExecStart присутствует для безголового полета: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1379"/>
        <source>PASS: display-manager.service -&gt; %1</source>
        <translation>PASS: display-manager.service -&gt; %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1380"/>
        <source>PASS: %1 command present for headless preflight: %2</source>
        <translation>PASS: %1 Command Present for Headless Preflight: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1381"/>
        <source>Detected Alpine OpenRC display manager: %1 (%2)</source>
        <translation>Дисплейный менеджер Alpine OpenRC: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1382"/>
        <source>PASS: /etc/runlevels/default/%1 -&gt; %2</source>
        <translation>PASS: /etc/runlevels/default/%1 -&gt; %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1383"/>
        <source>WARNING: no explicit command= was found for %1; sh -n and rc-service -e still passed.</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: для %1 не было найдено явной команды =; sh-n и rc-service -e все же прошли.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1384"/>
        <source>%1 will be configured offline only; Boot Bitch will not start a graphical session.</source>
        <translation>%1 будет настроен только в автономном режиме; Boot Bitch не запустит графическую сессию.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1385"/>
        <source>Restoring the OpenRC default-runlevel link for %1 (offline only; the service is NOT started).</source>
        <translation>Восстановление ссылки OpenRC по умолчанию для %1 (только в автономном режиме; служба НЕ запущена).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1386"/>
        <source>SIMULATE/PREFLIGHT: OpenRC graphical login / display manager (offline only)</source>
        <translation>SIMULATE/PREFLIGHT: OpenRC графический логин / дисплеи (только в автономном режиме)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1387"/>
        <source>Display-manager preflight plan: set graphical.target default, enable %1, and repair display-manager.service offline.</source>
        <translation>План предварительного полета дисплея-менеджера: установите графический.target по умолчанию, включите %1 и восстановите дисплей-manager.service в автономном режиме.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1388"/>
        <source>Restoring graphical.target as the target default.</source>
        <translation>Восстановление графического.target в качестве целевого по умолчанию.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1389"/>
        <source>Selected %1 from last-boot display-manager journal evidence.</source>
        <translation>Выбранный %1 из последних данных журнала дисплея-менеджера.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1390"/>
        <source>Selected %1 from last-boot display-manager journal evidence because multiple managers are installed.</source>
        <translation>Выбранный %1 из журнала дисплеев с последней загрузкой, поскольку установлено несколько менеджеров.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1391"/>
        <source>SIMULATE/PREFLIGHT: graphical login / display manager</source>
        <translation>SIMULATE/PREFLIGHT: графический логин/дисплеевый менеджер</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1392"/>
        <source>%1 is a static unit; preserving it through the display-manager.service alias.</source>
        <translation>%1 является статическим блоком, сохраняя его через дисплеи. служебный псевдоним.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1393"/>
        <source>WARNING: systemd-analyze is unavailable; skipped headless display-manager verification.</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: системный анализ недоступен; пропущена проверка безголового дисплея-менеджера.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1394"/>
        <source>WARNING: headless systemd verification reported issues for %1 (the manager will not be started during repair).</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: безголовая система проверки сообщила о проблемах для %1 (менеджер не будет запущен во время ремонта).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1395"/>
        <source>PASS: headless systemd verification for %1</source>
        <translation>PASS: проверка безголовой системы для %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1396"/>
        <source>This transaction downloads approximately %1; the apply step reports its progress only when it finishes.</source>
        <translation>Эта транзакция загружает приблизительно %1; шаг приложения сообщает о своем прогрессе только после завершения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1397"/>
        <source>PASS: dnf5 upgrade removes only the old kernel package(s) (%1) — accepted as in-place kernel replacement.</source>
        <translation>PASS: обновление dnf5 удаляет только старый пакет (пакеты) ядра (%1), принятый в качестве замены ядра.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1398"/>
        <source>dnf5 repository metadata cache rewritten (cache fingerprint changed); reporting the metadata refresh as changed.</source>
        <translation>dnf5 кэш метаданных репозитория переписан (отпечаток кэша изменен); сообщение об обновлении метаданных изменено.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1399"/>
        <source>PASS: dnf5 metadata cache refreshed (repository metadata cache is byte-identical).</source>
        <translation>PASS: обновленный кэш метаданных dnf5 (кэш метаданных в хранилище является байт-идентификационным).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1400"/>
        <source>REFUSED: dnf5 simulation reported an unresolved or conflicting transaction.</source>
        <translation>Моделирование dnf5 сообщило о неразрешенной или конфликтующей транзакции.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1401"/>
        <source>REFUSED: dnf5 simulation proposes a package downgrade.</source>
        <translation>ОТВЕТ: симуляция dnf5 предлагает понижение рейтинга.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1402"/>
        <source>REFUSED: dnf5 simulation failed with exit code %1.</source>
        <translation>Отказ: симуляция dnf5 провалилась с выходным кодом %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1403"/>
        <source>REFUSED: dnf5 simulation reported a locked package database.</source>
        <translation>Dnf5-симуляция сообщила о закрытой базе данных пакетов.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1404"/>
        <source>REFUSED: dnf5 simulation reported incomplete repository metadata.</source>
        <translation>Отказ: моделирование dnf5 сообщило о неполных метаданных хранилища.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1405"/>
        <source>REFUSED: dnf5 upgrade would obsolete critical package &apos;%1&apos;.</source>
        <translation>Обновление dnf5 будет устаревшим критическим пакетом %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1406"/>
        <source>REFUSED: dnf5 reinstall would replace &apos;%1&apos;, which is not one of the reinstalled packages.</source>
        <translation>Переустановка dnf5 заменит %1, который не является одним из переустановленных пакетов.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1407"/>
        <source>REFUSED: dnf5 simulation proposes package removals.</source>
        <translation>Отказ: моделирование dnf5 предлагает удаление пакетов.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1408"/>
        <source>REFUSED: dnf5 simulation would replace %1 packages (safety limit: %2).</source>
        <translation>Dnf5 заменит пакеты %1 (ограничение безопасности: %2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1409"/>
        <source>REFUSED: dnf5 simulation reported an untrusted or missing package signature key.</source>
        <translation>Dnf5-симуляция сообщала о ненадежном или отсутствующем ключе подписи пакета.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1410"/>
        <source>REFUSED: dnf5 simulation proposes %1 package changes (safety limit: %2).</source>
        <translation>В моделировании dnf5 предлагаются изменения пакета %1 (предел безопасности: %2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1411"/>
        <source>REFUSED: dnf5 simulation reported an error Boot Bitch does not recognize as safe.</source>
        <translation>Моделирование dnf5 сообщило об ошибке, которую Boot Bitch не признает безопасной.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1412"/>
        <source>dnf5 preflight version: %1</source>
        <translation>Предполетная версия dnf5: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1413"/>
        <source>SIMULATE/PREFLIGHT: dracut guarded per-kernel rebuild for %1 installed kernel(s) (single build per kernel; verified before any /boot write)</source>
        <translation>SIMULATE/PREFLIGHT: dracut guarded per-kernel rebuild for %1 installed kernel(s) (single build per kernel; verified before any /boot write)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1414"/>
        <source>dracut preflight version: %1</source>
        <translation>Предполетная версия dracut: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1415"/>
        <source>e2fsck preen left uncorrected errors (exit code 4); running the confirmed forced repair pass.</source>
        <translation>e2fsck preen оставил неисправленные ошибки (код выхода 4); запустив подтвержденный принудительный пропуск на ремонт.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1416"/>
        <source>Annotating selected-system EFI entries with OS/model identity: %1 / %2 (PARTUUID %3).</source>
        <translation>Аннотация записей выбранной системы EFI с идентификатором ОС / модели: %1 / %2 (PARTUUID %3).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1417"/>
        <source>EFI bootloader ID: %1</source>
        <translation>ID загрузчика EFI: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1418"/>
        <source>PASS: EFI BootOrder already groups each drive&apos;s firmware destinations by normal use.</source>
        <translation>PASS: EFI BootOrder уже группирует назначения прошивки каждого диска по обычному использованию.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1419"/>
        <source>PASS: EFI BootOrder grouped by drive; all retained firmware entries remain present.</source>
        <translation>PASS: EFI BootOrder сгруппирован по диску; все сохраненные записи прошивки остаются.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1420"/>
        <source>Grouping EFI BootOrder by drive and normal boot use: %1</source>
        <translation>Группировка EFI BootOrder по приводу и нормальному использованию загрузки: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1421"/>
        <source>Clearing BootNext introduced during EFI repair.</source>
        <translation>Очистка BootNext вводится при ремонте EFI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1422"/>
        <source>PASS: Boot%1 is first in BootOrder; all other entries were retained.</source>
        <translation>Boot%1 является первым в BootOrder; все остальные записи были сохранены.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1423"/>
        <source>PASS: selected ESP firmware destinations reconciled; removed Boot%1.</source>
        <translation>PASS: выбранные пункты назначения прошивки ESP согласованы; удален Boot%1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1424"/>
        <source>PASS: selected ESP firmware destinations are unique; no duplicate entries removed.</source>
        <translation>PASS: выбранные пункты назначения прошивки ESP уникальны; дубликаты не удаляются.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1425"/>
        <source>PASS: %1 firmware entry restored as Boot%2 on selected system ESP %3.</source>
        <translation>Ввод прошивки %1 восстановлен в виде Boot%2 на выбранной системе ESP %3.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1426"/>
        <source>PASS: generic EFI firmware entry restored as Boot%1 on selected system ESP %2.</source>
        <translation>PASS: общий вход прошивки EFI восстановлен в виде Boot%1 на выбранной системе ESP %2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1427"/>
        <source>EFI grouping did not retain Boot%1 first; re-promoting it once.</source>
        <translation>Группа EFI не сохранила Boot%1 первой; повторно продвигая его один раз.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1428"/>
        <source>EFI entry identity preserved while firmware ID changed: Boot%1 -&gt; Boot%2.</source>
        <translation>Идентификатор входа EFI сохраняется при изменении идентификатора прошивки: Boot%1 -&gt; Boot%2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1429"/>
        <source>iPXE/WebFAI EFI loader is absent on selected system ESP; no recovery firmware entry was created.</source>
        <translation>Загрузчик iPXE/WebFAI EFI отсутствует в выбранной системе ESP; ввод прошивки восстановления не производился.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1430"/>
        <source>iPXE/WebFAI EFI loader is present on the selected system ESP, but firmware variables are not writable; no recovery firmware entry was created.</source>
        <translation>Погрузчик iPXE/WebFAI EFI присутствует в выбранной системе ESP, но переменные прошивки не записываются; запись прошивки восстановления не была создана.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1431"/>
        <source>Repair-target EFI entry label preserved while loader changed: Boot%1 -&gt; Boot%2.</source>
        <translation>Ремонтно-целевой EFI сохранён при смене погрузчика: Boot%1 -&gt; Boot%2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1432"/>
        <source>EFI Boot%1 already names selected model; leaving label &apos;%2&apos; unchanged.</source>
        <translation>EFI Boot%1 уже назвал выбранную модель, оставив маркировку %2 без изменений.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1433"/>
        <source>EFI Boot%1 (%2) label updated to &apos;%3&apos; on selected system ESP only.</source>
        <translation>Маркировка EFI Boot%1 (%2) обновлена до «%3» только на выбранной системе ESP.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1434"/>
        <source>PASS: selected ESP EFI labels retain the drive model after final BootOrder maintenance.</source>
        <translation>PASS: выбранные метки ESP EFI сохраняют модель привода после окончательного обслуживания BootOrder.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1435"/>
        <source>Selected EFI loader is present at %1, but firmware variables are not writable; no generic firmware entry was created.</source>
        <translation>Выбранный погрузчик EFI присутствует в %1, но переменные прошивки не записываются; не было создано никакой общей записи прошивки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1436"/>
        <source>PASS: EFI loader files verified under %1/EFI/%2</source>
        <translation>PASS: файлы загрузчика EFI, проверенные в %1/EFI/%2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1437"/>
        <source>Making Boot%1 the explicit default while preserving all other EFI entries: %2</source>
        <translation>Создание Boot%1 по явному умолчанию при сохранении всех других записей EFI: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1438"/>
        <source>Multiple generic EFI entries already point to the selected system loader (%1); destination maintenance will retain one after the repair.</source>
        <translation>Несколько общих записей EFI уже указывают на выбранный системный погрузчик (%1); техническое обслуживание пункта назначения сохранит один после ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1439"/>
        <source>Multiple iPXE/WebFAI entries already point to the selected system ESP (%1); destination maintenance will retain one after the repair.</source>
        <translation>Несколько записей iPXE/WebFAI уже указывают на выбранную систему ESP (%1); техническое обслуживание пункта назначения сохранит одну после ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1440"/>
        <source>Multiple TUXEDO UKI entries already point to the selected ESP (%1); destination maintenance will retain one after the repair.</source>
        <translation>Несколько записей TUXEDO UKI уже указывают на выбранный ESP (%1); техническое обслуживание пункта назначения сохранит один после ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1441"/>
        <source>Selected EFI vendor directory has no primary loader; no generic firmware entry was created.</source>
        <translation>В выбранном каталоге поставщиков EFI нет основного загрузчика; не было создано ни одной общей записи прошивки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1442"/>
        <source>KNOWN ISSUE: firmware NVRAM registration failed; retrying once as a file-only --no-nvram reinstall.</source>
        <translation>ЗНАЙТЕ ИССУАЛЬНЫЙ: регистрация прошивки NVRAM не удалась; перезагрузка один раз в виде переустановки только файла - no-nvram.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1443"/>
        <source>EFI System Partition: %1 (%2)</source>
        <translation>Системный раздел EFI: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1444"/>
        <source>EFI registration mode: %1</source>
        <translation>Режим регистрации EFI: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1445"/>
        <source>EFI registration mode: Alpine --no-nvram install with helper-managed firmware entries (writable efivars: %1).</source>
        <translation>Режим регистрации EFI: установка Alpine-no-nvram с записями прошивки, управляемыми помощниками (письменные efivars: %1).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1446"/>
        <source>Removing duplicate/legacy selected-ESP firmware destination Boot%1.</source>
        <translation>Удаление дубликата/наследства выбранного-ESP прошивки назначения Boot%1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1447"/>
        <source>EFI repair read-only preflight: PASS (%1, %2, id=%3)</source>
        <translation>EFI ремонт только для чтения перед полетом: PASS (%1, %2, id=%3)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1448"/>
        <source>EFI repair completed; simulating and regenerating the GRUB fallback configuration.</source>
        <translation>Ремонт EFI завершен; моделирование и регенерация резервной конфигурации GRUB.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1449"/>
        <source>Restoring reconciled BootNext: %1</source>
        <translation>Восстановление согласованного BootNext: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1450"/>
        <source>Restoring reconciled EFI BootOrder (all pre-existing entries retained): %1</source>
        <translation>Восстановление согласованного BootOrder EFI (сохранены все ранее существовавшие записи): %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1451"/>
        <source>Restoring %1 firmware entry &apos;%2&apos; for PARTUUID %3 (EFI path %4).</source>
        <translation>Восстановление ввода прошивки %1 «%2» для PARTUUID %3 (EFI Path %4).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1452"/>
        <source>Restoring missing %1 firmware entry on selected system ESP %2 as &apos;%3&apos; (%4).</source>
        <translation>Восстановление отсутствующей записи прошивки %1 в выбранной системе ESP %2 как «%3» (%4).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1453"/>
        <source>Restoring missing generic EFI firmware entry on selected system ESP %1 as &apos;%2&apos; (%3).</source>
        <translation>Восстановление отсутствующей общей записи прошивки EFI в выбранной системе ESP %1 как «%2» (%3).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1454"/>
        <source>Alpine EFI-stub repair read-only preflight: PASS (%1, %2)</source>
        <translation>Предполетный ремонт альпийской EFI-стубы: PASS (%1, %2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1455"/>
        <source>TUXEDO UKI firmware entry is absent for the selected system ESP; creating only that selected-system entry as &apos;%1&apos;.</source>
        <translation>Запись прошивки TUXEDO UKI отсутствует для выбранной системы ESP; создание только этой записи выбранной системы как «%1».</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1456"/>
        <source>Writable UEFI efivars detected; first attempt will permit firmware registration.</source>
        <translation>Обнаружена запись UEFI efivars; первая попытка позволит зарегистрировать прошивку.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1457"/>
        <source>Writable UEFI efivars are unavailable; using the preflight-selected --no-nvram path.</source>
        <translation>Написанные эфивары UEFI недоступны; с использованием выбранного перед полетом пути -no-nvram.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1458"/>
        <source>ESP mount: auto-mount failed: target=%1 reason=%2 hint=%3</source>
        <translation>ESP mount: Automount failed: target=%1 reason=%2 hint= %3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1459"/>
        <source>ESP mount: auto-mount failed: target=%1 reason=mounted source %2 is not a block device on the selected target disk hint=%3</source>
        <translation>ESP mount: automount failed: target=%1 reason=mounted source %2 is not a block device on the selected target disk hint= %3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1460"/>
        <source>ESP mount: auto-mount failed: target=%1 reason=mounted source %2 is %3, not FAT hint=%4</source>
        <translation>ESP mount: automount failed: target=%1 reason=mounted source %2 is %3, not FAT hint= %4</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1461"/>
        <source>ESP mount cleanup: unmounted leaked ro layer target=%1 source=%2 id=%3</source>
        <translation>Очистка крепления ESP: немонтированная утечка целевого слоя ro = источник %1 = идентификатор %2 = %3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1462"/>
        <source>ESP mount preflight: target=%1 stack=%2 top-source=%3 top-options=%4 top-id=%5 verdict=%6 leaked-ro=%7</source>
        <translation>ESP mount preflight: target=%1 stack=%2 top-source=%3 top-options=%4 top-id=%5 verdict=%6 leaked-ro=%7 top-options</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1463"/>
        <source>ESP mount: mounted target=%1 source=%2 method=%3</source>
        <translation>ESP: установленная цель = источник %1 = метод %2 = %3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1464"/>
        <source>Backed up the extlinux configuration under %1</source>
        <translation>Резервное копирование конфигурации extlinux под %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1465"/>
        <source>PASS: extlinux candidate references %1 boot artifact(s) present in the target</source>
        <translation>PASS: кандидат extlinux ссылается на загрузочный артефакт (артефакты) %1, присутствующий в цели</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1466"/>
        <source>PASS: running host default extlinux entry is LABEL %1 -&gt; LINUX %2 + INITRD %3.</source>
        <translation>PASS: запущенный хост по умолчанию extlinux вход - LABEL %1 -&gt; LINUX %2 + INITRD %3.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1467"/>
        <source>Restored the pre-repair extlinux default configuration</source>
        <translation>Восстановлена предремонтная конфигурация extlinux по умолчанию</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1468"/>
        <source>Extlinux default entry: label=%1 kernel=%2 action=set</source>
        <translation>Запись по умолчанию Extlinux: label=%1 ядро=%2 действие=set</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1469"/>
        <source>Extlinux default entry: label=%1 kernel=%2 action=unchanged</source>
        <translation>Запись по умолчанию Extlinux: label=%1 ядро=%2 действие=без изменений</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1470"/>
        <source>PASS: /boot/extlinux.conf regenerated and verified; the boot sector was not written.</source>
        <translation>PASS: /boot/extlinux.conf восстановлен и проверен; загрузочный сектор не был написан.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1471"/>
        <source>Restored the pre-repair extlinux configuration</source>
        <translation>Восстановлена предремонтная конфигурация extlinux</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1472"/>
        <source>SIMULATE/PREFLIGHT: extlinux configuration regeneration via %1 (overwrite=0 trial; no boot sector write)</source>
        <translation>SIMULATE/PREFLIGHT: регенерация конфигурации extlinux через %1 (перезапись = 0 пробная версия; загрузочный сектор не записывается)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1473"/>
        <source>Fedora default entry: saved_entry=%1 resolves=%2 target=%3 action=set</source>
        <translation>Fedora default entry: save entry=%1 resolves=%2 target=%3 action=set</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1474"/>
        <source>Fedora default entry: saved_entry=%1 resolves=yes target=%2 action=set</source>
        <translation>Fedora default entry: save entry=%1 resolves=yes target=%2 action=set</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1475"/>
        <source>Fedora default entry: saved_entry=%1 resolves=yes target=%2 action=unchanged</source>
        <translation>Fedora default entry: save entry=%1 resolves=yes target=%2 action=unchanged</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1476"/>
        <source>Backed up Fedora GRUB2 configuration artifacts to %1.</source>
        <translation>Резервное копирование артефактов конфигурации Fedora GRUB2 в %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1477"/>
        <source>PASS: Fedora GRUB2 configuration regenerated and verified.</source>
        <translation>Конфигурация Fedora GRUB2 регенерирована и проверена.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1478"/>
        <source>Restored the Fedora GRUB2 configuration artifacts from %1.</source>
        <translation>Восстановлены конфигурационные артефакты Fedora GRUB2 от %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1479"/>
        <source>SIMULATE/PREFLIGHT: generate Fedora GRUB2 configuration with --no-grubenv-update</source>
        <translation>SIMULATE/PREFLIGHT: генерировать конфигурацию Fedora GRUB2 без обновления</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1480"/>
        <source>PASS: Fedora GRUB2 trial configuration generated successfully.</source>
        <translation>Пробная конфигурация Fedora GRUB2 успешно сгенерирована.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1481"/>
        <source>Fedora default entry: grubenv keys other than saved_entry preserved (sha256 %1…).</source>
        <translation>Запись по умолчанию Fedora: сохранены ключи grubenv, отличные от save entry (sha256 %1…).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1482"/>
        <source>PASS: Fedora initramfs for %1 rebuilt byte-identical; temporary build discarded.</source>
        <translation>PASS: Fedora initramfs для %1 восстановленный байт-идентичный; временная сборка выброшена.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1483"/>
        <source>PASS: Fedora initramfs rebuilt and verified for %1</source>
        <translation>Fedora initramfs восстановлена и проверена на %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1484"/>
        <source>File Copy %1: %2</source>
        <translation>Оригинальное название: %1: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1485"/>
        <source>Simulation exit code for &apos;--fix-broken install&apos;: %1</source>
        <translation>Код выхода для симуляции &quot;--fix-broken install&quot;: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1486"/>
        <source>Released the helper&apos;s read-only mount(s) of %1 before offline repair.</source>
        <translation>Выпустили только для чтения крепление(ы) помощника %1 перед офлайн-ремонтом.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1487"/>
        <source>REPAIR: %1 (%2) on %3 (fstype=%4, uuid=%5, mount=%6)</source>
        <translation>REPAIR: %1 (%2) на %3 (fstype=%4, uuid=%5, mount=%6)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1488"/>
        <source>FAIL: file system repair (%1) reported unresolved issues for %2 (exit code %3).</source>
        <translation>FAIL: Ремонт файловой системы (%1) сообщил о нерешенных проблемах для %2 (код выхода %3).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1489"/>
        <source>PASS: file system repair (%1) corrected errors on %2 (exit code %3).</source>
        <translation>PASS: исправление ошибок файловой системы (%1) на %2 (код выхода %3).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1490"/>
        <source>PASS: file system repair (%1) completed for %2 with exit code 0.</source>
        <translation>PASS: Ремонт файловой системы (%1) завершен для %2 с выходным кодом 0.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1491"/>
        <source>PASS: file system repair (%1) corrected errors on %2; a reboot is recommended before using the filesystem (exit code 2).</source>
        <translation>PASS: исправление ошибок файловой системы (%1) на %2; перед использованием файловой системы рекомендуется перезагрузка (код выхода 2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1492"/>
        <source>FAIL: file system repair (%1) timed out for %2 (exit code %3).</source>
        <translation>FAIL: Ремонт файловой системы (%1) приурочен к %2 (код выхода %3).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1493"/>
        <source>File system inspection found no resolvable scope filesystems (root, /boot, ESP and /home).</source>
        <translation>Проверка файловой системы не обнаружила разрешимых файловых систем (root, /boot, ESP и /home).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1494"/>
        <source>File system inspection started (read-only; no repair tool is invoked).</source>
        <translation>Началась проверка файловой системы (только чтение; не используется инструмент для ремонта).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1495"/>
        <source>WARNING: fstab %1 device %2 is not on the selected disk %3; excluded from the file system scope.</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: fstab %1 устройство %2 не находится на выбранном диске %3; исключено из области действия файловой системы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1496"/>
        <source>Captured complete firmware entry state before conventional GRUB EFI install: %1</source>
        <translation>Полное состояние ввода прошивки перед обычной установкой GRUB EFI: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1497"/>
        <source>SIMULATE/PREFLIGHT: GRUB EFI install target=%1 fs=%2 id=%3 mode=%4</source>
        <translation>SIMULATE/PREFLIGHT: GRUB EFI install target=%1 fs=%2 id=%3 mode=%4</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1498"/>
        <source>foreign-entry-added: %1</source>
        <translation>Добавлено: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1499"/>
        <source>foreign-entry-removed: %1</source>
        <translation>Иностранный вход: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1500"/>
        <source>menu-entry-added: %1</source>
        <translation>Добавлено меню: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1501"/>
        <source>GRUB menu entries added by the candidate: %1</source>
        <translation>Меню GRUB, добавленное кандидатом: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1502"/>
        <source>Using grub-mkconfig with an isolated output path for preflight.</source>
        <translation>Использование grub-mkconfig с изолированным выходом для предполетного полета.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1503"/>
        <source>PASS: GRUB configuration regenerated and verified.</source>
        <translation>Конфигурация GRUB регенерирована и проверена.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1504"/>
        <source>SIMULATE/PREFLIGHT: generate GRUB configuration to temporary session output</source>
        <translation>SIMULATE/PREFLIGHT: генерация конфигурации GRUB для вывода временного сеанса</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1505"/>
        <source>KNOWN ISSUE: GRUB trial failed on a stale mapper path; retrying once after compatibility alias correction.</source>
        <translation>Испытание GRUB потерпело неудачу на устаревшем пути картографа; повторная попытка один раз после коррекции псевдонима совместимости.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1506"/>
        <source>Trial GRUB generation exit code: %1</source>
        <translation>Пробный выходной код поколения GRUB: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1507"/>
        <source>PASS: GRUB trial configuration generated successfully.</source>
        <translation>PASS: успешно сгенерированная тестовая конфигурация GRUB.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1508"/>
        <source>Backed up GRUB2 BIOS boot code (MBR, BIOS boot partition, i386-pc modules) to %1.</source>
        <translation>Резервное копирование кода загрузки GRUB2 BIOS (MBR, раздел загрузки BIOS, модули i386-pc) в %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1509"/>
        <source>NOTE: GRUB2 BIOS boot-code reinstall is only implemented for a mounted repair target; the running host stays config-only.</source>
        <translation>Примечание: Переустановка загрузочного кода GRUB2 BIOS реализована только для установленной цели ремонта; работающий хост остается только конфигурацией.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1510"/>
        <source>PASS: GRUB2 BIOS boot code reinstalled and verified (partition table byte-identical).</source>
        <translation>PASS: загрузочный код GRUB2 BIOS переустанавливается и проверяется (байт-идентичный в таблице разделов).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1511"/>
        <source>GRUB2 boot-code probe: MBR and BIOS boot partition contain a GRUB signature; config-only regeneration.</source>
        <translation>Зонд загрузочного кода GRUB2: раздел загрузки MBR и BIOS содержит сигнатуру GRUB; регенерация только конфигурацией.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1512"/>
        <source>REPAIR: reinstall GRUB2 BIOS boot code (%1 --target=i386-pc --boot-directory=/boot --recheck %2)</source>
        <translation>REPAIR: переустановить загрузочный код GRUB2 BIOS (%1 --target=i386-pc --boot-directory=/boot --recheck %2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1513"/>
        <source>Restored the GRUB2 BIOS boot code from %1.</source>
        <translation>Восстановлен загрузочный код GRUB2 BIOS от %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1514"/>
        <source>GRUB2 config-only reconciliation requested; the BIOS boot-code reinstall substage is skipped.</source>
        <translation>Запрашивается только выверка конфигурации GRUB2; подстанция переустановки загрузочного кода BIOS пропускается.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1515"/>
        <source>UEFI firmware detected; the GRUB2 BIOS boot-code reinstall substage is not applicable.</source>
        <translation>Обнаружена прошивка UEFI; подстанция переустановки загрузочного кода GRUB2 BIOS не применима.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1516"/>
        <source>Host boot-stack Arch EFI read-only preflight: PASS (%1, %2, id=%3)</source>
        <translation>Арка EFI только для чтения: PASS (%1, %2, id=%3)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1517"/>
        <source>Host boot-stack TUXEDO UKI read-only preflight: PASS (%1, %2)</source>
        <translation>Загрузочный стек TUXEDO UKI только для чтения: PASS (%1, %2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1518"/>
        <source>Host command guard: firmware variables are read-only to package/kernel/vendor hooks; the helper owns explicit firmware registration.</source>
        <translation>Охрана команды хоста: переменные прошивки считываются только для крючков пакета / ядра / поставщика; помощник владеет явной регистрацией прошивки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1519"/>
        <source>Creating host %1 firmware entry on %2 as &apos;%3&apos; (%4); existing host, repair, and foreign entries are untouched.</source>
        <translation>Создание записи прошивки хоста %1 на %2 как «%3» (%4); существующие записи хоста, ремонта и посторонние записи не затронуты.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1520"/>
        <source>PASS: running host default EFI entry is Boot%1 on %2.</source>
        <translation>PASS: Запуск хоста по умолчанию EFI является Boot%1 на %2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1521"/>
        <source>WARNING: host destination is group-writable; the copied files may be modified by the owning group: %1</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: назначение хоста является групповым; скопированные файлы могут быть изменены владеющей группой: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1522"/>
        <source>Host disk: %1</source>
        <translation>Диск хоста: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1523"/>
        <source>Backed up running-host EFI loader files to %1 before the guarded reinstall.</source>
        <translation>Резервное копирование файлов загрузчика EFI в %1 перед защищенной переустановкой.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1524"/>
        <source>Canonical running-host EFI loader verified: %1 (%2) on %3.</source>
        <translation>Канонический загрузчик EFI проверен: %1 (%2) на %3.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1525"/>
        <source>Host EFI read-only preflight: PASS (%1, %2, id=%3)</source>
        <translation>Ведущий EFI только для чтения: PASS (%1, %2, id=%3)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1526"/>
        <source>EFI repair completed; regenerating the running host GRUB fallback configuration.</source>
        <translation>Завершен ремонт EFI; регенерация конфигурации резервного хоста GRUB.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1527"/>
        <source>Restored the running-host EFI loader files from %1.</source>
        <translation>Восстановил файлы загрузчика EFI из %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1528"/>
        <source>Host Alpine EFI-stub read-only preflight: PASS (%1, %2)</source>
        <translation>Ведущий Alpine EFI-stub только для чтения: PASS (%1, %2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1529"/>
        <source>Writable UEFI variables are unavailable; host maintenance will preserve files and BootOrder without firmware registration.</source>
        <translation>Записываемые переменные UEFI недоступны; обслуживание хоста будет сохранять файлы и BootOrder без регистрации прошивки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1530"/>
        <source>Host %1 entries %2 point to %3 on %4; keeping active entry Boot%5 and pruning the remaining duplicates.</source>
        <translation>Ведущие %1 записи %2 указывают на %3 на %4; сохраняя активный вход Boot%5 и обрезая оставшиеся дубликаты.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1531"/>
        <source>Host EFI System Partition: %1 (%2) %3</source>
        <translation>Системный раздел EFI: %1 (%2) %3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1532"/>
        <source>Native running-host repair preflight: PASS</source>
        <translation>Предполетный ремонт хозяина: PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1533"/>
        <source>All requested running-host repair stages completed successfully.</source>
        <translation>Все запрошенные этапы ремонта бегунов успешно завершены.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1534"/>
        <source>ROLLBACK: restored BootOrder=%1, but the firmware state differs from the pre-change capture:</source>
        <translation>ROLLBACK: восстановленный BootOrder=%1, но состояние прошивки отличается от захвата перед изменением:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1535"/>
        <source>CRITICAL: cannot detach the promoted host rollback mounts; refusing to rename Btrfs roots during recovery.</source>
        <translation>КРИТИЧЕСКИЙ: не может отсоединить продвигаемые хосты откатных креплений; отказываясь переименовать корни Btrfs во время восстановления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1536"/>
        <location filename="../src/MainWindow.cpp" line="1645"/>
        <source>CRITICAL: could not determine the preserved root subvolume ID.</source>
        <translation>КРИТИЧЕСКИЙ: не смог определить сохраненный идентификатор подобъема корня.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1537"/>
        <source>CRITICAL: could not move the failed host rollback candidate out of @; refusing to promote the preserved root.</source>
        <translation>КРИТИЧЕСКИЙ: не смог переместить несостоявшегося хоста-кандидата на откат из @; отказавшись продвигать сохранившийся корень.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1538"/>
        <source>CRITICAL: could not move the nested @/.snapshots child back into the preserved root.</source>
        <translation>КРИТИЧЕСКИЙ: не смог переместить вложенного @/.snapshots ребенка обратно в сохранившийся корень.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1539"/>
        <source>CRITICAL: could not remount the Btrfs top-level filesystem read-write during host rollback recovery.</source>
        <translation>CRITICAL: не удалось перемонтировать файловую систему верхнего уровня Btrfs во время восстановления отката хоста.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1540"/>
        <location filename="../src/MainWindow.cpp" line="1648"/>
        <source>CRITICAL: could not restore the previous Btrfs default subvolume.</source>
        <translation>CRITICAL: не удалось восстановить предыдущий объем Btrfs по умолчанию.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1541"/>
        <source>RUNNING-HOST SNAPSHOT ROLLBACK COMPLETE</source>
        <translation>Бегущий-хозяин SNAPSHOT ROLLBACK COMPLETE</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1542"/>
        <source>ROLLBACK: created entry Boot%1 is active (BootCurrent/BootNext); it is retained but no longer promoted.</source>
        <translation>Boot%1 активен (BootCurrent/BootNext); он сохраняется, но больше не продвигается.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1543"/>
        <location filename="../src/MainWindow.cpp" line="1649"/>
        <source>Creating writable rollback candidate %1</source>
        <translation>Создание письменного кандидата на откат %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1544"/>
        <source>Host rollback default-subvolume update failed; automatically restoring the preserved root.</source>
        <translation>Обновление хоста по умолчанию не удалось; автоматически восстанавливается сохраненный корень.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1545"/>
        <source>Pre-rollback boot artifact fingerprints: fstab=%1 grub=%2 cmdline=%3 uki=%4</source>
        <translation>Отпечатки пальцев артефакта перед загрузкой: fstab=%1 grub=%2 cmdline=%3 uki=%4</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1546"/>
        <source>Migrating nested @/.snapshots child subvolume into the promoted root.</source>
        <translation>Миграция вложенного @/.snapshots ребенка в продвигаемый корень.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1547"/>
        <source>Host rollback candidate mount/preflight failed; automatically restoring the preserved root.</source>
        <translation>Кандидат на откат хоста mount/preflight потерпел неудачу; автоматически восстанавливается сохраненный корень.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1548"/>
        <source>PASS: the original @ and its boot stack were restored automatically. Failed rollback candidate retained as %1.</source>
        <translation>PASS: оригинальный @ и его багажник были восстановлены автоматически. Неудавшийся кандидат на откат сохранился как %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1549"/>
        <source>CRITICAL: the original @ was restored, but its boot-stack reconciliation also failed. Manual boot repair is required before reboot.</source>
        <translation>КРИТИЧЕСКИЙ: оригинал @ был восстановлен, но его примирение с загрузочным стеком также провалилось. Перед перезагрузкой требуется ручной ремонт багажника.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1550"/>
        <location filename="../src/MainWindow.cpp" line="1670"/>
        <source>CRITICAL: preserved root %1 is missing; refusing to move the active rollback candidate.</source>
        <translation>Сохраненный корень %1 отсутствует; отказ от перемещения активного откатного кандидата.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1551"/>
        <location filename="../src/MainWindow.cpp" line="1705"/>
        <source>Previous root retained as: %1</source>
        <translation>Корень сохранился как: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1552"/>
        <source>Promoted root: @ (subvolume ID %1); the running system keeps the previous root until reboot.</source>
        <translation>Продвигаемый корень: @ (подобный ID %1); запущенная система сохраняет предыдущий корень до перезагрузки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1553"/>
        <source>HOST ROLLBACK RECOVERY: restoring the preserved pre-rollback @.</source>
        <translation>HOST ROLLBACK RECOVERY: Восстановление сохраненного предварительного отката @.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1554"/>
        <source>ROLLBACK: restored BootOrder=%1 and removed created entry Boot%2; firmware state equals the pre-change capture.</source>
        <translation>ROLLBACK: восстановленный BootOrder=%1 и удаленный созданный вход Boot%2; состояние прошивки равно захвату перед изменением.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1555"/>
        <source>Running-host rollback selected: snapshot %1.</source>
        <translation>Выбранный откат бегущего хоста: снимок %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1556"/>
        <source>Rollback source remains unchanged; the running host keeps the current root until reboot.</source>
        <translation>Источник Rollback остается неизменным; работающий хост сохраняет текущий корень до перезагрузки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1557"/>
        <source>Selected rollback target: %1</source>
        <translation>Выбранная цель отката: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1558"/>
        <source>Host rollback candidate failed post-switch validation/boot reconciliation; automatically restoring the preserved root.</source>
        <translation>Кандидат на откат хоста не прошел проверку после переключения / сверку загрузки; автоматически восстанавливается сохраненный корень.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1559"/>
        <source>Host root component: %1 (%2)</source>
        <translation>Корневой компонент хоста: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1560"/>
        <source>Host root mount: / (subvolume=%1)</source>
        <translation>Установка корня хоста: / (подобный = %1)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1561"/>
        <source>FAIL: Running-host shell command was cancelled at an interactive prompt.</source>
        <translation>Неудача: команда оболочки бегущего хозяина была отменена в интерактивном режиме.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1562"/>
        <source>Running-host shell exit code: %1</source>
        <translation>Код выхода оболочки Running-host: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1563"/>
        <source>FAIL: Running-host shell command (exit code %1)</source>
        <translation>FAIL: команда оболочки Running-host (код выхода %1)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1564"/>
        <source>PASS: Running-host shell command</source>
        <translation>PASS: команда Running-host shell</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1565"/>
        <source>Host snapshot inventory skipped: %1 is not Btrfs.</source>
        <translation>Пропущенный инвентарь снимков хоста: %1 не является Btrfs.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1566"/>
        <source>Host TUXEDO UKI firmware entry already present: Boot%1 on %2.</source>
        <translation>Ввод прошивки TUXEDO UKI уже присутствует: Boot%1 на %2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1567"/>
        <source>Host TUXEDO UKI file is present on %1, but firmware variables are not writable; host registration was not changed.</source>
        <translation>Файл Host TUXEDO UKI присутствует на %1, но переменные прошивки не записываются; регистрация хоста не была изменена.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1568"/>
        <source>Host TUXEDO UKI read-only preflight: PASS (%1, %2)</source>
        <translation>Ведущий TUXEDO UKI только для чтения: PASS (%1, %2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1569"/>
        <source>PASS: host TUXEDO UKI firmware entry restored as Boot%1 on %2.</source>
        <translation>PASS: вход прошивки TUXEDO UKI восстановлен в виде Boot%1 на %2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1570"/>
        <source>Restoring missing host TUXEDO UKI firmware entry on %1 as &apos;%2&apos;; existing host, repair, and foreign entries are untouched.</source>
        <translation>Восстановление отсутствующей записи прошивки TUXEDO UKI на %1 как «%2»; существующие записи хоста, ремонта и посторонние записи нетронуты.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1571"/>
        <source>Running-host validation complete; no host files were changed.</source>
        <translation>Проверка запуска-хоста завершена; никакие файлы хоста не были изменены.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1572"/>
        <source>Running-host validation summary</source>
        <translation>Резюме проверки запускающего хоста</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1573"/>
        <source>KNOWN ISSUE: %1 is missing; creating it instead of attempting an update.</source>
        <translation>%1 отсутствует; создание его вместо попытки обновления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1574"/>
        <source>PASS: initramfs %1 verified by mkinitfs -l build-input listing</source>
        <translation>PASS: initramfs %1 подтвержден списком встроенных входов mkinitfs-l</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1575"/>
        <source>SIMULATE/PREFLIGHT: initramfs generation for %1 installed kernel(s)</source>
        <translation>SIMULATE/PREFLIGHT: генерация initramfs для установленного ядра %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1576"/>
        <source>PASS: trial initramfs build for %1</source>
        <translation>PASS: пробная сборка initramfs для %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1577"/>
        <source>PASS: initramfs verified for %1</source>
        <translation>PASS: initramfs подтвержден для %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1578"/>
        <source>PASS: initramfs %1 verified by zcat/cpio listing</source>
        <translation>PASS: initramfs %1 подтвержден списком zcat/cpio</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1579"/>
        <source>KNOWN ISSUE: initrd.img-%1 is missing; creating it instead of attempting an update.</source>
        <translation>Initrd.img-%1 отсутствует; его создание вместо попытки обновления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1580"/>
        <source>%1 lives on the root filesystem; no separate mount required</source>
        <translation>%1 работает в корневой файловой системе; отдельного крепления не требуется.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1581"/>
        <source>PASS: lsinitcpio verified %1 Arch initramfs image(s)</source>
        <translation>PASS: lsinitcpio verified %1 Arch initramfs image(s)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1582"/>
        <source>PASS: lsinitramfs verified /boot/initrd.img-%1</source>
        <translation>PASS: lsinitramfs verified /boot/initrd.img-%1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1583"/>
        <source>PASS: lsinitrd verified %1</source>
        <translation>PASS: верифицированный %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1584"/>
        <source>LUKS target is already unlocked by existing mapper: %1</source>
        <translation>Цель LUKS уже разблокирована существующим картографом: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1585"/>
        <source>Mapper name: %1</source>
        <translation>Оригинальное название: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1586"/>
        <source>LUKS unlock complete. The mapper remains open for this recovery session.</source>
        <translation>Разблокировка LUKS завершена. Картограф остается открытым для этой сессии восстановления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1587"/>
        <source>Unlocking LUKS target %1</source>
        <translation>Разблокировка цели LUKS %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1588"/>
        <source>Mandatory safety preflight: PASS</source>
        <translation>Обязательный предполет безопасности: PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1589"/>
        <source>Mapper compatibility: %1 already resolves to %2</source>
        <translation>Совместимость с картером: %1 уже разрешен для %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1590"/>
        <source>Mapper compatibility: temporary %1 -&gt; %2</source>
        <translation>Совместимость с маппером: временный %1 -&gt; %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1591"/>
        <source>SIMULATE/PREFLIGHT: mkinitcpio trial builds for %1 installed kernel(s)</source>
        <translation>SIMULATE/PREFLIGHT: тестовые сборки mkinitcpio для установленного ядра %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1592"/>
        <source>PASS: trial mkinitcpio build for %1</source>
        <translation>PASS: пробная сборка mkinitcpio для %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1593"/>
        <source>SIMULATE/PREFLIGHT: mkinitfs trial builds for %1 installed kernel(s)</source>
        <translation>SIMULATE/PREFLIGHT: тестовые сборки mkinitfs для установленного ядра %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1594"/>
        <source>PASS: trial mkinitfs build for %1</source>
        <translation>PASS: пробная сборка mkinitfs для %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1595"/>
        <source>Mounted target ESP discovered by GPT type: %1 at /boot/efi (%2)</source>
        <translation>Установленная цель ESP, обнаруженная по типу GPT: %1 в /boot/efi (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1596"/>
        <source>Mounted target %1 from %2 (%3)</source>
        <translation>Установленная цель %1 от %2 (%3)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1597"/>
        <source>Mounting target %1 from %2 (%3)</source>
        <translation>Установка цели %1 из %2 (%3)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1598"/>
        <source>Mounting target Btrfs fstab subvolumes (%1)</source>
        <translation>Установка целевых субобъемов Btrfs fstab (%1)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1599"/>
        <source>Mounting target %1 from %2</source>
        <translation>Установка цели %1 из %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1600"/>
        <source>No TUXEDO UKI builder detected; retaining the distribution&apos;s existing EFI layout.</source>
        <translation>Строитель TUXEDO UKI не обнаружен, сохранив существующую компоновку дистрибутива EFI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1601"/>
        <source>FAIL: ntfsfix could not fully repair %1 (exit code %2); run Windows chkdsk /f for a real NTFS repair.</source>
        <translation>FAIL: ntfsfix не смог полностью восстановить %1 (код выхода %2); запустить Windows chkdsk /f для реального ремонта NTFS.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1602"/>
        <source>NOTE: ntfsfix only clears the NTFS dirty state; Windows chkdsk /f is required for a real NTFS repair.</source>
        <translation>Примечание: ntfsfix очищает только грязное состояние NTFS; Windows chkdsk /f требуется для реального ремонта NTFS.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1603"/>
        <source>Ownership validation: %1:%2 maps to %3:%4 on both sides; preserving numeric ownership</source>
        <translation>Проверка прав собственности: карты %1:%2 для %3:%4 с обеих сторон; сохранение числового владения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1604"/>
        <source>Ownership validation: source %1:%2 (%3:%4) does not map identically on the destination side; using destination owner %5:%6</source>
        <translation>Проверка прав собственности: источник %1:%2 (%3:%4) не отображается одинаково на стороне назначения; используя владельца назначения %5:%6</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1605"/>
        <source>Ownership policy: %1</source>
        <translation>Политика владения: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1606"/>
        <source>Package backend %1 did not report a change status; treating the stage as changed.</source>
        <translation>Пакетный бэкэнд %1 не сообщал об изменении статуса; рассматривая стадию как изменённую.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1607"/>
        <source>SKIP: package backend %1 is not runnable for stage &apos;%2&apos;: %3</source>
        <translation>SKIP: пакетный бэкэнд %1 не работает для этапа %2: %3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1608"/>
        <source>Package backend %1 change status: %2</source>
        <translation>Пакет обновления %1 Change Status: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1609"/>
        <source>Package stage &apos;%1&apos;: running %2 runnable backend(s): %3</source>
        <translation>Этап пакета «%1»: запускается %2 управляемый бэкэнд(ы): %3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1610"/>
        <source>PackageKit daemon is running; it is only a conflict while it holds package-manager locks or spawns apt/dpkg.</source>
        <translation>Демон PackageKit работает; это только конфликт, в то время как он держит блокировщик пакетов или порождает apt / dpkg.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1611"/>
        <source>Arch pacman transaction completed successfully despite mirror fallback.</source>
        <translation>Транзакция Arch pacman успешно завершена, несмотря на зеркальный откат.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1612"/>
        <source>WARN: pacman encountered %1 recoverable mirror retrieval failure(s).</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: pacman столкнулся с неисправностью (неисправностями) восстановления зеркала %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1613"/>
        <source>PASS: Arch pacman transaction preflight resolved without removals.</source>
        <translation>PASS: предполетная транзакция Arch pacman разрешена без удаления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1614"/>
        <source>REFUSED: pacman preflight proposes %1 packages (safety limit: 1000).</source>
        <translation>Предполетный рейс pacman предлагает пакеты %1 (ограничение безопасности: 1000).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1615"/>
        <source>REFUSED: pacman preflight reported repository, download or transaction integrity errors.</source>
        <translation>Предполетный отчет pacman об ошибках репозитория, загрузки или целостности транзакции.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1616"/>
        <source>REFUSED: pacman preflight proposes removals or unresolved dependencies.</source>
        <translation>Предполетный рейс pacman предлагает удаление или неразрешенные зависимости.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1617"/>
        <source>Arch pacman transaction sandbox prepared under %1; the target package database will not be used for preflight.</source>
        <translation>Arch pacman — песочница транзакций, подготовленная под %1; база данных целевого пакета не будет использоваться для предварительного полета.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1618"/>
        <source>SIMULATE/PREFLIGHT: full Arch pacman transaction (sandboxed database/cache; no target packages will be changed)</source>
        <translation>SIMULATE/PREFLIGHT: полная транзакция Arch pacman (база данных/кэш в песочнице; целевые пакеты не будут изменены)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1619"/>
        <source>PASS: dnf5 metadata cache refreshed.</source>
        <translation>PASS: кэш метаданных dnf5 обновлен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1620"/>
        <source>PASS: %1</source>
        <translation>PASS: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1621"/>
        <source>PASS: %1 completed through one full pacman transaction.</source>
        <translation>%1 завершается через одну полную транзакцию pacman.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1622"/>
        <source>PASS: Refresh package metadata</source>
        <translation>PASS: Обновить метаданные пакета</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1623"/>
        <source>PASS: Refresh package metadata (package lists byte-identical; no repository index was fetched)</source>
        <translation>PASS: Обновить метаданные пакета (списки пакетов байт-идентичные; индекс хранилища не был получен)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1624"/>
        <source>Host package-manager concurrency gate: PASS</source>
        <translation>Host package-manager concurrency gate: PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1625"/>
        <source>Post-upgrade dpkg audit:</source>
        <translation>После обновления dpkg:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1626"/>
        <source>PREVIEW COMPLETE — no files were changed.</source>
        <translation>PREVIEW COMPLETE — файлы не были изменены.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1627"/>
        <source>PREVIEW: target destination directory would be created: %1</source>
        <translation>PREVIEW: будет создан каталог целевого назначения: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1628"/>
        <source>Protected host check: PASS</source>
        <translation>Защищенная проверка хоста: PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1629"/>
        <source>Scheduling running-host reboot through the system reboot command.</source>
        <translation>Планирование перезагрузки хоста через команду перезагрузки системы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1630"/>
        <source>Scheduling running-host reboot through OpenRC.</source>
        <translation>Планирование перезагрузки бегущего хоста через OpenRC.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1631"/>
        <source>Scheduling running-host reboot through systemd.</source>
        <translation>Планирование перезагрузки бегущего хоста через систему.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1632"/>
        <source>WARNING: refusing an unsafe target Btrfs mountpoint (not absolute): %1</source>
        <translation>Предупреждение: отказ от небезопасной цели Btrfs (не абсолютный): %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1633"/>
        <source>WARNING: refusing an unsafe target Btrfs subvolume mount path: %1</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: отказ от небезопасной траектории установки подкачки Btrfs: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1634"/>
        <source>WARNING: refusing an unsafe target %1 mount path: %2</source>
        <translation>Предупреждение: отказ от небезопасной траектории установки %1: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1635"/>
        <source>Remounting confirmed target filesystem read-write for file copy</source>
        <translation>Восстановление подтвержденной целевой файловой системы read-write для копирования файла</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1636"/>
        <source>Remounting confirmed target root read-write</source>
        <translation>Восстановление подтвержденного целевого корня read-write</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1637"/>
        <source>Remounting target data filesystem %1 read-write</source>
        <translation>Восстановление файловой системы целевых данных %1 read-write</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1638"/>
        <source>Remounting target %1 read-write</source>
        <translation>Восстановление целевой %1 read-write</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1639"/>
        <source>All requested repair stages completed successfully.</source>
        <translation>Все требуемые этапы ремонта успешно завершены.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1640"/>
        <source>Rollback boot-stack reconciliation: using simulation-first adaptive component workflows.</source>
        <translation>Выверка загрузочного стека в обратном направлении: использование рабочих процессов адаптивных компонентов первого моделирования.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1641"/>
        <source>Rollback candidate default-subvolume update failed; automatically restoring the preserved root.</source>
        <translation>Обновление Rollback-кандидата по умолчанию не удалось; автоматически восстанавливается сохраненный корень.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1642"/>
        <source>Rollback candidate mount/preflight failed; automatically restoring the preserved root.</source>
        <translation>Откат кандидата mount/preflight не удался; автоматически восстанавливается сохраненный корень.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1643"/>
        <source>Rollback candidate failed post-switch validation/boot reconciliation; automatically restoring the preserved root.</source>
        <translation>Кандидат в откат провалил проверку после переключения / сверку загрузки; автоматически восстанавливая сохраненный корень.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1644"/>
        <source>CRITICAL: cannot detach promoted rollback mounts; refusing to rename Btrfs roots during recovery.</source>
        <translation>КРИТИЧЕСКИЙ: не может отсоединять продвигаемые откатные крепления; отказ от переименования корней Btrfs при восстановлении.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1646"/>
        <source>CRITICAL: could not move failed rollback candidate out of @; refusing to promote the preserved root.</source>
        <translation>КРИТИЧЕСКИЙ: не смог выдвинуть несостоявшегося кандидата на откат из @; отказавшись продвигать сохранившийся корень.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1647"/>
        <source>CRITICAL: could not remount the Btrfs top-level filesystem read-write during rollback recovery.</source>
        <translation>CRITICAL: не удалось перемонтировать файловую систему верхнего уровня Btrfs во время восстановления отката.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1650"/>
        <source>CRITICAL ROLLBACK FAILURE: could not restore original @ after default-subvolume failure. Do not reboot.</source>
        <translation>CRITICAL ROLLBACK FAILURE: не удалось восстановить исходный @ после сбоя в объеме по умолчанию. Не перезагружай.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1651"/>
        <source>CRITICAL ROLLBACK FAILURE: automatic restoration was incomplete. Do not reboot until Btrfs/boot state is inspected manually.</source>
        <translation>КРИТИЧЕСКИЙ РОЛЛБАК ФАЙЛУР: Автоматическое восстановление было неполным. Не перезагружайтесь до тех пор, пока состояние Btrfs/boot не будет проверено вручную.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1652"/>
        <source>CRITICAL ROLLBACK FAILURE: could not restore original @ after candidate mount failure. Do not reboot.</source>
        <translation>CRITICAL ROLLBACK FAILURE: не удалось восстановить исходный @ после сбоя установки кандидата. Не перезагружай.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1653"/>
        <source>Detaching target mounts before atomic @ name switch.</source>
        <translation>Отсоединяя целевые установки перед атомным переключателем имени.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1654"/>
        <source>ROLLBACK FAILED SAFELY: original @ restored; failed candidate retained as %1.</source>
        <translation>ROLLBACK FAILED SAFELY: оригинальный @ восстановлен; неудавшийся кандидат сохранился как %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1655"/>
        <source>ROLLBACK FAILED SAFELY: original @ restored after default-subvolume failure; failed candidate retained as %1.</source>
        <translation>ROLLBACK FAILED SAFELY: оригинальный @ восстановлен после отказа по умолчанию; неудавшийся кандидат сохранился как %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1656"/>
        <source>ROLLBACK FAILED SAFELY: original @ restored after candidate mount failure; failed candidate retained as %1.</source>
        <translation>ROLLBACK FAILED SAFELY: оригинальный @ восстановлен после отказа установки; неудавшийся кандидат сохранен как %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1657"/>
        <source>Rollback kernel %1 is missing initramfs before rebuild; update-initramfs will be asked to regenerate it.</source>
        <translation>Ядро Роллбэка %1 отсутствует initramfs перед восстановлением; update-initramfs будет предложено регенерировать его.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1658"/>
        <source>Rollback kernel %1 has no matching modules directory.</source>
        <translation>Ядро Rollback %1 не имеет каталога соответствующих модулей.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1659"/>
        <source>Rollback kernel pair before rebuild: %1</source>
        <translation>Пара ядер отката перед восстановлением: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1660"/>
        <source>Rollback kernel pair before rebuild: %1 (mkinitcpio flavor naming)</source>
        <translation>Пара ядер отката перед восстановлением: %1 (название вкуса mkinitcpio)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1661"/>
        <source>PASS: original @ and its boot stack were restored automatically. Failed rollback candidate retained as %1.</source>
        <translation>PASS: оригинальный @ и его багажник были восстановлены автоматически. Неудавшийся кандидат на откат сохранился как %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1662"/>
        <source>CRITICAL: original @ was restored, but its boot-stack reconciliation also failed. Manual boot repair is required before reboot.</source>
        <translation>КРИТИЧЕСКИЙ: Оригинальный @ был восстановлен, но его сверка загрузочного стека также не удалась. Перед перезагрузкой требуется ручной ремонт багажника.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1663"/>
        <source>Rollback preflight: snapshot root fstab is compatible with promoted @.</source>
        <translation>Rollback preflight: snapshot root fstab совместим с промоутером @.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1664"/>
        <source>Rollback preflight: snapshot fstab has no root (/) entry.</source>
        <translation>Предварительный полет в обратном направлении: снимок fstab не имеет корневой (/) записи.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1665"/>
        <source>Rollback preflight: snapshot root fstab type is &apos;%1&apos;, not btrfs.</source>
        <translation>Перед полетом: снимок корня fstab типа «%1», а не btrfs.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1666"/>
        <source>Rollback preflight: snapshot fstab expects root subvolume &apos;%1&apos;, but transactional rollback promotes the selected snapshot to @.</source>
        <translation>Rollback preflight: snapshot fstab ожидает root subvolume &apos;%1&apos;, но транзакционный откат продвигает выбранный снимок на @.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1667"/>
        <source>Rollback preflight: snapshot / fstab source cannot be resolved: %1</source>
        <translation>Откат перед полетом: снимок / источник fstab не может быть разрешен: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1668"/>
        <source>Rollback preflight: snapshot / fstab root source has an unsupported format: %1</source>
        <translation>Rollback preflight: snapshot / fstab root source имеет неподдерживаемый формат: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1669"/>
        <source>Rollback preflight: snapshot / fstab source resolves to %1 rather than selected root %2.</source>
        <translation>Откат перед полетом: источник моментального снимка / fstab разрешает %1, а не выбранный корень %2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1671"/>
        <source>Preserved root name: %1</source>
        <translation>Оригинальное название: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1672"/>
        <source>PASS: promoted rollback root and boot stack validated.</source>
        <translation>PASS: продвигаемый корень отката и стек загрузки проверены.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1673"/>
        <source>ROLLBACK RECOVERY: restoring preserved pre-rollback @.</source>
        <translation>ROLLBACK RECOVERY: Восстановление сохраненного предварительного отката @.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1674"/>
        <source>Transactional rollback selected: snapshot %1 (%2).</source>
        <translation>Выбран транзакционный откат: снимок %1 (%2).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1675"/>
        <source>Rollback source snapshot will remain unchanged.</source>
        <translation>Снимок источника Rollback останется неизменным.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1676"/>
        <source>Root component: %1 (%2)</source>
        <translation>Корневой компонент: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1677"/>
        <source>Root component fallback: selected component %1 (%2) lacks /etc/os-release; resolved %3 (%4) from %5.</source>
        <translation>Корневой компонент: выбранный компонент %1 (%2) не имеет /etc/os-release; разрешенный %3 (%4) от %5.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1678"/>
        <source>RPM missing-file repair: reinstalling %1 package(s) with missing files: %2</source>
        <translation>Восстановление недостающих файлов RPM: переустановка пакета (пакетов) %1 с отсутствующими файлами: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1679"/>
        <source>RPM missing-file detection: no missing package files; recording read-only dnf5 dependency-check evidence.</source>
        <translation>Обнаружение отсутствующих файлов RPM: отсутствие отсутствующих файлов пакетов; запись только для чтения dnf5.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1680"/>
        <source>rpm preflight version: %1</source>
        <translation>Предполетная версия: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1681"/>
        <source>Running-host identity and boot-mount check: PASS</source>
        <translation>Идентификация бегущего хозяина и проверка загрузки: PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1682"/>
        <source>Security: host-to-repair copies keep -aHAX (modes, ownership and xattrs are copied from the trusted host source).</source>
        <translation>Безопасность: копии хоста для ремонта сохраняют -aHAX (режимы, права собственности и xattrs копируются из надежного источника хоста).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1683"/>
        <source>Security: setuid/setgid bits and file capabilities are removed on repair-to-host copies.</source>
        <translation>Безопасность: биты setuid/setgid и возможности файлов удаляются на копиях для восстановления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1684"/>
        <source>Selected EFI System Partition: %1 (%2) mounted at %3</source>
        <translation>Выбранный системный раздел EFI: %1 (%2), установленный на %3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1685"/>
        <source>SELinux enforcement: %1</source>
        <translation>Системные требования SELinux: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1686"/>
        <source>SELinux status: %1</source>
        <translation>Состояние SELinux: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1687"/>
        <source>SELinux status: sestatus/getenforce are not installed in the target; recorded as unavailable.</source>
        <translation>Статус SELinux: сестатус/гетенфорс не устанавливаются в цель; регистрируются как недоступные.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1688"/>
        <source>========================================</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1689"/>
        <source>Session log was NOT appended into the target: unsafe target log path.</source>
        <translation>Журнал сеанса не был добавлен в цель: небезопасный путь регистрации цели.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1690"/>
        <source>SIMULATE: apk %1 --simulate (no packages will be changed)</source>
        <translation>SIMULATE: apk %1 - симуляция (пакеты не будут изменены)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1691"/>
        <source>SIMULATE: apt-get %1 (no packages will be changed)</source>
        <translation>SIMULATE: apt-get %1 (пакеты не будут изменены)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1692"/>
        <source>SIMULATE CORRECTION: %1</source>
        <translation>Упрощенная коррекция: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1693"/>
        <source>SIMULATE: dnf5 %1 --assumeno (no packages will be changed)</source>
        <translation>SIMULATE: dnf5 %1-assumeno (пакеты не будут изменены)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1694"/>
        <source>SIMULATE: apt-get --fix-broken install (no packages will be changed)</source>
        <translation>SIMULATE: установка apt-get-fix-broken (пакеты не будут изменены)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1695"/>
        <source>Simulation exit code for &apos;%1&apos;: %2</source>
        <translation>Код выхода симуляции для %1: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1696"/>
        <source>WARNING: skipping unresolved Btrfs fstab entry %1 -&gt; %2</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: пропуск неразрешенной записи Btrfs fstab %1 -&gt; %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1697"/>
        <source>WARNING: skipping unresolved data fstab entry %1 -&gt; %2</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: пропуск неразрешенной записи fstab данных %1 -&gt; %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1698"/>
        <source>Target has snapper apt hooks; guarding the chroot shell against snapshots (temporary, read-only)</source>
        <translation>Цель имеет крючки apt; защита раковины корня от снимков (временный, только для чтения)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1699"/>
        <source>WARNING: could not bind the read-only snapper guard over %1; the chroot shell continues without the snapshot kill-switch.</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: не может связать охрану только для чтения над %1; оболочка корня продолжается без переключателя.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1700"/>
        <source>Active writable root: @ (subvolume ID %1)</source>
        <translation>Активный корень записи: @ (подобный ID %1)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1701"/>
        <source>Initramfs/UKI/GRUB reconciliation: PASS</source>
        <translation>Выверка initramfs/UKI/GRUB: PASS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1702"/>
        <source>Btrfs default subvolume now points to the promoted @.</source>
        <translation>Объём по умолчанию Btrfs теперь указывает на продвигаемый @.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1703"/>
        <source>Snapshot inventory complete: %1 snapshot(s) found.</source>
        <translation>Завершена инвентаризация снимков: найден снимок (снимки) %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1704"/>
        <source>Snapshot inventory skipped: %1 is not Btrfs.</source>
        <translation>Пропущенный инвентарь снимков: %1 не является Btrfs.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1706"/>
        <source>SNAPSHOT ROLLBACK COMPLETE</source>
        <translation>Снэпшот Роллбэк комплет</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1707"/>
        <source>Selected snapshot: %1</source>
        <translation>Выбранный снимок: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1708"/>
        <source>Source snapshot retained unchanged.</source>
        <translation>Источник фото сохранился без изменений.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1709"/>
        <source>Sources: %1</source>
        <translation>Источник: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1710"/>
        <source>Target /dev: private writable tmpfs populated from the recovery host with a private devpts at /dev/pts; the running host /dev and its ptys are not modified or leaked.</source>
        <translation>Target /dev: частные записываемые tmpfs, загруженные из хоста восстановления с частными devpts в /dev/pts; запущенный хост /dev и его ptys не изменены или не просочились.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1711"/>
        <source>Target disk: %1</source>
        <translation>Целевой диск: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1712"/>
        <source>Target root mount: %1 (subvolume=%2)</source>
        <translation>Целевой корневой крепление: %1 (подобный = %2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1713"/>
        <source>TRY exit code: %1 (%2)</source>
        <translation>Код выхода TRY: %1 (%2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1714"/>
        <source>TRY: %1</source>
        <translation>Попробуй: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1715"/>
        <source>PASS: TUXEDO UKI root/LUKS/subvolume binding verified.</source>
        <translation>Проверено связывание TUXEDO UKI root/LUKS/subvolume.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1716"/>
        <source>Captured complete firmware entry state before UKI rebuild: %1</source>
        <translation>Полное состояние ввода прошивки перед восстановлением UKI: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1717"/>
        <source>TUXEDO UKI cmdline: %1</source>
        <translation>TUXEDO UKI cmdline: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1718"/>
        <source>TUXEDO UKI embedded kernel: %1</source>
        <translation>Встроенное ядро TUXEDO UKI: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1719"/>
        <source>PASS: existing TUXEDO UKI validated for %1 on selected ESP %2; no replacement image was needed.</source>
        <translation>PASS: существующий TUXEDO UKI, подтвержденный для %1 на выбранном ESP %2; замена изображения не требуется.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1720"/>
        <source>KNOWN ISSUE: UKI kernel is stale; vendor UKI rebuild will correct it.</source>
        <translation>Ядро UKI устарело; реконструкция UKI исправит его.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1721"/>
        <source>TUXEDO UKI layout detected; simulating/prereflighting the vendor boot path before rebuild.</source>
        <translation>Обнаружена компоновка TUXEDO UKI; имитация/предварительный полет пути загрузки поставщика перед восстановлением.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1722"/>
        <source>KNOWN ISSUE: newest TUXEDO kernel %1 has no matching initramfs; rebuilding initramfs before UKI.</source>
        <translation>Новейшее ядро TUXEDO %1 не имеет соответствующего initramfs; восстановление initramfs до UKI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1723"/>
        <source>Writable UEFI variables/efibootmgr are unavailable; UKI file rebuild will proceed without BootOrder restoration.</source>
        <translation>Перезаписываемые переменные UEFI/efibootmgr недоступны; восстановление файла UKI будет происходить без восстановления BootOrder.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1724"/>
        <source>WARNING: objcopy could not inspect the rebuilt UKI .uname section.</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: Обжкопия не смогла осмотреть перестроенный раздел UKI .uname.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1725"/>
        <source>UKI preflight: current embedded kernel=%1; target newest kernel=%2</source>
        <translation>UKI preflight: текущее встроенное ядро = %1; целевое новейшее ядро = %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1726"/>
        <source>PASS: TUXEDO UKI preflight prerequisites are satisfied.</source>
        <translation>Предварительные условия полета TUXEDO UKI выполнены.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1727"/>
        <source>PASS: TUXEDO UKI rebuilt for %1 on selected ESP %2</source>
        <translation>TUXEDO UKI восстановлен для %1 на выбранном ESP %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1728"/>
        <source>TUXEDO UKI repair read-only preflight: PASS (%1, %2)</source>
        <translation>TUXEDO UKI ремонт только для чтения перед полетом: PASS (%1, %2)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1729"/>
        <source>PASS: vendor UKI command produced a changed TUX.EFI image.</source>
        <translation>Команда UKI произвела измененное изображение TUX.EFI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1730"/>
        <source>WARNING: vendor UKI command completed without changing TUX.EFI; the existing image will be validated against the selected target before this repair is reported successful.</source>
        <translation>ПРЕДУПРЕЖДЕНИЕ: команда поставщика UKI выполнена без изменения TUX.EFI; существующее изображение будет проверено на выбранную цель до того, как будет сообщено об успешном ремонте.</translation>
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
        <translation>Проверка завершена; целевые файлы не были изменены.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1734"/>
        <source>/etc/crypttab: %1</source>
        <translation>/etc/crypttab: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1735"/>
        <source>ESP mount candidate: %1</source>
        <translation>Кандидат на установку ESP: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1736"/>
        <source>/etc/fstab: %1</source>
        <translation>/etc/fstab: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1737"/>
        <source>GRUB config: %1</source>
        <translation>Конфигурация GRUB: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1738"/>
        <source>Supported modifying backend: %1</source>
        <translation>Поддерживаемый модифицирующий бэкэнд: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1739"/>
        <source>OS: %1</source>
        <translation>Производитель: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1740"/>
        <source>Root: %1</source>
        <translation>Источник: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1741"/>
        <source>Root filesystem: %1</source>
        <translation>Корневая файловая система: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1742"/>
        <source>Root fs: %1</source>
        <translation>Корень fs: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1743"/>
        <source>Repair root mount source: %1</source>
        <translation>Ремонт корневого монтажного источника: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1744"/>
        <source>Root subvolume: %1</source>
        <translation>Корневой объем: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1745"/>
        <source>Validation summary</source>
        <translation>Сводная валидация</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1746"/>
        <source>Vendor command completed successfully: %1</source>
        <translation>Командование поставщика выполнено успешно: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1747"/>
        <source>Repair-to-host capability scan: xattr inspection tooling is unavailable; the rsync security.capability filter was applied and trusted.</source>
        <translation>Сканирование возможностей ремонта до хоста: инструментарий проверки xattr недоступен; безопасность rsync. Функциональный фильтр применялся и пользовался доверием.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1748"/>
        <source>PASS: zpool scrub clean; clearing pool error counters with zpool clear.</source>
        <translation>PASS: zpool scrub clean; очистка бассейна счетчики ошибок с zpool clear.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1921"/>
        <location filename="../src/MainWindow.cpp" line="12565"/>
        <source>Repair tool %1 is unavailable.</source>
        <translation>Ремонтный инструмент %1 недоступен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1927"/>
        <location filename="../src/MainWindow.cpp" line="12570"/>
        <source>Repair tool %1 has unknown or unavailable diagnostic evidence.</source>
        <translation>Ремонтный инструмент %1 имеет неизвестные или недоступные диагностические доказательства.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="1934"/>
        <location filename="../src/MainWindow.cpp" line="12573"/>
        <source>No capability evidence for repair tool %1 in the selected scope. Run diagnostics first.</source>
        <translation>Нет доказательств возможности ремонта инструмента %1 в выбранной области. Сначала проведите диагностику.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2064"/>
        <location filename="../src/MainWindow.cpp" line="2069"/>
        <location filename="../src/MainWindow.cpp" line="2140"/>
        <source>Running-host default boot entry selection is unavailable.</source>
        <translation>Выбор загрузки Running-host по умолчанию недоступен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="2079"/>
        <location filename="../src/MainWindow.cpp" line="2098"/>
        <source>The helper did not identify a bootable running-host default entry.</source>
        <translation>Помощник не идентифицировал загрузочную запись по умолчанию.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="3335"/>
        <source>About %1</source>
        <translation>О компании %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="3885"/>
        <source>Linux detected — %1</source>
        <translation>Обнаружен Linux — %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="3894"/>
        <source>Unlocked Linux filesystem — inspect to confirm</source>
        <translation>Разблокированная файловая система Linux — проверка для подтверждения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="3897"/>
        <source>Likely Linux — inspect to confirm</source>
        <translation>Вероятнее всего Linux — проверьте</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="3900"/>
        <source>Likely Linux — encrypted</source>
        <translation>Likely Linux — зашифрованный</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="3903"/>
        <source>Encrypted — unlock to inspect</source>
        <translation>Зашифрованный — разблокировать для проверки</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="3906"/>
        <source>Linux-capable — inspect to confirm</source>
        <translation>Linux-совместимый — проверить, чтобы подтвердить</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="3908"/>
        <source>Available for inspection</source>
        <translation>Доступно для инспекции</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="3928"/>
        <source>Encrypted volume</source>
        <translation>Зашифрованный том</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="3930"/>
        <source>%1 volume</source>
        <translation>Объем %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="3932"/>
        <source>Partition</source>
        <translation>Разделение</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="3937"/>
        <source>Unnamed device</source>
        <translation>Неназванное устройство</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4385"/>
        <source>Preparing the Boot Bitch administrator session; authorization may be requested before the action starts.</source>
        <translation>Подготовка сеанса администратора Boot Bitch; авторизация может быть запрошена до начала действия.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4627"/>
        <location filename="../src/MainWindow.cpp" line="17061"/>
        <location filename="../src/MainWindow.cpp" line="19369"/>
        <source>Repair file system errors</source>
        <translation>Исправление ошибок файловой системы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4635"/>
        <source>The read-only check found file system errors in the %1. Select the devices to repair and the mode to use. Each repair runs separately after the helper repeats its scope, mount-state and tool preflights.</source>
        <translation>Проверка только для чтения обнаружила ошибки файловой системы в %1. Выберите устройства для ремонта и режим использования. Каждый ремонт выполняется отдельно после того, как помощник повторяет свой объем, состояние крепления и предполетные полёты.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4647"/>
        <location filename="../src/MainWindow.cpp" line="5773"/>
        <location filename="../src/MainWindow.cpp" line="5774"/>
        <source>Repair</source>
        <translation>ремонт</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4647"/>
        <location filename="../src/MainWindow.cpp" line="5407"/>
        <source>Device</source>
        <translation>Устройство</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4648"/>
        <source>File system</source>
        <translation>Файловая система</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4648"/>
        <source>Mode</source>
        <translation>Режим</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4649"/>
        <source>Issue summary</source>
        <translation>Резюме проблемы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4682"/>
        <source>Device: %1
File system: %2
Mount: %3
Check tool: %4</source>
        <translation>Устройство: %1
Файловая система: %2
Гора: %3
Инструмент проверки: %4</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4702"/>
        <source>The read-only check reported issues.</source>
        <translation>Проверка только для чтения сообщила о проблемах.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4717"/>
        <source>Offline repair modes refuse mounted filesystems; btrfs scrub and zpool scrub are online modes and require a mounted filesystem. btrfs check --repair asks for an extra backup warning before it runs.</source>
        <translation>Режимы автономного ремонта отказываются от установленных файловых систем; btrfs scrub и zpool scrub являются онлайн-режимами и требуют установленной файловой системы. btrfs check — ремонт требует дополнительного резервного предупреждения перед запуском.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4725"/>
        <source>Run Selected Repairs</source>
        <translation>Запуск выбранных ремонтов</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4919"/>
        <source>Linux recovery and boot-repair utility</source>
        <translation>Восстановление Linux и загрузочная утилита</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4921"/>
        <source>GUARDED REPAIR  •  %1</source>
        <translation>Обсуждение • %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4928"/>
        <source>Ordinary repairs require an explicitly selected non-host target. The protected running host has a separate deliberate maintenance mode with the same guarded repair stages and requires privilege authorization.</source>
        <translation>Обычный ремонт требует явно выбранной цели. Защищенный рабочий хост имеет отдельный преднамеренный режим обслуживания с теми же этапами охраняемого ремонта и требует разрешения на получение привилегий.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4953"/>
        <location filename="../src/MainWindow.cpp" line="4957"/>
        <source>A Boot Bitch operation is running in the background.</source>
        <translation>Операция Boot Bitch выполняется в фоновом режиме.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4994"/>
        <source>Show previous tab</source>
        <translation>Показать предыдущую вкладку</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="4999"/>
        <source>Show next tab</source>
        <translation>Показать следующую вкладку</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5018"/>
        <location filename="../src/MainWindow.cpp" line="5027"/>
        <location filename="../src/MainWindow.cpp" line="5222"/>
        <location filename="../src/MainWindow.cpp" line="5272"/>
        <location filename="../src/MainWindow.cpp" line="5273"/>
        <source>Systems</source>
        <translation>Системы</translation>
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
        <translation>диагностика</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5020"/>
        <location filename="../src/MainWindow.cpp" line="5027"/>
        <source>Repair</source>
        <comment>tab noun</comment>
        <translation>ремонт</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5021"/>
        <location filename="../src/MainWindow.cpp" line="5028"/>
        <location filename="../src/MainWindow.cpp" line="6053"/>
        <source>Snapshots</source>
        <translation>Снимки</translation>
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
        <translation>Копия файла</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5024"/>
        <location filename="../src/MainWindow.cpp" line="5028"/>
        <location filename="../src/MainWindow.cpp" line="5226"/>
        <location filename="../src/MainWindow.cpp" line="6538"/>
        <source>Logs</source>
        <translation>Лог</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5025"/>
        <location filename="../src/MainWindow.cpp" line="5028"/>
        <location filename="../src/MainWindow.cpp" line="5228"/>
        <source>Settings</source>
        <translation>Настройки</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5074"/>
        <source>Ready — guarded repair mode</source>
        <translation>Готовый — режим охраняемого ремонта</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5198"/>
        <source>&amp;File</source>
        <translation>&amp;Файл</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5199"/>
        <location filename="../src/MainWindow.cpp" line="5276"/>
        <source>Refresh Devices</source>
        <translation>Обновить устройства</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5205"/>
        <source>Lock Administrator Session</source>
        <translation>Закрытие сессии администратора</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5207"/>
        <source>Ends Boot Bitch&apos;s current privileged helper session, closes any LUKS mappings opened by Boot Bitch, and requires authorization again for the next root action. Pre-existing external mappings are left alone.</source>
        <translation>Завершает текущий сеанс поддержки Boot Bitch, закрывает все карты LUKS, открытые Boot Bitch, и снова требует авторизации для следующего действия. Существующие внешние карты остаются одни.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5213"/>
        <source>Administrator session locked — owned target resources released; the next privileged action will request authorization.</source>
        <translation>Сессия администратора заблокирована — выделены собственные целевые ресурсы; следующее привилегированное действие потребует авторизации.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5217"/>
        <source>Quit</source>
        <translation>Бросить</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5221"/>
        <source>&amp;View</source>
        <translation>&amp;Обзор</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5232"/>
        <source>Auto-size Device Columns</source>
        <translation>Колонки устройств автоматического размера</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5235"/>
        <source>Wrap Log Lines</source>
        <translation>Оберните лог-линии</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5240"/>
        <source>&amp;Help</source>
        <translation>&amp;помогает</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5241"/>
        <location filename="../src/MainWindow.cpp" line="16212"/>
        <source>Using Boot Bitch</source>
        <translation>Использование Boot Bitch</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5244"/>
        <location filename="../src/MainWindow.cpp" line="16235"/>
        <source>About Boot Bitch</source>
        <translation>Обсуждение Boot Bitch</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5274"/>
        <source>Select a physical drive; Boot Bitch resolves the most likely Linux system volume automatically. The running host stays protected from ordinary target repairs, with a separate explicit host-maintenance path for its own system.</source>
        <translation>Выберите физический диск; Boot Bitch автоматически решает наиболее вероятный объем системы Linux. Работающий хост остается защищенным от обычного ремонта цели, с отдельным явным способом обслуживания хоста для своей собственной системы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5313"/>
        <source>Detecting running system…</source>
        <translation>Обнаружение запущенной системы …</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5324"/>
        <source>Detecting protected storage…</source>
        <translation>Защищенное хранилище …</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5344"/>
        <source>PROTECTED</source>
        <translation>защищенный</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5350"/>
        <source>The running host remains protected from ordinary repair-target operations.</source>
        <translation>Работающий хост остается защищенным от обычных операций по ремонту.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5353"/>
        <source>Details</source>
        <translation>Подробности</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5355"/>
        <source>Show read-only details for the protected running host.</source>
        <translation>Покажите только для чтения детали для защищенного хоста.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5359"/>
        <location filename="../src/MainWindow.cpp" line="7441"/>
        <source>Host Maintenance</source>
        <translation>Обслуживание хоста</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5361"/>
        <location filename="../src/MainWindow.cpp" line="7442"/>
        <source>Host Maintenance — select the running host for deliberate guarded maintenance. All supported repair stages run against the active system; running-host Snapper @ snapshots are available in the Snapshots tab, while the chroot shell and file-copy workflows remain separate target tools.</source>
        <translation>Обслуживание хоста — выберите бегущий хост для преднамеренного охраняемого обслуживания. Все поддерживаемые этапы восстановления работают против активной системы; на вкладке Snapshots доступны скриншоты Snapper @, в то время как рабочие процессы оболочки chroot и копирования файлов остаются отдельными целевыми инструментами.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5366"/>
        <location filename="../src/MainWindow.cpp" line="7565"/>
        <source>Make Default</source>
        <translation>Сделать Default</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5368"/>
        <source>Restore/ensure the running host&apos;s verified default boot entry and select it as the default while preserving every other boot entry.</source>
        <translation>Восстановите/обеспечьте верифицированную загрузочную запись запущенного хоста по умолчанию и выберите ее по умолчанию, сохраняя при этом каждую другую загрузочную запись.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5383"/>
        <source>Available repair targets</source>
        <translation>Доступные ремонтные цели</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5384"/>
        <source>Repair targets</source>
        <translation>Ремонтные цели</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5385"/>
        <source>Drives are ranked by visible Linux, EFI, filesystem and encryption evidence. Select the top-level drive; partitions and mapped volumes are informational.</source>
        <translation>Диски ранжируются по видимым Linux, EFI, файловой системе и доказательствам шифрования. Выберите диск верхнего уровня; разделы и отображенные тома являются информационными.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5387"/>
        <source>Most likely first</source>
        <translation>Скорее всего, первым</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5388"/>
        <source>Candidates are ranked by visible Linux, EFI, filesystem and encryption evidence. Click a column header to sort by that column; click it again to reverse the order.</source>
        <translation>Кандидаты ранжируются по видимым доказательствам Linux, EFI, файловой системы и шифрования. Нажмите на заголовок столбца, чтобы сортировать по этому столбцу; нажмите его снова, чтобы изменить порядок.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5406"/>
        <source>Model / Label</source>
        <translation>Модель / этикетка</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5406"/>
        <location filename="../src/MainWindow.cpp" line="6092"/>
        <location filename="../src/MainWindow.cpp" line="6806"/>
        <source>Status</source>
        <translation>статус</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5406"/>
        <source>Connection</source>
        <translation>Подключение</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5407"/>
        <source>Size</source>
        <translation>Размер</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5407"/>
        <source>Filesystem</source>
        <translation>Файловая система</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5451"/>
        <source>Select Target</source>
        <translation>Выберите цель</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5455"/>
        <location filename="../src/MainWindow.cpp" line="7383"/>
        <location filename="../src/MainWindow.cpp" line="7729"/>
        <location filename="../src/MainWindow.cpp" line="9261"/>
        <location filename="../src/MainWindow.cpp" line="9340"/>
        <source>Unlock</source>
        <translation>Разблокировать</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5457"/>
        <location filename="../src/MainWindow.cpp" line="7385"/>
        <source>Select a drive whose detected target is a locked LUKS volume.</source>
        <translation>Выберите диск, целью которого является заблокированный объем LUKS.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5469"/>
        <location filename="../src/MainWindow.cpp" line="8374"/>
        <source>Administrator authorization was deferred for the current scope. Diagnostics and repairs stay available; the next privileged action will request authorization again, or press Authorize to establish the session now.</source>
        <translation>Разрешение администратора было отложено на текущий период. Диагностика и ремонт остаются в наличии; следующее привилегированное действие будет запрашивать разрешение снова или нажать Разрешить установить сеанс сейчас.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5473"/>
        <source>Authorize</source>
        <translation>разрешать</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5475"/>
        <source>Establish the privileged Boot Bitch helper session for the current scope now instead of waiting for the next privileged action.</source>
        <translation>Установите привилегированную сессию помощника Boot Bitch для текущего объема теперь вместо того, чтобы ждать следующего привилегированного действия.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5481"/>
        <location filename="../src/MainWindow.cpp" line="9501"/>
        <source>Committed target: none</source>
        <translation>Обязательная цель: нет</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5483"/>
        <source>No repair target has been committed yet. Row selection is inspection only until Select Target is pressed.</source>
        <translation>Цели ремонта пока не установлены. Выбор строки - это проверка только до тех пор, пока не будет нажата выбранная цель.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5487"/>
        <source>Unlock status</source>
        <translation>Статус разблокировки</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5501"/>
        <location filename="../src/MainWindow.cpp" line="7388"/>
        <source>Select a drive to see unlock status.</source>
        <translation>Выберите диск, чтобы увидеть статус разблокировки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5502"/>
        <location filename="../src/MainWindow.cpp" line="7724"/>
        <location filename="../src/MainWindow.cpp" line="8408"/>
        <source>No unlock operation recorded for this drive in the current session.</source>
        <translation>Операция разблокировки не записывалась для этого диска в текущей сессии.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5506"/>
        <source>Selected drive details</source>
        <translation>Выбранные детали диска</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5546"/>
        <source>Drive:</source>
        <translation>Водитель:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5547"/>
        <source>Detected target:</source>
        <translation>Обнаруженная цель:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5548"/>
        <source>Model / label:</source>
        <translation>Модель / этикетка:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5549"/>
        <source>Status:</source>
        <translation>Статус:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5550"/>
        <source>Size:</source>
        <translation>Размер:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5551"/>
        <source>Connection:</source>
        <translation>Подключение:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5552"/>
        <source>Filesystem:</source>
        <translation>Файловая система:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5553"/>
        <source>UUID:</source>
        <translation>UUID:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5554"/>
        <source>Mounts:</source>
        <translation>Горы:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5555"/>
        <source>Protection:</source>
        <translation>Защита:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5556"/>
        <source>Best system component visible without privileged probing. Read-only inspection will later resolve closed encryption and Btrfs root subvolumes automatically.</source>
        <translation>Лучший системный компонент, видимый без привилегированного зондирования. Проверка только для чтения будет позже автоматически разрешать закрытое шифрование и подобъемы корня Btrfs.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5597"/>
        <source>Diagnostics follow the Systems page: the selected repair drive while Host Maintenance is off, or the protected running host while Host Maintenance is active. Running Host diagnostics are available only inside the explicit Host Maintenance scope: enter Host Maintenance on the protected running-host card in Systems first. Both scopes provide read-only diagnostics; host EFI/UKI checks inspect the active ESP and firmware entries, while repair-drive checks mount only that system read-only.</source>
        <translation>Диагностика следует за страницей «Системы»: выбранный привод для ремонта во время обслуживания хоста отключен или защищенный работающий хост во время активного обслуживания хоста. Диагностика Running Host доступна только в явной области обслуживания хоста: сначала введите обслуживание хоста на защищенной карте хоста в системах. Обе области обеспечивают диагностику только для чтения; хост EFI / UKI проверяет активные записи ESP и прошивки, в то время как проверки на ремонт-драйв устанавливают только эту систему только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5606"/>
        <location filename="../src/MainWindow.cpp" line="5777"/>
        <location filename="../src/MainWindow.cpp" line="6056"/>
        <location filename="../src/MainWindow.cpp" line="6227"/>
        <location filename="../src/MainWindow.cpp" line="6304"/>
        <location filename="../src/MainWindow.cpp" line="9447"/>
        <source>Target: none selected</source>
        <translation>Цель: не выбрано</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5611"/>
        <location filename="../src/MainWindow.cpp" line="13419"/>
        <location filename="../src/MainWindow.cpp" line="13935"/>
        <source>Run All</source>
        <translation>Беги все</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5612"/>
        <source>Run All — run every available read-only diagnostic for the current scope.</source>
        <translation>Запустите все - запустите все доступные диагностические тесты только для чтения для текущего объема.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5628"/>
        <source>Select a target configuration file to inspect or edit through the guarded helper.</source>
        <translation>Выберите файл конфигурации цели, чтобы проверить или отредактировать через охраняемого помощника.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5630"/>
        <source>Edit Target File…</source>
        <translation>Обновление Target File…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5632"/>
        <source>Read or edit the selected target configuration file. Changes invalidate cached diagnostics.</source>
        <translation>Прочитайте или отредактируйте выбранный файл конфигурации цели. Изменения аннулируют кэшированную диагностику.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5638"/>
        <source>Target configuration:</source>
        <translation>Конфигурация цели:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5649"/>
        <source>Diagnostic checks</source>
        <translation>Диагностические проверки</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5668"/>
        <source>Selected diagnostic</source>
        <translation>Выбранная диагностика</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5673"/>
        <location filename="../src/MainWindow.cpp" line="11608"/>
        <source>Select a diagnostic</source>
        <translation>Выберите диагностику</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5676"/>
        <location filename="../src/MainWindow.cpp" line="11609"/>
        <source>Choose a diagnostic from the list.</source>
        <translation>Выберите диагностику из списка.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5680"/>
        <location filename="../src/MainWindow.cpp" line="11025"/>
        <location filename="../src/MainWindow.cpp" line="18302"/>
        <source>Ready</source>
        <translation>Готовы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5693"/>
        <source>Results</source>
        <translation>Результаты</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5696"/>
        <location filename="../src/MainWindow.cpp" line="11611"/>
        <location filename="../src/MainWindow.cpp" line="11678"/>
        <location filename="../src/MainWindow.cpp" line="11687"/>
        <location filename="../src/MainWindow.cpp" line="11715"/>
        <source>Run Diagnostic</source>
        <translation>Бег Диагностика</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5697"/>
        <source>Run Diagnostic — run the selected read-only diagnostic for the current scope; cached results offer a re-run.</source>
        <translation>Run Diagnostic — запустите выбранную диагностику только для чтения для текущего объема; кэшированные результаты предлагают повторный запуск.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5704"/>
        <source>Diagnostic results appear here.</source>
        <translation>Результаты диагностики появляются здесь.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5716"/>
        <source>Copy Results</source>
        <translation>Копировать результаты</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5717"/>
        <source>Save Results…</source>
        <translation>Сохранить результаты …</translation>
    </message>
    <message>
        <source>Run one repair tool or use the Full Repair plan against the selected repair drive. Choose Host Maintenance on Systems to run the same supported, guarded stages against the protected Running Host.</source>
        <translation type="vanished">Запустите один ремонтный инструмент или используйте план полного ремонта против выбранного ремонтного привода. Выберите техническое обслуживание хоста в системах для запуска тех же поддерживаемых, охраняемых этапов против защищенного запускающего хоста.</translation>
    </message>
    <message>
        <source>Choose Full Repair stages in Settings. Enabled stages run in the order shown. Each configurable stage also appears below as an individual tool; the Full Repair column mirrors its current Settings state. The boot tools (EFI / UKI bootloader, GRUB or extlinux configuration, boot-stack reconciliation and Make Default) are independent: run them in any order, and a later action re-verifies what an earlier one changed and reports its own result. The active scope is shown beside Repair: selected repair drive or Running Host maintenance.</source>
        <translation type="vanished">Выберите этапы полного ремонта в настройках. Включенные этапы выполняются в показанном порядке. Каждый настраиваемый этап также отображается ниже как отдельный инструмент; колонка «Полный ремонт» отражает его текущее состояние «Настройки». Загрузочные инструменты (EFI / UKI, GRUB или extlinux конфигурация, сверка загрузочного стека и по умолчанию) независимы: запустите их в любом порядке, а более поздние действия перепроверяют то, что изменилось ранее, и сообщают о своем собственном результате. Активный диапазон показан рядом с ремонтом: выбранный ремонтный привод или техническое обслуживание Running Host.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5775"/>
        <source>Run one repair tool or use the Full Repair plan against the selected repair drive. Choose Host Maintenance on Systems to run the same supported, guarded stages against the protected Running Host. Choose Full Repair stages in Settings. Enabled stages run in the order shown. Each configurable stage also appears below as an individual tool; the Full Repair column mirrors its current Settings state. The boot tools (EFI / UKI bootloader, GRUB or extlinux configuration, boot-stack reconciliation and Make Default) are independent: run them in any order, and a later action re-verifies what an earlier one changed and reports its own result. The active scope is shown beside Repair: selected repair drive or Running Host maintenance.</source>
        <translation>Запустите один ремонтный инструмент или используйте план полного ремонта против выбранного ремонтного привода. Выберите техническое обслуживание хоста в системах для запуска тех же поддерживаемых, охраняемых этапов против защищенного запускающего хоста. Выберите этапы полного ремонта в настройках. Включенные этапы выполняются в показанном порядке. Каждый настраиваемый этап также отображается ниже как отдельный инструмент; колонка «Полный ремонт» отражает его текущее состояние «Настройки». Инструменты загрузки (EFI / UKI загрузчик, GRUB или extlinux конфигурация, сверка загрузочного стека и по умолчанию) независимы: запустите их в любом порядке, а более позднее действие перепроверяет, что изменилось ранее, и сообщает о своем собственном результате. Активный диапазон показан рядом с ремонтом: выбранный ремонтный привод или техническое обслуживание Running Host.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5781"/>
        <location filename="../src/MainWindow.cpp" line="6727"/>
        <source>Full Repair plan</source>
        <translation>Полный план ремонта</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5793"/>
        <location filename="../src/MainWindow.cpp" line="17138"/>
        <source>No stages selected</source>
        <translation>Не выбраны этапы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5799"/>
        <source>Run All diagnostics for the selected target or running host before starting Full Repair. The report is read-only evidence used to choose and confirm repair stages.</source>
        <translation>Запустите всю диагностику выбранной цели или запуска хоста перед началом полного ремонта. Отчет представляет собой только показания, используемые для выбора и подтверждения этапов ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5812"/>
        <source>Configure Plan…</source>
        <translation>Настройка Plan…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5813"/>
        <source>Open Settings to choose which Full Repair stages are part of the plan.</source>
        <translation>Откройте настройки, чтобы выбрать, какие этапы полного ремонта являются частью плана.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5824"/>
        <location filename="../src/MainWindow.cpp" line="18883"/>
        <source>Run Full Repair</source>
        <translation>Запуск полного ремонта</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5827"/>
        <location filename="../src/MainWindow.cpp" line="5973"/>
        <source>Select a repair drive, or choose Host Maintenance on the protected running-host card.</source>
        <translation>Выберите накопитель для ремонта или выберите обслуживание хоста на защищенной карте бегущего хоста.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5872"/>
        <source>Individual repair tools</source>
        <translation>Индивидуальные ремонтные инструменты</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5878"/>
        <source>Tool</source>
        <translation>Инструмент</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5878"/>
        <location filename="../src/MainWindow.cpp" line="18845"/>
        <location filename="../src/MainWindow.cpp" line="18930"/>
        <location filename="../src/MainWindow.cpp" line="18944"/>
        <source>Full Repair</source>
        <translation>Полный ремонт</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5914"/>
        <location filename="../src/MainWindow.cpp" line="17305"/>
        <source>Validate environment</source>
        <translation>Проверить окружающую среду</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5915"/>
        <location filename="../src/MainWindow.cpp" line="17438"/>
        <location filename="../src/MainWindow.cpp" line="19310"/>
        <location filename="../src/MainWindow.cpp" line="19341"/>
        <source>File system repair</source>
        <translation>Ремонт файловой системы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5916"/>
        <location filename="../src/MainWindow.cpp" line="17313"/>
        <location filename="../src/MainWindow.cpp" line="18743"/>
        <source>Complete package configuration</source>
        <translation>Полная комплектация пакета</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5917"/>
        <location filename="../src/MainWindow.cpp" line="17320"/>
        <location filename="../src/MainWindow.cpp" line="18747"/>
        <source>Repair broken dependencies</source>
        <translation>Ремонт сломанных зависимостей</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5918"/>
        <location filename="../src/MainWindow.cpp" line="6736"/>
        <location filename="../src/MainWindow.cpp" line="17064"/>
        <location filename="../src/MainWindow.cpp" line="17331"/>
        <location filename="../src/MainWindow.cpp" line="18150"/>
        <location filename="../src/MainWindow.cpp" line="18751"/>
        <source>Refresh package metadata</source>
        <translation>Обновить метаданные пакета</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5919"/>
        <location filename="../src/MainWindow.cpp" line="17065"/>
        <location filename="../src/MainWindow.cpp" line="17342"/>
        <location filename="../src/MainWindow.cpp" line="18755"/>
        <source>Upgrade installed packages</source>
        <translation>Обновление установленных пакетов</translation>
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
        <translation>Инитрамфы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5923"/>
        <location filename="../src/MainWindow.cpp" line="17391"/>
        <source>EFI / UKI bootloader</source>
        <translation>EFI/UKI загрузчик</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5925"/>
        <location filename="../src/MainWindow.cpp" line="17431"/>
        <source>extlinux configuration</source>
        <translation>Конфигурация extlinux</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5926"/>
        <location filename="../src/MainWindow.cpp" line="17453"/>
        <source>Boot stack reconciliation</source>
        <translation>Обсуждение Boot Stack</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5935"/>
        <source>Mirrors the corresponding Settings → Full Repair plan checkbox. Individual tools remain runnable independently.</source>
        <translation>Зеркала соответствующих настроек → Full Repair Plan Checkbox. Индивидуальные инструменты остаются управляемыми независимо.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5953"/>
        <source>Selected tool</source>
        <translation>Выбранный инструмент</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5968"/>
        <location filename="../src/MainWindow.cpp" line="17282"/>
        <source>Select a repair tool</source>
        <translation>Выберите инструмент для ремонта</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5971"/>
        <location filename="../src/MainWindow.cpp" line="17285"/>
        <source>Run Tool</source>
        <translation>Скачать Run Tool</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="5977"/>
        <source>Select a tool to review its repair action.</source>
        <translation>Выберите инструмент для проверки его действия по ремонту.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6052"/>
        <source>Btrfs snapshots</source>
        <translation>Снимки Btrfs</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6054"/>
        <source>Inspect Btrfs root snapshots and perform a transactional rollback. Rollback keeps the source snapshot unchanged, preserves the current @ root, promotes a writable copy to @, rebuilds the boot stack and automatically restores the old root if post-switch validation fails. In Host Maintenance the same workflow targets the running host through Snapper @ snapshots and Boot Bitch @rollback-before-* undo points; the running host starts the promoted root only after a reboot.</source>
        <translation>Осмотрите снимки корня Btrfs и выполните транзакционный откат. Rollback сохраняет исходный снимок без изменений, сохраняет текущий @ root, продвигает написанную копию на @, восстанавливает стек загрузки и автоматически восстанавливает старый корень, если валидация после переключения не удается. В техническом обслуживании хоста тот же рабочий процесс нацелен на бегущего хоста через Snapper @ snapshots и Boot Bitch @rollback-before-* undo points; бегущий хост запускает продвигаемый корень только после перезагрузки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6075"/>
        <location filename="../src/MainWindow.cpp" line="10361"/>
        <location filename="../src/MainWindow.cpp" line="10510"/>
        <source>Reboot Now</source>
        <translation>Перезагрузить сейчас</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6076"/>
        <source>Reboots the running host after a separate confirmation; all users are signed out and unsaved work is lost.</source>
        <translation>Перезагружает запущенный хост после отдельного подтверждения; все пользователи подписываются и несохраненная работа теряется.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6077"/>
        <location filename="../src/MainWindow.cpp" line="10362"/>
        <source>Later</source>
        <translation>Позже</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6078"/>
        <source>Hides the reboot reminder until Host Maintenance is re-entered; the staged rollback stays in effect.</source>
        <translation>Скрывает напоминание о перезагрузке до тех пор, пока не будет повторно введено обслуживание хоста; постановочный откат остается в силе.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6083"/>
        <source>Reboot reminder hidden; the running-host rollback stays staged until the host is rebooted.</source>
        <translation>Напоминание о перезагрузке скрыто; откат бегущего хозяина остается инсценированным, пока хозяин не перезагрузится.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6092"/>
        <source>Snapshot</source>
        <translation>Снимок</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6092"/>
        <source>Created</source>
        <translation>созданный</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6092"/>
        <source>Type</source>
        <translation>Тип</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6092"/>
        <source>Description</source>
        <translation>Описание</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6139"/>
        <source>Load snapshots to inspect recovery points. Snapshot discovery runs through the guarded read-only helper, so root-owned Snapper metadata does not need to be readable by the desktop user.</source>
        <translation>Снимки нагрузки для проверки точек восстановления. Открытие Snapshot проходит через защищенный помощник только для чтения, поэтому принадлежащие корневым пользователям метаданные Snapper не должны быть читаемыми пользователем рабочего стола.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6151"/>
        <source>Snapshot inventory</source>
        <translation>инвентарь снимков</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6163"/>
        <source>Load Snapshots</source>
        <translation>Загрузить Snapshots</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6164"/>
        <source>Inspect Selected</source>
        <translation>Проверка выбранная</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6165"/>
        <source>Roll Back to Selected</source>
        <translation>Обратно в избранное</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6170"/>
        <source>Load Btrfs root snapshots through the privileged helper using read-only mounts.</source>
        <translation>Загрузите снимки корня Btrfs через привилегированного помощника, используя только для чтения крепления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6171"/>
        <source>Inspect the selected snapshot read-only, including OS metadata, fstab/crypttab and visible kernel files.</source>
        <translation>Осмотрите выбранный снимок только для чтения, включая метаданные ОС, fstab /crypttab и видимые файлы ядра.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6172"/>
        <source>Run a read-only rollback preflight, then promote a writable copy of the selected snapshot to @ with automatic root restoration if boot-stack reconciliation fails.</source>
        <translation>Запустите предварительный полет только для чтения, а затем продвигайте написанную копию выбранного снимка на @ с автоматическим восстановлением корня, если сверка загрузочного стека не удается.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6180"/>
        <source>Select a snapshot to enable the actions above, or double-click a row to inspect it read-only.</source>
        <translation>Выберите снимок, чтобы включить действия выше, или дважды щелкните строку, чтобы проверить его только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6217"/>
        <location filename="../src/MainWindow.cpp" line="11110"/>
        <location filename="../src/MainWindow.cpp" line="11248"/>
        <source>Chroot shell</source>
        <translation>Корневая раковина</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6224"/>
        <location filename="../src/MainWindow.cpp" line="11119"/>
        <source>Run a command inside the selected repair system as root (sudo is not needed). Commands are executed one at a time in a fresh chroot. When a command asks a question, Boot Bitch shows it in a popup and sends your answer back to the command; cancelling stops the command. Non-interactive flags such as dnf update -y or apt-get -y upgrade remain recommended for unattended runs. Output is kept in this window and in the application log.</source>
        <translation>Запустите команду внутри выбранной системы ремонта как root (судо не требуется). Команды выполняются по одной за раз в свежем корне. Когда команда задает вопрос, Boot Bitch показывает его всплывающим окном и отправляет ответ обратно в команду; отмена останавливает команду. Неинтерактивные флаги, такие как обновление dnf -y или обновление apt-get -y, по-прежнему рекомендуются для необслуживаемых запусков. Выход хранится в этом окне и в журнале приложений.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6233"/>
        <location filename="../src/MainWindow.cpp" line="11126"/>
        <source>Command, for example: dnf update -y or update-grub</source>
        <translation>Командная строка: dnf Update -y или update-grub</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6237"/>
        <location filename="../src/MainWindow.cpp" line="11132"/>
        <source>Run Command</source>
        <translation>Запускайте команду</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6239"/>
        <location filename="../src/MainWindow.cpp" line="11099"/>
        <source>Execute the command inside the selected repair system as root.</source>
        <translation>Выполните команду внутри выбранной системы ремонта как root.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6255"/>
        <source>Command output will appear here.</source>
        <translation>Командный выход появится здесь.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6262"/>
        <location filename="../src/MainWindow.cpp" line="11137"/>
        <source>Commands can modify the target system. Review each command before running it.</source>
        <translation>Команды могут изменять целевую систему. Проверяйте каждую команду перед запуском.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6265"/>
        <source>Clear Output</source>
        <translation>Чистый результат</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6298"/>
        <source>File copy</source>
        <translation>Копия файла</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6301"/>
        <source>Copy and verify files in either direction. Host to Repair remounts only the selected target filesystem read-write after safety checks; Repair to Host keeps the repair target read-only. Browse Target Folders reads the selected repair tree through temporary read-only mounts, while transfers use rsync without --delete, path containment, ownership validation and post-copy verification.</source>
        <translation>Копировать и проверять файлы в любом направлении. Host to Repair восстанавливает только выбранную целевую файловую систему read-write после проверки безопасности; Ремонт хоста сохраняет цель ремонта только для чтения. Browse Target Folders считывает выбранное дерево ремонта через временные крепления только для чтения, в то время как передачи используют rsync без удаления, сдерживания пути, проверки собственности и проверки после копирования.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6313"/>
        <source>Preview Changes</source>
        <translation>Превью Изменения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6314"/>
        <location filename="../src/MainWindow.cpp" line="11447"/>
        <source>Copy and Verify</source>
        <translation>Копировать и проверять</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6317"/>
        <source>Run an rsync dry-run through the guarded helper. No files are changed.</source>
        <translation>Пробежать рысью через охраняемого помощника. Файлы не меняются.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6318"/>
        <source>Copy staged items and verify the result. Existing destination names are overwritten when source content differs; unrelated destination files are never deleted.</source>
        <translation>Копируйте постановочные элементы и проверяйте результат. Существующие имена пунктов назначения перезаписываются, когда исходный контент отличается; несвязанные файлы назначения никогда не удаляются.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6331"/>
        <source>Direction:</source>
        <translation>Направление:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6333"/>
        <location filename="../src/MainWindow.cpp" line="11401"/>
        <source>Host → Repair</source>
        <translation>Ремонт →</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6334"/>
        <location filename="../src/MainWindow.cpp" line="11401"/>
        <source>Repair → Host</source>
        <translation>Ремонт → Host</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6335"/>
        <source>Choose which system supplies the source files and which system receives them.</source>
        <translation>Выберите, какая система поставляет исходные файлы и какая система их получает.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6360"/>
        <location filename="../src/MainWindow.cpp" line="10574"/>
        <source>Add Files…</source>
        <translation>Добавление файлов …</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6361"/>
        <location filename="../src/MainWindow.cpp" line="10575"/>
        <source>Add Folder…</source>
        <translation>Добавить Folder…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6362"/>
        <source>Remove</source>
        <translation>Удалить</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6363"/>
        <source>Remove the selected staged source entries from this list.</source>
        <translation>Удалите выбранные элементы из этого списка.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6364"/>
        <location filename="../src/MainWindow.cpp" line="10672"/>
        <source>Clear</source>
        <translation>Чисто.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6365"/>
        <source>Clear every staged source entry from this list.</source>
        <translation>Очистите каждую постановочную запись источника из этого списка.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6388"/>
        <source>Choose Path…</source>
        <translation>Выберите Path…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6394"/>
        <source>3. Ownership and copy policy</source>
        <translation>3. Политика владения и копирования</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6397"/>
        <source>Ownership:</source>
        <translation>Право собственности:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6399"/>
        <source>Smart destination ownership (recommended)</source>
        <translation>Умное владение пунктом назначения (рекомендуется)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6400"/>
        <source>Preserve source numeric UID/GID</source>
        <translation>Сохранить исходный номер UID/GID</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6402"/>
        <source>Smart mode validates UID/GID identity mapping across the two systems and falls back to the destination-directory owner when the same numeric ID means a different account.</source>
        <translation>Смарт-режим проверяет отображение идентификационных данных UID / GID в обеих системах и возвращается к владельцу каталога назначения, когда один и тот же цифровой идентификатор означает другую учетную запись.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6537"/>
        <source>Application log</source>
        <translation>Журнал приложений</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6539"/>
        <source>Shows this application&apos;s session activity. Session files are listed on the left; Save a copy when you need to share troubleshooting details.</source>
        <translation>Показывает сессионную активность этого приложения. Файлы сеанса перечислены слева; Сохраните копию, когда вам нужно поделиться деталями устранения неполадок.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6541"/>
        <source>Save As…</source>
        <translation>Сохранить As…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6542"/>
        <source>Clear Register</source>
        <translation>Чистый регистр</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6576"/>
        <source>Session logs</source>
        <translation>Журналы сеансов</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6590"/>
        <source>New Session Log</source>
        <translation>Новый Session Log</translation>
    </message>
    <message>
        <source>Truncate the current session log and start it over with a cleared-by-user entry. Prior session files are never modified.</source>
        <translation type="vanished">Сократите текущий журнал сеанса и начните его с проясненной пользовательской записи. Файлы предыдущих сеансов никогда не изменяются.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6591"/>
        <source>Delete</source>
        <translation>Исключить</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6593"/>
        <location filename="../src/MainWindow.cpp" line="10737"/>
        <source>Refresh</source>
        <translation>освежить</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6594"/>
        <location filename="../src/MainWindow.cpp" line="15033"/>
        <source>Add Note</source>
        <translation>Добавить комментарий</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6595"/>
        <source>Append a timestamped NOTE entry to the current session log.</source>
        <translation>Добавить в журнал текущей сессии запись ПРИМЕЧАНИЯ с меткой времени.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6610"/>
        <source>Search log:</source>
        <translation>Журнал поиска:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6616"/>
        <source>Filter application log (fuzzy match; use @section, e.g. @errors, for whole diagnostic sections)…</source>
        <translation>Журнал приложений фильтров (нечеткое совпадение; используйте @section, например @errors, для целых диагностических разделов) …</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6618"/>
        <source>Type any characters to show matching application-log entries. Matching is case-insensitive and fuzzy. A term beginning with @ selects a complete diagnostic section by key or title (for example @errors, @boot or @all) together with the repair entries related to that section; other terms keep the fuzzy line match and combine as AND.</source>
        <translation>Введите любые символы, чтобы показать соответствующие записи журнала приложений. Совпадение нечувствительно и нечетко. Термин, начинающийся с @, выбирает полный диагностический раздел по ключу или заголовку (например, @errors, @boot или @all) вместе с ремонтными записями, относящимися к этому разделу; другие термины поддерживают соответствие нечеткой линии и объединяются как AND.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6627"/>
        <source>All entries</source>
        <translation>Все записи</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6629"/>
        <source>Repairs</source>
        <translation>ремонт</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6638"/>
        <source>Filter the visible log by entry kind. Selecting a diagnostic section shows that complete section plus the repair entries recorded for its related repair tools; a workflow filter such as File system repair additionally shows the whole storage evidence sections it works on.</source>
        <translation>Фильтруйте видимый журнал по типу записи. Выбор диагностического раздела показывает, что полный раздел плюс записи о ремонте, записанные для связанных с ним инструментов ремонта; фильтр рабочего процесса, такой как ремонт файловой системы, дополнительно показывает все разделы доказательств хранения, на которых он работает.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6717"/>
        <source>Device discovery</source>
        <translation>Открытие устройства</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6719"/>
        <source>Show devices without an identified Linux installation</source>
        <translation>Показать устройства без идентифицированной установки Linux</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6720"/>
        <source>Show removable and USB storage</source>
        <translation>Показать съемное и USB-хранилище</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6721"/>
        <source>Show encrypted devices before unlocking</source>
        <translation>Показать зашифрованные устройства перед разблокировкой</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6729"/>
        <source>Repair file system errors (read-only check first)</source>
        <translation>Исправление ошибок файловой системы (проверка только для чтения)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6730"/>
        <source>Runs a read-only file system check for the root, /boot, ESP and /home filesystems, then offers an explicit per-device repair for each filesystem that reports errors. Offline repair tools refuse mounted filesystems; btrfs scrub and zpool scrub are online modes.</source>
        <translation>Запускает проверку файловой системы только для чтения для корневой, /boot, ESP и /home файловых систем, а затем предлагает явный ремонт каждого устройства для каждой файловой системы, которая сообщает об ошибках. Инструменты автономного ремонта отказываются от установленных файловых систем; btrfs scrub и zpool scrub - это онлайн-режимы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6734"/>
        <location filename="../src/MainWindow.cpp" line="17062"/>
        <location filename="../src/MainWindow.cpp" line="18863"/>
        <source>Complete interrupted package configuration</source>
        <translation>Полная прерванная конфигурация пакета</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6735"/>
        <location filename="../src/MainWindow.cpp" line="17063"/>
        <location filename="../src/MainWindow.cpp" line="18147"/>
        <source>Repair broken package dependencies</source>
        <translation>Ремонт сломанных зависимостей пакета</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6737"/>
        <location filename="../src/MainWindow.cpp" line="18153"/>
        <source>Upgrade installed packages (adaptive APT simulation)</source>
        <translation>Обновление установленных пакетов (адаптивное моделирование APT)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6738"/>
        <location filename="../src/MainWindow.cpp" line="17066"/>
        <location filename="../src/MainWindow.cpp" line="18867"/>
        <source>Rebuild DKMS modules</source>
        <translation>Восстановление модулей DKMS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6739"/>
        <location filename="../src/MainWindow.cpp" line="18156"/>
        <source>Restore detected graphical login manager and graphical.target</source>
        <translation>Восстановить обнаруженный графический менеджер входа и графический. цель</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6740"/>
        <location filename="../src/MainWindow.cpp" line="17068"/>
        <location filename="../src/MainWindow.cpp" line="18159"/>
        <source>Rebuild initramfs after mapper/crypttab validation</source>
        <translation>Восстановление initramfs после валидации mapper/crypttab</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6741"/>
        <location filename="../src/MainWindow.cpp" line="18141"/>
        <source>Repair EFI / UKI boot path (explicit target ESP repair)</source>
        <translation>Ремонт загрузочного пути EFI / UKI (явный целевой ремонт ESP)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6742"/>
        <location filename="../src/MainWindow.cpp" line="17070"/>
        <location filename="../src/MainWindow.cpp" line="18144"/>
        <source>Update GRUB configuration</source>
        <translation>Обновление конфигурации GRUB</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6743"/>
        <location filename="../src/MainWindow.cpp" line="17071"/>
        <location filename="../src/MainWindow.cpp" line="18162"/>
        <source>Update extlinux configuration</source>
        <translation>Обновление конфигурации extlinux</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6759"/>
        <source>Automatically regenerate read-only diagnostics after repairs or target changes</source>
        <translation>Автоматически регенерировать диагностику только для чтения после ремонта или изменения цели</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6760"/>
        <source>Regenerates the cached read-only diagnostics for the current Diagnostics scope after a repair, target change, or other evidence invalidation. The refresh runs only inside an already authorized administrator session; it never triggers a new Polkit prompt.</source>
        <translation>Восстанавливает кэшированную диагностику только для чтения для текущего объема диагностики после ремонта, изменения цели или другой недействительности доказательств. Обновление выполняется только внутри уже авторизованной сессии администратора; оно никогда не запускает новую подсказку Polkit.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6766"/>
        <source>Mandatory safety controls</source>
        <translation>Обязательный контроль безопасности</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6769"/>
        <source>Protect every physical device backing /, /boot and /boot/efi</source>
        <translation>Защита каждого физического устройства /, /boot и /boot/efi</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6770"/>
        <source>Require explicit confirmation before package installation or repair actions</source>
        <translation>Требует явного подтверждения перед установкой упаковки или ремонтом.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6771"/>
        <source>Require mapper/crypttab consistency before initramfs rebuild</source>
        <translation>Требуется согласованность с картографом/криптабом перед восстановлением initramfs</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6772"/>
        <source>Preserve pre-rollback Btrfs root and auto-restore it on validation failure</source>
        <translation>Сохранить корень Btrfs и автоматически восстановить его при сбое проверки</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6773"/>
        <source>Never log LUKS passphrases or authentication secrets</source>
        <translation>Никогда не записывайте парольные фразы LUKS или секреты аутентификации.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6783"/>
        <source>Host capabilities and dependencies</source>
        <translation>Возможности и зависимости хоста</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6798"/>
        <source>Distribution:</source>
        <translation>Распределение:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6799"/>
        <source>Package manager family:</source>
        <translation>Семья менеджеров пакетов:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6800"/>
        <source>KAuth build support:</source>
        <translation>Поддержка KAuth build:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6805"/>
        <source>Feature</source>
        <translation>Особенность</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6805"/>
        <source>Command</source>
        <translation>Командование</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6805"/>
        <source>Scope</source>
        <translation>охват</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6806"/>
        <source>Suggested package</source>
        <translation>Предлагаемый пакет</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6806"/>
        <source>Notes</source>
        <translation>Заметки</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6855"/>
        <source>Refresh Capabilities</source>
        <translation>Обновить возможности</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6856"/>
        <source>Install Missing Support…</source>
        <translation>Недостающая поддержка …</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="6858"/>
        <source>Automatic installation will require explicit package mapping and privilege authorization.</source>
        <translation>Автоматическая установка потребует явного отображения пакетов и авторизации привилегий.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7082"/>
        <source>Scanning block devices…</source>
        <translation>Сканирующие блоки устройств …</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7093"/>
        <location filename="../src/MainWindow.cpp" line="7094"/>
        <source>Device scan failed</source>
        <translation>Сканирование устройства провалилось</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7126"/>
        <source>Device scan complete — discovery did not modify storage</source>
        <translation>Сканирование устройства завершено — обнаружение не изменило хранилище</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7181"/>
        <source>Showing %1 of %2 repair candidate disk(s); %3 running-system disk(s) protected</source>
        <translation>Показаны %1 диска (дисков)-кандидатов на ремонт %2; защищенный (защищенный) диск (диски) операционной системы %3</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7251"/>
        <source>Running system protection unresolved</source>
        <translation>Защита операционной системы не решена</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7252"/>
        <source>No protected physical backing disk was identified</source>
        <translation>Защищенный физический бэк-диск не обнаружен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7262"/>
        <source>Current running Linux system</source>
        <translation>Текущая работающая система Linux</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7266"/>
        <source>Protected running-system storage</source>
        <translation>Защищенное хранилище операционной системы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7267"/>
        <source>Critical mounts: %1</source>
        <translation>Критические крепления: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7347"/>
        <source>Select to inspect this partition/volume. Select Target still chooses the physical drive and its preferred Linux root.
%1</source>
        <translation>Выберите, чтобы проверить этот раздел / объем. Select Target по-прежнему выбирает физический диск и предпочитаемый корень Linux.
%1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7352"/>
        <source>Select this physical drive as the repair target. Expand it only to view technical partition/volume details.
%1</source>
        <translation>Выберите этот физический привод в качестве цели ремонта. Расширяйте его только для просмотра технических деталей раздела / объема.
%1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7425"/>
        <source>Showing protected running-host details</source>
        <translation>Показ защищенных деталей бегущего хоста</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7479"/>
        <source>Host maintenance unavailable</source>
        <translation>Обслуживание хоста недоступно</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7493"/>
        <source>Exit Host Maintenance</source>
        <translation>Обслуживание Exit Host</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7494"/>
        <source>Exit Host Maintenance — leave running-host maintenance and return to ordinary repair-target mode.</source>
        <translation>Exit Host Maintenance — оставьте обслуживание бегущего хоста и вернитесь в обычный режим ремонта-цели.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7539"/>
        <source>Host default unavailable</source>
        <translation>Host default недоступен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7545"/>
        <source>Make host the default boot entry</source>
        <translation>Сделайте хост по умолчанию boot entry</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7546"/>
        <source>Restore/ensure and select the running host&apos;s default boot entry?</source>
        <translation>Восстановить/убедиться и выбрать загрузочный вход запущенного хоста по умолчанию?</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7552"/>
        <location filename="../src/MainWindow.cpp" line="12643"/>
        <source>default boot entry</source>
        <translation>Загрузка по умолчанию</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7553"/>
        <location filename="../src/MainWindow.cpp" line="12644"/>
        <source>%1 default boot entry</source>
        <translation>Загрузка по умолчанию %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7555"/>
        <source>Host disk: %1
Root: %2

Boot Bitch will identify the host ESP, restore/ensure the verified %3, and place it first in BootOrder. The fallback/recovery route and every other firmware entry stay bootable and are never deleted.</source>
        <translation>Диск хоста: %1
Источник: %2

Boot Bitch идентифицирует хост ESP, восстанавливает/обеспечивает верифицированный %3 и помещает его первым в BootOrder. Маршрут резервного копирования / восстановления и каждая другая запись прошивки остаются загрузочными и никогда не удаляются.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7559"/>
        <source>Host disk: %1
Root: %2

Boot Bitch will identify the running host&apos;s boot configuration, restore/ensure the verified %3, and select it as the default. Every other boot entry stays bootable and is never deleted.</source>
        <translation>Диск хоста: %1
Источник: %2

Boot Bitch идентифицирует конфигурацию загрузки запущенного хоста, восстановит / обеспечит верифицированный %3 и выберет его по умолчанию. Каждая другая загрузочная запись остается загрузочной и никогда не удаляется.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7577"/>
        <source>Make host default boot entry</source>
        <translation>Загрузка host default boot entry</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7628"/>
        <source>Host default set to %1</source>
        <translation>Хостинг по умолчанию установлен на %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7631"/>
        <source>Host default verified</source>
        <translation>Проверенный дефолт хоста</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7632"/>
        <source>%1

Full helper output is available in Logs.</source>
        <translation>%1

Полный вспомогательный выход доступен в журналах.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7639"/>
        <source>Host default not verified: the helper did not name a verified boot entry.</source>
        <translation>Хост по умолчанию не проверен: помощник не назвал верифицированную загрузочную запись.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7640"/>
        <source>Host default not verified</source>
        <translation>Дефолт хоста не проверен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7642"/>
        <source>Host default operation failed: %1</source>
        <translation>Операция по умолчанию не удалась: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7645"/>
        <source>Host default operation failed</source>
        <translation>Операция по умолчанию Host не удалась</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7646"/>
        <source>The host default operation failed: %1

Full helper output is available in Logs.</source>
        <translation>Операция хоста по умолчанию не удалась: %1

Полный вспомогательный выход доступен в журналах.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7666"/>
        <source>Pending inspection</source>
        <translation>Предстоящая проверка</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7679"/>
        <source>PROTECTED — running system; read-only details only</source>
        <translation>Защищено — запущенная система; только детали для чтения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7681"/>
        <source>Live / installer media — not selectable</source>
        <translation>Live / Installer Media - не выбираемый</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7683"/>
        <source>Unlock required before selection</source>
        <translation>Unlock требуется перед выбором</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7684"/>
        <source>Eligible repair candidate</source>
        <translation>Кандидат на ремонт</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7698"/>
        <source>Commit this physical drive as the repair target.</source>
        <translation>Совершите этот физический драйв в качестве цели ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7701"/>
        <source>Live / installer media is read-only boot media and cannot be selected as a repair target.</source>
        <translation>Живые / установочные носители - это загрузочные носители только для чтения и не могут быть выбраны в качестве цели ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7703"/>
        <source>Unlock the encrypted volume first; Select Target becomes available after a Linux filesystem is detected.</source>
        <translation>Сначала разблокируйте зашифрованный том; после обнаружения файловой системы Linux становится доступен Select Target.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7704"/>
        <source>No selectable repair target was detected on this drive.</source>
        <translation>На этом приводе не было обнаружено какой-либо выборочной цели ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7705"/>
        <source>The protected running host cannot be selected as a repair target.</source>
        <translation>Защищенный работающий хост не может быть выбран в качестве цели ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7721"/>
        <source>Already unlocked before this Boot Bitch session. No unlock operation was performed here; the visible mapper will be reused and will not be closed by Boot Bitch.</source>
        <translation>Уже разблокирован перед сессией Boot Bitch. Здесь не проводилась операция разблокировки; видимый картограф будет повторно использоваться и не будет закрыт Boot Bitch.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7729"/>
        <source>Already Unlocked</source>
        <translation>Уже разблокирован</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7733"/>
        <source>Unlock %1 using cryptsetup through the privileged helper. The passphrase is sent on standard input and is never placed in command arguments or logs.</source>
        <translation>Разблокируйте %1 с помощью криптозащиты через привилегированного помощника. Парольная фраза отправляется на стандартный вход и никогда не помещается в командные аргументы или журналы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7736"/>
        <source>An unlocked Linux filesystem is already visible on this drive. Boot Bitch will reuse the existing mapper and will not close or reopen a mapping created by another recovery tool.</source>
        <translation>На этом диске уже видна разблокированная файловая система Linux. Boot Bitch будет повторно использовать существующий картограф и не будет закрывать или повторно открывать отображение, созданное другим инструментом восстановления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7739"/>
        <source>No locked LUKS component is currently visible on this selected drive.</source>
        <translation>На этом выбранном диске в настоящее время не видно запертого компонента LUKS.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7741"/>
        <source>The protected running host cannot be unlocked or modified by Boot Bitch.</source>
        <translation>Защищенный хост не может быть разблокирован или изменен Boot Bitch.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7764"/>
        <location filename="../src/MainWindow.cpp" line="7800"/>
        <source>Target unavailable</source>
        <translation>Цель недоступна</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7765"/>
        <source>The selected drive is no longer available. Refresh devices and select it again.</source>
        <translation>Выбранный диск больше не доступен. Обновите устройства и выберите его снова.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7778"/>
        <source>Protected system</source>
        <translation>Защищенная система</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7779"/>
        <source>The running system cannot be selected as a repair target.</source>
        <translation>Система не может быть выбрана в качестве цели ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7788"/>
        <source>Live / installer media cannot be selected</source>
        <translation>Live / Installer Media не может быть выбран</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7789"/>
        <source>This drive carries read-only live/installer media. Insert or select a data drive instead; the boot media itself is never a repair target.</source>
        <translation>Этот диск несет только для чтения живые / инсталляторы. Вставьте или выберите диск данных; загрузочный носитель сам по себе никогда не является целью ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7795"/>
        <source>Unlock or select a Linux system first</source>
        <translation>Сначала разблокируйте или выберите систему Linux</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7796"/>
        <source>This encrypted drive has no visible Linux filesystem yet. Use Unlock, refresh devices, and select the target after its Linux root is detected.</source>
        <translation>Этот зашифрованный диск еще не имеет видимой файловой системы Linux. Используйте Unlock, обновите устройства и выберите цель после обнаружения корня Linux.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7801"/>
        <source>This drive has no resolvable target component. Refresh devices and select it again.</source>
        <translation>Этот диск не имеет разрешимого целевого компонента. Обновите устройства и выберите его снова.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7819"/>
        <source>Repair drive selected: %1</source>
        <translation>Выбранный привод для ремонта: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="7970"/>
        <source>Requesting administrator authorization</source>
        <translation>Запросить разрешение администратора</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8009"/>
        <location filename="../src/MainWindow.cpp" line="8152"/>
        <source>Administrator authorization active for this Boot Bitch window</source>
        <translation>Активное разрешение администратора для этого окна Boot Bitch</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8359"/>
        <source>Requesting administrator authorization…</source>
        <translation>Запрос авторизации администратора …</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8360"/>
        <source>The single Polkit authorization request is already running. The Authorize button re-enables when it finishes.</source>
        <translation>Запрос на авторизацию Polkit уже запущен. Кнопка авторизации включается, когда она заканчивается.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8372"/>
        <source>Administrator authorization deferred — diagnostics will regenerate after you authorize.</source>
        <translation>Разрешение администратора отложено — диагностика восстановится после вашего разрешения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8379"/>
        <source>Administrator authorization failed — press Authorize to retry.</source>
        <translation>Разрешение администратора не удалось — пресса Разрешить на повторную попытку.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8614"/>
        <source>another privileged operation</source>
        <translation>Еще одна привилегированная операция</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8618"/>
        <source>superseded by the scope change to %1 and was not queued</source>
        <translation>Заменен изменением области охвата на %1 и не стоял в очереди</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8619"/>
        <source>refused because &apos;%1&apos; is still running and was not queued. Wait for it to finish and retry, or cancel it</source>
        <translation>Он отказался, потому что %1 все еще работает и не стоял в очереди. Подождите, пока он закончит и повторит, или отмените его.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8620"/>
        <source>Privileged request &apos;%1&apos; was %2.</source>
        <translation>Привилегированным запросом %1 был %2.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8622"/>
        <source>ERROR: Privileged request &apos;%1&apos; was %2.
</source>
        <translation>Привилегированным запросом %1 был %2.
</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8695"/>
        <source>The administrator authorization session could not be established.</source>
        <translation>Сессия авторизации администратора не может быть создана.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8701"/>
        <source>Authorization unavailable</source>
        <translation>Авторизация недоступна</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8711"/>
        <source>The privileged helper session is not running; the plan stopped before its request started.</source>
        <translation>Сеанс привилегированного помощника не работает; план остановился до того, как его просьба началась.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8723"/>
        <source>Using the authorized Boot Bitch administrator session. The GUI itself is still running as your normal user.</source>
        <translation>Использование авторизованной сессии администратора Boot Bitch. Сам GUI по-прежнему работает как обычный пользователь.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8768"/>
        <source>Privileged request &apos;%1&apos; exceeded the %2 safety limit; aborting the request and closing the unresponsive helper session so the UI cannot stay busy indefinitely.</source>
        <translation>Привилегированный запрос «%1» превысил предел безопасности %2; отмена запроса и закрытие неотзывчивой сессии помощника, чтобы пользовательский интерфейс не мог оставаться занятым бесконечно.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8771"/>
        <source>The privileged operation exceeded the %1 safety limit and was aborted. Review the output before retrying.</source>
        <translation>Привилегированная операция превысила предел безопасности %1 и была прервана. Просмотрите выход перед повторным использованием.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8880"/>
        <source>Protocol note: a PROMPT record exceeded the 1024-byte payload bound and was dropped; the helper was answered with an empty ANSWER record.</source>
        <translation>Протокольное примечание: запись PROMPT превысила предел полезной нагрузки в 1024 байта и была сброшена; помощнику ответили пустой записью ANSWER.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8905"/>
        <source>ERROR: The privileged request exceeded the interactive prompt budget; treating the request as a protocol failure.</source>
        <translation>Ошибка: привилегированный запрос превысил интерактивный оперативный бюджет; рассматривая запрос как сбой протокола.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8925"/>
        <source>Shell command is asking for input</source>
        <translation>Команда Shell просит ввести</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8928"/>
        <source>The shell command is waiting for input. Review its output below, type the answer and press OK. Cancel — or an empty answer — stops the command.</source>
        <translation>Командование снаряда ждет ввода. Просмотрите его вывод ниже, введите ответ и нажмите OK. Отмена — или пустой ответ — останавливает команду.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8942"/>
        <source>Answer (for example: y, n, Y, I, N, Z, or a word)</source>
        <translation>Ответ (например: y, n, Y, I, N, Z или слово)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="8946"/>
        <source>Send Answer</source>
        <translation>Отправить ответ</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9013"/>
        <source>Privileged operation completed successfully. Administrator authorization remains active for this Boot Bitch window.</source>
        <translation>Привилегированная операция успешно завершена. Для этого окна Boot Bitch остается активным разрешение администратора.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9014"/>
        <source>Privileged operation stopped with an error. Administrator authorization remains active; review the output before closing.</source>
        <translation>Привилегированная операция прекратилась с ошибкой. Разрешение администратора остается активным; просмотрите результаты до закрытия.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9038"/>
        <source>The privileged helper session ended unexpectedly. The next root action will require authorization again.</source>
        <translation>Сеанс привилегированного помощника закончился неожиданно. Следующее действие корня потребует повторного авторизации.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9137"/>
        <source>ERROR: Privileged request &apos;%1&apos; exceeded the %2 safety limit; the request was aborted and the unresponsive helper session was closed. The operation may not have completed.</source>
        <translation>Привилегированный запрос %1 превысил предел безопасности %2; запрос был прерван, а неотзывчивый сеанс помощника был закрыт. Операция может не завершиться.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9227"/>
        <source>Unlock not required</source>
        <translation>Unlock не требуется</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9229"/>
        <source>This drive already has an unlocked Linux filesystem. Boot Bitch will reuse the existing mapper.</source>
        <translation>Этот диск уже имеет разблокированную файловую систему Linux. Boot Bitch будет использовать существующий картограф.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9230"/>
        <source>The selected drive does not currently contain a locked LUKS component that needs to be opened.</source>
        <translation>Выбранный диск в настоящее время не содержит запертого компонента LUKS, который необходимо открыть.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9237"/>
        <location filename="../src/MainWindow.cpp" line="9244"/>
        <location filename="../src/MainWindow.cpp" line="9270"/>
        <source>Unlock unavailable</source>
        <translation>Unlock Недоступно</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9238"/>
        <location filename="../src/MainWindow.cpp" line="18291"/>
        <location filename="../src/MainWindow.cpp" line="18328"/>
        <source>The privileged Boot Bitch helper was not found. Rebuild or install this source tree.</source>
        <translation>Привилегированный помощник Boot Bitch не найден. Восстановление или установка этого дерева.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9245"/>
        <source>pkexec/Polkit is required to authorize LUKS unlock operations.</source>
        <translation>Pkexec/Polkit должен разрешить операции разблокировки LUKS.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9252"/>
        <location filename="../src/MainWindow.cpp" line="9298"/>
        <source>Unlock encrypted repair target</source>
        <translation>Зашифрованная цель восстановления</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9253"/>
        <source>Unlock %1?</source>
        <translation>Разблокировать %1?</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9254"/>
        <source>Target disk: %1

Boot Bitch will ask Polkit for authorization and open a temporary device-mapper mapping. A mapping opened by Boot Bitch remains available for this authorized app session so later diagnostics and repairs can reuse it, then closes when you lock the administrator session or exit. A mapping that was already open before Boot Bitch attached to it is reused but never closed by Boot Bitch.</source>
        <translation>Целевой диск: %1

Boot Bitch запросит у Polkit разрешение и откроет временное картографирование устройства. Открытое Boot Bitch отображение остается доступным для этой авторизованной сессии приложения, поэтому более поздняя диагностика и ремонт могут повторно использовать его, а затем закрываются, когда вы блокируете сеанс администратора или выходите. Картографирование, которое было открыто до присоединения Boot Bitch, используется повторно, но никогда не закрывается Boot Bitch.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9266"/>
        <source>Unlocking %1</source>
        <translation>Разблокировка %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9278"/>
        <source>Unlock LUKS repair target</source>
        <translation>Разблокировать цель ремонта LUKS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9307"/>
        <source>LUKS volume:</source>
        <translation>Объем LUKS:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9317"/>
        <source>Enter the passphrase to unlock this volume.</source>
        <translation>Введите парольную фразу, чтобы разблокировать этот том.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9324"/>
        <source>LUKS passphrase</source>
        <translation>Фраза LUKS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9329"/>
        <source>Administrator authorization is already active. The passphrase is sent only to cryptsetup over the privileged helper pipe and is never logged or placed on a command line.</source>
        <translation>Разрешение администратора уже действует. Пассфраза отправляется только для криптозащиты по привилегированной вспомогательной трубе и никогда не регистрируется или не размещается в командной строке.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9356"/>
        <source>Passphrase required</source>
        <translation>Требуется пароль</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9357"/>
        <source>An empty passphrase was not submitted. Enter the LUKS passphrase or choose Cancel.</source>
        <translation>Пустая фраза не была представлена. Введите парольную фразу LUKS или выберите Отменить.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9368"/>
        <source>Unlock LUKS target</source>
        <translation>Разблокировать цель LUKS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9397"/>
        <source>Passphrase not accepted</source>
        <translation>Перефразирование не принято</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9398"/>
        <source>The LUKS passphrase was not accepted.</source>
        <translation>LUKS не был принят.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9399"/>
        <source>Try again? Administrator authorization remains active, so only the disk passphrase will be requested again.</source>
        <translation>Попробуй еще раз? Авторизация администратора остается активной, поэтому будет запрошена только парольная фраза диска.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9440"/>
        <source>Encrypted target unlocked — select or reselect the repair target</source>
        <translation>Зашифрованная цель разблокирована — выберите или перевыберите цель ремонта</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9451"/>
        <location filename="../src/MainWindow.cpp" line="9499"/>
        <source>Host maintenance: %1</source>
        <translation>Обслуживание хоста: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9453"/>
        <source>Running host selected for guarded maintenance: %1
Root component: %2</source>
        <translation>Бегущий хост, выбранный для охранного обслуживания: %1
Корневой компонент: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9456"/>
        <source>Target: %1</source>
        <translation>Цель: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9457"/>
        <source>Selected physical repair drive: %1</source>
        <translation>Выбранный физический привод для ремонта: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9460"/>
        <source>
Automatically detected system component: %1</source>
        <translation>
Автоматически обнаруженный компонент системы: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9476"/>
        <source>Running Host: %1</source>
        <translation>Разработчик: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9478"/>
        <source>Running host selected for the Host Shell: %1
Root component: %2</source>
        <translation>Ведущий, выбранный для Host Shell: %1
Корневой компонент: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9502"/>
        <source>Committed target: %1</source>
        <translation>Цель: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9504"/>
        <source>Explicit native running-host maintenance is active. Ordinary target repairs, file copy and chroot remain unavailable; running-host Snapper @ snapshot rollback is available in the Snapshots tab.</source>
        <translation>Явное нативное обслуживание бегущего хозяина является активным. Обычный целевой ремонт, копия файла и chroot остаются недоступными; бегущий хост Snapper @ snapshot rollback доступен во вкладке Snapshots.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9506"/>
        <source>Row selection is inspection only. Press Select Target to commit a repair drive.</source>
        <translation>Выбор строки - это только проверка. Нажмите «Выбрать цель», чтобы выполнить ремонтный привод.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9507"/>
        <source>Committed repair target. Repair, Diagnostics, Snapshots and File Copy target this physical drive until another drive is explicitly selected with Select Target.
%1</source>
        <translation>Целевой ремонт. Ремонт, диагностика, снимки и копирование файлов нацелены на этот физический диск, пока другой диск не будет явно выбран с помощью Select Target.
%1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9541"/>
        <source>✓ SELECTED TARGET  •  %1</source>
        <translation>✓ SELECTED TARGET • %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9552"/>
        <source>Committed repair target. Clicking another row changes only the inspection highlight; repair actions continue to target %1 until Select Target is pressed on another physical drive.
%2</source>
        <translation>Целевой ремонт. Нажатие на другую строку изменяет только подсветку инспекции; действия по ремонту продолжают нацеливаться на %1 до тех пор, пока выбранная цель не будет нажата на другой физический диск.
%2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9598"/>
        <source>The running host root is not Btrfs, so Btrfs snapshots are not available.</source>
        <translation>Корень хоста не является Btrfs, поэтому снимки Btrfs недоступны.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9599"/>
        <source>The selected Linux root is not Btrfs, so Btrfs snapshots are not available.</source>
        <translation>Выбранный корень Linux не является Btrfs, поэтому снимки Btrfs недоступны.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9606"/>
        <source>Enumerate running-host root snapshots through a temporary privileged read-only Btrfs mount.</source>
        <translation>Перечислите снимки корня бегущего хоста через временную привилегированную установку Btrfs для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9607"/>
        <source>Enumerate root snapshots through a temporary privileged read-only Btrfs mount.</source>
        <translation>Перечислите снимки корня с помощью временной привилегированной установки Btrfs для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9613"/>
        <source>Inspect the selected snapshot read-only.</source>
        <translation>Проверьте выбранный снимок только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9614"/>
        <source>Select a snapshot row first.</source>
        <translation>Сначала выберите строку снимка.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9623"/>
        <location filename="../src/MainWindow.cpp" line="9634"/>
        <source>Select a valid Linux root snapshot first.</source>
        <translation>Сначала выберите действительный снимок корня Linux.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9639"/>
        <source>Validate a running-host rollback plan read-only, preserve the running @ as @rollback-before-*, promote a writable snapshot copy and reconcile the boot stack in a scratch chroot. A reboot is required and is never automatic.</source>
        <translation>Проверяйте план отката бегущего хоста только для чтения, сохраняйте запущенный @ как @rollback-before-*, продвигайте написанную копию снимка и примиряйте стек загрузки в скретч-корне. Перезагрузка необходима и никогда не бывает автоматической.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9640"/>
        <source>Validate a rollback plan read-only, preserve the current @ root, promote a writable snapshot copy, reconcile initramfs and the detected bootloader path and auto-restore the old @ if validation fails.</source>
        <translation>Проверяйте план отката только для чтения, сохраняйте текущий @ root, продвигайте написанную копию снимка, сверяйте initramfs и обнаруженный путь загрузчика и автоматически восстанавливайте старый @, если проверка не удалась.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9711"/>
        <source>Btrfs snapshot preload for %1 is deferred until the running privileged operation finishes; the request was not queued.</source>
        <translation>Предварительная загрузка снимка Btrfs для %1 откладывается до окончания выполнения привилегированной операции; запрос не стоял в очереди.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9726"/>
        <source>Loading Btrfs snapshots in the background…</source>
        <translation>Загрузка снимков Btrfs в фоновом режиме …</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9783"/>
        <source>Btrfs snapshot inventory is not applicable: the running host root filesystem is %1, not Btrfs.

No snapshot was loaded and the running host was not modified.</source>
        <translation>Кадастр снимков Btrfs неприменим: запущенная корневая файловая система хоста - это %1, а не Btrfs.

Ни один снимок не был загружен, а бегущий хост не был изменен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9784"/>
        <source>Btrfs snapshot inventory is not applicable: the selected target filesystem is %1, not Btrfs.

No snapshot was loaded and the target was not modified.</source>
        <translation>Опись Btrfs не применима: выбранная целевая файловая система - %1, а не Btrfs.

Ни один снимок не был загружен, и цель не была изменена.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9798"/>
        <source>Snapshot inventory unavailable</source>
        <translation>Инвентарь снимков недоступен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9827"/>
        <source>Btrfs snapshot inventory for %1 is deferred until the running privileged operation finishes; the request was not queued.</source>
        <translation>Опись моментального снимка Btrfs для %1 откладывается до окончания привилегированной операции; запрос не стоял в очереди.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9833"/>
        <source>Loading Btrfs snapshots after the running operation finishes…</source>
        <translation>Загрузка снимков Btrfs после завершения работы …</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9839"/>
        <source>Loading running-host Btrfs snapshots</source>
        <translation>Загрузка бегущего хоста Btrfs</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9840"/>
        <source>Loading Btrfs snapshots</source>
        <translation>Загрузка снимков Btrfs</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9855"/>
        <source>Load running-host Btrfs snapshots</source>
        <translation>Загрузка бегущего хоста Btrfs</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9856"/>
        <source>Load Btrfs snapshots</source>
        <translation>Скачать Btrfs snapshots</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9890"/>
        <source>Snapshot inventory failed. See Logs for details.</source>
        <translation>Снимки провалились. Смотрите журналы для деталей.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9943"/>
        <source>Loaded %1 running-host Btrfs root snapshot(s) read-only at %2. Select a row and choose Inspect Selected, or double-click a row, for snapshot-specific validation.

No snapshot or host file was modified.</source>
        <translation>Загруженный %1-хост Btrfs root snapshot(s) только для чтения на %2. Выберите строку и выберите Inspect Selected или дважды щелкните строку для валидации для конкретного снимка.

Ни один снимок или хост-файл не был изменен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9945"/>
        <source>Loaded %1 Btrfs root snapshot(s) read-only at %2. Select a row and choose Inspect Selected, or double-click a row, for snapshot-specific validation.

No snapshot or target file was modified.</source>
        <translation>Загруженный %1 Btrfs root snapshot(s) только для чтения на %2. Выберите строку и выберите Inspect Selected или дважды щелкните строку для валидации для конкретного снимка.

Ни один снимок или целевой файл не был изменен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9949"/>
        <source>No Snapper-style Btrfs root snapshots or Boot Bitch rollback backups were found on the running host. The scan was read-only.</source>
        <translation>На бегущем хосте не было обнаружено снимков корня Btrfs в стиле Snapper или резервных копий Boot Bitch. Сканирование было только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="9950"/>
        <source>No Snapper-style Btrfs root snapshots were found on the selected target. The scan was read-only.</source>
        <translation>На выбранной цели не было обнаружено снимков корня Btrfs в стиле Snapper. Сканирование было только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10035"/>
        <source>Inspecting running-host snapshot %1</source>
        <translation>Снимок бегущего хоста %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10036"/>
        <source>Inspecting snapshot %1</source>
        <translation>Скриншоты %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10044"/>
        <source>Inspect running-host Btrfs snapshot %1</source>
        <translation>Осмотр бегущего хоста Btrfs snapshot %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10045"/>
        <source>Inspect Btrfs snapshot %1</source>
        <translation>Проверить снимок Btrfs %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10054"/>
        <source>Captured: %1

%2</source>
        <translation>Производитель: %1

%2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10082"/>
        <source>Rolling back snapshot %1</source>
        <translation>Откат назад снимок %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10088"/>
        <source>Preflight snapshot rollback %1</source>
        <translation>Скриншоты из игры Preflight Rollback %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10097"/>
        <source>Rollback preflight captured: %1

%2</source>
        <translation>Захваченный обратный рейс: %1

%2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10101"/>
        <source>Rollback preflight failed</source>
        <translation>Rollback Preflight провалился</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10102"/>
        <source>The selected snapshot did not pass the transactional rollback preflight. No rollback was performed.

Review the Snapshots details pane and Logs.</source>
        <translation>Выбранный снимок не прошел предполетный транзакционный откат. Откат не производился.

Обзор панели деталей Snapshots и журналов.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10108"/>
        <source>Confirm transactional snapshot rollback</source>
        <translation>Подтвердить транзакционный моментальный откат</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10109"/>
        <source>Promote snapshot %1 to the normal writable @ root?</source>
        <translation>Продвигать снимок %1 до нормального записного @ root?</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10110"/>
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
        <location filename="../src/MainWindow.cpp" line="10118"/>
        <location filename="../src/MainWindow.cpp" line="10301"/>
        <source>Continue to Confirmation</source>
        <translation>Продолжайте подтверждать</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10128"/>
        <location filename="../src/MainWindow.cpp" line="10311"/>
        <source>Type ROLLBACK to continue</source>
        <translation>ROLLBACK продолжится</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10129"/>
        <source>This operation changes the active Btrfs root and rebuilds boot artifacts.

Type ROLLBACK exactly to continue:</source>
        <translation>Эта операция изменяет активный корень Btrfs и восстанавливает загрузочные артефакты.

Введите ROLLBACK, чтобы продолжить:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10142"/>
        <source>Roll back to Btrfs snapshot %1</source>
        <translation>Скачать Btrfs snapshot %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10151"/>
        <source>Rollback finished: %1

%2</source>
        <translation>Скриншоты из игры %1

%2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10159"/>
        <source>Rollback completed: %1

%2

Snapshot inventory was cleared because the active root changed. Choose Load Snapshots to refresh it.</source>
        <translation>Завершено обновление: %1

%2

Опись снимков была очищена, потому что активный корень изменился. Выберите Load Snapshots, чтобы обновить его.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10167"/>
        <source>Snapshot rollback complete</source>
        <translation>Скриншоты из игры Snapshot Rollback Complete</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10168"/>
        <source>Snapshot %1 was promoted to a writable @ root and the boot stack passed reconciliation.

The previous @ was retained under a timestamped @rollback-before-* name. Reboot using the normal boot path when ready.</source>
        <translation>Snapshot %1 был повышен до написанного @ root, и стек загрузки прошел согласование.

Предыдущий @ был сохранен под меткой @rollback-before-*. Перезагрузка с использованием обычного пути загрузки, когда он готов.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10177"/>
        <source>Rollback attempt finished: %1

%2

Cached diagnostics and snapshot inventory were cleared because the rollback request may have changed and/or restored target state.</source>
        <translation>Попытка отката завершена: %1

%2

Кашедная диагностика и инвентаризация снимков были очищены, потому что запрос на откат мог изменить и / или восстановить целевое состояние.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10185"/>
        <source>Snapshot rollback failed</source>
        <translation>Откат снимка провалился</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10186"/>
        <source>The rollback did not complete successfully. Do not reboot until you review the Snapshots output and Logs. The helper attempts to restore the preserved @ automatically when post-switch validation fails.</source>
        <translation>Откат не завершился успешно. Не перезагружайте, пока не просмотрите выход Snapshots и журналы. Помощник пытается восстановить сохраненный @ автоматически, когда валидация после переключения не удается.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10202"/>
        <source>Running-host snapshot rollback is only available in Host Maintenance.</source>
        <translation>Running-host snapshot rollback доступен только в Host Maintenance.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10219"/>
        <location filename="../src/MainWindow.cpp" line="10228"/>
        <source>Running-host snapshot rollback is unavailable.</source>
        <translation>Running-host snapshot rollback недоступен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10222"/>
        <source>Running-host snapshot rollback has unknown capability evidence.</source>
        <translation>Откат снимков бегущего хозяина имеет неизвестные доказательства способности.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10229"/>
        <source>Run running-host diagnostics in Host Maintenance first.</source>
        <translation>Сначала запустите диагностику бегущего хоста в обслуживании хоста.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10232"/>
        <source>Running-host snapshot rollback is available.</source>
        <translation>Running-host snapshot rollback доступен.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10246"/>
        <source>Rollback already staged</source>
        <translation>Роллбэк уже поставлен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10247"/>
        <source>A running-host rollback to %1 is already staged and takes effect on the next reboot.</source>
        <translation>Откат бегущего хоста к %1 уже инсценирован и вступает в силу при следующей перезагрузке.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10249"/>
        <source>an earlier snapshot</source>
        <translation>Более ранний снимок</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10250"/>
        <source>snapshot %1</source>
        <translation>Фотография %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10251"/>
        <source>Rolling back again preserves the currently running root as a new @rollback-before-* backup and replaces the staged snapshot. The earlier staged root remains on disk but is no longer the recorded undo point.

Continue only if you intend to replace the staged rollback.</source>
        <translation>Откат снова сохраняет текущий корень в качестве новой резервной копии @rollback-before-* и заменяет постановочный снимок. Ранний постановочный корень остается на диске, но больше не является записанной точкой отмены.

Продолжайте только в том случае, если вы намерены заменить постановочный откат.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10256"/>
        <source>Replace Staged Rollback</source>
        <translation>Заменить Staged Rollback</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10264"/>
        <source>Rolling back running-host snapshot %1</source>
        <translation>Откат назад бегущий хост снимок %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10270"/>
        <source>Preflight running-host rollback %1</source>
        <translation>Откат бегущего хозяина %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10279"/>
        <source>Running-host rollback preflight captured: %1

%2</source>
        <translation>Откат бегущего хозяина захвачен: %1

%2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10283"/>
        <source>Host rollback preflight failed</source>
        <translation>Host Rollback Preflight провалился</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10284"/>
        <source>The selected snapshot did not pass the running-host rollback preflight. No rollback was performed.

Review the Snapshots details pane and Logs.</source>
        <translation>Выбранный снимок не прошел предполетный откат бегущего хозяина. Откат не производился.

Обзор панели деталей Snapshots и журналов.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10290"/>
        <source>Confirm running-host snapshot rollback</source>
        <translation>Подтвердить откат бегущего хозяина</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10291"/>
        <source>Roll the running host back to snapshot %1?</source>
        <translation>Переверните бегущий хост обратно на снимок %1?</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10292"/>
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
        <location filename="../src/MainWindow.cpp" line="10312"/>
        <source>This operation stages the selected snapshot as the running host&apos;s next root and rebuilds boot artifacts.

Type ROLLBACK exactly to continue:</source>
        <translation>Эта операция делает выбранный снимок следующим корнем бегущего хоста и восстанавливает загрузочные артефакты.

Введите ROLLBACK, чтобы продолжить:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10325"/>
        <source>Roll back the running host to snapshot %1</source>
        <translation>Переверните бегущий хост на снимок %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10334"/>
        <source>Running-host rollback finished: %1

%2</source>
        <translation>Откат бегунов закончился: %1

%2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10346"/>
        <source>Running-host rollback staged: %1

%2

Snapshot inventory and cached running-host diagnostics were cleared because the next boot will start the promoted root. Reboot when ready; the reboot is never automatic.</source>
        <translation>Откат бегущего хозяина: %1

%2

Опись снимков и кэшированная диагностика бегущего хоста были очищены, потому что следующий ботинок запустит продвигаемый корень. Перезагрузка, когда она готова; перезагрузка никогда не бывает автоматической.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10356"/>
        <source>Running-host rollback staged</source>
        <translation>Бегущий хозяин Rollback постановочный</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10357"/>
        <source>Snapshot %1 is staged as the running host&apos;s next root.</source>
        <translation>Snapshot %1 является следующим корнем бегущего хоста.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10358"/>
        <source>Reboot required — the running host will start snapshot %1 after the next reboot. The previous root was retained as an @rollback-before-* undo point and the boot stack passed reconciliation.

Use Reboot Now to reboot immediately, or Later to keep working and reboot manually.</source>
        <translation>Требуется перезагрузка — запущенный хост запустит снимок %1 после следующей перезагрузки. Предыдущий корень был сохранен в виде точки отмены @rollback-before-*, а стек загрузки прошел согласование.

Используйте Reboot Now для немедленной перезагрузки или Later для продолжения работы и перезагрузки вручную.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10381"/>
        <source>Running-host rollback attempt finished: %1

%2

Cached running-host diagnostics and snapshot inventory were cleared. Do not reboot until you review the Snapshots output and Logs.</source>
        <translation>Попытка отката бегущего хозяина завершена: %1

%2

Кэшированная диагностика бегущего хозяина и инвентаризация снимков были очищены. Не перезагружайте, пока не просмотрите выход Snapshots и журналы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10388"/>
        <location filename="../src/MainWindow.cpp" line="10391"/>
        <source>Host rollback failed</source>
        <translation>Host Rollback провалился</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10389"/>
        <source>The running-host rollback did not complete and automatic recovery could not be proven. DO NOT REBOOT. Review the Snapshots output and Logs, and repair the boot stack from another system before rebooting.

The helper output carries HOST_ROLLBACK_RECOVERY and any CRITICAL lines verbatim.</source>
        <translation>Откат бегущего хоста не был завершен и автоматическое восстановление не могло быть доказано. Не перечитывайте. Просмотрите выход Snapshots и журналы и отремонтируйте стек загрузки из другой системы перед перезагрузкой.

Помощник выводит HOST ROLLBACK RECOVERY и любые критические линии дословно.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10392"/>
        <source>The running-host rollback did not complete. The helper reports the preserved @ was restored automatically; do not reboot until you review the Snapshots output and Logs.</source>
        <translation>Откат бегущего хозяина не завершился. Помощник сообщает, что сохраненный @ был восстановлен автоматически; не перезагружайте, пока вы не просмотрите выход Snapshots и журналы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10503"/>
        <location filename="../src/MainWindow.cpp" line="10504"/>
        <source>Reboot the running host now?</source>
        <translation>Перезагрузить бегущего хозяина?</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10505"/>
        <source>The rolled-back snapshot is already staged and takes effect on the next boot. Rebooting now signs out all users and closes unsaved work.

The host reboots only after this separate confirmation.</source>
        <translation>Снимок отката уже поставлен и вступает в силу на следующем ботинке. Перезагрузка теперь выявляет всех пользователей и закрывает несохраненную работу.

Хозяин перезагружается только после этого отдельного подтверждения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10517"/>
        <source>Rebooting running host</source>
        <translation>Перезагрузка бегущего хоста</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10522"/>
        <source>Reboot the running host</source>
        <translation>Перезагрузить бегущий хост</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10530"/>
        <source>Running-host reboot scheduled; the host is restarting.</source>
        <translation>Запуск перезагрузки хоста запланирован; хост перезапускается.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10537"/>
        <source>Host reboot not scheduled</source>
        <translation>Перезагрузка хоста не запланирована</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10539"/>
        <source>The running host did not schedule a reboot. The staged rollback and its reminder remain in effect.</source>
        <translation>Ведущий не планировал перезагрузку. Постановочный откат и его напоминание остаются в силе.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10557"/>
        <source>Repair → Host file copy</source>
        <translation>Ремонт → Копия файла Host</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10558"/>
        <source>Host → Repair file copy</source>
        <translation>→ Восстановить копию файла</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10562"/>
        <source>1. Select source paths from repaired system</source>
        <translation>1.Выберите пути источника из отремонтированной системы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10563"/>
        <source>2. Choose destination on this host</source>
        <translation>2.Выберите пункт назначения на этом хосте</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10564"/>
        <source>Add File Path…</source>
        <translation>Добавить файл Path…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10565"/>
        <source>Add Folder Path…</source>
        <translation>Добавить папку Path…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10566"/>
        <source>Enter an absolute file path as it appears inside the repaired system, for example /home/user/document.txt.</source>
        <translation>Введите абсолютный путь файла, как он появляется внутри восстановленной системы, например /home/user/document.txt.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10567"/>
        <source>Enter an absolute folder path as it appears inside the repaired system.</source>
        <translation>Введите абсолютный путь папки, как он появляется в восстановленной системе.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10568"/>
        <source>Choose a host destination folder</source>
        <translation>Выберите папку host destination</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10569"/>
        <source>Browse…</source>
        <translation>Browse…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10572"/>
        <source>1. Select source files or folders from this host</source>
        <translation>1.Выберите исходные файлы или папки этого хоста</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10573"/>
        <source>2. Choose destination in repaired system</source>
        <translation>2.Выберите пункт назначения в отремонтированной системе</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10576"/>
        <source>Choose one or more source files from the running host.</source>
        <translation>Выберите один или несколько исходных файлов из запущенного хоста.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10577"/>
        <source>Choose a source folder from the running host.</source>
        <translation>Выберите исходную папку из запущенного хоста.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10579"/>
        <source>Select a repair target first</source>
        <translation>Сначала выберите цель ремонта</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10580"/>
        <source>Choose an absolute destination path inside the repaired system</source>
        <translation>Выберите абсолютный путь назначения внутри отремонтированной системы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10581"/>
        <source>Browse Target Folders…</source>
        <translation>Обсуждение Target Folders…</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10582"/>
        <source>Browse the selected repair system through temporary read-only mounts and choose an absolute destination path. No target files are changed while browsing.</source>
        <translation>Просмотрите выбранную систему ремонта через временные крепления только для чтения и выберите абсолютный путь назначения. Никакие целевые файлы не меняются во время просмотра.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10604"/>
        <source>Run a guarded rsync dry-run. The selected repair filesystem remains read-only.</source>
        <translation>Запускай охраняемую рысь. Выбранная файловая система восстановления остается только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10605"/>
        <source>Copy staged items and verify them. Existing same-name destination content can be overwritten; unrelated destination files are never deleted.</source>
        <translation>Копируйте постановочные элементы и проверяйте их. Существующий одноимённый контент назначения может быть перезаписан; несвязанные файлы назначения никогда не удаляются.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10618"/>
        <source>Choose host destination folder</source>
        <translation>Выберите папку host destination</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10631"/>
        <source>Select a repair target</source>
        <translation>Выберите цель ремонта</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10632"/>
        <source>Select the repair drive in Systems before choosing a destination inside it.</source>
        <translation>Выберите ремонтный привод в системах, прежде чем выбрать пункт назначения внутри него.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10642"/>
        <source>Select repair-system destination</source>
        <translation>Выберите пункт назначения ремонтной системы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10649"/>
        <source>Choose a destination folder inside the repaired system</source>
        <translation>Выберите папку назначения внутри восстановленной системы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10658"/>
        <source>Browse folders from the selected repaired system through temporary read-only mounts. Nothing is written while browsing, and the guarded copy validates the chosen path again before any write.</source>
        <translation>Просмотрите папки из выбранной отремонтированной системы через временные крепления только для чтения. Ничего не написано во время просмотра, и защищенная копия подтверждает выбранный путь еще раз перед любой записью.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10667"/>
        <source>Absolute path inside repaired system (for example /home/user/Recovered)</source>
        <translation>Абсолютный путь внутри отремонтированной системы (например, /home/user/Recovered)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10669"/>
        <source>Browse repair folders…</source>
        <translation>Ремонтные папки …</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10670"/>
        <source>Open a read-only view of the selected repair system&apos;s folders. The target is unmounted again after each directory listing.</source>
        <translation>Откройте только для чтения вид папок выбранной системы ремонта. Цель снова не установлена после каждого списка каталогов.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10673"/>
        <source>Clear the destination path.</source>
        <translation>Очистите путь назначения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10688"/>
        <location filename="../src/MainWindow.cpp" line="10774"/>
        <source>Browse repaired-system folders</source>
        <translation>Просмотр исправленных системных папок</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10701"/>
        <source>Choose a folder from the repaired system</source>
        <translation>Выберите папку из отремонтированной системы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10710"/>
        <source>The selected repair filesystem is mounted read-only only for the current directory listing. The mount is removed after the request; the host filesystem is never used as the folder tree.</source>
        <translation>Выбранная файловая система восстановления монтируется только для чтения только для текущего списка каталогов. Монтаж удаляется после запроса; файловая система хоста никогда не используется в качестве дерева папок.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10735"/>
        <source>Up</source>
        <translation>Вверх</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10736"/>
        <source>Open Selected</source>
        <translation>Открытый выбор</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10738"/>
        <source>Show the parent folder in the repaired system.</source>
        <translation>Покажите родительскую папку в отремонтированной системе.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10739"/>
        <source>Open the selected repaired-system folder.</source>
        <translation>Откройте выбранную папку исправленной системы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10740"/>
        <source>Reload this repaired-system folder through a fresh read-only mount.</source>
        <translation>Перезагрузите эту исправленную системную папку через новое крепление только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10747"/>
        <source>Read-only target view; no files are modified.</source>
        <translation>Просмотр только для чтения; файлы не изменяются.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10753"/>
        <source>Choose This Folder</source>
        <translation>Выберите эту папку</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10766"/>
        <source>Invalid repair-system path.</source>
        <translation>Недействительный путь ремонтной системы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10778"/>
        <source>Unable to read this repaired-system folder. Review Logs for the helper error.</source>
        <translation>Невозможно прочитать эту исправленную системную папку. Отзывы об ошибке Helper.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10808"/>
        <source>Current repaired-system folder: %1</source>
        <translation>Текущая папка исправленной системы: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10812"/>
        <source>Read-only target view; %1 subfolder(s) found. Nothing was modified.</source>
        <translation>Вид цели только для чтения; найдена подпапка %1. Ничего не было изменено.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10865"/>
        <location filename="../src/MainWindow.cpp" line="10896"/>
        <location filename="../src/MainWindow.cpp" line="10943"/>
        <source>Invalid repair path</source>
        <translation>Неверный путь ремонта</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10866"/>
        <location filename="../src/MainWindow.cpp" line="10897"/>
        <location filename="../src/MainWindow.cpp" line="10944"/>
        <source>Use an absolute path inside the repaired system. Parent-directory escape paths are not accepted.</source>
        <translation>Используйте абсолютный путь внутри отремонтированной системы. Пути побега родителей не принимаются.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10886"/>
        <source>Repair-system source file</source>
        <translation>Ремонт исходного файла системы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10887"/>
        <source>Enter an absolute file path inside the repaired system:</source>
        <translation>Введите абсолютный путь файла в восстановленную систему:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10911"/>
        <source>Select host files</source>
        <translation>Выберите host файлы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10933"/>
        <source>Repair-system source folder</source>
        <translation>Исходная папка системы восстановления</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10934"/>
        <source>Enter an absolute folder path inside the repaired system:</source>
        <translation>Введите абсолютный путь папки внутри отремонтированной системы:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10958"/>
        <source>Select host folder</source>
        <translation>Выберите папку host</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="10998"/>
        <source>Add at least one source file or folder.</source>
        <translation>Добавьте хотя бы один исходный файл или папку.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11002"/>
        <source>Choose a destination folder.</source>
        <translation>Выберите папку назначения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11006"/>
        <source>rsync is required for verified File Copy.</source>
        <translation>rsync требуется для верифицированной копии файла.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11015"/>
        <source>Repair → Host requires an existing, non-symlink host destination folder.</source>
        <translation>Ремонт → Хост требует существующей папки назначения хоста без симлинков.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11020"/>
        <source>Host → Repair requires a specific absolute path inside the repaired system.</source>
        <translation>Ремонт хоста → требует определенного абсолютного пути внутри отремонтированной системы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11093"/>
        <source>Read-only diagnostics are being generated. Commands stay disabled until the refresh completes so no command runs against a half-refreshed evidence state.</source>
        <translation>Создается только диагностика. Команды остаются отключенными до тех пор, пока обновление не завершится, поэтому никакая команда не работает против полуобновленного состояния доказательств.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11098"/>
        <location filename="../src/MainWindow.cpp" line="11154"/>
        <source>Execute a command on the running host as root.</source>
        <translation>Выполните команду на бегущем хосте в качестве корня.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11110"/>
        <location filename="../src/MainWindow.cpp" line="11248"/>
        <source>Host shell</source>
        <translation>Хранитель снаряда</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11118"/>
        <source>Run a command on the running host as root (sudo is not needed). Commands are executed directly on the active system. When a command asks a question, Boot Bitch shows it in a popup and sends your answer back to the command; cancelling stops the command. Non-interactive flags such as apt-get -y upgrade remain recommended for unattended runs. Output is kept in this window and in the application log.</source>
        <translation>Запустите команду на бегущем хосте как root (судо не требуется). Команды выполняются непосредственно в активной системе. Когда команда задает вопрос, Boot Bitch показывает его всплывающим окном и отправляет ответ обратно в команду; отмена останавливает команду. Неинтерактивные флаги, такие как обновление apt-get-y, по-прежнему рекомендуются для необслуживаемых запусков. Выход хранится в этом окне и в журнале приложений.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11125"/>
        <source>Command to run on the running host, for example: apt update</source>
        <translation>Команда для запуска на запущенном хосте, например: обновление apt</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11132"/>
        <source>Run on Host</source>
        <translation>Беги по хосту</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11136"/>
        <source>Commands can modify the running host. Review each command before running it.</source>
        <translation>Команды могут изменять работающий хост. Проверяйте каждую команду перед запуском.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11150"/>
        <source>Shell</source>
        <translation>Shell</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11151"/>
        <source>Host Shell</source>
        <translation>Ведущий Shell</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11155"/>
        <source>Execute a command inside the selected repair system as root.</source>
        <translation>Выполните команду внутри выбранной системы ремонта как root.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11195"/>
        <source>• %1: %2 changed from &apos;%3&apos; to &apos;%4&apos;</source>
        <translation>• %1: %2 изменился с «%3» на «%4»</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11219"/>
        <source>Host shell unavailable</source>
        <translation>Host shell недоступен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11220"/>
        <source>Chroot shell unavailable</source>
        <translation>Chroot shell недоступен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11235"/>
        <source>The running-host identity is unresolved; no host shell command was sent.</source>
        <translation>Идентификация бегущего хоста неразрешена; команда оболочки хоста не была отправлена.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11236"/>
        <source>The repair target identity is incomplete; no chroot shell command was sent.</source>
        <translation>Идентификатор цели ремонта неполный; командная оболочка не была отправлена.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11244"/>
        <source>Running host shell command</source>
        <translation>Запуск команды Host shell</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11245"/>
        <source>Running chroot shell command</source>
        <translation>Запуск команды Chroot shell</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11292"/>
        <source>the affected repository</source>
        <translation>Пострадавшее хранилище</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11298"/>
        <source>Repository metadata changed</source>
        <translation>Изменились метаданные хранилища</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11299"/>
        <source>The repository changed its release metadata:
%1</source>
        <translation>Репозиторий изменил свои метаданные выпуска:
%1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11305"/>
        <source>The package manager refused the change. Allowing it re-runs the command once with Acquire::AllowReleaseInfoChange=true; the change is accepted for this retry only. Signature, key and package verification remain enforced.

Retry command:
%1</source>
        <translation>Менеджер пакета отказался от изменений. Разрешить повторно запустить команду один раз с помощью Acquire:: AllowReleaseInfoChange=true; изменение принимается только для этого повтора. Проверка подписи, ключа и пакета остается в силе.

Командование Retry:
%1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11313"/>
        <source>Allow and Retry</source>
        <translation>Разрешить и повторить</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11332"/>
        <source>Host shell response incomplete</source>
        <translation>Реакция Host shell неполная</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11332"/>
        <source>Chroot shell response incomplete</source>
        <translation>Реакция Chroot shell неполная</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11333"/>
        <source>The privileged helper did not return a complete response for this command. The request was treated as failed; review the output and Logs before retrying.</source>
        <translation>Привилегированный помощник не вернул полного ответа на эту команду. Запрос был рассмотрен как несостоявшийся; проверьте выход и журналы перед повторным использованием.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11368"/>
        <location filename="../src/MainWindow.cpp" line="11395"/>
        <source>File Copy unavailable</source>
        <translation>File Copy недоступен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11377"/>
        <source>Previewing file copy</source>
        <translation>Предварительный просмотр копии файла</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11388"/>
        <source>Preview File Copy</source>
        <translation>Предварительный просмотр File Copy</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11402"/>
        <source>Smart destination ownership</source>
        <translation>Умное владение</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11425"/>
        <source>… %1 more item(s)</source>
        <translation>… %1 More item(s)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11430"/>
        <source>Confirm File Copy</source>
        <translation>Подтвердить копию файла</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11431"/>
        <source>%1 — copy and verify %2 source item(s)?</source>
        <translation>%1 — копирование и проверка исходного элемента(ов) %2?</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11433"/>
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
        <location filename="../src/MainWindow.cpp" line="11436"/>
        <source>The repair target stays read-only. Existing same-name files in the host destination may be overwritten. The privileged helper refuses system-critical host destinations.

</source>
        <translation>Цель ремонта остается только для чтения. Существующие файлы с тем же именем в пункте назначения хоста могут быть перезаписаны. Привилегированный помощник отказывается от критически важных для системы мест назначения.

</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11438"/>
        <source>The selected repair filesystem is promoted read-write only after host/target safety checks. Existing same-name target files may be overwritten.

</source>
        <translation>Выбранная файловая система ремонта продвигается для чтения-записи только после проверки безопасности хоста/цели. Существующие целевые файлы могут быть перезаписаны.

</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11440"/>
        <source>SENSITIVE TARGET PATH: this destination can change boot or operating-system files.

</source>
        <translation>SENSITIVE TARGET PATH: этот пункт назначения может изменять загрузочные или операционные файлы.

</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11443"/>
        <source>rsync does not use --delete, so unrelated destination files remain. A second checksum/metadata pass and SHA-256 verification of regular files run after the copy.</source>
        <translation>rsync не использует — удаляет, поэтому несвязанные файлы назначения остаются. Второй контрольный пакет и проверка SHA-256 обычных файлов выполняются после копии.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11452"/>
        <source>Copying files</source>
        <translation>Копирование файлов</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11467"/>
        <source>File Copy — %1</source>
        <translation>Копия файла - %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11478"/>
        <source>Available — KF6 KAuth linked</source>
        <translation>Доступно: KF6 KAuth</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11480"/>
        <source>Not compiled — optional integration unavailable</source>
        <translation>Не скомпилировано — опциональная интеграция недоступна</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11492"/>
        <source>Available</source>
        <translation>Доступный</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11492"/>
        <source>Missing</source>
        <translation>Пропавший</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11571"/>
        <source>Running Host diagnostics require Host Maintenance. Choose Enter Host Maintenance on the protected running-host card in Systems first.</source>
        <translation>Диагностика хоста требует обслуживания хоста. Сначала выберите Enter Host Maintenance на защищенной хост-карте в Systems.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11658"/>
        <source>Individual diagnostics have changed. Please re-run all diagnostics.</source>
        <translation>Изменилась индивидуальная диагностика. Пожалуйста, повторите диагностику.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11665"/>
        <source>The selected system changed after a repair action. Please re-run the %1 diagnostic before reviewing or running this repair action.</source>
        <translation>Выбранная система изменилась после ремонта. Пожалуйста, повторите диагностику %1 перед просмотром или запуском этого ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11677"/>
        <source>Select a repair target in Systems.</source>
        <translation>Выберите цель ремонта в системах.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11697"/>
        <source>Cached: %1 result</source>
        <translation>Кашед: результат %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11700"/>
        <source>Cached: %1 — read-only selected repair-system result. Selecting another diagnostic and returning here keeps this output; use Re-run only for fresh data.</source>
        <translation>Cached: %1 — результат только для чтения выбранной системы ремонта. Выбирая другую диагностику и возвращаясь сюда, сохраняйте этот вывод; используйте повторный запуск только для свежих данных.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11701"/>
        <source>Cached: %1 — protected running-host result.</source>
        <translation>Cached: %1 — защищенный результат выполнения хоста.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11702"/>
        <source>Re-run Diagnostic</source>
        <translation>Повторная диагностика</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11706"/>
        <source>Please re-run this diagnostic after the last repair or target configuration change.</source>
        <translation>Пожалуйста, повторите эту диагностику после последнего ремонта или изменения конфигурации цели.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11708"/>
        <source>Available — read-only selected repair-system inspection; administrator authorization is already active for this Boot Bitch window.</source>
        <translation>Доступно — только для чтения выбрана проверка ремонтной системы; авторизация администратора уже активна для этого окна Boot Bitch.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11709"/>
        <source>Available — read-only selected repair-system inspection; the first root action will request administrator authorization.</source>
        <translation>Доступно — только для чтения выбранная проверка ремонтной системы; первое действие root запросит разрешение администратора.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11712"/>
        <source>Please re-run this diagnostic after the last repair action.</source>
        <translation>Пожалуйста, повторите диагностику после последнего ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11713"/>
        <source>Available — protected running host</source>
        <translation>Доступно: защищенный бегущий хост</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11969"/>
        <source>Run running-host diagnostics</source>
        <translation>Диагностика бегущего хозяина</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11970"/>
        <source>Read-only running-host diagnostic</source>
        <translation>Диагностика только для бегущих хостов</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11973"/>
        <source>Run All target diagnostics</source>
        <translation>Запуск всех целевых диагностических</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="11974"/>
        <source>Read-only target diagnostic</source>
        <translation>Диагностика только для чтения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12029"/>
        <source>Starting read-only %1 diagnostic &apos;%2&apos; on %3 (%4).</source>
        <translation>Начало диагностики %1 только для чтения «%2» на %3 (%4).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12040"/>
        <source>Read-only %1 diagnostic &apos;%2&apos; %3.</source>
        <translation>Только для чтения %1 диагностический %2 %3.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12042"/>
        <source>completed</source>
        <translation>завершено</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12042"/>
        <source>failed</source>
        <translation>неудачный</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12078"/>
        <location filename="../src/MainWindow.cpp" line="12158"/>
        <source>%1 finished with exit code 0 (success).</source>
        <translation>%1 завершается выходным кодом 0 (успех).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12093"/>
        <location filename="../src/MainWindow.cpp" line="12210"/>
        <source>Target diagnostic unavailable</source>
        <translation>Диагностика цели недоступна</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12114"/>
        <source>Discarded the read-only target diagnostic &apos;%1&apos; because the active repair target changed while the request was running.</source>
        <translation>Отбросили цель диагностики %1 только для чтения, потому что активная цель ремонта изменилась во время выполнения запроса.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12174"/>
        <location filename="../src/MainWindow.cpp" line="12209"/>
        <source>Host diagnostic unavailable</source>
        <translation>Диагностика хоста недоступна</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12395"/>
        <source>No repair target is ready for diagnostics.</source>
        <translation>Ни одна цель не готова для диагностики.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12398"/>
        <source>The selected repair target is ready for diagnostics.</source>
        <translation>Выбранная цель ремонта готова к диагностике.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12403"/>
        <source>Running Host diagnostics require Host Maintenance. Choose Host Maintenance on the protected running-host card in Systems first.</source>
        <translation>Диагностика хоста требует обслуживания хоста. Сначала выберите обслуживание хоста на защищенной карте бегущего хоста в системах.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12409"/>
        <source>The running host is not resolved for diagnostics.</source>
        <translation>Бегущий хозяин не решен для диагностики.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12412"/>
        <source>The running host is ready for diagnostics.</source>
        <translation>Ведущий готов к диагностике.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12427"/>
        <source>Target diagnostics were invalidated by a repair or target change.</source>
        <translation>Диагностика цели была признана недействительной в результате ремонта или изменения цели.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12431"/>
        <source>Target diagnostic sections were invalidated by a repair action and await regeneration.</source>
        <translation>Целевые диагностические секции были признаны недействительными в результате ремонта и ожидают регенерации.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12435"/>
        <source>No cached diagnostics belong to the selected target.</source>
        <translation>Кэшированная диагностика не относится к выбранной цели.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12439"/>
        <source>No cached diagnostics exist for the selected target.</source>
        <translation>Для выбранной цели не существует кэшированной диагностики.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12446"/>
        <source>Running-host diagnostic sections were invalidated by a repair action and await regeneration.</source>
        <translation>Диагностические секции бегущего хозяина были признаны недействительными в результате ремонта и ожидают регенерации.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12450"/>
        <source>No cached running-host diagnostics exist.</source>
        <translation>Никакой кэшированной диагностики бегущего хозяина не существует.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12466"/>
        <source>Automatic diagnostics regeneration is disabled in Settings.</source>
        <translation>Регенерация автоматической диагностики отключена в настройках.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12469"/>
        <source>A diagnostic or repair operation is already in progress.</source>
        <translation>Диагностическая или ремонтная работа уже ведется.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12479"/>
        <source>Current-scope diagnostic evidence is fresh.</source>
        <translation>Современные диагностические данные свежи.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12501"/>
        <source>Unknown repair tool: %1.</source>
        <translation>Неизвестный инструмент для ремонта: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12505"/>
        <location filename="../src/MainWindow.cpp" line="12694"/>
        <location filename="../src/MainWindow.cpp" line="18271"/>
        <source>Select a repair target in Systems first.</source>
        <translation>Сначала выберите цель ремонта в системе.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12508"/>
        <location filename="../src/MainWindow.cpp" line="12540"/>
        <source>Diagnostic evidence belongs to a different target. Run diagnostics for the selected target.</source>
        <translation>Диагностические данные принадлежат другой цели. Проведите диагностику выбранной цели.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12516"/>
        <source>Target state changed after the last diagnostic or repair action. Please regenerate diagnostics before starting another repair.</source>
        <translation>Состояние цели изменилось после последнего диагностического или ремонтного действия. Пожалуйста, восстановите диагностику перед началом ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12520"/>
        <location filename="../src/MainWindow.cpp" line="12559"/>
        <source>Repair tool %1 is available in the selected scope&apos;s cached diagnostics.</source>
        <translation>Ремонтный инструмент %1 доступен в кэшированной диагностике выбранной области.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12611"/>
        <source>Unrecognised running-host default-entry capability evidence: %1. Run running-host diagnostics again, and update Boot Bitch if the helper format changed.</source>
        <translation>Непризнанные доказательства возможностей ввода по умолчанию: %1. Запустите диагностику хоста и обновите Boot Bitch, если формат помощника изменился.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12617"/>
        <source>The running host&apos;s verified default boot entry can be restored or promoted.</source>
        <translation>Проверенная запись загрузки хоста по умолчанию может быть восстановлена или повышена.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12623"/>
        <source>The cached running-host diagnostics do not decide whether the host default entry can be restored. Regenerate running-host diagnostics in Host Maintenance, and update Boot Bitch if the helper format changed.</source>
        <translation>Кэшированная диагностика бегущего хоста не решает, можно ли восстановить вход по умолчанию. Восстановите диагностику бегущего хоста в обслуживании хоста и обновите Boot Bitch, если формат помощника изменился.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12624"/>
        <source>No running-host default-entry capability evidence is cached. Run read-only running-host diagnostics in Host Maintenance before using Make Default.</source>
        <translation>Ни одно доказательство возможности запуска-хоста по умолчанию не кэшируется. Запустите диагностику только для чтения в обслуживании хоста перед использованием Make Default.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12646"/>
        <source>Restore/ensure the running host&apos;s verified %1 and place it first in BootOrder while preserving every other firmware entry.</source>
        <translation>Восстановите/обеспечьте верифицированный %1 работающего хоста и поместите его первым в BootOrder, сохраняя при этом все другие записи прошивки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12647"/>
        <source>Restore/ensure the running host&apos;s verified %1 and select it as the default while preserving every other boot entry.</source>
        <translation>Восстановите/обеспечьте верифицированный %1 запущенного хоста и выберите его по умолчанию, сохраняя при этом каждую другую загрузочную запись.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12686"/>
        <source>Run the %1 diagnostic for the protected running host before starting this maintenance action.</source>
        <translation>Запустите диагностику %1 для защищенного хоста перед началом этого действия по техническому обслуживанию.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12689"/>
        <source>Required read-only running-host diagnostics are cached for this maintenance action.</source>
        <translation>Требуемая диагностика бегущего хоста только для чтения кэшируется для этого действия по техническому обслуживанию.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12698"/>
        <source>Run the selected target diagnostic before starting this repair.</source>
        <translation>Запустите выбранную целевую диагностику перед началом этого ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12708"/>
        <source>Target state changed after the last diagnostic or repair action. Run the %1 diagnostic for this target before starting this repair.</source>
        <translation>Состояние цели изменилось после последнего диагностического или ремонтного действия. Проведите диагностику %1 для этой цели перед началом ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12709"/>
        <source>Run the %1 diagnostic for this target before starting this repair.</source>
        <translation>Проведите диагностику %1 для этой цели перед началом ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12712"/>
        <source>Required read-only diagnostic evidence is cached for this repair tool.</source>
        <translation>Для этого инструмента ремонта кэшируются необходимые диагностические данные только для чтения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12934"/>
        <source>package metadata refreshed — changes were applied</source>
        <translation>Обновлены метаданные пакета — внесены изменения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12935"/>
        <source>package metadata already current — no changes</source>
        <translation>Метаданные пакета уже актуальны — никаких изменений</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12936"/>
        <source>package metadata refresh failed</source>
        <translation>Обновление метаданных не удалось</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12937"/>
        <source>package metadata refresh not checked</source>
        <translation>Обновление метаданных пакета не проверено</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12938"/>
        <source>package metadata refresh did not run</source>
        <translation>Обновление метаданных пакета не запускалось</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12940"/>
        <source>package configuration completed — changes were applied</source>
        <translation>Конфигурация пакета завершена — внесены изменения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12941"/>
        <source>nothing to configure</source>
        <translation>Нечего настраивать</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12942"/>
        <source>package configuration failed</source>
        <translation>Конфигурация пакета не удалась</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12943"/>
        <source>package configuration not checked</source>
        <translation>Конфигурация пакета не проверяется</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12944"/>
        <source>package configuration did not run</source>
        <translation>Конфигурация пакета не работает</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12946"/>
        <source>broken dependencies repaired — changes were applied</source>
        <translation>Исправлены неисправные зависимости — внесены изменения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12947"/>
        <source>no broken dependencies</source>
        <translation>Нет сломанных зависимостей</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12948"/>
        <source>broken dependency repair failed</source>
        <translation>Неудачный ремонт сломанной зависимости</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12949"/>
        <source>broken dependency repair not checked</source>
        <translation>Непроверенный ремонт зависимостей</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12950"/>
        <source>broken dependency repair did not run</source>
        <translation>Неисправный ремонт зависимостей не работает</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12952"/>
        <source>packages upgraded — changes were applied</source>
        <translation>пакеты модернизированы — внесены изменения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12953"/>
        <source>no packages to upgrade</source>
        <translation>Нет пакетов для обновления</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12954"/>
        <source>package upgrade failed</source>
        <translation>Обновление пакета провалилось</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12955"/>
        <source>package upgrade not checked</source>
        <translation>Обновление пакета не проверено</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12956"/>
        <source>package upgrade did not run</source>
        <translation>Обновление пакета не запущено</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12958"/>
        <source>DKMS modules rebuilt — changes were applied</source>
        <translation>Модули ДКМС перестроены — внесены изменения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12959"/>
        <source>DKMS modules already current — no changes</source>
        <translation>Модули DKMS уже работают — изменений нет</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12960"/>
        <source>DKMS rebuild failed</source>
        <translation>Восстановление КДМС провалилось</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12961"/>
        <source>DKMS rebuild not checked</source>
        <translation>Восстановление КДМС не проверено</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12962"/>
        <source>DKMS rebuild did not run</source>
        <translation>Восстановление ДКМС не проводилось</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12964"/>
        <source>display manager repaired — changes were applied</source>
        <translation>Ремонт дисплея — внесены изменения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12965"/>
        <source>display manager already correct — no changes</source>
        <translation>Дисплей-менеджер уже исправен — никаких изменений</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12966"/>
        <source>display manager repair failed</source>
        <translation>Ремонт дисплея провалился</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12967"/>
        <source>display manager repair not checked</source>
        <translation>Ремонт дисплея не проверен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12968"/>
        <source>display manager repair did not run</source>
        <translation>Ремонт дисплея не состоялся</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12970"/>
        <source>initramfs rebuilt — images regenerated</source>
        <translation>initramfs реконструирован — изображения регенерированы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12971"/>
        <source>initramfs already current — no changes</source>
        <translation>initramfs уже работает — никаких изменений</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12972"/>
        <source>initramfs rebuild failed</source>
        <translation>Реконструкция initramfs провалилась</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12973"/>
        <source>initramfs rebuild not checked</source>
        <translation>Реконструкция initramfs не проверена</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12974"/>
        <source>initramfs rebuild did not run</source>
        <translation>Реконструкция initramfs не состоялась</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12976"/>
        <source>EFI boot path repaired — changes were applied</source>
        <translation>Загрузочный путь EFI отремонтирован — внесены изменения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12977"/>
        <source>EFI boot path already correct — no changes</source>
        <translation>Путь загрузки EFI уже правильный - никаких изменений</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12978"/>
        <source>EFI boot path repair failed</source>
        <translation>Ремонт загрузочного пути EFI провалился</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12979"/>
        <source>EFI boot path repair not checked</source>
        <translation>Ремонт загрузочного пути EFI не проверен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12980"/>
        <source>EFI boot path repair did not run</source>
        <translation>Ремонт загрузочного пути EFI не состоялся</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12982"/>
        <source>GRUB configuration regenerated — changes were applied</source>
        <translation>Регенерирована конфигурация GRUB — внесены изменения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12983"/>
        <source>GRUB configuration already current — no changes</source>
        <translation>Конфигурация GRUB уже актуальна — никаких изменений</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12984"/>
        <source>GRUB regeneration failed</source>
        <translation>Регенерация GRUB провалилась</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12985"/>
        <source>GRUB regeneration not checked</source>
        <translation>Регенерация GRUB не проверена</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12986"/>
        <source>GRUB regeneration did not run</source>
        <translation>Регенерация GRUB не прошла</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12988"/>
        <source>extlinux configuration regenerated — changes were applied</source>
        <translation>Регенерирована конфигурация extlinux — внесены изменения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12989"/>
        <source>extlinux configuration already current — no changes</source>
        <translation>Конфигурация extlinux уже актуальна — никаких изменений</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12990"/>
        <source>extlinux regeneration failed</source>
        <translation>Регенерация extlinux провалилась</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12991"/>
        <source>extlinux regeneration not checked</source>
        <translation>Регенерация extlinux не проверена</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12992"/>
        <source>extlinux regeneration did not run</source>
        <translation>Регенерация extlinux не прошла</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12994"/>
        <source>file system repaired — changes were applied</source>
        <translation>Ремонт файловой системы — изменения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12995"/>
        <source>no file system errors found — no changes</source>
        <translation>Не найдено ошибок файловой системы — нет изменений</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12996"/>
        <source>file system repair failed</source>
        <translation>Ремонт файловой системы провалился</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12997"/>
        <source>not all filesystems were checked</source>
        <translation>Не все файловые системы были проверены.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="12998"/>
        <source>file system repair did not run</source>
        <translation>Ремонт файловой системы не выполняется</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13000"/>
        <source>boot stack reconciled — changes were applied</source>
        <translation>Загрузочный стек помирился — были применены изменения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13001"/>
        <source>boot stack already current — no changes</source>
        <translation>Загрузочный стек уже существующий - никаких изменений</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13002"/>
        <source>boot stack repair failed</source>
        <translation>Ремонт багажника провалился</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13003"/>
        <source>boot stack not checked</source>
        <translation>Загрузочный стек не проверен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13004"/>
        <source>boot stack repair did not run</source>
        <translation>Ремонт багажника не состоялся</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13006"/>
        <source>validation completed — changes were applied</source>
        <translation>Валидация завершена — внесены изменения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13007"/>
        <source>validation is read-only — no changes</source>
        <translation>Валидация только для чтения - никаких изменений</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13008"/>
        <source>validation failed</source>
        <translation>валидация провалилась</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13009"/>
        <source>validation not checked</source>
        <translation>валидация не проверена</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13010"/>
        <source>validation did not run</source>
        <translation>Проверка не проводилась</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13012"/>
        <source>host boot default updated — changes were applied</source>
        <translation>Обновленный по умолчанию хост-загрузка — изменения применены</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13013"/>
        <source>host boot default already correct — no changes</source>
        <translation>Загрузка хоста по умолчанию уже верна — никаких изменений</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13014"/>
        <source>host boot default update failed</source>
        <translation>Обновление хост-загрузки по умолчанию провалилось</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13015"/>
        <source>host boot default not checked</source>
        <translation>хост-загрузка по умолчанию не проверяется</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13016"/>
        <source>host boot default update did not run</source>
        <translation>Обновление host boot default не работает</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13018"/>
        <source>repair successful — changes were applied</source>
        <translation>Успешный ремонт – изменения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13019"/>
        <source>no repair needed — no changes were detected</source>
        <translation>Ремонт не требуется — никаких изменений не обнаружено.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13020"/>
        <source>repair failed</source>
        <translation>Неудачный ремонт</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13021"/>
        <source>not checked — no repair was attempted</source>
        <translation>Не проверено — ремонт не был предпринят</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13022"/>
        <source>repair did not run</source>
        <translation>Ремонт не проводился</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13348"/>
        <source>Automatic running-host diagnostics regeneration skipped: Host Maintenance is not active.</source>
        <translation>Пропущена регенерация автоматической диагностики бегущего хоста: техническое обслуживание хоста неактивно.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13358"/>
        <source>Regenerating target diagnostics</source>
        <translation>Восстановление целевой диагностики</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13359"/>
        <source>Regenerating running-host diagnostics</source>
        <translation>Регенерация диагностики бегущего хоста</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13364"/>
        <location filename="../src/MainWindow.cpp" line="13860"/>
        <source>Running…</source>
        <translation>Запуск …</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13412"/>
        <location filename="../src/MainWindow.cpp" line="13590"/>
        <source>%1 diagnostic run: %2</source>
        <translation>%1 диагностический запуск: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13413"/>
        <location filename="../src/MainWindow.cpp" line="13590"/>
        <location filename="../src/MainWindow.cpp" line="13908"/>
        <source>Target</source>
        <translation>Цель</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13413"/>
        <location filename="../src/MainWindow.cpp" line="13590"/>
        <location filename="../src/MainWindow.cpp" line="13908"/>
        <source>Host</source>
        <translation>ведущий</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13453"/>
        <source>A privileged operation is already running; the diagnostic was not started. Try again when it finishes.</source>
        <translation>Привилегированная операция уже запущена, диагностика не начата. Попробуйте еще раз, когда закончите.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13464"/>
        <location filename="../src/MainWindow.cpp" line="13824"/>
        <source>Host diagnostic refused: %1</source>
        <translation>Отказ в диагностике хозяина: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13468"/>
        <location filename="../src/MainWindow.cpp" line="13828"/>
        <source>Host Maintenance required</source>
        <translation>Обслуживание хоста необходимо</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13499"/>
        <source>Running diagnostic: %1</source>
        <translation>Диагностика: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13603"/>
        <location filename="../src/MainWindow.cpp" line="13838"/>
        <source>No repair target selected</source>
        <translation>Не выбрана цель ремонта</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13611"/>
        <source>Read target configuration</source>
        <translation>Читать целевую конфигурацию</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13615"/>
        <source>Configuration unavailable</source>
        <translation>Конфигурация недоступна</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13626"/>
        <source>Edit target %1</source>
        <translation>Редактировать цель %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13629"/>
        <source>Edit this target file through the guarded administrator helper. A successful save invalidates cached diagnostics; rerun diagnostics before repair. Generated files such as /boot/grub/grub.cfg may be replaced by the next bootloader update.</source>
        <translation>Редактируйте этот целевой файл через охраняемого помощника администратора. Успешное сохранение аннулирует кэшированную диагностику; повторите диагностику перед ремонтом. Сгенерированные файлы, такие как /boot/grub/grub.cfg, могут быть заменены следующим обновлением загрузчика.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13641"/>
        <source>Save Target File</source>
        <translation>Сохранить целевой файл</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13648"/>
        <location filename="../src/MainWindow.cpp" line="13690"/>
        <source>Write target configuration</source>
        <translation>Записать целевую конфигурацию</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13649"/>
        <source>Write the edited contents to %1? This modifies the repair target and invalidates cached diagnostics.</source>
        <translation>Написать отредактированный контент на %1? Это изменяет цель ремонта и аннулирует кэшированную диагностику.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13660"/>
        <source>Configuration write refused</source>
        <translation>Конфигурация письма отказала</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13661"/>
        <source>The edited content contains NUL bytes; the guarded write refuses it.</source>
        <translation>Отредактированный контент содержит байты NUL; охраняемая запись отказывается от него.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13666"/>
        <source>File too large</source>
        <translation>Файл слишком большой</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13667"/>
        <source>The edited file is larger than 1 MiB; the guarded write refuses it. Edit the file from a console instead.</source>
        <translation>Отредактированный файл больше 1 МиБ; охраняемая запись отказывается от него. Вместо этого отредактируйте файл с консоли.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13672"/>
        <source>Configuration write unavailable</source>
        <translation>Конфигурация Write Unavailable</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13673"/>
        <source>The edited content could not be written to a private temporary file in the application log directory; the write was not started.</source>
        <translation>Отредактированный контент не мог быть записан в частный временный файл в каталоге журнала приложений; запись не была начата.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13701"/>
        <source>Configuration write failed</source>
        <translation>Конфигурация писать не удалось</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13721"/>
        <source>Configuration saved</source>
        <translation>Конфигурация сохранена</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13805"/>
        <source>The full diagnostic report is already running; results appear when it completes.</source>
        <translation>Полный диагностический отчет уже запущен; результаты появляются, когда он завершается.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13809"/>
        <source>Full diagnostics will run after the current diagnostic finishes.</source>
        <translation>Полная диагностика будет проводиться после текущей диагностики.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13839"/>
        <source>Select a repair target in Systems, or enter Host Maintenance for running-host diagnostics.</source>
        <translation>Выберите цель ремонта в системах или введите хост-поддержку для диагностики бегущих хостов.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13845"/>
        <source>Regenerating diagnostics automatically</source>
        <translation>Автоматическая регенерация диагностики</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13846"/>
        <source>Running all diagnostics</source>
        <translation>Запуск всех диагностических</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13910"/>
        <source>Starting all available read-only diagnostics (%1 scope).</source>
        <translation>Запуск всей доступной диагностики только для чтения (%1).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13931"/>
        <source>All available read-only diagnostic summaries generated and cached by diagnostic.</source>
        <translation>Все доступные диагностические резюме, созданные и кэшированные диагностикой.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="13932"/>
        <source>Run All diagnostics failed; failed output was not cached as successful diagnostic data.</source>
        <translation>Запуск Все диагностики не удался; неудавшийся вывод не был кэширован как успешные диагностические данные.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14040"/>
        <source>Regenerating diagnostics</source>
        <translation>Регенерирующая диагностика</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14050"/>
        <source>Regenerating read-only diagnostics…</source>
        <translation>Восстановление диагностики только для чтения …</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14072"/>
        <source>Read-only diagnostics regenerated automatically.</source>
        <translation>Диагностика только для чтения восстанавливается автоматически.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14073"/>
        <source>Automatic diagnostics regeneration did not complete; repair actions remain disabled until diagnostics are regenerated.</source>
        <translation>Регенерация автоматической диагностики не завершена, ремонтные работы остаются отключенными до восстановления диагностики.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14112"/>
        <source>Diagnostic results copied</source>
        <translation>Диагностические результаты скопированы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14137"/>
        <source>Unable to save diagnostics</source>
        <translation>Не удается сохранить диагностику</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14143"/>
        <source>Diagnostics saved to %1</source>
        <translation>Диагностика сохранена в %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14152"/>
        <source>Save application log</source>
        <translation>Сохранить журнал приложений</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14161"/>
        <source>Unable to save log</source>
        <translation>Не удается сохранить лог</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14169"/>
        <source>Log saved to %1</source>
        <translation>Лог сохранен для %1</translation>
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
        <translation>Нынешняя сессия</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14942"/>
        <source>Current session — not started</source>
        <translation>Нынешняя сессия не началась</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14944"/>
        <source> — %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14951"/>
        <source>Entries from this window. A fresh launch starts empty; a session file is created once a running-host or repair-target scope is identified.</source>
        <translation>Вход из этого окна. Свежий запуск начинается пустым; файл сеанса создается, как только идентифицируется область выполнения или цели восстановления.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="14996"/>
        <source>Viewing prior session log %1 — generated %2 — read only</source>
        <translation>Просмотр журнала предыдущей сессии %1 — сгенерированный %2 — читать только</translation>
    </message>
    <message>
        <source>Clear the current session log %1? The active file is truncated and cannot be restored. Prior session files are not touched.</source>
        <translation type="vanished">Очистить журнал текущей сессии %1? Активный файл усечен и не может быть восстановлен. Файлы предыдущих сеансов не затрагиваются.</translation>
    </message>
    <message>
        <source>No session log file exists yet because no scope has been identified. Clear the in-memory entries only?</source>
        <translation type="vanished">Файл журнала сеанса еще не существует, поскольку не был идентифицирован объем. Очистить только записи в памяти?</translation>
    </message>
    <message>
        <source>Clear session log</source>
        <translation type="vanished">Чистый журнал сессии</translation>
    </message>
    <message>
        <source>Unable to truncate %1.</source>
        <translation type="vanished">Невозможно усечение %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15034"/>
        <source>Note:</source>
        <translation>Примечание:</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15070"/>
        <location filename="../src/MainWindow.cpp" line="15077"/>
        <source>Delete session log</source>
        <translation>Исключить журнал сессии</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15071"/>
        <source>Delete %1 permanently? This cannot be undone.</source>
        <translation>Удалить %1 навсегда Это нельзя отменить.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15078"/>
        <source>Unable to delete %1.</source>
        <translation>Невозможно удалить %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="15890"/>
        <source>(no matching entries for this filter)</source>
        <translation>(нет соответствующих записей для этого фильтра)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16213"/>
        <source>Boot Bitch must run from a different booted Linux environment than the system being repaired. Use a Linux live USB or another Linux installation on a different physical drive. The running host is protected from ordinary repair-target selection, but it can be explicitly selected through Host Maintenance for guarded native diagnostics and supported maintenance stages. Diagnostics follow the Systems page: the selected repair drive while Host Maintenance is off, or the protected running host while it is active. The first root action requests administrator authorization once for this Boot Bitch window; File → Lock Administrator Session ends that helper session immediately.</source>
        <translation>Boot Bitch должен работать в другой загруженной среде Linux, чем отремонтированная система. Используйте Linux Live USB или другую установку Linux на другом физическом диске. Бегущий хост защищен от обычного выбора цели ремонта, но он может быть явно выбран через техническое обслуживание хоста для защищенной нативной диагностики и поддерживаемых этапов обслуживания. Диагностика следует за страницей «Системы»: выбранный накопитель для ремонта во время обслуживания хоста отключен или защищенный работающий хост во время его активности. Первое действие root требует авторизации администратора для этого окна Boot Bitch; Сессия администратора блокировки файлов → немедленно завершает эту сессию помощника.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16228"/>
        <source>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;</source>
        <translation>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16229"/>
        <source>&lt;p&gt;&lt;b&gt;Developer:&lt;/b&gt; CaptainMorgan12&lt;/p&gt;&lt;p&gt;A native Qt 6 Linux recovery and boot-repair utility.&lt;/p&gt;</source>
        <translation>&lt;p&gt;&lt;b&gt;Разработчик:&lt;/b&gt; CaptainMorgan12&lt;/p&gt;&lt;p&gt;Нативная утилита восстановления и восстановления Qt 6 Linux.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16231"/>
        <source>&lt;p&gt;&lt;b&gt;Guarded repair mode:&lt;/b&gt; read-only diagnostics can inspect either the protected Running Host or an explicitly selected repair drive. Debian/Ubuntu-family repairs can run through a privileged helper after confirmation; Host Maintenance enables the same supported stages natively on the active system after repeating the host identity and boot-mount checks.&lt;/p&gt;</source>
        <translation>&lt;p&gt;&lt;b&gt;Охраняемый режим ремонта: &lt;/b&gt; Диагностика только для чтения может проверить защищенный Running Host или явно выбранный привод для ремонта. Ремонт семейства Debian/Ubuntu может выполняться через привилегированного помощника после подтверждения; техническое обслуживание хоста позволяет выполнять те же поддерживаемые этапы в активной системе после повторения проверки личности хоста и загрузки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16232"/>
        <source>&lt;p&gt;Arch-family systems expose backend profiling and guarded package, initramfs, GRUB and conventional EFI repairs when their transaction-specific preflights pass. Fedora/RPM-family systems expose guarded dnf package transactions, dracut initramfs rebuilds, GRUB2 configuration and boot-code repair, and the systemd GDM display path from the same probe evidence; other RPM-family systems remain diagnostics-only until their transaction backend is implemented.&lt;/p&gt;</source>
        <translation>&lt;p&gt;Семейные системы обнажают бэкэнд-профилирование и защищенный пакет, initramfs, GRUB и обычный EFI ремонт, когда проходят их предварительные полеты. Системы семейства Fedora/RPM выявляют защищенные транзакции пакетов dnf, dracut initramfs восстанавливает, конфигурирует GRUB2 и восстанавливает загрузочный код, а также систематизированный путь отображения GDM из тех же доказательств зонда; другие системы семейства RPM остаются диагностическими только до тех пор, пока не будет реализован их бэкэнд транзакций.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16233"/>
        <source>&lt;p&gt;The first privileged action authorizes one narrow helper session for the current Boot Bitch window while the Qt GUI remains unprivileged. It can be ended at any time from File → Lock Administrator Session.&lt;/p&gt;</source>
        <translation>Первое привилегированное действие разрешает один узкий сеанс помощника для текущего окна Boot Bitch, в то время как Qt GUI остается непривилегированным. Он может быть завершен в любое время с сеанса администратора блокировки файлов →.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="16234"/>
        <source>&lt;p&gt;LUKS target unlock, verified bidirectional File Copy, read-only host/repair diagnostics, transactional Btrfs snapshot rollback, offline graphical login recovery, distribution-aware EFI/UKI repair and boot-stack reconciliation are enabled through the guarded helper. Snapshot rollback preserves the previous @ and automatically restores it if critical post-switch reconciliation fails.&lt;/p&gt;</source>
        <translation>&lt;p&gt;Разблокировка цели LUKS, верифицированная двунаправленная копия файла, диагностика хоста / ремонта только для чтения, откат снимка транзакционного Btrfs, восстановление автономного графического входа в систему, исправление дистрибутива EFI / UKI и сверка загрузочного стека включены через охраняемого помощника. Откат снимка сохраняет предыдущий @ и автоматически восстанавливает его, если критическое выверка после переключения не удается.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17067"/>
        <location filename="../src/MainWindow.cpp" line="18763"/>
        <source>Restore detected graphical login manager</source>
        <translation>Восстановление обнаруженного графического менеджера входа</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17069"/>
        <source>Reinstall EFI bootloader</source>
        <translation>Переустановить загрузчик EFI</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17083"/>
        <location filename="../src/MainWindow.cpp" line="18197"/>
        <source>Reinstall Alpine GRUB EFI loader</source>
        <translation>Reinstall Alpine GRUB EFI погрузчик</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17085"/>
        <location filename="../src/MainWindow.cpp" line="18200"/>
        <source>Regenerate Alpine GRUB configuration</source>
        <translation>Восстановление конфигурации Alpine GRUB</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17095"/>
        <location filename="../src/MainWindow.cpp" line="18170"/>
        <source>Update GRUB2 configuration</source>
        <translation>Обновление конфигурации GRUB2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17097"/>
        <location filename="../src/MainWindow.cpp" line="18173"/>
        <source>Reinstall GRUB2 bootloader</source>
        <translation>Reinstall загрузчик GRUB2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17104"/>
        <location filename="../src/MainWindow.cpp" line="18244"/>
        <source>Rebuild initramfs (dracut)</source>
        <translation>Восстановление initramfs (dracut)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17132"/>
        <source>%1. %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17134"/>
        <source>Execution order %1: %2</source>
        <translation>Исполнительный приказ %1: %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17140"/>
        <source>No Full Repair stages selected — use Configure Plan… or Settings.</source>
        <translation>Не выбраны этапы полного ремонта - используйте конфигурацию Plan… или настройки.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17145"/>
        <source>1 stage selected</source>
        <translation>1 этап выбранный</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17146"/>
        <source>%1 stages selected</source>
        <translation>Выбранные стадии %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17178"/>
        <source>No repair stages are selected. Use Configure Plan… to choose the stages Full Repair will run.</source>
        <translation>Стадии ремонта не выбираются. Используйте Configure Plan…, чтобы выбрать этапы полного ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17179"/>
        <source>No selected stages are available. %1</source>
        <translation>Отобранные этапы отсутствуют. %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17186"/>
        <source> Please regenerate diagnostics before starting another repair.</source>
        <translation>Пожалуйста, восстановите диагностику перед началом ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17189"/>
        <source>Ready: required cached read-only diagnostics are available for the selected stages. Review them in Diagnostics or Logs before confirming.</source>
        <translation>Готовы: для выбранных этапов доступна необходимая кэшированная диагностика только для чтения. Просмотрите их в диагностике или журналах, прежде чем подтвердить.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17195"/>
        <location filename="../src/MainWindow.cpp" line="18845"/>
        <source>No Full Repair stages are selected.</source>
        <translation>Полноценный ремонт не подбирается.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17201"/>
        <source>Runs the selected stages using the cached read-only diagnostic evidence after privilege confirmation.</source>
        <translation>Запускает выбранные этапы с использованием кэшированных диагностических данных только для чтения после подтверждения привилегий.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17214"/>
        <source>Disabled in Settings</source>
        <translation>Инвалиды в настройках</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17221"/>
        <source>Enabled in Settings</source>
        <translation>Включено в настройках</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17222"/>
        <source>Disabled in Settings — enable it to include this stage</source>
        <translation>Инвалиды в настройках — позволяют включить этот этап</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17226"/>
        <source>Always preflight</source>
        <translation>Всегда предполетный</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17250"/>
        <source>Manual recovery tool</source>
        <translation>Ручной инструмент восстановления</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17255"/>
        <source>Unavailable: %1</source>
        <translation>Недоступно: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17283"/>
        <source>Select a tool to review its workflow.</source>
        <translation>Выберите инструмент для просмотра его рабочего процесса.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17306"/>
        <source>Check the running host or selected repair system&apos;s mounts, filesystem metadata, boot files, mapper consistency and dependency readiness before any repair action. This is an independent safety preflight rather than an optional Full Repair stage.</source>
        <translation>Проверьте работающий хост или выбранные установки системы ремонта, метаданные файловой системы, загрузочные файлы, согласованность картографа и готовность к зависимости перед любым действием по ремонту. Это самостоятельный предполет безопасности, а не факультативный этап полного ремонта.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17309"/>
        <source>Validate</source>
        <translation>подтверждать</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17311"/>
        <source>Full Repair: automatic safety preflight</source>
        <translation>Полный ремонт: автоматическая предполетная безопасность</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17314"/>
        <source>Complete interrupted dpkg package configuration in the running host or selected repair system. This is the same stage controlled by Settings → Full Repair plan → Complete interrupted package configuration, but it can also be run independently here.</source>
        <translation>Полная прерываемая конфигурация пакета dpkg в запущенном хосте или выбранной системе ремонта. Эта же стадия управляется планом полного ремонта Settings → Full Repair Plan → Complete с прерванной конфигурацией пакета, но она также может быть запущена самостоятельно.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17316"/>
        <source>Complete Configuration</source>
        <translation>Полная конфигурация</translation>
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
        <translation>Полный ремонт: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17321"/>
        <source>Repair package dependencies in the running host or selected repair system after the mandatory safety preflight. Debian/Ubuntu uses APT; Arch uses one sandbox-preflighted full pacman transaction; Alpine uses apk fix with a simulation first. This maps directly to Settings → Repair broken package dependencies.</source>
        <translation>Зависимости пакета ремонта в запущенном хосте или выбранной системе ремонта после обязательного предполетного полета. Debian/Ubuntu использует APT; Arch использует одну предполетную транзакцию pacman; Alpine сначала использует исправление apk с симуляцией. Это отображается непосредственно в настройках → Ремонт сломанных зависимостей пакета.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17324"/>
        <source> Fedora/RPM systems use rpm verification to find missing or corrupt package files and restore them with one simulated dnf reinstall transaction; dependency problems reported by dnf check are shown but never auto-removed.</source>
        <translation>Системы Fedora/RPM используют проверку rpm для поиска отсутствующих или поврежденных файлов пакетов и восстановления их с помощью одной смоделированной транзакции переустановки dnf; проблемы с зависимостью, о которых сообщает проверка dnf, показаны, но никогда не удаляются автоматически.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17327"/>
        <source>Repair Dependencies</source>
        <translation>Ремонт зависимостей</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17332"/>
        <source>Refresh Debian/Ubuntu APT metadata in the running host or selected repair system without upgrading installed packages. Arch and Alpine deliberately refuse a partial metadata-only transaction; their guarded upgrade refreshes the package index itself. This maps directly to Settings → Refresh package metadata.</source>
        <translation>Обновите метаданные Debian/Ubuntu APT в запущенном хосте или выбранной системе ремонта без обновления установленных пакетов. Arch и Alpine сознательно отказываются от частичной транзакции только с метаданными; их защищенное обновление обновляет сам индекс пакетов. Эта карта отображается непосредственно в метаданных пакета Settings → Refresh.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17335"/>
        <source> Fedora refreshes dnf metadata (dnf makecache) as its standalone metadata stage; the cache write is always reported as changed.</source>
        <translation>Fedora обновляет dnf метаданные (dnf makecache) в качестве отдельной стадии метаданных; запись кэша всегда сообщается как измененная.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17338"/>
        <source>Refresh Metadata</source>
        <translation>Обновление метаданных</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17343"/>
        <source>Simulate the distribution&apos;s package transaction first, inspect proposed removals, then apply a safe upgrade. Debian/Ubuntu chooses an APT mode; Arch runs one full pacman transaction; Alpine runs one guarded apk upgrade transaction. This maps directly to Settings → Upgrade installed packages.</source>
        <translation>Сначала имитируйте транзакцию пакета дистрибуции, проверьте предлагаемые удаления, а затем примените безопасное обновление. Debian/Ubuntu выбирает режим APT; Arch запускает одну полную транзакцию pacman; Alpine запускает одну защищенную транзакцию обновления apk. Эта карта отображается непосредственно в установленных пакетах Settings → Upgrade.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17346"/>
        <source> Fedora runs one guarded dnf upgrade transaction: the simulated transaction must be removal- and downgrade-free, signature-checked and bounded before the exact same command is applied.</source>
        <translation>Fedora запускает одну защищенную транзакцию обновления dnf: смоделированная транзакция должна быть удалена и понижена, проверена подписью и ограничена до того, как будет применена та же команда.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17349"/>
        <source>Simulate and Upgrade</source>
        <translation>Моделирование и обновление</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17354"/>
        <source>Rebuild out-of-tree kernel modules for kernels installed in the running host or selected repair system. The helper refuses this action when DKMS is not installed in the selected system.</source>
        <translation>Восстановление модулей ядра из дерева для ядер, установленных в работающем хосте или выбранной системе ремонта. Помощник отказывается от этого действия, когда DKMS не установлена в выбранной системе.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17356"/>
        <location filename="../src/MainWindow.cpp" line="18759"/>
        <source>Rebuild DKMS</source>
        <translation>Восстановление DKMS</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17361"/>
        <source>Restore the display manager identified from the running host or selected repair system&apos;s boot evidence and configuration (for example SDDM, GDM/GDM3, LightDM, or another systemd manager), set graphical.target as the default, and repair display-manager.service. </source>
        <translation>Восстановление дисплея дисплея, идентифицированного из запущенного хоста или выбранной системы восстановления загрузочных данных и конфигурации (например, SDDM, GDM/GDM3, LightDM или другого системного менеджера), задается графическим. Цель как по умолчанию, так и ремонт дисплея-менеджера. обслуживание.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17364"/>
        <source>The detected backend is GDM (Fedora naming); its configuration lives in /etc/gdm/custom.conf. </source>
        <translation>Обнаруженный бэкэнд - GDM (название Fedora); его конфигурация живет в /etc/gdm/custom.conf.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17367"/>
        <source>The detected backend is GDM3; its configuration lives in /etc/gdm3/daemon.conf. </source>
        <translation>Обнаруженный бэкэнд - GDM3; его конфигурация живет в /etc/gdm3/daemon.conf.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17370"/>
        <location filename="../src/MainWindow.cpp" line="17407"/>
        <source>The detected backend is %1. </source>
        <translation>Обнаруженный бэкэнд — %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17372"/>
        <source>When OpenRC is the detected service manager, the same action restores the detected manager&apos;s default runlevel symlink without starting it. Boot Bitch deliberately does not start a graphical session inside a repair chroot or host helper; use Diagnostics to inspect installed packages, service configuration, and recent boot/journal evidence first when graphical boot fails.</source>
        <translation>Когда OpenRC является обнаруженным диспетчером службы, то это же действие восстанавливает обнаруженный менеджер по умолчанию, не запуская его. Boot Bitch намеренно не запускает графическую сессию внутри ремонтного сервера или помощника хоста; используйте Диагностику для проверки установленных пакетов, конфигурации службы и недавних доказательств загрузки / журнала, когда графическая загрузка выходит из строя.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17375"/>
        <source>Restore Graphical Login</source>
        <translation>Восстановить графический логин</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17380"/>
        <source>Rebuild initramfs images for the running host or selected repair system only after mapper and crypttab consistency checks pass. The helper uses update-initramfs on Debian/Ubuntu, transaction-specific mkinitcpio trials on Arch, and mkinitfs trials on Alpine.</source>
        <translation>Восстановите изображения initramfs для работающего хоста или выбранной системы ремонта только после прохождения проверки согласованности картографа и crypttab. Помощник использует update-initramfs на Debian/Ubuntu, mkinitcpio на Arch и mkinitfs на Alpine.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17383"/>
        <source> The detected dracut backend pairs every installed kernel with its /boot/vmlinuz-&lt;kver&gt; and /boot/initramfs-&lt;kver&gt;.img, runs a trial build to a temporary path first, verifies the image with lsinitrd, and backs up each image before the apply so a failed verification restores the previous initramfs.</source>
        <translation>Обнаруженный бэкэнд dracut объединяет каждое установленное ядро с его /boot/vmlinuz-&lt;kver&gt; и /boot/initramfs-&lt;kver&gt;.img, сначала запускает пробную сборку на временный путь, проверяет изображение с помощью lsinitrd и резервирует каждое изображение перед применением, поэтому неудачная проверка восстанавливает предыдущий initramfs.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17386"/>
        <source>Rebuild Initramfs</source>
        <translation>Восстановление Initramfs</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17392"/>
        <source>Repair the running host or selected repair system&apos;s boot path with the detected backend. The stage uses the guarded installer or vendor builder for the detected layout, restores one verified default-loader entry when a guarded installer only writes files, and preserves every other ESP&apos;s firmware entries and BootOrder. </source>
        <translation>Отремонтируйте запущенный хост или выбранный путь загрузки системы ремонта с обнаруженным бэкэндом. Этап использует защищенный установщик или конструктор поставщиков для обнаруженного макета, восстанавливает одну верифицированную запись по умолчанию, когда защищенный установщик пишет только файлы, и сохраняет все другие записи прошивки ESP и BootOrder.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17395"/>
        <source>The detected vendor UKI layout (for example the TUXEDO create_boot_uki_base.sh / TUX.EFI builder) is rebuilt or re-registered through its official builder. </source>
        <translation>Обнаруженная вендором компоновка UKI (например, TUXEDO create boot uki base.sh/TUX.EFI builder) перестраивается или перерегистрируется через своего официального конструктора.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17398"/>
        <source>The detected Fedora GRUB2/BLS backend keeps /boot/grub2/grub.cfg and the BLS entries intact, repairs the boot code on a BIOS layout, and leaves the firmware default to the separate Make Default action. </source>
        <translation>Обнаруженный бэкэнд Fedora GRUB2/BLS сохраняет /boot/grub2/grub.cfg и записи BLS неповрежденными, восстанавливает загрузочный код на макете BIOS и оставляет по умолчанию прошивки отдельное действие Make Default.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17401"/>
        <source>The detected syslinux/extlinux layout is repaired through its configuration and boot code, never through EFI. </source>
        <translation>Обнаруженная компоновка syslinux/extlinux восстанавливается через конфигурацию и загрузочный код, а не через EFI.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17404"/>
        <source>The detected GRUB EFI layout uses a guarded grub-install on the validated ESP and restores one verified vendor-loader firmware entry if the guarded installer only writes files. </source>
        <translation>Обнаруженный макет GRUB EFI использует защищенный grub-install на проверенном ESP и восстанавливает одну верифицированную запись прошивки поставщика-загрузчика, если защищенный установщик пишет только файлы.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17409"/>
        <source>On Alpine UEFI GRUB systems it backs up the ESP loader files, runs grub-install --target=x86_64-efi --bootloader-id=&lt;detected&gt; --boot-directory=/boot --no-nvram, refreshes the EFI/boot/bootx64.efi fallback copy when the layout had one, and reconciles one firmware entry for the detected loader; a failed install restores the ESP backup. An Alpine EFI-stub-only system is detected and reported, and the stage reconciles captured firmware entries only without synthesising kernel command lines. Afterward, decoded entries on each maintained ESP retain their distribution/vendor label and receive that drive&apos;s model once; an existing model name is not duplicated. Unrelated entries on other disks are never removed.</source>
        <translation>В системах Alpine UEFI GRUB выполняется резервное копирование файлов загрузчика ESP, выполняется grub-install -target=x86 64-efi -bootloader-id=&lt;detected&gt; -boot-directory=/boot-no-nvram, обновляется резервная копия EFI/boot/bootx64.efi, когда макет имел один, и согласовывается одна запись прошивки для обнаруженного загрузчика; неудавшаяся установка восстанавливает резервную копию ESP. Обнаружена и зарегистрирована только альпийская система EFI-stub, и этап согласовывает захваченные записи прошивки только без синтеза командных строк ядра. После этого декодированные записи на каждом поддерживаемом ESP сохраняют свою ярлык дистрибуции / поставщика и получают модель этого диска один раз; существующее имя модели не дублируется. Несвязанные записи на других дисках никогда не удаляются.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17411"/>
        <source>Repair EFI / UKI</source>
        <translation>Ремонт EFI/UKI</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17416"/>
        <source>GRUB2 configuration</source>
        <translation>Конфигурация GRUB2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17417"/>
        <source>Regenerate the running host or selected repair system&apos;s GRUB2 configuration after the mandatory safety preflight. Fedora ships grub2-mkconfig and stores /boot/grub2/grub.cfg with /boot/grub2/grubenv and BLS entries under /boot/loader/entries; the stage regenerates the configuration with --no-grubenv-update, an entry-preserving guard and a rollback when a previous menu entry or BLS entry would be lost. When the read-only boot-code probe finds the MBR or BIOS boot partition broken, the same stage performs a guarded Reinstall GRUB2 bootloader (grub2-install --target=i386-pc --boot-directory=/boot) with MBR and bios_grub backup and rollback; a healthy boot code stays config-only. This never writes firmware NVRAM and does not reinstall EFI loader files; use EFI / UKI bootloader when the firmware loader itself needs repair.</source>
        <translation>Восстановите конфигурацию GRUB2 запущенного хоста или выбранной системы ремонта после обязательного предварительного полета. Fedora отправляет grub2-mkconfig и хранит /boot/grub2/grub.cfg с записями /boot/grub2/grubenv и BLS в /boot/loader/entries; этап восстанавливает конфигурацию с -no-grubenv-update, охраной для сохранения входа и откатом, когда предыдущая запись меню или запись BLS будет потеряна. Когда зонд только для чтения загрузочного кода обнаруживает, что раздел загрузки MBR или BIOS сломан, на той же стадии выполняется защищенный загрузчик Reinstall GRUB2 (grub2-install -target=i386-pc -boot-directory=/boot) с резервным копированием и откатом MBR и bios grub; здоровый загрузочный код остается только конфигурацией. Это никогда не записывает прошивку NVRAM и не переустанавливает файлы загрузчика EFI; используйте загрузчик EFI / UKI, когда сам загрузчик прошивки нуждается в ремонте.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17421"/>
        <source>Regenerate GRUB2</source>
        <translation>Регенерировать RUB2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17424"/>
        <source>Regenerate the running host or selected repair system&apos;s GRUB menu/configuration after the mandatory safety preflight. Debian/Ubuntu uses update-grub; Arch and Alpine use grub-mkconfig with an isolated trial output, an entry-preserving guard and a rollback when a previous menu entry would be lost. This does not reinstall EFI loader files; use EFI / UKI bootloader when the firmware loader itself needs repair.</source>
        <translation>Восстановите меню/конфигурацию GRUB запущенного хоста или выбранной системы ремонта после обязательного предварительного полета. Debian/Ubuntu использует update-grub; Arch и Alpine используют grub-mkconfig с изолированным пробным выходом, защитой для сохранения входа и откатом, когда предыдущая запись меню будет потеряна. Это не переустанавливает файлы загрузчика EFI; используйте загрузчик EFI / UKI, когда загрузчик прошивки сам нуждается в ремонте.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17426"/>
        <source>Regenerate GRUB</source>
        <translation>Регенерировать GRUB</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17432"/>
        <source>Regenerate the running host or selected repair system&apos;s extlinux bootloader configuration after the mandatory safety preflight. The detected syslinux/extlinux backend uses update-extlinux with an entry-preserving guard and a boot-artifact backup; existing boot entries are never dropped. This regenerates the configuration only and does not reinstall bootloader files.</source>
        <translation>Восстановите конфигурацию загрузчика extlinux для запуска или выбранной системы ремонта после обязательного предварительного полета. Обнаруженный бэкэнд syslinux / extlinux использует обновление-extlinux с защитой для сохранения входа и резервной копией загрузочного артефакта; существующие загрузочные записи никогда не сбрасываются. Это восстанавливает только конфигурацию и не переустанавливает файлы загрузчика.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17434"/>
        <source>Regenerate extlinux</source>
        <translation>Регенерировать extlinux</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17439"/>
        <source>Run a read-only file system check for the selected system&apos;s root, /boot, ESP and /home filesystems, then repair only the devices that report errors. ext2/3/4 uses e2fsck, XFS xfs_repair, Btrfs check/rescue/scrub, FAT fsck.fat, exFAT fsck.exfat, NTFS ntfsfix (limited Linux-side repair; Windows chkdsk is still required), F2FS fsck.f2fs (repair only, no read-only check), JFS jfs_fsck, ReiserFS reiserfsck and ZFS zpool. Offline tools refuse mounted filesystems; btrfs scrub and zpool scrub are online modes, and btrfs check --repair requires a separate backup confirmation because upstream flags it as dangerous. The running host root is never repaired offline, and unsupported filesystems are reported rather than guessed about. This maps directly to Settings → Repair file system errors (read-only check first).</source>
        <translation>Запустите проверку файловой системы только для чтения для корня выбранной системы, /boot, ESP и /home файловых систем, а затем отремонтируйте только устройства, которые сообщают об ошибках. ext2/3/4 использует e2fsck, XFS xfs repair, Btrfs check/rescue/scrub, FAT fsck.fat, exFAT fsck.exfat, NTFS ntfsfix (ограниченный ремонт на стороне Linux; Windows chkdsk по-прежнему требуется), F2FS fsck.f2fs (только ремонт, нет проверки только для чтения), JFS jfs fsck, ReiserFS reiserfsck и ZFS zpool. Офлайн-инструменты отказываются от установленных файловых систем; btrfs scrub и zpool scrub - это онлайн-режимы, а исправление требует отдельного подтверждения резервного копирования, потому что выше по течению это опасно. Бегущий корень хоста никогда не восстанавливается в автономном режиме, и неподдерживаемые файловые системы сообщаются, а не угадываются. Это отображается непосредственно в настройках исправления ошибок файловой системы → (проверка только для чтения).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17449"/>
        <source>Check File Systems</source>
        <translation>Проверьте файловые системы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17454"/>
        <source>Reconcile a repaired or restored root with its boot artifacts: validate mapper/crypttab, rebuild installed-kernel initramfs images, repair the detected bootloader path (the vendor UKI builder when that layout is present, GRUB EFI, extlinux or Fedora BLS), reconcile one canonical default destination per purpose, and regenerate the detected bootloader configuration. </source>
        <translation>Согласуйте отремонтированный или восстановленный корень с его загрузочными артефактами: валидируйте картограф/криптаб, восстанавливайте изображения установленного ядра initramfs, ремонтируйте обнаруженный путь загрузчика (разработчик UKI при наличии этой компоновки, GRUB EFI, extlinux или Fedora BLS), согласовывайте одно каноническое назначение по умолчанию для цели и восстанавливайте обнаруженную конфигурацию загрузчика.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17457"/>
        <source>On a Fedora BIOS target the same reconciliation validates mapper/crypttab, rebuilds the dracut initramfs images and regenerates the GRUB2 configuration (config-only; the guarded bootloader reinstall stays in the GRUB stage). </source>
        <translation>На мишени Fedora BIOS та же самая сверка проверяет Mapper/crypttab, восстанавливает изображения dracut initramfs и регенерирует конфигурацию GRUB2 (только конфигурация; переустановка защищенного загрузчика остается на стадии GRUB).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17460"/>
        <source>When the EFI / UKI bootloader repair already ran for the same system in this session, this action reuses it: the duplicate bootloader rebuild and GRUB regeneration are skipped and logged, while mapper/crypttab validation and initramfs reconciliation still run. The boot tools are independent: EFI / UKI bootloader repair, GRUB or extlinux configuration, boot-stack reconciliation and Make Default can be run in any order, and a later action re-verifies what an earlier one changed and reports its own result instead of replacing it. This is the focused recovery action for a root/boot mismatch after a partial update or snapshot restore; it does not delete kernels or unrelated boot entries.</source>
        <translation>Когда загрузчик EFI / UKI уже работал для одной и той же системы в этой сессии, это действие повторно использует его: дубликат загрузчика восстановления и GRUB регенерации пропущены и зарегистрированы, в то время как валидация картографа / шифрования и initramfs сверка все еще работает. Загрузочные инструменты независимы: EFI / UKI загрузчик ремонт, GRUB или extlinux конфигурация, выверка загрузочного стека и по умолчанию Make Default могут быть запущены в любом порядке, а более позднее действие перепроверяет, что изменилось ранее, и сообщает о своем собственном результате вместо его замены. Это целенаправленное действие восстановления для несоответствия корня / загрузки после частичного обновления или восстановления снимка; оно не удаляет ядра или несвязанные загрузочные записи.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17464"/>
        <source>Reconcile Boot Stack</source>
        <translation>Обсуждение Boot Stack</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17466"/>
        <source>Full Repair plan: Manual recovery tool</source>
        <translation>Полный план ремонта: инструмент ручного восстановления</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17468"/>
        <source>Unknown repair tool</source>
        <translation>Неизвестный ремонтный инструмент</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17469"/>
        <source>No repair workflow is registered for this item.</source>
        <translation>Ремонтные работы по данному пункту не регистрируются.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17470"/>
        <source>Unavailable</source>
        <translation>Недоступный</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17472"/>
        <source>Full Repair plan: unavailable</source>
        <translation>Полный план ремонта: недоступен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17496"/>
        <source>
Reuse: the bootloader repair from this session will be reused (the duplicate bootloader rebuild and GRUB regeneration are skipped).</source>
        <translation>
Повторное использование: восстановление загрузчика с этого сеанса будет повторно использовано (пропущена реконструкция дубликата загрузчика и регенерация GRUB).</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17500"/>
        <source>
Unavailable: %1</source>
        <translation>
Недоступно: %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17509"/>
        <source>Reuses the bootloader repair from this session: the second UKI rebuild and duplicate GRUB regeneration are skipped when the earlier EFI / UKI stage already rebuilt and verified the detected layout. Mapper/crypttab validation and initramfs reconciliation still run.</source>
        <translation>Повторное использование загрузчика с этой сессии: вторая реконструкция UKI и дублирование регенерации GRUB пропускаются, когда более ранняя стадия EFI / UKI уже восстановлена и проверена обнаруженная компоновка. Проверка Mapper/crypttab и сверка initramfs все еще работают.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="17510"/>
        <source>Run this guarded repair action using the cached read-only diagnostic evidence. A confirmation is shown first.</source>
        <translation>Запустите эту охраняемую операцию по ремонту, используя кэшированные диагностические данные только для чтения. Сначала показывается подтверждение.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18102"/>
        <source>No cached capability evidence for this stage yet. Your saved selection is kept and its availability is re-checked when diagnostics for this scope complete.</source>
        <translation>Никаких кэшированных доказательств способности для этого этапа пока нет. Ваш сохраненный выбор сохраняется, и его доступность перепроверяется при завершении диагностики.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18108"/>
        <source> This stage is off by default: select it here to include it in Full Repair.</source>
        <translation>Этот этап выключен по умолчанию: выберите его здесь, чтобы включить его в полный ремонт.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18180"/>
        <source>Repair broken package dependencies (all detected package managers)</source>
        <translation>Ремонт сломанных зависимостей пакетов (все обнаруженные менеджеры пакетов)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18183"/>
        <source>Upgrade installed packages (all detected package managers)</source>
        <translation>Обновление установленных пакетов (все обнаруженные менеджеры пакетов)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18190"/>
        <source>Repair Alpine packages (apk fix)</source>
        <translation>Ремонт альпийских упаковок (apk)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18193"/>
        <source>Upgrade Alpine packages (apk upgrade)</source>
        <translation>Обновление альпийских пакетов (apk)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18209"/>
        <source>Repair Arch package dependencies (full pacman transaction)</source>
        <translation>Зависимость пакета Arch (полная транзакция pacman)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18212"/>
        <source>Upgrade Arch packages (full pacman transaction)</source>
        <translation>Обновление пакетов Arch (полная транзакция pacman)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18220"/>
        <source>Repair Fedora packages (dnf)</source>
        <translation>Ремонт пакетов Fedora (dnf)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18223"/>
        <source>Refresh package metadata (dnf makecache)</source>
        <translation>Обновление метаданных пакета (dnf makecache)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18226"/>
        <source>Upgrade Fedora packages (dnf upgrade)</source>
        <translation>Обновление пакетов Fedora (dnf)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18231"/>
        <source>Restore detected graphical login manager (OpenRC runlevel)</source>
        <translation>Восстановление обнаруженного графического менеджера входа в систему (OpenRC runlevel)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18239"/>
        <source>Restore detected graphical login manager (%1)</source>
        <translation>Восстановление обнаруженного графического менеджера входа (%1)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18248"/>
        <source>Rebuild initramfs (mkinitfs)</source>
        <translation>Восстановление initramfs (mkinitfs)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18252"/>
        <source>Rebuild initramfs (mkinitcpio)</source>
        <translation>Восстановление initramfs (mkinitcpio)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18257"/>
        <location filename="../src/MainWindow.cpp" line="18785"/>
        <source>Regenerate extlinux configuration</source>
        <translation>Регенерировать конфигурацию extlinux</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18278"/>
        <source>The running host is protected and cannot be repaired from itself.</source>
        <translation>Бегущий хост защищен и не может быть отремонтирован сам от себя.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18282"/>
        <source>The selected Linux root is still LUKS-encrypted. Unlock it first, refresh devices, then select the mapped filesystem.</source>
        <translation>Выбранный корень Linux по-прежнему зашифрован LUKS. Сначала разблокируйте его, обновите устройства, затем выберите отображенную файловую систему.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18286"/>
        <source>No mountable Linux root filesystem has been identified on the selected target.</source>
        <translation>Ни одна установленная корневая файловая система Linux не была идентифицирована по выбранной цели.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18297"/>
        <source>pkexec/Polkit is required to authorize repair operations.</source>
        <translation>Для проведения ремонтных работ требуется pkexec/Polkit.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18317"/>
        <source>The running host disk and root component could not be resolved.</source>
        <translation>Запущенный хост-диск и корневой компонент не могли быть решены.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18324"/>
        <source>The selected host identity is no longer marked as the protected running system. Refresh devices before retrying.</source>
        <translation>Выбранный идентификатор хоста больше не помечается как защищенная работающая система. Обновите устройства перед повторным использованием.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18334"/>
        <source>pkexec/Polkit is required to authorize host maintenance.</source>
        <translation>pkexec/Polkit требуется для авторизации обслуживания хоста.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18339"/>
        <source>Running host is ready for explicit guarded maintenance.</source>
        <translation>Запуск хоста готов к явному охраняемому обслуживанию.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18347"/>
        <source>Select Host Maintenance on the protected running-host card first.</source>
        <translation>Сначала выберите обслуживание хоста на защищенной карте бегущего хоста.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18360"/>
        <source>Confirm changes to the running host</source>
        <translation>Подтвердить изменения в бегущем хосте</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18361"/>
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
        <location filename="../src/MainWindow.cpp" line="18368"/>
        <source>Confirm changes to the selected repair system</source>
        <translation>Подтвердить изменения в выбранной системе ремонта</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18369"/>
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
        <location filename="../src/MainWindow.cpp" line="18378"/>
        <source>Run Repair</source>
        <translation>Запуск ремонта</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18397"/>
        <source>Repair unavailable</source>
        <translation>Ремонт недоступен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18637"/>
        <source>%1 failed</source>
        <translation>%1 провалился</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18638"/>
        <source>The repair action failed. Review the captured helper output in Logs for the failing stage and its reason.</source>
        <translation>Ремонтные работы провалились. Просмотрите захваченный вывод помощника в журналах для стадии отказа и его причины.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18690"/>
        <source>A file system check and repair is already running. Wait for it to finish before starting another repair; this request was not queued.</source>
        <translation>Проверка и ремонт файловой системы уже запущены. Подождите, пока он закончит, прежде чем начинать очередной ремонт; эта просьба не стояла в очереди.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18694"/>
        <source>File system repair in progress</source>
        <translation>Ремонт файловой системы продолжается</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18712"/>
        <location filename="../src/MainWindow.cpp" line="18852"/>
        <source>Repair diagnostics required</source>
        <translation>Требуется ремонтная диагностика</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18716"/>
        <location filename="../src/MainWindow.cpp" line="18810"/>
        <source>Repair tool unavailable</source>
        <translation>Ремонтный инструмент недоступен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18721"/>
        <source>Validate running host</source>
        <translation>Проверка бегущего хоста</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18726"/>
        <source>Validate repair target</source>
        <translation>Цель валидного ремонта</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18740"/>
        <location filename="../src/MainWindow.cpp" line="18859"/>
        <location filename="../src/MainWindow.cpp" line="19245"/>
        <source>running host</source>
        <translation>бегущий хост</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18741"/>
        <location filename="../src/MainWindow.cpp" line="18860"/>
        <location filename="../src/MainWindow.cpp" line="19246"/>
        <source>selected repair system</source>
        <translation>Выбранная система ремонта</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18745"/>
        <source>Complete interrupted package configuration in the %1 with dpkg --configure -a</source>
        <translation>Полная конфигурация прерванного пакета в %1 с dpkg -configure -a</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18749"/>
        <source>Repair broken APT package dependencies in the %1</source>
        <translation>Ремонт сломанных зависимостей пакета APT в %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18753"/>
        <source>Refresh APT package metadata in the %1</source>
        <translation>Обновить метаданные пакета APT в %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18757"/>
        <source>Run the distribution-specific transaction preflight, then choose a safe upgrade for the %1</source>
        <translation>Запустите предполетную транзакцию для конкретной дистрибуции, а затем выберите безопасное обновление для %1.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18761"/>
        <source>Run DKMS autoinstall in the %1</source>
        <translation>Запустите автоинсталляцию DKMS в %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18765"/>
        <source>Set graphical.target as the %1&apos;s default boot target</source>
        <translation>Установите graphical.target в качестве загрузочной цели по умолчанию %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18766"/>
        <source>Enable the detected display manager and repair display-manager.service without starting it inside a chroot or host helper</source>
        <translation>Включите обнаруженный дисплеем менеджер и ремонт дисплея-менеджера. сервис, не запуская его внутри chroot или помощника хоста</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18768"/>
        <source>Rebuild initramfs</source>
        <translation>Реконструкция initramfs</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18770"/>
        <source>Trial-build and then rebuild all initramfs images for the %1</source>
        <translation>Пробная сборка, а затем восстановление всех изображений initramfs для %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18772"/>
        <source>Repair EFI / UKI bootloader</source>
        <translation>Ремонт загрузчика EFI/UKI</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18776"/>
        <location filename="../src/MainWindow.cpp" line="18800"/>
        <source>detected bootloader</source>
        <translation>обнаруженный загрузчик</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18778"/>
        <source>Repair the %1&apos;s %2 boot path through its guarded installer or vendor builder, preserving every other boot entry</source>
        <translation>Отремонтируйте путь загрузки %1 %2 через своего охраняемого установщика или конструктора поставщиков, сохраняя каждый другой вход в загрузку.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18779"/>
        <source>Restore one verified default-loader entry when the guarded installer only wrote files</source>
        <translation>Восстановите одну верифицированную запись по умолчанию, когда защищенный установщик писал только файлы</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18781"/>
        <source>Regenerate GRUB configuration</source>
        <translation>Регенерировать конфигурацию GRUB</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18783"/>
        <source>Trial-generate and then regenerate the %1&apos;s GRUB configuration</source>
        <translation>Пробная генерация, а затем регенерация конфигурации %1 GRUB</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18787"/>
        <source>Regenerate the %1&apos;s extlinux bootloader configuration with an entry-preserving guard</source>
        <translation>Регенерировать конфигурацию загрузчика %1 extlinux с помощью защитного устройства для сохранения входа</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18789"/>
        <source>Reconcile boot stack</source>
        <translation>Обсуждение Boot Stack</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18802"/>
        <source>Validate mapper/crypttab against the %1</source>
        <translation>Валидатный картограф/криптаб против %1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18803"/>
        <source>Trial-build and rebuild initramfs for installed kernels</source>
        <translation>Испытание и восстановление initramfs для установленных ядер</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18804"/>
        <source>Repair the detected %1 boot path, removing only duplicate default destinations</source>
        <translation>Ремонт обнаруженного пути загрузки %1, удаление только дублирующих пунктов назначения по умолчанию</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18805"/>
        <source>Regenerate the detected bootloader configuration</source>
        <translation>Восстановление обнаруженной конфигурации загрузчика</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18807"/>
        <source>Reuse the bootloader repair already completed for this %1: skip the duplicate bootloader rebuild and GRUB regeneration</source>
        <translation>Повторное использование уже завершенного ремонта загрузчика для этого %1: пропустите восстановление дубликата загрузчика и регенерацию GRUB</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18811"/>
        <source>No repair workflow is registered for the selected tool.</source>
        <translation>Ремонтный процесс для выбранного инструмента не регистрируется.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18822"/>
        <location filename="../src/MainWindow.cpp" line="18881"/>
        <source>Read-only diagnostics completed; review the full evidence in Logs before confirming repair.</source>
        <translation>Завершена диагностика только для чтения; просмотрите все доказательства в журналах, прежде чем подтвердить ремонт.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18862"/>
        <source>Run the read-only file system check, then repair the selected devices</source>
        <translation>Запустите проверку файловой системы только для чтения, а затем отремонтируйте выбранные устройства</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18864"/>
        <source>Repair broken APT dependencies</source>
        <translation>Ремонт сломанных зависимостей APT</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18865"/>
        <source>Refresh APT package metadata</source>
        <translation>Обновить метаданные пакета APT</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18866"/>
        <source>Simulate APT first, then choose a safe upgrade/full-upgrade/dist-upgrade transaction</source>
        <translation>Сначала имитируйте APT, а затем выберите безопасную транзакцию обновления / полного обновления / отключения</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18868"/>
        <source>Restore the detected display manager and graphical.target without starting the GUI inside chroot</source>
        <translation>Восстановление обнаруженного дисплея и графического. Цель без запуска GUI внутри chroot</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18869"/>
        <source>Rebuild all initramfs images</source>
        <translation>Восстановление всех изображений initramfs</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18870"/>
        <source>Repair the %1 EFI / UKI boot path using its validated ESP</source>
        <translation>Ремонт загрузочного пути %1 EFI / UKI с использованием проверенного ESP</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18871"/>
        <source>Regenerate the %1&apos;s GRUB configuration</source>
        <translation>Регенерировать конфигурацию %1 GRUB</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18872"/>
        <source>Regenerate the %1&apos;s extlinux configuration</source>
        <translation>Регенерировать конфигурацию %1 extlinux</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18952"/>
        <source>The Full Repair plan is running. The read-only file system check pre-stage runs first, then the selected repair stages; each stage&apos;s output streams here as it completes.</source>
        <translation>План полного ремонта работает. Сначала выполняется проверка файловой системы только для чтения, затем выбранные этапы восстановления; выходной поток каждого этапа здесь по мере его завершения.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18970"/>
        <source>Full Repair plan finished with failures. Review the per-stage results (✓/✗/▪) in Logs, then close this window.</source>
        <translation>Полный ремонт закончился неудачами. Просмотрите результаты каждой стадии (✓/✗/▪) в журналах, затем закройте это окно.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="18971"/>
        <source>Full Repair plan finished. Review the per-stage results (✓/✗/▪) in Logs, then close this window.</source>
        <translation>Полный план ремонта завершен. Просмотрите результаты каждой стадии (✓/✗/▪) в журналах, затем закройте это окно.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19170"/>
        <source>File system check unavailable</source>
        <translation>Проверка файловой системы недоступна</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19196"/>
        <source>Check File Systems (Full Repair pre-stage)</source>
        <translation>Проверка файловых систем (полный ремонт на стадии)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19197"/>
        <source>Check File Systems (manual)</source>
        <translation>Проверьте файловые системы (ручные)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19211"/>
        <source>File system check failed</source>
        <translation>Проверка файловой системы провалилась</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19212"/>
        <source>The read-only file system check failed. Review the captured helper output in Logs for the reason.</source>
        <translation>Проверка файловой системы только для чтения не удалась. Просмотрите захваченный вывод помощника в журналах по этой причине.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19228"/>
        <source>File system repair unavailable</source>
        <translation>Ремонт файловой системы недоступен</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19241"/>
        <source>Checking and repairing file systems (Full Repair)</source>
        <translation>Проверка и ремонт файловых систем (полный ремонт)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19242"/>
        <source>Checking and repairing file systems</source>
        <translation>Проверка и ремонт файловых систем</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19292"/>
        <source>%1 mounted filesystem(s) were skipped because their check tools are offline-only</source>
        <translation>Файловая система (ы), установленная на %1, была пропущена, потому что их инструменты проверки отключены только</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19296"/>
        <source>%1 filesystem(s) are unsupported or have no installed check tool</source>
        <translation>Файловая система %1 не поддерживается или не имеет установленного инструмента проверки</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19300"/>
        <source>No file system errors were detected on the %1&apos;s root, /boot, ESP or /home filesystems. No repair was run.</source>
        <translation>Ошибки файловой системы не были обнаружены на %1 root, /boot, ESP или /home. Ремонт не проводился.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19302"/>
        <source>No repairable file system errors were detected on the %1&apos;s root, /boot, ESP or /home filesystems, but %2. No repair was run.</source>
        <translation>Никаких исправленных ошибок файловой системы не было обнаружено на корне %1, /boot, ESP или /home файловых системах, но %2. Ремонт не проводился.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19334"/>
        <source>The read-only check found issues on %1 mounted filesystem(s) that have no safe repair mode from this environment. Unmount the filesystem and check again, or repair it from a live system that is not using it.</source>
        <translation>Проверка только для чтения обнаружила проблемы в установленной файловой системе %1, которые не имеют безопасного режима восстановления из этой среды. Отсоедините файловую систему и проверьте снова или отремонтируйте ее из живой системы, которая ее не использует.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19367"/>
        <source>Run %1 repair on %2</source>
        <translation>Ремонт %1 на %2</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19390"/>
        <source>Dangerous Btrfs repair</source>
        <translation>Опасный ремонт Btrfs</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19391"/>
        <source>btrfs check --repair is a last-resort tool</source>
        <translation>Проверка btrfs - ремонт - инструмент последней инстанции</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19392"/>
        <source>Upstream Btrfs documentation warns that check --repair can make a damaged filesystem worse and can lose data. Back up everything you can reach first. Prefer a scrub, a rescue, or a fresh backup/restore when either is possible.

Run btrfs check --repair on:
%1</source>
        <translation>Документация Btrfs предупреждает, что исправление может ухудшить поврежденную файловую систему и привести к потере данных. Подкрепите все, что вы можете достичь в первую очередь. Предпочитаете скраб, спасение или свежее резервное копирование / восстановление, когда это возможно.

Проверка btrfs - ремонт:
%1</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19398"/>
        <source>Run dangerous Btrfs repair</source>
        <translation>Опасный ремонт Btrfs</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19435"/>
        <source>File system repair (%1)</source>
        <translation>Ремонт файловой системы (%1)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19436"/>
        <source>File system repair (%1, manual)</source>
        <translation>Ремонт файловой системы (%1, ручной)</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19474"/>
        <source>File system repair failed</source>
        <translation>Ремонт файловой системы провалился</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19475"/>
        <source>One or more file system repairs failed. Review the captured helper output in Logs for the failing device and its reason.</source>
        <translation>Одно или несколько исправлений файловой системы не удались. Просмотрите захваченный вывод помощника в журналах для отказа устройства и его причины.</translation>
    </message>
    <message>
        <location filename="../src/MainWindow.cpp" line="19691"/>
        <source>Device columns auto-sized. Drag headers to fine-tune widths.</source>
        <translation>Колонны устройств автоматического размера. Перетаскивайте заголовки для точной настройки ширины.</translation>
    </message>
</context>
</TS>
