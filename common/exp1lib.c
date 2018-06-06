#include "exp1lib.h"
#include "exp1.h"

int exp1_tcp_listen(int port)
{
  int sock;
  struct sockaddr_in addr;
  int yes = 1;
  int ret;

  sock = socket(PF_INET, SOCK_STREAM, 0);
  if(sock < 0) {
	perror("socket");
	exit(1);
  }

  bzero((char *) &addr, sizeof(addr));
  addr.sin_family = PF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

  ret =  bind(sock, (struct sockaddr *)&addr, sizeof(addr));
  if(ret < 0) {
	perror("bind");
	exit(1);
  }

  ret = listen(sock, 5);
  if(ret < 0) {
	perror("reader: listen");
	close(sock);
	exit(-1);
  }

  return sock;
}

int exp1_tcp_connect(const char *hostname, int port)
{
  int sock;
  int ret;
  struct sockaddr_in addr;
  struct hostent *host;

  //ソケットの作成
  sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  // ソケットの設定
  addr.sin_family = AF_INET;
  host = gethostbyname(hostname);
  addr.sin_addr = *(struct in_addr *)(host->h_addr_list[0]);
  addr.sin_port = htons(port);

  ret = connect(sock, (struct sockaddr *)&addr, sizeof addr);
  if(ret < 0){
	return -1;
  }else{
	return sock;
  }
}