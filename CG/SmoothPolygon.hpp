#pragma once
#include "RoundedLine.hpp"
#include <GL/freeglut.h>
#include "SimplePolygon.hpp"

class SmoothPolygon : public DrawObject {
public:
	int sample_count = 1000;
	std::vector<Point2d> sample;
	std::vector<Color> colors;
	SimplePolygon * _polygon;
	int n = 0;
	float width = 1.0f;
	bool fill = true;
	SmoothPolygon(std::vector<Point2d>& _samples, float wd = 10.0f, std::vector<Color> _colors = {},bool _fill = true, int _sample_count = 1000)
		: sample(_samples), sample_count(_sample_count), width(wd), colors(_colors),fill(_fill) {
		n = _samples.size();
	}

	Point2d bezier_curve(float t) {
		int cn = n;
		std::vector<Point2d> cur = sample;
		while (cur.size() > 1) {
			std::vector<Point2d> tmp;
			for (int i = 1; i < cur.size(); ++i) {
				tmp.push_back(cur[i - 1] * t + cur[i] * (1 - t));
			}
			cur = tmp;
		}
		return cur[0];
	}
	void Update(int elapsed) override {
		std::vector<Point2i> _poly;
		std::vector < Color > _color;
		for (int i = 0; i < sample_count; ++i) {

			float partial = i / (float)sample_count;
			int color_index = (int)(partial * n);

			Color cur = global_paint;
			if (colors.size()) {
				cur = colors[color_index];
				if (color_index + 1 < n) {
					float par = partial * n - color_index;
					cur = blend(cur, colors[color_index + 1], par);
				}
			}
			_poly.push_back(Point2i(TP(bezier_curve(partial))));
			_color.push_back(cur);
		}
		_polygon = new SimplePolygon(_poly, _color);
		if(fill) _polygon->Update(0);
		float x_0 = sample[0].x;
		float x_n = sample[n - 1].x;;
		for (int i = 1; i < sample_count; ++i) {

			float prv_partial = (i - 1) / (float)sample_count;
			float partial = i / (float)sample_count;
			int color_index = (int)(partial * n);
			float xi = (x_n - x_0) * partial + x_0;
			float xpi = (x_n - x_0) * prv_partial + x_0;

			// 平均相鄰點顏色
			Color cur = global_paint;
			if (colors.size()) {
				cur = colors[color_index];
				if (color_index + 1 < n) {
					float par = partial * n - color_index;
					cur = blend(cur, colors[color_index + 1], par);
				}
			}
			//auto rl = new RoundedLine(Point2d(xpi,lagrange(xpi)),Point2d(xi,lagrange(xi)), width, cur);
			auto rl = new RoundedLine(bezier_curve(prv_partial), bezier_curve(partial), width, cur);
			rl->Update(0);
		}
	}
};
