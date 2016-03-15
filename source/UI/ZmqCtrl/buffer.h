#ifndef BUFFER_H
#define BUFFER_H


class Buffer
{
public:
    Buffer();
    ~Buffer();
    Buffer(const Buffer& other);
    Buffer& operator = (Buffer& other);

    bool isValid() const;
    void clear();
    inline int getLen() const { return _len; }
    const char* getBuf() const { return _buf; }

    void setBuf(const char* buf, int len);

private:
    int _len;
    char* _buf;
};

#endif // BUFFER_H
