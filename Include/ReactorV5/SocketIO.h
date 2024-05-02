#ifndef __SOCKETIO_H__
#define __SOCKETIO_H__

namespace OurPool
{
    class SocketIO
    {
    public:
        explicit SocketIO(int fd);
        ~SocketIO();
        int readn(char *buf, int len);
        int readLine(char *buf, int len);
        int writen(const char *buf, int len);

    private:
        int _fd;
    };

}; // end of OurPool

#endif
