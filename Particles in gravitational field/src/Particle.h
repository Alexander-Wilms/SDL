class Particle {
public:
		Particle(SDL_Renderer*, int, int, double, double, SDL_Surface*,
			float, float, int);
		~Particle();
		SDL_Rect* getRect();
		void render();
		bool isFaded();
	private:
		bool faded;
		float alpha;
		float realx;
		float realy;
		Uint32 timestamp_creation;
		SDL_Surface* surface;
		SDL_Surface* screen_surface;
		SDL_Surface* png_surface;
		SDL_Texture* texture;
		SDL_Rect rect;
		SDL_Renderer* renderer;
		float x_velocity;
		float y_velocity;
	}
	;
