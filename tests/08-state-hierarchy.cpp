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
	try {
		// check id uniqueness
		d.new_state(sm, "n0", "test");
	} catch (const Cyberiada::ParametersException&){
	}
	try {
		// check non-empty name
		d.new_state(sm, "");
	} catch (const Cyberiada::ParametersException&){
	}
	
	CYB_ASSERT(parent1->is_simple_state());
	d.new_state(parent1, "State 0-0");
	CYB_ASSERT(parent1->is_composite_state());

	State* subparent = d.new_state(parent1, "Subparent 0-1");
	d.new_state(subparent, "State 0-1-0");
	d.new_state(subparent, "State 0-1-1");
	
	State* parent2 = d.new_state(sm, "Parent 1");
	d.new_state(parent2, "State 1-0");
	State* ch = d.new_state(parent2, "State 1-1");

	CYB_ASSERT(ch->qualified_name() == "Parent 1::State 1-1");
	CYB_ASSERT(ch->full_qualified_name() == "SM::Parent 1::State 1-1");

	try {
		// check id uniqueness on the deep level
		d.new_state(parent2, "n1::n1", "test");
	} catch (const Cyberiada::ParametersException&){
	}

	try {
		cout << d << endl;
		LocalDocument ld(d, string(argv[0]) + ".graphml");
		ld.save();
	} catch (const Cyberiada::Exception&) {
		return 1;
	}
	return 0;
}
