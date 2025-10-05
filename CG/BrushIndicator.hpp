#pragma once
#include "UIElement.hpp"
#include "Dot.hpp"
#include <GL/freeglut.h>
#include <sstream>
#include <chrono>


class BrushIndicator : public Animated {
public:
	Dot* circle; 
	// teal : rgb(0, 150, 136)
	Color teal = {0,0.5859375,0.53125};
	float radius = 0.0f;
	BrushIndicator()  {
		radius = brush_size / 2.0f;
	};
	
	void Update(int elapsed) override {
		float prog = 1 - (elapsed / (float)timetolive);
		teal.a = prog;
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		circle = new Dot(projected_mouse, radius + 4.0f,teal,radius,1000);
		circle->Update(elapsed);
		glColor4f(T4C(teal));
		glRasterPos2d(toView(projected_mouse + Point2d(radius,radius)));
		std::stringstream ss;
		ss << std::fixed << std::setprecision(2);
		ss << "size : " << brush_size;
		std::string display_text = ss.str();
		for (auto c : display_text)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
		glDisable(GL_BLEND);

	}
};