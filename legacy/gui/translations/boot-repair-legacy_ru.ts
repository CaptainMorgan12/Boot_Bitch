<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS language="ru">
<context>
    <name>LegacyMainWindow</name>
    <message>
        <source>Validate environment</source>
        <translation>Проверить окружающую среду</translation>
    </message>
    <message>
        <source>Validate</source>
        <translation>подтверждать</translation>
    </message>
    <message>
        <source>Check the selected system&apos;s mounts, filesystem metadata, boot files, mapper consistency and dependency readiness before any repair action. This is an independent safety preflight rather than an optional Full Repair stage.</source>
        <translation>Проверьте установки выбранной системы, метаданные файловой системы, загрузочные файлы, согласованность картографа и готовность к зависимости перед любым действием по ремонту. Это самостоятельный предполет безопасности, а не факультативный этап полного ремонта.</translation>
    </message>
    <message>
        <source>Always preflight</source>
        <translation>Всегда предполетный</translation>
    </message>
    <message>
        <source>File system repair</source>
        <translation>Ремонт файловой системы</translation>
    </message>
    <message>
        <source>Check File Systems</source>
        <translation>Проверьте файловые системы</translation>
    </message>
    <message>
        <source>Run the read-only file system check for the selected system&apos;s root and /boot filesystems and report each device&apos;s check tool and result without changing anything. This legacy frontend exposes the read-only check only; device repair is not wired.</source>
        <translation>Запустите проверку файловой системы только для чтения для корневой и / загрузочной файловых систем выбранной системы и сообщите об инструменте проверки каждого устройства и получите результат, ничего не изменяя. Этот устаревший интерфейс выставляет только проверку чтения; ремонт устройства не подключен.</translation>
    </message>
    <message>
        <source>Full Repair plan: unavailable on this frontend - read-only check only</source>
        <translation>Полный план ремонта: недоступен на этом интерфейсе - только для чтения</translation>
    </message>
    <message>
        <source>Complete package configuration</source>
        <translation>Полная комплектация пакета</translation>
    </message>
    <message>
        <source>Complete Configuration</source>
        <translation>Полная конфигурация</translation>
    </message>
    <message>
        <source>Complete interrupted dpkg package configuration in the selected repair system. This is the same stage controlled by Settings -&gt; Full Repair plan -&gt; Complete interrupted package configuration, but it can also be run independently here.</source>
        <translation>Полное прерывание конфигурации пакета dpkg в выбранной системе ремонта. Это тот же этап, контролируемый настройками -&gt; Полный план ремонта -&gt; Полная прерываемая конфигурация пакета, но она также может быть запущена самостоятельно.</translation>
    </message>
    <message>
        <source>Run the guarded dpkg-configure repair? The helper keeps its package-lock and runtime preflights.</source>
        <translation>Запуск охраняемого dpkg-конфигурированного ремонта? Помощник сохраняет свой пакет-блок и время выполнения предварительных рейсов.</translation>
    </message>
    <message>
        <source>Repair broken dependencies</source>
        <translation>Ремонт сломанных зависимостей</translation>
    </message>
    <message>
        <source>Repair Dependencies</source>
        <translation>Ремонт зависимостей</translation>
    </message>
    <message>
        <source>Repair package dependencies in the selected repair system after the mandatory safety preflight. This maps directly to Settings -&gt; Repair broken package dependencies.</source>
        <translation>Зависимости пакета ремонта в выбранной системе ремонта после обязательного предполета безопасности. Эти карты непосредственно к настройкам -&gt; Ремонт сломанных пакетных зависимостей.</translation>
    </message>
    <message>
        <source>Run the guarded fix-broken repair? The helper keeps its simulation-first preflight and runtime guards.</source>
        <translation>Запуск охраняемого ремонта? Помощник сохраняет свои симуляторы-первый предполет и время выполнения охранников.</translation>
    </message>
    <message>
        <source>Refresh package metadata</source>
        <translation>Обновить метаданные пакета</translation>
    </message>
    <message>
        <source>Refresh Metadata</source>
        <translation>Обновление метаданных</translation>
    </message>
    <message>
        <source>Refresh APT metadata in the selected repair system without upgrading installed packages. This maps directly to Settings -&gt; Refresh package metadata.</source>
        <translation>Обновите метаданные APT в выбранной системе ремонта без обновления установленных пакетов. Эти карты непосредственно к настройкам -&gt; Обновить метаданные пакета.</translation>
    </message>
    <message>
        <source>Refresh package metadata for the selected scope? The helper requires a reachable, trusted APT source.</source>
        <translation>Обновить метаданные пакета для выбранной области? Помощник требует доступного, надежного источника APT.</translation>
    </message>
    <message>
        <source>Upgrade installed packages</source>
        <translation>Обновление установленных пакетов</translation>
    </message>
    <message>
        <source>Simulate and Upgrade</source>
        <translation>Моделирование и обновление</translation>
    </message>
    <message>
        <source>Simulate the APT transaction first, inspect proposed removals, then apply a safe upgrade. This maps directly to Settings -&gt; Upgrade installed packages.</source>
        <translation>Сначала имитируйте транзакцию APT, проверьте предлагаемые удаления, а затем примените безопасное обновление. Эти карты непосредственно к настройкам -&gt; Обновление установленных пакетов.</translation>
    </message>
    <message>
        <source>Run the guarded apt-upgrade transaction? The helper keeps its simulation-first and source guards.</source>
        <translation>Запуск защищенной транзакции апгрейда? Помощник сохраняет свою симуляцию-первую и охрану источника.</translation>
    </message>
    <message>
        <source>DKMS</source>
        <translation>DKMS</translation>
    </message>
    <message>
        <source>Rebuild DKMS</source>
        <translation>Восстановление DKMS</translation>
    </message>
    <message>
        <source>Rebuild out-of-tree kernel modules for kernels installed in the selected system. The helper refuses this action when DKMS is not installed; this legacy frontend exposes no DKMS action.</source>
        <translation>Восстановление модулей ядра для ядер, установленных в выбранной системе. Помощник отказывается от этого действия, когда DKMS не установлен; этот устаревший интерфейс не раскрывает действие DKMS.</translation>
    </message>
    <message>
        <source>Graphical login / display manager</source>
        <translation>Графический логин / дисплеев менеджер</translation>
    </message>
    <message>
        <source>Restore Graphical Login</source>
        <translation>Восстановить графический логин</translation>
    </message>
    <message>
        <source>Restore the legacy SysV display manager configured for the running host: the /etc/X11/default-display-manager entry and the missing runlevel S-symlink, with a backup and rollback, never starting the GUI. This is a host-scope stage on this legacy frontend.</source>
        <translation>Восстановление устаревшего дисплея SysV, настроенного для запуска хоста: запись /etc/X11/default-display-manager и отсутствующий S-symlink на уровне выполнения с резервным копированием и откатом, никогда не запускающий графический интерфейс. Это хост-сцена на этом унаследованном фронтенде.</translation>
    </message>
    <message>
        <source>Restore the graphical login configuration for the running host? The helper backs up /etc/X11/default-display-manager and the runlevel symlink state, restores the configured entry and the missing S-symlink, rolls back on any failure, and never starts the display manager.</source>
        <translation>Восстановить графическую конфигурацию входа для запущенного хоста? Помощник выполняет резервное копирование /etc/X11/default-дисплея-менеджера и состояния симлинка на уровне выполнения, восстанавливает настроенную запись и отсутствующую S-симлинку, откатывает любой отказ и никогда не запускает дисплея менеджера.</translation>
    </message>
    <message>
        <source>Initramfs</source>
        <translation>Инитрамфы</translation>
    </message>
    <message>
        <source>Rebuild Initramfs</source>
        <translation>Восстановление Initramfs</translation>
    </message>
    <message>
        <source>Rebuild initramfs images for the selected repair system only after mapper and crypttab consistency checks pass. The helper backs up each image before the apply. On Etch the stage runs through the guarded plain-chroot fallback (no unshare required).</source>
        <translation>Восстановите изображения initramfs для выбранной системы ремонта только после прохождения проверки согласованности картографа и crypttab. Помощник резервирует каждое изображение перед подачей заявки. На Etch сцена проходит через охраняемый запасной вариант с простым корнем (не требуется разделять).</translation>
    </message>
    <message>
        <source>Rebuild the initramfs for the selected scope? The helper keeps its mapper/crypttab and backup preflights.</source>
        <translation>Восстановление initramfs для выбранной области? Помощник хранит свой картпер/криптаб и резервные предполеты.</translation>
    </message>
    <message>
        <source>EFI / UKI bootloader</source>
        <translation>EFI/UKI загрузчик</translation>
    </message>
    <message>
        <source>Repair EFI / UKI</source>
        <translation>Ремонт EFI / UKI</translation>
    </message>
    <message>
        <source>Repair the selected system&apos;s EFI / UKI boot path. This legacy frontend exposes no EFI action; the Etch target is a BIOS/GRUB-legacy system.</source>
        <translation>Ремонт выбранного пути загрузки системы EFI/UKI. Этот унаследованный интерфейс не раскрывает действия EFI; целью Etch является система BIOS / GRUB-legacy.</translation>
    </message>
    <message>
        <source>GRUB configuration</source>
        <translation>Конфигурация GRUB</translation>
    </message>
    <message>
        <source>Regenerate GRUB</source>
        <translation>Регенерировать GRUB</translation>
    </message>
    <message>
        <source>Regenerate the selected repair system&apos;s GRUB menu/configuration after the mandatory safety preflight. The helper backs up menu.lst, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Регенерировать выбранное меню/конфигурацию системы ремонта GRUB после обязательного предварительного полета. Помощник резервного копирования меню.lst, сохраняет каждый существующий вход загрузки и откатывает на любой отказ.</translation>
    </message>
    <message>
        <source>Regenerate the GRUB configuration? The helper backs up the target menu/configuration, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Восстановление конфигурации GRUB. Помощник поддерживает целевое меню / конфигурацию, сохраняет каждый существующий вход в загрузку и откатывает любой отказ.</translation>
    </message>
    <message>
        <source>extlinux configuration</source>
        <translation>Конфигурация extlinux</translation>
    </message>
    <message>
        <source>Regenerate extlinux</source>
        <translation>Регенерировать extlinux</translation>
    </message>
    <message>
        <source>Regenerate the selected system&apos;s extlinux bootloader configuration. This legacy frontend exposes no extlinux action.</source>
        <translation>Восстановление конфигурации загрузчика extlinux выбранной системы. Этот устаревший интерфейс не раскрывает действия extlinux.</translation>
    </message>
    <message>
        <source>Boot stack reconciliation</source>
        <translation>Обсуждение Boot Stack</translation>
    </message>
    <message>
        <source>Reconcile Boot Stack</source>
        <translation>Обсуждение Boot Stack</translation>
    </message>
    <message>
        <source>Reconcile the selected repair system&apos;s boot stack in one guarded legacy pass: mapper/crypttab validation, initramfs rebuild and GRUB-legacy configuration regeneration, with the component backups and preflights unchanged. This is the Etch equivalent of the modern boot-stack reconciliation and stays out of the Full Repair plan.</source>
        <translation>Согласуйте загрузочный стек выбранной системы ремонта в одном защищенном пропуске: валидация Mapper /crypttab, восстановление initramfs и регенерация конфигурации GRUB-legacy с неизменными резервными копиями компонентов и предполетами. Это эквивалент Etch современного согласования загрузочного стека и не входит в план полного ремонта.</translation>
    </message>
    <message>
        <source>Full Repair plan: Manual recovery tool</source>
        <translation>Полный план ремонта: инструмент ручного восстановления</translation>
    </message>
    <message>
        <source>Run the guarded boot-stack reconciliation? The helper runs the mapper/crypttab validation, the initramfs rebuild and the GRUB-legacy regeneration in one pass with every component preflight and backup.</source>
        <translation>Управляйте охраняемой примирением багажника? Помощник запускает валидацию mapper/crypttab, восстановление initramfs и регенерацию GRUB-legacy за один проход с каждым компонентом предварительного полета и резервной копии.</translation>
    </message>
    <message>
        <source>Complete interrupted package configuration</source>
        <translation>Полная прерванная конфигурация пакета</translation>
    </message>
    <message>
        <source>Repair broken package dependencies</source>
        <translation>Ремонт сломанных зависимостей пакета</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Сделать Default</translation>
    </message>
    <message>
        <source>Repair file system errors (read-only check first)</source>
        <translation>Исправление ошибок файловой системы (проверка только для чтения)</translation>
    </message>
    <message>
        <source>the legacy frontend exposes the read-only file system check only; per-device repair is not wired on this frontend (fail closed)</source>
        <translation>унаследованный интерфейс выставляет только проверку файловой системы для чтения; ремонт каждого устройства не подключен к этому интерфейсу (не закрыт)</translation>
    </message>
    <message>
        <source>Upgrade installed packages (adaptive APT simulation)</source>
        <translation>Обновление установленных пакетов (адаптивное моделирование APT)</translation>
    </message>
    <message>
        <source>Rebuild DKMS modules</source>
        <translation>Восстановление модулей DKMS</translation>
    </message>
    <message>
        <source>Restore graphical login manager</source>
        <translation>Восстановление графического менеджера входа</translation>
    </message>
    <message>
        <source>Rebuild initramfs after mapper/crypttab validation</source>
        <translation>Восстановление initramfs после валидации mapper/crypttab</translation>
    </message>
    <message>
        <source>Repair EFI / UKI boot path</source>
        <translation>Ремонт EFI / UKI пути загрузки</translation>
    </message>
    <message>
        <source>Update GRUB configuration</source>
        <translation>Обновление конфигурации GRUB</translation>
    </message>
    <message>
        <source>Update extlinux configuration</source>
        <translation>Обновление конфигурации extlinux</translation>
    </message>
    <message>
        <source>Choose Full Repair stages in Settings. Enabled stages run in the order shown. Each configurable stage also appears below as an individual tool; the Full Repair column mirrors its current Settings state. The boot tools (EFI / UKI bootloader, GRUB or extlinux configuration, boot-stack reconciliation and Make Default) are independent: run them in any order, and a later action re-verifies what an earlier one changed and reports its own result. The active scope is shown beside Repair: selected repair drive or Running Host maintenance.</source>
        <translation>Выберите этапы полного ремонта в настройках. Включенные этапы выполняются в показанном порядке. Каждый настраиваемый этап также отображается ниже как отдельный инструмент; колонка «Полный ремонт» отражает его текущее состояние «Настройки». Инструменты загрузки (EFI / UKI загрузчик, GRUB или extlinux конфигурация, сверка загрузочного стека и по умолчанию) независимы: запустите их в любом порядке, а более позднее действие перепроверяет, что изменилось ранее, и сообщает о своем собственном результате. Активный диапазон показан рядом с ремонтом: выбранный ремонтный привод или техническое обслуживание Running Host.</translation>
    </message>
    <message>
        <source>Run All diagnostics for the selected target or running host before starting Full Repair. The report is read-only evidence used to choose and confirm repair stages.</source>
        <translation>Запустите всю диагностику выбранной цели или запуска хоста перед началом полного ремонта. Отчет представляет собой только показания, используемые для выбора и подтверждения этапов ремонта.</translation>
    </message>
    <message>
        <source>Ready: required cached read-only diagnostics are available for the selected stages. Review them in Diagnostics or Logs before confirming.</source>
        <translation>Готовы: для выбранных этапов доступна необходимая кэшированная диагностика только для чтения. Просмотрите их в диагностике или журналах, прежде чем подтвердить.</translation>
    </message>
    <message>
        <source>Environment validation</source>
        <translation>Проверка окружающей среды</translation>
    </message>
    <message>
        <source>Summarizes the selected system, protection state, mounted identity and inspection readiness.</source>
        <translation>Обобщает выбранную систему, состояние защиты, установленную идентичность и готовность к проверке.</translation>
    </message>
    <message>
        <source>Distribution and boot backend profile</source>
        <translation>Профиль Backend и Boot Backend</translation>
    </message>
    <message>
        <source>Identifies the distribution family, package manager, initramfs generator, bootloader and current guarded repair capability.</source>
        <translation>Идентифицирует семейство дистрибутивов, менеджер пакетов, генератор initramfs, загрузчик и текущую охраняемую возможность ремонта.</translation>
    </message>
    <message>
        <source>Boot diagnostics</source>
        <translation>Диагностика бута</translation>
    </message>
    <message>
        <source>Shows boot mounts and /boot contents plus storage evidence without changing the selected system.</source>
        <translation>Показывает загрузочные крепления и /boot-содержимое плюс доказательства хранения без изменения выбранной системы.</translation>
    </message>
    <message>
        <source>Boot evidence and selection history</source>
        <translation>Доказательства сапоги и история отбора</translation>
    </message>
    <message>
        <source>Correlates the detected boot chain, bootloader selection, kernel/initramfs and unlock evidence.</source>
        <translation>Сопоставляет обнаруженную цепочку загрузки, выбор загрузчика, ядро/initramfs и разблокировку доказательств.</translation>
    </message>
    <message>
        <source>Kernel / initramfs</source>
        <translation>Ядро / initramfs</translation>
    </message>
    <message>
        <source>Reviews kernel files and verifies matching initramfs images through a read-only inspection.</source>
        <translation>Обзор файлов ядра и проверка соответствия изображений initramfs с помощью проверки только для чтения.</translation>
    </message>
    <message>
        <source>Reviews the GRUB configuration without changing boot files.</source>
        <translation>Обзор конфигурации GRUB без изменения загрузочных файлов.</translation>
    </message>
    <message>
        <source>EFI / UKI boot state</source>
        <translation>EFI / UKI состояние загрузки</translation>
    </message>
    <message>
        <source>Inspects EFI/UKI evidence; unavailable on this legacy BIOS frontend with the helper&apos;s probe reason.</source>
        <translation>Проверяет доказательства EFI / UKI; недоступен на этом устаревшем фронтенде BIOS по причине зонда помощника.</translation>
    </message>
    <message>
        <source>Reviews the configured display manager and recent boot evidence without starting the GUI.</source>
        <translation>Обзор настроенного дисплея и недавних доказательств загрузки без запуска графического интерфейса.</translation>
    </message>
    <message>
        <source>Boot errors</source>
        <translation>Ошибки загрузки</translation>
    </message>
    <message>
        <source>Reads recent error-priority entries from the running host or selected repair system when available.</source>
        <translation>Считывает последние записи с приоритетом ошибки от запущенного хоста или выбранной системы ремонта, когда это доступно.</translation>
    </message>
    <message>
        <source>Disk usage</source>
        <translation>Использование диска</translation>
    </message>
    <message>
        <source>Summarizes filesystem capacity and free space for the running host or read-only repair target.</source>
        <translation>Обобщает емкость файловой системы и свободное пространство для запуска хоста или цели восстановления только для чтения.</translation>
    </message>
    <message>
        <source>File systems</source>
        <translation>Файловые системы</translation>
    </message>
    <message>
        <source>Runs the read-only file system check for the selected system&apos;s root, /boot and other filesystems.</source>
        <translation>Запускает проверку файловой системы только для чтения для корня выбранной системы, /boot и других файловых систем.</translation>
    </message>
    <message>
        <source>/etc/fstab review</source>
        <translation>/etc/fstab обзор</translation>
    </message>
    <message>
        <source>Displays the running host or selected repair system&apos;s fstab; repair-system inspection is mounted read-only.</source>
        <translation>Отображает запущенный хост или выбранную систему ремонта fstab; проверка ремонтной системы устанавливается только для чтения.</translation>
    </message>
    <message>
        <source>Btrfs status</source>
        <translation>Btrfs статус</translation>
    </message>
    <message>
        <source>Shows Btrfs filesystem and subvolume information when the target uses Btrfs.</source>
        <translation>Показывает файловую систему Btrfs и объемную информацию при использовании Btrfs.</translation>
    </message>
    <message>
        <source>Device-mapper ancestry</source>
        <translation>Предки приборов</translation>
    </message>
    <message>
        <source>Shows selected mapper ancestry and device-mapper state when available.</source>
        <translation>Показывает выбранную родословную картографа и состояние карты устройства, когда это доступно.</translation>
    </message>
    <message>
        <source>LUKS / crypttab evidence</source>
        <translation>LUKS / Скриптаб доказательства</translation>
    </message>
    <message>
        <source>Shows LUKS/mapped ancestry plus crypttab and fstab mapper references.</source>
        <translation>Показывает LUKS / картографическую родословную плюс ссылки на crypttab и fstab mapper.</translation>
    </message>
    <message>
        <source>Full diagnostic report</source>
        <translation>Полный диагностический отчет</translation>
    </message>
    <message>
        <source>Combines all read-only diagnostics for the selected scope (same as Run All).</source>
        <translation>Объединяет всю диагностику только для чтения для выбранной области (такой же, как Run All).</translation>
    </message>
    <message>
        <source>All entries</source>
        <translation>Все записи</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>диагностика</translation>
    </message>
    <message>
        <source>Repairs</source>
        <translation>ремонт</translation>
    </message>
    <message>
        <source>Package repair</source>
        <translation>Ремонт пакета</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Копия файла</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Открытие устройства</translation>
    </message>
    <message>
        <source>Host</source>
        <translation>ведущий</translation>
    </message>
    <message>
        <source>Required for block-device inventory</source>
        <translation>Требуется для инвентаризации блок-устройства</translation>
    </message>
    <message>
        <source>Filesystem identification</source>
        <translation>Идентификация файловой системы</translation>
    </message>
    <message>
        <source>Used to identify filesystem metadata</source>
        <translation>Используется для идентификации метаданных файловой системы</translation>
    </message>
    <message>
        <source>Mount inspection</source>
        <translation>Горная инспекция</translation>
    </message>
    <message>
        <source>Used to understand active mounts</source>
        <translation>Используется для понимания активных креплений</translation>
    </message>
    <message>
        <source>LUKS support</source>
        <translation>Поддержка LUKS</translation>
    </message>
    <message>
        <source>Required to unlock encrypted targets</source>
        <translation>Требуется для разблокировки зашифрованных целей</translation>
    </message>
    <message>
        <source>Btrfs support</source>
        <translation>Поддержка Btrfs</translation>
    </message>
    <message>
        <source>Required for Btrfs inspection and snapshot rollback</source>
        <translation>Требуется для проверки Btrfs и мгновенного отката</translation>
    </message>
    <message>
        <source>Bidirectional file copy</source>
        <translation>Двунаправленная копия файла</translation>
    </message>
    <message>
        <source>Host/Repair</source>
        <translation>Хост/ремонт</translation>
    </message>
    <message>
        <source>Required for verified Host-to-Repair and Repair-to-Host transfer</source>
        <translation>Требуется для верифицированного переноса хоста в ремонт и ремонта в хост</translation>
    </message>
    <message>
        <source>Chroot repair</source>
        <translation>Корневой ремонт</translation>
    </message>
    <message>
        <source>Required for target-side repair commands</source>
        <translation>Требуется для команд восстановления на стороне цели</translation>
    </message>
    <message>
        <source>Offline systemd repair</source>
        <translation>Оффлайн системный ремонт</translation>
    </message>
    <message>
        <source>Used to restore graphical.target and the configured display manager without starting the target GUI</source>
        <translation>Используется для восстановления графики. Целевой и настроенный дисплеевый менеджер без запуска целевого графического интерфейса</translation>
    </message>
    <message>
        <source>UEFI NVRAM inspection</source>
        <translation>Проверка UEFI NVRAM</translation>
    </message>
    <message>
        <source>Used to preserve target EFI BootOrder during TUXEDO UKI rebuilds when efivars are available</source>
        <translation>Используется для сохранения целевого EFI BootOrder во время восстановления TUXEDO UKI, когда доступны эфивары.</translation>
    </message>
    <message>
        <source>UKI verification</source>
        <translation>Проверка UKI</translation>
    </message>
    <message>
        <source>Used to verify the kernel embedded in a rebuilt unified kernel image</source>
        <translation>Используется для проверки ядра, встроенного в восстановленное унифицированное изображение ядра</translation>
    </message>
    <message>
        <source>GRUB EFI repair</source>
        <translation>Ремонт GRUB EFI</translation>
    </message>
    <message>
        <source>Target/Host</source>
        <translation>Цель/хостинг</translation>
    </message>
    <message>
        <source>Required only for conventional GRUB-based EFI systems</source>
        <translation>Требуется только для обычных систем EFI на основе GRUB.</translation>
    </message>
    <message>
        <source>Target</source>
        <translation>Цель</translation>
    </message>
    <message>
        <source>Debian-family GRUB helper</source>
        <translation>Семья Debian GRUB</translation>
    </message>
    <message>
        <source>Portable GRUB configuration generator used by Arch and other non-Debian systems</source>
        <translation>Портативный генератор конфигурации GRUB, используемый Arch и другими не-Debian системами</translation>
    </message>
    <message>
        <source>Initramfs rebuild</source>
        <translation>Восстановление Initramfs</translation>
    </message>
    <message>
        <source>Debian-family initramfs helper</source>
        <translation>Семья Debian initramfs</translation>
    </message>
    <message>
        <source>Arch-family initramfs generator</source>
        <translation>Архисемейный генератор initramfs</translation>
    </message>
    <message>
        <source>Alternative initramfs generator used by Arch and other distributions</source>
        <translation>Альтернативный генератор initramfs, используемый Arch и другими дистрибутивами</translation>
    </message>
    <message>
        <source>Initramfs verification</source>
        <translation>Проверка Initramfs</translation>
    </message>
    <message>
        <source>Read-only verification for mkinitcpio images</source>
        <translation>Проверка только для чтения изображений mkinitcpio</translation>
    </message>
    <message>
        <source>Read-only verification for dracut images</source>
        <translation>Проверка только для чтения изображений dracut</translation>
    </message>
    <message>
        <source>systemd-boot inspection</source>
        <translation>Инспекция systemd-boot</translation>
    </message>
    <message>
        <source>Host/Target</source>
        <translation>Хост/цель</translation>
    </message>
    <message>
        <source>Read-only inspection of systemd-boot and generic UKI layouts</source>
        <translation>Проверка systemd-boot и общих макетов UKI</translation>
    </message>
    <message>
        <source>Arch package manager</source>
        <translation>Арка менеджер пакетов</translation>
    </message>
    <message>
        <source>Arch-family package database and transaction tool</source>
        <translation>База данных пакетов Arch-family и инструмент транзакций</translation>
    </message>
    <message>
        <source>DKMS rebuild</source>
        <translation>Реконструкция DKMS</translation>
    </message>
    <message>
        <source>Required only when target uses DKMS modules</source>
        <translation>Требуется только при использовании модулей DKMS.</translation>
    </message>
    <message>
        <source>LVM inspection</source>
        <translation>Инспекция LVM</translation>
    </message>
    <message>
        <source>Optional LVM storage-stack support</source>
        <translation>Опциональная поддержка хранилища LVM</translation>
    </message>
    <message>
        <source>Software RAID</source>
        <translation>Программное обеспечение RAID</translation>
    </message>
    <message>
        <source>Optional Linux MD RAID support</source>
        <translation>Поддержка Linux MD RAID</translation>
    </message>
    <message>
        <source>Process namespace isolation</source>
        <translation>Изоляция пространства имен</translation>
    </message>
    <message>
        <source>Host+Target</source>
        <translation>Хозяин+ Цель</translation>
    </message>
    <message>
        <source>The ported helper falls back to a guarded plain chroot when unshare is absent</source>
        <translation>Портированный помощник падает обратно на охраняемый простой корень, когда неделя отсутствует.</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>отменить</translation>
    </message>
    <message>
        <source>OK</source>
        <translation>Хорошо.</translation>
    </message>
    <message>
        <source>Yes</source>
        <translation>Да.</translation>
    </message>
    <message>
        <source>No</source>
        <translation>Нет.</translation>
    </message>
    <message>
        <source>read-only helper diagnostic</source>
        <translation>Диагностика только для чтения</translation>
    </message>
</context>
<context>
    <name>legacy::LegacyMainWindow</name>
    <message>
        <source>Boot Bitch Legacy (Etch / KDE 3.5 era)</source>
        <translation>Boot Bitch Legacy (эра Etch/KDE 3.5)</translation>
    </message>
    <message>
        <source>Linux recovery and boot-repair utility</source>
        <translation>Восстановление Linux и загрузочная утилита</translation>
    </message>
    <message>
        <source>GUARDED REPAIR</source>
        <translation>Гвардейский ремонт</translation>
    </message>
    <message>
        <source>Ordinary repairs require an explicitly selected non-host target. The protected running host has a separate deliberate maintenance mode with the same guarded repair stages and requires privilege authorization.</source>
        <translation>Обычный ремонт требует явно выбранной цели. Защищенный рабочий хост имеет отдельный преднамеренный режим обслуживания с теми же этапами охраняемого ремонта и требует разрешения на получение привилегий.</translation>
    </message>
    <message>
        <source>Systems</source>
        <translation>Системы</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>диагностика</translation>
    </message>
    <message>
        <source>Repair</source>
        <translation>ремонт</translation>
    </message>
    <message>
        <source>Chroot Shell</source>
        <translation>Хрут Shell</translation>
    </message>
    <message>
        <source>File Copy</source>
        <translation>Копия файла</translation>
    </message>
    <message>
        <source>Logs</source>
        <translation>Лог</translation>
    </message>
    <message>
        <source>Settings</source>
        <translation>Настройки</translation>
    </message>
    <message>
        <source>(not created yet)</source>
        <translation>(до сих пор не создан)</translation>
    </message>
    <message>
        <source>Information</source>
        <translation>Информация</translation>
    </message>
    <message>
        <source>Close</source>
        <translation>Закрыть</translation>
    </message>
    <message>
        <source>&amp;File</source>
        <translation>&amp;Файл</translation>
    </message>
    <message>
        <source>&amp;Refresh Devices</source>
        <translation>Устройства &amp;Refresh</translation>
    </message>
    <message>
        <source>&amp;Lock Administrator Session</source>
        <translation>Сессия администратора &amp;Lock</translation>
    </message>
    <message>
        <source>&amp;Quit</source>
        <translation>&amp;уходит</translation>
    </message>
    <message>
        <source>&amp;View</source>
        <translation>&amp;Обзор</translation>
    </message>
    <message>
        <source>&amp;Systems</source>
        <translation>&amp;Системы</translation>
    </message>
    <message>
        <source>&amp;Diagnostics</source>
        <translation>&amp;Диагностика</translation>
    </message>
    <message>
        <source>&amp;Logs</source>
        <translation>&amp;Логи</translation>
    </message>
    <message>
        <source>&amp;Settings</source>
        <translation>&amp;Настройки</translation>
    </message>
    <message>
        <source>&amp;Auto-size Device Columns</source>
        <translation>Колонки устройств &amp;Auto-size</translation>
    </message>
    <message>
        <source>&amp;Wrap Log Lines</source>
        <translation>&amp;Wrap Логические линии</translation>
    </message>
    <message>
        <source>&amp;Help</source>
        <translation>&amp;помогает</translation>
    </message>
    <message>
        <source>&amp;Using Boot Bitch</source>
        <translation>использует &amp;Boot Bitch</translation>
    </message>
    <message>
        <source>&amp;About Boot Bitch</source>
        <translation>о &amp;Boot Bitch</translation>
    </message>
    <message>
        <source>Device columns auto-sized. Drag headers to fine-tune widths.</source>
        <translation>Колонны устройств автоматического размера. Перетаскивайте заголовки для точной настройки ширины.</translation>
    </message>
    <message>
        <source>A helper command is running; wait for it to finish before locking the session.</source>
        <translation>Помощник работает; дождитесь, пока он закончит, прежде чем закрыть сеанс.</translation>
    </message>
    <message>
        <source>Administrator session locked; the next privileged action will request authorization.</source>
        <translation>Сессия администратора закрыта; следующее привилегированное действие потребует разрешения.</translation>
    </message>
    <message>
        <source>Using Boot Bitch</source>
        <translation>Использование Boot Bitch</translation>
    </message>
    <message>
        <source>Boot Bitch must run from a different booted Linux environment than the system being repaired. Use a Linux live medium or another Linux installation on a different physical drive.&lt;br&gt;&lt;br&gt;The running host is protected from ordinary repair-target selection, but it can be explicitly selected through &lt;b&gt;Host Maintenance&lt;/b&gt; for guarded native diagnostics and supported maintenance stages.&lt;br&gt;&lt;br&gt;Diagnostics follow the Systems page: the committed repair drive while Host Maintenance is off, or the protected running host while it is active.&lt;br&gt;&lt;br&gt;The first privileged action requests administrator authorization once for this Boot Bitch window; &lt;b&gt;File - Lock Administrator Session&lt;/b&gt; ends that helper session immediately. Every repair keeps the helper&apos;s own runtime preflights.</source>
        <translation>Boot Bitch должен работать в другой загруженной среде Linux, чем отремонтированная система. Используйте среду Linux Live или другую установку Linux на другом физическом диске. Бегущий хост защищен от обычного выбора цели ремонта, но его можно явно выбрать через &lt;b&gt;Host Maintenance&lt;/b&gt; для защищенной нативной диагностики и поддерживаемых этапов обслуживания. Диагностика следует за страницей «Системы»: совершенный ремонтный диск, когда обслуживание хоста отключено, или защищенный работающий хост, когда он активен. Первое привилегированное действие запрашивает авторизацию администратора один раз для этого окна Boot Bitch; &lt;b&gt; Сессия администратора блокировки &lt;/b&gt; немедленно завершает эту сессию помощника. Каждый ремонт поддерживает собственные предварительные полеты помощника.</translation>
    </message>
    <message>
        <source>Select a physical drive; Boot Bitch resolves the most likely Linux system volume automatically. The running host stays protected from ordinary target repairs, with a separate explicit host-maintenance path for its own system. The Details button shows the protected host&apos;s facts in the details pane; selecting any drive row restores the per-drive pane.</source>
        <translation>Выберите физический диск; Boot Bitch автоматически решает наиболее вероятный объем системы Linux. Работающий хост остается защищенным от обычного ремонта цели, с отдельным явным способом обслуживания хоста для своей собственной системы. Кнопка «Подробности» показывает факты защищенного хоста в панели деталей; выбор любого ряда дисков восстанавливает панель с приводом.</translation>
    </message>
    <message>
        <source>Refresh Devices</source>
        <translation>Обновить устройства</translation>
    </message>
    <message>
        <source>Re-read the read-only kernel inventory (/proc/partitions, /proc/mounts, /proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* and the udev metadata database). No block device is opened and nothing is written.</source>
        <translation>Перечитайте инвентарь ядра, предназначенного только для чтения (/proc/partitions, /proc/mounts, /proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* и базу метаданных udev). Блок-устройство не открывается и ничего не пишется.</translation>
    </message>
    <message>
        <source>[OK]</source>
        <translation>[Хорошо]</translation>
    </message>
    <message>
        <source>The running system was detected and stays protected from ordinary target repairs.</source>
        <translation>Система была обнаружена и остается защищенной от обычного ремонта.</translation>
    </message>
    <message>
        <source>Detecting running system...</source>
        <translation>Обнаружение запущенной системы...</translation>
    </message>
    <message>
        <source>Detecting protected storage...</source>
        <translation>Обнаружение защищенного хранилища...</translation>
    </message>
    <message>
        <source>PROTECTED</source>
        <translation>защищенный</translation>
    </message>
    <message>
        <source>The running host remains protected from ordinary repair-target operations.</source>
        <translation>Работающий хост остается защищенным от обычных операций по ремонту.</translation>
    </message>
    <message>
        <source>Details</source>
        <translation>Подробности</translation>
    </message>
    <message>
        <source>Show read-only details for the protected running host.</source>
        <translation>Покажите только для чтения детали для защищенного хоста.</translation>
    </message>
    <message>
        <source>Host Maintenance</source>
        <translation>Обслуживание хоста</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Сделать Default</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host (menu.lst default directive with a backup and rollback). Requires Host Maintenance and the cached host-default probe.</source>
        <translation>Сделайте каноническую установленную запись ядра загрузочной записью GRUB-legacy по умолчанию на запущенном хосте (menu.lst директива по умолчанию с резервным копированием и откатом). Требуется техническое обслуживание хоста и кэшированный зонд по умолчанию.</translation>
    </message>
    <message>
        <source>Available repair targets</source>
        <translation>Доступные ремонтные цели</translation>
    </message>
    <message>
        <source>Most likely first</source>
        <translation>Скорее всего, первым</translation>
    </message>
    <message>
        <source>Drives are listed by the read-only inventory. Select a row to inspect it; Select Target commits the selected non-host drive with its auto-resolved Linux root component.</source>
        <translation>Водители перечислены в инвентаре только для чтения. Выберите строку, чтобы проверить ее; Выберите Target фиксирует выбранный не хост-накопитель с его авторазрешенным корневым компонентом Linux.</translation>
    </message>
    <message>
        <source>Device</source>
        <translation>Устройство</translation>
    </message>
    <message>
        <source>Size</source>
        <translation>Размер</translation>
    </message>
    <message>
        <source>Type</source>
        <translation>Тип</translation>
    </message>
    <message>
        <source>Filesystem</source>
        <translation>Файловая система</translation>
    </message>
    <message>
        <source>Select Target</source>
        <translation>Выберите цель</translation>
    </message>
    <message>
        <source>Unlock</source>
        <translation>Разблокировать</translation>
    </message>
    <message>
        <source>Authorize</source>
        <translation>разрешать</translation>
    </message>
    <message>
        <source>Establish the privileged helper session for the current scope now instead of waiting for the next privileged action.</source>
        <translation>Установите привилегированную сессию помощника для текущего объема сейчас, а не ждать следующего привилегированного действия.</translation>
    </message>
    <message>
        <source>Committed target: none</source>
        <translation>Обязательная цель: нет</translation>
    </message>
    <message>
        <source>Unlock state for the selected drive; the LUKS passphrase is never logged.</source>
        <translation>Состояние разблокировки для выбранного диска; парольная фраза LUKS никогда не регистрируется.</translation>
    </message>
    <message>
        <source>Unlock status</source>
        <translation>Статус разблокировки</translation>
    </message>
    <message>
        <source>Read-only inventory plus helper-confirmed facts; mirrors the modern Qt6 Selected drive details panel.</source>
        <translation>Инвентарь только для чтения плюс подтвержденные помощником факты; отражает современную панель деталей диска Qt6 Selected.</translation>
    </message>
    <message>
        <source>Selected drive details</source>
        <translation>Выбранные детали диска</translation>
    </message>
    <message>
        <source>Field</source>
        <translation>поле</translation>
    </message>
    <message>
        <source>Value</source>
        <translation>ценность</translation>
    </message>
    <message>
        <source>Run All runs every available read-only diagnostic for the current scope; selecting a check runs it alone. Diagnostics are read-only and are the only evidence source for the gated repair actions.</source>
        <translation>Run All запускает каждую доступную диагностику только для чтения для текущего объема; выбор чека запускает его самостоятельно. Диагностика только для чтения и является единственным источником доказательств для закрытых действий по ремонту.</translation>
    </message>
    <message>
        <source>Target: none selected</source>
        <translation>Цель: не выбрано</translation>
    </message>
    <message>
        <source>Diagnostics follow the committed repair target, or the protected running host while Host Maintenance is active.</source>
        <translation>Диагностика следует за поставленной целью ремонта или защищенным работающим хостом, в то время как техническое обслуживание хоста активно.</translation>
    </message>
    <message>
        <source>Run All</source>
        <translation>Беги все</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope.</source>
        <translation>Запустите все - запустите каждую доступную диагностику только для чтения для текущего объема.</translation>
    </message>
    <message>
        <source>Target configuration:</source>
        <translation>Конфигурация цели:</translation>
    </message>
    <message>
        <source>Etch-era target configuration files; availability is probed read-only by the helper&apos;s diagnostics.</source>
        <translation>Файлы конфигурации цели эпохи Etch; доступность проверяется только с помощью диагностики помощника.</translation>
    </message>
    <message>
        <source>Edit Target File...</source>
        <translation>Редактировать целевой файл...</translation>
    </message>
    <message>
        <source>Runs one read-only diagnostic for the selected scope through the helper (`diagnose &lt;key&gt;` / `host-diagnose &lt;key&gt;`); Run All is the combined report.</source>
        <translation>Запускает одну диагностику только для чтения для выбранной области с помощью помощника («диагностика &lt;key&gt;» / «диагностика хозяина &lt;key&gt;»); Run All - это комбинированный отчет.</translation>
    </message>
    <message>
        <source>Diagnostic checks</source>
        <translation>Диагностические проверки</translation>
    </message>
    <message>
        <source>Check</source>
        <translation>Проверить</translation>
    </message>
    <message>
        <source>Selected diagnostic</source>
        <translation>Выбранная диагностика</translation>
    </message>
    <message>
        <source>Select a diagnostic</source>
        <translation>Выберите диагностику</translation>
    </message>
    <message>
        <source>Choose a diagnostic from the list.</source>
        <translation>Выберите диагностику из списка.</translation>
    </message>
    <message>
        <source>Ready</source>
        <translation>Готовы</translation>
    </message>
    <message>
        <source>Results</source>
        <translation>Результаты</translation>
    </message>
    <message>
        <source>Run Diagnostic</source>
        <translation>Бег Диагностика</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic for the current scope.</source>
        <translation>Run Diagnostic - запустите выбранную диагностику только для чтения для текущего объема.</translation>
    </message>
    <message>
        <source>Copy Results</source>
        <translation>Копировать результаты</translation>
    </message>
    <message>
        <source>Save Results...</source>
        <translation>Сохранить результаты...</translation>
    </message>
    <message>
        <source>The Full Repair plan runs the selected legacy stages in order through the guarded helper; the individual tools run one stage at a time. Every action stays disabled until the cached capability lines say available and the helper keeps its runtime preflights.</source>
        <translation>План полного ремонта выполняет выбранные этапы наследия в порядке через охраняемого помощника; отдельные инструменты выполняют один этап за раз. Каждое действие остается отключенным до тех пор, пока кэшированные линии возможностей не скажут, что доступны, и помощник сохраняет свои предварительные полеты.</translation>
    </message>
    <message>
        <source>Full Repair plan</source>
        <translation>Полный план ремонта</translation>
    </message>
    <message>
        <source>No stages selected</source>
        <translation>Не выбраны этапы</translation>
    </message>
    <message>
        <source>Configure Plan...</source>
        <translation>Настройка плана...</translation>
    </message>
    <message>
        <source>Open Settings to choose which Full Repair stages are part of the plan.</source>
        <translation>Откройте настройки, чтобы выбрать, какие этапы полного ремонта являются частью плана.</translation>
    </message>
    <message>
        <source>Run Full Repair</source>
        <translation>Запуск полного ремонта</translation>
    </message>
    <message>
        <source>Select a repair drive, or choose Host Maintenance on the protected running-host card.</source>
        <translation>Выберите накопитель для ремонта или выберите обслуживание хоста на защищенной карте бегущего хоста.</translation>
    </message>
    <message>
        <source>Stage</source>
        <translation>этап</translation>
    </message>
    <message>
        <source>Individual repair tools</source>
        <translation>Индивидуальные ремонтные инструменты</translation>
    </message>
    <message>
        <source>Tool</source>
        <translation>Инструмент</translation>
    </message>
    <message>
        <source>Full Repair</source>
        <translation>Полный ремонт</translation>
    </message>
    <message>
        <source>not reported</source>
        <translation>не сообщается</translation>
    </message>
    <message>
        <source>Selected tool</source>
        <translation>Выбранный инструмент</translation>
    </message>
    <message>
        <source>Select a repair tool</source>
        <translation>Выберите инструмент для ремонта</translation>
    </message>
    <message>
        <source>Run Tool</source>
        <translation>Скачать Run Tool</translation>
    </message>
    <message>
        <source>Select a tool to review its repair action.</source>
        <translation>Выберите инструмент для проверки его действия по ремонту.</translation>
    </message>
    <message>
        <source>Write actions ask for confirmation and then run the helper&apos;s own runtime preflights; the GUI never weakens them. A repair that is not proven &apos;unchanged&apos; invalidates the cached diagnostics and disables the gated actions until diagnostics run again.</source>
        <translation>Напишите действия, попросите подтверждения, а затем запустите собственные предварительные полеты помощника; GUI никогда не ослабляет их. Восстановление, которое не доказано как «неизмененное», отменяет кэшированную диагностику и отключает закрытые действия до тех пор, пока диагностика не повторится.</translation>
    </message>
    <message>
        <source>Chroot shell</source>
        <translation>Корневая раковина</translation>
    </message>
    <message>
        <source>Offline commands run one at a time in a fresh chroot and cannot answer interactive prompts (apt-get -y upgrade works). Host-shell commands run directly on the running host. The helper&apos;s probe lines gate the command field; the exact reason appears in the tooltip.</source>
        <translation>Офлайн-команды выполняются по одной за раз в свежем chroot и не могут отвечать на интерактивные подсказки (работает обновление apt-get-y). Команды Host-shell работают непосредственно на бегущем хосте. Зондовые линии помощника заходят в командное поле; точная причина появляется в подсказке.</translation>
    </message>
    <message>
        <source>No &apos;Legacy feature shell:&apos; line is cached; run diagnostics for the selected scope to evaluate the helper&apos;s chroot/timeout containment probes (fail closed).</source>
        <translation>Никакая оболочка функции «Наследие»: строка кэшируется; запустите диагностику для выбранной области для оценки зондов сдерживания хрота / тайм-аута помощника (не закрыта).</translation>
    </message>
    <message>
        <source>Command</source>
        <translation>Командование</translation>
    </message>
    <message>
        <source>Command:</source>
        <translation>Командир:</translation>
    </message>
    <message>
        <source>One reviewed command string, passed to the helper as a single argument (no shell interpolation by the GUI).</source>
        <translation>Один рассмотрел командную строку, переданную помощнику в качестве единого аргумента (без интерполяции оболочки GUI).</translation>
    </message>
    <message>
        <source>Run Command</source>
        <translation>Запускайте команду</translation>
    </message>
    <message>
        <source>Clear Output</source>
        <translation>Чистый результат</translation>
    </message>
    <message>
        <source>The helper exposes `shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for an offline target chroot and `host-shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for the running host. Both keep the helper&apos;s runtime preflights; this tab enables the command only when the scope is committed, the session is authorized and the scope&apos;s Legacy feature probe reports available.</source>
        <translation>Помощник разоблачает «shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;» для автономного целевого chroot и «host-shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;» для запущенного хоста. Оба поддерживают предварительные полеты помощника во время выполнения; эта вкладка позволяет команду только тогда, когда область действия совершена, сессия разрешена, а отчеты зонда функции Наследия области доступны.</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Копия файла</translation>
    </message>
    <message>
        <source>Copy and verify files in either direction through the guarded helper (cp -a plus ownership restoration and a per-file byte-compare). The helper&apos;s file-copy probe gates the controls and keeps the direction and path containment checks.</source>
        <translation>Копировать и проверять файлы в любом направлении через охраняемого помощника (cp - плюс восстановление собственности и байт-сравнение каждого файла). Зонд-копия помощника закрывает элементы управления и сохраняет контроль направления и пути.</translation>
    </message>
    <message>
        <source>Preview Changes</source>
        <translation>Превью Изменения</translation>
    </message>
    <message>
        <source>Run a copy dry-run through the guarded helper. No files are changed.</source>
        <translation>Пробейте копию через охраняемого помощника. Файлы не меняются.</translation>
    </message>
    <message>
        <source>Copy and Verify</source>
        <translation>Копировать и проверять</translation>
    </message>
    <message>
        <source>Copy staged items and verify the result. Existing destination names are overwritten when source content differs; unrelated destination files are never deleted.</source>
        <translation>Копируйте постановочные элементы и проверяйте результат. Существующие имена пунктов назначения перезаписываются, когда исходный контент отличается; несвязанные файлы назначения никогда не удаляются.</translation>
    </message>
    <message>
        <source>Direction:</source>
        <translation>Направление:</translation>
    </message>
    <message>
        <source>Choose which system supplies the source files and which system receives them.</source>
        <translation>Выберите, какая система поставляет исходные файлы и какая система их получает.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from this host</source>
        <translation>1.Выберите исходные файлы или папки этого хоста</translation>
    </message>
    <message>
        <source>Source</source>
        <translation>Источник</translation>
    </message>
    <message>
        <source>Files and folders staged for the verified copy. The legacy backend copies with cp -a and restores ownership with chown --reference; every regular file is byte-compared after the copy.</source>
        <translation>Файлы и папки, созданные для верифицированной копии. Наследственные бэкэнд-копии с cp-a и восстанавливают право собственности с помощью ссылки; каждый обычный файл сравнивается с байтом после копии.</translation>
    </message>
    <message>
        <source>Add Files...</source>
        <translation>Добавить файлы...</translation>
    </message>
    <message>
        <source>Add Folder...</source>
        <translation>Добавить папку...</translation>
    </message>
    <message>
        <source>Remove</source>
        <translation>Удалить</translation>
    </message>
    <message>
        <source>Clear</source>
        <translation>Чисто.</translation>
    </message>
    <message>
        <source>Clear the staged source list (nothing is copied or deleted).</source>
        <translation>Очистите постановочный список источников (ничего не копируется и не удаляется).</translation>
    </message>
    <message>
        <source>2. Choose destination in repaired system</source>
        <translation>2.Выберите пункт назначения в отремонтированной системе</translation>
    </message>
    <message>
        <source>unavailable: see the helper&apos;s Legacy feature file-copy: probe reason above</source>
        <translation>Недоступно: см. файл-копия наследства помощника: причина зонда выше</translation>
    </message>
    <message>
        <source>An absolute path inside the selected repair system (Host to Repair) or on the running host (Repair to Host).</source>
        <translation>Абсолютный путь внутри выбранной системы ремонта (Host to Repair) или на запущенном хосте (Repair to Host).</translation>
    </message>
    <message>
        <source>Browse Target Folders...</source>
        <translation>Просмотр целевых папок...</translation>
    </message>
    <message>
        <source>Browse the selected repair system through the helper&apos;s temporary read-only mounts and choose an absolute destination path. No target files are changed while browsing.</source>
        <translation>Просмотрите выбранную систему ремонта через временные крепления только для чтения помощника и выберите абсолютный путь назначения. Никакие целевые файлы не меняются во время просмотра.</translation>
    </message>
    <message>
        <source>3. Ownership and copy policy</source>
        <translation>3. Политика владения и копирования</translation>
    </message>
    <message>
        <source>Ownership:</source>
        <translation>Право собственности:</translation>
    </message>
    <message>
        <source>Smart destination ownership (recommended)</source>
        <translation>Умное владение пунктом назначения (рекомендуется)</translation>
    </message>
    <message>
        <source>Preserve source numeric UID/GID</source>
        <translation>Сохранить исходный номер UID/GID</translation>
    </message>
    <message>
        <source>Smart mode validates UID/GID identity mapping across the two systems and falls back to the destination-directory owner when the same numeric ID means a different account (the legacy backend implements it with chown --reference).</source>
        <translation>Смарт-режим проверяет отображение идентификационных данных UID / GID в обеих системах и возвращается к владельцу каталога назначения, когда один и тот же цифровой идентификатор означает другую учетную запись (унаследованный бэкэнд реализует его с помощью ссылки).</translation>
    </message>
    <message>
        <source>Application log</source>
        <translation>Журнал приложений</translation>
    </message>
    <message>
        <source>The complete session register; Save As... writes every entry even while a filter hides lines. If a writable system share mount exists at /host, Save As... starts there; otherwise the log directory is the fallback. Prior session files are listed read-only.</source>
        <translation>Полный регистр сеанса; Save As... записывает каждую запись, даже когда фильтр скрывает строки. Если на /host существует монтировка для общего доступа к записной системе, Save As... начинается там; в противном случае каталог журнала является запасным вариантом. Файлы предыдущих сессий перечислены только для чтения.</translation>
    </message>
    <message>
        <source>Session logs</source>
        <translation>Журналы сеансов</translation>
    </message>
    <message>
        <source>Session</source>
        <translation>Заседание</translation>
    </message>
    <message>
        <source>The first entry is the live session; earlier files in the log directory are listed read-only below it.</source>
        <translation>Первая запись - это сеанс в реальном времени; более ранние файлы в каталоге журналов перечислены только для чтения ниже.</translation>
    </message>
    <message>
        <source>New Session Log</source>
        <translation>Новый Session Log</translation>
    </message>
    <message>
        <source>Close the active session file; it becomes a prior session and the next log entry starts a new file.</source>
        <translation>Закройте файл активной сессии; он становится предыдущей сессией, и следующая запись журнала запускает новый файл.</translation>
    </message>
    <message>
        <source>Add Note</source>
        <translation>Добавить комментарий</translation>
    </message>
    <message>
        <source>Append a NOTE entry to the live session register.</source>
        <translation>Добавить запись ПРИМЕЧАНИЯ в регистр живых сессий.</translation>
    </message>
    <message>
        <source>Delete</source>
        <translation>Исключить</translation>
    </message>
    <message>
        <source>Delete the selected prior session file (the live session is never deleted).</source>
        <translation>Удалите выбранный файл предыдущей сессии (сессия в реальном времени никогда не удаляется).</translation>
    </message>
    <message>
        <source>Refresh</source>
        <translation>освежить</translation>
    </message>
    <message>
        <source>Save As...</source>
        <translation>Спасти как...</translation>
    </message>
    <message>
        <source>Save the complete session log (all entries, not just the current filter).</source>
        <translation>Сохраните полный журнал сеанса (все записи, а не только текущий фильтр).</translation>
    </message>
    <message>
        <source>Clear Register</source>
        <translation>Чистый регистр</translation>
    </message>
    <message>
        <source>Clear the live register and view; prior session files are never modified.</source>
        <translation>Очистите живой регистр и просмотр; файлы предыдущих сеансов никогда не изменяются.</translation>
    </message>
    <message>
        <source>Search log:</source>
        <translation>Журнал поиска:</translation>
    </message>
    <message>
        <source>Type any characters to show matching log entries (case-insensitive). Save As always writes every entry.</source>
        <translation>Введите любые символы, чтобы показать соответствующие записи журнала (case-insensitive). Спасти Как всегда, записывает каждую запись.</translation>
    </message>
    <message>
        <source>Filter:</source>
        <translation>Фильтр:</translation>
    </message>
    <message>
        <source>Filter the visible log by entry kind. Selecting a diagnostic section shows the lines captured for that section; a workflow filter such as File system repair or Package repair shows its mapped repair lines (File copy has no lines in this frontend). Save As always writes every entry.</source>
        <translation>Фильтруйте видимый журнал по типу записи. Выбор диагностического раздела показывает линии, захваченные для этого раздела; фильтр рабочего процесса, такой как ремонт файловой системы или ремонт пакетов, показывает свои отображаемые линии ремонта (копия файла не имеет линий в этом интерфейсе). Спасти Как всегда, записывает каждую запись.</translation>
    </message>
    <message>
        <source>Settings are stored per user under ~/.qt/, one file per settings group (devicesrc, logsrc, diagnosticsrc, repairrc), and are saved immediately on every change and on close. Launch the GUI as the same user to keep your overrides; a GUI started as root keeps its own copies.</source>
        <translation>Настройки хранятся на каждого пользователя в ~/.qt/, один файл на группу настроек (устройство, логсрк, диагностика, ремонт) и сохраняются сразу же при каждом изменении и при закрытии. Запустите GUI как один и тот же пользователь, чтобы сохранить свои перезагрузки; GUI, запущенный как root, сохраняет свои собственные копии.</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Открытие устройства</translation>
    </message>
    <message>
        <source>Show devices without an identified Linux installation</source>
        <translation>Показать устройства без идентифицированной установки Linux</translation>
    </message>
    <message>
        <source>Show removable and USB storage</source>
        <translation>Показать съемное и USB-хранилище</translation>
    </message>
    <message>
        <source>Show encrypted devices before unlocking</source>
        <translation>Показать зашифрованные устройства перед разблокировкой</translation>
    </message>
    <message>
        <source>When off, drives without a visible Linux filesystem are hidden unless they still contain an encrypted device and encrypted devices are shown.</source>
        <translation>При выключении диски без видимой файловой системы Linux скрыты, если они все еще не содержат зашифрованное устройство и зашифрованные устройства не показаны.</translation>
    </message>
    <message>
        <source>When off, removable and USB drives are hidden from the repair-target list.</source>
        <translation>При выключении съемные и USB-накопители скрыты от списка целей ремонта.</translation>
    </message>
    <message>
        <source>When off, drives with an encrypted device are hidden until the volume is unlocked.</source>
        <translation>При выключении диски с зашифрованным устройством скрываются до тех пор, пока громкость не разблокируется.</translation>
    </message>
    <message>
        <source>Include the %1 stage in the Full Repair plan. The stage runs in the plan order shown on the Repair tab.</source>
        <translation>Включите этап %1 в план полного ремонта. Этап проходит в порядке плана, показанном на вкладке «Ремонт».</translation>
    </message>
    <message>
        <source>Automatically regenerate read-only diagnostics after repairs or target changes</source>
        <translation>Автоматически регенерировать диагностику только для чтения после ремонта или изменения цели</translation>
    </message>
    <message>
        <source>Regenerates the cached read-only diagnostics for the current scope after an operation that invalidates them (LUKS unlock, target configuration edit). It runs only inside an already authorized administrator session and never opens an authorization prompt by itself.</source>
        <translation>Восстанавливает кэшированную диагностику только для чтения для текущего объема после операции, которая делает их недействительными (разблокировка LUKS, редактирование конфигурации цели). Он работает только внутри уже авторизованной сессии администратора и никогда не открывает запрос на авторизацию самостоятельно.</translation>
    </message>
    <message>
        <source>Wrap long log lines</source>
        <translation>Оберните длинные бревенчатые линии</translation>
    </message>
    <message>
        <source>Mandatory safety controls</source>
        <translation>Обязательный контроль безопасности</translation>
    </message>
    <message>
        <source>Host capabilities and dependencies</source>
        <translation>Возможности и зависимости хоста</translation>
    </message>
    <message>
        <source>Refresh Capabilities</source>
        <translation>Обновить возможности</translation>
    </message>
    <message>
        <source>Re-run the read-only host capability probes (a PATH search, nothing is executed) and refresh the distribution summary.</source>
        <translation>Перезапуск зондов возможностей только для чтения (поиск PATH, ничего не выполняется) и обновление сводки дистрибутива.</translation>
    </message>
    <message>
        <source>Install Missing Support...</source>
        <translation>Недостающая поддержка...</translation>
    </message>
    <message>
        <source>Automatic installation will require explicit package mapping and privilege authorization.</source>
        <translation>Автоматическая установка потребует явного отображения пакетов и авторизации привилегий.</translation>
    </message>
    <message>
        <source>Application configuration</source>
        <translation>Конфигурация приложения</translation>
    </message>
    <message>
        <source>Boot Bitch Legacy</source>
        <translation>Наследие Boot Bitch</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list first.</source>
        <translation>Сначала выберите физический привод в списке доступных целей ремонта.</translation>
    </message>
    <message>
        <source>Protected system</source>
        <translation>Защищенная система</translation>
    </message>
    <message>
        <source>The running system cannot be selected as a repair target. Use Host Maintenance for the protected running host or choose another disk.</source>
        <translation>Система не может быть выбрана в качестве цели ремонта. Используйте Host Maintenance для защищенного хоста или выберите другой диск.</translation>
    </message>
    <message>
        <source>Unlock or select a Linux system first</source>
        <translation>Сначала разблокируйте или выберите систему Linux</translation>
    </message>
    <message>
        <source>This encrypted drive has no visible Linux filesystem yet. Use Unlock, refresh devices, and select the target after its Linux root is detected.</source>
        <translation>Этот зашифрованный диск еще не имеет видимой файловой системы Linux. Используйте Unlock, обновите устройства и выберите цель после обнаружения корня Linux.</translation>
    </message>
    <message>
        <source>The selected root component (%1) belongs to the running system and cannot be committed as a repair target. Use Host Maintenance for the protected running host.</source>
        <translation>Выбранный корневой компонент (%1) относится к работающей системе и не может быть выполнен в качестве цели ремонта. Используйте Host Maintenance для защищенного хоста.</translation>
    </message>
    <message>
        <source>repair target commit</source>
        <translation>Целевой ремонт</translation>
    </message>
    <message>
        <source>Repair drive selected: %1</source>
        <translation>Выбранный привод для ремонта: %1</translation>
    </message>
    <message>
        <source>; best detected system component: %1</source>
        <translation>Лучший обнаруженный компонент системы: %1</translation>
    </message>
    <message>
        <source>. No mount or repair action was performed.</source>
        <translation>Никаких монтажных или ремонтных работ не проводилось.</translation>
    </message>
    <message>
        <source>Make Default unavailable</source>
        <translation>Сделать Default недоступным</translation>
    </message>
    <message>
        <source>Make Default is a running-host action on this frontend; enter Host Maintenance first.</source>
        <translation>Make Default - это действие бегущего хоста на этом интерфейсе; сначала введите обслуживание хоста.</translation>
    </message>
    <message>
        <source>Administrator authorization required</source>
        <translation>Требуется разрешение администратора</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize first.</source>
        <translation>Сеанс администратора не активен; сначала нажмите Авторизовать.</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host?

The helper verifies /boot/grub/menu.lst, sets the `default &lt;N&gt;` directive to the canonical entry, backs the menu up first and restores it on any failure.</source>
        <translation>Сделайте каноническую установленную запись ядра загрузочной записью GRUB-legacy на запущенном хосте?

Помощник проверяет /boot/grub/menu.lst, устанавливает директиву «по умолчанию &lt;N&gt;» для канонической записи, сначала возвращает меню и восстанавливает его при любом сбое.</translation>
    </message>
    <message>
        <source>Host Maintenance unavailable</source>
        <translation>Обслуживание хоста недоступно</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a committed repair target or a detected running host.</source>
        <translation>Запускаемая хост-мишень не может быть обнаружена; для диагностики требуется совершенный ремонт или обнаруженный запущенный хост.</translation>
    </message>
    <message>
        <source>Exit Host Maintenance</source>
        <translation>Обслуживание Exit Host</translation>
    </message>
    <message>
        <source>Host Maintenance is active; diagnostics and gated repairs target the protected running host.</source>
        <translation>Обслуживание хоста активно; диагностика и закрытый ремонт нацелены на защищенный работающий хост.</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list on the Systems tab first, or use Host Maintenance for the protected running host.</source>
        <translation>Сначала выберите физический диск в списке доступных целей ремонта на вкладке «Системы» или используйте техническое обслуживание хоста для защищенного работающего хоста.</translation>
    </message>
    <message>
        <source>The selected drive (%1) is the protected running host. Choose Host Maintenance on the Systems tab to run read-only host diagnostics and guarded host repairs; ordinary target repairs stay disabled.</source>
        <translation>Выбранный диск (%1) является защищенным работающим хостом. Выберите обслуживание хоста на вкладке «Системы», чтобы запускать диагностику хоста только для чтения и охраняемый ремонт хоста; обычный ремонт цели остается отключенным.</translation>
    </message>
    <message>
        <source>No repair target is committed. Choose Select Target on the Systems tab (or Host Maintenance for the protected running host) first.</source>
        <translation>Целевой ремонт не выполняется. Сначала выберите «Выбрать цель» на вкладке «Системы» (или «Поддержание хоста для защищенного работающего хоста»).</translation>
    </message>
    <message>
        <source>The selection changed after the target was committed. Choose Select Target again.</source>
        <translation>Выбор изменился после достижения цели. Выберите снова цель.</translation>
    </message>
    <message>
        <source>Diagnostics scope required</source>
        <translation>Диагностика требуемого объема</translation>
    </message>
    <message>
        <source>Diagnostics scope unresolved</source>
        <translation>Диагностическая область неразрешенной</translation>
    </message>
    <message>
        <source>The running host target could not be resolved; use Refresh Devices and commit a repair target or re-enter Host Maintenance.</source>
        <translation>Выполняемая цель хоста не может быть решена; используйте Refresh Devices и совершите ремонт цели или повторно введите обслуживание хоста.</translation>
    </message>
    <message>
        <source>Diagnostic check required</source>
        <translation>Требуется диагностика</translation>
    </message>
    <message>
        <source>Select a diagnostic check in the list first.</source>
        <translation>Сначала выберите диагностический контроль в списке.</translation>
    </message>
    <message>
        <source>Repair target required</source>
        <translation>Требуемая цель ремонта</translation>
    </message>
    <message>
        <source>Target file editing needs a committed repair target. Running-host maintenance has no target-file editing; commit an offline target first.</source>
        <translation>Редактирование целевого файла требует целевого восстановления. Обслуживание Running-host не имеет редактирования целевого файла; сначала совершите офлайн-цель.</translation>
    </message>
    <message>
        <source>Configuration file required</source>
        <translation>Требуется файл конфигурации</translation>
    </message>
    <message>
        <source>Select a target configuration file first.</source>
        <translation>Сначала выберите целевой файл конфигурации.</translation>
    </message>
    <message>
        <source>Edit target %1</source>
        <translation>Редактировать цель %1</translation>
    </message>
    <message>
        <source>Edit this target file through the guarded administrator helper. A successful save invalidates cached diagnostics; rerun diagnostics before repair. Generated files such as /boot/grub/menu.lst may be replaced by the next bootloader update.</source>
        <translation>Редактируйте этот целевой файл через охраняемого помощника администратора. Успешное сохранение аннулирует кэшированную диагностику; повторите диагностику перед ремонтом. Сгенерированные файлы, такие как /boot/grub/menu.lst, могут быть заменены следующим обновлением загрузчика.</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>отменить</translation>
    </message>
    <message>
        <source>Save Target File</source>
        <translation>Сохранить целевой файл</translation>
    </message>
    <message>
        <source>No changes to %1.</source>
        <translation>Никаких изменений в %1.</translation>
    </message>
    <message>
        <source>Configuration write refused</source>
        <translation>Конфигурация письма отказала</translation>
    </message>
    <message>
        <source>The edited content contains NUL bytes; the guarded write refuses it.</source>
        <translation>Отредактированный контент содержит байты NUL; охраняемая запись отказывается от него.</translation>
    </message>
    <message>
        <source>File too large</source>
        <translation>Файл слишком большой</translation>
    </message>
    <message>
        <source>The edited file is larger than 1 MiB. The guarded write refuses it; edit the file from a console instead.</source>
        <translation>Отредактированный файл больше 1 МиБ. Охраняемая запись отказывается от него; редактируйте файл с консоли.</translation>
    </message>
    <message>
        <source>Write target configuration</source>
        <translation>Записать целевую конфигурацию</translation>
    </message>
    <message>
        <source>Write the edited contents to %1? This modifies the repair target and invalidates cached diagnostics.</source>
        <translation>Написать отредактированный контент на %1? Это изменяет цель ремонта и аннулирует кэшированную диагностику.</translation>
    </message>
    <message>
        <source>No diagnostic results to copy yet.</source>
        <translation>Пока нет результатов диагностики для копирования.</translation>
    </message>
    <message>
        <source>Diagnostic results copied to the clipboard.</source>
        <translation>Результаты диагностики скопированы в буфер обмена.</translation>
    </message>
    <message>
        <source>No diagnostic results to save yet.</source>
        <translation>Никаких диагностических результатов пока не сохранилось.</translation>
    </message>
    <message>
        <source>Text files (*.txt);;All files (*)</source>
        <translation>Текстовые файлы (*.txt); Все файлы (*)</translation>
    </message>
    <message>
        <source>Save diagnostic results</source>
        <translation>Экономия диагностических результатов</translation>
    </message>
    <message>
        <source>Could not write %1.</source>
        <translation>Писать %1 нельзя.</translation>
    </message>
    <message>
        <source>Diagnostic results saved to %1</source>
        <translation>Результаты диагностики сохранены в %1</translation>
    </message>
    <message>
        <source>Unlock not available</source>
        <translation>Unlock недоступен</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked. Select an offline repair target to unlock.</source>
        <translation>Защищенный хост не может быть разблокирован. Выберите цель автономного ремонта, чтобы разблокировать.</translation>
    </message>
    <message>
        <source>No locked LUKS component is currently visible on this selected drive.</source>
        <translation>На этом выбранном диске в настоящее время не видно запертого компонента LUKS.</translation>
    </message>
    <message>
        <source>Confirm LUKS unlock</source>
        <translation>Подтвердите разблокировку LUKS</translation>
    </message>
    <message>
        <source>Unlock %1 on %2?

The helper opens a temporary device-mapper mapping with cryptsetup and keeps it open for this recovery session. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>Разблокировать %1 на %2?

Помощник открывает временное картографическое отображение устройства с помощью cryptsetup и сохраняет его открытым для этой сессии восстановления. Парольная фраза проходит через закрытый ключевой файл и никогда не помещается в командные аргументы или журналы.</translation>
    </message>
    <message>
        <source>LUKS unlock</source>
        <translation>Разблокировка LUKS</translation>
    </message>
    <message>
        <source>Unlock LUKS repair target</source>
        <translation>Разблокировать цель ремонта LUKS</translation>
    </message>
    <message>
        <source>Enter the passphrase for %1.

It is sent only to cryptsetup over the helper&apos;s standard input and is never logged or placed on a command line.</source>
        <translation>Введите парольную фразу для %1.

Он отправляется только для шифрования по стандартному входу помощника и никогда не регистрируется или не размещается в командной строке.</translation>
    </message>
    <message>
        <source>Passphrase required</source>
        <translation>Требуется пароль</translation>
    </message>
    <message>
        <source>An empty passphrase was not submitted. Enter the LUKS passphrase or choose Cancel.</source>
        <translation>Пустая фраза не была представлена. Введите парольную фразу LUKS или выберите Отменить.</translation>
    </message>
    <message>
        <source>Unlock keyfile unavailable</source>
        <translation>Unlock keyfile скачать бесплатно</translation>
    </message>
    <message>
        <source>The LUKS passphrase could not be written to a private keyfile in %1; the unlock was not started.</source>
        <translation>Парольная фраза LUKS не могла быть записана в %1; разблокировка не была начата.</translation>
    </message>
    <message>
        <source>Configuration write unavailable</source>
        <translation>Конфигурация Write Unavailable</translation>
    </message>
    <message>
        <source>The edited content could not be written to a private temporary file in %1; the write was not started.</source>
        <translation>Отредактированный контент не может быть записан в частный временный файл в %1; запись не была начата.</translation>
    </message>
    <message>
        <source>ERROR: cannot read the session log %1</source>
        <translation>ERROR: не может читать журнал сеанса %1</translation>
    </message>
    <message>
        <source>Viewing a prior session log (read-only): %1</source>
        <translation>Просмотр журнала предыдущей сессии (только для чтения): %1</translation>
    </message>
    <message>
        <source>Session log list refreshed.</source>
        <translation>Список журналов сеансов обновлен.</translation>
    </message>
    <message>
        <source>Note:</source>
        <translation>Примечание:</translation>
    </message>
    <message>
        <source>Delete session log</source>
        <translation>Исключить журнал сессии</translation>
    </message>
    <message>
        <source>Delete %1 permanently? This cannot be undone.</source>
        <translation>Удалить %1 навсегда Это нельзя отменить.</translation>
    </message>
    <message>
        <source>%1 changed while the confirmation was open; the delete was refused.</source>
        <translation>%1 изменился, когда подтверждение было открыто; в удалении было отказано.</translation>
    </message>
    <message>
        <source>Unable to delete %1.</source>
        <translation>Невозможно удалить %1.</translation>
    </message>
    <message>
        <source>Device discovery filters updated.</source>
        <translation>Обновлены фильтры обнаружения устройств.</translation>
    </message>
    <message>
        <source>Unknown Linux distribution</source>
        <translation>Неизвестный дистрибутив Linux</translation>
    </message>
    <message>
        <source>sudo / gksu (no KAuth on this frontend)</source>
        <translation>sudo / gksu (нет KAuth на этом фронтенде)</translation>
    </message>
    <message>
        <source>Available</source>
        <translation>Доступный</translation>
    </message>
    <message>
        <source>Missing on this frontend</source>
        <translation>Пропавший на этом фронте</translation>
    </message>
    <message>
        <source>Missing</source>
        <translation>Пропавший</translation>
    </message>
    <message>
        <source>No repair tool is selected.</source>
        <translation>Ремонтный инструмент не подбирается.</translation>
    </message>
    <message>
        <source>A helper command is already running.</source>
        <translation>Команда помощников уже работает.</translation>
    </message>
    <message>
        <source>Restore Graphical Login is a host-scope stage on this legacy frontend; enter Host Maintenance to run it.</source>
        <translation>Восстановление графического входа - это этап хоста на этом устаревшем интерфейсе; введите обслуживание хоста, чтобы запустить его.</translation>
    </message>
    <message>
        <source>The selected scope has no resolved root component; use Refresh Devices and commit the repair target again.</source>
        <translation>Выбранная область не имеет решенного корневого компонента; используйте Refresh Devices и снова установите цель ремонта.</translation>
    </message>
    <message>
        <source>This legacy frontend exposes no %1 action; the helper reports the capability as available.</source>
        <translation>Этот унаследованный интерфейс не показывает действия %1; помощник сообщает о возможности по мере ее доступности.</translation>
    </message>
    <message>
        <source>Run this guarded repair action using the cached read-only diagnostic evidence. A confirmation is shown first.</source>
        <translation>Запустите эту охраняемую операцию по ремонту, используя кэшированные диагностические данные только для чтения. Сначала показывается подтверждение.</translation>
    </message>
    <message>
        <source>Enabled in Settings</source>
        <translation>Включено в настройках</translation>
    </message>
    <message>
        <source>Disabled in Settings - enable it to include this stage</source>
        <translation>Инвалиды в настройках - позволяют включить этот этап</translation>
    </message>
    <message>
        <source>Unavailable: %1</source>
        <translation>Недоступно: %1</translation>
    </message>
    <message>
        <source>
Unavailable: %1</source>
        <translation>
Недоступно: %1</translation>
    </message>
    <message>
        <source>Repair tool unavailable</source>
        <translation>Ремонтный инструмент недоступен</translation>
    </message>
    <message>
        <source>Confirm repair</source>
        <translation>Подтвердить ремонт</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Your saved selection is kept and its availability is re-checked when diagnostics for this scope complete.</source>
        <translation>Никаких кэшированных доказательств способности для этого этапа пока нет. Ваш сохраненный выбор сохраняется, и его доступность перепроверяется при завершении диагностики.</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Run diagnostics for the selected scope to populate the Full Repair plan; your selection is saved once the stage becomes available.</source>
        <translation>Никаких кэшированных доказательств способности для этого этапа пока нет. Запустите диагностику выбранной области для заполнения плана полного ремонта; ваш выбор сохраняется, как только этап становится доступным.</translation>
    </message>
    <message>
        <source>Unknown Full Repair stage.</source>
        <translation>Неизвестный этап полного ремонта.</translation>
    </message>
    <message>
        <source>No Full Repair stages are selected or available; use Configure Plan... to choose the stages.</source>
        <translation>Полные этапы ремонта не выбираются или не доступны; используйте план настройки для выбора этапов.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab to establish the session.</source>
        <translation>Требуется разрешение администратора; нажмите Авторизация на вкладке «Системы» или «Ремонт» для создания сессии.</translation>
    </message>
    <message>
        <source>Runs the selected stages using the cached read-only diagnostic evidence after privilege confirmation.</source>
        <translation>Запускает выбранные этапы с использованием кэшированных диагностических данных только для чтения после подтверждения привилегий.</translation>
    </message>
    <message>
        <source>No Full Repair stages selected - use Configure Plan... or Settings.</source>
        <translation>Не выбраны этапы полного ремонта - используйте план настройки или настройки.</translation>
    </message>
    <message>
        <source>1 stage selected</source>
        <translation>1 этап выбранный</translation>
    </message>
    <message>
        <source>%1 stages selected</source>
        <translation>Выбранные стадии %1</translation>
    </message>
    <message>
        <source>No repair stages are selected. Use Configure Plan... to choose the stages Full Repair will run.</source>
        <translation>Стадии ремонта не выбираются. Используйте план настройки ... чтобы выбрать этапы полного ремонта.</translation>
    </message>
    <message>
        <source>No selected stages are available. %1</source>
        <translation>Отобранные этапы отсутствуют. %1</translation>
    </message>
    <message>
        <source>Full Repair unavailable</source>
        <translation>Недоступен полный ремонт</translation>
    </message>
    <message>
        <source>Run the Full Repair plan?

The selected stages run in order through the helper&apos;s guarded repair command:

</source>
        <translation>Запуск плана полного ремонта?

Выбранные этапы проходят в порядке через охраняемую команду ремонта помощника:

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
Помощник выполняет каждый предварительный полет во время выполнения; этап, который терпит неудачу, останавливает план.</translation>
    </message>
    <message>
        <source>Running %1 through the privileged helper... The Logs tab keeps the complete transcript.</source>
        <translation>Запуск %1 через привилегированного помощника На вкладке «Логи» хранится полная расшифровка.</translation>
    </message>
    <message>
        <source>Shell scope required</source>
        <translation>Требуемый объем Shell</translation>
    </message>
    <message>
        <source>No administrator session is active.

Press Authorize on the Systems or Repair tab to establish the session, or enter Host Maintenance / commit a repair target on the Systems tab; the chroot shell then reuses the cached authorization.</source>
        <translation>Сеанс администратора не активен.

Нажмите Авторизовать на вкладке «Системы» или «Ремонт», чтобы установить сеанс, или введите «Поддержание хоста» / совершите целевой ремонт на вкладке «Системы»; оболочка chroot затем повторно использует кэшированную авторизацию.</translation>
    </message>
    <message>
        <source>Shell command required</source>
        <translation>Требуется команда Shell</translation>
    </message>
    <message>
        <source>Enter the command to run first.</source>
        <translation>Введите команду, чтобы бежать первым.</translation>
    </message>
    <message>
        <source>Confirm running-host command</source>
        <translation>Подтвердить команду Runing Host</translation>
    </message>
    <message>
        <source>Run this command as root on the protected running host?

%1

The helper keeps its runtime preflights; the command is passed as one argument and is never interpreted by the GUI.</source>
        <translation>Запустите эту команду как root на защищенном хосте?

%1

Помощник выполняет свои предварительные полеты; команда передается как один аргумент и никогда не интерпретируется GUI.</translation>
    </message>
    <message>
        <source>Running %1...</source>
        <translation>При этом %1...</translation>
    </message>
    <message>
        <source>Authorization scope required</source>
        <translation>Объем требуемой авторизации</translation>
    </message>
    <message>
        <source>administrator authorization</source>
        <translation>авторизация администратора</translation>
    </message>
    <message>
        <source>Administrator authorization</source>
        <translation>Разрешение администратора</translation>
    </message>
    <message>
        <source>Administrator authorization is required for %1.

Enter the password for %2 (sudo). It is used only for this sudo authentication, is sent over a pipe and is never logged or placed on a command line. The authorization is cached for this session and reused by diagnostics and repairs.</source>
        <translation>Для %1 требуется разрешение администратора.

Введите пароль для %2 (sudo). Он используется только для аутентификации sudo, отправляется по трубе и никогда не регистрируется или не размещается в командной строке. Разрешение кэшируется для этой сессии и повторно используется для диагностики и ремонта.</translation>
    </message>
    <message>
        <source>your account</source>
        <translation>Ваш счет</translation>
    </message>
    <message>
        <source>Password required</source>
        <translation>Требуется пароль</translation>
    </message>
    <message>
        <source>An empty password was not submitted. Enter the sudo password or choose Cancel.</source>
        <translation>Пустой пароль не был предоставлен. Введите пароль sudo или выберите Отменить.</translation>
    </message>
    <message>
        <source>Administrator authorization failed</source>
        <translation>Разрешение администратора провалилось</translation>
    </message>
    <message>
        <source>sudo did not accept the password: %1

The command was not started.</source>
        <translation>Sudo не принял пароль %1

Командование не началось.</translation>
    </message>
    <message>
        <source>elevation requires an interactive sudo password; run the smoke as root or after `sudo -S -v` with --elevate &apos;sudo -n&apos;</source>
        <translation>Подъем требует интерактивного пароля sudo; запускайте дым как корень или после «sudo -S -v» с -поднимите «sudo -n»</translation>
    </message>
    <message>
        <source>No administrator session is active for %1.

Press Authorize on the Systems or Repair tab to establish the session now, or enter Host Maintenance / commit a repair target on the Systems tab; diagnostics and repairs then reuse the cached authorization.</source>
        <translation>Сеанс администратора не активен для %1.

Нажмите Авторизовать на вкладке «Системы» или «Ремонт», чтобы установить сеанс сейчас, или введите «Поддержание хоста» / совершите целевой ремонт на вкладке «Системы»; диагностика и ремонт затем повторно используйте кэшированную авторизацию.</translation>
    </message>
    <message>
        <source>Administrator authorization expired</source>
        <translation>Срок полномочий администратора истек</translation>
    </message>
    <message>
        <source>The cached administrator authorization for %1 expired or was refused.

Press Authorize on the Systems or Repair tab to re-establish the session, then run the command again. No command was started.</source>
        <translation>Срок действия разрешения администратора %1 истек или ему было отказано.

Нажмите Авторизовать на вкладке Системы или Ремонт, чтобы восстановить сеанс, а затем снова запустить команду. Командование не было начато.</translation>
    </message>
    <message>
        <source>Privileged operation completed successfully. Administrator authorization remains active for this session.</source>
        <translation>Привилегированная операция успешно завершена. На эту сессию по-прежнему действует разрешение администратора.</translation>
    </message>
    <message>
        <source>Privileged operation stopped with an error. Administrator authorization remains active; review the output before closing.</source>
        <translation>Привилегированная операция прекратилась с ошибкой. Разрешение администратора остается активным; просмотрите результаты до закрытия.</translation>
    </message>
    <message>
        <source>Inspection is read-only.

</source>
        <translation>Инспекция только читается.

</translation>
    </message>
    <message>
        <source>Configuration unavailable</source>
        <translation>Конфигурация недоступна</translation>
    </message>
    <message>
        <source>The helper could not read %1:

%2</source>
        <translation>Помощник не мог прочитать %1:

%2</translation>
    </message>
    <message>
        <source>Configuration write failed</source>
        <translation>Конфигурация писать не удалось</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
Result: mapping opened for this recovery session.</source>
        <translation>Название: Unlocked
Компонент: %1
Маппер: %2
Способ: разблокировка помощника (шифрование; парольная фраза через файл-ключ в режиме-600, удаленный после использования)
Результат: картографирование открылось для этой сессии восстановления.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: the passphrase was not accepted; retry offered.</source>
        <translation>Государство: Заперто
Компонент: %1
Способ: разблокировка помощника (cryptsetup)
Ошибка: парольная фраза не была принята; повторная предлагалась.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: %2</source>
        <translation>Государство: Заперто
Компонент: %1
Способ: разблокировка помощника (cryptsetup)
Ошибка: %2</translation>
    </message>
    <message>
        <source>Passphrase not accepted</source>
        <translation>Перефразирование не принято</translation>
    </message>
    <message>
        <source>The LUKS passphrase was not accepted.

Try again?</source>
        <translation>LUKS не был принят.

Попробуй еще раз?</translation>
    </message>
    <message>
        <source>[-] %1 - not run (the plan stopped before reaching this stage)</source>
        <translation>%1 - не работает (план остановился до достижения этой стадии)</translation>
    </message>
    <message>
        <source>filesystem</source>
        <translation>файловая система</translation>
    </message>
    <message>
        <source>[-] %1 - no file system errors found - no changes</source>
        <translation>%1 - никаких ошибок файловой системы не обнаружено</translation>
    </message>
    <message>
        <source>[FAIL] %1 - not reported</source>
        <translation>%1 - не сообщается</translation>
    </message>
    <message>
        <source>%1 stage(s) failed; review the helper output in Logs.</source>
        <translation>Стадия (стадии) %1 не удалась; просмотрите вывод помощника в журналах.</translation>
    </message>
    <message>
        <source>the plan stopped before any stage completed.</source>
        <translation>План был остановлен до завершения любого этапа.</translation>
    </message>
    <message>
        <source>no repair was needed; cached diagnostics remain valid.</source>
        <translation>Ремонт не требовался, кэш-диагностика остается в силе.</translation>
    </message>
    <message>
        <source>repair completed; cached diagnostics were invalidated and must be regenerated.</source>
        <translation>Ремонт завершен, кэш-диагностика признана недействительной и должна быть восстановлена.</translation>
    </message>
    <message>
        <source>%1 results: [OK] %2 successful | [FAIL] %3 failed | [-] %4 no repair needed - %5</source>
        <translation>Результаты %1: [OK] %2 удачный | [FAIL] %3 не удалось | [-] %4 не требуется ремонт - %5</translation>
    </message>
    <message>
        <source>Re-run Diagnostic</source>
        <translation>Повторная диагностика</translation>
    </message>
    <message>
        <source>Unavailable</source>
        <translation>Недоступный</translation>
    </message>
    <message>
        <source>Commit this physical drive as the repair target.</source>
        <translation>Совершите этот физический драйв в качестве цели ремонта.</translation>
    </message>
    <message>
        <source>Protection:</source>
        <translation>Защита:</translation>
    </message>
    <message>
        <source>Host shell</source>
        <translation>Хранитель снаряда</translation>
    </message>
    <message>
        <source>Run on Host</source>
        <translation>Беги по хосту</translation>
    </message>
    <message>
        <source>Host Shell</source>
        <translation>Разработчик: Shell</translation>
    </message>
    <message>
        <source>The read-only probe found no editable target configuration file in this target. %1</source>
        <translation>Зонд только для чтения не обнаружил редактируемого файла конфигурации цели в этой цели. %1</translation>
    </message>
    <message>
        <source>Not present in the selected target (omitted): %1</source>
        <translation>Не присутствует в выбранной цели (опущена): %1</translation>
    </message>
    <message>
        <source>Run diagnostics to probe which target configuration files exist; the helper&apos;s read-only probe decides the list.</source>
        <translation>Запустите диагностику, чтобы исследовать, какие целевые файлы конфигурации существуют; зонд только для чтения помощника решает список.</translation>
    </message>
    <message>
        <source>Probed read-only by the helper; a saved edit invalidates the cached diagnostics.</source>
        <translation>Пробужденный только для чтения помощником; сохраненное редактирование аннулирует кэшированную диагностику.</translation>
    </message>
    <message>
        <source>Host maintenance: %1</source>
        <translation>Обслуживание хоста: %1</translation>
    </message>
    <message>
        <source>unresolved</source>
        <translation>неразрешенный</translation>
    </message>
    <message>
        <source>Target: %1 + %2</source>
        <translation>Цель: %1 + %2</translation>
    </message>
    <message>
        <source>Host command</source>
        <translation>Команда принимающей стороны</translation>
    </message>
    <message>
        <source>Execute a command on the running host as root.</source>
        <translation>Выполните команду на бегущем хосте в качестве корня.</translation>
    </message>
    <message>
        <source>Execute a command inside the selected repair system as root.</source>
        <translation>Выполните команду внутри выбранной системы ремонта как root.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab (or re-enter Host Maintenance / re-commit the repair target) to authorize this session.</source>
        <translation>Требуется авторизация администратора; нажмите Авторизация на вкладке «Системы или ремонт» (или повторно введите хост-поддержку / повторное выполнение цели ремонта) для авторизации этой сессии.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root on the running host through the helper&apos;s guarded host-shell verb.</source>
        <translation>Запустите одну из рассмотренных команд как корень на бегущем хосте через охраняемый глагол оболочки хоста помощника.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root inside the target chroot through the helper&apos;s guarded shell verb.</source>
        <translation>Запускайте одну проверенную команду как корень внутри целевого корня через охраняемый глагол оболочки помощника.</translation>
    </message>
    <message>
        <source>Run a command on the running host as root (sudo is not needed). Commands are executed directly on the active system; output is kept in this window and in the application log.</source>
        <translation>Запустите команду на бегущем хосте как root (судо не требуется). Команды выполняются непосредственно в активной системе; вывод хранится в этом окне и в журнале приложений.</translation>
    </message>
    <message>
        <source>Run a command inside the selected repair system as root (sudo is not needed). Commands are executed one at a time in a fresh chroot and cannot answer interactive prompts; use non-interactive flags such as apt-get -y upgrade. Output is kept in this window and in the application log.</source>
        <translation>Запустите команду внутри выбранной системы ремонта как root (судо не требуется). Команды выполняются по одной за раз в свежем chroot и не могут отвечать на интерактивные подсказки; используйте неинтерактивные флаги, такие как апгрейд apt-get-y. Выход хранится в этом окне и в журнале приложений.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize or re-enter Host Maintenance / commit a repair target.</source>
        <translation>Никакая сессия администратора не активна; нажмите Авторизовать или повторно войти в Обслуживание хоста / выполнить цель ремонта.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from the repaired system</source>
        <translation>1.Выберите исходные файлы или папки из отремонтированной системы</translation>
    </message>
    <message>
        <source>2. Choose destination on this host</source>
        <translation>2.Выберите пункт назначения на этом хосте</translation>
    </message>
    <message>
        <source>Add File Path...</source>
        <translation>Добавить файл Path...</translation>
    </message>
    <message>
        <source>Add Folder Path...</source>
        <translation>Добавьте папоротник...</translation>
    </message>
    <message>
        <source>Browse...</source>
        <translation>Просмотреть...</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it (Host Maintenance does not provide a repair tree to browse).</source>
        <translation>Выберите ремонтный привод в системах, прежде чем выбрать пункт назначения внутри него (обслуживание хоста не обеспечивает дерево ремонта для просмотра).</translation>
    </message>
    <message>
        <source>Choose a host destination folder directly.</source>
        <translation>Выберите папку назначения хоста напрямую.</translation>
    </message>
    <message>
        <source>Copy the staged files and folders with cp -a, restore ownership with chown --reference and byte-compare every regular file afterwards.</source>
        <translation>Копируйте постановочные файлы и папки с помощью cp -a, восстанавливайте право собственности с помощью chown - ссылка и байт-сравните каждый обычный файл после этого.</translation>
    </message>
    <message>
        <source>File Copy unavailable</source>
        <translation>File Copy недоступен</translation>
    </message>
    <message>
        <source>Choose host destination folder</source>
        <translation>Выберите папку host destination</translation>
    </message>
    <message>
        <source>Select a repair target</source>
        <translation>Выберите цель ремонта</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it.</source>
        <translation>Выберите ремонтный привод в системах, прежде чем выбрать пункт назначения внутри него.</translation>
    </message>
    <message>
        <source>File Copy - Browse Target Folders</source>
        <translation>Оригинальное название: Browse Target Folders</translation>
    </message>
    <message>
        <source>Browse Target Folders</source>
        <translation>Скачать Target Folders</translation>
    </message>
    <message>
        <source>The helper could not list the repair-system folder:

%1</source>
        <translation>Помощник не смог перечислить папку системы ремонта:

%1</translation>
    </message>
    <message>
        <source>BROWSE_ENTRY</source>
        <translation>BROWSE ENTRY</translation>
    </message>
    <message>
        <source>(choose this folder: %1)</source>
        <translation>(Выберите эту папку: %1)</translation>
    </message>
    <message>
        <source>(parent folder)</source>
        <translation>(родительская папка)</translation>
    </message>
    <message>
        <source>Select repair-system destination</source>
        <translation>Выберите пункт назначения ремонтной системы</translation>
    </message>
    <message>
        <source>Choose a destination folder inside the repaired system (current folder: %1):</source>
        <translation>Выберите папку назначения внутри отремонтированной системы (текущая папка: %1):</translation>
    </message>
    <message>
        <source>Folder</source>
        <translation>папка</translation>
    </message>
    <message>
        <source>Open</source>
        <translation>Открыть</translation>
    </message>
    <message>
        <source>(choose this folder:</source>
        <translation>(Выберите эту папку:</translation>
    </message>
    <message>
        <source>Add files to copy</source>
        <translation>Добавить файлы в копирование</translation>
    </message>
    <message>
        <source>Add folder to copy</source>
        <translation>Добавить папку в копирование</translation>
    </message>
    <message>
        <source>Stage at least one source and name a destination path first.</source>
        <translation>Установите хотя бы один источник и сначала назовите путь назначения.</translation>
    </message>
    <message>
        <source>Copy the %1 staged item(s) to %2?

The helper keeps its direction and path containment checks; a sensitive repair-system destination is refused unless the helper approves it, and every regular file is byte-compared after the copy.</source>
        <translation>Копировать %1 постановочный элемент(ы) в %2?

Помощник сохраняет свои проверки направления и пути сдерживания; чувствительный пункт назначения ремонтной системы отклоняется, если помощник не одобряет его, и каждый обычный файл сравнивается с байтом после копии.</translation>
    </message>
    <message>
        <source>File Copy - Copy and Verify</source>
        <translation>Копирование файлов - копирование и проверка</translation>
    </message>
    <message>
        <source>File Copy - Preview Changes</source>
        <translation>Копии файлов - Превью изменений</translation>
    </message>
    <message>
        <source>Protected running host - details</source>
        <translation>Защищенный бегущий хост - подробности</translation>
    </message>
    <message>
        <source>not mounted (offline target)</source>
        <translation>не установленная (оффлайн-цель)</translation>
    </message>
    <message>
        <source>Select a drive to see its details.</source>
        <translation>Выберите диск, чтобы увидеть его детали.</translation>
    </message>
    <message>
        <source>Inspecting the selected component.</source>
        <translation>Проверка выбранного компонента.</translation>
    </message>
    <message>
        <source>Helper-confirmed by the last read-only diagnostics.</source>
        <translation>Подтверждено последней диагностикой только для чтения.</translation>
    </message>
    <message>
        <source>Read-only inventory only; run diagnostics to confirm.</source>
        <translation>Прочитайте только инвентарь; запустите диагностику для подтверждения.</translation>
    </message>
    <message>
        <source>PROTECTED - running system; read-only details only</source>
        <translation>Защищено - запущенная система; только детали для чтения</translation>
    </message>
    <message>
        <source>Live / installer media - not selectable</source>
        <translation>Live / Installer Media - не выбираемый</translation>
    </message>
    <message>
        <source>PROTECTED - running host scope</source>
        <translation>Защищенный - Runing host scope</translation>
    </message>
    <message>
        <source>Unlock required before selection</source>
        <translation>Unlock требуется перед выбором</translation>
    </message>
    <message>
        <source>Eligible repair candidate</source>
        <translation>Кандидат на ремонт</translation>
    </message>
    <message>
        <source>Drive:</source>
        <translation>Водитель:</translation>
    </message>
    <message>
        <source>Detected target:</source>
        <translation>Обнаруженная цель:</translation>
    </message>
    <message>
        <source>Pending inspection</source>
        <translation>Предстоящая проверка</translation>
    </message>
    <message>
        <source>Model / label:</source>
        <translation>Модель / этикетка:</translation>
    </message>
    <message>
        <source>Status:</source>
        <translation>Статус:</translation>
    </message>
    <message>
        <source>Size:</source>
        <translation>Размер:</translation>
    </message>
    <message>
        <source>Connection:</source>
        <translation>Подключение:</translation>
    </message>
    <message>
        <source>Filesystem:</source>
        <translation>Файловая система:</translation>
    </message>
    <message>
        <source>UUID:</source>
        <translation>UUID:</translation>
    </message>
    <message>
        <source>Mounts:</source>
        <translation>Горы:</translation>
    </message>
    <message>
        <source>Running system protection unresolved</source>
        <translation>Защита операционной системы не решена</translation>
    </message>
    <message>
        <source>No protected physical backing disk was identified</source>
        <translation>Защищенный физический бэк-диск не обнаружен</translation>
    </message>
    <message>
        <source>Current running Linux system</source>
        <translation>Текущая работающая система Linux</translation>
    </message>
    <message>
        <source>Critical mounts: %1</source>
        <translation>Критические крепления: %1</translation>
    </message>
    <message>
        <source>Read-only protected-running-system facts: the helper OS fact plus the inventory model, device path, size, transport and critical mounts. Nothing here is probed destructively.</source>
        <translation>Факты только для чтения защищенной системы: факт вспомогательной ОС плюс модель инвентаря, путь устройства, размер, транспорт и критические крепления. Ничто здесь не исследуется разрушительно.</translation>
    </message>
    <message>
        <source>Select a drive to see unlock status.</source>
        <translation>Выберите диск, чтобы увидеть статус разблокировки.</translation>
    </message>
    <message>
        <source>State: protected
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
The protected running host cannot be unlocked or modified; unlock is available for an offline repair target only. Use Host Maintenance for the protected running host.</source>
        <translation>Государство: защищено
Компонент: %1
Маппер: (ничего)
Способ: разблокировка помощника (шифрование; парольная фраза через файл-ключ в режиме-600, удаленный после использования)
Защищенный запущенный хост не может быть разблокирован или изменен; разблокировка доступна только для автономного ремонта. Используйте Host Maintenance для защищенного хоста.</translation>
    </message>
    <message>
        <source>(none detected)</source>
        <translation>(не обнаружено)</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
A locked LUKS container is visible on this drive; press Unlock to open it for this recovery session.</source>
        <translation>Государство: Заперто
Компонент: %1
Маппер: (ничего)
Способ: разблокировка помощника (шифрование; парольная фраза через файл-ключ в режиме-600, удаленный после использования)
На этом диске виден заблокированный контейнер LUKS; нажмите Unlock, чтобы открыть его для этой сессии восстановления.</translation>
    </message>
    <message>
        <source>(visible mapper)</source>
        <translation>(видимый картограф)</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: already open before this session (visible mapper; Boot Bitch will reuse it and will not close it).</source>
        <translation>Название: Unlocked
Компонент: %1
Маппер: %2
Метод: уже открыт перед этой сессией (видимый картограф; Boot Bitch будет повторно использовать его и не будет закрывать его).</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper-confirmed mapping from the last read-only diagnostics.</source>
        <translation>Название: Unlocked
Компонент: %1
Маппер: %2
Метод: подтверждённое помощником отображение из последней диагностики только для чтения.</translation>
    </message>
    <message>
        <source>(mapper)</source>
        <translation>(маппер)</translation>
    </message>
    <message>
        <source>State: locked or no encrypted component detected
Component: (none detected)
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
No locked LUKS component and no unlock operation were recorded for this drive in the current session.</source>
        <translation>Состояние: заблокированный или незашифрованный компонент
Компонент: (не обнаружен)
Маппер: (ничего)
Способ: разблокировка помощника (шифрование; парольная фраза через файл-ключ в режиме-600, удаленный после использования)
Ни один запертый компонент LUKS и ни одна операция разблокировки не были записаны для этого диска в текущей сессии.</translation>
    </message>
    <message>
        <source>unavailable - %1</source>
        <translation>Недоступно - %1</translation>
    </message>
    <message>
        <source>Host maintenance:
%1</source>
        <translation>Обслуживание хоста:
%1</translation>
    </message>
    <message>
        <source>Target:
%1 + %2</source>
        <translation>Цель:
%1 + %2</translation>
    </message>
    <message>
        <source>Authorization required: diagnostics and repairs fail closed until you press Authorize.</source>
        <translation>Требуется разрешение: диагностика и ремонт не выполняются до тех пор, пока вы не нажмете «Разрешение».</translation>
    </message>
    <message>
        <source>Re-establish the privileged helper session for the current scope now. The password is requested in the hidden-input modal and is never logged.</source>
        <translation>Восстановите привилегированную сессию помощника для текущего объема. Пароль запрашивается в модальном режиме скрытого ввода и никогда не регистрируется.</translation>
    </message>
    <message>
        <source>Running...</source>
        <translation>Бежать...</translation>
    </message>
    <message>
        <source>Scope required</source>
        <translation>Область применения</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope; this unlocks the gated actions.</source>
        <translation>Запустите все - запустите каждую доступную диагностику только для чтения для текущего объема; это открывает закрытые действия.</translation>
    </message>
    <message>
        <source>Select a target and wait for any running command first.</source>
        <translation>Сначала выберите цель и дождитесь любой команды.</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic through the helper.</source>
        <translation>Запустите Диагностику - запустите выбранную диагностику только для чтения через помощника.</translation>
    </message>
    <message>
        <source>Run All for the current scope and refresh the read-only backend profile.</source>
        <translation>Запустите All для текущего объема и обновите профиль только для чтения.</translation>
    </message>
    <message>
        <source>Read or edit the selected target configuration file through the guarded helper; a saved edit invalidates cached diagnostics.</source>
        <translation>Прочитайте или отредактируйте выбранный файл конфигурации цели через защищенного помощника; сохраненное редактирование аннулирует кэшированную диагностику.</translation>
    </message>
    <message>
        <source>Host Maintenance has no target-file editing; commit an offline repair target first.</source>
        <translation>Обслуживание хоста не имеет редактирования целевого файла; сначала совершите офлайн-цель ремонта.</translation>
    </message>
    <message>
        <source>Commit an offline repair target first.</source>
        <translation>Сначала выполните офлайн-цель ремонта.</translation>
    </message>
    <message>
        <source>No target configuration file is available for this target; run diagnostics to probe the list.</source>
        <translation>Для этой цели не доступен файл конфигурации цели; запустите диагностику для изучения списка.</translation>
    </message>
    <message>
        <source>Already Unlocked</source>
        <translation>Уже разблокирован</translation>
    </message>
    <message>
        <source>An unlocked Linux filesystem is already visible on this drive. Boot Bitch will reuse the existing mapper and will not close or reopen a mapping created by this recovery session. Mounting happens during diagnostics (read-only) and repairs (read-write); data filesystems are never auto-mounted on selection.</source>
        <translation>На этом диске уже видна разблокированная файловая система Linux. Boot Bitch будет повторно использовать существующий картограф и не будет закрывать или повторно открывать отображение, созданное этой сессией восстановления. Монтаж происходит во время диагностики (только чтение) и ремонта (чтение-запись); файловые системы данных никогда не устанавливаются автоматически при выборе.</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked; use Host Maintenance for the protected running host.</source>
        <translation>Защищенный запущенный хост не может быть разблокирован; используйте обслуживание хоста для защищенного запущенного хоста.</translation>
    </message>
    <message>
        <source>Host Maintenance is the current scope, but the selected offline drive can still be unlocked.</source>
        <translation>Обслуживание хоста - это текущая область, но выбранный автономный диск все еще можно разблокировать.</translation>
    </message>
    <message>
        <source>Unlock %1 using cryptsetup through the privileged helper. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>Разблокируйте %1 с помощью криптозащиты через привилегированного помощника. Парольная фраза проходит через закрытый ключевой файл и никогда не помещается в командные аргументы или журналы.</translation>
    </message>
    <message>
        <source>The protected running system cannot be selected as a repair target; use Host Maintenance for the protected running host.</source>
        <translation>Защищенная работающая система не может быть выбрана в качестве цели ремонта; используйте техническое обслуживание хоста для защищенного работающего хоста.</translation>
    </message>
    <message>
        <source>Live / installer media is read-only boot media and cannot be selected as a repair target.</source>
        <translation>Живые / установочные носители - это загрузочные носители только для чтения и не могут быть выбраны в качестве цели ремонта.</translation>
    </message>
    <message>
        <source>Unlock the encrypted volume first; Select Target becomes available after a Linux filesystem is detected.</source>
        <translation>Сначала разблокируйте зашифрованный том; после обнаружения файловой системы Linux становится доступен Select Target.</translation>
    </message>
    <message>
        <source>Committed repair target. Repair, Diagnostics and File Copy target this physical drive until another drive is explicitly selected with Select Target.</source>
        <translation>Целевой ремонт. Ремонт, диагностика и копирование файлов нацелены на этот физический диск, пока другой диск не будет явно выбран с помощью Select Target.</translation>
    </message>
    <message>
        <source>Commit %1 as the repair target; this leaves Host Maintenance and switches the scope to the selected drive.</source>
        <translation>Включите %1 в качестве цели ремонта; это оставляет обслуживание хоста и переключает область действия на выбранный диск.</translation>
    </message>
    <message>
        <source>The running host target could not be detected.</source>
        <translation>Запускаемая цель хоста не может быть обнаружена.</translation>
    </message>
    <message>
        <source>Leave Host Maintenance and return to repair-target mode.</source>
        <translation>Оставьте обслуживание хоста и вернитесь в режим ремонта-цели.</translation>
    </message>
    <message>
        <source>Select the running host for deliberate guarded maintenance; the administrator authorization is requested here once and cached for the session.</source>
        <translation>Выберите запущенный хост для преднамеренного охраняемого обслуживания; авторизация администратора запрашивается здесь один раз и кэшируется для сессии.</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a repair target.</source>
        <translation>Запуск хост-мишени не может быть обнаружен; диагностика требует ремонта мишени.</translation>
    </message>
    <message>
        <source>Committed target: %1 + %2</source>
        <translation>Цель: %1 + %2</translation>
    </message>
    <message>
        <source>Committed target: none (selection changed)</source>
        <translation>Заявленная цель: нет (отбор изменен)</translation>
    </message>
    <message>
        <source>The selected scope has no resolved Linux root component; Refresh Devices and commit the repair target again.</source>
        <translation>Выбранная область не имеет решенного корневого компонента Linux; Обновите устройства и снова установите цель восстановления.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize on the Systems or Repair tab to re-establish it. Run All reuses the cached authorization and never prompts by itself.</source>
        <translation>Никакая сессия администратора не активна; нажмите Авторизовать систему или Ремонт, чтобы восстановить ее. Run All повторно использует кэшированную авторизацию и никогда не запрашивает самостоятельно.</translation>
    </message>
    <message>
        <source>Run diagnostics for this scope to unlock the gated actions. Diagnostics are read-only and the only evidence source.</source>
        <translation>Запустите диагностику для этой области, чтобы разблокировать закрытые действия. Диагностика только для чтения и единственный источник доказательств.</translation>
    </message>
    <message>
        <source>A repair was not proven unchanged, so the cached diagnostics are invalidated. Run diagnostics again before another gated action.</source>
        <translation>Ремонт не был доказан без изменений, поэтому кэшированная диагностика недействительна. Проведите диагностику еще раз перед очередным закрытым действием.</translation>
    </message>
    <message>
        <source>Gated actions reflect the cached capability lines; the helper still runs every runtime preflight when a command starts.</source>
        <translation>Заданные действия отражают кэшированные линии возможностей; помощник по-прежнему выполняет каждый предварительный полет во время выполнения, когда начинается команда.</translation>
    </message>
    <message>
        <source>Regenerating diagnostics automatically</source>
        <translation>Автоматическая регенерация диагностики</translation>
    </message>
    <message>
        <source>Running diagnostic: %1</source>
        <translation>Диагностика: %1</translation>
    </message>
    <message>
        <source>Running all diagnostics</source>
        <translation>Запуск всех диагностических</translation>
    </message>
    <message>
        <source>Unlocking %1</source>
        <translation>Разблокировка %1</translation>
    </message>
    <message>
        <source>running host</source>
        <translation>бегущий хост</translation>
    </message>
    <message>
        <source>offline target</source>
        <translation>оффлайн цель</translation>
    </message>
    <message>
        <source>host maintenance active</source>
        <translation>Активное техническое обслуживание</translation>
    </message>
    <message>
        <source>repair target committed</source>
        <translation>Цель ремонта</translation>
    </message>
    <message>
        <source>no committed scope</source>
        <translation>не установленный предел</translation>
    </message>
    <message>
        <source>Current session</source>
        <translation>Нынешняя сессия</translation>
    </message>
    <message>
        <source>Log files (*.log);;All files (*)</source>
        <translation>Файлы журнала (*.log); Все файлы (*)</translation>
    </message>
    <message>
        <source>Save log as</source>
        <translation>Сохранить лог как</translation>
    </message>
    <message>
        <source>About Boot Bitch</source>
        <translation>Обсуждение Boot Bitch</translation>
    </message>
    <message>
        <source>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;&lt;p&gt;&lt;b&gt;Developer:&lt;/b&gt; CaptainMorgan12&lt;/p&gt;&lt;p&gt;This GUI is the package&apos;s entry point: a Qt 3.3.x frontend with the ported guarded helper for Debian Etch-era systems.&lt;/p&gt;&lt;p&gt;&lt;b&gt;Guarded repair mode:&lt;/b&gt; read-only diagnostics can inspect either the protected Running Host or an explicitly selected repair drive. The guarded Debian/APT package stages (interrupted configuration, broken dependencies, metadata refresh, upgrade), GRUB-legacy configuration regeneration, LUKS target unlock and guarded target-file editing run through the ported helper after confirmation; Host Maintenance enables the same supported stages natively on the active system after repeating the host identity and boot-mount checks.&lt;/p&gt;&lt;p&gt;The modern-only features - verified File Copy, Btrfs snapshot rollback, EFI/UKI and extlinux repair, boot-stack reconciliation and Make Default - are greyed with the helper&apos;s own probe reasons on this frontend; Arch/Alpine/Fedora package backends stay diagnostics-only here.&lt;/p&gt;&lt;p&gt;The first privileged action authorizes one cached administrator session per scope through a hidden-input modal (the Qt GUI remains unprivileged). It can be ended at any time from File - Lock Administrator Session.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;&lt;p&gt;&lt;b&gt;Разработчик:&lt;/b&gt;КапитанМорган12&lt;/p&gt;&lt;p&gt; Этот графический интерфейс является точкой входа пакета: фронтенд Qt 3.3.x с портированным защищенным помощником для систем эпохи Debian Etch.&lt;/p&gt;&lt;p&gt;&lt;b&gt; Режим защищенного ремонта: &lt;/b&gt; Диагностика только для чтения может проверить либо защищенный Running Host, либо явно выбранный привод для ремонта. Защищенные этапы пакетов Debian/APT (прерванная конфигурация, сломанные зависимости, обновление метаданных, обновление), регенерация конфигурации GRUB-legacy, разблокировка цели LUKS и редактирование защищенных целевых файлов выполняются через портированного помощника после подтверждения; техническое обслуживание хоста позволяет выполнять те же поддерживаемые этапы изначально в активной системе после повторения идентификации хоста и проверки загрузки. Современные функции - проверенная копия файла, откат снимка Btrfs, ремонт EFI / UKI и extlinux, сверка загрузочного стека и по умолчанию - серые с собственными причинами зонда помощника на этом фронтенде; бэкэнды пакета Arch / Alpine / Fedora остаются диагностическими только здесь. &lt;/p&gt;&lt;p&gt; Первое привилегированное действие разрешает одну кэшированную сессию администратора для каждой области через модаль скрытого ввода (Qt GUI остается непривилегированным). Он может быть завершен в любое время с сеанса администратора блокировки.&lt;/p&gt;</translation>
    </message>
</context>
</TS>
