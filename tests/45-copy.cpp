/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The elements copy test
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
		LocalDocument ld;
		ld.open(string(argv[0]) + "-input.graphml");
		StateMachine* sm = ld.get_state_machines().front();

		// augment the diagram so every element kind is copied
		State* state = static_cast<State*>(sm->find_elements_by_type(elementSimpleState).front());
		ld.new_final(sm);
		ld.new_terminate(sm);
		ld.new_choice(sm);
		Comment* comment = ld.new_comment(sm, "A comment");
		ld.add_comment_to_element(comment, state);
		ld.add_comment_to_element_name(comment, state, state->get_name());

		// the document copy constructor copies every element
		Document d1(ld);
		Document d2(d1);
		CYB_ASSERT(d2.dump_to_str() == d1.dump_to_str());
		CYB_ASSERT(d2.elements_count() == d1.elements_count());
		const StateMachine* sm1 = d1.get_state_machines().front();
		const StateMachine* sm2 = d2.get_state_machines().front();
		CYB_ASSERT(sm1->check_isomorphism(*sm2, false) == smiIdentical);

		// the local document copies
		LocalDocument ld2(ld);
		CYB_ASSERT(ld2.dump_to_str() == ld.dump_to_str());
		CYB_ASSERT(ld2.get_file_path() == ld.get_file_path());
		Element* ldc = ld.copy(NULL);
		CYB_ASSERT(ldc->dump_to_str() == ld.dump_to_str());
		delete ldc;

		// the individual element copies
		Element* smc = sm1->copy(NULL);
		CYB_ASSERT(smc->get_id() == sm1->get_id());
		CYB_ASSERT(smc->elements_count() == sm1->elements_count());
		delete smc;

		Element* st = d1.find_elements_by_type(elementSimpleState).front();
		Element* stc = st->copy(st->get_parent());
		CYB_ASSERT(stc->get_id() == st->get_id());
		CYB_ASSERT(stc->dump_to_str() == st->dump_to_str());
		delete stc;

		Transition* tr = d1.get_state_machines().front()->get_transitions().front();
		Element* trc = tr->copy(tr->get_parent());
		CYB_ASSERT(trc->get_id() == tr->get_id());
		delete trc;
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
