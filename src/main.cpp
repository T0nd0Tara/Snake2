#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define TARA_PGE_EXTENSION
#include <taraNS.h>

#include <iostream>
#include <cassert>
#include <map>
#include <time.h>

enum Dir {
	Up, Left, Down, Right
};
class Snake : public olc::PixelGameEngine
{
public:
	Snake() {
		sAppName = "Snake";
	}

private:
	olc::Decal* dFullTex;

	olc::vi2d GridSize{ 16,16 }, GridCellSize;
	std::vector<olc::vi2d> SnakeCords;
	olc::vi2d AppleCords;

	std::map<Dir, std::map<Dir, olc::vi2d>> SnakeBody;
	std::map<Dir, olc::vi2d> SnakeHead, SnakeTail;
	olc::vi2d Apple;

	Dir dir, new_dir;
	float frameTime, frameCounter;
	uint32_t score;

	void newApple() {
		do {
			AppleCords = olc::vi2d{rand() % GridSize.x, rand() % GridSize.y};
		} while (is_in(SnakeCords, AppleCords));
	}

	void DrawSnake() {
		Dir d1, d2;
		// Drawing Head
		olc::vi2d temp = SnakeCords[1] - SnakeCords[0];
		if (temp == olc::vi2d{ 0,1 }) d1 = Up; // Up
		else if (temp == olc::vi2d{ 0,-1 }) d1 = Down; // Down
		else if (temp == olc::vi2d{ 1,0 }) d1 = Left; // Left
		else if (temp == olc::vi2d{ -1,0 }) d1 = Right; // Right
		else assert(false && "Gap Between Head and Body");
		DrawPartialDecal(GridCellSize * SnakeCords[0], GridCellSize, dFullTex, SnakeHead[d1], { 64,64 });

		// DrawingBody
		for (uint32_t i = 1; i < SnakeCords.size() - 1; i++) {
			
			temp = SnakeCords[i] - SnakeCords[i-1];
			if (temp == olc::vi2d{ 0,1 }) d1 = Up;
			else if (temp == olc::vi2d{ 0, -1 }) d1 = Down;
			else if (temp == olc::vi2d{ 1, 0 }) d1 = Left;
			else if (temp == olc::vi2d{ -1,0 }) d1 = Right;
			else assert(false && "Gap in Body");

			temp =  SnakeCords[i] - SnakeCords[i + 1];
			if (temp == olc::vi2d{ 0,1 }) d2 = Up;
			else if (temp == olc::vi2d{ 0, -1 }) d2 = Down;
			else if (temp == olc::vi2d{ 1, 0 }) d2 = Left;
			else if (temp == olc::vi2d{ -1,0 }) d2 = Right;
			else assert(false && "Gap in Body");

			if (GetKey(olc::SPACE).bHeld) std::cout << "dir of part " << i << ", is: " << d1 << ", " << d2 << '\n';
			DrawPartialDecal(GridCellSize * SnakeCords[i], GridCellSize, dFullTex, SnakeBody[d1][d2], { 64,64 });
		}
		// Drawing Head
		temp = SnakeCords[SnakeCords.size() - 1] - SnakeCords[SnakeCords.size() - 2];
		if (temp == olc::vi2d{ 0,1 }) d1 = Up; // Up
		else if (temp == olc::vi2d{ 0,-1 }) d1 = Down; // Down
		else if (temp == olc::vi2d{ -1,0 }) d1 = Left; // Left
		else if (temp == olc::vi2d{ 1,0 }) d1 = Right; // Right
		else assert(false && "Gap Between Body and Tail");
		DrawPartialDecal(GridCellSize * SnakeCords.back(), GridCellSize, dFullTex, SnakeTail[d1], { 64,64 });
	}

	void DrawApple() {
		DrawPartialDecal(GridCellSize * AppleCords, GridCellSize, dFullTex, Apple, { 64,64 });
	}
protected:
	bool OnUserCreate() override {
		srand(time(NULL));
		
#pragma region Setting Up Sprites
		olc::Sprite* sFullTex = new olc::Sprite("snake-graphics.png");
		dFullTex = new olc::Decal(sFullTex);

		SnakeHead[Up] = olc::vi2d{ 3, 0 } *64;
		SnakeHead[Down] = olc::vi2d{ 4, 1 } *64;
		SnakeHead[Left] = olc::vi2d{ 3, 1 } *64;
		SnakeHead[Right] = olc::vi2d{ 4, 0 } *64;

		SnakeTail[Up] = olc::vi2d{ 3, 2 } *64;
		SnakeTail[Down] = olc::vi2d{ 4, 3 } *64;
		SnakeTail[Right] = olc::vi2d{ 3, 3 } *64;
		SnakeTail[Left] = olc::vi2d{ 4, 2 } *64;

		SnakeBody[Left][Right] = SnakeBody[Right][Left] = olc::vi2d{ 1, 0 } *64;
		SnakeBody[Up][Down] = SnakeBody[Down][Up] = olc::vi2d{ 2, 1 } *64;

		SnakeBody[Left][Up] = SnakeBody[Up][Left] = olc::vi2d{ 2, 2 } *64;
		SnakeBody[Right][Up] = SnakeBody[Up][Right] = olc::vi2d{ 0, 1 } *64;
		SnakeBody[Left][Down] = SnakeBody[Down][Left] = olc::vi2d{ 2, 0 } *64;
		SnakeBody[Right][Down] = SnakeBody[Down][Right] = olc::vi2d{ 0, 0 } *64;

		Apple = olc::vi2d{ 0, 3 } *64;
#pragma endregion

		GridCellSize = { ScreenWidth() / GridSize.x, ScreenHeight() / GridSize.y };
		GridCellSize = { min(GridCellSize), min(GridCellSize) }; // making sure the Grid is square 
																 //(I'm not using regilar uint32_t so it would be easier coding later)

		olc::vi2d MidGrid = { GridSize.x >> 1, GridSize.y >> 1 };
		SnakeCords = {
			MidGrid,
			MidGrid + olc::vi2d{0,1},
			MidGrid + olc::vi2d{0,2} };

		dir = Up;
		new_dir = Up;

		frameTime = 0.25f;
		frameCounter = 0.0f;

		score = 0;

		return true;
	}

	bool OnUserUpdate(float elapsedTime) override {
		// Input
		if (GetKey(olc::ESCAPE).bPressed) return false;

		if (GetKey(olc::W).bPressed || GetKey(olc::UP).bPressed) new_dir = Up;
		if (GetKey(olc::A).bPressed || GetKey(olc::LEFT).bPressed) new_dir = Left;
		if (GetKey(olc::S).bPressed || GetKey(olc::DOWN).bPressed) new_dir = Down;
		if (GetKey(olc::D).bPressed || GetKey(olc::RIGHT).bPressed) new_dir = Right;

		Clear(olc::BLACK);
		DrawSnake();
		DrawApple();

		// Making it choppy
		frameCounter += elapsedTime;
		if (frameCounter >= frameTime) {
			do {
				frameCounter -= frameTime;
			} while (frameCounter >= frameTime);
		}
		else return true;

		// Making sure you can't press down after up (making the snake collide into his neck)
		if ((new_dir + 2) % 4 != dir) dir = new_dir;

		switch (dir) {
		case Up:
			SnakeCords.insert(SnakeCords.begin(), SnakeCords[0] + olc::vi2d{0,-1});
			break;
		case Left:
			SnakeCords.insert(SnakeCords.begin(), SnakeCords[0] + olc::vi2d{ -1,0});
			break;
		case Down:
			SnakeCords.insert(SnakeCords.begin(), SnakeCords[0] + olc::vi2d{ 0,1 });
			break;
		case Right:
			SnakeCords.insert(SnakeCords.begin(), SnakeCords[0] + olc::vi2d{ 1,0 });
			break;
		}
		if (SnakeCords[0] == AppleCords) {
			newApple();
			frameTime *= 0.99f;
			score++;
			std::cout << "Score: " << score << '\n';
		}
		else SnakeCords.pop_back();

		// check hit self
		if (is_in(SnakeCords, SnakeCords[0], 1)) {
			std::cout << "You hit yourself!\nFinished with score: " << score << '\n';
			return false;
		}

		// check hit edges
		if (SnakeCords[0].x < 0 || SnakeCords[0].x >= GridSize.x ||
		    SnakeCords[0].y < 0 || SnakeCords[0].y >= GridSize.y) {
			std::cout << "You hit the edge!\nFinished with score: " << score << '\n';
			return false;
		}

		return true;
	}
};

int main() {
	Snake demo;
	if (demo.Construct(256, 240, 4, 4))
		demo.Start();
	return 0;
}