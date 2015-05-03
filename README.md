# libpsx - A Simple Interface to Precision Simulator X

libpsx provides a simple interface to Precision Simulator X for use from C++.

libpsx uses SDL to provide portable threading support and is suitable for 
inclusion in a range of applications ranging from CDU emulators (which is 
what I originally wrote it for) to panel interfacing code.

# Platforms

All POSIX-likes should work out of the box.

There has been some consideration made for Windows using the VC compiler, 
but it currently remains untested.

# License & Copyright

This library is released under the GNU LGPL v2.1 - please see LICENSE.

Where not indicated in the source (except with the exception below), 
Copyright (C) 2015, Christopher Collins.

The header psx_codes.h is automatically dervied from documentation provided
with Precision Simulator X and on the Aerowinx Forums and no copyright is
claimed over its contents.

# Known Issues

*  `stopListener()` can block for a few seconds whilst the connection code is 
   spinning on a broken socket.

*  There's no reporting of connection state - either it's working or it's not.

*  No protection against race on the send socket.

*  There's little documentation as the source is currently very small.  This
   is also a feature. ;)

# Usage

Only include the `psx.h` header.  All relevant enums and classes will be in the
`psx` namespace.

Instantiate a subclass of `SimConnection`.  A subclass that injects messages into
the SDL event queue is included (as it's what I'm currently using).  You can 
subclass it yourself if you wish to use an alternate integration.

call `startListener()` which will start the connection thread and begin 
processing inbound events.

Once `startListener()` is called, you can use the `send` method to send 
messages to Precision Simulator.

the `interpret()` method will be called for each inbound message.

If you're using the provided `SimConnectionSdl`, it will register a user event
type and inject the messages into the SDL event queue.  The `WirePair` is on `data1`.

# Other Included Bits

A copy of `make_psx_codes.pl` is included which is the perl script used to 
generate the enums in psx_codes.h.
