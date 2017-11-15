#include <stdio.h>
#include <string.h>


int numLines(FILE* fp);
struct inst *readFile(FILE* fp);
int IF(int c, int pgm_c);  					//author: Noah,		tester: Aleksa , Done in testing
int ID(long *registers, struct buffer IfId);								//author: Aleksa,	tester: Noah, 
int EX(int n, int m);						//author: Noah,		tester: Aleksa, Peter, Done in testing
int MEM(int mem_cycles, struct buffer ExeMem); 	//author: Peter,	tester: Aleksa
int WB(long *registers, struct buffer MemWb);					//author: Aleksa,	tester: Noah
char *progScanner(char *c); 				//author: Peter,	tester: Noah,  tested
char *regNumberConverter(char *prog); 		//author: Aleksa,	tester:	Peter, tested
struct inst parser(char *input);			//author: Noah,		tester: Peter, Done
//main  									//author: Peter



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


int numLines_test1(){
	
	FILE *fp;fp = fopen("test1.txt", "r");

	//printf("%s\n", "open file");
	return numLines(fp) == 7;

	fclose(fp);	
}


int fileRead_test1(){
	
	FILE *fp;
	fp = fopen("test1.txt", "r");

	struct inst *fileC = readFile(fp);
	struct inst test[7] = {{addi, 0, 1, 0, 10}, {addi,0,1,0,20},{addi,0,2,0,30},{mult,1,2,3,0},{sub,3,2,4,0},{add,2,1,1,0},{7,0,0,0,0}};
	//printInst(fileC[0]);

	int i,t;t=0;
	for(i = 0; i < 7; i++){
		if(compare_instructions(fileC[i], test[i])){
			t++;
		}else{
		//printInst(fileC[i]);
		}
		//t += compare_instructions(fileC[i], test[i]);
	}

	fclose(fp);

	//printf("t=%d\n", t);
	return t == 7;	
}


int progScanner_Test1(){

	char *testStr = progScanner("add	$s0,,$s1         ,     $s2");

	return strcmp(testStr, "add $s0 $s1 $s2") == 0;
}

int progScanner_Test2(){

	return strcmp(progScanner("lw $s0, 8($t0)"), "lw $s0 8 $t0") == 0;
}

int progScanner_Test3(){

	return strcmp(progScanner("lw 			$s0   4($t0)"), "lw $s0 4 $t0") == 0;
}

int progScanner_Test4(){

	return strcmp(progScanner("add     $s0,      $s1,,,,,,$s2"), "add $s0 $s1 $s2") == 0;
}

int progScanner_Test5(){

	return strcmp(progScanner("beq $t0 $t1 1000"), "beq $t0 $t1 1000") == 0;
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

int regNumberConverter_Test6(){
	return strcmp(regNumberConverter("beq $t0 $s1 1000"), "beq 8 17 1000") == 0;
}


int parser_Test1(){
	struct inst test = {add, 17, 18, 16, 0};

	return compare_instructions(test, parser("add 16 17 18"));
}

int parser_Test2(){
	struct inst test = {lw, 18, 17, 0, 4};

	struct inst peter = parser("lw 17 4 18");

	printf("%d  %d  %d  %d  %d\n", peter.opcode, peter.rs, peter.rt, peter.rd, peter.Imm);

	return compare_instructions(test, peter);
}

int parser_Test3(){
	struct inst test = {beq, 9, 8, 0, 100};

	char *line = regNumberConverter("beq 8 9 100");
	printf("%s\n", line);

	struct inst peter = parser(line);
	printf("%d  %d  %d  %d  %d\n", peter.opcode, peter.rs, peter.rt, peter.rd, peter.Imm);

	return compare_instructions(test, peter);
}

int parser_Test4(){
	struct inst test = {lw, 8, 16, 0, 4};

	char *line = regNumberConverter(progScanner("lw $s0 4 $t0"));
	printf("%s\n", line);

	struct inst peter = parser(regNumberConverter("lw $s0 4 $t0"));
	printf("%d  %d  %d  %d  %d\n", peter.opcode, peter.rs, peter.rt, peter.rd, peter.Imm);


	return compare_instructions(test, peter);
}

int parser_Test5(){
	struct inst test = {sub, 17, 18, 16, 0};

	printf("%s\n", progScanner("sub $s0, $s1, $s2"));
	char *line = regNumberConverter(progScanner("sub $s0, $s1, $s2"));
	printf("%s\n", line);

	struct inst peter = parser(line);
	printf("%d  %d  %d  %d  %d\n", peter.opcode, peter.rs, peter.rt, peter.rd, peter.Imm);


	return compare_instructions(test, peter);
}

int parser_Test6(){
	struct inst test = {sub, 19, 20, 18, 0};

	printf("%s\n", progScanner("sub $s2, $s3, $s4"));
	char *line = regNumberConverter(progScanner("sub $s2, $s3, $s4"));
	printf("%s\n", line);


	struct inst peter = parser(line);
	printf("%d  %d  %d  %d  %d\n",peter.opcode, peter.rs, peter.rt, peter.rd, peter.Imm);


	return compare_instructions(test, peter);
}

int parser_Test7(){
	struct inst test = {lw, 8, 16, 0, 24};

	printf("%s\n", progScanner("lw $s0, 24($t0)"));
	char *line = regNumberConverter(progScanner("lw $s0, 24($t0)"));
	printf("[%s]\n", line);


	struct inst peter = parser(line);
	printf("%d  %d  %d  %d  %d\n",peter.opcode, peter.rs, peter.rt, peter.rd, peter.Imm);


	return compare_instructions(test, peter);
}




int main(int argc, char const *argv[])
{
	printf("Example Test: %d\n\n", ExampleTest());
	printf("IF Test: %d\n\n", IF_Test());
	printf("ID Test: %d\n\n", ID_Test());
	printf("EX Test: %d\n\n", EX_Test());
	printf("MEM Test: %d\n\n", MEM_Test());
	printf("WB Test: %d\n\n", WB_Test());
	printf("Number of Lines Test 1: %d\n\n", numLines_test1());
	//printf("File Reader Test 1: %d\n\n", fileRead_test1());
	printf("Program Scanner Test 1: %d\n\n", progScanner_Test1());
	printf("Program Scanner Test 2: %d\n\n", progScanner_Test2());
	printf("Program Scanner Test 3: %d\n\n", progScanner_Test3());
	printf("Program Scanner Test 4: %d\n\n", progScanner_Test4());	
	printf("Program Scanner Test 5: %d\n\n", progScanner_Test5());			
	printf("regNumberConverter Test 1: %d\n\n", regNumberConverter_Test1());
	printf("regNumberConverter Test 2: %d\n\n", regNumberConverter_Test2());
	printf("regNumberConverter Test 3: %d\n\n", regNumberConverter_Test3());
	printf("regNumberConverter Test 4: %d\n\n", regNumberConverter_Test4());
	printf("regNumberConverter Test 5: %d\n\n", regNumberConverter_Test5());
	printf("regNumberConverter Test 6: %d\n\n", regNumberConverter_Test6());
	printf("parser Test1: %d\n\n", parser_Test1());
	printf("parser Test2: %d\n\n", parser_Test2());
	printf("parser Test3: %d\n\n", parser_Test3());
	printf("parser Test4: %d\n\n", parser_Test4());
	printf("parser Test5: %d\n\n", parser_Test5());
	printf("parser Test6: %d\n\n", parser_Test6());
	printf("parser Test7: %d\n\n", parser_Test7());





	return 0;
}
