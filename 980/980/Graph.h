//
//  Graph.h
//  980MergeSA2
//
//  Created by Hensin on 2016-03-30.
//  Copyright (c) 2016 Hensin. All rights reserved.
//
#ifndef ___80Merge__Graph__
#define ___80Merge__Graph__

#include <vector>
#include <iostream>
#include "Point.h"
#include "fltk.h"
#include "cluster.h"
#include "fplan.h"

namespace Graph_lib {
// defense against ill-behaved Linux macros:
#undef major
#undef minor

struct Color {
	enum Color_type {
		red=FL_RED, blue=FL_BLUE, green=FL_GREEN,
		yellow=FL_YELLOW, white=FL_WHITE, black=FL_BLACK,
		magenta=FL_MAGENTA, cyan=FL_CYAN, dark_red=FL_DARK_RED,
		dark_green=FL_DARK_GREEN, dark_yellow=FL_DARK_YELLOW, dark_blue=FL_DARK_BLUE,
		dark_magenta=FL_DARK_MAGENTA, dark_cyan=FL_DARK_CYAN
	};
	enum Transparency { invisible = 0, visible=255 };

	Color(Color_type cc) :c(Fl_Color(cc)), v(visible) { }
	Color(Color_type cc, Transparency vv) :c(Fl_Color(cc)), v(vv) { }
	Color(int cc) :c(Fl_Color(cc)), v(visible) { }
	Color(Transparency vv) :c(Fl_Color()), v(vv) { }

	int as_int() const { return c; }
	char visibility() const { return v; }
	void set_visibility(Transparency vv) { v=vv; }
private:
	unsigned char v;	// 0 or 1 for now
	Fl_Color c;
};

struct Line_style {
	enum Line_style_type {
		solid=FL_SOLID,				// -------
		dash=FL_DASH,				// - - - -
		dot=FL_DOT,					// ....... 
		dashdot=FL_DASHDOT,			// - . - . 
		dashdotdot=FL_DASHDOTDOT,	// -..-..
	};
	Line_style(Line_style_type ss) :s(ss), w(0) { }
	Line_style(Line_style_type lst, int ww) :s(lst), w(ww) { }
	Line_style(int ss) :s(ss), w(0) { }

	int width() const { return w; }
	int style() const { return s; }
private:
	int s;
	int w;
};

class Font {
public:
	enum Font_type {
		helvetica=FL_HELVETICA,
		helvetica_bold=FL_HELVETICA_BOLD,
		helvetica_italic=FL_HELVETICA_ITALIC,
		helvetica_bold_italic=FL_HELVETICA_BOLD_ITALIC,
		courier=FL_COURIER,
  		courier_bold=FL_COURIER_BOLD,
  		courier_italic=FL_COURIER_ITALIC,
  		courier_bold_italic=FL_COURIER_BOLD_ITALIC,
		times=FL_TIMES,
		times_bold=FL_TIMES_BOLD,
		times_italic=FL_TIMES_ITALIC,
		times_bold_italic=FL_TIMES_BOLD_ITALIC,
		symbol=FL_SYMBOL,
		screen=FL_SCREEN,
		screen_bold=FL_SCREEN_BOLD,
		zapf_dingbats=FL_ZAPF_DINGBATS
	};

	Font(Font_type ff) :f(ff) { }
	Font(int ff) :f(ff) { }

	int as_int() const { return f; }
private:
	int f;
};

template<class T> class Vector_ref {
	vector<T*> v;
	vector<T*> owned;
public:
	Vector_ref() {}

	Vector_ref(T* a, T* b=0, T* c=0, T* d=0)
	{
			if (a) push_back(a);
			if (b) push_back(b);
			if (c) push_back(c);
			if (d) push_back(d);
	}

	~Vector_ref() { for (int i=0; i<owned.size(); ++i) delete owned[i]; }

	void push_back(T& s) { v.push_back(&s); }
	void push_back(T* p) { v.push_back(p); owned.push_back(p); }

	// ???void erase(???)

	T& operator[](int i) { return *v[i]; }
	const T& operator[](int i) const { return *v[i]; }
	int size() const { return v.size(); }
};

typedef double Fct(double);

class Shape  {	// deals with color and style, and holds sequence of lines
protected:
	Shape() { }
	Shape(initializer_list<Point> lst);  // add() the Points to this Shape

//	Shape() : lcolor(fl_color()),
//		ls(0),
//		fcolor(Color::invisible) { }
	
	void add(Point p){ points.push_back(p); }
	void set_point(int i, Point p) { points[i] = p; }
public:
	void draw();					// deal with color and draw_lines
protected:
	virtual void draw_lines();	// simply draw the appropriate lines
public:
	virtual void move(int dx, int dy);	// move the shape +=dx and +=dy

	void set_color(Color col) { lcolor = col; }
	Color color() const { return lcolor; }

	void set_style(Line_style sty) { ls = sty; }
	Line_style style() const { return ls; }

	void set_fill_color(Color col) { fcolor = col; }
	Color fill_color() const { return fcolor; }

	Point point(int i) const { return points[i]; }
	int number_of_points() const { return int(points.size()); }

	virtual ~Shape() { }
	/*
	struct Window* attached;
	Shape(const Shape& a)
		:attached(a.attached), points(a.points), line_color(a.line_color), ls(a.ls)
	{
		if (a.attached)error("attempt to copy attached shape");
	}
	*/
	Shape(const Shape&) = delete;
	Shape& operator=(const Shape&) = delete;
private:
	vector<Point> points;	// not used by all shapes
	Color lcolor {static_cast<int>(fl_color())};
	Line_style ls {0};
	Color fcolor {Color::invisible};

//	Shape(const Shape&);
//	Shape& operator=(const Shape&);
};
 
//A shape used to show the merged clusters in terms of module
struct CLMO : Shape {
    CLMO() {};
    CLMO(vector<Cluster> clts, int t){
        clusters = clts;
        s=t;
    };

    void draw_lines();
    int s;
private:
    vector<Cluster> clusters;
};
    
//A shape used to draw the modules after packing
struct MO : Shape {
    MO() {};
    MO(Modules_Info moi, int t){
        modulesInfo = moi;
        s=t;
    };
    
    void draw_lines();
    int s;
private:
    Modules_Info modulesInfo;
};


}
#endif

