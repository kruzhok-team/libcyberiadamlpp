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
	LocalDocument ld;
	try {
		ld.open(string(argv[0]) + "-input.graphml", formatCyberiada10);
		Document d(ld);
		CYB_ASSERT(d.get_state_machines().front()->get_name() == "SM");
		CYB_ASSERT(d.meta().platform_name == "Berloga");
		CYB_ASSERT(d.meta().platform_version == "1.4");
		CYB_ASSERT(d.meta().platform_language == "script");
		CYB_ASSERT(d.meta().target_system == "Unit");
		CYB_ASSERT(d.meta().name == "Test document");
		CYB_ASSERT(d.meta().author == "Author");
		CYB_ASSERT(d.meta().contact == "platform@kruzhok.org");
		CYB_ASSERT(d.meta().description == "1\n2\n3"); 
		CYB_ASSERT(d.meta().version == "0.1");
		CYB_ASSERT(d.meta().date == "2024-04-14T11:22:00");
		CYB_ASSERT(d.meta().markup_language == "html");
		CYB_ASSERT(d.meta().transition_order_flag); // exit first
		CYB_ASSERT(d.meta().event_propagation_flag); // propagate
		cout << d << endl;
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
