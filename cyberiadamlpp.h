/* -----------------------------------------------------------------------------
 * The Cyberiada GraphML library implemention
 *
 * The C++ library header
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

#ifndef __CYBERIADA_ML_CPP_H
#define __CYBERIADA_ML_CPP_H

#include <string>
#include <list>
#include <vector>
#include <ostream>
#include <cyberiada/cyberiadaml.h>

// -----------------------------------------------------------------------------
// The Cyberiada GraphML classes
// -----------------------------------------------------------------------------

namespace Cyberiada {

// -----------------------------------------------------------------------------
// Base types
// -----------------------------------------------------------------------------
    // Cyberiada diagram element types:
	enum ElementType {
		elementRoot = 0,             // document (root namespace)
		elementSM,                   // state machine
		elementSimpleState,          // simple state
		elementCompositeState,       // composite state
		elementComment,              // informal (human-readable) comment node
		elementFormalComment,        // formal (machine-readable) comment node
		elementInitial,              // initial pseudostate
		elementFinal,                // final state
		elementChoice,               // choice pseudostate
		elementTerminate,            // terminate pseudostate
		elementTransition            // transition
	};

	typedef std::string String;
	typedef String ID;
	typedef String Name;
	typedef String QualifiedName;
	typedef String Color;

	const String QUALIFIED_NAME_SEPARATOR = "::";

// -----------------------------------------------------------------------------
// Base Element
// -----------------------------------------------------------------------------
	class Element {
	public:
		Element(Element* parent, ElementType type, const ID& id);
		Element(Element* parent, ElementType type, const ID& id, const Name& name);
		virtual ~Element() {}

		ElementType            get_type() const { return type; }

		const ID&              get_id() const { return id; }

		bool                   has_name() const { return name_is_set; }
		const Name&            get_name() const { return name; }
		virtual void           set_name(const Name& name);
		bool                   has_qualified_name() const;
		QualifiedName          qualified_name() const;

		bool                   is_root() const { return !parent; }
		const Element*         get_parent() const { return parent; }
		Element*               get_parent() { return parent; }
		virtual bool           has_children() const { return false; }
		virtual size_t         children_count() const { return 0; }
		virtual size_t         elements_count() const { return 1; }
		virtual int            index() const;

		virtual bool           has_geometry() const = 0;

		friend std::ostream&   operator<<(std::ostream& os, const Element& e);
		virtual CyberiadaNode* to_node() const;

		virtual std::string    dump_to_str() const;

	protected:		
		Element*               find_root();
		void                   set_type(ElementType t) { type = t; };
		virtual std::ostream&  dump(std::ostream& os) const;

	private:		
		ElementType            type;
		ID                     id;
		Name	               name;
		bool                   name_is_set;
		Element*               parent;
	};

	std::ostream& operator<<(std::ostream& os, const Element& e);

// -----------------------------------------------------------------------------
// Geometry
// -----------------------------------------------------------------------------	
	struct Point {
		Point(): valid(false) {}
		Point(float _x, float _y):
			valid(true), x(_x), y(_y) {}
		Point(CyberiadaPoint* p);

		CyberiadaPoint* c_point() const;
		
		bool   valid;
		float  x, y;
	};
	
	struct Rect {
		Rect(): valid(false) {}
		Rect(float _x, float _y, float _width, float _height):
			valid(true), x(_x), y(_y), width(_width), height(_height) {}
		Rect(CyberiadaRect* r);

		CyberiadaRect* c_rect() const;
		
		bool   valid;
		float  x, y;
		float  width, height;
	};
	
	typedef std::list<Point> Polyline;

	std::ostream& operator<<(std::ostream& os, const Point& p);
	std::ostream& operator<<(std::ostream& os, const Rect& r);
	std::ostream& operator<<(std::ostream& os, const Polyline& pl);

	CyberiadaPolyline* c_polyline(const Polyline& polyline);
	
// -----------------------------------------------------------------------------
// Comment
// -----------------------------------------------------------------------------
	enum CommentSubjectType {
		commentSubjectElement = 0,
		commentSubjectName,
		commentSubjectData
	};
	
	class CommentSubject {
	public:
		CommentSubject(const ID& id, Element* element,
					   const Point& source = Point(), const Point& target = Point(), const Polyline& pl = Polyline());
		CommentSubject(const ID& id, Element* element, CommentSubjectType type, const String& fragment,
					   const Point& source = Point(), const Point& target = Point(), const Polyline& pl = Polyline());
		
		CommentSubject&        operator=(const CommentSubject& cs);

		const ID&              get_id() const { return id; }
		CommentSubjectType     get_type() const { return type; }
		const Element*         get_element() const { return element; }
		Element*               get_element() { return element; }

		bool                   has_fragment() const { return has_frag; }
		const String&          get_fragment() const { return fragment; }
		
		bool                   has_geometry() const { return source_point.valid || target_point.valid || has_polyline(); }
		bool                   has_geometry_source_point() const { return source_point.valid; }
		bool                   has_geometry_target_point() const { return target_point.valid; }
		bool                   has_polyline() const { return !polyline.empty(); }
		const Point&           get_geometry_source_point() const { return source_point; }
		const Point&           get_geometry_target_point() const { return target_point; }
		const Polyline&        get_geometry_polyline() const { return polyline; }

	protected:
		std::ostream&          dump(std::ostream& os) const;
		friend std::ostream&   operator<<(std::ostream& os, const CommentSubject& cs);
		
	private:
		CommentSubjectType     type;
		ID                     id;
		Element*               element;
		bool                   has_frag;
		String                 fragment;
		Point                  source_point;
		Point                  target_point;
		Polyline               polyline;
	};

	std::ostream& operator<<(std::ostream& os, const CommentSubject& cs);

	class Comment: public Element {
	public:
		Comment(Element* parent, const ID& id, const String& body, bool human_readable = true,
				const String& markup = String(), const Rect& rect = Rect(), const Color& color = Color());
		Comment(Element* parent, const ID& id, const String& body, const Name& name, bool human_readable,
				const String& markup = String(), const Rect& rect = Rect(), const Color& color = Color());

		bool                             is_human_readable() const { return human_readable; }
		bool                             is_machine_readable() const { return !human_readable; }

		bool                             has_body() const { return !body.empty(); }
		void                             set_body(const String& b) { body = b; }
		
		bool                             has_subjects() const { return !subjects.empty(); }
		const std::list<CommentSubject>& get_subjects() const { return subjects; }
	    const CommentSubject&            add_subject(const CommentSubject& s);
		void                             remove_subject(CommentSubjectType type, const String& fragment);

		virtual bool                     has_geometry() const { return geometry_rect.valid; }
		const Rect&                      get_geometry_rect() const { return geometry_rect; }

		virtual bool                     has_children() const { return false; }

        bool                             has_color() const { return !color.empty(); }
		const Color&                     get_color() const { return color; }

		bool                             has_markup() const { return !markup.empty(); }
		const String&                    get_markup() const { return markup; }

		virtual CyberiadaNode*           to_node() const;
		virtual CyberiadaEdge*           subjects_to_edges() const;

	protected:
	    virtual std::ostream&            dump(std::ostream& os) const;
	
	private:
		void                             update_comment_type();

		String                           body;
		String                           markup;
		bool                             human_readable;
		Rect                             geometry_rect;
		std::list<CommentSubject>        subjects;
		Color                            color;
	};

// -----------------------------------------------------------------------------
// Vertex
// -----------------------------------------------------------------------------
	class Vertex: public Element {
	public:
		Vertex(Element* parent, ElementType type, const ID& id, const Point& pos = Point());
		Vertex(Element* parent, ElementType type, const ID& id, const Name& name, const Point& pos = Point());

		virtual bool           has_geometry() const { return geometry_point.valid; }
		const Point&           get_geometry_point() const { return geometry_point; }

		virtual bool           has_children() const { return false; }

		virtual CyberiadaNode* to_node() const;
		
	protected:
	    virtual std::ostream&  dump(std::ostream& os) const;
		
	private:
		Point                  geometry_point;
	};

// -----------------------------------------------------------------------------
// Collection of Elements
// (the combination of Namespace and Region from the PRIMS standard)
// -----------------------------------------------------------------------------
	typedef std::list<const Element*> ConstElementList;
	typedef std::list<Element*>       ElementList;
	typedef std::vector<ElementType>  ElementTypes;
	
	class ElementCollection: public Element {
	public:
		ElementCollection(Element* parent, ElementType type, const ID& id,
						  const Name& name, const Rect& rect = Rect(), const Color& color = Color());
		virtual ~ElementCollection();

		bool                     has_qualified_name(const ID& element_id) const;
		QualifiedName            qualified_name(const ID& element_id) const;

		virtual bool             has_children() const { return !children.empty(); }
		virtual size_t           children_count() const { return children.size(); }
		virtual size_t           elements_count() const;
		const Element*           first_element() const;
		Element*                 first_element();
		const Element*           get_element(int index) const;
		Element*                 get_element(int index);
		const Element*           find_element_by_id(const ID& id) const;
		Element*                 find_element_by_id(const ID& id);
		ConstElementList         find_elements_by_type(ElementType type) const;
		ConstElementList         find_elements_by_types(const ElementTypes& types) const;
		ElementList              find_elements_by_type(ElementType type);
		ElementList              find_elements_by_types(const ElementTypes& types);
		bool                     has_initial() const;
		virtual int              element_index(const Element* e) const;

		virtual void             add_element(Element* e);
		virtual void             add_first_element(Element* e);
		virtual void             remove_element(const ID& id);
		void                     clear();

		std::list<const Vertex*> get_vertexes() const;
		std::list<Vertex*>       get_vertexes();

		virtual bool             has_geometry() const { return geometry_rect.valid; }
		const Rect&              get_geometry_rect() const { return geometry_rect; }

        bool                     has_color() const { return !color.empty(); }
		const Color&             get_color() const { return color; }

		virtual CyberiadaNode*   to_node() const;

	protected:
		virtual std::ostream&    dump(std::ostream& os) const;

		ElementList              children;
		
	private:
		Rect                     geometry_rect;
		Color                    color;
	};

// -----------------------------------------------------------------------------
// Pseudostate
// -----------------------------------------------------------------------------
	class Pseudostate: public Vertex {
	public:
		Pseudostate(Element* parent, ElementType type, const ID& id, const Point& p = Point());
		Pseudostate(Element* parent, ElementType type, const ID& id, const Name& name, const Point& p = Point());
	};

// -----------------------------------------------------------------------------
// Initial pseudostate
// -----------------------------------------------------------------------------
	class InitialPseudostate: public Pseudostate {
	public:
		InitialPseudostate(Element* parent, const ID& id, const Point& p = Point());
		InitialPseudostate(Element* parent, const ID& id, const Name& name, const Point& p = Point());
	};

// -----------------------------------------------------------------------------
// Choice pseudostate
// -----------------------------------------------------------------------------
	class ChoicePseudostate: public Pseudostate {
	public:
		ChoicePseudostate(Element* parent, const ID& id,
						  const Rect& r = Rect(), const Color& color = Color());
		ChoicePseudostate(Element* parent, const ID& id, const Name& name,
						  const Rect& r = Rect(), const Color& color = Color());

		virtual bool           has_geometry() const { return geometry_rect.valid; }
		const Rect&            get_geometry_rect() const { return geometry_rect; }

        bool                   has_color() const { return !color.empty(); }
		const Color&           get_color() const { return color; }

		virtual CyberiadaNode* to_node() const;
		
	protected:
	    virtual std::ostream&  dump(std::ostream& os) const;

		Rect                   geometry_rect;
		Color                  color;
	};

// -----------------------------------------------------------------------------
// Initial pseudostate
// -----------------------------------------------------------------------------
	class TerminatePseudostate: public Pseudostate {
	public:
		TerminatePseudostate(Element* parent, const ID& id, const Point& p = Point());
		TerminatePseudostate(Element* parent, const ID& id, const Name& name, const Point& p = Point());
	};
	
// -----------------------------------------------------------------------------
// Final state
// -----------------------------------------------------------------------------
	class FinalState: public Vertex {
	public:
		FinalState(Element* parent, const ID& id, const Point& point = Point());
		FinalState(Element* parent, const ID& id, const Name& name, const Point& point = Point());
	};

// -----------------------------------------------------------------------------
// Action
// -----------------------------------------------------------------------------	

	// Cyberiada action types:
	typedef enum {
		actionTransition,
		actionEntry,
		actionExit
	} ActionType;

	typedef String Event; 
	typedef String Guard;
	typedef String Behavior;
	
	class Action {
	public:
		Action(ActionType type, const Behavior& behavior = Behavior());
		Action(const Event& trigger = Event(), const Guard& guard = Guard(), const Behavior& behavior = Behavior());

		bool                   is_empty_transition() const { return (type == actionTransition && !has_trigger() &&
																	 !has_guard() && !has_behavior()); }
		ActionType             get_type() const { return type; }
		bool                   has_trigger() const { return !trigger.empty(); }
		const Event&           get_trigger() const { return trigger; }
		bool                   has_guard() const { return !guard.empty(); }
		const Guard&           get_guard() const { return guard; }
		bool                   has_behavior() const { return !behavior.empty(); }
		const Behavior&        get_behavior() const { return behavior; }

	protected:
		std::ostream&          dump(std::ostream& os) const;
		friend std::ostream&   operator<<(std::ostream& os, const Action& a);
		
	private:
		ActionType             type;
		Event                  trigger;
		Guard                  guard;
		Behavior               behavior;
	};

	std::ostream& operator<<(std::ostream& os, const Action& a);
	
// -----------------------------------------------------------------------------
// State
// -----------------------------------------------------------------------------
	class State: public ElementCollection {
	public:
		State(Element* parent, const ID& id, const Name& name,
			  const Rect& r = Rect(), const Color& color = Color());

		virtual void             add_element(Element* e);
		virtual void             remove_element(const ID& id);
		
		bool                     is_simple_state() const { return get_type() == elementSimpleState; }
		bool                     is_composite_state() const { return get_type() == elementCompositeState; }

		std::list<const State*>  get_substates() const;
		std::list<State*>        get_substates();

		bool                     has_actions() const { return !actions.empty(); }
		const std::list<Action>& get_actions() const { return actions; }
		std::list<Action>&       get_actions() { return actions; }
		void                     add_action(const Action& a);
		
		virtual CyberiadaNode*   to_node() const;
		
	protected:
		virtual std::ostream&    dump(std::ostream& os) const;
		void                     update_state_type();
		
		std::list<Action>        actions;
	};

// -----------------------------------------------------------------------------
// Transition
// -----------------------------------------------------------------------------
	class Transition: public Element {
	public:
		Transition(Element* parent, const ID& id, Element* source, Element* target, const Action& action,
				   const Polyline& pl = Polyline(), const Point& sp = Point(), const Point& tp = Point(),
				   const Point& label = Point(), const Color& color = Color());

		const Element*         source_element() const { return source; }
		const Element*         target_element() const { return target; }

		bool                   has_action() const { return (action.has_trigger() ||
														   action.has_guard() ||
														   action.has_behavior()); }
		const Action&          get_action() const { return action; }
		Action&                get_action() { return action; }
		
		bool                   has_geometry() const { return (source_point.valid ||
															 target_point.valid ||
															 label_point.valid ||
															 has_polyline()); }
		bool                   has_polyline() const { return !polyline.empty(); }
		bool                   has_geometry_source_point() const { return source_point.valid; }
		bool                   has_geometry_target_point() const { return target_point.valid; }
		bool                   has_geometry_label_point() const { return label_point.valid; }
		const Polyline&        get_geometry_polyline() const { return polyline; }
		const Point&           get_source_point() const { return source_point; }
		const Point&           get_target_point() const { return target_point; }
		const Point&           get_label_point() const { return label_point; }

		bool                   has_color() const { return !color.empty(); }

		virtual CyberiadaEdge* to_edge() const;
		
	protected:
		virtual std::ostream&  dump(std::ostream& os) const;

	private:
		Element*               source;
		Element*               target;
		Action                 action;
		Point                  source_point;
		Point                  target_point;
		Point                  label_point;
		Polyline               polyline;
		Color                  color;
	};

// -----------------------------------------------------------------------------
// State Machine
// -----------------------------------------------------------------------------
	class StateMachine: public ElementCollection {
	public:
		StateMachine(Element* parent, const ID& id, const Name& name = "", const Rect& r = Rect());

		std::list<const Comment*>    get_comments() const;
		std::list<Comment*>          get_comments();
		std::list<const Transition*> get_transitions() const;
		std::list<Transition*>       get_transitions();

	protected:
		virtual std::ostream&        dump(std::ostream& os) const;
	};

// -----------------------------------------------------------------------------
// Cyberiada-GraphML document
// -----------------------------------------------------------------------------

	enum DocumentFormat {
		formatCyberiada10 = 0,                      // Cyberiada 1.0 format
		formatLegacyYED = 1,                        // Legacy YED-based Berloga/Ostranna format 
		formatDetect = 99                           // Format is not specified and will be detected while loading
	};

	struct DocumentMetainformation {
		String                         standard_version;      // PRIMS standard version
		String                         platform_name;         // target platform name
		String                         platform_version;      // target platform version
		String                         platform_language;     // target platform language
		String                         target_system;         // target system controlled by the SM
		String                         name;                  // document name
		String                         author;                // document author
		String                         contact;               // document author's contact
		String                         description;           // document description 
		String                         version;               // document version
		String                         date;                  // document date
		String                         markup_language;       // default comments' markup language
		bool                           transition_order_flag; // false = transition first; true = exit first
		bool                           event_propagation_flag;// false = block events; true = propagate events
	};
	
	class Document: public ElementCollection {
	public: 
		Document();

		void                           reset();
		StateMachine*                  new_state_machine(const String& sm_name, const Rect& r = Rect());
		StateMachine*                  new_state_machine(const ID& id, const String& sm_name, const Rect& r = Rect());
		State*                         new_state(ElementCollection* parent, const String& state_name, 
												 const Action& a = Action(), const Rect& r = Rect(), const Color& color = Color());
		State*                         new_state(ElementCollection* parent, const ID& id, const String& state_name,
												 const Action& a = Action(), const Rect& r = Rect(), const Color& color = Color());
		InitialPseudostate*            new_initial(ElementCollection* parent, const Point& p = Point());
		InitialPseudostate*            new_initial(ElementCollection* parent, const Name& name, const Point& p = Point());
		InitialPseudostate*            new_initial(ElementCollection* parent, const ID& id, const Name& name, const Point& p = Point());
		FinalState*                    new_final(ElementCollection* parent, const Point& point = Point());
		FinalState*                    new_final(ElementCollection* parent, const Name& name, const Point& point = Point());
		FinalState*                    new_final(ElementCollection* parent, const ID& id, const Name& name, const Point& point = Point());
		ChoicePseudostate*             new_choice(ElementCollection* parent,
												  const Rect& r = Rect(), const Color& color = Color());
		ChoicePseudostate*             new_choice(ElementCollection* parent, const Name& name,
												  const Rect& r = Rect(), const Color& color = Color());
		ChoicePseudostate*             new_choice(ElementCollection* parent, const ID& id, const Name& name,
												  const Rect& r = Rect(), const Color& color = Color());
		TerminatePseudostate*          new_terminate(ElementCollection* parent, const Point& p = Point());
		TerminatePseudostate*          new_terminate(ElementCollection* parent, const Name& name, const Point& p = Point());
		TerminatePseudostate*          new_terminate(ElementCollection* parent, const ID& id, const Name& name, const Point& p = Point());
		Transition*                    new_transition(StateMachine* sm, Element* source, Element* target,
													  const Action& action, const Polyline& pl = Polyline(),
													  const Point& sp = Point(), const Point& tp = Point(),
													  const Point& label = Point(), const Color& color = Color());
		Transition*                    new_transition(StateMachine* sm, const ID& id, Element* source, Element* target,
													  const Action& action, const Polyline& pl = Polyline(),
													  const Point& sp = Point(), const Point& tp = Point(),
													  const Point& label = Point(), const Color& color = Color());
		Comment*                       new_comment(ElementCollection* parent, const String& body,
												   const Rect& rect = Rect(), const Color& color = Color(),
												   const String& markup = String());
		Comment*                       new_comment(ElementCollection* parent, const String& name, const String& body,
												   const Rect& rect = Rect(), const Color& color = Color(),
												   const String& markup = String());
		Comment*                       new_comment(ElementCollection* parent, const ID& id, const String& name, const String& body,
												   const Rect& rect = Rect(), const Color& color = Color(),
												   const String& markup = String());
		Comment*                       new_formal_comment(ElementCollection* parent, const String& body,
														  const Rect& rect = Rect(), const Color& color = Color(),
														  const String& markup = String());
		Comment*                       new_formal_comment(ElementCollection* parent, const String& name, const String& body,
														  const Rect& rect = Rect(), const Color& color = Color(),
														  const String& markup = String());
		Comment*                       new_formal_comment(ElementCollection* parent, const ID& id, const String& name, const String& body,
														  const Rect& rect = Rect(), const Color& color = Color(),
														  const String& markup = String());
		const CommentSubject&          add_comment_to_element(Comment* comment, Element* element,
															  const Point& source = Point(), const Point& target = Point(),
															  const Polyline& pl = Polyline());
		const CommentSubject&          add_comment_to_element(Comment* comment, Element* element, const ID& id,
															  const Point& source = Point(), const Point& target = Point(),
															  const Polyline& pl = Polyline());
		const CommentSubject&          add_comment_to_element_name(Comment* comment, Element* element, const String& fragment,
																   const Point& source = Point(), const Point& target = Point(),
																   const Polyline& pl = Polyline());
		const CommentSubject&          add_comment_to_element_name(Comment* comment, Element* element, const String& fragment, const ID& id,
																   const Point& source = Point(), const Point& target = Point(),
																   const Polyline& pl = Polyline());
		const CommentSubject&          add_comment_to_element_body(Comment* comment, Element* element, const String& fragment,
																   const Point& source = Point(), const Point& target = Point(),
																   const Polyline& pl = Polyline());
		const CommentSubject&          add_comment_to_element_body(Comment* comment, Element* element, const String& fragment, const ID& id,
																   const Point& source = Point(), const Point& target = Point(),
																   const Polyline& pl = Polyline());
		
		void                           load(const String& path, DocumentFormat f = formatDetect);
		void                           save(const String& path, DocumentFormat f = formatCyberiada10) const;

		virtual void                   set_name(const Name& name);
		const DocumentMetainformation& meta() const { return metainfo; }
		DocumentMetainformation&       meta() { return metainfo; }

		std::list<const StateMachine*> get_state_machines() const;
		std::list<StateMachine*>       get_state_machines();
		const StateMachine*            get_parent_sm(const Element* element) const;

	protected:
		virtual std::ostream&          dump(std::ostream& os) const;
		
	private:
		void                           update_metainfo_element();
		ID                             generate_sm_id() const;
		ID                             generate_vertex_id(const Element* parent) const;
		ID                             generate_transition_id(const String& source_id, const String& target_id) const;
		void                           import_nodes_recursively(ElementCollection* collection, CyberiadaNode* nodes);
		void                           import_edges(ElementCollection* collection, CyberiadaEdge* edges);
		void                           export_edges(CyberiadaEdge** edges, const StateMachine* sm) const;
		CyberiadaMetainformation*      export_meta() const;

		void                           check_cyberiada_error(int res, const String& msg = "") const;
		void                           check_nonempty_string(const String& s) const;
		void                           check_parent_element(const Element* parent) const;
		void                           check_id_uniqueness(const ID& id) const;
		void                           check_single_initial(const ElementCollection* parent) const;
		void                           check_transition_source(const Element* element) const;
		void                           check_transition_target(const Element* element) const;
		void                           check_comment_subject_element(const Element* element) const;
		void                           check_transition_action(const Action& action) const;
		
		String                         format;
		DocumentMetainformation        metainfo;
		Comment*                       metainfo_element;
	};
	
// -----------------------------------------------------------------------------
// Exceptions
// -----------------------------------------------------------------------------
	class Exception {
	public:
		Exception(const String& msg = "", const String& e = "Generic Error"):
			error_type(e), message(msg) {}

		String        str() const { return error_type + ": " + message; }

	private:
		String        error_type;
		String        message;
	};
// -----------------------------------------------------------------------------
	class FormatException: public Exception {
	public:
		FormatException(const String& msg = "", const String& e = "Format Exception"):
			Exception(e, msg) {}
	};
// -----------------------------------------------------------------------------
	class XMLException: public FormatException {
	public:
		XMLException(const String& msg = "", const String& e = "XML Exception"):
			FormatException(msg, e) {}
	};
// -----------------------------------------------------------------------------
	class CybMLException: public FormatException {
	public:
		CybMLException(const String& msg = "", const String& e = "CyberiadaML Exception"):
			FormatException(msg, e) {}
	};
// -----------------------------------------------------------------------------
	class ActionException: public CybMLException {
	public:
		ActionException(const String& msg = "", const String& e = "Action Exception"):
			CybMLException(msg, e) {}
	};
// -----------------------------------------------------------------------------
	class MetainformationException: public FormatException {
	public:
		MetainformationException(const String& msg = "", const String& e = "Metainfo Exception"):
			FormatException(msg, e) {}
	};
// -----------------------------------------------------------------------------
	class ParametersException: public Exception {
	public:
		ParametersException(const String& msg = "", const String& e = "Parameters Exception"):
			Exception(msg, e) {}
	};
// -----------------------------------------------------------------------------
	class NotFoundException: public ParametersException {
	public:
		NotFoundException(const String& msg = "", const String& e = "Not Found Exception"):
			ParametersException(msg, e) {}
	};
// -----------------------------------------------------------------------------
	class AssertException: public Exception {
	public:
		AssertException(const String& msg, const String& e = "Assert Exception"):
			Exception(msg, e) {}
	};
// -----------------------------------------------------------------------------

};

#endif
