/****************************************************************
* Vac Switch.ino
*
* Author : Terence Ang
*
* Firmware for Auto Vacuum Switch.
*
* This code is Partsware; send me any useful unused parts if
* you've find my code helpful.
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

#define DEBUG

// the setup routine runs once when you press reset:
void setup() {

#ifdef DEBUG
	// initialize serial communication at 115200 bits per second:
	Serial.begin(115200);
#endif // DEBUG

	pinMode(SSR, OUTPUT);
	pinMode(modeSwitch, INPUT_PULLUP);
	digitalWrite(modeSwitch, HIGH);

	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);

	// check switch on boot up.. to prevent vacuum running on boot up.
	while (!digitalRead(modeSwitch))
	{
		digitalWrite(LED_BUILTIN, LOW);
		delay(1000);
		digitalWrite(LED_BUILTIN, HIGH);
		delay(1000);
		Serial.println("mode switch on manual on mode");
	}
}

uint16_t ctValue = 0;

// the loop routine runs over and over again forever:
void loop() {

	//check switch in Auto mode - High by pull up
	if (digitalRead(modeSwitch)) {

		//read the sensor value.
		ctValue = analogRead(A0);

		//Check CT for current flow
		if (ctValue > THR) {

			//abitary number, got it by testing against actual value.
			currentValue += 5; 

			// set max values to prevent overflow.
			if (currentValue >= onTHR) currentValue = onTHR * 3; 
		}
		else {
			currentValue--;

			//set min value to prevent underflow.
			if (currentValue <= 0) currentValue = 0; 
		}

#ifdef DEBUG
		Serial.print(ctValue);
		Serial.print(" : ");
		Serial.println(currentValue);
#endif

		if (currentValue > onTHR) {
			if (!ssrState) {
				currentDLY = millis();
				ssrState = true;
			}

			//turn on SSR after on delay to prevent inrush current from tripping the circuit.
			if ((currentDLY + onDLY) < millis()) digitalWrite(SSR, HIGH); 
		}
		else
		{
			if (ssrState) {
				currentDLY = millis();
				ssrState = false;
			}

			//turn off SSR after off delay to clean up left over debri while the bandsaw is powering down.
			if ((currentDLY + offDLY) < millis()) digitalWrite(SSR, LOW);
		}
	}
	else {

		//turn on SSR if mode switch is in manual mode = low.
		digitalWrite(SSR, HIGH); 

	}

}


