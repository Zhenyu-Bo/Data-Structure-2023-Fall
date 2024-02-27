#include<stdio.h>
#include<stdlib.h>
#include<time.h>

//添加的nqueen_test中使用的nqueen_array即solution，length即len，简单起见，直接用宏替换
#define nqueen_array solution
#define length len

//定义全局变量
int *solution;//solution[col]=row,表示第col列皇后放置的行号
int len;//棋盘宽度，也即皇后个数
int temp;//全局变量，用于swap函数中，避免每次调用swap函数都为temp分配空间
int *diag1;//存放副对角线上的皇后数量
int *diag2;//存放正对角线上的皇后数量

//函数声明
void initSolution();//初始化生成一个棋盘
void restart();//随机重启，重置solution
void swap(int i,int j);//交换棋盘的第i列和第j列
int traverseNeighbors();//遍历邻居棋盘
int evaluate();//计算棋盘的评价函数
int Conflicts(int i,int j);//计算i,j两列皇后所在的四条对角线以及交换i,j两列后的皇后所在的四条对角线上的冲突数量
int NqueenTest();//判断求得的解是否正确

int main(){
	//long seed = 820;  //随机数种子，如果这个数不改变，那么每次算法运行的结果都是一样的，即使用rand()函数
	//srand(seed);  //这一句注释掉，等于每次的随机数种子都不一样

    srand(time(NULL));  // 添加随机种子

	printf("请输入皇后个数：");
	scanf("%d",&len);     	// 键盘读入棋盘的大小

	solution = (int *)malloc(sizeof(int)*len);
	diag1 = (int *)malloc(sizeof(int)*(len*2-1));
	diag2 = (int *)malloc(sizeof(int)*(len*2-1));
	if (!solution||!diag1||!diag2)  return 0; //分配空间失败

	initSolution();			// 初始化一个棋盘布局
	
	int flag;
	while ((flag = traverseNeighbors())!=0)
		if (flag < 0)	restart();
	
	printf("冲突数为：%d\n得到的解为:",evaluate());
	for(int i=0;i<len;++i)
		printf("%d,",solution[i]);
    printf("\b ");//退格符，删去最后多余的一个“,”
    NqueenTest();
	getchar();

	//释放堆空间
	free(solution);
	free(diag1);
	free(diag2);

    system("pause");
	return 0;
}

//将棋盘的第i列和第j列交换,同时修改相关对角线上的皇后数量
void swap(int i, int j){
	//(i,solution[i]),(j,solution[j])所在对角线上的皇后数量均减1
    diag1[i+solution[i]]--; diag2[len-1+i-solution[i]]--;
    diag1[j+solution[j]]--; diag2[len-1+j-solution[j]]--;

    //交换i，j两列上的皇后
    int temp = solution[i];
    solution[i] = solution[j];
    solution[j] = temp;

    //新的(i,solution[i]),(j,solution[j])所在对角线上的皇后数量均加1
    diag1[i+solution[i]]++; diag2[len-1+i-solution[i]]++;
    diag1[j+solution[j]]++; diag2[len-1+j-solution[j]]++;
}


/***  初始化一个棋盘布局,将邻居数组准备好  ***/
void initSolution(){
	int i = 0;
	//先将diag1, diag2全部初始化为0
	for(i = 0;i < len * 2 - 1;i++) {
		diag1[i] = 0;
		diag2[i] = 0;
	}
	/*** 设定初始布局为所有皇后均在正对角线上，
	所以每条副对角线上的皇后数量均为1，主对角线上皇后数量为len，其余正对角线上无皇后 ***/
	for (i = 0; i < len; ++i){
        solution[i] = i;
        diag1[i*2] = 1;    
    }
    diag2[len-1] = len;
    int a,b;
	//再对棋盘进行2len次交换
    for (int i = 0; i < 2 * len; ++i){
        a = rand()%len;
        b = rand()%len;
        if(a != b)  swap(a,b);
    }
}

//计算棋盘的评价函数，返回可相互攻击到的皇后对数
int evaluate(){
	int value = 0;
	//因为设置棋盘时每行每列都只有一个皇后，所以计算冲突数量时只需要考虑对角线上的冲突
	//如果一条对角线上有n个皇后，那么这条对角线上的皇后数量为n(n-1)/2
    for (int i = 0; i < len*2-1; i++) {
		value=value+diag1[i]*(diag1[i]-1)/2+diag2[i]*(diag2[i]-1)/2;
    }
    return value;
}

//计算与交换前后的皇后相关的八条对角线上的冲突数量
int Conflicts(int i,int j){
    //int si=solution[i],sj=solution[j];
    //int num = 0;
	//为了便于观看，先分别计算八条对角线上的冲突数量再相加，如果全部一起相加行代码会过长
    int n1=diag1[i+solution[i]]*(diag1[i+solution[i]]-1)/2;
    int n2=diag2[len-1+i-solution[i]]*(diag2[len-1+i-solution[i]]-1)/2;
    int n3=diag1[j+solution[j]]*(diag1[j+solution[j]]-1)/2;
    int n4=diag2[len-1+j-solution[j]]*(diag2[len-1+j-solution[j]]-1)/2;
    int n5=diag1[i+solution[j]]*(diag1[i+solution[j]]-1)/2;
    int n6=diag2[len-1+i-solution[j]]*(diag2[len-1+i-solution[j]]-1)/2;
    int n7=diag1[j+solution[i]]*(diag1[j+solution[i]]-1)/2;
    int n8=diag2[len-1+j-solution[i]]*(diag2[len-1+j-solution[i]]-1)/2;
	/*** 如果交换前的两个皇后在同一正（副）对角线上，交换后的两皇后也会在同一副（正）对角线上，
	***  那么这八条对角线有重复的部分 
	***/
    if(j+solution[j]==i+solution[i]){
        n3=0;
        n8=0;
    }
    else if(j-solution[j]==i-solution[i]){
        n4=0;
        n7=0;
    }
    return (n1+n2+n3+n4+n5+n6+n7+n8);
}

/***  检查当前解（棋盘）的邻居棋盘（交换solution[i]和solution[j],i和j随机选择，共有n(n-1)/2种不同的可能）
 ***  保留冲突最小的棋盘，将它视为新的解（best found策略）
 ***  若返回-1,表示没有找到更好的邻居，需要重启 restart() ,返回0表示找到所求的解，
 ***  返回大于0的值表示需要更新当前解solution后要继续遍历新解的邻居
 ***/
int traverseNeighbors(){
	int min_value = evaluate();     // 当前棋盘的所有邻居的最小评价函数值
	if(min_value==0)    return min_value;
	int min_col = -1,min_row = -1;	// 上述对应的邻居
	int delta = 0;//交换皇后后的冲突减少量
    /*** 遍历邻居，评估每个邻居冲突皇后对相较于原来的减少量，找到最小冲突对数的邻居 
	 *** 因为交换皇后前后只有(i,solution[i]),(j,solution[j]),(i,solution[j]),(j,solution[i])上的皇后数量变化
	 *** 所以只需考虑与之关联的八条对角线上的冲突减少量即可
	 ***/
    for(int i=0;i<len;++i)
    {
        for(int j=i+1;j<len;++j)
        {
			int old = Conflicts(i,j);
            swap(i,j);
			int new = Conflicts(i,j);//old,new分别表示交换前后相关的八条对角线上冲突的皇后数量

            if ( old - new > delta ) {
                delta = old - new;
                min_col = i;
                min_row = j;
            }
            swap(i,j); //还原棋盘状态
        }
    }

    if (min_col == -1) {  //如果遍历了所有的邻居节点，也找不到更好的移动方式，那么就需要重新启动算法，初始棋局改变一下
    	printf("找不到解，正在重新生成初始解和运行搜索算法中...\n");
    	return -1;
    }
	min_value=min_value-delta;
    swap(min_col,min_row);	// 找到更好的移动方式，那么就以这个邻居进行下一次爬山
    return min_value;
}

/***  当遍历完所有邻居，也没有找到更好的邻居
 ***  那么随机重置solution，然后再用爬山法搜索
 ***  随机重启：邻居是邻域内的“微小”的扰动，增加扰动范围，比如随机交换times=20次
 ***/
void restart(){
	int times = 20;  // 随机交换解的次数，扰动大小,设置times>1即可
	for(int i = 0; i < times; ++i)
		swap(rand()%len,rand()%len);
	puts("restart()被调用！");
}

int NqueenTest()		//传入N皇后的数组(从0开始存)以及N的大小
{
	int size_set = 10;
	if (length <= 4) {
		printf("棋盘太小！\n");
		return -4;
	}
	if (length <= size_set) {			//小棋盘二维打印
		for (int i = 0; i < length; i++) {
			for (int j = 0; j < length; j++) {
				if (j == nqueen_array[i]) {
					printf("* ");
				}
				else {
					printf("0 ");
				}
			}
			printf("\n");
		}
	}
	else {
		for (int i = 0; i < length; i++) {
			printf("%d\n", nqueen_array[i]);
		}
	}
	for (int i = 0; i < length; i++) {
		if (nqueen_array[i] < 0 || nqueen_array[i] >= length) {
			printf("皇后摆出棋盘了\n");
			return -3;
		}
		for (int j = i + 1; j < length; j++) {
			if (nqueen_array[i] == nqueen_array[j]) {
				printf("直线冲突\n");
				return -2;
			}
			if (abs(nqueen_array[i] - nqueen_array[j]) == abs(i - j)) {		//abs需要include<stdlib.h>
				printf("斜线冲突\n");
				return -1;
			}
		}
	}
	printf("N皇后的解正确\n");
	return 1;
}