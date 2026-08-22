/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The comments mutation test
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
		StateMachine* sm = d.new_state_machine("sm1", "SM");
		State* s1 = d.new_state(sm, "s1", "State 1", Action(actionEntry, "init();"));
		State* s2 = d.new_state(sm, "s2", "State 2");

		// the comment factories
		Comment* c1 = d.new_comment(sm, "The first comment", Rect(0, 0, 100, 40));
		Comment* c2 = d.new_comment(sm, "note2", "The second comment", Rect(0, 60, 100, 40));
		Comment* c3 = d.new_comment(sm, "c3", "note3", "The third comment", Rect(0, 120, 100, 40));
		CYB_ASSERT(c3->get_id() == "c3");
		Comment* f1 = d.new_formal_comment(sm, "key/ value");
		Comment* f2 = d.new_formal_comment(sm, "formal2", "key2/ value2");
		Comment* f3 = d.new_formal_comment(sm, "f3", "formal3", "key3/ value3");
		CYB_ASSERT(f1->is_machine_readable());
		CYB_ASSERT(f2->is_machine_readable());
		CYB_ASSERT(f3->get_id() == "f3");
		CYB_ASSERT(f3->is_machine_readable());

		// the comment subjects
		CommentSubject sub1 = d.add_comment_to_element(c1, s1, Point(0, 0), Point(10, 10));
		CommentSubject sub2 = d.add_comment_to_element(c1, s2, "subj2");
		CYB_ASSERT(sub2.get_id() == "subj2");
		Polyline pl;
		pl.push_back(Point(1, 2));
		pl.push_back(Point(3, 4));
		CommentSubject sub3 = d.add_comment_to_element_name(c2, s1, "State", Point(1, 2), Point(3, 4), pl);
		CommentSubject sub4 = d.add_comment_to_element_name(c2, s2, "ate", "subj4");
		CommentSubject sub5 = d.add_comment_to_element_body(c3, s1, "init");
		CommentSubject sub6 = d.add_comment_to_element_body(c3, s2, "nit", "subj6");
		CYB_ASSERT(sub1.get_type() == commentSubjectElement);
		CYB_ASSERT(sub3.get_type() == commentSubjectName);
		CYB_ASSERT(sub3.has_fragment());
		CYB_ASSERT(sub3.has_polyline());
		CYB_ASSERT(sub5.get_type() == commentSubjectData);
		CYB_ASSERT(sub6.get_id() == "subj6");
		CYB_ASSERT(c1->has_subjects());
		CYB_ASSERT(c1->get_subjects().size() == 2);

		// the subject copy and assignment
		CommentSubject cs(sub1);
		cs = sub3;
		CYB_ASSERT(cs.get_id() == sub3.get_id());
		CYB_ASSERT(cs.get_fragment() == "State");
		cout << cs.to_str() << endl;

		// the comment body update
		c1->set_body("The updated comment");
		CYB_ASSERT(c1->get_body() == "The updated comment");

		// commenting the state machine is not allowed
		try {
			d.add_comment_to_element(c1, sm);
			return 1;
		} catch (const Cyberiada::ParametersException&) {
		}

		cout << d << endl;
		LocalDocument(d, string(argv[0]) + ".graphml").save();

		// the subject removal
		c2->remove_subject(commentSubjectName, "State");
		CYB_ASSERT(c2->get_subjects().size() == 1);
		CYB_ASSERT(c2->get_subjects().front().get_fragment() == "ate");
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
