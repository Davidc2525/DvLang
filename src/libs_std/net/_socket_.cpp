
#include "_socket_.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

//--implementacion
_Socket_ *_Socket_::instance = 0;

_Socket_ *_Socket_::getInstace()
{
    if (instance == 0)
    {
        instance = new _Socket_();
    }

    return instance;
}

int _Socket_::create_socket(int a, int b, char *adr, int port)
{
    int sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server; //TODO

    // crear el socket
    // socket(int domain, int type, int protocol)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
        error("ERROR opening socket");

    //sockes_fd.insert(make_pair(index, sockfd));

    server = gethostbyname(adr);

    bzero((char *)&serv_addr, sizeof(serv_addr));

    portno = port;

    serv_addr.sin_family = AF_INET;

    serv_addr.sin_addr.s_addr = INADDR_ANY;

    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);

    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

#if DEBUG
    cout << "server fd: " << sockfd << " adr: " << adr << " port: " << port << endl;
#endif

   // index++;
    return sockfd;
}

int _Socket_::_accept(int s_fd)
{
#if DEBUG
    cout << "accept s_fd:" << s_fd << endl;
#endif

    int newsockfd;
    socklen_t clilen;
    struct sockaddr_in cli_addr;
    clilen = sizeof(cli_addr);

    listen(s_fd, 5);

    newsockfd = accept(s_fd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");

    //sockets_con_fd.insert(make_pair(index_c, newsockfd));
    //index_c++;
#if DEBUG
    printf("server: got connection from  port %d\n", ntohs(cli_addr.sin_port));
    cout << "conn fd:" << newsockfd << endl;
#endif

    return newsockfd;
}

int _Socket_::_connect(char *adr, int port)
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    portno = port;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    server = gethostbyname(adr);

    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;

    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);

    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

#if DEBUG
    cout << "Connected with server: " << adr << " port: " << port << " s_fd: " << sockfd << endl;
#endif
    return sockfd;
}

int _Socket_::_write(int c_fd, char *content, int len)
{
    int n = 0;
    n = write(c_fd, content, len);
    if (n < 0)
        error("ERROR writing to socket");

    return n;
}

int _Socket_::_send(int c_fd, char *content, int len)
{
    /* char len_p[10];
    sprintf(len_p,"%.10i",len);
    cout<<"LEN: "<<len_p<<endl; */
    return send(c_fd, content, len, MSG_DONTWAIT);
}

int _Socket_::_read(int c_fd, char *buffer, int len)
{
    int n = 0;

    char *res;

    n = read(c_fd, buffer, len);
    if (n < 0)
        error("ERROR reading from socket");
    if (n == 0)
        error("CONNECTION CLOSED.");

#if DEBUG
    printf("Here is the message: %s\n", buffer);
    cout << "n: " << n << " strl: " << strlen(buffer) << endl;
#endif
    /* res = (char *)malloc(n+1 * sizeof(char *));
    sprintf(res, "%s", buffer); */
    /*  for (unsigned int x = 0; x < n; x++)
    {
        res[x] = buffer[x];
    } */
    /* free(buffer); */
    return n;
}

int _Socket_::_close(int s_fd)
{
    close(s_fd);
}
