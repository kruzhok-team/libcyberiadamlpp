/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The testing program
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
#include <stdlib.h>
#include "cyberiadamlpp.h"

using namespace Cyberiada;
using namespace std;

void usage(const char* program)
{
	cerr << program << " <print|convert> [[-f <cyberiada|yed>] -o <path-to-output-graphml-file>] <path-to-input-graphml-file>" << endl;
	cerr << "\tprint\tPrint the graphml SM structure of the file <path-to-input-graphml-file>" << endl;
	cerr << "\tconvert\tConvert the graphml SM file from <path-to-input-graphml-file> to <path-to-output-graphml-file> using format:" << endl;
	cerr << "\t\t\tcyberiada   Cyberiada-GraphML 1.0 format" << endl;
	cerr << "\t\t\tyed         Legacy Berloga-YED format" << endl;
	exit(1);
}

int main(int argc, char** argv)
{
	Document d;
	DocumentFormat format;
	string command, from_file, to_file, format_str;
	
	if (argc < 3) {
		usage(argv[0]);
	}

	command = argv[1];
	if (command == "print" && argc == 3) {
		from_file = argv[2];
	} else if (command == "convert" && (argc == 5 || argc == 7)) {
		if (argc == 5 && string(argv[2]) == "-o") {
			to_file = argv[3];
			from_file = argv[4];
		} else if (argc == 7) {
			if (string(argv[2]) == "-f" && string(argv[4]) == "-o") {
				to_file = argv[5];
				format_str = argv[3];
			} else if (string(argv[2]) == "-o" && string(argv[4]) == "-f") {
				to_file = argv[3];
				format_str = argv[5];
			} else {
				usage(argv[0]);
			}
			from_file = argv[6];
		} else {
			usage(argv[0]);
		}
		if (format_str.empty() || format_str == "cyberiada") {
			format = formatCyberiada10;
		} else if (format_str == "yes") {
			format = formatLegacyYED;
		} else {
			usage(argv[0]);
		}
	} else {
		usage(argv[0]);
	}
	
	try {
		d.load(from_file);
		if (argc == 3) {
			cout << d << endl;
		} else {
			d.save(to_file, format);
		}
	} catch (const Cyberiada::Exception& e) {
		cerr << "Error while processing graphml file: " << e.str() << endl;
		return 2;
	}
	return 0;
}
