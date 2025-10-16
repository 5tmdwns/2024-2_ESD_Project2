<p align="center">
  <h1 align="center">FreeRTOS 기반 식당관리 시스템(모순 MOSOON)✨</h1>
</p>

## Index ⭐
 - [1. 프로젝트 목표](#1-프로젝트-목표)
 - [2. 아이디어 구현](#2-아이디어-구현)
   - [2-1. Information(STM32)](#21-InformationSTM32)
   - [2-2. 객실(STM32)](#22-객실STM32)
   - [2-3. 주방(STM32)](#23-주방STM32)
 - [3. 회로](#3-회로)
   - [3-1. Information(STM32)](#31-InformationSTM32)
   - [3-2. 객실(STM32)](#32-객실STM32)
   - [3-3. 주방(STM32)](#33-주방STM32)
   - [3-4. 코스 사진 출력 Display(STM32)](#34-코스-사진-출력-DisplaySTM32)
   - [3-5. 전체 회로도](#35-전체-회로도)
 - [4. SPI 4-wire Interface TFT IPS LCD모듈(ST7789)](#4-SPI-4wire-Interface-TFT-IPS-LCD모듈ST7789)
 - [5. 보드별 코드 및 통신 분석]
   - [5-1. 주방(STM32)](#51-주방STM32)
   - [5-2. 코스 사진 출력 Display(STM32)](#52-코스-사진-출력-DisplaySTM32)
 - [6. 시연영상]

## 1. 프로젝트 목표
&nbsp;최근 들어 파인다이닝에 대한 대중의 관심도가 높아지고 있습니다. <br/>
이에 따라, 고객의 취향에 맞추어 맞춤 서비스를 제공하여 만족도를 높이고, 식당 차원에서도 관리를 용이하게 하는 시스템을 구현하는 것을 목표로 설정했습니다. <br/>
프로젝트의 주제인 멀티태스킹을 적절히 이용하기 위해 파인다이닝에서 발생할 수 있는 다양한 상황을 RTOS의 스케줄링과 뮤텍스, 세마포어를 통해 공유 자원을 효율적으로 관리하여 입장부터 안내, 식사 상황, 그리고 주방 안에서 일어날 수 있는 여러 가지 상황을 유기적이고 효율적으로 관리하는 시스템을 구현하였습니다. <br/>

&nbsp;RTOS를 활용하여 프로젝트를 구현하기에 앞서 기본적인 개념을 이해해야 합니다. <br/>
목적에 따라 중요도가 설정된 Task를 수행하고, 우선 순위에 따라 빈도수 및 제어권을 우선적으로 가질 수 있도록 합니다. <br/>
이와 동시에 여러 Task가 제어권을 가짐에 따라서 Primitive Scheduling(선점형 스케쥴링)이 적용됩니다. <br/>
이때, 우선 순위가 CPU 점유권을 행사할 수 있는 근거가 되어 멀티 프로세스 환경에서 각 Task에 대한 응답성을 적극 활용할 수 있게 됩니다. <br/>
하지만, 선점형 스케쥴링의 문제는 공유 자원에 대한 대가 타이밍에 의해 이루어지며, 이를 위해서는 엄격한 흐름 제어가 필요합니다. <br/>
이에 대한 해결책으로 2가지 방법을 떠올릴 수 있습니다. <br/>

**1) 시행 착오를 통한 엄격한 타이밍 설정** – 하지만, 위 방법의 경우 디버깅을 통해 수많은 시행 착오를 해야하며, 동시에 엄격한 타이밍이 설정된다고 하더라도 Critical Section에 대처를 할 수 없다는 단점이 있습니다. <br/>
**2) 세마포어 및 뮤텍스** – 이는 위 방법의 단점을 보완할 수 있는 동기화 방법입니다. <br/> 이를 통해 공유 자원에 접근할 수 있는 권한을 확인하고, 안전하게 제어 메커니즘을 형성할 수 있습니다. <br/>

&nbsp;Task를 우선 순위(중요도)에 따라 CPU를 독점할 수 있게 했다면, 그 다음으로 중요한 것은 공유 자원에 대한 동기화입니다. <br/>
응급실에서 아무리 급한 환자라고 하더라도, 위급 상황 및 긴급한 문제 발생 등에 대한 매뉴얼이 있는 것처럼 공유 자원에 대한 매뉴얼을 정해주는 것이 필요합니다. <br/>
이를 위해서 세마포어 및 뮤텍스에 대한 적극적인 활용이 필요하다는 것을 확인할 수 있습니다. <br/>

&nbsp;“어떻게 세마포어와 뮤텍스를 제어에 활용하느냐?”, 이것이 이번 프로젝트의 핵심이었다고 생각합니다. <br/> 
적절한 Task를 설정하고, 공유 자원에 대한 안전한 처리 규정을 정해주는 것을 구현의 목표로 설정하여 수많은 디버깅을 통해 Dead Lock, Data Corruption, Race Condition 그리고 Priority Inversion 등에 대한 문제가 다양하게 발생하는 것을 확인할 수 있었습니다. <br/>
이를 해결하기 위해 Task의 중요도를 변경하고, 세마포어 및 뮤텍스 등에 변화를를 주며 Serial Monitor를 통해 Task가 요구하는 목적에 적합하게 제어될 수 있도록 했습니다. <br/>

## 2. 아이디어 구현
### 2-1. Information(STM32)

- 사용센서: PIR센서, LCD, 버튼, OLED
- 공유자원: LCD 제어 권한

<table>
  <tr>
    <th>First Priority Task</th>
    <td>
      PIR 센서로 손님의 인원수 감지 프로세스<br/>
      LCD를 통해 비밀번호 입력 프로세스 (Mutex take 필요)
    </td>
  </tr>
  <tr>
    <th>Second Priority Task</th>
    <td>
      STM2에 손님 입장 시그널 통신 전송<br/>
      LCD를 통한 환영 문구 및 테이블 안내 프로세스 (Mutex take 필요)<br/>
      OLED를 통해 메뉴 소개 프로세스
    </td>
  </tr>
  <tr>
    <th>Third Priority Task</th>
    <td>
      LCD, OLED를 통한 코스 메뉴 안내 (Mutex take 필요)
    </td>
  </tr>
</table>

### 2-2. 객실(STM32)

- 사용센서: LCD 센서, Button
- 공유자원: Menu_Buffer, current_Guest, current_Course, AllGuestSelected, Num_People

<table>
  <tr>
    <th>First Priority Task</th>
    <td>
      객실 내부 인원 체크 및 자리 비움 확인
    </td>
  </tr>
  <tr>
    <th>Second Priority Task</th>
    <td>
      객실원의 메뉴 결정 및 데이터 관리
    </td>
  </tr>
  <tr>
    <th>Third Priority Task</th>
    <td>
      자체 딜레이 로직을 통한 SPI통신(주방(STM32)과의 통신 로직)
    </td>
  </tr>
</table>

### 2-3. 주방(STM32)

- 사용센서: BUZZER, LED, LCD
- 공유자원: 4명의 코스 데이터(구조체 배열)

<table>
  <tr>
    <th>First Priority Task</th>
    <td>
      객실(STM32)로 부터 받은 데이터 Copy
    </td>
  </tr>
  <tr>
    <th>Second Priority Task</th>
    <td>
      Copy한 데이터 정보를 읽어 재료 관리 및 조리 시간 변경
    </td>
  </tr>
  <tr>
    <th>Third Priority Task</th>
    <td>
      First, Second를 거친 데이터를 통한 LCD 작업
    </td>
  </tr>
  <tr>
    <th>Fourth Priority Task</th>
    <td>
      음식이 완료된 후의 작업
    </td>
  </tr>
</table>

## 3. 회로
### 3-1. Information(STM32)

<p align="center" style="margin: 20px 0;">
  <img width="60%" alt="Information Circuit Image" src="https://github.com/user-attachments/assets/a0d6e0ac-30b2-4ea2-abd5-774e2565c644" />
</p>

&nbsp;**SPI 통신** <br/>
- `PA5`: `SPI1_SCK`
- `PA6`: `SPI1_MISO`
- `PA7`: `SPI1_MOSI`

### 3-2. 객실(STM32)

<p align="center" style="margin: 20px 0;">
  <img width="60%" alt="객실 Circuit Image" src="https://github.com/user-attachments/assets/5887c934-c959-41f5-97f4-aca9bde8861e" />
</p>


&nbsp;**SPI 통신** <br/>
- `PA5`: `SPI1_SCK`
- `PA6`: `SPI1_MISO`
- `PA7`: `SPI1_MOSI`
- `PC4`: `SPI1_SS`
 
### 3-3. 주방(STM32)

<p align="center" style="margin: 20px 0;">
  <img width="60%" alt="주방 Circuit Image" src="https://github.com/user-attachments/assets/b6ae1c15-a1fb-46ca-a82c-6fdae070cec8" />
</p>

&nbsp;**SPI 통신** <br/>
- `PA5`: `SPI1_SCK`
- `PA6`: `SPI1_MISO`
- `PA7`: `SPI1_MOSI`
- `PC4`: `SPI1_SS`
 
### 3-4. 코스 사진 출력 Display(STM32)

<p align="center" style="margin: 20px 0;">
  <img width="49%" alt="코스 사진 출력 Display Circuit Image" src="https://github.com/user-attachments/assets/866383f9-3c9d-429f-88c4-53cc21bad8df" />
  <img width="49%" alt="코스 사진 출력 Display Pin Map Image" src="https://github.com/user-attachments/assets/3d597556-0ef4-4b3c-887e-e9a89d8c5edd" />
</p>

&nbsp;**4-wire Interface** <br/>
- `SCL`: `PA5`
- `SDA`: `PA7`
- `RES`: `PA1`
- `DC`: `PA2`
- `CS`: `PC5`
- `BLK`: `PB0`
 
### 3-5. 전체 회로도

<p align="center" style="margin: 20px 0;">
  <img width="60%" alt="전체 회로도 Image" src="https://github.com/user-attachments/assets/e0dcad6b-d163-4360-a4df-b87c74bcf474" />
</p>

&nbsp;**Information(STM32)** <br/>
- OLED
- PIR
- LCD

&nbsp;**객실(STM32)** <br/>
- LCD
- BUTTON

&nbsp;**주방(STM32)** <br/>
- LCD
- BUZZER
- LED

&nbsp;**코스 사진 출력 Display(STM32)** <br/>
- ST7789 TFT LCD
  
