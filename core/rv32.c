#include "rv32.h"

// operands
#define RD		(opcode >> 07) & 0x1F
#define RS1 	(opcode >> 15) & 0x1F
#define RS2 	(opcode >> 20) & 0x1F
#define OP 		(opcode & 0x7F)
#define FUNCT3 	(opcode >> 12) & 0x3
#define FUNCT7 	(opcode >> 25) & 0x7F
#define IMM12	(opcode >> 20) & 0xFFF
#define IMM5	(opcode >> 07) & 0x1F
#define IMM7	(opcode >> 25) & 0x7F
#define IMM20	(opcode >> 12) & 0xFFFFF
// opcode format
#define R_TYPE 0b0110011
#define I_TYPE 0b0010011
#define S_TYPE 0b0100011
#define B_TYPE 0b1100011
#define J_TYPE 0b1101111
// helpers
#define REGS cpu->regs
#define MEM cpu->memory

rv32_t rv32_create(u32* program, u32 len) {
	rv32_t* cpu = (rv32_t*)malloc(sizeof(rv32_t));
	cpu->pc = 0;

	for (int i = 0; i < len; i++) {
		cpu->memory[i] = *(program + i);
	}
}

void rv32_step(rv32_t* cpu) {
	u32 instr = rv32_read(cpu, cpu->pc);

	rv32_exec_instr(cpu, instr);

	// x0 always equals 0
	cpu->regs[0] = 0;
}

void rv32_exec_instr(rv32_t* cpu, u32 opcode) {
	u32 new_pc = 0;

	switch (OP) {
		case R_TYPE: {
			switch (FUNCT3) {
				// ADD / SUB
				case 0x0: {
					switch (FUNCT7) {
						// ADD
						case (0x0):
							REGS[RD] = REGS[RS1] + REGS[RS2];
							break;
						// SUB
						case (0x20):
							REGS[RD] = REGS[RS1] - REGS[RS2];
							break;
					}
					break;
				}
				
				// XOR
				case 0x4:
					REGS[RD] = REGS[RS1] ^ REGS[RS2];
					break;
				// OR
				case 0x6:
					REGS[RD] = REGS[RS1] | REGS[RS2];
					break;
				// AND
				case 0x7:
					REGS[RD] = REGS[RS1] & REGS[RS2];
					break;
				// SLL
				case 0x1:
					REGS[RD] = REGS[RS1] << REGS[RS2];
					break;
				// SRL / SRA
				case 0x5: {
					switch (FUNCT7) {
						// SRL
						case 0x0:
							REGS[RD] = REGS[RS1] >> REGS[RS2];
							break;
						// SRA
						case 0x20:
							REGS[RD] = (i32)REGS[RS1] >> REGS[RS2];
							break;
					}
					break;
				}
				// SLT
				case 0x2:
					REGS[RD] = (REGS[RS1] < REGS[RS2]) ? 1 : 0;
					break;
				// SLTU
				case 0x3:
					REGS[RD] = ((i32)REGS[RS1] < (i32)REGS[RS2]) ? 1 : 0;
					break;
			}
		}
		case I_TYPE: {
			switch (FUNCT3) {
				// ADDI
				case 0x0:
					REGS[RD] = REGS[RS1] + IMM12;
					break;
				// XORI
				case 0x4:
					REGS[RD] = REGS[RS1] ^ IMM12;
					break;
				// ORI
				case 0x6:
					REGS[RD] = REGS[RS1] | IMM12;
				// ANDI
				case 0x7:
					REGS[RD] = REGS[RS1] & IMM12;
				// SLLI
				case 0x1:
					REGS[RD] = REGS[RS1] << (IMM12 & 0xF);
				// SRLI / SRAI
				case 0x5: {
					switch (FUNCT7) {
						// SRLI
						case 0x0:
							REGS[RD] = REGS[RS1] >> (IMM12 & 0xF);
							break;
						// SRAI
						case 0x20:
							REGS[RD] = (i32)REGS[RS1] >> (IMM12 & 0xF);

					}
					break;
				}
				// SLTI
				case 0x2:
					REGS[RD] = (REGS[RS1] < REGS[RS2]) ? 1 : 0;
					break;
				// SLTIU
				case 0x3:
					REGS[RD] = ((u32)REGS[RS1] < (u32)REGS[RS2]) ? 1 : 0;
					break;
			}
		}
		// load instructions use different opcode
		case I_TYPE & 0b1101111: {
			switch (FUNCT3) {
				// LB
				case 0x0:
					REGS[RD] = MEM[REGS[RS1] + IMM12] & 0xFF;
					break;
				// LH
				case 0x1:
					REGS[RD] = MEM[REGS[RS1] + IMM12] & 0xFFFF;
					break;
				// LW
				case 0x2:
					REGS[RD] = MEM[REGS[RS1] + IMM12] & 0xFFFF;
					break;
			}
		}
	}
}

u32 rv32_read(rv32_t* cpu, u32 addr) {
	return cpu->memory[addr & 0x7FFFFFFF];
}

void rv32_write(rv32_t* cpu, u32 addr, u32 write) {
	cpu->memory[addr & 0x7FFFFFFF] = write;
}