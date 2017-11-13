#include <stdio.h>
#include <string.h>


int numLines(FILE* fp);
struct inst *readFile(FILE* fp);
int IF(int c, int pgm_c, struct inst *instruct);  					//author: Noah,		tester: Aleksa , Done in testing
struct buffer ID(long *registers, struct buffer IfId);								//author: Aleksa,	tester: Noah, 
int EX(int n, int m);						//author: Noah,		tester: Aleksa, Peter, Done in testing
int MEM(int cycles_counter, int mem_cycles, struct buffer ExeMem); 	//author: Peter,	tester: Aleksa
int WB(int cycles_count, long *registers, struct buffer MemWb);					//author: Aleksa,	tester: Noah
char *progScannner(char *c); 				//author: Peter,	tester: Noah,  tested
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

/*
int ID_Test(){
    struct buffer input;
    long* reg_file = {88,188,288,388,488,588,688,788,888,988,1088,1188,1288,1388,1488,1588,1688,1788,1888,1988,2088,2188,2288,2388,2488,2588,2688,2788,2888,2988,3088,3188};
    //addition case
    input.instruction.opcode = 0;
    input.instruction.rs = 2;
    input.instruction.rt = 4;
    input.instruction.rd = 24;
    struct buffer output;
    output = ID(reg_file, input);
    
    int result = reg_file[2] - output.instruction.rs;
    result = result + (reg_file[4] - output.instruction.rt);
    result = result + (24 - output.instruction.rd);
    
    //subtration case
    input.instruction.opcode = 2;
    input.instruction.rs = 2;
    input.instruction.rt = 4;
    input.instruction.rd = 24;
    output = ID(reg_file, input);
    
    result = result + (reg_file[2] - output.instruction.rs);
    result = result + (reg_file[4] - output.instruction.rt);
    result = result + (24 - output.instruction.rd);
    
    //multiplication case
    input.instruction.opcode = 3;
    input.instruction.rs = 2;
    input.instruction.rt = 4;
    input.instruction.rd = 24;
    output = ID(reg_file, input);
    
    result = reg_file[2] - output.instruction.rs;
    result = result + (reg_file[4] - output.instruction.rt);
    result = result + (24 - output.instruction.rd);
    
    //addi case
    input.instruction.opcode = 1;
    input.instruction.rs = 2;
    input.instruction.rt = 4;
    input.instruction.Imm = 24;
    output = ID(reg_file, input);
    
    result = reg_file[2] - output.instruction.rs;
    result = result + (2 - output.instruction.rt);
    result = result + (24 - output.instruction.Imm);
    
    //beq case
    input.instruction.opcode = 1;
    input.instruction.rs = 2;
    input.instruction.rt = 4;
    output = ID(reg_file, input);
    
    result = reg_file[2] - output.instruction.rs;
    result = result + (2 - output.instruction.rt);
    result = result + (24 - output.instruction.Imm);
    
    //lw case
    input.instruction.opcode = 1;
    input.instruction.rs = 2;
    input.instruction.rt = 4;
    input.instruction.Imm = 24;
    output = ID(reg_file, input);
    
    result = reg_file[2] - output.instruction.rs;
    result = result + (2 - output.instruction.rt);
    result = result + (24 - output.instruction.Imm);
    
    //sw case
    input.instruction.opcode = 1;
    input.instruction.rs = 2;
    input.instruction.rt = 4;
    input.instruction.Imm = 24;
    output = ID(reg_file, input);
    
    result = reg_file[2] - output.instruction.rs;
    result = result + (2 - output.instruction.rt);
    result = result + (24 - output.instruction.Imm);
    
	return result;
}
*/
/*
int EX_Test_Add(){
	//addition test
		IDEX.instruction.opcode = 0;
		IDEX.instruction.rs = 4;
		IDEX.instruction.rt = 3;
		int i = EX(4,5);

	return EXMEM.data - 7;
}

int EX_Test_Sub(){
	//subtration test
	IDEX.instruction.opcode = 1;
	IDEX.instruction.rs = 4;
	IDEX.instruction.rt = 3;
	int i = EX(4,5);

	return EXMEM.data - 1;
}

int EX_Test_Addi(){
	//add-immediate test
	IDEX.instruction.opcode = 2;
	
}
*/

int EX_Test(){
	return 0;
}

int MEM_Test(){
	return 0;
}


int WB_Test(){
	/*
    struct buffer input;
    int result;
    long* reg_file = {88,188,288,388,488,588,688,788,888,988,1088,1188,1288,1388,1488,1588,1688,1788,1888,1988,2088,2188,2288,2388,2488,2588,2688,2788,2888,2988,3088,3188};
    input.instruction.opcode = add;
    input.data = 88;
    for(int i=0;i<32;i++){
        input.wbReg = i;
        int count = WB(0,reg_file,input);
    }
    for(int i=0;i<32;i++){
        result = result + reg_file[i]-88;
    }
	return result;
	*/
	return 0;
}


int numLines_test1(){
	
	FILE *fp;fp = fopen("test1.txt", "r");

	//printf("%s\n", "open file");
	return numLines(fp) == 5;

	fclose(fp);	
}


int fileRead_test1(){
	/*
	FILE *fp;
	fp = fopen("test1.txt", "r");
	FILE *fp2;fp2 = fp;

	printf("%s\t%d\n", "open file", numLines(fp2));

	struct inst *fileC = readFile(fp);
	struct inst test = {2, 1, 0, 10, 0};

	return compare_instructions(fileC[0], test) == 0;
	fclose(fp);
	*/
	return 0;
	
}

int progScanner_Test1(){

	return strcmp(progScannner("add	$s0,,$s1         ,     $s2"), "add $s0 $s1 $s2") == 0;
}

int progScanner_Test2(){

	return strcmp(progScannner("lw $s0, 8($t0)"), "lw $s0 8 $t0") == 0;
}

int progScanner_Test3(){

	return strcmp(progScannner("lw 			$s0   4($t0)"), "lw $s0 4 $t0") == 0;
}

int progScanner_Test4(){

	return strcmp(progScannner("add     $s0,      $s1,,,,,,$s2"), "add $s0 $s1 $s2") == 0;
}

int progScanner_Test5(){

	return strcmp(progScannner("beq $t0 $t1 1000"), "beq $t0 $t1 1000") == 0;
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
	struct inst test = {add, 16, 17, 18, 0};

	return compare_instructions(test, parser("add 16 17 18"));
}

int parser_Test2(){
	struct inst test = {lw, 17, 18, 0, 4};

	struct inst peter = parser("lw 17 4 18");

	printf("%d  %d  %d  %d  %d\n", peter.opcode, peter.rs, peter.rt, peter.rd, peter.Imm);

	return compare_instructions(test, peter);
}

int parser_Test3(){
	struct inst test = {beq, 8, 9, 0, 100};

	char *line = regNumberConverter("beq 8 9 100");
	printf("%s\n", line);

	struct inst peter = parser(line);
	printf("%d  %d  %d  %d  %d\n", peter.opcode, peter.rs, peter.rt, peter.rd, peter.Imm);

	return compare_instructions(test, peter);
}

int parser_Test4(){
	struct inst test = {lw, 16, 8, 0, 8};

	char *line = regNumberConverter(progScannner("lw $s0 8 $t0"));
	printf("%s\n", line);

	struct inst peter = parser(regNumberConverter("lw $s0 8 $t0"));
	printf("%d  %d  %d  %d  %d\n", peter.opcode, peter.rs, peter.rt, peter.rd, peter.Imm);


	return compare_instructions(test, peter);
}

int parser_Test5(){
	struct inst test = {sub, 16, 17, 18, 0};

	printf("%s\n", progScannner("sub $s0, $s1, $s2"));
	char *line = regNumberConverter(progScannner("sub $s0, $s1, $s2"));
	printf("%s\n", line);

	struct inst peter = parser(line);
	//printf("%d  %d  %d  %d  %d\n", peter.opcode, peter.rs, peter.rt, peter.rd, peter.Imm);


	return compare_instructions(test, peter);
}

int parser_Test6(){
	struct inst test = {sub, 18, 19, 20, 0};

	printf("%s\n", progScannner("sub $s2, $s3, $s4"));
	char *line = regNumberConverter(progScannner("sub $s2, $s3, $s4"));
	printf("%s\n", line);


	struct inst peter = parser(line);
	//printf("%d  %d  %d  %d  %d\n",peter.opcode, peter.rs, peter.rt, peter.rd, peter.Imm);


	return compare_instructions(test, peter);
}

int parser_Test7(){
	struct inst test = {lw, 16, 8, 0, 8};

	printf("%s\n", progScannner("lw $s0, 8($t0)"));
	char *line = regNumberConverter(progScannner("lw $s0, 8($t0)"));
	printf("[%s]\n", line);


	struct inst peter = parser(line);


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
	printf("File Reader Test 1: %d\n\n", fileRead_test1());
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
