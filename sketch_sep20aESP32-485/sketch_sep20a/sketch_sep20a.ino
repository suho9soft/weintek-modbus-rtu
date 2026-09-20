#include <Arduino.h>

#define DE_RE  2          // RS485 DE/RE 제어 핀 (GPIO2)
#define SENSOR_PIN 34     // 아날로그 센서 입력 (GPIO34)

// 릴레이 핀 정의 (ESP32 사용 가능한 GPIO)
const int relayPins[8] = {4, 5, 18, 19, 21, 22, 23, 25};

uint8_t slaveID = 1;          // Modbus Slave ID
uint16_t holdingRegs[16];     // Holding Registers
bool coils[8];                // 릴레이 상태 저장

// CRC 계산 함수
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

void setup() {
  pinMode(DE_RE, OUTPUT);
  digitalWrite(DE_RE, LOW); // 수신 모드

  // 릴레이 초기화 (Active Low → 기본 OFF 상태 = HIGH)
  for (int i = 0; i < 8; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], HIGH);
    coils[i] = false;
  }

  // ESP32는 Serial2를 RS485용으로 사용 (RX=16, TX=17 예시)
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  Serial.begin(115200);  // 디버깅용 시리얼 모니터
}

void loop() {
  if (Serial2.available() > 0) {
    uint8_t request[32];
    int len = Serial2.readBytes(request, sizeof(request));

    if (request[0] != slaveID) return; // Slave ID 확인
    uint8_t function = request[1];

    // -------------------------------
    // Function Code 0x05: Write Single Coil
    // -------------------------------
    if (function == 0x05 && len >= 8) {
      uint16_t addr = (request[2] << 8) | request[3];
      uint16_t value = (request[4] << 8) | request[5];
      if (addr < 8) {
        coils[addr] = (value == 0xFF00);
        digitalWrite(relayPins[addr], coils[addr] ? LOW : HIGH);
      }
      digitalWrite(DE_RE, HIGH);
      Serial2.write(request, 8); // 요청 그대로 응답
      Serial2.flush();
      digitalWrite(DE_RE, LOW);
    }

    // -------------------------------
    // Function Code 0x0F: Write Multiple Coils
    // -------------------------------
    else if (function == 0x0F && len >= 9) {
      uint16_t addr = (request[2] << 8) | request[3];
      uint16_t count = (request[4] << 8) | request[5];
      for (int i = 0; i < count; i++) {
        bool state = (request[7 + i/8] >> (i % 8)) & 0x01;
        if (addr+i < 8) {
          coils[addr+i] = state;
          digitalWrite(relayPins[addr+i], state ? LOW : HIGH);
        }
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
      digitalWrite(DE_RE, HIGH);
      Serial2.write(response, 8);
      Serial2.flush();
      digitalWrite(DE_RE, LOW);
    }

    // -------------------------------
    // Function Code 0x01: Read Coils
    // -------------------------------
    else if (function == 0x01 && len >= 8) {
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
      digitalWrite(DE_RE, HIGH);
      Serial2.write(response, 5 + byteCount);
      Serial2.flush();
      digitalWrite(DE_RE, LOW);
    }

    // -------------------------------
    // Function Code 0x03: Read Holding Registers
    // -------------------------------
    else if (function == 0x03 && len >= 8) {
      uint16_t addr = (request[2] << 8) | request[3];
      uint16_t count = (request[4] << 8) | request[5];

      // 센서 값 읽기 (아날로그 센서)
      holdingRegs[0] = analogRead(SENSOR_PIN);   // HR0: 아날로그 센서
      for (int i = 0; i < 8; i++) {
        holdingRegs[1+i] = coils[i] ? 1 : 0; // HR1~HR8: 릴레이 상태
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
      digitalWrite(DE_RE, HIGH);
      Serial2.write(response, 5 + 2*count);
      Serial2.flush();
      digitalWrite(DE_RE, LOW);
    }
  }
}
