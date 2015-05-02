/* wire_pair.cpp
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
