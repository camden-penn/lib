#pragma once
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdlib>
#define MAX_BUFFER_LENGTH 200
class sserver{
  public:
    int smallSet(char* machine_name, int port,unsigned int SecretKey, char* variable_name, char*value,unsigned short & data_length);
    int smallGet(char* machine_name, int port,unsigned int SecretKey, char* variable_name, char*value,unsigned short & data_length);
    int smallDigest(char* machine_name, int port,unsigned int SecretKey, char* data, unsigned short data_length, char* result,unsigned short & result_length);
    int smallRun(char* machine_name, int port,unsigned int SecretKey, char* request, char* result,unsigned short & resultLength);
    enum request_operation:unsigned short{
      set,
      get,
      digest,
      run
    };
    typedef struct request_t{
      unsigned int key;
      sserver::request_operation request_type;
      unsigned short buffer_length;
      char data[MAX_BUFFER_LENGTH];
    } request_t;
    typedef struct response_t{
      signed char success;
      unsigned short buffer_length;
      char data[MAX_BUFFER_LENGTH];
    } response_t;
  private:
    sserver::response_t send_request(char* machine_name, int port, sserver::request_t request); 
};
