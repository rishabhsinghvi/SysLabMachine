// Author: Aleksa Deric, Noah Sheridan, Peter Stanwicks
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#define SINGLE 1
#define BATCH 0
#define REG_NUM 32

int CLK = 0;
int PC = 0;
int BRANCH_PENDING = 0;
int DECODE_UNFINISHED = 0;
int EXECUTE_UNFINISHED = 0;
int MEMORY_UNFINISHED = 0;
int WRITEBACK_REG_NEW = 0;  //new contents in the writeback buffer register for writeback stage to consume
int RAW_HAZARD = 0;
int HALT_SIMULATION = 0;
int IFCTDN = 0;
int IDCTDN = 0;
int EXCTDN = 0;
int MEM_cycle_count = 0;
int WB_cycle_count = 0;

enum opcode {add, addi, sub, mult, beq, lw, sw, haltSimulation, noop};

char* correctOpcode[] = {"add", "addi", "sub", "mult", "beq", "lw", "sw", "haltSimulation"};


long* arrayPntr;
long dataMemory[64]; //2kb of 32-bit words

struct inst
{
	enum opcode opcode;
	long rs;
	long rt;
	int rd;
	int Imm;
};

struct buffer
{
	struct inst instruction;
	int readyToRead;
	int readytoWrite;
	int address;
	int wbReg;
	int VALID;
	long data;
};

struct buffer IDEX;
struct buffer IFID;
struct buffer IDEX;
struct buffer EXMEM;
struct buffer MEMWB;
struct inst *IM;  //can we get an intruction count and do malloc later to get exact size?


int numLines(FILE* fp);
char *str_cat(char *dest, const char *src);
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

void initLatches(){

}

char *str_cat(char *dest, const char *src){
    size_t i,j;
    for (i = 0; dest[i] != '\0'; i++)
        ;
    for (j = 0; src[j] != '\0'; j++)
        dest[i+j] = src[j];
    dest[i+j] = '\0';
    return dest;
}

int str_comp(char str1[], char str2[]){
	int ctr=0;

	while(str1[ctr]==str2[ctr]){
		if(str1[ctr]=='\0'||str2[ctr]=='\0')
			break;
		ctr++;
	}

	if(str1[ctr]=='\0' && str2[ctr]=='\0')
		return 0;
	else
		return -1;
}

int numLines(FILE* fp){
	int lines, ch;lines = 0;

	do{
		ch = fgetc(fp);


		if(ch == '\n')
			lines++;
	}while(ch != EOF);

	if(ch != '\n' && lines != 0)
		lines++;
	return lines;
}

struct inst *readFile(FILE* fp){
	int lines = numLines(fp);
	struct inst *instructions = (struct inst*)malloc(lines*sizeof(struct inst));
	char line[100];

	rewind(fp);
	
	int i;
	for (i = 0; i < lines; ++i){
		fgets(line, 100, fp);
		size_t len = strlen(line);
		printf("%d \[%s\]\n", i,line);

		instructions[i] = parser(regNumberConverter(progScannner(line)));
	}

	return instructions;
	free(instructions);
}


int main (int argc, char *argv[]){
	int sim_mode=0;//mode flag, 1 for single-cycle, 0 for batch
	int c,m,n;
	int i;//for loop counter
	long mips_reg[REG_NUM];
	long pgm_c=0;//program counter
	long sim_cycle=0;//simulation cycle counter
	//define your own counter for the usage of each pipeline stage here




	int test_counter=0;
	FILE *input=NULL;
	FILE *output=NULL;
	printf("The arguments are:");
	
	for(i=1;i<argc;i++){
		printf("%s ",argv[i]);
	}
	printf("\n");
	if(argc==7){
		if(strcmp("-s",argv[1])==0){
			sim_mode=SINGLE;
		}
		else if(strcmp("-b",argv[1])==0){
			sim_mode=BATCH;
		}
		else{
			printf("Wrong sim mode chosen\n");
			exit(0);
		}
		
		m=atoi(argv[2]);
		n=atoi(argv[3]);
		c=atoi(argv[4]);
		input=fopen(argv[5],"r");
		output=fopen(argv[6],"w");
		
	}
	
	else{
		printf("Usage: ./sim-mips -s m n c input_name output_name (single-sysle mode)\n or \n ./sim-mips -b m n c input_name  output_name(batch mode)\n");
		printf("m,n,c stand for number of cycles needed by multiplication, other operation, and memory access, respectively\n");
		exit(0);
	}
	if(input==NULL){
		printf("Unable to open input or output file\n");
		exit(0);
	}
	if(output==NULL){
		printf("Cannot create output file\n");
		exit(0);
	}
	//initialize registers and program counter
	if(sim_mode==1){
		for (i=0;i<REG_NUM;i++){
			mips_reg[i]=0;
		} 
	}
	

	//start your code from here
	struct inst *Inst_Mem;
	Inst_Mem = readFile(input);

	while(!HALT_SIMULATION){





		sim_cycle = sim_cycle + IF(c, pgm_c, Inst_Mem);
		if((BRANCH_PENDING!=0)||(RAW_HAZARD!=0))
		//pgm_c = pgm_c -1;
		IDEX = ID(mips_reg, IFID);
		sim_cycle++;
		sim_cycle = sim_cycle + EX(n,m);
		sim_cycle = sim_cycle + MEM(sim_cycle,c,EXMEM);
		sim_cycle = sim_cycle + WB(1,mips_reg,MEMWB);


	}





 



	double iftime = (100*IFCTDN)/sim_cycle;
	double idtime = (100*IDCTDN)/sim_cycle;
	double extime = (100*EXCTDN)/sim_cycle;
	double memtime = (100*MEM_cycle_count)/sim_cycle;
	double wbtime = (100*WB_cycle_count)/sim_cycle;

	printf("The percetage of time used by IF: %f \nThe percetage of time used by ID: %f \nThe percetage of time used by EX: %f \nThe percetage of time used by MEM: %f \nThe percetage of time used by WB: %f \n", iftime, idtime, extime, memtime, wbtime);

	


}

//
char *progScannner(char *c){

	char *ret; ret = (char *)malloc(strlen(c));
	char t[2];
	char space,open;open = 0;

	int i;
	for (i = 0; i < strlen(c); i++){

		if(c[i] == '\n')
			return ret;

		if( ('a' <= c[i] && c[i] <= 'z') || ('A' <= c[i] && c[i] <= 'Z') || ('0' <= c[i] && c[i] <= '9') || c[i] == '$'){
			t[0] = c[i];
			t[1] = '\0';
			str_cat(ret, t);

			space = 0;
		}else{
			if(space == 0 && open==0){
				t[0] = ' ';
				t[1] = '\0';
				str_cat(ret, t);
			}

			space = 1;
		}


		if(c[i] == '('){
			if(open == 1){
				printf("%s\n", "Mismatched paranthesis");
				exit(0);
			}
			open = 1;
		}else if(c[i] == ')'){
			if(open == 0){
				printf("%s\n", "Mismatched paranthesis");
				exit(0);
			}
			open = 0;
		}


	}
	if(open == 1){
		printf("%s\n", "Mismatched paranthesis");
		exit(0);
	}
	

	return ret;
	free(ret);
}

char *regNumberConverter(char *prog){
	
	char* registers1[] = {"zero", "at", "v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7"};
	char* registers2[] = {"s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7","t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"};	//+16
	
	int len = strlen(prog); 
	int i =0; int last = 0; int end, copy, tempCopy, size, registerNumber, regarg;
	
	
	char* finalString = (char*)malloc(len);
	
	char* temp;
	char* numberTemp;
	
	
	for (i = 0; i < (len);i++){			//parse the string
		
		
		
		if(prog[i] == '$'){		//if $ is found
			
			end = i;
			
			while(!(prog[end]==' ')&&!(prog[end]=='\0')){	//find the edge
				
				end++;
			}
			
			
			temp = (char*)malloc(end-i); 	//temporarily copy the argument in temp
			numberTemp = (char*)malloc(5);
			
			tempCopy = 0;
			
			for (copy = i+1; copy < end; copy++){
				temp[tempCopy] = prog[copy];
				tempCopy++;
			}
			temp[tempCopy] = '\0';
			
			
			
			if (!(strcmp(temp,"0"))){		//check if the argument is 0
				
				sprintf(numberTemp, "%d",0);
				//printf("DIGIT ARGUMENT (zero detected) [%s]\n",numberTemp);
			}
			
			else if(!(atoi(temp) == 0)){	//check if the argument is a number less than 32
				assert(atoi(temp) < 32);
				sprintf(numberTemp, "%d",atoi(temp));
				//printf("DIGIT ARGUMENT [%s]\n",numberTemp);
				
			}
			else{		//check if the argument is a name of one of the registers
			
			//this is ugly and should be changed eventually, i couldnt get it to run with strcmp and arrays
				registerNumber = -1;

					if(!(strcmp("zero",temp))){
						registerNumber = 0;
					}
					else if(!(strcmp("at",temp))){
						registerNumber = 1;
					}
					else if(!(strcmp("v0",temp))){
						registerNumber = 2;
					}
					else if(!(strcmp("v1",temp))){
						registerNumber = 3;
					}
					else if(!(strcmp("a0",temp))){
						registerNumber = 4;
					}
					else if(!(strcmp("a1",temp))){
						registerNumber = 5;
					}
					else if(!(strcmp("a2",temp))){
						registerNumber = 6;
					}
					else if(!(strcmp("a3",temp))){
						registerNumber = 7;
					}
					else if(!(strcmp("t0",temp))){
						registerNumber = 8;
					}
					else if(!(strcmp("t1",temp))){
						registerNumber = 9;
					}
					else if(!(strcmp("t2",temp))){
						registerNumber = 10;
					}
					else if(!(strcmp("t3",temp))){
						registerNumber = 11;
					}
					else if(!(strcmp("t4",temp))){
						registerNumber = 12;
					}
					else if(!(strcmp("t5",temp))){
						registerNumber = 13;
					}
					else if(!(strcmp("t6",temp))){
						registerNumber = 14;
					}
					else if(!(strcmp("t7",temp))){
						registerNumber = 15;
					}
					else if(!(strcmp("s0",temp))){
						registerNumber = 16;
					}
					else if(!(strcmp("s1",temp))){
						registerNumber = 17;
					}
					else if(!(strcmp("s2",temp))){
						registerNumber = 18;
					}
					else if(!(strcmp("s3",temp))){
						registerNumber = 19;
					}
					else if(!(strcmp("s4",temp))){
						registerNumber = 20;
					}
					else if(!(strcmp("s5",temp))){
						registerNumber = 21;
					}
					else if(!(strcmp("s6",temp))){
						registerNumber = 22;
					}
					else if(!(strcmp("s7",temp))){
						registerNumber = 23;
					}
					else if(!(strcmp("t8",temp))){
						registerNumber = 24;
					}
					else if(!(strcmp("t9",temp))){
						registerNumber = 25;
					}
					else if(!(strcmp("k0",temp))){
						registerNumber = 26;
					}
					else if(!(strcmp("k1",temp))){
						registerNumber = 27;
					}
					else if(!(strcmp("gp",temp))){
						registerNumber = 28;
					}
					else if(!(strcmp("sp",temp))){
						registerNumber = 29;
					}
					else if(!(strcmp("fp",temp))){
						registerNumber = 30;
					}
					else if(!(strcmp("ra",temp))){
						registerNumber = 31;
					}
					
				
				assert(registerNumber > -1);			//if no argument found, crash the program
				sprintf(numberTemp, "%d",registerNumber);
				//printf("REGISTER [%s] maps to [%s]\n",temp, numberTemp);
			}
			
			copy = 0;
			
			while (!(numberTemp[copy]=='\0')){		//append the number
				finalString[last] = numberTemp[copy];
				copy++;
				last++;
			}
			
			free(temp);
			free(numberTemp);
			i=end-1;
			
		}
		else{
			finalString[last] = prog[i];
			last++;
		}
		
	}
	finalString[last] = '\0';
	//printf("[%s]\n\n",finalString);

	//printf("TERM\n");
	return finalString;
}


struct inst parser(char *input){

	

	struct inst output;
	char* opcode; 
	
	char delimeter[] = " ";
	char inputString[strlen(input)+1];
	
	inputString[strlen(input)+1] = '\0';
	
	int l = 0;
	
	//copy input into inputString
	while (input[l] != '\0'){
		inputString[l]=input[l];
		l++;
	}
	
	//break up the inputString
    opcode = strtok(inputString, " ");
	//char* commandArgs[4];
	//int argumentCount = 0;
	
	
	//parse and extract arguments
	char commandArgs[4][7];
	int argumentCount = 0;
	int trav = 0;
	int column = 0;
	int row = 0;
	
	while(input[trav] != '\0'){
		if(input[trav] == ' '){
			commandArgs[argumentCount][column] = '\0';
			column = 0;
			trav++;
			argumentCount++;
			continue;
		}
		commandArgs[argumentCount][column] = input[trav];
		column++;
		trav++;
	}
	commandArgs[argumentCount][column] = '\0';
	argumentCount++;
	
	
	int v = 0;
	for (v = 0; v < argumentCount; v++){
		printf("TEMPARGS[%s]\n",commandArgs[v]);
		
	}
	
	
	
	//store arguments in commandArgs array and count arguments
	
	/*while (opcode != NULL)
	  {
		commandArgs[argumentCount] = opcode;
		//printf ("AA%s\n",opcode);
		opcode = strtok(NULL, " ");
		argumentCount++;
	  }*/
	  
	  
	 int temp = 0;
	 if (!strcmp("add",commandArgs[0])){
		 output.opcode = add;
		if(argumentCount != 4){
			 printf("ERROR: WRONG NUMBER OF ARGUMENTS\n");
			 exit(0);
		 }
		 
		 //rs
		 if (!strcmp("0",commandArgs[1])){
			 output.rs = 0;
		 }
		 else{
			 output.rs = atoi(commandArgs[1]);
		 }
		 //rt
		 if (!strcmp("0",commandArgs[2])){
			 output.rt = 0;
		 }
		 else{
			 output.rt = atoi(commandArgs[2]);
		 }
		 
		 //rd
		 if (!strcmp("0",commandArgs[3])){
			 output.rd = 0;
		 }
		 else{
			 output.rd = atoi(commandArgs[3]);
		 }
		 output.Imm = 0;
		 return output;
	 }
	 
	 else if (!strcmp("sub",commandArgs[0])){
		 output.opcode = sub;
		 if(argumentCount != 4){
			 printf("ERROR: WRONG NUMBER OF ARGUMENTS\n");
			 exit(0);
		 }
		 
		 //rs
		 if (!strcmp("0",commandArgs[1])){
			 output.rs = 0;
		 }
		 else{
			 output.rs = atoi(commandArgs[1]);
		 }
		 //rt
		 if (!strcmp("0",commandArgs[2])){
			 output.rt = 0;
		 }
		 else{
			 output.rt = atoi(commandArgs[2]);
		 }
		 
		 //rd
		 if (!strcmp("0",commandArgs[3])){
			 output.rd = 0;
		 }
		 else{
			 output.rd = atoi(commandArgs[3]);
		 }
		 output.Imm = 0;
		 return output;
	 }
	 else if (!strcmp("addi",commandArgs[0])){
		 output.opcode = addi;
		 if(argumentCount != 4){
			 printf("ERROR: WRONG NUMBER OF ARGUMENTS\n");
			 exit(0);
		 }
		 
		 //rs
		 if (!strcmp("0",commandArgs[1])){
			 output.rs = 0;
		 }
		 else{
			 output.rs = atoi(commandArgs[1]);
		 }
		 //rt
		 if (!strcmp("0",commandArgs[2])){
			 output.rt = 0;
		 }
		 else{
			 output.rt = atoi(commandArgs[2]);
		 }
		 
		 //Imm
		 if (!strcmp("0",commandArgs[3])){
			 output.Imm = 0;
		 }
		 else{
			 
			 if(65536 <= (unsigned)atoi(commandArgs[3])){
				 printf("ERROR: IMMEDIATE VALUE TOO LARGE\n");
				 exit(0);
			 }
			 output.Imm = atoi(commandArgs[3]);
		 }
		 
		 output.rd = 0;
		 
		 return output;
	 }
	 else if (!strcmp("mul",commandArgs[0])){
		 output.opcode = mult;
		 if(argumentCount != 4){
			 printf("ERROR: WRONG NUMBER OF ARGUMENTS\n");
			 exit(0);
		 }
		 
		 //rs
		 if (!strcmp("0",commandArgs[1])){
			 output.rs = 0;
		 }
		 else{
			 output.rs = atoi(commandArgs[1]);
		 }
		 //rt
		 if (!strcmp("0",commandArgs[2])){
			 output.rt = 0;
		 }
		 else{
			 output.rt = atoi(commandArgs[2]);
		 }
		 
		 //rd
		 if (!strcmp("0",commandArgs[3])){
			 output.rd = 0;
		 }
		 else{
			 output.rd = atoi(commandArgs[3]);
		 }
		 
		 //Imm
		 output.Imm = 0;
		 return output;
		 
		 
	 }
	 else if (!strcmp("lw",commandArgs[0])){
		 output.opcode = lw;
		 if(argumentCount == 4){
			 
					//rs
					 if (!strcmp("0",commandArgs[1])){
						 output.rs = 0;
					 }
					 else{
						 output.rs = atoi(commandArgs[1]);
					 }
					 //rt
					 if (!strcmp("0",commandArgs[3])){
						 output.rt = 0;
					 }
					 else{
						 output.rt = atoi(commandArgs[3]);
					 }
					 
					 //Imm
					 if (!strcmp("0",commandArgs[2])){
						 output.Imm = 0;
					 }
					 else{
						 
						 if(65536 <= (unsigned)atoi(commandArgs[2])){
							 printf("ERROR: IMMEDIATE VALUE TOO LARGE\n");
							 exit(0);
						 }
						 output.Imm = atoi(commandArgs[2]);
					 }
					 
					 output.rd = 0;
						 
		 }
		 else if(argumentCount == 3){
					//rs
				 if (!strcmp("0",commandArgs[1])){
					 output.rs = 0;
				 }
				 else{
					 output.rs = atoi(commandArgs[1]);
				 }
				 //rt
				 if (!strcmp("0",commandArgs[2])){
					 output.rt = 0;
				 }
				 else{
					 output.rt = atoi(commandArgs[2]);
				 }
				 output.rd = 0;
				 output.Imm = 0;
		 }
		 else{
			 printf("ERROR: WRONG NUMBER OF ARGUMENTS\n");
			 exit(0);
		 }
		 return output;
	 }
	 else if (!strcmp("sw",commandArgs[0])){
		 output.opcode = sw;
		 if(argumentCount == 4){
			 
					//rs
					 if (!strcmp("0",commandArgs[1])){
						 output.rs = 0;
					 }
					 else{
						 output.rs = atoi(commandArgs[1]);
					 }
					 //rt
					 if (!strcmp("0",commandArgs[3])){
						 output.rt = 0;
					 }
					 else{
						 output.rt = atoi(commandArgs[3]);
					 }
					 
					 //Imm
					 if (!strcmp("0",commandArgs[2])){
						 output.Imm = 0;
					 }
					 else{
						 
						 if(65536 <= (unsigned)atoi(commandArgs[2])){
							 printf("ERROR: IMMEDIATE VALUE TOO LARGE\n");
							 exit(0);
						 }
						 output.Imm = atoi(commandArgs[2]);
					 }
					 
					 output.rd = 0;
						 
		 }
		 else if(argumentCount == 3){
					//rs
				 if (!strcmp("0",commandArgs[1])){
					 output.rs = 0;
				 }
				 else{
					 output.rs = atoi(commandArgs[1]);
				 }
				 //rt
				 if (!strcmp("0",commandArgs[2])){
					 output.rt = 0;
				 }
				 else{
					 output.rt = atoi(commandArgs[2]);
				 }
				 output.rd = 0;
				 output.Imm = 0;
		 }
		 else{
			 printf("ERROR: WRONG NUMBER OF ARGUMENTS\n");
			 exit(0);
		 }
		 return output;
	 }
	 else if (!strcmp("beq",commandArgs[0])){
		 output.opcode = beq;
		 
		 if(argumentCount != 4){
			 printf("ERROR: WRONG NUMBER OF ARGUMENTS\n");
			 exit(0);
		 }
		 
		 //rs
		 if (!strcmp("0",commandArgs[1])){
			 output.rs = 0;
		 }
		 else{
			 output.rs = atoi(commandArgs[1]);
		 }
		 //rt
		 if (!strcmp("0",commandArgs[2])){
			 output.rt = 0;
		 }
		 else{
			 output.rt = atoi(commandArgs[2]);
		 }
		 
		 //Imm
		 if (!strcmp("0",commandArgs[3])){
			 output.Imm = 0;
		 }
		 else{
			 
			 if(65536 <= (unsigned)atoi(commandArgs[3])){
				 printf("ERROR: IMMEDIATE VALUE TOO LARGE\n");
				 exit(0);
			 }
			 output.Imm = atoi(commandArgs[3]);
		 }
		 
		 output.rd = 0;
		 
		 return output;
	 }
	 else if (!strcmp("nop",commandArgs[0])){
		 output.opcode = noop;
		 output.rs = 0;
		 output.rd = 0;
		 output.rt = 0;
		 output.Imm = 0;
		 return output;
	 }
	 else if (!strcmp("haltSimulation",commandArgs[0])){
		 output.opcode = haltSimulation;
		 output.rs = 0;
		 output.rd = 0;
		 output.rt = 0;
		 output.Imm = 0;
		 
		 //DO WHAT
		 return output;
	 }
	 else{
		//ILLEGAL OPCODE DETECTED
		printf("ERROR: ILLEGAL OPCODE DETECTED\n");
		exit(0);
	 }
	return output;
}


int IF(int c, int pgm_c, struct inst *instruct){
        if((BRANCH_PENDING!=0)||(RAW_HAZARD!=0)){//instroduce noop to the system
            struct inst toBuffer;
            toBuffer.opcode = noop;
            IFID.instruction = toBuffer;
            IFCTDN=IFCTDN+c;
            return c;
        }
        if(DECODE_UNFINISHED!=0){//do nothing because decode is not finished reading from bufffer
            return c;
        }
        IFID.instruction = instruct[pgm_c];
        IFID.readyToRead = 1;
        IFCTDN = 1;
    IFCTDN = IFCTDN + c;
    IFID.readyToRead=0;
    return c;
}

struct buffer ID(long *registers, struct buffer IfId){  //please make sure that if opcode is 'halt_program' everything stops and control is returned to main()
	if(IfId.readyToRead == 0){
		//not ready to read
		return IDEX;
	}
	if(IfId.instruction.opcode == haltSimulation){
	//halt simulation instruction detected, propagate the halt instruction
	return IfId;
	}
	
	
	switch(IfId.instruction.opcode){
		case add:
			IfId.instruction.rs = registers[IfId.instruction.rs];
			IfId.instruction.rt = registers[IfId.instruction.rt];
			IDEX = IfId;
			return IfId;
		case addi:
			IfId.instruction.rd = IfId.instruction.rt;
			IfId.instruction.rs = registers[IfId.instruction.rs];
			IfId.instruction.rt = registers[IfId.instruction.rt];
			IDEX = IfId;
			return IfId;
			
		case sub:
			IfId.instruction.rs = registers[IfId.instruction.rs];
			IfId.instruction.rt = registers[IfId.instruction.rt];
			IDEX = IfId;
			return IfId;
			
		case mult:
			IfId.instruction.rs = registers[IfId.instruction.rs];
			IfId.instruction.rt = registers[IfId.instruction.rt];
			IDEX = IfId;
			return IfId;
			
		case beq:
			IfId.instruction.rs = registers[IfId.instruction.rs];
			IfId.instruction.rt = registers[IfId.instruction.rt];
            IDEX = IfId;
			return IfId;
			
		case lw:
			IfId.instruction.rs = registers[IfId.instruction.rs];
			IfId.instruction.rt = registers[IfId.instruction.rt];
			IDEX = IfId;
			return IfId;
		
		case sw:
			IfId.instruction.rd = IfId.instruction.rt;
			IfId.instruction.rs = registers[IfId.instruction.rs];
			IfId.instruction.rt = registers[IfId.instruction.rt];
            IDEX = IfId;
			
			return IfId;
			
		case haltSimulation:
			IDEX = IfId;
			return IfId;
			
		case noop:
			IDEX = IfId;
			return IfId;
		
	}
	

}

int EX(int n, int m){
    if(IDEX.instruction.opcode==noop){
        EXECUTE_UNFINISHED = 0;
        EXMEM.readyToRead = 1;
        return n;
    }
    if(IDEX.instruction.opcode==add){
        EXMEM.data = IDEX.instruction.rs+IDEX.instruction.rt;
        EXMEM.wbReg = IDEX.instruction.rd;
        EXMEM.address = -1;  //so you know nothing needs to be written to memory!
        EXMEM.readyToRead = 1;
        EXCTDN = EXCTDN + n;
        //add to useful process count
        return n;
    }
    if(IDEX.instruction.opcode==addi){
        EXMEM.data = IDEX.instruction.rs+IDEX.instruction.Imm;
        EXMEM.wbReg = IDEX.instruction.rt;
        EXMEM.address = -1;
        EXCTDN = EXCTDN + n;
        EXMEM.readyToRead = 1;
        return n;
    }
    if(IDEX.instruction.opcode==sub){
        EXMEM.data = IDEX.instruction.rs-IDEX.instruction.rt;
        EXMEM.wbReg = IDEX.instruction.rd;
        EXMEM.address = -1;  //so you know nothing needs to be written to memory!
        EXCTDN = EXCTDN + n;
        EXMEM.readyToRead = 1;
        return n;
    }
    if(IDEX.instruction.opcode==beq){
        BRANCH_PENDING=1; //maybe should be done in the ID stage
        if((IDEX.instruction.rs-IDEX.instruction.rt)==0){
            PC=PC+IDEX.instruction.Imm;
        }
        EXMEM.wbReg = -1;  //do not write anything to reg file
        EXMEM.address = -1;
        BRANCH_PENDING = 0;
        EXCTDN = EXCTDN + n;
        EXMEM.readyToRead = 1;
        return n;
    }
    if(IDEX.instruction.opcode==lw){
        EXMEM.wbReg = IDEX.instruction.rt;
        EXMEM.address = IDEX.instruction.rs+IDEX.instruction.Imm;
        EXCTDN = EXCTDN + n;
        EXMEM.readyToRead = 1;
        return n;
    }
    if(IDEX.instruction.opcode==sw){
        EXMEM.data = IDEX.instruction.rt;
        EXMEM.address = IDEX.instruction.rs+IDEX.instruction.Imm;
        EXCTDN = EXCTDN + n;
        EXMEM.readyToRead = 1;
        return n;  
    }
    if(IDEX.instruction.opcode==haltSimulation){
    	EXMEM.instruction = IDEX.instruction;
    	EXMEM.wbReg = -1;
    	EXMEM.address = -1;
    	return n;
    }
        if(IDEX.instruction.opcode==mult){
            int result = IDEX.instruction.rs*IDEX.instruction.rt;
            result = result|0x0000ffff; //making sure the result if only the low reg
            EXMEM.data = result;
            EXMEM.wbReg = IDEX.instruction.rd;
            EXMEM.address = -1;
            EXMEM.readyToRead = 1;
            EXCTDN = EXCTDN + m;
            return m;
            
        }
    return 0;
}

//takes previous buffer and return the total cycles taken so far
int MEM(int cycles_counter, int mem_cycles, struct buffer ExeMem){//should take sim_cycle, c from command line and last buffer
	int address = ExeMem.instruction.rs + ExeMem.instruction.Imm;

	if(ExeMem.instruction.opcode == sw){
		ExeMem.instruction.rt = dataMemory[address];

		cycles_counter += mem_cycles;
		MEM_cycle_count += mem_cycles;

	}else if(ExeMem.instruction.opcode == lw){
		dataMemory[address] = ExeMem.instruction.rt; 

		cycles_counter += mem_cycles;
		MEM_cycle_count += mem_cycles;
	}

	MEMWB = ExeMem;

	return cycles_counter;
}

//takes previous buffer and returns total cycles taken so far
int WB(int cycles_count, long *registers, struct buffer MemWb){
	if((MemWb.instruction.opcode >= add) || (MemWb.instruction.opcode <= mult)){
		registers[MemWb.instruction.rd] = MemWb.data;

		WB_cycle_count++;
		cycles_count++;

	}else if(MemWb.instruction.opcode == sw){
		registers[MemWb.instruction.rd] = MemWb.instruction.rt;

		WB_cycle_count++;
		cycles_count++;
	}

	if(MemWb.instruction.opcode == haltSimulation){
		HALT_SIMULATION = 1;
	}


	return cycles_count;
}
