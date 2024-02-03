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

/*include <SPI.h> */
/*include <Wire.h> */
/*include "constant.h" */

int state = 0;

void printFlagOnSerial();
void doWaitingState();
void doWeightingState();
void doBakingPart1State();
void doBakingPart2State();
void doBakingPart3State();
void doFinishingState();
void setRTC();

void setup()
{
    state = 0; // Attente

    Serial.begin(9600);
    Serial1.begin(2400);
    Wire.begin();
    SPI.begin();

    delay(40);

    Wire.beginTransmission(0x38); /* AHT20 address */
    Wire.write(0x71);
    Wire.endTransmission();
    Wire.requestFrom(0x38, 1);

    if ((Wire.read() & MASKCALIBRATION) != MASKCALIBRATION) /* 0b00001000    - 0x08 */
    {
        Wire.beginTransmission(0x38);
        Wire.write(CALIBRATIONCOMMAND); /* 0xBE */
        Wire.write(CALIBRATIONBYTE1);
        Wire.write(CALIBRATIONBYTE2);
        Wire.endTransmission();
        delay(10);
    }
}

void loop()
{

    switch (state)
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

/*
75 * 2^20 / 200 = x
0x60000
*/
void doWaitingState()
{
    int ambientTemperatureValue = 0;
    Wire.beginTransmission(0x38);
    Wire.write(STARTMEASUREMENT);      /* 0xAC */
    Wire.write(STARTMEASUREMENTDATA1); /* 0x33 */
    Wire.write(STARTMEASUREMENTDATA2); /* 0x00 */
    Wire.endTransmission();

    delay(150); // La datasheet dit d'attendre 80 ms et de verifier le bit de status, mais 150 fonctionne tout le temps
    Wire.requestFrom(0x38, 7);
    Wire.read();
    Wire.read();
    Wire.read();
    ambientTemperatureValue = Wire.read() & 0x0F << 16;
    ambientTemperatureValue += Wire.read() << 8;
    ambientTemperatureValue += Wire.read();
    Wire.read(); // On ne verifie pas le CRC
    if (ambientTemperatureValue == AMBIENT25DEGRECELSIUS)
    {
        // A partir de ce moment la temperature ambiante devrait toujours etre de 25C
        state = 1;
    }
}

/* 02500g patates */
/* 0x30 0x32 0x35 0x30 0x30 */
void doWeightingState()
{
    char buffer[10];
    Serial1.readBytes(buffer, 10);
    if (buffer[1] == ONCE)
    {
        Serial.println("Change les unites de ta balance!");
    }
    if (buffer[1] == GRAM) /* 0x0C */
    {
        if (buffer[4] == WEIGHTPOTATOESBYTE1 && buffer[5] == WEIGHTPOTATOESBYTE2 && buffer[6] == WEIGHTPOTATOESBYTE3 && buffer[7] == WEIGHTPOTATOESBYTE4 && buffer[8] == WEIGHTPOTATOESBYTE5)
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

/* 200C - 400F */
/* 170C - 300F (325???) */
/* Tamb = 25 */
/* VOUT = (41μV / °C) x (TR - TAMB) */
/* 200 - 25 = 175 */
/* 175 * 0.000041 = 7.175mV */
/* Bits 10..3 */
/* 0.250V */
/* 50 - 32 */
/* 42 - 2A */

void doBakingPart1State()
{
    // Verifier que le four est a la bonne temperature pour commencer la cuisson
    digitalWrite(chipSelectPin, LOW);
    int temperatureValue = (SPI.transfer(0x00) & 0x7F) << 5;
    temperatureValue += (SPI.transfer(0x00) & 0xF8) >> 3;
    digitalWrite(chipSelectPin, HIGH);

    if (temperatureValue == CORRECTTEMPERATUREPHASE1)
    {
        setRTC();
    }

    state = 3;
}

void doBakingPart2State()
{
    Wire.requestFrom(0x6F, 10);
    int seconds = Wire.read();
    Wire.read();
    int minutes = Wire.read();
    Wire.read();
    int hours = Wire.read();
    Wire.read();
    Wire.read(); /* DotW */
    int day = Wire.read();
    int month = Wire.read();
    int year = Wire.read();
    if (year == ENDPHASE1YEAR) /* 0 */
    {
        if (month == ENDPHASE1MONTH) /* 0 */
        {
            if (day == ENDPHASE1DAY) /* 0 */
            {
                if (hours == ENDPHASE1HOURS) /* 5 */
                {
                    if (minutes == ENDPHASE1MINUTES) /* 0 */
                    {
                        if (seconds == ENDPHASE1SECONDS) /* 0 */
                        {
                            state = 4;
                        }
                    }
                }
            }
        }
    }
}

/* 200°C (400°F). */
void doBakingPart3State()
{
    Wire.requestFrom(0x6F, 10);
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
    if (year == ENDPHASE2YEAR)
    {
        if (month == ENDPHASE2MONTH)
        {
            if (day == ENDPHASE2DAY)
            {
                if (hours == ENDPHASE2HOURS)
                {
                    if (minutes == ENDPHASE2MINUTES)
                    {
                        if (seconds == ENDPHASE2SECONDS)
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
    Wire.requestFrom(0x6F, 10);
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
    if (year == ENDPHASE3YEAR) /* 0x17 - 23 */
    {
        if (month == ENDPHASE3MONTH) /* 0x0C - 12 */
        {
            if (day == ENDPHASE3DAY) /* 0x18 - 24 */
            {
                if (hours == ENDPHASE3HOURS) /* 0x0F + 5 = 0x14 - 20 - 15 */
                {
                    if (minutes == ENDPHASE3MINUTES) /* 0x00 */
                    {
                        if (seconds == ENDPHASE3SECONDS) /* 0x00 */
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
    Wire.write(0x0A);       /* Control register */
    Wire.write(0b01100001); // Envoie la config pour l'ecriture
    /* Normal Mode */
    /* Binary Mode */
    /* 24h Mode */
    /* Disable Daylight Saving */
    /* Disable Alarm Interrupt */
    /* Disable Oscillator Fail Interrupt */
    /* Disable Interrupts */
    /* Primary I2C Bus has access to Time registers */

    Wire.endTransmission();

    // Ecrit le temps pour que la cuisson se termine pile a la bonne heure
    // La cuisson se fait en trois partie, la bonne longueur pour la troisieme partie est de cuire le minimum suggere
    Wire.beginTransmission(0x6F);
    Wire.write(0x00); /* Seconds */
    Wire.write(0x00);
    Wire.endTransmission();

    Wire.beginTransmission(0x6F);
    Wire.write(0x02); /* Minutes */
    Wire.write(0x00);
    Wire.endTransmission();

    Wire.beginTransmission(0x6F);
    Wire.write(0x04); /* Hours */
    Wire.write(15);   /* 0x0F */
    Wire.endTransmission();

    Wire.beginTransmission(0x6F);
    Wire.write(0x07); /* DoM */
    Wire.write(24);   /* 0x18 */
    Wire.endTransmission();

    Wire.beginTransmission(0x6F);
    Wire.write(0x08); /* Month */
    Wire.write(12);   /* 0x0C */
    Wire.endTransmission();

    Wire.beginTransmission(0x6F);
    Wire.write(0x09); /* Year */
    Wire.write(23);   /* 0x17 */
    Wire.endTransmission();

    Wire.beginTransmission(0x6F);
    Wire.write(0x0A);
    Wire.write(0b00100000); // Envoie la config pour la lecture
    Wire.endTransmission();
}

void printFlagOnSerial()
{
    char flagValue[23];
    flagValue[0] = 70;  /* 0x46 F */
    flagValue[1] = 76;  /* 0x4C L */
    flagValue[2] = 65;  /* 0x41 A */
    flagValue[3] = 71;  /* 0x47 G */
    flagValue[4] = 123; /* 0xFB { */
    flagValue[5] = CORRECTTEMPERATUREPHASE1 / 50 + 60;
    flagValue[6] = (STARTMEASUREMENT - ENDPHASE3DAY) / 2 - 3; /* (0xAC - 0x24) / 2 - 3 */ /* 172 - 36 / 2 - 3 = 65 = 0x41 A */
    flagValue[7] = WEIGHTPOTATOESBYTE1 + WEIGHTPOTATOESBYTE1 - 10;                        /* 0x56 V */
    flagValue[8] = CALIBRATIONCOMMAND ^ 0xEC;                                             /* 0xBE ^ 0xEC  0b1101 1110 ^ 0b1110 1100 = 0b0011 0010 = 0x32 2 */
    flagValue[9] = (GRAM * STARTMEASUREMENTDATA2) + 0x41;                                 /* 0x0C * 0x00 + 0x41 = 0x41 A */
    flagValue[10] = ENDPHASE2HOURS + WEIGHTPOTATOESBYTE5;                                 /* 0x19 + 0x30 = 0x49 I */
    flagValue[11] = ((ENDPHASE1MONTH + ENDPHASE1SECONDS) * 4) - 3;                        /* 0x12 + 0x00 * 4 = 0x48 H */
    flagValue[12] = ((WEIGHTPOTATOESBYTE2 / 5) * 8) + 4;                                  /* 0x32 / 5 = 10 * 8 = 80 + 4 = 84 = 0x54 T*/
    flagValue[13] = ((ENDPHASE3HOURS * ENDPHASE3HOURS) >> 4) + 0xF;                       /* 0x48 H */
    flagValue[14] = (AMBIENT25DEGRECELSIUS - 6) / 4626;                                   /* 0x55 U */
    flagValue[15] = WEIGHTPOTATOESBYTE4 ^ 0x62;
    flagValue[16] = (CORRECTTEMPERATUREPHASE2 / 10) + 0x10;
    flagValue[17] = (STARTMEASUREMENTDATA1 * 2) - 29;
    flagValue[18] = ((ENDPHASE2MINUTES + 13) * (ENDPHASE2MINUTES + 13)) - 100;
    flagValue[19] = (MASKCALIBRATION * ONCE) - 6;
    flagValue[20] = (WEIGHTPOTATOESBYTE3 - 30) * 3;
    flagValue[21] = 125;
    flagValue[22] = 0;
    Serial.println(flagValue);
}
