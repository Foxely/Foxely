#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include <streambuf>
#include <unistd.h>
#include <termios.h>
#include <sstream>
#include <algorithm>

#include "chunk.hpp"
#include "debug.h"
#include "Parser.h"
#include "vm.hpp"


char* RED = "\u001b[31m";
char* BLACK = "\u001b[30m";
char* GREEN = "\u001b[32m";
char* YELLOW = "\u001b[33m";
char* BLUE = "\u001b[34m";
char* MAGENTA = "\u001b[35m";
char* CYAN = "\u001b[36m";
char* WHITE = "\u001b[37m";

char* Bright_Black = "\u001b[30;1m";
char* Bright_Red = "\u001b[31;1m";
char* Bright_Green = "\u001b[32;1m";
char* Bright_Yellow = "\u001b[33;1m";
char* Bright_Blue = "\u001b[34;1m";
char* Bright_Magenta = "\u001b[35;1m";
char* Bright_Cyan = "\u001b[36;1m";
char* Bright_White = "\u001b[37;1m";

char* Bold = "\u001b[1m";
char* Underline = "\u001b[4m";
char* Reversed = "\u001b[7m";


char* Up = "\u001b[{n}A";
char* Down = "\u001b[{n}B";
char* Right = "\u001b[{n}C";
char* Left = "\u001b[1D";


char* RESET = "\u001b[0m";


void repl()
{
    GC::Gc.pVm = VM::GetInstance();

    std::string input;
    std::string line;
    while(true)
    {
        input.clear();
        std::cout << "> ";
        std::cout << Left;
        std::cout << RESET;
        std::getline(std::cin, input);

        if (input[input.size() - 1] == '{') {
            std::cout << "| ";
            while (std::getline(std::cin, line))
            {
                if (line == "}") {
                    input += line;
                    break;
                }
                std::cout << "| ";
                input += line;
            }
            std::cout << "\n";
        }

        if (input == "exit")
            break;

        if (input[0] != '\n' && input[0] != '\0')
            VM::GetInstance()->interpret(input.c_str());
        else {
            std::cout << std::endl;
            break;
        }
    }
}

int getch() {
   struct termios oldtc;
   struct termios newtc;
   int ch;
   tcgetattr(STDIN_FILENO, &oldtc);
   newtc = oldtc;
   newtc.c_lflag &= ~(ICANON | ECHO);
   tcsetattr(STDIN_FILENO, TCSANOW, &newtc);
   ch = getchar();
   tcsetattr(STDIN_FILENO, TCSANOW, &oldtc);
   return ch;
}

void replv2()
{
    GC::Gc.pVm = VM::GetInstance();
    char line[1024] = "";
    std::string input;
    std::string word;
    bool loop = true;
    int index = 0;
    // while (loop)
    // {
        // word.clear();
        printf("> ");
        char c;

        system("/bin/stty raw");
        while(1)
        {
            c = getch();

            // if (c == 65)
            // {
            //     printf("\u001b[1D");
            // }
            // printf("%d", c);

            // if ((int)c == 13) {
            //     std::cout << "\b\b\n";
            //     break;
            // }

            if (c == 3) // CTRL-C
                return;
            else if (32 <= c <= 126)
            {
                // input = input.substr(0, index) + (char)c + input.substr(index, 1);
                input += c;
                index += 1;

            }
            else if (10 < c > 13)
            {
                printf("\u001b[1000D");
                printf("\nechoing... %s", input);
                input = "";
                index = 0;
            }

            else if (c == 65)
            {
                index = std::max(0, index - 1);
            }
            else if (c == 67)
            {
                index = std::min((int)input.length(), index + 1);
            }

            printf("\u001b[1000D");
            printf("%s", input.c_str());
            printf("\u001b[1000D");
            if (index > 0)
                printf("\u001b[%dC", index);
            std::cout.flush();
        }
        system("/bin/stty cooked");
        // std::cout << "\n";
        // VM::GetInstance()->interpret(line);
    // }
}

// void replv2()
// {
//     int ch;
//     std::string word;
//     std::string line;
//     int startPos = 0;

//     bool loop = true;
//     while (loop) {
//         std::cout << "> ";
//         line.clear();
//         word.clear();
//         while (true)
//         {
//             ch = getch();
//             startPos = line.size();
//             std::stringstream new_buffer;
//             std::streambuf* old_buffer = std::cout.rdbuf(new_buffer.rdbuf());

//             if (word == "for") {
//                 word = "\033[0;32m" + word + "\033[0m";
//                 // line.erase(startPos, word.size());
//             }

//             if (word == "while") {
//                 word = "\033[0;32m" + word + "\033[0m";
//                 // line.erase(startPos, word.size());
//             }

//             if (word == "var") {
//                 word = "\033[0;33m" + word + "\033[0m";
//                 // line.erase(startPos, word.size());
//             }

//             if (ch == 127)
//             {
//                 if (word.size() > 0)
//                     word.erase(word.size() - 1, 1);
//                 else if (line.size() > 0)
//                     line.erase(line.size() - 1, 1);
//             } else
//                 word += ch;

//             if (ch == ' ')
//             {
//                 line += word;
//                 word.clear();
//             }

//             if (ch == '\n')
//                 line += word;

//             std::cout << "> ";
//             std::cout << line;
//             std::cout << word;
//             system("clear");

//             // Restore original buffer:
//             std::cout.rdbuf(old_buffer);
//             std::cout << new_buffer.str();

//             if(ch == '\n')
//                 break;
//         }
//         if (word == "exit\n")
//             break;
//         VM::GetInstance()->interpret(line.c_str());
//     }
// }

void runFile(const char* path)
{
    GC::Gc.pVm = VM::GetInstance();
	std::ifstream t(path);
	std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    t.close();

    InterpretResult result = VM::GetInstance()->interpret(str.c_str());

    if (result == INTERPRET_COMPILE_ERROR) exit(65);
    if (result == INTERPRET_RUNTIME_ERROR) exit(70);
}

int main(int ac, char** av)
{
    VM::GetInstance()->argc = ac;
    VM::GetInstance()->argv = av;
    if (ac == 1) {
        repl();
    } else if (ac >= 2) {
        runFile(av[1]);
    } else {
        fprintf(stderr, "Usage: foxely [path]\n");
        exit(64);
    }
    return 0;
}
