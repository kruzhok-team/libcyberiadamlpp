/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The isomorhism check test
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
#include <vector>
#include "cyberiadamlpp.h"
#include "testutils.h"

using namespace Cyberiada;
using namespace std;

int main(int argc, char** argv)
{
	try {
		LocalDocument d1, d2, d3, d4, d5;
		d1.open(string(argv[0]) + "-graph1.graphml");
		//cout << d1 << endl;
		d2.open(string(argv[0]) + "-graph2.graphml");
		//cout << d2 << endl;
		const StateMachine* sm1 = d1.get_state_machines().front();
		const StateMachine* sm2 = d2.get_state_machines().front();
		CYB_ASSERT(sm1->check_isomorphism(*sm2) == smiIdentical);
		d3.open(string(argv[0]) + "-graph3.graphml");
		const StateMachine* sm3 = d3.get_state_machines().front();
		//cout << d3 << endl;
		CYB_ASSERT(sm1->check_isomorphism(*sm3) == smiIsomorphic);
		d4.open(string(argv[0]) + "-graph4.graphml");
		d5.open(string(argv[0]) + "-graph5.graphml");
		
		vector<ID> diff_nodes;
		vector<SMIsomorphismFlagsResult> diff_nodes_flags;
		
		const StateMachine* sm4 = d4.get_state_machines().front();
		const StateMachine* sm5 = d5.get_state_machines().front();
		CYB_ASSERT(sm4->check_isomorphism_details(*sm5, true, false, NULL, &diff_nodes, &diff_nodes_flags) == smiIsomorphic);
		CYB_ASSERT(diff_nodes.size() == 6);
		CYB_ASSERT(diff_nodes[2] == "node-0-0-0");
		CYB_ASSERT(diff_nodes_flags.size() == 6);
		CYB_ASSERT(diff_nodes_flags[3] | smiNodeDiffFlagTitle);
		for (size_t i = 0; i < diff_nodes_flags.size(); i++) {
			CYB_ASSERT(diff_nodes_flags[i] | smiNodeDiffFlagID);
		}
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
