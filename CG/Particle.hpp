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
		glColor3f(TC(Color(0,0,0)));
		glBegin(GL_POINTS);     /*  Draw a point */
		glVertex3f(toView(data.pt), 0);
		glEnd();

		glPointSize(data.size);
		glColor3f(TC(data.color));
		glBegin(GL_POINTS);     /*  Draw a point */
		glVertex3f(toView(data.pt), 0);
		glEnd();
	}
};
