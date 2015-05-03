/* sim_connection_sdl.cpp
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
