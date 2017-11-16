//  Henry's Bench IR Remote Tutorial 2

//https://github.com/z3t0/Arduino-IRremote
#include <IRremote.h>

//https://bitbucket.org/teckel12/arduino-new-tone/wiki/Home
#include <NewTone.h>

//https://github.com/jfturcot/SimpleTimer
#include <SimpleTimer.h>

// the timer object

SimpleTimer timer;

int IR_PIN = 2;

IRrecv irDetect(IR_PIN);

decode_results irIn;

int piezoPin = 13;
int pinD12FiltroAr = 12;
int pinD11Geral = 11;
int pinD10VentiladorPrincipalVelocidade_1 = 10;
int pinD9VentiladorPrincipalVelocidade_2 = 9;
int pinD8VentiladorPrincipalVelocidade_3 = 8;
int pinD7BombaAguaCirculacao = 7;
int pinD6OsciladorVentilador = 6;
int pinD5Ozonizador = 5;
int pinD3NivelAgua = 3;


boolean chaveGeralLigado = false;
boolean bombaAguaLigado = false;
boolean osciladorVentiladorLigado = false;
boolean ozonizadorLigado = false;

void setup()
{
  //Serial.begin(9600);
  
  irDetect.enableIRIn(); // Start the Receiver

    // initialize digital pin LED_BUILTIN as an output.
//  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(piezoPin,  OUTPUT); 
  pinMode(pinD12FiltroAr,  INPUT_PULLUP); 
  pinMode(pinD11Geral, OUTPUT);
  pinMode(pinD10VentiladorPrincipalVelocidade_1, OUTPUT);
  pinMode(pinD9VentiladorPrincipalVelocidade_2, OUTPUT);
  pinMode(pinD8VentiladorPrincipalVelocidade_3, OUTPUT);
  pinMode(pinD7BombaAguaCirculacao, OUTPUT);
  pinMode(pinD6OsciladorVentilador, OUTPUT);
  pinMode(pinD5Ozonizador, OUTPUT);
  pinMode(pinD3NivelAgua,  INPUT_PULLUP);

  desligaTudo();

  alarmConectadoTomada();

  // a cada 5 secodos verifica o reservatorio de agua
  timer.setInterval(5000, verificaNivelReservatorio);

  // a cada 3 segundos verifica se o filtro esta encaixado no climatizador
  timer.setInterval(3000, verificaFiltroArEncaixado);

}

void loop() {
  if (irDetect.decode(&irIn)) {
    decodeIR();
    irDetect.resume(); 
  }

  timer.run();
  
}


void ligaVentiladorPrincipalVelocidade_1() {
  if (chaveGeralLigado == true) {
   alarmFuncaoAcionada(); 
   digitalWrite(pinD10VentiladorPrincipalVelocidade_1, LOW);
   digitalWrite(pinD9VentiladorPrincipalVelocidade_2, HIGH);
   digitalWrite(pinD8VentiladorPrincipalVelocidade_3, HIGH);
  }
}

void ligaVentiladorPrincipalVelocidade_2() {
  if (chaveGeralLigado == true) {
   alarmFuncaoAcionada();
   digitalWrite(pinD10VentiladorPrincipalVelocidade_1, HIGH);
   digitalWrite(pinD9VentiladorPrincipalVelocidade_2, LOW);
   digitalWrite(pinD8VentiladorPrincipalVelocidade_3, HIGH);
  }
}

void ligaVentiladorPrincipalVelocidade_3() {
  if (chaveGeralLigado == true) {
   alarmFuncaoAcionada(); 
   digitalWrite(pinD10VentiladorPrincipalVelocidade_1, HIGH);
   digitalWrite(pinD9VentiladorPrincipalVelocidade_2, HIGH);
   digitalWrite(pinD8VentiladorPrincipalVelocidade_3,LOW);
  }
}


void ligaBombaAguaCirculacao() {
  if (chaveGeralLigado == true) {
    if (bombaAguaLigado == false) {
      boolean reservatorioVazio = digitalRead(pinD3NivelAgua);
      
      if (reservatorioVazio) {
        alarmeReservatorioVazio();
      } else {
        digitalWrite(pinD7BombaAguaCirculacao,LOW);
        bombaAguaLigado = true;
        delay(50);
      }

    } else {
      desligaBombaAguaCirculacao();
      bombaAguaLigado = false;
    }
    alarmFuncaoAcionada(); 
  }
}

void desligaBombaAguaCirculacao() {
  digitalWrite(pinD7BombaAguaCirculacao,HIGH);
}

void ligaOsciladorVentilador() {
  if (chaveGeralLigado == true ) {
    if (osciladorVentiladorLigado == false) {
      digitalWrite(pinD6OsciladorVentilador,LOW);
      osciladorVentiladorLigado = true;
      delay(50);
    } else {
      digitalWrite(pinD6OsciladorVentilador,HIGH);
      osciladorVentiladorLigado = false;
    }
    alarmFuncaoAcionada();
  }
}

void ligaOzonizador() {
  if (chaveGeralLigado == true ) {
    if (ozonizadorLigado == false) {
      digitalWrite(pinD5Ozonizador,LOW);
      ozonizadorLigado = true;
      delay(50);
    } else {
      digitalWrite(pinD5Ozonizador,HIGH);
      ozonizadorLigado = false;
    }
    alarmFuncaoAcionada();
  }
}

void desligaTudo() {
    //inicialmente deixa tudo desligado
  digitalWrite(pinD11Geral, HIGH);
  digitalWrite(pinD10VentiladorPrincipalVelocidade_1, HIGH);
  digitalWrite(pinD9VentiladorPrincipalVelocidade_2, HIGH);
  digitalWrite(pinD8VentiladorPrincipalVelocidade_3, HIGH);
  digitalWrite(pinD7BombaAguaCirculacao, HIGH);
  digitalWrite(pinD6OsciladorVentilador, HIGH);
  digitalWrite(pinD5Ozonizador, HIGH);  

  chaveGeralLigado = false;
  bombaAguaLigado = false;
  osciladorVentiladorLigado = false;
  ozonizadorLigado = false;
}


void iniciaVentilador() {
  if (chaveGeralLigado == false ) {
      //liga o geral
      digitalWrite(pinD11Geral, LOW); 
      chaveGeralLigado = true;
      //inicia o ventilador na velocidade 1
      ligaVentiladorPrincipalVelocidade_1();
    } else {
      alarmFuncaoAcionada();
      digitalWrite(pinD11Geral, HIGH); 
      //desliga todos os reles
      desligaTudo();
    }
}

void verificaNivelReservatorio () {
  
  boolean reservatorioVazio = digitalRead(pinD3NivelAgua);
  
  if (reservatorioVazio) {
    if (bombaAguaLigado) {
      desligaBombaAguaCirculacao();
      alarmeReservatorioVazio();    
    }
  }

}

void verificaFiltroArEncaixado() {

  boolean state = digitalRead(pinD12FiltroAr);
 
  if (state) {
    desligaTudo();
    alarmeFiltroArNaoEncaixado();
  } 

}

void alarmeFiltroArNaoEncaixado() {
 for (int thisNote = 0; thisNote < 10; thisNote++) {
     NewTone(piezoPin, 1500, 80); // Play thisNote for noteDuration.
     delay(200);
  }
}

void alarmeReservatorioVazio() {
  for (int thisNote = 0; thisNote < 10; thisNote++) {
     NewTone(piezoPin, 1500, 500); // Play thisNote for noteDuration.
     delay(750);
  }
}

void alarmConectadoTomada() {
  for (int thisNote = 0; thisNote < 2; thisNote++) {
     NewTone(piezoPin, 1500, 100); // Play thisNote for noteDuration.
     delay(250);
  }
}

void alarmFuncaoAcionada() {
  NewTone(piezoPin, 1500, 100); // Play thisNote for noteDuration.
}

void decodeIR() // Indicate what key is pressed

{

  delay(50);

  switch(irIn.value)

  {

  case 0xFF629D:  
    {
      //Serial.println("Up Arrow"); 
      iniciaVentilador();         
    }
    break;

  case 0xFF22DD:  
    {
      //Serial.println("Left Arrow"); 
      ligaBombaAguaCirculacao();
    }
    break;

  case 0xFF02FD:  
    {
      //Serial.println("OK"); 
      
    }
    break;

  case 0xFFC23D:  
    {
      //Serial.println("Right Arrow"); 
      ligaOsciladorVentilador();
    }
    break;

  case 0xFFA857:  
    {	
      //Serial.println("Down Arrow"); 
      ligaOzonizador();
    }
    break;

  case 0xFF6897:  
    {
      //Serial.println("1"); 
      ligaVentiladorPrincipalVelocidade_1();
    }
    break;

  case 0xFF9867:  
    {
      //Serial.println("2"); 
      ligaVentiladorPrincipalVelocidade_2();
    }
    break;

  case 0xFFB04F:  
    {
      //Serial.println("3"); 
      ligaVentiladorPrincipalVelocidade_3();
    }
    break;

//  case 0xFF30CF:  
//    Serial.println("4");
//    break;
//
//  case 0xFF18E7:  
//    Serial.println("5"); 
//    break;
//
//  case 0xFF7A85:  
//    Serial.println("6"); 
//    break;
//
//  case 0xFF10EF:  
//    Serial.println("7"); 
//    break;
//
//  case 0xFF38C7:  
//    Serial.println("8"); 
//    break;
//
//  case 0xFF5AA5:  
//    Serial.println("9"); 
//    break;
//
//  case 0xFF42BD: 
//    Serial.println("*"); 
//    break;
//
//  case 0xFF4AB5:  
//    Serial.println("0"); 
//    break;
//
//  case 0xFF52AD:  
//    Serial.println("#");      
//    break; 

  default: 
   break;

  }



}

