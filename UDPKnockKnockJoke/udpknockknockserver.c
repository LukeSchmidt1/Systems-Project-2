/*
 * server-udp.c
 *
 *  Version 2.1
 *
 * A very simple UDP server that waits for a message from
 * a UDP client. Once it receives the message, it will
 * immediately sends back an acknowledgment message to
 * the client. After that, the server loops back waiting
 * for a new client.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>     // struct sockaddr_in
#include <netdb.h>
#include "Practical.h"

int main(int argc, char **argv)
{
    int sock;
    char *servPort;     // either a decimal port number in string form
                        // or a standard service name
    char buf[BUFSIZE];

    if (argc != 2) {    // Test for correct number of arguments
        fprintf(stderr, "Usage: %s port/service\n", argv[0]);
        exit(0);
    }

    servPort = argv[1];  // first command line argument is local port/service

    // Construct the server address structure
    struct addrinfo addrCriteria;                   // Criteria for address
    memset(&addrCriteria, 0, sizeof(addrCriteria)); // Zero out structure
    addrCriteria.ai_family = AF_UNSPEC;             // Any address family (AF_INET or AF_INET6)
    //addrCriteria.ai_family = AF_INET;             // Any address family (AF_INET for IPv4)
    addrCriteria.ai_flags = AI_PASSIVE;             // Accept on any address/port
                                                    // (Special ANY address)
    addrCriteria.ai_socktype = SOCK_DGRAM;          // Only datagram socket
    addrCriteria.ai_protocol = IPPROTO_UDP;         // Only UDP socket

    struct addrinfo *servAddrInfo; // List of server addresses
    int rtnVal = getaddrinfo(NULL, servPort, &addrCriteria, &servAddrInfo);
    // Note: the above hostname parameter is set to NULL
    //       Coupled with AI_PASSIVE, specialized wildcard ANY address will be
    //       used in case multiple IP addresses are configured on server machine
    if (rtnVal != 0)
        DieWithUserMessage("getaddrinfo() failed", gai_strerror(rtnVal));

    // FYI: Walk the list of address(es) and display each IP address
    int i = 0;
    for (struct addrinfo *pai = servAddrInfo; pai; pai = pai->ai_next, i++) {
        getnameinfo(pai->ai_addr, pai->ai_addrlen, buf, BUFSIZE, NULL, 0, NI_NUMERICHOST);
        printf("Server IP address (%d): %s\n", i, buf);
    }
    // You should only see wildcard addresses such as 0.0.0.0 and/or ::

    // Create socket for incoming connections
    // (note: in the creation of the local socket, we use the returned
    //        address family, socket type, protocol as given in servAddrInfo;
    //        there might be multiple servAddrInfo's linked together, but
    //        we simply use the first one.)
    sock = socket(servAddrInfo->ai_family, servAddrInfo->ai_socktype,
                  servAddrInfo->ai_protocol);
    if (sock < 0)
        DieWithSystemMessage("socket() failed");

    // Bind to the local address
    if (bind(sock, servAddrInfo->ai_addr, servAddrInfo->ai_addrlen) < 0)
        DieWithSystemMessage("bind() failed");
    // This makes the local socket a passive server socket

    // Free address list allocated by getaddrinfo()
    freeaddrinfo(servAddrInfo);
    while (1) {
    // Receive message from client
    struct sockaddr_storage clntAddr;
    socklen_t clntAddrLen = sizeof(clntAddr);
    char buffer[BUFSIZE];
    ssize_t numBytesRcvd = recvfrom(sock, buffer, BUFSIZE, 0,
                                    (struct sockaddr *) &clntAddr,
                                    &clntAddrLen);
    if (numBytesRcvd < 0)
        DieWithSystemMessage("recvfrom() failed");

    // Print the received message
    buffer[numBytesRcvd] = '\0'; // Null-terminate
    printf("%s", buffer);

    // Send first server message
    const char *msg1 = "Who is there?";
    ssize_t numBytesSent = sendto(sock, msg1, strlen(msg1), 0, 
                                   (struct sockaddr *) &clntAddr, clntAddrLen);
    if (numBytesSent < 0)
        DieWithSystemMessage("sendto() failed");

    // Wait for the next message from the client
    numBytesRcvd = recvfrom(sock, buffer, BUFSIZE, 0, 
                            (struct sockaddr *) &clntAddr, &clntAddrLen);
    if (numBytesRcvd < 0)
        DieWithSystemMessage("recvfrom() failed");

    buffer[numBytesRcvd] = '\0'; // Null-terminate
    printf("%s", buffer);

    // Send second server message
    const char *msg2 = "Robin who?";
    numBytesSent = sendto(sock, msg2, strlen(msg2), 0, 
                          (struct sockaddr *) &clntAddr, clntAddrLen);
    if (numBytesSent < 0)
        DieWithSystemMessage("sendto() failed");
}
    // WILL NEVER REACH THIS POINT

    return 0;   // not really needed, but just in case the compiler is not happy
}