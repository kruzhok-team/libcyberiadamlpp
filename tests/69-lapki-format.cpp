/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The Lapki format test: the Lapki IDE dialect written for lapki-client and lapki-compiler
 *
 * Copyright (C) 2024 Alexey Fedoseev <aleksey@fedoseev.net>
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
#include <fstream>
#include <sstream>
#include "cyberiadamlpp.h"
#include "testutils.h"

using namespace Cyberiada;
using namespace std;

static string text(const string& path)
{
	ifstream f(path.c_str());
	stringstream s;
	s << f.rdbuf();
	return s.str();
}

int main(int, char** argv)
{
	try {
		string input = string(argv[0]) + "-input.graphml";
		string output = string(argv[0]) + "-lapki.graphml";

		// a document opened in the Lapki format is saved in it
		LocalDocument d;
		d.open(input, formatLapki);
		CYB_ASSERT(d.get_file_format() == formatLapki);
		CYB_ASSERT(d.get_file_format_str() == "Cyberiada-GraphML-Lapki");
		d.save_as(output, formatDetect);
		string written = text(output);
		CYB_ASSERT(written.find("<data key=\"gFormat\">Cyberiada-GraphML-1.0</data>") != string::npos);
		CYB_ASSERT(written.find("<data key=\"dName\">CGML_COMPONENT</data>") != string::npos);
		CYB_ASSERT(written.find("id/ LED1") != string::npos);

		// the detected document is Cyberiada; the Lapki format is chosen on saving
		LocalDocument detected;
		detected.open(output);
		CYB_ASSERT(detected.get_file_format() == formatCyberiada10);
		detected.save_as(output, formatCyberiada10);
		CYB_ASSERT(text(output).find("CGML_COMPONENT LED1") != string::npos);
		detected.save_as(output, formatLapki);
		CYB_ASSERT(detected.get_file_format() == formatLapki);
		CYB_ASSERT(text(output).find("CGML_COMPONENT LED1") == string::npos);
		cout << detected.get_file_format_str() << endl;
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
