/* wire_pair.cpp
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
#include <string>

using namespace std;
using namespace psx;

WirePair::WirePair()
{
}

WirePair::WirePair(const WirePair &src) 
  : key(src.key), value(src.value)
{
}

WirePair::WirePair(const std::string &fromWire) {
  size_t offs = fromWire.find_first_of('=');
  if (string::npos == offs) {
    value = fromWire;
  } else {
    key = fromWire.substr(0, offs);
    value = fromWire.substr(offs+1);
  }
}

std::string
WirePair::toWire() const {
  std::string out = value;

  if (key != "") {
    out = key + "=" + value;
  }

  return out;
}

void
WirePair::setKey(Qh newKey)
{
  key = "Qh" + to_string(static_cast<int>(newKey));
}

void
WirePair::setKey(Qs newKey)
{
  key = "Qs" + to_string(static_cast<int>(newKey));
}

void
WirePair::setKey(Qi newKey)
{
  key = "Qi" + to_string(static_cast<int>(newKey));
}
