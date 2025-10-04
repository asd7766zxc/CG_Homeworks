#pragma once
#include "DrawObject.hpp"
#include <GL/freeglut.h>

class Line : public DrawObject {
public:
	Point2d begin,end;
	float width = 1.0f;
	Color color;
	Line(Point2d bg,Point2d ed, float wd = 1.0f,Color _color = global_paint) 
		: begin(bg), end(ed),width(wd),color(_color) {
	}
	void Update(int elapsed) override {
		auto vec = begin - end;
		vec = unified(vec);
		auto l = rotate(vec, pi / 2) * width / 2.0;
		auto r = rotate(vec, -pi / 2) * width / 2.0;
		auto poly = convex_hull({begin + l,begin + r,end + l,end + r});
		glColor3f(TC(color));
		glBegin(GL_POLYGON);
		for (auto c : poly) {
			glVertex2f(toView(c));
		}
		glEnd();
		/*glColor3f(TC(color));
		glLineWidth(400.0f);
		glBegin(GL_LINES);
		glVertex2f(toView(begin));
		glVertex2f(toView(end));
		glEnd();*/
	}
};