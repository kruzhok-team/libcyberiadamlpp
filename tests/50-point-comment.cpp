/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The point geometry comment test
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
	try {
		// the strict load rejects the comment with point geometry
		bool rejected = false;
		try {
			LocalDocument bad;
			bad.open(string(argv[0]) + "-input.graphml");
		} catch (const FormatException&) {
			rejected = true;
		}
		CYB_ASSERT(rejected);

		// the reconstruction mode repairs the malformed geometry
		LocalDocument d;
		d.open(string(argv[0]) + "-input.graphml", formatDetect,
			   geometryFormatQt, true);
		const StateMachine* sm = d.get_state_machines().front();
		const Comment* note = static_cast<const Comment*>(sm->find_element_by_id("cX"));
		CYB_ASSERT(note);
		CYB_ASSERT(note->get_type() == elementComment);
		CYB_ASSERT(note->has_geometry());
		CYB_ASSERT(note->get_geometry_rect().valid);
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
