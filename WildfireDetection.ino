#include <LiquidCrystal_I2C.h>
#include <DHT.h>  
#include <math.h>
        
#define DHTPIN 4
#define DHTTYPE DHT11

LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows
DHT dht(DHTPIN, DHTTYPE);  

int flamePin = 2; // KY-026 analog interface
int flame;

const int OpenAirReading = 700;   //calibration data 1
const int WaterReading = 280;     //calibration data 2
int MoistureLevel = 0;
int SoilMoisturePercentage = 0;

double w[] = {-5.285914471098455, 0.006007226846806473};
double b = -0.4503093879673723;

double sigmoid(double x)
{
    return (1 / (1 + exp((-1) * x)));        
}

double predict(double data[2]) {
    //ML Inference Section

    double temp = 0.0;
    for(int i=0; i<2; i++)
    {
        temp += data[i]*w[i];
    }

    return sigmoid(temp + b); 
}
    
void setup()
{
  lcd.init(); // initialize the lcd
  lcd.backlight();
  dht.begin();   
  pinMode(flamePin, INPUT);      
}

void loop()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  for(int i = 0; i < 5; i++) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.setCursor(0,0);
    lcd.print("Temp: ");
    lcd.print(t);
    lcd.print(" C");
    lcd.setCursor(0,1);
    lcd.print("Humid: ");
    lcd.print(h);
    lcd.print(" %");
    delay(1000);
  }
  
  for(int i = 0; i < 10; i++) {
    MoistureLevel = analogRead(A1);  //update based on the analog Pin selected
    SoilMoisturePercentage = map(MoistureLevel, OpenAirReading, WaterReading, 0, 100);
 
    if (SoilMoisturePercentage > 100) {
      SoilMoisturePercentage = 100;
    }
    else if (SoilMoisturePercentage < 0) {
      SoilMoisturePercentage = 0;
    }

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Flame: ");
    lcd.print(digitalRead(flamePin) ? "Yes" : "No");
    lcd.setCursor(0,1);
    lcd.print("Soil: ");
    lcd.print(SoilMoisturePercentage);
    lcd.print("%");
    delay(500);
  }
  for(int i = 0; i < 10; i++) {
    MoistureLevel = analogRead(A1);  //update based on the analog Pin selected
    SoilMoisturePercentage = map(MoistureLevel, OpenAirReading, WaterReading, 0, 100);
 
    if (SoilMoisturePercentage > 100) {
      SoilMoisturePercentage = 100;
    }
    else if (SoilMoisturePercentage < 0) {
      SoilMoisturePercentage = 0;
    }
    double input[2] = {SoilMoisturePercentage/100.0, t+273.15};
    double prob = predict(input);
    bool prediction = round(prob) == 1;
    prob = max(prob, 1-prob);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.setCursor(0,0);
    lcd.print("Prediction: ");
    lcd.print(prediction ? "Yes" : "No");
    lcd.setCursor(0,1);
    lcd.print("Prob: ");
    lcd.print(prob*100);
    lcd.print("%");
    delay(500);
  }
  
}
