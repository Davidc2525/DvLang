#include <unordered_map>
#include <iostream>
using namespace std;

#ifndef _SOCKET_H_
#define _SOCKET_H_

//-- interface
class _Socket_
{
public:
  static _Socket_ *instance;

  static _Socket_ *getInstace();

  int create_socket(int, int, char *adr, int port);

  int _accept(int);

  int _connect(char *adr, int port);

  int _write(int s_fd, char *content, int len);

  int _send(int c_fd, char *content, int len);

  //char *_read(int c_fd);

  int _read(int c_fd, char *buffer, int len);

  int _close(int s_fd);

private:
  _Socket_()
  {
#if DEBUG
    cout << "new socket manager libnet" << endl;
#endif
  };
  int index = 0;
  int index_c = 0;
  //vector<int> sockets_fd;
  unordered_map<int, int> sockes_fd;
  unordered_map<int, int> sockets_con_fd;

  unordered_map<int, int> socket_by_fd;
};

#endif