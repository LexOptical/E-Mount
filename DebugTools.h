#ifndef DebugTools_h
#define DebugTools_h

void printHexBuffer(byte *buffer, int length) {
    char tmp[2];
    for (int i = 0; i < length; i++) {
        sprintf(tmp, "%.2X", buffer[i]);
        Serial.print(tmp);
    }
}

void writeSerial1Debuggable(byte b) {
    Serial1.write(b);
    if (DEBUG) {
        Serial.print(b, HEX);
    }
}

#endif
