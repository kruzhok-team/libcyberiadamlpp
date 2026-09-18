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

	// the referenced machine
	StateMachine* worker = d.new_state_machine("Worker");
	d.new_state(worker, "Run");

	// the caller with a submachine state and connection points (8.1)
	StateMachine* sm = d.new_state_machine("Caller");
	d.new_state(sm, "After");
	// entry/exit points standalone at the SM level (8.3.2)
	d.new_entry(sm, "SM in", Point(5, 5));
	d.new_exit(sm, "SM out", Point(15, 15));
	SubmachineState* sub = d.new_submachine_state(sm, worker->get_id(), "Nested", Rect(0, 0, 80, 40));
	// the connection points bound to the referenced machine, on the state border
	d.new_entry(sub, "Start", Point(-40, 0));
	d.new_exit(sub, "Done", Point(40, 0));

	string path = string(argv[0]) + ".graphml";
	try {
		cout << d << endl;
		LocalDocument(d, path).save();
		// the submachine reference and its connection points reload intact
		LocalDocument reloaded;
		reloaded.open(path);
		cout << reloaded << endl;
	} catch (const Cyberiada::Exception&) {
		return 1;
	}
	return 0;
}
