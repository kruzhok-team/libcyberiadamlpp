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

	enum DocumentFormat {
		formatCyberiada10 = 0,                      // Cyberiada 1.0 format
		formatLegacyYED = 1,                        // Legacy YED-based Berloga/Ostranna format 
		formatDetect = 99                           // Format is not specified and will be detected while loading
	};

	enum DocumentGeometryFormat {
		geometryFormatNone,                         // No geometry
		geometryFormatLegacyYED,                    // Legacy YED geometry: absolute coordinates, edges with local center source/target poins 
		geometryFormatCyberiada10,                  // Cyberiada 1.0 geometry: local left-top coordinates, edges based on local border points
		geometryFormatQt,                           // Qt geometry: local centered coordinates, edges based on local border points
	};

	class Document;
	
// -----------------------------------------------------------------------------
// Geometry
// -----------------------------------------------------------------------------	
	struct Point {
		Point(): valid(false) {}
		Point(float _x, float _y):
			valid(true), x(_x), y(_y) {}
		Point(CyberiadaPoint* p);

		CyberiadaPoint* c_point() const;
		void  round();
		Point round() const;
		String to_str() const;

		bool   valid;
		float  x, y;
	};

	class Polyline: public std::list<Point> {
	public:
		Polyline(): std::list<Point>() {}
		
		CyberiadaPolyline* c_polyline() const;
		void round();
		String to_str();
	};

	struct Rect {
		Rect(): valid(false), x(0.0), y(0.0), width(0.0), height(0.0) {}
		Rect(float _x, float _y, float _width, float _height):
			valid(true), x(_x), y(_y), width(_width), height(_height) {}
		Rect(CyberiadaRect* r);

		void expand(const Point& p, const Document& d);
		void expand(const Rect& r, const Document& d);
		void expand(const Polyline& pl, const Document& d);
		void round();
		Rect round() const;		
		CyberiadaRect* c_rect() const;
		String to_str() const;

		bool operator==(const Rect& r) const;
		bool operator!=(const Rect& r) const;
		bool almost_equal(const Rect& r) const;
		
		bool   valid;
		float  x, y;
		float  width, height;
	};
	
	std::ostream& operator<<(std::ostream& os, const Point& p);
	std::ostream& operator<<(std::ostream& os, const Rect& r);
	std::ostream& operator<<(std::ostream& os, const Polyline& pl);
	
// -----------------------------------------------------------------------------
// Base Element
// -----------------------------------------------------------------------------
	class Element {
	public:
		Element(Element* parent, ElementType type, const ID& id);
		Element(Element* parent, ElementType type, const ID& id, const Name& name);
		Element(const Element& e);
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
		void                   update_parent(Element* p) { parent = p; }
		virtual bool           has_children() const { return false; }
		virtual size_t         children_count() const { return 0; }
		virtual size_t         elements_count() const { return 1; }
		int                    index() const;

		virtual bool           has_geometry() const = 0;
		virtual Rect           get_bound_rect(const Document& d) const = 0;
		virtual void           clean_geometry() = 0;
		virtual void           round_geometry() = 0;

		friend std::ostream&   operator<<(std::ostream& os, const Element& e);
		virtual CyberiadaNode* to_node() const;

		virtual std::string    dump_to_str() const;
		virtual Element*       copy(Element* parent) const = 0;

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
		CommentSubject(const CommentSubject& cs);
		
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
        Rect                   get_bound_rect(const Document& d) const;
		void                   clean_geometry();
		void                   round_geometry();

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
		Comment(const Comment& c);

		bool                             is_human_readable() const { return human_readable; }
		bool                             is_machine_readable() const { return !human_readable; }

		bool                             has_body() const { return !body.empty(); }
		const String&                    get_body() const { return body; }
		void                             set_body(const String& b) { body = b; }
		
		bool                             has_subjects() const { return !subjects.empty(); }
		const std::list<CommentSubject>& get_subjects() const { return subjects; }
	    const CommentSubject&            add_subject(const CommentSubject& s);
		void                             remove_subject(CommentSubjectType type, const String& fragment);

		bool                             has_geometry() const override { return geometry_rect.valid; }
		const Rect&                      get_geometry_rect() const { return geometry_rect; }
		Rect                             get_bound_rect(const Document& d) const override;
		void                             clean_geometry() override;
		void                             round_geometry() override;
		
		bool                             has_children() const override { return false; }

        bool                             has_color() const { return !color.empty(); }
		const Color&                     get_color() const { return color; }

		bool                             has_markup() const { return !markup.empty(); }
		const String&                    get_markup() const { return markup; }

		CyberiadaNode*                   to_node() const override;
		CyberiadaEdge*                   subjects_to_edges() const;
		Element*                         copy(Element* parent) const override;

	protected:
	    std::ostream&                    dump(std::ostream& os) const override;
	
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
		Vertex(const Vertex& v);

		bool                   has_geometry() const override { return geometry_point.valid; }
		const Point&           get_geometry_point() const { return geometry_point; }
		Rect                   get_bound_rect(const Document& d) const override;
		void                   clean_geometry() override;
		void                   round_geometry() override;
		
		bool                   has_children() const override { return false; }

		CyberiadaNode*         to_node() const override;
		
	protected:
	    std::ostream&          dump(std::ostream& os) const override;
		
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
		ElementCollection(const ElementCollection& ec);
		virtual ~ElementCollection();

		bool                     has_qualified_name(const ID& element_id) const;
		QualifiedName            qualified_name(const ID& element_id) const;

		bool                     has_children() const override { return !children.empty(); } 
		size_t                   children_count() const override { return children.size(); }
		virtual size_t           elements_count() const override;
		ConstElementList         get_children() const;
		const ElementList&       get_children() { return children; };
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
		int                      element_index(const Element* e) const;

		virtual void             add_element(Element* e);
		virtual void             add_first_element(Element* e);
		virtual void             remove_element(const ID& id);
		void                     clear();

		std::list<const Vertex*> get_vertexes() const;
		std::list<Vertex*>       get_vertexes();

		bool                     has_geometry() const override { return geometry_rect.valid; }
		const Rect&              get_geometry_rect() const { return geometry_rect; }
		Rect                     get_bound_rect(const Document& d) const override;
		void                     clean_geometry() override;
		void                     round_geometry() override;
		
        bool                     has_color() const { return !color.empty(); }
		const Color&             get_color() const { return color; }

		CyberiadaNode*           to_node() const override;
		
	protected:
		std::ostream&            dump(std::ostream& os) const override;
		void                     copy_elements(const ElementCollection& source);

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

		Element*       copy(Element* parent) const override;
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
		ChoicePseudostate(const ChoicePseudostate& cp);

		bool                   has_geometry() const override { return geometry_rect.valid; }
		const Rect&            get_geometry_rect() const { return geometry_rect; }
		Rect                   get_bound_rect(const Document& d) const override;
		void                   clean_geometry() override;
		void                   round_geometry() override;
		
        bool                   has_color() const { return !color.empty(); }
		const Color&           get_color() const { return color; }

		CyberiadaNode*         to_node() const override;
		Element*               copy(Element* parent) const override;
		
	protected:
	    std::ostream&          dump(std::ostream& os) const override;

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

		Element*               copy(Element* parent) const override;
	};
	
// -----------------------------------------------------------------------------
// Final state
// -----------------------------------------------------------------------------
	class FinalState: public Vertex {
	public:
		FinalState(Element* parent, const ID& id, const Point& point = Point());
		FinalState(Element* parent, const ID& id, const Name& name, const Point& point = Point());

		Element*       copy(Element* parent) const override;
	};

// -----------------------------------------------------------------------------
// Action
// -----------------------------------------------------------------------------	

	// Cyberiada action types:
	typedef enum {
		actionTransition = 0,
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
		State(const State& s);

		void                     add_element(Element* e) override;
		void                     remove_element(const ID& id) override;
		
		bool                     is_simple_state() const { return get_type() == elementSimpleState; }
		bool                     is_composite_state() const { return get_type() == elementCompositeState; }

		std::list<const State*>  get_substates() const;
		std::list<State*>        get_substates();

		bool                     has_actions() const { return !actions.empty(); }
		const std::list<Action>& get_actions() const { return actions; }
		std::list<Action>&       get_actions() { return actions; }
		void                     add_action(const Action& a);
		
		CyberiadaNode*           to_node() const override;
		Element*                 copy(Element* parent) const override;
		
	protected:
		std::ostream&            dump(std::ostream& os) const override;
		void                     update_state_type();
		
		std::list<Action>        actions;
	};

// -----------------------------------------------------------------------------
// Transition
// -----------------------------------------------------------------------------
	class Transition: public Element {
	public:
		Transition(Element* parent, const ID& id, const ID& source, const ID& target, const Action& action,
				   const Polyline& pl = Polyline(), const Point& sp = Point(), const Point& tp = Point(),
				   const Point& label_point = Point(), const Rect& label_rect = Rect(), const Color& color = Color());
		Transition(const Transition& t);

		const ID&              source_element_id() const { return source_id; }
		const ID&              target_element_id() const { return target_id; }

		bool                   has_action() const { return (action.has_trigger() ||
														   action.has_guard() ||
														   action.has_behavior()); }
		const Action&          get_action() const { return action; }
		Action&                get_action() { return action; }
		
		bool                   has_geometry() const override { return (source_point.valid ||
																	   target_point.valid ||
																	   label_point.valid ||
																	   label_rect.valid ||
																	   has_polyline()); }
		bool                   has_polyline() const { return !polyline.empty(); }
		bool                   has_geometry_source_point() const { return source_point.valid; }
		bool                   has_geometry_target_point() const { return target_point.valid; }
		bool                   has_geometry_label_point() const { return label_point.valid; }
		bool                   has_geometry_label_rect() const { return label_rect.valid; }
		const Polyline&        get_geometry_polyline() const { return polyline; }
		const Point&           get_source_point() const { return source_point; }
		const Point&           get_target_point() const { return target_point; }
		const Point&           get_label_point() const { return label_point; }
		const Rect&            get_label_rect() const { return label_rect; }
		Rect                   get_bound_rect(const Document& d) const override;
		void                   clean_geometry() override;
		void                   round_geometry() override;
		
		bool                   has_color() const { return !color.empty(); }
		const Color&           get_color() const { return color; }

		virtual CyberiadaEdge* to_edge() const;
		Element*       copy(Element* parent) const override;
		
	protected:
		std::ostream&  dump(std::ostream& os) const override;

	private:
		ID                     source_id;
		ID                     target_id;
		Action                 action;
		Point                  source_point;
		Point                  target_point;
		Point                  label_point;
		Rect                   label_rect;
		Polyline               polyline;
		Color                  color;
	};

// -----------------------------------------------------------------------------
// State Machine
// -----------------------------------------------------------------------------
	class StateMachine: public ElementCollection {
	public:
		StateMachine(Element* parent, const ID& id, const Name& name = "", const Rect& r = Rect());
		StateMachine(const StateMachine& sm);

		std::list<const Comment*>    get_comments() const;
		std::list<Comment*>          get_comments();
		std::list<const Transition*> get_transitions() const;
		std::list<Transition*>       get_transitions();

		//virtual Rect                 get_bound_rect(const Document& d) const;

		CyberiadaNode*               to_node(const Point& center) const;
		
		Element*                     copy(Element* parent) const override;
		
	protected:
		std::ostream&                dump(std::ostream& os) const override;
	};

// -----------------------------------------------------------------------------
// Cyberiada-GraphML document
// -----------------------------------------------------------------------------

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
		Document(DocumentGeometryFormat format = geometryFormatNone);
		Document(const Document& d);

		void                           reset(DocumentGeometryFormat format = geometryFormatNone);
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
													  const Point& label_point = Point(), const Rect& label_rect = Rect(),
													  const Color& color = Color());
		Transition*                    new_transition(StateMachine* sm, const ID& id, Element* source, Element* target,
													  const Action& action, const Polyline& pl = Polyline(),
													  const Point& sp = Point(), const Point& tp = Point(),
													  const Point& label_point = Point(), const Rect& label_rect = Rect(),
													  const Color& color = Color());
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
		
		void                           decode(const String& buffer,
											  DocumentFormat& format,
											  String& format_str,
											  DocumentGeometryFormat gf = geometryFormatQt,
											  bool reconstruct = false);
		void                           encode(String& buffer,
											  DocumentFormat f = formatCyberiada10,
											  bool round = false) const;

		void                           set_name(const Name& name) override;
		const DocumentMetainformation& meta() const { return metainfo; }
		DocumentMetainformation&       meta() { return metainfo; }
		DocumentGeometryFormat         get_geometry_format() const { return geometry_format; }
		
		std::list<const StateMachine*> get_state_machines() const;
		std::list<StateMachine*>       get_state_machines();
		const StateMachine*            get_parent_sm(const Element* element) const;
		StateMachine*                  get_parent_sm(const Element* element);

		bool                           has_geometry() const override { return geometry_format != geometryFormatNone; }
		Rect                           get_bound_rect() const;
		Rect                           get_bound_rect(const Document& d) const override;
		void                           convert_geometry(DocumentGeometryFormat geom_format);
		void                           reconstruct_geometry();
		void                           clean_geometry() override;
		
		Element*                       copy(Element* parent) const override;
		
	protected:
		std::ostream&                  dump(std::ostream& os) const override;
		void                           update_from_document(DocumentGeometryFormat gf,
															CyberiadaDocument* doc);
		void                           to_document(CyberiadaDocument* doc) const;
		
	private:
		void                           update_metainfo_element();
		ID                             generate_sm_id() const;
		ID                             generate_vertex_id(const Element* parent) const;
		ID                             generate_transition_id(const String& source_id, const String& target_id) const;
		void                           import_nodes_recursively(ElementCollection* collection, CyberiadaNode* nodes);
		void                           import_edges(ElementCollection* collection, CyberiadaEdge* edges);
		void                           export_edges(CyberiadaEdge** edges,
													const StateMachine* sm,
													const CyberiadaSM* new_sm) const;
		CyberiadaMetainformation*      export_meta() const;
		void                           set_geometry(DocumentGeometryFormat format);

		void                           check_cyberiada_error(int res, const String& msg = "") const;
		void                           check_nonempty_string(const String& s) const;
		void                           check_parent_element(const Element* parent) const;
		void                           check_id_uniqueness(const ID& id) const;
		void                           check_single_initial(const ElementCollection* parent) const;
		void                           check_transition_source(const Element* element) const;
		void                           check_transition_target(const Element* element) const;
		void                           check_comment_subject_element(const Element* element) const;
		void                           check_transition_action(const Action& action) const;
		void                           check_geometry_update(const Rect& r);
		void                           check_geometry_update(const Point& p);
		void                           check_geometry_update(const Polyline& pl);

		DocumentGeometryFormat         geometry_format;
		DocumentMetainformation        metainfo;
		Comment*                       metainfo_element;
		Point                          center_point;
	};

	class LocalDocument: public Document {
	public: 
		LocalDocument();
		LocalDocument(const Document& d, const String& part, DocumentFormat f = formatCyberiada10);
		LocalDocument(const LocalDocument& ld);

		void                           reset();
		void                           open(const String& path,
											DocumentFormat f = formatDetect,
											DocumentGeometryFormat gf = geometryFormatQt,
											bool reconstruct = false);
		void                           save(bool round = false);
		void                           save_as(const String& path,
											   DocumentFormat f = formatDetect,
											   bool round = false);

		DocumentFormat                 get_file_format() const { return file_format; }
		String                         get_file_format_str() const;

		Element*                       copy(Element* parent) const override;
		
	protected:
		std::ostream&                  dump(std::ostream& os) const override;
		
	private:
		String                         file_path;
		DocumentFormat                 file_format;
		String                         file_format_str;
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
	class NotImplementedException: public Exception {
	public:
		NotImplementedException(const String& msg, const String& e = "Not Implemented Exception"):
			Exception(msg, e) {}
	};
// -----------------------------------------------------------------------------

};

#endif
