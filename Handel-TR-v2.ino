#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const uint64_t pipe01 = 0xF0F1F2F3F4LL; // индитификатор передачи, "труба"
const uint64_t pipe02 = 0xF0F1F2F3F1LL; // могут различатся только на младший байт

RF24 radio(9, 10); // CE, CSN
short int data[5];
void setup() {
  Serial.begin(9600);

  radio.begin();
  delay(2);
  radio.setChannel(9); // канал (0-127)

  // скорость, RF24_250KBPS, RF24_1MBPS или RF24_2MBPS
  // RF24_250KBPS на nRF24L01 (без +) неработает.
  // меньше скорость, выше чувствительность приемника.
  radio.setDataRate(RF24_250KBPS);

  // мощьность передатчика RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM,
  radio.setPALevel(RF24_PA_HIGH);

  radio.openWritingPipe(pipes02);
  radio.openReadingPipe(1, pipe01); // открываем первую трубу с индитификатором "pipe"
  //radio.openReadingPipe(2, pipe02); // открываем трубу с индитификатором "pipe02"
  //  radio.openReadingPipe(0,pipe01); // или открываем все трубы разом

  radio.startListening(); // включаем приемник, начинаем слушать трубу
}

// radio.stopListening(); // останавливает приём (нужно перед началом передачи)

void loop()
{
  if (Serial.available()) {
    char ch = Serial.read();
    if (ch == 'T') {
      radio.stopListening();

      radio.write(&ch, sizeof(ch));
      radio.startListening();
    }
  } else {
    if (radio.available()) { // проверяем не пришло ли чего в буфер.
      radio.read(&data, sizeof(data)); // читаем данные, указываем сколько байт читать
      Serial.print(data[0]);
      Serial.print(" ");
      Serial.print(data[1]);
      Serial.print(" ");
      Serial.print(data[2]);
      Serial.print(" ");
      Serial.print(data[3]);
      Serial.print(",");
      Serial.flush();
      delay(2);
    }
  }
}
