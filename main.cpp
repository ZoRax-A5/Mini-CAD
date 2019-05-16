#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "acllib.h"


#define INF 65536
#define length 1380
#define width 800
#define Ox 40
#define Oy 700
#define delta 0.01

#define SIZE 1000
#define MAX 20
#define SIN 1
#define COS 2
#define SQRT 3
#define FABS 4
#define TAN 5
#define ASIN 6
#define ACOS 7
#define ATAN 8
#define EXP 9
#define LOG 10

int top_operand = -1;
int top_operator = -1;
int cnt = 0;
int judge = 1;
double number = 0;
int flag = 0, dot = 1;
int funcnt = 0;
int function[MAX] = { 0 };
char InfixExpression[SIZE];
char *p, *t;

void DrawXYCoordinate(double x_min,double x_max,int y_min,int y_max);
double ReadFunction(double x);

void PrintIntroduction();
void Error();
int TransToDigit();
void InsertOperand(double Operand[]);
void InsertOperator(char Operator[]);
void DealOperand(double Operand[], char Operator[]);
int CompareOperator(char Operator[]);
void DealOperator(double Operand[], char Operator[], int i);
void Calculation(double Operand[], char Operator[]);
void DealFunction(double Operand[], char Operator[]);
double PrintResult(double Operand[], char Operator[]);

double(*funcp[15])(double x) = { 0,sin,cos,sqrt,fabs,tan,asin,acos,atan,exp,log };

int Setup()
{
	double x_max = 0, x_min = 0, x_rate = 0, y_rate = 0;
	int y_max = -INF, y_min = INF;
	double x, y, dx, dy;

	initConsole();
	PrintIntroduction();
	gets_s(InfixExpression);
	scanf_s("%lf %lf", &x_min, &x_max);

	if ((x_max - x_min) > length) x_rate = 1;
	else x_rate = (length - 2*Ox) / (x_max - x_min);
	for (dx = Ox;dx < length-Ox;dx += delta)
	{
		x = x_min + (dx - Ox) / x_rate;
		if (fabs(x) < 1e-8) continue;
		y = ReadFunction(x);
		if (y > INF/2 || y < -INF/2) continue;
		if (y > y_max) y_max = y+1;
		if (y < y_min) y_min = y-1;
	}
	if ((y_max - y_min) > width) y_rate = 1;
	else y_rate = (width - 2*100) / (y_max - y_min);
	DrawXYCoordinate(x_min,x_max,y_min,y_max);

	beginPaint();
	for (dx = Ox;dx < length-Ox;dx += delta)
	{
		x = x_min + (dx - Ox) / x_rate;
		if (fabs(x) < 1e-8) continue;
		y = ReadFunction(x);
		if (y > INF / 2 || y < -INF / 2) continue;
		dy = Oy - y_rate * (y - y_min);
		putPixel(dx, dy, DEFAULT);
	}
	endPaint();

	return 0;
}

void DrawXYCoordinate(double x_min,double x_max,int y_min,int y_max)
{
	double x_rate = 0, y_rate = 0, x = 0, y = 0;
	int x_big = 1, y_big = 1;
	const int point = 5;
	int i = x_min, j = y_min;
	if ((x_max - x_min) > length) x_rate = 1;
	else x_rate = (length - 2*Ox) / (x_max - x_min);
	if ((y_max - y_min) > width) y_rate = 1;
	else y_rate = (width - 2*100) / (y_max - y_min);
	if ((x_max - x_min) > 500) x_big = 100;
	else if ((x_max - x_min) > 80) x_big = 10;
	else if ((x_max - x_min) > 30) x_big = 2;
	if ((y_max - y_min) > 500) y_big = 100;
	else if ((y_max - y_min) > 80) y_big = 10;
	else if ((y_max - y_min) > 30) y_big = 2;
	initWindow("zwx", DEFAULT, DEFAULT, length, width);
	beginPaint();
	line(0, width / 2, length, width / 2);
	line(15, 0, 20, width);
	setTextSize(30);
	paintText(50, 20, InfixExpression);
	setTextSize(15);
	for (x = Ox, y = width / 2;x <= length ;x += x_rate*x_big, i+=x_big)
	{
		moveTo(x, y);
		lineRel(0, -point);		{			char ch[SIZE];
			_gcvt(i, 3, ch);			paintText(x-point, y + point, ch);		}
	}
	for (x = 15, y = Oy;y >= 100;y -= y_rate*y_big, j+=y_big)
	{
		moveTo(x, y);
		lineRel(point, 0);
		{			char ch[SIZE];
			_gcvt(j, 3, ch);			paintText(x + point+1, y-point-1, ch);		}
	}
	endPaint();
}
double ReadFunction(double x)
{
	double Operand[SIZE] = { 0 };
	char Operator[SIZE] = { 0 };
	double result = 0;

	top_operand = -1;
	top_operator = -1;
	flag = 0;dot = 1;
	p = InfixExpression;
	if (*p == '*' || *p == '/') Error();
	else if (*p == '+' || *p == '-')
	{
		InsertOperator(Operator);
		top_operand++;
		p++;
	}
	for (t = p + 1;*p != '=';p++, t++)
	{
		if (*p == ' ') continue;
		if (isdigit(*p)) DealOperand(Operand, Operator);
		else if (*p == '(' && (*t == '+' || *t == '-'))
		{
			InsertOperator(Operator);
			p++;t++;
			InsertOperator(Operator);
			top_operand++;
		}
		else if (((*p == '+' || *p == '-' || *p == '*' || *p == '/') && (*t != '+'&&*t != '-'&&*t != '*'&&*t != '/')) || (*p == '('&&*t != ')') || *p == ')')
		{
			int i = CompareOperator(Operator);
			DealOperator(Operand, Operator, i);
		}
		else if (*p == 'x')
		{
			number = x;
			InsertOperand(Operand);
		}
		else if (isalpha(*p)) DealFunction(Operand, Operator);
		else Error();
	}
	result = PrintResult(Operand, Operator);
	return result;
}
void PrintIntroduction()
{
	if (cnt) printf("The function is:\n");
	else printf("The function is (Such as 3*x*x+sin(x)=.):\n");
}
void Error()
{
	printf("Syntax ERROR!\n");
	top_operand = -1;
	top_operator = -1;
	exit(0);
}
int TransToDigit()
{
	int n;
	n = *p - '0';
	return n;
}
void InsertOperand(double Operand[])
{
	number = number / dot;
	Operand[++top_operand] = number;
	number = 0;flag = 0;dot = 1;
}
void InsertOperator(char Operator[])
{
	Operator[++top_operator] = *p;
}
void DealOperand(double Operand[], char Operator[])
{
	if (!isdigit(*t) && *t != '.')
	{
		number = number * 10 + TransToDigit();
		if (flag) dot *= 10;
		InsertOperand(Operand);
	}
	else if (*t == '.')
	{
		number = number * 10 + TransToDigit();
		flag = 1;
		p++;t++;
	}
	else
	{
		number = number * 10 + TransToDigit();
		if (flag) dot *= 10;
	}
}
int CompareOperator(char Operator[])
{
	int i;
	if (*p == ')') i = 1;
	else if (top_operator == -1 || (Operator[top_operator] == '+' || Operator[top_operator] == '-') && (*p == '*' || *p == '/') || Operator[top_operator] == '(' || *p == '(') i = 0;
	else i = 2;
	return i;
}
void DealOperator(double Operand[], char Operator[], int i)
{
	if (i == 0)	InsertOperator(Operator);
	else if (i == 1)
	{
		while (Operator[top_operator] != '(')
		{
			Calculation(Operand, Operator);
		}
		top_operator--;
		if (!function[funcnt])
		{
			Operand[top_operand] = (*funcp[function[funcnt - 1]])(Operand[top_operand]);
			function[--funcnt] = 0;
		}
	}
	else
	{
		Calculation(Operand, Operator);
		while (CompareOperator(Operator) == 2)
		{
			Calculation(Operand, Operator);
		}
		InsertOperator(Operator);
	}
}
void Calculation(double Operand[], char Operator[])
{
	double m = 0;
	if (Operator[top_operator] == '/')
	{
		if (Operand[top_operand] == 0) Error();
		m = Operand[top_operand - 1] / Operand[top_operand];
		top_operator--;top_operand--;
		Operand[top_operand] = m;
		return;
	}
	else if (Operator[top_operator] == '+') m = Operand[top_operand - 1] + Operand[top_operand];
	else if (Operator[top_operator] == '-') m = Operand[top_operand - 1] - Operand[top_operand];
	else if (Operator[top_operator] == '*') m = Operand[top_operand - 1] * Operand[top_operand];
	top_operator--;top_operand--;
	Operand[top_operand] = m;
}
void DealFunction(double Operand[], char Operator[])
{
	char temp[10] = { 0 };
	int i = 0;
	while (isalpha(*p))
	{
		temp[i++] = *p;
		p++;t++;
	}
	if (!strcmp(temp, "sin")) function[funcnt++] = SIN;
	else if (!strcmp(temp, "cos"))function[funcnt++] = COS;
	else if (!strcmp(temp, "sqrt"))function[funcnt++] = SQRT;
	else if (!strcmp(temp, "fabs"))function[funcnt++] = FABS;
	else if (!strcmp(temp, "tan"))function[funcnt++] = TAN;
	else if (!strcmp(temp, "atan"))function[funcnt++] = ATAN;
	else if (!strcmp(temp, "asin"))function[funcnt++] = ASIN;
	else if (!strcmp(temp, "acos"))function[funcnt++] = ACOS;
	else if (!strcmp(temp, "exp"))function[funcnt++] = EXP;
	else if (!strcmp(temp, "log"))function[funcnt++] = LOG;
	else Error();
	p--;t--;
}
double PrintResult(double Operand[], char Operator[])
{
	if (top_operand != -1)
	{
		while (funcnt)
		{
			Operand[top_operand] = (*funcp[function[funcnt - 1]])(Operand[top_operand]);
			function[--funcnt] = 0;
		}
		while (top_operand != 0)
		{
			Calculation(Operand, Operator);
		}
		return Operand[0];
	}
	else Error();
}

