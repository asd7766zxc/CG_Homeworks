#pragma once
#include "DrawObject.hpp"
#include <GL/freeglut.h>

class Line : public DrawObject {
public:
	Point2d begin,end;
	Color begin_color, end_color;
	float width = 1.0f;
	Color color;
	Line(Point2d bg,Point2d ed, float wd = 1.0f,Color _color = global_paint) 
		: begin(bg), end(ed),width(wd),color(_color) {
		begin_color = _color;
		end_color = _color;
	}
	Line(Point2d bg, Point2d ed, float wd, Color _bg,Color _ed)
		: begin(bg), end(ed), width(wd), begin_color(_bg), end_color(_ed){
	}
	void Update(int elapsed) override {
		auto vec = begin - end;
		vec = unified(vec);
		auto l = rotate(vec, pi / 2) * width / 2.0;
		auto r = rotate(vec, -pi / 2) * width / 2.0;
		auto poly = convex_hull({begin + l,begin + r,end + l,end + r});
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(T4C(color));
		glBegin(GL_POLYGON);
		glColor4f(T4C(begin_color));
		glVertex2f(toView(poly[0]));
		glVertex2f(toView(poly[1]));
		glColor4f(T4C(end_color));
		glVertex2f(toView(poly[2]));
		glVertex2f(toView(poly[3]));
		glEnd();
		glDisable(GL_BLEND);
	}
};