// print all arguments
// $  g++ module-test/argument_printer.cc -std=c++23 -o module-test/argument_printer.out

#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
    for (int i = 0; argv[i] != nullptr; i++)
        cout << argv[i] << endl;
    return 0;
}
