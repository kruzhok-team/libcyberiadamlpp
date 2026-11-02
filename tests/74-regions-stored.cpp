/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The test of the regionGeometry meta parameter: set, saved, read back, removed
 *
 * Copyright (C) 2026 Alexey Fedoseev <aleksey@fedoseev.net>
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

int main(int, char** argv)
{
	LocalDocument d;
	d.open(string(argv[0]) + "-input.graphml");
	cout << "read: " << d.meta().regions_stored() << endl;
	d.reconstruct_geometry(true);
	cout << "the layout leaves it to the producer: " << d.meta().regions_stored() << endl;
	d.meta().set_regions_stored(true);
	cout << "set: " << d.meta().regions_stored() << endl;
	string path = string(argv[0]) + "-saved.graphml";
	d.save_as(path, formatCyberiada10);
	LocalDocument saved;
	saved.open(path);
	cout << "read back: " << saved.meta().regions_stored() << endl;
	saved.meta().set_regions_stored(false);
	cout << "removed: " << saved.meta().regions_stored() << endl;
	return 0;
}
