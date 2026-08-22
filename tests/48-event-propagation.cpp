/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The event propagation keywords test
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

int main(int argc, char** argv)
{
	try {
		LocalDocument d;
		d.open(string(argv[0]) + "-input.graphml");

		const StateMachine* sm = d.get_state_machines().front();

		// the defer keyword is imported into the state action
		const State* waiting = static_cast<const State*>(sm->find_element_by_id("n0"));
		CYB_ASSERT(waiting);
		const vector<Action>& actions = waiting->get_actions();
		CYB_ASSERT(actions.size() == 2);
		CYB_ASSERT(!actions[0].has_propagation());
		CYB_ASSERT(actions[1].has_propagation());
		CYB_ASSERT(actions[1].get_propagation() == eventPropagationDefer);

		// the keywords are imported into the transition actions
		vector<const Transition*> transitions = sm->get_transitions();
		CYB_ASSERT(transitions.size() == 4);
		for (vector<const Transition*>::const_iterator i = transitions.begin();
			 i != transitions.end(); i++) {
			const Transition* t = *i;
			const Action& a = t->get_action();
			if (t->get_id() == "n0-n1") {
				CYB_ASSERT(a.get_propagation() == eventPropagationPropagate);
			} else if (t->get_id() == "n1-n2") {
				CYB_ASSERT(a.get_propagation() == eventPropagationBlock);
			} else if (t->get_id() == "n2-n0") {
				CYB_ASSERT(a.get_propagation() == eventPropagationDefer);
			} else {
				CYB_ASSERT(!a.has_propagation());
			}
		}

		Document doc(d);
		cout << doc << endl;

		// the keywords survive the save/load round trip
		LocalDocument saved(d, string(argv[0]) + ".graphml");
		saved.save();
		LocalDocument d2;
		d2.open(string(argv[0]) + ".graphml");
		const StateMachine* sm2 = d2.get_state_machines().front();
		CYB_ASSERT(sm->check_isomorphism(*sm2) == smiIdentical);

		// the propagation difference is reported by the actions compare
		Document pd;
		StateMachine* psm = pd.new_state_machine("SM");
		State* s1 = pd.new_state(psm, "State 1");
		State* s2 = pd.new_state(psm, "State 2");
		s1->add_action(Action("EVENT", "", "action();", eventPropagationBlock));
		s2->add_action(Action("EVENT", "", "action();", eventPropagationPropagate));
		ActionsDiffFlags f = s1->compare_actions(*s2);
		CYB_ASSERT(f & adiffPropagation);
		CYB_ASSERT(!(f & adiffNumber));
		CYB_ASSERT(!(f & adiffGuards));
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
