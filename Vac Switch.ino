/****************************************************************
* VacuumSwitch.ino
*
* Author : Terence Ang
*
* Firmware for Auto Vacuum Switch.
*
* This code is Partsware; send me any useful unsed parts if
* you've found our code helpful.
*
* Distributed as-is; no warranty is given.
***************************************************************/

#define SSR D1 //Pin for SSR.
#define modeSwitch D3 // Auto or manual mode - Auto = Low

#define THR 10 // ctValue Threshold
#define onDLY 500 // Turn On delay = 1sec
#define offDLY 5000 // Turn Off delay = 5sec

#define onTHR 100 // how long before switch is latched

int currentValue = 0;
int currentDLY = 0;
bool ssrState = false;

//#define debug

// the setup routine runs once when you press reset:
void setup() {

	// initialize serial communication at 115200 bits per second:
	Serial.begin(115200);

	pinMode(SSR, OUTPUT);
	pinMode(modeSwitch, INPUT_PULLUP);
	digitalWrite(modeSwitch, HIGH);
}

uint16_t ctValue = 0;
// the loop routine runs over and over again forever:
void loop() {

	if (digitalRead(modeSwitch)) {

		//check switch in Auto mode

		//read the sensor value.
		ctValue = analogRead(A0);

		//Check CT for current flow
		if (ctValue > THR) {
			currentValue += 5; //abitary number, got it by testing against actual value.
			if (currentValue >= onTHR) currentValue = onTHR * 3; // set max values to prevent overflow.
		}
		else {
			currentValue--;
			if (currentValue <= 0) currentValue = 0; //set min value to prevent underflow.
		}

#ifdef debug
		Serial.print(ctValue);
		Serial.print(" : ");
		Serial.println(currentValue);
#endif

		if (currentValue > onTHR) {
			if (!ssrState) {
				currentDLY = millis();
				ssrState = true;
			}
			if ((currentDLY + onDLY) < millis()) digitalWrite(SSR, HIGH); //turn on SSR after on delay to prevent inrush current from tripping the circuit.
		}
		else
		{
			if (ssrState) {
				currentDLY = millis();
				ssrState = false;
			}
			if ((currentDLY + offDLY) < millis()) digitalWrite(SSR, LOW); //turn off SSR after off delay to clean up left over debri while the bandsaw is powering down.
		}
	}
	else {

		digitalWrite(SSR, HIGH); //turn on SSR if mode switch is in manual mode = low.

	}


}


