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
 
  - 📌 프로젝트 요약-
  - 
🖥️ 환경
보드: Arduino Due

이더넷 쉴드: W5500 기반 Ethernet2

센서: DHT22 (온도/습도)

릴레이: 8채널, Active Low 타입

⚙️ 주요 기능
Modbus TCP Slave 구현 (포트 502)

지원 기능 코드:

0x01 → Coil 읽기 (릴레이 상태)

0x03 → Holding Register 읽기 (릴레이 상태 + 센서 값)

0x05 → 단일 Coil 쓰기 (릴레이 제어)

0x0F → 다중 Coil 쓰기 (릴레이 제어)

릴레이 제어: Coil 0~7 → 릴레이 핀 {3,5,6,7,8,9,11,13}

센서 데이터:

HR9 → 온도 (×10 스케일링, 예: 23.5°C → 235)

HR10 → 습도 (×10 스케일링, 예: 55.2% → 552)

🔌 핀 연결
릴레이: 3, 5, 6, 7, 8, 9, 11, 13번 핀

DHT22 데이터핀: 12번 핀

DHT22 전원: 3.3V, GND

풀업 저항: 데이터핀과 VCC 사이에 10kΩ

📊 동작 흐름
Modbus 클라이언트가 Coil 쓰기 → 릴레이 ON/OFF 제어

릴레이 상태는 HR0~HR7에 반영

DHT22 센서 읽기 → HR9(온도), HR10(습도)에 저장

클라이언트에서 HR9/HR10 읽기 → 센서 값 확인

📌 전체 설명
🖥️ 하드웨어 환경
보드: Arduino Due (3.3V 로직)

이더넷 쉴드: Ethernet2 (W5500)

센서: DHT22 (온도/습도)

릴레이 모듈: 8채널, Active Low 타입

⚙️ 네트워크
MAC 주소: {DE:AD:BE:EF:FE:ED}

고정 IP: 172.30.1.177

Modbus TCP 포트: 502

🔌 핀 연결
릴레이 핀: {3, 5, 6, 7, 8, 9, 11, 13}

DHT22 데이터핀: 12번 핀

DHT22 전원: 3.3V, GND

풀업 저항: 데이터핀과 VCC 사이에 10kΩ

📊 Modbus 매핑
Coil 0~7 → 릴레이 제어

Holding Register HR0~HR7 → 릴레이 상태 피드백 (ON=1, OFF=0)

Holding Register HR9 → 온도 (×10 스케일링, 예: 23.5°C → 235)

Holding Register HR10 → 습도 (×10 스케일링, 예: 55.2% → 552)

🔄 지원 기능 코드
0x05 → Write Single Coil (릴레이 하나 제어)

0x0F → Write Multiple Coils (릴레이 여러 개 제어)

0x01 → Read Coils (릴레이 상태 읽기)

0x03 → Read Holding Registers (릴레이 상태 + 센서 값 읽기)

📝 동작 흐름
릴레이 초기화: 시작 시 모든 릴레이 OFF (HIGH 출력, Active Low 기준).

Modbus 요청 처리:

Coil 쓰기 요청 → 릴레이 ON/OFF 제어

Coil 읽기 요청 → 현재 릴레이 상태 반환

Holding Register 읽기 요청 → 릴레이 상태 + DHT22 온도/습도 반환

센서 읽기:

DHT22에서 온도·습도 측정

NaN이 아닐 경우 HR9, HR10에 값 저장

클라이언트 응답: Modbus TCP 패킷 형식에 맞게 응답 전송

✅ 요약
릴레이 8개 제어 및 상태 피드백

DHT22 센서 값(온도/습도) Modbus HR9, HR10에 제공

Modbus TCP 표준 포트 502에서 동작

Active Low 릴레이 모듈 대응
