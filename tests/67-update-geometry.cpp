/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The geometry update test: the drawn elements stay, the new ones join the flow
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
#include "testutils.h"

using namespace Cyberiada;
using namespace std;

static Rect rect_of(const Document& d, const char* id)
{
	const StateMachine* sm = d.get_state_machines().front();
	return static_cast<const State*>(sm->find_element_by_id(id))->get_geometry_rect();
}

/* the Qt coordinates are relative to the machine centre, which moves when
   the machine grows: the drawn elements are compared by their mutual places */
int main(int, char** argv)
{
	LocalDocument d;
	d.open(string(argv[0]) + "-input.graphml");
	d.reconstruct_geometry(true);
	d.round_geometry();
	Rect p0 = rect_of(d, "n0"), p1 = rect_of(d, "n1");
	Rect c0 = rect_of(d, "n0::n0"), c1 = rect_of(d, "n0::n1");

	/* a new state after the last one and a new transition into it, no geometry */
	{
		StateMachine* machine = d.get_state_machines().front();
		State* n1 = static_cast<State*>(machine->find_element_by_id("n1"));
		State* added = d.new_state(machine, "Added");
		d.new_transition(machine, transitionExternal, n1, added, Action("NEXT"));
	}
	d.update_geometry(true);
	d.round_geometry();

	Rect q0 = rect_of(d, "n0"), q1 = rect_of(d, "n1");
	Rect e0 = rect_of(d, "n0::n0"), e1 = rect_of(d, "n0::n1");
	const StateMachine* sm = d.get_state_machines().front();
	const State* added = static_cast<const State*>(sm->find_element_by_id("n2"));
	cout << "the sizes stay: " << (p0.width == q0.width && p0.height == q0.height &&
								   p1.width == q1.width && p1.height == q1.height) << endl;
	cout << "n1 keeps its place below n0: " << (q1.x - q0.x == p1.x - p0.x && q1.y - q0.y == p1.y - p0.y) << endl;
	cout << "the composite content stays: " << (e0.x == c0.x && e0.y == c0.y && e1.x == c1.x && e1.y == c1.y) << endl;
	cout << "the added state below n1: " << (added && added->has_geometry() &&
											  added->get_geometry_rect().y > q1.y) << endl;
	cout << "check: " << d.check_geometry() << endl;
	cout << d << endl;
	return 0;
}
