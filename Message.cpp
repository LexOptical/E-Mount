#include "Arduino.h"
#include "Message.h"


void printHexBufferForHumans(byte *buffer, int length) {
    char tmp[2];
    for (int i = 0; i < length; i++) {
        sprintf(tmp, "%.2X", buffer[i]);
        Serial.print(tmp);
    }
}

Message::Message(byte *bufferFromWire, int wireLength) {
    this->wireLength = wireLength;
    bodyLength = wireLength - 9;
    messageClass = bufferFromWire[3];
    sequenceNumber = bufferFromWire[4];
    messageType = bufferFromWire[5];
    body = bufferFromWire + 6;
    checksum = (bufferFromWire[wireLength - 2] << 8) + bufferFromWire[wireLength - 3];
}


Message::Message(byte messageClass, int sequenceNumber, byte messageType, const byte *body, int bodyLength) {
    wireLength = bodyLength + 9;
    this->bodyLength = bodyLength;
    this->messageClass = messageClass;
    this->sequenceNumber = sequenceNumber;
    this->messageType = messageType;
    this->body = new byte[bodyLength];
    memcpy ( this->body, body, bodyLength);
    checksum = 0;
}

void Message::print() {
    printHexBufferForHumans(this->outputBuffer, this->wireLength);
}

void Message::prepForSending() {
    int position = 0;
    outputBuffer[position++] = START_BYTE;
    outputBuffer[position++] = wireLength & 0xFF;
    outputBuffer[position++] = wireLength >> 8;
    outputBuffer[position++] = messageClass;
    outputBuffer[position++] = (messageClass == MESSAGE_CLASS_NORMAL ? sequenceNumber : 0);
    outputBuffer[position++] = messageType;
    for (int i = 0; i < bodyLength; i++) {
        outputBuffer[position++] = body[i];
    }

    int checksum = 0;
    for (int i = 1; i < position; i++) {
        checksum += outputBuffer[i];
    }
    outputBuffer[position++] = checksum & 0xFF;
    outputBuffer[position++] = checksum >> 8;
    outputBuffer[position++] = END_BYTE;

   // printHexBufferForHumans(outputBuffer, position);
}
