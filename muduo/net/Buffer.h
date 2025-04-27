#ifndef BUFFER_H
#define BUFFER_H

#include <cstddef>
#include <vector>
#include <string>
#include <algorithm>

class Buffer
{
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    explicit Buffer(size_t initialSize = kInitialSize)
        : buffer_(kCheapPrepend + initialSize),
          readerIndex_(kCheapPrepend),
          writerIndex_(kCheapPrepend)
    {
    }

    //可读大小
    size_t readableBytes() const { return writerIndex_ - readerIndex_; }
    //可写大小
    size_t writableBytes() const { return buffer_.size() - writerIndex_; }
    //头部预留大小
    size_t prependableBytes() const { return readerIndex_; }

    //返回可读区块的头指针
    const char* peek() const
    {
        return begin() + readerIndex_;
    }
    
    void retrieveUntil(const char* end)
    {
        retrieve(end - peek());
    }

    void retrieve(size_t len)
    {
        //只读了缓冲区的部分长度
        if (len < readableBytes())
        {
            readerIndex_ += len;
        }
        else  //读完缓冲区的全部内容
        {
            retrieveAll();
        }
    }

    void retrieveAll()
    {
        //重置读写指针
        readerIndex_ = kCheapPrepend;
        writerIndex_ = kCheapPrepend;    
    }

    std::string GetBufferAllAsString()
    {
        size_t len = readableBytes();
        std::string result(peek(), len);
        return result;
    }

    std::string retrieveAllAsString()
    {
        return retrieveAsString(readableBytes());
    }

    std::string retrieveAsString(size_t len)
    {
        std::string result(peek(), len);
        retrieve(len);
        return result;
    }

    void ensureWritableBytes(size_t len)
    {
        if (writableBytes() < len)
        {
            makeSpace(len);
        }
    }

    void append(const std::string& str)
    {
        append(str.data(), str.size());
    }

    void append(const char* data, size_t len)
    {
        ensureWritableBytes(len);
        std::copy(data, data + len, beginWrite());
        writerIndex_ += len;
    }

    const char* findCRLF() const 
    {
        const char* crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF + 2);
        return crlf == beginWrite() ? NULL : crlf;
    }

    char* beginWrite()
    {
        return begin() + writerIndex_;
    }

    const char* beginWrite() const
    {
        return begin() + writerIndex_;
    }

    ssize_t readFd(int fd, int* saveErrno);
    ssize_t writeFd(int fd, int* saveErrno);

private:
    char* begin()
    {
        return &(*buffer_.begin());
    }

    const char* begin() const
    {
        return &(*buffer_.begin());
    }

    void makeSpace(int len)
    {
        if (writableBytes() + prependableBytes() < len + kCheapPrepend)
        {
            buffer_.resize(writerIndex_ + len);
        }
        else 
        {
            size_t readable = readableBytes();
            std::copy(begin() + readerIndex_, begin() + writerIndex_, begin() + kCheapPrepend);
            readerIndex_ = kCheapPrepend;
            writerIndex_ = readerIndex_ + readable;
        }
    }

    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;
    static const char kCRLF[];
};

#endif