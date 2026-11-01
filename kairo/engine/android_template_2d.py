# Opções SCons para o TEMPLATE de exportação Android 2D (jogo final, leve).
# Uso (na raiz do código-fonte):
#   scons platform=android target=template_release arch=arm64 profile=kairo/engine/android_template_2d.py
# Arquivos de perfil são executados pelo SCons, então aqui só valores simples.

# Remove tudo que é 3D/XR (só vale para templates; o editor não aceita estes flags).
disable_3d = "yes"
disable_xr = "yes"
disable_navigation_3d = "yes"
disable_physics_3d = "yes"

# Release enxuto para mobile (o LTO é decidido automaticamente por production=yes).
production = "yes"
optimize = "size"
debug_symbols = "no"
deprecated = "no"
accesskit = "no"

# Gráficos: GLES3 cobre mais aparelhos Android e é mais que suficiente para 2D.
# Troque para "yes" para também incluir o renderer Vulkan (mobile).
vulkan = "no"
forward_plus_renderer = "no"
forward_mobile_renderer = "no"
opengl3 = "yes"

# Módulos que não servem para jogos 2D.
module_csg_enabled = "no"
module_gridmap_enabled = "no"
module_gltf_enabled = "no"
module_fbx_enabled = "no"
module_meshoptimizer_enabled = "no"
module_xatlas_unwrap_enabled = "no"
module_lightmapper_rd_enabled = "no"
module_vhacd_enabled = "no"
module_raycast_enabled = "no"
module_openxr_enabled = "no"
module_webxr_enabled = "no"
module_mobile_vr_enabled = "no"
module_visionos_xr_enabled = "no"
module_godot_physics_3d_enabled = "no"
module_jolt_physics_enabled = "no"
module_navigation_3d_enabled = "no"
module_upnp_enabled = "no"
module_camera_enabled = "no"
module_webrtc_enabled = "no"

module_kairo_enabled = "yes"
