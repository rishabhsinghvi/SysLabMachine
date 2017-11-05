#include <stdio.h>
#include <string.h>

void IF(void);  					//author: Noah,		tester: Aleksa
void ID(void);						//author: Aleksa,	tester: Noah
void EX(void);						//author: Noah,		tester:Aleksa, Peter
void MEM(void);						//author: Peter,	tester: Aleksa
void WB(void);						//author: Aleksa,	tester: Noah
char *progScannner(char *c); 		//author: Peter,	tester: Noah
char *regNumberConverter(void);		//author: Aleksa,	tester:	Peter
struct inst parser(char *input);	//author: Noah,		tester: Peter

/*
	To run this testing file compile the project using the makefile 
	by running the $make command which will create a sim-test and 
	sim-mips executable (hopefully works for windows, mac and linux)
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

int progScanner_Test1(){

	return strcmp(progScannner("add	$s0,,$s1         ,     $s2"), "add $s0 $s1 $s2");
}

int progScanner_Test2(){

	return strcmp(progScannner("lw $s0, 8($t0)"), "lw $s0 8 $t0");
}

int progScanner_Test3(){

	return strcmp(progScannner("lw 			$s0   4($t0)"), "lw $s0 4 $t0");
}

int progScanner_Test4(){

	return strcmp(progScannner("add     $s0,      $s1,,,,,,$2"), "add $s0 $s1 $s2");
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
	printf("Program Scanner Test 1: %d\n", progScanner_Test1());
	printf("Program Scanner Test 2: %d\n", progScanner_Test2());
	printf("Program Scanner Test 3: %d\n", progScanner_Test3());
	printf("Program Scanner Test 4: %d\n", progScanner_Test4());			
	printf("regNumberConverter Test: %d\n", regNumberConverter_Test());
	printf("parser Test: %d\n", parser_Test());





	return 0;
}