#include <GL/freeglut.h>
#include "UIElement.hpp"
#include "DrawObject.hpp"

class Box : public DrawObject {
public:
	int width, height;
	int border_width = 0;
	Color border_color;
	Color color;
	
	void Draw(int x,int y) override {
		x += world_x, y += world_y;


	}
};