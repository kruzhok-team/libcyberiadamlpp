/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The EXT-FA documents: the automaton flag, the alphabets, the FA exception
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

static void open_document(LocalDocument& d, const String& path, bool strict)
{
	d.open(path, formatCyberiada10, geometryFormatNone, false, false, false, false, false, strict);
}

int main(int, char** argv)
{
	try {
		String prefix = string(argv[0]) + "-";

		// a valid automaton: the flag and the alphabet comments
		LocalDocument valid;
		open_document(valid, prefix + "valid.graphml", true);
		CYB_ASSERT(valid.is_fa());
		CYB_ASSERT(valid.has_input_alphabet());
		CYB_ASSERT(valid.get_input_alphabet()->get_name() == "FA_INPUT");
		CYB_ASSERT(valid.get_input_alphabet()->get_body() == "0\n1");
		CYB_ASSERT(valid.has_output_alphabet());
		CYB_ASSERT(valid.get_output_alphabet()->get_name() == "FA_OUTPUT_RE");

		// a transition with a guard is refused in both modes with the FA exception
		for (int strict = 0; strict < 2; strict++) {
			try {
				LocalDocument invalid;
				open_document(invalid, prefix + "invalid.graphml", strict != 0);
				return 1;
			} catch (const FAException&) {
			}
		}

		// the same document without type/ fa is an ordinary state machine
		LocalDocument plain;
		open_document(plain, prefix + "plain.graphml", true);
		CYB_ASSERT(!plain.is_fa());
		CYB_ASSERT(!plain.has_input_alphabet());
		CYB_ASSERT(!plain.has_output_alphabet());
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
