#pragma once
#include "DrawObject.hpp"
#include <GL/freeglut.h>
#include <algorithm>

class SimplePolygon : public DrawObject {
public:
	std::vector<Point2i> vertices;
	std::vector<Color> colors;
	std::vector<int> indics;
    std::vector<std::vector<int>> triangles;
	int N = 0;
    std::vector<int> now;
    int prev(int a) {
        int n = now.size();
        int i = 0;
        for (; i < n; ++i) {
            if (now[i] == a) break;
        }
        i = ((i - 1) + n) % n;
        return now[i];
    }
    int nxt(int a) {
        int n = now.size();
        int i = 0;
        for (; i < n; ++i) {
            if (now[i] == a) break;
        }
        i = ((i + 1)) % n;
        return now[i];
    }
    // O(n^2) 三角化算法, 簡單多邊形一定有耳朵
    //耳朵 O(n)
    bool is_ear(int ind) {
        int prv = prev(ind);
        int nx = nxt(ind);
        auto vec1 = vertices[ind] - vertices[prv];
        auto vec2 = vertices[nx] - vertices[prv];
        if ((vec1 ^ vec2) < 0) return false; //凹
        int area = (vec1 ^ vec2);
        std::vector<Point2i> pt = { vertices[prv],vertices[ind],vertices[nx] };
        for (int c : now) {
            if (c == ind || c == prv || c == nx) continue;
            auto cur = vertices[c];
            int sum = 0;
            for (int i = 0; i < 3; ++i) {
                auto a = pt[i] - cur;
                auto b = pt[(i + 1) % 3] - cur;
                sum += abs((a ^ b));
            }
            if (sum == area) {
                return false;
            }
        }
        return true;
    }
	//不知道使用者畫的點順序 
    SimplePolygon(std::vector<Point2i>& pts, std::vector<Color> _colors = {})
		: vertices(pts),colors(_colors), N(pts.size()) {
		indics = std::vector<int>(N);
        if (N < 3) return;
		for (int i = 0; i < N; ++i) indics[i] = i;
        //用面積正負確定順序
        int poly_area = 0;
        for (int i = 0; i < N; ++i) {
            poly_area += (pts[i] ^ pts[(i + 1) % N]);
        }
        if (poly_area < 0) {
           std::reverse(vertices.begin(), vertices.end()); //逆時針給點
        }

        //Triangulation O(n^3)
        now = indics;
        while (now.size() > 3) {
            int f = -1;
            for (auto c : now) {
                if (is_ear(c)) {
                    f = c;
                    break;
                }
            }
            if (f < 0) break;
            std::vector<int> tmp;
            for (auto c : now) if (c != f) tmp.push_back(c);
            int lef = prev(f);
            int rgh = nxt(f);
            triangles.push_back({lef,f,rgh});
            now = tmp;
        }
        triangles.push_back(now);
	}
    void Update(int elapsed) override {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        for (auto tri : triangles) {
            glBegin(GL_POLYGON);
            for (auto c : tri) {
                if (colors.size()) glColor4f(T4C(colors[c]));
                else glColor4f(T4C(global_paint));
                glVertex2f(toView(vertices[c]));
            }
            glEnd();
        }
        glDisable(GL_BLEND);
    }
};