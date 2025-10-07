#pragma once
#include "UIElement.hpp"
#include "Dot.hpp"
#include <GL/freeglut.h>
#include <sstream>
#include <chrono>


class MouseIndicator : public Animated {
public:
	Dot* circle;
	// teal : rgb(0, 150, 136)
	Color teal = { 0,0.5859375,0.53125 };
	float radius = 0.0f;
	float now_indicate = 0;
	Point2d offseted;
	std::string text = "";
	bool custom_text;
	MouseIndicator() {
		offseted = Point2d(0, 50);
	};

	void Update(int elapsed) override {
		float prog = 1 - (elapsed / (float)timetolive);
		teal.a = prog;
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(T4C(teal));
		glRasterPos2d(toView(projected_mouse + offseted));
		std::stringstream ss;
		ss << std::fixed << std::setprecision(2);
		ss << text << now_indicate;
		std::string display_text = ss.str();
		if (custom_text) display_text = text;
		for (auto c : display_text)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
		glDisable(GL_BLEND);

	}
};