// based on https://github.com/Alexander-Wilms/Robotics-simulation/blob/master/Termin%206%20-%20Breitensuche/Termin6/Termin6/main.cpp

#include <stdio.h>
#include <SDL.h>
#include <SDL_endian.h>
#include <SDL_events.h>
#include <SDL_pixels.h>
#include <SDL_render.h>
#include <SDL_stdinc.h>
#include <SDL_surface.h>
#include <SDL_timer.h>
#include <SDL_video.h>
#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>
#include <cstdlib>
#include <time.h>
#include <cmath>

#define at(x) operator[](x)

using std::vector;

/*
 *  In der Cell Struktur werden Ergebnisse aus dem Algorithmus gespeichert
 */
struct Cell {
	bool bMarked;  // Zeigt an, ob das Feld schon besucht wurde (true = besucht)
	int nLastX; // Die X Koordinate des vorherigen Feldes (-1 = kein vorheriges Feld)
	int nLastY;    // Die Y Koordinate des vorherigen Feldes
};

struct Coordinate {
	int x, y;
};

// Uninteressant
bool operator<(const Coordinate& c1, const Coordinate& c2) {
	return false;
}
bool operator==(const Coordinate& c1, const Coordinate& c2) {
	return false;
}

/*
 *  In dieser Tabelle steht die Reihenfolge in der die umliegenden
 *  Felder abgearbeitet werden sollen
 */
const int DirTable[8][2] = { { -1, 0 },  // oben
		{ 0, 1 },  // rechts
		{ 1, 0 },  // unten
		{ 0, -1 },  // links
		{ -1, 1 },  // rechts-oben
		{ 1, 1 },  // rechts-unten
		{ 1, -1 },  // links-unten
		{ -1, -1 } };   // links-oben
// Globale Variablen
int height;  // Hoehe des Konfigurationsraums
int width;   // Breite des Konfigurationsraums

SDL_Surface* surface;
SDL_Texture* texture;

// Funktionsprototypen
bool FindPath(int nStartX, int nStartY, int nGoalX, int nGoalY);
void ClearCells();

using std::cout;
using std::cin;
using std::endl;
using std::vector;

SDL_Window* window;
SDL_Renderer* renderer;

// http://sdl.beuc.net/sdl.wiki/Pixel_Access
// https://www.gamedev.net/topic/530253-sdl-getting-rgb-values-of-a-surfaces-pixel/
SDL_Color getpixel(SDL_Surface *surface, int x, int y) {
	const Uint8 &bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	Uint8 *p = (Uint8 *) surface->pixels + y * surface->pitch + x * bpp;

	Uint32 result = 0;

	switch (bpp) {
	case 1:
		result = *p;
		break;
	case 2:
		result = *(Uint16 *) p;
		break;
	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			result = (p[0] << 16 | p[1] << 8 | p[2]);
		else
			result = (p[0] | p[1] << 8 | p[2] << 16);
		break;
	case 4:
		result = *(Uint32 *) p;
		break;
	default:
		result = 0; /* shouldn't happen, but avoids warnings */
	}

	SDL_Color theKey;
	SDL_GetRGB(result, surface->format, &theKey.r, &theKey.g, &theKey.b);

	return theKey;
}

// http://sdl.beuc.net/sdl.wiki/Pixel_Access
void putpixel(SDL_Surface *surface, int x, int y, SDL_Color pixel) {
	Uint32 flattened_pixel = SDL_MapRGB(surface->format, pixel.r, pixel.g, pixel.b);
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to set */
	Uint8 *p = (Uint8 *) surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp) {
	case 1:
		*p = flattened_pixel;
		break;

	case 2:
		*(Uint16 *) p = flattened_pixel;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			p[0] = (flattened_pixel >> 16) & 0xff;
			p[1] = (flattened_pixel >> 8) & 0xff;
			p[2] = flattened_pixel & 0xff;
		} else {
			p[0] = flattened_pixel & 0xff;
			p[1] = (flattened_pixel >> 8) & 0xff;
			p[2] = (flattened_pixel >> 16) & 0xff;
		}
		break;

	case 4:
		*(Uint32 *) p = flattened_pixel;
		break;
	}
}

vector<vector<SDL_Color> > configuration_space_colors;
vector<vector<Cell> > configuration_space_cells;

SDL_Event e;

void deinit();

void update();

int main(int, char**) {

	srand(time(NULL));

	std::cout.setstate(std::ios_base::failbit);

	SDL_Init(SDL_INIT_VIDEO);

	surface = SDL_LoadBMP("cspace.bmp");

	window = SDL_CreateWindow("2D Pathfinding", SDL_WINDOWPOS_CENTERED,
	SDL_WINDOWPOS_CENTERED, surface->w, surface->h, SDL_WINDOW_SHOWN);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, 360, 360);

	update();

	vector<vector<int> > bmp_array;

	width = surface->w;
	height = surface->h;

	configuration_space_colors.resize(width, vector<SDL_Color>(height));
	configuration_space_cells.resize(width, vector<Cell>(height));

	for (int x = 0; x < surface->w; x++) {
		for (int y = 0; y < surface->h; y++) {
			configuration_space_colors.at(x).at(y) = getpixel(surface, x, y);
			configuration_space_cells.at(x).at(y).bMarked = false;
		}
	}

	int x, y, length = 0;

	// Datei:
	// cspace.png            (100,300) -> (300,20)
	// cspace_prismatic.png  (620,320) -> (550,410)
	// cspace_revolute.png   (350,180) -> (120,180)
	// Startposition
	int nStartX = 100;
	int nStartY = 300;
	// Zielposition
	int nGoalX = 300;
	int nGoalY = 20;

	Uint32 starttime = SDL_GetTicks();

	// Pfad suchen
	bool bFound = FindPath(nStartX, nStartY, nGoalX, nGoalY);

	Uint32 dwElapsed = SDL_GetTicks() - starttime;
	starttime = SDL_GetTicks();
	std::cout.clear();
	cout << "Die Suche dauerte: " << dwElapsed << "ms" << endl;
	cout << "Pfad gefunden: " << bFound << endl;

	// Überprüfen, ob ein Pfad gefunden wurde
	if (bFound) {
		//update();
		//FILE *fp = fopen("path.prg", "w");
		//if (fp != NULL)
		//fp = stdout;

		// Den Pfad vom Ziel zum Start zurückgehen und den Pfad markieren
		x = nGoalX;
		y = nGoalY;

		Cell cell;
		while (configuration_space_cells.at(x).at(y).nLastX != -1) // Bei -1 sind wir am Start
		{
			SDL_PollEvent(&e);
			if (e.type == SDL_QUIT) {

				deinit();

				return 0;
			}

			//cout << "LastX: " << configuration_space_cells.at(x).at(y).nLastX << endl;
			// EASY-ROB Programm erzeugen
			// Die Gelenkwerte müssen entsprechend den Zellen angepasst werden!!!
			//fprintf(fp, "JUMP_TO_AX %f %f\n", (x - width / 2) * 0.002f, (y - height / 2) * 0.002f); // von -1m bis +1m in 20mm Schritten
			//fprintf(fp, "JUMP_TO_AX %f %f\n", x - width/2.0f, y - width/2.0f); // von -180° bis +180° in 1° Schritten
			// Schwarzen Pixel setzen
			configuration_space_colors.at(x).at(y).r = 0;
			configuration_space_colors.at(x).at(y).g = 0;
			configuration_space_colors.at(x).at(y).b = 0;
			putpixel(surface, x, y, configuration_space_colors.at(x).at(y));
			cell = configuration_space_cells.at(x).at(y);
			x = cell.nLastX;
			y = cell.nLastY;
			++length;
			//update();
		}

		dwElapsed = SDL_GetTicks() - starttime;
		cout << "Pfad der Laenge " << length << " gefunden in " << dwElapsed << " ms" << endl;
		update();

		//std::string file_save = file_basename + "_bfs.png";
		// Bild ausgeben, so das man den Pfad sehen kann
		//if (!SaveAsPNG(file_save.c_str(), cspace, width, height))
		//printf("Fehler beim Erzeugen der PNG Datei\n");

		//if (fp != stdout)
		//fclose(fp);
	} else {
		printf("Keinen Pfad gefunden!\n");
	}

	while (true) {
		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {

			deinit();

			return 0;
		}
	}

}

void update() {
	SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);

	SDL_RenderCopy(renderer, texture, NULL, NULL);

	SDL_RenderPresent(renderer);
}

void deinit() {
	SDL_DestroyTexture(texture);

	SDL_DestroyRenderer(renderer);

	SDL_DestroyWindow(window);

	SDL_Quit();
}

/*
 *  FindPath - Breadth-First (Breitensuche)
 *  Algorithmus zum finden eines Pfades vom Start zum Endpunkt (goal)
 */
bool FindPath(int nStartX, int nStartY, int nGoalX, int nGoalY) {
	printf("Suche Pfad von x=%d y=%d nach x=%d y=%d\n", nStartX, nStartY, nGoalX, nGoalY);

	// initalize cells
	for (int my_y = 0; my_y < height; my_y++) {
		for (int my_x = 0; my_x < width; my_x++) {
			configuration_space_cells[my_x][my_y].bMarked = false;
		}
	}

	// create queue
	std::queue<Coordinate> queue;

	// starting cell
	Coordinate c;
	c.x = nStartX;
	c.y = nStartY;

	// push first cell into queue
	queue.push(c);

	// mark starting cell for backtracing
	configuration_space_cells[nStartX][nStartY].nLastX = -1;

	// mark starting cell
	configuration_space_cells[c.x][c.y].bMarked = true;
	double color = 0;
	int iteration_counter = 0;
	// while there's still cells in the queue...
	while (!queue.empty()) {
		// slow down and update rendering
		//iteration_counter++;
		/*if (iteration_counter % 100 == 0) {
			update();
			SDL_Delay(1);
		}*/

		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {
			return false;
		}

		// get the first element in the queue
		c.x = queue.front().x;
		c.y = queue.front().y;

		queue.pop();

		// if it has the desired coordinates we terminate
		if (c.x == nGoalX && c.y == nGoalY) {
			printf("last: %d %d \n", configuration_space_cells[c.x][c.y].nLastX, configuration_space_cells[c.x][c.y].nLastY);
			return true;
		}

		Coordinate tmp;
		int i_tmp;
		for (int i = 0; i < 8; i++) {
			i_tmp = i;
			// randomize walk
			// i = rand()%9;
			// make sure we're still within the array's bounds
			if (c.x + DirTable[i][0] >= 0 && c.x + DirTable[i][0] < height && c.y + DirTable[i][1] >= 0 && c.y + DirTable[i][1] < width) {
				// if we haven't checked the next cell yet...
				if (configuration_space_cells.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).bMarked == false) {
					// mark cell
					configuration_space_cells.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).bMarked = true;

					// if there's no collision in the next cell...
					if (configuration_space_colors.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).r == 255 && configuration_space_colors.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).g == 255 && configuration_space_colors.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).b == 255) {

						// watch the BFS as it happens
						cout << "coloring pixel" << endl;
						configuration_space_colors.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).r = 127;//(int) color;
						configuration_space_colors.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).g = 127;//(int) color;
						configuration_space_colors.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).b = 127;//(int) color;

//						color += 0.5;
//						//color=std::fmod(color,255);
//						if (color > 255)
//							color = 0;

						putpixel(surface, c.x + DirTable[i][0], c.y + DirTable[i][1], configuration_space_colors.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]));

						configuration_space_cells.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).nLastX = c.x;
						configuration_space_cells.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).nLastY = c.y;
						tmp.x = c.x + DirTable[i][0];
						tmp.y = c.y + DirTable[i][1];
						queue.push(tmp);
					}
				}
			}
			i = i_tmp;
		}
	}
	return false;
}

/*
 *  ClearCells()
 *  Setzt alle Zellen auf die Initialwerte zurück
 */
void ClearCells() {
	// Zellen zurücksetzen
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			configuration_space_cells.at(x).at(y).bMarked = false;   // Nicht besucht
			configuration_space_cells.at(x).at(y).nLastX = -1; // Keine vorherige Zelle
			configuration_space_cells.at(x).at(y).nLastY = -1;
		}
	}
}
