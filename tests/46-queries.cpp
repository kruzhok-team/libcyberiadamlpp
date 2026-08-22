/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The collection queries test
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
		LocalDocument ld;
		ld.open(string(argv[0]) + "-input.graphml");
		StateMachine* sm = ld.get_state_machines().front();
		CYB_ASSERT(!sm->has_initial());

		// augment the hierarchy with an initial, a transition and a comment
		State* p0 = static_cast<State*>(sm->find_element_by_id("n0"));
		CYB_ASSERT(p0);
		InitialPseudostate* init = ld.new_initial(sm);
		ld.new_transition(sm, transitionExternal, init, p0, Action());
		ld.new_comment(sm, "A comment");
		CYB_ASSERT(sm->has_initial());

		// the element access by index
		cout << "children: " << sm->children_count() << endl;
		cout << "elements: " << ld.elements_count() << endl;
		const Element* first = sm->first_element();
		CYB_ASSERT(first == sm->get_element(0));
		CYB_ASSERT(sm->get_element(int(sm->children_count())) == NULL);
		CYB_ASSERT(sm->element_index(first) == 0);
		CYB_ASSERT(first->index() == 0);
		cout << "first: " << first->get_id() << endl;
		Element* last = sm->get_element(int(sm->children_count()) - 1);
		CYB_ASSERT(last->index() == int(sm->children_count()) - 1);
		cout << "last: " << last->get_id() << endl;

		// the children lists
		ConstElementList cc = static_cast<const StateMachine*>(sm)->get_children();
		const ElementList& mc = sm->get_children();
		CYB_ASSERT(cc.size() == mc.size());

		// the deep search by identifier
		const Element* deep = ld.find_element_by_id("n0::n1::n0");
		CYB_ASSERT(deep);
		cout << "deep: " << deep->qualified_name() << endl;
		CYB_ASSERT(deep->has_qualified_name());
		CYB_ASSERT(ld.find_element_by_id("nonexistent") == NULL);
		Element* deep2 = ld.find_element_by_id("n0::n1::n1");
		CYB_ASSERT(deep2);
		cout << "deep2: " << deep2->qualified_name() << endl;

		// the vertexes
		std::vector<const Vertex*> cv = static_cast<const StateMachine*>(sm)->get_vertexes();
		std::vector<Vertex*> mv = sm->get_vertexes();
		CYB_ASSERT(cv.size() == mv.size());
		cout << "vertexes: " << cv.size() << endl;

		// the substates
		std::vector<const State*> substates = static_cast<const State*>(p0)->get_substates();
		std::vector<State*> msubstates = p0->get_substates();
		CYB_ASSERT(substates.size() == msubstates.size());
		cout << "substates:";
		for (size_t i = 0; i < substates.size(); i++) {
			cout << " '" << substates[i]->get_name() << "'";
		}
		cout << endl;

		// the comments and transitions
		cout << "comments: " << sm->get_comments().size()
			 << " transitions: " << sm->get_transitions().size() << endl;
		cout << "const comments: " << static_cast<const StateMachine*>(sm)->get_comments().size()
			 << " const transitions: " << static_cast<const StateMachine*>(sm)->get_transitions().size() << endl;

		// the parent state machine
		CYB_ASSERT(ld.get_parent_sm(deep) == sm);
		CYB_ASSERT(ld.get_parent_sm(sm) == sm);
		CYB_ASSERT(ld.get_parent_sm(&ld) == NULL);
		CYB_ASSERT(ld.get_parent_sm(NULL) == NULL);
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
