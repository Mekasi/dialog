#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#define SelfTest 5
#define zg 6
#define sl 7
#define led 2
#define gSelect 8
short int x, y, z, p;
const uint64_t pipe = 0xF0F1F2F3F4LL;
RF24 radio(9, 10);

short int data [4];
void setup()
{
  Serial.begin(9600);
  pinMode(sl, OUTPUT);
  pinMode(gSelect, OUTPUT);
  pinMode(led, INPUT);
  digitalWrite(sl, HIGH);
  digitalWrite(gSelect, LOW);
  radio.begin();
  delay(2);
  radio.setChannel(9); // канал (0-127)
  // скорость, RF24_250KBPS, RF24_1MBPS или RF24_2MBPS
  // RF24_250KBPS на nRF24L01 (без +) неработает.
  // меньше скорость, выше чувствительность приемника.
  radio.setDataRate(RF24_250KBPS);
  // мощьность передатчика, RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM,
  radio.setPALevel(RF24_PA_HIGH);
  radio.openWritingPipe(pipe); // открываем трубу на передачу.
}

short int AVal1() {
  x = analogRead(A0);
  y = analogRead(A1);
  z = analogRead(A2);

  return x, y, z;
}

short int AVal2() {
  x = fabs((x - analogRead(A0) ) ) ;
  y = fabs((y - analogRead(A1) ) ) ;
  z = fabs((z - analogRead(A2) ) ) ;
  return x, y, z;
}

short int PVal() {
  p = analogRead(A3);
  return p;
}

void loop()
{

  AVal1();//значения акселерометра
  PVal();//значение пульсометра
  data[0] = p;
  radio.write(&data, sizeof(data));
  AVal2();//разница значений акселерометра
  if (x > 15 || y > 15 || z > 15) {
    data[0] = p;
    data[1] = x;
    data[2] = y;
    data[3] = z;
    radio.write(&data, sizeof(data));
  } else {
    PVal();
    data[0] = p;
    radio.write(&data, sizeof(data));
  };
  Serial.print(data[0]);
  Serial.print(" ");
  Serial.print(data[1]);
  Serial.print(" ");
  Serial.print(data[2]);
  Serial.print(" ");
  Serial.print(data[3]);
  Serial.print(",");
  delay(2);
}
