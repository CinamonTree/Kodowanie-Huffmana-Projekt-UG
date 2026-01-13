#include <cstdint>
#include <cstring>
#include "arg_paraser.h"
#include <windows.h>

int main(int argc, char* argv[]) {
    SetConsoleOutputCP(CP_UTF8);
    auto args = ArgumentParaser::parse_args(argc, argv);
    return 0;
};