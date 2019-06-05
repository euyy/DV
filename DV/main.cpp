#include "Router.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <iostream>
#include <time.h>
#include <Windows.h>
#include <thread>
#include <mutex>
#include <regex>
using namespace std;

vector<class Router> routerList;	//存储所有路由器的信息

int updateTimes = 1;				//路由表更新次数
bool isEnd = false;					//进程是否结束
mutex my_mutex;						//互斥量，保证同时只有一个对路由器的路由表进行操作

//打印所有路由器的路由表
void PrintAllRouter() {
	my_mutex.lock();
	int cnt = routerList.size();
	for (int i = 0; i < cnt; i++) {
		//cout << "-------------------------------- " << char(i + 'A') << " Router Start Print -------------------------------" << endl;
		routerList[i].PrintRouter();
		//cout << "-------------------------------- " << char(i + 'A') << " Router  End  Print -------------------------------" << endl << endl;
	}
	my_mutex.unlock();
}
//根据文件内容配置所有路由器的初始路由表
void InitRouters() {
	FILE *fp;
	fp = fopen("Router.txt", "r");
	if (fp == NULL) {
		cout << "ERROR! Open File Failed!" << endl;
	}
	int cost, row = 0;;
	char ch;
	Router router;

	while (fscanf(fp, "%d", &cost) != EOF) {
		router.cost.push_back(cost);
		//如果不是邻居节点，则设下一跳为-1，表示不可达
		if (cost == INFINT_COST) {
			router.nextHop.push_back(-1);
		}
		//如果是节点自身，则设下一跳为自身
		else if (cost == 0) {
			router.nextHop.push_back(router.cost.size() - 1);
		}
		//否则为邻居节点，设下一跳为邻居节点，并加入当前节点的邻居列表当中
		else {
			router.nextHop.push_back(router.cost.size() - 1);
			router.neighborID.push_back(router.cost.size() - 1);
			router.neighborCost.push_back(cost);
		}
		//当一行读完，则表示一个路由器已经初始化完成
		ch = fgetc(fp);
		if (ch == '\n') {
			router.No = row;
			row++;
			routerList.push_back(router);
			router.cost.clear();
			router.nextHop.clear();
			router.neighborID.clear();
			router.neighborCost.clear();
		}
	}
}
//更新所有路由器的路由表
void UpdateRouterList() {
	vector<class Router> RouterListCopy = routerList;		//当前所有路由的一个备份
	int cnt = routerList.size();
	for (int i = 0; i < cnt; i++) {
		routerList[i].UpdateRouter(RouterListCopy);
	}
}
//运行所有路由器
void RunRouters() {
	while (1) {
		if (isEnd)return;
		my_mutex.lock();
		cout << "-------------------------------The " << updateTimes++ << " times update-------------------------------" << endl;
		UpdateRouterList();
		my_mutex.unlock();
		Sleep(1000);
	}
}
//更改两个路由器间的Cost
int  ChangeCost(int router1,int router2,int cost) {
	int neighborNum = routerList[router1].neighborID.size();
	int i;
	for (i = 0; i < neighborNum; i++) {
		if (routerList[router1].neighborID[i] == router2) {
			routerList[router1].neighborCost[i] = cost;
			break;
		}
	}
	if (i == neighborNum) {
		return -1;
	}
	neighborNum = routerList[router2].neighborID.size();
	for (i = 0; i < neighborNum; i++) {
		if (routerList[router2].neighborID[i] == router1) {
			routerList[router2].neighborCost[i] = cost;
			break;
		}
	}
	if (i == neighborNum) {
		return -1;
	}
	int routerNum = routerList.size();
	for (i = 0; i < routerNum; i++) {
		routerList[i].convergenceTimes = 0;
		routerList[i].unchangedTimes = 0;
	}
	return 0;
}


int main() {
	InitRouters();
	thread runRouters(RunRouters);
	runRouters.detach();

	string cmd;	//输入的命令
	while (1) {
		cin >> cmd;
		//若是命令exit则退出程序
		if (cmd == "exit") {
			isEnd = true;
			return 0;
		}
		//若命令stop则暂停更新路由表
		else if (cmd == "stop") {
			my_mutex.lock();
			cin >> cmd;
			my_mutex.unlock();

		}
		//若是ALL则打印所有路由器的路由表
		else if (cmd == "ALL") {
			PrintAllRouter();
		}
		//如果是某个路由器的名字，则打印该路由器的路由表
		else if (cmd.size() == 1) {
			if (cmd[0] <= 'Z'&&cmd[0] >= 'A') {
				int routerNo = cmd[0] - 'A';
				if (routerNo < routerList.size()) {
					my_mutex.lock();
					//cout << "******************************** " << char(routerNo + 'A') << " Router Start Print********************************" << endl;
					routerList[routerNo].PrintRouter();
					//cout << "******************************** " << char(routerNo + 'A') << " Router  End  Print**********************************" << endl << endl;

					my_mutex.unlock();
				}
				else {
					cout << "Error! Router " << cmd << " does not exist!" << endl;
				}
			}
			else {
				cout << "Command Error!" << endl;
			}
		}
		//如果是change，则根据提示输入要改变的2个路由器的名字以及变化的cost
		else {
			if (cmd == "change") {
				my_mutex.lock();
				int router1 = -1, router2 = -1, cost = -1;
				while (1) {
					cout << "Please input Router1 name" << endl;
					cin >> cmd;
					if (cmd.size() == 1) {
						if (cmd[0] <= 'Z'&&cmd[0] >= 'A') {
							router1 = cmd[0] - 'A';
							if (router1 >= routerList.size()) {
								cout << "Router1 name Error!" << endl;
								continue;
							}
							else {
								break;
							}
						}
						else {
							cout << "Router1 name Error!" << endl;
							continue;
						}
					}
					else if (cmd == "exit") {
						break;
					}
					else {
						cout << "Router1 name Error!" << endl;
						continue;
					}
				}
				if (cmd == "exit") {
					my_mutex.unlock();
					continue;
				}
				while (1) {
					cout << "Please input Router2 name" << endl;
					cin >> cmd;
					if (cmd.size() == 1) {
						if (cmd[0] <= 'Z'&&cmd[0] >= 'A') {
							router2 = cmd[0] - 'A';
							if (router2 >= routerList.size()) {
								cout << "Router2 name Error!" << endl;
								continue;
							}
							else {
								break;
							}
						}
						else {
							cout << "Router2 name Error!" << endl;
							continue;
						}
					}
					else if (cmd == "exit") {
						break;
					}
					else {
						cout << "Router2 name Error!" << endl;
						continue;
					}
				}
				if (cmd == "exit") {
					my_mutex.unlock();
					continue;
				}
				while (1) {
					cout << "Please input the Cost between Router " << char(router1 + 'A') << "and Router " << char(router2 + 'A') << endl;
					cin >> cmd;
					regex num("^[0-9]+");
					if (regex_match(cmd, num)) {
						cost = atoi(cmd.c_str());
						break;
					}
					else if (cmd == "exit") {
						break;
					}
					else {
						cout << "Cost number Error!" << endl;
						continue;
					}					
				}
				if (cmd == "exit") {
					my_mutex.unlock();
					continue;
				}
				if (ChangeCost(router1, router2, cost) == -1) {
					cout << "Change Cost Failed! Router " << char(router1 + 'A') << " and Router " << char(router2 + 'A') << " are not neighbors!" << endl;
					my_mutex.unlock();
					continue;
				}
				my_mutex.unlock();
			}
			else {
				cout << "Command Error!" << endl;
				continue;
			}

		}
	}
	return 0;
}