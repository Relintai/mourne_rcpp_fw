#include "weather_controller.h"

#include "core/html/form_validator.h"
#include "core/html/html_builder.h"
#include "core/http/cookie.h"
#include "core/http/http_enums.h"
#include "core/http/http_session.h"
#include "core/http/request.h"
#include "core/http/session_manager.h"

#include "weather_model.h"

#include "../html_macros.h"

void WeatherController::handle_request_default(Request *request) {
}

void WeatherController::admin_handle_request_main(Request *request) {
	String seg = request->get_current_path_segment();

	if (seg == "") {
		admin_render_weather_list(request);
		return;
	} else if (seg == "new") {
		request->push_path();
		Ref<Weather> b;
		b.instance();

		admin_render_weather(request, b);
		return;
	} else if (seg == "edit") {
		request->push_path();

		String seg_weather_id = request->get_current_path_segment();

		if (!seg_weather_id.is_int()) {
			request->send_error(HTTP_STATUS_CODE_404_NOT_FOUND);
			return;
		}

		int bid = seg_weather_id.to_int();

		Ref<Weather> b = WeatherModel::get_singleton()->get_weather(bid);

		if (!b.is_valid()) {
			request->send_error(HTTP_STATUS_CODE_404_NOT_FOUND);
			return;
		}

		admin_render_weather(request, b);
		return;
	}

	request->send_error(404);
}
String WeatherController::admin_get_section_name() {
	return "Weathers";
}
void WeatherController::admin_add_section_links(Vector<AdminSectionLinkInfo> *links) {
	links->push_back(AdminSectionLinkInfo("- Weather Editor", ""));
}
bool WeatherController::admin_full_render() {
	return false;
}

void WeatherController::admin_render_weather_list(Request *request) {
	Vector<Ref<Weather> > weathers = WeatherModel::get_singleton()->get_all();

	HTMLBuilder b;

	b.div("back")->f()->fa(request->get_url_root_parent(), "<--- Back")->cdiv();
	b.br();
	b.fdiv("Weather Editor", "top_menu");
	b.br();
	b.div("top_menu")->f()->fa(request->get_url_root("new"), "Create New")->cdiv();
	b.br();

	b.div("list_container");

	for (int i = 0; i < weathers.size(); ++i) {
		Ref<Weather> weather = weathers[i];

		if (!weather.is_valid()) {
			continue;
		}

		if (i % 2 == 0) {
			b.div("row");
		} else {
			b.div("row second");
		}
		{
			b.fdiv(String::num(weather->id), "attr_box");
			b.fdiv(String::num(weather->rank), "attr_box");
			b.fdiv(String::num(weather->next_rank), "attr_box");
			b.fdiv(weather->name, "name");

			b.div("actionbox")->f()->fa(request->get_url_root("edit/" + String::num(weather->id)), "Edit")->cdiv();
		}
		b.cdiv();
	}

	b.cdiv();

	request->body += b.result;
}

void WeatherController::admin_render_weather(Request *request, Ref<Weather> weather) {
	if (!weather.is_valid()) {
		RLOG_ERR("admin_render_weather: !weather.is_valid()\n");
		request->send_error(HTTP_STATUS_CODE_500_INTERNAL_SERVER_ERROR);
		return;
	}

	Vector<Ref<Weather> > weathers = WeatherModel::get_singleton()->get_all();

	HTMLBuilder b;

	b.div("back")->f()->fa(request->get_url_root_parent(), "<--- Back")->cdiv();
	b.br();
	b.fdiv("Weather Editor", "top_menu");
	b.br();

	b.form_post(request->get_url_root());

	bool show_post = false; //request->get_method() == HTTP_METHOD_POST && validation errors;

	ADMIN_EDIT_INPUT_TEXT("Name:", "name", show_post, weather->name, request->get_parameter("name"));
	ADMIN_EDIT_INPUT_TEXTAREA("Description:", "description", show_post, weather->description, request->get_parameter("description"));

	b.div("row_edit");
	b.fdiv("Icon:", "edit_name");
	//todo I'm not sure yet how this worked originally
	//b.div("edit_input")->f()->input_image("icon", weather->icon)->f()->cdiv();
	b.div("edit_input")->f()->w("TODO")->cdiv();
	b.cdiv();

	ADMIN_EDIT_INPUT_TEXT("Rank:", "rank", show_post, String::num(weather->rank), request->get_parameter("rank"));

	Vector<Ref<Weather> > nrbs = WeatherModel::get_singleton()->get_all();
	b.div("row_edit");
	b.fdiv("Next Rank:", "edit_name");
	b.div("edit_input");
	{
		b.select("next_rank", "drop");
		{
			int current_id = weather->id;
			int current_nr = weather->next_rank;

			b.foption(String::num(0), "- None -", current_nr == 0);

			for (int i = 0; i < nrbs.size(); ++i) {
				Ref<Weather> build = nrbs[i];

				int id = build->id;

				if (id == current_id) {
					continue;
				}

				b.foption(String::num(id), build->name + " R" + String::num(build->rank), current_nr == id);
			}
		}
		b.cselect();
	}
	b.cdiv();
	b.cdiv();

	ADMIN_EDIT_INPUT_TEXT("Time to Build:", "time_to_build", show_post, String::num(weather->time_to_build), request->get_parameter("time_to_build"));

	ADMIN_EDIT_LINE_SPACER();

	ADMIN_EDIT_INPUT_TEXT("Score:", "score", show_post, String::num(weather->score), request->get_parameter("score"));
	ADMIN_EDIT_INPUT_TEXT("Defense:", "defense", show_post, String::num(weather->defense), request->get_parameter("defense"));

	//TODO
	/*
		int ability;

		<div class="row_edit">
		<div class="edit_name">
		Ability:
		</div>
		<div class="edit_input">
		<?=form_dropdown('ability', $opt_ability, $sability, $attr_drop); ?>
		</div>
		</div>
	*/

	b.div("row_edit");
	b.fdiv("Ability:", "edit_name");
	b.div("edit_input")->f()->w("TODO")->cdiv();
	b.cdiv();

	ADMIN_EDIT_LINE_SPACER();

	ADMIN_EDIT_INPUT_TEXT("Cost Food:", "cost_food", show_post, String::num(weather->cost_food), request->get_parameter("cost_food"));
	ADMIN_EDIT_INPUT_TEXT("Cost Wood:", "cost_wood", show_post, String::num(weather->cost_wood), request->get_parameter("cost_wood"));
	ADMIN_EDIT_INPUT_TEXT("Cost Stone:", "cost_stone", show_post, String::num(weather->cost_stone), request->get_parameter("cost_stone"));
	ADMIN_EDIT_INPUT_TEXT("Cost Iron:", "cost_iron", show_post, String::num(weather->cost_iron), request->get_parameter("cost_iron"));
	ADMIN_EDIT_INPUT_TEXT("Cost Mana:", "cost_mana", show_post, String::num(weather->cost_food), request->get_parameter("cost_mana"));

	ADMIN_EDIT_LINE_SPACER();

/*
	int creates;
	int num_creates;

	<div class="row_edit">
	<div class="edit_name">
	Creates:
	</div>
	<div class="edit_input">
	<?=form_dropdown($name_creates, $optcre, $screate, $attr_creates); ?>
	X (max) <?=form_input($attr_num_creates); ?>
	</div>
	</div>
*/

	b.div("row_edit");
	b.fdiv("Creates:", "edit_name");
	b.div("edit_input")->f()->w("TODO")->cdiv();
	b.cdiv();

	ADMIN_EDIT_LINE_SPACER();

	ADMIN_EDIT_INPUT_TEXT("Mod Max Food:", "mod_max_food", show_post, String::num(weather->mod_max_food), request->get_parameter("mod_max_food"));
	ADMIN_EDIT_INPUT_TEXT("Mod Max Wood:", "mod_max_wood", show_post, String::num(weather->mod_max_wood), request->get_parameter("mod_max_wood"));
	ADMIN_EDIT_INPUT_TEXT("Mod Max Stone:", "mod_max_stone", show_post, String::num(weather->mod_max_stone), request->get_parameter("mod_max_stone"));
	ADMIN_EDIT_INPUT_TEXT("Mod Max Iron:", "mod_max_iron", show_post, String::num(weather->mod_max_iron), request->get_parameter("mod_max_iron"));
	ADMIN_EDIT_INPUT_TEXT("Mod Max Mana:", "mod_max_mana", show_post, String::num(weather->mod_max_mana), request->get_parameter("mod_max_mana"));

	ADMIN_EDIT_LINE_SPACER();

	ADMIN_EDIT_INPUT_TEXT("Mod Rate Food:", "mod_rate_food", show_post, String::num(weather->mod_rate_food), request->get_parameter("mod_rate_food"));
	ADMIN_EDIT_INPUT_TEXT("Mod Rate Wood:", "mod_rate_wood", show_post, String::num(weather->mod_rate_wood), request->get_parameter("mod_rate_wood"));
	ADMIN_EDIT_INPUT_TEXT("Mod Rate Stone:", "mod_rate_stone", show_post, String::num(weather->mod_rate_stone), request->get_parameter("mod_rate_stone"));
	ADMIN_EDIT_INPUT_TEXT("Mod Rate Iron:", "mod_rate_iron", show_post, String::num(weather->mod_rate_iron), request->get_parameter("mod_rate_iron"));
	ADMIN_EDIT_INPUT_TEXT("Mod Rate Mana:", "mod_rate_mana", show_post, String::num(weather->mod_rate_mana), request->get_parameter("mod_rate_mana"));

	ADMIN_EDIT_LINE_SPACER();

	ADMIN_EDIT_INPUT_TEXT("Mod Percent Food:", "mod_percent_food", show_post, String::num(weather->mod_percent_food), request->get_parameter("mod_percent_food"));
	ADMIN_EDIT_INPUT_TEXT("Mod Percent Wood:", "mod_percent_wood", show_post, String::num(weather->mod_percent_wood), request->get_parameter("mod_percent_wood"));
	ADMIN_EDIT_INPUT_TEXT("Mod Percent Stone:", "mod_percent_stone", show_post, String::num(weather->mod_percent_stone), request->get_parameter("mod_percent_stone"));
	ADMIN_EDIT_INPUT_TEXT("Mod Percent Iron:", "mod_percent_iron", show_post, String::num(weather->mod_percent_iron), request->get_parameter("mod_percent_iron"));
	ADMIN_EDIT_INPUT_TEXT("Mod Percent Mana:", "mod_percent_mana", show_post, String::num(weather->mod_percent_mana), request->get_parameter("mod_percent_mana"));

	ADMIN_EDIT_LINE_SPACER();

	//TODO <?=form_dropdown($name_assign1, $optass, $assign1, $attr_assign); ?>

	ADMIN_EDIT_INPUT_TEXT("Assignment 1:", "assignment1", show_post, String::num(weather->assignment1), request->get_parameter("assignment1"));
	ADMIN_EDIT_INPUT_TEXT("Assignment 2:", "assignment2", show_post, String::num(weather->assignment2), request->get_parameter("assignment2"));
	ADMIN_EDIT_INPUT_TEXT("Assignment 3:", "assignment3", show_post, String::num(weather->assignment3), request->get_parameter("assignment3"));
	ADMIN_EDIT_INPUT_TEXT("Assignment 4:", "assignment4", show_post, String::num(weather->assignment4), request->get_parameter("assignment4"));
	ADMIN_EDIT_INPUT_TEXT("Assignment 5:", "assignment5", show_post, String::num(weather->assignment5), request->get_parameter("assignment5"));

	ADMIN_EDIT_LINE_SPACER();

	//TODO <?=form_dropdown($name_req_tech, $optreqtech, $selreqtech, $attr_req_tech); ?>
	ADMIN_EDIT_INPUT_TEXT("Required Technology:", "req_tech", show_post, String::num(weather->req_tech), request->get_parameter("req_tech"));
	ADMIN_EDIT_LINE_SPACER();
	//TODO <?=form_dropdown($name_tech_group, $opttechgroup, $seltechgroup, $attr_assign);?>
	ADMIN_EDIT_INPUT_TEXT("Technology Group:", "tech_group", show_post, String::num(weather->tech_group), request->get_parameter("tech_group"));
	ADMIN_EDIT_LINE_SPACER();
	//TODO <?=form_dropdown($name_tech_secondary_group, $opttechgroup, $seltechsecgroup, $attr_tech_secondary_group); ?>
	ADMIN_EDIT_INPUT_TEXT("Secondary Technology Group:", "tech_secondary_group", show_post, String::num(weather->tech_secondary_group), request->get_parameter("tech_secondary_group"));

	b.div("edit_submit")->f()->input_submit("Save", "submit")->f()->cdiv();

	b.cform();

	request->body += b.result;
}

void WeatherController::migrate() {
	WeatherModel::get_singleton()->migrate();
}
void WeatherController::add_default_data() {
	WeatherModel::get_singleton()->add_default_data();
}

WeatherController *WeatherController::get_singleton() {
	return _self;
}

WeatherController::WeatherController() :
		AdminController() {

	if (_self) {
		printf("WeatherController::WeatherController(): Error! self is not null!/n");
	}

	_self = this;
}

WeatherController::~WeatherController() {
	if (_self == this) {
		_self = nullptr;
	}
}

WeatherController *WeatherController::_self = nullptr;
