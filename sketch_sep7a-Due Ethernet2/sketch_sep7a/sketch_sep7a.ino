#include <Arduino.h>
#include <SPI.h>
#include <Ethernet2.h>

// -----------------------------
// 네트워크 설정
// -----------------------------
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(172, 30, 1, 177);   // 고정 IP
EthernetServer server(502);      // Modbus TCP 기본 포트

// -----------------------------
// 릴레이 설정 (DUE 핀)
// -----------------------------
// 릴레이 0~8 → 총 9개
const int relayPins[9] = {5,6,7,8,15,16,17,18,19};
uint16_t coilMap = 0;             // 릴레이 상태 비트맵 (최대 16비트 사용)
uint16_t holdingRegs[16];         // HR0~HR15

// -----------------------------
// 릴레이 제어 함수
// -----------------------------
void setRelay(int idx, bool state) {
  if (idx < 0 || idx >= 9) return; // 안전 체크

  if (state) bitSet(coilMap, idx);
  else bitClear(coilMap, idx);

  // Active Low 릴레이 → LOW = ON, HIGH = OFF
  digitalWrite(relayPins[idx], state ? LOW : HIGH);
  holdingRegs[idx] = state ? 1 : 0;

  Serial.print("Relay ");
  Serial.print(idx);
  Serial.print(" -> ");
  Serial.println(state ? "ON" : "OFF");
}

// -----------------------------
// Modbus 요청 처리 함수
// -----------------------------
void handleModbus(EthernetClient &client, uint8_t *request, int len) {
  if (len < 8) return;

  uint8_t unitID = request[6];
  uint8_t function = request[7];

  switch (function) {
    case 0x05: { // Write Single Coil
      uint16_t addr = (request[8] << 8) | request[9];
      uint16_t value = (request[10] << 8) | request[11];
      setRelay(addr, (value == 0xFF00));
      client.write(request, 12);
      break;
    }

    case 0x0F: { // Write Multiple Coils
      uint16_t addr = (request[8] << 8) | request[9];
      uint16_t count = (request[10] << 8) | request[11];
      uint8_t byteCount = request[12];

      for (int i = 0; i < count; i++) {
        bool state = (request[13 + i/8] >> (i % 8)) & 0x01;
        setRelay(addr+i, state);
      }

      uint8_t response[12];
      memcpy(response, request, 12);
      client.write(response, 12);
      break;
    }

    case 0x01: { // Read Coils
      uint16_t addr = (request[8] << 8) | request[9];
      uint16_t count = (request[10] << 8) | request[11];
      uint8_t byteCount = (count+7)/8;
      uint8_t response[9 + byteCount];

      response[0] = request[0];
      response[1] = request[1];
      response[2] = 0;
      response[3] = 0;
      response[4] = 0;
      response[5] = 3 + byteCount;
      response[6] = unitID;
      response[7] = 0x01;
      response[8] = byteCount;
      memset(&response[9], 0, byteCount);

      for (int i = 0; i < count; i++) {
        if (bitRead(coilMap, addr+i)) {
          response[9 + i/8] |= (1 << (i % 8));
        }
      }
      client.write(response, 9 + byteCount);
      break;
    }

    case 0x03: { // Read Holding Registers
      uint16_t addr = (request[8] << 8) | request[9];
      uint16_t count = (request[10] << 8) | request[11];

      // HR0~HR8: 릴레이 상태 반영
      for (int i = 0; i < 9; i++) {
        holdingRegs[i] = bitRead(coilMap, i) ? 1 : 0;
      }

      uint8_t response[9 + 2*count];
      response[0] = request[0];
      response[1] = request[1];
      response[2] = 0;
      response[3] = 0;
      response[4] = 0;
      response[5] = 3 + 2*count;
      response[6] = unitID;
      response[7] = 0x03;
      response[8] = count*2;

      for (int i = 0; i < count; i++) {
        response[9 + i*2] = (holdingRegs[addr+i] >> 8) & 0xFF;
        response[10 + i*2] = holdingRegs[addr+i] & 0xFF;
      }
      client.write(response, 9 + 2*count);
      break;
    }
  }
}

// -----------------------------
// Setup
// -----------------------------
void setup() {
  Serial.begin(9600);

  // 릴레이 초기화
  for (int i = 0; i < 9; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], HIGH); // OFF 상태
    bitClear(coilMap, i);
    holdingRegs[i] = 0;
  }

  // Ethernet 초기화
  Ethernet.begin(mac, ip);
  server.begin();

  Serial.print("Modbus TCP Slave 시작, IP: ");
  Serial.println(Ethernet.localIP());
}

// -----------------------------
// Loop
// -----------------------------
void loop() {
  EthernetClient client = server.available();
  if (client && client.available() > 0) {
    uint8_t request[64];
    int len = client.read(request, sizeof(request));
    handleModbus(client, request, len);
  }
}