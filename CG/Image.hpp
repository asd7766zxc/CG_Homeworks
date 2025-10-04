#pragma once
#include "DrawingUtility.hpp"
#include <GL/freeglut.h>

#define MAX_WINDOW_WIDTH 1920
#define MAX_WINDOW_HEIGHT 1080

class Image {
public:
	BYTE buff[MAX_WINDOW_WIDTH * MAX_WINDOW_HEIGHT][4];
	BYTE offseted_buff[MAX_WINDOW_WIDTH * MAX_WINDOW_HEIGHT][4];
	int _width, _height;
	Image(int w, int h) : _width(w), _height(h) {
		memset(buff, 0, sizeof(buff));
		memset(offseted_buff, 0, sizeof(offseted_buff));
	}
	void initial(Color color) {
		/*auto [r, g, b, a] = color;
		int tmp[4] = { r,g,b,a};
		for (int i = 0; i < _height; ++i) {
			for (int j = 0; j < _width; ++j) {
				int index = i * _width + j;
				for (int g = 0; g < 4; ++g) buff[index][g] = 0xff;
				for (int g = 0; g < 4; ++g) offseted_buff[index][g] = 0xff;
			}
		*/
	}
	void ReadPixelToBuffer(int x = 0,int y = 0,int w = -1,int h = -1) {
		if (w < 0) w = _width;
		if (h < 0) h = _height;


		h = std::max(0, std::min(h + y, h));
		w = std::max(0, std::min(w + x, w));
		if (x < 0) x = 0;
		if (y < 0) y = 0;

		glReadPixels(x,y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, offseted_buff);
		for(int i =  _height - h; i < _height;++i)
			for (int j = _width - w; j <  _width; ++j){
				int index = i * _width + j;
				int indics = (i - (_height - h)) * w + (j - (_width - w));
				for (int g = 0; g < 4; ++g) buff[index][g] = offseted_buff[indics][g];
				if (buff[index][0] == 0 && buff[index][1] == 0 && buff[index][2] == 0) buff[index][3] = 127;

			}
	}
	void LoadPixelFromBuffer(int x = 0,int y = 0,int w = -1,int h = -1) {
		if (w < 0) w = _width;
		if (h < 0) h = _height;

		h = std::max(0, std::min(h + y, h));
		w = std::max(0, std::min(w + x, w));
		if (x < 0) x = 0;
		if (y < 0) y = 0;
		for (int i = _height - h; i < _height; ++i)
			for (int j = _width - w; j < _width; ++j) {
				int index = i * _width + j;
				int indics = (i - (_height - h)) * w + (j - (_width - w));
				for (int g = 0; g < 4; ++g) offseted_buff[indics][g] = buff[index][g];
			}
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glRasterPos2i(x,y);
		glDrawPixels(w, h, GL_RGBA, GL_UNSIGNED_BYTE, offseted_buff);
		glDisable(GL_BLEND);
		glFlush();
	}
};