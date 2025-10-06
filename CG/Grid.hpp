#include <GL/freeglut.h>
#include "DrawObject.hpp"
#include "Line.hpp"
#include "Text.hpp"
#include <format>

class Grid : public DrawObject {
public:
	float left = -10000.0f;
	float right = 10000.0f;
	float top = 10000.0f;
	float bottom = -10000.0f;
	std::vector<DrawObject * > objs;
	float bold_step = 250.0f;
	float thin_step = 50.0;
	float bold_width = 2.0f;
	float thin_width = 1.0f;
	float axis_width = 7.0f;
	Grid() {
		for (float x = left; x <= right + 1; x += bold_step) {
			auto line = new Line(Point2d(x, bottom), Point2d(x, top), bold_width, Color{ 1,1,1, 0.5 });
			objs.push_back(line);
		}
		for (float y = bottom; y <= top + 1; y += bold_step) {
			auto line = new Line(Point2d(left, y), Point2d(right, y), bold_width, Color{ 1,1,1, 0.5 });
			objs.push_back(line);
		}

		for (float x = left; x <= right; x += thin_step) {
			auto line = new Line(Point2d(x, bottom), Point2d(x, top), thin_width, Color{ 1,1,1,0.1 });
			objs.push_back(line);
		}
		for (float y = bottom; y <= top; y += thin_step) {
			auto line = new Line(Point2d(left, y), Point2d(right, y), thin_width, Color{ 1,1,1,0.1 });
			objs.push_back(line);
		}
		objs.push_back(new Line(Point2d(0, bottom), Point2d(0, top), axis_width, Color{ 1,1,1, 1 }));
		objs.push_back(new Line(Point2d(left, 0), Point2d(right, 0), axis_width, Color{ 1,1,1, 1 }));
		auto txt = new Text(Point2d(10, 10),"0", false, GLUT_BITMAP_HELVETICA_18, Color{1,1,1,0.5}, true);
		objs.push_back(txt);


		for (float x = left; x <= right + 1; x += bold_step) {
			if (sgn(x) == 0) continue;
			auto txt = new Text(Point2d(10 + x, 10), std::format("{}",(int)x), false, GLUT_BITMAP_HELVETICA_18, Color{1,1,1,0.5}, true);
			objs.push_back(txt);
		}
		for (float y = bottom; y <= top + 1; y += bold_step) {
			if (sgn(y) == 0) continue;
			auto txt = new Text(Point2d(10, 10 + y), std::format("{}", (int)y), false, GLUT_BITMAP_HELVETICA_18, Color{ 1,1,1,0.5 }, true);
			objs.push_back(txt);
		}
	}
	void Update(int elapsed) override {
		for (auto obj : objs) {
			obj->Update(0);
		}
	}
};