#include "types.h"

typedef struct {
	u32 pc;
	u32 regs[32];
	u32 memory[0x7FFFFFFF];
} rv32_t;

rv32_t rv32_create(u32*, u32 len);
void rv32_step(rv32_t*);
void rv32_exec_instr(rv32_t*, u32 opcode);
u32 rv32_read(rv32_t*, u32 addr);
void rv32_write(rv32_t*, u32 addr, u32 write);