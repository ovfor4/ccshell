#pragma once

#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <cstdio>

#include "error.h"

namespace ov4
{

class T_unix_file_d;
class T_unix_file_p;


class T_unix_file_d
{
public:
    int fd = -1;

    virtual int close()
    {
        if (fd == -1) return -1;
        int ret = ::close(fd);
        fd = -1;
        return ret;
    }

    T_unix_file_d(const std::string &path, int oflag, int mode = -1)
    {
        if (oflag & O_CREAT)
        {
            if (mode == -1) throw T_error{error_code::FILE_MODE_NOT_SET, "Mode should be set for O_CREAT"};
            fd = open(path.c_str(), oflag, mode);
            if (fd == -1)
                throw T_error(error_code::FILE_FAIL_TO_OPEN, "Fail to open file");
            return;
        }
        fd = open(path.c_str(), oflag);
        if (fd == -1)
            throw T_error(error_code::FILE_FAIL_TO_OPEN, "Fail to open file");
    }

    ~T_unix_file_d()
    {
        this->close();
    }

    T_unix_file_d(const T_unix_file_d &) = delete;
    T_unix_file_d &operator=(const T_unix_file_d &) = delete;
};

class T_unix_file_p : public T_unix_file_d
{
public:
    FILE* fp = nullptr;

    int close() override
    {
        if (fp == nullptr) return -1;
        int ret = fclose(fp);
        fp = nullptr;
        fd = -1;
        return ret;
    }

    T_unix_file_p(const std::string &path, int oflag, int mode = -1) 
        : T_unix_file_d(path, oflag, mode)
    {
        const char *fp_mode;
        switch (oflag & O_ACCMODE)
        {
            case O_RDONLY:
                fp_mode = "r";
                break;
            
            case O_WRONLY:
                fp_mode = "w";
                break;

            case O_RDWR:
                fp_mode = "r+";
                break;

            default:
                throw T_error{error_code::FILE_OFLAG_ERROR, "oflag error"};
        }
        fp = fdopen(fd, fp_mode);

        if (fp == nullptr)
            throw T_error{error_code::FILE_POINTER_FAIL_TO_OPEN, "Fail to open file pointer"};
    }

    ~T_unix_file_p()
    {
        this->close();
    }

    T_unix_file_p(const T_unix_file_p &) = delete;
    T_unix_file_p &operator=(const T_unix_file_p &) = delete;
};

}