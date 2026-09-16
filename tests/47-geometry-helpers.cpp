/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The geometry and string helpers test
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

int main()
{
	try {
		// the point helpers
		const Point p_invalid;
		CYB_ASSERT(p_invalid.to_str() == "()");
		CYB_ASSERT(!p_invalid.round().valid);
		Point p(1.25f, 2.75f);
		CYB_ASSERT(p.to_str() == "(1.25; 2.75)");
		const Point& p_cref = p;
		Point pr = p_cref.round();
		CYB_ASSERT(pr.x == 1.0f && pr.y == 2.0f);
		p.round();
		CYB_ASSERT(p.to_str() == "(1; 2)");

		// the rect helpers
		const Rect r_invalid;
		CYB_ASSERT(r_invalid.to_str() == "()");
		CYB_ASSERT(!r_invalid.round().valid);
		Rect r1(0, 0, 10, 10);
		Rect r2(0, 0, 10, 10);
		CYB_ASSERT(r1 == r2);
		Rect r3(0, 0, 10.0005f, 10);
		CYB_ASSERT(r1 != r3);
		CYB_ASSERT(r1.almost_equal(r3));
		CYB_ASSERT(r_invalid.almost_equal(Rect()));
		CYB_ASSERT(!r_invalid.almost_equal(r1));
		CYB_ASSERT(!r1.almost_equal(r_invalid));
		CYB_ASSERT(r1 != r_invalid);
		Rect r4(1.25f, 1.75f, 2.25f, 2.75f);
		const Rect& r4_cref = r4;
		Rect r4r = r4_cref.round();
		CYB_ASSERT(r4r == Rect(1, 1, 2, 2));
		r4.round();
		CYB_ASSERT(r4 == r4r);

		// the polyline helpers
		Polyline pl;
		pl.push_back(Point(1.25f, 2.75f));
		pl.push_back(Point(3.25f, 4.75f));
		CYB_ASSERT(pl.to_str() == "[ (1.25; 2.75), (3.25; 4.75) ]");
		pl.round();
		CYB_ASSERT(pl.to_str() == "[ (1; 2), (3; 4) ]");

		// the rect expansion with the qt geometry (centered coordinates)
		Document dq(geometryFormatQt);
		Rect e1(0, 0, 10, 10);
		e1.expand(Point(), dq);
		CYB_ASSERT(e1 == Rect(0, 0, 10, 10));
		e1.expand(Point(10, 0), dq);
		CYB_ASSERT(e1.almost_equal(Rect(2.5f, 0, 15, 10)));
		e1.expand(Rect(-20, 0, 2, 2), dq);
		CYB_ASSERT(e1.almost_equal(Rect(-5.5f, 0, 31, 10)));
		Rect e3;
		e3.expand(Point(5, 5), dq);
		CYB_ASSERT(e3.almost_equal(Rect(5, 5, 0, 0)));
		Rect e4;
		e4.expand(Rect(1, 2, 3, 4), dq);
		CYB_ASSERT(e4 == Rect(1, 2, 3, 4));

		// the rect expansion with the cyberiada geometry (left-top coordinates)
		Document dc(geometryFormatCyberiada10);
		Rect e2(0, 0, 10, 10);
		e2.expand(Point(15, -5), dc);
		CYB_ASSERT(e2.almost_equal(Rect(0, -5, 15, 15)));
		e2.expand(Rect(20, 20, 5, 5), dc);
		CYB_ASSERT(e2.almost_equal(Rect(0, -5, 25, 30)));
		Polyline pl2;
		pl2.push_back(Point(30, 0));
		pl2.push_back(Point(-10, -10));
		e2.expand(pl2, dc);
		CYB_ASSERT(e2.almost_equal(Rect(-10, -10, 40, 35)));

		// the action strings
		CYB_ASSERT(Action().to_str() == "");
		CYB_ASSERT(Action("EVENT", "g", "b();").to_str() == "trigger: 'EVENT', guard: 'g', behavior: 'b();'");
		CYB_ASSERT(Action(actionEntry, "init();").to_str() == "entry, behavior: 'init();'");
		CYB_ASSERT(Action(actionExit).to_str() == "exit");
		CYB_ASSERT(Action("EV", "", "b();", eventPropagationDefer).to_str() ==
				   "trigger: 'EV', propagation: 'defer', behavior: 'b();'");

		// a transition action may be updated to guard-only or behaviour-only
		// (a choice branch, an initial/completion edge); only a fully empty
		// update is refused
		Action tr("EVENT", "", "b();");
		tr.update("", "g", "c();");
		CYB_ASSERT(tr.get_trigger() == "" && tr.get_guard() == "g" && tr.get_behavior() == "c();");
		tr.update("", "", "");
		CYB_ASSERT(tr.get_behavior() == "c();");

		// guards are not allowed in the entry/exit activities
		Document d(geometryFormatQt);
		StateMachine* sm = d.new_state_machine("sm1", "SM");
		State* s1 = d.new_state(sm, "s1", "State 1");
		Action bad(actionEntry, "b();");
		bad.update("entry", "guard", "b();");
		try {
			s1->add_action(bad);
			return 1;
		} catch (const Cyberiada::ParametersException&) {
		}

		// the geometry cleanup cascade
		State* s2 = d.new_state(sm, "s2", "State 2", Action(), Rect(0, 0, 50, 50), Rect(0, 0, 40, 40));
		d.new_state(s2, "sub1", "Substate", Action(), Rect(0, 0, 10, 10));
		InitialPseudostate* init = d.new_initial(sm, Point(-10, 0));
		d.new_final(sm, Point(100, 0));
		d.new_choice(sm, Rect(50, 50, 20, 20));
		d.new_terminate(sm, Point(120, 0));
		Comment* cm = d.new_comment(sm, "A comment", Rect(150, 0, 60, 30));
		d.add_comment_to_element(cm, s2, Point(0, 0), Point(10, 10));
		Polyline pl3;
		pl3.push_back(Point(0, 0));
		pl3.push_back(Point(5, 5));
		d.new_transition(sm, transitionExternal, init, s2, Action(), pl3, Point(0, 0), Point(1, 1));
		CYB_ASSERT(d.has_geometry());
		d.clean_geometry();
		CYB_ASSERT(!d.has_geometry());
		CYB_ASSERT(d.get_geometry_format() == geometryFormatNone);
		CYB_ASSERT(!s2->has_geometry());
		CYB_ASSERT(!s2->has_region_geometry());
		CYB_ASSERT(!init->has_geometry());
		CYB_ASSERT(!cm->has_geometry());
		CYB_ASSERT(!cm->get_subjects().front().has_geometry());
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
