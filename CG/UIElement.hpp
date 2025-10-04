#include "Animated.hpp"

//Drawing UI in real time
class UIElement : public Animated {
public:
	Point2i position;
	int width, height;
	virtual void OnMouseEnter() {};
	virtual void OnMouseClick() {};
	virtual void OnKeyboard() {};
};
