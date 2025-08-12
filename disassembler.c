#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h> 

char* regs[] = {"zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};
char* rnames[] = {"add\0sub\0mul", "sll", "slt", "sltu", "xor\0div", "srl\0sra", "or\0\0rem", "and\0remu"};
char* inames1[] = {"lb", "lh", "lw", "", "lbu","lhu"};
char* inames2[] = {"addi", "slli", "slti", "sltiu", "xori", "srli\0srai", "ori", "andi"};
char* snames[] = {"sb", "sh", "sw"};

int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("Usage: disa path_to_program\n");
		return 1;
	}

	char* path = argv[1];
		
	uint32_t ins = 0;

	int fd = open(path, O_RDONLY);

	if (fd == -1) {
		printf("Error opening file\n");
		return 1;
	}

	int address = 0;

	uint8_t opcode;
	uint8_t rd;
	uint8_t funct3;
	uint8_t rs1;
	uint8_t rs2;
	uint8_t funct7;
	int32_t immj;
	int16_t immi;
	char* opname;
	uint32_t immu;
	int16_t imms;

	while (read(fd, (uint8_t*) &ins, 4) != 0) {
		opcode = ins & 0x7F; // bits 0 to 6
		rd = (ins & 0xF80) >> 7; // bits 7 to 11
		funct3 = (ins & 0x7000) >> 12; // bits 12 to 14
		rs1 = (ins & 0xf8000) >> 15; // bits 15 to 19
		rs2 = (ins & 0x1f00000) >> 20; // bits 20 to 24
		funct7 = (ins & 0xFE000000) >> 25; // bits 25 to 31
		immi = (int32_t)(ins & 0xFFF00000) >> 20; // bits 31 to 20
		immu = (ins & 0xFFFFF000) >> 12;

		switch (opcode) {
		case 51: // R-type
			if ((funct7 != 0 && funct7 != 32 && funct7 != 1) || (funct7 == 32 && (funct3 != 0 && funct3 != 5)) || (funct7 == 1 && (funct3 != 0 && funct3 != 4 && funct3 != 6 && funct3 != 7))) {
				printf("0x%X: unknown R-type instruction %x\n", address, ins);
				break;
			}

			opname = rnames[funct3];

			if (funct7 == 32) {opname += 4;}
			
			if (funct7 == 1) {
				if (funct3 == 0) {opname += 8;}
				else {opname += 4;}
			}

			printf("0x%X: %s %s, %s, %s\n", address, opname, regs[rd], regs[rs1], regs[rs2]);

			break;
		case 3: // I-type 1
			if (funct3 > 5 || funct3 == 3) {
				printf("0x%X: unknown I-type instruction %x\n", address, ins);
				break;
			}
		
			opname = inames1[funct3];

			printf("0x%X: %s %s, %d(%s)\n", address, opname, regs[rd], immi, regs[rs1]);
			break;
		case 19: // I-type 2
			if ((funct3 == 1 && funct7 != 0) || (funct3 == 5 && (funct7 != 0 && funct7 != 32))) {
				printf("0x%X: unknown I-type instruction %x\n", address, ins);
				break;
			}

			if (funct3 == 1 || funct3 == 5) {immi = immi & 0x001F;}
			
			opname = inames2[funct3];

			if (funct7 == 32) {opname += 5;}

			printf("0x%X: %s %s, %s, %d\n", address, opname, regs[rd], regs[rs1], immi);
			break;
		case 103: // I-type jalr
			if (funct3 == 0) {opname = "jalr";}
			else{
				printf("0x%X: unknown I-type instruction %x\n", address, ins);
				break;
			}
			
			printf("0x%X: %s %s, %s, %d\n", address, opname, regs[rd], regs[rs1], immi);
			break;
		case 115:
			if (funct3 == 0 && immi == 0) {opname = "ecall";}
			else if (funct3 == 0 && immi == 1) {opname = "ebreak";}
			else{
				printf("0x%X: unknown I-type instruction %x\n", address, ins);
				break;
			}
			
			printf("0x%X: %s\n", address, opname);
			break;
		case 35: // S-type
			if (funct3 > 2){
				printf("0x%X: unknown S-type instruction %x\n", address, ins);
				break;
			}

			opname = snames[funct3];

			imms = (int32_t)(ins & 0xFE000000) >> 20; // bits 25 to 31
			imms += rd;

			printf("0x%X: %s %s, %d(%s)\n", address, opname, regs[rs2], imms, regs[rs1]);
			
			break;
		case 99: // B-type
			printf("B-type\n");
			break;
		case 23: // U-type auipc
			printf("0x%X: auipc %s, 0x%x\n", address, regs[rd], immu);
			break;
		case 55: // U-type lui
			printf("0x%X: lui %s, 0x%x\n", address, regs[rd], immu);
			break;
		case 111: // J-type jal
			immj = (int32_t)(ins & 0x80000000) >> 11; // bit 31 (sign extended)
			immj += (ins & 0x7FE00000)  >> 20; // bits 30 to 21
			immj += (ins & 0x100000) >> 9; // bit 20
			immj += (ins & 0xff000); // bits 19 to 12
			printf("0x%X: jal %s, %d\n", address, regs[rd], immj);
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
