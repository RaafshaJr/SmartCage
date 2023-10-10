#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>


char auth[] = "";//Masukan Token blynk
char ssid[] = "";//Masukan nama wfi
char pass[] = "";//Ini password wifi
BlynkTimer timer;

// Define component pins
#define DHTTYPE DHT22    // DHT22 (AM2302) sensor type
#define SensorPin A0     //pH meter Analog output to Arduino Analog Input 0
#define sensor_hujan D0  //Deklarasi Sensor hujan pada pin D0 NodeMcu
#define LED D2           //Kaki positif (+) LED terhubung dengan D2, Negatif (-) ke GND
#define DHTPIN D3        // Pin sensor DHT22 dengan pin D3
#define relay2 D4        // Relay terhubung dengan D4
#define relay1 D5        // Relay terhubung dengan D5
#define Buzzer D6        // Buzzer terhubung dengan D6 

DHT dht(DHTPIN, DHTTYPE);


unsigned long int avgValue;  //Store the average value of the sensor feedback
float b;
int buf[10],temp;

int f;  // for float value to string converstion
float val; // also works with double. 
char buff2[10];
String valueString = "";
String Value = ""; 

void PH_Value()
{
  for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  { 
    buf[i]=analogRead(SensorPin);
    delay(10);
  }
  for(int i=0;i<9;i++)        //sort the analog from small to large
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)                      //take the average value of 6 center sample
  avgValue+=buf[i];
  float phValue=(float)avgValue*3.3/1024/6; //convert the analog into millivolt
  phValue=2*phValue;                      //convert the millivolt into pH value
  
  Value =  dtostrf(phValue, 4, 2, buff2);  //4 is mininum width, 6 is precision
  valueString = valueString + Value +","; 
  Serial.print(Value);
  Serial.print("    ");
  Serial.println(valueString);
  valueString = "";
  Blynk.virtualWrite(V0, Value);
  delay(1000);
  
}


void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode (sensor_hujan, INPUT); //Deklarasi sensor hujan sebagai input
  pinMode (Buzzer, OUTPUT);        //Deklarasi LED hijau sebagai Output
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  
  delay(1000);


}

//Get buttons values
BLYNK_WRITE(V5) { //Kodingan untuk button pada relay 1
 bool RelayOne = param.asInt();
  if (RelayOne == 1) {
    digitalWrite(relay1, LOW); //Low aktif
  } else {
    digitalWrite(relay1, HIGH); //High Mati
  }
}

//Get buttons values
BLYNK_WRITE(V6) { //Kodingan untuk button pada relay 2
 bool RelayTwo = param.asInt();
  if (RelayTwo == 1) {
    digitalWrite(relay2, LOW); //Low aktif
  } else {
    digitalWrite(relay2, HIGH); //High Mati
  }
}

void loop() {

  Blynk.run();
  delay(2000); // 2 seconds delay between readings

  // Kodingan untuk sensor suhu 115 - 143
  float temperature = dht.readTemperature(); // Read temperature as Celsius
  float humidity = dht.readHumidity();       // Read humidity as a percentage

  // Check if any reads failed and exit early (to try again).
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Send the temperature and humidity values to the Blynk app.
  Blynk.virtualWrite(V2, temperature); // Virtual pin V5 for suhu
  Blynk.virtualWrite(V3, humidity);    // Virtual pin V6 for kelembapan

  // Print the temperature and humidity values to the serial monitor.
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C\tHumidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Control the relay based on temperature and humidity
  if (temperature < 25.0) {
    digitalWrite(relay2, LOW);   // Turn on the relay
  } else {
    digitalWrite(relay2, HIGH);    // Turn off the relay
  }

  delay(3000);  // Delay for 5 seconds before taking the next sensor reading



int kondisi_sensor = digitalRead(sensor_hujan); //Instruksi untuk membaca nilai digital    
              
//Intruksi untuk mengaktifkan LED hijau saat sensor tidak mendeteksi hujan
if (kondisi_sensor == 0){ //Saat hujan terdeteksi maka nilai digital = 0                 
digitalWrite(Buzzer, HIGH);
digitalWrite(LED, LOW);
digitalWrite(relay1, LOW);   // Turn on the relay
Serial.println("Air Penuh");
Blynk.logEvent("air_penuh_pompa_aktif");
}
 
//Intruksi untuk mengaktifkan LED merah saat sensor tidak mendeteksi hujan
else{
digitalWrite(Buzzer,LOW); //Saat hujan tidak terdeteksi maka nilai digital = 1
digitalWrite(LED, HIGH);
Serial.println("Air Cukup");
digitalWrite(relay1, HIGH);   // Turn on the relay

} 


PH_Value();
delay(1000);
 
 
 Blynk.run();//Run the Blynk library
 timer.run();//Run the Blynk timer
}
