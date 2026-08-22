/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The comment subjects test
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

		// the subject edges are imported into the comment
		const Comment* note = static_cast<const Comment*>(sm->find_element_by_id("n2"));
		CYB_ASSERT(note);
		CYB_ASSERT(note->get_type() == elementComment);
		CYB_ASSERT(note->has_subjects());
		const vector<CommentSubject>& subjects = note->get_subjects();
		CYB_ASSERT(subjects.size() == 3);
		CYB_ASSERT(subjects[0].get_type() == commentSubjectElement);
		CYB_ASSERT(!subjects[0].has_fragment());
		CYB_ASSERT(subjects[0].get_element()->get_id() == "n0");
		CYB_ASSERT(subjects[1].get_type() == commentSubjectName);
		CYB_ASSERT(subjects[1].get_fragment() == "Second");
		CYB_ASSERT(subjects[1].get_element()->get_id() == "n1");
		CYB_ASSERT(subjects[2].get_type() == commentSubjectData);
		CYB_ASSERT(subjects[2].get_fragment() == "run");
		CYB_ASSERT(subjects[2].get_element()->get_id() == "n0");

		Document doc(d);
		cout << doc << endl;

		// the subjects survive the save/load round trip
		LocalDocument saved(d, string(argv[0]) + ".graphml");
		saved.save();
		LocalDocument d2;
		d2.open(string(argv[0]) + ".graphml");
		Comment* note2 = static_cast<Comment*>(d2.get_state_machines().front()->find_element_by_id("n2"));
		CYB_ASSERT(note2);
		CYB_ASSERT(note2->get_subjects().size() == 3);

		// removal by index works for an element-type subject
		note2->remove_subject(0);
		CYB_ASSERT(note2->get_subjects().size() == 2);
		CYB_ASSERT(note2->get_subjects()[0].get_type() == commentSubjectName);
		bool caught = false;
		try {
			note2->remove_subject(2);
		} catch (const ParametersException&) {
			caught = true;
		}
		CYB_ASSERT(caught);
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
