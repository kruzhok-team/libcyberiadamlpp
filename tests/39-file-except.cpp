/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The file exceptions test
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
		// opening a nonexistent file
		try {
			LocalDocument ld;
			ld.open(string(argv[0]) + "-nonexistent.graphml");
			return 1;
		} catch (const Cyberiada::FileException&) {
		}

		// opening an empty file
		try {
			LocalDocument ld;
			ld.open(string(argv[0]) + "-empty.graphml");
			return 1;
		} catch (const Cyberiada::FileException&) {
		}

		// saving to an unavailable path
		try {
			Document d;
			d.new_state_machine("SM");
			LocalDocument ld(d, "/nonexistent-dir/out.graphml");
			ld.save();
			return 1;
		} catch (const Cyberiada::FileException&) {
		}

		// the exception messages
		CYB_ASSERT(FileException("boom").str() == "File Exception: boom");
		CYB_ASSERT(FormatException("boom").str() == "Format Exception: boom");
		CYB_ASSERT(XMLException("boom").str() == "XML Exception: boom");
		CYB_ASSERT(CybMLException("boom").str() == "CyberiadaML Exception: boom");
		CYB_ASSERT(ActionException("boom").str() == "Action Exception: boom");
		CYB_ASSERT(MetainformationException("boom").str() == "Metainfo Exception: boom");
		CYB_ASSERT(NotFoundException("boom").str() == "Not Found Exception: boom");
		CYB_ASSERT(NotImplementedException("boom").str() == "Not Implemented Exception: boom");

		// the exceptions hierarchy
		try {
			throw NotFoundException("x");
		} catch (const Cyberiada::ParametersException&) {
		}
		try {
			throw XMLException("x");
		} catch (const Cyberiada::FormatException&) {
		}
		try {
			throw MetainformationException("x");
		} catch (const Cyberiada::FormatException&) {
		}
		try {
			throw ActionException("x");
		} catch (const Cyberiada::CybMLException&) {
		}
		try {
			throw NotImplementedException("x");
		} catch (const Cyberiada::Exception&) {
		}
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
