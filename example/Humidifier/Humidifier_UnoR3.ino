#include <DHT11.h>                  // 온습도 센서 DHT11 라이브러리 
#include <Wire.h>
#include <LiquidCrystal_I2C.h>      // 1602 LCD 라이브러리

#define EN1 2                       // 가습기 모듈 1번 신호선
#define EN2 3                       // 가습기 모듈 2번 신호선

LiquidCrystal_I2C lcd(0x27, 16, 2);

DHT11 dht11(4);

int tem, hum;

int Opt_hum_min = 50, Opt_hum_max = 60; // Optimal_humidity

void setup(){
  Serial.begin(9600);
  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);

  lcd.init();                           // LCD 초기화
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("tem:  C");                 // LCD에 온도 및 습도 문구와 Timer 글자 출력
  lcd.setCursor(8, 0);
  lcd.print("hum:  %");
}

void loop(){
  int result = dht11.readTemperatureHumidity(tem, hum); // 온습도 센서값 받아오기
  
  lcd.setCursor(12, 0);                                 // LCD에 습도 값 출력
  lcd.print(hum);

  lcd.setCursor(4, 0);                                  // LCD에 온도 값 출력
  lcd.print(tem);
  
  if(hum < Opt_hum_min){                                // 측정된 습도 값이 설정 최소 습도값(50%)보다 낮을 경우
    digitalWrite(EN1, LOW);                               // 가습기 두 개 모두 동작
    digitalWrite(EN2, LOW);
  }else if(hum >= Opt_hum_min && hum < Opt_hum_max){    // 측정된 습도 값이 설정 최소 습도값(50%)과 최대 습도값(60%) 사이일 경우
    digitalWrite(EN1, LOW);                               // 가습기 두 개 중 하나만 동작
    digitalWrite(EN2, HIGH);
  }else if(hum >= Opt_hum_max){                         // 측정된 습도 값이 설정 최대 습도값(60%)보다 높을 경우
    digitalWrite(EN1, HIGH);                              // 가습기 모두 정지
    digitalWrite(EN2, HIGH);
  }
}
