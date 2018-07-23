//
//  Window.h
//  980MergeSA2
//
//  Created by Hensin on 2016-03-30.
//  Copyright (c) 2016 Hensin. All rights reserved.
//
#ifndef ___80Merge__Window__
#define ___80Merge__Window__

#include "fltk.h"
#include "Point.h"

#include <string>
#include <vector>

using namespace std;

namespace Graph_lib {

class Shape;	// "forward declare" Shape
class Widget;   // "forward declare" Widget

class Window : public Fl_Window { 
public: 
	Window(int w, int h, const string& title );			// let the system pick the location
	Window(Point xy, int w, int h, const string& title );	// top left corner in xy
	virtual ~Window() { }

	int x_max() const { return w; }
	int y_max() const { return h; }

	void resize(int ww, int hh) { w=ww, h=hh; size(ww,hh); }

	void set_label(const string& s) { label(s.c_str()); }

	void attach(Shape& s);
	void attach(Widget& w);

	void detach(Shape& s);	// remove s from shapes 
	void detach(Widget& w);	// remove w from window (deactivate callbacks)

	void put_on_top(Shape& p);	// put p on top of other shapes

protected:
	void draw();
     
private:
	  vector<Shape*> shapes;	// shapes attached to window
	  int w,h;					// window size

	  void init();
}; 

int gui_main();	// invoke GUI library's main event loop

inline int x_max() { return Fl::w(); }	// width of screen in pixels
inline int y_max() { return Fl::h(); }	// height of screen in pixels
    
}

#endif
