/* sim_connection.cpp
 *
 * libpsx
 *
 * Copyright (C) 2015, Christopher Collins
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
*/
#include "psx.h"
#include <iostream>
#include <cstdio>
#ifndef _WIN32
// For Posix-like systems (Linux, OSX, most others)
#include <netinet/in.h> 
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define SOCKOPT_OPT_CAST(x)  (void*)(x)
#else
// for Windows
#include <winsock2.h>
#include <ws2tcpip.h>

// general windows strangeness
#define snprintf _snprintf

// winsock decided to arbitrarily not follow the posix API - fixup to work around that.
#define SHUT_RDWR SD_BOTH
#define SOCKOPT_OPT_CAST(x)  (char*)(x)
inline static int 
close(SOCKET s) {
  return closesocket(s); 
}
#endif

using namespace std;
using namespace psx;

SimConnection::SimConnection(const std::string &hostname, int port)
  : hostname(hostname), port(port)
{
#ifdef _WIN32
  WSADATA	wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
  msgMutex = SDL_CreateMutex();
}

SimConnection::~SimConnection()
{
  stopListener();
  if (NULL != msgMutex) {
    SDL_DestroyMutex(msgMutex);
    msgMutex = NULL;
  }
#ifdef _WIN32
  WSACleanup();
#endif
}

bool
SimConnection::connect()
{
  char    serviceName[16];
  snprintf(serviceName, 16, "%d", port);
  serviceName[15] = '\0';

  struct addrinfo addrHints;
  memset(&addrHints, 0, sizeof(addrHints));
  addrHints.ai_flags = AI_NUMERICSERV | AI_ADDRCONFIG;
  addrHints.ai_family = AF_INET;
  addrHints.ai_socktype = SOCK_STREAM;

  struct addrinfo *results = NULL;
  if (getaddrinfo(hostname.c_str(), serviceName, &addrHints, &results)) {
    return false;
  }

  socket_fd = socket(results->ai_family, results->ai_socktype, 0);
  if (-1 == socket_fd) {
    perror("connect: socket");
    return false;
  }
  int nodelay = 1;
  setsockopt(socket_fd, IPPROTO_TCP, TCP_NODELAY, SOCKOPT_OPT_CAST(&nodelay), sizeof(nodelay));
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
  ::close(socket_fd);
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
      cerr << "failed to connect" << endl;
      continue;
    }

    std::string   curMsg = "";
    while(running) {
      char cBuf;
      int len = recv(socket_fd, &cBuf, 1, 0);
      if (len > 0) {
        if (cBuf == '\n' || cBuf == '\r') {
		  if (curMsg.length() > 0) {
			interpret(curMsg);
		  }
          curMsg = "";
        } else {
          curMsg += cBuf;
        }
      } else {
        if (len < 0 && errno == EINTR) {
          continue;
        }
        // some kind of error.  Reconnect.
        ::close(socket_fd);
        socket_fd = -1;
        break;
      }
    }
  }
  return 0;
}

void
SimConnection::send(const std::string &msg)
{
  ::send(socket_fd, msg.c_str(), msg.length(), 0);
}

void
SimConnection::send(const WirePair &pair)
{
  std::string wireString = pair.toWire();
  wireString += "\n";

  send(wireString);
}
