// based on https://github.com/Alexander-Wilms/Robotics-simulation/blob/master/Termin%206%20-%20Breitensuche/Termin6/Termin6/main.cpp

#include <iostream>
#include <fstream>
#include <SDL.h>
#include "SDL_image.h"
#include <vector>
#include <iomanip>
#include <stdio.h>
#include <queue>
#include <string> // for std::string
#include <stdlib.h> // for sleep()

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

//char **cspace; // 2D Konfigurationsraum. Falls cspace.at(x).at(y) != 0, dann verursacht die Konfiguration <x,y> eine Kollision
//Cell **aCells; // In diesem 2D Feld werden die Ergebisse des Algorithmus abgelegt

/*vector<vector<SDL_Color> > cspace;
 vector<vector<Cell> > aCells;*/

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
	y %= 360;
	x %= 360;
	if (y < 0)
		y = 0;
	if (x < 0)
		x = 0;

	//y = 360 - 1 - y;
	//x = 360-1-x;
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
	try {
		y %= 360;
		x %= 360;

		if (y < 0)
			y = 0;
		if (x < 0)
			x = 0;

		//y = 360 - 1 - y;
		//x=360-1-x;
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
	} catch (int e) {
		cout << "err in put" << endl;
	}
}

vector<vector<SDL_Color> > configuration_space_colors;
vector<vector<Cell> > configuration_space_cells;

SDL_Event e;

void deinit();

void update();

int main(int, char**) {

	//std::cout.setstate(std::ios_base::failbit);

	SDL_Init(SDL_INIT_VIDEO);

	surface = SDL_LoadBMP("cspace.bmp");

	window = SDL_CreateWindow("2D Pathfinding", SDL_WINDOWPOS_CENTERED,
	SDL_WINDOWPOS_CENTERED, surface->w, surface->h, SDL_WINDOW_SHOWN);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	//texture = SDL_CreateTextureFromSurface(renderer, surface);

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, 360, 360);

	cout << "pixel 0,0: " << endl;

	SDL_Color testcolor = getpixel(surface, 0, 0);

	cout << (int) testcolor.r << "," << (int) testcolor.g << "," << (int) testcolor.b << endl;

	update();

	vector<vector<int> > bmp_array;
	//vector<int>* color;

	width = surface->w;
	height = surface->h;

	cout << "width: " << width << endl;
	cout << "height: " << height << endl;

	//	for (int x = 0; x < surface->w; x++) {
	//		for (int y = 0; y < surface->h; y++) {
	//			color = getpixel(surface, x, y);
	//			cout << std::setw(3) << color->at(0) << "," << std::setw(3)
	//					<< color->at(1) << "," << std::setw(3) << color->at(2)
	//					<< "\t";
	//		}
	//		cout << endl;
	//	}

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
	//getchar();

	// Pfad suchen
	bool bFound = FindPath(nStartX, nStartY, nGoalX, nGoalY);

	cout << "returned from findpath" << endl;
	Uint32 dwElapsed = SDL_GetTicks() - starttime;
	cout << "Die Suche dauerte: " << dwElapsed << "ms" << endl;
	cout << "Pfad gefunden: " << bFound << endl;

	// Überprüfen, ob ein Pfad gefunden wurde
	if (bFound) {
		update();
		//FILE *fp = fopen("path.prg", "w");
		//if (fp != NULL)
		//fp = stdout;

		// Den Pfad vom Ziel zum Start zurückgehen und den Pfad markieren
		x = nGoalX;
		y = nGoalY;

		cout << "Goal x=" << x << ", y=" << y << endl;

		Cell cell;
		while (configuration_space_cells.at(x).at(y).nLastX != -1) // Bei -1 sind wir am Start
		{
			SDL_PollEvent(&e);
			if (e.type == SDL_QUIT) {

				deinit();

				return 0;
			}

			cout << "LastX: " << configuration_space_cells.at(x).at(y).nLastX << endl;
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
			if(cell.nLastX == -1)
				break;
			++length;
			update();
		}

		cout << "Pfad der Laenge " << length << " gefunden in " << dwElapsed << " ms" << endl;

		//std::string file_save = file_basename + "_bfs.png";
		// Bild ausgeben, so das man den Pfad sehen kann
		//if (!SaveAsPNG(file_save.c_str(), cspace, width, height))
		//printf("Fehler beim Erzeugen der PNG Datei\n");

		//SDL_Color tmp;
		/*for (int i = 0; i < surface->w; i++) {
		 for (int j = 0; j < surface->h; j++) {
		 putpixel(surface, i, j, configuration_space_colors[i][j]);
		 }
		 }*/
		//update();
		//if (fp != stdout)
		//fclose(fp);
	} else {
		printf("Keinen Pfad gefunden!\n");
	}
	// Cleanup
	/*for (y = 0; y < height; y++)    // Alle Cells vom Heap löschen
	 delete[] aCells[y];
	 delete[] aCells;*/

	while (true) {
		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {

			deinit();

			return 0;
		}
	}

}

void update() {
	if (getpixel(surface, 180, 180).r == 0)
		getchar();

	SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);

	if (texture == NULL)
		cout << "shit";
	//SDL_RenderClear(renderer);

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
	try {
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

		//  Folgende Zeilen demonstrieren die Manipulation von Queues (Warteschlange)
		//  Einfach auskommentieren und testen

		/* printf("Queue leer? %d\n", queue.empty());       // Am Anfang ist die Queue leer
		 queue.push(c);                                   // Element in die Queue anstellen
		 printf("Queue leer? %d\n", queue.empty());       // Jetzt sollte die Queue nicht mehr leer sein
		 printf("Elemente in Queue: %d\n", queue.size()); // Anzahl Elemente in der Queue
		 Coordinate test = queue.front();                 // front() gibt das forderste Element in der Queue zurück,
		 printf("x: %d, y: %d\n", test.x, test.y);        // jedoch wird das Element dadurch nicht entfernt!
		 queue.pop();                                     // Entfernt das erste Element aus der Queue
		 printf("Queue leer? %d\n", queue.empty());       // Nun sollte die Queue wieder leer sein
		 */

		// push first cell into queue
		queue.push(c);

		cout << "nStartY: " << nStartY << endl;
		cout << "nStartX: " << nStartX << endl;

		// mark starting cell for backtracing
		configuration_space_cells[nStartX][nStartY].nLastX = -1;

		cout << "nach zuweisung" << endl;

		// mark starting cell
		configuration_space_cells[c.x][c.y].bMarked = true;

		int color_int = 127;

		int everytens = 0;
		// while there's still cells in the queue...
		while (!queue.empty()) {
			//everytens++;
			//if (everytens % 100000 == 0)
				//update();
			cout << "in loop" << endl;
			cout << "error" << SDL_GetError() << endl;

			cout << "queue size: " << queue.size() << endl;
			SDL_PollEvent(&e);
			if (e.type == SDL_QUIT) {
				//update();
				cout << "quitting" << endl;
				cout << "queue size was: " << queue.size() << endl;
				cout << "error" << SDL_GetError() << endl;
				std::queue<Coordinate> empty;
				std::swap(queue, empty);
				break;
			}

			// get the first element in the queue
			c.x = queue.front().x;
			c.y = queue.front().y;
			//		color.r = 255;
			//			color.g = color_int2;
			//			color_int2++;
			//			color_int2%=255;
			//			color.b = 0;
			//			if(c.x < 0)
			//				c.x = 0;
			//			if(c.y < 0)
			//				c.y = 0;
			//			if(c.x>359)
			//				c.x = 359;
			//			if(c.y>359)
			//				c.y = 359;
			//			putpixel(surface, c.x, c.y, color);
			//update();

			/*SDL_Color color;
			 color.r = 0;
			 color.g = 0;
			 color.b = 0;
			 putpixel(surface, c.x, c.y, color);*/

			//update();
			cout << "x: " << c.x << endl;
			cout << "y: " << c.y << endl;

			queue.pop();

			// if it has the desired coordinates we terminate
			if (c.x == nGoalX && c.y == nGoalY) {
				cout << "arrived at goal coordinates" << endl;
				printf("found: %d %d \n", c.x, c.y);
				//printf("last: %d %d \n", configuration_space_cells[c.x][c.y].nLastX, configuration_space_cells[c.x][c.y].nLastY);
				return true;
			} else {
				cout << "have not arrived at goal" << endl;
			}

			update();

			Coordinate temp;
			for (int i = 0; i < 8; i++) {

				cout << "betrachte nachbar " << i << "an der stelle" << c.x + DirTable[i][0] << "," << c.y + DirTable[i][1] << endl;
				// make sure we're still within the array's bounds
				if (c.x + DirTable[i][0] >= 0 && c.x + DirTable[i][0] < height && c.y + DirTable[i][1] >= 0 && c.y + DirTable[i][1] < width) {
					// if we haven't checked the next cell yet...
					if (configuration_space_cells.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).bMarked == false) {
						cout << "betrachtete Zelle war noch nicht markiert" << endl;
						// mark cell
						configuration_space_cells.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).bMarked = true;

						// if there's no collision in the next cell...

						cout << "r of neighbor: " << (int) configuration_space_colors.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).r << endl;
						cout << "g of neighbor: " << (int) configuration_space_colors.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).g << endl;
						cout << "b of neighbor: " << (int) configuration_space_colors.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).b << endl;

						//if ((configuration_space_colors.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).r == 255 && configuration_space_colors.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).g == 255 && configuration_space_colors.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).b == 255) || (configuration_space_colors.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).r == configuration_space_colors.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).g && configuration_space_colors.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).g == configuration_space_colors.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).b)) {
							if (configuration_space_colors.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).r == 255 && configuration_space_colors.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).g == 255 && configuration_space_colors.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).b == 255) {

							cout << "coloring pixel" << endl;
							// watch the BFS as it happens
							//color_int %= 255;
							configuration_space_colors.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).r = 127;
							configuration_space_colors.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).g = 127;
							configuration_space_colors.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).b = 127;
							//color_int++;

							putpixel(surface, c.x + DirTable[i][0], c.y + DirTable[i][1], configuration_space_colors.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]));

							/*for (int k = 0; k < surface->w; k++) {
							 for (int l = 0; l < surface->h; l++) {
							 putpixel(surface, k, l, configuration_space_colors[k][l]);
							 }
							 }*/

							//update();
							configuration_space_cells.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).nLastX = c.x;
							configuration_space_cells.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).nLastY = c.y;
							temp.x = c.x + DirTable[i][0];
							temp.y = c.y + DirTable[i][1];
							queue.push(temp);
							cout << "pushed into queue" << endl;

							//color.r++;
							//color.g++;
							//color.b++;
						}
					} else {
						cout << "betrachtete Zelle war schon markiert" << endl;
					}
				}
				cout << "nachbar abgearbeitet" << endl;

			}

			cout << "queue size: " << queue.size() << endl;
		}
		cout << "after loop" << endl;
		return false; // Default Return
	} catch (int e) {
		cout << "shit" << endl;
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
