#include "../common/exp1.h"
#include "../common/exp1lib.h"
#include "../common/exp1lib.c"
#include<string.h>


typedef struct
{
  char cmd[64];
  char path[256];
  char real_path[256];
  char type[64];
  int code;
  int size;
  char body[16384];
} exp1_info_type;



void exp1_parse_status(char* status, exp1_info_type *pinfo)
{
  char cmd[1024];
  char path[1024];
  char* pext;
  int i, j;

  enum state_type
  {
    SEARCH_CMD,
    SEARCH_PATH,
    SEARCH_END
  }state;

  state = SEARCH_CMD;
  j = 0;
  for(i = 0; i < strlen(status); i++){
    switch(state){
    case SEARCH_CMD:
      if(status[i] == ' '){
        cmd[j] = '\0';
        j = 0;
        state = SEARCH_PATH;
      }else{
        cmd[j] = status[i];
        j++;
      }
      break;
    case SEARCH_PATH:
      if(status[i] == ' '){
        path[j] = '\0';
        j = 0;
        state = SEARCH_END;
      }else{
        path[j] = status[i];
        j++;
      }
      break;
    }
  }

  strcpy(pinfo->cmd, cmd);
  strcpy(pinfo->path, path);
}

void exp1_check_file(exp1_info_type *info)
{
  struct stat s;
  int ret;
  char* pext;

  sprintf(info->real_path, "html%s", info->path);
  ret = stat(info->real_path, &s);

  if((s.st_mode & S_IFMT) == S_IFDIR){
    sprintf(info->real_path, "%s/index.html", info->real_path);
  }

  ret = stat(info->real_path, &s);

  if(ret == -1){
    info->code = 404;
  }else{
    info->code = 200;
    info->size = (int) s.st_size;
  }

  pext = strstr(info->path, ".");
  if(pext != NULL && strcmp(pext, ".html") == 0){
    strcpy(info->type, "text/html");
  }else if(pext != NULL && strcmp(pext, ".jpg") == 0){
    strcpy(info->type, "image/jpeg");
  }else if(pext != NULL && strcmp(pext, ".js") == 0){
    strcpy(info->type, "text/javascript");
  }else if(pext != NULL && strcmp(pext, ".php") == 0){
    strcpy(info->type, "text/html");
  }else if(pext != NULL && strcmp(pext, ".mp4") == 0){
    strcpy(info->type, "audio/mp4");
  }
  
}

int exp1_parse_post(char* buf, int size, exp1_info_type* info)
{
  char header[16384];
  char body[16384];
  int i=0, j=0;

  enum {
    E_FIND_CR,
    E_FIND_CRLF,
    E_FIND_CRLFCR,
    E_FIND_CRLFCRLF,
    E_FIND_NULL
  } parse_status;

  bzero(header, sizeof(header));
  bzero(body, sizeof(body));

  parse_status = E_FIND_CR;

    while(buf[i] != 0){
    switch(parse_status){
    case E_FIND_CR:
      header[j] = buf[i];
      j++;

      if(buf[i] == '\r'){
        parse_status = E_FIND_CRLF;
      }else{
        parse_status = E_FIND_CR;
      }
      break;
    case E_FIND_CRLF:
      header[j] = buf[i];
      j++;

      if(buf[i] == '\n'){
        parse_status = E_FIND_CRLFCR;
      }else{
        parse_status = E_FIND_CR;
      }
      break;
    case E_FIND_CRLFCR:
      header[j] = buf[i];
      j++;

      if(buf[i] == '\r'){
        parse_status = E_FIND_CRLFCRLF;
      }else{
        parse_status = E_FIND_CR;
      }
      break;
    case E_FIND_CRLFCRLF:
      header[j] = buf[i];
      j++;

      if(buf[i] == '\n'){
        parse_status = E_FIND_NULL;
        j = 0;
      }else{
        parse_status = E_FIND_CR;
      }

      break;
    case E_FIND_NULL:
      body[j] = buf[i];
      j++;
      break;
    default:
      printf("impossible\n");
      exit(-1);
    }
    i++;
  }

  //printf("################ header is ##############\n");
  //printf("%s\n", header);

  //printf("################ body is ################\n");
  //printf("%s\n", body);
  strcpy(info->body, body);

  return 0;

}

int getstring(char *src, char *element, char *dest)
{
    int len;
    char *data, *amp;
    char *temp;

    len = (int)strlen(src) + 1;
    temp = (char *)malloc(len);

    len = (int)strlen(src);


    strcpy(temp, src);

    data = strstr(temp, element);
    if(data == NULL) {
        strcpy(dest, "no element");
        free(temp);
        return -2;
    }

    len = (int)strlen(element) + 1;

    amp = strstr(data, "&");
    if (amp == NULL) {
        strcpy(dest, data + len);
        if (dest[0] == '\0') {
            strcpy(dest, "no data");
            free(temp);
            return 0;
        }
        free(temp);
        return 1;
    }
    data[(int)(amp-data)] = '\0';
    strcpy(dest, data + len);

    if (dest[0] == '\0') {
        strcpy(dest, "no data");
        free(temp);
        return 0;
    }

    free(temp);
    return 0;
}


int exp1_parse_header(char* buf, int size, exp1_info_type* info)
{
  char status[1024];
  int i, j;

  enum state_type
  {
    PARSE_STATUS,
    PARSE_END
  }state;

  state = PARSE_STATUS;
  j = 0;
  for(i = 0; i < size; i++){
    switch(state){
    case PARSE_STATUS:
      if(buf[i] == '\r'){
        status[j] = '\0';
        j = 0;
        state = PARSE_END;
        exp1_parse_status(status, info);
        exp1_check_file(info);


        if(strncmp(status,"POST",4)==0){
            exp1_parse_post(buf, size,info);
        }
        //printf("infotype:%s\n",info->type);
        
        
      }else{
        status[j] = buf[i];
        j++;
      }
      //printf("i:%d",i);
      break;
    }

    if(state == PARSE_END){
      return 1;
    }
  }
  return 0;
}



void exp1_send_404(int sock)
{
  char buf[16384];
  int ret;

  sprintf(buf, "HTTP/1.0 404 Not Found\r\n\r\n");
  printf("%s", buf);
  ret = send(sock, buf, strlen(buf), 0);

  if(ret < 0){
    shutdown(sock, SHUT_RDWR);
    close(sock);
  }
}

void exp1_send_file(int sock, char* filename)
{
  FILE *fp;
  int len;
  char buf[16384];

  fp = fopen(filename, "r");
  if(fp == NULL){
    shutdown(sock, SHUT_RDWR);
    close(sock);
    return;
  }

  len = fread(buf, sizeof(char), 16384, fp);
  while(len > 0){
    int ret = send(sock, buf, len, 0);
    if(ret < 0){
      shutdown(sock, SHUT_RDWR);
      close(sock);
      break;
    }
    len = fread(buf, sizeof(char), 1460, fp);
  }

  fclose(fp);
}



void exp1_http_reply(int sock, exp1_info_type *info)
{
  char buf[16384];
  int len;
  int ret;
  char name[64]= {};

  if(info->code == 404){
    exp1_send_404(sock);
    printf("404 not found %s\n", info->path);
    return;
  }
  getstring(info->body, "name", name);
  
  if(strncmp(info->cmd,"GET",3)==0){
    printf("GET HTTP/1.0 200 OK\n");
    printf("Content-Length: %d\r\n", info->size);
    printf("Content-Type: %s\r\n", info->type);
    printf("\r\n");
    len = sprintf(buf, "HTTP/1.0 200 OK\r\n");
    len += sprintf(buf + len, "Content-Length: %d\r\n", info->size);
    len += sprintf(buf + len, "Content-Type: %s\r\n", info->type);
    len += sprintf(buf + len, "\r\n");

    ret = send(sock, buf, len, 0);
    if(ret < 0){
      shutdown(sock, SHUT_RDWR);
      close(sock);
      return;
    }
    exp1_send_file(sock, info->real_path);
  }else if(strncmp(info->cmd,"POST",4)==0){
    printf("POST HTTP/1.0 200 OK\n");
    printf("Content-Length: %d\r\n", info->size);
    printf("Content-Type: text/html\r\n");
    printf("\r\n");
    printf("%s\r\n",info->body);


    //system("php html/php/ajax.php > tmp.html");

    len = sprintf(buf, "HTTP/1.0 200 OK\r\n");
    len += sprintf(buf + len, "Content-Length: %d\r\n", info->size);
    len += sprintf(buf + len, "Content-Type: text/html\r\n");
    len += sprintf(buf + len, "\r\n");
    len += sprintf(buf + len, "You are name is %s<BR>\n",name);
    ret = send(sock, buf, len, 0);
    printf("ret:%d",ret);

    if(ret < 0){
      shutdown(sock, SHUT_RDWR);
      close(sock);
      return;
    }
    
    
    exp1_send_file(sock, info->real_path);
    system("php test.php > tmp.html");

    
  }
}

int exp1_http_session(int sock)
{
  char buf[2048]={};
  int recv_size = 0;
  exp1_info_type info;
  int ret = 0;
  
  while(ret == 0){
    int size = recv(sock, buf + recv_size, 2048, 0);

    if(size == -1){
      return -1;
    }

    recv_size += size;
    ret = exp1_parse_header(buf, recv_size, &info);
    
  }
  //printf("buf:\n%s\n",buf);
  //printf("post:%d\n",strncmp(info.cmd,"POST",4));
  exp1_http_reply(sock, &info);

  return 0;
}

int main(int argc, char **argv)
{
  int sock_listen;

  sock_listen = exp1_tcp_listen(16098);

  while(1){
    struct sockaddr addr;
    int sock_client;
    int len;

    sock_client = accept(sock_listen, &addr, (socklen_t*) &len);
 
    exp1_http_session(sock_client);

    shutdown(sock_client, SHUT_RDWR);
    close(sock_client);
  }
}