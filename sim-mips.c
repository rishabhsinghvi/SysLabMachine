// Author: Aleksa Deric, Noah Sheridan, Peter Stanwicks
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#define SINGLE 1
#define BATCH 0
#define REG_NUM 32

int BRANCH_PENDING = 0;
int RAW_HAZARD = 0;
int HALT_SIMULATION = 0;
int IF_cycle_count = 0;
int ID_cycle_count = 0;
int EX_cycle_count = 0;
int MEM_cycle_count = 0;
int WB_cycle_count = 0;

enum opcode {add, addi, sub, mult, beq, lw, sw, haltSimulation, noop};

long dataMemory[512]; //2kb of 32-bit words

struct inst
{
	enum opcode opcode;
	int rs;
	int rt;
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
	int data;
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
int ID(long *registers, struct buffer IfId);								//author: Aleksa,	tester: Noah, 
int EX(int n, int m, int *p);						//author: Noah,		tester: Aleksa, Peter, Done in testing
int MEM(int mem_cycles, struct buffer ExeMem); 	//author: Peter,	tester: Aleksa
int WB(long *registers, struct buffer MemWb);					//author: Aleksa,	tester: Noah
char *progScanner(char *c); 				//author: Peter,	tester: Noah,  tested
char *regNumberConverter(char *prog); 		//author: Aleksa,	tester:	Peter, tested
struct inst parser(char *input);			//author: Noah,		tester: Peter, Done
//main  									//author: Peter


void printBuffer(struct buffer b){
	//printf("Instruction\n");
	//printInst(b.instruction);
	printf("ready to read:%d\nready to write:%d\ndata:%d\n", b.readyToRead, b.readytoWrite, b.data);
}

void initLatches(){
IFID.readyToRead = 0;
IFID.readytoWrite = 1;
IFID.address = -1;
IFID.wbReg = -1;
IFID.VALID = 1;
IFID.data = 0;

IDEX.readyToRead = 0;
IDEX.readytoWrite = 1;
IDEX.address = -1;
IDEX.wbReg = -1;
IDEX.VALID = 1;
IDEX.data = 0;

EXMEM.readyToRead = 0;
EXMEM.readytoWrite = 1;
EXMEM.address = -1;
EXMEM.wbReg = -1;
EXMEM.VALID = 1;
EXMEM.data = 0;

MEMWB.readyToRead = 0;
MEMWB.readytoWrite = 1;
MEMWB.address = -1;
MEMWB.wbReg = -1;
MEMWB.VALID = 1;
MEMWB.data = 0;
}

void printInst(struct inst a){
	printf("\nopcode: %d\nrs:%d\nrt:%d\nrd:%d\nImm:%d\n\n", a.opcode, a.rs, a.rt,a.rd,a.Imm);
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

int str_len(char str[]){
	int i;
	for(i = 0; str[i] != '\0';++i);

	return i;
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
		printf("%d >%s< [%s]\n", i,progScanner(line),regNumberConverter(progScanner(line)));

		instructions[i] = parser(regNumberConverter(progScanner(line)));
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
	if(sim_mode==1 || sim_mode==0){
		for (i=0;i<REG_NUM;i++){
			mips_reg[i]=0;
		} 
	}
	

	//start your code from here **************************************************
	struct inst *Inst_Mem;
	Inst_Mem = readFile(input);
	initLatches();HALT_SIMULATION = 0;
	int IF_c,ID_c,EX_c,MEM_c,WB_c; 
	IF_c = 0; ID_c = 0; EX_c = 0; MEM_c = 0; WB_c = 0;


	while(!HALT_SIMULATION){
		printf("\n\n\nPROGRAM COUNT:%d    ##################\n",pgm_c);

		printf("%s\n", "WB");
		WB_c += WB(mips_reg, MEMWB);

		printf("%s\n", "MEM");
		MEM_c += MEM(m,EXMEM);
		printInst(MEMWB.instruction);
		printBuffer(MEMWB);

		printf("%s\n", "Execute");
		EX_c += EX(n,m,&pgm_c);
		printInst(EXMEM.instruction); //printf("DATA: %d\n\n", EXMEM.data);
		printBuffer(EXMEM);

		printf("%s\n", "Instruction Decode");
		ID_c += ID(mips_reg, IFID);
		printInst(IDEX.instruction);
		printBuffer(IDEX);

		printf("%s\n", "Instruction Fetch");
		IF_c += IF(c,pgm_c,Inst_Mem);
		printInst(IFID.instruction);
		printBuffer(IFID);



		if(!RAW_HAZARD){
			pgm_c++;
		}

		/*
		int j;
		for(j = 0; j < 32; j++){
			printf("%d ", mips_reg[j]);
			if((j+1)%4 == 0)
				printf("\n");
		}
		*/
		

		//output code 2: the following code will output the register 
	        //value to screen at every cycle and wait for the ENTER key
	        //to be pressed; this will make it proceed to the next cycle 
		
		if(sim_mode==1){
			printf("cycle: %d register value: ",sim_cycle);
			for (i=1;i<REG_NUM;i++){
				printf("%d  ",mips_reg[i]);
			}
			printf("program counter: %d\n",pgm_c);
			printf("press ENTER to continue\n");
			while(getchar() != '\n');
		}
		
		sim_cycle+=1;
		


	}

 



	IF_c -= c;//remove cycle count for halt sim

	double ifUtil, idUtil, exUtil, memUtil, wbUtil;
	ifUtil = (IF_c*1.0)/sim_cycle;
	idUtil = (ID_c*1.0)/sim_cycle;
	exUtil = (EX_c*1.0)/sim_cycle;
	memUtil = (MEM_c*1.0)/sim_cycle;
	wbUtil = (WB_c*1.0)/sim_cycle;	


	if(sim_mode==0){
		fprintf(output,"program name: %s\n",argv[5]);
		fprintf(output,"stage utilization: %f  %f  %f  %f  %f \n",
                             ifUtil, idUtil, exUtil, memUtil, wbUtil);
                     // add the (double) stage_counter/sim_cycle for each 
                     // stage following sequence IF ID EX MEM WB
		
		fprintf(output,"register values ");
		for (i=1;i<REG_NUM;i++){
			fprintf(output,"%d  ",mips_reg[i]);
		}
		fprintf(output,"%d\n",pgm_c);
	
	}
	//close input and output files at the end of the simulation
	fclose(input);
	fclose(output);
	return 0;
}

//
char *progScanner(char *c){

	//char *ret; ret = (char*)calloc(strlen(c)+1, sizeof(char));
	int len,paren; paren = 0;
	len = str_len(c) + 1;
	char *ret; ret = (char*)malloc((len)*sizeof(char));strcpy(ret,"");
	char t[2];
	char space;space=0;

	int i;
	for (i = 0; i < strlen(c); i++){

		if(c[i] == '\n')
			return ret;

		if( ('a' <= c[i] && c[i] <= 'z') || ('A' <= c[i] && c[i] <= 'Z') || ('0' <= c[i] && c[i] <= '9') || c[i] == '$'){
			
			if(space){
				t[0] = ' ';
				t[1] = '\0';
				str_cat(ret, t);
			}


			t[0] = c[i];
			t[1] = '\0';
			str_cat(ret, t);

			space = 0;
		}else if((c[i] == ' ') || (c[i] == ',') || (c[i] == '(')){
			space = 1;
		}


		if(c[i] == '('){
			paren++;
		}else if(c[i] == ')'){
			paren--;
		}

		//printf("%s\n",ret);
	}

	if(paren != 0){
		printf("%s\n", "Mismatched paranthesis");
		exit(0);
	}


	/*
	for (i = strlen(ret); i >= 0; --i){
		if(ret[i] >='0' && ret[i] <= '9'){
			printf("LAST LETTER:%c\n", ret[i]);
			ret[i+1]='\0';
			break;
		}

	}
	*/
	
	
	if(ret[strlen(ret)-1] == ' ')
		ret[strlen(ret)-1] = '\0';

	return ret;

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
	
	inputString[strlen(input)] = '\0';  //removed +1 index
	
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
		 if (!strcmp("0",commandArgs[2])){
			 output.rs = 0;
		 }
		 else{
			 output.rs = atoi(commandArgs[2]);
		 }
		 //rt
		 if (!strcmp("0",commandArgs[3])){
			 output.rt = 0;
		 }
		 else{
			 output.rt = atoi(commandArgs[3]);
		 }
		 
		 //rd
		 if (!strcmp("0",commandArgs[1])){
			 output.rd = 0;
		 }
		 else{
			 output.rd = atoi(commandArgs[1]);
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
		 if (!strcmp("0",commandArgs[2])){
			 output.rs = 0;
		 }
		 else{
			 output.rs = atoi(commandArgs[2]);
		 }
		 //rt
		 if (!strcmp("0",commandArgs[3])){
			 output.rt = 0;
		 }
		 else{
			 output.rt = atoi(commandArgs[3]);
		 }
		 
		 //rd
		 if (!strcmp("0",commandArgs[1])){
			 output.rd = 0;
		 }
		 else{
			 output.rd = atoi(commandArgs[1]);
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
		 if (!strcmp("0",commandArgs[2])){
			 output.rs = 0;
		 }
		 else{
			 output.rs = atoi(commandArgs[2]);
		 }
		 //rt
		 if (!strcmp("0",commandArgs[1])){
			 output.rt = 0;
		 }
		 else{
			 output.rt = atoi(commandArgs[1]);
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
		 if (!strcmp("0",commandArgs[2])){
			 output.rs = 0;
		 }
		 else{
			 output.rs = atoi(commandArgs[2]);
		 }
		 //rt
		 if (!strcmp("0",commandArgs[3])){
			 output.rt = 0;
		 }
		 else{
			 output.rt = atoi(commandArgs[3]);
		 }
		 
		 //rd
		 if (!strcmp("0",commandArgs[1])){
			 output.rd = 0;
		 }
		 else{
			 output.rd = atoi(commandArgs[1]);
		 }
		 
		 //Imm
		 output.Imm = 0;
		 return output;
		 
		 
	 }
	 else if (!strcmp("lw",commandArgs[0])){
		 output.opcode = lw;
		 if(argumentCount == 4){
			 
					//rs
					 if (!strcmp("0",commandArgs[3])){
						 output.rs = 0;
					 }
					 else{
						 output.rs = atoi(commandArgs[3]);
					 }
					 //rt
					 if (!strcmp("0",commandArgs[1])){
						 output.rt = 0;
					 }
					 else{
						 output.rt = atoi(commandArgs[1]);
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
				 if (!strcmp("0",commandArgs[2])){
					 output.rs = 0;
				 }
				 else{
					 output.rs = atoi(commandArgs[2]);
				 }
				 //rt
				 if (!strcmp("0",commandArgs[1])){
					 output.rt = 0;
				 }
				 else{
					 output.rt = atoi(commandArgs[1]);
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
					 if (!strcmp("0",commandArgs[3])){
						 output.rs = 0;
					 }
					 else{
						 output.rs = atoi(commandArgs[3]);
					 }
					 //rt
					 if (!strcmp("0",commandArgs[1])){
						 output.rt = 0;
					 }
					 else{
						 output.rt = atoi(commandArgs[1]);
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
				 if (!strcmp("0",commandArgs[2])){
					 output.rs = 0;
				 }
				 else{
					 output.rs = atoi(commandArgs[2]);
				 }
				 //rt
				 if (!strcmp("0",commandArgs[1])){
					 output.rt = 0;
				 }
				 else{
					 output.rt = atoi(commandArgs[1]);
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
		 if (!strcmp("0",commandArgs[2])){
			 output.rs = 0;
		 }
		 else{
			 output.rs = atoi(commandArgs[2]);
		 }
		 //rt
		 if (!strcmp("0",commandArgs[1])){
			 output.rt = 0;
		 }
		 else{
			 output.rt = atoi(commandArgs[1]);
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

    if(IFID.readytoWrite && !BRANCH_PENDING){
        IFID.instruction = instruct[pgm_c];
        IFID.readyToRead = 1;
        IFID.readytoWrite = 0;
        IF_cycle_count = IF_cycle_count + c;
        return c;
	}

	return c;
}

int ID(long *registers, struct buffer IfId){  

	if(IDEX.readytoWrite && IFID.readyToRead && !BRANCH_PENDING){


		if((IFID.instruction.rs != 0) && ((IFID.instruction.rs == IDEX.instruction.rd) || (IFID.instruction.rs  == EXMEM.instruction.rd) || (IFID.instruction.rs == MEMWB.instruction.rd))){
			printf("RAW HAZARD, Register:%d\n", IFID.instruction.rs);
			IDEX.instruction.opcode = noop;
			IDEX.instruction.rd = 0;
			IDEX.readyToRead = 1;
			IDEX.readytoWrite = 0;
            ID_cycle_count++;
			RAW_HAZARD = 1;
			return 1;
		}else if((IFID.instruction.rt != 0) && ((IFID.instruction.rt == IDEX.instruction.rd) || (IFID.instruction.rt  == EXMEM.instruction.rd) || (IFID.instruction.rt == MEMWB.instruction.rd))){
			printf("RAW HAZARD, Register:%d\n", IFID.instruction.rt);
			IDEX.instruction.opcode = noop;
			IDEX.instruction.rd = 0;
			IDEX.readyToRead = 1;
			IDEX.readytoWrite = 0;
            ID_cycle_count++;
			RAW_HAZARD = 1;
			return 1;
		}
		RAW_HAZARD = 0;
		
		
		switch(IfId.instruction.opcode){
			case add: //r type
				IfId.instruction.rs = registers[IfId.instruction.rs];
				IfId.instruction.rt = registers[IfId.instruction.rt];
				IDEX = IfId;
				IDEX.readyToRead = 1;
				IDEX.readytoWrite = 0;
				IFID.readytoWrite = 1;
				IFID.readyToRead = 0;
                ID_cycle_count++;
				return 1;//return IfId;
			case addi: // i type
				printf("%s\n", "ADDI");
				IfId.instruction.rd = IfId.instruction.rt;
				IfId.instruction.rs = registers[IfId.instruction.rs];
				IfId.instruction.rt = registers[IfId.instruction.rt];
				IDEX = IfId;
				IDEX.readyToRead = 1;
				IDEX.readytoWrite = 0;
				IFID.readytoWrite = 1;
				IFID.readyToRead = 0;
                ID_cycle_count++;
				return 1;//return IfId;
				
			case sub: // r type
				IfId.instruction.rs = registers[IfId.instruction.rs];
				IfId.instruction.rt = registers[IfId.instruction.rt];
				IDEX = IfId;
				IDEX.readyToRead = 1;
				IDEX.readytoWrite = 0;
				IFID.readytoWrite = 1;
				IFID.readyToRead = 0;
                ID_cycle_count++;
				return 1;//return IfId;
				
			case mult: //r type
				IfId.instruction.rs = registers[IfId.instruction.rs];
				IfId.instruction.rt = registers[IfId.instruction.rt];
				IDEX = IfId;
				IDEX.readyToRead = 1;
				IDEX.readytoWrite = 0;
				IFID.readytoWrite = 1;
				IFID.readyToRead = 0;
                ID_cycle_count++;
				return 1;//return IfId;
				
			case beq: //i type
				IfId.instruction.rs = registers[IfId.instruction.rs];
				IfId.instruction.rt = registers[IfId.instruction.rt];
	            IDEX = IfId;
				IDEX.readyToRead = 1;
				IDEX.readytoWrite = 0;
				IFID.readytoWrite = 1;
				IFID.readyToRead = 0;
				BRANCH_PENDING = 1;
                ID_cycle_count++;
				return 1;//return IfId;
				
			case lw://i type
				IfId.instruction.rd = IfId.instruction.rt;
				IfId.instruction.rs = registers[IfId.instruction.rs];
				IfId.instruction.rt = registers[IfId.instruction.rt];
				if(IfId.instruction.Imm%4 != 0){
					printf("Misaligned Memory\n");
					exit(0);
				}

				IDEX = IfId;
				IDEX.readyToRead = 1;
				IDEX.readytoWrite = 0;
				IFID.readytoWrite = 1;
				IFID.readyToRead = 0;
                ID_cycle_count++;
				return 1;//return IfId;
			
			case sw://i type
				IfId.instruction.rd = IfId.instruction.rt;
				IfId.instruction.rs = registers[IfId.instruction.rs];
				IfId.instruction.rt = registers[IfId.instruction.rt];
				if(IfId.instruction.Imm%4 != 0){
					printf("Misaligned Memory\n");
					exit(0);
				}

	            IDEX = IfId;
				IDEX.readyToRead = 1;
				IDEX.readytoWrite = 0;
				IFID.readytoWrite = 1;
				IFID.readyToRead = 0;
                ID_cycle_count++;
				return 1;//return IfId;
				
			case haltSimulation:
				IDEX = IfId;
				IDEX.readyToRead = 1;
				IDEX.readytoWrite = 0;
				IFID.readytoWrite = 0;
				IFID.readyToRead = 0;
				return 0;//return IfId;
				
			case noop:
				IDEX = IfId;
				IDEX.readyToRead = 1;
				IDEX.readytoWrite = 0;
				IFID.readytoWrite = 1;
				IFID.readyToRead = 0;
				return 0;//return IfId;
			
		}
	}
    return 0;
}

int EX(int n, int m, int *p){


	if(IDEX.readyToRead && EXMEM.readytoWrite){
	    switch(IDEX.instruction.opcode){
		    case noop:
		        EXMEM.readyToRead = 1;
		        EXMEM.readytoWrite = 0;
		        IDEX.readytoWrite = 1;
		        IDEX.readyToRead = 0;
		        EXMEM.instruction = IDEX.instruction;
                EX_cycle_count = EX_cycle_count + n;
		        return 0;
	    	case add:
		        EXMEM.data = IDEX.instruction.rs+IDEX.instruction.rt;
		        EXMEM.wbReg = IDEX.instruction.rd;
		        EXMEM.address = -1;  //so you know nothing needs to be written to memory!
		        EXMEM.readyToRead = 1;
		        EXMEM.readytoWrite = 0;
		        IDEX.readytoWrite = 1;
		        IDEX.readyToRead = 0;
		        EXMEM.instruction = IDEX.instruction;
                EX_cycle_count = EX_cycle_count + n;
		        //add to useful process count
		        return n;
	    	case addi:
		    	//printf("%s   %d     %d\n", "ADDI-EX", IDEX.instruction.rs, IDEX.instruction.Imm);
		        EXMEM.data = IDEX.instruction.rs+IDEX.instruction.Imm;
		        EXMEM.wbReg = IDEX.instruction.rt;
		        EXMEM.address = -1;
		        EXMEM.readyToRead = 1;
		        EXMEM.readytoWrite = 0;
		        IDEX.readytoWrite = 1;
		        IDEX.readyToRead = 0;
		        EXMEM.instruction = IDEX.instruction;
                EX_cycle_count = EX_cycle_count + n;
	        	return n;
	    	case sub:
		        EXMEM.data = IDEX.instruction.rs-IDEX.instruction.rt;
		        EXMEM.wbReg = IDEX.instruction.rd;
		        EXMEM.address = -1;  //so you know nothing needs to be written to memory!
		        EXMEM.readyToRead = 1;
		        EXMEM.readytoWrite = 0;
		        IDEX.readytoWrite = 1;
		        IDEX.readyToRead = 0;
		        EXMEM.instruction = IDEX.instruction;
                EX_cycle_count = EX_cycle_count + n;
		        return n;
	    	case beq:
		        BRANCH_PENDING=1; //maybe should be done in the ID stage
		        if((IDEX.instruction.rs-IDEX.instruction.rt)==0){
		            *p=*p+IDEX.instruction.Imm;/////////********************change program counter
		        }
		        EXMEM.wbReg = -1;  //do not write anything to reg file
		        EXMEM.address = -1;
		        BRANCH_PENDING = 0;
		        EXMEM.readyToRead = 1;
		        EXMEM.readytoWrite = 0;
		        IDEX.readytoWrite = 1;
		        IDEX.readyToRead = 0;
		        EXMEM.instruction = IDEX.instruction;
                EX_cycle_count = EX_cycle_count + n;
		        return n;
			case lw:
		        EXMEM.wbReg = IDEX.instruction.rt; 
		        EXMEM.address = IDEX.instruction.rs+IDEX.instruction.Imm;
		        EXMEM.readyToRead = 1;
		        EXMEM.readytoWrite = 0;
		        IDEX.readytoWrite = 1;
		        IDEX.readyToRead = 0;
		        EXMEM.instruction = IDEX.instruction;
                EX_cycle_count = EX_cycle_count + n;
		        return n;
			case sw:
		        EXMEM.data = IDEX.instruction.rt;
		        EXMEM.address = IDEX.instruction.rs+IDEX.instruction.Imm;
		        EXMEM.readyToRead = 1;
		        EXMEM.readytoWrite = 0;
		        IDEX.readytoWrite = 1;
		        IDEX.readyToRead = 0;
		        EXMEM.instruction = IDEX.instruction;
                EX_cycle_count = EX_cycle_count + n;
		        return n; 
	     	case haltSimulation:
		    	EXMEM.instruction = IDEX.instruction;
		    	EXMEM.wbReg = -1;
		    	EXMEM.address = -1;
		        EXMEM.readyToRead = 1;
		        EXMEM.readytoWrite = 0;
		        IDEX.readytoWrite = 0;
		        IDEX.readyToRead = 0;    	
		    	return 0;
	    	case mult:{
	            int result;
	            result = IDEX.instruction.rs*IDEX.instruction.rt;
	            result = result&0x0000ffff; //making sure the result if only the low reg
	            EXMEM.data = result;
	            EXMEM.wbReg = IDEX.instruction.rd;
		        EXMEM.address = -1;
		        EXMEM.readyToRead = 1;
		        EXMEM.readytoWrite = 0;
		        IDEX.readytoWrite = 1;
		        IDEX.readyToRead = 0;
	            EXMEM.instruction = IDEX.instruction;
                EX_cycle_count = EX_cycle_count + m;
	            return m;
	        }
	    }
	}

	
    return 0;
}

//takes previous buffer and return the total cycles taken so far
int MEM(int mem_cycles, struct buffer ExeMem){//should take sim_cycle, c from command line and last buffer
	int cycles;cycles = 0;

	if(EXMEM.readyToRead && MEMWB.readytoWrite){
		int address = ExeMem.instruction.rs + ExeMem.instruction.Imm;

		if(ExeMem.instruction.opcode == sw){
			//ExeMem.instruction.rt = dataMemory[address];
			dataMemory[address] = ExeMem.instruction.rt;


			cycles += mem_cycles;

		}else if(ExeMem.instruction.opcode == lw){
			//dataMemory[address] = ExeMem.instruction.rt; 
			ExeMem.instruction.rt = dataMemory[address];

			cycles += mem_cycles;
		}
		MEMWB = ExeMem;
		MEMWB.readyToRead = 1;
		MEMWB.readytoWrite = 0;
		EXMEM.readyToRead = 0;
		EXMEM.readytoWrite = 1;
		
	}
	return cycles;


}

//takes previous buffer and returns total cycles taken so far
int WB(long *registers, struct buffer MemWb){
	int cycles;cycles = 0;

	if(MEMWB.readyToRead){

		if((MemWb.instruction.opcode >= add) && (MemWb.instruction.opcode <= mult)){
			registers[MemWb.instruction.rd] = MemWb.data;

			cycles++;

		}else if(MemWb.instruction.opcode == lw){
			registers[MemWb.instruction.rd] = MemWb.instruction.rt;

			cycles++;
		}

		if(MemWb.instruction.opcode == haltSimulation){
			HALT_SIMULATION = 1;
		}

		MEMWB.readytoWrite = 1;
		MEMWB.readyToRead = 0;
	}


	return cycles;
}
