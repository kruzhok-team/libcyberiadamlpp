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
		CYB_ASSERT(d.meta().get_string("platform") == "Berloga");
		CYB_ASSERT(d.meta().get_string("platformVersion") == "1.4");
		CYB_ASSERT(d.meta().get_string("platformLanguage") == "script");
		CYB_ASSERT(d.meta().get_string("target") == "Unit");
		CYB_ASSERT(d.meta().get_string("name") == "Test document");
		CYB_ASSERT(d.meta().get_string("author") == "Author");
		CYB_ASSERT(d.meta().get_string("contact") == "platform@kruzhok.org");
		CYB_ASSERT(d.meta().get_string("description") == "1\n2\n3"); 
		CYB_ASSERT(d.meta().get_string("version") == "0.1");
		CYB_ASSERT(d.meta().get_string("date") == "2024-04-14T11:22:00");
		CYB_ASSERT(d.meta().get_string("markupLanguage") == "html");
		CYB_ASSERT(d.meta().transition_order_flag); // exit first
		CYB_ASSERT(d.meta().event_propagation_flag); // propagate
		cout << d << endl;
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
