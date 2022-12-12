// Configuraciones.

//+PIN DE SALIDA: pin donde se conecta la bomba.
#define PIN_SALIDA D8 // Colocar en "D3" el pin que queremos utilizar como salida

//+TIEMPO POR LITRO: tiempo que la bomba permanece encedida por cada litro en segundos.
#define TIEMPO_POR_LITRO 3 // Estariamos prendiendo la bomba 5 segundos por cada litro seleccionado.
//Ej: con "TIEMPO_POR_LITRO = 5" si seleccionamos 10 litros
//la bomba estará encendida 50 segundos.

//+PINES DE LOS BOTONES: Colocar dentro del vector los pines donde irán conectados los botones.
//En este orden: { Subir , Bajar , ON}
//Los botones deben estar conectados en PULL_UP (no es necesaria resistencias)
#define CANT_BOTONES 3 // Cantidad de botones que vamos a conectar.
int BOTONES[CANT_BOTONES] = {D5, D6, D7};

//+RETARDO REINICIO: el tiempo de retardo (en milisegundos) que espera el programa
//cuando queremos reiniciarlo presionando el boton "ON".
#define RETARDO_REINICIO 2000

//+VALOR INICIAL: si nunca se cargo ningun valor en la EEPROM inicia con este valor.
int valor_inicial = 5; // Valor inicial

// Fin de las configuraciones.

//Inicio del programa
#define __DEBUG__
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

#define ANCHO_PANTALLA 128 // Ancho pantalla OLED
#define ALTO_PANTALLA 64 // Alto pantalla OLED
Adafruit_SSD1306 display(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, -1);

#define BOTON_SUBIR 0
#define BOTON_BAJAR 1
#define BOTON_ON 2

int litros;
int comenzar = 0;
int subir = 0;
int bajar = 0;
int i = 0;
int pausa = 0;
int aux;
int frenar = 0;
int tiempo_restante = 0;
int minutos = 59;
int segundos = 59;

void setup() {
  /*
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
#ifdef __DEBUG__
    Serial.println("No se encuentra la pantalla OLED");
#endif
    while (true);
  }
*/
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();

  EEPROM.begin(512);
  Serial.begin(9600);
  pinMode(D5, INPUT_PULLUP);
  pinMode(D6, INPUT_PULLUP);
  pinMode(D7, INPUT_PULLUP);

  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D8, OUTPUT);

  EEPROM.get(0, litros);
  if (litros < 1 || litros > 999)
    litros = valor_inicial;

}

void loop() {

  Funcion_Pantalla(litros); // Funcion para mostrar informacion en la pantalla

  // Subir la cantidad de litros
  subir = Funcion_Subir_Litros(BOTON_SUBIR);

  // Bajar la cantidad de litros
  bajar = Funcion_Bajar_Litros(BOTON_BAJAR);

  // Cuando se pulsa el botón comenzar
  comenzar = Funcion_Leer_Boton(BOTON_ON);
  if (comenzar == 1) {
    EEPROM.put(0, litros);
    EEPROM.commit();
    tiempo_restante = TIEMPO_POR_LITRO * litros;
    while ((frenar == 0) && tiempo_restante > 0) {

      digitalWrite(PIN_SALIDA, HIGH); // Enciendo la bomba

      minutos = Funcion_Minutos(tiempo_restante);
      segundos = Funcion_Segundos(tiempo_restante);
      tiempo_restante--;

      Funcion_Pantalla_Con_Reloj(litros, minutos, segundos);

      // Delay de 1 segundo, verificando si se presiona el BOTON_OFF
      i = 1000;
      while (frenar == 0 && i > 0) {
        i--;
        delay(1);
        pausa = 0;
        aux = 0;
        pausa = Funcion_Boton_Retardo(BOTON_ON);
        //***
        if (pausa == 1) { // Se presiono sin retardo
          pausa = 0;
          delay(5);
          while (pausa == 0) {
            // Se presionó el botón de pausa.
            delay(1);
            digitalWrite(PIN_SALIDA, LOW);
            Funcion_Pantalla_Pausada (minutos, segundos);
            pausa = Funcion_Boton_Retardo(BOTON_ON);
          }
        }
        if (pausa == 2) {
          frenar = 1;
          digitalWrite(PIN_SALIDA, LOW); //APAGO LA BOMBA
          Funcion_Pantalla_Fin();
        }
        //***
      }
    } // Llave del while
    if (frenar == 0) {
      digitalWrite(PIN_SALIDA, LOW); //APAGO LA BOMBA
      Funcion_Pantalla_Fin();
    }
    delay(2500); // Muestro "FIN" en la pantalla por este tiempo.
    frenar = 0;
    comenzar = 0;
  } // Cierre del if
} // Cierre del loop

//--- Funcion para leer el estado del botón.
//--- La funcion devuelve 1 si se presiona el botón,
//--- devuelve 0 si no está presionado.

int Funcion_Leer_Boton (int codigo_boton) {
  int boton_presionado = 0, retorno = 0;
  //******
  if (digitalRead(BOTONES[codigo_boton]) == LOW)
  {
    boton_presionado = 1;
    while (digitalRead(BOTONES[codigo_boton]) == LOW)
      delay (5);
  }
  if (digitalRead(BOTONES[codigo_boton]) == HIGH && boton_presionado == 1)
  {
    boton_presionado = 0;
    retorno = 1;
  }
  return retorno;
}
//--- Fin de la función

//--- Función para mostrar en pantalla la cantidad de litros.
void Funcion_Pantalla(int litros) {
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 5);
  display.print("ECLIPSE");

  //Dibujamos Linea.
  display.drawLine(0, 30, 128, 30, SSD1306_WHITE);


  if (litros < 100) {
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(5, 40);
    display.print(litros);

    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(55, 40);
    if (litros > 1)
      display.print("LITROS");
    if (litros == 1)
      display.print("LITRO");
  }

  if (litros > 99) {
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(5, 40);
    display.print(litros);

    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(55, 40);
    display.print("LITROS");

  }


  display.display();
}
//--- Fin de la función

//--- Funcion para mostrar "FIN" en la pantalla.
void Funcion_Pantalla_Fin() {
  display.clearDisplay();
  display.setTextSize(5);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 10);
  display.print("FIN");
  display.display();
}
//--- Fin de la función

//--- Función para mostrar la pantalla con el contador.
void Funcion_Pantalla_Con_Reloj (int litros, int minutos, int segundos) {
  display.clearDisplay();
  //-
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Llenando:  ");
  display.print(litros);
  if (litros > 1)
    display.print(" litros");
  if (litros == 1)
    display.print(" litro");
  // TIEMPO RESTANTE
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 30);
  display.print("Tiempo restante:");
  // RELOJ
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(30, 42);
  if (minutos < 10)
    display.print("0");
  display.print(minutos);
  display.print(":");
  if (segundos < 10)
    display.print("0");
  display.print(segundos);
  // Enviar a pantalla
  display.display();
  return;
}
//--- Fin de la función

//--- Función que devuelve la cantidad de minutos.
int Funcion_Minutos(int tiempo) {
  int minutos = 0;
  minutos = tiempo / 60;
  return minutos;
}
//--- Fin de la función

//--- Función que devuelve la cantidad de segundos.
int Funcion_Segundos(int tiempo) {
  int minutos = 0;
  int segundos = 0;
  minutos = tiempo / 60;
  segundos = tiempo - (minutos * 60);
  return segundos;
}
//--- Fin de la función

//--- Función que muestra la pantalla con el sistema pausado.
void Funcion_Pantalla_Pausada (int minutos, int segundos) {
  display.clearDisplay();
  //-
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Sistema pausado!");
  display.println("Presione:");
  display.print(" ON para continuar");
  // TIEMPO RESTANTE
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 30);
  display.print("Tiempo restante:");
  // RELOJ
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(30, 42);
  if (minutos < 10)
    display.print("0");
  display.print(minutos);
  display.print(":");
  if (segundos < 10)
    display.print("0");
  display.print(segundos);
  // Enviar a pantalla
  display.display();
  return;
}
//--- Fin de la función

//******************************************
//--- Funcion para leer el estado del botón CON RETARDO.
//--- La funcion devuelve 1 si se presiona el botón
//--- La funcion devuelve 2 si me mantiene presionado un
// tiempo mayor a "retardo_ms"
//--- devuelve 0 si no está presionado.

int Funcion_Boton_Retardo (int codigo_boton) {

  int retardo_ms = RETARDO_REINICIO;
  int boton_presionado = 0, retorno = 0;
  int i;

  //******
  i = 0;

  if (digitalRead(BOTONES[codigo_boton]) == LOW)
  {
    boton_presionado = 1;
    while (digitalRead(BOTONES[codigo_boton]) == LOW && i < retardo_ms) {
      delay (1);
      i++;
    }
  }
  // Sin retardo
  if (boton_presionado == 1 && i < retardo_ms)
  {
    boton_presionado = 0;
    retorno = 1;
    while (digitalRead(BOTONES[codigo_boton]) == LOW)
      delay (1);
  }
  // Con Retardo
  if (boton_presionado == 1 && i == retardo_ms)
  {
    boton_presionado = 0;
    retorno = 2;
    //while (digitalRead(BOTONES[codigo_boton]) == LOW)
    delay (1);
  }
  return retorno;
}
//--- Fin de la función
//***********************************************

//***********************************************
//--- FUNCION BAJAR LITROS AUTOMATICOS
int Funcion_Bajar_Litros (int codigo_boton) {
  int retardo_ms = 1500;
  int boton_presionado = 0, retorno = 0;
  int i, x = 0;
  //******
  i = 0;
  if (digitalRead(BOTONES[codigo_boton]) == LOW)
  {
    boton_presionado = 1;
    if (litros > 1) {
      litros--;
      Funcion_Pantalla(litros);
    }
    while (digitalRead(BOTONES[codigo_boton]) == LOW && i < 500) {
      delay (1);
      i++;
    }
    //******
    if (i == 500) {
      while (digitalRead(BOTONES[codigo_boton]) == LOW) {
        delay (1);
        i++;

        if (i > 1000 && i < 3000) {
          x++;
          if (x == 300) {
            if (litros > 1) {
              litros--;
              Funcion_Pantalla(litros);
            }
            x = 0;
          }
        }
        if (i > 3000 && i < 8000) {
          if (x > 170)
            x = 0;
          x++;
          if (x == 170) {
            if (litros > 1) {
              litros--;
              Funcion_Pantalla(litros);
            }
            x = 0;
          }
        }
        if (i > 8000) {
          if (x > 100)
            x = 0;
          x++;
          if (x == 100) {
            if (litros > 1) {
              litros--;
              Funcion_Pantalla(litros);
            }
            x = 0;
          }
        }
      }
    }
    while (digitalRead(BOTONES[codigo_boton]) == LOW)
      delay (1);
  }
  return retorno;
}//llave del if principal
//--- Fin de la función
//***********************************************


//***********************************************
//--- FUNCION SUBIR LITROS AUTOMATICOS
int Funcion_Subir_Litros (int codigo_boton) {
  int retardo_ms = 1500;
  int boton_presionado = 0, retorno = 0;
  int i, x = 0;
  //******
  i = 0;
  if (digitalRead(BOTONES[codigo_boton]) == LOW)
  {
    boton_presionado = 1;
    litros++;
    Funcion_Pantalla(litros);

    while (digitalRead(BOTONES[codigo_boton]) == LOW && i < 500) {
      delay (1);
      i++;
    }
    //******
    if (i == 500) {
      while (digitalRead(BOTONES[codigo_boton]) == LOW) {
        delay (1);
        i++;

        if (i > 1000 && i < 3000) {
          x++;
          if (x == 300) {
            litros++;
            Funcion_Pantalla(litros);
            x = 0;
          }
        }
        if (i > 3000 && i < 8000) {
          if (x > 170)
            x = 0;
          x++;
          if (x == 170) {
            litros++;
            Funcion_Pantalla(litros);
            x = 0;
          }
        }
        if (i > 8000) {
          if (x > 100)
            x = 0;
          x++;
          if (x == 100) {
            litros++;
            Funcion_Pantalla(litros);
            x = 0;
          }
        }
      }
    }
    while (digitalRead(BOTONES[codigo_boton]) == LOW)
      delay (1);
  }
  return retorno;
}//llave del if principal

//--- Fin de la función
//***********************************************
