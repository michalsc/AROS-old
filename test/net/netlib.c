/*
 * Small netlib autoopen test.
 * In future may grow into something bigger.
 */

#include <proto/socket.h>

#include <stdio.h>

int __nocommandline = 1;

int main(void)
{
    printf("Library opened, SocketBase is 0x%p\n", SocketBase);

    return 0;
}
