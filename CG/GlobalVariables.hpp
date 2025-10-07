#pragma once
#include "DrawingUtility.hpp"

int height = 512, width = 512;
float wcenx = height / 2.0, wceny = width / 2.0;
float brush_size = 10.0f;
int mouse_x = 0, mouse_y = 0;
int omouse_x = 0, omouse_y = 0;

int mdelta_x = 0, mdelta_y = 0;
float world_x = wcenx, world_y = wceny;
Color global_paint;
Point2d projected_mouse(0, 0);
Point2d viewport_mouse(0, 0);
Point2d typing_cursor(0, 0);
std::vector<Point2d> ords = { {-1,0},{1,0},{0,-1},{0,1} };
bool erasering = false;
bool typing = false;
bool pen_tool = false;
int current_canva = -1;
int canva_brush = -1;
float global_opacity = 1.0f;
