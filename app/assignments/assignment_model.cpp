#include "assignment_model.h"

#include "core/database/database.h"
#include "core/database/database_manager.h"
#include "core/database/query_builder.h"
#include "core/database/query_result.h"
#include "core/database/table_builder.h"

#include "core/hash/sha256.h"

#include "assignment.h"

#define BUILDING_TABLE_NAME "assignments"

#define BUILDING_TABLE_COLUMNS "id, name, description, icon, rank, next_rank, time_to_build, creates, num_creates, score, defense, ability, cost_food, cost_wood, cost_stone, cost_iron, cost_mana, mod_max_food, mod_max_wood, mod_max_stone, mod_max_iron, mod_max_mana, mod_rate_food, mod_rate_wood, mod_rate_stone, mod_rate_iron, mod_rate_mana, mod_percent_food, mod_percent_wood, mod_percent_stone, mod_percent_iron, mod_percent_mana, assignment1, assignment2, assignment3, assignment4, assignment5, req_tech, tech_group, tech_secondary_group"
#define BUILDING_TABLE_COLUMNS_NOID "name, description, icon, rank, next_rank, time_to_build, creates, num_creates, score, defense, ability, cost_food, cost_wood, cost_stone, cost_iron, cost_mana, mod_max_food, mod_max_wood, mod_max_stone, mod_max_iron, mod_max_mana, mod_rate_food, mod_rate_wood, mod_rate_stone, mod_rate_iron, mod_rate_mana, mod_percent_food, mod_percent_wood, mod_percent_stone, mod_percent_iron, mod_percent_mana, assignment1, assignment2, assignment3, assignment4, assignment5, req_tech, tech_group, tech_secondary_group"

Ref<Assignment> AssignmentModel::get_assignment(const int id) {
	if (id == 0) {
		return Ref<Assignment>();
	}

	Ref<QueryBuilder> b = DatabaseManager::get_singleton()->ddb->get_query_builder();

	b->select(BUILDING_TABLE_COLUMNS);
	b->from(BUILDING_TABLE_NAME);

	b->where()->wp("id", id);

	b->end_command();

	Ref<QueryResult> r = b->run();

	if (!r->next_row()) {
		return Ref<Assignment>();
	}

	Ref<Assignment> assignment;
	assignment.instance();

	parse_row(r, assignment);

	return assignment;
}

Vector<Ref<Assignment> > AssignmentModel::get_all() {
	Ref<QueryBuilder> b = DatabaseManager::get_singleton()->ddb->get_query_builder();

	b->select(BUILDING_TABLE_COLUMNS);
	b->from(BUILDING_TABLE_NAME);
	b->end_command();
	//b->print();

	Vector<Ref<Assignment> > assignments;

	Ref<QueryResult> r = b->run();

	while (r->next_row()) {
		Ref<Assignment> assignment;
		assignment.instance();

		parse_row(r, assignment);

		assignments.push_back(assignment);
	}

	return assignments;
}

void AssignmentModel::save_assignment(Ref<Assignment> &assignment) {
	Ref<QueryBuilder> b = DatabaseManager::get_singleton()->ddb->get_query_builder();

	if (assignment->id == 0) {
		b->insert(BUILDING_TABLE_NAME, BUILDING_TABLE_COLUMNS_NOID);

		b->values();

		b->val(assignment->name);
		b->val(assignment->description);
		b->val(assignment->icon);

		b->val(assignment->rank);
		b->val(assignment->next_rank);
		b->val(assignment->time_to_build);
		b->val(assignment->creates);
		b->val(assignment->num_creates);
		b->val(assignment->score);
		b->val(assignment->defense);
		b->val(assignment->ability);

		b->val(assignment->cost_food);
		b->val(assignment->cost_wood);
		b->val(assignment->cost_stone);
		b->val(assignment->cost_iron);
		b->val(assignment->cost_mana);

		b->val(assignment->mod_max_food);
		b->val(assignment->mod_max_wood);
		b->val(assignment->mod_max_stone);
		b->val(assignment->mod_max_iron);
		b->val(assignment->mod_max_mana);

		b->vald(assignment->mod_rate_food);
		b->vald(assignment->mod_rate_wood);
		b->vald(assignment->mod_rate_stone);
		b->vald(assignment->mod_rate_iron);
		b->vald(assignment->mod_rate_mana);

		b->val(assignment->mod_percent_food);
		b->val(assignment->mod_percent_wood);
		b->val(assignment->mod_percent_stone);
		b->val(assignment->mod_percent_iron);
		b->val(assignment->mod_percent_mana);

		b->val(assignment->assignment1);
		b->val(assignment->assignment2);
		b->val(assignment->assignment3);
		b->val(assignment->assignment4);
		b->val(assignment->assignment5);

		b->val(assignment->req_tech);
		b->val(assignment->tech_group);
		b->val(assignment->tech_secondary_group);

		b->cvalues();

		b->end_command();
		b->select_last_insert_id();
		//b->print();

		Ref<QueryResult> r = b->run();

		assignment->id = r->get_last_insert_rowid();
	} else {
		b->update(BUILDING_TABLE_NAME);
		b->set();

		b->setp("name", assignment->name);
		b->setp("description", assignment->description);
		b->setp("icon", assignment->icon);

		b->setp("userankrname", assignment->rank);
		b->setp("next_rank", assignment->next_rank);
		b->setp("time_to_build", assignment->time_to_build);
		b->setp("creates", assignment->creates);
		b->setp("num_creates", assignment->num_creates);
		b->setp("score", assignment->score);
		b->setp("defense", assignment->defense);
		b->setp("ability", assignment->ability);

		b->setp("cost_food", assignment->cost_food);
		b->setp("cost_wood", assignment->cost_wood);
		b->setp("cost_stone", assignment->cost_stone);
		b->setp("cost_iron", assignment->cost_iron);
		b->setp("cost_mana", assignment->cost_mana);

		b->setp("mod_max_food", assignment->mod_max_food);
		b->setp("mod_max_wood", assignment->mod_max_wood);
		b->setp("mod_max_stone", assignment->mod_max_stone);
		b->setp("mod_max_iron", assignment->mod_max_iron);
		b->setp("mod_max_mana", assignment->mod_max_mana);

		b->setpd("mod_rate_food", assignment->mod_rate_food);
		b->setpd("mod_rate_wood", assignment->mod_rate_wood);
		b->setpd("mod_rate_stone", assignment->mod_rate_stone);
		b->setpd("mod_rate_iron", assignment->mod_rate_iron);
		b->setpd("mod_rate_mana", assignment->mod_rate_mana);

		b->setp("mod_percent_food", assignment->mod_percent_food);
		b->setp("mod_percent_wood", assignment->mod_percent_wood);
		b->setp("mod_percent_stone", assignment->mod_percent_stone);
		b->setp("mod_percent_iron", assignment->mod_percent_iron);
		b->setp("mod_percent_mana", assignment->mod_percent_mana);

		b->setp("assignment1", assignment->assignment1);
		b->setp("assignment2", assignment->assignment2);
		b->setp("assignment3", assignment->assignment3);
		b->setp("assignment4", assignment->assignment4);
		b->setp("assignment5", assignment->assignment5);

		b->setp("req_tech", assignment->req_tech);
		b->setp("tech_group", assignment->tech_group);
		b->setp("tech_secondary_group", assignment->tech_secondary_group);

		b->cset();
		b->where()->wp("id", assignment->id);

		//b->print();

		b->run_query();
	}
}

void AssignmentModel::parse_row(Ref<QueryResult> &result, Ref<Assignment> &assignment) {

	assignment->id = result->get_cell_int(0);

	assignment->name = result->get_cell(1);
	assignment->description = result->get_cell(2);
	assignment->icon = result->get_cell(3);

	assignment->rank = result->get_cell_int(4);
	assignment->next_rank = result->get_cell_int(5);
	assignment->time_to_build = result->get_cell_int(6);
	assignment->creates = result->get_cell_int(7);
	assignment->num_creates = result->get_cell_int(8);
	assignment->score = result->get_cell_int(9);
	assignment->defense = result->get_cell_int(10);
	assignment->ability = result->get_cell_int(11);

	assignment->cost_food = result->get_cell_int(12);
	assignment->cost_wood = result->get_cell_int(13);
	assignment->cost_stone = result->get_cell_int(14);
	assignment->cost_iron = result->get_cell_int(15);
	assignment->cost_mana = result->get_cell_int(16);

	assignment->mod_max_food = result->get_cell_int(17);
	assignment->mod_max_wood = result->get_cell_int(18);
	assignment->mod_max_stone = result->get_cell_int(19);
	assignment->mod_max_iron = result->get_cell_int(20);
	assignment->mod_max_mana = result->get_cell_int(21);

	assignment->mod_rate_food = result->get_cell_double(22);
	assignment->mod_rate_wood = result->get_cell_double(23);
	assignment->mod_rate_stone = result->get_cell_double(24);
	assignment->mod_rate_iron = result->get_cell_double(25);
	assignment->mod_rate_mana = result->get_cell_double(26);

	assignment->mod_percent_food = result->get_cell_int(27);
	assignment->mod_percent_wood = result->get_cell_int(28);
	assignment->mod_percent_stone = result->get_cell_int(29);
	assignment->mod_percent_iron = result->get_cell_int(30);
	assignment->mod_percent_mana = result->get_cell_int(31);

	assignment->assignment1 = result->get_cell_int(32);
	assignment->assignment2 = result->get_cell_int(33);
	assignment->assignment3 = result->get_cell_int(34);
	assignment->assignment4 = result->get_cell_int(35);
	assignment->assignment5 = result->get_cell_int(36);

	assignment->req_tech = result->get_cell_int(37);
	assignment->tech_group = result->get_cell_int(38);
	assignment->tech_secondary_group = result->get_cell_int(39);
}

void AssignmentModel::create_table() {
	Ref<TableBuilder> tb = DatabaseManager::get_singleton()->ddb->get_table_builder();

	tb->create_table(BUILDING_TABLE_NAME);
	tb->integer("id", 11)->auto_increment()->next_row();
	tb->varchar("name", 200)->not_null()->next_row();
	tb->varchar("description", 500)->not_null()->next_row();
	tb->varchar("icon", 500)->not_null()->next_row();
	tb->integer("rank", 11)->not_null()->next_row();
	tb->integer("next_rank", 11)->not_null()->next_row();
	tb->integer("time_to_build", 11)->not_null()->next_row();
	tb->integer("creates", 11)->not_null()->defval("0")->next_row();
	tb->integer("num_creates", 11)->not_null()->defval("0")->next_row();
	tb->integer("score", 11)->not_null()->next_row();
	tb->integer("defense", 11)->not_null()->next_row();
	tb->integer("ability", 11)->not_null()->defval("0")->next_row();

	tb->integer("cost_food", 11)->not_null()->next_row();
	tb->integer("cost_wood", 11)->not_null()->next_row();
	tb->integer("cost_stone", 11)->not_null()->next_row();
	tb->integer("cost_iron", 11)->not_null()->next_row();
	tb->integer("cost_mana", 11)->not_null()->next_row();

	tb->integer("mod_max_food", 11)->not_null()->defval("0")->next_row();
	tb->integer("mod_max_wood", 11)->not_null()->defval("0")->next_row();
	tb->integer("mod_max_stone", 11)->not_null()->defval("0")->next_row();
	tb->integer("mod_max_iron", 11)->not_null()->defval("0")->next_row();
	tb->integer("mod_max_mana", 11)->not_null()->defval("0")->next_row();

	tb->real_double("mod_rate_food")->not_null()->defval("0")->next_row();
	tb->real_double("mod_rate_wood")->not_null()->defval("0")->next_row();
	tb->real_double("mod_rate_stone")->not_null()->defval("0")->next_row();
	tb->real_double("mod_rate_iron")->not_null()->defval("0")->next_row();
	tb->real_double("mod_rate_mana")->not_null()->defval("0")->next_row();

	tb->integer("mod_percent_food", 11)->not_null()->defval("0")->next_row();
	tb->integer("mod_percent_wood", 11)->not_null()->defval("0")->next_row();
	tb->integer("mod_percent_stone", 11)->not_null()->defval("0")->next_row();
	tb->integer("mod_percent_iron", 11)->not_null()->defval("0")->next_row();
	tb->integer("mod_percent_mana", 11)->not_null()->defval("0")->next_row();

	tb->integer("assignment1", 11)->not_null()->defval("0")->next_row();
	tb->integer("assignment2", 11)->not_null()->defval("0")->next_row();
	tb->integer("assignment3", 11)->not_null()->defval("0")->next_row();
	tb->integer("assignment4", 11)->not_null()->defval("0")->next_row();
	tb->integer("assignment5", 11)->not_null()->defval("0")->next_row();

	tb->integer("req_tech", 11)->not_null()->defval("0")->next_row();
	tb->integer("tech_group", 11)->not_null()->defval("0")->next_row();
	tb->integer("tech_secondary_group", 11)->not_null()->defval("0")->next_row();

	tb->primary_key("id");
	tb->ccreate_table();

	tb->run_query();
	//tb->print();
}
void AssignmentModel::drop_table() {
	Ref<TableBuilder> tb = DatabaseManager::get_singleton()->ddb->get_table_builder();

	tb->drop_table_if_exists(BUILDING_TABLE_NAME)->cdrop_table();

	tb->run_query();
}
void AssignmentModel::migrate() {
	drop_table();
	create_table();
}

void AssignmentModel::add_default_data() {
	String table_columns = "id, name, description, icon, rank, next_rank, time_to_build, creates, num_creates, score, defense, ability, cost_food, cost_wood, cost_stone, cost_iron, cost_mana, mod_max_food, mod_max_wood, mod_max_stone, mod_max_iron, mod_max_mana, mod_rate_food, mod_rate_wood, mod_rate_stone, mod_rate_iron, mod_rate_mana, mod_percent_food, mod_percent_wood, mod_percent_stone, mod_percent_iron, mod_percent_mana, assignment1, assignment2, assignment3, assignment4, assignment5, req_tech, tech_group, tech_secondary_group";

	Ref<QueryBuilder> qb = DatabaseManager::get_singleton()->ddb->get_query_builder();

	qb->begin_transaction()->nl();
	qb->insert(BUILDING_TABLE_NAME, table_columns)->nl()->w("VALUES(1, 'empty', '', 'empty/empty.png', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)")->end_command()->nl();
	qb->insert(BUILDING_TABLE_NAME, table_columns)->nl()->w("VALUES(2, 'Build in Progress', '', 'bip/bip.png', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)")->end_command()->nl();
	qb->insert(BUILDING_TABLE_NAME, table_columns)->nl()->w("VALUES(3, 'Corn Field', 'Produces food.', 'corn_field/r1.png', 1, 7, 20, 0, 0, 20, 1, 0, 60, 100, 10, 5, 0, 0, 0, 0, 0, 0, 0.01, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 4, 3, 0, 0, 2, 3)")->end_command()->nl();
	qb->insert(BUILDING_TABLE_NAME, table_columns)->nl()->w("VALUES(4, 'Lumber Mill', 'Your main wood producing assignment.', 'lumber_mill/r1.png', 1, 0, 1000, 0, 0, 20, 0, 0, 30, 40, 50, 10, 0, 0, 0, 0, 0, 0, 0, 0.01, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 1, 2)")->end_command()->nl();
	qb->insert(BUILDING_TABLE_NAME, table_columns)->nl()->w("VALUES(5, 'Stone Mine', 'Your main stone producing assignment.', 'stone_mine/r1.png', 1, 0, 1000, 2, 20, 0, 0, 0, 30, 50, 20, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0.01, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)")->end_command()->nl();
	qb->insert(BUILDING_TABLE_NAME, table_columns)->nl()->w("VALUES(6, 'House', 'Can create villagers.', 'house/r1.png', 1, 0, 20, 1, 10, 0, 0, 0, 50, 70, 30, 5, 0, 0, 0, 0, 0, 0, -0.005, -0.001, -0.001, -0.001, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)")->end_command()->nl();
	qb->insert(BUILDING_TABLE_NAME, table_columns)->nl()->w("VALUES(7, 'Corn Field', '', 'corn_field/r2.png', 2, 0, 20, 0, 0, 0, 0, 0, 40, 60, 20, 10, 0, 0, 0, 0, 0, 0, 0.2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0, 2, 0, 0)")->end_command()->nl();
	qb->insert(BUILDING_TABLE_NAME, table_columns)->nl()->w("VALUES(8, 'Farm', 'Creates villagers.', 'farm/r1.png', 1, 0, 80, 1, 20, 0, 0, 0, 50, 60, 10, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)")->end_command()->nl();
	qb->insert(BUILDING_TABLE_NAME, table_columns)->nl()->w("VALUES(9, 'Iron Mine', 'Your main iron producing assignment.', 'iron_mine/r1.png', 1, 0, 1000, 2, 100000, 0, 0, 0, 70, 30, 70, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.01, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0)")->end_command()->nl();
	qb->insert(BUILDING_TABLE_NAME, table_columns)->nl()->w("VALUES(10, 'School', 'School', 'school/r1.png', 1, 0, 60, 2, 60, 0, 0, 0, 300, 300, 300, 300, 20, 0, 0, 0, 0, 0, 0.001, 0.001, 0.001, 0.001, 0.001, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 2)")->end_command()->nl();
	qb->commit();

	qb->run_query();
	//qb->print();
}

AssignmentModel *AssignmentModel::get_singleton() {
	return _self;
}

AssignmentModel::AssignmentModel() :
		Object() {

	if (_self) {
		printf("AssignmentModel::AssignmentModel(): Error! self is not null!/n");
	}

	_self = this;
}

AssignmentModel::~AssignmentModel() {
	if (_self == this) {
		_self = nullptr;
	}
}

AssignmentModel *AssignmentModel::_self = nullptr;
