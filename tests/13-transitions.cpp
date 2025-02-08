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
	State* parent1 = d.new_state(sm, "Parent 0");
	State* s1 = d.new_state(parent1, "State 0");
	State* s2 = d.new_state(parent1, "State 1");
	d.new_transition(sm, transitionExternal, s1, s2, Action());
	d.new_transition(sm, transitionExternal, s1, s2, Action("A"));
	try {
		// check id uniqueness
		d.new_transition(sm, transitionExternal, "n0::n0-n0::n1", s1, s2, Action());
	} catch (const Cyberiada::ParametersException&){
	}
	try {
		// check sm-related transition
		d.new_transition(sm, transitionExternal, sm, s1, Action());
	} catch (const Cyberiada::ParametersException&){
	}

	Polyline pl;
	pl.push_back(Point(0, 0));
	pl.push_back(Point(5, 10));
	pl.push_back(Point(15, 20));
	d.new_transition(sm, transitionExternal, s1, s1, Action("IDLE"), pl, Point(-1, -2), Point(3, 4));
	d.new_transition(sm, transitionExternal, parent1, s1, Action("INSIDE"), Polyline(), Point(-1, -2), Point(3, 4));
	d.new_transition(sm, transitionExternal, s2, parent1, Action("OUTSIDE"), Polyline(), Point(-1, -2), Point(3, 4), Point(5, 6));

	try {
		// check transition action
		d.new_transition(sm, transitionExternal, s1, s2, Action(actionEntry, "init();"));
	} catch (const Cyberiada::ParametersException&){
	}
	
	State* parent2 = d.new_state(sm, "Parent 1");
	d.new_transition(sm, transitionExternal, s2, parent2, Action("EVENT", "guard()", "action();"));
	
	try {
		cout << d << endl;
		LocalDocument ld(d, string(argv[0]) + ".graphml");
		ld.save();
	} catch (const Cyberiada::Exception&) {
		return 1;
	}
	return 0;
}
