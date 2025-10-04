#pragma once
#include "DrawingUtility.hpp"
//Control by constant speed main timer
class Animated {
public:
	int birth = 0;
	int z_index = 0; // to reArrange drawing order
	int timetolive = 1000000; // -1 for forever 
	bool markdead = false;
	virtual void Update(int elapsed) {}
	virtual void Clear(int elapsed) {}
};
