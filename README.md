# Arduino UNO R4 Minima - Modbus RTU Relay Controller

이 프로젝트는 **Arduino UNO R4 Minima** 보드를 RS485(Modbus RTU) Slave로 동작시켜  
EasyBuilder Pro HMI와 연동하여 릴레이 및 센서를 제어하는 예제입니다.

---

## 주요 기능
- Modbus Slave ID = 1
- 지원 기능 코드:
  - 0x01 : Read Coils
  - 0x03 : Read Holding Registers
  - 0x05 : Write Single Coil
  - 0x0F : Write Multiple Coils
- 릴레이 8채널 제어 (D4~D12, D9 제외)
- 아날로그 센서 입력 (A0)

---

## 하드웨어 연결
- RS485 모듈 (MAX485)
  - DE/RE → D2
  - DI → TX1
  - RO → RX1
- 릴레이 모듈 → D4~D12
- 센서 → A0

---

## EasyBuilder Pro 연동
- 장치: Modbus RTU Slave
- Slave ID: 1
- Baudrate: 9600
- 태그 매핑:
  - Coil 0~7 → 릴레이 제어
  - HR0 → 센서 값
  - HR1~HR8 → 릴레이 상태 표시
