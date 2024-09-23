/*
 * client-udp.c
 *
 *  Version 2.1
 *
 * A very simple UDP client that reads a line of user input
 * and sends it (message) to a UDP server. The client then
 * waits for the server to acknowledge the receipt of the
 * message and terminates.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "Practical.h"

int main(int argc, char **argv)
{
    int sock;
    char *server;
    char *servPort;     // either a decimal port number in string form
                        // or a standard service name
    char buf[BUFSIZE];

    if (argc != 3) {
        fprintf(stderr, "Usage: %s server port/service\n", argv[0]);
        exit(0);
    }

    server = argv[1];     // First arg: server address/name
    servPort = argv[2];   // Second arg: server port/service

    // Tell the system what kind(s) of address info we want
    struct addrinfo addrCriteria;                   // Criteria for address match
    memset(&addrCriteria, 0, sizeof(addrCriteria)); // Zero out structure
    addrCriteria.ai_family = AF_UNSPEC;             // Any address family (AF_INET or AF_INET6)
    //addrCriteria.ai_family = AF_INET;             // Any address family (AF_INET for IPv4)
    // For the following fields, a zero value means "don't care"
    addrCriteria.ai_socktype = SOCK_DGRAM;          // Only datagram sockets
    addrCriteria.ai_protocol = IPPROTO_UDP;         // Only UDP protocol

    // Get address(es)
    struct addrinfo *servAddrInfo; // List of server addresses
    int rtnVal = getaddrinfo(server, servPort, &addrCriteria, &servAddrInfo);
    if (rtnVal != 0)
        DieWithUserMessage("getaddrinfo() failed", gai_strerror(rtnVal));

    // FYI: Walk the list of address(es) and display each IP address
    int i = 0;
    for (struct addrinfo *pai = servAddrInfo; pai; pai = pai->ai_next, i++) {
        getnameinfo(pai->ai_addr, pai->ai_addrlen, buf, BUFSIZE, NULL, 0, NI_NUMERICHOST);
        printf("Server IP address (%d): %s\n", i, buf);
    }

    // Create a datagram/UDP socket on client side
    // (note: in the creation of the client socket, we must match server's
    //        address family, socket type, protocol as given in servAddrInfo;
    //        there might be multiple servAddrInfo's linked together, but
    //        we simply use the first one.)
    sock = socket(servAddrInfo->ai_family, servAddrInfo->ai_socktype,
                  servAddrInfo->ai_protocol); // Socket descriptor for client
    if (sock < 0)
        DieWithSystemMessage("socket() failed");

    // This is the (real) server socket address in generic structure
    struct sockaddr *p_servAddr = servAddrInfo->ai_addr;
    socklen_t = servAddrInfo->ai_addrlen;
    // Caution: Note that I didn't do a deep copy of the server's sockaddr structure.
    //          So don't free the servAddrInfo until you are done with the p_servAddr.
    //          Or you will use the sockaddr while it has been freed, which is
    //          a severe violation (On Linux, you may not see the problem
    //          manifest right away, but on Mac, it immediately crashes when I
    //          test your code.)

 
    // send first message & wait for response
    const char *msg1 = "Knock Knock\n";
    size_t sendlen = strlen(msg1);
    ssize_t numBytes = sendto(sock, msg1, sendlen, 0, p_servAddr, servAddrLen);
    if (numBytes < 0)
        DieWithSystemMessage("sendto() failed");
    else if (numBytes != sendlen)
        DieWithUserMessage("sendto() error", "sent unexpected number of bytes");
    
    struct sockaddr_storage fromAddr;
    socklen_t fromAddrLen = sizeof(fromAddr);
    numBytes = recvfrom(sock, buf, BUFSIZE, 0, (struct sockaddr *) &fromAddr, &fromAddrLen);
    if (numBytes < 0)
        DieWithSystemMessage("recvfrom() failed");

    // Verify reception from expected source
    if (!SockAddrsEqual(p_servAddr, (struct sockaddr *)&fromAddr))
        DieWithUserMessage("recvfrom()", "received a packet from unknown source");

    fputs("Recieved: ", stdout);
    fflush(stdout);
    write(1, buf, numBytes);
    fputc('\n', stdout);

    // send second message & wait for response
    const char *msg2 = "Robin\n";
    sendlen = strlen(msg2);
    numBytes = sendto(sock, msg2, sendlen, 0, p_servAddr, servAddrLen);
    if (numBytes < 0)
        DieWithSystemMessage("sendto() failed");
    else if (numBytes != sendlen)
        DieWithUserMessage("sendto() error", "sent unexpected number of bytes");
    
    //struct sockaddr_storage fromAddr;
    //socklen_t fromAddrLen = sizeof(fromAddr);
    numBytes = recvfrom(sock, buf, BUFSIZE, 0, (struct sockaddr *) &fromAddr, &fromAddrLen);
    if (numBytes < 0)
        DieWithSystemMessage("recvfrom() failed");

    // Verify reception from expected source
    if (!SockAddrsEqual(p_servAddr, (struct sockaddr *)&fromAddr))
        DieWithUserMessage("recvfrom()", "received a packet from unknown source");

    fputs("Recieved: ", stdout);
    fflush(stdout);
    write(1, buf, numBytes);
    fputc('\n', stdout);
    
    // recieve response & send third message
    const char *msg3 = "Robin you! Hand over your cash\n";
    sendlen = strlen(msg3);
    numBytes = sendto(sock, msg3, sendlen, 0, p_servAddr, servAddrLen);
    if (numBytes < 0)
        DieWithSystemMessage("sendto() failed");
    else if (numBytes != sendlen)
        DieWithUserMessage("sendto() error", "sent unexpected number of bytes");

    // Receive a response
    //struct sockaddr_storage fromAddr;   // Source address of server
    // Must set length of from address structure (in-out parameter)
    fromAddrLen = sizeof(fromAddr);
    numBytes = recvfrom(sock, buf, BUFSIZE, 0,
                        (struct sockaddr *) &fromAddr,
                        &fromAddrLen);
    // above call also set the fromAddr and fromAddrLen
    // so that we can find out which server was talking to the client
    if (numBytes < 0)
        DieWithSystemMessage("recvfrom() failed");

    // Verify reception from expected source
    if (!SockAddrsEqual(p_servAddr, (struct sockaddr *) &fromAddr))
        DieWithUserMessage("recvfrom()", "received a packet from unknown source");

    // Now that I am done with p_servAddr, I can free servAddrInfo.
    freeaddrinfo(servAddrInfo);

    // display the received message
    //fputs("Received ack: ", stdout);
    fflush(stdout);
    write(1, buf, numBytes);
    fputc('\n', stdout);

    // let's print out the server information
    //fputs("From: ", stdout);
    PrintSocketAddress((struct sockaddr *) &fromAddr, stdout);
    fputc('\n', stdout);
    fputc('\n', stdout);
    fputc('\n', stdout);

    // close sock to release resource
    close(sock);

    return 0;
}
