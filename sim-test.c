#include <stdio.h>

void IF(void);  				//author: Noah,		tester: Aleksa
void ID(void);					//author: Aleksa,	tester: Noah
void EX(void);					//author: Noah,		tester:Aleksa, Peter
void MEM(void);					//author: Peter,	tester: Aleksa
void WB(void);					//author: Aleksa,	tester: Noah
char *progScannner(void); 		//author: Peter,	tester: Noah
char *regNumberConverter(void); //author: Aleksa,	tester:	Peter
struct inst parser(void);		//author: Noah,		tester: Peter

/*
	To run this testing file either run the testSim.sh script 
	or compile the project using the makefile by running the 
	$make command which will create a sim-test executable 

*/

int ExampleTest(){
	return 0 == 0;
}

int IF_Test(){
	return 0;
}

int ID_Test(){
	return 0;
}

int EX_Test(){
	return 0;
}

int MEM_Test(){
	return 0;
}

int WB_Test(){
	return 0;
}

int progScanner_Test(){
	return 0;
}

int regNumberConverter_Test(){
	return 0;
}

int parser_Test(){
	return 0;
}

int main(int argc, char const *argv[])
{
	printf("Example Test: %d\n", ExampleTest());
	printf("IF Test: %d\n", IF_Test());
	printf("ID Test: %d\n", ID_Test());
	printf("EX Test: %d\n", EX_Test());
	printf("MEM Test: %d\n", MEM_Test());
	printf("WB Test: %d\n", WB_Test());
	printf("Program Scanner Test: %d\n", progScanner_Test());
	printf("regNumberConverter Test: %d\n", regNumberConverter_Test());
	printf("parser Test: %d\n", parser_Test());





	return 0;
}