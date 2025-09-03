//0909 화 까지 감점X

#include <iostream>
#include <stdlib.h>
#include <random>
#include <vector>

using namespace std;

random_device dre;
mt19937 gen(dre());
uniform_int_distribution<int> num(1, 9);

int main() {
	int metrix[2][4][4]{};
	int metrix_temp[2][4][4]{};
	int result[4][4]{};

	bool e_down{}, f_down{}, num_down{};

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
			for (int j{}; j < 4; j++) {
				for (int k{}; k < 4; k++) {
					if (k == 0) cout << "| ";
					cout << metrix[i][j][k] << ' ';
					if (k == 3) cout << '|';
				}
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

		char cmd;
		cout << "명령어 입력: ";
		cin >> cmd;


		if (isdigit(cmd)) {

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


				break;
			case 't':
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