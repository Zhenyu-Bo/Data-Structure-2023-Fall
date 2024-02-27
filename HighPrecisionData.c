#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

#define precision 200
#define maxn precision*2
#define maxn_integ 10

//定义高精度数结构体
typedef struct{
    int Integer;//定义整数部分
    int IntArray[maxn_integ];//定义用来存储整数部分的数组
    int Decimal[maxn];//定义小数部分，存放在数组里
    int base;//进制
    int isNegative;//是否为负数，表示符号位
} Number;

//定义分数结构体
typedef struct{
    int a;//分子
    int b;//分母
    int isNegative;//符号位
} Frac;

/*//定义多项式结构体
typedef struct {
    Number a;//系数
    int e;//指数
} Polyn;*/

//初始化生成一个高精度数
void InitNum(Number *N)
{
    N->Integer=0;
    N->base=10;
    N->isNegative=0;
    memset(N->Decimal,0,sizeof(N->Decimal));
    memset(N->IntArray,0,sizeof(N->IntArray));
    //return N;
}

Number *CreateNum()
{
    Number *Result= (Number *)malloc(sizeof(Number));
    if(!Result){
        printf("Error\n");
        return NULL;
    }
    InitNum(Result);
    return Result;
}

Number *UnsignedSubtract(Number *N1,Number *N2)//该函数只计算数值位相减的结果，且N1的数值位大于N2的数值位
{
    Number *Result=CreateNum();
    int i=maxn,cout=0;//cout表示借位，初始化为0
    //InitNum(Result);//初始化Result
    while(--i>=0)
    {
        Result->Decimal[i]=N1->Decimal[i]-N2->Decimal[i]-cout;//计算当前位的差
        cout=0;
        if(Result->Decimal[i]<0)
            cout=(-Result->Decimal[i])/10+1;//计算借位
        Result->Decimal[i]+=cout*10;//计算当前位的最终结果
    }
    Result->Integer=N1->Integer-N2->Integer-cout;//计算整数部分
    return Result;
}

//定义高精度数加法函数，在十进制下进行
Number *Add(Number *N1, Number *N2)
{
    int i=0,cout=0;//cout表示进位,初始化为0
    Number *Result=CreateNum();
    //根据符号位分类计算
    if(N1->isNegative!=N2->isNegative)
    {
        if(N1->Integer!=N2->Integer)
        {
            if(N1->Integer>N2->Integer){
                Result=UnsignedSubtract(N1,N2);
                Result->isNegative=N1->isNegative;
            }

            else{
                Result=UnsignedSubtract(N2,N1);
                Result->isNegative=N2->isNegative;
            }
        }       
        else{
            for(i=0;i<maxn;i++){
                if(N1->Decimal[i]>N2->Decimal[i]){
                    Result=UnsignedSubtract(N1,N2);
                    Result->isNegative=N1->isNegative;
                    break;
                }
                else if(N1->Decimal[i]<N2->Decimal[i]){
                    Result=UnsignedSubtract(N2,N1);
                    Result->isNegative=N2->isNegative;
                    break;
                }
                //如果循环中没有对Result赋值，说明N1=N2,最终返回的Result为初始化后生成的十进制的0
            }    
        }
    }
    else
    {
        Result->isNegative=N1->isNegative;//Result符号位与两加法项相同
        for(i=maxn-1;(N1->Decimal[i]==0&&N2->Decimal[i]==0);i--);//找到最后一位非零位
        while(i>=0)
        {
            Result->Decimal[i]=N1->Decimal[i]+N2->Decimal[i]+cout;//计算当前位的和
            cout=Result->Decimal[i]/10;//计算进位
            Result->Decimal[i]%=10;//取余数
            i--;
        }
        Result->Integer=N1->Integer+N2->Integer+cout;//计算整数部分
    }
    return Result;
}

//定义减法函数，N1-N2=N1+(-N2)
Number *Subtract(Number *N1,Number *N2)
{
    N2->isNegative=!N2->isNegative;
    return Add(N1,N2);
}

//将十进制数乘以n
Number *Multi(Number *N,int n)
{
    Number *Result=CreateNum();
    if(n==0)    return Result;//n=0时直接返回0
    /*if(n==1){
        Result=N;
        return Result;
    }*/
    Result->isNegative=N->isNegative;
    int i=maxn-1,cout=0;//cout表示进位
    while(N->Decimal[i--]==0);
    //从最后一位非零位开始计算
    for(i++;i>=0;i--)
    {
        Result->Decimal[i]=N->Decimal[i]*n+cout;//计算当前位的值
        cout=Result->Decimal[i]/10;//计算进位
        Result->Decimal[i]%=10;//取余数
    }
    Result->Integer=cout+N->Integer*n;
    return Result;
}

//模拟竖式乘法计算高精度数乘法
Number *Multiply(Number *N1,Number *N2)
{
    Number *Result=CreateNum();
    //Result=Multi(N1,N2->Integer);
    Result->isNegative=(N1->isNegative==N2->isNegative)?0:1;
    int i=0,j=0,cout=0;
    //模拟竖式乘法,计算N1,N2小数部分相乘的结果
    for(i=1;i<=precision;i++)
    {
        for(j=1;j<=i;j++)
        {
            Result->Decimal[maxn-i]+=N2->Decimal[precision-j]*N1->Decimal[precision-(i+1-j)];
        }
        Result->Decimal[maxn-i]+=cout;
        cout=Result->Decimal[maxn-i]/10;//计算进位
        Result->Decimal[maxn-i]%=10;//取余数
    }
    for(i=precision+1;i<=maxn-1;i++)
    {
        for(j=0;j<=maxn-i-1;j++)
        {
            Result->Decimal[maxn-i]+=N2->Decimal[j]*N1->Decimal[maxn-i-1-j];
        }
        Result->Decimal[maxn-i]+=cout;
        cout=Result->Decimal[maxn-i]/10;//计算进位
        Result->Decimal[maxn-i]%=10;//取余数
    }
    Result->Decimal[0]=cout;
    //再加上整数部分相乘和整数部分与小数部分相乘的结果
    Number *n1,*n2;
    n1=N1;n2=N2;
    n1->Integer=0;n2->Integer=0;//n1,n2分别表示N1,N2的小数部分
    if(N2->Integer)
        Result=Add(Result,Multi(n1,N2->Integer));//将N1的整数部分与N2的小数部分相乘再加到结果中
    if(N1->Integer)
        Result=Add(Result,Multi(n2,N1->Integer));//将N2的整数部分与N1的小数部分相乘再加到结果中
    Result->Integer+=N1->Integer*N2->Integer;//将N1，N2的整数部分相乘加到结果中
    return Result;
}

//将高精度数后移n位，即将其除以10^n
Number *NumMove(Number *N,int n)
{
    int i,j;
    for(j=precision-1;j>=0&&N->Decimal[j]==0;j--);//找到最后一位非零位
    for(i=j;i>=0;i--)
    {
        N->Decimal[i+n]=N->Decimal[i];
    }
    for(i=n-1;i>=0;i--)
    {
        N->Decimal[i]=N->Integer%10;
        N->Integer/=10;
    }
    return N;
}

//将其中一个数的每一位数分别与另一位相乘最后加到一起
Number *Multiply_2(Number *N1,Number *N2)
{
    Number *Result=CreateNum(),*N=CreateNum();
    Result=Multi(N1,N2->Integer);
    int sign=(N1->isNegative==N2->isNegative)?0:1;//先记录符号位
    N1->isNegative=0;N2->isNegative=0;Result->isNegative=0;//接下来将两数绝对值相乘
    int i=0,j=0;
    for(i=maxn-1;N2->Decimal[i]==0&&i>=0;i--);//找到N2小数部分的最后一位非零位
    if(i<0) return Result;
    else
    {
        for(j=0;j<=i;j++)
        {
            //将N1与N2小数部分的第j+1位相乘后，后移j+1位，再加到Result中
            N=Multi(N1,N2->Decimal[j]);
            N=NumMove(N,j+1);
            Result=Add(Result,N);
        }
    }
    Result->isNegative=sign;
    return Result;
}

//分数转高精度数
Number *FracToNum(Frac origin)
{
    Number *result=CreateNum();
    //InitNum(result);
    result->base = 10;
    result->isNegative = origin.isNegative;
    result->Integer = origin.a/origin.b; 
    origin.a %= origin.b;
    for(int i=0;i<precision;i++){
        origin.a *= 10; 
        result->Decimal[i] = origin.a/origin.b;
        origin.a%=origin.b;
    }
    origin.a *= 10;
    if(origin.a/origin.b >= 5){result->Decimal[precision-1]++;}
    return result;
}

//十进制数转为N进制数
Number *DecToN(Number *Dec,int base)
{
    Number *N=CreateNum();
    N->base=base;
    N->isNegative=Dec->isNegative;
    int i,Dec_Int=Dec->Integer;
    //十进制整数部分转为N进制
    for(i=1;Dec_Int;i++)
    {
        N->IntArray[maxn_integ-i]=Dec_Int%base;
        Dec_Int/=base;
    }
    if(base==10)    N->Integer=Dec->Integer;
    //十进制数小数部分转为N进制
    Number *D=Dec;
    D->Integer=0;
    //Number *temp=Multiply(D,Two);
    for(i=0;i<maxn;i++)
    {
        D=Multi(D,base);
        N->Decimal[i]=D->Integer;
        D->Integer=0;
    }
    free(D);
    return N;
}

//将字符串转化为base进制数
Number *StrToNum(char *str,int base)
{
    Number *N=CreateNum();
    N->base=base;
    if(str[0]=='-')     N->isNegative=1;
    int i=0,j=0,count=0;
    char s[maxn_integ]={'\0'};
    for(count=0;str[count]!='.'&&str[count]!='\0';count++);//计算整数位宽度为count
    for(i=0;i<count-N->isNegative;i++,j++)
    {
        N->IntArray[maxn_integ-i-1]=str[count-i-1]-'0';//将str的整数部分转化到N->IntArray中
        s[i]=str[i+N->isNegative];
    }
    i=i+N->isNegative+1;//跳过小数点
    if(base==10)
        N->Integer=atoi(s);//N为十进制数时存储N->Integer
    for(int j=0;str[i]!='\0';i++,j++)
        N->Decimal[j]=str[i]-'0';//处理小数部分
    if(base!=10)
        DecToN(N,base);//将十进制数转化为base进制
    return N;
}

//十进制数转化为二进制数，调用十进制转N进制即可
Number *DecToBin(Number *Dec)
{
    return DecToN(Dec,2);
}

//二进制转十进制
Number *TwoToDec(Number *Bin)
{
    Number *Dec=CreateNum();
    //Dec->isNegative=Bin->isNegative;
    int i=0;
    while(Bin->IntArray[i++]==0);//找到第一个非零项的位置
    //转换整数部分
    for(i--;i<maxn_integ;i++)
    {
        Dec->Integer+=Bin->IntArray[i]*pow(2,maxn_integ-1-i);
    }
    //二进制小数转化为十进制小数
    //先生成存储2^(-n)的数组
    Number *Half_n[maxn],*H=CreateNum();
    for(i=0;i<maxn;i++) 
        Half_n[i]=CreateNum();//初始化指针数组的所有元素
    //InitNum(H);InitNum(Half_n[0]);
    H->Decimal[0]=5;Half_n[0]->Decimal[0]=5;//初始化H,Half_n[0]表示1/2
    for(i=1;i<maxn;i++)
    {
        //InitNum(Half_n[i]);
        Half_n[i]=Multiply(Half_n[i-1],H);
    }
    for(i=0;i<maxn;i++)
    {
        if(Bin->Decimal[i])
        {
            //如果某小数位上的数字不为0，则将其对应的2^(-i)加到Dec上
            Dec=Add(Dec,Half_n[i]);
        }
    }
    for(i=0;i<maxn;i++) free(Half_n[i]);//释放空间
    //最后再赋值符号位
    Dec->isNegative=Bin->isNegative;
    return Dec;
}

//判断字符是不是数字
int isNumber(char s)
{
    if(s>='0'&&s<='9')
        return 1;
    else
        return 0;
}

//判断高精度数是否为0
int NumEqu0(Number *N)
{
    if(N->Integer!=0)   return 0;
    else{
        for(int i=0;i<maxn;i++)
        {
            if(N->Decimal[i]!=0)   return 0;
        }
    }
    return 1;
}

//读取多项式并运算
Number *Res(char *str,Number *x)
{
    Number *Result=CreateNum(),*N1=CreateNum(),*N2=CreateNum(),*N=CreateNum();
    //InitNum(Result);InitNum(N1);InitNum(N2);
    Number *coefficient[10];//存储系数
    Number *operand[10];//存储x^n的结果
    int i=0,j=0,k=0,c=0,o=0,sign=0;
    Frac origin;
    char s[maxn]={'\0'};
    //初始化两数组
    for(i=0;i<10;i++)
    {
        coefficient[i]=CreateNum();
        operand[i]=CreateNum();
        operand[i]->Integer=1;
    }
    for(i=0;str[i]!='\0';)
    {
        if(str[i]=='x')
        {
            i++;
            operand[o]=x;
            if(str[i]=='^')
            {
                i++;
                for(j=1;j<str[i]-'0';j++){//注意str[i]是字符，不是数字，需要减去'0'
                    operand[o]=Multiply_2(operand[o],x);
                }
                i++;
            }
            o++;    
        }
        else if(str[i]=='-'){
            coefficient[c]->isNegative=1;
            i++;
        }
        else if(isNumber(str[i])){
            j=0;
            for(;isNumber(str[i+j])||str[i+j]=='.';j++)
                s[j]=str[i+j];
            i=i+j;
            sign=coefficient[c]->isNegative;//先保存coefficient[c]的符号位
            coefficient[c]=StrToNum(s,10);
            coefficient[c]->isNegative=sign;
            //coefficient[c]=StrToNum(s,10);
            memset(s,0,sizeof(s));//将s转化为高精度数之后清空s以便后续使用
            if(str[i]=='/'){
                i++;
                //将分数转化为高精度数
                for(k=0;isNumber(str[i+k]);k++)
                    s[k]=str[i+k];
                i=i+k;
                origin.a=coefficient[c]->Integer;
                origin.b=atoi(s);
                origin.isNegative=coefficient[c]->isNegative;
                coefficient[c]=FracToNum(origin);
            }
            c++;
        }
        else    i++;
    }
    for(i=0;!(NumEqu0(coefficient[i]));i++)
    {
        N=Multiply_2(coefficient[i],operand[i]);
        Result=Add(Result,N);
    }
    return Result;
}

//打印高精度数
void PrintNum(Number *N)
{
    if(N->isNegative)   printf("-");
    int i=0,j=0;
    //打印整数部分
    if(N->base==10)
        printf("%d",N->Integer);
    else
    {
        //先找到第一个非零项的位置
        for(i=0;i<maxn_integ&&N->IntArray[i]==0;i++);
        if(i==maxn_integ) printf("0");
        else{
            for(;i<maxn_integ;i++){
                printf("%d",N->IntArray[i]); 
                if(N->base>10)  printf(" ");//为了便于观看，进制大于10时用空格隔开打印
            }       
        }
    }
    printf(".");
    for(j=precision-1;j>=0&&N->Decimal[j]==0;j--);//找到最后一个非零项的位置
    for(i=0;i<=j;i++)
    {
        /*if(i==precision-1&&N->Decimal[i+1]>N->base/2){
            printf("%d",N->Decimal[i]+1);
            break;
        }
        else*/    
            printf("%d",N->Decimal[i]);
        //为了便于观看，进制大于10时用空格隔开打印
        if(N->base>10)  printf(" ");
    }
    printf("\n");
}

int main()
{
    char str1[maxn+maxn_integ]={'\0'},str2[maxn+maxn_integ]={'\0'};
    int n;
    Number *a=CreateNum(),*b=CreateNum(),*c=CreateNum();
    //InitNum(a);InitNum(b);InitNum(c);

    /*printf("十进制与二进制间的转换:\n");
    printf("请输入十进制数a:\n");
    gets(str1);
    a=StrToNum(str1,10);
    memset(str1,'\0',sizeof(str1));//str读取数据后清零以便后续使用
    printf("十进制转二进制:\n");
    PrintNum(DecToBin(a));
    printf("请输入二进制数b:\n");
    gets(str2);
    //b->base=2;
    b=StrToNum(str2,2);
    memset(str2,'\0',sizeof(str2));//str读取数据后清零以便后续使用
    printf("二进制转十进制：\n");
    PrintNum(TwoToDec(b));

    printf("十进制到N进制的转换:\n");
    printf("请输入进制N:\n");
    scanf("%d",&n);getchar();
    printf("请输入十进制数:\n");
    gets(str1);
    a=StrToNum(str1,10);
    memset(str1,'\0',sizeof(str1));//str读取数据后清零以便后续使用
    PrintNum(DecToN(a,n));
    printf("请再次输入进制N及十进制数:\n");
    scanf("%d",&n);getchar();
    gets(str1);
    a=StrToNum(str1,10);
    memset(str1,'\0',sizeof(str1));//str读取数据后清零以便后续使用
    PrintNum(DecToN(a,n));*/

    printf("N进制下的加减乘操作:\n");
    printf("请输入进制N:\n");
    scanf("%d",&n);getchar();
    printf("请输入十进制数a,b:\n");
    gets(str1);gets(str2);
    a=StrToNum(str1,10);
    //b->base=10;
    b=StrToNum(str2,10);
    memset(str1,'\0',sizeof(str1));
    memset(str2,'\0',sizeof(str2));

    printf("a+b=\n十进制结果:");
    c=Add(a,b);
    PrintNum(c);
    printf("N进制结果:");
    PrintNum(DecToN(c,n));
    
    //再调用减法函数时，会改变b的符号，所以需要先保存b的符号
    int sign=b->isNegative;
    printf("a-b=\n十进制结果:");
    c=Subtract(a,b);
    b->isNegative=sign;
    PrintNum(c);
    printf("N进制结果:");
    PrintNum(DecToN(c,n));

    printf("a*b=\n十进制结果:");
    c=Multiply_2(a,b);
    PrintNum(c);
    printf("N进制结果:");
    PrintNum(DecToN(c,n));
    
    printf("单变量多项式求值:\n");
    printf("请输入多项式：\n");
    gets(str1);
    printf("请输入x的值:\n");
    gets(str2);
    c=StrToNum(str2,10);
    //PrintNum(c);
    c=Res(str1,c);
    PrintNum(c);
    memset(str1,'\0',sizeof(str1));
    memset(str2,'\0',sizeof(str2));
    printf("请再次输入多项式及x的值:\n");
    gets(str1);gets(str2);
    c=StrToNum(str2,10);
    c=Res(str1,c);
    PrintNum(c);
    memset(str1,'\0',sizeof(str1));
    memset(str2,'\0',sizeof(str2));

    system("pause");
    return 0;
}