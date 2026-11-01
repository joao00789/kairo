#include "kairo_mobile_doctor.h"

#include "core/config/project_settings.h"
#include "core/object/callable_mp.h"
#include "editor/docks/editor_dock_manager.h"
#include "editor/editor_string_names.h"
#include "editor/settings/editor_command_palette.h"
#include "editor/themes/editor_scale.h"
#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/label.h"
#include "scene/gui/scroll_container.h"
#include "scene/gui/separator.h"

static String _t(const char *p_utf8) {
	return String::utf8(p_utf8);
}

void KairoMobileDoctor::_build_checks() {
	checks.clear();

	{
		Check c;
		c.key = "rendering/renderer/rendering_method";
		c.default_value = "forward_plus";
		c.fix_value = "gl_compatibility";
		c.severity = SEVERITY_ERROR;
		c.ok_values.push_back("gl_compatibility");
		c.needs_restart = true;
		c.title = _t("Renderizador (PC/geral) precisa ser Compatível");
		c.why = _t("O KAIRO é uma engine 2D e os templates de jogo vêm só com o renderizador Compatível (OpenGL ES 3), que roda em praticamente qualquer celular. Se o projeto pedir outro renderizador, o jogo exportado não abre.");
		checks.push_back(c);
	}
	{
		Check c;
		c.key = "rendering/renderer/rendering_method.mobile";
		c.default_value = "mobile";
		c.fix_value = "gl_compatibility";
		c.severity = SEVERITY_ERROR;
		c.ok_values.push_back("gl_compatibility");
		c.needs_restart = true;
		c.title = _t("Renderizador no celular precisa ser Compatível");
		c.why = _t("Mesma regra do item anterior, mas valendo só quando o jogo roda em Android/iOS. É essa configuração que o aparelho realmente usa.");
		checks.push_back(c);
	}
	{
		Check c;
		c.key = "rendering/textures/vram_compression/import_etc2_astc";
		c.default_value = false;
		c.fix_value = true;
		c.severity = SEVERITY_ERROR;
		c.ok_values.push_back(true);
		c.needs_restart = true;
		c.title = _t("Compressão de textura ETC2/ASTC desligada");
		c.why = _t("Celulares usam ETC2/ASTC para guardar imagens na memória de vídeo. Sem isso, a exportação para Android é bloqueada e as texturas ocupam muito mais memória.");
		checks.push_back(c);
	}
	{
		Check c;
		c.key = "display/window/stretch/mode";
		c.default_value = "disabled";
		c.fix_value = "canvas_items";
		c.severity = SEVERITY_WARNING;
		c.ok_values.push_back("canvas_items");
		c.ok_values.push_back("viewport");
		c.title = _t("A tela não escala para diferentes celulares");
		c.why = _t("Com o modo 'disabled', o jogo mostra pixels fixos: em telas grandes tudo fica minúsculo e em telas pequenas some. 'canvas_items' redimensiona o jogo mantendo a imagem nítida (use 'viewport' só para pixel art).");
		checks.push_back(c);
	}
	{
		Check c;
		c.key = "display/window/stretch/aspect";
		c.default_value = "keep";
		c.fix_value = "expand";
		c.severity = SEVERITY_WARNING;
		c.rule = RULE_NOT_IN_SET;
		c.ok_values.push_back("keep");
		c.title = _t("Faixas pretas em telas de formato diferente");
		c.why = _t("O modo 'keep' preserva a proporção do jogo e coloca barras pretas nos celulares com tela mais comprida (a maioria hoje). 'expand' aproveita a tela toda; posicione a interface com âncoras.");
		checks.push_back(c);
	}
	{
		Check c;
		c.key = "application/run/max_fps";
		c.default_value = 0;
		c.fix_value = 60;
		c.severity = SEVERITY_INFO;
		c.rule = RULE_RANGE_1_MAX;
		c.max_ok = 120;
		c.title = _t("FPS sem limite (gasta bateria)");
		c.why = _t("Com FPS em 0, o jogo roda o mais rápido que o aparelho aguentar, esquentando o celular e gastando bateria à toa. 60 é um bom padrão para a maioria dos jogos 2D.");
		checks.push_back(c);
	}
	{
		Check c;
		c.key = "physics/common/physics_ticks_per_second";
		c.default_value = 60;
		c.fix_value = 60;
		c.severity = SEVERITY_INFO;
		c.rule = RULE_RANGE_1_MAX;
		c.max_ok = 60;
		c.title = _t("Física rodando mais de 60 vezes por segundo");
		c.why = _t("Cada tick extra de física custa processamento. Em celulares de entrada isso pode causar quedas de FPS. 60 costuma ser suficiente para jogos 2D.");
		checks.push_back(c);
	}
	{
		Check c;
		c.key = "input_devices/pointing/emulate_touch_from_mouse";
		c.default_value = false;
		c.fix_value = true;
		c.severity = SEVERITY_INFO;
		c.ok_values.push_back(true);
		c.title = _t("Não dá para testar o toque com o mouse no PC");
		c.why = _t("Ligando esta opção, o clique do mouse vira um toque na tela. Assim você testa joysticks e botões de toque direto no computador, sem exportar para o celular a cada mudança.");
		checks.push_back(c);
	}
}

Variant KairoMobileDoctor::_get_current(const Check &p_check) const {
	ProjectSettings *ps = ProjectSettings::get_singleton();
	if (ps->has_setting(p_check.key)) {
		return ps->get_setting(p_check.key);
	}
	return p_check.default_value;
}

bool KairoMobileDoctor::_is_ok(const Check &p_check, const Variant &p_current) const {
	switch (p_check.rule) {
		case RULE_IN_SET: {
			for (const Variant &v : p_check.ok_values) {
				if (v.get_type() == Variant::STRING) {
					if (String(p_current) == String(v)) {
						return true;
					}
				} else if (p_current == v) {
					return true;
				}
			}
			return false;
		}
		case RULE_NOT_IN_SET: {
			for (const Variant &v : p_check.ok_values) {
				if (String(p_current) == String(v)) {
					return false;
				}
			}
			return true;
		}
		case RULE_RANGE_1_MAX: {
			const double value = p_current;
			return value >= 1.0 && value <= p_check.max_ok;
		}
	}
	return true;
}

Color KairoMobileDoctor::_severity_color(Severity p_severity) const {
	switch (p_severity) {
		case SEVERITY_ERROR:
			return get_theme_color(SNAME("error_color"), EditorStringName(Editor));
		case SEVERITY_WARNING:
			return get_theme_color(SNAME("warning_color"), EditorStringName(Editor));
		case SEVERITY_INFO:
			return get_theme_color(SNAME("font_color"), EditorStringName(Editor));
	}
	return Color(1, 1, 1);
}

void KairoMobileDoctor::_refresh() {
	// Limpa a lista anterior.
	while (list->get_child_count() > 0) {
		Node *child = list->get_child(0);
		list->remove_child(child);
		child->queue_free();
	}

	int problems = 0;
	const Color ok_color = get_theme_color(SNAME("success_color"), EditorStringName(Editor));

	for (int i = 0; i < checks.size(); i++) {
		const Check &c = checks[i];
		const bool ok = _is_ok(c, _get_current(c));
		if (!ok) {
			problems++;
		}

		HBoxContainer *row = memnew(HBoxContainer);
		list->add_child(row);

		Label *mark = memnew(Label);
		mark->set_text(ok ? _t("✔") : _t("●"));
		mark->add_theme_color_override(SNAME("font_color"), ok ? ok_color : _severity_color(c.severity));
		row->add_child(mark);

		Label *row_title = memnew(Label);
		row_title->set_text(c.title);
		row_title->set_h_size_flags(Control::SIZE_EXPAND_FILL);
		row_title->set_autowrap_mode(TextServer::AUTOWRAP_WORD_SMART);
		row_title->set_custom_minimum_size(Size2(80, 0) * EDSCALE);
		if (ok) {
			row_title->set_modulate(Color(1, 1, 1, 0.55));
		}
		row->add_child(row_title);

		if (!ok) {
			Button *fix = memnew(Button);
			fix->set_text(_t("Corrigir"));
			fix->set_tooltip_text(_t("Aplica o valor recomendado em Configurações do Projeto."));
			fix->connect(SceneStringName(pressed), callable_mp(this, &KairoMobileDoctor::_fix_one).bind(i));
			row->add_child(fix);

			Label *why = memnew(Label);
			why->set_text(_t("Por quê: ") + c.why);
			why->set_autowrap_mode(TextServer::AUTOWRAP_WORD_SMART);
			why->set_custom_minimum_size(Size2(80, 0) * EDSCALE);
			why->set_modulate(Color(1, 1, 1, 0.75));
			list->add_child(why);
		}

		list->add_child(memnew(HSeparator));
	}

	if (problems == 0) {
		summary->set_text(_t("Tudo certo! Seu projeto está pronto para mobile 2D."));
		summary->add_theme_color_override(SNAME("font_color"), ok_color);
	} else {
		summary->set_text(vformat(_t("%d item(ns) para ajustar."), problems));
		summary->remove_theme_color_override(SNAME("font_color"));
	}
	if (restart_hint) {
		summary->set_text(summary->get_text() + _t("\nAlgumas mudanças só têm efeito depois de reiniciar o editor."));
	}
	fix_all_button->set_disabled(problems == 0);
}

void KairoMobileDoctor::_fix_one(int p_index) {
	ERR_FAIL_INDEX(p_index, checks.size());
	const Check &c = checks[p_index];
	ProjectSettings *ps = ProjectSettings::get_singleton();
	ps->set_setting(c.key, c.fix_value);
	if (c.needs_restart) {
		restart_hint = true;
	}
	ps->save();
	_refresh();
}

void KairoMobileDoctor::_fix_all() {
	ProjectSettings *ps = ProjectSettings::get_singleton();
	for (const Check &c : checks) {
		if (!_is_ok(c, _get_current(c))) {
			ps->set_setting(c.key, c.fix_value);
			if (c.needs_restart) {
				restart_hint = true;
			}
		}
	}
	ps->save();
	_refresh();
}

void KairoMobileDoctor::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_VISIBILITY_CHANGED: {
			if (is_visible_in_tree()) {
				_refresh();
			}
		} break;

		case NOTIFICATION_THEME_CHANGED: {
			if (list && is_visible_in_tree()) {
				_refresh();
			}
		} break;
	}
}

KairoMobileDoctor::KairoMobileDoctor() {
	set_name(TTRC("KAIRO Mobile"));
	set_icon_name("Tools");
	set_dock_shortcut(ED_SHORTCUT_AND_COMMAND("docks/open_kairo_mobile", TTRC("Open KAIRO Mobile Dock")));
	set_default_slot(EditorDock::DOCK_SLOT_RIGHT_BL);

	_build_checks();

	VBoxContainer *root = memnew(VBoxContainer);
	root->set_v_size_flags(SIZE_EXPAND_FILL);
	add_child(root);

	Label *intro = memnew(Label);
	intro->set_text(_t("Confere se o projeto está bem configurado para celular. Cada item explica o motivo e pode ser corrigido com um toque."));
	intro->set_autowrap_mode(TextServer::AUTOWRAP_WORD_SMART);
	intro->set_custom_minimum_size(Size2(80, 0) * EDSCALE);
	root->add_child(intro);

	summary = memnew(Label);
	summary->set_autowrap_mode(TextServer::AUTOWRAP_WORD_SMART);
	summary->set_custom_minimum_size(Size2(80, 0) * EDSCALE);
	root->add_child(summary);

	HBoxContainer *buttons = memnew(HBoxContainer);
	root->add_child(buttons);

	Button *recheck = memnew(Button);
	recheck->set_text(_t("Verificar de novo"));
	recheck->set_h_size_flags(SIZE_EXPAND_FILL);
	recheck->connect(SceneStringName(pressed), callable_mp(this, &KairoMobileDoctor::_refresh));
	buttons->add_child(recheck);

	fix_all_button = memnew(Button);
	fix_all_button->set_text(_t("Corrigir tudo"));
	fix_all_button->set_h_size_flags(SIZE_EXPAND_FILL);
	fix_all_button->connect(SceneStringName(pressed), callable_mp(this, &KairoMobileDoctor::_fix_all));
	buttons->add_child(fix_all_button);

	root->add_child(memnew(HSeparator));

	ScrollContainer *scroll = memnew(ScrollContainer);
	scroll->set_v_size_flags(SIZE_EXPAND_FILL);
	scroll->set_horizontal_scroll_mode(ScrollContainer::SCROLL_MODE_DISABLED);
	root->add_child(scroll);

	list = memnew(VBoxContainer);
	list->set_h_size_flags(SIZE_EXPAND_FILL);
	scroll->add_child(list);
}

void KairoMobileDoctorPlugin::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_EXIT_TREE: {
			if (doctor) {
				EditorDockManager::get_singleton()->remove_dock(doctor);
				memdelete(doctor);
				doctor = nullptr;
			}
		} break;
	}
}

KairoMobileDoctorPlugin::KairoMobileDoctorPlugin() {
	doctor = memnew(KairoMobileDoctor);
	EditorDockManager::get_singleton()->add_dock(doctor);
}
