#include <stdio.h>
#include <ctype.h>

int hexChar2Int(char hexChar) {
	if (hexChar >= '0' && hexChar <= '9') {
		return hexChar - '0'; // Convert '0'-'9' to 0-9
	}
	else if (hexChar >= 'a' && hexChar <= 'f') {
		return hexChar - 'a' + 10; // Convert 'a'-'f' to 10-15
	}
	else if (hexChar >= 'A' && hexChar <= 'F') {
		return hexChar - 'A' + 10; // Convert 'A'-'F' to 10-15
	}
	else {
		return -1; // Indicate an invalid hex character
	}
}

char intToHexChar(long value) {
	if (value >= 0 && value <= 9) {
		return (char)('0' + value);
	}
	else if (value >= 10 && value <= 15) {
		return (char)('A' + (value - 10)); // For uppercase A-F
										   // return (char)('a' + (value - 10)); // For lowercase a-f
	}
	else {
		// Handle error or invalid input
		printf("Invalid data to intToHexChar...\n");
		return '\0';
	}
}