#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

#define PIN_SALIDA D8 // Colocar en "D3" el pin que queremos utilizar como salida
#define RETARDO_REINICIO 2000

#define ANCHO_PANTALLA 128 // Ancho pantalla OLED
#define ALTO_PANTALLA 64 // Alto pantalla OLED
Adafruit_SSD1306 display(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, -1);

#define CANT_BOTONES 4
//{ Subir , Bajar , ON , OFF }
int BOTONES[CANT_BOTONES] = {D5, D6, D7};

#define BOTON_SUBIR 0
#define BOTON_BAJAR 1
#define BOTON_ON 2

int litros = 5;
float tiempo_litro;
int dir_litros = 5;
int dir_tiempo = 0;

void setup() {
  Serial.begin(9600);
  EEPROM.begin(512);
  pinMode(D5, INPUT_PULLUP);
  pinMode(D6, INPUT_PULLUP);
  pinMode(D7, INPUT_PULLUP);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  EEPROM.get(dir_litros, litros);
  if (litros < 1 || litros > 999)
    litros = 10;

  EEPROM.get(dir_tiempo, tiempo_litro);
  if (tiempo_litro < 1 || tiempo_litro > 999)
    tiempo_litro = 5.00;


}

void loop() {
  Funcion_Pantalla(litros);
  Funcion_Boton_Retardo(BOTON_SUBIR, BOTON_BAJAR);
  Funcion_Boton_Inicio();

}

//******************************************
//--- Funcion para leer el estado del botón CON/SIN RETARDO.
//--- La funcion devuelve 1 si se presiona el botón
//--- La funcion devuelve 2 si me mantiene presionado un
// tiempo mayor a "retardo_ms"
//--- devuelve 0 si no está presionado.

void Funcion_Boton_Retardo (int boton1, int boton2) {

  int retardo_ms = 1500; // Cantidad de milisegundos que quiero
  //que espere cuando mantengo presionado el boton.
  int boton_presionado = 0, retorno = 0;
  int i = 0;
  int uno, dos, x;
  uno = 0;
  dos = 0;

  //Miro si estan ambos presionados al mismo tiempo
  if (digitalRead(BOTONES[boton1]) == LOW && digitalRead(BOTONES[boton2]) == LOW) {
    while (digitalRead(BOTONES[boton1]) == LOW && digitalRead(BOTONES[boton2]) == LOW && dos < retardo_ms) {
      delay(1);
      dos++;
    }
    if (dos == retardo_ms) { //Se mantuvieron los dos presionados x segundos
      //carpeta menu
      Funcion_Menu();
    }
  } else if (digitalRead(BOTONES[boton1]) == LOW) {
    Funcion_Boton_Subir(boton1, boton2);

  } else if (digitalRead(BOTONES[boton2]) == LOW) {
    Funcion_Boton_Bajar(boton1, boton2);

  }
  delay(50);
  //*****************
  //} //cierre del if del boton presionado.
}
//--- Fin de la función


//************ FUNCION SUBIR ***********************
void Funcion_Boton_Subir(int boton1, int boton2) {
  int flag = 0;
  int retardo = 400;
  int uno = 0;
  int x, i;
  i = 0;
  x = 0;

  //*** Subo litros +1
  if (litros < 999) {
    litros++;
    Funcion_Pantalla(litros);
  }
  //***

  while (digitalRead(BOTONES[boton1]) == LOW && digitalRead(BOTONES[boton2]) == HIGH && uno < retardo) {
    delay(1);
    uno++;
  }

  if (uno == retardo) { //Se presiono uno solo
    //cont automatico.

    //****************************** conteo automatico
    while (digitalRead(BOTONES[boton1]) == LOW && flag == 0) {
      delay (1);
      i++;
      if (digitalRead(BOTONES[boton1]) == LOW && digitalRead(BOTONES[boton2]) == LOW) {
        flag = 1;
        Funcion_Boton_Retardo (boton1, boton2);
      } else {
        if (i > 1000 && i < 3000) {
          x++;
          if (x == 300) {
            if (litros < 999)
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
            if (litros < 999)
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
            if (litros < 999)
              litros++;
            Funcion_Pantalla(litros);
            x = 0;
          }
        }
      } //llave else
    } //llave while

    //******************************
  } // if uno>400
} //cierre funcion

//**************************************************
//************ FUNCION BAJAR ***********************
void Funcion_Boton_Bajar(int boton1, int boton2) {
  int flag = 0;
  int retardo = 400;
  int uno = 0;
  int x, i;
  i = 0;
  x = 0;

  //*** Resto litros -1
  if (litros > 1) {
    litros--;
    Funcion_Pantalla(litros);
  }
  //***

  while (digitalRead(BOTONES[boton2]) == LOW && digitalRead(BOTONES[boton1]) == HIGH && uno < retardo) {
    delay(1);
    uno++;
  }

  if (uno == retardo) { //Se presiono uno solo
    //cont automatico.

    //*** conteo automatico
    while (digitalRead(BOTONES[boton2]) == LOW && flag == 0) {
      delay (1);
      i++;
      if (digitalRead(BOTONES[boton1]) == LOW && digitalRead(BOTONES[boton2]) == LOW) {
        flag = 1;
        Funcion_Boton_Retardo (boton1, boton2);
      } else {
        if (i > 1000 && i < 3000) {
          x++;
          if (x == 300) {
            if (litros > 1)
              litros--;
            Funcion_Pantalla(litros);
            x = 0;
          }
        }
        if (i > 3000 && i < 8000) {
          if (x > 170)
            x = 0;
          x++;
          if (x == 170) {
            if (litros > 1)
              litros--;
            Funcion_Pantalla(litros);
            x = 0;
          }
        }
        if (i > 8000) {
          if (x > 100)
            x = 0;
          x++;
          if (x == 100) {
            if (litros > 1)
              litros--;
            Funcion_Pantalla(litros);
            x = 0;
          }
        }
      } //llave else
    } //llave while

    //************
  } // if uno>400
} //cierre funcion

//***************************************************
//***************************************************
void Pantalla_Menu() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(25, 5);
  display.print("CONFIGURACION");

  //Dibujamos Linea.
  display.drawLine(0, 20, 128, 20, SSD1306_WHITE);

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(35, 30);
  display.print(tiempo_litro);

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 50);
  display.print("segundos por litro.");

  display.display();
}
//***********************************************
void Funcion_Pantalla(int litros) {
  //---------
  display.clearDisplay();

  //Titulo:
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 0);
  display.print("ECLIPSE");

  //Dibujamos Linea.
  display.drawLine(0, 18, 128, 18, SSD1306_WHITE);

  //Litros:
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(5, 28);
  display.print(litros);

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(55, 28);
  if (litros > 1)
    display.print("LITROS");
  if (litros == 1)
    display.print("LITRO");


  //Segundos por litro
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(5, 55);
  display.print(tiempo_litro);

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(55, 55);
  display.print("seg / litro");

  display.display();
}
//--- Fin de la función
//***********************************************
//*** Funcion Menu ***
void Funcion_Menu() {
  Pantalla_Menu();
  while (digitalRead(BOTONES[BOTON_SUBIR]) == LOW || digitalRead(BOTONES[BOTON_BAJAR]) == LOW)
    delay(1);

  while (digitalRead(BOTONES[BOTON_ON]) != LOW) {
    Subir_Tiempo();
    delay(15);
    Bajar_Tiempo();
    delay(15);
  }

  while (digitalRead(BOTONES[BOTON_ON]) == LOW)
    delay(1);
  Funcion_Pantalla(litros);
  //Guardar valor en la EEPROM:
  EEPROM.put(dir_tiempo, tiempo_litro);
  EEPROM.commit();

}
//*********************
//************ FUNCION SUBIR TIEMPO ***********************
void Subir_Tiempo () {
  int retardo_ms = 1500;
  int boton_presionado = 0, retorno = 0;
  int i, x = 0;
  //******
  i = 0;
  if (digitalRead(BOTONES[BOTON_SUBIR]) == LOW)
  {
    boton_presionado = 1;
    tiempo_litro += 0.1 ;
    Pantalla_Menu();

    while (digitalRead(BOTONES[BOTON_SUBIR]) == LOW && i < 500) {
      delay (1);
      i++;
    }
    //******
    if (i == 500) {
      while (digitalRead(BOTONES[BOTON_SUBIR]) == LOW) {
        delay (1);
        i++;

        if (i > 1000 && i < 3000) {
          x++;
          if (x == 300) {
            tiempo_litro += 0.1 ;
            Pantalla_Menu();
            x = 0;
          }
        }
        if (i > 3000 && i < 8000) {
          if (x > 170)
            x = 0;
          x++;
          if (x == 170) {
            tiempo_litro += 0.1 ;
            Pantalla_Menu();
            x = 0;
          }
        }
        if (i > 8000) {
          if (x > 100)
            x = 0;
          x++;
          if (x == 100) {
            tiempo_litro += 0.1 ;
            Pantalla_Menu();
            x = 0;
          }
        }
      }
    }
    while (digitalRead(BOTONES[BOTON_SUBIR]) == LOW)
      delay (1);
  }

}//llave del if principal

//--- Fin de la función
//***********************************************
//************ FUNCION BAJAR TIEMPO ***********************
void Bajar_Tiempo () {
  int retardo_ms = 1500;
  int boton_presionado = 0, retorno = 0;
  int i, x = 0;
  //******
  i = 0;
  if (digitalRead(BOTONES[BOTON_BAJAR]) == LOW)
  {
    boton_presionado = 1;
    tiempo_litro -= 0.1 ;
    Pantalla_Menu();

    while (digitalRead(BOTONES[BOTON_BAJAR]) == LOW && i < 500) {
      delay (1);
      i++;
    }
    //******
    if (i == 500) {
      while (digitalRead(BOTONES[BOTON_BAJAR]) == LOW) {
        delay (1);
        i++;

        if (i > 1000 && i < 3000) {
          x++;
          if (x == 300) {
            tiempo_litro -= 0.1 ;
            Pantalla_Menu();
            x = 0;
          }
        }
        if (i > 3000 && i < 8000) {
          if (x > 170)
            x = 0;
          x++;
          if (x == 170) {
            tiempo_litro -= 0.1 ;
            Pantalla_Menu();
            x = 0;
          }
        }
        if (i > 8000) {
          if (x > 100)
            x = 0;
          x++;
          if (x == 100) {
            tiempo_litro -= 0.1 ;
            Pantalla_Menu();
            x = 0;
          }
        }
      }
    }
    while (digitalRead(BOTONES[BOTON_BAJAR]) == LOW)
      delay (1);
  }
}//llave del if principal

//--- Fin de la función

//********************************************
//*** FUNCION BOTON INICIO ***
void Funcion_Boton_Inicio() {
  // Cuando se pulsa el botón comenzar
  int i = 0, frenar = 0, comenzar = 0, pausa = 0, aux = 0;
  float tiempo_restante = 0;
  int minutos = 0, segundos = 0;

  if (digitalRead(BOTONES[BOTON_ON]) == LOW) {
    EEPROM.put(dir_litros, litros);
    EEPROM.commit();

    tiempo_restante = (float) tiempo_litro * litros;

    minutos = (int) tiempo_restante / 60;

    segundos = (int) (((tiempo_restante / 60) - minutos) * 60);

    /*
        Serial.print("Litros:");
        Serial.println(litros);
        Serial.print("Tiempo por litro:");
        Serial.println(tiempo_litro);
        Serial.print("Tiempo_restante:");
        Serial.println(tiempo_restante);
        Serial.print("Minutos:");
        Serial.println(minutos);
        Serial.print("Segundos:");
        Serial.println(segundos);
    */

    while ((frenar == 0) && tiempo_restante > 0) {

      digitalWrite(PIN_SALIDA, HIGH); // Enciendo la bomba



      if (segundos != 0)
        segundos--;

      if (segundos == 0) {
        segundos = 59;
        if (minutos != 0)
          minutos--;
        else if (minutos == 0) {
          tiempo_restante = 0;
          segundos = 0;

        }
      }

      Funcion_Pantalla_Con_Reloj(litros, minutos, segundos);

      while (digitalRead(BOTONES[BOTON_ON]) == LOW)
        delay(1);

      // Delay de 1 segundo, verificando si se presiona el BOTON_OFF
      i = 1000;
      while (frenar == 0 && i > 0) {
        i--;
        delay(1);
        pausa = 0;
        aux = 0;
        pausa = Funcion_Boton_Retardo2(BOTON_ON);
        //***
        if (pausa == 1) { // Se presiono sin retardo
          pausa = 0;
          delay(5);
          while (pausa == 0) {
            // Se presionó el botón de pausa.
            delay(1);
            digitalWrite(PIN_SALIDA, LOW);
            Funcion_Pantalla_Pausada (minutos, segundos);
            pausa = Funcion_Boton_Retardo2(BOTON_ON);
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



}

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

  //Segundos por litro
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(5, 15);
  display.print(tiempo_litro);

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(55, 15);
  display.print("seg / litro");

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

int Funcion_Boton_Retardo2 (int codigo_boton) {

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
