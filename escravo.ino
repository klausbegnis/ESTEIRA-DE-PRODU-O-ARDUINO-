// ESCRAVO

//funções do lcd -- tempo contagem modo
  // T: 11111 contagem de tempo 
  // M:0/1 modo
  // C: 11111 contagem de produtos que passaram pelo último led
//SERVO
//RX PRO MODO 
//TX SENSOR

#include <LiquidCrystal.h> 
byte volatile sensor_out = 0;
byte volatile estado = 0;
volatile char c;

int tempo = 0;
LiquidCrystal lcd(5, 6, 10, 9, 8, 7);

void setup()
{
  //inicia o lcd
  lcd.begin(16,2);			
  lcd.setCursor(0,0);
  atualiza_lcd();
  
  //configuração dos registradores
  config_serial();			
  interrupcoes();			
  temporizadores();			
}

void loop()
{
  if (check_receive()); //checa se chegou informação
  {
    c = UDR0; //LEITURA
    
    // se estiver ligado
    if(c == 'L'){
      estado=1;
      atualiza_lcd();
    }
    
    // se estiver desligado
    if(c == 'D'){
      estado=0;
      tempo=0;
      sensor_out=0;
      atualiza_lcd();
    }
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

void interrupcoes()
{
 //interruções RISING
  EICRA = (1 << ISC01) | (1 << ISC00); 
  
  // habilita INT0
  EIMSK = (1 << INT0);
}  

void temporizadores()
{
  TCCR1A = 0; 
  TCCR1B  = ( 1 << CS12 );  // Prescaler em 256
  TCCR1B |= ( 1 << WGM12);  // Modo CTC
  
  OCR1A   = 62500 - 1;      // interrupt no valor de 62499  
  TCNT1 = 0; 
  TIMSK1 = ( 1 << OCIE1A );  // comparação com A
}

void atualiza_lcd()
{
  //variáveis de escopo
  int horas;
  int minutos;
  int segundos;
  lcd.setCursor(0,0);
  lcd.print("T:");
  
  //cálculo do tempo
  horas = tempo / 3600;
  minutos = (tempo % 3600) / 60;
  segundos = ((tempo % 3600) % 60);
  
  //atualiza o lcd p/ tempo
  if (horas < 10)
  	lcd.print("0");
    
  lcd.print(horas);
  
  lcd.print(":");
  
  if (minutos < 10)
    lcd.print("0");
  
  lcd.print(minutos);
  
  lcd.print(":");
  
  if (segundos < 10)
    lcd.print("0");
  
  lcd.print(segundos);
  
  //atualiza o modo
  lcd.setCursor(0,2);
  lcd.print("M:");
  lcd.print(estado);
  
  lcd.print(" ");
  
  //atualiza a contagem
  lcd.print("C:");
  lcd.print(sensor_out);
  
}

//interrupção do sensor
ISR(INT0_vect)
{
  if (estado > 0)
  {
  	sensor_out += 1;
    UDR0 = 'S';
  	atualiza_lcd();
  }
}

//interrupção de 1 segundo
ISR(TIMER1_COMPA_vect)
{
  if (estado > 0)
  { 
  	tempo +=1;
    atualiza_lcd();
  }
}
