#pragma	once

#include "NetBuffer.h"

// A Serializable object is one that can be written into a NetBuffer and read
// from a NetBuffer.
class Serializable {
	// Writes a representation of the object into the given buffer.
	virtual void serialize(NetBuffer &buffer) const = 0;

	// Assuming the first thing in the given buffer is the serialized version of
	// this type of object, reads the data from the buffer and loads it into
	// the object.
	virtual void deserialize(NetBuffer &buffer) = 0;
};