#include<stdio.h>
#include<string.h>
#define maxn 210
#define precision 200

typedef struct frac{
    int a; //����
    int b; //��ĸ����������������
    int isNegative; //�Ƿ��Ǹ���,1��0��
}Frac;

typedef struct HighAccuracyNum{
    int Integer; //��������
    int Decimal[maxn]; //С�����֣�Decimal[0]��ʮ��λ��Decimal[1]�ǰٷ�λ
    int base; //����
    int isNegative;
}Number;

void init(Number *InitNum){//��ʼ�����
    InitNum->Integer = 0;
    memset(InitNum->Decimal,0,sizeof(InitNum->Decimal));
    InitNum->base = 0;
    InitNum->isNegative = 0;
}

Number FractoNumber(Frac origin){
    Number result;
    init(&result);
    result.base = 10;
    result.isNegative = origin.isNegative;
    result.Integer = origin.a/origin.b; //�����������
    origin.a %= origin.b;
    for(int i=0;i<precision;i++){
        origin.a *= 10; //ģ����ʽ����
        result.Decimal[i] = origin.a/origin.b;
        origin.a%=origin.b;
    }
    origin.a *= 10;
    if(origin.a/origin.b >= 5){result.Decimal[precision-1]++;}//���һλ��������
    return result;
}

int main(){
    Frac x = (Frac){5,3,0};
    Number res = FractoNumber(x);
}
