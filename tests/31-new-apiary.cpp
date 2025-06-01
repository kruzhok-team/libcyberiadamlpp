/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The new Apiary Defense format test
 *
 * Copyright (C) 2025 Alexey Fedoseev <aleksey@fedoseev.net>
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
	try {
		LocalDocument d;
		d.open(string(argv[0]) + "-input.graphml", formatDetect, geometryFormatNone);
		cout << d << endl;		
		d.open(string(argv[0]) + "-input.graphml", formatDetect, geometryFormatLegacyYED);
		cout << d << endl;
		d.open(string(argv[0]) + "-input.graphml", formatDetect, geometryFormatCyberiada10);
		cout << d << endl;
		d.open(string(argv[0]) + "-input.graphml", formatDetect, geometryFormatQt);
		cout << d << endl;
		d.save_as(string(argv[0]) + ".graphml", formatCyberiada10);
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
