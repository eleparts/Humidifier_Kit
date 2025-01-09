#include <RTC.h>
#include <DHT11.h>                  // 온습도 센서 DHT11 라이브러리 
#include <Wire.h>
#include <LiquidCrystal_I2C.h>      // 1602 LCD 라이브러리
#include "Arduino_LED_Matrix.h"

#define EN1 2                       // 가습기 모듈 1번 신호선
#define EN2 3                       // 가습기 모듈 2번 신호선

LiquidCrystal_I2C lcd(0x27, 16, 2);

DHT11 dht11(4);

ArduinoLEDMatrix matrix;

int tem, hum; 

int se = 0, mi = 0, ho = 0;
int before_se = 0;
int timer = 0;
int Opt_hum_min = 50, Opt_hum_max = 60; // Optimal_humidity

void setup(){
  Serial.begin(9600);
  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);

  lcd.init();                           // LCD 초기화
  lcd.backlight();
  lcd.clear();

  lcd.print("tem:  C");                 // LCD에 온도 및 습도 문구와 Timer 글자 출력
  lcd.setCursor(8, 0);
  lcd.print("hum:  %");
  lcd.setCursor(0, 1);
  lcd.print("Timer");
  
  RTC.begin();                          // 타이머를 위한 RTC 모듈에서 현재 시간을 0시 0분 0초로 세팅
  RTCTime startTime(01, Month::JANUARY, 2025, 00, 00, 00, DayOfWeek::MONDAY, SaveLight::SAVING_TIME_ACTIVE);
  RTC.setTime(startTime);

  matrix.begin();                       // LED 매트릭스 시작
}

/* 
 *  아래 32비트 정수 배열 ON의 2차원 배열 버전
byte ON[8][12] = {
  { 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1 },
  { 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1 },
  { 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1 },
  { 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1 },
  { 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1 },
  { 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1 }
};
*/

const uint32_t ON[] = {                 // LED_Matrix ON 배열
  0xf9189989,
  0x58938918,
  0x91891f91
};

/* 
 *  아래 32비트 정수 배열 OFF의 2차원 배열 버전
byte OFF[8][12] = {
  { 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0 },
  { 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0 },
  { 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0 },
  { 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0 },
  { 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0 },
  { 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0 },
  { 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0 },
  { 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0 }
};
*/

const uint32_t OFF[] = {                 // LED_Matrix OFF 배열
  0xeeea88a8,
  0x8aeea88a,
  0x88a88e88
};

void loop(){
  int result = dht11.readTemperatureHumidity(tem, hum); // 온습도 센서값 받아오기
  
  lcd.setCursor(12, 0);                                 // LCD에 습도 값 출력
  lcd.print(hum);

  lcd.setCursor(4, 0);                                  // LCD에 온도 값 출력
  lcd.print(tem);
  
  RTCTime currentTime;                                  // RTC 시간 받아오기
  RTC.getTime(currentTime);
  ho = currentTime.getHour();
  mi = currentTime.getMinutes();
  se = currentTime.getSeconds();
  
  static int target_ho = ho + 7;                        // 타이머 시간 설정 0시 0분 0초 시작으로 설정했기에
  static int target_mi = mi;                            // + n 하는 만큼 타이머가 설정됨
  static int target_se = se;
  
  static int print_ho = target_ho - ho;                 // LCD에 타이머에 대한 시간으로 출력 될 변수 생성
  static int print_mi = target_mi - mi;
  static int print_se = target_se - se;

  if(before_se != se ){                                 // 이전 초 값이 현재 초 값과 다르면
    print_se--;                                         // 즉, 초 값이 변경되면 출력되는 초 값을 1초 감소
    if(print_se < 0){
      print_se = 59;
      print_mi--;
    }
    if(print_mi < 0){
      print_mi = 59;
      print_ho--; 
    }
  }

  if(timer == 0){                    // 타이머가 끝나지 않았을 시,

    lcd.setCursor(6,1);             // 타이머 남은 시간 출력
    lcd.print(print_ho);
    lcd.setCursor(7,1);
    lcd.print("h");

    if(print_mi < 10){
      lcd.setCursor(9,1);
      lcd.print("0");
      lcd.setCursor(10,1);
      lcd.print(print_mi);
    }else if(print_mi >= 10){
      lcd.setCursor(9,1);
      lcd.print(print_mi);
    }
    lcd.setCursor(11,1);
    lcd.print("m");

    if(print_se < 10){
      lcd.setCursor(13,1);
      lcd.print("0");
      lcd.setCursor(14,1);
      lcd.print(print_se);
    }else if(print_se >= 10){
      lcd.setCursor(13,1);
      lcd.print(print_se);
    }
    lcd.setCursor(15,1);
    lcd.print("s");
    
    if(hum < Opt_hum_min){                                // 측정된 습도 값이 설정 최소 습도값(50%)보다 낮을 경우
      matrix.loadFrame(ON);                               // ON LED_Matrix 동작 및 가습기 두 개 모두 동작
      digitalWrite(EN1, LOW);
      digitalWrite(EN2, LOW);
    }else if(hum >= Opt_hum_min && hum < Opt_hum_max){    // 측정된 습도 값이 설정 최소 습도값(50%)과 최대 습도값(60%) 사이일 경우
      matrix.loadFrame(ON);                               // ON LED_Matrix 동작 및 가습기 두 개 중 하나만 동작
      digitalWrite(EN1, LOW);
      digitalWrite(EN2, HIGH);
    }else if(hum >= Opt_hum_max){                         // 측정된 습도 값이 설정 최대 습도값(60%)보다 높을 경우
      matrix.loadFrame(OFF);                              // OFF LED_Matrix 동작 및 가습기 모두 정지
      digitalWrite(EN1, HIGH);
      digitalWrite(EN2, HIGH);
    }
  
    if(print_ho == 0){                                    // LCD에 출력되는 시, 분, 초 값이 모두 0이 되면, 타이머 종료
      if(print_mi == 0){
        if(print_se == 0){
          timer = 1;
        }
      }
    }
  }else if(timer == 1){                                   // 타이머가 종료되었을 때, OFF LED_Matrix 동작 및 가습기 모두 정지
    matrix.loadFrame(OFF);
    digitalWrite(EN1, HIGH);
    digitalWrite(EN2, HIGH);
  }

  before_se = se;
}
