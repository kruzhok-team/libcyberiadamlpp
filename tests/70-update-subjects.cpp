/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The test: the comment subjects changed in place - the geometry, the target,
 * the type and the fragment
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

static bool same(const Point& p, double x, double y)
{
	return p.valid && p.x == x && p.y == y;
}

static bool refused(Document& d, Comment* c, size_t index, Element* element,
					CommentSubjectType type = commentSubjectElement, const String& fragment = String())
{
	try {
		d.update_comment_subject(c, index, element, type, fragment);
	} catch (const Cyberiada::ParametersException&) {
		return true;
	}
	return false;
}

int main(int, char** argv)
{
	try {
		Document d(geometryFormatQt);
		StateMachine* sm = d.new_state_machine("sm1", "SM", Rect(0, 0, 600, 400));
		State* s1 = d.new_state(sm, "s1", "State 1", Action(), Rect(-150, -100, 100, 50));
		State* s2 = d.new_state(sm, "s2", "State 2", Action(actionEntry, "init();"), Rect(150, -100, 100, 50));
		Transition* t = d.new_transition(sm, transitionExternal, s1, s2, Action("GO"));
		Comment* c = d.new_comment(sm, "A note", Rect(0, 100, 100, 40));
		Polyline pl;
		pl.push_back(Point(-20, -60));

		d.add_comment_to_element(c, s1, Point(-50, 0), Point(0, 25));
		d.add_comment_to_element(c, t, Point(0, -20), Point(150, 0));
		CYB_ASSERT(c->get_subjects()[0].get_id() == c->get_id() + "-s1");

		// the geometry in place
		c->update_subject_geometry(0, Point(-50, -20), Point(25, 25), pl);
		const CommentSubject& g = c->get_subjects()[0];
		CYB_ASSERT(same(g.get_geometry_source_point(), -50, -20));
		CYB_ASSERT(same(g.get_geometry_target_point(), 25, 25));
		CYB_ASSERT(g.get_geometry_polyline().size() == 1);
		cout << "geometry: " << c->get_subjects()[0] << endl;

		// a new target: a new id, the target point dropped, the source and the order kept
		const CommentSubject& r = d.update_comment_subject(c, 0, s2);
		CYB_ASSERT(r.get_id() == c->get_id() + "-s2");
		CYB_ASSERT(r.get_element() == s2);
		CYB_ASSERT(!r.has_geometry_target_point());
		CYB_ASSERT(same(r.get_geometry_source_point(), -50, -20));
		CYB_ASSERT(c->get_subjects().size() == 2 && c->get_subjects()[1].get_element() == t);
		cout << "retarget: " << c->get_subjects()[0] << endl;

		// the same target as a fragment: the id stays, no target point
		const CommentSubject& f = d.update_comment_subject(c, 0, s2, commentSubjectData, "init");
		CYB_ASSERT(f.get_id() == c->get_id() + "-s2");
		CYB_ASSERT(f.get_type() == commentSubjectData && f.get_fragment() == "init");
		c->update_subject_geometry(0, Point(-50, 0), Point(10, 10));
		CYB_ASSERT(c->get_subjects()[0].has_geometry_source_point());
		CYB_ASSERT(!c->get_subjects()[0].has_geometry_target_point());
		cout << "fragment: " << c->get_subjects()[0] << endl;

		// back to the element: the same target keeps no stale point
		d.update_comment_subject(c, 0, s2);
		CYB_ASSERT(c->get_subjects()[0].get_type() == commentSubjectElement);
		CYB_ASSERT(!c->get_subjects()[0].has_fragment());

		// the refusals
		CYB_ASSERT(refused(d, c, 0, sm));
		CYB_ASSERT(refused(d, c, 0, c));
		CYB_ASSERT(refused(d, c, 2, s1));
		CYB_ASSERT(refused(d, c, 0, s1, commentSubjectName, ""));
		try {
			c->update_subject_geometry(5, Point(0, 0));
			return 1;
		} catch (const Cyberiada::ParametersException&) {
		}

		cout << d << endl;
		LocalDocument(d, string(argv[0]) + ".graphml").save();
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
