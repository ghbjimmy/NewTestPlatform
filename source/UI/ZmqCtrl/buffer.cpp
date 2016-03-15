#include "buffer.h"
#include <string.h>

Buffer::Buffer()
{
    _len = -1;
    _buf = NULL;
}

Buffer::~Buffer()
{
    clear();
}

Buffer::Buffer(const Buffer& other)
{
    if (&other == this)
    {
        return;
    }

    if (other._len > 0)
    {
        _buf = new char[other._len];
        memcpy(_buf, other._buf, other._len);
        _len = other._len;
    }
    else
    {
        clear();
    }
}

Buffer& Buffer::operator = (Buffer& other)
{
    if (&other == this)
    {
        return *this;
    }

    if (other._len > 0)
    {
        _buf = new char[other._len];
        memcpy(_buf, other._buf, other._len);
        _len = other._len;
    }
    else
    {
        clear();
    }

    return *this;
}

bool Buffer::isValid() const
{
    if (_buf == NULL || _len <= 0)
        return false;
    return true;
}

void Buffer::clear()
{
    if (_buf != NULL && _len > 0)
    {
        delete[] _buf;
        _buf = NULL;
    }

    _len = -1;
}

void Buffer::setBuf(const char* buf, int len)
{
    clear();
    _buf = new char[len];
    memcpy(_buf, buf, len);
    _len = len;
}
