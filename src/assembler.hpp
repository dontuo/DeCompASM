#pragma once

#include <cstdint>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

#define MEM_BUFF 4096

enum class ArgStatus
{
    NO_INPUT_FILE = 0,
    VALID_INPUT = 1,
    INVALID_ARGS = 2
};

struct Variable
{
    uint16_t address;
    uint16_t value;
};

enum TokenType
{
    INSTRUCTION,
    VARIABLE,
    VALUE,
    LAB
};

struct Token
{
    TokenType type;

    std::string value;
};

extern std::unordered_map<std::string, uint16_t> opcodeMap;
extern std::unordered_map<std::string, uint16_t> additionalMap;

extern std::string patternVariable;
extern std::string patternLab;
extern std::string patternInstruction;

std::vector<uint8_t> Cnv16to8ROL(std::vector<uint16_t> array);
std::vector<Token> Tokenize(std::string line);
uint16_t StrToUint16(std::string num);
std::vector<uint8_t> AssembleFromFile(std::string filename);
ArgStatus HandleArgs(int argc, char **argv);
