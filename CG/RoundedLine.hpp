#pragma once
#include "DrawObject.hpp"
#include "Dot.hpp"
#include "Line.hpp"
#include <GL/freeglut.h>

class RoundedLine : public DrawObject {
public:
	Dot * a, * b;
	Line * l;
	Point2d begin, end;
	float width = 1.0f;
	Color color;
	RoundedLine(Point2d bg, Point2d ed, float wd = 1.0f,Color _color = global_paint) 
		: begin(bg), end(ed), width(wd),color(_color) {
		l = new Line(begin, end, wd, color);
		a = new Dot(begin, wd / 2.0f, color);
		b = new Dot(end, wd / 2.0f, color);
	}
	void Update(int elapsed) override {
		l->Update(elapsed);
		a->Update(elapsed);
		b->Update(elapsed);
	}
};