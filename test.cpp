/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The testing program
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
#include <stdlib.h>
#include "cyberiadamlpp.h"

using namespace Cyberiada;
using namespace std;

void usage(const char* program)
{
	cerr << program << " <path-to-graphml-file>" << endl;
	cerr << "\tPrint the graphml SM structure of the file <path-to-graphml-file>" << endl;
	exit(1);
}

int main(int argc, char** argv)
{
	Document d;
	
	if (argc != 2) {
		usage(argv[0]);
	} 

	try {
		d.load(argv[1]);
		cout << d << endl;
	} catch (const Cyberiada::Exception& e) {
		cerr << "Error while opening graphml file: " << e.str() << endl;
		return 2;
	}
	return 0;
}
