#include <iostream>
#include <print>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <string>
#include <cstring>
#include <print>

using namespace std;

size_t write_simulator(const char *s, size_t len, int &err)
{
    size_t rand_len = rand() % (len+1);
    size_t ret = write(STDOUT_FILENO, s, rand_len);
    err = errno;
    return ret;
}

int main()
{
    const char *s = "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz\n0123456789";
    srand(time(nullptr));
    size_t pos = 0;
    int err = EINTR;
    while (pos < strlen(s))
    {
        ;
        size_t ret = write_simulator(s+pos, strlen(s+pos), err);
        if (ret == string::npos) 
            if (err == EINTR) ret = 0;
            else return 0;
        pos += ret;
    }
    return 0;

}
