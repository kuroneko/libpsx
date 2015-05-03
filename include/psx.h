/* psx.h
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
#include <string>
#include <SDL.h>

#ifndef _PSX_H
#define _PSX_H

namespace psx {
	#include "psx_codes.h"

	class WirePair {
	public:
		WirePair();
		WirePair(const WirePair &src);
		WirePair(const std::string &fromWire);

		std::string   key;
		std::string   value;

		void          setKey(Qh key);
		void          setKey(Qs key);
		void          setKey(Qi key);
		std::string   toWire() const;
	};

	class SimConnection {
	public:
		std::string		hostname;
		int 			port;

		int             base_retry_interval =  500;
		int             max_retry_interval = 10000;

		SimConnection(const std::string &hostname, int port = 10747);
		~SimConnection();

		void stopListener();
		void startListener();
		void send(const WirePair &pair);

		// do not invoke - this is started in its own thread, but needs to b
		// public as its invoked by a C method.
		int listener();
	protected:
		int     		socket_fd = -1;
		bool    		running = false;
		SDL_Thread  	*rcvThread = NULL;
		SDL_mutex   	*msgMutex = NULL;

		void			send(const std::string &message);
		virtual void    interpret(std::string message) = 0;
		bool 			connect();
		void 			disconnect();
	};  

	enum PSX_EventCode {
		PSX_Connect,
		PSX_Disconnect,
		PSX_ServerMessage,
	};

	// inject connection events into SDL
	class SimConnectionSdl : public SimConnection {
	public:
		SimConnectionSdl(const std::string &hostname, int port = 10747);

		Uint32      simEventType;
	protected:
		virtual void        interpret(std::string message);
	};
};

#endif