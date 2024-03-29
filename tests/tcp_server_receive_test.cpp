#include "net/tcp_server.h"
#include "net/tcp_connection.h"
#include "net/event_loop.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void onConnection(const Tcp_Server::Tcp_Connection_Ptr& conn)
{
  if (conn->connected())
  {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &conn->peer_address().sin_addr, ip, INET_ADDRSTRLEN);
    printf("onConnection(): new connection [%s] from %s\n",
           conn->name().c_str(), ip);
  }
  else
  {
    printf("onConnection(): connection [%s] is down\n",
           conn->name().c_str());
  }
}

void onMessage(const Tcp_Server::Tcp_Connection_Ptr& conn,
               Buffer* buf,
               Timestamp receiveTime)
{
  printf("onMessage(): received %zd bytes from connection [%s] at %s\n",
         buf->readable_bytes(),
         conn->name().c_str(),
         receiveTime.to_formatted_string().c_str());

  printf("onMessage(): [%s]\n", buf->retrieve_all_as_string().c_str());
}

int main()
{
  printf("main(): pid = %d\n", getpid());

  sockaddr_in listen_addr;
  memset(&listen_addr, 0, sizeof(listen_addr));
  listen_addr.sin_family = AF_INET;
  listen_addr.sin_addr.s_addr = INADDR_ANY;
  listen_addr.sin_port = htons(9981);
  Event_Loop loop;

  Tcp_Server server(&loop, listen_addr, "test_server");
  server.set_connection_callback(onConnection);
  server.set_message_callback(onMessage);
  server.start();

  loop.loop();
}