/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The test
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

int main(int argc, char** argv)
{
	Document d;
	StateMachine* sm = d.new_state_machine("SM");
	State* s1 = d.new_state(sm, "State 1");
	try {
		// check empty action
		s1->add_action(Action());
	} catch (const Cyberiada::ParametersException&){
	}
	s1->add_action(Action(actionEntry));
	s1->add_action(Action(actionExit, "exit();"));

	State* s2 = d.new_state(s1, "State 2");
	s2->add_action(Action("EVENT", "is_guard()", "action();"));
	s2->add_action(Action("EVENT(b)", "is_guard() && is_second()", "action1();\naction2();"));
	s2->add_action(Action("EVENT", "else"));
	s2->add_action(Action(actionEntry, "init();"));
	try {
		cout << d << endl;
		LocalDocument(d, string(argv[0]) + ".graphml").save();

		ActionsDiffFlags f = s1->compare_actions(*s2);
		CYB_ASSERT(!(f & adiffArguments));
		CYB_ASSERT(!(f & adiffOrder));
		CYB_ASSERT(!(f & adiffGuards));
		CYB_ASSERT(f & adiffActions);
		CYB_ASSERT(f & adiffNumber);
		CYB_ASSERT(f & adiffTypes);
	} catch (const Cyberiada::Exception&) {
		return 1;
	}

	
	return 0;
}
