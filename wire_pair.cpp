/* wire_pair.cpp
*/

#include "psx.h"
#include <string>

using namespace std;
using namespace psx;

WirePair::WirePair()
{
}

WirePair::WirePair(const std::string &fromWire) {
  size_t offs = fromWire.find_first_of('=');
  if (string::npos == offs) {
    value = fromWire;
  } else {
    key = fromWire.substr(0, offs-1);
    value = fromWire.substr(offs+1, string::npos);
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
