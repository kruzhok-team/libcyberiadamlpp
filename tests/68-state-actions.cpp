/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The state actions test: set, clear and remove the actions of a state
 *
 * Copyright (C) 2024 Alexey Fedoseev <aleksey@fedoseev.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see https://www.gnu.org/licenses/
 * ----------------------------------------------------------------------------- */
#include <iostream>
#include "cyberiadamlpp.h"
#include "testutils.h"

using namespace Cyberiada;
using namespace std;

static void print_actions(const State* s)
{
	const vector<Action>& actions = s->get_actions();
	cout << "actions: " << actions.size() << endl;
	for (vector<Action>::const_iterator i = actions.begin(); i != actions.end(); i++) {
		cout << "  " << *i << endl;
	}
}

int main(int, char**)
{
	Document d;
	StateMachine* sm = d.new_state_machine("SM");
	InitialPseudostate* init = d.new_initial(sm);
	State* s = d.new_state(sm, "S");
	d.new_transition(sm, transitionExternal, init, s, Action());
	s->add_action(Action(actionEntry, "old()"));

	vector<Action> list;
	list.push_back(Action(actionEntry, "start()"));
	list.push_back(Action("KEY", "ready", "pressed()", eventPropagationPropagate));
	list.push_back(Action("LATER", "", "", eventPropagationDefer));
	list.push_back(Action(actionExit, "stop()"));
	s->set_actions(list);
	cout << "set" << endl;
	print_actions(s);

	vector<Action> refused = list;
	refused.push_back(Action());
	try {
		s->set_actions(refused);
		cout << "the empty transition action accepted" << endl;
	} catch (const ParametersException&) {
		cout << "the empty transition action refused" << endl;
	}
	CYB_ASSERT(s->get_actions().size() == list.size());

	s->remove_action(1);
	cout << "removed 1" << endl;
	print_actions(s);
	try {
		s->remove_action(9);
		cout << "index 9 accepted" << endl;
	} catch (const ParametersException&) {
		cout << "index 9 refused" << endl;
	}

	String buffer, format_str;
	DocumentFormat format = formatDetect;
	d.encode(buffer);
	Document copy;
	copy.decode(buffer, format, format_str, geometryFormatQt, false, false, false, false, false, true);
	const State* cs = static_cast<const State*>(copy.find_element_by_id(s->get_id()));
	CYB_ASSERT(cs);
	cout << "strict round trip: " << s->compare_actions(*cs) << endl;

	s->clear_actions();
	cout << "cleared: " << s->has_actions() << endl;
	return 0;
}
