#include <iostream>
#include <stdlib.h>
#include <vector>
#include <random>
#include <Windows.h>

#define BOARD_SIZE 5

using namespace std;

random_device rd;
mt19937 gen(rd());
uniform_int_distribution<int> rnum(0, BOARD_SIZE-1);

struct Board {
	Board() = default;
	Board(int Alpha, bool open)
		: Alpha{ Alpha }, open{ open } {
	}

	int Alpha;
	bool open;
};

int main() {
	vector<vector<Board>> board;
	board.assign(5, vector<Board>(5, Board(0, false)));

	int num{ 0 };
	for (int i{0}; i < 13; i++) {
		for (int j{}; j < 2; j++) {
			int x{ rnum(gen) }, y{ rnum(gen) };
			if (board[y][x].Alpha == 0) {
				board[y][x].Alpha += num;
				if (num == 12) break;
			}
			else {
				j--;
			}
		}
		num++;
	}

	while (true) {
		system("cls");

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		cout << "    a b c d e" << endl << endl;

		for (int i{}; i < board.size(); i++) {
			for (int j{}; j < board[i].size(); j++) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
				if (j == 0) {
					cout << i + 1 << "   ";
				}

				if (board[i][j].open) {
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), board[i][j].Alpha+1);
					if (board[i][j].Alpha == 12)
						cout << "@ ";
					else
						cout << (char)('A' + board[i][j].Alpha) << ' ';
				}
				else {
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
					cout << "* ";
				}
			}
			cout << endl;
		}
		cout << endl;
		
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		cout << "i: 셀입력, r: 리셋, h: 잠시 보기, q:종료" << endl;
		cout << "cmd: ";
		char cmd;
		cin >> cmd;

		switch (cmd) {
		case 'i':
			cout << "셀 입력 ex) a3 b4: ";
			int x1, y1, x2, y2;
			char c1, c2;
			cin >> c1 >> y1 >> c2 >> y2;

			y1--; y2--;
			x1 = (int)(c1 - 'a');
			x2 = (int)(c2 - 'a');

			if (x1 < 0 or x2 < 0 or y1 < 0 or y2 < 0 or
				x1 >= BOARD_SIZE or x2 >= BOARD_SIZE or y1 >= BOARD_SIZE or y2 >= BOARD_SIZE) {
				cout << "잘못된 인덱스 입력" << endl;
				Sleep(2000);
				break;
			}

			system("cls");

			if (board[y1][x1].Alpha == board[y2][x2].Alpha) {
				board[y1][x1].open = true;
				board[y2][x2].open = true;
			}
			else if (board[y1][x1].Alpha == 13 or board[y2][x2].Alpha == 13) {
				board[y1][x1].open = true;
				board[y2][x2].open = true;
				int find_num = board[y1][x1].Alpha + board[y2][x2].Alpha - 13;
				for (int i{}; i < board.size(); i++) {
					for (int j{}; j < board[i].size(); j++) {
						if (find_num == board[i][j].Alpha) {
							board[i][j].open = true;
							find_num = -1;
							break;
						}
					}
					if (find_num < 0) break;
				}
			}

			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
			cout << "    a b c d e" << endl << endl;
			for (int i{}; i < board.size(); i++) {
				for (int j{}; j < board[i].size(); j++) {
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
					if (j == 0) {
						cout << i + 1 << "   ";
					}

					if (board[i][j].open) {
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), board[i][j].Alpha + 1);
						if (board[i][j].Alpha == 12)
							cout << "@ ";
						else
							cout << (char)('A' + board[i][j].Alpha) << ' ';
					}
					else {
						if ((i == y1 and j == x1) or (i == y2 and j == x2)) {
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), board[i][j].Alpha + 1);
							cout << (char)('a' + board[i][j].Alpha) << ' ';
						}
						else {
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
							cout << "* ";
						}
					}
				}
				cout << endl;
			}
			cout << endl;

			Sleep(2000);
			break;
		case 'r':
			board.assign(5, vector<Board>(5, Board(0, false)));
			num={ 0 };
			for (int i{ 0 }; i < 13; i++) {
				for (int j{}; j < 2; j++) {
					int x{ rnum(gen) }, y{ rnum(gen) };
					if (board[y][x].Alpha == 0) {
						board[y][x].Alpha += num;
						if (num == 12) break;
					}
					else {
						j--;
					}
				}
				num++;
			}
			break;
		case 'h':
			system("cls");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
			cout << "    a b c d e" << endl << endl;
			for (int i{}; i < board.size(); i++) {
				for (int j{}; j < board[i].size(); j++) {
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
					if (j == 0) {
						cout << i + 1 << "   ";
					}

					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), board[i][j].Alpha + 1);
					if (board[i][j].Alpha == 12)
						cout << "@ ";
					else
						cout << (char)('A' + board[i][j].Alpha) << ' ';
				}
				cout << endl;
			}
			cout << endl;

			Sleep(3000);
			break;
		case 'q':
			return 0;
		}

		cin.clear();
		cin.ignore(1000, '\n');
	}
}