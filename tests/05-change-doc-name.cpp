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

#define CYB_ASSERT(q)   if (!(q)) {                                      \
					        throw AssertException(std::string(__FILE__) + ":" + std::to_string(__LINE__)); \
	                    }

int main(int argc, char** argv)
{
	Document d;
	try {
		d.load(string(argv[0]) + "-input.graphml", formatCyberiada10);
		CYB_ASSERT(d.meta().name == "Test document");
		d.set_name("Test document 2");
		CYB_ASSERT(d.meta().name == "Test document 2");
		cout << d << endl;
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
