#include "arg_paraser.h"

#include <cstring>
#include <string>
#include <stdexcept>
#include <regex>

std::regex INPUT_REGEX{R"(^[A-Za-z0-9._\-\\/:]+$)"};
std::regex OUTPUT_REGEX{R"(^[A-Za-z0-9._\-\\/:]+$)"};

// Sprawdza, czy napis ma wskazany sufiks.
bool ends_with(const std::string& str, const std::string& suffix) {
    if (str.size() < suffix.size()) return false;
    return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

// Sprawdza, czy sciezka konczy sie rozszerzeniem .huff.
bool ends_with_huff(const std::string& path) {
    return ends_with(path, ".huff");
}

// Usuwa rozszerzenie .huff, jesli wystepuje.
std::string remove_huff_extension(const std::string& path) {
    const std::string ext = ".huff";
    if (ends_with(path, ext)) {
        return path.substr(0, path.size() - ext.size());
    }
    return path;
}

// Waliduje argumenty i buduje strukture Arguments.
Arguments ArgumentParaser::parse_args(int argc, char** argv) {
    bool is_mode_selected = false;
    bool is_output_path_selected = false;
    std::string input_path;
    std::string output_path;
    Arguments output_args;

    for (size_t i = 1; i < (size_t)argc; i++) {
        bool is_enough_args = (int)(i + 1) < argc;

        if (std::strcmp(argv[i], "-c") == 0) {
            if (is_mode_selected) {
                throw std::runtime_error("Tylko jedna z opcji (-c, -d) może być aktywna w czasie użycia.");
            }
            if (!is_enough_args) {
                throw std::runtime_error("Podano za mało argumentów. Opcja (-c [ścieżka_do_pliku]) oczekuje ścieżki do pliku wejściowego.");
            }
            bool is_option = argv[i + 1][0] == '-';
            if (is_option) {
                throw std::runtime_error("Oczekiwano argumentu [ścieżka_do_pliku] dla (-c [ścieżka_do_pliku]). Otrzymano kolejną opcję.");
            }
            bool is_match_regex = std::regex_match(argv[i + 1], INPUT_REGEX);
            if (!is_match_regex) {
                throw std::runtime_error("Ścieżka do pliku wejściowego przy (-c [ścieżka_do_pliku]) ma niewłaściwy format. Format musi spełniać wyrażenie [^[A-Za-z0-9._-]+$].");
            }
            is_mode_selected = true;
            output_args.mode = MODE::COMPRESS;
            input_path = std::string(argv[i + 1]);
            i++;
            continue;
        }

        if (std::strcmp(argv[i], "-d") == 0) {
            if (is_mode_selected) {
                throw std::runtime_error("Tylko jedna z opcji (-c, -d) może być aktywna w czasie użycia.");
            }
            if (!is_enough_args) {
                throw std::runtime_error("Podano za mało argumentów. Opcja (-d [ścieżka_do_pliku]) oczekuje ścieżki do pliku wejściowego.");
            }
            bool is_option = argv[i + 1][0] == '-';
            if (is_option) {
                throw std::runtime_error("Oczekiwano argumentu [ścieżka_do_pliku] dla (-d [ścieżka_do_pliku]). Otrzymano kolejną opcję.");
            }
            bool is_match_regex = std::regex_match(argv[i + 1], INPUT_REGEX);
            if (!is_match_regex) {
                throw std::runtime_error("Ścieżka do pliku wejściowego przy (-d [ścieżka_do_pliku]) ma niewłaściwy format. Format musi spełniać wyrażenie [^[A-Za-z0-9._-]+$].");
            }

            std::string candidate = std::string(argv[i + 1]);
            if (!ends_with_huff(candidate)) {
                throw std::runtime_error("Dekomresja (-d) wymaga pliku wejściowego z rozszerzeniem .huff");
            }

            is_mode_selected = true;
            output_args.mode = MODE::DECOMPRESS;
            input_path = candidate;
            i++;
            continue;
        }

        if (std::strcmp(argv[i], "-o") == 0) {
            if (is_output_path_selected) {
                throw std::runtime_error("Nazwę pliku wyjściowego (-o [nazwa_wyjścia]) można wybrać tylko raz.");
            }
            if (!is_enough_args) {
                throw std::runtime_error("Podano za mało argumentów. Opcja (-o [nazwa_pliku]) oczekuje nazwy pliku wyjściowego.");
            }
            bool is_option = argv[i + 1][0] == '-';
            if (is_option) {
                throw std::runtime_error("Oczekiwano argumentu [nazwa_pliku] dla (-o [nazwa_pliku]). Otrzymano kolejną opcję.");
            }
            bool is_match_regex = std::regex_match(argv[i + 1], OUTPUT_REGEX);
            if (!is_match_regex) {
                throw std::runtime_error("Nazwa pliku wyjściowego przy (-o [nazwa_pliku]) ma niewłaściwy format. Format musi spełniać wyrażenie [^[A-Za-z0-9._-]+$].");
            }
            is_output_path_selected = true;
            output_path = std::string(argv[i + 1]);
            i++;
            continue;
        }

        throw std::runtime_error(std::string("Nieznana opcja: ") + argv[i]);
    }

    if (!is_mode_selected) {
        throw std::runtime_error("Nie wybrano trybu pracy. Użyj -c [ścieżka_do_pliku] albo -d [ścieżka_do_pliku].");
    }
    if (input_path.empty()) {
        throw std::runtime_error("Nie podano ścieżki pliku wejściowego.");
    }

    output_args.input_path = input_path;

    if (!is_output_path_selected) {
        if (output_args.mode == MODE::COMPRESS) {
            output_args.output_path = input_path + ".huff";
        } else {
            output_args.output_path = remove_huff_extension(input_path);
        }
    } else {
        output_args.output_path = output_path;
        if (output_args.mode == MODE::COMPRESS && !ends_with_huff(output_args.output_path)) {
            output_args.output_path += ".huff";
        }
    }

    return output_args;
}
