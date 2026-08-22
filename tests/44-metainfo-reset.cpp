/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The metainformation and document reset test
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
		LocalDocument ld;
		ld.open(string(argv[0]) + "-input.graphml", formatCyberiada10);

		// the metainformation strings
		CYB_ASSERT(ld.meta().get_string("platform") == "Berloga");
		CYB_ASSERT(ld.meta().get_string("nonexistent") == "");
		ld.meta().set_string("platform", "Arduino");
		CYB_ASSERT(ld.meta().get_string("platform") == "Arduino");
		ld.meta().set_string("customKey", "custom value");
		CYB_ASSERT(ld.meta().get_string("customKey") == "custom value");

		// the document rename updates the meta comment
		ld.set_name("Updated document");
		CYB_ASSERT(ld.meta().get_string("name") == "Updated document");
		CYB_ASSERT(ld.get_meta_element() != NULL);

		// the metainfo comment update is a stub
		CYB_ASSERT(!ld.update_metainfo_from_comment("name/ Other"));

		// the encode/decode buffer round trip
		String buffer;
		ld.encode(buffer, formatCyberiada10);
		Document d2;
		DocumentFormat format = formatDetect;
		String format_str;
		d2.decode(buffer, format, format_str);
		CYB_ASSERT(format == formatCyberiada10);
		CYB_ASSERT(d2.elements_count() == ld.elements_count());
		CYB_ASSERT(d2.meta().get_string("customKey") == "custom value");

		cout << d2 << endl;

		// opening without geometry
		LocalDocument ld2;
		ld2.open(string(argv[0]) + "-input.graphml", formatCyberiada10, geometryFormatNone);
		CYB_ASSERT(ld2.get_geometry_format() == geometryFormatNone);
		CYB_ASSERT(!ld2.has_geometry());

		// renaming an empty document
		Document empty;
		empty.set_name("No SM");
		CYB_ASSERT(empty.get_name() == "No SM");
		CYB_ASSERT(empty.get_meta_element() == NULL);

		// the document reset
		ld.reset();
		CYB_ASSERT(ld.children_count() == 0);
		CYB_ASSERT(ld.get_file_path() == "");
		CYB_ASSERT(ld.get_file_format() == formatCyberiada10);
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
