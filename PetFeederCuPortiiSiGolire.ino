#include <Wire.h>
#include <RTClib.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

RTC_DS1307 rtc;
Servo mot;
LiquidCrystal_I2C lcd(0x27, 16, 2);


int oraSetata   = -1;
int minutSetat = -1;


const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {2, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3,12};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);


bool hranitAzi = false;

const int ledPin = 13;
const int servoPin = 9;
int portie = 0;
void setup() {
  Serial.begin(9600);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin,LOW);
  mot.attach(servoPin);
  mot.write(0);

  Wire.begin();
  rtc.begin();

  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); 
  lcd.init();
  lcd.backlight();
  lcd.clear();
   lcd.setCursor(0,0);
  lcd.print("SISTEM PORNIT");
  lcd.setCursor(0,1);
  lcd.print("APASA * SETARE");

  Serial.println("SISTEM PORNIT");
  Serial.println("Apasa * pentru setare ora hranire (HHMM)");
}

void loop() {
  DateTime now = rtc.now();

  Serial.print("Ora curenta: ");
  Serial.print(now.hour());
  Serial.print(":");
  Serial.println(now.minute());

  char key = keypad.getKey();

  
  if (key == 'C') {
  golireManuala();
  }

  if (key == '*') {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("SETARE ORA");
    lcd.setCursor(0,1);
    lcd.print("INTRODU 4 CIFRE");

    seteazaOraHranire();

  }

  
  if (oraSetata >= 0 && portie > 0&& !hranitAzi &&
      now.hour() == oraSetata &&
      now.minute() == minutSetat) {

    lcd.clear();
    lcd.setCursor(0,0);    
    lcd.println("=== HRANIRE ===");

    
    digitalWrite(13, HIGH);

    for (int pos = 0; pos <= 90; pos += 5) {
      mot.write(pos);
       delay(50);
    }
    if(portie==2){
      delay(800);
    }else{
      delay(300);
    }
    
    for (int pos = 90; pos >= 0; pos -= 5) {
        mot.write(pos);
        delay(50);
    }

    digitalWrite(13, LOW);

    hranitAzi = true;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("ANIMAL HRANIT");
  }

  
  if (now.hour() == 0 && now.minute() == 1) {
    hranitAzi = false;
  }

  delay(300);
}
void afiseazaSetariFinale() {
  lcd.clear();

  lcd.setCursor(0,0);
  if (portie == 2) {
    lcd.print("PORTIE MARE");
  } else if (portie == 1) {
    lcd.print("PORTIE MICA");
  }

  lcd.setCursor(0,1);
  lcd.print("ORA ");
  if (oraSetata < 10) lcd.print("0");
  lcd.print(oraSetata);
  lcd.print(":");
  if (minutSetat < 10) lcd.print("0");
  lcd.print(minutSetat);
}

void selecteazaPortie() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("SELECTEAZA");
  lcd.setCursor(0,1);
  lcd.print("A=Mare B=Mica");

  Serial.println("Selecteaza portia: A = Mare, B = Mica");

  while (true) {
    char k = keypad.getKey();
    if (!k) continue;

    if (k == 'A') {
      portie = 2;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("PORTIE MARE");
      Serial.println("PORTIE MARE");
      delay(1500);
      break;
    }

    if (k == 'B') {
      portie = 1;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("PORTIE MICA");
      Serial.println("PORTIE MICA");
      delay(1500);
      break;
    }
  }
}


void seteazaOraHranire() {
  String buffer = "";

  Serial.println("SETARE ORA HRANIRE");
  Serial.println("Introdu 4 cifre (HHMM) si apasa #");

  while (true) {
    char k = keypad.getKey();
    if (!k) continue;

    if (k >= '0' && k <= '9') {
      if (buffer.length() < 4) {
        buffer += k;
        Serial.print(k);
      }
    }

    if (k == '#') {
      Serial.println();

      if (buffer.length() == 4) {
        oraSetata   = (buffer[0] - '0') * 10 + (buffer[1] - '0');
        minutSetat = (buffer[2] - '0') * 10 + (buffer[3] - '0');


        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("ORA HRANIRE");
        lcd.setCursor(0,1);
        lcd.print(oraSetata);
        lcd.print(":");
        if (minutSetat < 10) lcd.print("0");
        lcd.print(minutSetat);

        Serial.print("ORA HRANIRE SETATA: ");
        Serial.print(oraSetata);
        Serial.print(":");
        Serial.println(minutSetat);
        selecteazaPortie();
        afiseazaSetariFinale();
        delay(3000);
      } else {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("EROARE");
        lcd.setCursor(0,1);
        lcd.print("INTRODU 4 CIFRE");

        Serial.println("EROARE: introdu exact 4 cifre!");
      }

      break;  
    }
  }
  

}
void golireManuala() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("GOLIRE MANUALA");

  digitalWrite(ledPin, HIGH);

  mot.write(120);      
  delay(4000);        

  mot.write(0);        


  digitalWrite(ledPin, LOW);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("GOLIRE FINALIZATA");
  delay(2000);
}
