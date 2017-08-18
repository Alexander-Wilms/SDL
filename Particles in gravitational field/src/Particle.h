class Particle {
public:
		Particle(SDL_Renderer*, int, int, double, double, SDL_Surface*,
			double, double, int);
		~Particle();
		SDL_Rect* getRect();
		void render();
		bool isFaded();
	private:
		double SafeAcos (double);
		double G;
		double ownMass;
		double CenterMass;
		bool faded;
		double alpha;
		Uint32 timestamp_creation;
		SDL_Surface* surface;
		SDL_Surface* screen_surface;
		SDL_Surface* png_surface;
		SDL_Texture* texture;
		SDL_Rect rect;
		SDL_Renderer* renderer;
		double x_velocity;
		double y_velocity;
		double r;
		double F;
		double a;
		double angle_of_acting_force;
		double ax;
		double ay;
	};
