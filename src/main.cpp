#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <ios>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <exception>
//buffer of DeComp 
#define MEM_BUFF 4096

enum class ArgStatus {
    NO_INPUT_FILE = 0,
    VALID_INPUT = 1,
    INVALID_ARGS = 2  // Adding an extra state in case you need it later
};

//instructions of DeComp
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

// convert 16-bit buffer to 8-bit buffer and bit rotation
std::vector<uint8_t> Cnv16to8ROL(std::vector<uint16_t> array)
{
    std::vector<uint8_t> result;
    
    for (uint16_t value : array) {
        result.push_back(static_cast<uint8_t>(value & 0xFF));  // Low byte
        result.push_back(static_cast<uint8_t>(value >> 8));    // High byte
    }

    return result;
}

std::vector<std::string> Tokenize(std::string line)
{
    std::vector<std::string> result; 
    std::stringstream ss(line);

    std::string token;

    while(ss >> token)
    {
        result.push_back(token);
    }
    
    return result;
}

uint16_t StrToUint16(std::string num)
{
    int number = std::stoi(num.c_str());
    return static_cast<uint16_t>(number);
}

std::vector<uint8_t> AssembleFromFile(std::string filename)
{
    std::vector<uint16_t> data;
    data.resize(MEM_BUFF);
    std::ifstream code(filename, std::ios::in);
    std::string instruction;
    
    static std::vector<std::string> invalidTokens;
    if(invalidTokens.size() != 0)
        invalidTokens.clear();

    int i = 0;
    while(std::getline(code, instruction))
    {
        std::vector<std::string> tokens = Tokenize(instruction);
        if(tokens.empty()) {continue;}

        std::string instruction = tokens[0];
        
        if(opcodeMap.find(instruction) == opcodeMap.end())
        {
            std::string err = "invalid token. Instruction: ";
            err += instruction + ' '; 
            err += "line: ";
            err += std::to_string(i + 1);
            i++;
            invalidTokens.push_back(err);
            continue;
        }
        else
        {
            if(tokens.size() == 2)
            {
                data[i] = opcodeMap[instruction] | StrToUint16(tokens[1]);
            }
            if(tokens.size() == 1)
            {
                data[i] = (opcodeMap[instruction]);
            }
            if(tokens.size() == 3 && tokens[0] == "STORE")
            {
                int index = std::stoi(tokens[1].c_str());
                data[index] = StrToUint16(tokens[2]);
            }

            i++;
        }

    }

    if(invalidTokens.size() != 0)
        throw invalidTokens;

    return Cnv16to8ROL(data);
}

ArgStatus HandleArgs(int argc, char **argv)
{
    if(argc == 1)
    {
        std::cout << "no input file:\n" << argv[0] << " filename";
        return ArgStatus::NO_INPUT_FILE;
    }

    if(argc == 2)
    {
        std::ifstream code(argv[1], std::ios::in);
        
        if(!code.is_open())
        {
            return ArgStatus::INVALID_ARGS;
        }
        return ArgStatus::VALID_INPUT;
    }

    return ArgStatus::INVALID_ARGS;
}

int main(int argc, char **argv)
{
    switch(HandleArgs(argc,argv))
    {
        case ArgStatus::NO_INPUT_FILE:
            std::cout << "no input file.";
            return -2;
            break;

        case ArgStatus::INVALID_ARGS:
        {
            std::cout << "invalid arguments (probably no file or can't open it)." << std::endl;
            return -1;
            break;
        }

        case ArgStatus::VALID_INPUT:
        {
            std::ofstream file("DeComp.mem", std::ios::binary);
            try
            {
                std::vector<uint8_t> buffer8 = AssembleFromFile(argv[1]);
                file.write(reinterpret_cast<char*>(&buffer8[0]), buffer8.size());
            }
            catch(std::vector<std::string> &errors)
            {
                for(std::string err : errors)
                std::cout << "error: " << err << std::endl;;
            }
        }
    }
    return 0;
}
