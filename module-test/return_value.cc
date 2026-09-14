/*
    return specific value
    $ g++ module-test/return_value.cc -std=c++23
*/

#include <iostream>
#include <print>
#include <string>

using namespace std;

int main(int argc, char **argv)
{
    if (argv == nullptr) return 0;
    if (argv[1] == nullptr) return 0;
    string s = argv[1];
    return stoi(s);
}
