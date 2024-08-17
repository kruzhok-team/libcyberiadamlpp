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
	d.meta().platform_name = "Berloga";
	d.meta().platform_version = "1.4";
	d.meta().platform_language = "script";
	d.meta().target_system = "Unit";
	d.meta().name = "Test document";
	d.meta().author = "Author";
	d.meta().contact = "platform@kruzhok.org";
	d.meta().description = "1\n2\n3"; 
	d.meta().version = "0.1";
	d.meta().date = "2024-04-14T11:22:00";
	d.meta().markup_language = "html";
	d.meta().transition_order_flag = true; // exit first
	d.meta().event_propagation_flag = true; // propagate
	try {
		d.save_as(string(argv[0]) + ".graphml", formatCyberiada10);
	} catch (const Cyberiada::Exception&) {
		return 1;
	}
	return 0;
}
