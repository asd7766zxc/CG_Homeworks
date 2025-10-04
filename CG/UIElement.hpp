#include "Animated.hpp"

//Drawing UI in real time
class UIElement : public Animated {
public:
	Point2i position;
	int width, height;
	virtual void OnMouseDown(int btn) {};
	virtual void OnMouseUp(int btn) {};
	virtual void OnMouseMove(bool down) {};
	virtual void OnMouseEnter() {};
	virtual void OnMouseClick() {};
	virtual void OnKeyboard() {};
};
