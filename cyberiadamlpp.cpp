/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML library implemention
 *
 * The C++ library implementation
 *
 * Copyright (C) 2024 Alexey Fedoseev <aleksey@fedoseev.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see https://www.gnu.org/licenses/
 *
 * ----------------------------------------------------------------------------- */

#include <algorithm>
#include <sstream>
#include "cyberiadamlpp.h"

#define CYB_CHECK_RESULT(r) this->check_cyberiada_error((r), std::string(__FILE__) + ":" + std::to_string(__LINE__))

#define CYB_ASSERT(q)   if (!(q)) {                                      \
							throw AssertException(std::string(__FILE__) + ":" + std::to_string(__LINE__)); \
	                    }

namespace Cyberiada {

	static const String STANDARD_VERSION = "1.0";	
	static const String DEFAULT_GRAPHML_FORMAT = "Cyberiada-GraphML-1.0";
	static const String META_NODE_NAME = "CGML_META";
	static const String META_NODE_ID = "nMeta";
	static const String VERTEX_ID_PREFIX = "n";
	static const String SM_ID_PREFIX = "G";
	static const String TRANTISION_ID_SEP = "-";
	static const String TRANTISION_ID_NUM_SEP = "#"; 
	static const std::string tab = "\t";
};
	
using namespace Cyberiada;

// -----------------------------------------------------------------------------
// Element
// -----------------------------------------------------------------------------	

Element::Element(Element* _parent, ElementType _type, const ID& _id):
	type(_type), id(_id), name_is_set(false), parent(_parent)
{
}

Element::Element(Element* _parent, ElementType _type, const ID& _id, const Name& _name):
	type(_type), id(_id), parent(_parent)
{
	set_name(_name);
}

int Element::index() const
{
	if (parent) {
		const ElementCollection* collection = static_cast<const ElementCollection*>(parent);
		CYB_ASSERT(collection);
		return collection->element_index(this);
	} else {
		return 0;
	}
}

void Element::set_name(const Name& n)
{
	name = n;
	name_is_set = true;
}

bool Element::has_qualified_name() const
{
	return is_root() || name_is_set || parent->has_qualified_name();
}

QualifiedName Element::qualified_name() const
{
	if (is_root()) {
		return name;
	} else {
		if (parent->is_root()) {
			return name;
		} else {
			return parent->qualified_name() + QUALIFIED_NAME_SEPARATOR + name;
		}
	}
}

Element* Element::find_root()
{
	if (!parent) {
		return this;
	} else {
		return parent->find_root();
	}
}

#include <iostream>

CyberiadaNode* Element::to_node() const
{
	CyberiadaNode* node = cyberiada_new_node(get_id().c_str());
	switch (type) {
	case elementSM:             node->type = cybNodeSM; break;
	case elementSimpleState:    node->type = cybNodeSimpleState; break;
	case elementCompositeState: node->type = cybNodeCompositeState; break;
	case elementComment:        node->type = cybNodeComment; break;
	case elementFormalComment:  node->type = cybNodeFormalComment; break;
	case elementInitial:        node->type = cybNodeInitial; break;
	case elementFinal:          node->type = cybNodeFinal; break;
	case elementChoice:         node->type = cybNodeChoice; break;
	case elementTerminate:      node->type = cybNodeTerminate; break;
	default:
		std::cerr << id << " " << type << std::endl;
		CYB_ASSERT(false);
	}	
	if (has_name()) {
		cyberiada_copy_string(&(node->title), &(node->title_len),
							  get_name().c_str());
	}
	return node;
}

std::string Element::dump_to_str() const
{
	std::ostringstream s;
	dump(s);
	return s.str();
}

std::ostream& Element::dump(std::ostream& os) const
{
	String type_str;
	switch (type) {
	case elementRoot:           type_str = "Document"; break;
	case elementSM:             type_str = "State Machine"; break;
	case elementSimpleState:    type_str = "Simple State"; break;
	case elementCompositeState: type_str = "Composite State"; break;
	case elementComment:        type_str = "Comment"; break;
	case elementFormalComment:  type_str = "Formal Comment"; break;
	case elementInitial:        type_str = "Initial"; break;
	case elementFinal:          type_str = "Final"; break;
	case elementChoice:         type_str = "Choice"; break;
	case elementTerminate:      type_str = "Terminate"; break;
	case elementTransition:     type_str = "Transition"; break;
	default:
		CYB_ASSERT(false);
	}
	os << type_str << ": {id: '" << id << "'";
	if (name_is_set) {
		os << ", name: '" << name << "'";
	}
	return os;
}

std::ostream& Cyberiada::operator<<(std::ostream& os, const Element& e)
{
	e.dump(os);
	return os;
}

// -----------------------------------------------------------------------------
// Geometry
// -----------------------------------------------------------------------------	

Point::Point(CyberiadaPoint* p)
{
	if (p) {
		valid = true;
		x = p->x;
		y = p->y;
	} else {
		valid = false;
	}
}

CyberiadaPoint* Point::c_point() const
{
	if (valid) {
		CyberiadaPoint* p = cyberiada_new_point();
		p->x = x;
		p->y = y;
		return p;
	} else {
		return NULL;
	}
}

Rect::Rect(CyberiadaRect* r)
{
	if (r) {
		valid = true;
		x = r->x;
		y = r->y;
		width = r->width;
		height = r->height;
	} else {
		valid = false;
	}
}

CyberiadaRect* Rect::c_rect() const
{
	if (valid) {
		CyberiadaRect* r = cyberiada_new_rect();
		r->x = x;
		r->y = y;
		r->width = width;
		r->height = height;
		return r;
	} else {
		return NULL;
	}
}

CyberiadaPolyline* Cyberiada::c_polyline(const Polyline& polyline)
{
	CyberiadaPolyline* result = NULL;
	for (Polyline::const_iterator i = polyline.begin(); i != polyline.end(); i++) {
		const Point& point = *i;
		CyberiadaPolyline* pl = cyberiada_new_polyline();
		pl->point.x = point.x;
		pl->point.y = point.y;
		if (result) {
			CyberiadaPolyline* last_pl = result;
			while (last_pl->next) last_pl = last_pl->next;
			last_pl->next = pl;
		} else {
			result = pl;
		}
	}
	return result;
}

// -----------------------------------------------------------------------------
// Action
// -----------------------------------------------------------------------------	

Action::Action(ActionType _type, const Behavior& _behavior):
	type(_type), behavior(_behavior)
{
}

Action::Action(const Event& _trigger, const Guard& _guard, const Behavior& _behavior):
	type(actionTransition), trigger(_trigger), guard(_guard), behavior(_behavior)
{
}

std::ostream& Action::dump(std::ostream& os) const
{
	if (type != actionTransition) {
		if (type == actionEntry) {
			os << "entry";
		} else {
			CYB_ASSERT(type == actionExit);
			os << "exit";
		}
	} else if (!trigger.empty()) {
		os << "trigger: '" << trigger << "'";
	}
	if (!guard.empty()) {
		if (type != actionTransition || !trigger.empty()) {
			os << ", ";
		}
		os << "guard: '" << guard << "'";
	}
	if (!behavior.empty()) {
		if (type != actionTransition || !trigger.empty() || !guard.empty()) {
			os << ", ";
		}
		os << "behavior: '" << behavior << "'";
	}
	return os;
}

// -----------------------------------------------------------------------------
// Geometry objects
// -----------------------------------------------------------------------------

bool Rect::operator==(const Rect& r)
{
	if (!valid && !r.valid) return true;
	if (!valid || !r.valid) return false;
	return x == r.x && y == r.y && width == r.width && height == r.height;
}

void Rect::expand(const Point& p)
{
	if (p.valid) {
		if (valid) {
			if (p.x < x) x = p.x;
			if (p.x > x + width) width = p.x - x;
			if (p.y < y) y = p.y;
			if (p.y > y + height) height = p.y - y;
		} else {
			x = p.x;
			y = p.y;
			width = height = 0.0;
		}
	}
}

void Rect::expand(const Rect& r)
{
	if (r.valid) {
		if (valid) {
			expand(Point(r.x, r.y));
			expand(Point(r.x + r.width, r.y + r.height));
		} else {
			*this = r;
		}
	}
}

void Rect::expand(const Polyline& pl)
{
	if (pl.size() > 0) {
		for (Polyline::const_iterator i = pl.begin(); i != pl.end(); i++) {
			expand(*i);
		}
	}
}

std::ostream& Cyberiada::operator<<(std::ostream& os, const Point& p)
{
	if (!p.valid) {
		os << "()";
	} else {
		os << "(" << p.x << "; " << p.y << ")";
	}
	return os;
}

std::ostream& Cyberiada::operator<<(std::ostream& os, const Rect& r)
{
	if (!r.valid) {
		os << "()";
	} else {
		os << "(" << r.x << "; " << r.y << "; " << r.width << "; " << r.height << ")";
	}
	return os;
}

std::ostream& Cyberiada::operator<<(std::ostream& os, const Polyline& pl)
{
	os << "[ ";
	for (Polyline::const_iterator i = pl.begin(); i != pl.end(); i++) {	
		os << *i;
		if (std::next(i) != pl.end()) {
			os << ", ";
		}
	}
	os << " ]";
	return os;
}

// -----------------------------------------------------------------------------
// Comment
// -----------------------------------------------------------------------------

CommentSubject::CommentSubject(const ID& _id, Element* _element,
							   const Point& source, const Point& target, const Polyline& pl):
	type(commentSubjectElement), id(_id), element(_element), has_frag(false), source_point(source), target_point(target), polyline(pl)
{
}

CommentSubject::CommentSubject(const ID& _id, Element* _element, CommentSubjectType _type, const String& _fragment,
							   const Point& source, const Point& target, const Polyline& pl):
	type(_type), id(_id), element(_element), has_frag(true), fragment(_fragment), source_point(source), target_point(target), polyline(pl)
{
}

CommentSubject& CommentSubject::operator=(const CommentSubject& cs)
{
	type = cs.type;
	id = cs.id;
	element = cs.element;
	has_frag = cs.has_frag;
	fragment = cs.fragment;
	source_point = cs.source_point;
	target_point = cs.target_point;
	polyline = cs.polyline;
	return *this;
}

Rect CommentSubject::get_bound_rect() const
{
	Rect r;
	if (has_geometry()) {
		if (has_geometry_source_point()) {
			r.expand(source_point);
		}
		if (has_geometry_target_point()) {
			r.expand(target_point);
		}
		if (has_polyline()) {
			r.expand(polyline);
		}
	}
	return r;
}

std::ostream& Cyberiada::operator<<(std::ostream& os, const CommentSubject& cs)
{
	cs.dump(os);
	return os;
}

std::ostream& CommentSubject::dump(std::ostream& os) const
{
	String type_str;
	if (type == commentSubjectElement) {
		type_str = "element";
	} else if (type == commentSubjectName) {
		type_str = "name";
	} else {
		CYB_ASSERT(type == commentSubjectData);
		type_str = "data";
	}
	os << "{id: '" << id << "'";
	os << ", type: " << type_str;
	if (element) {
		os << ", to: '" << element->get_id() << "'";
		if (has_frag) {
			os << ", fragment: '" << fragment << "'";
		}
		if (source_point.valid) {
			os << ", source point: " << source_point;
		}
		if (target_point.valid) {
			os << ", target point: " << target_point;
		}
		if (!polyline.empty()) {
			os << ", polyline: " << polyline;
		}
	}
	os << "}";
	return os;
}

Comment::Comment(Element* _parent, const ID& _id, const String& _body, bool _human_readable,
				 const String& _markup, const Rect& rect, const Color& _color):
	Element(_parent, elementComment, _id), body(_body), markup(_markup),
	human_readable(_human_readable), geometry_rect(rect), color(_color)
{
	update_comment_type();
}

Comment::Comment(Element* _parent, const ID& _id, const String& _body, const Name& _name, bool _human_readable,
				 const String& _markup, const Rect& rect, const Color& _color):
	Element(_parent, elementComment, _id, _name), body(_body), markup(_markup),
	human_readable(_human_readable), geometry_rect(rect), color(_color)
{
	update_comment_type();
}

const CommentSubject& Comment::add_subject(const CommentSubject& s)
{
	subjects.push_back(s);
	return subjects.back();
}

void Comment::remove_subject(CommentSubjectType _type, const String& fragment)
{
	for (std::list<CommentSubject>::iterator i = subjects.begin(); i != subjects.end(); i++) {
		if (i->get_type() == _type &&
			i->has_fragment()
			&& i->get_fragment() == fragment) {
			
			subjects.erase(i);
			break;
		}
	}
}

void Comment::update_comment_type()
{
	if (human_readable) {
		this->set_type(elementComment);
	} else {
		this->set_type(elementFormalComment);
	}
}

CyberiadaNode* Comment::to_node() const
{
	CyberiadaNode* node = Element::to_node();
	CyberiadaCommentData* data = cyberiada_new_comment_data();
	if (!body.empty()) {
		cyberiada_copy_string(&(data->body), &(data->body_len), body.c_str());
	}
	if (!markup.empty()) {
		cyberiada_copy_string(&(data->markup), &(data->markup_len), markup.c_str());
	}
	node->comment_data = data;
	if (has_geometry()) {
		node->geometry_rect = geometry_rect.c_rect();
		if (has_color()) {
			cyberiada_copy_string(&(node->color), &(node->color_len), color.c_str());
		}
	}
	return node;
}

CyberiadaEdge* Comment::subjects_to_edges() const
{
	CyberiadaEdge* result = NULL;
	if (has_subjects()) {
		for (std::list<CommentSubject>::const_iterator i = subjects.begin(); i != subjects.end(); i++) {
			CyberiadaEdge *edge = cyberiada_new_edge(i->get_id().c_str(),
													 get_id().c_str(),
													 i->get_element()->get_id().c_str());
			edge->type = cybEdgeComment;
			CyberiadaCommentSubjectType t;
			if (i->get_type() == commentSubjectElement) {
				t = cybCommentSubjectNode;
			} else if (i->get_type() == commentSubjectName) {
				t = cybCommentSubjectNameFragment;
			} else {
				CYB_ASSERT(i->get_type() == commentSubjectData);
				t = cybCommentSubjectDataFragment;
			}
			CyberiadaCommentSubject* cs = cyberiada_new_comment_subject(t);
			
			if (i->has_fragment()) {
				cyberiada_copy_string(&(cs->fragment), &(cs->fragment_len), i->get_fragment().c_str());			
			}
			edge->comment_subject = cs;

			if (i->has_geometry()) {
				if (i->get_geometry_source_point().valid) {
					edge->geometry_source_point = i->get_geometry_source_point().c_point();
				}
				if (i->get_geometry_target_point().valid) {
					edge->geometry_target_point = i->get_geometry_target_point().c_point();
				}
				if (i->has_polyline()) {
					edge->geometry_polyline = c_polyline(i->get_geometry_polyline());
				}
			}

			if (result) {
				CyberiadaEdge* e = result;
				while (e->next) e = e->next;
				e->next = edge;
			} else {
				result = edge;
			}
		}
	}
	return result;
}

Rect Comment::get_bound_rect() const
{
	Rect r;
	if (has_geometry()) {
		r = geometry_rect;
	}
	if (has_subjects()) {
		for (std::list<CommentSubject>::const_iterator i = subjects.begin(); i != subjects.end(); i++) {
			r.expand(i->get_bound_rect());
		}
	}	
	return r;
}

std::ostream& Comment::dump(std::ostream& os) const
{
	Element::dump(os);
	os << ", body: '" << body << "'";
	if (has_geometry()) {
		os << ", geometry: " << geometry_rect;
	}
	if (has_subjects()) {
		os << ", subjects: {";
		for (std::list<CommentSubject>::const_iterator i = subjects.begin(); i != subjects.end(); i++) {
			os << *i;
			if(std::next(i) != subjects.end()) {
				os << ", ";
			}
		}
		os << "}";
	}
	os << "}";
	return os;
}

// -----------------------------------------------------------------------------
// Vertex
// -----------------------------------------------------------------------------

Vertex::Vertex(Element* _parent, ElementType _type, const ID& _id, const Point& pos):
	Element(_parent, _type, _id), geometry_point(pos)
{
}

Vertex::Vertex(Element* _parent, ElementType _type, const ID& _id, const Name& _name, const Point& pos):
	Element(_parent, _type, _id, _name), geometry_point(pos)
{
}

Rect Vertex::get_bound_rect() const
{
	Rect r;
	if (has_geometry()) {
		r.expand(geometry_point);
	}	
	return r;
}

std::ostream& Vertex::dump(std::ostream& os) const
{
	Element::dump(os);
	if (has_geometry()) {
		os << ", geometry: " << geometry_point;
	}
	os << "}";	
	return os;
}

CyberiadaNode* Vertex::to_node() const
{
	CyberiadaNode* node = Element::to_node();
	if (has_geometry()) {
		node->geometry_point = geometry_point.c_point();
	}
	return node;
}

// -----------------------------------------------------------------------------
// Collection of Elements
// -----------------------------------------------------------------------------

ElementCollection::ElementCollection(Element* _parent, ElementType _type, const ID& _id, const Name& _name,
									 const Rect& rect, const Color& _color):
	Element(_parent, _type, _id, _name), geometry_rect(rect), color(_color)
{
}

ElementCollection::~ElementCollection()
{
	clear();
}

bool ElementCollection::has_qualified_name(const ID& element_id) const
{
	const Element* e = find_element_by_id(element_id);
	if (!e) {
		return false;
	} else {
		return e->has_qualified_name();
	}
}

QualifiedName ElementCollection::qualified_name(const ID& element_id) const
{
	const Element* e = find_element_by_id(element_id);
	if (!e) {
		throw NotFoundException();
	} else {
		if (is_root()) {
			return e->qualified_name();
		} else {
			return get_name() + QUALIFIED_NAME_SEPARATOR + e->qualified_name();
		}
	}
}

const Element* ElementCollection::find_element_by_id(const ID& _id) const
{
	for (ElementList::const_iterator i = children.begin(); i != children.end(); i++) {
		const Element* e = *i;
		if (e->get_id() == _id) {
			return e;
		} else if (e->has_children()) {
			const ElementCollection* c = static_cast<const ElementCollection*>(e);
			e = c->find_element_by_id(_id);
			if (e) {
				return e;
			}
		}
	}
	return NULL;
}

Element* ElementCollection::find_element_by_id(const ID& _id)
{
	for (ElementList::iterator i = children.begin(); i != children.end(); i++) {
		Element* e = *i;
		if (e->get_id() == _id) {
			return e;
		} else if (e->has_children()) {
			ElementCollection* c = static_cast<ElementCollection*>(e);
			e = c->find_element_by_id(_id);
			if (e) {
				return e;
			}
		}
	}
	return NULL;
}

ConstElementList ElementCollection::find_elements_by_type(ElementType _type) const
{
	ConstElementList result;
	for (ElementList::const_iterator i = children.begin(); i != children.end(); i++) {
		const Element* e = *i;
		if (e->get_type() == _type) {
			result.push_back(e);
		}
		if (e->has_children()) {
			const ElementCollection* c = static_cast<const ElementCollection*>(e);
			ConstElementList r = c->find_elements_by_type(_type);
			result.insert(result.end(), r.begin(), r.end());
		}
	}	
	return result;
}

ConstElementList ElementCollection::find_elements_by_types(const ElementTypes& types) const
{
	ConstElementList result;
	for (ElementList::const_iterator i = children.begin(); i != children.end(); i++) {
		const Element* e = *i;
		if (std::find(types.begin(), types.end(), e->get_type()) != types.end()) {
			result.push_back(e);
		}
		if (e->has_children()) {
			const ElementCollection* c = static_cast<const ElementCollection*>(e);
			ConstElementList r = c->find_elements_by_types(types);
			result.insert(result.end(), r.begin(), r.end());
		}
	}	
	return result;
}

ElementList ElementCollection::find_elements_by_type(ElementType _type)
{
	ElementList result;
	for (ElementList::iterator i = children.begin(); i != children.end(); i++) {
		Element* e = *i;
		if (e->get_type() == _type) {
			result.push_back(e);
		}
		if (e->has_children()) {
			ElementCollection* c = static_cast<ElementCollection*>(e);
			ElementList r = c->find_elements_by_type(_type);
			result.insert(result.end(), r.begin(), r.end());
		}
	}
	return result;
}

ElementList ElementCollection::find_elements_by_types(const ElementTypes& types)
{
	ElementList result;
	for (ElementList::iterator i = children.begin(); i != children.end(); i++) {
		Element* e = *i;
		if (std::find(types.begin(), types.end(), e->get_type()) != types.end()) {
			result.push_back(e);
		}
		if (e->has_children()) {
			ElementCollection* c = static_cast<ElementCollection*>(e);
			ElementList r = c->find_elements_by_types(types);
			result.insert(result.end(), r.begin(), r.end());
		}
	}	
	return result;	
}

size_t  ElementCollection::elements_count() const
{
	size_t count = 1;
	for (ElementList::const_iterator i = children.begin(); i != children.end(); i++) {
		count += (*i)->elements_count();
	}
	return count;
}

bool ElementCollection::has_initial() const
{
	for (ElementList::const_iterator i = children.begin(); i != children.end(); i++) {
		if ((*i)->get_type() == elementInitial) {
			return true;
		}
	}
	return false;
}

int ElementCollection::element_index(const Element* e) const
{
	CYB_ASSERT(e);
	int index = 0;
	for (ElementList::const_iterator i = children.begin(); i != children.end(); i++, index++) {
		if (*i == e) {
			return index;
		}
	}
	return -1;
}

void ElementCollection::add_element(Element* e)
{
	CYB_ASSERT(e);
	CYB_ASSERT(e->get_parent() == this);
	if (e->get_type() == elementTransition) {
		children.push_back(e);
	} else {
		ElementList::iterator i;
		for (i = children.begin(); i != children.end(); i++) {
			if ((*i)->get_type() == elementTransition) {
				break;
			}
		}
		children.insert(i, e);
	}
}

void ElementCollection::add_first_element(Element* e)
{
	CYB_ASSERT(e);
	CYB_ASSERT(e->get_parent() == this);
	children.push_front(e);
}

void ElementCollection::remove_element(const ID& _id)
{
	for (ElementList::iterator i = children.begin(); i != children.end(); i++) {
		if ((*i)->get_id() == _id) {
			children.remove(*i);
			break;
		}
	}
}

void ElementCollection::clear()
{
	for (ElementList::iterator i = children.begin(); i != children.end(); i++) {
		Element* e = *i;
		delete e;
	}
	children.clear();
}

std::list<const Vertex*> ElementCollection::get_vertexes() const
{
	ElementTypes types = { elementSimpleState,
						   elementCompositeState,
						   elementInitial,
						   elementFinal,
						   elementChoice,
						   elementTerminate};
	std::list<const Vertex*> result;
	ConstElementList vertexes = find_elements_by_types(types);
	for (ConstElementList::const_iterator i = vertexes.begin(); i != vertexes.end(); i++) {
		result.push_back(static_cast<const Vertex*>(*i));
	}
	return result;
}

std::list<Vertex*> ElementCollection::get_vertexes()
{
	ElementTypes types = { elementComment,
						   elementFormalComment,
						   elementSimpleState,
						   elementCompositeState,
						   elementInitial,
						   elementFinal,
						   elementChoice,
						   elementTerminate};
	std::list<Vertex*> result;
	ElementList vertexes = find_elements_by_types(types);
	for (ElementList::const_iterator i =  vertexes.begin(); i != vertexes.end(); i++) {
		result.push_back(static_cast<Vertex*>(*i));
	}
	return result;
}

const Element* ElementCollection::first_element() const
{
	if (has_children()) {
		const Element* element = *(children.begin());
		return element;
	} else {
		return NULL;
	}
}

Element* ElementCollection::first_element()
{
	if (has_children()) {
		Element* element = *(children.begin());
		return element;
	} else {
		return NULL;
	}
}

const Element* ElementCollection::get_element(int index) const
{
	int idx = 0;
	if (has_children()) {
		for (ElementList::const_iterator i = children.begin(); i != children.end(); i++, idx++) {
			if (idx == index) {
				return *i;
			}
		}
	}
	return NULL;
}

Element* ElementCollection::get_element(int index)
{
	int idx = 0;
	if (has_children()) {
		for (ElementList::iterator i = children.begin(); i != children.end(); i++, idx++) {
			if (idx == index) {
				return *i;
			}
		}
	}
	return NULL;	
}

CyberiadaNode* ElementCollection::to_node() const
{
	CyberiadaNode* node = Element::to_node();
	if (has_geometry()) {
		node->geometry_rect = geometry_rect.c_rect();
	}
	if (has_color()) {
		cyberiada_copy_string(&(node->color), &(node->color_len), color.c_str());
	}
	for (ElementList::const_iterator i = children.begin(); i != children.end(); i++) {
		const Element* e = *i;
		CYB_ASSERT(e);
		if (e->get_type() == elementTransition) {
			continue;
		}
		CyberiadaNode* child = e->to_node();
		CYB_ASSERT(child);
		if (node->children) {
			CyberiadaNode* n = node->children;
			while (n->next) n = n->next;
			n->next = child;
		} else {
			node->children = child;
		}
	}
	return node;
}

Rect ElementCollection::get_bound_rect() const
{
	Rect r;
	if (has_geometry()) {
		r = geometry_rect;
	}
	return r;
}

std::ostream& ElementCollection::dump(std::ostream& os) const
{
	if (has_geometry()) {
		os << ", geometry: " << geometry_rect;
		if (has_color()) {
			os << ", color: " << color;
		}
	}
	if (has_children()) {
		os << ", elements: {";
		for (ElementList::const_iterator i = children.begin(); i != children.end(); i++) {
			const Element* e = *i;
			CYB_ASSERT(e);
			os << *e;
			if (std::next(i) != children.end()) {
				os << ", ";
			}
		}
		os << "}";
	}
	return os;
}

// -----------------------------------------------------------------------------
// Pseudostate
// -----------------------------------------------------------------------------

Pseudostate::Pseudostate(Element* _parent, ElementType _type, const ID& _id, const Point& p):
	Vertex(_parent, _type, _id, p)
{
}

Pseudostate::Pseudostate(Element* _parent, ElementType _type, const ID& _id, const Name& _name, const Point& p):
	Vertex(_parent, _type, _id, _name, p)
{
}

// -----------------------------------------------------------------------------
// Initial pseudostate
// -----------------------------------------------------------------------------

InitialPseudostate::InitialPseudostate(Element* _parent, const ID& _id, const Point& p):
	Pseudostate(_parent, elementInitial, _id, p)
{
}

InitialPseudostate::InitialPseudostate(Element* _parent, const ID& _id, const Name& _name, const Point& p):
	Pseudostate(_parent, elementInitial, _id, _name, p)
{
}

// -----------------------------------------------------------------------------
// Terminate pseudostate
// -----------------------------------------------------------------------------

TerminatePseudostate::TerminatePseudostate(Element* _parent, const ID& _id, const Point& p):
	Pseudostate(_parent, elementTerminate, _id, p)
{
}

TerminatePseudostate::TerminatePseudostate(Element* _parent, const ID& _id, const Name& _name, const Point& p):
	Pseudostate(_parent, elementTerminate, _id, _name, p)
{
}

// -----------------------------------------------------------------------------
// Choice pseudostate
// -----------------------------------------------------------------------------

ChoicePseudostate::ChoicePseudostate(Element* _parent, const ID& _id, const Rect& r, const Color& _color):
	Pseudostate(_parent, elementChoice, _id), geometry_rect(r), color(_color)
{
}

ChoicePseudostate::ChoicePseudostate(Element* _parent, const ID& _id, const Name& _name, const Rect& r, const Color& _color):
	Pseudostate(_parent, elementChoice, _id, _name), geometry_rect(r), color(_color)
{
}

CyberiadaNode* ChoicePseudostate::to_node() const
{
	CyberiadaNode* node = Element::to_node();
	if (has_geometry()) {
		node->geometry_rect = geometry_rect.c_rect();
		if (has_color()) {
			cyberiada_copy_string(&(node->color), &(node->color_len), color.c_str());
		}
	}
	return node;
}

Rect ChoicePseudostate::get_bound_rect() const
{
	Rect r;
	if (has_geometry()) {
		r = geometry_rect;
	}
	return r;
}

std::ostream& ChoicePseudostate::dump(std::ostream& os) const
{
	Element::dump(os);
	if (has_geometry()) {
		os << ", geometry: " << geometry_rect;
		if (has_color()) {
			os << ", color: " << color;
		}
	}		
	os << "}";
	return os;
}

// -----------------------------------------------------------------------------
// Final state
// -----------------------------------------------------------------------------

FinalState::FinalState(Element* _parent, const ID& _id, const Point& p):
	Vertex(_parent, elementFinal, _id, p)
{
}

FinalState::FinalState(Element* _parent, const ID& _id, const Name& _name, const Point& p):
	Vertex(_parent, elementFinal, _id, _name, p)
{
}

// -----------------------------------------------------------------------------
// State
// -----------------------------------------------------------------------------

std::ostream& Cyberiada::operator<<(std::ostream& os, const Action& a)
{
	a.dump(os);
	return os;
}

State::State(Element* _parent, const ID& _id, const Name& _name, const Rect& r, const Color& c):
	ElementCollection(_parent, elementSimpleState, _id, _name, r, c)
{
}

void State::add_element(Element* e)
{
	ElementCollection::add_element(e);
	update_state_type();
}

void State::remove_element(const ID& _id)
{
	ElementCollection::remove_element(_id);
	update_state_type();
}

void State::add_action(const Action& a)
{
	if (a.is_empty_transition()) {
		throw ParametersException("Empty transition action is not allowed");
	}
	if (a.get_type() != actionTransition && a.has_guard()) {
		throw ParametersException("Guards are not allowed for entry/exit activities");
	}
	actions.push_back(a);
}

std::list<const State*> State::get_substates() const
{
	ElementTypes types = { elementSimpleState,
						   elementCompositeState };
	std::list<const State*> result;
	ConstElementList states = find_elements_by_types(types);
	for (ConstElementList::const_iterator i = states.begin(); i != states.end(); i++) {
		result.push_back(static_cast<const State*>(*i));
	}
	return result;
}

std::list<State*> State::get_substates()
{
	ElementTypes types = { elementSimpleState,
						   elementCompositeState };
	std::list<State*> result;
	ElementList states = find_elements_by_types(types);
	for (ElementList::const_iterator i = states.begin(); i != states.end(); i++) {
		result.push_back(static_cast<State*>(*i));
	}
	return result;
}

void State::update_state_type()
{
	if (has_children()) {
		this->set_type(elementCompositeState);
	} else {
		this->set_type(elementSimpleState);
	}
}

CyberiadaNode* State::to_node() const
{
	CyberiadaNode* node = ElementCollection::to_node();
	if (has_actions()) {
		for (std::list<Action>::const_iterator i = actions.begin(); i != actions.end(); i++) {
			const Action& a = *i;
			CyberiadaActionType at;
			if (a.get_type() == actionEntry) {
				at = cybActionEntry;
			} else if (a.get_type() == actionExit) {
				at = cybActionExit;
			} else {
				CYB_ASSERT(a.get_type() == actionTransition);
				at = cybActionTransition;
			}
			CyberiadaAction* action = cyberiada_new_action(at,
														   a.get_trigger().c_str(),
														   a.get_guard().c_str(),
														   a.get_behavior().c_str());
			if (node->actions) {
				CyberiadaAction* last_a = node->actions;
				while (last_a->next) last_a = last_a->next;
				last_a->next = action;
			} else {
				node->actions = action;
			}
		}
	}
	return node;
}

std::ostream& State::dump(std::ostream& os) const
{
	Element::dump(os);
	if (has_actions()) {
		os << ", actions: {";
		for (std::list<Action>::const_iterator i = actions.begin(); i != actions.end(); i++) {
			os << "a {" << *i << "}";
			if (std::next(i) != actions.end()) {
				os << ", ";
			}
		}
		os << "}";
	}
	ElementCollection::dump(os);
	os << "}";
	return os;
}

// -----------------------------------------------------------------------------
// Transition
// -----------------------------------------------------------------------------

Transition::Transition(Element* _parent, const ID& _id, Element* _source, Element* _target, const Action& _action,
					   const Polyline& pl, const Point& sp, const Point& tp, const Point& label,
					   const Color& c):
	Element(_parent, elementTransition, _id), source(_source), target(_target), action(_action),
	source_point(sp), target_point(tp), label_point(label), polyline(pl), color(c)
{
}

CyberiadaEdge* Transition::to_edge() const
{
	CYB_ASSERT(source);
	CYB_ASSERT(target);
	CyberiadaEdge* edge = cyberiada_new_edge(get_id().c_str(),
											 source->get_id().c_str(),
											 target->get_id().c_str());
	edge->type = cybEdgeTransition;
	if (has_action()) {
		edge->action = cyberiada_new_action(cybActionTransition,
											action.get_trigger().c_str(),
											action.get_guard().c_str(),
											action.get_behavior().c_str());
	}
	if (has_geometry()) {
		if (source_point.valid) {
			edge->geometry_source_point = source_point.c_point();
		}
		if (target_point.valid) {
			edge->geometry_target_point = target_point.c_point();
		}
		if (label_point.valid) {
			edge->geometry_label_point = label_point.c_point();
		}
		if (has_polyline()) {
			edge->geometry_polyline = c_polyline(polyline);
		}
		if (has_color()) {
			cyberiada_copy_string(&(edge->color), &(edge->color_len), color.c_str());
		}		
	}
	return edge;
}

Rect Transition::get_bound_rect() const
{
	Rect r;
	if (has_geometry()) {
		if (has_geometry_source_point()) {
			r.expand(source_point);
		}
		if (has_geometry_target_point()) {
			r.expand(target_point);
		}
		if (has_geometry_label_point()) {
			r.expand(label_point);
		}
		if (has_polyline()) {
			r.expand(polyline);
		}
	}
	return r;
}

std::ostream& Transition::dump(std::ostream& os) const
{
	Element::dump(os);
	os << ", source: '" << source->get_id() << "'";
	os << ", target: '" << target->get_id() << "'";
	if (has_action()) {
		os << ", action: {";
		os << action;
		os << "}";
	}
	if (has_geometry()) {
		if (source_point.valid) {
			os << ", sp: " << source_point;
		}
		if (target_point.valid) {
			os << ", tp: " << target_point;
		}
		if (label_point.valid) {
			os << ", label: " << label_point;
		}
		if (has_polyline()) {
			os << ", polyline: " << polyline;
		}
		if (has_color()) {
			os << ", color: " << color;
		}
	}
	os << "}";
	return os;
}

// -----------------------------------------------------------------------------
// State Machine
// -----------------------------------------------------------------------------
StateMachine::StateMachine(Element* _parent, const ID& _id, const Name& _name, const Rect& r):
	ElementCollection(_parent, elementSM, _id, _name, r)
{
}

std::list<const Comment*> StateMachine::get_comments() const
{
	ElementTypes types = { elementComment,
						   elementFormalComment };
	std::list<const Comment*> result;
	ConstElementList comments = find_elements_by_types(types);
	for (ConstElementList::const_iterator i = comments.begin(); i != comments.end(); i++) {
		result.push_back(static_cast<const Comment*>(*i));
	}
	return result;
}

std::list<Comment*> StateMachine::get_comments()
{
	ElementTypes types = { elementComment,
						   elementFormalComment };
	std::list<Comment*> result;
	ElementList comments = find_elements_by_types(types);
	for (ElementList::iterator i = comments.begin(); i != comments.end(); i++) {
		result.push_back(static_cast<Comment*>(*i));
	}
	return result;
}

std::list<const Transition*> StateMachine::get_transitions() const
{
	std::list<const Transition*> result;
	ConstElementList transitions = find_elements_by_type(elementTransition);
	for (ConstElementList::const_iterator i = transitions.begin(); i != transitions.end(); i++) {
		result.push_back(static_cast<const Transition*>(*i));
	}
	return result;
}

std::list<Transition*> StateMachine::get_transitions()
{
	std::list<Transition*> result;
	ElementList transitions = find_elements_by_type(elementTransition);
	for (ElementList::iterator i = transitions.begin(); i != transitions.end(); i++) {
		result.push_back(static_cast<Transition*>(*i));
	}
	return result;
}

Rect StateMachine::get_bound_rect() const
{
	Rect r;
	if (has_geometry()) {
		r = ElementCollection::get_bound_rect();
	} else if (has_children()) {
		for (ElementList::const_iterator i = children.begin(); i != children.end(); i++) {
			r.expand((*i)->get_bound_rect());
		}
	}
	return r;
}

std::ostream& StateMachine::dump(std::ostream& os) const
{
	Element::dump(os);
	ElementCollection::dump(os);
	os << "}";
	return os;
}

// -----------------------------------------------------------------------------
// Cyberiada-GraphML Document
// -----------------------------------------------------------------------------
Document::Document():
	ElementCollection(NULL, elementRoot, "", ""),
	format_str(DEFAULT_GRAPHML_FORMAT),
	format(formatCyberiada10),
	metainfo_element(NULL)
{
	reset();
}

void Document::reset()
{
	metainfo = DocumentMetainformation();
	metainfo.standard_version = STANDARD_VERSION;
	metainfo.transition_order_flag = false;     
	metainfo.event_propagation_flag = false;
	format = formatCyberiada10;
	format_str = DEFAULT_GRAPHML_FORMAT;
	metainfo_element = NULL;
	clear();
}

StateMachine* Document::new_state_machine(const String& sm_name, const Rect& r)
{
	StateMachine* sm = new StateMachine(this, generate_sm_id(), sm_name, r);
	add_element(sm);
	update_metainfo_element();
	return sm;
}

StateMachine* Document::new_state_machine(const ID& _id, const String& sm_name, const Rect& r)
{
	check_id_uniqueness(_id);
	
	StateMachine* sm = new StateMachine(this, _id, sm_name, r); 
	add_element(sm);
	update_metainfo_element();
	return sm;
}

State* Document::new_state(ElementCollection* _parent, const String& state_name, const Action& a,
						   const Rect& r, const Color& _color)
{
	check_parent_element(_parent);
	check_nonempty_string(state_name);
	
	State* state = new State(_parent, generate_vertex_id(_parent), state_name, r, _color);
	if (!a.is_empty_transition()) {
		state->add_action(a);
	}
	_parent->add_element(state);
	return state;
}

State* Document::new_state(ElementCollection* _parent, const ID& state_id, const String& state_name, const Action& a,
						   const Rect& r, const Color& _color)
{
	check_parent_element(_parent);
	check_nonempty_string(state_name);
	check_id_uniqueness(state_id);

	State* state = new State(_parent, state_id, state_name, r, _color);
	if (!a.is_empty_transition()) {
		state->add_action(a);
	}
	_parent->add_element(state);
	return state;
}

InitialPseudostate* Document::new_initial(ElementCollection* _parent, const Point& p)
{
	check_parent_element(_parent);
	check_single_initial(_parent);

	InitialPseudostate* initial = new InitialPseudostate(_parent, generate_vertex_id(_parent), p);
	_parent->add_element(initial);
	return initial;
}

InitialPseudostate* Document::new_initial(ElementCollection* _parent, const Name& initial_name, const Point& p)
{
	check_parent_element(_parent);
	check_nonempty_string(initial_name);
	check_single_initial(_parent);
	
	InitialPseudostate* initial = new InitialPseudostate(_parent, generate_vertex_id(_parent), initial_name, p);
	_parent->add_element(initial);
	return initial;
}

InitialPseudostate* Document::new_initial(ElementCollection* _parent, const ID& _id, const Name& initial_name, const Point& p)
{
	check_parent_element(_parent);
	check_nonempty_string(initial_name);
	check_single_initial(_parent);
	check_id_uniqueness(_id);
	
	InitialPseudostate* initial = new InitialPseudostate(_parent, _id, initial_name, p);
	_parent->add_element(initial);
	return initial;
}

FinalState* Document::new_final(ElementCollection* _parent, const Point& point)
{
	check_parent_element(_parent);

	FinalState* fin = new FinalState(_parent, generate_vertex_id(_parent), point);
	_parent->add_element(fin);
	return fin;
}

FinalState* Document::new_final(ElementCollection* _parent, const Name& _name, const Point& point)
{
	check_parent_element(_parent);
	check_nonempty_string(_name);

	FinalState* fin = new FinalState(_parent, generate_vertex_id(_parent), _name, point);
	_parent->add_element(fin);
	return fin;
}

FinalState* Document::new_final(ElementCollection* _parent, const ID& _id, const Name& _name, const Point& point)
{
	check_parent_element(_parent);
	check_nonempty_string(_name);
	check_id_uniqueness(_id);

	FinalState* fin = new FinalState(_parent, _id, _name, point);
	_parent->add_element(fin);
	return fin;	
}

ChoicePseudostate* Document::new_choice(ElementCollection* _parent, const Rect& r, const Color& c)
{
	check_parent_element(_parent);

	ChoicePseudostate* choice = new ChoicePseudostate(_parent, generate_vertex_id(_parent), r, c);
	_parent->add_element(choice);
	return choice;
}

ChoicePseudostate* Document::new_choice(ElementCollection* _parent, const Name& _name, const Rect& r, const Color& c)
{
	check_parent_element(_parent);
	check_nonempty_string(_name);

	ChoicePseudostate* choice = new ChoicePseudostate(_parent, generate_vertex_id(_parent), _name, r, c);
	_parent->add_element(choice);
	return choice;
}

ChoicePseudostate* Document::new_choice(ElementCollection* _parent, const ID& _id, const Name& _name, const Rect& r, const Color& c)
{
	check_parent_element(_parent);
	check_nonempty_string(_name);
	check_id_uniqueness(_id);
	
	ChoicePseudostate* choice = new ChoicePseudostate(_parent, generate_vertex_id(_parent), _name, r, c);
	_parent->add_element(choice);
	return choice;
}

TerminatePseudostate* Document::new_terminate(ElementCollection* _parent, const Point& p)
{
	check_parent_element(_parent);

	TerminatePseudostate* term = new TerminatePseudostate(_parent, generate_vertex_id(_parent), p);
	_parent->add_element(term);
	return term;
}

TerminatePseudostate* Document::new_terminate(ElementCollection* _parent, const Name& _name, const Point& p)
{
	check_parent_element(_parent);
	check_nonempty_string(_name);

	TerminatePseudostate* term = new TerminatePseudostate(_parent, generate_vertex_id(_parent), _name, p);
	_parent->add_element(term);
	return term;
}

TerminatePseudostate* Document::new_terminate(ElementCollection* _parent, const ID& _id, const Name& _name, const Point& p)
{
	check_parent_element(_parent);
	check_nonempty_string(_name);
	check_id_uniqueness(_id);
	
	TerminatePseudostate* term = new TerminatePseudostate(_parent, _id, _name, p);
	_parent->add_element(term);
	return term;
}

Transition* Document::new_transition(StateMachine* sm, Element* source, Element* target,
									 const Action& action, const Polyline& pl,
									 const Point& sp, const Point& tp,
									 const Point& label, const Color& c)
{
	check_parent_element(sm);
	check_transition_source(source);
	check_transition_target(target);
	check_transition_action(action);
	
	Transition* t = new Transition(sm, generate_transition_id(source->get_id(), target->get_id()),
								   source, target, action, pl, sp, tp, label, c);
	sm->add_element(t);
	return t;
}

Transition* Document::new_transition(StateMachine* sm, const ID& _id, Element* source, Element* target,
									 const Action& action, const Polyline& pl,
									 const Point& sp, const Point& tp,
									 const Point& label, const Color& c)
{
	check_parent_element(sm);
	check_transition_source(source);
	check_transition_target(target);	
	check_id_uniqueness(_id);
	check_transition_action(action);

	Transition* t = new Transition(sm, _id, source, target, action, pl, sp, tp, label, c);
	sm->add_element(t);
	return t;
}

Comment* Document::new_comment(ElementCollection* _parent, const String& body, const Rect& r, const Color& c, const String& markup)
{
	check_parent_element(_parent);

	Comment* comm = new Comment(_parent, generate_vertex_id(_parent), body, true, markup, r, c);
	_parent->add_element(comm);
	return comm;
}

Comment* Document::new_comment(ElementCollection* _parent, const String& _name, const String& body, const Rect& r, const Color& c,
							   const String& markup)
{
	check_parent_element(_parent);
	check_nonempty_string(_name);

	Comment* comm = new Comment(_parent, generate_vertex_id(_parent), body, _name, true, markup, r, c);
	_parent->add_element(comm);
	return comm;
}

Comment* Document::new_comment(ElementCollection* _parent, const ID& _id, const String& _name, const String& body,
							   const Rect& r, const Color& c, const String& markup)
{
	check_parent_element(_parent);
	check_id_uniqueness(_id);

	Comment* comm = new Comment(_parent, _id, body, _name, true, markup, r, c);
	_parent->add_element(comm);
	return comm;
}

Comment* Document::new_formal_comment(ElementCollection* _parent, const String& body, const Rect& r, const Color& c, const String& markup)
{
	check_parent_element(_parent);

	Comment* comm = new Comment(_parent, generate_vertex_id(_parent), body, false, markup, r, c);
	_parent->add_element(comm);
	return comm;
}

Comment* Document::new_formal_comment(ElementCollection* _parent, const String& _name, const String& body, const Rect& r, const Color& c,
									  const String& markup)
{
	check_parent_element(_parent);
	check_nonempty_string(_name);

	Comment* comm = new Comment(_parent, generate_vertex_id(_parent), body, _name, false, markup, r, c);
	_parent->add_element(comm);
	return comm;
}

Comment* Document::new_formal_comment(ElementCollection* _parent, const ID& _id, const String& _name, const String& body,
									  const Rect& r, const Color& c, const String& markup)
{
	check_parent_element(_parent);
	check_id_uniqueness(_id);

	Comment* comm = new Comment(_parent, _id, body, _name, true, markup, r, c);
	_parent->add_element(comm);
	return comm;
}

const CommentSubject& Document::add_comment_to_element(Comment* comment, Element* element,
													   const Point& source, const Point& target, const Polyline& pl)
{
	check_parent_element(comment);
	check_comment_subject_element(element);

	return comment->add_subject(CommentSubject(generate_transition_id(comment->get_id(), element->get_id()),
											   element, source, target, pl));
}

const CommentSubject& Document::add_comment_to_element(Comment* comment, Element* element, const ID& _id,
													   const Point& source, const Point& target, const Polyline& pl)
{
	check_parent_element(comment);
	check_comment_subject_element(element);
	check_id_uniqueness(_id);

	return comment->add_subject(CommentSubject(_id, element, source, target, pl));	
}

const CommentSubject& Document::add_comment_to_element_name(Comment* comment, Element* element, const String& fragment,
															const Point& source, const Point& target, const Polyline& pl)
{
	check_parent_element(comment);
	check_comment_subject_element(element);
	check_nonempty_string(fragment);
	
	return comment->add_subject(CommentSubject(generate_transition_id(comment->get_id(), element->get_id()), element,
											   commentSubjectName, fragment, source, target, pl));
}

const CommentSubject& Document::add_comment_to_element_name(Comment* comment, Element* element, const String& fragment, const ID& _id,
															const Point& source, const Point& target, const Polyline& pl)
{
	check_parent_element(comment);
	check_comment_subject_element(element);
	check_nonempty_string(fragment);
	check_id_uniqueness(_id);
	
	return comment->add_subject(CommentSubject(_id, element,
											   commentSubjectName, fragment, source, target, pl));
}

const CommentSubject& Document::add_comment_to_element_body(Comment* comment, Element* element, const String& fragment,
															const Point& source, const Point& target, const Polyline& pl)
{
	check_parent_element(comment);
	check_comment_subject_element(element);
	check_nonempty_string(fragment);
	
	return comment->add_subject(CommentSubject(generate_transition_id(comment->get_id(), element->get_id()), element,
											   commentSubjectData, fragment, source, target, pl));
}

const CommentSubject& Document::add_comment_to_element_body(Comment* comment, Element* element, const String& fragment, const ID& _id,
															const Point& source, const Point& target, const Polyline& pl)
{
	check_parent_element(comment);
	check_comment_subject_element(element);
	check_nonempty_string(fragment);
	check_id_uniqueness(_id);
	
	return comment->add_subject(CommentSubject(_id, element,
											   commentSubjectData, fragment, source, target, pl));
}

void Document::check_cyberiada_error(int res, const String& msg) const
{
	switch (res) {
	case CYBERIADA_XML_ERROR: throw XMLException(msg);
	case CYBERIADA_FORMAT_ERROR: throw CybMLException(msg);
	case CYBERIADA_ACTION_FORMAT_ERROR: throw ActionException(msg);
	case CYBERIADA_METADATA_FORMAT_ERROR: throw MetainformationException(msg);
	case CYBERIADA_NOT_FOUND: throw NotFoundException(msg);
	case CYBERIADA_BAD_PARAMETER: throw ParametersException(msg);
    case CYBERIADA_ASSERT: throw AssertException(msg);
	default:
		break;
	}
}

void Document::check_parent_element(const Element* _parent) const
{
	if (!_parent) {
		throw ParametersException("No parent element");
	}
}

void Document::check_nonempty_string(const String& s) const
{
	if (s.empty()) {
		throw ParametersException(String("Empty string parameter"));
	}
}

void Document::check_id_uniqueness(const ID& _id) const
{
	if (find_element_by_id(_id)) {
		throw ParametersException(String("New element id ") + _id + " is not unique");
	}
}

void Document::check_single_initial(const ElementCollection* _parent) const
{
	if (_parent->has_initial()) {
		throw ParametersException("Parent already has initial element");
	}
}

void Document::check_transition_action(const Action& action) const
{
	if (action.get_type() != actionTransition) {
		throw ParametersException("Transitions cannot contain entry/exit activities");
	}
}

void Document::check_transition_source(const Element* element) const
{
	if (!element) {
		throw ParametersException("Empty element");
	}
	if (element->get_type() == elementRoot || 
		element->get_type() == elementSM ||
		element->get_type() == elementComment ||
		element->get_type() == elementFormalComment ||
		element->get_type() == elementFinal ||
		element->get_type() == elementTerminate ||
		element->get_type() == elementTransition) {
		throw ParametersException("Bad source for transition");
	}
}

void Document::check_transition_target(const Element* element) const
{
	if (!element) {
		throw ParametersException("Empty element");
	}
	if (element->get_type() == elementRoot || 
		element->get_type() == elementSM ||
		element->get_type() == elementComment ||
		element->get_type() == elementFormalComment ||
		element->get_type() == elementInitial ||
		element->get_type() == elementTransition) {
		throw ParametersException("Bad target for transition");
	} else if (element->get_type() == elementChoice) {
		bool found = false;
		ConstElementList transitions = find_elements_by_type(elementTransition);
		for (ConstElementList::const_iterator i = transitions.begin(); i != transitions.end(); i++) {
			const Transition* t = static_cast<const Transition*>(*i);
			if (t->target_element()->get_id() == element->get_id()) {
				found = true;
				break;
			}
		}
		if (found) {
			throw ParametersException("Choice pseudostate may have only one incoming transition");
		}
	}
}

void Document::check_comment_subject_element(const Element* element) const
{
	if (!element) {
		throw ParametersException("Empty element");
	}
	if (element->get_type() == elementRoot || element->get_type() == elementSM) {
		throw ParametersException("Bad element to comment");
	}
}

void Document::import_nodes_recursively(ElementCollection* collection, CyberiadaNode* nodes)
{
	CYB_ASSERT(collection);
	for (CyberiadaNode* n = nodes; n; n = n->next) {
		Element* element = NULL;
		State* state = NULL;

		CYB_ASSERT(n->id);

		Point  point;
		Rect   rect;
		Color  _color;
		String comment_body;
		String comment_markup;

		if (n->geometry_rect) {
			rect = Rect(n->geometry_rect);
		}
		if (n->geometry_point) {
			point = Point(n->geometry_point);
		}
		if (n->color) {
			_color = Color(n->color);
		}
		
		switch (n->type) {
		case cybNodeSimpleState:
		case cybNodeCompositeState:
			if (!n->title) {
				throw CybMLException("State element w/o title");
			}
			state = new State(collection, n->id, n->title, rect, _color);
			CYB_ASSERT(state);
			
			for (CyberiadaAction* a = n->actions; a; a = a->next) {
				if (a->type == cybActionTransition) {
					state->add_action(Action(a->trigger, a->guard, a->behavior));
				} else {
					ActionType at;
					if (a->type == cybActionEntry) {
						at = actionEntry;
					} else if (a->type == cybActionExit) {
						at = actionExit;
					} else {
						throw CybMLException("Unsupported action type " + std::to_string(a->type));
					}
					if (a->guard && *(a->guard)) {
						// guard is not empty
						throw CybMLException("Guards are not allowed in entry/exit actions");
					}
					state->add_action(Action(at, a->behavior));
				}
			}
			
			element = state;
			
			break;

		case cybNodeComment:
		case cybNodeFormalComment:
			if (!n->comment_data) {
				throw CybMLException("No comment data in Comment element");
			}
	
			if (n->comment_data->body) {
				comment_body = n->comment_data->body;
			}
			if (n->comment_data->markup) {
				comment_markup = n->comment_data->markup;
			}

			if (n->title) {
				Comment* comment = new Comment(collection, n->id, comment_body, n->title,
											   n->type == cybNodeComment, comment_markup, rect, _color);
				element = comment;
				if (n->type == cybNodeFormalComment && String(n->title) == META_NODE_NAME) {
					CYB_ASSERT(!metainfo_element);
					metainfo_element = comment;
				}
			} else {
				element = new Comment(collection, n->id, comment_body, n->type == cybNodeComment,
									  comment_markup, rect, _color);
			}
			break;

		case cybNodeChoice:
			if (n->title) {
				element = new ChoicePseudostate(collection, n->id, n->title, rect, _color);
			} else {
				element = new ChoicePseudostate(collection, n->id, rect, _color);
			}
			break;
			
		case cybNodeInitial:
			if (n->title) {
				element = new InitialPseudostate(collection, n->id, n->title, point);
			} else {
				element = new InitialPseudostate(collection, n->id, point);
			}

			break;

		case cybNodeTerminate:
			if (n->title) {
				element = new TerminatePseudostate(collection, n->id, n->title, point);
			} else {
				element = new TerminatePseudostate(collection, n->id, point);
			}

			break;
			
		case cybNodeFinal:
			if (n->title) {
				element = new FinalState(collection, n->id, n->title, point);
			} else {
				element = new FinalState(collection, n->id, point);
			}

			break;
	
		default:
			throw CybMLException("Unsupported node type " + std::to_string(n->type));
		}

		collection->add_element(element);
			
		if (n->children) {
			if (collection->get_type() != elementSM &&
				collection->get_type() != elementSimpleState &&
				collection->get_type() != elementCompositeState) {

				throw CybMLException("Children nodes inside element with type " +
									 std::to_string(collection->get_type()));
			}
			
			import_nodes_recursively(static_cast<ElementCollection*>(element), n->children);
		}
	}
}

void Document::import_edges(ElementCollection* collection, CyberiadaEdge* edges)
{
	CYB_ASSERT(collection);
	for (CyberiadaEdge* e = edges; e; e = e->next) {
		CYB_ASSERT(e->id);

		Element* element = NULL;
		Point    source_point, target_point, label_point;
		Polyline polyline;
		Color    _color;
		Action   action;

		if (e->geometry_source_point) {
			source_point = Point(e->geometry_source_point);
		}
		if (e->geometry_target_point) {
			target_point = Point(e->geometry_target_point);
		}
		if (e->geometry_label_point) {
			label_point = Point(e->geometry_label_point);
		}
		if (e->geometry_polyline) {
			for (CyberiadaPolyline* pl = e->geometry_polyline; pl; pl = pl->next) {
				polyline.push_back(Point(pl->point.x, pl->point.y));
			}
		}
		if (e->color) {
			_color = Color(e->color);
		}

		Element* source_element = find_element_by_id(e->source_id);
		CYB_ASSERT(source_element);
		Element* target_element = find_element_by_id(e->target_id);
		CYB_ASSERT(target_element);
		Comment* comment = NULL;
		
		switch (e->type) {
		case cybEdgeTransition:
			if (e->action) {
				action = Action(e->action->trigger, e->action->guard, e->action->behavior);
			}
			
			element = new Transition(collection, e->id, source_element, target_element,
									 action, polyline, source_point, target_point, label_point,
									 _color);
			break;

		case cybEdgeComment:
			CYB_ASSERT(source_element->get_type() == elementComment ||
					   source_element->get_type() == elementFormalComment);
			CYB_ASSERT(e->comment_subject);

			comment = static_cast<Comment*>(source_element);
			
			if (e->comment_subject->type == cybCommentSubjectNode) {
				comment->add_subject(CommentSubject(e->id, target_element, source_point, target_point, polyline));
			} else {
				CommentSubjectType cst;
				if (e->comment_subject->type == cybCommentSubjectNameFragment) {
					cst = commentSubjectName;
				} else if (e->comment_subject->type == cybCommentSubjectDataFragment) {
					cst = commentSubjectData;
				} else {
					throw CybMLException("Unsupported comment subject type " + std::to_string(e->comment_subject->type));
				}
				CYB_ASSERT(e->comment_subject->fragment);
				comment->add_subject(CommentSubject(e->id, target_element, cst, e->comment_subject->fragment,
													source_point, target_point, polyline));
			}			
			break;
			
		default:
			throw CybMLException("Unsupported edge type " + std::to_string(e->type));
		}

		collection->add_element(element);
	}	
}

void Document::set_name(const Name& _name)
{
	Element::set_name(_name);
	metainfo.name = _name;
	update_metainfo_element();
}

void Document::load(const String& path, DocumentFormat f)
{
	reset();
	CyberiadaDocument doc;
	int res = cyberiada_init_sm_document(&doc);
	CYB_ASSERT(res == CYBERIADA_NO_ERROR);

	res = cyberiada_read_sm_document(&doc, path.c_str(), CyberiadaXMLFormat(f), CYBERIADA_NO_ERROR);
	if (res != CYBERIADA_NO_ERROR) {
		cyberiada_cleanup_sm_document(&doc);
		CYB_CHECK_RESULT(res);
	}

	try {		

		CYB_ASSERT(doc.format);
		format_str = doc.format;
		if (format_str == DEFAULT_GRAPHML_FORMAT) {
			format = formatCyberiada10;
		} else {
			format = formatLegacyYED;
		}
		
		CYB_ASSERT(doc.meta_info);
		CYB_ASSERT(doc.meta_info->standard_version);
		metainfo.standard_version = doc.meta_info->standard_version;
		if (doc.meta_info->platform_name) {
			metainfo.platform_name = doc.meta_info->platform_name;
		}
		if (doc.meta_info->platform_version) {
			metainfo.platform_version = doc.meta_info->platform_version;
		}
		if (doc.meta_info->platform_language) {
			metainfo.platform_language = doc.meta_info->platform_language;
		}
		if (doc.meta_info->target_system) {
			metainfo.target_system = doc.meta_info->target_system;
		}
		if (doc.meta_info->name) {
			set_name(doc.meta_info->name);
		}
		if (doc.meta_info->author) {
			metainfo.author = doc.meta_info->author;
		}
		if (doc.meta_info->contact) {
			metainfo.contact = doc.meta_info->contact;
		}
		if (doc.meta_info->description) {
			metainfo.description = doc.meta_info->description;
		}
		if (doc.meta_info->version) {
			metainfo.version = doc.meta_info->version;
		}
		if (doc.meta_info->date) {
			metainfo.date = doc.meta_info->date;
		}
		if (doc.meta_info->markup_language) {
			metainfo.markup_language = doc.meta_info->markup_language;
		}
		metainfo.transition_order_flag = doc.meta_info->transition_order_flag == 2;
		metainfo.event_propagation_flag = doc.meta_info->event_propagation_flag == 2;
		
		for (CyberiadaSM* sm = doc.state_machines; sm; sm = sm->next) {
			CyberiadaNode* root = sm->nodes;
			CYB_ASSERT(root);
			CYB_ASSERT(root->type == cybNodeSM);
			CYB_ASSERT(!root->next);
			CYB_ASSERT(root->id);
			StateMachine* new_sm;
			if (root->title) {
				new_sm = new_state_machine(root->id, root->title, root->geometry_rect);
			} else {
				new_sm = new_state_machine(root->id, "", root->geometry_rect);
			}
			CYB_ASSERT(new_sm);
			if (root->children) {
				import_nodes_recursively(new_sm, root->children);
			}
			if (sm->edges) { 
				import_edges(new_sm, sm->edges);
			}
		}
	} catch (const CybMLException& e) {
		cyberiada_cleanup_sm_document(&doc);
		throw CybMLException(e.str());		
	} catch (const Exception& e) {
		cyberiada_cleanup_sm_document(&doc);
		throw AssertException("Internal load error: " + e.str());
	}
	
	cyberiada_cleanup_sm_document(&doc);
}

void Document::update_metainfo_element()
{
	std::list<StateMachine*> sms = get_state_machines();
	if (sms.size() == 0) {
		return ;
	}

	String new_meta_comment;
	CyberiadaMetainformation* meta = export_meta();
	char* buffer = NULL;
	cyberiada_encode_meta(meta, &buffer, NULL);	
	if (buffer) {
		new_meta_comment = buffer;
		free(buffer);
	}
	cyberiada_destroy_meta(meta);
	if (metainfo_element) {
		metainfo_element->set_body(new_meta_comment);
	} else {
		StateMachine* sm = *(sms.begin()); // first SM
		if (sm->has_children()) {
			Element* first = sm->first_element();
			if (first->get_type() == elementFormalComment &&
				first->has_name() &&
				first->get_name() == META_NODE_NAME) {
				
				metainfo_element = static_cast<Comment*>(first);
				CYB_ASSERT(metainfo_element);
				metainfo_element->set_body(new_meta_comment);
				return ;
			}
		}	
//		Comment* comment = new Comment(sm, META_NODE_ID, new_meta_comment, META_NODE_NAME, false);
//		sm->add_first_element(comment);
//		metainfo_element = comment;
	}
}

void Document::export_edges(CyberiadaEdge** edges, const StateMachine* sm) const
{
	CyberiadaEdge* edge;
	std::list<const Transition*> transitions = sm->get_transitions();
	for (std::list<const Transition*>::const_iterator i = transitions.begin(); i != transitions.end(); i++) {
		const Transition* t = *i;
		edge = t->to_edge();
		if (*edges) {
			CyberiadaEdge* e = *edges;
			while (e->next) e = e->next;
			e->next = edge;
		} else {
			*edges = edge;
		}
	}
	std::list<const Comment*> comments = sm->get_comments();
	for (std::list<const Comment*>::const_iterator j = comments.begin(); j != comments.end(); j++) {
		const Comment* c = *j;
		edge = c->subjects_to_edges();
		if (*edges) {
			CyberiadaEdge* e = *edges;
			while (e->next) e = e->next;
			e->next = edge;
		} else {
			*edges = edge;
		}
	}
}

CyberiadaMetainformation* Document::export_meta() const
{
	CyberiadaMetainformation* meta_info = cyberiada_new_meta();
	CYB_ASSERT(metainfo.standard_version == String(meta_info->standard_version));
	if (!metainfo.platform_name.empty()) {
		cyberiada_copy_string(&(meta_info->platform_name),
							  &(meta_info->platform_name_len),
							  metainfo.platform_name.c_str());
	}
	if (!metainfo.platform_version.empty()) {
		cyberiada_copy_string(&(meta_info->platform_version),
							  &(meta_info->platform_version_len),
							  metainfo.platform_version.c_str());
	}
	if (!metainfo.platform_language.empty()) {
		cyberiada_copy_string(&(meta_info->platform_language),
							  &(meta_info->platform_language_len),
							  metainfo.platform_language.c_str());
	}
	if (!metainfo.target_system.empty()) {
		cyberiada_copy_string(&(meta_info->target_system),
							  &(meta_info->target_system_len),
							  metainfo.target_system.c_str());
	}
	if (!metainfo.name.empty()) {
		cyberiada_copy_string(&(meta_info->name),
							  &(meta_info->name_len),
								  metainfo.name.c_str());
	}
	if (!metainfo.author.empty()) {
		cyberiada_copy_string(&(meta_info->author),
							  &(meta_info->author_len),
							  metainfo.author.c_str());
	}
	if (!metainfo.contact.empty()) {
		cyberiada_copy_string(&(meta_info->contact),
							  &(meta_info->contact_len),
							  metainfo.contact.c_str());
	}
	if (!metainfo.description.empty()) {
		cyberiada_copy_string(&(meta_info->description),
							  &(meta_info->description_len),
							  metainfo.description.c_str());
	}
	if (!metainfo.version.empty()) {
		cyberiada_copy_string(&(meta_info->version),
							  &(meta_info->version_len),
							  metainfo.version.c_str());
	}
	if (!metainfo.date.empty()) {
		cyberiada_copy_string(&(meta_info->date),
							  &(meta_info->date_len),
							  metainfo.date.c_str());
	}
	if (!metainfo.markup_language.empty()) {
		cyberiada_copy_string(&(meta_info->markup_language),
							  &(meta_info->markup_language_len),
							  metainfo.markup_language.c_str());
	}
	
	meta_info->transition_order_flag = metainfo.transition_order_flag ? 2: 1;
	meta_info->event_propagation_flag = metainfo.event_propagation_flag ? 2: 1;

	return meta_info;
}

void Document::save(const String& path, DocumentFormat f) const
{
	CyberiadaDocument doc;
	int res;
	
	if (f == formatDetect) {
		throw ParametersException("Bad save format " + std::to_string(f));
	} else if (f == formatLegacyYED) {
		if (children_count() != 1) {
			throw ParametersException("Legacy Berloga-YED format supports single-SM documents only");
		}
	}

	std::list<const StateMachine*> state_machines = get_state_machines();
	if (state_machines.empty()) {
		throw ParametersException("At least one state machine required");
	}
	
	res = cyberiada_init_sm_document(&doc);
	CYB_ASSERT(res == CYBERIADA_NO_ERROR);
	
	try {
		if (f == formatCyberiada10) {
			cyberiada_copy_string(&(doc.format), &(doc.format_len),
								  DEFAULT_GRAPHML_FORMAT.c_str());
		}
		
		doc.meta_info = export_meta();

		for (std::list<const StateMachine*>::const_iterator i = state_machines.begin(); i != state_machines.end(); i++) {
			const StateMachine* orig_sm = *i;
			CYB_ASSERT(orig_sm);
			CyberiadaSM* new_sm = cyberiada_new_sm();
			new_sm->nodes = orig_sm->to_node();
			export_edges(&(new_sm->edges), orig_sm);
			if (doc.state_machines) {
				CyberiadaSM* sm = doc.state_machines;
				while(sm->next) sm = sm->next;
				sm->next = new_sm;
			} else {
				doc.state_machines = new_sm;
			}
		}	
	} catch (const Exception& e) {
		cyberiada_cleanup_sm_document(&doc);
		throw AssertException("Internal save error: " + e.str());
	}
	
	res = cyberiada_write_sm_document(&doc, path.c_str(), CyberiadaXMLFormat(f), CYBERIADA_NO_ERROR);
	if (res != CYBERIADA_NO_ERROR) {
		cyberiada_cleanup_sm_document(&doc);
		CYB_CHECK_RESULT(res);
	}

	cyberiada_cleanup_sm_document(&doc);
}

String Document::get_format_str() const
{
	if (format == formatCyberiada10) {
		return DEFAULT_GRAPHML_FORMAT;
	} else {
		CYB_ASSERT(format == formatLegacyYED);
		return format_str;
	}
}

std::list<const StateMachine*> Document::get_state_machines() const
{
	std::list<const StateMachine*> result;
	for (ElementList::const_iterator i = children.begin(); i != children.end(); i++) {
		CYB_ASSERT((*i)->get_type() == elementSM);
		result.push_back(static_cast<const StateMachine*>(*i));
	}
	return result;
}

std::list<StateMachine*> Document::get_state_machines()
{
	std::list<StateMachine*> result;
	for (ElementList::const_iterator i = children.begin(); i != children.end(); i++) {
		CYB_ASSERT((*i)->get_type() == elementSM);
		result.push_back(static_cast<StateMachine*>(*i));
	}
	return result;
}

const StateMachine* Document::get_parent_sm(const Element* element) const
{
	if (element != NULL) {
		ElementType type = element->get_type();
		if (type == elementRoot) {
			return NULL;
		} else if (type == elementSM) {
			return static_cast<const StateMachine*>(element);
		} else {
			CYB_ASSERT(element->get_parent());
			return get_parent_sm(element->get_parent());
		}
	} else {
		return NULL;
	}
}

std::ostream& Document::dump(std::ostream& os) const
{
	Element::dump(os);
	os << ", format: '" << get_format_str() << "', meta: {";
	std::list<String> params;
	if (!metainfo.standard_version.empty()) {
		params.push_back("standard version: '" + metainfo.standard_version + "'");
	}
	if (!metainfo.platform_name.empty()) {
		params.push_back("platform name: '" + metainfo.platform_name + "'");
	}
	if (!metainfo.platform_version.empty()) {
		params.push_back("platform version: '" + metainfo.platform_version + "'");
	}
	if (!metainfo.platform_language.empty()) {
		params.push_back("platform language: '" + metainfo.platform_language + "'");
	}
	if (!metainfo.target_system.empty()) {
		params.push_back("target system: '" + metainfo.target_system + "'");
	}
	if (!metainfo.name.empty()) {
		params.push_back("name: '" + metainfo.name + "'");
	}
	if (!metainfo.author.empty()) {
		params.push_back("author: '" + metainfo.author + "'");
	}
	if (!metainfo.contact.empty()) {
		params.push_back("contact: '" + metainfo.contact + "'");
	}
	if (!metainfo.description.empty()) {
		params.push_back("description: '" + metainfo.description + "'");
	}
	if (!metainfo.version.empty()) {
		params.push_back("version: '" + metainfo.version + "'");
	}
	if (!metainfo.date.empty()) {
		params.push_back("date: '" + metainfo.date + "'");
	}
	if (!metainfo.markup_language.empty()) {
		params.push_back("markup language: '" + metainfo.markup_language + "'");
	}
	params.push_back(String("transition order: ") + (metainfo.transition_order_flag ? "exit first": "transition first"));
	params.push_back(String("event propagation: ") +  (metainfo.event_propagation_flag ? "propagate events": "block events"));
	for (std::list<String>::const_iterator i = params.begin(); i != params.end(); i++) {
		os << *i;
		if (std::next(i) != params.end()) {
			os << ", ";
		}
	}
	os << "}";
	ElementCollection::dump(os);
	return os;
}

ID Document::generate_sm_id() const
{
	std::list<const StateMachine*> sm = get_state_machines();
	size_t id_num = sm.size();
	ID result;
	do {
		std::ostringstream s;
		s << SM_ID_PREFIX << id_num;
		result = ID(s.str());
		id_num++;
	} while(find_element_by_id(result));	
	return result;
}

ID Document::generate_vertex_id(const Element* p) const
{
	size_t id_num = 0;
	ID result;
	String base_name;
	if (p != NULL && p->get_type() != elementRoot && p->get_type() != elementSM) {
		base_name = p->get_id();
	}
	do {
		std::ostringstream s;
		if (base_name.empty()) {
			s << VERTEX_ID_PREFIX << id_num;
		} else {
			s << base_name << QUALIFIED_NAME_SEPARATOR << VERTEX_ID_PREFIX << id_num;
		}
		result = ID(s.str());
		id_num++;
	} while(find_element_by_id(result));	
	return result;
}

ID Document::generate_transition_id(const String& source_id, const String& target_id) const
{
	std::ostringstream s;
	size_t id_num = 0;
	String base_name;
	s << source_id << TRANTISION_ID_SEP << target_id;
	base_name = s.str();
	
	String result = ID(base_name);
	while (find_element_by_id(result)) {
		std::ostringstream s2;
		s2 << base_name << TRANTISION_ID_NUM_SEP << id_num;
		result = ID(s2.str());
		id_num++;
	}	
	return result;
}
