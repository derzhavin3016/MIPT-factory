#include "remote/remote.h"
#include <Servo.h>

RF24 radio_gugu(9, 10);

constexpr unsigned DATA_SIZE = 5;
int data[DATA_SIZE];// = {-1, -1};

struct ServAngle
{
private:
  Servo servo_{};

public:
  void attach(int pin)
  {
    servo_.attach(pin);
  }

  void noRotate()
  {
    servo_.write(DEFAULT_ANGLE);
  }

  void rotateRight()
  {
    servo_.write(RIGHT_ROT);
  }

  void rotateLeft()
  {
    servo_.write(LEFT_ROT);
  }

  void rotate(int angle)
  {
    if (angle <= RIGHT_ROT && angle >= LEFT_ROT)
      servo_.write(angle);
  }
} serv;

constexpr int SERVO_PIN = 8;

void setup()
{
  Serial.begin(9600);

  setup_radio(radio_gugu);
  radio_gugu.openReadingPipe(1, pipeAddr);
  radio_gugu.startListening();

  // motor pins
  for (int i = 4; i < 8; ++i)
    pinMode(i, OUTPUT);

  Serial.println("RADIO GUGU START");

  serv.attach(SERVO_PIN);
}

constexpr int SPEED_1 = 5;
constexpr int DIR_1 = 4;
constexpr int SPEED_2 = 6;
constexpr int DIR_2 = 7;

void loop()
{
  static int def = 0;
  if (!def)
    serv.noRotate(), def = 1;

  if (!radio_gugu.available())
    return;

  radio_gugu.read(&data, sizeof(data));
  Serial.println("RADIO GUGU");
  Serial.println(data[0]);
  Serial.println(data[1]);

  auto rot_angle = data[2];
  auto tmblrState = data[3];
  auto ptmrVal = data[4];

  processDir(data[0], data[1]);
  serv.rotate(rot_angle);
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
    setEngine(LOW, LOW, spd);
    break;
  case Dir::BACKWARD:
    setEngine(HIGH, HIGH, spd);
    break;
  case Dir::RIGHT:
    setEngine(HIGH, LOW, spd);
    break;
  case Dir::LEFT:
    setEngine(LOW, HIGH, spd);
    break;
  default:
    setEngine(HIGH, HIGH, 0);
    Serial.println("No direction");
  }
}
