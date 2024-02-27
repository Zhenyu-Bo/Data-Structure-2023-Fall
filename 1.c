#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 10000

typedef struct {
    int digits[MAX_SIZE]; // 数字数组
    int decimalPoint;     // 小数点位置
} HighPrecisionNumber;

void initializeNumber(HighPrecisionNumber* number) {
    for (int i = 0; i < MAX_SIZE; i++) {
        number->digits[i] = 0;
    }
    number->decimalPoint = 0;
}

void printNumber(HighPrecisionNumber* number, int base) {
    // 打印整数部分
    int i = MAX_SIZE - 1;
    while (i >= number->decimalPoint) {
        printf("%d", number->digits[i]);
        i--;
    }

    // 打印小数部分
    if (number->decimalPoint > 0) {
        printf(".");
        i = number->decimalPoint - 1;
        while (i >= 0) {
            printf("%d", number->digits[i]);
            i--;
        }
    }

    // 打印进制
    printf("(base %d)\n", base);
}

void convertToBase(HighPrecisionNumber* number, int base) {
    int quotient = 0;
    int remainder = 0;
    int currentPosition = 0;
    int newDecimalPoint = 0;

    // 将十进制转换为指定进制
    while (number->digits[number->decimalPoint - 1] != 0 || number->decimalPoint > 0) {
        quotient = 0;
        for (int i = number->decimalPoint - 1; i >= 0; i--) {
            int dividend = remainder * 10 + number->digits[i];
            number->digits[i] = dividend / base;
            remainder = dividend % base;
            if (i == number->decimalPoint - 1 && number->digits[i] == 0) {
                newDecimalPoint = i;
            }
        }
        number->decimalPoint = newDecimalPoint;
        number->digits[currentPosition] = remainder;
        currentPosition++;
        remainder = 0;
    }
}

void addNumbers(HighPrecisionNumber* number1, HighPrecisionNumber* number2, HighPrecisionNumber* result) {
    int carry = 0;
    for (int i = 0; i < MAX_SIZE; ++i) {
        int sum = number1->digits[i] + number2->digits[i] + carry;
        result->digits[i] = sum % 10;
        carry = sum / 10;
    }
    result->decimalPoint = number1->decimalPoint > number2->decimalPoint ? number1->decimalPoint : number2->decimalPoint;
    if (carry != 0) {
        result->digits[result->decimalPoint] = carry;
        result->decimalPoint++;
    }
}

void subtractNumbers(HighPrecisionNumber* number1, HighPrecisionNumber* number2, HighPrecisionNumber* result) {
    int borrow = 0;
    for (int i = 0; i < MAX_SIZE; ++i) {
        int diff = number1->digits[i] - number2->digits[i] - borrow;
        if (diff < 0) {
            diff += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        result->digits[i] = diff;
    }
    result->decimalPoint = number1->decimalPoint;
    while (result->digits[result->decimalPoint - 1] == 0 && result->decimalPoint > 0) {
        result->decimalPoint--;
    }
}

void multiplyNumbers(HighPrecisionNumber* number1, HighPrecisionNumber* number2, HighPrecisionNumber* result) {
    HighPrecisionNumber temp;
    initializeNumber(&temp);
    int carry = 0;

    for (int i = 0; i < MAX_SIZE; ++i) {
        carry = 0;
        initializeNumber(&temp);

        for (int j = 0; j < MAX_SIZE; ++j) {
            int product = number1->digits[i] * number2->digits[j] + carry;
            temp.digits[i + j] = product % 10;
            carry = product / 10;
        }

        addNumbers(result, &temp, result);
    }

    result->decimalPoint = number1->decimalPoint + number2->decimalPoint;
}

void evaluatePolynomial(double x, int n) {
    HighPrecisionNumber result;
    initializeNumber(&result);

    // 计算 f(x) = 3/7*x^3 - 1/3*x^2 + 4
    HighPrecisionNumber xTo3;
    initializeNumber(&xTo3);
    xTo3.digits[0] = 1; // x^0 = 1
    xTo3.decimalPoint = 0;
    multiplyNumbers(&xTo3, &xTo3, &result); // x^2
    multiplyNumbers(&result, &xTo3, &result); // x^3

    HighPrecisionNumber xTo2;
    initializeNumber(&xTo2);
    xTo2.digits[0] = 1; // x^0 = 1
    xTo2.decimalPoint = 0;
    multiplyNumbers(&xTo2, &xTo2, &result); // x^2

    double xDouble = x;
    int xInt = (int)xDouble;
    double decimalPart = xDouble - xInt;
    double base = 1.0 / n;

    // 计算 f(x) = f(x) - 1/3*x^2
    HighPrecisionNumber oneThird;
    initializeNumber(&oneThird);
    oneThird.digits[0] = 3; // 分子为 3
    oneThird.decimalPoint = 0;
    convertToBase(&oneThird, n);
    multiplyNumbers(&oneThird, &xTo2, &result); // 1/3*x^2

    // 计算 f(x) = f(x) + 4
    HighPrecisionNumber four;
    initializeNumber(&four);
    four.digits[0] = 4; // 常数项为 4
    four.decimalPoint = 0;
    convertToBase(&four, n);
    addNumbers(&result, &four, &result); // +4

    // 转换计算结果到指定进制
    convertToBase(&result, n);

    // 输出十进制和指定进制结果
    printf("f(%.2f) = ", x);
    printNumber(&result, n);

    // 计算小数部分
    for (int i = 0; i < n; ++i) {
        decimalPart *= n;
        int digit = (int)decimalPart;
        result.digits[result.decimalPoint++] = digit;
        decimalPart -= digit;
    }

    printf("Decimal part of %.2f in base %d is: ", x, n);
    printNumber(&result, n);

    printf("\n");
}

int main() {
    double x = 1.4;
    int n = 200;

    evaluatePolynomial(x, n);

    return 0;
}