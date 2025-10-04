#pragma once
#include "Animated.hpp"
#include <math.h>
#include <GL/freeglut.h>
#include <functional>

class Particle : public Animated {
public:
	std::function<Data(int,Particle * )> motion;
	Data data;
	Particle(std::function<Data(int,Particle * )> mo,Data initial = {}) {
		motion = mo;
		data = initial;
	}

	void Update(int elapsed) override {
		auto data = motion(elapsed,this);
		glPointSize(data.size);
		auto [r, g, b, a] = data.color;
		glColor3f(r,g,b);
		glBegin(GL_POINTS);     /*  Draw a point */
		auto [x, y] = data.pt;
		glVertex3f(x + world_x, y + world_y, 0);
		glEnd();
	}
};
