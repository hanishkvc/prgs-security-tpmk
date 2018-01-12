#include <stdio.h>

#define CONST32TEST 0x01234567
#define BUFLEN 64

__uint8_t cBuf[BUFLEN];

void blind_assign()
{
	cBuf[10] = CONST32TEST;
	for(int i = 0; i < BUFLEN; i++) {
		if ((i % 16) == 0)
			printf("\n");
		printf("0x%.2x, ", cBuf[i]);
	}
}

void castconvert_assign()
{
	*((__uint32_t*)&cBuf[10]) = CONST32TEST;
	for(int i = 0; i < BUFLEN; i++) {
		if ((i % 16) == 0)
			printf("\n");
		printf("0x%.2x, ", cBuf[i]);
	}
}

int main(int argc, char *argv)
{
	blind_assign();
	castconvert_assign();
	return 0;
}

