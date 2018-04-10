#include <cmath>
#include <iostream>
#include <locale.h>
#include <stdlib.h>
using namespace std;

int bin2dec(string bin)
{
	int out = 0;
	for (int i =0; i <12; i++)
	{
		out += ((int)bin[12-i-1]%48 % 10)*pow(2, i);
	}
	return out;
}
void dec2bin(int value)
{
	char buf[13];
    itoa(value, buf, 2);
	cout << buf;
}

int main()
{
	setlocale(LC_ALL, "russian");
	int B[12] = { bin2dec("011111111111"),
				  bin2dec("111011100010"),
		/********/bin2dec("110111000101"),
		/********/bin2dec("101110001011"),
		/********/bin2dec("111100010110"),
		/********/bin2dec("111000101101"),
		/********/bin2dec("110001011011"),
		/********/bin2dec("100010110111"),
		/********/bin2dec("100101101110"),
		/********/bin2dec("101011011100"),
		/********/bin2dec("110110111000"),
		/********/bin2dec("101101110001"),
	};

	int code[2]; // код, состо€щий из 12 бит информационной части
				 // и 12 бит проверочной

	code[0] = bin2dec("101010111101");
	cout << "      »сходна€ последовательность: ";
	dec2bin(code[0]); cout << endl;
	code[1] = 0;
	for (int i = 0; i < 12; i++)
	{
		int semiRes = 0;
		for (int j = 0; j < 12; j++) {
			semiRes = semiRes ^ (((code[0] & B[i]) >> j) % 2);
		}
		code[1] = (code[1] << 1) ^ semiRes;
	}

	cout << "«акодированна€ последовательность: ";
	dec2bin(code[0]);
	dec2bin(code[1]);
}

