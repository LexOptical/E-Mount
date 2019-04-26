#include "Config.h"
#include "Constants.h"
#include "DebugTools.h"
#include "Message.h"

const int PIN_LENS_CS_BODY = 2;
const int PIN_BODY_CS_LENS = 3;
const int PIN_BODY_VD_LENS = 4;

Message *message05 = new Message(MESSAGE_CLASS_NORMAL, 0, 0x05, norm05, sizeof(norm05));
Message *message06 = new Message(MESSAGE_CLASS_NORMAL, 0, 0x06, norm06, sizeof(norm06));

int bodyToLensBufferPosition = INVALID_POSITION;
int lensToBodyBufferPosition = INVALID_POSITION;
int packetLength = INVALID_POSITION;
byte bodyToLensBuffer[INPUT_BUFFER_SIZE] = {0};
byte lensToBodyBuffer[INPUT_BUFFER_SIZE] = {0};

byte message03target01 = 15, message03target02 = 15;  // AF positions

byte inited = LISTEN_ONLY ? 6 : 0;
int unusedClockWindows = 0;

void startMessage() {
    //wait for body_cs to go LOW
    while (digitalRead(PIN_BODY_CS_LENS)) {
        delayMicroseconds(1);
    }
    digitalWrite(PIN_LENS_CS_BODY, HIGH);
    printLenCS(true);
    delayMicroseconds(40);  //maybe 80
}

void finishMessage() {
    Serial1.flush();
    delayMicroseconds(40);
    digitalWrite(PIN_LENS_CS_BODY, LOW);
    printLenCS(false);
    unusedClockWindows = 0;  // We've sent something back
}

void setup() {
    Serial.begin(115200);

    Serial1.setRX(0);
    Serial1.setTX(LISTEN_ONLY ? 5 : 1);
    Serial1.begin(750000, SERIAL_8N1);

    Serial2.setRX(9);
    Serial2.begin(750000, SERIAL_8N1);

    pinMode(PIN_BODY_VD_LENS, INPUT);

    if(LISTEN_ONLY) {
        pinMode(PIN_LENS_CS_BODY, INPUT);
        attachInterrupt(digitalPinToInterrupt(PIN_LENS_CS_BODY), lenCsChange, CHANGE);
	} else {
	    pinMode(PIN_LENS_CS_BODY, OUTPUT);
        digitalWrite(PIN_LENS_CS_BODY, LOW);
	}
		
    pinMode(PIN_BODY_CS_LENS, INPUT);
    attachInterrupt(digitalPinToInterrupt(PIN_BODY_CS_LENS), bodyCsChange, CHANGE);

    pinMode(PIN_BODY_VD_LENS, INPUT);
    attachInterrupt(digitalPinToInterrupt(PIN_BODY_VD_LENS), bodyVdChange, CHANGE);
}

void bodyVdChange() {
    if(DEBUG_TIMING) {
        Serial.print(digitalRead(PIN_BODY_VD_LENS) ? "C " : "c ");
        Serial.println(micros());
    }
    if (!LISTEN_ONLY && inited != 0) {
        unusedClockWindows++;
        if (unusedClockWindows > 100) {
            Serial.print("RESETTING too many unusedClockWindows");
            inited = 0;  // RESET we've got into a bad state
            unusedClockWindows = 0;
        }
    }
}

void lenCsChange() {
    printLenCS(digitalRead(PIN_LENS_CS_BODY));
}

void printLenCS(bool val) {
    if (inited < 2 || DEBUG_TIMING) {
        Serial.print(val ? "[L:" : "[l:");
        Serial.print(micros());
        Serial.println("]");
    }
}

void bodyCsChange() {
    if (inited < 2 || DEBUG_TIMING) {
        Serial.print(digitalRead(PIN_BODY_CS_LENS) ? "[B:" : "[b:");
        Serial.print(micros());
        Serial.println("]");
    }
    if (inited == 0) {
        delayMicroseconds(990);
        digitalWrite(PIN_LENS_CS_BODY, HIGH);
        printLenCS(true);
        inited++;
    } else if (inited == 1) {
        delayMicroseconds(305);
        digitalWrite(PIN_LENS_CS_BODY, LOW);
        printLenCS(false);
        inited++;
    }
}

void loop() {
    // put your main code here, to run repeatedly

    if (LISTEN_ONLY && Serial1.available() > 0) {
        processByte(Serial1.read(), lensToBodyBuffer, lensToBodyBufferPosition, lensToBody);
    }

    if (Serial2.available() > 0) {
        processByte(Serial2.read(), bodyToLensBuffer, bodyToLensBufferPosition, bodyToLens);
    }
}

int aperture = 0;

void processMessage(Message *input) {
    if (LISTEN_ONLY) {
        return;
    }

    switch (input->messageType) {
        case 0x03:
            if (input->bodyLength > 22) {
                message05->body[77] = input->body[21];
                message05->body[78] = input->body[22];
            }
            break;
        case 0x04:
            // Send both 0x05 and 0x06 back
            startMessage();
            message05->sequenceNumber = input->sequenceNumber + 1;
            aperture += 1;
            aperture %= 0xFF;
            message05->body[30] = aperture & 0xFF;  // 0xAB;  // change aperture
            message05->body[31] = aperture >> 8;
            message05->prepForSending();

            writeSerial1Debuggable(message05->outputBuffer, message05->wireLength);
            finishMessage();
            flushDebugOutputBuffer();

            delayMicroseconds(10);

            startMessage();
            message06->sequenceNumber = input->sequenceNumber + 1;
            message06->prepForSending();
            writeSerial1Debuggable(message06->outputBuffer, message06->wireLength);
            finishMessage();
            break;

            //INIT MESSAGES
        case 0x01:
            startMessage();
            //delayMicroseconds(520); //not needed
            writeSerial1Debuggable(init01, sizeof(init01));
            finishMessage();
            break;
        case 0x07:
            startMessage();
            writeSerial1Debuggable(init07, sizeof(init07));
            finishMessage();
            break;
        case 0x08:
            startMessage();
            writeSerial1Debuggable(init08, sizeof(init08));
            finishMessage();
            break;
        case 0x09:
            startMessage();
            writeSerial1Debuggable(init09, sizeof(init09));
            finishMessage();
            break;
        case 0x0A:
            startMessage();
            writeSerial1Debuggable(init0A, sizeof(init0A));
            finishMessage();
            break;
        case 0x0B:
            startMessage();
            writeSerial1Debuggable(init0B, sizeof(init0B));
            finishMessage();
            break;
        case 0x0D:
            startMessage();
            writeSerial1Debuggable(init0D, sizeof(init0D));
            finishMessage();
            break;
        case 0x10:
            startMessage();
            writeSerial1Debuggable(init10, sizeof(init10));
            finishMessage();
            break;
    }
}

void processByte(int read, byte *buffer, int &position, int direction) {
    if (position == INVALID_POSITION) {
        if (read == START_BYTE) {
            position = 0;
            //Serial.print("FB ");  //first byte
            //Serial.println(micros());
        } else {
            return;
        }
    }

    buffer[position] = read;
    position++;
    if (position >= INPUT_BUFFER_SIZE) {
        //ERROR
        position = INVALID_POSITION;
    }
    if (position == 2) {
        packetLength = (buffer[2] << 8) + buffer[1];
    }
    if (packetLength == position) {
        if (read == END_BYTE) {
            Message *message = new Message(buffer, position);
            processMessage(message);
            delete message;

            if (direction == lensToBody) {
                Serial.print("Lens->Body ");
            } else {
                Serial.print("Body->Lens ");
            }

            //Serial.print(message->messageType, HEX);
            printHexBuffer(buffer, position);
            Serial.print(' ');
            Serial.println(micros());

            flushDebugOutputBuffer();

            position = INVALID_POSITION;
        } else {
            //Error
            position = INVALID_POSITION;
        }
    }
}

/*
void parseMessage(byte *buffer, int length)
{
	message recieved;
	recieved.length = length-9;
	recieved.messageClass = buffer[3];
	recieved.sequenceNumber = buffer[4];
	recieved.messageType = buffer[5];
	recieved.body = buffer+6;
	recieved.checksum = (buffer[length -2] << 8) + buffer[length -3];

	printHexBufferForHumans(recieved.body, recieved.length);
	return recieved;
}
*/

/*
void sendMessage(byte portNum, message *message)
{
	Serial1.write(START_BYTE);
	Serial1.write(0xFF & message.);
	Serial1.write(END_BYTE);
}*/
/*
  if(Serial2.available() > 0){
    int read = Serial2.read();
    if(read == 0xF0){
      Serial.print("Body->Lens: ");
      Serial.println(micros());
    }
    Serial.println(read,HEX);
  }
  */

/*
// Sequence diagram generator
  if(Serial1.available() > 0){
    int read = Serial1.read();
    if(read == 0xF0){
      s1Count = 5;
      }
    if(s1Count == 0){
      Serial.print("Lens->Body: ");
      Serial.println(read,HEX);
    }
    s1Count--;
  }

   if(Serial2.available() > 0){
    int read = Serial2.read();
    if(read == 0xF0){
      s2Count = 5;
      }
    if(s2Count == 0){
      Serial.print("Body->Lens: ");
      Serial.println(read,HEX);
    }
    s2Count--;
  }
  */
