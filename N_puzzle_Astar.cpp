/**********************************************************************
 * 项目：AI实验一：利用A*算法求解N数码问题
 * 环境：VS2017
 * 功能：输出可解N数码问题的求解路径
 * 作者：flyAkari
 * 日期：2018/11/17
 **********************************************************************/

// N_puzzle_Astar.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#define _CRT_SECURE_NO_WARNINGS //禁用安全检查，防止fopen、fscanf报错
#include <iostream>
#include <time.h>

#define N 3              //N数码
#define MAX 10000        //最大搜索树规模

//int S0[N][N] = { {1,2,3}, {7,4,0}, {6,8,5} };
//int SG[N][N] = { {1,2,3}, {8,0,4}, {7,6,5} };

//int S0[N][N] = { {2,1,6}, {4,0,8}, {7,5,3} };
//int SG[N][N] = { {1,2,3}, {8,0,4}, {7,6,5} };

//int S0[N][N] = { {5,1,2,4}, {9,6,3,8}, {13,15,10,11},{14,0,7,12} };
//int SG[N][N] = { {1,2,3,4}, {5,6,7,8}, {9,10,11,12},{13,14,15,0} };

int S0[N][N]; //初始节点
int SG[N][N]; //目标节点

typedef struct Node
{
	int grid[N][N];  //N数码
	int parent;      //父节点
	int g;           //已走路径长度
	int h;           //曼哈顿距离（每个数和其目的位置的横纵距离之和）
	int f;           //估价函数:f = g + h
}Node;               //树的结点

Node OPEN[MAX];          //OPEN表
Node CLOSE[MAX];         //CLOSE表
int open = 0, close = 0; //OPEN表与CLOSE表中的结点个数

Node childNode, tmpChildNode; //孩子结点（从OPEN表中取出并展开的结点），临时孩子结点（用于变换时的临时存储）

int isGridEqual(Node A, Node B) //判断NxN的数码阵A和数码阵B是否相同
{
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			if (A.grid[i][j] != B.grid[i][j]) return 0; //若不同返回0
	return 1;                                           //若相同返回1
}

int isSG() //判断childNode中的数码阵是否为目标状态
{
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			if (SG[i][j] != childNode.grid[i][j]) return 0; //若不是返回0
	return 1;                                               //若是则返回1
}

void copyGrid(int srcGrid[][N], int dstGrid[][N]) //从src复制数码阵到dst
{
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			dstGrid[i][j] = srcGrid[i][j];
}

void printGrid(int Grid[][N])  //控制台输出数码阵
{
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			printf("%d ", Grid[i][j]);
		}
		printf("\n");
	}
}

void copyNode(Node src, Node &dst) //从src复制结点到dst
{
	copyGrid(src.grid, dst.grid);
	dst.parent = src.parent;
	dst.g = src.g;
	dst.h = src.h;
	dst.f = src.f;
}

void getGridFromOPEN()  //从OPEN表中取出一个结点放入childNode中
{
	copyNode(OPEN[0], childNode);
	open--;
	for (int i = 0; i < open; i++)
	{
		copyNode(OPEN[i + 1], OPEN[i]);
	}
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			OPEN[open].grid[i][j] = 0;
	OPEN[open].f = 0;
	OPEN[open].g = 0;
	OPEN[open].h = 0;
	OPEN[open].parent = 0;
}

void swapNode(Node &node1, Node &node2) //交换两个结点的位置
{
	Node tmp_Node;
	copyNode(node1, tmp_Node);
	copyNode(node2, node1);
	copyNode(tmp_Node, node2);
}

void sortOPEN()  //按估价函数值重排OPEN表
{
	for (int i = 0; i < open; i++)
		for (int j = 1; j < open - i; j++){
			if (OPEN[j - 1].f > OPEN[j].f){
				swapNode(OPEN[j - 1], OPEN[j]);
			}
		}
}

void swap(int &num1, int &num2) //交换两个数
{
	int tmp;
	tmp = num1;
	num1 = num2;
	num2 = tmp;
}

void addToOPEN() //添加childNode到OPEN表末尾
{
	copyNode(childNode, OPEN[open]);
	open++;
	sortOPEN();
}

void addToCLOSE() //添加childNode到CLOSE表末尾
{
	copyNode(childNode, CLOSE[close]);
	close++;
}

void searchZero(int &ii, int &jj) //寻找空格(0)在childNode中数码阵上的位置
{
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++){
			if (childNode.grid[i][j] == 0){
				ii = i;
				jj = j;
				return;
			}
		}
}

int moveLeftNum() //尝试移动空格左边的数
{
	int zero_i, zero_j;
	searchZero(zero_i, zero_j);
	if (zero_j == 0)
		return 0; //若空格在最左边则不能移动
	swap(childNode.grid[zero_i][zero_j], childNode.grid[zero_i][zero_j - 1]);
	return 1;
}

int moveRightNum() //尝试移动空格右边的数
{
	int zero_i, zero_j;
	searchZero(zero_i, zero_j);
	if (zero_j == N - 1)
		return 0;  //若空格在最右边则不能移动
	swap(childNode.grid[zero_i][zero_j], childNode.grid[zero_i][zero_j + 1]);
	return 1;
}

int moveUpNum()  //尝试移动空格上边的数
{
	int zero_i, zero_j;
	searchZero(zero_i, zero_j);
	if (zero_i == 0)
		return 0; //若空格在最上边则不能移动
	swap(childNode.grid[zero_i][zero_j], childNode.grid[zero_i - 1][zero_j]);
	return 1;
}

int moveDownNum()  //尝试移动空格下边的数
{
	int zero_i, zero_j;
	searchZero(zero_i, zero_j);
	if (zero_i == N - 1)
		return 0;  //若空格在最下边则不能移动
	swap(childNode.grid[zero_i][zero_j], childNode.grid[zero_i + 1][zero_j]);
	return 1;
}

int compareWithCLOSE() //将childNode与CLOSE表中的结点对比
{
	for (int i = 0; i < close; i++){
		if (isGridEqual(childNode, CLOSE[i]))
			return 0;  //若CLOSE表中已有相同状态，即已展开这种状态，则返回0
	}
	return 1;          //若CLOSE表中没有则返回1
}

int SearchNumInGrid(int &ii, int &jj, int num, int Grid[][N]) //在数码阵中寻找一个数num的位置
{
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++){
			if (Grid[i][j] == num){
				ii = i;
				jj = j;
				return 1;
			}
		}
	return 0;
}

int myAbs(int num) //返回num的绝对值
{
	if (num < 0)
		return -num;
	return num;
}

int getH(int Grid[][N], int SG[][N]) //计算曼哈顿距离
{
	int sum = 0;
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++){
			int num_i, num_j;
			if (Grid[i][j] == 0) continue;
			SearchNumInGrid(num_i, num_j, Grid[i][j], SG);
			sum += (myAbs(num_i - i) + myAbs(num_j - j));
		}
	return sum;
}

void expandNode()
{
	copyNode(childNode, tmpChildNode);
	if (moveLeftNum()) {                              //扩展N, 尝试移动空格左边的数，若可扩展则生成一个子节点		
		childNode.h = getH(childNode.grid, SG);       //计算h，为曼哈顿距离
		childNode.parent = close - 1;                 //记录父节点
		childNode.g = CLOSE[childNode.parent].g + 1;  //计算g，为其父节点的g+1
		childNode.f = childNode.g + childNode.h;      //计算f = g + h
		if (compareWithCLOSE()) {                     //若没展开过这种状态
			addToOPEN();                              //则放入OPEN表
			sortOPEN();                               //并按评价函数的升序重新排序OPEN表
		}
	}                                                 //继续生成下一个子节点
	copyNode(tmpChildNode, childNode);
	if (moveRightNum()) {
		childNode.h = getH(childNode.grid, SG);
		childNode.parent = close - 1;
		childNode.g = CLOSE[childNode.parent].g + 1;
		childNode.f = childNode.g + childNode.h;
		if (compareWithCLOSE()) {
			addToOPEN();
			sortOPEN();
		}
	}
	copyNode(tmpChildNode, childNode);
	if (moveUpNum()) {
		childNode.h = getH(childNode.grid, SG);
		childNode.parent = close - 1;
		childNode.g = CLOSE[childNode.parent].g + 1;
		childNode.f = childNode.g + childNode.h;
		if (compareWithCLOSE()) {
			addToOPEN();
			sortOPEN();
		}
	}
	copyNode(tmpChildNode, childNode);
	if (moveDownNum()) {
		childNode.h = getH(childNode.grid, SG);
		childNode.parent = close - 1;
		childNode.g = CLOSE[childNode.parent].g + 1;
		childNode.f = childNode.g + childNode.h;
		if (compareWithCLOSE()) {
			addToOPEN();
			sortOPEN();
		}
	}
	copyNode(tmpChildNode, childNode);
	//转Step2
}

void Astar()
{
	Node N0;
	N0.g = 0;
	N0.h = getH(S0, SG);
	N0.f = N0.g + N0.h;
	N0.parent = MAX;

	copyGrid(S0, N0.grid);
	copyNode(N0, childNode);
	addToOPEN();                //Step1: 把初始节点S0放入OPEN表中
	for (;;) {
		if (open == 0)          //Step2: 若OPEN表为空,则搜索失败,退出.
			return;
		getGridFromOPEN();      //Step3: 移出OPEN中第一个节点N
		addToCLOSE();           //       放入CLOSED表中, 并标以顺序号n;
		if (isSG()) return;     //Step4: 若目标节点Sg=N, 则搜索成功,结束.
		expandNode();           //Step5: 若N不可扩展, 则转Step2;
								//Step6: 展开一个结点，生成一组子节点并放入OPEN表
	}
}

void printPath() //输出信息和路径到控制台和文件
{
	Node printList[MAX];              //用于存储路径
	int printNum = 0;                   //用于记录路径长度
	int p = close - 1;                  //从CLOSE表末端开始找
	for (int i = 0; i < MAX; i++)
	{
		if (p == MAX) break;          //若已找到Start状态则退出循环
		copyNode(CLOSE[p], printList[i]);
		printNum++;
		p = CLOSE[p].parent;            //寻找其父节点
	}
	FILE *fp;
	fp = fopen("outfile.txt", "w");
	if (fp == NULL)
	{
		printf("Create file failed...\n");
		exit(0);
	}
	printf("Length=%d\n", printNum);    //路径长度
	fprintf(fp, "Length=%d\n", printNum);

	printf("Nodes=%d\n", open + close); //搜索树规模
	fprintf(fp, "Nodes=%d\n", open + close);

	for (int i = printNum - 1; i >= 0; i--)
	{
		printf("-----%d-----\n", printNum - i);
		fprintf(fp, "-----%d-----\n", printNum - i);
		printGrid(printList[i].grid);

		for (int ii = 0; ii < N; ii++){
			for (int jj = 0; jj < N; jj++){
				fprintf(fp, "%d ", printList[i].grid[ii][jj]);
			}
			fprintf(fp, "\n");
		}
	}
	fclose(fp);
}

int readFile()  //从文件中读取输入信息
{
	printf("Reading from \"infile.txt\"...\n");
	FILE *fp = fopen("infile.txt", "r");
	if (fp == NULL) {
		printf("Cannot found \"infile.txt\"\n");
		return -1;
	}
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			fscanf(fp, "%d", &S0[i][j]);
			fscanf(fp, "\n");
		}
	}
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			fscanf(fp, "%d", &SG[i][j]);
			fscanf(fp, "\n");
		}
	}
	fclose(fp);
	printf("Start:\n");
	printGrid(S0);
	printf("Goal:\n");
	printGrid(SG);
}

int main()
{
	readFile();               //从文件读取输入
	clock_t start, finish;
	double  duration;
	start = clock();          //计时开始
	Astar();                  //A*算法求解
	finish = clock();         //计时停止
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	printf("Time=%f seconds\n", duration);
	printPath();              //输出路径
	getchar();
}
