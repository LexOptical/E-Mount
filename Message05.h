#ifndef Message05_h
#define Message05_h

#include "Arduino.h"
#include "Message.h"

class Message05 : public Message {
   public:
    Message05(byte *bufferFromWire, int wireLength);
    Message05(byte messageClass, int sequenceNumber, byte messageType, const byte *body, int bodyLength);
    
    void setAperture(int value);
    void updateBasedOn03(Message *input); 
    
    int aperture;
};

#endif
