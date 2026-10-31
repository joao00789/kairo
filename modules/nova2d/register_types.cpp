#include "register_types.h"

#include "touch_joystick_2d.h"

#include "core/object/class_db.h"

void initialize_nova2d_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		GDREGISTER_CLASS(TouchJoystick2D);
	}
}

void uninitialize_nova2d_module(ModuleInitializationLevel p_level) {
	// Nada a fazer.
}
