/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The comment subjects on transitions test
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

int main(int, char** argv)
{
	try {
		// the comment subject refers to a transition (8.5)
		LocalDocument d;
		d.open(string(argv[0]) + "-input.graphml", formatCyberiada10, geometryFormatNone);
		StateMachine* sm = d.get_state_machines().front();
		const Comment* note = static_cast<const Comment*>(sm->find_element_by_id("n2"));
		CYB_ASSERT(note);
		CYB_ASSERT(note->get_subjects().size() == 1);
		const CommentSubject& subject = note->get_subjects().front();
		CYB_ASSERT(subject.get_type() == commentSubjectElement);
		CYB_ASSERT(subject.get_element()->get_type() == elementTransition);
		CYB_ASSERT(subject.get_element()->get_id() == "t0");
		cout << Document(d) << endl;

		// the edge order in the document does not matter
		LocalDocument d2;
		d2.open(string(argv[0]) + "-input2.graphml", formatCyberiada10, geometryFormatNone);
		const Comment* note2 = static_cast<const Comment*>(d2.get_state_machines().front()->find_element_by_id("n2"));
		CYB_ASSERT(note2);
		CYB_ASSERT(note2->get_subjects().front().get_element()->get_id() == "t0");

		// a new subject may be attached to a transition as well
		Comment* new_note = d.new_comment(sm, "A second note");
		Element* transition = sm->find_element_by_id("t0");
		CYB_ASSERT(transition);
		d.add_comment_to_element(new_note, transition);
		CYB_ASSERT(new_note->get_subjects().size() == 1);

		// the links survive the save/load round trip
		LocalDocument saved(d, string(argv[0]) + ".graphml");
		saved.save();
		LocalDocument d3;
		d3.open(string(argv[0]) + ".graphml", formatCyberiada10, geometryFormatNone);
		StateMachine* sm3 = d3.get_state_machines().front();
		const Comment* note3 = static_cast<const Comment*>(sm3->find_element_by_id("n2"));
		CYB_ASSERT(note3);
		CYB_ASSERT(note3->get_subjects().front().get_element()->get_id() == "t0");
		// the isomorphism check ignores the comments: the C library compares the node targets only
		CYB_ASSERT(sm->check_isomorphism(*sm3, true) == smiIdentical);
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
