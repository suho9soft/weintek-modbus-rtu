📂 프로젝트 구조
/Arduino-ModbusTCP-RTU-Gateway
 ├── src/
 │    └── main.ino        // 아두이노 코드
 ├── docs/
 │    └── wiring.png      // 핀 연결 다이어그램 (선택)
 ├── README.md            // 프로젝트 설명
 └── LICENSE              // 라이선스 (예: MIT)
# Arduino Modbus TCP ↔ RTU Gateway

이 프로젝트는 **Arduino Mega 2560 + Ethernet Shield 2(W5500) + MAX485**를 이용해  
Modbus TCP 클라이언트(예: Weintek HMI, SCADA, PLC)와 RS485(Modbus RTU) 장치를 연결하는 게이트웨이입니다.

---

## ⚙️ 하드웨어 구성
- Arduino Mega 2560
- Ethernet Shield 2 (W5500)
- MAX485 RS485 모듈
- 릴레이 모듈 (D4, D5, D6, D7, D8, D10, D11, D12 핀 연결)
- 아날로그 센서 (A0 입력)

---

## 📡 네트워크 설정
- Arduino IP: `172.30.1.177`
- Port: `502` (Modbus TCP 기본 포트)
- Slave ID: `1`

---

## 🔌 핀 연결
- RS485 DE/RE → D2
- RS485 TX → D18 (TX1)
- RS485 RX → D19 (RX1)
- 릴레이 → D4, D5, D6, D7, D8, D10, D11, D12
- 아날로그 센서 → A0

---

## 🖥️ 지원 기능
- **Function Code 0x05**: 단일 Coil 제어 (릴레이 ON/OFF)
- **Function Code 0x0F**: 다중 Coil 제어
- **Function Code 0x01**: Coil 상태 읽기
- **Function Code 0x03**: Holding Register 읽기
  - HR0: 아날로그 센서 값
  - HR1~HR8: 릴레이 상태

---

## 📲 Weintek HMI 설정 (EasyBuilder Pro)
1. Device → Modbus TCP Client 추가
2. IP 주소: `172.30.1.177`
3. Port: `502`
4. Slave ID: `1`
5. Coil/HR 주소 매핑:
   - Coil 0~7 → 릴레이 제어 버튼
   - HR0 → 센서 값 표시
   - HR1~HR8 → 릴레이 상태 표시

---

## 🚀 실행 방법
1. 아두이노에 `src/main.ino` 업로드
2. Ethernet Shield 2를 네트워크에 연결
3. Weintek HMI 또는 Modbus Poll에서 IP `172.30.1.177:502` 접속
4. 릴레이 및 센서 제어/모니터링 확인
