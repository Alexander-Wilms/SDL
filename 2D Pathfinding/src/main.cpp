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
#include <time.h>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <map>
#include <queue>
#include <set>
#include <utility>
#include <vector>

#define SIZE 360

//#define at(x) operator[](x)

using std::vector;
using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::priority_queue;

// global variables
// cell structure for BFS
struct BFS_Cell {
	bool bMarked;  // Zeigt an, ob das Feld schon besucht wurde (true = besucht)
	int nLastX; // Die X Koordinate des vorherigen Feldes (-1 = kein vorheriges Feld)
	int nLastY;    // Die Y Koordinate des vorherigen Feldes
};

typedef struct A_star_Node A_star_Node;

struct A_star_Node {
	int predecessor_x;
	int predecessor_y;
	int x;
	int y;
	mutable double f; // length of path from start node to goal node via this node
	mutable double g; // cost of path from start node to this node
	mutable double h; // estimate of cost of path from this node to goal node (Luftlinie)

	bool operator<(const A_star_Node& cell_2) const {
		return x < cell_2.x || (x == cell_2.x && y < cell_2.y);
	}

	bool operator==(const A_star_Node& cell_2) const {
		return (x == cell_2.x && y == cell_2.y);
	}

	bool operator!=(const A_star_Node& cell_2) const {
		return !(x == cell_2.x && y == cell_2.y);
	}
};

// coordinate structure for BFS
struct Coordinate {
	int x, y;
};

//  In dieser Tabelle steht die Reihenfolge in der die umliegenden
//  Felder abgearbeitet werden sollen
//const int DirTable[8][2] = { { -1, 0 },  // oben
//		{ 0, 1 },  // rechts
//		{ 1, 0 },  // unten
//		{ 0, -1 },  // links
//		{ -1, 1 },  // rechts-oben
//		{ 1, 1 },  // rechts-unten
//		{ 1, -1 },  // links-unten
//		{ -1, -1 } };   // links-oben

int DirTable[8][2] = { { 1, 1 },  // oben
		{ 0, 1 },  // rechts
		{ -1, 1 },  // unten
		{ -1, 0 },  // links
		{ -1, -1 },  // rechts-oben
		{ 0, -1 },  // rechts-unten
		{ 1, -1 },  // links-unten
		{ 1, 0 } };   // links-oben

int height;  // Hoehe des Konfigurationsraums
int width;   // Breite des Konfigurationsraums
SDL_Surface* surface;
SDL_Texture* texture;
vector<vector<SDL_Color> > configuration_space_colors;
vector<vector<BFS_Cell> > configuration_space_bfs_cells;
vector<vector<A_star_Node> > configuration_space_a_star_cells;
std::set<A_star_Node> openSet;
std::set<A_star_Node> closedSet;
SDL_Event e;
SDL_Window* window;
SDL_Renderer* renderer;

// function prototypes
bool breadth_first_search(int nStartX, int nStartY, int nGoalX, int nGoalY);
void ClearCells();
SDL_Color getpixel(SDL_Surface *surface, int x, int y);
void putpixel(SDL_Surface *surface, int x, int y, SDL_Color pixel);
void deinit();
void update();
void init();
int BFS(int nStartX, int nStartY, int nGoalX, int nGoalY);
int A_star(int nStartX, int nStartY, int nGoalX, int nGoalY);
void expand_node(A_star_Node current_node, A_star_Node goal_node);
double heuristic(A_star_Node start, A_star_Node goal);

void randomizeNeighbors() {
	int index_1 = rand()%8;
	int index_2 = rand()%8;
	while(index_1 == index_2) {
		index_2 = rand()%8;
	}
	int tmp1 = DirTable[index_1][0];
	int tmp2 = DirTable[index_1][1];

	DirTable[index_1][0] = DirTable[index_2][0];
	DirTable[index_1][1] = DirTable[index_2][1];

	DirTable[index_2][0] = tmp1;
	DirTable[index_2][1] = tmp2;
}

int main(int, char**) {

	init();

	// Datei:
	// cspace.png            (100,300) -> (300,20)
	// cspace_prismatic.png  (620,320) -> (550,410)
	// cspace_revolute.png   (350,180) -> (120,180)

	// Startposition
	int nStartX = 100;
	int nStartY = 300;

	// Zielposition
	int nGoalX = 140;
	int nGoalY = 40;

	//BFS(nStartX, nStartY, nGoalX, nGoalY);
	A_star(nStartX, nStartY, nGoalX, nGoalY);

	while (true) {
		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {
			deinit();
			return 0;
		}
	}
}

int A_star(int nStartX, int nStartY, int nGoalX, int nGoalY) {
	configuration_space_a_star_cells.resize(width, vector<A_star_Node>(height));

	for (int x = 0; x < surface->w; x++) {
		for (int y = 0; y < surface->h; y++) {
			configuration_space_colors.at(x).at(y) = getpixel(surface, x, y);
			configuration_space_a_star_cells.at(x).at(y).x = x;
			configuration_space_a_star_cells.at(x).at(y).y = y;
			configuration_space_a_star_cells.at(x).at(y).predecessor_x = -1;
			configuration_space_a_star_cells.at(x).at(y).predecessor_y = -1;
			configuration_space_a_star_cells.at(x).at(y).g = -1;
		}
	}


	A_star_Node start_node;
	start_node.x = nStartX;
	start_node.y = nStartY;
	start_node.f = 0;
	start_node.g = 0;

	cout << start_node.x << endl;
	cout << start_node.y << endl;

	openSet.insert(start_node);

	A_star_Node goal_node;
	goal_node.x = nGoalX;
	goal_node.y = nGoalY;

	A_star_Node min_node;
	min_node.x = 0;
	min_node.y = 0;

	A_star_Node max_node;
	max_node.x = SIZE-1;
	max_node.y = SIZE-1;

	double start_distance = heuristic(min_node, max_node);

	start_node.h = start_distance;

	int iteration_counter = 0;

	A_star_Node current_node;

	while (!openSet.empty()) {
		//getchar();
		cout << "---" << endl << "selecting next node" << endl;
		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {
			deinit();
			return 0;
		}

		iteration_counter++;
		if (iteration_counter % 1000000 == 0) {
			update();
			//SDL_Delay(100);
		}

		double min_f = 0;
		double min_h = start_distance;

		std::set<A_star_Node>::iterator iter;

		int iteration = 0;

		for (iter = openSet.begin(); iter != openSet.end(); iter++) {
			//cout << "checking f: " << iter->f << endl;
//			if (iteration == 0) {
//				current_node = *iter;
//				min_f = iter->f;
//				min_h = iter->h;
//				iteration++;
//			} else if ( iter->h < min_h ) {
//				current_node = *iter;
//				min_f = iter->f;
//				min_h = iter->h;
//			} else if (iter->f < min_f && iter->h == min_h) {
//				current_node = *iter;
//				min_h = iter->h;
//				min_f = iter->f;
//			}

			if (iteration == 0) {
				current_node = *iter;
				min_f = iter->f;
				min_h = iter->h;
				iteration++;
			} else if ( iter->f < min_f ) {
				current_node = *iter;
				min_f = iter->f;
				min_h = iter->h;
			} else if (iter->h < min_h && iter->f == min_f) {
				current_node = *iter;
				min_h = iter->h;
				min_f = iter->f;
			}

//			if (iteration == 0) {
//				current_node = *iter;
//				min_f = iter->f;
//				iteration++;
//			} else if ( iter->f < min_f ) {
//				current_node = *iter;
//				min_f = iter->f;
//			}
		}

		//getchar();

		//cout << "final f: " << current_node.f << endl;

		cout << "centraler node: (" << current_node.x << "," << current_node.y << ")" << endl;

		double h = current_node.h;

		//cout << "update screen" << endl;
		configuration_space_colors.at(current_node.x).at(current_node.y).r = fmod(h/start_distance*255,255);
		cout << "color: " << fmod(h/start_distance*255,255) << endl;
		configuration_space_colors.at(current_node.x).at(current_node.y).g = fmod(h/start_distance*255,255);
		configuration_space_colors.at(current_node.x).at(current_node.y).b = fmod(h/start_distance*255,255);
		putpixel(surface, current_node.x, current_node.y, configuration_space_colors.at(current_node.x).at(current_node.y));
		update();

		//cout << "add current node to closedSet" << endl;

		expand_node(current_node, goal_node);
		openSet.erase(openSet.find(current_node));
		closedSet.insert(current_node);

		if (current_node == goal_node) {
			cout << "arrived at goal node" << endl;
			break;
		}
	}

	cout << "now retrace steps" << endl;

	while (current_node != start_node) {

		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {
			deinit();
			return 0;
		}

		cout << "current:" << current_node.x << ", " << current_node.y << endl;

		configuration_space_colors.at(current_node.x).at(current_node.y).r = 255;
		configuration_space_colors.at(current_node.x).at(current_node.y).g = 0;
		configuration_space_colors.at(current_node.x).at(current_node.y).b = 255;
		putpixel(surface, current_node.x, current_node.y, configuration_space_colors.at(current_node.x).at(current_node.y));
		update();

		cout << "predecessor: " << current_node.predecessor_x << ", " << current_node.predecessor_y << endl;
		current_node = configuration_space_a_star_cells.at(current_node.predecessor_x).at(current_node.predecessor_y);
		cout << endl;
		//getchar();
	}

	configuration_space_colors.at(current_node.x).at(current_node.y).r = 0;
	configuration_space_colors.at(current_node.x).at(current_node.y).g = 0;
	configuration_space_colors.at(current_node.x).at(current_node.y).b = 0;
	putpixel(surface, current_node.x, current_node.y, configuration_space_colors.at(current_node.x).at(current_node.y));
	update();

	while (true) {
		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {
			deinit();
			return 0;
		}
	}
	return 0;
}

void expand_node(A_star_Node current_node, A_star_Node goal_node) {

	randomizeNeighbors();

	for (int i = 0; i < 8; i++) {
		cout << "---" << endl;
		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {
			deinit();
			return;
		}

		if ((current_node.x + DirTable[i][0] >= 0 && current_node.x + DirTable[i][0] < height && current_node.y + DirTable[i][1] >= 0 && current_node.y + DirTable[i][1] < width) &&
				(configuration_space_colors.at(current_node.x + DirTable[i][0]).at(current_node.y + DirTable[i][1]).r ==
				configuration_space_colors.at(current_node.x + DirTable[i][0]).at(current_node.y + DirTable[i][1]).g &&
				configuration_space_colors.at(current_node.x + DirTable[i][0]).at(current_node.y + DirTable[i][1]).g ==
				configuration_space_colors.at(current_node.x + DirTable[i][0]).at(current_node.y + DirTable[i][1]).b)) {
			// neighbor is within limits

			A_star_Node neighbor = configuration_space_a_star_cells.at(current_node.x + DirTable[i][0]).at(current_node.y + DirTable[i][1]);

			cout << "inspecting neighbor at " << "(" << neighbor.x << "," << neighbor.y << ")" << endl;

			neighbor.predecessor_x = current_node.x;
			neighbor.predecessor_y = current_node.y;

			neighbor.g = current_node.g + heuristic(current_node, neighbor);
			neighbor.h = heuristic(neighbor, goal_node);
			neighbor.f = neighbor.g + neighbor.h;

			cout << "f = g + h = " << neighbor.g << " + " << neighbor.h << " = " << neighbor.f << endl;

			auto openSet_iter = openSet.find(neighbor);
			auto closedSet_iter = closedSet.find(neighbor);

			// neighbor is not in closedset
			if (closedSet_iter == closedSet.end()) {
				//neighbor.f = neighbor.g + heuristic(neighbor, goal_node);

				neighbor.g = current_node.g + heuristic(current_node, neighbor);
							neighbor.h = heuristic(neighbor, goal_node);
							neighbor.f = neighbor.g + neighbor.h;

				if (openSet_iter == openSet.end()) {
					openSet.insert(neighbor);
					configuration_space_a_star_cells.at(neighbor.x).at(neighbor.y) = neighbor;
				} else {
					// neighbor has been visited already, now check if the current path is better
					A_star_Node openneighbor = *openSet_iter;
					if (neighbor.f < openneighbor.f) {
						openneighbor.x = neighbor.x;
						openneighbor.y = neighbor.y;
						openneighbor.g = neighbor.g;
						openneighbor.f = neighbor.f;
						openneighbor.predecessor_x = neighbor.predecessor_x;
						openneighbor.predecessor_y = neighbor.predecessor_y;
						configuration_space_a_star_cells.at(openneighbor.x).at(openneighbor.y) = openneighbor;
						openSet.erase(neighbor);
						openSet.insert(openneighbor);
					}
				}
			}
		}
	}
}

double heuristic(A_star_Node start, A_star_Node goal) {
	// euclidian distance
	return std::sqrt(pow(goal.x - start.x, 2.) + pow(goal.y - start.y, 2.));

	// Manhattan distance
	//return abs(goal.x - start.x)+abs(goal.y - start.y);

	// diagonal distance
	// http://theory.stanford.edu/~amitp/GameProgramming/Heuristics.html#speed-or-accuracy
	//double dx = abs(start.x - goal.x);
	//double dy = abs(start.y - goal.y);
	// Chebyshev distance
	//double D = 1;
	//double D2 = 1;
	// octile distance
	//double D = 1;
	//double D2 = std::sqrt(2);
	//return D*(dx+dy)+(D2-2*D)*std::min(dx,dy);

}

int BFS(int nStartX, int nStartY, int nGoalX, int nGoalY) {
	Uint32 starttime = SDL_GetTicks();

	configuration_space_bfs_cells.resize(width, vector<BFS_Cell>(height));

	for (int x = 0; x < surface->w; x++) {
		for (int y = 0; y < surface->h; y++) {
			configuration_space_colors.at(x).at(y) = getpixel(surface, x, y);
			configuration_space_bfs_cells.at(x).at(y).bMarked = false;
		}
	}

	int x, y, length = 0;

	// Pfad suchen
	bool bFound = breadth_first_search(nStartX, nStartY, nGoalX, nGoalY);

	Uint32 dwElapsed = SDL_GetTicks() - starttime;
	starttime = SDL_GetTicks();
	std::cout.clear();
	cout << "Die Suche dauerte: " << dwElapsed << "ms" << endl;
	cout << "Pfad gefunden: " << bFound << endl;

	// überprüfen, ob ein Pfad gefunden wurde
	if (bFound) {
		//update();
		//FILE *fp = fopen("path.prg", "w");
		//if (fp != NULL)
		//fp = stdout;

		// Den Pfad vom Ziel zum Start zurückgehen und den Pfad markieren
		x = nGoalX;
		y = nGoalY;

		BFS_Cell cell;
		while (configuration_space_bfs_cells.at(x).at(y).nLastX != -1) // Bei -1 sind wir am Start
		{
			SDL_PollEvent(&e);
			if (e.type == SDL_QUIT) {

				deinit();

				return 0;
			}

			//cout << "LastX: " << configurations_space_bfs_cells.at(x).at(y).nLastX << endl;
			// EASY-ROB Programm erzeugen
			// Die Gelenkwerte müssen entsprechend den Zellen angepasst werden!!!
			//fprintf(fp, "JUMP_TO_AX %f %f\n", (x - width / 2) * 0.002f, (y - height / 2) * 0.002f); // von -1m bis +1m in 20mm Schritten
			//fprintf(fp, "JUMP_TO_AX %f %f\n", x - width/2.0f, y - width/2.0f); // von -180° bis +180° in 1° Schritten
			// Schwarzen Pixel setzen
			configuration_space_colors.at(x).at(y).r = 0;
			configuration_space_colors.at(x).at(y).g = 0;
			configuration_space_colors.at(x).at(y).b = 0;
			putpixel(surface, x, y, configuration_space_colors.at(x).at(y));
			cell = configuration_space_bfs_cells.at(x).at(y);
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
	return 0;
}

void init() {
	srand(time(NULL));

	//std::cout.setstate(std::ios_base::failbit);

	SDL_Init(SDL_INIT_VIDEO);

	surface = SDL_LoadBMP("cspace.bmp");

	if(surface == NULL) {
		cout << "Could not load bitmap"	<< endl;
		exit(0);
	}

	window = SDL_CreateWindow("2D Pathfinding", SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED, surface->w, surface->h, SDL_WINDOW_SHOWN);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, SIZE, SIZE);

	vector<vector<int> > bmp_array;

	width = surface->w;
	height = surface->h;

	configuration_space_colors.resize(width, vector<SDL_Color>(height));
}

void update() {
	SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);

	//SDL_RenderSetScale(renderer, 2, 2);

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
bool breadth_first_search(int nStartX, int nStartY, int nGoalX, int nGoalY) {
	printf("Suche Pfad von x=%d y=%d nach x=%d y=%d\n", nStartX, nStartY, nGoalX, nGoalY);

	// initalize cells
	for (int my_y = 0; my_y < height; my_y++) {
		for (int my_x = 0; my_x < width; my_x++) {
			configuration_space_bfs_cells[my_x][my_y].bMarked = false;
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
	configuration_space_bfs_cells[nStartX][nStartY].nLastX = -1;

	double start_distance = sqrt(pow(SIZE,2)+pow(SIZE,2));

	// mark starting cell
	configuration_space_bfs_cells[c.x][c.y].bMarked = true;
	double color = 0;
	int iteration_counter = 0;
	// while there's still cells in the queue...
	while (!queue.empty()) {
		// slow down and update rendering
		iteration_counter++;
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
			printf("last: %d %d \n", configuration_space_bfs_cells[c.x][c.y].nLastX, configuration_space_bfs_cells[c.x][c.y].nLastY);
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
				if (configuration_space_bfs_cells.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).bMarked == false) {
					// mark cell
					configuration_space_bfs_cells.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).bMarked = true;

					// if there's no collision in the next cell...
					if (configuration_space_colors.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).r == 255 && configuration_space_colors.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).g == 255 && configuration_space_colors.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).b == 255) {

						// watch the BFS as it happens
						cout << "coloring pixel" << endl;
						configuration_space_colors.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).r = fmod(iteration_counter/start_distance*255,255);
						configuration_space_colors.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).g = fmod(iteration_counter/start_distance*255,255);
						configuration_space_colors.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).b = fmod(iteration_counter/start_distance*255,255);

						//						color += 0.5;
						//						//color=std::fmod(color,255);
						//						if (color > 255)
						//							color = 0;

						putpixel(surface, c.x + DirTable[i][0], c.y + DirTable[i][1], configuration_space_colors.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]));

						update();

						configuration_space_bfs_cells.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).nLastX = c.x;
						configuration_space_bfs_cells.at(c.x + DirTable[i][0]).at(c.y + DirTable[i][1]).nLastY = c.y;
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
			configuration_space_bfs_cells.at(x).at(y).bMarked = false;   // Nicht besucht
			configuration_space_bfs_cells.at(x).at(y).nLastX = -1; // Keine vorherige Zelle
			configuration_space_bfs_cells.at(x).at(y).nLastY = -1;
		}
	}
}

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

// Uninteressant
bool operator<(const Coordinate& c1, const Coordinate& c2) {
	return false;
}

bool operator==(const Coordinate& c1, const Coordinate& c2) {
	return false;
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
