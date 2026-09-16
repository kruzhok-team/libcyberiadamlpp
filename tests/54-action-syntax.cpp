/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The action syntax test
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
#include <vector>
#include "cyberiadamlpp.h"
#include "testutils.h"

using namespace Cyberiada;
using namespace std;

int main(int, char** argv)
{
	try {
		LocalDocument d;
		d.open(string(argv[0]) + "-input.graphml", formatCyberiada10, geometryFormatNone);
		const StateMachine* sm = d.get_state_machines().front();
		const State* s = static_cast<const State*>(sm->find_element_by_id("n0"));
		CYB_ASSERT(s);
		const std::vector<Action>& actions = s->get_actions();
		CYB_ASSERT(actions.size() == 3);

		// the defer keyword follows the action separator (6.8)
		CYB_ASSERT(actions[0].get_trigger() == "TICK");
		CYB_ASSERT(actions[0].get_propagation() == eventPropagationDefer);
		CYB_ASSERT(actions[0].get_behavior() == "count();");

		// the keyword before the separator is accepted as well
		CYB_ASSERT(actions[1].get_trigger() == "TOCK");
		CYB_ASSERT(actions[1].get_propagation() == eventPropagationDefer);

		// the event name may be empty
		CYB_ASSERT(!actions[2].has_trigger());
		CYB_ASSERT(actions[2].get_behavior() == "plain();");

		// the escaped brackets are kept in the guard
		const Transition* t = static_cast<const Transition*>(sm->find_element_by_id("t0"));
		CYB_ASSERT(t);
		CYB_ASSERT(t->get_action().get_guard() == "a \\[b\\] c");

		cout << Document(d) << endl;

		// the canonical form is written back and read again
		LocalDocument saved(d, string(argv[0]) + ".graphml");
		saved.save();
		LocalDocument d2;
		d2.open(string(argv[0]) + ".graphml", formatCyberiada10, geometryFormatNone);
		const State* s2 = static_cast<const State*>(d2.get_state_machines().front()->find_element_by_id("n0"));
		CYB_ASSERT(s2);
		CYB_ASSERT(s2->get_actions()[1].get_propagation() == eventPropagationDefer);
		CYB_ASSERT(d.get_state_machines().front()->check_isomorphism(*(d2.get_state_machines().front()),
																	 false) == smiIdentical);
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
