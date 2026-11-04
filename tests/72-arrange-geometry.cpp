/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The test of the geometry arrange
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

static State* state_of(Document& d, const char* id)
{
	StateMachine* sm = d.get_state_machines().front();
	return static_cast<State*>(sm->find_element_by_id(id));
}

/* the full layout puts n1 below the composite n0; drawn above it, n1 stays
   above after the arrange, both laid out anew */
int main(int, char** argv)
{
	LocalDocument d;
	d.open(string(argv[0]) + "-input.graphml");
	d.reconstruct_geometry(true);
	d.round_geometry();
	State* n0 = state_of(d, "n0");
	State* n1 = state_of(d, "n1");
	Rect p0 = n0->get_geometry_rect(), p1 = n1->get_geometry_rect();
	cout << "the full layout: n1 below n0: " << (p1.y > p0.y) << endl;

	/* the seed: n1 drawn above n0, both drawn small */
	n0->update_geometry(Rect(p0.x, p1.y, 10, 10));
	n1->update_geometry(Rect(p1.x, p0.y, 10, 10));
	d.arrange_geometry(true);
	d.round_geometry();

	/* the geometry calls rebuild the elements: fetch them again */
	Rect q0 = state_of(d, "n0")->get_geometry_rect(), q1 = state_of(d, "n1")->get_geometry_rect();
	cout << "the arrange: n1 above n0: " << (q1.y + q1.height <= q0.y) << endl;
	cout << "the sizes from the texts: " << (q1.width == p1.width && q1.height == p1.height) << endl;
	cout << "check: " << d.check_geometry() << endl;
	cout << d << endl;
	return 0;
}
