#pragma once
#include "DrawObject.hpp"
#include <GL/freeglut.h>
#include <string>

class FlyingText :public DrawObject {
public:
	Point2d position;
	Point2d initial_pos;
	bool inview = false;
	void* font;
	std::string text;
	Color color;
	bool no_change_cord = false;
	void mimic_drawing() {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(0, 0, 0, 0); //transparent text
		glRasterPos2i(TP(typing_cursor));
		for (auto c : text) glutBitmapCharacter(font, c);
		int rasterpos[4];
		glGetIntegerv(GL_CURRENT_RASTER_POSITION, rasterpos);
		typing_cursor = Point2d(rasterpos[0], rasterpos[1]);
		glDisable(GL_BLEND);
	}
	FlyingText(Point2d pos, std::string str,Point2d _inipos, bool con = false, void* _font = GLUT_BITMAP_HELVETICA_18, Color _color = global_paint, bool inView = true) :
		position(pos),initial_pos(_inipos), font(_font), text(str), color(_color), inview(inView), no_change_cord(con) {
		position = typing_cursor;
		mimic_drawing();
	}
	void Update(int elapsed) override {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		float f = elapsed / (float)(timetolive);
		f = f * sin(pi / 2.0 * f);
		color.a = f;
		glColor4f(T4C(color));
		auto [x, y] = position;
		auto vec = (position - initial_pos) * color.a;
		glRasterPos2i(TP((vec + initial_pos)));
		for (auto c : text) glutBitmapCharacter(font, c);
		glDisable(GL_BLEND);
	}
};