/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The reconstruction test: the missing geometry is generated
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

int main(int argc, char** argv)
{
	try {
		/* the full reconstruction of a geometry-less document */
		LocalDocument d;
		d.open(string(argv[0]) + "-input.graphml");
		d.reconstruct_geometry(true);
		d.round_geometry();
		cout << d << endl;

		/* the read-path fill-in: only the edges lack geometry */
		LocalDocument p;
		p.open("tests/23-cyb-autoborder.test-input.graphml", formatDetect,
			   geometryFormatQt, true, true);
		p.round_geometry();
		cout << p << endl;

		/* the metainformation node is not displayed (6.9): the reconstruction
		   leaves it and the comment edge it carries out of the geometry */
		LocalDocument m;
		m.open(string(argv[0]) + "-input2.graphml");
		m.reconstruct_geometry(true);
		m.round_geometry();
		const Element* meta = m.find_element_by_id("nMeta");
		CYB_ASSERT(meta && meta->get_type() == elementFormalComment);
		CYB_ASSERT(!meta->has_geometry());
		CYB_ASSERT(m.find_element_by_id("n0")->has_geometry());
		cout << Document(m) << endl;
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
