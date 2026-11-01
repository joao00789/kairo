#include "register_types.h"

#include "touch_joystick_2d.h"

#include "core/object/class_db.h"

#ifdef TOOLS_ENABLED
#include "editor/kairo_mobile_doctor.h"
#include "editor/plugins/editor_plugin.h"
#endif

void initialize_kairo_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		GDREGISTER_CLASS(TouchJoystick2D);
	}
#ifdef TOOLS_ENABLED
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		EditorPlugins::add_by_type<KairoMobileDoctorPlugin>();
	}
#endif
}

void uninitialize_kairo_module(ModuleInitializationLevel p_level) {
	// Nada a fazer.
}
