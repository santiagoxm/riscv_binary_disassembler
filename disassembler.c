#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h> 

char* regs[] = {"zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};
char* rnames[] = {"add\0sub", "sll", "slt", "sltu", "xor", "srl\0sra", "or", "and"};

int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("Usage: disa path_to_program\n");
		return 1;
	}

	char* path = argv[1];
		
	uint32_t ins = 0;

	int fd = open(path, O_RDONLY);

	int address = 0;

	uint8_t opcode;
	uint8_t rd;
	uint8_t funct3;
	uint8_t rs1;
	uint8_t rs2;
	uint8_t funct7;
	int32_t immj;
	char* opname;

	while (read(fd, (uint8_t*) &ins, 4) != 0) {
		opcode = ins & 0x7F; // bits 0 to 6
		rd = (ins & 0xF80) >> 7; // bits 7 to 11

		switch (opcode) {
		case 51: // R-type
			funct3 = (ins & 0x7000) >> 12; // bits 12 to 14
			rs1 = (ins & 0xf8000) >> 15; // bits 15 to 19
			rs2 = (ins & 0x1f00000) >> 20; // bits 20 to 24
			funct7 = (ins & 0xFC000000) >> 25; // bits 25 to 31

			if (funct3 > 7) {
				printf("0x%X: unknown funct3 %b\n", address, funct3);
			}

			if (funct7 != 0 && funct7 != 32) {
				printf("0x%X: unknown funct7 %b\n", address, funct7);
				break;
			}

			opname = rnames[funct3];

			if (funct7 == 32) {opname += 4;}

			printf("0x%X: %s, %s, %s, %s\n", address, opname, regs[rd], regs[rs1], regs[rs2]);

			break;
		case 3: // I-type
		case 19:
		case 103:
		case 115:
			printf("I-type\n");
			break;
		case 35: // S-type
			printf("S-type\n");
			break;
		case 99: // B-type
			printf("B-type\n");
			break;
		case 23: // U-type
		case 55:
			printf("U-type\n");
			break;
		case 111: // J-type
			immj = (int32_t)(ins & 0x80000000) >> 11; // bit 31 (sign extended)
			immj += (ins & 0x7FE00000)  >> 20; // bits 30 to 21
			immj += (ins & 0x100000) >> 9; // bit 20
			immj += (ins & 0xff000); // bits 19 to 12
			printf("0x%X: jal, %s, %d\n", address, regs[rd], immj);	
			// since it's 5 bits, rd will never be out of the array's bounds
			break;	 
		default: // unknown opcode
			printf("0x%X: unknown opcode %b\n", address, opcode);
			break;
		}

		address += 4;
	}
	
	return 0;
}
