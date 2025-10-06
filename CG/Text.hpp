#pragma once
#include "DrawObject.hpp"
#include <GL/freeglut.h>
#include <string>

class Text :public DrawObject {
public:
	Point2d position;
	bool inview = false;
	void* font;
	std::string text;
	Color color;
	bool no_change_cord = false;
	Text(Point2d pos,std::string str,bool con = false,void* _font = GLUT_BITMAP_HELVETICA_18,Color _color = global_paint,bool inView = true):
		position(pos),font(_font),text(str),color(_color),inview(inView), no_change_cord(con){
	}
	void Update(int elapsed) override {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(T4C(color));
		if (!no_change_cord) {
			if (inview) glRasterPos2d(toView(position));
			else glRasterPos2d(TP(position));
			for (auto c : text) glutBitmapCharacter(font, c);
		}
		else {
			glRasterPos2i(TP(typing_cursor));
			for (auto c : text) glutBitmapCharacter(font, c);
			int rasterpos[4];
			glGetIntegerv(GL_CURRENT_RASTER_POSITION, rasterpos);
			typing_cursor = Point2d(rasterpos[0], rasterpos[1]);
		}
		glDisable(GL_BLEND);
	}
};