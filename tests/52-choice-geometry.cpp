/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The choice pseudostate geometry update test
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

static ChoicePseudostate* choice(Document& d, const ID& id)
{
	Element* e = d.get_state_machines().front()->find_element_by_id(id);
	CYB_ASSERT(e && e->get_type() == elementChoice);
	return static_cast<ChoicePseudostate*>(e);
}

int main(int argc, char** argv)
{
	try {
		LocalDocument d;
		d.open(string(argv[0]) + "-input.graphml");

		ChoicePseudostate* local = choice(d, "n1::n0");
		CYB_ASSERT(local->has_geometry());
		CYB_ASSERT(local->has_rect_geometry());
		CYB_ASSERT(!local->has_point_geometry());
		CYB_ASSERT(local->get_geometry_rect() == Rect(0, 5, 100, 50));

		// the choice rect can be updated
		local->update_geometry(Rect(20, 30, 60, 40));
		CYB_ASSERT(local->get_geometry_rect() == Rect(20, 30, 60, 40));
		CYB_ASSERT(local->get_bound_rect(d) == Rect(20, 30, 60, 40));

		// the update gives geometry to a choice without one
		ChoicePseudostate* bare = choice(d, "n0");
		CYB_ASSERT(!bare->has_geometry());
		bare->update_geometry(Rect(-100, -50, 40, 40));
		CYB_ASSERT(bare->has_geometry());

		// both rects survive the save/reopen round trip
		LocalDocument saved(d, string(argv[0]) + ".graphml");
		saved.save();
		LocalDocument d2;
		d2.open(string(argv[0]) + ".graphml");
		CYB_ASSERT(choice(d2, "n1::n0")->get_geometry_rect() == Rect(20, 30, 60, 40));
		CYB_ASSERT(choice(d2, "n0")->get_geometry_rect() == Rect(-100, -50, 40, 40));

		// the geometry can be dropped again
		choice(d2, "n0")->clean_geometry();
		CYB_ASSERT(!choice(d2, "n0")->has_geometry());
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
