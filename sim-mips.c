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

enum opcode {add, addi, sub, mult, beq, lw, sw, halt_program};

char* correctOpcode[] = {"add", "addi", "sub", "mult", "beq", "lw", "sw", "halt_program"};

struct inst
{
	enum opcode op;
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
	int data;
};


void IF(void);  							//author: Noah,		tester: Aleksa
void ID(struct inst);								//author: Aleksa,	tester: Noah
void EX(void);								//author: Noah,		tester: Aleksa, Peter
void MEM(void);								//author: Peter,	tester: Aleksa
void WB(void);								//author: Aleksa,	tester: Noah
char *progScannner(char *c); 				//author: Peter,	tester: Noah,  Done and tested
char *regNumberConverter(void); 			//author: Aleksa,	tester:	Peter
struct inst parser(char *input);			//author: Noah,		tester: Peter
//main  									//author: Peter


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
}

//
char *progScannner(char *c){

	char *ret; ret = (char *)malloc(strlen(c));
	char t[2];
	char space,open;

	
	for(int i = 0; i < strlen(c); i++){


		if( ('a' <= c[i] && c[i] <= 'z') || ('A' <= c[i] && c[i] <= 'Z') || ('0' <= c[i] && c[i] <= '9')){
			t[0] = c[i];
			t[1] = '\0';
			strcat(ret, t);

			space = 0;
		}else{
			if(space == 0){
				t[0] = ' ';
				t[1] = '\0';
				strcat(ret, t);
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

char *regNumberConverter(void){

	return 0;
}


struct inst parser(char *input){

	

	struct inst output;
	char* delimeter = " ";
	char* op = strtok(input, delimeter);
    enum opcode command;
    int i;
	for(i=0; i<9; i++){
		if(strcmp(op, correctOpcode[i])==0){
            command = i;
            i = 9;
		}
	}
    if(i==9){
        printf("Oops!  Something was entered wrong!\n");
        output.op = halt_program;  //halt program
        return output;
    }
	
	if(output.op == add || output.op == sub || output.op == mult){
		output.rs = atoi(strtok(NULL, delimeter));  //saving rs value
		output.rt = atoi(strtok(NULL, delimeter));  //saving rt value
		output.rd = atoi(strtok(NULL, delimeter));  //saving rd value
	}
	else{
		output.rs = atoi(strtok(NULL, delimeter));
		output.rt = atoi(strtok(NULL, delimeter));
		output.Imm = atoi(strtok(NULL, delimeter));
		if(output.Imm>=65536){
			printf("Ooop! The immediate value entered was too large!\n");
            output.op = halt_program;
            return output;
		}
	}
	free(delimeter);
    free(op);

	return output;
}


void IF(void){
    if((BRANCH_PENDING!=0)||(DECODE_UNFINISHED!=0)){
        CLK++;  //????
        //do nothing
        return;
    }
    //ID(readInstruction(PC));
    PC++;
}

void ID(struct inst i){  //please make sure that if opcode is 'halt_program' everything stops and control is returned to main()

}

void EX(void){

}

void MEM(void){

}

void WB(void){

}

