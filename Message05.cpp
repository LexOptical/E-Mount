#include "Arduino.h"
#include "Message05.h"

Message05::Message05(byte messageClass, int sequenceNumber, byte messageType, const byte *body, int bodyLength): Message(messageClass, sequenceNumber, messageType, body, bodyLength) {
}

Message05::Message05(byte *bufferFromWire, int wireLength): Message(bufferFromWire, wireLength) {
}

void Message05::setAperture(int value) {
    aperture = value;
    aperture %= 0xAB; // Enforce an arbirary max value for testing

    //Set the bytes in our buffer
    body[30] = aperture & 0xFF;
    body[31] = aperture >> 8;
}

void Message05::updateBasedOn03(Message *input) {
  if (input->bodyLength > 22) {
    body[77] = input->body[21];
    body[78] = input->body[22];
  }
}
