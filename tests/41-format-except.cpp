/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The format exceptions test
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
#include <type_traits>
#include "cyberiadamlpp.h"
#include "testutils.h"

using namespace Cyberiada;
using namespace std;

// every exception must reach a catch site expecting std::exception:
// a private base compiles but is silently uncatchable (MSVC C4670 / C4673)
#define CYB_CATCHABLE(T) \
	static_assert(std::is_convertible<T*, std::exception*>::value, \
				  #T " is not catchable as std::exception")

CYB_CATCHABLE(Exception);
CYB_CATCHABLE(FileException);
CYB_CATCHABLE(FormatException);
CYB_CATCHABLE(XMLException);
CYB_CATCHABLE(CybMLException);
CYB_CATCHABLE(ActionException);
CYB_CATCHABLE(MetainformationException);
CYB_CATCHABLE(ParametersException);
CYB_CATCHABLE(NotFoundException);
CYB_CATCHABLE(AssertException);
CYB_CATCHABLE(NotImplementedException);

int main(int, char** argv)
{
	try {
		// a state action not matching the action grammar
		try {
			LocalDocument ld;
			ld.open(string(argv[0]) + "-input.graphml");
			return 1;
		} catch (const Cyberiada::CybMLException&) {
		}

		// an edge action not matching the action grammar
		try {
			LocalDocument ld;
			ld.open(string(argv[0]) + "-input2.graphml");
			return 1;
		} catch (const Cyberiada::CybMLException&) {
		}

		// a metainformation line without the separator
		try {
			LocalDocument ld;
			ld.open(string(argv[0]) + "-input3.graphml");
			return 1;
		} catch (const Cyberiada::MetainformationException&) {
		}

		// an unsupported version of the standard
		try {
			LocalDocument ld;
			ld.open(string(argv[0]) + "-input4.graphml");
			return 1;
		} catch (const Cyberiada::MetainformationException&) {
		}

		// the same exception seen through the standard base
		try {
			throw FileException("file.graphml");
		} catch (const std::exception& e) {
			CYB_ASSERT(String(e.what()).find("file.graphml") != String::npos);
		}
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
