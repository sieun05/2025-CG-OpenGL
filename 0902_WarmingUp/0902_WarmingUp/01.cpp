//0909 화 까지 감점X

#include <iostream>
#include <stdlib.h>
#include <random>
#include <algorithm>

using namespace std;

random_device rd;
mt19937 gen(rd());
uniform_int_distribution<int> num(1, 9);

int metrix_det_4x4(int metrix[4][4]);
int metrix_det_3x3(int metrix[3][3]);
int metrix_det_2x2(int metrix[2][2]);

int main() 
{
	int metrix[2][4][4]{};
	int metrix_temp[2][4][4]{};
	int result[4][4]{};
	int metrix_det[2]{};

	char cmd{};
	int digit_down{};
	bool e_down{}, f_down{}, r_down{};

	for (int i{}; i < 2; i++) {
		for (int j{}; j < 4; j++) {
			for (int k{}; k < 4; k++) {
				metrix[i][j][k] = num(gen);
			}
		}
	}

	while (true) {
		system("cls");

		for (int i{}; i < 2; i++) {
			metrix_det[0] = metrix_det_4x4(metrix[0]);
			metrix_det[1] = metrix_det_4x4(metrix[1]);

			for (int j{}; j < 4; j++) {
				for (int k{}; k < 4; k++) {
					if (k == 0) cout << "| ";

					if (digit_down>0) {
						if (metrix[i][j][k] % digit_down == 0)
							cout << metrix[i][j][k] << ' ';
						else
							cout << "* ";
					}else
						cout << metrix[i][j][k] << ' ';

					if (k == 3) cout << '|';
				}
				if (j == 0 and r_down) cout << "        determinant: " << metrix_det[i];
				cout << endl;
			}
			cout << endl;
		}

		cout << "= \n\n";

		for (int i{}; i < 4; i++) {
			for (int j{}; j < 4; j++) {
				if (j == 0) cout << "| ";
				cout << result[i][j] << ' ';
				if (j == 3) cout << '|';
			}
			cout << endl;
		}
		cout << endl;

		memset(&result, 0, sizeof(result));

		cout << "명령어 입력: ";
		cin >> cmd;
		

		if (isdigit(cmd)) {
			if (digit_down > 0 and (int)(cmd-'0') == digit_down)
				digit_down = 0;
			else
				digit_down = (int)(cmd - '0');
		}
		else {
			switch (cmd) {
			case 'm':

				for (int i{}; i < 4; i++) {
					for (int j{}; j < 4; j++) {
						for (int k{}; k < 4; k++) {
							result[i][j] +=
								metrix[0][i][k] * metrix[1][k][j];
						}
					}
				}

				break;
			case 'a':
				for (int i{}; i < 4; i++) {
					for (int j{}; j < 4; j++) {
						result[i][j] = metrix[0][i][j] + metrix[1][i][j];
					}
				}

				break;
			case 'd':
				for (int i{}; i < 4; i++) {
					for (int j{}; j < 4; j++) {
						result[i][j] = metrix[0][i][j] - metrix[1][i][j];
					}
				}

				break;
			case 'r':
				r_down = r_down ? false : true;

				break;
			case 't':
				for (int i{}; i < 2; i++) {
					for (int j{}; j < 4; j++) {
						for (int k{j+1}; k < 4; k++) {
							swap(metrix[i][j][k], metrix[i][k][j]);
						}
					}
				}

				break;
			case 'e':
			{
				if (not e_down) {
					if (f_down) {
						e_down = false;
						memcpy(&metrix, &metrix_temp, sizeof(metrix_temp));
					}

					e_down = true;
					int min{ 99 };

					for (int i{}; i < 2; i++) {
						for (int j{}; j < 4; j++) {
							for (int k{}; k < 4; k++) {
								if (min > metrix[i][j][k])
									min = metrix[i][j][k];
							}
						}
					}

					memcpy(&metrix_temp, &metrix, sizeof(metrix));

					for (int i{}; i < 2; i++) {
						for (int j{}; j < 4; j++) {
							for (int k{}; k < 4; k++) {
								metrix[i][j][k] -= min;
							}
						}
					}
				}
				else {
					e_down = false;
					memcpy(&metrix, &metrix_temp, sizeof(metrix_temp));
				}

				break;
			}
			case 'f':
				if (not f_down) {
					if (e_down) {
						f_down = false;
						memcpy(&metrix, &metrix_temp, sizeof(metrix_temp));
					}

					f_down = true;
					int max{ -1 };

					for (int i{}; i < 2; i++) {
						for (int j{}; j < 4; j++) {
							for (int k{}; k < 4; k++) {
								if (max < metrix[i][j][k])
									max = metrix[i][j][k];
							}
						}
					}

					memcpy(&metrix_temp, &metrix, sizeof(metrix));

					for (int i{}; i < 2; i++) {
						for (int j{}; j < 4; j++) {
							for (int k{}; k < 4; k++) {
								metrix[i][j][k] += max;
							}
						}
					}
				}
				else {
					f_down = false;
					memcpy(&metrix, &metrix_temp, sizeof(metrix_temp));
				}

				break;
			case 's':
				for (int i{}; i < 2; i++) {
					for (int j{}; j < 4; j++) {
						for (int k{}; k < 4; k++) {
							metrix[i][j][k] = num(gen);
						}
					}
				}

				break;
			case 'q':
				return 0;

				break;
			}
		}
		cin.clear();
		cin.ignore(1000, '\n');
	}
}

int metrix_det_4x4(int metrix[4][4]) 
{
	int result{};
	int metrix_temp[3][3]{};

	for (int i{}; i < 4; i++) {
		int x{}, y{};

		for (int j{1}; j < 4; j++) {
			for (int k{}; k < 4; k++) {
				if (i == k) continue;
				metrix_temp[x][y++] = metrix[j][k];
				if (y == 3) {
					y = 0;
					++x;
				}
			}
		}

		int sign = i % 2 == 0 ? 1 : -1;
		result += metrix[0][i] * sign * metrix_det_3x3(metrix_temp);
	}
	
	return result;
}

int metrix_det_3x3(int metrix[3][3])
{
	int result{};
	int metrix_temp[2][2]{};

	for (int i{}; i < 3; i++) {
		int x{}, y{};

		for (int j{ 1 }; j < 3; j++) {
			for (int k{}; k < 3; k++) {
				if (i == k) continue;
				metrix_temp[x][y++] = metrix[j][k];
				if (y == 2) {
					y = 0;
					++x;
				}
			}
		}

		int sign = i % 2 == 0 ? 1 : -1;
		result += metrix[0][i] * sign * metrix_det_2x2(metrix_temp);
	}

	return result;
}

int metrix_det_2x2(int metrix[2][2]) 
{
	return metrix[0][0] * metrix[1][1] - metrix[0][1] * metrix[1][0];
}