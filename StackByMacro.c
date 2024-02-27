#include<stdio.h>
#include<stdlib.h>

#define STACK_INIT_SIZE 10//初始栈大小
#define STACKINCREMENT 10//栈满时增加的空间大小

typedef int SElemType;//定义SElemType为int类型

//先使用全局变量顺序整数栈的静态结构
unsigned int_stacksize;//栈预分配存储空间大小
int *int_stack_top;//栈顶指针
int *int_stack_base;//栈底指针

//类似地，再定义顺序字符栈
unsigned char_stacksize;
char *char_stack_top;//栈顶指针
char *char_stack_base;//栈底指针
//再定义顺序结构体栈
typedef struct Node{
    int num;
    char letter;
} SNode;
unsigned SNode_stacksize;
SNode *SNode_stack_top;//栈顶指针
SNode *SNode_stack_base;//栈底指针

//使用宏定义实现ADT，在宏定义中换行时需加'\'

//初始化生成栈
#define InitStack(ElemType) ElemType ## _stack_base=(ElemType *)malloc(sizeof(ElemType)*STACK_INIT_SIZE);\
                            if(!ElemType ## _stack_base)\
                            {\
                                printf("Stack overflow!");\
                                exit(0);\
                            }\
                            ElemType ## _stack_top=ElemType ## _stack_base;\
                            ElemType ## _stacksize=STACK_INIT_SIZE
//注意若宏中最后一句不加分号则作为函数使用时要加分号，若加了分号作为函数使用时则不能再加分号

//判断栈是否为空
#define StackEmpty(ElemType) (ElemType ## _stack_top==ElemType ## _stack_base)?1:0

//入栈操作
#define PushStack(ElemType,e) \
if(ElemType ## _stack_top-ElemType ## _stack_base>=STACK_INIT_SIZE)\
{\
    printf("栈已满!现重新分配并延伸空间\n");\
    ElemType *temp=(ElemType *)realloc(ElemType ## _stack_base,sizeof(ElemType)*(STACK_INIT_SIZE+STACKINCREMENT));\
    if(!temp)\
    {\
        printf("为满栈重新分配空间失败！\n");\
        exit(0);\
    }\
    printf("成功重新分配并延伸空间!\n");\
    ElemType ## _stack_base=temp;\
    ElemType ## _stack_top=ElemType ## _stack_base+(ElemType ## _stacksize);\
    ElemType ## _stacksize+=STACKINCREMENT;\
}\
*(ElemType ## _stack_top++)=e

//出栈操作,若栈空则返回0，否则返回1并用e带回返回值，这里可利用逗号表达式防止出栈值为0时返回0
//因为使用的是宏定义直接对变量操作，所以e不需要再用指针
#define PopStack(ElemType,e) (StackEmpty(ElemType))?0:(e=*(--ElemType ## _stack_top),1)

//取栈顶元素,与出栈操作类似，不同点是未改变栈
#define GetTop(ElemType,e) StackEmpty(ElemType)?0:(e=*(ElemType ## _stack_top-1),1)

//求栈长,直接用栈顶指针减去栈底指针即可
#define StackLength(ElemType) ElemType ## _stack_top-ElemType ## _stack_base

//清空栈,直接将栈底指针赋值给栈顶指针即可
#define ClearStack(ElemType) ElemType ## _stack_top=ElemType ## _stack_base

//销毁栈，直接用free函数释放空间即可
#define DestroyStack(ElemType) free(ElemType ## _stack_base)

//遍历栈
#define TraverseStack(ElemType,func,ptr) \
printf("表长=%d\n",StackLength(ElemType));\
ptr=ElemType ## _stack_top-1;\
while(ptr+1!=ElemType ## _stack_base)\
{\
    func(ptr);\
    ptr--;\
}\
printf("\b\b  \n")

//打印栈的元素
#define PrintIntElem(ptr)  printf("%d->",*ptr)
#define PrintCharElem(ptr)  printf("%c->",*ptr)
#define PrintSNodeElem(ptr) printf("%d--%c->",ptr->num,ptr->letter)

//三种类型的栈都写一个各种栈操作的函数
void int_stack_check()
{
    printf("检查整数栈操作:\n");
    //生成整数栈
    InitStack(int);
    if(StackEmpty(int)){
        printf("成功生成非空整数栈\n");
    }
    int i;
    //检查空栈出栈及取栈顶元素是否报错
    if(!PopStack(int,i)){
        int_stack_top++;//因为不论能否出栈，栈顶指针都会下移，所以若出栈失败，需要将栈顶指针上移
        printf("空栈!无法出栈!\n");
    }
    if(!GetTop(int,i))
        printf("空栈!无法取栈顶元素!\n");
    for(i=1;i<=STACK_INIT_SIZE;i++)
    {
        //任意插入几个元素
        PushStack(int,i*2);
    }
    //现在栈已满，再入栈一次测试此时能否成功入栈
    PushStack(int,i*2);
    //遍历栈并打印
    int *ptr;
    if(!StackEmpty(int))
        TraverseStack(int,PrintIntElem,ptr);
    //取栈顶元素
    if(GetTop(int,i))
    {
        printf("成功取得栈顶元素%d\n",i);
    }
    //出栈
    if(PopStack(int,i))
    {
        printf("元素%d成功出栈\n",i);
    }
    //再次遍历栈并打印以验证是否已成功出栈
    TraverseStack(int,PrintIntElem,ptr);
    //清空栈
    ClearStack(int);
    if(StackEmpty(int))
        printf("成功清空整数栈\n");
    //销毁栈
    DestroyStack(int);
    printf("整数栈已检查完毕\n");
}

void char_stack_check()
{
    printf("检查字符栈操作\n");
    //生成字符栈
    InitStack(char);
    
    if(StackEmpty(char)){
        printf("成功生成非空字符栈\n");
    }
    char i;
    //检查空栈出栈及取栈顶元素是否报错
    if(!PopStack(char,i)){
        char_stack_top++;//因为不论能否出栈，栈顶指针都会下移，所以若出栈失败，需要将栈顶指针上移
        printf("空栈!无法出栈!\n");
    }
    if(!GetTop(char,i))
        printf("空栈!无法取栈顶元素!\n");
    for(i='a';i<'a'+10;i++)
    {
        //任意插入几个元素
        PushStack(char,i);
    }
    //现在栈已满，再入栈一次测试此时能否成功入栈
    PushStack(char,i);
    //遍历栈并打印
    char *ptr;
    if(!StackEmpty(char))
        TraverseStack(char,PrintCharElem,ptr);
    //取栈顶元素
    if(GetTop(char,i))
    {
        printf("成功取得栈顶元素%c\n",i);
    }
    //出栈
    if(PopStack(char,i))
    {
        printf("元素%c成功出栈\n",i);
    }
    //再次遍历栈并打印以验证是否已成功出栈
    TraverseStack(char,PrintCharElem,ptr);
    //清空栈
    ClearStack(char);
    if(StackEmpty(char))
        printf("成功清空字符栈\n");
    //销毁栈
    DestroyStack(char);
    printf("字符栈已检查完毕\n");
}

void SNode_stack_check()
{
    printf("检查结构体栈操作\n");
    //生成字符栈
    InitStack(SNode);
    
    if(StackEmpty(SNode)){
        printf("成功生成非空结构体栈\n");
    }
    SNode i;
    //检查空栈出栈及取栈顶元素是否报错
    if(!PopStack(SNode,i)){
        SNode_stack_top++;//因为不论能否出栈，栈顶指针都会下移，所以若出栈失败，需要将栈顶指针上移
        printf("空栈!无法出栈!\n");
    }
    if(!GetTop(SNode,i))
        printf("空栈!无法取栈顶元素!\n");
    for(i.num=1,i.letter='a';i.num<=10;i.num++,i.letter++){
        //任意插入几个元素
        PushStack(SNode,i);
    }
    //现在栈已满，再入栈一次测试此时能否成功入栈
    PushStack(SNode,i);
    //遍历栈并打印
    SNode *ptr;
    if(!StackEmpty(SNode))
        TraverseStack(SNode,PrintSNodeElem,ptr);
    //取栈顶元素
    if(GetTop(SNode,i))
    {
        printf("成功取得栈顶元素%d--%c\n",i.num,i.letter);
    }
    //出栈
    if(PopStack(SNode,i))
    {
        printf("元素%d--%c成功出栈\n",i.num,i.letter);
    }
    //再次遍历栈并打印以验证是否已成功出栈
    TraverseStack(SNode,PrintSNodeElem,ptr);
    //清空栈
    ClearStack(SNode);
    if(StackEmpty(SNode))
        printf("成功清空结构体栈\n");
    //销毁栈
    DestroyStack(SNode);
    printf("结构体栈已检查完毕\n");
}

int main()
{
    int_stack_check();
    putchar('\n');
    char_stack_check();
    putchar('\n');
    SNode_stack_check();
    system("pause");
    return 0;
}