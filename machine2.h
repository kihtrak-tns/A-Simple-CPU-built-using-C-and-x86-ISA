/*
 * Instruction Encoding for Machine 2
 *	Revision History
 *	Added	-- JUMP Instruction
 *			-- R15 as Instruction Pointer 
 */

#define NOP 	0
#define LOAD 	1
#define STORE	2
#define MOVE	3
#define ADD		4
#define MULT	5 
#define JUMP	6				//jump unconditionally -- JUMP <EIP offset>
#define JUMPZ	7				//jump if ZERO FLAG -- JUMPZ <EIP offset>
#define CMP		8				//compare -- CMP <src reg> <immediate value>


#define HALT	0xF 

/*    --- Instruction Encoding Opcode ---
 ----------------------------------------------
 | Size | Immediate | Src | Dst | Instruction |
 ----------------------------------------------
 | 7    |     6     |  5  |  4  |   3:0       |
 ----------------------------------------------

Size 		-- 	1 = WORD, 0 = BYTE
Immediate	--	1 = Immediate Value, 0 = TBD
Src			--	1 = Register Source, 0 = TBD
Dst			--	1 = Register Destination, 0 = TBD
Instruction --	Instruction Type
 ----------------------------------------------*/

#define SIZE_MASK		0x80
#define IMMEDIATE_MASK	0x40
#define SRC_MASK		0x20
#define DST_MASK		0x10
#define INST_MASK 		0x0F


/*    --- Instruction Encoding Source BYTE  ---
 ----------------------------------------------
 |         Src2         |          Src1       | 
 ----------------------------------------------
 |         7:4          |          3:0        | 
 ----------------------------------------------

Src2		--	Register Number for Source 2
Src1		--	Register Number for Source 1
 ----------------------------------------------*/
#define SRC2_REG_MASK	0xF0
#define SRC1_REG_MASK	0x0F

/*    --- Instruction Encoding Destination BYTE ---
 ----------------------------------------------
 |         TBD          |          Dst        | 
 ----------------------------------------------
 |         7:4          |          3:0        | 
 ----------------------------------------------

Dst			--	Register Number for Destination 
 ----------------------------------------------*/

#define DST_REG_MASK	0x0F

/*    --- 16 bit Registers ---

	R[0] -- R[7] 	General Purpose

    R[14]			Status Register
-----------------------------------------------
|             TBD                | Zero Flag  |
-----------------------------------------------
|             15:1               |   0        |
-----------------------------------------------
 
	R[15]			Instruction Pointer

*/	


#define ZERO_FLAG_MASK 0x01
