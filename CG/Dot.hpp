#pragma once
#include "DrawObject.hpp"
#include <GL/freeglut.h>

class Dot : public DrawObject {
public:
	Point2d pos;
	float size = 1.0f;
	float inner_radius = 0.0f;
	int sec = 16;
	GLUquadricObj* circle = NULL;
	Color color;
	Dot(Point2d _pos,float sz = 1.0f,Color _color = global_paint,float _inner_radius = 0.0f,int _sec = 16)
		: pos(_pos),size(sz),color(_color),inner_radius(_inner_radius),sec(_sec) {
		circle = gluNewQuadric();
		gluQuadricDrawStyle(circle, GLU_FILL);
	}
	void Update(int elapsed) override {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(T4C(color));
		glPushMatrix();
		glTranslatef(toView(pos), 0.0f);
		gluDisk(circle, inner_radius, size, sec, 3);
		glPopMatrix();
		glEnd();
		glDisable(GL_BLEND);
	}
};