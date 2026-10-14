/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The reconstruction border test: the wide labels fit the machine border
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

/* The full reconstruction keeps the wide transition labels inside the
   machine border, and the geometry check reads the Qt-format label rects
   by their centre */
int main(int, char**)
{
	Document d;
	StateMachine* sm = d.new_state_machine("Bomb");
	InitialPseudostate* init = d.new_initial(sm);
	State* setting = d.new_state(sm, "Setting");
	State* timing = d.new_state(sm, "Timing");
	d.new_transition(sm, transitionExternal, init, setting,
					 Action("", "", "timeout = INIT_TIMEOUT"));
	d.new_transition(sm, transitionExternal, setting, timing, Action("ARM", "", "code = 0"));
	d.new_transition(sm, transitionExternal, timing, setting,
					 Action("ARM", "code == defuse", "show(code); reset(timer)"));
	d.reconstruct_geometry(true);
	cout << "check after the reconstruction: " << d.check_geometry() << endl;
	cout << d << endl;
	return 0;
}
