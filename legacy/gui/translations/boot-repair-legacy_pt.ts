<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS language="pt">
<context>
    <name>LegacyMainWindow</name>
    <message>
        <source>Validate environment</source>
        <translation>Validar ambiente</translation>
    </message>
    <message>
        <source>Validate</source>
        <translation>Validar</translation>
    </message>
    <message>
        <source>Check the selected system&apos;s mounts, filesystem metadata, boot files, mapper consistency and dependency readiness before any repair action. This is an independent safety preflight rather than an optional Full Repair stage.</source>
        <translation>Verifique as montagens do sistema selecionado, metadados do sistema de arquivos, arquivos de inicialização, consistência mapper e prontidão de dependência antes de qualquer ação de reparo. Este é um pré-voo de segurança independente em vez de um estágio opcional de reparo completo.</translation>
    </message>
    <message>
        <source>Always preflight</source>
        <translation>Sempre pré- voo</translation>
    </message>
    <message>
        <source>File system repair</source>
        <translation>Reparação do sistema de arquivos</translation>
    </message>
    <message>
        <source>Check File Systems</source>
        <translation>Verificar os Sistemas de Ficheiros</translation>
    </message>
    <message>
        <source>Run the read-only file system check for the selected system&apos;s root and /boot filesystems and report each device&apos;s check tool and result without changing anything. This legacy frontend exposes the read-only check only; device repair is not wired.</source>
        <translation>Execute a verificação do sistema de arquivos somente leitura para os sistemas de arquivos root e /boot selecionados e relate a ferramenta e o resultado de verificação de cada dispositivo sem alterar nada. Este frontend legado expõe apenas a verificação somente de leitura; o reparo do dispositivo não está ligado.</translation>
    </message>
    <message>
        <source>Full Repair plan: unavailable on this frontend - read-only check only</source>
        <translation>Plano de reparo completo: indisponível neste frontend - somente para leitura</translation>
    </message>
    <message>
        <source>Complete package configuration</source>
        <translation>Configuração completa do pacote</translation>
    </message>
    <message>
        <source>Complete Configuration</source>
        <translation>Configuração Completa</translation>
    </message>
    <message>
        <source>Complete interrupted dpkg package configuration in the selected repair system. This is the same stage controlled by Settings -&gt; Full Repair plan -&gt; Complete interrupted package configuration, but it can also be run independently here.</source>
        <translation>Configuração completa do pacote dpkg interrompido no sistema de reparo selecionado. Esta é a mesma fase controlada pelas Definições -&gt; Plano de reparo completo -&gt; Configuração completa do pacote interrompido, mas também pode ser executado independentemente aqui.</translation>
    </message>
    <message>
        <source>Run the guarded dpkg-configure repair? The helper keeps its package-lock and runtime preflights.</source>
        <translation>Fazer a reparação vigiada de dpkg-configure? O ajudante mantém os seus pré-voos em pacote e em tempo de execução.</translation>
    </message>
    <message>
        <source>Repair broken dependencies</source>
        <translation>Reparar dependências quebradas</translation>
    </message>
    <message>
        <source>Repair Dependencies</source>
        <translation>Dependências de reparo</translation>
    </message>
    <message>
        <source>Repair package dependencies in the selected repair system after the mandatory safety preflight. This maps directly to Settings -&gt; Repair broken package dependencies.</source>
        <translation>Reparar dependências do pacote no sistema de reparo selecionado após o pré-voo de segurança obrigatório. Isto mapeia diretamente para Configurações -&gt; Reparar dependências de pacotes quebradas.</translation>
    </message>
    <message>
        <source>Run the guarded fix-broken repair? The helper keeps its simulation-first preflight and runtime guards.</source>
        <translation>Fazer os reparos? O ajudante mantém os seus guardas de simulação e pré-voo.</translation>
    </message>
    <message>
        <source>Refresh package metadata</source>
        <translation>Actualizar os meta- dados do pacote</translation>
    </message>
    <message>
        <source>Refresh Metadata</source>
        <translation>Atualizar metadados</translation>
    </message>
    <message>
        <source>Refresh APT metadata in the selected repair system without upgrading installed packages. This maps directly to Settings -&gt; Refresh package metadata.</source>
        <translation>Atualizar os metadados APT no sistema de reparo selecionado sem atualizar os pacotes instalados. Isto mapeia diretamente para Configurações -&gt; Actualizar os meta- dados do pacote.</translation>
    </message>
    <message>
        <source>Refresh package metadata for the selected scope? The helper requires a reachable, trusted APT source.</source>
        <translation>Atualizar os metadados do pacote para o escopo selecionado? O ajudante requer uma fonte APT acessível e confiável.</translation>
    </message>
    <message>
        <source>Upgrade installed packages</source>
        <translation>Atualizar pacotes instalados</translation>
    </message>
    <message>
        <source>Simulate and Upgrade</source>
        <translation>Simular e atualizar</translation>
    </message>
    <message>
        <source>Simulate the APT transaction first, inspect proposed removals, then apply a safe upgrade. This maps directly to Settings -&gt; Upgrade installed packages.</source>
        <translation>Simule a transação APT primeiro, inspecione remoções propostas e, em seguida, aplique uma atualização segura. Isto mapeia diretamente para Configurações -&gt; Atualizar pacotes instalados.</translation>
    </message>
    <message>
        <source>Run the guarded apt-upgrade transaction? The helper keeps its simulation-first and source guards.</source>
        <translation>Fazer a transacção de apt-upgrade vigiada? O ajudante mantém a simulação primeiro e os guardas de origem.</translation>
    </message>
    <message>
        <source>DKMS</source>
        <translation>DKMS</translation>
    </message>
    <message>
        <source>Rebuild DKMS</source>
        <translation>Reconstruir o DKMS</translation>
    </message>
    <message>
        <source>Rebuild out-of-tree kernel modules for kernels installed in the selected system. The helper refuses this action when DKMS is not installed; this legacy frontend exposes no DKMS action.</source>
        <translation>Reconstruir módulos de kernel fora da árvore para kernels instalados no sistema selecionado. O helper recusa esta ação quando o DKMS não está instalado; este frontend legado não expõe nenhuma ação do DKMS.</translation>
    </message>
    <message>
        <source>Graphical login / display manager</source>
        <translation>Gerenciador gráfico de login / exibição</translation>
    </message>
    <message>
        <source>Restore Graphical Login</source>
        <translation>Restaurar login gráfico</translation>
    </message>
    <message>
        <source>Restore the legacy SysV display manager configured for the running host: the /etc/X11/default-display-manager entry and the missing runlevel S-symlink, with a backup and rollback, never starting the GUI. This is a host-scope stage on this legacy frontend.</source>
        <translation>Restaurar o gerenciador de tela legado SysV configurado para o host em execução: a entrada /etc/X11/default-display-manager e o runlevel S-symlink faltando, com backup e rollback, nunca iniciando a GUI. Este é um palco de host-scope neste frontend legado.</translation>
    </message>
    <message>
        <source>Restore the graphical login configuration for the running host? The helper backs up /etc/X11/default-display-manager and the runlevel symlink state, restores the configured entry and the missing S-symlink, rolls back on any failure, and never starts the display manager.</source>
        <translation>Restaurar a configuração gráfica de login da máquina em execução? O helper faz backup do /etc/X11/default-display-manager e do estado do runlevel symlink, restaura a entrada configurada e o S-symlink faltando, retorna em qualquer falha e nunca inicia o gerenciador de exibição.</translation>
    </message>
    <message>
        <source>Initramfs</source>
        <translation>Initramfs</translation>
    </message>
    <message>
        <source>Rebuild Initramfs</source>
        <translation>Reconstruir o Initramfs</translation>
    </message>
    <message>
        <source>Rebuild initramfs images for the selected repair system only after mapper and crypttab consistency checks pass. The helper backs up each image before the apply. On Etch the stage runs through the guarded plain-chroot fallback (no unshare required).</source>
        <translation>Reconstruir imagens initramfs para o sistema de reparo selecionado apenas após mapper e cryptab verificação de consistência passar. O ajudante faz backup de cada imagem antes da aplicação. No Etch o palco corre através do recuo de chroot simples guardado (sem necessidade de partilha).</translation>
    </message>
    <message>
        <source>Rebuild the initramfs for the selected scope? The helper keeps its mapper/crypttab and backup preflights.</source>
        <translation>Reconstruir o initramfs para o escopo selecionado? O ajudante mantém seu mapper/cryptab e pré-voos de backup.</translation>
    </message>
    <message>
        <source>EFI / UKI bootloader</source>
        <translation>Carregador de arranque EFI / UKI</translation>
    </message>
    <message>
        <source>Repair EFI / UKI</source>
        <translation>Reparar EFI / UKI</translation>
    </message>
    <message>
        <source>Repair the selected system&apos;s EFI / UKI boot path. This legacy frontend exposes no EFI action; the Etch target is a BIOS/GRUB-legacy system.</source>
        <translation>Reparar o caminho de inicialização EFI / UKI do sistema selecionado. Este frontend legado não expõe nenhuma ação EFI; o alvo Etch é um sistema BIOS/GRUB-legacy.</translation>
    </message>
    <message>
        <source>GRUB configuration</source>
        <translation>Configuração GRUB</translation>
    </message>
    <message>
        <source>Regenerate GRUB</source>
        <translation>Regenerar GRUB</translation>
    </message>
    <message>
        <source>Regenerate the selected repair system&apos;s GRUB menu/configuration after the mandatory safety preflight. The helper backs up menu.lst, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Regenere o menu/configuração GRUB do sistema de reparo selecionado após o pré-voo de segurança obrigatório. O helper faz backup do menu.lst, preserva cada entrada de inicialização existente e rebobina em qualquer falha.</translation>
    </message>
    <message>
        <source>Regenerate the GRUB configuration? The helper backs up the target menu/configuration, preserves every existing boot entry and rolls back on any failure.</source>
        <translation>Regenerar a configuração GRUB? O helper faz backup do menu/configuração de destino, preserva cada entrada de boot existente e retorna em qualquer falha.</translation>
    </message>
    <message>
        <source>extlinux configuration</source>
        <translation>Configuração extlinux</translation>
    </message>
    <message>
        <source>Regenerate extlinux</source>
        <translation>Regenerar extlinux</translation>
    </message>
    <message>
        <source>Regenerate the selected system&apos;s extlinux bootloader configuration. This legacy frontend exposes no extlinux action.</source>
        <translation>Regenere a configuração do carregador de inicialização extlinux do sistema selecionado. Este frontend legado não expõe nenhuma ação extlinux.</translation>
    </message>
    <message>
        <source>Boot stack reconciliation</source>
        <translation>Reconciliação da pilha de arranque</translation>
    </message>
    <message>
        <source>Reconcile Boot Stack</source>
        <translation>Reconcile a pilha de arranque</translation>
    </message>
    <message>
        <source>Reconcile the selected repair system&apos;s boot stack in one guarded legacy pass: mapper/crypttab validation, initramfs rebuild and GRUB-legacy configuration regeneration, with the component backups and preflights unchanged. This is the Etch equivalent of the modern boot-stack reconciliation and stays out of the Full Repair plan.</source>
        <translation>Reconcile a pilha de inicialização do sistema de reparo selecionado em um passe legado protegido: validação mapper/cryptab, reconstrução initramfs e regeneração de configuração GRUB-legacy, com os backups de componentes e pré-voos inalterados. Este é o equivalente Etch da reconciliação moderna boot-stack e fica fora do plano de reparação completa.</translation>
    </message>
    <message>
        <source>Full Repair plan: Manual recovery tool</source>
        <translation>Plano de reparo completo: Ferramenta de recuperação manual</translation>
    </message>
    <message>
        <source>Run the guarded boot-stack reconciliation? The helper runs the mapper/crypttab validation, the initramfs rebuild and the GRUB-legacy regeneration in one pass with every component preflight and backup.</source>
        <translation>Fazer a reconciliação guardada? O ajudante executa a validação mapper/cryptab, a reconstrução initramfs e a regeneração GRUB-legacy em uma passagem com cada componente pré-voo e backup.</translation>
    </message>
    <message>
        <source>Complete interrupted package configuration</source>
        <translation>Configuração completa do pacote interrompido</translation>
    </message>
    <message>
        <source>Repair broken package dependencies</source>
        <translation>Reparar as dependências do pacote quebradas</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Predefinição</translation>
    </message>
    <message>
        <source>Repair file system errors (read-only check first)</source>
        <translation>Reparar os erros do sistema de arquivos (apenas verificação de leitura primeiro)</translation>
    </message>
    <message>
        <source>the legacy frontend exposes the read-only file system check only; per-device repair is not wired on this frontend (fail closed)</source>
        <translation>o frontend legado expõe apenas a verificação do sistema de arquivos somente para leitura; o reparo por dispositivo não está conectado neste frontend (falha fechada)</translation>
    </message>
    <message>
        <source>Upgrade installed packages (adaptive APT simulation)</source>
        <translation>Atualizar pacotes instalados (simulação APT adaptativa)</translation>
    </message>
    <message>
        <source>Rebuild DKMS modules</source>
        <translation>Reconstruir módulos DKMS</translation>
    </message>
    <message>
        <source>Restore graphical login manager</source>
        <translation>Restaurar gerenciador gráfico de login</translation>
    </message>
    <message>
        <source>Rebuild initramfs after mapper/crypttab validation</source>
        <translation>Reconstruir initramfs após validação mapper/cryptab</translation>
    </message>
    <message>
        <source>Repair EFI / UKI boot path</source>
        <translation>Reparar o caminho de arranque EFI / UKI</translation>
    </message>
    <message>
        <source>Update GRUB configuration</source>
        <translation>Actualizar a configuração GRUB</translation>
    </message>
    <message>
        <source>Update extlinux configuration</source>
        <translation>Actualizar a configuração extlinux</translation>
    </message>
    <message>
        <source>Choose Full Repair stages in Settings. Enabled stages run in the order shown. Each configurable stage also appears below as an individual tool; the Full Repair column mirrors its current Settings state. The boot tools (EFI / UKI bootloader, GRUB or extlinux configuration, boot-stack reconciliation and Make Default) are independent: run them in any order, and a later action re-verifies what an earlier one changed and reports its own result. The active scope is shown beside Repair: selected repair drive or Running Host maintenance.</source>
        <translation>Escolha etapas de reparação completa em Configurações. Os estágios habilitados são executados na ordem mostrada. Cada estágio configurável também aparece abaixo como uma ferramenta individual; a coluna de reparo completo espelha seu estado atual Configurações. As ferramentas de inicialização (EFI / UKI bootloader, configuração GRUB ou extlinux, reconciliação boot-stack e Make Default) são independentes: execute-as em qualquer ordem, e uma ação posterior re-verifica o que um anterior mudou e relata seu próprio resultado. O escopo ativo é mostrado ao lado do Repair: unidade de reparo selecionada ou manutenção do Runing Host.</translation>
    </message>
    <message>
        <source>Run All diagnostics for the selected target or running host before starting Full Repair. The report is read-only evidence used to choose and confirm repair stages.</source>
        <translation>Execute todos os diagnósticos para o alvo selecionado ou o host em execução antes de iniciar o reparo completo. O relatório é uma evidência somente de leitura usada para escolher e confirmar as etapas de reparo.</translation>
    </message>
    <message>
        <source>Ready: required cached read-only diagnostics are available for the selected stages. Review them in Diagnostics or Logs before confirming.</source>
        <translation>Pronto: os diagnósticos necessários em cache somente leitura estão disponíveis para as etapas selecionadas. Revise-os em diagnósticos ou logs antes de confirmar.</translation>
    </message>
    <message>
        <source>Environment validation</source>
        <translation>Validação do ambiente</translation>
    </message>
    <message>
        <source>Summarizes the selected system, protection state, mounted identity and inspection readiness.</source>
        <translation>Resuma o sistema selecionado, estado de proteção, identidade montada e prontidão para inspeção.</translation>
    </message>
    <message>
        <source>Distribution and boot backend profile</source>
        <translation>Distribuição e perfil de infraestrutura de inicialização</translation>
    </message>
    <message>
        <source>Identifies the distribution family, package manager, initramfs generator, bootloader and current guarded repair capability.</source>
        <translation>Identifica a família de distribuição, gerenciador de pacotes, gerador initramfs, carregador de boot e capacidade de reparo guardado atual.</translation>
    </message>
    <message>
        <source>Boot diagnostics</source>
        <translation>Diagnóstico de arranque</translation>
    </message>
    <message>
        <source>Shows boot mounts and /boot contents plus storage evidence without changing the selected system.</source>
        <translation>Mostra as montagens de arranque e o conteúdo do /boot mais a evidência de armazenamento sem alterar o sistema seleccionado.</translation>
    </message>
    <message>
        <source>Boot evidence and selection history</source>
        <translation>Evidência inicial e histórico de seleção</translation>
    </message>
    <message>
        <source>Correlates the detected boot chain, bootloader selection, kernel/initramfs and unlock evidence.</source>
        <translation>Correla a cadeia de inicialização detectada, seleção do bootloader, kernel/initramfs e desbloquear evidência.</translation>
    </message>
    <message>
        <source>Kernel / initramfs</source>
        <translation>Kernel / initramfs</translation>
    </message>
    <message>
        <source>Reviews kernel files and verifies matching initramfs images through a read-only inspection.</source>
        <translation>Revê arquivos do kernel e verifica a correspondência de imagens initramfs através de uma inspeção somente de leitura.</translation>
    </message>
    <message>
        <source>Reviews the GRUB configuration without changing boot files.</source>
        <translation>Revê a configuração GRUB sem alterar arquivos de inicialização.</translation>
    </message>
    <message>
        <source>EFI / UKI boot state</source>
        <translation>EFI / UKI estado de arranque</translation>
    </message>
    <message>
        <source>Inspects EFI/UKI evidence; unavailable on this legacy BIOS frontend with the helper&apos;s probe reason.</source>
        <translation>Inspeciona a evidência EFI/UKI; indisponível neste frontend legado da BIOS com a razão da sonda do ajudante.</translation>
    </message>
    <message>
        <source>Reviews the configured display manager and recent boot evidence without starting the GUI.</source>
        <translation>Revê o gerenciador de exibição configurado e evidência de inicialização recente sem iniciar a GUI.</translation>
    </message>
    <message>
        <source>Boot errors</source>
        <translation>Erros de arranque</translation>
    </message>
    <message>
        <source>Reads recent error-priority entries from the running host or selected repair system when available.</source>
        <translation>Lê entradas de prioridade de erro recentes do host em execução ou sistema de reparo selecionado quando disponível.</translation>
    </message>
    <message>
        <source>Disk usage</source>
        <translation>Utilização do disco</translation>
    </message>
    <message>
        <source>Summarizes filesystem capacity and free space for the running host or read-only repair target.</source>
        <translation>Soma a capacidade do sistema de arquivos e espaço livre para o host em execução ou alvo de reparo somente leitura.</translation>
    </message>
    <message>
        <source>File systems</source>
        <translation>Sistemas de ficheiros</translation>
    </message>
    <message>
        <source>Runs the read-only file system check for the selected system&apos;s root, /boot and other filesystems.</source>
        <translation>Executa a verificação do sistema de arquivos somente leitura para o root do sistema selecionado, /boot e outros sistemas de arquivos.</translation>
    </message>
    <message>
        <source>/etc/fstab review</source>
        <translation>/etc/fstab review</translation>
    </message>
    <message>
        <source>Displays the running host or selected repair system&apos;s fstab; repair-system inspection is mounted read-only.</source>
        <translation>Mostra o servidor em execução ou o fstab do sistema de reparo selecionado; a inspeção do sistema de reparo é montada somente para leitura.</translation>
    </message>
    <message>
        <source>Btrfs status</source>
        <translation>Btrfs status</translation>
    </message>
    <message>
        <source>Shows Btrfs filesystem and subvolume information when the target uses Btrfs.</source>
        <translation>Mostra o sistema de ficheiros Btrfs e as informações do subvolume quando o alvo usa o Btrfs.</translation>
    </message>
    <message>
        <source>Device-mapper ancestry</source>
        <translation>Antecedentes de dispositivos</translation>
    </message>
    <message>
        <source>Shows selected mapper ancestry and device-mapper state when available.</source>
        <translation>Mostra o estado de ancestralidade do mapper selecionado e do mapper do dispositivo quando disponível.</translation>
    </message>
    <message>
        <source>LUKS / crypttab evidence</source>
        <translation>LUKS / cripttab evidência</translation>
    </message>
    <message>
        <source>Shows LUKS/mapped ancestry plus crypttab and fstab mapper references.</source>
        <translation>Mostra referências LUKS/mapped ancestry plus cryptab and fstab mapper.</translation>
    </message>
    <message>
        <source>Full diagnostic report</source>
        <translation>Relatório diagnóstico completo</translation>
    </message>
    <message>
        <source>Combines all read-only diagnostics for the selected scope (same as Run All).</source>
        <translation>Combina todos os diagnósticos somente de leitura para o escopo selecionado (o mesmo que Executar Todos).</translation>
    </message>
    <message>
        <source>All entries</source>
        <translation>Todos os itens</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>Diagnósticos</translation>
    </message>
    <message>
        <source>Repairs</source>
        <translation>Reparos</translation>
    </message>
    <message>
        <source>Package repair</source>
        <translation>Reparação de embalagens</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Cópia do arquivo</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Descoberta do dispositivo</translation>
    </message>
    <message>
        <source>Host</source>
        <translation>Máquina</translation>
    </message>
    <message>
        <source>Required for block-device inventory</source>
        <translation>Necessário para o inventário de dispositivos de bloco</translation>
    </message>
    <message>
        <source>Filesystem identification</source>
        <translation>Identificação do sistema de ficheiros</translation>
    </message>
    <message>
        <source>Used to identify filesystem metadata</source>
        <translation>Usado para identificar metadados do sistema de ficheiros</translation>
    </message>
    <message>
        <source>Mount inspection</source>
        <translation>Inspecção da montagem</translation>
    </message>
    <message>
        <source>Used to understand active mounts</source>
        <translation>Usado para compreender as montagens activas</translation>
    </message>
    <message>
        <source>LUKS support</source>
        <translation>Suporte LUKS</translation>
    </message>
    <message>
        <source>Required to unlock encrypted targets</source>
        <translation>É necessário desbloquear alvos encriptados</translation>
    </message>
    <message>
        <source>Btrfs support</source>
        <translation>Suporte Btrfs</translation>
    </message>
    <message>
        <source>Required for Btrfs inspection and snapshot rollback</source>
        <translation>Necessário para inspeção Btrfs e rollback instantâneo</translation>
    </message>
    <message>
        <source>Bidirectional file copy</source>
        <translation>Cópia de arquivo bidirecional</translation>
    </message>
    <message>
        <source>Host/Repair</source>
        <translation>Máquina/Reparação</translation>
    </message>
    <message>
        <source>Required for verified Host-to-Repair and Repair-to-Host transfer</source>
        <translation>Necessário para a transferência verificada Host-to-Repair e Reparar-to-Host</translation>
    </message>
    <message>
        <source>Chroot repair</source>
        <translation>Reparação de raízes</translation>
    </message>
    <message>
        <source>Required for target-side repair commands</source>
        <translation>Necessário para comandos de reparo do lado-alvo</translation>
    </message>
    <message>
        <source>Offline systemd repair</source>
        <translation>Reparação de sistemas off-line</translation>
    </message>
    <message>
        <source>Used to restore graphical.target and the configured display manager without starting the target GUI</source>
        <translation>Usado para restaurar a gráfica. alvo e o gerenciador de exibição configurado sem iniciar a GUI de destino</translation>
    </message>
    <message>
        <source>UEFI NVRAM inspection</source>
        <translation>inspeção UEFI NVRAM</translation>
    </message>
    <message>
        <source>Used to preserve target EFI BootOrder during TUXEDO UKI rebuilds when efivars are available</source>
        <translation>Usado para preservar alvo EFI BootOrder durante TUXEDO UKI reconstrui quando efivars estão disponíveis</translation>
    </message>
    <message>
        <source>UKI verification</source>
        <translation>Verificação UKI</translation>
    </message>
    <message>
        <source>Used to verify the kernel embedded in a rebuilt unified kernel image</source>
        <translation>Usado para verificar o kernel incorporado numa imagem do kernel unificada reconstruída</translation>
    </message>
    <message>
        <source>GRUB EFI repair</source>
        <translation>Reparação GRUB EFI</translation>
    </message>
    <message>
        <source>Target/Host</source>
        <translation>Alvo/Host</translation>
    </message>
    <message>
        <source>Required only for conventional GRUB-based EFI systems</source>
        <translation>Necessário apenas para sistemas convencionais EFI baseados em GRUB</translation>
    </message>
    <message>
        <source>Target</source>
        <translation>Alvo</translation>
    </message>
    <message>
        <source>Debian-family GRUB helper</source>
        <translation>Ajudador GRUB da família Debian</translation>
    </message>
    <message>
        <source>Portable GRUB configuration generator used by Arch and other non-Debian systems</source>
        <translation>Gerador de configuração GRUB portátil usado por Arch e outros sistemas não-debian</translation>
    </message>
    <message>
        <source>Initramfs rebuild</source>
        <translation>Initramfs reconstruir</translation>
    </message>
    <message>
        <source>Debian-family initramfs helper</source>
        <translation>Ajudador initramfs da família Debian</translation>
    </message>
    <message>
        <source>Arch-family initramfs generator</source>
        <translation>Gerador initramfs da família Arch</translation>
    </message>
    <message>
        <source>Alternative initramfs generator used by Arch and other distributions</source>
        <translation>Gerador initramfs alternativo usado pelo Arch e outras distribuições</translation>
    </message>
    <message>
        <source>Initramfs verification</source>
        <translation>Verificação do initramfs</translation>
    </message>
    <message>
        <source>Read-only verification for mkinitcpio images</source>
        <translation>Verificação somente de leitura para imagens mkinitcpio</translation>
    </message>
    <message>
        <source>Read-only verification for dracut images</source>
        <translation>Verificação somente de leitura para imagens dracut</translation>
    </message>
    <message>
        <source>systemd-boot inspection</source>
        <translation>Inspeção systemd-boot</translation>
    </message>
    <message>
        <source>Host/Target</source>
        <translation>Máquina/Alvo</translation>
    </message>
    <message>
        <source>Read-only inspection of systemd-boot and generic UKI layouts</source>
        <translation>Inspeção somente de leitura de systemd-boot e layouts genéricos UKI</translation>
    </message>
    <message>
        <source>Arch package manager</source>
        <translation>Gestor de pacotes Arch</translation>
    </message>
    <message>
        <source>Arch-family package database and transaction tool</source>
        <translation>Arch-family pacote banco de dados e ferramenta de transação</translation>
    </message>
    <message>
        <source>DKMS rebuild</source>
        <translation>Reconstrução do DKMS</translation>
    </message>
    <message>
        <source>Required only when target uses DKMS modules</source>
        <translation>Só é necessário quando o alvo utiliza módulos DKMS</translation>
    </message>
    <message>
        <source>LVM inspection</source>
        <translation>Inspecção LVM</translation>
    </message>
    <message>
        <source>Optional LVM storage-stack support</source>
        <translation>Suporte LVM opcional para armazenamento-estaca</translation>
    </message>
    <message>
        <source>Software RAID</source>
        <translation>RAID de software</translation>
    </message>
    <message>
        <source>Optional Linux MD RAID support</source>
        <translation>Suporte opcional ao Linux MD RAID</translation>
    </message>
    <message>
        <source>Process namespace isolation</source>
        <translation>Isolamento do espaço de nomes do processo</translation>
    </message>
    <message>
        <source>Host+Target</source>
        <translation>Máquina+ Alvo</translation>
    </message>
    <message>
        <source>The ported helper falls back to a guarded plain chroot when unshare is absent</source>
        <translation>O ajudante portado cai de volta para um chroot simples guardado quando não compartilha está ausente</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Cancelar</translation>
    </message>
    <message>
        <source>OK</source>
        <translation>Está bem.</translation>
    </message>
    <message>
        <source>Yes</source>
        <translation>Sim.</translation>
    </message>
    <message>
        <source>No</source>
        <translation>Não</translation>
    </message>
    <message>
        <source>read-only helper diagnostic</source>
        <translation>diagnóstico do auxiliar somente para leitura</translation>
    </message>
</context>
<context>
    <name>legacy::LegacyMainWindow</name>
    <message>
        <source>Boot Bitch Legacy (Etch / KDE 3.5 era)</source>
        <translation>Legado Boot Bitch (era Etch / KDE 3.5)</translation>
    </message>
    <message>
        <source>Linux recovery and boot-repair utility</source>
        <translation>Recuperação Linux e utilitário de recuperação de inicialização</translation>
    </message>
    <message>
        <source>GUARDED REPAIR</source>
        <translation>REPARAÇÃO CONGELADA</translation>
    </message>
    <message>
        <source>Ordinary repairs require an explicitly selected non-host target. The protected running host has a separate deliberate maintenance mode with the same guarded repair stages and requires privilege authorization.</source>
        <translation>Os reparos comuns requerem um alvo não-host explicitamente selecionado. O host em execução protegido tem um modo de manutenção deliberado separado com as mesmas etapas de reparo vigiadas e requer autorização de privilégio.</translation>
    </message>
    <message>
        <source>Systems</source>
        <translation>Sistemas</translation>
    </message>
    <message>
        <source>Diagnostics</source>
        <translation>Diagnósticos</translation>
    </message>
    <message>
        <source>Repair</source>
        <translation>Reparação</translation>
    </message>
    <message>
        <source>Chroot Shell</source>
        <translation>Croot Shell</translation>
    </message>
    <message>
        <source>File Copy</source>
        <translation>Ficheiro Copiar</translation>
    </message>
    <message>
        <source>Logs</source>
        <translation>Registos</translation>
    </message>
    <message>
        <source>Settings</source>
        <translation>Configurações</translation>
    </message>
    <message>
        <source>(not created yet)</source>
        <translation>(ainda não criado)</translation>
    </message>
    <message>
        <source>Information</source>
        <translation>Informação</translation>
    </message>
    <message>
        <source>Close</source>
        <translation>Fechar</translation>
    </message>
    <message>
        <source>&amp;File</source>
        <translation>&amp;Arquivo</translation>
    </message>
    <message>
        <source>&amp;Refresh Devices</source>
        <translation>&amp;Atualizar dispositivos</translation>
    </message>
    <message>
        <source>&amp;Lock Administrator Session</source>
        <translation>&amp;Sessão de Administrador de Bloqueio</translation>
    </message>
    <message>
        <source>&amp;Quit</source>
        <translation>&amp;Sair do</translation>
    </message>
    <message>
        <source>&amp;View</source>
        <translation>&amp;Vista</translation>
    </message>
    <message>
        <source>&amp;Systems</source>
        <translation>&amp;Sistemas</translation>
    </message>
    <message>
        <source>&amp;Diagnostics</source>
        <translation>&amp;Diagnósticos</translation>
    </message>
    <message>
        <source>&amp;Logs</source>
        <translation>&amp;Registos</translation>
    </message>
    <message>
        <source>&amp;Settings</source>
        <translation>&amp;Configuração do</translation>
    </message>
    <message>
        <source>&amp;Auto-size Device Columns</source>
        <translation>&amp;Colunas de Dispositivo de Tamanho Automático do</translation>
    </message>
    <message>
        <source>&amp;Wrap Log Lines</source>
        <translation>&amp;Wrap Log Lines</translation>
    </message>
    <message>
        <source>&amp;Help</source>
        <translation>&amp;Ajuda do</translation>
    </message>
    <message>
        <source>&amp;Using Boot Bitch</source>
        <translation>&amp;usando Boot Bitch</translation>
    </message>
    <message>
        <source>&amp;About Boot Bitch</source>
        <translation>&amp;Sobre Boot Bitch</translation>
    </message>
    <message>
        <source>Device columns auto-sized. Drag headers to fine-tune widths.</source>
        <translation>Colunas de dispositivos de tamanho automático. Arraste os cabeçalhos para ajustar as larguras.</translation>
    </message>
    <message>
        <source>A helper command is running; wait for it to finish before locking the session.</source>
        <translation>Um comando helper está em execução; espere que ele termine antes de bloquear a sessão.</translation>
    </message>
    <message>
        <source>Administrator session locked; the next privileged action will request authorization.</source>
        <translation>Sessão de administrador bloqueada; a próxima ação privilegiada solicitará autorização.</translation>
    </message>
    <message>
        <source>Using Boot Bitch</source>
        <translation>Usando Boot Bitch</translation>
    </message>
    <message>
        <source>Boot Bitch must run from a different booted Linux environment than the system being repaired. Use a Linux live medium or another Linux installation on a different physical drive.&lt;br&gt;&lt;br&gt;The running host is protected from ordinary repair-target selection, but it can be explicitly selected through &lt;b&gt;Host Maintenance&lt;/b&gt; for guarded native diagnostics and supported maintenance stages.&lt;br&gt;&lt;br&gt;Diagnostics follow the Systems page: the committed repair drive while Host Maintenance is off, or the protected running host while it is active.&lt;br&gt;&lt;br&gt;The first privileged action requests administrator authorization once for this Boot Bitch window; &lt;b&gt;File - Lock Administrator Session&lt;/b&gt; ends that helper session immediately. Every repair keeps the helper&apos;s own runtime preflights.</source>
        <translation>Boot Bitch deve ser executado a partir de um ambiente Linux inicializado diferente do sistema que está sendo reparado. Use uma mídia Linux live ou outra instalação Linux em uma unidade física diferente.&lt;br&gt;&lt;br&gt; O host em execução é protegido da seleção comum de reparo-alvo, mas pode ser explicitamente selecionado através de &lt;b&gt;Manutenção de Host&lt;/b&gt; para diagnósticos nativos guardados e estágios de manutenção suportados. Os diagnósticos seguem a página Sistemas: a unidade de reparo comprometida enquanto Host Maintenance está desligada, ou o host em execução protegido enquanto está ativo.&lt;br&gt;&lt;br&gt; A primeira ação privilegiada solicita autorização de administrador uma vez para esta janela Boot Bitch; &lt;b&gt;File - Lock Administrator Session&lt;/b&gt; termina essa sessão de helper imediatamente. Cada reparação mantém os pré-vôos do ajudante.</translation>
    </message>
    <message>
        <source>Select a physical drive; Boot Bitch resolves the most likely Linux system volume automatically. The running host stays protected from ordinary target repairs, with a separate explicit host-maintenance path for its own system. The Details button shows the protected host&apos;s facts in the details pane; selecting any drive row restores the per-drive pane.</source>
        <translation>Selecione uma unidade física; Boot Bitch resolve o volume do sistema Linux mais provável automaticamente. O host em execução permanece protegido de reparos de alvos comuns, com um caminho de manutenção de host explícito separado para seu próprio sistema. O botão Detalhes mostra os fatos da máquina protegida no painel de detalhes; selecionar qualquer linha de unidade restaura o painel por unidade.</translation>
    </message>
    <message>
        <source>Refresh Devices</source>
        <translation>Atualizar dispositivos</translation>
    </message>
    <message>
        <source>Re-read the read-only kernel inventory (/proc/partitions, /proc/mounts, /proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* and the udev metadata database). No block device is opened and nothing is written.</source>
        <translation>Leia novamente o inventário do kernel somente leitura (/proc/partições, /proc/mounts, /proc/swaps, /sys/block, /dev/mapper, /dev/disk/by-* e o banco de dados de metadados udev). Nenhum dispositivo de bloco é aberto e nada está escrito.</translation>
    </message>
    <message>
        <source>[OK]</source>
        <translation>[OK]</translation>
    </message>
    <message>
        <source>The running system was detected and stays protected from ordinary target repairs.</source>
        <translation>O sistema de corrida foi detectado e permanece protegido contra reparos de alvos comuns.</translation>
    </message>
    <message>
        <source>Detecting running system...</source>
        <translation>A detectar o sistema em execução...</translation>
    </message>
    <message>
        <source>Detecting protected storage...</source>
        <translation>Detectando armazenamento protegido...</translation>
    </message>
    <message>
        <source>PROTECTED</source>
        <translation>PROTEgido</translation>
    </message>
    <message>
        <source>The running host remains protected from ordinary repair-target operations.</source>
        <translation>O hospedeiro em execução permanece protegido de operações normais de reparação-alvo.</translation>
    </message>
    <message>
        <source>Details</source>
        <translation>Detalhes</translation>
    </message>
    <message>
        <source>Show read-only details for the protected running host.</source>
        <translation>Mostrar os detalhes apenas de leitura da máquina em execução protegida.</translation>
    </message>
    <message>
        <source>Host Maintenance</source>
        <translation>Manutenção da Máquina</translation>
    </message>
    <message>
        <source>Make Default</source>
        <translation>Predefinição</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host (menu.lst default directive with a backup and rollback). Requires Host Maintenance and the cached host-default probe.</source>
        <translation>Faça com que a entrada canônica instalada do kernel a entrada padrão de inicialização GRUB-legacy no host em execução (menu.lst default diretive with a backup and rollback). Requer Manutenção do Host e a sonda cached host-default.</translation>
    </message>
    <message>
        <source>Available repair targets</source>
        <translation>Objectivos de reparação disponíveis</translation>
    </message>
    <message>
        <source>Most likely first</source>
        <translation>Provavelmente primeiro.</translation>
    </message>
    <message>
        <source>Drives are listed by the read-only inventory. Select a row to inspect it; Select Target commits the selected non-host drive with its auto-resolved Linux root component.</source>
        <translation>As unidades são listadas pelo inventário somente de leitura. Selecione uma linha para inspecioná- la; Selecione Target commits a unidade não- host selecionada com seu componente de raiz Linux resolvido automaticamente.</translation>
    </message>
    <message>
        <source>Device</source>
        <translation>Dispositivo</translation>
    </message>
    <message>
        <source>Size</source>
        <translation>Tamanho</translation>
    </message>
    <message>
        <source>Type</source>
        <translation>Tipo</translation>
    </message>
    <message>
        <source>Filesystem</source>
        <translation>Sistema de arquivos</translation>
    </message>
    <message>
        <source>Select Target</source>
        <translation>Selecionar alvo</translation>
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
        <translation>Estabelecer a sessão de ajuda privilegiada para o escopo atual agora em vez de esperar pela próxima ação privilegiada.</translation>
    </message>
    <message>
        <source>Committed target: none</source>
        <translation>Alvo autorizado: nenhum</translation>
    </message>
    <message>
        <source>Unlock state for the selected drive; the LUKS passphrase is never logged.</source>
        <translation>Desbloqueie o estado da unidade selecionada; a frase-passe LUKS nunca é registrada.</translation>
    </message>
    <message>
        <source>Unlock status</source>
        <translation>Desbloquear status</translation>
    </message>
    <message>
        <source>Read-only inventory plus helper-confirmed facts; mirrors the modern Qt6 Selected drive details panel.</source>
        <translation>Inventário somente leitura e fatos confirmados pelo ajudante; espelha o moderno painel Qt6 Selected drive details.</translation>
    </message>
    <message>
        <source>Selected drive details</source>
        <translation>Detalhes selecionados da unidade</translation>
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
        <translation>Executar Todos executa todos os diagnósticos disponíveis somente leitura para o escopo atual; selecionando uma verificação executa-o sozinho. Os diagnósticos são somente de leitura e são a única fonte de evidência para as ações de reparo fechadas.</translation>
    </message>
    <message>
        <source>Target: none selected</source>
        <translation>Alvo: nenhum selecionado</translation>
    </message>
    <message>
        <source>Diagnostics follow the committed repair target, or the protected running host while Host Maintenance is active.</source>
        <translation>Os diagnósticos seguem o alvo de reparo comprometido, ou o hospedeiro em execução protegido enquanto a Manutenção do Host está ativa.</translation>
    </message>
    <message>
        <source>Run All</source>
        <translation>Executar Tudo</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope.</source>
        <translation>Executar tudo - execute todos os diagnósticos disponíveis somente de leitura para o escopo atual.</translation>
    </message>
    <message>
        <source>Target configuration:</source>
        <translation>Configuração do alvo:</translation>
    </message>
    <message>
        <source>Etch-era target configuration files; availability is probed read-only by the helper&apos;s diagnostics.</source>
        <translation>Os ficheiros de configuração de destino do Etch-era; a disponibilidade é examinada apenas para leitura pelos diagnósticos do auxiliar.</translation>
    </message>
    <message>
        <source>Edit Target File...</source>
        <translation>Editar o Ficheiro Alvo...</translation>
    </message>
    <message>
        <source>Runs one read-only diagnostic for the selected scope through the helper (`diagnose &lt;key&gt;` / `host-diagnose &lt;key&gt;`); Run All is the combined report.</source>
        <translation>Executa um diagnóstico somente leitura para o escopo selecionado através do helper (`diagnose &lt;key&gt;` / `host-diagnose &lt;key&gt;`); Run All é o relatório combinado.</translation>
    </message>
    <message>
        <source>Diagnostic checks</source>
        <translation>Controlos de diagnóstico</translation>
    </message>
    <message>
        <source>Check</source>
        <translation>Verificar</translation>
    </message>
    <message>
        <source>Selected diagnostic</source>
        <translation>Diagnóstico selecionado</translation>
    </message>
    <message>
        <source>Select a diagnostic</source>
        <translation>Selecione um diagnóstico</translation>
    </message>
    <message>
        <source>Choose a diagnostic from the list.</source>
        <translation>Escolha um diagnóstico da lista.</translation>
    </message>
    <message>
        <source>Ready</source>
        <translation>Pronto</translation>
    </message>
    <message>
        <source>Results</source>
        <translation>Resultados</translation>
    </message>
    <message>
        <source>Run Diagnostic</source>
        <translation>Executar o Diagnóstico</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic for the current scope.</source>
        <translation>Run Diagnostic - execute o diagnóstico selecionado somente leitura para o escopo atual.</translation>
    </message>
    <message>
        <source>Copy Results</source>
        <translation>Copiar os Resultados</translation>
    </message>
    <message>
        <source>Save Results...</source>
        <translation>Salvar resultados...</translation>
    </message>
    <message>
        <source>The Full Repair plan runs the selected legacy stages in order through the guarded helper; the individual tools run one stage at a time. Every action stays disabled until the cached capability lines say available and the helper keeps its runtime preflights.</source>
        <translation>O plano de reparo completo executa os estágios legados selecionados em ordem através do ajudante vigiado; as ferramentas individuais executam um estágio de cada vez. Cada ação permanece desativada até que as linhas de capacidade em cache digam disponíveis e o ajudante mantenha seus pré-voos em tempo de execução.</translation>
    </message>
    <message>
        <source>Full Repair plan</source>
        <translation>Plano de reparação completo</translation>
    </message>
    <message>
        <source>No stages selected</source>
        <translation>Nenhuma etapa selecionada</translation>
    </message>
    <message>
        <source>Configure Plan...</source>
        <translation>Configurar plano...</translation>
    </message>
    <message>
        <source>Open Settings to choose which Full Repair stages are part of the plan.</source>
        <translation>Abra Configurações para escolher quais etapas de reparo completo são parte do plano.</translation>
    </message>
    <message>
        <source>Run Full Repair</source>
        <translation>Executar reparação completa</translation>
    </message>
    <message>
        <source>Select a repair drive, or choose Host Maintenance on the protected running-host card.</source>
        <translation>Selecione uma unidade de reparo, ou escolha Manutenção Host no cartão de execução protegido.</translation>
    </message>
    <message>
        <source>Stage</source>
        <translation>Etapa</translation>
    </message>
    <message>
        <source>Individual repair tools</source>
        <translation>Ferramentas de reparação individuais</translation>
    </message>
    <message>
        <source>Tool</source>
        <translation>Ferramenta</translation>
    </message>
    <message>
        <source>Full Repair</source>
        <translation>Reparação completa</translation>
    </message>
    <message>
        <source>not reported</source>
        <translation>não reportado</translation>
    </message>
    <message>
        <source>Selected tool</source>
        <translation>Ferramenta selecionada</translation>
    </message>
    <message>
        <source>Select a repair tool</source>
        <translation>Selecione uma ferramenta de reparo</translation>
    </message>
    <message>
        <source>Run Tool</source>
        <translation>Ferramenta de Execução</translation>
    </message>
    <message>
        <source>Select a tool to review its repair action.</source>
        <translation>Selecione uma ferramenta para rever sua ação de reparo.</translation>
    </message>
    <message>
        <source>Write actions ask for confirmation and then run the helper&apos;s own runtime preflights; the GUI never weakens them. A repair that is not proven &apos;unchanged&apos; invalidates the cached diagnostics and disables the gated actions until diagnostics run again.</source>
        <translation>As ações de escrita pedem confirmação e, em seguida, executam os pré-voos do próprio ajudante; a GUI nunca os enfraquece. Um reparo que não é provado &apos;inalterado&apos; invalida os diagnósticos em cache e desativa as ações fechadas até que o diagnóstico seja executado novamente.</translation>
    </message>
    <message>
        <source>Chroot shell</source>
        <translation>Carapaça de croot</translation>
    </message>
    <message>
        <source>Offline commands run one at a time in a fresh chroot and cannot answer interactive prompts (apt-get -y upgrade works). Host-shell commands run directly on the running host. The helper&apos;s probe lines gate the command field; the exact reason appears in the tooltip.</source>
        <translation>Comandos off-line rodam um de cada vez em um chroot fresco e não pode responder prompts interativos (apt-get -y upgrade funciona). Os comandos host-shell são executados diretamente na máquina em execução. As linhas de sonda do ajudante portam o campo de comando; a razão exata aparece na dica.</translation>
    </message>
    <message>
        <source>No &apos;Legacy feature shell:&apos; line is cached; run diagnostics for the selected scope to evaluate the helper&apos;s chroot/timeout containment probes (fail closed).</source>
        <translation>Nenhuma linha &apos;Legacy feature shell:&apos; está em cache; execute diagnósticos para o escopo selecionado para avaliar as sondas de contenção chroot/timeout do ajudante (falha fechada).</translation>
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
        <translation>Um texto de comando revisado, passado para o helper como um único argumento (sem interpolação shell pela GUI).</translation>
    </message>
    <message>
        <source>Run Command</source>
        <translation>Executar o Comando</translation>
    </message>
    <message>
        <source>Clear Output</source>
        <translation>Limpar saída</translation>
    </message>
    <message>
        <source>The helper exposes `shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for an offline target chroot and `host-shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` for the running host. Both keep the helper&apos;s runtime preflights; this tab enables the command only when the scope is committed, the session is authorized and the scope&apos;s Legacy feature probe reports available.</source>
        <translation>O auxiliar expõe `shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` para um chroot alvo offline e `host-shell &lt;disk&gt; &lt;root&gt; &lt;command&gt;` para a máquina em execução. Ambos mantêm os pré-voos de tempo de execução do helper; esta aba permite o comando somente quando o escopo é comprometido, a sessão é autorizada e os relatórios de sonda do recurso Legacy do escopo disponíveis.</translation>
    </message>
    <message>
        <source>File copy</source>
        <translation>Cópia do arquivo</translation>
    </message>
    <message>
        <source>Copy and verify files in either direction through the guarded helper (cp -a plus ownership restoration and a per-file byte-compare). The helper&apos;s file-copy probe gates the controls and keeps the direction and path containment checks.</source>
        <translation>Copie e verifique arquivos em qualquer direção através do helper vigiado (cp -a mais restauração de propriedade e um byte-compare por arquivo). A sonda de cópia de arquivo do ajudante fecha os controles e mantém a direção e o caminho de verificação de contenção.</translation>
    </message>
    <message>
        <source>Preview Changes</source>
        <translation>Antevisão das Alterações</translation>
    </message>
    <message>
        <source>Run a copy dry-run through the guarded helper. No files are changed.</source>
        <translation>Passe uma cópia a seco pelo ajudante vigiado. Nenhum arquivo é alterado.</translation>
    </message>
    <message>
        <source>Copy and Verify</source>
        <translation>Copiar e Verificar</translation>
    </message>
    <message>
        <source>Copy staged items and verify the result. Existing destination names are overwritten when source content differs; unrelated destination files are never deleted.</source>
        <translation>Copie os itens encenados e verifique o resultado. Nomes de destino existentes são substituídos quando o conteúdo de origem difere; arquivos de destino não relacionados nunca são excluídos.</translation>
    </message>
    <message>
        <source>Direction:</source>
        <translation>Direcção:</translation>
    </message>
    <message>
        <source>Choose which system supplies the source files and which system receives them.</source>
        <translation>Escolha qual sistema fornece os arquivos de origem e qual sistema os recebe.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from this host</source>
        <translation>1. Selecione arquivos ou pastas de origem desta máquina</translation>
    </message>
    <message>
        <source>Source</source>
        <translation>Origem</translation>
    </message>
    <message>
        <source>Files and folders staged for the verified copy. The legacy backend copies with cp -a and restores ownership with chown --reference; every regular file is byte-compared after the copy.</source>
        <translation>Arquivos e pastas encenadas para a cópia verificada. A infra-estrutura legada copia com cp -a e restaura a propriedade com chown --reference; cada arquivo regular é comparado por byte após a cópia.</translation>
    </message>
    <message>
        <source>Add Files...</source>
        <translation>Adicionar arquivos...</translation>
    </message>
    <message>
        <source>Add Folder...</source>
        <translation>Adicionar uma Pasta...</translation>
    </message>
    <message>
        <source>Remove</source>
        <translation>Remover</translation>
    </message>
    <message>
        <source>Clear</source>
        <translation>Limpar</translation>
    </message>
    <message>
        <source>Clear the staged source list (nothing is copied or deleted).</source>
        <translation>Limpar a lista de fontes encenada (nada é copiado ou excluído).</translation>
    </message>
    <message>
        <source>2. Choose destination in repaired system</source>
        <translation>2. Escolha o destino no sistema reparado</translation>
    </message>
    <message>
        <source>unavailable: see the helper&apos;s Legacy feature file-copy: probe reason above</source>
        <translation>indisponível: veja a cópia de arquivo do recurso Legacy do ajudante: razão da sonda acima</translation>
    </message>
    <message>
        <source>An absolute path inside the selected repair system (Host to Repair) or on the running host (Repair to Host).</source>
        <translation>Um caminho absoluto dentro do sistema de reparo selecionado (Host to Repair) ou na máquina em execução (Repair to Host).</translation>
    </message>
    <message>
        <source>Browse Target Folders...</source>
        <translation>Navegar nas Pastas Alvo...</translation>
    </message>
    <message>
        <source>Browse the selected repair system through the helper&apos;s temporary read-only mounts and choose an absolute destination path. No target files are changed while browsing.</source>
        <translation>Navegue pelo sistema de reparo selecionado através das montagens temporárias somente de leitura do ajudante e escolha um caminho de destino absoluto. Nenhum arquivo de destino é alterado durante a navegação.</translation>
    </message>
    <message>
        <source>3. Ownership and copy policy</source>
        <translation>3. Política de propriedade e cópia</translation>
    </message>
    <message>
        <source>Ownership:</source>
        <translation>Propriedade:</translation>
    </message>
    <message>
        <source>Smart destination ownership (recommended)</source>
        <translation>Propriedade de destino inteligente (recomendada)</translation>
    </message>
    <message>
        <source>Preserve source numeric UID/GID</source>
        <translation>Preservar UID/GID numérico de origem</translation>
    </message>
    <message>
        <source>Smart mode validates UID/GID identity mapping across the two systems and falls back to the destination-directory owner when the same numeric ID means a different account (the legacy backend implements it with chown --reference).</source>
        <translation>O modo inteligente valida o mapeamento de identidade UID/GID entre os dois sistemas e cai de volta para o proprietário do diretório de destino quando o mesmo ID numérico significa uma conta diferente (a infra-estrutura legada implementa-a com chown --reference).</translation>
    </message>
    <message>
        <source>Application log</source>
        <translation>Registo de aplicações</translation>
    </message>
    <message>
        <source>The complete session register; Save As... writes every entry even while a filter hides lines. If a writable system share mount exists at /host, Save As... starts there; otherwise the log directory is the fallback. Prior session files are listed read-only.</source>
        <translation>O registo completo da sessão; Gravar Como... escreve todos os itens, mesmo quando um filtro esconde linhas. Se existir um compartilhamento de sistema gravável no /host, Salvar Como... começa aí; caso contrário, o diretório de log é o backback. Os arquivos de sessão anteriores estão listados somente para leitura.</translation>
    </message>
    <message>
        <source>Session logs</source>
        <translation>Registos de sessão</translation>
    </message>
    <message>
        <source>Session</source>
        <translation>Sessão</translation>
    </message>
    <message>
        <source>The first entry is the live session; earlier files in the log directory are listed read-only below it.</source>
        <translation>O primeiro item é a sessão ao vivo; os arquivos anteriores no diretório de log estão listados somente para leitura abaixo dele.</translation>
    </message>
    <message>
        <source>New Session Log</source>
        <translation>Novo Registo de Sessão</translation>
    </message>
    <message>
        <source>Close the active session file; it becomes a prior session and the next log entry starts a new file.</source>
        <translation>Feche o arquivo de sessão ativa; ele se torna uma sessão anterior e o próximo registro inicia um novo arquivo.</translation>
    </message>
    <message>
        <source>Add Note</source>
        <translation>Adicionar uma Nota</translation>
    </message>
    <message>
        <source>Append a NOTE entry to the live session register.</source>
        <translation>Anexar uma entrada de NOTA ao registro de sessão ao vivo.</translation>
    </message>
    <message>
        <source>Delete</source>
        <translation>Apagar</translation>
    </message>
    <message>
        <source>Delete the selected prior session file (the live session is never deleted).</source>
        <translation>Apagar o ficheiro de sessão anterior seleccionado (a sessão ao vivo nunca é apagada).</translation>
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
        <translation>Salve o registro completo da sessão (todos os itens, não apenas o filtro atual).</translation>
    </message>
    <message>
        <source>Clear Register</source>
        <translation>Limpar o Registo</translation>
    </message>
    <message>
        <source>Clear the live register and view; prior session files are never modified.</source>
        <translation>Limpar o registro e visualização ao vivo; arquivos de sessão anteriores nunca são modificados.</translation>
    </message>
    <message>
        <source>Search log:</source>
        <translation>Registo de pesquisa:</translation>
    </message>
    <message>
        <source>Type any characters to show matching log entries (case-insensitive). Save As always writes every entry.</source>
        <translation>Digite quaisquer caracteres para mostrar os registros correspondentes (insensíveis ao caso). Gravar Como sempre escreve todas as entradas.</translation>
    </message>
    <message>
        <source>Filter:</source>
        <translation>Filtro:</translation>
    </message>
    <message>
        <source>Filter the visible log by entry kind. Selecting a diagnostic section shows the lines captured for that section; a workflow filter such as File system repair or Package repair shows its mapped repair lines (File copy has no lines in this frontend). Save As always writes every entry.</source>
        <translation>Filtrar o log visível por tipo de entrada. Selecionando uma seção de diagnóstico mostra as linhas capturadas para essa seção; um filtro de fluxo de trabalho, como o reparo do sistema de arquivos ou o reparo do pacote, mostra suas linhas de reparo mapeadas (a cópia do arquivo não tem linhas neste frontend). Gravar Como sempre escreve todas as entradas.</translation>
    </message>
    <message>
        <source>Settings are stored per user under ~/.qt/, one file per settings group (devicesrc, logsrc, diagnosticsrc, repairrc), and are saved immediately on every change and on close. Launch the GUI as the same user to keep your overrides; a GUI started as root keeps its own copies.</source>
        <translation>As configurações são armazenadas por usuário em ~/.qt/, um arquivo por grupo de configurações (dispositivosrc, logsrc, diagnosticsrc, repairrc), e são salvas imediatamente em cada mudança e ao fechar. Inicie a GUI como o mesmo usuário para manter suas sobreposições; uma GUI iniciada como root mantém suas próprias cópias.</translation>
    </message>
    <message>
        <source>Device discovery</source>
        <translation>Descoberta do dispositivo</translation>
    </message>
    <message>
        <source>Show devices without an identified Linux installation</source>
        <translation>Mostrar os dispositivos sem uma instalação Linux identificada</translation>
    </message>
    <message>
        <source>Show removable and USB storage</source>
        <translation>Mostrar armazenamento removível e USB</translation>
    </message>
    <message>
        <source>Show encrypted devices before unlocking</source>
        <translation>Mostrar os dispositivos encriptados antes de desbloquear</translation>
    </message>
    <message>
        <source>When off, drives without a visible Linux filesystem are hidden unless they still contain an encrypted device and encrypted devices are shown.</source>
        <translation>Quando desligados, unidades sem um sistema de arquivos Linux visível são escondidas a menos que ainda contenham um dispositivo criptografado e dispositivos criptografados são mostrados.</translation>
    </message>
    <message>
        <source>When off, removable and USB drives are hidden from the repair-target list.</source>
        <translation>Quando desligado, as unidades removíveis e USB são ocultas da lista de reparo-alvo.</translation>
    </message>
    <message>
        <source>When off, drives with an encrypted device are hidden until the volume is unlocked.</source>
        <translation>Quando desligado, unidades com um dispositivo criptografado são escondidas até que o volume seja desbloqueado.</translation>
    </message>
    <message>
        <source>Include the %1 stage in the Full Repair plan. The stage runs in the plan order shown on the Repair tab.</source>
        <translation>Incluir o estágio %1 no plano de reparo completo. O palco é executado na ordem de plano mostrada na aba Reparar.</translation>
    </message>
    <message>
        <source>Automatically regenerate read-only diagnostics after repairs or target changes</source>
        <translation>Regenerar automaticamente os diagnósticos somente de leitura após reparos ou mudanças de alvo</translation>
    </message>
    <message>
        <source>Regenerates the cached read-only diagnostics for the current scope after an operation that invalidates them (LUKS unlock, target configuration edit). It runs only inside an already authorized administrator session and never opens an authorization prompt by itself.</source>
        <translation>Regenera os diagnósticos somente de leitura em cache para o escopo atual após uma operação que os invalida (travagem LUKS, edição de configuração de destino). Ele é executado apenas dentro de uma sessão de administrador já autorizada e nunca abre um prompt de autorização por si só.</translation>
    </message>
    <message>
        <source>Wrap long log lines</source>
        <translation>Quebrar linhas de registo longas</translation>
    </message>
    <message>
        <source>Mandatory safety controls</source>
        <translation>Controlos obrigatórios de segurança</translation>
    </message>
    <message>
        <source>Host capabilities and dependencies</source>
        <translation>Capacidades e dependências da máquina</translation>
    </message>
    <message>
        <source>Refresh Capabilities</source>
        <translation>Atualizar capacidades</translation>
    </message>
    <message>
        <source>Re-run the read-only host capability probes (a PATH search, nothing is executed) and refresh the distribution summary.</source>
        <translation>Repetir as sondas de capacidade de host somente leitura (uma pesquisa PATH, nada é executado) e atualizar o resumo da distribuição.</translation>
    </message>
    <message>
        <source>Install Missing Support...</source>
        <translation>Instalar suporte em falta...</translation>
    </message>
    <message>
        <source>Automatic installation will require explicit package mapping and privilege authorization.</source>
        <translation>A instalação automática exigirá mapeamento explícito de pacotes e autorização de privilégio.</translation>
    </message>
    <message>
        <source>Application configuration</source>
        <translation>Configuração da aplicação</translation>
    </message>
    <message>
        <source>Boot Bitch Legacy</source>
        <translation>Boot Bitch Legado</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list first.</source>
        <translation>Selecione uma unidade física na lista de alvos de reparos disponíveis primeiro.</translation>
    </message>
    <message>
        <source>Protected system</source>
        <translation>Sistema protegido</translation>
    </message>
    <message>
        <source>The running system cannot be selected as a repair target. Use Host Maintenance for the protected running host or choose another disk.</source>
        <translation>O sistema em execução não pode ser selecionado como um alvo de reparo. Use a manutenção da máquina para a máquina em execução protegida ou escolha outro disco.</translation>
    </message>
    <message>
        <source>Unlock or select a Linux system first</source>
        <translation>Desbloquear ou selecionar um sistema Linux primeiro</translation>
    </message>
    <message>
        <source>This encrypted drive has no visible Linux filesystem yet. Use Unlock, refresh devices, and select the target after its Linux root is detected.</source>
        <translation>Esta unidade criptografada ainda não tem sistema de arquivos Linux visível. Use Desbloquear, atualizar dispositivos e selecionar o alvo após sua raiz Linux ser detectada.</translation>
    </message>
    <message>
        <source>The selected root component (%1) belongs to the running system and cannot be committed as a repair target. Use Host Maintenance for the protected running host.</source>
        <translation>O componente root selecionado (%1) pertence ao sistema de execução e não pode ser comprometido como um alvo de reparo. Use a manutenção da máquina para a máquina em execução protegida.</translation>
    </message>
    <message>
        <source>repair target commit</source>
        <translation>reparar o compromisso de destino</translation>
    </message>
    <message>
        <source>Repair drive selected: %1</source>
        <translation>Unidade de reparo selecionada: %1</translation>
    </message>
    <message>
        <source>; best detected system component: %1</source>
        <translation>; melhor componente do sistema detectado: %1</translation>
    </message>
    <message>
        <source>. No mount or repair action was performed.</source>
        <translation>Não foi realizada nenhuma ação de montagem ou reparo.</translation>
    </message>
    <message>
        <source>Make Default unavailable</source>
        <translation>Tornar Indisponível por Omissão</translation>
    </message>
    <message>
        <source>Make Default is a running-host action on this frontend; enter Host Maintenance first.</source>
        <translation>Make Default é uma ação de máquina em execução nesta interface; insira a Manutenção da Máquina primeiro.</translation>
    </message>
    <message>
        <source>Administrator authorization required</source>
        <translation>Autorização do administrador necessária</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize first.</source>
        <translation>Nenhuma sessão de administrador está ativa; pressione Autorizar primeiro.</translation>
    </message>
    <message>
        <source>Make the canonical installed kernel entry the default GRUB-legacy boot entry on the running host?

The helper verifies /boot/grub/menu.lst, sets the `default &lt;N&gt;` directive to the canonical entry, backs the menu up first and restores it on any failure.</source>
        <translation>Tornar a entrada do kernel instalado canônico a entrada de inicialização GRUB-legacy padrão na máquina em execução?

O helper verifica /boot/grub/menu.lst, configura a diretiva `default &lt;N&gt;` para a entrada canônica, faz backup do menu primeiro e restaura-o em qualquer falha.</translation>
    </message>
    <message>
        <source>Host Maintenance unavailable</source>
        <translation>Manutenção da máquina indisponível</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a committed repair target or a detected running host.</source>
        <translation>O alvo do host em execução não pôde ser detectado; os diagnósticos precisam de um alvo de reparo comprometido ou um host em execução detectado.</translation>
    </message>
    <message>
        <source>Exit Host Maintenance</source>
        <translation>Sair da Manutenção da Máquina</translation>
    </message>
    <message>
        <source>Host Maintenance is active; diagnostics and gated repairs target the protected running host.</source>
        <translation>A manutenção do host está ativa; os diagnósticos e reparos fechados visam o host de corrida protegido.</translation>
    </message>
    <message>
        <source>Select a physical drive in the Available repair targets list on the Systems tab first, or use Host Maintenance for the protected running host.</source>
        <translation>Selecione uma unidade física na lista de alvos de reparo disponíveis na guia Sistemas primeiro, ou use a Manutenção Host para o host em execução protegido.</translation>
    </message>
    <message>
        <source>The selected drive (%1) is the protected running host. Choose Host Maintenance on the Systems tab to run read-only host diagnostics and guarded host repairs; ordinary target repairs stay disabled.</source>
        <translation>A unidade selecionada (%1) é a máquina em execução protegida. Escolha Manutenção Host na guia Sistemas para executar diagnósticos de host somente leitura e reparos de host vigiados; reparos de destino comuns permanecem desativados.</translation>
    </message>
    <message>
        <source>No repair target is committed. Choose Select Target on the Systems tab (or Host Maintenance for the protected running host) first.</source>
        <translation>Nenhum alvo de reparação está comprometido. Escolha Selecionar Alvo na aba Sistemas (ou Manutenção do Host para o host em execução protegido) primeiro.</translation>
    </message>
    <message>
        <source>The selection changed after the target was committed. Choose Select Target again.</source>
        <translation>A seleção mudou depois que o alvo foi comprometido. Escolha Selecionar Alvo novamente.</translation>
    </message>
    <message>
        <source>Diagnostics scope required</source>
        <translation>Âmbito de diagnóstico necessário</translation>
    </message>
    <message>
        <source>Diagnostics scope unresolved</source>
        <translation>Âmbito de diagnóstico não resolvido</translation>
    </message>
    <message>
        <source>The running host target could not be resolved; use Refresh Devices and commit a repair target or re-enter Host Maintenance.</source>
        <translation>O alvo da máquina em execução não pôde ser resolvido; use Atualizar Dispositivos e commit um alvo de reparo ou reinserir Manutenção da Máquina.</translation>
    </message>
    <message>
        <source>Diagnostic check required</source>
        <translation>Verificação diagnóstica necessária</translation>
    </message>
    <message>
        <source>Select a diagnostic check in the list first.</source>
        <translation>Selecione primeiro uma verificação diagnóstica na lista.</translation>
    </message>
    <message>
        <source>Repair target required</source>
        <translation>É necessário reparar o alvo</translation>
    </message>
    <message>
        <source>Target file editing needs a committed repair target. Running-host maintenance has no target-file editing; commit an offline target first.</source>
        <translation>A edição de arquivo alvo precisa de um alvo de reparo comprometido. A manutenção da máquina em execução não tem edição de ficheiros de destino; primeiro, commit um alvo offline.</translation>
    </message>
    <message>
        <source>Configuration file required</source>
        <translation>Arquivo de configuração necessário</translation>
    </message>
    <message>
        <source>Select a target configuration file first.</source>
        <translation>Selecione primeiro um arquivo de configuração de destino.</translation>
    </message>
    <message>
        <source>Edit target %1</source>
        <translation>Editar o alvo %1</translation>
    </message>
    <message>
        <source>Edit this target file through the guarded administrator helper. A successful save invalidates cached diagnostics; rerun diagnostics before repair. Generated files such as /boot/grub/menu.lst may be replaced by the next bootloader update.</source>
        <translation>Edite este arquivo de destino através do ajudante de administrador vigiado. Um bem sucedido salvar invalida diagnósticos em cache; reexecução diagnósticos antes de reparar. Arquivos gerados como /boot/grub/menu.lst podem ser substituídos pela próxima atualização do bootloader.</translation>
    </message>
    <message>
        <source>Cancel</source>
        <translation>Cancelar</translation>
    </message>
    <message>
        <source>Save Target File</source>
        <translation>Gravar o Ficheiro Alvo</translation>
    </message>
    <message>
        <source>No changes to %1.</source>
        <translation>Sem alterações para %1.</translation>
    </message>
    <message>
        <source>Configuration write refused</source>
        <translation>A gravação da configuração foi recusada</translation>
    </message>
    <message>
        <source>The edited content contains NUL bytes; the guarded write refuses it.</source>
        <translation>O conteúdo editado contém bytes NUL; a escrita guardada o recusa.</translation>
    </message>
    <message>
        <source>File too large</source>
        <translation>Arquivo muito grande</translation>
    </message>
    <message>
        <source>The edited file is larger than 1 MiB. The guarded write refuses it; edit the file from a console instead.</source>
        <translation>O arquivo editado é maior que 1 MiB. A escrita guardada recusa- a; edite o ficheiro de uma consola.</translation>
    </message>
    <message>
        <source>Write target configuration</source>
        <translation>Gravar a configuração do alvo</translation>
    </message>
    <message>
        <source>Write the edited contents to %1? This modifies the repair target and invalidates cached diagnostics.</source>
        <translation>Escrever o conteúdo editado para %1? Isso modifica o alvo de reparo e invalida diagnósticos em cache.</translation>
    </message>
    <message>
        <source>No diagnostic results to copy yet.</source>
        <translation>Nenhum resultado diagnóstico para copiar ainda.</translation>
    </message>
    <message>
        <source>Diagnostic results copied to the clipboard.</source>
        <translation>Resultados de diagnóstico copiados para a área de transferência.</translation>
    </message>
    <message>
        <source>No diagnostic results to save yet.</source>
        <translation>Nenhum resultado diagnóstico para salvar ainda.</translation>
    </message>
    <message>
        <source>Text files (*.txt);;All files (*)</source>
        <translation>Arquivos de texto (*.txt);; Todos os arquivos (*)</translation>
    </message>
    <message>
        <source>Save diagnostic results</source>
        <translation>Salvar resultados de diagnóstico</translation>
    </message>
    <message>
        <source>Could not write %1.</source>
        <translation>Não foi possível escrever %1.</translation>
    </message>
    <message>
        <source>Diagnostic results saved to %1</source>
        <translation>Resultados diagnósticos salvos em %1</translation>
    </message>
    <message>
        <source>Unlock not available</source>
        <translation>Desbloquear não disponível</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked. Select an offline repair target to unlock.</source>
        <translation>A máquina em execução protegida não pode ser desbloqueada. Selecione um alvo de reparo offline para desbloquear.</translation>
    </message>
    <message>
        <source>No locked LUKS component is currently visible on this selected drive.</source>
        <translation>Nenhum componente LUKS bloqueado está atualmente visível nesta unidade selecionada.</translation>
    </message>
    <message>
        <source>Confirm LUKS unlock</source>
        <translation>Confirmar desbloqueio LUKS</translation>
    </message>
    <message>
        <source>Unlock %1 on %2?

The helper opens a temporary device-mapper mapping with cryptsetup and keeps it open for this recovery session. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>Desbloquear %1 em %2?

O helper abre um mapeamento temporário dispositivo-mapper com criptsetup e o mantém aberto para esta sessão de recuperação. A frase-passe viaja através de um arquivo de chaves privado e nunca é colocada em argumentos de comando ou logs.</translation>
    </message>
    <message>
        <source>LUKS unlock</source>
        <translation>LUKS desbloquear</translation>
    </message>
    <message>
        <source>Unlock LUKS repair target</source>
        <translation>Desbloquear alvo de reparo LUKS</translation>
    </message>
    <message>
        <source>Enter the passphrase for %1.

It is sent only to cryptsetup over the helper&apos;s standard input and is never logged or placed on a command line.</source>
        <translation>Digite a frase- senha para %1.

Ele é enviado apenas para cryptsetup sobre a entrada padrão do helper e nunca é registrado ou colocado em uma linha de comando.</translation>
    </message>
    <message>
        <source>Passphrase required</source>
        <translation>Frase- senha necessária</translation>
    </message>
    <message>
        <source>An empty passphrase was not submitted. Enter the LUKS passphrase or choose Cancel.</source>
        <translation>Não foi apresentada uma frase- senha vazia. Digite a frase- senha LUKS ou escolha Cancelar.</translation>
    </message>
    <message>
        <source>Unlock keyfile unavailable</source>
        <translation>Desbloquear o arquivo de chaves indisponível</translation>
    </message>
    <message>
        <source>The LUKS passphrase could not be written to a private keyfile in %1; the unlock was not started.</source>
        <translation>A frase- senha LUKS não pôde ser escrita para um arquivo de chaves privado em %1; o desbloqueio não foi iniciado.</translation>
    </message>
    <message>
        <source>Configuration write unavailable</source>
        <translation>Configuração não disponível</translation>
    </message>
    <message>
        <source>The edited content could not be written to a private temporary file in %1; the write was not started.</source>
        <translation>O conteúdo editado não pôde ser escrito para um arquivo temporário privado em %1; a escrita não foi iniciada.</translation>
    </message>
    <message>
        <source>ERROR: cannot read the session log %1</source>
        <translation>ERRO: não é possível ler o log da sessão %1</translation>
    </message>
    <message>
        <source>Viewing a prior session log (read-only): %1</source>
        <translation>Visualizando um registro de sessão anterior (somente leitura): %1</translation>
    </message>
    <message>
        <source>Session log list refreshed.</source>
        <translation>A lista de registros de sessão foi atualizada.</translation>
    </message>
    <message>
        <source>Note:</source>
        <translation>Nota:</translation>
    </message>
    <message>
        <source>Delete session log</source>
        <translation>Apagar o registo de sessão</translation>
    </message>
    <message>
        <source>Delete %1 permanently? This cannot be undone.</source>
        <translation>Apagar o %1 permanentemente? Isto não pode ser desfeito.</translation>
    </message>
    <message>
        <source>%1 changed while the confirmation was open; the delete was refused.</source>
        <translation>O %1 mudou enquanto a confirmação estava aberta; o delete foi recusado.</translation>
    </message>
    <message>
        <source>Unable to delete %1.</source>
        <translation>Não foi possível apagar o %1.</translation>
    </message>
    <message>
        <source>Device discovery filters updated.</source>
        <translation>Filtros de descoberta de dispositivos actualizados.</translation>
    </message>
    <message>
        <source>Unknown Linux distribution</source>
        <translation>Distribuição Linux desconhecida</translation>
    </message>
    <message>
        <source>sudo / gksu (no KAuth on this frontend)</source>
        <translation>sudo / gksu (sem KAuth nesta interface)</translation>
    </message>
    <message>
        <source>Available</source>
        <translation>Disponível</translation>
    </message>
    <message>
        <source>Missing on this frontend</source>
        <translation>Faltando nesta interface</translation>
    </message>
    <message>
        <source>Missing</source>
        <translation>Faltando</translation>
    </message>
    <message>
        <source>No repair tool is selected.</source>
        <translation>Nenhuma ferramenta de reparo está selecionada.</translation>
    </message>
    <message>
        <source>A helper command is already running.</source>
        <translation>Um comando auxiliar já está em execução.</translation>
    </message>
    <message>
        <source>Restore Graphical Login is a host-scope stage on this legacy frontend; enter Host Maintenance to run it.</source>
        <translation>Restaurar o Login Gráfico é uma etapa host-scope neste frontend legado; digite Manutenção Host para executá-lo.</translation>
    </message>
    <message>
        <source>The selected scope has no resolved root component; use Refresh Devices and commit the repair target again.</source>
        <translation>O escopo selecionado não tem componente root resolvido; use Atualizar dispositivos e commit o alvo de reparo novamente.</translation>
    </message>
    <message>
        <source>This legacy frontend exposes no %1 action; the helper reports the capability as available.</source>
        <translation>Este frontend legado não expõe nenhuma ação %1; o helper relata a capacidade como disponível.</translation>
    </message>
    <message>
        <source>Run this guarded repair action using the cached read-only diagnostic evidence. A confirmation is shown first.</source>
        <translation>Execute esta ação de reparo guardada usando a evidência diagnóstica somente de leitura em cache. Uma confirmação é mostrada primeiro.</translation>
    </message>
    <message>
        <source>Enabled in Settings</source>
        <translation>Activado na Configuração</translation>
    </message>
    <message>
        <source>Disabled in Settings - enable it to include this stage</source>
        <translation>Desactivado nas Definições - habilite- o a incluir esta fase</translation>
    </message>
    <message>
        <source>Unavailable: %1</source>
        <translation>Indisponível: %1</translation>
    </message>
    <message>
        <source>
Unavailable: %1</source>
        <translation>
Indisponível: %1</translation>
    </message>
    <message>
        <source>Repair tool unavailable</source>
        <translation>Ferramenta de reparação indisponível</translation>
    </message>
    <message>
        <source>Confirm repair</source>
        <translation>Confirmar reparação</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Your saved selection is kept and its availability is re-checked when diagnostics for this scope complete.</source>
        <translation>Ainda não há provas de capacidade para esta fase. Sua seleção salva é mantida e sua disponibilidade é re-checked quando os diagnósticos para este escopo completo.</translation>
    </message>
    <message>
        <source>No cached capability evidence for this stage yet. Run diagnostics for the selected scope to populate the Full Repair plan; your selection is saved once the stage becomes available.</source>
        <translation>Ainda não há provas de capacidade para esta fase. Executar diagnósticos para o escopo selecionado para preencher o plano de reparo completo; sua seleção é salva uma vez que o estágio fica disponível.</translation>
    </message>
    <message>
        <source>Unknown Full Repair stage.</source>
        <translation>Fase de reparação completa desconhecida.</translation>
    </message>
    <message>
        <source>No Full Repair stages are selected or available; use Configure Plan... to choose the stages.</source>
        <translation>Nenhum estágio de reparo completo está selecionado ou disponível; use Configurar plano... para escolher os estágios.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab to establish the session.</source>
        <translation>É necessária autorização do administrador; pressione Autorizar na aba Sistemas ou Reparar para estabelecer a sessão.</translation>
    </message>
    <message>
        <source>Runs the selected stages using the cached read-only diagnostic evidence after privilege confirmation.</source>
        <translation>Executa as etapas selecionadas usando a evidência diagnóstica somente leitura em cache após a confirmação do privilégio.</translation>
    </message>
    <message>
        <source>No Full Repair stages selected - use Configure Plan... or Settings.</source>
        <translation>Nenhum estágio de reparação completo selecionado - use Configurar plano... ou configurações.</translation>
    </message>
    <message>
        <source>1 stage selected</source>
        <translation>1 fase seleccionada</translation>
    </message>
    <message>
        <source>%1 stages selected</source>
        <translation>Fases %1 seleccionadas</translation>
    </message>
    <message>
        <source>No repair stages are selected. Use Configure Plan... to choose the stages Full Repair will run.</source>
        <translation>Não são selecionados estágios de reparo. Use Configurar plano... para escolher as etapas de reparação completa será executado.</translation>
    </message>
    <message>
        <source>No selected stages are available. %1</source>
        <translation>Nenhum estágio selecionado está disponível. %1</translation>
    </message>
    <message>
        <source>Full Repair unavailable</source>
        <translation>Reparação completa indisponível</translation>
    </message>
    <message>
        <source>Run the Full Repair plan?

The selected stages run in order through the helper&apos;s guarded repair command:

</source>
        <translation>Executar o plano de reparação completa?

Os estágios selecionados são executados em ordem através do comando de reparo vigiado do ajudante:

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
O ajudante mantém cada pré-voo em tempo de execução; um estágio que falha para o plano.</translation>
    </message>
    <message>
        <source>Running %1 through the privileged helper... The Logs tab keeps the complete transcript.</source>
        <translation>A correr %1 através do ajudante privilegiado... A aba Registros mantém a transcrição completa.</translation>
    </message>
    <message>
        <source>Shell scope required</source>
        <translation>Âmbito Shell exigido</translation>
    </message>
    <message>
        <source>No administrator session is active.

Press Authorize on the Systems or Repair tab to establish the session, or enter Host Maintenance / commit a repair target on the Systems tab; the chroot shell then reuses the cached authorization.</source>
        <translation>Nenhuma sessão de administrador está ativa.

Pressione Autorizar na guia Sistemas ou Reparar para estabelecer a sessão, ou insira Manutenção do Host / commit um alvo de reparo na guia Sistemas; o shell chroot então reutiliza a autorização em cache.</translation>
    </message>
    <message>
        <source>Shell command required</source>
        <translation>É necessário o comando Shell</translation>
    </message>
    <message>
        <source>Enter the command to run first.</source>
        <translation>Digite o comando a executar primeiro.</translation>
    </message>
    <message>
        <source>Confirm running-host command</source>
        <translation>Confirmar o comando da máquina em execução</translation>
    </message>
    <message>
        <source>Run this command as root on the protected running host?

%1

The helper keeps its runtime preflights; the command is passed as one argument and is never interpreted by the GUI.</source>
        <translation>Executar este comando como root na máquina em execução protegida?

%1

O ajudante mantém seus pré-voos em tempo de execução; o comando é passado como um argumento e nunca é interpretado pela GUI.</translation>
    </message>
    <message>
        <source>Running %1...</source>
        <translation>A executar o %1...</translation>
    </message>
    <message>
        <source>Authorization scope required</source>
        <translation>Âmbito de autorização necessário</translation>
    </message>
    <message>
        <source>administrator authorization</source>
        <translation>autorização do administrador</translation>
    </message>
    <message>
        <source>Administrator authorization</source>
        <translation>Autorização do administrador</translation>
    </message>
    <message>
        <source>Administrator authorization is required for %1.

Enter the password for %2 (sudo). It is used only for this sudo authentication, is sent over a pipe and is never logged or placed on a command line. The authorization is cached for this session and reused by diagnostics and repairs.</source>
        <translation>A autorização do administrador é necessária para %1.

Digite a senha para %2 (sudo). Ele é usado apenas para esta autenticação sudo, é enviado sobre um pipe e nunca é registrado ou colocado em uma linha de comando. A autorização é armazenada para esta sessão e reutilizada por diagnósticos e reparos.</translation>
    </message>
    <message>
        <source>your account</source>
        <translation>sua conta</translation>
    </message>
    <message>
        <source>Password required</source>
        <translation>Senha necessária</translation>
    </message>
    <message>
        <source>An empty password was not submitted. Enter the sudo password or choose Cancel.</source>
        <translation>Não foi enviada uma senha vazia. Digite a senha do sudo ou escolha Cancelar.</translation>
    </message>
    <message>
        <source>Administrator authorization failed</source>
        <translation>A autorização do administrador falhou</translation>
    </message>
    <message>
        <source>sudo did not accept the password: %1

The command was not started.</source>
        <translation>sudo não aceitou a senha: %1

O comando não foi iniciado.</translation>
    </message>
    <message>
        <source>elevation requires an interactive sudo password; run the smoke as root or after `sudo -S -v` with --elevate &apos;sudo -n&apos;</source>
        <translation>elevação requer uma senha de sudo interativa; execute o smoke como root ou depois `sudo -S -v&apos; com -- elevate &apos;sudo -n&apos;</translation>
    </message>
    <message>
        <source>No administrator session is active for %1.

Press Authorize on the Systems or Repair tab to establish the session now, or enter Host Maintenance / commit a repair target on the Systems tab; diagnostics and repairs then reuse the cached authorization.</source>
        <translation>Nenhuma sessão de administrador está ativa para %1.

Pressione Autorizar na aba Sistemas ou Reparar para estabelecer a sessão agora, ou insira Manutenção Host / commit um alvo de reparo na guia Sistemas; diagnósticos e reparos então reutilizar a autorização em cache.</translation>
    </message>
    <message>
        <source>Administrator authorization expired</source>
        <translation>Autorização do administrador expirada</translation>
    </message>
    <message>
        <source>The cached administrator authorization for %1 expired or was refused.

Press Authorize on the Systems or Repair tab to re-establish the session, then run the command again. No command was started.</source>
        <translation>A autorização de administrador em cache para %1 expirou ou foi recusada.

Pressione Autorizar na aba Sistemas ou Reparar para restabelecer a sessão e então executar o comando novamente. Nenhum comando foi iniciado.</translation>
    </message>
    <message>
        <source>Privileged operation completed successfully. Administrator authorization remains active for this session.</source>
        <translation>Operação privilegiada concluída com sucesso. A autorização do administrador permanece ativa para esta sessão.</translation>
    </message>
    <message>
        <source>Privileged operation stopped with an error. Administrator authorization remains active; review the output before closing.</source>
        <translation>A operação privada parou com um erro. A autorização do administrador permanece ativa; reveja a saída antes de fechar.</translation>
    </message>
    <message>
        <source>Inspection is read-only.

</source>
        <translation>A inspeção é somente leitura.

</translation>
    </message>
    <message>
        <source>Configuration unavailable</source>
        <translation>Configuração indisponível</translation>
    </message>
    <message>
        <source>The helper could not read %1:

%2</source>
        <translation>O ajudante não conseguiu ler %1:

%2</translation>
    </message>
    <message>
        <source>Configuration write failed</source>
        <translation>Erro na gravação da configuração</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
Result: mapping opened for this recovery session.</source>
        <translation>Estado: desbloqueado
Componente: %1
Mapper: %2
Método: helper destravar (criptsetup; frase- senha via um arquivo de chave modo- 600, excluído após o uso)
Resultado: mapeamento aberto para esta sessão de recuperação.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: the passphrase was not accepted; retry offered.</source>
        <translation>Estado: bloqueado
Componente: %1
Método: helper desbloquear (criptografar)
Erro: a frase- senha não foi aceita; tente novamente.</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Method: helper unlock (cryptsetup)
Error: %2</source>
        <translation>Estado: bloqueado
Componente: %1
Método: helper desbloquear (criptografar)
Erro: %2</translation>
    </message>
    <message>
        <source>Passphrase not accepted</source>
        <translation>Frase- senha não aceita</translation>
    </message>
    <message>
        <source>The LUKS passphrase was not accepted.

Try again?</source>
        <translation>A frase-passe LUKS não foi aceita.

Tentar outra vez?</translation>
    </message>
    <message>
        <source>[-] %1 - not run (the plan stopped before reaching this stage)</source>
        <translation>[-] %1 - não executar (o plano parou antes de chegar a esta fase)</translation>
    </message>
    <message>
        <source>filesystem</source>
        <translation>sistema de ficheiros</translation>
    </message>
    <message>
        <source>[-] %1 - no file system errors found - no changes</source>
        <translation>[-] %1 - não foram encontrados erros de sistema de arquivos - não foram encontradas alterações</translation>
    </message>
    <message>
        <source>[FAIL] %1 - not reported</source>
        <translation>[FAIL] %1 - não comunicado</translation>
    </message>
    <message>
        <source>%1 stage(s) failed; review the helper output in Logs.</source>
        <translation>O( s) estágio( s) %1 falhou; reveja o resultado do helper em Logs.</translation>
    </message>
    <message>
        <source>the plan stopped before any stage completed.</source>
        <translation>O plano parou antes de qualquer fase ser concluída.</translation>
    </message>
    <message>
        <source>no repair was needed; cached diagnostics remain valid.</source>
        <translation>nenhum reparo foi necessário; diagnósticos em cache permanecem válidos.</translation>
    </message>
    <message>
        <source>repair completed; cached diagnostics were invalidated and must be regenerated.</source>
        <translation>os diagnósticos em cache foram invalidados e devem ser regenerados.</translation>
    </message>
    <message>
        <source>%1 results: [OK] %2 successful | [FAIL] %3 failed | [-] %4 no repair needed - %5</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Re-run Diagnostic</source>
        <translation>Diagnóstico de repetição</translation>
    </message>
    <message>
        <source>Unavailable</source>
        <translation>Indisponível</translation>
    </message>
    <message>
        <source>Commit this physical drive as the repair target.</source>
        <translation>Transmita esta unidade física como alvo de reparação.</translation>
    </message>
    <message>
        <source>Protection:</source>
        <translation>Protecção:</translation>
    </message>
    <message>
        <source>Host shell</source>
        <translation>Shell da máquina</translation>
    </message>
    <message>
        <source>Run on Host</source>
        <translation>Executar na Máquina</translation>
    </message>
    <message>
        <source>Host Shell</source>
        <translation>Máquina Shell</translation>
    </message>
    <message>
        <source>The read-only probe found no editable target configuration file in this target. %1</source>
        <translation>A sonda somente leitura não encontrou nenhum arquivo de configuração de destino editável neste alvo. %1</translation>
    </message>
    <message>
        <source>Not present in the selected target (omitted): %1</source>
        <translation>Não presente no alvo seleccionado (omitido): %1</translation>
    </message>
    <message>
        <source>Run diagnostics to probe which target configuration files exist; the helper&apos;s read-only probe decides the list.</source>
        <translation>Executar diagnósticos para sondar quais os arquivos de configuração de destino existem; a sonda somente de leitura do ajudante decide a lista.</translation>
    </message>
    <message>
        <source>Probed read-only by the helper; a saved edit invalidates the cached diagnostics.</source>
        <translation>Uma edição salva invalida os diagnósticos em cache.</translation>
    </message>
    <message>
        <source>Host maintenance: %1</source>
        <translation>Manutenção da máquina: %1</translation>
    </message>
    <message>
        <source>unresolved</source>
        <translation>não resolvido</translation>
    </message>
    <message>
        <source>Target: %1 + %2</source>
        <translation>Alvo: %1 + %2</translation>
    </message>
    <message>
        <source>Host command</source>
        <translation>Comando da máquina</translation>
    </message>
    <message>
        <source>Execute a command on the running host as root.</source>
        <translation>Executar um comando na máquina em execução como root.</translation>
    </message>
    <message>
        <source>Execute a command inside the selected repair system as root.</source>
        <translation>Execute um comando dentro do sistema de reparo selecionado como root.</translation>
    </message>
    <message>
        <source>Administrator authorization is required; press Authorize on the Systems or Repair tab (or re-enter Host Maintenance / re-commit the repair target) to authorize this session.</source>
        <translation>Autorização do administrador é necessária; pressione Autorizar na guia Sistemas ou Reparar (ou re-entrar Host Maintenance / re-commit o alvo de reparo) para autorizar esta sessão.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root on the running host through the helper&apos;s guarded host-shell verb.</source>
        <translation>Executar um comando revisado como root no host em execução através do verbo host-shell guardado do helper.</translation>
    </message>
    <message>
        <source>Run one reviewed command as root inside the target chroot through the helper&apos;s guarded shell verb.</source>
        <translation>Execute um comando revisado como root dentro do alvo chroot através do verbo shell guardado do ajudante.</translation>
    </message>
    <message>
        <source>Run a command on the running host as root (sudo is not needed). Commands are executed directly on the active system; output is kept in this window and in the application log.</source>
        <translation>Executar um comando na máquina em execução como root (sudo não é necessário). Os comandos são executados diretamente no sistema ativo; a saída é mantida nesta janela e no registro da aplicação.</translation>
    </message>
    <message>
        <source>Run a command inside the selected repair system as root (sudo is not needed). Commands are executed one at a time in a fresh chroot and cannot answer interactive prompts; use non-interactive flags such as apt-get -y upgrade. Output is kept in this window and in the application log.</source>
        <translation>Execute um comando dentro do sistema de reparo selecionado como root (sudo não é necessário). Os comandos são executados um de cada vez em um novo chroot e não pode responder a prompts interativos; use sinalizadores não-interativos como apt-get -y upgrade. A saída é mantida nesta janela e no log da aplicação.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize or re-enter Host Maintenance / commit a repair target.</source>
        <translation>Nenhuma sessão de administrador está ativa; pressione Autorizar ou reentrar Manutenção do Host / commit um alvo de reparo.</translation>
    </message>
    <message>
        <source>1. Select source files or folders from the repaired system</source>
        <translation>1. Selecione arquivos de origem ou pastas do sistema reparado</translation>
    </message>
    <message>
        <source>2. Choose destination on this host</source>
        <translation>2. Escolha o destino nesta máquina</translation>
    </message>
    <message>
        <source>Add File Path...</source>
        <translation>Adicionar Localização do Ficheiro...</translation>
    </message>
    <message>
        <source>Add Folder Path...</source>
        <translation>Adicionar um Caminho de Pasta...</translation>
    </message>
    <message>
        <source>Browse...</source>
        <translation>Navegar...</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it (Host Maintenance does not provide a repair tree to browse).</source>
        <translation>Selecione a unidade de reparo em Sistemas antes de escolher um destino dentro dele (Host Manutenção não fornece uma árvore de reparo para navegar).</translation>
    </message>
    <message>
        <source>Choose a host destination folder directly.</source>
        <translation>Escolha uma pasta de destino da máquina diretamente.</translation>
    </message>
    <message>
        <source>Copy the staged files and folders with cp -a, restore ownership with chown --reference and byte-compare every regular file afterwards.</source>
        <translation>Copie os arquivos e pastas encenados com cp -a, restaure a propriedade com chown --reference e byte-compare todos os arquivos regulares depois.</translation>
    </message>
    <message>
        <source>File Copy unavailable</source>
        <translation>Arquivo Copiar não disponível</translation>
    </message>
    <message>
        <source>Choose host destination folder</source>
        <translation>Escolha a pasta de destino da máquina</translation>
    </message>
    <message>
        <source>Select a repair target</source>
        <translation>Selecione um alvo de reparo</translation>
    </message>
    <message>
        <source>Select the repair drive in Systems before choosing a destination inside it.</source>
        <translation>Selecione a unidade de reparo em Sistemas antes de escolher um destino dentro dele.</translation>
    </message>
    <message>
        <source>File Copy - Browse Target Folders</source>
        <translation>Ficheiro Copiar - Navegar nas Pastas de Destino</translation>
    </message>
    <message>
        <source>Browse Target Folders</source>
        <translation>Navegar nas Pastas do Alvo</translation>
    </message>
    <message>
        <source>The helper could not list the repair-system folder:

%1</source>
        <translation>O helper não pôde listar a pasta do sistema de reparos:

%1</translation>
    </message>
    <message>
        <source>BROWSE_ENTRY</source>
        <translation>BROWSE ENTRY</translation>
    </message>
    <message>
        <source>(choose this folder: %1)</source>
        <translation>(escolha esta pasta: %1)</translation>
    </message>
    <message>
        <source>(parent folder)</source>
        <translation>(pasta pai)</translation>
    </message>
    <message>
        <source>Select repair-system destination</source>
        <translation>Selecione o destino do sistema de reparo</translation>
    </message>
    <message>
        <source>Choose a destination folder inside the repaired system (current folder: %1):</source>
        <translation>Escolha uma pasta de destino dentro do sistema reparado (pastor atual: %1):</translation>
    </message>
    <message>
        <source>Folder</source>
        <translation>Pasta</translation>
    </message>
    <message>
        <source>Open</source>
        <translation>Abrir</translation>
    </message>
    <message>
        <source>(choose this folder:</source>
        <translation>(escolha esta pasta:</translation>
    </message>
    <message>
        <source>Add files to copy</source>
        <translation>Adicionar arquivos para copiar</translation>
    </message>
    <message>
        <source>Add folder to copy</source>
        <translation>Adicionar pasta para copiar</translation>
    </message>
    <message>
        <source>Stage at least one source and name a destination path first.</source>
        <translation>Stage pelo menos uma fonte e nomeie um caminho de destino primeiro.</translation>
    </message>
    <message>
        <source>Copy the %1 staged item(s) to %2?

The helper keeps its direction and path containment checks; a sensitive repair-system destination is refused unless the helper approves it, and every regular file is byte-compared after the copy.</source>
        <translation>Copiar o( s) item( s) em fase %1 para %2?

O helper mantém suas verificações de direção e caminho de contenção; um destino sensível do sistema de reparo é recusado a menos que o helper o aprove, e cada arquivo regular é comparado após a cópia.</translation>
    </message>
    <message>
        <source>File Copy - Copy and Verify</source>
        <translation>Ficheiro Copiar - Copiar e Verificar</translation>
    </message>
    <message>
        <source>File Copy - Preview Changes</source>
        <translation>Ficheiro Copiar - Antevisão de Alterações</translation>
    </message>
    <message>
        <source>Protected running host - details</source>
        <translation>Máquina em execução protegida - detalhes</translation>
    </message>
    <message>
        <source>not mounted (offline target)</source>
        <translation>não montado (alvo desligado)</translation>
    </message>
    <message>
        <source>Select a drive to see its details.</source>
        <translation>Selecione uma unidade para ver seus detalhes.</translation>
    </message>
    <message>
        <source>Inspecting the selected component.</source>
        <translation>Inspecionando o componente selecionado.</translation>
    </message>
    <message>
        <source>Helper-confirmed by the last read-only diagnostics.</source>
        <translation>Ajudador confirmado pelos últimos diagnósticos somente de leitura.</translation>
    </message>
    <message>
        <source>Read-only inventory only; run diagnostics to confirm.</source>
        <translation>Apenas inventário de leitura; execute diagnósticos para confirmar.</translation>
    </message>
    <message>
        <source>PROTECTED - running system; read-only details only</source>
        <translation>PROTEgido - sistema em execução; apenas detalhes de leitura</translation>
    </message>
    <message>
        <source>Live / installer media - not selectable</source>
        <translation>Mídia ao vivo / instalador - não selecionável</translation>
    </message>
    <message>
        <source>PROTECTED - running host scope</source>
        <translation>PROTEgido - âmbito da máquina em execução</translation>
    </message>
    <message>
        <source>Unlock required before selection</source>
        <translation>Desbloquear antes da seleção</translation>
    </message>
    <message>
        <source>Eligible repair candidate</source>
        <translation>Candidato à reparação elegível</translation>
    </message>
    <message>
        <source>Drive:</source>
        <translation>Conduza:</translation>
    </message>
    <message>
        <source>Detected target:</source>
        <translation>Alvo detectado:</translation>
    </message>
    <message>
        <source>Pending inspection</source>
        <translation>Inspecção pendente</translation>
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
        <translation>Tamanho:</translation>
    </message>
    <message>
        <source>Connection:</source>
        <translation>Ligação:</translation>
    </message>
    <message>
        <source>Filesystem:</source>
        <translation>Sistema de arquivos:</translation>
    </message>
    <message>
        <source>UUID:</source>
        <translation>UUID:</translation>
    </message>
    <message>
        <source>Mounts:</source>
        <translation>Montagens:</translation>
    </message>
    <message>
        <source>Running system protection unresolved</source>
        <translation>Proteção do sistema de execução não resolvida</translation>
    </message>
    <message>
        <source>No protected physical backing disk was identified</source>
        <translation>Não foi identificado nenhum disco de apoio físico protegido</translation>
    </message>
    <message>
        <source>Current running Linux system</source>
        <translation>Sistema Linux em execução atual</translation>
    </message>
    <message>
        <source>Critical mounts: %1</source>
        <translation>Montagens críticas: %1</translation>
    </message>
    <message>
        <source>Read-only protected-running-system facts: the helper OS fact plus the inventory model, device path, size, transport and critical mounts. Nothing here is probed destructively.</source>
        <translation>Fatos do sistema de execução protegida somente para leitura: o fato do OS auxiliar mais o modelo de inventário, caminho do dispositivo, tamanho, transporte e montagens críticas. Nada aqui é sondado destrutivamente.</translation>
    </message>
    <message>
        <source>Select a drive to see unlock status.</source>
        <translation>Selecione uma unidade para ver o estado de desbloqueio.</translation>
    </message>
    <message>
        <source>State: protected
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
The protected running host cannot be unlocked or modified; unlock is available for an offline repair target only. Use Host Maintenance for the protected running host.</source>
        <translation>Estado: protegido
Componente: %1
Mapper: (nenhum)
Método: helper destravar (criptsetup; frase- senha via um arquivo de chave modo- 600, excluído após o uso)
O host em execução protegido não pode ser desbloqueado ou modificado; desbloqueio está disponível apenas para um alvo de reparo offline. Use a manutenção da máquina para a máquina em execução protegida.</translation>
    </message>
    <message>
        <source>(none detected)</source>
        <translation>(nenhum detectado)</translation>
    </message>
    <message>
        <source>State: locked
Component: %1
Mapper: (none)
Method: helper unlock (cryptsetup; passphrase via a mode-600 keyfile, deleted after use)
A locked LUKS container is visible on this drive; press Unlock to open it for this recovery session.</source>
        <translation>Estado: bloqueado
Componente: %1
Mapper: (nenhum)
Método: helper destravar (criptsetup; frase- senha via um arquivo de chave modo- 600, excluído após o uso)
Um recipiente LUKS bloqueado é visível nesta unidade; pressione Desbloquear para abri-lo para esta sessão de recuperação.</translation>
    </message>
    <message>
        <source>(visible mapper)</source>
        <translation>(mapeador visível)</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: already open before this session (visible mapper; Boot Bitch will reuse it and will not close it).</source>
        <translation>Estado: desbloqueado
Componente: %1
Mapper: %2
Método: já aberto antes desta sessão (o mapper visível; Boot Bitch irá reutilizá-lo e não irá fechá-lo).</translation>
    </message>
    <message>
        <source>State: unlocked
Component: %1
Mapper: %2
Method: helper-confirmed mapping from the last read-only diagnostics.</source>
        <translation>Estado: desbloqueado
Componente: %1
Mapper: %2
Método: mapeamento confirmado pelo auxiliar dos últimos diagnósticos somente de leitura.</translation>
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
        <translation>Estado: bloqueado ou nenhum componente criptografado detectado
Componente: (nenhum detectado)
Mapper: (nenhum)
Método: helper destravar (criptsetup; frase- senha via um arquivo de chave modo- 600, excluído após o uso)
Nenhum componente LUKS bloqueado e nenhuma operação de desbloqueio foram gravados para esta unidade na sessão atual.</translation>
    </message>
    <message>
        <source>unavailable - %1</source>
        <translation>indisponível - %1</translation>
    </message>
    <message>
        <source>Host maintenance:
%1</source>
        <translation>Manutenção da máquina:
%1</translation>
    </message>
    <message>
        <source>Target:
%1 + %2</source>
        <translation>Alvo:
%1 + %2</translation>
    </message>
    <message>
        <source>Authorization required: diagnostics and repairs fail closed until you press Authorize.</source>
        <translation>Autorização necessária: diagnósticos e reparos falham fechados até que você pressione Autorizar.</translation>
    </message>
    <message>
        <source>Re-establish the privileged helper session for the current scope now. The password is requested in the hidden-input modal and is never logged.</source>
        <translation>Restabeleça a sessão de ajuda privilegiada para o escopo atual agora. A senha é solicitada no modal de entrada oculta e nunca é registrada.</translation>
    </message>
    <message>
        <source>Running...</source>
        <translation>A correr...</translation>
    </message>
    <message>
        <source>Scope required</source>
        <translation>Âmbito de aplicação exigido</translation>
    </message>
    <message>
        <source>Run All - run every available read-only diagnostic for the current scope; this unlocks the gated actions.</source>
        <translation>Executar Tudo - execute todos os diagnósticos disponíveis somente de leitura para o escopo atual; isso desbloqueia as ações fechadas.</translation>
    </message>
    <message>
        <source>Select a target and wait for any running command first.</source>
        <translation>Selecione um alvo e aguarde por qualquer comando em execução primeiro.</translation>
    </message>
    <message>
        <source>Run Diagnostic - run the selected read-only diagnostic through the helper.</source>
        <translation>Executar diagnóstico - execute o diagnóstico selecionado somente leitura através do ajudante.</translation>
    </message>
    <message>
        <source>Run All for the current scope and refresh the read-only backend profile.</source>
        <translation>Executar tudo para o escopo atual e atualizar o perfil de infraestrutura somente de leitura.</translation>
    </message>
    <message>
        <source>Read or edit the selected target configuration file through the guarded helper; a saved edit invalidates cached diagnostics.</source>
        <translation>Leia ou edite o arquivo de configuração de destino selecionado através do helper vigiado; uma edição salva invalida diagnósticos em cache.</translation>
    </message>
    <message>
        <source>Host Maintenance has no target-file editing; commit an offline repair target first.</source>
        <translation>A manutenção do host não tem edição de arquivo-alvo; commit um alvo de reparo offline primeiro.</translation>
    </message>
    <message>
        <source>Commit an offline repair target first.</source>
        <translation>Transmita um alvo de reparo offline primeiro.</translation>
    </message>
    <message>
        <source>No target configuration file is available for this target; run diagnostics to probe the list.</source>
        <translation>Nenhum arquivo de configuração de destino está disponível para este alvo; execute diagnósticos para sondar a lista.</translation>
    </message>
    <message>
        <source>Already Unlocked</source>
        <translation>Já Desbloqueado</translation>
    </message>
    <message>
        <source>An unlocked Linux filesystem is already visible on this drive. Boot Bitch will reuse the existing mapper and will not close or reopen a mapping created by this recovery session. Mounting happens during diagnostics (read-only) and repairs (read-write); data filesystems are never auto-mounted on selection.</source>
        <translation>Um sistema de arquivos Linux desbloqueado já está visível nesta unidade. Boot Bitch irá reutilizar o mapper existente e não vai fechar ou reabrir um mapeamento criado por esta sessão de recuperação. A montagem acontece durante diagnósticos (apenas leitura) e reparos (leitura-escrita); sistemas de arquivos de dados nunca são montados automaticamente na seleção.</translation>
    </message>
    <message>
        <source>The protected running host cannot be unlocked; use Host Maintenance for the protected running host.</source>
        <translation>A máquina em execução protegida não pode ser desbloqueada; use a Manutenção da Máquina para a máquina em execução protegida.</translation>
    </message>
    <message>
        <source>Host Maintenance is the current scope, but the selected offline drive can still be unlocked.</source>
        <translation>Manutenção Host é o escopo atual, mas a unidade offline selecionada ainda pode ser desbloqueada.</translation>
    </message>
    <message>
        <source>Unlock %1 using cryptsetup through the privileged helper. The passphrase travels through a private keyfile and is never placed in command arguments or logs.</source>
        <translation>Desbloquear %1 usando cryptsetup através do ajudante privilegiado. A frase-passe viaja através de um arquivo de chaves privado e nunca é colocada em argumentos de comando ou logs.</translation>
    </message>
    <message>
        <source>The protected running system cannot be selected as a repair target; use Host Maintenance for the protected running host.</source>
        <translation>O sistema de execução protegido não pode ser selecionado como um alvo de reparo; use a Manutenção Host para o host de execução protegido.</translation>
    </message>
    <message>
        <source>Live / installer media is read-only boot media and cannot be selected as a repair target.</source>
        <translation>Mídia ao vivo / instalador é somente leitura de mídia de inicialização e não pode ser selecionado como um alvo de reparo.</translation>
    </message>
    <message>
        <source>Unlock the encrypted volume first; Select Target becomes available after a Linux filesystem is detected.</source>
        <translation>Desbloqueie o volume criptografado primeiro; Selecione Target fica disponível após um sistema de arquivos Linux ser detectado.</translation>
    </message>
    <message>
        <source>Committed repair target. Repair, Diagnostics and File Copy target this physical drive until another drive is explicitly selected with Select Target.</source>
        <translation>Alvo de reparação autorizado. Reparar, Diagnostics e File Copy visam esta unidade física até que outra unidade seja explicitamente selecionada com Select Target.</translation>
    </message>
    <message>
        <source>Commit %1 as the repair target; this leaves Host Maintenance and switches the scope to the selected drive.</source>
        <translation>Commit %1 como o alvo de reparo; isso deixa Host Manutenção e muda o escopo para a unidade selecionada.</translation>
    </message>
    <message>
        <source>The running host target could not be detected.</source>
        <translation>Não foi possível detectar o alvo da máquina em execução.</translation>
    </message>
    <message>
        <source>Leave Host Maintenance and return to repair-target mode.</source>
        <translation>Deixe a manutenção do host e retorne ao modo de reparo-alvo.</translation>
    </message>
    <message>
        <source>Select the running host for deliberate guarded maintenance; the administrator authorization is requested here once and cached for the session.</source>
        <translation>Selecione a máquina em execução para manutenção deliberada guardada; a autorização do administrador é solicitada aqui uma vez e em cache para a sessão.</translation>
    </message>
    <message>
        <source>The running host target could not be detected; diagnostics need a repair target.</source>
        <translation>O alvo do hospedeiro em execução não pôde ser detectado; os diagnósticos precisam de um alvo de reparo.</translation>
    </message>
    <message>
        <source>Committed target: %1 + %2</source>
        <translation>Alvo comprometido: %1 + %2</translation>
    </message>
    <message>
        <source>Committed target: none (selection changed)</source>
        <translation>Alvo autorizado: nenhum (alterado na seleção)</translation>
    </message>
    <message>
        <source>The selected scope has no resolved Linux root component; Refresh Devices and commit the repair target again.</source>
        <translation>O escopo selecionado não tem componente raiz do Linux resolvido; Atualizar dispositivos e commit o alvo de reparo novamente.</translation>
    </message>
    <message>
        <source>No administrator session is active; press Authorize on the Systems or Repair tab to re-establish it. Run All reuses the cached authorization and never prompts by itself.</source>
        <translation>Nenhuma sessão de administrador está ativa; pressione Autorizar na aba Sistemas ou Reparar para restabelecê-la. Executar Todos reutiliza a autorização em cache e nunca pede por si só.</translation>
    </message>
    <message>
        <source>Run diagnostics for this scope to unlock the gated actions. Diagnostics are read-only and the only evidence source.</source>
        <translation>Executar diagnósticos para este escopo para desbloquear as ações fechadas. Os diagnósticos são apenas de leitura e a única fonte de provas.</translation>
    </message>
    <message>
        <source>A repair was not proven unchanged, so the cached diagnostics are invalidated. Run diagnostics again before another gated action.</source>
        <translation>Um reparo não foi provado inalterado, de modo que os diagnósticos em cache são invalidados. Execute diagnósticos novamente antes de outra ação fechada.</translation>
    </message>
    <message>
        <source>Gated actions reflect the cached capability lines; the helper still runs every runtime preflight when a command starts.</source>
        <translation>As ações do gated refletem as linhas de capacidade em cache; o helper ainda executa cada pré-voo em execução quando um comando começa.</translation>
    </message>
    <message>
        <source>Regenerating diagnostics automatically</source>
        <translation>Regenerando diagnósticos automaticamente</translation>
    </message>
    <message>
        <source>Running diagnostic: %1</source>
        <translation>Executando diagnóstico: %1</translation>
    </message>
    <message>
        <source>Running all diagnostics</source>
        <translation>Executar todos os diagnósticos</translation>
    </message>
    <message>
        <source>Unlocking %1</source>
        <translation>Desbloquear %1</translation>
    </message>
    <message>
        <source>running host</source>
        <translation>máquina em execução</translation>
    </message>
    <message>
        <source>offline target</source>
        <translation>alvo offline</translation>
    </message>
    <message>
        <source>host maintenance active</source>
        <translation>Manutenção da máquina activa</translation>
    </message>
    <message>
        <source>repair target committed</source>
        <translation>alvo de reparação autorizado</translation>
    </message>
    <message>
        <source>no committed scope</source>
        <translation>sem âmbito de aplicação autorizado</translation>
    </message>
    <message>
        <source>Current session</source>
        <translation>Sessão atual</translation>
    </message>
    <message>
        <source>Log files (*.log);;All files (*)</source>
        <translation>Ficheiros de registo (*.log);; Todos os ficheiros (*)</translation>
    </message>
    <message>
        <source>Save log as</source>
        <translation>Gravar o registo como</translation>
    </message>
    <message>
        <source>About Boot Bitch</source>
        <translation>Sobre Boot Bitch</translation>
    </message>
    <message>
        <source>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;&lt;p&gt;&lt;b&gt;Developer:&lt;/b&gt; CaptainMorgan12&lt;/p&gt;&lt;p&gt;This GUI is the package&apos;s entry point: a Qt 3.3.x frontend with the ported guarded helper for Debian Etch-era systems.&lt;/p&gt;&lt;p&gt;&lt;b&gt;Guarded repair mode:&lt;/b&gt; read-only diagnostics can inspect either the protected Running Host or an explicitly selected repair drive. The guarded Debian/APT package stages (interrupted configuration, broken dependencies, metadata refresh, upgrade), GRUB-legacy configuration regeneration, LUKS target unlock and guarded target-file editing run through the ported helper after confirmation; Host Maintenance enables the same supported stages natively on the active system after repeating the host identity and boot-mount checks.&lt;/p&gt;&lt;p&gt;The modern-only features - verified File Copy, Btrfs snapshot rollback, EFI/UKI and extlinux repair, boot-stack reconciliation and Make Default - are greyed with the helper&apos;s own probe reasons on this frontend; Arch/Alpine/Fedora package backends stay diagnostics-only here.&lt;/p&gt;&lt;p&gt;The first privileged action authorizes one cached administrator session per scope through a hidden-input modal (the Qt GUI remains unprivileged). It can be ended at any time from File - Lock Administrator Session.&lt;/p&gt;</source>
        <translation>&lt;h3&gt;Boot Bitch %1&lt;/h3&gt;&lt;p&gt;&lt;b&gt;Desenvolvedor:&lt;/b&gt; Capitão Morgan12&lt;/p&gt;&lt;p&gt; Esta GUI é o ponto de entrada do pacote: uma interface Qt 3.3.x com o helper guardado portado para sistemas Debian Etch-era.&lt;/p&gt;&lt;p&gt;&lt;b&gt;Modo de reparo protegido:&lt;/b&gt; Os diagnósticos somente de leitura podem inspecionar tanto a máquina de execução protegida quanto uma unidade de reparo explicitamente selecionada. Os estágios de pacote Debian/APT guardados (configuração interrompida, dependências quebradas, atualização de metadados), regeneração de configuração GRUB-legacy, desbloqueio de alvo LUKS e edição de arquivo-alvo guardado são executados através do ajudante portado após confirmação; Manutenção do Host permite os mesmos estágios suportados nativamente no sistema ativo após repetir a identidade do host e verificação de inicialização.&lt;/p&gt;&lt;p&gt; Os recursos somente modernos - verificado File Copy, Btrfs snapshot rollback, EFI/UKI e extlinux reparação, reconciliação boot-stack e Make Default - são acinzentados com as razões da sonda do ajudante neste frontend; Arch/Alpine/Fedora backends pacote permanecer diagnóstico-somente aqui. &lt;/p&gt;&lt;p&gt; A primeira ação privilegiada autoriza uma sessão de administrador em cache por escopo através de um modal de entrada oculta (o Qt GUI permanece sem privilégios). Ele pode ser terminado a qualquer momento a partir de File - Lock Administrator Session.&lt;/p&gt;</translation>
    </message>
</context>
</TS>
