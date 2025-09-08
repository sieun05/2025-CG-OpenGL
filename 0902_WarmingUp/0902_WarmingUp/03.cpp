#include <iostream>
#include <vector>
#include <stdlib.h>

using namespace std;

struct Point {
	Point() = default;
	Point(int x, int y, int z, bool data_flag)
		: x{ x }, y{ y }, z{ z }, data_flag{ data_flag } {
	}

	int x, y, z;
	bool data_flag;
};

int main() {
	vector<Point> list;
	list.assign(10, Point(0, 0, 0, false));
	

}