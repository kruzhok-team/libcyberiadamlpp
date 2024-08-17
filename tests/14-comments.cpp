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
	Comment* comm = d.new_comment(sm, "Top level");
	try {
		// check id uniqueness
		d.new_comment(sm, "n0", "testname", "Testbody");
	} catch (const Cyberiada::ParametersException&){
	}
	State* state = d.new_state(sm, "State", Action(actionEntry, "action();"));
	Comment* comm2 = d.new_comment(state, "Comment inside a state\nwith two lines");
	d.new_formal_comment(sm, "Name", "Named formal comment", Rect(0, 5, 100, 50));

	d.add_comment_to_element(comm, state);
	d.add_comment_to_element_name(comm2, state, "S", Point(-1, -2), Point(3, 4));
	Polyline pl;
	pl.push_back(Point(0, 0));
	pl.push_back(Point(5, 10));
	pl.push_back(Point(15, 20));
	d.add_comment_to_element_body(comm2, state, "action", Point(), Point(), pl);

	try {
		cout << d << endl;
		LocalDocument(d, string(argv[0]) + ".graphml").save();
	} catch (const Cyberiada::Exception&) {
		return 1;
	}
	return 0;
}
