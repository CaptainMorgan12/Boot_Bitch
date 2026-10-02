<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS language="es">
<context>
    <name>LegacyMainWindow</name>
    <message>
        <source>Validate environment</source>
        <translation>Validar el entorno</translation>
    </message>
    <message>
        <source>Validate</source>
        <translation>Validar</translation>
    </message>
    <message>
        <source>Check the selected system&apos;s mounts, filesystem metadata, boot files, mapper consistency and dependency readiness before any repair action. This is an independent safety preflight rather than an optional Full Repair stage.</source>
        <translation>Revise los montajes del sistema seleccionado, metadatos del sistema de archivos, archivos de arranque, consistencia de mapper y disponibilidad de dependencia antes de cualquier acción de reparación. Este es un preflight de seguridad independiente en lugar de una etapa de reparación completa opcional.</translation>
    </message>
    <message>
        <source>Always preflight</source>
        <translation>Siempre antes del vuelo</translation>
    </message>
    <message>
        <source>File system repair</source>
        <translation>Reparación del sistema de archivos</translation>
    </message>
    <message>
        <source>Check File Systems</source>
        <translation>Verificar sistemas de archivos</translation>
    </message>
    <message>
        <source>Run the read-only file system check for the selected system&apos;s root and /boot filesystems and report each device&apos;s check tool and result without changing anything. This legacy frontend exposes the read-only check only; device repair is not wired.</source>
        <translation>Ejecute el sistema de archivos de sólo lectura para los sistemas de root y /boot del sistema seleccionado y informe la herramienta de verificación y el resultado de cada dispositivo sin cambiar nada. Este frontend heredado expone el solo control de lectura; la reparación del dispositivo no está cableada.</translation>
    </message>
    <message>
        <source>Full Repair plan: unavailable on this frontend - read-only check only</source>
        <translation>Plan de reparación completo: indisponible en este frontend - sólo lectura sólo cheque</translation>
    </message>
    <message>
        <source>Complete package configuration</source>
        <translation>Completar la configuración de paquetes</translation>
    </message>
    <message>
        <source>Complete Configuration</source>
        <translation>Completar la configuración</translation>
    </message>
    <message>
        <source>Complete interrupted dpkg package configuration in the selected repair system. This is the same stage controlled by Settings -&gt; Full Repair plan -&gt; Complete interrupted package configuration, but it can also be run independently here.</source>
        <translation>Configuración completa interrumpida del paquete dpkg en el sistema de reparación seleccionado. Esta es la misma etapa controlada por Settings - título Plan de reparación completo - título Configuración completa interrumpida de paquetes, pero también puede ejecutarse independientemente aquí.</translation>
    </message>
    <message>
        <source>Run the guarded dpkg-configure repair? The helper keeps its package-lock and runtime preflights.</source>
        <translation>Ejecutar la reparación vigilada dpkg-configure? El ayudante mantiene su paquete-lock y preflights de tiempo de ejecución.</translation>
    </message>
    <message>
        <source>Repair broken dependencies</source>
        <translation>Reparar dependencias rotas</translation>
    </message>
    <message>
        <source>Repair Dependencies</source>
        <translation>Reparar dependencias</translation>
    </message>
    <message>
        <source>Repair package dependencies in the selected repair system after the mandatory safety preflight. This maps directly to Settings -&gt; Repair broken package dependencies.</source>
        <translation>Reparar las dependencias del paquete en el sistema de reparación seleccionado después del preflight de seguridad obligatorio. Mapas directamente a Settings - confiar Reparar dependencias de paquetes rotas.</translation>
    </message>
    <message>
        <source>Run the guarded fix-broken repair? The helper keeps its simulation-first preflight and runtime guards.</source>
        <translation>¿Ejecutar la reparación cerrada? El ayudante mantiene su simulación-primer preflight y guardias de tiempo de ejecución.</translation>
    </message>
    <message>
        <source>Refresh package metadata</source>
        <translation>Actualizar metadatos de paquetes</translation>
    </message>
    <message>
        <source>Refresh Metadata</source>
        <translation>Actualizar metadatos</translation>
    </message>
    <message>
        <source>Refresh APT metadata in the selected repair system without upgrading installed packages. This maps directly to Settings -&gt; Refresh package metadata.</source>
        <translation>Refresh Metadatos APT en el sistema de reparación seleccionado sin actualizar paquetes instalados. Mapas directamente a Settings - confiar Metadatos de paquetes refrescos.</translation>
    </message>
    <message>
        <source>Refresh package metadata for the selected scope? The helper requires a reachable, trusted APT source.</source>
        <translation>¿Metadatos de paquetes para el alcance seleccionado? El ayudante requiere una fuente APT accesible y confiable.</translation>
    </message>
    <message>
        <source>Upgrade installed packages</source>
        <translation>Actualizar paquetes instalados</translation>
    </message>
    <message>
        <source>Simulate and Upgrade</source>
        <translation>Simular y actualizar</translation>
    </message>
    <message>
        <source>Simulate the APT transaction first, inspect proposed removals, then apply a safe upgrade. This maps directly to Settings -&gt; Upgrade installed packages.</source>
        <translation>Simular la transacción APT primero, inspeccionar las absorciones propuestas, luego aplicar una actualización segura. Mapas directamente a Settings - confiar Actualizar paquetes instalados.</translation>
    </message>
    <message>
        <source>Run the guarded apt-upgrade transaction? The helper keeps its simulation-first and source guards.</source>
        <translation>¿Ejecutar la transacción vigilada apt-upgrade? El ayudante mantiene sus guardias de simulación primero y fuente.</translation>
    </message>
    <message>
        <source>DKMS</source>
        <translation>DKMS</translation>
    </message>
    <message>
        <source>Rebuild DKMS</source>
        <translation>Reconstruir DKMS</translation>
    </message>
    <message>
        <source>Rebuild out-of-tree kernel modules for kernels installed in the selected system. The helper refuses this action when DKMS is not installed; this legacy frontend exposes no DKMS action.</source>
        <translation>Recuperar módulos de núcleo fuera de árbol para núcleos instalados en el sistema seleccionado. El ayudante rechaza esta acción cuando DKMS no está instalado; este frontend legado no expone ninguna acción DKMS.</translation>
    </message>
    <message>
        <source>Graphical login / display manager</source>
        <translation>Acceso gráfico / gestor de pantalla</translation>
    </message>
    <message>
        <source>Restore Graphical Login</source>
        <translation>Restaurar el acceso gráfico</translation>
    </message>
    <message>
        <source>Restore the legacy SysV display manager configured for the running host: the /etc/X11/default-display-manager entry and the missing runlevel S-symlink, with a backup and rollback, never starting the GUI. This is a host-scope stage on this legacy frontend.</source>
        <translation>Restaurar el administrador de pantalla SysV configurado para el host en ejecución: la entrada /etc/X11/default-display-manager y el S-symlink de nivel de ejecución perdido, con una copia de seguridad y devolución, nunca comenzando el GUI. Esta es una etapa de anfitriona en este frontend legado.</translation>
    </message>
    <message>
        <source>Restore the graphical login configuration for the running host? The helper backs up /etc/X11/default-display-manager and the runlevel symlink state, restores the configured entry and the missing S-symlink, rolls back on any failure, and never starts the display manager.</source>
        <translation>¿Restorear la configuración gráfica de inicio de sesión para el host en ejecución? El ayudante retrocede /etc/X11/default-display-manager y el estado de symlink de nivel de ejecución, restaura la entrada configurada y el S-symlink desaparecido, vuelve a rodar en cualquier fallo, y nunca comienza el gestor de pantalla.</translation>
    </message>
    <message>
        <source>Initramfs</source>
        <translation>Initramfs</translation>
    </message>
    <message>
        <source>Rebuild Initramfs</source>
        <translation>Reconstruir initramfs</translation>
    </message>
    <message>
        <source>Rebuild initramfs images for the selected repair system only after mapper and crypttab consistency checks pass. The helper backs up each image before the apply. On Etch the stage runs through the guarded plain-chroot fallback (no unshare required).</source>
        <translation>Recuperar las imágenes initramfs para el sistema de reparación seleccionado sólo después de que pasen las comprobaciones de consistencia de mapper y crypttab. El ayudante respalda cada imagen antes de la aplicación. En Etch el escenario corre a través del retroceso plano-chroot vigilado (no se requiere inquebrantable).</translation>
    </message>
    <message>
        <source>Rebuild the initramfs for the selected scope? The helper keeps its mapper/crypttab and backup preflights.</source>
        <translation>¿Reconstruir el initramfs para el alcance seleccionado? El ayudante mantiene sus preflights de mapper/crypttab y backup.</translation>
    </message>
    <message>
        <source>EFI / UKI bootloader</source>
        <translation>Gestor de arranque EFI / UKI</translation>
    </message>
    <message>
        <source>Repair EFI / UKI</source>
        <translation>Reparar EFI / UKI</translation>
    </message>
    <message>
        <source>Repair the selected system&apos;s EFI / UKI boot path. This legacy frontend exposes no EFI action; the Etch target is a BIOS/GRUB-legacy system.</source>
        <translation>Reparar la ruta de arranque EFI / UKI del sistema seleccionado. Este frontend legado no expone ninguna acción EFI; el objetivo Etch es un sistema BIOS/GRUB-legacy.</translation>
    </message>
    <message>
        <source>GRUB configuration</source>
        <translation>Configuración de GRUB</translation>
    </message>
    <message>
        <source>Regenerate GRUB</source>
        <translation>Regenerar GRUB</translation>
    </message>
    <message>
        <source>Regenerate the selected repair system&apos;s GRUB menu/configuration after the mandatory safety preflight. The helper backs up menu.lst, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Regenerar el menú/configuración del sistema de reparación seleccionado después del preluz de seguridad obligatorio. El ayudante respalda menu.lst, preserva cada entrada de arranque existente y vuelve a rodar en cualquier fallo.</translation>
    </message>
    <message>
        <source>Regenerate the GRUB configuration? The helper backs up the target menu/configuration, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>¿Regenerar la configuración GRUB? El ayudante respalda el menú de destino/configuración, preserva cada entrada de arranque existente y vuelve a rodar en cualquier fallo.</translation>
    </message>
    <message>
        <source>extlinux configuration</source>
        <translation>Configuración extlinux</translation>
    </message>
    <message>
        <source>Regenerate extlinux</source>
        <translation>Regenerar extlinux</translation>
    </message>
    <message>
        <source>Regenerate the selected system&apos;s extlinux bootloader configuration. This legacy frontend exposes no extlinux action.</source>
        <translation>Regenerar la configuración de arranque extlinux del sistema seleccionado. Este frontend legado no expone ninguna acción extlinux.</translation>
    </message>
    <message>
        <source>Boot stack reconciliation</source>
        <translation>Reconciliación de la pila de arranque</translation>
    </message>
    <message>
        <source>Reconcile Boot Stack</source>
        <translation>Reconciliar la pila de arranque</translation>
    </message>
    <message>
        <source>Reconcile the selected repair system&apos;s boot stack in one guarded legacy pass: mapper/crypttab validation, initramfs rebuild and GRUB-legacy configuration regeneration, with the component backups and preflights unchanged. This is the Etch equivalent of the modern boot-stack reconciliation and stays out of the Full Repair plan.</source>
        <translation>Reconcile la pila de arranque del sistema de reparación seleccionada en un pase de legado vigilado: validación de mapper/crypttab, reconstrucción initramfs y regeneración de configuración de la división GRUB, sin cambios en las copias de seguridad de los componentes y los preflights. Este es el equivalente Etch de la moderna reconciliación de arranque y se mantiene fuera del plan de reparación completa.</translation>
    </message>
    <message>
        <source>Full Repair plan: Manual recovery tool</source>
        <translation>Plan de reparación completo: herramienta de recuperación manual</translation>
    </message>
    <message>
        <source>Run the guarded boot-stack reconciliation? The helper runs the mapper/crypttab validation, the initramfs rebuild and the GRUB-legacy regeneration in one pass with every component preflight and backup.</source>
        <translation>¿Ejecutar la reconciliación de arranque vigilada? El ayudante ejecuta la validación de mapper/crypttab, la reconstrucción initramfs y la regeneración del legado GRUB en un solo paso con cada componente de preluz y respaldo.</translation>
    </message>
    <message>
        <source>Complete interrupted package configuration</source>
        <translation>Completar la configuración de paquetes interrumpida</translation>
    </message>
    <message>
        <source>Repair broken package dependencies</source>
        <translation>Reparar dependencias de paquetes rotas</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Establecer como predeterminado</translation>
    </message>
    <message>
        <source>Repair file system errors (read-only check first)</source>
        <translation>Reparar errores del sistema de archivos (primero verificación de solo lectura)</translation>
    </message>
    <message>
        <source>the legacy frontend exposes the read-only file system check only; per-device repair is not wired on this frontend (fail closed)</source>
        <translation>el frontend heredado expone el sistema de archivos sólo de lectura; la reparación por dispositivo no se conecta en este frontend (fail cerrado)</translation>
    </message>
    <message>
        <source>Upgrade installed packages (adaptive APT simulation)</source>
        <translation>Actualizar paquetes instalados (simulación APT adaptativa)</translation>
    </message>
    <message>
        <source>Rebuild DKMS modules</source>
        <translation>Reconstruir los módulos DKMS</translation>
    </message>
    <message>
        <source>Restore graphical login manager</source>
        <translation>Restore graphical login manager</translation>
    </message>
    <message>
        <source>Rebuild initramfs after mapper/crypttab validation</source>
        <translation>Reconstruir initramfs tras la validación de mapper/crypttab</translation>
    </message>
    <message>
        <source>Repair EFI / UKI boot path</source>
        <translation>Reparación EFI / vía de arranque UKI</translation>
    </message>
    <message>
        <source>Update GRUB configuration</source>
        <translation>Actualizar la configuración de GRUB</translation>
    </message>
    <message>
        <source>Update extlinux configuration</source>
        <translation>Actualizar la configuración extlinux</translation>
    </message>
    <message>
        <source>Choose Full Repair stages in Settings. Enabled stages run in the order shown. Each configurable stage also appears below as an individual tool; the Full Repair column mirrors its current Settings state. The boot tools (EFI / UKI bootloader, GRUB or extlinux configuration, boot-stack reconciliation and Make Default) are independent: run them in any order, and a later action re-verifies what an earlier one changed and reports its own result. The active scope is shown beside Repair: selected repair drive or Running Host maintenance.</source>
        <translation>Elige etapas de reparación completas en Ajustes. Las etapas habilitadas se ejecutan en el orden mostrado. Cada etapa configurable también aparece abajo como una herramienta individual; la columna de reparación completa refleja su estado de configuración actual. Las herramientas de arranque (EFI / UKI bootloader, configuración GRUB o extlinux, reconciliación de arranque y Make Default) son independientes: ejecutarlas en cualquier orden, y una acción posterior reverifica lo que un anterior cambió e informa de su propio resultado. El alcance activo se muestra junto a Reparación: unidad de reparación seleccionada o mantenimiento Running Host.</translation>
    </message>
    <message>
        <source>Run All diagnostics for the selected target or running host before starting Full Repair. The report is read-only evidence used to choose and confirm repair stages.</source>
        <translation>Ejecute Todos los diagnósticos para el objetivo seleccionado o el anfitrión en ejecución antes de iniciar la reparación completa. El informe es solo una prueba usada para elegir y confirmar etapas de reparación.</translation>
    </message>
    <message>
        <source>Ready: required cached read-only diagnostics are available for the selected stages. Review them in Diagnostics or Logs before confirming.</source>
        <translation>Listo: los diagnósticos de sólo lectura caché requeridos están disponibles para las etapas seleccionadas. Reviselos en Diagnósticos o Registros antes de confirmar.</translation>
    </message>
    <message>
        <source>Environment validation</source>
        <translation>Validación del entorno</translation>
    </message>
    <message>
        <source>Summarizes the selected system, protection state, mounted identity and inspection readiness.</source>
        <translation>Resume el sistema seleccionado, estado de protección, disposición de identidad montada e inspección.</translation>
    </message>
    <message>
        <source>Distribution and boot backend profile</source>
        <translation>Distribución y perfil de backend de arranque</translation>
    </message>
    <message>
        <source>Identifies the distribution family, package manager, initramfs generator, bootloader and current guarded repair capability.</source>
        <translation>Identifica la familia de distribución, gestor de paquetes, generador initramfs, cargador de arranque y capacidad de reparación vigilada actual.</translation>
    </message>
    <message>
        <source>Boot diagnostics</source>
        <translation>Diagnóstico de arranque</translation>
    </message>
    <message>
        <source>Shows boot mounts and /boot contents plus storage evidence without changing the selected system.</source>
        <translation>Muestra montajes de arranque y /boot contenidos más evidencia de almacenamiento sin cambiar el sistema seleccionado.</translation>
    </message>
    <message>
        <source>Boot evidence and selection history</source>
        <translation>Ejecutar evidencia e historial de selección</translation>
    </message>
    <message>
        <source>Correlates the detected boot chain, bootloader selection, kernel/initramfs and unlock evidence.</source>
        <translation>Correlaciona la cadena de arranque detectada, selección de arranque, kernel/initramfs y desbloquea evidencia.</translation>
    </message>
    <message>
        <source>Kernel / initramfs</source>
        <translation>Kernel / initramfs</translation>
    </message>
    <message>
        <source>Reviews kernel files and verifies matching initramfs images through a read-only inspection.</source>
        <translation>Reseña los archivos del núcleo y verifica que las imágenes initramfs coincidan a través de una inspección sólo lectura.</translation>
    </message>
    <message>
        <source>Reviews the GRUB configuration without changing boot files.</source>
        <translation>Reseña la configuración GRUB sin cambiar los archivos de arranque.</translation>
    </message>
    <message>
        <source>EFI / UKI boot state</source>
        <translation>EFI / estado de arranque UKI</translation>
    </message>
    <message>
        <source>Inspects EFI/UKI evidence; unavailable on this legacy BIOS frontend with the helper&apos;s probe reason.</source>
        <translation>Inspecciona la evidencia EFI/UKI; no disponible en este frontend BIOS legado con la razón de sonda del ayudante.</translation>
    </message>
    <message>
        <source>Reviews the configured display manager and recent boot evidence without starting the GUI.</source>
        <translation>Reseña el gestor de pantalla configurado y las recientes pruebas de arranque sin iniciar el GUI.</translation>
    </message>
    <message>
        <source>Boot errors</source>
        <translation>Errores de arranque</translation>
    </message>
    <message>
        <source>Reads recent error-priority entries from the running host or selected repair system when available.</source>
        <translation>Lea las recientes entradas de prioridad de error del host en ejecución o sistema de reparación seleccionado cuando esté disponible.</translation>
    </message>
    <message>
        <source>Disk usage</source>
        <translation>Uso del disco</translation>
    </message>
    <message>
        <source>Summarizes filesystem capacity and free space for the running host or read-only repair target.</source>
        <translation>Summarizes capacidad del sistema de archivos y espacio libre para el host en funcionamiento o objetivo de reparación sólo lectura.</translation>
    </message>
    <message>
        <source>File systems</source>
        <translation>Sistemas de archivos</translation>
    </message>
    <message>
        <source>Runs the read-only file system check for the selected system&apos;s root, /boot and other filesystems.</source>
        <translation>Ejecute el sistema de archivos sólo lectura para la raíz del sistema seleccionado, /boot y otros sistemas de archivos.</translation>
    </message>
    <message>
        <source>/etc/fstab review</source>
        <translation>/etc/fstab review</translation>
    </message>
    <message>
        <source>Displays the running host or selected repair system&apos;s fstab; repair-system inspection is mounted read-only.</source>
        <translation>Muestra la frestab del host en funcionamiento o el sistema de reparación seleccionado; la inspección del sistema de reparación se monta solo lectura.</translation>
    </message>
    <message>
        <source>Btrfs status</source>
        <translation>Situación Btrfs</translation>
    </message>
    <message>
        <source>Shows Btrfs filesystem and subvolume information when the target uses Btrfs.</source>
        <translation>Muestra información de sistema de archivos Btrfs y subvolumen cuando el objetivo utiliza Btrfs.</translation>
    </message>
    <message>
        <source>Device-mapper ancestry</source>
        <translation>Ancestro de la máquina de dispositivo</translation>
    </message>
    <message>
        <source>Shows selected mapper ancestry and device-mapper state when available.</source>
        <translation>Muestra acestía de mapper seleccionada y estado de máquina de dispositivo cuando esté disponible.</translation>
    </message>
    <message>
        <source>LUKS / crypttab evidence</source>
        <translation>LUKS / evidencia crypttab</translation>
    </message>
    <message>
        <source>Shows LUKS/mapped ancestry plus crypttab and fstab mapper references.</source>
        <translation>Muestra las referencias LUKS/mapped ancestry más crypttab y fstab mapper.</translation>
    </message>
    <message>
        <source>Full diagnostic report</source>
        <translation>Informe completo de diagnóstico</translation>
    </message>
    <message>
        <source>Combines all read-only diagnostics for the selected scope (same as Run All).</source>
        <translation>Combina todos los diagnósticos sólo leídos para el alcance seleccionado (igual que Run All).</translation>
    </message>
    <message>
        <source>All entries</source>
        <translation>Todas las entradas</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>Diagnóstico</translation>
    </message>
    <message>
        <source>Repairs</source>
        <translation>Reformas</translation>
    </message>
    <message>
        <source>Package repair</source>
        <translation>Reparación de paquetes</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Copia de archivos</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Detección de dispositivos</translation>
    </message>
    <message>
        <source>Host</source>
        <translation>Host</translation>
    </message>
    <message>
        <source>Required for block-device inventory</source>
        <translation>Necesario para el inventario del dispositivo bloqueado</translation>
    </message>
    <message>
        <source>Filesystem identification</source>
        <translation>Identificación del sistema de archivos</translation>
    </message>
    <message>
        <source>Used to identify filesystem metadata</source>
        <translation>Se utiliza para identificar metadatos del sistema de archivos</translation>
    </message>
    <message>
        <source>Mount inspection</source>
        <translation>Inspección de montaje</translation>
    </message>
    <message>
        <source>Used to understand active mounts</source>
        <translation>Se utiliza para entender montajes activos</translation>
    </message>
    <message>
        <source>LUKS support</source>
        <translation>Soporte LUKS</translation>
    </message>
    <message>
        <source>Required to unlock encrypted targets</source>
        <translation>Necesario para desbloquear objetivos cifrados</translation>
    </message>
    <message>
        <source>Btrfs support</source>
        <translation>Soporte Btrfs</translation>
    </message>
    <message>
        <source>Required for Btrfs inspection and snapshot rollback</source>
        <translation>Necesario para la inspección de Btrfs y la devolución de instantáneas</translation>
    </message>
    <message>
        <source>Bidirectional file copy</source>
        <translation>Copia de archivo bidireccional</translation>
    </message>
    <message>
        <source>Host/Repair</source>
        <translation>Host/Repair</translation>
    </message>
    <message>
        <source>Required for verified Host-to-Repair and Repair-to-Host transfer</source>
        <translation>Necesario para la transferencia verificada de Host-to-Repair y Reparación-a-Host</translation>
    </message>
    <message>
        <source>Chroot repair</source>
        <translation>Reparación de cromo</translation>
    </message>
    <message>
        <source>Required for target-side repair commands</source>
        <translation>Se requiere para los comandos de reparación de lado objetivo</translation>
    </message>
    <message>
        <source>Offline systemd repair</source>
        <translation>Reparación del sistema sin conexión</translation>
    </message>
    <message>
        <source>Used to restore graphical.target and the configured display manager without starting the target GUI</source>
        <translation>Solía restaurar gráficamente. objetivo y el gestor de pantalla configurado sin iniciar el GUI objetivo</translation>
    </message>
    <message>
        <source>UEFI NVRAM inspection</source>
        <translation>Inspección UEFI NVRAM</translation>
    </message>
    <message>
        <source>Used to preserve target EFI BootOrder during TUXEDO UKI rebuilds when efivars are available</source>
        <translation>Utilizado para preservar el objetivo EFI BootOrder durante TUXEDO UKI reconstruye cuando los efivares están disponibles</translation>
    </message>
    <message>
        <source>UKI verification</source>
        <translation>Verificación de UKI</translation>
    </message>
    <message>
        <source>Used to verify the kernel embedded in a rebuilt unified kernel image</source>
        <translation>Se utiliza para verificar el núcleo incrustado en una imagen de núcleo unificada reconstruida</translation>
    </message>
    <message>
        <source>GRUB EFI repair</source>
        <translation>GRUB EFI reparación</translation>
    </message>
    <message>
        <source>Target/Host</source>
        <translation>Objetivo/Host</translation>
    </message>
    <message>
        <source>Required only for conventional GRUB-based EFI systems</source>
        <translation>Se requiere sólo para sistemas convencionales de EFI basados en GRUB</translation>
    </message>
    <message>
        <source>Target</source>
        <translation>Meta</translation>
    </message>
    <message>
        <source>Debian-family GRUB helper</source>
        <translation>Ayudante GRUB de la familia Debian</translation>
    </message>
    <message>
        <source>Portable GRUB configuration generator used by Arch and other non-Debian systems</source>
        <translation>Generador de configuración portátil GRUB utilizado por Arch y otros sistemas no Debian</translation>
    </message>
    <message>
        <source>Initramfs rebuild</source>
        <translation>Initramfs reconstrucción</translation>
    </message>
    <message>
        <source>Debian-family initramfs helper</source>
        <translation>Ayudante initramfs de la familia Debian</translation>
    </message>
    <message>
        <source>Arch-family initramfs generator</source>
        <translation>Generador Arch-family initramfs</translation>
    </message>
    <message>
        <source>Alternative initramfs generator used by Arch and other distributions</source>
        <translation>Generador initramfs alternativo utilizado por Arch y otras distribuciones</translation>
    </message>
    <message>
        <source>Initramfs verification</source>
        <translation>Verificación de Initramfs</translation>
    </message>
    <message>
        <source>Read-only verification for mkinitcpio images</source>
        <translation>Verificación de sólo lectura para imágenes mkinitcpio</translation>
    </message>
    <message>
        <source>Read-only verification for dracut images</source>
        <translation>Verificación de sólo lectura para imágenes dracut</translation>
    </message>
    <message>
        <source>systemd-boot inspection</source>
        <translation>Inspección systemd-boot</translation>
    </message>
    <message>
        <source>Host/Target</source>
        <translation>Host/Target</translation>
    </message>
    <message>
        <source>Read-only inspection of systemd-boot and generic UKI layouts</source>
        <translation>Inspección de sólo lectura de systemd-boot y diseños genéricos UKI</translation>
    </message>
    <message>
        <source>Arch package manager</source>
        <translation>Arch package manager</translation>
    </message>
    <message>
        <source>Arch-family package database and transaction tool</source>
        <translation>Base de datos de paquetes de Arch-family y herramienta de transacción</translation>
    </message>
    <message>
        <source>DKMS rebuild</source>
        <translation>Reconstrucción de DKMS</translation>
    </message>
    <message>
        <source>Required only when target uses DKMS modules</source>
        <translation>Se requiere sólo cuando el objetivo utiliza los módulos DKMS</translation>
    </message>
    <message>
        <source>LVM inspection</source>
        <translation>Inspección de vehículos pesados</translation>
    </message>
    <message>
        <source>Optional LVM storage-stack support</source>
        <translation>Soporte opcional para almacenamiento LVM</translation>
    </message>
    <message>
        <source>Software RAID</source>
        <translation>Software RAID</translation>
    </message>
    <message>
        <source>Optional Linux MD RAID support</source>
        <translation>Soporte opcional para Linux MD RAID</translation>
    </message>
    <message>
        <source>Process namespace isolation</source>
        <translation>Proceso de aislamiento espacio-nombre</translation>
    </message>
    <message>
        <source>Host+Target</source>
        <translation>Host+ Meta</translation>
    </message>
    <message>
        <source>The ported helper falls back to a guarded plain chroot when unshare is absent</source>
        <translation>El ayudante portado se cae de nuevo a un chroot vigilado cuando el inshare está ausente</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Cancelar</translation>
    </message>
    <message>
        <source>OK</source>
        <translation>OK</translation>
    </message>
    <message>
        <source>Yes</source>
        <translation>Sí.</translation>
    </message>
    <message>
        <source>No</source>
        <translation>No</translation>
    </message>
    <message>
        <source>read-only helper diagnostic</source>
        <translation>diagnóstico de sólo ayuda</translation>
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
        <translation>Utilidad de recuperación y recuperación de arranque</translation>
    </message>
    <message>
        <source>GUARDED REPAIR</source>
        <translation>REPARACIÓN PROTEGIDA</translation>
    </message>
    <message>
        <source>Ordinary repairs require an explicitly selected non-host target. The protected running host has a separate deliberate maintenance mode with the same guarded repair stages and requires privilege authorization.</source>
        <translation>Las reparaciones ordinarias requieren un objetivo no huésped explícitamente seleccionado. El host en funcionamiento protegido tiene un modo de mantenimiento deliberado separado con las mismas etapas de reparación vigiladas y requiere autorización de privilegios.</translation>
    </message>
    <message>
        <source>Systems</source>
        <translation>Sistemas</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>Diagnóstico</translation>
    </message>
    <message>
        <source>Repair</source>
        <translation>Reparar</translation>
    </message>
    <message>
        <source>Chroot Shell</source>
        <translation>Shell chroot</translation>
    </message>
    <message>
        <source>File Copy</source>
        <translation>Copia de archivos</translation>
    </message>
    <message>
        <source>Logs</source>
        <translation>Registros</translation>
    </message>
    <message>
        <source>Settings</source>
        <translation>Ajustes</translation>
    </message>
    <message>
        <source>(not created yet)</source>
        <translation>(no creado aún)</translation>
    </message>
    <message>
        <source>Information</source>
        <translation>Información</translation>
    </message>
    <message>
        <source>Close</source>
        <translation>Cerca</translation>
    </message>
    <message>
        <source>&amp;File</source>
        <translation>&amp;Archivo</translation>
    </message>
    <message>
        <source>&amp;Refresh Devices</source>
        <translation>&amp;Refresh Devices</translation>
    </message>
    <message>
        <source>&amp;Lock Administrator Session</source>
        <translation>&amp;Lock Administrator Session</translation>
    </message>
    <message>
        <source>&amp;Quit</source>
        <translation>&amp;Quit</translation>
    </message>
    <message>
        <source>&amp;View</source>
        <translation>&amp;Ver</translation>
    </message>
    <message>
        <source>&amp;Systems</source>
        <translation>&amp;Systems</translation>
    </message>
    <message>
        <source>&amp;Diagnostics</source>
        <translation>&amp;Diagnósticos</translation>
    </message>
    <message>
        <source>&amp;Logs</source>
        <translation>&amp;Logs</translation>
    </message>
    <message>
        <source>&amp;Settings</source>
        <translation>&amp;Ajustes</translation>
    </message>
    <message>
        <source>&amp;Auto-size Device Columns</source>
        <translation>&amp;Columnas de dispositivo de tamaño automático</translation>
    </message>
    <message>
        <source>&amp;Wrap Log Lines</source>
        <translation>&amp;Wrap Log Lines</translation>
    </message>
    <message>
        <source>&amp;Help</source>
        <translation>&amp;Ayuda</translation>
    </message>
    <message>
        <source>&amp;Using Boot Bitch</source>
        <translation>&amp;Utilizando Boot Bitch</translation>
    </message>
    <message>
        <source>&amp;About Boot Bitch</source>
        <translation>&amp;Acerca de Boot Bitch</translation>
    </message>
    <message>
        <source>Device columns auto-sized. Drag headers to fine-tune widths.</source>
        <translation>Columnas de dispositivo auto tamaño. Arrastre las cabeceras a anchos finos.</translation>
    </message>
    <message>
        <source>A helper command is running; wait for it to finish before locking the session.</source>
        <translation>Se ejecuta un comando helper; espere a que termine antes de cerrar la sesión.</translation>
    </message>
    <message>
        <source>Administrator session locked; the next privileged action will request authorization.</source>
        <translation>Sesión de administrador cerrada; la siguiente acción privilegiada solicitará autorización.</translation>
    </message>
    <message>
        <source>Using Boot Bitch</source>
        <translation>Utilizando Boot Bitch</translation>
    </message>
    <message>
        <source>Boot Bitch must run from a different booted Linux environment than the system being repaired. Use a Linux live medium or another Linux installation on a different physical drive.&lt;br&gt;&lt;br&gt;The running host is protected from ordinary repair-target selection, but it can be explicitly selected through &lt;b&gt;Host Maintenance&lt;/b&gt; for guarded native diagnostics and supported maintenance stages.&lt;br&gt;&lt;br&gt;Diagnostics follow the Systems page: the committed repair drive while Host Maintenance is off, or the protected running host while it is active.&lt;br&gt;&lt;br&gt;The first privileged action requests administrator authorization once for this Boot Bitch window; &lt;b&gt;File - Lock Administrator Session&lt;/b&gt; ends that helper session immediately. Every repair keeps the helper&apos;s own runtime preflights.</source>
        <translation>Boot Bitch debe funcionar desde un entorno Linux de arranque diferente al sistema que se está reparando. Utilice un medio vivo Linux u otra instalación Linux en una unidad física diferente. El host en funcionamiento está protegido de la selección ordinaria de los objetivos de reparación, pero puede ser seleccionado explícitamente a través de нелентенния mantenimiento/b contacto para los diagnósticos nativos vigilados y las etapas de mantenimiento soportadas. Diagnósticos siguen la página de Sistemas: la unidad de reparación comprometida mientras que Host Maintenance está apagado, o el anfitrión de funcionamiento protegido mientras está activo. La primera acción privilegiada solicita autorización de administrador una vez para esta ventana de Boot Bitch; &lt;b confianzaFile - Lock Administrator Sesión seleccionada/b confiar termina esa sesión de ayuda inmediatamente. Cada reparación mantiene los propios preflights del ayudante.</translation>
    </message>
    <message>
        <source>Select a physical drive; Boot Bitch resolves the most likely Linux system volume automatically. The running host stays protected from ordinary target repairs, with a separate explicit host-maintenance path for its own system. The Details button shows the protected host&apos;s facts in the details pane; selecting any drive row restores the per-drive pane.</source>
        <translation>Seleccione una unidad física; Boot Bitch resuelve automáticamente el volumen del sistema Linux más probable. El host en funcionamiento se mantiene protegido de las reparaciones de destino ordinario, con un camino de mantenimiento explícito separado para su propio sistema. El botón Detalles muestra los hechos del host protegido en el panel de detalles; seleccionar cualquier fila de la unidad restaura el panel por goteo.</translation>
    </message>
    <message>
        <source>Refresh Devices</source>
        <translation>Actualizar dispositivos</translation>
    </message>
    <message>
        <source>Re-read the read-only kernel inventory (/proc/partitions, /proc/mounts, /proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* and the udev metadata database). No block device is opened and nothing is written.</source>
        <translation>Releer el inventario del núcleo sólo leído (/proc/partitions, /proc/mounts, /proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* y la base de datos de metadatos de udev). No se abre ningún dispositivo de bloque y nada está escrito.</translation>
    </message>
    <message>
        <source>[OK]</source>
        <translation>[OK]</translation>
    </message>
    <message>
        <source>The running system was detected and stays protected from ordinary target repairs.</source>
        <translation>Se detectó el sistema de funcionamiento y se mantiene protegido de las reparaciones de objetivos comunes.</translation>
    </message>
    <message>
        <source>Detecting running system...</source>
        <translation>Detectando sistema en ejecución...</translation>
    </message>
    <message>
        <source>Detecting protected storage...</source>
        <translation>Detectando almacenamiento protegido...</translation>
    </message>
    <message>
        <source>PROTECTED</source>
        <translation>PROTEGIDO</translation>
    </message>
    <message>
        <source>The running host remains protected from ordinary repair-target operations.</source>
        <translation>El anfitrión en funcionamiento sigue protegido de las operaciones ordinarias de reparación-objetivo.</translation>
    </message>
    <message>
        <source>Details</source>
        <translation>Detalles</translation>
    </message>
    <message>
        <source>Show read-only details for the protected running host.</source>
        <translation>Mostrar sólo detalles de lectura para el anfitrión de funcionamiento protegido.</translation>
    </message>
    <message>
        <source>Host Maintenance</source>
        <translation>Mantenimiento del host</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Establecer como predeterminado</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host (menu.lst default directive with a backup and rollback). Requires Host Maintenance and the cached host-default probe.</source>
        <translation>Haga la entrada canónica del kernel instalado la entrada de arranque predeterminada GRUB-legacy en el host en ejecución (menu.lst directiva predeterminada con una copia de seguridad y devolución de rollos). Requiere Mantenimiento de Host y la sonda de host-default en caché.</translation>
    </message>
    <message>
        <source>Available repair targets</source>
        <translation>Destinos de reparación disponibles</translation>
    </message>
    <message>
        <source>Most likely first</source>
        <translation>El más probable primero</translation>
    </message>
    <message>
        <source>Drives are listed by the read-only inventory. Select a row to inspect it; Select Target commits the selected non-host drive with its auto-resolved Linux root component.</source>
        <translation>Las unidades están listadas por el inventario de sólo lectura. Seleccione una fila para inspeccionarla; Select Target compromete la unidad no host seleccionada con su componente raíz Linux autoresolvado.</translation>
    </message>
    <message>
        <source>Device</source>
        <translation>Dispositivo</translation>
    </message>
    <message>
        <source>Size</source>
        <translation>Tamaño</translation>
    </message>
    <message>
        <source>Type</source>
        <translation>Tipo</translation>
    </message>
    <message>
        <source>Filesystem</source>
        <translation>Filesystem</translation>
    </message>
    <message>
        <source>Select Target</source>
        <translation>Seleccionar destino</translation>
    </message>
    <message>
        <source>Unlock</source>
        <translation>Desbloquear</translation>
    </message>
    <message>
        <source>Authorize</source>
        <translation>Autorizar</translation>
    </message>
    <message>
        <source>Establish the privileged helper session for the current scope now instead of waiting for the next privileged action.</source>
        <translation>Establecer la sesión de ayuda privilegiada para el alcance actual ahora en lugar de esperar la próxima acción privilegiada.</translation>
    </message>
    <message>
        <source>Committed target: none</source>
        <translation>Destino confirmado: ninguno</translation>
    </message>
    <message>
        <source>Unlock state for the selected drive; the LUKS passphrase is never logged.</source>
        <translation>Desbloquear el estado para la unidad seleccionada; la contraseña LUKS nunca se ha registrado.</translation>
    </message>
    <message>
        <source>Unlock status</source>
        <translation>Estado de desbloqueo</translation>
    </message>
    <message>
        <source>Read-only inventory plus helper-confirmed facts; mirrors the modern Qt6 Selected drive details panel.</source>
        <translation>Inventario sólo lectura y hechos confirmados por el ayudante; refleja el moderno Qt6 Panel de detalles de la unidad seleccionada.</translation>
    </message>
    <message>
        <source>Selected drive details</source>
        <translation>Detalles del disco seleccionado</translation>
    </message>
    <message>
        <source>Field</source>
        <translation>Campo</translation>
    </message>
    <message>
        <source>Value</source>
        <translation>Valor</translation>
    </message>
    <message>
        <source>Run All runs every available read-only diagnostic for the current scope; selecting a check runs it alone. Diagnostics are read-only and are the only evidence source for the gated repair actions.</source>
        <translation>Ejecutar Todos corre todos los diagnósticos disponibles solo para el alcance actual; seleccionar un cheque lo ejecuta solo. Los diagnósticos son sólo leídos y son la única fuente de evidencia para las acciones de reparación cerradas.</translation>
    </message>
    <message>
        <source>Target: none selected</source>
        <translation>Meta: ninguno seleccionado</translation>
    </message>
    <message>
        <source>Diagnostics follow the committed repair target, or the protected running host while Host Maintenance is active.</source>
        <translation>Los diagnósticos siguen el objetivo de reparación comprometido, o el anfitrión de funcionamiento protegido mientras el mantenimiento de host está activo.</translation>
    </message>
    <message>
        <source>Run All</source>
        <translation>Ejecutar todo</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope.</source>
        <translation>Ejecutar todo - ejecutar todos los diagnósticos disponibles sólo lectura para el alcance actual.</translation>
    </message>
    <message>
        <source>Target configuration:</source>
        <translation>Configuración del destino:</translation>
    </message>
    <message>
        <source>Etch-era target configuration files; availability is probed read-only by the helper&apos;s diagnostics.</source>
        <translation>Archivos de configuración de destino Etch-era; la disponibilidad es probed sólo lectura por el diagnóstico del ayudante.</translation>
    </message>
    <message>
        <source>Edit Target File...</source>
        <translation>Editar archivo de destino...</translation>
    </message>
    <message>
        <source>Runs one read-only diagnostic for the selected scope through the helper (`diagnose &lt;key&gt;` / `host-diagnose &lt;key&gt;`); Run All is the combined report.</source>
        <translation>Ejecute un diagnóstico sólo de lectura para el alcance seleccionado a través del ayudante ( &amp;quot; diagnosticar &amp;quot; llavero &amp;quot; / &amp;quot;host-diagnose &amp;quot; ); Run All es el informe combinado.</translation>
    </message>
    <message>
        <source>Diagnostic checks</source>
        <translation>Verificaciones de diagnóstico</translation>
    </message>
    <message>
        <source>Check</source>
        <translation>Check</translation>
    </message>
    <message>
        <source>Selected diagnostic</source>
        <translation>Diagnóstico seleccionado</translation>
    </message>
    <message>
        <source>Select a diagnostic</source>
        <translation>Seleccione un diagnóstico</translation>
    </message>
    <message>
        <source>Choose a diagnostic from the list.</source>
        <translation>Elija un diagnóstico de la lista.</translation>
    </message>
    <message>
        <source>Ready</source>
        <translation>Listo</translation>
    </message>
    <message>
        <source>Results</source>
        <translation>Resultados</translation>
    </message>
    <message>
        <source>Run Diagnostic</source>
        <translation>Ejecutar diagnóstico</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic for the current scope.</source>
        <translation>Ejecutar Diagnóstico - ejecutar el diagnóstico de sólo lectura seleccionado para el alcance actual.</translation>
    </message>
    <message>
        <source>Copy Results</source>
        <translation>Copiar resultados</translation>
    </message>
    <message>
        <source>Save Results...</source>
        <translation>Guardar resultados...</translation>
    </message>
    <message>
        <source>The Full Repair plan runs the selected legacy stages in order through the guarded helper; the individual tools run one stage at a time. Every action stays disabled until the cached capability lines say available and the helper keeps its runtime preflights.</source>
        <translation>El plan de reparación completa ejecuta las etapas heredadas seleccionadas para el ayudante vigilado; las herramientas individuales ejecutan una etapa a la vez. Cada acción se mantiene desactivada hasta que las líneas de capacidad de caché digan disponibles y el ayudante mantiene sus preflights de tiempo de ejecución.</translation>
    </message>
    <message>
        <source>Full Repair plan</source>
        <translation>Plan de reparación completa</translation>
    </message>
    <message>
        <source>No stages selected</source>
        <translation>Ninguna etapa seleccionada</translation>
    </message>
    <message>
        <source>Configure Plan...</source>
        <translation>Configure Plan...</translation>
    </message>
    <message>
        <source>Open Settings to choose which Full Repair stages are part of the plan.</source>
        <translation>Abrir configuración para elegir qué etapas de reparación completa son parte del plan.</translation>
    </message>
    <message>
        <source>Run Full Repair</source>
        <translation>Ejecutar la reparación completa</translation>
    </message>
    <message>
        <source>Select a repair drive, or choose Host Maintenance on the protected running-host card.</source>
        <translation>Seleccione una unidad de reparación, o elija Host Maintenance en la tarjeta de reserva protegida.</translation>
    </message>
    <message>
        <source>Stage</source>
        <translation>Etapa</translation>
    </message>
    <message>
        <source>Individual repair tools</source>
        <translation>Herramientas de reparación individuales</translation>
    </message>
    <message>
        <source>Tool</source>
        <translation>Herramienta</translation>
    </message>
    <message>
        <source>Full Repair</source>
        <translation>Reparación completa</translation>
    </message>
    <message>
        <source>not reported</source>
        <translation>not reported</translation>
    </message>
    <message>
        <source>Selected tool</source>
        <translation>Herramienta seleccionada</translation>
    </message>
    <message>
        <source>Select a repair tool</source>
        <translation>Seleccionar una herramienta de reparación</translation>
    </message>
    <message>
        <source>Run Tool</source>
        <translation>Ejecutar herramienta</translation>
    </message>
    <message>
        <source>Select a tool to review its repair action.</source>
        <translation>Seleccione una herramienta para revisar su acción de reparación.</translation>
    </message>
    <message>
        <source>Write actions ask for confirmation and then run the helper&apos;s own runtime preflights; the GUI never weakens them. A repair that is not proven &apos;unchanged&apos; invalidates the cached diagnostics and disables the gated actions until diagnostics run again.</source>
        <translation>Escribe acciones piden confirmación y luego ejecutan los propios preflights de tiempo de ejecución del ayudante; el GUI nunca los debilita. Una reparación que no se pruebe &apos;sin cambios&apos; invalida el diagnóstico caché y desactiva las acciones cerradas hasta que el diagnóstico vuelva a funcionar.</translation>
    </message>
    <message>
        <source>Chroot shell</source>
        <translation>Shell chroot</translation>
    </message>
    <message>
        <source>Offline commands run one at a time in a fresh chroot and cannot answer interactive prompts (apt-get -y upgrade works). Host-shell commands run directly on the running host. The helper&apos;s probe lines gate the command field; the exact reason appears in the tooltip.</source>
        <translation>Los comandos Offline ejecutan uno a la vez en un chroot fresco y no pueden responder a los impulsos interactivos (apt-get -y obras de actualización). Los comandos Host-shell funcionan directamente en el host en ejecución. La sonda del ayudante cierra el campo de mando; la razón exacta aparece en el campo de herramientas.</translation>
    </message>
    <message>
        <source>No &apos;Legacy feature shell:&apos; line is cached; run diagnostics for the selected scope to evaluate the helper&apos;s chroot/timeout containment probes (fail closed).</source>
        <translation>No &apos;Característica de Legacy:&apos; línea es caché; ejecutar diagnóstico para el alcance seleccionado para evaluar las sondas de contención chroot/timeout del ayudante (fail cerrado).</translation>
    </message>
    <message>
        <source>Command</source>
        <translation>Comando</translation>
    </message>
    <message>
        <source>Command:</source>
        <translation>Comando:</translation>
    </message>
    <message>
        <source>One reviewed command string, passed to the helper as a single argument (no shell interpolation by the GUI).</source>
        <translation>Una cadena de comando revisada, pasó al ayudante como un solo argumento (sin interpolación de conchas por el GUI).</translation>
    </message>
    <message>
        <source>Run Command</source>
        <translation>Ejecutar comando</translation>
    </message>
    <message>
        <source>Clear Output</source>
        <translation>Borrar salida</translation>
    </message>
    <message>
        <source>The helper exposes `shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for an offline target chroot and `host-shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for the running host. Both keep the helper&apos;s runtime preflights; this tab enables the command only when the scope is committed, the session is authorized and the scope&apos;s Legacy feature probe reports available.</source>
        <translation>El ayudante expone &amp;quot; Shell &amp;quot; ilustría &amp;quot; para un chroot objetivo fuera de línea y &amp;quot; host-shell &amp;quot; нелиних &amp;quot; para el anfitrión en marcha. Ambos mantienen los preflights de tiempo de ejecución del ayudante; esta pestaña permite el comando sólo cuando el alcance está comprometido, la sesión está autorizada y los informes de probe de características Legacy del alcance están disponibles.</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Copia de archivos</translation>
    </message>
    <message>
        <source>Copy and verify files in either direction through the guarded helper (cp -a plus ownership restoration and a per-file byte-compare). The helper&apos;s file-copy probe gates the controls and keeps the direction and path containment checks.</source>
        <translation>Copiar y verificar archivos en cualquier dirección a través del ayudante vigilado (cp -a más restauración de la propiedad y un byte-compare per-file). La sonda de copia de archivos del ayudante cierra los controles y mantiene los controles de dirección y ruta de contención.</translation>
    </message>
    <message>
        <source>Preview Changes</source>
        <translation>Vista previa de cambios</translation>
    </message>
    <message>
        <source>Run a copy dry-run through the guarded helper. No files are changed.</source>
        <translation>Ejecute una copia en seco a través del ayudante vigilado. No se cambian los archivos.</translation>
    </message>
    <message>
        <source>Copy and Verify</source>
        <translation>Copiar y verificar</translation>
    </message>
    <message>
        <source>Copy staged items and verify the result. Existing destination names are overwritten when source content differs; unrelated destination files are never deleted.</source>
        <translation>Copiar los elementos escenificados y verificar el resultado. Los nombres de destino existentes se sobrescriben cuando el contenido de fuente difiere; los archivos de destino no relacionados nunca se eliminan.</translation>
    </message>
    <message>
        <source>Direction:</source>
        <translation>Dirección:</translation>
    </message>
    <message>
        <source>Choose which system supplies the source files and which system receives them.</source>
        <translation>Elija qué sistema suministra los archivos fuente y qué sistema los recibe.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from this host</source>
        <translation>1. Seleccione archivos fuente o carpetas de este host</translation>
    </message>
    <message>
        <source>Source</source>
        <translation>Fuente</translation>
    </message>
    <message>
        <source>Files and folders staged for the verified copy. The legacy backend copies with cp -a and restores ownership with chown --reference; every regular file is byte-compared after the copy.</source>
        <translation>Archivos y carpetas escenificados para la copia verificada. El backend heredado copia con cp -a y restablece la propiedad con corewn --reference; cada archivo regular es byte-compared después de la copia.</translation>
    </message>
    <message>
        <source>Add Files...</source>
        <translation>Añadir archivos...</translation>
    </message>
    <message>
        <source>Add Folder...</source>
        <translation>Añadir Carpeta...</translation>
    </message>
    <message>
        <source>Remove</source>
        <translation>Quitar</translation>
    </message>
    <message>
        <source>Clear</source>
        <translation>Borrar</translation>
    </message>
    <message>
        <source>Clear the staged source list (nothing is copied or deleted).</source>
        <translation>Borrar la lista de fuentes escalonadas (nada se copia o elimina).</translation>
    </message>
    <message>
        <source>2. Choose destination in repaired system</source>
        <translation>2. Elija el destino en el sistema reparado</translation>
    </message>
    <message>
        <source>unavailable: see the helper&apos;s Legacy feature file-copy: probe reason above</source>
        <translation>indisponible: vea la característica Legacy del ayudante copia de archivo: razón de sonda arriba</translation>
    </message>
    <message>
        <source>An absolute path inside the selected repair system (Host to Repair) or on the running host (Repair to Host).</source>
        <translation>Un camino absoluto dentro del sistema de reparación seleccionado (Host to Repair) o en el host en ejecución (Repair to Host).</translation>
    </message>
    <message>
        <source>Browse Target Folders...</source>
        <translation>Busca Carpetas de Destino...</translation>
    </message>
    <message>
        <source>Browse the selected repair system through the helper&apos;s temporary read-only mounts and choose an absolute destination path. No target files are changed while browsing.</source>
        <translation>Consulte el sistema de reparación seleccionado a través de los montajes de sólo lectura temporales del ayudante y elija una ruta de destino absoluta. No se cambian los archivos de destino mientras navega.</translation>
    </message>
    <message>
        <source>3. Ownership and copy policy</source>
        <translation>3. Política de propiedad y copia</translation>
    </message>
    <message>
        <source>Ownership:</source>
        <translation>Propiedad:</translation>
    </message>
    <message>
        <source>Smart destination ownership (recommended)</source>
        <translation>Propiedad de destino inteligente (recomendada)</translation>
    </message>
    <message>
        <source>Preserve source numeric UID/GID</source>
        <translation>Fuente preserve numeric UID/GID</translation>
    </message>
    <message>
        <source>Smart mode validates UID/GID identity mapping across the two systems and falls back to the destination-directory owner when the same numeric ID means a different account (the legacy backend implements it with chown --reference).</source>
        <translation>El modo inteligente valida el mapeo de identidad UID/GID a través de los dos sistemas y vuelve al propietario del directorio de destino cuando el mismo ID numérico significa una cuenta diferente (el backend heredado lo implementa con triwn --reference).</translation>
    </message>
    <message>
        <source>Application log</source>
        <translation>Registro de aplicación</translation>
    </message>
    <message>
        <source>The complete session register; Save As... writes every entry even while a filter hides lines. If a writable system share mount exists at /host, Save As... starts there; otherwise the log directory is the fallback. Prior session files are listed read-only.</source>
        <translation>El registro completo de sesión; Save As... escribe cada entrada incluso mientras un filtro esconde líneas. Si un sistema writable comparte montaje existe en /host, Save As... comienza allí; de lo contrario el directorio log es el retroceso. Los archivos de sesión previa se enumeran solos.</translation>
    </message>
    <message>
        <source>Session logs</source>
        <translation>Registros de sesión</translation>
    </message>
    <message>
        <source>Session</source>
        <translation>Período de sesiones</translation>
    </message>
    <message>
        <source>The first entry is the live session; earlier files in the log directory are listed read-only below it.</source>
        <translation>La primera entrada es la sesión en vivo; los archivos anteriores en el directorio de registro se enumeran sólo a continuación.</translation>
    </message>
    <message>
        <source>New Session Log</source>
        <translation>Nuevo período de sesiones</translation>
    </message>
    <message>
        <source>Close the active session file; it becomes a prior session and the next log entry starts a new file.</source>
        <translation>Cerrar el archivo de sesión activo; se convierte en una sesión previa y la próxima entrada de registro comienza un nuevo archivo.</translation>
    </message>
    <message>
        <source>Add Note</source>
        <translation>Nota</translation>
    </message>
    <message>
        <source>Append a NOTE entry to the live session register.</source>
        <translation>Apéndice una entrada NOTA al registro de sesión en vivo.</translation>
    </message>
    <message>
        <source>Delete</source>
        <translation>Eliminar</translation>
    </message>
    <message>
        <source>Delete the selected prior session file (the live session is never deleted).</source>
        <translation>Eliminar el archivo seleccionado de sesión anterior (la sesión en vivo nunca se elimina).</translation>
    </message>
    <message>
        <source>Refresh</source>
        <translation>Actualizar</translation>
    </message>
    <message>
        <source>Save As...</source>
        <translation>Salvar como...</translation>
    </message>
    <message>
        <source>Save the complete session log (all entries, not just the current filter).</source>
        <translation>Guardar el registro completo de sesión (todas las entradas, no sólo el filtro actual).</translation>
    </message>
    <message>
        <source>Clear Register</source>
        <translation>Borrar el registro</translation>
    </message>
    <message>
        <source>Clear the live register and view; prior session files are never modified.</source>
        <translation>Limpiar el registro en vivo y la vista; los archivos anteriores de sesión nunca se modifican.</translation>
    </message>
    <message>
        <source>Search log:</source>
        <translation>Registro de búsqueda:</translation>
    </message>
    <message>
        <source>Type any characters to show matching log entries (case-insensitive). Save As always writes every entry.</source>
        <translation>Escriba cualquier personaje para mostrar entradas de registro (insensibles en caso). Guardar Como siempre escribe cada entrada.</translation>
    </message>
    <message>
        <source>Filter:</source>
        <translation>Filtro:</translation>
    </message>
    <message>
        <source>Filter the visible log by entry kind. Selecting a diagnostic section shows the lines captured for that section; a workflow filter such as File system repair or Package repair shows its mapped repair lines (File copy has no lines in this frontend). Save As always writes every entry.</source>
        <translation>Filtrar el registro visible por tipo de entrada. Selección de una sección de diagnóstico muestra las líneas capturadas para esa sección; un filtro de flujo de trabajo como reparación del sistema de archivos o reparación del paquete muestra sus líneas de reparación mapeadas (La copia de archivo no tiene líneas en este frontend). Guardar Como siempre escribe cada entrada.</translation>
    </message>
    <message>
        <source>Settings are stored per user under ~/.qt/, one file per settings group (devicesrc, logsrc, diagnosticsrc, repairrc), and are saved immediately on every change and on close. Launch the GUI as the same user to keep your overrides; a GUI started as root keeps its own copies.</source>
        <translation>Los ajustes se almacenan por usuario bajo ~/.qt/, un archivo por grupo de configuración (devicesrc, logsrc, diagnosticsrc, repairrc), y se guardan inmediatamente en cada cambio y de cerca. Inicie el GUI como el mismo usuario para mantener sus anulaciones; un GUI comenzó como root mantiene sus propias copias.</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Detección de dispositivos</translation>
    </message>
    <message>
        <source>Show devices without an identified Linux installation</source>
        <translation>Mostrar dispositivos sin una instalación Linux identificada</translation>
    </message>
    <message>
        <source>Show removable and USB storage</source>
        <translation>Mostrar almacenamiento extraíble y USB</translation>
    </message>
    <message>
        <source>Show encrypted devices before unlocking</source>
        <translation>Mostrar dispositivos cifrados antes de desbloquear</translation>
    </message>
    <message>
        <source>When off, drives without a visible Linux filesystem are hidden unless they still contain an encrypted device and encrypted devices are shown.</source>
        <translation>Cuando está apagado, las unidades sin un sistema de archivos Linux visible están ocultas a menos que todavía contengan un dispositivo cifrado y se muestren dispositivos cifrados.</translation>
    </message>
    <message>
        <source>When off, removable and USB drives are hidden from the repair-target list.</source>
        <translation>Cuando está apagado, las unidades desmontables y USB están ocultas de la lista de dispositivos de reparación.</translation>
    </message>
    <message>
        <source>When off, drives with an encrypted device are hidden until the volume is unlocked.</source>
        <translation>Cuando se apaga, las unidades con un dispositivo cifrado se ocultan hasta que el volumen se desbloquea.</translation>
    </message>
    <message>
        <source>Include the %1 stage in the Full Repair plan. The stage runs in the plan order shown on the Repair tab.</source>
        <translation>Incluya la etapa %1 en el plan de reparación completa. El escenario se ejecuta en el orden del plan mostrado en la pestaña Reparación.</translation>
    </message>
    <message>
        <source>Automatically regenerate read-only diagnostics after repairs or target changes</source>
        <translation>Regenerar automáticamente diagnósticos sólo lectura después de reparaciones o cambios de destino</translation>
    </message>
    <message>
        <source>Regenerates the cached read-only diagnostics for the current scope after an operation that invalidates them (LUKS unlock, target configuration edit). It runs only inside an already authorized administrator session and never opens an authorization prompt by itself.</source>
        <translation>Regenera el diagnóstico de sólo lectura en caché para el alcance actual después de una operación que los invalida (LUKS desbloquear, edición de configuración de destino). Sólo funciona dentro de una sesión de administrador ya autorizada y nunca abre una solicitud de autorización por sí misma.</translation>
    </message>
    <message>
        <source>Wrap long log lines</source>
        <translation>Líneas de registro largas</translation>
    </message>
    <message>
        <source>Mandatory safety controls</source>
        <translation>Controles obligatorios de seguridad</translation>
    </message>
    <message>
        <source>Host capabilities and dependencies</source>
        <translation>Capacidades de acogida y dependencias</translation>
    </message>
    <message>
        <source>Refresh Capabilities</source>
        <translation>Capacidades de reserva</translation>
    </message>
    <message>
        <source>Re-run the read-only host capability probes (a PATH search, nothing is executed) and refresh the distribution summary.</source>
        <translation>Re-run the read-only host capacity probes (a PATH search, nada se ejecuta) y refrescar el resumen de distribución.</translation>
    </message>
    <message>
        <source>Install Missing Support...</source>
        <translation>Instala el soporte perdido...</translation>
    </message>
    <message>
        <source>Automatic installation will require explicit package mapping and privilege authorization.</source>
        <translation>La instalación automática requerirá una asignación explícita de paquetes y autorización de privilegios.</translation>
    </message>
    <message>
        <source>Application configuration</source>
        <translation>Configuración de aplicaciones</translation>
    </message>
    <message>
        <source>Boot Bitch Legacy</source>
        <translation>Boot Bitch Legacy</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list first.</source>
        <translation>Seleccione una unidad física en la lista de objetivos de reparación disponibles primero.</translation>
    </message>
    <message>
        <source>Protected system</source>
        <translation>Sistema protegido</translation>
    </message>
    <message>
        <source>The running system cannot be selected as a repair target. Use Host Maintenance for the protected running host or choose another disk.</source>
        <translation>El sistema de funcionamiento no puede ser seleccionado como objetivo de reparación. Utilice el mantenimiento de host para el anfitrión de funcionamiento protegido o elegir otro disco.</translation>
    </message>
    <message>
        <source>Unlock or select a Linux system first</source>
        <translation>Desbloquear o seleccionar un sistema Linux primero</translation>
    </message>
    <message>
        <source>This encrypted drive has no visible Linux filesystem yet. Use Unlock, refresh devices, and select the target after its Linux root is detected.</source>
        <translation>Esta unidad cifrada no tiene ningún sistema de archivos Linux visible todavía. Use Desbloquear, refrescar dispositivos y seleccione el objetivo después de que se detecte su raíz Linux.</translation>
    </message>
    <message>
        <source>The selected root component (%1) belongs to the running system and cannot be committed as a repair target. Use Host Maintenance for the protected running host.</source>
        <translation>El componente raíz seleccionado (%1) pertenece al sistema de funcionamiento y no puede ser cometido como objetivo de reparación. Utilice el mantenimiento de hosts para el huésped en funcionamiento protegido.</translation>
    </message>
    <message>
        <source>repair target commit</source>
        <translation>objetivo de reparación</translation>
    </message>
    <message>
        <source>Repair drive selected: %1</source>
        <translation>Unidad de reparación seleccionada: %1</translation>
    </message>
    <message>
        <source>; best detected system component: %1</source>
        <translation>; componente del sistema mejor detectado: %1</translation>
    </message>
    <message>
        <source>. No mount or repair action was performed.</source>
        <translation>. No se realizó ninguna acción de montaje o reparación.</translation>
    </message>
    <message>
        <source>Make Default unavailable</source>
        <translation>Establecer como predeterminado no disponible</translation>
    </message>
    <message>
        <source>Make Default is a running-host action on this frontend; enter Host Maintenance first.</source>
        <translation>Make Default es una acción de hospedaje en este frontend; ingrese primero Host Maintenance.</translation>
    </message>
    <message>
        <source>Administrator authorization required</source>
        <translation>Se requiere autorización de administrador</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize first.</source>
        <translation>No hay sesión de administrador activa; pulse Autorizar primero.</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host?

The helper verifies /boot/grub/menu.lst, sets the `default &lt;N&gt;` directive to the canonical entry, backs the menu up first and restores it on any failure.</source>
        <translation>¿Hacer la entrada canónica del núcleo instalado la entrada de arranque predeterminada GRUB-legacy en el host en ejecución?

El ayudante verifica /boot/grub/menu.lst, establece la directriz &apos;default&apos; a la entrada canónica, respalda el menú primero y lo restaura en cualquier fallo.</translation>
    </message>
    <message>
        <source>Host Maintenance unavailable</source>
        <translation>Mantenimiento del host no disponible</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a committed repair target or a detected running host.</source>
        <translation>No se pudo detectar el objetivo de host en funcionamiento; los diagnósticos necesitan un objetivo de reparación comprometido o un host en funcionamiento detectado.</translation>
    </message>
    <message>
        <source>Exit Host Maintenance</source>
        <translation>Salir del mantenimiento del host</translation>
    </message>
    <message>
        <source>Host Maintenance is active; diagnostics and gated repairs target the protected running host.</source>
        <translation>Host Maintenance es activo; los diagnósticos y las reparaciones cerradas apuntan al anfitrión de funcionamiento protegido.</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list on the Systems tab first, or use Host Maintenance for the protected running host.</source>
        <translation>Seleccione una unidad física en la lista de objetivos de reparación disponibles en la pestaña Sistemas primero, o utilice Host Maintenance para el anfitrión de funcionamiento protegido.</translation>
    </message>
    <message>
        <source>The selected drive (%1) is the protected running host. Choose Host Maintenance on the Systems tab to run read-only host diagnostics and guarded host repairs; ordinary target repairs stay disabled.</source>
        <translation>La unidad seleccionada (%1) es el anfitrión de funcionamiento protegido. Elija Mantenimiento de Host en la pestaña Sistemas para ejecutar diagnósticos de host solo lectura y reparaciones de host vigilados; las reparaciones de destino ordinario permanecen deshabilitadas.</translation>
    </message>
    <message>
        <source>No repair target is committed. Choose Select Target on the Systems tab (or Host Maintenance for the protected running host) first.</source>
        <translation>No hay objetivo de reparación. Elija Seleccionar Meta en la pestaña Sistemas (o Mantenimiento de Host para el anfitrión de funcionamiento protegido) primero.</translation>
    </message>
    <message>
        <source>The selection changed after the target was committed. Choose Select Target again.</source>
        <translation>La selección cambió después de que se comprometiera el objetivo. Elija Seleccionar Meta de nuevo.</translation>
    </message>
    <message>
        <source>Diagnostics scope required</source>
        <translation>Se requiere ámbito de diagnóstico</translation>
    </message>
    <message>
        <source>Diagnostics scope unresolved</source>
        <translation>Ámbito de diagnóstico sin resolver</translation>
    </message>
    <message>
        <source>The running host target could not be resolved; use Refresh Devices and commit a repair target or re-enter Host Maintenance.</source>
        <translation>El objetivo de host en funcionamiento no podría resolverse; utilizar Dispositivos Refresh y comprometer un objetivo de reparación o volver a entrar en mantenimiento de hosts.</translation>
    </message>
    <message>
        <source>Diagnostic check required</source>
        <translation>Se requiere verificación de diagnóstico</translation>
    </message>
    <message>
        <source>Select a diagnostic check in the list first.</source>
        <translation>Seleccione un cheque de diagnóstico en la lista primero.</translation>
    </message>
    <message>
        <source>Repair target required</source>
        <translation>Objetivo de reparación requerido</translation>
    </message>
    <message>
        <source>Target file editing needs a committed repair target. Running-host maintenance has no target-file editing; commit an offline target first.</source>
        <translation>La edición de archivos objetivo necesita un objetivo de reparación comprometido. El mantenimiento de Running-host no tiene edición de un fichero objetivo; comprometer un objetivo fuera de línea primero.</translation>
    </message>
    <message>
        <source>Configuration file required</source>
        <translation>Archivo de configuración requerido</translation>
    </message>
    <message>
        <source>Select a target configuration file first.</source>
        <translation>Seleccione un archivo de configuración de destino primero.</translation>
    </message>
    <message>
        <source>Edit target %1</source>
        <translation>Editar el objetivo %1</translation>
    </message>
    <message>
        <source>Edit this target file through the guarded administrator helper. A successful save invalidates cached diagnostics; rerun diagnostics before repair. Generated files such as /boot/grub/menu.lst may be replaced by the next bootloader update.</source>
        <translation>Edite este archivo de destino a través del ayudante del administrador vigilado. Un ahorro exitoso invalida los diagnósticos en caché; reincorporó el diagnóstico antes de la reparación. Los archivos generados como /boot/grub/menu.lst pueden ser reemplazados por la siguiente actualización del cargador de arranque.</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Cancelar</translation>
    </message>
    <message>
        <source>Save Target File</source>
        <translation>Guardar el archivo de destino</translation>
    </message>
    <message>
        <source>No changes to %1.</source>
        <translation>No hay cambios en %1.</translation>
    </message>
    <message>
        <source>Configuration write refused</source>
        <translation>Configuración escrita rechazada</translation>
    </message>
    <message>
        <source>The edited content contains NUL bytes; the guarded write refuses it.</source>
        <translation>El contenido editado contiene bytes NUL; el escrito guardado lo rechaza.</translation>
    </message>
    <message>
        <source>File too large</source>
        <translation>Archivo demasiado grande</translation>
    </message>
    <message>
        <source>The edited file is larger than 1 MiB. The guarded write refuses it; edit the file from a console instead.</source>
        <translation>El archivo editado es más grande que 1 MiB. La escritura guardada lo rechaza; editar el archivo de una consola en su lugar.</translation>
    </message>
    <message>
        <source>Write target configuration</source>
        <translation>Escriba configuración de destino</translation>
    </message>
    <message>
        <source>Write the edited contents to %1? This modifies the repair target and invalidates cached diagnostics.</source>
        <translation>Escribe el contenido editado a %1? Esto modifica el objetivo de reparación e invalida el diagnóstico de caché.</translation>
    </message>
    <message>
        <source>No diagnostic results to copy yet.</source>
        <translation>Todavía no hay resultados diagnósticos.</translation>
    </message>
    <message>
        <source>Diagnostic results copied to the clipboard.</source>
        <translation>Resultados diagnósticos copiados al portapapeles.</translation>
    </message>
    <message>
        <source>No diagnostic results to save yet.</source>
        <translation>Todavía no hay resultados diagnósticos.</translation>
    </message>
    <message>
        <source>Text files (*.txt);;All files (*)</source>
        <translation>Archivos de texto (*.txt);Todos los archivos (*)</translation>
    </message>
    <message>
        <source>Save diagnostic results</source>
        <translation>Guardar resultados diagnósticos</translation>
    </message>
    <message>
        <source>Could not write %1.</source>
        <translation>No podía escribir %1.</translation>
    </message>
    <message>
        <source>Diagnostic results saved to %1</source>
        <translation>Resultados de diagnóstico guardados en %1</translation>
    </message>
    <message>
        <source>Unlock not available</source>
        <translation>Desbloqueo no disponible</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked. Select an offline repair target to unlock.</source>
        <translation>No se puede desbloquear el host en funcionamiento protegido. Seleccione un objetivo de reparación fuera de línea para desbloquear.</translation>
    </message>
    <message>
        <source>No locked LUKS component is currently visible on this selected drive.</source>
        <translation>Ningún componente LUKS bloqueado es actualmente visible en esta unidad seleccionada.</translation>
    </message>
    <message>
        <source>Confirm LUKS unlock</source>
        <translation>Confirmar LUKS desbloqueo</translation>
    </message>
    <message>
        <source>Unlock %1 on %2?

The helper opens a temporary device-mapper mapping with cryptsetup and keeps it open for this recovery session. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>¿Desbloquear %1 en %2?

El ayudante abre un mapeo temporal con criptsetup y lo mantiene abierto para esta sesión de recuperación. La contraseña viaja a través de un archivo clave privado y nunca se coloca en argumentos de comando o registros.</translation>
    </message>
    <message>
        <source>LUKS unlock</source>
        <translation>Liberar LUKS</translation>
    </message>
    <message>
        <source>Unlock LUKS repair target</source>
        <translation>Desbloquear el objetivo de reparación LUKS</translation>
    </message>
    <message>
        <source>Enter the passphrase for %1.

It is sent only to cryptsetup over the helper&apos;s standard input and is never logged or placed on a command line.</source>
        <translation>Introduzca la contraseña para %1.

Se envía sólo a criptsetup sobre la entrada estándar del ayudante y nunca se registra o se coloca en una línea de comandos.</translation>
    </message>
    <message>
        <source>Passphrase required</source>
        <translation>Se requiere frase de contraseña</translation>
    </message>
    <message>
        <source>An empty passphrase was not submitted. Enter the LUKS passphrase or choose Cancel.</source>
        <translation>No se presentó una contraseña vacía. Introduzca la contraseña LUKS o elija Cancelar.</translation>
    </message>
    <message>
        <source>Unlock keyfile unavailable</source>
        <translation>Desbloquear el archivo clave no disponible</translation>
    </message>
    <message>
        <source>The LUKS passphrase could not be written to a private keyfile in %1; the unlock was not started.</source>
        <translation>La contraseña LUKS no podía ser escrita a un fichero clave privado en %1; el desbloqueo no se inició.</translation>
    </message>
    <message>
        <source>Configuration write unavailable</source>
        <translation>Configuración escriba no disponible</translation>
    </message>
    <message>
        <source>The edited content could not be written to a private temporary file in %1; the write was not started.</source>
        <translation>El contenido editado no pudo ser escrito a un archivo temporal privado en %1; el escrito no se inició.</translation>
    </message>
    <message>
        <source>ERROR: cannot read the session log %1</source>
        <translation>ERROR: no puede leer el registro de sesión %1</translation>
    </message>
    <message>
        <source>Viewing a prior session log (read-only): %1</source>
        <translation>Viewing a prior session log (read-only): %1</translation>
    </message>
    <message>
        <source>Session log list refreshed.</source>
        <translation>Lista de registro de sesión actualizada.</translation>
    </message>
    <message>
        <source>Note:</source>
        <translation>Nota:</translation>
    </message>
    <message>
        <source>Delete session log</source>
        <translation>Eliminar el registro del período de sesiones</translation>
    </message>
    <message>
        <source>Delete %1 permanently? This cannot be undone.</source>
        <translation>Eliminar %1 permanentemente? Esto no puede ser deshecho.</translation>
    </message>
    <message>
        <source>%1 changed while the confirmation was open; the delete was refused.</source>
        <translation>%1 cambió mientras la confirmación estaba abierta; el borrador fue rechazado.</translation>
    </message>
    <message>
        <source>Unable to delete %1.</source>
        <translation>Incapaz de eliminar %1.</translation>
    </message>
    <message>
        <source>Device discovery filters updated.</source>
        <translation>Filtros de descubrimiento de dispositivos actualizados.</translation>
    </message>
    <message>
        <source>Unknown Linux distribution</source>
        <translation>Distribución de Linux desconocida</translation>
    </message>
    <message>
        <source>sudo / gksu (no KAuth on this frontend)</source>
        <translation>sudo / gksu (no KAuth en este frontend)</translation>
    </message>
    <message>
        <source>Available</source>
        <translation>Disponible</translation>
    </message>
    <message>
        <source>Missing on this frontend</source>
        <translation>Desapareciendo en este frontend</translation>
    </message>
    <message>
        <source>Missing</source>
        <translation>Falta</translation>
    </message>
    <message>
        <source>No repair tool is selected.</source>
        <translation>Ninguna herramienta de reparación seleccionada.</translation>
    </message>
    <message>
        <source>A helper command is already running.</source>
        <translation>Ya se está ejecutando un comando de ayuda.</translation>
    </message>
    <message>
        <source>Restore Graphical Login is a host-scope stage on this legacy frontend; enter Host Maintenance to run it.</source>
        <translation>Restaurar Graphical Iniciar sesión es un estadio host-scope en este frontend heredado; introduzca Host Maintenance para ejecutarlo.</translation>
    </message>
    <message>
        <source>The selected scope has no resolved root component; use Refresh Devices and commit the repair target again.</source>
        <translation>El alcance seleccionado no tiene componente raíz resuelto; use Dispositivos Refresh y vuelva a comprometer el objetivo de reparación.</translation>
    </message>
    <message>
        <source>This legacy frontend exposes no %1 action; the helper reports the capability as available.</source>
        <translation>Este frontend legado no expone ninguna acción %1; el ayudante reporta la capacidad disponible.</translation>
    </message>
    <message>
        <source>Run this guarded repair action using the cached read-only diagnostic evidence. A confirmation is shown first.</source>
        <translation>Ejecute esta acción de reparación vigilada usando la prueba de diagnóstico sólo lectura caché. Una confirmación se muestra primero.</translation>
    </message>
    <message>
        <source>Enabled in Settings</source>
        <translation>Enabled in Settings</translation>
    </message>
    <message>
        <source>Disabled in Settings - enable it to include this stage</source>
        <translation>Discapacitados en Ajustes - permitir que incluya esta etapa</translation>
    </message>
    <message>
        <source>Unavailable: %1</source>
        <translation>No disponible: %1</translation>
    </message>
    <message>
        <source>
Unavailable: %1</source>
        <translation>
No disponible: %1</translation>
    </message>
    <message>
        <source>Repair tool unavailable</source>
        <translation>Herramienta de reparación no disponible</translation>
    </message>
    <message>
        <source>Confirm repair</source>
        <translation>Confirma la reparación</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Your saved selection is kept and its availability is re-checked when diagnostics for this scope complete.</source>
        <translation>Todavía no hay pruebas de capacidad caché para esta etapa. Su selección guardada se mantiene y su disponibilidad es revisada cuando el diagnóstico de este alcance se completa.</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Run diagnostics for the selected scope to populate the Full Repair plan; your selection is saved once the stage becomes available.</source>
        <translation>Todavía no hay pruebas de capacidad caché para esta etapa. Ejecute el diagnóstico para el alcance seleccionado para poblar el plan de reparación completa; su selección se guarda una vez que el escenario esté disponible.</translation>
    </message>
    <message>
        <source>Unknown Full Repair stage.</source>
        <translation>Etapa de reparación completa desconocida.</translation>
    </message>
    <message>
        <source>No Full Repair stages are selected or available; use Configure Plan... to choose the stages.</source>
        <translation>No se seleccionan o están disponibles etapas completas de reparación; utilice el Plan Configure... para elegir las etapas.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab to establish the session.</source>
        <translation>Se requiere autorización del administrador; pulse Autorizar en la pestaña Sistemas o Reparación para establecer la sesión.</translation>
    </message>
    <message>
        <source>Runs the selected stages using the cached read-only diagnostic evidence after privilege confirmation.</source>
        <translation>Ejecute las etapas seleccionadas usando la prueba de diagnóstico sólo lectura caché después de la confirmación del privilegio.</translation>
    </message>
    <message>
        <source>No Full Repair stages selected - use Configure Plan... or Settings.</source>
        <translation>No se seleccionan etapas completas de reparación - use Configurar Plan... o Ajustes.</translation>
    </message>
    <message>
        <source>1 stage selected</source>
        <translation>1 etapa seleccionada</translation>
    </message>
    <message>
        <source>%1 stages selected</source>
        <translation>etapas %1 seleccionadas</translation>
    </message>
    <message>
        <source>No repair stages are selected. Use Configure Plan... to choose the stages Full Repair will run.</source>
        <translation>No se seleccionan etapas de reparación. Use Configure Plan... para elegir las etapas Full Repair funcionará.</translation>
    </message>
    <message>
        <source>No selected stages are available. %1</source>
        <translation>No hay etapas seleccionadas disponibles. %1</translation>
    </message>
    <message>
        <source>Full Repair unavailable</source>
        <translation>Reparación completa no disponible</translation>
    </message>
    <message>
        <source>Run the Full Repair plan?

The selected stages run in order through the helper&apos;s guarded repair command:

</source>
        <translation>¿Ejecutar el plan de reparación completa?

Las etapas seleccionadas funcionan en orden a través del comando de reparación vigilado del ayudante:

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
El ayudante mantiene cada preluz de tiempo de ejecución; una etapa que falla para el plan.</translation>
    </message>
    <message>
        <source>Running %1 through the privileged helper... The Logs tab keeps the complete transcript.</source>
        <translation>Corriendo %1 a través del ayudante privilegiado... La ficha Logs mantiene la transcripción completa.</translation>
    </message>
    <message>
        <source>Shell scope required</source>
        <translation>Alcance Shell requerido</translation>
    </message>
    <message>
        <source>No administrator session is active.

Press Authorize on the Systems or Repair tab to establish the session, or enter Host Maintenance / commit a repair target on the Systems tab; the chroot shell then reuses the cached authorization.</source>
        <translation>No hay sesión de administrador activa.

Presione Autorizar en la pestaña Sistemas o Reparación para establecer la sesión, o ingrese a Host Maintenance / comprometer un objetivo de reparación en la pestaña Sistemas; el shell chroot luego reutiliza la autorización de caché.</translation>
    </message>
    <message>
        <source>Shell command required</source>
        <translation>comando Shell requerido</translation>
    </message>
    <message>
        <source>Enter the command to run first.</source>
        <translation>Introduzca el comando para ejecutar primero.</translation>
    </message>
    <message>
        <source>Confirm running-host command</source>
        <translation>Confirme el comando running-host</translation>
    </message>
    <message>
        <source>Run this command as root on the protected running host?

%1

The helper keeps its runtime preflights; the command is passed as one argument and is never interpreted by the GUI.</source>
        <translation>¿Ejecutar este comando como root en el host protegido?

%1

El ayudante mantiene sus preluz de tiempo de ejecución; el comando se transmite como un argumento y nunca es interpretado por el GUI.</translation>
    </message>
    <message>
        <source>Running %1...</source>
        <translation>Corriendo %1...</translation>
    </message>
    <message>
        <source>Authorization scope required</source>
        <translation>Alcance de autorización requerido</translation>
    </message>
    <message>
        <source>administrator authorization</source>
        <translation>Autorización del administrador</translation>
    </message>
    <message>
        <source>Administrator authorization</source>
        <translation>Autorización de administrador</translation>
    </message>
    <message>
        <source>Administrator authorization is required for %1.

Enter the password for %2 (sudo). It is used only for this sudo authentication, is sent over a pipe and is never logged or placed on a command line. The authorization is cached for this session and reused by diagnostics and repairs.</source>
        <translation>Se requiere autorización de administrador para %1.

Introduzca la contraseña para %2 (sudo). Se utiliza sólo para esta autenticación de sudo, se envía sobre una tubería y nunca se registra o se coloca en una línea de comandos. La autorización está preparada para esta sesión y reutilizada por diagnósticos y reparaciones.</translation>
    </message>
    <message>
        <source>your account</source>
        <translation>su cuenta</translation>
    </message>
    <message>
        <source>Password required</source>
        <translation>Contraseña requerida</translation>
    </message>
    <message>
        <source>An empty password was not submitted. Enter the sudo password or choose Cancel.</source>
        <translation>No se presentó una contraseña vacía. Introduzca la contraseña de sudo o seleccione Cancelar.</translation>
    </message>
    <message>
        <source>Administrator authorization failed</source>
        <translation>Falló la autorización de administrador</translation>
    </message>
    <message>
        <source>sudo did not accept the password: %1

The command was not started.</source>
        <translation>sudo no aceptó la contraseña: %1

El comando no comenzó.</translation>
    </message>
    <message>
        <source>elevation requires an interactive sudo password; run the smoke as root or after `sudo -S -v` with --elevate &apos;sudo -n&apos;</source>
        <translation>elevación requiere una contraseña interactiva de sudo; ejecutar el humo como raíz o después de `sudo -S -v` con --elevate &apos;sudo -n&apos;</translation>
    </message>
    <message>
        <source>No administrator session is active for %1.

Press Authorize on the Systems or Repair tab to establish the session now, or enter Host Maintenance / commit a repair target on the Systems tab; diagnostics and repairs then reuse the cached authorization.</source>
        <translation>No hay sesión de administrador activa para %1.

Presione Autorizar en la pestaña Sistemas o Reparación para establecer la sesión ahora, o introduzca Host Maintenance / comprometer un objetivo de reparación en la pestaña Sistemas; diagnóstico y reparaciones luego reutilizar la autorización de caché.</translation>
    </message>
    <message>
        <source>Administrator authorization expired</source>
        <translation>La autorización de administrador expiró</translation>
    </message>
    <message>
        <source>The cached administrator authorization for %1 expired or was refused.

Press Authorize on the Systems or Repair tab to re-establish the session, then run the command again. No command was started.</source>
        <translation>La autorización de administrador caché para %1 caducó o fue rechazada.

Pulse Autorizar en la pestaña Sistemas o Reparación para restablecer la sesión, luego ejecutar el comando de nuevo. No se inició el comando.</translation>
    </message>
    <message>
        <source>Privileged operation completed successfully. Administrator authorization remains active for this session.</source>
        <translation>Funcionamiento privilegiado completado con éxito. La autorización del administrador sigue siendo activa para este período de sesiones.</translation>
    </message>
    <message>
        <source>Privileged operation stopped with an error. Administrator authorization remains active; review the output before closing.</source>
        <translation>La operación privilegiada se detuvo con un error. La autorización del administrador sigue siendo activa; revisar el producto antes de cerrar.</translation>
    </message>
    <message>
        <source>Inspection is read-only.

</source>
        <translation>La inspección es sólo lectura.

</translation>
    </message>
    <message>
        <source>Configuration unavailable</source>
        <translation>Configuración no disponible</translation>
    </message>
    <message>
        <source>The helper could not read %1:

%2</source>
        <translation>El ayudante no podía leer %1:

%2</translation>
    </message>
    <message>
        <source>Configuration write failed</source>
        <translation>Error de configuración</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
Result: mapping opened for this recovery session.</source>
        <translation>Estado: abierto
Componente: %1
Mapper: %2
Método: desbloqueo del ayudante (cryptsetup; passphrase a través de un fichero clave mode-600, eliminado después del uso)
Resultado: se abrió un mapa para esta sesión de recuperación.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: the passphrase was not accepted; retry offered.</source>
        <translation>Estado: cerrado
Componente: %1
Método: desbloqueo del ayudante (cifrado)
Error: la contraseña no fue aceptada; reingreso ofrecido.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: %2</source>
        <translation>Estado: cerrado
Componente: %1
Método: desbloqueo del ayudante (cifrado)
Error: %2</translation>
    </message>
    <message>
        <source>Passphrase not accepted</source>
        <translation>Frase de contraseña no aceptada</translation>
    </message>
    <message>
        <source>The LUKS passphrase was not accepted.

Try again?</source>
        <translation>La contraseña LUKS no fue aceptada.

¿Intentar de nuevo?</translation>
    </message>
    <message>
        <source>[-] %1 - not run (the plan stopped before reaching this stage)</source>
        <translation>[-] %1 - no se ejecuta (el plan se detuvo antes de llegar a esta etapa)</translation>
    </message>
    <message>
        <source>filesystem</source>
        <translation>filesystem</translation>
    </message>
    <message>
        <source>[-] %1 - no file system errors found - no changes</source>
        <translation>[-] %1 - no se encontraron errores del sistema de archivos - no hay cambios</translation>
    </message>
    <message>
        <source>[FAIL] %1 - not reported</source>
        <translation>[FAIL] %1 - not reported</translation>
    </message>
    <message>
        <source>%1 stage(s) failed; review the helper output in Logs.</source>
        <translation>%1 etapa(s) falló; revise la salida del ayudante en Logs.</translation>
    </message>
    <message>
        <source>the plan stopped before any stage completed.</source>
        <translation>el plan se detuvo antes de completar cualquier etapa.</translation>
    </message>
    <message>
        <source>no repair was needed; cached diagnostics remain valid.</source>
        <translation>no se necesitaba reparación; los diagnósticos en caché siguen siendo válidos.</translation>
    </message>
    <message>
        <source>repair completed; cached diagnostics were invalidated and must be regenerated.</source>
        <translation>reparación completada; diagnóstico en caché fue invalidado y debe ser regenerado.</translation>
    </message>
    <message>
        <source>%1 results: [OK] %2 successful | [FAIL] %3 failed | [-] %4 no repair needed - %5</source>
        <translation>Resultado de %1: [OK] %2 exitoso Silencio [FAIL] %3 falló Silencio [-] %4 no necesita reparación - %5</translation>
    </message>
    <message>
        <source>Re-run Diagnostic</source>
        <translation>Volver a ejecutar el diagnóstico</translation>
    </message>
    <message>
        <source>Unavailable</source>
        <translation>No disponible</translation>
    </message>
    <message>
        <source>Commit this physical drive as the repair target.</source>
        <translation>Compromete esta unidad física como el objetivo de reparación.</translation>
    </message>
    <message>
        <source>Protection:</source>
        <translation>Protección:</translation>
    </message>
    <message>
        <source>Host shell</source>
        <translation>Shell del host</translation>
    </message>
    <message>
        <source>Run on Host</source>
        <translation>Ejecutar en host</translation>
    </message>
    <message>
        <source>Host Shell</source>
        <translation>Shell del host</translation>
    </message>
    <message>
        <source>The read-only probe found no editable target configuration file in this target. %1</source>
        <translation>La sonda de sólo lectura no encontró ningún archivo de configuración de destino editable en este objetivo. %1</translation>
    </message>
    <message>
        <source>Not present in the selected target (omitted): %1</source>
        <translation>No está presente en el objetivo seleccionado (omitido): %1</translation>
    </message>
    <message>
        <source>Run diagnostics to probe which target configuration files exist; the helper&apos;s read-only probe decides the list.</source>
        <translation>Ejecute el diagnóstico para probar qué archivos de configuración de destino existen; la sonda de sólo lectura del ayudante decide la lista.</translation>
    </message>
    <message>
        <source>Probed read-only by the helper; a saved edit invalidates the cached diagnostics.</source>
        <translation>Probed sólo leída por el ayudante; una edición guardada invalida el diagnóstico en caché.</translation>
    </message>
    <message>
        <source>Host maintenance: %1</source>
        <translation>Mantenimiento del huésped: %1</translation>
    </message>
    <message>
        <source>unresolved</source>
        <translation>sin resolver</translation>
    </message>
    <message>
        <source>Target: %1 + %2</source>
        <translation>Meta: %1 + %2</translation>
    </message>
    <message>
        <source>Host command</source>
        <translation>El comando host</translation>
    </message>
    <message>
        <source>Execute a command on the running host as root.</source>
        <translation>Ejecute un comando en el host en ejecución como root.</translation>
    </message>
    <message>
        <source>Execute a command inside the selected repair system as root.</source>
        <translation>Ejecute un comando dentro del sistema de reparación seleccionado como root.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab (or re-enter Host Maintenance / re-commit the repair target) to authorize this session.</source>
        <translation>Se requiere autorización del administrador; pulse Autorizar en la pestaña Sistemas o Reparación (o reingresar Mantenimiento de Host / volver a introducir el objetivo de reparación) para autorizar esta sesión.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root on the running host through the helper&apos;s guarded host-shell verb.</source>
        <translation>Ejecute un comando revisado como raíz en el anfitrión corriendo a través del verbo de host-shell vigilado del ayudante.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root inside the target chroot through the helper&apos;s guarded shell verb.</source>
        <translation>Ejecute un comando revisado como raíz dentro del objetivo chroot a través del verbo de shell vigilado del ayudante.</translation>
    </message>
    <message>
        <source>Run a command on the running host as root (sudo is not needed). Commands are executed directly on the active system; output is kept in this window and in the application log.</source>
        <translation>Ejecute un comando en el host en ejecución como root (sudo no es necesario). Los comandos se ejecutan directamente en el sistema activo; la salida se mantiene en esta ventana y en el registro de aplicaciones.</translation>
    </message>
    <message>
        <source>Run a command inside the selected repair system as root (sudo is not needed). Commands are executed one at a time in a fresh chroot and cannot answer interactive prompts; use non-interactive flags such as apt-get -y upgrade. Output is kept in this window and in the application log.</source>
        <translation>Ejecute un comando dentro del sistema de reparación seleccionado como root (sudo no es necesario). Los comandos se ejecutan uno a la vez en un chroot fresco y no pueden responder a los impulsos interactivos; utilizar banderas no interactivas como apt-get -y actualización. La salida se mantiene en esta ventana y en el registro de la aplicación.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize or re-enter Host Maintenance / commit a repair target.</source>
        <translation>No hay sesión de administrador activa; pulse Autorizar o reingresar Mantenimiento de Host / comprometer un objetivo de reparación.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from the repaired system</source>
        <translation>1. Seleccione archivos fuente o carpetas del sistema reparado</translation>
    </message>
    <message>
        <source>2. Choose destination on this host</source>
        <translation>2. Elija el destino en este host</translation>
    </message>
    <message>
        <source>Add File Path...</source>
        <translation>Añadir File Path...</translation>
    </message>
    <message>
        <source>Add Folder Path...</source>
        <translation>Añadir Carpeta de Carpeta...</translation>
    </message>
    <message>
        <source>Browse...</source>
        <translation>Examine...</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it (Host Maintenance does not provide a repair tree to browse).</source>
        <translation>Seleccione la unidad de reparación en Sistemas antes de elegir un destino dentro de él (Host Maintenance no proporciona un árbol de reparación para navegar).</translation>
    </message>
    <message>
        <source>Choose a host destination folder directly.</source>
        <translation>Elija una carpeta de destino host directamente.</translation>
    </message>
    <message>
        <source>Copy the staged files and folders with cp -a, restore ownership with chown --reference and byte-compare every regular file afterwards.</source>
        <translation>Copia los archivos y carpetas escenificados con cp -a, restaurar la propiedad con chown --reference y byte-compare cada archivo regular después.</translation>
    </message>
    <message>
        <source>File Copy unavailable</source>
        <translation>Copia de archivos no disponible</translation>
    </message>
    <message>
        <source>Choose host destination folder</source>
        <translation>Elija la carpeta de destino anfitrión</translation>
    </message>
    <message>
        <source>Select a repair target</source>
        <translation>Seleccionar un destino de reparación</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it.</source>
        <translation>Seleccione la unidad de reparación en Sistemas antes de elegir un destino dentro de él.</translation>
    </message>
    <message>
        <source>File Copy - Browse Target Folders</source>
        <translation>Copia de archivo - Hojee carpetas de destino</translation>
    </message>
    <message>
        <source>Browse Target Folders</source>
        <translation>Buscar carpetas de destino</translation>
    </message>
    <message>
        <source>The helper could not list the repair-system folder:

%1</source>
        <translation>El ayudante no pudo enumerar la carpeta del sistema de reparación:

%1</translation>
    </message>
    <message>
        <source>BROWSE_ENTRY</source>
        <translation>BROWSE ENTRY</translation>
    </message>
    <message>
        <source>(choose this folder: %1)</source>
        <translation>(escoge esta carpeta: %1)</translation>
    </message>
    <message>
        <source>(parent folder)</source>
        <translation>(Carpeta aparente)</translation>
    </message>
    <message>
        <source>Select repair-system destination</source>
        <translation>Seleccione el destino del sistema de reparación</translation>
    </message>
    <message>
        <source>Choose a destination folder inside the repaired system (current folder: %1):</source>
        <translation>Elija una carpeta de destino dentro del sistema reparado (carpeta actual: %1):</translation>
    </message>
    <message>
        <source>Folder</source>
        <translation>Folder</translation>
    </message>
    <message>
        <source>Open</source>
        <translation>Abierto</translation>
    </message>
    <message>
        <source>(choose this folder:</source>
        <translation>(escoge esta carpeta:</translation>
    </message>
    <message>
        <source>Add files to copy</source>
        <translation>Agregar archivos para copiar</translation>
    </message>
    <message>
        <source>Add folder to copy</source>
        <translation>Agregar carpeta para copiar</translation>
    </message>
    <message>
        <source>Stage at least one source and name a destination path first.</source>
        <translation>Estira al menos una fuente y nombre una ruta de destino primero.</translation>
    </message>
    <message>
        <source>Copy the %1 staged item(s) to %2?

The helper keeps its direction and path containment checks; a sensitive repair-system destination is refused unless the helper approves it, and every regular file is byte-compared after the copy.</source>
        <translation>Copiar el elemento(s) de %1 en estadio(s) a %2?

El ayudante mantiene su dirección y control de contención de ruta; se rechaza un destino sensible del sistema de reparación a menos que el ayudante lo apruebe, y todos los archivos regulares se completan después de la copia.</translation>
    </message>
    <message>
        <source>File Copy - Copy and Verify</source>
        <translation>Copia del archivo - Copia y Verificación</translation>
    </message>
    <message>
        <source>File Copy - Preview Changes</source>
        <translation>Copia de archivo - Cambios de vista previa</translation>
    </message>
    <message>
        <source>Protected running host - details</source>
        <translation>Protected running host - detalles</translation>
    </message>
    <message>
        <source>not mounted (offline target)</source>
        <translation>no montado (objetivo fijo)</translation>
    </message>
    <message>
        <source>Select a drive to see its details.</source>
        <translation>Seleccione una unidad para ver sus detalles.</translation>
    </message>
    <message>
        <source>Inspecting the selected component.</source>
        <translation>Inspección del componente seleccionado.</translation>
    </message>
    <message>
        <source>Helper-confirmed by the last read-only diagnostics.</source>
        <translation>Confirmado por el helper mediante el último diagnóstico de solo lectura.</translation>
    </message>
    <message>
        <source>Read-only inventory only; run diagnostics to confirm.</source>
        <translation>Solo inventario de solo lectura; ejecute el diagnóstico para confirmar.</translation>
    </message>
    <message>
        <source>PROTECTED - running system; read-only details only</source>
        <translation>PROTECTO - sistema de funcionamiento; sólo detalles de lectura</translation>
    </message>
    <message>
        <source>Live / installer media - not selectable</source>
        <translation>Live / instalador multimedia - no seleccionable</translation>
    </message>
    <message>
        <source>PROTECTED - running host scope</source>
        <translation>PROTECTED - alcance de host en funcionamiento</translation>
    </message>
    <message>
        <source>Unlock required before selection</source>
        <translation>Se requiere desbloquear antes de seleccionar</translation>
    </message>
    <message>
        <source>Eligible repair candidate</source>
        <translation>Eligible candidato de reparación</translation>
    </message>
    <message>
        <source>Drive:</source>
        <translation>Disco:</translation>
    </message>
    <message>
        <source>Detected target:</source>
        <translation>Destino detectado:</translation>
    </message>
    <message>
        <source>Pending inspection</source>
        <translation>Inspección pendiente</translation>
    </message>
    <message>
        <source>Model / label:</source>
        <translation>Modelo / etiqueta:</translation>
    </message>
    <message>
        <source>Status:</source>
        <translation>Estado:</translation>
    </message>
    <message>
        <source>Size:</source>
        <translation>Tamaño:</translation>
    </message>
    <message>
        <source>Connection:</source>
        <translation>Conexión:</translation>
    </message>
    <message>
        <source>Filesystem:</source>
        <translation>Sistema de archivos:</translation>
    </message>
    <message>
        <source>UUID:</source>
        <translation>UUID:</translation>
    </message>
    <message>
        <source>Mounts:</source>
        <translation>Puntos de montaje:</translation>
    </message>
    <message>
        <source>Running system protection unresolved</source>
        <translation>Protección del sistema de ejecución sin solución</translation>
    </message>
    <message>
        <source>No protected physical backing disk was identified</source>
        <translation>No se identificó ningún disco de respaldo físico protegido</translation>
    </message>
    <message>
        <source>Current running Linux system</source>
        <translation>Sistema Linux en ejecución actual</translation>
    </message>
    <message>
        <source>Critical mounts: %1</source>
        <translation>Montajes críticos: %1</translation>
    </message>
    <message>
        <source>Read-only protected-running-system facts: the helper OS fact plus the inventory model, device path, size, transport and critical mounts. Nothing here is probed destructively.</source>
        <translation>Datos del sistema de gestión sólo protegidos: el hecho del sistema de ayuda más el modelo de inventario, la ruta del dispositivo, el tamaño, el transporte y los montajes críticos. Nada aquí es destructivo.</translation>
    </message>
    <message>
        <source>Select a drive to see unlock status.</source>
        <translation>Seleccione una unidad para ver el estado de desbloqueo.</translation>
    </message>
    <message>
        <source>State: protected
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
The protected running host cannot be unlocked or modified; unlock is available for an offline repair target only. Use Host Maintenance for the protected running host.</source>
        <translation>Estado: protegido
Componente: %1
Mapper: (ninguno)
Método: desbloqueo del ayudante (cryptsetup; passphrase a través de un fichero clave mode-600, eliminado después del uso)
El host de funcionamiento protegido no puede ser desbloqueado o modificado; desbloqueo está disponible sólo para un objetivo de reparación fuera de línea. Utilice el mantenimiento de hosts para el huésped en funcionamiento protegido.</translation>
    </message>
    <message>
        <source>(none detected)</source>
        <translation>(ninguno detectado)</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
A locked LUKS container is visible on this drive; press Unlock to open it for this recovery session.</source>
        <translation>Estado: cerrado
Componente: %1
Mapper: (ninguno)
Método: desbloqueo del ayudante (cryptsetup; passphrase a través de un fichero clave mode-600, eliminado después del uso)
Un contenedor LUKS bloqueado es visible en esta unidad; pulse Desbloquear para abrirlo para esta sesión de recuperación.</translation>
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
        <translation>Estado: abierto
Componente: %1
Mapper: %2
Método: ya abierto antes de esta sesión (visible mapper; Boot Bitch lo reutilizará y no lo cerrará).</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper-confirmed mapping from the last read-only diagnostics.</source>
        <translation>Estado: abierto
Componente: %1
Mapper: %2
Método: cartografía confirmada por el ayudante del último diagnóstico sólo leído.</translation>
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
        <translation>Estado: bloqueado o no detectado ningún componente cifrado
Componente: (ninguno detectado)
Mapper: (ninguno)
Método: desbloqueo del ayudante (cryptsetup; passphrase a través de un fichero clave mode-600, eliminado después del uso)
No se registró ningún componente LUKS bloqueado y ninguna operación de desbloqueo para esta unidad en la sesión actual.</translation>
    </message>
    <message>
        <source>unavailable - %1</source>
        <translation>no disponible - %1</translation>
    </message>
    <message>
        <source>Host maintenance:
%1</source>
        <translation>Mantenimiento de hospedaje:
%1</translation>
    </message>
    <message>
        <source>Target:
%1 + %2</source>
        <translation>Meta:
%1 + %2</translation>
    </message>
    <message>
        <source>Authorization required: diagnostics and repairs fail closed until you press Authorize.</source>
        <translation>Autorización requerida: diagnósticos y reparaciones no cierran hasta que presiona Autorizar.</translation>
    </message>
    <message>
        <source>Re-establish the privileged helper session for the current scope now. The password is requested in the hidden-input modal and is never logged.</source>
        <translation>Reestablezca ahora la sesión de ayuda privilegiada para el alcance actual. La contraseña se solicita en el modal de entrada oculta y nunca se ha identificado.</translation>
    </message>
    <message>
        <source>Running...</source>
        <translation>Corriendo...</translation>
    </message>
    <message>
        <source>Scope required</source>
        <translation>Ámbito necesario</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope; this unlocks the gated actions.</source>
        <translation>Ejecutar Todo - ejecutar todos los diagnósticos disponibles sólo lectura para el alcance actual; esto desbloquea las acciones cerradas.</translation>
    </message>
    <message>
        <source>Select a target and wait for any running command first.</source>
        <translation>Seleccione un objetivo y espere a cualquier comando de ejecución primero.</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic through the helper.</source>
        <translation>Ejecutar Diagnóstico - ejecutar el diagnóstico de sólo lectura seleccionado a través del ayudante.</translation>
    </message>
    <message>
        <source>Run All for the current scope and refresh the read-only backend profile.</source>
        <translation>Ejecutar Todo para el alcance actual y refrescar el perfil de backend sólo lectura.</translation>
    </message>
    <message>
        <source>Read or edit the selected target configuration file through the guarded helper; a saved edit invalidates cached diagnostics.</source>
        <translation>Lea o edite el archivo seleccionado de configuración de destino a través del ayudante vigilado; una edición guardada invalida el diagnóstico en caché.</translation>
    </message>
    <message>
        <source>Host Maintenance has no target-file editing; commit an offline repair target first.</source>
        <translation>Host Maintenance no tiene edición de archivo objetivo; comprometer un objetivo de reparación fuera de línea primero.</translation>
    </message>
    <message>
        <source>Commit an offline repair target first.</source>
        <translation>Commitir un objetivo de reparación fuera de línea primero.</translation>
    </message>
    <message>
        <source>No target configuration file is available for this target; run diagnostics to probe the list.</source>
        <translation>No hay archivo de configuración de destino disponible para este objetivo; ejecutar diagnósticos para probar la lista.</translation>
    </message>
    <message>
        <source>Already Unlocked</source>
        <translation>Ya desbloqueado</translation>
    </message>
    <message>
        <source>An unlocked Linux filesystem is already visible on this drive. Boot Bitch will reuse the existing mapper and will not close or reopen a mapping created by this recovery session. Mounting happens during diagnostics (read-only) and repairs (read-write); data filesystems are never auto-mounted on selection.</source>
        <translation>Un sistema de archivos Linux desbloqueado ya es visible en esta unidad. Boot Bitch reutilizará el mapper existente y no cerrará ni reabrirá una asignación creada por esta sesión de recuperación. El montaje ocurre durante el diagnóstico (sólo lectura) y las reparaciones (read-write); los sistemas de archivos de datos nunca se montan automáticamente en la selección.</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked; use Host Maintenance for the protected running host.</source>
        <translation>El host en funcionamiento protegido no puede ser desbloqueado; use Host Maintenance para el host en funcionamiento protegido.</translation>
    </message>
    <message>
        <source>Host Maintenance is the current scope, but the selected offline drive can still be unlocked.</source>
        <translation>Host Maintenance es el alcance actual, pero la unidad offline seleccionada todavía se puede desbloquear.</translation>
    </message>
    <message>
        <source>Unlock %1 using cryptsetup through the privileged helper. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>Desbloquear %1 usando criptsetup a través del ayudante privilegiado. La contraseña viaja a través de un archivo clave privado y nunca se coloca en argumentos de comando o registros.</translation>
    </message>
    <message>
        <source>The protected running system cannot be selected as a repair target; use Host Maintenance for the protected running host.</source>
        <translation>El sistema de funcionamiento protegido no puede ser seleccionado como objetivo de reparación; utilice el mantenimiento de host para el anfitrión de funcionamiento protegido.</translation>
    </message>
    <message>
        <source>Live / installer media is read-only boot media and cannot be selected as a repair target.</source>
        <translation>Live / instalador medios es sólo lectura de los medios de arranque y no se puede seleccionar como un objetivo de reparación.</translation>
    </message>
    <message>
        <source>Unlock the encrypted volume first; Select Target becomes available after a Linux filesystem is detected.</source>
        <translation>Desbloquear el volumen cifrado primero; Select Target estará disponible después de que se detecte un sistema de archivos Linux.</translation>
    </message>
    <message>
        <source>Committed repair target. Repair, Diagnostics and File Copy target this physical drive until another drive is explicitly selected with Select Target.</source>
        <translation>Objetivo de reparación comprometido. Reparación, diagnósticos y copia de archivo apuntan esta unidad física hasta que otra unidad se selecciona explícitamente con Select Target.</translation>
    </message>
    <message>
        <source>Commit %1 as the repair target; this leaves Host Maintenance and switches the scope to the selected drive.</source>
        <translation>Compromete %1 como el objetivo de reparación; esto deja Host Maintenance y cambia el alcance a la unidad seleccionada.</translation>
    </message>
    <message>
        <source>The running host target could not be detected.</source>
        <translation>No se pudo detectar el objetivo de host en funcionamiento.</translation>
    </message>
    <message>
        <source>Leave Host Maintenance and return to repair-target mode.</source>
        <translation>Deje el mantenimiento de host y vuelva al modo de reparación-objetivo.</translation>
    </message>
    <message>
        <source>Select the running host for deliberate guarded maintenance; the administrator authorization is requested here once and cached for the session.</source>
        <translation>Seleccione el host en funcionamiento para mantenimiento vigilado deliberado; la autorización del administrador se solicita aquí una vez y se coloca para la sesión.</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a repair target.</source>
        <translation>No se pudo detectar el objetivo de host en funcionamiento; los diagnósticos necesitan un objetivo de reparación.</translation>
    </message>
    <message>
        <source>Committed target: %1 + %2</source>
        <translation>Objetivo comprometido: %1 + %2</translation>
    </message>
    <message>
        <source>Committed target: none (selection changed)</source>
        <translation>Objetivo comprometido: ninguno (cambió la elección)</translation>
    </message>
    <message>
        <source>The selected scope has no resolved Linux root component; Refresh Devices and commit the repair target again.</source>
        <translation>El alcance seleccionado no tiene componente raíz de Linux resuelto; Refresh Devices y comprometer el objetivo de reparación de nuevo.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize on the Systems or Repair tab to re-establish it. Run All reuses the cached authorization and never prompts by itself.</source>
        <translation>No hay sesión de administrador activa; pulse Autorizar en la pestaña Sistemas o Reparación para restablecerla. Run All reutiliza la autorización de caché y nunca se pide por sí mismo.</translation>
    </message>
    <message>
        <source>Run diagnostics for this scope to unlock the gated actions. Diagnostics are read-only and the only evidence source.</source>
        <translation>Ejecute el diagnóstico para que este alcance desbloquee las acciones cerradas. Los diagnósticos son sólo leídos y la única fuente de evidencia.</translation>
    </message>
    <message>
        <source>A repair was not proven unchanged, so the cached diagnostics are invalidated. Run diagnostics again before another gated action.</source>
        <translation>Una reparación no fue probada sin cambios, por lo que los diagnósticos de caché son invalidados. Ejecute el diagnóstico otra vez antes de otra acción cerrada.</translation>
    </message>
    <message>
        <source>Gated actions reflect the cached capability lines; the helper still runs every runtime preflight when a command starts.</source>
        <translation>Las acciones Gated reflejan las líneas de capacidad de caché; el ayudante sigue funcionando cada preluz de ejecución cuando un comando comienza.</translation>
    </message>
    <message>
        <source>Regenerating diagnostics automatically</source>
        <translation>Diagnóstico regenerador automáticamente</translation>
    </message>
    <message>
        <source>Running diagnostic: %1</source>
        <translation>Diagnóstico de ejecución: %1</translation>
    </message>
    <message>
        <source>Running all diagnostics</source>
        <translation>Correr todos los diagnósticos</translation>
    </message>
    <message>
        <source>Unlocking %1</source>
        <translation>Desbloquear %1</translation>
    </message>
    <message>
        <source>running host</source>
        <translation>anfitrión</translation>
    </message>
    <message>
        <source>offline target</source>
        <translation>Objetivo offline</translation>
    </message>
    <message>
        <source>host maintenance active</source>
        <translation>mantenimiento de host activo</translation>
    </message>
    <message>
        <source>repair target committed</source>
        <translation>objetivo de reparación comprometido</translation>
    </message>
    <message>
        <source>no committed scope</source>
        <translation>no compromiso</translation>
    </message>
    <message>
        <source>Current session</source>
        <translation>Período de sesiones actual</translation>
    </message>
    <message>
        <source>Log files (*.log);;All files (*)</source>
        <translation>Archivos de registro (*.log);;Todos los archivos (*)</translation>
    </message>
    <message>
        <source>Save log as</source>
        <translation>Guardar registro como</translation>
    </message>
    <message>
        <source>About Boot Bitch</source>
        <translation>Acerca de Boot Bitch</translation>
    </message>
    <message>
        <source>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;&lt;p&gt;&lt;b&gt;Developer:&lt;/b&gt; CaptainMorgan12&lt;/p&gt;&lt;p&gt;This GUI is the package&apos;s entry point: a Qt 3.3.x frontend with the ported guarded helper for Debian Etch-era systems.&lt;/p&gt;&lt;p&gt;&lt;b&gt;Guarded repair mode:&lt;/b&gt; read-only diagnostics can inspect either the protected Running Host or an explicitly selected repair drive. The guarded Debian/APT package stages (interrupted configuration, broken dependencies, metadata refresh, upgrade), GRUB-legacy configuration regeneration, LUKS target unlock and guarded target-file editing run through the ported helper after confirmation; Host Maintenance enables the same supported stages natively on the active system after repeating the host identity and boot-mount checks.&lt;/p&gt;&lt;p&gt;The modern-only features - verified File Copy, Btrfs snapshot rollback, EFI/UKI and extlinux repair, boot-stack reconciliation and Make Default - are greyed with the helper&apos;s own probe reasons on this frontend; Arch/Alpine/Fedora package backends stay diagnostics-only here.&lt;/p&gt;&lt;p&gt;The first privileged action authorizes one cached administrator session per scope through a hidden-input modal (the Qt GUI remains unprivileged). It can be ended at any time from File - Lock Administrator Session.&lt;/p&gt;</source>
        <translation type="unfinished"></translation>
    </message>
</context>
</TS>
