#include <iostream>
#include <vector>
#include <stdlib.h>
#include <random>
#include <Windows.h>

#define BOARD_SIZE 30

using namespace std;

random_device rd;
mt19937 gen(rd());
uniform_int_distribution<int> rnum(4, 9);
uniform_int_distribution<int> rpos(0, BOARD_SIZE-1);

struct Player {
	Player() = default;
	Player(int x, int y)
		: x{ x }, y{ y } {
	}

	int x, y;
};

void move(Player& p, vector<vector<int>>& board, int dx, int dy) {
	int x = p.x + dx;
	int y = p.y + dy;

	if (x > -1 and y > -1 and x < BOARD_SIZE and y < BOARD_SIZE and board[y][x] > -1) {
		p.x = x;
		p.y = y;
		board[y][x]++;
	}
}

int main() {
	vector<vector<int>> board(BOARD_SIZE, vector<int>(BOARD_SIZE, 0));
	Player p{ 0, 0 };
	board[p.y][p.x] = 1;

	int n{ rnum(gen) };
	for (int i{}; i < n; i++) {
		int x{ rpos(gen) }, y{ rpos(gen) };
		board[y][x] = -1;
	}

	while (true) {
		system("cls");

		for (int i{}; i < board.size(); i++) {
			for (int j{}; j < board[i].size(); j++) {
				if (board[i][j] < 0) {
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
					cout << "X ";
				}
				else if (i == p.y and j == p.x) {
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
					cout << "* ";
				}
				else {
					int color = board[i][j] > 0 ? 10 : 7;
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
					cout << board[i][j] << ' ';
				}
			}
			cout << endl;
		}
		cout << endl;

		cout << "cmd: ";
		char cmd;
		cin >> cmd;

		switch (cmd) {
		case 'w':
			move(p, board, 0, -1);
			break;
		case 'a':
			move(p, board, -1, 0);
			break;
		case 's':
			move(p, board, 0, 1);
			break;
		case 'd':
			move(p, board, 1, 0);
			break;
		case 'r':
			board.assign(BOARD_SIZE, vector<int>(BOARD_SIZE, 0));
			p.x = p.y = 0;
			n = rnum(gen);
			for (int i{}; i < n; i++) {
				int x{ rpos(gen) }, y{ rpos(gen) };
				board[y][x] = -1;
			}
			break;
		case 'q':
			return 0;
		}

		cin.clear();
		cin.ignore(1000, '\n');
	}
}