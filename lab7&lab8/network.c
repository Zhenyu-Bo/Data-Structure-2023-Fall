#ifndef _NETWORK_CPP_
#define _NETWORK_CPP_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#define MAXVERTEX 100
#define MAXEDGE 20000

//顶点结构体
typedef struct Vertex {
	int id;//顶点编号（唯一）
	int w;//顶点权值
	int degree;//顶点的度
	int edges[MAXVERTEX];//边的编号
	//int visited;
} Vertex;

//边结构体
typedef struct Edge {
	int id;//边的编号（唯一）
	int h;//边头顶点编号
	int t;//边尾顶点编号
	int w;//权值
} Edge;

//图结构体
typedef struct Graph {
	Vertex v[MAXVERTEX];//顶点数组
	Edge e[MAXEDGE];//边数组
	int nv;//顶点数
	int ne;//边数
	bool dirctional;//t:有向图，f:无向图
	bool weighted;//t:带权图，f:等权图
}Graph ;

#include "showgt.h"

int getEdgeIdx(Graph* g, int id);

//生成一个随机图，包括n个顶点，每个顶点和其它顶点连边的概率为p
int randgengraph(int n, float p, bool directional, bool weighted, const char* filename, int seed) {
	srand(seed);
	char* content = (char*)malloc(5000000);
	if (!content) {
		printf("分配存储空间失败！\n");
		exit(0);
	}
	FILE* fp = fopen(filename, "wb");
	if (!fp) {
		printf("生成图文件%s时失败！\n",filename);
		exit(0);
	}
	long offset = 0;//content当前的写入位置
	int ne = 0;//生成边数
	for (int i = 0; i < n; i++)
		offset += sprintf(content + offset, "%3d %5d\n", i, rand() % 100);//写入顶点的编号和顶点随机权值
	int pn = (int)(10000 * p);
	if (directional) {//有向图
		for(int i = 0; i < n; i++)
			for(int j = 0; j < n; j++)
				if (i != j && rand() % 10000 < pn) {
					offset += sprintf(content + offset, "%3d %3d %4d\n", i, j, weighted ? rand() % 100 : 1);
					ne++;
				}
	}
	else {//无向图
		for(int i = 0; i < n; i++)
			for(int j = i; j < n; j++)
				if (i != j && rand() % 10000 < pn) {
					offset += sprintf(content + offset, "%3d %3d %4d\n", i, j, weighted ? rand() % 100 : 1);
					ne++;
				}
	}
	char ch[40];
	int sz = sprintf(ch, "%d %7d %5s %5s\n", n, ne, weighted ? "true" : "flase", directional ? "true" : "flase");
	fwrite(&ch, sz, 1, fp);//写入图的顶点数和边数
	fwrite(content, offset, 1, fp);//写入图的顶点和边的信息
	fclose(fp);
	free(content);
}

//从文件中读入图，在内存中生成图的表示
void initGraphFromFile(Graph* g, const char* gfile) {
	FILE* fp = fopen(gfile, "r");
	if (!fp) {
		printf("读取图数据文件出错！\n");
		exit(0);
	}
	char bv[10];
	//读取图的基本信息
	fscanf(fp, "%u%u%s", &(g->nv), &(g->ne), bv);
	//printf("%d %d %s", g->ne, g->nv, bv);
	g->weighted = strcmp(bv, "true") == 0 ? true : false;
	fscanf(fp, "%s", bv);
	g->dirctional = strcmp(bv, "true") == 0 ? true : false;
	//读取顶点信息
	for (int i = 0; i < g->nv; i++) {
		fscanf(fp, "%d%d", &(g->v[i].id), &(g->v[i].w));
		g->v[i].degree = 0;
		//g->v[i].visited = 0;
		//printf("顶点%d-权%d\n", g->v[i].id, g->v[i].w);
	}
	//读取边信息
	for (unsigned long i = 0; i < g->ne; i++) {
		fscanf(fp, "%u%u%d", &(g->e[i].h), &(g->e[i].t), &(g->e[i].w));
		//printf("边:%u-%u=>%d\n", g->e[i].h, g->e[i].t, g->e[i].w);
		g->e[i].id = i;
		unsigned h, t;
		h = g->e[i].h;
		t = g->e[i].t;
		g->v[h].edges[g->v[h].degree++] = i;
		if(g->dirctional==false)
			g->v[t].edges[g->v[t].degree++] = i;
	}
	fclose(fp);
}

// 生成一个空图
Graph* initGraph(bool directional, bool weighted){
	Graph * g = (Graph *)malloc(sizeof(Graph));
	g->nv = g->ne = 0;
	g->dirctional = directional;
	g->weighted = weighted;
	return g;
}

//打印邻接表的信息
void printgraph(Graph* g) {
	printf("图的基本信息:顶点数(%u)-边数(%lu)-%s-%s\n", g->nv, g->ne, g->weighted ? "加权图" : "等权图", g->dirctional ? "有向图" : "无向图");
	for (int i = 0; i < g->nv; i++) {
		printf("ID(%u)-度(%d)-权(%d)-边表 t(eid|w):%u", g->v[i].id, g->v[i].degree, g->v[i].w, g->v[i].id);
		for (int j = 0; j < g->v[i].degree; j++) {
			unsigned long e = g->v[i].edges[j];
			//unsigned long e = getEdgeIdx(g,g->v[i].edges[j]);
			if (g->v[i].id == g->e[e].h)
				printf("->%u(%d)", g->e[e].t, g->e[e].w);
			else 
				printf("->%u(%d)", g->e[e].h, g->e[e].w);
		}
		printf("\n");
	}
}

//查找给定编号的顶点，返回其顶点数组下标
int getVexIdx(Graph* g, int id) {
	for (int i = 0; i < g->nv; i++)
		if (g->v[i].id == id)
			return i;
	printf("input wrong vertex id in getVexIdx()!\n");
	//exit(0);
	return -1;
}

//查找给定编号的顶点，返回其权值
int getVexW(Graph* g, int id) {
	for (int i = 0; i < g->nv; i++)
		if (g->v[i].id == id)
			return g->v[i].w;
	printf("input wrong vertex id in getVexW()!\n");
	exit(0);
}

//查找给定编号的顶点，返回其度
int getVexDegree(Graph* g, int id) {
	for (int i = 0; i < g->nv; i++)
		if (g->v[i].id == id)
			return g->v[i].degree;
	printf("input wrong vertex id in getVexDegree()!\n");
	exit(0);
}

//查找给定编号的顶点,返回其所有边
int* getEdgesByNode(Graph* g, int id, int* ne) {
	for (int i = 0; i < g->nv; i++)
		if (g->v[i].id == id) {
			*ne = g->v[i].degree;
			return g->v[i].edges;
		}
	printf("input wrong vertex id in getEdgesByNode()!\n");
	exit(0);
}

//查找给定编号的边，返回其边数组下标
int getEdgeIdx(Graph* g, int id) {
	for (int i = 0; i < g->ne; i++)
		if (g->e[i].id == id)
			return i;
	printf("input wrong edge id in getEdgeIdx()!\n");
	//exit(0);
	return -1;
}

//查找给定编号的边，返回其权值
int getEdgeW(Graph* g, int id) {
	for (int i = 0; i < g->ne; i++)
		if (g->e[i].id == id)
			return g->e[i].w;
	printf("input wrong edge id in getEdgeW()!\n");
	exit(0);
}

//查找给定编号的边，返回其两个顶点
void getVexByEdge(Graph* g, int id, int* h, int* t) {
	for (int i = 0; i < g->ne; i++)
		if (g->e[i].id == id) {
			*h = g->e[i].h;
			*t = g->e[i].t;
			return ;
		}		
	printf("input wrong edge id in getVexByEdge()!\n");
	exit(0);
}

//通过顶点编号查找边，返回边的编号
int locateEdgeById(Graph *g,int id)
{
	for(int i = 0;i < g->ne;i++)
	{
		if(g->e[i].h == id || g->e[i].t == id)
			return i;
	}
	return -1;
}

//通过权值查找顶点编号
int locateVex(Graph* g, int w) {
	for (int i = 0; i < g->nv; i++)
		if (g->v[i].w == w)
			return g->v[i].id;
	return -1;
}

//通过权值查找边编号
int locateEdge(Graph* g, int w) {
	for (int i = 0; i < g->ne; i++)
		if (g->e[i].w == w)
			return g->e[i].id;
	return -1;
}

//设置顶点权值
void setVexW(Graph* g, int id, int w) {
	for (int i = 0; i < g->nv; i++)
		if (g->v[i].id == id)
		{
			g->v[i].w = w;
			return ;
		}		
	printf("input wrong vertex id in setVexW()!\n");
	exit(0);
}

//设置边权值
void setEdgeW(Graph* g, int id, int w) {
	for (int i = 0; i < g->ne; i++)
		if (g->e[i].id == id)
		{
			g->e[i].w = w;
			return ;
		}			
	printf("input wrong edge id in setEdgeW()!\n");
	exit(0);
}

//删除边
void deleteEdge(Graph* g, int id) {
	int idx = getEdgeIdx(g,id);
	//首先判断边是否存在
	if(idx == -1)
	{
		printf("编号为%d的边不存在!\n",id);
		return ;
	}
	int h = g->e[idx].h , t = g->e[idx].t , i;
	//先处理顶点
	for(i = 0;i < g->v[h].degree;i++)
	{
		if(g->v[h].edges[i] == id)
		{
			//将编号为id的边替换为v[h]关联的最后一条边，再将最后一条边删去（degree--即可）
			g->v[h].edges[i] = g->v[h].edges[g->v[h].degree - 1];
			//g->e[g->v[h].edges[g->v[h].degree - 1]].id = id;
			g->v[h].degree--;
			break;
		}
	}
	//如果g为无向图，则头和尾都需要处理
	if(g->dirctional == 0)
	{
		for(i = 0;i < g->v[t].degree;i++)
		{
			if(g->v[t].edges[i] == id)
			{
				//将编号为id的边替换为v[t]关联的最后一条边，再将最后一条边删去（degree--即可）
				g->v[t].edges[i] = g->v[t].edges[g->v[t].degree-1];
				g->v[t].degree--;
				break;
			}
		}
	}
	//再处理边,先将编号为idx的边替换为最后一条边（编号不变，只改变头尾结点和权值），再将最后一条边关联的顶点关联到编号为idx的边上
	g->e[idx] = g->e[g->ne - 1];
	g->e[idx].id = idx;
	h = g->e[idx].h; t = g->e[idx].t;
	for(i = 0;i < g->v[h].degree;i++)
	{
		if(g->v[h].edges[i] == g->ne - 1)
		{
			g->v[h].edges[i] = idx;
			break;
		}		
	}
	if(!g->dirctional)
	{
		for(i = 0;i < g->v[t].degree;i++)
		{
			if(g->v[t].edges[i] == g->ne - 1)
			{
				g->v[t].edges[i] = idx;
				break;
			}
		}
	}
	g->ne--;
}


//删除顶点，同时与其关联的边全部删除，记得调用 deleteEdge() 哟~
void deleteVex(Graph* g, int id) {
	int idx = getVexIdx(g,id);
	if(idx == -1)
	{
		printf("编号为%d的顶点不存在!\n",id);
		return ;
	}
	int i;
	//遍历所有边，删除所有与id关联的边
	while((i = locateEdgeById(g,id)) != -1)
		deleteEdge(g,i);
	g->v[idx] = g->v[g->nv - 1];
	g->nv--;
}


//增加顶点
void addVex(Graph* g, int w) {
	
	//首先判断顶点数组是否已满
	if(g->nv > MAXVERTEX)
	{
		printf("顶点数组已满!\n");
		return ;
	}
	//再判断图中是否已存在权值为w的顶点
	if(locateVex(g,w) != -1)
	{
		printf("权值为%d的顶点已经存在!\n",w);
		return ;
	}
	
	g->v[g->nv].degree = 0;
	g->v[g->nv].id = g->nv;
	//添加的顶点的编号设为最大值，以确保顶点编号不会重复
	for(int i = 0;i < g->nv;i++)
	{
		if(g->v[i].id >= g->v[g->nv].id)
			g->v[g->nv].id = g->v[i].id + 1;
	}
	g->v[g->nv].w = w;
	g->nv++;
}

//增加边
void addEdge(Graph* g, int w, int h, int t) {
	//首先判断增加的边的两个顶点是否存在，图g中存在的顶点编号应满足 0 <= id < g->v
	if(getVexIdx(g,h) == -1 || getVexIdx(g,t) == -1)
	{
		printf("边所依附的顶点不存在!\n");
		return ;
	}
	//再判断边数组是否已满
	else if(g->ne >= MAXEDGE)
	{
		printf("边数组已满!\n");
		return ;
	}
	//最后再判断要增加的边是否已经存在
	int i = 0;
	for(i = 0;i < g->v[h].degree;i++)
	{
		if(g->e[g->v[h].edges[i]].t == t && g->e[g->v[h].edges[i]].w == w)
		{
			printf("The edge has exited!\n");
			return ;
		}
	}
	//若以上几种情况都不成立，则可以添加边
	g->e[g->ne].h = h;
	g->e[g->ne].t = t;
	g->e[g->ne].w = w;
	g->e[g->ne].id = g->ne;
	/*//添加的边的编号设为最大值，以确保边编号不会重复
	for(i = 0;i < g->ne;i++)
	{
		if(g->e[i].id >= g->e[g->ne].id)
			g->e[g->ne].id = g->e[i].id + 1;
	}*/
 	g->v[h].edges[g->v[h].degree] = g->ne;
	g->v[h].degree++;
	if(!g->dirctional)
	{
		//如果g为无向图，h的degree和edges也需要修改
		g->v[t].edges[g->v[t].degree] = g->ne;
		g->v[t].degree++;
	}	
	g->ne++;
}

int main() {
	Graph* g = (Graph*)malloc(sizeof(Graph));
	if (!g) {
		printf("error\n");
		exit(0);
	}	
	randgengraph(10, 0.2, 0, 1, "graph.txt", 5);
	initGraphFromFile(g, "graph.txt");
	printgraph(g);
	saveGraph(g,"net.html");
	
	int h ,t;
	addVex(g, 30);//增加的顶点已存在，失败
	addVex(g,31);//增加的顶点不存在，成功
	addEdge(g, 50, 1, 3);//增加的边不存在，成功
	addEdge(g,7,0,5);//增加的边已存在，失败
	printgraph(g);

	getVexByEdge(g, 10, &h, &t);
	printf("%d %d\n", h, t);
	deleteEdge(g, 10);//删除边存在，成功
	deleteEdge(g,30);//删除的边不存在
	printgraph(g);
	
	addEdge(g,25,5,2);//增加边
	printgraph(g);
	deleteVex(g,5);//删除顶点
	deleteVex(g,5);//删除不存在的顶点
	printgraph(g);
	saveGraph(g,"new_net.html");
} 
#endif