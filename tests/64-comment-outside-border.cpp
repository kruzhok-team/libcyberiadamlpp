/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The comment placement test: a comment may sit outside the SM border
 *
 * Copyright (C) 2026 Alexey Fedoseev <aleksey@fedoseev.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see https://www.gnu.org/licenses/
 *
 * ----------------------------------------------------------------------------- */

#include <iostream>
#include "cyberiadamlpp.h"

using namespace Cyberiada;
using namespace std;

int main(int, char** argv)
{
	try {
		LocalDocument d;
		d.open(string(argv[0]) + "-input.graphml", formatDetect, geometryFormatCyberiada10);
		const StateMachine* sm = d.get_state_machines().front();

		/* the comment sits outside the border, yet the geometry is valid: the
		   border fit ignores comments */
		cout << "comment outside, check: " << d.check_geometry() << endl;
		cout << "border: " << sm->get_geometry_rect() << endl;
		/* the export bound includes the comment, the border-fit bound excludes it */
		cout << "bound with comment: " << sm->ElementCollection::get_bound_rect(d, false) << endl;
		cout << "bound border-fit:   " << sm->ElementCollection::get_bound_rect(d, true) << endl;

		/* reconstructing the SM border does not stretch it toward the outside
		   comment: the border stays bounded by the inside content */
		d.reconstruct_geometry(true);
		sm = d.get_state_machines().front();   // the reconstruction rebuilt the tree
		Rect b = sm->get_geometry_rect();
		cout << "after reconstruct, check: " << d.check_geometry() << endl;
		cout << "border reaches the comment: " << (b.valid && b.x + b.width >= 700 ? 1 : 0) << endl;

		/* it round-trips through a save and reopen */
		d.save_as(string(argv[0]) + "-roundtrip.graphml", formatCyberiada10);
		LocalDocument d2;
		d2.open(string(argv[0]) + "-roundtrip.graphml", formatDetect, geometryFormatCyberiada10);
		cout << "after round-trip, check: " << d2.check_geometry() << endl;
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
