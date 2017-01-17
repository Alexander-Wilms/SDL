class Particle {
public:
	Particle(SDL_Renderer*,int,int,double, double);
	~Particle();
	void render();
private:
	float alpha;
	float realx;
	float realy;
	Uint32 timestamp_creation;
	SDL_Surface* surface;
	SDL_Texture* texture;
	SDL_Rect rect;
	SDL_Renderer* renderer;
	float x_velocity;
	float y_velocity;
};
