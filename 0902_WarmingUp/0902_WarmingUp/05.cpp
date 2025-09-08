#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <Windows.h>
#include <vector>

#define BOARD_SIZE 30

using namespace std;

struct Rect {
	Rect() = default;
	Rect(int x1, int x2, int y1, int y2)
		: x1{ x1 }, x2{ x2 }, y1{ y1 }, y2{ y2 } {
	};

	int Rect_size() {
		return (x2 - x1) * (y2 - y1);
	}

	int x1, x2, y1, y2;
};

int main()
{
	vector<vector<int>> board(30, vector<int>(30, 0));
	int add_board{};

	int x1, x2, y1, y2;
	cout << "»ï°¢Çü1 ÁÂÇ¥ ÀÔ·Â(ex. 3 3 8 8): ";
	cin >> x1 >> y1 >> x2 >> y2;
	Rect rect1(x1, x2+1, y1, y2+1);

	cout << "»ï°¢Çü2 ÁÂÇ¥ ÀÔ·Â(ex. 3 3 8 8): ";
	cin >> x1 >> y1 >> x2 >> y2;
	Rect rect2(x1, x2+1, y1, y2+1);


	while (true) {
		system("cls");

		for (int i{}; i < rect1.x2 - rect1.x1; i++) {
			for (int j{}; j < rect1.y2 - rect1.y1; j++) {

				if (rect1.x1 + i > -1 and rect1.x1 + i < board.size() and rect1.y1 + j > -1 and rect1.y1 + j < board[i].size()) {
					board[rect1.x1 + i][rect1.y1 + j] += 1;
				}
				else {
					int rx{}, ry{};
					if (rect1.x1 + i < 0) rx = rect1.x1 + i + (BOARD_SIZE + add_board - 1);
					else if (rect1.x1 + i >= BOARD_SIZE + add_board) rx = rect1.x1 + i - (BOARD_SIZE + add_board);

					if (rect1.y1 + j < 0) ry = rect1.y1 + j + (BOARD_SIZE + add_board - 1);
					else if (rect1.y1 + j >= BOARD_SIZE + add_board) ry = rect1.y1 + j - (BOARD_SIZE + add_board);

					board[rx][ry] += 1;
				}
			}
		}
		for (int i{}; i < rect2.x2 - rect2.x1; i++) {
			for (int j{}; j < rect2.y2 - rect2.y1; j++) {
				

				/*if (rect2.x1 + i < board.size() and rect2.y1 + j < board[i].size()) {
					board[rect2.x1 + i][rect2.y1 + j] += 2;
				}
				else {
					int rx{}, ry{};
					if (rect2.x1 + i < 0) rx = rect2.x1 + i + (BOARD_SIZE + add_board);
					else if (rect2.x1 + i >= BOARD_SIZE + add_board) rx = rect2.x1 + i - (BOARD_SIZE + add_board);

					if (rect2.y1 + j < 0) ry = rect2.y1 + j + (BOARD_SIZE + add_board);
					else if (rect2.y1 + j >= BOARD_SIZE + add_board) ry = rect2.y1 + j - (BOARD_SIZE + add_board);

					board[rx][ry] += 2;
				}*/
			}
		}

		for (int i{}; i < board.size(); i++) {
			for (int j{}; j < board[i].size(); j++) {
				if (board[i][j] == 0)
					cout << ". ";
				else if (board[i][j] == 1)
					cout << "0 ";
				else if (board[i][j] == 2)
					cout << "X ";
				else if (board[i][j] == 3)
					cout << "@ ";
			}
			cout << endl;
		}
		cout << endl;

		cout << "cmd: ";
		char cmd{};
		cin >> cmd;

		switch (cmd) {
		case 'x':
		case 'X':
			if (islower(cmd)) {
				rect1.x1--;
				rect1.x2--;
			}
			else {
				rect1.x1++;
				rect1.x2++;
			}
			break;
		case 'y':
		case 'Y':
			if (islower(cmd)) {
				rect1.y1--;
				rect1.y2--;
			}
			else {
				rect1.y1++;
				rect1.y2++;
			}
			break;
		case 's':
		case 'S':
			if (islower(cmd)) {
				if (rect1.x2 - rect1.x1 > 1 and rect1.y2 - rect1.y1 > 1) {
					rect1.x2--;
					rect1.y2--;
				}
			}
			else {
				if (rect1.x2 - rect1.x1 < BOARD_SIZE + add_board and rect1.y2 - rect1.y1 < BOARD_SIZE + add_board) {
					rect1.x2++;
					rect1.y2++;
				}
			}
			break;
		case 'i':
		case 'I':
			if (islower(cmd)) {
				if (rect1.x2 - rect1.x1 > 1)
					rect1.x2--;
			}
			else {
				if (rect1.x2 - rect1.x1 < BOARD_SIZE + add_board)
					rect1.x2++;
			}
			break;
		case 'j':
		case 'J':
			if (islower(cmd)) {
				if (rect1.y2 - rect1.y1 > 1)
					rect1.y2--;
			}
			else {
				if (rect1.y2 - rect1.y1 < BOARD_SIZE + add_board)
					rect1.y2++;
			}
			break;
		case 'a':
		case 'A':
			if (islower(cmd)) {
				if (rect1.x2 - rect1.x1 < BOARD_SIZE + add_board and rect1.y2 - rect1.y1 > 1) {
					rect1.x2++;
					rect1.y2--;
				}
			}
			else {
				if (rect1.x2 - rect1.x1 and rect1.y2 - rect1.y1 < BOARD_SIZE + add_board) {
					rect1.x2--;
					rect1.y2++;
				}
			}
			break;
		case 'l':
		case 'L':
			if (islower(cmd)) {
				rect2.x1--;
				rect2.x2--;
			}
			else {
				rect2.x1++;
				rect2.x2++;
			}
			break;
		case 'm':
		case 'M':
			if (islower(cmd)) {
				rect2.y1--;
				rect2.y2--;
			}
			else {
				rect2.y1++;
				rect2.y2++;
			}
			break;
		case 'n':
		case 'N':
			if (islower(cmd)) {
				if (rect2.x2 - rect2.x1 > 1 and rect2.y2 - rect2.y1 > 1) {
					rect2.x2--;
					rect2.y2--;
				}
			}
			else {
				if (rect2.x2 - rect2.x1 < BOARD_SIZE+add_board and rect2.y2 - rect2.y1 < BOARD_SIZE + add_board) {
					rect2.x2++;
					rect2.y2++;
				}
			}
			break;
		case 'o':
		case 'O':
			if (islower(cmd)) {
				if(rect2.x2 - rect2.x1 >1)
					rect2.x2--;
			}
			else {
				if(rect2.x2 - rect2.x1 < BOARD_SIZE + add_board)
					rect2.x2++;
			}
			break;
		case 'p':
		case 'P':
			if (islower(cmd)) {
				if(rect2.y2 - rect2.y1 > 1)
					rect2.y2--;
			}
			else {
				if(rect2.y2 - rect2.y1 < BOARD_SIZE + add_board)
					rect2.y2++;
			}
			break;
		case 't':
		case 'T':
			if (islower(cmd)) {
				if (rect2.x2 - rect2.x1 < BOARD_SIZE + add_board and rect2.y2 - rect2.y1 > 1) {
					rect2.x2++;
					rect2.y2--;
				}
			}
			else {
				if (rect2.x2 - rect2.x1 > 1 and rect2.y2 - rect2.y1 < BOARD_SIZE + add_board) {
					rect2.x2--;
					rect2.y2++;
				}
			}
			break;

		case 'b':
			cout << "»ç°¢Çü1ÀÇ ³ÐÀÌ: " << rect1.Rect_size() << endl;
			cout << "»ç°¢Çü2ÀÇ ³ÐÀÌ: " << rect2.Rect_size() << endl;

			system("pause");
			break;
		case 'c':
			if (add_board < 10) {
				add_board++;
				board.assign(BOARD_SIZE+add_board, vector<int>(BOARD_SIZE + add_board, 0));
			}
			break;
		case 'd':
			if (add_board > -20) {
				add_board--;
				board.assign(BOARD_SIZE + add_board, vector<int>(BOARD_SIZE + add_board, 0));
			}
			break;
		case 'r':
			board.assign(BOARD_SIZE, vector<int>(BOARD_SIZE, 0));
			cout << "»ï°¢Çü1 ÁÂÇ¥ ÀÔ·Â(ex. 3 3 8 8): ";
			cin >> x1 >> y1 >> x2 >> y2;
			rect1={ x1, x2 + 1, y1, y2 + 1 };

			cout << "»ï°¢Çü2 ÁÂÇ¥ ÀÔ·Â(ex. 3 3 8 8): ";
			cin >> x1 >> y1 >> x2 >> y2;
			rect2 = { x1, x2 + 1, y1, y2 + 1 };
			break;
		case 'q':
			return 0;
			break;
		}

		for (auto& row : board) {
			std::fill(row.begin(), row.end(), 0);
		}
		cin.clear();
		cin.ignore(1000, '\n');
	}
}