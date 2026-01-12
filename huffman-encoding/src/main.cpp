#include <cstdint>
#include <cstring>

enum class ERROR: uint8_t {
    INVALID_ARGS_COUNT = 1,
    INVALID_ARGS_FORMAT = 2
};

enum class OPTION {
    COMPRESS,
    DECOMPRESS,
    OUTPUT_NAME,
    UNKNOWN
};

ERROR validate_arguments(int argc, char* argv[]) {
    if (argc < 1) {
        return ERROR::INVALID_ARGS_COUNT;
    }
}

constexpr OPTION get_option_by_index(char* argv[], ssize_t i) {
    if (strcmp(argv[i], "-c") == 0) 
        return OPTION::COMPRESS;
    if (strcmp(argv[i], "-d") == 0) 
        return OPTION::DECOMPRESS;
    if (strcmp(argv[i], "-o") == 0) 
        return OPTION::OUTPUT_NAME;
    return OPTION::UNKNOWN;
}

int main(int argc, char* argv[]) {
    for (size_t i = 0; i < argc; i++) {
        OPTION current_option = get_option_by_index(argv, i);
        switch (current_option){
            case OPTION::COMPRESS: {
                
                break;
            }
            case OPTION::DECOMPRESS: {
                break;
            }
            case OPTION::OUTPUT_NAME: {
                break;
            }
        }
    }
};