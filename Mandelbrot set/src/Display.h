/*
 * Display.h
 *
 *  Created on: 15.01.2017
 *      Author: Alexander Wilms
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <vector>
#include <SDL.h>

using std::vector;

class Display {
private:
	vector<vector<double>> data;
	SDL_Rect* rect;
	SDL_Renderer* renderer;
	SDL_Window* window;
	int xsize;
	int ysize;
public:
	Display(int, int);
	virtual ~Display();
	void setdata(vector<vector<double>> array);
	void updateGraphics();
};

#endif /* DISPLAY_H_ */
