#pragma once

#include <string>
#include <regex>

enum class MODE {
    COMPRESS,
    DECOMPRESS
};

struct Arguments{
    MODE mode;
    std::string input_path;
    std::string output_path;
};

class ArgumentParaser {
public:
    static Arguments parse_args(int argc, char** argv);
};