#include "Config.h"

#include "Constants.h"
#include "DebugTools.h"
#include "Message.h"

const int PIN_LENS_CS_BODY = LISTEN_ONLY ? 6 : 2;
const int PIN_BODY_CS_LENS = 3;
const int PIN_BODY_VD_LENS = 4;

const int PIN_LENS_CS_BODY_IN = LISTEN_ONLY ? 2 : 6;


Message *message05 = new Message(MESSAGE_CLASS_NORMAL, 0, 0x05, norm05, sizeof(norm05));
Message *message06 = new Message(MESSAGE_CLASS_NORMAL, 0, 0x06, norm06, sizeof(norm06));


int bodyToLensBufferPosition = INVALID_POSITION;
int lensToBodyBufferPosition = INVALID_POSITION;
int packetLength = INVALID_POSITION;
byte bodyToLensBuffer[INPUT_BUFFER_SIZE] = {0};
byte lensToBodyBuffer[INPUT_BUFFER_SIZE] = {0};

byte message03target01 = 15, message03target02 = 15;

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
}

void setup() {
    Serial.begin(115200);

    Serial1.setRX(0);
    Serial1.setTX(LISTEN_ONLY ? 5 : 1);
    Serial1.begin(750000, SERIAL_8N1);

    Serial2.setRX(9);
    Serial2.begin(750000, SERIAL_8N1);

    pinMode(PIN_BODY_VD_LENS, INPUT);

    pinMode(PIN_LENS_CS_BODY, OUTPUT);
    digitalWrite(PIN_LENS_CS_BODY, LOW);

    pinMode(PIN_LENS_CS_BODY_IN, INPUT);
    attachInterrupt(digitalPinToInterrupt(PIN_LENS_CS_BODY_IN), lenCsChange, CHANGE);

    pinMode(PIN_BODY_CS_LENS, INPUT);
    attachInterrupt(digitalPinToInterrupt(PIN_BODY_CS_LENS), bodyCsChange, CHANGE);

    pinMode(PIN_BODY_VD_LENS, INPUT);
    attachInterrupt(digitalPinToInterrupt(PIN_BODY_VD_LENS), bodyVdChange, CHANGE);
}

void bodyVdChange() {
    Serial.print(digitalRead(PIN_BODY_VD_LENS) ? "C " : "c ");
    Serial.println(micros());
}

void lenCsChange() {
    printLenCS(digitalRead(PIN_LENS_CS_BODY_IN));
}

void printLenCS(bool val) {
    Serial.print(val ? "[L:" : "[l:");
    Serial.print(micros());
    Serial.println("]");
}

byte inited = LISTEN_ONLY ? 6 : 0;

void bodyCsChange() {
    // if (inited  || inited == 6) {
    Serial.print(digitalRead(PIN_BODY_CS_LENS) ? "[B:" : "[b:");
    Serial.print(micros());
    Serial.println("]");
    //	}
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
    } /*else if (inited == 2) {
		inited++;
	} else if (inited == 3) {
		digitalWrite(PIN_LENS_CS_BODY, HIGH);
		printLenCS(true);
		delayMicroseconds(163);
		digitalWrite(PIN_LENS_CS_BODY, LOW);
		printLenCS(false);
		inited++;
	}*/
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

void processMessage(Message *input) {
    if (LISTEN_ONLY) {
        return;
    }

    switch (input->messageType) {
        case 0x03:
            if (input->bodyLength > 22) {
                message05->body[77] = input->body[21];
                message05->body[78] = input->body[22];
                //Serial.print("targets: ");
                //Serial.print(message03target01, HEX);
                //Serial.print(message03target02, HEX);
            }
            break;
        case 0x04:
            // send a message back
            // Message* message05 = new Message(MESSAGE_CLASS_NORMAL,input->sequenceNumber+1, 0x05, norm05, sizeof(norm05));
            startMessage();
            message05->sequenceNumber = input->sequenceNumber + 1;
            message05->prepForSending();
            //message05->body[30] = (message05->body[30] + 1) % 0xAB;
            for (int i = 0; i < message05->wireLength; i++) {
                writeSerial1Debuggable(message05->outputBuffer[i]);
            }
            finishMessage();

            delayMicroseconds(10);
            
            startMessage();
            message06->sequenceNumber = input->sequenceNumber + 1;
            message06->prepForSending();
            for (int i = 0; i < message06->wireLength; i++) {
                writeSerial1Debuggable(message06->outputBuffer[i]);
            }
            finishMessage();
            break;

            //INIT MESSAGES
        case 0x01:
            startMessage();
            //delayMicroseconds(520);
            for (uint i = 0; i < sizeof(init01); i++) {
                writeSerial1Debuggable(init01[i]);
            }
            finishMessage();
            break;
        case 0x07:
            startMessage();
            // delayMicroseconds(520);
            for (uint i = 0; i < sizeof(init07); i++) {
                writeSerial1Debuggable(init07[i]);
            }
            finishMessage();
            break;
        case 0x08:
            startMessage();
            // delayMicroseconds(520);
            for (uint i = 0; i < sizeof(init08); i++) {
                writeSerial1Debuggable(init08[i]);
            }
            finishMessage();
            break;
        case 0x09:
            startMessage();
            //delayMicroseconds(520);
            for (uint i = 0; i < sizeof(init09); i++) {
                writeSerial1Debuggable(init09[i]);
            }
            finishMessage();
            break;
        case 0x0A:
            startMessage();
            //  delayMicroseconds(520);
            for (uint i = 0; i < sizeof(init0A); i++) {
                writeSerial1Debuggable(init0A[i]);
            }
            finishMessage();
            break;
        case 0x0B:
            startMessage();
            // delayMicroseconds(520);
            for (uint i = 0; i < sizeof(init0B); i++) {
                writeSerial1Debuggable(init0B[i]);
            }
            finishMessage();
            break;
        case 0x0D:
            startMessage();
            //  delayMicroseconds(520);
            for (uint i = 0; i < sizeof(init0D); i++) {
                writeSerial1Debuggable(init0D[i]);
            }
            finishMessage();
            break;
        case 0x10:
            startMessage();
            //   delayMicroseconds(520);
            for (uint i = 0; i < sizeof(init10); i++) {
                writeSerial1Debuggable(init10[i]);
            }
            finishMessage();
            break;
    }
}

void confirmReceipt() {
    //digitalWrite(PIN_LENS_CS_BODY, HIGH);
    //delayMicroseconds(300);
    //digitalWrite(PIN_LENS_CS_BODY, LOW);
}

void processByte(int read, byte *buffer, int &position, int direction) {
    if (position == INVALID_POSITION) {
        if (read == START_BYTE) {
            position = 0;
            Serial.print("FB ");  //first byte
            Serial.println(micros());
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
            //confirmReceipt();

            if (direction == lensToBody) {
                Serial.print("Lens->Body ");
            } else {
                Serial.print("Body->Lens ");
            }
            

            Message *message = new Message(buffer, position);

            Serial.print(message->messageType, HEX);
            Serial.print(' ');
            Serial.println(micros());

            processMessage(message);
            delete message;

            printHexBuffer(buffer, position);
            Serial.println();

            Serial.print(' ');
            Serial.println(micros());

            

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
