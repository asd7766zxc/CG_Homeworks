#pragma once
#include "DrawingUtility.hpp"
#include <algorithm>
#include "Image.hpp"

Point2d radvec(float rad) {
    return Point2d(cos(rad), sin(rad));
}

Point2d viewport_to_world(Point2i point) {
    auto ret = Point2d(point.x - world_x, point.y - world_y);
    return ret;
};
Point2d polar_to_cartesian(Point2d point) {
    auto [x, y] = point;
    float R = abs(point);
    float rad = atan2(y, x);
    if (rad < 0) rad = 2 * pi + rad;
    return Point2d(R, rad);
}
Point2d rotate(Point2d point, float rad) {
    auto [x, y] = point;
    float c = cos(rad);
    float s = sin(rad);
    return Point2d(c * x - s * y, s * x + c * y);
}
std::vector<Point2d> convex_hull(std::vector<Point2d> P) {
    std::sort(P.begin(), P.end());
    P.erase(unique(P.begin(), P.end()), P.end());
    P.insert(P.end(), P.rbegin() + 1, P.rend());
    std::vector<Point2d> stk;
    for (auto p : P) {
        auto it = stk.rbegin();
        while (stk.rend() - it >= 2 and \
            ori(*std::next(it), *it, p) <= 0 and \
            (*next(it) < *it) == (*it < p)) {
            it++;
        }
        stk.resize(stk.rend() - it);
        stk.push_back(p);
    }
    stk.pop_back();
    return stk;
}


//O(nlog(n)) 但找到的圓不一定是最理想的 :[ //堪用..
Point2d minimum_enclosing_circle(std::vector<Point2d>& points) {
    float minv = 1e30;
    float maxv = -1e30;
    auto chky = [&](float x, float y) {
        Point2d pt(x, y);
        float ret = abs(points[0] - pt);
        for (auto c : points) ret = std::max(ret, abs(c - pt));
        return ret;
        };
    auto chk = [&](int x) {
        float l = minv;
        float r = maxv;
        while (r - l > 1000 * eps) {
            float ml = (r - l) / 3.0 + l;
            float mr = (r - l) * 2.0 / 3.0 + l;
            if (chky(x, ml) > chky(x, mr)) l = ml;
            else r = mr;
        }
        return Point2d{ l,chky(x,l) };
        };
    for (auto [x, y] : points) {
        minv = std::min({ x,y,minv });
        maxv = std::max({ x,y,maxv });
    }
    float l = minv;
    float r = maxv;
    while (r - l > 1000 * eps) {
        float ml = (r - l) / 3.0 + l;
        float mr = (r - l) * 2.0 / 3.0 + l;
        if (chk(ml).y > chk(mr).y) l = ml;
        else r = mr;
    }
    return Point2d(l, chk(l).x);
}

Color hsv_to_rgb(int h, float s, float v) {
    int hi = h / 60;
    float f, p, q, t;
    f = h / 60.0 - hi;
    p = v * (1 - s);
    q = v * (1 - f * s);
    t = v * (1 - (1 - f) * s);
    switch (hi) {
    case 0:
        return Color(v, t, p);
    case 1:
        return Color(q, v, p);
    case 2:
        return Color(p, v, t);
    case 3:
        return Color(p, q, v);
    case 4:
        return Color(t, p, v);
    case 5:
        return Color(v, p, q);
    }
    return Color{};
}

int sgn(float x) { return (x > -eps) - (x < eps); } // dcmp == sgn
bool erasering = false;
bool typing = false;
bool pen_tool = false;
int current_canva = -1;