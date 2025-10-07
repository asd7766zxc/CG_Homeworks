#pragma once
#include "DrawingUtility.hpp"
#include <GL/freeglut.h>
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
	float scale = 1.0f;
	int size = 0;
	bool showing_scale = false;
	std::string name;
	bool first_read = true;
	int pos[2];
	int canva_size[2];
	float angle = 0;
	Image(int w, int h,std::string _name) 
		: _width(w), _height(h), name(_name) {
		memset(buff, 0, sizeof(buff));
		memset(offseted_buff, 0, sizeof(offseted_buff));
		size = w * h * 4;
	}
	Image(std::string _name) 
	: name(_name){
		read_from_file();
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

	BYTE scaled_buff[MAX_WINDOW_WIDTH * MAX_WINDOW_HEIGHT][4];
	BYTE rotated_buff[MAX_WINDOW_WIDTH * MAX_WINDOW_HEIGHT][4];
	// 照原比例 放大 所以 每個像素會有 scalex * scaley 的空間
	// 全部填左上角那個像素的顏色 :>
	// 駁回
	// 我直接把像素等比放大 (向上取整)
	// 然後直接塗下去 (一定要往右下塗 這樣才能被蓋住)
	// 牛逼
	// 放大的圖醜死了qwqqq....
	void LoadPixelFromBuffer(Point2d pos) {
		if (canva_brush != z_index) return;
		int w = _width, h = _height;
		int _w = w, _h = h;
		if (scale >= 1) {
			int ww = w * scale;
			int hh = h * scale;
			int dw = ceil(scale);
			int dh = ceil(scale);
			//先直接放大像素....
			for(int i = 0; i < h; ++i)
				for (int j = 0; j < w; ++j) {
					int ind = i * w + j;
					for (int dx = 0; dx < dw; ++dx) {
						for (int dy = 0; dy < dh; ++dy) {
							int xx = dx + i * scale;
							int yy = dy + j * scale;
							if (!(0 <= xx && xx < hh && 0 <= yy && yy < ww)) continue;
							int ins = xx * ww + yy;
							for (int k = 0; k < 4; ++k) {
								scaled_buff[ins][k] = buff[ind][k];
							}
						}
					}
				}
			_w = ww;
			_h = hh;
		}
		else {
			// 縮小沒差 但其實能考慮 blend 上去 嗎?
			int ww = w * scale;
			int hh = h * scale;
			for (int i = 0; i < h; ++i)
				for (int j = 0; j < w; ++j) {
					int ind = i * w + j;
					int xx = i * scale;
					int yy = j * scale;
					int ins = xx * ww + yy;
					for (int k = 0; k < 4; ++k) {
						scaled_buff[ins][k] = buff[ind][k];
					}
				}
			_w = ww;
			_h = hh;
		}

		h = _h;
		w = _w;

		int cenx = _h / 2;
		int ceny = _w / 2;

		//要把像素直接當四個點，旋轉後點在的格子都要著色
		for (int i = 0; i < h; ++i) 
			for (int j = 0; j < w; ++j) {
				int ind = i * w + j;
				rotated_buff[ind][0] = rotated_buff[ind][1] = rotated_buff[ind][2] = rotated_buff[ind][3] = 0;
				// 先清乾淨
			}
		for (int i = 0; i < h; ++i) {
			for (int j = 0; j < w; ++j) {
				int ind = i * w + j;
				int yy = j - ceny;
				int xx = i - cenx;
				auto ret = rotate(Point2d(xx, yy), angle);
				xx = round(ret.x);
				yy = round(ret.y);
				yy += ceny;
				xx += cenx;
				for (int dx = 0; dx <= 1; ++dx) 
					for (int dy = 0; dy <= 1; ++dy) {
						int xxx = xx + dx;
						int yyy = yy + dy;
						if (0 <= xxx && xxx < w && 0 <= yyy && yyy < h) {
							int ins = xxx * w + yyy;
							for (int k = 0; k < 4; ++k) {
								rotated_buff[ins][k] = scaled_buff[ind][k];
							}
						}
					}
			}
		}

		pos.x -= w / 2;
		pos.y -= h / 2;

		auto [x, y] = Point2i(TP(transform_to_viewport(pos)));
		h = std::max(0, std::min(h + y, h));
		w = std::max(0, std::min(w + x, w));
		if (x < 0) x = 0;
		if (y < 0) y = 0;
		for (int i = _h - h; i < _h; ++i)
			for (int j = _w - w; j < _w; ++j) {
				int index = i * _w + j;
				int indics = (i - (_h - h)) * w + (j - (_w - w));
				for (int g = 0; g < 4; ++g) offseted_buff[indics][g] = rotated_buff[index][g];
				offseted_buff[indics][3] = (BYTE)(global_opacity * offseted_buff[indics][3]);
			}
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glRasterPos2i(x, y);
		glDrawPixels(w, h, GL_RGBA, GL_UNSIGNED_BYTE, offseted_buff);
		glDisable(GL_BLEND);
		glFlush();

	}

	// play resize animation
	void Update(int elapsed) override {
		float prog = 1 - (elapsed / (float)timetolive);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		float offset = 4.0f;
		float wid = 2.0f;
		auto [x, y] = projected_mouse;
		float w = _width * scale;
		float h = _height * scale;
		x -= w / 2;
		y -= h / 2;
		std::string tmp = "";
		tmp += '(';
		tmp += std::to_string((int)w);
		tmp += ", ";
		tmp += std::to_string((int)h);
		tmp += ')';
		tmp += ' ';
		float converted_angle = (angle / pi) * 180;
		converted_angle = round(converted_angle * 100);
		int o = ((int)(converted_angle)) / 100;
		tmp += std::to_string(o);
		tmp += '.';
		if (((int)(abs(converted_angle))) % 100 < 10) tmp += '0';
		tmp += std::to_string(((int)(abs(converted_angle))) %100);
		Color color_now = teal;
		color_now.a = prog;
		Text* txt = new Text(Point2d(x - offset, y - 18 - offset), tmp, false, GLUT_BITMAP_HELVETICA_18, color_now, true);
		txt->Update(0);

		Line* line0 = new Line(Point2d(x - offset - wid / 2.0f, y - offset), Point2d(x + w + offset + wid / 2.0f, y - offset), wid, color_now);
		Line* line1 = new Line(Point2d(x + w + offset, y - offset), Point2d(x + w + offset, y + h + offset), wid, color_now);
		Line* line2 = new Line(Point2d(x - offset, y - offset), Point2d(x - offset, y + h + offset), wid, color_now);
		Line* line3 = new Line(Point2d(x - offset - wid / 2.0f, y + h + offset), Point2d(x + w + offset + wid / 2.0f, y + h + offset), wid, color_now);
		line0->Update(0), line1->Update(0), line2->Update(0), line3->Update(0);
		glDisable(GL_BLEND);
		glFlush();
	}
	void save_file() {
		FILE* file = fopen(name.c_str(), "w");
		std::string str = name;
		str += "pos";
		FILE* file_pos = fopen(str.c_str(), "w");
		str += "size";
		FILE* file_size = fopen(str.c_str(), "w");
		fwrite(buff, sizeof(BYTE), size, file);
		fclose(file);

		pos[0] = position.x;
		pos[1] = position.y;
		fwrite(pos, sizeof(int), 2, file_pos);
		fclose(file_pos);

		canva_size[0] = _width;
		canva_size[1] = _height;
		fwrite(canva_size, sizeof(int), 2, file_size);
		fclose(file_size);
	}
	void read_from_file() {
		FILE* file = fopen(name.c_str(), "r");
		std::string str = name;
		str += "pos";
		FILE* file_pos = fopen(str.c_str(), "r");
		str += "size";
		FILE* file_size = fopen(str.c_str(), "r");

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
		if (file_size) {
			fread(canva_size, sizeof(int), 2, file_size);
			fclose(file_size);
			_width = canva_size[0];
			_height = canva_size[1];
			size = _width * _height * 4;
		}
	}
	void DrawIndicator() {
		teal.a = (z_index == current_canva) ? 1 : 0.5;
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		float offset = 4.0f;
		float wid = 2.0f;
		auto [x, y] = position;
		std::string tmp = name;
		tmp += ' ';
		tmp += '(';
		tmp += std::to_string((int)x);
		tmp += ',';
		tmp += std::to_string((int)y);
		tmp += ')';
		Color color_now = teal;
		if (canva_brush == z_index) {
			color_now = global_paint;
			color_now.a = 0.8;
		}
		Text* txt = new Text(Point2d(x - offset, y - 18 - offset), tmp,false,GLUT_BITMAP_HELVETICA_18, color_now,true);
		txt->Update(0);

		
		Line* line0 = new Line(Point2d(x - offset - wid / 2.0f, y - offset), Point2d(x + _width + offset + wid / 2.0f, y - offset), wid, color_now);
		Line* line1 = new Line(Point2d(x + _width + offset, y - offset), Point2d(x + _width + offset, y + _height + offset), wid, color_now);
		Line* line2 = new Line(Point2d(x - offset, y - offset), Point2d(x - offset, y + _height + offset), wid, color_now);
		Line* line3 = new Line(Point2d(x - offset - wid / 2.0f, y + _height + offset), Point2d(x + _width + offset + wid / 2.0f, y + _height + offset), wid, color_now);
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