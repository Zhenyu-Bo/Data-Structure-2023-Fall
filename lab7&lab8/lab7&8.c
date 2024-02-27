#ifndef _NETWORK_CPP_
#define _NETWORK_CPP_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#define MAXVERTEX 500
#define MAXEDGE 20000
#define MAXSIZE 200//栈和队列的容量
#define INFINITY 2147483647

//顶点结构体
typedef struct Vertex {
	int id;//顶点编号（唯一）
	int w;//顶点权值
	int degree;//顶点的度
	int edges[MAXVERTEX];//边的编号
	int visited;//标记顶点是否已被访问过
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

//定义栈和队列
typedef struct{
    int *base;//栈底指针
    int *top;//栈顶指针
    int stacksize;//栈最大容量
} Stack;

typedef struct{
    int *base;//队列基址
    int front;//队头
    int rear;//队尾
} Queue;

#include "showgt.h"

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
		g->v[i].visited = 0;
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

//查找给定两端点的边，返回其编号
int getEdgeByVex(Graph *g,int h,int t)
{
	for(int i = 0;i < g->ne;i++)
		if(g->e[i].h == h && g->e[i].t == t)
			return g->e[i].id;
	printf("input wrong edge id in getEdgeIdx()!\n");
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
	h = getVexIdx(g,h); t = getVexIdx(g,t);
	//先处理顶点
	for(i = 0;i < g->v[h].degree;i++)
	{
		if(g->v[h].edges[i] == id)
		{
			//将编号为id的边替换为v[h]关联的最后一条边，再将最后一条边删去（degree--即可）
			g->v[h].edges[i] = g->v[h].edges[g->v[h].degree - 1];
			//g->e[g->v[h].edges[g->v[h].degree - 1]].id = id;
			
			break;
		}
	}
	g->v[h].degree--;
	//如果g为无向图，则头和尾都需要处理
	if(g->dirctional == 0)
	{
		for(i = 0;i < g->v[t].degree;i++)
		{
			if(g->v[t].edges[i] == id)
			{
				//将编号为id的边替换为v[t]关联的最后一条边，再将最后一条边删去（degree--即可）
				g->v[t].edges[i] = g->v[t].edges[g->v[t].degree-1];
				
				break;
			}
		}
		g->v[t].degree--;
	}
	//再处理边,先将编号为idx的边替换为最后一条边（编号不变，只改变头尾结点和权值），再将最后一条边关联的顶点关联到编号为idx的边上
	g->e[idx] = g->e[g->ne - 1];
	g->e[idx].id = idx;
	h = g->e[idx].h; t = g->e[idx].t;
	h = getVexIdx(g,h); t = getVexIdx(g,t);
	for(i = 0;i < g->v[h].degree;i++)
	{
		if(g->v[h].edges[i] == g->e[g->ne - 1].id)
		{
			g->v[h].edges[i] = g->e[idx].id;
			break;
		}		
	}
	if(!g->dirctional)
	{
		for(i = 0;i < g->v[t].degree;i++)
		{
			if(g->v[t].edges[i] == g->e[g->ne - 1].id)
			{
				g->v[t].edges[i] = g->e[idx].id;
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
	int idx = getVexIdx(g,h);
	for(i = 0;i < g->v[idx].degree;i++)
	{
		if(g->e[g->v[idx].edges[i]].t == t && g->e[g->v[idx].edges[i]].w == w)
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
 	g->v[idx].edges[g->v[idx].degree] = g->ne;
	g->v[idx].degree++;
	if(!g->dirctional)
	{
		int idx1 = getVexIdx(g,t);
		//如果g为无向图，h的degree和edges也需要修改
		g->v[idx1].edges[g->v[idx1].degree] = g->ne;
		g->v[idx1].degree++;
	}	
	g->ne++;
}

//定义栈的基本操作

//初始化生成空栈
void InitStack(Stack *S)
{
    S->base = (int *)malloc(sizeof(int)*MAXSIZE);
    if(!S->base)
    {
        printf("内存空间分配失败!\n");
        exit(0);
    }
    S->top = S->base;
    S->stacksize = MAXSIZE;
}

//判断栈是否为空
int EmptyStack(Stack *S)
{
    return (S->top == S->base) ? 1 : 0;
}

//入栈
int Push(Stack *S, int e)
{
    //先判断栈是否已满
    if(S->top - S->base >= S->stacksize)
    {
        /*int *new_base = (int *)realloc(S->base,S->stacksize * 2);
        if(!new_base)
        {
            printf("栈已满且重新分配更大空间失败!\n");
            return 0;
        }
        S->base = new_base;
        S->stacksize = S->stacksize * 2;
        S->top = S->base + S->stacksize;*/
		//为使搜索循环会结束，在栈满时不再额外分配空间，而是报错
		printf("栈已满!\n");
		return 0;
    }
    *(S->top++) = e;
    return 1;
}

//出栈
int Pop(Stack *S,int *e)
{
    if(EmptyStack(S))
    {
        printf("空栈!\n");
        return 0;
    }
    *e = *(--(S->top));
    return 1;
}

//定义队列的基本操作

//初始化生成一个队列
void InitQueue(Queue *Q)
{
	Q->base = (int *)malloc(sizeof(int)*MAXSIZE);
	if(!Q->base)
	{
		printf("内存空间分配失败!\n");
		exit(0);
	}
	Q->front = Q->rear = 0;
}

//判断队列是否为空
int EmptyQueue(Queue *Q)
{
	return (Q->front == Q->rear) ? 1 : 0;
}

//队尾入队
int EnQueue(Queue *Q, int e)
{
	if(Q->front == (Q->rear + 1) % MAXSIZE)
	{
		printf("队列已满!\n");
		return 0;
	}
	Q->base[Q->rear] = e;
	Q->rear = (Q->rear + 1) % MAXSIZE;//采用循环队列的方式提高空间利用率
	return 1;
}

//队头出队
int DeQueue(Queue *Q,int *e)
{
	if(EmptyQueue(Q))
	{
		printf("空队列!\n");
		return 0;
	}
	*e = Q->base[Q->front];
	Q->front = (Q->front + 1) % MAXSIZE;
	return 1;
}

//利用Fringe栈实现深度优先搜索:寻找从顶点i到顶点j的路径
int *DFS(Graph *g,int i,int j)
{
	Stack *Fringe = (Stack *)malloc(sizeof(Stack));
	if(!Fringe){
		printf("ERROR!\n");
		return NULL;
	}
	InitStack(Fringe);
	int length = 0;
	int *path = (int *)malloc(sizeof(int)*MAXSIZE);
	memset(path,-1,sizeof(int)*MAXSIZE);//因为顶点编号全都非负，所以路径数组全都初始化为-1表示没有顶点
	if(i == j)
	{
		path[length++] = i;
		return path;
	}
	//先将所有顶点的visited设置为0
	for(int k = 0;k < g->nv;k++)
		g->v[k].visited = 0;
	Push(Fringe,i);
	int s,k,s1;
	while(1)
	{
		if(EmptyStack(Fringe) || length >= MAXSIZE)//如果Fringe为空或者路径长度超过了数组长度，返回空表示未找到路径
			return NULL;
		Pop(Fringe,&s);
		if(!g->v[s].visited)//如果s未被访问过，访问s
		{
			g->v[s].visited = 1;
			path[length++] = s;
			if(g->dirctional)
			{
				for(k = 0;k < g->v[s].degree;k++)
				{
					//if(s == g->e[g->v[s].edges[k]].h)
						s1 = g->e[g->v[s].edges[k]].t;
					//else
						//s1 = g->e[g->v[s].edges[k]].h;
					if(s1 == j)
						return path;
					else
					{
						if(!Push(Fringe,s1))
							return NULL;//如果入栈失败，说明此时栈已满，返回NULL
					}		
				}
			}
			else
			{
				for(k = 0;k < g->ne;k++)
				{
					if(s == g->e[k].h)
						s1 = g->e[g->v[s].edges[k]].t;
					else
						s1 = g->e[k].h;
					if(s1 == j)
						return path;
					else
					{
						if(!Push(Fringe,s1))
							return NULL;//如果入栈失败，说明此时栈已满，返回NULL
					}		
				}
			}
		}		
	}
}

//打印路径
void PrintPath(int *path,int j)
{
	for(int i = 0;path[i] != -1;i++)
		printf("%d->",path[i]);
	printf("%d\n",j);
}

//利用Fringe队列实现广度优先搜索，基本与深度优先搜索相同，将栈换成队列即可
int *WFS(Graph *g,int i,int j)
{
	Queue *Fringe = (Queue *)malloc(sizeof(Queue));
	if(!Fringe){
		printf("ERROR!\n");
		return NULL;
	}
	InitQueue(Fringe);
	int length = 0;
	int *path = (int *)malloc(sizeof(int)*MAXSIZE);
	memset(path,-1,sizeof(int)*MAXSIZE);//因为顶点编号全都非负，所以路径数组全都初始化为-1表示没有顶点
	if(i == j)
	{
		path[length++] = i;
		return path;
	}
	//先将所有顶点的visited设置为0
	for(int k = 0;k < g->nv;k++)
		g->v[k].visited = 0;
	EnQueue(Fringe,i);
	int s,k,s1;
	while(1)
	{
		if(EmptyQueue(Fringe) || length >= MAXSIZE)//如果Fringe为空或者路径长度超过了数组长度，返回空表示未找到路径
			return NULL;
		DeQueue(Fringe,&s);
		if(!g->v[s].visited)//如果s未被访问过，访问s
		{
			g->v[s].visited = 1;
			path[length++] = s;
			if(g->dirctional)
			{
				for(k = 0;k < g->v[s].degree;k++)
				{
					//if(s == g->e[g->v[s].edges[k]].h)
						s1 = g->e[g->v[s].edges[k]].t;
					//else
						//s1 = g->e[g->v[s].edges[k]].h;
					if(s1 == j)
						return path;
					else
					{
						if(!EnQueue(Fringe,s1))
							return NULL;//如果入队失败，说明此时队列已满，返回NULL
					}		
				}
			}
			else
			{
				for(k = 0;k < g->ne;k++)
				{
					if(s == g->e[k].h)
						s1 = g->e[g->v[s].edges[k]].t;
					else
						s1 = g->e[k].h;
					if(s1 == j)
						return path;
					else
					{
						if(!EnQueue(Fringe,s1))
							return NULL;//如果入队失败，说明此时队列已满，返回NULL
					}		
				}
			}
		}		
	}
}

void DFS_2(Graph *g,int i)
{
	int j, u, v;
	Stack *S = (Stack *)malloc(sizeof(Stack));
	InitStack(S);
	g->v[i].visited = 1;
	Push(S,i);
	while(!EmptyStack(S))
	{
		Pop(S,&u);
		//u = g->v[u].id;
		for(j = 0;j < g->ne;j++)
		{
			//遍历u的所有邻接点
			if(g->e[j].h == g->v[u].id)
				v = g->e[j].t;
			else if (g->e[j].t == g->v[u].id)
				v = g->e[j].h;
			else
				continue;
			v = getVexIdx(g,v);//获得v的下标
			if(!g->v[v].visited)
			{
				g->v[v].visited = 1;
				Push(S,v);
			}
		}
	}
}

//求图的连通片数量
int ConSlices(Graph *g)
{
	int count = 0,i;
	for(i = 0;i < g->nv;i++)
		g->v[i].visited = 0;
	for(i = 0;i < g->nv;i++)
	{
		if(!g->v[i].visited)
		{
			count++;
			DFS_2(g,i);
		}
	}
	return count;
}

//int father[MAXSIZE];
int num_of_sets;

//找结点x的前驱节点
int find(int x,int father[])
{
	int root = x,original_father;
	while(father[root] != -1)
	{
		root = father[root];
	}
	while(x != root)
	{
		original_father = father[x];
		father[x] = root;
		x = original_father;
	}
	return root;
}

int isConnected(int x,int y,int father[])
{
	return (father[x] == father[y]);
}

//合并两个连通片，同时并查集数量减1
void Union(int x,int y,int father[])
{
	int root_x = father[x], root_y = father[y];
	if(root_y != root_x)
	{
		father[root_y] = root_x;
		num_of_sets--;
	}
}

//求连通片数量
int ConSlices_2(Graph *g)
{
	int father[g->nv],i,j,u,v,w,fa;
	num_of_sets = g->nv;
	for(i = 0;i < g->nv;i++)
		father[i] = i;
	for(i = 0;i < g->nv;i++)
	{
		u = g->v[i].id;
		for(j = 0; j < g->v[i].degree;j++)
		{
			v = g->e[g->v[i].edges[j]].t;
			v = getVexIdx(g,v);
			if(father[i] != father[v])
			{
				fa = father[v];
				for(w = 0;w < g->nv;w++)
				{
					if(father[w] == fa)
						father[w] = father[i];
				}
				num_of_sets--;
			}
				//Union(i,v,father);
		}
	}
	return num_of_sets;
}


//Dijkstra算法求v0到其余所有顶点的最短路径
//其中Path数组用于记录最短路径上每个节点的前驱结点，dist数组用于记录v0到i到最短路径长度
void ShortestPaths(Graph *g,int v0,int path[],int *dist)
{
	int i,j,min,v,w,weight;
	int final[g->nv];//final[i]为1时表示已找到从u到i的最短路径
	for(i = 0;i < g->nv;i++)
	{
		final[i] = 0;
		dist[i] = INFINITY;
		path[i] = -1;//path数组用于记录每个结点的前驱结点
	}
	for(i = 0;i < g->v[v0].degree;i++)
	{
		dist[g->e[g->v[v0].edges[i]].t] = g->e[g->v[v0].edges[i]].w;//将所有接收了v0发出的边的顶点到v0的距离设置为边的权值
		path[g->e[g->v[v0].edges[i]].t] = v0;
	}	
	final[v0] = 0;//认为v0到v0间已经有最短路径0
	dist[v0] = 0;
	path[v0] = v0;
	//开始主循环，每次求得一个V-S中的到v0距离最短的一个顶点v，将其加到S集合中，即将对应的final赋值为1
	for(i = 1;i < g->nv;i++)
	{
		min = INFINITY;
		for(j = 0;j < g->nv;j++)
		{
			if(!final[j])//j不在集合S中
			{
				if(dist[j] < min)
				{
					min = dist[j];
					v = j;
				}
			}
		}
		//找到当前V-S集合中离v0最近的顶点，将其添加到S集合中，再更新其余非S集合中的顶点到v0的最短路径
		final[v] = 1;
		for(j = 0;j < g->v[v].degree;j++)
		{
			w = g->e[g->v[v].edges[j]].t;
			weight = g->e[g->v[v].edges[j]].w;
			if(!final[w] && dist[v] + weight < dist[w])
			{
				//更新v0到w的路径及其长度
				dist[w] = dist[v] + weight;
				path[w] = v;
			}
		}
	}
}

//打印v0到i的最短路径
void PrintSPath(Graph *g,int path[],int i,int v0)
{
	Stack *S = (Stack *)malloc(sizeof(Stack));
	InitStack(S);
	//Push(S,i);
	int u = i;
	while((u = path[u]) != v0)
	{
		if(u == -1)
		{
			printf("无路径!\n");
			return ;
		}
		Push(S,u);
	}
		
	//printf("%d->",v0);
	while(!EmptyStack(S))
	{
		printf("%d->",u);
		Pop(S,&u);
	}
	printf("%d->%d\n",u,i);
}

//测试函数
void Test(Graph *g)
{
	int i,j,count;

	//给所有编号为偶数的顶点间增加边
	for(i = 0;i < g->nv/2 - 1;i++)
	{
		addEdge(g,100,i*2,i*2+2);
	}
	printgraph(g);
	saveGraph(g,"net_1.html");
	int s,t,u,id;
	//利用Dijkstra算法求最短路径，并删除两个顶点的最短路径上的所有边
	printf("利用Dijkstra算法求s到其他点的最短路径:\n请输入s\n");
	scanf("%d",&s);
	int Path[g->nv],dist[g->nv];
	ShortestPaths(g,s,Path,dist);
	for(i = 0;i < g->nv;i++)
	{
		printf("从%d到%d的最短路径为:\n",s,i);
		PrintSPath(g,Path,i,s);
		printf("路径长度为:%d\n",dist[i]);
	}

	printf("删除s到t的最短路径上的所有边,请输入t:\n");
	scanf("%d",&t);
	u = t;
	while(Path[u] != -1)
	{
		id = getEdgeByVex(g,Path[u],u);
		deleteEdge(g,id);
		u = Path[u];
		if(u == s)
			break;
	}
	printgraph(g);
	saveGraph(g,"net_2.html");
	
	//再测试深度优先搜索和广度优先搜索
	printf("测试深度优先搜索和广度优先搜索:\n");
	printf("请输入起点和终点的编号:\n");
	scanf("%d%d" ,&i,&j);
	int *path = DFS(g,i,j);
	if(!path)
		printf("%d到%d之间无路径!\n",i,j);
	else
	{
		printf("利用深度优先搜索得%d到%d之间的路径为:\n",i,j);
		PrintPath(path,j);
	}
	path = WFS(g,i,j);
	if(!path)
		printf("%d到%d之间无路径!\n",i,j);
	else
	{
		printf("利用广度优先搜索得%d到%d之间的路径为:\n",i,j);
		PrintPath(path,j);
	}

	count = ConSlices(g);
	printf("图的连通片个数为:%d\n",count);

	//顺序删除顶点至图不连通
	i = 0;
	//num_of_sets = g->nv;
	while((count = ConSlices(g)) == 1)
	{
		deleteVex(g,i++);
		//printgraph(g);
	}
	printf("图的连通片个数为:%d\n",count);
	printgraph(g);
	saveGraph(g,"net_3.html");

	//添加顶点，并将其与其余所有顶点间连一条边
	addVex(g,100);
	printf("图的连通片个数为:%d\n",ConSlices(g));
	saveGraph(g,"net_4.html");
	id = locateVex(g,100);//查找顶点编号
	for(i = 0;i < g->nv - 1;i++)
	{
		addEdge(g,0,id,g->v[i].id);
		//printgraph(g);
	}
	printf("图的连通片个数为:%d\n",ConSlices(g));
	printgraph(g);
	saveGraph(g,"net_5.html");
}

int main() {
	Graph* g = (Graph*)malloc(sizeof(Graph));
	if (!g) {
		printf("error\n");
		exit(0);
	}	
	randgengraph(10, 0.2, 1, 1, "graph.txt", 5);
	initGraphFromFile(g, "graph.txt");
	printgraph(g);
	saveGraph(g,"net.html");
	Test(g);
	return 0;
} 

#endif