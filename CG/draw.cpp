/******************************************************************
 * This program illustrates the fundamental instructions for handling
 * mouse and keyboeard events as well as menu buttons.
 */

#include <stdio.h>
#include <algorithm>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <GL/freeglut.h>
#include <random>
#include <utility>
#include <chrono>
#include <queue>
#include <ranges>
#include "Particle.hpp"
#include "ColorWheel.hpp"
#include "Dot.hpp"
#include "Image.hpp"
#include "Line.hpp"
#include "RoundedLine.hpp"
#include "SmoothCurve.hpp"
#include "BrushIndicator.hpp"
#include "Text.hpp"
#include "FlyingText.hpp"
#include "Grid.hpp"
#include "SimplePolygon.hpp"
#include "MouseIndicator.hpp"
#include "SmoothPolygon.hpp"

ColorWheel* color_wheel;
BrushIndicator* brush_indicator = nullptr;
Grid* grid;
MouseIndicator* mouse_indicator = nullptr;
MouseIndicator* mouse_pos_indicator = nullptr;
Point2d projected_omouse(0, 0);
float view_scale = 1.0f;

void* current_font = GLUT_BITMAP_HELVETICA_18;

#pragma region Containers


//Containers 
std::vector<Animated*> animation_list;
std::queue<DrawObject*> drawing_queue;
std::vector<DrawObject*> drawing_cache;
std::vector<DrawObject*> drawing_animations;
std::vector<UIElement*> uielement_list;
std::vector<Point2d> mouse_trace;
std::vector<Color> color_trace;
std::vector<Point2i> polygon_cache;
std::vector<Color> polygon_color;
std::vector<Point2d> canva_draw_cache;

std::vector<int> onClick_trigger_list, onDown_trigger_list, onUp_trigger_list, onMove_trigger_list;
#pragma endregion

//drawing status
bool dragging = false;
bool drawing = false;
bool mark_backup = false;

template<class T>
Point2d transform_to_viewport(_Point2d<T> point) {
    auto ret = Point2d(point.x + world_x, point.y + world_y);
    return ret;
};

std::vector<Image*> Canvas;
std::vector<int> canva_layers;

void clear_screen() {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
}
bool first_draw = true;
bool grid_on = false;
void calculate_canva() {
    clear_screen();
    for (auto canva : Canvas) {
        if (!canva) continue;
        clear_screen();
        auto queue_back = drawing_queue;
        canva->LoadPixelFromBuffer();
        while (queue_back.size()) {
            auto am = queue_back.front(); queue_back.pop();
            am->Update(am->timetolive);
        }
        if (canva_brush >= 0) {
            for (auto pt : canva_draw_cache) {
                Canvas[canva_brush]->LoadPixelFromBuffer(pt);
            }
        }
        if (mark_backup) canva->backup();
        if (!canva->first_draw) {
            canva->ReadPixelToBuffer();
        }
        canva->first_draw = false;
        clear_screen();
    }
    while (drawing_queue.size()) drawing_queue.pop();
    clear_screen();
    mark_backup = false;
    canva_draw_cache.clear();
}
void draw_canva() {
    for (auto i : canva_layers) {
        auto canva = Canvas[i];
        if (canva) canva->LoadPixelFromBuffer();
    } 
}
/*------------------------------------------------------------
 * Callback function for display, redisplay, expose events
 * Just clear the window again
 */
void display_func(void){
    /* define window background color */
    //glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
}

int elapsed = 0;
void play_Animation(Animated* am) {
    int span = elapsed - am->birth;
    if (!(am->timetolive < 0) && (am->timetolive < span)) {
        am->markdead = true;
    }
    if (am->markdead) return;
    am->Update(span);
}

bool show_mouse_pos = false;
void AnimationLoop(int value) {
    calculate_canva();
    ++elapsed;
    if(grid_on) grid->Update(0);
    draw_canva();
    for (auto canva : Canvas) if (canva) canva->DrawIndicator();
    for (auto am : animation_list) {
        play_Animation(am);
    }
    for (auto c : drawing_cache) c->Update(0);
    for (auto da : drawing_animations) {
        play_Animation(da);
    }
    for (auto ui : uielement_list) {
        play_Animation(ui);
    }
    if (canva_brush >= 0) {
        Canvas[canva_brush]->LoadPixelFromBuffer(projected_mouse);
        if(Canvas[canva_brush]->showing_scale)play_Animation(Canvas[canva_brush]);
    } 
    if (brush_indicator) play_Animation(brush_indicator);
    if (mouse_indicator) play_Animation(mouse_indicator);
    if (mouse_pos_indicator && show_mouse_pos) play_Animation(mouse_pos_indicator);
    glutSwapBuffers();
    
    glutTimerFunc(0, AnimationLoop, -1);
}

std::mt19937 mt(std::chrono::steady_clock::now().time_since_epoch().count());
std::uniform_real_distribution<> dis(-1, 1);
std::uniform_int_distribution<> intdis(1000, 2000);
void CleanUpLoop(int value) {
    glutTimerFunc(30, CleanUpLoop, -1);
    int ttl = intdis(mt);
    Point2d direction(dis(mt), dis(mt));
    direction = unified(direction);
    Animated* par = new Particle([&](int elapsed, Particle* self) {
        float progs = ((elapsed) % self->timetolive / (float)self->timetolive);
        float angle = progs * 2 * pi;
        float alpha = 1.0 - progs;
        auto [c, s] = self->data.pt;
        // cx - sy, sx + cy
        auto cen = Point2d(300, 300);
        //Point2d fluctuation(cos(angle) * 20, sin(angle) * 20);
        float x = progs * 400;
        float y = progs * 20 * sin(progs * 10 * pi + s * 10);
        Point2d pos(c * x - s * y, s * x + c * y);
        pos = pos + cen;
        pos.y -= progs * progs * 1000;
        //Point2d pos = (self->data.pt) * 400 * progs + cen;
        return Data{ pos,10,global_paint * alpha };
        }, Data{ direction,10,Color(0,0,0,0) });
    par->timetolive = ttl;
    par->birth = elapsed;
    //animation_list.push_back(par);

    // remove dead animations
    std::vector<Animated*> tmp;
    std::vector<DrawObject*> da_tmp;

    for (auto c : animation_list) {
        if (c->markdead) continue;
        tmp.push_back(c);
    }
    std::sort(tmp.begin(), tmp.end(), [&](Animated* a, Animated* b) {
        return a->z_index < b->z_index;
        });
    for (auto da : drawing_animations) {
        if (da->markdead) drawing_queue.push(da);
        else da_tmp.push_back(da);
    }
    animation_list = tmp;
    drawing_animations = da_tmp;
}

bool adjusting_opacity = false;
bool adjusting_canva_size = false;
bool rotating_canva = false;
/*--------------------------------------------------------------
 * Callback function for keyboard event.
 * key = the key pressed,
 * (x,y)= position in the window, where the key is pressed.
 */
void add_flying_tex(std::string str) {
    Point2d ini = Point2d();
    Point2d direction(dis(mt), dis(mt));
    direction = unified(direction);
    FlyingText* text = new FlyingText(typing_cursor, str, typing_cursor + direction * 200.f, true, current_font);
    text->birth = elapsed;
    text->timetolive = 50;
    drawing_animations.push_back(text);
}
void keyboard(unsigned char key, int x, int y){
    if (typing) {
        std::string tmp = "";
        tmp += key;
        add_flying_tex(tmp);
        return;
    }
    if (tolower(key) == 'q') exit(0);
    if (tolower(key) == 'p') {
        show_mouse_pos = !show_mouse_pos;
    }
    if (tolower(key) == 'r') {
        if (canva_brush >= 0) rotating_canva = !rotating_canva;
    }
    if (tolower(key) == 'b') {
        if (canva_brush < 0) canva_brush = current_canva;
        else canva_brush = -1;
    }
    if (tolower(key) == 'u') {
        color_wheel->enable_color_animation = !color_wheel->enable_color_animation;
    }
    if (tolower(key) == 'o') {
        adjusting_opacity = !adjusting_opacity;
    }
    if (tolower(key) == 's') {
        for (auto canva : Canvas) if (canva) canva->save_file();
    }
    if (tolower(key) == 'g') {
        grid_on = !grid_on;
    }
    if (tolower(key) == 'z') {
        for (auto c : Canvas) c->retrive();
    }
    if (tolower(key) == 'x') {
        for (auto c : Canvas) c->next();
    }
    if (tolower(key) == '=') {
        color_wheel->spin_time = std::max(50, color_wheel->spin_time - 50);
    }
    if (tolower(key) == '-') {
        color_wheel->spin_time = std::min(1000, color_wheel->spin_time + 50);
    }
}

void update_mouse_position(int x, int y) {
    omouse_x = mouse_x;
    omouse_y = mouse_y;
    mouse_x = x;
    mouse_y = height - y;
    mdelta_x = mouse_x - omouse_x;
    mdelta_y = mouse_y - omouse_y;
    projected_mouse = viewport_to_world(Point2i(mouse_x,mouse_y));
    projected_omouse = viewport_to_world(Point2i(omouse_x, omouse_y));
    viewport_mouse = Point2d(mouse_x, mouse_y);
    if (show_mouse_pos) {
        mouse_pos_indicator = new MouseIndicator();
        mouse_pos_indicator->birth = elapsed;
        mouse_pos_indicator->timetolive = -1;
        mouse_pos_indicator->custom_text = true;
        mouse_pos_indicator->offseted = Point2d(10,10);
        mouse_pos_indicator->text = "(" + std::to_string((int)(projected_mouse.x)) + ","+ std::to_string((int)(projected_mouse.y)) + ")";
    }
}


bool first_point = false;
Point2d fpoint;
bool first_pen = true;
Point2i firs_pen_point;
Point2i poly_omouse;
Point2i poly_mouse;
Point2d poly_projected_omouse;
void draw_smooth_trace() {
    if (!first_point) {
        auto cm = new RoundedLine(projected_omouse, projected_mouse, brush_size);
        drawing_cache.push_back(cm);
    }
    else {
        fpoint = projected_mouse;
    }
    first_point = false;
}

//get stalling type (0: for straight line) (1: for curve) (2: for circle)
int get_stalling_type() {
    if (!mouse_trace.size()) return -1;
    auto fpt = mouse_trace[0];
    auto lpt = mouse_trace.back();
    float len = abs(fpt - lpt);
    float mx = 0;
    for (auto p : mouse_trace) {
        float dis = (abs((p - lpt) ^ (fpt - lpt))) / len; //以最遠的直線距離判斷現在在畫啥
        mx = std::max(mx, dis);
    }
    float ratio = mx / len;
    if (ratio < 0.1f) return 0;
    else if (ratio < 4.0f) {
        // to avoid screen froze
        if (mouse_trace.size() > 300) return 0;
        return 1;
    }
    else return 2;
}

int stall_index = 0;
//if user is drawing and stall (but not actually drawing)
void drawing_stall(int value) {
    if (!drawing) return;
    //the last callback is the same one, meaning user is stalling
    if (value != stall_index)  return;
    switch (get_stalling_type()) {
    case 0: {
        auto rl = new RoundedLine(mouse_trace[0],mouse_trace.back(), 
        brush_size,color_trace.front(),color_trace.back());
        drawing_queue.push(rl);
    }
        break;
    case 1: {
        auto sc = new SmoothCurve(mouse_trace, brush_size,color_trace);
        drawing_queue.push(sc);
    }
        break;
    case 2: {
        auto cir = minimum_enclosing_circle(mouse_trace);
        float radii = abs(cir - mouse_trace[0]);
        for (auto pt : mouse_trace) {
            radii = std::max(radii, abs(pt - cir));
        }
        auto dt = new Dot(cir, radii, global_paint, radii - brush_size,1000);
        drawing_queue.push(dt);
    }
        break;
    default:
        break;
    }
    //auto lm = new RoundedLine(fpoint,projected_mouse,10.0f);
    //drawing_queue.push(lm);
    drawing_cache.clear();
    mouse_trace.clear();
    color_trace.clear();
    mark_backup = true;
}

void add_brush_indicator() {
    brush_indicator = new BrushIndicator();
    brush_indicator->birth = elapsed;
    brush_indicator->timetolive = 100;
}
void add_mouse_indicator() {
    mouse_indicator = new MouseIndicator();
    mouse_indicator->birth = elapsed;
    mouse_indicator->timetolive = 100;
}
/*------------------------------------------------------------
 * Callback function handling mouse-press events
 */

bool polygon_fill = true;
void setup_canva_scale_animation() {
    Canvas[canva_brush]->birth = elapsed;
    Canvas[canva_brush]->timetolive = 100;
    Canvas[canva_brush]->showing_scale = true;
    Canvas[canva_brush]->markdead = false;
}

#define SMOOTH_FILL 1
#define NO_FILL 2
#define SMOOTH 3
#define FILL 4
int polygon_mode = 4;
void mouse_func(int button, int state, int x, int y){
    update_mouse_position(x, y);
    if (current_canva >= 0) Canvas[current_canva]->dragging = false;
    current_canva = -1;
    //從 topmost 向下 traverse
    for (auto i : canva_layers | std::views::reverse) {
        auto _canva = Canvas[i];
        if (_canva->mouse_inside()) {
            current_canva = _canva->z_index;
            break;
        }
    }
    
    //move current to topmost
    std::vector<int> ind_tmp;
    for (auto c : canva_layers) {
        if (c == current_canva) continue;
        ind_tmp.push_back(c);
    }
    if (current_canva >= 0) ind_tmp.push_back(current_canva);
    canva_layers = ind_tmp;

    // Scroll up
    if (button == 3) {
        if (adjusting_opacity) {
            global_opacity = std::max(global_opacity - 0.02f, 0.0f);
            global_paint.a = global_opacity;
            add_mouse_indicator();
            mouse_indicator->text = "alpha : ";
            mouse_indicator->now_indicate = global_opacity;
            return;
        }
        if (canva_brush >= 0) {
            if (rotating_canva) {
                Canvas[canva_brush]->angle = std::max(Canvas[canva_brush]->angle - 0.01, -2 * pi);
                setup_canva_scale_animation();
                return;
            }
            Canvas[canva_brush]->scale = std::max(Canvas[canva_brush]->scale - 0.05f, 0.1f);
            setup_canva_scale_animation();
            return;
        }
        brush_size = std::max(brush_size - 1.0f, 1.0f);
        add_brush_indicator();
        return;
    }
    // Scroll down
    if (button == 4) {
        if (adjusting_opacity) {
            global_opacity = std::min(global_opacity + 0.02f, 1.0f);
            global_paint.a = global_opacity;
            add_mouse_indicator();
            mouse_indicator->text = "alpha : ";
            mouse_indicator->now_indicate = global_opacity;
            return;
        }
        if (canva_brush >= 0) {
            if (rotating_canva) {
                Canvas[canva_brush]->angle = std::min(Canvas[canva_brush]->angle + 0.01,2 * pi);
                setup_canva_scale_animation();
                return;
            }
            Canvas[canva_brush]->scale = std::min(Canvas[canva_brush]->scale + 0.05f, 10.0f);
            setup_canva_scale_animation();
            return;
        }
        brush_size = std::min(brush_size + 1.0f, 1000.0f);
        add_brush_indicator();
        return;
    }

    if (button == GLUT_MIDDLE_BUTTON) {
        if (current_canva >= 0) {
            Canvas[current_canva]->dragging = (state == GLUT_DOWN);
            dragging = false;
            return;
        }
        dragging = (state == GLUT_DOWN);
        return;
    }

    if (state == GLUT_DOWN) for (auto index : onDown_trigger_list) uielement_list[index]->OnMouseDown(button);
    if(state == GLUT_UP) for (auto index : onUp_trigger_list) uielement_list[index]->OnMouseUp(button);

    if ((button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)) {
        for (auto index : onClick_trigger_list) {
            //Click test
            if (uielement_list[index]->OnMouseClick()) {
                return;
            }
        }
    }
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN && typing) {
            typing_cursor = viewport_mouse;
            return;
        }
        if (typing) return;
        if (state == GLUT_DOWN && pen_tool) {
            if ((abs(poly_mouse.x - poly_omouse.x) +    \
                abs(poly_mouse.y - poly_omouse.y) <= 2) \
                && !first_pen) {
                switch (polygon_mode) {
                case FILL: {
                    SimplePolygon* polygon = new SimplePolygon(polygon_cache, polygon_color);
                    drawing_queue.push(polygon);
                    break;
                }
                case SMOOTH: {
                    std::vector<Point2d> tmp;
                    for (auto c : polygon_cache) tmp.push_back(Point2d(TP(c)));
                    tmp.push_back(tmp.front());
                    polygon_color.push_back(polygon_color.front());
                    SmoothPolygon* polygon = new SmoothPolygon(tmp,brush_size, polygon_color,false);
                    drawing_queue.push(polygon);
                    break;
                }
                case SMOOTH_FILL: {
                    std::vector<Point2d> tmp;
                    for (auto c : polygon_cache) tmp.push_back(Point2d(TP(c)));
                    tmp.push_back(tmp.front());
                    polygon_color.push_back(polygon_color.front());
                    SmoothPolygon* polygon = new SmoothPolygon(tmp, brush_size, polygon_color, true);
                    drawing_queue.push(polygon);
                    break;
                }
                case NO_FILL: {
                    auto line = new RoundedLine(projected_mouse, Point2d(TP(polygon_cache[0])), brush_size, polygon_color.back(), global_paint);
                    drawing_cache.push_back(line);
                    for (auto c : drawing_cache) drawing_queue.push(c);
                    break;
                }
                }
                drawing_cache.clear();  
                polygon_cache.clear();
                polygon_color.clear();
                pen_tool = false;
                first_pen = true;
                return;
            }
            auto am = new Dot(projected_mouse, brush_size / 2.0f);
            polygon_color.push_back(global_paint);
            drawing_cache.push_back(am);
            polygon_cache.push_back(Point2i(TP(projected_mouse)));
            if (!first_pen) {
                auto line = new RoundedLine(poly_projected_omouse, projected_mouse, brush_size,polygon_color.back(),global_paint);
                drawing_cache.push_back(line);
            }
            poly_projected_omouse = projected_mouse;
            first_pen = false;
            //firs_pen_point = Point2i(TP(projected_mouse));
            poly_omouse = poly_mouse;
            poly_mouse = Point2i(x, y);
            mark_backup = true;
            return;
        }
        if (pen_tool) return;
        drawing = (state == GLUT_DOWN);
        if (drawing) {
            first_point = true;
            if (canva_brush < 0) {
                auto am = new Dot(projected_mouse, brush_size / 2.0f);
                drawing_cache.push_back(am);
            }
            else {
                canva_draw_cache.push_back(projected_mouse);
            }
        }
        else {
            if (drawing_cache.size() > 2) {
                drawing_cache.erase(drawing_cache.begin());
            }
            for (auto c : drawing_cache) drawing_queue.push(c);
            drawing_cache.clear();
            mouse_trace.clear();
            mark_backup = true;
        }
    }
}

/*-------------------------------------------------------------
 * motion callback function. The mouse is pressed and moved.
 */
void motion_func(int  x, int y){
    update_mouse_position(x, y);
    for (auto index : onMove_trigger_list) uielement_list[index]->OnMouseMove(true);
    if (drawing) {
        if (canva_brush >= 0) {
            canva_draw_cache.push_back(projected_mouse);
            return;
        }
        draw_smooth_trace();
        mouse_trace.push_back(projected_mouse);
        color_trace.push_back(global_paint);
        glutTimerFunc(1000, drawing_stall, ++stall_index);
        return;
    }
    if (current_canva >= 0 && Canvas[current_canva]->dragging) {
        Canvas[current_canva]->position.x += mdelta_x;
        Canvas[current_canva]->position.y += mdelta_y;
    }
    if (dragging) {
        world_x += mdelta_x;
        world_y += mdelta_y;
    }
}

void passive_motion_func(int x, int y) {
    update_mouse_position(x, y);
    for (auto index : onMove_trigger_list) uielement_list[index]->OnMouseMove(false);
}

void redef_viewport() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (double)width, 0.0, (double)height);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_MODELVIEW);
}
/*-------------------------------------------------------------
 * reshape callback function for window.
 */
void my_reshape(int new_w, int new_h){
    height = new_h;
    width = new_w;
    wcenx = height / 2.0;
    wceny = height / 2.0;

    redef_viewport();
    glutPostRedisplay();   /*---Trigger Display event for redisplay window*/
}

/*--------------------------------------------------------
 * procedure to clear window
 */
void init_window(void){
    redef_viewport();
    glLoadIdentity();
    clear_screen();
    glFlush();
}


/*------------------------------------------------------
 * Procedure to initialize data alighment and other stuff
 */
void init_func(){
    glDrawBuffer(GL_BACK);
    glReadBuffer(GL_BACK);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

#define ACTION_SELECT_VALUE 1
#define ACTION_TYPING_VALUE 2
#define ACTION_ERASER_VALUE 3
#define ACTION_PLPENS_VALUE 4
#define ACTION_FILLMO_VALUE 5

int top_m;
float g_opa_tmp = 0;
Color g_color_tmp;
void top_menu(int value){
    switch (value) {
    case ACTION_SELECT_VALUE:
        printf("hey\n");
        break;
    case ACTION_TYPING_VALUE:
        typing = !typing;
        break;
    case ACTION_ERASER_VALUE:
        erasering = !erasering;
        if (erasering) {
            color_wheel->enable_color_animation = false;
            g_color_tmp = global_paint;
            global_paint = Color{ 0,0,0,1 };
            g_opa_tmp = global_opacity;
            global_opacity = 1.0f;
        }
        else {
            global_opacity = g_opa_tmp;
            global_paint = g_color_tmp;
        }
        break;
    case ACTION_PLPENS_VALUE:
        pen_tool = !pen_tool;
        break;
    case ACTION_FILLMO_VALUE:
        polygon_fill = !polygon_fill;
        break;
    default:
        break;
    }
}
int font_m;
void font_menu(int value) {
    current_font = (void*)value;
}

int canva_m;
#define _50X50 1
#define _100X100 2
#define _500X500 3
#define _1000X1000 4
void add_canva_menu(int value) {
    Point2i size(100,100);
    switch (value) {
        case 1:
            size = Point2i(50, 50);
            break;
        case 2:
            size = Point2i(100, 100);
            break;
        case 3:
            size = Point2i(500, 500);
            break;
        case 4:
            size = Point2i(1000, 1000);
            break;
        break;
    }
    std::string name = "Canva";
    name += std::to_string((canva_layers.size()));
    auto ss = name.c_str();
    Image* can = new Image(size.x, size.y, ss);
    can->position = projected_mouse;
    can->z_index = canva_layers.size();
    Canvas.push_back(can);
    canva_layers.push_back(can->z_index);
}

void read_canvas() {
    int i = 0;
    while (1) {
        std::string name = "Canva";
        name += std::to_string(i);
        auto ss = name.c_str();
        FILE* file = fopen(ss, "r");
        if (file) {
            Image* can = new Image(ss);
            can->position = projected_mouse;
            can->z_index = canva_layers.size();
            Canvas.push_back(can);
            canva_layers.push_back(can->z_index);

        }
        else {
            break;
        }
        ++i;
    }
}
int fill_m;
void change_fill_mode(int value) {
    polygon_mode = value;
}
/*---------------------------------------------------------------
 * Main procedure sets up the window environment.
 */
int main(int argc, char** argv){
#pragma region Registrations 
    glutInit(&argc, argv);    /*---Make connection with server---*/

    int  size_menu;
    glutInitWindowPosition(0, 0);  /*---Specify window position ---*/
    glutInitWindowSize(width, height); /*--Define window's height and width--*/
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); /*---set display mode---*/
    init_func();
    /* Create parent window */
    glutCreateWindow("Lament, Counter-clock we rose");
    glClear(GL_COLOR_BUFFER_BIT);

    glutDisplayFunc(display_func); /* Associate display event callback func */
    glutReshapeFunc(my_reshape);  /* Associate reshape event callback func */

    glutKeyboardFunc(keyboard); /* Callback func for keyboard event */

    glutMouseFunc(mouse_func);  /* Mouse Button Callback func */
    glutMotionFunc(motion_func);/* Mouse motion event callback func */
    glutPassiveMotionFunc(passive_motion_func); // allow to track mouse in anytime

#pragma endregion

#pragma region Menus


    font_m = glutCreateMenu(font_menu);
    glutAddMenuEntry("GLUT_BITMAP_HELVETICA_18", (int)GLUT_BITMAP_HELVETICA_18);
    glutAddMenuEntry("GLUT_BITMAP_TIMES_ROMAN_10", (int)GLUT_BITMAP_TIMES_ROMAN_10);
    glutAddMenuEntry("GLUT_BITMAP_TIMES_ROMAN_24", (int)GLUT_BITMAP_TIMES_ROMAN_24);

    canva_m = glutCreateMenu(add_canva_menu);
    glutAddMenuEntry("50x50", _50X50);
    glutAddMenuEntry("100x100", _100X100);
    glutAddMenuEntry("500x500", _500X500);
    glutAddMenuEntry("1000x1000", _1000X1000);

    fill_m = glutCreateMenu(change_fill_mode);
    glutAddMenuEntry("None", NO_FILL);
    glutAddMenuEntry("Smooth fill", SMOOTH_FILL);
    glutAddMenuEntry("Fill", FILL);
    glutAddMenuEntry("Smooth", SMOOTH);

    top_m = glutCreateMenu(top_menu);
    glutAddMenuEntry("Select", ACTION_SELECT_VALUE);
    glutAddMenuEntry("Typing", ACTION_TYPING_VALUE);
    glutAddMenuEntry("Eraser", ACTION_ERASER_VALUE);
    glutAddMenuEntry("Pen Tool", ACTION_PLPENS_VALUE);
    glutAddSubMenu("Fill Mode", fill_m);
    glutAddSubMenu("Add Canva", canva_m);
    glutAddSubMenu("Font", font_m);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    /*---Test whether overlay support is available --*/
    if (glutLayerGet(GLUT_OVERLAY_POSSIBLE)) {
        fprintf(stderr, "Overlay is available\n");
    }
    else {
        fprintf(stderr, "Overlay is NOT available, May encounter problems for menu\n");
    }
    
#pragma endregion
#pragma region color_wheel

    //add color wheel
    color_wheel = new ColorWheel(120,120);
    color_wheel->enable_color_animation = true;
    color_wheel->timetolive = -1;
    color_wheel->birth = 0;
    
    uielement_list.push_back(color_wheel);
    onClick_trigger_list.push_back(uielement_list.size() - 1);
#pragma endregion

    grid = new Grid();
    read_canvas();
    if (!Canvas.size()) {
        Image* Canva = new Image(512, 512,"Canva0");
        Canva->position = Point2d(512, 512);
        Canva->z_index = 0;
        canva_layers.push_back(Canva->z_index);
        Canvas.push_back(Canva);
    }
    glutTimerFunc(30, CleanUpLoop, -1);
    glutTimerFunc(1, AnimationLoop, -1);


    /*---Enter the event loop ----*/
    glutMainLoop();
}

