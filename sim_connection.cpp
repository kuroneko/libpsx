#include "psx.h"
#include <cstdio>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#endif

using namespace std;
using namespace psx;

SimConnection::SimConnection(const std::string &hostname, int port)
  : hostname(hostname), port(port)
{
  msgMutex = SDL_CreateMutex();
}

SimConnection::~SimConnection()
{
  stopListener();
  if (NULL != msgMutex) {
    SDL_DestroyMutex(msgMutex);
    msgMutex = NULL;
  }
}

bool
SimConnection::connect()
{
  char    serviceName[16];
  snprintf(serviceName, 16, "%d", port);
  serviceName[15] = '\0';

  struct addrinfo addrHints = {
    ai_flags: AI_NUMERICSERV | AI_ADDRCONFIG,
    ai_family: AF_INET,
    ai_socktype: SOCK_STREAM,
  };

  struct addrinfo *results = NULL;
  if (!getaddrinfo(hostname.c_str(), serviceName, &addrHints, &results)) {
    return false;
  }

  socket_fd = socket(results->ai_family, results->ai_socktype, 0);
  if (-1 == socket_fd) {
    return false;
  }
  int retry_delay = base_retry_interval;
  while (running) {
    if (!::connect(socket_fd, results->ai_addr, results->ai_addrlen)) {
      return true;
    }
    SDL_Delay(retry_delay);
    if ((retry_delay * 2) <= max_retry_interval) {
      retry_delay *= 2;
    }
  }
  close(socket_fd);
  return false;
}

void
SimConnection::stopListener()
{
  running = false;
  // force shtudown the socket - this should trip a 0 byte iop in the listener which will let it die.
  if (socket_fd >= 0) {
    shutdown(socket_fd, SHUT_RDWR);
  }
  if (rcvThread != NULL) {
    SDL_WaitThread(rcvThread, NULL);
    rcvThread = NULL;
  }
}

extern "C" {
  static int
  bootstrapListener(void *data)
  {
    SimConnection *sc = static_cast<SimConnection *>(data);

    return sc->listener();
  }
}

void
SimConnection::startListener()
{
  if (NULL != rcvThread) {
    return;
  }
  running = true;
  rcvThread = SDL_CreateThread(bootstrapListener, "PSXListener", static_cast<void*>(this));
}

int
SimConnection::listener()
{
  while (running) {
    if (!connect()) {
      return 1;
    }

    std::string   curMsg = "";
    while(running) {
      char cBuf;
      int len = recv(socket_fd, &cBuf, 1, 0);
      if (len > 0) {
        if (cBuf == '\n') {
          interpret(curMsg);
          curMsg = "";
        } else {
          curMsg += cBuf;
        }
      } else {
        if (len < 0 && errno == EINTR) {
          continue;
        }
        // some kind of error.  Reconnect.
        close(socket_fd);
        socket_fd = -1;
        break;
      }
    }
  }
  return 0;
}