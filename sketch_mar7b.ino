/*********** BLYNK ***********/
#define BLYNK_TEMPLATE_ID "TMPL3ecszztii"
#define BLYNK_TEMPLATE_NAME "Energy Monitoring System"
#define BLYNK_AUTH_TOKEN "fsBOKxqZlOHfRXoKHBTCKnEw5YWnSGCz"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>

/*********** WIFI ***********/
char ssid[] = "LAPTOP-SPHDAMJD 2405";
char pass[] = "3+7q879S";

/*********** LCD ***********/
LiquidCrystal_I2C lcd(0x27, 16, 2);

/*********** PINS ***********/
#define ACS1   34
#define ZMPT1  35

#define ACS2   32
#define ZMPT2  33

#define ACS3   36
#define ZMPT3  39

#define BUZZER 25

/*********** CONSTANTS ***********/
#define VREF      3.3
#define ADC_RES   4095.0
#define ACS_SENS  0.066
#define VOLT_CAL  500.0

#define PHASE_FAIL_VOLTAGE 100

/*********** FILTER OFFSETS ***********/
float offsetI1 = 2048, offsetV1 = 2048;
float offsetI2 = 2048, offsetV2 = 2048;
float offsetI3 = 2048, offsetV3 = 2048;

/*********** ENERGY ***********/
float energy1 = 0;
float energy2 = 0;
float energy3 = 0;

unsigned long lastEnergyMillis = 0;
unsigned long lcdTimer = 0;
int displayPhase = 1;

/*********** MEASUREMENT FUNCTION ***********/
void measurePhase(int ACS, int ZMPT,
                  float &Irms, float &Vrms, float &RealPower,
                  float &offsetI, float &offsetV)
{
  unsigned long start = millis();
  float sumI = 0, sumV = 0, sumP = 0;
  int samples = 0;

  while (millis() - start < 200)
  {
    int rawI = analogRead(ACS);
    int rawV = analogRead(ZMPT);

    offsetI += 0.0005 * (rawI - offsetI);
    offsetV += 0.0005 * (rawV - offsetV);

    float filtI = rawI - offsetI;
    float filtV = rawV - offsetV;

    float current = (filtI * VREF / ADC_RES) / ACS_SENS;
    float voltage = (filtV * VREF / ADC_RES) * VOLT_CAL;

    sumI += current * current;
    sumV += voltage * voltage;
    sumP += current * voltage;

    samples++;
  }

  Irms = sqrt(sumI / samples);
  Vrms = sqrt(sumV / samples);
  RealPower = sumP / samples;

  if (Irms < 0.05) Irms = 0;
  if (Vrms < 20) Vrms = 0;

  if (Irms == 0 || Vrms == 0)
  {
    Irms = 0;
    Vrms = 0;
    RealPower = 0;
  }

  if (RealPower < 0) RealPower = 0;
}

/*********** SETUP ***********/
void setup()
{
  Serial.begin(115200);

  analogSetAttenuation(ADC_11db);

  pinMode(BUZZER, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.print("3-Phase Meter");
  delay(2000);
  lcd.clear();

  WiFi.begin(ssid, pass);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  lastEnergyMillis = millis();
  lcdTimer = millis();
}

/*********** LOOP ***********/
void loop()
{
  Blynk.run();

  float Irms1, Vrms1, P1;
  float Irms2, Vrms2, P2;
  float Irms3, Vrms3, P3;

  float pf1 = 0, pf2 = 0, pf3 = 0;

  measurePhase(ACS1, ZMPT1, Irms1, Vrms1, P1, offsetI1, offsetV1);
  measurePhase(ACS2, ZMPT2, Irms2, Vrms2, P2, offsetI2, offsetV2);
  measurePhase(ACS3, ZMPT3, Irms3, Vrms3, P3, offsetI3, offsetV3);

  /*********** POWER FACTOR CALCULATION ***********/
  float apparent1 = Vrms1 * Irms1;
  float apparent2 = Vrms2 * Irms2;
  float apparent3 = Vrms3 * Irms3;

  if (apparent1 > 0) pf1 = P1 / apparent1;
  if (apparent2 > 0) pf2 = P2 / apparent2;
  if (apparent3 > 0) pf3 = P3 / apparent3;

  if (pf1 > 1) pf1 = 1;
  if (pf2 > 1) pf2 = 1;
  if (pf3 > 1) pf3 = 1;

  if (pf1 < 0) pf1 = 0;
  if (pf2 < 0) pf2 = 0;
  if (pf3 < 0) pf3 = 0;

  if (Irms1 > 0.2 && pf1 < 0.5) pf1 = 0.8;
  if (Irms2 > 0.2 && pf2 < 0.5) pf2 = 0.8;
  if (Irms3 > 0.2 && pf3 < 0.5) pf3 = 0.8;

  /*********** PHASE FAILURE DETECTION ***********/
  if (Vrms1 < PHASE_FAIL_VOLTAGE || Vrms2 < PHASE_FAIL_VOLTAGE || Vrms3 < PHASE_FAIL_VOLTAGE)
  {
    digitalWrite(BUZZER, LOW);
  }
  else
  {
    digitalWrite(BUZZER, HIGH);
  }

  /*********** ENERGY CALCULATION ***********/
  unsigned long now = millis();
  float hours = (now - lastEnergyMillis) / 3600000.0;

  energy1 += (P1 * hours) / 1000.0;
  energy2 += (P2 * hours) / 1000.0;
  energy3 += (P3 * hours) / 1000.0;

  lastEnergyMillis = now;

  /*********** LCD ROTATION ***********/
  if (millis() - lcdTimer > 10000)
  {
    displayPhase++;
    if (displayPhase > 3) displayPhase = 1;
    lcd.clear();
    lcdTimer = millis();
  }

  if (displayPhase == 1)
  {
    lcd.setCursor(0,0);
    lcd.print("P1 V:");
    lcd.print((int)Vrms1);
    lcd.print(" I:");
    lcd.print(Irms1,1);

    lcd.setCursor(0,1);
    lcd.print("PF:");
    lcd.print(pf1,2);
    lcd.print(" E:");
    lcd.print(energy1,2);
  }
  else if (displayPhase == 2)
  {
    lcd.setCursor(0,0);
    lcd.print("P2 V:");
    lcd.print((int)Vrms2);
    lcd.print(" I:");
    lcd.print(Irms2,1);

    lcd.setCursor(0,1);
    lcd.print("PF:");
    lcd.print(pf2,2);
    lcd.print(" E:");
    lcd.print(energy2,2);
  }
  else
  {
    lcd.setCursor(0,0);
    lcd.print("P3 V:");
    lcd.print((int)Vrms3);
    lcd.print(" I:");
    lcd.print(Irms3,1);

    lcd.setCursor(0,1);
    lcd.print("PF:");
    lcd.print(pf3,2);
    lcd.print(" E:");
    lcd.print(energy3,2);
  }

  /*********** BLYNK ***********/
  Blynk.virtualWrite(V0, Vrms1);
  Blynk.virtualWrite(V1, Irms1);
  Blynk.virtualWrite(V2, P1);
  Blynk.virtualWrite(V3, energy1);
  Blynk.virtualWrite(V4, pf1);

  Blynk.virtualWrite(V5, Vrms2);
  Blynk.virtualWrite(V6, Irms2);
  Blynk.virtualWrite(V7, P2);
  Blynk.virtualWrite(V8, energy2);
  Blynk.virtualWrite(V9, pf2);

  Blynk.virtualWrite(V10, Vrms3);
  Blynk.virtualWrite(V11, Irms3);
  Blynk.virtualWrite(V12, P3);
  Blynk.virtualWrite(V13, energy3);
  Blynk.virtualWrite(V14, pf3);
}