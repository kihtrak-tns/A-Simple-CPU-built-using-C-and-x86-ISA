/*
 * Machine 1 - Simple CPU to compute y= mx + b
 * Machine 2 -	added JUMP instruction 
 *			 -  executes all instructions in prog2.txt
 *			 -  R[15] = EIP
 */

//#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "machine2.h"

unsigned char imem[100];				//Byte Wide Instruction Memory
unsigned char dmem[100];				//Byte Wide Data Memory

short int R[16];						//16 registers each 16 bits wide

/* --- BEGIN - Fetch Instruction Local Variables --- */
	int imem_addr;
	int imem_addr_limit = 100;
	int dmem_addr_limit = 100;		
	
	int	inst_size;			//Instruction Size
	int	inst_imm;			//Instruction Immediate 
	int	inst_src;			//Instruction Register Source 
	int	inst_dst;			//Instruction Destination Source 
	unsigned char inst;		//Instruction 

	int inst_src_addr;			//Load Source Operand Address
	int inst_dst_addr;			//Store Destination Operand Address
	int inst_dst_reg;			//Destination Register	
	int inst_src1_reg;			//Source 1 Register	
	int inst_src2_reg;			//Source 2 Register	
	short int inst_imm_value;	//16bit Immediate Value

	int	inst_num_bytes;			//number of bytes in current instruction

/* --- END - Fetch Instruction Local Variables --- */

	int exec_next_eip;	//16bit Next EIP Value from Execute Unit 


	int machine_halt = 0;
	int machine_inst_exec_count = 0;			//Number of instructions executed
	int machine_inst_exec_limit = 26;		//Maximum Number of instructions to be executed

void read_text_input();
void read_data_input();
int fetch_decode_instruction();
int execute_instruction();
void dump_registers();
void dump_dmem();
int my_getline(FILE *fp, char s[], int lim);

int main(void)
{
	printf("===== Machine 2 =====\n");
	printf("***** Reading Instruction Memory Contents from prog2.txt *****\n");
	read_text_input();
	printf("***** Reading Data Memory Contents from data2.txt *****\n");
	read_data_input();

	printf("***** Fetch - Decode - Execute *****\n");

	imem_addr = 0;	
	R[15] = imem_addr;				//Initialize Instruction Pointer to 0

	while (!machine_halt && (machine_inst_exec_count < machine_inst_exec_limit) )
	{
		printf("----------------------------------------------------------------------------------------\n");
		inst_num_bytes = fetch_decode_instruction(imem_addr);		//fetch and decode instructions
		
		exec_next_eip = execute_instruction();						//execute instruction
	
		printf("Executed Instruction at imem_addr = %.4X\n",imem_addr);
		machine_inst_exec_count++;


		imem_addr = imem_addr + inst_num_bytes + exec_next_eip;
		R[15] = imem_addr;				//Update EIP

		dump_registers();
		printf("========================================================================================\n"); 
	}
	printf("***** Machine HALT *****\n");

	dump_dmem();

}

void dump_dmem()
{
	int j;

	printf("--------Dumping DMEM Contents---------\n");

	for (j=0;j<dmem_addr_limit;j++)					//dump imem
		printf("dmem[%.4X] = %.2X\n",j,dmem[j]);
}


void dump_registers()
{
	int i,j;

	printf("STATUS Register - R[14]\n");
	printf("\tZERO FLAG = %d\n",R[14] & ZERO_FLAG_MASK);
	printf("Register DUMP\n");

	for(i=0;i<16;i++)
		printf("R[%.2d] = %.4X\n",i,R[i]);
	
//	for(i=0;i<4;i++)
//	{
//		for(j=0;j<4;j++)
//			printf("R[%.2d] = %.4X\t\t",i*4 +j,R[i*4 +j]);
//		printf("\n");
//	}
}

/* --- void execute_instruction ---
 *
 */
int execute_instruction()
{

		int exec_dvalue;
		int	exec_src1_value;
		int	exec_src2_value;
		int	exec_dst_value;
		int exec_store_addr;			//Store Address

		switch(inst)
		{
/*  -------------------- LOAD INSTRUCTION --------------------
	--- LOAD Memory Contents into Register                 ---
	inst_src_addr = address of memory location 
	inst_dst_reg = destination register 
	----------------------------------------------------------
*/
			case LOAD:
						if (inst_size == 0)		//Byte Size
						{
								exec_dvalue = dmem[inst_src_addr];	
								R[inst_dst_reg] = R[inst_dst_reg] & 0xFF00;
								R[inst_dst_reg] = R[inst_dst_reg] | exec_dvalue;
						}
						break;

/*  -------------------- MULT INSTRUCTION --------------------
	--- MULT 8bit src1 register to 8bit src2 register      ---
	inst_src1_reg = src1 register
	inst_src2_reg = src2 register 
	inst_dst_reg = destination register 
	----------------------------------------------------------
*/
			case MULT:
						if (inst_size == 0)		//Byte Size
						{
								exec_src1_value = R[inst_src1_reg] & 0x00FF;
								exec_src2_value = R[inst_src2_reg] & 0x00FF;
								exec_dst_value = exec_src1_value * exec_src2_value;
								R[inst_dst_reg] = exec_dst_value & 0xFFFF;
						}
						break;

/*  -------------------- ADD INSTRUCTION --------------------
	--- ADD src1 register to src2 register  			  ---
	inst_src1_reg = src1 register
	inst_src2_reg = src2 register 
	inst_dst_reg = destination register 
	----------------------------------------------------------
*/
			case ADD:
						if (inst_size == SIZE_MASK)			//Word Size
						{
								exec_dst_value = R[inst_src1_reg] + R[inst_src2_reg];
								R[inst_dst_reg] = exec_dst_value & 0xFFFF;
						}
						break;

/*  -------------------- MOVE INSTRUCTION --------------------
	--- move 16 bit immediate value into register ---
	inst_imm_value = 16 bit immediate value
	inst_dst_reg = destination register for immediate value
	----------------------------------------------------------
*/
			case MOVE:
						if (inst_size == SIZE_MASK)			//Word Size
						{
							R[inst_dst_reg] = inst_imm_value;			//move 16bit immediate into destination register
						}
						break;
						
/*  -------------------- STORE INSTRUCTION -------------------
	--- Store Register to memory location                  ---
	inst_src1_reg = register contents to write to memory 
	inst_dst_reg = register containing address of memory location
	----------------------------------------------------------
*/
			case STORE:
						if (inst_size == SIZE_MASK)			//Word Size
						{
								exec_store_addr = R[inst_dst_reg] & 0xFFFF;
								dmem[exec_store_addr] = R[inst_src1_reg];
						}
						break;

			case JUMP:
						if (inst_size == SIZE_MASK)			//Word Size
						{
								return(inst_imm_value);		//next eip = jump offset

						}
						break;


			default:
						break;
		}

		return(0);			//next eip = 0

}


/*
 * --- int fetch_decode_instruction(int imem_addr)  ---
 * Fetch instruction from imem[imem_addr]
 * Decode instruction and return # of bytes in instruction
 */
int fetch_decode_instruction(int imem_addr)
{
	unsigned char ibyte[3];

		ibyte[0] = imem[imem_addr];		
		ibyte[1] = imem[imem_addr+1];		
		ibyte[2] = imem[imem_addr+2];		
		ibyte[3] = imem[imem_addr+3];		

		inst_size = ibyte[0] & SIZE_MASK;
		inst_imm = ibyte[0] & IMMEDIATE_MASK;
		inst_src = ibyte[0] & SRC_MASK;
		inst_dst = ibyte[0] & DST_MASK;
		inst = ibyte[0] & INST_MASK;

/*  -------- LOAD INSTRUCTION ----------
	BYTE0 BYTE1 BYTE2 BYTE3
	BYTE0 -- OPCODE
	BYTE1 -- Destination Register
	BYTE2 -- Source Address (Low Byte)
	BYTE3 -- Source Address (High Byte)
	-----------------------------------
*/
		if (inst == LOAD)
		{
			inst_dst_reg = ibyte[1] & DST_REG_MASK;
			inst_src_addr = (ibyte[3] << 8) + ibyte[2];
			printf("FETCH ADDR: %.4X -- INST: LOAD [%.4X], R%d \n",imem_addr,inst_src_addr,inst_dst_reg);
			return(4);
		}

/*  -------- MULT INSTRUCTION ----------
	BYTE0 BYTE1 BYTE2 
	BYTE0 -- OPCODE
	BYTE1 -- Source Register
	BYTE2 -- Destination Register 
	-----------------------------------
*/
		if (inst == MULT)
		{
			inst_dst_reg = ibyte[2] & DST_REG_MASK;
			inst_src1_reg = ibyte[1] & SRC1_REG_MASK;
			inst_src2_reg = (ibyte[1] & SRC2_REG_MASK) >> 4;
			printf("FETCH ADDR: %.4X -- INST: MULT R%d, R%d, R%d \n",imem_addr,inst_src1_reg,inst_src2_reg,inst_dst_reg);
			return(3);
		}
/*  -------- ADD INSTRUCTION ----------
	BYTE0 BYTE1 BYTE2 
	BYTE0 -- OPCODE
	BYTE1 -- Source Register
	BYTE2 -- Destination Register 
	-----------------------------------
*/

		if (inst == ADD)
		{
			inst_dst_reg = ibyte[2] & DST_REG_MASK;
			inst_src1_reg = ibyte[1] & SRC1_REG_MASK;
			inst_src2_reg = (ibyte[1] & SRC2_REG_MASK) >> 4;
			printf("FETCH ADDR: %.4X -- INST: ADD R%d, R%d, R%d \n",imem_addr,inst_src1_reg,inst_src2_reg,inst_dst_reg);
			return(3);
		}

/*  -------- MOVE INSTRUCTION ----------
	BYTE0 BYTE1 BYTE2 BYTE3
	BYTE0 -- OPCODE
	BYTE1 -- Destination Register 
	BYTE2 -- Immediate Low Byte 
	BYTE3 -- Immediate High Byte 
	-----------------------------------
*/
		if (inst == MOVE)
		{
			inst_imm_value = (ibyte[3] << 8) + ibyte[2];	
			inst_dst_reg = ibyte[1] & DST_REG_MASK;
			printf("FETCH ADDR: %.4X -- INST: MOVE %.4X, R%d \n",imem_addr,inst_imm_value,inst_dst_reg);
			return(4);
		}


/*  -------- STORE INSTRUCTION ----------
	BYTE0 BYTE1 BYTE2 
	BYTE0 -- OPCODE
	BYTE1 -- Source Register 
	BYTE2 -- Destination Register containing Indirect Destination Address 
	-----------------------------------
*/
		if (inst == STORE)
		{
			inst_src1_reg = ibyte[1] & SRC1_REG_MASK;
			inst_dst_reg = ibyte[2] & DST_REG_MASK;
			inst_dst_addr = R[inst_dst_reg];					//Indirect Addressing
			printf("FETCH ADDR: %.4X -- INST: STORE R%d, [R%d] \n",imem_addr,inst_src1_reg,inst_dst_reg);
			return(3);
		}

//LECTURE 06 - M1

/*  -------- JUMP INSTRUCTION ----------
	BYTE0 BYTE1 BYTE2 
	BYTE0 -- OPCODE
	BYTE1 -- LOW BYTE DISPLACEMENT 
	BYTE2 -- HIGH BYTE DISPLACEMENT 
	-----------------------------------
*/
		if (inst == JUMP)
		{
			inst_imm_value = (ibyte[2] << 8) + ibyte[1];	
			printf("FETCH ADDR: %.4X -- INST: JUMP  Displacement = %.4X\n",imem_addr,inst_imm_value);
			return(3);
		}


/*  -------- NOP INSTRUCTION ----------
	BYTE0 
	BYTE0 -- OPCODE
	-----------------------------------
*/
		if (inst == NOP)
		{
			printf("FETCH ADDR: %.4X -- INST: NOP \n",imem_addr);
			return(1);
		}
	
/*  -------- HALT INSTRUCTION ----------
	BYTE0 
	BYTE0 -- OPCODE
	-----------------------------------
*/
		if (inst == HALT )
		{
			printf("FETCH ADDR: %.4X -- INST: HALT \n",imem_addr);
			machine_halt = 1;		//halt instruction, stop machine 
			return(1);
		}
}


/*
 * read_text_input() -- reads program into instruction memory
 */
void read_text_input()
{

	FILE *fp;

	size_t len = 0;
	ssize_t read;
	char line[80]; 
	char *line_bytes[10];
	char *line_nolinefeed;
	char *line_item;
	int i, j;
	long item;
	unsigned char imem_byte;
	int imem_addr;

	fp = fopen("prog2.txt","r");

	printf("Processing: prog2.txt\n");
	imem_addr = 0;

	while (fgets(line,80,fp))
	{
		if ( line[0]  != '%')			//Non comment lines
		{
//			printf("%s",line); 
		
			i = 0;	
			line_nolinefeed = strtok(line,"\n");	//remove linefeed
			line_item = strtok(line_nolinefeed," ");	//split into fields

			while(line_item != NULL)
			{
				line_bytes[i++] = line_item;
				line_item = strtok(NULL," ");
			}

			for (j=0;j<i;j++)
			{
				item = strtol(line_bytes[j],NULL,16);	//convert from string base 16 into long integer
				imem_byte = (unsigned char) item;
//				printf("line_bytes[%d] = %s item = %ld  imem_byte = %d\n",j,line_bytes[j],item,imem_byte);
				if (j != 0)
					imem[imem_addr++] = imem_byte;		//write imem, skip address
			}

				
		} 
	}
			for (j=0;j<imem_addr;j++)					//dump imem
				printf("imem[%.4X] = %.2X\n",j,imem[j]);

	fclose(fp);

}

void read_data_input()
{

	FILE *fp;

	size_t len = 0;
	ssize_t read;
	char line[80]; 
	char *line_bytes[10];
	char *line_nolinefeed;
	char *line_item;
	int i, j;
	long item;
	unsigned char dmem_byte;
	int dmem_addr;

	fp = fopen("data2.txt","r");

	printf("Processing: data2.txt\n");

	dmem_addr = 0;
	while (fgets(line,80,fp))
	{
		if ( line[0]  != '%')			//Non comment lines
		{
//			printf("%s",line); 
		
			i = 0;	
			line_nolinefeed = strtok(line,"\n");		//remove linefeed
			line_item = strtok(line_nolinefeed," ");	//split into fields where <space> is the delimiter

			while(line_item != NULL)					//iterate through all the fields
			{
				line_bytes[i++] = line_item;
				line_item = strtok(NULL," ");
			}

			for (j=0;j<i;j++)
			{
				item = strtol(line_bytes[j],NULL,16);	//convert from string base 16 into long integer
				dmem_byte = (unsigned char) item;
//				printf("line_bytes[%d] = %s item = %ld  imem_byte = %d\n",j,line_bytes[j],item,dmem_byte);
				if (j != 0)
					dmem[dmem_addr++] = dmem_byte;		//write imem, skip address
			}

				
		} 
	}
			for (j=0;j<dmem_addr;j++)					//dump imem
				printf("dmem[%.4X] = %.2X\n",j,dmem[j]);

	fclose(fp);


}








