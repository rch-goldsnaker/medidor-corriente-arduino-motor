#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

#define btnRIGHT 0
#define btnUP 1
#define btnDOWN 2
#define btnLEFT 3
#define btnSELECT 4
#define btnNONE 5

//Sensor de corriente
float Sensibilidad = 0.066;  //sensibilidad en V/A para nuestro sensor
float offset = -0.50;        // Equivale a la amplitud del ruido

//Declaramos variables
float Irms;  //corriente media, medido por el sensor de corriente.
float Ip;    //corriente pico, medido por el sensor de corriente.

int currentScreen = 1;  // Variable para controlar la pantalla actual
int state;              // variable para controlar el nivel 1 de estado
float Ilim = 8;         //valor por defecto de la corriente limite
int Tlim = 3000;        //valor por defecto del tiempo de arranque limite

int count = 0;      //contador
int RelayPin = 13;  //salida normal
int IRmax = 15;     //corriente maximo para determinar un arranque
int IRmin = 2;      //corriente minima para determinar una parada

int read_LCD_buttons() {
  int valor_ADC = analogRead(0);
  if (valor_ADC > 1000) return btnNONE;
  if (valor_ADC < 50) return btnRIGHT;
  if (valor_ADC < 195) return btnUP;
  if (valor_ADC < 380) return btnDOWN;
  if (valor_ADC < 555) return btnLEFT;
  if (valor_ADC < 790) return btnSELECT;
  return btnNONE;
}

void setup() {
  lcd.begin(16, 2);
  pinMode(RelayPin, OUTPUT);
  Serial.begin(9600);
  digitalWrite(RelayPin, HIGH);
}

void loop() {
  if (currentScreen == 1) {
    //Inicio
    lcd.setCursor(0, 0);
    lcd.print("> Monitorear");
    lcd.setCursor(0, 1);
    lcd.print("  Programar");
  } else if (currentScreen == 2) {
    //Inicio
    lcd.setCursor(0, 0);
    lcd.print("  Monitorear");
    lcd.setCursor(0, 1);
    lcd.print("> Programar");
  } else if (currentScreen == 3) {
    //Monitorear
    Ip = get_corriente();
    Irms = Ip * 0.707;

    if (Irms > IRmax) {
      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print("ARRANQ");
      lcd.setCursor(9, 0);
      lcd.print("Ta=");
      lcd.print(Tlim);

      lcd.setCursor(0, 1);
      lcd.print("I=");
      lcd.print(Irms);
      lcd.setCursor(9, 1);
      lcd.print("Id=");
      lcd.print(Ilim);

      delay(Tlim);
      count = count + 1;
    }

    if (IRmin < Irms && Irms < IRmax) {
      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print("MOVI");
      lcd.setCursor(9, 0);
      lcd.print("Ta=");
      lcd.print(Tlim);

      lcd.setCursor(0, 1);
      lcd.print("I=");
      lcd.print(Irms);
      lcd.setCursor(9, 1);
      lcd.print("Id=");
      lcd.print(Ilim);

      // count = 0;

      // digitalWrite(RelayPin, HIGH);
    }

    if (Irms < IRmin) {
      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print("PARADO");
      lcd.setCursor(9, 0);
      lcd.print("Ta=");
      lcd.print(Tlim);

      lcd.setCursor(0, 1);
      lcd.print("I=");
      lcd.print(Irms);
      lcd.setCursor(9, 1);
      lcd.print("Id=");
      lcd.print(Ilim);
      count = 0;

      digitalWrite(RelayPin, HIGH);
    }

    if (count > 0) {
      pulse();
    }


  } else if (currentScreen == 4) {
    //Programar
    ///Select Corriente
    lcd.setCursor(0, 0);
    lcd.print("> Corriente Limite ");
    lcd.setCursor(0, 1);
    lcd.print("  Tiempo Arranque");
  } else if (currentScreen == 5) {
    //Programar
    ///Select Tiempo
    lcd.setCursor(0, 0);
    lcd.print("  Corriente Limite ");
    lcd.setCursor(0, 1);
    lcd.print("> Tiempo Arranque");
  } else if (currentScreen == 6) {
    //Programar
    ///Select Corriente
    ////Modificar Corriente
    lcd.setCursor(0, 0);
    lcd.print("Corriente Limite ");
    lcd.setCursor(0, 1);
    lcd.print("I=");
    lcd.print(Ilim);
  } else if (currentScreen == 7) {
    //Programar
    ///Select Tiempo
    ////Modificar Tiempo
    lcd.setCursor(0, 0);
    lcd.print("Tiempo Arranque ");
    lcd.setCursor(0, 1);
    lcd.print("t(ms)=");
    lcd.print(Tlim);
  }

  int boton = read_LCD_buttons();

  switch (boton) {
    case btnUP:
      if (currentScreen == 2) {
        currentScreen = 1;
      }
      if (currentScreen == 5) {
        currentScreen = 4;
      }
      if (currentScreen == 6) {
        Ilim += 0.1;
      }
      if (currentScreen == 7) {
        Tlim += 100;
      }
      lcd.clear();
      delay(200);
      break;
    case btnDOWN:
      if (currentScreen == 1) {
        currentScreen = 2;
      }
      if (currentScreen == 4) {
        currentScreen = 5;
      }
      if (currentScreen == 6) {
        Ilim -= 0.1;
      }
      if (currentScreen == 7) {
        Tlim -= 100;
      }
      lcd.clear();
      delay(200);
      break;
    case btnRIGHT:
      //Desde Home a Monitorear o Programar
      switch (currentScreen) {
        case 1:
          currentScreen = 3;
          state = 1;
          break;
        case 2:
          currentScreen = 4;
          state = 1;
          break;
        case 4:
          if (state == 1) {
            currentScreen = 6;
            state = 2;
            break;
          }
          break;
        case 5:
          if (state == 1) {
            currentScreen = 7;
            state = 2;
            break;
          }
          break;
      }
      lcd.clear();
      delay(200);
      break;
    case btnLEFT:
      if (currentScreen == 3 || currentScreen == 4 || currentScreen == 5) {
        currentScreen = 1;
        state = 0;
      }
      if (currentScreen == 6 || currentScreen == 7) {
        currentScreen = 4;
        state = 1;
      }
      lcd.clear();
      delay(200);
      break;
  }
}

float get_corriente() {
  float voltajeSensor;
  float corriente = 0;
  long tiempo = millis();
  float Imax = 0;
  float Imin = 0;
  while (millis() - tiempo < 1000)  //realizamos mediciones durante 0.5 segundos
  {
    voltajeSensor = analogRead(A1) * (5.0 / 1023.0);                               //lectura del sensor
    corriente = 0.9 * corriente + 0.1 * ((voltajeSensor - 2.498) / Sensibilidad);  //Ecuación  para obtener la corriente
    if (corriente > Imax) Imax = corriente;
    if (corriente < Imin) Imin = corriente;
  }
  return (((Imax - Imin) / 2) - offset);
}

float pulse() {
  Ip = get_corriente();
  Irms = Ip * 0.707;
  Serial.print("Irms");
  Serial.print(Irms);
  Serial.print("Ilim");
  Serial.print(Ilim);
  if (Irms > Ilim) {
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("SOBRE");
    lcd.setCursor(9, 0);
    lcd.print("Ta=");
    lcd.print(Tlim);

    lcd.setCursor(0, 1);
    lcd.print("I=");
    lcd.print(Irms);
    lcd.setCursor(9, 1);
    lcd.print("Id=");
    lcd.print(Ilim);

    digitalWrite(RelayPin, LOW);
  } else {

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("PERMI");
    lcd.setCursor(9, 0);
    lcd.print("Ta=");
    lcd.print(Tlim);

    lcd.setCursor(0, 1);
    lcd.print("I=");
    lcd.print(Irms);
    lcd.setCursor(9, 1);
    lcd.print("Id=");
    lcd.print(Ilim);

    digitalWrite(RelayPin, HIGH);
  }
}