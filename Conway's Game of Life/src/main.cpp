// Compile with -O3 (optimize most) for a better frame rate

#include <iostream>
#include <cstdlib>
#include <time.h>
#include <vector>
#include <SDL.h>
#include "Display.h"

void initRand();

#define SIZE 500

using std::cout;
using std::endl;
using std::vector;

int neighbors[8][2] = {
		{-1, -1}, 	// links unten
		{0,-1},		// unten
		{1,-1},		// rechts unten
		{1,0},		// rechts
		{1,1},		// rechts oben
		{0,1},		// oben
		{-1,1},		// links oben
		{-1,0}};	// links

bool glider[3][3] = {
		{false,false,true},
		{true,false,true},
		{false,true,true}};

bool blinker[3][3] = {
		{false,true,false},
		{false,true,false},
		{false,true,false}};

int a, b, c, d;

void run();

void computeNextStep();

void initPattern(bool glider2[][3], int width, int height);

int min(int a, int b);
int max(int a, int b);

Display display(SIZE);


vector<vector<bool>> data(SIZE, vector<bool>(SIZE));
vector<vector<bool>> dataOfNextStep(SIZE, vector<bool>(SIZE));

int main(int argc, char *argv[]){

	srand (time(NULL));

	a = rand() % SIZE;
	b = rand() % SIZE;
	c = rand() % SIZE;
	d = rand() % SIZE;

	run();


	return 0;
}



void run() {
	for(int i = 0; i <SIZE; i++) {
		for(int j = 0; j < SIZE; j++) {
			data[i][j] = false;
			dataOfNextStep[i][j] = false;
		}
	}

	for(int i = 0; i < 1000; i++)
		initRand();

	//initPattern(glider,3,3);

	bool running = true;
	SDL_Event event;

	while(running) {


		while( SDL_PollEvent( &event ) ) {
			if( event.type == SDL_QUIT ) {
				running = false;
			}
		}

		computeNextStep();

		data = dataOfNextStep;

		for(int i = 0; i <SIZE; i++) {
			for(int j = 0; j < SIZE; j++) {
				dataOfNextStep[i][j] = false;
			}
		}

		display.setdata(data);

		display.updateGraphics();
	}
}

void initRand() {
	/*cout << min(a,b) << endl;
	cout << max(a,b) << endl;
	cout << min(c,d) << endl;
	cout << max(c,d) << endl;*/

	for(int i = min(a,b); i < max(a,b); i++) {

		for(int j = min(c,d); j < max(c,d); j++) {

			if(rand()%2) {
				data[i][j] = true;
			}
		}
	}
}

int min(int a, int b) {
	return a < b? a : b;
}

int max(int a, int b) {
	return a > b? a : b;
}

void initPattern(bool glider2[][3], int width, int height) {
	int x = SIZE/2-width/2, y = SIZE/2-height/2;
	for(int i = 0; i < width; i++) {
		for(int j = 0; j < height; j++) {
			data[x+i][y+j] = glider2[i][j];
			cout << data[x+i][y+j] << endl;
		}
	}
}

void printData() {
	for(int i = 0; i <SIZE; i++) {
		for(int j = 0; j <SIZE; j++) {
			cout << data[i][j] << endl;
		}
		cout << endl;
	}
}

void computeNextStep() {
	int liveNeighbors = 0;

	for(int i = 0; i < SIZE; i++) {
		for(int j = 0; j < SIZE; j++) {
			liveNeighbors = 0;

			//printData();

			for(int k = 0; k < 8; k++) {
				if(0 <= i+neighbors[k][0] && i+neighbors[k][0] < SIZE && 0 <= j+neighbors[k][1] && j+neighbors[k][1] < SIZE) {
					//System.out.println("Testing " + (i+neighbors[k][1]) + ", " + (j+neighbors[k][0]));
					if(data[i+neighbors[k][0]][j+neighbors[k][1]] == true) {
						//System.out.println("Someone lives here");
						liveNeighbors++;
					}
				}
			}

			//System.out.println(i + " " + j +": " + liveNeighbors + "alive neighbors");

			if(data[i][j] == true) {
				// live cell
				if(liveNeighbors < 2)
					dataOfNextStep[i][j] = false;
				else if(liveNeighbors == 2 || liveNeighbors == 3)
					dataOfNextStep[i][j] = true;
				else if(liveNeighbors > 3)
					dataOfNextStep[i][j] = false;
			} else if(data[i][j] == false){
				// dead cell
				if(liveNeighbors == 3)
					dataOfNextStep[i][j] = true;
			}
		}
	}
}


