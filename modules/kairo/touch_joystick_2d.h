#pragma once

#include "scene/2d/node_2d.h"

// Joystick virtual multitoque para jogos 2D mobile.
// - Modo FIXED: o stick fica sempre no mesmo lugar.
// - Modo DYNAMIC: o stick aparece onde o dedo tocar dentro de `touch_area_radius`.
// Pode injetar ações do Input Map (ex.: "move_left"/"move_right"/"move_up"/"move_down")
// e/ou ser lido por código via get_vector().
class TouchJoystick2D : public Node2D {
	GDCLASS(TouchJoystick2D, Node2D);

public:
	enum JoystickMode {
		MODE_FIXED,
		MODE_DYNAMIC,
	};

	enum VisibilityMode {
		VISIBILITY_ALWAYS,
		VISIBILITY_TOUCHSCREEN_ONLY,
	};

private:
	JoystickMode mode = MODE_FIXED;
	VisibilityMode visibility = VISIBILITY_ALWAYS;

	float base_radius = 90.0f;
	float knob_radius = 38.0f;
	float touch_area_radius = 160.0f;
	float deadzone = 0.15f;
	bool clamp_to_base = true;
	bool show_idle_hint = true;
	bool consume_input = true;
	Color base_color = Color(1, 1, 1, 0.20);
	Color knob_color = Color(1, 1, 1, 0.55);

	StringName action_left;
	StringName action_right;
	StringName action_up;
	StringName action_down;

	int finger = -1;
	Point2 origin_local; // Centro do stick enquanto pressionado (espaço local).
	Vector2 raw_vector; // Deslocamento normalizado bruto, comprimento <= 1.
	Vector2 output_vector; // Já com deadzone aplicada.

	virtual void input(const Ref<InputEvent> &p_event) override;

	bool _is_in_touch_area(const Point2 &p_global_canvas_point) const;
	void _consume_event();
	void _begin(int p_index, const Point2 &p_pos);
	void _update(const Point2 &p_pos);
	void _end(bool p_exiting = false);
	void _apply_actions(const Vector2 &p_old, const Vector2 &p_new);
	void _release_actions();

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	Vector2 get_vector() const;
	Vector2 get_raw_vector() const;
	float get_strength() const;
	bool is_pressed() const;

	void set_mode(JoystickMode p_mode);
	JoystickMode get_mode() const;
	void set_visibility_mode(VisibilityMode p_mode);
	VisibilityMode get_visibility_mode() const;

	void set_base_radius(float p_radius);
	float get_base_radius() const;
	void set_knob_radius(float p_radius);
	float get_knob_radius() const;
	void set_touch_area_radius(float p_radius);
	float get_touch_area_radius() const;
	void set_deadzone(float p_deadzone);
	float get_deadzone() const;
	void set_clamp_to_base(bool p_clamp);
	bool is_clamp_to_base() const;
	void set_show_idle_hint(bool p_show);
	bool is_show_idle_hint() const;
	void set_consume_input(bool p_consume);
	bool is_consume_input() const;
	void set_base_color(const Color &p_color);
	Color get_base_color() const;
	void set_knob_color(const Color &p_color);
	Color get_knob_color() const;

	void set_action_left(const StringName &p_action);
	StringName get_action_left() const;
	void set_action_right(const StringName &p_action);
	StringName get_action_right() const;
	void set_action_up(const StringName &p_action);
	StringName get_action_up() const;
	void set_action_down(const StringName &p_action);
	StringName get_action_down() const;
};

VARIANT_ENUM_CAST(TouchJoystick2D::JoystickMode);
VARIANT_ENUM_CAST(TouchJoystick2D::VisibilityMode);
