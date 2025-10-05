#pragma once
#include "UIElement.hpp"
#include <GL/freeglut.h>


class ColorWheel : public UIElement {
public:
	Point2d position;
	Point2d cursor_pos;
	
	int sat_sec = 1;
	int hue_sec = 360;
	float value = 1.0f;
	float radius = 100.0f;
	bool enable_color_animation = false;

	ColorWheel(int _x, int _y) : position(_x, _y) {
		cursor_pos = position;
		global_paint = Color{ 1,1,1,1 };
	};
	void PickColor(Point2d color) {
		auto [r, theta] = color;
		cursor_pos = Point2d(radius * r * cos(theta) + position.x,radius * r * sin(theta) + position.y);
		float sat = r;
		int h = (theta / pi / 2.0f) * hue_sec;
		int hue = h % hue_sec;
		global_paint = hsv_to_rgb(hue, sat, 1.0f);
	}
	void OnMouseClick() override {
		auto [mx, my] = viewport_mouse;
		if (abs2(viewport_mouse - position) > radius * radius) return;
		cursor_pos = Point2d(mx, my);
		auto [r,theta] = polar_to_cartesian((cursor_pos - position));
		float sat = r / radius;
		int h = (theta / pi / 2.0f) * hue_sec;
		int hue = h % hue_sec;
		global_paint = hsv_to_rgb(hue, sat,1.0f);
	}
	void Update(int elapsed) override {

		//Draw color wheel
		auto [x, y] = (position);
		Point2d pos(x, y);

		for (int sat = 1; sat <= sat_sec; ++sat) {
			glBegin(GL_TRIANGLE_FAN);
			glColor3f(TC(hsv_to_rgb(0.0f, 0.0f, value)));
			glVertex2f(TP(pos));
			float saturation = 1;
			float R = radius * saturation;
			for (int h = 0; h <= hue_sec; ++h) {
				int hue = h % hue_sec;
				float rad = h / (float)hue_sec * pi * 2.0f;

				glColor3f(TC(hsv_to_rgb(hue, saturation, value)));
				glVertex2f(TP((radvec(rad) * R + pos)));
			}
			glEnd();
		}


		//animated color selection
		if (enable_color_animation) {
			int elap = elapsed % 501;
			float rad = (elap / 500.0) * 2 * pi;
			PickColor({ 0.5f,rad });
		}

		// Draw selection cross
		auto [r, g, b, a] = global_paint;
		glColor3f(1.0 - r, 1.0 - g, 1.0 - b);
		glLineWidth(4.0f);
		glBegin(GL_LINES);
		for (auto c : ords) glVertex2f(TP((c * 10 + cursor_pos)));
		glEnd();

		float radii = radius * 2.0f;
		glColor3f(1.0f,1.0f,1.0f);
		auto [px, py] = position;
		px += radius;
		py -= radius;
		glRasterPos2d(TP(Point2d(px, py)));
		std::stringstream ss;
		ss << std::fixed << std::setprecision(2);
		ss << "rgb("<< r << "," << g  << "," << b << ")";
		std::string display_text = ss.str();
		for(auto c:display_text)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}
};