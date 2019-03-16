// Snake2.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"
#include <iostream>
#include <conio.h>
#include <string>
#include <Windows.h>

using namespace std;

class Snake {
#define WALL -1
#define PLAYER 1
#define TAIL 2
#define DEAD -9
#define FRUIT 9

public:
	Snake() : width(15), height(15) {
		if (width <= 5 || height <= 5) exit(-1);
		init();
	}
	~Snake() {
		for (int i = 0; i < height; i++) {
			delete[] map[i];
		}
		delete[] map;

		delete[] tailX;
		delete[] tailY;
	}
	void init() {
		RemoveCursor();
		isGameOver = false;
		dir = STOP;
		playerPosX = width / 2;
		playerPosY = height / 2;
		score = 0;
		
		fruitPosX = (rand() % (width - 2)) + 1;
		fruitPosY = (rand() % (height - 2)) + 1;

		
		mapInit();
		tailGenerator();
		space = (width - 2) * (height - 2) - 2;
		isHalfFinished = false;
	}
	void tailGenerator() {
		tailX = new int[width*height - 2];
		tailY = new int[height*width - 2];

		for (int i = 0; i < width*height-2; i++) tailX[i] = 0;
		for (int i = 0; i < height*width-2; i++) tailY[i] = 0;
	}
	void mapInit() {
		//map 동적할당
		map = new int*[height];
		for (int i = 0; i < height; i++) {
			map[i] = new int[width];

			for (int j = 0; j < width; j++) {
				map[i][j] = 0;
			}
		}
		//map 정보 초기화
		for (int i = 0; i < width; i++) map[0][i] = WALL;
		for (int i = 1; i < height - 1; i++) {
			map[i][0] = WALL;
			map[i][width - 1] = WALL;
		}
		for (int i = 0; i < width; i++) map[height - 1][i] = WALL;

		map[playerPosY][playerPosX] = PLAYER;
		map[fruitPosY][fruitPosX] = FRUIT;

		// draw 버퍼 초기화
		buffer = "";
	}
	void RemoveCursor(void)
	{
		CONSOLE_CURSOR_INFO curInfo;
		GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
		curInfo.bVisible = 0; // bVisible 멤버 변경
		SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo); // 변경값 적용
	}
	void randomizeFruitPos() {
		//player가 맵을 절반정도 점유하면 다음
		int* tempArr = new int[space];

		/*
		if (isHalfFinished) {
			int k = 0;
			for (int i = 0; i < height; i++) {
				for (int j = 0; j < width; j++) {
					if (map[i][j] == 0) *(tempArr+k) = map[i][j];
					k++;
				}
			}

			tempArr[rand() % space] = FRUIT;

			delete[] tempArr;
			space--;
		}
		
		*/
			//if (space < (width - 2)*(height - 2) / 2) isHalfFinished = true;
			fruitPosX = (rand() % (width - 2)) + 1;
			fruitPosY = (rand() % (height - 2)) + 1;
			
			while (map[fruitPosY][fruitPosX] != 0) {
				fruitPosX = (rand() % (width - 2)) + 1;
				fruitPosY = (rand() % (height - 2)) + 1;
			}

			map[fruitPosY][fruitPosX] = FRUIT;
			/*
			if (map[fruitPosY][fruitPosX] != 0) {
				randomizeFruitPos();
			}*/


	}
	void draw() {
		system("cls");
		buffer = "";
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				if (map[i][j] == WALL) buffer += "▩";
				else if (map[i][j] == PLAYER) buffer += "■";
				else if (map[i][j] == DEAD) buffer += "GG";
				else if (map[i][j] == FRUIT) buffer += "★";
				else if (map[i][j] == TAIL) buffer += "□";
				else buffer += "  ";
			}
			buffer += "\n";
		}
		cout << buffer << endl;
	}
	void keyInput() {
		if (_kbhit()) {
			switch (_getch()) {
			case 'a':
				if (dir == RIGHT) break;
				dir = LEFT;
				break;
			case 'd':
				if (dir == LEFT) break;
				dir = RIGHT;
				break;
			case 'w':
				if (dir == DOWN) break;
				dir = UP;
				break;
			case 's':
				if (dir == UP) break;
				dir = DOWN;
				break;
			case 'x':
				isGameOver = true;
				break;
			}
		}
	}
	void gameOver() {
		isGameOver = true;
		map[playerPosY][playerPosX] = DEAD;
	}
	void gameManager() {
		// 벽에 닿으면 gameOver
		if (playerPosX <= 0 || playerPosX >= width - 1 || playerPosY <= 0 || playerPosY >= height - 1) gameOver();
		
		//내꼬리에 닿으면 gameOver
		if (nTail > 3) {
			for (int i = 0; i < nTail; i++) {
				if (tailX[i] == playerPosX && tailY[i] == playerPosY) gameOver();
			}
		}

		//과일 먹으면
		if (playerPosX == fruitPosX && playerPosY == fruitPosY) {
			randomizeFruitPos();
			nTail++;
			score += 100;
		}
	}
	void playerlogic() {
		int prevTailX = tailX[0];
		int prevTailY = tailY[0];
		int prev2X, prev2Y = 0;

		tailX[0] = playerPosX;
		tailY[0] = playerPosY;

		for (int i = 1; i < nTail; i++) {
			
			prev2X = tailX[i];
			prev2Y = tailY[i];
			tailX[i] = prevTailX;
			tailY[i] = prevTailY;
			prevTailX = prev2X;
			prevTailY = prev2Y;
			
			map[tailY[i]][tailX[i]] = PLAYER;
		}
		
		switch (dir) {
		case LEFT:
			if(nTail == 0) map[playerPosY][playerPosX] = 0;
			else {
				if (map[tailY[nTail - 1]][tailX[nTail - 1]] == FRUIT) break;
				map[tailY[nTail - 1]][tailX[nTail - 1]] = 0;
			}
			map[playerPosY][--playerPosX] = PLAYER;
			break;
		case RIGHT:
			if (nTail == 0) map[playerPosY][playerPosX] = 0;
			else {
				if (map[tailY[nTail - 1]][tailX[nTail - 1]] == FRUIT) break;
				map[tailY[nTail - 1]][tailX[nTail - 1]] = 0;
			}
			map[playerPosY][++playerPosX] = PLAYER;
			break;
		case UP:
			if (nTail == 0) map[playerPosY][playerPosX] = 0;
			else {
				if (map[tailY[nTail - 1]][tailX[nTail - 1]] == FRUIT) break;
				map[tailY[nTail - 1]][tailX[nTail - 1]] = 0;
			}
			map[--playerPosY][playerPosX] = PLAYER;
			break;
		case DOWN:
			if (nTail == 0) map[playerPosY][playerPosX] = 0;
			else {
				if (map[tailY[nTail - 1]][tailX[nTail - 1]] == FRUIT) break;
				map[tailY[nTail - 1]][tailX[nTail - 1]] = 0;
			}
			map[++playerPosY][playerPosX] = PLAYER;
			break;
		}
	}

	void startGame() {
		while (!isGameOver) {
			keyInput();
			playerlogic();
			gameManager();
			draw();
			cout << "==================" << endl;
			cout << playerPosX << " " << playerPosY << endl;
			cout << "fruitPosS" << fruitPosX << fruitPosY << endl;
			Sleep(66.666);
		}
	}

private:
	int** map;
	string buffer;
	bool isHalfFinished;
	int space;
	enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN} dir;
	bool isGameOver;
	const int width;
	const int height;
	int playerPosX;
	int playerPosY;
	int fruitPosX;
	int fruitPosY;
	int score;
	int* tailX;
	int* tailY;
	int nTail = 1;
};
int main()
{
	Snake snake;
	snake.startGame();
}