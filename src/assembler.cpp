#include "assembler.hpp"
#include <cctype>
#include <csignal>
#include <cstdint>
#include <map>
#include <string>
#include <sys/types.h>
#include <vector>

// instructions of DeComp
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

// additional instrictions so you could easily write code
std::unordered_map<std::string, uint16_t> additionalMap =
{
    {"Lab", 0xFF},
    {"dvar", 0xFF},
};

std::vector<uint8_t> Cnv16to8ROL(std::vector<uint16_t> array) {
    std::vector<uint8_t> result;
    for (uint16_t value : array) {
        result.push_back(static_cast<uint8_t>(value & 0xFF));
        result.push_back(static_cast<uint8_t>(value >> 8));
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
        auto sign = token.find(';');

        if(token[0] == ';')
        {
            break;
        }
        if(sign != std::string::npos)
        {
            result.push_back(token.substr(0, sign));
            break;
        }
        result.push_back(token);
    }

    return result;
}

uint16_t StrToUint16(std::string num) 
{
    int number = std::stoi(num.c_str());
    return static_cast<uint16_t>(number);
}

int GetInstructionCount(std::string filename)
{
    std::ifstream file(filename);

    std::string line;

    int instructionCount = 0;
    
    while (std::getline(file, line)) 
    {
        std::vector<std::string> tokens = Tokenize(line);
        if(tokens.empty())
            continue;

        std::string instruction = tokens[0];

        if (opcodeMap.find(instruction) != opcodeMap.end() && additionalMap.find(instruction) == additionalMap.end()) 
        {
            instructionCount++;
        }
    }

    return instructionCount;
}

std::unordered_map<std::string, Variable> GetVariablesMap(std::string filename, int instructionCount)
{
    std::unordered_map<std::string, Variable> variables;
    std::ifstream file(filename);
    std::string line;

    //adding variables name, values and addresses 
    int instructionIndex = 0;
    while (std::getline(file, line))
    {
        std::vector<std::string> tokens = Tokenize(line);

        if(tokens.empty())
        {
            continue;
        }
        
        if(tokens.size() != 4)
        {
            continue;
        }

        if(tokens[0] != "dvar")
        {
            continue;
        }

        std::string variableName = tokens[1];
        std::string equal_sign = tokens[2];
        std::string value = tokens[3];
         
        if(variableName != "dvar" && equal_sign != "=")
        {
            instructionIndex++;
            continue;
        }
        variables.insert({variableName, Variable{(uint16_t)(instructionCount + instructionIndex), (uint16_t)std::stoi(value.c_str())}});
        instructionIndex++;
    }

    return variables;
}

std::unordered_map<std::string, uint16_t> GetLablesMap(std::string filename)
{
     std::unordered_map<std::string, uint16_t> lables;

    std::ifstream file(filename);

    std::string line;
    //adding variables name, values and addresses 
    //for(int i = 0; std::getline(file, line); i++) 
    int instructionIndex = 0;

    while (std::getline(file, line))
    {
        std::vector<std::string> tokens = Tokenize(line);

        if(tokens.empty())
        {
            continue;
        }

        if(additionalMap.find(tokens[0]) != additionalMap.end() && tokens[0] != "Lab")
        {
            continue;
        }
        if(opcodeMap.find(tokens[0]) != additionalMap.end())
        {
            instructionIndex++;
            continue;
        }

        if(tokens.size() != 2)
        {
            instructionIndex++;
            continue;
        }

        auto sign = tokens[1].find(':');
        std::string lableName = tokens[1].substr(0, sign);
         
        lables.insert({lableName, uint16_t(instructionIndex)});
    }

    return lables;
}

std::vector<std::string> ParseAsmFileForErrors(std::string filename)
{
    std::ifstream file(filename);
    std::string line;

    std::vector<std::string> invalidTokens;

    for(int lineNumber = 0; std::getline(file, line); lineNumber++)
    {
        std::vector<std::string> tokens = Tokenize(line);

        if(tokens.empty())
            continue;

        std::string instruction = tokens[0];

        // checks is instruction in opcode map or additional map
        if (opcodeMap.find(instruction) == opcodeMap.end() && additionalMap.find(instruction) == additionalMap.end()) 
        {
            invalidTokens.push_back("invalid token. Instruction: " + instruction + " line: " + std::to_string(lineNumber + 1));
        }
    }

    return invalidTokens;
}

std::vector<uint8_t> AssembleFromFile(std::string filename) 
{
    std::vector<std::string> invalidTokens;

    int instructionCount = GetInstructionCount(filename);
    std::unordered_map<std::string, Variable> variables;
    std::unordered_map<std::string, uint16_t> labels;

    variables = GetVariablesMap(filename, instructionCount);
        labels = GetLablesMap(filename);
    /*try
    {
    }
    catch (std::vector<std::string> errors)
    {
        invalidTokens.emplace_back(errors); 
    }

    try
    {
    }
    catch(std::vector<std::string> errors)
    {
        invalidTokens.emplace_back(errors); 
    }*/

    std::vector<uint16_t> data(MEM_BUFF);
    
    std::ifstream file(filename);
    std::string line;

    for(int i = 0; (std::getline(file, line)); i++)
    {
        std::vector<std::string> tokens = Tokenize(line);

        if (tokens.empty()) 
        {
            i--;
            continue;
        }

        std::string instruction = tokens[0];
    
        if (opcodeMap.find(instruction) == opcodeMap.end() && additionalMap.find(instruction) == additionalMap.end()) 
        {
            invalidTokens.push_back("invalid token. Instruction: " + instruction + " line: " + std::to_string(i + 1));
            continue;
        }

        if (tokens.size() == 1) 
        {
            data[i] = opcodeMap[instruction];
        }
        else if (tokens.size() == 2) 
        {
            //this if for variables
            if(variables.find(tokens[1]) != variables.end())
                data[i] = opcodeMap[instruction] | (variables[tokens[1]].address);
            //this if for labs
            else if (labels.find(tokens[1]) != labels.end())
                data[i] = opcodeMap[instruction] | (labels[tokens[1]]);
            else if(instruction != "Lab")
                data[i] = opcodeMap[instruction] | StrToUint16(tokens[1]);
            else
                i--; 
        }
        else if (tokens.size() == 4 && instruction == "dvar") 
        {
            std::string variableName = tokens[1];
            int index = variables[variableName].address;
            data[index] = variables[variableName].value;
            i--;
        }
    }

    if (!invalidTokens.empty()) throw invalidTokens;

    return Cnv16to8ROL(data);
}

ArgStatus HandleArgs(int argc, char **argv) 
{
    switch (argc) 
    {
        case 1: 
            std::cout << "no input file.\n" << argv[0] << " filename";
            return ArgStatus::NO_INPUT_FILE;
        break;
        case 2:
        {
            std::ifstream code(argv[1]);
            if (!code.is_open()) return ArgStatus::INVALID_ARGS;
            return ArgStatus::VALID_INPUT;
        }
        break;
        default:
            return ArgStatus::INVALID_ARGS;
        break;
    }
}
