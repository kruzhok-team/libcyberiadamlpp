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
	d.new_state(sm, "A", Action(), Rect(0, 50, 100, 25));
	State* s = d.new_state(sm, "B", Action(), Rect(-100, -250, 100, 200));
	d.new_state(s, "B2", Action(), Rect(0, 50, 50, 50));
	d.new_initial(s, Point(0, 0));
	d.new_state(sm, "C", Action(), Rect(-50, 0, 1000, 100));
	try {
		Rect br = d.get_bound_rect();
		//cout << br << endl;
		CYB_ASSERT(br == Rect(-50, -143.75, 1000, 412.5));
	} catch (const Cyberiada::Exception&) {
		return 1;
	}
	return 0;
}
