//=============================================================================//
// Software for interfacing Arduino Uno with an ultrasonic sensor
// Based on distance, progressive visual alarm with LED(s) and
// audible alarm using active buzzer incorporated.
//
// The readout of distance from the ultrasonic module is displayed
// in realtime on a 4 digit 7 segment display.
//
// This code is in public domain.
// Author : Nithin Philip Kuruvila
// Date   : 23/06/2017
//=============================================================================//


//=============================================================================//
// Initialize pins for digits, segments, LED(s), buzzer and ultrasonic
//=============================================================================//

int digit1 = 10; 
int digit2 = 11;
int digit3 = 12;
int digit4 = 13;

int segA = 9; 
int segB = 3; 
int segC = 4; 
int segD = 5; 
int segE = 6; 
int segF = 8; 
int segG = 7; 

int LED1 = A2;
int LED2 = A3;
int LED3 = A4;
int LED4 = A5;

int BUZ = 2;

int ULTRA_TRIG = A1;
int ULTRA_ECHO = A0;

//=============================================================================//
// Define variables for sensing distance
//=============================================================================//
long echoDuration;
int distance;


//=============================================================================//
// Define ON/OFF states
//=============================================================================//
#define SEGMENT_ON  LOW			// NOTE : Polarity may be need to be reversed based
#define SEGMENT_OFF HIGH		// on common cathode/anode nature of the display.

#define DIGIT_ON HIGH
#define DIGIT_OFF LOW
#define LED_ON HIGH
#define LED_OFF LOW
#define BUZZER_ON HIGH
#define BUZZER_OFF LOW
#define TRIGGER_ON HIGH
#define TRIGGER_OFF LOW
#define ECHO_ON HIGH


void setup() 
{                
	// Set segment pins as output
	pinMode(segA, OUTPUT);
	pinMode(segB, OUTPUT);
	pinMode(segC, OUTPUT);
	pinMode(segD, OUTPUT);
	pinMode(segE, OUTPUT);
	pinMode(segF, OUTPUT);
	pinMode(segG, OUTPUT);

	// Set digit pins as output
	pinMode(digit1, OUTPUT);
	pinMode(digit2, OUTPUT);
	pinMode(digit3, OUTPUT);
	pinMode(digit4, OUTPUT);

	// Set LED pins as output
	pinMode(LED1, OUTPUT);
	pinMode(LED2, OUTPUT);
	pinMode(LED3, OUTPUT);
	pinMode(LED4, OUTPUT);

	// Set buzzer pin as output
	pinMode(BUZ, OUTPUT);

	// Set the ultrasonic trigger as output
	pinMode(ULTRA_TRIG,OUTPUT);

	// Set the ultrasonic echo as input
	pinMode(ULTRA_ECHO,INPUT);
}


//=============================================================================//
// The following function turns the segments for the passed number ON
// If '10' is passed, all segments are turned OFF
//=============================================================================//
void displaySegments(int numberToDisplay) 
{


	switch (numberToDisplay)
	{

		case 0:
			digitalWrite(segA, SEGMENT_ON);
			digitalWrite(segB, SEGMENT_ON);
			digitalWrite(segC, SEGMENT_ON);
			digitalWrite(segD, SEGMENT_ON);
			digitalWrite(segE, SEGMENT_ON);
			digitalWrite(segF, SEGMENT_ON);
			digitalWrite(segG, SEGMENT_OFF);
			break;

		case 1:
			digitalWrite(segA, SEGMENT_OFF);
			digitalWrite(segB, SEGMENT_ON);
			digitalWrite(segC, SEGMENT_ON);
			digitalWrite(segD, SEGMENT_OFF);
			digitalWrite(segE, SEGMENT_OFF);
			digitalWrite(segF, SEGMENT_OFF);
			digitalWrite(segG, SEGMENT_OFF);
			break;

		case 2:
			digitalWrite(segA, SEGMENT_ON);
			digitalWrite(segB, SEGMENT_ON);
			digitalWrite(segC, SEGMENT_OFF);
			digitalWrite(segD, SEGMENT_ON);
			digitalWrite(segE, SEGMENT_ON);
			digitalWrite(segF, SEGMENT_OFF);
			digitalWrite(segG, SEGMENT_ON);
			break;

		case 3:
			digitalWrite(segA, SEGMENT_ON);
			digitalWrite(segB, SEGMENT_ON);
			digitalWrite(segC, SEGMENT_ON);
			digitalWrite(segD, SEGMENT_ON);
			digitalWrite(segE, SEGMENT_OFF);
			digitalWrite(segF, SEGMENT_OFF);
			digitalWrite(segG, SEGMENT_ON);
			break;

		case 4:
			digitalWrite(segA, SEGMENT_OFF);
			digitalWrite(segB, SEGMENT_ON);
			digitalWrite(segC, SEGMENT_ON);
			digitalWrite(segD, SEGMENT_OFF);
			digitalWrite(segE, SEGMENT_OFF);
			digitalWrite(segF, SEGMENT_ON);
			digitalWrite(segG, SEGMENT_ON);
			break;

		case 5:
			digitalWrite(segA, SEGMENT_ON);
			digitalWrite(segB, SEGMENT_OFF);
			digitalWrite(segC, SEGMENT_ON);
			digitalWrite(segD, SEGMENT_ON);
			digitalWrite(segE, SEGMENT_OFF);
			digitalWrite(segF, SEGMENT_ON);
			digitalWrite(segG, SEGMENT_ON);
			break;

		case 6:
			digitalWrite(segA, SEGMENT_ON);
			digitalWrite(segB, SEGMENT_OFF);
			digitalWrite(segC, SEGMENT_ON);
			digitalWrite(segD, SEGMENT_ON);
			digitalWrite(segE, SEGMENT_ON);
			digitalWrite(segF, SEGMENT_ON);
			digitalWrite(segG, SEGMENT_ON);
			break;

		case 7:
			digitalWrite(segA, SEGMENT_ON);
			digitalWrite(segB, SEGMENT_ON);
			digitalWrite(segC, SEGMENT_ON);
			digitalWrite(segD, SEGMENT_OFF);
			digitalWrite(segE, SEGMENT_OFF);
			digitalWrite(segF, SEGMENT_OFF);
			digitalWrite(segG, SEGMENT_OFF);
			break;

		case 8:
			digitalWrite(segA, SEGMENT_ON);
			digitalWrite(segB, SEGMENT_ON);
			digitalWrite(segC, SEGMENT_ON);
			digitalWrite(segD, SEGMENT_ON);
			digitalWrite(segE, SEGMENT_ON);
			digitalWrite(segF, SEGMENT_ON);
			digitalWrite(segG, SEGMENT_ON);
			break;

		case 9:
			digitalWrite(segA, SEGMENT_ON);
			digitalWrite(segB, SEGMENT_ON);
			digitalWrite(segC, SEGMENT_ON);
			digitalWrite(segD, SEGMENT_ON);
			digitalWrite(segE, SEGMENT_OFF);
			digitalWrite(segF, SEGMENT_ON);
			digitalWrite(segG, SEGMENT_ON);
			break;

		case 10:
			digitalWrite(segA, SEGMENT_OFF);
			digitalWrite(segB, SEGMENT_OFF);
			digitalWrite(segC, SEGMENT_OFF);
			digitalWrite(segD, SEGMENT_OFF);
			digitalWrite(segE, SEGMENT_OFF);
			digitalWrite(segF, SEGMENT_OFF);
			digitalWrite(segG, SEGMENT_OFF);
			break;
	}
}

//=============================================================================//
// Following function is used to display a number on the 7 segment display
// and to control the brightness of the display by changing the duty cycle
// of the pulse at the segments.
//
// LED and buzzer are alarms are progressively triggered as a function of
// measured distance
//=============================================================================//

void displayNumber(int toDisplay) 
{

#define DISPLAY_BRIGHTNESS 5000			// Time in microseconds for which the pulse width will be HIGH

#define BUZZER_ALARM_LEVEL1 500			// Time in microseconds for which the pulse width of the buzzer
#define BUZZER_ALARM_LEVEL2 2000		// tone will be HIGH. The longer the pulse width, the higher
#define BUZZER_ALARM_LEVEL3 5000		// the perceived frequency of the buzzer tone

#define DISPLAY_WAIT_DURATION 20		// Time in milliseconds after which we will display again

	long beginTime = millis();

	for(int digit = 4 ; digit > 0 ; digit--) 
	{

		//Turn on a digit for a short amount of time
		switch(digit) 
		{
			case 1:
				digitalWrite(digit1, DIGIT_ON);
				break;
			case 2:
				digitalWrite(digit2, DIGIT_ON);
				break;
			case 3:
				digitalWrite(digit3, DIGIT_ON);
				break;
			case 4:
				digitalWrite(digit4, DIGIT_ON);
				break;
		}

		//Turn on the segments for this digit
		// Extract the last least significant digit and pass
		displaySegments(toDisplay % 10);
		
		// Divide by ten for next pass
		toDisplay /= 10;

		// Light up the LED(s) and adjust buzzer tone based on the distance
		if (distance >= 45 && distance < 60)
		{
			digitalWrite(LED1,LED_ON);
			digitalWrite(BUZ,BUZZER_ON);
			delayMicroseconds(BUZZER_ALARM_LEVEL1);
			digitalWrite(BUZ,BUZZER_OFF);

		}
		else if (distance < 45 && distance >= 30)
		{
			digitalWrite(LED1,LED_ON);
			digitalWrite(LED2,LED_ON);
			digitalWrite(BUZ,BUZZER_ON);
			delayMicroseconds(BUZZER_ALARM_LEVEL2);
			digitalWrite(BUZ,BUZZER_OFF);
		}
		else if (distance < 30 && distance >= 15)
		{
			digitalWrite(LED1,LED_ON);
			digitalWrite(LED2,LED_ON);
			digitalWrite(LED3,LED_ON);
			digitalWrite(BUZ,BUZZER_ON);
			delayMicroseconds(BUZZER_ALARM_LEVEL3);
			digitalWrite(BUZ,BUZZER_OFF);
		}
		else if (distance < 15)
		{
			digitalWrite(LED1,LED_ON);
			digitalWrite(LED2,LED_ON);
			digitalWrite(LED3,LED_ON);
			digitalWrite(LED4,LED_ON);
			digitalWrite(BUZ,BUZZER_ON);		// Buzzer won't be turned OFF here so as to
								// create a continuous tone
		}

		// The below delay determines how we modulate the pulse width.
		// This affects the percieved brightness of the dislay
		delayMicroseconds(DISPLAY_BRIGHTNESS); 

		//Turn off all segments
		displaySegments(10); 

		//Turn off all digits
		digitalWrite(digit1, DIGIT_OFF);
		digitalWrite(digit2, DIGIT_OFF);
		digitalWrite(digit3, DIGIT_OFF);
		digitalWrite(digit4, DIGIT_OFF);

		// Turn off buzzer and LED(s)
		digitalWrite(BUZ,BUZZER_OFF);
		digitalWrite(LED1,LED_OFF);
		digitalWrite(LED2,LED_OFF);
		digitalWrite(LED3,LED_OFF);
		digitalWrite(LED4,LED_OFF);
	}

	// Wait for 20ms before displaying again
	while( (millis() - beginTime) < DISPLAY_WAIT_DURATION) ; 
}

void loop() 
{
	// Clear the trigger pin
	digitalWrite(ULTRA_TRIG, BUZZER_OFF);
	delayMicroseconds(2);

	// Set the trigger to high for ten microseconds to trigger ultrasonic pulse
	digitalWrite(ULTRA_TRIG, TRIGGER_ON);
	delayMicroseconds(10);
	digitalWrite(ULTRA_TRIG, TRIGGER_OFF);

	// Read the echo pin to get distance estimate
	echoDuration = pulseIn(ULTRA_ECHO,ECHO_ON);

	// Calculate the distance. Assume speed of sound = 340m/s.
	distance = (echoDuration*0.034)/2;

	// Display the measured distance on the 7 segment display
	displayNumber(distance);
}
