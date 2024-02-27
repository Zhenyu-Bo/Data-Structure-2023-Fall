#include<stdio.h>
#include<stdlib.h>
typedef struct Lnode{
    int data;
    struct Lnode *next;
} LNode,*Linklist;

//初始化链表，创建链表头结点，不存储元素
Linklist initList()
{
    LNode *head=(LNode *)malloc(sizeof(LNode));//为头结点分配空间
    if(!head)
    {
        //若未成功分配空间，则退出程序
        printf("初始化分配头结点失败！");
        exit(0);
    }
    head->next=NULL;
    return head;
}

//创建一个结点
Linklist LNodeCreate()
{
    LNode *p=(LNode *)malloc(sizeof(LNode));
    if(!p)
    {
        //若未能成功生成新结点则返回空
        return NULL;
    }
    else
    {
        p->next=NULL;
        return p;
    }
}

//创建链表
Linklist CreateList(int n)
{
    LNode *head=initList();//初始化头结点
    printf("请输入链表数据:\n");
    for(int i=0;i<n;i++)
    {
        Linklist pnew=(LNode *)malloc(sizeof(LNode));//为新结点分配空间
        if(!pnew)
        {
            printf("为新结点分配空间失败!\n");
            exit(0);
        }
        scanf("%d",&pnew->data);
        //头插法创建链表
        pnew->next=head->next;
        head->next=pnew;
    }
    return head;
}

//打印链表
void PrintList(Linklist L)
{
    LNode *p=L;
    while(p=p->next)
    {
        printf("%d->",p->data);
        //p=p->next;
    }
    printf("\b\b  \n");
    //\b是退格符，删去最后多余的->,->为两个符号，故需加两个空格
}

//求链表长度
int ListLength(Linklist p)
{
    int count=0;
    for(p=p->next;p;p=p->next,count++);//循环体为空语句
    return count;
}

//判断链表是否为空
int ListEmpty(Linklist L)
{
    return (L->next==NULL)?1:0;//返回值为1表示链表非空,返回值为0表示链表为空
}

//将链表重置为空表
void ClearList(Linklist L)
{
    LNode *p=L;
    while(p->next){
        //将头结点后的所有结点空间释放
        LNode *q=p->next;
        p->next=q->next;//保存q->next以继续遍历
        free(q);//相当于删除了q指向的结点并释放了空间
    }
}

//销毁链表
void DestroyList(Linklist L)
{
    //先将头结点后的所有结点空间释放，再将头结点空间释放
    ClearList(L);
    free(L);
}

//用e返回第i个数据元素的值
int GetElem(Linklist L,int i,int *e)
{
    LNode *p=L->next;
    int j;
    for(j=1;p;j++,p=p->next)
    {
        if(j==i){
            *e=p->data;
            return j;
        }
    }
    //若for循环中未找到第i个元素，则返回0
    return 0;
}

//寻找数值与e相同的元素下标
Linklist LocateElem(Linklist L,int e)
{
    LNode *p=L;
    while(p=p->next){
        if(p->data==e)
            return p;
        //p=p->next;
    }
    return NULL;//若未找到数值为e的元素，则返回空指针
}

//返回结点直接前驱
Linklist PriorElem(Linklist L,LNode *cur_e)
{
    while(L=L->next){
        if(L->next==cur_e)  return L;
        //L=L->next;
    }
    return NULL;//未找到则返回空指针
}

//返回结点直接后继
Linklist NextElem(Linklist L,LNode *cur_e)
{
    return cur_e->next;
}

//插入结点
Linklist ListInsert(Linklist L,int i,int e)
{
    if(i<1){
        //考虑插入位置小于1的情况
        printf("输入下标不合法");
        return NULL;
    }
    //为新结点分配空间
    LNode *pnew=(LNode *)malloc(sizeof(LNode));
    if(!pnew){
        //考虑分配空间失败的情况
        printf("新结点内存分配失败！");
        exit(0);
    }
    //为新结点赋值
    pnew->data=e;
    pnew->next=NULL;
    LNode *p=L;
    int j;
    for(j=1;p;p=p->next,j++)
    {
        if(j==i)
        {
            pnew->next=p->next;
            p->next=pnew;
            return L;
        }
    }
    return NULL;//若在循环体中未找到插入位置，则返回空指针
}

//利用尾插法插入结点
Linklist TailInsert(Linklist L,int e)
{
    //其实可以直接用ListInsert(L,ListLength(L),e)来实现
    LNode *p=L;
    //找到链表尾结点
    while(p->next)
        p=p->next;
    LNode *q=LNodeCreate();
    if(q)
    {
        q->data=e;
        p->next=q;
        return L;
    }
    return NULL;//若插入失败，则返回空指针
}

//删除L的第i个元素并用e带回被删除元素的数值
int ListDelete(Linklist L,int i,int *e)
{
    //先考虑下标不合法的情况
    if(i<1)
    {
        printf("输入的下标不合法！\n");
        return 0;
    }
    LNode *pre=L,*cur=L->next;
    //寻找待删除结点
    for(int j=1;cur;pre=cur,cur=cur->next,j++)
    {
        if(j==i){
            //找到待删除结点后，先将pre->next指向cur->next，再释放cur的空间
            pre->next=cur->next;
            *e=cur->data;//用e带回删除元素的值
            free(cur);
            return 1;
        }
    }
    return 0;//若在循环体中未找到待删除结点，则返回0
}

//将元素数值加2
void Add2(LNode *p)
{
    p->data+=2;
    //printf("+2,");
}

//打印链表各结点地址和数值
void DebugLinkNode(LNode *p)
{
    printf("结点-(*addr)=value:");
    printf("(*%lx)=%d\n",p,p->data);
}

//遍历链表，并在遍历过程中使用函数指针调用函数
void ListTraverse(Linklist p,void (*ptrFunc)(LNode *ptr))
{
    printf("表长=%d\n",ListLength(p));
    while(p=p->next)
        (*ptrFunc)(p);//遍历链表时利用函数指针可调用各种函数，实现各种功能
    printf("\b\b  \n");//退格符
    //printf("\n");
}

//打印链表元素
void PrintLinkNode(LNode *p)
{
    printf("%d->",p->data);
}

//定义全局变量以方便在函数中使用
void (*ptrFunc)(LNode *p);
int i;
int e;

void CheckInsert(Linklist head)
{
    //任意插入几个结点，构建链表
    for(i=1;i<=5;i++)
    {
        ListInsert(head,i,i*2);//第一次循环即为空链表插入结点
    }
    if(!ListEmpty(head))
        ListTraverse(head,ptrFunc);
    //再利用尾插法在链尾插入一些结点
    for(;i<10;i++)
    {
        TailInsert(head,i+2);
    }
    //也可直接用以下方式在链表插入元素
    ListInsert(head,ListLength(head)+1,555);
    ListTraverse(head,ptrFunc);
    //再在中间位置随机插入几个结点
    ListInsert(head,3,666);
    ListInsert(head,5,777);
    ListInsert(head,7,888);
    ListInsert(head,8,999);
    ListTraverse(head,ptrFunc);
    //测试插入结点位置大于链长时的情况
    if(!ListInsert(head,ListLength(head)+2,10))
        printf("结点插入失败!\n\n");
}

void CheckDelete(Linklist head)
{
    i=1;//删除头结点
    printf("删除前的e值为%d--",e);
    if(ListDelete(head,i,&e)){
        printf("删除的第%d位数据为%d\n",i,e);
    }
    else
        printf("未能成功删除第i位数据%d\n",i);
    //再打印一遍链表
    ListTraverse(head,ptrFunc);
    printf("删除前e的值为%d--",e);
    i=6;
    if(ListDelete(head,i,&e)){
        printf("删除的第%d位数据为%d\n",i,e);
    }
    else
        printf("未能成功删除第%d位数据\n",i);
    //再打印一遍链表
    ListTraverse(head,ptrFunc);
    //测试删除结点位置大于链长时的情况
    if(!ListDelete(head,ListLength(head)+1,&e))
        printf("结点删除失败!\n\n");
}

int main()
{
    //初始化链表，头结点用head表示
    Linklist head=initList();
    if(ListEmpty(head))
        printf("链表为空\n");
    //检验空链表删除元素是否报错,ListDelete报错的方式是返回0
    e=10;
    if(!ListDelete(head,1,&e))
        printf("删除第%d个元素失败\n",1);
    //定义指针函数指向DebugLinkNode
    ptrFunc=DebugLinkNode;
    //再调用指针函数遍历链表
    ListTraverse(head,ptrFunc);
    
    CheckInsert(head);//插入操作写了好多，所以整理到一个函数里面了
    CheckDelete(head);//删除操作同样写在了一个函数里
    
    //测试GetElem函数,读取头结点元素/中间元素/末尾元素
    printf("取数据之前e的值为%d\n",e);
    if(GetElem(head,i=1,&e))
        printf("成功读取第%d位数据%d\n",i,e);
    if(GetElem(head,i=6,&e))
        printf("成功读取第%d位数据%d\n",i,e);
    if(GetElem(head,i=ListLength(head),&e))
        printf("成功读取第%d位数据%d\n\n",i,e);
    //测试LocateElem,PriorElem,NextElem等函数
    LNode *q=LocateElem(head,999);
    if(!q)
    {
        printf("未找到结点\n");
    }
    else{
        q=PriorElem(head,q);
        printf("找到结点的前驱为%d\n找到的结点为%d\n",q->data,q->next->data);
        if(q->next->next){
            printf("找到的结点后继为%d",NextElem(head,NextElem(head,q))->data);
        }
        printf("\n");
    }
    //再用PrintList函数打印一遍链表
    PrintList(head);
    //利用函数指针调用Add2函数
    ptrFunc=Add2;
    printf("每个数据准备+2\n");
    ListTraverse(head,ptrFunc);
    printf("完成+2后,新的链表为:\n");
    //先用PrintList函数打印一遍链表
    PrintList(head);
    //再用ListTraverse遍历链表并依次打印结点
    ListTraverse(head,PrintLinkNode);
    //检验ClearList函数
    ClearList(head);
    if(ListEmpty(head))
        printf("成功清空链表\n");
    //销毁链表，释放空间
    DestroyList(head);
    system("pause");
    return 0;
}