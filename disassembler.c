#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h> 

int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("Usage: decomp path_to_program\n");
		return 1;
	}

	const char* path = argv[1];
		
	uint32_t buf = 0;

	int fd = open(path, O_RDONLY);

	while (read(fd, (uint8_t*) &buf, 4) != 0) {
		uint8_t  opcode = buf & 0x7F; // first 6 bits

		switch (opcode) {
		case 51: // R-type
			printf("R-type\n");
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
			printf("J-type\n");
			break;	 
		default: // unknown opcode
			printf("Error: unknown opcode %b\n", opcode);
			break;
		}

		//printf("%u\n", opcode);
	}
	
	return 0;
}
