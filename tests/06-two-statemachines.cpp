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
	Document d;
	d.new_state_machine("SM1");
	try {
		// check id uniqueness
		d.new_state_machine("G0", "SM2");
	} catch (const Cyberiada::ParametersException&){
	}
	d.new_state_machine("SM2", Rect(1, 2, 300, 40));
	try {
		cout << d << endl;
		d.save(string(argv[0]) + ".graphml", formatCyberiada10);
	} catch (const Cyberiada::Exception&) {
		return 1;
	}
	return 0;
}
