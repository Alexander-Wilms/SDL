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
	vector<vector<bool>> data;
	SDL_Rect* rect;
	SDL_Renderer* renderer;
	SDL_Window* window;
public:
	Display(int);
	virtual ~Display();
	void setdata(vector<vector<bool>> array);
	void updateGraphics();
};



#endif /* DISPLAY_H_ */
