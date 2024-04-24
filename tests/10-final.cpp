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

	d.new_final(sm);
	try {
		// check id uniqueness
		d.new_final(sm, "n0", "test");
	} catch (const Cyberiada::ParametersException&){
	}
	try {
		// check non-empty name
		d.new_final(sm, "");
	} catch (const Cyberiada::ParametersException&){
	}
	
	State* parent = d.new_state(sm, "State");
	d.new_final(parent, "Local exit");

	try {
		cout << d << endl;
		d.save(string(argv[0]) + ".graphml", formatCyberiada10);
	} catch (const Cyberiada::Exception&) {
		return 1;
	}
	return 0;
}
