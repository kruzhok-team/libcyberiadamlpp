/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The XML and buffer exceptions test
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
		// broken XML markup
		try {
			LocalDocument ld;
			ld.open(string(argv[0]) + "-input.graphml");
			return 1;
		} catch (const Cyberiada::XMLException&) {
		}

		// missing GraphML root node
		try {
			LocalDocument ld;
			ld.open(string(argv[0]) + "-input2.graphml");
			return 1;
		} catch (const Cyberiada::XMLException&) {
		}

		DocumentFormat format = formatDetect;
		String format_str;

		// decoding a non-XML buffer
		try {
			Document d;
			d.decode("not an xml buffer", format, format_str);
			return 1;
		} catch (const Cyberiada::XMLException&) {
		}

		// decoding an empty buffer
		try {
			Document d;
			d.decode("", format, format_str);
			return 1;
		} catch (const Cyberiada::ParametersException&) {
		}

		// decoding with a bad geometry format
		try {
			Document d;
			d.decode("buffer", format, format_str, DocumentGeometryFormat(99));
			return 1;
		} catch (const Cyberiada::ParametersException&) {
		}

		// encoding with the detect format
		try {
			Document d;
			d.new_state_machine("SM");
			String buffer;
			d.encode(buffer, formatDetect);
			return 1;
		} catch (const Cyberiada::ParametersException&) {
		}

		// the legacy format supports single-SM documents only
		try {
			Document d;
			d.new_state_machine("SM 1");
			d.new_state_machine("SM 2");
			String buffer;
			d.encode(buffer, formatLegacyYED);
			return 1;
		} catch (const Cyberiada::ParametersException&) {
		}
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
