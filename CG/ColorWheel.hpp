#pragma once
#include "UIElement.hpp"
#include "Text.hpp"
#include <GL/freeglut.h>


class ColorWheel : public UIElement {
public:
	Point2d position;
	Point2d cursor_pos;
	
	int sat_sec = 1;
	int hue_sec = 360;
	float value = 1.0f;
	float radius = 100.0f;
	int spin_time = 500;
	bool enable_color_animation = false;
	clock_t last;

	ColorWheel(int _x, int _y) : position(_x, _y) {
		cursor_pos = position;
		global_paint = Color{ 1,1,1,1 };
		last = clock();
	};
	void PickColor(Point2d color) {
		auto [r, theta] = color;
		cursor_pos = Point2d(radius * r * cos(theta) + position.x,radius * r * sin(theta) + position.y);
		float sat = r;
		int h = (theta / pi / 2.0f) * hue_sec;
		int hue = h % hue_sec;
		global_paint = hsv_to_rgb(hue, sat, 1.0f);
		global_paint.a = global_opacity;
	}
	bool OnMouseClick() override {
		auto [mx, my] = viewport_mouse;
		if (abs2(viewport_mouse - position) > radius * radius) return false;
		cursor_pos = Point2d(mx, my);
		auto [r,theta] = polar_to_cartesian((cursor_pos - position));
		float sat = r / radius;
		int h = (theta / pi / 2.0f) * hue_sec;
		int hue = h % hue_sec;
		global_paint = hsv_to_rgb(hue, sat,1.0f);
		global_paint.a = global_opacity;
		return true;
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
			int elap = elapsed % (spin_time + 1);
			float rad = (elap / (float)spin_time) * 2 * pi;
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
		auto [px, py] = position;
		px += radius;
		py -= radius;
		std::stringstream ss;
		ss << std::fixed << std::setprecision(2);
		ss << "rgba("<< r << "," << g  << "," << b << "," << global_opacity << ")";

		Text* txt_color = new Text(Point2d(px, py), ss.str(), false, GLUT_BITMAP_HELVETICA_18, Color{ 1,1,1,0.5 }, false);
		txt_color->Update(0);
		clock_t now = clock();
		std::stringstream s1;
		s1 << std::fixed << std::setprecision(2);
		s1 << 1.0 / ((double)(now - last) / CLOCKS_PER_SEC) << " fps";
		last = now;
		Text* txt_fps = new Text(Point2d(px, py + 18), s1.str(), false, GLUT_BITMAP_HELVETICA_18, Color{ 1,1,1,0.5 }, false);
		txt_fps->Update(0);

		std::stringstream s2;
		if (erasering) s2 << "[Eraser]";
		if (erasering && typing) s2 << " ";
		if (typing) s2 << "[Typing]";
		if (pen_tool) s2 << " " << "[Pen Tool]";
		if (canva_brush >= 0) s2 << " " << "[Drawing Canva" << canva_brush <<"]";
		if (rotating_canva) s2 << " " << "[Rotating Canva]";
		if (pen_tool) {
				s2 << " [Fill Mode: ";
			switch (polygon_mode) {
			case 1: {
				s2 << "Smooth Fill]";
				break;
			};
			case 2: {
				s2 << "No Fill]";
				break;
			};
			case 3: {
				s2 << "Smooth]";
				break;
			};
			case 4: {
				s2 << "Fill]";
				break;
			}
			}
		}
		Text* txt_state = new Text(Point2d(px, py + 36), s2.str(), false, GLUT_BITMAP_HELVETICA_18, Color{ 1,1,1,0.5 }, false);
		txt_state->Update(0);
	}
};
