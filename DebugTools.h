#ifndef DebugTools_h
#define DebugTools_h

byte debugOutputBuffer[INPUT_BUFFER_SIZE] = {0};
int debugOutputBufferPosition = 0;

byte debugInputBuffer[INPUT_BUFFER_SIZE] = {0};
int debugInputBufferPosition = 0;

void printHexBuffer(byte* buffer, int length) {
    char tmp[2];
    for (int i = 0; i < length; i++) {
        sprintf(tmp, "%.2X", buffer[i]);
        Serial.print(tmp);
    }
}

void writeSerial1Debuggable(byte b) {
    Serial1.write(b);
    if (DEBUG) {
        if (debugOutputBufferPosition < INPUT_BUFFER_SIZE) {
            debugOutputBuffer[debugOutputBufferPosition++] = b;
        }
    }
}

void writeSerial1Debuggable(byte* buffer, int size) {
    Serial1.write(buffer, size);
    if (DEBUG) {
        memcpy(&debugOutputBuffer[debugOutputBufferPosition], buffer, size);
        debugOutputBufferPosition += size;
    }
}

void writeSerial1Debuggable(const byte* buffer, int size) {
    writeSerial1Debuggable((byte*)buffer, size);
}

void flushDebugOutputBuffer() {
    if (DEBUG) {
        Serial.print("Us->Body ");
        if (debugInputBufferPosition != 0) {
            printHexBuffer(debugOutputBuffer, debugOutputBufferPosition);
        } else {
            Serial.print("no message");  
        }
        debugOutputBufferPosition = 0;
        Serial.println();
    }
}

#endif
