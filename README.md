<p align="center">
  <h1 align="center">FreeRTOS 기반 식당관리 시스템(모순 MOSOON)✨</h1>
</p>

> **해당 프로젝트에서는 `주방(STM32)`, `코스 사진 출력 Display(STM32)` 보드 개발 역할을 맡았습니다.** <br/>
> **5. 보드별 코드 및 통신 분석에서는 해당 보드에 대한 설명만 있습니다.** <br/>
> **또한, Source 코드도 해당 보드에 대한 코드만 있습니다.** <br/>

## Index ⭐
 - [1. 프로젝트 목표](#1-프로젝트-목표)
 - [2. 아이디어 구현](#2-아이디어-구현)
   - [2-1. Information(STM32)](#2-1-informationstm32)
   - [2-2. 객실(STM32)](#2-2-객실stm32)
   - [2-3. 주방(STM32)](#2-3-주방stm32)
 - [3. 회로](#3-회로)
   - [3-1. Information(STM32)](#3-1-informationstm32)
   - [3-2. 객실(STM32)](#3-2-객실stm32)
   - [3-3. 주방(STM32)](#3-3-주방stm32)
   - [3-4. 코스 사진 출력 Display(STM32)](#3-4-코스-사진-출력-displaystm32)
   - [3-5. 전체 회로도](#3-5-전체-회로도)
 - [4. SPI 4-wire Interface TFT IPS LCD모듈(ST7789)](#4-SPI-4wire-interface-tft-ips-lcd모듈st7789)
 - [5. 보드별 코드 및 통신 분석](#5-보드별-코드-및-통신-분석)
   - [5-1. 주방(STM32)](#5-1-주방stm32)
   - [5-2. 코스 사진 출력 Display(STM32)](#5-2-코스-사진-출력-displaystm32)
 - [6. 시연영상](#6-시연영상)

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

1) **시행 착오를 통한 엄격한 타이밍 설정** – 하지만, 위 방법의 경우 디버깅을 통해 수많은 시행 착오를 해야하며, 동시에 엄격한 타이밍이 설정된다고 하더라도 Critical Section에 대처를 할 수 없다는 단점이 있습니다.
2) **세마포어 및 뮤텍스** – 이는 위 방법의 단점을 보완할 수 있는 동기화 방법입니다. 이를 통해 공유 자원에 접근할 수 있는 권한을 확인하고, 안전하게 제어 메커니즘을 형성할 수 있습니다.

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
- 공유자원: `Menu_Buffer`, `current_Guest`, `current_Course`, `AllGuestSelected`, `Num_People`

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
  <img width="90%" alt="Information Circuit Image" src="https://github.com/user-attachments/assets/a0d6e0ac-30b2-4ea2-abd5-774e2565c644" />
</p>

&nbsp;**SPI 통신** <br/>
- `PA5`: `SPI1_SCK`
- `PA6`: `SPI1_MISO`
- `PA7`: `SPI1_MOSI`

### 3-2. 객실(STM32)

<p align="center" style="margin: 20px 0;">
  <img width="90%" alt="객실 Circuit Image" src="https://github.com/user-attachments/assets/5887c934-c959-41f5-97f4-aca9bde8861e" />
</p>


&nbsp;**SPI 통신** <br/>
- `PA5`: `SPI1_SCK`
- `PA6`: `SPI1_MISO`
- `PA7`: `SPI1_MOSI`
- `PC4`: `SPI1_SS`
 
### 3-3. 주방(STM32)

<p align="center" style="margin: 20px 0;">
  <img width="90%" alt="주방 Circuit Image" src="https://github.com/user-attachments/assets/b6ae1c15-a1fb-46ca-a82c-6fdae070cec8" />
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
  <img width="90%" alt="전체 회로도 Image" src="https://github.com/user-attachments/assets/e0dcad6b-d163-4360-a4df-b87c74bcf474" />
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
  
## 4. SPI 4-wire Interface TFT IPS LCD모듈(ST7789)

### - 사용 이유
&nbsp;기존의 제공받은 OLED로 손님들이 대접받는 코스별 요리의 사진을 표현하고자 한계가 있어, 손님들에게 코스 요리에 대한 정보를 제공하고자 SPI 4-wire Interface 통신을 지원하는 TFT IPS LCD모듈을 사용하게 되었습니다. <br/>

<table>
  <tr>
    <td align="center" width="70%">
      <img width="50%" alt="TFT IPS LCD 모듈 사진" src="https://github.com/user-attachments/assets/4f2624b4-5e9d-4325-b61a-b280446c8b56" />
      <sub><strong>모델명: ZJY169S0800TG01</strong></sub>
    </td>
    <td width="30%">
      <table>
          <tr><th>해상도</th><td>240x280</td></tr>
          <tr><th>입력 전압</th><td>3.3V(2.5~3.3V)</td></tr>
          <tr><th>드라이버 IC</th><td>ST7789V2</td></tr>
          <tr><th>인터페이스</th><td>4-wire SPI</td></tr>
          <tr><th>백라이트</th><td>3 Chip-White LED</td></tr>
          <tr><th>휘도</th><td>480 Cd/m2(TYP)</td></tr>
          <tr><th>색상</th><td>262K</td></tr>
          <tr><th>크기</th><td>31mm x 48mm</td></tr>
          <tr><th>동작온도</th><td>-20~70도</td></tr>
          <tr><th>보관온도</th><td>-30~80도</td></tr>
      </table>
</table>

### - SPI Timing Digaram

<p align="center" style="margin: 20px 0;">
  <img width="49%" alt="Serial Interface Characteristics" src="https://github.com/user-attachments/assets/6bdc68a2-4a06-49f2-916e-b890cab54dac" />
  <img width="49%" alt="4-line Serial Interface Write Protocol" src="https://github.com/user-attachments/assets/fb21f1ff-2e7d-4ba7-ada7-1720e312774a" />
</p>

&nbsp;Clock Polarity는 0(`SCL` 비활성화 상태에서 LOW 유지), Clock Phase는 0 `SCL` 첫번째 엣지에서 데이터를 채가고, 두번째 엣지에서 출력합니다. <br/>
`CPOL` = 0, `CPHA` = 0 이므로 `SCL`의 Rising Edged에서 `SDA` 라인의 데이터를 채가고 Falling Edge에서 데이터를 출력합니다. <br/>

&nbsp;`D/C`: LOW일 때 전송되는 데이터는 명령어이고, HIGH일 때 전송되는 데이터는 명령어 레지스터에 저장될 매개변수 혹은 디스플레이 데이터 램에 저장될 RGB 데이터 배열이다. <br/>

&nbsp;데이터는 8비트씩 MSB부터 전송합니다. (이후, 메모리에 저장되는 방식이 리틀 앤디안, 빅 앤디안의 차이. 이는 설정할 수 있음.) <br/>

&nbsp;전송할 데이터의 종류에 따라 (명령어 = 0, 디스플레이 Data/Command 매개변수 = 1) `D/C` 라인을 설정합니다. <br/>

&nbsp;`C/S` 라인이 LOW 상태가 된 후에 데이터 전송이 반드시 이루어져야만 합니다. <br/>

&nbsp;데이터 전송이 완료된 이후엔 `C/S` 라인을 다시 HIGH로 설정합니다. (이는 SPI의 `SS`) <br/>

<p align="center" style="margin: 20px 0;">
  <img width="90%" alt="4-line Serial Interface Characteristics" src="https://github.com/user-attachments/assets/91613869-a212-4231-a9ea-a11fc45b001f" />
</p>

&nbsp;ST7789에 데이터를 쓸 때는 SCL의 주기가 최소 16ns가 되어야 하고, 데이터를 읽을 때는 최소 150ns가 되어야 합니다. <br/>

## 5. 보드별 코드 및 통신 분석
### 5-1. 주방(STM32)
> **해당 프로젝트에서는 `주방(STM32)`, `코스 사진 출력 Display(STM32)` 보드 개발 역할을 맡았습니다.** <br/>
> **5. 보드별 코드 및 통신 분석에서는 해당 보드에 대한 설명만 있습니다.** <br/>
> **또한, Source 코드도 해당 보드에 대한 코드만 있습니다.** <br/>

#### - Task 간단 정보
&nbsp;해당 STM32에서의 FreeRTOS의 Task는 다음과 같습니다. <br/>

| **Task Priority** | **Task** | **Execution** |
|:---:|:---:|:---:|
| 3 | `copyTask` | 받은 데이터 공유자원으로 복사|
| 2 | `cookTask` | 재료관리 및 요리시작 |
| 1 | `lcdTask` | 재료 및 타이머 LCD 동작 |
| 1 | `buzzerTask` | 요리 완료 후 동작 |

#### - Critical Section
&nbsp;공유자원은 다음과 같습니다.

``` c:main.c
...
typedef struct {
	uint8_t courseData [5][2];
	uint8_t changeCourse [3];
} user_data;
/* USER CODE END PTD */
...
```

&nbsp;위는 구조체 배열로 식당의 코스별 요리 정보와 주방에서 그 코스에 만드는데 걸리는 시간이 담겨있습니다. <br/>
위 구조체 배열은 3개의 Task가 접근을 하며, Critical Section입니다. <br/>

#### - 동작 Flow

``` c:main.c
...
enum cuisine {
	SmallBites = 31,
	AlmondWithCavior, //서브1
	SnowCrabAndPickledChrysanthemum, //서브1 대체1
	AblaloneTaco, //서브1 대체2
	HearthOvenGrilledHanwoo, //메인
	EmberToastedAcornNoodle, //메인 대체1
	BlackSesameSeaurchinNasturtium, //메인 대체2
	BurdockTarteTatinWithSkinJuice, //서브2
	TilefishMustardBrassica, //서브2 대체1
	fattyTuna, //서브2 대체2
	SmallSweets
} cuisine;
...
```

&nbsp;`enum` 배열로 선언한 각 코스의 이름을 선언하였습니다. <br/>
기본 에피타이저 - 서브1 - 메인 - 서브2 - 후식순으로 코스가 진행되는데 서브1, 메인, 서브2는 대체가 2가지가 있어 이는 STM2에서 받는 데이터의 정보를 통하여 수정하여 다시 구조체 배열로 `copyTask`로 인해 저장하게 됩니다. <br/>

``` c:main.c
...
void dataInit()
{
	for (int i = 0; i < 4; i ++)
	{
		userData[i].courseData[0][0] = SmallBites;
		userData[i].courseData[0][1] = 5;
		userData[i].courseData[1][0] = AlmondWithCavior;
		userData[i].courseData[1][1] = 10;
		userData[i].courseData[2][0] = HearthOvenGrilledHanwoo;
		userData[i].courseData[2][1] = 15;
		userData[i].courseData[3][0] = BurdockTarteTatinWithSkinJuice;
		userData[i].courseData[3][1] = 10;
		userData[i].courseData[4][0] = SmallSweets;
		userData[i].courseData[4][1] = 15;
	}
	memset(remainingStuff, 50, sizeof(remainingStuff));
}
...
```

&nbsp;처음 Critical Section을 `enum`의 기본 코스로 저장하는 과정이다. 이는 `int main`안에(While 밖)에 선언함으로써, 한번 초기화하게 합니다. <br/>

``` c:main.c
...
HAL_SPI_Receive_IT(&hspi1, rxBuffer, sizeof(rxBuffer));
  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  spiSem = xSemaphoreCreateBinary();
  cookSem = xSemaphoreCreateBinary();
  lcdSem = xSemaphoreCreateBinary();
  /* USER CODE END RTOS_SEMAPHORES */
...
```

&nbsp;인터럽트 수신대기와 BinarySemaphore의 3가지 선언입니다. <br/>
이는 주방에서 행해지는 작업이 순차적으로 진행됨으로, Semaphore로 3가지 테스크의 시행을 순차적으로 진행시키기 위해 선언하였습니다. <br/>

``` c:main.c
...
  /* start timers, add new ones, ... */
  userTimerHandler = xTimerCreate("userTimer", pdMS_TO_TICKS(1000), pdTRUE, NULL, updateUserTimer);
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of deaultTask */

  /* USER CODE BEGIN RTOS_THREADS */
  xTaskCreate(copyTask, "spiTask", 128, NULL, 3, &copyHandler);
  xTaskCreate(cookTask, "cookTask", 128, NULL, 2, &cookHandler);
  xTaskCreate(lcdTask, "lcdTask", 128, NULL, 1, &lcdHandler);
  xTaskCreate(buzzerTask, "buzzerTask", 128, NULL, 1, &buzzerHandler);

  /* add threads, ... */
...
```

&nbsp;주방에서 다음 요리가 나오기 위한 시간을 LCD로 출력하기 위한 `S/W Timer Handler`를 선언하였다. <br/>
이는 Priode Timer로 `1000pdMS_TICK`간격으로 `xTimerStop`을 선언하기 전까지 시행된다. <br/>

``` c:main.c
...
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if (hspi -> Instance == SPI1)
	{
		startSig = true;
		lcdState = true;
		//UART_Print("SPI rxBuffer Received!\r\n");
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		for (int i = 0; i < 5; i ++)
			rxBuffer[i] = rxBuffer[i] - '0';
		HAL_SPI_Receive_IT(&hspi1, rxBuffer, sizeof(rxBuffer));
		xSemaphoreGiveFromISR(spiSem, &xHigherPriorityTaskWoken);
		//UART_Print("SPI_ISR spiSem Give!\r\n");
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}
...
```

&nbsp;SPI Interrupt 수신으로 버퍼에 데이터를 저장하는 과정이다. 여기서 `‘0’`을 하는 이유는 전달 받는 데이터가 아스키코드로 `‘0’`, `‘1’`, `‘2’`를 각 배열에 전달을 받기 때문에, 이를 현 코드에서의 데이터로 가공시키기 위함입니다. <br/>
또한, `spiSem`을 Give함으로 `copyTask`가 동작하도록 합니다. <br/>

``` c:main.c
...
void copyTask(void *argument)
{
	while(1)
	{
		// UART_Print("copyTask excution!\r\n");
		if(xSemaphoreTake(spiSem, portMAX_DELAY) == pdTRUE)
		{
			//UART_Print("copyTask spiSem Take!\r\n");
			memcpy(&whatCourse, &rxBuffer[0], sizeof(rxBuffer[0]));
			switch (whatCourse)
				{
				case 1 :
					for (int i = 0; i < 4; i ++)
						memcpy(&userData[i].changeCourse[0], &rxBuffer[i + 1], 1);
					break;
				case 2 :
					for (int i = 0; i < 4; i ++)
						memcpy(&userData[i].changeCourse[1], &rxBuffer[i + 1], 1);
					break;
				case 3 :
					for (int i = 0; i < 4; i ++)
						memcpy(&userData[i].changeCourse[2], &rxBuffer[i + 1], 1);
					break;
				}
			xSemaphoreGive(cookSem);
			//UART_Print("copyTask cookSem Give!\r\n");
		}
		vTaskDelay(500);
	}
}
...
```

&nbsp;copyTask에서는 STM2에서 전달받은 데이터를 Critical Section으로 복사하는 작업을 시행합니다. <br/>
여기서 인터럽트가 걸리지 않았으면, `spiSem`의 Take가 불가하므로 시행이 되지 않습니다. <br/>
따라서, 전달받는 데이터가 없을 시, Critical Section으로의 접근은 불가합니다. <br/>
`copyTask`가 진행되었을 시, `cookSem`을 Give함으로써, 다음 순위인 `cookTask`가 실행되도록 합니다. <br/>
마찬가지로, `spiSem`의 Take가 없을 시, `cookTask`도 실행되지 않습니다. <br/>

``` c:main.c
...
void cookTask(void *argument)
{
	while(1)
	{
		if (startSig)
		{
			// UART_Print("cookTask (startSig == true)\r\n");
			if (xSemaphoreTake(cookSem, portMAX_DELAY) == pdTRUE)
			{
				switch (whatCourse)
				{
				case 0 :
					for (int i = 0; i < 6; i ++)
						remainingStuff[i] --;
					printRemainingStuff(Seaweed);
					printRemainingStuff(Potato);
					printRemainingStuff(SalmonRoe);
					printRemainingStuff(Deodeok);
					printRemainingStuff(Radish);
					printRemainingStuff(RedCabbage);
					xSemaphoreGive(lcdSem);
					//UART_Print("cookTask lcdSem Give(whatCourse == 0)!\r\n");
				break;
...
...
				case 4 :
					for (int i = 26; i < 30; i ++)
						remainingStuff[i] --;
					printRemainingStuff(Tuna);
					printRemainingStuff(Hibiscus);
					printRemainingStuff(Peanut);
					printRemainingStuff(HoneyCookie);
					printRemainingStuff(Kombucha);
					xSemaphoreGive(lcdSem);
					//UART_Print("cookTask lcdSem Give(whatCourse == 4)!\r\n");
				break;
				}
			}
		}
		vTaskDelay(500);
	}
}
```

&nbsp;`cookTask`에서는 `copyTask`에서 Critical Section으로 복사한 데이터를 읽어와서, 손님이 변경한 대체정보를 읽어와 Critical Section의 정보를 수정합니다. <br/>
이는 `copyTask`가 먼저 실행되고 나서, `copyTask`가 `spiSem`을 Take하고 `cookSem`을 Give하여 `copyTask`가 다시 실행될 수 없음을 이용하여, Critical Section으로의 접근을 막습니다. <br/> 
따라서, `cookTask`가 Critical Section에 접근할 수 있게 됩니다.  <br/>
접근하여 작업을 수행한 뒤에는 `lcdSem`을 Give함으로, `lcdTask`의 동작이 이루어지게 합니다. <br/>
또한, 남은 재료의 갯수를 Serial Monitor로 띄울 수 있게 합니다. <br/>

``` c:main.c
...
void lcdTask(void *argument)
{
	while(1)
	{
		if (!startSig)
		{
			lcd_clear();
			lcd_put_cur(0, 0);
			lcd_send_string("Seaweed");
			vTaskDelay(1000);
			lcd_clear();
			lcd_put_cur(0, 0);
			lcd_send_string("Potato");
			vTaskDelay(1000);
			lcd_clear();
			lcd_put_cur(0, 0);
			lcd_send_string("SalmonRoe");
			vTaskDelay(1000);
			lcd_clear();
			lcd_put_cur(0, 0);
			lcd_send_string("Deodeok");
			vTaskDelay(1000);
			lcd_clear();
			lcd_put_cur(0, 0);
			lcd_send_string("Radish");
			vTaskDelay(1000);
			lcd_put_cur(0, 0);
			lcd_send_string("RedCabbage");
			vTaskDelay(1000);
		}
		else
...
...
			case 4 :
				if(xSemaphoreTake(lcdSem, portMAX_DELAY) == pdTRUE)
				{
					if (lcdState)
					{
						//UART_Print("lcdTask lcdSem Take(whatCourse == 4)!\r\n");
						for(int i = 0; i < 4; i ++)
							memcpy(&arrCourseTime[i], &userData[i].courseData[0][1], 1);
						maxIndex = findMaxIndex(arrCourseTime, 4);
						lcdState = false;
						xTimerStart(userTimerHandler, 0);
					}
				}
				break;
			}
		}
		vTaskDelay(500);
	}
}
...
```

&nbsp;`lcdTask`의 작업은 손님의 코스변경정보를 받지 않았을 시에는 해당 요리에 필요한 재료정보를 직원들에게 보여주는 용도로 사용합니다. <br/>
손님의 코스변경정보를 받았을 시에는 `cookTask`에서 가공된 Critical Section의 정보를 받아 만드는데 걸리는 시간이 변경되었을 것입니다. <br/>
`cookTask`가 실행되면면 `lcdSem`을 Give하여 Critical Section에 접근하여 새로 복사를 하여 변경된 나오는데 걸리는 시간을 LCD에 카운트 다운을 S/W Timer로 실행하게 됩니다. <br/>

``` c:main.c
...
void updateUserTimer (TimerHandle_t xTimer)
{
	//UART_Print("Timer Start!\r\n");
	uint8_t temp = 0;
	lcd_clear();
	switch (whatCourse)
	{
	case 1 :
		//UART_Print("SUB1 Course Timer Start!\r\n");
		switch (maxIndex1)
		{
		case 0 :
			if (arrCourseTime1[maxIndex1] > 0)
			{
				temp = arrCourseTime1[maxIndex1];
				lcd_put_cur(0, 0);
				formatTime(arrCourseTime1[0], timeStr1);
				lcd_send_string(timeStr1);
				arrCourseTime1[maxIndex1] --;
			}
			if (temp <= arrCourseTime1[1] && arrCourseTime1[1] > 0)
			{
				lcd_put_cur(0, 8);
				formatTime(arrCourseTime1[1], timeStr2);
				lcd_send_string(timeStr2);
				arrCourseTime1[1] --;
			}
			if (temp <= arrCourseTime1[2] && arrCourseTime1[2] > 0)
			{
				lcd_put_cur(1, 0);
				formatTime(arrCourseTime1[2], timeStr3);
				lcd_send_string(timeStr3);
				arrCourseTime1[2] --;
			}
			if (temp <= arrCourseTime1[3] && arrCourseTime1[3] > 0)
			{
				lcd_put_cur(1, 8);
				formatTime(arrCourseTime1[3], timeStr4);
				lcd_send_string(timeStr4);
				arrCourseTime1[3] --;
			}
			break;
		case 1 :
...
	default :
		//UART_Print("Appetizer or Desert Course Timer Start!\r\n");
		lcd_clear();
		lcd_put_cur(0, 0);
		formatTime(arrCourseTime[0], timeStr1);
		lcd_send_string(timeStr1);
		lcd_put_cur(0, 8);
		formatTime(arrCourseTime[1], timeStr2);
		lcd_send_string(timeStr2);
		lcd_put_cur(1, 0);
		formatTime(arrCourseTime[2], timeStr3);
		lcd_send_string(timeStr3);
		lcd_put_cur(1, 8);
		formatTime(arrCourseTime[3], timeStr4);
		lcd_send_string(timeStr4);
		for (int i = 0; i < 4; i ++)
			arrCourseTime[i]--;
		if (arrCourseTime[0] == 0 && arrCourseTime[1] == 0 && arrCourseTime[2] == 0 && arrCourseTime[3] == 0)
		{
			lcd_clear();
			lcd_put_cur(0, 0);
			lcd_send_string("Course Finish!");
			buzzerSig = true;
			xSemaphoreGive(lcdSem);
			UART_Print("S/W Timer lcdSem Give!\r\n");
			xTimerStop(userTimerHandler, 0);
		}
		break;
	}
}
...
```

&nbsp;S/W Timer의 CallBack 함수입니다. <br/>
이는 타이머를 띄우는 용도로 각각의 손님 4명의 고른음식이 다를테니 LCD를 4등분하여 코스 요리를 맞춰서 내보낸다고 가정했을 때, 각각의 손님 4명의 음식이 나오는데 걸리는 시간을 순서대로 띄우는 작업을 합니다. <br/>
4명의 음식이 나올때, `lcdSem`을 Give함으로서 직원들이 다음 음식에 필요한 재료를 `lcdTask`가 동작하게 하여 띄우게 합니다. <br/>

``` c:main.c
...
void buzzerTask(void *argument)
{
	while(1)
	{
		// UART_Print("buzzerTask execution!\r\n");
		if (buzzerSig)
		{
			txData = 0x31;
			HAL_SPI_Transmit(&hspi1, &txData, sizeof(txData), HAL_MAX_DELAY);
			//UART_Print("SPI txData transmit to master!\r\n");
			HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
			vTaskDelay(500);
			HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
			vTaskDelay(500);
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
			vTaskDelay(500);
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
			vTaskDelay(500);
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
			vTaskDelay(500);
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
			vTaskDelay(500);
			txData = 0x30;
			if (whatCourse >= 4)
			{
				whatCourse = 0;
				startSig = false;
				dataInit();
			}
			buzzerSig = !buzzerSig;
		}
		vTaskDelay(500);
	}
}
...
```

&nbsp;buzzerTask에서는 각 코스요리가 나왔다는 신호를 보내기 위한 작업을 합니다. <br/>
이는 Buzzer와 LED를 동작시키게 합니다. <br/>
buzzerTask에서 사용하는 Critical Section은 없습니다. <br/>
따라서, buzzerSig로 실행되게 합니다. <br/>
또한, STM2로 데이터를 보냄으로 음식이 나왔다는 신호를 알려줍니다. <br/>

### 5-2. 코스 사진 출력 Display(STM32)
&nbsp;ST7789 Driver를 사용하기 위한 라이브러리 파일을 사용했습니다. <br/>
라이브러리 파일은 다음과 같이 Github를 참고하여 사용하였습니다. ([라이브러리 Reference](https://github.com/Floyd-Fish/ST7789-STM32.git))

<p align="center" style="margin:20px 0;">
  <img width="90%" alt="ST7789 Library Git Source" src="https://github.com/user-attachments/assets/c2684c85-5b05-4720-b207-c39ef9429597" />
</p>

``` c:st7789.c
#include "st7789.h"

#ifdef USE_DMA
#include <string.h>
uint16_t DMA_MIN_SIZE = 16;
/* If you're using DMA, then u need a "framebuffer" to store datas to be displayed.
 * If your MCU don't have enough RAM, please avoid using DMA(or set 5 to 1).
 * And if your MCU have enough RAM(even larger than full-frame size),
 * Then you can specify the framebuffer size to the full resolution below.
 */
 #define HOR_LEN 	1	//	Also mind the resolution of your screen!
uint8_t disp_buf[ST7789_WIDTH * HOR_LEN];
#endif

/**
 * @brief Write command to ST7789 controller
 * @param cmd -> command to write
 * @return none
 */
static void ST7789_WriteCommand(uint8_t cmd)
{
	ST7789_Select();
	ST7789_DC_Clr();
	HAL_SPI_Transmit(&ST7789_SPI_PORT, &cmd, sizeof(cmd), HAL_MAX_DELAY);
	ST7789_UnSelect();
}

/**
 * @brief Write data to ST7789 controller
 * @param buff -> pointer of data buffer
 * @param buff_size -> size of the data buffer
 * @return none
 */
...
```

``` c:st7789.h
...
#ifndef __ST7789_H
#define __ST7789_H

#include "fonts.h"
#include "main.h"

/* choose a Hardware SPI port to use. */
#define ST7789_SPI_PORT hspi1
extern SPI_HandleTypeDef ST7789_SPI_PORT;

/* choose whether use DMA or not */
// #define USE_DMA

/* If u need CS control, comment below*/
// #define CFG_NO_CS

/* Pin connection*/
#define ST7789_RST_PORT GPIOA
#define ST7789_RST_PIN  GPIO_PIN_1
#define ST7789_DC_PORT  GPIOA
#define ST7789_DC_PIN   GPIO_PIN_2

#ifndef CFG_NO_CS
#define ST7789_CS_PORT  GPIOC
#define ST7789_CS_PIN   GPIO_PIN_5
#endif
...
```

&nbsp;위와같이 `st7789.c`와 `st7789.h`를 사용하였습니다. <br/>
음식에 대한 사진을 출력해야 하기 때문에, 라이브러리에서 다음과 같은 함수를 사용합니다. <br/>

``` c:st7789.c
...
void ST7789_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *data)
{
	if ((x >= ST7789_WIDTH) || (y >= ST7789_HEIGHT))
		return;
	if ((x + w - 1) >= ST7789_WIDTH)
		return;
	if ((y + h - 1) >= ST7789_HEIGHT)
		return;

	ST7789_Select();
	ST7789_SetAddressWindow(x, y, x + w - 1, y + h - 1);
	ST7789_WriteData((uint8_t *)data, sizeof(uint16_t) * w * h);
	ST7789_UnSelect();
}
...
```

&nbsp;위 함수의 사용에 큰 제약이 있었습니다. <br/>
바로, `const uint16_t *data`에 들어갈 data는 사진파일을 RGB565 형식으로 변환한 뒤, 이를 2차원 배열의 .c파일을 Src에 넣어 다음과 같이 선언하여 함수를 출력해보면 Flash가 부족하다는 이유였습니다. <br/>
그 이유를 찾아보면, 다음과 같았습니다. <br/>

<p align="center" style="margin: 20px 0;">
	<img width="49%" alt="STMF103RB Spec 1" src="https://github.com/user-attachments/assets/ee4785c5-38b2-4b4f-9d24-10d8abf00138" />
	<img width="49%" alt="STMF103RB Spec 2" src="https://github.com/user-attachments/assets/823809f2-39ed-4e66-8f29-b0049cc794ff" />
</p>

위와같이, 우리가 사용하는 `STM32Nucleo-F103RB`의 Flash Memory는 128kB이여서, 사진을 여러장 띄울 수 없었습니다. <br/>

<p align="center" style="margin: 20px 0;">
	<img width="49%" alt="STM32F407VGT6 Spec 1" src="https://github.com/user-attachments/assets/53969553-92fa-4a2a-bebb-2ec10972a427" />
	<img width="49%" alt="STM32F407VGT6 Spec 2" src="https://github.com/user-attachments/assets/525392be-2d25-4b03-a6bd-9ca4f4d50623" />
</p>

따라서, 위와같이 1MByte Flash Memory를 지원하는 `STM32F407VGT6`보드를 사용하여, 출력하게 하였습니다. <br/>

``` c:main.c
...
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "st7789.h"
#include "image.h"
/* USER CODE END Includes */
...
...
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  ST7789_DrawImage(0, 40, 240, 240, (uint16_t *)SmallBites);
	  HAL_Delay(1000);
	  ST7789_Init();
	  ST7789_DrawImage(0, 40, 240, 240, (uint16_t *)AlmondWithCavior);
	  HAL_Delay(1000);
	  ST7789_Init();
	  ST7789_DrawImage(0, 40, 240, 240, (uint16_t *)SnowCrabAndPickledChrysanthemum);
	  HAL_Delay(1000);
	  ST7789_Init();
	  ST7789_DrawImage(0, 40, 240, 240, (uint16_t *)BurdockTarteTatinWithSkinJuice);
	  HAL_Delay(1000);
	  ST7789_Init();
	  ST7789_DrawImage(0, 40, 240, 240, (uint16_t *)EmberToastedAcornNoodle);
	  HAL_Delay(1000);
	  ST7789_Init();
	  ST7789_DrawImage(0, 40, 240, 240, (uint16_t *)HearthOvenGrilledHanwoo);
	  HAL_Delay(1000);
	  ST7789_Init();
	  ST7789_DrawImage(0, 40, 240, 240, (uint16_t *)TilefishMustardBrassica);
	  HAL_Delay(1000);
	  ST7789_Init();
	  ST7789_DrawImage(0, 40, 240, 240, (uint16_t *)SmallSweet1);
	  HAL_Delay(1000);
	  ST7789_Init();
	  ST7789_DrawImage(0, 40, 240, 240, (uint16_t *)anSungJae);
	  HAL_Delay(3000);
	  ST7789_Init();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}
```

<table align="center" style="width:100%; table-layout:fixed; border-collapse:collapse;">
  <tr>
    <!-- 왼쪽: 훨씬 넓게 -->
    <td align="center" style="width:50%;">
      <img width="98%" src="https://github.com/user-attachments/assets/0aa63b90-3400-4f1d-bd0b-5a363bd0fc6d" alt="File List"/>
    </td>
    <!-- 가운데 -->
    <td align="center" style="width:25%;">
      <img width="90%" src="https://github.com/user-attachments/assets/80dfefc0-eb73-4db0-beb2-479975c49db7" alt="anSungJae.c"/>
    </td>
    <!-- 오른쪽 -->
    <td align="center" style="width:25%;">
      <img width="90%" src="https://github.com/user-attachments/assets/263ae3a3-99bd-4f84-9803-5eaef9b28721" alt="image.h"/>
    </td>
  </tr>
  <!-- 캡션 -->
  <tr>
    <td align="center"><strong><code>File List</code></strong></td>
    <td align="center"><strong><code>anSungJae.c</code></strong></td>
    <td align="center"><strong><code>image.h</code></strong></td>
  </tr>
</table>

&nbsp;이는 단순히 손님들에게 코스요리에 대한 사진을 시각화함으로 FreeRTOS를 사용하지 않았습니다. <br/>
또한, 위와같은 파일들은 `image.h`에 선언함으로 `extern`하여 사용하였고, 위처럼 배열로 변경하는 작업은 Python Script를 사용하여 기존이미지를 240x240으로 변경하여, RGB565포맷으로 변경한뒤 리틀앤디안으로 저장하기 위하여 상위바이트와 하위바이트를 변경한뒤 .c 2차원배열파일로 저장하여 사용하였습니다. <br/>

``` python:imageToAndianRgb565.py
from PIL import Image

resize_to_fixed_size = True 
fixed_width, fixed_height = 240, 240  

def swap_bytes(rgb565):
    high_byte = (rgb565 >> 8) & 0xFF  
    low_byte = rgb565 & 0xFF          
    return (low_byte << 8) | high_byte  

image_path = "파일이름.jpeg"  
output_file = "파일이름.c"   

image = Image.open(image_path)

if resize_to_fixed_size:
    image = image.resize((fixed_width, fixed_height), Image.Resampling.LANCZOS)
    width, height = fixed_width, fixed_height  
else:
    width, height = image.size  

rgb565_data = []


for y in range(height):
    row = [] 
    for x in range(width):
        r, g, b = image.getpixel((x, y))[:3]  
        rgb565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)
        swapped_rgb565 = swap_bytes(rgb565)
        row.append(swapped_rgb565) 
    rgb565_data.append(row)  

with open(output_file, "w") as file:
    file.write("#include \"image.h\"\n\n")
    file.write(f"const uint16_t 파일이름[{height}][{width}] = {{\n")
    for row in rgb565_data:
        file.write("    {")
        file.write(",".join(f"0x{value:04X}" for value in row))
        file.write("},\n")
    file.write("};\n")

print(f" {output_file} save!")
```

## 6. 시연영상
- [입장](https://drive.google.com/file/d/1NTCRfR3sXfr2LmJT8r22jeOMRN_iNV_2/view?usp=sharing)
- [QR서버홈](https://drive.google.com/file/d/1gZuGmhyukw768nXnn1dyceuLz3geT0pV/view?usp=sharing)
- [QR서버메뉴](https://drive.google.com/file/d/1QcCQY33HyL5IlGx7LmlhcJpJos2wTtdM/view?usp=sharing)
- [주방 타이머 1](https://drive.google.com/file/d/1d9EcNRx9hgoyCosS8EqPOA8HVtiw9l-P/view?usp=sharing)
- [주방 타이머 2](https://drive.google.com/file/d/1aASdzvGTf2WcpOVzPhBbp_Wvt3CHuEjN/view?usp=sharing)
