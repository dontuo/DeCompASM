#include "assembler.hpp"
#include <string>

std::unordered_map<std::string, uint16_t> opcodeMap = {
    {"LOAD",  0b0000000000000000},
    {"STORE", 0b0001000000000000},
    {"ADD",   0b0010000000000000},
    {"SUB",   0b0011000000000000},
    {"AND",   0b0100000000000000},
    {"OR",    0b0101000000000000},
    {"XOR",   0b0110000000000000},
    {"NOT",   0b0111000000000000},
    {"INPUT", 0b0111010000000000},
    {"OUTPUT",0b0111100000000000},
    {"HALT",  0b0111110000000000},
    {"JNZ",   0b1000000000000000},
    {"JZ",    0b1001000000000000},
    {"JP",    0b1010000000000000},
    {"JM",    0b1011000000000000},
    {"JNC",   0b1100000000000000},
    {"JC",    0b1101000000000000},
    {"JMP",   0b1110000000000000},
    {"LSL",   0b1111000000000000},
    {"LSR",   0b1111001000000000},
    {"ASL",   0b1111010000000000},
    {"ASR",   0b1111011000000000},
    {"ROL",   0b1111100000000000},
    {"ROR",   0b1111101000000000},
    {"RCL",   0b1111110000000000},
    {"RCR",   0b1111111000000000}
};

std::vector<uint8_t> Cnv16to8ROL(std::vector<uint16_t> array) {
    std::vector<uint8_t> result;
    for (uint16_t value : array) {
        result.push_back(static_cast<uint8_t>(value & 0xFF));
        result.push_back(static_cast<uint8_t>(value >> 8));
    }
    return result;
}

std::vector<std::string> Tokenize(std::string line) {
    std::vector<std::string> result;
    std::stringstream ss(line);
    std::string token;

    while(ss >> token) {
        if(token[0] == ';' or token.find(';') != std::string::npos)
        {
            std::cout << line << std::endl;
            goto skip;
        }
        result.push_back(token);
    }
skip:
    return result;
}

uint16_t StrToUint16(std::string num) {
    int number = std::stoi(num.c_str());
    return static_cast<uint16_t>(number);
}

std::vector<uint8_t> AssembleFromFile(std::string filename) {
    std::vector<uint16_t> data(MEM_BUFF);
    std::ifstream code(filename);
    std::string instruction;
    static std::vector<std::string> invalidTokens;
    invalidTokens.clear();
    int i = 0;
    while (std::getline(code, instruction)) {
        std::vector<std::string> tokens = Tokenize(instruction);
        if (tokens.empty()) continue;
        std::string inst = tokens[0];
        if (opcodeMap.find(inst) == opcodeMap.end()) {
            invalidTokens.push_back("invalid token. Instruction: " + inst + " line: " + std::to_string(i + 1));
            i++;
            continue;
        }
        if (tokens.size() == 2) {
            data[i] = opcodeMap[inst] | StrToUint16(tokens[1]);
        } else if (tokens.size() == 1) {
            data[i] = opcodeMap[inst];
        } else if (tokens.size() == 3 && tokens[0] == "STORE") {
            int index = std::stoi(tokens[1].c_str());
            data[index] = StrToUint16(tokens[2]);
        }
        i++;
    }
    if (!invalidTokens.empty()) throw invalidTokens;
    return Cnv16to8ROL(data);
}

ArgStatus HandleArgs(int argc, char **argv) {
    if (argc == 1) {
        std::cout << "no input file.\n" << argv[0] << " filename";
        return ArgStatus::NO_INPUT_FILE;
    }
    if (argc == 2) {
        std::ifstream code(argv[1]);
        if (!code.is_open()) return ArgStatus::INVALID_ARGS;
        return ArgStatus::VALID_INPUT;
    }
    return ArgStatus::INVALID_ARGS;
}
