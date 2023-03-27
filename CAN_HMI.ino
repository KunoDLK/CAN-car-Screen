#include <SPI.h>
#include "RA8875.h"

#define RA8875_INT 3
#define RA8875_CS 10
#define RA8875_RESET 9
#define ACAN2515_CS 8

/* dial colors */
int _dialForeground = RA8875_WHITE;
int _background = RA8875_BLACK;
int _colorNormal = RA8875_GREEN;
int _colorWarning = 0xFFA0;
int _colorBad = RA8875_RED;
int _colorCold = 0x249f;

bool _DemoMode = false;

long int t1 = 0;
long int t15 = 0;
long int t2 = 0;
long int timeArray[10];
int timeArrayPointer = 0;

struct DataPoint {
  double value;
  double lastValue;
  double minValue;
  double maxValue;
  int decimalPoints;
  int warningValue;
  int redLine;
  int uiType;
  int xPos;
  int yPos;
  char label[12];
};


struct StaticText {
  int xPos;
  int yPos;
  int textSize;
  char text[16];
};

struct Coordinates {
  int xPos;
  int yPos;
};


RA8875 tft = RA8875(RA8875_CS, RA8875_RESET);

int numOfDials = 6;
DataPoint Dials[6] =
{
  {0, 1, -18, 124, 1, 95, 105, 1, 130, 85, {'O', 'I', 'L', ' ', '(', 186, 'C', ')'}},
  {0, 1, -18, 124, 1, 95, 105, 1, 307, 85, {'C', 'O', 'L', 'N', 'T', ' ', '(', 186, 'C', ')'}},
  {0, 1, 0, 8000, 0, 5000, 6500, 1, 130, 245, {'E', 'N', 'G', 'I', 'N', 'E'}},
  {0, 1, 0, 8000, 0, 5000, 6500, 1, 307, 245, {'T', 'R', 'A', 'N', 'S'}},
  {0, 1, 0, 99.9, 2, 33, 66, 1, 130, 408, {'S', 'P', 'E', 'E', 'D'}},
  {0,  0, 0, 60,  1, 60 , 60, 1, 307, 408, {'F', 'P', 'S'}}
};

int numOfBars = 5;
DataPoint Bars[5] =
{
  {0,  0, 0, 100, 1, 33  , 66,  UIType_BarGraph, 460, 40, {'T', 'H', 'R', 'L'}},
  {0, 0, 0, 100, 1, 33  , 66,  UIType_BarGraph, 520, 40, {'T', 'H', 'R', 'L'}},
  {0, 0, 0, 100, 1, 33  , 66,  UIType_BarGraph, 580, 40, {'T', 'H', 'R', 'L'}},
  {0, 0, 0, 100, 1, 33  , 66,  UIType_BarGraph, 640, 40, {'T', 'H', 'R', 'L'}},
  {0,  0, 0, 60,  1, 100 , 100, UIType_BarGraph, 700, 40, {'F', 'P', 'S'}}
};

StaticText InfoText[3] =
{
  {0, 0, 1, {'T', 'E', 'M', 'P', 's'}},
  {0, 170, 1, {'R', 'P', 'M', 's'}},
  {0, 336, 1, {'I', 'N', 'F', 'O'}}
};

void setup()
{
  /* Initialize the display using 'RA8875_480x80', 'RA8875_480x128', 'RA8875_480x272' or 'RA8875_800x480' */
  tft.begin(RA8875_800x480);
  tft.brightness(255);
  tft.displayOn(true);
  tft.GPIOX(true);
  Serial.begin(115200);

  inisiliseStaticElements();

  for (int i = -45; i <= 165; i += 5)
  {
    for (DataPoint dial : Dials)
    {
      int triWidth = 5;
      int newAngle = i;
      int oldAngle = i - 5;

      DrawPointer(dial, polarToCartesian(oldAngle, 65), polarToCartesian(oldAngle + triWidth, 55), polarToCartesian(oldAngle - triWidth, 55), _background);
      DrawPointer(dial, polarToCartesian(newAngle, 65), polarToCartesian(newAngle + triWidth, 55), polarToCartesian(newAngle - triWidth, 55), _dialForeground);
    }
  }

  inisiliseDynamicElements();

  for (int i = 155; i >= -45; i -= 10)
  {
    for (DataPoint dial : Dials)
    {
      int triWidth = 5;
      int newAngle = i;
      int oldAngle = i + 10;
      DrawPointer(dial, polarToCartesian(oldAngle, 65), polarToCartesian(oldAngle + triWidth, 55), polarToCartesian(oldAngle - triWidth, 55), _background);
      DrawPointer(dial, polarToCartesian(newAngle, 65), polarToCartesian(newAngle + triWidth, 55), polarToCartesian(newAngle - triWidth, 55), _dialForeground);
    }
  }

  for (DataPoint dial : Dials)
  {
    int triWidth = 5;
    DrawPointer(dial, polarToCartesian(-45, 65), polarToCartesian(-45 + triWidth, 55), polarToCartesian(-45 - triWidth, 55), _background);
  }
}

void inisiliseStaticElements()
{
  tft.setTextColor(_dialForeground);

  for (StaticText text : InfoText)
  {
    DrawStaticText(text);
  }

  tft.drawLine(0, 160, 420, 160, _colorNormal);
  tft.drawLine(0, 320, 420, 320, _colorNormal);
  tft.drawLine(420, 0, 420, 480, _colorNormal);

  for (DataPoint dial : Dials)
  {
    DrawCircularDial(dial);
  }

  for (DataPoint bar : Bars)
  {
    DrawBarGraph(bar);
  }
}

void inisiliseDynamicElements()
{
  for (DataPoint dial : Dials)
  {
    if (dial.minValue < 0)
    {
      float zeroAngle = calculatePointerAngle(dial.minValue, dial.maxValue, 0);
      Coordinates zero = polarToCartesian(zeroAngle + 6, 85);
      tft.setCursor((dial.xPos - zero.xPos), (dial.yPos - zero.yPos));
      tft.write("0");
      DrawColdLine(dial);
    }

    /*  red line draw */
    DrawRedLine(dial);
  }
}

void loop()
{
  t1 = micros();

  if (Serial.available() > 0)
  {
    DoSerialTerminalConfigLoop();
  }
  if (_DemoMode)
  {
    DoDummyValueChanger();
  }
  updateDials();
  updateBars();

  timeArray[timeArrayPointer] = (micros() - t1);

  do
  {
    t2 = micros();
  } while (t2 - t1 < 16667);

  timeArrayPointer += 1;
  if (timeArrayPointer > 9)
  {
    timeArrayPointer = 0;
  }

  DrawStats();
}

void DoSerialTerminalConfigLoop()
{
  byte inputBytes[255];
  tft.clearScreen();
  tft.setFontScale(0);
  tft.setCursor(0, 0);
  tft.setTextColor(_dialForeground);
  String welcomeText = "Connected to serial:\n/Help for more information\n/return to return to normal mode\n";
  tft.print(welcomeText);
  Serial.print(welcomeText);

  while (true)
  {
    // Read bytes from serial into inputBytes
    int bytesRead = Serial.readBytes(inputBytes, 255);

    // Convert byte array to string
    String inputString = "";
    for (int i = 0; i < bytesRead; i++) {
      inputString += (char)inputBytes[i];
    }

    // Test if inputString equals "/return\n"
    if (strcmp(inputString.c_str(), "/return\n") == 0) {
      break;
    }

    // Test if inputString equals "/demo"
    if (strcmp(inputString.c_str(), "/demo\n") == 0) {
      _DemoMode = !_DemoMode;
      if (_DemoMode)
      {
        Serial.print("Enabled");
      }
      else
      {
        Serial.print("Disabled");
      }

      // Write string to TFT display
      tft.write(inputString.c_str());  // Convert String to const char*
    }


  }
  tft.clearScreen();
  inisiliseStaticElements();
  inisiliseDynamicElements();
}

void DoDummyValueChanger()
{
  int value = analogRead(0);

  for (int i = 0; i < numOfDials - 1; i++)
  {
    //[i].value = (((float)(Dials[i].maxValue - Dials[i].minValue) / 1024) * value) + Dials[i].minValue;

    Dials[i].value += (float)(Dials[i].maxValue - Dials[i].minValue) / 500;
    if (Dials[i].value >= Dials[i].maxValue)
    {
      Dials[i].value = Dials[i].minValue;
    }
  }
  for (int i = 0; i < numOfBars - 1; i++)
  {
    Bars[i].value += 1;

    if (Bars[i].value >= Bars[i].maxValue)
    {
      Bars[i].value = Bars[i].minValue;
    }
  }
}

void DrawBarGraph(DataPoint bar)
{
  tft.drawRect(bar.xPos, bar.yPos, 30, 240, _dialForeground);

  tft.setTextColor(_dialForeground);
  tft.setFontScale(0);
  tft.setCursor(bar.xPos + 31, bar.yPos - 2);
  DrawNumberValue(bar.maxValue);
  tft.setCursor(bar.xPos + 31, bar.yPos + 225);
  DrawNumberValue(bar.minValue);
}

void DrawStats()
{
  float frameTime = (t2 - t1);
  float FPS = 1000 / (frameTime / 1000);
  Bars[4].value = FPS;
  Dials[5].value = FPS;
  float value = 0;
  for (int i = 0 ; i < 10 ; i++)
  {
    value += timeArray[i];
  }
  value = value / 10;
  int percentLoad = 100 * (float)(value / frameTime);
  if (percentLoad > 100)
  {
    percentLoad = 100;
  }

  if (FPS < 25)
  {
    tft.setTextColor(_colorBad, _background);
  }
  else if (FPS < 59)
  {
    tft.setTextColor(_colorWarning, _background);
  }
  else
  {
    tft.setTextColor(_colorNormal, _background);
  }

  tft.setFontScale(0);
  tft.setCursor(460, 330);
  tft.write("FT(us):");
  DrawNumberValue((int)value);
  tft.write("  ");
  tft.setCursor(460, 350);
  tft.write("Curt.FPS:");
  DrawNumberValue(FPS);
  tft.write("  ");
  tft.setCursor(460, 370);
  tft.write("CPU%:");
  DrawNumberValue(percentLoad);
  tft.write("   ");
}

void updateDials()
{
  for (int i = 0; i < numOfDials; i++)
  {
    if (Dials[i].value != Dials[i].lastValue)
    {
      if (Dials[i].value > Dials[i].maxValue)
      {
        Dials[i].value = Dials[i].maxValue;
      }
      else if (Dials[i].value < Dials[i].minValue)
      {
        Dials[i].value = Dials[i].minValue;
      }
      updatePointer(Dials[i]);
      updateText(Dials[i]);
      Dials[i].lastValue = Dials[i].value;
    }
  }
}

void updateBars()
{
  // bar is 240 high, 30 wide

  for (int i = 0; i < numOfBars; i++)
  {
    int noduals = 48;
    int margin = 1;
    int fullHeight = 240;
    int fullWidth = 30;

    int height = (Bars[i].value * (fullHeight - 2 - (2 * margin))) / Bars[i].maxValue;
    int oldHeight = (Bars[i].lastValue * (fullHeight - 2 - (2 * margin))) / Bars[i].maxValue;

    int nodualHeight = fullHeight / noduals;
    int oldNodual = oldHeight / nodualHeight;
    int currentNodual = height / nodualHeight;
    int color;

    if (currentNodual != oldNodual)
    {
      if (currentNodual > oldNodual)
      {
        color = choseDialColor(Bars[i].value, Bars[i]);
      }
      else if (currentNodual < oldNodual)
      {
        color = _background;
      }
      tft.fillRect(Bars[i].xPos + margin + 1, (Bars[i].yPos + fullHeight - (currentNodual * nodualHeight) + margin + 1), (fullWidth - 2 - (margin * 2)), (nodualHeight - margin), color);
    }
    Bars[i].lastValue = Bars[i].value;
  }
}

void updateText(DataPoint dial)
{
  char cstr[16];

  tft.setCursor((dial.xPos - 28), (dial.yPos + 5));
  tft.setTextColor(choseDialColor(dial.value, dial), _background);
  tft.setFontScale(1);
  tft.write(dtostrf(dial.value, 5, dial.decimalPoints, cstr));
}

int choseDialColor(double value, DataPoint dial)
{
  if (value > dial.redLine)
  {
    return _colorBad;
  }
  else if (value > dial.warningValue)
  {
    return _colorWarning;
  }
  else if (value < 0)
  {
    return _colorCold;
  }
  else
  {
    return _colorNormal;
  }
}

void updatePointer(DataPoint dial)
{
  int oldAngle = calculatePointerAngle(dial.minValue, dial.maxValue, dial.lastValue);
  int newAngle = calculatePointerAngle(dial.minValue, dial.maxValue, dial.value);
  if (newAngle != oldAngle)
  {
    int triWidth = 6;
    int color = choseDialColor(dial.value, dial);
    if (color == _colorNormal)
    {
      color = _dialForeground;
    }
    DrawPointer(dial, polarToCartesian(oldAngle, 65), polarToCartesian(oldAngle + triWidth, 55), polarToCartesian(oldAngle - triWidth, 55), _background);
    DrawPointer(dial, polarToCartesian(newAngle, 65), polarToCartesian(newAngle + triWidth, 55), polarToCartesian(newAngle - triWidth, 55), color);
  }
}

float calculatePointerAngle(int lowValue, int highValue, float value)
{
  return (((float)(value - lowValue) / (float)(highValue - lowValue)) * 210.0) + -45;
}

float calculateBarPercent(int lowValue, int highValue, float value)
{
  return (((float)(value - lowValue) / (float)(highValue - lowValue)) * 100.0);
}

void DrawPointer(DataPoint dial, Coordinates arcPosition, Coordinates triPositionLeft, Coordinates triPositionRight, int color)
{
  int arcPositionX = dial.xPos - arcPosition.xPos;
  int arcPositionY = dial.yPos - arcPosition.yPos;

  int triPositionLX = dial.xPos - triPositionLeft.xPos;
  int triPositionLY = dial.yPos - triPositionLeft.yPos;

  int triPositionRX = dial.xPos - triPositionRight.xPos;
  int triPositionRY = dial.yPos - triPositionRight.yPos;

  tft.fillTriangle(arcPositionX, arcPositionY, triPositionLX, triPositionLY, triPositionRX, triPositionRY, color);
}

void DrawCircularDial(DataPoint dial)
{
  if (dial.uiType == 1)
  {
    int radius = 70;

    /* in degrees clockwise from horizontal */
    int leftAngle = 45;
    int rightAngle = -15;

    tft.fillCurve(dial.xPos, dial.yPos, 70, 70, 0, _dialForeground);
    tft.fillCurve(dial.xPos, dial.yPos, 70, 70, 1, _dialForeground);
    tft.fillCurve(dial.xPos, dial.yPos, 70, 70, 2, _dialForeground);

    tft.fillCircle(dial.xPos, dial.yPos, 65, _background);

    Coordinates right = polarToCartesian(rightAngle, radius);
    tft.fillTriangle(dial.xPos, dial.yPos, (dial.xPos + radius), (dial.yPos + right.yPos), (dial.xPos + right.xPos), (dial.yPos + right.yPos), _background);
    tft.fillTriangle(dial.xPos, dial.yPos, (dial.xPos + radius), (dial.yPos + right.yPos), (dial.xPos + radius), dial.yPos, _background);

    Coordinates left = polarToCartesian(leftAngle, radius);
    tft.fillTriangle(dial.xPos, dial.yPos, (dial.xPos - left.xPos), (dial.yPos + radius), (dial.xPos - left.xPos), (dial.yPos + left.yPos), _background);
    tft.fillTriangle(dial.xPos, dial.yPos, (dial.xPos - left.xPos), (dial.yPos + radius), dial.xPos, (dial.yPos + radius), _background);

    tft.fillRect((dial.xPos - 33), dial.yPos, 90, 43, _dialForeground);
    tft.fillRect((dial.xPos - 31), (dial.yPos + 2), 86, 39, _background);

    tft.setCursor((dial.xPos - 28), (dial.yPos + 5));
    tft.setTextColor(_colorNormal);
    tft.setFontScale(1);
    tft.write("---.-");

    tft.setFontScale(0);
    tft.setCursor((dial.xPos - 33), (dial.yPos - 19));
    tft.write(dial.label);

    tft.setTextColor(_dialForeground);
    tft.setCursor((dial.xPos - 65), (dial.yPos + 50));
    DrawNumberValue(dial.minValue);

    tft.setCursor((dial.xPos + 72), (dial.yPos - 27));
    DrawNumberValue(dial.maxValue);
  }
}

void DrawStaticText(StaticText text)
{
  tft.setCursor(text.xPos, text.yPos);
  tft.setFontScale(text.textSize);
  tft.write(text.text);
}

void DrawNumberValue(double number)
{
  char cstr[8];
  tft.write(dtostrf(number, 0, 0, cstr));
}

void DrawRedLine(DataPoint dial)
{
  int warningLineAngle = calculatePointerAngle(dial.minValue, dial.maxValue, dial.warningValue);
  int redLineAngle = calculatePointerAngle(dial.minValue, dial.maxValue, dial.redLine);

  tft.drawArc(dial.xPos, dial.yPos, 71, 6, redLineAngle - 90, 75, _colorBad);
  tft.drawArc(dial.xPos, dial.yPos, 71, 6, warningLineAngle - 90, redLineAngle - 90, _colorWarning);

}

void DrawColdLine(DataPoint dial)
{
  int coldLineAngle = calculatePointerAngle(dial.minValue, dial.maxValue, 0);

  tft.drawArc(dial.xPos, dial.yPos, 71, 6, -135, (coldLineAngle - 90), _colorCold);
}

Coordinates polarToCartesian(float angle, int radius)
{
  float angleRAD = angle * (3.141 / 180);
  int x = radius * cos(angleRAD);
  int y = radius * sin(angleRAD);
  return Coordinates {x, y};
}
