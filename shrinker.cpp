// This program packs padded 4-bit data. I used it in an FPGA project.

#include <array>
#include <bitset>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

int main(int argc, char **argv) {
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <in> <out>\n";
		return 1;
	}

	std::ifstream ifstream(argv[1]);
	std::stringstream sstream;
	sstream << ifstream.rdbuf();

	std::ofstream ofstream(argv[2]);

	uint8_t temp = 0;
	bool first = true;

	for (const uint8_t byte: sstream.str()) {
		if (first) {
			temp = (byte & 0xf) << 4;
			first = false;
		} else {
			ofstream << static_cast<char>(temp | (byte & 0xf));
			first = true;
		}
	}
}
