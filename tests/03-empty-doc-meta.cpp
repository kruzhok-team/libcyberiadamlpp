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

using namespace Cyberiada;
using namespace std;

int main(int argc, char** argv)
{
	LocalDocument d;
	d.new_state_machine("SM");
	d.meta().set_string("platform", "Berloga");
	d.meta().set_string("platformVersion", "1.4");
	d.meta().set_string("platformLanguage", "script");
	d.meta().set_string("target", "Unit");
	d.meta().set_string("name", "Test document");
	d.meta().set_string("author", "Author");
	d.meta().set_string("contact", "platform@kruzhok.org");
	d.meta().set_string("description", "1\n2\n3"); 
	d.meta().set_string("version", "0.1");
	d.meta().set_string("date", "2024-04-14T11:22:00");
	d.meta().set_string("markupLanguage", "html");
	d.meta().transition_order_flag = true; // exit first
	d.meta().event_propagation_flag = true; // propagate
	try {
		d.save_as(string(argv[0]) + ".graphml", formatCyberiada10);
	} catch (const Cyberiada::Exception&) {
		return 1;
	}
	return 0;
}
