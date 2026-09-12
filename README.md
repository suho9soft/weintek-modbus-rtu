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

 # Arduino Modbus TCP Relay Controller

이 프로젝트는 **Arduino DUE**와 **Ethernet Shield (W5500 기반 Ethernet2 라이브러리)**를 사용하여  
Modbus TCP 프로토콜을 통해 릴레이(총 9개)를 제어하는 예제입니다.

---

## ⚙️ 주요 기능
- Modbus TCP Slave (Port 502)
- 지원 기능 코드:
  - `0x01` : Read Coils
  - `0x03` : Read Holding Registers
  - `0x05` : Write Single Coil
  - `0x0F` : Write Multiple Coils
- 릴레이 상태를 **Coil Map**과 **Holding Register**에 반영
- Active Low 릴레이 제어 (LOW = ON, HIGH = OFF)

---

## 🛠️ 하드웨어 구성
- **Arduino DUE**
- **Ethernet Shield (W5500)**
- **릴레이 모듈 9개**
- 릴레이 핀 연결:
  - DUE 핀: `5, 6, 7, 8, 15, 16, 17, 18, 19`

---

## 📡 네트워크 설정
- MAC 주소: `DE:AD:BE:EF:FE:ED`
- 고정 IP: `172.30.1.177`
- Modbus TCP 포트: `502`

---

## ▶️ 사용 방법
1. 아두이노 IDE에서 코드를 업로드합니다.
2. Ethernet Shield를 네트워크에 연결합니다.
3. Modbus TCP 클라이언트(예: Modbus Poll, SCADA 시스템)에서 IP와 포트(502)를 설정합니다.
4. Coil 또는 Holding Register를 읽거나 쓰기를 통해 릴레이를 제어합니다.

---

## 📖 예시
- Coil 0 → 릴레이 0 제어
- Coil 1 → 릴레이 1 제어
- Holding Register 0~8 → 릴레이 상태 반영 (0=OFF, 1=ON)

---

## 🧑‍💻 코드 파일
- `relay_modbus_tcp.ino` : 메인 아두이노 스케치

---

# Arduino Modbus TCP Relay Controller

## 📌 프로젝트 개요
이 프로젝트는 **Arduino DUE**와 **W5500 Ethernet Shield**를 사용하여  
Modbus TCP 프로토콜로 9채널 릴레이를 제어하는 예제입니다.  
Node-RED, SCADA, PLC 등과 연동하여 자동화 시스템을 구축할 수 있습니다.

## ⚙️ 하드웨어 구성
- Arduino DUE
- W5500 Ethernet Shield
- 9채널 릴레이 모듈 (Active Low)
- 네트워크 연결 (고정 IP: 172.30.1.177)

## 🔌 릴레이 핀 연결
Relay 0 → D5  
Relay 1 → D6  
Relay 2 → D7  
Relay 3 → D8  
Relay 4 → D15  
Relay 5 → D16  
Relay 6 → D17  
Relay 7 → D18  
Relay 8 → D19  

## 📡 지원 Modbus 기능
- **0x01**: Read Coils (릴레이 상태 읽기)
- **0x03**: Read Holding Registers (HR 읽기)
- **0x05**: Write Single Coil (릴레이 하나 제어)
- **0x0F**: Write Multiple Coils (릴레이 여러 개 제어)

## 🚀 실행 방법
1. Arduino IDE에서 `src/modbus_relay.ino` 업로드
2. Ethernet Shield를 네트워크에 연결
3. Modbus TCP 클라이언트(Node-RED, Modbus Poll 등)에서  
   - IP: `172.30.1.177`  
   - Port: `502`  
   - Unit ID: `1`  
   로 접속
4. Coil/HR 읽기·쓰기 → 릴레이 제어

## 🖥️ Node-RED 연동
- `node-red-contrib-modbus` 설치
- `Modbus Write` 노드로 Coil 제어
- `Modbus Read` 노드로 릴레이 상태 모니터링
- 대시보드 버튼으로 릴레이 ON/OFF 가능

## 📝 라이선스
MIT License


 
