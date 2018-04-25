#include "sserver.h"
int sserver::smallSet(char* machine_name, int port,unsigned int SecretKey, char* variable_name, char*value,unsigned short & data_length){
  sserver::request_t req;
  req.key=SecretKey;
  req.request_type=request_operation::set;
  req.buffer_length=18+data_length;
  strncpy(req.data,variable_name,16);
  unsigned short temp = htons(data_length);
  strncpy(req.data+16,(char*)(&temp),2);
  strncpy(req.data+18,value,data_length);
  sserver::response_t response = send_request(machine_name, port, req);
  return (int)(response.success);
}
int sserver::smallGet(char* machine_name, int port,unsigned int SecretKey, char* variable_name, char*value,unsigned short & data_length){
  sserver::request_t req;
  req.key=SecretKey;
  req.request_type=request_operation::get;
  req.buffer_length=16;
  strncpy(req.data,variable_name,16);

  sserver::response_t response = send_request(machine_name, port, req);
  if(response.success==0){
    data_length = response.buffer_length;
    value = (char*) malloc(sizeof(char)*data_length);
    strncpy(value,response.data,data_length);
  }
  return (int)(response.success);
}
int sserver::smallDigest(char* machine_name, int port,unsigned int SecretKey, char* data, unsigned short data_length, char* result,unsigned short & result_length){
  sserver::request_t req;
  req.key=SecretKey;
  req.request_type=request_operation::digest;
  req.buffer_length=2+data_length;
  unsigned short temp = htons(data_length);
  strncpy(req.data,(char*)(&temp),2);
  strncpy(req.data+2,data,data_length);
  sserver::response_t response = send_request(machine_name, port, req);
  if(response.success==0){
    result_length=response.buffer_length;
    result =(char*)malloc(sizeof(char)*result_length);
    strncpy(result,response.data,result_length);
  }
  return (int)(response.success);
}
int sserver::smallRun(char* machine_name, int port,unsigned int SecretKey, char* request, char* result,unsigned short & result_length){
  sserver::request_t req;
  req.key=SecretKey;
  req.request_type=request_operation::run;
  req.buffer_length=8;
  strncpy(req.data,request,8);
  sserver::response_t response = send_request(machine_name, port, req);
  if(response.success==0){
    result_length = response.buffer_length;
    result =(char*) malloc(sizeof(char)*result_length);
    strncpy(result,response.data,result_length);
  }
  return (int)(response.success);
}
sserver::response_t sserver::send_request(char* machine_name, int port, sserver::request_t request){
  sserver::response_t response;
  //struct sockaddr_in address;
  struct sockaddr_in server_address;
  int sock=0;
  
  //Open connection
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    response.success=-1;
    return response;
  }
    
  memset(&server_address, '0', sizeof(server_address));

  server_address.sin_family=AF_INET;
  server_address.sin_port=htons(port);
  if(inet_pton(AF_INET, machine_name, &server_address.sin_addr)<=0) 
  {
    response.success=-1;
    return response;
  }    
  
  
  if (connect(sock,(struct sockaddr*)&server_address, sizeof(server_address)) < 0){
    response.success=-1;
    return response;
  }

  //Send request
  unsigned int temp=htonl(request.key);
  send(sock,(void*)(&temp),4,0);
  unsigned short temp_2=htons(request.request_type);
  send(sock,(void*)(&temp_2),2,0);
  send(sock,"42",2,0);//unused
  send(sock,request.data,request.buffer_length,0);
  
  //Retrieve, format response
  int length_received=0;
  length_received=read(sock,&(response.success),1);
  if(length_received<=0){
    response.success=-1;
    return response;
  }
  if(request.request_type!=request_operation::set){
    char garbage[3];
    read(sock,garbage,3);
    read(sock,&(response.buffer_length),2);
    response.buffer_length=htons(response.buffer_length);
    read(sock,response.data,response.buffer_length);
  }
  return response;
} 
