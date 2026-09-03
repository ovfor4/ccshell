#include "util/io.h"

using namespace std;

namespace ov4
{

void safe_print(const char *s, bool ignore_wrap)
{
    int errno_old = errno;
    if (ignore_wrap)
        for (int i = 0; s[i] != 0; i++)
            if (s[i] == '\n')
            {
                write(STDOUT_FILENO, s, i);
                return;
            }
    write(STDOUT_FILENO, s, strlen(s));
    errno = errno_old;
}

void safe_print(int x)
{
    int errno_old = errno;
    char msg[NUMBER_LENGTH];
    itoa(x, msg, DECIMAL);
    safe_print(msg);
    errno = errno_old;
}

void reverse(char *str, int length)
{
    int errno_old = errno;
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        end--;
        start++;
    }
    errno = errno_old;
}

void safe_debug(const char *s, bool ignore_wrap)
{
    int errno_old = errno;
    if (verbose && GLOBAL_DEBUG) safe_print(s, ignore_wrap);
    errno = errno_old;
}

void safe_debug(int x)
{
    int errno_old = errno;
    if (verbose && GLOBAL_DEBUG) safe_print(x);
    errno = errno_old;
}

bool isnum(const char *s)
{
    int errno_old = errno;
    for (int i = 0; s[i] != 0; i++)
        if (!isdigit(s[i]))
            return false;
    return true;
    errno = errno_old;
}


char *itoa(int num, char* str, int base)
{
    int errno_old = errno;
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

    errno = errno_old;
    return str;
}

void wrap_eliminator(char *s, char replacement)
{
    if (replacement == '\0')
    {
        char *r = s;
        char *w = s;
        while (*r) {
            if (*r != '\n')
                *w++ = *r;
            r++;
        }
        *w = '\0';
        return;
    }
    size_t len = strlen(s);
    for (size_t i = 0; i < len; i++)
        if (s[i] == '\n')
            s[i] = replacement;
}

void _write_helper(const char *s)
{
    int errno_old = errno;

    size_t len = strlen(s);
    for (size_t i = 0; i < len; i++)
        write(STDIN_FILENO, &s[i], 1);

    errno = errno_old;
}

/*
 * unix_error - unix-style error routine
 */
void unix_error(const char *msg)
{
    fprintf(stdout, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

/*
 * app_error - application-style error routine
 */
void app_error(const char *msg)
{
    fprintf(stdout, "%s\n", msg);
    exit(1);
}

}
