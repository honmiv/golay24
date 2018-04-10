// ------------------------------------------------------------------------
//  File: golay24.c
//
//  An arithmetic decoder for the (24,12,8) Golay code.
//  Note: The C code below assumes execution in a 16-bit computer.
//  Inspired by Stephen Wicker's book, pp. 143-144.
// ------------------------------------------------------------------------
// This program is complementary material for the book:
//
// R.H. Morelos-Zaragoza, The Art of Error Correcting Coding, Wiley, 2002.
//
// ISBN 0471 49581 6
//
// This and other programs are available at http://the-art-of-ecc.com
//
// You may use this program for academic and personal purposes only.
// If this program is used to perform simulations whose results are
// published in a journal or book, please refer to the book above.
//
// The use of this program in a commercial product requires explicit
// written permission from the author. The author is not responsible or
// liable for damage or loss that may be caused by the use of this program.
//
// Copyright (c) 2002. Robert H. Morelos-Zaragoza. All rights reserved.
// ------------------------------------------------------------------------
#include <stdio.h>
#include <math.h>
#include <ctime>
#include <stdlib.h>
#include <iostream>

using namespace std;

int b2d(string bin)
{
	int out = 0;
	for (int i = 0; i <12; i++)
	{
		out += ((int)bin[12 - i - 1] % 48 % 10)*pow(2, i);
	}
	return out;
}
void d2b(int value)
{
	char buf[13];
	itoa(value, buf, 2);
	int freespaceCount = 0;
	for (int i = 0; i < 12; i++)
		if (buf[i] != '0' && buf[i] != '1')
			freespaceCount++;
	for (int i = 0; i < freespaceCount; i++)
		cout << '0';
	cout << buf << " ";
}

int main()
{
	/* Array x contains the twelve rows (columns) of the identity matrix */
	int x[12] = { 0x800, 0x400, 0x200, 0x100, 0x080, 0x040,
		0x020, 0x010, 0x008, 0x004, 0x002, 0x001 };

	/* Array y contains the twelve rows (columns) of the parity-check matrix */
	int y[12] = { 0x7ff, 0xee2, 0xdc5, 0xb8b, 0xf16, 0xe2d,
		0xc5b, 0x8b7, 0x96e, 0xadc, 0xdb8, 0xb71 };

	int c[2];          /* Codeword composed of 12-bit info and 12-bit parity */
	int r[2];          /* Received vector in two halfs of 12 bits each */
	int e[2];          /* Estimated error vector */

	int s;             /* syndrome */
	int q;             /* modified syndrome */
	int c_hat[2];      /* Estimated codeword */

	int i, j;
	int aux, found;
	int weight(int vector);

	c[0] = b2d("101010111101"); // input 12bit information part
	c[1] = b2d("100100000011"); // input 12bit parity
	cout << "codeWord = "; d2b(c[0]), d2b(c[1]);
	/* --------------------- Introduce errors ------------------------- */
	e[0] = b2d("000000000000");
	e[1] = b2d("000001000000");
	cout << "\nerrors   = "; d2b(e[0]), d2b(e[1]);
	cout << "\nweight   = " << weight(e[0]) + weight(e[1]);

	/* ---------------------- Received word --------------------------- */
	r[0] = c[0] ^ e[0];
	r[1] = c[1] ^ e[1];
	cout << "\nrecieved = "; d2b(r[0]); d2b(r[1]);
	/******* STEP 1: Compute the syndrome of the received vector ********/
	s = 0;
	for (j = 0; j < 12; j++)
	{
		aux = 0;
		for (i = 0; i < 12; i++)
			aux = aux ^ ((x[j] & r[0]) >> i & 0x01); /* identity part */
		for (i = 0; i < 12; i++)
			aux = aux ^ ((y[j] & r[1]) >> i & 0x01); /* parity part */
		s = (s << 1) ^ aux;
	}
	cout << "\nsyndrome = "; d2b(s);
	cout << "\nweight   = " << weight(s);

	/******* STEP 2 */
	printf("\nStep 2");
	if (weight(s) <= 3)
	{
		e[0] = s;
		e[1] = 0;
	}
	else
	{
		/******* STEP 3 */
		printf("\nStep 3");
		i = 0;
		found = 0;
		do {
			if (weight(s^y[i]) <= 2)
			{
				e[0] = s^y[i];
				e[1] = x[i];
				found = 1;
			}
			i++;
		} while ((i < 12) && (!found));

		if ((i == 12) && (!found))
		{
			/******* STEP 4 */
			printf("\nStep 4");
			q = 0;
			for (j = 0; j < 12; j++)
			{
				aux = 0;
				for (i = 0; i < 12; i++)
					aux = aux ^ ((y[j] & s) >> i & 0x01);
				q = (q << 1) ^ aux;
			}

			/******* STEP 5 */
			printf("\nStep 5");
			if (weight(q) <= 3)
			{
				e[0] = 0;
				e[1] = q;
			}
			else
			{
				/******* STEP 6 */
				printf("\nStep 6");
				i = 0;
				found = 0;
				do {
					if (weight(q^y[i]) <= 2)
					{
						e[0] = x[i];
						e[1] = q^y[i];
						found = 1;
					}
					i++;
				} while ((i < 12) && (!found));

				if ((i == 12) && (!found))
				{
					/******* STEP 7 */
					printf("\nuncorrectable error pattern");
					/* You can raise a flag here, or output the vector as is */
					exit(1);
				}
			}
		}
	}

	/* ------------------- Correct received word --------------------- */
	c_hat[0] = r[0] ^ e[0];
	c_hat[1] = r[1] ^ e[1];
	if (c_hat[0] != c[0] || c_hat[1] != c[1])
	{
		printf("\nuncorrectable error pattern"); getchar(); exit(1);
	}
	printf("\ndecoded  = "); d2b(c_hat[0]); d2b(c_hat[1]);
	cout << "\ncodeWord = "; d2b(c[0]), d2b(c[1]);
}

/* Function to compute the Hamming weight of a 12-bit integer */
int weight(int vector)
{
	int i, aux;
	aux = 0;
	for (i = 0; i<12; i++)
		if ((vector >> i) & 1)
			aux++;
	return(aux);
}
