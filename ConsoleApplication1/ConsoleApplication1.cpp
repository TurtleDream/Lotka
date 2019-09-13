#include <iostream>
#include <utility>
#include <vector>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <SDL2/SDL.h>

using namespace std;

#define A 0.009
#define B 0.0000025
#define C 0.0012
#define D 0.0000003

void foo(pair<float, float>* p) {
	float temp = p->second;

	p->second += ((-C * p->second) + (D * p->first * p->second));
	p->first += ((A * p->first) - (B * temp * p->first));
}

void start() {
	pair <float , float > p;
	pair<float, float> *s = &p;

	p.first = 3000;
	p.second = 500;

	vector<pair<float, float>> v;

	v.push_back(p);

	for (int i = 0; i < 500000; i++) {

		foo(s);
		v.push_back(p);

		std::cout << "x = " << p.first << endl;
		std::cout << "y = "  << p.second << endl;
	}
}

SDL_Window* window; //объект окна, нужен для взаимодействия с оконной системой ОС
SDL_GLContext context; //объект OpenGL. Теоретически, все вызовы должны осуществляться его методами, но напрямую - проще

struct sGridLines {
	float min;
	float max;
	float diff;
	float pdiff;
};
const sGridLines X = { -5,5,1,0.05 };
const sGridLines Y = { -5,5,1,0.05 };
#define BG_COLOR 1,1,1
#define FG_COLOR 0,0,0
#define GRID_COLOR 0,1,0

bool inout(double x, double y) {
	if (fabs(x) > 3 || fabs(y) > 3) return false;
	return (x <= 0 ? fabs(y) >= fabs(x) : (x * x + y * y) <= 9);
}
void draw_grid() {
	float i;
	glBegin(GL_LINES);
	glColor4f(GRID_COLOR, 0.2);
	for (i = 0; i < X.max; i += X.diff) { glVertex2f(i, Y.min); glVertex2f(i, Y.max); }
	for (i = 0; i > X.min; i -= X.diff) { glVertex2f(i, Y.min); glVertex2f(i, Y.max); }
	for (i = 0; i < Y.max; i += Y.diff) { glVertex2f(X.min, i); glVertex2f(X.max, i); }
	for (i = 0; i > Y.min; i -= Y.diff) { glVertex2f(X.min, i); glVertex2f(X.max, i); }
	glColor4f(GRID_COLOR, 0.5);
	glVertex2f(X.min, 0);
	glVertex2f(X.max, 0);
	glVertex2f(0, Y.min);
	glVertex2f(0, Y.max);
	glEnd();
}
void draw() {
	float x, y;
	draw_grid();
	glBegin(GL_POINTS);
	glColor4f(FG_COLOR, 1);

	for (x = X.min; x < X.max; x += X.pdiff)
		for (y = Y.min; y < Y.max; y += Y.pdiff)if (inout(x, y))glVertex2f(x, y);
	glEnd();
}

char update() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) { return false; }
	}
	return true;
}

int main(int argc, char** argv) {
	window = SDL_CreateWindow("Open GL + SDL window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (window == NULL) { goto FINISH; } //не удалось создать окно - ошибка (на модели не обрабатывается)
	context = SDL_GL_CreateContext(window);
	if (context == NULL) { goto FINISH; } //не удалось привязать OpenGL к окну - ошибка (не обрабатывается)
	//настройки OpenGL
	glClearColor(BG_COLOR, 0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(X.min, X.max, Y.min, Y.max);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_BLEND); //разрешение полупрозрачности
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //метод обработки полупрозрачности (это- самый распространенный)

	while (update()) {
		glClear(GL_COLOR_BUFFER_BIT);
		draw();
		glFlush();
		SDL_GL_SwapWindow(window);
	}
FINISH:
	if (context) { SDL_GL_DeleteContext(context); context = 0; }
	if (window) { SDL_DestroyWindow(window); window = 0; }
	SDL_Quit();
}

