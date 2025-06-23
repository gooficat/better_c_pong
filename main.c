#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdbool.h>

#define WIDTH 640
#define HEIGHT 480
#define GRID 32
#define PADDLE_SPEED 500
#define PADDLE_POWER 50


typedef struct {
	int x, y;
	int width, height;
	int dy;
} paddle_t;

typedef struct {
	int x, y;
	int width, height;
	bool reset;
	int speed;
	int dx, dy;
} ball_t;

bool colliding(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
	if (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2)
		return true;
	else return false;
}

int clamp(int value, int min, int max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

int main() {
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* window = SDL_CreateWindow("pong!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,  SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);


	paddle_t left_paddle = {
		.x = 0,
		.y = HEIGHT/3,
		.width = WIDTH/GRID,
		.height = HEIGHT/3
	};
	paddle_t right_paddle = {
		.x = (GRID-1) * (WIDTH/GRID),
		.y = HEIGHT/3,
		.width = WIDTH/GRID,
		.height = HEIGHT/3
	};
	
	ball_t ball = {
		.x = WIDTH/2,
		.y = HEIGHT/2,
		.width = WIDTH/GRID,
		.height = WIDTH/GRID,
		.reset = false,
		.speed = 300,
		.dx = 1,
		.dy = 1
	};
	
	Uint32 lastFrameTime = SDL_GetTicks();
	float deltaTime;
	SDL_Event e;
	bool running = true;
	while (running) {
		Uint32 currentTime = SDL_GetTicks();
		deltaTime = (currentTime - lastFrameTime) / 1000.0f;;
		lastFrameTime = currentTime;
		while (SDL_PollEvent(&e)) {

			
			if (e.type == SDL_QUIT) running = 0;
			else if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym == SDLK_w)
					left_paddle.dy = -PADDLE_SPEED;
				if (e.key.keysym.sym == SDLK_s)
					left_paddle.dy = PADDLE_SPEED;
				if (e.key.keysym.sym == SDLK_UP)
					right_paddle.dy = -PADDLE_SPEED;
				if (e.key.keysym.sym == SDLK_DOWN)
					right_paddle.dy = PADDLE_SPEED;
			}
			else if (e.type == SDL_KEYUP) {
				if (e.key.keysym.sym == SDLK_w || e.key.keysym.sym == SDLK_s)
					left_paddle.dy = 0;
				if (e.key.keysym.sym == SDLK_UP || e.key.keysym.sym == SDLK_DOWN)
					right_paddle.dy = 0;
			}
		}
		
		left_paddle.y += left_paddle.dy * deltaTime;
		right_paddle.y += right_paddle.dy * deltaTime;
		
		left_paddle.y = clamp(left_paddle.y, 0, 2*HEIGHT/3);
		right_paddle.y = clamp(right_paddle.y, 0, 2*HEIGHT/3);
		
		ball.x += ball.dx * deltaTime * ball.speed;
		ball.y += ball.dy * deltaTime * ball.speed;

		if ((ball.x < 0 || ball.x > WIDTH) && !ball.reset) {
			ball.reset = true;
		}
		
		if ((ball.y <= 0 || ball.y + ball.height >= HEIGHT)) {
			ball.y = clamp(ball.y, 0, HEIGHT - ball.height);
			ball.dy *= -1;
		}
	
		if (colliding(ball.x, ball.y, ball.width, ball.height, left_paddle.x, left_paddle.y, left_paddle.width, left_paddle.height)) {
			ball.dx *= -1;
			ball.x = left_paddle.width;
			if (ball.speed < 500)
				ball.speed += PADDLE_POWER;
		}
		
		if (colliding(ball.x, ball.y, ball.width, ball.height, right_paddle.x, right_paddle.y, right_paddle.width, right_paddle.height)) {
			ball.dx *= -1;
			ball.x = right_paddle.x - right_paddle.width;
			
		}
		
		if (ball.reset) {
			ball.x = WIDTH / 2;
			ball.y = HEIGHT / 2;
			ball.dx = -ball.dx;
			ball.reset = false;
		}
		
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		
		SDL_RenderDrawLine(renderer, WIDTH/2, 0, WIDTH/2, HEIGHT);
		
		SDL_Rect rect = {left_paddle.x, left_paddle.y, left_paddle.width, left_paddle.height};
		SDL_RenderFillRect(renderer, &rect);
		
		rect = (SDL_Rect){right_paddle.x, right_paddle.y, right_paddle.width, right_paddle.height};
		SDL_RenderFillRect(renderer, &rect);
		
		rect = (SDL_Rect){ball.x, ball.y, ball.width, ball.height};
		SDL_RenderFillRect(renderer, &rect);
		
		SDL_RenderPresent(renderer);
	}
	
	SDL_Quit();
	return 0;
}
