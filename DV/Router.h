#pragma once
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <iostream>
using namespace std;

#define INFINT_COST 255				//两路由器之间的cost值，表示不可达
#define MAX_UNCHANGE 3				//当路由表不变次数达到该值，认为收敛

class Router {
public:
	vector<int> cost;				//到各个路由的代价
	vector<int> nextHop;			//到x路由所要经过的下一个路由
	vector<int> neighborID;			//邻居列表
	vector<int> neighborCost;		//到邻居的代价
	int convergenceTimes;			//收敛次数
	int unchangedTimes;				//更新后与上次值相同，当达到MAX_UNCHANGE时判定其已经收敛
	int No;							//路由器编号
public:
	Router() {
		convergenceTimes = 0;
		unchangedTimes = 0;
	}
	//打印路由表
	void PrintRouter() {
		cout << "******************************** " << char(No + 'A') << " Router Start Print ********************************" << endl;

		cout << "\t\tDistance\t\tNext-Hop" << endl;
		int cnt = cost.size();
		for (int i = 0; i < cnt; i++) {
			cout << char(i + 'A') << "\t\t";
			if (cost[i] == INFINT_COST) {
				cout << endl;
			}
			else if (cost[i] == 0) {
				cout << "   " << 0 << "\t\t\t  ---" << endl;
			}
			else {
				cout << "   " << cost[i] << "\t\t\t   " << char(nextHop[i] + 'A') << endl;
			}
		}
		cout << "******************************** " << char(No + 'A') << " Router  End  Print ********************************" << endl << endl;

	}
	//更新路由表
	void UpdateRouter(vector<class Router> Routers) {
		int routerNum = Routers.size();
		for (int i = 0; i < routerNum; i++) {
			if (i == No) {
				cost[i] = 0;
				nextHop[i] = i;
			}
			else {
				cost[i] = INFINT_COST;
				nextHop[i] = -1;
			}
		}
		int neighborNum = Routers[No].neighborID.size();
		for (int i = 0; i < neighborNum; i++) {
			int neighborID = Routers[No].neighborID[i];
			int twoRoutersCost = Routers[No].neighborCost[i];
			Router neighbor = Routers[neighborID];
			for (int j = 0; j < routerNum; j++) {
				if (neighbor.cost[j] + twoRoutersCost < cost[j]) {
					//去掉注释部分则可快速收敛，否则收敛速度较慢
					//if (neighbor.nextHop[j] == No) {
					//	cost[j] = INFINT_COST;
					//	nextHop[j] = neighborID;
					//}
					//else {
						cost[j] = neighbor.cost[j] + twoRoutersCost;
						nextHop[j] = neighborID;
					//}
				}
			}
		}
		if (Routers[No].cost == cost) {
			if (unchangedTimes < MAX_UNCHANGE) {
				convergenceTimes++;
				unchangedTimes++;
				if (unchangedTimes >= MAX_UNCHANGE) {
					cout << "------------------------------Router " << char(No + 'A') << " has convergenced!------------------------" << endl;
					cout << "Convergence Times : " << convergenceTimes << endl;
					PrintRouter();
				}
			}
		}
		else {
			convergenceTimes++;
			unchangedTimes = 0;
		}
	}
};



/*
Test 1:
0	7	255	255	10
7	0	1	255	8
255	1	0	2	255
255	255	2	0	2
10	8	255	2	0

Test 2:
0	4	50
4	0	1
50	1	0

*/