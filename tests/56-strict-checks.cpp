/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The strict standard checks test
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

// load the diagram and report whether the library accepted it
static bool loaded(const String& path, bool strict)
{
	try {
		LocalDocument d;
		d.open(path, formatCyberiada10, geometryFormatNone, false, false, false, false, false, strict);
	} catch (const CybMLException&) {
		return false;
	}
	return true;
}

int main(int argc, char** argv)
{
	try {
		String prefix = string(argv[0]) + "-";

		// the optional requirements are checked in the strict mode only
		const char* strict_only[] = { "bad-id-char", "no-marker", "no-sm-name", "vertex-not-first" };
		for (size_t i = 0; i < sizeof(strict_only) / sizeof(strict_only[0]); i++) {
			String path = prefix + strict_only[i] + ".graphml";
			CYB_ASSERT(loaded(path, false));
			CYB_ASSERT(!loaded(path, true));
		}

		// the mandatory requirements are checked in any mode
		CYB_ASSERT(!loaded(prefix + "foreign-tag.graphml", false));
		CYB_ASSERT(!loaded(prefix + "foreign-tag.graphml", true));

		// the conforming document is read in the strict mode
		CYB_ASSERT(loaded(prefix + "two-machines.graphml", false));
		CYB_ASSERT(loaded(prefix + "two-machines.graphml", true));
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
