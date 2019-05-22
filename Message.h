#ifndef Message_h
#define Message_h

#include "Arduino.h"
#include "Constants.h"

class Message {
   public:
    Message(byte *bufferFromWire, int wireLength);
    Message(byte messageClass, int sequenceNumber, byte messageType, const byte *body, int bodyLength);

    void print();
    void prepForSending();
   
    int bodyLength;
    int wireLength;
    byte messageClass;
    byte sequenceNumber;
    byte messageType;
    byte *body;
    byte outputBuffer[INPUT_BUFFER_SIZE] = {0};

   private:
    int checksum;
};

#endif
