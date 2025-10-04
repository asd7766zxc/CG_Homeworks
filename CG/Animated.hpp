#pragma once
#include "DrawingUtility.hpp"
//Control by constant speed main timer
class Animated {
public:
	int birth;
	int z_index; // to reArrange drawing order
	int timetolive; // -1 for forever 
	bool markdead = false;
	virtual void Update(int elapsed) {}
};
