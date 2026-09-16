/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The edge label and comment subject bound rect test
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

#include <cmath>
#include <iostream>
#include "cyberiadamlpp.h"
#include "testutils.h"

using namespace Cyberiada;
using namespace std;

int main(int, char** argv)
{
	try {
		// a transition label rect outside the node extents loads without
		// the bound rect mismatch and extends the document bound rect
		LocalDocument d;
		d.open(string(argv[0]) + "-input.graphml");
		Rect br = d.get_bound_rect();
		CYB_ASSERT(br.valid);
		CYB_ASSERT(fabs(br.width - 1000.0) < 0.01);
		CYB_ASSERT(fabs(br.height - 760.0) < 0.01);
		const StateMachine* sm = d.get_state_machines().front();
		const Transition* t =
			static_cast<const Transition*>(sm->find_element_by_id("n0-n1"));
		CYB_ASSERT(t);
		CYB_ASSERT(t->has_geometry_label_rect());

		// the label survives the save/reopen round trip
		LocalDocument saved(d, string(argv[0]) + ".graphml");
		saved.save();
		LocalDocument d2;
		d2.open(string(argv[0]) + ".graphml");
		const Transition* t2 = static_cast<const Transition*>(
			d2.get_state_machines().front()->find_element_by_id("n0-n1"));
		CYB_ASSERT(t2);
		CYB_ASSERT(t2->has_geometry_label_rect());
		Rect br2 = d2.get_bound_rect();
		CYB_ASSERT(fabs(br2.width - br.width) < 0.01);
		CYB_ASSERT(fabs(br2.height - br.height) < 0.01);

		// a subject edge polyline agrees between the libraries...
		LocalDocument s1;
		s1.open(string(argv[0]) + "-input2.graphml");
		const Comment* c1 = static_cast<const Comment*>(
			s1.get_state_machines().front()->find_element_by_id("cX"));
		CYB_ASSERT(c1);
		CYB_ASSERT(c1->has_subjects());

		// ...with a geometry-less comment as well
		LocalDocument s2;
		s2.open(string(argv[0]) + "-input3.graphml");
		const Comment* c2 = static_cast<const Comment*>(
			s2.get_state_machines().front()->find_element_by_id("cX"));
		CYB_ASSERT(c2);
		CYB_ASSERT(!c2->has_geometry());
		CYB_ASSERT(c2->has_subjects());
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
