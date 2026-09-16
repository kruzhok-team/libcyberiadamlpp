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

int main(int, char** argv)
{
	Document d;

	StateMachine* sm = d.new_state_machine("SM");
	State* parent = d.new_state(sm, "State");
	// a history pseudostate belongs to a composite state's region
	d.new_shallow_history(parent, "Shallow", Point(10, 20));
	d.new_deep_history(parent, "Deep", Point(30, 40));

	try {
		// check id uniqueness and non-empty name
		d.new_shallow_history(parent, "n0", "name");
	} catch (const Cyberiada::ParametersException&){
	}

	string path = string(argv[0]) + ".graphml";
	try {
		cout << d << endl;
		LocalDocument(d, path).save();
		// the saved document reloads with the history tokens intact
		LocalDocument reloaded;
		reloaded.open(path);
		cout << reloaded << endl;
	} catch (const Cyberiada::Exception&) {
		return 1;
	}
	return 0;
}
