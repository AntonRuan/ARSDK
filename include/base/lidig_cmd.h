#include <map>
#include <unistd.h>
#include <iostream>

using namespace std;

void print_help() {
    cout << "Usage:" << endl;
    cout << "  -h   Print help message" << endl;
    cout << "  -i   Print info message" << endl;
    cout << "  -d   Print debug message" << endl;
    cout << "  -t   Print trace message" << endl;
    cout << "  -s   Input the string" << endl;
    cout << "  -c   Path to the configuration file" << endl;
}

map<char, string> lidig_cmd(int argc, char const *argv[]) {
    map<char, string> cmd;
    int option;
    while ((option = getopt(argc, (char **)argv, "hidts:c:")) != -1) {
        switch (option) {
        case 'h':
            print_help();
            exit(0);
        case 'i':
            cmd['i'] = "info";
            break;
        case 'd':
            cmd['d'] = "debug";
            break;
        case 't':
            cmd['t'] = "trace";
            break;
        case 'c':
            cmd['c'] = optarg;
            break;
        case 's':
            cmd['s'] = optarg;
            break;
        case '?':
            print_help();
            exit(0);
        }
    }
    return cmd;
}