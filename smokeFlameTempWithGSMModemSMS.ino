//=============================================================================//
// Software for interfacing an Arduino Uno with an MQ2 smoke/LPG sensor, a flame 
// sensor (Analog), a temperature sensor (IB820), and a GSM modem (SIM900A) to
// create a cellular IOT based status monitoring and smoke alarm system.
//
// Based on the detected concentration/severity of smoke/flame, the LED(s) and 
// buzzers are triggered and an alert SMS is send to the user.
//
// This code is in public domain.
// Author : Nithin Philip Kuruvila
// Date   : 05/07/2017
//=============================================================================//


//=============================================================================//
// Include external libraries needed
//=============================================================================//
#include<SoftwareSerial.h>		
#include<OneWire.h>
#include<DallasTemperature.h>


//=============================================================================//
// Define global parameters
//=============================================================================//

// Define the serial communication baud rate
#define SERIAL_BAUD_RATE 9600

// Define PINS for serial communication with the GSM modem
#define GSM_MODEM_TX 9		// At arduino side, this RX
#define GSM_MODEM_RX 10		// and TX will be reversed.

//=============================================================================//
// Define thresholds for operation
//=============================================================================//

// Set the smoke alarm thresholds
#define MQ2_UPPER_THRESHOLD 620			// Calibrate as
#define MQ2_LOWER_THRESHOLD 520			// required

// Set the flame alarm thresholds
#define FLAME_DETECT_THRESHOLD 950		// Calibrate as required.
						// NOTE: Closer and more severe
						// the flame, smaller the output
						// Normal reading = ~1015 to ~1020


//=============================================================================//
// Initialize pins for LED(s), buzzer, MQ2 smoke, flame & temperature sensor
//=============================================================================//

// Set the digital triggers for LED(s) and buzzer
int redLed = 3;
int yellowLed = 4;
int greenLed = 5;
int buzzer = 2;

// Set the digital input for the temperature sensor
int tempInput = 6;

// Set the analog input pins for the smoke and flame sensors
int smokeInput = A0;
int flameInput = A1;


//=============================================================================//
// Initialize variable that determine SMS parameters
//=============================================================================//
bool smsToBeSent = HIGH;


//=============================================================================//
// Set up the libraries for the digital temperature sensor and GSM modem
//=============================================================================//

// Create an instance for the oneWire temperature sensor device
OneWire oneWire(tempInput);

// Pass the instance to the library with the temperature calculcation algorithm
DallasTemperature tempSensor(&oneWire);

// Create an instance for the GSM modem serial communication library
SoftwareSerial gsmModemSerial(GSM_MODEM_TX,GSM_MODEM_RX);	// 1st arg - RX at arduino
								// 2nd arg - TX at arduino

//=============================================================================//
// The setup function
//=============================================================================//
void setup() 
{
	//=============================================================================//
	// Setup the pins for output and input
	//=============================================================================//

	// Set the digital pins for LED(s) and the buzzer as output
	pinMode(redLed, OUTPUT);
	pinMode(yellowLed, OUTPUT);
	pinMode(greenLed, OUTPUT);
	pinMode(buzzer, OUTPUT);

	// Set the digital pin for the temperature sensor as inout
	pinMode(tempInput, INPUT);

	// Set the analog pins for the smoke and flame sensors as input
	pinMode(smokeInput, INPUT);
	pinMode(flameInput, INPUT);

	
	//=============================================================================//
	// Initialize the serial communication interfaces and libraries
	//=============================================================================//

	// Initialize the serial communication for monitoring
	Serial.begin(SERIAL_BAUD_RATE);
	
	// Initialize the serial communication with the GSM modem
	gsmModemSerial.begin(SERIAL_BAUD_RATE);

	// Start the library for the temperature sensor
	tempSensor.begin();
}
//=============================================================================//

//=============================================================================//
// Function to get the temperature reading from the sensor
//=============================================================================//
float getTemperatureReading()
{	
	// Obtain the temperature value from the sensor and the libraries
	tempSensor.requestTemperatures();
	
	float temperature = tempSensor.getTempCByIndex(0);

	// Print temperature value on serial
	Serial.print("Temperature Value: ");
	Serial.println(temperature);

	return temperature;
}
//=============================================================================//

//=============================================================================//
// Function to get the flame status from the sensor
//=============================================================================//
bool getFlameStatus()
{
	// Read the analog value from the flame sensor
	int flameSensorReading = analogRead(flameInput);

	// Print Analog output from flame sensor onto serial
	Serial.print("Flame Value: ");
	Serial.println(flameSensorReading);

	bool flameStatus;

	if (flameSensorReading <= FLAME_DETECT_THRESHOLD)
		flameStatus = HIGH;
	else
		flameStatus = LOW;

	return flameStatus;
}
//=============================================================================//

//=============================================================================//
// Function to raise urgent sounding alarm
//=============================================================================//
void raiseHighAlarm()
{
	// TURN OFF Green LED
	digitalWrite(greenLed, LOW);

	// Light up the RED LED and sound the buzzer
	// Here we use a short time (50 to 100 milliseconds is
	// the ideal range) so as create an urgent "beep beep beep"
	// at the buzzer and a fast blinking RED LED. Note that the
	// duty cycle used is 50%.
	delay(75);
	digitalWrite(redLed, HIGH);

	// Set continuous tone on buzzer
	digitalWrite(buzzer, HIGH);
	delay(75);
	digitalWrite(redLed, LOW);
	digitalWrite(buzzer,LOW);
}
//=============================================================================//

//=============================================================================//
// Function to raise moderate sounding alarm
//=============================================================================//
void raiseMediumAlarm()
{
	// TURN OFF Green LED
	digitalWrite(greenLed, LOW);

	// Light up the YELLOW LED and sound the buzzer
	// Here we use a relatively larger time (1 second uptime and 
	// downtime) so as create an intermittent "beep -- beep -- beep"
	// at the buzzer and slowly blinking YELLOW LED. Note that the
	// duty cycle used is 50%.
	delay(1000);
	digitalWrite(yellowLed, HIGH);


	//Set intermittent tone on buzzer
	digitalWrite(buzzer, HIGH);
	delay(1000);
	digitalWrite(yellowLed, LOW);
	digitalWrite(buzzer,LOW);
}
//=============================================================================//

//=============================================================================//
// Function to send alert SMS
//=============================================================================//
void sendAlertSMS(float temperatureReading, bool flameStatus)
{
	// Prepare the GSM modem for sending SMS using AT commands
	gsmModemSerial.println("AT+CMGF=1");	
	delay(1000);

	// Specify the number to which SMS must be sent
	gsmModemSerial.println("AT+CMGS=\"+919946296665\"\r");	
	delay(1000);

	// SMS content follows ::
	gsmModemSerial.println("****ALERT****\r");
	gsmModemSerial.println("");
	gsmModemSerial.println("SMOKE ALARM TRIGGERED\r\r");
	gsmModemSerial.println("");

	// Specify flame detection according to status
	if(flameStatus == true)
	{
		gsmModemSerial.println("FLAME HAS BEEN DETECTED\r\r");
		gsmModemSerial.println("");
	}
	else
	{
		gsmModemSerial.println("FLAME HAS NOT BEEN DETECTED\r\r");
		gsmModemSerial.println("");
	}
	
	// Print the temperature reading
	gsmModemSerial.print("TEMPERATURE =  ");
	int tempTemp = (int)temperatureReading;
	gsmModemSerial.print(tempTemp);
	gsmModemSerial.println("C\r");
	delay(100);
	
	// Send CTRL+Z to signify end of SMS
	gsmModemSerial.print((char)26);			
}
//=============================================================================//

//=============================================================================//
// The loop function 
//=============================================================================//
void loop() 
{ 
	// Read the analog value from the smoke sensor
	int smokeSensorReading = analogRead(smokeInput);
	
	// Print Analog output from MQ2 onto serial
	Serial.print("Smoke Value: ");
	Serial.println(smokeSensorReading);


	// Check smoke sensor readout value and process accordingly 
	if (smokeSensorReading > MQ2_UPPER_THRESHOLD)
	{
		// Trigger sending of the ALERT SMS
		if (smsToBeSent == HIGH)
		{
			//Get the status of flame and temperature
			bool flameStatus = getFlameStatus();
			float temperatureReading = getTemperatureReading();

			// Call the function to send the SMS
			//sendAlertSMS(temperatureReading,flameStatus);
		
			// Set SMS variable to LOW, so SMS will not be repeated
			smsToBeSent = LOW;
		}

		// Raise urgent alarm with LED/Buzzer
		raiseHighAlarm();
	}
	else if (smokeSensorReading <= MQ2_UPPER_THRESHOLD && smokeSensorReading > MQ2_LOWER_THRESHOLD)
	{
		// Raise medium alarm with LED/buzzer
		raiseMediumAlarm();
	}
	else if (smokeSensorReading <= MQ2_LOWER_THRESHOLD)	
	{
		// Light up green LED
		digitalWrite(greenLed, HIGH);
		// Turn off the buzzer
		digitalWrite(buzzer,LOW);
	}
}
//=============================================================================//
