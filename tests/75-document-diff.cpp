/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML C++ library implemention
 *
 * The test of the revision diff of two documents and of two state machines
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

static const char* flag_names[] = {
	"added", "removed", "type", "title", "actions", "parent", "link", "comment", "color", "geometry",
	"collapsed", "source", "target", "trigger", "guard", "behavior", "propagation", "subject", "meta",
	"elements"
};

static const char* kind_names[] = {"node", "transition", "link"};

static string flags_str(DiffFlags flags)
{
	string s;
	for (size_t i = 0; i < sizeof(flag_names) / sizeof(*flag_names); i++) {
		if (flags & (1U << i)) {
			s += string(" ") + flag_names[i];
		}
	}
	return s;
}

static string side(const ID& id)
{
	return id.empty() ? "-" : id;
}

static void print(const DocumentDiff& d)
{
	cout << "document:" << flags_str(d.flags) << endl;
	for (size_t i = 0; i < d.meta.size(); i++) {
		const MetaDiff& m = d.meta[i];
		cout << " meta " << m.name << ": " << (m.in_first ? m.first : "-") << " -> "
			 << (m.in_second ? m.second : "-") << endl;
	}
	for (size_t i = 0; i < d.machines.size(); i++) {
		const StateMachineDiff& sm = d.machines[i];
		cout << " sm " << side(sm.first) << " " << side(sm.second) << ":" << flags_str(sm.flags) << endl;
		for (size_t j = 0; j < sm.elements.size(); j++) {
			const ElementDiff& e = sm.elements[j];
			if (!e.flags) continue;
			cout << "  " << kind_names[e.kind] << " " << side(e.first) << " " << side(e.second) << ":"
				 << flags_str(e.flags);
			if (e.kind == diffKindCommentLink) {
				cout << " (comment " << side(e.second_comment.empty() ? e.first_comment : e.second_comment) << ")";
			}
			cout << endl;
		}
	}
}

int main(int, char** argv)
{
	try {
		LocalDocument before, after;
		before.open(string(argv[0]) + "-before.graphml");
		after.open(string(argv[0]) + "-after.graphml");

		DocumentDiff same = before.diff(before);
		CYB_ASSERT(!same.changed());
		CYB_ASSERT(same.machines.size() == 1 && !same.machines[0].changed());

		cout << "== the edited revision" << endl;
		DocumentDiff d = before.diff(after);
		CYB_ASSERT(d.changed() && (d.flags & diffMeta));
		print(d);

		cout << "== with the geometry" << endl;
		DiffOptions geometry;
		geometry.geometry = true;
		print(before.diff(after, geometry));

		cout << "== without the comments" << endl;
		DiffOptions no_comments;
		no_comments.comments = false;
		print(before.diff(after, no_comments));

		cout << "== a new document" << endl;
		Document empty;
		DocumentDiff added = empty.diff(after);
		CYB_ASSERT(added.machines.size() == 1 && (added.machines[0].flags & diffAdded));
		for (size_t i = 0; i < added.machines[0].elements.size(); i++) {
			CYB_ASSERT(added.machines[0].elements[i].added());
		}
		cout << "elements added: " << added.machines[0].elements.size() << endl;

		cout << "== the state machines" << endl;
		StateMachineDiff smd = before.get_state_machines().front()->diff(*after.get_state_machines().front());
		CYB_ASSERT(smd.changed() && (smd.flags & diffElements));
		cout << "sm " << smd.first << " " << smd.second << ":" << flags_str(smd.flags) << endl;
	} catch (const Cyberiada::Exception& e) {
		cerr << e.str() << endl;
		return 1;
	}
	return 0;
}
