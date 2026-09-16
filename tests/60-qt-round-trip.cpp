/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The Qt geometry round trip test: a state machine without a rect keeps its global coordinates
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
#include <cmath>
#include "cyberiadamlpp.h"
#include "testutils.h"

using namespace Cyberiada;
using namespace std;

// the content is not centred on the origin on purpose: a frame taken from
// the content would move it on the way through the file
int main()
{
	try {
		Document d;
		StateMachine* sm = d.new_state_machine("SM");
		State* working = d.new_state(sm, "Working", Action(), Rect(10, 20, 200, 100));
		State* parent = d.new_state(sm, "Parent", Action(), Rect(-300, -200, 400, 300));
		Vertex* initial = d.new_initial(parent, Point(-40, -40));
		Vertex* final = d.new_final(sm, Point(300, 100));
		Rect before = sm->get_bound_rect(d);

		String buffer;
		d.encode(buffer);
		Document d1;
		DocumentFormat f = formatDetect;
		String fs;
		d1.decode(buffer, f, fs, geometryFormatQt);

		const StateMachine* sm1 = d1.get_state_machines().front();
		CYB_ASSERT(sm1->get_bound_rect(d1).almost_equal(before));
		const State* w1 = static_cast<const State*>(d1.find_element_by_id(working->get_id()));
		const State* p1 = static_cast<const State*>(d1.find_element_by_id(parent->get_id()));
		const Vertex* i1 = static_cast<const Vertex*>(d1.find_element_by_id(initial->get_id()));
		const Vertex* f1 = static_cast<const Vertex*>(d1.find_element_by_id(final->get_id()));
		CYB_ASSERT(w1 && p1 && i1 && f1);
		CYB_ASSERT(w1->get_geometry_rect().almost_equal(working->get_geometry_rect()));
		CYB_ASSERT(p1->get_geometry_rect().almost_equal(parent->get_geometry_rect()));
		CYB_ASSERT(std::fabs(i1->get_geometry_point().x - initial->get_geometry_point().x) < 0.001 &&
				   std::fabs(i1->get_geometry_point().y - initial->get_geometry_point().y) < 0.001);
		CYB_ASSERT(std::fabs(f1->get_geometry_point().x - final->get_geometry_point().x) < 0.001 &&
				   std::fabs(f1->get_geometry_point().y - final->get_geometry_point().y) < 0.001);
		// and the round trip is stable: a second one changes nothing either
		String buffer2;
		d1.encode(buffer2);
		CYB_ASSERT(buffer2 == buffer);
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
