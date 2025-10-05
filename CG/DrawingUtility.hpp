#pragma once
#include <math.h>
#include<numbers>
#include <vector>
#define TP(X) (X).x,(X).y
#define T4C(X) (X).r,(X).g,(X).b,(X).a
#define TC(X) (X).r,(X).g,(X).b
#define toView(P) TP(transform_to_viewport(P))

template<class T>
struct _Point2d {
	T x, y;
	_Point2d(T _x, T _y) :x(_x), y(_y) {}
	_Point2d() :x(0), y(0) {}

	_Point2d operator * (T  c) { return _Point2d(x * c, y * c); }
	_Point2d operator / (T  c) { return _Point2d(x / c, y / c); }
	_Point2d operator + (_Point2d a) { return _Point2d(x + a.x, y + a.y); }
	_Point2d operator - (_Point2d a) { return _Point2d(x - a.x, y - a.y); }
	T  operator * (_Point2d a) { return x * a.x + y * a.y; }
	T  operator ^ (_Point2d a) { return x * a.y - y * a.x; }

	bool operator < (_Point2d a) const { return x < a.x || (x == a.x && y < a.y); };
	bool operator== (_Point2d a) const { return x == a.x and y == a.y; };

	friend T ori(_Point2d a, _Point2d b, _Point2d c) { return (b - a) ^ (c - a); }
	friend T abs2(_Point2d a) { return a * a; }
	friend T abs(_Point2d a) { return sqrt(abs2(a)); }
	friend _Point2d unified(_Point2d vec) {
		return vec / abs(vec);
	}
};

using Point2d = _Point2d<float>;
using Point2i = _Point2d<int>;
using std::numbers::pi;

const float eps = 1e-6;

struct Color {
	float r, g, b, a;
	Color(float _r, float _g, float _b, float _a = 1.0) :r(_r), g(_g), b(_b), a(_a){}
	Color() :r(0.0), g(0.0), b(0.0), a(1.0) {}
	Color operator * (float  c) { return Color(r*c,g*c,b*c,a*c); }
	Color operator + (Color rst) { return Color(r + rst.r, g + rst.g, b + rst.b, a + rst.a); }
	friend Color blend(Color A, Color B, float p) {
		return A * p + B * (1.0 - p);
	}
};


struct Data {
	Point2d pt;
	float size;
	Color color;
};
struct Pixel {

};

/*L for bottom left corner
R for upper right corner*/
template<class T>
bool point_within(_Point2d<T> L,_Point2d<T> R, _Point2d<T> l,_Point2d<T> r) {
	auto [x0, y0] = L;
	auto [x1, y1] = R;

	auto [x2, y2] = l;
	auto [x3, y3] = r;

	return (x0 <= x2 && y0 <= y2 && x3 <= x1 && y3 <= y1);
}
template<class T>
extern Point2d transform_to_viewport(_Point2d<T> point);
extern Point2d viewport_to_world(Point2i point);
extern Point2d polar_to_cartesian(Point2d point);
extern Point2d radvec(float rad);
extern Point2d rotate(Point2d point, float rad);

extern Color hsv_to_rgb(int h, float s, float v);
extern std::vector<Point2d> convex_hull(std::vector<Point2d> P);

extern std::vector<Point2d> ords;

extern float wcenx, wceny;
extern int height, width;
extern int mouse_x, mouse_y;
extern float world_x, world_y;
extern int mdelta_x, mdelta_y;
extern Color global_paint;
extern float view_scale;
extern Point2d projected_mouse;
extern Point2d viewport_mouse;
extern Point2d projected_omuse;
extern float brush_size;