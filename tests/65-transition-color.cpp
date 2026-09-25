/* -----------------------------------------------------------------------------
 * The Cyberiada State Machine Editor
 * The C++ library for CyberiadaML files
 *
 * The transition color is saved without the transition geometry
 *
 * Copyright (C) 2026 Alexey Fedoseev <aleksey@fedoseev.net>
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

int main(int, char** argv)
{
	Document d;
	StateMachine* sm = d.new_state_machine("SM");
	State* a = d.new_state(sm, "A");
	State* b = d.new_state(sm, "B");
	// a colored transition without any geometry keeps its color on saving
	Transition* t = d.new_transition(sm, transitionExternal, a, b, Action("GO"));
	t->set_color(Color("#00CC44"));
	string path = string(argv[0]) + ".graphml";
	try {
		LocalDocument(d, path).save();
		LocalDocument reloaded;
		reloaded.open(path);
		const Transition* rt = static_cast<const Transition*>(reloaded.find_element_by_id(t->get_id()));
		if (!rt || !rt->has_color() || rt->get_color() != Color("#00CC44")) {
			cerr << "the transition color is lost" << endl;
			return 1;
		}
		cout << reloaded << endl;
	} catch (const Cyberiada::Exception&) {
		return 1;
	}
	return 0;
}
