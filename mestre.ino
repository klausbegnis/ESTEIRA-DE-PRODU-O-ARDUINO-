// MESTRE
//

#define LEDR 13
#define LEDG 11
#define cc 9
#define servoPin 12
//biblioteca p/ micro servo
#include <Servo.h>
Servo myservo;

byte volatile estado = 0; 
int pos = 0;
int pausa = 2;
int inicio = 3;
volatile char c;

void setup()
{
  //configurações de pino
  pinMode(pausa, OUTPUT);
  pinMode(inicio, OUTPUT);
  pinMode(cc, OUTPUT);
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  
  //configurações de registradores
  config_serial();
  interrupcoes();
  
  //inicia o servo
  myservo.attach(servoPin);
  myservo.write(pos);
}

void loop()
{
  //checa o sensor
  if (check_receive())
  {
    c = UDR0; //LEITURA
    if (c == 'S' and estado == 1)
    {
      //libera próxima peça
      myservo.write(45);
      delay(500);
      myservo.write(0);
    }
  }
  
  // atualiza o estado 
  if (estado == 0){
    parado();
  }
  else
    operando();
  
}

void interrupcoes()
{
  //interruções RISING
  EICRA = (1 << ISC11) | (1 << ISC10) 
        | (1 << ISC01) | (1 << ISC00); 
  
  // habilita INT1 e INT0
  EIMSK = (1 << INT1) | (1 << INT0); 
  
  //INT0 -> STOP
  //INT1 -> START
}

// ações do modo parado
void parado()
{
  troca_cor(false);
  digitalWrite(cc,0);
}

// ações do modo operando
void operando()
{
  troca_cor(true);
  digitalWrite(cc,1);
}

// atualiza o led
void troca_cor(bool modo)
{
  if (modo == true)
  {
    digitalWrite(LEDR, 0);
    digitalWrite(LEDG, 1); 
  }
  else
  {
    digitalWrite(LEDG, 0);
    digitalWrite(LEDR, 1);
  }
}

bool check_receive() 
{
  if (UCSR0A & (1 << RXC0)) // testa o bit de recebimento
  	return true;
  else
  	return false;
}

void config_serial()
{
  UBRR0 = 103; // Taxa de 9600
  // Liga    transmissor |    receptor
  UCSR0B = ( 1 << TXEN0 ) | ( 1 << RXEN0 ); 
  UCSR0C = ( 1 << UCSZ00 ); // 8 bits de pacotes
  UCSR0C |= ( 1 << UCSZ01 ); 
}


// interrupção do botão stop
ISR(INT0_vect)
{
  estado = 0;
  UDR0 = 'D';
}

// interrupção do botão start
ISR(INT1_vect)
{
  estado = 1;
  UDR0 = 'L';
}






