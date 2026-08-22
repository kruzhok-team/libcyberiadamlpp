/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The elements mutation test
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
		Document d(geometryFormatQt);

		// the explicit id factories
		StateMachine* sm = d.new_state_machine("sm1", "SM");
		CYB_ASSERT(sm->get_id() == "sm1");
		State* s1 = d.new_state(sm, "s1", "State 1", Action(actionEntry, "init();"), Rect(0, 0, 100, 50));
		State* s2 = d.new_state(sm, "s2", "State 2");
		InitialPseudostate* init = d.new_initial(sm, "init1", "Init", Point(-10, -10));
		FinalState* fin = d.new_final(sm, "fin1", "Fin", Point(200, 0));
		ChoicePseudostate* choice = d.new_choice(sm, "ch1", "Choice", Rect(50, 100, 20, 20));
		TerminatePseudostate* term = d.new_terminate(sm, "term1", "Term", Point(300, 0));
		CYB_ASSERT(init->get_id() == "init1");
		CYB_ASSERT(fin->get_id() == "fin1");
		CYB_ASSERT(choice->get_id() == "ch1");
		CYB_ASSERT(term->get_id() == "term1");
		Transition* t1 = d.new_transition(sm, transitionExternal, "t1", init, s1, Action());
		CYB_ASSERT(t1->get_id() == "t1");

		// duplicate identifiers are not allowed
		try {
			d.new_state(sm, "s1", "Duplicate");
			return 1;
		} catch (const Cyberiada::ParametersException&) {
		}
		try {
			d.new_choice(sm, "ch1", "Duplicate");
			return 1;
		} catch (const Cyberiada::ParametersException&) {
		}
		try {
			d.new_transition(sm, transitionExternal, "t1", s1, s2, Action());
			return 1;
		} catch (const Cyberiada::ParametersException&) {
		}

		// the identifier change
		s2->set_id("s2x");
		CYB_ASSERT(d.find_element_by_id("s2x") == s2);
		CYB_ASSERT(d.find_element_by_id("s2") == NULL);

		// the collapsed flag
		s1->set_collapsed(true);
		CYB_ASSERT(s1->is_collapsed());
		s1->set_collapsed(false);
		CYB_ASSERT(!s1->is_collapsed());

		// the region and element geometry updates
		s1->update_region_geometry_rect(Rect(0, 0, 90, 40));
		CYB_ASSERT(s1->has_region_geometry());
		s1->update_geometry(Rect(10, 10, 120, 60));
		CYB_ASSERT(s1->get_geometry_rect() == Rect(10, 10, 120, 60));

		// adding a child makes the state composite
		CYB_ASSERT(s1->is_simple_state());
		d.new_state(s1, "sub1", "Substate");
		CYB_ASSERT(s1->is_composite_state());

		// the transition updates
		Transition* t2 = d.new_transition(sm, transitionLocal, "t2", s1, s2, Action("GO", "ready", "run();"));
		t2->update(Point(1, 2), Point(3, 4));
		CYB_ASSERT(t2->has_geometry_source_point());
		CYB_ASSERT(t2->has_geometry_target_point());
		Polyline pl;
		pl.push_back(Point(5, 6));
		pl.push_back(Point(7, 8));
		t2->update(pl);
		CYB_ASSERT(t2->has_polyline());
		t2->update(ID("s2x"), ID("s1"));
		CYB_ASSERT(t2->source_element_id() == "s2x");
		CYB_ASSERT(t2->target_element_id() == "s1");

		// the action updates
		Action& a = t2->get_action();
		a.update("STOP", "", "halt();", eventPropagationBlock);
		CYB_ASSERT(a.get_trigger() == "STOP");
		CYB_ASSERT(a.get_propagation() == eventPropagationBlock);
		a.update("finish();");
		CYB_ASSERT(a.get_behavior() == "finish();");

		cout << d << endl;
		LocalDocument(d, string(argv[0]) + ".graphml").save();

		// the removal flips the state back to simple
		s1->remove_element("sub1");
		CYB_ASSERT(s1->is_simple_state());

		// the first element insertion
		InitialPseudostate* init0 = new InitialPseudostate(sm, "init0", Point(0, 0));
		sm->add_first_element(init0);
		CYB_ASSERT(sm->first_element() == init0);
		sm->remove_element("init0");

		// the action cleanup
		a.clear();
		CYB_ASSERT(!t2->has_action());

		// the collection cleanup
		sm->clear();
		CYB_ASSERT(sm->children_count() == 0);
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
