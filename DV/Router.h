#pragma once
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <iostream>
using namespace std;

#define INFINT_COST 255				//��·����֮���costֵ����ʾ���ɴ�
#define MAX_UNCHANGE 3				//��·�ɱ�������ﵽ��ֵ����Ϊ����

class Router {
public:
	vector<int> cost;				//������·�ɵĴ���
	vector<int> nextHop;			//��x·����Ҫ��������һ��·��
	vector<int> neighborID;			//�ھ��б�
	vector<int> neighborCost;		//���ھӵĴ���
	int convergenceTimes;			//��������
	int unchangedTimes;				//���º����ϴ�ֵ��ͬ�����ﵽMAX_UNCHANGEʱ�ж����Ѿ�����
	int No;							//·�������
public:
	Router() {
		convergenceTimes = 0;
		unchangedTimes = 0;
	}
	//��ӡ·�ɱ�
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
	//����·�ɱ�
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
					//ȥ��ע�Ͳ�����ɿ������������������ٶȽ���
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