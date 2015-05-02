#include "psx.h"

using namespace std;
using namespace psx;

SimConnectionSdl::SimConnectionSdl(const std::string &hostname, int port)
  : SimConnection(hostname, port)
{
  simEventType = SDL_RegisterEvents(1);
}

void
SimConnectionSdl::interpret(std::string message)
{
  SDL_Event event;
  SDL_zero(event);
  event.type = simEventType;
  event.user.code = PSX_ServerMessage;
  event.user.data1 = static_cast<void*>(new WirePair(message));
  event.user.data2 = NULL;
  SDL_PushEvent(&event);
}