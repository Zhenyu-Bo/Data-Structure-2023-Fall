#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//定义串的顺序存储结构
#define MAX_STRLEN 256
typedef struct{
    char str[MAX_STRLEN];
    int len;
} SString;
//定义串的堆分配存储结构
typedef struct{
    char *ch;
    int len;
} HString;


/*** 以下为堆分配存储的ADT ***/

//初始化堆分配存储的串
void InitHStr(HString *S)
{
    S->ch = (char *)malloc(sizeof(char)*MAX_STRLEN);
    if(!S->ch){
        printf("Error!\n");
        exit(0);
    }
    S->len = 0;
}

//销毁串
void DestroyHStr(HString *S)
{
    free(S);
}

//清空串
void ClearHStr(HString *S)
{
    S->len = 0;
}

//求串长
int StrLength(HString *S)
{
    return S->len;
}

//求子串
int SubStr(HString *S, int start, int length, HString *Sub)
{
    if(start < 0 || start + length > S->len){
        printf("Error!\n");
        return 0;
    }
    int i,j;
    for(i=0;i < length;i++)
    {
        Sub->ch[i] = S->ch[i+start];
    }
    Sub->len = length;
    return 1;
}

//拼接串
int StrConcat(HString *s1, HString *s2, HString *T)
{
    int i,j;
    //if(T->ch)   InitHStr(T);//释放旧空间
    //t_len = s1->len + s2->len;
    //如果s1,s2的长度之和大于MAX_STRLEN，则为T->ch分配更大空间
    if(s1->len + s2->len > MAX_STRLEN)
    {
        char *new_base = (char *)realloc(T->ch,(s1->len+s2->len)*sizeof(char));
        if(!new_base){
            printf("空间不足,新空间分配失败!\n");
            return 0;
        }
        T->ch = new_base;
    }
    T->len = s1->len + s2->len;
    for(i=0;i < s1->len;i++)
    {
        T->ch[i] = s1->ch[i];
    }
    for(j=0;j < s2->len;j++)
    {
        T->ch[s1->len+j] = s2->ch[j];
    }
    return 1;
}

//遍历打印串
void TraverseStr(HString *S)
{
    if(!S->len){
        printf("空串!\n");
        //return ;
    }
    else{
        for(int i = 0;i < S->len;i++)
            printf("%c",S->ch[i]);
        printf("\n");
    }
    
}

//KMP模式匹配

//计算next数组
void get_next(HString *S,int next[])
{
    int i,j;
    memset(next,0,S->len*sizeof(int));
    next[0] = -1;
    next[1] = 0;
    for(i=2;i<S->len;i++)
    {
        for(j=i-2;j>=0;j--)
        {
            if(S->ch[j] == S->ch[i-1])
            {
                if(strncmp(S->ch,&(S->ch[i-j-1]),j+1)==0)//strncmp(p1,p2,n),比较p1和p2的前n个字符是否相同，相同则返回0
                    next[i] = j + 1;
            }
        }
    }
    //return next;
}

void get_next_2(HString *S,int next[])
{
    int i = 0,j = -1;
    next[0] = -1;
    while(i < S->len -1)
    {
        if(j == -1 || S->ch[i] == S->ch[j])
        {
            i++; j++;
            next[i] = j;
        }
        else
            j = next[j];
    }
}

//直接计算nextval数组
void get_nextval_1(HString *S,int nextval[])
{
    int j = 0, k = -1;
    nextval[0] = -1;
    while(j < S->len)
    {
        if(k == -1 || S->ch[j] == S->ch[k])
        {
            j++; k++;
            /*** 在求next数组时，这里会直接将k赋值给next[j] 
             *** 而求nextval数组时，则需要考虑能否更进一步
             *** 如果S-ch[j] == S->ch[k],则可以直接回溯至nextval[j]；如果不等，则将k赋值给nextval[j]
             *** ***/
            if(S->ch[j] != S->ch[k])
                nextval[j] = k;
            else
                nextval[j] = nextval[k];
        }
        else
            k = nextval[k];
    }
}

//利用next数组计算nextval数组
void get_nextval_2(HString *S,int nextval[])
{
    int i,j;
    memset(nextval,0,S->len*sizeof(int));
    nextval[0] = -1;
    /*int *next = (int *)malloc(sizeof(int)*S->len);
    if(!next){
        printf("Error!\n");
        return 0;
    }
    next = get_next(S,next);*/
    int next[S->len];
    get_next(S,next);
    for(i=1;i<S->len;i++)
    {
        if(S->ch[i] == S->ch[next[i]])
            nextval[i] = nextval[next[i]];
        else
            nextval[i] = next[i];
    }
    printf("nextval数组为:\n");
    for(i = 0;i < S->len;i++)
    {
        printf("%d ",nextval[i]);
    }
    printf("\n");
}

//利用nextval数组进行KMP模式匹配
int KMP_Match(HString *S, HString *T)
{
    //S和T分别为主串和模式串
    int nextval[T->len], i = 0, j = 0;
    get_nextval_1(T,nextval);
    
    while(i < S->len && j < T->len)
    {
        if(j == -1 || S->ch[i] == T->ch[j])
        {
            i++;
            j++;
        }
        else
            j = nextval[j];
    }
    if(j >= T->len)
        return (i-T->len);//返回主串中第一个模式串的首下标
    else
        return -1;
}

//创建一个堆分配的空串
HString *CreateNullHStr()
{
    HString *S = (HString *)malloc(sizeof(HString));
    if(!S){
        printf("ERROR!");
        return NULL;
    }
    InitHStr(S);
    return S;
}

//将主串S中的所有不重复的模式串T替换为V
void Replace(HString *S, HString *T, HString *V)
{
    int i = KMP_Match(S,T),j = 0;
    if(i == -1)     return ;//递归结束条件
    else
    {
        HString *Sub1 = CreateNullHStr(), *Sub2 = CreateNullHStr(), *temp = CreateNullHStr();
        Sub1->len = i;
        Sub2->len = S->len - T->len - i;
        //Sub1,Sub2分别表示主串在第一个T前和第一个T后的部分，下面对其进行赋值
        for(j = 0;j < S->len;j++)
        {
            if(j < i){
                Sub1->ch[j] = S->ch[j];
            }
            else if(j >= i + T->len){
                Sub2->ch[j - (i + T->len)] = S->ch[j];
            }
        }
        //先处理Sub2，因为Sub2中还可能有子串与模式串相同
        Replace(Sub2,T,V);
        //再将Sub1,V,Sub2拼接起来
        StrConcat(Sub1,V,temp);
        StrConcat(temp,Sub2,S);
    }
}

//创建串
HString *CreateHStr()
{
    HString *S = (HString *)malloc(sizeof(HString));
    if(!S){
        printf("ERROR!\n");
        return NULL;
    }
    InitHStr(S);
    gets(S->ch);
    int i = 0;
    for(i = 0;S->ch[i] != '\0';i++);
    S->len = i;
    //S->len = strlen(S->ch);//sizeof会把串尾标志'\0'也计入，所以要减1
    return S;
}


/*** 以下为顺序存储的ADT ***/

//初始化串
void InitSStr(SString *S)
{
    memset(S->str,'\0',sizeof(S->str));
    S->len = 0;
}

//清空串
void ClearSStr(SString *S)
{
    memset(S->str,'0',S->len);
    S->len = 0;
}

//销毁串
void DestroySStr(SString *S)
{
    free(S);
    S = NULL;
}

//求串长
int SStrLength(SString *S)
{
    return S->len;
}

//求子串
int SubSStr(SString *S, int start,int length,SString *Sub)
{
    if(start < 0 || start + length > S->len)
    {
        printf("Error!\n");
        return 0;
    }
    for(int i = 0;i < length;i++)
    {
        Sub->str[i] = S->str[start + i];
    }
    Sub->len = length;
    return 1;
}

//拼接串
int SStrConcat(SString *s1,SString *s2,SString *T)
{
    if(s1->len + s2->len > MAX_STRLEN)
    {
        //不同于堆分配，顺序存储的串的长度如果超出最大容量，无法进行扩容，只能返回0表示拼接失败
        printf("ERROR!\n");
        return 0;
    }
    T->len = s1->len + s2->len;
    for(int i = 0;i < s1->len;i++)
        T->str[i] = s1->str[i];
    for(int j = 0;j < s2->len;j++)
        T->str[s1->len + j] = s2->str[j];
    return 1;
}

//遍历打印串
void TraverseSStr(SString *S)
{
    if(!S->len)
    {
        printf("空串\n");
    }
    else
    {
        for(int i = 0;i < S->len;i++)
            printf("%c",S->str[i]);
        printf("\n");
    }    
}

//计算nextval数组
void get_nextval_SStr(SString *S,int nextval[])
{
    int j = 0, k = -1;
    nextval[0] = -1;
    while(j < S->len)
    {
        if(k == -1 || S->str[j] == S->str[k])
        {
            j++; k++;
            if(S->str[j] != S->str[k])
                nextval[j] = k;
            else
                nextval[j] = nextval[k];
        }
        else
            k = nextval[k];
    }
}

//KMP模式匹配
int KMP_Match_SStr(SString *S,SString *T)
{
    //S和T分别为主串和模式串
    int nextval[T->len], i = 0, j = 0;
    get_nextval_SStr(T,nextval);
    while(i < S->len && j < T->len)
    {
        if(j == -1 || S->str[i] == T->str[j])
        {
            i++;
            j++;
        }
        else
            j = nextval[j];
    }
    if(j >= T->len)
        return (i-T->len);//返回主串中第一个模式串的首下标
    else
        return -1;
}

SString *CreateNullSStr()
{
    SString *S = (SString *)malloc(sizeof(SString));
    if(!S)
    {
        printf("Error!\n");
        return NULL;
    }
    InitSStr(S);
    return S;
}

//将主串S中的所有不重复的模式串T替换为V
void Replace_SStr(SString *S,SString *T,SString *V)
{
    int i = KMP_Match_SStr(S,T), j = 0;
    if(i == -1)     return ;
    else
    {
        SString *Sub1 = CreateNullSStr(), *Sub2 = CreateNullSStr(), *temp = CreateNullSStr();
        Sub1->len = i;
        Sub2->len = S->len - T->len - i;
        //Sub1,Sub2分别表示主串在第一个T前和第一个T后的部分，下面对其进行赋值
        for(j = 0;j < S->len;j++)
        {
            if(j < i){
                Sub1->str[j] = S->str[j];
            }
            else if(j >= i + T->len){
                Sub2->str[j - (i + T->len)] = S->str[j];
            }
        }
        //先处理Sub2，因为Sub2中还可能有子串与模式串相同
        Replace_SStr(Sub2,T,V);
        //再将Sub1,V,Sub2拼接起来
        SStrConcat(Sub1,V,temp);
        SStrConcat(temp,Sub2,S);
    }
}

SString *CreateSStr()
{
    SString *S = (SString *)malloc(sizeof(SString));
    if(!S){
        printf("Error!\n");
        return NULL;
    }
    InitSStr(S);
    gets(S->str);
    int count = 0;
    while(S->str[count++] != '\0');
    S->len = count - 1;
    return S;
}



//测试堆分配存储的串的各种操作
void HStrTest()
{
    int n1,n2,n3;
    printf("请依次输入主串,模式串,替换串:\n");
    
    HString *S = CreateHStr();
    //getchar();
    HString *T = CreateHStr();
    //getchar();
    HString *V = CreateHStr();
    //getchar();
    HString *temp = CreateNullHStr();
    /*printf("请输入各串的长度:\n");
    scanf("%d%d%d",&n1,&n2,&n3);
    S->len = n1;T->len = n2;V->len = n3;*/
    
    printf("主串为:\n");
    TraverseStr(S);
    printf("主串长度为%d\n",StrLength(S));
    printf("请输入欲求子串起始位置及长度:\n");
    int start,length;
    scanf("%d%d",&start,&length);
    if(SubStr(S,start,length,temp)){
        printf("子串:");
        TraverseStr(temp);
    }
    printf("主串与模式串拼接后的结果为:\n");
    StrConcat(S,T,temp);
    TraverseStr(temp);
    int pos = KMP_Match(S,T);
    if(pos != -1)
        printf("模式串在主串中的第一次出现的位置为:%d\n",pos);
    else
        printf("主串中没有模式串\n");

    printf("替换后的结果为:\n");
    Replace(S,T,V);
    TraverseStr(S);

    DestroyHStr(S);
    DestroyHStr(T);
    DestroyHStr(V);
}


void SStrTest()
{
    printf("请依次输入主串,模式串,替换串:\n");
    SString *S = CreateSStr();
    //getchar();
    SString *T = CreateSStr();
    //getchar();
    SString *V = CreateSStr();
    //getchar();
    SString *temp = CreateNullSStr();
    
    printf("主串为:\n");
    TraverseSStr(S);
    printf("主串长度为%d\n",SStrLength(S));
    printf("请输入欲求子串起始位置及长度:\n");
    int start,length;
    scanf("%d%d",&start,&length);
    if(SubSStr(S,start,length,temp)){
        printf("子串:");
        TraverseSStr(temp);
    }
    printf("主串与模式串拼接后的结果为:\n");
    SStrConcat(S,T,temp);
    TraverseSStr(temp);
    int pos = KMP_Match_SStr(S,T);
    if(pos != -1)
        printf("模式串在主串中的第一次出现的位置为:%d\n",pos);
    else
        printf("主串中没有模式串\n");

    printf("替换后的结果为:\n");
    Replace_SStr(S,T,V);
    TraverseSStr(S);

    DestroySStr(S);
    DestroySStr(T);
    DestroySStr(V);
}

int main()
{
    HStrTest();
    /*** 这里有个问题：不加getchar()时，SStrTest第一次读入串会读入一个空串，这是为什么？
     *** HStrTest的最后一个函数是DestroyHStr函数，最后一次打印的是TraverseStr函数打印的'\n'
     *** 为什么会读入全是'\0'的空串？ 
     ***/
    getchar();
    SStrTest();
    system("pause");
    return 0;
}