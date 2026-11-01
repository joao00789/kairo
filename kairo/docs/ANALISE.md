# Análise do código-fonte (Godot 4.8 beta)

## Visão geral
- 15.745 arquivos. Pastas principais: `core` (base), `servers` (renderização, física, áudio), `scene` (nós), `drivers`, `platform`, `modules`, `editor`, `thirdparty` (5.354 arquivos, bibliotecas).
- Build com SCons. Opções que importam para nós: `disable_3d`, `disable_xr`, `disable_physics_3d`, `disable_navigation_3d`, `build_profile`, `custom_modules`, `module_<nome>_enabled`, `profile=<arquivo.py>`.

## O que já é 2D e vamos reaproveitar
- `scene/2d` (Sprite2D, AnimatedSprite2D, Camera2D, Light2D, CPUParticles2D, Parallax, Skeleton2D...), `scene/2d/physics` (CharacterBody2D, Area2D, TouchScreenButton...), `modules/tilemap`, `modules/godot_physics_2d`, `modules/navigation_2d`, `scene/gui` (UI), `servers/rendering` (renderer GLES3 + Vulkan).

## Descobertas importantes
1. **O editor não compila com `disable_3d`** (SConstruct restringe a opção aos templates de exportação). O template de exportação Android desativa 3D; no editor, o perfil padrão KAIRO oculta a área de trabalho 3D e a criação de nós `Node3D`. O código upstream de 3D continua presente para preservar tipos e serviços compartilhados pelo engine.
2. **Já existe editor rodando no Android** em `platform/android/java/editor`. Dá para ter a engine criando jogos direto no celular/tablet (`scripts/build_editor_android.sh`, experimental).
3. **Módulos são o caminho certo para nossas melhorias**: cada pasta em `modules/` tem `config.py` + `SCsub` + `register_types.cpp`. Não mexemos no núcleo, então atualizar o Godot upstream fica barato.
4. Há ~19 módulos 3D/XR/extras que não servem para 2D (csg, gridmap, gltf, fbx, openxr, jolt_physics etc.); o perfil `android_template_2d.py` já os desliga.

## O que foi feito na V0.1
- Módulo `modules/kairo` com o nó **TouchJoystick2D** (joystick virtual multitoque, modo fixo/dinâmico, zona morta, injeta ações do Input Map). Não existe equivalente no Godot.
- Perfil de build Android 2D leve (GLES3, sem 3D/XR, otimizado para tamanho).
- Scripts de aplicar/branding/build, projeto modelo mobile e este documento.

## Verificação
- Aplicado o overlay numa cópia do código e conferido: o SCons detecta `module_kairo_enabled`, o perfil do template carrega sem erro, e os `.cpp` do módulo passam em `g++ -fsyntax-only -Wall -Wextra` contra os headers reais do Godot.
- **Não foi feita compilação completa nem teste em aparelho** (precisa de NDK/SDK e horas de build). O primeiro build real pode revelar ajustes.

## Licença
Godot é MIT. Mantenha `LICENSE.txt` e `COPYRIGHT.txt`, e não use o nome/logo "Godot" no produto.
