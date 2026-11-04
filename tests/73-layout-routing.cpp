/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The test of the layout routing: the orthogonal routes of the reconstruction,
 * the update and the arrange, the polylines the default
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
#include <sstream>
#include "cyberiadamlpp.h"
#include "testutils.h"

using namespace Cyberiada;
using namespace std;

static string dump(const Document& d)
{
	ostringstream s;
	s << d;
	return s.str();
}

int main(int, char** argv)
{
	LocalDocument d, o;
	d.open(string(argv[0]) + "-input.graphml");
	o.open(string(argv[0]) + "-input.graphml");
	d.reconstruct_geometry(true);
	o.reconstruct_geometry(true, layoutRoutingPolyline);
	d.round_geometry();
	o.round_geometry();
	cout << "the polylines the default: " << (dump(d) == dump(o)) << endl;

	o.reconstruct_geometry(true, layoutRoutingOrthogonal);
	o.round_geometry();
	cout << "the orthogonal routes differ: " << (dump(d) != dump(o)) << endl;
	cout << "check: " << o.check_geometry() << endl;
	o.update_geometry(true, layoutRoutingOrthogonal);
	cout << "the update check: " << o.check_geometry() << endl;
	o.arrange_geometry(true, layoutRoutingOrthogonal);
	o.round_geometry();
	cout << "the arrange check: " << o.check_geometry() << endl;
	cout << o << endl;
	return 0;
}
