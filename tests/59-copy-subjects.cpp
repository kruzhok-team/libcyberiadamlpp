/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The copied comment subjects test: the copy points into itself
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
		LocalDocument* ld = new LocalDocument();
		ld->open(string(argv[0]) + "-input.graphml");
		StateMachine* sm = ld->get_state_machines().front();
		State* state = static_cast<State*>(sm->find_elements_by_type(elementSimpleState).front());
		Comment* comment = ld->new_comment(sm, "A note");
		ld->add_comment_to_element(comment, state);
		ld->add_comment_to_element_name(comment, state, state->get_name());
		ld->add_comment_to_element_body(comment, state, "entry");
		const ID state_id = state->get_id();
		const ID comment_id = comment->get_id();

		// the copies point into themselves: the original may go
		Document* d1 = new Document(*ld);
		LocalDocument* ld2 = new LocalDocument(*ld);
		Element* e3 = ld->copy(NULL);
		delete ld;

		Document* copies[] = { d1, ld2, static_cast<Document*>(e3) };
		for (int c = 0; c < 3; c++) {
			Document* d = copies[c];
			Element* own_state = d->find_element_by_id(state_id);
			Comment* own_comment = static_cast<Comment*>(d->find_element_by_id(comment_id));
			CYB_ASSERT(own_state && own_comment);
			const std::vector<CommentSubject>& subjects = own_comment->get_subjects();
			CYB_ASSERT(subjects.size() == 3);
			for (size_t i = 0; i < subjects.size(); i++) {
				CYB_ASSERT(subjects[i].get_element() == own_state);
			}
			String buffer;
			d->encode(buffer);
			CYB_ASSERT(!buffer.empty());
		}
		cout << *d1 << endl;

		// a subtree copy keeps the targets outside it and re-binds those inside
		State* composite = static_cast<State*>(d1->find_elements_by_type(elementCompositeState).front());
		State* inner = static_cast<State*>(composite->find_elements_by_type(elementSimpleState).front());
		Element* outer = d1->find_element_by_id(state_id);
		CYB_ASSERT(outer->get_parent() != composite);
		Comment* nested = d1->new_comment(composite, "A nested note");
		d1->add_comment_to_element(nested, inner);
		d1->add_comment_to_element(nested, outer);
		State* copied = static_cast<State*>(composite->copy(composite->get_parent()));
		Comment* copied_note = static_cast<Comment*>(copied->find_element_by_id(nested->get_id()));
		CYB_ASSERT(copied_note);
		CYB_ASSERT(copied_note->get_subjects()[0].get_element() == copied->find_element_by_id(inner->get_id()));
		CYB_ASSERT(copied_note->get_subjects()[0].get_element() != inner);
		CYB_ASSERT(copied_note->get_subjects()[1].get_element() == outer);
		delete copied;

		// a subject that targets the moved subtree ROOT must re-bind to the copy:
		// the editor reparents by copy + rebind + remove-original + add-copy, and
		// find_element_by_id skips the root, so before the fix the subject dangled
		StateMachine* sm1 = d1->get_state_machines().front();
		Comment* rootnote = d1->new_comment(sm1, "points at the composite");
		d1->add_comment_to_element(rootnote, composite);
		ElementCollection* cparent = static_cast<ElementCollection*>(composite->get_parent());
		const ID composite_id = composite->get_id();
		State* moved = static_cast<State*>(composite->copy(cparent));
		d1->rebind_subjects(*moved);
		CYB_ASSERT(rootnote->get_subjects().back().get_element() == moved);
		CYB_ASSERT(rootnote->get_subjects().back().get_element() != composite);
		cparent->remove_element(composite_id);   // free the original, as move() does
		cparent->add_element(moved);
		String rebound;
		d1->encode(rebound);                     // a use-after-free before the fix
		CYB_ASSERT(!rebound.empty());

		delete d1;
		delete ld2;
		delete e3;
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
