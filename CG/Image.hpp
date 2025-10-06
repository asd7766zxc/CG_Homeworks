#pragma once
#include "DrawingUtility.hpp"
#include <GL/freeglut.h>
#include <string>
#include "Text.hpp"
#include "Line.hpp"
#include "UIElement.hpp"

#define MAX_WINDOW_WIDTH 1920
#define MAX_WINDOW_HEIGHT 1080

class Image : public UIElement {
public:
	BYTE buff[MAX_WINDOW_WIDTH * MAX_WINDOW_HEIGHT][4];
	BYTE offseted_buff[MAX_WINDOW_WIDTH * MAX_WINDOW_HEIGHT][4];
	std::vector<BYTE**>  drawing_trace;
	int current_indx = -1;
	// rgba
	int _width, _height;
	Point2d position;
	Color teal = { 0,0.5859375,0.53125,0.8 };
	Color _color;
	bool dragging = false;
	bool first_draw = false;
	int size = 0;
	const char* name;
	bool first_read = true;
	int pos[2];
	Image(int w, int h,const char * _name) 
		: _width(w), _height(h), name(_name) {
		memset(buff, 0, sizeof(buff));
		memset(offseted_buff, 0, sizeof(offseted_buff));
		size = w * h * 4;
	}
	void initial(Color color) {
		_color = color;
	}

	void ReadPixelToBuffer() {
		if (first_read) {
			read_from_file();
			first_read = false;
			return;
		}
		auto [x, y] = Point2i(TP(transform_to_viewport(position)));
		int w = _width, h = _height;

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
				if (buff[index][0] == 0 && buff[index][1] == 0 && buff[index][2] == 0) buff[index][3] = 0;
			}
	}
	void LoadPixelFromBuffer() {
		auto [x, y] = Point2i(TP(transform_to_viewport(position)));
		int w = _width, h = _height;

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
	void save_file() {
		FILE* file = fopen(name, "w");
		std::string str = name;
		str += "pos";
		FILE* file_pos = fopen(str.c_str(), "w");
		fwrite(buff, sizeof(BYTE), size, file);
		fclose(file);
		pos[0] = position.x;
		pos[1] = position.y;
		fwrite(pos, sizeof(int), 2, file_pos);
		fclose(file_pos);
	}
	void read_from_file() {
		FILE* file = fopen(name, "r");
		std::string str = name;
		str += "pos";
		FILE* file_pos = fopen(str.c_str(), "r");
		if (file) {
			fread(buff, sizeof(BYTE), size, file);
			fclose(file);
		}
		if (file_pos) {
			fread(pos, sizeof(int), 2, file_pos);
			fclose(file_pos);
			position.x = pos[0];
			position.y = pos[1];
		}
	}
	void DrawIndicator() {
		teal.a = (z_index == current_canva) ? 1 : 0.5;
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		float offset = 4.0f;
		float wid = 2.0f;
		auto [x, y] = position;
		Text* txt = new Text(Point2d(x - offset, y - 18 - offset),name,false,GLUT_BITMAP_HELVETICA_18, teal,true);
		txt->Update(0);
		
		Line* line0 = new Line(Point2d(x - offset - wid / 2.0f, y - offset), Point2d(x + _width + offset + wid / 2.0f, y - offset), wid,teal);
		Line* line1 = new Line(Point2d(x + _width + offset, y - offset), Point2d(x + _width + offset, y + _height + offset), wid, teal);
		Line* line2 = new Line(Point2d(x - offset, y - offset), Point2d(x - offset, y + _height + offset), wid, teal);
		Line* line3 = new Line(Point2d(x - offset - wid / 2.0f, y + _height + offset), Point2d(x + _width + offset + wid / 2.0f, y + _height + offset), wid, teal);
		line0->Update(0), line1->Update(0), line2->Update(0), line3->Update(0);
		glDisable(GL_BLEND);
		glFlush();
	}
	void* new_2d_array(int h, int w, int size){
		void** p;
		p = (void**)new char[h * sizeof(void*) + h * w * size];
		for (int i = 0; i < h; i++) p[i] = ((char*)(p + h)) + i * w * size;
		return p;
	}
	#define NEW2DARR(H, W, TYPE) (TYPE **)new_2d_array(H, W, sizeof(TYPE))
	void backup() {
		int H = _width * _height;
		BYTE** tmp = NEW2DARR(H, 4, BYTE);
		for (int i = 0; i < H; ++i) {
			for(int g = 0; g < 4;++g)
			tmp[i][g] = buff[i][g];
		}
		current_indx++;
		if(current_indx >= drawing_trace.size()) drawing_trace.push_back(tmp);
		else drawing_trace[current_indx] = tmp;
	}
	void retrive() {
		int H = _width * _height;
		current_indx = std::max(current_indx-1, 0);
		if (drawing_trace.size()) {
			for (int i = 0; i < H; ++i) {
				for (int g = 0; g < 4; ++g)
					buff[i][g] = drawing_trace[current_indx][i][g];
			}
		}
	}

	//WIP
	void next() {
		current_indx = std::min({ drawing_trace.size() - 1, (size_t)current_indx + 1 });

		//memcpy(buff, next_buff, size);
	}
	bool mouse_inside() {
		Point2d offseted_mouse = projected_mouse - position;
		auto [dx, dy] = offseted_mouse;
		return (0 <= dx && dx < _width && 0 <= dy && dy < _height);
	}
};