/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The transition label geometry setter and the triggerless action test
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
		Document d;
		StateMachine* sm = d.new_state_machine("SM");
		State* a = d.new_state(sm, "A");
		State* b = d.new_state(sm, "B");
		Transition* t = d.new_transition(sm, transitionExternal, a, b, Action(actionTransition));

		// the label point setter: set, then clear (an invalid point resets it)
		CYB_ASSERT(!t->has_geometry_label_point());
		t->update_label(Point(12, -8));
		CYB_ASSERT(t->has_geometry_label_point());
		CYB_ASSERT(t->get_label_point().x == 12 && t->get_label_point().y == -8);
		t->update_label(Point());
		CYB_ASSERT(!t->has_geometry_label_point());

		// a transition action may be guard-only or behaviour-only, not only
		// trigger-bearing; only a fully empty one is refused
		t->get_action().update("", "x > 0", "");
		CYB_ASSERT(t->get_action().get_guard() == "x > 0");
		t->get_action().update("", "", "run()");
		CYB_ASSERT(t->get_action().get_behavior() == "run()");
		CYB_ASSERT(!t->get_action().has_trigger() && !t->get_action().has_guard());
		t->get_action().update("", "", "");                 // fully empty: a no-op
		CYB_ASSERT(t->get_action().get_behavior() == "run()");
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
