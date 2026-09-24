#include <Arduino.h>

#define DE_RE       2     // RS485 DE/RE 제어 핀 (GPIO2)
#define SENSOR_PIN 34     // 아날로그 센서 입력 (GPIO34)

// 릴레이 핀 정의 (ESP32 사용 가능한 GPIO)
const int relayPins[8] = {4, 5, 18, 19, 21, 22, 23, 25};

uint8_t slaveID = 1;          // Modbus Slave ID
uint16_t holdingRegs[16];     // Holding Registers
bool coils[8];                // 릴레이 상태 저장

// -------------------------------
// CRC 계산 함수
// -------------------------------
uint16_t modbusCRC(uint8_t *buf, int len) {
  uint16_t crc = 0xFFFF;
  for (int pos = 0; pos < len; pos++) {
    crc ^= (uint16_t)buf[pos];
    for (int i = 0; i < 8; i++) {
      if (crc & 0x0001) {
        crc >>= 1;
        crc ^= 0xA001;
      } else {
        crc >>= 1;
      }
    }
  }
  return crc;
}

// -------------------------------
// 릴레이 제어 함수
// -------------------------------
void setRelay(int index, bool state) {
  if (index >= 0 && index < 8) {
    coils[index] = state;
    digitalWrite(relayPins[index], state ? LOW : HIGH); // Active Low
  }
}

// -------------------------------
// Modbus 응답 전송 함수
// -------------------------------
void sendResponse(uint8_t *response, int len) {
  digitalWrite(DE_RE, HIGH);   // 송신 모드
  Serial2.write(response, len);
  Serial2.flush();
  digitalWrite(DE_RE, LOW);    // 수신 모드
}

// -------------------------------
// Function Code 처리
// -------------------------------
void handleWriteSingleCoil(uint8_t *request) {
  uint16_t addr = (request[2] << 8) | request[3];
  uint16_t value = (request[4] << 8) | request[5];
  setRelay(addr, (value == 0xFF00));
  sendResponse(request, 8); // 요청 그대로 응답
}

void handleWriteMultipleCoils(uint8_t *request) {
  uint16_t addr = (request[2] << 8) | request[3];
  uint16_t count = (request[4] << 8) | request[5];
  for (int i = 0; i < count; i++) {
    bool state = (request[7 + i/8] >> (i % 8)) & 0x01;
    setRelay(addr+i, state);
  }
  uint8_t response[8];
  response[0] = slaveID;
  response[1] = 0x0F;
  response[2] = (addr >> 8) & 0xFF;
  response[3] = addr & 0xFF;
  response[4] = (count >> 8) & 0xFF;
  response[5] = count & 0xFF;
  uint16_t crc = modbusCRC(response, 6);
  response[6] = crc & 0xFF;
  response[7] = (crc >> 8) & 0xFF;
  sendResponse(response, 8);
}

void handleReadCoils(uint8_t *request) {
  uint16_t addr = (request[2] << 8) | request[3];
  uint16_t count = (request[4] << 8) | request[5];
  uint8_t byteCount = (count+7)/8;
  uint8_t response[3 + byteCount + 2];
  memset(response, 0, sizeof(response));
  response[0] = slaveID;
  response[1] = 0x01;
  response[2] = byteCount;
  for (int i = 0; i < count; i++) {
    if (addr+i < 8 && coils[addr+i]) {
      response[3 + i/8] |= (1 << (i % 8));
    }
  }
  uint16_t crc = modbusCRC(response, 3 + byteCount);
  response[3 + byteCount] = crc & 0xFF;
  response[4 + byteCount] = (crc >> 8) & 0xFF;
  sendResponse(response, 5 + byteCount);
}

void handleReadHoldingRegs(uint8_t *request) {
  uint16_t addr = (request[2] << 8) | request[3];
  uint16_t count = (request[4] << 8) | request[5];

  // 센서 값 업데이트
  holdingRegs[0] = analogRead(SENSOR_PIN);   // HR0: 아날로그 센서
  for (int i = 0; i < 8; i++) {
    holdingRegs[1+i] = coils[i] ? 1 : 0;     // HR1~HR8: 릴레이 상태
  }

  uint8_t response[3 + 2*count + 2];
  response[0] = slaveID;
  response[1] = 0x03;
  response[2] = count * 2;
  for (int i = 0; i < count; i++) {
    response[3 + i*2] = (holdingRegs[addr+i] >> 8) & 0xFF;
    response[4 + i*2] = holdingRegs[addr+i] & 0xFF;
  }
  uint16_t crc = modbusCRC(response, 3 + 2*count);
  response[3 + 2*count] = crc & 0xFF;
  response[4 + 2*count] = (crc >> 8) & 0xFF;
  sendResponse(response, 5 + 2*count);
}

// -------------------------------
// Setup & Loop
// -------------------------------
void setup() {
  pinMode(DE_RE, OUTPUT);
  digitalWrite(DE_RE, LOW); // 기본 수신 모드

  // 릴레이 초기화
  for (int i = 0; i < 8; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], HIGH); // OFF 상태
    coils[i] = false;
  }

  Serial2.begin(9600, SERIAL_8N1, 16, 17); // RS485
  Serial.begin(115200);  // 디버깅용
}

void loop() {
  if (Serial2.available() > 0) {
    uint8_t request[32];
    int len = Serial2.readBytes(request, sizeof(request));

    if (request[0] != slaveID) return; // Slave ID 확인
    uint8_t function = request[1];

    switch (function) {
      case 0x05: if (len >= 8) handleWriteSingleCoil(request); break;
      case 0x0F: if (len >= 9) handleWriteMultipleCoils(request); break;
      case 0x01: if (len >= 8) handleReadCoils(request); break;
      case 0x03: if (len >= 8) handleReadHoldingRegs(request); break;
      default: break; // 지원하지 않는 기능 코드
    }
  }
}
