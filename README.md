# Modbus Relay Controller (Arduino RS485)

이 프로젝트는 **Arduino 기반 RS485(Modbus RTU) 릴레이 제어기**입니다.  
아날로그 센서 입력과 8채널 릴레이를 Modbus 프로토콜을 통해 제어할 수 있습니다.

---

## ✨ 주요 기능
- **Modbus Slave ID = 1**
- 지원 기능 코드:
  - `0x01` : Read Coils (릴레이 상태 읽기)
  - `0x03` : Read Holding Registers (센서 값 및 릴레이 상태 읽기)
  - `0x05` : Write Single Coil (단일 릴레이 제어)
  - `0x0F` : Write Multiple Coils (다중 릴레이 제어)
- **릴레이 8채널 제어 (D4~D12, D9 제외)**
- **아날로그 센서 입력 (A0)**

---

## ⚙️ 하드웨어 연결
- **RS485 모듈 (MAX485)**
  - DE/RE → D2
  - DI → TX1
  - RO → RX1
- **릴레이 모듈**
  - D4, D5, D6, D7, D8, D10, D11, D12 → 릴레이 입력
- **센서**
  - A0 → 아날로그 센서 입력

---

## 📖 Holding Registers 정의
| 주소 | 설명 |
|------|------|
| HR0  | 아날로그 센서 값 (0~1023) |
| HR1~HR8 | 릴레이 상태 (0=OFF, 1=ON) |

---

## 🖥️ 사용 방법
1. 아두이노에 코드를 업로드합니다.
2. RS485 통신을 통해 Modbus Master에서 제어합니다.
3. 예시:
   - Coil 0 ON → Function 0x05, Addr=0, Value=0xFF00
   - Read Holding Registers → Function 0x03, Addr=0, Count=9

---

## 📜 라이선스
MIT License
