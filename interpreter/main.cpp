#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include <streambuf>
#include <unistd.h>
#include <termios.h>
#include <sstream>

#include "chunk.hpp"
#include "debug.h"
#include "Parser.h"
#include "vm.hpp"

void repl()
{
    GC::Gc.pVm = VM::GetInstance();

    std::string input;
    std::string line;
    while(true)
    {
        input.clear();
        std::cout << "> ";
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

// void replv2()
// {
//     GC::Gc.pVm = VM::GetInstance();
//     char line[1024] = "";
//     std::string word;
//     bool loop = true;
//     while (loop)
//     {
//         word.clear();
//         printf("> ");
//         char c;


//         system("/bin/stty raw");
//         while(1)
//         {
//             c = getchar();
//             word += c;
//             if(word == "exit") {
//                 system("stty cooked");
//                 loop = false;
//                 break;
//             }

//             if(word == "for")
//             {
//                 std::string color = "\033[0;31m" + word + "\033[0m";
//                 word = color;
//             }

//             if(c == ' ')
//                 word.clear();

//             if((int)c == 13) {
//                 std::cout << "\b\b";
//                 break;
//             }
//         }
//         system("/bin/stty cooked");
//         std::cout << "\n";
//         VM::GetInstance()->interpret(line);
//     }
// }

// int getch() {
//    struct termios oldtc;
//    struct termios newtc;
//    int ch;
//    tcgetattr(STDIN_FILENO, &oldtc);
//    newtc = oldtc;
//    newtc.c_lflag &= ~(ICANON | ECHO);
//    tcsetattr(STDIN_FILENO, TCSANOW, &newtc);
//    ch = getchar();
//    tcsetattr(STDIN_FILENO, TCSANOW, &oldtc);
//    return ch;
// }

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
