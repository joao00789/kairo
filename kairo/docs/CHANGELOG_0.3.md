# KAIRO 0.3.0 – o que mudou (sobre a 0.2.0 "2d-gray")

## Estado real do 3D (importante)
O 3D **não foi removido do código**: `scene/3d` (126 arquivos), `editor/scene/3d` e os módulos `csg`, `gridmap`, `gltf`, `jolt_physics`, `openxr` etc. continuam na árvore. A 0.2.0 apenas **esconde** o 3D do editor (aba 3D desligada, `Node3D` oculto, abre no 2D). Isso é o caminho seguro: o editor do Godot não compila com `disable_3d`, e várias classes 2D/UI dependem de tipos compartilhados. O jogo final (template Android) já fica sem 3D com `kairo/engine/android_template_2d.py`.

## Revisão das alterações da 0.2.0
- Aba/área de trabalho padrão = 2D e perfil "2D only": correto.
- Tema cinza + destaque cinza: contraste do destaque (≈4:1 sobre a base) é aceitável para UI; só vale conferir seleção/foco em telas ao ar livre.
- Guardas `#ifdef VULKAN_ENABLED` no Android: corretas e consistentes (definições e chamadas no `java_godot_lib_jni.cpp`). Não pude compilar o Android aqui.

## Bugs corrigidos
1. **Perfil 2D reativado a cada abertura do editor** (`editor_feature_profile.cpp`): quem desligava o perfil de propósito o via voltar sempre. Agora ele só é ativado na primeira execução (flag `_kairo_default_profile_applied`). Também deixou de sobrescrever o estado se salvar/carregar o arquivo do perfil falhar.
2. **Handles do 2D difíceis de pegar com o dedo** (`canvas_item_manipulator` + `canvas_item_editor_plugin`): o ícone do handle é desenhado maior no toque, mas a área de captura continuava pequena (7,5 px × escala). Agora a captura acompanha o tamanho desenhado e tem mínimo de 16 px × escala em telas de toque.
3. **TouchJoystick2D deixava o toque "vazar"** para botões/UI embaixo dele. Novo `consume_input` (ligado por padrão) marca o toque como tratado.

## Novidades
- **KAIRO Mobile** (dock no editor, aba ao lado de Nó/Sinais): confere 8 pontos de configuração para mobile 2D, explica o **porquê** de cada um e corrige com um botão (ou "Corrigir tudo"). Itens: renderizador compatível (PC e mobile), compressão ETC2/ASTC, modo/aspecto de escala da tela, FPS máximo, ticks de física, emular toque com mouse.
- **TouchJoystick2D**: `show_idle_hint` (anel mostrando onde tocar no modo dinâmico) e `consume_input`.
- Nome do produto: KAIRO (`version.py`, módulo `modules/kairo`, pasta `kairo/`, perfil "KAIRO 2D").

## Como foi verificado
Compilados de verdade (SCons, editor Linux x86_64, sem erros e sem avisos): `touch_joystick_2d`, `register_types`, `kairo_mobile_doctor`, `canvas_item_manipulator`, `editor_feature_profile`, `canvas_item_editor_plugin`. O perfil `android_template_2d.py` carrega no SCons.

## Ainda NÃO verificado
- Link completo do editor e execução (só compilei os arquivos alterados).
- Qualquer teste em aparelho, e o build Android (precisa de SDK/NDK).
- Não varri os ~15 mil arquivos atrás de bugs: foquei em toque, perfil 2D, Android e nas peças KAIRO.
