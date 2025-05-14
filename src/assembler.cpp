#include "assembler.hpp"
#include <bitset>
#include <cctype>
// instructions of DeComp
std::unordered_map<std::string, uint16_t> opcodeMap = {
    {"LOAD", 0b0000000000000000}, 
    {"STORE", 0b0001000000000000}, 
    {"ADD", 0b0010000000000000}, 
    {"SUB", 0b0011000000000000}, 
    {"AND", 0b0100000000000000}, 
    {"OR", 0b0101000000000000}, 
    {"XOR", 0b0110000000000000}, 
    {"NOT", 0b0111000000000000}, 
    {"INPUT", 0b0111010000000000}, 
    {"OUTPUT", 0b0111100000000000}, 
    {"HALT", 0b0111110000000000}, 
    {"JNZ", 0b1000000000000000}, 
    {"JZ", 0b1001000000000000}, 
    {"JP", 0b1010000000000000}, 
    {"JM", 0b1011000000000000}, 
    {"JNC", 0b1100000000000000}, 
    {"JC", 0b1101000000000000}, 
    {"JMP", 0b1110000000000000}, 
    {"LSL", 0b1111000000000000}, 
    {"LSR", 0b1111001000000000}, 
    {"ASL", 0b1111010000000000}, 
    {"ASR", 0b1111011000000000}, 
    {"ROL", 0b1111100000000000}, 
    {"ROR", 0b1111101000000000}, 
    {"RCL", 0b1111110000000000}, 
    {"RCR", 0b1111111000000000}
};

// additional instrictions so you could easily write code
std::unordered_map<std::string, uint16_t> additionalMap = {
    {"Lab", 0xFF},
    {"dvar", 0xFF},
};

std::string patternVariable(R"(\s*dvar\s+([a-zA-Z][a-zA-Z0-9_]*)\s*=\s*(\d+)\s*(?:;.*)?)");
std::string patternLab(R"(\s*Lab\s+([a-zA-Z][a-zA-Z0-9_]*)\s*:\s*(?:;.*)?)");
std::string patternInstruction(R"(\s*([A-Z]+)(?:\s+([a-zA-Z_][a-zA-Z0-9_]*|\d+))?\s*(?:;.*)?)");
std::string patternEmptyLine(R"(\s*(?:;.*)?)");

std::vector<uint8_t> Cnv16to8ROL(std::vector<uint16_t> array)
{
    std::vector<uint8_t> result;
    for (uint16_t value : array)
    {
        result.push_back(static_cast<uint8_t>(value & 0xFF));
        result.push_back(static_cast<uint8_t>(value >> 8));
    }
    return result;
}

std::vector<Token> Tokenize(std::string line)
{
    std::smatch match;
    std::vector<Token> result;

    if (std::regex_match(line, match, std::regex(patternVariable)))
    {
        std::string name = match.str(1);
        std::string value = match.str(2);

        result.push_back(Token{TokenType::VARIABLE, name});
        result.push_back(Token{TokenType::VALUE, value});
    }
    else if (std::regex_match(line, match, std::regex(patternLab)))
    {
        std::string name = match.str(1);

        result.push_back(Token{TokenType::LAB, name});
    }
    else if (std::regex_match(line, match, std::regex(patternInstruction)))
    {
        std::string instruction = match.str(1);
        result.push_back(Token{TokenType::INSTRUCTION, instruction});

        if (match.size() == 3 && !match.str(2).empty())
        {
            std::string value = match.str(2);
            result.push_back(Token{TokenType::VALUE, value});
        }
    }
    else
    {
        // well, not really good, but I have function that check is there are any syntax errors in code
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
        std::vector<Token> tokens = Tokenize(line);

        if (tokens.empty())
            continue;

        if (tokens[0].type == TokenType::INSTRUCTION)
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

    int instructionIndex = 0;
    while (std::getline(file, line))
    {
        std::vector<Token> tokens = Tokenize(line);

        if (tokens.empty())
        {
            continue;
        }

        if (tokens[0].type != TokenType::VARIABLE)
        {
            continue;
        }

        std::string variableName = tokens[0].value;
        std::string value = tokens[1].value;

        variables.insert({variableName, Variable{(uint16_t)(instructionCount + instructionIndex), StrToUint16(value)}});
        instructionIndex++;
    }

    return variables;
}

std::vector<std::string> CheckFileForSyntaxErrors(std::string filename)
{
    std::vector<std::string> invalidTokens;

    std::ifstream file(filename);
    std::string line;

    std::string patternCombined = "(" + patternLab + ")|(" + patternVariable + ")|(" + patternInstruction + ")|(" + patternEmptyLine + ")";

    for (int i = 0; std::getline(file, line); i++)
    {
        if (!std::regex_match(line, std::regex(patternCombined)))
        {
            invalidTokens.push_back("-------------------------------------------\ninvalid syntax. line number: " + std::to_string(i + 1) + "\nline: \n" + line + "\n-------------------------------------------");
        }
    }
    return invalidTokens;
}

std::unordered_map<std::string, uint16_t> GetLablesMap(std::string filename)
{
    std::unordered_map<std::string, uint16_t> lables;

    std::ifstream file(filename);

    std::string line;

    int instructionIndex = 0;

    while (std::getline(file, line))
    {
        std::vector<Token> tokens = Tokenize(line);

        if (tokens.empty())
        {
            continue;
        }

        if (tokens[0].type == TokenType::INSTRUCTION)
        {
            instructionIndex++;
            continue;
        }

        if (tokens[0].type == TokenType::VARIABLE)
        {
            continue;
        }

        std::string lableName = tokens[0].value;

        lables.insert({lableName, uint16_t(instructionIndex)});
    }

    return lables;
}
std::vector<std::string>CheckFileForNameConflicts(std::string filename)
{
    std::vector<std::string> result;
    std::unordered_map<std::string, Variable> variables;
    std::unordered_map<std::string, uint16_t> labels;
    
    variables = GetVariablesMap(filename, 0);
    labels = GetLablesMap(filename);

    for(auto &a: variables)
    {
        if(labels.contains(a.first))
        {
            result.push_back("-------------------------------------------\nName conflict – '" + a.first + "' is used as both a label and a variable." + "\n-------------------------------------------");
        }
    }

    std::ifstream file(filename);
    std::string line;

    for (int i = 0; (std::getline(file, line));i++)
    {
        std::vector<Token> tokens = Tokenize(line);

        if(tokens.empty())
            continue;

        if(tokens[0].type == TokenType::INSTRUCTION && tokens.size() == 2)
        {
            if(!std::isdigit(tokens[1].value[0]))
            {
                if(!(variables.contains(tokens[1].value) or labels.contains(tokens[1].value)))
                {
                    result.push_back("-------------------------------------------\nUnknown name: '" + tokens[1].value + "'. \nLine: " + std::to_string(i + 1) + "\n-------------------------------------------");
                }
            }
        }
    }
    

    return result;
}

std::vector<uint8_t> AssembleFromFile(std::string filename)
{
    std::vector<std::string> invalidTokens = CheckFileForSyntaxErrors(filename);
    
    std::vector<std::string> conflicts = CheckFileForNameConflicts(filename);

    if(!conflicts.empty())
    {
        invalidTokens.insert(invalidTokens.end(), conflicts.begin(), conflicts.end());
    }
    if (!invalidTokens.empty())
        throw invalidTokens;

    int instructionCount = GetInstructionCount(filename);
    std::unordered_map<std::string, Variable> variables;
    std::unordered_map<std::string, uint16_t> labels;

    variables = GetVariablesMap(filename, instructionCount);
    labels = GetLablesMap(filename);

    std::vector<uint16_t> data(MEM_BUFF);

    std::ifstream file(filename);
    std::string line;

    for (int i = 0; (std::getline(file, line));)
    {
        std::vector<Token> tokens = Tokenize(line);

        if (tokens.empty())
        {
            continue;
        }

        if (tokens[0].type == TokenType::INSTRUCTION)
        {
            std::string instruction = tokens[0].value;
            data[i] = opcodeMap[instruction];

            if (tokens.size() == 2)
            {
                std::string value = tokens[1].value;

                if(isalnum(value[0]))
                {
                    data[i] |= StrToUint16(value);
                }
                if (variables.contains(value))
                {
                    data[i] |= variables[value].address;
                }
                else if (labels.contains(value))
                {
                    data[i] |= labels[value];
                }
            }
            i++;
        }
        else if (tokens[0].type == TokenType::VARIABLE)
        {
            uint16_t variableAdress = variables[tokens[0].value].address;
            uint16_t value = variables[tokens[0].value].value;

            data[variableAdress] = value;
        }
    }

    std::cout << "final machine code: \n";
    for(int i = 0; i < instructionCount + variables.size(); i++)
    {
        std::bitset<16> instr(data[i]);
        std::cout << i << "  " << instr << '\n';
    }
    
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
    case 2: {
        std::ifstream code(argv[1]);
        if (!code.is_open())
            return ArgStatus::INVALID_ARGS;
        return ArgStatus::VALID_INPUT;
    }
    break;
    default:
        return ArgStatus::INVALID_ARGS;
        break;
    }
}
