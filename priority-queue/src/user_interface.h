#pragma once

#include <cstdio>
#include <cstring>
#include <limits>
#include <memory>
#include <string>
#include <type_traits>

#include "priority_queue.h"

class UserInterface {

public:
    void run() {
        welcome_screen();
        DataFormat format = choose_format();
        switch (format) {
            case DataFormat::INT:
                main_loop<int>();
                break;
            case DataFormat::CHAR:
                main_loop<char>();
                break;
            case DataFormat::STRING:
                main_loop<std::string>();
                break;
            default:
                break;
        }
    }
    
private:
    enum class DataFormat {
        INT = 1,
        CHAR = 2,
        STRING = 3
    };

    enum class UserOperation {
        PUSH = 1,
        POP = 2,
        EXIT = 3
    };

    void welcome_screen() {
        printf("Prezentacja kolejki priorytetowej\n");
        printf("Projekt - Rafal Jankowski\n"); 
        wait_for_enter();
        clear_console();
    }

    DataFormat choose_format() {
        int input = 0;
        while (true) {
            printf("Wybierz format danych dla kolejki:\n");
            printf("1) int    - liczba calkowita\n");
            printf("2) char   - znak ASCII\n");
            printf("3) string - ciag znakow\n");
            printf("Wybor: ");
            if (scanf("%d", &input) != 1) {
                clear_buffer();
                continue;
            }
            clear_buffer();
            if (input >= 1 && input <= 3) {
                clear_console();
                return static_cast<DataFormat>(input);
            }
            printf("Niepoprawny wybor. Sprobuj ponownie.\n");
        }
    }

    UserOperation get_user_operation() {
        printf("\nWybierz operacje do wykonania:\n");
        printf("1) Push\n");
        printf("2) Pop\n");
        printf("3) Wyjdz\n");
        printf("Wybor: ");
        int operation = 0;
        while (true) {
            if (scanf("%d", &operation) != 1) {
                continue;
            }
            clear_buffer();
            if (operation < 1 || operation > 3) {
                printf("Niepoprawny wybor. Sprobuj ponownie.\n");
                continue;
            }
            return static_cast<UserOperation>(operation);
        }
    }

    template <typename T>
    void main_loop() {
        MinPriorityQueue<T> queue;
        while (true) {
            print_queue(queue);
            UserOperation operation = get_user_operation();
            switch (operation) {
                case UserOperation::POP: {
                    auto popped = queue.pop();
                    if (!popped.second) {
                        printf("Kolejka jest pusta.\n");
                        wait_for_enter();
                    } else {
                        printf("Usunieto element (%u, ", popped.first);
                        print_value_overload(*popped.second);
                        printf(").\n");
                        wait_for_enter();
                    }
                    break;
                }
                case UserOperation::PUSH: {
                    T value;
                    printf("Podaj wartosc do dodania: ");
                    if constexpr (std::is_same_v<T, int>) {
                        scanf("%d", &value);
                    } else if constexpr (std::is_same_v<T, char>) {
                        scanf(" %c", &value);
                    } else if constexpr (std::is_same_v<T, std::string>) {
                        char buffer[1024];
                        scanf("%1023s", buffer);
                        value = buffer;
                    }
                    unsigned int priority;
                    printf("Podaj priorytet (>= 0): ");
                    while (scanf("%u", &priority) != 1) {
                        clear_buffer();
                        printf("Niepoprawny priorytet. Sprobuj ponownie: ");
                    }
                    clear_buffer();

                    queue.push(priority, std::make_unique<T>(value));
                    break;
                }
                case UserOperation::EXIT:{
                    return;
                }
            }
            clear_console();
        }
    }

    void print_value_overload(const int& value) {
        printf("%d", value);
    }

    void print_value_overload(const char& value) {
        printf("'%c'", value);
    }

    void print_value_overload(const std::string& value) {
        printf("%s", value.c_str());
    }

    template <typename T>
    void print_queue(MinPriorityQueue<T>& queue) {
        auto view = queue.get_view();
        printf("[");
        for (size_t i = 0; i < view.size(); ++i) {
            if (i) printf(", ");
            printf("(%u, ", view[i].first);
            print_value_overload(*(view[i].second));
            printf(")");
        }
        printf("]\n");
    }

    void clear_console() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }

    void clear_buffer() {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
    }

    void wait_for_enter() {
        printf("Nacisnij Enter, aby kontynuowac...");
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
    }
};
