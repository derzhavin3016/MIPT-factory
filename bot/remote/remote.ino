#include "remote.h"

RF24 radio_gaga(9, 10);
int data[5];

constexpr int xPin = A0;
constexpr int yPin = A1;
constexpr int buttonPin = 4;
constexpr int ptmrPin = A4;
constexpr int tumblrPin = 0;

int xPosition = 0;
int yPosition = 0;
int buttonState = 0;
int tumblrState = 0;
int ptmrState = 0;

int speed = 0;
int direction = 0;

void setup()
{
  Serial.begin(9600);
  setup_radio(radio_gaga);
  radio_gaga.openWritingPipe(pipeAddr);

  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(tumblrPin, INPUT);
  pinMode(ptmrPin, INPUT);
}
constexpr int DEAD_HIGH = 562;
constexpr int DEAD_LOW = 462;

void loop()
{
  xPosition = analogRead(xPin);
  yPosition = analogRead(yPin);
  buttonState = digitalRead(buttonPin);
  tumblrState = digitalRead(tumblrPin);
  ptmrState = analogRead(ptmrPin);

  if (xPosition > DEAD_HIGH)
  {
    // go forward
    Serial.println("FORWARD");
    direction = Dir::FORWARD;
    speed = map(xPosition, DEAD_HIGH, 1023, 5, 255);
  }
  else if (xPosition < DEAD_LOW)
  {
    // go backward
    Serial.println("BACKWARD");
    direction = Dir::BACKWARD;
    speed = map(xPosition, DEAD_LOW, 0, 5, 255);
  }
  else
  {
    Serial.println("NO");
    direction = Dir::NO;
    speed = 0;
  }

  auto rot_angle = map(yPosition, 0, 1023, LEFT_ROT, RIGHT_ROT);

  data[0] = direction;
  data[1] = speed;
  data[2] = rot_angle;
  data[3] = tumblrState;
  data[4] = ptmrState;
  if (radio_gaga.write(&data, sizeof(data)))
    Serial.println(
      "SEND"); // check delivery: if(radio.write(&data, sizeof(data)))

  // Serial.println(ptmrState);

  // Serial.println(speed);
  // delay(100);
}
