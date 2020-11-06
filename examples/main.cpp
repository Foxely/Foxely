#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include <streambuf>
#include <unistd.h>
#include <termios.h>
#include <sstream>
#include <algorithm>

#include "library/library.h"
#include "foxely.h"


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
    VM oVM;

    std::string input;
    std::string line;
    std::cout << RESET;
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
            oVM.Interpret(NULL, input.c_str());
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
    VM oVM;

    char line[1024] = "";
    std::string input = "";
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

            if (c == 3) // CTRL-C
                break;
            else if (c == 27)
            {
                char next1 = getch();
                char next2 = getch();
                if (next1 == 91) {
                    if (next2 == 68) // Left
                        index = std::max(0, index - 1);
                    else if (next2 == 67) // Right
                    {
                        index = std::min((int)input.size(), index + 1);
                    }
                }
            }
            else if ((int)c == 13)
            {
                printf("\u001b[1000D");
                printf("%s\n", input.c_str());
                input = "";
                index = 0;
            }
            else if (c == 127)
            {
                input = input.substr(0, index - 1) + input.substr(index);
                index -= 1;
            }
            else if (32 <= c <= 126)
            {
                input = input.substr(0, index) + (char)c + input.substr(index);
                index += 1;
            }

            printf("\u001b[1000D");
            printf("\u001b[0K");
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

void completion(const char *buf, linenoiseCompletions *lc)
{
        std::cout << lc->len;
    if (buf[0] == 'f')
    {
        linenoiseAddCompletion(lc, "for");
    }
    else if (buf[0] == 'w')
    {
        linenoiseAddCompletion(lc, "while");
    }
    else if (buf[0] == 'i')
    {
        linenoiseAddCompletion(lc, "if");
    }
}

char *hints(const char *buf, int *color, int *bold)
{
    if (!strcasecmp(buf, "f"))
    {
        *color = 35;
        *bold = 0;
        return "or";
    }

    if (!strcasecmp(buf, "w"))
    {
        *color = 35;
        *bold = 0;
        return "hile";
    }

    if (!strcasecmp(buf, "i"))
    {
        *color = 35;
        *bold = 0;
        return "f";
    }
    return NULL;
}

void replv3(int ac, char** av)
{
    char *line;

    VM oVM;
    oVM.argc = ac;
    oVM.argv = av;

    while(ac > 1) {
        ac--;
        av++;
        if (!strcmp(*av,"--multiline")) {
            linenoiseSetMultiLine(1);
        } else if (!strcmp(*av,"--keycodes")) {
            linenoisePrintKeyCodes();
            exit(0);
        } else {
            fprintf(stderr, "Usage: foxely [--multiline] [--keycodes]\n");
            exit(1);
        }
    }

    /* Set the completion callback. This will be called every time the
     * user uses the <tab> key. */
    linenoiseSetCompletionCallback(completion);
    linenoiseSetHintsCallback(hints);

    /* Load history from file. The history file is just a plain text file
     * where entries are separated by newlines. */
    linenoiseHistoryLoad("history.txt"); /* Load the history at startup */

    /* Now this is the main loop of the typical linenoise-based application.
     * The call to linenoise() will block as long as the user types something
     * and presses enter.
     *
     * The typed string is returned as a malloc() allocated string by
     * linenoise, so the user needs to free() it. */
    
    while((line = linenoise("> ")) != NULL)
    {
        /* Do something with the string. */
        if (line[0] != '\0' && line[0] != '#')
        {
            oVM.Interpret(NULL, line);
            linenoiseHistoryAdd(line); /* Add to the history. */
            linenoiseHistorySave("history.txt"); /* Save the history on disk. */
        }
        else if (!strncmp(line, "#ShowToken", 10))
        {
            std::cout << "Show tokens activated...\n";
        }
        else if (line[0] == '#')
        {
            printf("Unreconized command: %s\n", line);
        }
        free(line);
    }
}

void runFile(int ac, char** av, const char* path)
{
    VM oVM;

    oVM.argc = ac;
    oVM.argv = av;
	std::ifstream t(path);
	std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    t.close();

    oVM.DefineFunction("core", "clock", clockNative);
    DefineIOModule(&oVM);

    InterpretResult result = INTERPRET_OK;
    result = oVM.Interpret("main", str.c_str());

    Callable say = oVM.Function("main", "sayHello()");
    // while(true) {
        PrintValue(say.Call());
    // }

    // Callable add = oVM.Function("main", "add(_,_)");
    // oVM.EnsureSlots(2);
    // oVM.SetSlotInteger(0, 2);
    // oVM.SetSlotInteger(1, 1);
    // add.Call(2);

    if (result == INTERPRET_COMPILE_ERROR) exit(65);
    if (result == INTERPRET_RUNTIME_ERROR) exit(70);
}

int main(int ac, char** av)
{
    if (ac == 1) {
        IsRepl = true;
        repl();
    } else if (ac >= 2) {
        runFile(ac, av, av[1]);
    } else {
        fprintf(stderr, "Usage: foxely [path]\n");
        exit(64);
    }
    return 0;
}
