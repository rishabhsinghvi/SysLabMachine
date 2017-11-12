#include <stdio.h>
#include <string.h>

void IF(void);  							//author: Noah,		tester: Aleksa
void ID(void);						//author: Aleksa,	tester: Noah
void EX(void);								//author: Noah,		tester: Aleksa, Peter
void MEM(void);								//author: Peter,	tester: Aleksa
void WB(void);								//author: Aleksa,	tester: Noah
char *progScannner(char *c); 				//author: Peter,	tester: Noah,  Done and tested
char *regNumberConverter(char *prog);		//author: Aleksa,	tester:	Peter
struct inst parser(char *input);			//author: Noah,		tester: Peter

/*
	To run this testing file compile the project using the makefile 
	by running the $make command which will create a sim-test and 
	sim-mips executable (hopefully works for windows, mac and linux)
*/
enum opcode {add, addi, sub, mult, beq, lw, sw, haltSimulation, noop};

struct inst
{
	enum opcode opcode;
	long rs;
	long rt;
	int rd;
	int Imm;
};

int compare_instructions(struct inst a, struct inst b){
	if((a.opcode == b.opcode) && (a.rs == b.rs) && (a.rt == b.rt) && (a.rd == b.rd) && (a.Imm == b.Imm)){
		return 1;
	}else{
		return 0;
	}
}

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

	return strcmp(progScannner("add	$s0,,$s1         ,     $s2"), "add $s0 $s1 $s2") == 0;
}

int progScanner_Test2(){

	return strcmp(progScannner("lw $s0, 8($t0)"), "lw $s0 8 $t0 ") == 0;
}

int progScanner_Test3(){

	return strcmp(progScannner("lw 			$s0   4($t0)"), "lw $s0 4 $t0 ") == 0;
}

int progScanner_Test4(){

	return strcmp(progScannner("add     $s0,      $s1,,,,,,$s2"), "add $s0 $s1 $s2") == 0;
}

int regNumberConverter_Test1(){
	return strcmp(regNumberConverter("add $zero $0 $s7"), "add 0 0 23") == 0;
}

int regNumberConverter_Test2(){
	return strcmp(regNumberConverter("add $t0 $8 $t8"), "add 8 8 24") == 0;
}

int regNumberConverter_Test3(){
	return strcmp(regNumberConverter("add $s0 $s1 $s2"), "add 16 17 18") == 0;
}

int regNumberConverter_Test4(){
	return strcmp(regNumberConverter("add $zero $at $v0"), "add 0 1 2") == 0;
}

int regNumberConverter_Test5(){
	return strcmp(regNumberConverter("lw $s0 8 $t0"), "lw 16 8 8") == 0;
}


int parser_Test1(){
	struct inst test = {0, 16, 17, 18, 0};

	return compare_instructions(test, parser("add 16 17 18"));
}

int parser_Test2(){
	struct inst test = {4, 17, 18, 0, 4};

	return compare_instructions(test, parser("lw 17 4 18"));
}

int parser_Test3(){
	struct inst test = {6, 8, 9, 0, 100};

	return compare_instructions(test, parser("beq 8 9 100"));
}

int main(int argc, char const *argv[])
{
	printf("Example Test: %d\n\n", ExampleTest());
	printf("IF Test: %d\n\n", IF_Test());
	printf("ID Test: %d\n\n", ID_Test());
	printf("EX Test: %d\n\n", EX_Test());
	printf("MEM Test: %d\n\n", MEM_Test());
	printf("WB Test: %d\n\n", WB_Test());
	printf("Program Scanner Test 1: %d\n\n", progScanner_Test1());
	printf("Program Scanner Test 2: %d\n\n", progScanner_Test2());
	printf("Program Scanner Test 3: %d\n\n", progScanner_Test3());
	printf("Program Scanner Test 4: %d\n\n", progScanner_Test4());			
	printf("regNumberConverter Test 1: %d\n\n", regNumberConverter_Test1());
	printf("regNumberConverter Test 2: %d\n\n", regNumberConverter_Test2());
	printf("regNumberConverter Test 3: %d\n\n", regNumberConverter_Test3());
	printf("regNumberConverter Test 4: %d\n\n", regNumberConverter_Test4());
	printf("regNumberConverter Test 5: %d\n\n", regNumberConverter_Test5());
	printf("parser Test1: %d\n\n", parser_Test1());
	printf("parser Test2: %d\n\n", parser_Test2());
	printf("parser Test3: %d\n\n", parser_Test3());





	return 0;
}