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

	ColorWheel(int _x, int _y) : position(_x, _y) {
		cursor_pos = position;
		global_paint = Color{ 1,1,1,1 };
	};
	void OnMouseClick() override {
		auto [mx, my] = projected_mouse;
		if (abs2(projected_mouse - position) > radius * radius) return;
		cursor_pos = Point2d(mx, my);
		auto [r,theta] = polar_to_cartesian((cursor_pos - position));
		float sat = r / radius;
		int h = (theta / pi / 2.0f) * hue_sec;
		int hue = h % hue_sec;
		global_paint = hsv_to_rgb(hue, sat,1.0f);
	}
	void Update(int elapsed) override {

		auto [x, y] = transform_to_viewport(position);
		
		// Draw selection
		auto [xx, yy] = transform_to_viewport(cursor_pos);

		for (int sat = 1; sat <= sat_sec; ++sat) {
			glBegin(GL_TRIANGLE_FAN);
			auto [r, g, b, a] = hsv_to_rgb(0.0f, 0.0f, value);
			glColor3f(r, g, b);
			glVertex2f(x, y);
			float saturation = 1;
			float R = radius * saturation;
			for (int h = 0; h <= hue_sec; ++h) {
				int hue = h % hue_sec;
				float rad = h / (float)hue_sec * pi * 2.0f;

				auto [r, g, b, a] = hsv_to_rgb(hue, saturation, value);
				glColor3f(r, g, b);
				glVertex2f(R * cos(rad) + x, R * sin(rad) + y);
			}
			glEnd();
		}

		auto [r, g, b, a] = global_paint;
		glColor3f(1.0 - r, 1.0 - g, 1.0 - b);
		glLineWidth(4.0f * view_scale);
		glBegin(GL_LINES);
		Point2i cursor(xx, yy);
		for (auto c : ords) {
			auto [x, y] = (c * 10 + cursor);
			glVertex2f(x, y);
		}
		glEnd();
	}
};