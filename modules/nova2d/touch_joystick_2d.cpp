#include "touch_joystick_2d.h"

#include "core/config/engine.h"
#include "core/input/input.h"
#include "core/input/input_event.h"
#include "core/object/class_db.h"
#include "servers/display/display_server.h"

void TouchJoystick2D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_DRAW: {
			if (!Engine::get_singleton()->is_editor_hint() && visibility == VISIBILITY_TOUCHSCREEN_ONLY && !DisplayServer::get_singleton()->is_touchscreen_available()) {
				return;
			}

			// Em modo dinâmico, só desenha enquanto o dedo está na tela (ou no editor).
			const bool editor = Engine::get_singleton()->is_editor_hint();
			if (mode == MODE_DYNAMIC && finger == -1 && !editor) {
				return;
			}

			const Point2 center = (finger != -1) ? origin_local : Point2();
			draw_circle(center, base_radius, base_color);
			draw_arc(center, base_radius, 0, Math::TAU, 48, base_color.lightened(0.3), 2.0, true);

			const Point2 knob_pos = center + raw_vector * base_radius;
			draw_circle(knob_pos, knob_radius, knob_color);
		} break;

		case NOTIFICATION_ENTER_TREE: {
			if (!Engine::get_singleton()->is_editor_hint()) {
				set_process_input(is_visible_in_tree());
			}
			queue_redraw();
		} break;

		case NOTIFICATION_EXIT_TREE: {
			if (finger != -1) {
				_end(true);
			}
		} break;

		case NOTIFICATION_VISIBILITY_CHANGED: {
			if (Engine::get_singleton()->is_editor_hint()) {
				break;
			}
			if (is_visible_in_tree()) {
				set_process_input(true);
			} else {
				set_process_input(false);
				if (finger != -1) {
					_end();
				}
			}
		} break;

		case NOTIFICATION_SUSPENDED:
		case NOTIFICATION_PAUSED: {
			if (finger != -1) {
				_end();
			}
		} break;
	}
}

bool TouchJoystick2D::_is_in_touch_area(const Point2 &p_canvas_point) const {
	const Point2 local = get_global_transform_with_canvas().affine_inverse().xform(p_canvas_point);
	const float radius = (mode == MODE_DYNAMIC) ? touch_area_radius : base_radius;
	return local.length_squared() <= radius * radius;
}

void TouchJoystick2D::input(const Ref<InputEvent> &p_event) {
	ERR_FAIL_COND(p_event.is_null());

	if (!is_visible_in_tree()) {
		return;
	}

	const InputEventScreenTouch *st = Object::cast_to<InputEventScreenTouch>(*p_event);
	const InputEventScreenDrag *sd = Object::cast_to<InputEventScreenDrag>(*p_event);

	if (st) {
		if (st->is_pressed()) {
			if (finger == -1 && _is_in_touch_area(st->get_position())) {
				_begin(st->get_index(), st->get_position());
			}
		} else if (st->get_index() == finger) {
			_end();
		}
	} else if (sd && sd->get_index() == finger) {
		_update(sd->get_position());
	}
}

void TouchJoystick2D::_begin(int p_index, const Point2 &p_pos) {
	finger = p_index;
	const Transform2D inv = get_global_transform_with_canvas().affine_inverse();
	origin_local = (mode == MODE_DYNAMIC) ? inv.xform(p_pos) : Point2();
	raw_vector = Vector2();
	output_vector = Vector2();
	emit_signal(SNAME("pressed"));
	_update(p_pos);
}

void TouchJoystick2D::_update(const Point2 &p_pos) {
	const Point2 local = get_global_transform_with_canvas().affine_inverse().xform(p_pos);
	Vector2 delta = (local - origin_local) / MAX(base_radius, 1.0f);

	if (clamp_to_base && delta.length() > 1.0f) {
		delta = delta.normalized();
	}
	raw_vector = delta;

	// Deadzone radial com reescala: a saída vai de 0 a 1 depois da zona morta.
	const float len = raw_vector.length();
	Vector2 out;
	if (len > deadzone) {
		const float scaled = CLAMP((len - deadzone) / MAX(1.0f - deadzone, 0.0001f), 0.0f, 1.0f);
		out = raw_vector / len * scaled;
	}

	const Vector2 old = output_vector;
	output_vector = out;
	_apply_actions(old, output_vector);

	if (!old.is_equal_approx(output_vector)) {
		emit_signal(SNAME("vector_changed"), output_vector);
	}
	queue_redraw();
}

void TouchJoystick2D::_end(bool p_exiting) {
	const Vector2 old = output_vector;
	finger = -1;
	raw_vector = Vector2();
	output_vector = Vector2();
	_release_actions();

	if (!p_exiting) {
		if (!old.is_zero_approx()) {
			emit_signal(SNAME("vector_changed"), Vector2());
		}
		emit_signal(SNAME("released"));
		queue_redraw();
	}
}

void TouchJoystick2D::_apply_actions(const Vector2 &p_old, const Vector2 &p_new) {
	Input *in = Input::get_singleton();
	if (!in) {
		return;
	}

	// Eixo X
	if (p_new.x < 0) {
		if (!action_right.is_empty() && p_old.x > 0) {
			in->action_release(action_right);
		}
		if (!action_left.is_empty()) {
			in->action_press(action_left, -p_new.x);
		}
	} else if (p_new.x > 0) {
		if (!action_left.is_empty() && p_old.x < 0) {
			in->action_release(action_left);
		}
		if (!action_right.is_empty()) {
			in->action_press(action_right, p_new.x);
		}
	} else {
		if (!action_left.is_empty()) {
			in->action_release(action_left);
		}
		if (!action_right.is_empty()) {
			in->action_release(action_right);
		}
	}

	// Eixo Y (Y negativo = cima, como no resto do Godot)
	if (p_new.y < 0) {
		if (!action_down.is_empty() && p_old.y > 0) {
			in->action_release(action_down);
		}
		if (!action_up.is_empty()) {
			in->action_press(action_up, -p_new.y);
		}
	} else if (p_new.y > 0) {
		if (!action_up.is_empty() && p_old.y < 0) {
			in->action_release(action_up);
		}
		if (!action_down.is_empty()) {
			in->action_press(action_down, p_new.y);
		}
	} else {
		if (!action_up.is_empty()) {
			in->action_release(action_up);
		}
		if (!action_down.is_empty()) {
			in->action_release(action_down);
		}
	}
}

void TouchJoystick2D::_release_actions() {
	Input *in = Input::get_singleton();
	if (!in) {
		return;
	}
	if (!action_left.is_empty()) {
		in->action_release(action_left);
	}
	if (!action_right.is_empty()) {
		in->action_release(action_right);
	}
	if (!action_up.is_empty()) {
		in->action_release(action_up);
	}
	if (!action_down.is_empty()) {
		in->action_release(action_down);
	}
}

Vector2 TouchJoystick2D::get_vector() const {
	return output_vector;
}

Vector2 TouchJoystick2D::get_raw_vector() const {
	return raw_vector;
}

float TouchJoystick2D::get_strength() const {
	return output_vector.length();
}

bool TouchJoystick2D::is_pressed() const {
	return finger != -1;
}

void TouchJoystick2D::set_mode(JoystickMode p_mode) {
	mode = p_mode;
	queue_redraw();
}

TouchJoystick2D::JoystickMode TouchJoystick2D::get_mode() const {
	return mode;
}

void TouchJoystick2D::set_visibility_mode(VisibilityMode p_mode) {
	visibility = p_mode;
	queue_redraw();
}

TouchJoystick2D::VisibilityMode TouchJoystick2D::get_visibility_mode() const {
	return visibility;
}

void TouchJoystick2D::set_base_radius(float p_radius) {
	base_radius = MAX(p_radius, 1.0f);
	queue_redraw();
}

float TouchJoystick2D::get_base_radius() const {
	return base_radius;
}

void TouchJoystick2D::set_knob_radius(float p_radius) {
	knob_radius = MAX(p_radius, 1.0f);
	queue_redraw();
}

float TouchJoystick2D::get_knob_radius() const {
	return knob_radius;
}

void TouchJoystick2D::set_touch_area_radius(float p_radius) {
	touch_area_radius = MAX(p_radius, 1.0f);
	queue_redraw();
}

float TouchJoystick2D::get_touch_area_radius() const {
	return touch_area_radius;
}

void TouchJoystick2D::set_deadzone(float p_deadzone) {
	deadzone = CLAMP(p_deadzone, 0.0f, 0.95f);
}

float TouchJoystick2D::get_deadzone() const {
	return deadzone;
}

void TouchJoystick2D::set_clamp_to_base(bool p_clamp) {
	clamp_to_base = p_clamp;
}

bool TouchJoystick2D::is_clamp_to_base() const {
	return clamp_to_base;
}

void TouchJoystick2D::set_base_color(const Color &p_color) {
	base_color = p_color;
	queue_redraw();
}

Color TouchJoystick2D::get_base_color() const {
	return base_color;
}

void TouchJoystick2D::set_knob_color(const Color &p_color) {
	knob_color = p_color;
	queue_redraw();
}

Color TouchJoystick2D::get_knob_color() const {
	return knob_color;
}

void TouchJoystick2D::set_action_left(const StringName &p_action) {
	action_left = p_action;
}

StringName TouchJoystick2D::get_action_left() const {
	return action_left;
}

void TouchJoystick2D::set_action_right(const StringName &p_action) {
	action_right = p_action;
}

StringName TouchJoystick2D::get_action_right() const {
	return action_right;
}

void TouchJoystick2D::set_action_up(const StringName &p_action) {
	action_up = p_action;
}

StringName TouchJoystick2D::get_action_up() const {
	return action_up;
}

void TouchJoystick2D::set_action_down(const StringName &p_action) {
	action_down = p_action;
}

StringName TouchJoystick2D::get_action_down() const {
	return action_down;
}

void TouchJoystick2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_vector"), &TouchJoystick2D::get_vector);
	ClassDB::bind_method(D_METHOD("get_raw_vector"), &TouchJoystick2D::get_raw_vector);
	ClassDB::bind_method(D_METHOD("get_strength"), &TouchJoystick2D::get_strength);
	ClassDB::bind_method(D_METHOD("is_pressed"), &TouchJoystick2D::is_pressed);

	ClassDB::bind_method(D_METHOD("set_mode", "mode"), &TouchJoystick2D::set_mode);
	ClassDB::bind_method(D_METHOD("get_mode"), &TouchJoystick2D::get_mode);
	ClassDB::bind_method(D_METHOD("set_visibility_mode", "mode"), &TouchJoystick2D::set_visibility_mode);
	ClassDB::bind_method(D_METHOD("get_visibility_mode"), &TouchJoystick2D::get_visibility_mode);

	ClassDB::bind_method(D_METHOD("set_base_radius", "radius"), &TouchJoystick2D::set_base_radius);
	ClassDB::bind_method(D_METHOD("get_base_radius"), &TouchJoystick2D::get_base_radius);
	ClassDB::bind_method(D_METHOD("set_knob_radius", "radius"), &TouchJoystick2D::set_knob_radius);
	ClassDB::bind_method(D_METHOD("get_knob_radius"), &TouchJoystick2D::get_knob_radius);
	ClassDB::bind_method(D_METHOD("set_touch_area_radius", "radius"), &TouchJoystick2D::set_touch_area_radius);
	ClassDB::bind_method(D_METHOD("get_touch_area_radius"), &TouchJoystick2D::get_touch_area_radius);
	ClassDB::bind_method(D_METHOD("set_deadzone", "deadzone"), &TouchJoystick2D::set_deadzone);
	ClassDB::bind_method(D_METHOD("get_deadzone"), &TouchJoystick2D::get_deadzone);
	ClassDB::bind_method(D_METHOD("set_clamp_to_base", "enabled"), &TouchJoystick2D::set_clamp_to_base);
	ClassDB::bind_method(D_METHOD("is_clamp_to_base"), &TouchJoystick2D::is_clamp_to_base);
	ClassDB::bind_method(D_METHOD("set_base_color", "color"), &TouchJoystick2D::set_base_color);
	ClassDB::bind_method(D_METHOD("get_base_color"), &TouchJoystick2D::get_base_color);
	ClassDB::bind_method(D_METHOD("set_knob_color", "color"), &TouchJoystick2D::set_knob_color);
	ClassDB::bind_method(D_METHOD("get_knob_color"), &TouchJoystick2D::get_knob_color);

	ClassDB::bind_method(D_METHOD("set_action_left", "action"), &TouchJoystick2D::set_action_left);
	ClassDB::bind_method(D_METHOD("get_action_left"), &TouchJoystick2D::get_action_left);
	ClassDB::bind_method(D_METHOD("set_action_right", "action"), &TouchJoystick2D::set_action_right);
	ClassDB::bind_method(D_METHOD("get_action_right"), &TouchJoystick2D::get_action_right);
	ClassDB::bind_method(D_METHOD("set_action_up", "action"), &TouchJoystick2D::set_action_up);
	ClassDB::bind_method(D_METHOD("get_action_up"), &TouchJoystick2D::get_action_up);
	ClassDB::bind_method(D_METHOD("set_action_down", "action"), &TouchJoystick2D::set_action_down);
	ClassDB::bind_method(D_METHOD("get_action_down"), &TouchJoystick2D::get_action_down);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "mode", PROPERTY_HINT_ENUM, "Fixed,Dynamic"), "set_mode", "get_mode");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "visibility_mode", PROPERTY_HINT_ENUM, "Always,TouchScreen Only"), "set_visibility_mode", "get_visibility_mode");

	ADD_GROUP("Shape", "");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "base_radius", PROPERTY_HINT_RANGE, "8,512,0.5,suffix:px"), "set_base_radius", "get_base_radius");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "knob_radius", PROPERTY_HINT_RANGE, "4,256,0.5,suffix:px"), "set_knob_radius", "get_knob_radius");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "touch_area_radius", PROPERTY_HINT_RANGE, "8,1024,0.5,suffix:px"), "set_touch_area_radius", "get_touch_area_radius");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "base_color"), "set_base_color", "get_base_color");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "knob_color"), "set_knob_color", "get_knob_color");

	ADD_GROUP("Behavior", "");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "deadzone", PROPERTY_HINT_RANGE, "0,0.95,0.01"), "set_deadzone", "get_deadzone");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "clamp_to_base"), "set_clamp_to_base", "is_clamp_to_base");

	ADD_GROUP("Input Actions", "action_");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "action_left"), "set_action_left", "get_action_left");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "action_right"), "set_action_right", "get_action_right");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "action_up"), "set_action_up", "get_action_up");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "action_down"), "set_action_down", "get_action_down");

	ADD_SIGNAL(MethodInfo("pressed"));
	ADD_SIGNAL(MethodInfo("released"));
	ADD_SIGNAL(MethodInfo("vector_changed", PropertyInfo(Variant::VECTOR2, "vector")));

	BIND_ENUM_CONSTANT(MODE_FIXED);
	BIND_ENUM_CONSTANT(MODE_DYNAMIC);
	BIND_ENUM_CONSTANT(VISIBILITY_ALWAYS);
	BIND_ENUM_CONSTANT(VISIBILITY_TOUCHSCREEN_ONLY);
}
