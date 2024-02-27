#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define maxn 210
#define precision 200

typedef struct {
    int Integer;
    int Decimal[maxn];
    int base;
    int isNegative;
} Number;

// 初始化高精度数
void initNumber(Number* num, int base) {
    num->Integer = 0;
    memset(num->Decimal, 0, sizeof(num->Decimal));
    num->base = base;
    num->isNegative = 0;
}

// 字符串转换为高精度数
void stringToNumber(Number* num, char* str) {
    int len = strlen(str);
    int dotPos = -1;
    for (int i = 0; i < len; i++) {
        if (str[i] == '.') {
            dotPos = i;
            break;
        }
    }

    // 处理整数部分
    int multiplier = 1;
    int integerPart = 0;
    for (int i = dotPos - 1; i >= 0; i--) {
        integerPart += (str[i] - '0') * multiplier;
        multiplier *= 10;
    }
    num->Integer = integerPart;

    // 处理小数部分
    multiplier = num->base;
    int decimalPart = 0;
    for (int i = dotPos + 1; i < len && i <= dotPos + precision; i++) {
        decimalPart += (str[i] - '0') * multiplier;
        multiplier /= 10;
    }
    memcpy(num->Decimal, &decimalPart, sizeof(num->Decimal));

    if (str[0] == '-') {
        num->isNegative = 1;
    }
}

// 高精度数转换为字符串
void numberToString(Number* num, char* str) {
    int idx = 0;
    if (num->isNegative) {
        str[idx++] = '-';
    }
    sprintf(&str[idx], "%d", num->Integer);

    int decimalPart = *(int*)num->Decimal;
    if (decimalPart != 0) {
        while (decimalPart % 10 == 0) {
            decimalPart /= 10;
        }
        if (decimalPart != 0) {
            int len = strlen(str);
            str[len] = '.';
            idx = len + 1;

            int multiplier = num->base / 10;
            while (decimalPart > 0 && idx < len + 1 + precision) {
                str[idx++] = decimalPart / multiplier + '0';
                decimalPart %= multiplier;
                multiplier /= 10;
            }
        }
    }

    str[idx] = '\0';
}

// 高精度数比较大小
int compareNumber(Number* num1, Number* num2) {
    if (num1->Integer < num2->Integer) {
        return -1;
    } else if (num1->Integer > num2->Integer) {
        return 1;
    }

    int decimalPart1 = *(int*)num1->Decimal;
    int decimalPart2 = *(int*)num2->Decimal;
    while (decimalPart1 % 10 == 0 && decimalPart2 % 10 == 0) {
        decimalPart1 /= 10;
        decimalPart2 /= 10;
    }

    if (decimalPart1 < decimalPart2) {
        return -1;
    } else if (decimalPart1 > decimalPart2) {
        return 1;
    }

    return 0;
}

// 高精度数加法
Number addNumber(Number* num1, Number* num2) {
    Number sum;
    initNumber(&sum, num1->base);

    // 处理符号位
    if (num1->isNegative && num2->isNegative) {
        sum.isNegative = 1;
    } else if (num1->isNegative && !num2->isNegative) {
        num1->isNegative = 0;
        return SubtractNumber(num2, num1);
    } else if (!num1->isNegative && num2->isNegative) {
        num2->isNegative = 0;
        return SubtractNumber(num1, num2);
    }

    sum.Integer = num1->Integer + num2->Integer;

    int carry = 0; // 进位
    int* dec1 = num1->Decimal;
    int* dec2 = num2->Decimal;
    int* decSum = sum.Decimal;
    for (int i = precision - 1; i >= 0; i--) {
        int tempSum = dec1[i] + dec2[i] + carry;
        decSum[i] = tempSum % sum.base;
        carry = tempSum / sum.base;
    }

    return sum;
}

// 高精度数减法
Number SubtractNumber(Number* num1, Number* num2) {
    Number diff;
    initNumber(&diff, num1->base);

    // 处理符号位
    if (num1->isNegative && !num2->isNegative) {
        num1->isNegative = 0;
        return addNumber(num1, num2);
    } else if (!num1->isNegative && num2->isNegative) {
        num2->isNegative = 0;
        diff = addNumber(num1, num2);
        diff.isNegative = 0;
        return diff;
    }

    if (compareNumber(num1, num2) < 0) {
        diff.isNegative = 1;
        Number* temp = num1;
        num1 = num2;
        num2 = temp;
    }

    int borrow = 0; // 借位
    diff.Integer = num1->Integer - num2->Integer;
    int* dec1 = num1->Decimal;
    int* dec2 = num2->Decimal;
    int* decDiff = diff.Decimal;
    for (int i = precision - 1; i >= 0; i--) {
        int tempDiff = dec1[i] - dec2[i] - borrow;
        if (tempDiff < 0) {
            tempDiff += diff.base;
            borrow = 1;
        } else {
            borrow = 0;
        }
        decDiff[i] = tempDiff;
    }

    return diff;
}

// 高精度数乘法
Number multiplyNumber(Number* num1, Number* num2) {
    Number product;
    initNumber(&product, num1->base);

    // 处理符号位
    if (num1->isNegative != num2->isNegative) {
        product.isNegative = 1;
    }

    int integerPart1 = num1->Integer;
    int integerPart2 = num2->Integer;
    product.Integer = integerPart1 * integerPart2;

    int decimalPart1 = *(int*)num1->Decimal;
    int decimalPart2 = *(int*)num2->Decimal;

    int* decProduct = product.Decimal;
    int carry = 0; // 进位
    for (int i = precision - 1; i >= 0; i--) {
        int tempProduct = decimalPart1 * decimalPart2 + decProduct[i] + carry;
        decProduct[i] = tempProduct % product.base;
        carry = tempProduct / product.base;
    }

    return product;
}

// 高精度数除法
Number divideNumber(Number* num1, Number* num2) {
    Number quotient;
    initNumber(&quotient, num1->base);

    // 处理符号位
    if (num1->isNegative != num2->isNegative) {
        quotient.isNegative = 1;
    }

    int integerPart1 = num1->Integer;
    int integerPart2 = num2->Integer;
    quotient.Integer = integerPart1 / integerPart2;

    int decimalPart1 = *(int*)num1->Decimal;
    int decimalPart2 = *(int*)num2->Decimal;

    int* decQuotient = quotient.Decimal;
    for (int i = 0; i < precision; i++) {
        decimalPart1 *= 10;
        decQuotient[i] = decimalPart1 / decimalPart2;
        decimalPart1 %= decimalPart2;
    }

    return quotient;
}

int main() {
    char str1[] = "1234.5678";
    char str2[] = "-5678.1234";

    Number number1, number2;
    initNumber(&number1, 10);
    initNumber(&number2, 10);

    stringToNumber(&number1, str1);
    stringToNumber(&number2, str2);

    char resultStr[100];

    Number sum = addNumber(&number1, &number2);
    numberToString(&sum, resultStr);
    printf("加法结果：%s\n", resultStr);

    Number diff = SubtractNumber(&number1, &number2);
    numberToString(&diff, resultStr);
    printf("减法结果：%s\n", resultStr);

    Number product = multiplyNumber(&number1, &number2);
    numberToString(&product, resultStr);
    printf("乘法结果：%s\n", resultStr);

    Number quotient = divideNumber(&number1, &number2);
    numberToString(&quotient, resultStr);
    printf("除法结果：%s\n", resultStr);

    system("pause");
    return 0;
}