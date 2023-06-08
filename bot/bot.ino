#include "remote/remote.h"
#include <Servo.h>

RF24 radio_gugu(9, 10);

constexpr unsigned DATA_SIZE = 5;
int data[DATA_SIZE];

struct ServAngle
{
private:
  Servo servo_{};

public:
  void attach(int pin)
  {
    servo_.attach(pin);
  }

  int read()
  {
    return servo_.read();
  }

  auto write(int angle) -> decltype(servo_.write(-1))
  {
    return servo_.write(angle);
  }
} serv;

constexpr int SERVO_PIN = 9;

void setup()
{
  // Serial.begin(9600);

  // setup_radio(radio_gugu);
  // radio_gugu.openReadingPipe(1, pipeAddr);
  // radio_gugu.startListening();

  // motor pins
  for (int i = 4; i < 8; ++i)
    pinMode(i, OUTPUT);

  // Serial.println("RADIO GUGU START");

  serv.attach(SERVO_PIN);
}

constexpr int SPEED_1 = 5;
constexpr int DIR_1 = 4;
constexpr int SPEED_2 = 6;
constexpr int DIR_2 = 7;

constexpr int DEFAULT_ANGLE = 100;
constexpr int ROT_ANGLE = 50;
constexpr int RIGHT_ROT = DEFAULT_ANGLE + ROT_ANGLE;
constexpr int LEFT_ROT = DEFAULT_ANGLE - ROT_ANGLE;

void loop()
{
  digitalWrite(DIR_1, HIGH);
  analogWrite(SPEED_1, 255);
  digitalWrite(DIR_2, HIGH);
  analogWrite(SPEED_2, 255);

  auto curAngle = 0;//serv.read();
  auto rotAngle = 180;
  static int rotated = 0;
  if (!rotated) {
    serv.write(RIGHT_ROT);
    rotated = 1;
  }
  // ang *= -1;
  delay(3000);

  if (!radio_gugu.available())
    return;

  radio_gugu.read(&data, sizeof(data));
  Serial.println("RADIO GUGU");

  auto btnState = data[2];
  auto tmblrState = data[3];
  auto ptmrVal = data[4];

  processDir(data[0], data[1]);
}

void setEngine(int dir1, int dir2, int spd)
{
  digitalWrite(DIR_1, dir1);
  analogWrite(SPEED_1, spd);

  digitalWrite(DIR_2, dir2);
  analogWrite(SPEED_2, spd);
}

void processDir(int dir, int spd)
{
  switch (dir)
  {
  case Dir::FORWARD:
    setEngine(HIGH, LOW, spd);
    break;
  case Dir::BACKWARD:
    setEngine(LOW, HIGH, spd);
    break;
  case Dir::RIGHT:
    setEngine(HIGH, HIGH, spd);
    break;
  case Dir::LEFT:
    setEngine(LOW, LOW, spd);
    break;
  default:
    setEngine(HIGH, HIGH, 0);
    Serial.println("No direction");
  }
}
