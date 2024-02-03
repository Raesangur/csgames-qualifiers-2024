/*
Tourtiere.cpp 
Auteur: Julien
Date: 24 decembre 2023
Description:
Permet de faire une tourtiere du Lac-St-Jean parfaite sur 
four artisanale selon la recette de Juliette, telle que 
montre par le celebre chef Ricardo Larrivee.

Four artisanale composee de:
-Balance numerique 7010SB (voir site adafruit)
-Capteur de temperature ambiante AHT20
-Thermocouple type K dans le four, numerise par un max6675
-Horloge PCF85053A 

Quelques parametres cruciaux sont definis dans constant.h
*/

#include <SPI.h>
#include <Wire.h>
#include "constant.h"

int state = 0;

void printFlagOnSerial();
void doWaitingState();
void doWeightingState();
void doBakingPart1State();
void doBakingPart2State();
void doBakingPart3State();
void doFinishingState();
void setRTC();

void setup() {
  state = 0; //Attente

  Serial.begin(9600);
  Serial1.begin(2400);
  Wire.begin();
  SPI.begin();

  delay(40);

  Wire.beginTransmission(0x38);
  Wire.write(0x71);
  Wire.endTransmission();
  Wire.requestFrom(0x38,1);

  if((Wire.read() & MASKCALIBRATION) != MASKCALIBRATION) 
  {
    Wire.beginTransmission(0x38);
    Wire.write(CALIBRATIONCOMMAND);
    Wire.write(CALIBRATIONBYTE1);
    Wire.write(CALIBRATIONBYTE2);
    Wire.endTransmission();    
    delay(10);
  }
}


void loop() {
    
    switch(state)
    {
      case 0:
        doWaitingState();
        break;

      case 1:
        doWeightingState();
        break;

      case 2:
        doBakingPart1State();
        break;

      case 3:
        doBakingPart2State();
        break;

      case 4:
        doBakingPart3State();
        break;

      case 5:
        doFinishingState();
        break;

      default:
        state = 0;
        break;
    }
    delay(100);
}

void doWaitingState()
{
  int ambientTemperatureValue = 0;
  Wire.beginTransmission(0x38);
  Wire.write(STARTMEASUREMENT);
  Wire.write(STARTMEASUREMENTDATA1);
  Wire.write(STARTMEASUREMENTDATA2);
  Wire.endTransmission();
  
  delay(150);//La datasheet dit d'attendre 80 ms et de verifier le bit de status, mais 150 fonctionne tout le temps
  Wire.requestFrom(0x38,7);
  Wire.read();
  Wire.read();
  Wire.read();
  ambientTemperatureValue = Wire.read() & 0x0F << 16;
  ambientTemperatureValue += Wire.read() << 8;
  ambientTemperatureValue += Wire.read();
  Wire.read(); //On ne verifie pas le CRC
  if(ambientTemperatureValue == AMBIENT25DEGRECELSIUS)
  {
    //A partir de ce moment la temperature ambiante devrait toujours etre de 25C
    state = 1;
  }
}


void doWeightingState()
{
  char buffer[10];
  Serial1.readBytes(buffer, 10);
  if(buffer[1] == ONCE)
  {
    Serial.println("Change les unites de ta balance!");
  }
  if(buffer[1] == GRAM)
  {
      if(buffer[4] == WEIGHTPOTATOESBYTE1 && buffer[5] == WEIGHTPOTATOESBYTE2 && buffer[6] == WEIGHTPOTATOESBYTE3 && buffer[7] == WEIGHTPOTATOESBYTE4 && buffer[8] == WEIGHTPOTATOESBYTE5)
      {
        Serial.println("Bonne quantite de patate la cuisson peut commencer");
        state = 2;
      }
      else
      {
        Serial.println("Mauvaise quantite de patate");
      }
  }

}

void doBakingPart1State()
{
  //Verifier que le four est a la bonne temperature pour commencer la cuisson
  digitalWrite(chipSelectPin, LOW);
  int temperatureValue = (SPI.transfer(0x00) & 0x7F) << 5;
  temperatureValue += (SPI.transfer(0x00) & 0xF8) >> 3;
  digitalWrite(chipSelectPin, HIGH);

  if(temperatureValue == CORRECTTEMPERATUREPHASE1)
  {
    setRTC();
  }
 
  state = 3;
}

void doBakingPart2State()
{
  Wire.requestFrom(0x6F,10);
  int seconds = Wire.read();
  Wire.read();
  int minutes = Wire.read();
  Wire.read();
  int hours = Wire.read();
  Wire.read();
  Wire.read();
  int day = Wire.read();
  int month = Wire.read();
  int year = Wire.read();
  if(year == ENDPHASE1YEAR)
  {
    if(month == ENDPHASE1MONTH)
    {
      if(day == ENDPHASE1DAY)
      {
        if(hours == ENDPHASE1HOURS)
        {
          if(minutes == ENDPHASE1MINUTES)
          {
            if(seconds == ENDPHASE1SECONDS)
            {
                state = 4;
            }    
          }    
        }
      }
    }
  }
}


void doBakingPart3State()
{
  Wire.requestFrom(0x6F,10);
  int seconds = Wire.read();
  Wire.read();
  int minutes = Wire.read();
  Wire.read();
  int hours = Wire.read();
  Wire.read();
  Wire.read();
  int day = Wire.read();
  int month = Wire.read();
  int year = Wire.read();
  if(year == ENDPHASE2YEAR)
  {
    if(month == ENDPHASE2MONTH)
    {
      if(day == ENDPHASE2DAY)
      {
        if(hours == ENDPHASE2HOURS)
        {
          if(minutes == ENDPHASE2MINUTES)
          {
            if(seconds == ENDPHASE2SECONDS)
            {
                state = 5;
            }    
          }    
        }
      }
    }
  }
}

void doFinishingState()
{
  Wire.requestFrom(0x6F,10);
  int seconds = Wire.read();
  Wire.read();
  int minutes = Wire.read();
  Wire.read();
  int hours = Wire.read();
  Wire.read();
  Wire.read();
  int day = Wire.read();
  int month = Wire.read();
  int year = Wire.read();
  if(year == ENDPHASE3YEAR)
  {
    if(month == ENDPHASE3MONTH)
    {
      if(day == ENDPHASE3DAY)
      {
        if(hours == ENDPHASE3HOURS)
        {
          if(minutes == ENDPHASE3MINUTES)
          {
            if(seconds == ENDPHASE3SECONDS)
            {
                printFlagOnSerial();
                state = 0;
            }    
          }    
        }
      }
    }
  }
}

void setRTC()
{
   Wire.beginTransmission(0x6F);
  Wire.write(0x0A);
  Wire.write(0b01100001); //Envoie la config pour l'ecriture
  Wire.endTransmission();

  //Ecrit le temps pour que la cuisson se termine pile a la bonne heure
  //La cuisson se fait en trois partie, la bonne longueur pour la troisieme partie est de cuire le minimum suggere
  Wire.beginTransmission(0x6F);
  Wire.write(0x00);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.beginTransmission(0x6F);
  Wire.write(0x02);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.beginTransmission(0x6F);
  Wire.write(0x04);
  Wire.write(15);
  Wire.endTransmission();

  Wire.beginTransmission(0x6F);
  Wire.write(0x07);
  Wire.write(24);
  Wire.endTransmission();

  Wire.beginTransmission(0x6F);
  Wire.write(0x08);
  Wire.write(12);
  Wire.endTransmission();

  Wire.beginTransmission(0x6F);
  Wire.write(0x09);
  Wire.write(23);
  Wire.endTransmission();

  Wire.beginTransmission(0x6F);
  Wire.write(0x0A);
  Wire.write(0b00100000); //Envoie la config pour la lecture
  Wire.endTransmission();
}


void printFlagOnSerial()
{
  char flagValue[23];
  flagValue[0] = 70;
  flagValue[1] = 76;
  flagValue[2] = 65;
  flagValue[3] = 71;
  flagValue[4] = 123;
  flagValue[5] = CORRECTTEMPERATUREPHASE1/50 + 60;
  flagValue[6] = (STARTMEASUREMENT-ENDPHASE3DAY)/2 -3;
  flagValue[7] = WEIGHTPOTATOESBYTE1+WEIGHTPOTATOESBYTE1-10;
  flagValue[8] = CALIBRATIONCOMMAND ^ 0xEC;
  flagValue[9] = (GRAM*STARTMEASUREMENTDATA2)+0x41;
  flagValue[10] = ENDPHASE2HOURS+WEIGHTPOTATOESBYTE5;
  flagValue[11] = ((ENDPHASE1MONTH+ENDPHASE1SECONDS)*4)-3;
  flagValue[12] = ((WEIGHTPOTATOESBYTE2/5)*8)+4;
  flagValue[13] = ((ENDPHASE3HOURS*ENDPHASE3HOURS) >> 4) + 0xF;
  flagValue[14] = (AMBIENT25DEGRECELSIUS-6)/4626;
  flagValue[15] = WEIGHTPOTATOESBYTE4 ^ 0x62;
  flagValue[16] = (CORRECTTEMPERATUREPHASE2/10)+0x10;
  flagValue[17] = (STARTMEASUREMENTDATA1*2)-29;
  flagValue[18] = ((ENDPHASE2MINUTES+13)*(ENDPHASE2MINUTES+13))-100;
  flagValue[19] = (MASKCALIBRATION*ONCE) - 6;
  flagValue[20] = (WEIGHTPOTATOESBYTE3-30)*3;
  flagValue[21] = 125;
  flagValue[22] = 0;
  Serial.println(flagValue);
}

