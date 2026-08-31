#include "util.h"

namespace ov4
{

using namespace std;

void atomic_print(char *s, bool ignore_wrap)
{
    if (ignore_wrap)
        for (int i = 0; s[i] != 0; i++)
            if (s[i] == '\n')
            {
                write(STDOUT_FILENO, s, i);
                return;
            }
    write(STDOUT_FILENO, s, strlen(s));
}

void atomic_print(int x)
{
    char msg[LENGTH];
    itoa(x, msg, DECIMAL);
    atomic_print(msg);
}

void reverse(char str[], int length)
{
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        end--;
        start++;
    }
}

void atomic_debug(char *s, bool ignore_wrap)
{
    if (verbose && GLOBAL_DEBUG) atomic_print(s, ignore_wrap);
}

void atomic_debug(int x)
{
    if (verbose && GLOBAL_DEBUG) atomic_print(x);
}

bool isnum(char *s)
{
    for (int i = 0; s[i] != 0; i++)
        if (!isdigit(s[i]))
            return false;
    return true;
}


char *itoa(int num, char* str, int base)
{
    int i = 0;
    bool isNegative = false;

    /* Handle 0 explicitly, otherwise empty string is
     * printed for 0 */
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // In standard itoa(), negative numbers are handled
    // only with base 10. Otherwise numbers are
    // considered unsigned.
    if (num < 0 && base == 10) {
        isNegative = true;
        num = -num;
    }

    // Process individual digits
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';

    str[i] = '\0'; // Append string terminator

    // Reverse the string
    reverse(str, i);

    return str;
}

/* 
 * builtin_cmd - If the user has typed a built-in command then execute
 *    it immediately.  
 */
int builtin_cmd(char *c_str) 
{
    string s(c_str);
    for (auto c : builtin_cmd_list)
        if (c == s)
            return 1;
    return 0;     /* not a builtin command */
}

void builtin_command_quit()
{
    exit(0);
}


}
