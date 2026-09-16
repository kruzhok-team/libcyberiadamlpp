/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The optional metainformation parameters test
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
		// the absent parameters are not restored on load
		LocalDocument d;
		d.open(string(argv[0]) + "-input.graphml", formatCyberiada10, geometryFormatNone);
		CYB_ASSERT(d.meta().transition_order == transitionOrderNone);
		CYB_ASSERT(d.meta().event_propagation == docEventPropagationNone);
		cout << Document(d) << endl;

		// ... and are not added to the saved document
		LocalDocument saved(d, string(argv[0]) + ".graphml");
		saved.save();
		LocalDocument d1;
		d1.open(string(argv[0]) + ".graphml", formatCyberiada10, geometryFormatNone);
		CYB_ASSERT(d1.meta().transition_order == transitionOrderNone);
		CYB_ASSERT(d1.meta().event_propagation == docEventPropagationNone);

		// the legacy transitionFirst value is read as the action order
		LocalDocument d2;
		d2.open(string(argv[0]) + "-input2.graphml", formatCyberiada10, geometryFormatNone);
		CYB_ASSERT(d2.meta().transition_order == transitionOrderAction);
		CYB_ASSERT(d2.meta().event_propagation == docEventPropagationPropagate);
		// the standard value is written back
		String buffer;
		d2.encode(buffer);
		CYB_ASSERT(buffer.find("transitionOrder/ actionFirst") != String::npos);
		CYB_ASSERT(buffer.find("transitionFirst") == String::npos);

		// a single parameter survives the round trip
		LocalDocument d3;
		d3.open(string(argv[0]) + "-input3.graphml", formatCyberiada10, geometryFormatNone);
		CYB_ASSERT(d3.meta().transition_order == transitionOrderExit);
		CYB_ASSERT(d3.meta().event_propagation == docEventPropagationNone);
		cout << Document(d3) << endl;

		// a document created from scratch carries both parameters
		Document d4;
		d4.new_state_machine("sm", "SM");
		CYB_ASSERT(d4.meta().transition_order == transitionOrderAction);
		CYB_ASSERT(d4.meta().event_propagation == docEventPropagationBlock);
		d4.encode(buffer);
		CYB_ASSERT(buffer.find("transitionOrder/ actionFirst") != String::npos);
		CYB_ASSERT(buffer.find("eventPropagation/ block") != String::npos);
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
