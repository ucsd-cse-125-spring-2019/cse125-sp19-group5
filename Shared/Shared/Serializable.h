#pragma	once

#include "NetBuffer.h"

class Serializable {
	virtual void serialize(NetBuffer &buffer) const = 0;
};