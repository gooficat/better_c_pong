#include <raylib.h>
#include <cmath>
#include <iostream>


const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 360;

//for keeping paddles the same width as the ball
int BLOCK_SCALING = 24;

float PADDLE_SPEED = 400.0f;
float BALL_SPEED = 800.0f;

//how much the speed should be multiplied by on paddle hit
float PADDLE_MULTIPLE = 1.2f;

int BALL_STARTX = SCREEN_WIDTH/2 - BLOCK_SCALING/2;
int BALL_STARTY = SCREEN_HEIGHT/2 - BLOCK_SCALING/2;

int leftScore = 0;
int rightScore = 0;


bool RESET = false;

int clamp_i(int in, int min, int max) {
	if(in > max) return max;
	if(in < min) return min;
	return in;
}

void rotateVector_f(float* x, float* y, float angle) {
	float sinTheta = std::sin(angle);
	float cosTheta = std::cos(angle);
	float newX = *x * cosTheta - *y * sinTheta;
	*y = *x * sinTheta + *y * cosTheta;
	*x = newX;
} 

class Paddle {
public:
	float x, y;
	int width, height;
	int UP_KEY, DOWN_KEY;
	Paddle(int x, int y, int width, int height, int UP_KEY, int DOWN_KEY):
			x(x), y(y), width(width), height(height), UP_KEY(UP_KEY), DOWN_KEY(DOWN_KEY) {}
	void draw() {
		DrawRectangle(x, y, width, height, RAYWHITE);
	}
	void update(float deltaTime) {
		if(IsKeyDown(UP_KEY))
			y -= PADDLE_SPEED * deltaTime;
		if(IsKeyDown(DOWN_KEY))
			y += PADDLE_SPEED * deltaTime;
		y = clamp_i(y, 0, SCREEN_HEIGHT - height);
	}
};

class Ball {
public:
	float x, y;
	float size;
	float x_velocity;
	float y_velocity;
	float speed;
	Ball(int size, int speed) :
		x(BALL_STARTX), y(BALL_STARTY), size(size), speed(speed) {
			x_velocity = speed/2;
		}
	void draw() {
		DrawRectangle(x, y, size, size, RAYWHITE);
	}
	void checkCourt() {
		if (x + size > SCREEN_WIDTH) {
			RESET = true;
			leftScore++;
		}else if (x < 0) {
			RESET = true;
			rightScore++;
		}
		if (y + size > SCREEN_HEIGHT) {
			y_velocity *= -1;
			y = SCREEN_HEIGHT - size;
		}
		else if (y < 0) {
			y_velocity *= -1;
			y = 0;
		}
	}
	void update(float deltaTime) {
		x += x_velocity * deltaTime;
		y += y_velocity * deltaTime;
	}
	void checkPaddle(Paddle& paddle) {
		if (x + size > paddle.x and x < paddle.x + paddle.width and y + size > paddle.y and y < paddle.y + paddle.height) {
			float normal = std::atan2f(x + size/2 - paddle.x + paddle.width/2, y + size/2 - paddle.y + paddle.height/2);
			float velocity_angle = std::atan2f(x_velocity, y_velocity);  
			rotateVector_f(&x_velocity, &y_velocity, -velocity_angle);
			rotateVector_f(&x_velocity, &y_velocity, normal * PADDLE_MULTIPLE);
			std::cout << "bonk" << std::endl;
			
		}
	}
};

int main() {
	SetConfigFlags(FLAG_VSYNC_HINT);
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong!");
	float deltaTime;
	
	Paddle leftPaddle(0, SCREEN_HEIGHT/2, BLOCK_SCALING, SCREEN_HEIGHT/4, 87, 83);
	Paddle rightPaddle(SCREEN_WIDTH-BLOCK_SCALING, SCREEN_HEIGHT/2, BLOCK_SCALING, SCREEN_HEIGHT/4, 73, 75);
	
	Ball ball(BLOCK_SCALING, BALL_SPEED);
	
	while(!WindowShouldClose()) {
		//keep speed constant regardless of fps
		deltaTime = 0.0167f;
		
		leftPaddle.update(deltaTime);
		rightPaddle.update(deltaTime);
		
		ball.update(deltaTime);
		ball.checkPaddle(leftPaddle);
		ball.checkPaddle(rightPaddle);
		ball.checkCourt();
		
		
		//if the court resets, put everything back to home positions
		if (RESET) {
			ball.x = BALL_STARTX;
			ball.y = BALL_STARTY;
			ball.x_velocity = BALL_SPEED/2;
			ball.y_velocity = 0;
			leftPaddle.y = SCREEN_HEIGHT/2;
			rightPaddle.y = SCREEN_HEIGHT/2;
			RESET = false;
		}
		
		BeginDrawing();
			//two-tone background
			DrawRectangle(0, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT, SKYBLUE);
			DrawRectangle(SCREEN_WIDTH/2, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT, GRAY);
			
			ball.draw();
			//draw paddles
			leftPaddle.draw();
			rightPaddle.draw();
			
			//draw scores
			DrawText(TextFormat("%d", leftScore), SCREEN_WIDTH/4, SCREEN_HEIGHT/12, 48, RAYWHITE);
			DrawText(TextFormat("%d", rightScore), 3*SCREEN_WIDTH/4, SCREEN_HEIGHT/12, 48, RAYWHITE);
		EndDrawing();
	}
}
