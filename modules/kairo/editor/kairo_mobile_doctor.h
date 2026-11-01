#pragma once

#include "editor/docks/editor_dock.h"
#include "editor/plugins/editor_plugin.h"

class Button;
class Label;
class VBoxContainer;

// KAIRO Mobile Doctor: confere as configurações do projeto para mobile 2D,
// explica cada problema em linguagem simples e oferece correção com um toque.
class KairoMobileDoctor : public EditorDock {
	GDCLASS(KairoMobileDoctor, EditorDock);

public:
	enum Severity {
		SEVERITY_ERROR,
		SEVERITY_WARNING,
		SEVERITY_INFO,
	};

	enum Rule {
		RULE_IN_SET, // Aceita se o valor atual estiver em `ok_values`.
		RULE_NOT_IN_SET, // Aceita se o valor atual NÃO estiver em `ok_values`.
		RULE_RANGE_1_MAX, // Aceita se 1 <= valor <= `max_ok`.
	};

	struct Check {
		String key;
		Variant default_value; // Usado quando a configuração ainda não existe.
		Variant fix_value;
		Severity severity = SEVERITY_WARNING;
		Rule rule = RULE_IN_SET;
		Vector<Variant> ok_values;
		double max_ok = 0;
		String title;
		String why;
		bool needs_restart = false;
	};

private:
	Vector<Check> checks;

	VBoxContainer *list = nullptr;
	Label *summary = nullptr;
	Button *fix_all_button = nullptr;
	bool restart_hint = false;

	void _build_checks();
	bool _is_ok(const Check &p_check, const Variant &p_current) const;
	Variant _get_current(const Check &p_check) const;
	void _refresh();
	void _fix_one(int p_index);
	void _fix_all();
	Color _severity_color(Severity p_severity) const;

protected:
	void _notification(int p_what);

public:
	KairoMobileDoctor();
};

class KairoMobileDoctorPlugin : public EditorPlugin {
	GDCLASS(KairoMobileDoctorPlugin, EditorPlugin);

	KairoMobileDoctor *doctor = nullptr;

protected:
	void _notification(int p_what);

public:
	KairoMobileDoctorPlugin();
};
