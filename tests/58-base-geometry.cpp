/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The base (short) geometry format test
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

#define EPS 0.01

// the loose rect size is derived on load: no state is left without one
static void check_sizes(const Document& d)
{
	ElementTypes types;
	types.push_back(elementSimpleState);
	types.push_back(elementCompositeState);
	ConstElementList states = d.find_elements_by_types(types);
	CYB_ASSERT(!states.empty());
	for (ConstElementList::const_iterator i = states.begin(); i != states.end(); i++) {
		const ElementCollection* s = static_cast<const ElementCollection*>(*i);
		CYB_ASSERT(s->has_geometry());
		Rect r = s->get_geometry_rect();
		CYB_ASSERT(r.valid);
		CYB_ASSERT(r.width > 0.0 && r.height > 0.0);
	}
}

static Rect state_rect(const Document& d, const ID& id)
{
	const Element* e = d.find_element_by_id(id);
	CYB_ASSERT(e);
	return static_cast<const ElementCollection*>(e)->get_geometry_rect();
}

int main(int argc, char** argv)
{
	try {
		// the robot-vacuum example of the standard is authored in the base format
		LocalDocument d;
		d.open(string(argv[0]) + "-input.graphml", formatCyberiada10, geometryFormatCyberiada10);
		CYB_ASSERT(d.meta().get_geometry() == geometryDeclarationShort);
		check_sizes(d);
		// the content of the composite fits it
		CYB_ASSERT(d.check_geometry());

		// the corners the file authored are kept, the sizes are derived
		Rect r = state_rect(d, "n0::n1");
		CYB_ASSERT(fabs(r.x - 50.0) < EPS && fabs(r.y - 100.0) < EPS);
		r = state_rect(d, "n0::n2");
		CYB_ASSERT(fabs(r.x - 50.0) < EPS && fabs(r.y - 550.0) < EPS);
		r = state_rect(d, "n0");
		CYB_ASSERT(fabs(r.x - 800.0) < EPS && fabs(r.y - 0.0) < EPS);
		CYB_ASSERT(r.width >= 350.0 && r.height >= 750.0);
		cout << Document(d) << endl;

		// the declaration and the derived sizes survive the round trip
		LocalDocument saved(d, string(argv[0]) + ".graphml");
		saved.save();
		LocalDocument d1;
		d1.open(string(argv[0]) + ".graphml", formatCyberiada10, geometryFormatCyberiada10);
		CYB_ASSERT(d1.meta().get_geometry() == geometryDeclarationShort);
		check_sizes(d1);
		CYB_ASSERT(d1.get_state_machines().front()->check_isomorphism(*(d.get_state_machines().front())) ==
				   smiIdentical);

		// the shrunk appendix Г.4 example
		LocalDocument d2;
		d2.open(string(argv[0]) + "-input2.graphml", formatCyberiada10, geometryFormatCyberiada10);
		CYB_ASSERT(d2.meta().get_geometry() == geometryDeclarationShort);
		check_sizes(d2);
		CYB_ASSERT(d2.check_geometry());
		r = state_rect(d2, "node-0-0-1");
		CYB_ASSERT(fabs(r.x - 50.0) < EPS && fabs(r.y - 90.0) < EPS);
		cout << Document(d2) << endl;

		// the declaration is written, replaced and removed
		Document d3;
		d3.new_state_machine("sm", "SM");
		CYB_ASSERT(d3.meta().get_geometry() == geometryDeclarationAbsent);
		String buffer;
		d3.meta().set_geometry(geometryDeclarationFull);
		d3.encode(buffer);
		CYB_ASSERT(buffer.find("geometry/ full") != String::npos);
		d3.meta().set_geometry(geometryDeclarationNone);
		CYB_ASSERT(d3.meta().get_geometry() == geometryDeclarationNone);
		d3.meta().set_geometry(geometryDeclarationAbsent);
		CYB_ASSERT(d3.meta().get_geometry() == geometryDeclarationAbsent);
		d3.encode(buffer);
		CYB_ASSERT(buffer.find("geometry/") == String::npos);
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
