#include <algorithm>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <gtest/gtest.h>
#include <vector>
#include "assembler.hpp"

TEST(Assembler, Tokenize) {
    std::vector<std::string>inputValues = 
    {
        "ADD 10 ;asdqwer",
        "MOVE 12 2134r5",
        ";213432423 32452 35123",
        "MOVE 1; 2134r5",
        "MOVE ;; 2134r5",
    };

    std::vector<std::vector<std::string>> expectedValues = 
    {
        {"ADD", "10"},
        {"MOVE", "12", "2134r5"},
        {},
        {"MOVE", "1"},
        {"MOVE"}
    };

    std::vector<std::vector<std::string>> outputValues;

    for(int i = 0; i < inputValues.size(); i++)
    {
        outputValues.push_back(Tokenize(inputValues[i]));
    }
    
    ASSERT_EQ(outputValues, expectedValues);
}

TEST(AssemblerFromFile, Variable_Test_no_erors)
{
    std::string fileName = "../../tests/variable_test.txt";
    std::ifstream file(fileName);
    ASSERT_TRUE(file.is_open()); 

    std::cout << "File didn't opened";
    
    std::vector<uint8_t> data = 
    {
        0b00000011, 0b00010000, 0b00000011, 0b00100000, 0b00000100, 0b00110000,
        0b00010111, 0b00000000, 0b00001010
    };
    
    std::vector<uint8_t> expectedValue(MEM_BUFF * 2, 0);
    std::copy(data.begin(), data.end(), expectedValue.begin());
    
    std::vector<uint8_t> outputValue = AssembleFromFile(fileName);

    EXPECT_EQ(expectedValue, outputValue);
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
