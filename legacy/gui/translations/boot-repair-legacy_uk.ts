<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS language="uk">
<context>
    <name>LegacyMainWindow</name>
    <message>
        <source>Validate environment</source>
        <translation>Діє середовище</translation>
    </message>
    <message>
        <source>Validate</source>
        <translation>Подати заявку</translation>
    </message>
    <message>
        <source>Check the selected system&apos;s mounts, filesystem metadata, boot files, mapper consistency and dependency readiness before any repair action. This is an independent safety preflight rather than an optional Full Repair stage.</source>
        <translation>Перевірити вибрані розміри системи, метадані файлової системи, файли завантаження, консистенцію картографа та готовність залежностей до будь-яких заходів з ремонту. Це незалежне забезпечення безпеки, а не додаткове повне відновлення.</translation>
    </message>
    <message>
        <source>Always preflight</source>
        <translation>Завжди префайт</translation>
    </message>
    <message>
        <source>File system repair</source>
        <translation>Ремонт файлової системи</translation>
    </message>
    <message>
        <source>Check File Systems</source>
        <translation>Перевірка файлових систем</translation>
    </message>
    <message>
        <source>Run the read-only file system check for the selected system&apos;s root and /boot filesystems and report each device&apos;s check tool and result without changing anything. This legacy frontend exposes the read-only check only; device repair is not wired.</source>
        <translation>Запустіть перевірку файлової системи зчитування для кореня вибраної системи та / завантаження файлових систем та звітуйте інструмент перевірки кожного пристрою та не змінюючи нічого. Ця передня частина Legacy виводить тільки перевірку, але ремонт пристрою не ведеться.</translation>
    </message>
    <message>
        <source>Full Repair plan: unavailable on this frontend - read-only check only</source>
        <translation>Повний план ремонту: недоступний на цьому фронті - перевірте тільки</translation>
    </message>
    <message>
        <source>Complete package configuration</source>
        <translation>Комплектація пакету</translation>
    </message>
    <message>
        <source>Complete Configuration</source>
        <translation>Повна конфігурація</translation>
    </message>
    <message>
        <source>Complete interrupted dpkg package configuration in the selected repair system. This is the same stage controlled by Settings -&gt; Full Repair plan -&gt; Complete interrupted package configuration, but it can also be run independently here.</source>
        <translation>конфігурація пакета dpkg в обраній системі ремонту. Це один і той самий етап, який керується налаштуваннями -&gt; План повного ремонту -&gt; Повна конфігурація пакету, але вона також може працювати самостійно тут.</translation>
    </message>
    <message>
        <source>Run the guarded dpkg-configure repair? The helper keeps its package-lock and runtime preflights.</source>
        <translation>Запуск захищеного ремонту dpkg-configure? Допоможитель зберігає свій пакет-розрядний і робочий час.</translation>
    </message>
    <message>
        <source>Repair broken dependencies</source>
        <translation>Ремонт розбитих залежностей</translation>
    </message>
    <message>
        <source>Repair Dependencies</source>
        <translation>Ремонт залежностей</translation>
    </message>
    <message>
        <source>Repair package dependencies in the selected repair system after the mandatory safety preflight. This maps directly to Settings -&gt; Repair broken package dependencies.</source>
        <translation>Ремонт залежності пакету в обраній системі ремонту після обов&apos;язкової безпеки. Карти безпосередньо в налаштуваннях -&gt; Ремонт пошкоджених пакетних залежностей.</translation>
    </message>
    <message>
        <source>Run the guarded fix-broken repair? The helper keeps its simulation-first preflight and runtime guards.</source>
        <translation>Запуск захищеного ремонту фіксатора? Допоможисер зберігає свої симуляції-перший приплив і охоронці робочого часу.</translation>
    </message>
    <message>
        <source>Refresh package metadata</source>
        <translation>метаданих пакетів Refresh</translation>
    </message>
    <message>
        <source>Refresh Metadata</source>
        <translation>Refresh Метадані</translation>
    </message>
    <message>
        <source>Refresh APT metadata in the selected repair system without upgrading installed packages. This maps directly to Settings -&gt; Refresh package metadata.</source>
        <translation>Відновити метадані APT у вибраній системі ремонту без оновлення встановлених пакетів. Карти безпосередньо в налаштуваннях -&gt; метаданих пакетів Refresh.</translation>
    </message>
    <message>
        <source>Refresh package metadata for the selected scope? The helper requires a reachable, trusted APT source.</source>
        <translation>метаданих пакетів Refresh для вибраного обсягу? Помічник вимагає доступного, довіреного джерела APT.</translation>
    </message>
    <message>
        <source>Upgrade installed packages</source>
        <translation>Оновлення встановлених пакетів</translation>
    </message>
    <message>
        <source>Simulate and Upgrade</source>
        <translation>Симулятор і оновлення</translation>
    </message>
    <message>
        <source>Simulate the APT transaction first, inspect proposed removals, then apply a safe upgrade. This maps directly to Settings -&gt; Upgrade installed packages.</source>
        <translation>Симулювати операцію APT першим, оглянути запропоновані видалення, потім застосувати безпечне оновлення. Карти безпосередньо в налаштуваннях -&gt; Оновлення встановлених пакетів.</translation>
    </message>
    <message>
        <source>Run the guarded apt-upgrade transaction? The helper keeps its simulation-first and source guards.</source>
        <translation>Запуск охороняється угода про крадіжку? Помічник зберігає свої симуляції-перших і вихідних охоронців.</translation>
    </message>
    <message>
        <source>DKMS</source>
        <translation>ДКМС</translation>
    </message>
    <message>
        <source>Rebuild DKMS</source>
        <translation>Збудувати DKMS</translation>
    </message>
    <message>
        <source>Rebuild out-of-tree kernel modules for kernels installed in the selected system. The helper refuses this action when DKMS is not installed; this legacy frontend exposes no DKMS action.</source>
        <translation>Збудувати модулі ядра ядер для ядер, встановлених у вибраній системі. Помічник відмовляється від цієї дії, коли DKMS не встановлена; цей фронтенд Legacy розширює дію DKMS.</translation>
    </message>
    <message>
        <source>Graphical login / display manager</source>
        <translation>Графічний логін / диспетчер відображення</translation>
    </message>
    <message>
        <source>Restore Graphical Login</source>
        <translation>Відновлення графічного входу</translation>
    </message>
    <message>
        <source>Restore the legacy SysV display manager configured for the running host: the /etc/X11/default-display-manager entry and the missing runlevel S-symlink, with a backup and rollback, never starting the GUI. This is a host-scope stage on this legacy frontend.</source>
        <translation>Відновити менеджер відображення Legacy SysV налаштований для бігового хосту: запис /etc/X11/default-displaymanager і відсутній рівень S-symlink, з резервним копіюванням і зворотним зв&apos;язком, ніколи не відпускаючи GUI. Це фаза хост-скопа на цій нозі фронтенд.</translation>
    </message>
    <message>
        <source>Restore the graphical login configuration for the running host? The helper backs up /etc/X11/default-display-manager and the runlevel symlink state, restores the configured entry and the missing S-symlink, rolls back on any failure, and never starts the display manager.</source>
        <translation>Відновити графічну конфігурацію логіну для ходового хосту? EnglishDeutschPусский简体中文中國傳統EspañolالعربيةFrançaisελληνικάDanskАнглійскаябългарскиCatalàČeskýEestiSuomiGaeilgeहिन्दीHrvatskiMagyarIndonesiaIcelandicItalianoעברי日本の한국의LietuvosLatvijasмакедонскиMalayMaltiNederlandsNorskPolskiPortuguêsRomânescSlovenskýSlovenskiShqiptarCрпскиSvenskaไทยTürkçeYкраїнськийTiếng việtייִדישKiswahili</translation>
    </message>
    <message>
        <source>Initramfs</source>
        <translation>Інтерамфс</translation>
    </message>
    <message>
        <source>Rebuild Initramfs</source>
        <translation>Збудувати Initramfs</translation>
    </message>
    <message>
        <source>Rebuild initramfs images for the selected repair system only after mapper and crypttab consistency checks pass. The helper backs up each image before the apply. On Etch the stage runs through the guarded plain-chroot fallback (no unshare required).</source>
        <translation>Перебудувати зображення initramfs для вибраної системи ремонту тільки після проходження перевірок консистенції картера та crypttab. Помічник повертає кожне зображення перед застосуванням. На етч-сцену проходить охоронець звичайно-кроотним відступом (не обов&apos;язково).</translation>
    </message>
    <message>
        <source>Rebuild the initramfs for the selected scope? The helper keeps its mapper/crypttab and backup preflights.</source>
        <translation>Перебудувати initramfs для обраного обсягу? Допоможитель зберігає свій картограф / crypttab і резервні копії.</translation>
    </message>
    <message>
        <source>EFI / UKI bootloader</source>
        <translation>EFI / UKI завантажувач</translation>
    </message>
    <message>
        <source>Repair EFI / UKI</source>
        <translation>Ремонт EFI / UKI</translation>
    </message>
    <message>
        <source>Repair the selected system&apos;s EFI / UKI boot path. This legacy frontend exposes no EFI action; the Etch target is a BIOS/GRUB-legacy system.</source>
        <translation>Ремонт виділеної системи EFI / UKI шлях завантаження. Ця передова передня спадщина не передбачає дії EFI, мета Etch – система BIOS/GRUB-legacy.</translation>
    </message>
    <message>
        <source>GRUB configuration</source>
        <translation>Налаштування GRUB</translation>
    </message>
    <message>
        <source>Regenerate GRUB</source>
        <translation>Регенерація GRUB</translation>
    </message>
    <message>
        <source>Regenerate the selected repair system&apos;s GRUB menu/configuration after the mandatory safety preflight. The helper backs up menu.lst, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Відновлення вибраної системи ремонту GRUB меню/конфігурація після обов&apos;язкового забезпечення безпеки. Помічник повертає меню.lst, зберігає всі існуючі записи завантаження та розгортається назад на будь-який момент.</translation>
    </message>
    <message>
        <source>Regenerate the GRUB configuration? The helper backs up the target menu/configuration, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Відновлення конфігурації GRUB? Помічник повертає цільове меню / налаштування, зберігає кожен існуючий запис завантаження та розгортається назад на будь-який збій.</translation>
    </message>
    <message>
        <source>extlinux configuration</source>
        <translation>Налаштування extlinux</translation>
    </message>
    <message>
        <source>Regenerate extlinux</source>
        <translation>Регенерат extlinux</translation>
    </message>
    <message>
        <source>Regenerate the selected system&apos;s extlinux bootloader configuration. This legacy frontend exposes no extlinux action.</source>
        <translation>Відновити обрану конфігурацію завантажувача extlinux. Ця спадщина фронтенду не поширюється на дії extlinux.</translation>
    </message>
    <message>
        <source>Boot stack reconciliation</source>
        <translation>Завантажувальний стенд</translation>
    </message>
    <message>
        <source>Reconcile Boot Stack</source>
        <translation>Reconcile Черевики Stack</translation>
    </message>
    <message>
        <source>Reconcile the selected repair system&apos;s boot stack in one guarded legacy pass: mapper/crypttab validation, initramfs rebuild and GRUB-legacy configuration regeneration, with the component backups and preflights unchanged. This is the Etch equivalent of the modern boot-stack reconciliation and stays out of the Full Repair plan.</source>
        <translation>Здійснити завантаження виділеної системи ремонту в одному захищеному проходженні спадщини: реконструкція картера/crypttab, реконструкція initramfs та регенерація конфігурації GRUB-legacy, з резервними копіями компонентів та прожекторами незмінно. Це еквівалентний Etch сучасної завантажувальної конденсації і залишається з плану повного ремонту.</translation>
    </message>
    <message>
        <source>Full Repair plan: Manual recovery tool</source>
        <translation>Повний план ремонту: Ручний інструмент відновлення</translation>
    </message>
    <message>
        <source>Run the guarded boot-stack reconciliation? The helper runs the mapper/crypttab validation, the initramfs rebuild and the GRUB-legacy regeneration in one pass with every component preflight and backup.</source>
        <translation>Запустіть захищене завантаження-стека? Допоможець веде перевірку на картографі / crypttab, реставрацію initramfs і регенерацію GRUB-legacy в одному проходженні з кожним компонентом і резервним копіюванням.</translation>
    </message>
    <message>
        <source>Complete interrupted package configuration</source>
        <translation>конфігурація пакета повного переривання</translation>
    </message>
    <message>
        <source>Repair broken package dependencies</source>
        <translation>Ремонт пошкоджених пакетних залежностей</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>За замовчуванням</translation>
    </message>
    <message>
        <source>Repair file system errors (read-only check first)</source>
        <translation>Перевірити помилки файлової системи</translation>
    </message>
    <message>
        <source>the legacy frontend exposes the read-only file system check only; per-device repair is not wired on this frontend (fail closed)</source>
        <translation>передоплатник Legacy виводить лише перевірку файлової системи, що читаються, а ремонт не ведеться на цьому фронті.</translation>
    </message>
    <message>
        <source>Upgrade installed packages (adaptive APT simulation)</source>
        <translation>Оновлення встановлених пакетів (адептивне моделювання APT)</translation>
    </message>
    <message>
        <source>Rebuild DKMS modules</source>
        <translation>Rebuild DKMS модулі</translation>
    </message>
    <message>
        <source>Restore graphical login manager</source>
        <translation>Відновлення графічного менеджера</translation>
    </message>
    <message>
        <source>Rebuild initramfs after mapper/crypttab validation</source>
        <translation>Rebuild initramfs після перевірки картпера/crypttab</translation>
    </message>
    <message>
        <source>Repair EFI / UKI boot path</source>
        <translation>Ремонт EFI / UKI шлях завантаження</translation>
    </message>
    <message>
        <source>Update GRUB configuration</source>
        <translation>Оновлення конфігурації GRUB</translation>
    </message>
    <message>
        <source>Update extlinux configuration</source>
        <translation>Оновлення конфігурації extlinux</translation>
    </message>
    <message>
        <source>Choose Full Repair stages in Settings. Enabled stages run in the order shown. Each configurable stage also appears below as an individual tool; the Full Repair column mirrors its current Settings state. The boot tools (EFI / UKI bootloader, GRUB or extlinux configuration, boot-stack reconciliation and Make Default) are independent: run them in any order, and a later action re-verifies what an earlier one changed and reports its own result. The active scope is shown beside Repair: selected repair drive or Running Host maintenance.</source>
        <translation>Виберіть етапи повного ремонту в налаштуваннях. Увімкнути етапи виконання замовлення наведено. Кожна конфігурована сцена також з&apos;являється нижче як окремий інструмент; Повна колонка відновлення відображає її поточний стан налаштувань. Завантажувальні інструменти (EFI / UKI Bootloader, GRUB або extlinux конфігурація, завантажувальну стійку примирення і зробити за замовчуванням) незалежні: запускати їх в будь-якому порядку, а пізніша дія повторює те, що раніше змінено і звітує власний результат. В активному об&apos;ємі відводиться ремонт: обраний ремонтний привід або обслуговування пускових установок.</translation>
    </message>
    <message>
        <source>Run All diagnostics for the selected target or running host before starting Full Repair. The report is read-only evidence used to choose and confirm repair stages.</source>
        <translation>Виконайте всю діагностику для обраного цільового або ходового господаря перед початком повного ремонту. Репортаж, що використовується для вибору та підтвердження етапів ремонту.</translation>
    </message>
    <message>
        <source>Ready: required cached read-only diagnostics are available for the selected stages. Review them in Diagnostics or Logs before confirming.</source>
        <translation>Готовність: для обраних етапів доступна додаткова діагностика кешування. Перегляньте їх в Діагностика або журнали перед підтвердженням.</translation>
    </message>
    <message>
        <source>Environment validation</source>
        <translation>Перевірка навколишнього середовища</translation>
    </message>
    <message>
        <source>Summarizes the selected system, protection state, mounted identity and inspection readiness.</source>
        <translation>Summarizes вибраної системи, стану захисту, встановленої ідентичності та перевірки готовності.</translation>
    </message>
    <message>
        <source>Distribution and boot backend profile</source>
        <translation>Розподіл та завантаження профілю</translation>
    </message>
    <message>
        <source>Identifies the distribution family, package manager, initramfs generator, bootloader and current guarded repair capability.</source>
        <translation>Визначає родину дистрибуції, менеджера пакетів, генератора initramfs, завантаження та поточної працездатності.</translation>
    </message>
    <message>
        <source>Boot diagnostics</source>
        <translation>Діагностика взуття</translation>
    </message>
    <message>
        <source>Shows boot mounts and /boot contents plus storage evidence without changing the selected system.</source>
        <translation>Показати завантаження кріплень та / завантаження вмісту плюс дані для зберігання без зміни вибраної системи.</translation>
    </message>
    <message>
        <source>Boot evidence and selection history</source>
        <translation>Історія завантаження</translation>
    </message>
    <message>
        <source>Correlates the detected boot chain, bootloader selection, kernel/initramfs and unlock evidence.</source>
        <translation>Корельує виявлені ланцюжки завантаження, вибір завантажувача, ядро/initramfs і розблокування доказів.</translation>
    </message>
    <message>
        <source>Kernel / initramfs</source>
        <translation>Кернел / initramfs</translation>
    </message>
    <message>
        <source>Reviews kernel files and verifies matching initramfs images through a read-only inspection.</source>
        <translation>Перегляньте файли з ядра та межі, що відповідають зображенням initramfs через анонімний огляд.</translation>
    </message>
    <message>
        <source>Reviews the GRUB configuration without changing boot files.</source>
        <translation>Перегляньте налаштування GRUB без зміни файлів завантаження.</translation>
    </message>
    <message>
        <source>EFI / UKI boot state</source>
        <translation>EFI / UKI завантажувальний стан</translation>
    </message>
    <message>
        <source>Inspects EFI/UKI evidence; unavailable on this legacy BIOS frontend with the helper&apos;s probe reason.</source>
        <translation>Inspects EFI/UKI докази; недоступні на цьому Legacy BIOS фронтенд з причиною ймовірності помічника.</translation>
    </message>
    <message>
        <source>Reviews the configured display manager and recent boot evidence without starting the GUI.</source>
        <translation>Перегляньте настрочений менеджер відображення та останні дані завантаження без запуску GUI.</translation>
    </message>
    <message>
        <source>Boot errors</source>
        <translation>Похибки завантаження</translation>
    </message>
    <message>
        <source>Reads recent error-priority entries from the running host or selected repair system when available.</source>
        <translation>Читає останні записи про похибку-пріоритетності з ходового хосту або вибраної системи ремонту при наявності.</translation>
    </message>
    <message>
        <source>Disk usage</source>
        <translation>Використання диска</translation>
    </message>
    <message>
        <source>Summarizes filesystem capacity and free space for the running host or read-only repair target.</source>
        <translation>Суммаризує продуктивність файлової системи та вільний простір для ходового хосту або наочно відремонтувати ціль.</translation>
    </message>
    <message>
        <source>File systems</source>
        <translation>Файлові системи</translation>
    </message>
    <message>
        <source>Runs the read-only file system check for the selected system&apos;s root, /boot and other filesystems.</source>
        <translation>Запускає перевірку файлової системи для кореня вибраної системи, / завантаження та інших файлових систем.</translation>
    </message>
    <message>
        <source>/etc/fstab review</source>
        <translation>Огляд /etc/fstab</translation>
    </message>
    <message>
        <source>Displays the running host or selected repair system&apos;s fstab; repair-system inspection is mounted read-only.</source>
        <translation>Відображення ходового хосту або виділеної системи ремонту; контрольно-системної перевірки монтується читально.</translation>
    </message>
    <message>
        <source>Btrfs status</source>
        <translation>Статус на сервери</translation>
    </message>
    <message>
        <source>Shows Btrfs filesystem and subvolume information when the target uses Btrfs.</source>
        <translation>Показати файлову систему Btrfs та інформацію про об’єм при використанні цілі Btrfs.</translation>
    </message>
    <message>
        <source>Device-mapper ancestry</source>
        <translation>Пристрої-максперові вироби</translation>
    </message>
    <message>
        <source>Shows selected mapper ancestry and device-mapper state when available.</source>
        <translation>Покажіть вибраний стан картпера і стан пристрою-картпера при наявності.</translation>
    </message>
    <message>
        <source>LUKS / crypttab evidence</source>
        <translation>LUKS / crypttab докази</translation>
    </message>
    <message>
        <source>Shows LUKS/mapped ancestry plus crypttab and fstab mapper references.</source>
        <translation>Shows LUKS/збиті ancestry плюс crypttab і fstab картографічні посилання.</translation>
    </message>
    <message>
        <source>Full diagnostic report</source>
        <translation>Повний діагностичний звіт</translation>
    </message>
    <message>
        <source>Combines all read-only diagnostics for the selected scope (same as Run All).</source>
        <translation>Комбінує всю діагностику для обраного обсягу (наприклад, Run All).</translation>
    </message>
    <message>
        <source>All entries</source>
        <translation>Всі записи</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>Діагностика</translation>
    </message>
    <message>
        <source>Repairs</source>
        <translation>Ремонт</translation>
    </message>
    <message>
        <source>Package repair</source>
        <translation>Ремонт упаковки</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Файл копіювання</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Відкриття пристрою</translation>
    </message>
    <message>
        <source>Host</source>
        <translation>Проживання</translation>
    </message>
    <message>
        <source>Required for block-device inventory</source>
        <translation>Потрібні для інвентаризації блоків</translation>
    </message>
    <message>
        <source>Filesystem identification</source>
        <translation>Ідентифікація файлової системи</translation>
    </message>
    <message>
        <source>Used to identify filesystem metadata</source>
        <translation>Використовується для виявлення метаданих файлової системи</translation>
    </message>
    <message>
        <source>Mount inspection</source>
        <translation>Перевірка монтажу</translation>
    </message>
    <message>
        <source>Used to understand active mounts</source>
        <translation>Використовується для розуміння активних кріплень</translation>
    </message>
    <message>
        <source>LUKS support</source>
        <translation>Підтримка LUKS</translation>
    </message>
    <message>
        <source>Required to unlock encrypted targets</source>
        <translation>Необхідно розблокувати зашифровані цілі</translation>
    </message>
    <message>
        <source>Btrfs support</source>
        <translation>Підтримка Btrfs</translation>
    </message>
    <message>
        <source>Required for Btrfs inspection and snapshot rollback</source>
        <translation>Необхідно для перевірки Btrfs і зворотного знімку</translation>
    </message>
    <message>
        <source>Bidirectional file copy</source>
        <translation>Копіювати файл</translation>
    </message>
    <message>
        <source>Host/Repair</source>
        <translation>Хост/Ремонт</translation>
    </message>
    <message>
        <source>Required for verified Host-to-Repair and Repair-to-Host transfer</source>
        <translation>Обов&apos;язкові для перевірки Хост-до-Ремонт і Ремонт-до-Host переказу</translation>
    </message>
    <message>
        <source>Chroot repair</source>
        <translation>Хроот ремонт</translation>
    </message>
    <message>
        <source>Required for target-side repair commands</source>
        <translation>Обов&apos;язкові для команд з технічного обслуговування</translation>
    </message>
    <message>
        <source>Offline systemd repair</source>
        <translation>Ремонт автономних систем</translation>
    </message>
    <message>
        <source>Used to restore graphical.target and the configured display manager without starting the target GUI</source>
        <translation>Використовується для відновлення графіки. цільовий і налаштований менеджер відображення без запуску цільового GUI</translation>
    </message>
    <message>
        <source>UEFI NVRAM inspection</source>
        <translation>UEFI NVRAM огляд</translation>
    </message>
    <message>
        <source>Used to preserve target EFI BootOrder during TUXEDO UKI rebuilds when efivars are available</source>
        <translation>Використовується для збереження цільових EFI BootOrder під час TUXEDO UKI перебудує при наявності efivars</translation>
    </message>
    <message>
        <source>UKI verification</source>
        <translation>Перевірка UKI</translation>
    </message>
    <message>
        <source>Used to verify the kernel embedded in a rebuilt unified kernel image</source>
        <translation>Використовується для перевірки ядер, вбудованого в перебудований об&apos;єм ядра</translation>
    </message>
    <message>
        <source>GRUB EFI repair</source>
        <translation>GRUB EFI ремонт</translation>
    </message>
    <message>
        <source>Target/Host</source>
        <translation>Цільова/Хост</translation>
    </message>
    <message>
        <source>Required only for conventional GRUB-based EFI systems</source>
        <translation>Необхідно тільки для звичайних систем GRUB на основі EFI</translation>
    </message>
    <message>
        <source>Target</source>
        <translation>Мета</translation>
    </message>
    <message>
        <source>Debian-family GRUB helper</source>
        <translation>Debian-family GRUB помічник</translation>
    </message>
    <message>
        <source>Portable GRUB configuration generator used by Arch and other non-Debian systems</source>
        <translation>Портативний конфігураційний генератор GRUB, який використовується аркою та іншими неглибими системами</translation>
    </message>
    <message>
        <source>Initramfs rebuild</source>
        <translation>Initramfs rebuild</translation>
    </message>
    <message>
        <source>Debian-family initramfs helper</source>
        <translation>Debian-family initramfs помічник</translation>
    </message>
    <message>
        <source>Arch-family initramfs generator</source>
        <translation>Генератор Arch-family initramfs</translation>
    </message>
    <message>
        <source>Alternative initramfs generator used by Arch and other distributions</source>
        <translation>Альтернативний генератор initramfs, що використовується аркою та іншими розподілами</translation>
    </message>
    <message>
        <source>Initramfs verification</source>
        <translation>Перевірка Initramfs</translation>
    </message>
    <message>
        <source>Read-only verification for mkinitcpio images</source>
        <translation>Перевірка для mkinitcpio зображень</translation>
    </message>
    <message>
        <source>Read-only verification for dracut images</source>
        <translation>Перевірка для dracut зображень</translation>
    </message>
    <message>
        <source>systemd-boot inspection</source>
        <translation>Перевірка systemd-boot</translation>
    </message>
    <message>
        <source>Host/Target</source>
        <translation>Хост/Таргет</translation>
    </message>
    <message>
        <source>Read-only inspection of systemd-boot and generic UKI layouts</source>
        <translation>Читання-тільки перевірка макетів systemd-boot та генних UKI</translation>
    </message>
    <message>
        <source>Arch package manager</source>
        <translation>Архивний менеджер</translation>
    </message>
    <message>
        <source>Arch-family package database and transaction tool</source>
        <translation>Інструмент для баз даних та транзакцій Arch-family</translation>
    </message>
    <message>
        <source>DKMS rebuild</source>
        <translation>DKMS перебудувати</translation>
    </message>
    <message>
        <source>Required only when target uses DKMS modules</source>
        <translation>Необхідно тільки при використанні модулів DKMS</translation>
    </message>
    <message>
        <source>LVM inspection</source>
        <translation>ЛВМ огляд</translation>
    </message>
    <message>
        <source>Optional LVM storage-stack support</source>
        <translation>Додаткова підтримка LVM</translation>
    </message>
    <message>
        <source>Software RAID</source>
        <translation>Програмне забезпечення RAID</translation>
    </message>
    <message>
        <source>Optional Linux MD RAID support</source>
        <translation>Додаткова підтримка RAID Linux MD</translation>
    </message>
    <message>
        <source>Process namespace isolation</source>
        <translation>Промислова ізоляція</translation>
    </message>
    <message>
        <source>Host+Target</source>
        <translation>Зареєструватися Мета</translation>
    </message>
    <message>
        <source>The ported helper falls back to a guarded plain chroot when unshare is absent</source>
        <translation>Зібраний помічник падає назад до захищеного звичайного хрота, коли неготливий</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Зареєструватися</translation>
    </message>
    <message>
        <source>OK</source>
        <translation>Зареєструватися</translation>
    </message>
    <message>
        <source>Yes</source>
        <translation>Я</translation>
    </message>
    <message>
        <source>No</source>
        <translation>Ні</translation>
    </message>
    <message>
        <source>read-only helper diagnostic</source>
        <translation>діагностика діагностичного засобу</translation>
    </message>
</context>
<context>
    <name>legacy::LegacyMainWindow</name>
    <message>
        <source>Boot Bitch Legacy (Etch / KDE 3.5 era)</source>
        <translation>Boot Bitch Legacy (Etch / KDE 3.5 е)</translation>
    </message>
    <message>
        <source>Linux recovery and boot-repair utility</source>
        <translation>Linux відновлення та завантаження</translation>
    </message>
    <message>
        <source>GUARDED REPAIR</source>
        <translation>ЗАМОВИТИ РЕМОНТ</translation>
    </message>
    <message>
        <source>Ordinary repairs require an explicitly selected non-host target. The protected running host has a separate deliberate maintenance mode with the same guarded repair stages and requires privilege authorization.</source>
        <translation>Ординарні ремонти вимагають явно вибраного непристойного призначення. Захищений ведучий має окремий режим технічного обслуговування з тими самими охороняючими етапами ремонту і вимагає авторизації привілеїв.</translation>
    </message>
    <message>
        <source>Systems</source>
        <translation>Системи</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>Діагностика</translation>
    </message>
    <message>
        <source>Repair</source>
        <translation>Ремонт</translation>
    </message>
    <message>
        <source>Chroot Shell</source>
        <translation>Хроот Shell</translation>
    </message>
    <message>
        <source>File Copy</source>
        <translation>Файл Копіювання</translation>
    </message>
    <message>
        <source>Logs</source>
        <translation>Логін</translation>
    </message>
    <message>
        <source>Settings</source>
        <translation>Налаштування</translation>
    </message>
    <message>
        <source>(not created yet)</source>
        <translation>(не створене ще)</translation>
    </message>
    <message>
        <source>Information</source>
        <translation>Інформація</translation>
    </message>
    <message>
        <source>Close</source>
        <translation>Закрити</translation>
    </message>
    <message>
        <source>&amp;File</source>
        <translation>&amp;Файл</translation>
    </message>
    <message>
        <source>&amp;Refresh Devices</source>
        <translation>&amp;Бібліотечні пристрої</translation>
    </message>
    <message>
        <source>&amp;Lock Administrator Session</source>
        <translation>&amp;Засідання адміністратора блокування</translation>
    </message>
    <message>
        <source>&amp;Quit</source>
        <translation>&amp;Кайт</translation>
    </message>
    <message>
        <source>&amp;View</source>
        <translation>&amp;Переглянути</translation>
    </message>
    <message>
        <source>&amp;Systems</source>
        <translation>&amp;Системи</translation>
    </message>
    <message>
        <source>&amp;Diagnostics</source>
        <translation>&amp;Діагностика</translation>
    </message>
    <message>
        <source>&amp;Logs</source>
        <translation>&amp;Логи</translation>
    </message>
    <message>
        <source>&amp;Settings</source>
        <translation>&amp;Налаштування</translation>
    </message>
    <message>
        <source>&amp;Auto-size Device Columns</source>
        <translation>&amp;Авторозмірні колонки</translation>
    </message>
    <message>
        <source>&amp;Wrap Log Lines</source>
        <translation>&amp;Обгортання рядків</translation>
    </message>
    <message>
        <source>&amp;Help</source>
        <translation>&amp;Допомога</translation>
    </message>
    <message>
        <source>&amp;Using Boot Bitch</source>
        <translation>Використання &amp;Boot Bitch</translation>
    </message>
    <message>
        <source>&amp;About Boot Bitch</source>
        <translation>Про &amp;Boot Bitch</translation>
    </message>
    <message>
        <source>Device columns auto-sized. Drag headers to fine-tune widths.</source>
        <translation>Пристрої стовпчики автоматично розмірні. Перетягніть головки до дрібно-ніжних широт.</translation>
    </message>
    <message>
        <source>A helper command is running; wait for it to finish before locking the session.</source>
        <translation>Команда помічника працює, чекаючи на нього, щоб закінчити до блокування сеансу.</translation>
    </message>
    <message>
        <source>Administrator session locked; the next privileged action will request authorization.</source>
        <translation>Заблоковано сеанс адміністратора, наступну привілейовану дію вимагатиме авторизації.</translation>
    </message>
    <message>
        <source>Using Boot Bitch</source>
        <translation>Використання Boot Bitch</translation>
    </message>
    <message>
        <source>Boot Bitch must run from a different booted Linux environment than the system being repaired. Use a Linux live medium or another Linux installation on a different physical drive.&lt;br&gt;&lt;br&gt;The running host is protected from ordinary repair-target selection, but it can be explicitly selected through &lt;b&gt;Host Maintenance&lt;/b&gt; for guarded native diagnostics and supported maintenance stages.&lt;br&gt;&lt;br&gt;Diagnostics follow the Systems page: the committed repair drive while Host Maintenance is off, or the protected running host while it is active.&lt;br&gt;&lt;br&gt;The first privileged action requests administrator authorization once for this Boot Bitch window; &lt;b&gt;File - Lock Administrator Session&lt;/b&gt; ends that helper session immediately. Every repair keeps the helper&apos;s own runtime preflights.</source>
        <translation>Boot Bitch повинен працювати з різних завантажених Linux середовища, ніж система, яка буде відновлена. Використовуйте Linux живу середню або іншу установку Linux на іншому фізичному диску.&lt;br&gt;&lt;br&gt; Ведуться роботи з звичайного вибору зубного знака, але його можна явно вибрати через &lt;b&gt;Host Обслуговування&lt;/b&gt; для охорони рідної діагностики та підтримуваних етапів технічного обслуговування.&lt;br&gt;&lt;br&gt;&lt;br&gt; Діагностика слідувати за сторінкою Системи: скоєний ремонт при проведенні технічного обслуговування, або захищений ходовий господар, при цьому він активний.&lt;br&gt;&lt;br&gt;&lt;br&gt;&lt;br&gt; Перша привілейована авторизація запитів адміністратора після цього вікна Boot Bitch; &lt;b&gt;File - Сесія адміністратора блокування&lt;/b&gt; закінчується, що помічник негайно. Кожен ремонт зберігає власні фари маніпулятора.</translation>
    </message>
    <message>
        <source>Select a physical drive; Boot Bitch resolves the most likely Linux system volume automatically. The running host stays protected from ordinary target repairs, with a separate explicit host-maintenance path for its own system. The Details button shows the protected host&apos;s facts in the details pane; selecting any drive row restores the per-drive pane.</source>
        <translation>Виберіть фізичний диск; Boot Bitch вирішує найбільш ймовірний обсяг системи Linux автоматично. Пробіг ходового хосту залишається захищеним від звичайного капітального ремонту, з окремим явним стеблом забудови для власної системи. Кнопка деталі показує захищені факти хосту в деталях сковорідки; вибравши будь-який дисковий ряд відновлює запірну каструлю.</translation>
    </message>
    <message>
        <source>Refresh Devices</source>
        <translation>Refresh Пристрої</translation>
    </message>
    <message>
        <source>Re-read the read-only kernel inventory (/proc/partitions, /proc/mounts, /proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* and the udev metadata database). No block device is opened and nothing is written.</source>
        <translation>Перечитайте винахідники ядра (/proc/partitions, /proc/mounts, /proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* та бази даних метаданих udev). Відкрито і нічого не записано.</translation>
    </message>
    <message>
        <source>[OK]</source>
        <translation>[OK]</translation>
    </message>
    <message>
        <source>The running system was detected and stays protected from ordinary target repairs.</source>
        <translation>Виявлено та залишатися захищеним від звичайного ремонту цілей.</translation>
    </message>
    <message>
        <source>Detecting running system...</source>
        <translation>Виявлення ходової системи ...</translation>
    </message>
    <message>
        <source>Detecting protected storage...</source>
        <translation>Виявлення захищеного зберігання...</translation>
    </message>
    <message>
        <source>PROTECTED</source>
        <translation>ПРОТЕКЦІЯ</translation>
    </message>
    <message>
        <source>The running host remains protected from ordinary repair-target operations.</source>
        <translation>Захищаючи від звичайних ремонтних робіт.</translation>
    </message>
    <message>
        <source>Details</source>
        <translation>Детальніше</translation>
    </message>
    <message>
        <source>Show read-only details for the protected running host.</source>
        <translation>Показати деталі для захищеного бігу.</translation>
    </message>
    <message>
        <source>Host Maintenance</source>
        <translation>Обслуговування</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>За замовчуванням</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host (menu.lst default directive with a backup and rollback). Requires Host Maintenance and the cached host-default probe.</source>
        <translation>Зробіть запис канонічного встановленого ядра за замовчуванням GRUB-legacy завантажувальний запис на ходовому хості (ручний прямий за замовчуванням з резервним копіюванням та розвантаженням). Потрібні послуги хосту та розкладний хост-захист.</translation>
    </message>
    <message>
        <source>Available repair targets</source>
        <translation>Доступні цілі ремонту</translation>
    </message>
    <message>
        <source>Most likely first</source>
        <translation>Найімовірніше</translation>
    </message>
    <message>
        <source>Drives are listed by the read-only inventory. Select a row to inspect it; Select Target commits the selected non-host drive with its auto-resolved Linux root component.</source>
        <translation>Приводи зазначені чителювальним інвентарем. Виберіть рядок, щоб перевірити його; Виберіть Ціль, координує вибраний непристойний диск з його автоматично вилученим компонентом кореня Linux.</translation>
    </message>
    <message>
        <source>Device</source>
        <translation>Пристрої</translation>
    </message>
    <message>
        <source>Size</source>
        <translation>Розмір</translation>
    </message>
    <message>
        <source>Type</source>
        <translation>Тип</translation>
    </message>
    <message>
        <source>Filesystem</source>
        <translation>Файлова система</translation>
    </message>
    <message>
        <source>Select Target</source>
        <translation>Виберіть Ціль</translation>
    </message>
    <message>
        <source>Unlock</source>
        <translation>Розблокувати</translation>
    </message>
    <message>
        <source>Authorize</source>
        <translation>Авторизація</translation>
    </message>
    <message>
        <source>Establish the privileged helper session for the current scope now instead of waiting for the next privileged action.</source>
        <translation>Налаштуйте привілейовану довідку на поточний обсяг, а не чекаючи наступної привілеї.</translation>
    </message>
    <message>
        <source>Committed target: none</source>
        <translation>Призначення товару: немає</translation>
    </message>
    <message>
        <source>Unlock state for the selected drive; the LUKS passphrase is never logged.</source>
        <translation>Розблокувати стан для вибраного приводу; LUKS passphrase ніколи не записується.</translation>
    </message>
    <message>
        <source>Unlock status</source>
        <translation>Статус на сервери</translation>
    </message>
    <message>
        <source>Read-only inventory plus helper-confirmed facts; mirrors the modern Qt6 Selected drive details panel.</source>
        <translation>Готовий інвентар плюс помічник-підтверджених фактів; дзеркала сучасного Qt6 Вибране панель деталей приводу.</translation>
    </message>
    <message>
        <source>Selected drive details</source>
        <translation>Вибрані деталі приводу</translation>
    </message>
    <message>
        <source>Field</source>
        <translation>Сфера</translation>
    </message>
    <message>
        <source>Value</source>
        <translation>Ціна</translation>
    </message>
    <message>
        <source>Run All runs every available read-only diagnostic for the current scope; selecting a check runs it alone. Diagnostics are read-only and are the only evidence source for the gated repair actions.</source>
        <translation>Виконайте всі всі доступні чителювально-діагностичні для поточної сфери; вибравши його самостійно. Діагностика читаються і є єдиним джерелом доказів для здійснення ремонту воріт.</translation>
    </message>
    <message>
        <source>Target: none selected</source>
        <translation>Мета: ніхто не обраний</translation>
    </message>
    <message>
        <source>Diagnostics follow the committed repair target, or the protected running host while Host Maintenance is active.</source>
        <translation>Діагностика слідувати за вказаним ремонтом або захищеним ходовим хостом під час роботи.</translation>
    </message>
    <message>
        <source>Run All</source>
        <translation>Всі</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope.</source>
        <translation>Запустіть всі - запустіть кожну наявну діагностику для поточного обсягу.</translation>
    </message>
    <message>
        <source>Target configuration:</source>
        <translation>Призначення:</translation>
    </message>
    <message>
        <source>Etch-era target configuration files; availability is probed read-only by the helper&apos;s diagnostics.</source>
        <translation>Цільові файли конфігураційної конфігурації Etch-era; доступність полягає в тому, щоб читати он-лайн діагностиці помічника.</translation>
    </message>
    <message>
        <source>Edit Target File...</source>
        <translation>Редагувати цільовий файл ...</translation>
    </message>
    <message>
        <source>Runs one read-only diagnostic for the selected scope through the helper (`diagnose &lt;key&gt;` / `host-diagnose &lt;key&gt;`); Run All is the combined report.</source>
        <translation>Запускає одну чителювальну діагностику для обраного об’єкту через помічник (діагностування &lt;key&gt;` / `host-diagnose &lt;key&gt;`); Запуск Все це комбінований звіт.</translation>
    </message>
    <message>
        <source>Diagnostic checks</source>
        <translation>Діагностика перевірок</translation>
    </message>
    <message>
        <source>Check</source>
        <translation>Зареєструватися</translation>
    </message>
    <message>
        <source>Selected diagnostic</source>
        <translation>Вибрані діагностичні</translation>
    </message>
    <message>
        <source>Select a diagnostic</source>
        <translation>Виберіть діагноз</translation>
    </message>
    <message>
        <source>Choose a diagnostic from the list.</source>
        <translation>Виберіть діагностику зі списку.</translation>
    </message>
    <message>
        <source>Ready</source>
        <translation>Про нас</translation>
    </message>
    <message>
        <source>Results</source>
        <translation>Результати</translation>
    </message>
    <message>
        <source>Run Diagnostic</source>
        <translation>Діагностика</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic for the current scope.</source>
        <translation>Запустіть діагностику - запустіть вибрану діагностику для поточного обсягу.</translation>
    </message>
    <message>
        <source>Copy Results</source>
        <translation>Статус на сервери</translation>
    </message>
    <message>
        <source>Save Results...</source>
        <translation>Зберегти результати ...</translation>
    </message>
    <message>
        <source>The Full Repair plan runs the selected legacy stages in order through the guarded helper; the individual tools run one stage at a time. Every action stays disabled until the cached capability lines say available and the helper keeps its runtime preflights.</source>
        <translation>План повного ремонту проходить вибрані етапи спадщини для того, щоб через захищений помічник; індивідуальні інструменти працюють на одному етапі. Кожна акція залишається відключена до тих пір, поки не скажеться лініями кешування, і помічник зберігає свої робочі місця.</translation>
    </message>
    <message>
        <source>Full Repair plan</source>
        <translation>Повний план ремонту</translation>
    </message>
    <message>
        <source>No stages selected</source>
        <translation>Вибрані етапи</translation>
    </message>
    <message>
        <source>Configure Plan...</source>
        <translation>Налаштування плану...</translation>
    </message>
    <message>
        <source>Open Settings to choose which Full Repair stages are part of the plan.</source>
        <translation>Відкрийте налаштування, щоб вибрати, які етапи повного ремонту є частиною плану.</translation>
    </message>
    <message>
        <source>Run Full Repair</source>
        <translation>Повний ремонт</translation>
    </message>
    <message>
        <source>Select a repair drive, or choose Host Maintenance on the protected running-host card.</source>
        <translation>Виберіть привід ремонту або оберіть Обслуговування хостів на захищеній ходовій картці.</translation>
    </message>
    <message>
        <source>Stage</source>
        <translation>Етапи</translation>
    </message>
    <message>
        <source>Individual repair tools</source>
        <translation>Інструменти індивідуального ремонту</translation>
    </message>
    <message>
        <source>Tool</source>
        <translation>Головна</translation>
    </message>
    <message>
        <source>Full Repair</source>
        <translation>Повний ремонт</translation>
    </message>
    <message>
        <source>not reported</source>
        <translation>не повідомлено</translation>
    </message>
    <message>
        <source>Selected tool</source>
        <translation>Вибраний інструмент</translation>
    </message>
    <message>
        <source>Select a repair tool</source>
        <translation>Оберіть інструмент для ремонту</translation>
    </message>
    <message>
        <source>Run Tool</source>
        <translation>Запуск інструменту</translation>
    </message>
    <message>
        <source>Select a tool to review its repair action.</source>
        <translation>Виберіть інструмент для перегляду його ремонту.</translation>
    </message>
    <message>
        <source>Write actions ask for confirmation and then run the helper&apos;s own runtime preflights; the GUI never weakens them. A repair that is not proven &apos;unchanged&apos; invalidates the cached diagnostics and disables the gated actions until diagnostics run again.</source>
        <translation>Напишіть дії, які просять підтвердити, а потім запустіть власні просвіти помічника; GUI ніколи не ослабляє їх. Ремонт, який не зарекомендував себе недійсним, не несе відповідальності за діагностику та відключає дії, що ведуться до діагностування.</translation>
    </message>
    <message>
        <source>Chroot shell</source>
        <translation>Хроот оболонка</translation>
    </message>
    <message>
        <source>Offline commands run one at a time in a fresh chroot and cannot answer interactive prompts (apt-get -y upgrade works). Host-shell commands run directly on the running host. The helper&apos;s probe lines gate the command field; the exact reason appears in the tooltip.</source>
        <translation>Команди офлайн працюють одночасно в свіжому вигляді і не можуть відповісти на інтерактивні підказки (апт-get -y оновлені роботи). Команди Host-shell працюють безпосередньо на біговому господарі. Пояснюється поле командного рядка помічника; точна причина з&apos;являється в панелі інструментів.</translation>
    </message>
    <message>
        <source>No &apos;Legacy feature shell:&apos; line is cached; run diagnostics for the selected scope to evaluate the helper&apos;s chroot/timeout containment probes (fail closed).</source>
        <translation>No &apos;Legacy функції оболонки:&apos; рядок кешується; запустіть діагностику для вибраного обсягу, щоб оцінити прохідність хроот/timeout (замкнений закритий).</translation>
    </message>
    <message>
        <source>Command</source>
        <translation>Про компанію</translation>
    </message>
    <message>
        <source>Command:</source>
        <translation>Команда:</translation>
    </message>
    <message>
        <source>One reviewed command string, passed to the helper as a single argument (no shell interpolation by the GUI).</source>
        <translation>Один рецензований командний рядок, переданий до помічника як один аргумент (не оболонка інтерполяції GUI).</translation>
    </message>
    <message>
        <source>Run Command</source>
        <translation>Команди</translation>
    </message>
    <message>
        <source>Clear Output</source>
        <translation>Очистити вихід</translation>
    </message>
    <message>
        <source>The helper exposes `shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for an offline target chroot and `host-shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for the running host. Both keep the helper&apos;s runtime preflights; this tab enables the command only when the scope is committed, the session is authorized and the scope&apos;s Legacy feature probe reports available.</source>
        <translation>Помічник виводить `shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` для автономної мети chroot і `host-shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` для бігу. Як зберегти робочі прожектори помічника; ця вкладка дозволяє командувати тільки тоді, коли сфера буде виконана, сеанс уповноважений, а також звіти про спадщина, доступні.</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Файл копіювання</translation>
    </message>
    <message>
        <source>Copy and verify files in either direction through the guarded helper (cp -a plus ownership restoration and a per-file byte-compare). The helper&apos;s file-copy probe gates the controls and keeps the direction and path containment checks.</source>
        <translation>Скопіюйте та перевірте файли в будь-якому напрямку через захищений помічник (cp -a плюс відновлення прав власності та байт-compare). Файл-копія помічника зонду забиває контроль і зберігає напрямок і контроль за доріжками.</translation>
    </message>
    <message>
        <source>Preview Changes</source>
        <translation>Зміни перегляду</translation>
    </message>
    <message>
        <source>Run a copy dry-run through the guarded helper. No files are changed.</source>
        <translation>Виконайте копію сухий пробіг через захищений помічник. Не змінено файли.</translation>
    </message>
    <message>
        <source>Copy and Verify</source>
        <translation>Статус на сервери</translation>
    </message>
    <message>
        <source>Copy staged items and verify the result. Existing destination names are overwritten when source content differs; unrelated destination files are never deleted.</source>
        <translation>Скопіювати застарілі елементи і перевірити результат. Виконувати імена пунктів призначення перезаписуються при різному вмісту джерела; не пов&apos;язані файли призначення ніколи не видаляються.</translation>
    </message>
    <message>
        <source>Direction:</source>
        <translation>Напрямок:</translation>
    </message>
    <message>
        <source>Choose which system supplies the source files and which system receives them.</source>
        <translation>Виберіть яку систему подає вихідні файли та які системи отримують їх.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from this host</source>
        <translation>1. Виберіть вихідні файли або папки з цього хосту</translation>
    </message>
    <message>
        <source>Source</source>
        <translation>Джерело</translation>
    </message>
    <message>
        <source>Files and folders staged for the verified copy. The legacy backend copies with cp -a and restores ownership with chown --reference; every regular file is byte-compared after the copy.</source>
        <translation>Файли та папки зафіксовані для перевіреної копії. Скопіювати копії Legacy Backend з cp -a і відновити власність з chown --reference; кожен звичайний файл передається після копіювання.</translation>
    </message>
    <message>
        <source>Add Files...</source>
        <translation>Додати файли...</translation>
    </message>
    <message>
        <source>Add Folder...</source>
        <translation>Додати папку ...</translation>
    </message>
    <message>
        <source>Remove</source>
        <translation>Прибрати</translation>
    </message>
    <message>
        <source>Clear</source>
        <translation>Очистити</translation>
    </message>
    <message>
        <source>Clear the staged source list (nothing is copied or deleted).</source>
        <translation>Очистити поетапний список джерела (без копіювання або видалення).</translation>
    </message>
    <message>
        <source>2. Choose destination in repaired system</source>
        <translation>2. Виберіть пункт призначення в ремонтній системі</translation>
    </message>
    <message>
        <source>unavailable: see the helper&apos;s Legacy feature file-copy: probe reason above</source>
        <translation>0 товар(ов) - 0.00 р.</translation>
    </message>
    <message>
        <source>An absolute path inside the selected repair system (Host to Repair) or on the running host (Repair to Host).</source>
        <translation>Абсолютний шлях всередині вибраної системи ремонту (доступ до ремонту) або на ходовому хості (Ремонт до Хост).</translation>
    </message>
    <message>
        <source>Browse Target Folders...</source>
        <translation>Перегляд цільових складників ...</translation>
    </message>
    <message>
        <source>Browse the selected repair system through the helper&apos;s temporary read-only mounts and choose an absolute destination path. No target files are changed while browsing.</source>
        <translation>Перегляд виділеної системи ремонту за допомогою тимчасових читальних кріплень помічника і виберіть абсолютний шлях призначення. Не змінюються цільові файли під час перегляду.</translation>
    </message>
    <message>
        <source>3. Ownership and copy policy</source>
        <translation>3. Адміністрування та копіювання</translation>
    </message>
    <message>
        <source>Ownership:</source>
        <translation>Власникство:</translation>
    </message>
    <message>
        <source>Smart destination ownership (recommended)</source>
        <translation>Розумна власність призначення (рекомендована)</translation>
    </message>
    <message>
        <source>Preserve source numeric UID/GID</source>
        <translation>Збереження джерела numeric UID / GID</translation>
    </message>
    <message>
        <source>Smart mode validates UID/GID identity mapping across the two systems and falls back to the destination-directory owner when the same numeric ID means a different account (the legacy backend implements it with chown --reference).</source>
        <translation>Розумний режим діє на картографування ідентичності UID/GID у двох системах і повертається до власника пункту призначення, коли той самий неоднорідний ID означає інший обліковий запис (попередня спадщина реалізовує його хаун --референція).</translation>
    </message>
    <message>
        <source>Application log</source>
        <translation>Журнал додатків</translation>
    </message>
    <message>
        <source>The complete session register; Save As... writes every entry even while a filter hides lines. If a writable system share mount exists at /host, Save As... starts there; otherwise the log directory is the fallback. Prior session files are listed read-only.</source>
        <translation>Повний реєстр сеансу; Заощаджуйте всі записи, навіть якщо фільтр приховує лінії. У разі відсутності змінної кількості систем в / привиді, Зберегти як... Пріоритетні файли сеансу вказані на сайті.</translation>
    </message>
    <message>
        <source>Session logs</source>
        <translation>Журнали сеансу</translation>
    </message>
    <message>
        <source>Session</source>
        <translation>Сесія</translation>
    </message>
    <message>
        <source>The first entry is the live session; earlier files in the log directory are listed read-only below it.</source>
        <translation>Першим записом є інформаційна сесія; попередні файли в каталозі журналу перераховують на читання нижче.</translation>
    </message>
    <message>
        <source>New Session Log</source>
        <translation>Новий журнал сеансу</translation>
    </message>
    <message>
        <source>Close the active session file; it becomes a prior session and the next log entry starts a new file.</source>
        <translation>Закрийте файл активного сеансу; він стає передчасною сеансою та наступним записом журналу починає новий файл.</translation>
    </message>
    <message>
        <source>Add Note</source>
        <translation>Додати Примітка</translation>
    </message>
    <message>
        <source>Append a NOTE entry to the live session register.</source>
        <translation>Прийміть запис на інформаційну сесію.</translation>
    </message>
    <message>
        <source>Delete</source>
        <translation>Делет</translation>
    </message>
    <message>
        <source>Delete the selected prior session file (the live session is never deleted).</source>
        <translation>Видалення вибраного файлу попереднього сеансу (візитка не видалена).</translation>
    </message>
    <message>
        <source>Refresh</source>
        <translation>Реверс</translation>
    </message>
    <message>
        <source>Save As...</source>
        <translation>Зберегти як ...</translation>
    </message>
    <message>
        <source>Save the complete session log (all entries, not just the current filter).</source>
        <translation>Заощаджуйте повний журнал сеансу (всі записи, не тільки поточний фільтр).</translation>
    </message>
    <message>
        <source>Clear Register</source>
        <translation>Реєстрація</translation>
    </message>
    <message>
        <source>Clear the live register and view; prior session files are never modified.</source>
        <translation>Прозорий регістр і перегляд; передчасні файли сеансу ніколи не змінені.</translation>
    </message>
    <message>
        <source>Search log:</source>
        <translation>Пошук журналу:</translation>
    </message>
    <message>
        <source>Type any characters to show matching log entries (case-insensitive). Save As always writes every entry.</source>
        <translation>Введіть будь-які символи, щоб показати відповідні записи в журналі (case-insensitive). Зберегти Як завжди записується кожен запис.</translation>
    </message>
    <message>
        <source>Filter:</source>
        <translation>Фільтр:</translation>
    </message>
    <message>
        <source>Filter the visible log by entry kind. Selecting a diagnostic section shows the lines captured for that section; a workflow filter such as File system repair or Package repair shows its mapped repair lines (File copy has no lines in this frontend). Save As always writes every entry.</source>
        <translation>Фільтрувати видимий журнал за типом входу. Вибір діагностичного розділу показує лінії, які зафіксовані для цього розділу; рефлектор робочої системи, такі як відновлення файлової системи або ремонт пакетів показує її намальовані ремонтні лінії (Файл-копія не має ліній на цьому фронті). Зберегти Як завжди записується кожен запис.</translation>
    </message>
    <message>
        <source>Settings are stored per user under ~/.qt/, one file per settings group (devicesrc, logsrc, diagnosticsrc, repairrc), and are saved immediately on every change and on close. Launch the GUI as the same user to keep your overrides; a GUI started as root keeps its own copies.</source>
        <translation>Налаштування зберігаються для користувача під ~ / .qt /, один файл для групи налаштувань (devicesrc, logsrc, діагностична дуга, ремонт дуги), і зберігаються відразу на кожній зміні і в безпосередній близькості. Запустіть графічний інтерфейс, як і той самий користувач, щоб зберегти ваші перенависті; графічний інтерфейс почав як корінь зберігає свої копії.</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Відкриття пристрою</translation>
    </message>
    <message>
        <source>Show devices without an identified Linux installation</source>
        <translation>Показати пристрої без встановленої установки Linux</translation>
    </message>
    <message>
        <source>Show removable and USB storage</source>
        <translation>Показати знімний та USB-накопичувач</translation>
    </message>
    <message>
        <source>Show encrypted devices before unlocking</source>
        <translation>Показати зашифровані пристрої перед розблокуванням</translation>
    </message>
    <message>
        <source>When off, drives without a visible Linux filesystem are hidden unless they still contain an encrypted device and encrypted devices are shown.</source>
        <translation>При вимкненні диски без видимої файлової системи Linux приховані, якщо вони все ще містять зашифрований пристрій і зашифровані пристрої.</translation>
    </message>
    <message>
        <source>When off, removable and USB drives are hidden from the repair-target list.</source>
        <translation>При вимкненні, знімні та USB-накопичувачі приховані з ремонту-завантаження.</translation>
    </message>
    <message>
        <source>When off, drives with an encrypted device are hidden until the volume is unlocked.</source>
        <translation>При вимкненні диски з зашифрованим пристроєм приховані до розблокування обсягу.</translation>
    </message>
    <message>
        <source>Include the %1 stage in the Full Repair plan. The stage runs in the plan order shown on the Repair tab.</source>
        <translation>У комплекті фази %1 у плані повного ремонту. На вкладці «Ремонт».</translation>
    </message>
    <message>
        <source>Automatically regenerate read-only diagnostics after repairs or target changes</source>
        <translation>Автоматично регенерувати діагностику після ремонту або цільових змін</translation>
    </message>
    <message>
        <source>Regenerates the cached read-only diagnostics for the current scope after an operation that invalidates them (LUKS unlock, target configuration edit). It runs only inside an already authorized administrator session and never opens an authorization prompt by itself.</source>
        <translation>Регенерує кешовану діагностику для поточного обсягу після операції, яка не несе їх (LUKS розблокування, редагування цільової конфігурації). Він працює тільки в вже уповноваженому адміністратору, і ніколи не відкриває запит авторизації.</translation>
    </message>
    <message>
        <source>Wrap long log lines</source>
        <translation>Обгортання довгих колод ліній</translation>
    </message>
    <message>
        <source>Mandatory safety controls</source>
        <translation>Управління безпекою персоналу</translation>
    </message>
    <message>
        <source>Host capabilities and dependencies</source>
        <translation>Можливості та залежності</translation>
    </message>
    <message>
        <source>Refresh Capabilities</source>
        <translation>Зручності в ребершах</translation>
    </message>
    <message>
        <source>Re-run the read-only host capability probes (a PATH search, nothing is executed) and refresh the distribution summary.</source>
        <translation>Переконайтесь про те, що ознайомтесь з читацькими хостами (пошук PATH, нічого не виконано) і освіжайте загальний розподіл.</translation>
    </message>
    <message>
        <source>Install Missing Support...</source>
        <translation>Встановити Missing Support ...</translation>
    </message>
    <message>
        <source>Automatic installation will require explicit package mapping and privilege authorization.</source>
        <translation>Автоматична інсталяція зажадає явний пакет картографування та авторизації привілеїв.</translation>
    </message>
    <message>
        <source>Application configuration</source>
        <translation>Налаштування додатків</translation>
    </message>
    <message>
        <source>Boot Bitch Legacy</source>
        <translation>Boot Bitch Legacy</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list first.</source>
        <translation>Виберіть фізичний привід у переліку запасних цілей.</translation>
    </message>
    <message>
        <source>Protected system</source>
        <translation>Система захисту</translation>
    </message>
    <message>
        <source>The running system cannot be selected as a repair target. Use Host Maintenance for the protected running host or choose another disk.</source>
        <translation>Система ходової роботи не може бути обрана в якості технічного завдання. Використовуйте службу хост для захищеного хосту або вибрати інший диск.</translation>
    </message>
    <message>
        <source>Unlock or select a Linux system first</source>
        <translation>Розблокувати або вибрати систему Linux першим</translation>
    </message>
    <message>
        <source>This encrypted drive has no visible Linux filesystem yet. Use Unlock, refresh devices, and select the target after its Linux root is detected.</source>
        <translation>Цей зашифрований диск не має видимої файлової системи Linux. Використовуйте розблокування, освіжаючи пристрої та оберіть ціль після його виявлення кореня Linux.</translation>
    </message>
    <message>
        <source>The selected root component (%1) belongs to the running system and cannot be committed as a repair target. Use Host Maintenance for the protected running host.</source>
        <translation>Вибраний компонент кореневого компонента (%1) належить до системи, що працює, і не може бути здійснений в якості технічного завдання. Використовуйте службу хост для захищеного хосту.</translation>
    </message>
    <message>
        <source>repair target commit</source>
        <translation>ремонт цільового коміту</translation>
    </message>
    <message>
        <source>Repair drive selected: %1</source>
        <translation>Вибраний привід: %1</translation>
    </message>
    <message>
        <source>; best detected system component: %1</source>
        <translation>; Кращий виявлений системний компонент: %1</translation>
    </message>
    <message>
        <source>. No mount or repair action was performed.</source>
        <translation>. Не було виконано монтаж або ремонт.</translation>
    </message>
    <message>
        <source>Make Default unavailable</source>
        <translation>За замовчуванням 0</translation>
    </message>
    <message>
        <source>Make Default is a running-host action on this frontend; enter Host Maintenance first.</source>
        <translation>Зробіть за замовчуванням, що працює на цьому фронті; введіть службу підтримки</translation>
    </message>
    <message>
        <source>Administrator authorization required</source>
        <translation>Вимоги до авторизації адміністратора</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize first.</source>
        <translation>Не працює сеанс адміністратора; натиснути кнопку Авторизація.</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host?

The helper verifies /boot/grub/menu.lst, sets the `default &lt;N&gt;` directive to the canonical entry, backs the menu up first and restores it on any failure.</source>
        <translation>Зробіть запис канонічного встановленого ядра за замовчуванням GRUB-legacy завантажувальний вхід на запущений хост?

Допомоги /завантаження/груб/меню.лст, встановлює `default &lt;N&gt;` пряма до канонічного в&apos;їзду, повертає меню першим і відновлює його на будь-якій з ладу.</translation>
    </message>
    <message>
        <source>Host Maintenance unavailable</source>
        <translation>Обслуговування гостей 0</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a committed repair target or a detected running host.</source>
        <translation>Мета роботи не може бути виявлена; діагностику потрібно виконати відремонтовану ціль або виявлений ходовий господар.</translation>
    </message>
    <message>
        <source>Exit Host Maintenance</source>
        <translation>Обслуговування Exit Хост</translation>
    </message>
    <message>
        <source>Host Maintenance is active; diagnostics and gated repairs target the protected running host.</source>
        <translation>Ведуться роботи з діагностування та ремонту воріт.</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list on the Systems tab first, or use Host Maintenance for the protected running host.</source>
        <translation>Виберіть фізичний диск у списку доступних завдань з ремонту на вкладці Системи спочатку, або скористайтеся сервісом Хост для захищеного бігу.</translation>
    </message>
    <message>
        <source>The selected drive (%1) is the protected running host. Choose Host Maintenance on the Systems tab to run read-only host diagnostics and guarded host repairs; ordinary target repairs stay disabled.</source>
        <translation>Вибраний привід (%1) є захищеним ходовим господарем. Підібрати технічне обслуговування на вкладці Системи, щоб запустити діагностику та охоронець ремонт господарів; звичайні капітальні ремонти залишаються вимкненими.</translation>
    </message>
    <message>
        <source>No repair target is committed. Choose Select Target on the Systems tab (or Host Maintenance for the protected running host) first.</source>
        <translation>Відправка не здійснюється. Виберіть Ціль на вкладці Системи (або Обслуговування хостів для захищеного хосту)</translation>
    </message>
    <message>
        <source>The selection changed after the target was committed. Choose Select Target again.</source>
        <translation>Підбір змінено після виконання поставлених цілей. Виберіть ще раз.</translation>
    </message>
    <message>
        <source>Diagnostics scope required</source>
        <translation>Діагностичне значення</translation>
    </message>
    <message>
        <source>Diagnostics scope unresolved</source>
        <translation>Діагностика нерозчинної</translation>
    </message>
    <message>
        <source>The running host target could not be resolved; use Refresh Devices and commit a repair target or re-enter Host Maintenance.</source>
        <translation>Ціль ходового хосту не може бути вирішена; використовувати пристрої Refresh і впорядковувати цільову або повторну роботу.</translation>
    </message>
    <message>
        <source>Diagnostic check required</source>
        <translation>Діагностичний контроль</translation>
    </message>
    <message>
        <source>Select a diagnostic check in the list first.</source>
        <translation>Виберіть діагностичну перевірку у списку.</translation>
    </message>
    <message>
        <source>Repair target required</source>
        <translation>Вимоги до ремонту</translation>
    </message>
    <message>
        <source>Target file editing needs a committed repair target. Running-host maintenance has no target-file editing; commit an offline target first.</source>
        <translation>Цільове редагування файлів потребує цільової мети ремонту. Запускне обслуговування не має цільового редагування; виконайте офлайн цілі першим.</translation>
    </message>
    <message>
        <source>Configuration file required</source>
        <translation>Налаштування файлу необхідно</translation>
    </message>
    <message>
        <source>Select a target configuration file first.</source>
        <translation>Виберіть файл конфігурації цілі першим.</translation>
    </message>
    <message>
        <source>Edit target %1</source>
        <translation>Редагування цільової %1</translation>
    </message>
    <message>
        <source>Edit this target file through the guarded administrator helper. A successful save invalidates cached diagnostics; rerun diagnostics before repair. Generated files such as /boot/grub/menu.lst may be replaced by the next bootloader update.</source>
        <translation>Редагувати цей цільовий файл через захищений адміністратор. Вдале економить недійсну діагностику, перебіг діагностики перед ремонтом. Генеровані файли, такі як /завантаження/груб/menu.lst, можуть бути замінені на наступний оновлення завантаження.</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Зареєструватися</translation>
    </message>
    <message>
        <source>Save Target File</source>
        <translation>Зберегти цільовий файл</translation>
    </message>
    <message>
        <source>No changes to %1.</source>
        <translation>Немає змін до %1.</translation>
    </message>
    <message>
        <source>Configuration write refused</source>
        <translation>Написати відповідь</translation>
    </message>
    <message>
        <source>The edited content contains NUL bytes; the guarded write refuses it.</source>
        <translation>Відредагований вміст містить NUL байти; захищений запис від нього.</translation>
    </message>
    <message>
        <source>File too large</source>
        <translation>Файл занадто великий</translation>
    </message>
    <message>
        <source>The edited file is larger than 1 MiB. The guarded write refuses it; edit the file from a console instead.</source>
        <translation>Редагований файл більше 1 MiB. Захищаючи його запис, відредагуйте файл з консолі замість.</translation>
    </message>
    <message>
        <source>Write target configuration</source>
        <translation>Настроювання цілі</translation>
    </message>
    <message>
        <source>Write the edited contents to %1? This modifies the repair target and invalidates cached diagnostics.</source>
        <translation>Написати редагований вміст до %1? Дана модіфікує діагностику ремонтних цілей та недійсних коштів.</translation>
    </message>
    <message>
        <source>No diagnostic results to copy yet.</source>
        <translation>Немає діагностичних результатів для копіювання ще.</translation>
    </message>
    <message>
        <source>Diagnostic results copied to the clipboard.</source>
        <translation>Діагностичні результати прикопуються до буферу обміну.</translation>
    </message>
    <message>
        <source>No diagnostic results to save yet.</source>
        <translation>Немає діагностичних результатів, щоб зберегти ще.</translation>
    </message>
    <message>
        <source>Text files (*.txt);;All files (*)</source>
        <translation>Текстові файли (*.txt);</translation>
    </message>
    <message>
        <source>Save diagnostic results</source>
        <translation>Зберегти діагностичні результати</translation>
    </message>
    <message>
        <source>Could not write %1.</source>
        <translation>Не писати %1.</translation>
    </message>
    <message>
        <source>Diagnostic results saved to %1</source>
        <translation>Діагностичні результати, що зберігаються в %1</translation>
    </message>
    <message>
        <source>Unlock not available</source>
        <translation>Нерозблокувати</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked. Select an offline repair target to unlock.</source>
        <translation>Захищений хост не може бути розблокований. Виберіть офлайн-регулятор для розблокування.</translation>
    </message>
    <message>
        <source>No locked LUKS component is currently visible on this selected drive.</source>
        <translation>Немає замкненого компонента LUKS в даний час відображається на цьому вибраному диску.</translation>
    </message>
    <message>
        <source>Confirm LUKS unlock</source>
        <translation>Підтвердити LUKS розблокування</translation>
    </message>
    <message>
        <source>Unlock %1 on %2?

The helper opens a temporary device-mapper mapping with cryptsetup and keeps it open for this recovery session. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>Розблокувати %1 на %2?

Помічник відкриває тимчасову копіювальну картографічну картографію з cryptsetup і зберігає її відкритим для цієї сеансу відновлення. Перехід проходить через приватний ключ і ніколи не розміщується в аргументах або журналах.</translation>
    </message>
    <message>
        <source>LUKS unlock</source>
        <translation>LUKS розблокування</translation>
    </message>
    <message>
        <source>Unlock LUKS repair target</source>
        <translation>Unlock LUKS ремонт цільової</translation>
    </message>
    <message>
        <source>Enter the passphrase for %1.

It is sent only to cryptsetup over the helper&apos;s standard input and is never logged or placed on a command line.</source>
        <translation>Введіть файл для %1.

Відправляється тільки для розшифрування над стандартним введенням помічника і ніколи не ввійде або розміщується на командному рядку.</translation>
    </message>
    <message>
        <source>Passphrase required</source>
        <translation>Пасфразе обов&apos;язкове</translation>
    </message>
    <message>
        <source>An empty passphrase was not submitted. Enter the LUKS passphrase or choose Cancel.</source>
        <translation>Не подано порожній пасфрас. Введіть номер мобільного, який Ви вказали при укладаннi договору з банком - для ідентифікації.</translation>
    </message>
    <message>
        <source>Unlock keyfile unavailable</source>
        <translation>English, Українська, Français...</translation>
    </message>
    <message>
        <source>The LUKS passphrase could not be written to a private keyfile in %1; the unlock was not started.</source>
        <translation>LUKS Passphrase не може бути написаний для приватного ключа в %1; розблокування не було розпочато.</translation>
    </message>
    <message>
        <source>Configuration write unavailable</source>
        <translation>Настроювання запису 0</translation>
    </message>
    <message>
        <source>The edited content could not be written to a private temporary file in %1; the write was not started.</source>
        <translation>Відредагований вміст не може бути записаний до приватного тимчасового файлу в %1; запис не було розпочато.</translation>
    </message>
    <message>
        <source>ERROR: cannot read the session log %1</source>
        <translation>ERROR: не можна читати журнал сеансу %1</translation>
    </message>
    <message>
        <source>Viewing a prior session log (read-only): %1</source>
        <translation>Перегляд попереднього журналу сеансу (тільки): %1</translation>
    </message>
    <message>
        <source>Session log list refreshed.</source>
        <translation>Список журналів Сесія оновлений.</translation>
    </message>
    <message>
        <source>Note:</source>
        <translation>Примітка:</translation>
    </message>
    <message>
        <source>Delete session log</source>
        <translation>Видалити запис сеансу</translation>
    </message>
    <message>
        <source>Delete %1 permanently? This cannot be undone.</source>
        <translation>Видалити %1 постійно? Це не може бути неоновим.</translation>
    </message>
    <message>
        <source>%1 changed while the confirmation was open; the delete was refused.</source>
        <translation>%1 змінилися під час відкриття підтвердження; видалення було відмовлено.</translation>
    </message>
    <message>
        <source>Unable to delete %1.</source>
        <translation>Неможливо видалити %1.</translation>
    </message>
    <message>
        <source>Device discovery filters updated.</source>
        <translation>Фільтри для виявлення пристроїв оновлено.</translation>
    </message>
    <message>
        <source>Unknown Linux distribution</source>
        <translation>Невідомий розподіл Linux</translation>
    </message>
    <message>
        <source>sudo / gksu (no KAuth on this frontend)</source>
        <translation>sudo / gksu (не KAuth на цьому фронті)</translation>
    </message>
    <message>
        <source>Available</source>
        <translation>В наявності</translation>
    </message>
    <message>
        <source>Missing on this frontend</source>
        <translation>Проходження на цей фронтенд</translation>
    </message>
    <message>
        <source>Missing</source>
        <translation>Мапа</translation>
    </message>
    <message>
        <source>No repair tool is selected.</source>
        <translation>Не вибрано інструмент ремонту.</translation>
    </message>
    <message>
        <source>A helper command is already running.</source>
        <translation>Команда помічника вже працює.</translation>
    </message>
    <message>
        <source>Restore Graphical Login is a host-scope stage on this legacy frontend; enter Host Maintenance to run it.</source>
        <translation>Відновити Графічний Логін - це хост-скоп-сценція на цій передовій частині спадщини; вводити технічне обслуговування хостів, щоб запустити його.</translation>
    </message>
    <message>
        <source>The selected scope has no resolved root component; use Refresh Devices and commit the repair target again.</source>
        <translation>Вибраний обсяг не врегульований кореневим компонентом; використовувати Refresh Devices і знову заблокувати поставлену задачу ремонту.</translation>
    </message>
    <message>
        <source>This legacy frontend exposes no %1 action; the helper reports the capability as available.</source>
        <translation>Ця передова передня спадщина не поширюється на дію %1; помічник повідомляє про можливість як доступні.</translation>
    </message>
    <message>
        <source>Run this guarded repair action using the cached read-only diagnostic evidence. A confirmation is shown first.</source>
        <translation>Запуск цієї захищеної дії ремонту, використовуючи діагностичні докази, що містяться в кабіні. Підтвердження було показано першим.</translation>
    </message>
    <message>
        <source>Enabled in Settings</source>
        <translation>Увімкнути налаштування</translation>
    </message>
    <message>
        <source>Disabled in Settings - enable it to include this stage</source>
        <translation>Увімкніть налаштування - ввімкніть його, щоб включити цей етап</translation>
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
        <translation>Інструмент для ремонту 0</translation>
    </message>
    <message>
        <source>Confirm repair</source>
        <translation>Підтвердження ремонту</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Your saved selection is kept and its availability is re-checked when diagnostics for this scope complete.</source>
        <translation>На цьому етапі ще немає чіткості. Ваш збережений вибір зберігається і його доступність повторюється при діагностиці даного обсягу.</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Run diagnostics for the selected scope to populate the Full Repair plan; your selection is saved once the stage becomes available.</source>
        <translation>На цьому етапі ще немає чіткості. Проводити діагностику для обраного об&apos;єму, щоб з&apos;єднати план повного ремонту; ваш вибір зберігається один раз на стадії.</translation>
    </message>
    <message>
        <source>Unknown Full Repair stage.</source>
        <translation>Невідомий етап повного ремонту.</translation>
    </message>
    <message>
        <source>No Full Repair stages are selected or available; use Configure Plan... to choose the stages.</source>
        <translation>Для вибору етапу можна вибрати всі етапи.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab to establish the session.</source>
        <translation>Для встановлення сеансу необхідно авторизацію адміністратора; натиснути Авторизації на вкладці Системи або Ремонт.</translation>
    </message>
    <message>
        <source>Runs the selected stages using the cached read-only diagnostic evidence after privilege confirmation.</source>
        <translation>Виконує вибрані етапи, використовуючи кеш-чит-тільки діагностичні докази після підтвердження привілеїв.</translation>
    </message>
    <message>
        <source>No Full Repair stages selected - use Configure Plan... or Settings.</source>
        <translation>Не вибрано етапи повного ремонту - використовуйте План налаштування... або налаштування.</translation>
    </message>
    <message>
        <source>1 stage selected</source>
        <translation>1 етап обраний</translation>
    </message>
    <message>
        <source>%1 stages selected</source>
        <translation>Вибрані етапи %1</translation>
    </message>
    <message>
        <source>No repair stages are selected. Use Configure Plan... to choose the stages Full Repair will run.</source>
        <translation>Вибрані етапи ремонту. Використовуйте план налаштування ... вибрати етапи Повний ремонт буде працювати.</translation>
    </message>
    <message>
        <source>No selected stages are available. %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Full Repair unavailable</source>
        <translation>Повний ремонт 0</translation>
    </message>
    <message>
        <source>Run the Full Repair plan?

The selected stages run in order through the helper&apos;s guarded repair command:

</source>
        <translation>Запустіть план повного ремонту?

Вибрані етапи виконуються для того, щоб через службу ремонту помічника:

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
Допоможитель зберігає в будь-який час, коли не зупиняється план.</translation>
    </message>
    <message>
        <source>Running %1 through the privileged helper... The Logs tab keeps the complete transcript.</source>
        <translation>Запуск %1 через привілейований помічник ... Вкладка Logs зберігає повну транскрипцію.</translation>
    </message>
    <message>
        <source>Shell scope required</source>
        <translation>Область Shell вимагає</translation>
    </message>
    <message>
        <source>No administrator session is active.

Press Authorize on the Systems or Repair tab to establish the session, or enter Host Maintenance / commit a repair target on the Systems tab; the chroot shell then reuses the cached authorization.</source>
        <translation>Не працює сеанс адміністратора.

Натиснути авторизації на вкладці Системи або ремонту, щоб встановити сеанс, або ввести службу підтримки хосту / впорядкувати цільову на вкладці Системи; оболонку хроот, потім повторно застосує заставу.</translation>
    </message>
    <message>
        <source>Shell command required</source>
        <translation>Команда Shell вимагає</translation>
    </message>
    <message>
        <source>Enter the command to run first.</source>
        <translation>Введіть команду для запуску першого.</translation>
    </message>
    <message>
        <source>Confirm running-host command</source>
        <translation>Підтвердити команду run-host</translation>
    </message>
    <message>
        <source>Run this command as root on the protected running host?

%1

The helper keeps its runtime preflights; the command is passed as one argument and is never interpreted by the GUI.</source>
        <translation>Запустіть цю команду як корінь на захищеному біговому господарі?

%1

Допоможисер зберігає свої просвіти в режимі runtime; команда передається як один аргумент і ніколи не інтерпретується GUI.</translation>
    </message>
    <message>
        <source>Running %1...</source>
        <translation>Запуск %1 ...</translation>
    </message>
    <message>
        <source>Authorization scope required</source>
        <translation>Вимоги до авторизації</translation>
    </message>
    <message>
        <source>administrator authorization</source>
        <translation>авторизація адміністратора</translation>
    </message>
    <message>
        <source>Administrator authorization</source>
        <translation>Авторизація адміністратора</translation>
    </message>
    <message>
        <source>Administrator authorization is required for %1.

Enter the password for %2 (sudo). It is used only for this sudo authentication, is sent over a pipe and is never logged or placed on a command line. The authorization is cached for this session and reused by diagnostics and repairs.</source>
        <translation>Вимоги до авторизації адміністратора для %1.

Введіть пароль для %2 (sudo). Він використовується тільки для цієї автентифікації sudo, надсилається над трубою і ніколи не ввіймається або розміщується на командному рядку. Авторизація здійснюється за допомогою діагностики та ремонту.</translation>
    </message>
    <message>
        <source>your account</source>
        <translation>Ваш рахунок</translation>
    </message>
    <message>
        <source>Password required</source>
        <translation>Необхідний пароль</translation>
    </message>
    <message>
        <source>An empty password was not submitted. Enter the sudo password or choose Cancel.</source>
        <translation>Порожнього пароля не подано. Введіть пароль sudo або виберіть Скасувати.</translation>
    </message>
    <message>
        <source>Administrator authorization failed</source>
        <translation>Авторизація адміністратора не вдалося</translation>
    </message>
    <message>
        <source>sudo did not accept the password: %1

The command was not started.</source>
        <translation>sudo не приймає пароль: %1

Команда не була запущена.</translation>
    </message>
    <message>
        <source>elevation requires an interactive sudo password; run the smoke as root or after `sudo -S -v` with --elevate &apos;sudo -n&apos;</source>
        <translation>елеватор вимагає інтерактивного sudo пароля; запустіть дим як корінь або після `sudo -S -v` з --elevate &apos;sudo -n&apos;</translation>
    </message>
    <message>
        <source>No administrator session is active for %1.

Press Authorize on the Systems or Repair tab to establish the session now, or enter Host Maintenance / commit a repair target on the Systems tab; diagnostics and repairs then reuse the cached authorization.</source>
        <translation>Немає сеансу адміністратора для %1.

Натиснути авторизації на вкладці Системи або ремонту, щоб встановити сеанс зараз, або ввести технічне обслуговування хосту / внести на вкладку Системи; діагностику та ремонт, після чого повторно зловживати заставу.</translation>
    </message>
    <message>
        <source>Administrator authorization expired</source>
        <translation>Авторизація адміністратора</translation>
    </message>
    <message>
        <source>The cached administrator authorization for %1 expired or was refused.

Press Authorize on the Systems or Repair tab to re-establish the session, then run the command again. No command was started.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Privileged operation completed successfully. Administrator authorization remains active for this session.</source>
        <translation>Привілегована операція успішно завершена. Авторизація адміністратора залишається активним для цього сеансу.</translation>
    </message>
    <message>
        <source>Privileged operation stopped with an error. Administrator authorization remains active; review the output before closing.</source>
        <translation>Припинена операція припинена з помилкою. Авторизація адміністратора залишається активним; огляд виходу перед закриттям.</translation>
    </message>
    <message>
        <source>Inspection is read-only.

</source>
        <translation>Інспекція читає-тільки.

</translation>
    </message>
    <message>
        <source>Configuration unavailable</source>
        <translation>Налаштування 0</translation>
    </message>
    <message>
        <source>The helper could not read %1:

%2</source>
        <translation>Помічник не міг читати %1:

%2</translation>
    </message>
    <message>
        <source>Configuration write failed</source>
        <translation>Написання конфігурації не вдалося</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
Result: mapping opened for this recovery session.</source>
        <translation>Стан: розблокований
Компонент: %1
Оператор: %2
Спосіб: розблокування помічника (cryptsetup; passphrase через мод-600 keyfile, видалений після використання)
Результат: відкриття карти для цієї сесії відновлення.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: the passphrase was not accepted; retry offered.</source>
        <translation>Стан: замкнений
Компонент: %1
Спосіб: розблокування помічника (cryptsetup)
Помилка: пасфрас не прийнято; запропонована птиця.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: %2</source>
        <translation>Стан: замкнений
Компонент: %1
Спосіб: розблокування помічника (cryptsetup)
Помилка: %2</translation>
    </message>
    <message>
        <source>Passphrase not accepted</source>
        <translation>Пасфрас не прийнято</translation>
    </message>
    <message>
        <source>The LUKS passphrase was not accepted.

Try again?</source>
        <translation>ТР22X пасфрас не було прийнято.

Спробуйте знову?</translation>
    </message>
    <message>
        <source>[-] %1 - not run (the plan stopped before reaching this stage)</source>
        <translation>[-] %1 - не запустити (план припинився до досягнення цього етапу)</translation>
    </message>
    <message>
        <source>filesystem</source>
        <translation>файлова система</translation>
    </message>
    <message>
        <source>[-] %1 - no file system errors found - no changes</source>
        <translation>[-] %1 - не знайдено помилок файлової системи - ніяких змін</translation>
    </message>
    <message>
        <source>[FAIL] %1 - not reported</source>
        <translation>[FAIL] %1 - не повідомлено</translation>
    </message>
    <message>
        <source>%1 stage(s) failed; review the helper output in Logs.</source>
        <translation>%1 етап(s) не вдалося; огляд виходу помічника в журналах.</translation>
    </message>
    <message>
        <source>the plan stopped before any stage completed.</source>
        <translation>план припинив до завершення будь-якого етапу.</translation>
    </message>
    <message>
        <source>no repair was needed; cached diagnostics remain valid.</source>
        <translation>не потрібно ремонтувати; діагностика залишається дійсним.</translation>
    </message>
    <message>
        <source>repair completed; cached diagnostics were invalidated and must be regenerated.</source>
        <translation>ремонт завершено; недійсною діагностику, яка повинна бути відновлена.</translation>
    </message>
    <message>
        <source>%1 results: [OK] %2 successful | [FAIL] %3 failed | [-] %4 no repair needed - %5</source>
        <translation>%1 результати: [OK] %2 успішний [FAIL] %3 не вдалося [-] %4 не потрібно ремонту - %5</translation>
    </message>
    <message>
        <source>Re-run Diagnostic</source>
        <translation>Діагностика</translation>
    </message>
    <message>
        <source>Unavailable</source>
        <translation>Відкрито</translation>
    </message>
    <message>
        <source>Commit this physical drive as the repair target.</source>
        <translation>Прийміть цей фізичний привід як кінцевий ремонт.</translation>
    </message>
    <message>
        <source>Protection:</source>
        <translation>Захист:</translation>
    </message>
    <message>
        <source>Host shell</source>
        <translation>Штукатурка</translation>
    </message>
    <message>
        <source>Run on Host</source>
        <translation>Запуск на Хост</translation>
    </message>
    <message>
        <source>Host Shell</source>
        <translation>Хост Shell</translation>
    </message>
    <message>
        <source>The read-only probe found no editable target configuration file in this target. %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Not present in the selected target (omitted): %1</source>
        <translation>Не присутні в обраній цілі (з&apos;єднаних сторін): %1</translation>
    </message>
    <message>
        <source>Run diagnostics to probe which target configuration files exist; the helper&apos;s read-only probe decides the list.</source>
        <translation>Діагностика виконання завдань, які існують файли конфігурацій; контроль за читанням помічника</translation>
    </message>
    <message>
        <source>Probed read-only by the helper; a saved edit invalidates the cached diagnostics.</source>
        <translation>Проведний читання-тільки помічником; збережений редаг недійсний діагноз.</translation>
    </message>
    <message>
        <source>Host maintenance: %1</source>
        <translation>Обслуговування гостей: %1</translation>
    </message>
    <message>
        <source>unresolved</source>
        <translation>нерозчинний</translation>
    </message>
    <message>
        <source>Target: %1 + %2</source>
        <translation>Призначення: %1 + %2</translation>
    </message>
    <message>
        <source>Host command</source>
        <translation>Хост команди</translation>
    </message>
    <message>
        <source>Execute a command on the running host as root.</source>
        <translation>Виконувати команду на біговому хості як корінь.</translation>
    </message>
    <message>
        <source>Execute a command inside the selected repair system as root.</source>
        <translation>Виконувати команду в обраній системі ремонту як корінь.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab (or re-enter Host Maintenance / re-commit the repair target) to authorize this session.</source>
        <translation>Авторизація адміністратора; натиснути Авторизації на вкладці Системи або Ремонт (або ре-enter Hostservice / перезвітати попередню ціль) для авторизації цієї сесії.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root on the running host through the helper&apos;s guarded host-shell verb.</source>
        <translation>Запуск одного рецензованої команди як корінь на біговому хості через службу підтримки.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root inside the target chroot through the helper&apos;s guarded shell verb.</source>
        <translation>Виконайте один рецензований команду як корінь всередині цільового chroot через службу підтримки.</translation>
    </message>
    <message>
        <source>Run a command on the running host as root (sudo is not needed). Commands are executed directly on the active system; output is kept in this window and in the application log.</source>
        <translation>Запустіть команду на біговому хості як корінь (sudo не потрібно). Команди виконуються безпосередньо на активній системі; вихід зберігається в цьому вікні і в журналі додатків.</translation>
    </message>
    <message>
        <source>Run a command inside the selected repair system as root (sudo is not needed). Commands are executed one at a time in a fresh chroot and cannot answer interactive prompts; use non-interactive flags such as apt-get -y upgrade. Output is kept in this window and in the application log.</source>
        <translation>Запуск команди всередині вибраної системи ремонту як корінь (sudo не потрібно). Команди виконують один раз в свіжому вигляді і не можуть відповісти на інтерактивні підказки; використовувати неактивні прапори, такі як apt-get -y оновлення. Вихід зберігається в цьому вікні і в журналі додатків.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize or re-enter Host Maintenance / commit a repair target.</source>
        <translation>Не активовано сеанс адміністратора, натиснути Авторизацію або повторно-enter Хост Обслуговування / впорядкувати завдання ремонту.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from the repaired system</source>
        <translation>1. Виберіть вихідні файли або папки з системи ремонту</translation>
    </message>
    <message>
        <source>2. Choose destination on this host</source>
        <translation>2. Виберіть пункт призначення на такому господарстві</translation>
    </message>
    <message>
        <source>Add File Path...</source>
        <translation>Додати шлях...</translation>
    </message>
    <message>
        <source>Add Folder Path...</source>
        <translation>Додати складний шлях ...</translation>
    </message>
    <message>
        <source>Browse...</source>
        <translation>Поза «69»</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it (Host Maintenance does not provide a repair tree to browse).</source>
        <translation>Виберіть привід ремонту в системах, перш ніж вибрати пункт призначення всередині нього (Послуга не забезпечує ремонт дерева для перегляду).</translation>
    </message>
    <message>
        <source>Choose a host destination folder directly.</source>
        <translation>Виберіть папку призначення хосту безпосередньо.</translation>
    </message>
    <message>
        <source>Copy the staged files and folders with cp -a, restore ownership with chown --reference and byte-compare every regular file afterwards.</source>
        <translation>Скопіюйте поетапні файли і папки з cp -a, відновляйте власність з chown --reference and byte-compare кожного регулярного файлу після</translation>
    </message>
    <message>
        <source>File Copy unavailable</source>
        <translation>Файл Копіювання 0</translation>
    </message>
    <message>
        <source>Choose host destination folder</source>
        <translation>Виберіть папку призначення ведучих</translation>
    </message>
    <message>
        <source>Select a repair target</source>
        <translation>Оберіть кінцевий варіант</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it.</source>
        <translation>Виберіть привід ремонту в системах, перш ніж вибрати пункт призначення всередині нього.</translation>
    </message>
    <message>
        <source>File Copy - Browse Target Folders</source>
        <translation>File Copy - Перегляд цільових складників</translation>
    </message>
    <message>
        <source>Browse Target Folders</source>
        <translation>Перегляд цільових складників</translation>
    </message>
    <message>
        <source>The helper could not list the repair-system folder:

%1</source>
        <translation>Помічник не може перерахувати папку ремонтно-системи:

%1</translation>
    </message>
    <message>
        <source>BROWSE_ENTRY</source>
        <translation>Бруней Даруссалам</translation>
    </message>
    <message>
        <source>(choose this folder: %1)</source>
        <translation>(Вибрати цю папку: %1)</translation>
    </message>
    <message>
        <source>(parent folder)</source>
        <translation>(парентна папка)</translation>
    </message>
    <message>
        <source>Select repair-system destination</source>
        <translation>Виберіть пункт призначення системи ремонту</translation>
    </message>
    <message>
        <source>Choose a destination folder inside the repaired system (current folder: %1):</source>
        <translation>Виберіть папку призначення всередині системи ремонту (поточна папка: %1):</translation>
    </message>
    <message>
        <source>Folder</source>
        <translation>Склад</translation>
    </message>
    <message>
        <source>Open</source>
        <translation>Відкрито</translation>
    </message>
    <message>
        <source>(choose this folder:</source>
        <translation>(Вибрати цю папку:</translation>
    </message>
    <message>
        <source>Add files to copy</source>
        <translation>Додати файли до копіювання</translation>
    </message>
    <message>
        <source>Add folder to copy</source>
        <translation>Додати папку для копіювання</translation>
    </message>
    <message>
        <source>Stage at least one source and name a destination path first.</source>
        <translation>Етап принаймні один джерело і ім&apos;я пункту призначення першим.</translation>
    </message>
    <message>
        <source>Copy the %1 staged item(s) to %2?

The helper keeps its direction and path containment checks; a sensitive repair-system destination is refused unless the helper approves it, and every regular file is byte-compared after the copy.</source>
        <translation>Скопіювати %1 поетапний елемент (s) до %2?

Помічник зберігає свій напрямок та контроль за домовленістю шляху; відмовлено у відставці, якщо помічник затверджує його, і кожен регулярний файл, який знаходиться після копіювання.</translation>
    </message>
    <message>
        <source>File Copy - Copy and Verify</source>
        <translation>Статус на сервери</translation>
    </message>
    <message>
        <source>File Copy - Preview Changes</source>
        <translation>Статус на сервери</translation>
    </message>
    <message>
        <source>Protected running host - details</source>
        <translation>Захищений ходовий господар - деталі</translation>
    </message>
    <message>
        <source>not mounted (offline target)</source>
        <translation>не монтується (попередня мета)</translation>
    </message>
    <message>
        <source>Select a drive to see its details.</source>
        <translation>Виберіть диск, щоб побачити його деталі.</translation>
    </message>
    <message>
        <source>Inspecting the selected component.</source>
        <translation>Перевірка вибраного компонента.</translation>
    </message>
    <message>
        <source>Helper-confirmed by the last read-only diagnostics.</source>
        <translation>Допомагач-підтвердження останньої діагностики.</translation>
    </message>
    <message>
        <source>Read-only inventory only; run diagnostics to confirm.</source>
        <translation>Тільки для перевірки запасів;</translation>
    </message>
    <message>
        <source>PROTECTED - running system; read-only details only</source>
        <translation>PROTECTED - система керування; тільки деталі для читання</translation>
    </message>
    <message>
        <source>Live / installer media - not selectable</source>
        <translation>Live / інсталятор медіа - не вибрано</translation>
    </message>
    <message>
        <source>PROTECTED - running host scope</source>
        <translation>PROTECTED - пускова база</translation>
    </message>
    <message>
        <source>Unlock required before selection</source>
        <translation>Розблокувати потрібно до вибору</translation>
    </message>
    <message>
        <source>Eligible repair candidate</source>
        <translation>Обов&apos;язковий ремонт кандидата</translation>
    </message>
    <message>
        <source>Drive:</source>
        <translation>Привід:</translation>
    </message>
    <message>
        <source>Detected target:</source>
        <translation>Визначена мета:</translation>
    </message>
    <message>
        <source>Pending inspection</source>
        <translation>Попередній огляд</translation>
    </message>
    <message>
        <source>Model / label:</source>
        <translation>Модель / етикетка:</translation>
    </message>
    <message>
        <source>Status:</source>
        <translation>Статус:</translation>
    </message>
    <message>
        <source>Size:</source>
        <translation>Розмір:</translation>
    </message>
    <message>
        <source>Connection:</source>
        <translation>Підключення:</translation>
    </message>
    <message>
        <source>Filesystem:</source>
        <translation>Файлова система:</translation>
    </message>
    <message>
        <source>UUID:</source>
        <translation>UUID:</translation>
    </message>
    <message>
        <source>Mounts:</source>
        <translation>Гори:</translation>
    </message>
    <message>
        <source>Running system protection unresolved</source>
        <translation>Захист пускової системи нерозчинний</translation>
    </message>
    <message>
        <source>No protected physical backing disk was identified</source>
        <translation>Виявлено не захищений фізичний задній диск</translation>
    </message>
    <message>
        <source>Current running Linux system</source>
        <translation>Поточний запуск Linux системи</translation>
    </message>
    <message>
        <source>Critical mounts: %1</source>
        <translation>Критичні кріплення: %1</translation>
    </message>
    <message>
        <source>Read-only protected-running-system facts: the helper OS fact plus the inventory model, device path, size, transport and critical mounts. Nothing here is probed destructively.</source>
        <translation>Читання захищених даних-систем: помічник OS-фахівець плюс модель інвентаризації, шлях пристрою, розмір, транспортні та критичні кріплення. Ніщо тут проводиться деструктивно.</translation>
    </message>
    <message>
        <source>Select a drive to see unlock status.</source>
        <translation>Виберіть диск, щоб побачити статус розблокування.</translation>
    </message>
    <message>
        <source>State: protected
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
The protected running host cannot be unlocked or modified; unlock is available for an offline repair target only. Use Host Maintenance for the protected running host.</source>
        <translation>Стан: захищений
Компонент: %1
Мапер: (не)
Спосіб: розблокування помічника (cryptsetup; passphrase через мод-600 keyfile, видалений після використання)
Захищений ведучий не може бути розблокований або модифікований; розблокування доступна тільки для автономного ремонту. Використовуйте службу хост для захищеного хосту.</translation>
    </message>
    <message>
        <source>(none detected)</source>
        <translation>(не виявлено)</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
A locked LUKS container is visible on this drive; press Unlock to open it for this recovery session.</source>
        <translation>Стан: замкнений
Компонент: %1
Мапер: (не)
Спосіб: розблокування помічника (cryptsetup; passphrase через мод-600 keyfile, видалений після використання)
Замкнений контейнер LUKS видно на цьому диску; натисніть розблокувати, щоб відкрити його для цієї сесії відновлення.</translation>
    </message>
    <message>
        <source>(visible mapper)</source>
        <translation>(видимий картпер)</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: already open before this session (visible mapper; Boot Bitch will reuse it and will not close it).</source>
        <translation>Стан: розблокований
Компонент: %1
Оператор: %2
Методика: вже відкрита перед цим сеансом (видима картпера; Boot Bitch перетворить її і не закриє її).</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper-confirmed mapping from the last read-only diagnostics.</source>
        <translation>Стан: розблокований
Компонент: %1
Оператор: %2
Метод: довідник-підтверджене картування з останньої ознайомчої діагностики.</translation>
    </message>
    <message>
        <source>(mapper)</source>
        <translation>(папер)</translation>
    </message>
    <message>
        <source>State: locked or no encrypted component detected
Component: (none detected)
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
No locked LUKS component and no unlock operation were recorded for this drive in the current session.</source>
        <translation>Стан: виявлений замкнений або не зашифрований компонент
Компонент: (не виявлено)
Мапер: (не)
Спосіб: розблокування помічника (cryptsetup; passphrase через мод-600 keyfile, видалений після використання)
Немає замкненого компонента LUKS і не розблокувати операцію були записані для цього приводу в поточному сеансі.</translation>
    </message>
    <message>
        <source>unavailable - %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Host maintenance:
%1</source>
        <translation>Обслуговування гостей:
%1</translation>
    </message>
    <message>
        <source>Target:
%1 + %2</source>
        <translation>Мета:
%1 + %2</translation>
    </message>
    <message>
        <source>Authorization required: diagnostics and repairs fail closed until you press Authorize.</source>
        <translation>Авторизація вимагає: діагностика та ремонт не закривається до моменту натискання Авторизації.</translation>
    </message>
    <message>
        <source>Re-establish the privileged helper session for the current scope now. The password is requested in the hidden-input modal and is never logged.</source>
        <translation>Відновити примітивну програму помічника для поточного обсягу. Пароль запитується в модалі прихованого введення і ніколи не записується.</translation>
    </message>
    <message>
        <source>Running...</source>
        <translation>Запуск</translation>
    </message>
    <message>
        <source>Scope required</source>
        <translation>Обов&apos;язкові поля</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope; this unlocks the gated actions.</source>
        <translation>Запустіть всі - запустіть всі доступні чителю-тільки діагностичні для поточного обсягу; це розблокує воротарські дії.</translation>
    </message>
    <message>
        <source>Select a target and wait for any running command first.</source>
        <translation>Виберіть ціль і почекайте будь-яку команду.</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic through the helper.</source>
        <translation>Запустіть діагностику - запустіть вибрану діагностику через помічник.</translation>
    </message>
    <message>
        <source>Run All for the current scope and refresh the read-only backend profile.</source>
        <translation>Запустіть все для поточного обсягу і освіжайте чителю-на-тільки задньої профілю.</translation>
    </message>
    <message>
        <source>Read or edit the selected target configuration file through the guarded helper; a saved edit invalidates cached diagnostics.</source>
        <translation>Перевірити або редагувати вибраний файл конфігурації цілі через захищений помічник; збережені редагування недійсних кешування.</translation>
    </message>
    <message>
        <source>Host Maintenance has no target-file editing; commit an offline repair target first.</source>
        <translation>Ведуться роботи з технічного обслуговування, які не мають цільового редагування; впорядковуються в автономному режимі.</translation>
    </message>
    <message>
        <source>Commit an offline repair target first.</source>
        <translation>Прийміть в автономному ремонті мішень першим.</translation>
    </message>
    <message>
        <source>No target configuration file is available for this target; run diagnostics to probe the list.</source>
        <translation>Не доступний файл налаштування цільової конфігурації для цієї мети; запустіть діагностику для виявлення списку.</translation>
    </message>
    <message>
        <source>Already Unlocked</source>
        <translation>Вже розблоковано</translation>
    </message>
    <message>
        <source>An unlocked Linux filesystem is already visible on this drive. Boot Bitch will reuse the existing mapper and will not close or reopen a mapping created by this recovery session. Mounting happens during diagnostics (read-only) and repairs (read-write); data filesystems are never auto-mounted on selection.</source>
        <translation>Розблокована файлова система Linux вже відображається на цьому диску. Boot Bitch відтворить існуючу картографію і не закриє або відтворить картографію, створену цією сеансом відновлення. Монтаж відбувається при діагностиці (попередньо) та ремонті (прочитано-відписати); файлові системи даних ніколи не встановлюються на вибір.</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked; use Host Maintenance for the protected running host.</source>
        <translation>Захищений хост не може бути розблокований; використовувати хост Обслуговування для захищеного хосту.</translation>
    </message>
    <message>
        <source>Host Maintenance is the current scope, but the selected offline drive can still be unlocked.</source>
        <translation>Хостове обслуговування - це поточний обсяг, але вибраний диск може бути розблокований.</translation>
    </message>
    <message>
        <source>Unlock %1 using cryptsetup through the privileged helper. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>Розблокувати %1 за допомогою cryptsetup через привілейований помічник. Перехід проходить через приватний ключ і ніколи не розміщується в аргументах або журналах.</translation>
    </message>
    <message>
        <source>The protected running system cannot be selected as a repair target; use Host Maintenance for the protected running host.</source>
        <translation>Система захисту не може бути обрана в якості технічного завдання з ремонту; використовувати службу Хост для захищеного бігу.</translation>
    </message>
    <message>
        <source>Live / installer media is read-only boot media and cannot be selected as a repair target.</source>
        <translation>Живий / інсталяторний медіа прочитано-тільки завантажувальний носій і не можна вибрати в якості технічного завдання.</translation>
    </message>
    <message>
        <source>Unlock the encrypted volume first; Select Target becomes available after a Linux filesystem is detected.</source>
        <translation>Заблокувати зашифрований обсяг спочатку; Виберіть Ціль стає доступним після файлової системи Linux.</translation>
    </message>
    <message>
        <source>Committed repair target. Repair, Diagnostics and File Copy target this physical drive until another drive is explicitly selected with Select Target.</source>
        <translation>Призначення ремонту товарів. Відновлення, діагностика та копіювання файлів, ціль цей фізичний диск, доки інший диск явно вибрано з Виберіть Ціль.</translation>
    </message>
    <message>
        <source>Commit %1 as the repair target; this leaves Host Maintenance and switches the scope to the selected drive.</source>
        <translation>Commit %1 як кінцева робота з ремонтом; це листове обслуговування та переключає обсяг до вибраного приводу.</translation>
    </message>
    <message>
        <source>The running host target could not be detected.</source>
        <translation>Мета роботи не може бути виявлена.</translation>
    </message>
    <message>
        <source>Leave Host Maintenance and return to repair-target mode.</source>
        <translation>Залиште технічне обслуговування та повернення в режим ремонту.</translation>
    </message>
    <message>
        <source>Select the running host for deliberate guarded maintenance; the administrator authorization is requested here once and cached for the session.</source>
        <translation>Вибираємо ходовий хост для навмисного забезпечення опіку; авторизація адміністратора запитується тут один раз і нараховується на сеанс.</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a repair target.</source>
        <translation>Мета роботи не може бути виявлена; діагностика потребує проведення ремонтних цілей.</translation>
    </message>
    <message>
        <source>Committed target: %1 + %2</source>
        <translation>Призначення товару: %1 + %2</translation>
    </message>
    <message>
        <source>Committed target: none (selection changed)</source>
        <translation>Прийнята мета: немає (змінюється виділення)</translation>
    </message>
    <message>
        <source>The selected scope has no resolved Linux root component; Refresh Devices and commit the repair target again.</source>
        <translation>Вибраний обсяг не вирішується кореневим компонентом Linux; перезавантажити пристрої і знову заблокувати поставлену задачу ремонту.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize on the Systems or Repair tab to re-establish it. Run All reuses the cached authorization and never prompts by itself.</source>
        <translation>Не активовано сеанс адміністратора; натиснути кнопку Авторизація на вкладці Системи або ремонт для відновлення його. Виконайте всі перевикористання і ніколи не підкаже себе.</translation>
    </message>
    <message>
        <source>Run diagnostics for this scope to unlock the gated actions. Diagnostics are read-only and the only evidence source.</source>
        <translation>Діагностика запуску для цієї сфери для розблокування воріт. Діагностика читаються і тільки джерело доказів.</translation>
    </message>
    <message>
        <source>A repair was not proven unchanged, so the cached diagnostics are invalidated. Run diagnostics again before another gated action.</source>
        <translation>Ремонт не зарекомендував себе незмінним, тому недійсна діагностика. Проводити діагностику ще до іншої дії воріт.</translation>
    </message>
    <message>
        <source>Gated actions reflect the cached capability lines; the helper still runs every runtime preflight when a command starts.</source>
        <translation>Зібрані дії відображають рядки з кешуванням; помічник все ще працює в режимі реального часу, коли починається командування.</translation>
    </message>
    <message>
        <source>Regenerating diagnostics automatically</source>
        <translation>Відновлення діагностики автоматично</translation>
    </message>
    <message>
        <source>Running diagnostic: %1</source>
        <translation>Модель: %1</translation>
    </message>
    <message>
        <source>Running all diagnostics</source>
        <translation>Запуск всіх діагнозів</translation>
    </message>
    <message>
        <source>Unlocking %1</source>
        <translation>Розблокування %1</translation>
    </message>
    <message>
        <source>running host</source>
        <translation>ходовий хост</translation>
    </message>
    <message>
        <source>offline target</source>
        <translation>в автономному режимі</translation>
    </message>
    <message>
        <source>host maintenance active</source>
        <translation>Активне обслуговування гостей</translation>
    </message>
    <message>
        <source>repair target committed</source>
        <translation>робота по ремонту</translation>
    </message>
    <message>
        <source>no committed scope</source>
        <translation>не скоєний обсяг</translation>
    </message>
    <message>
        <source>Current session</source>
        <translation>Поточна сесія</translation>
    </message>
    <message>
        <source>Log files (*.log);;All files (*)</source>
        <translation>Файли входу (*.log);</translation>
    </message>
    <message>
        <source>Save log as</source>
        <translation>Зберегти журнал як</translation>
    </message>
    <message>
        <source>About Boot Bitch</source>
        <translation>Про Boot Bitch</translation>
    </message>
    <message>
        <source>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;&lt;p&gt;&lt;b&gt;Developer:&lt;/b&gt; CaptainMorgan12&lt;/p&gt;&lt;p&gt;This GUI is the package&apos;s entry point: a Qt 3.3.x frontend with the ported guarded helper for Debian Etch-era systems.&lt;/p&gt;&lt;p&gt;&lt;b&gt;Guarded repair mode:&lt;/b&gt; read-only diagnostics can inspect either the protected Running Host or an explicitly selected repair drive. The guarded Debian/APT package stages (interrupted configuration, broken dependencies, metadata refresh, upgrade), GRUB-legacy configuration regeneration, LUKS target unlock and guarded target-file editing run through the ported helper after confirmation; Host Maintenance enables the same supported stages natively on the active system after repeating the host identity and boot-mount checks.&lt;/p&gt;&lt;p&gt;The modern-only features - verified File Copy, Btrfs snapshot rollback, EFI/UKI and extlinux repair, boot-stack reconciliation and Make Default - are greyed with the helper&apos;s own probe reasons on this frontend; Arch/Alpine/Fedora package backends stay diagnostics-only here.&lt;/p&gt;&lt;p&gt;The first privileged action authorizes one cached administrator session per scope through a hidden-input modal (the Qt GUI remains unprivileged). It can be ended at any time from File - Lock Administrator Session.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;&lt;p&gt;&lt;b&gt;Developer:&lt;/b&gt; КапітанMorgan12&lt;/p&gt;&lt;p&gt; Цей графічний інтерфейс є пунктом входу пакета: Qt 3.3.x переданий з захищеним охоронцем для Debian Etch-era систем.&lt;/p&gt;&lt;p&gt;&lt;p&gt;&lt;b&gt;Підібраний режим ремонту:&lt;/b&gt; Read-only діагностика може перевіряти або захищений пусковий хост або явно вибраний привід ремонту. Стійки пакету Debian/APT (змінна конфігурація, розбиті залежності, оновлення метаданих, оновлення), регенерація конфігурації GRUB-legacy, розблокування цілі LUKS та захист редагування цільового профілю, що виконується через портовий помічник після підтвердження; Обслуговування Host дозволяє тим самим підтримувані етапи нативно від активної системи після повторення ідентичності та перевірки завантаження.&lt;/p&gt;&lt;p&gt; Сучасні функції - перевірений файл Копіювання, Btrfs знімок, EFI/UKI і extlinux ремонт, завантажувального конденсату і зробити за замовчуванням - сірі з власними причинами зонду на цьому фронті; арка/Alpine/Fedora посилається на діагностику. &lt;/p&gt;&lt;p&gt; Перша привілейована дія авторизації одного окремого адміністратора за обсягом через модуль прихованого введення (Qt GUI залишається незареєстрованим). Він може бути завершений в будь-який час з файлу - Сесія адміністратора блокування.&lt;/p&gt;</translation>
    </message>
</context>
</TS>
