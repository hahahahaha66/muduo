#include "LogStream.h"

#include <algorithm>
#include <cstdio>

//原作者奇思：利用索引，简化了负数的判断
static const char digits[] = {'9', '8', '7','6', '5', 
                              '4', '3', '2', '1', '0', 
                             '1', '2', '3',  '4', '5', 
                             '6', '7', '8', '9'};

template <typename T>
void LogStream::formatInteger(T num)
{
    if (buffer_.avail() >= kMaxNumericSize)
    {
        char* start = buffer_.current();
        char* cur = start;
        const char* zero = digits + 9;
        bool negative = (num < 0);

        do {
            int remainder = static_cast<int>(num % 10);
            *(cur++) = zero[remainder];  //不用判断正负
            num /= 10;
        } while (num != 0);

        if (negative) 
        {
            *(cur++) = '-';  //最后再加上负号
        }
        *cur = '\0';

        std::reverse(start, cur);
        buffer_.add(static_cast<int>(cur - start));
    }
}

LogStream& LogStream::operator<<(short v)
{
    *this << static_cast<int>(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned short v)
{
    *this << static_cast<unsigned int>(v);
    return *this;
}

LogStream& LogStream::operator<<(int v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned int v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(long v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(long long v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long long v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(float v)
{
    *this << static_cast<double>(v);
    return *this;
}

LogStream& LogStream::operator<<(double v)
{
    if (buffer_.avail() >= kMaxNumericSize)
    {
        int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12g", v);
        buffer_.add(len);
    }
    return *this;
}

LogStream& LogStream::operator<<(char c)
{
    buffer_.append(&c, 1);
    return *this;
}

LogStream& LogStream::operator<<(const void* data)
{
    formatPointer(data);
    return *this;
    // *this << static_cast<const char*>(data);
    // return *this;
}

LogStream& LogStream::operator<<(const char* str)
{
    if (str)
    {
        buffer_.append(str, strlen(str));
    }
    else 
    {
        buffer_.append("(null)", 6);
    }
    return *this;
}

LogStream& LogStream::operator<<(const unsigned char* str)
{
    return operator<<(reinterpret_cast<const char*>(str));
}

LogStream& LogStream::operator<<(const std::string& str)
{
    buffer_.append(str.c_str(), str.size());
    return *this;
}

LogStream& LogStream::operator<<(const Buffer& buf)
{
    *this << buf.toString();
    return *this;
}

LogStream& LogStream::operator<<(const GeneralTemplate& g)
{
    buffer_.append(g.data_, g.len_);
    return *this;
}

void LogStream::formatPointer(const void* p) {
    __intptr_t v = reinterpret_cast<__intptr_t>(p);
    if (buffer_.avail() >= kMaxNumericSize) {
      char* buf = buffer_.current();
      snprintf(buf, kMaxNumericSize, "0x%lx", v);  // 或使用 PRIxPTR 更安全
      buffer_.add(strlen(buf));
    }
  }