/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The document saving options test
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
		String input = string(argv[0]) + "-input.graphml";
		String out = string(argv[0]) + ".graphml";

		// the dialect is detected on reading
		LocalDocument doc;
		doc.open(input);
		CYB_ASSERT(doc.get_file_format() == formatLegacyYED);
		CYB_ASSERT(doc.get_file_format_str() == "yEd Berloga-1.6");

		CYB_ASSERT(is_legacy_yed_format(formatLegacyYED));
		CYB_ASSERT(is_legacy_yed_format(formatLegacyYEDOstranna));
		CYB_ASSERT(is_legacy_yed_format(formatLegacyYEDBerloga16));
		CYB_ASSERT(!is_legacy_yed_format(formatCyberiada10));

		// the yEd dialects are explicit write targets
		String buffer;
		doc.encode(buffer, formatLegacyYEDOstranna);
		CYB_ASSERT(buffer.find("SchemeName") == String::npos);
		buffer.clear();
		doc.encode(buffer, formatLegacyYEDBerloga16);
		CYB_ASSERT(buffer.find("SchemeName") != String::npos);
		CYB_ASSERT(buffer.find("coreMeta") != String::npos);

		// the geometry cannot be skipped in the yEd format
		bool refused = false;
		try {
			buffer.clear();
			doc.encode(buffer, formatLegacyYEDOstranna, false, true);
		} catch (const ParametersException&) {
			refused = true;
		}
		CYB_ASSERT(refused);

		// the checks are passed to the library
		buffer.clear();
		doc.encode(buffer, formatCyberiada10, true, false, true, true, true);
		CYB_ASSERT(!buffer.empty());

		// the document keeps its own file when the new one is refused
		refused = false;
		try {
			doc.save_as(out, formatLegacyYEDOstranna, false, true);
		} catch (const ParametersException&) {
			refused = true;
		}
		CYB_ASSERT(refused);
		CYB_ASSERT(doc.get_file_path() == input);
		CYB_ASSERT(doc.get_file_format_str() == "yEd Berloga-1.6");

		// the saved document keeps the chosen dialect
		doc.save_as(out, formatLegacyYEDBerloga16);
		CYB_ASSERT(doc.get_file_format() == formatLegacyYEDBerloga16);
		CYB_ASSERT(doc.get_file_format_str() == "yEd Berloga-1.6");

		LocalDocument reopened;
		reopened.open(out);
		CYB_ASSERT(reopened.get_file_format_str() == "yEd Berloga-1.6");
		cout << "the saved document keeps the dialect" << endl;
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
