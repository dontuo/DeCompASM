#include "assembler.hpp"

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
