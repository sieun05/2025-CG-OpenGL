#include <iostream>
#include <vector>
#include <stdlib.h>
#include <Windows.h>
#include <algorithm> // 추가

using namespace std;

struct Point {
	Point() = default;
	Point(int x, int y, int z, bool data_flag)
		: x{ x }, y{ y }, z{ z }, data_flag{ data_flag } {
	}
	Point(const Point& p) = default;
	Point& operator=(const Point& p) = default;

	int x, y, z;
	bool data_flag;
};

int main() {
	vector<Point> list;
	list.assign(10, Point(0, 0, 0, false));
	//list[0].x = list[0].y = list[0].z = 0;
	//list[0].data_flag = true;

	bool sorted_view = false;
	vector<Point> backup;

	while (true) {
		system("cls");

		for (int i{ (int)list.size()-1 }; i >= 0; i--) {
			cout << i << " | ";
			if (list[i].data_flag)
				cout << list[i].x << ' ' << list[i].y << ' ' << list[i].z;
			cout << endl;
			cout << "-------------------" << endl;
		}

		cout << "cmd: ";
		char cmd{};
		int x{-1}, y{-1}, z{-1};
		cin >> cmd;
		bool pass = true;
		if (cin.peek() != '\n') {
			pass = false;
			cin >> x >> y >> z;
		}

		switch (cmd) {
		case '+':
		{
			if (pass) break;
			bool flag = false;
			for (int i{ (int)list.size() - 1 }; i >= 0; i--) {
				if (i == 9 and list[i].data_flag) break;
				if (list[i].data_flag) {
					flag = true;
					list[i + 1] = { x, y, z, true };
					break;
				}
				else 
					continue;
			}
			if (not flag) {
				for (Point& p : list) {
					if (p.data_flag) continue;
					else {
						p = { x, y, z, true };
						flag = true;
						break;
					}
				}
				if (not flag) {
					list[0] = { x, y, z, true };
				}
			}
		}
			break;
		case '-':
			for (int i{ (int)list.size() - 1 }; i >= 0; i--) {
				if (list[i].data_flag) {
					list[i].data_flag = false;
					break;
				}
			}
			break;
		case 'e':
		{
			if (pass) break;
			bool flag = false;
			for (int i{}; i < (int)list.size(); i++) {
				if (list[i].data_flag) {
					if (i == 0) {
						size_t used = 0;
						while (used < list.size() && list[used].data_flag) ++used;

						// 꽉 찼다면 맨 끝 하나를 버리고(드롭) 한 칸만 밀 수 있게 used 조정
						if (used == list.size()) {
							// 끝 원소를 잃는 정책 (원치 않으면 resize/예외 등 다른 정책 선택)
							used = list.size() - 1;
						}
						std::move_backward(list.begin(), list.begin() + used, list.begin() + used + 1);
					}
					flag = true;
					list[i] = { x, y, z, true };
					break;
				}
				else
					continue;
			}
			if (not flag) {
				list[0] = { x, y, z, true };
			}

		}
			break;
		case 'd':
			for (Point& p : list) {
				if (p.data_flag) {
					p.data_flag = false;
					break;
				}
			}
			break;
		case 'a':
		{
			int num{};
			for (Point& p : list) {
				if (p.data_flag) num++;
			}
			cout << "점의 개수: " << num << "개" << endl;
			Sleep(1500);
		}
			break;
		case 'b':
			std::rotate(list.begin(), list.begin() + 1, list.end());
			break;
		case 'c':
			list.assign(10, Point(0, 0, 0, false));
			break;
		case 'f': {
			if (!sorted_view) {
				// 정렬 전 스냅샷 저장
				backup = list;

				// 활성(Point.data_flag==true)만 모아 정렬
				vector<Point> actives;
				actives.reserve(list.size());
				for (const auto& p : list)
					if (p.data_flag) actives.push_back(p);

				std::stable_sort(actives.begin(), actives.end(),
					[](const Point& a, const Point& b) {
						long long da = 1LL * a.x * a.x + 1LL * a.y * a.y + 1LL * a.z * a.z;
						long long db = 1LL * b.x * b.x + 1LL * b.y * b.y + 1LL * b.z * b.z;
						return da < db; // 오름차순(원하면 > 로 내림차순)
					});

				// 0번부터 빈칸 없이 재배치, 나머진 비우기
				size_t i = 0;
				for (; i < actives.size() && i < list.size(); ++i) {
					list[i] = actives[i];
					list[i].data_flag = true;
				}
				for (; i < list.size(); ++i) {
					list[i] = Point(0, 0, 0, false);
				}

				sorted_view = true;
			}
			else {
				// 복원
				list = backup;
				sorted_view = false;
			}
		} break;
		case 'q':
			return 0;
		}

		cin.clear();
		cin.ignore(1000, '\n');
	}
}